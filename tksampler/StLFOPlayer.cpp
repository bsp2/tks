// ----
// ---- file   : StLFOPlayer.cpp
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2009-2024 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : This is part of the "syntracker" midi sequencer.
// ----
// ---- created: 23Sep2009
// ---- changed: 21Jan2010, 20Feb2010, 01Aug2010, 04Apr2011, 22Apr2011, 23Apr2011, 07Jan2012
// ----          29Dec2018, 17Jan2019, 30Mar2019, 12May2019, 17May2019, 13Jul2019, 25Jul2019
// ----          15Feb2020, 18Feb2020, 19Feb2020, 12Apr2023, 03Oct2024
// ----
// ----
// ----

#include "tksampler.h"

#include "StLFO.h"
#include "StLFOPlayer.h"


StLFOPlayer::StLFOPlayer(void) {
   lfsr.init((void*)this);

   lfo             = NULL;
   phase           = 0.0f;
   fade_time       = 0.0f;
   delay_countdown = 0.0f;
   fadein_speed    = 0.0f;
   faded_speed_amt = 0.0f;
   mod_speed       = 1.0f;
   mod_level       = 1.0f;
   mmdst_speed     = 1.0f;
   noise_time      = 0.0f;
   noise_spd       = 0.0f;
   noise_cval      = 0.0f;
   noise_nval      = 0.0f;
   last_level      = 0.0f;

   sp_mod_speed = NULL;
   sp_mod_level = NULL;
}

StLFOPlayer::~StLFOPlayer() {
}

void StLFOPlayer::resetPhase(void) {
   if(NULL != lfo)
   {
      phase = lfo->phase;

      if(phase >= 1.0f)
      {
         phase = lfsr.randf();
      }
   }
}

void StLFOPlayer::startLFO(StLFO *_lfo, sBool _bResetMod) {
   lfo = _lfo;

   if(NULL != lfo)
   {
      if(_lfo->b_reset)
      {
         resetPhase();
      }

      level           = lfo->start_level;
      delay_countdown = (sF32) lfo->delay;
      fade_time       = 0.0f;
      fadein_speed    = lfo->calcFadeInSpeed();
      faded_speed_amt = 0.0f;

      if(_bResetMod)
      {
         mod_speed = 1.0f;
         mod_level = 1.0f;

         mmdst_speed = 1.0f;
      }

      if(STLFO_TYPE_NOISE == lfo->type)
      {
         // Fill both cval and nval
         calcNextNoise();
         calcNextNoise();
      }
   }
}

void StLFOPlayer::stopLFO(void) {
   lfo = NULL;
}

void StLFOPlayer::calcNextNoise() {
   noise_time = 0.0f;

   sF32 min = lfo->noise_minamp;
   sF32 max = lfo->noise_maxamp;

   if(min > max)
   {
      sF32 t = min;
      min = max;
      max = t;
   }

   noise_cval = noise_nval;
   if(noise_cval < min) // clipping needed for first sample
   {
      noise_cval = min;
   }
   else if(noise_cval > max)
   {
      noise_cval = max;
   }

   noise_nval = (sF32) (lfsr.randf() * (max - min) ) + min;

   sF32 minSpd = lfo->noise_minspd;
   sF32 maxSpd = lfo->noise_maxspd;

   if(minSpd > maxSpd)
   {
      sF32 t = minSpd;
      minSpd = maxSpd;
      maxSpd = t;
   }

   // Calc new noise speed (milliseconds)
   noise_spd = (sF32) (lfsr.randf() * (maxSpd - minSpd) ) + minSpd;

   // Convert noise speed in milliseconds to 'add per tick', normalized to 0..1 range
   if(noise_spd > 0.0f)
   {
      ////noise_spd = 1.0f / (lfo->timebase * (noise_spd/1000.0f));
      noise_spd = 1000.0f / (lfo->timebase * noise_spd);

      // printf("xxx calcNextNoise: noise_spd=%f\n", noise_spd);
   }
   else
   {
      noise_spd = 0.0f;
   }
}

