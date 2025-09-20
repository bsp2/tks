// ----
// ---- file   : StSamplePlayer.cpp
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2009-2025 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : This is part of the "syntracker" midi sequencer.
// ----
// ---- created: 23Sep2009
// ----
// ---- changed: 26Dec2009, 27Dec2009, 29Dec2009, 02Jan2010, 05Jan2010, 07Jan2010, 15Jan2010
// ----          19Jan2010, 21Jan2010, 01Mar2010, 29Apr2010, 11May2010, 28Jun2010, 29Jun2010
// ----          01Jun2010, 05Sep2010, 10Mar2011, 28Mar2011, 24Mar2013, 13Dec2018, 29Dec2018
// ----          31Dec2018, 05Jan2019, 17Jan2019, 23Mar2019, 24Mar2019, 30Mar2019, 11May2019
// ----          12May2019, 15May2019, 06Jul2019, 12Jul2019, 13Jul2019, 25Jul2019, 01Aug2019
// ----          16Feb2020, 18Feb2020, 20Feb2020, 21Feb2020, 22Feb2020, 25Feb2020, 17May2020
// ----          19May2020, 20May2020, 24May2020, 01Jun2020, 08Jun2020, 24Aug2020, 31Aug2020
// ----          12Sep2020, 28Apr2021, 20May2021, 26Jun2021, 17Jul2021, 01Aug2021, 10Aug2021
// ----          10Dec2022, 20Dec2022, 05Feb2023, 12Apr2023, 07Sep2023, 08Sep2023, 17Sep2023
// ----          21Jan2024, 20Sep2024, 28Sep2024, 01Oct2024, 31Oct2024, 15Nov2024, 11Dec2024
// ----          14Jan2025, 28May2025, 30May2025, 13Jun2025
// ----
// ----
// ----
#include <stdio.h>

#include "tksampler.h"

#include "StWaveform.h"
#include "StADSR.h"
#include "StRange.h"
#include "StSampleMutexGroup.h"
#include "StSample.h"
#include "StSampleBank.h"


#define Dbeginloopvoicesbykey \
   sUI voiceKey = (sUI) _key;\
   if(voiceKey)\
   {\
      for(sUI i = 0u; i < num_voices; i++)\
      {\
         StSampleVoice *v = voices[i];\
         if(voiceKey == v->voice_key)\
         {

#define Dendloopvoicesbykey \
         }\
      }\
   }


#define Dbeginloopvoicesbysample \
   {\
      sUI i;\
      for(i=0; i<num_voices; i++)\
      {\
         StSampleVoice *v = voices[i];\
         if(_sample == v->sample)\
         {

#define Dendloopvoicesbysample \
         }\
      }\
   }


#define Dbeginloopallvoices \
   {\
      for(sUI i = 0u; i < num_voices; i++)\
      {\
         StSampleVoice *v = voices[i];\
         if(v->voice_key > 0u)\
         {

#define Dendloopallvoices \
         }\
      }\
   }

sF32 sp_voice_bus_null_buffer[STSAMPLEVOICE_MAX_VOICE_BUS_FRAMES * 2u];
static sBool b_voice_bus_null_buffer_init = YAC_FALSE;


StSamplePlayer::StSamplePlayer(void) {
   lfsr.init((void*)this);

   voices      = NULL;
   voice_order = NULL;
   num_voices  = 0u;
   volume      = 1.0f;
   poly_mode   = STSAMPLEPLAYER_POLYMODE_OLDEST;

   default_mixrate    = 48000.0f;
   default_freq_table = NULL;

   next_voice_key = 1u;
   next_voice_alloc_idx = 0u;

   b_glide_switch = YAC_TRUE;

   float_block_off = 0.0f;
   float_block_size = STSAMPLEVOICE_BLOCK_LEN * default_mixrate;
   global_tick_nr = 0u;

   setTempo(125.0f/*bpm*/, 192/*ppq*/);

   resetModulators();

   last_started_samplebank = NULL;

#ifndef LIBSYNERGY_BUILD
   mtx_process_tick_nr = NULL;
#endif // LIBSYNERGY_BUILD

   plugin_cache.first = NULL;

   if(!b_voice_bus_null_buffer_init)
   {
      b_voice_bus_null_buffer_init = YAC_TRUE;
      ::memset((void*)sp_voice_bus_null_buffer, 0, sizeof(sp_voice_bus_null_buffer));
   }
}

StSamplePlayer::~StSamplePlayer() {
   freeVoices();
   freePluginCache();
}

void StSamplePlayer::freePluginCache(void) {
   st_plugin_cache_entry_t *c = plugin_cache.first;
   while(NULL != c)
   {
      st_plugin_cache_entry_t *n = c->next;
      if(Dyac_host_yacGetDebugLevel() > 1u)
      {
         Dyac_host_printf("xxx StSamplePlayer::freePluginCache: this=%p delete voice=%p\n", this, c->voice);
      }
      if(NULL != c->voice->info->voice_delete)
      {
         c->voice->info->voice_delete(c->voice);
      }
      else
      {
         Dyac_host_printf("[!!!] StSamplePlayer::freePluginCache: plugin does not implement voice_delete (id=\"%s\")\n", c->voice->info->id);
      }
      c->voice = NULL;
      c->b_used = YAC_FALSE;
      delete c;
      c = n;
   }
   plugin_cache.first = NULL;
}

st_plugin_cache_entry_t *StSamplePlayer::findOrCreatePluginCacheEntry(st_plugin_info_t *_info, sUI _voiceIdx) {
   // (todo) optimize cache search
   st_plugin_cache_entry_t *c = plugin_cache.first;
   st_plugin_cache_entry_t *p = NULL;
   while(NULL != c)
   {
      if(!c->b_used)
      {
         if(c->voice->info == _info)
         {
            c->b_used = YAC_TRUE;
            return c;
         }
      }

      p = c;
      c = c->next;
   }

   // Add new instance
   if(NULL != _info->voice_new)
   {
      c = new(std::nothrow)st_plugin_cache_entry_t;
      if(NULL != c)
      {
         if(_info->api_version < 2u)
         {
            // v1: no 'voiceIdx' parameter
            st_plugin_voice_t *(ST_PLUGIN_API *voiceNew) (st_plugin_info_t *_info) =
               (st_plugin_voice_t *(ST_PLUGIN_API *) (st_plugin_info_t *_info)) _info->voice_new;
            c->voice = voiceNew(_info);
         }
         else
         {
            // v2+: pass 'voiceIdx' parameter
            c->voice = _info->voice_new(_info, _voiceIdx);
         }
         if(NULL != c->voice)
         {
            c->voice->info = _info;
            c->voice->voice_idx = _voiceIdx;
            c->next = p;
            plugin_cache.first = c;
            c->b_used = YAC_TRUE;
            if(Dyac_host_yacGetDebugLevel() > 1u)
               Dyac_host_printf("xxx StSamplePlayer::findOrCreatePluginInstance: add new voice instance (plugin id=\"%s\" voiceIdx=%u)\n", _info->id, _voiceIdx);
            return c;
         }
         else
         {
            Dyac_host_printf("[---] StSamplePlayer::findOrCreatePluginInstance: voice_new() failed (plugin id=\"%s\" voiceIdx=%u)\n", _info->id, _voiceIdx);
            delete c;
         }
      }
      // else: failed to alloc cache entry
   }
   else
   {
      Dyac_host_printf("[---] StSamplePlayer::findOrCreatePluginInstance: instance_new() cbk is NULL (plugin id=\"%s\")\n", _info->id);
   }

   // Failed
   return NULL;
}

void StSamplePlayer::resetModulators(void) {
   mod_vol                   = 1.0f;
   mod_pan                   = 0.0f;
   mod_mod                   = 0.0f;
   mod_freq                  = 0.0f;
   mod_freq2                 = 0.0f;
   mod_timestretch           = 0.0f;
   mod_timestretch_bend      = 0.0f;
   mod_sampleoff             = 0.0f;
   mod_cyclelen              = 1.0f;
   mod_sampleshift           = 0.0f;
   mod_sampleshift_endofloop = 0.0f;
   mod_altvolume             = 1.0f;

   mod_lfo_freq_amt          = 1.0f;
   mod_lfo_vol_amt           = 1.0f;
   mod_lfo_pan_amt           = 1.0f;
   mod_lfo_aux_flt_amt       = 1.0f;

   mod_lfo_freq_spd          = 1.0f;
   mod_lfo_freq_lvl          = 0.0f;

   mod_lfo_vol_spd           = 1.0f;
   mod_lfo_vol_lvl           = 0.0f;

   mod_lfo_pan_spd           = 1.0f;
   mod_lfo_pan_lvl           = 0.0f;

   mod_lfo_aux_spd           = 1.0f;
   mod_lfo_aux_lvl           = 0.0f;

   mod_adsr_freq_spd         = 1.0f;
   mod_adsr_freq_lvl         = 0.0f;
   mod_adsr_freq_int         = 0.0f;
   mod_adsr_freq_aspd        = 1.0f;
   mod_adsr_freq_rspd        = 1.0f;
   mod_adsr_freq_sspd        = 1.0f;

   mod_adsr_vol_spd          = 1.0f;
   mod_adsr_vol_lvl          = 0.0f;
   mod_adsr_vol_int          = 0.0f;
   mod_adsr_vol_aspd         = 1.0f;
   mod_adsr_vol_rspd         = 1.0f;
   mod_adsr_vol_sspd         = 1.0f;

   mod_adsr_pan_spd          = 1.0f;
   mod_adsr_pan_lvl          = 0.0f;
   mod_adsr_pan_int          = 0.0f;
   mod_adsr_pan_aspd         = 1.0f;
   mod_adsr_pan_rspd         = 1.0f;
   mod_adsr_pan_sspd         = 1.0f;

   mod_adsr_aux_spd          = 1.0f;
   mod_adsr_aux_lvl          = 0.0f;
   mod_adsr_aux_int          = 0.0f;
   mod_adsr_aux_aspd         = 1.0f;
   mod_adsr_aux_rspd         = 1.0f;
   mod_adsr_aux_sspd         = 1.0f;

   mod_filter_cutoff    = 0.0f;
   mod_filter_pan       = 0.0f;
   mod_filter_offset    = 0.0f;
   mod_filter_resonance = 0.0f;

   for(sUI i = 0u; i < STSAMPLE_NUM_MODSEQ; i++)
   {
      mod_modseq[i].speed    = 1.0f;
      mod_modseq[i].level    = 0.0f;
      mod_modseq[i].numsteps = 0.0f;
      mod_modseq[i].advance  = 0.0f;
   }

#ifndef LIBSYNERGY_BUILD
   mod_liverec_loop_shift = 0;
#endif // LIBSYNERGY_BUILD

   // // ::memset((void*)perf_ctl, 0, sizeof(perf_ctl));
   for(sUI i = 0u; i < STSAMPLEPLAYER_NUM_PERFCTL; i++)
   {
      perf_ctl[i] = -1.0f;  // invalid
   }
   perf_ctl[STSAMPLEPLAYER_PERFCTL_PITCHBEND]    = 8192.0f;
   perf_ctl[STSAMPLEPLAYER_PERFCTL_CC8_BALANCE]  = 64.0f;
   perf_ctl[STSAMPLEPLAYER_PERFCTL_CC10_PAN]     = 64.0f;

   sustain_pedal = YAC_FALSE;
   soft_pedal    = YAC_FALSE;

   ::memset((void*)note_history_ms, 0, sizeof(note_history_ms));
   ::memset((void*)note_history_note, 0, sizeof(note_history_note));
   note_history_write_idx = 0u;

   global_tick_advance = 0u;
}

void StSamplePlayer::freeVoices(void) {

   if(NULL != voices)
   {
      for(sUI i = 0u; i < num_voices; i++)
      {
         StSampleVoice *v = (StSampleVoice*) voices[i];
         YAC_DELETE(v);
      }
      delete [] voices;
      voices = NULL;
   }

   if(NULL != voice_order)
   {
      delete [] voice_order;
      voice_order = NULL;
   }

}

sSI StSamplePlayer::allocVoices(sUI _maxPoly) {

   // ::printf("xxx StSamplePlayer::allocVoices(maxPoly=%u)\n", _maxPoly);

   freeVoices();

   if(_maxPoly > STSAMPLEPLAYER_MAXVOICES)
      _maxPoly = STSAMPLEPLAYER_MAXVOICES;

   voices = new(std::nothrow) StSampleVoice*[_maxPoly];
   voice_order = new(std::nothrow) sSI[_maxPoly];
   for(sUI i = 0u; i < _maxPoly; i++)
   {
      voices[i] = YAC_NEW(StSampleVoice);
      voices[i]->voice_idx = i;
      voices[i]->setSamplePlayer(this);
      voice_order[i] = -1;
   }
   num_voices = _maxPoly;

   return YAC_TRUE;
}

sUI StSamplePlayer::getNumVoices(void) {
   return num_voices;
}

sUI StSamplePlayer::getNumUsedVoices(void) {
   sUI r = 0u;
   for(sUI i = 0u; i < num_voices; i++)
   {
      StSampleVoice *v = voices[i];
      if(v->b_used)
         r++;
   }
   return r;
}

sUI StSamplePlayer::getNumActiveVoices(void) {
   sUI r = 0u;
   for(sUI i = 0u; i < num_voices; i++)
   {
      StSampleVoice *v = voices[i];
      if(v->voice_key > 0u)
         r++;
   }
   return r;
}

