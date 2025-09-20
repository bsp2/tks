/*
 * Copyright (c) 2009-2024 Ross Bencina and Phil Burk, TkScript bindings by Bastian Spiegel
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


#include "PaDeviceInfo.h"
#include "PaStreamParameters.h"
#include "PaHostApiInfo.h"
#include "PaStream.h"
#include "PortAudio.h"


_PortAudio::_PortAudio(void) {
   is_initialized = YAC_FALSE;
}

_PortAudio::~_PortAudio() {
}

void _PortAudio::_Initialize(void) {
   if(!is_initialized)
   {
      Pa_Initialize();

      is_initialized = YAC_TRUE;
   }
}

sSI _PortAudio::_GetNumDevices(void) {
   return (sSI) Pa_GetDeviceCount();
}

void _PortAudio::_GetDeviceInfo(sUI _devIdx, YAC_Value *_r) {
   const PaDeviceInfo *c = Pa_GetDeviceInfo(_devIdx);
   if(NULL != c)
   {
      _PaDeviceInfo *o = (_PaDeviceInfo*) YAC_NEW(PaDeviceInfo);
      o->initDeviceInfo(_devIdx, c);
      _r->initObject(o, 1);
   }
   else
   {
      yac_host->printf("[---] PortAudio::getDeviceInfo: Pa_GetDeviceInfo() failed (devIdx=%d).\n", _devIdx);
   }
}

void _PortAudio::_ShowControlPanel(sUI _devIdx, sUI _hwnd) {
#ifdef YAC_WIN32
   yac_host->printf("call PaAsio_ShowControlPanel devIdx=%u hwnd=0x%08x\n", _devIdx, _hwnd);
   PaAsio_ShowControlPanel( (PaDeviceIndex) _devIdx, (void*)_hwnd/*systemSpecific=HWND*/ );
#endif // YAC_WIN32
}

sSI _PortAudio::_GetDefaultInputDeviceIndex(void) {
   return (sSI) Pa_GetDefaultInputDevice();
}

sSI _PortAudio::_GetDefaultOutputDeviceIndex(void) {
   return (sSI) Pa_GetDefaultOutputDevice();
}

sBool _PortAudio::_IsFormatSupported(YAC_Object *_inParam, YAC_Object *_outParam, sF32 _sampleRate) {
   if(YAC_CHK(_inParam, clid_PaStreamParameters))
   {
      if(YAC_CHK(_outParam, clid_PaStreamParameters))
      {
         YAC_CAST_ARG(_PaStreamParameters, isp, _inParam);
         YAC_CAST_ARG(_PaStreamParameters, osp, _outParam);

         return (sBool) Pa_IsFormatSupported(&isp->params, &osp->params, (double)_sampleRate);
      }
      else
      {
         Dyac_throw_def(InvalidPointer, "outParam is not a valid PaStreamParameters object");
         return YAC_FALSE;
      }
   }
   else
   {
      Dyac_throw_def(InvalidPointer, "inParam is not a valid PaStreamParameters object");
      return YAC_FALSE;
   }
}


