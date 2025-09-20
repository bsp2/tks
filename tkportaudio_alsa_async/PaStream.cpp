
// (note) ALSA async callbacks use signals. signals are somewhat parallel to pthreads
//        and the signalcode will run in the current thread, possibly causing deadlocks.
//        to avoid this, the open() method creates a new thread, which sigmask is set to
//        allow only the ALSA signal.


// (note) async support seems broken (random lockups)
// #define USE_ASYNC  defined
// #define USE_MUTEX  defined

// #define Dprintf if(0);else yac_host->printf
#define Dprintf if(1);else yac_host->printf

#include <stdio.h>
#include <string.h>
#include <math.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <signal.h>
#include <dlfcn.h>

#include "tkportaudio.h"

#include "PaStream.h"

#ifdef YAC_LINUX
#include <syscall.h>
#endif


#ifdef USE_ASYNC
static sUI async_cbk_count_in  = 0u;
static sUI async_cbk_count_out = 0u;
#endif // USE_ASYNC


#if 0
static struct timeval tv_last;
static sF64 GetMilliSecondsDouble(void) {

   struct timeval tv;
   ::gettimeofday(&tv, NULL);

   sF32 msDelta = (sF64) ((tv.tv_sec-tv_last.tv_sec)*1000 + ((tv.tv_usec-tv_last.tv_usec) / 1000.0));

   tv_last = tv;

   return msDelta;
}
#endif


#ifdef PANDORA_LOWLATENCY_HACK
typedef int (*snd_pcm_hw_set_hack_params_fxn_t)(snd_pcm_t *pcm, unsigned int frames_min, unsigned int frames_max);

static sBool loc_perform_hack(snd_pcm_t *handle, sUI _minBuf, sUI _maxBuf)
{
   // by Grazvydas Ignotas ("notaz")
	snd_pcm_hw_set_hack_params_fxn_t snd_pcm_hw_set_hack_params;
	int ret;
   void *dlHandle = NULL;

   // return YAC_FALSE;

   // dlHandle = dlopen("/usr/lib/libasound.so.2", RTLD_LAZY);

   // if(NULL != dlHandle)
   {
      snd_pcm_hw_set_hack_params = (snd_pcm_hw_set_hack_params_fxn_t) dlsym(dlHandle, "snd_pcm_hw_set_hack_params");

      if(NULL != snd_pcm_hw_set_hack_params)
      {
         ret = snd_pcm_hw_set_hack_params(handle, _minBuf, _maxBuf);

         yac_host->printf("[dbg] PaStream<alsa_async>: set hack params: %d(\"%s\")\n", ret, strerror(-ret));

         return (0 == ret);
      }
      else
      {
         yac_host->printf("[~~~] PaStream<alsa_async>: no pandora hack in libasound.\n");
      }

      // dlclose(dlHandle);
   }
   // else
   // {
   //    yac_host->printf("[~~~] PaStream<alsa_async>: failed to dlopen() libasound.\n");
   // }

   return YAC_FALSE;
}
#endif /* PANDORA_LOWLATENCY_HACK */


static void *in__threadentry(void *_userData) {
   _PaStream *st = (_PaStream*) _userData;

   st->b_in_running = YAC_TRUE;
   while(*(volatile sBool*)&st->b_in_running)
   {
      st->processInput(YAC_TRUE/*bVerbose*/);
   }

   return NULL;
}

static void *out__threadentry(void *_userData) {
   _PaStream *st = (_PaStream*) _userData;

   st->b_out_running = YAC_TRUE;
   while(*(volatile sBool*)&st->b_out_running)
   {
      st->processOutput(YAC_TRUE/*bVerbose*/);
   }

   return NULL;
}


static void *pastream__threadentry(void *_userData) {
   _PaStream *st = (_PaStream*) _userData;
   int err;
   sUI periodCount = 0u;
   yac_host = st->yac_host_obj;  // (todo) remove

#ifdef YAC_LINUX
   Dprintf("[dbg] PaStream::threadentry: threadId=" YAC_PRINTF_PTRPREFIX "%p LWPid=%d\n", pthread_self(), syscall(SYS_gettid));
#endif

   // increase thread priority
   if(0)
   {
      struct sched_param param;

      memset((void*)&param, 0, sizeof(struct sched_param));
      param.sched_priority = 80;//_priority;
      ////pthread_setschedparam(st->pthread_id, SCHED_OTHER, &param);
      err = pthread_setschedparam(st->pthread_id, SCHED_FIFO, &param);
      if(0 != err)
      {
         yac_host->printf("[---] PaStream: pthread_setschedparam() failed, errno=%d(\"%s\")\n", errno, strerror(errno));
      }
   }

#ifdef USE_ASYNC
   // Allow only ALSA I/O signal
   //  (note) there must not be any other thread that allows this signal!
   //  (note) the TKS runtime disallows SIGIO by default
   {
      sigset_t ss;
      sigfillset(&ss);
      pthread_sigmask(SIG_BLOCK, &ss, NULL); // block all signals..
      sigemptyset(&ss);
      sigaddset(&ss, SIGIO);
      pthread_sigmask(SIG_UNBLOCK, &ss, NULL); // ..except for SIGIO
   }
#endif // USE_ASYNC

   st->b_running = YAC_TRUE;

   while(*(volatile sBool*)&st->b_running)
   {
#ifdef USE_MUTEX
      Dprintf("xxx audio tick: try lock mutex\n");
      yac_host->yacMutexLock(st->mtx_async);
      Dprintf("xxx audio tick: mutex locked\n");
#endif // USE_MUTEX

      // sUI sleepLeft = sleep(1); // (note) sleep() gets interrupted by SIGIO from alsa kernel module

      // if(0u == sleepLeft)
      {
         if(st->b_running && (NULL != st->in_handle))
         {
            // Dprintf("xxx call snd_pcm_state()\n");
            snd_pcm_state_t pcmState = snd_pcm_state(st->in_handle);
            Dprintf("xxx snd_pcm_state<in>=%d\n", pcmState);
            if(SND_PCM_STATE_PREPARED == pcmState)
            {
               snd_pcm_start(st->in_handle);
            }
         }

         if(st->b_running)
         {
            snd_pcm_state_t pcmState = snd_pcm_state(st->out_handle);
            Dprintf("xxx snd_pcm_state<out>=%d\n", pcmState);

            sUI inAvail  = (sUI)yac_host->yacAtomicInc((volatile int*)&st->in_ringbuf_avail,  0/*inc*/);
            sUI outAvail = (sUI)yac_host->yacAtomicInc((volatile int*)&st->out_ringbuf_avail, 0/*inc*/);
            Dprintf("[trc] tkportaudio<alsa_async>:thread: numInCh=%u numOutCh=%u inAvail=%u outAvail=%u\n", st->num_in_ch, st->num_out_ch, inAvail, outAvail);
            sUI maxRingAvail = st->num_periods * st->period_size;
            Dprintf("[trc] tkportaudio<alsa_async>:thread: numInCh=%u numOutCh=%u inAvail=%u outAvail=%u maxRingAvail=%u\n", st->num_in_ch, st->num_out_ch, inAvail, outAvail, maxRingAvail);

            if( (( (maxRingAvail - outAvail) >= st->period_size) && ( (0u == st->num_in_ch) || (inAvail >= st->period_size) ))
                || (outAvail < st->period_size)
                )
            {
               sBool bVerbose = 1 || (0u == (periodCount & 255u));
               periodCount++;

               if(bVerbose)
               {
                  Dprintf("[trc] tkportaudio<alsa_async>:thread: periodCount=%u numInCh=%u numOutCh=%u inAvail=%u outAvail=%u periodSize=%u\n", periodCount, st->num_in_ch, st->num_out_ch, inAvail, outAvail, st->period_size);
               }

               // Copy in_ringbuf data to flat array
               if(st->num_in_ch > 0u)
               {
                  YAC_FloatArray *ip = st->input_buf;
                  sF32 *d = ip->elements;
                  const sUI numSamples = ip->num_elements = st->period_size * st->num_in_ch;
                  if(inAvail >= st->period_size)
                  {
                     const sF32 *s = st->in_ringbuf;
                     for(sUI i = 0u; i < numSamples; i++)
                     {
                        d[i] = s[st->in_ringbuf_readoff];
                        st->in_ringbuf_readoff = (st->in_ringbuf_readoff + 1u) & (TKPORTAUDIO_RINGSIZE - 1u);
                     }
                     (void)yac_host->yacAtomicDec((volatile int*)&st->in_ringbuf_avail, st->period_size/*dec*/);
                  }
                  else
                  {
                     memset((void*)d, 0, sizeof(sF32) * numSamples);
                     Dprintf("[~~~] tkportaudio<alsa_async>:thread: input underrun: inAvail=%u req=%u. filling with zero..\n", inAvail, st->period_size);
                  }
               }

               // Calculate new output data
               Dprintf("xxx thread: begin callStreamCallback\n");
               st->callStreamCallback(st->period_size);
               Dprintf("xxx thread: END callStreamCallback\n");

               // Write output data to out_ringbuf
               {
                  YAC_FloatArray *op = st->output_buf;
                  const sF32 *s = op->elements;
                  const sUI numSamples = op->num_elements;
                  sF32 *d = st->out_ringbuf;
                  for(sUI i = 0u; i < numSamples; i++)
                  {
                     d[st->out_ringbuf_writeoff] = s[i];
                     st->out_ringbuf_writeoff = (st->out_ringbuf_writeoff + 1u) & (TKPORTAUDIO_RINGSIZE - 1u);
                  }
                  (void)yac_host->yacAtomicInc((volatile int*)&st->out_ringbuf_avail, st->period_size/*inc*/);
               }

               Dprintf("xxx thread: added %u frames to out_ringbuf\n", st->period_size);

            }

            if(SND_PCM_STATE_RUNNING == pcmState)
            {
#if 0
               Dprintf("[dbg] tkportaudio<alsa_async>:thread: pcmState=SND_PCM_STATE_RUNNING, async_cbk_count_out=%u avgAvail=%.3f\n",
                       async_cbk_count_out,
                       st->dbg_avg_avail
                       );
#endif
            }
            else if(SND_PCM_STATE_XRUN == pcmState)
            {
               yac_host->printf("[~~~] tkportaudio<alsa_async>:thread: detected SND_PCM_STATE_XRUN, (re-)preparing stream..\n");

               err = snd_pcm_prepare(st->out_handle);

               if(err < 0)
               {
                  yac_host->printf("[~~~] tkportaudio<alsa_async>:thread: snd_pcm_prepare() failed. err=%d(\"%s\").\n",
                                   err,
                                   snd_strerror(err)
                                   );
               }
            }
            else if(SND_PCM_STATE_PREPARED == pcmState)
            {
               yac_host->printf("[dbg] tkportaudio<alsa_async>:thread: detected SND_PCM_STATE_PREPARED, (re-)starting stream..\n");

               // (note) MUST write data or stream will stay in PREPARED state
               memset(st->out_buf, 0, TKPORTAUDIO_MAX_BUFSIZE);
               for(sUI i = 0u; i < st->num_periods; i++)
               {
                  snd_pcm_writei(st->out_handle, st->out_buf, st->period_size);
               }
               Dprintf("[trc] tkportaudio<alsa_async>:thread: SND_PCM_STATE_PREPARED: snd_pcm_writei() returned\n");
            }
            else if(SND_PCM_STATE_PAUSED == pcmState)
            {
               if(st->b_active)
               {
                  err = snd_pcm_start(st->out_handle);
                  if(err < 0)
                  {
                     yac_host->printf("[~~~] tkportaudio<alsa_async>:thread: snd_pcm_start() failed. err=%d(\"%s\").\n",
                                      err,
                                      snd_strerror(err)
                                      );
                  }
               }
            }
            else if(SND_PCM_STATE_SETUP == pcmState)
            {
               // during shutdown
            }
            else
            {
#ifdef USE_ASYNC
               Dprintf("[dbg] tkportaudio<alsa_async>:thread: pcmState=%d, async_cbk_count_out=%u\n", pcmState, async_cbk_count_out);
#else
               Dprintf("[dbg] tkportaudio<alsa_async>:thread: pcmState=%d\n", pcmState);
#endif // USE_ASYNC
            }
         } // if st->b_running
      }
      // else
      // {
      //    if(0 == (async_cbk_count & 1023))
      //    {
      //       Dprintf("[trc] PaStream: async_cbk_count=%u avgAvail=%.3f\n", async_cbk_count, st->dbg_avg_avail);
      //    }
      // }

#ifdef USE_MUTEX
      Dprintf("xxx thread: unlock mutex\n");
      yac_host->yacMutexUnlock(st->mtx_async);
#endif // USE_MUTEX

      yac_host->yacThreadYield();
   } // while st->b_running

   return NULL;
}

