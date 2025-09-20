
#include "tkportaudio.h"


#include "PaStreamParameters.h"


_PaStreamParameters::_PaStreamParameters(void) {
   channel_count = 2;
   sample_format = paInt16;
}

_PaStreamParameters::~_PaStreamParameters() {
}

sSI  _PaStreamParameters::_getDeviceIndex(void) {
   return 0;
}

void _PaStreamParameters::_setDeviceIndex(sSI _devIdx) {
   (void)_devIdx;
}

sSI _PaStreamParameters::_getChannelCount(void) {
   return channel_count;
}

void _PaStreamParameters::_setChannelCount(sSI _channelCount) {
   channel_count = _channelCount;
}

sSI _PaStreamParameters::_getSampleFormat(void) {
   return sample_format;
}

void _PaStreamParameters::_setSampleFormat(sSI _sampleFormat) {
   sample_format = _sampleFormat;
}

sF32 _PaStreamParameters::_getSuggestedLatency(void) {
   return (PAR_LATENCY / ((sF32)PAR_RATE));
}

void _PaStreamParameters::_setSuggestedLatency(sF32 _latency) {
   (void)_latency;
}
