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


// #define Dprintf if(0);else printf
#define Dprintf if(1);else printf


static void loc_wcstoascii(char *d, const wchar_t *s, size_t numChars) {
   for(size_t i = 0; i < numChars; i++)
      d[i] = (char)s[i];
}

static void loc_asciitowcs(wchar_t *d, const sU8 *s, size_t numChars) {
   for(size_t i = 0; i < numChars; i++)
      d[i] = (wchar_t)s[i];
}


BlueToothClient::BlueToothClient(void) {
   remote_socket = NULL;
}

BlueToothClient::~BlueToothClient() {
   close();
}

void BlueToothClient::scanTest(void) {

   yac_host->printf("xxx BlueToothClient::scanTest: ENTER\n");

   HANDLE hRadio = NULL;

   {
      BLUETOOTH_FIND_RADIO_PARAMS findRadioParams;
      findRadioParams.dwSize = sizeof(findRadioParams);
      HBLUETOOTH_RADIO_FIND hRadioFind =
         BluetoothFindFirstRadio(&findRadioParams, &hRadio);

      yac_host->printf("xxx hRadioFind=%p hRadio=%p\n", hRadioFind, hRadio);

      if(NULL != hRadioFind)
      {
         // https://docs.microsoft.com/en-us/windows/desktop/api/BluetoothAPIs/nf-bluetoothapis-bluetoothgetradioinfo
         BLUETOOTH_RADIO_INFO radioInfo = {sizeof(BLUETOOTH_RADIO_INFO),0,};
         int err = BluetoothGetRadioInfo(hRadio, &radioInfo);

         if(ERROR_SUCCESS == err)
         {
            wprintf(L"\tInstance Name: %s\r\n", radioInfo.szName);
            yac_host->printf("\tAddress: %02X:%02X:%02X:%02X:%02X:%02X\r\n",
                   radioInfo.address.rgBytes[5],
                   radioInfo.address.rgBytes[4],
                   radioInfo.address.rgBytes[3],
                   radioInfo.address.rgBytes[2],
                   radioInfo.address.rgBytes[1],
                   radioInfo.address.rgBytes[0]
                   );

            yac_host->printf("\tClass: 0x%08x\r\n", radioInfo.ulClassofDevice);
            yac_host->printf("\tManufacturer: 0x%04x\r\n", radioInfo.manufacturer);
         }
         else
         {
            yac_host->printf("[---] BluetoothGetRadioInfo() failed. err=%d\n", err);
         }

         if(BluetoothEnableIncomingConnections(hRadio, TRUE))
         {
            yac_host->printf("[dbg] BluetoothEnableIncomingConnections(): ok\n");
         }
         else
         {
            yac_host->printf("[dbg] BluetoothEnableIncomingConnections(): FAILED\n");
         }

         {
            BOOL bIsConnectable = BluetoothIsConnectable(hRadio);
            yac_host->printf("[dbg] BluetoothIsConnectable() = %d\n", bIsConnectable);
         }

#if 0
         CloseHandle(hRadio);

         for(;;)
         {
            if(!BluetoothFindNextRadio(hRadioFind, &hRadio))
            {
               break;
            }
            else
            {
               yac_host->printf("xxx hRadioFind next hRadio=%p\n", hRadio);
               CloseHandle(hRadio);
            }
         }

#endif
         (void)BluetoothFindRadioClose(hRadioFind);
      }

   }

   if(NULL != hRadio)
   {
      if(!BluetoothEnableDiscovery(hRadio, TRUE))
      {
         yac_host->printf("xxx BluetoothEnableDiscovery() failed.\n");
      }


      BLUETOOTH_DEVICE_SEARCH_PARAMS deviceSearchParams;
      memset((void*)&deviceSearchParams, 0, sizeof(deviceSearchParams));
      deviceSearchParams.dwSize = sizeof(deviceSearchParams);
      deviceSearchParams.fReturnAuthenticated = TRUE;
      deviceSearchParams.fReturnRemembered    = FALSE;
      deviceSearchParams.fReturnUnknown       = TRUE;
      deviceSearchParams.fReturnConnected     = TRUE;
      deviceSearchParams.fIssueInquiry        = TRUE;
      // deviceSearchParams.cTimeoutMultiplier   = 10;  // ~12sec
      deviceSearchParams.cTimeoutMultiplier   = 3;  // ~12sec
      deviceSearchParams.hRadio = hRadio;  // NULL == all radios

      BLUETOOTH_DEVICE_INFO deviceInfo;
      memset((void*)&deviceInfo, 0, sizeof(deviceInfo));
      deviceInfo.dwSize = sizeof(deviceInfo);
      HBLUETOOTH_DEVICE_FIND hDeviceFind =
         BluetoothFindFirstDevice(&deviceSearchParams, &deviceInfo);

      yac_host->printf("xxx hDeviceFind=%p\n", hDeviceFind);

      sUI deviceIdx = 0u;

      if(NULL != hDeviceFind)
      {
         do
         {
            wprintf(L"[dbg] deviceInfo[%u].Address = %02x:%02x:%02x:%02x:%02x:%02x  isConnected=%d isAuth=%d  name=\"%s\"\n",
                    deviceIdx,
                    deviceInfo.Address.rgBytes[5],
                    deviceInfo.Address.rgBytes[4],
                    deviceInfo.Address.rgBytes[3],
                    deviceInfo.Address.rgBytes[2],
                    deviceInfo.Address.rgBytes[1],
                    deviceInfo.Address.rgBytes[0],
                    deviceInfo.fConnected,
                    deviceInfo.fAuthenticated,
                    deviceInfo.szName
                    );

            deviceIdx++;
         }
         while(BluetoothFindNextDevice(hDeviceFind, &deviceInfo));

         BluetoothFindDeviceClose(hDeviceFind);
      }
      else
      {
         int wsaErr = WSAGetLastError();
         yac_host->printf("[---] BluetoothFindFirstDevice() returned NULL. GetLastError=%d WSAGetLastError()=%d\n", GetLastError(), wsaErr);

         if(ERROR_NO_MORE_ITEMS == wsaErr)
         {
            yac_host->printf("[~~~] BluetoothFindFirstDevice(): NO_MORE_ITEMS\n");
         }
      }
   }

   if(NULL != hRadio)
   {
      CloseHandle(hRadio);
   }

   yac_host->printf("xxx BlueToothClient::scanTest: LEAVE\n");

}

