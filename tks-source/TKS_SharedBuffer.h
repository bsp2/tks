/// TKS_SharedBuffer.h
///
/// (c) 2005-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///
#ifndef __TKS_SHAREDBUFFER_H__
#define __TKS_SHAREDBUFFER_H__

enum __TKS_SharedBufferHeader {
   TKS_SBH_SIZE = 0, // size of region excluding this header
      
   TKS_SHAREDBUFFERHEADER_SIZE = sizeof(sSI)
};

class TKS_SharedBuffer : public YAC_Buffer {
public:
   YAC_String shmKey;
   sBool      shmOwner;
   
   union {
      void *ptr;   // OS-dependent shared memory handle
      int   id;    // fdMapFile on Linux/macOS
   } shmHandle;

   sBool b_locked;

#ifdef YAC_WIN32
   void *hMutex;
#endif // YAC_WIN32

protected:
   // Internal methods:
   void genSharedMemoryKey(void);

public:
   TKS_SharedBuffer(void);
   ~TKS_SharedBuffer();

   // Exported methods (API):
   sBool       allocSharedMemory      (sUI _size);
   YAC_String *getSharedMemoryKey     (void);
   void        free                   (void);

   // Platform specific methods
   sBool       allocSharedMemoryByKey (YAC_String *_shmKey, sUI _size);
   sBool       visitSharedMemory      (YAC_String *_shmKey, sUI _size);
   void        freeSharedMemory       (void); // must only be called by free()
   sBool       lock                   (sF32 _timeoutMS);
   void        unlock                 (void);
   sBool       isOpen                 (void);
   sBool       isOwner                (void);

   // The following methods fail and issue a warning in this class
   sBool setSize    (sUI _numbytes); 
   sBool resize     (sUI _numbytes);

   sBool YAC_VCALL yacArrayAlloc  (sUI _sx, sUI, sUI, sUI);
   sBool YAC_VCALL yacArrayRealloc(sUI _sx, sUI _sy, sUI _type, sUI _elementbytesize);

   // overrides YAC_Buffer::pokeI32() (atomic write)
   void pokeI32 (sSI _offset, sSI _value);

   // overrides YAC_Buffer::peekI32() (atomic read)
   sSI  peekI32 (sSI _offset);

   // overrides YAC_Buffer::ringWriteBuffer() (volatile write)
   sUI ringWriteBuffer(sUI _ringStartOff, sUI _ringSz, sUI _writeOff, YAC_Buffer *_src, sUI _srcOff, sUI _numBytes);

   // overrides YAC_Buffer::ringReadBuffer() (volatile read)
   sUI ringReadBuffer(sUI _ringStartOff, sUI _ringSz, sUI _readOff, YAC_Buffer *_dst, sUI _dstOff, sUI _numBytes);
   
};


#endif
