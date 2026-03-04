/*
Copyright (c) 2018 bsp

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#define YAC_BIGSTRING
#define YAC_PRINTF
#include <yac.h>
#include "tkbluetooth.h"
#include "ying_tkbluetooth.h"

#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

// #define Dprintf if(0);else printf
#define Dprintf if(1);else printf


BlueToothClient::BlueToothClient(void) {
}

BlueToothClient::~BlueToothClient() {
   close();
}

void BlueToothClient::scanTest(void) {

   // http://mapidev.blogspot.com/2009/11/bluetooth-device-discovery-in-c.html
   int devId = ::hci_get_route(NULL);
   int dd = ::hci_open_dev(devId);
   if(dd > 0)
   {
      inquiry_info ii[256];
      inquiry_info *iip = ii;
      int numRSP = ::hci_inquiry(devId,
                                 28/*len*1.28sec*/,
                                 256/*max_rsp*/,
                                 NULL/*lap (type of inquiry)*/, 
                                 &iip,
                                 IREQ_CACHE_FLUSH/*flags*/
                                 );

      yac_host->printf("[dbg] BluetoothClient::scanTest: numRSP=%d\n", numRSP);

      if(numRSP > 0)
      {
         char name[248] = { 0 };
         char addr[19] = { 0 };

         for(int i = 0; i < numRSP; i++)
         {
            ba2str(&(ii+i)->bdaddr, addr); // convert bdaddr to a string
            memset(name, 0, sizeof(name)); // memset to store friendly name

            if(0 != ::hci_read_remote_name(dd, &(ii+i)->bdaddr, sizeof(name), name, 0)) // store in name the first sizeof(name) byte of the friendly name of the given addr.
            {
               // this mean the friendly name was not found
               strcpy(name, "[unknown]");
            }

            yac_host->printf("[...] BlueToothClient::scanTest: dev[%i] addr=%s name=\"%s\"\n", i, addr, name);
         }
      }
      else
      {
         yac_host->printf("[---] BlueToothClient::scanTest: hci_inquiry() returned numRSP=%d\n", numRSP);
      }

      ::close(dd);
   }
   else
   {
      yac_host->printf("[---] BlueToothClient::scanTest: hci_open_dev() failed (dd=%d)\n", dd);
   }
}

void BlueToothClient::getLocalAddress(YAC_Value *_r) {
   _r->initNull();

   int devId = ::hci_get_route(NULL);
   // int dev_id = hci_devid( "01:23:45:67:89:AB" );
   int dd = ::hci_open_dev(devId);

   if(dd > 0)
   {
      bdaddr_t addr;
      if(0 == ::hci_read_bd_addr(dd, &addr, 0/*to*/))
      {
         BlueToothAddress *retAddr = YAC_NEW(BlueToothAddress);
         retAddr->bytes[0] = addr.b[5];
         retAddr->bytes[1] = addr.b[4];
         retAddr->bytes[2] = addr.b[3];
         retAddr->bytes[3] = addr.b[2];
         retAddr->bytes[4] = addr.b[1];
         retAddr->bytes[5] = addr.b[0];
         _r->initObject(retAddr, 1);
         // yac_host->printf("xxx hci_read_bd_addr: addr=
      }

      ::close(dd);
   }
}

void BlueToothClient::getLocalName(YAC_Value *_r) {
   _r->initNull();

   int devId = ::hci_get_route(NULL);
   // int dev_id = hci_devid( "01:23:45:67:89:AB" );
   int dd = ::hci_open_dev(devId);

   if(dd > 0)
   {
      char name[248] = { 0 };
      
      if(0 == ::hci_read_local_name(dd, 248, name, 0/*to*/))
      {
         _r->initEmptyString();
         _r->value.string_val->copy(name);
      }

      ::close(dd);
   }
}

void BlueToothClient::findAddrByName(YAC_String *_remoteName, YAC_Value *_r) {
   _r->initNull();

   if(YAC_Is_String(_remoteName))
   {
      if(NULL != _remoteName->chars)
      {
         int devId = ::hci_get_route(NULL);
         int dd = ::hci_open_dev(devId);
         if(dd > 0)
         {
            inquiry_info ii[256];
            inquiry_info *iip = ii;
            int numRSP = ::hci_inquiry(devId,
                                       8/*len*1.28sec*/,
                                       256/*max_rsp*/,
                                       NULL/*lap (type of inquiry)*/, 
                                       &iip,
                                       0/*flags*/
                                       );
            if(numRSP > 0)
            {
               char name[248] = { 0 };
               char addr[19] = { 0 };

               for(int i = 0; i < numRSP; i++)
               {
                  ba2str(&(ii+i)->bdaddr, addr); // convert bdaddr to a string
                  memset(name, 0, sizeof(name)); // memset to store friendly name

                  if(0 == ::hci_read_remote_name(dd, &(ii+i)->bdaddr, sizeof(name), name, 0))
                  {
                     if(!strncmp((const char*)_remoteName->chars, name, size_t(248)))
                     {
                        _r->initEmptyString();
                        _r->value.string_val->copy(name);
                        break;
                     }
                  }
                  
                  yac_host->printf("[...] BlueToothClient::findAddrByName: dev[%i] addr=%s name=\"%s\"\n", i, addr, name);
               }
            }
            else
            {
               yac_host->printf("[---] BlueToothClient::findAddrByName: hci_inquiry() returned numRSP=%d\n", numRSP);
            }

            ::close(dd);
         }
         else
         {
            yac_host->printf("[---] BlueToothClient::findAddrByName: hci_open_dev() failed (dd=%d)\n", dd);
         }
      }
   }
}

