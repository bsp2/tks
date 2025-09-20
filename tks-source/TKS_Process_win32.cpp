/// TKS_Process_win32.cpp
///
/// (c) 2018-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///
///

#include "tks.h"

#ifdef YAC_WIN32
#include <process.h>  // getpid()
#endif // YAC_WIN32

#include "YAC_Buffer.h"
#include "YAC_String.h"
#include "YAC_StringArray.h"
#include "TKS_Process.h"


TKS_Process::TKS_Process(void) {
   child_in_rd = NULL;
   child_in_wr = NULL;
   child_out_rd = NULL;
   child_out_wr = NULL;
   ::ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
}

TKS_Process::~TKS_Process() {
   if(NULL != child_in_rd)
   {
      ::CloseHandle(child_in_rd);
      child_in_rd = NULL;
   }

   if(NULL != child_in_wr)
   {
      ::CloseHandle(child_in_wr);
      child_in_wr = NULL;
   }

   if(NULL != child_out_rd)
   {
      ::CloseHandle(child_out_rd);
      child_out_rd = NULL;
   }

   if(NULL != child_out_wr)
   {
      ::CloseHandle(child_out_wr);
      child_out_wr = NULL;
   }

   kill();

   ::ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
}

sBool YAC_VCALL TKS_Process::yacIsStream(void) {
   return 1;
}

sUI YAC_VCALL TKS_Process::yacStreamGetSize(void) {
   return waitAvail(0u/*timeoutMillisec*/);
}

sU8 YAC_VCALL TKS_Process::yacStreamReadI8(void) {
   sU8 buf = 0u;

   if(NULL != piProcInfo.hProcess)
   {
      DWORD dwRead;
      if(::ReadFile(child_out_rd, &buf, 1, &dwRead, NULL))
      {
         if(dwRead != 1)
         {
            yac_host->printf("[---] TKS_Process::yacStreamReadI8: failed to read byte.\n");
         }
      }
   }

   return buf;
}

sSI YAC_VCALL TKS_Process::yacStreamReadString(YAC_String *_s, sUI _max) {
   sSI r = 0;

   if(NULL != piProcInfo.hProcess)
   {
      if(YAC_Is_String(_s))
      {
         (void)_s->yacArrayRealloc(_max+1, 0,0,0);

         if(_s->buflen > _max)
         {
            _s->chars[0] = 0;
            DWORD dwRead;
            if(::ReadFile(child_out_rd, _s->chars, _max, &dwRead, NULL))
            {
               r = sSI(dwRead);
            }

            if(r >= 0)
            {
               _s->chars[r] = 0;
            }

            _s->key = YAC_LOSTKEY;
            _s->fixLength();
         }
      }
   }

   return r;
}

sSI YAC_VCALL TKS_Process::yacStreamReadBuffer(YAC_Buffer *_b, sUI _off, sUI _max, sBool _resize) {
   sSI r = 0;

   if(NULL != piProcInfo.hProcess)
   {
      if(_max > 0u)
      {
         if(YAC_Is_Buffer(_b))
         {
            YAC_CAST_ARG(YAC_Buffer, buf, _b);

            if(_resize && ((_off + _max) > _b->size))
            {
               (void)_b->yacArrayRealloc((_off + _max), 0,0,0);
            }

            if(_off < buf->size)
            {
               if((_off + _max) <= buf->size)
               {
                  if((_off + _max) > _off)
                  {
                     DWORD dwRead;
                     if(::ReadFile(child_out_rd, buf->buffer + _off, _max, &dwRead, NULL))
                     {
                        r = sSI(dwRead);
                        _b->io_offset = _off + sUI(r);
                     }
                  }
               }
            }
         }
      }
   }

   return r;
}

sSI YAC_VCALL TKS_Process::yacStreamWrite(sU8 *_buf, sUI _num) {
   sSI r = 0;

   if(NULL != piProcInfo.hProcess)
   {
      if(_num > 0u)
      {
         if(NULL != _buf)
         {
            DWORD dwWritten;
            if(::WriteFile(child_in_wr, _buf, _num, &dwWritten, NULL))
            {
               r = sSI(dwWritten);
            }
         }
      }
   }

   return r;
}

void YAC_VCALL TKS_Process::yacStreamWriteI8(sU8 _value) {
   (void)yacStreamWrite(&_value, 1);
}