static int stream_callback(const void *input, void *output,
                           unsigned long frameCount,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData )
{
   (void) timeInfo;
   (void) statusFlags;

   _PaStream *st = (_PaStream*) userData;

//    {
//       const unsigned short *s = (const unsigned short*) input;
//       for(int i=0; i<frameCount; i++)
//       {
//          if(s[i] != 0)
//          {
//             printf("s[%d]=%d\n", i, s[i]);
//          }
//       }
//    }

   YAC_FloatArray *ip = st->input_buf;
   ip->num_elements = frameCount * st->num_input_channels;

   // // printf("xxx numin=%d framecount=%d\n", st->num_input_channels, frameCount);

   // Convert input samples to float format
   {
      sUI i;
      sUI numElem = ip->num_elements;
      sF32 *d = ip->elements;

      switch(st->input_sample_format)
      {
         default:
            yac_host->printf("[---] tkportaudio: unhandled output_sample_format=%d\n", st->input_sample_format);
            break;

         case paFloat32:
         {
            const sF32 *s = (const sF32*) input;
            // yac_host->printf("xxx convert input paFloat32 numElem=%u\n", numElem);
            for(i = 0u; i < numElem; i++)
            {
               *d++ = *s++;
            }
         }
         break;

         case paInt32:
         {
            const sSI *s = (const sSI*) input;
            for(i = 0u; i < numElem; i++)
            {
               *d++ = (*s++ / 2147483647.0f);
            }
         }
         break;

         case paInt24:
         {
            //
            // note: m-audio delta 1010lt driver seems to be buggy -> only silence when 24bit input is selected
            //
            const sU8 *s = (const sU8*) input;
            for(i = 0u; i < numElem; i++)
            {
               sUI v = ((sUI)s[0]) | ((sUI)(s[1]<<8)) | ((sUI)(s[1]<<16));
               if(v & 0x800000)
                  v |= 0xFF000000;
               *d++ = ((*(sSI*)&v) / 8388607.0f);
               s += 3;
            }
         }
         break;

         case paInt8:
         {
            const sS8 *s = (const sS8*) input;
            for(i = 0u; i < numElem; i++)
            {
               *d++ = (*s++) / 127.0f;
            }
         }
         break;

         case paInt16:
         {
            const sS16 *s = (const sS16*) input;
            for(i = 0u; i < numElem; i++)
            {
               *d++ = (*s++) / 32767.0f;
            }
         }
         break;

         case paUInt8:
         {
            const sU8 *s = (const sU8*) input;
            for(i = 0u; i < numElem; i++)
            {
               *d++ = (((sSI)(*s++))-128) / 127.0f;
            }
         }
         break;
      } // switch sampleformat
   }



   YAC_FloatArray *op = st->output_buf;
   op->num_elements = frameCount * st->num_output_channels;
   // fill output buffer with "silence"
   {
      sUI numElem = op->num_elements;
      sF32 *d = op->elements;
      for(sUI i = 0u; i <numElem; i++)
      {
         *d++ = 0.0f;
      }
   }

   // (note) yacEvalFunction() automatically sets default context if there is none
   yac_host->yacContextSetDefault(st->stream_cbk_context);  // [25Jan2018] enabled

   YAC_Value args[4];
   args[0].initObject(st, 0);
   args[1].initObject(ip, 0);
   args[2].initObject(op, 0);
   args[3].initInt((sSI)frameCount);
   yac_host->yacEvalFunction(st->stream_cbk_context, st->stream_cbk, 4, args);


#if 1
   // Convert outputbuffer to actual sampleformat
   {
      sUI i;
      sUI numElem = op->num_elements;
      const sF32 *s = op->elements;

      if(numElem != (frameCount * st->num_output_channels))
      {
         yac_host->printf("[~~~] tkportaudio: convert output buffer: numElem(%u) does not match portaudio buffer size (%u, frameCount=%d numOutCh=%u). skipping..\n", numElem, (frameCount * st->num_output_channels), frameCount, st->num_output_channels);
      }
      else
      {
#define Dclipv sF32 v = *s++; if(v > 1.0f) v = 1.0f; else if(v < -1.0f) v = -1.0f
         switch(st->output_sample_format)
         {
            default:
               yac_host->printf("[---] tkportaudio: unhandled output_sample_format=%d\n", st->output_sample_format);
               break;

            case paFloat32:
            {
               sF32 *d = (sF32*) output;
               for(i = 0u; i < numElem; i++)
               {
                  Dclipv;
                  *d++ = v;
               }
            }
            break;

            case paInt32:
            {
               sSI *d = (sSI*) output;
               for(i = 0u; i < numElem; i++)
               {
                  Dclipv;
                  *d++ = (sSI) (v * 2147483647.0f);
               }
            }
            break;

            case paInt24:
            {
               sU8 *d = (sU8*) output;
               for(i = 0u; i < numElem; i++)
               {
                  Dclipv;
                  sUI vi = (sUI) (v * 8388607.0f);
                  d[0] = (vi&255);
                  d[1] = ((vi>>8)&255);
                  d[2] = ((vi>>16)&255);
                  d += 3;
               }
            }
            break;

            case paInt16:
            {
               sSI *d = (sSI*) output;
               for(i = 0u; i < numElem; i++)
               {
                  Dclipv;
                  *d++ = (sSI) (v * 32767.0f);
               }
            }
            break;

            case paInt8:
            {
               sS8 *d = (sS8*) output;
               for(i = 0u; i < numElem; i++)
               {
                  Dclipv;
                  *d++ = (sS8) ( v * 127.0f );
               }
            }
            break;

            case paUInt8:
            {
               sU8 *d = (sU8*) output;
               for(i = 0u; i < numElem; i++)
               {
                  Dclipv;
                  *d++ = (sU8) ( v * 127.0f + 127.0f );
               }
            }
            break;
         } // switch sampleformat
      }
   }
#endif

   return paContinue;
}


