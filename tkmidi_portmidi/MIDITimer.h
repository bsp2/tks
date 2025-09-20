/// MIDITimer.h
///
/// (c) 2006-2023 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///
/// created: 08Sep2006
/// changed: 14May2010, 15May2010, 19Jul2010, 28Sep2010, 08Mar2014, 29Aug2014, 24Aug2015
///          20Jan2023, 01Feb2023
///
///
///

#ifndef __MIDITIMER_H__
#define __MIDITIMER_H__

YG("midi");


extern YAC_MutexHandle mtx_miditimer;


/* @class MIDITimer,Object

A MIDI timer that has a resolution of ~1 millisecond.
*/
YC class MIDITimer : public YAC_Object {
public:
   sF64 t_last;
   sF64 t_tick_count;

   sSI delay;
   sF32 interval;

   YAC_FunctionHandle sig_timer;

   YAC_ContextHandle script_context;

   static sBool b_running;
   static sBool b_pt_running;

   static YAC_MutexHandle mtx; // default is internal mtx_miditimer but some applications may need to override this to avoid deadlocks


public:
   YAC(MIDITimer);

   MIDITimer(void);
   ~MIDITimer();

   void YAC_VCALL yacGetSignalStringList(YAC_String *_sig);
   void YAC_VCALL yacRegisterSignal(sUI _id, YAC_FunctionHandle _fun);

   sF64 getMilliSecondsDouble(void);

public:
   YM sBool setMutexOverrideByName(YAC_String *_name);
   YM sBool open   (YAC_Object *_name);
   YM sBool start  (sSI _delayMs, sF32 _intervalMs, YAC_Object *_funObject);
   YM void  update (sF32 _intervalMs);
   YM void  stop   (void);
   YM void  close  (void);

};


#endif // __MIDITIMER_H__