sBool TKS_Process::exec(YAC_String *_cmd, YAC_Object *_args) {
   sBool r = YAC_FALSE;

   if(YAC_Is_String(_cmd))
   {
      if(NULL != _cmd->chars)
      {
         // <https://docs.microsoft.com/en-us/windows/desktop/procthread/creating-a-child-process-with-redirected-input-and-output>
         SECURITY_ATTRIBUTES saAttr;

         // Set the bInheritHandle flag so pipe handles are inherited.
         saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
         saAttr.bInheritHandle = TRUE;
         saAttr.lpSecurityDescriptor = NULL;

         // Create a pipe for the child process's STDOUT.
         if(::CreatePipe(&child_out_rd, &child_out_wr, &saAttr, 0))
         {
            // Ensure the read handle to the pipe for STDOUT is not inherited.
            if(::SetHandleInformation(child_out_rd, HANDLE_FLAG_INHERIT, 0))
            {
               // Create a pipe for the child process's STDIN.
               if(::CreatePipe(&child_in_rd, &child_in_wr, &saAttr, 0))
               {
                  // Ensure the write handle to the pipe for STDIN is not inherited.
                  if(::SetHandleInformation(child_in_wr, HANDLE_FLAG_INHERIT, 0))
                  {
                     STARTUPINFO siStartInfo;
                     DWORD dwCreateFlags = 0;

                     // Set up members of the PROCESS_INFORMATION structure.
                     ::ZeroMemory( &piProcInfo, sizeof(PROCESS_INFORMATION) );

                     // Set up members of the STARTUPINFO structure.
                     // This structure specifies the STDIN and STDOUT handles for redirection.
                     ::ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );
                     siStartInfo.cb = sizeof(STARTUPINFO);

                     sBool bBG = (-1 != _cmd->_indexOfChar('&', 0));
                     if(!bBG)
                     {
                        siStartInfo.hStdError  = child_out_wr;
                        siStartInfo.hStdOutput = child_out_wr;
                        siStartInfo.hStdInput  = child_in_rd;
                        siStartInfo.dwFlags |= STARTF_USESTDHANDLES;  // causes UI apps to not start until parent is quit
                     }
                     else
                     {
                        siStartInfo.dwFlags |= STARTF_USESHOWWINDOW;
                        siStartInfo.wShowWindow = SW_SHOWDEFAULT;
                        dwCreateFlags |= CREATE_NEW_CONSOLE;
                     }

                     YAC_String *cmdLine = YAC_New_String();

                     cmdLine->append("\"");
                     cmdLine->append(_cmd);
                     if(bBG)
                        cmdLine->replace("&", "");
                     cmdLine->append("\"");

                     if(YAC_Is_StringArray(_args))
                     {
                        YAC_CAST_ARG(YAC_StringArray, args, _args);
                        if(args->num_elements > 0)
                        {
                           for(sUI i = 0; i < args->num_elements; i++)
                           {
                              cmdLine->append(" ");
                              cmdLine->append(&args->elements[i]);
                           }
                        }
                     }

                     // Create the child process.
                     BOOL bSuccess = ::CreateProcess(NULL,
                                                     (char*)cmdLine->chars,     // command line
                                                     NULL,          // process security attributes
                                                     NULL,          // primary thread security attributes
                                                     TRUE,          // handles are inherited
                                                     dwCreateFlags, // creation flags
                                                     NULL,          // use parent's environment
                                                     NULL,          // use parent's current directory
                                                     &siStartInfo,  // STARTUPINFO pointer
                                                     &piProcInfo    // receives PROCESS_INFORMATION
                                                     );

                     // If an error occurs, exit the application.
                     if(bSuccess)
                     {
                        // Close handles to the child process and its primary thread.
                        // Some applications might keep these handles to monitor the status
                        // of the child process, for example.
                        // // ::CloseHandle(piProcInfo.hProcess);
                        // // ::CloseHandle(piProcInfo.hThread);
                        r = YAC_TRUE;
                     }
                     else
                     {
                        yac_host->printf("[---] Process::exec: CreateProcess(cmdLine=\"%s\") failed. GetLastError()=%d\n", (const char*)cmdLine->chars, GetLastError());
                        ::CloseHandle(child_in_rd);
                        child_in_rd = NULL;
                        ::CloseHandle(child_in_wr);
                        child_in_wr = NULL;
                        ::CloseHandle(child_out_rd);
                        child_out_rd = NULL;
                        ::CloseHandle(child_out_wr);
                        child_out_wr = NULL;
                     }

                     YAC_DELETE_SAFE(cmdLine);
                  }
               }
            }
         }

      }
   }

   return r;
}

sUI TKS_Process::waitAvail(sUI _timeoutMillisec) {
   sUI r = 0u;

   DWORD t;

   // (note) Windows does not have select()
   if(_timeoutMillisec > 0u)
   {
      t = GetTickCount();
      t += _timeoutMillisec;
   }

   if(NULL != piProcInfo.hProcess)
   {
      for(;;)
      {
         DWORD dwTotalBytesAvail;
         if(::PeekNamedPipe(child_out_rd,
                            NULL/*lpBuffer*/,
                            0/*nBufferSize*/,
                            NULL/*lpBytesRead*/,
                            &dwTotalBytesAvail,
                            NULL/*lpBytesLeftThisMessage*/
                            )
            )
         {
            r = sSI(dwTotalBytesAvail);
            if(r > 0)
               break;
         }

         if(_timeoutMillisec > 0u)
         {
            if(::GetTickCount() > t)
            {
               break;
            }
            else
            {
               ::Sleep(1);
            }
         }
      }
   }

   return r;
}

void TKS_Process::kill(void) {
   if(NULL != piProcInfo.hProcess)
   {
      ::CloseHandle(piProcInfo.hProcess);
      ::CloseHandle(piProcInfo.hThread);
      ::ZeroMemory( &piProcInfo, sizeof(PROCESS_INFORMATION) );
   }
}

sSI TKS_Process::GetPID(void) {
#ifdef YAC_WIN32
   return sSI(_getpid());
#else
   return sSI(getpid());
#endif // YAC_WIN32
}

void TKS_Process::SendSignal(int _pid, int _signal) {
   (void)_pid;
   (void)_signal;
   // not supported
   // ::kill((pid_t)_pid, _signal);
}

void TKS_Process::SendSignal_SIGUSR1(int _pid) {
   (void)_pid;
   // not supported
}

void TKS_Process::WaitSignal_SIGUSR1(void) {
   // not supported
}