void BlueToothClient::getLocalAddress(YAC_Value *_r) {

   _r->initNull();

   HANDLE hRadio = NULL;
   BLUETOOTH_FIND_RADIO_PARAMS findRadioParams;
   findRadioParams.dwSize = sizeof(findRadioParams);
   HBLUETOOTH_RADIO_FIND hRadioFind =
      BluetoothFindFirstRadio(&findRadioParams, &hRadio);

   // yac_host->printf("xxx getLocalAddress: hRadioFind=%p hRadio=%p\n", hRadioFind, hRadio);

   if(NULL != hRadioFind)
   {
      // https://docs.microsoft.com/en-us/windows/desktop/api/BluetoothAPIs/nf-bluetoothapis-bluetoothgetradioinfo
      BLUETOOTH_RADIO_INFO radioInfo = {sizeof(BLUETOOTH_RADIO_INFO),0,};
      int err = BluetoothGetRadioInfo(hRadio, &radioInfo);

      if(ERROR_SUCCESS == err)
      {
         BlueToothAddress *retAddr = YAC_NEW(BlueToothAddress);
         retAddr->bytes[0] = radioInfo.address.rgBytes[5];
         retAddr->bytes[1] = radioInfo.address.rgBytes[4];
         retAddr->bytes[2] = radioInfo.address.rgBytes[3];
         retAddr->bytes[3] = radioInfo.address.rgBytes[2];
         retAddr->bytes[4] = radioInfo.address.rgBytes[1];
         retAddr->bytes[5] = radioInfo.address.rgBytes[0];
         _r->initObject(retAddr, 1);
      }
      else
      {
         yac_host->printf("[---] BlueToothClient::getLocalAddress: BluetoothGetRadioInfo() failed. err=%d\n", err);
      }

      CloseHandle(hRadio);
      (void)BluetoothFindRadioClose(hRadioFind);
   }

}

