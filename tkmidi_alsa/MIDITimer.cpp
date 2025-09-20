/// MIDITimer.cpp
///
/// (c) 2006-2015 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///
/// changed: 08Sep2006, 12Apr2010, 25Jul2010, 28Sep2010, 08Mar2014, 25Aug2014, 26Jan2015
///
///
/// also see: http://www.alsa-project.org/alsa-doc/alsa-lib/_2test_2timer_8c-example.html
///
///
///  (note) # modprobe snd_hrtimer
///
///

#include <yac.h>

#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "tkmidi.h"

#include "MIDITimer.h"


void *miditimer__threadentry(void *_self) {

   MIDITimer *self = (MIDITimer*)_self;

   int err;

   // Increase thread priority
   {
      struct sched_param param;
      memset((void*)&param, 0, sizeof(struct sched_param));
      param.sched_priority = 40;//25;
      int err = pthread_setschedparam(self->pthread_id, SCHED_FIFO, &param);
      if(0 != err)
      {
         printf("[---] MIDITimer<oss>: pthread_setschedparam() failed, errno=%d(\"%s\")\n", errno, strerror(errno));
      }
   }

   err = snd_timer_start(self->timer);

   if(0 == err)
   {
      snd_timer_read_t tr;

      self->b_thread_started = YAC_TRUE;

      while(self->b_running)
      {
         if(!self->b_timer_paused)
         {
            if(snd_timer_read(self->timer, &tr, sizeof(tr)) == sizeof(tr))
            {
               if(self->sig_timer)
               {
                  YAC_Value args[1];
                  args[0].initObject(self, 0);
                  yac_host->yacMutexLock(self->mtx_tick);

                  // Set default context in case the script function calls a native fxn/method, which
                  //  again calls another script method..  (MIDIPipeNode!)
                  yac_host->yacContextSetDefault(self->script_context);

                  yac_host->yacEvalFunction(self->script_context, self->sig_timer, 1, args);
                  yac_host->yacMutexUnlock(self->mtx_tick);
               }

               self->t_tick_count = self->t_tick_count + 1.0f;
            }
         }
         else
         {
            self->b_timer_paused_ack = YAC_TRUE;
         }
      }

   }
   else
   {
      yac_host->printf("[---] MIDITimer::start: snd_timer_start() failed. err=%d(\"%s\").\n",
                       err, snd_strerror(err)
                       );
   }

   yac_host->printf("[dbg] MIDITimer::<thread>: 1 exiting.\n");

   snd_timer_stop(self->timer);

   self->b_thread_done = YAC_TRUE;

   yac_host->printf("[dbg] MIDITimer::<thread>: 2 exiting.\n");

   return NULL;
}


MIDITimer::MIDITimer(void) {
   sig_timer        = NULL;
   delay            = 0;
   interval         = 0.0f;
   script_context   = NULL;
   b_running        = YAC_FALSE;
   timer            = NULL;
   timer_res        = 1;
   b_thread_started = YAC_FALSE;
   b_thread_done    = YAC_TRUE;
   mtx_tick         = NULL;
}

MIDITimer::~MIDITimer() {
   close();
}

sBool MIDITimer::open(YAC_Object *_name) {
   const char *timerName;
   char defTimerName[128];

   yac_host->printf("xxx MIDITimer::open: ENTER\n");

   close();

   if(!YAC_VALID(_name))
   {
      sprintf(defTimerName, 
              "hw:CLASS=%i,SCLASS=%i,CARD=%i,DEV=%i,SUBDEV=%i",
              SND_TIMER_CLASS_GLOBAL,  // class
              SND_TIMER_SCLASS_NONE,   // sub-class
              0,                       // card
              ////SND_TIMER_GLOBAL_SYSTEM, // device  (very bad res, >4ms)
              SND_TIMER_GLOBAL_HRTIMER,
              0                        // sub-device
              );

      timerName = defTimerName;
   }
   else
   {
      if(YAC_Is_String(_name))
      {
         YAC_CAST_ARG(YAC_String, name, _name);
         timerName = (const char*)name->chars;
      }
      else
      {
         timerName = NULL;
      }
   }

   if(NULL != timerName)
   {
      int err = snd_timer_open(&timer, timerName, 0/*SND_TIMER_OPEN_NONBLOCK*/);

      if(0 == err)
      {
         yac_host->printf("[...] MIDITimer::open: OK, timer=\"%s\".\n", timerName);

         snd_timer_info_t *info;
         snd_timer_info_alloca(&info);

         err = snd_timer_info(timer, info);

         if(0 == err)
         {
            timer_res = snd_timer_info_get_resolution(info);

            yac_host->printf("[dbg] MIDITimer::open: timer_res=%li\n", timer_res);

            // Succeeded
            return YAC_TRUE;
         }
         else
         {
            yac_host->printf("[---] MIDITimer::open: snd_timer_info() failed. err=%d(\"%s\").\n",
                             err, snd_strerror(err)
                             );

            snd_timer_close(timer);
         }
      }
      else
      {
         yac_host->printf("[---] MIDITimer::open: snd_timer_open(\"%s\") failed. err=%d(\"%s\").\n",
                          timerName,
                          err, snd_strerror(err)
                          );
      }
   }
   else
   {
      yac_host->printf("[---] MIDITimer::open: invalid timer name (null)\n");
   }

   // Failed
   return YAC_FALSE;
}

