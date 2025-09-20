/// MIDITimer.cpp
///
/// (c) 2006-2018 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///
/// created: 08Sep2006
/// changed: 15May2010, 19Jul2010, 28Sep2010, 08Mar2014, 24Aug2015, 04Oct2017, 19Jan2018
///
///
///

#include <windows.h>
#include <yac.h>

#include "MIDITimer.h"


static LARGE_INTEGER pfcFrequency;


YAC_MutexHandle MIDITimer::mtx = NULL;

#ifdef BUILD_64
void CALLBACK winmm__timer_cbk(UINT uTimerId, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2) {
#else
#define __DWORD_PTR DWORD
void __stdcall winmm__timer_cbk(UINT uTimerId, UINT uMsg, __DWORD_PTR dwUser, __DWORD_PTR dw1, __DWORD_PTR dw2) {
#endif // BUILD_64

   // yac_host->printf("xxx winmm__timer_cbk: ENTER\n");

   volatile MIDITimer *self = (MIDITimer*)dwUser;

   // Boost thread priority
   SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL/*15*/);

   yac_host->yacMutexLock(MIDITimer::mtx);

   // yac_host->printf("xxx winmm__timer_cbk: mutex locked\n");

   if(self->b_running)
   {
      if(uTimerId == self->timer_handle)
      {
         sF64 t = ((MIDITimer*)self)->getMilliSecondsDouble();
         sF64 dt;

         if(self->t_last < 1)
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

            //midiout_hdr.lpData = (char*) mdat__noteoffon;
            //midiout_hdr.dwBufferLength = midiout_hdr.dwBytesRecorded = sizeof(mdat__noteoffon);
            //midiOutPrepareHeader(midiout_handle, &midiout_hdr, sizeof(MIDIHDR));
            //midiOutLongMsg(midiout_handle, &midiout_hdr, sizeof(MIDIHDR));
            //midiOutUnprepareHeader(midiout_handle, &midiout_hdr, sizeof(MIDIHDR));

            //midiOutShortMsg(midiout_handle, 0x00C0); // ProgChange 0
            //midiOutShortMsg(midiout_handle, 0x403280); // Noteoff
            //midiOutShortMsg(midiout_handle, 0x403290); // noteOn

            //midiOutShortMsg(midiout_handle, 0x403399); // noteOn
         }

      }
   }

   // yac_host->printf("xxx winmm__timer_cbk: mutex unlocked\n");

   yac_host->yacMutexUnlock(MIDITimer::mtx);

   // yac_host->printf("xxx winmm__timer_cbk: LEAVE\n");
}


MIDITimer::MIDITimer(void) {
   timer_handle   = 0;
   sig_timer      = NULL;
   delay          = 0;
   interval       = 0.0f;
   script_context = NULL;
   b_running      = YAC_FALSE;
   ////mtx            = NULL;
}

MIDITimer::~MIDITimer() {
   stop();
}

sBool MIDITimer::open(YAC_Object *_name) {
   TIMECAPS tc;
   // UINT     wTimerRes;
   sBool ret;

   if(TIMERR_NOERROR == timeGetDevCaps(&tc, sizeof(TIMECAPS))) 
   {
      yac_host->printf("[dbg] MIDITimer::open: wPeriodMin=%u wPeriodMax=%u\n",
                       tc.wPeriodMin,
                       tc.wPeriodMax
                       );

      MMRESULT err = timeBeginPeriod(1/*millisecs*/);

      yac_host->printf("[dbg] timeBeginPeriod(1) return MMRESULT=%0x%08x\n", err);

      ret = YAC_TRUE;
   }
   else
   {
      yac_host->printf("[---] MIDITimer::open: timeGetDevCaps() failed.\n");

      ret = YAC_FALSE;
   }

   return ret;
}

sBool MIDITimer::setMutexOverrideByName(YAC_String *_name) {
   if(YAC_Is_String(_name))
   {
      YAC_CAST_ARG(YAC_String, name, _name);

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

   ::QueryPerformanceFrequency(&pfcFrequency);
   
   script_context = yac_host->yacContextCreate();
   
   delay = _delayMs;
   interval = _intervalMs;
   if(delay<1) delay=1;
   if(interval<0) interval=0.0f;
   t_last = 0.0f;
   t_tick_count = 0.0f;

   timer_handle = timeSetEvent(1,//16,//delay,
                               1/*(sUI)interval*/,
                               winmm__timer_cbk,
                               (DWORD_PTR)this,
                               TIME_PERIODIC //| TIME_CALLBACK_FUNCTION
                               );
   
   if(timer_handle == 0)
   {
      yac_host->printf("[---] MIDITimer::start: failed to set multimedia timer.\n");
      yac_host->yacMutexUnlock(mtx);
      return 0;
   }
   else
   {
      *(volatile sBool*)&b_running = YAC_TRUE;
      yac_host->yacMutexUnlock(mtx);
      return 1;
   }
}

void MIDITimer::update(sF32 _intervalMS) {
   interval = _intervalMS;
   if(interval<0) interval=0.0f;
}

void MIDITimer::stop(void) {

   // printf("xxx MIDITimer::stop: mtx=%p mtx_miditimer=%p\n", mtx, mtx_miditimer);

   if(NULL == mtx)
   {
      mtx = mtx_miditimer;
   }

   *(volatile sBool *)&b_running = YAC_FALSE;

   // yac_host->printf("xxx MIDITimer::stop: ENTER\n");

   yac_host->yacMutexLock(mtx);

   // yac_host->printf("xxx MIDITimer::stop: mutex locked\n");

   if(timer_handle)
   {
      // Stop timer
      timeKillEvent(timer_handle);
      timer_handle = 0;
   }

   if(script_context)
   {
      yac_host->yacContextDestroy(script_context);
      script_context = NULL;
   }

   // yac_host->printf("xxx MIDITimer::stop: unlock mutex\n");

   yac_host->yacMutexUnlock(mtx);

   // yac_host->printf("xxx MIDITimer::stop: unlock LEAVE\n");
}

void MIDITimer::close(void) {
   stop();

   timeEndPeriod(1/*millisecs*/);
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

   return (sF64)timeGetTime();

#if 0
   LARGE_INTEGER pfcCurrent;
   if( ::QueryPerformanceCounter(&pfcCurrent) )
   {
      sF64 s = ((sF64)pfcCurrent.QuadPart) / ((sF64)pfcFrequency.QuadPart);
     
      //return (1000.0 * (s- ((sSI)s)));
      return 1000.0 * s;
   }
   else
   {
      return 0.0;
   }
#endif

}
