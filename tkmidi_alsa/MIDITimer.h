/// MIDITimer.h
///
/// (c) 2006-2014 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///
/// changed: 08Sep2006, 12Apr2010, 28Sep2010, 08Mar2014, 25Aug2014
///
///
///

#ifndef __MIDITIMER_H__
#define __MIDITIMER_H__

YG("midi");

extern YAC_MutexHandle mtx_miditimer;


YC class MIDITimer : public YAC_Object {
public:
   sF64 t_tick_count;

   sSI delay;
   sF32 interval;

   void *sig_timer;

   YAC_ContextHandle script_context;

   pthread_t pthread_id;

   volatile sBool b_running;
   volatile sBool b_thread_started;
   volatile sBool b_thread_done;

   volatile sBool b_timer_paused;
   volatile sBool b_timer_paused_ack;

   struct timeval tv_last;

   YAC_MutexHandle mtx_tick; // default is internal mtx_miditimer but some applications may need to override this to avoid deadlocks

   snd_timer_t *timer;
   long         timer_res;


public:
   YAC(MIDITimer);

   MIDITimer(void);
   ~MIDITimer();

   void YAC_VCALL yacGetSignalStringList(YAC_String *_sig);
   void YAC_VCALL yacRegisterSignal(sUI _id, YAC_FunctionHandle _fun);

   sF64 getMilliSecondsDouble(void);

   void waitThreadCond(volatile sBool *_bCond, sBool _bInv, const char *_name);

public:
   YM sBool setMutexOverrideByName(YAC_String *_name);
   YM sBool open   (YAC_Object *_name);
   YM sBool start  (sSI _delayMs, sF32 _intervalMs, YAC_Object *_funObject);
   YM void  update (sF32 _intervalMs);
   YM void  stop   (void);
   YM void  close  (void);

};


#endif // __MIDITIMER_H__
