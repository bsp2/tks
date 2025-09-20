// ----
// ---- file   : StADSRPlayer.cpp
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2009-2024 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : This is part of the "syntracker" midi sequencer.
// ----
// ---- created: 23Sep2009
// ----
// ---- changed: 21Jan2010, 14Feb2010, 29Jun2010, 01Aug2010, 15Dec2018, 28Dec2018, 17Jan2019
// ----          11May2019, 12May2019, 13Jul2019, 25Jul2019, 23Aug2021, 12Apr2023, 29Sep2024
// ----          03Oct2024
// ----
// ----
// ----

#include "tksampler.h"


static sF32 loc_powxy(const sF32 _f, const sF32 _exp) {
   if(_exp >= 0.1f)
   {
      if(_f > 0.0f)
         return (sF32)( exp(_exp*log(_f)) );
      else if(_f < 0.0f)
         return (sF32)( -exp(_exp*log(-_f)) );
   }
   return _f;
}


StADSRPlayer::StADSRPlayer(void) {
   adsr        = NULL;
   current_env = NULL;
   orig_st_env = NULL;
   orig_env    = NULL;

   min_level = -2.0f;

   sp_mod_speed     = NULL;
   sp_mod_level     = NULL;
   sp_mod_intensity = NULL;
   sp_mod_aspeed    = NULL;
   sp_mod_sspeed    = NULL;
   sp_mod_rspeed    = NULL;
}

StADSRPlayer::~StADSRPlayer() {
   YAC_DELETE_SAFE(current_env);
}

void StADSRPlayer::visitEnv(StEnvelope *_stEnv) {
   // Dyac_host_printf("xxx StADSRPlayer::visitEnv: stEnv=%p\n", _stEnv);
   if(NULL != _stEnv)
   {
      TKS_Envelope *env = _stEnv->data;

      // Dyac_host_printf("xxx StADSRPlayer::visitEnv: env=%p\n", env);

      if(NULL != env)
      {
         orig_st_env = _stEnv;
         orig_env    = env;

         // (note) duplicate env (except for data) so each instance gets its own state
         current_env->own_data      = 0;
         current_env->elements      = env->elements;
         current_env->num_elements  = env->num_elements;
         current_env->max_elements  = env->max_elements;
         current_env->current_index = env->current_index;
         current_env->abs_time      = env->abs_time;
         current_env->delta_time    = env->delta_time;
         current_env->env_fun       = env->env_fun;
         current_env->b_shreset     = env->b_shreset;
         current_env_intensity      = _stEnv->intensity;

         // Dyac_host_printf("xxx StADSRPlayer::visitEnv: 1 num_elements=%u\n", current_env->num_elements);
         // current_env->yacArrayGetEnvValue(); // (note) do NOT call ->get(), it does not work in VS2017 and will set elements to NULL instead (!)
         // Dyac_host_printf("xxx StADSRPlayer::visitEnv: 2 num_elements=%u\n", current_env->num_elements);

         // Timescaling / env time advance (updated again in tick() so UI editing has an immediate effect)
         current_env_timescale      = adsr->timescale * _stEnv->timescale;
         current_env->time_advance  = env->time_advance * current_env_timescale;

         // Calc total env time
         //  (TODO) move this to TKS_Envelope class ?
         sUI i;
         current_env_totaltime = 0.0f;
         for(i=0; i<current_env->num_elements; i+=2)
         {
            sF32 dt = current_env->elements[i + 0];
            current_env_totaltime += dt;
         }
         // Dyac_host_printf("xxx StADSRPlayer: 1 this=%p current_env=%p current_env->num_elements=%u totaltime=%f\n", this, current_env, current_env->num_elements, current_env_totaltime);

         if(current_env_totaltime > 0)
         {
            // Ok, envelope started
            return;
         }
      }
   }

   // Cannot start envelope
   current_env->own_data     = 0;
   current_env->elements     = NULL;
   current_env->num_elements = 0;
   current_env->max_elements = 0;
   current_env_totaltime     = 0.0f;
   current_env_timescale     = 1.0;
   current_env_intensity     = 0.0f;

   // Dyac_host_printf("xxx StADSRPlayer: failed to start envelope current_env=%p\n", current_env);
}

