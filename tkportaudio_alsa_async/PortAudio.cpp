
#include "tkportaudio.h"


#include "PaDeviceInfo.h"
#include "PaStreamParameters.h"
#include "PaHostApiInfo.h"
#include "PaStream.h"
#include "PortAudio.h"


static sBool loc_test_sample_format(snd_pcm_t *_pcm, snd_pcm_hw_params_t *_hwParams, sSI _paFormat, snd_pcm_format_t _pcmFormat, sSI *_retSampleFormat, snd_pcm_format_t *_retPCMFormat) {
   if(snd_pcm_hw_params_test_format(_pcm, _hwParams, _pcmFormat) >= 0)
   {
      *_retSampleFormat = _paFormat;
      *_retPCMFormat    = _pcmFormat;
      return YAC_TRUE;
   }
   return YAC_FALSE;
}

_PortAudio::_PortAudio(void) {
   memset(device_infos, 0, sizeof(device_infos));
   num_devices = 0u;
}

_PortAudio::~_PortAudio() {
   for(sUI i = 0u; i < MAX_DEVICE_INFOS; i++)
   {
      YAC_DELETE_SAFE(device_infos[i]);
   }
}

void _PortAudio::_Initialize(void) {
}

void _PortAudio::lazyEnumerateDevices(void) {
   if(num_devices > 0u)
      return;

   // adapted from portaudio/src/hostapi/alsa/pa_linux_alsa.c
   /*
     * snd_card_next() modifies the integer passed to it to be:
     *   the index of the first card if the parameter is -1
     *   the index of the next card if the parameter is the index of a card
     *   -1 if there are no more cards
     *
     * The function itself returns 0 if it succeeded. */
   int cardIdx = -1;
   // int res;
   // int blocking = SND_PCM_NONBLOCK;
   // int usePlughw = 0;
   const char *hwPrefix = "";
   char alsaCardName[128];

   snd_ctl_card_info_t *cardInfo;
   snd_ctl_card_info_alloca(&cardInfo);

   snd_pcm_info_t *pcmInfo;
   snd_pcm_info_alloca(&pcmInfo);

   snd_pcm_hw_params_t *hwParams;
   snd_pcm_hw_params_alloca(&hwParams);

   // Enumerate cards
   while( 0 == snd_card_next(&cardIdx) && cardIdx >= 0 )
   {
      snprintf(alsaCardName, sizeof(alsaCardName), "hw:%d", cardIdx);

      // Acquire name of card
      snd_ctl_t *ctl;
      if(snd_ctl_open(&ctl, alsaCardName, 0) < 0)
      {
         // Unable to open card :(
         yac_host->printf("[---] PortAudio<alsa_async>::enumerateDevices: unable to open card %s\n", alsaCardName);
         continue;
      }
      snd_ctl_card_info(ctl, cardInfo);
      const char *cardInfoName = snd_ctl_card_info_get_name(cardInfo);

      // Enumerate card devices
      int devIdx = -1;
      while( 0 == snd_ctl_pcm_next_device(ctl, &devIdx) && devIdx >= 0 )
      {
         // // char *alsaDeviceName, *deviceName;

         // snprintf( buf, sizeof (buf), "%s%s,%d", hwPrefix, alsaCardName, devIdx );
         // snprintf( buf, sizeof (buf), "%s%s,%d", hwPrefix, alsaCardName, devIdx );

         /* Obtain info about this particular device */
         snd_pcm_info_set_device(pcmInfo, devIdx);
         snd_pcm_info_set_subdevice(pcmInfo, 0);

         snd_pcm_info_set_stream(pcmInfo, SND_PCM_STREAM_CAPTURE);
         sBool hasCapture = (snd_ctl_pcm_info(ctl, pcmInfo) >= 0);
         
         snd_pcm_info_set_stream(pcmInfo, SND_PCM_STREAM_PLAYBACK);
         sBool hasPlayback = (snd_ctl_pcm_info(ctl, pcmInfo) >= 0);

         // if( !hasPlayback && !hasCapture )
         (void)hasCapture;
         if(!hasPlayback)
         {
            // Error
            continue;
         }

         char hwName[256];  // for snd_pcm_open()
         snprintf(hwName, sizeof(hwName), "%s%s,%d", hwPrefix, alsaCardName, devIdx);

         char dpyName[256]; // display name (does NOT include internal device index)
         const char *pcmInfoName = snd_pcm_info_get_name(pcmInfo);
         if(!strcmp(cardInfoName,pcmInfoName))  // filter "Session I/O,Session I/O"
            snprintf(dpyName, sizeof(dpyName), "%s", cardInfoName);
         else
            snprintf(dpyName, sizeof(dpyName), "%s,%s", cardInfoName, pcmInfoName);

         sUI sampleRate = 0;
         sSI sampleFormat = paFloat32;
         snd_pcm_format_t sndSmpFormat = SND_PCM_FORMAT_UNKNOWN;
         sUI numIn = 0u;
         sUI numOut = 0u;
         sF32 defMinInputLatency = 0.0f;  // (todo)
         sF32 defMaxInputLatency = 0.0f;  // (todo)
         sF32 defMinOutputLatency = 0.0f;
         sF32 defMaxOutputLatency = 0.0f;

         // Open device and query rate/#channels
         {
            snd_pcm_t *pcm;
            int err;

            err = snd_pcm_open(&pcm, hwName, SND_PCM_STREAM_CAPTURE, 0);
            if(err >= 0)
            {
               snd_pcm_hw_params_any(pcm, hwParams);
               
               snd_pcm_hw_params_get_channels_max(hwParams, &numIn);
               // yac_host->printf("xxx open capture dev, numIn=%u\n", numIn);

               snd_pcm_close(pcm);
            }

            err = snd_pcm_open(&pcm, hwName, SND_PCM_STREAM_PLAYBACK, 0);
            if(err >= 0)
            {
               snd_pcm_uframes_t bufferSizeMin = 0u;
               snd_pcm_uframes_t bufferSizeMax = 0u;
               snd_pcm_uframes_t periodSizeMin = 0u;
               snd_pcm_uframes_t periodSizeMax = 0u;

#if 0
               // Query ringbuffer + period sizes
               //  ==> err=-77("File descriptor in bad state")
               err = snd_pcm_get_params(pcm, &bufferSize, &periodSize);
               if(err >= 0)
               {
               }
               else
               {
                  yac_host->printf("[~~~] PortAudio<alsa_async>::enumerateDevices: snd_pcm_get_params() failed. err=%d(\"%s\")\n", err, snd_strerror(err));
               }
#endif

               snd_pcm_hw_params_any(pcm, hwParams);

               // Find default sample format
               {
                  // (todo) support more formats
                  loc_test_sample_format(pcm, hwParams, paFloat32, SND_PCM_FORMAT_FLOAT_LE, &sampleFormat, &sndSmpFormat);
                  // e.g. Scarlett 18i20 2nd gen
                  loc_test_sample_format(pcm, hwParams, paInt32,   SND_PCM_FORMAT_S32_LE,   &sampleFormat, &sndSmpFormat);
                  loc_test_sample_format(pcm, hwParams, paInt32,   SND_PCM_FORMAT_S32_BE,   &sampleFormat, &sndSmpFormat);
                  // e.g. Native Instruments Session I/O
                  loc_test_sample_format(pcm, hwParams, paInt24,   SND_PCM_FORMAT_S24_3BE,  &sampleFormat, &sndSmpFormat);
                  loc_test_sample_format(pcm, hwParams, paInt24,   SND_PCM_FORMAT_S24_3LE,  &sampleFormat, &sndSmpFormat);
                  loc_test_sample_format(pcm, hwParams, paInt16,   SND_PCM_FORMAT_S16_LE,   &sampleFormat, &sndSmpFormat);
                  loc_test_sample_format(pcm, hwParams, paInt16,   SND_PCM_FORMAT_S16_BE,   &sampleFormat, &sndSmpFormat);
                  loc_test_sample_format(pcm, hwParams, paInt8,    SND_PCM_FORMAT_S8,       &sampleFormat, &sndSmpFormat);
                  loc_test_sample_format(pcm, hwParams, paUInt8,   SND_PCM_FORMAT_U8,       &sampleFormat, &sndSmpFormat);

                  if(SND_PCM_FORMAT_UNKNOWN == sndSmpFormat)
                  {
                     yac_host->printf("[~~~] PortAudio<alsa_async>::enumerateDevices: unsupported sample format (hwName=\"%s\")\n", hwName);
                  }
               }
               
               snd_pcm_hw_params_get_channels_max(hwParams, &numOut);

#if 1
               // Prefer rate=48000Hz, check if it's supported
               {
                  err = snd_pcm_hw_params_test_rate(pcm, hwParams, 48000u, 0/*dir*/);
                  if(err >= 0)
                     sampleRate = 48000u;
               }
#endif

#if 1
               if(0u == sampleRate)
               {
                  sUI rateMin = 0u;
                  err = snd_pcm_hw_params_get_rate_min(hwParams, &rateMin, 0/*dir*/);
                  if(err >= 0)
                  {
                     sampleRate = rateMin;
                  }
                  else
                  {
                     yac_host->printf("[~~~] PortAudio<alsa_async>::enumerateDevices: snd_pcm_hw_params_get_rate_min() failed. err=%d(\"%s\")\n", err, snd_strerror(err));
                  }
               }

               if(0u == sampleRate)
               {
                  sUI rateMax = 0u;
                  err = snd_pcm_hw_params_get_rate_max(hwParams, &rateMax, 0/*dir*/);
                  if(err >= 0)
                  {
                     sampleRate = rateMax;
                  }
                  else
                  {
                     yac_host->printf("[~~~] PortAudio<alsa_async>::enumerateDevices: snd_pcm_hw_params_get_rate_max() failed. err=%d(\"%s\")\n", err, snd_strerror(err));
                  }
               }

#else
               // (note) fails for most devices (but is expected behaviour according to docs)
               {
                  sUI rateNum = 0u;
                  sUI rateDen = 0u;
                  err = snd_pcm_hw_params_get_rate_numden(hwParams, &rateNum, &rateDen);
                  if(err >= 0)
                  {
                     sampleRate = rateNum;
                     (void)rateDen;
                  }
                  else
                  {
                     yac_host->printf("[~~~] PortAudio<alsa_async>::enumerateDevices: snd_pcm_hw_params_get_rate_numden() failed. err=%d(\"%s\")\n", err, snd_strerror(err));
                  }
               }
#endif

               // Prefer min bufferSize=128. Check if it's supported
               err = snd_pcm_hw_params_test_buffer_size(pcm, hwParams, 128u);
               if(err >= 0)
                  bufferSizeMin = 128u;

               // Prefer max bufferSize=512. Check if it's supported
               err = snd_pcm_hw_params_test_buffer_size(pcm, hwParams, 512u);
               if(err >= 0)
                  bufferSizeMax = 512u;

               // Prefer min periodSize=64. Check if it's supported
               err = snd_pcm_hw_params_test_period_size(pcm, hwParams, 64u, 0/*dir*/);
               if(err >= 0)
                  periodSizeMin = periodSizeMax = 64u;

               // Prefer max periodSize=128. Check if it's supported
               err = snd_pcm_hw_params_test_period_size(pcm, hwParams, 128u, 0/*dir*/);
               if(err >= 0)
                  periodSizeMax = 128u;

#if 1
               // Query ringbuffer min/max size
               // Query period min/max size
               if(0u == bufferSizeMin)
               {
                  err = snd_pcm_hw_params_get_buffer_size_min(hwParams, &bufferSizeMin);
                  if(err < 0)
                  {
                     yac_host->printf("[~~~] PortAudio<alsa_async>::enumerateDevices: snd_pcm_hw_params_get_buffer_size_min() failed. err=%d(\"%s\")\n", err, snd_strerror(err));
                  }
                  else
                  {
                     yac_host->printf("[trc] PortAudio<alsa_async>::enumerateDevices: bufferSizeMin=%u\n", bufferSizeMin);
                  }
               }

               if(0u == bufferSizeMax)
               {
                  err = snd_pcm_hw_params_get_buffer_size_max(hwParams, &bufferSizeMax);
                  if(err < 0)
                  {
                     yac_host->printf("[~~~] PortAudio<alsa_async>::enumerateDevices: snd_pcm_hw_params_get_buffer_size_max() failed. err=%d(\"%s\")\n", err, snd_strerror(err));
                  }
                  else
                  {
                     yac_host->printf("[trc] PortAudio<alsa_async>::enumerateDevices: bufferSizeMax=%u\n", bufferSizeMax);
                  }
               }

               // Query period min/max size
               if(0u == periodSizeMin)
               {
                  int dir = 0;
                  err = snd_pcm_hw_params_get_period_size_min(hwParams, &periodSizeMin, &dir);
                  if(err < 0)
                  {
                     yac_host->printf("[~~~] PortAudio<alsa_async>::enumerateDevices: snd_pcm_hw_params_get_period_size_min() failed. err=%d(\"%s\")\n", err, snd_strerror(err));
                  }
               }

               if(0u == periodSizeMax)
               {
                  int dir = 0;
                  err = snd_pcm_hw_params_get_period_size_max(hwParams, &periodSizeMax, &dir);
                  if(err < 0)
                  {
                     yac_host->printf("[~~~] PortAudio<alsa_async>::enumerateDevices: snd_pcm_hw_params_get_period_size_max() failed. err=%d(\"%s\")\n", err, snd_strerror(err));
                  }
               }
#endif

               if(sampleRate > 0u)
               {
                  defMinOutputLatency = bufferSizeMin / sF32(sampleRate);
                  defMaxOutputLatency = bufferSizeMax / sF32(sampleRate);
               }

               snd_pcm_close(pcm);
            }
            else
            {
               yac_host->printf("[~~~] PortAudio<alsa_async>::enumerateDevices: snd_pcm_open(hwName=\"%s\") failed. err=%d(\"%s\")\n", hwName, err, snd_strerror(err));
            }
         }

         if(sampleRate > 0u && numOut > 0u)
         {
            // Store in PaDeviceInfo
            _PaDeviceInfo *paDeviceInfo = YAC_NEW_(PaDeviceInfo);
            device_infos[num_devices] = paDeviceInfo;
            paDeviceInfo->initDeviceInfo(hwName, dpyName,
                                         sampleRate,
                                         sampleFormat,
                                         sndSmpFormat,
                                         numIn, numOut,
                                         defMinInputLatency, defMaxInputLatency,
                                         defMinOutputLatency, defMaxOutputLatency
                                         );
            num_devices++;
         }

      }  // while snd_ctl_pcm_next_device()

      snd_ctl_close( ctl );

   } // while snd_card_next()

}

