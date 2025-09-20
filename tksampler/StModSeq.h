// ----
// ---- file   : StModSeq.h
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2020-2024 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : This is part of the "syntracker" midi sequencer.
// ----
// ---- created: 15Feb2020
// ---- changed: 16Feb2020, 17Feb2020, 18Feb2020, 19Feb2020, 20Feb2020, 03Sep2020, 12Sep2020
// ----          23Sep2020, 13Oct2020, 01Aug2021, 01Oct2024, 03Oct2024
// ----
// ----
// ----

#ifndef ST_MODSEQ_H__
#define ST_MODSEQ_H__

YG("tksampler")


/* @class StModSeq,Object
 */
YC class StModSeq : public YAC_Object {
  public:
#define STMODSEQ_MAX_STEPS  YCI  16

#define STMODSEQ_PLAYMODE_FWD           YCI  0
#define STMODSEQ_PLAYMODE_BWD           YCI  1
#define STMODSEQ_PLAYMODE_PINGPONG      YCI  2
#define STMODSEQ_PLAYMODE_PINGPONG_NR   YCI  3   // don't repeat first/last
#define STMODSEQ_PLAYMODE_RANDOM        YCI  4
#define STMODSEQ_PLAYMODE_RANDOM_ND     YCI  5   // no doubles (don't repeat last val)
#define STMODSEQ_PLAYMODE_RANDOM_NR     YCI  6   // no repeats (entire sequence)
   sUI play_mode;

#define STMODSEQ_STEPMODE_TIME            YCI  0   // auto-advance (depending on duration+speed)
#define STMODSEQ_STEPMODE_NOTEON          YCI  1   // advance at note-on
#define STMODSEQ_STEPMODE_NOTEOFF         YCI  2   // advance at note-off
#define STMODSEQ_STEPMODE_MOD_RISE        YCI  3   // advance only via modulation (rising edge)
#define STMODSEQ_STEPMODE_MOD_FALL        YCI  4   // advance only via modulation (falling edge)
#define STMODSEQ_STEPMODE_MOD_EDGE        YCI  5   // advance only via modulation (rising or falling edge)
#define STMODSEQ_STEPMODE_MOD_XCROSS      YCI  6   // advance only via modulation (zero crossing, e.g. LFO)
#define STMODSEQ_STEPMODE_START_MOD_NZ    YCI  7   // start sequence not until "advance" exceeds 0
#define STMODSEQ_STEPMODE_RESTART_MOD_NZ  YCI  8   // (re-)start sequence (not until / when) "advance" exceeds 0
   sUI step_mode;


   sUI  num_steps;  // 1..16
   sUI  repeat_offset;  // 0..15  (mainly a "hack" for the Tiny Synergy Replayer on Amiga, does not work with pingpong modes)

   sF32 step_values[STMODSEQ_MAX_STEPS];
   sU8  step_durations[STMODSEQ_MAX_STEPS];  // 0..15

   // (note) 1=1 millisec (1 audio tick) or tempo-based seq tick (2.5ms=1 seq tick @ 125bpm/192ppq)
   sF32  speed;     // step duration (#ticks)
   sBool b_tempo_sync;  // 1=use sequencer tick duration as timing base, 0=use audio ticks (1 millisec)

   sF32 value_scl;
   sF32 value_off;

   sF32 slew_amt;  // 0.0=no slew .. 1.0=no change

   sBool b_oneshot;
   sBool b_global;
   sBool b_reset;   // 1=reset step idx when seq is started, 0=continue with prev idx (pseudo-random), -1(maybe)=don't start seq until retrig
   sBool b_pulse;   // true=reset output to 0 after playing new event, maybe=reset to -1

   sSI ui_last_played_idx;
   void *ui_last_started_voice;   // StSampleVoice ref (never dereferenced). used when updating UI modseq pos.

  public:
   sF32 readModuloIndexedValue (sUI _index);
   sF32 readIndexedValue (sUI _index);

  public:
   StModSeq(void);
   ~StModSeq();

   YAC(StModSeq);

   YM void reset (void);

   YM void _setPlayMode (sUI _mode);
   YM sUI  _getPlayMode (void);

   YM void _setStepMode (sUI _mode);
   YM sUI  _getStepMode (void);

   YM void _setNumSteps (sUI _num);
   YM sUI  _getNumSteps (void);

   YM void _setRepeatOffset (sUI _off);
   YM sUI  _getRepeatOffset (void);

   YM void _setStepValue (sUI _idx, sF32 _val);
   YM sF32 _getStepValue (sUI _idx);
   YM sF32 _getEffectiveStepValue (sUI _idx);  // after scl+off, for UI

   YM void _setStepDuration (sUI _idx, sUI _dur);
   YM sUI  _getStepDuration (sUI _idx);

   YM void _setSpeed (sF32 _speed);
   YM sF32 _getSpeed (void);

   YM void  _setEnableTempoSync (sBool _bEnabled);
   YM sBool _getEnableTempoSync (void);

   YM void _setValueScl (sF32 _scl);
   YM sF32 _getValueScl (void);

   YM void _setValueOff (sF32 _off);
   YM sF32 _getValueOff (void);

   YM void _setSlewAmt (sF32 _amt);
   YM sF32 _getSlewAmt (void);

   YM void  _setEnableOneShot (sBool _bEnabled);
   YM sBool _getEnableOneShot (void);

   YM void  _setEnableGlobal (sBool _bEnabled);
   YM sBool _getEnableGlobal (void);

   YM void  _setEnableReset (sBool _bEnabled);
   YM sBool _getEnableReset (void);

   YM void  _setEnablePulse (sBool _bEnabled);
   YM sBool _getEnablePulse (void);

   YM sSI _uiGetLastPlayedIdx (void);
};


#endif // ST_MODSEQ_H__