sUI StSamplePlayer::getNumPlayingVoices(void) {
   sUI r = 0u;
   for(sUI i = 0u; i < num_voices; i++)
   {
      StSampleVoice *v = voices[i];
      if(v->voice_key > 0u)
      {
         if(v->b_playing)
            r++;
      }
   }
   return r;
}

sUI StSamplePlayer::getNumActiveNotes(void) {
   sUI r = 0u;
   for(sUI i = 0u; i < num_voices; i++)
   {
      StSampleVoice *v = voices[i];
      if(v->voice_key > 0u)
      {
         if(!v->b_release)
            r++;
      }
   }
   return r;
}

YAC_Object *StSamplePlayer::_getVoice(sUI _idx) {
   return (_idx < num_voices) ? voices[_idx] : NULL;
}

YAC_Object *StSamplePlayer::_findFirstActiveVoiceBySample(YAC_Object *_sample) {
   if(YAC_CHK(_sample, clid_StSample))
   {
      YAC_CAST_ARG(StSample, sample, _sample);

      for(sUI i = 0u; i < num_voices; i++)
      {
         StSampleVoice *v = voices[i];
         if(v->voice_key > 0u)
         {
            if(!v->b_release)
            {
               // Dyac_host_printf("xxx v[%u] sample=%p, compare=%p\n", i, v->sample, _sample);
               if(v->sample == sample)
               {
                  return v;
               }
            }
         }
         // else
         //    Dyac_host_printf("xxx find active: v[%u] voice_key is 0 (skip)\n", i);
      }
   }
   return NULL;
}

YAC_Object *StSamplePlayer::_findLastStartedVoiceBySample(YAC_Object *_sample) {
   if(YAC_CHK(_sample, clid_StSample))
   {
      YAC_CAST_ARG(StSample, sample, _sample);

      sSI bestVoiceKey = -1;
      sUI bestIdx = 0u;

      for(sUI i = 0u; i < num_voices; i++)
      {
         StSampleVoice *v = voices[i];
         if(v->last_voice_key > 0u)
         {
            if(v->sample == sample)
            {
               if(sSI(v->last_voice_key) > bestVoiceKey)
               {
                  bestVoiceKey = v->last_voice_key;
                  bestIdx = i;
               }
            }
         }
         // else
         //    Dyac_host_printf("xxx find active: v[%u] voice_key is 0 (skip)\n", i);
      }

      // Dyac_host_printf("xxx bestVoiceKey=%d bestIdx=%u\n", bestVoiceKey, bestIdx);

      if(-1 != bestVoiceKey)
         return voices[bestIdx];

   }
   return NULL;
}

void StSamplePlayer::stopSampleBankNotes(StSampleBank *_sb, sSI _note) {
   for(sUI i = 0u; i < num_voices; i++)
   {
      StSampleVoice *v = voices[i];
      if(v->isAllocated())
      {
         if(v->sample->parent_samplebank == _sb)
         {
            if(v->note == _note)
            {
               v->softStopVoice();
            }
         }
      }
   }
}

void StSamplePlayer::applyMaxSampleBankVoicesConstraint(StSampleBank *_sb) {
   //
   //
   // Apply samplebank.max_voices constraint
   //  - called during voice allocation
   //
   //
   sUI i;

   sUI uniqueKeys[STSAMPLEPLAYER_MAXVOICES];
   sUI numUniqueKeys = 0u;

   //
   // Count number of times the samplebank has been triggered (unique voice keys)
   //
   for(i = 0u; i < num_voices; i++)
   {
      StSampleVoice *v = voices[i];
      if(v->isAllocated())
      {
         if(v->sample->parent_samplebank == _sb)
         {
            // Check if voice_key is already on list
            sUI j = 0u;
            while(j < numUniqueKeys)
            {
               if(v->voice_key == uniqueKeys[j])
                  break;
               j++;
            }
            if(numUniqueKeys == j)
            {
               // Not in list, add it
               uniqueKeys[numUniqueKeys++] = v->voice_key;
            }
         }
      }
   }

   //printf("xxx apply sb.maxvoices constraint: numUsed=%d, stop voices according to polymode\n", numUniqueKeys);

   // (note) this is actually the number of samplebank noteons to stop
   sSI numVoicesToStop = (numUniqueKeys - _sb->max_voices);

   // (note) compare >= 0 since new voices have to be started (0== all voices in use)
   // (note) can this ever be > 0 ? (probably not since the constraint is applied each time a samplebank is started)
   if(numVoicesToStop >= 0)
   {
      //
      // Stop voices according to poly_mode
      //  - iterate all voices and find best matching voice, then stop all voices that use the same voice_key
      //
      sSI bestMatchIdx   = -1;
      sUI bestMatchTicks = 0;
      sSI bestMatchNote  = (STSAMPLEPLAYER_POLYMODE_HIGHEST==poly_mode) ? 0 : 127;

      // First try to stop voice that is already in release phase
      for(i=0; i<num_voices; i++)
      {
         StSampleVoice *v = voices[i];
         if(v->isAllocated())
         {
            if(v->sample->parent_samplebank == _sb)
            {
               if(v->b_release)
               {
                  // Is candidate, check poly_mode
                  switch(poly_mode)
                  {
                     default:
                     case STSAMPLEPLAYER_POLYMODE_OLDEST:
                     {
                        if(v->replay_ticks >= bestMatchTicks)
                        {
                           bestMatchTicks = v->replay_ticks;
                           bestMatchIdx = i;
                        }
                     }
                     break;

                     case STSAMPLEPLAYER_POLYMODE_LOWEST:
                     {
                        if(v->note <= bestMatchNote)
                        {
                           bestMatchNote = v->note;
                           bestMatchIdx = i;
                        }
                     }
                     break;

                     case STSAMPLEPLAYER_POLYMODE_HIGHEST:
                     {
                        if(v->note >= bestMatchNote)
                        {
                           bestMatchNote = v->note;
                           bestMatchIdx = i;
                        }
                     }
                     break;

                  } // switch poly_mode
               }
            }
         } // if v playing
      } // find voice

      if(-1 == bestMatchIdx)
      {
         // Stop any voice
         for(i=0; i<num_voices; i++)
         {
            StSampleVoice *v = voices[i];
            if(v->isAllocated())
            {
               if(v->sample->parent_samplebank == _sb)
               {
                  // Is candidate, check poly_mode
                  switch(poly_mode)
                  {
                     default:
                     case STSAMPLEPLAYER_POLYMODE_OLDEST:
                     {
                        if(v->replay_ticks >= bestMatchTicks)
                        {
                           bestMatchTicks = v->replay_ticks;
                           bestMatchIdx = i;
                        }
                     }
                     break;

                     case STSAMPLEPLAYER_POLYMODE_LOWEST:
                     {
                        if(v->note <= bestMatchNote)
                        {
                           bestMatchNote = v->note;
                           bestMatchIdx = i;
                        }
                     }
                     break;

                     case STSAMPLEPLAYER_POLYMODE_HIGHEST:
                     {
                        if(v->note >= bestMatchNote)
                        {
                           bestMatchNote = v->note;
                           bestMatchIdx = i;
                        }
                     }
                     break;

                  } // switch poly_mode
               }
            } // if v playing
         } // find voice
      }

      ////printf("xxx StSamplePlayer::allocVoice: count samplebankvoices numUsed=%d\n", numUsed);

      //printf("xxx maxsbvoices: bestMathIdx=%d bestMatchTicks=%d\n", bestMatchIdx, bestMatchTicks);

      //
      // Stop bestmatching voice(s)
      //   - stop all voices that match v's voicekey
      //
      StSampleVoice *v = voices[bestMatchIdx];
      sUI j;
      for(j = 0u; j < num_voices; j++)
      {
         StSampleVoice *ov = voices[j];
         // // if(ov->b_playing)
         // // {
         if(ov->voice_key == v->voice_key)
         {
            ov->softStopVoice();
         }
         // // }
      }

   }
   else
   {
      // Ok, no need to stop more voices
   }
}

void StSamplePlayer::applyMaxVoicesConstraint(StSample *_sample, sUI _voiceKey) {
   // Called during voice allocation
   //
   // voiceKey:
   //          >0: prefer to stop voices that do _not_ use the given voiceKey
   //           0: apply constraint to all playing or allocated voices

   sUI i;
   sUI numUsed = 0u;

   // Count number of voices currently playing the given sample
   for(i = 0u; i < num_voices; i++)
   {
      StSampleVoice *v = voices[i];
      if(v->isAllocated() && ((0u == _voiceKey) || (v->voice_key != _voiceKey)))
      {
         if(v->sample == _sample) //// && (v != nv) )
         {
            numUsed++;
         }
      }
   }

   // Dyac_host_printf("xxx applyMaxVoicesConstraint: sample=%p voiceKey=%u => numUsed=%u\n", _sample, _voiceKey, numUsed);

   //printf("xxx smp=\"%s\" numUsed=%d max=%d\n", (_sample->name?(const char*)_sample->name->chars:""), numUsed, _sample->max_voices);

   // Stop voices according to poly_mode
   while(numUsed >= _sample->max_voices)
   {
      sUI bestMatchIdx   = 999999u;
      sUI bestMatchTicks = 0u;
      sSI bestMatchNote  = (STSAMPLEPLAYER_POLYMODE_HIGHEST==poly_mode) ? 0 : 127;

      // First try to stop voice that is already in release phase
      for(i = 0u; i < num_voices; i++)
      {
         StSampleVoice *v = voices[i];
         if(v->isAllocated() && ((0u == _voiceKey) || (v->voice_key != _voiceKey)))
         {
            if(v->sample == _sample)
            {
               if(v->b_release)
               {
                  // Is candidate, check poly_mode
                  switch(poly_mode)
                  {
                     default:
                     case STSAMPLEPLAYER_POLYMODE_OLDEST:
                     {
                        if(v->replay_ticks >= bestMatchTicks)
                        {
                           bestMatchTicks = v->replay_ticks;
                           bestMatchIdx = i;
                        }
                     }
                     break;

                     case STSAMPLEPLAYER_POLYMODE_LOWEST:
                     {
                        if(v->note <= bestMatchNote)
                        {
                           bestMatchNote = v->note;
                           bestMatchIdx = i;
                        }
                     }
                     break;

                     case STSAMPLEPLAYER_POLYMODE_HIGHEST:
                     {
                        if(v->note >= bestMatchNote)
                        {
                           bestMatchNote = v->note;
                           bestMatchIdx = i;
                        }
                     }
                     break;

                  } // switch poly_mode
               }
            }
         } // if v playing
      } // find voice

      if(999999 == bestMatchIdx)
      {
         for(i = 0u; i < num_voices; i++)
         {
            StSampleVoice *v = voices[i];
            if(v->isAllocated() && ((0u == _voiceKey) || (v->voice_key != _voiceKey)))
            {
               if(v->sample == _sample)
               {
                  // Is candidate, check poly_mode
                  switch(poly_mode)
                  {
                     default:
                     case STSAMPLEPLAYER_POLYMODE_OLDEST:
                     {
                        if(v->replay_ticks >= bestMatchTicks)
                        {
                           bestMatchTicks = v->replay_ticks;
                           bestMatchIdx = i;
                        }
                     }
                     break;

                     case STSAMPLEPLAYER_POLYMODE_LOWEST:
                     {
                        if(v->note <= bestMatchNote)
                        {
                           bestMatchNote = v->note;
                           bestMatchIdx = i;
                        }
                     }
                     break;

                     case STSAMPLEPLAYER_POLYMODE_HIGHEST:
                     {
                        if(v->note >= bestMatchNote)
                        {
                           bestMatchNote = v->note;
                           bestMatchIdx = i;
                        }
                     }
                     break;

                  } // switch poly_mode
               }
            }
         } // find bestmatch samplevoice
      }

      // Stop best matching sample voice
      /////printf("xxx stopsamplevoice numUsed=%d idx=%d\n", numUsed, bestMatchIdx);
      if(bestMatchIdx < num_voices)
      {
         StSampleVoice *v = voices[bestMatchIdx];
         v->softStopVoice();
      }

      // Next iteration
      numUsed--;
   } // while numUsed>=max
}