sUI _PortAudio::_GetNumDevices(void) {
   lazyEnumerateDevices();
   return num_devices;
}

void _PortAudio::_GetDeviceInfo(sUI _devIdx, YAC_Value *_r) {
   lazyEnumerateDevices();
   if(_devIdx >= num_devices)
   {
      _r->initNull();
      return;
   }
   _r->initObject(device_infos[_devIdx], 0);
}

void _PortAudio::_ShowControlPanel(sUI _devIdx, sUI _hwnd) {
   (void)_devIdx;
   (void)_hwnd;
   ////PaAsio_ShowControlPanel( (PaDeviceIndex) _devIdx, NULL/*systemSpecific=HWND*/ );
}

sSI _PortAudio::_GetDefaultInputDeviceIndex(void) {
   return 0;
}

sSI _PortAudio::_GetDefaultOutputDeviceIndex(void) {
   return 0;
}

sSI _PortAudio::_IsFormatSupported(YAC_Object *_inParam, YAC_Object *_outParam, sF32 _sampleRate) {
   // (todo) implement me properly
   (void)_inParam;
   (void)_outParam;
   // if( PAR_RATE == _sampleRate )
   {
      return 1;
   }
   // else
   // {
   //    return 0;
   // }
}


sSI _PortAudio::_OpenStream(YAC_Object *_inParams,
                            YAC_Object *_outParams,
                            sF32        _sampleRate,
                            sUI         _framesPerBuffer,
                            YAC_Object *_streamCbk,
                            YAC_Object *_retStream
                            ) {
   lazyEnumerateDevices();
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
               
               if(s->initStream(this, (void*)_streamCbk, isp, osp, (sUI)_sampleRate, _framesPerBuffer))
               {
                  // Succeeded
                  return 1;
               }
               else
               {
                  yac_host->printf("[---] PortAudio<alsa_async>::openStream: initStream() failed.\n");
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
   return 0;
}

sSI _PortAudio::_GetSampleSize(sSI _format) {
   switch(_format)
   {
      case paInt8:    return 1;
      case paInt16:   return 2;
      case paInt24:   return 3;
      case paInt32:
      case paFloat32: return 4;
   }
   yac_host->printf("[~~~] PortAudio<alsa_async>::GetSampleSize: unsupported format %d\n", _format);
   return 0;
}

void _PortAudio::_Terminate(void) {
}
