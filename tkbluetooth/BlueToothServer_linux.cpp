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

#include <stdio.h>

#define YAC_BIGSTRING
#define YAC_PRINTF
#include <yac.h>
#include "tkbluetooth.h"
#include "ying_tkbluetooth.h"


BlueToothServer::BlueToothServer(void) {
   server_socket = -1;
   client_socket = -1;
}

BlueToothServer::~BlueToothServer() {
   close();
}

sBool BlueToothServer::create(sUI _portNr) {
   struct sockaddr_rc serverAddr = { 0 };

   // Create server socket
   server_socket = ::socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

   // <portNr> must be in the range 1..30
   serverAddr.rc_family = AF_BLUETOOTH;
   bdaddr_t bdaddrAny = { 0 };
   serverAddr.rc_bdaddr = bdaddrAny;////*BDADDR_ANY;
   serverAddr.rc_channel = (uint8_t) _portNr;
   ::bind(server_socket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

   // Listen for connections on the server socket
   ::listen(server_socket, 1/*backlog*/);

   return YAC_TRUE;
}

void BlueToothServer::close(void) {
   if(-1 != server_socket)
   {
      ::close(server_socket);
      server_socket = -1;
   }

   if(-1 != client_socket)
   {
      ::close(client_socket);
      client_socket = -1;
   }
}

void BlueToothServer::closeClient(void) {
   if(-1 != client_socket)
   {
      ::close(client_socket);
      client_socket = -1;
   }
}

sBool BlueToothServer::waitForSocket(int _socket, sUI _timeoutMillisec) {
   sBool ret = YAC_FALSE;

   if(-1 != _socket)
   {
      int highestFDPlus1 = _socket + 1;

      fd_set readFDS; FD_ZERO(&readFDS); FD_SET(_socket, &readFDS);

      struct timeval tv;

      tv.tv_sec = _timeoutMillisec / 1000u;
      tv.tv_usec = _timeoutMillisec % 1000u;

      int err = select(highestFDPlus1, &readFDS, NULL/*writeFDS*/, NULL/*exceptFDS*/, &tv/*timeout*/);

      if(err > 0)
      {
         ret = YAC_TRUE;
      }
   }

   return ret;
}

sBool BlueToothServer::waitForClientConnection(sUI _timeoutMillisec) {
   sBool ret = waitForSocket(server_socket, _timeoutMillisec);
   return ret;
}

sBool BlueToothServer::waitForClientData(sUI _timeoutMillisec) {
   sBool ret = waitForSocket(client_socket, _timeoutMillisec);
   return ret;
}

sBool BlueToothServer::acceptClientConnection(void) {
   sBool ret = YAC_FALSE;
   socklen_t opt = sizeof(client_addr);

   client_socket = ::accept(server_socket, (struct sockaddr *)&client_addr, &opt);

   if(-1 != client_socket)
   {
#if 0
      char buf[1024];
      ba2str( &client_addr.rc_bdaddr, buf );
      yac_host->printf("[dbg] BlueToothServer::acceptClient: connection from %s\n", buf);
#endif
      ret = YAC_TRUE;
   }

   return ret;
}

void BlueToothServer::getClientAddress(YAC_Value *_r) {
   _r->initNull();

   if(-1 != client_socket)
   {
      BlueToothAddress *retAddr = YAC_NEW(BlueToothAddress);
      retAddr->bytes[0] = client_addr.rc_bdaddr.b[5];
      retAddr->bytes[1] = client_addr.rc_bdaddr.b[4];
      retAddr->bytes[2] = client_addr.rc_bdaddr.b[3];
      retAddr->bytes[3] = client_addr.rc_bdaddr.b[2];
      retAddr->bytes[4] = client_addr.rc_bdaddr.b[1];
      retAddr->bytes[5] = client_addr.rc_bdaddr.b[0];
      _r->initObject(retAddr, 1);
   }
}

sSI BlueToothServer::send(YAC_Object *_buffer) {
   sSI numBytes = 0;

   if(-1 != client_socket)
   {
      if(YAC_Is_Buffer(_buffer))
      {
         YAC_CAST_ARG(YAC_Buffer, buf, _buffer);

         if(buf->io_offset > 0)
         {
            numBytes = ::write(client_socket, buf->buffer, buf->io_offset);

            if(numBytes > 0)
            {
               // yac_host->printf("[dbg] BlueToothServer::send: ok, sent %u bytes to client.\n", numBytes);
            }
            else
            {
               yac_host->printf("[---] BlueToothServer::send: FAILED to send %u bytes to client.\n", buf->io_offset);
            }
         }
      }
   }
   
   return numBytes;
}

sSI BlueToothServer::recv(YAC_Object *_buffer) {
   sSI numBytes = 0;

   if(-1 != client_socket)
   {
      if(YAC_Is_Buffer(_buffer))
      {
         YAC_CAST_ARG(YAC_Buffer, buf, _buffer);
         
         if(buf->size > 0)
         {
            buf->io_offset = 0;
            
            numBytes = ::read(client_socket, buf->buffer, buf->size);
            
            if(numBytes >= 0)
            {
               // yac_host->printf("[dbg] BlueToothServer::recv: ok, recvd %u bytes from client.\n", numBytes);
               buf->io_offset = numBytes;
            }
            else
            {
               yac_host->printf("[---] BlueToothServer::recv: FAILED to recv reply from client.\n");
            }
         }
      }
   }
   
   return numBytes;
}
