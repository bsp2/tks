
#include "tkportaudio.h"

#include "PaHostApiInfo.h"


_PaHostApiInfo::_PaHostApiInfo(void) {
}

_PaHostApiInfo::~_PaHostApiInfo() {
}

sSI _PaHostApiInfo::_getApiTypeId(void) {
   return 0;
}

void _PaHostApiInfo::_getName(YAC_Value *_r) {
   YAC_String *s = YAC_New_String();
   s->visit("oss");
   _r->initString(s, 1);
}

sSI _PaHostApiInfo::_getDeviceCount(void) {
   return 1;
}

sSI _PaHostApiInfo::_getDefaultInputDeviceIndex(void) {
   return 0;
}

sSI _PaHostApiInfo::_getDefaultOutputDeviceIndex(void) {
   return 0;
}