#ifdef USE_ASYNC
static void async__cbk_in(snd_async_handler_t *_handler) {
   // (note) this callback is always invoked in the context of the thread created by open()
   // (note) may not use malloc/free (?)
   _PaStream *st = (_PaStream*) snd_async_handler_get_callback_private(_handler);
   yac_host = st->yac_host_obj;  // (todo) remove

   Dprintf("[trc] async__cbk_in: ENTER async_cbk_count_in=%u\n", async_cbk_count_in);
   sBool bVerbose = 1 || (0u == (async_cbk_count_in & 255u));
   async_cbk_count_in++;


   if(st->b_active && (NULL != st->in_handle))
   {
      st->processInput(bVerbose);
   } // if active

   Dprintf("xxx async__cbk_in: LEAVE\n");
}

static void async__cbk_out(snd_async_handler_t *_handler) {
   // (note) this callback is always invoked in the context of the thread created by open()
   // (note) may not use malloc/free (?)

   _PaStream *st = (_PaStream*) snd_async_handler_get_callback_private(_handler);

#ifdef USE_MUTEX
   yac_host->yacMutexLock(st->mtx_async);
#endif // USE_MUTEX

   sBool bVerbose = 1 || (0u == (async_cbk_count_out & 255u));
   async_cbk_count_out++;

   if(bVerbose)
   {
      Dprintf("[trc] async__cbk_out: yac_host=%p async_cbk_count_out=%u\n", yac_host, async_cbk_count_out);
   }

#if 0
   static sBool bFirstFrag = 1;

   if(bFirstFrag)
   {
      struct sched_param param;
      int err;
      int policy;

      bFirstFrag = 0;

      memset((void*)&param, 0, sizeof(struct sched_param));
      err = pthread_getschedparam(pthread_self(), &policy, &param);
      Dprintf("[trc] async__cbk_out: param.sched_priority=%d policy=%d (SCHED_FIFO=%d) err=%d\n", param.sched_priority, policy, SCHED_FIFO, err);
      // ==> ok, prio=80, policy=1=SCHED_FIFO
   }
#endif

   if(st->b_active && (NULL != st->in_handle))
   {
      Dprintf("xxx async__cbk_out: begin processInput\n");
      st->processInput(bVerbose);
      Dprintf("xxx async__cbk_out: END processInput\n");
   }

   if(st->b_active && (NULL != st->out_handle))
   {
      Dprintf("xxx async__cbk_out: begin processOutput\n");
      st->processOutput(bVerbose);
      Dprintf("xxx async__cbk_out: END processOutput\n");
   } // if active

#ifdef USE_MUTEX
   yac_host->yacMutexUnlock(st->mtx_async);
#endif // USE_MUTEX

   if(bVerbose)
   {
      Dprintf("xxx async__cbk_out: LEAVE\n");
   }
}
#endif // USE_ASYNC


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

   yac_host_obj = yac_host;

   mtx_async = NULL;

   b_out_running = YAC_FALSE;
   b_in_running = YAC_FALSE;

   b_running = YAC_FALSE;
   b_active  = YAC_FALSE;
   b_processing_cbk = YAC_FALSE;
   in_handle    = NULL;
   out_handle   = NULL;

   input_buf  = NULL;
   output_buf = NULL;

   sample_rate = 0u;
   num_in_ch   = 0u;
   num_out_ch  = 0u;

   dbg_avg_avail = 0.0f;

   resetRingBuf();
}

_PaStream::~_PaStream() {
#ifdef USE_MUTEX
   if(NULL != mtx_async)
   {
      yac_host->yacMutexDestroy(mtx_async);
      mtx_async = NULL;
   }
#endif // USE_MUTEX

   _abort();
   _close();
}

void _PaStream::closeHandles(void) {

#if 0
   if((NULL != in_handle) && (NULL != out_handle))
      snd_pcm_unlink(in_handle);
#endif

   if(NULL != in_handle)
   {
      snd_pcm_close(in_handle);
      in_handle = NULL;
   }

   if(NULL != out_handle)
   {
      snd_pcm_close(out_handle);
      out_handle = NULL;
   }
}

void _PaStream::resetRingBuf(void) {
   memset((void*)in_ringbuf, 0, sizeof(in_ringbuf));
   memset((void*)out_ringbuf, 0, sizeof(out_ringbuf));
   in_ringbuf_avail    = 0u;
   in_ringbuf_readoff  = 0u;
   in_ringbuf_writeoff = 0u;
   out_ringbuf_avail   = 0u;
   out_ringbuf_readoff  = 0u;
   out_ringbuf_writeoff = 0u;
}