void BlueToothClient::getLocalName(YAC_Value *_r) {
   _r->initNull();

   HANDLE hRadio = NULL;

   {
      BLUETOOTH_FIND_RADIO_PARAMS findRadioParams;
      findRadioParams.dwSize = sizeof(findRadioParams);
      HBLUETOOTH_RADIO_FIND hRadioFind =
         BluetoothFindFirstRadio(&findRadioParams, &hRadio);

      yac_host->printf("xxx BlueToothClient::getLocalName: hRadioFind=%p hRadio=%p\n", hRadioFind, hRadio);

      if(NULL != hRadioFind)
      {
         // https://docs.microsoft.com/en-us/windows/desktop/api/BluetoothAPIs/nf-bluetoothapis-bluetoothgetradioinfo
         BLUETOOTH_RADIO_INFO radioInfo = {sizeof(BLUETOOTH_RADIO_INFO),0,};
         int err = BluetoothGetRadioInfo(hRadio, &radioInfo);

         if(ERROR_SUCCESS == err)
         {
            char localName[256];
            // // loc_wcstoascii(localName, radioInfo.szName, wstrlen(radioInfo.szName));
            loc_wcstoascii(localName, radioInfo.szName, wcslen(radioInfo.szName));

            _r->initEmptyString();
            _r->value.string_val->copy(localName);
         }
         else
         {
            yac_host->printf("[---] BlueToothClient::getLocalName: BluetoothGetRadioInfo() failed. err=%d\n", err);
         }

         ::CloseHandle(hRadio);
         (void)::BluetoothFindRadioClose(hRadioFind);
      }
   }
}