StSampleVoice *StSamplePlayer::findBestMatchVoice(sUI _polyMode, sBool _bFadedOnly, sBool _bSoftStoppedOnly, sBool _bReleaseOnly, sBool _bForce) {
      ////printf("xxx find bestmatch voice\n");

   StSampleVoice *nv = NULL;

   // All voices in use. Voice allocation strategy applies..
   sUI bestMatchIdx = 999999;

   switch(_polyMode)
   {
      default:
      case STSAMPLEPLAYER_POLYMODE_OLDEST:
      {
         sUI bestMatchTicks = 0u;
         sUI i;
         for(i = 0u; i < num_voices; i++)
         {
            StSampleVoice *v = voices[i];
            if(((v->voice_key != next_voice_key) && v->canAllocate()) || _bForce)
            {
               if(v->replay_ticks >= bestMatchTicks)
               {
                  if(!_bForce)
                  {
                     // Dyac_host_printf("xxx i=%u v->isFadedOut()=%d\n", i, v->isFadedOut());
                     if(_bFadedOnly && !v->isFadedOut())
                     {
                        continue;
                     }
                     else if(_bSoftStoppedOnly && !v->isSoftStopped())
                     {
                        continue;
                     }
                     else if(_bReleaseOnly && !v->b_release)
                     {
                        continue;
                     }
                  }

                  // if(_bFadedOnly)
                  //    Dyac_host_printf("xxx set bestMatchIdx=%u\n", bestMatchIdx);

                  bestMatchTicks = v->replay_ticks;
                  bestMatchIdx = i;
               }
            }
         }
      }
      break;

      case STSAMPLEPLAYER_POLYMODE_LOWEST:
      {
         sSI bestMatchNote = 127;
         sUI i;
         for(i = 0u; i < num_voices; i++)
         {
            StSampleVoice *v = voices[i];
            if(((v->voice_key != next_voice_key) && v->canAllocate()) || _bForce)
            {
               if(v->note <= bestMatchNote)
               {
                  if(!_bForce)
                  {
                     if(_bFadedOnly && !v->isFadedOut())
                     {
                        continue;
                     }
                     else if(_bSoftStoppedOnly && !v->isSoftStopped())
                     {
                        continue;
                     }
                     else if(_bReleaseOnly && !v->b_release)
                     {
                        continue;
                     }
                  }

                  bestMatchNote = v->note;
                  bestMatchIdx = i;
               }
            }
         }
      }
      break;

      case STSAMPLEPLAYER_POLYMODE_HIGHEST:
      {
         sSI bestMatchNote = 0;
         sUI i;
         for(i = 0u; i < num_voices; i++)
         {
            StSampleVoice *v = voices[i];
            if(((v->voice_key != next_voice_key) && v->canAllocate()) || _bForce)
            {
               if(v->note >= bestMatchNote)
               {
                  if(!_bForce)
                  {
                     if(_bFadedOnly && !v->isFadedOut())
                     {
                        continue;
                     }
                     else if(_bSoftStoppedOnly && !v->isSoftStopped())
                     {
                        continue;
                     }
                     else if(_bReleaseOnly && !v->b_release)
                     {
                        continue;
                     }
                  }

                  bestMatchNote = v->note;
                  bestMatchIdx = i;
               }
            }
         }
      }
      break;

   } // switch poly_mode

   if(bestMatchIdx < num_voices)
   {
      // Dyac_host_printf("xxx findBestMatchVoice: bestMatchIdx=%d\n", bestMatchIdx);
      nv = voices[bestMatchIdx];
      nv->b_playing = false;
   }

   return nv;
}

StSampleVoice *StSamplePlayer::allocSampleVoice(StSample *_sample, sSI _note) {

   StSampleVoice *nv = NULL;

   // Dyac_host_printf("xxx StSamplePlayer::allocSampleVoice: sample=%p note=%d poly=%u\n", _sample, _note, num_voices);

   //
   //
   // Apply sample.max_voices constraint
   //  (note) this only make sense in special cases so the default for a sample
   //         is to just apply the sample bank voice limiter (i.e. sample->max_voices should usually be 0)
   //
   //
   if(_sample->max_voices > 0u)
   {
      applyMaxVoicesConstraint(_sample, next_voice_key/*prefer any other than this*/);
      applyMaxVoicesConstraint(_sample, 0u/*any voice_key*/);
   }

   // Check whether there are active voices that belong to the same mutex group as _sample
   if(NULL != _sample->mutex_group)
   {
      sUI i;
      for(i = 0u; i < num_voices; i++)
      {
         StSampleVoice *v = voices[i];
         if((v->voice_key > 0u) && (v->voice_key != next_voice_key))
         {
            if(v->sample->mutex_group == _sample->mutex_group)
            {
               // Stop mutual exclusive sample voice
               v->softStopVoice();
            }
         }
      }
   }


   // Find first inactive voice
   //  (todo) remove this ? voice keys usually remain set so that voices can be retriggered
   sUI i;
   for(i = 0u; i < num_voices; i++)
   {
      StSampleVoice *v = voices[i];
      if(!v->voice_key) // Note: breaks retrig as described above, i.e. retrig only works if sample is still playing
         //                       (voice key must be unset when voice really stops)
      {
         nv = v;
         break;
      }
   }
   // Dyac_host_printf("\n\nxxx find first inactive voice nv=%p\n", nv);


   if(NULL == nv)
   {
      // All voices in use

      // Try to find voice that is completely stopped (faded-out) already
      nv = findBestMatchVoice(poly_mode,
                              true/*bFadedOnly*/, false/*bSoftStoppedOnly*/, false/*bReleaseOnly*/,
                              false/*bForce*/
                              );
      // Dyac_host_printf("xxx find faded-out voice nv=%p\n", nv);

      if(NULL == nv)
      {
         // Try to find voice that is soft-stopped already
         nv = findBestMatchVoice(poly_mode,
                                 false/*bFadedOnly*/, true/*bSoftStoppedOnly*/, false/*bReleaseOnly*/,
                                 false/*bForce*/
                                 );
         // Dyac_host_printf("xxx find soft-stopped voice nv=%p\n", nv);

         if(NULL == nv)
         {
            // Try to find voice that is in release phase
            nv = findBestMatchVoice(poly_mode,
                                    false/*bFadedOnly*/, false/*bSoftStoppedOnly*/, true/*bReleaseOnly*/,
                                    false/*bForce*/
                                    );
            // Dyac_host_printf("xxx find release voice nv=%p\n\n", nv);

            if(NULL == nv)
            {
               // Find *any* voice (voice stealing!)
               nv = findBestMatchVoice(poly_mode,
                                       false/*bFadedOnly*/, false/*bSoftStoppedOnly*/, false/*bReleaseOnly*/,
                                       false/*bForce*/
                                       );
               // Dyac_host_printf("xxx steal voice nv=%p\n\n", nv);
            }
            else
            {
               //printf("xxx re-use release voice nv=%p\n", nv);
            }
         }
         else
         {
            //printf("xxx re-use softstopped voice nv=%p\n", nv);
         }
      }
      else
      {
         //printf("xxx re-use faded-out voice nv=%p\n", nv);
      }
   } // polymode voicealloc

   sBool bTryStopOther = YAC_TRUE;

   // Dyac_host_printf("xxx allocSampleVoice: nv=%p nv->b_playing=%d\n", nv, nv->b_playing);

   if(NULL == nv)
   {
#if 0
      Dyac_host_printf("[~~~] StSamplePlayer::allocSampleVoice: all voices in use, force POLYMODE_OLDEST\n");
#endif
      nv = findBestMatchVoice(STSAMPLEPLAYER_POLYMODE_OLDEST,
                              false/*bFadedOnly*/, false/*bSoftStoppedOnly*/, false/*bReleaseOnly*/,
                              true/*bForce*/
                              );
      bTryStopOther = YAC_FALSE;
   }


   if(NULL == nv)
   {
      Dyac_host_printf("[~~~] StSamplePlayer::allocSampleVoice: failed to allocate voice !! force-realloc first voice..\n");
      nv = voices[0];
      nv->b_playing = false;
      bTryStopOther = YAC_FALSE;
   }

   // (note) nv can still be 0 if multiple notes are triggered on the same tick (and same track)
   //         in that case simply do not start a new voice
   if(NULL != nv)
   {
      if(bTryStopOther)
      {
         // Stop all other voices that share the allocated voice's voicekey
         {
            sUI i;
            for(i=0; i<num_voices; i++)
            {
               StSampleVoice *v = voices[i];
               if(v->b_playing && (v->voice_key == nv->voice_key))
               {
                  // Dyac_host_printf("xxx allocSampleVoice: stop other voice[%u] with key=%d\n", i, nv->voice_key);
                  v->softStopVoice();
               }
            }
         }
      }

      // Dyac_host_printf("xxx end allocSampleVoice nv=0x%p\n", nv);

      lazyCreateVoicePlugins(_sample, nv);

      nv->prepareToPlay(_sample, next_voice_key, next_voice_alloc_idx++);
   }

   //printf("xxx nv=0x%p nv->voice_key=%d\n", nv, nv->voice_key);


   // Return allocated voice
   return nv;
}

StSampleVoice *StSamplePlayer::allocSampleVoiceGlide(StSample *_sample, sSI _note) {

   StSampleVoice *nv = NULL;
   sSI bestMatchNoteDist = 999999;
   for(sUI i = 0u; i < num_voices; i++)
   {
      StSampleVoice *v = voices[i];

      sBool bCanAlloc = v->b_playing && (v->sample == _sample) && !v->b_alloc;
      bCanAlloc = bCanAlloc && (v->sample->b_glide_release || !v->b_release);
      if(bCanAlloc && (0u != _sample->glide_min_time))
      {
         bCanAlloc = (v->replay_ticks >= _sample->glide_min_time) && (0u == _sample->glide_max_time || v->replay_ticks <= _sample->glide_max_time);
      }

      if(bCanAlloc)
      {
         sSI noteDist = v->note - _note;
         noteDist = sABS(noteDist);
         if( (noteDist <= _sample->glide_max_note_dist) && (noteDist < bestMatchNoteDist) )
         {
            bestMatchNoteDist = noteDist;
            nv = v;
         }
      }
   }

   // Return allocated voice
   return nv;
}

void StSamplePlayer::lazyCreateVoicePlugins(StSample *_sample, StSampleVoice *_nv) {
   // Lazy-instantiate voice plugins
   for(sUI pluginIdx = 0u; pluginIdx < STSAMPLE_NUM_PLUGINS; pluginIdx++)
   {
      StPluginShared *pluginShared = _sample->plugins[pluginIdx].shared;
      st_plugin_cache_entry_t *pce = _nv->plugin_cache_entries[pluginIdx];
      if(NULL != pluginShared)
      {
         if(Dyac_host_yacGetDebugLevel() > 1u)
            Dyac_host_printf("xxx StSamplePlayer::lazyCreateVoicePlugins: pluginIdx=%u pluginShared=%p pce=%p\n", pluginIdx, pluginShared, pce);
         StPluginInfo *pluginInfo = pluginShared->info;
         if(Dyac_host_yacGetDebugLevel() >= 10u)
            Dyac_host_printf("xxx StSamplePlayer::lazyCreateVoicePlugins: pluginIdx=%u pluginInfo=%p\n", pluginIdx, pluginInfo);
         if(NULL != pce)
         {
            if(pce->voice->info != pluginInfo->info)
            {
               // Plugin changed, remove old instance (mark cache entry as unused)
               pce->b_used = YAC_FALSE;
               pce->voice->shared = NULL;
               pce = NULL;
            }
            else
            {
               // Keep previous plugin instance (just update shared ptr)
               pce->voice->shared = pluginShared->shared;
            }
         }
         if(NULL == pce)
         {
            // Find or create new plugin instance
            if(Dyac_host_yacGetDebugLevel() >= 10u)
               Dyac_host_printf("xxx findOrCreatePluginCacheEntry(info=%p)\n", pluginInfo->info);
            pce = findOrCreatePluginCacheEntry(pluginInfo->info, _nv->voice_idx);
            pce->voice->shared = pluginShared->shared;
         }
         _nv->plugin_cache_entries[pluginIdx] = pce;
      }
      else
      {
         // No plugin assigned
         if(NULL != pce)
         {
            // Remove previous plugin
            pce->b_used = YAC_FALSE;
            pce->voice->shared = NULL;
            _nv->plugin_cache_entries[pluginIdx] = NULL;
         }
      }
   }
}

sF32 StSamplePlayer::getDefaultMixRate(void) {
   return default_mixrate;
}

void StSamplePlayer::setDefaultMixRate(sF32 _mixRate) {
   if(_mixRate != default_mixrate)
   {
      default_mixrate = _mixRate;

      float_block_off = 0.0f;
      float_block_size = STSAMPLEVOICE_BLOCK_LEN * default_mixrate;
   }
}

YAC_Object *StSamplePlayer::getDefaultFreqTable(void) {
   return default_freq_table;
}

void StSamplePlayer::setDefaultFreqTable(YAC_Object *_fa) {
   YAC_FloatArray *tbl = NULL;

   if(YAC_BCHK(_fa, YAC_CLID_FLOATARRAY))
   {
      YAC_CAST_ARG(YAC_FloatArray, fa, _fa);
      if(fa->num_elements >= 128u)
      {
         tbl = fa;
      }
      else
      {
         Dyac_host_printf("StSamplePlayer::setDefaultFreqTable: table too small (%d < 128)\n", fa->num_elements);
      }
   }
   else
   {
      ////Dyac_throw_def(InvalidPointer, "freqTable is not a valid FloatArray object");
   }
   default_freq_table = tbl;
}

void StSamplePlayer::setVolume(sF32 _vol) {
   volume = _vol;
}

sF32 StSamplePlayer::getVolume(void) {
   return volume;
}

void StSamplePlayer::setPolyMode(sUI _mode) {
   poly_mode = _mode;
   if(poly_mode >= STSAMPLEPLAYER_NUM_POLYMODES)
   {
      poly_mode = STSAMPLEPLAYER_POLYMODE_OLDEST;
   }
}

sUI StSamplePlayer::getPolyMode(void) {
   return poly_mode;
}

void StSamplePlayer::allNotesOff(void) {
   sUI i;
   for(i = 0u; i < num_voices; i++)
   {
      StSampleVoice *v = voices[i];
      if(v->voice_key > 0u)
      {
         // // v->noteOff(0.0f/*velocity*/);
         v->softStopVoice();
      }
      else
      {
         v->_resetVoice();
      }
   }

   // Reset controllers
   // // resetModulators();
}

