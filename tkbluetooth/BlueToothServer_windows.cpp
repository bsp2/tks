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
}

BlueToothServer::~BlueToothServer() {
   close();
}

sBool BlueToothServer::create(sUI _portNr) {
   return YAC_FALSE;
}

void BlueToothServer::close(void) {
}

void BlueToothServer::closeClient(void) {
}

sBool BlueToothServer::waitForClientConnection(sUI _timeoutMillisec) {
   sBool ret = YAC_FALSE;
   return ret;
}

sBool BlueToothServer::waitForClientData(sUI _timeoutMillisec) {
   sBool ret = YAC_FALSE;
   return ret;
}

sBool BlueToothServer::acceptClientConnection(void) {
   sBool ret = YAC_FALSE;
   return ret;
}

void BlueToothServer::getClientAddress(YAC_Value *_r) {
   _r->initNull();
}

sSI BlueToothServer::send(YAC_Object *_buffer) {
   sSI numBytes = 0;
   return numBytes;
}

sSI BlueToothServer::recv(YAC_Object *_buffer) {
   sSI numBytes = 0;
   return numBytes;
}
