// ----
// ---- file   : StModSeqPlayer.cpp
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2020-2023 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : This is part of the "syntracker" midi sequencer.
// ----
// ---- created: 15Feb2020
// ---- changed: 16Feb2020, 17Feb2020, 18Feb2020, 19Feb2020, 05Jun2020, 03Sep2020, 12Sep2020
// ----          01Aug2021, 30Aug2021, 12Apr2023
// ----
// ----
// ----

#include "tksampler.h"

#include "StModSeq.h"
#include "StModSeqPlayer.h"


StModSeqPlayer::StModSeqPlayer(void) {
   lfsr.init((void*)this);
   rand_nr.init(STMODSEQ_MAX_STEPS);  // updated when length changes (but not via modulation)

   modseq = NULL;

   step_idx         = 0u;
   step_ticks       = 0.0f;
   num_steps_played = 0u;
   b_pulse          = YAC_FALSE;
   tempo_speed_scl  = 1.0f;
   b_running        = YAC_FALSE;

   resetMod();

   sp_mod_speed    = NULL;
   sp_mod_level    = NULL;
   sp_mod_numsteps = NULL;
   sp_mod_advance  = NULL;

   last_advance = 0.0f;
   last_level = 0.0f;

   ui_sample_voice = NULL;
}

StModSeqPlayer::~StModSeqPlayer(void) {
}

void StModSeqPlayer::resetMod(void) {
   mod_speed       = 1.0f;
   mod_level       = 1.0f;
   mod_numsteps    = 1.0f;
   mod_advance     = 0.0f;

   mmdst_speed     = 1.0f;
   mmdst_numsteps  = 0.0f;
   mmdst_advance   = 0.0f;
   mmdst_step      = 0.0f;
   b_mmdst_step    = YAC_FALSE;
}

void StModSeqPlayer::startModSeq(StModSeq *_modseq, sBool _bResetMod, sF32 _tempoSpeedScl, void *_sampleVoice) {
   modseq = _modseq;
   tempo_speed_scl = _tempoSpeedScl;
   ui_sample_voice = _sampleVoice;

   if(NULL != modseq)
   {
      modseq->ui_last_started_voice = _sampleVoice;

      if(STMODSEQ_STEPMODE_NOTEON == modseq->step_mode)
      {
         nextStepIdx(calcNumSteps());
      }
      else if(modseq->b_reset)
      {
         handleRestart();
      }

      if(_bResetMod)
      {
         // Note on
         resetMod();

         b_running =
            (YAC_MAYBE != modseq->b_reset) &&
            (STMODSEQ_STEPMODE_START_MOD_NZ   != modseq->step_mode) &&  // (todo) remove these ??
            (STMODSEQ_STEPMODE_RESTART_MOD_NZ != modseq->step_mode) ;
      }
      else
      {
         // via RETRIG_MODSEQx modmatrix dest
         b_running = true;
      }
   }
}

void StModSeqPlayer::updateModSeq(StModSeq *_modseq) {
   sBool bWasNull = (NULL == modseq);
   modseq = _modseq;

   if(NULL != modseq)
      modseq->ui_last_started_voice = ui_sample_voice;

   if(bWasNull)
      startModSeq(modseq, YAC_FALSE/*bResetMod*/, tempo_speed_scl, ui_sample_voice);
}

void StModSeqPlayer::handleRestart(void) {
   step_idx         = 0u;
   step_ticks       = 0.0f;
   num_steps_played = 0u;
   b_pulse          = YAC_TRUE;
   last_level       = (YAC_MAYBE == modseq->b_pulse) ? -1 : 0.0f;
}

void StModSeqPlayer::stopModSeq(void) {
   if(NULL != modseq)
   {
      if(modseq->ui_last_started_voice == ui_sample_voice)
         modseq->ui_last_played_idx = -1;
   }
   modseq = NULL;
}

