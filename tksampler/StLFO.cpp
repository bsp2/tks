// ----
// ---- file   : StLFO.cpp
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2009-2023 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : This is part of the "syntracker" midi sequencer.
// ----
// ---- created: 23Sep2009
// ----
// ---- changed: 21Jan2010, 20Feb2010, 21Feb2010, 04Apr2011, 22Apr2011, 23Apr2011, 07Jan2012
// ----          30Mar2019, 12May2019, 12Apr2023
// ----
// ----
// ----

#include "tksampler.h"

#include "StLFO.h"


StLFO::StLFO(void) {
   type  = STLFO_TYPE_SIN;
   phase = 0.0f;
   speed = 0.01f;
   pulse_width = 0.5f;

   delay = 0;
   fadein_time = 20.0f; // 20 ms

   start_level = 0.0f;
   faded_level = 1.0f;

   faded_speed_factor = 1.0f;

   dc_offset = 0.0f;

   // // waveform = NULL;

   noise_minamp = 0.0f;
   noise_maxamp = 1.0f;
   noise_minspd = 0.1f;
   noise_maxspd = 250.0f;
   b_noise_interpol = 1;

   timebase = STSAMPLEVOICE_TICKSPERSEC;
   b_mod_delay = 0;

   b_oneshot = YAC_FALSE;
   b_global = YAC_FALSE;
   b_reset = YAC_TRUE;
}

StLFO::~StLFO() {
}

void StLFO::_setTimebase(sF32 _tb) {
   if(_tb < 1.0f)
      _tb = 1.0f;
   timebase = _tb;
}

sF32 StLFO::_getTimebase(void) {
   return timebase;
}

void StLFO::_setType(sUI _type) {
   if(_type >= STLFO_NUM_TYPES)
   {
      type = STLFO_TYPE_SIN;
   }
   else
   {
      type = _type;
   }
}

sUI  StLFO::_getType(void) {
   return type;
}

void StLFO::_setPhase(sF32 _phase) {
   phase = _phase;
}

sF32 StLFO::_getPhase(void) {
   return phase;
}

void StLFO::_setFreq(sF32 _hz) {
   /*
     blocklen = 0.0005 sec
     tickspersec = (1/blocklen) = 2000

     hz = speed * tickspersec
     speed = hz / tickspersec
   */

   if(_hz > 0)
   {
      ////speed = _hz * STSAMPLEVOICE_BLOCK_LEN;
      speed = _hz * (1.0f / timebase);
   }
   else
   {
      speed = 0;
   }

   // Dyac_host_printf("xxx StLFO::setFreq: Hz=%f timebase=%f => speed=%f\n", _hz, timebase, speed);
}

sF32 StLFO::_getFreq(void) {
   return speed * timebase;
}

void StLFO::_setPulseWidth(sF32 _pw) {
   pulse_width = _pw;
}

sF32 StLFO::_getPulseWidth(void) {
   return pulse_width;
}

void StLFO::_setDelay(sF32 _millisec) {
   /*
     millisec = (1000 * delay) / tickspersec
     delay = (millisec * tickspersec) / 1000
    */
   if(_millisec > 0.0f)
   {
      delay = (sUI) ((_millisec * timebase) / 1000.0f);
   }
   else
   {
      delay = 0;
   }
}

sF32 StLFO::_getDelay(void) {
   // Return delay length in milliseconds
   return (1000.0f * delay) / timebase;
}

sF32 StLFO::calcFadeInSpeed(void) {
   if(fadein_time > 0.0f)
   {
      sF32 ticks = (fadein_time / 1000.0f) * timebase;
      ////printf("xxx StLFO::calcFadeInSpeed: fadein_time=%f ticks=%f\n", fadein_time, ticks);
      if(ticks > 0.0f)
      {
         // // sF32 add = (faded_level - start_level) / ticks;
         sF32 add = 1.0f / ticks;
         if(!Dfltzero(add))
         {
            return add;
         }
         return YAC_FLT_EPSILON;
      }
   }
   return 0.0f;
}

void StLFO::_setFadeInTime(sF32 _millisec) {
   fadein_time = _millisec;
}

sF32 StLFO::_getFadeInTime(void) {
   return fadein_time;
}

void StLFO::_setStartLevel(sF32 _start) {
   start_level = _start;
}

sF32 StLFO::_getStartLevel(void) {
   return start_level;
}

void StLFO::_setFadedLevel(sF32 _end) {
   faded_level = _end;
}

sF32 StLFO::_getFadedLevel(void) {
   return faded_level;
}

void StLFO::_setFadedSpeedFactor(sF32 _factor) {
   faded_speed_factor = _factor;
}

sF32 StLFO::_getFadedSpeedFactor(void) {
   return faded_speed_factor;
}

void StLFO::_setDcOffset(sF32 _off) {
   dc_offset = _off;
}

sF32 StLFO::_getDcOffset(void) {
   return dc_offset;
}

void StLFO::_setNoiseMinAmp(sF32 _minAmp) {
   noise_minamp = _minAmp;
}

sF32 StLFO::_getNoiseMinAmp(void) {
   return noise_minamp;
}

void StLFO::_setNoiseMaxAmp(sF32 _maxAmp) {
   noise_maxamp = _maxAmp;
}

sF32 StLFO::_getNoiseMaxAmp(void) {
   return noise_maxamp;
}

void StLFO::_setNoiseMinSpeed(sF32 _minSpd) {
   noise_minspd = _minSpd;
}

sF32 StLFO::_getNoiseMinSpeed(void) {
   return noise_minspd;
}

void StLFO::_setNoiseMaxSpeed(sF32 _maxSpd) {
   noise_maxspd = _maxSpd;
}

sF32 StLFO::_getNoiseMaxSpeed(void) {
   return noise_maxspd;
}

void  StLFO::_setEnableNoiseInterpol(sBool _bEnabled) {
   b_noise_interpol = _bEnabled;
}

sBool StLFO::_getEnableNoiseInterpol(void) {
   return b_noise_interpol;
}

void StLFO::_setEnableModDelay(sBool _bEnabled) {
   b_mod_delay = _bEnabled;
}

sBool StLFO::_getEnableModDelay(void) {
   return b_mod_delay;
}

void StLFO::_setEnableOneShot(sBool _bEnabled) {
   b_oneshot = _bEnabled;
}

sBool StLFO::_getEnableOneShot(void) {
   return b_oneshot;
}

void StLFO::_setEnableGlobal(sBool _bEnabled) {
   b_global = _bEnabled;
}

sBool StLFO::_getEnableGlobal(void) {
   return b_global;
}

void StLFO::_setEnableReset(sBool _bEnabled) {
   b_reset = _bEnabled;
}

sBool StLFO::_getEnableReset(void) {
   return b_reset;
}