void StSamplePlayer::allNotesOffRelease(void) {
   sUI i;
   for(i = 0u; i < num_voices; i++)
   {
      StSampleVoice *v = voices[i];
      if(v->voice_key > 0u)
      {
         v->noteOff(0.0f/*vel*/);
      }
   }
}

void StSamplePlayer::resetVoices(void) {
   for(sUI i = 0u; i < num_voices; i++)
   {
      StSampleVoice *v = voices[i];
      v->_resetVoice();
   }

   // Reset controllers
   resetModulators();

   last_started_samplebank = NULL;
}

void StSamplePlayer::resetVoicesBySample(YAC_Object *_sample) {
   for(sUI i = 0u; i < num_voices; i++)
   {
      StSampleVoice *v = voices[i];
      // // if(v->isAllocated())
      {
         if((void*)v->sample == (void*)_sample)
         {
            v->_resetVoice();
         }
      }
   }
}

sSI StSamplePlayer::noteOff(sSI _note, sF32 velocity) {
   // Dyac_host_printf("xxx StSamplePlayer::noteoff: note=%d\n", _note);

   sSI r = 0;

   for(sUI i = 0u; i < num_voices; i++)
   {
      StSampleVoice *v = voices[i];
      if(v->voice_key > 0u)
      {
         if(v->note == _note)
         {
            v->noteOff(velocity);
            r++;
         }
      }
   }

   return r;
}

sSI StSamplePlayer::noteOffByKey(sSI _key, sF32 velocity) {
   // Dyac_host_printf("xxx StSamplePlayer::noteoff: key=%d\n", _key);

   sSI r = 0;

   for(sUI i = 0u; i < num_voices; i++)
   {
      StSampleVoice *v = voices[i];
      if(sSI(v->voice_key) == _key)
      {
         // printf("xxx StSamplePlayer::noteOff: key=%d note=%d\n", _key, v->note);
         v->noteOff(velocity);
         r++;
      }
   }

   return r;
}

void StSamplePlayer::addToNoteHistory(sSI _note) {
   note_history_ms[note_history_write_idx] = Dyac_host_yacMilliSeconds();
   note_history_note[note_history_write_idx] = _note;
   // Dyac_host_printf("xxx addToNoteHistory: ms=%u note=%d\n", note_history_ms[note_history_write_idx], note_history_note[note_history_write_idx]);
   note_history_write_idx = (note_history_write_idx + 1u) & NOTE_HISTORY_LEN_MASK;
}

void StSamplePlayer::getNoteHistory(YAC_Object *_ia, sUI _maxDeltaMilliSec) {
   if(YAC_Is_IntArray(_ia))
   {
      YAC_CAST_ARG(YAC_IntArray, ia, _ia);
      sUI now = Dyac_host_yacMilliSeconds();
      for(sUI i = 0u; i< NOTE_HISTORY_LEN; i++)
      {
         sUI dt = now - note_history_ms[i];
         // Dyac_host_printf("xxx [%u] dt=%u\n", i, dt);
         if(dt <= _maxDeltaMilliSec)
         {
            YAC_Value v;
            v.initInt(note_history_note[i]);
            ia->yacArraySet(NULL/*context*/, ia->num_elements, &v);
         }
      }
   }
}

sSI StSamplePlayer::startSample(YAC_Object *_sample, YAC_Object *_freqTableOrNull,
                                sSI _note, sF32 _vel, sF32 _mod, sF32 _vol, sF32 _pan, sF32 _freq
                                )  {
   //
   // Start single sample
   //

   sSI r = 0; // returns voice key

   if(YAC_CHK(_sample, clid_StSample))
   {
      YAC_CAST_ARG(StSample, sample, _sample);

      const sF32 *freqTable = (NULL != default_freq_table) ? default_freq_table->elements : NULL;
      if(NULL != freqTable)
      {
         StSampleVoice *v = NULL;

         // // if(STSAMPLE_GLIDE_NONE != sample->glide_type && (b_glide_switch ^ sample->b_glide_invert))
         if(STSAMPLE_GLIDE_NONE != sample->glide_type && b_glide_switch)
         {
            // Find voice that is still playing and has nearest distance to new note
            v = allocSampleVoiceGlide(sample, _note);

            if(NULL != v)
            {
               // Assign new voice key
               v->voice_key = next_voice_key;
               v->b_glide = YAC_TRUE;
               v->b_allow_smpoff = YAC_FALSE;
               v->b_release = YAC_FALSE;  // [15Nov2024]
               v->b_release_pulse = YAC_FALSE;  // [15Nov2024]

               if(1u == sample->max_voices)
               {
                  v->removeFromGlideNoteHistory(v->note, YAC_FALSE/*bLast*/);
                  v->addToGlideNoteHistory(v->note);
               }
               else
               {
                  v->clearGlideNoteHistory();
               }

               // Set glide source note
               v->glide_src_note = v->glide_src_note_orig = v->getCurrentGlideNote();
               v->glide_time = 0.0f;

               v->b_used = YAC_TRUE;  // was set to FALSE by softStopVoice()
            }
         }

         if(NULL == v)
         {
            v = allocSampleVoice(sample, _note);
         }

         if(NULL != v)
         {
            v->voice_bus_first_voice = v;
            v->voice_bus_buffers = v->voice_bus_buffers_self;
            v->voice_bus_buffers[0] = v->voice_bus_buffer_self;
            v->voice_bus_voices[0] = v;
            for(sUI i = 1u; i < STSAMPLEVOICE_MAX_LAYERS; i++)
               v->voice_bus_buffers[i] = sp_voice_bus_null_buffer;

            v->layer_idx  = 0u;
            v->num_layers = 1u;
            v->startVoiceInt(sample, NULL/*freqTable=default*/, default_mixrate, _note, _vel, _mod, _vol, _pan, _freq);

            r = (sSI) next_voice_key;

            addToNoteHistory(_note);

            next_voice_key++;
         }
         else
         {
            r = 0;
         }
      }
      else
      {
         Dyac_host_printf("[---] StSamplePlayer::startSample: FAILED (invalid frequency table).\n");
      }
   }

   return r;
}

sSI StSamplePlayer::startSampleBank(YAC_Object *_sampleBank, YAC_Object *_freqTableOrNull,
                                    sSI _note, sF32 _vel, sF32 _mod, sF32 _vol, sF32 _pan, sF32 _freq
                                    ) {
   //
   // Start all samples that match the given freq/vel/mod zone
   //
   // Also see initStartedVoiceByKey()
   //

   sSI r = 0; // Returns voice key

   if(YAC_CHK(_sampleBank, clid_StSampleBank))
   {
      YAC_CAST_ARG(StSampleBank, sb, _sampleBank);

      last_started_samplebank = sb;

      const sF32 *freqTable = (NULL != default_freq_table) ? default_freq_table->elements : NULL;
      if(NULL != freqTable)
      {
         if(sb->b_unique_notes)
         {
            // Soft-stop voices (=> b_used=false, b_playing=true)
            stopSampleBankNotes(sb, _note);
         }

         applyMaxSampleBankVoicesConstraint(sb);

         sF32 freq2 = 0.0f;
         sF32 vol2  = 1.0f;  // (note) sb->volume is multiplied in StSampleVoice::calcNextBlockState()
         sF32 pan2  = sb->pan;
         sF32 off2  = 0.0f;

         if(sb->freq_rand_amount > 0.0f)
         {
            freq2 += (lfsr.randf()*2.0f*sb->freq_rand_amount) - sb->freq_rand_amount;
         }

         if(sb->volume_rand_amount > 0.0f)
         {
            vol2 += (lfsr.randf()*2.0f*sb->volume_rand_amount) - sb->volume_rand_amount;
         }

         if(sb->pan_rand_amount > 0.0f)
         {
            pan2 += (lfsr.randf()*2.0f*sb->pan_rand_amount) - sb->pan_rand_amount;
         }

         if(sb->sampleoffset_rand_amount > 0.0f)
         {
            off2 += lfsr.randf() * sb->sampleoffset_rand_amount;
         }

         ////printf("xxx StSamplePlayer::startSampleBank: BEGIN\n");
         StSample *c = sb->first_sample;
         StSampleVoice *firstVoice = NULL;
         sUI layerIdx = 0u;
         if(NULL != c)
         {
            sUI i = 0;
            while( NULL != c && i < num_voices )
            {
               StSample *n = c->next;

               if(c->filterNoteOn(_note, _vel, _mod))
               {
                  sBool bGlide = (STSAMPLE_GLIDE_NONE != c->glide_type);

                  // Dyac_host_printf("xxx c=%p c->alt.num=%u\n", c, c->alt.num);
                  if(c->alt.b_enable && c->alt.num > 0u)
                  {
                     // Alt group header
                     StSample *a = NULL;

                     if(bGlide || sb->b_realloc)
                     {
                        // Find zone in current alt-glide group that is already playing
                        n = c;
                        for(sUI i = 0u;  i < c->alt.num; i++)
                        {
                           if(NULL == n)
                           {
                              // Should not be reachable
                              Dyac_host_printf("[~~~] StSamplePlayer::startSampleBank: alt-glide n == NULL !!!\n");
                              break;
                           }

                           StSampleVoice *v = findActiveVoiceBySample(n, YAC_FALSE/*bAllowInRelease*/);
                           // Dyac_host_printf("[~~~] StSamplePlayer::startSampleBank: alt-glide activeVoice=%p\n", v);
                           if(NULL != v)
                           {
                              a = n;
                              break;
                           }

                           n = n->next;
                           // Dyac_host_printf("xxx seek alt-glide, n=%p\n", n);
                        }
                     }

                     if(NULL == a)
                     {
                        // Pick new random sample from alt-group
                        a = c->getNextAltRandSample();
                     }

                     if(NULL != a)
                     {
                        // Skip other alt group samples (seek to next alt header sample or NULL)
                        n = c;
                        for(sUI i = 0u;  i < c->alt.num; i++)
                        {
                           if(NULL == n)
                           {
                              // Should not be reachable
                              Dyac_host_printf("[~~~] StSamplePlayer::startSampleBank: alt n == NULL !!!\n");
                              break;
                           }
                           n = n->next;
                           // Dyac_host_printf("xxx seek alt, n=%p\n", n);
                        }
                        c = a;
                     }
                     else
                     {
                        // Should not be reachable
                        Dyac_host_printf("[~~~] StSamplePlayer::startSampleBank: getNextAltSample() returned NULL !!!\n");
                     }
                  }

                  StSampleVoice *v = NULL;

                  if(bGlide)
                  {
                     // Find voice that is still playing and has nearest distance to new note
                     v = allocSampleVoiceGlide(c, _note);
                     // Dyac_host_printf("xxx allocSampleVoiceGlide returned v=%p\n", v);

                     if(NULL != v)
                     {
                        // Assign new voice key
                        // Dyac_host_printf("xxx glide v->voice_key=%u\n", v->voice_key);
                        v->voice_key = next_voice_key;
                        v->b_glide = YAC_TRUE;
                        v->b_allow_smpoff = YAC_FALSE;
                        v->b_release = YAC_FALSE;  // [15Nov2024]
                        v->b_release_pulse = YAC_FALSE;  // [15Nov2024]

                        if(1u == c->max_voices)
                        {
                           v->removeFromGlideNoteHistory(v->note, YAC_FALSE/*bLast*/);
                           v->addToGlideNoteHistory(v->note);
                        }
                        else
                        {
                           v->clearGlideNoteHistory();
                        }

                        // Set glide source note
                        v->glide_src_note = v->glide_src_note_orig = v->getCurrentGlideNote();
                        v->glide_time = 0.0f;

                        v->b_used = YAC_TRUE;  // was set to FALSE by softStopVoice()
                     }
                  }

                  if(NULL == v)
                  {
                     if(sb->b_realloc)
                     {
                        // Find voice that is already playing the sample
                        //  (note) e.g. when using mono analog-style envelopes
                        v = findActiveVoiceBySample(c, YAC_TRUE/*bAllowInRelease*/);
                        // Dyac_host_printf("[~~~] StSamplePlayer::startSampleBank: realloc v=%p\n", v);
                        if(NULL != v)
                        {
                           lazyCreateVoicePlugins(c, v);  // should already exist
                           v->prepareToPlay(c, next_voice_key, next_voice_alloc_idx++);
                        }
                     }
                     
                     if(NULL == v)
                     {
                        // Allocate new voice (calls prepareToPlay() and assigns voice key)
                        v = allocSampleVoice(c, _note);
                        // Dyac_host_printf("xxx allocSampleVoice returned v=%p\n", v);
                     }

                     if(NULL != v)
                     {
                        v->glide_src_note = v->glide_src_note_orig = sF32(_note);
                        v->glide_time = 1.0f;
                        v->b_glide = YAC_FALSE;
                        v->clearGlideNoteHistory();
                     }
                  }

                  if(NULL != v)
                  {
                     // Set "alloc" flag to prevent voice from being reallocated twice
                     v->b_alloc = YAC_TRUE;

                     if(NULL == firstVoice)
                     {
                        firstVoice = v;
                        memset(&firstVoice->voice_bus_buffers_self, 0, sizeof(firstVoice->voice_bus_buffers_self));
                     }
                     v->voice_bus_first_voice = firstVoice;
                     v->voice_bus_buffers = firstVoice->voice_bus_buffers_self;
                     if(layerIdx < STSAMPLEVOICE_MAX_LAYERS)
                     {
                        v->voice_bus_buffers[layerIdx] = v->voice_bus_buffer_self;
                        firstVoice->voice_bus_voices[layerIdx] = v;
                     }
                     v->layer_idx = layerIdx++;

                     v->startVoiceInt(c, NULL/*freqTable=default*/, default_mixrate, _note, _vel, _mod, _vol, _pan, _freq);
                     // Dyac_host_printf("xxx    v->queued_noteon.b_valid=%d\n", v->queued_noteon.b_valid);

                     if(!v->b_glide)
                     {
                        // Set initial freq/vol/pan/smpoff randomization
                        v->_setFreq2(freq2);
                        v->_setVol2(vol2);
                        v->_setPan2(pan2);
                        v->_setSampleOff2(off2);
                     }

                     // // r++;//?
                     i++;
                  }
                  else
                  {
                     return 0;
                  }
               } // if c->filterNoteOn
               // Dyac_host_printf("xxx allocSampleVoice: n=%p\n", n);
               c = n;
            }
            /////printf("xxx StSamplePlayer::startSampleBank: END\n");

            if(NULL != firstVoice)
            {
               if(layerIdx <= STSAMPLEVOICE_MAX_LAYERS)
                  firstVoice->num_layers = layerIdx;
               else
                  firstVoice->num_layers = STSAMPLEVOICE_MAX_LAYERS;

               for(sUI i = layerIdx; i < STSAMPLEVOICE_MAX_LAYERS; i++)
               {
                  firstVoice->voice_bus_buffers[i] = sp_voice_bus_null_buffer;
               }
            }

            // Debug-print voices
            if(0)
            {
               for(sUI i = 0u; i < num_voices; i++)
               {
                  StSampleVoice *v = voices[i];
                  Dyac_host_printf("voices[%2u] %svoice_key=%d b_queued=%d b_playing=%d fdcountdown=%d\n", i, ((v->voice_key == next_voice_key)?"*":" "), v->voice_key, v->queued_noteon.b_valid, v->b_playing, v->volramp_fadeout_countdown);
               }
            }

            r = next_voice_key;

            addToNoteHistory(_note);

            next_voice_key++;

            // // // Reset "alloc" flag (==> done in initStartedVoicesByKey())
            // // {
            // //    sUI i;
            // //    for(i=0; i < num_voices; i++)
            // //    {
            // //       StSampleVoice *v = (StSampleVoice*) voices[i];
            // //       v->b_alloc = YAC_FALSE;
            // //    }
            // // }
         }
         else
         {
            // No samples to play
         }
      }
      else
      {
         Dyac_host_printf("[---] StSamplePlayer::startSampleBank: FAILED (invalid frequency table).\n");
      }
   }
   else
   {
      Dyac_throw_def(InvalidPointer, "_sampleBank is not a valid StSampleBank object");
   }

   return r;
}

