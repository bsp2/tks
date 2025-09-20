/// TKS_SharedBuffer_win32.cpp
///
/// (c) 2005-2023 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///
///

#include <string.h>
#include "tks.h"

#ifdef YAC_WIN32

#include "PTN_Node.h"
#include "YAC_Buffer.h"
#include "TKS_SharedBuffer.h"

#include <windows.h>

#include <time.h>


void TKS_SharedBuffer::freeSharedMemory(void) {
   ::CloseHandle((HANDLE)hMutex);
   hMutex = NULL;
   ::UnmapViewOfFile(buffer - TKS_SHAREDBUFFERHEADER_SIZE);
   ::CloseHandle((HANDLE)shmHandle.ptr);
}

sBool TKS_SharedBuffer::allocSharedMemoryByKey(YAC_String *_shmKey, sUI _size) {
   TKS_SharedBuffer::free();

   if(YAC_Is_String(_shmKey))
   {
      shmKey.yacCopy(_shmKey);

      // adapted from http://msdn.microsoft.com/library/default.asp?url=/library/en-us/fileio/fs/creating_named_shared_memory.asp
      HANDLE hMapFile = ::CreateFileMapping(
         INVALID_HANDLE_VALUE,    // use paging file
         NULL,                    // default security
         PAGE_READWRITE,          // read/write access
         0,                       // max. object size
         _size + TKS_SHAREDBUFFERHEADER_SIZE, // buffer size
         (char*)shmKey.chars);    // name of mapping object

      if ( hMapFile == NULL || hMapFile == INVALID_HANDLE_VALUE)
      {
         Dprintf("[---] SharedBuffer::allocSharedMemory(size=%i): failed to create file mapping object (%d).\n",
            _size, GetLastError());
         shmKey.free();
         return 0;
      }
      buffer = (sU8*) MapViewOfFile(
         hMapFile,            // handle to map object
         FILE_MAP_ALL_ACCESS, // read/write permission
         0,                   // <dwFileOffsetHigh>
         0,                   // <dwFileOffsetLow>
         _size + TKS_SHAREDBUFFERHEADER_SIZE);

      if (buffer == NULL)
      {
         Dprintf("[---] SharedBuffer::allocSharedMemory(size=%i): failed to map view of file (%d).\n",
            _size, GetLastError());
         ::CloseHandle(hMapFile);
         shmKey.free();
         return YAC_FALSE;
      }

      *(sUI*)(buffer + TKS_SBH_SIZE) = _size;
      buffer += TKS_SHAREDBUFFERHEADER_SIZE;

      shmHandle.ptr = (void *) hMapFile;
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

sBool TKS_SharedBuffer::visitSharedMemory(YAC_String *_shmKey, sUI _size) {
   TKS_SharedBuffer::free();

   if(YAC_Is_String(_shmKey))
   {
      shmKey.yacCopy(_shmKey);

      // adapted from http://msdn.microsoft.com/library/default.asp?url=/library/en-us/fileio/fs/creating_named_shared_memory.asp
      HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS,   // read/write access
                                        FALSE,                 // do not inherit the name
                                        (char*)shmKey.chars    // name of mapping object
                                        );

      if (hMapFile == NULL)
      {
         Dprintf("[---] SharedBuffer::visitSharedMemory(name=%s, size=%i): failed to open file mapping object (%d).\n",
                 (char*)shmKey.chars,
                 _size,
                 GetLastError()
                 );
         shmKey.free();
         return YAC_FALSE;
      }

      buffer = (sU8*) ::MapViewOfFile(hMapFile,    // handle to mapping object
                                      FILE_MAP_ALL_ACCESS,  // read/write permission
                                      0,                    // <dwFileOffsetHigh>
                                      0,                    // <dwFileOffsetLow>
                                      _size + TKS_SHAREDBUFFERHEADER_SIZE
                                      );

      if(NULL == buffer)
      {
         Dprintf("[---] SharedBuffer::visitSharedMemory(name=%s, size=%i): failed to map view of file (%d).\n",
                 (char*)shmKey.chars,
                 _size,
                 GetLastError()
                 );
         ::CloseHandle(hMapFile);
         shmKey.free();
         return YAC_FALSE;
      }

      // Check whether the requested size does not exceed the actual shared memory size
      sUI shmSize = *(sUI*) (buffer + TKS_SBH_SIZE);

      if(((sUI)_size) > shmSize)
      {
         Dprintf("[---] SharedBuffer::visitSharedMemory(name=%s, size=%i): requested size exceeds size of shared memory region (%u).\n",
            (char*)shmKey.chars,
            _size,
            shmSize
            );
         ::UnmapViewOfFile(buffer);
         ::CloseHandle(hMapFile);
         shmKey.free();
         buffer = NULL;
         return YAC_FALSE;
      }

      buffer += TKS_SHAREDBUFFERHEADER_SIZE;
      shmHandle.ptr = hMapFile;
      shmOwner = YAC_FALSE;
      size = _size;

      hMutex = (void*)::OpenMutex(MUTEX_ALL_ACCESS,                // request full access
                                  FALSE,                           // handle not inheritable
                                  TEXT((const char*)shmKey.chars)  // object name
                                  );

      return YAC_TRUE;
   }
   else
   {
      Dprintf("[---] SharedBuffer::visitSharedMemory(): _name is not a String.\n");
      return YAC_FALSE;
   }
}

sBool TKS_SharedBuffer::lock(sF32 _timeoutMS) {
   if(shmHandle.ptr)
   {
      if(!b_locked)
      {
         // see <https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-waitforsingleobject>
         DWORD dwWaitResult = ::WaitForSingleObject((HANDLE)hMutex,    // handle to mutex
                                                    (DWORD)_timeoutMS  // time-out interval
                                                    );
         if(WAIT_OBJECT_0 == dwWaitResult)
         {
            b_locked = YAC_TRUE;
         }
      }
   }
   return b_locked;
}

void TKS_SharedBuffer::unlock(void) {
   if(shmHandle.ptr)
   {
      if(b_locked)
      {
         ::ReleaseMutex((HANDLE)hMutex);
         b_locked = YAC_FALSE;
      }
   }
}

sBool TKS_SharedBuffer::isOpen(void) {
   return (NULL != shmHandle.ptr);
}


#endif // YAC_WIN32
