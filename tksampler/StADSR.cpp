// ----
// ---- file   : StADSR.cpp
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2009-2024 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : This is part of the "syntracker" midi sequencer.
// ----
// ---- created: 23Sep2009
// ---- changed: 21Jan2010, 08Feb2010, 09Feb2010, 14Feb2010, 15Feb2010, 28Dec2018, 29Dec2018
// ----          12Apr2023, 03Oct2024
// ----
// ----
// ----

#include "tksampler.h"


StADSR::StADSR(void) {
   env_attack  = NULL;
   env_sustain = NULL;
   env_release = NULL;

   intensity = 1.0f;
   level     = 1.0f;
   _setTimescaleMillisec(250.0f);

   stage_op = STADSR_STAGEOP_MUL;
   b_sustain_loop = YAC_TRUE;
}

StADSR::~StADSR() {
   YAC_DELETE_SAFE(env_attack);
   YAC_DELETE_SAFE(env_sustain);
   YAC_DELETE_SAFE(env_release);
}

YAC_Object *StADSR::_getOrCreateEnvAttack(void) {
   if(NULL == env_attack)
   {
      // env_attack = (StEnvelope*) yac_host->yacNew(NULL, "StEnvelope");
      env_attack = YAC_NEW(StEnvelope);
   }
   return env_attack;
}

YAC_Object *StADSR::_getOrCreateEnvSustain(void) {
   if(NULL == env_sustain)
   {
      // env_sustain = (StEnvelope*) yac_host->yacNew(NULL, "StEnvelope");
      env_sustain = YAC_NEW(StEnvelope);
   }
   return env_sustain;
}

YAC_Object *StADSR::_getOrCreateEnvRelease(void) {
   if(NULL == env_release)
   {
      // env_release = (StEnvelope*) yac_host->yacNew(NULL, "StEnvelope");
      env_release = YAC_NEW(StEnvelope);
   }
   return env_release;
}

YAC_Object *StADSR::_getOrCreateEnvAttackData(void) {
   StEnvelope *env = (StEnvelope*) _getOrCreateEnvAttack();
   return env->_getOrCreateData();
}

YAC_Object *StADSR::_getOrCreateEnvSustainData(void) {
   StEnvelope *env = (StEnvelope*) _getOrCreateEnvSustain();
   return env->_getOrCreateData();
}

YAC_Object *StADSR::_getOrCreateEnvReleaseData(void) {
   StEnvelope *env = (StEnvelope*) _getOrCreateEnvRelease();
   return env->_getOrCreateData();
}

YAC_Object *StADSR::_getEnvAttack(void) {
   return env_attack;
}

YAC_Object *StADSR::_getEnvSustain(void) {
   return env_sustain;
}

YAC_Object *StADSR::_getEnvRelease(void) {
   return env_release;
}

void StADSR::_deleteEnvAttack(void) {
   YAC_DELETE_SAFE(env_attack);
}

void StADSR::_deleteEnvSustain(void) {
   YAC_DELETE_SAFE(env_sustain);
}

void StADSR::_deleteEnvRelease(void) {
   YAC_DELETE_SAFE(env_release);
}

YAC_Object *StADSR::_getEnvAttackMetaData(void) {
   if(NULL != env_attack)
   {
      return env_attack->metadata_object;
   }
   return NULL;
}

YAC_Object *StADSR::_getEnvSustainMetaData(void) {
   if(NULL != env_sustain)
   {
      return env_sustain->metadata_object;
   }
   return NULL;
}

YAC_Object *StADSR::_getEnvReleaseMetaData(void) {
   if(NULL != env_release)
   {
      return env_release->metadata_object;
   }
   return NULL;
}

sF32 StADSR::_getIntensity(void) {
   return intensity;
}

void StADSR::_setIntensity(sF32 _v) {
   intensity = _v;
}

sF32 StADSR::_getLevel(void) {
   return level;
}

void StADSR::_setLevel(sF32 _v) {
   level = _v;
}

sF32 StADSR::_getTimescale(void) {
   return (sF32) timescale;
}

void StADSR::_setTimescale(sF32 _v) {
   if(_v < STENVELOPE_MIN_TIMESCALE)
   {
      _v = STENVELOPE_MIN_TIMESCALE;
   }
   timescale = _v;
}

sF32 StADSR::_getTimescaleMillisec(void) {
   // Note: - Unit envelope duration is 1.0
   //       - Time is increased by 1.0 each tick()
   if(timescale > 0.0)
   {
      return (sF32) ( (1000.0 / timescale) * STSAMPLEVOICE_BLOCK_LEN );
   }
   else
   {
      return 0.0f;
   }
}

void StADSR::_setTimescaleMillisec(sF32 _millisec) {
   // Note: it's actually only the duration of the attack envelope. plus the envelopes have relative timescaling factors
   //       (default=1)
   /*
     ta_sec = 1 / tickspersec = blocklen
     ta = blocklen / (_millisec/1000)
     ta = (1000 * blocklen) / _millisec
    */

   if(_millisec > 0.0f)
   {
      if(_millisec > STENVELOPE_MAX_TIMESCALE_MS)
      {
         timescale = STENVELOPE_MIN_TIMESCALE;
      }
      else
      {
         timescale = (1000.0 * STSAMPLEVOICE_BLOCK_LEN) / _millisec;
         ////printf("xxx StADSR: millisec=%f timescale=%f\n", _millisec, timescale);
      }
   }
   else
   {
      timescale = STENVELOPE_MIN_TIMESCALE;
   }
}

sSI StADSR::_getStageOp(void) {
   return stage_op;
}

void StADSR::_setStageOp(sSI _op) {
   stage_op = _op;
}

sSI StADSR::_getEnableSustainLoop(void) {
   return b_sustain_loop;
}

void StADSR::_setEnableSustainLoop(sSI _bEnabled) {
   b_sustain_loop = _bEnabled;
}