sSI StSamplePlayer::_findVoicesByKey(sSI _key, YAC_Object *_retPA) {
   sSI r = 0;

   if(YAC_BCHK(_retPA, YAC_CLID_POINTERARRAY))
   {
      sUI voiceKey = (sUI) _key;
      YAC_CAST_ARG(YAC_PointerArray, pa, _retPA);

      sUI i;
      for(i = 0u; i < num_voices; i++)
      {
         StSampleVoice *v = voices[i];
         if(voiceKey == v->voice_key)
         {
            YAC_Value av;
            av.initObject(v, 0);
            pa->yacArraySet(NULL, pa->num_elements, &av); // add element
            r++;
         }
      } // loop voices
   }

   return r;
}

StSampleVoice *StSamplePlayer::findActiveVoiceBySample(StSample *_s, sBool _bAllowInRelease) {
   for(sUI i = 0u; i < num_voices; i++)
   {
      StSampleVoice *v = voices[i];
      if(v->voice_key > 0u && v->b_playing && v->sample == _s && (_bAllowInRelease || !v->b_release))
         return v;
   }
   return NULL;
}

StSampleVoice *StSamplePlayer::findVoiceByKeyAndSample(sSI _key, StSample *_s) {
   for(sUI i = 0u; i < num_voices; i++)
   {
      StSampleVoice *v = voices[i];
      if(_key == (sSI)v->voice_key)
      {
         if(v->sample == _s)
            return v;
      }
   }
   return NULL;
}

sSI StSamplePlayer::_findPreviousGlideNoteByKeyAndHistoryIdx(sSI _key, sSI _historyIdx) {

   if(_historyIdx >= 0)
   {
      sUI voiceKey = (sUI) _key;

      sUI i;
      for(i = 0u; i < num_voices; i++)
      {
         StSampleVoice *v = voices[i];
         if( (voiceKey == v->voice_key) && v->b_glide && (1u == v->sample->max_voices) )
         {
            // Dyac_host_printf("xxx StSamplePlayer::findPreviousGlideNoteByKeyAndHistoryIdx: key=%d historyIdx=%d v->glide_note_history_idx=%d\n", _key, _historyIdx, v->glide_note_history_idx);
            // for(sSI i = 0; i <= v->glide_note_history_idx; i++)
            //    Dyac_host_printf("xxx   FIND glide_note_history[%d] = %d\n", i, v->glide_note_history[i]);

            if(_historyIdx <= v->glide_note_history_idx)
            {
               return v->glide_note_history[v->glide_note_history_idx - _historyIdx];  // LIFO
            }
         }
      } // loop voices
   }

   return -1;
}

sBool StSamplePlayer::_removeFromGlideNoteHistory(sSI _note, sBool _bLastOnly) {
   sBool bChanged = YAC_FALSE;
   for(sUI i = 0u; i < num_voices; i++)
   {
      StSampleVoice *v = voices[i];
      if(v->b_glide)
      {
         bChanged |= v->removeFromGlideNoteHistory(_note, _bLastOnly);
      }
   } // loop voices

   return bChanged;
}

void StSamplePlayer::_initStartedVoicesByKey(sSI _key) {
   sUI voiceKey = (sUI) _key;

   // Dyac_host_printf("xxx initStartedVoicesByKey: key=%d\n", _key);

   if(voiceKey)
   {
      // Dyac_host_printf("xxx initStartedVoicesByKey: key=%d num_voices=%d voices=0x%p\n", voiceKey, num_voices, voices);

      for(sUI i = 0u; i < num_voices; i++)
      {
         StSampleVoice *v = voices[i];
         // Dyac_host_printf("xxx initStartedVoicesByKey: i=%d v=0x%p\n", i, v);
         if(voiceKey == v->voice_key)
         {
            // Dyac_host_printf("xxx initStartedVoice i=%d\n", i);
            // Dyac_host_printf("xxx initStartedVoicesByKey: 1 init v=0x%p\n", v);
            // printf("xxx initStartedVoicesByKey: 1 init v=0x%p v->sample=0x%p\n", v, v->sample);
            // printf("xxx initStartedVoicesByKey: 1 init v=0x%p v->sample->waveform=0x%p\n", v, v->sample->waveform);
            // printf("xxx initStartedVoicesByKey: 1 init v=0x%p v->sample->waveform->sample_data=0x%p\n", v, v->sample->waveform->sample_data);
            v->initStartedVoice();
            // Dyac_host_printf("xxx initStartedVoicesByKey: 2 init v=0x%p\n", v);
         }
      } // loop voices

      // Dyac_host_printf("xxx sortVoices\n");
      sortVoices();
      // Dyac_host_printf("xxx sortVoices 2\n");
   }

}

void StSamplePlayer::sortVoices(void) {

   for(sUI i = 0u; i < num_voices; i++)
   {
      StSampleVoice *v = voices[i];
      v->b_sort_done = YAC_FALSE;
   }

   sUI outIdx = 0u;

   for(sUI i = 0u; i < num_voices; i++)
   {
      StSampleVoice *v = voices[i];
      // Dyac_host_printf("sortVoices v[%u].b_playing=%d\n", i, v->b_playing);
      if(v->b_playing)
      {
         if(!v->b_sort_done)
         {
            if(v->voice_bus_first_voice == v)
            {
               // First voice in a group of layered voices
               v->b_sort_done = YAC_TRUE;
               StSampleVoice *firstVoice = v;
               // Dyac_host_printf("xxx voice_order[%u] = %u  (num_layers=%u)\n", outIdx, firstVoice->voice_idx, firstVoice->num_layers);
               voice_order[outIdx++] = firstVoice->voice_idx;
               for(sUI layerIdx = 1u; layerIdx < firstVoice->num_layers; layerIdx++)
               {
                  v = firstVoice->voice_bus_voices[layerIdx];
                  if(!(v->b_sort_done || (v->voice_key != firstVoice->voice_key)))
                  {
                     // Add next layer voice
                     // Dyac_host_printf("xxx voice_order[%u] = %u\n", outIdx, v->voice_idx);
                     voice_order[outIdx++] = v->voice_idx;
                     v->b_sort_done = YAC_TRUE;
                  }
                  // else: voice has been reallocated (todo: stop all layer voices in case that happens)
               }
            }
         }
      }

   }

   // Dyac_host_printf("xxx sortVoices: outIdx=%u (num_voices=%u)\n", outIdx, num_voices);

   if(outIdx < num_voices)
      voice_order[outIdx] = -1;

}

void StSamplePlayer::_restartVoicesByKey(sSI _key) {
   sUI voiceKey = (sUI) _key;

   if(voiceKey)
   {
      for(sUI i = 0u; i < num_voices; i++)
      {
         StSampleVoice *v = voices[i];
         if(voiceKey == v->voice_key)
         {
            /////printf("xxx StSamplePlayer::restartVoicesByKey: key=%d\n", voiceKey);
            v->_restartVoice();
         }
      } // loop voices
   }
}

void StSamplePlayer::stopVoicesByKey(sSI _key) {
   Dbeginloopvoicesbykey;
   /////v->stopVoice();
   v->softStopVoice();
   Dendloopvoicesbykey;
}

void StSamplePlayer::stopVoicesBySample(YAC_Object *_sample) {
   for(sUI i = 0u; i < num_voices; i++)
   {
      StSampleVoice *v = voices[i];
      if(v->isAllocated())
      {
         if((void*)v->sample == (void*)_sample)
         {
            v->stopVoice();
            v->voice_key = 0u;  // [15Jun2019] try to fix crash while previewing samples (noteOffByKey)
         }
      }
   }
}

void StSamplePlayer::stopLFOVolBySample(YAC_Object *_sample) {
   Dbeginloopvoicesbysample;
   v->lfo_vol.stopLFO();
   Dendloopvoicesbysample;
}

void StSamplePlayer::stopLFOPanBySample(YAC_Object *_sample) {
   Dbeginloopvoicesbysample;
   v->lfo_pan.stopLFO();
   Dendloopvoicesbysample;
}

void StSamplePlayer::stopLFOFreqBySample(YAC_Object *_sample) {
   Dbeginloopvoicesbysample;
   v->lfo_freq.stopLFO();
   Dendloopvoicesbysample;
}

void StSamplePlayer::stopADSRVolBySample(YAC_Object *_sample) {
   Dbeginloopvoicesbysample;
   v->adsr_vol.stopADSR();
   Dendloopvoicesbysample;
}

void StSamplePlayer::stopADSRPanBySample(YAC_Object *_sample) {
   Dbeginloopvoicesbysample;
   v->adsr_pan.stopADSR();
   Dendloopvoicesbysample;
}

void StSamplePlayer::stopADSRFreqBySample(YAC_Object *_sample) {
   Dbeginloopvoicesbysample;
   v->adsr_freq.stopADSR();
   Dendloopvoicesbysample;
}

void StSamplePlayer::_render(YAC_Object *_fa) {
   if(YAC_BCHK(_fa, YAC_CLID_FLOATARRAY))
   {
      YAC_CAST_ARG(YAC_FloatArray, buf, _fa);

      renderInt(buf, NULL/*inputsOrNull*/);
   }
   else
   {
      Dyac_throw_def(InvalidPointer, "_buf is not a valid FloatArray object");
   }
}

