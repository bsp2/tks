/// TKS_SharedBuffer.cpp
///
/// (c) 2005-2023 Bastian Spiegel <bs@tkscript.de> 
///     - distributed under terms of the GNU general public license (GPL).
///
///

#include <string.h>
#include "tks.h"
#include "PTN_Node.h"
#include "YAC_Buffer.h"
#include "TKS_SharedBuffer.h"

#ifdef YAC_WIN32
#include <windows.h> 
#endif

#include <time.h>


TKS_SharedBuffer::TKS_SharedBuffer(void) : YAC_Buffer() {
   class_ID = TKS_CLID_SHAREDBUFFER;
   shmOwner = YAC_FALSE;
   shmHandle.ptr = NULL;
   b_locked = YAC_FALSE;
#ifdef YAC_WIN32
   hMutex = NULL;
#endif // YAC_WIN32
}

TKS_SharedBuffer::~TKS_SharedBuffer() {
   TKS_SharedBuffer::free();
}

sBool TKS_SharedBuffer::isOwner(void) {
   return shmOwner;
}

void TKS_SharedBuffer::free(void) {
   if(shmHandle.ptr)
   {
      if(b_locked)
         unlock();
      freeSharedMemory();
      shmHandle.ptr = NULL;
      shmOwner      = YAC_FALSE;
      buffer        = NULL;
      io_offset     = 0;
      size          = 0;
      shmKey.empty();
   }
}

sBool TKS_SharedBuffer::allocSharedMemory(sUI _size) {
   TKS_SharedBuffer::free();
   
   genSharedMemoryKey();

#ifdef YAC_WIN32
   // see <https://learn.microsoft.com/en-us/windows/win32/sync/using-named-objects>
   hMutex = ::CreateMutex(NULL,                            // default security descriptor
                          FALSE,                           // mutex not owned
                          TEXT((const char*)shmKey.chars)  // object name
                          );
#endif // YAC_WIN32

   return allocSharedMemoryByKey(&shmKey, _size);
}

void TKS_SharedBuffer::genSharedMemoryKey(void) {
   time_t t; time(&t); 
   sUI rand_ms = (sUI)t;
   shmKey.alloc(18);
   shmKey.printf("%08x%08x", rand_ms, &rand_ms);
}

YAC_String *TKS_SharedBuffer::getSharedMemoryKey (void) {
   return &shmKey;
}

sBool TKS_SharedBuffer::setSize(sUI _size) {
   Dprintf("[---] warning: setSize(size=%u) called on class SharedBuffer.\n", _size);
   return YAC_FALSE;
}

sBool TKS_SharedBuffer::resize(sUI _size) {
   Dprintf("[---] warning: resize(size=%u) called on class SharedBuffer.\n", _size);
   return YAC_FALSE;
}

sBool YAC_VCALL TKS_SharedBuffer::yacArrayAlloc(sUI _sx, sUI _sy, sUI _type, sUI _elementByteSize) {
   Dprintf("[---] warning: yacArrayAlloc(sx=%i sy=%i type=%i elementByteSize=%i) called on SharedBuffer.\n", 
      _sx, _sy, _type, _elementByteSize);
   return YAC_FALSE;
}

sBool YAC_VCALL TKS_SharedBuffer::yacArrayRealloc(sUI _sx, sUI _sy, sUI _type, sUI _elementByteSize) {
   Dprintf("[---] warning: yacArrayRealloc(sx=%i sy=%i type=%i elementByteSize=%i) called on SharedBuffer.\n", 
      _sx, _sy, _type, _elementByteSize);
   return YAC_FALSE;
}