void _PaStream::processInput(sBool _bVerbose) {
   // Query number of available input frames
   sSI avail = pcmAvailUpdateRecover(in_handle);

   if(avail > 0)
   {
      if(_bVerbose)
      {
         Dprintf("[trc] PaStream<alsa_async>::processInput: avail=%d\n", avail);
      }

      while(avail > 0)
      {
         sSI curNumFrames;

         if(avail >= (sSI)period_size)
         {
            curNumFrames = period_size;
         }
         else
         {
            curNumFrames = avail;
         }

         // Is there any space to store new samples in "in_ringbuf" ?
         sUI maxRingAvail = num_periods * period_size;
         sUI ringAvail = (sUI)yac_host->yacAtomicInc((volatile int*)&in_ringbuf_avail, 0/*inc*/);
         if(ringAvail < maxRingAvail)
         {
            if( sSI(maxRingAvail - ringAvail) < curNumFrames )
               curNumFrames = sSI(maxRingAvail - ringAvail);

            // Read samples
            snd_pcm_sframes_t frames = snd_pcm_readi(in_handle,
                                                     in_buf,
                                                     curNumFrames
                                                     );
            // if(frames > 0)
            //    yac_host->printf("xxx in_buf[0]=%u\n", in_buf[0]);

            if(frames != (snd_pcm_sframes_t)curNumFrames)
            {
               Dprintf("[~~~] PaStream<alsa_async>::processInput: snd_pcm_readi() returned frames=%d but requested curNumFrames=%u\n", frames, curNumFrames);
            }

            if(frames < 0)
            {
               frames = snd_pcm_recover(in_handle, frames/*err*/, 1/*silent*/);
            }

            // Convert input samples to float and write to in_ringbuf
            {
               sF32 *d = in_ringbuf;
               sUI numSamples = curNumFrames * num_in_ch;
               sUI i;

               // yac_host->printf("xxx read avail=%u curNumFrames=%u\n", avail, curNumFrames);

               switch(sample_format)
               {
                  case 99:
                  {
                     // Silence
                     for(i = 0u; i < numSamples; i++)
                     {
                        d[in_ringbuf_writeoff] = 0.0f;
                        in_ringbuf_writeoff = (in_ringbuf_writeoff + 1u) & (TKPORTAUDIO_RINGSIZE - 1u);
                     }
                  }
                  break;

                  case paFloat32:
                  {
                     const sF32 *s = (sF32*) in_buf;
                     for(i = 0u; i < numSamples; i++)
                     {
                        d[in_ringbuf_writeoff] = *s++;
                        in_ringbuf_writeoff = (in_ringbuf_writeoff + 1u) & (TKPORTAUDIO_RINGSIZE - 1u);
                     }
                  }
                  break;

                  case paInt32:
                  {
                     if(SND_PCM_FORMAT_S32_LE == alsa_sample_format)
                     {
                        const sSI *s = (sSI*) in_buf;
                        for(i = 0u; i < numSamples; i++)
                        {
                           d[in_ringbuf_writeoff] = (*s++ / 2147483647.0f);
                           in_ringbuf_writeoff = (in_ringbuf_writeoff + 1u) & (TKPORTAUDIO_RINGSIZE - 1u);
                        }
                     }
                     else if(SND_PCM_FORMAT_S32_BE == alsa_sample_format)
                     {
                        const sU8 *s = (sU8*) in_buf;
                        for(i = 0u; i < numSamples; i++)
                        {
                           sUI v = ((sUI)s[3]) | ((sUI)(s[2]<<8)) | ((sUI)(s[1]<<16)) | ((sUI)(s[0]<<24));
                           d[in_ringbuf_writeoff] = ((*(sSI*)&v) / 2147483647.0f);
                           s += 4;
                           in_ringbuf_writeoff = (in_ringbuf_writeoff + 1u) & (TKPORTAUDIO_RINGSIZE - 1u);
                        }
                     }
                  }
                  break;

                  case paInt24:
                  {
                     if(SND_PCM_FORMAT_S24_3BE == alsa_sample_format)
                     {
                        const sU8 *s = (sU8*) in_buf;
                        for(i = 0u; i < numSamples; i++)
                        {
                           sUI v = ((sUI)s[2]) | ((sUI)(s[1]<<8)) | ((sUI)(s[0]<<16));
                           if(v & 0x800000)
                              v |= 0xFF000000;
                           d[in_ringbuf_writeoff] = ((*(sSI*)&v) / 8388607.0f);
                           s += 3;
                           in_ringbuf_writeoff = (in_ringbuf_writeoff + 1u) & (TKPORTAUDIO_RINGSIZE - 1u);
                        }
                     }
                     else if(SND_PCM_FORMAT_S24_3LE == alsa_sample_format)
                     {
                        const sU8 *s = (sU8*) in_buf;
                        for(i = 0u; i < numSamples; i++)
                        {
                           sUI v = ((sUI)s[0]) | ((sUI)(s[1]<<8)) | ((sUI)(s[1]<<16));
                           if(v & 0x800000)
                              v |= 0xFF000000;
                           d[in_ringbuf_writeoff] = ((*(sSI*)&v) / 8388607.0f);
                           s += 3;
                           in_ringbuf_writeoff = (in_ringbuf_writeoff + 1u) & (TKPORTAUDIO_RINGSIZE - 1u);
                        }
                     }
                  }
                  break;

                  case paInt16:
                  {
                     if(SND_PCM_FORMAT_S16_LE == alsa_sample_format)
                     {
                        const sS16 *s = (sS16*) in_buf;
                        for(i = 0u; i < numSamples; i++)
                        {
                           d[in_ringbuf_writeoff] = (*s++) / 32767.0f;
                           in_ringbuf_writeoff = (in_ringbuf_writeoff + 1u) & (TKPORTAUDIO_RINGSIZE - 1u);
                        }
                     }
                     else if(SND_PCM_FORMAT_S16_BE == alsa_sample_format)
                     {
                        const sU8 *s = (sU8*) in_buf;
                        for(i = 0u; i < numSamples; i++)
                        {
                           sS16 v = sS16( ((sUI)s[1]) | ((sUI)(s[0]<<8)) );
                           d[in_ringbuf_writeoff] = v / 32767.0f;
                           s += 2;
                           in_ringbuf_writeoff = (in_ringbuf_writeoff + 1u) & (TKPORTAUDIO_RINGSIZE - 1u);
                        }
                     }
                  }
                  break;

                  case paInt8:
                  {
                     const sS8 *s = (sS8*) in_buf;
                     for(i = 0u; i < numSamples; i++)
                     {
                        d[in_ringbuf_writeoff] = (*s++) / 127.0f;
                        in_ringbuf_writeoff = (in_ringbuf_writeoff + 1u) & (TKPORTAUDIO_RINGSIZE - 1u);
                     }
                  }
                  break;

                  case paUInt8:
                  {
                     const sU8 *s = (sU8*) in_buf;
                     for(i = 0u; i < numSamples; i++)
                     {
                        d[in_ringbuf_writeoff] = (((sSI)(*s++))-128) / 127.0f;
                        in_ringbuf_writeoff = (in_ringbuf_writeoff + 1u) & (TKPORTAUDIO_RINGSIZE - 1u);
                     }
                  }
                  break;
               } // switch sampleformat
            }

            avail -= curNumFrames;
            (void)yac_host->yacAtomicInc((volatile int*)&in_ringbuf_avail, curNumFrames);

         } // if ringAvail < maxRingAvail

         // if(avail > 0)
         // {
         //    yac_host->printf("[~~~] PaStream<alsa_async>::processInput: input overflow (%d frames left), skipping..\n", avail);
         //    snd_pcm_sframes_t frames = snd_pcm_readi(in_handle,
         //                                             in_buf,
         //                                             avail
         //                                             );
         //    avail = 0;
         // }

      } // while avail > 0
   } // if avail > 0
}

