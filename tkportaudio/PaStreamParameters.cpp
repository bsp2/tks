/*
 * Copyright (c) 2009 Ross Bencina and Phil Burk, TkScript bindings by Bastian Spiegel
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
/*
 * The text above constitutes the entire PortAudio license; however, 
 * the PortAudio community also makes the following non-binding requests:
 *
 * Any person wishing to distribute modifications to the Software is
 * requested to send the modifications to the original developer so that
 * they can be incorporated into the canonical version. It is also 
 * requested that these non-binding requests be included along with the 
 * license above.
 */

#include "tkportaudio.h"


#include "PaStreamParameters.h"


_PaStreamParameters::_PaStreamParameters(void) {
   memset(&params, 0, sizeof(PaStreamParameters));
}

_PaStreamParameters::~_PaStreamParameters() {
}

sSI  _PaStreamParameters::_getDeviceIndex(void) {
   return (sSI) params.device;
}

void _PaStreamParameters::_setDeviceIndex(sSI _devIdx) {
   params.device = (PaDeviceIndex) _devIdx;
}

sSI _PaStreamParameters::_getChannelCount(void) {
   return params.channelCount;
}

void _PaStreamParameters::_setChannelCount(sSI _channelCount) {
   params.channelCount = _channelCount;
}

sSI _PaStreamParameters::_getSampleFormat(void) {
   return (sSI) params.sampleFormat;
}

void _PaStreamParameters::_setSampleFormat(sSI _sampleFormat) {
   params.sampleFormat = (PaSampleFormat) _sampleFormat;
}

sF32 _PaStreamParameters::_getSuggestedLatency(void) {
   return (sF32) params.suggestedLatency;
}

void _PaStreamParameters::_setSuggestedLatency(sF32 _latency) {
   params.suggestedLatency = (PaTime) _latency;
}
