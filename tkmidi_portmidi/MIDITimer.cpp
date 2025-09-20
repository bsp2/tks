/// MIDITimer.cpp
///
/// (c) 2006-2023 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///
/// created: 08Sep2006
/// changed: 15May2010, 19Jul2010, 28Sep2010, 08Mar2014, 24Aug2015, 04Oct2017, 19Jan2018
///          20Jan2023, 22Jan2023, 01Feb2023, 04Feb2023
///
///
///

#include <yac.h>

#ifdef YAC_WIN32
#include <windows.h>  // SetThreadPriority, GetCurrentThread
#else
// #include <unistd.h>  // usleep
#endif // YAC_WIN32

#include "porttime.h"

#include "MIDITimer.h"

YAC_MutexHandle MIDITimer::mtx = NULL;

// (note) PortMidi timer must be started before opening any MIDI device
sBool MIDITimer::b_pt_running = YAC_FALSE;
sBool MIDITimer::b_running = YAC_FALSE;


static MIDITimer *loc_timer_cbk_userData = NULL;

static void timer_cbk(PtTimestamp timestamp, void *userDataUNUSED) {

#ifdef YAC_WIN32
   static sBool bPrioSet = YAC_FALSE;
   if(!bPrioSet)
   {
      // Boost thread priority
      //  (note) move to script ?
      SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL/*15*/);
      bPrioSet = YAC_TRUE;
   }
#endif // YAC_WIN32

   if(NULL != MIDITimer::mtx)
   {
      yac_host->yacMutexLock(MIDITimer::mtx);

      // yac_host->printf("xxx timer_cbk: mutex locked\n");

      if(MIDITimer::b_running)
      {
         // yac_host->printf("xxx timer_cbk: ENTER\n");
         volatile MIDITimer *self = loc_timer_cbk_userData;

         sF64 t = ((MIDITimer*)self)->getMilliSecondsDouble();
         sF64 dt;

         if(self->t_last < 1.0)
         {
            dt = 0.0;
         }
         else
         {
            dt = (t-self->t_last);
         }

         self->t_last = t;
         self->t_tick_count += dt;

         if(self->t_tick_count >= self->interval)
         {
            self->t_tick_count -= self->interval;

            if(self->sig_timer)
            {
               YAC_Value args[1];
               args[0].initObject((MIDITimer*)self, 0);

               // Set default context in case the script function calls a native fxn/method, which
               //  again calls another script method..  (MIDIPipeNode!)
               yac_host->yacContextSetDefault(self->script_context);

               yac_host->yacEvalFunction(self->script_context, self->sig_timer, 1, args);
            }
         }
      } // if MIDITimer::b_running

      // yac_host->printf("xxx timer_cbk: mutex unlocked\n");

      yac_host->yacMutexUnlock(MIDITimer::mtx);
   }

   // yac_host->printf("xxx timer_cbk: LEAVE\n");
}

void MIDITimer__pt_start(void) {
   PtError ptErr;
   if(!MIDITimer::b_pt_running)
   {
      ptErr = Pt_Start(1/*resolution*/, &timer_cbk, (void*)NULL);
      MIDITimer::b_pt_running = (ptNoError == ptErr) ? YAC_TRUE : YAC_FALSE;
   }
   else
   {
      // PortMidi timer is already running
      ptErr = ptNoError;
   }

   if(ptNoError != ptErr)
   {
      yac_host->printf("[---] MIDITimer<portmidi>::MIDITimer__pt_start: Pt_Start() failed (err=%d)\n", ptErr);
   }
}

void MIDITimer__pt_stop(void) {
   if(MIDITimer::b_pt_running)
   {
      MIDITimer::b_pt_running = YAC_FALSE;
      Pt_Stop();
   }
}


MIDITimer::MIDITimer(void) {
   sig_timer      = NULL;
   delay          = 0;
   interval       = 0.0f;
   script_context = NULL;
}

MIDITimer::~MIDITimer() {
   stop();
}

sBool MIDITimer::open(YAC_Object *_name) {
   (void)_name;
   return YAC_TRUE;
}

sBool MIDITimer::setMutexOverrideByName(YAC_String *_name) {
   if(YAC_Is_String(_name))
   {
      YAC_MutexHandle h = yac_host->yacMutexFindByName((const char*)_name->chars);
      if(NULL != h)
      {
         mtx = h;
         return 1;
      }
   }
   return 0;
}

sBool MIDITimer::start(sSI _delayMs, sF32 _intervalMs, YAC_Object *_funObject) {

   stop();

   // printf("xxx MIDITimer::start: mtx=%p mtx_miditimer=%p\n", mtx, mtx_miditimer);

   if(NULL == mtx)
   {
      mtx = mtx_miditimer;
   }

   yac_host->yacMutexLock(mtx);

   *(volatile sBool*)&b_running = YAC_FALSE; // (note) should already be false

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
   interval = _intervalMs;
   if(delay<1) delay=1;
   if(interval<0) interval=0.0f;
   t_last = 0.0;
   t_tick_count = 0.0f;

   if(b_pt_running)
   {
      loc_timer_cbk_userData = this;
      *(volatile sBool*)&b_running = YAC_TRUE;
      yac_host->yacMutexUnlock(mtx);
      return YAC_TRUE;
   }
   else
   {
      yac_host->printf("[---] MIDITimer<portmidi>::start: prior call to Pt_Start() failed\n");
      yac_host->yacMutexUnlock(mtx);
      return YAC_FALSE;
   }

}

void MIDITimer::update(sF32 _intervalMS) {
   interval = _intervalMS;
   if(interval < 0.0f)
      interval = 0.0f;
}

void MIDITimer::stop(void) {
   if(b_running)
   {
      sBool bDebug = (yac_host->yacGetDebugLevel() > 79);

      // printf("xxx MIDITimer::stop: mtx=%p mtx_miditimer=%p\n", mtx, mtx_miditimer);

      if(NULL == mtx)
      {
         mtx = mtx_miditimer;
      }

      *(volatile sBool *)&b_running = YAC_FALSE;

      if(bDebug) yac_host->printf("xxx MIDITimer::stop: ENTER\n");

      yac_host->yacMutexLock(mtx);

      if(bDebug) yac_host->printf("xxx MIDITimer::stop: mutex locked\n");

      if(script_context)
      {
         yac_host->yacContextDestroy(script_context);
         script_context = NULL;
      }

      if(bDebug) yac_host->printf("xxx MIDITimer::stop: unlock mutex\n");

      loc_timer_cbk_userData = NULL;

      yac_host->yacMutexUnlock(mtx);

#if 0
      Pt_Stop();
      b_pt_running = YAC_FALSE;
#endif

      if(bDebug) yac_host->printf("xxx MIDITimer::stop: unlock LEAVE\n");
   }
}

void MIDITimer::close(void) {
   stop();
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
   return (sF64)Pt_Time();
}