void _PaStream::processOutput(sBool _bVerbose) {
   sSI avail = pcmAvailUpdateRecover(out_handle);

   dbg_avg_avail = (dbg_avg_avail + avail) * 0.5f;

   if(avail > 0)
   {
      if(_bVerbose)
      {
         Dprintf("[trc] PaStream<alsa_async>::processOutput: avail=%d\n", avail);
      }

      while(avail > 0)
      {
         sSI curNumFrames;

         if(avail >= (sSI)period_size)
         {
            curNumFrames = period_size;
         }
         else
         {
            curNumFrames = avail;
         }


//             if(bFirstFrag)
//             {
//                sF32 dt = GetMilliSecondsDouble();
//                printf("audio dt=%f avail=%u \n", dt, avail);
//                bFirstFrag = 0;
//             }

         // Are there any frames avail in "out_ringbuf" ?
         sUI ringAvail = (sUI)yac_host->yacAtomicInc((volatile int*)&out_ringbuf_avail, 0/*inc*/);
         if(ringAvail > 0u)
         {
            if(_bVerbose)
            {
               Dprintf("[trc] PaStream<alsa_async>::processOutput: ringAvail=%u\n", ringAvail);
            }

            if( sSI(ringAvail) < curNumFrames )
            {
               // yac_host->printf("[~~~] PaStream<alsa_async>::processOutput: output underrun, ringAvail=%u, avail=%d\n", ringAvail, avail);
               curNumFrames = sSI(ringAvail);
            }

            {
               // Convert (float) out_ringbuf to flat out_buf array (actual sampleformat)
               sUI i;
               sUI numSamples = curNumFrames * num_out_ch;
               const sF32 *s = out_ringbuf;

#define Dclipv sF32 v = s[out_ringbuf_readoff]; out_ringbuf_readoff = (out_ringbuf_readoff + 1u) & (TKPORTAUDIO_RINGSIZE - 1u); if(v > 1.0f) v = 1.0f;else if(v < -1.0f) v = -1.0f
               switch(sample_format)
               {
                  case paFloat32:
                  {
                     sF32 *d = (sF32*) out_buf;
                     for(i = 0u; i < numSamples; i++)
                     {
                        Dclipv;
                        *d++ = v;
                     }
                  }
                  break;

                  case paInt32:
                  {
                     if(SND_PCM_FORMAT_S32_LE == alsa_sample_format)
                     {
                        sSI *d = (sSI*) out_buf;
                        for(i = 0u; i < numSamples; i++)
                        {
                           Dclipv;
                           *d++ = (sSI) (v * 2147483647.0f);
                        }
                     }
                     else if(SND_PCM_FORMAT_S32_BE == alsa_sample_format)
                     {
                        sU8 *d = (sU8*) out_buf;
                        for(i = 0u; i < numSamples; i++)
                        {
                           Dclipv;
                           sS32 vi = sS32(v * 2147483647.0f);
                           d[0] = sU8((vi >> 24) & 255);
                           d[1] = sU8((vi >> 16) & 255);
                           d[2] = sU8((vi >>  8) & 255);
                           d[3] = sU8(vi & 255);
                           d += 4;
                        }
                     }
                  }
                  break;

                  case paInt24:
                  {
                     if(SND_PCM_FORMAT_S24_3BE == alsa_sample_format)
                     {
                        // e.g. Guitar Rig / Native Instruments Session I/O audio interface
                        sU8 *d = (sU8*) out_buf;
                        for(i = 0u; i < numSamples; i++)
                        {
                           Dclipv;
                           sUI vi = (sUI) (v * 8388607.0f);
                           d[2] = (vi&255);
                           d[1] = ((vi>>8)&255);
                           d[0] = ((vi>>16)&255);
                           d += 3;
                        }
                     }
                     else if(SND_PCM_FORMAT_S24_3LE == alsa_sample_format)
                     {
                        sU8 *d = (sU8*) out_buf;
                        for(i = 0u; i < numSamples; i++)
                        {
                           Dclipv;
                           sUI vi = (sUI) (v * 8388607.0f);
                           d[0] = (vi&255);
                           d[1] = ((vi>>8)&255);
                           d[2] = ((vi>>16)&255);
                           d += 3;
                        }
                     }
                  }
                  break;

                  case paInt16:
                  {
                     if(SND_PCM_FORMAT_S16_LE == alsa_sample_format)
                     {
                        sS16 *d = (sS16*) out_buf;
                        for(i = 0u; i < numSamples; i++)
                        {
                           Dclipv;
                           *d++ = (sS16) (v * 32767.0f);
                        }
                     }
                     else if(SND_PCM_FORMAT_S16_BE == alsa_sample_format)
                     {
                        sU8 *d = (sU8*) out_buf;
                        for(i = 0u; i < numSamples; i++)
                        {
                           Dclipv;
                           sS16 vi = (sS16) (v * 32767.0f);
                           d[0] = sU8(vi >> 8);
                           d[1] = sU8(vi & 255);
                           d += 2;
                        }
                     }
                  }
                  break;

                  case paInt8:
                  {
                     sS8 *d = (sS8*) out_buf;
                     for(i = 0u; i < numSamples; i++)
                     {
                        Dclipv;
                        *d++ = (sS8) ( v * 127.0f );
                     }
                  }
                  break;

                  case paUInt8:
                  {
                     sU8 *d = (sU8*) out_buf;
                     for(i = 0u; i < numSamples; i++)
                     {
                        Dclipv;
                        *d++ = (sU8) ( v * 127.0f + 128.0f );
                     }
                  }
                  break;
               } // switch sampleformat
#undef Dclipv
            }

            avail -= curNumFrames;
            (void)yac_host->yacAtomicDec((volatile int*)&out_ringbuf_avail, curNumFrames);

            if(_bVerbose)
            {
               Dprintf("[trc] PaStream<alsa_async>::processOutput: processed curNumFrames=%d => new avail=%d\n", curNumFrames, avail);
            }

            // Write samples
            snd_pcm_sframes_t frames = snd_pcm_writei(out_handle,
                                                      out_buf,
                                                      curNumFrames
                                                      );

            if(_bVerbose)
            {
               Dprintf("[trc] PaStream<alsa_async>::processOutput: snd_pcm_writei() returned %d. remaining avail=%d\n", frames, avail);
            }

            if(frames != (snd_pcm_sframes_t)curNumFrames)
            {
               Dprintf("[~~~] PaStream<alsa_async>::processOutput: snd_pcm_writei() returned frames=%d but requested curNumFrames=%u\n", frames, curNumFrames);
            }

            if(frames < 0)
            {
               frames = snd_pcm_recover(out_handle, frames/*err*/, 1/*silent*/);
            }

            if(frames < 0)
            {
               yac_host->printf("[~~~] PaStream::processOutput: snd_pcm_recover<writei> failed: err=%d\n", (int)frames);
            }

         } // if ringAvail > 0

         // if(avail > 0)
         // {
         //    yac_host->printf("[~~~] PaStream<alsa_async>::processOutput: output underrun (need %d. filling with zero..)\n", avail);
         //    writeZeroes((sUI)avail);
         //    avail = 0;
         // }

      } // while avail > 0
   } // if avail > 0
}

void _PaStream::writeZeroes(const sUI _numFrames) {
   sUI frameSz;
   switch(sample_format)
   {
      default:
      case paFloat32: frameSz = 4; break;
      case paInt32:   frameSz = 4; break;
      case paInt24:   frameSz = 3; break;
      case paInt16:   frameSz = 2; break;
      case paInt8:    frameSz = 1; break;
   }
   frameSz *= num_out_ch;
   sUI numSamples = frameSz * _numFrames;
   ::memset((void*)out_buf, 0, numSamples);
   snd_pcm_writei(out_handle, out_buf, _numFrames);
}

sBool _PaStream::hwParamsSet(snd_pcm_t *handle, snd_pcm_hw_params_t *hwparams, int _line) {
   int err = snd_pcm_hw_params(handle,
                               hwparams
                               );
   if(err < 0)
   {
      yac_host->printf("[---] PaStream<alsa_async>::hwParamsSet:%d: snd_pcm_hw_params() failed. err=%d(\"%s\")\n",
                       _line,
                       err,
                       snd_strerror(err)
                       );
      closeHandles();
      return YAC_FALSE;
   }
   else
   {
      return YAC_TRUE;
   }
}

sBool _PaStream::hwParamsGet(snd_pcm_t *handle, snd_pcm_hw_params_t *hwparams, int _line) {
   int err = snd_pcm_hw_params_current(handle,
                                       hwparams
                                       );
   if(err < 0)
   {
      yac_host->printf("[---] PaStream<alsa_sync>:hwParamsGet:%d: snd_pcm_hw_params_current() failed. err=%d(\"%s\")\n",
                       _line,
                       err,
                       snd_strerror(err)
                       );
      closeHandles();
      return YAC_FALSE;
   }
   else
   {
      return YAC_TRUE;
   }
}

