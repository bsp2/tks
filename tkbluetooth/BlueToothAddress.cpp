#include <stdio.h>

#define YAC_BIGSTRING
#define YAC_PRINTF
#include <yac.h>
#include "tkbluetooth.h"

BlueToothAddress::BlueToothAddress(void) {
   bytes[0] = 0xCDu;
   bytes[1] = 0xCDu;
   bytes[2] = 0xCDu;
   bytes[3] = 0xCDu;
   bytes[4] = 0xCDu;
   bytes[5] = 0xCDu;
}

BlueToothAddress::~BlueToothAddress() {
}

sBool YAC_VCALL BlueToothAddress::yacToString(YAC_String *rs) const { 
   rs->alloc(18);
   ::sprintf((char*)rs->chars, "%02x:%02x:%02x:%02x:%02x:%02x", 
             bytes[0],
             bytes[1],
             bytes[2],
             bytes[3],
             bytes[4],
             bytes[5]
             );
   rs->fixLength(); 
   return YAC_TRUE;
} 

unsigned long long BlueToothAddress::getULLong(void) {
   union {
      unsigned long long btAddr;
      unsigned char bytes[6];
   } uAddr;
   uAddr.bytes[0] = bytes[5];
   uAddr.bytes[1] = bytes[4];
   uAddr.bytes[2] = bytes[3];
   uAddr.bytes[3] = bytes[2];
   uAddr.bytes[4] = bytes[1];
   uAddr.bytes[5] = bytes[0];
   return uAddr.btAddr;
}

sUI BlueToothAddress::getByte(sUI _index) {
   if(_index < 6u)
      return bytes[_index];
   return 0u;
}

sUI BlueToothAddress::setByte(sUI _index, sUI _val) {
   if(_index < 6u)
      bytes[_index] = sU8(_val);
   return 0u;
}