sF32 StLFOPlayer::tick(void) {

   sF32 r = 0.0f;

   if(NULL != lfo)
   {
      if(delay_countdown <= 0.0f)
      {
         if(fadein_speed != 0.0f)
         {
            // Do fade
            if(fade_time < 1.0f)
            {
               fade_time += fadein_speed * (lfo->b_mod_delay ? (mod_speed * *sp_mod_speed * mmdst_speed) : 1.0f);
               if(fade_time > 1.0f)
               {
                  fade_time = 1.0f;
               }
            }

            level = lfo->start_level + (lfo->faded_level - lfo->start_level) * fade_time;

            if(faded_speed_amt < 1.0f)
            {
               faded_speed_amt += fadein_speed * (lfo->b_mod_delay ? (mod_speed * *sp_mod_speed * mmdst_speed) : 1.0f);
               if(faded_speed_amt > 1.0f)
                  faded_speed_amt = 1.0f;
            }
         }
         else
         {
            level = lfo->faded_level;
         }

         switch(lfo->type)
         {
            default:
            case STLFO_TYPE_NONE:
            {
               r = 0.0f;
            }
            break;

            case STLFO_TYPE_SIN:
            {
               r = (sF32) sin(phase * sM_PI * 2.0f) * level + lfo->dc_offset;
            }
            break;

            case STLFO_TYPE_SAWUP:
            {
               r = (-1.0f + 2.0f * phase) * level + lfo->dc_offset;
            }
            break;

            case STLFO_TYPE_SAWDN:
            {
               r = (1.0f - 2.0f * phase) * level + lfo->dc_offset;
            }
            break;

            case STLFO_TYPE_TRI:
            {
               r = phase + 0.25f;
               if(r > 1.0f)
               {
                  r -= 1.0f;
               }
               r = -1.0f + phase * 2.0f;
               if(r > 0.0f)
               {
                  r = 1.0f - (2.0f * r);
               }
               else
               {
                  r = 1.0f + (2.0f * r);
               }
               r = r * level + lfo->dc_offset;
            }
            break;

            case STLFO_TYPE_PULSE:
            {
               if(phase < lfo->pulse_width)
               {
                  r = 1.0f;
               }
               else
               {
                  r = -1.0f;
               }
               r = r * level + lfo->dc_offset;
            }
            break;

            case STLFO_TYPE_NOISE:
            {
               // xxx TODO: implement noise LFO

               if(lfo->b_noise_interpol)
               {
                  // Smoothstep:
                  sF32 t = noise_time;
                  t = t * t * (3.0f - 2.0f * t);
                  r = (sF32) (noise_cval + (noise_nval - noise_cval) * t) * level + lfo->dc_offset;
               }
               else
               {
                  r = (sF32) noise_cval * level + lfo->dc_offset;
               }

               // printf("xxx noise r=%f\n", r);

               //printf("xxx noise_time=%f noise_spd=%f\n", noise_time, noise_spd);

               // (note) noise_spd is typically ~0.001..~0.1

               noise_time += noise_spd * (mod_speed * *sp_mod_speed * mmdst_speed);
               if(noise_time >= 1.0f)
               {
                  calcNextNoise();
               }
            }
            break;

         } // switch type

         if(STLFO_TYPE_NOISE != lfo->type)
         {
            sF32 spd = lfo->speed;
            sF32 spdFaded = lfo->speed * lfo->faded_speed_factor;
            // Dyac_host_printf("xxx lfo spd=%f spdFaded=%f mod_speed=%f\n", spd, spdFaded, mod_speed);
            spd = spd + (spdFaded - spd) * faded_speed_amt;  // lerp
            spd *= (mod_speed * *sp_mod_speed * mmdst_speed);

            phase += spd;

            while(phase >= 1.0f)
            {
               if(lfo->b_oneshot)
               {
                  phase = 0.99999f;
               }
               else
               {
                  phase -= 1.0f;
               }
            }
         }
      }
      else
      {
         delay_countdown -= (lfo->b_mod_delay ? (mod_speed * *sp_mod_speed * mmdst_speed) : 1.0f);
      }
   }

   sF32 modLvl = (mod_level + *sp_mod_level);
   modLvl = sRANGE(modLvl, 0.0f, 16.0f);
   last_level = r * modLvl;
   return last_level;
}


//
// script-wrapper:
//

_StLFOPlayer::_StLFOPlayer(void) {
}

_StLFOPlayer::~_StLFOPlayer() {
}

void _StLFOPlayer::_startLFO(YAC_Object *_lfo, sBool _bNoteOn) {
   if(YAC_BCHK(_lfo, clid_StLFO))
   {
      YAC_CAST_ARG(StLFO, lfo, _lfo);

      lfo_player.startLFO(lfo, _bNoteOn);
   }
   else
   {
      Dyac_host_printf("[---] StLFOPlayer::startLFO: _lfo is not a valid StLFO object\n");
      lfo_player.stopLFO();
   }
}

void _StLFOPlayer::_stopLFO(void) {
   lfo_player.stopLFO();
}

sF32 _StLFOPlayer::_calc(void) {
   // (todo) update StLFOPlayer so there actually is a clean separation between tick/calc

   // hack: zero mod_speed so that the following tick() call does not advance
   sF32 origModSpeed = lfo_player.mod_speed;
   lfo_player.mod_speed = 0.0f;

   // calculate current LFO level
   sF32 r = lfo_player.tick();

   // hack: restore actual speed
   lfo_player.mod_speed = origModSpeed;

   return r;
}

void _StLFOPlayer::_tick(void) {
   // (todo) update StLFOPlayer so there actually is a clean separation between tick/calc

   (void)lfo_player.tick(); // discard return value, simply advance in time
}

void _StLFOPlayer::_setModSpeed(sF32 _modSpeed) {
   lfo_player.mod_speed = _modSpeed;
}

void _StLFOPlayer::_setModLevel(sF32 _modLevel) {
   lfo_player.mod_level = _modLevel;
}

sF32 _StLFOPlayer::_getLastLevel(void) {
   return lfo_player.last_level;
}
