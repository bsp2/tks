
#include "tkportaudio.h"

#include "PaHostApiInfo.h"
#include "PaDeviceInfo.h"


_PaDeviceInfo::_PaDeviceInfo(void) {
   hw_name = NULL;
   name = NULL;
   sample_rate = 0u;
   sample_format = 0;
   alsa_sample_format = 0;
   num_in = 0u;
   num_out = 0u;
   min_input_latency = 0.0f;
   max_input_latency = 0.0f;
   min_output_latency = 0.0f;
   max_output_latency = 0.0f;
}

_PaDeviceInfo::~_PaDeviceInfo() {
   YAC_DELETE_SAFE(hw_name);
   YAC_DELETE_SAFE(name);
}

void _PaDeviceInfo::initDeviceInfo(const char *_hwName, const char *_name,
                                   sUI _sampleRate,
                                   sSI _sampleFormat,
                                   sSI _alsaSmpFormat,
                                   sUI _numIn, sUI _numOut,
                                   sF32 _defMinInputLatency, sF32 _defMaxInputLatency,
                                   sF32 _defMinOutputLatency, sF32 _defMaxOutputLatency
                                   ) {
   hw_name = YAC_New_String();
   hw_name->copy(_hwName);

   name = YAC_New_String();
   name->copy(_name);

   sample_rate        = _sampleRate;
   sample_format      = _sampleFormat;
   alsa_sample_format = _alsaSmpFormat;
   num_in             = _numIn;
   num_out            = _numOut;
   min_input_latency  = _defMinInputLatency;
   max_input_latency  = _defMaxInputLatency;
   min_output_latency = _defMinOutputLatency;
   max_output_latency = _defMaxOutputLatency;
}

void _PaDeviceInfo::_getName(YAC_Value *_r) {
   YAC_String *s = YAC_New_String();
   s->visit( (const char*) ( (NULL != name) ? name->chars : NULL ) );
   _r->initString(s, 1);
}

sSI _PaDeviceInfo::_getHostApiIndex(void) {
   return 0;
}

void _PaDeviceInfo::_getHostApiName(YAC_Value *_r) {
   YAC_String *s = YAC_New_String();
   s->visit("ALSA");
   _r->initString(s, 1);
}

void _PaDeviceInfo::_getHostApiInfo(YAC_Value *_r) {
   _PaHostApiInfo *o = (_PaHostApiInfo*) 
      YAC_NEW_(PaHostApiInfo);
   _r->initObject(o, 1);
}

sUI _PaDeviceInfo::_getMaxInputChannels(void) {
   return num_in;
}

sUI _PaDeviceInfo::_getMaxOutputChannels(void) {
   return num_out;
}

void _PaDeviceInfo::_getInputChannelName(sUI _ch, YAC_Value *_r) {
   YAC_String *s = YAC_New_String();
   if(s->yacArrayAlloc(64, 0,0,0))
   {
      s->printf("input channel #%u", _ch);
   }
   _r->initString(s, 1);
}

void _PaDeviceInfo::_getOutputChannelName(sUI _ch, YAC_Value *_r) {
   YAC_String *s = YAC_New_String();
   if(s->yacArrayAlloc(64, 0,0,0))
   {
      s->printf("output channel #%u", _ch);
   }
   _r->initString(s, 1);
}

sF32 _PaDeviceInfo::_getDefaultLowInputLatency(void) {
   return min_input_latency;
}

sF32 _PaDeviceInfo::_getDefaultLowOutputLatency(void) {
   return min_output_latency;
}

sF32 _PaDeviceInfo::_getDefaultHighInputLatency(void) {
   return max_input_latency;
}

sF32 _PaDeviceInfo::_getDefaultHighOutputLatency(void) {
   return max_output_latency;
}

sF32 _PaDeviceInfo::_getDefaultSampleRate(void) {
   return sF32(sample_rate);
}

sSI _PaDeviceInfo::_getDefaultSampleFormat(void) {
   return sSI(sample_format);
}
