/// TKS_Process_linux.cpp
///
/// (c) 2018-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///
///

#include <signal.h>

#include "tks.h"

#include "YAC_Buffer.h"
#include "YAC_String.h"
#include "YAC_StringArray.h"
#include "TKS_Process.h"


#undef Dprintf
// #define Dprintf if(0);else printf
#define Dprintf if(1);else printf


TKS_Process::TKS_Process(void) {
   pid = -1;
}

TKS_Process::~TKS_Process() {
   kill();
}

sBool YAC_VCALL TKS_Process::yacIsStream(void) {
   return 1;
}

sUI YAC_VCALL TKS_Process::yacStreamGetSize(void) {
   return waitAvail(0u/*timeoutMillisec*/);
}

sU8 YAC_VCALL TKS_Process::yacStreamReadI8(void) {
   sU8 buf = 0u;

   if(-1 != pid)
   {
      int numRead = ::read(fd2[0], &buf, 1);
      if(1 != numRead)
      {
         yac_host->printf("[---] TKS_Process::yacStreamReadI8: failed to read byte.\n");
      }
   }

   return buf;
}

sSI YAC_VCALL TKS_Process::yacStreamReadString(YAC_String *_s, sUI _max) {
   sSI r = 0;

   if(-1 != pid)
   {
      if(YAC_Is_String(_s))
      {
         _s->yacArrayRealloc(_max+1, 0,0,0);

         if(_s->buflen > _max)
         {
            _s->chars[0] = 0;
            int numRead = ::read(fd2[0], _s->chars, _max);
            if(numRead > 0)
            {
               r = sSI(numRead);
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

   if(-1 != pid)
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
                     int numRead = ::read(fd2[0], buf->buffer + _off, _max);
                     if(numRead > 0)
                     {
                        r = sSI(numRead);
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

   if(-1 != pid)
   {
      if(_num > 0u)
      {
         if(NULL != _buf)
         {
            int numWritten = ::write(fd1[1], _buf, _num);
            if(numWritten > 0)
            {
               r = sSI(numWritten);
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
         int err;

         // <http://man7.org/linux/man-pages/man2/pipe.2.html>
         err = pipe(fd1);

         if(err >= 0)
         {
            err = pipe(fd2);

            if(err >= 0)
            {
               pid = fork();

               if(0 == pid)
               {
                  // Child process

                  // Close unused streams
                  close(fd1[1]);
                  close(fd2[0]);

                  close(STDIN_FILENO);
                  close(STDOUT_FILENO);
                  // close(STDERR_FILENO);

                  // <https://linux.die.net/man/2/dup2>

                  // if(STDIN_FILENO == dup3(fd1[0], STDIN_FILENO, O_CLOEXEC))
                  if(STDIN_FILENO == dup2(fd1[0], STDIN_FILENO))
                  {
                     // if(STDOUT_FILENO == dup3(fd2[1], STDOUT_FILENO, O_CLOEXEC))
                     if(STDOUT_FILENO == dup2(fd2[1], STDOUT_FILENO))
                     {
                        if(1)//STDERR_FILENO == dup2(fd2[1], STDERR_FILENO))
                        {
                           YAC_CAST_ARG(YAC_StringArray, args, _args);
                           sUI numArgs = 0u;
                           if(YAC_Is_StringArray(args))
                           {
                              numArgs = args->num_elements;
                           }

                           char *cmdArgs[2 + numArgs];

                           cmdArgs[0] = (char*)_cmd->chars;
                           sUI argIdx = 1u;

                           if(numArgs > 0u)
                           {

                              for(sUI i = 0u; i< args->num_elements; i++)
                              {
                                 cmdArgs[argIdx++] = (char*)args->elements[i].chars;
                              }
                           }

                           cmdArgs[argIdx] = NULL;

                           // <https://linux.die.net/man/3/execl>
                           // (note) only returns if an error has occurred
                           // Dprintf("[dbg] Process::exec: execv(cmd=\"%s\")\n", (const char*)_cmd->chars);
                           execv((const char*)_cmd->chars, cmdArgs);
                           yac_host->printf("[---] Process::exec<child>: execv() failed. cmd=\"%s\" numArgs=%u errno=%d\n", (const char*)_cmd->chars, numArgs, errno);
                        }
                        else
                        {
                           yac_host->printf("[---] Process::exec<child>: dup2<stderr> failed. errno=%d\n", errno);
                           close(fd2[1]);
                        }
                     }
                     else
                     {
                        yac_host->printf("[---] Process::exec<child>: dup2<stdout> failed. errno=%d\n", errno);
                        close(fd2[1]);
                     }
                  }
                  else
                  {
                     yac_host->printf("[---] Process::exec<child>: dup2<stdin> failed. errno=%d\n", errno);
                     close(fd1[0]);
                  }

               }
               else if(pid > 0)
               {
                  // Parent process
                  Dprintf("[dbg] Process::exec: parent_pid=%d child_pid=%d\n", int(getpid()), int(pid));

                  // Close unused streams
                  close(fd1[0]);
                  close(fd2[1]);

                  r = YAC_TRUE;
               }
               else
               {
                  yac_host->printf("[---] Process::exec: fork() failed (err=%d)\n", (int)pid);
               }
            }
            else
            {
               yac_host->printf("[---] Process::exec: failed to create pipe fd2 (err=%d).\n", err);
            }
         }
         else
         {
            yac_host->printf("[---] Process::exec: failed to create pipe fd1 (err=%d).\n", err);
         }
      }
   }

   return r;
}

sUI TKS_Process::waitAvail(sUI _timeoutMillisec) {
   sUI r = 0u;

   if(-1 != pid)
   {
      if(_timeoutMillisec > 0u)
      {
         int highestFDPlus1 = fd2[0] + 1;

         fd_set readFDS; FD_ZERO(&readFDS); FD_SET(fd2[0], &readFDS);

         struct timeval tv;

         tv.tv_sec = _timeoutMillisec / 1000u;
         tv.tv_usec = _timeoutMillisec % 1000u;

         int err = select(highestFDPlus1, &readFDS, NULL/*writeFDS*/, NULL/*exceptFDS*/, &tv/*timeout*/);
         (void)err;
      }

      {
         size_t numAvail = 0;
         // <http://man7.org/linux/man-pages/man2/ioctl.2.html>
         int err = ::ioctl(fd2[0], FIONREAD, &numAvail);
         if(0 == err)
         {
            r = sUI(numAvail);
         }
      }
   }

   return r;
}

void TKS_Process::kill(void) {

   if(-1 != pid)
   {
      close(fd1[1]);
      close(fd2[0]);

      Dprintf("xxx Process::kill: pid=%d\n", pid);

      int status;

      // <https://linux.die.net/man/2/waitpid>
      if(::waitpid(pid, &status, WNOHANG) != pid)
      {
         // <http://man7.org/linux/man-pages/man2/kill.2.html>
         if(0 == ::kill(pid, SIGTERM))
         {
            sBool bKilled = YAC_FALSE;

            for(int i = 0; i < 3; i++)
            {
               if(::waitpid(pid, &status, WNOHANG) == pid)
               {
                  bKilled = YAC_TRUE;
                  break;
               }

               ::sleep(1);
            }

            if(!bKilled)
            {
               (void)::kill(pid, SIGKILL);
            }
         }
         else
         {
            yac_host->printf("[~~~] Process::kill: kill(pid=%d, SIGTERM) failed. errno=%d\n", pid, errno);
         }
      }

      pid = -1;
   }
}

sSI TKS_Process::GetPID(void) {
   return sSI(getpid());
}

void TKS_Process::SendSignal(int _pid, int _signal) {
   if(_pid > 0)
      ::kill((pid_t)_pid, _signal);
}

void TKS_Process::SendSignal_SIGUSR1(int _pid) {
   if(_pid > 0)
      ::kill((pid_t)_pid, SIGUSR1);
}

void TKS_Process::WaitSignal_SIGUSR1(void) {
   sigset_t set;
   sigemptyset(&set);
   // if(sigaddset(&set, SIGURG) >= 0)
   if(sigaddset(&set, SIGUSR1) >= 0)
   {
      int retSig = -1;
      // tkscript->printf("xxx call sigwait SIGUSR1\n");
      int r = sigwait(&set, &retSig);
      if(0 != r)
      {
         tkscript->printf("[~~~] TKS_Process::WaitSignal_SIGUSR1: error: r=%d\n", r);
      }
      // tkscript->printf("xxx sigwait SIGUSR1 returned\n");
   }
}
