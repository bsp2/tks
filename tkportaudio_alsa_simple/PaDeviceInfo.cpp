
#include "tkportaudio.h"

#include "PaHostApiInfo.h"
#include "PaDeviceInfo.h"


_PaDeviceInfo::_PaDeviceInfo(void) {
}

_PaDeviceInfo::~_PaDeviceInfo() {
}

void _PaDeviceInfo::initDeviceInfo(void) {
}

void _PaDeviceInfo::_getName(YAC_Value *_r) {
   YAC_String *s = YAC_New_String();
   s->visit("/dev/dsp");
   _r->initString(s, 1);
}

sSI _PaDeviceInfo::_getHostApiIndex(void) {
   return 0;
}

void _PaDeviceInfo::_getHostApiName(YAC_Value *_r) {
   YAC_String *s = YAC_New_String();
   s->visit("oss");
   _r->initString(s, 1);
}

void _PaDeviceInfo::_getHostApiInfo(YAC_Value *_r) {
   _PaHostApiInfo *o = (_PaHostApiInfo*) 
      YAC_NEW_(PaHostApiInfo);
   _r->initObject(o, 1);
}

sSI _PaDeviceInfo::_getMaxInputChannels(void) {
   return 0;
}

sSI _PaDeviceInfo::_getMaxOutputChannels(void) {
   return 2;
}

void _PaDeviceInfo::_getInputChannelName(sSI _ch, YAC_Value *_r) {
}

void _PaDeviceInfo::_getOutputChannelName(sSI _ch, YAC_Value *_r) {
   YAC_String *s = YAC_New_String();
   if(s->yacArrayAlloc(64, 0,0,0))
   {
      s->printf("output channel #%d", _ch);
   }
   _r->initString(s, 1);
}

sF32 _PaDeviceInfo::_getDefaultLowInputLatency(void) {
   return PAR_LATENCY_SEC;
}

sF32 _PaDeviceInfo::_getDefaultLowOutputLatency(void) {
   return PAR_LATENCY_SEC;
}

sF32 _PaDeviceInfo::_getDefaultHighInputLatency(void) {
   return PAR_LATENCY_SEC;
}

sF32 _PaDeviceInfo::_getDefaultHighOutputLatency(void) {
   return PAR_LATENCY_SEC;
}

sF32 _PaDeviceInfo::_getDefaultSampleRate(void) {
   return PAR_RATE;
}