sUI StModSeqPlayer::calcNumSteps(void) const {
   // Dyac_host_printf("xxx calcNumSteps: sp_mod_numsteps=%p\n", sp_mod_numsteps);
   sF32 modLen = (mod_numsteps + *sp_mod_numsteps + mmdst_numsteps);
   // Dyac_host_printf("xxx calcNumSteps: modLen=%f\n", modLen);
   modLen = sRANGE(modLen, 0.0f, 1.0f);
   sUI numSteps = sUI(modseq->num_steps * modLen + 0.5f);
   if(numSteps < 1u)
      numSteps = 1u;
   else if(numSteps > modseq->num_steps)
      numSteps = modseq->num_steps;
   return numSteps;
}

sUI StModSeqPlayer::calcStepIdx(sUI _idx, sUI _numSteps) const {
   // (note) numSteps is in 1..n range
   sUI r;
   sUI t;
   switch(modseq->play_mode)
   {
      default:
      case STMODSEQ_PLAYMODE_FWD:
      case STMODSEQ_PLAYMODE_RANDOM:
      case STMODSEQ_PLAYMODE_RANDOM_ND:
      case STMODSEQ_PLAYMODE_RANDOM_NR:
         r = _idx % _numSteps;
         break;

      case STMODSEQ_PLAYMODE_BWD:
         r = (_numSteps - 1u) - (_idx % _numSteps);
         break;

      case STMODSEQ_PLAYMODE_PINGPONG:
         t = (_idx / _numSteps);
         if(t & 1u)
         {
            // bwd
            r = (_numSteps - 1u) - (_idx % _numSteps);
         }
         else
         {
            // fwd
            r = _idx % _numSteps;
         }
         break;

      case STMODSEQ_PLAYMODE_PINGPONG_NR:
         // 0 => 0  (for len=3)
         // 1 => 1
         // 2 => 2
         // 3 => 1
         // 4 => 0
         // 5 => 1
         // 6 => 2
         // ..
         if(_numSteps > 1u)
         {
            t = (_idx % (2u * _numSteps - 2u));
            if(t >= _numSteps)
            {
               // bwd
               r = (_numSteps - (t - _numSteps) - 2u);
               // Dyac_host_printf("xxx pingpong_nr _idx=%u t=%u numSteps=%u idx=%u  (BWD)\n", _idx, t, _numSteps, r);
            }
            else
            {
               // fwd
               r = t;
               // Dyac_host_printf("xxx pingpong_nr numSteps=%u idx=%u  (FWD)\n", _numSteps, r);
            }
         }
         else
         {
            r = 0u;
         }
         break;
   }

   return r;
}

void StModSeqPlayer::nextStepIdx(sUI _numSteps) {
   b_running = YAC_TRUE;

   switch(modseq->play_mode)
   {
      default:
      case STMODSEQ_PLAYMODE_FWD:
      case STMODSEQ_PLAYMODE_BWD:
         if(modseq->b_oneshot)
         {
            if(num_steps_played < _numSteps)
            {
               num_steps_played++;
               step_idx++;
               b_pulse = YAC_TRUE;
            }
         }
         else
         {
            num_steps_played++;
            step_idx++;
            if(step_idx >= modseq->num_steps)
               step_idx = modseq->repeat_offset;
            b_pulse = YAC_TRUE;
         }
         break;

      case STMODSEQ_PLAYMODE_PINGPONG:
         if(modseq->b_oneshot)
         {
            if(num_steps_played < (_numSteps*2u))
            {
               num_steps_played++;
               step_idx++;
               b_pulse = YAC_TRUE;
            }
         }
         else
         {
            num_steps_played++;
            step_idx++;
            b_pulse = YAC_TRUE;
         }
         break;

      case STMODSEQ_PLAYMODE_PINGPONG_NR:
         if(modseq->b_oneshot)
         {
            if(num_steps_played < (2u * _numSteps -1u))
            {
               num_steps_played++;
               step_idx++;
               b_pulse = YAC_TRUE;
            }
         }
         else
         {
            num_steps_played++;
            step_idx++;
            b_pulse = YAC_TRUE;
         }
         break;

      case STMODSEQ_PLAYMODE_RANDOM:
         if(modseq->b_oneshot)
         {
            if(num_steps_played < _numSteps)
            {
               num_steps_played++;
               step_idx = lfsr.rand();
               b_pulse = YAC_TRUE;
            }
         }
         else
         {
            num_steps_played++;
            step_idx = lfsr.rand();
            b_pulse = YAC_TRUE;
         }
         break;

      case STMODSEQ_PLAYMODE_RANDOM_ND:
         if(modseq->b_oneshot)
         {
            if(num_steps_played < _numSteps)
            {
               num_steps_played++;
               step_idx = lfsr.randModNR(_numSteps);
               b_pulse = YAC_TRUE;
            }
         }
         else
         {
            num_steps_played++;
            step_idx = lfsr.randModNR(_numSteps);
            b_pulse = YAC_TRUE;
         }
         break;

      case STMODSEQ_PLAYMODE_RANDOM_NR:
         if(modseq->b_oneshot)
         {
            if(num_steps_played < _numSteps)
            {
               num_steps_played++;
               step_idx = rand_nr.next();
               b_pulse = YAC_TRUE;
            }
         }
         else
         {
            num_steps_played++;
            step_idx = rand_nr.next();
            b_pulse = YAC_TRUE;
         }
         break;
   }
}

