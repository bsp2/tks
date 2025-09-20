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

#include "PaHostApiInfo.h"
#include "PaDeviceInfo.h"


_PaDeviceInfo::_PaDeviceInfo(void) {
   device_info  = NULL;
   device_index = ~0;
}

_PaDeviceInfo::~_PaDeviceInfo() {
}

void _PaDeviceInfo::initDeviceInfo(sUI _devIdx, const PaDeviceInfo *_devInfo) {
   device_index = _devIdx;
   device_info  = _devInfo;
}

void _PaDeviceInfo::_getName(YAC_Value *_r) {
   if(NULL != device_info)
   {
      YAC_String *s = YAC_New_String();
      s->visit(device_info->name);
      _r->initString(s, 1);
   }
}

sSI _PaDeviceInfo::_getHostApiIndex(void) {
   if(NULL != device_info)
   {
      return device_info->hostApi;
   }
   return -1;
}

void _PaDeviceInfo::_getHostApiName(YAC_Value *_r) {
   if(NULL != device_info)
   {
      YAC_String *s = YAC_New_String();
      s->visit( Pa_GetHostApiInfo( device_info->hostApi )->name );
      _r->initString(s, 1);
   }
}

void _PaDeviceInfo::_getHostApiInfo(YAC_Value *_r) {
   if(NULL != device_info)
   {
      const PaHostApiInfo *c = Pa_GetHostApiInfo(device_info->hostApi);
      if(NULL != c)
      {
         _PaHostApiInfo *o = (_PaHostApiInfo*) YAC_NEW(PaHostApiInfo);
         o->host_api_info = c;
         _r->initObject(o, 1);
      }
      else
      {
         yac_host->printf("[---] PaDeviceInfo::getHostApiInfo: Pa_GetHostApiInfo() failed.\n");
      }
   }
}

sSI _PaDeviceInfo::_getMaxInputChannels(void) {
   if(NULL != device_info)
   {
      return (sSI) device_info->maxInputChannels;
   }
   return 0;
}

sSI _PaDeviceInfo::_getMaxOutputChannels(void) {
   if(NULL != device_info)
   {
      return (sSI) device_info->maxOutputChannels;
   }
   return 0;
}

void _PaDeviceInfo::_getInputChannelName(sSI _ch, YAC_Value *_r) {
   if(NULL != device_info)
   {
      YAC_String *s = YAC_New_String();
#ifdef YAC_WIN32
      // Note: This an ASIO specific fxn
      const char *channelName;
      PaAsio_GetInputChannelName(device_index, _ch, &channelName);
      s->visit(channelName);
#else
      if(s->yacArrayAlloc(64, 0,0,0))
      {
         s->printf("input channel #%d", _ch);
      }
#endif
      _r->initString(s, 1);
   }
}

void _PaDeviceInfo::_getOutputChannelName(sSI _ch, YAC_Value *_r) {
   if(NULL != device_info)
   {
      YAC_String *s = YAC_New_String();
#ifdef YAC_WIN32
      // Note: This an ASIO specific fxn
      const char *channelName;
      PaAsio_GetOutputChannelName(device_index, _ch, &channelName);
      s->visit(channelName);
#else
      if(s->yacArrayAlloc(64, 0,0,0))
      {
         s->printf("input channel #%d", _ch);
      }
#endif
      _r->initString(s, 1);
   }
}

sF32 _PaDeviceInfo::_getDefaultLowInputLatency(void) {
   if(NULL != device_info)
   {
      return (sF32) device_info->defaultLowInputLatency;
   }
   return -1.0f;
}

sF32 _PaDeviceInfo::_getDefaultLowOutputLatency(void) {
   if(NULL != device_info)
   {
      return (sF32) device_info->defaultLowOutputLatency;
   }
   return -1.0f;
}

sF32 _PaDeviceInfo::_getDefaultHighInputLatency(void) {
   if(NULL != device_info)
   {
      return (sF32) device_info->defaultHighInputLatency;
   }
   return -1.0f;
}

sF32 _PaDeviceInfo::_getDefaultHighOutputLatency(void) {
   if(NULL != device_info)
   {
      return (sF32) device_info->defaultHighOutputLatency;
   }
   return -1.0f;
}

sF32 _PaDeviceInfo::_getDefaultSampleRate(void) {
   if(NULL != device_info)
   {
      return (sF32) device_info->defaultSampleRate;
   }
   return -1.0f;
}

sSI _PaDeviceInfo::_getDefaultSampleFormat(void) {
#ifdef YAC_MACOS
   // macOS
   return paFloat32;
#else
   // Windows
   return paInt24;
#endif
   // (note) on Linux, please use tkportaudio_alsa_async version of plugin
   //         (portaudio does not support all of the ALSA sample formats, e.g. my "Session I/O" interface
   //          requires SND_PCM_FORMAT_S24_3BE)
}