sBool _PortAudio::_OpenStream(YAC_Object *_inParams,
                              YAC_Object *_outParams,
                              sF32 _sampleRate,
                              sUI _framesPerBuffer,
                              YAC_Object *_streamCbk,
                              YAC_Object *_retStream
                              ) {
   if(YAC_CHK(_inParams, clid_PaStreamParameters))
   {
      YAC_CAST_ARG(_PaStreamParameters, isp, _inParams);

      if(YAC_CHK(_outParams, clid_PaStreamParameters))
      {
         YAC_CAST_ARG(_PaStreamParameters, osp, _outParams);

         if(YAC_VALID(_streamCbk)) //if(YAC_CHK(_streamCbk, YAC_CLID_FUNCTION))
         {
            if(YAC_CHK(_retStream, clid_PaStream))
            {
               YAC_CAST_ARG(_PaStream, s, _retStream);

               // Clamp max. channels
               if(isp->params.channelCount > TKPORTAUDIO_MAX_CHANNELS)
                  isp->params.channelCount = TKPORTAUDIO_MAX_CHANNELS;

               if(osp->params.channelCount > TKPORTAUDIO_MAX_CHANNELS)
                  osp->params.channelCount = TKPORTAUDIO_MAX_CHANNELS;

               PaStream *stream;
               PaError err = Pa_OpenStream(&stream,
                                           (isp->params.channelCount > 0) ? &isp->params : NULL,
                                           (osp->params.channelCount > 0) ? &osp->params : NULL,
                                           (double)_sampleRate,
                                           _framesPerBuffer,// 0 == paFramesPerBufferUnspecified
                                           0, // StreamFlags
                                           stream_callback,
                                           (void*)s // User data
                                           );

               if(paNoError == err)
               {
                  s->initStream(stream,
                                (void*)_streamCbk,
                                isp->params.channelCount, osp->params.channelCount,
                                isp->params.sampleFormat, osp->params.sampleFormat
                                );

                  // Succeeded
                  return YAC_TRUE;
               }
               else
               {
                  yac_host->printf("[---] PortAudio::openStream: Pa_OpenStream() failed (err=%d).\n", err);
               }

            }
            else
            {
               Dyac_throw_def(InvalidPointer, "retStream is not a valid PaStream object");
            }
         }
         else
         {
            Dyac_throw_def(InvalidPointer, "streamCbk is not a valid Function object");
         }
      }
      else
      {
         Dyac_throw_def(InvalidPointer, "outParams is not a valid PaStreamParameters object");
      }
   }
   else
   {
      Dyac_throw_def(InvalidPointer, "inParams is not a valid PaStreamParameters object");
   }
   return YAC_FALSE;
}

sSI _PortAudio::_GetSampleSize(sSI _format) {
   return (sSI) Pa_GetSampleSize((PaSampleFormat)_format);
}

void _PortAudio::_Terminate(void) {
   if(is_initialized)
   {
      is_initialized = YAC_FALSE;

      Pa_Terminate();
   }
}