sUI TKS_SharedBuffer::ringWriteBuffer(sUI _ringStartOff, sUI _ringSz, sUI _writeOff, YAC_Buffer *_src, sUI _srcOff, sUI _numBytes) {
   sUI r = _writeOff;
   if(_numBytes > 0u)
   {
      if(0u == (_ringStartOff & 3u) && _ringStartOff < size)
      {
         if(_ringSz > 0u && 0u == (_ringSz & 3u) && _ringSz <= size && (_ringStartOff + _ringSz) <= size)
         {
            if(YAC_Is_Buffer(_src)) 
            { 
               if(_writeOff < _ringSz)
               {
                  if(_srcOff < _src->size && (_srcOff + _numBytes) <= _src->size)
                  {
                     sUI k = _srcOff;
                     sUI j = _writeOff;
                     const sU8 *s = _src->buffer;
                     volatile sS32 *d = (volatile sS32*)buffer;
                     const sUI ringStartOffDWords = _ringStartOff >> 2;

                     yacmem32 mDst; mDst.s32 = tkscript->yacAtomicGet(&d[ringStartOffDWords + (j >> 2)]);
                     sS32 dstShift;  // 0..24 (LE) or 24..0 (BE)
                     sS32 dstShiftStep;
                     sS32 dstShiftThres;
                     sS32 dstShiftReset;
                     if(YAC_LITTLEENDIAN == byteOrder)
                     {
                        dstShift = (j & 3u) << 3;  // 0 1 2 3
                        dstShiftStep  = 8;
                        dstShiftThres = 32;
                        dstShiftReset = 0;
                     }
                     else
                     {
                        dstShift = 24 - ((j & 3u) << 3);  // 3 2 1 0
                        dstShiftStep  = -8;
                        dstShiftThres = 0;
                        dstShiftReset = 24;
                     }

                     // tkscript->printf("xxx ~~~~~~~~ ringWriteBuffer: read initial dword=0x%08x @off=%u dstShift=%u j=%u\n", mDst.u32, (j>>2)<<2, dstShift, j);

                     for(sUI i = 0u; i < _numBytes; i++)
                     {
                        mDst.u32 = (mDst.u32 & ~(255u << dstShift)) | (s[k++] << dstShift);
                        dstShift += dstShiftStep;

                        if(dstShift == dstShiftThres)
                        {
                           // DWord complete, store and begin next one
                           tkscript->yacAtomicSet(&d[ringStartOffDWords + (j >> 2)], mDst.s32);
                           // tkscript->printf("xxx ringWriteBuffer: i=%u/%u store dword 0x%08x @off=%u j=%u\n", i, _numBytes, mDst.u32, (j>>2)<<2, j);
                           mDst.u32 = 0u;
                           dstShift = dstShiftReset;
                        }

                        if(++j >= _ringSz)
                           j = 0u;
                     }

                     if(0u != (j & 3u))
                     {
                        // Store incomplete last dword
                        //  - remaining dword bytes will be set to 0
                        //  - shared memory 'fields' / 'regions' should always start on a dword boundary
                        //     - (note) _could_ read dword and OR new bytes but not worth it since
                        //              there shouldn't be valid data anyway
                        // tkscript->printf("xxx ringWriteBuffer: store incomplete last dword 0x%08x @off=%u j=%u\n", mDst.u32, (j>>2)<<2, j);
                        tkscript->yacAtomicSet(&d[ringStartOffDWords + (j >> 2)], mDst.s32);
                     }

                     r = j;
                  }
                  else
                  {
                     tkscript->printf("[---] TKS_SharedBuffer::ringWriteBuffer: invalid srcOff(%u) (src->size=%u)\n", _srcOff, _src->size);
                  }
               }
               else
               {
                  tkscript->printf("[---] TKS_SharedBuffer::ringWriteBuffer: invalid writeOff(%u) (ringSz=%u)\n", _writeOff, _ringSz);
               }
            }
            else
            {
               tkscript->printf("[---] TKS_SharedBuffer::ringWriteBuffer: src@%p is not a Buffer\n", _src);
            }
         }
         else
         {
            tkscript->printf("[---] TKS_SharedBuffer::ringWriteBuffer: invalid ringSz(%u) (must be >0, dword-aligned, and fit in SHM_size=%u)\n", _ringSz, size);
         }
      }
      else
      {
         tkscript->printf("[---] TKS_SharedBuffer::ringWriteBuffer: invalid ringStartOff(%u) (must be dword-aligned and fit in SHM_size=%u)\n", _ringStartOff, _ringSz);
      }
   }
   else
   {
      // nothing to do, numBytes is 0
   }
   return r;
}

