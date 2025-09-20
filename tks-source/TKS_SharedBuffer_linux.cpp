/// TKS_SharedBuffer_linux.cpp
///
/// (c) 2005-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///
///

#include <string.h>
#include "tks.h"

// (note) must be defined or memory view will not be consistent across processes
//         applications should use peekI32() / pokeI32() to atomically read / write dwords
//         - read/write offset must be dword-aligned
//         - endianess conversion is not supported (make sure both reader and writer use the same buffer endianess)
#define USE_ATOMIC  defined

#if defined(YAC_LINUX) || defined(YAC_MACOS) || defined(YAC_QNX)

#include "PTN_Node.h"
#include "YAC_Buffer.h"
#include "TKS_SharedBuffer.h"

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>
#include <sys/mman.h>


void TKS_SharedBuffer::freeSharedMemory(void) {
   buffer -= TKS_SHAREDBUFFERHEADER_SIZE;
   ::munmap( (void*) buffer, (size_t) (*(sUI*)&buffer[TKS_SBH_SIZE]) );
   if(shmOwner)
   {
     YAC_String realFileName;
     realFileName.copy("/tmp/tks_mmap_");
     realFileName.append(&shmKey);
     ::unlink( (char*)realFileName.chars );
   }
   ::close( shmHandle.id );
}

sBool TKS_SharedBuffer::allocSharedMemoryByKey(YAC_String *_shmKey, sUI _size) {
   TKS_SharedBuffer::free();

   if(YAC_Is_String(_shmKey))
   {
      shmKey.yacCopy(_shmKey);
      YAC_String realFileName;
      realFileName.copy("/tmp/tks_mmap_");
      realFileName.append(&shmKey);

      int fdMapFile = ::open( (char*)realFileName.chars,
                              O_RDWR | O_CREAT
#if 0
#ifdef YAC_MACOS
                              | O_DSYNC
#endif // YAC_MACOS
#endif
                              ,
                              S_IRWXU
                              );

      if(-1 == fdMapFile)
      {
         Dprintf("[---] SharedBuffer::allocSharedMemory(size=%u): failed to open mmap file \"%s\". errno=%i (%s).\n",
                 _size,
                 (const char*)realFileName.chars,
                 errno,
                 strerror(errno)
                 );
         shmKey.free();
         return YAC_FALSE;
      }

      // Immediately unlink the file xxx unfortunately this trick only works when using fork() to create the child process
      ////::unlink( (char*) shmKey.chars );

      // Truncate the file to the desired shared memory size.
      {
         int r = ::ftruncate(fdMapFile, (_size + TKS_SHAREDBUFFERHEADER_SIZE) );
         (void)r;
      }

      // Now memory map the file
      buffer = (sU8*) ::mmap(NULL, (_size + TKS_SHAREDBUFFERHEADER_SIZE),
                             PROT_READ|PROT_WRITE, MAP_SHARED,
                             fdMapFile, 0
                             );

      if(MAP_FAILED == buffer)
      {
         Dprintf("[---] SharedBuffer::allocSharedMemory(size=%i): Could not mmap file. errno=%i (%s).\n",
                 _size,
                 errno,
                 strerror(errno)
                 );
         buffer = NULL;
         ::unlink( (char*) realFileName.chars );
         ::close(fdMapFile);
         shmKey.free();
         return YAC_FALSE;
      }

      *(sUI*)(buffer + TKS_SBH_SIZE) = _size;
      buffer += TKS_SHAREDBUFFERHEADER_SIZE;

      shmHandle.id = fdMapFile;
      shmOwner = 1;
      size = _size;
      return YAC_TRUE;
   }
   else
   {
      Dprintf("[---] SharedBuffer::allocSharedMemory(): _name is not a String.\n");
      return YAC_FALSE;
   }
}

