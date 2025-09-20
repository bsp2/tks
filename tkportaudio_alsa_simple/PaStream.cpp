
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>


#include "tkportaudio.h"

#include "PaStream.h"



static int stream_callback(const void *  input,
                           void *        output,
                           unsigned long frameCount,
                           _PaStream *   st
                           )
{
   YAC_FloatArray *ip = st->input_buf;


#if 0
   ip->num_elements = frameCount * st->num_input_channels;

   ////printf("xxx numin=%d framecount=%d\n", st->num_input_channels, frameCount);

   // Convert input samples to float format
   {
      sUI i;
      sUI numElem = ip->num_elements;
      sF32 *d = ip->elements;

      switch(99)
      {
         case 99:
         {
            // Silence
            sF32 *s = (sF32*) input;
            for(i=0; i<numElem; i++)
            {
               *d++ = 0.0f;
            }
         }
         break;

         case paFloat32:
         {
            sF32 *s = (sF32*) input;
            for(i=0; i<numElem; i++)
            {
               *d++ = *s++;
            }
         }
         break;

         case paInt32:
         {
            sSI *s = (sSI*) input;
            for(i=0; i<numElem; i++)
            {
               *d++ = (*s++ / 2147483647.0f);
            }
         }
         break;

         case paInt24:
         {
            sU8 *s = (sU8*) input;
            for(i=0; i<numElem; i++)
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
            sS8 *s = (sS8*) input;
            for(i=0; i<numElem; i++)
            {
               *d++ = (*s++) / 127.0f;
            }
         }
         break;

         case paInt16:
         {
            sS16 *s = (sS16*) input;
            for(i=0; i<numElem; i++)
            {
               *d++ = (*s++) / 32767.0f;
            }
         }
         break;

         case paUInt8:
         {
            sU8 *s = (sU8*) input;
            for(i=0; i<numElem; i++)
            {
               *d++ = (((sSI)(*s++))-128) / 127.0f;
            }
         }
         break;
      } // switch sampleformat
   }
#else

   // sample input not implemented, yet
   ip->num_elements = 0;

#endif // 0


   YAC_FloatArray *op = st->output_buf;
   op->num_elements = frameCount * PAR_CHANNELS;
   // fill output buffer with "silence"
   {
      sUI i;
      sUI numElem = op->num_elements;
      sF32 *d = op->elements;
      for(i=0; i<numElem; i++)
      {
         *d++ = 0.0f;
      }
   }

   // Call script fxn to fill output buffer
   YAC_Value args[4];
   args[0].initObject(st, 0);
   args[1].initObject(ip, 0);
   args[2].initObject(op, 0);
   args[3].initInt((sSI)frameCount);
   yac_host->yacEvalFunction(st->stream_cbk_context, st->stream_cbk, 4, args);

   // Convert outputbuffer to actual sampleformat
   {
      sUI i;
      sUI numElem = op->num_elements;
      sF32 *s = op->elements;

#define Dclipv sF32 v = *s++; if(v>1)v=1;else if(v<-1)v=-1
      switch(PAR_FORMAT)
      {
         case paFloat32:
         {
            sF32 *d = (sF32*) output;
            for(i=0; i<numElem; i++)
            {
               Dclipv;
               *d++ = *s++;
            }
         }
         break;

         case paInt32:
         {
            sSI *d = (sSI*) output;
            for(i=0; i<numElem; i++)
            {
               Dclipv;
               *d++ = (sSI) (v * 2147483647.0f);
            }
         }
         break;

         case paInt24:
         {
            sU8 *d = (sU8*) output;
            for(i=0; i<numElem; i++)
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
            sS16 *d = (sS16*) output;
            for(i=0; i<numElem; i++)
            {
               Dclipv;
               *d++ = (sS16) (v * 32767.0f);
            }
         }
         break;

         case paInt8:
         {
            sS8 *d = (sS8*) output;
            for(i=0; i<numElem; i++)
            {
               Dclipv;
               *d++ = (sS8) ( v * 127.0f );
            }
         }
         break;

         case paUInt8:
         {
            sU8 *d = (sU8*) output;
            for(i=0; i<numElem; i++)
            {
               Dclipv;
               *d++ = (sU8) ( v * 127.0f + 127.0f );
            }
         }
         break;
      } // switch sampleformat
   }

   return paContinue;
}


static void *pastream__threadentry(void *_userData) {

   _PaStream *st = (_PaStream*) _userData;

   // increase thread priority
   {
      struct sched_param param;
      memset((void*)&param, 0, sizeof(struct sched_param));
      param.sched_priority = 20;//_priority;
      int err = pthread_setschedparam(st->pthread_id, SCHED_FIFO, &param);
      if(0 != err)
      {
         printf("[---] PaStream: pthread_setschedparam() failed, errno=%d(\"%s\").\n", errno, strerror(errno));
      }
   }


   snd_pcm_sframes_t frames;

   short *buf = (short*) st->out_buf;
   memset(buf, 0, PAR_FRAGBYTESIZE);

   st->b_running = 1;

   while(st->b_running)
   {
      if(st->b_active)
      {
         // Calc sample data
         stream_callback(NULL, buf,
                         PAR_LATENCY, /* frameCount */
                         st
                         );

         // Write fragment
         frames = snd_pcm_writei(st->handle, buf, PAR_LATENCY);
         if (frames < 0)
            frames = snd_pcm_recover(st->handle, frames, 0);
         if (frames < 0) {
            printf("snd_pcm_writei failed: frames=%d\n", (int)frames);
            break;
         }
         if (frames > 0 && frames < (long)PAR_LATENCY)
            printf("Short write (expected %li, wrote %li)\n", (long)PAR_LATENCY, frames);
      }
      else
      {
         usleep(5000);
      }
   }

   return 0;
}