void StADSRPlayer::startADSR(StADSR *_adsr, sF32 _defaultLevel, sBool _bResetMod, sBool _bRelease) {
   adsr = _adsr;
   if(NULL != adsr)
   {
      time          = 0.0f;
      env_index     = ENV_ATTACK;
      b_finished    = YAC_FALSE;
      b_sustain_finished = YAC_FALSE;

      b_skip_sustain = _bRelease;  // when restarting envelope after note off

      {
         default_level = _defaultLevel;
         attack_level  = _defaultLevel; // reset last seen levels
         sustain_level = _defaultLevel;
         final_level   = 0.0f;
      }

      if(_bResetMod)
      {
         mod_speed     = 1.0f;
         mod_level     = 1.0f;
         mod_intensity = 1.0f;
         mod_aspeed    = 1.0f;
         mod_sspeed    = 1.0f;
         mod_rspeed    = 1.0f;

         mmdst_speed     = 1.0f;
         // // mmdst_level     = 1.0f;
         mmdst_intensity = 0.0f;
         mmdst_aspeed    = 1.0f;
         mmdst_sspeed    = 1.0f;
         mmdst_rspeed    = 1.0f;
      }

      // Lazy create temporary envelope
      if(NULL == current_env)
      {
#ifndef LIBSYNERGY_BUILD
         current_env = (TKS_Envelope*) yac_host->yacNew(NULL, "Envelope");
#else
         current_env = YAC_NEW(TKS_Envelope);
#endif // LIBSYNERGY_BUILD
      }

      // Start attack envelope
      visitEnv(adsr->env_attack);

      // Dyac_host_printf("xxx startADSR: attack this=%p current_env=%p elements=%p num_elements=%u\n", this, current_env, current_env->elements, current_env->num_elements);

      // Skip attack envelope if it does not exist
      if(0 == current_env->num_elements)
      {
         if(b_skip_sustain)
         {
            // Start release envelope
            noteOff();
         }
         else
         {
            // No AD envelope, proceed directly to sustain
            env_index = ENV_SUSTAIN;
            visitEnv(adsr->env_sustain);
         }

         // Dyac_host_printf("xxx startADSR: skip ENV_ATTACK\n");

         // // if(0 == current_env->num_elements)
         // // {
         // //    // No S envelope
         // // }
      }

      // Dyac_host_printf("xxx startADSR: leave this=%p current_env=%p elements=%p num_elements=%u\n", this, current_env, current_env->elements, current_env->num_elements);
   }
}

void StADSRPlayer::stopADSR(void) {
   adsr = NULL;
}

sBool StADSRPlayer::isInRelease(void) const {
   return (ENV_RELEASE == env_index);
}

void StADSRPlayer::noteOff(void) {
   // Dyac_host_printf("xxx StADSRPlayer::noteOff: b_finished=%d\n", b_finished);
   if(!b_finished)
   {
      if(NULL != adsr)
      {
         // Start release envelope
         if(YAC_MAYBE != adsr->b_sustain_loop)  // infinite sustain loop ?
         {
            env_index = ENV_RELEASE;
            time      = 0.0f;

            visitEnv(adsr->env_release);
            current_env->yacEnvSetTime(0.0f);

            if(0 == current_env->num_elements)
            {
               b_finished = 1;
            }
         }
         else if(ENV_SUSTAIN != env_index)
         {
            // Infinite sustain loop
            env_index = ENV_SUSTAIN;
            time      = 0.0f;

            visitEnv(adsr->env_sustain);
            current_env->yacEnvSetTime(0.0f);

            if(0 == current_env->num_elements)
            {
               b_finished = 1;
            }
         }

         // Dyac_host_printf("xxx StADSRPlayer::noteOff: current_env->num_elements=%d\n",current_env->num_elements);
         // Dyac_host_printf("xxx StADSRPlayer::noteOff: end of S env. sustain_level=%f\n", sustain_level);
      }
   }
}


