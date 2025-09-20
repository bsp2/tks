// ----
// ---- file   : StSampleVoice.cpp
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
// ----          19Jan2010, 21Jan2010, 27Jan2010, 28Feb2010, 01Mar2010, 12Apr2010, 29Apr2010
// ----          30Apr2010, 02May2010, 04May2010, 11May2010, 14May2010, 28Jun2010, 29Jun2010
// ----          07Jul2010, 08Jul2010, 04Sep2010, 05Sep2010, 09Sep2010, 05Mar2011, 10Mar2011
// ----          20Mar2011, 28Mar2011, 29Mar2011, 08May2013, 09May2013, 13Sep2015, 19Dec2018
// ----          29Dec2018, 04Jan2019, 07Jan2019, 08Jan2019, 10Jan2019, 17Jan2019, 24Mar2019
// ----          30Mar2019, 31Mar2019, 23Apr2019, 11May2019, 12May2019, 13May2019, 15May2019
// ----          16May2019, 17May2019, 18Jun2019, 06Jul2019, 12Jul2019, 13Jul2019, 21Jul2019
// ----          25Jul2019, 01Aug2019, 29Nov2019, 15Feb2020, 16Feb2020, 17Feb2020, 18Feb2020
// ----          19Feb2020, 20Feb2020, 21Feb2020, 22Feb2020, 23Feb2020, 24Feb2020, 26Feb2020
// ----          27Feb2020, 17May2020, 18May2020, 19May2020, 20May2020, 21May2020, 22May2020
// ----          23May2020, 25May2020, 28May2020, 31May2020, 01Jun2020, 06Jun2020, 08Jun2020
// ----          23Aug2020, 31Aug2020, 01Sep2020, 06Sep2020, 07Sep2020, 12Sep2020, 06Oct2020
// ----          24Apr2021, 26Apr2021, 27Apr2021, 28Apr2021, 30Apr2021, 03May2021, 20May2021
// ----          21May2021, 26Jun2021, 17Jul2021, 18Jul2021, 27Jul2021, 01Aug2021, 04Aug2021
// ----          30Aug2021, 04Oct2021, 20Oct2021, 22Oct2021, 24Oct2021, 23Nov2022, 24Nov2022
// ----          10Dec2022, 20Dec2022, 21Dec2022, 22Dec2022, 23Dec2022, 27Dec2022, 28Dec2022
// ----          29Dec2022, 30Dec2022, 31Dec2022, 05Feb2023, 09Mar2023, 13Mar2023, 03Apr2023
// ----          12Apr2023, 13May2023, 18Jul2023, 11Aug2023, 14Aug2023, 16Aug2023, 17Aug2023
// ----          07Sep2023, 14Sep2023, 17Sep2023, 18Nov2023, 09Jan2024, 10Jan2024, 13Jan2024
// ----          14Jan2024, 15Jan2024, 16Jan2024, 19Jan2024, 28Sep2024, 30Sep2024, 03Oct2024
// ----          31Oct2024, 03Nov2024, 08Nov2024, 09Nov2024, 11Dec2024, 03Jan2025, 04Jan2025
// ----          12Jan2025
// ----
// ----
// ----

#define YAC_EPSILONCOMPARE_ABS defined
#define YAC_EPSILONCOMPARE_ABS_DEFAULT defined

#include "tksampler.h"
extern sF32 stsamplevoice_bipolar_to_scale (const sF32 _t, const sF32 _div, const sF32 _mul);

//   #define DP if(0);else Dyac_host_printf
 #define DP if(1);else Dyac_host_printf

#define last_sample_l last_sample[0]
#define last_sample_r last_sample[1]

#define USE_STATS defined


#ifdef USE_STATS
static sUI stat_opt_innermix = 0;
static sUI stat_opt_lastmix  = 0;
static sUI stat_opt_generic  = 0;
#endif // USE_STATS


#define VOLRAMP_NUMSTEPS STSAMPLEVOICE_VOLRAMP_NUMSTEPS

#define NUM_ZERO_SAMPLES 256
static sF32 zero_samples[2 * NUM_ZERO_SAMPLES];
static sSI b_fill_zero_samples = YAC_TRUE;


//#define ANTICLICK_FADEOUT_TIME 512 //4096
#define ANTICLICK_FADEOUT_TIME 64

// # of samples to interpolate when modulating sample offset in granular mode
#define ANTICLICK_GRANULAR_SMPOFFINTERPOL_TIME 512


#define Dsoftstopped() (volramp_fadeout_countdown >= 0)
#define Dfadedout() (!b_playing && (volramp_fadeout_countdown < 0))


static void loc_init_lanczos_tbl(sF32 *_d, sUI _num, sSI _a) {

   float x = sF32(-_a);
   float xStep = (2.0f * _a) / sF32(_num);

   // // sUI numH = _num/2;

   sUI k = 0u;

   for(; k < _num/2; k++)
   {
      sF64 y =
         (_a * sin(sM_PI*x) * sin((sM_PI*x)/_a)) /
         ((sM_PI*sM_PI) * x*x)
         ;
      _d[k] = sF32(y);
      x += xStep;
   }

   _d[k] = 1.0f;
   x += xStep;

   for(; k < _num; k++)
   {
      sF64 y =
         (_a * sin(sM_PI*x) * sin((sM_PI*x)/_a)) /
         ((sM_PI*sM_PI) * x*x)
         ;
      _d[k] = sF32(y);
      x += xStep;
   }

   _d[k] = 0.0f;
}

sF32 StSampleVoice::lanczos_tbl_a2 [LANCZOS_TSZ_A2  + 1];
sF32 StSampleVoice::lanczos_tbl_a4 [LANCZOS_TSZ_A4  + 1];
sF32 StSampleVoice::lanczos_tbl_a8 [LANCZOS_TSZ_A8  + 1];
sF32 StSampleVoice::lanczos_tbl_a16[LANCZOS_TSZ_A16 + 1];
sF32 StSampleVoice::lanczos_tbl_a32[LANCZOS_TSZ_A32 + 1];

void StSampleVoice::InitLanczosTables(void) {
   loc_init_lanczos_tbl(lanczos_tbl_a2,  LANCZOS_TSZ_A2,   2);
   loc_init_lanczos_tbl(lanczos_tbl_a4,  LANCZOS_TSZ_A4,   4);
   loc_init_lanczos_tbl(lanczos_tbl_a8,  LANCZOS_TSZ_A8,   8);
   loc_init_lanczos_tbl(lanczos_tbl_a16, LANCZOS_TSZ_A16, 16);
   loc_init_lanczos_tbl(lanczos_tbl_a32, LANCZOS_TSZ_A32, 32);
}

sF32 StSampleVoice::additive_sin_tbl[4096];

void StSampleVoice::InitAdditiveTables(void) {

   // Sine
   {
      sF32 a = 0.0f;
      sF32 w = (sF32)((2.0f * sM_PI) / 4096.0f);
      for(sUI i = 0u; i < 4096u; i++)
      {
         additive_sin_tbl[i] = sinf(a);
         a += w;
      }
   }

}

sF32 StSampleVoice::mm_curve_lut[32*32*256];

void StSampleVoice::calc_mm_curve(sF32 _c, sUI _off) {
   // see "tks-examples/tksdl/mm_curves.tks"
   const sUI num = 256u*4u;
   sF32 t = 0.0f;
   const sF32 tStep = 1.0f / (num - 1u);
   const sF32 p1x = 0.0f;
   const sF32 p1y = 0.0f;
   const sF32 p2x = 0.5f + _c*0.5f;
   const sF32 p2y = 0.5f - _c*0.5f;
   const sF32 p3x = 1.0f;
   const sF32 p3y = 1.0f;
   for(sUI k = 0u; k < num; k++)
   {
      sF32 t12x = p1x + (p2x - p1x) * t;
      sF32 t12y = p1y + (p2y - p1y) * t;
      sF32 t23x = p2x + (p3x - p2x) * t;
      sF32 t23y = p2y + (p3y - p2y) * t;
      sF32 tx = t12x + (t23x - t12x) * t;
      sF32 ty = t12y + (t23y - t12y) * t;
      sUI i = sUI(tx * 255);
      // if(i > 255u)
      //    Dyac_host_printf("xxx calc_mm_curve: oob k=%u c=%f i=%u\n", k, _c, i);
      mm_curve_lut[_off + i] = ty;
      // if( (_off == (563 * 256)) )
      //    Dyac_host_printf("xxx calc_mm_curve: k=%u i=%u t=%f tx=%f ty=%f\n", k, i, t, tx, ty);
      t += tStep;
   }
   mm_curve_lut[_off]        = 0.0f;
   mm_curve_lut[_off + 255u] = 1.0f;
}

void StSampleVoice::InitMMCurveLUT(void) {
   sUI off = 0u;
   sF32 c =  1.0f;
   sF32 cStep = 2.0f / (32*32);
   for(sUI i = 0u; i < (32u*32u); i++)
   {
      calc_mm_curve(c, off);
      off += 256u;
      c -= cStep;
   }
}

sF32 StSampleVoice::FracSmooth(sF32 _s, sU8 _order) {
   sF32 t;
   switch(_order & 7u)
   {
      default:
      case 0:
         t = _s;
         break;

      case 1:
         t = _s * _s * (3.0f - 2.0f * _s);
         break;

      case 2:
      {
         sF32 s2 = _s * _s;
         sF32 s3 = s2 * _s;
         t = s3 * (6.0f*s2 - 15.0f*_s + 10.0f);
      }
      break;

      case 3:
      {
         sF32 s2 = _s * _s;
         sF32 s3 = s2 * _s;
         sF32 s4 = s2 * s2;
         t = s4 * (-20.0f*s3 + 70.0f*s2 - 84.0f*_s + 35.0f);
      }
      break;

      case 4:
      {
         sF32 s2 = _s * _s;
         sF32 s3 = s2 * _s;
         sF32 s4 = s2 * s2;
         sF32 s5 = s2 * s3;
         t = s5 * (70.0f*s4 - 315.0f*s3 + 540.0f*s2 - 420.0f*_s + 126.0f);
      }
      break;

      case 5:
      {
         sF32 s2 = _s * _s;
         sF32 s3 = s2 * _s;
         sF32 s4 = s2 * s2;
         sF32 s5 = s2 * s3;
         sF32 s6 = s5 * _s;
         t = s6 * (-252.0f*s5 + 1386.0f*s4 - 3080.0f*s3 + 3465.0f*s2 - 1980.0f*_s + 462.0f);
      }
      break;

      case 6:
      case 7: // (todo)
      {
         sF32 s2 = _s * _s;
         sF32 s3 = s2 * _s;
         sF32 s4 = s2 * s2;
         sF32 s5 = s2 * s3;
         sF32 s6 = s5 * _s;
         sF32 s7 = s4 * s3;
         t = s7 * (924.0f*s6 - 6006.0f*s5 + 16380.0f*s4 - 24024.0f*s3 + 20020.0f*s2 - 9009.0f*_s + 1716.0f);
      }
      break;
   }

   return t;
}

StSampleVoice::StSampleVoice(void) {
   lfsr.init((void*)this);

   freq_table    = NULL;
   sample_player = NULL;

   override_sample_loops = NULL;
   override_smpdat = NULL;
   override_smpdat_sz = 0u;
   b_override_smpdat_copied = YAC_FALSE;

   memset(plugin_cache_entries, 0, sizeof(plugin_cache_entries));

   memset((void*)partial_phases, 0, sizeof(partial_phases));
   memset((void*)partial_speeds, 0, sizeof(partial_speeds));

   additive_tbl = NULL;

   adsr_vol.min_level = 0.0f;

   _resetVoice();
}

StSampleVoice::~StSampleVoice() {
   YAC_DELETE_SAFE(override_sample_loops);

   if(NULL != override_smpdat)
   {
      delete [] override_smpdat;
      override_smpdat = NULL;
      override_smpdat_sz = 0u;
   }

   if(NULL != additive_tbl)
   {
      delete [] additive_tbl;
      additive_tbl = NULL;
   }
}

void StSampleVoice::lazyAllocOverrideSmpDat(sUI _sz) {
   if((NULL == override_smpdat) || (override_smpdat_sz != _sz))
   {
      override_smpdat = new(std::nothrow)sF32[_sz];
      if(NULL != override_smpdat)
      {
         override_smpdat_sz = _sz;
#if 1
         ::memset((void*)override_smpdat, 0, sizeof(sF32)*_sz);
#endif
      }
      else
         override_smpdat_sz = 0u;
   }
}

const sF32 *StSampleVoice::getCurrentFreqTable(void) const {
   const sF32 *ret = NULL;

   if(NULL == ret)
      ret = freq_table;

   if(NULL == ret/* && NULL != sample*/)
   {
      ret = sample->getCurrentTuningTableOrNull();

      if(NULL == ret && NULL != sample->parent_samplebank)
         ret = sample->parent_samplebank->getCurrentTuningTableOrNull();
   }

   if(NULL == ret)
      ret = sample_player->default_freq_table->elements;

   return ret;
}

const sF32 *StSampleVoice::getCurrentFreqTableMM(void) const {
   if(sample->forced_tuning_table_idx < 0 &&
      ((NULL == sample->parent_samplebank || sample->parent_samplebank->forced_tuning_table_idx < 0))
      )
   {
      if(mmdst.tuning_tbl_rel >= 0.0f)
      {
         // updated in modmatrix.cpp
         return interpolated_freq_table;
      }
      else if(mmdst.tuning_tbl_abs > 0.0f)
      {
         // e.g. CC 0..127 => MM.srcValDef=0..1 => 0..127,
         // i.e. CC val 1 selects table nr 1, 16 selects table nr 16, ..
         sUI tblIdx = sUI(mmdst.tuning_tbl_abs * 127.0f + 0.5f);
         if(tblIdx >= STSAMPLE_MAX_TUNING_TABLES)
            tblIdx = STSAMPLE_MAX_TUNING_TABLES - 1u;
         if(tblIdx > 0u)
         {
            tblIdx--;
            if(NULL != sample->tuning_tables[tblIdx])
               return sample->tuning_tables[tblIdx];

            if(NULL != sample->parent_samplebank && NULL != sample->parent_samplebank->tuning_tables[tblIdx])
               return sample->parent_samplebank->tuning_tables[tblIdx];
         }
      }
   }
   return getCurrentFreqTable();
}

void StSampleVoice::_resetVoice(void) {
   // // Dyac_host_printf("xxx StSampleVoice::_resetVoice\n");
   sample                      = NULL;
   b_used                      = YAC_FALSE;
   b_playing                   = YAC_FALSE;
   b_trig_pulse                = YAC_FALSE;
   b_release                   = YAC_TRUE;
   b_release_pulse             = YAC_FALSE;
   b_alloc                     = YAC_FALSE;
   b_glide                     = YAC_FALSE;
   b_allow_smpoff              = YAC_TRUE;
   replay_ticks                = 0;
   voice_key                   = 0;
   last_voice_key              = 0;
   voice_alloc_idx             = 0u;
   b_fading_out                = YAC_FALSE;
   queued_noteon.b_valid       = 0;
   volramp_fade_vol            = 1.0f;
   volramp_fadein_countdown    = -1;
   volramp_fadeout_countdown   = -1;
   anticlick_fadeout_countdown = 0;
   last_sample_l               = 0.0f;
   last_sample_r               = 0.0f;
   anticlick_granular_smpoffinterpol_countdown = 0;
#ifdef USE_SAMPLESHIFT_EOL_XFADE_BUF
   sampleshift_eol_xfade_loopstart_frameidx = 0u;
   // (note) clearing the buffer should not be necessary (just a precaution)
   memset((void*)sampleshift_eol_xfade_buf_l, 0, sizeof(sampleshift_eol_xfade_buf_l));
   memset((void*)sampleshift_eol_xfade_buf_r, 0, sizeof(sampleshift_eol_xfade_buf_r));
#endif // USE_SAMPLESHIFT_EOL_XFADE_BUF

   float_block_off       = 0.0f;
   float_block_size      = 0.0f;
   current_block_size    = 0u;
   current_block_offset  = 0u;
   last_relsmpoff        = 0;
   current_sample_offset = 0u;
   current_sample_len    = 0u;
   current_clipend_off   = 0u;
   current_smpdat_shift  = 0;
   samples_until_end_of_loopstep = 0;
   fadein_countdown      = 0;
   fadeout_countdown     = 0;
   b_fading_out          = YAC_FALSE;
   current_ts_offset     = 0.0f;

   current_play_offset    = 0.0;
   play_offset_orig_speed = 0.0;
   play_offset_actual     = 0.0;
   b_queued_delayed_loop  = YAC_FALSE;
   smpoff_mask            = 0u;
   mmdst_srr              = 0.0f;
   br_mask                = 0u;
   mmdst_br               = 0.0f;
   mmdst_sync_speed       = 0.0f;
   mmdst_sync_slew        = 1.0f;
   sync_factor_avg        = 1.0;
   mod_vol                = 1.0f;
   mod_vol2               = 1.0f;
   mod_pan                = 0.0f;
   mod_pan2               = 0.0f;
   mod_freq               = 0.0f;
   mod_freq2              = 0.0f;
   mod_mod                = 0.0f;
   mod_timestretch        = 0.0f;
   mmdst_timestretch      = 0.0f;
   mod_timestretch_bend   = 0.0f;
   mmdst_timestretch_bend = 0.0f;
   mod_sampleoff          = 0.0f;
   mod_sampleoff2         = 0.0f;  // internal (rand, vel to off)
   mod_cyclelen           = 0.0f;
   mod_sampleshift        = 0.0f;
   mod_sampleshift_endofloop = -1.0f;
   mod_altvolume          = 1.0f;
   mod_jumptoloop         = -1;
   mmdst_loop_repeats_scale = 1.0f;
   b_queued_wt2d          = YAC_FALSE;
   wt2d_x                 = 0.0f;
   wt2d_y                 = 0.0f;
   mod_sampleoff_wt2d     = 0.0f;

   mod_lfo_freq_amt    = 1.0f;
   mod_lfo_vol_amt     = 1.0f;
   mod_lfo_pan_amt     = 1.0f;
   mod_lfo_aux_flt_amt = 1.0f;

   samples_since_cycle = 0;
   fragment_interpol = 0.0f;

   current_vol      = 0.0f;
   next_vol         = 0.0f;
   cached_vel_vol   = 0.0f;
   current_pan      = 0.0f;
   next_pan         = 0.0f;
   current_freq     = 0.0f;
   next_freq        = 0.0f;
   current_aux_adsr = 0.0f;
   next_aux_adsr    = 0.0f;
   current_aux_lfo  = 0.0f;
   next_aux_lfo     = 0.0f;

   wavepath_idx        = 0;
   next_wavepath_idx   = 0;
   queued_wavepath_idx = 0;

   // // current_loop_idx      = -1;

   filter_type = STSAMPLE_FLT_TYPE_NONE;
   mod_filter_cutoff    = 0.0f;
   mod_filter_pan       = 0.0f;
   mod_filter_offset    = 0.0f;
   mod_filter_resonance = 0.0f;

   mod_glide_switch = YAC_TRUE;
   mod_glide_speed  = 1.0f;
   mmdst_glide_speed = 1.0f;

   perfctl_poly_pressure = -1.0f;
   perfctl_pressure_max = 0.0f;

   for(sUI i = 0u; i < STSAMPLE_NUM_MODSEQ; i++)
      modseq_patch[i] = 0u;

   resetBiquad();

   glide_src_note = glide_src_note_orig = 60.0f;
   glide_time = 0.0f;
   clearGlideNoteHistory();

   active_note_idx = 0;

   b_fwd = 1;

   if( (NULL == sample) || !(sample->keepInitialOscCopyOverrideBuffer() && b_override_smpdat_copied) )
   {
      b_override_smpdat_copied = YAC_FALSE;
      liverec_copy_loop_offset = -1;
      liverec_copy_loop_len = 0;
      liverec_osc_pitch_factor = 1.0f;
   }
   else
   {
      // Dyac_host_printf("xxx skip liverec osc reset, liverec_copy_loop_len=%u\n", liverec_copy_loop_len);
   }
   liverec_copy_doublebuffer_rec_idx = 0u;

   // Remove plugin cache refs / mark previous cache entries as unused
   for(sUI pluginIdx = 0u; pluginIdx < STSAMPLE_NUM_PLUGINS; pluginIdx++)
   {
      st_plugin_cache_entry_t *pce = plugin_cache_entries[pluginIdx];
      if(NULL != pce)
      {
         pce->b_used = YAC_FALSE;
         plugin_cache_entries[pluginIdx] = NULL;
      }
   }

   voice_bus_buffers = NULL;
   // // ::memset((void*)&voice_bus_buffers_self, 0, sizeof(voice_bus_buffers_self));
   for(sUI i = 0u; i < STSAMPLEVOICE_MAX_LAYERS; i++)
      voice_bus_buffers_self[i] = sp_voice_bus_null_buffer;

   last_additive_cfg = -1.0f;
   mmdst_additive_cfg = 0.0f;
   mmdst_additive_stereo_spread = 0.0f;
   mmdst_additive_num_partials = 0.0f;

   // // ::memset((void*)slew_partial_phases, 0, sizeof(slew_partial_phases));
   ::memset((void*)slew_partial_speeds, 0, sizeof(slew_partial_speeds));
   ::memset((void*)slew_partial_magnitudes, 0, sizeof(slew_partial_magnitudes));

   memset((void*)modmatrix_slewed_src_values, 0, sizeof(sF64) * STSAMPLE_NUM_MODMATRIX_ENTRIES);

   mmdst_var = 0.0f;
   mmvar_last_seen_var_idx = 0u;  // UI / editor
}

void StSampleVoice::clearGlideNoteHistory(void) {
   for(sUI idx = 0u; idx < GLIDE_NOTE_HISTORY_SZ; idx++)
      glide_note_history[idx] = -1;
   glide_note_history_idx = -1;
}

void StSampleVoice::addToGlideNoteHistory(sSI _note) {
   // Dyac_host_printf("xxx StSampleVoice::addToGlideNoteHistory: note=%d glide_note_history_idx=%d\n", _note, glide_note_history_idx);
   if(glide_note_history_idx < (GLIDE_NOTE_HISTORY_SZ-1))
   {
      // Dyac_host_printf("xxx StSampleVoice::addToGlideNoteHistory: ADD note=%d glide_note_history_idx=%d\n", _note, glide_note_history_idx);
      if(glide_note_history_idx >= 0)
      {
         sSI idx = 0;
         for(; idx <= glide_note_history_idx; idx++)
         {
            if(glide_note_history[idx] == _note)
               return;
         }
      }
      glide_note_history[++glide_note_history_idx] = _note;

      // for(sSI i = 0; i <= glide_note_history_idx; i++)
      //    Dyac_host_printf("xxx    ADD glide_note_history[%d] = %d\n", i, glide_note_history[i]);
   }
}

sBool StSampleVoice::removeFromGlideNoteHistory(sSI _note, sBool _bLastOnly) {
   sBool bChanged = YAC_FALSE;
   if(glide_note_history_idx >= 0)
   {
      sUI srcIdx = 0u;
      for(sUI idx = 0u; idx < GLIDE_NOTE_HISTORY_SZ; idx++, srcIdx++)
      {
         if(glide_note_history[idx] == _note)
         {
            if(_bLastOnly)
            {
               // Dyac_host_printf("xxx StSampleVoice::removeFromGlideNoteHistory: bLastOnly** note=%d idx=%u glide_note_history_idx=%d\n", _note, idx, glide_note_history_idx);
               if(sSI(idx) != glide_note_history_idx)
                  return YAC_FALSE;  // not reachable ?
               glide_note_history[glide_note_history_idx--] = -1;
               bChanged = true;
               break;
            }
            srcIdx++;
            bChanged = YAC_TRUE;
            glide_note_history_idx--;
         }
         if(bChanged)
         {
            if(srcIdx < GLIDE_NOTE_HISTORY_SZ)
            {
               glide_note_history[idx] = glide_note_history[srcIdx];
               // // if(-1 != glide_note_history[idx])
               // //    glide_note_history_idx = sSI(idx);
            }
            else
               glide_note_history[idx] = -1;
         }
      }
   }

   // Dyac_host_printf("xxx StSampleVoice::removeFromGlideNoteHistory: note=%d glide_note_history_idx=%d bLastOnly=%d bChanged=%d\n", _note, glide_note_history_idx, _bLastOnly, bChanged);
   // for(sSI i = 0; i <= glide_note_history_idx; i++)
   //    Dyac_host_printf("xxx    DEL glide_note_history[%d] = %d\n", i, glide_note_history[i]);

   return bChanged;
}

void StSampleVoice::setSamplePlayer(StSamplePlayer *_samplePlayer) {
   sample_player = _samplePlayer;

   lfo_freq.sp_mod_speed = &_samplePlayer->mod_lfo_freq_spd;
   lfo_freq.sp_mod_level = &_samplePlayer->mod_lfo_freq_lvl;

   lfo_vol.sp_mod_speed = &_samplePlayer->mod_lfo_vol_spd;
   lfo_vol.sp_mod_level = &_samplePlayer->mod_lfo_vol_lvl;

   lfo_pan.sp_mod_speed = &_samplePlayer->mod_lfo_pan_spd;
   lfo_pan.sp_mod_level = &_samplePlayer->mod_lfo_pan_lvl;

   lfo_aux.sp_mod_speed = &_samplePlayer->mod_lfo_aux_spd;
   lfo_aux.sp_mod_level = &_samplePlayer->mod_lfo_aux_lvl;

   adsr_freq.sp_mod_speed     = &_samplePlayer->mod_adsr_freq_spd;
   adsr_freq.sp_mod_level     = &_samplePlayer->mod_adsr_freq_lvl;
   adsr_freq.sp_mod_intensity = &_samplePlayer->mod_adsr_freq_int;
   adsr_freq.sp_mod_aspeed    = &_samplePlayer->mod_adsr_freq_aspd;
   adsr_freq.sp_mod_rspeed    = &_samplePlayer->mod_adsr_freq_sspd;
   adsr_freq.sp_mod_sspeed    = &_samplePlayer->mod_adsr_freq_rspd;

   adsr_vol.sp_mod_speed     = &_samplePlayer->mod_adsr_vol_spd;
   adsr_vol.sp_mod_level     = &_samplePlayer->mod_adsr_vol_lvl;
   adsr_vol.sp_mod_intensity = &_samplePlayer->mod_adsr_vol_int;
   adsr_vol.sp_mod_aspeed    = &_samplePlayer->mod_adsr_vol_aspd;
   adsr_vol.sp_mod_rspeed    = &_samplePlayer->mod_adsr_vol_sspd;
   adsr_vol.sp_mod_sspeed    = &_samplePlayer->mod_adsr_vol_rspd;

   adsr_pan.sp_mod_speed     = &_samplePlayer->mod_adsr_pan_spd;
   adsr_pan.sp_mod_level     = &_samplePlayer->mod_adsr_pan_lvl;
   adsr_pan.sp_mod_intensity = &_samplePlayer->mod_adsr_pan_int;
   adsr_pan.sp_mod_aspeed    = &_samplePlayer->mod_adsr_pan_aspd;
   adsr_pan.sp_mod_rspeed    = &_samplePlayer->mod_adsr_pan_sspd;
   adsr_pan.sp_mod_sspeed    = &_samplePlayer->mod_adsr_pan_rspd;

   adsr_aux.sp_mod_speed     = &_samplePlayer->mod_adsr_aux_spd;
   adsr_aux.sp_mod_level     = &_samplePlayer->mod_adsr_aux_lvl;
   adsr_aux.sp_mod_intensity = &_samplePlayer->mod_adsr_aux_int;
   adsr_aux.sp_mod_aspeed    = &_samplePlayer->mod_adsr_aux_aspd;
   adsr_aux.sp_mod_rspeed    = &_samplePlayer->mod_adsr_aux_sspd;
   adsr_aux.sp_mod_sspeed    = &_samplePlayer->mod_adsr_aux_rspd;

   for(sUI i = 0u; i < STSAMPLE_NUM_MODSEQ; i++)
   {
      modseq[i].sp_mod_speed    = &_samplePlayer->mod_modseq[i].speed;
      modseq[i].sp_mod_level    = &_samplePlayer->mod_modseq[i].level;
      modseq[i].sp_mod_numsteps = &_samplePlayer->mod_modseq[i].numsteps;
      modseq[i].sp_mod_advance  = &_samplePlayer->mod_modseq[i].advance;
   }
}

sF32 StSampleVoice::getCurrentGlideNote(void) {
   sF32 r;
   sF32 t;

   if(mod_glide_switch && !isInstantGlide())
   {
      switch(sample->glide_type)
      {
         default:
         case STSAMPLE_GLIDE_NONE:
            r = sF32(note);
            break;

         case STSAMPLE_GLIDE_TIME_LIN:
            r = glide_src_note + (note - glide_src_note) * glide_time;
            break;

         case STSAMPLE_GLIDE_TIME_EXP2:
            t = 1.0f - glide_time;
            t = t * t;
            t = 1.0f - t;
            r = glide_src_note + (note - glide_src_note) * t;
            break;

         case STSAMPLE_GLIDE_TIME_EXP3:
            t = 1.0f - glide_time;
            t = t * t * t;
            t = 1.0f - t;
            r = glide_src_note + (note - glide_src_note) * t;
            break;

         case STSAMPLE_GLIDE_TIME_EXP4:
            t = 1.0f - glide_time;
            t = t * t;
            t = t * t;
            t = 1.0f - t;
            r = glide_src_note + (note - glide_src_note) * t;
            break;

         case STSAMPLE_GLIDE_FREQ_LIN:
         case STSAMPLE_GLIDE_FREQ_EXP2:
         case STSAMPLE_GLIDE_FREQ_EXP3:
         case STSAMPLE_GLIDE_FREQ_EXP4:
            r = glide_src_note;
            break;
      }

      if(sample->b_glide_glissando)
      {
         r = sF32(sSI(r + 0.5f));
      }
   }
   else
   {
      r = sF32(note);
   }

   return r;
}

sBool StSampleVoice::isInstantGlide(void) const {
   sBool r = YAC_FALSE;

   switch(sample->glide_type)
   {
      case STSAMPLE_GLIDE_NONE:
         r = YAC_TRUE;
         break;

      case STSAMPLE_GLIDE_TIME_LIN:
      case STSAMPLE_GLIDE_TIME_EXP2:
      case STSAMPLE_GLIDE_TIME_EXP3:
      case STSAMPLE_GLIDE_TIME_EXP4:
         sF32 glideSpeed;
         if(glide_src_note > note)
         {
            // Glide down
            glideSpeed = sample->glide_speed_time_down * mod_glide_speed * mmdst_glide_speed;
         }
         else
         {
            // Glide up
            glideSpeed = sample->glide_speed_time_up * mod_glide_speed * mmdst_glide_speed;
         }
         r = (glideSpeed >= 1.0f);
         break;
   }

   return r;
}

void StSampleVoice::tickGlide(void) {

   switch(sample->glide_type)
   {
      default:
      case STSAMPLE_GLIDE_NONE:
         break;

      case STSAMPLE_GLIDE_TIME_LIN:
      case STSAMPLE_GLIDE_TIME_EXP2:
      case STSAMPLE_GLIDE_TIME_EXP3:
      case STSAMPLE_GLIDE_TIME_EXP4:
         {
            // yac_host->printf("xxx tickGlide: glide_time=%f\n", glide_time);
            sF32 glideSpeed;
            if(glide_src_note > note)
            {
               // Glide down
               glideSpeed = sample->glide_speed_time_down * mod_glide_speed * mmdst_glide_speed;
            }
            else
            {
               // Glide up
               glideSpeed = sample->glide_speed_time_up * mod_glide_speed * mmdst_glide_speed;
            }
            if(glideSpeed < 1.0f)
            {
               glide_time += glideSpeed;
               if(glide_time > 1.0f)
                  glide_time = 1.0f;
            }
            else
            {
               glide_time = 1.0f;
            }
         }
         break;

      case STSAMPLE_GLIDE_FREQ_LIN:
         if(glide_src_note > note)
         {
            // Glide down
            glide_src_note -= sample->glide_speed_freq_down * mod_glide_speed * mmdst_glide_speed;
            if(glide_src_note < sF32(note))
               glide_src_note = sF32(note);
         }
         else
         {
            // Glide up
            glide_src_note += sample->glide_speed_freq_up * mod_glide_speed * mmdst_glide_speed;
            if(glide_src_note > sF32(note))
               glide_src_note = sF32(note);
         }
         break;

      case STSAMPLE_GLIDE_FREQ_EXP2:
         if(glide_src_note > note)
         {
            // Glide down
            sF64 t = glide_src_note - sF32(note);
            t = t / (glide_src_note_orig - sF32(note));
            t = 1.0 - t;
            t = t * t;
            t = 1.0 - t;
            t *= mod_glide_speed * mmdst_glide_speed;

            glide_src_note = sF32(glide_src_note - 2.0f * sample->glide_speed_freq_down * t);
            if(glide_src_note < sF32(note))
               glide_src_note = sF32(note);
         }
         else if(glide_src_note < note)
         {
            // Glide up
            sF64 t = sF32(note) - glide_src_note;
            t = t / (sF32(note) - glide_src_note_orig);
            t = 1.0 - t;
            t = t * t;
            t = 1.0 - t;
            t *= mod_glide_speed * mmdst_glide_speed;
            glide_src_note = sF32(glide_src_note + 2.0f * sample->glide_speed_freq_up * t);
            if(glide_src_note > sF32(note))
               glide_src_note = sF32(note);
         }
         break;

      case STSAMPLE_GLIDE_FREQ_EXP3:
         if(glide_src_note > note)
         {
            // Glide down
            sF64 t = glide_src_note - sF32(note);
            t = t / (glide_src_note_orig - sF32(note));
            t = 1.0 - t;
            t = t * t * t;
            t = 1.0 - t;
            t *= mod_glide_speed * mmdst_glide_speed;
            glide_src_note = sF32(glide_src_note - 2.0f * sample->glide_speed_freq_down * t);
            if(glide_src_note < sF32(note))
               glide_src_note = sF32(note);
         }
         else if(glide_src_note < note)
         {
            // Glide up
            sF64 t = sF32(note) - glide_src_note;
            t = t / (sF32(note) - glide_src_note_orig);
            t = 1.0 - t;
            t = t * t * t;
            t = 1.0 - t;
            t *= mod_glide_speed * mmdst_glide_speed;
            glide_src_note = sF32(glide_src_note + 2.0f * sample->glide_speed_freq_up * t);
            if(glide_src_note > sF32(note))
               glide_src_note = sF32(note);
         }
         break;

      case STSAMPLE_GLIDE_FREQ_EXP4:
         if(glide_src_note > note)
         {
            // Glide down
            sF64 t = glide_src_note - sF32(note);
            t = t / (glide_src_note_orig - sF32(note));
            t = 1.0 - t;
            t = t * t;
            t = t * t;
            t = 1.0 - t;
            t *= mod_glide_speed * mmdst_glide_speed;
            glide_src_note = sF32(glide_src_note - 2.0f * sample->glide_speed_freq_down * t);
            if(glide_src_note < sF32(note))
               glide_src_note = sF32(note);
         }
         else if(glide_src_note < note)
         {
            // Glide up
            sF64 t = sF32(note) - glide_src_note;
            t = t / (sF32(note) - glide_src_note_orig);
            t = 1.0 - t;
            t = t * t;
            t = t * t;
            t = 1.0 - t;
            t *= mod_glide_speed * mmdst_glide_speed;
            glide_src_note = sF32(glide_src_note + 2.0f * sample->glide_speed_freq_up * t);
            if(glide_src_note > sF32(note))
               glide_src_note = sF32(note);
         }
         break;
   }

   // Update glide_time for MM_SRC_GLIDE_*
   switch(sample->glide_type)
   {
      case STSAMPLE_GLIDE_FREQ_LIN:
      case STSAMPLE_GLIDE_FREQ_EXP2:
      case STSAMPLE_GLIDE_FREQ_EXP3:
      case STSAMPLE_GLIDE_FREQ_EXP4:
         if(glide_src_note_orig != note)
         {
            if(glide_src_note > note)
            {
               // Glide down
               glide_time = ((glide_src_note - note) / (glide_src_note_orig - note));
            }
            else
            {
               // Glide up
               glide_time = 1.0f - ((glide_src_note - note) / (glide_src_note_orig - note));
            }
         }
         else
         {
            glide_time = 1.0f;
         }
         break;
   }
}

void StSampleVoice::resetBiquad(void) {
   biquad_a_1[0].reset();
   biquad_a_1[1].reset();
   biquad_a_2[0].reset();
   biquad_a_2[1].reset();

   biquad_b_1[0].reset();
   biquad_b_1[1].reset();
   biquad_b_2[0].reset();
   biquad_b_2[1].reset();
}

sUI StSampleVoice::getVoiceIndex(void) {
   return voice_idx;
}

sBool StSampleVoice::isAllocated(void) {
   //printf("xxx isAllocated: this=0x%p volramp_fadeout_countdown=%d\n", this, volramp_fadeout_countdown);
   return (b_playing || queued_noteon.b_valid) && (volramp_fadeout_countdown < 0);
}

sBool StSampleVoice::canAllocate(void) {
   return !queued_noteon.b_valid;// && !b_used;
}

sBool StSampleVoice::isSoftStopped(void) {
   return Dsoftstopped();
}

sBool StSampleVoice::isFadedOut(void) {
   return Dfadedout();
}

void StSampleVoice::reallyStartVoice(const StSampleVoiceNoteOnParams *_params,
                                     sBool _bDelayed
                                     ) {
   // Note: if _bDelayed is 1, the voice was delayed because of volramp fadeout.
   //       -> do not reset block size since this fxn is called within the render() loop
   //       -> assumes that _mixRate has not changed

   // Dyac_host_printf("xxx reallyStartVoice: ENTER\n");

   ////printf("xxx StSampleVoice::reallyStartVoice: this=0x%p current_vol=%f next_vol=%f\n", this, current_vol, next_vol);

   queued_noteon.b_valid = YAC_FALSE;

   // printf("xxx StSampleVoice::reallyStartVoice: note=%d vel=%f mod=%f vol=%f pan=%f freq=%f ts=%f so=%f cl=%f\n",
   //        _params->_note,
   //        _params->_vel,
   //        _params->_mod,
   //        _params->_vol,
   //        _params->_pan,
   //        _params->_freq,
   //        _params->_timestretch,
   //        _params->_sampleoff,
   //        _params->_cyclelen
   //        );

   sample       = _params->_sample;
   freq_table   = _params->_freqTable;
   b_playing    = YAC_FALSE;
   mix_rate     = _params->_mixRate;
   note         = _params->_note;
   replay_ticks = 0;

   play_offset_orig_speed = 0.0;
   play_offset_actual     = 0.0;
   sync_factor_avg        = 1.0;

   smpoff_mask = sample->smpoff_mask;
   br_mask     = sample->br_mask;

   if(!b_glide || sample->b_glide_retrig_sample)
   {
      anticlick_fadeout_countdown = 0;
      anticlick_granular_smpoffinterpol_countdown = 0;
      // // Dyac_host_printf("xxx reallyStartVoice: sample=%p wf=%p\n", sample, sample->waveform);
      sample->waveform->ui_last_played_offset = 0.0f;
      current_delay_countdown = (sUI) ((sample->sample_delay * sample->sample_delay_multiplier * _params->_mixRate) / 1000);
   }

   // Patch that prevents the samplerengine from crashing in case a waveform could not be loaded
   if(NULL == sample->waveform->sample_data)
   {
      b_used    = YAC_FALSE;
      b_playing = YAC_FALSE;
      b_release = YAC_FALSE;
      return;
   }

   sample->waveform->ui_last_started_voice = (void*)this;

   if( !(b_override_smpdat_copied && sample->keepInitialOscCopyOverrideBuffer() && (liverec_copy_loop_len > 0)) )
   {
      b_override_smpdat_copied = YAC_FALSE;
      liverec_copy_loop_offset = -1;
      liverec_copy_loop_len    = 0;
      liverec_osc_pitch_factor = 1.0f;
   }
   else
   {
      // Dyac_host_printf("xxx reallyStartVoice: skip liverec osc reset, liverec_copy_loop_len=%u pitch_factor=%f\n", liverec_copy_loop_len, liverec_osc_pitch_factor);
   }

   if(sample->b_liverec && sample->b_liverec_osc_sustain && (STSAMPLE_LIVEREC_OSC_MODE_OFF != sample->liverec_osc_mode))
   {
      const YAC_IntArray *loopsOverride = sample->getSampleLoopsOverride();
      // Dyac_host_printf("xxx sustain: loopsOverride=%p\n", loopsOverride);
      if((NULL != loopsOverride) && (loopsOverride->num_elements >= (2*3u)))
      {
         if(NULL == override_sample_loops)
            _allocOverrideSampleLoops();

         if(override_sample_loops->max_elements < loopsOverride->num_elements)
         {
            override_sample_loops->yacArrayRealloc(loopsOverride->num_elements, 0,0,0);
         }

         override_sample_loops->num_elements = loopsOverride->num_elements;

         sUI *loopDat = (sUI*)override_sample_loops->elements;

         for(sUI i = 0u; i < loopsOverride->num_elements; i++)
         {
            loopDat[i] = loopsOverride->elements[i];
            // // Dyac_host_printf("xxx loopsOverride[%u]=%u\n", i, loopDat[i]);
         }

         if(sample->b_liverec_copy)
         {
            liverec_copy_loop_offset = sSI(loopsOverride->elements[0]);
            liverec_copy_loop_len    = sSI(loopsOverride->elements[1]);

            if(0u != liverec_copy_loop_offset)
            {
               // Make loops relative to copy buffer
               for(sUI i = 0u; i < loopsOverride->num_elements; i += 3u)
               {
                  loopDat[i] -= liverec_copy_loop_offset;
               }
            }

            // Dyac_host_printf("xxx sustain+copy: copy_off=%d copy_len=%d\n", liverec_copy_loop_offset, liverec_copy_loop_len);
            liverec_copy_doublebuffer_rec_idx = sample->liverec_doublebuffer_rec_idx;
         }
      }
   }
   else
   {
      _freeOverrideSampleLoops();  // lazy-free

      if( !(b_override_smpdat_copied && sample->keepInitialOscCopyOverrideBuffer() && (liverec_copy_loop_len > 0)) )
      // // if(!sample->keepInitialOscCopyOverrideBuffer())
      {
         liverec_copy_loop_offset = sample->liverec_override_loop_offset;
         liverec_copy_loop_len    = sample->liverec_override_loop_len;
      }

      // Dyac_host_printf("xxx liverec_copy_loop_offset=%u liverec_copy_loop_len=%u\n", liverec_copy_loop_offset, liverec_copy_loop_len);

      if(sample->b_liverec && sample->b_liverec_copy)
      {
         if(STSAMPLE_LIVEREC_OSC_MODE_OFF == sample->liverec_osc_mode)
         {
            // // if(0u == sample->liverec_front_buffer_sz)
            // // {
            // //    // Lazy-start live recording (this note will not sound)
            // //    sSI smpIdx = sample->parent_samplebank->_getSampleIdx(sample);
            // //    if(smpIdx >= 0)
            // //    {
            // //       sample_player->startLiveRecording(sUI(smpIdx), YAC_TRUE/*bIndex*/, YAC_TRUE/*bRestart*/);
            // //    }
            // //    return;
            // // }

            // Oneshot mode
            if((NULL != sample->liverec_front_buffer) && (sample->liverec_front_buffer_sz > 0u))
            {
               sUI waveNumFrames = sample->waveform->_getNumFrames();
               sUI sz = waveNumFrames * sample->waveform->num_channels;
               if((NULL != sample->liverec_front_buffer) && (sample->liverec_front_buffer_sz >= sz))
               {
                  lazyAllocOverrideSmpDat(sz);
                  if(NULL != override_smpdat)
                  {
                     // // const sF32 *smpDatBase = sample->waveform->sample_data->elements;
                     ::memcpy((void*)override_smpdat,
                              sample->liverec_front_buffer,
                              sz * sizeof(sF32)
                              );
                     b_override_smpdat_copied = YAC_TRUE;
                     // Dyac_host_printf("xxx StSampleVoice::reallyStartVoice: copy sz=%u from front buffer\n");
                  }
               }
            }
         }
         else
         {
            // OSC mode
            if(liverec_copy_loop_offset < 0)
            {
               // After loading project when no waveform has been recorded, yet
               if(STSAMPLE_LIVEREC_DBLBUF_MODE_VOICE == sample->liverec_doublebuffer_mode)
               {
                  // Stuck in doublebuffer (not queued yet)
                  sample->liveRecSwapBuffers();
                  if(sample->liverec_override_loop_offset >= 0)
                  {
                     liverec_copy_loop_offset          = sample->liverec_override_loop_offset;
                     liverec_copy_loop_len             = sample->liverec_override_loop_len;
                     liverec_copy_doublebuffer_rec_idx = sample->liverec_doublebuffer_rec_idx;
                     Dyac_host_printf("[>>>] StSampleVoice::reallyStartVoice: retry initial copy_loop off=%d len=%d\n", liverec_copy_loop_offset, liverec_copy_loop_len);
                  }
               }

               if(sample->liverec_override_loop_offset < 0)
               {
#ifndef LIBSYNERGY_BUILD
                  // Lazy-start live recording (this note will not sound)
                  sSI smpIdx = sample->parent_samplebank->_getSampleIdx(sample);
                  if(smpIdx >= 0)
                  {
                     sample_player->startLiveRecording(sUI(smpIdx), YAC_TRUE/*bIndex*/, YAC_TRUE/*bRestart*/);
                  }
#endif // LIBSYNERGY_BUILD
                  return;
               }
            }

            if(STSAMPLE_LIVEREC_OSC_MODE_OFF != sample->liverec_osc_mode)
            {
               // compensate pitch when detected loop cycle len differs from ideal cycle len
               if(sample->liverec_ref_cyclelen > 0.0f)
               {
                  liverec_osc_pitch_factor = liverec_copy_loop_len / sample->liverec_ref_cyclelen;
                  // Dyac_host_printf("xxx liverec_osc_pitch_factor=%f (ref_cyclelen=%u, copy_looplen=%u)\n", liverec_osc_pitch_factor, sUI(sample->liverec_ref_cyclelen), liverec_copy_loop_len);
               }
            }
         }
      } // if b_liverec && b_liverec_copy
   }
   liverec_copy_doublebuffer_rec_idx = sample->liverec_doublebuffer_rec_idx;

   // Dyac_host_printf("xxx reallyStartVoice: copy_loop off=%d len=%d\n", liverec_copy_loop_offset, liverec_copy_loop_len);


   if(!b_glide || sample->b_glide_retrig_sample)
   {
      current_orig_sample_len = current_sample_len = sample->sample_len;  // should be 0 (== sample_loops only)
      // Dyac_host_printf("xxx reallyStartVoice: sample->sample_len=%u\n", sample->sample_len);
      current_loop_idx      = -1;
      current_loop_countdown = getCurrentSampleLoops() ? 1 : 0;
      samples_until_end_of_loopstep = 0;
      current_clipend_off = 0u;
   }

   // Set initial controllers/params
   if(!b_glide)
   {
      mod_vol                  = _params->_vol;
      mod_vol2                 = _params->_vol2;
      mod_pan                  = _params->_pan;
      mod_pan2                 = _params->_pan2;
      mod_freq                 = _params->_freq;
      mod_freq2                = _params->_freq2;
      mod_mod                  = _params->_mod;
      mod_timestretch          = _params->_timestretch;
      mmdst_timestretch        = 0.0f;
      mod_timestretch_bend     = _params->_timestretch_bend;
      mmdst_timestretch_bend   = 0.0f;
      mod_sampleoff            = _params->_sampleoff;
      mod_sampleoff2           = _params->_sampleoff2;
      mmdst_sampleoff          = 0.0f;
      mmdst_cyclelen_sampleoff = 0.0f;
      b_queued_delayed_loop    = YAC_FALSE;
      b_queued_wt2d            = YAC_FALSE;
      wt2d_x                   = 0.0f;
      wt2d_y                   = 0.0f;
      mod_sampleoff_wt2d       = 0.0f;
      mmdst_sync_speed         = 0.0f;
      mmdst_sync_slew          = 1.0f;
      mmdst_srr                = 0.0f;
      mmdst_br                 = 0.0f;

      for(sUI i = 0u; i < STSAMPLEVOICE_NUM_REGS; i++)
      {
         mmdst_reg[i] = 0.0f;
      }
      mmdst_cc = 0.0f;

      for(sUI i = 0u; i < STSAMPLE_NUM_MODMATRIX_ENTRIES; i++)
      {
         switch(sample->modmatrix[i].slew_reset)
         {
            default:
            case STSAMPLE_MM_SLEW_RESET_OFF:
               break;

            case STSAMPLE_MM_SLEW_RESET_ZERO:
               modmatrix_slewed_src_values[i] = 0.0;
               break;

            case STSAMPLE_MM_SLEW_RESET_ONE:
               modmatrix_slewed_src_values[i] = 1.0;
               break;
         }
      }
      // // memset((void*)modmatrix_slewed_src_values, 0, sizeof(sF64) * STSAMPLE_NUM_MODMATRIX_ENTRIES);
   }

   if(!b_glide || sample->b_glide_retrig_sample)
   {
      if(sample->sampleoffset_rand_amount > 0.0f)
      {
         mod_sampleoff2 += ((sF32) (lfsr.randf()*sample->sampleoffset_rand_amount));
      }

      if(sample->b_invert_sampleoffset_velocity_amount)
      {
         mod_sampleoff2 += sample->sampleoffset_velocity_amount - (sample->sampleoffset_velocity_amount * _params->_vel);
      }
      else
      {
         mod_sampleoff2 += (sample->sampleoffset_velocity_amount * _params->_vel);
      }
   }

   if(!b_glide)
   {
      mod_cyclelen       = _params->_cyclelen;
      mmdst_cyclelen     = 1.0f;
      mmdst_cyclelen_eol = 1.0f;
      cur_cyclelen_eol   = 1.0f;
      queued_cyclelen_eol = -1.0f;
      mod_sampleshift    = _params->_sampleshift;
      mmdst_sampleshift  = 0.0f;
      mod_sampleshift_endofloop   = _params->_sampleshift_endofloop;
      mmdst_sampleshift_endofloop_last = -1.0f;
      mod_altvolume      = _params->_altvolume;
      mod_jumptoloop     = _params->_jumptoloop;

      last_sample_sampleshift_eol_l = 0.0f;
      last_sample_sampleshift_eol_r = 0.0f;;
      sampleshift_eol_xfade_frames_left = 0u;

      looprestart_xfade_frames_left = 0u;
      last_sample_looprestart_l = 0.0f;
      last_sample_looprestart_r = 0.0f;
   }

   if(!b_glide || sample->b_glide_retrig_sample)
   {
      wavepath_idx = queued_wavepath_idx = next_wavepath_idx = _params->_wavepathidx;

      // // Randomize sample offset ? (test)
      // // // mod_sampleoff += ((rand()*(0.999999999999f/((sF32)RAND_MAX)) )*0.0001) - (0.0001*0.5);

      // Reset loop vars/registers
      {
         for(sUI i = 0u; i < 16u; i++)
         {
            loop_vars[i] = 0;
         }
      }

      // Determine number of sample loops
      if(getCurrentSampleLoops())
      {
         num_sample_loops = (sSI) getCurrentSampleLoops()->num_elements / 3; // div 3 because of sampleoff,len,repeats
      }
      else
      {
         num_sample_loops = 0;
      }

      b_timedloop = YAC_FALSE;
      fadein_countdown = -1;
      fadeout_countdown = -1;

      b_fading_out = YAC_FALSE;
      b_zero = YAC_FALSE;
   }

   // printf("\n\nxxx reallyStartVoice: this=0x%p\n", this);
   // Dyac_host_printf("xxx reallyStartVoice: this=0x%p num_sample_loops=%d sample=0x%p sample->sample_loops=0x%p\n", this, num_sample_loops, sample, sample->sample_loops);

   b_trig_pulse = YAC_TRUE;
   b_release = YAC_FALSE;
   b_release_pulse = YAC_FALSE;

   if(sample->b_liverec && (STSAMPLE_LIVEREC_DBLBUF_MODE_OFF != sample->liverec_doublebuffer_mode))
   {
      sample->handleLiveRecDeferredBufferSwap();
   }

   if(!b_glide || sample->b_glide_retrig_sample)
   {
      // Handle loop-only replay
      //
      if(0 == current_sample_len)
      {
         sF32 *dummy;
         b_timedloop = sample->b_timedloop && (num_sample_loops > 0);
         timedloop_base = sample->timedloop_base;

         if(!handleEndOfLoop(&current_play_offset, &dummy, &dummy, sample->b_from_start/*bFromStart*/, YAC_TRUE/*bAllowCOffReset*/))
         {
            // no loops, nothing to play
            return;
         }
      }
      else
      {
         // Absolute sample offset
         current_sample_offset = sample->sample_offset;
         // Dyac_host_printf("xxx retrig current_sample_offset = %u\n", current_sample_offset);
      }

      // Calculate initial (relative) play offset
      last_relsmpoff = -1; // Force calculation of current_play_offset
      calcCurrentSampleLen();

      current_ts_offset   = 0.0f;
      samples_since_cycle = 0;
      fragment_interpol   = 0.0f;

      if(sample->b_timestretch)
      {
         sF32 ts;
         calcCurrentCycleLen(ts);
         calcCurrentSampleLen();

         if(sample->timestretch_startphase_rand_amount > 0.0f)
         {
            current_ts_offset = (lfsr.rand() % (sUI)current_cyclelen ) * sample->timestretch_startphase_rand_amount;
            while(current_ts_offset >= current_cyclelen)
               current_ts_offset -= current_cyclelen;
         }

      }

      calcCurrentOffset();

      if(sample->b_wavepath)
      {
         current_play_offset = (sF64) wavepath_next_play_offset;
      }

      // Clip play offset
      // Dyac_host_printf("xxx clip play offset: relSmpOff=%f off=%f currentLen=%u\n", relSmpOff, current_play_offset, current_sample_len);
      if( ((sUI)current_play_offset) >= current_sample_len)
      {
         if(0 != current_sample_len)
         {
            current_play_offset = ((sF64) current_sample_len)-1.0f;
         }
         else
         {
            current_play_offset = 0.0f;
         }
      }

      if(sample->b_wavepath)
      {
         // Copy back clipped offset
         wavepath_next_play_offset = (sUI) current_play_offset;
      }

      ////Dyac_host_printf("xxx reallyStartVoice: current_play_offset=%f\n", current_play_offset);

      ////printf("xxx reallyStartVoice: off=%u len=%u\n", current_sample_offset, current_sample_len);

      if(sample->b_timestretch_additive)
      {
         if(sample->partial_cyclelen_reset >= 8.0f)
         {
            partial_phase_offset = 0.0f;
            if((NULL != sample->partial_phases) && !sample->b_partial_phase_0)
            {
               sUI k = 0u;
               for(sUI waveIdx = 0u; waveIdx < 256u; waveIdx++)
               {
                  for(sUI partialIdx = 0u; partialIdx < 256u; partialIdx++)
                  {
                     partial_phases[k] = sU16( ((lfsr.randf() * sample->timestretch_startphase_rand_amount) + sample->partial_phases->elements[k]) * 65536);
                     // if(waveIdx == 0u)
                     //    Dyac_host_printf("xxx reallyStartVoice: sample->partial_phases->elements[%u]=%f\n", k, sample->partial_phases->elements[k]);
                     k++;
                  }
               }
            }
            else if(sample->timestretch_startphase_rand_amount > 0.0f)
            {
               sUI k = 0u;
               for(sUI waveIdx = 0u; waveIdx < 256u; waveIdx++)
               {
                  for(sUI partialIdx = 0u; partialIdx < 256u; partialIdx++)
                  {
                     partial_phases[k] = sU16( ((lfsr.randf() * sample->timestretch_startphase_rand_amount)) * 65536);
                     k++;
                  }
               }
            }
            else
            {
               memset((void*)partial_phases, 0, sizeof(partial_phases));
            }
         }
         else if(sample->b_partial_phase_0)
         {
            if(sample->timestretch_startphase_rand_amount > 0.0f)
            {
               sUI k = 0u;
               for(sUI waveIdx = 0u; waveIdx < 256u; waveIdx++)
               {
                  for(sUI partialIdx = 0u; partialIdx < 256u; partialIdx++)
                  {
                     partial_phases[k] = sU16( ((lfsr.randf() * sample->timestretch_startphase_rand_amount)) * 65536);
                     k++;
                  }
               }
            }
            else
            {
               memset((void*)partial_phases, 0, sizeof(partial_phases));
            }
         }
      }

      b_slew_partial_first = YAC_TRUE;
      fade_partial_amt = 0.0f;

      // Lazy fill zero samples array
      if(b_fill_zero_samples)
      {
         b_fill_zero_samples = YAC_FALSE;
         memset((void*)zero_samples, 0, sizeof(sF32) * (2u * NUM_ZERO_SAMPLES));
      }
   }

   // Dyac_host_printf("xxx current_sample_offset = %u\n", current_sample_offset);

   // Dyac_host_printf("xxx reallyStartVoice: sample has %d loops. b_timedloop=%d\n", num_sample_loops, b_timedloop);

   sUI retrigMask = _params->_retrigmask;
   // Dyac_host_printf("xxx startvoice: retrigMask=%02x\n", retrigMask);
   // Dyac_host_printf("xxx startvoice: b_glide=%d final retrigMask=%02x\n", b_glide, retrigMask);


   // Start ADSR envelopes and LFOs
   startADSRAndLFO(retrigMask, true/*bNoteOn/bResetMod*/ && !b_glide);
   // Dyac_host_printf("xxx reallystartvoice: after startadsr adsr_vol.current_env=%p elements=%p\n", adsr_vol.current_env, adsr_vol.current_env->elements);

   if(!b_glide)
   {
      lfo_freq.mod_speed = _params->_lfofreqspd;
      lfo_freq.mod_level = _params->_lfofreqlvl;
      lfo_freq.mmdst_speed = 1.0f;
      // // lfo_freq.mmdst_level = 0.0f;  // relative

      sample->lfo_freq_global.mod_speed = _params->_lfofreqspd;
      sample->lfo_freq_global.mod_level = _params->_lfofreqlvl;
      sample->lfo_freq_global.mmdst_speed = 1.0f;
      // // sample->lfo_freq_global.mmdst_level = 0.0f;  // relative

      lfo_vol.mod_speed = _params->_lfovolspd;
      lfo_vol.mod_level = _params->_lfovollvl;
      lfo_vol.mmdst_speed = 1.0f;
      // // lfo_vol.mmdst_level = 0.0f;  // relative

      sample->lfo_vol_global.mod_speed = _params->_lfovolspd;
      sample->lfo_vol_global.mod_level = _params->_lfovollvl;
      sample->lfo_vol_global.mmdst_speed = 1.0f;
      // // sample->lfo_vol_global.mmdst_level = 0.0f;  // relative

      lfo_pan.mod_speed = _params->_lfopanspd;
      lfo_pan.mod_level = _params->_lfopanlvl;
      lfo_pan.mmdst_speed = 1.0f;
      // // lfo_pan.mmdst_level = 0.0f;  // relative

      sample->lfo_pan_global.mod_speed = _params->_lfopanspd;
      sample->lfo_pan_global.mod_level = _params->_lfopanlvl;
      sample->lfo_pan_global.mmdst_speed = 1.0f;
      // // sample->lfo_pan_global.mmdst_level = 0.0f;  // relative

      lfo_aux.mod_speed = _params->_lfoauxspd;
      lfo_aux.mod_level = _params->_lfoauxlvl;
      lfo_aux.mmdst_speed = 1.0f;
      // // lfo_aux.mmdst_level = 0.0f;  // relative

      sample->lfo_aux_global.mod_speed = _params->_lfoauxspd;
      sample->lfo_aux_global.mod_level = _params->_lfoauxlvl;
      sample->lfo_aux_global.mmdst_speed = 1.0f;
      // // sample->lfo_aux_global.mmdst_level = 0.0f;  // relative

      adsr_freq.mod_speed     = _params->_adsrfreqspd;
      adsr_freq.mod_level     = _params->_adsrfreqlvl;
      adsr_freq.mod_intensity = _params->_adsrfreqint;
      adsr_freq.mod_aspeed    = _params->_afreqspd;
      adsr_freq.mod_sspeed    = _params->_sfreqspd;
      adsr_freq.mod_rspeed    = _params->_rfreqspd;

      adsr_vol.mod_speed     = _params->_adsrvolspd;
      adsr_vol.mod_level     = _params->_adsrvollvl;
      adsr_vol.mod_intensity = _params->_adsrvolint;
      adsr_vol.mod_aspeed    = _params->_avolspd;
      adsr_vol.mod_sspeed    = _params->_svolspd;
      adsr_vol.mod_rspeed    = _params->_rvolspd;

      adsr_pan.mod_speed     = _params->_adsrpanspd;
      adsr_pan.mod_level     = _params->_adsrpanlvl;
      adsr_pan.mod_intensity = _params->_adsrpanint;
      adsr_pan.mod_aspeed    = _params->_apanspd;
      adsr_pan.mod_sspeed    = _params->_spanspd;
      adsr_pan.mod_rspeed    = _params->_rpanspd;

      adsr_aux.mod_speed     = _params->_adsrauxspd;
      adsr_aux.mod_level     = _params->_adsrauxlvl;
      adsr_aux.mod_intensity = _params->_adsrauxint;
      adsr_aux.mod_aspeed    = _params->_aauxspd;
      adsr_aux.mod_sspeed    = _params->_sauxspd;
      adsr_aux.mod_rspeed    = _params->_rauxspd;
   }

   if(!b_glide)
      filter_type          = sample->filter_type;
   mod_filter_cutoff    = _params->_filter_cutoff;
   mod_filter_pan       = _params->_filter_pan;
   mod_filter_offset    = _params->_filter_offset;
   mod_filter_resonance = _params->_filter_resonance;
   if(!b_glide)
      resetBiquad();

   mod_glide_switch = _params->_glide_switch;
   mod_glide_speed  = _params->_glide_speed;
   mmdst_glide_speed = 1.0f;

   perfctl_poly_pressure = _params->_perfctl_poly_pressure;
   ::memcpy((void*)perf_ctl_on,  (void*)sample_player->perf_ctl, sizeof(perf_ctl_on));
   ::memcpy((void*)perf_ctl_off, (void*)sample_player->perf_ctl, sizeof(perf_ctl_off));  // copy in case option is changed in editor while sample is playing

   if(!b_glide)
      perfctl_pressure_max = 0.0f;

   // Start Mod Sequencers
   for(sUI i = 0u; i < STSAMPLE_NUM_MODSEQ; i++)
      modseq_patch[i] = _params->_modseq_patch[i];

   retrigMask = _params->_modseq_retrigmask;
   startModSeq(retrigMask, true/*bNoteOn/bResetMod*/ && !b_glide);

   for(sUI i = 0u; i < STSAMPLE_NUM_MODSEQ; i++)
   {
      modseq[i].mod_speed    = _params->_modseq_speed[i];
      modseq[i].mod_level    = _params->_modseq_level[i];
      modseq[i].mod_numsteps = _params->_modseq_numsteps[i];
      modseq[i].mod_advance  = _params->_modseq_advance[i];
      modseq[i].mmdst_speed    = 1.0f;
      // // modseq[i].mmdst_level    = 0.0f;  // relative
      modseq[i].mmdst_numsteps = 0.0f;  // relative
      modseq[i].mmdst_advance  = 0.0f;  // relative

      sample->modseq_global[i].mod_speed    = _params->_modseq_speed[i];
      sample->modseq_global[i].mod_level    = _params->_modseq_level[i];
      sample->modseq_global[i].mod_numsteps = _params->_modseq_numsteps[i];
      sample->modseq_global[i].mod_advance  = _params->_modseq_advance[i];
      sample->modseq_global[i].mmdst_speed    = 1.0f;
      // // sample->modseq_global[i].mmdst_level    = 0.0f;  // relative
      sample->modseq_global[i].mmdst_numsteps = 0.0f;  // relative
      sample->modseq_global[i].mmdst_advance  = 0.0f;  // relative
   }


   // Determine "max" volume according to velocity zone map
   cached_vel_vol = sample->calcVelZoneVolume(_params->_vel);

   if(!_bDelayed)
   {
      float_block_size     = STSAMPLEVOICE_BLOCK_LEN * _params->_mixRate;
      float_block_off      = 0.0f;
      current_block_size   = (sUI) float_block_size;
      current_block_offset = 0;
   }

   // Dyac_host_printf("xxx reallystartvoice: 2 startadsr adsr_vol.current_env=%p elements=%p\n", adsr_vol.current_env, adsr_vol.current_env->elements);

   if(!b_glide || sample->b_glide_retrig_sample)
   {
      // (block-based) volume ramping
      volramp_fadeout_countdown = -1;
      if(sample->num_volramp_steps_in > 0)
      {
         volramp_fadein_countdown = sample->num_volramp_steps_in;// + 1; // +1 since calcNextBlockState is called twice
         volramp_fade_vol = 0.0f;
      }
      else
      {
         volramp_fadein_countdown = -1;
         volramp_fade_vol = 1.0f;
      }
   }
   else if(b_glide)
   {
      volramp_fadeout_countdown = -1;
      volramp_fadein_countdown = -1;
      volramp_fade_vol = 1.0f;
   }

   // Dyac_host_printf("xxx volramp_fadein_countdown=%d\n", volramp_fadein_countdown);
   // Dyac_host_printf("xxx volramp_fadeout_countdown=%d\n", volramp_fadeout_countdown);

   // Configure voice plugins
   // Dyac_host_printf("xxx reallyStartVoice: BEGIN configure voice plugins\n");
   for(sUI pluginIdx = 0u; pluginIdx < STSAMPLE_NUM_PLUGINS; pluginIdx++)
   {
      st_plugin_cache_entry_t *pce = plugin_cache_entries[pluginIdx];
      if(NULL != pce)
      {
         // Sample player
         if(NULL != pce->voice->info->set_sampleplayer)
         {
            pce->voice->info->set_sampleplayer(pce->voice, (void*)sample_player, sample_player->num_voices);
         }

         // Sample bank
         if(NULL != pce->voice->info->set_samplebank)
         {
            pce->voice->info->set_samplebank(pce->voice, (void*)sample->parent_samplebank);
         }

         // Sample
         if(NULL != pce->voice->info->set_sample)
         {
            pce->voice->info->set_sample(pce->voice, (void*)sample);
         }

         // Sample rate
         if(NULL != pce->voice->info->set_sample_rate)
         {
            pce->voice->info->set_sample_rate(pce->voice, mix_rate);
         }

         // BPM
         if(NULL != pce->voice->info->set_bpm && NULL != sample_player)
         {
            pce->voice->info->set_bpm(pce->voice, sample_player->bpm);
         }

         // Note-on
         // Dyac_host_printf("xxx note on: layer_idx=%u num_layers=%u\n", layer_idx, voice_bus_first_voice->num_layers);
         pce->voice->voice_bus_buffers = voice_bus_buffers;
         pce->voice->layer_idx         = layer_idx;
         pce->voice->voice_idx         = voice_idx;
         pce->voice->active_note_idx   = active_note_idx;
         pce->voice->note_hz           = noteToFreq(sF32(note)) * liverec_osc_pitch_factor;

         if(NULL != pce->voice->info->note_on)
         {
            pce->voice->info->note_on(pce->voice,
                                      b_glide ? sample->b_glide_retrig_sample ? -1 : 1 : 0,
                                      (sU8)note,
                                      _params->_vel
                                      );
         }
      }
   }


   // Dyac_host_printf("xxx reallystartvoice: 3 startadsr adsr_vol.current_env=%p elements=%p\n", adsr_vol.current_env, adsr_vol.current_env->elements);

   if(!b_glide)  // [09Jan2024] fix glide clicks due to "cached_vel_vol" (velocity) changes
   {
      calcNextBlockState(YAC_FALSE/*bNext*/); // Calc initial state
   }

   // Dyac_host_printf("xxx reallystartvoice: 4 startadsr adsr_vol.current_env=%p elements=%p\n", adsr_vol.current_env, adsr_vol.current_env->elements);

   calcNextBlockState(YAC_TRUE/*bNext*/); // Calc "next" state

   // Dyac_host_printf("xxx reallystartvoice: 5 startadsr adsr_vol.current_env=%p elements=%p\n", adsr_vol.current_env, adsr_vol.current_env->elements);

   if(!b_glide || sample->b_glide_retrig_sample)
   {
      if((sample->b_timedloop && sample->b_timedloop_fade))///// || b_restartfadein)
      {
         // Fadein first "n" samples to avoid clicks
         fadein_countdown = VOLRAMP_NUMSTEPS;
      }
   }

   ////printf("xxx reallyStartVoice: cvol=%f nvol=%f\n", current_vol, next_vol);

   // // printf("[---] reallyStartVoice: current_sample_len is %u\n", current_sample_len);
   // // if(0 == current_sample_len)
   // // {
   // //    printf("[---] reallyStartVoice: current_sample_len is 0 !!\n");
   // //    stopVoice();
   // //    return;
   // // }

   if(sample->b_liverec)
   {
      sF32 hz = noteToFreq(sample->liverec_osc_tune);
      // // // sample->liverec_ref_cyclelen =
      // // //    (hz / sample->waveform->base_frequency) *
      // // //    ((sample->waveform->sample_rate * sample->sample_rate_ratio) / mix_rate);
      if(hz > 0.0f)
      {
         sample->liverec_ref_cyclelen = (sample->waveform->sample_rate * sample->sample_rate_ratio) / hz;
      }
      else
      {
         sample->liverec_ref_cyclelen = 0.0f;
      }
   }

   updateWt2dOffset();

   // Determine voiceplugin-optimized rendering mode
   {
      sBool bOk = YAC_TRUE;

      bOk = (NULL != sample->sample_loops);
      bOk = bOk && (sample->sample_loops->elements[1] <= 32);
      bOk = bOk && (sample->sample_loops->elements[2] ==  0);
      // // bOk = bOk && (0u == br_mask);
      bOk = bOk && (0u == sample->num_volramp_steps_in);
      bOk = bOk && (0u == sample->num_volramp_steps_out);
      bOk = bOk && ((YAC_FALSE == sample->b_filter) || (STSAMPLE_FLT_TYPE_NONE == sample->filter_type));
      bOk = bOk && (YAC_TRUE == sample->b_enable_fx);
      bOk = bOk && (0u == sample->plugin_alg);
      if(bOk)
      {
         // Does first plugin exist and does it have ST_PLUGIN_FLAG_OSC ?
         st_plugin_cache_entry_t *pce = plugin_cache_entries[0u];
         bOk = (NULL != pce);
         bOk = bOk && (0u != (pce->voice->info->flags & ST_PLUGIN_FLAG_OSC));
      }

      b_voiceplugin_osc_hint = bOk;
      // Dyac_host_printf("xxx b_voiceplugin_osc_hint=%d\n", b_voiceplugin_osc_hint);
   }


   b_playing = YAC_TRUE;

   // Dyac_host_printf("xxx reallystartvoice: LEAVE adsr_vol.current_env=%p elements=%p\n", adsr_vol.current_env, adsr_vol.current_env->elements);
   // Dyac_host_printf("xxx reallyStartVoice: LEAVE\n");
}

void StSampleVoice::stopVoice(void) {
   // stop *immediately*

   // Dyac_host_printf("xxx StSampleVoice::stopVoice: this=0x%p\n", this);

   if(b_playing)
   {
      if(current_delay_countdown > 0)
      {
         // Sample still delaying, do not fade out
         current_delay_countdown = 0;
         anticlick_fadeout_countdown = 0;
      }
      else
      {
         // fade out last played sample to avoid clicks
         anticlick_fadeout_countdown = ANTICLICK_FADEOUT_TIME;
      }
      // Note: voice_key is unset as soon as sample has been faded out

      b_playing = YAC_FALSE;
   }

   if(NULL != sample)
   {
      sample->waveform->ui_last_played_offset = -1.0f;
   }
}

void StSampleVoice::softStopVoice(void) {

   // Already fading out ?
   if(volramp_fadeout_countdown < 0)
   {
      // Dyac_host_printf("xxx StSampleVoice::softStopVoice: this=0x%p b_playing=%d volramp_fadeout_countdown=%d\n", this, b_playing, volramp_fadeout_countdown);

      queued_noteon.b_valid = 0;

      b_used = YAC_FALSE; // => can allocate

      // //xxx workaround to prevent clicks in "14Sep2010" testcase (bassline)
      //    ==> should be fixed now
      // //stopVoice();

      // stop after volume ramp fadeout
      if(b_playing && (NULL != sample))
      {
         volramp_fadeout_countdown = (sSI)sample->num_volramp_steps_out;
      }

      if(volramp_fadeout_countdown < 0)
         volramp_fadeout_countdown = 0;

      if(NULL != sample)
      {
         sample->waveform->ui_last_played_offset = -1.0f;
      }
   }
}

void StSampleVoice::prepareToPlay(StSample *_sample, sSI _voiceKey, sUI _voiceAllocIdx) {
   // called directly after voice allocation
   //  (todo) this will not work with softStopVoice() / fadeout since sampleptr is already set here
   //          move this to reallyStartVoice ?
   // // if(NULL == sample)
   // // {
   // //    sample = _sample; // normally done in startVoice(), but "sample" must always point to a valid sample
   // // }
   sample = _sample; // must set this or applyMaxVoicesConstraint() may stop the wrong voices

   // LFO+ModSeq global modulation
   sample->setSamplePlayerForGlobalModulation(sample_player);

   voice_key       = _voiceKey;
   last_voice_key  = _voiceKey;
   voice_alloc_idx = _voiceAllocIdx;
   b_used          = YAC_TRUE;
   b_allow_smpoff  = YAC_TRUE;

   for(sUI mmIdx = 0u; mmIdx < STSAMPLE_NUM_MODMATRIX_ENTRIES; mmIdx++)
   {
      modmatrix_amt[mmIdx] = 0.0f;
   }

   // // b_release     = 0; // moved to reallyStartVoice();

   // // mod_vol         = 1.0f;
   // // mod_pan         = 0.0f;
   // // mod_freq        = 0.0f;
   // // mod_mod         = 0.0f;
   // // mod_timestretch = 0.0f;
   // // mod_sampleoff   = 0.0f;
   // // mod_cyclelen    = 0.0f;
   // // mod_sampleshift = 0.0f;
   // // mod_altvolume   = 1.0f;
   // // mod_jumptoloop  = -1;

   current_delay_countdown = 0;

   for(sUI randIdx = 0u; randIdx < 4u; randIdx++)
   {
      noteon_rand_p[randIdx] = lfsr.randf();
      noteon_rand_pm[randIdx] = noteon_rand_p[randIdx] * 2.0f - 1.0f;
   }

   active_note_idx = sample_player->getNumActiveNotes();

   release_velocity = 0.0f;
}

void StSampleVoice::initStartedVoice(void) {
   // called when voice should actually start playing (after ctl/param have been set)

   // Dyac_host_printf("xxx initStartedVoice: queued_noteon.b_valid=%d sample=%p\n", queued_noteon.b_valid, sample);
   // Dyac_host_printf("xxx initStartedVoice: sample->num_volramp_steps_out=%u\n", sample->num_volramp_steps_out);

   b_alloc = YAC_FALSE;

   if(queued_noteon.b_valid)
   {
      b_restartfadein = YAC_FALSE;

      if(b_playing && !b_glide && (sample->num_volramp_steps_out > 0)) // fadeout current sample?
      {
         // Delay actual note-on until sample has been faded out
         // TODO: make ctl. modification attempts block until new sample has really been started?
         volramp_fadeout_countdown = (sSI)sample->num_volramp_steps_out;
         // printf("xxx StSampleVoice::initStartedVoice: fadeout current voice, cd=%d\n", volramp_fadeout_countdown);
      }
      else
      {
         // Initialize voice immediately
         // printf("xxx StSampleVoice::initStartedVoice: start immediately. b_playing=%d\n", b_playing);
         volramp_fadeout_countdown = -1;
         queued_noteon.b_valid     = 0;

         reallyStartVoice(&queued_noteon, 0);
         // Dyac_host_printf("xxx 2 StSampleVoice::initStartedVoice: start immediately. b_playing=%d\n", b_playing);
      }
   }
   else
   {
      b_playing = YAC_FALSE;
      b_used    = YAC_FALSE;
      voice_key = 0;
   }

   // Dyac_host_printf("xxx initStartedVoice: LEAVE: b_playing=%d\n", b_playing);
}

void StSampleVoice::_restartVoice(void) {

   // (note) this causes problems with voice-allocation:
   //         the method is called, when an arpeggio is played (noteOnRetrig())
   //         but during note-on, the next (mono) voice has not been allocated, yet.
   //         setting b_valid to true again will retrigger the previous voice when
   //         the volramp fadeout has finished (softStopVoice())
   //         ==> must not be called when a voice is going to be allocated on the same replay tick

   // ///////////////if(b_playing)
   // printf("xxx StSampleVoice::restartVoice: this=0x%p volramp_fadeout_countdown=%d\n", this, volramp_fadeout_countdown);
   if(!Dsoftstopped())
   {
      // Re-Initialize voice immediately
      b_used                    = YAC_TRUE;
      queued_noteon.b_valid     = YAC_TRUE;
      volramp_fadeout_countdown = (sSI)(sample->num_volramp_steps_out);
      b_playing                 = YAC_TRUE;
      b_restartfadein           = YAC_TRUE;
   }
}

void StSampleVoice::startVoiceInt(StSample *_sample,
                                  const sF32 *_freqTable,
                                  sF32 _mixRate,
                                  sSI _note, sF32 _vel,
                                  sF32 _mod, sF32 _vol, sF32 _pan, sF32 _freq
                                  ) {
   replay_ticks = 0u; // Reset ticks so multi-sample voice is not reallocated in same tick

   queued_noteon._sample                 = _sample;
   queued_noteon._freqTable              = _freqTable;
   queued_noteon._mixRate                = _mixRate;
   queued_noteon._note                   = _note;
   queued_noteon._vel                    = _vel;
   queued_noteon._mod                    = _mod;
   queued_noteon._vol                    = _vol;
   queued_noteon._vol2                   = 1.0f;
   queued_noteon._pan                    = _pan;
   queued_noteon._pan2                   = 0.0f;
   queued_noteon._freq                   = _freq;
   queued_noteon._freq2                  = 0.0f;
   queued_noteon._timestretch            = 0.0f;
   queued_noteon._timestretch_bend       = 0.0f;
   queued_noteon._sampleoff              = 0.0f;
   queued_noteon._sampleoff_ms           = 0.0f;
   queued_noteon._sampleoff2             = 0.0f;
   queued_noteon._cyclelen               = 1.0f;
   queued_noteon._sampleshift            = 0.0f;
   queued_noteon._sampleshift_endofloop  = -1.0f;
   queued_noteon._altvolume              = 1.0f;

   sU8 retrigMask = 255u;  // start all env+LFOs
   if(b_glide)
   {
      if(!_sample->b_glide_retrig_env_freq)
         retrigMask &= ~1u;

      if(!_sample->b_glide_retrig_env_vol)
         retrigMask &= ~2u;

      if(!_sample->b_glide_retrig_env_pan)
         retrigMask &= ~4u;

      if(!_sample->b_glide_retrig_env_aux)
         retrigMask &= ~8u;

      if(!_sample->b_glide_retrig_lfo_freq)
         retrigMask &= ~16u;

      if(!_sample->b_glide_retrig_lfo_vol)
         retrigMask &= ~32u;

      if(!_sample->b_glide_retrig_lfo_pan)
         retrigMask &= ~64u;

      if(!_sample->b_glide_retrig_lfo_aux)
         retrigMask &= ~128u;
   }
   else
   {
      // [07Sep2023] fix slide from glide_src_note init val=60(C-5) after voice reset (replay start)
      glide_src_note = glide_src_note_orig = sF32(_note);
   }
   queued_noteon._retrigmask            = retrigMask;
   queued_noteon._lfofreqspd            = 1.0f;
   queued_noteon._lfofreqlvl            = 1.0f;
   queued_noteon._lfofreqamt            = 1.0f;
   queued_noteon._lfovolspd             = 1.0f;
   queued_noteon._lfovollvl             = 1.0f;
   queued_noteon._lfovolamt             = 1.0f;
   queued_noteon._lfopanspd             = 1.0f;
   queued_noteon._lfopanlvl             = 1.0f;
   queued_noteon._lfopanamt             = 1.0f;
   queued_noteon._lfoauxspd             = 1.0f;
   queued_noteon._lfoauxlvl             = 1.0f;
   queued_noteon._lfoauxfltamt          = 1.0f;
   queued_noteon._adsrfreqspd           = 1.0f;
   queued_noteon._adsrfreqlvl           = 1.0f;
   queued_noteon._adsrfreqint           = 1.0f;
   queued_noteon._adsrvolspd            = 1.0f;
   queued_noteon._adsrvollvl            = 1.0f;
   queued_noteon._adsrvolint            = 1.0f;
   queued_noteon._adsrpanspd            = 1.0f;
   queued_noteon._adsrpanlvl            = 1.0f;
   queued_noteon._adsrpanint            = 1.0f;
   queued_noteon._adsrauxspd            = 1.0f;
   queued_noteon._adsrauxlvl            = 1.0f;
   queued_noteon._adsrauxint            = 1.0f;
   queued_noteon._jumptoloop            = -1;
   queued_noteon._wavepathidx           = _sample->wavepath_idx;
   queued_noteon._afreqspd              = 1.0f;
   queued_noteon._sfreqspd              = 1.0f;
   queued_noteon._rfreqspd              = 1.0f;
   queued_noteon._avolspd               = 1.0f;
   queued_noteon._svolspd               = 1.0f;
   queued_noteon._rvolspd               = 1.0f;
   queued_noteon._apanspd               = 1.0f;
   queued_noteon._spanspd               = 1.0f;
   queued_noteon._rpanspd               = 1.0f;
   queued_noteon._aauxspd               = 1.0f;
   queued_noteon._sauxspd               = 1.0f;
   queued_noteon._rauxspd               = 1.0f;
   queued_noteon._filter_cutoff         = 0.0f;
   queued_noteon._filter_pan            = 0.0f;
   queued_noteon._filter_offset         = 0.0f;
   queued_noteon._filter_resonance      = 0.0f;
   queued_noteon._glide_switch          = YAC_TRUE;
   queued_noteon._glide_speed           = 1.0f;
   queued_noteon._perfctl_poly_pressure = -1.0f;

   retrigMask = 15u;
   if(b_glide)
   {
      if(!_sample->b_glide_retrig_modseq[0])
         retrigMask &= ~1u;

      if(!_sample->b_glide_retrig_modseq[1])
         retrigMask &= ~2u;

      if(!_sample->b_glide_retrig_modseq[2])
         retrigMask &= ~4u;

      if(!_sample->b_glide_retrig_modseq[3])
         retrigMask &= ~8u;
   }
   queued_noteon._modseq_retrigmask  = retrigMask; // start all mod sequencers

   for(sUI i = 0u; i < STSAMPLE_NUM_MODSEQ; i++)
   {
      queued_noteon._modseq_speed[i]    = 1.0f;
      queued_noteon._modseq_level[i]    = 1.0f;
      queued_noteon._modseq_numsteps[i] = 1.0f;
      queued_noteon._modseq_advance[i]  = 0.0f;
      queued_noteon._modseq_patch[i]    = _sample->default_modseq_patches[i];
   }

   queued_noteon.b_valid = YAC_TRUE;

   // Dyac_host_printf("xxx StSampleVoice::startVoice: this=0x%p note=%d vel=%f\n", this, _note, _vel);

   // Note: these queued params can be overwritten by setMod(), setVol(), ..
   //       before the voice is actually started (reallyStartVoice()).
}

void StSampleVoice::_startVoice(YAC_Object *_sample,
                                YAC_Object *_freqTable,
                                sF32 _mixRate,
                                sSI _note, sF32 _vel,
                                sF32 _mod, sF32 _vol, sF32 _pan, sF32 _freq
                                ) {
   // Dyac_host_printf("xxx startVoice: sample=%p freqTable=%p\n", _sample, _freqTable);
   if(YAC_CHK(_sample, clid_StSample))
   {
      YAC_CAST_ARG(StSample, sample, _sample);

      if(YAC_BCHK(_freqTable, YAC_CLID_FLOATARRAY))
      {
         YAC_CAST_ARG(YAC_FloatArray, freqTable, _freqTable);

         startVoiceInt(sample,
                       (freqTable->num_elements >= 128u) ? freqTable->elements : NULL,
                       _mixRate,
                       _note, _vel,
                       _mod, _vol, _pan, _freq
                       );
      }
      else
      {
         Dyac_throw_def(InvalidPointer, "_freqTable is not a valid FloatArray object");
      }
   }
   else
   {
      Dyac_throw_def(InvalidPointer, "_sample is not a valid StSample object");
   }
}

void StSampleVoice::noteOff(sF32 _vel) {

   // Dyac_host_printf("xxx noteOff: vel=%f cached_vel_vol=%f\n", _vel, cached_vel_vol);

   // // cached_vel_vol =
   // //    (cached_vel_vol * (1.0f - sample->volume_velocity_amount)) +
   // //    (cached_vel_vol * _vel * sample->volume_velocity_amount);
   // // if(cached_vel_vol > 1.0f)
   // // {
   // //    cached_vel_vol = 1.0f;
   // // }
   // // else if(cached_vel_vol < 0.00001f)
   // // {
   // //    cached_vel_vol = 0.0f;
   // // }

   // Dyac_host_printf("xxx noteOff b_release=%d delay_countdown=%d\n", b_release, current_delay_countdown);

   // // // queued_noteon.b_valid = 0;

   release_velocity = _vel;

   b_allow_smpoff = !b_glide;  // [03Nov2024]
   b_glide = YAC_FALSE;

   if( (current_delay_countdown > 0) || (NULL == sample) )
   {
      // Has not started playing, yet
      ////printf("xxx stopVoice 1\n");
      stopVoice();
   }
   else if(!b_release)
   {
      b_release = YAC_TRUE;
      b_release_pulse = YAC_TRUE;

      if( (NULL != sample->parent_samplebank) && sample->parent_samplebank->b_perfctl_freeze_noteoff )
      {
         ::memcpy((void*)perf_ctl_off, (void*)sample_player->perf_ctl, sizeof(perf_ctl_off));
      }

      // Notify voice plugins
      for(sUI pluginIdx = 0u; pluginIdx < STSAMPLE_NUM_PLUGINS; pluginIdx++)
      {
         st_plugin_cache_entry_t *pce = plugin_cache_entries[pluginIdx];
         if(NULL != pce)
         {
            if(NULL != pce->voice->info->note_off)
            {
               pce->voice->info->note_off(pce->voice,
                                          (sU8)note,
                                          _vel
                                          );
            }
            // else: plugin is not interested in note ons
         }
      }

      if(adsr_vol.adsr)
      {
         // sBool bGlideRelease = (b_glide && sample->b_glide_release);
         // Dyac_host_printf("xxx bGlideRelease=%d adsr_vol.isInRelease()=%d\n", bGlideRelease, adsr_vol.isInRelease());
         // if(!bGlideRelease || !adsr_vol.isInRelease())
         if(adsr_vol.isInRelease())
            adsr_vol.noteOffRestartRelease();
         else
            adsr_vol.noteOff();

         if(!adsr_vol.isFinished())
         {
            if(adsr_pan.isInRelease())
               adsr_pan.noteOffRestartRelease();
            else
               adsr_pan.noteOff();

            if(adsr_freq.isInRelease())
               adsr_freq.noteOffRestartRelease();
            else
               adsr_freq.noteOff();

            if(adsr_aux.isInRelease())
               adsr_aux.noteOffRestartRelease();
            else
               adsr_aux.noteOff();

            for(sUI i = 0u ; i < STSAMPLE_NUM_MODSEQ; i++)
            {
               modseq[i].noteOff();
               sample->modseq_global[i].noteOff();
            }

            // jump to release loop step
            // Dyac_host_printf("xxx current_loop_idx=%d sample->noteoff_loop_index=%d\n", current_loop_idx, sample->noteoff_loop_index);
            if( (sUI(sample->noteoff_loop_index) < sUI(num_sample_loops)) &&
                (current_loop_idx != sample->noteoff_loop_index)
                )
            {
               current_loop_idx = sample->noteoff_loop_index;

               if(sample->b_timedloop || (current_loop_countdown > 0))
                  current_loop_idx--;  // inc'd in handleEndOfLoop()

               // // if(!b_timedloop && sample->b_timedloop)
               if(sample->b_timedloop)
               {
                  // [04Jan2019] Re-Enable timedloop after infinite loop
                  samples_until_end_of_loopstep = (VOLRAMP_NUMSTEPS + 1);  // fade-out + start next loop after volume ramp
                  b_fading_out = YAC_FALSE;
                  b_timedloop = YAC_TRUE;
               }
               else if(sample->b_noteoff_immediate_loop_jump)  //  || b_timedloop
               {
                  // may cause clicks, especially with "single-cycle" waveforms
                  current_loop_countdown = 1;
                  sF32 *dummy;
                  handleEndOfLoop(&current_play_offset, &dummy, &dummy, YAC_FALSE/*bFromStart*/, YAC_TRUE/*bAllowCOffReset*/);
               }
            }
         }
         else
         {
            // No Release phase, "immediately" stop voice (after volume ramp down)
            softStopVoice();
            ////printf("xxx StSampleVoice::noteOff: no release phase, softStopVoice 2\n");
         }
      }
      else
      {
         // No Release phase, "immediately" stop voice (after volume ramp down)
         softStopVoice();
         ////printf("xxx StSampleVoice::noteOff: no release phase, softStopVoice\n");
      }

   }
}

void StSampleVoice::calcNextBlockState(sBool _bNext) {
   // Copy state
   current_vol  = next_vol;
   current_pan  = next_pan;
   current_freq = next_freq;
   current_aux_adsr  = next_aux_adsr;
   current_aux_lfo  = next_aux_lfo;

   // Dyac_host_printf("xxx calcNextBlockState: this=0x%p b_playing=%d volramp_fadeout_countdown=%d cvol=%f cpan=%f\n", this, b_playing, volramp_fadeout_countdown, current_vol, current_pan);

   // Do sample fadeout
   if(volramp_fadeout_countdown >= 0)
   {

      // Continue fade out
      volramp_fadeout_countdown--;
      if(-1 == volramp_fadeout_countdown)
      {
         ////printf("xxx StSampleVoice: fade-out finished this=0x%p b_valid=%d\n", this, queued_noteon.b_valid);

         // Fadeout finished
         if(queued_noteon.b_valid) // xx paranoia check, should never be 0 ATP
         {
            // Start queued sample
            volramp_fade_vol = 0.0f;
            queued_noteon.b_valid = YAC_FALSE;
            reallyStartVoice(&queued_noteon, 1);
            // Note: reallyStartVoice re-enters calcNextBlockState()
            return;
         }
         else
         {
            // Dyac_host_printf("xxx stopVoice 2\n");
            stopVoice();

            // Allow re-allocation of voice
            voice_key = 0;
         }
      }
      else
      {
         volramp_fade_vol = (((sF32)volramp_fadeout_countdown) / sample->num_volramp_steps_out);
         // Dyac_host_printf("xxx volramp_fade_vol=%f countdown=%d\n", volramp_fade_vol, volramp_fadeout_countdown);
      }
   }
   else if(volramp_fadein_countdown >= 0)
   {
      // Do sample fade in
      volramp_fade_vol = 1.0f - ( ((sF32)volramp_fadein_countdown) / sample->num_volramp_steps_in);
      volramp_fadein_countdown--;
      // Dyac_host_printf("xxx volramp_fade_vol=%f cd=%d/%d\n", volramp_fade_vol, volramp_fadein_countdown, sample->num_volramp_steps_in);
   }



   // Update modifiers to values set by the application

   // Calc new state
   ////printf("xxx sample.vol=%f cached_vel_vol=%f mod_vol=%f sp.vol=%f\n", sample->volume, cached_vel_vol, mod_vol, sample_player->mod_vol);

   next_freq = getCurrentGlideNote();
   next_freq = next_freq + sample->transpose + mod_freq + mod_freq2 + sample_player->mod_freq + sample_player->mod_freq2;
   // printf("xxx next_freq=%f\n", next_freq);
   if(NULL != sample->parent_samplebank)
   {
      next_freq += sample->parent_samplebank->transpose;
   }

   sF32 modZoneVol = sample->calcKeyModZoneVolume (sF32(note), getEffectiveMod());

   next_vol  =
      sample->volume               *
      cached_vel_vol               *
      mod_vol                      *
      mod_vol2                     *
      sample_player->mod_vol       *
      mod_altvolume                *
      sample_player->mod_altvolume *
      modZoneVol                   ;

   if(sample->voice_calibration_enable[2/*volume*/])
   {
      next_vol +=
         sample->voice_calibration_amount[2/*volume*/] * sample->voice_calibration
         [2/*volume*/]
         [voice_alloc_idx % sample->voice_calibration_modulo[2/*volume*/]];
   }

#if 0
   Dyac_host_printf("xxx next_vol=%f sample->volume=%f cached_vel_vol=%f mod_vol=%f mod_vol2=%f sample_player->mod_vol=%f mod_altvolume=%f sample_player->mod_altvolume=%f modZoneVol=%f\n", next_vol, sample->volume, cached_vel_vol, mod_vol, mod_vol2, sample_player->mod_vol, mod_altvolume, sample_player->mod_altvolume, modZoneVol);
#endif


#if 0
   static int xxxVol = 0;
   if(0 == (xxxVol++&127))
   {
      Dyac_host_printf("xxx next_vol=%f sample->volume=%f cached_vel_vol=%f mod_vol=%f mod_vol2=%f sample_player->mod_vol=%f mod_altvolume=%f sample_player->mod_altvolume=%f\n", next_vol, sample->volume, cached_vel_vol, mod_vol, mod_vol2, sample_player->mod_vol, mod_altvolume, sample_player->mod_altvolume);
   }
#endif

   if(NULL != sample->parent_samplebank)
   {
      next_vol *= sample->parent_samplebank->volume;
   }

   // Dyac_host_printf("xxx calcNextBlockState: next_vol=%f sample->volume=%f cached_vel_vol=%f mod_vol=%f sample_player->mod_vol=%f mod_altvolume=%f sample_player->mod_altvolume=%f\n", next_vol, sample->volume, cached_vel_vol, mod_vol, sample_player->mod_vol, mod_altvolume, sample_player->mod_altvolume);

   next_pan  = sample->pan + mod_pan + mod_pan2 + sample_player->mod_pan;
   // Dyac_host_printf("xxx next_pan=%f\n", next_pan);

   if(NULL != sample->parent_samplebank)
   {
      next_pan += sample->parent_samplebank->pan;
   }

   if(sample->voice_calibration_enable[1/*pan*/])
   {
      next_pan +=
         sample->voice_calibration_amount[1/*pan*/] * sample->voice_calibration
         [1/*pan*/]
         [voice_alloc_idx % sample->voice_calibration_modulo[1/*pan*/]];
   }

   next_aux_adsr = 0.0f;
   next_aux_lfo = 0.0f;

   ////printf("xxx next_pan=%f smp=%f mod=%f sp=%f\n", next_pan, sample->pan, mod_pan, sample_player->mod_pan);

   // Dyac_host_printf("xxx nextblockstate: 1 startadsr adsr_vol.current_env=%p elements=%p\n", adsr_vol.current_env, adsr_vol.current_env->elements);

   // (note) vol/pan destinations are next_vol and next_pan

   sUI prepareNumFrames = _bNext ? current_block_size : 0u;

   if(0u != sample->plugin_mask_vm)
   {
      if(0u == prepareNumFrames)
      {
         // yac_host->printf("xxx call updatePluginModulationInitial mask=0x%08x\n", sample->plugin_mask_vm);
         updatePluginModulationInitial(sample->plugin_mask_vm);
         processPluginsVM(0u/*numFrames*/);  // just call prepare_block
      }
      else
      {
         // yac_host->printf("xxx call regular processPluginsVM mask=0x%08x\n", sample->plugin_mask_vm);
      }
      processPluginsVM(1u/*numFrames*/);  // call prepare_block and process_replace (update STSAMPLE_MM_SRC_VOICE_PLUGIN_1..8)
   }

   // Process mod matrix
   calcModMatrix(mmdst);

   // if(replay_ticks < 10u)
   //    Dyac_host_printf("xxx tick %u: reg[0] is %f \n", replay_ticks, mmdst_reg[0]);

   // Retrig
   //  (todo) recalc lfo/env/modseq state when it is retriggered
   //          (otherwise the new output state will be a tick too late)
   if(mmdst.retrig_freq_env >= 0.5f)
   {
      startADSRAndLFO(0x01, YAC_FALSE/*bNoteOn*/);
   }
   if(mmdst.retrig_vol_env >= 0.5f)
   {
      startADSRAndLFO(0x02, YAC_FALSE/*bNoteOn*/);
   }
   if(mmdst.retrig_pan_env >= 0.5f)
   {
      startADSRAndLFO(0x04, YAC_FALSE/*bNoteOn*/);
   }
   if(mmdst.retrig_aux_env >= 0.5f)
   {
      startADSRAndLFO(0x08, YAC_FALSE/*bNoteOn*/);
   }
   if(mmdst.retrig_freq_lfo >= 0.5f)
   {
      startADSRAndLFO(0x10, YAC_FALSE/*bNoteOn*/);
   }
   if(mmdst.retrig_vol_lfo >= 0.5f)
   {
      startADSRAndLFO(0x20, YAC_FALSE/*bNoteOn*/);
   }
   if(mmdst.retrig_pan_lfo >= 0.5f)
   {
      startADSRAndLFO(0x40, YAC_FALSE/*bNoteOn*/);
   }
   if(mmdst.retrig_aux_lfo >= 0.5f)
   {
      startADSRAndLFO(0x80, YAC_FALSE/*bNoteOn*/);
   }

   for(sUI i = 0u; i < STSAMPLE_NUM_MODSEQ; i++)
   {
      sSI newModSeqPatch = sSI(mmdst.modseq_patch[i]);
      if(newModSeqPatch < 0)
         newModSeqPatch = 0;
      else if(newModSeqPatch >= STSAMPLE_MAX_MODSEQ_PATCHES)
         newModSeqPatch = STSAMPLE_MAX_MODSEQ_PATCHES - 1;

      if(modseq_patch[i] != sUI(newModSeqPatch))
      {
         // (note) legato-switch to new ModSeq pattern.
         // (note) starts modseq player when previous modseq was NULL.
         modseq_patch[i] = sUI(newModSeqPatch);
         modseq[i].updateModSeq(sample->modseq[i][newModSeqPatch]);
      }

      if(mmdst.retrig_modseq[i] >= 0.5f)
      {
         startModSeq((1u << i), YAC_FALSE/*bNoteOn==bResetMod*/);
      }
   }

   if(mmdst.b_jumptoloop_valid)
   {
      if(num_sample_loops > 1)
      {
         if(mmdst.jumptoloop >= 0.0f)
         {
            // // sSI mmJumpToLoopIdx = sSI((num_sample_loops -1) * mmdst.jumptoloop + 0.5f);
            sSI mmJumpToLoopIdx = sSI(num_sample_loops * mmdst.jumptoloop);
            if(mmJumpToLoopIdx > sSI(num_sample_loops-1))
               mmJumpToLoopIdx = sSI(num_sample_loops-1);

            // Dyac_host_printf("xxx this=%p jumptoloop=%f => loopIdx=%d current_loop_idx=%u\n", this, mmdst.jumptoloop, mmJumpToLoopIdx, current_loop_idx);

            if(mmJumpToLoopIdx != current_loop_idx)
            {
               mod_jumptoloop = mmJumpToLoopIdx;  // Queue (overwrites param modulation, if any)
               if(!b_playing)
               {
                  // Set initial loop (during reallyStartVoice())
                  sF32 *dummy;
                  // // (void)handleEndOfLoop(&current_play_offset, &dummy, sample->b_from_start/*bFromStart*/);
                  (void)handleEndOfLoop(&current_play_offset, &dummy, &dummy, YAC_FALSE/*bFromStart*/, YAC_TRUE/*bAllowCOffReset*/);
               }
               else
               {
                  if(sample->b_jumptoloop_immediately)
                  {
                     // Jump to loop immediately (may cause clicks)

                     // interpolate/de-click
                     looprestart_xfade_frames_left = STSAMPLEVOICE_LOOPRESTART_NUMSTEPS;
                     last_sample_looprestart_l = last_sample_l;
                     last_sample_looprestart_r = last_sample_r;

                     sF32 *dummy;
                     // // (void)handleEndOfLoop(&current_play_offset, &dummy, sample->b_from_start/*bFromStart*/);
                     (void)handleEndOfLoop(&current_play_offset, &dummy, &dummy, YAC_FALSE/*bFromStart*/, YAC_TRUE/*bAllowCOffReset*/);
                     current_play_offset = 0.0;
                  }
                  // else: update when current loop ends
               }
            }
         }
      }
   }

   if(mmdst.b_sampleshift_endofloop_valid)
   {
      // Queue
      if(mmdst.sampleshift_endofloop != mmdst_sampleshift_endofloop_last)
      {
         mod_sampleshift_endofloop = mmdst.sampleshift_endofloop;
         mmdst_sampleshift_endofloop_last = mmdst.sampleshift_endofloop;
      }
   }

   if((NULL != sample->parent_samplebank) && sample->b_liverec)
   {
      if(mmdst.liverec_stop >= 0.5f)
      {
#ifndef LIBSYNERGY_BUILD
         // Stop
         sSI smpIdx = sample->parent_samplebank->_getSampleIdx(sample);
         if(smpIdx >= 0)
         {
            sample_player->stopLiveRecording(sUI(smpIdx), YAC_TRUE/*bIndex*/);
         }
#endif // LIBSYNERGY_BUILD
      }
      if(mmdst.liverec_start >= 0.5f)
      {
#ifndef LIBSYNERGY_BUILD
         // Always restart
         // Dyac_host_printf("xxx mmdst.liverec_start=%f\n", mmdst.liverec_start);
         sSI smpIdx = sample->parent_samplebank->_getSampleIdx(sample);
         if(smpIdx >= 0)
         {
            sample_player->startLiveRecording(sUI(smpIdx), YAC_TRUE/*bIndex*/, YAC_TRUE/*bRestart*/);
         }
#endif // LIBSYNERGY_BUILD
      }
      else if(mmdst.liverec_continue >= 0.5f)
      {
#ifndef LIBSYNERGY_BUILD
         // Start when not already started
         sSI smpIdx = sample->parent_samplebank->_getSampleIdx(sample);
         if(smpIdx >= 0)
         {
            sample_player->startLiveRecording(sUI(smpIdx), YAC_TRUE/*bIndex*/, YAC_FALSE/*bRestart*/);
         }
#endif // LIBSYNERGY_BUILD
      }
   }

   if(mmdst.looprestart >= 0.5f)
   {
      if(b_playing)
      {
         // interpolate/de-click
         looprestart_xfade_frames_left = STSAMPLEVOICE_LOOPRESTART_NUMSTEPS;
         last_sample_looprestart_l = last_sample_l;
         last_sample_looprestart_r = last_sample_r;

         mod_jumptoloop = current_loop_idx;
         sF32 *dummy;
         // // (void)handleEndOfLoop(&current_play_offset, &dummy, sample->b_from_start/*bFromStart*/);
         (void)handleEndOfLoop(&current_play_offset, &dummy, &dummy, YAC_FALSE/*bFromStart*/, YAC_TRUE/*bAllowCOffReset*/);
         current_play_offset = 0.0;
      }
   }

   next_freq += mmdst.freq * 48.0f;
   next_freq += sF32(12.0f * ((mmdst.freq_oct >= 0) ? sSI(mmdst.freq_oct * 4.0f + 0.5f) : sSI(mmdst.freq_oct * 4.0f - 0.5f)));
   next_freq += sF32(7.0f * ((mmdst.freq_fifth >= 0) ? sSI(mmdst.freq_fifth * 4.0f + 0.5f) : sSI(mmdst.freq_fifth * 4.0f - 0.5f)));
   next_freq += sF32(5.0f * ((mmdst.freq_fourth >= 0) ? sSI(mmdst.freq_fourth * 4.0f + 0.5f) : sSI(mmdst.freq_fourth * 4.0f - 0.5f)));
   next_freq += sF32( ((mmdst.freq_semi >= 0) ? sSI(mmdst.freq_semi * 12.0f + 0.5f) : sSI(mmdst.freq_semi * 12.0f - 0.5f)) );
   next_freq += mmdst.freq_fine;

   if(sample->voice_calibration_enable[0/*fine*/])
   {
      next_freq +=
         sample->voice_calibration_amount[0/*fine*/] * sample->voice_calibration
         [0/*fine*/]
         [voice_alloc_idx % sample->voice_calibration_modulo[0/*fine*/]];
   }

   // Freq ADSR
   if(adsr_freq.adsr)
   {
      sF32 a = adsr_freq.adsr->intensity;
      next_freq += a * mmdst.env.freq * sample->freq_env_amt;
   }

   // Volume ADSR
   if(adsr_vol.adsr)
   {
      sF32 a = adsr_vol.adsr->intensity;
      sF32 av = mmdst.env.vol;
      // Dyac_host_printf("xxx nextblockstate: 1a startadsr adsr_vol.current_env=%p elements=%p\n", adsr_vol.current_env, adsr_vol.current_env->elements);
      // yac_host->printf("xxx next_vol=%f adsr_vol.adsr->intensity=%f mmdst.env.vol=%f\n", next_vol, adsr_vol.adsr->intensity, mmdst.env.vol);

      if(av < 0.0f)
         av = 0.0f;
      else if(av > 1.0f)
         av = 1.0f;

      // Dyac_host_printf("xxx next_vol=%f cached_vel_vol=%f adsrintensity=%f av=%f\n", next_vol, cached_vel_vol, a, av);
      next_vol = ((1.0f - a)*next_vol) + (next_vol * a * av); // blend
      // Dyac_host_printf("xxx after adsr next_vol=%f\n", next_vol);
      if(adsr_vol.b_finished)
      {
         if(!queued_noteon.b_valid)
         {
            // Dyac_host_printf("xxx stopVoice 3 queued_noteon.b_valid=%d\n", queued_noteon.b_valid);
            softStopVoice();
         }
      }
   }

   // Dyac_host_printf("xxx nextblockstate: 2 startadsr adsr_vol.current_env=%p elements=%p\n", adsr_vol.current_env, adsr_vol.current_env->elements);

   // Pan ADSR
   if(adsr_pan.adsr)
   {
      sF32 a = adsr_pan.adsr->intensity;
      sF32 av = mmdst.env.pan;
      ////printf("xxx next_pan=%f a=%f av=%f\n", next_pan, a, av);
      next_pan += a * av * sample->pan_env_amt;
   }

   // Aux (Filter) ADSR
   if(adsr_aux.adsr)
   {
      sF32 a = adsr_aux.adsr->intensity;
      next_aux_adsr += a * mmdst.env.aux;

#if 0
      static int xxxAux = 0;
      if(0 == (xxxAux++&127))
      {
         Dyac_host_printf("xxx next_aux_adsr=%f\n", next_aux_adsr);
      }
#endif

   }

   // Freq LFO
   sF32 freqLFOFreqAmt = sample->freq_lfo_amt * sample_player->mod_lfo_freq_amt * mod_lfo_freq_amt * mmdst.freq_lfo_freq_amt;
   // // freqLFOFreqAmt = sRANGE(freqLFOFreqAmt, 0.0f, 1.0f);
   next_freq += mmdst.lfo.freq * freqLFOFreqAmt;

   // Volume LFO
   sF32 volLFOVolAmt = sample->vol_lfo_amt * sample_player->mod_lfo_vol_amt * mod_lfo_vol_amt * mmdst.vol_lfo_vol_amt;
   // // volLFOVolAmt = sRANGE(volLFOVolAmt, 0.0f, 1.0f);
   next_vol += next_vol * mmdst.lfo.vol * volLFOVolAmt;

   // Volume modmatrix modulation
   // Dyac_host_printf("xxx next_vol=%f mmdst.vol=%f\n", next_vol, mmdst.vol);
   next_vol += next_vol * mmdst.vol;

   // Pan LFO
   sF32 panLFOPanAmt = sample->pan_lfo_amt * sample_player->mod_lfo_pan_amt * mod_lfo_pan_amt * mmdst.pan_lfo_pan_amt;
   // // panLFOPanAmt = sRANGE(panLFOPanAmt, 0.0f, 1.0f);
   next_pan += mmdst.lfo.pan * panLFOPanAmt;

   // Pan modmatrix modulation
   next_pan += mmdst.pan;

   // Aux LFO
   next_aux_lfo += mmdst.lfo.aux;

   // Volume block ramping
   // if(volramp_fade_vol < 1.0f)
   //    Dyac_host_printf("xxx volramp_fade_vol=%f\n", volramp_fade_vol);
   next_vol *= volramp_fade_vol;

   // Clip volume
   if(!sample->b_enabled)
   {
      next_vol = 0.0f; // sample is muted
   }
   else if(next_vol < 0.0f)
   {
      next_vol = 0.0f;
   }
   else if(next_vol > 16.0f)  // [28Jul2019] clip to 16.0 instead of 1.0
   {
      next_vol = 16.0f;
   }

   // Beat frequency
   sF32 beatFreq = sample->beat_frequency + mmdst.freq_beat;
   if(Dfltnonzero(beatFreq))
   {
      sF32 beatDetune = beatFreq / powf(2.0f, (next_freq - 60.0f) / 12.0f);
      next_freq += (beatDetune * 0.01f);
   }

   // Clip frequency
   if(next_freq < 0.00001f)
   {
      next_freq = 0.00001f;
   }

   // // if(current_freq != next_freq)
   // // {
       // // Dyac_host_printf("xxx next_freq=%f\n", next_freq);
   // // }

   // Clip panning
   if(next_pan > 1.0f)
   {
      next_pan = 1.0f;
   }
   else if(next_pan < -1.0f)
   {
      next_pan = -1.0f;
   }

   if(!sample->b_lerp_vol)
      current_vol = next_vol;

   if(!sample->b_lerp_freq)
      current_freq = next_freq;

   // Notify plugins
   {
      sF32 nextFreqHz = -1.0f;
      sF32 nextNote = next_freq;
      for(sUI pluginIdx = 0u; pluginIdx < STSAMPLE_NUM_PLUGINS; pluginIdx++)
      {
         if(0u == (sample->plugin_mask_vm & (1u << pluginIdx)))  // else: prepare/render already done in processPluginsVM()
         {
            st_plugin_cache_entry_t *pce = plugin_cache_entries[pluginIdx];
            if(NULL != pce)
            {
               if(NULL != pce->voice->info->prepare_block)
               {
                  if(nextFreqHz < 0.0f)
                  {
                     nextFreqHz = noteToFreq(next_freq) * liverec_osc_pitch_factor;
                     if(mmdst.note_2 > 0.0f)
                     {
                        nextFreqHz += noteToFreq(mmdst.note_2*127.0f + mmdst.freq_2) - noteToFreq(5*12/*C-5*/);
                        nextNote = FreqToNote(nextFreqHz, getCurrentFreqTableMM());
                     }
                     else if(mmdst.freq_2 != 0.0f)
                     {
                        nextFreqHz += noteToFreq((5.0f*12.0f)/*C-5*/ + mmdst.freq_2) - noteToFreq(5.0f*12.0f/*C-5*/);
                        nextNote = FreqToNote(nextFreqHz, getCurrentFreqTableMM());
                     }
                  }

                  pce->voice->info->prepare_block(pce->voice,
                                                  prepareNumFrames,
                                                  nextFreqHz,
                                                  nextNote,
                                                  next_vol,
                                                  next_pan
                                                  );
               }
            }
         }
      }
   }

   // Calc filter params
   sF32 fltCutOff = sample->filter_cutoff + mod_filter_cutoff + sample_player->mod_filter_cutoff + mmdst.flt_cutoff;

   if(sample->voice_calibration_enable[3/*flt cutoff*/])
   {
      fltCutOff +=
         sample->voice_calibration_amount[3/*flt cutoff*/] * sample->voice_calibration
         [3/*flt cutoff*/]
         [voice_alloc_idx % sample->voice_calibration_modulo[3/*flt cutoff*/]];
   }

   fltCutOff += next_aux_adsr * (sample->filter_aux_env_amount + (sample->filter_aux_env_velocity_amount * queued_noteon._vel));
   sF32 auxLFOFltAmt = sample->filter_aux_lfo_amount * sample_player->mod_lfo_aux_flt_amt * mod_lfo_aux_flt_amt * mmdst.aux_lfo_flt_amt;
   // // auxLFOFltAmt = sRANGE(auxLFOFltAmt, 0.0f, 1.0f);
   fltCutOff += next_aux_lfo * auxLFOFltAmt;

   fltCutOff += ((next_freq - 64.0f) / 127.0f) * sample->filter_keyboard_amount;

   sF32 fltPan = (sample->filter_pan + mod_filter_pan + sample_player->mod_filter_pan) + mmdst.flt_pan;
   sF32 fltCutOffL = fltCutOff - fltPan;
   sF32 fltCutOffR = fltCutOff + fltPan;
   sF32 fltRes = sample->filter_resonance + mod_filter_resonance + sample_player->mod_filter_resonance + mmdst.flt_res;
   fltRes = sMIN(1.0f, sMAX(fltRes, 0.0f));
   sF32 fltGainDb = sample->filter_eq_gain;
#define Dcutoff(f) f = normToCutOffFreq(f)

   // for dual filter LPF/HPF/BPF/BRF
   sF32 fltOffset = sample->filter_offset + mod_filter_offset + sample_player->mod_filter_offset + mmdst.flt_offset;

   switch(filter_type)
   {
      default:
      case STSAMPLE_FLT_TYPE_NONE:
         break;

         // Single Filter Configurations:
      case STSAMPLE_FLT_TYPE_LP:
      case STSAMPLE_FLT_TYPE_HP:
      case STSAMPLE_FLT_TYPE_BP:
      case STSAMPLE_FLT_TYPE_BR:
      case STSAMPLE_FLT_TYPE_PEQ:
      case STSAMPLE_FLT_TYPE_LSH:
      case STSAMPLE_FLT_TYPE_HSH:
      {
         sSI type;

         switch(filter_type)
         {
            default:
            case STSAMPLE_FLT_TYPE_LP:
               type = StSampleVoiceBiquad2::LPF;
               break;

            case STSAMPLE_FLT_TYPE_HP:
               type = StSampleVoiceBiquad2::HPF;
               break;

            case STSAMPLE_FLT_TYPE_BP:
               type = StSampleVoiceBiquad2::BPF;
               break;

            case STSAMPLE_FLT_TYPE_BR:
               type = StSampleVoiceBiquad2::BRF;
               break;

            case STSAMPLE_FLT_TYPE_PEQ:
               type = StSampleVoiceBiquad2::PEQ;
               break;

            case STSAMPLE_FLT_TYPE_LSH:
               type = StSampleVoiceBiquad2::LSH;
               break;

            case STSAMPLE_FLT_TYPE_HSH:
               type = StSampleVoiceBiquad2::HSH;
               break;
         }

         Dcutoff(fltCutOffL);
         Dcutoff(fltCutOffR);

         // Dyac_host_printf("xxx type=%d fltGainDb=%f fltCutOffL=%f fltRes=%f\n", type, fltGainDb, fltCutOffL, fltRes);

         biquad_b_1[0].calcParams(float_block_size,
                                  type,
                                  fltGainDb,
                                  fltCutOffL,
                                  fltRes
                                  );
         biquad_b_1[1].calcParams(float_block_size,
                                  type,
                                  fltGainDb,
                                  fltCutOffR,
                                  fltRes
                                  );
      }
      break;

         // Dual Filter Configurations:
      case STSAMPLE_FLT_TYPE_LPLP:
      case STSAMPLE_FLT_TYPE_LPHP:
      case STSAMPLE_FLT_TYPE_LPBP:
      case STSAMPLE_FLT_TYPE_LPBR:
      case STSAMPLE_FLT_TYPE_HPLP:
      case STSAMPLE_FLT_TYPE_HPHP:
      case STSAMPLE_FLT_TYPE_HPBP:
      case STSAMPLE_FLT_TYPE_HPBR:
      case STSAMPLE_FLT_TYPE_BPLP:
      case STSAMPLE_FLT_TYPE_BPHP:
      case STSAMPLE_FLT_TYPE_BPBP:
      case STSAMPLE_FLT_TYPE_BPBR:
      case STSAMPLE_FLT_TYPE_BRLP:
      case STSAMPLE_FLT_TYPE_BRHP:
      case STSAMPLE_FLT_TYPE_BRBP:
      case STSAMPLE_FLT_TYPE_BRBR:
      case STSAMPLE_FLT_TYPE_PEQ2:
      case STSAMPLE_FLT_TYPE_LSH2:
      case STSAMPLE_FLT_TYPE_HSH2:
      {
         sSI type1;
         sSI type2;
         switch(filter_type)
         {
            default:
            case STSAMPLE_FLT_TYPE_LPLP:
               type1 = StSampleVoiceBiquad2::LPF;
               type2 = StSampleVoiceBiquad2::LPF;
               break;
            case STSAMPLE_FLT_TYPE_LPHP:
               type1 = StSampleVoiceBiquad2::LPF;
               type2 = StSampleVoiceBiquad2::HPF;
               break;
            case STSAMPLE_FLT_TYPE_LPBP:
               type1 = StSampleVoiceBiquad2::LPF;
               type2 = StSampleVoiceBiquad2::BPF;
               break;
            case STSAMPLE_FLT_TYPE_LPBR:
               type1 = StSampleVoiceBiquad2::LPF;
               type2 = StSampleVoiceBiquad2::BRF;
               break;

            case STSAMPLE_FLT_TYPE_HPLP:
               type1 = StSampleVoiceBiquad2::HPF;
               type2 = StSampleVoiceBiquad2::LPF;
               break;
            case STSAMPLE_FLT_TYPE_HPHP:
               type1 = StSampleVoiceBiquad2::HPF;
               type2 = StSampleVoiceBiquad2::HPF;
               break;
            case STSAMPLE_FLT_TYPE_HPBP:
               type1 = StSampleVoiceBiquad2::HPF;
               type2 = StSampleVoiceBiquad2::BPF;
               break;
            case STSAMPLE_FLT_TYPE_HPBR:
               type1 = StSampleVoiceBiquad2::HPF;
               type2 = StSampleVoiceBiquad2::BRF;
               break;

            case STSAMPLE_FLT_TYPE_BPLP:
               type1 = StSampleVoiceBiquad2::BPF;
               type2 = StSampleVoiceBiquad2::LPF;
               break;
            case STSAMPLE_FLT_TYPE_BPHP:
               type1 = StSampleVoiceBiquad2::BPF;
               type2 = StSampleVoiceBiquad2::HPF;
               break;
            case STSAMPLE_FLT_TYPE_BPBP:
               type1 = StSampleVoiceBiquad2::BPF;
               type2 = StSampleVoiceBiquad2::BPF;
               break;
            case STSAMPLE_FLT_TYPE_BPBR:
               type1 = StSampleVoiceBiquad2::BPF;
               type2 = StSampleVoiceBiquad2::BRF;
               break;

            case STSAMPLE_FLT_TYPE_BRLP:
               type1 = StSampleVoiceBiquad2::BRF;
               type2 = StSampleVoiceBiquad2::LPF;
               break;
            case STSAMPLE_FLT_TYPE_BRHP:
               type1 = StSampleVoiceBiquad2::BRF;
               type2 = StSampleVoiceBiquad2::HPF;
               break;
            case STSAMPLE_FLT_TYPE_BRBP:
               type1 = StSampleVoiceBiquad2::BRF;
               type2 = StSampleVoiceBiquad2::BPF;
               break;
            case STSAMPLE_FLT_TYPE_BRBR:
               type1 = StSampleVoiceBiquad2::BRF;
               type2 = StSampleVoiceBiquad2::BRF;
               break;

            case STSAMPLE_FLT_TYPE_PEQ2:
               type1 = StSampleVoiceBiquad2::PEQ;
               type2 = StSampleVoiceBiquad2::PEQ;
               fltOffset = 0.0f;
               break;
            case STSAMPLE_FLT_TYPE_LSH2:
               type1 = StSampleVoiceBiquad2::LSH;
               type2 = StSampleVoiceBiquad2::LSH;
               fltOffset = 0.0f;
               break;
            case STSAMPLE_FLT_TYPE_HSH2:
               type1 = StSampleVoiceBiquad2::HSH;
               type2 = StSampleVoiceBiquad2::HSH;
               fltOffset = 0.0f;
               break;
         }
         sF32 fltCutOffL_1 = fltCutOffL + fltOffset;
         sF32 fltCutOffL_2 = fltCutOffL - fltOffset;

         sF32 fltCutOffR_1 = fltCutOffR + fltOffset;
         sF32 fltCutOffR_2 = fltCutOffR - fltOffset;

         Dcutoff(fltCutOffL_1);
         Dcutoff(fltCutOffL_2);

         Dcutoff(fltCutOffR_1);
         Dcutoff(fltCutOffR_2);

         // Dyac_host_printf("xxx cutoff l=(%f; %f) r=(%f; %f)  bw=%f\n",
         //                  fltCutOffL_hp,
         //                  fltCutOffL_lp,
         //                  fltCutOffR_hp,
         //                  fltCutOffR_lp,
         //                  fltBandwidth
         //                  );

         biquad_b_1[0].calcParams(float_block_size,
                                  type1,
                                  fltGainDb,
                                  fltCutOffL_1,
                                  fltRes
                                  );

         biquad_b_2[0].calcParams(float_block_size,
                                  type2,
                                  fltGainDb,
                                  fltCutOffL_2,
                                  fltRes
                                  );

         biquad_b_1[1].calcParams(float_block_size,
                                  type1,
                                  fltGainDb,
                                  fltCutOffR_1,
                                  fltRes
                                  );

         biquad_b_2[1].calcParams(float_block_size,
                                  type2,
                                  fltGainDb,
                                  fltCutOffR_2,
                                  fltRes
                                  );
      }
      break;
   } // switch filter_type

   ////Dyac_host_printf("xxx calcNextBlockState: (final) next_vol=%f\n", next_vol);
   ////Dyac_host_printf("xxx calcNextBlockState this=0x%p freq=%f vol=%f pan=%f\n", this, next_freq, next_vol, next_pan);

   replay_ticks++;
}

sF32 StSampleVoice::FreqToNote(sF32 _freq, const sF32 *_freqTable) {
   for(sUI i = 0u; i < 127u; i++)
   {
      sF32 fa = _freqTable[i];
      sF32 fb = _freqTable[i+1];
      if( _freq >= fa && _freq < fb )
      {
         return i + ((_freq - fa) / (fb - fa));
      }
   }
   return 126.99f;
}

sF32 StSampleVoice::_FreqToNote(sF32 _freq, YAC_Object *_freqTable) {
   if(YAC_BCHK(_freqTable, YAC_CLID_FLOATARRAY))
   {
      YAC_CAST_ARG(YAC_FloatArray, ft, _freqTable);

      if(ft->num_elements >= 128u)
      {
         return FreqToNote(_freq, ft->elements);
      }
   }
   return 0.0f;
}

sF32 StSampleVoice::noteToFreq(sF32 _note) {
   sF32 freq;

   const sF32 *freqTbl = getCurrentFreqTableMM();
   if(NULL != freqTbl)
   {
      if(_note >= 0.0f)
      {
         if(_note >= 127.0f)
            _note = 126.9999f;
         sUI intNote = (sUI) _note;
         sF32 frac = _note - (sF32)intNote;
         // // freq = freqTbl[intNote] * (1.0f - frac) + freqTbl[intNote + 1] * frac;
         freq = freqTbl[intNote] + (freqTbl[intNote + 1] - freqTbl[intNote]) * frac;
      }
      else
      {
         _note = - _note;
         if(_note >= 127.0f)
            _note = 126.9999f;
         sUI intNote = (sUI) _note;
         sF32 frac = _note - (sF32)intNote;
         // // freq = - ( freqTbl[intNote] * (1.0f - frac) + freqTbl[intNote + 1] * frac );
         freq = - (freqTbl[intNote] + (freqTbl[intNote + 1] - freqTbl[intNote]) * frac);
      }
   }
   else
   {
      // Should not be reachable
      freq = 440.0f;
   }
   // // freq *= calcLiveRecOscTuningFactor();
   return freq;
}

sF32 StSampleVoice::noteToSampleRate(sF32 _note) {
   sF32 hz = noteToFreq(_note);

   return
      (hz / sample->waveform->base_frequency) *
      ((sample->waveform->sample_rate * sample->sample_rate_ratio) / mix_rate);
}

sF32 StSampleVoice::freqToSampleRate(sF32 _hz) {
   return
      (_hz / sample->waveform->base_frequency) *
      ((sample->waveform->sample_rate * sample->sample_rate_ratio) / mix_rate);
}

sF32 StSampleVoice::normToCutOffFreq(sF32 _norm) {
   const sF32 *freqTbl = getCurrentFreqTableMM();
   if(NULL != freqTbl)
   {
      _norm *= 127.0f;
      _norm = sMIN(127.0f, sMAX(_norm, 0.0f));

      sUI intNote = (sUI) _norm;
      sF32 frac = _norm - (sF32)intNote;
      // // sF32 freq = freqTbl[intNote] * (1.0f - frac) + freqTbl[intNote + 1] * frac;
      sF32 freq = freqTbl[intNote] + (freqTbl[intNote + 1] - freqTbl[intNote]) * frac;
      // // freq *= calcLiveRecOscTuningFactor();
      freq /= mix_rate;
      return freq;
   }
   return 0.0f;
}

// // sF32 StSampleVoice::calcLiveRecOscTuningFactor(void) {
// //    sF32 r = 1.0f;
// //    if(sample->hasLiveRecAdaptiveLoopSize())
// //    {
// //       if(current_orig_sample_len > 0u)
// //       {
// //          r = sF32(sample->liverec_override_loop_len) / sF32(current_orig_sample_len);
// //          // Dyac_host_printf("xxx calcLiveRecOscTuningFactor=%f\n", r);
// //       }
// //    }
// //    return r;
// // }

void StSampleVoice::_setVol(sF32 _vol) {
   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._vol = _vol;
   }
   else
   {
      // Modify voice
      mod_vol = _vol;
   }
}

sF32 StSampleVoice::_getVol(void) {
   return mod_vol;
}

void StSampleVoice::_setVol2(sF32 _vol) {
   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._vol2 = _vol;
   }
   else
   {
      // Modify voice
      mod_vol2 = _vol;
   }
}

sF32 StSampleVoice::_getVol2(void) {
   return mod_vol2;
}

void StSampleVoice::_setPan(sF32 _pan) {
   if(_pan > 1.0f)
      _pan = 1.0f;
   else if(_pan < -1.0f)
      _pan = -1.0f;

   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._pan = _pan;
   }
   else
   {
      // Modify voice
      mod_pan = _pan;
   }
}

sF32 StSampleVoice::_getPan(void) {
   return mod_pan;
}

void StSampleVoice::_setPan2(sF32 _pan) {
   if(_pan > 1.0f)
      _pan = 1.0f;
   else if(_pan < -1.0f)
      _pan = -1.0f;

   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._pan2 = _pan;
   }
   else
   {
      // Modify voice
      mod_pan2 = _pan;
   }
}

sF32 StSampleVoice::_getPan2(void) {
   return mod_pan2;
}

void StSampleVoice::_setFreq(sF32 _freq) {
   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._freq = _freq;
   }
   else
   {
      // Modify voice
      mod_freq = _freq;
   }
}

sF32 StSampleVoice::_getFreq(void) {
   return mod_freq;
}

void StSampleVoice::_setFreq2(sF32 _freq) {
   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._freq2 = _freq;
   }
   else
   {
      // Modify voice
      mod_freq2 = _freq;
   }
}

sF32 StSampleVoice::_getFreq2(void) {
   return mod_freq2;
}

void StSampleVoice::_setMod(sF32 _mod) {
   if(_mod < 0.0f)
      _mod = 0.0f;
   else if(_mod > 1.0f)
      _mod = 1.0f;

   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._mod = _mod;
   }
   else
   {
      // Modify voice
      mod_mod = _mod;
   }
}

sF32 StSampleVoice::_getMod(void) {
   return mod_mod;
}

void StSampleVoice::_setTimestretch(sF32 _ts) {

   // // if(_ts < 0.0f)
   // //    _ts = 0.0f;
   // // else if(_ts > 1.0f)
   // //    _ts = 1.0f;

   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._timestretch = _ts;
   }
   else
   {
      // Modify voice
      mod_timestretch = _ts;
   }
}

sF32 StSampleVoice::_getTimestretch(void) {
   return mod_timestretch;
}

void StSampleVoice::_setTimestretchBend(sF32 _bend) {

   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._timestretch_bend = _bend;
   }
   else
   {
      // Modify voice
      mod_timestretch_bend = _bend;
   }
}

sF32 StSampleVoice::_getTimestretchBend(void) {
   return mod_timestretch_bend;
}

void StSampleVoice::_setSampleOff(sF32 _so) {

   // if(_so < 0.0f)
   //    _so = 0.0f;
   // else if(_so > 1.0f)
   //    _so = 1.0f;

   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._sampleoff = _so;
   }
   else
   {
      // Modify voice
      if(!sample->b_sampleoffnoteon)
      {
         mod_sampleoff = _so;
      }
   }
}

void StSampleVoice::_setSampleOffMs(sF32 _ms) {
   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._sampleoff_ms = _ms;
   }
}

sF32 StSampleVoice::_getSampleOff(void) {
   return mod_sampleoff;
}

void StSampleVoice::_setSampleOff2(sF32 _so) {
   // if(_so < 0.0f)
   //    _so = 0.0f;
   // else if(_so > 1.0f)
   //    _so = 1.0f;

   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._sampleoff2 = _so;
   }
   else
   {
      // Modify voice
      mod_sampleoff2 = _so;
   }
}

sF32 StSampleVoice::_getSampleOff2(void) {
   return mod_sampleoff2;
}

void StSampleVoice::_setCycleLen(sF32 _cl) {
   // // if(_cl < 0.0f)
   // //    _cl = 0.0f;
   // // else if(_cl > 1.0f)
   // //    _cl = 1.0f;

   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._cyclelen = _cl;
   }
   else
   {
      // Modify voice
      mod_cyclelen = _cl;
   }
}

sF32 StSampleVoice::_getCycleLen(void) {
   return mod_cyclelen;
}

void StSampleVoice::_setSampleShift(sF32 _sh) {
   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._sampleshift = _sh;
   }
   else
   {
      // Modify voice
      mod_sampleshift = _sh;
   }
}

sF32 StSampleVoice::_getSampleShift(void) {
   return mod_sampleshift;
}

void StSampleVoice::_setSampleShiftEndOfLoop(sF32 _sh) {
   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._sampleshift_endofloop = _sh;
   }
   else
   {
      // Modify voice
      mod_sampleshift_endofloop = _sh;
   }
}

sF32 StSampleVoice::_getSampleShiftEndOfLoop(void) {
   return mod_sampleshift_endofloop;
}

void StSampleVoice::_setAltVolume(sF32 _av) {
   // // if(_av < 0.0f)
   // //    _av = 0.0f;
   // // else if(_av > 1.0f)
   // //    _av = 1.0f;

   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._altvolume = _av;
   }
   else
   {
      // Modify voice
      mod_altvolume = _av;
   }
}

sF32 StSampleVoice::_getAltVolume(void) {
   return mod_altvolume;
}

void StSampleVoice::_setRetrigMask(sUI _mask) {
   // (Re-)trigger envelopes and LFOs:
   //   - 0x01: reset freq envelope
   //   - 0x02: reset volume envelope
   //   - 0x04: reset pan envelope
   //   - 0x08: reset aux envelope
   //   - 0x10: reset freq LFO
   //   - 0x20: reset volume LFO
   //   - 0x40: reset pan LFO
   //   - 0x80: reset aux LFO

   if(queued_noteon.b_valid)
   {
      ////printf("xxx b_valid: setRetrigMask=%02x\n", _mask);
      queued_noteon._retrigmask = _mask;
   }
   else
   {
      // Dyac_host_printf("xxx voice running setRetrigMask=%02x\n", _mask);
      startADSRAndLFO(_mask, false);
   }
}

void StSampleVoice::_setLFOFreqSpd(sF32 _spd) {
   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._lfofreqspd = _spd;
   }
   else
   {
      // Modify voice
      lfo_freq.mod_speed = _spd;
   }
}

void StSampleVoice::_setLFOFreqLvl(sF32 _lvl) {
   // // if(_lvl < 0.0f)
   // //    _lvl = 0.0f;
   // // else if(_lvl > 1.0f)
   // //    _lvl = 1.0f;

   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._lfofreqlvl = _lvl;
   }
   else
   {
      // Modify voice
      lfo_freq.mod_level = _lvl;
   }
}

void StSampleVoice::_setLFOFreqAmt(sF32 _amt) {
   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._lfofreqamt = _amt;
   }
   else
   {
      // Modify voice
      mod_lfo_freq_amt = _amt;
   }
}

void StSampleVoice::_setLFOVolSpd(sF32 _spd) {
   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._lfovolspd = _spd;
   }
   else
   {
      // Modify voice
      lfo_vol.mod_speed = _spd;
   }
}

void StSampleVoice::_setLFOVolLvl(sF32 _lvl) {
   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._lfovollvl = _lvl;
   }
   else
   {
      // Modify voice
      lfo_vol.mod_level = _lvl;
   }
}

void StSampleVoice::_setLFOVolAmt(sF32 _amt) {
   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._lfovolamt = _amt;
   }
   else
   {
      // Modify voice
      mod_lfo_vol_amt = _amt;
   }
}

void StSampleVoice::_setLFOPanSpd(sF32 _spd) {
   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._lfopanspd = _spd;
   }
   else
   {
      // Modify voice
      lfo_pan.mod_speed = _spd;
   }
}

void StSampleVoice::_setLFOPanLvl(sF32 _lvl) {
   // // if(_lvl < 0.0f)
   // //    _lvl = 0.0f;
   // // else if(_lvl > 1.0f)
   // //    _lvl = 1.0f;

   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._lfopanlvl = _lvl;
   }
   else
   {
      // Modify voice
      lfo_pan.mod_level = _lvl;
   }
}

void StSampleVoice::_setLFOPanAmt(sF32 _amt) {
   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._lfopanamt = _amt;
   }
   else
   {
      // Modify voice
      mod_lfo_pan_amt = _amt;
   }
}

void StSampleVoice::_setLFOAuxSpd(sF32 _spd) {
   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._lfoauxspd = _spd;
   }
   else
   {
      // Modify voice
      lfo_aux.mod_speed = _spd;
   }
}

void StSampleVoice::_setLFOAuxLvl(sF32 _lvl) {

   // // if(_lvl < 0.0f)
   // //    _lvl = 0.0f;
   // // else if(_lvl > 1.0f)
   // //    _lvl = 1.0f;

   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._lfoauxlvl = _lvl;
   }
   else
   {
      // Modify voice
      lfo_aux.mod_level = _lvl;
   }
}

void StSampleVoice::_setLFOAuxFltAmt(sF32 _amt) {
   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._lfoauxfltamt = _amt;
   }
   else
   {
      // Modify voice
      mod_lfo_aux_flt_amt = _amt;
   }
}

void StSampleVoice::_setADSRFreqSpd(sF32 _spd) {
   if(_spd < 0.00001f)
   {
      _spd  = 0.00001f;
   }

   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._adsrfreqspd = _spd;
   }
   else
   {
      // Modify voice
      adsr_freq.mod_speed = _spd;
   }
}

void StSampleVoice::_setADSRFreqLvl(sF32 _lvl) {
   if(_lvl < 0.0f)
      _lvl = 0.0f;
   else if(_lvl > 1.0f)
      _lvl = 1.0f;

   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._adsrfreqlvl = _lvl;
   }
   else
   {
      // Modify voice
      adsr_freq.mod_level = _lvl;
   }
}

void StSampleVoice::_setADSRFreqInt(sF32 _int) {
   // // if(_int < 0.0f)
   // //    _int = 0.0f;
   // // else if(_int > 1.0f)
   // //    _int = 1.0f;

   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._adsrfreqint = _int;
   }
   else
   {
      // Modify voice
      adsr_freq.mod_intensity = _int;
   }
}

void StSampleVoice::_setADSRVolSpd(sF32 _spd) {
   if(_spd < 0.00001f)
      _spd  = 0.00001f;

   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._adsrvolspd = _spd;
   }
   else
   {
      // Modify voice
      adsr_vol.mod_speed = _spd;
   }
}

void StSampleVoice::_setADSRVolLvl(sF32 _lvl) {
   // // if(_lvl < 0.0f)
   // //    _lvl = 0.0f;
   // // else if(_lvl > 1.0f)
   // //    _lvl = 1.0f;

   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._adsrvollvl = _lvl;
   }
   else
   {
      // Modify voice
      adsr_vol.mod_level = _lvl;
   }
}

void StSampleVoice::_setADSRVolInt(sF32 _int) {
   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._adsrvolint = _int;
   }
   else
   {
      // Modify voice
      adsr_vol.mod_intensity = _int;
   }
}

void StSampleVoice::_setADSRPanSpd(sF32 _spd) {
   if(_spd < 0.00001f)
      _spd  = 0.00001f;

   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._adsrpanspd = _spd;
   }
   else
   {
      // Modify voice
      adsr_pan.mod_speed = _spd;
   }
}

void StSampleVoice::_setADSRPanLvl(sF32 _lvl) {
   // // if(_lvl < 0.0f)
   // //    _lvl = 0.0f;
   // // else if(_lvl > 1.0f)
   // //    _lvl = 1.0f;

   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._adsrpanlvl = _lvl;
   }
   else
   {
      // Modify voice
      adsr_pan.mod_level = _lvl;
   }
}

void StSampleVoice::_setADSRPanInt(sF32 _int) {
   // // if(_int < 0.0f)
   // //    _int = 0.0f;
   // // else if(_int > 1.0f)
   // //    _int = 1.0f;

   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._adsrpanint = _int;
   }
   else
   {
      // Modify voice
      adsr_pan.mod_intensity = _int;
   }
}

void StSampleVoice::_setADSRAuxSpd(sF32 _spd) {
   if(_spd < 0.00001f)
      _spd  = 0.00001f;

   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._adsrauxspd = _spd;
   }
   else
   {
      // Modify voice
      adsr_aux.mod_speed = _spd;
   }
}

void StSampleVoice::_setADSRAuxLvl(sF32 _lvl) {
   // // if(_lvl < 0.0f)
   // //    _lvl = 0.0f;
   // // else if(_lvl > 1.0f)
   // //    _lvl = 1.0f;

   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._adsrauxlvl = _lvl;
   }
   else
   {
      // Modify voice
      adsr_aux.mod_level = _lvl;
   }
}

void StSampleVoice::_setADSRAuxInt(sF32 _int) {
   if(_int < 0.0f)
      _int = 0.0f;
   else if(_int > 1.0f)
      _int = 1.0f;

   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._adsrauxint = _int;
   }
   else
   {
      // Modify voice
      adsr_aux.mod_intensity = _int;
   }
}

void StSampleVoice::_setAFreqSpd(sF32 _spd) {
   if(_spd < 0.00001f)
      _spd  = 0.00001f;

   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._afreqspd = _spd;
   }
   else
   {
      // Modify voice
      adsr_freq.mod_aspeed = _spd;
   }
}

void StSampleVoice::_setSFreqSpd(sF32 _spd) {
   if(_spd < 0.00001f)
      _spd  = 0.00001f;

   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._sfreqspd = _spd;
   }
   else
   {
      // Modify voice
      adsr_freq.mod_sspeed = _spd;
   }
}

void StSampleVoice::_setRFreqSpd(sF32 _spd) {
   if(_spd < 0.00001f)
      _spd  = 0.00001f;

   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._rfreqspd = _spd;
   }
   else
   {
      // Modify voice
      adsr_freq.mod_rspeed = _spd;
   }
}

void StSampleVoice::_setAVolSpd(sF32 _spd) {
   if(_spd < 0.00001f)
      _spd  = 0.00001f;

   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._avolspd = _spd;
   }
   else
   {
      // Modify voice
      adsr_vol.mod_aspeed = _spd;
   }
}

void StSampleVoice::_setSVolSpd(sF32 _spd) {
   if(_spd < 0.00001f)
      _spd  = 0.00001f;

   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._svolspd = _spd;
   }
   else
   {
      // Modify voice
      adsr_vol.mod_sspeed = _spd;
   }
}

void StSampleVoice::_setRVolSpd(sF32 _spd) {
   if(_spd < 0.00001f)
      _spd  = 0.00001f;

   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._rvolspd = _spd;
   }
   else
   {
      // Modify voice
      adsr_vol.mod_rspeed = _spd;
   }
}

void StSampleVoice::_setAPanSpd(sF32 _spd) {
   if(_spd < 0.00001f)
      _spd  = 0.00001f;

   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._apanspd = _spd;
   }
   else
   {
      // Modify voice
      adsr_pan.mod_aspeed = _spd;
   }
}

void StSampleVoice::_setSPanSpd(sF32 _spd) {
   if(_spd < 0.00001f)
      _spd  = 0.00001f;

   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._spanspd = _spd;
   }
   else
   {
      // Modify voice
      adsr_pan.mod_sspeed = _spd;
   }
}

void StSampleVoice::_setRPanSpd(sF32 _spd) {
   if(_spd < 0.00001f)
      _spd  = 0.00001f;

   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._rpanspd = _spd;
   }
   else
   {
      // Modify voice
      adsr_pan.mod_rspeed = _spd;
   }
}

void StSampleVoice::_setAAuxSpd(sF32 _spd) {
   if(_spd < 0.00001f)
      _spd  = 0.00001f;

   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._aauxspd = _spd;
   }
   else
   {
      // Modify voice
      adsr_aux.mod_aspeed = _spd;
   }
}

void StSampleVoice::_setSAuxSpd(sF32 _spd) {
   if(_spd < 0.00001f)
      _spd  = 0.00001f;

   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._sauxspd = _spd;
   }
   else
   {
      // Modify voice
      adsr_aux.mod_sspeed = _spd;
   }
}

void StSampleVoice::_setRAuxSpd(sF32 _spd) {
   if(_spd < 0.00001f)
      _spd  = 0.00001f;

   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._rauxspd = _spd;
   }
   else
   {
      // Modify voice
      adsr_aux.mod_rspeed = _spd;
   }
}

void StSampleVoice::startADSRAndLFO(sUI _mask, sBool _bNoteOn) {

   // Dyac_host_printf("xxx startADSRAndLFO: mask=0x%08x bNoteOn=%d b_release=%d\n", _mask, _bNoteOn, b_release);

   if(_mask & 0x01u)
   {
      adsr_freq.startADSR(sample->adsr_freq, 0.0f, _bNoteOn/*bResetMod*/, b_release);
   }
   else if(_bNoteOn)
   {
      adsr_freq .stopADSR();
   }

   if(_mask & 0x02u)
   {
      adsr_vol .startADSR(sample->adsr_vol, 1.0f, _bNoteOn/*bResetMod*/, b_release);
   }
   else if(_bNoteOn)
   {
      adsr_vol .stopADSR();
   }

   if(_mask & 0x04u)
   {
      adsr_pan .startADSR(sample->adsr_pan, 0.0f, _bNoteOn/*bResetMod*/, b_release);
   }
   else if(_bNoteOn)
   {
      adsr_pan .stopADSR();
   }

   if(_mask & 0x08u)
   {
      adsr_aux .startADSR(sample->adsr_aux, 0.0f, _bNoteOn/*bResetMod*/, b_release);
   }
   else if(_bNoteOn)
   {
      adsr_aux .stopADSR();
   }


   if(_mask & 0x10)
   {
      lfo_freq.startLFO(sample->lfo_freq, _bNoteOn/*bResetMod*/);
      sample->lfo_freq_global.startLFO(sample->lfo_freq, _bNoteOn/*bResetMod*/);
   }
   else if(_bNoteOn)
   {
      lfo_freq .stopLFO();
      sample->lfo_freq_global.stopLFO();
   }

   if(_mask & 0x20u)
   {
      lfo_vol.startLFO(sample->lfo_vol, _bNoteOn/*bResetMod*/);
      sample->lfo_vol_global.startLFO(sample->lfo_vol, _bNoteOn/*bResetMod*/);
   }
   else if(_bNoteOn)
   {
      lfo_vol.stopLFO();
      sample->lfo_vol_global.stopLFO();
   }

   if(_mask & 0x40u)
   {
      lfo_pan.startLFO(sample->lfo_pan, _bNoteOn/*bResetMod*/);
      sample->lfo_pan_global.startLFO(sample->lfo_pan, _bNoteOn/*bResetMod*/);
   }
   else if(_bNoteOn)
   {
      lfo_pan.stopLFO();
      sample->lfo_pan_global.stopLFO();
   }

   if(_mask & 0x80u)
   {
      lfo_aux.startLFO(sample->lfo_aux, _bNoteOn/*bResetMod*/);
      sample->lfo_aux_global.startLFO(sample->lfo_aux, _bNoteOn/*bResetMod*/);
   }
   else if(_bNoteOn)
   {
      lfo_aux.stopLFO();
      sample->lfo_aux_global.stopLFO();
   }
}

void StSampleVoice::_setModSeqRetrigMask(sUI _mask) {
   // (Re-)trigger mod sequencers:
   //   - 0x01: reset mod seq 1
   //   - 0x02: reset mod seq 2
   //   - 0x04: reset mod seq 3
   //   - 0x08: reset mod seq 4

   if(queued_noteon.b_valid)
   {
      ////printf("xxx b_valid: setRetrigMask=%02x\n", _mask);
      queued_noteon._modseq_retrigmask = _mask;
   }
   else
   {
      // Dyac_host_printf("xxx voice running setRetrigMask=%02x\n", _mask);
      startModSeq(_mask, false);
   }
}

void StSampleVoice::_setModSeqSpeed(sUI _idx, sF32 _spd) {
   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._modseq_speed[_idx] = _spd;
   }
   else
   {
      // Modify voice
      modseq[_idx].mod_speed = _spd;
   }
}

void StSampleVoice::_setModSeqLevel(sUI _idx, sF32 _lvl) {
   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._modseq_level[_idx] = _lvl;
   }
   else
   {
      // Modify voice
      modseq[_idx].mod_level = _lvl;
   }
}

void StSampleVoice::_setModSeqNumStepsRel(sUI _idx, sF32 _numStepsRel) {
   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._modseq_numsteps[_idx] = _numStepsRel;
   }
   else
   {
      // Modify voice
      modseq[_idx].mod_numsteps = _numStepsRel;
   }
}

void StSampleVoice::_setModSeqAdvance(sUI _idx, sF32 _advance) {
   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._modseq_advance[_idx] = _advance;
   }
   else
   {
      // Modify voice
      modseq[_idx].mod_advance = _advance;
   }
}

void StSampleVoice::startModSeq(sUI _mask, sBool _bNoteOn) {
   // Dyac_host_printf("xxx startModSeq: mask=0x%02x bNoteOn=%d\n", _mask, _bNoteOn);
   sUI bit = 1u;
   for(sUI i = 0u; i < STSAMPLE_NUM_MODSEQ; i++)
   {
      sUI patchIdx = modseq_patch[i];
      // // if(patchIdx < 0)
      // //    patchIdx = 0;
      // // else if(patchIdx >= STSAMPLE_MAX_MODSEQ_PATCHES)
      // //    patchIdx = STSAMPLE_MAX_MODSEQ_PATCHES - 1;

      if(_mask & bit)
      {
         modseq[i].startModSeq(sample->modseq[i][patchIdx], _bNoteOn/*bResetMod*/, sample_player->bpm_audiotick_scl, (void*)this);
         sample->modseq_global[i].startModSeq(sample->modseq[i][patchIdx], _bNoteOn/*bResetMod*/, sample_player->bpm_audiotick_scl, (void*)this);
      }
      else if(_bNoteOn)
      {
         modseq[i].stopModSeq();
         sample->modseq_global[i].stopModSeq();
      }
      bit <<= 1;
   }
   // Dyac_host_printf("xxx startModSeq: LEAVE\n");
}

void StSampleVoice::_setJumpToLoop(sSI _idx) {
   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._jumptoloop = _idx;
   }
   else
   {
      // Modify voice
      mod_jumptoloop = _idx;
   }
}

void StSampleVoice::_setJumpToLoopRel(sF32 _relIdx) {
   if(num_sample_loops > 0)
   {
      // // sSI idx = sSI(_relIdx * (num_sample_loops-1) + 0.5f);
      if(_relIdx >= 0.0f)
      {
         sSI idx = sSI(num_sample_loops * _relIdx);
         if(idx > sSI(num_sample_loops-1))
            idx = sSI(num_sample_loops-1);

         if(queued_noteon.b_valid)
         {
            // Voice has not been started, yet
            queued_noteon._jumptoloop = idx;
         }
         else
         {
            // Modify voice
            mod_jumptoloop = idx;
         }
      }
   }
}

void StSampleVoice::_setWavepathIndex(sSI _idx) {
   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._wavepathidx = _idx;
   }
   else
   {
      // Modify voice
      queued_wavepath_idx = _idx;
   }
}

void StSampleVoice::handleEndOfLoopOver(const sF32 *_smpDatCur, sUI _frameOffOver, const sF32 **_smpDatNew, sUI *_frameOffNew) const {
   // called by readSample*Int() when reading samples beyond current_sample_len
   //  returns new sample address (left channel) or address of zero_samples (when sample playback has finished)

   sSI nextLoopIdx = -1;

   // default=silence
   *_smpDatNew   = zero_samples;
   *_frameOffNew = 0u;

   if(STSAMPLE_PLAY_MODE_FWDNOLOOP == sample->play_mode)
   {
      return;
   }

   nextLoopIdx = current_loop_idx;

   if(1 == b_timedloop)
   {
      // Timeout loop mode
      if((samples_until_end_of_loopstep > 0) && (-1 == mod_jumptoloop))
      {
         // Repeat same loop again
         *_smpDatNew   = _smpDatCur;
         *_frameOffNew = _frameOffOver % current_sample_len;
         return;
      }
      else
      {
         nextLoopIdx = (-1 != mod_jumptoloop) ? mod_jumptoloop : (current_loop_idx + 1);
      }
   }
   else
   {
      if(-1 != mod_jumptoloop)
         nextLoopIdx = mod_jumptoloop;
      else if(1 == current_loop_countdown)
         nextLoopIdx = (current_loop_idx + 1);
      else if(sample->b_from_start)
         nextLoopIdx = current_loop_idx; // move to looped area
      else if(0 == num_sample_loops)
      {
         // End of sample
         return;
      }
   }

   if(-1 != nextLoopIdx)
   {
      YAC_IntArray *sampleLoops = getCurrentSampleLoops();
      if(NULL != sampleLoops)
      {
         sUI sloff = 3u * sUI(nextLoopIdx);
         if(sloff < sampleLoops->num_elements)
         {
            sUI newOff = *(sUI*)&sampleLoops->elements[sloff + 0];
            sUI newLen = *(sUI*)&sampleLoops->elements[sloff + 1];
            const sF32 *smpDatBase = sample->waveform->sample_data->elements;
            sUI waveNumFrames = sample->waveform->_getNumFrames();

            if(sample->b_liverec)
            {
               if(sample->b_liverec_copy && (STSAMPLE_LIVEREC_OSC_MODE_OFF == sample->liverec_osc_mode))
               {
                  if((NULL != override_smpdat) && (override_smpdat_sz >= waveNumFrames))
                  {
                     // Use current sample front buffer
                     smpDatBase = override_smpdat;
                  }
                  else
                  {
                     // Should not be reachable
                     return;
                  }
               }
               else
               {
                  if(STSAMPLE_LIVEREC_DBLBUF_MODE_OFF != sample->liverec_doublebuffer_mode)
                  {
                     sUI waveNumFramesHalf = (waveNumFrames >> 1);

                     if( (newOff + newLen) <= waveNumFramesHalf )
                     {
                        waveNumFrames = waveNumFramesHalf;

                        sUI liveRecDblBufIdx = (sample->b_liverec_copy ? liverec_copy_doublebuffer_rec_idx : sample->liverec_doublebuffer_rec_idx);

                        if(0u == liveRecDblBufIdx)
                        {
                           // Currently recording to first half => Play second half
                           smpDatBase += (waveNumFrames * sample->waveform->num_channels);
                        }
                     }
                     else
                     {
                        // loop refers to 2nd waveform half and double-buffered liverec is enabled
                        //  => play as if live recording is not enabled
                     }
                  }

                  if(sample->b_liverec_copy && (liverec_copy_loop_offset >= 0))
                  {
                     if(NULL != override_smpdat)
                     {
                        // Dyac_host_printf("xxx b_override_smpdat_copied=%d\n", b_override_smpdat_copied);
                        smpDatBase = override_smpdat;
                        // Dyac_host_printf("xxx override smpDatBase to %p\n", override_smpdat);
                     }
                     else
                     {
                        // should not be reachable
                        return;
                     }
                  }
               }
            } // if sample->b_liverec

            if(newLen > 0u)
            {
               *_smpDatNew   = smpDatBase + (newOff * sample->waveform->num_channels);
               *_frameOffNew = _frameOffOver % newLen;
            }
            else
            {
               // should not be reachable
               *_smpDatNew   = zero_samples;
               *_frameOffNew = 0u;
               return;
            }
         }
         else
         {
            // Invalid loop, End of sample
            return;
         }
      }
      else
      {
         // No sample loops, End of sample
         return;
      }

   } // if nextLoopIdx
}

sBool StSampleVoice::handleEndOfLoop(sF64 *cOff, sF32**smpDat, sF32 **smpDatLRX, sBool _bFromStart, sBool _bAllowCOffReset) {
   //
   // end of sample, begin next loop
   //

   // Dyac_host_printf("xxx handleEndOfLoop: bFromStart=%d bAllowCOffReset=%d current_loop_countdown=%d\n", _bFromStart, _bAllowCOffReset, current_loop_countdown);

   sBool bCycleLenEOL = (queued_cyclelen_eol >= 0.0f);
   if(bCycleLenEOL || _bFromStart/*voice start*/)
   {
      if(bCycleLenEOL)
         cur_cyclelen_eol = queued_cyclelen_eol;
      queued_cyclelen_eol = -1.0f;
      // yac_host->printf("xxx cur_cyclelen_eol=%f\n", cur_cyclelen_eol);
      bCycleLenEOL = bCycleLenEOL && _bAllowCOffReset;
   }

   // Dyac_host_printf("xxx 1 handleEndOfLoop: cOff=%f current_sample_len=%u mod_jumptoloop=%d bFromStart=%d\n", *cOff, current_sample_len, mod_jumptoloop, _bFromStart);
   // printf("xxx smpDat=0x%p\n", smpDat);
   // printf("xxx sample=0x%p\n", sample);

   sBool bNextStep = false;
   b_allow_xfade = true;

#ifdef USE_SAMPLESHIFT_EOL_XFADE_BUF
   sampleshift_eol_xfade_loopstart_frameidx = 0u;
#endif // USE_SAMPLESHIFT_EOL_XFADE_BUF

   if(mod_sampleshift_endofloop >= 0.0f)
   {
      last_sample_sampleshift_eol_l = last_sample_l;
      last_sample_sampleshift_eol_r = last_sample_r;
      sampleshift_eol_xfade_frames_left = VOLRAMP_NUMSTEPS;

      mod_sampleshift = mod_sampleshift_endofloop;
      mod_sampleshift_endofloop = -1.0f;
   }

   if(1 == b_timedloop)
   {
      // Timeout loop mode
      // Dyac_host_printf("xxx handleEndOfLoop: timedloop, samples_until_end_of_loopstep=%d mod_jumptoloop=%d\n", samples_until_end_of_loopstep, mod_jumptoloop);

      if((samples_until_end_of_loopstep > 0) && (-1 == mod_jumptoloop))
      {
         // Repeat same loop again
         calcSmpDat(*smpDat, *smpDatLRX, current_sample_offset, current_orig_sample_len, current_sample_len);
         *cOff = 0;
         return YAC_TRUE;
      }
      else
      {
         // Dyac_host_printf("xxx bNextStep\n");
         bNextStep = 1;
      }
   }
   else
   {
      // Normal loop mode (loop cycle boundary)
      if(bCycleLenEOL)
         *cOff = 0.0;
      else if(current_sample_len)
      {
         while( (*cOff) >=  ((sF64)current_sample_len) )
            (*cOff) -= (sF64) current_sample_len;
      }

      // Dyac_host_printf("xxx handleEndOfLoop: current_loop_countdown=%u\n", current_loop_countdown);

      // Dyac_host_printf("xxx handleEndOfLoop: num_sample_loops=%u\n", num_sample_loops);

      if(current_loop_countdown)
      {
         bNextStep = (0 == --current_loop_countdown);
         if(!bNextStep)
            b_fwd ^= 1;
         else
            b_fwd = 1;
      }
      else if(0 == num_sample_loops)
      {
         // End of sample
         if(!queued_noteon.b_valid)
         {
            ////printf("xxx stopVoice 4\n");
            stopVoice();
         }
         return YAC_FALSE;
      }
      else
      {
         //
         // Loop last loop forever
         //
         if(b_playing)
            b_fwd ^= 1;

         // printf("xxx 2c1 this=0x%p\n", this);
         // Dyac_host_printf("xxx 2c1 sample=0x%p num_sample_loops=%d\n", sample, num_sample_loops);
         // printf("xxx 2c1 sample->sample_loops=0x%p\n", sample->sample_loops);
         // Dyac_host_printf("xxx loop last loop forever: current_loop_idx=%d\n", current_loop_idx);
         sUI sloff = 3u * current_loop_idx;
         YAC_IntArray *sampleLoops = getCurrentSampleLoops();
         current_sample_offset   = *(sUI*)&sampleLoops->elements[sloff + 0];
         current_orig_sample_len = *(sUI*)&sampleLoops->elements[sloff + 1];
         current_loop_countdown  =         sampleLoops->elements[sloff + 2];

         if(YAC_TRUE == _bFromStart)
         {
            // First iteration starts at the beginning of the sample (PT mode)
            if(current_sample_offset > 0u)
            {
               // // current_orig_sample_len += current_sample_offset;
               current_orig_sample_len = current_sample_offset;  // [24Apr2021] play first iteration until inf loop start (avoid crossfade clicks)
               current_sample_offset = 0u;
               b_allow_xfade = false;
            }
         }
         else if(YAC_MAYBE == _bFromStart)
         {
            // First iteration starts at last loop offset (multisamples, samplesynth)
            if(current_loop_idx < (num_sample_loops-1))
            {
               sUI lastLoopOff = *(sUI*)&sampleLoops->elements[sUI(num_sample_loops-1)*3u + 0];
               if(lastLoopOff < current_sample_offset)
               {
                  // current_orig_sample_len += (current_sample_offset - lastLoopOff);
                  current_orig_sample_len = (current_sample_offset - lastLoopOff);  // [24Apr2021] play first iteration until inf loop start (avoid crossfade clicks)
                  current_sample_offset = lastLoopOff;
                  b_allow_xfade = YAC_FALSE;
               }
            }
         }

         if(sample->ui_sync_first_loop_index)
         {
            current_loop_countdown = 0; // forever
         }

         if(b_playing)
         {
            if(YAC_MAYBE == sample->b_liverec_copy)
            {
               if(STSAMPLE_LIVEREC_OSC_MODE_OFF != sample->liverec_osc_mode)
               {
                  // // if(sample->b_liverec_copy) && (STSAMPLE_LIVEREC_OSC_MODE_OFF == sample->liverec_osc_mode))
                  if(NULL != override_smpdat)  // should not be NULL at this point
                  {
                     if(sample->b_liverec_osc_resample)
                     {
                        // constant cycle len
                        if(liverec_copy_loop_offset >= 0)
                        {
                           liveRecResampleToOverride(override_smpdat/*smpDatBase*/, 0u/*waveOff*/);
                        }
                     }
                     else
                     {
                        // adjust cycle len
                        //  (todo) use queued_override in ring mode
                        if( (STSAMPLE_LIVEREC_MODE_CONTINUOUS_RING == sample->liverec_mode) && (STSAMPLE_LIVEREC_OSC_MODE_OFF != sample->liverec_osc_mode) )
                        {
                           liverec_copy_loop_offset = sample->liverec_queued_override_loop_offset;
                           liverec_copy_loop_len    = sample->liverec_queued_override_loop_len;
                        }
                        else
                        {
                           liverec_copy_loop_offset = sample->liverec_override_loop_offset;
                           liverec_copy_loop_len    = sample->liverec_override_loop_len;
                        }

                        if(liverec_copy_loop_offset >= 0)
                        {
                           liveRecCopyToOverride(override_smpdat/*smpDatBase*/, 0u/*waveOff*/);
                        }

                        // compensate pitch when detected loop cycle len differs from ideal cycle len
                        if(sample->liverec_ref_cyclelen > 0.0f)
                        {
                           liverec_osc_pitch_factor = liverec_copy_loop_len / sample->liverec_ref_cyclelen;
                           // Dyac_host_printf("xxx liverec_osc_pitch_factor=%f (ref_cyclelen=%u, copy_looplen=%u)\n", liverec_osc_pitch_factor, sUI(sample->liverec_ref_cyclelen), liverec_copy_loop_len);
                        }
                     }
                  } // if override_smpdat
               } // if liverec_oscmode != OFF
            }
         }

         calcCurrentSampleLen();

         // Dyac_host_printf("xxx restart inf. loop idx=%i off=%u len=%u count=%u liverec_copy_loop_len=%u\n",
         //                  current_loop_idx,
         //                  current_sample_offset,
         //                  current_sample_len,
         //                  current_loop_countdown,
         //                  liverec_copy_loop_len
         //                  );

         if(bCycleLenEOL)
            *cOff = 0.0;
         else
         {
            // keep phase, e.g. for chiptune-like osc waveform sequences
            while( ((sUI)*cOff) >= current_sample_len )
            {
               *cOff -= (sF64)current_sample_len;
            }
            ////Dyac_host_printf("xxx loop last loop cOff=%u\n", (sUI)*cOff);
         }

#if 0
         *cOff = sF64(sUI(*cOff));
#endif

         calcSmpDat(*smpDat, *smpDatLRX, current_sample_offset, current_orig_sample_len, current_sample_len);
         // Dyac_host_printf("xxx loop smpDat=%p off=%d len=%d\n", *smpDat, current_sample_offset, current_sample_len);

         bNextStep = YAC_FALSE;
         b_zero = YAC_FALSE;
      }


   } // else if b_timedloop


   // Dyac_host_printf("xxx 2 handleEndOfLoop: bNextStep=%d cOff=%f current_sample_len=%u\n", bNextStep, *cOff, current_sample_len);


   if(-1 != mod_jumptoloop)
   {
      // Dyac_host_printf("xxx handleEndOfLoop: mod_jumptoloop=%d\n", mod_jumptoloop);

      if( ((sUI)mod_jumptoloop) < (sUI)num_sample_loops )
      {
         current_loop_idx = (sUI) (mod_jumptoloop -1); // will be inc'd in if(bNextStep) block
         bNextStep = 1;
      }
      mod_jumptoloop = -1;
   }

   if(STSAMPLE_PLAY_MODE_FWDNOLOOP == sample->play_mode)
   {
      if(!queued_noteon.b_valid && b_playing && !bNextStep)
      {
         stopVoice();
         return YAC_FALSE;
      }
   }

   // Dyac_host_printf("xxx StSampleVoice::handleEndOfLoop: start next loop current_loop_idx=%d bNextStep=%d\n", current_loop_idx, bNextStep);
   if(bNextStep)
   {
      // Start next loop
      if(num_sample_loops > 0)
      {
         if(-1 == current_loop_idx)
         {
            // First loop
            current_loop_idx = sample->first_loop_index;
         }
         else
         {
            // Next loop
            current_loop_idx++;
         }

         // Dyac_host_printf("xxx handleEndOfLoop b_timedloop=%d loopIdx=%d/%d\n", b_timedloop, current_loop_idx, num_sample_loops);

         if(current_loop_idx >= num_sample_loops)
         {
            // End of sample
            ////printf("xxx stopVoice 5\n");
            stopVoice();
            return 0;
         }
         else
         {
            // Play next loop
            sUI sloff = 3 * current_loop_idx;
            sUI times = *(sUI*)&getCurrentSampleLoops()->elements[sloff + 2];
            if(times > 0u)
            {
               times = sUI(times * mmdst_loop_repeats_scale);
               if(0u == times)
                  times = 1u;
            }
            sUI target;

            ////printf("xxx playnextloop idx=%d\n", current_loop_idx);

            while(times & StSample::LOOP_OP_MASK)
            {
               if(StSample::LOOP_OP_SET == (times & StSample::LOOP_OP_MASK))
               {
                  // Copy immediate value to loop register
                  sUI varIdx = (times & StSample::LOOP_VAR_MASK) >> StSample::LOOP_VAR_SHIFT;
                  loop_vars[varIdx] = times & StSample::LOOP_VAL_MASK;
                  // Continue with next loop/command
                  target = (current_loop_idx + 1); // new jmp target (next instruction)
                  ////Dyac_host_printf("xxx LOOP_OP_SET: %d = #%d\n", varIdx, loop_vars[varIdx]);
               }
               else if(StSample::LOOP_OP_JNZ == (times & StSample::LOOP_OP_MASK))
               {
                  // Decrement and branch if not zero
                  sUI varIdx = (times & StSample::LOOP_VAR_MASK) >> StSample::LOOP_VAR_SHIFT;
                  if(loop_vars[varIdx])
                  {
                     loop_vars[varIdx]--;
                  }
                  // set new jmp target
                  if(loop_vars[varIdx])
                  {
                     target = (times & StSample::LOOP_VAL_MASK);
                  }
                  else
                  {
                     // Continue with next loop/command
                     target = (current_loop_idx + 1);
                  }
                  ////Dyac_host_printf("xxx LOOP_OP_JNZ: varIdx=%d val=%d target=%u\n", varIdx, loop_vars[varIdx], target);
               }
               else if(StSample::LOOP_OP_MOV == (times & StSample::LOOP_OP_MASK))
               {
                  // Move register values
                  sUI varIdxSrc = (times & StSample::LOOP_VAL_MASK) & 15;
                  sUI varIdxDst = (times & StSample::LOOP_VAR_MASK) >> StSample::LOOP_VAR_SHIFT;
                  loop_vars[varIdxDst] = loop_vars[varIdxSrc];
                  // Continue with next loop/command
                  target = (current_loop_idx + 1); // new jmp target (next instruction)
                  ////Dyac_host_printf("xxx LOOP_OP_MOV: %d = %d\n", varIdxDst, varIdxSrc);
               }
               else
               {
                  // Unconditional jump
                  target = (times & StSample::LOOP_VAL_MASK);
                  ////Dyac_host_printf("xxx LOOP_OP_JMP: target=%d\n", target);
               }

               if(StSample::LOOP_OP_JMP == (target & StSample::LOOP_OP_MASK))
               {
                  // avoid infinite loops
                  ////printf("xxx stopVoice 6\n");
                  stopVoice();
                  return YAC_FALSE;
               }
               if( target < ((sUI)num_sample_loops) )
               {
                  current_loop_idx = target;
                  sloff = 3 * current_loop_idx;
                  times = *(sUI*)&getCurrentSampleLoops()->elements[sloff + 2];

                  if(times > 0u)
                  {
                     times = sUI(times * mmdst_loop_repeats_scale);
                     if(0u == times)
                        times = 1u;
                  }
               }
               else
               {
                  // illegal loop index
                  ////printf("xxx stopVoice 7\n");
                  stopVoice();
                  return 0;
               }
            } // while parse loop ops

            // Found a non-op loop entry
            if(sample->ui_sync_first_loop_index)
            {
               times = 0; // forever
            }

            current_loop_countdown  = (sSI) times;
            YAC_IntArray *sampleLoops = getCurrentSampleLoops();
            current_sample_offset   = *(sUI*)&sampleLoops->elements[sloff + 0];
            current_orig_sample_len = *(sUI*)&sampleLoops->elements[sloff + 1];

            if(YAC_TRUE == _bFromStart)
            {
               // First iteration starts at the beginning of the sample (PT mode)
               if(current_sample_offset > 0u)
               {
                  // // current_orig_sample_len += current_sample_offset;
                  current_orig_sample_len = current_sample_offset;  // [24Apr2021] play first iteration until inf loop start (avoid crossfade clicks)
                  current_sample_offset = 0u;
                  b_allow_xfade = false;
               }
            }
            else if(YAC_MAYBE == _bFromStart)
            {
               // First iteration starts at last loop offset (multisamples, samplesynth)
               if(current_loop_idx < (num_sample_loops-1))
               {
                  sUI lastLoopOff = *(sUI*)&sampleLoops->elements[sUI(num_sample_loops-1)*3u + 0];
                  if(lastLoopOff < current_sample_offset)
                  {
                     // // current_orig_sample_len += (current_sample_offset - lastLoopOff);
                     current_orig_sample_len = (current_sample_offset - lastLoopOff);  // [24Apr2021] play first iteration until inf loop start (avoid crossfade clicks)
                     current_sample_offset = lastLoopOff;
                     b_allow_xfade = false;
                  }
               }
            }

            calcCurrentSampleLen();

//                Dyac_host_printf("xxx start loop idx=%d off=%u len=%u count=%u\n",
//                                 current_loop_idx,
//                                 current_sample_offset,
//                                 current_sample_len,
//                                 current_loop_countdown
//                                 );

            // Dyac_host_printf("xxx after: calcCurrentSampleLen: current_sample_len=%u\n", current_sample_len);

            if(0 == current_sample_len)
            {
               // End of sample
               ////printf("xxx stopVoice 8\n");
               stopVoice();
               return YAC_FALSE;
            }
            else
            {
               ////Dyac_host_printf("xxx *cOff=%f current_sample_len=%d\n", *cOff, current_sample_len);


               if(b_timedloop)
               {
                  if(0 == times)
                  {
                     // Disable timedloop if step is looped forever
                     //  (note) not, do not! otherwise the infinitely looped sample part is not correctly faded in, causing clicks.
                     b_timedloop = 0;

                     if(sample->b_timedloop_fade)
                     {
                        fadein_countdown = VOLRAMP_NUMSTEPS;
                     }
                     else
                     {
                        fadein_countdown = -1;
                     }
                     fadeout_countdown = -1;
                     b_fading_out = 0;

                     if(bCycleLenEOL)
                     {
                        *cOff = 0.0;
                     }
                     else
                     {
                        while( ((sUI)*cOff) >= current_sample_len )
                        {
                           *cOff -= (sF64)current_sample_len;
                        }
                     }
                  }
                  else
                  {
                     samples_until_end_of_loopstep = (sSI) (times * mix_rate / timedloop_base);
                     ////printf("xxx handleEndOfLoop: samples_until_end_of_loopstep=%d\n", samples_until_end_of_loopstep);
                     if(sample->b_timedloop_fade)
                     {
                        fadein_countdown = VOLRAMP_NUMSTEPS;
                     }
                     else
                     {
                        fadein_countdown = -1;
                     }
                     fadeout_countdown = -1;
                     b_fading_out = 0;
                     *cOff = 0.0f;
                  }
               }
               else
               {
                  if(bCycleLenEOL)
                  {
                     *cOff = 0.0;
                  }
                  else
                  {
                     // keep phase, e.g. for chiptune-like osc waveform sequences
                     while( ((sUI)*cOff) >= current_sample_len )
                     {
                        *cOff -= (sF64)current_sample_len;
                     }
                  }
               }

               ////Dyac_host_printf("xxx 2 *cOff=%f current_sample_len=%d\n", *cOff, current_sample_len);
               ////Dyac_host_printf("xxx StSampleVoice: start loop off=%d len=%d\n", current_sample_offset, current_sample_len);
               calcSmpDat(*smpDat, *smpDatLRX, current_sample_offset, current_orig_sample_len, current_sample_len);
               b_zero = YAC_FALSE;
            }
         }
      }
   } // if bNextStep

   return YAC_TRUE;
}

void StSampleVoice::readWindowedCycleSample(const sF32 *smpDat,
                                            const sF32 *smpDatLRX,
                                            sUI         _off,
                                            sF32        _tsOff,
                                            sF32       *_l,
                                            sF32       *_r,
                                            const sUI   _curSampleLen
                                            ) {

   sUI tsIntOff = (sUI) _tsOff;
   if(tsIntOff == ( ((sUI)current_cyclelen) - 1))
   {
      sF32 al,ar,bl,br;
      sF32 tsFrac = _tsOff - tsIntOff;

      // printf("xxx readcyclesmp: LAST off=%u tsOff=%f tsFrac=%f\n", _off, _tsOff, tsFrac);

      // read last cycle sample
      readWindowedSample(smpDat, NULL/*smpDatLRX*/, _off + _tsOff, &al, &ar, false/*bAllowInterpol*/, _curSampleLen);

      // lerp to start of cycle
      readWindowedSample(smpDat, NULL/*smpDatLRX*/, _off + tsFrac, &bl, &br, false/*bAllowInterpol*/, _curSampleLen);

      *_l = (1.0f - tsFrac) * al + tsFrac * bl;
      *_r = (1.0f - tsFrac) * ar + tsFrac * br;

   //    // *_l = 0;
   //    // *_r = 0;

   //    // *_l = 1.0f;
   //    // *_r = 1.0f;

   }
   else
   {
      // printf("xxx readcyclesmp: off=%u tsOff=%f\n", _off, _tsOff);
      // if(_tsOff >= current_cyclelen)
      // {
      //    // printf("xxx readcyclesmp: CLIP tsoff=%f cyclelen=%f\n", _tsOff, current_cyclelen);
      //    _tsOff = current_cyclelen - 1;
      // }
      readWindowedSample(smpDat, smpDatLRX, _off + _tsOff, _l, _r, true/*bAllowInterpol*/, _curSampleLen);

      // *_l = 0;
      // *_r = 0;
   }
}

sF32 StSampleVoice::BitReduce(sF32 f, const sF32 _brPreAmp, const sU16 _brMask) {
   // (note) tanh warps the samples, even w/o amplification (e.g. tanh(0.7) = 0.604367777117)
   if(_brPreAmp != 1.0f)
      f = tanhf(f * _brPreAmp);
   // // else
   // // {
   // //    f = f * -sample->br_preamp;
   // //    f = sRANGE(f, -1.0f, 1.0f);
   // // }
   sS16 sam16 = sS16(f * 32767.0f);
   if(sam16 >= 0)
   {
      if(sam16 > 32767)
         sam16 = 32767;
      sam16 &= ~_brMask;
      f = sam16 / 32767.0f;
   }
   else
   {
      sam16 = -sam16;
      if(sam16 > 32767)
         sam16 = 32767;
      sam16 &= ~_brMask;
      f = sam16 / -32767.0f;
   }
   // static int xxx = 0;
   // if(0 == (++xxx & 4095))
   //    Dyac_host_printf("xxx f_8bit=%d\n", sSI(f*127));
   return f;
}

void StSampleVoice::readWindowedSample(const sF32 *smpDat, const sF32 *smpDatLRX, sF32 _off, sF32 *_l, sF32 *_r, sBool _bAllowInterpol, const sUI _curSampleLen) {

   ////printf("readWindowedSample _off=%f\n", _off);

   sUI numCh = sample->waveform->num_channels;

   sUI intOff = (sUI) _off;
   sF32 smpFrac = _off - intOff;
   ////printf("readWIndowedSample smpFrac=%f\n", smpFrac);
   while(intOff < 0)
   {
      intOff += _curSampleLen/*current_sample_len*/;
   }
   if(intOff >= _curSampleLen/*current_sample_len*/)
   {
      intOff -= _curSampleLen/*current_sample_len*/;
   }

   intOff &= smpoff_mask;

   sUI smpOff = intOff * numCh;
   if((STSAMPLE_INTERPOL_LINEAR == sample->interpol_type) && _bAllowInterpol)
   {
      // Linear interpolation
      sUI smpOffR;
      if((intOff + 1) >= _curSampleLen/*current_sample_len*/)
      {
         // interpolate to current loop start (ideally: start of next loop..)
         smpOffR = 0;
      }
      else
      {
         // interpolate to next neighbour sample
         smpOffR = smpOff + numCh;
      }

      ////printf("xxx _off=%f smpOff=%u len=%u\n", _off, smpOff, current_sample_len);

      sF32 smpL;
      sF32 smpLn;
      sF32 smpR;
      sF32 smpRn;

      smpL  = smpDat[smpOff  + 0u];
      smpLn = smpDat[smpOffR + 0u];

      if(NULL != smpDatLRX)
      {
         smpL  = smpL  + (smpDatLRX[smpOff  + 0u] - smpL)  * sample->liverec_frame_offset_xfade_amt;
         smpLn = smpLn + (smpDatLRX[smpOffR + 0u] - smpLn) * sample->liverec_frame_offset_xfade_amt;
      }

      if(numCh > 1)
      {
         smpR  = smpDat[smpOff  + 1u];
         smpRn = smpDat[smpOffR + 1u];

         if(NULL != smpDatLRX)
         {
            smpR  = smpR  + (smpDatLRX[smpOff  + 1u] - smpR)  * sample->liverec_frame_offset_xfade_amt;
            smpRn = smpRn + (smpDatLRX[smpOffR + 1u] - smpRn) * sample->liverec_frame_offset_xfade_amt;
         }

         if(br_mask)
         {
            smpL  = BitReduce(smpL,  sample->br_preamp, br_mask);
            smpLn = BitReduce(smpLn, sample->br_preamp, br_mask);

            smpR  = BitReduce(smpR,  sample->br_preamp, br_mask);
            smpRn = BitReduce(smpRn, sample->br_preamp, br_mask);
         }
      }
      else
      {
         if(br_mask)
         {
            smpL  = BitReduce(smpL,  sample->br_preamp, br_mask);
            smpLn = BitReduce(smpLn, sample->br_preamp, br_mask);
         }
      }

      // // *_l = smpDat[smpOff + 0] * (1-smpFrac) + smpDat[smpOffR + 0] * smpFrac;
      *_l = smpL + (smpLn - smpL) * smpFrac;
      if(numCh > 1)
      {
         *_r = smpR + (smpRn - smpR) * smpFrac;
      }
      else
      {
         *_r = *_l; // mono to stereo
      }
   }
   else
   {
      sF32 smpL;
      sF32 smpR;

      smpL  = smpDat[smpOff  + 0u];

      if(NULL != smpDatLRX)
      {
         smpL = smpL + (smpDatLRX[smpOff + 0u] - smpL)  * sample->liverec_frame_offset_xfade_amt;
      }

      // No interpolation
      if(br_mask)
      {
         smpL = BitReduce(smpL, sample->br_preamp, br_mask);
      }

      *_l = smpL;

      if(numCh > 1)
      {
         smpR = smpDat[smpOff  + 1u];

         if(NULL != smpDatLRX)
         {
            smpR = smpR + (smpDatLRX[smpOff + 1u] - smpR)  * sample->liverec_frame_offset_xfade_amt;
         }

         if(br_mask)
         {
            smpR  = BitReduce(smpR, sample->br_preamp, br_mask);
         }

         *_r = smpR;
      }
      else
      {
         *_r = *_l; // mono to stereo
      }
   }
}

void StSampleVoice::calcCurrentSampleLen(void) {
   // Dyac_host_printf("xxx calcCurrentSampleLen: b_zero=%d current_orig_sample_len=%u\n", b_zero, current_orig_sample_len);
   if(!b_zero)
   {
      if(sample->b_liverec &&
         (STSAMPLE_LIVEREC_OSC_MODE_OFF != sample->liverec_osc_mode) &&  // 30Apr2021 (front_buffer copy)
         (!sample->b_liverec_osc_sustain || (STSAMPLE_LIVEREC_OSC_MODE_OFF == sample->liverec_osc_mode)) &&
         (sample->b_liverec_copy || sample->hasLiveRecAdaptiveLoopSize())
         )
      {
         // yac_host->printf("xxx liverec_copy_loop_len=%d\n", liverec_copy_loop_len);
         if(sample->b_liverec_copy)
         {
            // Read from buffer copy
            //  (note) liverec_copy_loop_len may be 0 (loop detection failed)
            if(STSAMPLE_LIVEREC_OSC_MODE_OFF == sample->liverec_osc_mode)
            {
               // "long-sample" mode (with envelopes etc), keep original loop settings
               current_sample_len = (sUI) (current_orig_sample_len * mod_cyclelen * sample_player->mod_cyclelen * mmdst_cyclelen * cur_cyclelen_eol);

               if(current_sample_len > sUI(liverec_copy_loop_len))
                  current_sample_len = sUI(liverec_copy_loop_len);

               if(current_sample_len < 2u)
                  current_sample_len = 2u;
            }
            else
            {
               // Osc mode (adaptive loop len)
               current_sample_offset = 0u;
               current_sample_len    = sUI(liverec_copy_loop_len);
               // Dyac_host_printf("xxx calcCurrentSampleLen: read from buffer copy override_smpdat=%p len=%u\n", override_smpdat, liverec_copy_loop_len);
               if(current_sample_len < 2u)
                  current_sample_len = 2u;
            }
            current_orig_sample_len = current_sample_len;
         }
         else
         {
            // Read from live buf
            current_sample_offset = sUI(sample->liverec_override_loop_offset);
            current_sample_len    = sUI(sample->liverec_override_loop_len);
            // Dyac_host_printf("xxx liverec read from live buffer: off=%u len=%u\n", current_sample_offset, current_sample_len);
            if(current_sample_len < 2u)
               current_sample_len = 2u;
         }
         if(current_play_offset >= current_sample_len)
            current_play_offset = 0;
      }
      else if(sample->b_timestretch)
      {
         current_sample_len = current_orig_sample_len;
      }
      else
      {
         // Dyac_host_printf("xxx current_orig_sample_len=%u\n", current_orig_sample_len);
         current_sample_len = (sUI) (current_orig_sample_len * mod_cyclelen * sample_player->mod_cyclelen * mmdst_cyclelen * cur_cyclelen_eol);
         // Dyac_host_printf("xxx current_sample_len=%u cur_cyclelen_eol=%f\n", current_sample_len, cur_cyclelen_eol);

         if(current_sample_len < 2)
            current_sample_len = 2;
      }
   }
}

void StSampleVoice::alignSampleLenToCyclelen(sUI &_sampleLen) {
   sUI r = sUI(sUI(_sampleLen / current_cyclelen) * current_cyclelen + 0.5f);
   if(r < _sampleLen)
      _sampleLen = r;
}

void StSampleVoice::calcCurrentOffset(void) {
   // (note) offset calculation is forced in reallyStartVoice() by setting last_relsmpoff to -1
   //
   // (note) current_play_offset is relative to current sample/loop start
   //

   // // sF32 relSmpOff = (sample->sampleoffset_velocity_amount * queued_noteon._vel) + mod_sampleoff + mod_sampleoff2 + sample_player->mod_sampleoff;
   sF32 relSmpOff = mod_sampleoff + mod_sampleoff2 + mod_sampleoff_wt2d + mmdst_sampleoff + sample_player->mod_sampleoff + sample->sampleoffset_rel;

   // Dyac_host_printf("xxx relSmpOff=%f current_play_offset=%f\n", relSmpOff, current_play_offset);

   if(sample->b_timestretch)
   {
      // (todo) queue and switch to new cycle when current one ends (anti-click)
      sF32 absOff = (mmdst_cyclelen_sampleoff * current_cyclelen);
      relSmpOff += absOff / current_sample_len;
   }

   if(queued_noteon._sampleoff_ms > 0.0f)
   {
      // Audio-track like sample start offset (in milliseconds)
      sF32 smpOff = (sample->waveform->sample_rate * sample->sample_rate_ratio) * (queued_noteon._sampleoff_ms / 1000.0f);
      relSmpOff += (smpOff / current_sample_len);
   }

   if(relSmpOff > 0.999f)
   {
      relSmpOff = 0.999f;
   }
   else if(relSmpOff < 0.0f)
   {
      relSmpOff = 0.0f;
   }

   // (note) [04Oct2021] not updating current_play_offset every time causes distortion in wavetable mode when modulating MM_DST_SAMPLE_OFFSET
   //                     (but not when modulating WT_2D_X_REL)
   // // if(sample->b_timestretch || Dfltnotequal(relSmpOff, last_relsmpoff))
   if(b_allow_smpoff/*!b_glide*/ && Dfltnotequal(relSmpOff, last_relsmpoff))
   {
      // Dyac_host_printf("xxx recalc off relSmpOff=%f last_relsmpoff=%f\n", relSmpOff, last_relsmpoff);
      if((last_relsmpoff >= 0) && sample->b_timestretch)
      {
         // voice already started, avoid clicks by interpolating with last played cycle
         anticlick_granular_smpoffinterpol_countdown = ANTICLICK_GRANULAR_SMPOFFINTERPOL_TIME;
         ////printf("xxx current_play_offset=%f cyclen=%f\n", current_play_offset, current_cyclelen);
         anticlick_granular_smpoffinterpol_winoff    = (sUI) ( ((sUI)(current_play_offset / current_cyclelen)) * current_cyclelen );
         if(sample->b_wavepath)
         {
            anticlick_granular_smpoffinterpol_nextwinoff = (sUI)wavepath_next_play_offset;
         }
         else
         {
            anticlick_granular_smpoffinterpol_nextwinoff = (sUI)(anticlick_granular_smpoffinterpol_winoff + current_cyclelen);
         }
         anticlick_granular_smpoffinterpol_tsoff     = current_ts_offset;
         anticlick_granular_smpoffinterpol_fragoff   = sF32((current_play_offset - anticlick_granular_smpoffinterpol_winoff) / current_cyclelen);
      }

      current_play_offset  = (sF64) (relSmpOff * current_sample_len);
      // Dyac_host_printf("xxx mod_sampleoff=%f relSmpOff=%f\n", mod_sampleoff, relSmpOff);
      // Dyac_host_printf("xxx recalc current_play_offset=%f relSmpOff=%f\n", current_play_offset, relSmpOff);

      if(sample->b_wavepath)
      {
         if(NULL != sample->wavepath_table)
         {
            // Calc current cycle offset
            if(sample->wavepath_table->num_elements > 0)
            {
               // Use relSmpOff to modulate wavepath_idx
               if(last_relsmpoff >= 0.0f)
               {
                  wavepath_idx = (sSI) (sample->wavepath_table->num_elements * last_relsmpoff);
                  if(wavepath_idx < 0)
                  {
                     wavepath_idx = 0;
                  }
                  else if(wavepath_idx >= sample->wavepath_table->num_elements)
                  {
                     // should never be reached
                     wavepath_idx = sample->wavepath_table->num_elements - 1;
                  }
               }

               sSI intWinOff = (sSI) sample->wavepath_table->elements[wavepath_idx];
               sUI winOff = (sUI) (intWinOff * current_cyclelen);

               sUI winOffNext;

               if( (wavepath_idx + 1) < sample->wavepath_table->num_elements)
               {
                  winOffNext = (sUI) (((sUI)sample->wavepath_table->elements[wavepath_idx + 1]) * current_cyclelen);
               }
               else
               {
                  winOffNext = (sUI) (((sUI)sample->wavepath_table->elements[0]) * current_cyclelen);
               }

               wavepath_next_play_offset = winOffNext;
               current_play_offset = (sF64) winOff;

               ////printf("xxx winOff=%u winOffNext=%u idx=%u\n", winOff, winOffNext, wavepath_idx);

            }
            else
            {
               wavepath_next_play_offset = (sUI) current_play_offset;
            }
         }
         else
         {
            // Do not have wavepath_table, treat wavepath_idx as absolute cycle index
            ////wavepath_next_play_offset = (sUI) current_play_offset;

            current_play_offset = (sF64) (wavepath_idx * current_cyclelen);
            wavepath_next_play_offset = (sUI) (next_wavepath_idx * current_cyclelen);

            // [14May2019] clip to prevent crash while changing (increasing) cyclelen in UI
            if((current_play_offset >= current_sample_len) || (wavepath_next_play_offset >= current_sample_len))
            {
               current_play_offset = 0;
               wavepath_next_play_offset = 0;
            }
         }
      } // if sample->b_wavepath

      last_relsmpoff = relSmpOff;
   }
   // else: continue playing at current_play_offset
}

void StSampleVoice::calcCurrentCycleLen(sF32 &ts) {

   ts = (sample->timestretch + mod_timestretch + sample_player->mod_timestretch + mmdst_timestretch);

   current_cyclelen  = (sF32) (
      sample->timestretch_granularity * (1 - sample->timestretch_granularity_amount) +
      (sample->timestretch_granularity * sample->timestretch_granularity_amount * (1.0f - fabs(ts)))
                            );

   current_cyclelen *= mod_cyclelen * sample_player->mod_cyclelen * mmdst_cyclelen * cur_cyclelen_eol;

   ////printf("xxx StSampleVoice::calcCurrentCyclelen: len=%f\n", current_cyclelen);
   ////printf("xxx mod_cyclelen=%f\n", mod_cyclelen);

   if(current_cyclelen < 2)
      current_cyclelen = 2;
}

void StSampleVoice::calcFragmentInterpolPhase(sF32 _phase, sF32 _width) {
   if(_width <= 0.0f)
   {
      fragment_interpol = _phase;
   }
   else if(fragment_interpol < _width)
   {
      fragment_interpol = (fragment_interpol / _width) * _phase;
   }
   else if(fragment_interpol > (1.0f - _width))
   {
      sF32 t = ((1.0f - fragment_interpol) / _width);
      fragment_interpol = (1.0f - t) + (t * _phase);
   }
   else
   {
      fragment_interpol = _phase;
   }
}

sF32 StSampleVoice::getEffectiveMod(void) {
   // Dyac_host_printf("xxx getEffectiveMod: voicemod=%f spmod=%f\n", mod_mod, sample_player->mod_mod);
   sF32 m = mod_mod + sample_player->mod_mod;
   if(m < 0.0f) m = 0.0f; else if(m>1.0f) m = 1.0f;
   return m;
}

void StSampleVoice::updatePluginModulationInitial(const sUI _pluginMask) {
   // used for setting initial VM1..8 destination plugin modulation (before first modmatrix is evaluated)
   sUI pluginBit = 1u;
   for(sUI pluginIdx = 0u; pluginIdx < STSAMPLE_NUM_PLUGINS; pluginIdx++)
   {
      if(_pluginMask & pluginBit)
      {
         st_plugin_cache_entry_t *pce = plugin_cache_entries[pluginIdx];
         if(NULL != pce)
         {
            for(sUI modIdx = 0u; modIdx < STSAMPLE_NUM_MODS_PER_PLUGIN; modIdx++)
            {
               if(modIdx < pce->voice->info->num_mods)
               {
                  if(NULL != pce->voice->info->set_mod_value)
                  {
                     // Dyac_host_printf("xxx update modValue[%u][%u] to %f\n", pluginIdx, modIdx, mmdst.plugins[pluginIdx][modIdx]);
                     pce->voice->info->set_mod_value(pce->voice, modIdx, 0.0f, 0u/*frameOffset*/);
                  }
                  // else: plugin does not implement set_mod_value
               }
            }
         }
         // else : no plugin loaded
      }

      // copy levels (applied even when plugin slot is empty)
      plugin_levels[pluginIdx][0] = sample->plugins[pluginIdx].level[0];
      plugin_levels[pluginIdx][1] = sample->plugins[pluginIdx].level[1];

      // Next plugin
      pluginBit = pluginBit << 1;
   }
}

void StSampleVoice::updatePluginModulationFromMM(tksampler_mmdst_t &mmdst, const sUI _pluginMask) {
   sUI pluginBit = 1u;
   sUI pluginModBit = 1u;
   // yac_host->printf("xxx updatePluginModulationFromMM: pluginMask=0x%02x mmdst.plugin_mod_mask=0x%08x\n", _pluginMask, mmdst.plugin_mod_mask);
   for(sUI pluginIdx = 0u; pluginIdx < STSAMPLE_NUM_PLUGINS; pluginIdx++)
   {
      if(_pluginMask & pluginBit)
      {
         st_plugin_cache_entry_t *pce = plugin_cache_entries[pluginIdx];
         if(NULL != pce)
         {
            for(sUI modIdx = 0u; modIdx < STSAMPLE_NUM_MODS_PER_PLUGIN; modIdx++)
            {
               if(mmdst.plugin_mod_mask & pluginModBit)  // 8 bits per plugin (one per mod dest)
               {
                  if(modIdx < pce->voice->info->num_mods)
                  {
                     if(NULL != pce->voice->info->set_mod_value)
                     {
                        // Dyac_host_printf("xxx update modValue[%u][%u] to %f\n", pluginIdx, modIdx, mmdst.plugins[pluginIdx][modIdx]);
                        pce->voice->info->set_mod_value(pce->voice, modIdx, mmdst.plugins[pluginIdx][modIdx], 0u/*frameOffset*/);
                     }
                     // else: plugin does not implement set_mod_value
                  }
               }

               // Next
               pluginModBit = pluginModBit << 1;
            }
         }
         else
         {
            // No plugin loaded => skip
            pluginModBit = pluginModBit << STSAMPLE_NUM_MODS_PER_PLUGIN;
         }
      }
      else
      {
         // Plugin slot empty or disabled
         pluginModBit = pluginModBit << STSAMPLE_NUM_MODS_PER_PLUGIN;
      }

      // calc levels (applied even when plugin slot is empty)
      sF32 lvl = mmdst.plugin_levels[pluginIdx];
      if(lvl < 0.0f)
         lvl = 0.0f;

      plugin_levels[pluginIdx][0] = sample->plugins[pluginIdx].level[0] * lvl;
      plugin_levels[pluginIdx][1] = sample->plugins[pluginIdx].level[1] * lvl;

      // yac_host->printf("xxx updateFromMM: lvl=%f sample->plugin_levels[pluginIdx=%u]=%f pluginMask=x0%08x\n", lvl, pluginIdx, sample->plugins[pluginIdx].level[0], _pluginMask);

      // Next plugin
      pluginBit = pluginBit << 1;
   }
}

void StSampleVoice::processPluginsVM(const sUI _numFrames) {
   // Process voice plugins routed to 'VM1..8' destination
   //  - render just a single frame and clear the remaining bus frames
   //  - intended for low-frequency modulation (read in modmatrix via MM_SRC_VOICE_BUS_*)
   sUI pluginBit = 1u;
   const sUI numSamples = _numFrames << 1;
   for(sUI pluginIdx = 0u; pluginIdx < STSAMPLE_NUM_PLUGINS; pluginIdx++)
   {
      if(sample->plugin_mask_vm & pluginBit)
      {
         if(sample->plugins[pluginIdx].dst >= STSAMPLE_VOICEPLUGIN_DST_VM1 &&
            sample->plugins[pluginIdx].dst <= STSAMPLE_VOICEPLUGIN_DST_VM8
            )
         {
            // Render to selected voice bus
            st_plugin_cache_entry_t *pce = plugin_cache_entries[pluginIdx];
            if(NULL != pce->voice->info->prepare_block)
            {
               pce->voice->info->prepare_block(pce->voice,
                                               _numFrames,
                                               261.63f/*nextFreqHz*/,
                                               60.0f/*next_freq=C-5*/,
                                               1.0f/*next_vol*/,
                                               0.0f/*next_pan*/
                                               );

               if(_numFrames > 0u)
               {
                  const sUI voiceBusIdx = sample->plugins[pluginIdx].dst - STSAMPLE_VOICEPLUGIN_DST_VM1;
                  sF32 *vbBuf = voice_bus_buffers[voiceBusIdx];
                  sF32 *d = vbBuf + 0u;///render_buf_sample_offset;
                  memset((void*)d, 0, sizeof(sF32) * numSamples);
                  d[0] = d[1] = !b_release;  // gate
                  pce->voice->voice_bus_read_offset = 0u;///render_buf_sample_offset;

                  pce->voice->info->process_replace(pce->voice,
                                                    YAC_TRUE/*bMonoIn*/,
                                                    d,
                                                    d,
                                                    _numFrames
                                                    );

                  // Output amplification
                  d[0] = d[0] * plugin_levels[pluginIdx][0];
                  d[1] = d[1] * plugin_levels[pluginIdx][1];
               }
               // else: voice just started, just prepare block / calc initial modulation levels
            }
         }
      }
      pluginBit = pluginBit << 1;
   }
}

void StSampleVoice::processPluginsVB(const sUI _numFrames) {
   // Process voice plugins routed to 'VB1..8' destination
   sUI pluginBit = 1u;
   const sUI numSamples = _numFrames << 1;
   for(sUI pluginIdx = 0u; pluginIdx < STSAMPLE_NUM_PLUGINS; pluginIdx++)
   {
      if(sample->plugin_mask_vb & pluginBit)
      {
         if(sample->plugins[pluginIdx].dst >= STSAMPLE_VOICEPLUGIN_DST_VB1 &&
            sample->plugins[pluginIdx].dst <= STSAMPLE_VOICEPLUGIN_DST_VB8
            )
         {
            // Render to selected voice bus
            const sUI voiceBusIdx = sample->plugins[pluginIdx].dst - STSAMPLE_VOICEPLUGIN_DST_VB1;
            sF32 *vbBuf = voice_bus_buffers[voiceBusIdx];
            sF32 *d = vbBuf + render_buf_sample_offset;
            st_plugin_cache_entry_t *pce = plugin_cache_entries[pluginIdx];
            pce->voice->voice_bus_read_offset = render_buf_sample_offset;
            pce->voice->info->process_replace(pce->voice,
                                              YAC_FALSE/*bMonoIn*/,
                                              d,
                                              d,
                                              _numFrames
                                              );
#if 0
            for(sUI i = 0u, frameIdx = 0u; frameIdx < _numFrames; frameIdx++, i+= 2u)
            {
               Dyac_host_printf("d[%u]={%f;%f} ", i, d[i*2+0], d[i*2+1]);
            }
            Dyac_host_printf("\n");
#endif

            // Output amplification
            const sF32 ampL = plugin_levels[pluginIdx][0];
            const sF32 ampR = plugin_levels[pluginIdx][1];
            if(1.0f != ampL || 1.0f != ampR)
            {
               for(sUI i = 0u; i < numSamples; i+= 2u)
               {
                  d[i + 0u] *= ampL;
                  d[i + 1u] *= ampR;
               }
            }
         }
      }
      pluginBit = pluginBit << 1;
   }
}

sF32 StSampleVoice::processPluginsSR(const sF32 _cRate) {
   // Process voice plugins routed to 'SR' destination
   //  - process each frame separately
   //  - returns new sample rate
   sUI pluginBit = 1u;
   sF32 r = 0.0f;
   for(sUI pluginIdx = 0u; pluginIdx < STSAMPLE_NUM_PLUGINS; pluginIdx++)
   {
      if(sample->plugin_mask_sr & pluginBit)
      {
         if(STSAMPLE_VOICEPLUGIN_DST_SR == sample->plugins[pluginIdx].dst)
         {
            st_plugin_cache_entry_t *pce = plugin_cache_entries[pluginIdx];
            sF32 fGate = b_release ? 0.0f : 1.0f;
            sF32 smpIO[2] = { fGate, fGate };
            pce->voice->voice_bus_read_offset = render_buf_sample_offset;

            pce->voice->info->process_replace(pce->voice,
                                              YAC_TRUE/*bMonoIn*/,
                                              smpIO,
                                              smpIO,
                                              1u/*numFrames*/
                                              );

            // Output amplification
            const sF32 lvl  = plugin_levels[pluginIdx][0];
            r = smpIO[0] * lvl;
            // if(0u == render_buf_sample_offset)
            //    yac_host->printf("xxx SR lvl=%f plugin_levels[pluginIdx=%u][0]=%f\n", lvl, pluginIdx, plugin_levels[pluginIdx][0]);
         }
      }
      pluginBit = pluginBit << 1;
   }
   return _cRate * powf(2.0f, r);
}

void StSampleVoice::processPluginsDef(sBool _bPreFilter,
                                      sBool _bMonoIn
                                      ) {
   // v1: fixed 1*2*3*4 algorithm

   // Process voice plugins
   sUI pluginBit = 1u;
   for(sUI pluginIdx = 0u; pluginIdx < STSAMPLE_NUM_PLUGINS; pluginIdx++)
   {
      if(sample->plugin_mask_def & pluginBit)
      {
         st_plugin_cache_entry_t *pce = plugin_cache_entries[pluginIdx];
         // Dyac_host_printf("xxx processPlugins: pluginIdx=%u pce=%p pce->voice=%p\n", pluginIdx, pce, pce->voice);
         // // if(pce->voice->info->flags & ST_PLUGIN_FLAG_FX)
         {
            if(sample->plugins[pluginIdx].b_pre_filter == _bPreFilter)
            {
               pce->voice->voice_bus_read_offset = render_buf_sample_offset;
#if 0
               // plugin level controls output gain
               pce->voice->info->process_replace(pce->voice,
                                                 _bMonoIn,
                                                 last_sample,
                                                 last_sample,
                                                 1u/*numFrames*/
                                                 );

               last_sample[0u] *= plugin_levels[pluginIdx][0];
               last_sample[1u] *= plugin_levels[pluginIdx][1];
#else
               // plugin level controls input gain
               last_sample[0u] *= plugin_levels[pluginIdx][0];
               last_sample[1u] *= plugin_levels[pluginIdx][1];

               pce->voice->info->process_replace(pce->voice,
                                                 _bMonoIn,
                                                 last_sample,
                                                 last_sample,
                                                 1u/*numFrames*/
                                                 );
#endif

               _bMonoIn = _bMonoIn && (0u == (sample->plugins[pluginIdx].shared->info->info->flags & ST_PLUGIN_FLAG_TRUE_STEREO_OUT));

            }
         }
      }
      pluginBit = pluginBit << 1;
   }
}

void StSampleVoice::processPluginsAM(sF32 *_pluginAmp) {
   // Process voice plugins routed to 'AM'
   sUI pluginBit = 1u;
   for(sUI pluginIdx = 0u; pluginIdx < STSAMPLE_NUM_PLUGINS; pluginIdx++)
   {
      if(sample->plugin_mask_am & pluginBit)
      {
         if(STSAMPLE_VOICEPLUGIN_DST_AM == sample->plugins[pluginIdx].dst)
         {
            st_plugin_cache_entry_t *pce = plugin_cache_entries[pluginIdx];
            pce->voice->voice_bus_read_offset = render_buf_sample_offset;
            sF32 smpIn[2];
            sF32 smpOut[2];
            smpIn[0] = smpIn[1] = b_release ? 0.0f : 1.0f;  // Gate input

            // plugin level controls output intensity / influence on pluginAmp
            pce->voice->info->process_replace(pce->voice,
                                              YAC_TRUE/*bMonoIn*/,
                                              smpIn,
                                              smpOut,
                                              1u/*numFrames*/
                                              );
            const sF32 intensity = plugin_levels[pluginIdx][0];
            const sF32 curAmpL = _pluginAmp[0];
            const sF32 curAmpR = _pluginAmp[1];
            _pluginAmp[0] = curAmpL + ((curAmpL * smpOut[0]) - curAmpL) * intensity;
            _pluginAmp[1] = curAmpR + ((curAmpR * smpOut[1]) - curAmpR) * intensity;
         }
      }
      pluginBit = pluginBit << 1;
   }
}

void StSampleVoice::handleReorderVoicePlugins(const sUI *_ia) {
   st_plugin_cache_entry_t *orig[STSAMPLE_NUM_PLUGINS];
   ::memcpy((void*)orig, (const void*)plugin_cache_entries, sizeof(plugin_cache_entries));
   for(sUI pluginIdx = 0u; pluginIdx < STSAMPLE_NUM_PLUGINS; pluginIdx++)
   {
      plugin_cache_entries[pluginIdx] = orig[_ia[pluginIdx]];
   }
}

sF32 StSampleVoice::sum_partials(const sF32 *_tbl, sU16 *_phases, const sU16 *_speeds, const sF32 *_amps,
                                 const sUI _numPartials, const sF32 _lastPartialAmt
                                 ) {
   sF32 r = 0.0f;
   sUI i = 0u;
   for(; i < _numPartials; i++)
   {
      r += _tbl[_phases[i] >> 4] * _amps[i];
      _phases[i] += _speeds[i];
   }
   if(_lastPartialAmt > 0.0f)
   {
      r += _tbl[_phases[i] >> 4] * _amps[i] * _lastPartialAmt;
      _phases[i] += _speeds[i];
   }
   return r;
}

sF32 StSampleVoice::sum_partials_srr(const sF32 *_tbl, sU16 *_phases, const sU16 *_speeds, const sF32 *_amps,
                                     const sUI _numPartials, const sF32 _lastPartialAmt,
                                     const sUI _srMask
                                     ) {
   sF32 r = 0.0f;
   sUI i = 0u;
   for(; i < _numPartials; i++)
   {
      r += _tbl[(_phases[i] & _srMask) >> 4] * _amps[i];
      _phases[i] += _speeds[i];
   }
   if(_lastPartialAmt > 0.0f)
   {
      r += _tbl[(_phases[i] & _srMask) >> 4] * _amps[i] * _lastPartialAmt;
      _phases[i] += _speeds[i];
   }
   return r;
}

sF32 StSampleVoice::sum_partials_srr_br(const sF32 *_tbl, sU16 *_phases, const sU16 *_speeds, const sF32 *_amps,
                                        const sUI _numPartials, const sF32 _lastPartialAmt,
                                        const sUI _srMask, const sF32 _brPreAmp, const sU16 _brMask
                                        ) {
   sF32 r = 0.0f;
   sUI i = 0u;
   for(; i < _numPartials; i++)
   {
      r += BitReduce(_tbl[(_phases[i] & _srMask) >> 4] * _amps[i], _brPreAmp, _brMask);
      _phases[i] += _speeds[i];
   }
   if(_lastPartialAmt > 0.0f)
   {
      r += BitReduce(_tbl[(_phases[i] & _srMask) >> 4] * _amps[i] * _lastPartialAmt, _brPreAmp, _brMask);
      _phases[i] += _speeds[i];
   }
   return r;
}

void StSampleVoice::sum_partials_stereo(const sF32 *_tbl,
                                        sU16 *_phases, const sU16 *_speeds, const sF32 *_amps,
                                        const sUI _numPartials, const sF32 _lastPartialAmt,
                                        const sF32 *_vcalPan, const sF32 _stereoSpread,
                                        sF32 &retL, sF32 &retR
                                        ) {
   retL = 0.0f;
   retR = 0.0f;
   sUI i = 0u;
   for(; i < _numPartials; i++)
   {
      const float pan    = _vcalPan[i & 7u] * _stereoSpread;
      const float levelL = ( (pan < 0.0f) ? 1.0f : (1.0f - pan) );
      const float levelR = ( (pan > 0.0f) ? 1.0f : (1.0f + pan) );
      sF32 f = _tbl[_phases[i] >> 4] * _amps[i];
      retL += f * levelL;
      retR += f * levelR;
      _phases[i] += _speeds[i];
   }
   if(_lastPartialAmt > 0.0f)
   {
      const float pan    = _vcalPan[i & 7u] * _stereoSpread;
      const float levelL = ( (pan < 0.0f) ? 1.0f : (1.0f - pan) );
      const float levelR = ( (pan > 0.0f) ? 1.0f : (1.0f + pan) );
      sF32 f = _tbl[_phases[i] >> 4] * _amps[i] * _lastPartialAmt;
      retL += f * levelL;
      retR += f * levelR;
      _phases[i] += _speeds[i];
   }
}

void StSampleVoice::sum_partials_stereo_srr_br(const sF32 *_tbl,
                                               sU16 *_phases, const sU16 *_speeds, const sF32 *_amps,
                                               const sUI _numPartials, const sF32 _lastPartialAmt,
                                               const sF32 *_vcalPan, const sF32 _stereoSpread,
                                               const sUI _srMask, const sF32 _brPreAmp, const sU16 _brMask,
                                               sF32 &retL, sF32 &retR
                                               ) {
   retL = 0.0f;
   retR = 0.0f;
   sUI i = 0u;
   for(; i < _numPartials; i++)
   {
      const float pan    = _vcalPan[i & 7u] * _stereoSpread;
      const float levelL = ( (pan < 0.0f) ? 1.0f : (1.0f - pan) );
      const float levelR = ( (pan > 0.0f) ? 1.0f : (1.0f + pan) );
      sF32 f = BitReduce(_tbl[(_phases[i] & _srMask) >> 4] * _amps[i], _brPreAmp, _brMask);
      retL += f * levelL;
      retR += f * levelR;
      _phases[i] += _speeds[i];
   }
   if(_lastPartialAmt > 0.0f)
   {
      const float pan    = _vcalPan[i & 7u] * _stereoSpread;
      const float levelL = ( (pan < 0.0f) ? 1.0f : (1.0f - pan) );
      const float levelR = ( (pan > 0.0f) ? 1.0f : (1.0f + pan) );
      sF32 f = BitReduce(_tbl[(_phases[i] & _srMask) >> 4] * _amps[i] * _lastPartialAmt, _brPreAmp, _brMask);
      retL += f * levelL;
      retR += f * levelR;
      _phases[i] += _speeds[i];
   }
}

void StSampleVoice::renderBlockAdditive(sF32 *    _buf,
                                        sUI       _blkSz,
                                        sF32 *     smpDat,
                                        sF32 *     smpDatLRX,
                                        const sF32 a,
                                        const sF32 b,
                                        sF32       cVolL,
                                        sF32       cVolR,
                                        const sF32 sVolL,
                                        const sF32 sVolR,
                                        const sF32**_inputsOrNull/*sz=STSAMPLE_MAX_INPUTS*/
                                        ) {

   updateWt2dOffset();

   sF32 cNoteHz = noteToFreq(current_freq + (next_freq - current_freq) * a);
   // // sF32 nNoteHz = noteToFreq(current_freq + (next_freq - current_freq) * b);

   if(mmdst.note_2 > 0.0f)
   {
      sF32 hz2 = noteToFreq(mmdst.note_2*127.0f + mmdst.freq_2) - noteToFreq(5.0f*12.0f/*C-5*/);
      // Dyac_host_printf("xxx mmdst.note_2*127.0f=%f mmdst.freq_2=%f hz2=%f\n", mmdst.note_2*127.0f, mmdst.freq_2, hz2);
      cNoteHz += hz2;
   }
   else if(mmdst.freq_2 != 0.0f)
   {
      sF32 hz2 = noteToFreq((5.0f*12.0f)/*C-5*/ + mmdst.freq_2) - noteToFreq(5.0f*12.0f/*C-5*/);
      cNoteHz += hz2;
   }

   // Calc current and next sample rates
   sF32 cRate =  freqToSampleRate(cNoteHz);
   // // sF32 nRate = freqToSampleRate(nNoteHz);

#if 0
   sF32 rateFactor = sample->timestretch_granularity / (sample->waveform->sample_rate / 32.70f/*c-1*/);
   rateFactor *= 65536;
#else
   // (note) 'cRate' includes waveform/output rate ratio
   sF32 rateFactor = 65536.0f * 0.5f;
#endif
   rateFactor *= cRate;

   // // Calc sample rate delta
   // // sF32 sRate = (nRate /* * timeStretch */ - cRate) / _blkSz;

   sUI totalNumWaves = sUI( (sample->waveform->sample_data->num_elements/sample->waveform->num_channels) / sample->timestretch_granularity);
   if(totalNumWaves >= 256u)
      totalNumWaves = 256u;
   else if(totalNumWaves == 0u)
      totalNumWaves = 1u;  // not reachable ??

   // // sF32 tsMaxLen = numLoopWaves * sample->timestretch_granularity/*cycleLen*/;

   sF64 cOff = current_play_offset; // relative to current loop start  (0=&smpDat[0])

   sF32 ts;
   calcCurrentCycleLen(ts);

#if 0
   // (note) 'ts_offset' is the cycle phase in regular wavetable mode
   //         in additive mode, each frequency bin has its own phase
   cOff += current_ts_offset;
   current_ts_offset += 4.0f * ts / (float_block_size);
   if(current_ts_offset >= tsMaxLen)
      current_ts_offset -= tsMaxLen;
   else if(current_ts_offset < 0.0f)
      current_ts_offset += tsMaxLen;
#endif

   // // if(cOff < 0.0f)
   // //    cOff += tsMaxLen;
   // // else if(cOff >= tsMaxLen)
   // //    cOff -= tsMaxLen;

   sF32 *buf = _buf;
   sUI blkSz = _blkSz;

   const sF32 *inputsTmp[STSAMPLE_MAX_INPUTS] = {0};
   const sF32 **inputs;
   if(NULL != _inputsOrNull)
   {
      for(sUI i = 0u; i < STSAMPLE_MAX_INPUTS; i++)
      {
         inputsTmp[i] = _inputsOrNull[i];
      }
      inputs = inputsTmp;
   }
   else
   {
      inputs = NULL;
   }

   sUI loopStartOff     = ((smpDat - sample->waveform->sample_data->elements)/sample->waveform->num_channels)/*==current loop offset + smpshift*/;
   sUI waveIdxLoopStart = (sUI)(  loopStartOff                            / sample->timestretch_granularity );
   sUI waveIdxLoopEnd   = (sUI)( (loopStartOff + current_sample_len - 1u) / sample->timestretch_granularity );
   if(waveIdxLoopStart >= totalNumWaves)
      waveIdxLoopStart = totalNumWaves - 1u;
   if(waveIdxLoopEnd >= totalNumWaves)
      waveIdxLoopEnd = totalNumWaves - 1u;
   const sUI loopNumWaves = 1u + (waveIdxLoopEnd - waveIdxLoopStart);

   const sF64 waveOff = cOff + loopStartOff;
   sUI waveIdx = (sUI)(waveOff / sample->timestretch_granularity);
   if(waveIdx >= totalNumWaves)
      waveIdx = totalNumWaves - 1u;

   const sF32 *partialAmpsA   = sample->partial_magnitudes->elements + (waveIdx * 256u);
   const sF32 *partialSpeedsA = sample->partial_speeds->elements + (waveIdx * 256u);

   sU16 *partialPhases16A;
   sU16 *partialSpeeds16A;

   sF32 numPartialsF = sample->num_partials + mmdst_additive_num_partials;
   numPartialsF = sRANGE(numPartialsF, 1.0f, 256.0f);
   sUI numPartials = sUI(numPartialsF);
   sF32 lastPartialAmt = numPartialsF - numPartials;

   sF32 slewAmt = sample->timestretch_bend + mod_timestretch_bend + sample_player->mod_timestretch_bend + mmdst_timestretch_bend;
   if(slewAmt <= 0.0f)
      slewAmt = 1.0f;
   else
   {
      if(slewAmt > 1.0f)
         slewAmt = 0.0f;
      else
      {
         slewAmt = 1.0f - slewAmt;
         slewAmt = slewAmt * slewAmt;
      }
   }

   if(slewAmt == 1.0f)
   {
      partialPhases16A = partial_phases + (256u * waveIdx);
      partialSpeeds16A = partial_speeds;
   }
   else
   {
      partialPhases16A = partial_phases;
      partialSpeeds16A = partial_speeds;

      if(b_slew_partial_first)
      {
         // voice just started
         b_slew_partial_first = YAC_FALSE;
         slewAmt = 0.99999f;
      }

      if(sample->b_timestretch_xfade)
      {
         for(sUI i = 0u; i < numPartials; i++)
         {
            slew_partial_speeds[i]     = slew_partial_speeds[i]     + (partialSpeedsA[i] - slew_partial_speeds[i])     * slewAmt;
            slew_partial_magnitudes[i] = slew_partial_magnitudes[i] + (partialAmpsA[i]   - slew_partial_magnitudes[i]) * slewAmt;
         }
      }
      else
      {
         for(sUI i = 0u; i < numPartials; i++)
         {
            slew_partial_speeds[i]     = partialSpeedsA[i];
            slew_partial_magnitudes[i] = slew_partial_magnitudes[i] + (partialAmpsA[i]   - slew_partial_magnitudes[i]) * slewAmt;
         }
      }

      partialAmpsA   = slew_partial_magnitudes;
      partialSpeedsA = slew_partial_speeds;
   }

   for(sUI i = 0u; i < numPartials; i++)
   {
      partialSpeeds16A[i] = (sU16) (partialSpeedsA[i] * rateFactor + 0.5f);
   }

   // ////sUI waveIdxB = ((sUI)(cOff / sample->timestretch_granularity)) + 1u;
   sUI waveIdxB = waveIdx + 1u;
   if(waveIdxB >= (waveIdxLoopStart + loopNumWaves))
   {
      // Peek at next sample loop (or queued loop idx)
      const sF32 *smpDatNew;
      sUI frameOffNew;
      const sUI frameOffOver = cOff + sample->timestretch_granularity;
      handleEndOfLoopOver(smpDat/*smpDatCur*/, frameOffOver, &smpDatNew, &frameOffNew);
      waveIdxB = sUI( ((smpDatNew - sample->waveform->sample_data->elements)/sample->waveform->num_channels) / sample->timestretch_granularity);
      if(waveIdxB >= totalNumWaves)
         waveIdxB = totalNumWaves - 1u;  // when next loop start exceeds wavetable size OR handleEndOfLoopOver() returned zero_samples (??)
      // waveIdxB = waveIdxLoopStart;
   }
   // // if(waveIdxB >= totalNumWaves)
   // //    waveIdxB = totalNumWaves - 1u;  // should not be reachable

   // static int xxx = 0;
   // if(0 == (++xxx & 4095))
   // {
      // Dyac_host_printf("xxx additive cRate=%f\n", cRate);
   //    // Dyac_host_printf("xxx renderBlockAdditive\n");
   //    // Dyac_host_printf("xxx partial_phases[0]=0x%04x\n", partial_phases[0]);
   //    // Dyac_host_printf("xxx partial_speeds[0]=0x%04x\n", partial_speeds[0]);
   //    Dyac_host_printf("xxx cOff=%g waveIdx=%u\n", cOff, waveIdx);
    // }

   sBool bLerp = (1.0f == slewAmt) && ((STSAMPLE_TIMESTRETCH_INTERPOL_NONE != sample->timestretch_interpol_type) && (waveIdxB != waveIdx));

   const sF32 *partialAmpsB;
   const sF32 *partialSpeedsB;
   sU16 *partialPhases16B = partial_phases + (256u * waveIdxB);
   sU16 *partialSpeeds16B;
#ifdef YAC_GCC
   partialSpeeds16B = NULL;   // avoid GCC warning (only used when bLerp=true)
#endif // YAC_GCC

   if(bLerp)
   {
      fragment_interpol = (sF32)((waveOff - (waveIdx * sample->timestretch_granularity)) / sample->timestretch_granularity);
      if(fragment_interpol < 0.0f)
         fragment_interpol = 0.0f;
      else if(fragment_interpol > 1.0f)
         fragment_interpol = 0.0f;
      applyFragmentInterpol();

      bLerp = (fragment_interpol > 0.0f);

      if(bLerp)
      {
         partialAmpsB   = sample->partial_magnitudes->elements + (waveIdxB * 256u);
         partialSpeedsB = sample->partial_speeds->elements + (waveIdxB * 256u);

         // // partialPhases16B = partial_phases + (256u * waveIdxB);
         partialSpeeds16B = partial_speeds + 256u;

         for(sUI i = 0u; i < numPartials; i++)
         {
            partialSpeeds16B[i] = (sU16) (partialSpeedsB[i] * rateFactor + 0.5f);
         }
      }
   }

#if 0
   static int xxx = 0;
   if(0 == (++xxx & 255))
   {
      yac_host->printf("xxx wiStart=%u wiNum=%u wi=%u wiB=%u ipol=%f bLerp=%d cOff=%f lpOff=%u lpLen=%u cyLen=%f\n", waveIdxLoopStart, loopNumWaves, waveIdx, waveIdxB, fragment_interpol, bLerp, cOff, current_sample_offset, current_sample_len, sample->timestretch_granularity);
   }
#endif

   const sF32 *atbl;
   if(sample->b_force_additive_edit_cfg)
   {
      atbl = sample->additive_tbl;
   }
   else
   {
      if(last_additive_cfg != mmdst_additive_cfg)
      {
         recalcAdditiveTblMix();   // lazy-allocs / updates 'additive_tbl' array
         last_additive_cfg = mmdst_additive_cfg;
      }
      atbl = additive_tbl;
   }

   sF32 additiveStereoSpread = sample->voice_calibration_enable[1/*pan*/] ? (sample->additive_stereo_spread + mmdst_additive_stereo_spread) : 0.0f;
   additiveStereoSpread = sRANGE(additiveStereoSpread, 0.0f, 1.0f);
   const sBool bAdditiveStereoSpread = (0.0f != additiveStereoSpread);

   const sF32 fBoost = powf(2.0f, sample->timestretch_granularity_amount * 8.0f);

   // {
   //    static int xxx = 0;
   //    if(0 == (xxx & 8191))
   //    {
   //       yac_host->printf("xxx bAdditiveStereoSpread=%d bLerp=%d fBoost=%f br_mask=%u smpoff_mask=%u fade_partial_amt=%f\n",
   //                        bAdditiveStereoSpread, bLerp, fBoost, br_mask, smpoff_mask, fade_partial_amt
   //                        );
   //    }
   // }

   while(blkSz > 0u)
   {
      sF32 newSmpL;
      sF32 newSmpR;

      if(bAdditiveStereoSpread)
      {
         if(br_mask || (~0u != smpoff_mask))
         {
            sum_partials_stereo_srr_br(atbl, partialPhases16A, partialSpeeds16A, partialAmpsA, numPartials, lastPartialAmt,
                                       &sample->voice_calibration[1/*pan*/][0], additiveStereoSpread,
                                       (smpoff_mask<<6), sample->br_preamp, br_mask,
                                       newSmpL, newSmpR
                                       );
            if(fade_partial_amt > 0.0f)
            {
               sF32 newSmpLFade;
               sF32 newSmpRFade;
               sum_partials_stereo_srr_br(atbl, &fade_partial_phases[0], partialSpeeds16A, partialAmpsA, numPartials, lastPartialAmt,
                                          &sample->voice_calibration[1/*pan*/][0], additiveStereoSpread,
                                          (smpoff_mask<<6), sample->br_preamp, br_mask,
                                          newSmpLFade, newSmpRFade
                                          );
               newSmpL = newSmpL + (newSmpLFade - newSmpL) * fade_partial_amt;
               newSmpR = newSmpR + (newSmpRFade - newSmpR) * fade_partial_amt;
            }
         }
         else
         {
            sum_partials_stereo(atbl, partialPhases16A, partialSpeeds16A, partialAmpsA, numPartials, lastPartialAmt,
                                &sample->voice_calibration[1/*pan*/][0], additiveStereoSpread,
                                newSmpL, newSmpR
                                );
            if(fade_partial_amt > 0.0f)
            {
               sF32 newSmpLFade;
               sF32 newSmpRFade;
               sum_partials_stereo(atbl, &fade_partial_phases[0], partialSpeeds16A, partialAmpsA, numPartials, lastPartialAmt,
                                   &sample->voice_calibration[1/*pan*/][0], additiveStereoSpread,
                                   newSmpLFade, newSmpRFade
                                   );
               newSmpL = newSmpL + (newSmpLFade - newSmpL) * fade_partial_amt;
               newSmpR = newSmpR + (newSmpRFade - newSmpR) * fade_partial_amt;
            }
         }
      }
      else if(br_mask)
      {
         newSmpL = sum_partials_srr_br(atbl, partialPhases16A, partialSpeeds16A, partialAmpsA, numPartials, lastPartialAmt,
                                       (smpoff_mask<<6), sample->br_preamp, br_mask
                                       );
         if(fade_partial_amt > 0.0f)
         {
            sF32 newSmpLFade = sum_partials_srr_br(atbl, &fade_partial_phases[0], partialSpeeds16A, partialAmpsA, numPartials, lastPartialAmt,
                                                   (smpoff_mask<<6), sample->br_preamp, br_mask
                                                   );
            newSmpL = newSmpL + (newSmpLFade - newSmpL) * fade_partial_amt;
         }
      }
      else if(~0u != smpoff_mask)
      {
         newSmpL = sum_partials_srr(atbl, partialPhases16A, partialSpeeds16A, partialAmpsA, numPartials, lastPartialAmt,
                                    smpoff_mask<<6
                                    );
         if(fade_partial_amt > 0.0f)
         {
            sF32 newSmpLFade = sum_partials_srr(atbl, &fade_partial_phases[0], partialSpeeds16A, partialAmpsA, numPartials, lastPartialAmt,
                                                smpoff_mask<<6
                                                );
            newSmpL = newSmpL + (newSmpLFade - newSmpL) * fade_partial_amt;
         }
      }
      else
      {
         newSmpL = sum_partials(atbl, partialPhases16A, partialSpeeds16A, partialAmpsA, numPartials, lastPartialAmt);
         if(fade_partial_amt > 0.0f)
         {
            sF32 newSmpLFade = sum_partials(atbl, &fade_partial_phases[0], partialSpeeds16A, partialAmpsA, numPartials, lastPartialAmt);
            newSmpL = newSmpL + (newSmpLFade - newSmpL) * fade_partial_amt;
         }
      }

      if(bLerp)
      {
         sF32 newSmpLB;
         sF32 newSmpRB;
         if(bAdditiveStereoSpread)
         {
            if(br_mask || (~0u != smpoff_mask))
            {
               sum_partials_stereo_srr_br(atbl, partialPhases16B, partialSpeeds16B, partialAmpsB,
                                          numPartials, lastPartialAmt,
                                          &sample->voice_calibration[1/*pan*/][0], additiveStereoSpread,
                                          (smpoff_mask<<6), sample->br_preamp, br_mask,
                                          newSmpLB, newSmpRB
                                          );
               if(fade_partial_amt > 0.0f)
               {
                  sF32 newSmpLFade;
                  sF32 newSmpRFade;
                  sum_partials_stereo_srr_br(atbl, &fade_partial_phases[256], partialSpeeds16B, partialAmpsB,
                                             numPartials, lastPartialAmt,
                                             &sample->voice_calibration[1/*pan*/][0], additiveStereoSpread,
                                             (smpoff_mask<<6), sample->br_preamp, br_mask,
                                             newSmpLFade, newSmpRFade
                                             );
                  newSmpLB = newSmpLB + (newSmpLFade - newSmpLB) * fade_partial_amt;
               }
            }
            else
            {
               sum_partials_stereo(atbl, partialPhases16B, partialSpeeds16B, partialAmpsB,
                                   numPartials, lastPartialAmt,
                                   &sample->voice_calibration[1/*pan*/][0], additiveStereoSpread,
                                   newSmpLB, newSmpRB
                                   );
               if(fade_partial_amt > 0.0f)
               {
                  sF32 newSmpLFade;
                  sF32 newSmpRFade;
                  sum_partials_stereo(atbl, &fade_partial_phases[256], partialSpeeds16B, partialAmpsB,
                                      numPartials, lastPartialAmt,
                                      &sample->voice_calibration[1/*pan*/][0], additiveStereoSpread,
                                      newSmpLFade, newSmpRFade
                                      );
                  newSmpLB = newSmpLB + (newSmpLFade - newSmpLB) * fade_partial_amt;
               }
            }
         }
         else if(br_mask)
         {
            newSmpLB = sum_partials_srr_br(atbl, partialPhases16B, partialSpeeds16B, partialAmpsB, numPartials, lastPartialAmt,
                                           smpoff_mask<<6, sample->br_preamp, br_mask
                                           );
            if(fade_partial_amt > 0.0f)
            {
               sF32 newSmpLFade = sum_partials_srr_br(atbl, &fade_partial_phases[256], partialSpeeds16B, partialAmpsB, numPartials, lastPartialAmt,
                                                      smpoff_mask<<6, sample->br_preamp, br_mask
                                                      );
               newSmpLB = newSmpLB + (newSmpLFade - newSmpLB) * fade_partial_amt;
            }
         }
         else if(~0u != smpoff_mask)
         {
            newSmpLB = sum_partials_srr(atbl, partialPhases16B, partialSpeeds16B, partialAmpsB,
                                        numPartials, lastPartialAmt,
                                        smpoff_mask<<6
                                        );
            if(fade_partial_amt > 0.0f)
            {
               sF32 newSmpLFade = sum_partials_srr(atbl, &fade_partial_phases[256], partialSpeeds16B, partialAmpsB,
                                                   numPartials, lastPartialAmt,
                                                   smpoff_mask<<6
                                                   );
               newSmpLB = newSmpLB + (newSmpLFade - newSmpLB) * fade_partial_amt;
            }
         }
         else
         {
            newSmpLB = sum_partials(atbl, partialPhases16B, partialSpeeds16B, partialAmpsB,
                                    numPartials, lastPartialAmt
                                    );
            if(fade_partial_amt > 0.0f)
            {
               sF32 newSmpLFade = sum_partials(atbl, &fade_partial_phases[256], partialSpeeds16B, partialAmpsB,
                                               numPartials, lastPartialAmt
                                               );
               newSmpLB = newSmpLB + (newSmpLFade - newSmpLB) * fade_partial_amt;
            }
         }

         if(bAdditiveStereoSpread)
         {
            newSmpL += (newSmpLB - newSmpL) * fragment_interpol;
            newSmpR += (newSmpRB - newSmpR) * fragment_interpol;
         }
         else
            newSmpL += (newSmpLB - newSmpL) * fragment_interpol;
      }

      if(!bAdditiveStereoSpread)
         newSmpR = newSmpL;

      newSmpL *= fBoost;
      newSmpR *= fBoost;

      if(fade_partial_amt > 0.0f)
      {
         fade_partial_amt -= (1.0f / 32.0f);
      }

      // Add input
      if( (NULL != inputs) && ((YAC_TRUE == sample->b_sum_input) || ((YAC_MAYBE == sample->b_sum_input) && sample->b_liverec_active)) )
      {
         if((YAC_MAYBE == sample->b_sum_input) && sample->b_liverec_active)
         {
            newSmpL = 0.0f;
            newSmpR = 0.0f;
         }

         const sF32 pan = sample->sum_input_xfade;
         const sF32 volSmp = (pan < 0.0f) ? 1.0f : (1.0f - pan);
         const sF32 volInp = (pan > 0.0f) ? 1.0f : (1.0f + pan);

         newSmpL = (newSmpL*volSmp) + inputs[sample->liverec_input_idx][0] * volInp;

         if(sample->b_liverec_stereo)
         {
            newSmpR = (newSmpR*volSmp) + inputs[sUI(sample->liverec_input_idx + 1) % STSAMPLE_MAX_INPUTS][0] * volInp;
         }
         else
         {
            newSmpR = (newSmpR*volSmp) + inputs[sample->liverec_input_idx][0] * volInp;
         }
      }

      last_sample_l = newSmpL;
      last_sample_r = newSmpR;

      sF32 pluginAmp[2] = { 1.0f/*L*/, 1.0f/*R*/ };

      if(sample->b_enable_fx)
      {
         if(0u != sample->plugin_mask_vb)
            processPluginsVB(1u/*numFrames*/);

         if(0u == sample->plugin_alg)
         {
            processPluginsDef(YAC_TRUE/*bPreFilter*/, YAC_FALSE/*bMonoIn*/);
            filter(last_sample);
            processPluginsDef(YAC_FALSE/*bPreFilter*/, YAC_FALSE/*bMonoIn*/);
         }
#ifndef LIBSYNERGY_BUILD
         else
         {
            sUI algInt = sample->plugin_alg_int + YAC_FALSE/*bMonoIn*/;
            if(algInt < (sizeof(process_algint_tbl)/sizeof(stsamplevoice_process_alg_fxn_t)))
               (*this.*process_algint_tbl[algInt])();
            else
               Dyac_host_printf("[!!!] invalid algInt=%u\n", algInt);
         }
#endif // LIBSYNERGY_BUILD

         if(0u != sample->plugin_mask_am)
            processPluginsAM(pluginAmp);
      }

      last_sample_l *= cVolL * pluginAmp[0];
      last_sample_r *= cVolR * pluginAmp[1];

      buf[0] += last_sample_l;
      buf[1] += last_sample_r;

      // Next frame
      cVolL += sVolL;
      cVolR += sVolR;
      // // cOff  += cRate;
      // // cRate += sRate;

      sF32 fRate;
      if(0u != sample->plugin_mask_sr)
         fRate = processPluginsSR(cRate);
      else
         fRate = cRate;

      if(sample->partial_cyclelen_reset >= 8.0f)
      {
         partial_phase_offset += fRate;
         if(partial_phase_offset >= sample->partial_cyclelen_reset)
         {
#if 1
            partial_phase_offset -= sample->partial_cyclelen_reset;
#else
            partial_phase_offset = 0.0f;
#endif
            // for(sUI i = 0u; i < numPartials; i++)
            //    partialPhases16A[i] = sU16(partialPhases16A[i] - (partial_phase_offset/fRate) * partialSpeeds16A[i]);
            // memset(partialPhases16A, 0, sizeof(sU16)*256u);  // (todo) restore original partial phases ?

            if(sample->b_absolute_timestretch) // 'fdrst' in UI
            {
               fade_partial_amt = 1.0f;
               ::memcpy((void*)&fade_partial_phases[0],   partialPhases16A, sizeof(sU16)*256u);
               ::memcpy((void*)&fade_partial_phases[256], partialPhases16B, sizeof(sU16)*256u);
            }

            sUI j = waveIdx * 256u;
            if(sample->b_partial_phase_0)
            {
               // start all partials at phase 0
               for(sUI i = 0u; i < numPartials; i++)
                  partialPhases16A[i] = sU16( (partial_phase_offset/fRate) * partialSpeeds16A[i] );
               if(bLerp)
               {
                  // memset(partialPhases16B, 0, sizeof(sU16)*256u);  // (todo) restore original partial phases ?
                  // for(sUI i = 0u; i < numPartials; i++)
                  //    partialPhases16B[i] = sU16(partialPhases16B[i] - (partial_phase_offset/fRate) * partialSpeeds16B[i]);
                  j = waveIdxB * 256u;
                  for(sUI i = 0u; i < numPartials; i++)
                     partialPhases16B[i] = sU16( (partial_phase_offset/fRate) * partialSpeeds16B[i] );
               }
            }
            else
            {
               // use start phases from FFT
               for(sUI i = 0u; i < numPartials; i++)
                  partialPhases16A[i] = sU16( (sample->partial_phases->elements[j++]*65536) +  (partial_phase_offset/fRate) * partialSpeeds16A[i]);
               if(bLerp)
               {
                  // memset(partialPhases16B, 0, sizeof(sU16)*256u);  // (todo) restore original partial phases ?
                  // for(sUI i = 0u; i < numPartials; i++)
                  //    partialPhases16B[i] = sU16(partialPhases16B[i] - (partial_phase_offset/fRate) * partialSpeeds16B[i]);
                  j = waveIdxB * 256u;
                  for(sUI i = 0u; i < numPartials; i++)
                     partialPhases16B[i] = sU16( (sample->partial_phases->elements[j++]*65536) + (partial_phase_offset/fRate) * partialSpeeds16B[i]);
               }
            }
            // Dyac_host_printf("xxx RESET partial_phase_offset=%f fRate=%f cycleLen=%f\n", partial_phase_offset, fRate, sample->timestretch_granularity);
         }
      }

      buf += 2;
      render_buf_sample_offset += 2u;
      blkSz--;
   }

   
   // Animate wave position and handle loops
   cOff += 4.0f * ts / (float_block_size);

   if( cOff < 0.0f )
   {
      // reverse play
      // // cOff += (sF64) (current_sample_len - current_cyclelen);
      cOff += (sF64) (current_sample_len);
      if(cOff < 0)
         cOff = 0;
      // TODO: handle negative sample rate + loops (reverse play)
   }
   if( ((sUI)cOff) >= current_sample_len )
   {
      // Dyac_host_printf("xxx TS handleEndOfLoop cOff=%f current_sample_len=%u\n", cOff, current_sample_len);
      if(!handleEndOfLoop(&cOff, &smpDat, &smpDatLRX, YAC_FALSE/*bFromStart*/, YAC_TRUE/*bAllowCOffReset*/))
      {
         // End of sample
         return;
      }
      // // alignSampleLenToCyclelen(current_sample_len);
      // Dyac_host_printf("xxx TS 2 handleEndOfLoop cOff=%f current_sample_len=%u\n", cOff, current_sample_len);
   }

   current_play_offset = cOff;
}

void StSampleVoice::applyFragmentInterpol(void) {
   switch(sample->timestretch_interpol_type)
   {
      default:
      case STSAMPLE_TIMESTRETCH_INTERPOL_NONE:
         fragment_interpol = 0.0f;
         break;

      case STSAMPLE_TIMESTRETCH_INTERPOL_LINEAR:
         break;

      case STSAMPLE_TIMESTRETCH_INTERPOL_SMOOTHSTEP:
         fragment_interpol = (fragment_interpol * fragment_interpol) * (3.0f - 2.0f * fragment_interpol);
         break;

      case STSAMPLE_TIMESTRETCH_INTERPOL_CLIP25:
         if(fragment_interpol < 0.25f)
         {
            fragment_interpol = 0.0f;
         }
         else
         {
            fragment_interpol = (fragment_interpol - 0.25f) / 0.75f;
         }
         break;

      case STSAMPLE_TIMESTRETCH_INTERPOL_CLIP50:
         if(fragment_interpol < 0.5f)
         {
            fragment_interpol = 0.0f;
         }
         else
         {
            fragment_interpol = (fragment_interpol - 0.5f) / 0.5f;
         }
         break;

      case STSAMPLE_TIMESTRETCH_INTERPOL_CLIP75:
         if(fragment_interpol < 0.75f)
         {
            fragment_interpol = 0.0f;
         }
         else
         {
            fragment_interpol = (fragment_interpol - 0.75f) / 0.25f;
         }
         break;

      case STSAMPLE_TIMESTRETCH_INTERPOL_CLIPMOD:
      {
         sF32 m = mod_mod + sample_player->mod_mod;
         if(m < 0.0f) m = 0.0f; else if(m>0.9999f) m = 0.9999f;
         if(fragment_interpol < m)
         {
            fragment_interpol = 0.0f;
         }
         else
         {
            fragment_interpol = (fragment_interpol - m) / (1.0f - m);
         }
      }
      break;

      case STSAMPLE_TIMESTRETCH_INTERPOL_CLIPVEL:
      {
         sF32 m = queued_noteon._vel;
         if(m < 0.0f) m = 0.0f; else if(m>0.9999f) m = 0.9999f;
         if(fragment_interpol < m)
         {
            fragment_interpol = 0.0f;
         }
         else
         {
            fragment_interpol = (fragment_interpol - m) / (1.0f - m);
         }
      }
      break;

      case STSAMPLE_TIMESTRETCH_INTERPOL_OFF25:
         fragment_interpol = 0.25f + fragment_interpol*0.75f;
         break;

      case STSAMPLE_TIMESTRETCH_INTERPOL_OFF50:
         fragment_interpol = 0.5f + fragment_interpol*0.5f;
         break;

      case STSAMPLE_TIMESTRETCH_INTERPOL_OFF75:
         fragment_interpol = 0.75f + fragment_interpol*0.25f;
         break;

      case STSAMPLE_TIMESTRETCH_INTERPOL_OFFMOD:
      {
         sF32 m = getEffectiveMod();
         fragment_interpol = m + fragment_interpol*(1.0f - m);
      }
      break;

      case STSAMPLE_TIMESTRETCH_INTERPOL_OFFVEL:
      {
         sF32 m = queued_noteon._vel;
         fragment_interpol = m + fragment_interpol*(1.0f - m);
      }
      break;

      case STSAMPLE_TIMESTRETCH_INTERPOL_PHASE25W00:
         fragment_interpol =  0.25f;
         break;

      case STSAMPLE_TIMESTRETCH_INTERPOL_PHASE25W25:
         calcFragmentInterpolPhase(0.25f, 0.25f);
         break;

      case STSAMPLE_TIMESTRETCH_INTERPOL_PHASE25W50:
         calcFragmentInterpolPhase(0.25f, 0.50f);
         break;

      case STSAMPLE_TIMESTRETCH_INTERPOL_PHASE25W75:
         calcFragmentInterpolPhase(0.25f, 0.75f);
         break;

      case STSAMPLE_TIMESTRETCH_INTERPOL_PHASE50W00:
         fragment_interpol =  0.50f;
         break;

      case STSAMPLE_TIMESTRETCH_INTERPOL_PHASE50W25:
         calcFragmentInterpolPhase(0.50f, 0.25f);
         break;

      case STSAMPLE_TIMESTRETCH_INTERPOL_PHASE50W50:
         calcFragmentInterpolPhase(0.50f, 0.50f);
         break;

      case STSAMPLE_TIMESTRETCH_INTERPOL_PHASE50W75:
         calcFragmentInterpolPhase(0.50f, 0.75f);
         break;

      case STSAMPLE_TIMESTRETCH_INTERPOL_PHASE75W00:
         fragment_interpol = 0.75f;
         break;

      case STSAMPLE_TIMESTRETCH_INTERPOL_PHASE75W25:
         calcFragmentInterpolPhase(0.75f, 0.25f);
         break;

      case STSAMPLE_TIMESTRETCH_INTERPOL_PHASE75W50:
         calcFragmentInterpolPhase(0.75f, 0.50f);
         break;

      case STSAMPLE_TIMESTRETCH_INTERPOL_PHASE75W75:
         calcFragmentInterpolPhase(0.75f, 0.75f);
         break;

      case STSAMPLE_TIMESTRETCH_INTERPOL_PHASEMODW00:
         fragment_interpol = getEffectiveMod();
         break;

      case STSAMPLE_TIMESTRETCH_INTERPOL_PHASEMODW25:
         calcFragmentInterpolPhase(getEffectiveMod(), 0.25f);
         break;

      case STSAMPLE_TIMESTRETCH_INTERPOL_PHASEMODW50:
         calcFragmentInterpolPhase(getEffectiveMod(), 0.50f);
         break;

      case STSAMPLE_TIMESTRETCH_INTERPOL_PHASEMODW75:
         calcFragmentInterpolPhase(getEffectiveMod(), 0.75f);
         break;

      case STSAMPLE_TIMESTRETCH_INTERPOL_PHASEMODVEL:
         calcFragmentInterpolPhase(getEffectiveMod(), queued_noteon._vel);
         break;

      case STSAMPLE_TIMESTRETCH_INTERPOL_PHASEVELMOD:
         calcFragmentInterpolPhase(queued_noteon._vel, getEffectiveMod());
         break;

      case STSAMPLE_TIMESTRETCH_INTERPOL_ORDER2:
         fragment_interpol = (fragment_interpol*fragment_interpol);
         break;

      case STSAMPLE_TIMESTRETCH_INTERPOL_ORDER3:
         fragment_interpol = (fragment_interpol*fragment_interpol*fragment_interpol);
         break;

      case STSAMPLE_TIMESTRETCH_INTERPOL_ORDER4:
         fragment_interpol = (fragment_interpol*fragment_interpol*fragment_interpol*fragment_interpol);
         break;

      case STSAMPLE_TIMESTRETCH_INTERPOL_ORDER5:
         fragment_interpol = (fragment_interpol*fragment_interpol*fragment_interpol*fragment_interpol*fragment_interpol);
         break;

      case STSAMPLE_TIMESTRETCH_INTERPOL_ORDER6:
         fragment_interpol = (fragment_interpol*fragment_interpol*fragment_interpol*fragment_interpol*fragment_interpol*fragment_interpol);
         break;

      case STSAMPLE_TIMESTRETCH_INTERPOL_ORDER7:
         fragment_interpol = (fragment_interpol*fragment_interpol*fragment_interpol*fragment_interpol*fragment_interpol*fragment_interpol*fragment_interpol);
         break;
   }
}

void StSampleVoice::updateWt2dOffset(void) {
   if(sample->timestretch_2d_w > 1u)
   {
      if(b_queued_wt2d)
      {
         b_queued_wt2d = YAC_FALSE;
         sF32 absOff;
         if(sample->timestretch_2d_h > 1u)
            absOff = ((wt2d_x * (sample->timestretch_2d_w-1u)) + (sUI(wt2d_y * (sample->timestretch_2d_h-1u)+0.5f) * sample->timestretch_2d_w)) * current_cyclelen;
         else
            absOff = (wt2d_x * (sample->timestretch_2d_w-1u)) * current_cyclelen;
         mod_sampleoff_wt2d = absOff / current_sample_len;
         last_relsmpoff = -1;
         calcCurrentOffset();
         // Dyac_host_printf("xxx new wt2d mod_sampleoff=%f (abs=%f)\n", mod_sampleoff, absOff);
      }
   }
}

void StSampleVoice::renderBlockTimestretch(sF32 *    _buf,
                                           sUI       _blkSz,
                                           sF32 *     smpDat,
                                           sF32 *     smpDatLRX,
                                           const sF32 a,
                                           const sF32 b,
                                           sF32       cVolL,
                                           sF32       cVolR,
                                           const sF32 sVolL,
                                           const sF32 sVolR,
                                           const sF32**_inputsOrNull/*sz=STSAMPLE_MAX_INPUTS*/
                                           )
{
   // printf("xxx renderBlockTimestretch cVolL=%f cVolR=%f\n", cVolL, cVolR);

   // Calculate timestretch and cyclelen
   sF32 ts;
   calcCurrentCycleLen(ts);

   // [15May2019] align to cyclelen
   alignSampleLenToCyclelen(current_sample_len);

   // Dyac_host_printf("xxx current_cyclelen=%f current_sample_len=%u\n", current_cyclelen, current_sample_len);


   // Dyac_host_printf("xxx renderBlockTimestretch: smplen=%u cyclelen=%u\n", current_sample_len, current_cyclelen);
   if(current_sample_len < current_cyclelen)
   {
      return;
   }

   // [14May2019] clip (prevent crash when modulating wavetable sample offset with LFO)
   // (note) this check breaks loops (don't re-enable !!)
   // // if(current_play_offset < 0)
   // //    return;
   // // if((current_play_offset + current_cyclelen) > current_sample_len)
   // //    return;


   //////sF32 cycleLen  = (sF32) (mix_rate * sample->timestretch_granularity);

   ////Dyac_host_printf("this=%p ts=%f cycleLen=%f\n", this, ts, cycleLen);

   // mix rate to play sample at base frequency
   sF32 cRate;
   // mix rate to play cycle at desired frequency
   // //sF32 tsRate = (hz / cycleFreq) * (sample->waveform->sample_rate / mix_rate);
   sF32 tsRate;
   sF32 sRate;

   sF32 cNoteHz;
   sF32 nNoteHz;
   // // sF32 sNoteHz = (nNoteHz - cNoteHz) / _blkSz;

   cNoteHz = noteToFreq(current_freq + (next_freq - current_freq) * a) * liverec_osc_pitch_factor;
   nNoteHz = noteToFreq(current_freq + (next_freq - current_freq) * b) * liverec_osc_pitch_factor;

   if(mmdst.note_2 > 0.0f)
   {
      sF32 hz2 = noteToFreq(mmdst.note_2*127.0f + mmdst.freq_2) - noteToFreq(5.0f*12.0f/*C-5*/);
      // Dyac_host_printf("xxx mmdst.note_2*127.0f=%f mmdst.freq_2=%f hz2=%f\n", mmdst.note_2*127.0f, mmdst.freq_2, hz2);
      cNoteHz += hz2;
      nNoteHz += hz2;
   }
   else if(mmdst.freq_2 != 0.0f)
   {
      sF32 hz2 = noteToFreq((5.0f*12.0f)/*C-5*/ + mmdst.freq_2) - noteToFreq(5.0f*12.0f/*C-5*/);
      cNoteHz += hz2;
      nNoteHz += hz2;
   }

   sF32 tsBend = sample->timestretch_bend + mod_timestretch_bend + sample_player->mod_timestretch_bend + mmdst_timestretch_bend;
   if(tsBend > 1.0f)
      tsBend = 1.0f;
   else if(tsBend < -1.0f)
      tsBend = -1.0f;

   sBool bBend = (0.0f != tsBend);

   if(tsBend < 0.0f)
   {
      tsBend = (1.0f / 16.0f) + ((1.0f - (1.0f/16.0f))) * (1.0f + tsBend);
   }
   else
   {
      tsBend = 1.0f + (16.0f - 1.0f) * tsBend;
   }

   if(sample->b_static_cyclelen)
   {
      // Calc current sample rate
      cRate =  freqToSampleRate(cNoteHz);

      // Calc sample rate delta
      sRate = (freqToSampleRate(nNoteHz) /* * timeStretch */ - cRate) / _blkSz;

      // Cycle replay speed equals replay speed, for wavetable-style chip samples
      tsRate = cRate;

      // modulate move-through-sample speed by timestretch (0= play same cycle over and over again)
      if(sample->b_absolute_timestretch)
      {
         // pitch independent cyclewalking (interpreted ts as 1.0=1 second to reach next cycle)
         ///cRate = (ts * current_cyclelen) / (float_block_size * STSAMPLEVOICE_TICKSPERSEC);
         cRate = ts / float_block_size;
      }
      else
      {
         cRate *= ts;
      }
   }
   else
   {
      // Cycle replay speed depends on cyclelen (for timestretching non-chip samples)
      cRate = 1.0f * (sample->waveform->sample_rate / mix_rate);

      // modulate move-through-sample speed by timestretch (0= play same cycle over and over again)
      cRate *= ts;

      sF64 cycleFreq = (mix_rate / current_cyclelen); // div2 for niquist ??

      tsRate = (sF32) (cNoteHz / cycleFreq);// * (hz / sample->waveform->base_frequency);
      sRate = 0.0f; // (todo) support pitch block interpolation
   }



   sF32 tsOff = current_ts_offset;

   // sF32 cyclePlayLen = current_cyclelen / cRate;

   if(tsOff >= current_cyclelen)
   {
      updateWt2dOffset();
      // // if(sample->timestretch_2d_w > 1u)
      // // {
      // //    if(b_queued_wt2d)
      // //    {
      // //       b_queued_wt2d = YAC_FALSE;
      // //       sF32 absOff;
      // //       if(sample->timestretch_2d_h > 1u)
      // //          absOff = ((wt2d_x * (sample->timestretch_2d_w-1u)) + (sUI(wt2d_y * (sample->timestretch_2d_h-1u)+0.5f) * sample->timestretch_2d_w)) * current_cyclelen;
      // //       else
      // //          absOff = (wt2d_x * (sample->timestretch_2d_w-1u)) * current_cyclelen;
      // //       mod_sampleoff_wt2d = absOff / current_sample_len;
      // //       last_relsmpoff = -1;
      // //       calcCurrentOffset();
      // //       // Dyac_host_printf("xxx new mod_sampleoff=%f (abs=%f)\n", mod_sampleoff, absOff);
      // //    }
      // // }

      while(tsOff >= current_cyclelen)
      {
         tsOff -= current_cyclelen;
      }
   }

   sF64 cOff = current_play_offset;

   ////Dyac_host_printf("xxx this=%p hz=%f current_cyclelen=%f cRate=%f tsRate=%f sRate=%f tsOff=%f cyclePlayLen=%f\n", this, hz, current_cyclelen, cRate, tsRate, sRate, tsOff, cyclePlayLen);

   // Dyac_host_printf("xxx cOff=%f cRate=%f tsRate=%f tsOff=%f \n", cOff, cRate, tsRate, tsOff);

   ////Dyac_host_printf("xxx cVolL=%f cVolR=%f\n", cVolL, cVolR);


   //
   // Timestretch mixloop
   //
   // TODO: make sure that wave is not smaller than fragment size
   //
   sF32 *buf = _buf;
   sUI blkSz = _blkSz;

   const sF32 *inputsTmp[STSAMPLE_MAX_INPUTS] = {0};
   const sF32 **inputs;
   if(NULL != _inputsOrNull)
   {
      for(sUI i = 0u; i < STSAMPLE_MAX_INPUTS; i++)
      {
         inputsTmp[i] = _inputsOrNull[i];
      }
      inputs = inputsTmp;
   }
   else
   {
      inputs = NULL;
   }

   while(blkSz > 0u)
   {
      sF32 smpLA, smpRA, smpLB, smpRB;
      sF32 *smpDatNext = smpDat;
      sF32 *smpDatNextLRX = smpDatLRX;
      sUI nextSampleLen = current_sample_len;

      sSI intWinOff = ((sSI)(cOff / current_cyclelen));
      sUI winOff = (sUI) (intWinOff * current_cyclelen);
      fragment_interpol = sF32((cOff - winOff) / current_cyclelen);

      sUI winOffNext;

      // Calc cycle end offset
      winOffNext = (sUI) ((intWinOff+1) * current_cyclelen);

      // [15May2019] Wrap-around
      if((winOffNext + current_cyclelen) > current_sample_len)
      {
         winOffNext = 0u;
         if((sUI(current_loop_idx) + 1u) < sUI(num_sample_loops))
         {
            if(1u == current_loop_countdown)
            {
               // Lerp to next loop
               YAC_IntArray *sampleLoops = getCurrentSampleLoops();
               sUI sloff = (current_loop_idx + 1u) * 3u;
               sUI nextSampleOffset = *(sUI*)&sampleLoops->elements[sloff + 0];
               nextSampleLen        = *(sUI*)&sampleLoops->elements[sloff + 1];
               alignSampleLenToCyclelen(nextSampleLen);
               calcSmpDat(smpDatNext, smpDatNextLRX, nextSampleOffset, nextSampleLen, nextSampleLen);
               // if(! (((sUI)cOff)&15 ) )
               //    Dyac_host_printf("xxx lerpToNextLoop cOff=%f fragIpol=%f winOff=%u curOff=%u curLen=%u nextOff=%u nextLen=%u tsOff=%f\n", cOff, fragment_interpol, winOff, current_sample_offset, current_sample_len, nextSampleOffset, nextSampleLen, tsOff);
            }
         }
      }

      sF32 fRate;
      if(0u != sample->plugin_mask_sr)
         fRate = processPluginsSR(tsRate);
      else
         fRate = tsRate;

      if(sample->b_wavepath)
      {
         ////printf("xxx check end of cycle: cOff=%f cRate=%f winOffNext=%u\n", cOff, cRate, winOffNext);
         ////if( (cOff + cRate) >= winOffNext )
         if(intWinOff != sSI(wavepath_idx)) // (todo) in case of wavepath_table this should be wavepath_table[wavepath_idx]
         {
            //printf("xxx end of cycle\n");
            if(NULL != sample->wavepath_table)
            {
               // End of cycle
               if(sample->wavepath_table->num_elements > 0)
               {
                  // Select next cycle
                  wavepath_idx++;
                  if(wavepath_idx >= sample->wavepath_table->num_elements)
                  {
                     wavepath_idx = 0;
                  }

                  // Re-calculate current/next play offsets
                  last_relsmpoff = -1;
                  calcCurrentOffset();

                  winOff = (sUI) current_play_offset;
                  cOff = current_play_offset;
                  winOffNext = wavepath_next_play_offset;

                  fragment_interpol = 0.0f;
               }
               else
               {
                  winOffNext = wavepath_next_play_offset;
               }
            }
            else
            {
               // Manual wavepath (use wavepath_idx as absolute cycle index)
               // Dyac_host_printf("xxx endofcycle: cidx=%d nidx=%d coff=%f nextoff=%u ts=%f\n",
               //                  wavepath_idx, next_wavepath_idx,
               //                  current_play_offset, wavepath_next_play_offset,
               //                  ts
               //                  );
               wavepath_idx = next_wavepath_idx;
               next_wavepath_idx = queued_wavepath_idx;

               // Re-calculate current/next play offsets
               last_relsmpoff = -1;
               calcCurrentOffset();

               winOff = (sUI) current_play_offset;
               cOff = current_play_offset;
               winOffNext = wavepath_next_play_offset;

               fragment_interpol = 0.0f;

               // Dyac_host_printf("xxx endofcycle 2: cidx=%d nidx=%d coff=%f nextoff=%u\n",
               //                  wavepath_idx, next_wavepath_idx,
               //                  current_play_offset, wavepath_next_play_offset
               //                  );
            }
         }
         else
         {
            winOffNext = wavepath_next_play_offset;
         }
      }
      else
      {
         // Use neighbour cycle
      }

      // Apply fragment interpolation function
      applyFragmentInterpol();

      // if(! (((sUI)cOff)&15 ) )
      //    Dyac_host_printf("xxx interpol=%f intWinOff=%d tsOff=%f winOff=%u winOffNext=%u cOff=%f current_cyclelen=%f current_sample_len=%u dt=%f\n",
      //                     fragment_interpol, intWinOff, tsOff, winOff, winOffNext, cOff, current_cyclelen, current_sample_len, (cOff-winOff));

      sF32 tsReadOff;
      sF32 tsReadOffXFade;

      if(bBend)
      {
         tsReadOff = tsOff / current_cyclelen;
         tsReadOff = powf(tsReadOff, tsBend);  // (todo) optimize / approximate
         tsReadOff *= current_cyclelen;
      }
      else
      {
         tsReadOff = tsOff;
      }

      // // // tsReadOffXFade = current_cyclelen - 1.0f - tsReadOff;
      // // // if(tsReadOffXFade < 0.0f)
      // // //    tsReadOffXFade += current_cyclelen;
      tsReadOffXFade = current_cyclelen - tsReadOff;
      if(tsReadOffXFade >= current_cyclelen)
         tsReadOffXFade -= current_cyclelen;

      // Read current fragment (linear sample interpolation)
      {
         readWindowedCycleSample(smpDat,
                                 smpDatLRX,
                                 winOff,
                                 tsReadOff,
                                 &smpLA,
                                 &smpRA,
                                 current_sample_len
                                 );

         if(sample->b_timestretch_xfade)
         {
            sF32 l2, r2;
            readWindowedCycleSample(smpDat,
                                    smpDatLRX,
                                    winOff,
                                    tsReadOffXFade,
                                    &l2,
                                    &r2,
                                    current_sample_len
                                    );
            smpLA = (smpLA + l2) * 0.5f;
            smpRA = (smpRA + r2) * 0.5f;
         }

         // // smpLA *= cVolL;
         // // smpRA *= cVolR;
      }

      // Read next fragment (linear sample interpolation)
      {
         readWindowedCycleSample(smpDatNext,
                                 smpDatLRX,
                                 winOffNext,
                                 tsReadOff,
                                 &smpLB,
                                 &smpRB,
                                 nextSampleLen
                                 );

         if(sample->b_timestretch_xfade)
         {
            sF32 l2, r2;
            // readWindowedCycleSample(smpDat, winOffNext, ((sUI)current_cyclelen-1)-tsOff, &l2, &r2);
            readWindowedCycleSample(smpDatNext,
                                    smpDatNextLRX,
                                    winOffNext,
                                    tsReadOffXFade,
                                    &l2,
                                    &r2,
                                    nextSampleLen
                                    );
            smpLB = (smpLB + l2) * 0.5f;
            smpRB = (smpRB + r2) * 0.5f;
         }

         // // smpLB *= cVolL;
         // // smpRB *= cVolR;
      }

      // Calculate interpolated sample
      // //fragment_interpol = (fragment_interpol*fragment_interpol) * (3.0f - 2.0f * fragment_interpol); // smoothstep
      // // // sF32 newSmpL = smpLA * (1.0f - fragment_interpol) + smpLB * fragment_interpol;
      // // // sF32 newSmpR = smpRA * (1.0f - fragment_interpol) + smpRB * fragment_interpol;
      // // if(fragment_interpol > 0.0f)
      // //    Dyac_host_printf("xxx fragment_interpol=%f\n", fragment_interpol);
      sF32 newSmpL = smpLA + (smpLB - smpLA) * fragment_interpol;
      sF32 newSmpR = smpRA + (smpRB - smpRA) * fragment_interpol;

      float grainVol;
      switch(sample->timestretch_grain_window_type)
      {
         default:
         case STSAMPLE_TIMESTRETCH_GRAIN_WINDOW_NONE:
            break;

         case STSAMPLE_TIMESTRETCH_GRAIN_WINDOW_SINE:
            grainVol = sinf( (fragment_interpol-0.25f) * float(2.0f*sM_PI) ) * 0.5f + 0.5f;
            newSmpL *= grainVol;
            newSmpR *= grainVol;
            break;

         case STSAMPLE_TIMESTRETCH_GRAIN_WINDOW_TRI:
            grainVol = (fragment_interpol < 0.5f) ? (fragment_interpol*2.0f) : (1.0f - (fragment_interpol - 0.5f)*2.0f);
            newSmpL *= grainVol;
            newSmpR *= grainVol;
            break;
      }


      // newSmpL = smpLA; // xxx
      // newSmpR = smpRA; // xxx

      // Interpolate samples before and after modulation to avoid clicks when sample offset is being modulated
#if 1  /// xxxxxxxx [15May2019] removed during testing
      if(anticlick_granular_smpoffinterpol_countdown > 0)
      {
         anticlick_granular_smpoffinterpol_countdown--;
         sF32 lcSmpLA, lcSmpRA;
         sF32 lcSmpLB, lcSmpRB;
         // Read current fragment before modulation (linear sample interpolation)
         {
//             printf("xxx anticlick_granular_smpoffinterpol_winoff=%u anticlick_granular_smpoffinterpol_tsoff=%f\n",
//                    anticlick_granular_smpoffinterpol_winoff,
//                    anticlick_granular_smpoffinterpol_tsoff
//                    );
            // readWindowedSample(smpDat,
            //                    anticlick_granular_smpoffinterpol_winoff + anticlick_granular_smpoffinterpol_tsoff,
            //                    &lcSmpLA, &lcSmpRA
            //                    );
            readWindowedCycleSample(smpDat,
                                    smpDatLRX,
                                    anticlick_granular_smpoffinterpol_winoff,
                                    anticlick_granular_smpoffinterpol_tsoff,
                                    &lcSmpLA, &lcSmpRA,
                                    current_sample_len
                                    );

            if(sample->b_timestretch_xfade)
            {
               sF32 l2, r2;
               readWindowedCycleSample(smpDat,
                                       smpDatLRX,
                                       anticlick_granular_smpoffinterpol_winoff,
                                       ((sUI)current_cyclelen-1)-anticlick_granular_smpoffinterpol_tsoff,
                                       &l2, &r2,
                                       current_sample_len
                                       );
               lcSmpLA = (lcSmpLA + l2) * 0.5f;
               lcSmpRA = (lcSmpRA + r2) * 0.5f;
            }

            lcSmpLA *= cVolL;
            lcSmpRA *= cVolR;
         }

         // Read next fragment before modulation (linear sample interpolation)
         {
            // readWindowedSample(smpDat,
            //                    anticlick_granular_smpoffinterpol_nextwinoff + anticlick_granular_smpoffinterpol_tsoff,
            //                    &lcSmpLB, &lcSmpRB);
            readWindowedCycleSample(smpDat,
                                    smpDatLRX,
                                    anticlick_granular_smpoffinterpol_nextwinoff,
                                    anticlick_granular_smpoffinterpol_tsoff,
                                    &lcSmpLB, &lcSmpRB,
                                    current_sample_len
                                    );

            if(sample->b_timestretch_xfade)
            {
               sF32 l2, r2;
               readWindowedCycleSample(smpDat,
                                       smpDatLRX,
                                       anticlick_granular_smpoffinterpol_nextwinoff,
                                       ((sUI)current_cyclelen-1)-anticlick_granular_smpoffinterpol_tsoff,
                                       &l2, &r2,
                                       current_sample_len
                                       );
               lcSmpLB = (lcSmpLB + l2) * 0.5f;
               lcSmpRB = (lcSmpRB + r2) * 0.5f;
            }

            lcSmpLB *= cVolL;
            lcSmpRB *= cVolR;
         }

         // Calculate interpolated sample
         sF32 lcSmpL = lcSmpLA * (1.0f - anticlick_granular_smpoffinterpol_fragoff) + lcSmpLB * anticlick_granular_smpoffinterpol_fragoff;
         sF32 lcSmpR = lcSmpRA * (1.0f - anticlick_granular_smpoffinterpol_fragoff) + lcSmpRB * anticlick_granular_smpoffinterpol_fragoff;

         // Repeat sample calculation for sample offset before modulation
         sF32 t = (((sF32)anticlick_granular_smpoffinterpol_countdown) / ANTICLICK_GRANULAR_SMPOFFINTERPOL_TIME);
         ////printf("cd=%u t=%f\n", anticlick_granular_smpoffinterpol_countdown, t);
         // // newSmpL = (lcSmpL * t) + (newSmpL * (1.0f - t));
         // // newSmpR = (lcSmpR * t) + (newSmpR * (1.0f - t));
         newSmpL = newSmpL + (lcSmpL - newSmpL) * t;
         newSmpR = newSmpR + (lcSmpR - newSmpR) * t;

         // Increase and wrap-around before-modulation cycle offset
         anticlick_granular_smpoffinterpol_tsoff += fRate;
         while(anticlick_granular_smpoffinterpol_tsoff >= current_cyclelen)
         {
            anticlick_granular_smpoffinterpol_tsoff -= current_cyclelen;
         }
      }
      else
      {
         ////printf("nointerpol: cd=%u\n", anticlick_granular_smpoffinterpol_countdown);
      }
#endif // 0

      // printf("xxx w=%u ts=%f n=(%f; %f) v=(%f; %f)\n", winOff, tsOff, newSmpL, newSmpR, cVolL, cVolR);

      // Add input
      if( (NULL != _inputsOrNull) && ((YAC_TRUE == sample->b_sum_input) || ((YAC_MAYBE == sample->b_sum_input) && sample->b_liverec_active)) )
      {
         if((YAC_MAYBE == sample->b_sum_input) && sample->b_liverec_active)
         {
            newSmpL = 0.0f;
            newSmpR = 0.0f;
         }

         const sF32 pan = sample->sum_input_xfade;
         const sF32 volSmp = (pan < 0.0f) ? 1.0f : (1.0f - pan);
         const sF32 volInp = (pan > 0.0f) ? 1.0f : (1.0f + pan);

         newSmpL = (newSmpL*volSmp) + _inputsOrNull[sample->liverec_input_idx][0] * volInp;

         if(sample->b_liverec_stereo)
         {
            newSmpR = (newSmpR*volSmp) + _inputsOrNull[sUI(sample->liverec_input_idx + 1) % STSAMPLE_MAX_INPUTS][0] * volInp;
         }
         else
         {
            newSmpR = (newSmpR*volSmp) + _inputsOrNull[sample->liverec_input_idx][0] * volInp;
         }
      }

      last_sample_l = newSmpL;
      last_sample_r = newSmpR;

      sF32 pluginAmp[2] = { 1.0f/*L*/, 1.0f/*R*/ };

      if(sample->b_enable_fx)
      {
         if(0u != sample->plugin_mask_vb)
            processPluginsVB(1u/*numFrames*/);

         if(0u == sample->plugin_alg)
         {
            processPluginsDef(YAC_TRUE/*bPreFilter*/, YAC_FALSE/*bMonoIn*/);
            filter(last_sample);
            processPluginsDef(YAC_FALSE/*bPreFilter*/, YAC_FALSE/*bMonoIn*/);
         }
#ifndef LIBSYNERGY_BUILD
         else
         {
            sUI algInt = sample->plugin_alg_int + YAC_FALSE/*bMonoIn*/;
            if(algInt < (sizeof(process_algint_tbl)/sizeof(stsamplevoice_process_alg_fxn_t)))
               (*this.*process_algint_tbl[algInt])();
            else
               Dyac_host_printf("[!!!] invalid algInt=%u\n", algInt);
         }
#endif // LIBSYNERGY_BUILD

         if(0u != sample->plugin_mask_am)
            processPluginsAM(pluginAmp);
      }

      last_sample_l *= cVolL * pluginAmp[0];
      last_sample_r *= cVolR * pluginAmp[1];

      buf[0] += last_sample_l;
      buf[1] += last_sample_r;

      samples_since_cycle++; // for debugging purposes

#ifdef USE_SAMPLESHIFT_EOL_XFADE_BUF
      // Store for declick xfade (sampleshift_eol)
      if(sampleshift_eol_xfade_loopstart_frameidx < VOLRAMP_NUMSTEPS)
      {
         sampleshift_eol_xfade_buf_l[sampleshift_eol_xfade_loopstart_frameidx] = last_sample_l;
         sampleshift_eol_xfade_buf_r[sampleshift_eol_xfade_loopstart_frameidx] = last_sample_r;
         sampleshift_eol_xfade_loopstart_frameidx++;
      }
#endif // USE_SAMPLESHIFT_EOL_XFADE_BUF

      cVolL += sVolL;
      cVolR += sVolR;

      tsOff  += fRate;
      tsRate += sRate;

      // // cNoteHz += sNoteHz;

      ////Dyac_host_printf("xxx 1 tsOff=%f tsRate=%f\n", tsOff, tsRate);
      // // while(tsOff >= current_cyclelen)
      // // {
      // //    tsOff -= current_cyclelen;
      // // }
      if(tsOff >= current_cyclelen)
      {
         if(sample->timestretch_2d_w > 1u)
         {
            if(b_queued_wt2d)
            {
               b_queued_wt2d = YAC_FALSE;
               sF32 absOff;
               if(sample->timestretch_2d_h > 1u)
                  absOff = ((wt2d_x * (sample->timestretch_2d_w-1u)) + (sUI(wt2d_y * (sample->timestretch_2d_h-1u)+0.5f) * sample->timestretch_2d_w)) * current_cyclelen;
               else
                  absOff = (wt2d_x * (sample->timestretch_2d_w-1u)) * current_cyclelen;
               mod_sampleoff_wt2d = absOff / current_sample_len;
               // Dyac_host_printf("xxx new mod_sampleoff=%f (abs=%f)\n", mod_sampleoff, absOff);
               last_relsmpoff = -1;
               calcCurrentOffset();
               cOff = current_play_offset;
            }
         }

         while(tsOff >= current_cyclelen)
         {
            tsOff -= current_cyclelen;
         }
      }

      cOff += cRate;

#if 0
      while( cOff < 0.0f )
      {
         // reverse play
         cOff += (sF64) current_sample_len;
         // TODO: handle negative sample rate + loops (reverse play)
      }
      while( ((sUI)cOff) >= current_sample_len )
      {
         if(!handleEndOfLoop(&cOff, &smpDat, &smpDatLRX, YAC_FALSE/*bFromStart*/))
         {
            // End of sample
            return;
         }
      }
#else
      if( cOff < 0.0f )
      {
         // reverse play
         // // cOff += (sF64) (current_sample_len - current_cyclelen);
         cOff += (sF64) (current_sample_len);
         if(cOff < 0)
            cOff = 0;
         // TODO: handle negative sample rate + loops (reverse play)
      }
      if( ((sUI)cOff) >= current_sample_len )
      {
         // Dyac_host_printf("xxx TS handleEndOfLoop cOff=%f current_sample_len=%u\n", cOff, current_sample_len);
         if(!handleEndOfLoop(&cOff, &smpDat, &smpDatLRX, YAC_FALSE/*bFromStart*/, YAC_TRUE/*bAllowCOffReset*/))
         {
            // End of sample
            return;
         }
         alignSampleLenToCyclelen(current_sample_len);
         // Dyac_host_printf("xxx TS 2 handleEndOfLoop cOff=%f current_sample_len=%u\n", cOff, current_sample_len);
      }
#endif


      // debug:
      // if(samples_since_cycle >= cyclePlayLen)
      // {
      //    samples_since_cycle = 0;
      // }


      // Next sample
      buf += 2;
      render_buf_sample_offset += 2u;
      blkSz--;

      if(NULL != _inputsOrNull)
      {
         for(sUI i = 0u; i < STSAMPLE_MAX_INPUTS; i++)
         {
            inputs[i]++;
         }
      }
   }

   current_play_offset = cOff;
   current_ts_offset = tsOff;

   ////printf("xxx   --> end block cOff=%f\n", cOff);
}


sUI StSampleVoice::renderFragmentOptFwdMonoSimple(sF32 *&    buf,
                                                  const sUI  _fragSize,
                                                  sUI &      genSizeHint,
                                                  sF32 *&    smpDat,
                                                  sF32 &     cVolL,
                                                  sF32 &     cVolR,
                                                  sF32 &     cRate,
                                                  const sF32**_inputsOrNull/*sz=STSAMPLE_MAX_INPUTS*/
                                                  )
{
   // mono sample, fwd play, constant freq/vol, b_timedloop=0

   sF64 cOff  = current_play_offset;
   ////sUI  numCh = sample->waveform->num_channels;

   //printf("xxx renderFragmentOptFwdMonoSimple: start=%p smpDat=%p cOff=%f\n", sample->waveform->sample_data->elements, smpDat, cOff);

   // Dyac_host_printf("xxx StSampleVoice::renderFragmentOptFwdMonoSimple: _inputsOrNull=%p\n", _inputsOrNull);
   // if(NULL != _inputsOrNull)
   // {
   //    Dyac_host_printf("xxx StSampleVoice::renderFragmentOptFwdMonoSimple: inputs[0]=%p\n", _inputsOrNull[0]);
   // }


   ////printf("xxx mixmono: coff=%f len=%u\n", cOff, current_sample_len);

   //
   // special case optimization:
   //
   //    ---- mix mono sample at constant speed/volume/pan ----
   //

   sUI smpLeft = (sUI) ( ((current_sample_len - cOff) / cRate) );

   ///printf("xxx opt: smpLeft=%d\n", smpLeft);

   /////if( ( (smpLeft < 2) || ((sUI)cOff) == (current_sample_len-1) ) )
//    if( (smpLeft < 2) ||
//        ( (cOff + smpLeft*cRate) >= (current_sample_len-1) )
//        )


   if((smpLeft-2) > _fragSize)
   {
      // Mix until end of block
      smpLeft = _fragSize;
   }
   else
   {
      // Mix until end of sample (last sample is special case because of lerp to start of loop)
      //
      // let generic renderer handle end of sample (loop) special case
      //
      genSizeHint = smpLeft;
      if(genSizeHint > _fragSize)
      {
         genSizeHint = _fragSize;
      }
      return 0;
   }


// // //xxx debug: force generic loop
// //       genSizeHint = smpLeft;
// //       if(genSizeHint > _fragSize)
// //       {
// //          genSizeHint = _fragSize;
// //       }
// //       return 0;

   sSI xxxELO = (sSI)(cOff + smpLeft*cRate);
   //printf("xxx opt: ELO=%d current_sample_len=%d\n", xxxELO);

   if( (smpLeft < 2) || (xxxELO >= (sSI)(current_sample_len-2)) )
   {
      // let generic renderer handle last sample special case
      genSizeHint = smpLeft;
      if(0 == genSizeHint)
      {
         genSizeHint = 8;
         /////printf("xxx 1 genSizeHint is 0 (should never happen ATP) current_sample_len=%d cOff=%f cRate=%f\n", current_sample_len, cOff, cRate);
      }
      if(genSizeHint > _fragSize)
      {
         genSizeHint = _fragSize;
      }

      ////printf("xxx renderFragmentOptFwdMonoSimple: fallback to generic loop: smpLeft=%u xxxELO=%d\n", smpLeft, xxxELO);
      return 0;
   }

   sUI k = smpLeft;

#ifdef USE_STATS
   stat_opt_innermix += k;
#endif // USE_STATS

   ////printf("xxx render opt forward mono\n");

   // No loop-bounds crossed during block mixing
   if(STSAMPLE_INTERPOL_LINEAR == sample->interpol_type)
   {
      while(k-- > 0)
      {
         // Note: linear sample interpolation, interpolate to right neighbour
         sUI smpOff = ((sUI)cOff);

//       if( (smpOff+2) >= current_sample_len)
//       {
//          printf("xxx wtf smpLeft=%d cOff=%f current_sample_len=%d k=%d\n", smpLeft, cOff, current_sample_len, k);
//          printf("xxx opt: ELO=%d current_sample_len=%d\n", xxxELO);
//       }
         sF32 smpFrac = sF32(cOff - smpOff);
         sF32 smpL;

         smpOff &= smpoff_mask;

         // // smpL = smpDat[smpOff + 0] * (1-smpFrac) + smpDat[smpOff + 1 + 0] * smpFrac;
         smpL = smpDat[smpOff + 0] + (smpDat[smpOff + 1 + 0] - smpDat[smpOff + 0]) * smpFrac;

         float smpR = smpL;

         if( (NULL != _inputsOrNull) && ((YAC_TRUE == sample->b_sum_input) || ((YAC_MAYBE == sample->b_sum_input) && sample->b_liverec_active)) )
         {
            if((YAC_MAYBE == sample->b_sum_input) && sample->b_liverec_active)
            {
               smpL = 0.0f;
               smpR = 0.0f;
            }

            const sF32 pan = sample->sum_input_xfade;
            const sF32 volSmp = (pan < 0.0f) ? 1.0f : (1.0f - pan);
            const sF32 volInp = (pan > 0.0f) ? 1.0f : (1.0f + pan);

            smpL = (smpL*volSmp) + _inputsOrNull[sample->liverec_input_idx][0] * volInp;

            if(sample->b_liverec_stereo)
            {
               smpR = (smpR*volSmp) + _inputsOrNull[sUI(sample->liverec_input_idx + 1) % STSAMPLE_MAX_INPUTS][0] * volInp;
            }
            else
            {
               smpR = (smpR*volSmp) + _inputsOrNull[sample->liverec_input_idx][0] * volInp;
            }
         }

         last_sample_l = smpL * cVolL;
         last_sample_r = smpR * cVolR; // mono to stereo
         buf[0] += last_sample_l;
         buf[1] += last_sample_r;

#ifdef USE_SAMPLESHIFT_EOL_XFADE_BUF
         // Store for declick xfade (sampleshift_eol)
         if(sampleshift_eol_xfade_loopstart_frameidx < VOLRAMP_NUMSTEPS)
         {
            sampleshift_eol_xfade_buf_l[sampleshift_eol_xfade_loopstart_frameidx] = last_sample_l;
            sampleshift_eol_xfade_buf_r[sampleshift_eol_xfade_loopstart_frameidx] = last_sample_r;
            sampleshift_eol_xfade_loopstart_frameidx++;
         }
#endif // USE_SAMPLESHIFT_EOL_XFADE_BUF

         // Next sample frame
         cOff += cRate;
         buf += 2;
         render_buf_sample_offset += 2u;

         if(NULL != _inputsOrNull)
         {
            for(sUI i = 0u; i < STSAMPLE_MAX_INPUTS; i++)
            {
               _inputsOrNull[i]++;
            }
         }
      }
   }
   else
   {
      // No interpolation
      while(k-- > 0)
      {
         sUI smpOff = ((sUI)cOff);

         smpOff &= smpoff_mask;

         sF32 smpL;
         smpL = smpDat[smpOff + 0];
         ////printf("xxx smpOff=%u smpL=%f cVolL=%f cVolR=%f\n", smpOff, smpL, cVolL, cVolR);
         last_sample_l = smpL * cVolL;
         last_sample_r = smpL * cVolR; // mono to stereo
         buf[0] += last_sample_l;
         buf[1] += last_sample_r;

#ifdef USE_SAMPLESHIFT_EOL_XFADE_BUF
         // Store for declick xfade (sampleshift_eol)
         if(sampleshift_eol_xfade_loopstart_frameidx < VOLRAMP_NUMSTEPS)
         {
            sampleshift_eol_xfade_buf_l[sampleshift_eol_xfade_loopstart_frameidx] = last_sample_l;
            sampleshift_eol_xfade_buf_r[sampleshift_eol_xfade_loopstart_frameidx] = last_sample_r;
            sampleshift_eol_xfade_loopstart_frameidx++;
         }
#endif // USE_SAMPLESHIFT_EOL_XFADE_BUF

         // Next sample frame
         cOff += cRate;
         buf += 2;
         render_buf_sample_offset += 2u;

         if(NULL != _inputsOrNull)
         {
            for(sUI i = 0u; i < STSAMPLE_MAX_INPUTS; i++)
            {
               _inputsOrNull[i]++;
            }
         }
      }
   }

   if( ((sUI)cOff) >= current_sample_len )
   {
      // Begin next loop(-iteration)
      //DP("1 handleEndOfLoop pre cOff=%f\n", cOff);
      if(!handleEndOfLoop(&cOff, &smpDat, NULL/*smpDatLRX*/, YAC_FALSE/*bFromStart*/, YAC_TRUE/*bAllowCOffReset*/))
      {
         // End of sample
         return 0;
      }
      //DP("1 handleEndOfLoop post cOff=%f\n", cOff);
   }

   current_play_offset = cOff;

   return smpLeft; // return #samples processed
}

void StSampleVoice::filter(sF32 *_smp) {
   if(sample->b_filter)
   {
      switch(filter_type)
      {
         default:
         case STSAMPLE_FLT_TYPE_NONE:
            break;

         case STSAMPLE_FLT_TYPE_LP:
         case STSAMPLE_FLT_TYPE_HP:
         case STSAMPLE_FLT_TYPE_BP:
         case STSAMPLE_FLT_TYPE_BR:
         case STSAMPLE_FLT_TYPE_PEQ:
         case STSAMPLE_FLT_TYPE_LSH:
         case STSAMPLE_FLT_TYPE_HSH:
            _smp[0] = biquad_b_1[0].filter(_smp[0]);
            _smp[1] = biquad_b_1[1].filter(_smp[1]);
            break;

         case STSAMPLE_FLT_TYPE_LPLP:
         case STSAMPLE_FLT_TYPE_LPHP:
         case STSAMPLE_FLT_TYPE_LPBP:
         case STSAMPLE_FLT_TYPE_LPBR:
         case STSAMPLE_FLT_TYPE_HPLP:
         case STSAMPLE_FLT_TYPE_HPHP:
         case STSAMPLE_FLT_TYPE_HPBP:
         case STSAMPLE_FLT_TYPE_HPBR:
         case STSAMPLE_FLT_TYPE_BPLP:
         case STSAMPLE_FLT_TYPE_BPHP:
         case STSAMPLE_FLT_TYPE_BPBP:
         case STSAMPLE_FLT_TYPE_BPBR:
         case STSAMPLE_FLT_TYPE_BRLP:
         case STSAMPLE_FLT_TYPE_BRHP:
         case STSAMPLE_FLT_TYPE_BRBP:
         case STSAMPLE_FLT_TYPE_BRBR:
         case STSAMPLE_FLT_TYPE_PEQ2:
         case STSAMPLE_FLT_TYPE_LSH2:
         case STSAMPLE_FLT_TYPE_HSH2:
            _smp[0] = biquad_b_1[0].filter(_smp[0]);
            _smp[0] = biquad_b_2[0].filter(_smp[0]);
            _smp[1] = biquad_b_1[1].filter(_smp[1]);
            _smp[1] = biquad_b_2[1].filter(_smp[1]);
            break;
      }
   }
}

// // #undef STSAMPLEVOICE_AI_NUM_POLES
// // #define STSAMPLEVOICE_AI_NUM_POLES 1

sF32 StSampleVoice::readSample1IntAI(const sF32 *smpDat, sSI _off) {
   sF32 r;
   sF32 fc = ai_fc;
   sF32 q = sample->ai_q;
   sUI numPoles = sMIN(STSAMPLEVOICE_AI_MAX_POLES, sample->ai_num_poles);

   for(sUI poleIdx = 0u; poleIdx < numPoles; poleIdx++)
   {
      biquad_ai[poleIdx].reset();
      biquad_ai[poleIdx].calcParamsLPFStatic(fc, q);
   }

   r = 0.0f;
   // static int xxx = 0;
   // sBool bDebug = (0 == (++xxx&1023));
   // if(bDebug)
   //    Dyac_host_printf("xxx readSample1IntAI ai_fc=%f\n", ai_fc);
   for(sSI revOff = -1; revOff <= 0; revOff++)
   {
      sF32 s = readSample1IntRaw(smpDat, _off + revOff);
      // if(bDebug)
      //    Dyac_host_printf("xxx revOff=%d absOff=%d r=%f s=%f crate=%f biquad_ai[0].cur_a=(%f;%f;%f) b=(%f;%f)\n", revOff, (_off+revOff), r, s, ai_fc, biquad_ai[0].cur_a0, biquad_ai[0].cur_a1, biquad_ai[0].cur_a2, biquad_ai[0].cur_b1, biquad_ai[0].cur_b2);
      r = biquad_ai[0].filterNoStep(s);
      for(sUI poleIdx = 1u; poleIdx < numPoles; poleIdx++)
      {
         r = biquad_ai[poleIdx].filterNoStep(r);
      }
   }
   return r;
}

sF32 StSampleVoice::readSample1IntRaw(const sF32 *smpDat, sSI _off) /*const*/ {
   sF32 r;

   if(_off < 0)
   {
      sSI i = _off + current_sample_len;
      if(i >= 0)
      {
         r = smpDat[i];
      }
      else
      {
         r = 0.0f;
      }
   }
   else if(sUI(_off) >= current_sample_len)
   {
      // Peek at next queued loop
      const sF32 *smpDatNew;
      sUI frameOffNew;
      sUI frameOffOver = sUI(_off) - current_sample_len;
      handleEndOfLoopOver(smpDat, frameOffOver, &smpDatNew, &frameOffNew);
      r = smpDatNew[frameOffNew];
      // Dyac_host_printf("xxx readSample1Int: smpDat=%p frameOffOver=%u smpDatNew=%p frameOffNew=%u r=%f\n", smpDat, frameOffOver, smpDatNew, frameOffNew, r);
   }
   else
   {
      r = smpDat[_off];
   }

   return r;
}

sF32 StSampleVoice::readSample1IntWrap(const sF32 *smpDat, sSI _off) const {
   sF32 r;

   if(_off < 0)
   {
      sSI i = _off + current_sample_len;
      if(i >= 0)
      {
         r = smpDat[i];
      }
      else
      {
         r = 0.0f;
      }
   }
   else if(sUI(_off) >= current_sample_len)
   {
      // Wrap-around
      sUI i = sUI(_off) - current_sample_len;
      if(i < current_sample_len)
      {
         r = smpDat[i];
      }
      else
      {
         r = 0.0f;
      }
   }
   else
   {
      r = smpDat[_off];
   }

   return r;
}

sF32 StSampleVoice::readSample1LRX(const sF32 *smpDat, const sF32 *smpDatLRX, sSI _off) /*const*/ {
   sF32 r;

   r =
      (0u == sample->ai_num_poles)
      ? readSample1IntRaw(smpDat, _off)
      : readSample1IntAI(smpDat, _off)
      ;

   // (todo) crossfade region around liverec_frame_offset
   if(NULL != smpDatLRX)
   {
#if 0
      sF32 n = readSample1IntWrap(smpDatLRX, _off);
      r = r + (n - r) * sample->liverec_frame_offset_xfade_amt;
#else
#define L 128
      if(sample->liverec_frame_offset >= L)
      {
         if(_off < sSI(sample->liverec_frame_offset))
         {
            sF32 n = readSample1IntWrap(smpDatLRX, _off);
            sF32 amt = sample->liverec_frame_offset_xfade_amt;
            if(_off >= (sSI(sample->liverec_frame_offset) - L))
            {
               amt *= 1.0f - ((_off - (sSI(sample->liverec_frame_offset) - L)) / sF32(L));
               // static int xxx = 0;
               // if(0 == (xxx++ & 512))
               //    Dyac_host_printf("xxx sample->liverec_frame_offset_xfade_amt=%f amt=%f\n", sample->liverec_frame_offset_xfade_amt, amt);
            }
            r = r + (n - r) * amt;
         }
#undef L
      }
#endif
   }

   return r;
}

sF32 StSampleVoice::readSample1(const sF32 *smpDat, const sF32 *smpDatLRX, sSI _off) /*const*/ {
   // read mono sample
   sF32 r;

   // don't fade oneshot loops, don't fade "attack section" of "from start" infinite loop
   sBool bAllowXfade = b_allow_xfade && (0u == current_loop_countdown);

   switch(sample->play_mode)
   {
      default:
      case STSAMPLE_PLAY_MODE_FWDNOLOOP:
      case STSAMPLE_PLAY_MODE_FORWARD:
         r = readSample1LRX(smpDat, smpDatLRX, _off);
         break;

      case STSAMPLE_PLAY_MODE_BACKWARD:
         r = readSample1LRX(smpDat, smpDatLRX, sSI(current_sample_len) - _off - 1);
         break;

      case STSAMPLE_PLAY_MODE_PINGPONG:
         if(b_fwd)
            r = readSample1LRX(smpDat, smpDatLRX, _off);
         else
            r = readSample1LRX(smpDat, smpDatLRX, sSI(current_sample_len) - _off - 1);
         break;

      case STSAMPLE_PLAY_MODE_XFADE_ALL:
         r = readSample1LRX(smpDat, smpDatLRX, _off);
         if(bAllowXfade)
         {
            sF32 t = readSample1LRX(smpDat, smpDatLRX, (sSI(current_sample_len) - _off - 1));
            sF32 amt = sF32(_off) / sF32(current_sample_len);
            r = r + (t - r) * amt;
         }
         break;

      case STSAMPLE_PLAY_MODE_XFADE_16:
         r = readSample1LRX(smpDat, smpDatLRX, _off);
         if(bAllowXfade)
         {
#define L 16
            if((current_sample_len >= L) && ((current_smpdat_shift + current_sample_offset) >= L) && (_off >= (sSI(current_sample_len) - L)))
            {
               sSI winOff = _off - (((sSI)current_sample_len) - L);
               sF32 t = readSample1LRX(smpDat - L,  (NULL != smpDatLRX) ? (smpDatLRX - L) : NULL, winOff);
               sF32 amt = sF32(winOff) / sF32(L);
               r = r + (t - r) * amt;
            }
#undef L
         }
         break;

      case STSAMPLE_PLAY_MODE_XFADE_32:
         r = readSample1LRX(smpDat, smpDatLRX, _off);
         if(bAllowXfade)
         {
#define L 32
            if((current_sample_len >= L) && ((current_smpdat_shift + current_sample_offset) >= L) && (_off >= (sSI(current_sample_len) - L)))
            {
               sSI winOff = _off - (((sSI)current_sample_len) - L);
               sF32 t = readSample1LRX(smpDat - L,  (NULL != smpDatLRX) ? (smpDatLRX - L) : NULL, winOff);
               sF32 amt = sF32(winOff) / sF32(L);
               r = r + (t - r) * amt;
            }
#undef L
         }
         break;

      case STSAMPLE_PLAY_MODE_XFADE_64:
         r = readSample1LRX(smpDat, smpDatLRX, _off);
         if(bAllowXfade)
         {
#define L 64
            if((current_sample_len >= L) && ((current_smpdat_shift + current_sample_offset) >= L) && (_off >= (sSI(current_sample_len) - L)))
            {
               sSI winOff = _off - (((sSI)current_sample_len) - L);
               sF32 t = readSample1LRX(smpDat - L,  (NULL != smpDatLRX) ? (smpDatLRX - L) : NULL, winOff);
               sF32 amt = sF32(winOff) / sF32(L);
               r = r + (t - r) * amt;
            }
#undef L
         }
         break;

      case STSAMPLE_PLAY_MODE_XFADE_128:
         r = readSample1LRX(smpDat, smpDatLRX, _off);
         if(bAllowXfade)
         {
#define L 128
            if((current_sample_len >= L) && ((current_smpdat_shift + current_sample_offset) >= L) && (_off >= (sSI(current_sample_len) - L)))
            {
               sSI winOff = _off - (((sSI)current_sample_len) - L);
               sF32 t = readSample1LRX(smpDat - L,  (NULL != smpDatLRX) ? (smpDatLRX - L) : NULL, winOff);
               sF32 amt = sF32(winOff) / sF32(L);
               r = r + (t - r) * amt;
            }
#undef L
         }
         break;

      case STSAMPLE_PLAY_MODE_XFADE_256:
         r = readSample1LRX(smpDat, smpDatLRX, _off);
         if(bAllowXfade)
         {
#define L 256
            if((current_sample_len >= L) && ((current_smpdat_shift + current_sample_offset) >= L) && (_off >= (sSI(current_sample_len) - L)))
            {
               sSI winOff = _off - (((sSI)current_sample_len) - L);
               sF32 t = readSample1LRX(smpDat - L,  (NULL != smpDatLRX) ? (smpDatLRX - L) : NULL, winOff);
               sF32 amt = sF32(winOff) / sF32(L);
               r = r + (t - r) * amt;
            }
#undef L
         }
         break;

      case STSAMPLE_PLAY_MODE_XFADE_512:
         r = readSample1LRX(smpDat, smpDatLRX, _off);
         if(bAllowXfade)
         {
#define L 512
            if((current_sample_len >= L) && ((current_smpdat_shift + current_sample_offset) >= L) && (_off >= (sSI(current_sample_len) - L)))
            {
               sSI winOff = _off - (((sSI)current_sample_len) - L);
               sF32 t = readSample1LRX(smpDat - L, (NULL != smpDatLRX) ? (smpDatLRX - L) : NULL, winOff);
               sF32 amt = sF32(winOff) / sF32(L);
               r = r + (t - r) * amt;
            }
#undef L
         }
         break;

      case STSAMPLE_PLAY_MODE_XFADE_1024:
         r = readSample1LRX(smpDat, smpDatLRX, _off);
         if(bAllowXfade)
         {
#define L 1024
            if((current_sample_len >= L) && ((current_smpdat_shift + current_sample_offset) >= L) && (_off >= (sSI(current_sample_len) - L)))
            {
               sSI winOff = _off - (((sSI)current_sample_len) - L);
               sF32 t = readSample1LRX(smpDat - L, (NULL != smpDatLRX) ? (smpDatLRX - L) : NULL, winOff);
               sF32 amt = sF32(winOff) / sF32(L);
               r = r + (t - r) * amt;
            }
#undef L
         }
         break;

      case STSAMPLE_PLAY_MODE_XFADE_2048:
         r = readSample1LRX(smpDat, smpDatLRX, _off);
         if(bAllowXfade)
         {
#define L 2048
            if((current_sample_len >= L) && ((current_smpdat_shift + current_sample_offset) >= L) && (_off >= (sSI(current_sample_len) - L)))
            {
               sSI winOff = _off - (((sSI)current_sample_len) - L);
               sF32 t = readSample1LRX(smpDat - L,  (NULL != smpDatLRX) ? (smpDatLRX - L) : NULL, winOff);
               sF32 amt = sF32(winOff) / sF32(L);
               r = r + (t - r) * amt;
            }
#undef L
         }
         break;

      case STSAMPLE_PLAY_MODE_XFADE_4096:
         r = readSample1LRX(smpDat, smpDatLRX, _off);
         if(bAllowXfade)
         {
#define L 4096
            if((current_sample_len >= L) && ((current_smpdat_shift + current_sample_offset) >= L) && (_off >= (sSI(current_sample_len) - L)))
            {
               sSI winOff = _off - (((sSI)current_sample_len) - L);
               sF32 t = readSample1LRX(smpDat - L, (NULL != smpDatLRX) ? (smpDatLRX - L) : NULL, winOff);
               sF32 amt = sF32(winOff) / sF32(L);
               r = r + (t - r) * amt;
            }
#undef L
         }
         break;

      case STSAMPLE_PLAY_MODE_XFADE_8192:
         r = readSample1LRX(smpDat, smpDatLRX, _off);
         if(bAllowXfade)
         {
#define L 8192
            if((current_sample_len >= L) && ((current_smpdat_shift + current_sample_offset) >= L) && (_off >= (sSI(current_sample_len) - L)))
            {
               // abcd
               sSI winOff = _off - (((sSI)current_sample_len) - L);
               // static int xxx = 0;
               // if(0 == (++xxx & 255))
               //    Dyac_host_printf("xxx winOff=%d current_sample_offset=%u current_sample_len=%u smpDat=%p smpDatLRX=%p\n", winOff, current_sample_offset, current_sample_len, smpDat, smpDatLRX);
               // sF32 t = readSample1LRX(smpDat - L,  (NULL != smpDatLRX) ? (smpDatLRX - L) : NULL, winOff);
               sF32 t = 0.0f;
               sF32 amt = sF32(winOff) / sF32(L);
               r = r + (t - r) * amt;
            }
#undef L
         }
         break;

      case STSAMPLE_PLAY_MODE_XFADE_16384:
         r = readSample1LRX(smpDat, smpDatLRX, _off);
         if(bAllowXfade)
         {
#define L 16384
            if((current_sample_len >= L) && ((current_smpdat_shift + current_sample_offset) >= L) && (_off >= (sSI(current_sample_len) - L)))
            {
               sSI winOff = _off - (((sSI)current_sample_len) - L);
               sF32 t = readSample1LRX(smpDat - L,  (NULL != smpDatLRX) ? (smpDatLRX - L) : NULL, winOff);
               sF32 amt = sF32(winOff) / sF32(L);
               r = r + (t - r) * amt;
            }
#undef L
         }
         break;
   }

   return r;
}

sF32 StSampleVoice::readSample2IntAI(const sF32 *smpDat, sSI _off) {
   // (note) off is in samples, *not* frames
   sF32 r;
   sF32 fc = ai_fc;
   sF32 q = sample->ai_q;
   sUI numPoles = sMIN(STSAMPLEVOICE_AI_MAX_POLES, sample->ai_num_poles);

   for(sUI poleIdx = 0u; poleIdx < numPoles; poleIdx++)
   {
      biquad_ai[poleIdx].reset();
      biquad_ai[poleIdx].calcParamsLPFStatic(fc, q);
   }

   r = 0.0f;
   // static int xxx = 0;
   // sBool bDebug = (0 == (++xxx&1023));
   // if(bDebug)
   //    Dyac_host_printf("xxx readSample1Int AI ai_fc=%f\n", ai_fc);
   for(sSI revOff = -1*2; revOff <= 0; revOff+=2)
   {
      sF32 s = readSample2IntRaw(smpDat, _off + revOff);
      // if(bDebug)
      //    Dyac_host_printf("xxx revOff=%d absOff=%d r=%f s=%f crate=%f biquad_ai[0].cur_a=(%f;%f;%f) b=(%f;%f)\n", revOff, (_off+revOff), r, s, ai_fc, biquad_ai[0].cur_a0, biquad_ai[0].cur_a1, biquad_ai[0].cur_a2, biquad_ai[0].cur_b1, biquad_ai[0].cur_b2);
      r = biquad_ai[0].filterNoStep(s);
      for(sUI poleIdx = 1u; poleIdx < numPoles; poleIdx++)
      {
         r = biquad_ai[poleIdx].filterNoStep(r);
      }
   }
   return r;
}

sF32 StSampleVoice::readSample2IntRaw(const sF32 *smpDat, sSI _off) /*const*/ {
   // (note) off is in samples, *not* frames
   sF32 r;

   sUI len = (current_sample_len << 1);  // *2 (interleaved l/r)

   if(_off < 0)
   {
      sSI i = _off + sSI(len);
      if(i >= 0)
      {
         r = smpDat[i];
      }
      else
      {
         r = 0.0f;
      }
   }
   else if(sUI(_off) >= len)
   {
      // Peek at next queued loop
      sUI lrOff = sUI(_off) & 1u;
      sUI frameOff = sUI(_off) >> 1u;
      sUI frameOffOver = frameOff - current_sample_len;
      const sF32 *smpDatNew;
      sUI frameOffNew;
      handleEndOfLoopOver(smpDat, frameOffOver, &smpDatNew, &frameOffNew);
      r = smpDatNew[(frameOffNew << 1) + lrOff];
   }
   else
   {
      r = smpDat[_off];
   }

   return r;
}

sF32 StSampleVoice::readSample2IntWrap(const sF32 *smpDat, sSI _off) const {
   // (note) off is in samples, *not* frames
   sF32 r;

   sUI len = (current_sample_len << 1);  // *2 (interleaved l/r)

   if(_off < 0)
   {
      sSI i = _off + sSI(len);
      if(i >= 0)
      {
         r = smpDat[i];
      }
      else
      {
         r = 0.0f;
      }
   }
   else if(sUI(_off) >= len)
   {
      // wrap-around
      sUI i = sUI(_off) - len;
      if(i < len)
      {
         r = smpDat[i];
      }
      else
      {
         r = 0.0f;
      }
   }
   else
   {
      r = smpDat[_off];
   }

   return r;
}

sF32 StSampleVoice::readSample2LRX(const sF32 *smpDat, const sF32 *smpDatLRX, sSI _off) /*const*/ {
   sF32 r;

   r =
      (0u == sample->ai_num_poles)
      ? readSample2IntRaw(smpDat, _off)
      : readSample2IntAI(smpDat, _off)
      ;

   // (todo) crossfade region around liverec_frame_offset
   if(NULL != smpDatLRX)
   {
#if 0
      sF32 n = readSample2IntWrap(smpDatLRX, _off);
      r = r + (n - r) * sample->liverec_frame_offset_xfade_amt;
#else
#define L 1024
      if(sample->liverec_frame_offset >= L)
      {
         sSI frameOff = _off >> 1;
         if(frameOff < sSI(sample->liverec_frame_offset))
         {
            if(frameOff >= (sSI(sample->liverec_frame_offset) - L))
            {
               sF32 n = readSample2IntWrap(smpDatLRX, _off);
               sF32 amt = 1.0f - ((frameOff - (sSI(sample->liverec_frame_offset) - L)) / sF32(L));
               amt *= sample->liverec_frame_offset_xfade_amt;
               // static int xxx = 0;
               // if(0 == (xxx++ & 512))
               //    Dyac_host_printf("xxx sample->liverec_frame_offset_xfade_amt=%f amt=%f\n", sample->liverec_frame_offset_xfade_amt, amt);
               r = r + (n - r) * amt;
            }
         }
      }
#undef L
#endif
   }
   return r;
}

sF32 StSampleVoice::readSample2(const sF32 *smpDat, const sF32 *smpDatLRX, sSI _off) /*const*/ {
   sF32 r;

   sSI frameOff = _off >> 1;
   sSI revOff;

   // don't fade oneshot loops, don't fade "attack section" of "from start" infinite loop
   sBool bAllowXfade = b_allow_xfade && (0u == current_loop_countdown);

   switch(sample->play_mode)
   {
      default:
      case STSAMPLE_PLAY_MODE_FWDNOLOOP:
      case STSAMPLE_PLAY_MODE_FORWARD:
         r = readSample2LRX(smpDat, smpDatLRX, _off);
         break;

      case STSAMPLE_PLAY_MODE_BACKWARD:
         revOff = ((sSI(current_sample_len) - frameOff - 1) << 1) + (_off & 1);
         r = readSample2LRX(smpDat, smpDatLRX, revOff);
         break;

      case STSAMPLE_PLAY_MODE_PINGPONG:
         if(b_fwd)
         {
            r = readSample2LRX(smpDat, smpDatLRX, _off);
         }
         else
         {
            revOff = ((sSI(current_sample_len) - frameOff - 1) << 1) + (_off & 1);
            r = readSample2LRX(smpDat, smpDatLRX, revOff);
         }
         break;

      case STSAMPLE_PLAY_MODE_XFADE_ALL:
         r = readSample2LRX(smpDat, smpDatLRX, _off);
         if(bAllowXfade)
         {
            revOff = ((sSI(current_sample_len) - frameOff - 1) << 1) + (_off & 1);
            sF32 t = readSample2LRX(smpDat, smpDatLRX, revOff);
            sF32 amt = sF32(frameOff) / sF32(current_sample_len);
            r = r + (t - r) * amt;
         }
         break;

      case STSAMPLE_PLAY_MODE_XFADE_16:
         r = readSample2LRX(smpDat, smpDatLRX, _off);
         if(bAllowXfade)
         {
#define L 16
            if((current_sample_len >= L) && ((current_smpdat_shift + current_sample_offset) >= L) && (frameOff >= (sSI(current_sample_len) - L)))
            {
               sSI winOff = frameOff - (((sSI)current_sample_len) - L);
               sF32 t = readSample2LRX(smpDat - (L<<1),  (NULL != smpDatLRX) ? (smpDatLRX - (L<<1)) : NULL, (winOff<<1) + (_off & 1));
               sF32 amt = sF32(winOff) / sF32(L);
               r = r + (t - r) * amt;
            }
#undef L
         }
         break;

      case STSAMPLE_PLAY_MODE_XFADE_32:
         r = readSample2LRX(smpDat, smpDatLRX, _off);
         if(bAllowXfade)
         {
#define L 32
            if((current_sample_len >= L) && ((current_smpdat_shift + current_sample_offset) >= L) && (frameOff >= (sSI(current_sample_len) - L)))
            {
               sSI winOff = frameOff - (((sSI)current_sample_len) - L);
               sF32 t = readSample2LRX(smpDat - (L<<1),  (NULL != smpDatLRX) ? (smpDatLRX - (L<<1)) : NULL, (winOff<<1) + (_off & 1));
               sF32 amt = sF32(winOff) / sF32(L);
               r = r + (t - r) * amt;
            }
#undef L
         }
         break;

      case STSAMPLE_PLAY_MODE_XFADE_64:
         r = readSample2LRX(smpDat, smpDatLRX, _off);
         if(bAllowXfade)
         {
#define L 64
            if((current_sample_len >= L) && ((current_smpdat_shift + current_sample_offset) >= L) && (frameOff >= (sSI(current_sample_len) - L)))
            {
               sSI winOff = frameOff - (((sSI)current_sample_len) - L);
               sF32 t = readSample2LRX(smpDat - (L<<1),  (NULL != smpDatLRX) ? (smpDatLRX - (L<<1)) : NULL, (winOff<<1) + (_off & 1));
               sF32 amt = sF32(winOff) / sF32(L);
               r = r + (t - r) * amt;
            }
#undef L
         }
         break;

      case STSAMPLE_PLAY_MODE_XFADE_128:
         r = readSample2LRX(smpDat, smpDatLRX, _off);
         if(bAllowXfade)
         {
#define L 128
            if((current_sample_len >= L) && ((current_smpdat_shift + current_sample_offset) >= L) && (frameOff >= (sSI(current_sample_len) - L)))
            {
               sSI winOff = frameOff - (((sSI)current_sample_len) - L);
               sF32 t = readSample2LRX(smpDat - (L<<1),  (NULL != smpDatLRX) ? (smpDatLRX - (L<<1)) : NULL, (winOff<<1) + (_off & 1));
               sF32 amt = sF32(winOff) / sF32(L);
               r = r + (t - r) * amt;
            }
#undef L
         }
         break;

      case STSAMPLE_PLAY_MODE_XFADE_256:
         r = readSample2LRX(smpDat, smpDatLRX, _off);
         if(bAllowXfade)
         {
#define L 256
            if((current_sample_len >= L) && ((current_smpdat_shift + current_sample_offset) >= L) && (frameOff >= (sSI(current_sample_len) - L)))
            {
               sSI winOff = frameOff - (((sSI)current_sample_len) - L);
               sF32 t = readSample2LRX(smpDat - (L<<1),  (NULL != smpDatLRX) ? (smpDatLRX - (L<<1)) : NULL, (winOff<<1) + (_off & 1));
               sF32 amt = sF32(winOff) / sF32(L);
               r = r + (t - r) * amt;
            }
#undef L
         }
         break;

      case STSAMPLE_PLAY_MODE_XFADE_512:
         r = readSample2LRX(smpDat, smpDatLRX, _off);
         if(bAllowXfade)
         {
#define L 512
            if((current_sample_len >= L) && ((current_smpdat_shift + current_sample_offset) >= L) && (frameOff >= (sSI(current_sample_len) - L)))
            {
               sSI winOff = frameOff - (((sSI)current_sample_len) - L);
               sF32 t = readSample2LRX(smpDat - (L<<1),  (NULL != smpDatLRX) ? (smpDatLRX - (L<<1)) : NULL, (winOff<<1) + (_off & 1));
               sF32 amt = sF32(winOff) / sF32(L);
               r = r + (t - r) * amt;
            }
#undef L
         }
         break;

      case STSAMPLE_PLAY_MODE_XFADE_1024:
         r = readSample2LRX(smpDat, smpDatLRX, _off);
         if(bAllowXfade)
         {
#define L 1024
            if((current_sample_len >= L) && ((current_smpdat_shift + current_sample_offset) >= L) && (frameOff >= (sSI(current_sample_len) - L)))
            {
               sSI winOff = frameOff - (((sSI)current_sample_len) - L);
               sF32 t = readSample2LRX(smpDat - (L<<1),  (NULL != smpDatLRX) ? (smpDatLRX - (L<<1)) : NULL, (winOff<<1) + (_off & 1));
               sF32 amt = sF32(winOff) / sF32(L);
               r = r + (t - r) * amt;
            }
#undef L
         }
         break;

      case STSAMPLE_PLAY_MODE_XFADE_2048:
         r = readSample2LRX(smpDat, smpDatLRX, _off);
         if(bAllowXfade)
         {
#define L 2048
            if((current_sample_len >= L) && ((current_smpdat_shift + current_sample_offset) >= L) && (frameOff >= (sSI(current_sample_len) - L)))
            {
               sSI winOff = frameOff - (((sSI)current_sample_len) - L);
               sF32 t = readSample2LRX(smpDat - (L<<1),  (NULL != smpDatLRX) ? (smpDatLRX - (L<<1)) : NULL, (winOff<<1) + (_off & 1));
               sF32 amt = sF32(winOff) / sF32(L);
               r = r + (t - r) * amt;
            }
#undef L
         }
         break;

      case STSAMPLE_PLAY_MODE_XFADE_4096:
         r = readSample2LRX(smpDat, smpDatLRX, _off);
         if(bAllowXfade)
         {
#define L 4096
            if((current_sample_len >= L) && ((current_smpdat_shift + current_sample_offset) >= L) && (frameOff >= (sSI(current_sample_len) - L)))
            {
               sSI winOff = frameOff - (((sSI)current_sample_len) - L);
               sF32 t = readSample2LRX(smpDat - (L<<1),  (NULL != smpDatLRX) ? (smpDatLRX - (L<<1)) : NULL, (winOff<<1) + (_off & 1));
               sF32 amt = sF32(winOff) / sF32(L);
               r = r + (t - r) * amt;
            }
#undef L
         }
         break;

      case STSAMPLE_PLAY_MODE_XFADE_8192:
         r = readSample2LRX(smpDat, smpDatLRX, _off);
         if(bAllowXfade)
         {
#define L 8192
            if((current_sample_len >= L) && ((current_smpdat_shift + current_sample_offset) >= L) && (frameOff >= (sSI(current_sample_len) - L)))
            {
               sSI winOff = frameOff - (((sSI)current_sample_len) - L);
               sF32 t = readSample2LRX(smpDat - (L<<1),  (NULL != smpDatLRX) ? (smpDatLRX - (L<<1)) : NULL, (winOff<<1) + (_off & 1));
               sF32 amt = sF32(winOff) / sF32(L);
               r = r + (t - r) * amt;
            }
#undef L
         }
         break;

      case STSAMPLE_PLAY_MODE_XFADE_16384:
         r = readSample2LRX(smpDat, smpDatLRX, _off);
         if(bAllowXfade)
         {
#define L 16384
            if((current_sample_len >= L) && ((current_smpdat_shift + current_sample_offset) >= L) && (frameOff >= (sSI(current_sample_len) - L)))
            {
               sSI winOff = frameOff - (((sSI)current_sample_len) - L);
               sF32 t = readSample2LRX(smpDat - (L<<1),  (NULL != smpDatLRX) ? (smpDatLRX - (L<<1)) : NULL, (winOff<<1) + (_off & 1));
               sF32 amt = sF32(winOff) / sF32(L);
               r = r + (t - r) * amt;
            }
#undef L
         }
         break;
   }

   return r;
}

sUI StSampleVoice::renderFragmentGeneric(sF32 *&    buf,
                                         const sUI  _fragSize,
                                         sF32 *&    smpDat,
                                         sF32 *&    smpDatLRX,
                                         sF32 &     cVolL,
                                         sF32 &     cVolR,
                                         const sF32 sVolL,
                                         const sF32 sVolR,
                                         sF32 &     cRate,
                                         const sF32 sRate,
                                         const sF32**_inputsOrNull/*sz=STSAMPLE_MAX_INPUTS*/
                                         )
{
   // Dyac_host_printf("xxx voice renderFragmentGeneric ENTER\n");
   // yac_host->printf("xxx renderFragmentGeneric: fragSize=%u\n", _fragSize);

   sF64 cOff  = current_play_offset;
   sUI  numCh = sample->waveform->num_channels;

   // static int xxx = 0;
   // if((++xxx & 127) == 0)
   // {
   // Dyac_host_printf("xxx cRate=%f sRate=%f\n", cRate, sRate);
   //    Dyac_host_printf("xxx renderFragmentGeneric: start=%p smpDat=%p cOff=%f\n", sample->waveform->sample_data->elements, smpDat, cOff);
   // }


   // (slow) generic loop
   sUI fragSize = _fragSize;

   sF64 fetchCycleOff = (mmdst_cyclelen_sampleoff * current_sample_len);
   if(fetchCycleOff < 0.0)
      fetchCycleOff = 0.0;
   else if(fetchCycleOff >= current_sample_len)
      fetchCycleOff = current_sample_len;

   sUI startEndFadeNumFrames = sUI(sample->startend_fade_num_frames * mod_cyclelen * sample_player->mod_cyclelen * mmdst_cyclelen * cur_cyclelen_eol);

   // if(fetchCycleOff > 0.0)
   //    yac_host->printf("xxx fetchCycleOff=%f mmdst_cyclelen_sampleoff=%f current_sample_len=%u\n", fetchCycleOff, mmdst_cyclelen_sampleoff, current_sample_len);

   while(fragSize > 0)
   {
      sF32 fadeVol;

      sF64 fetchOff = cOff + fetchCycleOff;
      if(fetchOff >= current_sample_len)
         fetchOff -= current_sample_len;

      ////printf("xxx fadein=%d fadeout=%d\n", fadein_countdown, fadeout_countdown);

      if(fadein_countdown > 0)
      {
         fadein_countdown--;
         fadeVol = ( ((sF32)(VOLRAMP_NUMSTEPS - fadein_countdown)) / VOLRAMP_NUMSTEPS);
      }
      else if(fadeout_countdown > 0)
      {
         fadeout_countdown--;
         fadeVol = ( ((sF32)fadeout_countdown) / VOLRAMP_NUMSTEPS);
      }
      else if(b_fading_out)
      {
         fadeVol = 0.0f;
      }
      else
      {
         fadeVol = 1.0f;
      }

      sUI intOff = sUI(fetchOff);
      // Dyac_host_printf("xxx fetchOff=%f intOff=%d\n", fetchOff, intOff);

      // [01Aug2021] wrap-around is handled in readSample*()
      // // if(intOff >= current_sample_len)
      // // {
      // //    intOff = current_sample_len-1;
      // // }

      sUI smpOff = intOff * numCh;
      sF32 smpFrac = sF32(fetchOff - intOff);
      if(sample->interpol_order > 0u)
      {
         smpFrac = FracSmooth(smpFrac, sample->interpol_order);
      }

      sUI smpOffR;

      smpOff &= smpoff_mask;

      // [01Aug2021] wrap-around is handled in readSample*()
      // // if((intOff + 1u) >= current_sample_len)
      // // {
      // //    Dyac_host_printf("xxx loop intOff=%u current_sample_len=%u\n", intOff, current_sample_len);

      // //    // interpolate to current loop start (ideally: start of next loop..)
      // //    if(sample->b_timestretch || (current_sample_len <= 1024))
      // //    {
      // //       smpOffR = 0;
      // //    }
      // //    else
      // //    {
      // //       // workaround to avoid clicks at end of oneshot samples, whose first sample value is != 0
      // //       smpOffR = smpOff;
      // //    }
      // // }
      // // else
      // // {
      // //    // interpolate to next neighbour sample
      // //    smpOffR = smpOff + numCh;
      // // }
      smpOffR = smpOff + numCh;  // (todo) rename smpOffR to smpOffN

      ////Dyac_host_printf("xxx smpDat=%p cRate=%f cVolL=%f cVolR=%f cOff=%f smpOff=%u/%u\n", smpDat, cRate, cVolL, cVolR, cOff, smpOff, current_sample_len);

      sF32 smpL;
      sF32 smpLn;
      sF32 smpR;
      sF32 smpRn;

      ////printf("xxx smpL=%f cVolL=%f smpFrac=%f fadeVol=%f smpDat=%p zero=%p\n", smpL, cVolL, smpFrac, fadeVol, smpDat, zero_samples);

      if(STSAMPLE_INTERPOL_LINEAR == sample->interpol_type)
      {
         if(numCh > 1)
         {
            smpL  = readSample2(smpDat, smpDatLRX, smpOff  + 0);
            smpLn = readSample2(smpDat, smpDatLRX, smpOffR + 0);

            smpR  = readSample2(smpDat, smpDatLRX, smpOff  + 1);
            smpRn = readSample2(smpDat, smpDatLRX, smpOffR + 1);

            if(br_mask)
            {
               smpL  = BitReduce(smpL,  sample->br_preamp, br_mask);
               smpLn = BitReduce(smpLn, sample->br_preamp, br_mask);

               smpR  = BitReduce(smpR,  sample->br_preamp, br_mask);
               smpRn = BitReduce(smpRn, sample->br_preamp, br_mask);
            }

            smpL = smpL + (smpLn - smpL) * smpFrac;
            smpR = smpR + (smpRn - smpR) * smpFrac;
         }
         else
         {
            smpL  = readSample1(smpDat, smpDatLRX, smpOff  + 0);
            smpLn = readSample1(smpDat, smpDatLRX, smpOffR + 0);

            if(br_mask)
            {
               smpL  = BitReduce(smpL,  sample->br_preamp, br_mask);
               smpLn = BitReduce(smpLn, sample->br_preamp, br_mask);
            }

            smpL = smpL + (smpLn - smpL) * smpFrac;
            smpR = smpL; // mono to stereo
         }
      }
      else if(STSAMPLE_INTERPOL_VSR2 == sample->interpol_type)
      {
#define VSR_NUM 2
         sF64 fetchOffC = fetchOff;
         const sF64 cRateVSR  = cRate * (1.0f / VSR_NUM);

         smpL = 0.0f;
         smpR = 0.0f;

         for(sUI i = 0u; i < VSR_NUM; i++)
         {
            const sUI intOffC = sUI(fetchOffC) & smpoff_mask;

            if(1u == numCh)
            {
               // Mono
               const sUI smpOffC = intOffC;

               sF32 readL = readSample1(smpDat, smpDatLRX, smpOffC + 0u);

               if(br_mask)
               {
                  readL = BitReduce(readL, sample->br_preamp, br_mask);
               }

               smpL += readL;
            }
            else
            {
               // Stereo
               sUI smpOffC = intOffC * numCh;

               sF32 readL = readSample2(smpDat, smpDatLRX, smpOffC + 0u);
               sF32 readR = readSample2(smpDat, smpDatLRX, smpOffC + 1u);

               if(br_mask)
               {
                  readL = BitReduce(readL, sample->br_preamp, br_mask);
                  readR = BitReduce(readR, sample->br_preamp, br_mask);
               }

               smpL += readL;
               smpR += readR;
            }

            fetchOffC += cRateVSR;
            if(fetchOffC >= current_sample_len)
               fetchOffC -= current_sample_len;
         }

         if(1u == numCh)
         {
            smpL *= (1.0f / VSR_NUM);
            smpR = smpL; // mono to stereo
         }
         else
         {
            smpL *= (1.0f / VSR_NUM);
            smpR *= (1.0f / VSR_NUM);
         }
#undef VSR_NUM
      }
      else if(STSAMPLE_INTERPOL_VSR4 == sample->interpol_type)
      {
#define VSR_NUM 4
         sF64 fetchOffC = fetchOff;
         const sF64 cRateVSR  = cRate * (1.0f / VSR_NUM);

         smpL = 0.0f;
         smpR = 0.0f;

         for(sUI i = 0u; i < VSR_NUM; i++)
         {
            const sUI intOffC = sUI(fetchOffC) & smpoff_mask;

            if(1u == numCh)
            {
               // Mono
               const sUI smpOffC = intOffC;

               sF32 readL = readSample1(smpDat, smpDatLRX, smpOffC + 0u);

               if(br_mask)
               {
                  readL = BitReduce(readL, sample->br_preamp, br_mask);
               }

               smpL += readL;
            }
            else
            {
               // Stereo
               sUI smpOffC = intOffC * numCh;

               sF32 readL = readSample2(smpDat, smpDatLRX, smpOffC + 0u);
               sF32 readR = readSample2(smpDat, smpDatLRX, smpOffC + 1u);

               if(br_mask)
               {
                  readL = BitReduce(readL, sample->br_preamp, br_mask);
                  readR = BitReduce(readR, sample->br_preamp, br_mask);
               }

               smpL += readL;
               smpR += readR;
            }

            fetchOffC += cRateVSR;
            if(fetchOffC >= current_sample_len)
               fetchOffC -= current_sample_len;
         }

         if(1u == numCh)
         {
            smpL *= (1.0f / VSR_NUM);
            smpR = smpL; // mono to stereo
         }
         else
         {
            smpL *= (1.0f / VSR_NUM);
            smpR *= (1.0f / VSR_NUM);
         }
#undef VSR_NUM
      }
      else if(STSAMPLE_INTERPOL_VSR8 == sample->interpol_type)
      {
#define VSR_NUM 8
         sF64 fetchOffC = fetchOff;
         const sF64 cRateVSR  = cRate * (1.0f / VSR_NUM);

         smpL = 0.0f;
         smpR = 0.0f;

         for(sUI i = 0u; i < VSR_NUM; i++)
         {
            const sUI intOffC = sUI(fetchOffC) & smpoff_mask;

            if(1u == numCh)
            {
               // Mono
               const sUI smpOffC = intOffC;

               sF32 readL = readSample1(smpDat, smpDatLRX, smpOffC + 0u);

               if(br_mask)
               {
                  readL = BitReduce(readL, sample->br_preamp, br_mask);
               }

               smpL += readL;
            }
            else
            {
               // Stereo
               sUI smpOffC = intOffC * numCh;

               sF32 readL = readSample2(smpDat, smpDatLRX, smpOffC + 0u);
               sF32 readR = readSample2(smpDat, smpDatLRX, smpOffC + 1u);

               if(br_mask)
               {
                  readL = BitReduce(readL, sample->br_preamp, br_mask);
                  readR = BitReduce(readR, sample->br_preamp, br_mask);
               }

               smpL += readL;
               smpR += readR;
            }

            fetchOffC += cRateVSR;
            if(fetchOffC >= current_sample_len)
               fetchOffC -= current_sample_len;
         }

         if(1u == numCh)
         {
            smpL *= (1.0f / VSR_NUM);
            smpR = smpL; // mono to stereo
         }
         else
         {
            smpL *= (1.0f / VSR_NUM);
            smpR *= (1.0f / VSR_NUM);
         }
#undef VSR_NUM
      }
      else if(STSAMPLE_INTERPOL_VSR16 == sample->interpol_type)
      {
#define VSR_NUM 16
         sF64 fetchOffC = fetchOff;
         const sF64 cRateVSR  = cRate * (1.0f / VSR_NUM);

         smpL = 0.0f;
         smpR = 0.0f;

         for(sUI i = 0u; i < VSR_NUM; i++)
         {
            const sUI intOffC = sUI(fetchOffC) & smpoff_mask;

            if(1u == numCh)
            {
               // Mono
               const sUI smpOffC = intOffC;

               sF32 readL = readSample1(smpDat, smpDatLRX, smpOffC + 0u);

               if(br_mask)
               {
                  readL = BitReduce(readL, sample->br_preamp, br_mask);
               }

               smpL += readL;
            }
            else
            {
               // Stereo
               sUI smpOffC = intOffC * numCh;

               sF32 readL = readSample2(smpDat, smpDatLRX, smpOffC + 0u);
               sF32 readR = readSample2(smpDat, smpDatLRX, smpOffC + 1u);

               if(br_mask)
               {
                  readL = BitReduce(readL, sample->br_preamp, br_mask);
                  readR = BitReduce(readR, sample->br_preamp, br_mask);
               }

               smpL += readL;
               smpR += readR;
            }

            fetchOffC += cRateVSR;
            if(fetchOffC >= current_sample_len)
               fetchOffC -= current_sample_len;
         }

         if(1u == numCh)
         {
            smpL *= (1.0f / VSR_NUM);
            smpR = smpL; // mono to stereo
         }
         else
         {
            smpL *= (1.0f / VSR_NUM);
            smpR *= (1.0f / VSR_NUM);
         }
#undef VSR_NUM
      }
      else if(STSAMPLE_INTERPOL_VSR32 == sample->interpol_type)
      {
#define VSR_NUM 32
         sF64 fetchOffC = fetchOff;
         const sF64 cRateVSR  = cRate * (1.0f / VSR_NUM);

         smpL = 0.0f;
         smpR = 0.0f;

         for(sUI i = 0u; i < VSR_NUM; i++)
         {
            const sUI intOffC = sUI(fetchOffC) & smpoff_mask;

            if(1u == numCh)
            {
               // Mono
               const sUI smpOffC = intOffC;

               sF32 readL = readSample1(smpDat, smpDatLRX, smpOffC + 0u);

               if(br_mask)
               {
                  readL = BitReduce(readL, sample->br_preamp, br_mask);
               }

               smpL += readL;
            }
            else
            {
               // Stereo
               sUI smpOffC = intOffC * numCh;

               sF32 readL = readSample2(smpDat, smpDatLRX, smpOffC + 0u);
               sF32 readR = readSample2(smpDat, smpDatLRX, smpOffC + 1u);

               if(br_mask)
               {
                  readL = BitReduce(readL, sample->br_preamp, br_mask);
                  readR = BitReduce(readR, sample->br_preamp, br_mask);
               }

               smpL += readL;
               smpR += readR;
            }

            fetchOffC += cRateVSR;
            if(fetchOffC >= current_sample_len)
               fetchOffC -= current_sample_len;
         }

         if(1u == numCh)
         {
            smpL *= (1.0f / VSR_NUM);
            smpR = smpL; // mono to stereo
         }
         else
         {
            smpL *= (1.0f / VSR_NUM);
            smpR *= (1.0f / VSR_NUM);
         }
#undef VSR_NUM
      }
      else if(STSAMPLE_INTERPOL_VSR64 == sample->interpol_type)
      {
#define VSR_NUM 64
         sF64 fetchOffC = fetchOff;
         const sF64 cRateVSR  = cRate * (1.0f / VSR_NUM);

         smpL = 0.0f;
         smpR = 0.0f;

         for(sUI i = 0u; i < VSR_NUM; i++)
         {
            const sUI intOffC = sUI(fetchOffC) & smpoff_mask;

            if(1u == numCh)
            {
               // Mono
               const sUI smpOffC = intOffC;

               sF32 readL = readSample1(smpDat, smpDatLRX, smpOffC + 0u);

               if(br_mask)
               {
                  readL = BitReduce(readL, sample->br_preamp, br_mask);
               }

               smpL += readL;
            }
            else
            {
               // Stereo
               sUI smpOffC = intOffC * numCh;

               sF32 readL = readSample2(smpDat, smpDatLRX, smpOffC + 0u);
               sF32 readR = readSample2(smpDat, smpDatLRX, smpOffC + 1u);

               if(br_mask)
               {
                  readL = BitReduce(readL, sample->br_preamp, br_mask);
                  readR = BitReduce(readR, sample->br_preamp, br_mask);
               }

               smpL += readL;
               smpR += readR;
            }

            fetchOffC += cRateVSR;
            if(fetchOffC >= current_sample_len)
               fetchOffC -= current_sample_len;
         }

         if(1u == numCh)
         {
            smpL *= (1.0f / VSR_NUM);
            smpR = smpL; // mono to stereo
         }
         else
         {
            smpL *= (1.0f / VSR_NUM);
            smpR *= (1.0f / VSR_NUM);
         }
#undef VSR_NUM
      }
      else if(STSAMPLE_INTERPOL_LANCZOS4 == sample->interpol_type)
      {
#define A 2
#define TSZ LANCZOS_TSZ_A2
#define TBL lanczos_tbl_a2
         if(numCh > 1)
         {
            // Stereo
            // static int xxx = 0;
            sUI xfStep = (TSZ>>1) / A;
            sSI xi = sSI(smpOff) - A*2 + 2;
            sUI xf = sUI( (1.0f - smpFrac) * xfStep );
            smpL = smpR = 0.0f;
            sSI xo = -A;
            for(; xo < A; xo++)
            {
               sF32 inSmpL = readSample2(smpDat, smpDatLRX, xi+0);
               sF32 inSmpR = readSample2(smpDat, smpDatLRX, xi+1);
               sF32 t = TBL[xf];
               // if(0 == (xxx & 4095))
               //    Dyac_host_printf("xxx lanczos4s: inSmp[%d]=(%f; %f), TBL[%u]=%f\n", xi, inSmpL, inSmpR, xf, t);
               smpL += inSmpL * t;
               smpR += inSmpR * t;
               xf += xfStep;
               xi += 2;
            }
            if(br_mask)
            {
               smpL = BitReduce(smpL, sample->br_preamp, br_mask);
               smpR = BitReduce(smpR, sample->br_preamp, br_mask);
            }
            // if(0 == (++xxx & 4095))
            //    Dyac_host_printf("xxx lanczos4s: smpL=%f smpR=%f xfStep=%u xo=%d xi=%d\n", smpL, smpR, xfStep, xo, xi);
         }
         else
         {
            // Mono
            // static int xxx = 0;
            sUI xfStep = (TSZ>>1) / A;
            sSI xi = sSI(smpOff) - A + 1;
            sUI xf = sUI( (1.0f - smpFrac) * xfStep );
            smpL = 0.0f;
            sSI xo = -A;
            for(; xo < A; xo++)
            {
               sF32 inSmp = readSample1(smpDat, smpDatLRX, xi);
               sF32 t = TBL[xf];
               // if(0 == (xxx & 4095))
               //    Dyac_host_printf("xxx lanczos4m: inSmp[%d]=%f, TBL[%u]=%f\n", xi, inSmp, xf, t);
               smpL += inSmp * t;
               xf += xfStep;
               xi++;
            }
            if(br_mask)
            {
               smpL = BitReduce(smpL, sample->br_preamp, br_mask);
            }
            smpR = smpL; // mono to stereo
            // if(0 == (++xxx & 4095))
            //    Dyac_host_printf("xxx lanczos4m: smp=%f xfStep=%u xo=%d xi=%d\n", smpL, xfStep, xo, xi);
         }
#undef TSZ
#undef TBL
#undef A
      }
      else if(STSAMPLE_INTERPOL_LANCZOS8 == sample->interpol_type)
      {
#define A 4
#define TSZ LANCZOS_TSZ_A4
#define TBL lanczos_tbl_a4
         if(numCh > 1)
         {
            // Stereo
            // static int xxx = 0;
            sUI xfStep = (TSZ>>1) / A;
            sSI xi = sSI(smpOff) - A*2 + 2;
            sUI xf = sUI( (1.0f - smpFrac) * xfStep );
            smpL = smpR = 0.0f;
            sSI xo = -A;
            for(; xo < A; xo++)
            {
               sF32 inSmpL = readSample2(smpDat, smpDatLRX, xi+0);
               sF32 inSmpR = readSample2(smpDat, smpDatLRX, xi+1);
               sF32 t = TBL[xf];
               // if(0 == (xxx & 4095))
               //    Dyac_host_printf("xxx lanczos8s: inSmp[%d]=(%f; %f), TBL[%u]=%f\n", xi, inSmpL, inSmpR, xf, t);
               smpL += inSmpL * t;
               smpR += inSmpR * t;
               xf += xfStep;
               xi += 2;
            }
            if(br_mask)
            {
               smpL = BitReduce(smpL, sample->br_preamp, br_mask);
               smpR = BitReduce(smpR, sample->br_preamp, br_mask);
            }
            // if(0 == (++xxx & 4095))
            //    Dyac_host_printf("xxx lanczos8s: smpL=%f smpR=%f xfStep=%u xo=%d xi=%d\n", smpL, smpR, xfStep, xo, xi);
         }
         else
         {
            // Mono
            // static int xxx = 0;
            sUI xfStep = (TSZ>>1) / A;
            sSI xi = sSI(smpOff) - A + 1;
            sUI xf = sUI( (1.0f - smpFrac) * xfStep );
            smpL = 0.0f;
            sSI xo = -A;
            for(; xo < A; xo++)
            {
               sF32 inSmp = readSample1(smpDat, smpDatLRX, xi);
               sF32 t = TBL[xf];
               // if(0 == (xxx & 4095))
               //    Dyac_host_printf("xxx lanczos8m: inSmp[%d]=%f, TBL[%u]=%f\n", xi, inSmp, xf, t);
               smpL += inSmp * t;
               xf += xfStep;
               xi++;
            }
            if(br_mask)
            {
               smpL = BitReduce(smpL, sample->br_preamp, br_mask);
            }
            smpR = smpL; // mono to stereo
            // if(0 == (++xxx & 4095))
            //    Dyac_host_printf("xxx lanczos8m: smp=%f xfStep=%u xo=%d xi=%d\n", smpL, xfStep, xo, xi);
         }
#undef TSZ
#undef TBL
#undef A
      }
      else if(STSAMPLE_INTERPOL_LANCZOS16 == sample->interpol_type)
      {
#define A 8
#define TSZ LANCZOS_TSZ_A8
#define TBL lanczos_tbl_a8
         if(numCh > 1)
         {
            // Stereo
            // static int xxx = 0;
            sUI xfStep = (TSZ>>1) / A;
            sSI xi = sSI(smpOff) - A*2 + 2;
            sUI xf = sUI( (1.0f - smpFrac) * xfStep );
            smpL = smpR = 0.0f;
            sSI xo = -A;
            for(; xo < A; xo++)
            {
               sF32 inSmpL = readSample2(smpDat, smpDatLRX, xi+0);
               sF32 inSmpR = readSample2(smpDat, smpDatLRX, xi+1);
               sF32 t = TBL[xf];
               // if(0 == (xxx & 4095))
               //    Dyac_host_printf("xxx lanczos16s: inSmp[%d]=(%f; %f), TBL[%u]=%f\n", xi, inSmpL, inSmpR, xf, t);
               smpL += inSmpL * t;
               smpR += inSmpR * t;
               xf += xfStep;
               xi += 2;
            }
            if(br_mask)
            {
               smpL = BitReduce(smpL, sample->br_preamp, br_mask);
               smpR = BitReduce(smpR, sample->br_preamp, br_mask);
            }
            // if(0 == (++xxx & 4095))
            //    Dyac_host_printf("xxx lanczos16s: smpL=%f smpR=%f xfStep=%u xo=%d xi=%d\n", smpL, smpR, xfStep, xo, xi);
         }
         else
         {
            // Mono
            // static int xxx = 0;
            sUI xfStep = (TSZ>>1) / A;
            sSI xi = sSI(smpOff) - A + 1;
            sUI xf = sUI( (1.0f - smpFrac) * xfStep );
            smpL = 0.0f;
            sSI xo = -A;
            for(; xo < A; xo++)
            {
               sF32 inSmp = readSample1(smpDat, smpDatLRX, xi);
               sF32 t = TBL[xf];
               // if(0 == (xxx & 4095))
               //    Dyac_host_printf("xxx lanczos16m: inSmp[%d]=%f, TBL[%u]=%f\n", xi, inSmp, xf, t);
               smpL += inSmp * t;
               xf += xfStep;
               xi++;
            }
            if(br_mask)
            {
               smpL = BitReduce(smpL, sample->br_preamp, br_mask);
            }
            smpR = smpL; // mono to stereo
            // if(0 == (++xxx & 4095))
            //    Dyac_host_printf("xxx lanczos16m: smp=%f xfStep=%u xo=%d xi=%d\n", smpL, xfStep, xo, xi);
         }
#undef TSZ
#undef TBL
#undef A
      }
      else if(STSAMPLE_INTERPOL_LANCZOS32 == sample->interpol_type)
      {
#define A 16
#define TSZ LANCZOS_TSZ_A16
#define TBL lanczos_tbl_a16
         if(numCh > 1)
         {
            // Stereo
            // static int xxx = 0;
            sUI xfStep = (TSZ>>1) / A;
            sSI xi = sSI(smpOff) - A*2 + 2;
            sUI xf = sUI( (1.0f - smpFrac) * xfStep );
            smpL = smpR = 0.0f;
            sSI xo = -A;
            for(; xo < A; xo++)
            {
               sF32 inSmpL = readSample2(smpDat, smpDatLRX, xi+0);
               sF32 inSmpR = readSample2(smpDat, smpDatLRX, xi+1);
               sF32 t = TBL[xf];
               // if(0 == (xxx & 4095))
               //    Dyac_host_printf("xxx lanczos32s: inSmp[%d]=(%f; %f), TBL[%u]=%f\n", xi, inSmpL, inSmpR, xf, t);
               smpL += inSmpL * t;
               smpR += inSmpR * t;
               xf += xfStep;
               xi += 2;
            }
            if(br_mask)
            {
               smpL = BitReduce(smpL, sample->br_preamp, br_mask);
               smpR = BitReduce(smpR, sample->br_preamp, br_mask);
            }
            // if(0 == (++xxx & 4095))
            //    Dyac_host_printf("xxx lanczos32s: smpL=%f smpR=%f xfStep=%u xo=%d xi=%d\n", smpL, smpR, xfStep, xo, xi);
         }
         else
         {
            // Mono
            // static int xxx = 0;
            sUI xfStep = (TSZ>>1) / A;
            sSI xi = sSI(smpOff) - A + 1;
            sUI xf = sUI( (1.0f - smpFrac) * xfStep );
            smpL = 0.0f;
            sSI xo = -A;
            for(; xo < A; xo++)
            {
               sF32 inSmp = readSample1(smpDat, smpDatLRX, xi);
               sF32 t = TBL[xf];
               // if(0 == (xxx & 4095))
               //    Dyac_host_printf("xxx lanczos32m: inSmp[%d]=%f, TBL[%u]=%f\n", xi, inSmp, xf, t);
               smpL += inSmp * t;
               xf += xfStep;
               xi++;
            }
            if(br_mask)
            {
               smpL = BitReduce(smpL, sample->br_preamp, br_mask);
            }
            smpR = smpL; // mono to stereo
            // if(0 == (++xxx & 4095))
            //    Dyac_host_printf("xxx lanczos32m: smp=%f xfStep=%u xo=%d xi=%d\n", smpL, xfStep, xo, xi);
         }
#undef TSZ
#undef TBL
#undef A
      }
      else if(STSAMPLE_INTERPOL_LANCZOS64 == sample->interpol_type)
      {
#define A 32
#define TSZ LANCZOS_TSZ_A32
#define TBL lanczos_tbl_a32
         if(numCh > 1)
         {
            // Stereo
            // static int xxx = 0;
            sUI xfStep = (TSZ>>1) / A;
            sSI xi = sSI(smpOff) - A*2 + 2;
            sUI xf = sUI( (1.0f - smpFrac) * xfStep );
            smpL = smpR = 0.0f;
            sSI xo = -A;
            for(; xo < A; xo++)
            {
               sF32 inSmpL = readSample2(smpDat, smpDatLRX, xi+0);
               sF32 inSmpR = readSample2(smpDat, smpDatLRX, xi+1);
               sF32 t = TBL[xf];
               // if(0 == (xxx & 4095))
               //    Dyac_host_printf("xxx lanczos32s: inSmp[%d]=(%f; %f), TBL[%u]=%f\n", xi, inSmpL, inSmpR, xf, t);
               smpL += inSmpL * t;
               smpR += inSmpR * t;
               xf += xfStep;
               xi += 2;
            }
            if(br_mask)
            {
               smpL = BitReduce(smpL, sample->br_preamp, br_mask);
               smpR = BitReduce(smpR, sample->br_preamp, br_mask);
            }
            // if(0 == (++xxx & 4095))
            //    Dyac_host_printf("xxx lanczos32s: smpL=%f smpR=%f xfStep=%u xo=%d xi=%d\n", smpL, smpR, xfStep, xo, xi);
         }
         else
         {
            // Mono
            // static int xxx = 0;
            sUI xfStep = (TSZ>>1) / A;
            sSI xi = sSI(smpOff) - A + 1;
            sUI xf = sUI( (1.0f - smpFrac) * xfStep );
            smpL = 0.0f;
            sSI xo = -A;
            for(; xo < A; xo++)
            {
               sF32 inSmp = readSample1(smpDat, smpDatLRX, xi);
               sF32 t = TBL[xf];
               // if(0 == (xxx & 4095))
               //    Dyac_host_printf("xxx lanczos32m: inSmp[%d]=%f, TBL[%u]=%f\n", xi, inSmp, xf, t);
               smpL += inSmp * t;
               xf += xfStep;
               xi++;
            }
            if(br_mask)
            {
               smpL = BitReduce(smpL, sample->br_preamp, br_mask);
            }
            smpR = smpL; // mono to stereo
            // if(0 == (++xxx & 4095))
            //    Dyac_host_printf("xxx lanczos32m: smp=%f xfStep=%u xo=%d xi=%d\n", smpL, xfStep, xo, xi);
         }
#undef TSZ
#undef TBL
#undef A
      }
      else
      {
         // No interpolation
         smpL = smpDat[smpOff + 0];

         if(br_mask)
         {
            smpL = BitReduce(smpL, sample->br_preamp, br_mask);
         }

         if(numCh > 1)
         {
            smpR = smpDat[smpOff + 1];

            if(br_mask)
            {
               smpR = BitReduce(smpR, sample->br_preamp, br_mask);
            }
         }
         else
         {
            smpR = smpL; // mono to stereo
         }
      }

      if(startEndFadeNumFrames > 0u)
      {
         sSI playOffInt = sSI(fetchOff);
         sSI fadeLen = sSI(startEndFadeNumFrames);
         if(fadeLen > sSI(current_sample_len >> 1))
            fadeLen = sSI(current_sample_len >> 1);
         sF32 amt;
         if(playOffInt < fadeLen)
         {
            // Fade in
            amt = sF32(fetchOff) / fadeLen;
            smpL *= amt;
            smpR *= amt;
         }
         else if(playOffInt > sSI(current_sample_len - fadeLen))
         {
            // Fade out
            amt = sF32(fetchOff - (current_sample_len - fadeLen)) / fadeLen;
            amt = 1.0f - amt;
            smpL *= amt;
            smpR *= amt;
         }
         else
            amt = 1.0f;

         // (note) [Apple clang version 14.0.3 issue] not reading cOff to a dummy field causes audible clicks
         coff_clang_workaround = cOff;

         if(b_queued_delayed_loop)
         {
            if(amt <= (1.0f / 64.0f))
            {
               // yac_host->printf("xxx delay handleEndOfLoop. fade amt=%f cOff=%f fetchOff=%f current_sample_len=%u fdNum=%u\n", amt, cOff, fetchOff, current_sample_len, startEndFadeNumFrames);

               b_queued_delayed_loop = YAC_FALSE;

               // End of loop cycle, start new loop
               if(!handleEndOfLoop(&cOff, &smpDat, &smpDatLRX, YAC_FALSE/*bFromStart*/, YAC_FALSE/*bAllowCOffReset*/))
               {
                  // End of sample
                  return 0;
               }

               fetchCycleOff = (mmdst_cyclelen_sampleoff * current_sample_len);
               if(fetchCycleOff < 0.0)
                  fetchCycleOff = 0.0;
               else if(fetchCycleOff >= current_sample_len)
                  fetchCycleOff = current_sample_len;
               startEndFadeNumFrames = sUI(sample->startend_fade_num_frames * mod_cyclelen * sample_player->mod_cyclelen * mmdst_cyclelen * cur_cyclelen_eol);

               // yac_host->printf("xxx 2 delay handleEndOfLoop. fade amt=%f cOff=%f fetchOff=%f current_sample_len=%u fdNum=%u\n", amt, cOff, fetchOff, current_sample_len, startEndFadeNumFrames);

            }
         }
      }

      // Add input
      if( (NULL != _inputsOrNull) && ((YAC_TRUE == sample->b_sum_input) || ((YAC_MAYBE == sample->b_sum_input) && sample->b_liverec_active)) )
      {
         if((YAC_MAYBE == sample->b_sum_input) && sample->b_liverec_active)
         {
            smpL = 0.0f;
            smpR = 0.0f;
         }

         const sF32 pan = sample->sum_input_xfade;
         const sF32 volSmp = (pan < 0.0f) ? 1.0f : (1.0f - pan);
         const sF32 volInp = (pan > 0.0f) ? 1.0f : (1.0f + pan);

         smpL = (smpL*volSmp) + _inputsOrNull[sample->liverec_input_idx][0] * volInp;

         if(sample->b_liverec_stereo)
         {
            smpR = (smpR*volSmp) + _inputsOrNull[sUI(sample->liverec_input_idx + 1) % STSAMPLE_MAX_INPUTS][0] * volInp;
         }
         else
         {
            smpR = (smpR*volSmp) + _inputsOrNull[sample->liverec_input_idx][0] * volInp;
         }
      }

      // Calc output
      last_sample_l = smpL;
      last_sample_r = smpR;

      sF32 pluginAmp[2] = { 1.0f/*L*/, 1.0f/*R*/ };

      if(sample->b_enable_fx)
      {
         if(0u != sample->plugin_mask_vb)
            processPluginsVB(1u/*numFrames*/);

         if(0u == sample->plugin_alg)
         {
            processPluginsDef(YAC_TRUE/*bPreFilter*/, (1u == numCh)/*bMonoIn*/);
            // Filter
            //  (note) filter panning may produce true stereo
            filter(last_sample);
            processPluginsDef(YAC_FALSE/*bPreFilter*/, YAC_FALSE/*bMonoIn*/);
         }
#ifndef LIBSYNERGY_BUILD
         else
         {
            // (todo) properly handle/update mono hint
            // // sUI algInt = sample->plugin_alg_int + (1u == numCh)/*bMonoIn*/;
            sUI algInt = sample->plugin_alg_int + YAC_FALSE/*bMonoIn*/;
            // // static int xxx = 0;
            // // if(0 == (xxx++ & 1023))
            // //    Dyac_host_printf("xxx algInt=%u\n", algInt);
            if(algInt < (sizeof(process_algint_tbl)/sizeof(stsamplevoice_process_alg_fxn_t)))
               (*this.*process_algint_tbl[algInt])();
            else
               Dyac_host_printf("[!!!] invalid algInt=%u\n", algInt);
         }
#endif // LIBSYNERGY_BUILD

         if(0u != sample->plugin_mask_am)
            processPluginsAM(pluginAmp);
      }

      // Amp output
      last_sample_l = last_sample_l * cVolL * fadeVol * pluginAmp[0];
      last_sample_r = last_sample_r * cVolR * fadeVol * pluginAmp[1];

      if(looprestart_xfade_frames_left > 0u)
      {
         sF32 xfadeAmt = looprestart_xfade_frames_left-- * (1.0f / STSAMPLEVOICE_LOOPRESTART_NUMSTEPS);
         last_sample_l = last_sample_l + (last_sample_looprestart_l - last_sample_l) * xfadeAmt;
         last_sample_r = last_sample_r + (last_sample_looprestart_r - last_sample_r) * xfadeAmt;
      }

#if 0
      // [11Aug2023] removed, causes clicks (instead of fixing them)
      if(sampleshift_eol_xfade_frames_left > 0u)
      {
         // Crossfade from last loop frame to avoid clicks
#ifndef USE_SAMPLESHIFT_EOL_XFADE_BUF
         sampleshift_eol_xfade_frames_left--;
         sF32 xfadeAmt = sampleshift_eol_xfade_frames_left * (1.0f / VOLRAMP_NUMSTEPS);
         last_sample_l = last_sample_l + (last_sample_sampleshift_eol_l - last_sample_l) * xfadeAmt;
         last_sample_r = last_sample_r + (last_sample_sampleshift_eol_r - last_sample_r) * xfadeAmt;
#else
#ifdef USE_SAMPLESHIFT_EOL_XFADE_BUF_THRESHOLD
         sBool bUseXfadeBuf = (sampleshift_eol_xfade_loopstart_frameidx < VOLRAMP_NUMSTEPS);
         if(bUseXfadeBuf)
#endif // USE_SAMPLESHIFT_EOL_XFADE_BUF_THRESHOLD
         {
            sUI xfadeBufIdx = VOLRAMP_NUMSTEPS - sampleshift_eol_xfade_frames_left;
            sF32 xfadeAmt = sampleshift_eol_xfade_frames_left * (1.0f / VOLRAMP_NUMSTEPS);
            last_sample_l = last_sample_l + (sampleshift_eol_xfade_buf_l[xfadeBufIdx] - last_sample_l) * xfadeAmt;
            last_sample_r = last_sample_r + (sampleshift_eol_xfade_buf_r[xfadeBufIdx] - last_sample_r) * xfadeAmt;
            sampleshift_eol_xfade_frames_left--;
         }
#ifdef USE_SAMPLESHIFT_EOL_XFADE_BUF_THRESHOLD
         else
         {
            sampleshift_eol_xfade_frames_left--;
            sF32 xfadeAmt = sampleshift_eol_xfade_frames_left * (1.0f / VOLRAMP_NUMSTEPS);
            last_sample_l = last_sample_l + (last_sample_sampleshift_eol_l - last_sample_l) * xfadeAmt;
            last_sample_r = last_sample_r + (last_sample_sampleshift_eol_r - last_sample_r) * xfadeAmt;
         }
#endif // USE_SAMPLESHIFT_EOL_XFADE_BUF_THRESHOLD
#endif // USE_SAMPLESHIFT_EOL_XFADE_BUF
      }
#endif

      // Store left channel
      buf[0] += last_sample_l;

      // Store right channel
      buf[1] += last_sample_r;

#ifdef USE_SAMPLESHIFT_EOL_XFADE_BUF
      // Store for declick xfade (sampleshift_eol)
      if(sampleshift_eol_xfade_loopstart_frameidx < VOLRAMP_NUMSTEPS)
      {
         sampleshift_eol_xfade_buf_l[sampleshift_eol_xfade_loopstart_frameidx] = last_sample_l;
         sampleshift_eol_xfade_buf_r[sampleshift_eol_xfade_loopstart_frameidx] = last_sample_r;
         sampleshift_eol_xfade_loopstart_frameidx++;
      }
#endif // USE_SAMPLESHIFT_EOL_XFADE_BUF

      // Next sample frame
      buf += 2;
      render_buf_sample_offset += 2u;

      if(NULL != _inputsOrNull)
      {
         for(sUI i = 0u; i < STSAMPLE_MAX_INPUTS; i++)
         {
            _inputsOrNull[i]++;
         }
      }


      cVolL += sVolL;
      cVolR += sVolR;

      if(0u != sample->plugin_mask_sr)
      {
         sF32 fRate = processPluginsSR(cRate);
         play_offset_actual += fRate;
         cOff += fRate;
      }
      else
      {
         play_offset_actual += cRate;
         cOff += cRate;
      }
      cRate += sRate;

      // // cNoteHz += sNoteHz;

      // TODO: properly handle negative sample rate + loops (reverse play)
      //  (note) [27Feb2020] don't. see play_mode.

      if(cOff < 0.0f)
      {
         // reverse play
         cOff += (sF64) current_sample_len;
      }
      else
      {
         if(sample->b_timedloop)
         {
            samples_until_end_of_loopstep--;
            ////printf("xxx cOff=%d csl=%d samples_until_end_of_loopstep=%d\n", (sSI)cOff, current_sample_len, samples_until_end_of_loopstep);

//             {
//                sUI smpUntilZero = ((current_sample_len - ((sUI)cOff)) / cRate);
//                printf("xxx 1 smpUntilZero=%d\n", smpUntilZero);
//             }

            if((samples_until_end_of_loopstep <= 0) || (-1 != mod_jumptoloop))
            {
               // Timeout, start new loop
               // Dyac_host_printf("xxx start new loop\n");
               if(sample->b_jumptoloop_immediately)
               {
                  if(!handleEndOfLoop(&cOff, &smpDat, &smpDatLRX, YAC_FALSE/*bFromStart*/, YAC_TRUE/*bAllowCOffReset*/))
                  {
                     // End of sample
                     return 0;
                  }
               }
               else
               {
                  if( ((sUI)cOff) >= current_sample_len )
                  {
                     // End of loop cycle, start new loop
                     if(!handleEndOfLoop(&cOff, &smpDat, &smpDatLRX, YAC_FALSE/*bFromStart*/, YAC_TRUE/*bAllowCOffReset*/))
                     {
                        // End of sample
                        return 0;
                     }
                  }
               }
               ///printf("xxx untilend=%d\n", samples_until_end_of_loopstep);

               fetchCycleOff = (mmdst_cyclelen_sampleoff * current_sample_len);
               if(fetchCycleOff < 0.0)
                  fetchCycleOff = 0.0;
               else if(fetchCycleOff >= current_sample_len)
                  fetchCycleOff = current_sample_len;
               startEndFadeNumFrames = sUI(sample->startend_fade_num_frames * mod_cyclelen * sample_player->mod_cyclelen * mmdst_cyclelen * cur_cyclelen_eol);
            }
            else if(samples_until_end_of_loopstep == VOLRAMP_NUMSTEPS)
            {
               /////printf("xxx b_timedloop_fade=%d b_fading_out=%d cd=%d\n", sample->b_timedloop_fade, b_fading_out, fadeout_countdown);
               if(sample->b_timedloop_fade)
               {
                  // start fadeout because of destsample boundary
                  if(!b_fading_out)
                  {
                     fadeout_countdown = VOLRAMP_NUMSTEPS;
                     fadein_countdown = -1;
                     b_fading_out = 1;
                  }
               }
            }
            else if(sample->b_oneshot_loopstep)
            {
               if(!b_zero)
               {
                  if(!b_fading_out)
                  {
                     sUI smpUntilZero = (sUI) ((current_sample_len - ((sUI)cOff)) / cRate);
                     //printf("xxx smpUntilZero=%d\n", smpUntilZero);
                     if( smpUntilZero <= VOLRAMP_NUMSTEPS )
                     {
                        // Fade sample out before auto-inserting silence
                        b_fading_out = 1;
                        fadeout_countdown = smpUntilZero;
                        fadein_countdown = -1;
                        //printf("xxx fadeout countdown=%d\n", fadeout_countdown);
                     }
                  }
               }
            }

            // Continue looping the current cycle
            if( ((sUI)cOff) >= current_sample_len )
            {
               if(sample->b_oneshot_loopstep)
               {
                  //printf("xxx start zero fadeout_countdown=%d\n", fadeout_countdown);
                  b_zero = 1;
                  smpDat = zero_samples;
                  ////printf("xxx zero smpDat=%p\n", smpDat);
                  current_sample_len = NUM_ZERO_SAMPLES;
                  cOff = 0.0f;
               }
               else
               {
                  do
                  {
                     cOff -= (sF32) current_sample_len;
                  }
                  while( ((sUI)cOff) >= current_sample_len );
               }
            }

         }
         else
         {
            // Pitch-dependent/cycle-boundary loop mode

            // Fade end of sample to avoid clicks?
            //  only allowed for one-shot samples (workaround for DC offset'd waveforms!)
            if(sample->b_timedloop_fade && !num_sample_loops)
            {
               if(!b_fading_out)
               {
                  if( ((sSI)cOff) >= (((sSI)current_sample_len)-VOLRAMP_NUMSTEPS) )
                  {
                     // start fadeout because of destsample boundary
                     fadeout_countdown = VOLRAMP_NUMSTEPS;
                     fadein_countdown = -1;
                     b_fading_out = 1;
                  }
               }
            }

            if( ((sUI)cOff) >= current_sample_len )
            {
               if( (fetchCycleOff > 0.0) && (startEndFadeNumFrames >= 64) && (current_sample_len >= 64) )
               {
                  // Delay next loop until fade out
                  b_queued_delayed_loop = YAC_TRUE;

                  cOff -= current_sample_len;
               }
               else
               {
                  // End of loop cycle, start new loop
                  if(!handleEndOfLoop(&cOff, &smpDat, &smpDatLRX, YAC_FALSE/*bFromStart*/, YAC_TRUE/*bAllowCOffReset*/))
                  {
                     // End of sample
                     return 0;
                  }

                  fetchCycleOff = (mmdst_cyclelen_sampleoff * current_sample_len);
                  if(fetchCycleOff < 0.0)
                     fetchCycleOff = 0.0;
                  else if(fetchCycleOff >= current_sample_len)
                     fetchCycleOff = current_sample_len;
                  startEndFadeNumFrames = sUI(sample->startend_fade_num_frames * mod_cyclelen * sample_player->mod_cyclelen * mmdst_cyclelen * cur_cyclelen_eol);
               }
            }
         }

      } // else if reverse play

      // Next sample
      fragSize--;
   }

   current_play_offset = cOff;

#ifdef USE_STATS
   stat_opt_generic += _fragSize;
#endif

   // Dyac_host_printf("xxx voice renderFragmentGeneric LEAVE\n");

   return _fragSize;
}

sUI StSampleVoice::renderFragmentOptVoicePluginOsc(sF32 *&    buf,
                                                   const sUI  _fragSize,
                                                   sF32 &     cVolL,
                                                   sF32 &     cVolR,
                                                   const sF32 sVolL,
                                                   const sF32 sVolR
                                                   ) {
   // (note) caller ensures that _fragSize does not exceed MAX_VOICE_BUS_FRAMES
   // (note) unfortunately the CPU savings are rather minimal (compared to renderFragmentGeneric()). interesting.

   // v1: fixed 1*2*3*4 algorithm
   // Dyac_host_printf("xxx renderFragmentOptVoicePluginOsc: fragSize=%u\n", _fragSize);

   // yac_host->printf("xxx VB render_buf_sample_offset=%u numFrames=%u\n", render_buf_sample_offset, _fragSize);
   if(0u != sample->plugin_mask_vb)
      processPluginsVB(_fragSize/*numFrames*/);

   // Process voice plugins
   sUI pluginBit = 1u;
   sF32 pluginBuf[STSAMPLEVOICE_MAX_VOICE_BUS_FRAMES/*128u*/ * 2u];
   ::memset(pluginBuf, 0, (_fragSize << (1+2)));  // clear stereo floats (<<3 = *2*4)

   for(sUI pluginIdx = 0u; pluginIdx < STSAMPLE_NUM_PLUGINS; pluginIdx++)
   {
      if(sample->plugin_mask_def & pluginBit)
      {
         st_plugin_cache_entry_t *pce = plugin_cache_entries[pluginIdx];
         // Dyac_host_printf("xxx processPlugins: pluginIdx=%u pce=%p pce->voice=%p\n", pluginIdx, pce, pce->voice);
         // // if(pce->voice->info->flags & ST_PLUGIN_FLAG_FX)
         {
            // if(sample->plugins[pluginIdx].b_pre_filter == _bPreFilter)
            // {

               // // plugin level controls input gain
               // last_sample[0u] *= plugin_levels[pluginIdx][0];
               // last_sample[1u] *= plugin_levels[pluginIdx][1];

               // Input scaling ?
               const sF32 lvlL = plugin_levels[pluginIdx][0];
               const sF32 lvlR = plugin_levels[pluginIdx][1];
               if(1.0f != lvlL ||
                  1.0f != lvlR
                  )
               {
                  sUI k = 0u;
                  for(sUI i = 0u; i < _fragSize; i++)
                  {
                     pluginBuf[k++] *= lvlL;
                     pluginBuf[k++] *= lvlR;
                  }
               }

               if(0)
               {
                  // debug
                  for(sUI i = 0u; i < _fragSize; i++)
                  {
                     pce->voice->voice_bus_read_offset = render_buf_sample_offset + (i << 1);
                     pce->voice->info->process_replace(pce->voice,
                                                       YAC_FALSE/*bMonoIn*/,
                                                       last_sample,
                                                       last_sample,
                                                       1u/*numFrames*/
                                                       );
                     pluginBuf[(i<<1) + 0u] = last_sample_l;
                     pluginBuf[(i<<1) + 1u] = last_sample_r;
                  }
               }
               else
               {
                  pce->voice->voice_bus_read_offset = render_buf_sample_offset;
                  // yac_host->printf("xxx DEF render_buf_sample_offset=%u numFrames=%u\n", render_buf_sample_offset, _fragSize);
                  pce->voice->info->process_replace(pce->voice,
                                                    YAC_FALSE/*bMonoIn*/,
                                                    pluginBuf,
                                                    pluginBuf,
                                                    _fragSize/*numFrames*/
                                                    );
               }

               // _bMonoIn = _bMonoIn && (0u == (sample->plugins[pluginIdx].shared->info->info->flags & ST_PLUGIN_FLAG_TRUE_STEREO_OUT));

            // }
         }
      }

      // Next plugin
      pluginBit = pluginBit << 1;
   }

   // Apply volume envelope
   if(0u == sample->plugin_mask_am)
   {
      // no amplitude modulation plugins
      const sUI fragSizeSamples = _fragSize << 1;
      for(sUI k = 0u; k < fragSizeSamples; k += 2u)
      {
         // (todo) remove last_sample
         last_sample_l = pluginBuf[k + 0u] * cVolL;
         last_sample_r = pluginBuf[k + 1u] * cVolR;
         buf[k + 0u] += last_sample_l;
         buf[k + 1u] += last_sample_r;

         // Next frame
         cVolL += sVolL;
         cVolR += sVolR;
      }

      render_buf_sample_offset += _fragSize << 1;
   }
   else
   {
      // Process per-sample amplitude modulation plugins (dst:AM)
      // (todo) process block instead of single frames ? (would need to replicate gate signal to all frames)
      sUI fragSizeSamples = _fragSize << 1;
      for(sUI k = 0u; k < fragSizeSamples; k += 2u)
      {
         sF32 pluginAmp[2] = { 1.0f/*L*/, 1.0f/*R*/ };
         processPluginsAM(pluginAmp);

         // (todo) remove last_sample
         last_sample_l = pluginBuf[k + 0u] * cVolL * pluginAmp[0];
         last_sample_r = pluginBuf[k + 1u] * cVolR * pluginAmp[1];
         buf[k + 0u] += last_sample_l;
         buf[k + 1u] += last_sample_r;

         // Next frame
         cVolL += sVolL;
         cVolR += sVolR;
         render_buf_sample_offset += 2u;
      }
   }

   return _fragSize;
}

sUI StSampleVoice::renderBlockNormal(sF32 *     buf,
                                     sUI        blkSz,
                                     sF32 *     smpDat,
                                     sF32 *     smpDatLRX,
                                     const sF32 a,
                                     const sF32 b,
                                     sF32       cVolL,
                                     sF32       cVolR,
                                     const sF32 sVolL,
                                     const sF32 sVolR,
                                     const sF32**_inputsOrNull/*sz=STSAMPLE_MAX_INPUTS*/
                                     )
{
   //
   // No timestretching, return number of samples left (>0 if voice stops, for example)
   //
   ////printf("renderblocknormal\n");
   // Dyac_host_printf("xxx inputsOrNull=%p\n", _inputsOrNull);

   sF32 *bufStart = buf;
   sUI blkSzStart = blkSz;

   // for FX plugins:
   sF32 cNoteHz;
   sF32 nNoteHz;
   // // sF32 sNoteHz = (nNoteHz - cNoteHz) / blkSz;
   cNoteHz = noteToFreq(current_freq + (next_freq - current_freq) * a) * liverec_osc_pitch_factor;
   nNoteHz = noteToFreq(current_freq + (next_freq - current_freq) * b) * liverec_osc_pitch_factor;

   if(mmdst.note_2 > 0.0f)
   {
      sF32 hz2 = noteToFreq(mmdst.note_2*127.0f + mmdst.freq_2) - noteToFreq(5.0f*12.0f/*C-5*/);
      // Dyac_host_printf("xxx mmdst.note_2*127.0f=%f mmdst.freq_2=%f hz2=%f\n", mmdst.note_2*127.0f, mmdst.freq_2, hz2);
      cNoteHz += hz2;
      nNoteHz += hz2;
   }
   else if(mmdst.freq_2 != 0.0f)
   {
      sF32 hz2 = noteToFreq((5.0f*12.0f)/*C-5*/ + mmdst.freq_2) - noteToFreq(5.0f*12.0f/*C-5*/);
      cNoteHz += hz2;
      nNoteHz += hz2;
   }

   // Dyac_host_printf("xxx cFreq=%f cNoteHz=%f nFreq=%f nNoteHz=%f\n", current_freq, cNoteHz, next_freq, nNoteHz);

   if(mmdst_sync_speed > 0.0f)
   {
      sF32 r = 1.0f;
      sF64 f = 1.0;

      if(play_offset_orig_speed > play_offset_actual)
      {
         if(play_offset_actual > 0.0f)
         {
            // Speed up
            r = (sF32) (play_offset_orig_speed / play_offset_actual);
            f = r * (1.0 + mmdst_sync_speed);
            sync_factor_avg = sync_factor_avg + (f - sync_factor_avg) * (r*0.05*mmdst_sync_slew);
            f = sync_factor_avg;
            cNoteHz = sF32(cNoteHz * f);
            nNoteHz = sF32(nNoteHz * f);
         }
      }
      else if(play_offset_actual > play_offset_orig_speed)
      {
         if(play_offset_orig_speed > 0.0f)
         {
            // Slow down
            r = (sF32) (play_offset_actual / play_offset_orig_speed);
            f = r * (1.0 + mmdst_sync_speed);
            if(f > 0.0f)
            {
               f = 1.0f / f;
               sync_factor_avg = sync_factor_avg + (f - sync_factor_avg) * (r*0.05*mmdst_sync_slew);
               cNoteHz = sF32(cNoteHz * sync_factor_avg);
               nNoteHz = sF32(nNoteHz * sync_factor_avg);
            }
         }
      }
#if 0
      static int xxxPlayPos = 0;
      if(0 == (xxxPlayPos++&255))
         Dyac_host_printf("xxx play_offset_orig_speed=%f play_offset_actual=%f ratio=%f f=%f\n", play_offset_orig_speed, play_offset_actual, r, f);
#endif
   }

   // Calc current sample rate
   sF32 cRate = freqToSampleRate(cNoteHz);

   // Calc sample rate delta
   sF32 sRate = (freqToSampleRate(nNoteHz) - cRate) / blkSz;

   // xxx TODO: generic loop optimizations:
   //     - freq/rate interpolation yes/no
   //        - no rate interpol:
   //            - reverse play yes/no
   //            - loops yes/no
   //               - yes: calc #smps until loop end or use blkSz
   //                 (i.e. do not check for loop bounds in innerloop)
   //            - volume/pan interpolation yes/no
   //            - current/next volume is 0, rate => do not write samples but still calc sampleoff/loopidx
   //     - current_sample_len power of two and only default loop (use and to eliminate bounds check)

   sBool bDebug = YAC_FALSE;
#if 0
   {
      static int xxx = 0;
      if((++xxx & 127) == 0)
      {
         bDebug = YAC_TRUE;
      }
   }
#endif

   if(bDebug)
      Dyac_host_printf("xxx cRate=%f sRate=%f\n", cRate, sRate);


   const sF32 *inputsTmp[STSAMPLE_MAX_INPUTS] = {0};
   const sF32 **inputs;
   if(NULL != _inputsOrNull)
   {
      for(sUI i = 0u; i < STSAMPLE_MAX_INPUTS; i++)
      {
         inputsTmp[i] = _inputsOrNull[i];
      }
      inputs = inputsTmp;
   }
   else
   {
      inputs = NULL;
   }

   while(blkSz > 0)
   {
      sUI numDone = 0u; // number of samples handled by fragment renderer

      if(b_voiceplugin_osc_hint)
      {
         numDone = renderFragmentOptVoicePluginOsc(buf, blkSz,
                                                   cVolL, cVolR,
                                                   sVolL, sVolR
                                                   );
      }
      else
      {
         sUI genSizeHint;

         // anti-imaging/aliasing filter cutoff
         if(cRate > 1.0f)
         {
            if(sample->ai_num_poles > 0u)
            {
               // (note) technically, this is _down_ sampling (=> AA filter)
               sF32 invRate = (1.0f / cRate);
               ai_fc = powf(invRate, sample->ai_exp_up);
               sF32 cRateOct = ::logf(cRate) / sF32(sM_LN2);
               sF32 fcLin;
               if(cRateOct > sample->ai_lin_oct)
                  fcLin = sample->ai_lin_max;
               else
                  fcLin = (sample->ai_lin_oct > 0.0f) ? ((cRateOct * sample->ai_lin_max) / sample->ai_lin_oct) : 0.0f;

               ai_fc += fcLin;
               ai_fc *= 0.5f;

               if(ai_fc > 0.5f)
                  ai_fc = 0.5f;
               else if(ai_fc < 0.0001f)
                  ai_fc = 0.0001f;

               if(bDebug)
               {
                  Dyac_host_printf("xxx cFreq=%f lrPitchFac=%f cNoteHz=%f cRate=%f invRate=%f cRateOct=%f ai_fc=%f\n", current_freq, liverec_osc_pitch_factor, cNoteHz, cRate, invRate, cRateOct, ai_fc);
               }
            }
            else
            {
               if(bDebug)
               {
                  Dyac_host_printf("xxx cFreq=%f lrPitchFac=%f cNoteHz=%f cRate=%f\n", current_freq, liverec_osc_pitch_factor, cNoteHz, cRate);
               }
            }

         }
         else if(cRate < 1.0f)
         {
            if(sample->ai_num_poles > 0u)
            {
               // (note) technically, this is _up_ sampling (=> AI filter)
               ai_fc = powf(cRate, sample->ai_exp_down) * 0.5f;
               if(ai_fc > 0.5f)
                  ai_fc = 0.5f;
               else if(ai_fc < 0.0001f)
                  ai_fc = 0.0001f;
            }
         }
         else
            ai_fc = 0.5f;

         // special case 1:
         //  (todo) move most of these checks to reallyStartVoice()
         if(
            (0 == sample->b_timedloop)&&  // total loop time is pitch dependent
            (cRate >= 0.0f)   &&  // forward play
            (sRate == 0.0f)   &&  // no freq/rate interpolation
            (sVolL == 0.0f)   &&  // no vol. interpolation
            (1 == sample->waveform->num_channels) && // mono
            (STSAMPLE_FLT_TYPE_NONE == filter_type) && // no filtering
            (0u == sampleshift_eol_xfade_frames_left) &&  // no sampleshift declick/xfade
            (0u == br_mask) &&
            (sample->interpol_type <= STSAMPLE_INTERPOL_LINEAR) &&
            (0u == looprestart_xfade_frames_left) &&
            !sample->b_liverec &&
            ((STSAMPLE_PLAY_MODE_FORWARD == sample->play_mode) || (STSAMPLE_PLAY_MODE_FWDNOLOOP == sample->play_mode)) &&
            !sample->hasPlugins() &&
            ((0u == sample->interpol_order) || (STSAMPLE_INTERPOL_NONE == sample->interpol_type)) &&
            ((0u == sample->ai_num_poles) || (ai_fc >= 0.4999f && ai_fc < 0.5001f)) &&
            ((0u == sample->startend_fade_num_frames) || b_queued_delayed_loop) &&
            (0.0f == mmdst_cyclelen_sampleoff)
            )
         {
            genSizeHint = 0;
            numDone = renderFragmentOptFwdMonoSimple(buf, blkSz, genSizeHint, smpDat, cVolL, cVolR, cRate, inputs);
            // Dyac_host_printf("xxx opt: numDone=%d genSizeHint=%d blkSize=%d cVol=(%f; %f) b_playing=%d\n", numDone, genSizeHint, blkSz, cVolL, cVolR, b_playing);

            play_offset_actual += cRate * blkSz;
         }
         else
         {
            // force generic loop for entire block
            genSizeHint = blkSz;
         }

         if(!numDone)
         {
            if(0 != genSizeHint)
            {
               numDone = renderFragmentGeneric(buf, genSizeHint, smpDat, smpDatLRX,
                                               cVolL, cVolR, sVolL, sVolR,
                                               cRate, sRate,
                                               inputs
                                               );
               ////printf("xxx gen: numDone=%d genSizeHint=%d blkSize=%d b_playing=%d\n", numDone, genSizeHint, blkSz, b_playing);
            }
            else
            {
               // voice stopped playing in optimized loop
               ////printf("xxx voice stopped in opt. loop\n");
               return blkSz;
            }
         }
      }

      if(!numDone)
      {
         // voice stopped playing in any loop

//          if(buf != bufStart)
//          {
//             last_sample_l = buf[-2];
//             last_sample_r = buf[-1];
//             ////printf("xxx renderBlockNormal: voice stopped, lastSample=(%f; %f)\n", last_sample_l, last_sample_r);
//          }
// //          else
// //          {
// //             last_sample_l = 0.0f;
// //             last_sample_r = 0.0f;
// //          }

         // Return number of unprocessed buffer samples
         return sUI(blkSzStart - ((buf-bufStart)/2));
      }
      else
      {
//          if(buf != bufStart)
//          {
//             last_sample_l = buf[-2];
//             last_sample_r = buf[-1];
//             ////printf("xxx renderBlockNormal: numDone=%u lastSample=(%f; %f)\n", numDone, last_sample_l, last_sample_r);
//          }
//          else
//          {
//             last_sample_l = 0.0f;
//             last_sample_r = 0.0f;
//          }
      }


      blkSz -= numDone;

   } // while blkSize > 0

   return 0;
}


sUI StSampleVoice::renderBlock(sF32 *buf, sUI blkSz, sF32 a, sF32 b, sF32 _volScale, const sF32**_inputsOrNull/*sz=STSAMPLE_MAX_INPUTS*/) {

    // Dyac_host_printf("xxx voice.renderBlock ENTER\n");

   // return number of samples that have NOT been processed (e.g. voice stops)

   sF32 cVol = current_vol * _volScale;
   sF32 nVol = next_vol * _volScale;

   if(sample->b_liverec && (YAC_TRUE == sample->b_liverec_monitor))
      cVol = nVol = 0.0f;

   if(sample->b_freeze_sampleoff_when_quiet && Dfltzero(cVol) && Dfltzero(nVol))
   {
      // early out: speed optimization, do not render samples when vol is 0
      //            the drawback is that the layered samples may be phase-shifted
      return 0;
   }

   ////printf("xxx renderBlock: this=%p blkSz=%d a=%f b=%f cVol=%f nVol=%f\n", this, blkSz, a,b,cVol,nVol);
   ////printf("xxx renderBlock: cVol=%f nVol=%f current_vol=%f next_vol=%f volScale=%f\n", cVol,nVol, current_vol, next_vol, _volScale);

   // Lerp volume
   sF32 aVol = cVol + (nVol - cVol) * a;
   sF32 bVol = cVol + (nVol - cVol) * b;

   // Lerp panning
   sF32 aPan = current_pan + (next_pan - current_pan) * a;
   sF32 bPan = current_pan + (next_pan - current_pan) * b;

   // Dyac_host_printf("xxx aVol=%f aPan=%f bPan=%f\n", aVol, aPan, bPan);

   // Hotfix for clicking samples when pan=1.0
   //  (todo) investigate actual root cause
   if(aPan > 0.99999f)
      aPan = 0.99999f;
   if(bPan > 0.99999f)
      bPan = 0.99999f;

   // Calc current/next absolute volume
#if 0
   // old panning code from 2009 (center pan == half volume)
   sF32 ap = ((1.0f - aPan) * 0.5f);
   sF32 cVolL = aVol * ap;
   sF32 cVolR = aVol * (1.0f - ap);

   sF32 bp = ((1.0f - bPan) * 0.5f);
   sF32 bVolL = bVol * bp;
   sF32 bVolR = bVol * (1.0f - bp);
#else
   // PAN_LINEAR (13Dec2018)
   sF32 cVolL = (aPan < 0.0f) ? 1.0f : (1.0f - aPan);
   sF32 cVolR = (aPan > 0.0f) ? 1.0f : (1.0f + aPan);
   cVolL *= aVol;
   cVolR *= aVol;

   sF32 bVolL = (bPan < 0.0f) ? 1.0f : (1.0f - bPan);
   sF32 bVolR = (bPan > 0.0f) ? 1.0f : (1.0f + bPan);
   bVolL *= bVol;
   bVolR *= bVol;

   // Dyac_host_printf("xxx cVol=(%f; %f) bVol=(%f; %f)\n", cVolL, cVolR, bVolL, bVolR);
#endif

   // Calc volume delta
   sF32 sVolL = (bVolL - cVolL) / blkSz;
   sF32 sVolR = (bVolR - cVolR) / blkSz;

   // Recalc current_play_offset according to mod_sampleoff / sp.mod_sampleoff / mmdst_sampleoff
   calcCurrentSampleLen();
   if(sample->b_timestretch)
   {
      sF32 ts;
      calcCurrentCycleLen(ts); // may be used by calcCurrentOffset
   }
   calcCurrentOffset();

   // Dyac_host_printf("xxx 1 current_sample_offset=%u current_orig_sample_len=%u current_sample_len=%u\n", current_sample_offset, current_orig_sample_len, current_sample_len);

   sF32 *smpDat;
   sF32 *smpDatLRX;  // used for liverec doublebuffer x-fade
   if(b_zero)
   {
      smpDat = zero_samples;
      smpDatLRX = NULL;
   }
   else
   {
      calcSmpDat(smpDat, smpDatLRX, current_sample_offset, current_orig_sample_len, current_sample_len);
   }

   // Dyac_host_printf("xxx 2 current_sample_offset=%u current_sample_len=%u\n", current_sample_offset, current_sample_len);

#if 0
   {
      static int xxx = 0;
      if(0 == (++xxx & 127))
      {
         Dyac_host_printf("xxx renderBlock: current_play_offset=%f\n", current_play_offset);
      }
   }
#endif


   sUI blkLeft;

   //
   // Is timestretching enabled ?
   //

   // Dyac_host_printf("xxx renderBlock: cVolL=%f sVolL=%f\n", cVolL, sVolL);

   if(sample->b_timestretch)
   {
      if(sample->b_timestretch_additive && (NULL != sample->partial_speeds) && (NULL != sample->partial_speeds) )
      {
         renderBlockAdditive(buf, blkSz, smpDat, smpDatLRX, a, b, cVolL, cVolR, sVolL, sVolR, _inputsOrNull);
      }
      else
      {
         // Dyac_host_printf("xxx call renderBlockTimestretch a=%f b=%f cVol=(%f; %f) sVol=(%f; %f)\n", a, b, cVolL, cVolR, sVolL, sVolR);
         renderBlockTimestretch(buf, blkSz, smpDat, smpDatLRX, a, b, cVolL, cVolR, sVolL, sVolR, _inputsOrNull);
      }
      blkLeft = 0;
   }
   else
   {
      // Dyac_host_printf("xxx voice renderBlockNormal 1\n");
      blkLeft = renderBlockNormal(buf, blkSz, smpDat, smpDatLRX, a, b, cVolL, cVolR, sVolL, sVolR, _inputsOrNull);
      // Dyac_host_printf("xxx voice renderBlockNormal 2\n");
   } // else if b_timestrech

   play_offset_orig_speed += (sF64(blkSz) * noteToSampleRate(12*5/*C-5*/ + sample->transpose + ((NULL != sample->parent_samplebank) ? sample->parent_samplebank->transpose : 0.0f)));

   return blkLeft;
}

void StSampleVoice::liveRecCopyToOverride(sF32 *_smpDatBase, sUI _waveOff) {

   sUI numCh = sample->waveform->num_channels;

   if(STSAMPLE_LIVEREC_MODE_CONTINUOUS_RING == sample->liverec_mode)
   {
      // (note) liverec_queued_override_loop_offset/len is updated each time new sample frames have been added to the ring buffer
      if( (b_override_smpdat_copied && (sample->liverec_osc_fade_amt < 0.9999f)) || sample->keepInitialOscCopyOverrideBuffer())
      {
         // Dyac_host_printf("xxx call liveRecFadeRingToOverride\n");
         sample->liveRecFadeRingToOverride(override_smpdat,
                                           _waveOff,
                                           // // liverec_copy_loop_offset,
                                           // // liverec_copy_loop_len,////_curSampleLen
                                           sample->liverec_queued_override_loop_offset,  // src
                                           sample->liverec_queued_override_loop_len,
                                           sample->liverec_osc_fade_amt/*newAmt*/
                                           );
      }
      else
      {
         // Dyac_host_printf("xxx call liveRecCopyRingToOverride\n");
         sample->liveRecCopyRingToOverride(override_smpdat,
                                           _waveOff,
                                           // // liverec_copy_loop_offset,
                                           // // liverec_copy_loop_len////_curSampleLen
                                           sample->liverec_queued_override_loop_offset,  // src
                                           sample->liverec_queued_override_loop_len
                                           );
      }
   }
   else
   {
#if 1
      ::memcpy((void*)override_smpdat,
               _smpDatBase + (liverec_copy_loop_offset + _waveOff) * numCh,
               liverec_copy_loop_len * sizeof(sF32) * numCh
               );
#else
      ::memset((void*)override_smpdat, 0, liverec_copy_loop_len * sizeof(sF32) * numCh);
#endif
   }

   // Dyac_host_printf("xxx liverec copy from off=%u waveOff=%u num=%u #ch=%u\n", liverec_copy_loop_offset, waveOff, liverec_copy_loop_len, numCh);
}

void StSampleVoice::liveRecResampleToOverride(sF32 *_smpDatBase, sUI _waveOff) {

   sUI numCh = sample->waveform->num_channels;

   if(STSAMPLE_LIVEREC_MODE_CONTINUOUS_RING == sample->liverec_mode)
   {
      // (note) liverec_queued_override_loop_offset/len is updated each time new sample frames have been added to the ring buffer
      if(b_override_smpdat_copied || sample->keepInitialOscCopyOverrideBuffer())
      {
         sample->liveRecResampleAndFadeRingToOverride(override_smpdat,
                                                      _waveOff,
                                                      sample->liverec_queued_override_loop_offset,  // src
                                                      sample->liverec_queued_override_loop_len,
                                                      0u, /////liverec_copy_loop_offset,  // dst
                                                      liverec_copy_loop_len,
                                                      sample->liverec_osc_fade_amt/*newAmt*/
                                                      );
      }
      else
      {
         // Copy to implicit dstOffset=0
         // Dyac_host_printf("xxx call liveRecCopyRingToOverride 2\n");
         sample->liveRecCopyRingToOverride(override_smpdat,
                                           _waveOff,
                                           // // liverec_copy_loop_offset,  // src
                                           // // liverec_copy_loop_len////_curSampleLen
                                           sample->liverec_queued_override_loop_offset,  // src
                                           sample->liverec_queued_override_loop_len
                                           );
      }
   }
   else
   {
#if 1
      ::memcpy((void*)override_smpdat,
               _smpDatBase + (liverec_copy_loop_offset + _waveOff) * numCh,
               liverec_copy_loop_len * sizeof(sF32) * numCh
               );
#else
      ::memset((void*)override_smpdat, 0, liverec_copy_loop_len * sizeof(sF32) * numCh);
#endif
   }

   // Dyac_host_printf("xxx liverec copy from off=%u waveOff=%u num=%u #ch=%u\n", liverec_copy_loop_offset, waveOff, liverec_copy_loop_len, numCh);
}

void StSampleVoice::calcSmpDat(sF32*&smpDat,
                               sF32*&smpDatLRX,
                               const sUI _curSampleOffset,
                               const sUI _curOrigSampleLen,
                               const sUI _curSampleLen
                               ) {
   // Calculate current samplepointer, including (modulated) sample offset

   // (TODO) add sample->b_modshiftorig toggle
   sSI smpShift = (sSI) _curSampleOffset/*current_sample_offset*/;
   sUI waveNumFrames = sample->waveform->_getNumFrames();
   sUI waveOff = 0u;
   sSI waveOffNext = -1;
   sF32 *smpDatBase = sample->waveform->sample_data->elements;
   sUI uiLiveRecDblBufOff = 0u;
   current_smpdat_shift = 0;

#if 0
   static int xxx = 0;
   if(0 == (++xxx & 2047))
      Dyac_host_printf("xxx _curSampleOffset=%u _curSampleLen=%u liveRecOff=%d liveRecLen=%d\n", _curSampleOffset, _curSampleLen, sample->liverec_override_loop_offset, sample->liverec_override_loop_len);
#endif

   if(sample->b_liverec)
   {
      if(sample->b_liverec_copy && (STSAMPLE_LIVEREC_OSC_MODE_OFF == sample->liverec_osc_mode))
      {
         // Dyac_host_printf("xxx override_smpdat_sz=%u\n", override_smpdat_sz);
         if((NULL != override_smpdat) && (override_smpdat_sz >= waveNumFrames))
         {
            // Use current sample front buffer
            smpDatBase = override_smpdat;
         }
      }
      else
      {
         if(STSAMPLE_LIVEREC_DBLBUF_MODE_OFF != sample->liverec_doublebuffer_mode)
         {
            sUI waveNumFramesHalf = (waveNumFrames >> 1);

            if( (_curSampleOffset + _curSampleLen) <= waveNumFramesHalf )
            {
               waveNumFrames = waveNumFramesHalf;

               sUI liveRecDblBufIdx = (sample->b_liverec_copy ? liverec_copy_doublebuffer_rec_idx : sample->liverec_doublebuffer_rec_idx);

               if(0u == liveRecDblBufIdx)
               {
                  // Currently recording to first half => Play second half
                  waveOff = waveNumFrames;

                  uiLiveRecDblBufOff = waveOff;

                  if(sample->b_liverec_doublebuffer_xfade)
                     waveOffNext = sSI(0);
               }
               else if(sample->b_liverec_doublebuffer_xfade)
                  waveOffNext = sSI(waveNumFrames);
               // Dyac_host_printf("xxx calcSmpDat: waveOff=%u\n", waveOff);
            }
            else
            {
               // loop refers to 2nd waveform half and double-buffered liverec is enabled
               //  => play as if live recording is not enabled
            }
         }

         if(sample->b_liverec_copy && (liverec_copy_loop_offset >= 0))
         {
            if(NULL == override_smpdat)  // should not be NULL at this point (but can be when voice is edited after it was started)
            {
               lazyAllocOverrideSmpDat(STSAMPLEVOICE_OVERRIDE_SMPDAT_SZ * 2/*stereo*/);
            }
            if(NULL != override_smpdat)
            {
               // Dyac_host_printf("xxx b_override_smpdat_copied=%d\n", b_override_smpdat_copied);
               if(!b_override_smpdat_copied)
               {
                  if(_curSampleLen > STSAMPLEVOICE_OVERRIDE_SMPDAT_SZ)
                  {
                     // _curSampleLen = STSAMPLEVOICE_OVERRIDE_SMPDAT_SZ; // should NOT be reachable
                     Dyac_host_printf("[~~~] liverec copy: (curSampleLen(%u)>SMPDAT_SZ(%u)", _curSampleLen, STSAMPLEVOICE_OVERRIDE_SMPDAT_SZ);
                  }

                  if(sample->keepInitialOscCopyOverrideBuffer())
                  {
                     // do nothing (keep override buffer from previous note on to smoothen attack)
                  }
                  else
                  {
                     if(!sample->b_liverec_pause)
                        liveRecCopyToOverride(smpDatBase, waveOff);
                  }

                  b_override_smpdat_copied = YAC_TRUE;
               }

               smpDatBase = override_smpdat;
               // Dyac_host_printf("xxx override smpDatBase to %p\n", override_smpdat);

               waveNumFrames = STSAMPLEVOICE_OVERRIDE_SMPDAT_SZ;
               waveOff = 0u;
               waveOffNext = 0u;
            }
         }
      }
   } // if sample->b_liverec

   sSI smpLeft;
   if(sample->b_modshiftorig)
   {
      // modShift constant sweep: 0..(len-origsmplen)
      smpLeft = waveNumFrames - (smpShift + _curOrigSampleLen/*current_orig_sample_len*/);
   }
   else
   {
      // modShift dynamic sweep: 0..(len-modsmplen)
      smpLeft = waveNumFrames - (smpShift + _curSampleLen/*current_sample_len*/);
   }
   if(smpLeft > 1)
   {
      sF32 modSh = mod_sampleshift + sample_player->mod_sampleshift + mmdst_sampleshift;
      if(modSh > 0.99f)
      {
         modSh = 0.99f;
      }
      current_smpdat_shift = (sSI) (smpLeft * modSh);
      smpShift += current_smpdat_shift;
      ////printf("xxx calcSmpDat: modSh=%f\n", modSh);
   }
   if(smpShift < 0)
   {
      current_smpdat_shift -= smpShift;
      smpShift = 0;
   }
   // // // else if(smpShift >= (current_sample_len - current_sample_offset))
   // // //    smpShift = (current_sample_len - current_sample_offset);
   waveOff += smpShift;


   smpDat = smpDatBase + ( waveOff * sample->waveform->num_channels );

   if(-1 == waveOffNext)
   {
      smpDatLRX = NULL;
   }
   else
   {
      waveOffNext += smpShift;
      smpDatLRX = smpDatBase + ( waveOffNext * sample->waveform->num_channels );
   }

   if((void*)this == sample->waveform->ui_last_started_voice)
   {
      if(b_used && (_curOrigSampleLen == current_orig_sample_len)) // false after softStopVoice() (volrampdown)
      {
         if(sample->b_liverec && sample->b_liverec_copy && (liverec_copy_loop_offset >= 0) && (STSAMPLE_LIVEREC_OSC_MODE_OFF != sample->liverec_osc_mode))
         {
            // Show copied area
            if(sample->b_liverec_osc_sustain && (STSAMPLE_LIVEREC_OSC_MODE_OFF != sample->liverec_osc_mode))
            {
               if(b_fwd || (STSAMPLE_PLAY_MODE_PINGPONG != sample->play_mode))
                  sample->waveform->ui_last_played_offset = sF32(uiLiveRecDblBufOff + smpShift + current_play_offset + liverec_copy_loop_offset);
               else
                  sample->waveform->ui_last_played_offset = uiLiveRecDblBufOff + smpShift + sF32(current_sample_len - 1 - current_play_offset) + liverec_copy_loop_offset;
            }
            else
            {
               if(b_fwd || (STSAMPLE_PLAY_MODE_PINGPONG != sample->play_mode))
                  sample->waveform->ui_last_played_offset = sF32(uiLiveRecDblBufOff + (smpShift-_curSampleOffset+liverec_copy_loop_offset) + current_play_offset);
               else
                  sample->waveform->ui_last_played_offset = sF32(uiLiveRecDblBufOff + (smpShift-_curSampleOffset+liverec_copy_loop_offset) + current_sample_len - 1 - current_play_offset);

               if(STSAMPLE_LIVEREC_OSC_MODE_OFF == sample->liverec_osc_mode)
                  sample->waveform->ui_last_played_offset += current_sample_offset;
            }
         }
         else
         {
            // Show live area
            if(sample->b_timestretch && sample->b_timestretch_additive)
            {
               sF64 cOff = current_play_offset + current_ts_offset;
               if(cOff > _curSampleLen)
                  cOff -= _curSampleLen;
               sample->waveform->ui_last_played_offset = sF32(waveOff + cOff);
            }
            else if(b_fwd || (STSAMPLE_PLAY_MODE_PINGPONG != sample->play_mode))
               sample->waveform->ui_last_played_offset = sF32(waveOff + current_play_offset);
            else
               sample->waveform->ui_last_played_offset = waveOff + sF32(current_sample_len - 1 - current_play_offset);
         }
      }
   }

#if 0
   {
      static int xxx = 0;
      if(0 == (++xxx & 127))
      {
         Dyac_host_printf("xxx calcSmpDat: b_used=%d smpOff=%u smpShift=%u ui_last_played_offset=%f\n", b_used, current_sample_offset, smpShift, sample->waveform->ui_last_played_offset);

         Dyac_host_printf("xxx set smpDat=%p smpShift=%d csoff=%d shift=%f spshift=%f\n", smpDat, smpShift, current_sample_offset, mod_sampleshift, sample_player->mod_sampleshift);
      }
   }
#endif
}

sUI StSampleVoice::handleNextBlock(void) {
   sUI blkSz = (current_block_size - current_block_offset);
   if(0u == blkSz)
   {
      // Next block
      float_block_off      = (float_block_off + float_block_size) - (sF32)current_block_size;
      current_block_size   = (sUI) (float_block_off + float_block_size);
      current_block_offset = 0u;
      blkSz = current_block_size;

      ////Dyac_host_printf("xxx StSampleVoice::render: next block size=%u\n", blkSz);

      // Determine new "next_" values (envelopes, modulation, zones)
      calcNextBlockState(YAC_TRUE/*bNext*/);

      tickGlide();

      // replay_ticks inc moved to calcNextBlockState() to fix register init
      // // replay_ticks++;
   }
   return blkSz;
}

void StSampleVoice::renderInt(YAC_Object *_buf, sF32 _volScale, const sF32**_inputsOrNull/*sz=STSAMPLE_MAX_INPUTS*/) {

   // Dyac_host_printf("xxx StSampleVoice::_render: b_playing=%i anticlick_fadeout_countdown=%u volScale=%f\n", b_playing, anticlick_fadeout_countdown,_volScale);

   // "add" to stereo buffer
   if(YAC_BCHK(_buf, YAC_CLID_FLOATARRAY))
   {
      YAC_CAST_ARG(YAC_FloatArray, fa, _buf);
      sUI numFrames = fa->yacArrayGetNumElements() / 2u; // div by 2 because of stereo
      sF32 *buf = (sF32*) fa->yacArrayGetPointer();
      sF32 *dbgBufStart = buf;
      sUI dbgNumFrames  = numFrames;
      // Dyac_host_printf("xxx StSampleVoice::_render: numFrames=%u\n", numFrames);

      if(b_playing)
      {
         // Dyac_host_printf("xxx render() debug 1\n");

         render_buf_sample_offset = 0u;

         if(voice_bus_first_voice == this)
         {
            // first voice in a group of layered voices => clear voice bus buffers
            //  (note) all voices in a layer group are in sync (in terms of block offsets)
            for(sUI i = 0u; i < num_layers; i++)
            {
               ::memset((void*)voice_bus_buffers[i], 0, sizeof(sF32) * STSAMPLEVOICE_MAX_VOICE_BUS_FRAMES * 2u);
               // Dyac_host_printf("xxx init voice_bus_buffers_offset[%u]=%p\n", i, voice_bus_buffers_offset[i]);
            }
         }

         if(NULL != sample)
         {
            if(-1 != sample->voice_bus)
            {
               // Redirect output to voice bus
               if(0 == sample->voice_bus)
               {
                  // <c> / current voice buffer
                  buf = voice_bus_buffer_self;
                  dbgBufStart = buf;
               }
               else
               {
                  sUI busBufferIdx = sUI(sample->voice_bus - 1);
                  if(busBufferIdx < voice_bus_first_voice->num_layers)///STSAMPLEVOICE_MAX_LAYERS)
                  {
                     buf = voice_bus_buffers[busBufferIdx];
                     dbgBufStart = buf;
                  }
               }
            }
         }

         while(b_playing && (numFrames > 0))
         {
            sUI blkSz;

            ////Dyac_host_printf("xxx samplevoice: numFrames=%u float_block_size=%f\n", numFrames, float_block_size);

            if(current_delay_countdown > 0)
            {
               ////printf("xxx StSampleVoice::render: current_delay_countdown=%d\n", current_delay_countdown);

               blkSz = current_delay_countdown;
               if(blkSz > numFrames)
               {
                  blkSz = numFrames;
               }
               current_delay_countdown -= blkSz;

               if(current_delay_countdown <= 0)
               {
                  // // b_playing = 0;
                  // // reallyStartVoice(&queued_noteon, 0);

                  current_delay_countdown = 0;

                  queued_noteon.b_valid = 0;

                  // Calc initial block size
                  float_block_size     = STSAMPLEVOICE_BLOCK_LEN * queued_noteon._mixRate;
                  float_block_off      = 0.0f;
                  current_block_size   = (sUI) float_block_size;
                  current_block_offset = 0;

                  numFrames                -= blkSz;
                  buf                      += (blkSz << 1); // *2 for stereo
                  render_buf_sample_offset += (blkSz << 1);

                  // Advance inputs
                  if(NULL != _inputsOrNull)
                  {
                     for(sUI i = 0u; i < STSAMPLE_MAX_INPUTS; i++)
                     {
                        if(NULL != _inputsOrNull[i])
                           _inputsOrNull[i] += blkSz;
                     }
                  }

                  blkSz = handleNextBlock();
                  continue;
               }

               // "add" silence
            }
            else
            {
               blkSz = handleNextBlock();

               // Clip to outbuf size
               if(blkSz > numFrames)
               {
                  blkSz = numFrames;
               }

               // Calc current/next interpolation factors and render sample block
               sF32 a = (current_block_offset / (sF32)current_block_size);
               sF32 b = (a + (blkSz/(sF32)current_block_size));

               if(b_playing)
               {
                  // Dyac_host_printf("xxx voice renderBlock 1\n");
                  blkSz = blkSz - renderBlock(buf, blkSz, a, b, _volScale, _inputsOrNull);
                  // Dyac_host_printf("xxx voice renderBlock 2\n");
               }
               else
               {
                  blkSz = 0;
               }
            }

            // Dyac_host_printf("xxx render() debug 2\n");

            current_block_offset     += blkSz;
            numFrames                -= blkSz;
            buf                      += (blkSz << 1); // *2 for stereo
            // (note) render_buf_sample_offset is inc'd in renderBlock()

            // Advance inputs
            if(NULL != _inputsOrNull)
            {
               for(sUI i = 0u; i < STSAMPLE_MAX_INPUTS; i++)
               {
                  if(NULL != _inputsOrNull[i])
                     _inputsOrNull[i] += blkSz;
               }
            }
         }

      } // if b_playing

      if(0 && (!b_playing && (anticlick_fadeout_countdown > 0)))
      {
         // Handle anticlick fadeout but also sample retrig (in handleNextBlock() -> calcNextBlockState())
         while(numFrames > 0)
         {
            sUI blkSz = handleNextBlock(); // calls calcNextBlockState() if block boundary is hit

            // Clip to outbuf size
            if(blkSz > numFrames)
            {
               blkSz = numFrames;
            }

            sUI i;
            sUI k = 0;
            sUI l = (blkSz < anticlick_fadeout_countdown) ? blkSz : anticlick_fadeout_countdown;

            //Dyac_host_printf("xxx anticlick_fadeout_countdown=%u, numFrames=%u l=%u\n", anticlick_fadeout_countdown, numFrames, l);

            ////Dyac_host_printf("xxx anticlick_fadeout_countdown=%u, bufOff=%u numFrames=%u l=%u\n", anticlick_fadeout_countdown, (sUI)(buf-dbgBufStart), numFrames, l);
//          for(i=0; i<dbgNumSamples; i++)
//          {
//             printf("%f, ", dbgBufStart[i*2+0]);
//          }
//          printf("\n");

            for(i=0; i<l; i++)
            {
               sF32 f = anticlick_fadeout_countdown * (1.0f / ((sF32)ANTICLICK_FADEOUT_TIME));
               ////printf("xxx buf[k+0]=%f\n", buf[k+0]);
               buf[k + 0] += last_sample_l * f;
               buf[k + 1] += last_sample_r * f;
               ////Dyac_host_printf("xxx StSampleVoice: anticlick fadeout l=%f r=%f lastL=%f lastR=%f\n", buf[k+0], buf[k+1], last_sample_l, last_sample_r);
               anticlick_fadeout_countdown--;
               k += 2;
            }

            if(0 == anticlick_fadeout_countdown)
            {
               Dyac_host_printf("xxx StSampleVoice: replay finally stopped after fadeout last_sample_l=%f last_sample_r=%f queued_noteon.b_valid=%d\n", last_sample_l, last_sample_r, queued_noteon.b_valid);

               voice_key = 0; // setting this to 0 at this point will cause problems with noteretrig if sample has finished playing (i.e. must loop)
               b_used = 0;
            }

            current_block_offset     += blkSz;
            numFrames                -= blkSz;
            buf                      += (blkSz << 1); // *2 for stereo
            render_buf_sample_offset += (blkSz << 1);
         }

      }

      if(0)
      {
         sUI i;
         for(i=0; i<dbgNumFrames; i++)
         {
            sF32 l,r;
            ////printf("%f, ", dbgBufStart[i*2+0]);
             l = dbgBufStart[i*2+0];
             r = dbgBufStart[i*2+1];
             if(!b_playing)
             {
                dbgBufStart[i*2+0] = 0.0f;
                dbgBufStart[i*2+1] = 0.0f;
             }
             printf("xxx [%d] l=%f r=%f\n", i, l, r);
            // Dyac_dbgflt_32(l);
            // Dyac_dbgflt_32(r);
         }
         ////printf("\n---\n");
      }

   }
   else
   {
      Dyac_throw_def(InvalidPointer, "_buf is not a valid FloatArray");
   }
}

void StSampleVoice::_render(YAC_Object *_buf, sF32 _volScale) {
   renderInt(_buf, _volScale, NULL/*inputsOrNull*/);
}

void StSampleVoice::_PrintDebugStats(void) {
#ifdef USE_STATS
   sF32 total = (sF32) (stat_opt_innermix + stat_opt_lastmix + stat_opt_generic);
   Dyac_host_printf("[dbg] StSampleVoice::drawDebugStats: inner:%u(%f %%) last:%u (%f %%) generic:%u (%f %%)\n",
                    stat_opt_innermix, 100*(stat_opt_innermix/total),
                    stat_opt_lastmix, 100*(stat_opt_lastmix/total),
                    stat_opt_generic, 100*(stat_opt_generic/total)
                    );
#endif // USE_STATS
}

void StSampleVoice::_setTimedLoopBase(sF32 _base) {
   sF32 oldBase = timedloop_base;

   timedloop_base = (_base > 0.0f) ? _base : sample->timedloop_base;

   if(b_timedloop)
   {

      // // if(samples_until_end_of_loopstep)
      {
         // // samples_until_end_of_loopstep = (sSI) (current_loop_countdown * mix_rate / timedloop_base);
         sUI currentNum = (sUI) (samples_until_end_of_loopstep * (oldBase / mix_rate));
         samples_until_end_of_loopstep = (sSI) (currentNum * mix_rate / timedloop_base);
         // // printf("xxx new samples_until_end_of_loopstep=%d\n", samples_until_end_of_loopstep);

         if(fadeout_countdown > 0)
         {
            if(fadeout_countdown > samples_until_end_of_loopstep)
               fadeout_countdown = samples_until_end_of_loopstep;
            fadein_countdown = -1;
         }

         if(fadein_countdown > 0)
         {
            if(samples_until_end_of_loopstep < fadein_countdown)
            {
               fadeout_countdown = samples_until_end_of_loopstep;
               fadein_countdown = -1;
               b_fading_out = YAC_TRUE;
            }
         }
      }
   }
}

sF32 StSampleVoice::_getTimedLoopBase(void) {
   return timedloop_base;
}

YAC_Object *StSampleVoice::_getOverrideSampleLoops(void) {
   return override_sample_loops;
}

YAC_Object *StSampleVoice::_getOrCreateOverrideSampleLoops(void) {
   if(NULL == override_sample_loops)
   {
      override_sample_loops = YAC_New_IntArray();
   }
   return override_sample_loops;
}

void StSampleVoice::_allocOverrideSampleLoops(void) {
   _freeOverrideSampleLoops();
   if(NULL == override_sample_loops)
   {
      override_sample_loops = YAC_New_IntArray();
   }
}

void StSampleVoice::_freeOverrideSampleLoops(void) {
   YAC_DELETE_SAFE(override_sample_loops);
}

YAC_IntArray *StSampleVoice::getCurrentSampleLoops(void) const {
   YAC_IntArray *ret = NULL;

   if(NULL != override_sample_loops)
   {
      ret = override_sample_loops;
   }

   if(NULL == ret)
   {
      if(NULL != sample)
      {
         if(NULL != sample->sample_loops)
         {
            ret = sample->sample_loops;
         }
      }
   }

   return ret;
}

sBool StSampleVoice::_verifyOverrideSampleAreas(void) {
   //
   // - return true if one or more sample areas had to be clipped
   // - return false if sample areas are within waveform boundaries
   // - clipping strategy: first reduce len, then offset

   if(NULL != sample)
   {
      if(NULL != sample->waveform)
      {
         sUI range = (sUI) sample->waveform->_getNumFrames();

         sBool r = YAC_FALSE;

         // Verify loops
         if(NULL != override_sample_loops)
         {
            sUI numLoops = override_sample_loops->num_elements / 3;
            sUI *loopDat = (sUI*) override_sample_loops->elements;
            sUI i;
            sUI k = 0;
            for(i=0; i<numLoops; i++)
            {
               sUI times = loopDat[k + 2];
               if(!(times & StSample::LOOP_OP_MASK)) // skip loop special instructions
               {
                  r |= StSample::verify_sample_area(loopDat[k + 0], loopDat[k + 1], range); // Verify loop
               }
               k += 3;
            }
         }

         return r;
      }
      else
      {
         // No waveform selected, nothing to clip
         return 0;
      }
   }
   else
   {
      // No sample selected, nothing to clip
      return 0;
   }
}

sSI StSampleVoice::_liveRecGetOverrideLoopOffset(void) {
   return liverec_copy_loop_offset;
}

sSI StSampleVoice::_liveRecGetOverrideLoopLen(void) {
   return liverec_copy_loop_len;
}

sSI StSampleVoice::_liveRecGetOverridePlayBufIdx(void) {
   return sSI(liverec_copy_doublebuffer_rec_idx ^ 1u);
}

sUI StSampleVoice::_liveRecGetOverrideSmpDatSz(void) {
   return override_smpdat_sz;
}

sBool StSampleVoice::copyOverrideBufferToWaveform(void) {
   if((NULL != sample) && (NULL != sample->waveform) && (NULL != sample->waveform->sample_data))
   {
      if(sample->b_liverec && sample->b_liverec_copy)
      {
         sUI numCh = sample->waveform->num_channels;
         sUI maxFrames = sample->waveform->_getNumFrames();

         if(STSAMPLE_LIVEREC_OSC_MODE_OFF != sample->liverec_osc_mode)
         {
            if((liverec_copy_loop_offset >= 0) && (NULL != override_smpdat))
            {
               if(sUI(liverec_copy_loop_offset + liverec_copy_loop_len) <= maxFrames)
               {
                  ::memcpy((void*)(sample->waveform->sample_data->elements + (sUI(liverec_copy_loop_offset) * numCh)),
                           (void*)override_smpdat,
                           sizeof(sF32) * numCh * sUI(liverec_copy_loop_len)
                           );
                  return YAC_TRUE;
               }
               else
               {
                  Dyac_host_printf("[---] StSampleVoice::copyOverrideBufferToWaveform: copy_loop_offset=%d copy_loop_len=%d > maxFrames=%u\n", liverec_copy_loop_offset, liverec_copy_loop_len, maxFrames);
               }
            }
            else
            {
               Dyac_host_printf("[---] StSampleVoice::copyOverrideBufferToWaveform: copy_loop_offset=%d override_smpdat=%p\n", liverec_copy_loop_offset, override_smpdat);
            }
         }
         else
         {
            // Oneshot mode
            sUI smpDatSz = override_smpdat_sz / numCh;
            sUI cpSz = sMIN(smpDatSz, maxFrames);
            if((NULL != override_smpdat) && (cpSz > 0u))
            // // if((NULL != sample->liverec_front_buffer) && (sample->liverec_front_buffer_sz >= maxFrames))
            {
               ::memcpy((void*)(sample->waveform->sample_data->elements),
                        (void*)override_smpdat,
                        sizeof(sF32)*cpSz*numCh
                        );
               return YAC_TRUE;
            }
            else
            {
               Dyac_host_printf("[---] StSampleVoice::copyOverrideBufferToWaveform: override_smpdat=%p smpDatSz=%u maxFrames=%u cpSz=%u\n", override_smpdat, override_smpdat_sz, maxFrames, cpSz);
            }
         }
      }
      else
      {
         Dyac_host_printf("[---] StSampleVoice::copyOverrideBufferToWaveform: b_liverec=%d b_liverec_copy=%d copy_loop_offset=%d override_smpdat=%p\n", sample->b_liverec, sample->b_liverec_copy, liverec_copy_loop_offset, override_smpdat);
      }
   }
   return YAC_FALSE;
}

sBool StSampleVoice::copyOverrideBufferToFloatArray(YAC_Object *_d) {
   if((NULL != sample) && YAC_Is_FloatArray(_d))
   {
      YAC_CAST_ARG(YAC_FloatArray, d, _d);
      sUI numCh = sample->waveform->num_channels;
      sUI maxFrames = d->num_elements / numCh;
      if(sample->b_liverec && sample->b_liverec_copy)
      {
         if(STSAMPLE_LIVEREC_OSC_MODE_OFF != sample->liverec_osc_mode)
         {
            if((liverec_copy_loop_offset >= 0) && (NULL != override_smpdat))
            {
               if(sUI(liverec_copy_loop_len) <= maxFrames)
               {
                  ::memcpy((void*)d->elements,
                           (void*)override_smpdat,
                           sizeof(sF32) * sUI(liverec_copy_loop_len)
                           );
                  return YAC_TRUE;
               }
               else
               {
                  Dyac_host_printf("[---] StSampleVoice::copyOverrideBufferToFloatArray: copy_loop_offset=%d copy_loop_len=%d > maxFrames=%u\n", liverec_copy_loop_offset, liverec_copy_loop_len, maxFrames);
               }
            }
            else
            {
               Dyac_host_printf("[---] StSampleVoice::copyOverrideBufferToFloatArray: b_liverec=%d b_liverec_copy=%d copy_loop_offset=%d override_smpdat=%p\n", sample->b_liverec, sample->b_liverec_copy, liverec_copy_loop_offset, override_smpdat);
            }
         }
         else
         {
            // Oneshot mode
            sUI smpDatSz = override_smpdat_sz / numCh;
            sUI cpSz = sMIN(smpDatSz, maxFrames);
            if((NULL != override_smpdat) && (cpSz > 0u))
               // // if((NULL != sample->liverec_front_buffer) && (sample->liverec_front_buffer_sz >= maxFrames))
            {
               ::memcpy((void*)d->elements,
                        (void*)override_smpdat,
                        sizeof(sF32)*cpSz*numCh
                        );
               return YAC_TRUE;
            }
            else
            {
               Dyac_host_printf("[---] StSampleVoice::copyOverrideBufferToFloatArray: override_smpdat=%p smpDatSz=%u maxFrames=%u cpSz=%u\n", override_smpdat, override_smpdat_sz, maxFrames, cpSz);
            }
         }
      }
   }
   return YAC_FALSE;
}

void StSampleVoice::_setFilterCutOff(sF32 _f) {
   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._filter_cutoff = _f;
   }
   else
   {
      // Modify voice
      mod_filter_cutoff = _f;
   }
}

void StSampleVoice::_setFilterPan(sF32 _f) {
   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._filter_pan = _f;
   }
   else
   {
      // Modify voice
      mod_filter_pan = _f;
   }
}

void StSampleVoice::_setFilterOffset(sF32 _f) {
   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._filter_offset = _f;
   }
   else
   {
      // Modify voice
      mod_filter_offset = _f;
   }
}

void StSampleVoice::_setFilterResonance(sF32 _f) {
   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._filter_resonance = _f;
   }
   else
   {
      // Modify voice
      mod_filter_resonance = _f;
   }
}

void StSampleVoice::_setGlideSwitch(sBool _bEnable) {
   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._glide_switch = _bEnable;
   }
   else
   {
      // Modify voice
      mod_glide_switch = _bEnable;
   }
}

void StSampleVoice::_setGlideSpeed(sF32 _speed) {
   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._glide_speed = _speed;
   }
   else
   {
      // Modify voice
      mod_glide_speed = _speed;
   }
}

void StSampleVoice::_resetLFOFreqPhase(void) {
   if(NULL != sample)
   {
      sample->lfo_freq_global.resetPhase();
   }
   lfo_freq.resetPhase();
}

void StSampleVoice::_resetLFOVolPhase(void) {
   if(NULL != sample)
   {
      sample->lfo_vol_global.resetPhase();
   }
   lfo_vol.resetPhase();
}

void StSampleVoice::_resetLFOPanPhase(void) {
   if(NULL != sample)
   {
      sample->lfo_pan_global.resetPhase();
   }
   lfo_pan.resetPhase();
}

void StSampleVoice::_resetLFOAuxPhase(void) {
   if(NULL != sample)
   {
      sample->lfo_aux_global.resetPhase();
   }
   lfo_aux.resetPhase();
}

void StSampleVoice::_updatePerfCtlPolyPressure(sF32 _value) {
   if(queued_noteon.b_valid)
   {
      // Voice has not been started, yet
      queued_noteon._perfctl_poly_pressure = _value;
   }
   else
   {
      // Modify voice
      perfctl_poly_pressure = _value;
   }
}

StSampleVoiceBiquad::StSampleVoiceBiquad(void) {
   lfsr.init((void*)this);
}

StSampleVoiceBiquad::~StSampleVoiceBiquad() {
}

void StSampleVoiceBiquad::calcQ(sF32 &freq, sF32 &bandwidth, sF32 &qScale, sF32 q) {
   bandwidth = q;

   sF32 ra = ((lfsr.rand() & 65535u)*(1.0f/32768.0f)) - 1.0f; // xxx analogue feel
   bandwidth = bandwidth + (ra*0.05f);

   if(bandwidth < 0)
      bandwidth = 0;
   else if(bandwidth > 1)
      bandwidth = 1;

   qScale = 1.0f;

// // #define Q_THRESHOLD 0.95f
// // #define Q_FREQ_THRESHOLD 0.05f
// //    if( (bandwidth >= Q_THRESHOLD) && (freq <= Q_FREQ_THRESHOLD) )
// //    {
// //       sF32 t = (bandwidth - Q_THRESHOLD) * (1.0f/(1.0f - Q_THRESHOLD));
// //       t *= (Q_FREQ_THRESHOLD - freq) * (1.0f/Q_FREQ_THRESHOLD);
// //       t = (t*t * (3.0f - 2.0f*t));
// //       qScale *= 1.0f - t;
// //       qScale *= qScale;
// //       qScale *= qScale;
// //       //printf("qScale=%f\n");
// //    }
// // //    else
// // #define Q_FREQ_THRESHOLD2 0.005f
// //    if(freq <= Q_FREQ_THRESHOLD2)
// //    {
// //       sF32 t = (Q_FREQ_THRESHOLD2 - freq) * (1.0f/Q_FREQ_THRESHOLD2);
// //       t = (t*t * (3.0f - 2.0f*t));
// //       qScale = 1.0f - t;
// //       qScale *= qScale;
// //       qScale *= qScale;
// //    }

// // #define Q_THRESHOLD2 0.8f
// //    if(bandwidth > Q_THRESHOLD2)
// //    {
// //       sF32 t = (bandwidth - Q_THRESHOLD2) / (1.0f - Q_THRESHOLD2);
// //       t = (t*t * (3.0f - 2.0f*t));
// //       qScale *= (1.0f - 0.5f*t);
// //    }

   bandwidth = 4.0f * (1.0f - bandwidth) + 0.3f;
}

void StSampleVoiceBiquad::calcCoeff(sSI  type,
                                    sF32 dbGain,   // gain of filter  (0..100, def=2)
                                    sF32 freq,     // center frequency
                                    sF32 srate,    // sampling rate
                                    sF32 bandwidth // bandwidth in octaves
                                    )
{
   // sF32 A;
   sF32 omega, sn, cs, alpha;
   // sF32 beta;
   sF32 a0, a1, a2, b0, b1, b2;

   // Dyac_host_printf("xxx flt freq=%f\n", freq);

   /* setup variables */
   // A = powf(10, dbGain /40);
   omega = (sF32) (2 * sM_PI * freq /srate);
   sn = sinf(omega);
   cs = cosf(omega);
   alpha = (sF32) (sn * sinh(sM_LN2 /2 * bandwidth * omega /sn));
   // beta = sqrtf(A + A);

   switch (type)
   {
      default:
      case LPF:
         b0 = (1 - cs) /2;
         b1 = 1 - cs;
         b2 = (1 - cs) /2;
         a0 = 1 + alpha;
         a1 = -2 * cs;
         a2 = 1 - alpha;
         break;

      case HPF:
         b0 = (1 + cs) /2;
         b1 = -(1 + cs);
         b2 = (1 + cs) /2;
         a0 = 1 + alpha;
         a1 = -2 * cs;
         a2 = 1 - alpha;
         break;

      case BPF:
         b0 = alpha;
         b1 = 0;
         b2 = -alpha;
         a0 = 1 + alpha;
         a1 = -2 * cs;
         a2 = 1 - alpha;
         break;

      case BRF: //NOTCH:
         b0 = 1;
         b1 = -2 * cs;
         b2 = 1;
         a0 = 1 + alpha;
         a1 = -2 * cs;
         a2 = 1 - alpha;
         break;

      // case PEQ:
      //    b0 = 1 + (alpha * A);
      //    b1 = -2 * cs;
      //    b2 = 1 - (alpha * A);
      //    a0 = 1 + (alpha /A);
      //    a1 = -2 * cs;
      //    a2 = 1 - (alpha /A);
      //    break;

      // case LSH:
      //    b0 = A * ((A + 1) - (A - 1) * cs + beta * sn);
      //    b1 = 2 * A * ((A - 1) - (A + 1) * cs);
      //    b2 = A * ((A + 1) - (A - 1) * cs - beta * sn);
      //    a0 = (A + 1) + (A - 1) * cs + beta * sn;
      //    a1 = -2 * ((A - 1) + (A + 1) * cs);
      //    a2 = (A + 1) + (A - 1) * cs - beta * sn;
      //    break;

      // case HSH:
      //    b0 = A * ((A + 1) + (A - 1) * cs + beta * sn);
      //    b1 = -2 * A * ((A - 1) + (A + 1) * cs);
      //    b2 = A * ((A + 1) + (A - 1) * cs - beta * sn);
      //    a0 = (A + 1) - (A - 1) * cs + beta * sn;
      //    a1 = 2 * ((A - 1) - (A + 1) * cs);
      //    a2 = (A + 1) - (A - 1) * cs - beta * sn;
      //    break;
   }

   /* precompute the coefficients */
   biquad.a0 = b0 / a0;
   biquad.a1 = b1 / a0;
   biquad.a2 = b2 / a0;
   biquad.a3 = a1 / a0;
   biquad.a4 = a2 / a0;
}

void StSampleVoiceBiquad::calcParams(sSI _type,
                                     sF32 _dbGain,
                                     sF32 _freq,   // 0..1
                                     sF32 _res,    // 0..1
                                     sF32 _srate
                                     ) {
   // Filter type
   sSI type = _type;
   if(type < 0)
      type = 0;
   else if(type > BRF)
      type = BRF;

   // Cutoff/center frequency
   sF32 freq = _freq;
   //Dyac_host_printf("xxx freq=%f\n", freq);

   freq = sMIN(1.0f, sMAX(freq, 0.0001f));

#define Dfltfreq(f) (powf(2.0f, (f)*(f)) - 1.0f)
// #define Dfltfreq(f) (powf(2.0f, (f)) - 1.0f)
// #define Dfltfreq(f) (powf(2.0f, (f)) - 1.0f)

// #define Dfltfreq(f) ((log((f)*44100.0) * 1.44269504089/*log(2)*/) / 44100.0)
// #define Dfltfreq(f) ((log((f)*44100.0) * 1.44269504089/*log(2)*/) / 1024.0)
   freq = Dfltfreq(_freq);
#undef Dfltfreq

#if 0
   sF32 ra = ((lfsr.rand()&65535u) * (1.0f/32768.0f)) - 1.0f; // xxx analogue feel
   freq = freq * (1.0f + ra*0.15f);
#endif

   if(freq < 0.0f)
      freq = 0.0f;
   if(freq > 1.0f)
      freq = 1.0f;

   if(freq < 0.0005f)
      freq = 0.0005f;

   // Resonance
   calcQ(freq, bandwidth/*out*/, qScale/*out*/, _res/*q*/);

   // Recalc coefficients
   calcCoeff(type,
             _dbGain,
             freq * 0.5f * _srate,
             _srate,
             bandwidth
             );
}

sF32 StSampleVoiceBiquad::filter(const sF32 _inSmp) {
   sF32 outSmp = sF32(
      qScale * biquad.a0 * qScale * _inSmp +
      biquad.a1 * state.x1 +
      biquad.a2 * state.x2 -
      biquad.a3 * state.y1 -
      biquad.a4 * state.y2
                 );

#if 1
   if(outSmp < -10.0f) // (todo) is this check really necessary ?
      outSmp = 0.0f;
   else if(outSmp > 10.0f)
      outSmp = 0.0f;
#endif

   outSmp = Dyac_denorm_32(outSmp);

   // Shuffle history
   /* shift x1 to x2, sample to x1 */
   state.x2 = state.x1;
   state.x1 = _inSmp /** qScale*/;
   state.x1 = Dyac_denorm_32(state.x1);

   /* shift y1 to y2, result to y1 */
   state.y2 = state.y1;
   state.y1 = outSmp;

   return outSmp;
}



void StSampleVoiceBiquad2::calcParams(sF32 _blkSz,
                                      sSI _type,
                                      sF32 _dbGain,
                                      sF32 _freq,   // 0..1
                                      sF32 _res     // 0..1
                                      ) {
#define Q _res
#define Fc _freq

   cur_a0 = a0;
   cur_a1 = a1;
   cur_a2 = a2;
   cur_b1 = b1;
   cur_b2 = b2;

   // (note) sound dies after a short while with fmax=1.0
   // // _freq = sMIN(0.992f, sMAX(_freq, 0.001f));
   _freq = sMIN(0.9999f, sMAX(_freq, 0.001f));

   // (note) sound becomes very quiet with res < 0.5
   _res += 0.5f;
   if(_res < 0.5f)
      _res = 0.5f;
   else if(_res > 1.5f)
      _res = 1.5f;

   sF64 norm;
   sF64 K = tan(sM_PI * Fc);
   switch(_type)
   {
      case LPF:
         norm = 1 / (1 + K / Q + K * K);
         a0 = K * K * norm;
         a1 = 2 * a0;
         a2 = a0;
         b1 = 2 * (K * K - 1) * norm;
         b2 = (1 - K / Q + K * K) * norm;
         break;

      case HPF:
         norm = 1 / (1 + K / Q + K * K);
         a0 = 1 * norm;
         a1 = -2 * a0;
         a2 = a0;
         b1 = 2 * (K * K - 1) * norm;
         b2 = (1 - K / Q + K * K) * norm;
#if 0
         {
            static int xxx = 0;
            if(0 == (++xxx & 511))
            {
               Dyac_host_printf("xxx hpf a0=%f a1=%f b1=%f b2=%f\n", a0, a1, b1, b2);
            }
         }
#endif
         break;

      case BPF:
         norm = 1 / (1 + K / Q + K * K);
         a0 = K / Q * norm;
         a1 = 0;
         a2 = -a0;
         b1 = 2 * (K * K - 1) * norm;
         b2 = (1 - K / Q + K * K) * norm;
         break;

      case BRF:
         norm = 1 / (1 + K / Q + K * K);
         a0 = (1 + K * K) * norm;
         a1 = 2 * (K * K - 1) * norm;
         a2 = a0;
         b1 = a1;
         b2 = (1 - K / Q + K * K) * norm;
         break;

      case PEQ:
      {
         sF64 V = pow(10, fabs(_dbGain) / 20.0);  // PEQ, LSH, HSH

         if(_dbGain >= 0)
         {
            // boost
            norm = 1 / (1 + 1/Q * K + K * K);
            a0 = (1 + V/Q * K + K * K) * norm;
            a1 = 2 * (K * K - 1) * norm;
            a2 = (1 - V/Q * K + K * K) * norm;
            b1 = a1;
            b2 = (1 - 1/Q * K + K * K) * norm;
         }
         else
         {
            // cut
            norm = 1 / (1 + V/Q * K + K * K);
            a0 = (1 + 1/Q * K + K * K) * norm;
            a1 = 2 * (K * K - 1) * norm;
            a2 = (1 - 1/Q * K + K * K) * norm;
            b1 = a1;
            b2 = (1 - V/Q * K + K * K) * norm;
         }
      }
      break;

      case LSH:
      {
         sF64 V = pow(10, fabs(_dbGain) / 20.0);  // PEQ, LSH, HSH

         if(_dbGain >= 0)
         {
            // boost
            norm = 1 / (1 + sqrt(2) * K + K * K);
            a0 = (1 + sqrt(2*V) * K + V * K * K) * norm;
            a1 = 2 * (V * K * K - 1) * norm;
            a2 = (1 - sqrt(2*V) * K + V * K * K) * norm;
            b1 = 2 * (K * K - 1) * norm;
            b2 = (1 - sqrt(2) * K + K * K) * norm;
         }
         else
         {
            // cut
            norm = 1 / (1 + sqrt(2*V) * K + V * K * K);
            a0 = (1 + sqrt(2) * K + K * K) * norm;
            a1 = 2 * (K * K - 1) * norm;
            a2 = (1 - sqrt(2) * K + K * K) * norm;
            b1 = 2 * (V * K * K - 1) * norm;
            b2 = (1 - sqrt(2*V) * K + V * K * K) * norm;
         }
      }
      break;

      case HSH:
      {
         sF64 V = pow(10, fabs(_dbGain) / 20.0);  // PEQ, LSH, HSH

         if(_dbGain >= 0)
         {
            // boost
            norm = 1 / (1 + sqrt(2) * K + K * K);
            a0 = (V + sqrt(2*V) * K + K * K) * norm;
            a1 = 2 * (K * K - V) * norm;
            a2 = (V - sqrt(2*V) * K + K * K) * norm;
            b1 = 2 * (K * K - 1) * norm;
            b2 = (1 - sqrt(2) * K + K * K) * norm;
         }
         else
         {
            // cut
            norm = 1 / (V + sqrt(2*V) * K + K * K);
            a0 = (1 + sqrt(2) * K + K * K) * norm;
            a1 = 2 * (K * K - 1) * norm;
            a2 = (1 - sqrt(2) * K + K * K) * norm;
            b1 = 2 * (K * K - V) * norm;
            b2 = (V - sqrt(2*V) * K + K * K) * norm;
         }
      }
      break;
   }
#undef Q
#undef Fc

   step_a0 = (a0 - cur_a0) / _blkSz;
   step_a1 = (a1 - cur_a1) / _blkSz;
   step_a2 = (a2 - cur_a2) / _blkSz;
   step_b1 = (b1 - cur_b1) / _blkSz;
   step_b2 = (b2 - cur_b2) / _blkSz;
}

void StSampleVoiceBiquad2::calcParamsLPFStatic(sF32 _fc, sF32 _q) {
   sF64 norm;
   sF64 K = tan(sM_PI * _fc);
   // case LPF:
   const sF64 Q = _q + 0.5f;
   norm = 1 / (1 + K / Q + K * K);
   cur_a0 = K * K * norm;
   cur_a1 = 2 * cur_a0;
   cur_a2 = cur_a0;
   cur_b1 = 2 * (K * K - 1) * norm;
   cur_b2 = (1 - K / Q + K * K) * norm;
}

sF32 StSampleVoiceBiquad2::filter(const sF32 _inSmp) {
   sF64 out = _inSmp * cur_a0 + z1;
   z1 = _inSmp * cur_a1 + z2 - cur_b1 * out;
   z2 = _inSmp * cur_a2 - cur_b2 * out;

   cur_a0 += step_a0;
   cur_a1 += step_a1;
   cur_a2 += step_a2;
   cur_b1 += step_b1;
   cur_b2 += step_b2;

   sF32 out32 = sF32(out);
   return Dyac_denorm_32(out32);
}

sF32 StSampleVoiceBiquad2::filterNoStep(const sF32 _inSmp) {
   sF64 out = _inSmp * cur_a0 + z1;
   z1 = _inSmp * cur_a1 + z2 - cur_b1 * out;
   z2 = _inSmp * cur_a2 - cur_b2 * out;

   sF32 out32 = sF32(out);
   return Dyac_denorm_32(out32);
}

sF32 StSampleVoiceBiquad2::filterNoStepNoDenorm(const sF32 _inSmp) {
   sF64 out = _inSmp * cur_a0 + z1;
   z1 = _inSmp * cur_a1 + z2 - cur_b1 * out;
   z2 = _inSmp * cur_a2 - cur_b2 * out;
   return sF32(out);
}

void StSampleVoice::recalcAdditiveTblMix(void) {
   if(NULL == additive_tbl)
   {
      additive_tbl = new(std::nothrow)sF32[4096];
   }
   sample->recalcAdditiveTbl_int(999999/*mix only*/, additive_tbl, mmdst_additive_cfg);
   last_additive_cfg = mmdst_additive_cfg;
}

sUI StSampleVoice::_mmVarGetLastSeenIdx(void) {
   return mmvar_last_seen_var_idx;
}
