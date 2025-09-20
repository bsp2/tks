
#include "tkportaudio.h"

#include "PaStreamParameters.h"


_PaStreamParameters::_PaStreamParameters(void) {
   device_index      = -1;
   channel_count     = 2;
   sample_format     = paInt16;
   suggested_latency = 0.0f;
}

_PaStreamParameters::~_PaStreamParameters() {
}

sSI _PaStreamParameters::_getDeviceIndex(void) {
   return device_index;
}

void _PaStreamParameters::_setDeviceIndex(sSI _devIdx) {
   if( sUI(_devIdx) >= _PortAudio::MAX_DEVICE_INFOS )
      _devIdx = -1;
   device_index = _devIdx;
}

sSI _PaStreamParameters::_getChannelCount(void) {
   return channel_count;
}

void _PaStreamParameters::_setChannelCount(sSI _channelCount) {
   if(_channelCount < 0)
      _channelCount = 0;
   channel_count = _channelCount;
}

sSI _PaStreamParameters::_getSampleFormat(void) {
   return sample_format;
}

void _PaStreamParameters::_setSampleFormat(sSI _sampleFormat) {
#ifdef FORCE_PAINT16
   sample_format = paInt16;
#else
   sample_format = _sampleFormat;
#endif
}

sF32 _PaStreamParameters::_getSuggestedLatency(void) {
   return suggested_latency;
}

void _PaStreamParameters::_setSuggestedLatency(sF32 _latency) {
   if(_latency < 0.0f)
      _latency = 0.0f;
   else if(_latency > 0.02f)
      _latency = 0.02f;
   suggested_latency = _latency;
}