sBool _PaStream::initStream2(_PortAudio *_pa,
                             void *_cbk,
                             _PaStreamParameters *_inParams,
                             _PaStreamParameters *_outParams,
                             sUI _sampleRate,
                             sUI _numPeriods,
                             sUI _periodSize,
                             sUI _bufferSize
                           )
{
   if(-1 == _outParams->device_index)
   {
      yac_host->printf("[---] PaStream<alsa_async>::initStream: invalid output device index=%d\n", _outParams->device_index);
      return YAC_FALSE;
   }

   _PaDeviceInfo *deviceInfoOut = _pa->device_infos[_outParams->device_index];

   if(NULL == deviceInfoOut)
   {
      yac_host->printf("[---] PaStream<alsa_async>::initStream: output device index=%d is NULL\n", _outParams->device_index);
      return YAC_FALSE;
   }

   // Create script context
   stream_cbk         = _cbk;
   stream_cbk_context = yac_host->yacContextCreate();

   // Allocate 32bit float input buffer
   input_buf  = YAC_New_FloatArray();
   input_buf->yacArrayAlloc(TKPORTAUDIO_MAX_BUFSIZE / sizeof(float));

   // Allocate 32bit float output buffer
   output_buf = YAC_New_FloatArray();
   output_buf->yacArrayAlloc(TKPORTAUDIO_MAX_BUFSIZE / sizeof(float));

   snd_pcm_format_t sndSmpFormat;

   sample_format = _outParams->sample_format;

   resetRingBuf();

#if 0
   switch(_outParams->sample_format)
   {
      case paInt8:
         sndSmpFormat = SND_PCM_FORMAT_S8;
         break;

      default:
      case paInt16:
         sndSmpFormat = SND_PCM_FORMAT_S16;
         sample_format = paInt16;
         break;

      case paInt24:
         sndSmpFormat = SND_PCM_FORMAT_S24;  // == SND_PCM_FORMAT_S24_LE
         // // sndSmpFormat = SND_PCM_FORMAT_S24_3BE;  // Guitar Rig Session I/O  (see below)
         break;

      case paInt32:
         sndSmpFormat = SND_PCM_FORMAT_S32_LE;
         break;

      case paFloat32:
         sndSmpFormat = SND_PCM_FORMAT_FLOAT;
         break;
   }
#endif

   sndSmpFormat = (snd_pcm_format_t)deviceInfoOut->alsa_sample_format;

   sample_rate = _sampleRate;

   // Open device
   {
      int err;

      yac_host->printf("xxx initStream2: _inParams->channel_count=%u\n", _inParams->channel_count);

      if(_inParams->channel_count > 0)
      {
         err = snd_pcm_open(&in_handle, (const char*)deviceInfoOut->hw_name->chars, SND_PCM_STREAM_CAPTURE, 0);
         yac_host->printf("xxx snd_pcm_open in_handle err=%d\n", err);
         if(err < 0)
         {
            yac_host->printf("[---] PaStream<alsa_async>::initStream: snd_pcm_open(dev=\"%s\") failed (playback): err=%d(\"%s\").\n",
                             (const char*)deviceInfoOut->hw_name->chars,
                             err,
                             snd_strerror(err)
                             );
            return YAC_FALSE;
         }
      }

      err = snd_pcm_open(&out_handle, (const char*)deviceInfoOut->hw_name->chars, SND_PCM_STREAM_PLAYBACK, 0);
      if(err < 0)
      {
         yac_host->printf("[---] PaStream<alsa_async>::initStream: snd_pcm_open(dev=\"%s\") failed (playback): err=%d(\"%s\").\n",
                          (const char*)deviceInfoOut->hw_name->chars,
                          err,
                          snd_strerror(err)
                          );
         return YAC_FALSE;
      }

#if 0
      if(NULL != in_handle)
         snd_pcm_link(in_handle, out_handle);
#endif

      snd_pcm_uframes_t prefPeriodSize = _periodSize;
      int               prefNumPeriods = _numPeriods;
      snd_pcm_uframes_t prefBufferSize = _bufferSize; // should be (prefNumPeriods * prefPeriodSize)

#ifdef PANDORA_LOWLATENCY_HACK
      // Enable OpenPandora per-stream low latency hack (thanks Notaz!)
      if(_periodSize < 256u)
      {
         sBool bOk = loc_perform_hack(handle, (sUI)prefPeriodSize, 65536/*(sUI)prefBufferSize*/);
         if(!bOk)
         {
            /* Try next period/buffer sizes */
            closeHandles();
            return YAC_FALSE;
         }
      }
#endif // PANDORA_LOWLATENCY_HACK

      yac_host->printf("[dbg] PaStream<alsa_async>::initStream: prefPeriodSize=%lu, prefNumPeriods=%d, prefBufferSize=%lu.\n",
                       prefPeriodSize,
                       prefNumPeriods,
                       prefBufferSize
                       );

      int dir = -1;
      sUI               periods    = prefNumPeriods;
      snd_pcm_uframes_t periodSize = prefPeriodSize;
      snd_pcm_uframes_t bufferSize = prefBufferSize;

      // Query hardware parameters
      snd_pcm_hw_params_t *hwparams;
      snd_pcm_hw_params_alloca(&hwparams);

      err = snd_pcm_hw_params_any(out_handle,
                                  hwparams
                                  );
      if(err < 0)
      {
         yac_host->printf("[---] PaStream<alsa_async>::initStream<out>: snd_pcm_hw_params_any() failed. err=%d(\"%s\")\n",
                          err,
                          snd_strerror(err)
                          );
         closeHandles();
         return YAC_FALSE;
      }

      // configuration
      {

         // Set access
         if((err = snd_pcm_hw_params_set_access(out_handle,
                                                hwparams,
                                                SND_PCM_ACCESS_RW_INTERLEAVED
                                                )) < 0)
         {
            yac_host->printf("[---] PaStream<alsa_async>::initStream: snd_pcm_hw_params_set_access() failed. err=%d(\"%s\")\n",
                             err,
                             snd_strerror(err)
                             );
            closeHandles();
            return YAC_FALSE;
         }

#if 0
         // (note) ==> now done in PortAudio.cpp (enumerateDevices())
         // (note) see "/usr/include/alsa/pcm.h"
         if(SND_PCM_FORMAT_S24 == sndSmpFormat)
         {
            // Check which S24 format is actually available
            if(snd_pcm_hw_params_test_format(out_handle, hwparams, SND_PCM_FORMAT_S24_3LE) >= 0)
            {
               sndSmpFormat = SND_PCM_FORMAT_S24_3LE;
            }
            else if(snd_pcm_hw_params_test_format(out_handle, hwparams, SND_PCM_FORMAT_S24_3BE ) >= 0)
            {
               // e.g. Guitar Rig / Native Instruments Session I/O audio interface
               sndSmpFormat = SND_PCM_FORMAT_S24_3BE;
            }
         }
#endif

         alsa_sample_format = sndSmpFormat;

         // Set sample format
         err = snd_pcm_hw_params_set_format(out_handle,
                                            hwparams,
                                            sndSmpFormat
                                            );
         if(err < 0)
         {
            yac_host->printf("[---] PaStream<alsa_async>::initStream: snd_pcm_hw_params_set_format() failed. err=%d(\"%s\")\n",
                             err,
                             snd_strerror(err)
                             );
            closeHandles();
            return YAC_FALSE;
         }

         // Query min/max rates
         {
            sUI rateMin = 0u;
            sUI rateMax = 0u;

            err = snd_pcm_hw_params_get_rate_min(hwparams,
                                                 &rateMin,
                                                 &dir
                                                 );
            if(err < 0)
            {
               yac_host->printf("[---] PaStream<alsa_async>::initStream: snd_pcm_hw_params_get_rate_min() failed. err=%d(\"%s\")\n",
                                err,
                                snd_strerror(err)
                                );
            }

            err = snd_pcm_hw_params_get_rate_max(hwparams,
                                                 &rateMax,
                                                 &dir
                                                 );
            if(err < 0)
            {
               yac_host->printf("[---] PaStream<alsa_async>::initStream: snd_pcm_hw_params_get_rate_max() failed. err=%d(\"%s\")\n",
                                err,
                                snd_strerror(err)
                                );
            }

            yac_host->printf("[dbg] PaStream<alsa_async>::initStream: rateMin=%u rateMax=%u\n", rateMin, rateMax);
         }

         // Set rate
         err = snd_pcm_hw_params_set_rate(out_handle,
                                          hwparams,
                                          sample_rate,
                                          0/*dir. ?*/
                                          );
         if(err < 0)
         {
            yac_host->printf("[---] PaStream<alsa_async>::initStream: snd_pcm_hw_params_set_rate() failed. rate=%u, err=%d(\"%s\")\n",
                             sample_rate,
                             err,
                             snd_strerror(err)
                             );
         }

         // Set channels
         err = snd_pcm_hw_params_set_channels(out_handle,
                                              hwparams,
                                              _outParams->channel_count
                                              );
         if(err < 0)
         {
            yac_host->printf("[---] PaStream<alsa_async>::initStream: snd_pcm_hw_params_set_channels() failed. err=%d(\"%s\")\n",
                             err,
                             snd_strerror(err)
                             );
            closeHandles();
            return YAC_FALSE;
         }

         num_out_ch = _outParams->channel_count;

         // Query min/max buffer size
         snd_pcm_uframes_t bufferSizeMin;
         snd_pcm_uframes_t bufferSizeMax;

         err = snd_pcm_hw_params_get_buffer_size_min(hwparams, &bufferSizeMin);
         if(err < 0)
         {
            yac_host->printf("[---] PaStream<alsa_async>::initStream: snd_pcm_hw_params_get_buffer_size_min() failed. err=%d(\"%s\")\n",
                             err,
                             snd_strerror(err)
                             );
         }

         err = snd_pcm_hw_params_get_buffer_size_max(hwparams, &bufferSizeMax);
         if(err < 0)
         {
            yac_host->printf("[~~~] PaStream<alsa_async>::initStream: snd_pcm_hw_params_get_buffer_size_max() failed. err=%d(\"%s\")\n",
                             err,
                             snd_strerror(err)
                             );
         }

         yac_host->printf("[dbg] PaStream<alsa_async>::initStream: bufSizeMin=%u bufSizeMax=%u\n",
                          (sUI)bufferSizeMin, (sUI)bufferSizeMax
                          );

         // Set preferred buffer size
         err = snd_pcm_hw_params_set_buffer_size(out_handle,
                                                 hwparams,
                                                 prefBufferSize
                                                 );
         if(err < 0)
         {
            yac_host->printf("[---] PaStream<alsa_async>::initStream: snd_pcm_hw_params_set_buffer_size() failed. prefBufferSize=%u, err=%d(\"%s\")\n",
                             prefBufferSize,
                             err,
                             snd_strerror(err)
                             );
         }

         // Query min/max period size
         snd_pcm_uframes_t periodSizeMin;
         snd_pcm_uframes_t periodSizeMax;

         err = snd_pcm_hw_params_get_period_size_min(hwparams, &periodSizeMin, 0);
         if(err < 0)
         {
            yac_host->printf("[---] PaStream<alsa_async>::initStream: snd_pcm_hw_params_get_period_size_min() failed. err=%d(\"%s\")\n",
                             err,
                             snd_strerror(err)
                             );
         }

         err = snd_pcm_hw_params_get_period_size_max(hwparams, &periodSizeMax, 0);
         if(err < 0)
         {
            yac_host->printf("[---] PaStream<alsa_async>::initStream: snd_pcm_hw_params_get_period_size_max() failed. err=%d(\"%s\")\n",
                             err,
                             snd_strerror(err)
                             );
         }
         yac_host->printf("[dbg] PaStream<alsa_async>::initStream: periodSizeMin=%lu periodSizeMax=%lu\n",
                          periodSizeMin, periodSizeMax
                          );

         // Query min/max #periods
         err = snd_pcm_hw_params_get_periods_min(hwparams,
                                                 &periods,
                                                 &dir
                                                 );
         if(err < 0)
         {
            yac_host->printf("[---] PaStream<alsa_async>::initStream: snd_pcm_hw_params_get_periods_min() failed. err=%d(\"%s\")\n",
                             err,
                             snd_strerror(err)
                             );
         }
         else
         {
            yac_host->printf("[dbg] PaStream<alsa_async>::initStream: snd_pcm_hw_params_get_periods_min() returned %u\n",
                             periods
                             );
         }

         err = snd_pcm_hw_params_get_periods_max(hwparams,
                                                 &periods,
                                                 &dir
                                                 );
         if(err < 0)
         {
            yac_host->printf("[---] PaStream<alsa_async>::initStream: snd_pcm_hw_params_get_periods_max() failed. err=%d(\"%s\")\n",
                             err,
                             snd_strerror(err)
                             );
         }
         else
         {
            yac_host->printf("[dbg] PaStream<alsa_async>::initStream: snd_pcm_hw_params_get_periods_max() returned %u\n",
                             periods
                             );
         }

         // Set preferred #periods
         err = snd_pcm_hw_params_set_periods(out_handle,
                                             hwparams,
                                             prefNumPeriods,
                                             0
                                             );
         if(err < 0)
         {
            yac_host->printf("[---] PaStream<alsa_async>::initStream: snd_pcm_hw_params_set_periods() failed. err=%d(\"%s\")\n",
                             err,
                             snd_strerror(err)
                             );
         }

         // Set preferred period size
         err = snd_pcm_hw_params_set_period_size(out_handle,
                                                 hwparams,
                                                 prefPeriodSize,
                                                 0
                                                 );
         if(err < 0)
         {
            yac_host->printf("[---] PaStream<alsa_async>::initStream: snd_pcm_hw_params_set_period_size() failed. prefPeriodSize=%lu err=%d(\"%s\")\n",
                             prefPeriodSize,
                             err,
                             snd_strerror(err)
                             );
         }

         if(!hwParamsSet(out_handle, hwparams, __LINE__))
         {
            closeHandles();
            return YAC_FALSE;
         }

         // if(!hwParamsSet(hwparams, __LINE__))
         // {
         //    return YAC_FALSE;
         // }

         if(!hwParamsGet(out_handle, hwparams, __LINE__))
         {
            return YAC_FALSE;
         }

         // Query actual period size
         err = snd_pcm_hw_params_get_period_size(hwparams, &periodSize, 0);
         if(err < 0)
         {
            yac_host->printf("[---] PaStream<alsa_async>::initStream: snd_pcm_hw_params_get_period_size() failed. err=%d(\"%s\")\n",
                             err,
                             snd_strerror(err)
                             );
         }
         else
         {
            yac_host->printf("[dbg] PaStream<alsa_async>::initStream: snd_pcm_hw_param_get_period_size() returned %lu.\n",
                             periodSize
                             );
         }

         // Query actual #periods
         {
            int dir = -1;
            err = snd_pcm_hw_params_get_periods(hwparams,
                                                &periods,
                                                &dir
                                                );
            if(err < 0)
            {
               yac_host->printf("[---] PaStream<alsa_async>::initStream: snd_pcm_hw_params_get_periods() failed. err=%d(\"%s\")\n",
                                err,
                                snd_strerror(err)
                                );
            }
            else
            {
               yac_host->printf("[dbg] PaStream<alsa_async>::initStream: snd_pcm_hw_params_get_periods(): periods=%u\n", periods);
            }
         }

         // Query actual buffer size
         err = snd_pcm_hw_params_get_buffer_size(hwparams,
                                                 &bufferSize
                                                 );
         if(err < 0) {
            yac_host->printf("[---] PaStream<alsa_async>::initStream: snd_pcm_hw_params_get_buffer_size() failed. err=%d(\"%s\")\n",
                             err,
                             snd_strerror(err)
                             );
            closeHandles();
            return YAC_FALSE;
         }
         else
         {
            yac_host->printf("[dbg] PaStream<alsa_async>::initStream: snd_pcm_hw_params_get_buffer_size() returned bufferSize=%lu\n",
                             bufferSize
                             );
         }

         {
            int dir = 0;
            err = snd_pcm_hw_params_get_periods(hwparams,
                                                &periods,
                                                &dir
                                                );
            if(err < 0)
            {
               yac_host->printf("[---] PaStream<alsa_async>::initStream: snd_pcm_hw_params_get_periods() failed. err=%d(\"%s\")\n",
                                err,
                                snd_strerror(err)
                                );
               closeHandles();
               return YAC_FALSE;
            }
         }
         yac_host->printf("[dbg] PaStream<alsa_async>::initStream: #periods=%u\n", periods);

         // Query rate
         err = snd_pcm_hw_params_get_rate(hwparams,
                                          &sample_rate,
                                          &dir
                                          );
         if(err < 0)
         {
            yac_host->printf("[---] PaStream<alsa_async>: snd_pcm_hw_params_get_rate() failed. err=%d(\"%s\")\n",
                             err,
                             snd_strerror(err)
                             );
            closeHandles();
            return YAC_FALSE;
         }
         else
         {
            yac_host->printf("[dbg] PaStream<alsa_async>: snd_pcm_hw_params_get_rate(): sample_rate=%u.\n",
                             sample_rate
                             );
         }

      } // end configuration

      if(NULL != in_handle)
      {
         err = snd_pcm_hw_params_any(in_handle,
                                     hwparams
                                     );
         if(err < 0)
         {
            yac_host->printf("[---] PaStream<alsa_async>::initStream<in>: snd_pcm_hw_params_any() failed. err=%d(\"%s\")\n",
                             err,
                             snd_strerror(err)
                             );
            closeHandles();
            return YAC_FALSE;
         }

         // Set access
         if((err = snd_pcm_hw_params_set_access(in_handle,
                                                hwparams,
                                                SND_PCM_ACCESS_RW_INTERLEAVED
                                                )) < 0)
         {
            yac_host->printf("[---] PaStream<alsa_async>::initStream<in>: snd_pcm_hw_params_set_access() failed. err=%d(\"%s\")\n",
                             err,
                             snd_strerror(err)
                             );
            closeHandles();
            return YAC_FALSE;
         }

         // Set sample format
         err = snd_pcm_hw_params_set_format(in_handle,
                                            hwparams,
                                            sndSmpFormat
                                            );
         if(err < 0)
         {
            yac_host->printf("[---] PaStream<alsa_async>::initStream<in>: snd_pcm_hw_params_set_format() failed. err=%d(\"%s\")\n",
                             err,
                             snd_strerror(err)
                             );
            closeHandles();
            return YAC_FALSE;
         }

         // Set rate
         err = snd_pcm_hw_params_set_rate(in_handle,
                                          hwparams,
                                          sample_rate,
                                          0/*dir. ?*/
                                          );
         if(err < 0)
         {
            yac_host->printf("[---] PaStream<alsa_async>::initStream<in>: snd_pcm_hw_params_set_rate() failed. rate=%u, err=%d(\"%s\")\n",
                             sample_rate,
                             err,
                             snd_strerror(err)
                             );
         }

         // Set channels
         err = snd_pcm_hw_params_set_channels(in_handle,
                                              hwparams,
                                              _inParams->channel_count
                                              );
         if(err < 0)
         {
            yac_host->printf("[---] PaStream<alsa_async>::initStream<in>: snd_pcm_hw_params_set_channels() failed. err=%d(\"%s\")\n",
                             err,
                             snd_strerror(err)
                             );
            closeHandles();
            return YAC_FALSE;
         }

         num_in_ch = _inParams->channel_count;

         // Set preferred buffer size
         err = snd_pcm_hw_params_set_buffer_size(in_handle,
                                                 hwparams,
                                                 prefBufferSize
                                                 );
         if(err < 0)
         {
            yac_host->printf("[---] PaStream<alsa_async>::initStream<in>: snd_pcm_hw_params_set_buffer_size() failed. prefBufferSize=%u, err=%d(\"%s\")\n",
                             prefBufferSize,
                             err,
                             snd_strerror(err)
                             );
         }

         // Set preferred #periods
         err = snd_pcm_hw_params_set_periods(in_handle,
                                             hwparams,
                                             prefNumPeriods,
                                             0
                                             );
         if(err < 0)
         {
            yac_host->printf("[---] PaStream<alsa_async>::initStream<in>: snd_pcm_hw_params_set_periods() failed. err=%d(\"%s\")\n",
                             err,
                             snd_strerror(err)
                             );
         }

         // Set preferred period size
         err = snd_pcm_hw_params_set_period_size(in_handle,
                                                 hwparams,
                                                 prefPeriodSize,
                                                 0
                                                 );
         if(err < 0)
         {
            yac_host->printf("[---] PaStream<alsa_async>::initStream<in>: snd_pcm_hw_params_set_period_size() failed. prefPeriodSize=%lu err=%d(\"%s\")\n",
                             prefPeriodSize,
                             err,
                             snd_strerror(err)
                             );
         }

         if(!hwParamsSet(in_handle, hwparams, __LINE__))
         {
            closeHandles();
            return YAC_FALSE;
         }
      }

      // Configure sw parameters
      //  (note) does not seem to have any effect
      //          (e.g. when period_size=64, avail data in async__cbk is sometimes 55)
      {
         snd_pcm_sw_params_t *swparams;
         snd_pcm_sw_params_alloca(&swparams);

         if(NULL != in_handle)
         {
            snd_pcm_sw_params_current(in_handle, swparams);
            snd_pcm_sw_params_set_start_threshold(in_handle, swparams, period_size*num_periods - period_size);
            snd_pcm_sw_params_set_avail_min(in_handle, swparams, period_size);
         }

         snd_pcm_sw_params_current(out_handle, swparams);
         snd_pcm_sw_params_set_start_threshold(out_handle, swparams, period_size*num_periods - period_size);
         snd_pcm_sw_params_set_avail_min(out_handle, swparams, period_size);
      }

#ifdef USE_ASYNC
      // Set up stream input callback
      // if(NULL != in_handle)
      if(NULL == out_handle)
      {
         snd_async_handler_t *handler;
         err = snd_async_add_pcm_handler(&handler, in_handle, async__cbk_in, (void*)this);
         if(err < 0)
         {
            yac_host->printf("[---] PaStream<alsa_async>::initStream<in>: snd_async_add_pcm_handler() failed. err=%d(\"%s\")\n",
                             err,
                             snd_strerror(err)
                             );
            closeHandles();
            return YAC_FALSE;
         }
      }

      // Set up stream output callback
      if(NULL != out_handle)
      {
         snd_async_handler_t *handler;
         err = snd_async_add_pcm_handler(&handler, out_handle, async__cbk_out, (void*)this);
         if(err < 0)
         {
            yac_host->printf("[---] PaStream<alsa_async>::initStream<out>: snd_async_add_pcm_handler() failed. err=%d(\"%s\")\n",
                             err,
                             snd_strerror(err)
                             );
            closeHandles();
            return YAC_FALSE;
         }
      }
#endif

      yac_host->printf("[dbg] PaStream<alsa_async>::initStream: bufSize=%u\n",
                       (sUI)bufferSize
                       );

      yac_host->printf("[dbg] PaStream<alsa_async>::initStream: bufSize=%u, periodSize=%u, #periods=%u.\n",
                       (sUI)bufferSize,
                       (sUI)periodSize,
                       (sUI)periods
                       );

      // Remember period size for async callback
      period_size = periodSize;
      num_periods = periods;

      if((0u == num_periods) && (0u != period_size))
      {
         num_periods = bufferSize / period_size;

         if(num_periods <= 1u)
         {
            num_periods = 2u;
         }

         yac_host->printf("[~~~] PaStream<alsa_async>::initStream: num_periods==0, forcing num_periods=%u/%u=%u\n",
                          bufferSize, period_size, num_periods
                          );
      }
      else if(num_periods <= 1u)
      {
         num_periods = 2u;

         yac_host->printf("[~~~] PaStream<alsa_async>::initStream: num_periods<=1, forcing num_periods=2\n");
      }

      if(NULL != in_handle)
      {
         err = snd_pcm_prepare(in_handle);
         Dprintf("[trc] PaStream<alsa_async>::initStream<in>: snd_pcm_prepare() returned %d\n", err);
         if(err < 0)
         {
            yac_host->printf("[---] PaStream<alsa_async>::initStream<in>: snd_pcm_prepare() failed. err=%d(\"%s\")\n",
                             err,
                             snd_strerror(err)
                             );
            closeHandles();
            return YAC_FALSE;
         }
      }

      if(NULL != out_handle)
      {
         err = snd_pcm_prepare(out_handle);
         Dprintf("[trc] PaStream<alsa_async>::initStream<out>: snd_pcm_prepare() returned %d\n", err);
         if(err < 0)
         {
            yac_host->printf("[---] PaStream<alsa_async>::initStream<out>: snd_pcm_prepare() failed. err=%d(\"%s\")\n",
                             err,
                             snd_strerror(err)
                             );
            closeHandles();
            return YAC_FALSE;
         }
      }
   }

//    printf("[dbg] #frag=%d latency=%d samples (%f millisec) rate=%d\n",
//           PAR_FRAG,
//           PAR_LATENCY,
//           (((float)PAR_LATENCY) / PAR_RATE) * 1000,
//           PAR_RATE
//           );


   // Fill initial out_ringbuf data
   {
      YAC_FloatArray *ip = input_buf;
      YAC_FloatArray *op = output_buf;
      sUI sz = sizeof(sF32) * period_size * num_in_ch;
      ip->num_elements = period_size * num_in_ch;
      ::memset((void*)ip->elements, 0, sz);  // clear input
      callStreamCallback(period_size);

      // Copy to out_ringbuf
      sz = sizeof(sF32) * period_size * num_out_ch;
      ::memcpy((void*)out_ringbuf, (const void*)op->elements, sz);
      out_ringbuf_avail    = period_size;
      out_ringbuf_writeoff = period_size * num_out_ch;
   }

#ifndef USE_ASYNC
   // Create input thread
   if(NULL != in_handle)
   {
      if(0 == pthread_create(&pthread_id_in, NULL, in__threadentry, (void*)this))
      {
         yac_host->printf("[dbg] PaStream<alsa_async>::initStream: input thread started.\n");
      }
      else
      {
         yac_host->printf("[---] PaStream<alsa_async>::initStream: input pthread_create() failed.\n");
         closeHandles();
         return YAC_FALSE;
      }
   }

   // Create output thread
   if(NULL != out_handle)
   {
      if(0 == pthread_create(&pthread_id_out, NULL, out__threadentry, (void*)this))
      {
         yac_host->printf("[dbg] PaStream<alsa_async>::initStream: output thread started.\n");
      }
      else
      {
         yac_host->printf("[---] PaStream<alsa_async>::initStream: output pthread_create() failed.\n");
         closeHandles();
         return YAC_FALSE;
      }
   }
#endif // USE_ASYNC

   // Create worker thread
   {
      if(0 == pthread_create(&pthread_id, NULL, pastream__threadentry, (void*)this))
      {
         yac_host->printf("[dbg] PaStream<alsa_async>::initStream: thread started.\n");
         return YAC_TRUE;
      }
      else
      {
         yac_host->printf("[---] PaStream<alsa_async>::initStream: pthread_create() failed.\n");
         closeHandles();
         return YAC_FALSE;
      }
   }
}

