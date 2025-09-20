// ----
// ---- file   : modmatrix.cpp
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2009-2025 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : This is part of the "syntracker" midi sequencer.
// ----
// ---- created: 19Feb2020
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
// ----          19Feb2020, 21Feb2020, 24Feb2020, 27Feb2020, 06Mar2020, 19Mar2020, 28May2020
// ----          01Jun2020, 05Jun2020, 06Jun2020, 23Aug2020, 07Sep2020, 13Oct2020, 18Jul2021
// ----          30Aug2021, 05Sep2021, 24Nov2022, 20Dec2022, 21Dec2022, 22Dec2022, 23Dec2022
// ----          27Dec2022, 30Dec2022, 07Apr2023, 12Apr2023, 18Jul2023, 01Sep2023, 08Sep2023
// ----          19Sep2023, 18Nov2023, 08Jan2024, 10Jan2024, 15Jan2024, 16Jan2024, 26Apr2024
// ----          30Sep2024, 02Oct2024, 03Jan2025, 04Jan2025, 28May2025
// ----
// ----
// ----

#define YAC_EPSILONCOMPARE_ABS defined
#define YAC_EPSILONCOMPARE_ABS_DEFAULT defined

#include "tksampler.h"

//   #define DP if(0);else Dyac_host_printf
 #define DP if(1);else Dyac_host_printf


sF32 stsamplevoice_bipolar_to_scale(const sF32 _t, const sF32 _div, const sF32 _mul) {
   // t (-1..1) => /_div .. *_mul

   sF32 s;

   if(_t < 0.0f)
   {
      s = (1.0f / _div);
      s = 1.0f + (s - 1.0f) * -_t;
      if(s < 0.0f)
         s = 0.0f;
   }
   else
   {
      s = _mul;
      s = 1.0f + (s - 1.0f) * _t;
   }

   return s;
}

// static sF32 loc_log_lin_exp(sF32 _f, sF32 _c) {
//    // c: <0: log
//    //     0: lin
//    //    >0: exp
//    yacmem uSign;
//    uSign.f32 = _f;
//    yacmem u;
//    u.f32 = _f;
//    u.u32 &= 0x7fffFFFFu;
//    u.f32 = powf(u.f32, powf(2.0f, _c));
//    u.u32 |= uSign.u32 & 0x80000000u;
//    return u.f32;
// }