sBool BlueToothClient::connect(BlueToothAddress *_addr, sUI _portNr) {
   sBool ret = YAC_FALSE;
   return ret;
}

sSI BlueToothClient::send(YAC_Object *_buffer) {
   sSI numBytes = 0;
   return numBytes;
}

sSI BlueToothClient::recv(YAC_Object *_buffer) {
   sSI numBytes = 0;
   return numBytes;
}

void BlueToothClient::close(void) {
}



// Process::Process(void) {
//    pid = -1;
// }

// Process::~Process() {
//    kill();
// }

// sUI Process::getNumAvail(sUI _timeoutMillisec) {
//    sUI r = 0u;

//    if(-1 != pid)
//    {
//       if(_timeoutMillisec > 0u)
//       {
//          int highestFDPlus1 = fd2[0] + 1;

//          fd_set readFDS; FD_ZERO(&readFDS); FD_SET(fd2[0], &readFDS);
         
//          struct timeval tv;
         
//          tv.tv_sec = _timeoutMillisec / 1000u;
//          tv.tv_usec = _timeoutMillisec % 1000u;
         
//          int err = select(highestFDPlus1, &readFDS, NULL/*writeFDS*/, NULL/*exceptFDS*/, &tv/*timeout*/);
//          (void)err;
//       }

//       {
//          size_t numAvail = 0;
//          // <http://man7.org/linux/man-pages/man2/ioctl.2.html>
//          int err = ::ioctl(fd2[0], FIONREAD, &numAvail);
//          if(0 == err)
//          {
//             r = sUI(numAvail);
//          }
//       }
//    }

//    return r;
// }

// sBool Process::exec(YAC_String *_cmd, YAC_Object *_args) {
//    sBool r = YAC_FALSE;

//    if(YAC_Is_String(_cmd))
//    {
//       if(NULL != _cmd->chars)
//       {
//          int err;

//          // <http://man7.org/linux/man-pages/man2/pipe.2.html>
//          err = pipe(fd1);

//          if(err >= 0)
//          {
//             err = pipe(fd2);

//             if(err >= 0)
//             {
//                pid = fork();

//                if(0 == pid)
//                {
//                   // Child process

//                   // Close unused streams
//                   close(fd1[1]);
//                   close(fd2[0]);

// #if 1
//                   close(STDIN_FILENO);
//                   close(STDOUT_FILENO);
//                   // close(STDERR_FILENO);

//                   // <https://linux.die.net/man/2/dup2>

//                   // if(STDIN_FILENO == dup3(fd1[0], STDIN_FILENO, O_CLOEXEC))
//                   if(STDIN_FILENO == dup2(fd1[0], STDIN_FILENO))
//                   {
//                      // if(STDOUT_FILENO == dup3(fd2[1], STDOUT_FILENO, O_CLOEXEC))
//                      if(STDOUT_FILENO == dup2(fd2[1], STDOUT_FILENO))
//                      {
//                         if(1)//STDERR_FILENO == dup2(fd2[1], STDERR_FILENO))
//                         {
//                            char *args[2];
//                            args[0] = (char*)_cmd->chars;
//                            args[1] = NULL;

//                            if(YAC_Is_StringArray(_args))
//                            {
//                            }

//                            // const char *hello = "hello, world.";
//                            // write(STDOUT_FILENO, hello, strlen(hello));

//                            // <https://linux.die.net/man/3/execl>
//                            // (note) only returns if an error has occurred
//                            // Dprintf("[dbg] Process::exec: execv(cmd=\"%s\")\n", (const char*)_cmd->chars);
//                            execv((const char*)_cmd->chars, args);
//                         }
//                         else
//                         {
//                            yac_host->printf("[---] Process::exec<child>: dup2<stderr> failed. errno=%d\n", errno);
//                            close(fd2[1]);
//                         }
//                      }
//                      else
//                      {
//                         yac_host->printf("[---] Process::exec<child>: dup2<stdout> failed. errno=%d\n", errno);
//                         close(fd2[1]);
//                      }
//                   }
//                   else
//                   {
//                      yac_host->printf("[---] Process::exec<child>: dup2<stdin> failed. errno=%d\n", errno);
//                      close(fd1[0]);
//                   }
// #endif