void StModSeqPlayer::handleModAdvance(sUI _numSteps) {

   sF32 modAdv = (mod_advance + *sp_mod_advance + mmdst_advance);
   sBool bAdvance = YAC_FALSE;

   switch(modseq->step_mode)
   {
      default:
         break;

      case STMODSEQ_STEPMODE_MOD_RISE:
         bAdvance = (modAdv > last_advance);
         break;

      case STMODSEQ_STEPMODE_MOD_FALL:
         bAdvance = (modAdv < last_advance);
         break;

      case STMODSEQ_STEPMODE_MOD_EDGE:
         bAdvance = (modAdv != last_advance);
         break;

      case STMODSEQ_STEPMODE_MOD_XCROSS:
         bAdvance = (modAdv <= 0) ^ (last_advance <= 0);
         break;

      case STMODSEQ_STEPMODE_START_MOD_NZ:
         // allow single start (e.g. vibrato)
         if(modAdv > 0.0f)
         {
            if(!b_running)
            {
               handleRestart();
               b_running = YAC_TRUE;
            }
         }
         return;

      case STMODSEQ_STEPMODE_RESTART_MOD_NZ:
         // allow multiple restarts (e.g. volume env)
         if(modAdv > 0.0f)
         {
            handleRestart();
            b_running = YAC_TRUE;
         }
         return;
   }

   if(bAdvance)
   {
      nextStepIdx(_numSteps);
   }

   last_advance = modAdv;
}

