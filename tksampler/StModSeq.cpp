// ----
// ---- file   : StModSeq.cpp
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2020-2021 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : This is part of the "syntracker" midi sequencer.
// ----
// ---- created: 15Feb2020
// ---- changed: 16Feb2020, 17Feb2020, 19Feb2020, 20Feb2020, 03Sep2020, 23Sep2020, 13Oct2020
// ----          01Aug2021
// ----
// ----
// ----

#include "tksampler.h"

#include "StModSeq.h"


StModSeq::StModSeq(void) {
   reset();
}

StModSeq::~StModSeq() {
}

void StModSeq::reset(void) {
   play_mode = STMODSEQ_PLAYMODE_FWD;
   step_mode = STMODSEQ_STEPMODE_TIME;
   num_steps = 8u;
   repeat_offset = 0u;

   memset((void*)step_values,    0, sizeof(step_values));
   memset((void*)step_durations, 0, sizeof(step_durations));

   speed = 48.0f;
   b_tempo_sync = YAC_TRUE;

   value_scl = 1.0f;
   value_off = 0.0f;

   slew_amt = 0.0f;

   b_oneshot = YAC_FALSE;
   b_global  = YAC_FALSE;
   b_reset   = YAC_TRUE;
   b_pulse   = YAC_FALSE;

#if 0
   // Debug:
   num_steps = 5;
   step_values[0] = 0.0f;
   step_values[1] = 1.0f;
   step_values[2] = -1.0f;
   step_values[3] = 0.5f;
   step_values[4] = -0.5f;

   for(sUI i = 0u; i < 5; i++)
      step_durations[i] = 0;

   speed = 60.0f;
#endif

   ui_last_played_idx = -1;
   ui_last_started_voice = NULL;
}

void StModSeq::_setPlayMode(sUI _mode) {
   play_mode = _mode;
}

sUI StModSeq::_getPlayMode(void) {
   return play_mode;
}

void StModSeq::_setStepMode(sUI _mode) {
   step_mode = _mode;
}

sUI StModSeq::_getStepMode(void) {
   return step_mode;
}

void StModSeq::_setNumSteps(sUI _num) {
   num_steps = _num;

   if(num_steps < 1u)
      num_steps = 1u;
   else if(num_steps > STMODSEQ_MAX_STEPS)
      num_steps = STMODSEQ_MAX_STEPS;
}

sUI StModSeq::_getNumSteps(void) {
   return num_steps;
}

void StModSeq::_setRepeatOffset(sUI _off) {
   repeat_offset = _off;

   if(repeat_offset > (STMODSEQ_MAX_STEPS - 1u))
      repeat_offset = STMODSEQ_MAX_STEPS - 1u;
}

sUI StModSeq::_getRepeatOffset(void) {
   return repeat_offset;
}

void StModSeq::_setStepValue(sUI _idx, sF32 _val) {
   if(_idx < STMODSEQ_MAX_STEPS)
      step_values[_idx] = _val;
}

sF32 StModSeq::_getStepValue(sUI _idx) {
   if(_idx < STMODSEQ_MAX_STEPS)
      return step_values[_idx];
   else
      return 0.0f;
}

sF32 StModSeq::_getEffectiveStepValue(sUI _idx) {
   sF32 r = _getStepValue(_idx);
   r = r * value_scl + value_off;
   return r;
}

void StModSeq::_setStepDuration(sUI _idx, sUI _dur) {
   if(_idx < STMODSEQ_MAX_STEPS)
      step_durations[_idx] = _dur;
}

sUI StModSeq::_getStepDuration(sUI _idx) {
   if(_idx < STMODSEQ_MAX_STEPS)
      return step_durations[_idx];
   else
      return 0u;
}

void StModSeq::_setSpeed(sF32 _speed) {
   speed = _speed;
}

sF32 StModSeq::_getSpeed(void) {
   return speed;
}

void StModSeq::_setEnableTempoSync(sBool _bEnabled) {
   b_tempo_sync = _bEnabled;
}

sBool StModSeq::_getEnableTempoSync(void) {
   return b_tempo_sync;
}

void StModSeq::_setValueScl(sF32 _scl) {
   value_scl = _scl;
}

sF32 StModSeq::_getValueScl(void) {
   return value_scl;
}

void StModSeq::_setValueOff(sF32 _off) {
   value_off = _off;
}

sF32 StModSeq::_getValueOff(void) {
   return value_off;
}

void StModSeq::_setSlewAmt(sF32 _amt) {
   slew_amt = sRANGE(_amt, 0.0f, 1.0f);
}

sF32 StModSeq::_getSlewAmt(void) {
   return slew_amt;
}

void StModSeq::_setEnableOneShot(sBool _bEnabled) {
   b_oneshot = _bEnabled;
}

sBool StModSeq::_getEnableOneShot(void) {
   return b_oneshot;
}

void StModSeq::_setEnableGlobal(sBool _bEnabled) {
   b_global = _bEnabled;
}

sBool StModSeq::_getEnableGlobal(void) {
   return b_global;
}

void StModSeq::_setEnableReset(sBool _bEnabled) {
   b_reset = _bEnabled;
}

sBool StModSeq::_getEnableReset(void) {
   return b_reset;
}

void StModSeq::_setEnablePulse(sBool _bEnabled) {
   b_pulse = _bEnabled;
}

sBool StModSeq::_getEnablePulse(void) {
   return b_pulse;
}

sSI StModSeq::_uiGetLastPlayedIdx(void) {
   return ui_last_played_idx;
}

sF32 StModSeq::readModuloIndexedValue(sUI _index) {
   sUI i = _index % num_steps;
   ui_last_played_idx = i;
   sF32 r = step_values[i];
   r = (r * value_scl) + value_off;
   return r;
}

sF32 StModSeq::readIndexedValue(sUI _index) {
   sUI i = _index & (STMODSEQ_MAX_STEPS - 1u)/*15u*/;
   ui_last_played_idx = i;
   sF32 r = step_values[i];
   r = (r * value_scl) + value_off;
   return r;
}