sBool _PaStream::initStream(_PortAudio *_pa,
                            void *_cbk,
                            _PaStreamParameters *_inParams,
                            _PaStreamParameters *_outParams,
                            sUI _sampleRate,
                            sUI _framesPerBuffer
                           )
{
#ifdef USE_MUTEX
   if(NULL == mtx_async)
   {
      mtx_async = yac_host->yacMutexCreate();
   }
#endif // USE_MUTEX

   if(_framesPerBuffer < 64u)
      _framesPerBuffer = 64u;
   else if(_framesPerBuffer > 256u)
      _framesPerBuffer = 256u;

   sUI numPeriods = 2u;
   sUI periodSz   = _framesPerBuffer;

   if(periodSz < 64u)
      periodSz = 64u;
   else if(periodSz > 256u)
      periodSz = 256u;

   sUI bufSz = sUI(_outParams->suggested_latency * _sampleRate);

   if(bufSz <= 64u)
   {
      numPeriods = 2u;
      periodSz   = 32u;
   }
   else if(bufSz <= (128u + (192u-128u)/2u)/*160*/)
   {
      numPeriods = 2u;
      periodSz   = 64u;
   }
   else if(bufSz <= (192u + (256u - 192u)/2u)/*224*/)
   {
      numPeriods = 3u;
      periodSz   = 64u;
   }
   // else if(bufSz <= (256u + (320u - 256u)/2u))
   // {
   //    numPeriods = 4u;
   //    periodSz   = 64u;
   // }
   else
   {
      numPeriods = bufSz / periodSz;
      if(numPeriods < 2u)
         numPeriods = 2u;
   }

   yac_host->printf("[trc] PaStream<alsa_async>::initStream: suggested latency is %u frames @%uHz => #per=%u #perSz=%u\n", bufSz, _sampleRate, numPeriods, periodSz);

   if(!initStream2(_pa,
                   _cbk,
                   _inParams,
                   _outParams,
                   _sampleRate,
                   numPeriods,
                   periodSz,
                   (numPeriods * periodSz)/*bufferSize*/
                   ))
   {
      // if(!initStream2(_cbk, _inParams, _outParams, _sampleRate,
      //                 2, 128, 256
      //                 ))
      {
         // if(!initStream2(_cbk, _inParams, _outParams, _sampleRate,
         //                 2, 256, 512
         //                 ))
         {
            if(!initStream2(_pa, _cbk, _inParams, _outParams, _sampleRate,
                            2, 320, 640
                            ))
            {
               return initStream2(_pa, _cbk, _inParams, _outParams, _sampleRate,
                                  0, 0, 0
                                  );
            }
         }
      }
   }

   return YAC_TRUE;
}