void StSamplePlayer::_renderWithInputs(YAC_Object *_fa, YAC_Object *_paInputs, sBool _bRender, sUI _processTickNr) {
#ifndef LIBSYNERGY_BUILD
   // Dyac_host_printf("xxx StSamplePlayer::_renderWithInputs: ENTER\n");
   if(YAC_BCHK(_fa, YAC_CLID_FLOATARRAY))
   {
      YAC_CAST_ARG(YAC_FloatArray, buf, _fa);

      sUI numFrames = buf->num_elements >> 1;

      if(YAC_BCHK(_paInputs, YAC_CLID_POINTERARRAY))
      {
         YAC_CAST_ARG(YAC_PointerArray, paInputs, _paInputs);

         sUI numInputs = paInputs->num_elements;  // up to 48
         if(numInputs > STSAMPLE_MAX_INPUTS)
            numInputs = STSAMPLE_MAX_INPUTS;
         const sF32 *inputs[STSAMPLE_MAX_INPUTS/*4*/];

         // Dyac_host_printf("xxx StSamplePlayer::renderWithInputs: numInputs=%u\n", numInputs);

         sUI i = 0u;
         for(; i < numInputs; i++)
         {
            inputs[i] = NULL;
            YAC_Value r;
            paInputs->yacArrayGet(NULL/*context*/, i, &r);
            if(r.type >= YAC_TYPE_OBJECT)
            {
               if(YAC_BCHK(r.value.object_val, YAC_CLID_FLOATARRAY))
               {
                  YAC_CAST_ARG(YAC_FloatArray, faIn, r.value.object_val);
                  if(faIn->num_elements >= numFrames)
                  {
                     inputs[i] = faIn->elements;
                     // Dyac_host_printf("xxx StSamplePlayer::renderWithInputs: set inputs[%u] = %p\n", i, inputs[i]);
                  }
                  else
                  {
                     Dyac_host_printf("[---] StSamplePlayer::renderWithInputs: input[%u] numFrames < out.numFrames (%u<%u)\n", faIn->num_elements, numFrames);
                  }
               }
               else
               {
                  Dyac_host_printf("[---] StSamplePlayer::renderWithInputs: input[%u] is not a FloatArray\n");
               }
            }
            else
            {
               Dyac_host_printf("[---] StSamplePlayer::renderWithInputs: input[%u] is not a FloatArray object\n");
            }
         }
         for(; i < STSAMPLE_MAX_INPUTS; i++)
         {
            inputs[i] = NULL;
         }

         handleLiveRecording(buf->elements, inputs, numFrames, _processTickNr);

         if(_bRender)
         {
            renderInt(buf, inputs);
         }
      }
      else
      {
         Dyac_throw_def(InvalidPointer, "_paInputs is not a valid PointerArray object");
      }
   }
   else
   {
      Dyac_throw_def(InvalidPointer, "_buf is not a valid FloatArray object");
   }
#else
   (void)_fa;
   (void)_paInputs;
   (void)_bRender;
   (void)_processTickNr;
#endif // LIBSYNERGY_BUILD
}

void StSamplePlayer::renderInt(YAC_FloatArray *buf, const sF32*const*_inputsOrNull) {

   // Dyac_host_printf("xxx StSamplePlayer::inputsOrNull=%p\n", _inputsOrNull);

   // Dyac_host_printf("xxx sp.renderInt() debug 1\n");
   sUI bufNumFrames = (buf->num_elements >> 1);
   if(bufNumFrames <= STSAMPLEVOICE_MAX_VOICE_BUS_FRAMES)
   {
      // (note) render() should be called with rather small output buffers for global LFOs/ModSeq to work correctly
      sUI numFrames = buf->yacArrayGetNumElements() / 2; // div by 2 because of stereo
      float_block_off += sF32(numFrames);

      global_tick_advance = 0u;

      while(float_block_off >= float_block_size)
      {
         global_tick_advance++;
         float_block_off -= float_block_size;
      }

      if(global_tick_advance > 0u)
         global_tick_nr++;

      sUI numVoicesRendered = 0u;
      for(sUI i = 0u; i < num_voices; i++)
      {
         sSI voiceIdx = voice_order[i];
         // Dyac_host_printf("xxx voiceIdx=%d\n", voiceIdx);
         if(voiceIdx >= 0)
         {
            StSampleVoice *v = voices[voiceIdx];
            if(v->voice_key > 0u)
            {
               // Dyac_host_printf("xxx render voice %u\n", i);

               if(NULL != _inputsOrNull)
               {
                  // Create copy of input buffer pointers
                  //  (note) will be advanced during rendering
                  const sF32 *inputs[STSAMPLE_MAX_INPUTS] = {0};

                  for(sUI i = 0u; i < STSAMPLE_MAX_INPUTS; i++)
                     inputs[i] = _inputsOrNull[i];

                  v->renderInt(buf, volume, inputs);
               }
               else
               {
                  v->renderInt(buf, volume, NULL/*inputsOrNull*/);
               }

               // Dyac_host_printf("xxx END render voice %u\n", i);
               numVoicesRendered++;
            }
            // else
            // {
            //    printf("xxx voice %d is not allocated (voice_key==0)\n", i);
            // }
         }
         else
         {
            // No more active voices
            break;
         }
      }

      // Debug: dump samples
      if(0 && (numVoicesRendered>0))
      {
         for(sUI i = 0u; i < (buf->num_elements >> 1); i++)
         {
            printf("%f, ", buf->elements[i*2+0]);
         }
         printf("\n~~~~\n");
      }
   }
   else
   {
      Dyac_host_printf("[---] StSamplePlayer::renderInt: maximum render buffer size exceeded (%u > %u)\n", bufNumFrames, STSAMPLEVOICE_MAX_VOICE_BUS_FRAMES);
   }

   // Dyac_host_printf("xxx sp.renderInt() debug 2\n");
}

void StSamplePlayer::updateVol(sF32 _v) {
   mod_vol = _v;
}