sF32 StADSRPlayer::tick(void) {
   sF32 r = 0.0f;
   sF32 t;

   // if(adsr->b_abs_levels)
       // Dyac_host_printf("xxx StADSRPlayer: b_finished=%d adsr=%p idx=%d time=%f current_env_totaltime=%f\n", b_finished, adsr, env_index, time, current_env_totaltime);

   if(NULL != adsr)
   {
      if(!b_finished)
      {
         // Dyac_host_printf("xxx mod_speed=%f sp_mod_speed=%f mmdst_speed=%f\n", mod_speed, *sp_mod_speed, mmdst_speed);

         // Timescaling / env time advance
         //  (note) updated again so UI editing has an immediate effect
         current_env_timescale      = adsr->timescale * orig_st_env->timescale;
         current_env->time_advance  = orig_env->time_advance * current_env_timescale;
         sF32 stageTimescale = 1.0f;

         sF32 cIntensity = current_env_intensity * (mod_intensity + *sp_mod_intensity + mmdst_intensity);
         cIntensity = sRANGE(cIntensity, 0.0f, 1.0f);

         // Envelope finished ?
         if(time >= current_env_totaltime)
         {
            time -= current_env_totaltime;
            switch(env_index)
            {
               default:
               case ENV_ATTACK:
               {
                  // End of AD envelope, begin sustain
                  // Determine exact attack_level at end of AD env
                  current_env->yacEnvSetTime(current_env_totaltime);
                  t = current_env->yacEnvGetValue();
                  t = loc_powxy(t, orig_st_env->exponent);
                  if(t < min_level)
                     t = min_level;
                  attack_level = t * cIntensity + (default_level * (1.0f - cIntensity));
                  // Dyac_host_printf("xxx end of AD env. attack_level=%f\n", attack_level);

                  if(b_skip_sustain)
                  {
                     // Hold last value
                     b_sustain_finished = YAC_TRUE;
                     sustain_level = attack_level;

                     // Start release env (if any)
                     noteOff();
                  }
                  else
                  {
                     // Begin sustain envelope
                     visitEnv(adsr->env_sustain);
                     env_index++;
                     current_env->yacEnvSetTime((sF32)time);
                  }
               }
               break;

               case ENV_SUSTAIN:
               {
                  // End of sustain envelope
                  if(!b_sustain_finished)
                  {
                     if(YAC_FALSE != adsr->b_sustain_loop)
                     {
                        // Loop sustain envelope
                        // Dyac_host_printf("xxx loop sustain env\n");
                        current_env->yacEnvSetTime((sF32)time);
                     }
                     else
                     {
                        // Hold last value
                        b_sustain_finished = YAC_TRUE;
                     }
                  }
               }
               break;

               case ENV_RELEASE:
               {
                  // End of release envelope
                  // Dyac_host_printf("xxx end of R envelope\n");
                  b_finished = 1;
               }
               break;
            }
         }

         if(!b_finished)
         {
            switch(env_index)
            {
               default:
               case ENV_ATTACK:
               {
                  // Process attack/decay envelope
                  if(current_env->num_elements)
                  {
                     // Dyac_host_printf("xxx AD env get 1 current_env=%p elements=%p &elements=%p\n", current_env, current_env->elements, &current_env->elements);
                     t = current_env->yacEnvGetValue();
                     t = loc_powxy(t, orig_st_env->exponent);
                     if(t < min_level)
                        t = min_level;
                     sF32 envLvl = t;
                     // Dyac_host_printf("xxx AD env get 2 current_env=%p elements=%p\n", current_env, current_env->elements);
                     attack_level = envLvl * cIntensity + (default_level * (1.0f - cIntensity));
                     // Dyac_host_printf("xxx StADSRPlayer: AD time=%f attack_level=%f envLvl=%f speed=%f mod_speed=%f\n", time, attack_level, envLvl, current_env->time_advance, mod_speed);
                     // // // current_env->tickPrecise(mod_speed * mod_aspeed);
                     current_env->yacEnvTickPrecise(mod_speed * mod_aspeed * *sp_mod_speed * *sp_mod_aspeed * mmdst_speed * mmdst_aspeed);
                     // Dyac_host_printf("xxx AD env get 3 current_env=%p elements=%p\n", current_env, current_env->elements);
                     stageTimescale = mod_aspeed * *sp_mod_aspeed * mmdst_aspeed;
                  }
                  else
                  {
                     // No AD envelope (xxx ever reached??)
                     attack_level = default_level;
                     // Dyac_host_printf("xxx NO AD ENV!!! this=%p current_env=%p elements=%p num_elements=%u\n", this, current_env, current_env->elements, current_env->num_elements);
                  }

                  r = attack_level;
                  sustain_level = attack_level; // in case sustain is entirely skipped (noteoff)
               }
               break;

               case ENV_SUSTAIN:
               {
                  // Process sustain envelope
                  if(!b_sustain_finished)
                  {
                     if(current_env->num_elements)
                     {
                        // Have sustain envelope
                        t = current_env->yacEnvGetValue();
                        t = loc_powxy(t, orig_st_env->exponent);
                        if(t < min_level)
                           t = min_level;
                        sustain_level = t * cIntensity + (default_level * (1.0f - cIntensity));

                        switch(adsr->stage_op)
                        {
                           default:
                           case STADSR_STAGEOP_MUL:
                              sustain_level = attack_level * sustain_level;
                              break;

                           case STADSR_STAGEOP_MUL_INTENSITY:
                              sustain_level = (1.0f - cIntensity) * attack_level + sustain_level;
                              break;

                           case STADSR_STAGEOP_ADD:
                              sustain_level = attack_level + sustain_level;
                              break;
                        }

                        // if(adsr->b_abs_levels)
                        // {
                        //    Dyac_host_printf("xxx StADSRPlayer: sustainenv.abstime=%f sustain_level=%f attack_level=%f\n", current_env->abs_time, sustain_level, attack_level);
                        //    Dyac_host_printf("xxx StADSRPlayer: mod_speed=%f mod_sspeed=%f\n", mod_speed, mod_sspeed);
                        // }

                        current_env->yacEnvTickPrecise(mod_speed * mod_sspeed * *sp_mod_speed * *sp_mod_sspeed * mmdst_speed * mmdst_sspeed);
                        stageTimescale = mod_sspeed * *sp_mod_sspeed * mmdst_sspeed;
                     }
                     else
                     {
                        sustain_level = attack_level;
                     }
                  }

                  r = sustain_level;
               }
               break;

               case ENV_RELEASE:
               {
                  // Process release envelope
                  if(current_env->num_elements)
                  {
                     // Have release envelope
                     // // // r = current_env->get();
                     t = current_env->yacEnvGetValue();
                     t = loc_powxy(t, orig_st_env->exponent);
                     if(t < min_level)
                        t = min_level;
                     ////printf("xxx release cenv val=%f\n", r);
                     r = t * cIntensity + (default_level * (1.0f - cIntensity));

                     switch(adsr->stage_op)
                     {
                        default:
                        case STADSR_STAGEOP_MUL:
                           r = r * sustain_level;
                           break;

                        case STADSR_STAGEOP_MUL_INTENSITY:
                           r = r + (1.0f - cIntensity) * sustain_level;
                           break;

                        case STADSR_STAGEOP_ADD:
                           r = r + sustain_level;
                           break;
                     }

                     // printf("xxx env_release: attack_level=%f sustain_level=%f\n", attack_level, sustain_level);
                     // printf("xxx env_release: op=%d r=%f attack_level=%f sustain_level=%f\n", adsr->stage_op, r, attack_level, sustain_level);
                     // Dyac_host_printf("xxx StADSRPlayer: env_release: time=%f mod_speed=%f mod_rspeed=%f cIntensity=%f r=%f\n", time, mod_speed, mod_rspeed, cIntensity, r);
                     // // // current_env->tickPrecise(mod_speed * mod_rspeed);
                     current_env->yacEnvTickPrecise(mod_speed * mod_rspeed * *sp_mod_speed * *sp_mod_rspeed * mmdst_speed * mmdst_rspeed);
                     stageTimescale = mod_rspeed * *sp_mod_rspeed * mmdst_rspeed;
                  }
                  else
                  {
                     // No release envelope
                     b_finished = 1;
                     // Dyac_host_printf("xxx StADSRPlayer: No release envelope\n");
                     r = 0.0f;
                  }
               }
               break;
            }

            time += current_env_timescale * mod_speed * *sp_mod_speed * mmdst_speed * stageTimescale;

            final_level = r;
         } // if !b_finished
         else
         {
            r = final_level;
         }
      } // if !b_finished
      else
      {
         r = final_level;
      }

      // (note) [19Feb2020] mmdst_level is now multiplied after modmatrix processing (and it's '*' by default now)
      // // sF32 modLvl = (mod_level + *sp_mod_level + mmdst_level);
      sF32 modLvl = (mod_level + *sp_mod_level);
      modLvl = sRANGE(modLvl, 0.0f, 16.0f);
      r *= modLvl * adsr->level;

      // if( (r < -1.0f) || (r > 1.0f))
      // {
      //    printf("xxx adsr ill r=%f\n", r);
      // }

      // Dyac_host_printf("xxx ADSR r = %f this=0x%p\n", r, this);

      ////Dyac_host_printf("StADSRPlayer::tick: time=%f/%f spd=%f env_index=%i r=%f\n", time, current_env_totaltime, current_env_timescale, env_index, r);
      ////Dyac_host_printf("StADSRPlayer::tick: r=%f\n", r);
   } // if adsr
   return r;
}

sBool StADSRPlayer::isFinished(void) {
   return b_finished;
}

void StADSRPlayer::noteOffRestartRelease(void) {
   // called in b_glide_release mode (when already in release phase)
   //  => restart release at last seen release level
   sustain_level = final_level;
   noteOff();
}
