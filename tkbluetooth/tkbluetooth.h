#ifndef __TKBLUETOOTH_H__
#define __TKBLUETOOTH_H__

#include <stdio.h>

#ifdef YAC_WIN32
#include <initguid.h>
#include <winsock2.h>
#include <ws2bth.h>
#include <strsafe.h>
#include <intsafe.h>
#include <wchar.h>

#include <windows.h>
#include <Bthsdpdef.h>
#include <BluetoothAPIs.h>
#endif // YAC_WIN32

#ifdef YAC_LINUX
#include <unistd.h>
#include <sys/socket.h>

#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <wait.h>
#include <sys/ioctl.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#endif // YAC_LINUX

YG("tkbluetooth")


YC class BlueToothAddress : public YAC_Object {
  public:
   sU8 bytes[6];  // in left-to-right order (MSB first, not reversed like in windows bt structs!)

  public:
   BlueToothAddress(void);
   ~BlueToothAddress();

   YAC(BlueToothAddress);

  public:
   sBool YAC_VCALL yacToString (YAC_String *_s) const;  

  public:
   unsigned long long getULLong (void);

  public:
   YM sUI getByte (sUI _index);
   YM sUI setByte (sUI _index, sUI _val);

};


YC class BlueToothClient : public YAC_Object {

#ifdef YAC_WIN32
  public:
   SOCKET remote_socket;
#endif // YAC_WIN32

  public:
   YAC(BlueToothClient);
   BlueToothClient(void);
   ~BlueToothClient();

  public:

   // Scan for devices.
   // (note) Windows almost never finds any new Bluetooth devices.
   // (note) The workaround is to use the "add device" dialog in the settings to pair the device first.
   YM void scanTest (void);

   // Returns local BlueToothAddress object or null when there's no radio
   YM void getLocalAddress (YAC_Value *_r);

   // Returns local bluetooth network name or null
   YM void getLocalName (YAC_Value *_r);

   // Returns new BlueToothAddress object or null when address could not be resolved.
   YM void findAddrByName (YAC_String *_remoteName, YAC_Value *_r);

   // Connect to server. Portnr must be in the range 1..30.
   YM sBool connect(BlueToothAddress *_addr, sUI _portNr);

   // Send data to server.
   //  Returns number of bytes sent.
   //  Buffer offset determines the number of bytes to be sent.
   YM sSI send (YAC_Object *_buffer);

   // Receive data from server.
   //  Returns number of bytes received.
   //  Buffer size determines the maximum number of bytes to be received.
   //  Buffer offset is set to the actual number of bytes received.
   YM sSI recv (YAC_Object *_buffer);

   YM void close (void);
};


YC class BlueToothServer : public YAC_Object {

#ifdef YAC_LINUX
  public:
   int server_socket;
   int client_socket;
   struct sockaddr_rc client_addr = { 0 };
 #endif // YAC_LINUX

  public:
   YAC(BlueToothServer);
   BlueToothServer(void);
   ~BlueToothServer();

  protected:
#ifdef YAC_LINUX
   sBool waitForSocket (int _socket, sUI _timeoutMillisec);
#endif // YAC_LINUX

  public:
   // Create server socket. Portnr must be in the range 1..30.
   YM sBool create (sUI _portNr);

   // Close server + client sockets.
   YM void close (void);

   // Close client socket.
   YM void closeClient (void);

   // Wait for client connection (or timeout).
   // Return true when new client connection is pending.
   YM sBool waitForClientConnection (sUI _timeoutMillisec);

   // Wait for client data (or timeout).
   // Return true when new data from client is available.
   YM sBool waitForClientData (sUI _timeoutMillisec);

   // Accept new client connection (update client_socket).
   YM sBool acceptClientConnection (void);

   // query client BlueToothAddress
   YM void getClientAddress (YAC_Value *_r);

   // Receive data from client.
   //  Returns number of bytes received.
   //  Buffer size determines the maximum number of bytes to be received.
   //  Buffer offset is set to the actual number of bytes received.
   YM sSI recv (YAC_Object *_buffer);

   // Send data to client.
   //  Returns number of bytes sent.
   //  Buffer offset determines the number of bytes to be sent.
   YM sSI send (YAC_Object *_buffer);
};


#endif // __TKBLUETOOTH_H__
