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

#include "PaStream.h"


_PaStreamMacOSWorkgroupJoinToken::_PaStreamMacOSWorkgroupJoinToken(void) {
#ifdef YAC_MACOS
   ::memset(&join_token, 0, sizeof(os_workgroup_join_token_s));
#endif // YAC_MACOS
}

_PaStreamMacOSWorkgroupJoinToken::~_PaStreamMacOSWorkgroupJoinToken() {
}


_PaStream::_PaStream(void) {
   stream = NULL;

   stream_cbk         = NULL;
   stream_cbk_context = NULL;

   num_input_channels  = 0u;
   num_output_channels = 0u;

   input_sample_format  = 0;
   output_sample_format = 0;

#ifdef YAC_MACOS
   ::memset(&macos_audio_workgroup,  0, sizeof(os_workgroup_t));
#endif // YAC_MACOS
}

_PaStream::~_PaStream() {
   if(NULL != stream_cbk_context)
   {
      yac_host->yacContextDestroy(stream_cbk_context);
      stream_cbk_context = NULL;
   }

   _abort();
   _close();
}

void _PaStream::initStream(PaStream *_stream, void *_cbk, sUI _numIn, sUI _numOut, PaSampleFormat _inFmt, PaSampleFormat _outFmt) {
   stream = _stream;

   stream_cbk         = _cbk;
   if(NULL == stream_cbk_context)
   {
      stream_cbk_context = yac_host->yacContextCreate();
   }

   input_buf  = YAC_New_FloatArray();
   input_buf->yacArrayAlloc(TKPORTAUDIO_MAX_BUFSIZE * TKPORTAUDIO_MAX_CHANNELS);

   output_buf = YAC_New_FloatArray();
   output_buf->yacArrayAlloc(TKPORTAUDIO_MAX_BUFSIZE * TKPORTAUDIO_MAX_CHANNELS);

   num_input_channels  = _numIn;
   num_output_channels = _numOut;

   input_sample_format  = _inFmt;
   output_sample_format = _outFmt;
}

void _PaStream::_close(void) {
   if(NULL != stream)
   {
      Pa_CloseStream(stream);
      stream = NULL;

      YAC_DELETE_SAFE(input_buf);
      YAC_DELETE_SAFE(output_buf);
   }
}

void _PaStream::_start(void) {
   if(NULL != stream)
   {
      Pa_StartStream(stream);
   }
}

void _PaStream::_stop(void) {
   if(NULL != stream)
   {
      Pa_StopStream(stream);
   }
}

void _PaStream::_abort(void) {
   if(NULL != stream)
   {
      Pa_AbortStream(stream);
   }
}

sSI _PaStream::_isStopped(void) {
   if(NULL != stream)
   {
      return (1 == Pa_IsStreamStopped(stream));
   }
   return 0;
}

sSI _PaStream::_isActive(void) {
   if(NULL != stream)
   {
      return (1 == Pa_IsStreamActive(stream));
   }
   return 0;
}

sF32 _PaStream::_getCpuLoad(void) {
   if(NULL != stream)
   {
      return (sF32) Pa_GetStreamCpuLoad(stream);
   }
   return 0.0f;
}

sSI _PaStream::_getNumInputChannels(void) {
   return (sSI) num_input_channels;
}

sSI _PaStream::_getNumOutputChannels(void) {
   return (sSI) num_output_channels;
}

sF32 _PaStream::_getInputLatency(void) {
   if(NULL != stream)
   {
      const PaStreamInfo *info = Pa_GetStreamInfo(stream);
      if(NULL != info)
      {
         return (sF32)info->inputLatency;
      }
   }
   return 0.0f;
}

sF32 _PaStream::_getOutputLatency(void) {
   if(NULL != stream)
   {
      const PaStreamInfo *info = Pa_GetStreamInfo(stream);
      if(NULL != info)
      {
         return (sF32)info->outputLatency;
      }
   }
   return 0.0f;
}

sF32 _PaStream::_getSampleRate(void) {
   if(NULL != stream)
   {
      const PaStreamInfo *info = Pa_GetStreamInfo(stream);
      if(NULL != info)
      {
         return (sF32)info->sampleRate;
      }
   }
   return 0.0f;
}

// (note) see https://developer.apple.com/documentation/audiotoolbox/understanding-audio-workgroups
// (note) see https://developer.apple.com/documentation/audiotoolbox/adding-parallel-real-time-threads-to-audio-workgroups
// (note) see https://forum.juce.com/t/macos-audio-thread-workgroups/53857/4
// (note) /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/System/Library/Frameworks/CoreAudio.framework/Headers/AudioHardwareDeprecated.h:821:1: note: 'AudioDeviceGetProperty' has been explicitly marked deprecated here
void _PaStream::_macOSAudioWorkgroupJoin(YAC_Value *_r) {
   _r->initNull();
#ifdef YAC_MACOS
   if(NULL != stream)
   {
      _PaStreamMacOSWorkgroupJoinToken *joinToken = YAC_NEW_POOLED(PaStreamMacOSWorkgroupJoinToken);
      if(NULL != joinToken)
      {
         AudioDeviceID audioDeviceID = PaMacCore_GetStreamOutputDevice(stream);

         UInt32 count = sizeof(os_workgroup_t);
         ::AudioDeviceGetProperty(audioDeviceID, 0, 0,
                                  kAudioDevicePropertyIOThreadOSWorkgroup, &count, &macos_audio_workgroup
                                  );
         int result = ::os_workgroup_join(macos_audio_workgroup, &joinToken->join_token);
         if( (0 == result) || (EALREADY == result) )
         {
            // Succeeded
            _r->initObject(joinToken, 1);
         }
         else
         {
            yac_host->printf("[---] PaStream::macOSAudioWorkgroupJoin: os_workgroup_join() failed (result=%d)\n", result);
            YAC_DELETE(joinToken);
         }
      }
      else
      {
         yac_host->printf("[---] PaStream::macOSAudioWorkgroupJoin: failed to allocate joinToken\n");
      }
   }
#endif // YAC_MACOS
}

void _PaStream::_macOSAudioWorkgroupLeave(YAC_Object *_joinToken) {
#ifdef YAC_MACOS
   if(NULL != stream)
   {
      if(YAC_BCHK(_joinToken, clid_PaStreamMacOSWorkgroupJoinToken))
      {
         YAC_CAST_ARG(_PaStreamMacOSWorkgroupJoinToken, joinToken, _joinToken);
         ::os_workgroup_leave(macos_audio_workgroup, &joinToken->join_token);
      }
   }
#else
   (void)_joinToken;
#endif // YAC_MACOS
}