_PaStreamMacOSWorkgroupJoinToken::_PaStreamMacOSWorkgroupJoinToken(void) {
#ifdef YAC_MACOS
   ::memset(&join_token, 0, sizeof(os_workgroup_join_token_s));
#endif // YAC_MACOS
}

_PaStreamMacOSWorkgroupJoinToken::~_PaStreamMacOSWorkgroupJoinToken() {
}


_PaStream::_PaStream(void) {

   stream_cbk         = NULL;
   stream_cbk_context = NULL;

   b_running = 0;
   b_active  = 0;
   handle    = NULL;

   input_buf  = NULL;
   output_buf = NULL;
}

_PaStream::~_PaStream() {
   _abort();
   _close();
}

sBool _PaStream::initStream(void *_cbk,
                           _PaStreamParameters *_inParams,
                           _PaStreamParameters *_outParams
                           )
{

   if(PAR_FORMAT != _outParams->sample_format)
   {
      printf("[~~~] PaStream<alsa>::initStream: sample format mismatch (%d != %d). Forcing 16bit..\n",
             _outParams->sample_format,
             PAR_FORMAT
             );
//       return 0;
   }

   if(PAR_CHANNELS != _outParams->channel_count)
   {
      printf("[---] PaStream<alsa>::initStream: channel_count mismatch (%d != %d)\n",
             _outParams->channel_count,
             PAR_CHANNELS
             );
      return 0;
   }


   // Create script context
   stream_cbk         = _cbk;
   stream_cbk_context = yac_host->yacContextCreate();

   // Allocate 32bit float input buffer
   input_buf  = YAC_New_FloatArray();
   input_buf->yacArrayAlloc(PAR_LATENCY * PAR_CHANNELS);

   // Allocate 32bit float output buffer
   output_buf = YAC_New_FloatArray();
   output_buf->yacArrayAlloc(PAR_LATENCY * PAR_CHANNELS);

   int r;

   // Open device
   {
      int err;

      if ((err = snd_pcm_open(&handle, PAR_DEVNAME, SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
         printf("Playback open error: %s\n", snd_strerror(err));
         exit(EXIT_FAILURE);
      }
      if ((err = snd_pcm_set_params(handle,
                                    SND_PCM_FORMAT_S16,
                                    SND_PCM_ACCESS_RW_INTERLEAVED,
                                    2,
                                    PAR_RATE,
                                    1,
                                    (long) (((PAR_LATENCY*1000000.0f) / float(PAR_RATE))) // usec
                                    )) < 0) {
         printf("Playback open error: %s\n", snd_strerror(err));
         return 0;////exit(EXIT_FAILURE);
      }

   }

   printf("[dbg] #frag=%d latency=%d samples (%f millisec) rate=%d\n",
          PAR_FRAG,
          PAR_LATENCY,
          (((float)PAR_LATENCY) / PAR_RATE) * 1000,
          PAR_RATE
          );


   // Create worker thread
   {
      if(0 == pthread_create(&pthread_id, NULL, pastream__threadentry, (void*)this))
      {
         printf("[dbg] PaStream<alsa>::initStream: thread started.\n");

         // Succeeded
         return 1;
      }
      else
      {
         printf("[---] PaStream<alsa>::initStream: pthread_create() failed.\n");
         snd_pcm_close(handle);
         handle = NULL;
         return 0;
      }
   }

}

void _PaStream::_close(void) {
   if(b_running)
   {
      if(NULL != handle)
      {
         _stop();
         b_running = 0;

         // Wait until worker thread has exited
         pthread_join(pthread_id, NULL);

         // Close sound device
         snd_pcm_close(handle);
         handle = NULL;

         printf("[dbg] PaStream<alsa>::close: thread terminated.\n");
      }

      if(NULL != stream_cbk_context)
      {
         yac_host->yacContextDestroy(stream_cbk_context);
      }

      YAC_DELETE_SAFE(input_buf);

      YAC_DELETE_SAFE(output_buf);
   }
}

void _PaStream::_start(void) {
   b_active = 1;
}

void _PaStream::_stop(void) {
   b_active = 0;
}

void _PaStream::_abort(void) {
   //b_running = 0;
}

sSI _PaStream::_isStopped(void) {
   return b_active;
}

sSI _PaStream::_isActive(void) {
   return (NULL != handle);
}

sF32 _PaStream::_getCpuLoad(void) {
   return 0.0f;
}

sSI _PaStream::_getNumInputChannels(void) {
   return 0;//PAR_CHANNELS;
}

sSI _PaStream::_getNumOutputChannels(void) {
   return PAR_CHANNELS;
}

void _PaStream::_macOSAudioWorkgroupJoin(YAC_Value *_r) {
   _r->initNull();
}

void _PaStream::_macOSAudioWorkgroupLeave(YAC_Object *_joinToken) {
   (void)_joinToken;
}