sBool MIDITimer::setMutexOverrideByName(YAC_String *_name) {
   if(YAC_Is_String(_name))
   {
      YAC_CAST_ARG(YAC_String, name, _name);

      YAC_MutexHandle h = yac_host->yacMutexFindByName((const char*)name->chars);

      if(NULL != h)
      {
         mtx_tick = h;

         return YAC_TRUE;
      }
   }

   return YAC_FALSE;
}

sBool MIDITimer::start(sSI _delayMs, sF32 _intervalMs, YAC_Object *_funObject) {

   if(NULL != timer)
   {
      stop();

      if(NULL == mtx_tick)
      {
         mtx_tick = mtx_miditimer;
      }

      if(YAC_VALID(_funObject))
      {
         sig_timer = (void*) _funObject;
      }
      else
      {
         sig_timer = NULL;
      }

      script_context = yac_host->yacContextCreate();
   
      delay = _delayMs;
      if(delay<1) delay=1;

      update(_intervalMs);

      t_tick_count = 0.0f;

      b_thread_done    = YAC_FALSE;
      b_thread_started = YAC_FALSE;
      b_running        = YAC_TRUE;
      
      b_timer_paused     = YAC_FALSE;
      b_timer_paused_ack = YAC_FALSE;

      if(0 == pthread_create(&pthread_id, NULL, miditimer__threadentry, (void*) this))
      {
         waitThreadCond(&b_thread_started, 0/*inv*/, "thread_started");

         return b_thread_started;
      }
      else
      {
         yac_host->printf("[---] MIDITimer::start: failed to set multimedia timer.\n");
      }
   }
   else
   {
      yac_host->printf("[---] MIDITimer::start: no timer opened.\n");
   }

   return YAC_FALSE;
}

void MIDITimer::update(sF32 _intervalMS) {
   if(NULL != timer)
   {
      if(b_running)
      {
         b_timer_paused_ack = YAC_FALSE;
         b_timer_paused = YAC_TRUE;

         waitThreadCond(&b_timer_paused_ack, 0/*inv*/, "paused_ack");

         snd_timer_stop(timer);
      }

      interval = _intervalMS;
      if(interval<0) interval=0.0f;

      int err;

      snd_timer_params_t *params;
      snd_timer_params_alloca(&params);

      long ticks = (long) (_intervalMS * (1000000.0 / timer_res));
      
      yac_host->printf("[dbg] MIDITimer::update: intervalMS=%f, reqTicks=%li\n", _intervalMS, ticks);

      // Disable one-shot mode
      snd_timer_params_set_auto_start(params, 1);

      snd_timer_params_set_ticks(params,
                                 ticks
                                 );


      if(snd_timer_params_get_ticks(params) < 1)
      {
         snd_timer_params_set_ticks(params, 1);
      }

      yac_host->printf("[dbg] MIDITimer::update: using %li tick(s)\n", snd_timer_params_get_ticks(params));

      err = snd_timer_params(timer, params);

      if(err < 0)
      {
         yac_host->printf("[---] MIDITimer::update: snd_timer_params() failed. err=%d(\"%s\").\n",
                          err, snd_strerror(err)
                          );
      }

      if(b_running)
      {
         snd_timer_start(timer);

         b_timer_paused = YAC_FALSE;
      }
   }
   else
   {
      yac_host->printf("[---] MIDITimer::update: no timer opened.\n");
   }
}

void MIDITimer::waitThreadCond(volatile sBool *_bCond, sBool _bInv, const char *_name) {
   sUI waitCnt = 0;

   // (todo) this is kind of messy X)

   while(! (*_bCond ^ _bInv))
   {
      usleep(1000);

      if(waitCnt++ > 2000)
      {
         printf("[---] MIDITimer::waitThreadCond<%s>: thread still busy after 2s!\n", _name);
         break;
      }
   }
}

void MIDITimer::stop(void) {

   if(NULL != timer)
   {
      if(b_running)
      {
         b_running = YAC_FALSE;

         waitThreadCond(&b_thread_done, 0/*inv*/, "thread_done");

         pthread_join(pthread_id, NULL);
      }

      if(script_context)
      {
         yac_host->yacContextDestroy(script_context);
         script_context = NULL;
      }
   }
   else
   {
      yac_host->printf("[---] MIDITimer::stop: no timer opened.\n");
   }
}

void MIDITimer::close(void) {
   if(NULL != timer)
   {
      stop();

      snd_timer_close(timer);

      timer = NULL;
   }
}

void YAC_VCALL MIDITimer::yacGetSignalStringList(YAC_String *_sig) {
   _sig->visit("onTimer:3 ");
}

void YAC_VCALL MIDITimer::yacRegisterSignal(sUI _id, YAC_FunctionHandle _fun) {
   switch(_id)
   {
   case 0: sig_timer      =_fun; break; // begin new timeframe
   }
}

sF64 MIDITimer::getMilliSecondsDouble(void) {

   struct timeval tv;
   ::gettimeofday(&tv, NULL);

   sF32 msDelta = (sF64) ((tv.tv_sec-tv_last.tv_sec)*1000 + ((tv.tv_usec-tv_last.tv_usec) / 1000.0));

   tv_last = tv;

   return msDelta;
}