sSI _PaStream::pcmAvailUpdateRecover(snd_pcm_t *handle) {
   sSI avail = (sSI)snd_pcm_avail_update(handle);

   if(avail < 0)
   {
      snd_pcm_recover(handle, avail/*err*/, 1/*silent*/);

      avail = (sSI)snd_pcm_avail_update(handle);

      if(avail < 0)
      {
         yac_host->printf("[---] snd_pcm_recover<avail%s> failed: err=%d\n", (handle == in_handle)?"In":"Out", (int)avail);
      }
   }

   return avail;
}

void _PaStream::_close(void) {
   if(b_running)
   {
      if(NULL != out_handle)
      {
         if(b_in_running)
         {
            *(volatile sBool*)&b_in_running = YAC_FALSE;
            pthread_join(pthread_id_in, NULL);
            yac_host->printf("[dbg] PaStream<alsa_async>::close: input thread terminated.\n");
         }

         if(b_out_running)
         {
            *(volatile sBool*)&b_out_running = YAC_FALSE;
            pthread_join(pthread_id_out, NULL);
            yac_host->printf("[dbg] PaStream<alsa_async>::close: output thread terminated.\n");
         }

         waitProcessCbk();

         *(volatile sBool*)&b_running = YAC_FALSE;

         // Wait until worker thread has exited
         pthread_join(pthread_id, NULL);
         yac_host->printf("[dbg] PaStream<alsa_async>::close: worker thread terminated.\n");

         _stop();

         closeHandles();
      }

      if(NULL != stream_cbk_context)
      {
         yac_host->yacContextDestroy(stream_cbk_context);
      }

      YAC_DELETE_SAFE(input_buf);
      YAC_DELETE_SAFE(output_buf);
   }

   sample_format = 0;
   sample_rate   = 0u;
   num_in_ch     = 0u;
   num_out_ch    = 0u;
}