void BlueToothClient::findAddrByName(YAC_String *_remoteName, YAC_Value *_r) {
   // printf("xxx BlueToothClient::findAddrByName: ENTER\n");

   _r->initNull();

   int err;
   WSADATA WSAData = {0};
   // // SOCKADDR_BTH remoteBthAddr = {0};
   BLUETOOTH_ADDRESS_STRUCT remoteBthAddr;
   PWSAQUERYSET pWSAQuerySet = NULL;

   err = WSAStartup(MAKEWORD(2, 2), &WSAData);
   if(0 != err) {
      yac_host->printf("[---] BlueToothClient::findAddrByName: unable to initialize Winsock version 2.2\n");
      return;
   }

   ZeroMemory(&remoteBthAddr, sizeof(remoteBthAddr));

   ULONG ulPQSSize = sizeof(WSAQUERYSET);

   pWSAQuerySet = (PWSAQUERYSET) HeapAlloc(GetProcessHeap(),
                                           HEAP_ZERO_MEMORY,
                                           ulPQSSize);
   if ( NULL == pWSAQuerySet ) {
      yac_host->printf("[---] BlueToothClient::findAddrByName: unable to allocate memory for WSAQUERYSET\n");
      return;
   }

   ULONG ulFlags;

   //
   // WSALookupService is used for both service search and device inquiry
   // LUP_CONTAINERS is the flag which signals that we're doing a device inquiry.
   //
   ulFlags = LUP_CONTAINERS;
   
   //
   // Friendly device name (if available) will be returned in lpszServiceInstanceName
   //
   ulFlags |= LUP_RETURN_NAME;

   //
   // BTH_ADDR will be returned in lpcsaBuffer member of WSAQUERYSET
   //
   ulFlags |= LUP_RETURN_ADDR;

   // By setting LUP_FLUSHCACHE flag, we're asking the lookup service to do
   // a fresh lookup instead of pulling the information from device cache.
   //
   // ulFlags |= LUP_FLUSHCACHE;

   //
   // Start the lookup service
   //
   HANDLE hLookup = NULL;
   ZeroMemory(pWSAQuerySet, ulPQSSize);
   pWSAQuerySet->dwNameSpace = NS_BTH;
   pWSAQuerySet->dwSize = sizeof(WSAQUERYSET);
   err = WSALookupServiceBegin(pWSAQuerySet, ulFlags, &hLookup);

   // wchar_t szRemoteName[BTH_MAX_NAME_SIZE + 1] = {0};  // +1 for ASCIIZ
   wchar_t szRemoteAddr[17 + 1] = {0}; // 6 two-digit hex values plus 5 colons, plus ASCIIZ

   // loc_asciitowcs(szRemoteName, _remoteName->chars, _remoteName->length);

   bool bContinueLookup;
   bool bRemoteDeviceFound = false;

   if( (NO_ERROR == err) && (NULL != hLookup) )
   {
      bContinueLookup = true;
   }
   else
   {
      // https://docs.microsoft.com/de-de/windows/desktop/WinSock/windows-sockets-error-codes-2
      //  10108 = WSASERVICE_NOT_FOUND
      //   "This error is returned for a bluetooth service discovery request if no remote bluetooth devices were found."
      bContinueLookup = false;
      yac_host->printf("[---] BlueToothClient::findAddrByName: WSALookupServiceBegin err=%d WSAErr=%d\n", err, WSAGetLastError());
   }


   while(bContinueLookup)
   {
      //
      // Get information about next bluetooth device
      //
      // Note you may pass the same WSAQUERYSET from LookupBegin
      // as long as you don't need to modify any of the pointer
      // members of the structure, etc.
      //
      // ZeroMemory(pWSAQuerySet, ulPQSSize);
      // pWSAQuerySet->dwNameSpace = NS_BTH;
      // pWSAQuerySet->dwSize = sizeof(WSAQUERYSET);
      if ( NO_ERROR == WSALookupServiceNext(hLookup,
                                            ulFlags,
                                            &ulPQSSize,
                                            pWSAQuerySet) ) {
                    
         //
         // Compare the name to see if this is the device we are looking for.
         //
         yac_host->printf("[dbg] BlueToothClient::findAddrByName: pWSAQuerySet->lpszServiceInstanceName = \"%s\"\n", pWSAQuerySet->lpszServiceInstanceName);

#if 1
         if ( ( pWSAQuerySet->lpszServiceInstanceName != NULL ) &&
              // // ( 0 == _wcsicmp(pWSAQuerySet->lpszServiceInstanceName, szRemoteName) ) ) {
              ( 0 == strcmp(pWSAQuerySet->lpszServiceInstanceName, (const char*)_remoteName->chars) ) ) {
            //
            // Found a remote bluetooth device with matching name.
            // Get the address of the device and exit the lookup.
            //
            yac_host->printf("[dbg] BlueToothClient::findAddrByName: found remote name\n");
            const PSOCKADDR_BTH psockAddrBth = (PSOCKADDR_BTH) pWSAQuerySet->lpcsaBuffer->RemoteAddr.lpSockaddr;
            // // CopyMemory(&remoteBthAddr,
            // //            (PSOCKADDR_BTH) pWSAQuerySet->lpcsaBuffer->RemoteAddr.lpSockaddr,
            // //            sizeof(remoteBthAddr));
            remoteBthAddr.ullLong = psockAddrBth->btAddr;
            yac_host->printf("[dbg] BlueToothClient::findAddrByName: found remote name. Addr= %02x:%02x:%02x:%02x:%02x:%02x\n",
                             remoteBthAddr.rgBytes[5],
                             remoteBthAddr.rgBytes[4],
                             remoteBthAddr.rgBytes[3],
                             remoteBthAddr.rgBytes[2],
                             remoteBthAddr.rgBytes[1],
                             remoteBthAddr.rgBytes[0]
                             );
            BlueToothAddress *retAddr = YAC_NEW(BlueToothAddress);
            retAddr->bytes[0] = remoteBthAddr.rgBytes[5];
            retAddr->bytes[1] = remoteBthAddr.rgBytes[4];
            retAddr->bytes[2] = remoteBthAddr.rgBytes[3];
            retAddr->bytes[3] = remoteBthAddr.rgBytes[2];
            retAddr->bytes[4] = remoteBthAddr.rgBytes[1];
            retAddr->bytes[5] = remoteBthAddr.rgBytes[0];
            _r->initObject(retAddr, 1);
            bRemoteDeviceFound = true;
            bContinueLookup = false;
         }
#endif
      }
      else
      {
         err = WSAGetLastError();

         if(WSA_E_NO_MORE == err)
         {
            //
            // No more devices found.  Exit the lookup.
            //
            yac_host->printf("[dbg] BlueToothClient::findAddrByName: no more devices (WSA_E_NO_MORE)\n");
            bContinueLookup = false;
         }
         else if(WSAEFAULT == err)
         {
            yac_host->printf("[~~~] BlueToothClient::findAddrByName: WSAEFAULT (=> realloc QUERYSET buffer)\n");
            //
            // The buffer for QUERYSET was insufficient.
            // In such case 3rd parameter "ulPQSSize" of function "WSALookupServiceNext()" receives
            // the required size.  So we can use this parameter to reallocate memory for QUERYSET.
            //
            HeapFree(GetProcessHeap(), 0, pWSAQuerySet);
            pWSAQuerySet = (PWSAQUERYSET) HeapAlloc(GetProcessHeap(),
                                                    HEAP_ZERO_MEMORY,
                                                    ulPQSSize);
            if(NULL == pWSAQuerySet)
            {
               yac_host->printf("[---] BlueToothClient::findAddrByName: unable to allocate memory for WSAQERYSET\n");
               bContinueLookup = false;
            }
         }
         else
         {
            yac_host->printf("[---] BlueToothClient::findAddrByName: WSALookupServiceNext() failed with error code %d\n", err);
            bContinueLookup = false;
         }
      }
   }

   //
   // End the lookup service
   //
   WSALookupServiceEnd(hLookup);

   if ( NULL != pWSAQuerySet ) {
      HeapFree(GetProcessHeap(), 0, pWSAQuerySet);
      pWSAQuerySet = NULL;
   }
   
   // printf("xxx BlueToothClient::findAddrByName: LEAVE\n");
}