void StSampleVoice::calcModMatrix(tksampler_mmdst_t &mmdst) {

   struct  {
      struct {
         sF32 freq;
         sF32 vol;
         sF32 pan;
         sF32 aux;
      } env;
      struct {
         sF32 freq;
         sF32 vol;
         sF32 pan;
         sF32 aux;
      } lfo;
      sF32 modseq[STSAMPLE_NUM_MODSEQ];
   } mmsrc;

   sBool bFreqLFOGlobal = YAC_FALSE;
   sBool bVolLFOGlobal  = YAC_FALSE;
   sBool bPanLFOGlobal  = YAC_FALSE;
   sBool bAuxLFOGlobal  = YAC_FALSE;

   const sF32 *perfCtl =
      (!b_release || ((NULL == sample->parent_samplebank) || !sample->parent_samplebank->b_perfctl_freeze_noteoff))
      ? sample_player->perf_ctl
      : perf_ctl_off
      ;

#ifdef YAC_GCC
   // avoid GCC warning
   mmsrc.lfo.freq = 0.0f;
   mmsrc.lfo.vol  = 0.0f;
   mmsrc.lfo.pan  = 0.0f;
   mmsrc.lfo.aux  = 0.0f;
#endif // YAC_GCC

   // Freq LFO
   if( (NULL != sample->lfo_freq) && (sample->lfo_freq->b_global) )
   {
      if(sample->global_lfo_tick_nr != sample_player->global_tick_nr)
      {
         for(sUI i = 0u; i < sample_player->global_tick_advance; i++)
         {
            mmsrc.lfo.freq = sample->lfo_freq_global.tick();
         }
      }
      else
      {
         mmsrc.lfo.freq = sample->lfo_freq_global.last_level;
      }
      bFreqLFOGlobal = YAC_TRUE;
   }
   else
   {
      mmsrc.lfo.freq = lfo_freq.tick();
   }

   // Vol LFO
   if( (NULL != sample->lfo_vol) && (sample->lfo_vol->b_global) )
   {
      if(sample->global_lfo_tick_nr != sample_player->global_tick_nr)
      {
         for(sUI i = 0u; i < sample_player->global_tick_advance; i++)
         {
            mmsrc.lfo.vol = sample->lfo_vol_global.tick();
         }
      }
      else
      {
         mmsrc.lfo.vol = sample->lfo_vol_global.last_level;
      }
      bVolLFOGlobal = YAC_TRUE;
   }
   else
   {
      mmsrc.lfo.vol = lfo_vol.tick();
   }

   // Pan LFO
   if( (NULL != sample->lfo_pan) && (sample->lfo_pan->b_global) )
   {
      if(sample->global_lfo_tick_nr != sample_player->global_tick_nr)
      {
         for(sUI i = 0u; i < sample_player->global_tick_advance; i++)
         {
            mmsrc.lfo.pan = sample->lfo_pan_global.tick();
         }
      }
      else
      {
         mmsrc.lfo.pan = sample->lfo_pan_global.last_level;
      }
      bPanLFOGlobal = YAC_TRUE;
   }
   else
   {
      mmsrc.lfo.pan = lfo_pan.tick();
   }

   // Aux LFO
   if( (NULL != sample->lfo_aux) && (sample->lfo_aux->b_global) )
   {
      if(sample->global_lfo_tick_nr != sample_player->global_tick_nr)
      {
         for(sUI i = 0u; i < sample_player->global_tick_advance; i++)
         {
            mmsrc.lfo.aux = sample->lfo_aux_global.tick();
         }
      }
      else
      {
         mmsrc.lfo.aux = sample->lfo_aux_global.last_level;
      }
      bAuxLFOGlobal = YAC_TRUE;
   }
   else
   {
      mmsrc.lfo.aux = lfo_aux.tick();
   }

   // Mod sequencers
   sBool bModSeqGlobal[STSAMPLE_NUM_MODSEQ];
   for(sUI i = 0u; i < STSAMPLE_NUM_MODSEQ; i++)
   {
      StModSeq *mseq = sample->modseq[i][modseq_patch[i]];
      if( (NULL != mseq) && mseq->b_global )
      {
         if(sample->global_modseq_tick_nr != sample_player->global_tick_nr)
         {
            sUI numAdv = sample_player->global_tick_advance;
            mmsrc.modseq[i] = sample->modseq_global[i].tick(YAC_FALSE/*bPulseGate*/);
            for(sUI j = 1u; j < numAdv; j++)
            {
               // (note) hold pulse level if prev tick() was a pulse (gate)
               mmsrc.modseq[i] = sample->modseq_global[i].tick(YAC_TRUE/*bPulseGate*/);
            }
         }
         else
         {
            mmsrc.modseq[i] = sample->modseq_global[i].last_level;
         }
         bModSeqGlobal[i] = YAC_TRUE;
      }
      else
      {
         mmsrc.modseq[i] = modseq[i].tick(YAC_FALSE/*bPulseGate*/);
         bModSeqGlobal[i] = YAC_FALSE;
      }
   }

   sample->global_modseq_tick_nr = sample_player->global_tick_nr;
   sample->global_lfo_tick_nr = sample_player->global_tick_nr;

   mmsrc.env.freq = adsr_freq.adsr ? adsr_freq.tick() : 0.0f;
   mmsrc.env.vol  = adsr_vol.adsr  ? adsr_vol.tick()  : 0.0f;
   mmsrc.env.pan  = adsr_pan.adsr  ? adsr_pan.tick()  : 0.0f;
   mmsrc.env.aux  = adsr_aux.adsr  ? adsr_aux.tick()  : 0.0f;
   // Dyac_host_printf("xxx adsr_vol.adsr=%p mmsrc.env.vol=%f\n", adsr_vol.adsr, mmsrc.env.vol);

   if(sample->voice_calibration_enable[4/*pan env lvl*/])
   {
      sF32 a =
         sample->voice_calibration_amount[4/*pan env lvl*/] * sample->voice_calibration
         [4/*pan env lvl*/]
         [voice_alloc_idx % sample->voice_calibration_modulo[4/*pan env lvl*/]];
      a = stsamplevoice_bipolar_to_scale(a, 8.0f/*div*/, 8.0f/*mul*/);
      mmsrc.env.pan *= a;
   }

   if(sample->voice_calibration_enable[5/*aux env lvl*/])
   {
      sF32 a =
         sample->voice_calibration_amount[5/*aux env lvl*/] * sample->voice_calibration
         [5/*aux env lvl*/]
         [voice_alloc_idx % sample->voice_calibration_modulo[5/*aux env lvl*/]];
      a = stsamplevoice_bipolar_to_scale(a, 8.0f/*div*/, 8.0f/*mul*/);
      mmsrc.env.aux *= a;
   }

   if(perfctl_poly_pressure >= perfctl_pressure_max)
   {
      perfctl_pressure_max = perfctl_poly_pressure;
   }
   else if(perfCtl[STSAMPLEPLAYER_PERFCTL_PRESSURE] >= perfctl_pressure_max)
   {
      perfctl_pressure_max = perfCtl[STSAMPLEPLAYER_PERFCTL_PRESSURE];
   }


   mmdst.vol          = 0.0f;
   mmdst.pan          = 0.0f;

   mmdst.freq           = 0.0f;
   mmdst.freq_oct       = 0.0f;
   mmdst.freq_fifth     = 0.0f;
   mmdst.freq_fourth    = 0.0f;
   mmdst.freq_semi      = 0.0f;
   mmdst.freq_fine      = 0.0f;
   mmdst.freq_beat      = 0.0f;
   mmdst.tuning_tbl_abs = 0.0f;
   mmdst.tuning_tbl_rel = -1.0f;
   mmdst.note_2         = 0.0f;
   mmdst.freq_2         = 0.0f;

   mmdst.flt_cutoff   = 0.0f;
   mmdst.flt_pan      = 0.0f;
   mmdst.flt_offset   = 0.0f;
   mmdst.flt_res      = 0.0f;

   mmdst_sampleoff    = 0.0f;
   mmdst_sampleshift  = 0.0f;
   mmdst.sampleshift_endofloop = 0.0f;
   mmdst.b_sampleshift_endofloop_valid = YAC_FALSE;
   mmdst.looprestart  = 0.0f;
   mmdst_cyclelen     = 1.0f;
   mmdst_cyclelen_eol = 1.0f;
   b_mmdst_cyclelen_eol_valid = YAC_FALSE;
   mmdst_cyclelen_sampleoff = 0.0f;
   mmdst.jumptoloop  = 0.0f;
   mmdst.b_jumptoloop_valid = YAC_FALSE;
   mmdst_loop_repeats_scale = 1.0f;

   mmdst.wt2d_x         = 0.0f;
   mmdst.b_wt2d_x_valid = YAC_FALSE;
   mmdst.wt2d_y         = 0.0f;
   mmdst.b_wt2d_y_valid = YAC_FALSE;

   mmdst.freq_lfo_freq_amt = 1.0f;
   mmdst.vol_lfo_vol_amt   = 1.0f;
   mmdst.pan_lfo_pan_amt   = 1.0f;
   mmdst.aux_lfo_flt_amt   = 1.0f;

   mmdst.lfo_freq_level = 1.0f;  // *
   mmdst.lfo_vol_level  = 1.0f;  // *
   mmdst.lfo_pan_level  = 1.0f;  // *
   mmdst.lfo_aux_level  = 1.0f;  // *

   mmdst.env_freq_level = 1.0f;  // *
   mmdst.env_vol_level  = 1.0f;  // *
   mmdst.env_pan_level  = 1.0f;  // *
   mmdst.env_aux_level  = 1.0f;  // *

   mmdst.retrig_freq_env = 0.0f;
   mmdst.retrig_vol_env  = 0.0f;
   mmdst.retrig_pan_env  = 0.0f;
   mmdst.retrig_aux_env  = 0.0f;

   mmdst.retrig_freq_lfo = 0.0f;
   mmdst.retrig_vol_lfo  = 0.0f;
   mmdst.retrig_pan_lfo  = 0.0f;
   mmdst.retrig_aux_lfo  = 0.0f;

   mmdst_glide_speed = 1.0f;
   mmdst_timestretch = 0.0f;
   mmdst_timestretch_bend = 0.0f;

   mmdst.liverec_start    = 0.0f;
   mmdst.liverec_continue = 0.0f;
   mmdst.liverec_stop     = 0.0f;

   mmdst.plugin_mod_mask = 0u;
   memset((void*)mmdst.plugins, 0, sizeof(mmdst.plugins));

   mmdst_sync_speed = 0.0f;
   mmdst_sync_slew  = 1.0f;

   mmdst_srr = 0.0f;
   mmdst_br  = 0.0f;

   // (todo) speed/intensity modulation should be updated before modmatrix processing
   //         it's currently a tick too late (but probably not noticeable)
   lfo_freq.mmdst_speed = 1.0f;
   lfo_vol.mmdst_speed  = 1.0f;
   lfo_pan.mmdst_speed  = 1.0f;
   lfo_aux.mmdst_speed  = 1.0f;

   adsr_freq.mmdst_speed     = 1.0f;
   // // adsr_freq.mmdst_level     = 1.0f;  // *
   adsr_freq.mmdst_intensity = 0.0f;  // relative
   adsr_freq.mmdst_aspeed    = 1.0f;
   adsr_freq.mmdst_sspeed    = 1.0f;
   adsr_freq.mmdst_rspeed    = 1.0f;

   adsr_vol.mmdst_speed     = 1.0f;
   // // adsr_vol.mmdst_level     = 1.0f;  // *
   adsr_vol.mmdst_intensity = 0.0f;  // relative
   adsr_vol.mmdst_aspeed    = 1.0f;
   adsr_vol.mmdst_sspeed    = 1.0f;
   adsr_vol.mmdst_rspeed    = 1.0f;

   adsr_pan.mmdst_speed     = 1.0f;
   // // adsr_pan.mmdst_level     = 1.0f;  // *
   adsr_pan.mmdst_intensity = 0.0f;  // relative
   adsr_pan.mmdst_aspeed    = 1.0f;
   adsr_pan.mmdst_sspeed    = 1.0f;
   adsr_pan.mmdst_rspeed    = 1.0f;

   adsr_aux.mmdst_speed     = 1.0f;
   // // adsr_aux.mmdst_level     = 1.0f;  // *
   adsr_aux.mmdst_intensity = 0.0f;  // relative
   adsr_aux.mmdst_aspeed    = 1.0f;
   adsr_aux.mmdst_sspeed    = 1.0f;
   adsr_aux.mmdst_rspeed    = 1.0f;


   for(sUI i = 0u; i < STSAMPLE_NUM_MODSEQ; i++)
   {
      modseq[i].mmdst_speed    = 1.0f;
      // // modseq[i].mmdst_level    = 0.0f;  // relative
      modseq[i].mmdst_numsteps = 0.0f;  // relative
      modseq[i].mmdst_advance  = 0.0f;  // relative
      modseq[i].mmdst_step     = 0.0f;  // 0..1
      modseq[i].b_mmdst_step   = YAC_FALSE;

      sample->modseq_global[i].mmdst_speed    = 1.0f;
      // // sample->modseq_global[i].mmdst_level    = 0.0f;  // relative
      sample->modseq_global[i].mmdst_numsteps = 0.0f;  // relative
      sample->modseq_global[i].mmdst_advance  = 0.0f;  // relative

      mmdst.modseq_level[i]  = 1.0f;  // *
      mmdst.modseq_patch[i]  = sF32(modseq_patch[i]);
      mmdst.retrig_modseq[i] = 0.0f;
   }

   sBool mmWriteMask[STSAMPLE_NUM_MODMATRIX_ENTRIES] = { 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 };

   for(sUI pluginIdx = 0u; pluginIdx < STSAMPLE_NUM_PLUGINS; pluginIdx++)
   {
      mmdst.plugin_levels[pluginIdx] = 1.0f;
   }

   mmdst_additive_cfg           = 0.0f;
   mmdst_additive_stereo_spread = 0.0f;
   mmdst_additive_num_partials  = 0.0f;

   sUI signalTapIdx = 0u;

#define Dstr(d) #d
#define Dsignaltap(d) if(mm->b_signal_tap) Dyac_host_printf("[>>>] voice_idx=%u ticks=%u signal_tap[%u]<%s>=%f\n", voice_idx, replay_ticks, signalTapIdx++, Dstr(d), (d))


   for(sUI mmIdx = 0u; mmIdx < STSAMPLE_NUM_MODMATRIX_ENTRIES; mmIdx++)
   {
      const StSampleMMEntry *mm = &sample->modmatrix[mmIdx];
      sF32 mmNote;
      if(mm->b_enable)
      {
         sF32 srcVal = 0.0f;
         sF32 srcValCurveRangeScale = 1.0f;  // for KEYBOARD_* (and others that exceed normalized -1..1 range in mm curve lookup)
         sBool bSrcValid = YAC_TRUE;
         switch(mm->src)
         {
            default:
            case STSAMPLE_MM_SRC_NONE:
               srcVal = 0.0f;
               break;

            case STSAMPLE_MM_SRC_CONST_1:
               srcVal = 1.0f;
               break;

            case STSAMPLE_MM_SRC_CONST_M1:
               srcVal = -1.0f;
               break;

            case STSAMPLE_MM_SRC_CONST_0:
               srcVal = 0.0f;
               break;

            case STSAMPLE_MM_SRC_CONST_4:
               srcVal = 4.0f;
               break;

            case STSAMPLE_MM_SRC_CONST_16:
               srcVal = 16.0f;
               break;

            case STSAMPLE_MM_SRC_CONST_100:
               srcVal = 100.0f;
               break;

            case STSAMPLE_MM_SRC_CONST_127:
               srcVal = 127.0f;
               break;

            case STSAMPLE_MM_SRC_CONST_255:
               // e.g. for 'timestretch' dest
               srcVal = 255.0f;
               break;

            case STSAMPLE_MM_SRC_RAND_1_PM:
               srcVal = noteon_rand_pm[0];
               break;

            case STSAMPLE_MM_SRC_RAND_1_P:
               srcVal = noteon_rand_p[0];
               break;

            case STSAMPLE_MM_SRC_RAND_2_PM:
               srcVal = noteon_rand_pm[1];
               break;

            case STSAMPLE_MM_SRC_RAND_2_P:
               srcVal = noteon_rand_p[1];
               break;

            case STSAMPLE_MM_SRC_RAND_3_PM:
               srcVal = noteon_rand_pm[2];
               break;

            case STSAMPLE_MM_SRC_RAND_3_P:
               srcVal = noteon_rand_p[2];
               break;

            case STSAMPLE_MM_SRC_RAND_4_PM:
               srcVal = noteon_rand_pm[3];
               break;

            case STSAMPLE_MM_SRC_RAND_4_P:
               srcVal = noteon_rand_p[3];
               break;

            case STSAMPLE_MM_SRC_KEYBOARD_PM:
               mmNote = sample->calcMMNoteRel((sF32)note);
               srcVal = mmNote * (8.0f / 48.0f);
               srcValCurveRangeScale = 0.01f;
               break;

            case STSAMPLE_MM_SRC_KEYBOARD_P:
               mmNote = sample->calcMMNoteRel((sF32)note);
               srcVal = mmNote * (8.0f / 48.0f);  // -/+5 oct => -10..10
               if(srcVal < 0.0f)
                  srcVal = 0.0f;
               srcValCurveRangeScale = 0.01f;
               // yac_host->printf("xxx note=%d srcVal=%f\n", note, srcVal);
               break;

            case STSAMPLE_MM_SRC_KEYBOARD_M:
               mmNote = sample->calcMMNoteRel((sF32)note);
               srcVal = mmNote * (-8.0f / 48.0f);
               if(srcVal < 0.0f)
                  srcVal = 0.0f;
               srcValCurveRangeScale = 0.01f;
               break;

            case STSAMPLE_MM_SRC_KEYBOARD_ABS:
               mmNote = sample->calcMMNoteAbs((sF32)note);
               srcVal = mmNote * (8.0f / 127.0f);
               srcValCurveRangeScale = 1/8.0f;
               break;

            case STSAMPLE_MM_SRC_VOICE_NR:
               srcVal = sF32(voice_alloc_idx % sample_player->num_voices);
               break;

            case STSAMPLE_MM_SRC_VOICE_NR_EVEN:
               srcVal = (voice_alloc_idx & 1u) ? 0.0f : 1.0f;
               break;

            case STSAMPLE_MM_SRC_VOICE_NR_ODD:
               srcVal = (voice_alloc_idx & 1u) ? 1.0f : 0.0f;
               break;

            case STSAMPLE_MM_SRC_VOICE_NR_EVENODD:
               srcVal = (voice_alloc_idx & 1u) ? -1.0f : 1.0f;
               break;

            case STSAMPLE_MM_SRC_VOICE_NR_ODDEVEN:
               srcVal = (voice_alloc_idx & 1u) ? 1.0f : -1.0f;
               break;

            case STSAMPLE_MM_SRC_ACTIVE_NOTES:
               srcVal = sF32(active_note_idx + 1u) / sF32(sample->max_voices);
               if(srcVal > 1.0f)
                  srcVal = 1.0f;
               break;

            case STSAMPLE_MM_SRC_ACTIVE_NOTE_EVEN:
               srcVal = (0u != (active_note_idx & 1u)) ? 0.0f : 1.0f;
               break;

            case STSAMPLE_MM_SRC_ACTIVE_NOTE_ODD:
               srcVal = (0u != (active_note_idx & 1u)) ? 1.0f : 0.0f;
               break;

            case STSAMPLE_MM_SRC_ACTIVE_NOTE_EVENODD:
               srcVal = (0u != (active_note_idx & 1u)) ? -1.0f : 1.0f;
               break;

            case STSAMPLE_MM_SRC_ACTIVE_NOTE_ODDEVEN:
               srcVal = (0u != (active_note_idx & 1u)) ? 1.0f : -1.0f;
               break;

            case STSAMPLE_MM_SRC_LAYER_NR:
               srcVal = sF32(layer_idx);
               break;

            case STSAMPLE_MM_SRC_LAYER_NR_EVEN:
               srcVal = (layer_idx & 1u) ? 0.0f : 1.0f;
               break;

            case STSAMPLE_MM_SRC_LAYER_NR_ODD:
               srcVal = (layer_idx & 1u) ? 1.0f : 0.0f;
               break;

            case STSAMPLE_MM_SRC_LAYER_NR_EVENODD:
               srcVal = (layer_idx & 1u) ? -1.0f : 1.0f;
               break;

            case STSAMPLE_MM_SRC_LAYER_NR_ODDEVEN:
               srcVal = (layer_idx & 1u) ? 1.0f : -1.0f;
               break;

            case STSAMPLE_MM_SRC_VELOCITY:
               srcVal = queued_noteon._vel;
               break;

            case STSAMPLE_MM_SRC_VELOCITY_HI:
               srcVal = queued_noteon._vel;
               if(srcVal >= (96.0f / 127.0f))
               {
                  srcVal -= (96.0f / 127.0f);
                  srcVal *= 1.0f / ((127.0f - 96.0f)/127.0f);  // => 0..1
               }
               else
               {
                  srcVal = 0.0f;
               }
               break;

            case STSAMPLE_MM_SRC_VELOCITY_EXP2:
               srcVal = queued_noteon._vel;
               srcVal *= srcVal;
               break;

            case STSAMPLE_MM_SRC_VELOCITY_EXP3:
               srcVal = queued_noteon._vel;
               srcVal *= srcVal * srcVal;
               break;

            case STSAMPLE_MM_SRC_VELOCITY_SMOOTHSTEP:
               {
                  srcVal = queued_noteon._vel;
                  srcVal = srcVal * srcVal * (3.0f - 2.0f * srcVal);
               }
               break;

            case STSAMPLE_MM_SRC_VELOCITY_INV:
               {
                  srcVal = 1.0f - queued_noteon._vel;
               }
               break;

            case STSAMPLE_MM_SRC_VELOCITY_BIP:
               {
                  srcVal = (queued_noteon._vel * 2.0f) - 1.0f;
               }
               break;

            case STSAMPLE_MM_SRC_RELEASE_VELOCITY:
               srcVal = release_velocity;
               break;

            case STSAMPLE_MM_SRC_RELEASE_VELOCITY_HI:
               srcVal = release_velocity;
               if(srcVal >= (96.0f / 127.0f))
               {
                  srcVal -= (96.0f / 127.0f);
                  srcVal *= 1.0f / ((127.0f - 96.0f)/127.0f);  // => 0..1
               }
               else
               {
                  srcVal = 0.0f;
               }
               break;

            case STSAMPLE_MM_SRC_GATE:
               srcVal = b_release ? 0.0f : 1.0f;//////queued_noteon._vel;
               break;

            case STSAMPLE_MM_SRC_TRIG_PULSE:
               srcVal = b_trig_pulse ? 1.0f : 0.0f;
               break;

            case STSAMPLE_MM_SRC_RELEASE:
               srcVal = b_release ? 1.0f : 0.0f;
               break;

            case STSAMPLE_MM_SRC_RELEASE_PULSE:
               srcVal = b_release_pulse ? 1.0f : 0.0f;
               break;

            case STSAMPLE_MM_SRC_SUSTAIN_PEDAL:
               srcVal = sample_player->getSustainPedal() ? 1.0f : 0.0f;
               break;

            case STSAMPLE_MM_SRC_SOFT_PEDAL:
               srcVal = sample_player->getSoftPedal() ? 1.0f : 0.0f;
               break;

            case STSAMPLE_MM_SRC_PITCHBEND_PM:
               if(perfCtl[STSAMPLEPLAYER_PERFCTL_PITCHBEND] >= 0.0f)
               {
                  srcVal = perfCtl[STSAMPLEPLAYER_PERFCTL_PITCHBEND];
                  if(srcVal >= 8192.0f)
                  {
                     srcVal = (srcVal - 8192.0f) * (1.0f / 8191.0f);
                  }
                  else
                  {
                     srcVal = -1.0f + (srcVal * (1.0f / 8192.0f));
                  }
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_PITCHBEND_PM_ON:
               if(perf_ctl_on[STSAMPLEPLAYER_PERFCTL_PITCHBEND] >= 0.0f)
               {
                  srcVal = perf_ctl_on[STSAMPLEPLAYER_PERFCTL_PITCHBEND];
                  if(srcVal >= 8192.0f)
                  {
                     srcVal = (srcVal - 8192.0f) * (1.0f / 8191.0f);
                  }
                  else
                  {
                     srcVal = -1.0f + (srcVal * (1.0f / 8192.0f));
                  }
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_PITCHBEND_P:
               if(perfCtl[STSAMPLEPLAYER_PERFCTL_PITCHBEND] >= 0.0f)
               {
                  srcVal = perfCtl[STSAMPLEPLAYER_PERFCTL_PITCHBEND];
                  if(srcVal >= 8192.0f)
                  {
                     srcVal = (srcVal - 8192.0f) * (1.0f / 8191.0f);
                  }
                  else
                  {
                     srcVal = 0.0f;
                  }
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_PITCHBEND_P_ON:
               if(perf_ctl_on[STSAMPLEPLAYER_PERFCTL_PITCHBEND] >= 0.0f)
               {
                  srcVal = perf_ctl_on[STSAMPLEPLAYER_PERFCTL_PITCHBEND];
                  if(srcVal >= 8192.0f)
                  {
                     srcVal = (srcVal - 8192.0f) * (1.0f / 8191.0f);
                  }
                  else
                  {
                     srcVal = 0.0f;
                  }
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_PITCHBEND_M:
               if(perfCtl[STSAMPLEPLAYER_PERFCTL_PITCHBEND] >= 0.0f)
               {
                  srcVal = perfCtl[STSAMPLEPLAYER_PERFCTL_PITCHBEND];
                  if(srcVal <= 8192.0f)
                  {
                     srcVal = 1.0f - (srcVal * (1.0f / 8192.0f));
                  }
                  else
                  {
                     srcVal = 0.0f;
                  }
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_PITCHBEND_M_ON:
               if(perf_ctl_on[STSAMPLEPLAYER_PERFCTL_PITCHBEND] >= 0.0f)
               {
                  srcVal = perf_ctl_on[STSAMPLEPLAYER_PERFCTL_PITCHBEND];
                  if(srcVal <= 8192.0f)
                  {
                     srcVal = 1.0f - (srcVal * (1.0f / 8192.0f));
                  }
                  else
                  {
                     srcVal = 0.0f;
                  }
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_PRESSURE:
               if(perfctl_poly_pressure >= 0.0f)
               {
                  // Use per-note poly pressure
                  srcVal = perfctl_poly_pressure * (1.0f / 127.0f);
               }
               else
               {
                  // Use channel pressure
                  if(perfCtl[STSAMPLEPLAYER_PERFCTL_PRESSURE] >= 0.0f)
                  {
                     srcVal = perfCtl[STSAMPLEPLAYER_PERFCTL_PRESSURE] * (1.0f / 127.0f);
                  }
                  else
                  {
                     bSrcValid = YAC_FALSE;
                  }
               }
               break;

            case STSAMPLE_MM_SRC_PRESSURE_ON:
               if(perfctl_poly_pressure >= 0.0f)
               {
                  // Use per-note poly pressure
                  srcVal = perfctl_poly_pressure * (1.0f / 127.0f);
               }
               else
               {
                  // Use channel pressure
                  if(perf_ctl_on[STSAMPLEPLAYER_PERFCTL_PRESSURE] >= 0.0f)
                  {
                     srcVal = perf_ctl_on[STSAMPLEPLAYER_PERFCTL_PRESSURE] * (1.0f / 127.0f);
                  }
                  else
                  {
                     bSrcValid = YAC_FALSE;
                  }
               }
               break;

            case STSAMPLE_MM_SRC_PRESSURE_HI:
               if(perfctl_poly_pressure >= 0.0f)
               {
                  // Use per-note poly pressure
                  srcVal = perfctl_poly_pressure * (1.0f / 127.0f);
               }
               else
               {
                  // Use channel pressure
                  if(perfCtl[STSAMPLEPLAYER_PERFCTL_PRESSURE] >= 0.0f)
                  {
                     srcVal = perfCtl[STSAMPLEPLAYER_PERFCTL_PRESSURE] * (1.0f / 127.0f);
                  }
                  else
                  {
                     bSrcValid = YAC_FALSE;
                  }
               }
               if(bSrcValid)
               {
                  if(srcVal >= (96.0f / 127.0f))
                  {
                     srcVal -= (96.0f / 127.0f);
                     srcVal *= 1.0f / ((127.0f - 96.0f)/127.0f);  // => 0..1
                  }
               }
               else
               {
                  srcVal = 0.0f;
               }
               break;

            case STSAMPLE_MM_SRC_PRESSURE_HI_ON:
               if(perf_ctl_on[STSAMPLEPLAYER_PERFCTL_PRESSURE] >= 0.0f)
               {
                  srcVal = perf_ctl_on[STSAMPLEPLAYER_PERFCTL_PRESSURE] * (1.0f / 127.0f);
                  if(srcVal >= (96.0f / 127.0f))
                  {
                     srcVal -= (96.0f / 127.0f);
                     srcVal *= 1.0f / ((127.0f - 96.0f)/127.0f);  // => 0..1
                  }
                  else
                  {
                     srcVal = 0.0f;
                  }
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_PRESSURE_MAX:
               srcVal = perfctl_pressure_max * (1.0f / 127.0f);
               break;

            case STSAMPLE_MM_SRC_CC1_MODWHEEL:
               if(perfCtl[STSAMPLEPLAYER_PERFCTL_CC1_MODWHEEL] >= 0.0f)
               {
                  srcVal = perfCtl[STSAMPLEPLAYER_PERFCTL_CC1_MODWHEEL] * (1.0f / 127.0f);
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_CC1_MODWHEEL_ON:
               if(perf_ctl_on[STSAMPLEPLAYER_PERFCTL_CC1_MODWHEEL] >= 0.0f)
               {
                  srcVal = perf_ctl_on[STSAMPLEPLAYER_PERFCTL_CC1_MODWHEEL] * (1.0f / 127.0f);
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_CC1_MODWHEEL_INV:
               if(perfCtl[STSAMPLEPLAYER_PERFCTL_CC1_MODWHEEL] >= 0.0f)
               {
                  srcVal = perfCtl[STSAMPLEPLAYER_PERFCTL_CC1_MODWHEEL] * (1.0f / 127.0f);
                  srcVal = 1.0f - srcVal;
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_CC1_MODWHEEL_INV_ON:
               if(perf_ctl_on[STSAMPLEPLAYER_PERFCTL_CC1_MODWHEEL] >= 0.0f)
               {
                  srcVal = perf_ctl_on[STSAMPLEPLAYER_PERFCTL_CC1_MODWHEEL] * (1.0f / 127.0f);
                  srcVal = 1.0f - srcVal;
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_CC1_MODWHEEL_BIP:
               if(perfCtl[STSAMPLEPLAYER_PERFCTL_CC1_MODWHEEL] >= 0.0f)
               {
                  srcVal = perfCtl[STSAMPLEPLAYER_PERFCTL_CC1_MODWHEEL] * (1.0f / 127.0f);
                  srcVal = (srcVal * 2.0f) - 1.0f;
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_CC1_MODWHEEL_BIP_ON:
               if(perf_ctl_on[STSAMPLEPLAYER_PERFCTL_CC1_MODWHEEL] >= 0.0f)
               {
                  srcVal = perf_ctl_on[STSAMPLEPLAYER_PERFCTL_CC1_MODWHEEL] * (1.0f / 127.0f);
                  srcVal = (srcVal * 2.0f) - 1.0f;
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_CC2_BREATHCTL:
               if(perfCtl[STSAMPLEPLAYER_PERFCTL_CC2_BREATHCTL] >= 0.0f)
               {
                  srcVal = perfCtl[STSAMPLEPLAYER_PERFCTL_CC2_BREATHCTL] * (1.0f / 127.0f);
                  // Dyac_host_printf("xxx breathctl srcVal=%f\n", srcVal);
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_CC2_BREATHCTL_ON:
               if(perf_ctl_on[STSAMPLEPLAYER_PERFCTL_CC2_BREATHCTL] >= 0.0f)
               {
                  srcVal = perf_ctl_on[STSAMPLEPLAYER_PERFCTL_CC2_BREATHCTL] * (1.0f / 127.0f);
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_CC2_BREATHCTL_INV:
               if(perfCtl[STSAMPLEPLAYER_PERFCTL_CC2_BREATHCTL] >= 0.0f)
               {
                  srcVal = perfCtl[STSAMPLEPLAYER_PERFCTL_CC2_BREATHCTL] * (1.0f / 127.0f);
                  srcVal = 1.0f - srcVal;
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_CC2_BREATHCTL_INV_ON:
               if(perf_ctl_on[STSAMPLEPLAYER_PERFCTL_CC2_BREATHCTL] >= 0.0f)
               {
                  srcVal = perf_ctl_on[STSAMPLEPLAYER_PERFCTL_CC2_BREATHCTL] * (1.0f / 127.0f);
                  srcVal = 1.0f - srcVal;
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_CC2_BREATHCTL_BIP:
               if(perfCtl[STSAMPLEPLAYER_PERFCTL_CC2_BREATHCTL] >= 0.0f)
               {
                  srcVal = perfCtl[STSAMPLEPLAYER_PERFCTL_CC2_BREATHCTL] * (1.0f / 127.0f);
                  srcVal = (srcVal * 2.0f) - 1.0f;
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_CC2_BREATHCTL_BIP_ON:
               if(perf_ctl_on[STSAMPLEPLAYER_PERFCTL_CC2_BREATHCTL] >= 0.0f)
               {
                  srcVal = perf_ctl_on[STSAMPLEPLAYER_PERFCTL_CC2_BREATHCTL] * (1.0f / 127.0f);
                  srcVal = (srcVal * 2.0f) - 1.0f;
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_CC4_FOOTCTL:
               if(perfCtl[STSAMPLEPLAYER_PERFCTL_CC4_FOOTCTL] >= 0.0f)
               {
                  srcVal = perfCtl[STSAMPLEPLAYER_PERFCTL_CC4_FOOTCTL] * (1.0f / 127.0f);
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_CC4_FOOTCTL_ON:
               if(perf_ctl_on[STSAMPLEPLAYER_PERFCTL_CC4_FOOTCTL] >= 0.0f)
               {
                  srcVal = perf_ctl_on[STSAMPLEPLAYER_PERFCTL_CC4_FOOTCTL] * (1.0f / 127.0f);
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_CC4_FOOTCTL_INV:
               if(perfCtl[STSAMPLEPLAYER_PERFCTL_CC4_FOOTCTL] >= 0.0f)
               {
                  srcVal = perfCtl[STSAMPLEPLAYER_PERFCTL_CC4_FOOTCTL] * (1.0f / 127.0f);
                  srcVal = 1.0f - srcVal;
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_CC4_FOOTCTL_INV_ON:
               if(perf_ctl_on[STSAMPLEPLAYER_PERFCTL_CC4_FOOTCTL] >= 0.0f)
               {
                  srcVal = perf_ctl_on[STSAMPLEPLAYER_PERFCTL_CC4_FOOTCTL] * (1.0f / 127.0f);
                  srcVal = 1.0f - srcVal;
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_CC4_FOOTCTL_BIP:
               if(perfCtl[STSAMPLEPLAYER_PERFCTL_CC4_FOOTCTL] >= 0.0f)
               {
                  srcVal = perfCtl[STSAMPLEPLAYER_PERFCTL_CC4_FOOTCTL] * (1.0f / 127.0f);
                  srcVal = (srcVal * 2.0f) - 1.0f;
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_CC4_FOOTCTL_BIP_ON:
               if(perf_ctl_on[STSAMPLEPLAYER_PERFCTL_CC4_FOOTCTL] >= 0.0f)
               {
                  srcVal = perf_ctl_on[STSAMPLEPLAYER_PERFCTL_CC4_FOOTCTL] * (1.0f / 127.0f);
                  srcVal = (srcVal * 2.0f) - 1.0f;
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_CC7_VOLUME:
               if(perfCtl[STSAMPLEPLAYER_PERFCTL_CC7_VOLUME] >= 0.0f)
               {
                  srcVal = perfCtl[STSAMPLEPLAYER_PERFCTL_CC7_VOLUME] * (1.0f / 127.0f);
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_CC7_VOLUME_ON:
               if(perf_ctl_on[STSAMPLEPLAYER_PERFCTL_CC7_VOLUME] >= 0.0f)
               {
                  srcVal = perf_ctl_on[STSAMPLEPLAYER_PERFCTL_CC7_VOLUME] * (1.0f / 127.0f);
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_CC8_BALANCE:
               if(perfCtl[STSAMPLEPLAYER_PERFCTL_CC8_BALANCE] >= 0.0f)
               {
                  srcVal = perfCtl[STSAMPLEPLAYER_PERFCTL_CC8_BALANCE] * (1.0f / 127.0f);
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_CC8_BALANCE_ON:
               if(perf_ctl_on[STSAMPLEPLAYER_PERFCTL_CC8_BALANCE] >= 0.0f)
               {
                  srcVal = perf_ctl_on[STSAMPLEPLAYER_PERFCTL_CC8_BALANCE] * (1.0f / 127.0f);
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_CC10_PAN:
               if(perfCtl[STSAMPLEPLAYER_PERFCTL_CC10_PAN] >= 0.0f)
               {
                  srcVal = perfCtl[STSAMPLEPLAYER_PERFCTL_CC10_PAN] * (1.0f / 127.0f);
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_CC10_PAN_ON:
               if(perf_ctl_on[STSAMPLEPLAYER_PERFCTL_CC10_PAN] >= 0.0f)
               {
                  srcVal = perf_ctl_on[STSAMPLEPLAYER_PERFCTL_CC10_PAN] * (1.0f / 127.0f);
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_CC11_EXPR:
               if(perfCtl[STSAMPLEPLAYER_PERFCTL_CC11_EXPRESSION] >= 0.0f)
               {
                  srcVal = perfCtl[STSAMPLEPLAYER_PERFCTL_CC11_EXPRESSION] * (1.0f / 127.0f);
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_CC11_EXPR_ON:
               if(perf_ctl_on[STSAMPLEPLAYER_PERFCTL_CC11_EXPRESSION] >= 0.0f)
               {
                  srcVal = perf_ctl_on[STSAMPLEPLAYER_PERFCTL_CC11_EXPRESSION] * (1.0f / 127.0f);
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_CC11_EXPR_INV:
               if(perfCtl[STSAMPLEPLAYER_PERFCTL_CC11_EXPRESSION] >= 0.0f)
               {
                  srcVal = perfCtl[STSAMPLEPLAYER_PERFCTL_CC11_EXPRESSION] * (1.0f / 127.0f);
                  srcVal = 1.0f - srcVal;
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_CC11_EXPR_INV_ON:
               if(perf_ctl_on[STSAMPLEPLAYER_PERFCTL_CC11_EXPRESSION] >= 0.0f)
               {
                  srcVal = perf_ctl_on[STSAMPLEPLAYER_PERFCTL_CC11_EXPRESSION] * (1.0f / 127.0f);
                  srcVal = 1.0f - srcVal;
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_CC11_EXPR_BIP:
               if(perfCtl[STSAMPLEPLAYER_PERFCTL_CC11_EXPRESSION] >= 0.0f)
               {
                  srcVal = perfCtl[STSAMPLEPLAYER_PERFCTL_CC11_EXPRESSION] * (1.0f / 127.0f);
                  srcVal = (srcVal * 2.0f) - 1.0f;
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_CC11_EXPR_BIP_ON:
               if(perf_ctl_on[STSAMPLEPLAYER_PERFCTL_CC11_EXPRESSION] >= 0.0f)
               {
                  srcVal = perf_ctl_on[STSAMPLEPLAYER_PERFCTL_CC11_EXPRESSION] * (1.0f / 127.0f);
                  srcVal = (srcVal * 2.0f) - 1.0f;
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_CC16_GENERAL_1:
               if(perfCtl[STSAMPLEPLAYER_PERFCTL_CC16_GENERAL_1] >= 0.0f)
               {
                  srcVal = perfCtl[STSAMPLEPLAYER_PERFCTL_CC16_GENERAL_1] * (1.0f / 127.0f);
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_CC16_GENERAL_1_ON:
               if(perf_ctl_on[STSAMPLEPLAYER_PERFCTL_CC16_GENERAL_1] >= 0.0f)
               {
                  srcVal = perf_ctl_on[STSAMPLEPLAYER_PERFCTL_CC16_GENERAL_1] * (1.0f / 127.0f);
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_CC17_GENERAL_2:
               if(perfCtl[STSAMPLEPLAYER_PERFCTL_CC17_GENERAL_2] >= 0.0f)
               {
                  srcVal = perfCtl[STSAMPLEPLAYER_PERFCTL_CC17_GENERAL_2] * (1.0f / 127.0f);
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_CC17_GENERAL_2_ON:
               if(perf_ctl_on[STSAMPLEPLAYER_PERFCTL_CC17_GENERAL_2] >= 0.0f)
               {
                  srcVal = perf_ctl_on[STSAMPLEPLAYER_PERFCTL_CC17_GENERAL_2] * (1.0f / 127.0f);
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_CC18_GENERAL_3:
               if(perfCtl[STSAMPLEPLAYER_PERFCTL_CC18_GENERAL_3] >= 0.0f)
               {
                  srcVal = perfCtl[STSAMPLEPLAYER_PERFCTL_CC18_GENERAL_3] * (1.0f / 127.0f);
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_CC18_GENERAL_3_ON:
               if(perf_ctl_on[STSAMPLEPLAYER_PERFCTL_CC18_GENERAL_3] >= 0.0f)
               {
                  srcVal = perf_ctl_on[STSAMPLEPLAYER_PERFCTL_CC18_GENERAL_3] * (1.0f / 127.0f);
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_CC19_GENERAL_4:
               if(perfCtl[STSAMPLEPLAYER_PERFCTL_CC19_GENERAL_4] >= 0.0f)
               {
                  srcVal = perfCtl[STSAMPLEPLAYER_PERFCTL_CC19_GENERAL_4] * (1.0f / 127.0f);
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_CC19_GENERAL_4_ON:
               if(perf_ctl_on[STSAMPLEPLAYER_PERFCTL_CC19_GENERAL_4] >= 0.0f)
               {
                  srcVal = perf_ctl_on[STSAMPLEPLAYER_PERFCTL_CC19_GENERAL_4] * (1.0f / 127.0f);
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_CC74_BRIGHT:
               if(perfCtl[STSAMPLEPLAYER_PERFCTL_CC74_BRIGHTNESS] >= 0.0f)
               {
                  srcVal = perfCtl[STSAMPLEPLAYER_PERFCTL_CC74_BRIGHTNESS] * (1.0f / 127.0f);
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_CC74_BRIGHT_ON:
               if(perf_ctl_on[STSAMPLEPLAYER_PERFCTL_CC74_BRIGHTNESS] >= 0.0f)
               {
                  srcVal = perf_ctl_on[STSAMPLEPLAYER_PERFCTL_CC74_BRIGHTNESS] * (1.0f / 127.0f);
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_CC80_GENERAL_5:
               if(perfCtl[STSAMPLEPLAYER_PERFCTL_CC80_GENERAL_5] >= 0.0f)
               {
                  srcVal = perfCtl[STSAMPLEPLAYER_PERFCTL_CC80_GENERAL_5] * (1.0f / 127.0f);
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_CC80_GENERAL_5_ON:
               if(perf_ctl_on[STSAMPLEPLAYER_PERFCTL_CC80_GENERAL_5] >= 0.0f)
               {
                  srcVal = perf_ctl_on[STSAMPLEPLAYER_PERFCTL_CC80_GENERAL_5] * (1.0f / 127.0f);
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_CC81_GENERAL_6:
               if(perfCtl[STSAMPLEPLAYER_PERFCTL_CC81_GENERAL_6] >= 0.0f)
               {
                  srcVal = perfCtl[STSAMPLEPLAYER_PERFCTL_CC81_GENERAL_6] * (1.0f / 127.0f);
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_CC81_GENERAL_6_ON:
               if(perf_ctl_on[STSAMPLEPLAYER_PERFCTL_CC81_GENERAL_6] >= 0.0f)
               {
                  srcVal = perf_ctl_on[STSAMPLEPLAYER_PERFCTL_CC81_GENERAL_6] * (1.0f / 127.0f);
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_CC82_GENERAL_7:
               if(perfCtl[STSAMPLEPLAYER_PERFCTL_CC82_GENERAL_7] >= 0.0f)
               {
                  srcVal = perfCtl[STSAMPLEPLAYER_PERFCTL_CC82_GENERAL_7] * (1.0f / 127.0f);
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_CC82_GENERAL_7_ON:
               if(perf_ctl_on[STSAMPLEPLAYER_PERFCTL_CC82_GENERAL_7] >= 0.0f)
               {
                  srcVal = perf_ctl_on[STSAMPLEPLAYER_PERFCTL_CC82_GENERAL_7] * (1.0f / 127.0f);
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_CC83_GENERAL_8:
               if(perfCtl[STSAMPLEPLAYER_PERFCTL_CC83_GENERAL_8] >= 0.0f)
               {
                  srcVal = perfCtl[STSAMPLEPLAYER_PERFCTL_CC83_GENERAL_8] * (1.0f / 127.0f);
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_CC83_GENERAL_8_ON:
               if(perf_ctl_on[STSAMPLEPLAYER_PERFCTL_CC83_GENERAL_8] >= 0.0f)
               {
                  srcVal = perf_ctl_on[STSAMPLEPLAYER_PERFCTL_CC83_GENERAL_8] * (1.0f / 127.0f);
               }
               else
               {
                  bSrcValid = YAC_FALSE;
               }
               break;

            case STSAMPLE_MM_SRC_LFO_FREQ:
               srcVal = mmsrc.lfo.freq * mmdst.lfo_freq_level;
               break;

            case STSAMPLE_MM_SRC_LFO_VOL:
               srcVal = mmsrc.lfo.vol * mmdst.lfo_vol_level;
               break;

            case STSAMPLE_MM_SRC_LFO_PAN:
               srcVal = mmsrc.lfo.pan * mmdst.lfo_pan_level;
               break;

            case STSAMPLE_MM_SRC_LFO_AUX:
               srcVal = mmsrc.lfo.aux * mmdst.lfo_aux_level;
               break;

            case STSAMPLE_MM_SRC_ENV_FREQ:
               srcVal = mmsrc.env.freq * mmdst.env_freq_level;
               break;

            case STSAMPLE_MM_SRC_ENV_VOL:
               srcVal = mmsrc.env.vol * mmdst.env_vol_level;
               break;

            case STSAMPLE_MM_SRC_ENV_PAN:
               srcVal = mmsrc.env.pan * mmdst.env_pan_level;
               break;

            case STSAMPLE_MM_SRC_ENV_AUX:
               srcVal = mmsrc.env.aux * mmdst.env_aux_level;
               break;

            case STSAMPLE_MM_SRC_MODSEQ1:
               srcVal = mmsrc.modseq[0] * mmdst.modseq_level[0];
               break;

            case STSAMPLE_MM_SRC_MODSEQ2:
               srcVal = mmsrc.modseq[1] * mmdst.modseq_level[1];
               break;

            case STSAMPLE_MM_SRC_MODSEQ3:
               srcVal = mmsrc.modseq[2] * mmdst.modseq_level[2];
               break;

            case STSAMPLE_MM_SRC_MODSEQ4:
               srcVal = mmsrc.modseq[3] * mmdst.modseq_level[3];
               break;

            case STSAMPLE_MM_SRC_MODSEQ1_BY_VOICEKEY:
               if(NULL != sample->modseq[0][modseq_patch[0]])
                  srcVal = sample->modseq[0][modseq_patch[0]]->readModuloIndexedValue(voice_alloc_idx);
               else
                  srcVal = 0.0f;
               break;

            case STSAMPLE_MM_SRC_MODSEQ2_BY_VOICEKEY:
               if(NULL != sample->modseq[1][modseq_patch[1]])
                  srcVal = sample->modseq[1][modseq_patch[1]]->readModuloIndexedValue(voice_alloc_idx);
               else
                  srcVal = 0.0f;
               break;

            case STSAMPLE_MM_SRC_MODSEQ3_BY_VOICEKEY:
               if(NULL != sample->modseq[2][modseq_patch[2]])
                  srcVal = sample->modseq[2][modseq_patch[2]]->readModuloIndexedValue(voice_alloc_idx);
               else
                  srcVal = 0.0f;
               break;

            case STSAMPLE_MM_SRC_MODSEQ4_BY_VOICEKEY:
               if(NULL != sample->modseq[3][modseq_patch[3]])
                  srcVal = sample->modseq[3][modseq_patch[3]]->readModuloIndexedValue(voice_alloc_idx);
               else
                  srcVal = 0.0f;
               break;

            case STSAMPLE_MM_SRC_MODSEQ4_STEP_1:
            case STSAMPLE_MM_SRC_MODSEQ4_STEP_2:
            case STSAMPLE_MM_SRC_MODSEQ4_STEP_3:
            case STSAMPLE_MM_SRC_MODSEQ4_STEP_4:
            case STSAMPLE_MM_SRC_MODSEQ4_STEP_5:
            case STSAMPLE_MM_SRC_MODSEQ4_STEP_6:
            case STSAMPLE_MM_SRC_MODSEQ4_STEP_7:
            case STSAMPLE_MM_SRC_MODSEQ4_STEP_8:
            case STSAMPLE_MM_SRC_MODSEQ4_STEP_9:
            case STSAMPLE_MM_SRC_MODSEQ4_STEP_10:
            case STSAMPLE_MM_SRC_MODSEQ4_STEP_11:
            case STSAMPLE_MM_SRC_MODSEQ4_STEP_12:
            case STSAMPLE_MM_SRC_MODSEQ4_STEP_13:
            case STSAMPLE_MM_SRC_MODSEQ4_STEP_14:
            case STSAMPLE_MM_SRC_MODSEQ4_STEP_15:
            case STSAMPLE_MM_SRC_MODSEQ4_STEP_16:
               if(NULL != sample->modseq[3][modseq_patch[3]])
                  srcVal = sample->modseq[3][modseq_patch[3]]->readIndexedValue(mm->src - STSAMPLE_MM_SRC_MODSEQ4_STEP_1);
               else
                  srcVal = 0.0f;
               break;

            case STSAMPLE_MM_SRC_REG1:
               srcVal = mmdst_reg[0];
               break;

            case STSAMPLE_MM_SRC_REG2:
               srcVal = mmdst_reg[1];
               break;

            case STSAMPLE_MM_SRC_REG3:
               srcVal = mmdst_reg[2];
               break;

            case STSAMPLE_MM_SRC_REG4:
               srcVal = mmdst_reg[3];
               break;

            case STSAMPLE_MM_SRC_CC:
               srcVal = mmdst_cc;
               break;

            case STSAMPLE_MM_SRC_VOICE_CALIBRATION_1:
            case STSAMPLE_MM_SRC_VOICE_CALIBRATION_2:
            case STSAMPLE_MM_SRC_VOICE_CALIBRATION_3:
            case STSAMPLE_MM_SRC_VOICE_CALIBRATION_4:
            case STSAMPLE_MM_SRC_VOICE_CALIBRATION_5:
            case STSAMPLE_MM_SRC_VOICE_CALIBRATION_6:
               {
                  sUI laneIdx = (mm->src - STSAMPLE_MM_SRC_VOICE_CALIBRATION_1) >> 1;
                  if(sample->voice_calibration_enable[laneIdx])
                  {
                     srcVal = sample->voice_calibration
                        [laneIdx]
                        [voice_alloc_idx % sample->voice_calibration_modulo[laneIdx]];
                  }
               }
               break;

            case STSAMPLE_MM_SRC_VOICE_CALIBRATION_1_R:
            case STSAMPLE_MM_SRC_VOICE_CALIBRATION_2_R:
            case STSAMPLE_MM_SRC_VOICE_CALIBRATION_3_R:
            case STSAMPLE_MM_SRC_VOICE_CALIBRATION_4_R:
            case STSAMPLE_MM_SRC_VOICE_CALIBRATION_5_R:
            case STSAMPLE_MM_SRC_VOICE_CALIBRATION_6_R:
               {
                  // (note) voice_alloc_idx *3 / *5 / *7 can also yield interesting results (e.g. *5 % 8) but
                  //         with mismatching mul/mod settings all indices become 0
                  //         => stick to fwd / bwd play modes
                  sUI laneIdx = (mm->src - STSAMPLE_MM_SRC_VOICE_CALIBRATION_1_R) >> 1;
                  if(sample->voice_calibration_enable[laneIdx])
                  {
                     srcVal = sample->voice_calibration
                        [laneIdx]
                        [sample->voice_calibration_modulo[laneIdx] - 1u - (voice_alloc_idx % sample->voice_calibration_modulo[laneIdx])];
                  }
               }
               break;

            case STSAMPLE_MM_SRC_PLAYPOS_Z1:
            case STSAMPLE_MM_SRC_PLAYPOS_Z2:
            case STSAMPLE_MM_SRC_PLAYPOS_Z3:
            case STSAMPLE_MM_SRC_PLAYPOS_Z4:
            {
               if(NULL != sample->parent_samplebank)
               {
                  sUI zoneIdx = mm->src - STSAMPLE_MM_SRC_PLAYPOS_Z1;
                  StSample *s = (StSample*)sample->parent_samplebank->_getSampleByIdx(zoneIdx);
                  if(NULL != s)
                  {
                     StSampleVoice *v = sample_player->findVoiceByKeyAndSample(voice_key, s);
                     if(NULL != v)
                     {
                        srcVal = (sF32)((v->current_sample_offset + v->current_play_offset) / sample->waveform->_getNumFrames());
                        // yac_host->printf("xxx zoneIdx=%u => s=%p => v=%p => srcVal=%f\n", s, v, srcVal);
                     }
                  }
               }
               break;
            }

            case STSAMPLE_MM_SRC_GLIDE_TIME:
               // yac_host->printf("xxx MM_SRC_GLIDE_TIME: glide_time=%f\n", glide_time);
               srcVal = glide_time;  // 0..1
               break;

            case STSAMPLE_MM_SRC_GLIDE_TIME_INV:
               srcVal = 1.0f - glide_time;  // 0..1
               break;

            case STSAMPLE_MM_SRC_GLIDE_TIME_TRI:
               srcVal = glide_time * 2.0f;
               if(glide_time >= 0.5f)
                  srcVal = 2.0f - srcVal;
               break;

            case STSAMPLE_MM_SRC_GLIDE_TIME_TRI_INV:
               srcVal = glide_time * 2.0f;
               if(glide_time >= 0.5f)
                  srcVal = 2.0f - srcVal;
               srcVal = 1.0f - srcVal;
               break;

            case STSAMPLE_MM_SRC_VOICE_BUS_1:
            case STSAMPLE_MM_SRC_VOICE_BUS_1_ABS:
            case STSAMPLE_MM_SRC_VOICE_BUS_2:
            case STSAMPLE_MM_SRC_VOICE_BUS_2_ABS:
            case STSAMPLE_MM_SRC_VOICE_BUS_3:
            case STSAMPLE_MM_SRC_VOICE_BUS_3_ABS:
            case STSAMPLE_MM_SRC_VOICE_BUS_4:
            case STSAMPLE_MM_SRC_VOICE_BUS_4_ABS:
            case STSAMPLE_MM_SRC_VOICE_BUS_5:
            case STSAMPLE_MM_SRC_VOICE_BUS_5_ABS:
            case STSAMPLE_MM_SRC_VOICE_BUS_6:
            case STSAMPLE_MM_SRC_VOICE_BUS_6_ABS:
            case STSAMPLE_MM_SRC_VOICE_BUS_7:
            case STSAMPLE_MM_SRC_VOICE_BUS_7_ABS:
            case STSAMPLE_MM_SRC_VOICE_BUS_8:
            case STSAMPLE_MM_SRC_VOICE_BUS_8_ABS:
               if(NULL != voice_bus_buffers)
               {
                  sUI busIdx = mm->src - STSAMPLE_MM_SRC_VOICE_BUS_1;
                  const sF32 *busL = voice_bus_buffers[busIdx >> 1];
                  if(NULL != busL)
                  {
                     srcVal = *busL;
                     if( (busIdx & 1u) && (srcVal < 0.0f) )  // ABS
                        srcVal = -srcVal;
                  }
               }
               break;
         }

         if(bSrcValid)
         {
            // Apply slew
            sF32 oldVal = (sF32)modmatrix_slewed_src_values[mmIdx];
            if(srcVal > oldVal)
            {
               sF64 amt = 1.0 - mm->slew_up;
               amt *= amt;
               amt *= amt;
               modmatrix_slewed_src_values[mmIdx] = oldVal + (srcVal - oldVal) * amt;
               srcVal = (sF32)modmatrix_slewed_src_values[mmIdx];
            }
            else if(srcVal < oldVal)
            {
               sF64 amt = 1.0 - mm->slew_down;
               amt *= amt;
               amt *= amt;
               // amt *= amt * amt;
               // amt *= amt * amt;
               modmatrix_slewed_src_values[mmIdx] = oldVal + (srcVal - oldVal) * amt;
               srcVal = (sF32)modmatrix_slewed_src_values[mmIdx];
            }
            else
               modmatrix_slewed_src_values[mmIdx] = srcVal;


            if(0.0f != mm->curve)
            {
               sSI curveOff = sUI(512 + sSI(mm->curve * 512));
               if(curveOff < 0)
                  curveOff = 0;
               else if(curveOff >= (32*32))
                  curveOff = (32*32) - 1;
               curveOff *= 256;
               if(srcVal >= 0.0f)
               {
                  // positive
                  sF32 idxF = srcVal * srcValCurveRangeScale * 255.0f;
                  if(idxF < 255.0f)
                  {
                     sF32 fracF = idxF - sSI(idxF);
                     sF32 a = mm_curve_lut[curveOff + sUI(idxF)];
                     sF32 b = mm_curve_lut[curveOff + sUI(idxF) + 1u];
                     // if(STSAMPLE_MM_DST_VOLUME == mm->dst)
                     // {
                     //    yac_host->printf("MM_DST_VOLUME + curveOff=%u srcVal=%f idx=%u fracF=%f a=%f b=%f\n", curveOff/256u, srcVal, sUI(idxF), fracF, a, b);
                     // }
                     srcVal = a + (b - a) * fracF;
                     srcVal /= srcValCurveRangeScale;
                  }
               }
               else
               {
                  // negative
                  sF32 idxF = srcVal * srcValCurveRangeScale * -255.0f;
                  if(idxF < 255.0f)
                  {
                     sF32 fracF = idxF - sSI(idxF);
                     sF32 a = mm_curve_lut[curveOff + sUI(idxF)];
                     sF32 b = mm_curve_lut[curveOff + sUI(idxF)  + 1u];
                     // if(STSAMPLE_MM_DST_VOLUME == mm->dst)
                     // {
                     //    yac_host->printf("MM_DST_VOLUME - curveOff=%u srcVal=%f idx=%u fracF=%f a=%f b=%f\n", curveOff/256u, srcVal, sUI(idxF), fracF, a, b);
                     // }
                     srcVal = a + (b - a) * fracF;
                     srcVal = -srcVal;
                     srcVal /= srcValCurveRangeScale;
                  }
               }
               // Dyac_host_printf("  => srcVal=%f\n", srcVal);
            }
            sF32 mmAmt = mm->amt + modmatrix_amt[mmIdx];

            if(sample->b_mmvar_enable)
            {
               sUI mmVarIdx;
               if(sample->b_mmvar_force_edit_idx)
               {
                  mmVarIdx = sample->mmvar_edit_idx;
                  if(mmVarIdx > STSAMPLE_MMVAR_NUM)
                     mmVarIdx = STSAMPLE_MMVAR_NUM - 1u;  // should not be reachable
                  mmAmt += sample->mmvar_data[STSAMPLE_NUM_MODMATRIX_ENTRIES * mmVarIdx + mmIdx];
                  mmvar_last_seen_var_idx = mmVarIdx;
               }
               else
               {
                  sF32 mmVarIdxF = mmdst_var * sample->mmvar_num;
                  if(mmVarIdxF < 0.0f)
                     mmVarIdxF = 0.0f;
                  else if(mmVarIdxF > sF32(sample->mmvar_num))
                     mmVarIdxF = sF32(sample->mmvar_num);

                  mmVarIdx = sUI(mmVarIdxF);
                  
                  if(sample->b_mmvar_smooth)
                  {
                     sUI mmVarIdxB = mmVarIdx + 1u;
                     if(YAC_TRUE == sample->b_mmvar_smooth)
                     {
                        // Clamp to last

                        if(mmVarIdx >= sample->mmvar_num)
                           mmVarIdx = sample->mmvar_num - 1u;

                        if(mmVarIdxB >= sample->mmvar_num)
                           mmVarIdxB = sample->mmvar_num - 1u;
                     }
                     else
                     {
                        if(mmVarIdx >= sample->mmvar_num)
                           mmVarIdx = 0u;  // mmVarIdxF==1.0

                        // Wrap-around (lerp last back to first)
                        if(mmVarIdxB >= sample->mmvar_num)
                           mmVarIdxB = 0u;
                     }
                     const sF32 mmVarFrac = mmVarIdxF - sF32(mmVarIdx);
                     sF32 mmVarDelta = sample->mmvar_data[STSAMPLE_NUM_MODMATRIX_ENTRIES * mmVarIdx + mmIdx];
                     mmVarDelta += (sample->mmvar_data[STSAMPLE_NUM_MODMATRIX_ENTRIES * mmVarIdxB + mmIdx] - mmVarDelta) * mmVarFrac;
                     mmAmt += mmVarDelta;
                     mmvar_last_seen_var_idx = (mmVarDelta >= 0.5f) ? mmVarIdxB : mmVarIdx;
                  }
                  else
                  {
                     if(mmVarIdx >= sample->mmvar_num)
                        mmVarIdx = sample->mmvar_num - 1u;
                     mmAmt += sample->mmvar_data[STSAMPLE_NUM_MODMATRIX_ENTRIES * mmVarIdx + mmIdx];
                     mmvar_last_seen_var_idx = mmVarIdx;
                  }
               }
            } // if sample->b_mmvar_enable

            sF32 srcValDef = srcVal * mmAmt;

            sBool bAutoAdd = ((STSAMPLE_MM_OP_AUTO == mm->op) || (STSAMPLE_MM_OP_ADD == mm->op));
            sBool bAutoMul = ((STSAMPLE_MM_OP_AUTO == mm->op) || (STSAMPLE_MM_OP_MUL == mm->op));
            sBool bAutoRep = ((STSAMPLE_MM_OP_AUTO == mm->op) || (STSAMPLE_MM_OP_REPLACE == mm->op));

#define Delse_mm_lerp(d)                                                \
         else if(STSAMPLE_MM_OP_BLEND_SRC == mm->op)                    \
            d = (d) + (srcVal - (d)) * mmAmt;                           \
         else if(STSAMPLE_MM_OP_BLEND_AMT == mm->op)                    \
            d = (d) + (mmAmt - (d)) * srcVal;                           \
         else if(STSAMPLE_MM_OP_REPLACE == mm->op)                      \
            d = srcValDef;                                              \
         else if(STSAMPLE_MM_OP_REPLACE_INIT == mm->op)                 \
         { if(0u == replay_ticks) { d = srcValDef; } }                  \
         else if(STSAMPLE_MM_OP_COMPARE == mm->op)                      \
         { mmdst_cc = Dfltequal(srcVal, (d)) ? 0.0f : (srcVal > (d)) ? mmAmt : -mmAmt; (d) = srcVal; } \
         else if(STSAMPLE_MM_OP_ZEROCROSSING == mm->op)                 \
         { mmdst_cc = ((srcVal <= 0.0f) ^ ((d) <= 0.0f)) ? mmAmt : 0.0f; (d) = srcVal; } \
         else if(STSAMPLE_MM_OP_MODULO == mm->op)                       \
         { (d) = (0.0f != (d)) ? (srcValDef - (d) * floorf(srcValDef/(d))) : 0.0f; } \
         else if(STSAMPLE_MM_OP_TRI == mm->op)                          \
         {                                                              \
            if((d) > 0.0f)                                              \
            {                                                           \
               sF32 t = (d)+(d);                                        \
               sF32 m = (srcValDef - t * floorf(srcValDef/t));          \
               if(m >= (d))                                             \
               {                                                        \
                  m = t - m;                                            \
               }                                                        \
               (d) = m;                                                 \
            }                                                           \
            else                                                        \
               (d) = 0.0f;                                              \
         }                                                              \
         else if(STSAMPLE_MM_OP_TRI_NR == mm->op)                       \
         {                                                              \
            if((d) >= 2.0f)                                             \
            {                                                           \
               sF32 t = (d)+(d) - 2.0f;                                 \
               sF32 m = (srcValDef - t * floorf(srcValDef/t));          \
               if(m >= (d))                                             \
               {                                                        \
                  m = (d) - (m - (d)) - 2.0f;                           \
               }                                                        \
               (d) = m;                                                 \
            }                                                           \
            else                                                        \
               (d) = 0.0f;                                              \
         }                                                              \
         else if(STSAMPLE_MM_OP_STEP == mm->op)                         \
            (d) = ((d) != 0.0f) ? (sSI(srcValDef * (d) + ((srcValDef >= 0.0f) ? 0.5f : -0.5f)) / (d)) : 0.0f

#define Delse_mm_lerp_bipolar8(d)                                       \
         else if(STSAMPLE_MM_OP_BLEND_SRC == mm->op)                    \
            d = (d) + (stsamplevoice_bipolar_to_scale(srcVal, 8.0f/*div*/, 8.0f/*mul*/) - (d)) * mmAmt; \
         else if(STSAMPLE_MM_OP_BLEND_AMT == mm->op)                    \
            d = (d) + (stsamplevoice_bipolar_to_scale(mmAmt, 8.0f/*div*/, 8.0f/*mul*/) - (d)) * srcVal; \
         else if(STSAMPLE_MM_OP_REPLACE == mm->op)                      \
            d = srcValDef;                                              \
         else if(STSAMPLE_MM_OP_REPLACE_INIT == mm->op)                 \
         { if(0u == replay_ticks) { d = srcValDef; } }                  \
         else if(STSAMPLE_MM_OP_COMPARE == mm->op)                      \
         { mmdst_cc = Dfltequal(srcVal, (d)) ? 0.0f : (srcVal > (d)) ? mmAmt : -mmAmt; (d) = srcVal; } \
         else if(STSAMPLE_MM_OP_ZEROCROSSING == mm->op)                 \
         { mmdst_cc = ((srcVal <= 0.0f) ^ ((d) <= 0.0f)) ? mmAmt : 0.0f; (d) = srcVal; } \
         else if(STSAMPLE_MM_OP_MODULO == mm->op)                       \
         { (d) = (0.0f != (d)) ? (srcValDef - (d) * floorf(srcValDef/(d))) : 0.0f; } \
         else if(STSAMPLE_MM_OP_TRI == mm->op)                          \
         {                                                              \
            if((d) > 0.0f)                                              \
            {                                                           \
               sF32 t = (d)+(d);                                        \
               sF32 m = (srcValDef - t * floorf(srcValDef/t));          \
               if(m >= (d))                                             \
               {                                                        \
                  m = t - m;                                            \
               }                                                        \
               (d) = m;                                                 \
            }                                                           \
            else                                                        \
               (d) = 0.0f;                                              \
         }                                                              \
         else if(STSAMPLE_MM_OP_TRI_NR == mm->op)                       \
         {                                                              \
            if((d) >= 2.0f)                                             \
            {                                                           \
               sF32 t = (d)+(d) - 2.0f;                                 \
               sF32 m = (srcValDef - t * floorf(srcValDef/t));          \
               if(m >= (d))                                             \
               {                                                        \
                  m = (d) - (m - (d)) - 2.0f;                           \
               }                                                        \
               (d) = m;                                                 \
            }                                                           \
            else                                                        \
               (d) = 0.0f;                                              \
         }                                                              \
         else if(STSAMPLE_MM_OP_STEP == mm->op)                         \
            (d) = ((d) != 0.0f) ? (sSI(srcValDef * (d) + ((srcValDef >= 0.0f) ? 0.5f : -0.5f)) / (d)) : 0.0f

#define Delse_mm_lerp_scl(d, s)                                         \
         else if(STSAMPLE_MM_OP_BLEND_SRC == mm->op)                    \
            d = (d) + (srcVal*(s) - (d)) * mmAmt;                       \
         else if(STSAMPLE_MM_OP_BLEND_AMT == mm->op)                    \
            d = (d) + (mmAmt*(s) - (d)) * srcVal;                       \
         else if(STSAMPLE_MM_OP_REPLACE == mm->op)                      \
            d = srcValDef;                                              \
         else if(STSAMPLE_MM_OP_REPLACE_INIT == mm->op)                 \
         { if(0u == replay_ticks) { d = srcValDef; } } \
         else if(STSAMPLE_MM_OP_COMPARE == mm->op)                      \
         { mmdst_cc = Dfltequal(srcVal, (d)) ? 0.0f : (srcVal > (d)) ? mmAmt : -mmAmt; (d) = srcVal; } \
         else if(STSAMPLE_MM_OP_ZEROCROSSING == mm->op)                 \
         { mmdst_cc = ((srcVal <= 0.0f) ^ ((d) <= 0.0f)) ? mmAmt : 0.0f; (d) = srcVal; } \
         else if(STSAMPLE_MM_OP_MODULO == mm->op)                       \
         { (d) = (0.0f != (d)) ? (srcValDef - (d) * floorf(srcValDef/(d))) : 0.0f; } \
         else if(STSAMPLE_MM_OP_TRI == mm->op)                          \
         {                                                              \
            if((d) > 0.0f)                                              \
            {                                                           \
               sF32 t = (d)+(d);                                        \
               sF32 m = (srcValDef - t * floorf(srcValDef/t));          \
               if(m >= (d))                                             \
               {                                                        \
                  m = t - m;                                            \
               }                                                        \
               (d) = m;                                                 \
            }                                                           \
            else                                                        \
               (d) = 0.0f;                                              \
         }                                                              \
         else if(STSAMPLE_MM_OP_TRI_NR == mm->op)                       \
         {                                                              \
            if((d) >= 2.0f)                                             \
            {                                                           \
               sF32 t = (d)+(d) - 2.0f;                                 \
               sF32 m = (srcValDef - t * floorf(srcValDef/t));          \
               if(m >= (d))                                             \
               {                                                        \
                  m = (d) - (m - (d)) - 2.0f;                           \
               }                                                        \
               (d) = m;                                                 \
            }                                                           \
            else                                                        \
               (d) = 0.0f;                                              \
         }                                                              \
         else if(STSAMPLE_MM_OP_STEP == mm->op)                         \
            (d) = ((d) != 0.0f) ? (sSI(srcValDef * (d) + ((srcValDef >= 0.0f) ? 0.5f : -0.5f)) / (d)) : 0.0f

            switch(mm->dst)
            {
               default:
               case STSAMPLE_MM_DST_NONE:
               // (todo) mm dst
               break;

               case STSAMPLE_MM_DST_FREQ:
               if(bAutoAdd)
                  mmdst.freq += srcValDef;
               else if(bAutoMul)
                  mmdst.freq *= srcValDef;
               Delse_mm_lerp(mmdst.freq);
               Dsignaltap(mmdst.freq);
               break;

               case STSAMPLE_MM_DST_FREQ_OCT:
               if(bAutoAdd)
                  mmdst.freq_oct += srcValDef;
               else if(bAutoMul)
                  mmdst.freq_oct *= srcValDef;
               Delse_mm_lerp(mmdst.freq_oct);
               Dsignaltap(mmdst.freq_oct);
               break;

               case STSAMPLE_MM_DST_FREQ_FIFTH:
               if(bAutoAdd)
                  mmdst.freq_fifth += srcValDef;
               else if(bAutoMul)
                  mmdst.freq_fifth *= srcValDef;
               Delse_mm_lerp(mmdst.freq_fifth);
               Dsignaltap(mmdst.freq_fifth);
               break;

               case STSAMPLE_MM_DST_FREQ_FOURTH:
               if(bAutoAdd)
                  mmdst.freq_fourth += srcValDef;
               else if(bAutoMul)
                  mmdst.freq_fourth *= srcValDef;
               Delse_mm_lerp(mmdst.freq_fourth);
               Dsignaltap(mmdst.freq_fourth);
               break;

               case STSAMPLE_MM_DST_FREQ_SEMI:
               if(bAutoAdd)
                  mmdst.freq_semi += srcValDef;
               else if(bAutoMul)
                  mmdst.freq_semi *= srcValDef;
               Delse_mm_lerp(mmdst.freq_semi);
               Dsignaltap(mmdst.freq_semi);
               break;

               case STSAMPLE_MM_DST_FREQ_FINE:
               if(bAutoAdd)
                  mmdst.freq_fine += srcValDef;
               else if(bAutoMul)
                  mmdst.freq_fine *= srcValDef;
               Delse_mm_lerp(mmdst.freq_fine);
               Dsignaltap(mmdst.freq_fine);
               break;

               case STSAMPLE_MM_DST_FREQ_BEAT:
               if(bAutoAdd)
                  mmdst.freq_beat += srcValDef * 100.0f;
               else if(bAutoMul)
                  mmdst.freq_beat *= srcValDef;
               Delse_mm_lerp(mmdst.freq_beat);
               Dsignaltap(mmdst.freq_beat);
               break;

               case STSAMPLE_MM_DST_TUNING_TBL_ABS:
               if(bAutoAdd)
                  mmdst.tuning_tbl_abs += srcValDef;
               else if(bAutoMul)
                  mmdst.tuning_tbl_abs *= srcValDef;
               Delse_mm_lerp(mmdst.tuning_tbl_abs);
               Dsignaltap(mmdst.tuning_tbl_abs);
               break;

               case STSAMPLE_MM_DST_TUNING_TBL_REL:
               // 0..15 selects interpolated table 1..16
               if(bAutoRep)
                  mmdst.tuning_tbl_rel = srcValDef;
               else if(bAutoAdd)
                  mmdst.tuning_tbl_rel += srcValDef;
               else if(bAutoMul)
                  mmdst.tuning_tbl_rel *= srcValDef;
               Delse_mm_lerp(mmdst.tuning_tbl_rel);
               Dsignaltap(mmdst.tuning_tbl_rel);
               break;

               case STSAMPLE_MM_DST_NOTE_2:
               if(bAutoAdd)
                  mmdst.note_2 += srcValDef;
               else if(bAutoMul)
                  mmdst.note_2 *= srcValDef;
               Delse_mm_lerp(mmdst.note_2);
               Dsignaltap(mmdst.note_2);
               break;

               case STSAMPLE_MM_DST_FREQ_2:
               if(bAutoAdd)
                  mmdst.freq_2 += srcValDef;
               else if(bAutoMul)
                  mmdst.freq_2 *= srcValDef;
               Delse_mm_lerp(mmdst.freq_2);
               Dsignaltap(mmdst.freq_2);
               break;

               case STSAMPLE_MM_DST_VOLUME:
               if(bAutoAdd)
                  mmdst.vol += srcValDef;
               else if(bAutoMul)
                  mmdst.vol *= srcValDef;
               Delse_mm_lerp(mmdst.vol);
               Dsignaltap(mmdst.vol);
               break;

               case STSAMPLE_MM_DST_PAN:
               if(bAutoAdd)
                  mmdst.pan += srcValDef;
               else if(bAutoMul)
                  mmdst.pan *= srcValDef;
               Delse_mm_lerp(mmdst.pan);
               Dsignaltap(mmdst.pan);
               break;

               case STSAMPLE_MM_DST_FILTER_CUTOFF:
               if(bAutoAdd)
                  mmdst.flt_cutoff += srcValDef;
               else if(bAutoMul)
                  mmdst.flt_cutoff *= srcValDef;
               Delse_mm_lerp(mmdst.flt_cutoff);
               Dsignaltap(mmdst.flt_cutoff);
               break;

               case STSAMPLE_MM_DST_FILTER_PAN:
               if(bAutoAdd)
                  mmdst.flt_pan += srcValDef;
               else if(bAutoMul)
                  mmdst.flt_pan *= srcValDef;
               Delse_mm_lerp(mmdst.flt_pan);
               Dsignaltap(mmdst.flt_pan);
               break;

               case STSAMPLE_MM_DST_FILTER_OFFSET:
               if(bAutoAdd)
                  mmdst.flt_offset += srcValDef;
               else if(bAutoMul)
                  mmdst.flt_offset *= srcValDef;
               Delse_mm_lerp(mmdst.flt_offset);
               Dsignaltap(mmdst.flt_offset);
               break;

               case STSAMPLE_MM_DST_FILTER_RESONANCE:
               if(bAutoAdd)
                  mmdst.flt_res += srcValDef;
               else if(bAutoMul)
                  mmdst.flt_res *= srcValDef;
               Delse_mm_lerp(mmdst.flt_res);
               Dsignaltap(mmdst.flt_res);
               break;

               case STSAMPLE_MM_DST_SAMPLE_OFFSET:
               if(bAutoAdd)
                  mmdst_sampleoff += srcValDef;
               else if(bAutoMul)
                  mmdst_sampleoff *= srcValDef;
               Delse_mm_lerp(mmdst_sampleoff);
               Dsignaltap(mmdst_sampleoff);
               break;

               case STSAMPLE_MM_DST_SAMPLE_SHIFT:
               if(bAutoAdd)
                  mmdst_sampleshift += srcValDef;
               else if(bAutoMul)
                  mmdst_sampleshift *= srcValDef;
               Delse_mm_lerp(mmdst_sampleshift);
               Dsignaltap(mmdst_sampleshift);
               break;

               case STSAMPLE_MM_DST_SAMPLE_SHIFT_EOL:
               if(bAutoAdd)
                  mmdst.sampleshift_endofloop += srcValDef;
               else if(bAutoMul)
                  mmdst.sampleshift_endofloop *= srcValDef;
               Delse_mm_lerp(mmdst.sampleshift_endofloop);
               mmdst.b_sampleshift_endofloop_valid = YAC_TRUE;
               Dsignaltap(mmdst.sampleshift_endofloop);
               break;

               case STSAMPLE_MM_DST_LOOP_RESTART:
               if(bAutoAdd)
                  mmdst.looprestart += srcValDef;
               else if(bAutoMul)
                  mmdst.looprestart *= srcValDef;
               Delse_mm_lerp(mmdst.looprestart);
               Dsignaltap(mmdst.looprestart);
               break;

               case STSAMPLE_MM_DST_CYCLE_LEN:
               if(bAutoMul)
                  mmdst_cyclelen *= srcValDef;
               else if(bAutoAdd)
                  mmdst_cyclelen += srcValDef;
               Delse_mm_lerp(mmdst_cyclelen);
               Dsignaltap(mmdst_cyclelen);
               break;

               case STSAMPLE_MM_DST_CYCLE_LEN_EOL:
               b_mmdst_cyclelen_eol_valid = YAC_TRUE;
               if(bAutoMul)
                  mmdst_cyclelen_eol *= srcValDef;
               else if(bAutoAdd)
                  mmdst_cyclelen_eol += srcValDef;
               Delse_mm_lerp(mmdst_cyclelen_eol);
               Dsignaltap(mmdst_cyclelen_eol);
               break;

               case STSAMPLE_MM_DST_CYCLE_LEN_SAMPLE_OFFSET:
               if(bAutoAdd)
                  mmdst_cyclelen_sampleoff += srcValDef;
               else if(bAutoMul)
                  mmdst_cyclelen_sampleoff *= srcValDef;
               Delse_mm_lerp(mmdst_cyclelen_sampleoff);
               Dsignaltap(mmdst_cyclelen_sampleoff);
               break;

               case STSAMPLE_MM_DST_WT_2D_X_ABS:
               srcValDef = (sample->timestretch_2d_w > 0u) ? (srcValDef / sample->timestretch_2d_w) : 0.0f;
               if(bAutoAdd)
                  mmdst.wt2d_x += srcValDef;
               else if(bAutoMul)
                  mmdst.wt2d_x *= srcValDef;
               Delse_mm_lerp(mmdst.wt2d_x);
               Dsignaltap(mmdst.wt2d_x);
               mmdst.b_wt2d_x_valid = YAC_TRUE;
               break;

               case STSAMPLE_MM_DST_WT_2D_X_REL:
               if(bAutoAdd)
                  mmdst.wt2d_x += srcValDef;
               else if(bAutoMul)
                  mmdst.wt2d_x *= srcValDef;
               Delse_mm_lerp(mmdst.wt2d_x);
               Dsignaltap(mmdst.wt2d_x);
               mmdst.b_wt2d_x_valid = YAC_TRUE;
               break;

               case STSAMPLE_MM_DST_WT_2D_Y_ABS:
               srcValDef = (sample->timestretch_2d_h > 0u) ? (srcValDef / sample->timestretch_2d_h) : 0.0f;
               if(bAutoAdd)
                  mmdst.wt2d_y += srcValDef;
               else if(bAutoMul)
                  mmdst.wt2d_y *= srcValDef;
               Delse_mm_lerp(mmdst.wt2d_y);
               Dsignaltap(mmdst.wt2d_y);
               mmdst.b_wt2d_y_valid = YAC_TRUE;
               break;

               case STSAMPLE_MM_DST_WT_2D_Y_REL:
               if(bAutoAdd)
                  mmdst.wt2d_y += srcValDef;
               else if(bAutoMul)
                  mmdst.wt2d_y *= srcValDef;
               Delse_mm_lerp(mmdst.wt2d_y);
               Dsignaltap(mmdst.wt2d_y);
               mmdst.b_wt2d_y_valid = YAC_TRUE;
               break;

               case STSAMPLE_MM_DST_LOOP_REPEATS_SCALE:
               if(bAutoMul)
                  mmdst_loop_repeats_scale *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
               if(bAutoAdd)
                  mmdst_loop_repeats_scale += srcValDef * 8.0f;
               Delse_mm_lerp_bipolar8(mmdst_loop_repeats_scale);
               Dsignaltap(mmdst_loop_repeats_scale);
               break;

               case STSAMPLE_MM_DST_JUMPTOLOOP_REL:
               if((0u == replay_ticks) || (STSAMPLE_MM_OP_REPLACE_INIT != mm->op))
               {
                  if(bAutoAdd)
                     mmdst.jumptoloop += srcValDef;
                  else if(bAutoMul)
                     mmdst.jumptoloop *= srcValDef;
                  Delse_mm_lerp(mmdst.jumptoloop);
                  mmdst.b_jumptoloop_valid = YAC_TRUE;
                  Dsignaltap(mmdst.jumptoloop);
               }
               break;

               case STSAMPLE_MM_DST_JUMPTOLOOP_ABS127:
               if((0u == replay_ticks) || (STSAMPLE_MM_OP_REPLACE_INIT != mm->op))
               {
                  // yac_host->printf("xxx STSAMPLE_MM_DST_JUMPTOLOOP_ABS: srcValDef=%f num_sample_loops=%u\n", srcValDef, num_sample_loops);
                  // [02Dec2023] multiply value by 127 to avoid modmatrix amt scaling (should now be set to 100% instead of 12700%)
                  srcValDef = sF32(srcValDef * 127.0f /*+ 0.5f*/) / sF32((num_sample_loops > 0) ? num_sample_loops : 1);
                  if(bAutoAdd)
                     mmdst.jumptoloop += srcValDef;
                  else if(bAutoMul)
                     mmdst.jumptoloop *= srcValDef;
                  Delse_mm_lerp(mmdst.jumptoloop);
                  mmdst.b_jumptoloop_valid = YAC_TRUE;
                  Dsignaltap(mmdst.jumptoloop);
               }
               break;

               case STSAMPLE_MM_DST_JUMPTOLOOP_ABS:
               if((0u == replay_ticks) || (STSAMPLE_MM_OP_REPLACE_INIT != mm->op))
               {
                  // yac_host->printf("xxx STSAMPLE_MM_DST_JUMPTOLOOP_ABS: srcValDef=%f num_sample_loops=%u\n", srcValDef, num_sample_loops);
                  // [02Dec2023] multiply value by 127 to avoid modmatrix amt scaling (should now be set to 100% instead of 12700%)
                  srcValDef = sF32(srcValDef /*+ 0.5f*/) / sF32((num_sample_loops > 0) ? num_sample_loops : 1);
                  if(bAutoAdd)
                     mmdst.jumptoloop += srcValDef;
                  else if(bAutoMul)
                     mmdst.jumptoloop *= srcValDef;
                  Delse_mm_lerp(mmdst.jumptoloop);
                  mmdst.b_jumptoloop_valid = YAC_TRUE;
                  Dsignaltap(mmdst.jumptoloop);
               }
               break;

               case STSAMPLE_MM_DST_FREQ_LFO_SPEED:
               if(bFreqLFOGlobal)
               {
                  if(bAutoMul)
                     sample->lfo_freq_global.mmdst_speed *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
                  else if(bAutoAdd)
                     sample->lfo_freq_global.mmdst_speed += srcValDef * 8.0f;
                  Delse_mm_lerp_bipolar8(sample->lfo_freq_global.mmdst_speed);
                  Dsignaltap(sample->lfo_freq_global.mmdst_speed);
               }
               else
               {
                  if(bAutoMul)
                     lfo_freq.mmdst_speed *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
                  else if(bAutoAdd)
                     lfo_freq.mmdst_speed += srcValDef * 8.0f;
                  Delse_mm_lerp_bipolar8(lfo_freq.mmdst_speed);
                  Dsignaltap(lfo_freq.mmdst_speed);
               }
               break;

               case STSAMPLE_MM_DST_FREQ_LFO_SPEED_ABS:
               if(bFreqLFOGlobal)
               {
                  if(bAutoMul)
                     sample->lfo_freq_global.mmdst_speed *= srcValDef;
                  else if(bAutoAdd)
                     sample->lfo_freq_global.mmdst_speed += srcValDef;
                  Delse_mm_lerp(sample->lfo_freq_global.mmdst_speed);
                  Dsignaltap(sample->lfo_freq_global.mmdst_speed);
               }
               else
               {
                  if(bAutoMul)
                     lfo_freq.mmdst_speed *= srcValDef;
                  else if(bAutoAdd)
                     lfo_freq.mmdst_speed += srcValDef;
                  Delse_mm_lerp(lfo_freq.mmdst_speed);
                  Dsignaltap(lfo_freq.mmdst_speed);
               }
               break;

               case STSAMPLE_MM_DST_FREQ_LFO_LEVEL:
               if(bAutoMul)
                  mmdst.lfo_freq_level *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
               else if(bAutoAdd)
                  mmdst.lfo_freq_level += srcValDef * 8.0f;
               Delse_mm_lerp_bipolar8(mmdst.lfo_freq_level);
               Dsignaltap(mmdst.lfo_freq_level);
               break;

               case STSAMPLE_MM_DST_FREQ_LFO_FREQ_AMT:
               if(bAutoMul)
                  mmdst.freq_lfo_freq_amt *= srcValDef;
               else if(bAutoAdd)
                  mmdst.freq_lfo_freq_amt += srcValDef;
               Delse_mm_lerp_scl(mmdst.freq_lfo_freq_amt, 1.0f);
               Dsignaltap(mmdst.freq_lfo_freq_amt);
               break;

               case STSAMPLE_MM_DST_VOL_LFO_SPEED:
               if(bVolLFOGlobal)
               {
                  if(bAutoMul)
                     sample->lfo_vol_global.mmdst_speed *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
                  else if(bAutoAdd)
                     sample->lfo_vol_global.mmdst_speed += srcValDef * 8.0f;
                  Delse_mm_lerp_bipolar8(sample->lfo_vol_global.mmdst_speed);
                  Dsignaltap(sample->lfo_vol_global.mmdst_speed);
               }
               else
               {
                  if(bAutoMul)
                     lfo_vol.mmdst_speed *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
                  else if(bAutoAdd)
                     lfo_vol.mmdst_speed += srcValDef * 8.0f;
                  Delse_mm_lerp_bipolar8(lfo_vol.mmdst_speed);
                  Dsignaltap(lfo_vol.mmdst_speed);
               }
               break;

               case STSAMPLE_MM_DST_VOL_LFO_SPEED_ABS:
               if(bVolLFOGlobal)
               {
                  if(bAutoMul)
                     sample->lfo_vol_global.mmdst_speed *= srcValDef;
                  else if(bAutoAdd)
                     sample->lfo_vol_global.mmdst_speed += srcValDef;
                  Delse_mm_lerp(sample->lfo_vol_global.mmdst_speed);
                  Dsignaltap(sample->lfo_vol_global.mmdst_speed);
               }
               else
               {
                  if(bAutoMul)
                     lfo_vol.mmdst_speed *= srcValDef;
                  else if(bAutoAdd)
                     lfo_vol.mmdst_speed += srcValDef;
                  Delse_mm_lerp(lfo_vol.mmdst_speed);
                  Dsignaltap(lfo_vol.mmdst_speed);
               }
               break;

               case STSAMPLE_MM_DST_VOL_LFO_LEVEL:
               if(bAutoMul)
                  mmdst.lfo_vol_level *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
               else if(bAutoAdd)
                  mmdst.lfo_vol_level += srcValDef * 8.0f;
               Delse_mm_lerp_bipolar8(mmdst.lfo_vol_level);
               Dsignaltap(mmdst.lfo_vol_level);
               break;

               case STSAMPLE_MM_DST_VOL_LFO_VOL_AMT:
               if(bAutoMul)
                  mmdst.vol_lfo_vol_amt *= srcValDef;
               else if(bAutoAdd)
                  mmdst.vol_lfo_vol_amt += srcValDef;
               Delse_mm_lerp_scl(mmdst.vol_lfo_vol_amt, 1.0f);
               Dsignaltap(mmdst.vol_lfo_vol_amt);
               break;

               case STSAMPLE_MM_DST_PAN_LFO_SPEED:
               if(bPanLFOGlobal)
               {
                  if(bAutoMul)
                     sample->lfo_pan_global.mmdst_speed *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
                  else if(bAutoAdd)
                     sample->lfo_pan_global.mmdst_speed += srcValDef * 8.0f;
                  Delse_mm_lerp_bipolar8(sample->lfo_pan_global.mmdst_speed);
                  Dsignaltap(sample->lfo_pan_global.mmdst_speed);
               }
               else
               {
                  if(bAutoMul)
                     lfo_pan.mmdst_speed *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
                  else if(bAutoAdd)
                     lfo_pan.mmdst_speed += srcValDef * 8.0f;
                  Delse_mm_lerp_bipolar8(lfo_pan.mmdst_speed);
                  Dsignaltap(lfo_pan.mmdst_speed);
               }
               break;

               case STSAMPLE_MM_DST_PAN_LFO_SPEED_ABS:
               if(bPanLFOGlobal)
               {
                  if(bAutoMul)
                     sample->lfo_pan_global.mmdst_speed *= srcValDef;
                  else if(bAutoAdd)
                     sample->lfo_pan_global.mmdst_speed += srcValDef;
                  Delse_mm_lerp(sample->lfo_pan_global.mmdst_speed);
                  Dsignaltap(sample->lfo_pan_global.mmdst_speed);
               }
               else
               {
                  if(bAutoMul)
                     lfo_pan.mmdst_speed *= srcValDef;
                  else if(bAutoAdd)
                     lfo_pan.mmdst_speed += srcValDef;
                  Delse_mm_lerp(lfo_pan.mmdst_speed);
                  Dsignaltap(lfo_pan.mmdst_speed);
               }
               break;

               case STSAMPLE_MM_DST_PAN_LFO_LEVEL:
               if(bAutoMul)
                  mmdst.lfo_pan_level *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
               else if(bAutoAdd)
                  mmdst.lfo_pan_level += srcValDef * 8.0f;
               Delse_mm_lerp_bipolar8(mmdst.lfo_pan_level);
               Dsignaltap(mmdst.lfo_pan_level);
               break;

               case STSAMPLE_MM_DST_PAN_LFO_PAN_AMT:
               if(bAutoMul)
                  mmdst.pan_lfo_pan_amt *= srcValDef;
               else if(bAutoAdd)
                  mmdst.pan_lfo_pan_amt += srcValDef;
               Delse_mm_lerp_scl(mmdst.pan_lfo_pan_amt, 1.0f);
               Dsignaltap(mmdst.pan_lfo_pan_amt);
               break;

               case STSAMPLE_MM_DST_AUX_LFO_SPEED:
               if(bAuxLFOGlobal)
               {
                  if(bAutoMul)
                     sample->lfo_aux_global.mmdst_speed *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
                  else if(bAutoAdd)
                     sample->lfo_aux_global.mmdst_speed += srcValDef * 8.0f;
                  Delse_mm_lerp_bipolar8(sample->lfo_aux_global.mmdst_speed);
                  Dsignaltap(sample->lfo_aux_global.mmdst_speed);
               }
               else
               {
                  if(bAutoMul)
                     lfo_aux.mmdst_speed *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
                  else if(bAutoAdd)
                     lfo_aux.mmdst_speed += srcValDef * 8.0f;
                  Delse_mm_lerp_bipolar8(lfo_aux.mmdst_speed);
                  Dsignaltap(lfo_aux.mmdst_speed);
               }
               break;

               case STSAMPLE_MM_DST_AUX_LFO_SPEED_ABS:
               if(bAuxLFOGlobal)
               {
                  if(bAutoMul)
                     sample->lfo_aux_global.mmdst_speed *= srcValDef;
                  else if(bAutoAdd)
                     sample->lfo_aux_global.mmdst_speed += srcValDef;
                  Delse_mm_lerp(sample->lfo_aux_global.mmdst_speed);
                  Dsignaltap(sample->lfo_aux_global.mmdst_speed);
               }
               else
               {
                  if(bAutoMul)
                     lfo_aux.mmdst_speed *= srcValDef;
                  else if(bAutoAdd)
                     lfo_aux.mmdst_speed += srcValDef;
                  Delse_mm_lerp(lfo_aux.mmdst_speed);
                  Dsignaltap(lfo_aux.mmdst_speed);
               }
               break;

               case STSAMPLE_MM_DST_AUX_LFO_LEVEL:
               if(bAutoMul)
                  mmdst.lfo_aux_level *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
               else if(bAutoAdd)
                  mmdst.lfo_aux_level += srcValDef * 8.0f;
               Delse_mm_lerp_bipolar8(mmdst.lfo_aux_level);
               Dsignaltap(mmdst.lfo_aux_level);
               break;

               case STSAMPLE_MM_DST_AUX_LFO_FLT_AMT:
               if(bAutoMul)
                  mmdst.aux_lfo_flt_amt *= srcValDef;
               else if(bAutoAdd)
                  mmdst.aux_lfo_flt_amt += srcValDef;
               Delse_mm_lerp_scl(mmdst.aux_lfo_flt_amt, 1.0f);
               Dsignaltap(mmdst.aux_lfo_flt_amt);
               break;

               case STSAMPLE_MM_DST_FREQ_ENV_SPEED:
               if(bAutoMul)
                  adsr_freq.mmdst_speed *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
               else if(bAutoAdd)
                  adsr_freq.mmdst_speed += srcValDef * 8.0f;
               Delse_mm_lerp_bipolar8(adsr_freq.mmdst_speed);
               Dsignaltap(adsr_freq.mmdst_speed);
               break;

               case STSAMPLE_MM_DST_FREQ_ENV_SPEED_ABS:
               if(bAutoMul)
                  adsr_freq.mmdst_speed *= srcValDef;
               else if(bAutoAdd)
                  adsr_freq.mmdst_speed += srcValDef;
               Delse_mm_lerp_bipolar8(adsr_freq.mmdst_speed);
               Dsignaltap(adsr_freq.mmdst_speed);
               break;

               case STSAMPLE_MM_DST_FREQ_ENV_LEVEL:
               if(bAutoMul)
                  mmdst.env_freq_level *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
               else if(bAutoAdd)
                  mmdst.env_freq_level += srcValDef * 8.0f;
               Delse_mm_lerp_bipolar8(mmdst.env_freq_level);
               Dsignaltap(mmdst.env_freq_level);
               break;

               case STSAMPLE_MM_DST_FREQ_ENV_INTENSITY:
               if(bAutoAdd)
                  adsr_freq.mmdst_intensity += srcValDef;
               else if(bAutoMul)
                  adsr_freq.mmdst_intensity *= srcValDef;
               Delse_mm_lerp(adsr_freq.mmdst_intensity);
               Dsignaltap(adsr_freq.mmdst_intensity);
               break;

               case STSAMPLE_MM_DST_FREQ_ENV_ATTACK_SPEED:
               if(bAutoMul)
                  adsr_freq.mmdst_aspeed *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
               else if(bAutoAdd)
                  adsr_freq.mmdst_aspeed += srcValDef * 8.0f;
               Delse_mm_lerp_bipolar8(adsr_freq.mmdst_aspeed);
               Dsignaltap(adsr_freq.mmdst_aspeed);
               break;

               case STSAMPLE_MM_DST_FREQ_ENV_ATTACK_SPEED_ABS:
               if(bAutoMul)
                  adsr_freq.mmdst_aspeed *= srcValDef;
               else if(bAutoAdd)
                  adsr_freq.mmdst_aspeed += srcValDef;
               Delse_mm_lerp(adsr_freq.mmdst_aspeed);
               Dsignaltap(adsr_freq.mmdst_aspeed);
               break;

               case STSAMPLE_MM_DST_FREQ_ENV_SUSTAIN_SPEED:
               if(bAutoMul)
                  adsr_freq.mmdst_sspeed *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
               else if(bAutoAdd)
                  adsr_freq.mmdst_sspeed += srcValDef * 8.0f;
               Delse_mm_lerp_bipolar8(adsr_freq.mmdst_sspeed);
               Dsignaltap(adsr_freq.mmdst_sspeed);
               break;

               case STSAMPLE_MM_DST_FREQ_ENV_SUSTAIN_SPEED_ABS:
               if(bAutoMul)
                  adsr_freq.mmdst_sspeed *= srcValDef;
               else if(bAutoAdd)
                  adsr_freq.mmdst_sspeed += srcValDef;
               Delse_mm_lerp(adsr_freq.mmdst_sspeed);
               Dsignaltap(adsr_freq.mmdst_sspeed);
               break;

               case STSAMPLE_MM_DST_FREQ_ENV_RELEASE_SPEED:
               if(bAutoMul)
                  adsr_freq.mmdst_rspeed *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
               else if(bAutoAdd)
                  adsr_freq.mmdst_rspeed += srcValDef * 8.0f;
               Delse_mm_lerp_bipolar8(adsr_freq.mmdst_rspeed);
               Dsignaltap(adsr_freq.mmdst_rspeed);
               break;

               case STSAMPLE_MM_DST_FREQ_ENV_RELEASE_SPEED_ABS:
               if(bAutoMul)
                  adsr_freq.mmdst_rspeed *= srcValDef;
               else if(bAutoAdd)
                  adsr_freq.mmdst_rspeed += srcValDef;
               Delse_mm_lerp(adsr_freq.mmdst_rspeed);
               Dsignaltap(adsr_freq.mmdst_rspeed);
               break;

               case STSAMPLE_MM_DST_VOL_ENV_SPEED:
               if(bAutoMul)
                  adsr_vol.mmdst_speed *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
               else if(bAutoAdd)
                  adsr_vol.mmdst_speed += srcValDef * 8.0f;
               Delse_mm_lerp_bipolar8(adsr_vol.mmdst_speed);
               Dsignaltap(adsr_vol.mmdst_speed);
               break;

               case STSAMPLE_MM_DST_VOL_ENV_SPEED_ABS:
               if(bAutoMul)
                  adsr_vol.mmdst_speed *= srcValDef;
               else if(bAutoAdd)
                  adsr_vol.mmdst_speed += srcValDef;
               Delse_mm_lerp(adsr_vol.mmdst_speed);
               Dsignaltap(adsr_vol.mmdst_speed);
               break;

               case STSAMPLE_MM_DST_VOL_ENV_LEVEL:
               if(bAutoMul)
                  mmdst.env_vol_level *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
               else if(bAutoAdd)
                  mmdst.env_vol_level += srcValDef * 8.0f;
               Delse_mm_lerp_bipolar8(mmdst.env_vol_level);
               Dsignaltap(mmdst.env_vol_level);
               break;

               case STSAMPLE_MM_DST_VOL_ENV_INTENSITY:
               if(bAutoAdd)
                  adsr_vol.mmdst_intensity += srcValDef;
               else if(bAutoMul)
                  adsr_vol.mmdst_intensity *= srcValDef;
               Delse_mm_lerp(adsr_vol.mmdst_intensity);
               Dsignaltap(adsr_vol.mmdst_intensity);
               break;

               case STSAMPLE_MM_DST_VOL_ENV_ATTACK_SPEED:
               if(bAutoMul)
                  adsr_vol.mmdst_aspeed *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
               else if(bAutoAdd)
                  adsr_vol.mmdst_aspeed += srcValDef * 8.0f;
               Delse_mm_lerp_bipolar8(adsr_vol.mmdst_aspeed);
               Dsignaltap(adsr_vol.mmdst_aspeed);
               break;

               case STSAMPLE_MM_DST_VOL_ENV_ATTACK_SPEED_ABS:
               if(bAutoMul)
                  adsr_vol.mmdst_aspeed *= srcValDef;
               else if(bAutoAdd)
                  adsr_vol.mmdst_aspeed += srcValDef;
               Delse_mm_lerp(adsr_vol.mmdst_aspeed);
               Dsignaltap(adsr_vol.mmdst_aspeed);
               break;

               case STSAMPLE_MM_DST_VOL_ENV_SUSTAIN_SPEED:
               if(bAutoMul)
                  adsr_vol.mmdst_sspeed *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
               else if(bAutoAdd)
                  adsr_vol.mmdst_sspeed += srcValDef * 8.0f;
               Delse_mm_lerp_bipolar8(adsr_vol.mmdst_sspeed);
               Dsignaltap(adsr_vol.mmdst_sspeed);
               break;

               case STSAMPLE_MM_DST_VOL_ENV_SUSTAIN_SPEED_ABS:
               if(bAutoMul)
                  adsr_vol.mmdst_sspeed *= srcValDef;
               else if(bAutoAdd)
                  adsr_vol.mmdst_sspeed += srcValDef;
               Delse_mm_lerp(adsr_vol.mmdst_sspeed);
               Dsignaltap(adsr_vol.mmdst_sspeed);
               break;

               case STSAMPLE_MM_DST_VOL_ENV_RELEASE_SPEED:
               if(bAutoMul)
                  adsr_vol.mmdst_rspeed *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
               else if(bAutoAdd)
                  adsr_vol.mmdst_rspeed += srcValDef * 8.0f;
               Delse_mm_lerp_bipolar8(adsr_vol.mmdst_rspeed);
               Dsignaltap(adsr_vol.mmdst_rspeed);
               break;

               case STSAMPLE_MM_DST_VOL_ENV_RELEASE_SPEED_ABS:
               if(bAutoMul)
                  adsr_vol.mmdst_rspeed *= srcValDef;
               else if(bAutoAdd)
                  adsr_vol.mmdst_rspeed += srcValDef;
               Delse_mm_lerp(adsr_vol.mmdst_rspeed);
               Dsignaltap(adsr_vol.mmdst_rspeed);
               break;

               case STSAMPLE_MM_DST_PAN_ENV_SPEED:
               if(bAutoMul)
                  adsr_pan.mmdst_speed *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
               else if(bAutoAdd)
                  adsr_pan.mmdst_speed += srcValDef * 8.0f;
               Delse_mm_lerp_bipolar8(adsr_pan.mmdst_speed);
               Dsignaltap(adsr_pan.mmdst_speed);
               break;

               case STSAMPLE_MM_DST_PAN_ENV_SPEED_ABS:
               if(bAutoMul)
                  adsr_pan.mmdst_speed *= srcValDef;
               else if(bAutoAdd)
                  adsr_pan.mmdst_speed += srcValDef;
               Delse_mm_lerp(adsr_pan.mmdst_speed);
               Dsignaltap(adsr_pan.mmdst_speed);
               break;

               case STSAMPLE_MM_DST_PAN_ENV_LEVEL:
               if(bAutoMul)
                  mmdst.env_pan_level *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
               else if(bAutoAdd)
                  mmdst.env_pan_level += srcValDef * 8.0f;
               Delse_mm_lerp_bipolar8(mmdst.env_pan_level);
               Dsignaltap(mmdst.env_pan_level);
               break;

               case STSAMPLE_MM_DST_PAN_ENV_INTENSITY:
               if(bAutoAdd)
                  adsr_pan.mmdst_intensity += srcValDef;
               else if(bAutoMul)
                  adsr_pan.mmdst_intensity *= srcValDef;
               Delse_mm_lerp(adsr_pan.mmdst_intensity);
               Dsignaltap(adsr_pan.mmdst_intensity);
               break;

               case STSAMPLE_MM_DST_PAN_ENV_ATTACK_SPEED:
               if(bAutoMul)
                  adsr_pan.mmdst_aspeed *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
               else if(bAutoAdd)
                  adsr_pan.mmdst_aspeed += srcValDef * 8.0f;
               Delse_mm_lerp_bipolar8(adsr_pan.mmdst_aspeed);
               Dsignaltap(adsr_pan.mmdst_aspeed);
               break;

               case STSAMPLE_MM_DST_PAN_ENV_ATTACK_SPEED_ABS:
               if(bAutoMul)
                  adsr_pan.mmdst_aspeed *= srcValDef;
               else if(bAutoAdd)
                  adsr_pan.mmdst_aspeed += srcValDef;
               Delse_mm_lerp(adsr_pan.mmdst_aspeed);
               Dsignaltap(adsr_pan.mmdst_aspeed);
               break;

               case STSAMPLE_MM_DST_PAN_ENV_SUSTAIN_SPEED:
               if(bAutoMul)
                  adsr_pan.mmdst_sspeed *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
               else if(bAutoAdd)
                  adsr_pan.mmdst_sspeed += srcValDef * 8.0f;
               Delse_mm_lerp_bipolar8(adsr_pan.mmdst_sspeed);
               Dsignaltap(adsr_pan.mmdst_sspeed);
               break;

               case STSAMPLE_MM_DST_PAN_ENV_SUSTAIN_SPEED_ABS:
               if(bAutoMul)
                  adsr_pan.mmdst_sspeed *= srcValDef;
               else if(bAutoAdd)
                  adsr_pan.mmdst_sspeed += srcValDef;
               Delse_mm_lerp(adsr_pan.mmdst_sspeed);
               Dsignaltap(adsr_pan.mmdst_sspeed);
               break;

               case STSAMPLE_MM_DST_PAN_ENV_RELEASE_SPEED:
               if(bAutoMul)
                  adsr_pan.mmdst_rspeed *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
               else if(bAutoAdd)
                  adsr_pan.mmdst_rspeed += srcValDef * 8.0f;
               Delse_mm_lerp_bipolar8(adsr_pan.mmdst_rspeed);
               Dsignaltap(adsr_pan.mmdst_rspeed);
               break;

               case STSAMPLE_MM_DST_PAN_ENV_RELEASE_SPEED_ABS:
               if(bAutoMul)
                  adsr_pan.mmdst_rspeed *= srcValDef;
               else if(bAutoAdd)
                  adsr_pan.mmdst_rspeed += srcValDef;
               Delse_mm_lerp(adsr_pan.mmdst_rspeed);
               Dsignaltap(adsr_pan.mmdst_rspeed);
               break;

               case STSAMPLE_MM_DST_AUX_ENV_SPEED:
               if(bAutoMul)
                  adsr_aux.mmdst_speed *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
               else if(bAutoAdd)
                  adsr_aux.mmdst_speed += srcValDef * 8.0f;
               Delse_mm_lerp_bipolar8(adsr_aux.mmdst_speed);
               Dsignaltap(adsr_aux.mmdst_speed);
               break;

               case STSAMPLE_MM_DST_AUX_ENV_SPEED_ABS:
               if(bAutoMul)
                  adsr_aux.mmdst_speed *= srcValDef;
               else if(bAutoAdd)
                  adsr_aux.mmdst_speed += srcValDef;
               Delse_mm_lerp(adsr_aux.mmdst_speed);
               Dsignaltap(adsr_aux.mmdst_speed);
               break;

               case STSAMPLE_MM_DST_AUX_ENV_LEVEL:
               if(bAutoMul)
                  mmdst.env_aux_level *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
               else if(bAutoAdd)
                  mmdst.env_aux_level += srcValDef * 8.0f;
               Delse_mm_lerp_bipolar8(mmdst.env_aux_level);
               Dsignaltap(mmdst.env_aux_level);
               break;

               case STSAMPLE_MM_DST_AUX_ENV_INTENSITY:
               if(bAutoAdd)
                  adsr_aux.mmdst_intensity += srcValDef;
               else if(bAutoMul)
                  adsr_aux.mmdst_intensity *= srcValDef;
               Delse_mm_lerp(adsr_aux.mmdst_intensity);
               Dsignaltap(adsr_aux.mmdst_intensity);
               break;

               case STSAMPLE_MM_DST_AUX_ENV_ATTACK_SPEED:
               if(bAutoMul)
                  adsr_aux.mmdst_aspeed *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
               else if(bAutoAdd)
                  adsr_aux.mmdst_aspeed += srcValDef * 8.0f;
               Delse_mm_lerp_bipolar8(adsr_aux.mmdst_aspeed);
               Dsignaltap(adsr_aux.mmdst_aspeed);
               break;

               case STSAMPLE_MM_DST_AUX_ENV_ATTACK_SPEED_ABS:
               if(bAutoMul)
                  adsr_aux.mmdst_aspeed *= srcValDef;
               else if(bAutoAdd)
                  adsr_aux.mmdst_aspeed += srcValDef;
               Delse_mm_lerp(adsr_aux.mmdst_aspeed);
               Dsignaltap(adsr_aux.mmdst_aspeed);
               break;

               case STSAMPLE_MM_DST_AUX_ENV_SUSTAIN_SPEED:
               if(bAutoMul)
                  adsr_aux.mmdst_sspeed *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
               else if(bAutoAdd)
                  adsr_aux.mmdst_sspeed += srcValDef * 8.0f;
               Delse_mm_lerp_bipolar8(adsr_aux.mmdst_sspeed);
               Dsignaltap(adsr_aux.mmdst_sspeed);
               break;

               case STSAMPLE_MM_DST_AUX_ENV_SUSTAIN_SPEED_ABS:
               if(bAutoMul)
                  adsr_aux.mmdst_sspeed *= srcValDef;
               else if(bAutoAdd)
                  adsr_aux.mmdst_sspeed += srcValDef;
               Delse_mm_lerp(adsr_aux.mmdst_sspeed);
               Dsignaltap(adsr_aux.mmdst_sspeed);
               break;

               case STSAMPLE_MM_DST_AUX_ENV_RELEASE_SPEED:
               if(bAutoMul)
                  adsr_aux.mmdst_rspeed *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
               else if(bAutoAdd)
                  adsr_aux.mmdst_rspeed += srcValDef * 8.0f;
               Delse_mm_lerp_bipolar8(adsr_aux.mmdst_rspeed);
               Dsignaltap(adsr_aux.mmdst_rspeed);
               break;

               case STSAMPLE_MM_DST_AUX_ENV_RELEASE_SPEED_ABS:
               if(bAutoMul)
                  adsr_aux.mmdst_rspeed *= srcValDef;
               else if(bAutoAdd)
                  adsr_aux.mmdst_rspeed += srcValDef;
               Delse_mm_lerp(adsr_aux.mmdst_rspeed);
               Dsignaltap(adsr_aux.mmdst_rspeed);
               break;

               case STSAMPLE_MM_DST_MOD_1_AMOUNT:
               case STSAMPLE_MM_DST_MOD_2_AMOUNT:
               case STSAMPLE_MM_DST_MOD_3_AMOUNT:
               case STSAMPLE_MM_DST_MOD_4_AMOUNT:
               case STSAMPLE_MM_DST_MOD_5_AMOUNT:
               case STSAMPLE_MM_DST_MOD_6_AMOUNT:
               case STSAMPLE_MM_DST_MOD_7_AMOUNT:
               case STSAMPLE_MM_DST_MOD_8_AMOUNT:
               case STSAMPLE_MM_DST_MOD_9_AMOUNT:
               case STSAMPLE_MM_DST_MOD_10_AMOUNT:
               case STSAMPLE_MM_DST_MOD_11_AMOUNT:
               case STSAMPLE_MM_DST_MOD_12_AMOUNT:
               case STSAMPLE_MM_DST_MOD_13_AMOUNT:
               case STSAMPLE_MM_DST_MOD_14_AMOUNT:
               case STSAMPLE_MM_DST_MOD_15_AMOUNT:
               case STSAMPLE_MM_DST_MOD_16_AMOUNT:
               {
                  sUI mmDestIdx = (mm->dst - STSAMPLE_MM_DST_MOD_1_AMOUNT);
                  if(mmIdx != mmDestIdx)
                  {
                     if(mmWriteMask[mmDestIdx])
                     {
                        // Mod Matrix amount has already been written by another mod matrix entry
                        if(bAutoAdd)
                           modmatrix_amt[mmDestIdx] += srcValDef;
                        else if(bAutoMul)
                           modmatrix_amt[mmDestIdx] *= srcValDef;
                        Delse_mm_lerp(modmatrix_amt[mmDestIdx]);
                     }
                     else
                     {
                        // Mod Matrix amount has not been written, yet
                        if(bAutoAdd)
                           modmatrix_amt[mmDestIdx] = srcValDef;
                        else if(bAutoMul)
                           modmatrix_amt[mmDestIdx] = 0.0f;
                        Delse_mm_lerp(modmatrix_amt[mmDestIdx]);

                        mmWriteMask[mmDestIdx] = YAC_TRUE;
                     }
                     Dsignaltap(modmatrix_amt[mmDestIdx]);
                  }
               }
               break;

               case STSAMPLE_MM_DST_GLIDE_SPEED:
               if(bAutoMul)
                  mmdst_glide_speed *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
               else if(bAutoAdd)
                  mmdst_glide_speed += srcValDef * 8.0f;
               Delse_mm_lerp_bipolar8(mmdst_glide_speed);
               Dsignaltap(mmdst_glide_speed);
               break;

               case STSAMPLE_MM_DST_TIMESTRETCH:
               if(bAutoAdd)
                  mmdst_timestretch += srcValDef;
               else if(bAutoMul)
                  mmdst_timestretch *= srcValDef;
               Delse_mm_lerp(mmdst_timestretch);
               Dsignaltap(mmdst_timestretch);
               break;

               case STSAMPLE_MM_DST_TIMESTRETCH_BEND:
               if(bAutoAdd)
                  mmdst_timestretch_bend += srcValDef;
               else if(bAutoMul)
                  mmdst_timestretch_bend *= srcValDef;
               Delse_mm_lerp(mmdst_timestretch_bend);
               Dsignaltap(mmdst_timestretch_bend);
               break;

               case STSAMPLE_MM_DST_MODSEQ1_PATCH:
               // Dyac_host_printf("xxx MODSEQ1_PATCH srcValDef=%f bAutoAdd=%d mmdst.modseq_patch[0]=%f\n", srcValDef, bAutoAdd, mmdst.modseq_patch[0]);
               if(bAutoRep)
                  mmdst.modseq_patch[0] = srcValDef;
               else if(bAutoAdd)
                  mmdst.modseq_patch[0] += srcValDef;
               else if(bAutoMul)
                  mmdst.modseq_patch[0] *= srcValDef;
               Delse_mm_lerp(mmdst.modseq_patch[0]);
               Dsignaltap(mmdst.modseq_patch[0]);
               break;

               case STSAMPLE_MM_DST_MODSEQ1_SPEED:
               if(bModSeqGlobal[0])
               {
                  if(bAutoMul)
                     sample->modseq_global[0].mmdst_speed *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
                  else if(bAutoAdd)
                     sample->modseq_global[0].mmdst_speed += srcValDef * 8.0f;
                  Delse_mm_lerp_bipolar8(sample->modseq_global[0].mmdst_speed);
                  Dsignaltap(sample->modseq_global[0].mmdst_speed);
               }
               else
               {
                  if(bAutoMul)
                     modseq[0].mmdst_speed *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
                  else if(bAutoAdd)
                     modseq[0].mmdst_speed += srcValDef * 8.0f;
                  Delse_mm_lerp_bipolar8(modseq[0].mmdst_speed);
                  Dsignaltap(modseq[0].mmdst_speed);
               }
               break;

               case STSAMPLE_MM_DST_MODSEQ1_SPEED_ABS:
               if(bModSeqGlobal[0])
               {
                  if(bAutoMul)
                     sample->modseq_global[0].mmdst_speed *= srcValDef;
                  else if(bAutoAdd)
                     sample->modseq_global[0].mmdst_speed += srcValDef;
                  Delse_mm_lerp(sample->modseq_global[0].mmdst_speed);
                  Dsignaltap(sample->modseq_global[0].mmdst_speed);
               }
               else
               {
                  if(bAutoMul)
                     modseq[0].mmdst_speed *= srcValDef;
                  else if(bAutoAdd)
                     modseq[0].mmdst_speed += srcValDef;
                  Delse_mm_lerp(modseq[0].mmdst_speed);
                  Dsignaltap(modseq[0].mmdst_speed);
               }
               break;

               case STSAMPLE_MM_DST_MODSEQ1_LEVEL:
               if(bAutoMul)
                  mmdst.modseq_level[0] *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
               else if(bAutoAdd)
                  mmdst.modseq_level[0] += srcValDef * 8.0f;
               Delse_mm_lerp_bipolar8(mmdst.modseq_level[0]);
               Dsignaltap(mmdst.modseq_level[0]);
               break;

               case STSAMPLE_MM_DST_MODSEQ1_NUMSTEPS:
               if(bModSeqGlobal[0])
               {
                  if(bAutoAdd)
                     sample->modseq_global[0].mmdst_numsteps += srcValDef;
                  else if(bAutoMul)
                     sample->modseq_global[0].mmdst_numsteps *= srcValDef;
                  Delse_mm_lerp_scl(sample->modseq_global[0].mmdst_numsteps, 1.0f);
                  Dsignaltap(sample->modseq_global[0].mmdst_numsteps);
               }
               else
               {
                  if(bAutoAdd)
                     modseq[0].mmdst_numsteps += srcValDef;
                  else if(bAutoMul)
                     modseq[0].mmdst_numsteps *= srcValDef;
                  Delse_mm_lerp_scl(modseq[0].mmdst_numsteps, 1.0f);
                  Dsignaltap(modseq[0].mmdst_numsteps);
               }
               break;

               case STSAMPLE_MM_DST_MODSEQ1_ADVANCE:
               if(bModSeqGlobal[0])
               {
                  if(bAutoAdd)
                     sample->modseq_global[0].mmdst_advance += srcValDef;
                  else if(bAutoMul)
                     sample->modseq_global[0].mmdst_advance *= srcValDef;
                  Delse_mm_lerp_scl(sample->modseq_global[0].mmdst_advance, 1.0f);
                  Dsignaltap(sample->modseq_global[0].mmdst_advance);
               }
               else
               {
                  if(bAutoAdd)
                     modseq[0].mmdst_advance += srcValDef;
                  else if(bAutoMul)
                     modseq[0].mmdst_advance *= srcValDef;
                  Delse_mm_lerp_scl(modseq[0].mmdst_advance, 1.0f);
                  Dsignaltap(modseq[0].mmdst_advance);
               }
               break;

               case STSAMPLE_MM_DST_MODSEQ1_STEP:
               if(bModSeqGlobal[0])
               {
                  if(bAutoAdd)
                     sample->modseq_global[0].mmdst_step += srcValDef;
                  else if(bAutoMul)
                     sample->modseq_global[0].mmdst_step *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
                  Delse_mm_lerp_scl(sample->modseq_global[0].mmdst_step, 1.0f);
                  Dsignaltap(sample->modseq_global[0].mmdst_step);
                  sample->modseq_global[0].b_mmdst_step = YAC_TRUE;
               }
               else
               {
                  if(bAutoAdd)
                     modseq[0].mmdst_step += srcValDef;
                  else if(bAutoMul)
                     modseq[0].mmdst_step *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
                  Delse_mm_lerp_scl(modseq[0].mmdst_step, 1.0f);
                  Dsignaltap(modseq[0].mmdst_step);
                  modseq[0].b_mmdst_step = YAC_TRUE;
               }
               break;

               case STSAMPLE_MM_DST_MODSEQ2_PATCH:
               if(bAutoRep)
                  mmdst.modseq_patch[1] = srcValDef;
               else if(bAutoAdd)
                  mmdst.modseq_patch[1] += srcValDef;
               else if(bAutoMul)
                  mmdst.modseq_patch[1] *= srcValDef;
               Delse_mm_lerp(mmdst.modseq_patch[1]);
               Dsignaltap(mmdst.modseq_patch[1]);
               break;

               case STSAMPLE_MM_DST_MODSEQ2_SPEED:
               if(bModSeqGlobal[1])
               {
                  if(bAutoMul)
                     sample->modseq_global[1].mmdst_speed *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
                  else if(bAutoAdd)
                     sample->modseq_global[1].mmdst_speed += srcValDef * 8.0f;
                  Delse_mm_lerp_bipolar8(sample->modseq_global[1].mmdst_speed);
                  Dsignaltap(sample->modseq_global[1].mmdst_speed);
               }
               else
               {
                  if(bAutoMul)
                     modseq[1].mmdst_speed *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
                  else if(bAutoAdd)
                     modseq[1].mmdst_speed += srcValDef * 8.0f;
                  Delse_mm_lerp_bipolar8(modseq[1].mmdst_speed);
                  Dsignaltap(modseq[1].mmdst_speed);
               }
               break;

               case STSAMPLE_MM_DST_MODSEQ2_SPEED_ABS:
               if(bModSeqGlobal[1])
               {
                  if(bAutoMul)
                     sample->modseq_global[1].mmdst_speed *= srcValDef;
                  else if(bAutoAdd)
                     sample->modseq_global[1].mmdst_speed += srcValDef;
                  Delse_mm_lerp(sample->modseq_global[1].mmdst_speed);
                  Dsignaltap(sample->modseq_global[1].mmdst_speed);
               }
               else
               {
                  if(bAutoMul)
                     modseq[1].mmdst_speed *= srcValDef;
                  else if(bAutoAdd)
                     modseq[1].mmdst_speed += srcValDef;
                  Delse_mm_lerp(modseq[1].mmdst_speed);
                  Dsignaltap(modseq[1].mmdst_speed);
               }
               break;

               case STSAMPLE_MM_DST_MODSEQ2_LEVEL:
               if(bAutoMul)
                  mmdst.modseq_level[1] *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
               else if(bAutoAdd)
                  mmdst.modseq_level[1] += srcValDef * 8.0f;
               Delse_mm_lerp_bipolar8(mmdst.modseq_level[1]);
               Dsignaltap(mmdst.modseq_level[1]);
               break;

               case STSAMPLE_MM_DST_MODSEQ2_NUMSTEPS:
               if(bModSeqGlobal[1])
               {
                  if(bAutoAdd)
                     sample->modseq_global[1].mmdst_numsteps += srcValDef;
                  else if(bAutoMul)
                     sample->modseq_global[1].mmdst_numsteps *= srcValDef;
                  Delse_mm_lerp_scl(sample->modseq_global[1].mmdst_numsteps, 1.0f);
                  Dsignaltap(sample->modseq_global[1].mmdst_numsteps);
               }
               else
               {
                  if(bAutoAdd)
                     modseq[1].mmdst_numsteps += srcValDef;
                  else if(bAutoMul)
                     modseq[1].mmdst_numsteps *= srcValDef;
                  Delse_mm_lerp_scl(modseq[1].mmdst_numsteps, 1.0f);
                  Dsignaltap(modseq[1].mmdst_numsteps);
               }
               break;

               case STSAMPLE_MM_DST_MODSEQ2_ADVANCE:
               if(bModSeqGlobal[1])
               {
                  if(bAutoAdd)
                     sample->modseq_global[1].mmdst_advance += srcValDef;
                  else if(bAutoMul)
                     sample->modseq_global[1].mmdst_advance *= srcValDef;
                  Delse_mm_lerp_scl(sample->modseq_global[0].mmdst_advance, 1.0f);
                  Dsignaltap(sample->modseq_global[0].mmdst_advance);
               }
               else
               {
                  if(bAutoAdd)
                     modseq[1].mmdst_advance += srcValDef;
                  else if(bAutoMul)
                     modseq[1].mmdst_advance *= srcValDef;
                  Delse_mm_lerp_scl(modseq[1].mmdst_advance, 1.0f);
                  Dsignaltap(modseq[1].mmdst_advance);
               }
               break;

               case STSAMPLE_MM_DST_MODSEQ2_STEP:
               if(bModSeqGlobal[1])
               {
                  if(bAutoAdd)
                     sample->modseq_global[1].mmdst_step += srcValDef;
                  else if(bAutoMul)
                     sample->modseq_global[1].mmdst_step *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
                  Delse_mm_lerp_scl(sample->modseq_global[1].mmdst_step, 1.0f);
                  Dsignaltap(sample->modseq_global[1].mmdst_step);
                  sample->modseq_global[1].b_mmdst_step = YAC_TRUE;
               }
               else
               {
                  if(bAutoAdd)
                     modseq[1].mmdst_step += srcValDef;
                  else if(bAutoMul)
                     modseq[1].mmdst_step *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
                  Delse_mm_lerp_scl(modseq[1].mmdst_step, 1.0f);
                  Dsignaltap(modseq[1].mmdst_step);
                  modseq[1].b_mmdst_step = YAC_TRUE;
               }
               break;

               case STSAMPLE_MM_DST_MODSEQ3_PATCH:
               if(bAutoRep)
                  mmdst.modseq_patch[2] = srcValDef;
               else if(bAutoAdd)
                  mmdst.modseq_patch[2] += srcValDef;
               else if(bAutoMul)
                  mmdst.modseq_patch[2] *= srcValDef;
               Delse_mm_lerp(mmdst.modseq_patch[2]);
               Dsignaltap(mmdst.modseq_patch[2]);
               break;

               case STSAMPLE_MM_DST_MODSEQ3_SPEED:
               if(bModSeqGlobal[2])
               {
                  if(bAutoMul)
                     sample->modseq_global[2].mmdst_speed *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
                  else if(bAutoAdd)
                     sample->modseq_global[2].mmdst_speed += srcValDef * 8.0f;
                  Delse_mm_lerp_bipolar8(sample->modseq_global[2].mmdst_speed);
                  Dsignaltap(sample->modseq_global[2].mmdst_speed);
               }
               else
               {
                  if(bAutoMul)
                     modseq[2].mmdst_speed *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
                  else if(bAutoAdd)
                     modseq[2].mmdst_speed += srcValDef * 8.0f;
                  Delse_mm_lerp_bipolar8(modseq[2].mmdst_speed);
                  Dsignaltap(modseq[2].mmdst_speed);
               }
               break;

               case STSAMPLE_MM_DST_MODSEQ3_SPEED_ABS:
               if(bModSeqGlobal[2])
               {
                  if(bAutoMul)
                     sample->modseq_global[2].mmdst_speed *= srcValDef;
                  else if(bAutoAdd)
                     sample->modseq_global[2].mmdst_speed += srcValDef;
                  Delse_mm_lerp(sample->modseq_global[2].mmdst_speed);
                  Dsignaltap(sample->modseq_global[2].mmdst_speed);
               }
               else
               {
                  if(bAutoMul)
                     modseq[2].mmdst_speed *= srcValDef;
                  else if(bAutoAdd)
                     modseq[2].mmdst_speed += srcValDef;
                  Delse_mm_lerp(modseq[2].mmdst_speed);
                  Dsignaltap(modseq[2].mmdst_speed);
               }
               break;

               case STSAMPLE_MM_DST_MODSEQ3_LEVEL:
               if(bAutoMul)
                  mmdst.modseq_level[2] *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
               else if(bAutoAdd)
                  mmdst.modseq_level[2] += srcValDef * 8.0f;
               Delse_mm_lerp_bipolar8(mmdst.modseq_level[2]);
               Dsignaltap(mmdst.modseq_level[2]);
               break;

               case STSAMPLE_MM_DST_MODSEQ3_NUMSTEPS:
               if(bModSeqGlobal[2])
               {
                  if(bAutoAdd)
                     sample->modseq_global[2].mmdst_numsteps += srcValDef;
                  else if(bAutoMul)
                     sample->modseq_global[2].mmdst_numsteps *= srcValDef;
                  Delse_mm_lerp_scl(sample->modseq_global[2].mmdst_numsteps, 1.0f);
                  Dsignaltap(sample->modseq_global[2].mmdst_numsteps);
               }
               else
               {
                  if(bAutoAdd)
                     modseq[2].mmdst_numsteps += srcValDef;
                  else if(bAutoMul)
                     modseq[2].mmdst_numsteps *= srcValDef;
                  Delse_mm_lerp_scl(modseq[2].mmdst_numsteps, 1.0f);
                  Dsignaltap(modseq[2].mmdst_numsteps);
               }
               break;

               case STSAMPLE_MM_DST_MODSEQ3_ADVANCE:
               if(bModSeqGlobal[2])
               {
                  if(bAutoAdd)
                     sample->modseq_global[2].mmdst_advance += srcValDef;
                  else if(bAutoMul)
                     sample->modseq_global[2].mmdst_advance *= srcValDef;
                  Delse_mm_lerp_scl(sample->modseq_global[2].mmdst_advance, 1.0f);
                  Dsignaltap(sample->modseq_global[2].mmdst_advance);
               }
               else
               {
                  if(bAutoAdd)
                     modseq[2].mmdst_advance += srcValDef;
                  else if(bAutoMul)
                     modseq[2].mmdst_advance *= srcValDef;
                  Delse_mm_lerp_scl(modseq[2].mmdst_advance, 1.0f);
                  Dsignaltap(modseq[2].mmdst_advance);
               }
               break;

               case STSAMPLE_MM_DST_MODSEQ3_STEP:
               if(bModSeqGlobal[2])
               {
                  if(bAutoAdd)
                     sample->modseq_global[2].mmdst_step += srcValDef;
                  else if(bAutoMul)
                     sample->modseq_global[2].mmdst_step *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
                  Delse_mm_lerp_scl(sample->modseq_global[2].mmdst_step, 1.0f);
                  Dsignaltap(sample->modseq_global[2].mmdst_step);
                  sample->modseq_global[2].b_mmdst_step = YAC_TRUE;
               }
               else
               {
                  if(bAutoAdd)
                     modseq[2].mmdst_step += srcValDef;
                  else if(bAutoMul)
                     modseq[2].mmdst_step *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
                  Delse_mm_lerp_scl(modseq[2].mmdst_step, 1.0f);
                  Dsignaltap(modseq[2].mmdst_step);
                  modseq[2].b_mmdst_step = YAC_TRUE;
               }
               break;

               case STSAMPLE_MM_DST_MODSEQ4_PATCH:
               if(bAutoRep)
                  mmdst.modseq_patch[3] = srcValDef;
               else if(bAutoAdd)
                  mmdst.modseq_patch[3] += srcValDef;
               else if(bAutoMul)
                  mmdst.modseq_patch[3] *= srcValDef;
               Delse_mm_lerp(mmdst.modseq_patch[3]);
               Dsignaltap(mmdst.modseq_patch[3]);
               break;

               case STSAMPLE_MM_DST_MODSEQ4_SPEED:
               if(bModSeqGlobal[3])
               {
                  if(bAutoMul)
                     sample->modseq_global[3].mmdst_speed *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
                  else if(bAutoAdd)
                     sample->modseq_global[3].mmdst_speed += srcValDef * 8.0f;
                  Delse_mm_lerp_bipolar8(sample->modseq_global[3].mmdst_speed);
                  Dsignaltap(sample->modseq_global[3].mmdst_speed);
               }
               else
               {
                  if(bAutoMul)
                     modseq[3].mmdst_speed *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
                  else if(bAutoAdd)
                     modseq[3].mmdst_speed += srcValDef * 8.0f;
                  Delse_mm_lerp_bipolar8(modseq[3].mmdst_speed);
                  Dsignaltap(modseq[3].mmdst_speed);
               }
               break;

               case STSAMPLE_MM_DST_MODSEQ4_SPEED_ABS:
               if(bModSeqGlobal[3])
               {
                  if(bAutoMul)
                     sample->modseq_global[3].mmdst_speed *= srcValDef;
                  else if(bAutoAdd)
                     sample->modseq_global[3].mmdst_speed += srcValDef;
                  Delse_mm_lerp(sample->modseq_global[3].mmdst_speed);
                  Dsignaltap(sample->modseq_global[3].mmdst_speed);
               }
               else
               {
                  if(bAutoMul)
                     modseq[3].mmdst_speed *= srcValDef;
                  else if(bAutoAdd)
                     modseq[3].mmdst_speed += srcValDef;
                  Delse_mm_lerp(modseq[3].mmdst_speed);
                  Dsignaltap(modseq[3].mmdst_speed);
               }
               break;

               case STSAMPLE_MM_DST_MODSEQ4_LEVEL:
               if(bAutoMul)
                  mmdst.modseq_level[3] *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
               else if(bAutoAdd)
                  mmdst.modseq_level[3] += srcValDef * 8.0f;
               Delse_mm_lerp_bipolar8(mmdst.modseq_level[3]);
               Dsignaltap(mmdst.modseq_level[3]);
               break;

               case STSAMPLE_MM_DST_MODSEQ4_NUMSTEPS:
               if(bModSeqGlobal[3])
               {
                  if(bAutoAdd)
                     sample->modseq_global[3].mmdst_numsteps += srcValDef;
                  else if(bAutoMul)
                     sample->modseq_global[3].mmdst_numsteps *= srcValDef;
                  Delse_mm_lerp_scl(sample->modseq_global[3].mmdst_numsteps, 1.0f);
                  Dsignaltap(sample->modseq_global[3].mmdst_numsteps);
               }
               else
               {
                  if(bAutoAdd)
                     modseq[3].mmdst_numsteps += srcValDef;
                  else if(bAutoMul)
                     modseq[3].mmdst_numsteps *= srcValDef;
                  Delse_mm_lerp_scl(modseq[3].mmdst_numsteps, 1.0f);
                  Dsignaltap(modseq[3].mmdst_numsteps);
               }
               break;

               case STSAMPLE_MM_DST_MODSEQ4_ADVANCE:
               if(bModSeqGlobal[3])
               {
                  if(bAutoAdd)
                     sample->modseq_global[3].mmdst_advance += srcValDef;
                  else if(bAutoMul)
                     sample->modseq_global[3].mmdst_advance *= srcValDef;
                  Delse_mm_lerp_scl(sample->modseq_global[3].mmdst_advance, 1.0f);
                  Dsignaltap(sample->modseq_global[3].mmdst_advance);
               }
               else
               {
                  if(bAutoAdd)
                     modseq[3].mmdst_advance += srcValDef;
                  else if(bAutoMul)
                     modseq[3].mmdst_advance *= srcValDef;
                  Delse_mm_lerp_scl(modseq[3].mmdst_advance, 1.0f);
                  Dsignaltap(modseq[3].mmdst_advance);
               }
               break;

               case STSAMPLE_MM_DST_MODSEQ4_STEP:
               if(bModSeqGlobal[3])
               {
                  if(bAutoAdd)
                     sample->modseq_global[3].mmdst_step += srcValDef;
                  else if(bAutoMul)
                     sample->modseq_global[3].mmdst_step *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
                  Delse_mm_lerp_scl(sample->modseq_global[3].mmdst_step, 1.0f);
                  Dsignaltap(sample->modseq_global[3].mmdst_step);
                  sample->modseq_global[3].b_mmdst_step = YAC_TRUE;
               }
               else
               {
                  if(bAutoAdd)
                     modseq[3].mmdst_step += srcValDef;
                  else if(bAutoMul)
                     modseq[3].mmdst_step *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
                  Delse_mm_lerp_scl(modseq[3].mmdst_step, 1.0f);
                  Dsignaltap(modseq[3].mmdst_step);
                  modseq[3].b_mmdst_step = YAC_TRUE;
               }
               break;

               case STSAMPLE_MM_DST_RETRIG_FREQ_ENV:
               if(bAutoAdd)
                  mmdst.retrig_freq_env += srcValDef;
               else if(bAutoMul)
                  mmdst.retrig_freq_env *= srcValDef;
               Delse_mm_lerp_scl(mmdst.retrig_freq_env, 1.0f);
               Dsignaltap(mmdst.retrig_freq_env);
               break;

               case STSAMPLE_MM_DST_RETRIG_VOL_ENV:
               if(bAutoAdd)
                  mmdst.retrig_vol_env += srcValDef;
               else if(bAutoMul)
                  mmdst.retrig_vol_env *= srcValDef;
               Delse_mm_lerp_scl(mmdst.retrig_vol_env, 1.0f);
               Dsignaltap(mmdst.retrig_vol_env);
               break;

               case STSAMPLE_MM_DST_RETRIG_PAN_ENV:
               if(bAutoAdd)
                  mmdst.retrig_pan_env += srcValDef;
               else if(bAutoMul)
                  mmdst.retrig_pan_env *= srcValDef;
               Delse_mm_lerp_scl(mmdst.retrig_pan_env, 1.0f);
               Dsignaltap(mmdst.retrig_pan_env);
               break;

               case STSAMPLE_MM_DST_RETRIG_AUX_ENV:
               if(bAutoAdd)
                  mmdst.retrig_aux_env += srcValDef;
               else if(bAutoMul)
                  mmdst.retrig_aux_env *= srcValDef;
               Delse_mm_lerp_scl(mmdst.retrig_aux_env, 1.0f);
               Dsignaltap(mmdst.retrig_aux_env);
               break;

               case STSAMPLE_MM_DST_RETRIG_FREQ_LFO:
               if(bAutoAdd)
                  mmdst.retrig_freq_lfo += srcValDef;
               else if(bAutoMul)
                  mmdst.retrig_freq_lfo *= srcValDef;
               Delse_mm_lerp_scl(mmdst.retrig_freq_lfo, 1.0f);
               Dsignaltap(mmdst.retrig_freq_lfo);
               break;

               case STSAMPLE_MM_DST_RETRIG_VOL_LFO:
               if(bAutoAdd)
                  mmdst.retrig_vol_lfo += srcValDef;
               else if(bAutoMul)
                  mmdst.retrig_vol_lfo *= srcValDef;
               Delse_mm_lerp_scl(mmdst.retrig_vol_lfo, 1.0f);
               Dsignaltap(mmdst.retrig_vol_lfo);
               break;

               case STSAMPLE_MM_DST_RETRIG_PAN_LFO:
               if(bAutoAdd)
                  mmdst.retrig_pan_lfo += srcValDef;
               else if(bAutoMul)
                  mmdst.retrig_pan_lfo *= srcValDef;
               Delse_mm_lerp_scl(mmdst.retrig_pan_lfo, 1.0f);
               Dsignaltap(mmdst.retrig_pan_lfo);
               break;

               case STSAMPLE_MM_DST_RETRIG_AUX_LFO:
               if(bAutoAdd)
                  mmdst.retrig_aux_lfo += srcValDef;
               else if(bAutoMul)
                  mmdst.retrig_aux_lfo *= srcValDef;
               Delse_mm_lerp_scl(mmdst.retrig_aux_lfo, 1.0f);
               Dsignaltap(mmdst.retrig_aux_lfo);
               break;

               case STSAMPLE_MM_DST_RETRIG_MODSEQ1:
               if(bAutoAdd)
                  mmdst.retrig_modseq[0] += srcValDef;
               else if(bAutoMul)
                  mmdst.retrig_modseq[0] *= srcValDef;
               Delse_mm_lerp_scl(mmdst.retrig_modseq[0], 1.0f);
               Dsignaltap(mmdst.retrig_modseq[0]);
               break;

               case STSAMPLE_MM_DST_RETRIG_MODSEQ2:
               if(bAutoAdd)
                  mmdst.retrig_modseq[1] += srcValDef;
               else if(bAutoMul)
                  mmdst.retrig_modseq[1] *= srcValDef;
               Delse_mm_lerp_scl(mmdst.retrig_modseq[1], 1.0f);
               Dsignaltap(mmdst.retrig_modseq[1]);
               break;

               case STSAMPLE_MM_DST_RETRIG_MODSEQ3:
               if(bAutoAdd)
                  mmdst.retrig_modseq[2] += srcValDef;
               else if(bAutoMul)
                  mmdst.retrig_modseq[2] *= srcValDef;
               Delse_mm_lerp_scl(mmdst.retrig_modseq[2], 1.0f);
               Dsignaltap(mmdst.retrig_modseq[2]);
               break;

               case STSAMPLE_MM_DST_RETRIG_MODSEQ4:
               if(bAutoAdd)
                  mmdst.retrig_modseq[3] += srcValDef;
               else if(bAutoMul)
                  mmdst.retrig_modseq[3] *= srcValDef;
               Delse_mm_lerp_scl(mmdst.retrig_modseq[3], 1.0f);
               Dsignaltap(mmdst.retrig_modseq[3]);
               break;

               case STSAMPLE_MM_DST_REG_1_ON:
               // Dyac_host_printf("xxx DST_REG_1_ON: mmdst_reg[0]=%f replay_ticks=%u bAutoAdd=%d bAutoMul=%d srcValDef=%f\n", mmdst_reg[0], replay_ticks, bAutoAdd, bAutoMul, srcValDef);
               if(0u == replay_ticks)
               {
                  if(bAutoAdd)
                     mmdst_reg[0] += srcValDef;
                  else if(bAutoMul)
                     mmdst_reg[0] *= srcValDef;
                  Delse_mm_lerp_scl(mmdst_reg[0], 1.0f);
                  Dsignaltap(mmdst_reg[0]);
               }
               break;

               case STSAMPLE_MM_DST_REG_2_ON:
               if(0u == replay_ticks)
               {
                  if(bAutoAdd)
                     mmdst_reg[1] += srcValDef;
                  else if(bAutoMul)
                     mmdst_reg[1] *= srcValDef;
                  Delse_mm_lerp_scl(mmdst_reg[1], 1.0f);
                  Dsignaltap(mmdst_reg[1]);
               }
               break;

               case STSAMPLE_MM_DST_REG_3_ON:
               if(0u == replay_ticks)
               {
                  if(bAutoAdd)
                     mmdst_reg[2] += srcValDef;
                  else if(bAutoMul)
                     mmdst_reg[2] *= srcValDef;
                  Delse_mm_lerp_scl(mmdst_reg[2], 1.0f);
                  Dsignaltap(mmdst_reg[2]);
               }
               break;

               case STSAMPLE_MM_DST_REG_4_ON:
               if(0u == replay_ticks)
               {
                  if(bAutoAdd)
                     mmdst_reg[3] += srcValDef;
                  else if(bAutoMul)
                     mmdst_reg[3] *= srcValDef;
                  Delse_mm_lerp_scl(mmdst_reg[3], 1.0f);
                  Dsignaltap(mmdst_reg[3]);
               }
               break;

               case STSAMPLE_MM_DST_REG_1:
               if(bAutoAdd)
                  mmdst_reg[0] += srcValDef;
               else if(bAutoMul)
                  mmdst_reg[0] *= srcValDef;
               Delse_mm_lerp_scl(mmdst_reg[0], 1.0f);
               Dsignaltap(mmdst_reg[0]);
               break;

               case STSAMPLE_MM_DST_REG_2:
               if(bAutoAdd)
                  mmdst_reg[1] += srcValDef;
               else if(bAutoMul)
                  mmdst_reg[1] *= srcValDef;
               Delse_mm_lerp_scl(mmdst_reg[1], 1.0f);
               Dsignaltap(mmdst_reg[1]);
               break;

               case STSAMPLE_MM_DST_REG_3:
               if(bAutoAdd)
                  mmdst_reg[2] += srcValDef;
               else if(bAutoMul)
                  mmdst_reg[2] *= srcValDef;
               Delse_mm_lerp_scl(mmdst_reg[2], 1.0f);
               Dsignaltap(mmdst_reg[2]);
               break;

               case STSAMPLE_MM_DST_REG_4:
               if(bAutoAdd)
                  mmdst_reg[3] += srcValDef;
               else if(bAutoMul)
                  mmdst_reg[3] *= srcValDef;
               Delse_mm_lerp_scl(mmdst_reg[3], 1.0f);
               Dsignaltap(mmdst_reg[3]);
               break;

               case STSAMPLE_MM_DST_LIVEREC_START:
               if(bAutoAdd)
                  mmdst.liverec_start += srcValDef;
               else if(bAutoMul)
                  mmdst.liverec_start *= srcValDef;
               Delse_mm_lerp_scl(mmdst.liverec_start, 1.0f);
               Dsignaltap(mmdst.liverec_start);
               break;

               case STSAMPLE_MM_DST_LIVEREC_CONTINUE:
               if(bAutoAdd)
                  mmdst.liverec_continue += srcValDef;
               else if(bAutoMul)
                  mmdst.liverec_continue *= srcValDef;
               Delse_mm_lerp(mmdst.liverec_continue);
               Dsignaltap(mmdst.liverec_continue);
               break;

               case STSAMPLE_MM_DST_LIVEREC_STOP:
               if(bAutoAdd)
                  mmdst.liverec_stop += srcValDef;
               else if(bAutoMul)
                  mmdst.liverec_stop *= srcValDef;
               Delse_mm_lerp_scl(mmdst.liverec_stop, 1.0f);
               Dsignaltap(mmdst.liverec_stop);
               break;

               case STSAMPLE_MM_DST_PLUGIN_1_MOD_1:
               if(bAutoAdd)
                  mmdst.plugins[0][0] += srcValDef;
               else if(bAutoMul)
                  mmdst.plugins[0][0] *= srcValDef;
               Delse_mm_lerp_scl(mmdst.plugins[0][0], 1.0f);
               Dsignaltap(mmdst.plugins[0][0]);
               mmdst.plugin_mod_mask |= (1u << (8u*0 + 0u));
               break;

               case STSAMPLE_MM_DST_PLUGIN_1_MOD_2:
               if(bAutoAdd)
                  mmdst.plugins[0][1] += srcValDef;
               else if(bAutoMul)
                  mmdst.plugins[0][1] *= srcValDef;
               Delse_mm_lerp_scl(mmdst.plugins[0][1], 1.0f);
               Dsignaltap(mmdst.plugins[0][1]);
               mmdst.plugin_mod_mask |= (1u << (8u*0 + 1u));
               break;

               case STSAMPLE_MM_DST_PLUGIN_1_MOD_3:
               if(bAutoAdd)
                  mmdst.plugins[0][2] += srcValDef;
               else if(bAutoMul)
                  mmdst.plugins[0][2] *= srcValDef;
               Delse_mm_lerp_scl(mmdst.plugins[0][2], 1.0f);
               Dsignaltap(mmdst.plugins[0][2]);
               mmdst.plugin_mod_mask |= (1u << (8u*0 + 2u));
               break;

               case STSAMPLE_MM_DST_PLUGIN_1_MOD_4:
               if(bAutoAdd)
                  mmdst.plugins[0][3] += srcValDef;
               else if(bAutoMul)
                  mmdst.plugins[0][3] *= srcValDef;
               Delse_mm_lerp_scl(mmdst.plugins[0][3], 1.0f);
               Dsignaltap(mmdst.plugins[0][3]);
               mmdst.plugin_mod_mask |= (1u << (8u*0 + 3u));
               break;

               case STSAMPLE_MM_DST_PLUGIN_1_MOD_5:
               if(bAutoAdd)
                  mmdst.plugins[0][4] += srcValDef;
               else if(bAutoMul)
                  mmdst.plugins[0][4] *= srcValDef;
               Delse_mm_lerp_scl(mmdst.plugins[0][4], 1.0f);
               Dsignaltap(mmdst.plugins[0][4]);
               mmdst.plugin_mod_mask |= (1u << (8u*0 + 4u));
               break;

               case STSAMPLE_MM_DST_PLUGIN_1_MOD_6:
               if(bAutoAdd)
                  mmdst.plugins[0][5] += srcValDef;
               else if(bAutoMul)
                  mmdst.plugins[0][5] *= srcValDef;
               Delse_mm_lerp_scl(mmdst.plugins[0][5], 1.0f);
               Dsignaltap(mmdst.plugins[0][5]);
               mmdst.plugin_mod_mask |= (1u << (8u*0 + 5u));
               break;

               case STSAMPLE_MM_DST_PLUGIN_1_MOD_7:
               if(bAutoAdd)
                  mmdst.plugins[0][6] += srcValDef;
               else if(bAutoMul)
                  mmdst.plugins[0][6] *= srcValDef;
               Delse_mm_lerp_scl(mmdst.plugins[0][6], 1.0f);
               Dsignaltap(mmdst.plugins[0][6]);
               mmdst.plugin_mod_mask |= (1u << (8u*0 + 6u));
               break;

               case STSAMPLE_MM_DST_PLUGIN_1_MOD_8:
               if(bAutoAdd)
                  mmdst.plugins[0][7] += srcValDef;
               else if(bAutoMul)
                  mmdst.plugins[0][7] *= srcValDef;
               Delse_mm_lerp_scl(mmdst.plugins[0][7], 1.0f);
               Dsignaltap(mmdst.plugins[0][7]);
               mmdst.plugin_mod_mask |= (1u << (8u*0 + 7u));
               break;

               case STSAMPLE_MM_DST_PLUGIN_2_MOD_1:
               if(bAutoAdd)
                  mmdst.plugins[1][0] += srcValDef;
               else if(bAutoMul)
                  mmdst.plugins[1][0] *= srcValDef;
               Delse_mm_lerp_scl(mmdst.plugins[1][0], 1.0f);
               Dsignaltap(mmdst.plugins[1][0]);
               mmdst.plugin_mod_mask |= (1u << (8u*1 + 0u));
               break;

               case STSAMPLE_MM_DST_PLUGIN_2_MOD_2:
               if(bAutoAdd)
                  mmdst.plugins[1][1] += srcValDef;
               else if(bAutoMul)
                  mmdst.plugins[1][1] *= srcValDef;
               Delse_mm_lerp_scl(mmdst.plugins[1][1], 1.0f);
               Dsignaltap(mmdst.plugins[1][1]);
               mmdst.plugin_mod_mask |= (1u << (8u*1 + 1u));
               break;

               case STSAMPLE_MM_DST_PLUGIN_2_MOD_3:
               if(bAutoAdd)
                  mmdst.plugins[1][2] += srcValDef;
               else if(bAutoMul)
                  mmdst.plugins[1][2] *= srcValDef;
               Delse_mm_lerp_scl(mmdst.plugins[1][2], 1.0f);
               Dsignaltap(mmdst.plugins[1][2]);
               mmdst.plugin_mod_mask |= (1u << (8u*1 + 2u));
               break;

               case STSAMPLE_MM_DST_PLUGIN_2_MOD_4:
               if(bAutoAdd)
                  mmdst.plugins[1][3] += srcValDef;
               else if(bAutoMul)
                  mmdst.plugins[1][3] *= srcValDef;
               Delse_mm_lerp_scl(mmdst.plugins[1][3], 1.0f);
               Dsignaltap(mmdst.plugins[1][3]);
               mmdst.plugin_mod_mask |= (1u << (8u*1 + 3u));
               break;

               case STSAMPLE_MM_DST_PLUGIN_2_MOD_5:
               if(bAutoAdd)
                  mmdst.plugins[1][4] += srcValDef;
               else if(bAutoMul)
                  mmdst.plugins[1][4] *= srcValDef;
               Delse_mm_lerp_scl(mmdst.plugins[1][4], 1.0f);
               Dsignaltap(mmdst.plugins[1][4]);
               mmdst.plugin_mod_mask |= (1u << (8u*1 + 4u));
               break;

               case STSAMPLE_MM_DST_PLUGIN_2_MOD_6:
               if(bAutoAdd)
                  mmdst.plugins[1][5] += srcValDef;
               else if(bAutoMul)
                  mmdst.plugins[1][5] *= srcValDef;
               Delse_mm_lerp_scl(mmdst.plugins[1][5], 1.0f);
               Dsignaltap(mmdst.plugins[1][5]);
               mmdst.plugin_mod_mask |= (1u << (8u*1 + 5u));
               break;

               case STSAMPLE_MM_DST_PLUGIN_2_MOD_7:
               if(bAutoAdd)
                  mmdst.plugins[1][6] += srcValDef;
               else if(bAutoMul)
                  mmdst.plugins[1][6] *= srcValDef;
               Delse_mm_lerp_scl(mmdst.plugins[1][6], 1.0f);
               Dsignaltap(mmdst.plugins[1][6]);
               mmdst.plugin_mod_mask |= (1u << (8u*1 + 6u));
               break;

               case STSAMPLE_MM_DST_PLUGIN_2_MOD_8:
               if(bAutoAdd)
                  mmdst.plugins[1][7] += srcValDef;
               else if(bAutoMul)
                  mmdst.plugins[1][7] *= srcValDef;
               Delse_mm_lerp_scl(mmdst.plugins[1][7], 1.0f);
               Dsignaltap(mmdst.plugins[1][7]);
               mmdst.plugin_mod_mask |= (1u << (8u*1 + 7u));
               break;

               case STSAMPLE_MM_DST_PLUGIN_3_MOD_1:
               if(bAutoAdd)
                  mmdst.plugins[2][0] += srcValDef;
               else if(bAutoMul)
                  mmdst.plugins[2][0] *= srcValDef;
               Delse_mm_lerp_scl(mmdst.plugins[2][0], 1.0f);
               Dsignaltap(mmdst.plugins[2][0]);
               mmdst.plugin_mod_mask |= (1u << (8u*2 + 0u));
               break;

               case STSAMPLE_MM_DST_PLUGIN_3_MOD_2:
               if(bAutoAdd)
                  mmdst.plugins[2][1] += srcValDef;
               else if(bAutoMul)
                  mmdst.plugins[2][1] *= srcValDef;
               Delse_mm_lerp_scl(mmdst.plugins[2][1], 1.0f);
               Dsignaltap(mmdst.plugins[2][1]);
               mmdst.plugin_mod_mask |= (1u << (8u*2 + 1u));
               break;

               case STSAMPLE_MM_DST_PLUGIN_3_MOD_3:
               if(bAutoAdd)
                  mmdst.plugins[2][2] += srcValDef;
               else if(bAutoMul)
                  mmdst.plugins[2][2] *= srcValDef;
               Delse_mm_lerp_scl(mmdst.plugins[2][2], 1.0f);
               Dsignaltap(mmdst.plugins[2][2]);
               mmdst.plugin_mod_mask |= (1u << (8u*2 + 2u));
               break;

               case STSAMPLE_MM_DST_PLUGIN_3_MOD_4:
               if(bAutoAdd)
                  mmdst.plugins[2][3] += srcValDef;
               else if(bAutoMul)
                  mmdst.plugins[2][3] *= srcValDef;
               Delse_mm_lerp_scl(mmdst.plugins[2][3], 1.0f);
               Dsignaltap(mmdst.plugins[2][3]);
               mmdst.plugin_mod_mask |= (1u << (8u*2 + 3u));
               break;

               case STSAMPLE_MM_DST_PLUGIN_3_MOD_5:
               if(bAutoAdd)
                  mmdst.plugins[2][4] += srcValDef;
               else if(bAutoMul)
                  mmdst.plugins[2][4] *= srcValDef;
               Delse_mm_lerp_scl(mmdst.plugins[2][4], 1.0f);
               Dsignaltap(mmdst.plugins[2][4]);
               mmdst.plugin_mod_mask |= (1u << (8u*2 + 4u));
               break;

               case STSAMPLE_MM_DST_PLUGIN_3_MOD_6:
               if(bAutoAdd)
                  mmdst.plugins[2][5] += srcValDef;
               else if(bAutoMul)
                  mmdst.plugins[2][5] *= srcValDef;
               Delse_mm_lerp_scl(mmdst.plugins[2][5], 1.0f);
               Dsignaltap(mmdst.plugins[2][5]);
               mmdst.plugin_mod_mask |= (1u << (8u*2 + 5u));
               break;

               case STSAMPLE_MM_DST_PLUGIN_3_MOD_7:
               if(bAutoAdd)
                  mmdst.plugins[2][6] += srcValDef;
               else if(bAutoMul)
                  mmdst.plugins[2][6] *= srcValDef;
               Delse_mm_lerp_scl(mmdst.plugins[2][6], 1.0f);
               Dsignaltap(mmdst.plugins[2][6]);
               mmdst.plugin_mod_mask |= (1u << (8u*2 + 6u));
               break;

               case STSAMPLE_MM_DST_PLUGIN_3_MOD_8:
               if(bAutoAdd)
                  mmdst.plugins[2][7] += srcValDef;
               else if(bAutoMul)
                  mmdst.plugins[2][7] *= srcValDef;
               Delse_mm_lerp_scl(mmdst.plugins[2][7], 1.0f);
               Dsignaltap(mmdst.plugins[2][7]);
               mmdst.plugin_mod_mask |= (1u << (8u*2 + 7u));
               break;

               case STSAMPLE_MM_DST_PLUGIN_4_MOD_1:
               if(bAutoAdd)
                  mmdst.plugins[3][0] += srcValDef;
               else if(bAutoMul)
                  mmdst.plugins[3][0] *= srcValDef;
               Delse_mm_lerp_scl(mmdst.plugins[3][0], 1.0f);
               Dsignaltap(mmdst.plugins[3][0]);
               mmdst.plugin_mod_mask |= (1u << (8u*3 + 0u));
               break;

               case STSAMPLE_MM_DST_PLUGIN_4_MOD_2:
               if(bAutoAdd)
                  mmdst.plugins[3][1] += srcValDef;
               else if(bAutoMul)
                  mmdst.plugins[3][1] *= srcValDef;
               Delse_mm_lerp_scl(mmdst.plugins[3][1], 1.0f);
               Dsignaltap(mmdst.plugins[3][1]);
               mmdst.plugin_mod_mask |= (1u << (8u*3 + 1u));
               break;

               case STSAMPLE_MM_DST_PLUGIN_4_MOD_3:
               if(bAutoAdd)
                  mmdst.plugins[3][2] += srcValDef;
               else if(bAutoMul)
                  mmdst.plugins[3][2] *= srcValDef;
               Delse_mm_lerp_scl(mmdst.plugins[3][2], 1.0f);
               Dsignaltap(mmdst.plugins[3][2]);
               mmdst.plugin_mod_mask |= (1u << (8u*3 + 2u));
               break;

               case STSAMPLE_MM_DST_PLUGIN_4_MOD_4:
               if(bAutoAdd)
                  mmdst.plugins[3][3] += srcValDef;
               else if(bAutoMul)
                  mmdst.plugins[3][3] *= srcValDef;
               Delse_mm_lerp_scl(mmdst.plugins[3][3], 1.0f);
               Dsignaltap(mmdst.plugins[3][3]);
               mmdst.plugin_mod_mask |= (1u << (8u*3 + 3u));
               break;

               case STSAMPLE_MM_DST_PLUGIN_4_MOD_5:
               if(bAutoAdd)
                  mmdst.plugins[3][4] += srcValDef;
               else if(bAutoMul)
                  mmdst.plugins[3][4] *= srcValDef;
               Delse_mm_lerp_scl(mmdst.plugins[3][4], 1.0f);
               Dsignaltap(mmdst.plugins[3][4]);
               mmdst.plugin_mod_mask |= (1u << (8u*3 + 4u));
               break;

               case STSAMPLE_MM_DST_PLUGIN_4_MOD_6:
               if(bAutoAdd)
                  mmdst.plugins[3][5] += srcValDef;
               else if(bAutoMul)
                  mmdst.plugins[3][5] *= srcValDef;
               Delse_mm_lerp_scl(mmdst.plugins[3][5], 1.0f);
               Dsignaltap(mmdst.plugins[3][5]);
               mmdst.plugin_mod_mask |= (1u << (8u*3 + 5u));
               break;

               case STSAMPLE_MM_DST_PLUGIN_4_MOD_7:
               if(bAutoAdd)
                  mmdst.plugins[3][6] += srcValDef;
               else if(bAutoMul)
                  mmdst.plugins[3][6] *= srcValDef;
               Delse_mm_lerp_scl(mmdst.plugins[3][6], 1.0f);
               Dsignaltap(mmdst.plugins[3][6]);
               mmdst.plugin_mod_mask |= (1u << (8u*3 + 6u));
               break;

               case STSAMPLE_MM_DST_PLUGIN_4_MOD_8:
               if(bAutoAdd)
                  mmdst.plugins[3][7] += srcValDef;
               else if(bAutoMul)
                  mmdst.plugins[3][7] *= srcValDef;
               Delse_mm_lerp_scl(mmdst.plugins[3][7], 1.0f);
               Dsignaltap(mmdst.plugins[3][7]);
               mmdst.plugin_mod_mask |= (1u << (8u*3 + 7u));
               break;

               case STSAMPLE_MM_DST_PLUGIN_1_LEVEL:
               if(bAutoMul)
                  mmdst.plugin_levels[0] *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
               else if(bAutoAdd)
                  mmdst.plugin_levels[0] += srcValDef * 8.0f;
               Delse_mm_lerp_bipolar8(mmdst.plugin_levels[0]);
               Dsignaltap(mmdst.plugin_levels[0]);
               break;

               case STSAMPLE_MM_DST_PLUGIN_2_LEVEL:
               if(bAutoMul)
                  mmdst.plugin_levels[1] *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
               else if(bAutoAdd)
                  mmdst.plugin_levels[1] += srcValDef * 8.0f;
               Delse_mm_lerp_bipolar8(mmdst.plugin_levels[1]);
               Dsignaltap(mmdst.plugin_levels[1]);
               break;

               case STSAMPLE_MM_DST_PLUGIN_3_LEVEL:
               if(bAutoMul)
                  mmdst.plugin_levels[2] *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
               else if(bAutoAdd)
                  mmdst.plugin_levels[2] += srcValDef * 8.0f;
               Delse_mm_lerp_bipolar8(mmdst.plugin_levels[2]);
               Dsignaltap(mmdst.plugin_levels[2]);
               break;

               case STSAMPLE_MM_DST_PLUGIN_4_LEVEL:
               if(bAutoMul)
                  mmdst.plugin_levels[3] *= stsamplevoice_bipolar_to_scale(srcValDef, 8.0f/*div*/, 8.0f/*mul*/);
               else if(bAutoAdd)
                  mmdst.plugin_levels[3] += srcValDef * 8.0f;
               Delse_mm_lerp_bipolar8(mmdst.plugin_levels[3]);
               Dsignaltap(mmdst.plugin_levels[3]);
               break;

               case STSAMPLE_MM_DST_SYNC_SPEED:
               if(bAutoAdd)
                  mmdst_sync_speed += srcValDef;
               else if(bAutoMul)
                  mmdst_sync_speed *= srcValDef;
               Delse_mm_lerp(mmdst_sync_speed);
               Dsignaltap(mmdst_sync_speed);
               break;

               case STSAMPLE_MM_DST_SYNC_SLEW:
               if(bAutoMul)
                  mmdst_sync_slew *= srcValDef;
               else if(bAutoAdd)
                  mmdst_sync_slew += srcValDef;
               Delse_mm_lerp(mmdst_sync_slew);
               Dsignaltap(mmdst_sync_slew);
               break;

               case STSAMPLE_MM_DST_SAMPLE_RATE_REDUCTION:
               if(bAutoAdd)
                  mmdst_srr += srcValDef;
               else if(bAutoMul)
                  mmdst_srr *= srcValDef;
               Delse_mm_lerp(mmdst_srr);
               Dsignaltap(mmdst_srr);
               break;

               case STSAMPLE_MM_DST_BIT_REDUCTION:
               if(bAutoAdd)
                  mmdst_br += srcValDef;
               else if(bAutoMul)
                  mmdst_br *= srcValDef;
               Delse_mm_lerp(mmdst_br);
               Dsignaltap(mmdst_br);
               break;

               case STSAMPLE_MM_DST_WT_ADDITIVE_CFG:
               if(bAutoAdd)
                  mmdst_additive_cfg += srcValDef;
               else if(bAutoMul)
                  mmdst_additive_cfg *= srcValDef;
               Delse_mm_lerp(mmdst_additive_cfg);
               Dsignaltap(mmdst_additive_cfg);
               break;

               case STSAMPLE_MM_DST_WT_ADDITIVE_STEREO_SPREAD:
               if(bAutoAdd)
                  mmdst_additive_stereo_spread += srcValDef;
               else if(bAutoMul)
                  mmdst_additive_stereo_spread *= srcValDef;
               Delse_mm_lerp(mmdst_additive_stereo_spread);
               Dsignaltap(mmdst_additive_stereo_spread);
               break;

               case STSAMPLE_MM_DST_WT_ADDITIVE_PARTIALS:
                  if(bAutoAdd)
                     mmdst_additive_num_partials += srcValDef;
                  else if(bAutoMul)
                     mmdst_additive_num_partials *= srcValDef;
                  Delse_mm_lerp(mmdst_additive_num_partials);
                  Dsignaltap(mmdst_additive_num_partials);
                  break;

               case STSAMPLE_MM_DST_VARIATION:
                  if(bAutoRep)
                     mmdst_var = srcValDef;
                  else if(bAutoAdd)
                     mmdst_var += srcValDef;
                  else if(bAutoMul)
                     mmdst_var *= srcValDef;
                  Delse_mm_lerp(mmdst_var);
                  Dsignaltap(mmdst_var);
                  break;
            }
         } // if bSrcValid
      } // if mm->b_enable
   } // loop modmatrix entries

   for(sUI mmIdx = 0u; mmIdx < STSAMPLE_NUM_MODMATRIX_ENTRIES; mmIdx++)
   {
      if(!mmWriteMask[mmIdx])
         modmatrix_amt[mmIdx] = 0.0f;  // e.g. after modmatrix was edited
      // else: keep for next tick()
   }

   mmdst.lfo.freq = mmsrc.lfo.freq * mmdst.lfo_freq_level;
   mmdst.lfo.vol  = mmsrc.lfo.vol  * mmdst.lfo_vol_level;
   mmdst.lfo.pan  = mmsrc.lfo.pan  * mmdst.lfo_pan_level;
   mmdst.lfo.aux  = mmsrc.lfo.aux  * mmdst.lfo_aux_level;

   mmdst.env.freq = mmsrc.env.freq * mmdst.env_freq_level;
   mmdst.env.vol  = mmsrc.env.vol  * mmdst.env_vol_level;
   mmdst.env.pan  = mmsrc.env.pan  * mmdst.env_pan_level;
   mmdst.env.aux  = mmsrc.env.aux  * mmdst.env_aux_level;

   // Update interpolated tuning table (MM_DST_TUNING_TBL_REL)
   if(mmdst.tuning_tbl_rel >= 0.0f)
   {
      sF32 tblIdxF = mmdst.tuning_tbl_rel;
      if(tblIdxF >= sF32(STSAMPLE_MAX_TUNING_TABLES))
         tblIdxF = sF32(STSAMPLE_MAX_TUNING_TABLES - 0.00001f);  // clamp to 15.99999
      sUI tblIdxA = sUI(tblIdxF);
      sUI tblIdxB = tblIdxA + 1u;
      if(tblIdxB >= STSAMPLE_MAX_TUNING_TABLES)
         tblIdxB = 0u;
      sF32 bAmt = tblIdxF - sF32(tblIdxA);

      const sF32 *tblA = sample->tuning_tables[tblIdxA];
      if(NULL == tblA && (NULL != sample->parent_samplebank))
         tblA = sample->parent_samplebank->tuning_tables[tblIdxA];
      if(NULL == tblA)
         tblA = getCurrentFreqTable();

      const sF32 *tblB = sample->tuning_tables[tblIdxB];
      if(NULL == tblB && (NULL != sample->parent_samplebank))
         tblB = sample->parent_samplebank->tuning_tables[tblIdxB];
      if(NULL == tblB)
         tblB = getCurrentFreqTable();

      // yac_host->printf("xxx interpolate freq table. tblIdxA=%u tblIdxB=%u bAmt=%f\n", tblIdxA, tblIdxB, bAmt);

      for(sUI i = 0u; i < 128u; i++)
         interpolated_freq_table[i] = tblA[i] + (tblB[i] - tblA[i]) * bAmt;
   }

   // Update mod sequencers
   for(sUI i = 0u; i< STSAMPLE_NUM_MODSEQ; i++)
   {
      mmdst.modseq[i] = mmsrc.modseq[i] * mmdst.modseq_level[i];
   }

   // 2D Wavetable cycle select
   if(mmdst.b_wt2d_x_valid)
   {
      wt2d_x = sRANGE(mmdst.wt2d_x, 0.0f, 1.0f);
      b_queued_wt2d = YAC_TRUE;
   }
   if(mmdst.b_wt2d_y_valid)
   {
      wt2d_y = sRANGE(mmdst.wt2d_y, 0.0f, 1.0f);
      b_queued_wt2d = YAC_TRUE;
   }

   if(b_mmdst_cyclelen_eol_valid && (mmdst_cyclelen_eol != cur_cyclelen_eol))
   {
      // yac_host->printf("xxx mmdst_cyclelen_eol=%f cur_cyclelen_eol=%f\n", mmdst_cyclelen_eol, cur_cyclelen_eol);
      queued_cyclelen_eol = mmdst_cyclelen_eol;
   }

   // Sample rate reduction
   {
      sSI srr = sSI(mmdst_srr * 127);
      srr = sRANGE(srr, 0, 255);
      smpoff_mask = ~(sUI(srr) + ~sample->smpoff_mask);
   }

   // Bit reduction
   {
      sSI br = sSI(mmdst_br * 16383);
      br += sample->br_mask;
      br = sRANGE(br, 0, 16383);
      br_mask = sU16(br);
   }

   // Update plugin modulation
   updatePluginModulationFromMM(mmdst,
                                (sample->plugin_mask_vm  |
                                 sample->plugin_mask_sr  |
                                 sample->plugin_mask_vb  |
                                 sample->plugin_mask_def |
                                 sample->plugin_mask_am
                                 )
                                );

   b_trig_pulse = YAC_FALSE;
   b_release_pulse = YAC_FALSE;
}