//                }
//                else if(pid > 0)
//                {
//                   // Parent process
//                   Dprintf("[dbg] Process::exec: parent_pid=%d child_pid=%d\n", int(getpid()), int(pid));

//                   // Close unused streams
//                   close(fd1[0]);
//                   close(fd2[1]);

//                   r = YAC_TRUE;
//                }
//                else
//                {
//                   yac_host->printf("[---] Process::exec: fork() failed (err=%d)\n", (int)pid);
//                }
//             }
//             else
//             {
//                yac_host->printf("[---] Process::exec: failed to create pipe fd2 (err=%d).\n", err);
//             }
//          }
//          else
//          {
//             yac_host->printf("[---] Process::exec: failed to create pipe fd1 (err=%d).\n", err);
//          }
//       }
//    }

//    return r;
// }

// sSI Process::writeBuffer(YAC_Object *_buffer, sUI _off, sUI _num) {
//    sSI r = 0;

//    if(-1 != pid)
//    {
//       if(_num > 0u)
//       {
//          if(YAC_Is_Buffer(_buffer))
//          {
//             YAC_CAST_ARG(YAC_Buffer, buf, _buffer);

//             if(_off < buf->size)
//             {
//                if((_off + _num) <= buf->size)
//                {
//                   if((_off + _num) > _off)
//                   {
//                      r = ::write(fd1[1], buf->buffer + _off, _num);
//                   }
//                }
//             }
//          }
//       }
//    }

//    return r;
// }

// sSI Process::writeString(YAC_String *_s) {
//    sSI r = 0;

//    if(-1 != pid)
//    {
//       if(YAC_Is_String(_s))
//       {
//          if(NULL != _s->chars)
//          {
//             if(_s->length > 1)
//             {
//                r = ::write(fd1[1], _s->chars, _s->length - 1/*ASCIIZ*/);
//             }
//          }
//       }
//    }

//    return r;
// }

// sSI Process::readBuffer(YAC_Object *_buffer, sUI _off, sUI _max) {
//    sSI r = 0;

//    if(-1 != pid)
//    {
//       if(_max > 0u)
//       {
//          if(YAC_Is_Buffer(_buffer))
//          {
//             YAC_CAST_ARG(YAC_Buffer, buf, _buffer);

//             if(_off < buf->size)
//             {
//                if((_off + _max) <= buf->size)
//                {
//                   if((_off + _max) > _off)
//                   {
//                      r = ::read(fd2[0], buf->buffer + _off, _max);
//                   }
//                }
//             }
//          }
//       }
//    }

//    return r;
// }

// sSI Process::readString(YAC_String *_s, sUI _max) {
//    sSI r = 0;

//    if(-1 != pid)
//    {
//       if(YAC_Is_String(_s))
//       {
//          _s->yacArrayRealloc(_max+1, 0,0,0);
//          Dprintf("[dbg] Process::readString: fd2[0]=%d _s->buflen=%u\n", fd2[0], _s->buflen);

//          if(_s->buflen > _max)
//          {
//             // Dprintf("[dbg] Process::readString: max=%u\n", _max);

//             _s->chars[0] = 0;
//             r = ::read(fd2[0], _s->chars, _max);
//             Dprintf("[dbg] Process::readString: r=%d\n", r);

//             if(r >= 0)
//             {
//                _s->chars[r] = 0;
//             }
//             // Dprintf("xxx _s->chars=\"%s\"\n", _s->chars);

//             _s->key = YAC_LOSTKEY;
//             _s->fixLength();
//          }
//       }
//    }

//    return r;
// }

// void Process::kill(void) {
//    if(-1 != pid)
//    {
//       close(fd1[1]);
//       close(fd2[0]);

//       Dprintf("xxx Process::kill: pid=%d\n", pid);

//       int status;

//       // <https://linux.die.net/man/2/waitpid>
//       if(::waitpid(pid, &status, WNOHANG) != pid)
//       {
//          // <http://man7.org/linux/man-pages/man2/kill.2.html>
//          if(0 == ::kill(pid, SIGTERM))
//          {
//             sBool bKilled = YAC_FALSE;

//             for(int i = 0; i < 3; i++)
//             {
//                if(::waitpid(pid, &status, WNOHANG) == pid)
//                {
//                   bKilled = YAC_TRUE;
//                   break;
//                }

//                ::sleep(1);
//             }

//             // if(!bKilled)
//             // {
//             //    (void)::kill(pid, SIGKILL);
//             // }
//          }
//          else
//          {
//             yac_host->printf("[~~~] Process::kill: kill(pid=%d, SIGTERM) failed. errno=%d\n", pid, errno);
//          }
//       }

//       pid = -1;
//    }
// }