void StSamplePlayer::updateVolByKey(sSI _key, sF32 _v) {
   Dbeginloopvoicesbykey;
   v->_setVol(_v);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updatePan(sF32 _v) {
   if(_v > 1.0f)
      _v = 1.0f;
   else if(_v < -1.0f)
      _v = -1.0f;
   mod_pan = _v;
}

void StSamplePlayer::updatePanByKey(sSI _key, sF32 _v) {
   Dbeginloopvoicesbykey;
   v->_setPan(_v);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateFreq(sF32 _v) {
   mod_freq = _v;
}

void StSamplePlayer::updateFreqByKey(sSI _key, sF32 _v) {
   Dbeginloopvoicesbykey;
   v->_setFreq(_v);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateFreq2(sF32 _v) {
   mod_freq2 = _v;
}

void StSamplePlayer::updateFreq2ByKey(sSI _key, sF32 _v) {
   Dbeginloopvoicesbykey;
   v->_setFreq2(_v);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateMod(sF32 _v) {
   mod_mod = _v;
}

void StSamplePlayer::updateModByKey(sSI _key, sF32 _v) {
   Dbeginloopvoicesbykey;
   v->_setMod(_v);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateTimestretch(sF32 _v) {
   mod_timestretch = _v;
}

void StSamplePlayer::updateTimestretchByKey(sSI _key, sF32 _v) {
   Dbeginloopvoicesbykey;
   v->_setTimestretch(_v);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateTimestretchBend(sF32 _v) {
   mod_timestretch_bend = _v;
}

void StSamplePlayer::updateTimestretchBendByKey(sSI _key, sF32 _v) {
   Dbeginloopvoicesbykey;
   v->_setTimestretchBend(_v);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateSampleOff(sF32 _v) {
   mod_sampleoff = _v;
}

void StSamplePlayer::updateSampleOffByKey(sSI _key, sF32 _v) {
   Dbeginloopvoicesbykey;
   v->_setSampleOff(_v);
   Dendloopvoicesbykey;
}

void StSamplePlayer::setInitialSampleOffsetMsByKey(sSI _key, sF32 _ms) {
   Dbeginloopvoicesbykey;
   v->_setSampleOffMs(_ms);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateCycleLen(sF32 _v) {
   mod_cyclelen = _v;
}

void StSamplePlayer::updateCycleLenByKey(sSI _key, sF32 _v) {
   Dbeginloopvoicesbykey;
   v->_setCycleLen(_v);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateSampleShift(sF32 _v) {
   mod_sampleshift = _v;
}

void StSamplePlayer::updateSampleShiftByKey(sSI _key, sF32 _v) {
   Dbeginloopvoicesbykey;
   v->_setSampleShift(_v);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateSampleShiftEndOfLoop(sF32 _v) {
   mod_sampleshift_endofloop = _v;
}

void StSamplePlayer::updateSampleShiftEndOfLoopByKey(sSI _key, sF32 _v) {
   Dbeginloopvoicesbykey;
   v->_setSampleShiftEndOfLoop(_v);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateAltVolume(sF32 _v) {
   ////printf("xxx StSamplePlayer::updateAltVolume: v=%f\n", _v);
   mod_altvolume = _v;
}

void StSamplePlayer::updateAltVolumeByKey(sSI _key, sF32 _v) {
   Dbeginloopvoicesbykey;
   v->_setAltVolume(_v);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateRetrigMask(sUI _mask) {
   for(sUI i = 0u; i < num_voices; i++)
   {
      StSampleVoice *v = voices[i];
      if(v->voice_key > 0u) // is active ?
      {
         v->_setRetrigMask(_mask);
      }
   }
}

void StSamplePlayer::updateRetrigMaskByKey(sSI _key, sUI _mask) {
   Dbeginloopvoicesbykey;
   v->_setRetrigMask(_mask);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateLFOFreqSpd(sF32 _spd) {
   // // Dbeginloopallvoices;
   // // v->_setLFOFreqSpd(_spd);
   // // Dendloopallvoices;
   mod_lfo_freq_spd = _spd;
}

void StSamplePlayer::updateLFOFreqSpdByKey(sSI _key, sF32 _spd) {
   Dbeginloopvoicesbykey;
   v->_setLFOFreqSpd(_spd);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateLFOFreqLvl(sF32 _lvl) {
   // // Dbeginloopallvoices;
   // // v->_setLFOFreqLvl(_lvl);
   // // Dendloopallvoices;
   mod_lfo_freq_lvl = _lvl;
}

void StSamplePlayer::updateLFOFreqLvlByKey(sSI _key, sF32 _lvl) {
   Dbeginloopvoicesbykey;
   v->_setLFOFreqLvl(_lvl);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateLFOFreqAmt(sF32 _amt) {
   mod_lfo_freq_amt = _amt;
}

void StSamplePlayer::updateLFOFreqAmtByKey(sSI _key, sF32 _amt) {
   Dbeginloopvoicesbykey;
   v->_setLFOFreqAmt(_amt);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateLFOVolSpd(sF32 _spd) {
   // // Dbeginloopallvoices;
   // // v->_setLFOVolSpd(_spd);
   // // Dendloopallvoices;
   mod_lfo_vol_spd = _spd;
}

void StSamplePlayer::updateLFOVolSpdByKey(sSI _key, sF32 _spd) {
   Dbeginloopvoicesbykey;
   v->_setLFOVolSpd(_spd);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateLFOVolLvl(sF32 _lvl) {
   // // Dbeginloopallvoices;
   // // v->_setLFOVolLvl(_lvl);
   // // Dendloopallvoices;
   mod_lfo_vol_lvl = _lvl;
}

void StSamplePlayer::updateLFOVolLvlByKey(sSI _key, sF32 _lvl) {
   Dbeginloopvoicesbykey;
   v->_setLFOVolLvl(_lvl);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateLFOVolAmt(sF32 _amt) {
   mod_lfo_vol_amt = _amt;
}

void StSamplePlayer::updateLFOVolAmtByKey(sSI _key, sF32 _amt) {
   Dbeginloopvoicesbykey;
   v->_setLFOVolAmt(_amt);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateLFOPanSpd(sF32 _spd) {
   // // Dbeginloopallvoices;
   // // v->_setLFOPanSpd(_spd);
   // // Dendloopallvoices;
   mod_lfo_pan_spd = _spd;
}

void StSamplePlayer::updateLFOPanSpdByKey(sSI _key, sF32 _spd) {
   Dbeginloopvoicesbykey;
   v->_setLFOPanSpd(_spd);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateLFOPanLvl(sF32 _lvl) {
   // // Dbeginloopallvoices;
   // // v->_setLFOPanLvl(_lvl);
   // // Dendloopallvoices;
   mod_lfo_pan_lvl = _lvl;
}

void StSamplePlayer::updateLFOPanLvlByKey(sSI _key, sF32 _lvl) {
   Dbeginloopvoicesbykey;
   v->_setLFOPanLvl(_lvl);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateLFOPanAmt(sF32 _amt) {
   mod_lfo_pan_amt = _amt;
}

void StSamplePlayer::updateLFOPanAmtByKey(sSI _key, sF32 _amt) {
   Dbeginloopvoicesbykey;
   v->_setLFOPanAmt(_amt);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateLFOAuxSpd(sF32 _spd) {
   // // Dbeginloopallvoices;
   // // v->_setLFOAuxSpd(_spd);
   // // Dendloopallvoices;
   mod_lfo_aux_spd = _spd;
}

void StSamplePlayer::updateLFOAuxSpdByKey(sSI _key, sF32 _spd) {
   Dbeginloopvoicesbykey;
   v->_setLFOAuxSpd(_spd);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateLFOAuxLvl(sF32 _lvl) {
   // // Dbeginloopallvoices;
   // // v->_setLFOAuxLvl(_lvl);
   // // Dendloopallvoices;
   mod_lfo_aux_lvl = _lvl;
}

void StSamplePlayer::updateLFOAuxLvlByKey(sSI _key, sF32 _lvl) {
   Dbeginloopvoicesbykey;
   v->_setLFOAuxLvl(_lvl);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateLFOAuxFltAmt(sF32 _amt) {
   mod_lfo_aux_flt_amt = _amt;
}

void StSamplePlayer::updateLFOAuxFltAmtByKey(sSI _key, sF32 _amt) {
   Dbeginloopvoicesbykey;
   v->_setLFOAuxFltAmt(_amt);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateADSRFreqSpd(sF32 _spd) {
   // // Dbeginloopallvoices;
   // // v->_setADSRFreqSpd(_spd);
   // // Dendloopallvoices;
   mod_adsr_freq_spd = _spd;
}

void StSamplePlayer::updateADSRFreqSpdByKey(sSI _key, sF32 _spd) {
   Dbeginloopvoicesbykey;
   v->_setADSRFreqSpd(_spd);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateADSRFreqLvl(sF32 _lvl) {
   // // Dbeginloopallvoices;
   // // v->_setADSRFreqLvl(_lvl);
   // // Dendloopallvoices;
   mod_adsr_freq_lvl = _lvl;
}

void StSamplePlayer::updateADSRFreqLvlByKey(sSI _key, sF32 _lvl) {
   Dbeginloopvoicesbykey;
   v->_setADSRFreqLvl(_lvl);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateADSRFreqInt(sF32 _int) {
   // // Dbeginloopallvoices;
   // // v->_setADSRFreqInt(_int);
   // // Dendloopallvoices;
   mod_adsr_freq_int = _int;
}

void StSamplePlayer::updateADSRFreqIntByKey(sSI _key, sF32 _int) {
   Dbeginloopvoicesbykey;
   v->_setADSRFreqInt(_int);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateADSRVolSpd(sF32 _spd) {
   // // Dbeginloopallvoices;
   // // v->_setADSRVolSpd(_spd);
   // // Dendloopallvoices;
   mod_adsr_vol_spd = _spd;
}

void StSamplePlayer::updateADSRVolSpdByKey(sSI _key, sF32 _spd) {
   Dbeginloopvoicesbykey;
   v->_setADSRVolSpd(_spd);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateADSRVolLvl(sF32 _lvl) {
   // // Dbeginloopallvoices;
   // // v->_setADSRVolLvl(_lvl);
   // // Dendloopallvoices;
   mod_adsr_vol_lvl = _lvl;
}

void StSamplePlayer::updateADSRVolLvlByKey(sSI _key, sF32 _lvl) {
   Dbeginloopvoicesbykey;
   v->_setADSRVolLvl(_lvl);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateADSRVolInt(sF32 _int) {
   // // Dbeginloopallvoices;
   // // v->_setADSRVolInt(_int);
   // // Dendloopallvoices;
   mod_adsr_vol_int = _int;
}

void StSamplePlayer::updateADSRVolIntByKey(sSI _key, sF32 _int) {
   Dbeginloopvoicesbykey;
   v->_setADSRVolInt(_int);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateADSRPanSpd(sF32 _spd) {
   // // Dbeginloopallvoices;
   // // v->_setADSRPanSpd(_spd);
   // // Dendloopallvoices;
   mod_adsr_pan_spd = _spd;
}

void StSamplePlayer::updateADSRPanSpdByKey(sSI _key, sF32 _spd) {
   Dbeginloopvoicesbykey;
   v->_setADSRPanSpd(_spd);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateADSRPanLvl(sF32 _lvl) {
   // // Dbeginloopallvoices;
   // // v->_setADSRPanLvl(_lvl);
   // // Dendloopallvoices;
   mod_adsr_pan_lvl = _lvl;
}

void StSamplePlayer::updateADSRPanLvlByKey(sSI _key, sF32 _lvl) {
   Dbeginloopvoicesbykey;
   v->_setADSRPanLvl(_lvl);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateADSRPanInt(sF32 _int) {
   // // Dbeginloopallvoices;
   // // v->_setADSRPanInt(_int);
   // // Dendloopallvoices;
   mod_adsr_pan_int = _int;
}

void StSamplePlayer::updateADSRPanIntByKey(sSI _key, sF32 _int) {
   Dbeginloopvoicesbykey;
   v->_setADSRPanInt(_int);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateADSRAuxSpd(sF32 _spd) {
   // // Dbeginloopallvoices;
   // // v->_setADSRAuxSpd(_spd);
   // // Dendloopallvoices;
   mod_adsr_aux_spd = _spd;
}

void StSamplePlayer::updateADSRAuxSpdByKey(sSI _key, sF32 _spd) {
   Dbeginloopvoicesbykey;
   v->_setADSRAuxSpd(_spd);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateADSRAuxLvl(sF32 _lvl) {
   // // Dbeginloopallvoices;
   // // v->_setADSRAuxLvl(_lvl);
   // // Dendloopallvoices;
   mod_adsr_aux_lvl = _lvl;
}

void StSamplePlayer::updateADSRAuxLvlByKey(sSI _key, sF32 _lvl) {
   Dbeginloopvoicesbykey;
   v->_setADSRAuxLvl(_lvl);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateADSRAuxInt(sF32 _int) {
   // // Dbeginloopallvoices;
   // // v->_setADSRAuxInt(_int);
   // // Dendloopallvoices;
   mod_adsr_aux_int = _int;
}

void StSamplePlayer::updateADSRAuxIntByKey(sSI _key, sF32 _int) {
   Dbeginloopvoicesbykey;
   v->_setADSRAuxInt(_int);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateJumpToLoop(sSI _idx) {
   Dbeginloopallvoices;
   v->_setJumpToLoop(_idx);
   Dendloopallvoices;
}

void StSamplePlayer::updateJumpToLoopByKey(sSI _key, sSI _idx) {
   Dbeginloopvoicesbykey;
   v->_setJumpToLoop(_idx);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateJumpToLoopRel(sF32 _relIdx) {
   Dbeginloopallvoices;
   v->_setJumpToLoopRel(_relIdx);
   Dendloopallvoices;
}

void StSamplePlayer::updateJumpToLoopRelByKey(sSI _key, sF32 _relIdx) {
   Dbeginloopvoicesbykey;
   v->_setJumpToLoopRel(_relIdx);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateWavepathIndex(sSI _idx) {
   Dbeginloopallvoices;
   v->_setWavepathIndex(_idx);
   Dendloopallvoices;
}

void StSamplePlayer::updateWavepathIndexByKey(sSI _key, sSI _idx) {
   Dbeginloopvoicesbykey;
   v->_setWavepathIndex(_idx);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateAFreqSpd(sF32 _spd) {
   // // Dbeginloopallvoices;
   // // v->_setAFreqSpd(_spd);
   // // Dendloopallvoices;
   mod_adsr_freq_aspd = _spd;
}

void StSamplePlayer::updateAFreqSpdByKey(sSI _key, sF32 _spd) {
   Dbeginloopvoicesbykey;
   v->_setAFreqSpd(_spd);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateSFreqSpd(sF32 _spd) {
   // // Dbeginloopallvoices;
   // // v->_setSFreqSpd(_spd);
   // // Dendloopallvoices;
   mod_adsr_freq_sspd = _spd;
}

void StSamplePlayer::updateSFreqSpdByKey(sSI _key, sF32 _spd) {
   Dbeginloopvoicesbykey;
   v->_setSFreqSpd(_spd);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateRFreqSpd(sF32 _spd) {
   // // Dbeginloopallvoices;
   // // v->_setRFreqSpd(_spd);
   // // Dendloopallvoices;
   mod_adsr_freq_rspd = _spd;
}

void StSamplePlayer::updateRFreqSpdByKey(sSI _key, sF32 _spd) {
   Dbeginloopvoicesbykey;
   v->_setRFreqSpd(_spd);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateAVolSpd(sF32 _spd) {
   // // Dbeginloopallvoices;
   // // v->_setAVolSpd(_spd);
   // // Dendloopallvoices;
   mod_adsr_vol_aspd = _spd;
}

void StSamplePlayer::updateAVolSpdByKey(sSI _key, sF32 _spd) {
   Dbeginloopvoicesbykey;
   v->_setAVolSpd(_spd);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateSVolSpd(sF32 _spd) {
   // // Dbeginloopallvoices;
   // // v->_setSVolSpd(_spd);
   // // Dendloopallvoices;
   mod_adsr_vol_sspd = _spd;
}

void StSamplePlayer::updateSVolSpdByKey(sSI _key, sF32 _spd) {
   Dbeginloopvoicesbykey;
   v->_setSVolSpd(_spd);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateRVolSpd(sF32 _spd) {
   // // Dbeginloopallvoices;
   // // v->_setRVolSpd(_spd);
   // // Dendloopallvoices;
   mod_adsr_vol_rspd = _spd;
}

void StSamplePlayer::updateRVolSpdByKey(sSI _key, sF32 _spd) {
   Dbeginloopvoicesbykey;
   v->_setRVolSpd(_spd);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateAPanSpd(sF32 _spd) {
   // // Dbeginloopallvoices;
   // // v->_setAPanSpd(_spd);
   // // Dendloopallvoices;
   mod_adsr_pan_aspd = _spd;
}

void StSamplePlayer::updateAPanSpdByKey(sSI _key, sF32 _spd) {
   Dbeginloopvoicesbykey;
   v->_setAPanSpd(_spd);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateSPanSpd(sF32 _spd) {
   // // Dbeginloopallvoices;
   // // v->_setSPanSpd(_spd);
   // // Dendloopallvoices;
   mod_adsr_pan_sspd = _spd;
}

void StSamplePlayer::updateSPanSpdByKey(sSI _key, sF32 _spd) {
   Dbeginloopvoicesbykey;
   v->_setSPanSpd(_spd);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateRPanSpd(sF32 _spd) {
   // // Dbeginloopallvoices;
   // // v->_setRPanSpd(_spd);
   // // Dendloopallvoices;
   mod_adsr_pan_rspd = _spd;
}

void StSamplePlayer::updateRPanSpdByKey(sSI _key, sF32 _spd) {
   Dbeginloopvoicesbykey;
   v->_setRPanSpd(_spd);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateAAuxSpd(sF32 _spd) {
   // // Dbeginloopallvoices;
   // // v->_setAAuxSpd(_spd);
   // // Dendloopallvoices;
   mod_adsr_aux_aspd = _spd;
}

void StSamplePlayer::updateAAuxSpdByKey(sSI _key, sF32 _spd) {
   Dbeginloopvoicesbykey;
   v->_setAAuxSpd(_spd);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateSAuxSpd(sF32 _spd) {
   // // Dbeginloopallvoices;
   // // v->_setSAuxSpd(_spd);
   // // Dendloopallvoices;
   mod_adsr_aux_sspd = _spd;
}

void StSamplePlayer::updateSAuxSpdByKey(sSI _key, sF32 _spd) {
   Dbeginloopvoicesbykey;
   v->_setSAuxSpd(_spd);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateRAuxSpd(sF32 _spd) {
   // // Dbeginloopallvoices;
   // // v->_setRAuxSpd(_spd);
   // // Dendloopallvoices;
   mod_adsr_aux_rspd = _spd;
}

void StSamplePlayer::updateRAuxSpdByKey(sSI _key, sF32 _spd) {
   Dbeginloopvoicesbykey;
   v->_setRAuxSpd(_spd);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateGlideSwitch(sBool _bEnable) {
   Dbeginloopallvoices;
   v->_setGlideSwitch(_bEnable);
   Dendloopallvoices;
   b_glide_switch = _bEnable;
}

void StSamplePlayer::updateGlideSwitchByKey(sSI _key, sBool _bEnable) {
   Dbeginloopvoicesbykey;
   v->_setGlideSwitch(_bEnable);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateGlideSpeed(sF32 _speed) {
   Dbeginloopallvoices;
   v->_setGlideSpeed(_speed);
   Dendloopallvoices;
}

void StSamplePlayer::updateGlideSpeedByKey(sSI _key, sF32 _speed) {
   Dbeginloopvoicesbykey;
   v->_setGlideSpeed(_speed);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateFltCutOff(sF32 _f) {
   // // Dbeginloopallvoices;
   // // v->_setFilterCutOff(_f);
   // // Dendloopallvoices;
   mod_filter_cutoff = _f;
}

void StSamplePlayer::updateFltCutOffByKey(sSI _key, sF32 _f) {
   Dbeginloopvoicesbykey;
   v->_setFilterCutOff(_f);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateFltPan(sF32 _f) {
   // // Dbeginloopallvoices;
   // // v->_setFilterPan(_f);
   // // Dendloopallvoices;
   mod_filter_pan = _f;
}

void StSamplePlayer::updateFltPanByKey(sSI _key, sF32 _f) {
   Dbeginloopvoicesbykey;
   v->_setFilterPan(_f);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateFltOffset(sF32 _f) {
   // // Dbeginloopallvoices;
   // // v->_setFilterOffset(_f);
   // // Dendloopallvoices;
   mod_filter_offset = _f;
}

void StSamplePlayer::updateFltOffsetByKey(sSI _key, sF32 _f) {
   Dbeginloopvoicesbykey;
   v->_setFilterOffset(_f);
   Dendloopvoicesbykey;
}

void StSamplePlayer::updateFltRes(sF32 _f) {
   // // Dbeginloopallvoices;
   // // v->_setFilterResonance(_f);
   // // Dendloopallvoices;
   mod_filter_resonance = _f;
}

void StSamplePlayer::updateFltResByKey(sSI _key, sF32 _f) {
   Dbeginloopvoicesbykey;
   v->_setFilterResonance(_f);
   Dendloopvoicesbykey;
}

void StSamplePlayer::resetLFOFreqPhase(void) {
   Dbeginloopallvoices;
   v->_resetLFOFreqPhase();
   Dendloopallvoices;
}

void StSamplePlayer::resetLFOFreqPhaseByKey(sSI _key) {
   Dbeginloopvoicesbykey;
   v->_resetLFOFreqPhase();
   Dendloopvoicesbykey;
}

void StSamplePlayer::resetLFOVolPhase(void) {
   Dbeginloopallvoices;
   v->_resetLFOVolPhase();
   Dendloopallvoices;
}

void StSamplePlayer::resetLFOVolPhaseByKey(sSI _key) {
   Dbeginloopvoicesbykey;
   v->_resetLFOVolPhase();
   Dendloopvoicesbykey;
}

void StSamplePlayer::resetLFOPanPhase(void) {
   Dbeginloopallvoices;
   v->_resetLFOPanPhase();
   Dendloopallvoices;
}

void StSamplePlayer::resetLFOPanPhaseByKey(sSI _key) {
   Dbeginloopvoicesbykey;
   v->_resetLFOPanPhase();
   Dendloopvoicesbykey;
}

void StSamplePlayer::resetLFOAuxPhase(void) {
   Dbeginloopallvoices;
   v->_resetLFOAuxPhase();
   Dendloopallvoices;
}

void StSamplePlayer::resetLFOAuxPhaseByKey(sSI _key) {
   Dbeginloopvoicesbykey;
   v->_resetLFOAuxPhase();
   Dendloopvoicesbykey;
}

void StSamplePlayer::updatePerfCtl(sUI _idx, sF32 _val) {
   if(_idx < STSAMPLEPLAYER_NUM_PERFCTL)
   {
      perf_ctl[_idx] = _val;
   }
}

sF32 StSamplePlayer::updatePerfCtlMSB7(sUI _idx, sUI _val7) {
   if(_idx < STSAMPLEPLAYER_NUM_PERFCTL)
   {
      sUI tmp = sUI((16383.0f * perf_ctl[_idx]) / 127.0f) & 127u;
      perf_ctl[_idx] = (tmp | (_val7 << 7)) / (16383.0f/127.0f);
      return perf_ctl[_idx];
   }
   return 0.0f;
}

sF32 StSamplePlayer::updatePerfCtlLSB7(sUI _idx, sUI _val7) {
   if(_idx < STSAMPLEPLAYER_NUM_PERFCTL)
   {
      sUI tmp = sUI((16383.0f * perf_ctl[_idx]) / 127.0f) & (127u << 7);
      perf_ctl[_idx] = (tmp | _val7) / (16383.0f/127.0f);
      return perf_ctl[_idx];
   }
   return 0.0f;
}

void StSamplePlayer::updatePerfCtlByKey(sSI _key, sUI _idx, sF32 _val) {
   if(STSAMPLEPLAYER_PERFCTL_PRESSURE == _idx)
   {
      Dbeginloopvoicesbykey;
      v->_updatePerfCtlPolyPressure(_val);
      Dendloopvoicesbykey;
   }
}

void StSamplePlayer::updatePerfCtls(YAC_Object *_initialPerfCtl) {
   if(YAC_Is_FloatArray(_initialPerfCtl))
   {
      YAC_CAST_ARG(YAC_FloatArray, initialPerfCtl, _initialPerfCtl);
      sUI numCtl = sMIN(STSAMPLEPLAYER_NUM_PERFCTL, initialPerfCtl->num_elements);
      for(sUI ctlIdx = 0u; ctlIdx < numCtl; ctlIdx++)
      {
         sF32 val = initialPerfCtl->elements[ctlIdx];
         if(val >= 0.0f)
            updatePerfCtl(ctlIdx, val);
      }
   }
}

void StSamplePlayer::setSustainPedal(sBool _bPressed) {
   sustain_pedal = _bPressed;
}

sBool StSamplePlayer::getSustainPedal(void) {
   return sustain_pedal;
}

void StSamplePlayer::setSoftPedal(sBool _bPressed) {
   soft_pedal = _bPressed;
}

sBool StSamplePlayer::getSoftPedal(void) {
   return soft_pedal;
}

sUI StSamplePlayer::getLastVoiceKey(void) {
   sUI lastVoiceKey = next_voice_key - 1u;
   return lastVoiceKey;
}

void StSamplePlayer::findUniqueSamplesByVoiceKey(YAC_Object *_retSamplesPA, sUI _voiceKey) {
   if(YAC_Is_PointerArray(_retSamplesPA))
   {
      YAC_CAST_ARG(YAC_PointerArray, ret, _retSamplesPA);
      Dbeginloopallvoices;
      if(v->voice_key == _voiceKey)
      {
         if(NULL != v->sample)
         {
            // Is unique ?
            sUI idx = 0u;
            for(; idx < ret->num_elements; idx++)
            {
               if(ret->elements[idx].value.object_val == v->sample)
                  break;
            }
            if(idx == ret->num_elements)
            {
               // Add unique sample
               YAC_Value t;
               t.initObject(v->sample, 0);
               ret->yacArraySet(NULL/*context*/, ret->num_elements, &t);
            }
         }
      }
      Dendloopallvoices;
   }
}

void StSamplePlayer::setTempo(sF32 _bpm, sSI _ppq) {
   if(_bpm < 1.0f)
      _bpm = 1.0f;
   if(_ppq < 24)
      _ppq = 24;
   else if(_ppq > 768)
      _ppq = 768;
   bpm = _bpm;
   ppq = _ppq;
   bpm_audiotick_scl = ((1000.0f * 60.0f) / (_bpm * _ppq));
}

#ifndef LIBSYNERGY_BUILD
void StSamplePlayer::stopOtherLiveRecordingsExceptFor(StSample *_c) {
   StSample *o = last_started_samplebank->first_sample;
   while(NULL != o)
   {
      if(o != _c)
      {
         if(o->b_liverec && (o->liverec_input_idx == _c->liverec_input_idx))
         {
            // Don't let more than one zone record the same input
            o->_stopLiveRecording();
         }
      }
      o = o->next;
   }
}
#endif // LIBSYNERGY_BUILD

void StSamplePlayer::startLiveRecording(sUI _zoneMaskOrIndex, sBool _bIndex, sBool _bRestart) {
#ifndef LIBSYNERGY_BUILD
//    Dyac_host_printf("xxx StSamplePlayer::startLiveRecording: zoneMaskOrIndex=%u bIndex=%d bRestart=%d last_started_sampleba
// nk=%p\n", _zoneMaskOrIndex, _bIndex, _bRestart, last_started_samplebank);
   if(NULL != last_started_samplebank)
   {
      sUI i = 0u;
      StSample *c = last_started_samplebank->first_sample;
      while(NULL != c)
      {
         sBool bMatch;
         if(_bIndex)
            bMatch = (i == _zoneMaskOrIndex);
         else
            bMatch = (0u != (_zoneMaskOrIndex & (1u << i)));
         if(bMatch)
         {
            // Dyac_host_printf("xxx sp::startLiveRecording bMatch=true c->b_liverec=%d active=%d\n", c->b_liverec, c->b_liverec_active);
            if(c->b_liverec && (!c->b_liverec_active || _bRestart))
            {
               // Dyac_host_printf("xxx sp::startLiveRecording: call c->startLiveRecording()\n");
               stopOtherLiveRecordingsExceptFor(c);
               c->_startLiveRecording();
               if(_bIndex)
                  break;
            }
         }

         // Next sample / zone
         c = c->next;
         i++;
      }
   }
#else
   (void)_zoneMaskOrIndex;
   (void)_bIndex;
   (void)_bRestart;
#endif // LIBSYNERGY_BUILD
}

void StSamplePlayer::stopLiveRecording(sUI _zoneMaskOrIndex, sBool _bIndex) {
#ifndef LIBSYNERGY_BUILD
   if(NULL != last_started_samplebank)
   {
      sUI i = 0u;
      StSample *c = last_started_samplebank->first_sample;
      while(NULL != c)
      {
         sBool bMatch;
         if(_bIndex)
            bMatch = (i == _zoneMaskOrIndex);
         else
            bMatch = (0u != (_zoneMaskOrIndex & (1u << i)));

         if(bMatch)
         {
            c->_stopLiveRecording();
         }

         // Next sample / zone
         c = c->next;
         i++;
      }
   }
#else
   (void)_zoneMaskOrIndex;
   (void)_bIndex;
#endif // LIBSYNERGY_BUILD
}

#ifndef LIBSYNERGY_BUILD
void StSamplePlayer::mtxLazyInitProcessTickNr(void) {
   if(NULL == mtx_process_tick_nr)
   {
      mtx_process_tick_nr = yac_host->yacMutexFindByName("stsampleplayer_process_tick_nr");
   }
}

void StSamplePlayer::mtxLockProcessTickNr(void) {
   mtxLazyInitProcessTickNr();
   if(NULL != mtx_process_tick_nr)
   {
      yac_host->yacMutexLock(mtx_process_tick_nr);
   }
}

void StSamplePlayer::mtxUnlockProcessTickNr(void) {
   mtxLazyInitProcessTickNr();
   if(NULL != mtx_process_tick_nr)
   {
      yac_host->yacMutexLock(mtx_process_tick_nr);
   }
}
#endif // LIBSYNERGY_BUILD

#ifndef LIBSYNERGY_BUILD
void StSamplePlayer::handleLiveRecording(sF32 *_out, const sF32*const*_inputs, sUI _numFrames, sUI _processTickNr) {
   if(NULL != last_started_samplebank)
   {
      mtxLockProcessTickNr();
      if(last_started_samplebank->liverec_last_process_tick_nr != _processTickNr)
      {
         last_started_samplebank->liverec_last_process_tick_nr = _processTickNr;
         mtxUnlockProcessTickNr();

         // Dyac_host_printf("xxx StSamplePlayer::handleLiveRecording: numFrames=%u loopShift=%d\n", _numFrames, mod_liverec_loop_shift);
         // sUI i = 0u;
         StSample *c = last_started_samplebank->first_sample;
         while(NULL != c)
         {
            c->handleLiveRecording(_out, _inputs, _numFrames, mod_liverec_loop_shift);

            // Next sample / zone
            c = c->next;
            // i++;
         }
      }
      else
      {
         mtxUnlockProcessTickNr();
      }
   }
}
#endif // LIBSYNERGY_BUILD

void StSamplePlayer::setLiveRecLoopShift(sSI _shift) {
#ifndef LIBSYNERGY_BUILD
   // Dyac_host_printf("xxx StSamplePlayer::setLiveRecLoopShift(%d)\n", _shift);
   mod_liverec_loop_shift = _shift;
#else
   (void)_shift;
#endif // LIBSYNERGY_BUILD
}

void StSamplePlayer::setLiveRecSampleBank(YAC_Object *_sampleBank) {
   if(YAC_CHK(_sampleBank, clid_StSampleBank))
   {
      last_started_samplebank = (StSampleBank*)_sampleBank;
   }
   else
   {
      last_started_samplebank = NULL;
   }
}

void StSamplePlayer::setLiveRecDoubleBufferIndex(sUI _idx) {
   if(NULL != last_started_samplebank)
   {
      // sUI i = 0u;
      StSample *c = last_started_samplebank->first_sample;
      while(NULL != c)
      {
         if(c->b_liverec && (STSAMPLE_LIVEREC_DBLBUF_MODE_PARAM == c->liverec_doublebuffer_mode))
         {
            c->liverec_doublebuffer_rec_idx = _idx & 1u;
         }

         // Next sample / zone
         c = c->next;
         // i++;
      }
   }
}

sBool StSamplePlayer::uiCheckResetAnyRedrawFlag(void) {
   sBool r = YAC_FALSE;
   if(NULL != last_started_samplebank)
   {
      // Dyac_host_printf("xxx StSamplePlayer::handleLiveRecording: numFrames=%u\n", _numFrames);
      StSample *c = last_started_samplebank->first_sample;
      while(NULL != c)
      {
         r |= c->uiCheckResetRedrawFlag();

         // Next sample / zone
         c = c->next;
      }
   }
   return r;
}

void StSamplePlayer::unloadVoicePlugins(void) {
   resetVoices();

   for(sUI i = 0u; i < num_voices; i++)
   {
      StSampleVoice *v = voices[i];
      for(sUI pluginIdx = 0u; pluginIdx < STSAMPLE_NUM_PLUGINS; pluginIdx++)
      {
         v->plugin_cache_entries[pluginIdx] = NULL;
      }
   }

   freePluginCache();
}

void StSamplePlayer::handleReorderVoicePlugins(YAC_Object *_sb, YAC_Object *_ia) {
   if(YAC_CHK(_sb, clid_StSampleBank))
   {
      if(YAC_Is_IntArray(_ia))
      {
         YAC_CAST_ARG(YAC_IntArray, ia, _ia);
         if(ia->num_elements >= STSAMPLE_NUM_PLUGINS)
         {
            const sUI *iaMap = (sUI*)ia->elements;

            for(sUI i = 0u; i < num_voices; i++)
            {
               StSampleVoice *v = voices[i];

               if(NULL != v->sample)
               {
                  if((void*)v->sample->parent_samplebank == (void*)_sb)
                  {
                     v->handleReorderVoicePlugins(iaMap);
                  }
               }
            }
         }
      }
   }
}

void StSamplePlayer::setLastStartedSampleBankHint(YAC_Object *_sb) {
   if(YAC_CHK(_sb, clid_StSampleBank))
   {
      last_started_samplebank = (StSampleBank*)_sb;
   }
   else
   {
      last_started_samplebank = NULL;
   }
}

YAC_Object *StSamplePlayer::getLastStartedSampleBankHint(void) {
   return last_started_samplebank;
}