sBool TKS_SharedBuffer::visitSharedMemory (YAC_String *_shmKey, sUI _size) {
   TKS_SharedBuffer::free();

   if(YAC_Is_String(_shmKey))
   {
      shmKey.yacCopy(_shmKey);
      YAC_String realFileName;
      realFileName.copy("/tmp/tks_mmap_");
      realFileName.append(&shmKey);

      int fdMapFile = ::open( (char*)realFileName.chars, O_RDWR );

      if( fdMapFile == -1 )
      {
         Dprintf("[---] SharedBuffer::visitSharedMemory(size=%i): failed to open mmap file \"%s\". errno=%i (%s).\n",
                 _size,
                 (const char*)realFileName.chars,
                 errno,
                 strerror(errno)
                 );
         shmKey.free();
         return YAC_FALSE;
      }

      // Memory map the file
      buffer = (sU8*) ::mmap(NULL, (_size + TKS_SHAREDBUFFERHEADER_SIZE),
                             PROT_READ|PROT_WRITE, MAP_SHARED,
                             fdMapFile, 0
                             );

      if(buffer == (sU8*)-1)
      {
         Dprintf("[---] SharedBuffer::visitSharedMemory(size=%i): failed to mmap file. errno=%i (%s).\n",
                 _size,
                 errno,
                 strerror(errno)
                 );
         buffer = NULL;
         ::close(fdMapFile);
         shmKey.free();
         return YAC_FALSE;
      }

      // Check whether the requested size does not exceed the actual shared memory size (should never happen)
      sUI shmSize = *(sUI*) (buffer + TKS_SBH_SIZE);

      if(_size > shmSize)
      {
         Dprintf("[---] SharedBuffer::visitSharedMemory(name=%s, size=%i): Requested size exceeds size of shared memory region (%u).\n",
                 (char*)shmKey.chars,
                 _size,
                 shmSize
                 );

#ifdef YAC_LINUX
         ::munmap((caddr_t) buffer, (size_t) (_size + TKS_SHAREDBUFFERHEADER_SIZE) );
#else
         ::munmap((void*) buffer, (size_t) (_size + TKS_SHAREDBUFFERHEADER_SIZE) );
#endif // YAC_LINUX
         ::close( fdMapFile );
         shmKey.free();
         buffer = NULL;
         return YAC_FALSE;
      }

      buffer += TKS_SHAREDBUFFERHEADER_SIZE;
      shmHandle.id = fdMapFile;
      shmOwner = YAC_FALSE;
      size = _size;
      return YAC_TRUE;
   }
   else
   {
      Dprintf("[---] SharedBuffer::visitSharedMemory(): _name is not a String.\n");
      return YAC_FALSE;
   }
}

sBool TKS_SharedBuffer::lock(sF32 _timeoutMS) {
   if(shmHandle.id)
   {
      if(!b_locked)
      {
         if(0 == flock(shmHandle.id, LOCK_EX | LOCK_NB))
         {
            b_locked = YAC_TRUE;
         }
         else if(_timeoutMS > 0u)
         {
            sF64 tStart = TKS_ScriptEngine::GetMilliSecondsDouble();
            sF64 tEnd   = tStart + _timeoutMS;
            for(;;)
            {
               if(0 == flock(shmHandle.id, LOCK_EX | LOCK_NB))
               {
                  b_locked = YAC_TRUE;
                  break;
               }
               sF64 tCur = TKS_ScriptEngine::GetMilliSecondsDouble();
               if(tCur >= tEnd)
               {
                  break;
               }
               // tkscript->usleep(10u);
            }
         }
      }
   }
   return b_locked;
}

void TKS_SharedBuffer::unlock(void) {
   if(shmHandle.id)
   {
      if(b_locked)
      {
         (void)flock(shmHandle.id, LOCK_UN);
         b_locked = YAC_FALSE;
      }
   }
}

sBool TKS_SharedBuffer::isOpen(void) {
   return (0 != shmHandle.id);
}


#endif // YAC_LINUX || YAC_MACOS || YAC_QNX