DEFINE_GUID(g_dummy_guid, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

sBool BlueToothClient::connect(BlueToothAddress *_addr, sUI _portNr) {

   sBool ret = YAC_FALSE;

   // https://docs.microsoft.com/de-de/windows/desktop/api/ws2bth/ns-ws2bth-_sockaddr_bth
   SOCKADDR_BTH remoteAddr;
   remoteAddr.addressFamily  = AF_BTH;
   remoteAddr.btAddr         = _addr->getULLong();
   remoteAddr.serviceClassId = g_dummy_guid;  // "ignored if the port is specified"
   remoteAddr.port           = _portNr;  // "The valid range for requesting a specific RFCOMM port is 1 through 30."

   remote_socket = ::socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);

   if(INVALID_SOCKET != remote_socket)
   {
      {
         int timeout = 1000; // 1sec (setting this too low will cause disconnects)
         int err;

         err = ::setsockopt(remote_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(int));
         if(0 != err)
         {
            yac_host->printf("[---] BlueToothClient::connect: setsockopt(SO_RCVTIMEO) failed with err=%d\n", err);
         }

         err = ::setsockopt(remote_socket, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(int));
         if(0 != err)
         {
            yac_host->printf("[---] BlueToothClient::connect: setsockopt(SO_SNDTIMEO) failed with err=%d\n", err);
         }
      }

      yac_host->printf("[dbg] BlueToothClient::connect: calling connect()\n");

#if 0
      {
         // https://msdn.microsoft.com/de-de/library/windows/desktop/ms738573(v=vs.85).aspx
         unsigned long arg = 1;
         ::ioctlsocket(remote_socket, FIONBIO, &arg);
      }
#endif

      if(SOCKET_ERROR != ::connect(remote_socket, (struct sockaddr *) &remoteAddr, sizeof(SOCKADDR_BTH)))
      {
         yac_host->printf("[dbg] BlueToothClient::connect: ok, socket open.\n");

#if 0
         {
            // https://msdn.microsoft.com/de-de/library/windows/desktop/ms738573(v=vs.85).aspx
            unsigned long arg = 0;
            ::ioctlsocket(remote_socket, FIONBIO, &arg);
         }
#endif

#if 0
         // https://msdn.microsoft.com/de-de/library/windows/desktop/ms740149(v=vs.85).aspx
			if(SOCKET_ERROR != ::send(remote_socket, (char *)_s->chars, _s->length, 0/*flags*/))
         {
            yac_host->printf("[dbg] BlueToothClient::connect: ok, string sent (%u bytes).\n", _s->length);
         }
         else
         {
            yac_host->printf("[---] BlueToothClient::connect: FAILED to send string (%u bytes).\n", _s->length);
         }

         {
            char buf[128];
            int numRead = ::recv(remote_socket, buf, 128, 0/*flags*/);
            if(SOCKET_ERROR != numRead)
            {
               yac_host->printf("[dbg] BlueToothClient::connect: ok, recvd %u bytes from server (\"%s\").\n", numRead, buf);
            }
            else
            {
               yac_host->printf("[---] BlueToothClient::connect: FAILED to recv reply from server.\n");
            }
         }

         yac_host->printf("[dbg] BlueToothClient::connect: closing socket.\n");
         ::closesocket(remote_socket);
#endif

         ret = YAC_TRUE;
      }
      else
      {
         if(SOCKET_ERROR == ::closesocket(remote_socket))
         {
            yac_host->printf("[---] BlueToothClient::connect: closesocket() failed. GetLastError()=%d\n", GetLastError());
         }
         
         remote_socket = INVALID_SOCKET;
      }
   }
   else
   {
      yac_host->printf("[---] BlueToothClient::connect: socket() failed (INVALID_SOCKET)\n");
   }

   return ret;
}