void _PaStream::callStreamCallback(const sUI _numFrames) {
   b_processing_cbk = YAC_TRUE;

   // Clear output buffer
   YAC_FloatArray *ip = input_buf;
   const sUI numSamples = _numFrames * num_in_ch;
   if(numSamples <= ip->max_elements)
   {
      ip->num_elements = numSamples;

      YAC_FloatArray *op = output_buf;
      const sUI numSamples = _numFrames * num_out_ch;
      if(numSamples <= op->max_elements)
      {
         op->num_elements = numSamples;

         sF32 *d = op->elements;
         for(sUI i = 0u; i < numSamples; i++)
            *d++ = 0.0f;

         // Call script fxn to fill interleaved output buffer
         YAC_Value args[4];
         args[0].initObject(this, 0);
         args[1].initObject(ip, 0);
         args[2].initObject(op, 0);
         args[3].initInt((sSI)_numFrames);
         yac_host->yacEvalFunction(stream_cbk_context, stream_cbk, 4u, args);
      }
      else 
      {
         yac_host->printf("[~~~] PaStream<alsa>::callStreamCallback: invalid output numSamples (%u > %u)\n", numSamples, op->max_elements);
      }
   }
   else 
   {
      yac_host->printf("[~~~] PaStream<alsa>::callStreamCallback: invalid input numSamples (%u > %u)\n", numSamples, ip->max_elements);
   }

   b_processing_cbk = YAC_FALSE;
}

void _PaStream::_start(void) {
   if(NULL != out_handle)
   {
#if 0
      ::gettimeofday(&tv_last, NULL);
#endif
      // Thread will call snd_pcm_start()  (workaround)
      Dprintf("[trc] PaStream<alsa_async>::start: stream start queued.\n");
      b_active = YAC_TRUE;
   }
}

void _PaStream::waitProcessCbk(void) {
   sUI waitCnt = 0;

   while(b_processing_cbk)
   {
      usleep(1000);

      if(waitCnt++ > 2000)
      {
         printf("[---] PaStream::stop: async_cbk still busy after 2s!\n");
         break;
      }
   }
}

void _PaStream::_stop(void) {
   if(NULL != out_handle)
   {
      if(b_active)
      {
         b_active = YAC_FALSE;

         int err = snd_pcm_drop(out_handle);
         if(err < 0)
         {
            yac_host->printf("[---] PaStream<alsa_async>::stop: snd_pcm_drop() failed. err=%d(\"%s\")\n", err, snd_strerror(err));
         }

         usleep(250*1000); // why ?

#if 0
         int err = snd_pcm_pause(out_handle, 1);
         if(err < 0)
         {
            yac_host->printf("[---] PaStream<alsa_async>::stop: snd_pcm_pause() failed. err=%d(\"%s\")\n", err, snd_strerror(err));
         }
         else
         {
            // ok
            yac_host->printf("[...] PaStream<alsa_async>::stop: stream stopped.\n");
         }


         waitProcessCbk();

         usleep(250*1000); // why ?
#endif
      }
   }
}

void _PaStream::_abort(void) {
}

sSI _PaStream::_isStopped(void) {
   return b_active;
}

sSI _PaStream::_isActive(void) {
   return (NULL != out_handle);
}

sF32 _PaStream::_getCpuLoad(void) {
   return 0.0f; // (todo)
}

sSI _PaStream::_getNumInputChannels(void) {
   return num_in_ch;
}

sSI _PaStream::_getNumOutputChannels(void) {
   return num_out_ch;
}

sF32 _PaStream::_getInputLatency(void) {
   if(NULL != in_handle)
   {
      return (num_periods * period_size) / (sF32)sample_rate;
   }
   else
   {
      return 0.0f;
   }
}

sF32 _PaStream::_getOutputLatency(void) {
   if(NULL != out_handle)
   {
      return (num_periods * period_size) / (sF32)sample_rate;
   }
   else
   {
      return 0.0f;
   }
}

sF32 _PaStream::_getSampleRate(void) {
   if(NULL != out_handle)
   {
      return (sF32)sample_rate;
   }
   else
   {
      return 0.0f;
   }
}

void _PaStream::_macOSAudioWorkgroupJoin(YAC_Value *_r) {
   _r->initNull();
}

void _PaStream::_macOSAudioWorkgroupLeave(YAC_Object *_joinToken) {
   (void)_joinToken;
}