sF32 StModSeqPlayer::tick(sBool _bPulseGate) {

   if(!b_running)
   {
      if((NULL != modseq) && ((STMODSEQ_STEPMODE_START_MOD_NZ == modseq->step_mode) || (STMODSEQ_STEPMODE_RESTART_MOD_NZ == modseq->step_mode)))
         handleModAdvance(calcNumSteps());

      if(!b_running)
         return 0.0f;
   }

   // Dyac_host_printf("xxx mseq::tick: ENTER\n");
   sF32 r = 0.0f;

   if(NULL != modseq)
   {
      r = _bPulseGate ? last_level : (YAC_MAYBE == modseq->b_pulse) ? -1.0f : 0.0f;

      sUI numSteps = calcNumSteps();
      // Dyac_host_printf("xxx mseq: numSteps=%u num_steps_played=%u\n", numSteps, num_steps_played);

      if(0u == num_steps_played)
      {
         if((STMODSEQ_PLAYMODE_RANDOM    == modseq->play_mode) ||
            (STMODSEQ_PLAYMODE_RANDOM_ND == modseq->play_mode) ||
            (STMODSEQ_PLAYMODE_RANDOM_NR == modseq->play_mode)
            )
         {
            // Start with random step
            if(STMODSEQ_PLAYMODE_RANDOM_NR == modseq->play_mode)
            {
               if(rand_nr.len != numSteps)
                  rand_nr.init(numSteps);
            }
            nextStepIdx(numSteps);
         }
         else
         {
            num_steps_played = 1u;
         }
      }
      else
      {
         if(modseq->step_mode >= STMODSEQ_STEPMODE_MOD_RISE)
            handleModAdvance(numSteps);
      }

      sUI stepIdx = calcStepIdx(step_idx, numSteps);
      // Dyac_host_printf("xxx mseq: stepIdx=%u\n", stepIdx);

      if((STMODSEQ_STEPMODE_TIME == modseq->step_mode) ||
         (STMODSEQ_STEPMODE_START_MOD_NZ == modseq->step_mode) ||
         (STMODSEQ_STEPMODE_RESTART_MOD_NZ == modseq->step_mode)
         )
      {
         sF32 modSpd = mod_speed * *sp_mod_speed * mmdst_speed;
         if(modSpd > 0.0f)
         {
            step_ticks += modSpd;
            sF32 stepDur = (modseq->step_durations[stepIdx] + 1u) * modseq->speed * (modseq->b_tempo_sync ? tempo_speed_scl : 1.0f);
            if(step_ticks >= stepDur)
            {
               step_ticks -= stepDur;
               nextStepIdx(numSteps);
               stepIdx = calcStepIdx(step_idx, numSteps);
            }
         }
      }

      if(b_pulse || (YAC_FALSE == modseq->b_pulse))
      {
         b_pulse = YAC_FALSE;

         r = modseq->step_values[stepIdx];
         // Dyac_host_printf("xxx stepIdx=%u r=%f\n", stepIdx, r);

         r = (r * modseq->value_scl) + modseq->value_off;

         if(modseq->ui_last_started_voice == ui_sample_voice)
            modseq->ui_last_played_idx = sSI(stepIdx);

         sF32 modLvl = (mod_level + *sp_mod_level);
         r *= modLvl;
      }

      if(num_steps_played > 1u)
      {
         sF64 slewAmt = 1.0 - modseq->slew_amt;
         slewAmt *= slewAmt;
         slewAmt *= slewAmt;
         r = sF32(r + (last_level - r) * (1.0 - slewAmt));
      }

      if(b_mmdst_step)
      {
         b_mmdst_step = YAC_FALSE;
         sF32 stepT = mmdst_step;
         if(stepT < 0.0f)
            stepT = 0.0f;
         else if(stepT > 1.0f)
            stepT = 1.0f;
         stepT *= float(numSteps + 1u);
         sUI stepC = (sUI)stepT;
         sUI stepN = stepC + 1u;
         if(modseq->b_oneshot)
         {
            // no loop (hold last step value)
            if(stepC >= numSteps)
               stepC = numSteps-1;
            if(stepN >= numSteps)
               stepN = numSteps-1;
         }
         else
         {
            // loop
            if(stepC >= numSteps)
               stepC = 0u;
            if(stepN >= numSteps)
               stepN = 0u;
         }
         // (note) slew 0%=s&h, slew 100%=smooth interpolation
         stepT = stepT - sF32(sSI(stepT));
         sF32 shAmt = powf(10.0f, (-1.0f + modseq->slew_amt) * 6.0f);
         if(stepT >= 0.5f)
         {
            stepT = 1.0f - (0.5f-(stepT-0.5f))*shAmt;
         }
         else
         {
            stepT *= shAmt;
         }
         sF32 valC = modseq->step_values[stepC];
         sF32 valN = modseq->step_values[stepN];
         r = valC + (valN - valC) * stepT;

         r = (r * modseq->value_scl) + modseq->value_off;

         if(modseq->ui_last_started_voice == ui_sample_voice)
            modseq->ui_last_played_idx = sSI(stepC);

         sF32 modLvl = (mod_level + *sp_mod_level);
         r *= modLvl;
      }
   }

   last_level = r;

   // Dyac_host_printf("xxx mseq::tick: LEAVE r=%f\n", r);

   return r;
}

void StModSeqPlayer::noteOff(void) {
   if(NULL != modseq)
   {
      if(STMODSEQ_STEPMODE_NOTEOFF == modseq->step_mode)
         nextStepIdx(calcNumSteps());
   }
}