sSI BlueToothClient::send(YAC_Object *_buffer) {
   sSI numBytes = 0;

   if(NULL != remote_socket)
   {
      if(YAC_Is_Buffer(_buffer))
      {
         YAC_CAST_ARG(YAC_Buffer, buf, _buffer);

         if(buf->io_offset > 0)
         {
            // https://msdn.microsoft.com/de-de/library/windows/desktop/ms740149(v=vs.85).aspx
            int numSent = ::send(remote_socket, (const char*)buf->buffer, buf->io_offset, 0/*flags*/);

            if(SOCKET_ERROR != numSent)
            {
               // yac_host->printf("[dbg] BlueToothClient::send: ok, sent %u bytes to server.\n", numSent);
               numBytes = buf->io_offset;
            }
            else
            {
               yac_host->printf("[---] BlueToothClient::send: FAILED to send %u bytes.\n", buf->io_offset);
            }
         }
      }
   }
   
   return numBytes;
}

sSI BlueToothClient::recv(YAC_Object *_buffer) {
   sSI numBytes = 0;

   if(NULL != remote_socket)
   {
      if(YAC_Is_Buffer(_buffer))
      {
         YAC_CAST_ARG(YAC_Buffer, buf, _buffer);

         if(buf->size > 0)
         {
            buf->io_offset = 0;

            int numRead = ::recv(remote_socket, (char*)buf->buffer, buf->size, 0/*flags*/);

            if(SOCKET_ERROR != numRead)
            {
               // yac_host->printf("[dbg] BlueToothClient::recv: ok, recvd %u bytes from server.\n", numRead);
               numBytes = numRead;
               buf->io_offset = numBytes;
            }
            else
            {
               yac_host->printf("[---] BlueToothClient::recv: FAILED to recv reply from server.\n");
            }
         }
      }
   }
   
   return numBytes;
}

void BlueToothClient::close(void) {
   if(NULL != remote_socket)
   {
      ::closesocket(remote_socket);
      remote_socket = NULL;
   }
}
