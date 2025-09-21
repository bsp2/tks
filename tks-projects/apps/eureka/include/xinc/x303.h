/* based on:
 *
 * DavBassFilter - DAV Bass Filter
 * Copyright (c) 1999, 2000 David A. Bartold, 2003 Tim Janik
 *
 * This is a TB-303 filter clone based on the VCF303 portions of
 * gsyn v0.2.  Code in update_locals() is copyright (c) 1998 Andy Sloane.
 *
 * Filter algorithm in function recalc_a_b() is based on the one
 * described in Musical Applications of Microprocessors by Hal Chamberlin.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/* adapted from SyModuleFlt303 (2010) */

/*
 * @cycle_mod    x303
 * @cycle_voice  SyModuleFlt303 %cyid%;
 * @cycle_noteon voice->%cyid%.init();
 * @cycle_noteon voice->%cyid%.reset(voice->sample_rate, voice->sr_factor);
 * @cycle_input  freq
 * @cycle_input  q
 * @cycle_input  trig
 * @cycle_input  envmod
 * @cycle_input  envdcy
 * @cycle_emit   %cyio% = voice->%cyid%.calc(%cyio%, %cyin_freq%, %cyin_q%, %cyin_trig%, %cyin_envmod%, %cyin_envdcy%);
 *
 */

/* #include <stdio.h> */

class SyModuleFlt303State {
   // per-channel state
  public:
   double d1, d2;
   double v;

   inline void reset(void) {
      v = d1 = d2 = 0;
   }

   inline void shuffle(void) {
      d2 = d1;
      d1 = v;
   }
};

class SyModuleFlt303 {
   double filt_cutoff, filt_reso, env_mod, env_decay;
   double decay, resonance;
   double a, b, c0;
   double e0, e1;
   int envbound; /* 64 at 44100 */
   int envpos;
   float last_trigger;
   SyModuleFlt303State state;
   float sample_rate;
   float sr_factor;

   inline void recalc_resonance ()
      {
         /* Update resonance. */
         resonance = exp (-1.20 + 3.455 * filt_reso);
      }

   inline void recalc_filter ()
      {
         /* Update vars given envmod, cutoff, and reso. */
         e0 = exp (5.613 - 0.8 * env_mod + 2.1553 * filt_cutoff - 0.7696 * (1.0 - filt_reso));
         e1 = exp (6.109 + 1.5876 * env_mod + 2.1553 * filt_cutoff - 1.2 * (1.0 - filt_reso));
         double e0f = (ST_PLUGIN_PI * sr_factor) / sample_rate;
         e0 *= e0f;
         e1 *= e0f;
         e1 -= e0;
      }

   inline void recalc_decay ()
      {
         /* Update decay given envdecay. */
         envbound = (int) (0.001452 * sample_rate * (1.0f / sr_factor)); // 64 at 44100;
         envbound = Dstplugin_max (envbound, 1);
         double d = env_decay;
         d = 0.2 + (2.3 * d);
         d *= sample_rate / sr_factor;
         decay = pow (0.1, envbound / d);
      }

   inline void recalc_a_b ()
    {
      double whopping = e0 + c0;
      double k = exp (-whopping / resonance);
      a = 2.0 * cos (2.0 * whopping) * k;
      b = -k * k;
    }


  public:
   SyModuleFlt303(void) {
   }

   ~SyModuleFlt303() {
   }

   void init (void) {
   }

   void reset (float _sampleRate, float _srFactor) {
      sample_rate = _sampleRate;
      sr_factor   = _srFactor;
      state.reset();
      c0           = 0;
      envpos       = 0;
      last_trigger = 0.0f;
   }

   float calc (float _inSmp, float _inFreq, float _inQ, float _inTrigger, float _inEnvMod, float _inEnvDecay) {

      // Cutoff/center frequency
      float freq = _inFreq;
      if(freq < 0.0005f)
         freq = 0.0005f;
      else if(freq > 1.0f)
         freq = 1.0f;

      // Resonance
      float q = _inQ;
      if(q < 0.0f)
         q = 0.0f;
      else if(q > 1.0f)
         q = 1.0f;

      // Calc filter
      filt_cutoff = 1.6 * freq - 0.2;
      filt_reso   = q * 1.1f;
      env_mod     = _inEnvMod;
      env_decay   = _inEnvDecay;

      recalc_resonance();
      recalc_filter();
      recalc_decay();
      recalc_a_b();

      // check trigger
      if(last_trigger <= 0.0f && _inTrigger > 0.0f)
      {
         /* Reset filter delta freq. */
         c0 = e1;
         envpos = 0;
      }
      last_trigger = _inTrigger;

      double c = (1.0 - a - b) * 0.2;

      double ab = a * state.d1 + b * state.d2;
      state.v = ab + c * _inSmp;
      double out = 3.0 * state.v;
      state.shuffle();

      if(++envpos >= envbound)
      {
         envpos = 0;
         c0 *= decay;
         recalc_a_b ();
      }

      return out;
   }

};