sUI TKS_SharedBuffer::ringReadBuffer(sUI _ringStartOff, sUI _ringSz, sUI _readOff, YAC_Buffer *_dst, sUI _dstOff, sUI _numBytes) {
   sUI r = _readOff;
   if(_numBytes > 0u)
   {
      if(0u == (_ringStartOff & 3u) && _ringStartOff < size)
      {
         if(_ringSz > 0u && 0u == (_ringSz & 3u) && _ringSz <= size && (_ringStartOff + _ringSz) <= size)
         {
            if(YAC_Is_Buffer(_dst)) 
            { 
               if(_readOff < _ringSz)
               {
                  if(_dstOff < _dst->size && (_dstOff + _numBytes) <= _dst->size)
                  {
                     sUI k = _dstOff;
                     sUI j = _readOff;
                     sU8 *d = _dst->buffer;
                     volatile sS32 *s = (volatile sS32*)buffer;
                     const sUI ringStartOffDWords = _ringStartOff >> 2;
                     const sUI ringSzDWords = _ringSz >> 2;

                     yacmem32 mSrc; mSrc.s32 = tkscript->yacAtomicGet(&s[ringStartOffDWords + (j >> 2)]);
                     // tkscript->printf("xxx ~~~~~~ ringReadBuffer: first mSrc.u32=0x%08x @off=%u j=%u\n", mSrc.u32, (j>>2)<<2, j);
                     sS32 srcShift;  // 0..24 (LE) or 24..0 (BE)
                     sS32 srcShiftStep;
                     sS32 srcShiftThres;
                     sS32 srcShiftReset;
                     if(YAC_LITTLEENDIAN == byteOrder)
                     {
                        srcShift = (_readOff & 3u) << 3;  // 0 1 2 3
                        srcShiftStep  = 8;
                        srcShiftThres = 32;
                        srcShiftReset = 0;
                     }
                     else
                     {
                        srcShift = 24 - ((_readOff & 3u) << 3);  // 3 2 1 0
                        srcShiftStep  = -8;
                        srcShiftThres = 0;
                        srcShiftReset = 24;
                     }

                     for(sUI i = 0u; i < _numBytes; i++)
                     {
                        // tkscript->printf("xxx readBuffer: i=%u/%u srcShift=%u(thres=%u) j=%u\n", i, _numBytes, srcShift, srcShiftThres, j);
                        if(srcShift == srcShiftThres)
                        {
                           // All bytes read from dword, get next one
                           sUI jNextDWord = (j >> 2);
                           if(jNextDWord >= ringSzDWords)
                              jNextDWord = 0u;
                           // tkscript->printf("xxx readBuffer:   read jNextDWord@off=%u j=%u\n", (jNextDWord << 2), j);
                           mSrc.s32 = tkscript->yacAtomicGet(&s[ringStartOffDWords + jNextDWord]);
                           srcShift = srcShiftReset;
                        }
                        d[k++] = (sU8)((mSrc.u32 >> srcShift) & 255u);
                        // tkscript->printf("xxx readBuffer:     c=%u('%c')\n", d[k-1], (char)((d[k-1] >= ' ')?d[k-1]:'?'));
                        srcShift += srcShiftStep;
                        if(++j >= _ringSz)
                           j = 0u;
                     }
                     r = j;
                  }
                  else
                  {
                     tkscript->printf("[---] TKS_SharedBuffer::ringReadBuffer: invalid dstOff(%u) (dst->size=%u)\n", _dstOff, _dst->size);
                  }
               }
               else
               {
                  tkscript->printf("[---] TKS_SharedBuffer::ringReadBuffer: invalid readOff(%u) (ringSz=%u)\n", _readOff, _ringSz);
               }
            }
            else
            {
               tkscript->printf("[---] TKS_SharedBuffer::ringReadBuffer: dst@%p is not a Buffer\n", _dst);
            }
         }
         else
         {
            tkscript->printf("[---] TKS_SharedBuffer::ringReadBuffer: invalid ringSz(%u) (must be >0, dword-aligned, and fit in SHM_size=%u)\n", _ringSz, size);
         }
      }
      else
      {
         tkscript->printf("[---] TKS_SharedBuffer::ringReadBuffer: invalid ringStartOff(%u) (must be dword-aligned and fit in SHM_size=%u)\n", _ringStartOff, _ringSz);
      }
   }
   else
   {
      // nothing to do, numBytes is 0
   }
   return r;
}

sSI TKS_SharedBuffer::peekI32(sSI _offset) {
   // (note) offset must be aligned to dword boundary
#ifdef USE_ATOMIC
   if(size > 3u)
   {
      if( (0 == (_offset & 3)) && sUI(_offset + 3u) < size )
      {
         // ::fdatasync(shmHandle.id);
         // ::fsync(shmHandle.id);  // slower
         return tkscript->yacAtomicGet( (volatile sS32*)&buffer[_offset] );
      }
      else
         tkscript->printf("[---] TKS_SharedBuffer::peekI32: unaligned offset(%d) or out of bounds (size=%u)\n", _offset, size);
   }
   return 0u;
#else
   return YAC_Buffer::peekI32(_offset);
#endif // USE_ATOMIC
}

void TKS_SharedBuffer::pokeI32(sSI _offset, sSI _value) {
   // (note) offset must be aligned to dword boundary
#ifdef USE_ATOMIC
   if(size > 3u)
   {
      if( (0 == (_offset & 3)) && sUI(_offset + 3u) < size )
      {
         tkscript->yacAtomicSet((volatile sS32*)&buffer[_offset], _value);
         //::fdatasync(shmHandle.id);
         // ::fsync(shmHandle.id);  // slower
      }
      else
         tkscript->printf("[---] TKS_SharedBuffer::peekI32: unaligned offset(%d) or out of bounds (size=%u)\n", _offset, size);
   }
#else
   YAC_Buffer::pokeI32(_offset, _value);
#endif // USE_ATOMIC
}
