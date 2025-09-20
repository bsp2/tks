// ----
// ---- file   : StSample.cpp
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
// ----          19Jan2010, 21Jan2010, 27Jan2010, 21Feb2010, 01Mar2010, 29Apr2010, 02May2010
// ----          07Jul2010, 18Jul2010, 04Sep2010, 05Mar2011, 09Mar2011, 14Mar2011, 20Mar2011
// ----          28Mar2011, 08May2013, 09May2013, 23Dec2018, 27Dec2018, 30Dec2018, 31Dec2018
// ----          02Jan2019, 04Jan2019, 07Jan2019, 08Jan2019, 09Jan2019, 30Mar2019, 31Mar2019
// ----          11May2019, 15May2019, 18Jun2019, 13Jul2019, 21Jul2019, 25Jul2019, 29Nov2019
// ----          09Feb2020, 15Feb2020, 17Feb2020, 18Feb2020, 19Feb2020, 20Feb2020, 21Feb2020
// ----          22Feb2020, 23Feb2020, 24Feb2020, 25Feb2020, 26Feb2020, 04Mar2020, 17May2020
// ----          18May2020, 19May2020, 21May2020, 22May2020, 23May2020, 24May2020, 28May2020
// ----          31May2020, 04Jun2020, 08Jun2020, 10Jun2020, 23Aug2020, 31Aug2020, 06Sep2020
// ----          07Sep2020, 26Apr2021, 30Apr2021, 20May2021, 26Jun2021, 27Jul2021, 08Aug2021
// ----          10Aug2021, 30Aug2021, 04Oct2021, 05Oct2021, 20Oct2021, 22Oct2021, 24Oct2021
// ----          28Oct2021, 23Nov2022, 24Nov2022, 20Dec2022, 21Dec2022, 22Dec2022, 23Dec2022
// ----          27Dec2022, 28Dec2022, 31Dec2022, 19Feb2023, 09Mar2023, 10Mar2023, 12Apr2023
// ----          11Aug2023, 16Aug2023, 13Sep2023, 19Sep2023, 22Sep2023, 17Nov2023, 18Nov2023
// ----          10Jan2024, 13Jan2024, 14Jan2024, 15Jan2024, 16Jan2024, 19Apr2024, 04Aug2024
// ----          15Aug2024, 28Sep2024, 30Sep2024, 01Oct2024, 03Oct2024, 05Oct2024, 13Oct2024
// ----          14Oct2024, 08Nov2024, 09Nov2024, 11Dec2024, 03Jan2025
// ----
// ----
// ----

#include "tksampler.h"

#define MAX_VOLRAMP_STEPS 128


StSample::StSample(void) {
   lfsr.init((void*)this);

   next        = NULL;
   name        = NULL;
   waveform    = NULL;
   mutex_group = NULL;

   partial_magnitudes = NULL;
   partial_speeds     = NULL;
   partial_phases     = NULL;
   num_partials       = 32.0f;

   key_range = NULL;
   vel_range = NULL;
   mod_range = NULL;

   sample_loops  = NULL;

   adsr_freq = NULL;
   adsr_vol  = NULL;
   adsr_pan  = NULL;
   adsr_aux = NULL;

   lfo_freq = NULL;
   lfo_vol  = NULL;
   lfo_pan  = NULL;
   lfo_aux  = NULL;
   global_lfo_tick_nr = ~0u;

   ::memset((void*)modseq, 0, sizeof(modseq));
   ::memset((void*)default_modseq_patches, 0, sizeof(default_modseq_patches));
   global_modseq_tick_nr = ~0u;

   wavepath_table = NULL;

   parent_samplebank = NULL;

   ::memset(plugins, 0, sizeof(plugins));
   for(sUI pluginIdx = 0u; pluginIdx < STSAMPLE_NUM_PLUGINS; pluginIdx++)
   {
      plugins[pluginIdx].b_pre_filter = YAC_TRUE;
      plugins[pluginIdx].level[0]     = 1.0f;
      plugins[pluginIdx].level[1]     = 1.0f;
      plugins[pluginIdx].dst          = STSAMPLE_VOICEPLUGIN_DST_DEF;
      plugins[pluginIdx].drive        = 1.0f;
      plugins[pluginIdx].pan          = 0.0f;
   }
   plugin_mask_def = 0u;
   plugin_mask_sr  = 0u;
   plugin_mask_am  = 0u;
   plugin_mask_vb  = 0u;
   plugin_mask_vm  = 0u;
   ui_last_plugin_row_idx = 0u;
   plugin_alg            = 0u;  // 1*2*3*4
   plugin_alg_filter_pos = 0u;  // pre1 ("*1")
   plugin_alg_outer      = 0u;
   plugin_alg_int        = 0u;

   liverec_front_buffer = NULL;
   liverec_front_buffer_sz = 0u;

   liverec_osc_pre_filter_buffer = NULL;

   liverec_skip_input_num_frames  = 64;
   liverec_skip_input_frames_left = 0;

   b_ui_autoselect = YAC_TRUE;

   ::memset(tuning_tables, 0, sizeof(tuning_tables));
   ::memset(tuning_tables_meta, 0, sizeof(tuning_tables_meta));
   default_tuning_table_idx = -1;
   forced_tuning_table_idx = -1;

   b_mmvar_enable         = YAC_FALSE;
   mmvar_num              = 2u;
   b_mmvar_smooth         = YAC_FALSE;
   mmvar_edit_idx         = 0u;
   b_mmvar_force_edit_idx = YAC_FALSE;
   ::memset(mmvar_data, 0, sizeof(mmvar_data));

   reinit();
}

void StSample::reinit(void) {

   b_own_waveform = YAC_FALSE;

   sample_rate_ratio = 1.0f;

   b_skip_range = YAC_FALSE;

   max_voices = STSAMPLE_DEFAULT_MAXVOICES;

   b_enabled = YAC_TRUE;

   sample_delay            = 0.0f;
   sample_delay_multiplier = 1.0f;

   sample_offset = 0u;
   sample_len    = 0u;

   first_loop_index = 0;
   ui_sync_first_loop_index = 0;

   noteoff_loop_index            = -1;
   b_noteoff_immediate_loop_jump = YAC_FALSE;

   b_jumptoloop_immediately = YAC_FALSE;

   play_mode = STSAMPLE_PLAY_MODE_FORWARD;

   startend_fade_num_frames = 0u;

   b_timedloop        = YAC_FALSE;
   b_timedloop_fade   = 1;
   b_oneshot_loopstep = 0;
   timedloop_base     = STSAMPLE_DEFAULT_TIMEDLOOP_BASE;

   volume             = 0.0f;
   vol_lfo_amt        = 1.0f;
   pan                = 0.0f;
   pan_env_amt        = 1.0f;
   pan_lfo_amt        = 1.0f;
   transpose          = 0.0f;
   ui_transpose_oct   = 0;
   ui_transpose_semi  = 0;
   ui_transpose_cents = 0.0f;
   freq_env_amt       = 1.0f;
   freq_lfo_amt       = 1.0f;

   beat_frequency = 0.0f;

   b_lerp_freq = YAC_TRUE;
   b_lerp_vol  = YAC_TRUE;

   volume_velocity_amount                = 0.0f;
   sampleoffset_velocity_amount          = 0.0f;
   b_invert_sampleoffset_velocity_amount = YAC_TRUE;
   sampleoffset_rand_amount              = 0.0f;
   sampleoffset_rel                      = 0.0f;

   b_timestretch                  = YAC_FALSE;
   b_timestretch_additive         = YAC_FALSE;
   b_timestretch_xfade            = YAC_FALSE;
   timestretch                    = 0.0f;
   timestretch_granularity        = 128.0f;
   timestretch_granularity_amount = 0.0f;
   b_static_cyclelen              = YAC_TRUE;
   b_absolute_timestretch         = YAC_TRUE;
   timestretch_additive_xfade     = 0.5f;
   timestretch_interpol_type      = STSAMPLE_TIMESTRETCH_INTERPOL_LINEAR;
   timestretch_grain_window_type  = STSAMPLE_TIMESTRETCH_GRAIN_WINDOW_NONE;
   timestretch_bend               = 0.0f;
   timestretch_2d_w               = 0u;
   timestretch_2d_h               = 0u;

   b_wavepath                     = YAC_FALSE;
   wavepath_idx                   = 0;

   timestretch_startphase_rand_amount = 0.0f;

   num_volramp_steps_in  = 1;
   num_volramp_steps_out = 4;

   b_freeze_sampleoff_when_quiet = YAC_FALSE;
   b_modshiftorig                = YAC_FALSE;
   b_sampleoffnoteon             = YAC_FALSE;

   interpol_type  = STSAMPLE_INTERPOL_LINEAR;
   interpol_order = 0u;
   br_bits        = 0u;
   br_mask        = 0u;
   br_preamp      = 1.0f;
   smpoff_bits    = 0u;
   smpoff_mask    = ~0u;
   b_from_start   = YAC_FALSE;

   alt.b_enable = YAC_FALSE;
   alt.hdr      = NULL;
   alt.num      = 0u;

   b_filter                       = YAC_TRUE;
   filter_type                    = STSAMPLE_FLT_TYPE_NONE;
   // filter_cutoff                  = 0.503937007874f;  // 64 in range 0..127
   filter_cutoff                  = 0.944881889764f;     // 120 in range 0..127
   filter_pan                     = 0.0f;
   filter_offset                  = 0.0f;
   filter_eq_gain                 = 0.0f;
   filter_resonance               = 0.0f;
   filter_aux_env_amount          = 0.0f;
   filter_aux_env_velocity_amount = 0.0f;
   filter_aux_lfo_amount          = 0.0f;
   filter_keyboard_amount         = 0.5f;

   glide_type          = STSAMPLE_GLIDE_NONE;
   glide_speed_time_up = glide_speed_time_down =  1.0f / 90.0f;    // 90ms
   glide_speed_freq_up = glide_speed_freq_down = 24.0f / 1000.0f;  // two octaves / second
   b_glide_release     = YAC_FALSE;
   glide_max_note_dist = 127;
   b_glide_glissando   = YAC_FALSE;
   glide_min_time      = 0u;  // 50u
   glide_max_time      = 0u;  // 0u=inf  250u=reasonable setting for (monophonic) auto-glide

   b_glide_retrig_sample    = YAC_FALSE;
   b_glide_retrig_env_freq  = YAC_FALSE;
   b_glide_retrig_env_vol   = YAC_FALSE;
   b_glide_retrig_env_pan   = YAC_FALSE;
   b_glide_retrig_env_aux   = YAC_FALSE;
   b_glide_retrig_lfo_freq  = YAC_FALSE;
   b_glide_retrig_lfo_vol   = YAC_FALSE;
   b_glide_retrig_lfo_pan   = YAC_FALSE;
   b_glide_retrig_lfo_aux   = YAC_FALSE;

   for(sUI i = 0u; i < STSAMPLE_NUM_MODSEQ; i++)
   {
      b_glide_retrig_modseq[i] = YAC_FALSE;
   }

   mm_keyboard_center = 60.0f/*C-5*/;
   mm_keyboard_min    = 24.0f;
   mm_keyboard_max    = 24.0f;
   ::memset((void*)modmatrix, 0, sizeof(modmatrix));

   b_liverec                           = YAC_FALSE;
   b_liverec_active                    = YAC_FALSE;
   b_liverec_pause                     = YAC_FALSE;
   b_liverec_monitor                   = YAC_FALSE;
   liverec_input_idx                   = 0u;
   b_liverec_stereo                    = YAC_FALSE;
   liverec_input_pan                   = 0.0f;
   liverec_loop_idx                    = 0u;
   b_liverec_copy                      = YAC_FALSE;
   b_liverec_clear                     = YAC_TRUE;
   liverec_doublebuffer_mode           = STSAMPLE_LIVEREC_DBLBUF_MODE_OFF;
   b_liverec_doublebuffer_xfade        = YAC_FALSE;
   liverec_osc_mode                    = STSAMPLE_LIVEREC_OSC_MODE_OFF;
   b_liverec_osc_singlecycle           = YAC_FALSE;
   liverec_osc_tune                    = 12 * 5;  // 12*4 matches center octave on Intellijel Rubicon2 (i.e. 5=center_octave+1)
   b_liverec_osc_sustain               = YAC_FALSE;
   liverec_mode                        = STSAMPLE_LIVEREC_MODE_TRIG;
   liverec_threshold_on                = 0.01f;  // -40dB
   liverec_threshold_off               = 0.001f; // -58dB
   liverec_doublebuffer_rec_idx        = 1u;
   liverec_loop_offset                 = 0u;
   liverec_loop_len                    = 0u;
   liverec_frame_offset                = 0u;
   liverec_num_frames                  = 0u;
   liverec_ring_size                   = 0u;
   liverec_num_ring_frames             = 0u;
   liverec_last_osc_ring_frame_offset  = 0u;
   b_liverec_threshold_reached         = YAC_FALSE;
   b_liverec_silence_reached           = YAC_TRUE;
   ui_redraw_queued                    = YAC_FALSE;
   b_liverec_doublebuffer_swap_queued  = YAC_FALSE;
   liverec_frame_offset_xfade_amt      = 0.0f;
   liverec_override_loop_offset        = -1;
   liverec_override_loop_len           = 0;
   liverec_queued_override_loop_offset = -1;
   liverec_queued_override_loop_len    = 0;
   liverec_ref_cyclelen                = 110.0f;  // need some initial value != 0.0
   liverec_sustain_sample_loops[0].elements = NULL;
   liverec_sustain_sample_loops[1].elements = NULL;
   liverec_osc_fade_amt                = 0.0f;
   liverec_osc_pre_filter              = 1.0f;
   b_liverec_osc_resample              = YAC_FALSE;

   b_sum_input     = YAC_FALSE;
   sum_input_xfade = 0.0f;

   b_enable_fx = YAC_TRUE;

   voice_bus = -1;

   uiSVOffset   = -1.0f;
   uiSVZoom     = 1.0;
   uiSVZoomYIdx = 0u;

   ui_b_auxtowav = YAC_FALSE;

   memset((void*)voice_calibration, 0, sizeof(sF32)*6u*8u);
   for(sUI i = 0u; i < 6u; i++)
   {
      voice_calibration_modulo[i] = 4u;
      voice_calibration_amount[i] = 0.0f;
      voice_calibration_enable[i] = YAC_FALSE;
   }

   additive_edit_cfg = 0.0f;
   b_force_additive_edit_cfg = YAC_TRUE;

   for(sUI j = 0u; j < STSAMPLE_ADDITIVE_CFG_NUM; j++)
   {
      additive_cfg_valid[j] = YAC_FALSE;

      additive_normalize[j] = YAC_TRUE;

      additive_br_bits  [j] = 0u;
      additive_br_mask  [j] = 0u;
      additive_br_preamp[j] = 1.0f;

      additive_smpoff_bits[j] = 0u;
      additive_smpoff_mask[j] = ~0u;

      for(sUI i = 0u; i < STSAMPLE_ADDITIVE_OSC_NUM; i++)
      {
         additive_osc_lvl[j][i] = 0.0f;
         additive_osc_pha[j][i] = 0.0f;
         additive_osc_exa[j][i] = 1.0f;
         additive_osc_flt[j][i] = 1.0f;
         additive_osc_exl[j][i] = 1.0f;
      }
      additive_osc_lvl[j][0] = 1.0f;
      additive_tbl_cfg[j] = StSampleVoice::additive_sin_tbl;
   }
   additive_tbl = StSampleVoice::additive_sin_tbl;

   additive_stereo_spread = 0.0f;
   partial_cyclelen_reset = 128.0f;
   b_partial_phase_0      = YAC_TRUE;

   ai_num_poles = 0u;
   ai_q         = 0.5f;
   ai_exp_up    = 1.0f;
   ai_exp_down  = 0.2f;
   ai_lin_oct   = 3.0f;
   ai_lin_max   = 0.0f;
}

StSample::~StSample() {
   free();

   next = NULL;
}

void StSample::free(void) {
   YAC_DELETE_SAFE(name);
   YAC_DELETE_SAFE(sample_loops);
   YAC_DELETE_SAFE(adsr_freq);
   YAC_DELETE_SAFE(adsr_vol);
   YAC_DELETE_SAFE(adsr_pan);
   YAC_DELETE_SAFE(adsr_aux);
   YAC_DELETE_SAFE(lfo_freq);
   YAC_DELETE_SAFE(lfo_vol);
   YAC_DELETE_SAFE(lfo_pan);
   YAC_DELETE_SAFE(lfo_aux);
   YAC_DELETE_SAFE(key_range);
   YAC_DELETE_SAFE(vel_range);
   YAC_DELETE_SAFE(mod_range);
   YAC_DELETE_SAFE(wavepath_table);

   for(sUI i = 0u; i < STSAMPLE_NUM_MODSEQ; i++)
   {
      for(sUI j = 0u; j < STSAMPLE_MAX_MODSEQ_PATCHES; j++)
      {
         YAC_DELETE_SAFE(modseq[i][j]);
      }
   }

   if((NULL != waveform) && b_own_waveform)
   {
      YAC_DELETE_SAFE(waveform);
   }

   freeLiveRecSustainSampleLoops();

   YAC_DELETE_ARRAY_SAFE(liverec_front_buffer);
   YAC_DELETE_ARRAY_SAFE(liverec_osc_pre_filter_buffer);

   freePlugins();

   _freeTuningTables();
   _freeTuningTablesMetaData();
}

void StSample::freeLiveRecSustainSampleLoops(void) {
   for(sUI i = 0u; i < 2u; i++)
   {
      if(NULL != liverec_sustain_sample_loops[i].elements)
      {
         delete [] liverec_sustain_sample_loops[i].elements;
         liverec_sustain_sample_loops[i].elements = NULL;
      }
   }
}

YAC_Object *StSample::_getNext(void) {
   return next;
}

void StSample::_setNext(YAC_Object *_next) {
   if(YAC_CHK(_next, clid_StSample))
   {
      next = (StSample*) _next;
   }
   else
   {
      next = NULL;
   }
}

void StSample::setSamplePlayerForGlobalModulation(StSamplePlayer *_samplePlayer) {
   // called by StSampleVoice::prepareToPlayer()
   lfo_freq_global.sp_mod_speed = &_samplePlayer->mod_lfo_freq_spd;
   lfo_freq_global.sp_mod_level = &_samplePlayer->mod_lfo_freq_lvl;

   lfo_vol_global.sp_mod_speed = &_samplePlayer->mod_lfo_vol_spd;
   lfo_vol_global.sp_mod_level = &_samplePlayer->mod_lfo_vol_lvl;

   lfo_pan_global.sp_mod_speed = &_samplePlayer->mod_lfo_pan_spd;
   lfo_pan_global.sp_mod_level = &_samplePlayer->mod_lfo_pan_lvl;

   lfo_aux_global.sp_mod_speed = &_samplePlayer->mod_lfo_aux_spd;
   lfo_aux_global.sp_mod_level = &_samplePlayer->mod_lfo_aux_lvl;

   for(sUI i = 0u; i < STSAMPLE_NUM_MODSEQ; i++)
   {
      modseq_global[i].sp_mod_speed    = &_samplePlayer->mod_modseq[i].speed;
      modseq_global[i].sp_mod_level    = &_samplePlayer->mod_modseq[i].level;
      modseq_global[i].sp_mod_numsteps = &_samplePlayer->mod_modseq[i].numsteps;
      modseq_global[i].sp_mod_advance  = &_samplePlayer->mod_modseq[i].advance;
   }
}

YAC_Object *StSample::_getName(void) {
   return name;
}

void StSample::_setName(YAC_Object *_str) {
   if(NULL == name)
   {
      name = YAC_New_String();
   }
   name->yacCopy(_str);
}

YAC_Object *StSample::_getWaveform (void) {
   return waveform;
}

void StSample::_setWaveform(YAC_Object *_wav) {

   YAC_DEREF_ARG(StWaveform, wav, _wav);

   if(YAC_CHK(wav, clid_StWaveform))
   {
      if((void*)waveform != (void*)wav)
      {
         if((NULL != waveform) && b_own_waveform)
         {
            YAC_DELETE_SAFE(waveform);
         }
         waveform = (StWaveform*) wav;
         b_own_waveform = wav_deleteme;
      }
   }
   else
   {
      if((NULL != waveform) && b_own_waveform)
      {
         YAC_DELETE_SAFE(waveform);
      }
      waveform = NULL;
      b_own_waveform = YAC_FALSE;
      if(wav_deleteme)
         YAC_DELETE(wav);
   }
}

YAC_Object *StSample::_getPartialMagnitudes(void) {
   return partial_magnitudes;
}

void StSample::_setPartialMagnitudes(YAC_Object *_fa) {
   if(YAC_Is_FloatArray(_fa))
   {
      partial_magnitudes = (YAC_FloatArray*)_fa;
   }
   else
   {
      partial_magnitudes = NULL;
   }
}

YAC_Object *StSample::_getPartialSpeeds(void) {
   return partial_speeds;
}

void StSample::_setPartialSpeeds(YAC_Object *_fa) {
   if(YAC_Is_FloatArray(_fa))
   {
      partial_speeds = (YAC_FloatArray*)_fa;
   }
   else
   {
      partial_speeds = NULL;
   }
}

YAC_Object *StSample::_getPartialPhases(void) {
   return partial_phases;
}

void StSample::_setPartialPhases(YAC_Object *_fa) {
   if(YAC_Is_FloatArray(_fa))
   {
      partial_phases = (YAC_FloatArray*)_fa;
   }
   else
   {
      partial_phases = NULL;
   }
}

void StSample::_setNumPartials(sF32 _num) {
   if(_num < 1.0f)
      _num = 1.0f;
   else if(_num > 256.0f)
      _num = 256.0f;
   num_partials = _num;
}

sF32 StSample::_getNumPartials(void) {
   return num_partials;
}

void StSample::_setPartialCycleLenReset(sF32 _cycleLenReset) {
   partial_cyclelen_reset = _cycleLenReset;
}

sF32 StSample::_getPartialCycleLenReset(void) {
   return partial_cyclelen_reset;
}

void StSample::_setEnablePartialPhase0(sBool _bEnable) {
   b_partial_phase_0 = _bEnable;
}

sBool StSample::_getEnablePartialPhase0(void) {
   return b_partial_phase_0;
}

sBool StSample::verify_sample_area(sUI &offset, sUI &len, const sUI range) {

   if( (offset + len) < offset)
   {
      // 32bit wrap-around / overflow (should never happen)
      offset = 0;
      len    = range;

      return YAC_TRUE;
   }
   if( (offset + len) > range )
   {
      if(offset >= range)
      {
         if(range > 0)
         {
            offset = range - 1;
         }
         else
         {
            offset = 0;
         }
      }
      len = range - offset;

      ////printf("xxx verify_sample_area: new offset=%u len=%u (range=%u)\n", offset, len, range);

      return YAC_TRUE;
   }

   return YAC_FALSE;
}

static void shift_sample_area(sUI &offset, sUI &len, sUI _areaOffset, sSI _areaLen) {
   // (note) negative _areaLen means that samples have been deleted

   // yac_host->printf("xxx shift_sample_area: offset=%u len=%u areaOffset=%u areaLen=%u\n", offset, len, _areaOffset, _areaLen);

   sSI newOff = (sSI) offset;
   sSI newLen = (sSI) len;

   sSI ao = sSI(_areaOffset);
   sSI al = -_areaLen;
   sSI to = sSI(offset);
   sSI tl = sSI(len);

   // Dyac_host_printf("xxx shift_sample_area: oldOff=%u oldLen=%u areaOff=%u areaLen=%d\n", offset, len, _areaOffset, _areaLen);

   if(offset >= _areaOffset)
   {
      if(_areaLen < 0)
      {
         if((ao+al) >= to)
         {
            if((ao+al) >= (to+tl))
            {
               // Cut entire sample area
               // Dyac_host_printf("xxx cut entire sample area\n");
               newOff = 0;
               newLen = 0;
            }
            else
            {
               // Cut area at sample start
               // Dyac_host_printf("xxx cut area at sample start\n");
               newOff -= (to - ao);
               if(newOff < 0)
                  newOff = 0;
               newLen -= ((ao+al)-to);
            }
         }
         else
         {
            // Cut before sample area
            newOff += _areaLen;
            if(newOff < 0)
               newOff = 0;
         }
      }
      else
      {
         if(ao < to)
         {
            // Paste before sample area
            newOff += _areaLen;
         }
         else if(ao < to+tl)
         {
            // Paste within sample area
            newLen += _areaLen;
         }
         else
         {
            // Paste after sample area
         }
      }
   }
   else if(_areaLen < 0)
   {
      if( (ao+al) <= (to+tl) )
      {
         // Cut within sample area
         newLen -= al;
      }
      else if( ao <= (to+tl) )
      {
         // Cut crosses sample end
         newLen -= ((to+tl) - ao);
      }
   }
   else
   {
      if( (ao) < (to+tl) )
      {
         // Paste within sample area
         newLen += _areaLen;
      }
      else
      {
         // Paste after sample start
      }
   }

   // Dyac_host_printf("xxx shift_sample_area: oldOff=%u newOff=%d areaOff=%u areaLen=%d\n", offset, newOff, _areaOffset, _areaLen);
   if(newLen < 0)
      newLen = 0; // should not be reachable

   offset = (sUI) newOff;
   len    = (sUI) newLen;

   // Dyac_host_printf("xxx shift_sample_area: => newOff=%u newLen=%u\n", offset, len);
}

void StSample::_handleShiftSampleArea(sUI _offset, sSI _len) {

   // (note) no need to clip exact since verifySampleArea() is always called afterwards

   if(NULL != waveform)
   {
      // // sUI range = (sUI) waveform->_getNumFrames();

      // Shift attack/oneshot sample range
      if(sample_len > 0u)  // not using sample_loops only ?
         shift_sample_area(sample_offset, sample_len, _offset, _len);

      // Shift loops
      if(NULL != sample_loops)
      {
         sUI numLoops = sample_loops->num_elements / 3;
         sUI *loopDat = (sUI*) sample_loops->elements;
         sUI i;
         sUI k = 0u;
         for(i = 0u; i < numLoops; i++)
         {
            sUI times = loopDat[k + 2u];
            if(!(times & StSample::LOOP_OP_MASK)) // skip loop special instructions
            {
               shift_sample_area(loopDat[k + 0u], loopDat[k + 1u], _offset, _len);
            }
            k += 3u;
         }
      }
   }
   else
   {
      // No waveform selected, nothing to clip
   }
}

static void move_sample_area(sUI &offset, sUI &len, sUI _areaOffset, sUI _areaLen, sUI _newAreaOffset) {

   if( (offset >= _areaOffset) && ((offset + len) <= (_areaOffset + _areaLen)) )
   {
      sSI relOffset = (sSI(offset) - sSI(_areaOffset));
      offset = sUI(sSI(_newAreaOffset) + relOffset);
   }
}

void StSample::_handleMoveSampleArea(sUI _offset, sUI _len, sUI _newOffset) {
   if(NULL != waveform)
   {
      // // sUI range = (sUI) waveform->_getNumFrames();

      // Move attack/oneshot sample range
      if(sample_len > 0u)  // not using sample_loops only ?
         move_sample_area(sample_offset, sample_len, _offset, _len, _newOffset);

      // Move loops
      if(NULL != sample_loops)
      {
         sUI numLoops = sample_loops->num_elements / 3;
         sUI *loopDat = (sUI*) sample_loops->elements;
         sUI i;
         sUI k = 0u;
         for(i = 0u; i < numLoops; i++)
         {
            sUI times = loopDat[k + 2u];
            if(!(times & StSample::LOOP_OP_MASK)) // skip loop special instructions
            {
               move_sample_area(loopDat[k + 0u], loopDat[k + 1u], _offset, _len, _newOffset);
            }
            k += 3u;
         }
      }
   }
   else
   {
      // No waveform selected, nothing to clip
   }
}

sBool StSample::_verifySampleAreas(void) {
   //
   // - return true if one or more sample areas had to be clipped
   // - return false if sample areas are within waveform boundaries
   // - clipping strategy: first reduce len, then offset

   if(NULL != waveform)
   {
      sUI range = (sUI) waveform->_getNumFrames();
      // yac_host->printf("xxx verifySampleAreas: wf=%p range=%u\n", waveform, range);

      // Verify attack/oneshot sample range
      sBool r = YAC_TRUE;
      if(sample_len > 0u)  // not using sample_loops only ?
         r = verify_sample_area(sample_offset, sample_len, range);

      // Verify loops
      if(NULL != sample_loops)
      {
         sUI numLoops = sample_loops->num_elements / 3;
         sUI *loopDat = (sUI*) sample_loops->elements;
         sUI i;
         sUI k = 0u;
         for(i = 0u; i < numLoops; i++)
         {
            sUI times = loopDat[k + 2u];
            if(!(times & StSample::LOOP_OP_MASK)) // skip loop special instructions
            {
               r |= verify_sample_area(loopDat[k + 0u], loopDat[k + 1u], range); // Verify loop
            }
            k += 3u;
         }
      }

      // // if(b_liverec && (-1 != liverec_override_loop_offset) && (STSAMPLE_LIVEREC_DBLBUF_MODE_COPY != liverec_mode))
      if(b_liverec && (-1 != liverec_override_loop_offset) && !b_liverec_copy)
      {
         sUI lrOff = sUI(liverec_override_loop_offset);
         sUI lrLen = sUI(liverec_override_loop_len);
         if(0 != verify_sample_area(lrOff, lrLen, range))
         {
            liverec_override_loop_offset = -1;
            liverec_override_loop_len    = 0;
         }
      }

      return r;
   }
   else
   {
      // No waveform selected, nothing to clip
      return YAC_FALSE;
   }
}


YAC_Object *StSample::_getMutexGroup(void) {
   return mutex_group;
}

void StSample::_setMutexGroup(YAC_Object *_mtxGrp) {
   if(YAC_CHK(_mtxGrp, clid_StSampleMutexGroup))
   {
      mutex_group = (StSampleMutexGroup*) _mtxGrp;
   }
   else
   {
      mutex_group = NULL;
   }
}

void StSample::_setMaxVoices(sUI _maxVoices) {
   max_voices = _maxVoices;
}

sUI StSample::_getMaxVoices(void) {
   return max_voices;
}

YAC_Object *StSample::_getOrCreateKeyRange(void) {
   if(NULL == key_range)
   {
      key_range = YAC_NEW(StRange);
      key_range->b_high_inclusive = YAC_TRUE;
   }
   return key_range;
}

YAC_Object *StSample::_getOrCreateVelRange(void) {
   if(NULL == vel_range)
   {
      vel_range = YAC_NEW(StRange);
   }
   return vel_range;
}

YAC_Object *StSample::_getOrCreateModRange(void) {
   if(NULL == mod_range)
   {
      mod_range = YAC_NEW(StRange);
   }
   return mod_range;
}

YAC_Object *StSample::_getKeyRange(void) {
   return key_range;
}

YAC_Object *StSample::_getVelRange(void) {
   return vel_range;
}

YAC_Object *StSample::_getModRange(void) {
   return mod_range;
}

YAC_Object *StSample::_getOrCreateFreqADSR(void) {
   if(NULL == adsr_freq)
   {
      adsr_freq = YAC_NEW(StADSR);
   }
   return adsr_freq;
}

YAC_Object *StSample::_getOrCreateVolADSR(void) {
   if(NULL == adsr_vol)
   {
      adsr_vol = YAC_NEW(StADSR);
   }
   return adsr_vol;
}

YAC_Object *StSample::_getOrCreatePanADSR(void) {
   if(NULL == adsr_pan)
   {
      adsr_pan = YAC_NEW(StADSR);
   }
   return adsr_pan;
}

YAC_Object *StSample::_getOrCreateAuxADSR(void) {
   if(NULL == adsr_aux)
   {
      adsr_aux = YAC_NEW(StADSR);
   }
   return adsr_aux;
}

YAC_Object *StSample::_getFreqADSR(void) {
   return adsr_freq;
}

YAC_Object *StSample::_getVolADSR(void) {
   return adsr_vol;
}

YAC_Object *StSample::_getPanADSR(void) {
   return adsr_pan;
}

YAC_Object *StSample::_getAuxADSR(void) {
   return adsr_aux;
}

YAC_Object *StSample::_getOrCreateFreqLFO(void) {
   if(NULL == lfo_freq)
   {
      lfo_freq = YAC_NEW(StLFO);
   }
   return lfo_freq;
}

YAC_Object *StSample::_getOrCreateVolLFO(void) {
   if(NULL == lfo_vol)
   {
      lfo_vol = YAC_NEW(StLFO);
   }
   return lfo_vol;
}

YAC_Object *StSample::_getOrCreatePanLFO(void) {
   if(NULL == lfo_pan)
   {
      lfo_pan = YAC_NEW(StLFO);
   }
   return lfo_pan;
}

YAC_Object *StSample::_getOrCreateAuxLFO(void) {
   if(NULL == lfo_aux)
   {
      lfo_aux = YAC_NEW(StLFO);
   }
   return lfo_aux;
}

void StSample::_deleteFreqLFO(void) {
   YAC_DELETE_SAFE(lfo_freq);
}

void StSample::_deleteVolLFO(void) {
   YAC_DELETE_SAFE(lfo_vol);
}

void StSample::_deletePanLFO(void) {
   YAC_DELETE_SAFE(lfo_pan);
}

void StSample::_deleteAuxLFO(void) {
   YAC_DELETE_SAFE(lfo_aux);
}

YAC_Object *StSample::_getFreqLFO(void) {
   return lfo_freq;
}

YAC_Object *StSample::_getVolLFO(void) {
   return lfo_vol;
}

YAC_Object *StSample::_getPanLFO(void) {
   return lfo_pan;
}

YAC_Object *StSample::_getAuxLFO(void) {
   return lfo_aux;
}

YAC_Object *StSample::_getLFOByIndex(sUI _idx) {
   switch(_idx)
   {
      default:
         return NULL;

      case 0u:
         return lfo_freq;

      case 1u:
         return lfo_vol;

      case 2u:
         return lfo_pan;

      case 3u:
         return lfo_aux;
   }
   return NULL;
}

YAC_Object *StSample::_getOrCreateLFOByIndex(sUI _idx) {
   switch(_idx)
   {
      default:
         return NULL;

      case 0u:
         return _getOrCreateFreqLFO();

      case 1u:
         return _getOrCreateVolLFO();

      case 2u:
         return _getOrCreatePanLFO();

      case 3u:
         return _getOrCreateAuxLFO();
   }
   return NULL;
}

YAC_Object *StSample::_getModSeqByIndexAndPatch(sUI _idx, sUI _patchIdx) {
   if(_idx < STSAMPLE_NUM_MODSEQ)
   {
      if(_patchIdx < STSAMPLE_MAX_MODSEQ_PATCHES)
      {
         return modseq[_idx][_patchIdx];
      }
   }
   return NULL;
}

YAC_Object *StSample::_getOrCreateModSeqByIndexAndPatch(sUI _idx, sUI _patchIdx) {
   StModSeq *r = NULL;
   if(_idx < STSAMPLE_NUM_MODSEQ)
   {
      if(_patchIdx < STSAMPLE_MAX_MODSEQ_PATCHES)
      {
         r = modseq[_idx][_patchIdx];

         if(NULL == r)
         {
            r = modseq[_idx][_patchIdx] = YAC_NEW(StModSeq);
         }
      }
   }
   return r;
}

void StSample::_deleteModSeqByIndexAndPatch(sUI _idx, sUI _patchIdx) {
   if(_idx < STSAMPLE_NUM_MODSEQ)
   {
      if(_patchIdx < STSAMPLE_MAX_MODSEQ_PATCHES)
      {
         YAC_DELETE_SAFE(modseq[_idx][_patchIdx]);
      }
   }
}

void StSample::_setDefaultModSeqPatch(sUI _idx, sUI _patchIdx) {
   if(_idx < STSAMPLE_NUM_MODSEQ)
   {
      if(_patchIdx < STSAMPLE_MAX_MODSEQ_PATCHES)
      {
         default_modseq_patches[_idx] = _patchIdx;
      }
   }
}

sUI StSample::_getDefaultModSeqPatch(sUI _idx) {
   if(_idx < STSAMPLE_NUM_MODSEQ)
   {
      return default_modseq_patches[_idx];
   }
   return 0u;
}

sUI StSample::_findLastUsedModSeqPatch(sUI _idx) {
   // used to determine number of save slots when writing project file
   if(_idx < STSAMPLE_NUM_MODSEQ)
   {
      sUI maxPatchIdx = 0u;
      for(sUI j = 0u; j < STSAMPLE_MAX_MODSEQ_PATCHES; j++)
      {
         if(NULL != modseq[_idx][j])
            maxPatchIdx = j;
      }
      return maxPatchIdx;
   }
   return 0u;
}

YAC_Object *StSample::_getSampleLoops(void) {
   return sample_loops;
}

YAC_Object *StSample::_getOrCreateSampleLoops(void) {
   if(NULL == sample_loops)
   {
      sample_loops = YAC_New_IntArray();
   }
   return sample_loops;
}

void StSample::_allocSampleLoops(void) {
   _freeSampleLoops();
   if(NULL == sample_loops)
   {
      sample_loops = YAC_New_IntArray();
   }
}

void StSample::_freeSampleLoops(void) {
   YAC_DELETE_SAFE(sample_loops);
}

sUI StSample::_getNumSampleLoops(void) {
   if(NULL != sample_loops)
   {
      return sample_loops->num_elements / 3u;
   }
   return 0;
}

void StSample::_setFirstLoopIndex(sSI _loopIdx) {
   first_loop_index = _loopIdx;
}

sSI StSample::_getFirstLoopIndex(void) {
   return first_loop_index;
}

void StSample::_uiSetEnableSyncFirstLoopIndex(sBool _bEnabled) {
   ui_sync_first_loop_index = _bEnabled;
}

sBool StSample::_uiGetEnableSyncFirstLoopIndex(void) {
   return ui_sync_first_loop_index;
}

void StSample::_setNoteOffLoopIndex(sSI _loopIdx) {
   noteoff_loop_index = _loopIdx;
}

sSI  StSample::_getNoteOffLoopIndex(void) {
   return noteoff_loop_index;
}

void StSample::_setEnableNoteOffLoopImmediately(sSI _bEnabled) {
   b_noteoff_immediate_loop_jump = _bEnabled;
}

sSI  StSample::_getEnableNoteOffLoopImmediately(void) {
   return b_noteoff_immediate_loop_jump;
}

void StSample::_setEnableJumpToLoopImmediately(sSI _bEnabled) {
   b_jumptoloop_immediately = _bEnabled;
}

sSI  StSample::_getEnableJumpToLoopImmediately(void) {
   return b_jumptoloop_immediately;
}

sSI  StSample::_getFirstOffset(void) {
   sSI r = 0;

   if(NULL != sample_loops)
   {
      if(sample_loops->num_elements >= 3u)
      {
         r = sample_loops->elements[0];
      }
   }

   return r;
}

void StSample::_setPlayMode(sUI _mode) {
   play_mode = _mode;
}

sUI StSample::_getPlayMode(void) {
   return play_mode;
}

void StSample::_setFirstOffset(sSI _off) {
   (void)_getOrCreateSampleLoops();

   // Dyac_host_printf("xxx StSample::setFirstOffset: sample_loops=%p\n", sample_loops);
   // Dyac_host_printf("xxx StSample::setFirstOffset: num_elements=%u\n", sample_loops->num_elements);

   if(0u == sample_loops->num_elements)
   {
      sample_loops->yacArrayRealloc(3);
      sample_loops->elements[1] = 0;  // len
      sample_loops->elements[2] = 1;  // rep
      sample_loops->num_elements = 3;
   }
   // Dyac_host_printf("xxx StSample::setFirstOffset: 2 sample_loops=%p\n", sample_loops);
   // Dyac_host_printf("xxx StSample::setFirstOffset: 3 num_elements=%u\n", sample_loops->num_elements);
   // Dyac_host_printf("xxx StSample::setFirstOffset: 4 elements=%p\n", sample_loops->elements);
   sample_loops->elements[0] = _off;
}

sSI  StSample::_getFirstLen(void) {
   sSI r = 0;

   if(NULL != sample_loops)
   {
      if(sample_loops->num_elements >= 3u)
      {
         r = sample_loops->elements[1];
      }
   }

   return r;
}

void StSample::_setFirstLen(sSI _len) {
   (void)_getOrCreateSampleLoops();

   if(0u == sample_loops->num_elements)
   {
      sample_loops->yacArrayRealloc(3);
      sample_loops->num_elements = 3u;
      sample_loops->elements[0] = 0;  // off
      sample_loops->elements[2] = 0;  // len
   }
   sample_loops->elements[1] = _len;
}

sSI  StSample::_getFirstLoopOffset(void) {
   sSI r = 0;

   if(NULL != sample_loops)
   {
      if(sample_loops->num_elements >= (3*sUI(first_loop_index+1)))
      {
         r = sample_loops->elements[3*first_loop_index + 0];
      }
   }

   return r;
}

void StSample::_setFirstLoopOffset(sSI _off) {
   (void)_getOrCreateSampleLoops();

   if(sample_loops->num_elements >= (3*sUI(first_loop_index+1)))
   {
      sample_loops->elements[3*first_loop_index + 0] = _off;
   }
}

sSI  StSample::_getFirstLoopLen(void) {
   sSI r = 0;

   if(NULL != sample_loops)
   {
      if(sample_loops->num_elements >= (3*sUI(first_loop_index+1)))
      {
         r = sample_loops->elements[3*first_loop_index + 1];
      }
   }

   return r;
}

void StSample::_setFirstLoopLen(sSI _len) {
   (void)_getOrCreateSampleLoops();

   if(sample_loops->num_elements >= (3*sUI(first_loop_index+1)))
   {
      sample_loops->elements[3*first_loop_index + 1] = _len;
   }
}

sSI  StSample::_getFirstLoopRep(void) {
   sSI r = 0;

   if(NULL != sample_loops)
   {
      if(sample_loops->num_elements >= (3*sUI(first_loop_index+1)))
      {
         r = sample_loops->elements[3*first_loop_index + 2];
      }
   }

   return r;
}

void StSample::_setFirstLoopRep(sSI _num) {
   (void)_getOrCreateSampleLoops();

   if(sample_loops->num_elements >= (3*sUI(first_loop_index+1)))
   {
      sample_loops->elements[3*first_loop_index + 2] = _num;
   }
}

sSI StSample::_getLastLoopOffset(void) {
   // (note) used by SampleSynth to find target render area
   sSI r = 0;

   if(NULL != sample_loops)
   {
      if(sample_loops->num_elements >= 3u)
      {
         r = sample_loops->elements[sample_loops->num_elements - 3u + 0u];
      }
   }

   return r;
}

sSI StSample::_getLastLoopLen(void) {
   // (note) used by SampleSynth to find target render area
   sSI r = 0;

   if(NULL != sample_loops)
   {
      if(sample_loops->num_elements >= 3u)
      {
         r = sample_loops->elements[sample_loops->num_elements - 3u + 1u];
      }
   }

   return r;
}

sSI StSample::_getOffset(void) {
   return (sSI) sample_offset;
}

void StSample::_setOffset(sSI _off) {
   sample_offset = (sUI) _off;
}

sSI  StSample::_getLen(void) {
   return (sSI) sample_len;
}

void StSample::_setLen(sSI _len) {
   sample_len = (sUI) _len;
}

sUI StSample::_getMinOffset(void) {
   if(NULL != sample_loops)
   {
      sUI numLoops = sample_loops->num_elements / 3u;
      if(numLoops > 0u)
      {
         const sUI *loopDat = (const sUI*)sample_loops->elements;
         sUI ret = ~0u;
         for(sUI loopIdx = 0u; loopIdx < numLoops; loopIdx++)
         {
            if(loopDat[loopIdx * 3u + 0] < ret)
               ret = loopDat[loopIdx * 3u + 0];
         }
         return ret;
      }
      return 0u;
   }
   return sample_offset;
}

sUI StSample::_getMaxOffset(void) {
   if(NULL != sample_loops)
   {
      sUI numLoops = sample_loops->num_elements / 3u;
      const sUI *loopDat = (const sUI*)sample_loops->elements;
      sUI ret = 0u;
      for(sUI loopIdx = 0u; loopIdx < numLoops; loopIdx++)
      {
         sUI endOff = (loopDat[loopIdx * 3u + 0] + loopDat[loopIdx * 3u + 1]);
         if( endOff > ret)
            ret = endOff;
      }
      return ret;
   }
   return sample_offset + sample_len;
}

sF32 StSample::_getVolume(void) {
   return volume;
}

void StSample::_setVolume(sF32 _vol) {
   volume = _vol;
}

void StSample::_setVolLFOAmt(sF32 _amt) {
   vol_lfo_amt = _amt;
}

sF32 StSample::_getVolLFOAmt(void) {
   return vol_lfo_amt;
}

sF32 StSample::_getPan(void) {
   return pan;
}

void StSample::_setPan(sF32 _pan) {
   pan = _pan;
}

void StSample::_setPanEnvAmt(sF32 _amt) {
   pan_env_amt = _amt;
}

sF32 StSample::_getPanEnvAmt(void) {
   return pan_env_amt;
}

void StSample::_setPanLFOAmt(sF32 _amt) {
   pan_lfo_amt = _amt;
}

sF32 StSample::_getPanLFOAmt(void) {
   return pan_lfo_amt;
}

sF32 StSample::_getVolumeVelocityAmount(void) {
   return volume_velocity_amount;
}

void StSample::_setVolumeVelocityAmount(sF32 _a) {
   volume_velocity_amount = _a;
}

sF32 StSample::_getSampleOffsetVelocityAmount(void) {
   return sampleoffset_velocity_amount;
}

void StSample::_setSampleOffsetVelocityAmount(sF32 _a) {
   sampleoffset_velocity_amount = _a;
}

sSI  StSample::_getEnableInvertSampleOffsetVelocityAmount(void) {
   return b_invert_sampleoffset_velocity_amount;
}

void StSample::_setEnableInvertSampleOffsetVelocityAmount(sSI _bEnable) {
   b_invert_sampleoffset_velocity_amount = _bEnable;
}

sF32 StSample::_getSampleOffsetRandAmount(void) {
   return sampleoffset_rand_amount;
}

void StSample::_setSampleOffsetRandAmount(sF32 _a) {
   sampleoffset_rand_amount = _a;
}

void StSample::_setSampleOffsetRel(sF32 _pct) {
   sampleoffset_rel = _pct;
}

sF32 StSample::_getSampleOffsetRel(void) {
   return sampleoffset_rel;
}

sF32 StSample::_getSampleRateRatio(void) {
   return sample_rate_ratio;
}

void StSample::_setSampleRateRatio(sF32 _ratio) {
   if(_ratio > 0.0f)
   {
      sample_rate_ratio = _ratio;
   }
}

sF32 StSample::_getTranspose(void) {
   return transpose;
}

void StSample::_setTranspose(sF32 _transpose) {
   transpose = _transpose;
}

void StSample::_setTransposeAndUpdateOctSemiCents(sF32 _transpose) {
   // Dyac_host_printf("xxx Sample::setTransposeAndUpdateOctSemiCents: _transpose=%f\n", _transpose);
   transpose = _transpose;
   sF32 tr = _transpose;
   ui_transpose_oct  = sS8(int(tr+0.5) / 12.0f);
   tr -= ui_transpose_oct * 12;
   ui_transpose_semi = (tr < 0) ? -(int(-tr +0.5) % 12) : (int(tr+0.5) % 12);
   tr -= ui_transpose_semi;
   ui_transpose_cents = tr * 100.0f;
}

void StSample::updateTranspose(void) {
   _setTranspose(ui_transpose_oct * 12 + ui_transpose_semi + ui_transpose_cents/100.0f);
}

void StSample::_setUiTransposeOct(sSI _oct) {
   ui_transpose_oct = sS8(_oct);
   updateTranspose();
}

sSI StSample::_getUiTransposeOct(void) {
   return ui_transpose_oct;
}

void StSample::_setUiTransposeSemi(sSI _semi) {
   ui_transpose_semi = sS8(_semi);
   updateTranspose();
}

sSI StSample::_getUiTransposeSemi(void) {
   return ui_transpose_semi;
}

void StSample::_setUiTransposeCents(sF32 _cents) {
   ui_transpose_cents = _cents;
   updateTranspose();
}

sF32 StSample::_getUiTransposeCents(void) {
   return ui_transpose_cents;
}

void StSample::_setTransposeAndUpdateOctSemi(sSI _key) {
   ui_transpose_oct = sS8(_key / 12);
   ui_transpose_semi = sS8(_key - (ui_transpose_oct * 12));
   updateTranspose();
}

void StSample::_setBeatFrequency(sF32 _cents) {
   beat_frequency = _cents;
}

sF32 StSample::_getBeatFrequency(void) {
   return beat_frequency;
}

void StSample::_setEnableLerpFreq(sBool _bEnable) {
   b_lerp_freq = _bEnable;
}

sBool StSample::_getEnableLerpFreq(void) {
   return b_lerp_freq;
}

void StSample::_setEnableLerpVol(sBool _bEnable) {
   b_lerp_vol = _bEnable;
}

sBool StSample::_getEnableLerpVol(void) {
   return b_lerp_vol;
}

void StSample::_setFreqEnvAmt(sF32 _amt) {
   freq_env_amt = _amt;
}

sF32 StSample::_getFreqEnvAmt(void) {
   return freq_env_amt;
}

void StSample::_setFreqLFOAmt(sF32 _amt) {
   freq_lfo_amt = _amt;
}

sF32 StSample::_getFreqLFOAmt(void) {
   return freq_lfo_amt;
}

sF32 StSample::_getDelay (void) {
   return sample_delay;
}

void StSample::_setDelay (sF32 _dly) {
   sample_delay = _dly;
}

sF32 StSample::_getDelayMultiplier (void) {
   return sample_delay_multiplier;
}

void StSample::_setDelayMultiplier (sF32 _mul) {
   sample_delay_multiplier = _mul;
}

sBool StSample::filterNoteOn(sSI _note, sF32 _vel, sF32 _mod) {
   sBool r = YAC_TRUE;

   // Dyac_host_printf("xxx filterNoteOn: note=%d vel=%f mod=%f b_skip_range=%d\n", _note, _vel, _mod, b_skip_range);

   if(!b_skip_range)
   {
      if(NULL != key_range)
      {
         r = r && key_range->filterNoteOn((sF32)_note);
         // Dyac_host_printf("xxx filterNoteOn: after key_range r=%d\n", r);
      }

      if(r && (NULL != vel_range))
      {
         r = r && vel_range->filterNoteOn(_vel);
         // Dyac_host_printf("xxx filterNoteOn: after vel_range r=%d\n", r);
         // Dyac_host_printf("xxx filterNoteOn: vel_range lo=%f hi=%f\n", vel_range->lo, vel_range->hi);
      }

      if(r && (NULL != mod_range))
      {
         r = r && mod_range->filterNoteOn(_mod);
         // Dyac_host_printf("xxx filterNoteOn: after mod_range r=%d\n", r);
      }
   }
   return r;
}

sF32 StSample::calcVelZoneVolume(sF32 _vel) {
   sF32 v;

   v = (1.0f - volume_velocity_amount) + (_vel * volume_velocity_amount);
   ////printf("xxx v=%f vel_range=%p\n", v, vel_range);

   if(vel_range && !b_skip_range)
   {
      v = v * vel_range->calcAmount(_vel);
   }

   if(v > 1.0f)
   {
      v = 1.0f;
   }
   else if(v < 0.0f)
   {
      v = 0.0f;
   }

   return v;
}

sF32 StSample::calcKeyModZoneVolume(sF32 _freq, sF32 _mod) {
   sF32 v = 1.0f;

   if(key_range && !b_skip_range)
   {
      // Dyac_host_printf("xxx calcKeyModZoneVolume: freq=%f v=%f\n", _freq, v);
      sF32 a = key_range->calcAmount(_freq);
      v = v * a;
   }

   if(mod_range && !b_skip_range)
   {
      sF32 a = mod_range->calcAmount(_mod);
      v = v * a;
      // Dyac_host_printf("xxx calcKeyModZoneVolume: a=%f v=%f mod=%f\n", a, v, _mod);
   }

   return v;
}

void StSample::setEnableTimestretch(sSI _bEnabled) {
   b_timestretch = _bEnabled;
}

sSI StSample::getEnableTimestretch(void) {
   return b_timestretch;
}

void StSample::setEnableTimestretchAdditive(sSI _bEnabled) {
   b_timestretch_additive = _bEnabled;
}

sSI StSample::getEnableTimestretchAdditive(void) {
   return b_timestretch_additive;
}

void StSample::setEnableTimestretchXFade(sSI _bEnabled) {
   b_timestretch_xfade = _bEnabled;
}

sSI StSample::getEnableTimestretchXFade(void) {
   return b_timestretch_xfade;
}

void StSample::setTimestretch(sF32 _timestretch) {
   timestretch = _timestretch;
}

sF32 StSample::getTimestretch(void) {
   return timestretch;
}

void StSample::setTimestretchGranularity(sF32 _gran) {
   if(_gran < 2.0f)
      _gran = 2.0f;
   timestretch_granularity = _gran;
}

sF32 StSample::getTimestretchGranularity(void) {
   return timestretch_granularity;
}

void StSample::setTimestretchGranularityAmount(sF32 _a) {
   timestretch_granularity_amount = _a;
}

sF32 StSample::getTimestretchGranularityAmount(void) {
   return timestretch_granularity_amount;
}

void StSample::setEnableStaticCyclelen(sSI _bEnabled) {
   b_static_cyclelen = _bEnabled;
}

sSI StSample::getEnableStaticCyclelen(void) {
   return b_static_cyclelen;
}

void StSample::setEnableAbsoluteTimestretch(sSI _bEnabled) {
   b_absolute_timestretch = _bEnabled;
}

sSI StSample::getEnableAbsoluteTimestretch(void) {
   return b_absolute_timestretch;
}

void StSample::setTimestretchAdditiveXFade(sF32 _amt) {
   timestretch_additive_xfade = _amt;
}

sF32 StSample::getTimestretchAdditiveXFade(void) {
   return timestretch_additive_xfade;
}

void StSample::setTimestretchInterpolType(sSI _type) {
   timestretch_interpol_type = _type;
}

sSI StSample::getTimestretchInterpolType(void) {
   return timestretch_interpol_type;
}

void StSample::setTimestretchGrainWindowType(sSI _type) {
   timestretch_grain_window_type = _type;
}

sSI StSample::getTimestretchGrainWindowType(void) {
   return timestretch_grain_window_type;
}

void StSample::setTimestretchStartPhaseRandAmount(sF32 _amount) {
   timestretch_startphase_rand_amount = _amount;
}

sF32 StSample::getTimestretchStartPhaseRandAmount(void) {
   return timestretch_startphase_rand_amount;
}

void StSample::setTimestretchBend(sF32 _amount) {
   timestretch_bend = _amount;
}

sF32 StSample::getTimestretchBend(void) {
   return timestretch_bend;
}

void StSample::setTimestretch2DWidth(sUI _w) {
   timestretch_2d_w = _w;
}

sUI StSample::getTimestretch2DWidth(void) {
   return timestretch_2d_w;
}

void StSample::setTimestretch2DHeight(sUI _h) {
   timestretch_2d_h = _h;
}

sUI StSample::getTimestretch2DHeight(void) {
   return timestretch_2d_h;
}

void StSample::_setEnableTimedLoop(sSI _bEnabled) {
   b_timedloop = _bEnabled;
}

sSI StSample::_getEnableTimedLoop(void) {
   return b_timedloop;
}

void StSample::_setEnableTimedLoopFade(sSI _bEnabled) {
   b_timedloop_fade = _bEnabled;
}

sSI StSample::_getEnableTimedLoopFade(void) {
   return b_timedloop_fade;
}

void StSample::_setTimedLoopBase(sF32 _base) {
   if(_base > 0.0f)
   {
      timedloop_base = _base;
   }
   else
   {
      timedloop_base = STSAMPLE_DEFAULT_TIMEDLOOP_BASE;
   }
}

sF32 StSample::_getTimedLoopBase(void) {
   return timedloop_base;
}

void StSample::_setEnableOneShotLoopStep(sSI _bEnabled) {
   b_oneshot_loopstep = _bEnabled;
}

sSI StSample::_getEnableOneShotLoopStep(void) {
   return b_oneshot_loopstep;
}

void StSample::_setVolumeRampStepsMillisecIn(sF32 _millisec) {
   sF32 sec = _millisec * 0.001f;

   sUI numSteps = (sUI) (sec / STSAMPLEVOICE_BLOCK_LEN);

   if(numSteps > MAX_VOLRAMP_STEPS)
   {
      numSteps = MAX_VOLRAMP_STEPS;
   }

   num_volramp_steps_in = numSteps;
}

sF32 StSample::_getVolumeRampStepsMillisecIn(void) {
   return num_volramp_steps_in * (STSAMPLEVOICE_BLOCK_LEN * 1000.0f);
}

void StSample::_setVolumeRampStepsMillisecOut(sF32 _millisec) {
   sF32 sec = _millisec * 0.001f;

   sUI numSteps = (sUI) (sec / STSAMPLEVOICE_BLOCK_LEN);
   if(numSteps > MAX_VOLRAMP_STEPS)
   {
      numSteps = MAX_VOLRAMP_STEPS;
   }

   num_volramp_steps_out = numSteps;
}

sF32 StSample::_getVolumeRampStepsMillisecOut(void) {
   return num_volramp_steps_out * (STSAMPLEVOICE_BLOCK_LEN * 1000.0f);
}

void StSample::_setEnableFreezeSampleOffWhenQuiet(sSI _bEnabled) {
   b_freeze_sampleoff_when_quiet = _bEnabled;
}

sSI StSample::_getEnableFreezeSampleOffWhenQuiet(void) {
   return b_freeze_sampleoff_when_quiet;
}

void StSample::_setEnableModShiftOrig(sSI _bEnabled) {
   b_modshiftorig = _bEnabled;
}

sSI StSample::_getEnableModShiftOrig(void) {
   return b_modshiftorig;
}

void StSample::_setEnableSampleOffNoteOn(sSI _bEnabled) {
   b_sampleoffnoteon = _bEnabled;
}

sSI StSample::_getEnableSampleOffNoteOn(void) {
   return b_sampleoffnoteon;
}

void StSample::_setEnabled(sSI _bEnabled) {
   b_enabled = _bEnabled;
}

sSI StSample::_getEnabled(void) {
   return b_enabled;
}

void StSample::_setInterpolType(sSI _interpolType) {
   interpol_type = _interpolType;
}

sSI StSample::_getInterpolType(void) {
   return interpol_type;
}

void StSample::_setInterpolOrder(sUI _interpolOrder) {
   interpol_order = _interpolOrder;
}

sUI StSample::_getInterpolOrder(void) {
   return interpol_order;
}

void StSample::_setBitReductionPreAmp(sF32 _amp) {
   br_preamp = _amp;
}

sF32 StSample::_getBitReductionPreAmp(void) {
   return br_preamp;
}

void  StSample::_setBitReduction(sUI _shift) {
   br_bits = _shift;
   br_mask = ((1u << _shift) - 1u);
}

sUI  StSample::_getBitReduction(void) {
   return br_bits;
}

void  StSample::_setBitReductionMask(sUI _mask) {
   br_mask = (sU16)_mask;
}

sUI  StSample::_getBitReductionMask(void) {
   return (sU16)br_mask;
}

void  StSample::_setRateReduction(sUI _shift) {
   smpoff_bits = _shift;
   smpoff_mask = ~((1u << _shift) - 1u);
}

sUI  StSample::_getRateReduction(void) {
   return smpoff_bits;
}

void  StSample::_setRateReductionMask(sUI _mask) {
   smpoff_mask = ~_mask;
}

sUI  StSample::_getRateReductionMask(void) {
   return ~smpoff_mask;
}

void StSample::_setEnableFromStart(sBool _bEnabled) {
   b_from_start = _bEnabled;
}

sBool StSample::_getEnableFromStart(void) {
   return b_from_start;
}

YAC_Object *StSample::_getOrCreateWavepathTable() {
   if(NULL == wavepath_table)
   {
      wavepath_table = YAC_New_FloatArray();
   }
   return wavepath_table;
}

void StSample::_setEnableWavepath(sBool _bEnabled) {
   b_wavepath = _bEnabled;
}

sBool StSample::_getEnableWavepath(void) {
   return b_wavepath;
}

void StSample::_setWavepathIndex(sSI _idx) {
   wavepath_idx = _idx;
}

sSI StSample::_getWavepathIndex(void) {
   return wavepath_idx;
}

void StSample::_setEnableSkipRange(sSI _bEnable) {
   b_skip_range = _bEnable;
}

void StSample::_setEnableAlt(sSI _bEnable) {
   alt.b_enable = _bEnable;

   if(NULL != parent_samplebank)
   {
      parent_samplebank->updateAltGroups();
   }
}

sSI StSample::_getEnableAlt(void) {
   return alt.b_enable;
}

StSample *StSample::getNextAltRandSample(void) {
   if(alt.b_enable && alt.num > 0u)
   {
      // also see "tks-examples/non_repeating_rand.tks"
      sUI r;
      sUI retIdx;
      if(1u == alt.rand_num_left)
      {
         retIdx = alt.rand_tbl[0];
         alt.rand_num_left--;
         return alt.samples[retIdx];
      }
      else if(0u == alt.rand_num_left)
      {
         alt.rand_num_left = alt.num;
         sUI numLoop = 0u;
         do
         {
            r = sUI(lfsr.rand() % alt.num);
            if(0u == (numLoop++ & 31u))
               lfsr.shuffle(); // paranoia
            else if(numLoop > 255u)
            {
               r = 0u;
               Dyac_host_printf("[!!!] StSample::getNextAltRandSample: rand() failed to be random\n");
               break;
            }
         }
         while(0u == r);
      }
      else
      {
         r = sUI(lfsr.rand() % alt.rand_num_left);
      }
      retIdx = alt.rand_tbl[r];
      alt.rand_num_left--;
      alt.rand_tbl[r] = alt.rand_tbl[alt.rand_num_left];
      alt.rand_tbl[alt.rand_num_left] = retIdx;
      return alt.samples[retIdx];
   }
   return NULL;
}

void StSample::_setEnableFilter(sBool _bEnable) {
   b_filter = _bEnable;
}

sBool StSample::_getEnableFilter(void) {
   return b_filter;
}

void StSample::_setFilterType(sSI _type) {
   filter_type = sUI(_type);
}

sSI StSample::_getFilterType(void) {
   return sSI(filter_type);
}

void StSample::_setFilterCutOff(sF32 _f) {
   filter_cutoff = _f;
}

sF32 StSample::_getFilterCutOff(void) {
   return filter_cutoff;
}

void StSample::_setFilterPan(sF32 _f) {
   filter_pan = _f;
}

sF32 StSample::_getFilterPan(void) {
   return filter_pan;
}

void StSample::_setFilterOffset(sF32 _f) {
   filter_offset = _f;
}

sF32 StSample::_getFilterOffset(void) {
   return filter_offset;
}

void StSample::_setFilterEQGain(sF32 _f) {
   filter_eq_gain = _f;
}

sF32 StSample::_getFilterEQGain(void) {
   return filter_eq_gain;
}

void StSample::_setFilterResonance(sF32 _f) {
   filter_resonance = _f;
}

sF32 StSample::_getFilterResonance(void) {
   return filter_resonance;
}

void StSample::_setFilterAuxEnvAmount(sF32 _amount) {
   filter_aux_env_amount = _amount;
}

sF32 StSample::_getFilterAuxEnvAmount(void) {
   return filter_aux_env_amount;
}

void StSample::_setFilterAuxEnvVelocityAmount(sF32 _amount) {
   filter_aux_env_velocity_amount = _amount;
}

sF32 StSample::_getFilterAuxEnvVelocityAmount(void) {
   return filter_aux_env_velocity_amount;
}

void StSample::_setFilterAuxLFOAmount(sF32 _amount) {
   filter_aux_lfo_amount = _amount;
}

sF32 StSample::_getFilterAuxLFOAmount(void) {
   return filter_aux_lfo_amount;
}

void StSample::_setFilterKeyboardAmount(sF32 _amount) {
   filter_keyboard_amount = _amount;
}

sF32 StSample::_getFilterKeyboardAmount(void) {
   return filter_keyboard_amount;
}

void StSample::_setGlideType(sUI _type) {
   glide_type = _type;
}

sUI StSample::_getGlideType(void) {
   return glide_type;
}

void StSample::_setGlideSpeedTimeUp(sF32 _millisec) {
   if(_millisec > 0.0f)
   {
      glide_speed_time_up = 1.0f / _millisec;
   }
   else
   {
      glide_speed_time_up = 0.0f;
   }
}

sF32 StSample::_getGlideSpeedTimeUp(void) {
   if(glide_speed_time_up > 0.0f)
      return 1.0f / glide_speed_time_up;
   else
      return 0.0f;
}

void StSample::_setGlideSpeedTimeDown(sF32 _millisec) {
   if(_millisec > 0.0f)
   {
      glide_speed_time_down = 1.0f / _millisec;
   }
   else
   {
      glide_speed_time_down = 0.0f;
   }
}

sF32 StSample::_getGlideSpeedTimeDown(void) {
   if(glide_speed_time_down > 0.0f)
      return 1.0f / glide_speed_time_down;
   else
      return 0.0f;
}

void StSample::_setGlideSpeedFreqUp(sF32 _semiPerSec) {
   glide_speed_freq_up = _semiPerSec / 1000.0f;
}

sF32 StSample::_getGlideSpeedFreqUp(void) {
   return glide_speed_freq_up * 1000.0f;
}

void StSample::_setGlideSpeedFreqDown(sF32 _semiPerSec) {
   glide_speed_freq_down = _semiPerSec / 1000.0f;
}

sF32 StSample::_getGlideSpeedFreqDown(void) {
   return glide_speed_freq_down * 1000.0f;
}

void StSample::_setEnableGlideRelease(sBool _bEnable) {
   b_glide_release = _bEnable;
}

sBool StSample::_getEnableGlideRelease(void) {
   return b_glide_release;
}

void StSample::_setGlideMaxNoteDist(sSI _dist) {
   glide_max_note_dist = _dist;
}

sSI StSample::_getGlideMaxNoteDist(void) {
   return glide_max_note_dist;
}

void StSample::_setEnableGlideGlissando(sBool _bEnable) {
   b_glide_glissando = _bEnable;
}

sBool StSample::_getEnableGlideGlissando(void) {
   return b_glide_glissando;
}

void StSample::_setGlideMinTime(sUI _millisec) {
   glide_min_time = _millisec;
}

sUI StSample::_getGlideMinTime(void) {
   return glide_min_time;
}

void StSample::_setGlideMaxTime(sUI _millisec) {
   glide_max_time = _millisec;
}

sUI StSample::_getGlideMaxTime(void) {
   return glide_max_time;
}

void StSample::_setEnableGlideRetrigSample(sBool _bEnable) {
   b_glide_retrig_sample = _bEnable;
}

sBool StSample::_getEnableGlideRetrigSample(void) {
   return b_glide_retrig_sample;
}

void  StSample::_setEnableGlideRetrigEnvFreq(sBool _bEnable) {
   b_glide_retrig_env_freq = _bEnable;
}

sBool StSample::_getEnableGlideRetrigEnvFreq(void) {
   return b_glide_retrig_env_freq;
}

void  StSample::_setEnableGlideRetrigEnvVol(sBool _bEnable) {
   b_glide_retrig_env_vol = _bEnable;
}

sBool StSample::_getEnableGlideRetrigEnvVol(void) {
   return b_glide_retrig_env_vol;
}

void  StSample::_setEnableGlideRetrigEnvPan(sBool _bEnable) {
   b_glide_retrig_env_pan = _bEnable;
}

sBool StSample::_getEnableGlideRetrigEnvPan(void) {
   return b_glide_retrig_env_pan;
}

void  StSample::_setEnableGlideRetrigEnvAux(sBool _bEnable) {
   b_glide_retrig_env_aux = _bEnable;
}

sBool StSample::_getEnableGlideRetrigEnvAux(void) {
   return b_glide_retrig_env_aux;
}

void  StSample::_setEnableGlideRetrigLFOFreq(sBool _bEnable) {
   b_glide_retrig_lfo_freq = _bEnable;
}

sBool StSample::_getEnableGlideRetrigLFOFreq(void) {
   return b_glide_retrig_lfo_freq;
}

void  StSample::_setEnableGlideRetrigLFOVol(sBool _bEnable) {
   b_glide_retrig_lfo_vol = _bEnable;
}

sBool StSample::_getEnableGlideRetrigLFOVol(void) {
   return b_glide_retrig_lfo_vol;
}

void  StSample::_setEnableGlideRetrigLFOPan(sBool _bEnable) {
   b_glide_retrig_lfo_pan = _bEnable;
}

sBool StSample::_getEnableGlideRetrigLFOPan(void) {
   return b_glide_retrig_lfo_pan;
}

void  StSample::_setEnableGlideRetrigLFOAux(sBool _bEnable) {
   b_glide_retrig_lfo_aux = _bEnable;
}

sBool StSample::_getEnableGlideRetrigLFOAux(void) {
   return b_glide_retrig_lfo_aux;
}

void StSample::_setEnableGlideRetrigModSeq(sUI _idx, sBool _bEnable) {
   b_glide_retrig_modseq[_idx % STSAMPLE_NUM_MODSEQ] = _bEnable;
}

sBool StSample::_getEnableGlideRetrigModSeq(sUI _idx) {
   return b_glide_retrig_modseq[_idx % STSAMPLE_NUM_MODSEQ];
}

void StSample::_setMMKeyboardCenter(sF32 _ctr) {
   mm_keyboard_center = _ctr;
}

sF32 StSample::_getMMKeyboardCenter(void) {
   return mm_keyboard_center;
}

void StSample::_setMMKeyboardMin(sF32 _min) {
   mm_keyboard_min = _min;
}

sF32 StSample::_getMMKeyboardMin(void) {
   return mm_keyboard_min;
}

void StSample::_setMMKeyboardMax(sF32 _max) {
   mm_keyboard_max = _max;
}

sF32 StSample::_getMMKeyboardMax(void) {
   return mm_keyboard_max;
}

void StSample::_setMMEnable(sUI _idx, sBool _bEnable) {
   if(_idx < STSAMPLE_NUM_MODMATRIX_ENTRIES)
   {
      modmatrix[_idx].b_enable = (YAC_FALSE != _bEnable);
   }
}

sBool StSample::_getMMEnable(sUI _idx) {
   if(_idx < STSAMPLE_NUM_MODMATRIX_ENTRIES)
   {
      return modmatrix[_idx].b_enable;
   }
   return YAC_FALSE;
}

void StSample::_setMMOp(sUI _idx, sUI _op) {
   if(_idx < STSAMPLE_NUM_MODMATRIX_ENTRIES)
   {
      if(_op < STSAMPLE_MM_NUM_OP)
      {
         modmatrix[_idx].op = _op;
      }
   }
}

sUI StSample::_getMMOp(sUI _idx) {
   if(_idx < STSAMPLE_NUM_MODMATRIX_ENTRIES)
   {
      return modmatrix[_idx].op;
   }
   return STSAMPLE_MM_OP_AUTO;
}

void StSample::_setMMSlewUp(sUI _idx, sF32 _amtNorm) {
   if(_idx < STSAMPLE_NUM_MODMATRIX_ENTRIES)
   {
      modmatrix[_idx].slew_up = _amtNorm;
   }
}

sF32 StSample::_getMMSlewUp(sUI _idx) {
   if(_idx < STSAMPLE_NUM_MODMATRIX_ENTRIES)
   {
      return modmatrix[_idx].slew_up;
   }
   return 0.0f;
}

void StSample::_setMMSlewDown(sUI _idx, sF32 _amtNorm) {
   if(_idx < STSAMPLE_NUM_MODMATRIX_ENTRIES)
   {
      modmatrix[_idx].slew_down = _amtNorm;
   }
}

sF32 StSample::_getMMSlewDown(sUI _idx) {
   if(_idx < STSAMPLE_NUM_MODMATRIX_ENTRIES)
   {
      return modmatrix[_idx].slew_down;
   }
   return 0.0f;
}

void StSample::_setMMSlewReset(sUI _idx, sSI _mode) {
   if(_idx < STSAMPLE_NUM_MODMATRIX_ENTRIES)
   {
      modmatrix[_idx].slew_reset = _mode;
   }
}

sSI StSample::_getMMSlewReset(sUI _idx) {
   if(_idx < STSAMPLE_NUM_MODMATRIX_ENTRIES)
   {
      return modmatrix[_idx].slew_reset;
   }
   return STSAMPLE_MM_SLEW_RESET_OFF;
}

void StSample::_setMMSrc(sUI _idx, sUI _src) {
   if(_idx < STSAMPLE_NUM_MODMATRIX_ENTRIES)
   {
      if(_src < STSAMPLE_MM_NUM_SRC)
      {
         modmatrix[_idx].src = _src;
      }
   }
}

sUI StSample::_getMMSrc(sUI _idx) {
   if(_idx < STSAMPLE_NUM_MODMATRIX_ENTRIES)
   {
      return modmatrix[_idx].src;
   }
   return STSAMPLE_MM_SRC_NONE;
}

void StSample::_setMMCurve(sUI _idx, sF32 _c) {
   if(_idx < STSAMPLE_NUM_MODMATRIX_ENTRIES)
   {
      modmatrix[_idx].curve = _c;
   }
}

sF32 StSample::_getMMCurve(sUI _idx) {
   if(_idx < STSAMPLE_NUM_MODMATRIX_ENTRIES)
   {
      return modmatrix[_idx].curve;
   }
   return 0.0f;
}

void StSample::_setMMDst(sUI _idx, sUI _dst) {
   if(_idx < STSAMPLE_NUM_MODMATRIX_ENTRIES)
   {
      if(_dst < STSAMPLE_MM_NUM_DST)
      {
         modmatrix[_idx].dst = _dst;
      }
   }
}

sUI StSample::_getMMDst(sUI _idx) {
   if(_idx < STSAMPLE_NUM_MODMATRIX_ENTRIES)
   {
      return modmatrix[_idx].dst;
   }
   return STSAMPLE_MM_DST_NONE;
}

void StSample::_setMMAmt(sUI _idx, sF32 _amt) {
   if(_idx < STSAMPLE_NUM_MODMATRIX_ENTRIES)
   {
      // // if(_amt < -2.0f)
      // //    _amt = -2.0f;
      // // else if(_amt > 2.0f)
      // //    _amt = 2.0f;
      modmatrix[_idx].amt = _amt;
   }
}

sF32 StSample::_getMMAmt(sUI _idx) {
   if(_idx < STSAMPLE_NUM_MODMATRIX_ENTRIES)
   {
      return modmatrix[_idx].amt;
   }
   return 0.0f;
}

void StSample::_setMMSignalTap(sUI _idx, sBool _bEnable) {
   if(_idx < STSAMPLE_NUM_MODMATRIX_ENTRIES)
   {
      modmatrix[_idx].b_signal_tap = (YAC_FALSE != _bEnable);
   }
}

sBool StSample::_getMMSignalTap(sUI _idx) {
   if(_idx < STSAMPLE_NUM_MODMATRIX_ENTRIES)
   {
      return modmatrix[_idx].b_signal_tap;
   }
   return YAC_FALSE;
}

void StSample::_setLiveRecEnable(sBool _bEnable) {
   b_liverec = _bEnable;
}

sBool StSample::_getLiveRecEnable(void) {
   return b_liverec;
}

void StSample::_setLiveRecPause(sBool _bEnable) {
   b_liverec_pause = _bEnable;
}

sBool StSample::_getLiveRecPause(void) {
   return b_liverec_pause;
}

void StSample::_setLiveRecMonitor(sBool _bEnable) {
   b_liverec_monitor = _bEnable;
}

sBool StSample::_getLiveRecMonitor(void) {
   return b_liverec_monitor;
}

void StSample::_setLiveRecInput(sUI _idx) {
   if(_idx < STSAMPLE_MAX_INPUTS)
      liverec_input_idx = _idx;
}

sUI StSample::_getLiveRecInput(void) {
   return liverec_input_idx;
}

void StSample::_setLiveRecInputPan(sF32 _pan) {
   liverec_input_pan = _pan;
}

sF32 StSample::_getLiveRecInputPan(void) {
   return liverec_input_pan;
}

void StSample::_setLiveRecStereo(sBool _bEnable) {
   b_liverec_stereo = _bEnable;
}

sBool StSample::_getLiveRecStereo(void) {
   return b_liverec_stereo;
}

void StSample::_setLiveRecCopy(sBool _bEnable) {
   b_liverec_copy = _bEnable;
}

sBool StSample::_getLiveRecCopy(void) {
   return b_liverec_copy;
}

void StSample::_setLiveRecClear(sBool _bEnable) {
   b_liverec_clear = _bEnable;
}

sBool StSample::_getLiveRecClear(void) {
   return b_liverec_clear;
}

void StSample::_setLiveRecDoubleBufferXFade(sBool _bEnable) {
   b_liverec_doublebuffer_xfade = _bEnable;
}

sBool StSample::_getLiveRecDoubleBufferXFade(void) {
   return b_liverec_doublebuffer_xfade;
}

void StSample::_setLiveRecOscMode(sUI _mode) {
   liverec_osc_mode = _mode;
}

sUI StSample::_getLiveRecOscMode(void) {
   return liverec_osc_mode;
}

void StSample::_setLiveRecOscSingleCycle(sBool _bEnable) {
   b_liverec_osc_singlecycle = _bEnable;
}

sBool StSample::_getLiveRecOscSingleCycle(void) {
   return b_liverec_osc_singlecycle;
}

void StSample::_setLiveRecOscTune(sF32 _tune) {
   liverec_osc_tune = _tune;
}

sF32 StSample::_getLiveRecOscTune(void) {
   return liverec_osc_tune;
}

void StSample::_setLiveRecOscFadeAmt(sF32 _newAmt) {
   liverec_osc_fade_amt = _newAmt;
}

sF32 StSample::_getLiveRecOscFadeAmt(void) {
   return liverec_osc_fade_amt;
}

void StSample::_setLiveRecSkipInputNumFrames(sUI _numFrames) {
   if(_numFrames > 4096u)
      _numFrames = 4096u;
   liverec_skip_input_num_frames = (sSI)_numFrames;
}

sUI StSample::_getLiveRecSkipInputNumFrames(void) {
   return sUI(liverec_skip_input_num_frames);
}

void StSample::_setLiveRecOscPreFilter(sF32 _cutoff01) {
   liverec_osc_pre_filter = _cutoff01;
}

sF32 StSample::_getLiveRecOscPreFilter(void) {
   return liverec_osc_pre_filter;
}

sBool StSample::keepInitialOscCopyOverrideBuffer(void) const {
   return
      !b_liverec_clear                                    &&
      (STSAMPLE_LIVEREC_OSC_MODE_OFF != liverec_osc_mode) &&
      (YAC_MAYBE == b_liverec_copy)/*continuous*/         &&
      (liverec_osc_fade_amt < 0.99999f)
      ;
}

void StSample::_setLiveRecOscResample(sBool _bEnable) {
   b_liverec_osc_resample = _bEnable;
}

sBool StSample::_getLiveRecOscResample(void) {
   return b_liverec_osc_resample;
}

void StSample::_setLiveRecOscSustain(sBool _bEnable) {
   b_liverec_osc_sustain = _bEnable;
}

sBool StSample::_getLiveRecOscSustain(void) {
   return b_liverec_osc_sustain;
}

void StSample::_setLiveRecLoopIdx(sUI _idx) {
   liverec_loop_idx = _idx;
}

sUI StSample::_getLiveRecLoopIdx(void) {
   return liverec_loop_idx;
}

void StSample::_setLiveRecDoubleBufferMode(sUI _mode) {
   liverec_doublebuffer_mode = _mode;
}

sUI StSample::_getLiveRecDoubleBufferMode(void) {
   return liverec_doublebuffer_mode;
}

void StSample::_setLiveRecMode(sUI _mode) {
   liverec_mode = _mode;
}

sUI StSample::_getLiveRecMode(void) {
   return liverec_mode;
}

void StSample::_setLiveRecThresholdOn(sF32 _threshold) {
   liverec_threshold_on = _threshold;
}

sF32 StSample::_getLiveRecThresholdOn(void) {
   return liverec_threshold_on;
}

void StSample::_setLiveRecThresholdOff(sF32 _threshold) {
   liverec_threshold_off = _threshold;
}

sF32 StSample::_getLiveRecThresholdOff(void) {
   return liverec_threshold_off;
}

sBool StSample::isLiveRecActive(void) {
   return b_liverec && (b_liverec_active ||
                        (STSAMPLE_LIVEREC_MODE_WAIT_REPEAT         == liverec_mode) ||
                        (STSAMPLE_LIVEREC_MODE_WAIT_SILENCE_REPEAT == liverec_mode)
                        );
}

void StSample::_startLiveRecording(void) {
   if(b_liverec)
   {
      // // sBool bActive = isLiveRecActive();
      sBool bActive = b_liverec_active;
      // Dyac_host_printf("xxx StSample::startLiveRecording: this=%p bActive=%d\n", this, bActive);

      // // if(bActive && ((STSAMPLE_LIVEREC_DBLBUF_MODE_END == liverec_doublebuffer_mode) || (STSAMPLE_LIVEREC_DBLBUF_MODE_COPY == liverec_doublebuffer_mode)))
      if(bActive && (STSAMPLE_LIVEREC_DBLBUF_MODE_END == liverec_doublebuffer_mode))
      {
         // Restart recording (before end of recording buffer was reached) => Swap buffers
         handleLiveRecEnd(YAC_TRUE/*bRestart*/);
      }
      else if(bActive && (STSAMPLE_LIVEREC_DBLBUF_MODE_VOICE == liverec_doublebuffer_mode))
      {
         // Still recording and no new voice has been started, yet
         handleLiveRecEnd(YAC_TRUE/*bRestart*/);
      }
      // // else if(b_liverec_doublebuffer_swap_queued && (STSAMPLE_LIVEREC_DBLBUF_MODE_REC == liverec_doublebuffer_mode))
      else if(/*bActive &&*/ (STSAMPLE_LIVEREC_DBLBUF_MODE_REC == liverec_doublebuffer_mode))
      {
         // Switch to previously recorded buffer (may only have been filled partially)
         // (todo) optionally clear remaining area
         // Dyac_host_printf("xxx liverec dblbuf swap\n");
         handleLiveRecEnd(YAC_TRUE/*bRestart*/);
      }

      sBool bLazyFreeFrontBuffer = YAC_TRUE;

      if((NULL != waveform) && (NULL != waveform->sample_data) && (waveform->num_channels > 0u))
      {
         const YAC_FloatArray *faSmp = waveform->sample_data;
         sUI wfNumCh     = waveform->num_channels;
         sUI wfNumFrames = faSmp->num_elements / wfNumCh;

         if((wfNumFrames > 0u) && (NULL != faSmp->elements))
         {
            if(b_liverec_copy && (STSAMPLE_LIVEREC_OSC_MODE_OFF == liverec_osc_mode))
            {
               // Lazy-alloc/resize front buffer
               sUI reqSz = (wfNumFrames * wfNumCh);
               if(liverec_front_buffer_sz != reqSz)
                  YAC_DELETE_ARRAY_SAFE(liverec_front_buffer);
               if(NULL == liverec_front_buffer)
               {
                  liverec_front_buffer = new(std::nothrow)sF32[reqSz];
                  if(NULL != liverec_front_buffer)
                  {
                     liverec_front_buffer_sz = reqSz;
                     ::memset((void*)liverec_front_buffer, 0, sizeof(sF32)*liverec_front_buffer_sz);
                  }
               }
               // Copy previously captured area to front buffer
               if(NULL != liverec_front_buffer)
               {
                  if((liverec_loop_offset <= wfNumFrames) &&
                     ( (liverec_loop_offset + liverec_loop_len) <= wfNumFrames )
                     )
                  {
                     bLazyFreeFrontBuffer = YAC_FALSE;
                     ::memcpy((void*)(liverec_front_buffer + liverec_loop_offset*wfNumCh),
                              (void*)(faSmp->elements + liverec_loop_offset*wfNumCh),
                              sizeof(sF32)*liverec_loop_len*wfNumCh
                              );
                     if(YAC_TRUE == b_liverec_clear)
                     {
                        // Dyac_host_printf("xxx liverec clear off=%u len=%u\n", liverec_loop_offset, liverec_loop_len);
                        ::memset((void*)(faSmp->elements + liverec_loop_offset*wfNumCh),
                                 0,
                                 sizeof(sF32)*liverec_loop_len*wfNumCh
                                 );
                     }
                     // Dyac_host_printf("xxx StSample::_startLiveRecording: liverec_front_buffer copy off=%u len=%u\n", liverec_loop_offset, liverec_loop_len);
                  }
               }
            } // if copy
            else
            {
               // direct (non-copy) mode
               if(YAC_TRUE == b_liverec_clear)
               {
                  // Dyac_host_printf("xxx liverec clear off=%u len=%u\n", liverec_loop_offset, liverec_loop_len);
                  ::memset((void*)(faSmp->elements + liverec_loop_offset*wfNumCh),
                           0,
                           sizeof(sF32)*liverec_loop_len*wfNumCh
                           );
               }
            }
         }
      }

      if(bLazyFreeFrontBuffer)
      {
         YAC_DELETE_ARRAY_SAFE(liverec_front_buffer);
      }

      b_liverec_active = YAC_TRUE;
      if(STSAMPLE_LIVEREC_MODE_CONTINUOUS_RING != liverec_mode)
      {
         liverec_frame_offset = 0u;
         liverec_num_frames   = 0u;
      }

      liverec_num_ring_frames = 0u;
      liverec_skip_input_frames_left = liverec_skip_input_num_frames;
      b_liverec_threshold_reached = YAC_FALSE;
      b_liverec_silence_reached   = YAC_TRUE;
      ui_redraw_queued = YAC_TRUE;
   }
}

void StSample::liveRecSwapBuffers(void) {

   liverec_override_loop_offset = liverec_queued_override_loop_offset;
   liverec_override_loop_len    = liverec_queued_override_loop_len;

   b_liverec_doublebuffer_swap_queued = YAC_FALSE;
   liverec_doublebuffer_rec_idx ^= 1u;
   liverec_frame_offset_xfade_amt = 0.0f;
   ui_redraw_queued = YAC_TRUE;
}

void StSample::_stopLiveRecording(void) {
   if(isLiveRecActive())
   {
      // Dyac_host_printf("xxx StSample::stopLiveRecording\n");
      b_liverec_threshold_reached = YAC_FALSE;
      b_liverec_silence_reached = YAC_TRUE;
      b_liverec_active = YAC_FALSE;
      handleLiveRecEnd(YAC_FALSE/*bRestart*/);

      // (todo) ? optionally zero remaining samples ?
   }
}

void StSample::handleLiveRecEnd(sBool _bRestart) {

   // Dyac_host_printf("xxx handleLiveRecEnd\n");

   // Update liverec_queued_override_*  (if osc mode is != OFF)
   if(b_liverec_osc_sustain)
   {
      liveRecUpdateSustainModeSampleLoops();
   }
   else
   {
      if(STSAMPLE_LIVEREC_MODE_CONTINUOUS_RING == liverec_mode)
         liveRecAdjustOscLoopSizeRing();
      else
         liveRecAdjustOscLoopSize();

      // Dyac_host_printf("xxx liverec_queued_override_loop_offset=%u len=%u\n", liverec_queued_override_loop_offset, liverec_queued_override_loop_len);
   }

   switch(liverec_doublebuffer_mode)
   {
      default:
      case STSAMPLE_LIVEREC_DBLBUF_MODE_OFF:
         b_liverec_doublebuffer_swap_queued = YAC_FALSE;
         liverec_override_loop_offset = liverec_queued_override_loop_offset;
         liverec_override_loop_len    = liverec_queued_override_loop_len;
         // Dyac_host_printf("xxxx handleLiveRecEnd: override loop off=%d len=%d\n", liverec_override_loop_offset, liverec_override_loop_len);
         break;

      case STSAMPLE_LIVEREC_DBLBUF_MODE_REC:
         // Swap when next recording is started
         if(_bRestart)
         {
            // Dyac_host_printf("xxx handleLiveRecEnd: b_liverec_doublebuffer_swap_queued=%d\n", b_liverec_doublebuffer_swap_queued);
            liveRecSwapBuffers();
         }
         else
         {
            b_liverec_doublebuffer_swap_queued = YAC_TRUE;
         }
         break;

      case STSAMPLE_LIVEREC_DBLBUF_MODE_VOICE:
         // Swap when next voice or next recording is started
         if(_bRestart)
         {
            liveRecSwapBuffers();
         }
         else
         {
            b_liverec_doublebuffer_swap_queued = YAC_TRUE;
         }
         break;

      case STSAMPLE_LIVEREC_DBLBUF_MODE_END:
      // // case STSAMPLE_LIVEREC_DBLBUF_MODE_COPY:
         // Swap immediately
         b_liverec_doublebuffer_swap_queued = YAC_FALSE;
         liveRecSwapBuffers();
         // Dyac_host_printf("xxx liverec swap buffers (rec idx is now %u)\n", liverec_doublebuffer_rec_idx);
         break;

      case STSAMPLE_LIVEREC_DBLBUF_MODE_PARAM:
         // Manual swap (do nothing right now)
         b_liverec_doublebuffer_swap_queued = YAC_FALSE;
         break;
   }
   ui_redraw_queued = YAC_TRUE;
}

void StSample::handleLiveRecDeferredBufferSwap(void) {
   // called by StSampleVoice::reallyStartVoice()
   if(b_liverec_doublebuffer_swap_queued)
   {
      b_liverec_doublebuffer_swap_queued = YAC_FALSE;

      if(STSAMPLE_LIVEREC_DBLBUF_MODE_VOICE == liverec_doublebuffer_mode)
      {
         liveRecSwapBuffers();
      }
   }
}

sSI StSample::liveRecFindThresholdIdx(const sF32 *_input, sUI _numFrames, sF32 _threshold) {
   // (todo) require "n" frames to exceed threshold ?
   for(sUI i = 0u; i < _numFrames; i++)
   {
      sF32 inAbs = sABS(_input[i]);
      if(inAbs >= _threshold)
         return sSI(i);
   }
   return -1;
}

void StSample::handleLiveRecording(sF32 *_out, const sF32*const*_inputs, sUI _numFrames, sSI _loopShift) {
   if(isLiveRecActive() && !b_liverec_pause)
   {
      // Dyac_host_printf("xxx StSample::handleLiveRecording numFrames=%u\n", _numFrames);
      // // return; //xxxxx
      // Dyac_host_printf("xxx StSample::handleLiveRecording: this=%p loop shift=%d frame_offset=%u numFrames=%u\n", this, _loopShift, liverec_frame_offset, _numFrames);

      const sF32 *inputL = _inputs[liverec_input_idx];
      const sF32 *inputR = inputL;
      if(b_liverec_stereo)
      {
         if((liverec_input_idx + 1u) < STSAMPLE_MAX_INPUTS)
            inputR = _inputs[liverec_input_idx + 1u];
      }

      if((NULL != inputL) && (NULL != inputR))
      {
         sBool bLiveRecDone = YAC_TRUE;

         if((NULL != waveform) && (NULL != waveform->sample_data) && (waveform->num_channels > 0u))
         {
            const YAC_FloatArray *faSmp = waveform->sample_data;
            sUI wfNumCh = waveform->num_channels;
            sUI wfNumFrames = faSmp->num_elements / wfNumCh;

            if((wfNumFrames > 0u) && (NULL != faSmp->elements))
            {
               if(NULL != sample_loops)
               {
                  sUI numLoops = sample_loops->num_elements / 3;

                  sSI loopIdx = sSI(liverec_loop_idx) + _loopShift;

                  // Dyac_host_printf("xxx StSample::handleLiveRecording numLoops=%u loopIdx=%d (shift=%d)\n", numLoops, loopIdx, _loopShift);

                  if(sUI(loopIdx) < numLoops)
                  {
                     const sUI *loopDat = (const sUI*) sample_loops->elements;

                     sUI dstOff = loopDat[sUI(loopIdx) * 3u + 0u];
                     sUI dstLen = loopDat[sUI(loopIdx) * 3u + 1u];

                     // Dyac_host_printf("xxx liveRec: dstOff=%u dstLen=%u loopIdx=%u\n", dstOff, dstLen, sUI(loopIdx));

                     if(b_liverec_copy &&
                        (STSAMPLE_LIVEREC_OSC_MODE_OFF != liverec_osc_mode) &&
                        (dstLen > STSAMPLEVOICE_OVERRIDE_SMPDAT_SZ)
                        )
                     {
                        dstLen = STSAMPLEVOICE_OVERRIDE_SMPDAT_SZ;
                     }

                     liverec_loop_offset = dstOff;
                     liverec_loop_len    = dstLen;

                     if(STSAMPLE_LIVEREC_DBLBUF_MODE_OFF != liverec_doublebuffer_mode)
                     {
                        if(liverec_doublebuffer_rec_idx & 1u)
                           dstOff += (wfNumFrames >> 1u);  // record to 2nd half of waveform
                     }

                     // handle threshold recording
                     sSI numWriteSmp  = 0;
                     sSI numWriteZero = 0;
                     sSI numWriteSkip = 0;
                     sSI thresholdFrameIdx = -1;
                     sSI inOff = 0;

                     // Latency correction (skip input samples)
                     if(liverec_skip_input_frames_left > 0)
                     {
                        if(sSI(_numFrames) <= liverec_skip_input_frames_left)
                        {
                           liverec_skip_input_frames_left -= _numFrames;
                           return;
                        }
                        else
                        {
                           _numFrames -= liverec_skip_input_frames_left;
                           inOff += liverec_skip_input_frames_left;
                           liverec_skip_input_frames_left = 0;
                        }
                     }

                     bLiveRecDone = YAC_FALSE;

                     switch(liverec_mode)
                     {
                        default:
                        case STSAMPLE_LIVEREC_MODE_TRIG:
                           numWriteSmp = sSI(_numFrames);
                           break;

                        case STSAMPLE_LIVEREC_MODE_ZERO:
                           if(b_liverec_threshold_reached)
                           {
                              numWriteSmp = sSI(_numFrames);
                           }
                           else
                           {
                              thresholdFrameIdx = liveRecFindThresholdIdx(inputL, _numFrames, liverec_threshold_on);
                              if((-1 == thresholdFrameIdx) && b_liverec_stereo)
                                 thresholdFrameIdx = liveRecFindThresholdIdx(inputR, _numFrames, liverec_threshold_on);
                              if(-1 != thresholdFrameIdx)
                              {
                                 numWriteZero = thresholdFrameIdx;
                                 numWriteSmp = sSI(_numFrames) - numWriteZero;
                                 inOff = numWriteZero;
                                 b_liverec_threshold_reached = YAC_TRUE;
                              }
                              else
                              {
                                 numWriteZero = sSI(_numFrames);
                              }
                           }
                           break;

                        case STSAMPLE_LIVEREC_MODE_KEEP:
                           if(b_liverec_threshold_reached)
                           {
                              numWriteSmp = sSI(_numFrames);
                           }
                           else
                           {
                              thresholdFrameIdx = liveRecFindThresholdIdx(inputL, _numFrames, liverec_threshold_on);
                              if((-1 == thresholdFrameIdx) && b_liverec_stereo)
                                 thresholdFrameIdx = liveRecFindThresholdIdx(inputR, _numFrames, liverec_threshold_on);
                              if(-1 != thresholdFrameIdx)
                              {
                                 numWriteSkip = thresholdFrameIdx;
                                 numWriteSmp = sSI(_numFrames) - numWriteSkip;
                                 inOff = numWriteSkip;
                                 b_liverec_threshold_reached = YAC_TRUE;
                              }
                              else
                              {
                                 numWriteSkip = sSI(_numFrames);
                              }
                           }
                           break;

                        case STSAMPLE_LIVEREC_MODE_WAIT_ONCE:
                        case STSAMPLE_LIVEREC_MODE_WAIT_REPEAT:
                           if(b_liverec_threshold_reached)
                           {
                              numWriteSmp = sSI(_numFrames);
                           }
                           else
                           {
                              thresholdFrameIdx = liveRecFindThresholdIdx(inputL, _numFrames, liverec_threshold_on);
                              if((-1 == thresholdFrameIdx) && b_liverec_stereo)
                                 thresholdFrameIdx = liveRecFindThresholdIdx(inputR, _numFrames, liverec_threshold_on);
                              if(-1 != thresholdFrameIdx)
                              {
                                 numWriteSmp = sSI(_numFrames) - sUI(thresholdFrameIdx);
                                 b_liverec_threshold_reached = YAC_TRUE;
                                 b_liverec_active = true;
                              }
                           }
                           break;

                        case STSAMPLE_LIVEREC_MODE_WAIT_SILENCE_REPEAT:
                           // Dyac_host_printf("xxx WAIT_SILENCE_REPEAT: threshold=%d silence=%d\n", b_liverec_threshold_reached, b_liverec_silence_reached);
                           if(b_liverec_threshold_reached)
                           {
                              numWriteSmp = sSI(_numFrames);
                           }
                           else
                           {
                              if(b_liverec_silence_reached)
                              {
                                 thresholdFrameIdx = liveRecFindThresholdIdx(inputL, _numFrames, liverec_threshold_on);
                                 if((-1 == thresholdFrameIdx) && b_liverec_stereo)
                                    thresholdFrameIdx = liveRecFindThresholdIdx(inputR, _numFrames, liverec_threshold_on);
                                 if(-1 != thresholdFrameIdx)
                                 {
                                    numWriteSmp = sSI(_numFrames) - sUI(thresholdFrameIdx);
                                    b_liverec_threshold_reached = YAC_TRUE;
                                    b_liverec_silence_reached   = YAC_FALSE;
                                    b_liverec_active = true;
                                 }
                              }
                              else
                              {
                                 thresholdFrameIdx = liveRecFindThresholdIdx(inputL, _numFrames, liverec_threshold_off);
                                 if((-1 == thresholdFrameIdx) && b_liverec_stereo)
                                    thresholdFrameIdx = liveRecFindThresholdIdx(inputR, _numFrames, liverec_threshold_off);
                                 if(-1 == thresholdFrameIdx)
                                 {
                                    // Dyac_host_printf("xxx silence reached\n");
                                    b_liverec_silence_reached = YAC_TRUE;
                                    b_liverec_threshold_reached = YAC_FALSE;

                                    // Done, auto-stop recording
                                    bLiveRecDone = YAC_TRUE;

                                    handleLiveRecEnd(YAC_FALSE/*bRestart*/);
                                 }
                              }
                           }
                           break;

                        case STSAMPLE_LIVEREC_MODE_CONTINUOUS:
                        case STSAMPLE_LIVEREC_MODE_CONTINUOUS_RING:
                           numWriteSmp = sSI(_numFrames);
                           break;
                     }

                     sSI numReq = (numWriteSkip + numWriteZero + numWriteSmp);

                     // Dyac_host_printf("xxx StSample::handleLiveRecording: dstOff=%u (#wavframes=%u) dstLen=%u #skip=%u #zero=%u #write=%u => req=%u numFrames=%u\n", dstOff, wfNumFrames, dstLen, numWriteSkip, numWriteZero, numWriteSmp, numReq, _numFrames);

                     if(dstOff < wfNumFrames)
                     {
                        const sF32 cInVolL = (liverec_input_pan < 0.0f) ? 1.0f : (1.0f - liverec_input_pan);
                        const sF32 cInVolR = (liverec_input_pan > 0.0f) ? 1.0f : (1.0f + liverec_input_pan);

                        if(numReq > 0)
                        {
                           sBool bWriteOk = YAC_FALSE;
                           sF32 *smp = faSmp->elements;
                           sSI numFramesLeft = sSI(_numFrames);

                           if(STSAMPLE_LIVEREC_MODE_CONTINUOUS_RING == liverec_mode)
                           {
                              bWriteOk = YAC_TRUE;
                              liverec_ring_size = dstLen;
                              liverec_frame_offset = liverec_frame_offset % dstLen;

                              sUI ringOff = liverec_frame_offset;

                              if(numWriteSkip > 0)
                              {
                                 if(numWriteSkip > numFramesLeft)
                                    numWriteSkip = numFramesLeft;
                                 ringOff += sUI(numWriteSkip);
                                 numFramesLeft -= numWriteSkip;
                              }

                              if(numWriteZero > 0)
                              {
                                 if(numWriteZero > numFramesLeft)
                                    numWriteZero = numFramesLeft;
                                 if(1u == wfNumCh)
                                 {
                                    sUI smpOff = ringOff;
                                    for(sSI i = 0; i < numWriteZero; i++)
                                    {
                                       smp[dstOff + smpOff] = 0.0f;
                                       smpOff = (smpOff + 1u) % dstLen;
                                    }
                                 }
                                 else
                                 {
                                    sUI smpOff = ringOff;
                                    for(sSI i = 0; i < numWriteZero; i++)
                                    {
                                       smp[((dstOff + smpOff) << 1) + 0u] = 0.0f;
                                       smp[((dstOff + smpOff) << 1) + 1u] = 0.0f;
                                       smpOff = (smpOff + 1u) % dstLen;
                                    }
                                 }

                                 ringOff += numWriteZero;
                                 numFramesLeft -= numWriteZero;
                              }

                              if(numWriteSmp > numFramesLeft)
                                 numWriteSmp = numFramesLeft;

                              if(b_liverec_stereo)
                              {
                                 if(1u == wfNumCh)
                                 {
                                    // Stereo to mono (discard right channel)
                                    sUI smpOff = ringOff;
                                    sSI j = inOff;
                                    if(YAC_MAYBE == b_liverec_clear)
                                    {
                                       // overdub
                                       for(sSI i = 0; i < numWriteSmp; i++)
                                       {
                                          smp[((dstOff + smpOff))] += inputL[j++];
                                          smpOff = (smpOff + 1u) % dstLen;
                                       }
                                    }
                                    else
                                    {
                                       // replace
                                       for(sSI i = 0; i < numWriteSmp; i++)
                                       {
                                          smp[((dstOff + smpOff))] = inputL[j++];
                                          smpOff = (smpOff + 1u) % dstLen;
                                       }
                                    }
                                 }
                                 else
                                 {
                                    // (Dual Mono) Stereo to (interleaved) stereo
                                    sUI smpOff = ringOff;
                                    sSI j = inOff;
                                    if(YAC_MAYBE == b_liverec_clear)
                                    {
                                       // overdub
                                       for(sSI i = 0; i < numWriteSmp; i++)
                                       {
                                          smp[((dstOff + smpOff) << 1) + 0u] += inputL[j  ] * cInVolL;
                                          smp[((dstOff + smpOff) << 1) + 1u] += inputR[j++] * cInVolR;
                                          smpOff = (smpOff + 1u) % dstLen;
                                       }
                                    }
                                    else
                                    {
                                       // replace
                                       for(sSI i = 0; i < numWriteSmp; i++)
                                       {
                                          smp[((dstOff + smpOff) << 1) + 0u] = inputL[j  ] * cInVolL;
                                          smp[((dstOff + smpOff) << 1) + 1u] = inputR[j++] * cInVolR;
                                          smpOff = (smpOff + 1u) % dstLen;
                                       }
                                    }
                                 }
                              }
                              else
                              {
                                 if(1u == wfNumCh)
                                 {
                                    // Mono to mono
                                    sUI smpOff = ringOff;
                                    sSI j = inOff;
                                    if(YAC_MAYBE == b_liverec_clear)
                                    {
                                       // overdub
                                       for(sSI i = 0; i < numWriteSmp; i++)
                                       {
                                          smp[(dstOff + smpOff)] += inputL[j++];
                                          smpOff = (smpOff + 1u) % dstLen;
                                       }
                                    }
                                    else
                                    {
                                       // replace
                                       for(sSI i = 0; i < numWriteSmp; i++)
                                       {
                                          smp[(dstOff + smpOff)] = inputL[j++];
                                          smpOff = (smpOff + 1u) % dstLen;
                                       }
                                    }
                                 }
                                 else
                                 {
                                    // Mono to (interleaved) stereo (duplicate)
                                    sUI smpOff = dstOff << 1u;
                                    sSI j = inOff;
                                    if(YAC_MAYBE == b_liverec_clear)
                                    {
                                       // overdub
                                       for(sSI i = 0; i < numWriteSmp; i++)
                                       {
                                          const sF32 t = inputL[j++];
                                          smp[((dstOff + smpOff) << 1) + 0u] += t * cInVolL;
                                          smp[((dstOff + smpOff) << 1) + 1u] += t * cInVolR;
                                          smpOff = (smpOff + 1u) % dstLen;
                                       }
                                    }
                                    else
                                    {
                                       // replace
                                       for(sSI i = 0; i < numWriteSmp; i++)
                                       {
                                          const sF32 t = inputL[j++];
                                          smp[((dstOff + smpOff) << 1) + 0u] = t * cInVolL;
                                          smp[((dstOff + smpOff) << 1) + 1u] = t * cInVolR;
                                          smpOff = (smpOff + 1u) % dstLen;
                                       }
                                    }
                                 }
                              }

                              numFramesLeft -= numWriteSmp;
                           }
                           else
                           {
                              // Linear output
                              dstOff += liverec_frame_offset;

                              if(liverec_frame_offset < dstLen)
                              {
                                 if(sSI(liverec_frame_offset) >= sSI(dstLen))
                                    numFramesLeft = 0;
                                 else if( (sSI(liverec_frame_offset) + numReq) > sSI(dstLen) )
                                 {
                                    // Clip to loop area end
                                    numFramesLeft = sSI(dstLen - liverec_frame_offset);
                                 }
                                 else if( (sSI(dstOff) + numReq) > sSI(wfNumFrames) )
                                    numFramesLeft = sSI(wfNumFrames - dstOff);  // should not be reachable


                                 if(numWriteSkip > 0)
                                 {
                                    if(numWriteSkip > numFramesLeft)
                                       numWriteSkip = numFramesLeft;
                                    dstOff += sUI(numWriteSkip);
                                    numFramesLeft -= numWriteSkip;
                                 }

                                 if(numWriteZero > 0)
                                 {
                                    if(numWriteZero > numFramesLeft)
                                       numWriteZero = numFramesLeft;
                                    if(1u == wfNumCh)
                                    {
                                       sUI smpOff = dstOff;
                                       for(sSI i = 0; i < numWriteZero; i++)
                                       {
                                          smp[smpOff++] = 0.0f;
                                       }
                                    }
                                    else
                                    {
                                       sUI smpOff = (dstOff << 1);
                                       for(sSI i = 0; i < numWriteZero; i++)
                                       {
                                          smp[smpOff++] = 0.0f;
                                          smp[smpOff++] = 0.0f;
                                       }
                                    }

                                    dstOff += numWriteZero;
                                    numFramesLeft -= numWriteZero;
                                 }

                                 if(numWriteSmp > numFramesLeft)
                                    numWriteSmp = numFramesLeft;

                                 if(b_liverec_stereo)
                                 {
                                    if(1u == wfNumCh)
                                    {
                                       // Stereo to mono (discard right channel)
                                       sUI smpOff = dstOff;
                                       sSI j = inOff;
                                       if(YAC_MAYBE == b_liverec_clear)
                                       {
                                          // overdub
                                          for(sSI i = 0; i < numWriteSmp; i++)
                                          {
                                             smp[smpOff++] += inputL[j++];
                                          }
                                       }
                                       else
                                       {
                                          // replace
                                          for(sSI i = 0; i < numWriteSmp; i++)
                                          {
                                             smp[smpOff++] = inputL[j++];
                                          }
                                       }
                                    }
                                    else
                                    {
                                       // (Dual Mono) Stereo to (interleaved) stereo
                                       sUI smpOff = dstOff << 1u;
                                       sSI j = inOff;
                                       if(YAC_MAYBE == b_liverec_clear)
                                       {
                                          // overdub
                                          for(sSI i = 0; i < numWriteSmp; i++)
                                          {
                                             smp[smpOff++] += inputL[j  ] * cInVolL;
                                             smp[smpOff++] += inputR[j++] * cInVolR;
                                          }
                                       }
                                       else
                                       {
                                          // replace
                                          for(sSI i = 0; i < numWriteSmp; i++)
                                          {
                                             smp[smpOff++] = inputL[j  ] * cInVolL;
                                             smp[smpOff++] = inputR[j++] * cInVolR;
                                          }
                                       }
                                    }
                                 }
                                 else
                                 {
                                    if(1u == wfNumCh)
                                    {
                                       // Mono to mono
                                       sUI smpOff = dstOff;
                                       sSI j = inOff;
                                       if(YAC_MAYBE == b_liverec_clear)
                                       {
                                          // overdub
                                          for(sSI i = 0; i < numWriteSmp; i++)
                                          {
                                             smp[smpOff++] += inputL[j++];
                                          }
                                       }
                                       else
                                       {
                                          // replace
                                          for(sSI i = 0; i < numWriteSmp; i++)
                                          {
                                             smp[smpOff++] = inputL[j++];
                                          }
                                       }
                                    }
                                    else
                                    {
                                       // Mono to (interleaved) stereo (duplicate)
                                       sUI smpOff = dstOff << 1u;
                                       sSI j = inOff;
                                       if(YAC_MAYBE == b_liverec_clear)
                                       {
                                          // overdub
                                          for(sSI i = 0; i < numWriteSmp; i++)
                                          {
                                             const sF32 t = inputL[j++];
                                             smp[smpOff++] += t * cInVolL;
                                             smp[smpOff++] += t * cInVolR;
                                          }
                                       }
                                       else
                                       {
                                          // replace
                                          for(sSI i = 0; i < numWriteSmp; i++)
                                          {
                                             const sF32 t = inputL[j++];
                                             smp[smpOff++] = t * cInVolL;
                                             smp[smpOff++] = t * cInVolR;
                                          }
                                       }
                                    }
                                 }

                                 numFramesLeft -= numWriteSmp;
                                 bWriteOk = YAC_TRUE;
                              }
                              else
                                 numFramesLeft = 0;
                           }

                           if(bWriteOk)
                           {
                              // Dyac_host_printf("xxx StSample::handleLiveRecording: write %u frames to liverec_frame_offset=%d..%d\n", sUI(numWriteSkip + numWriteZero + numWriteSmp), liverec_frame_offset, liverec_frame_offset+sUI(numWriteSkip + numWriteZero + numWriteSmp));

                              ui_redraw_queued |= YAC_TRUE;

                              sUI numAdv = sUI(numWriteSkip + numWriteZero + numWriteSmp);

                              if(numFramesLeft < 0)
                                 Dyac_host_printf("xxx !!!!!! numFrames (%d) < 0 !!!!\n", numFramesLeft);

                              if(numAdv > _numFrames)
                                 Dyac_host_printf("xxx !!!!!! numFrames exceeded\n");

                              if(STSAMPLE_LIVEREC_MODE_CONTINUOUS_RING == liverec_mode)
                              {
                                 // Dyac_host_printf("xxx added numAdv=%u frames @off=%u\n", numAdv, liverec_frame_offset);
                                 liverec_frame_offset = (liverec_frame_offset + numAdv) % dstLen;
                                 liverec_num_frames += numAdv;

                                 if(liverec_num_frames >= dstLen)
                                 {
                                    liverec_num_frames = dstLen;
                                    liverec_num_ring_frames += numAdv;

                                    if(liverec_num_ring_frames >= dstLen)
                                    {
                                       liverec_num_ring_frames = 0u;

                                       //    // // Done, auto-stop recording
                                       //    // bLiveRecDone = YAC_TRUE;

                                       // Loop-Detection + swap buffers
                                       //  (todo) skip call to liveRecAdjustOscLoopSizeRing() (already done below)
                                       handleLiveRecEnd(YAC_FALSE/*bRestart*/);
                                    }
                                 }
                                 else
                                 {
                                    // // liverec_frame_offset_xfade_amt = sF32(liverec_frame_offset) / dstLen;
                                    liverec_frame_offset_xfade_amt = 0.0f;
                                 }
                              } // end if LIVEREC_MODE_CONTINUOUS_RING
                              else
                              {
                                 if(sUI(liverec_frame_offset + numAdv) > dstLen)
                                    Dyac_host_printf("xxx !!!!!! dstLen exceeded\n");
                                 liverec_frame_offset += numAdv;

                                 liverec_num_frames += numAdv;

                                 if(liverec_num_frames >= dstLen)
                                 {
                                    if(STSAMPLE_LIVEREC_MODE_WAIT_SILENCE_REPEAT == liverec_mode)
                                    {
                                       b_liverec_threshold_reached = YAC_FALSE;
                                       b_liverec_silence_reached   = YAC_FALSE;
                                       // now wait for silence
                                    }
                                    else
                                    {
                                       // Done, auto-stop recording
                                       bLiveRecDone = YAC_TRUE;
                                       // Dyac_host_printf("xxx liverec done, call handleLiveRecEnd(false)\n");
                                       handleLiveRecEnd(YAC_FALSE/*bRestart*/);
                                    }
                                 }
                                 else
                                 {
                                    liverec_frame_offset_xfade_amt = sF32(liverec_frame_offset) / dstLen;
                                 }
                              }
                           }
                           else
                           {
                              // Loop len edited after live recording started ?
                              if(STSAMPLE_LIVEREC_MODE_WAIT_SILENCE_REPEAT != liverec_mode)
                                 bLiveRecDone = YAC_TRUE;
                           }
                        } // if numReq > 0
                     } // if dstOff < wfNumFrames
                  } // if loop_idx < numLoops
               } // if sample_loops
            } // if wfNumFrames>0
         } // if wf sample_data

         if(bLiveRecDone)
         {
            // Dyac_host_printf("xxx StSample::handleLiveRecording: auto-stop recording after %u frames\n", liverec_frame_offset);
            if(! ( (STSAMPLE_LIVEREC_MODE_CONTINUOUS      == liverec_mode) ||
                   (STSAMPLE_LIVEREC_MODE_CONTINUOUS_RING == liverec_mode)
                   )
               )
            {
               b_liverec_active            = YAC_FALSE;
               b_liverec_threshold_reached = YAC_FALSE;
               // // b_liverec_silence_reached   = YAC_FALSE;
            }
            liverec_frame_offset = 0u;
            liverec_num_frames   = 0u;
         }

         if(STSAMPLE_LIVEREC_MODE_CONTINUOUS_RING == liverec_mode)
            liveRecAdjustOscLoopSizeRing();


      } // if inputL && inputR
   } // if b_live_rec_active

   if(b_liverec_monitor)
   {
      liveRecCopyInputToOutput(_out, _inputs, _numFrames);
   }

   // yac_host->printf("xxx StSample::handleLiveRecording: LEAVE\n");
}

void StSample::liveRecCopyInputToOutput(sF32 *_out, const sF32*const*_inputs, sUI _numFrames) {
   const sF32 *inputL = _inputs[liverec_input_idx];
   const sF32 *inputR = inputL;

   if(b_liverec_stereo)
   {
      if((liverec_input_idx + 1u) < STSAMPLE_MAX_INPUTS)
         inputR = _inputs[liverec_input_idx + 1u];
   }

   if((NULL != inputL) && (NULL != inputR))
   {
      if(b_liverec_stereo)
      {
         // (Dual Mono) Stereo to (interleaved) stereo
         sUI k = 0u;
         if(YAC_MAYBE == b_liverec_monitor)
         {
            // overdub
            for(sUI i = 0u; i < _numFrames; i++)
            {
               _out[k++] += inputL[i];
               _out[k++] += inputR[i];
            }
         }
         else
         {
            // replace
            for(sUI i = 0u; i < _numFrames; i++)
            {
               _out[k++] = inputL[i];
               _out[k++] = inputR[i];
            }
         }
      }
      else
      {
         // Mono to (interleaved) stereo (duplicate left)
         sUI k = 0u;
         if(YAC_MAYBE == b_liverec_monitor)
         {
            // overdub
            for(sUI i = 0u; i < _numFrames; i++)
            {
               _out[k++] += inputL[i];
               _out[k++] += inputL[i];
            }
         }
         else
         {
            // replace
            for(sUI i = 0u; i < _numFrames; i++)
            {
               _out[k++] = inputL[i];
               _out[k++] = inputL[i];
            }
         }
      }
   }
}

sBool StSample::uiCheckResetRedrawFlag(void) {
   sBool r = ui_redraw_queued;
   ui_redraw_queued = YAC_FALSE;
   return r;
}

sBool StSample::hasLiveRecAdaptiveLoopSize(void) {
   return
      b_liverec                                           &&
      !b_liverec_osc_sustain                              &&
      (STSAMPLE_LIVEREC_OSC_MODE_OFF != liverec_osc_mode) &&
      (-1 != liverec_override_loop_offset)                ;
}

sUI StSample::liveRecGetCurrentRecBufIdx(void) {
   if(STSAMPLE_LIVEREC_DBLBUF_MODE_OFF != liverec_doublebuffer_mode)
      return liverec_doublebuffer_rec_idx;
   else
      return 0u;
}

sUI StSample::liveRecGetCurrentPlayBufIdx(void) {
   if(STSAMPLE_LIVEREC_DBLBUF_MODE_OFF != liverec_doublebuffer_mode)
      return liverec_doublebuffer_rec_idx ^ 1u;
   else
      return 0u;
}

YAC_IntArray *StSample::getSampleLoopsOverride(void) {
   if(b_liverec && b_liverec_osc_sustain)
      return &liverec_sustain_sample_loops[liveRecGetCurrentPlayBufIdx()];
   else
      return NULL;
}

void StSample::liveRecUpdateSustainModeSampleLoops(void) {
   // Dyac_host_printf("xxx liveRecUpdateSustainModeSampleLoops: ENTER\n");

   if((NULL != sample_loops) && (NULL != sample_loops->elements))
   {
      sUI recBufIdx = liveRecGetCurrentRecBufIdx();
      YAC_IntArray *iaLoops = &liverec_sustain_sample_loops[recBufIdx];

      if(iaLoops->num_elements < sample_loops->max_elements)
      {
         // Realloc
         if(NULL != iaLoops->elements)
            delete [] iaLoops->elements;
         iaLoops->elements = new(std::nothrow) sSI[sample_loops->num_elements];
         memset((void*)iaLoops->elements, 0, sizeof(sUI) * sample_loops->num_elements);
         iaLoops->max_elements = sample_loops->num_elements;
      }
      iaLoops->num_elements = sample_loops->num_elements;

      const sUI *origLoopDat = (const sUI*)sample_loops->elements;
      sUI *loopDat = (sUI*)iaLoops->elements;
      sUI numLoops = sample_loops->num_elements / 3u;

      for(sUI loopIdx = 0u; loopIdx < numLoops; loopIdx++)
      {
         sSI overrideLoopOffset;
         sSI overrideLoopLen;

         sUI inputOffset = origLoopDat[loopIdx*3u + 0u];
         sUI inputLen    = origLoopDat[loopIdx*3u + 1u];

         if(b_liverec_copy)
         {
            if(inputOffset >= STSAMPLEVOICE_OVERRIDE_SMPDAT_SZ)
               continue;
            else if((inputOffset + inputLen) >= STSAMPLEVOICE_OVERRIDE_SMPDAT_SZ)
               inputLen = STSAMPLEVOICE_OVERRIDE_SMPDAT_SZ - inputOffset;
         }

         if(loopIdx > 0u)
         {

#if 0 // liverec_frame_offset wraps around in ring buffer, don't compare to inputOffset
            if(liverec_frame_offset < inputOffset)
               inputLen = 0u; // area not recorded at all, yet
            else if(liverec_frame_offset < (inputOffset + inputLen))
               inputLen = liverec_frame_offset - inputOffset;  // not enough data, yet
#endif

            liveRecAdjustOscLoopSizeInt(overrideLoopOffset,
                                        overrideLoopLen,
                                        inputOffset,
                                        inputLen
                                        );

            // Dyac_host_printf("xxx sustain loop[%u] off=%d len=%d\n", loopIdx, overrideLoopOffset, overrideLoopLen);

            if(overrideLoopOffset >= 0)
            {
               loopDat[loopIdx*3u + 0u] = sUI(overrideLoopOffset);
               loopDat[loopIdx*3u + 1u] = sUI(overrideLoopLen);
            }
            else
            {
               // keep on playing last detected loop (may sound wrong but better than stopping the playback)
            }
         }
         else
         {
            // First loop shouldn't be played but only be used to define the recording area
            loopDat[0] = inputOffset;
            loopDat[1] = inputLen;
         }

         loopDat[loopIdx*3u + 2u] = origLoopDat[loopIdx*3u + 2u];  // copy #repeats
      }
   }
   else
   {
      liverec_sustain_sample_loops[0].num_elements = 0u;
      liverec_sustain_sample_loops[1].num_elements = 0u;
   }

   // Dyac_host_printf("xxx liveRecUpdateSustainModeSampleLoops: LEAVE\n");
}

void StSample::liveRecAdjustOscLoopSize(void) {

   sUI inputOffset = liverec_loop_offset;  // == zone loop offset
   sUI inputLen    = liverec_loop_len;     // == zone loop len

#if 0 // liverec_frame_offset wraps around in ring buffer, don't compare to inputOffset
   if(liverec_frame_offset < inputOffset)
      inputLen = 0u; // area not recorded at all, yet
   else if(liverec_frame_offset < (inputOffset + inputLen))
      inputLen = liverec_frame_offset - inputOffset;  // not enough data, yet
#endif

   liveRecAdjustOscLoopSizeInt(/*out:*/liverec_queued_override_loop_offset,
                               /*out:*/liverec_queued_override_loop_len,
                               inputOffset,
                               inputLen
                               );
}

void StSample::liveRecAdjustOscLoopSizeInt(sSI &_overrideLoopOffset,
                                           sSI &_overrideLoopLen,
                                           sUI _inputOffset,
                                           sUI _inputLen
                                           ) {

   _overrideLoopOffset = -1;
   _overrideLoopLen    = 0;

   if(STSAMPLE_LIVEREC_OSC_MODE_OFF != liverec_osc_mode)
   {
      // Dyac_host_printf("xxx liveRecAdjustOscLoopSize: liverec_frame_offset=%u\n", liverec_frame_offset);
      if(_inputLen >= 16u)
      {
         if((NULL != waveform) && (NULL != waveform->sample_data) && (waveform->num_channels > 0u))
         {
            const YAC_FloatArray *faSmp = waveform->sample_data;
            sUI wfNumCh = waveform->num_channels;
            sUI wfNumFrames = faSmp->num_elements / wfNumCh;

            if((wfNumFrames > 0u) && (NULL != faSmp->elements))
            {
               if(1u == wfNumCh)
               {
                  const sF32 *smpDat = faSmp->elements;
                  smpDat += _inputOffset;

                  if(STSAMPLE_LIVEREC_DBLBUF_MODE_OFF != liverec_doublebuffer_mode)
                  {
                     if(1u == ((liverec_doublebuffer_rec_idx ^ b_liverec_doublebuffer_swap_queued) & 1u))
                     {
                        smpDat += (wfNumFrames >> 1u);  // was recorded to 2nd half of waveform
                        // Dyac_host_printf("xxx liveRecAdjustOscLoopSize: calc for bufIdx=1\n");
                     }
                     else
                     {
                        // Dyac_host_printf("xxx liveRecAdjustOscLoopSize: calc for bufIdx=0\n");
                     }
                  }

                  sBool l = (smpDat[0] <= 0.0f);
                  sUI i;
                  sUI zcFirst = 0u;
                  sUI zcLast = 0u;

                  if(STSAMPLE_LIVEREC_OSC_MODE_EVEN == liverec_osc_mode)
                  {
                     for(i = 1u; i < _inputLen; i++)
                     {
                        sBool c = (smpDat[i] <= 0.0f);
                        if(l && !c)
                        {
                           l = c;
                           break;
                        }
                        l = c;
                     }
                     // Dyac_host_printf("xxx even zcFirst=%u\n", i);
                     zcFirst = i;
                     zcLast = i;

                     sUI flip = 0u;
                     for(; i < _inputLen; i++)
                     {
                        sBool c = (smpDat[i] <= 0.0f);
                        if(l ^ c)
                        {
                           flip ^= 1u;
                           if(0u == flip)
                              zcLast = i;
                        }
                        l = c;
                     }
                     // Dyac_host_printf("xxx even zcLast=%u\n", zcLast);
                  }
                  else if(STSAMPLE_LIVEREC_OSC_MODE_EVEN_TUNED == liverec_osc_mode)
                  {
                     for(i = 1u; i < _inputLen; i++)
                     {
                        sBool c = (smpDat[i] <= 0.0f);
                        if(l && !c)
                        {
                           l = c;
                           break;
                        }
                        l = c;
                     }
                     // Dyac_host_printf("xxx even zcFirst=%u\n", i);
                     zcFirst = i;
                     zcLast = i;

                     if((i != _inputLen) &&
                        (liverec_ref_cyclelen > 0.0f) &&
                        (_inputLen >= sUI(liverec_ref_cyclelen * 2))
                        )
                     {
                        sSI bestMatch = -1;
                        sUI lenIdeal;
                        if(b_liverec_osc_singlecycle)
                           lenIdeal = sUI(liverec_ref_cyclelen+0.5f);
                        else
                           lenIdeal = sUI((sUI((_inputLen / liverec_ref_cyclelen)+0.5f)-1u) * liverec_ref_cyclelen);
                        // Dyac_host_printf("xxx liverec even lenIdeal=%u\n", lenIdeal);

                        sUI flip = 0u;
                        for(; i < _inputLen; i++)
                        {
                           sBool c = (smpDat[i] <= 0.0f);
                           if(l ^ c)
                           {
                              flip ^= 1u;
                              if(0u == flip)
                              {
                                 sUI len = i - zcFirst;
                                 sSI dist = sABS(sSI(lenIdeal) - sSI(len));
                                 if((-1 == bestMatch) || (dist < bestMatch) )
                                 {
                                    bestMatch = dist;
                                    zcLast = i;
                                    if(len > lenIdeal)
                                       break;
                                 }
                              }
                           }
                           l = c;
                        }
                     }
                     // Dyac_host_printf("xxx even zcLast=%u\n", zcLast);
                  }
                  else if(STSAMPLE_LIVEREC_OSC_MODE_TUNED == liverec_osc_mode)
                  {
                     for(i = 1u; i < _inputLen; i++)
                     {
                        sBool c = (smpDat[i] <= 0.0f);
                        if(l ^ c)
                        {
                           l = c;
                           break;
                        }
                        l = c;
                     }
                     // Dyac_host_printf("xxx odd zcFirst=%u\n", i);
                     zcFirst = i;

                     // Dyac_host_printf("xxx liverec_ref_cyclelen=%f liverec_frame_offset=%u\n", liverec_ref_cyclelen, liverec_frame_offset);
                     if((i != _inputLen) &&
                        (liverec_ref_cyclelen > 0.0f) &&
                        (_inputLen >= sUI(liverec_ref_cyclelen * 2))
                        )
                     {
                        sSI bestMatch = -1;
                        sUI lenIdeal;
                        if(b_liverec_osc_singlecycle)
                           lenIdeal = sUI(liverec_ref_cyclelen+0.5f);
                        else
                           lenIdeal = sUI((sUI((_inputLen / liverec_ref_cyclelen)+0.5f)-1u) * liverec_ref_cyclelen);
                        // Dyac_host_printf("xxx liverec lenIdeal=%u\n", lenIdeal);
                        sBool l = (smpDat[_inputLen - 1u] <= 0.0f);
                        for(i = (_inputLen - 2u); i < _inputLen; i--)
                        {
                           sBool c = (smpDat[i] <= 0.0f);
                           if(l ^ c)
                           {
                              sUI len = i - zcFirst;
                              sSI dist = sABS(sSI(lenIdeal) - sSI(len));
                              if((-1 == bestMatch) || (dist < bestMatch) )
                              {
                                 bestMatch = dist;
                                 zcLast = i;
                              }
                           }
                           l = c;
                        }
                     }
                  }
                  else if(STSAMPLE_LIVEREC_OSC_MODE_EDGE_P_TUNED == liverec_osc_mode)
                  {
                     l = (smpDat[0] >= 0.0f);
                     for(i = 1u; i < _inputLen; i++)
                     {
                        sBool c = (smpDat[i] >= 0.0f);
                        if(c && (l ^ c))
                        {
                           break;
                        }
                        l = c;
                     }
                     // Dyac_host_printf("xxx odd zcFirst=%u\n", i);
                     zcFirst = i;

                     // Dyac_host_printf("xxx liverec_ref_cyclelen=%f liverec_frame_offset=%u\n", liverec_ref_cyclelen, liverec_frame_offset);
                     if((i != _inputLen) &&
                        (liverec_ref_cyclelen > 0.0f) &&
                        (_inputLen >= sUI(liverec_ref_cyclelen * 2))
                        )
                     {
                        sSI bestMatch = -1;
                        sUI lenIdeal;
                        if(b_liverec_osc_singlecycle)
                           lenIdeal = sUI(liverec_ref_cyclelen+0.5f);
                        else
                           lenIdeal = sUI((sUI((_inputLen / liverec_ref_cyclelen)+0.5f)-1u) * liverec_ref_cyclelen);
                        // Dyac_host_printf("xxx liverec lenIdeal=%u\n", lenIdeal);
                        sBool l = (smpDat[_inputLen - 1u] >= 0.0f);
                        for(i = (_inputLen - 2u); i < _inputLen; i--)
                        {
                           sBool c = (smpDat[i] >= 0.0f);
                           if(!c && (l ^ c))
                           {
                              sUI len = i - zcFirst + 1u;
                              sSI dist = sABS(sSI(lenIdeal) - sSI(len));
                              if((-1 == bestMatch) || (dist < bestMatch) )
                              {
                                 bestMatch = dist;
                                 zcLast = i;
                              }
                           }
                           l = c;
                        }
                     }

                  }
                  else if(STSAMPLE_LIVEREC_OSC_MODE_EDGE_P_SLOPE_TUNED == liverec_osc_mode)
                  {
                     // rising edge
#define SLOPE_NUM_FRAMES  (12u)
                     l = (smpDat[0u] >= 0.0f);
                     sF32 slopeStart = 0.0f;
                     for(i = 1u; i < liverec_ring_size; i++)
                     {
                        sBool c = (smpDat[i % liverec_ring_size] >= 0.0f);
                        if(c && (l ^ c))
                        {
                           for(sUI j = 0u; j < SLOPE_NUM_FRAMES; j++)
                           {
                              slopeStart += smpDat[(i + j) % liverec_ring_size];
                           }
                           l = c;  // => true
                           break;
                        }
                        l = c;
                     }
                     // Dyac_host_printf("xxx odd zcFirst=%u\n", i);
                     zcFirst = i;
                     zcLast = i;

                     // Dyac_host_printf("xxx liverec_ref_cyclelen=%f liverec_ring_size=%u expect at least %u\n", liverec_ref_cyclelen, liverec_num_frames, sUI(liverec_ref_cyclelen * 2u));

                     if((i != liverec_ring_size) &&
                        (liverec_ref_cyclelen > 0.0f) &&
                        (liverec_ring_size >= sUI(liverec_ref_cyclelen * 3u))
                        )
                     {
                        sSI bestMatch = -1;
                        // (note) looping more than one cycle can result in buzzing bg noise
                        sUI lenIdeal;
                        if(b_liverec_osc_singlecycle)
                           lenIdeal = sUI(liverec_ref_cyclelen+0.5f);
                        else
                           lenIdeal = sUI((sUI((liverec_ring_size / liverec_ref_cyclelen)+0.5f)-2u) * liverec_ref_cyclelen);
                        // Dyac_host_printf("xxx liverec lenIdeal=%u\n", lenIdeal);
                        sUI liveCycleLenH = sUI(liverec_ref_cyclelen) >> 1u;
                        sUI idealEndI = i + lenIdeal;
                        zcLast = idealEndI;
                        sF32 bestSlopeDiff = 999999.0f;
                        // Forward
                        l = (smpDat[idealEndI % liverec_ring_size] >= 0.0f);
                        for(sUI idealDeltaIdx = 1u; idealDeltaIdx < liveCycleLenH; idealDeltaIdx++)
                        {
                           i = idealEndI + idealDeltaIdx;
                           sBool c = (smpDat[i % liverec_ring_size] >= 0.0f);
                           if(c && (l ^ c))
                           {
                              sF32 slopeCur = 0.0f;
                              for(sUI j = 0u; j < SLOPE_NUM_FRAMES; j++)
                              {
                                 slopeCur += smpDat[(i + j) % liverec_ring_size];
                              }

                              sF32 slopeDiff = sABS(slopeCur - slopeStart);
                              if( (-1 == bestMatch) || (slopeDiff < bestSlopeDiff) )
                              {
                                 sUI len = i - zcFirst;
                                 sSI dist = sABS(sSI(lenIdeal) - sSI(len));
                                 bestMatch = dist;
                                 bestSlopeDiff = slopeDiff;
                                 zcLast = i;
                              }
                           }
                           l = c;
                        }
                        // Backward
                        l = (smpDat[idealEndI % liverec_ring_size] >= 0.0f);
                        for(sUI idealDeltaIdx = 1u; idealDeltaIdx < liveCycleLenH; idealDeltaIdx++)
                        {
                           i = idealEndI - idealDeltaIdx;
                           sBool c = (smpDat[i % liverec_ring_size] >= 0.0f);
                           if(!c && (l ^ c))
                           {
                              i++;
                              sF32 slopeCur = 0.0f;
                              for(sUI j = 0u; j < SLOPE_NUM_FRAMES; j++)
                              {
                                 slopeCur += smpDat[(i + j) % liverec_ring_size];
                              }

                              sF32 slopeDiff = sABS(slopeCur - slopeStart);
                              if( (-1 == bestMatch) || (slopeDiff < bestSlopeDiff) )
                              {
                                 sUI len = i - zcFirst;
                                 sSI dist = sABS(sSI(lenIdeal) - sSI(len));
                                 bestMatch = dist;
                                 bestSlopeDiff = slopeDiff;
                                 zcLast = i;
                              }
                           }
                           l = c;
                        }
                        // Dyac_host_printf("xxx lenIdeal=%u liverec_ref_cyclelen=%u bestDist=%d => len=%d\n", lenIdeal, sUI(liverec_ref_cyclelen), bestMatch, int(zcLast) - int(zcFirst));
                     }
                  }
                  else
                  {
                     for(i = 1u; i < _inputLen; i++)
                     {
                        sBool c = (smpDat[i] <= 0.0f);
                        if(l ^ c)
                        {
                           l = c;
                           break;
                        }
                        l = c;
                     }
                     // Dyac_host_printf("xxx odd zcFirst=%u\n", i);
                     zcFirst = i;

                     if(i != _inputLen)
                     {
                        sBool l = (smpDat[_inputLen - 1u] <= 0.0f);
                        for(i = (_inputLen - 2u); i < _inputLen; i--)
                        {
                           sBool c = (smpDat[i] <= 0.0f);
                           if(l ^ c)
                              break;
                           l = c;
                        }
                        if(i < _inputLen)
                        {
                           zcLast = i;
                        }
                     }

                     // Dyac_host_printf("xxx odd zcLast=%u\n", zcLast);
                  }

                  if(zcLast > zcFirst)
                  {
                     _overrideLoopOffset = _inputOffset + sSI(zcFirst);
                     _overrideLoopLen    = sSI(zcLast - zcFirst);
                     // Dyac_host_printf("xxx new liverec osc area=(%d; %d)\n", liverec_queued_override_loop_offset, liverec_queued_override_loop_len);
                  }
               }
            }
         }
      }
   }
   else/// if(b_liverec_copy)
   {
      _overrideLoopOffset = _inputOffset;
      _overrideLoopLen    = _inputLen;
      // Dyac_host_printf("xxx liveRecAdjustOscLoopSizeInt: loopOffset=%u loopLen=%u\n", _inputOffset, _inputLen);
   }
}

void StSample::liveRecAdjustOscLoopSizeRing(void) {

   liverec_queued_override_loop_offset = -1;
   liverec_queued_override_loop_len    = 0;

   if(STSAMPLE_LIVEREC_OSC_MODE_OFF != liverec_osc_mode && liverec_ring_size > 0u)
   {
      // Dyac_host_printf("xxx liveRecAdjustOscLoopSize: this=%p liverec_frame_offset=%u num=%u #ring=%u liverec_ref_cyclelen=%u\n", this, liverec_frame_offset, liverec_num_frames, liverec_ring_size, sUI(liverec_ref_cyclelen));

      if(liverec_num_frames >= liverec_ring_size)  // at least one cycle complete ?
      {
         if(NULL != waveform && NULL != waveform->sample_data && waveform->num_channels > 0u)
         {
            const YAC_FloatArray *faSmp = waveform->sample_data;
            sUI wfNumCh = waveform->num_channels;
            sUI wfNumFrames = faSmp->num_elements / wfNumCh;

            if(wfNumFrames > 0u && NULL != faSmp->elements)
            {
               // base offset for resulting loop start offset
               //  (note) == next write off == oldest frame offset
               sUI ringOff = liverec_frame_offset;
               sUI scanSz = liverec_ring_size;
#if 1
               if(liverec_ring_size >= sUI(liverec_ref_cyclelen * 3u))
               {
                  scanSz = sUI(liverec_ref_cyclelen * 3u);
                  ringOff -= scanSz;
               }
               else if(liverec_ring_size >= sUI(liverec_ref_cyclelen * 2u))
               {
                  scanSz = sUI(liverec_ref_cyclelen * 2u);
                  ringOff -= scanSz;
               }
               ringOff = ringOff % liverec_ring_size;
#endif
               
               // remember for StSample::liveRecCopyRingToOverride()
               liverec_last_osc_ring_frame_offset = ringOff;

               // Dyac_host_printf("xxx liveRecAdjustOscLoopSizeRing: scan start liverec_frame_offset=%u ringOff=%u liverec_ring_size=%u scanSz=%u\n", liverec_frame_offset, ringOff, liverec_ring_size, scanSz);

               if(1u == wfNumCh)
               {
                  const sF32 *smpDat = faSmp->elements;
                  smpDat += liverec_loop_offset;  // add target sample zone loop start (usually 0)

                  if(STSAMPLE_LIVEREC_DBLBUF_MODE_OFF != liverec_doublebuffer_mode)
                  {
                     if(1u == ((liverec_doublebuffer_rec_idx ^ b_liverec_doublebuffer_swap_queued) & 1u))
                     {
                        smpDat += (wfNumFrames >> 1u);  // was recorded to 2nd half of waveform
                        // Dyac_host_printf("xxx liveRecAdjustOscLoopSize: calc for bufIdx=1\n");
                     }
                     else
                     {
                        // Dyac_host_printf("xxx liveRecAdjustOscLoopSize: calc for bufIdx=0\n");
                     }
                  }

                  if(liverec_osc_pre_filter < 1.0f)
                  {
                     // (todo) use scanSz
                     if(liverec_ring_size <= LIVEREC_OSC_PRE_FILTER_BUFFER_SZ)
                     {
                        if(NULL == liverec_osc_pre_filter_buffer)
                        {
                           // lazy-alloc filter buffer
                           liverec_osc_pre_filter_buffer = new(std::nothrow) sF32[LIVEREC_OSC_PRE_FILTER_BUFFER_SZ];
                        }

                        if(NULL != liverec_osc_pre_filter_buffer)
                        {
                           liverec_osc_pre_filter_biquad.reset();
                           liverec_osc_pre_filter_biquad.calcParams(StSampleVoiceBiquad::LPF,
                                                                    0.0f/*fltGainDb*/,
                                                                    liverec_osc_pre_filter/*fltCutOff*/,
                                                                    0.0f/*fltRes*/,
                                                                    1.0f/*srate*/
                                                                    );
                           {
                              const sF32 smpIn = smpDat[ringOff];
                              (void)liverec_osc_pre_filter_biquad.filter(smpIn);
                           }

                           for(sUI frameIdx = 0u; frameIdx < liverec_ring_size; frameIdx++)
                           {
                              sUI fltRingOff = (ringOff + frameIdx) % liverec_ring_size;
                              const sF32 smpIn = smpDat[fltRingOff];
                              sF32 smpFlt = liverec_osc_pre_filter_biquad.filter(smpIn);
                              liverec_osc_pre_filter_buffer[fltRingOff] = smpFlt;
                           }

                           // Now use filtered buffer for detection
                           smpDat = liverec_osc_pre_filter_buffer;
                           ringOff = (ringOff + liverec_ring_size + 1u) % liverec_ring_size; // filter delay
                           // // ringOff = (ringOff + liverec_ring_size + 2u) % liverec_ring_size; // filter delay
                           // // ringOff = (ringOff + 1u) % liverec_ring_size; // filter delay
                           // Dyac_host_printf("xxx osc ring: use prefilter buffer\n");
                        }
                     }
                  } // if liverec_osc_pre_filter < 1

                  sBool l = (smpDat[ringOff] <= 0.0f);
                  sUI i;
                  sUI zcFirst = 0u;
                  sUI zcLast = 0u;

                  // (todo) optimize / replace modulo

                  if(STSAMPLE_LIVEREC_OSC_MODE_EVEN == liverec_osc_mode)
                  {
                     // // for(i = 1u; i < liverec_frame_offset; i++)
                     for(i = 1u; i < scanSz; i++)
                     {
                        sBool c = (smpDat[(ringOff + i) % liverec_ring_size] <= 0.0f);
                        if(l && !c)
                        {
                           l = c;
                           break;
                        }
                        l = c;
                     }
                     // Dyac_host_printf("xxx even zcFirst=%u\n", i);
                     zcFirst = i;
                     zcLast = i;

                     sUI flip = 0u;
                     for(; i < scanSz; i++)
                     {
                        sBool c = (smpDat[(ringOff + i) % liverec_ring_size] <= 0.0f);
                        if(l ^ c)
                        {
                           flip ^= 1u;
                           if(0u == flip)
                              zcLast = i;
                        }
                        l = c;
                     }
                     // Dyac_host_printf("xxx even zcLast=%u\n", zcLast);
                  }
                  else if(STSAMPLE_LIVEREC_OSC_MODE_EVEN_TUNED == liverec_osc_mode)
                  {
                     // (todo) add special case optimizations for POT ring sizes
                     for(i = 1u; i < scanSz; i++)
                     {
                        sBool c = (smpDat[(ringOff + i) % liverec_ring_size] <= 0.0f);
                        if(l && !c)
                        {
                           l = c;
                           break;
                        }
                        l = c;
                     }
                     // Dyac_host_printf("xxx even zcFirst=%u\n", i);
                     zcFirst = i;
                     zcLast = i;

                     if((i != scanSz) &&
                        (liverec_ref_cyclelen > 0.0f) &&
                        (scanSz >= sUI(liverec_ref_cyclelen * 2))
                        )
                     {
                        sSI bestMatch = -1;
                        sUI lenIdeal;
                        if(b_liverec_osc_singlecycle)
                           lenIdeal = sUI(liverec_ref_cyclelen+0.5f);
                        else
                           lenIdeal = sUI((sUI((scanSz / liverec_ref_cyclelen)+0.5f)-1u) * liverec_ref_cyclelen);
                        // Dyac_host_printf("xxx liverec ring even_tuned lenIdeal=%u\n", lenIdeal);

                        sUI flip = 0u;
                        for(; i < scanSz; i++)
                        {
                           sBool c = (smpDat[(ringOff + i) % liverec_ring_size] <= 0.0f);
                           if(l ^ c)
                           {
                              flip ^= 1u;
                              if(0u == flip)
                              {
                                 sUI len = i - zcFirst;
                                 sSI dist = sABS(sSI(lenIdeal) - sSI(len));
                                 if((-1 == bestMatch) || (dist < bestMatch) )
                                 {
                                    bestMatch = dist;
                                    zcLast = i;
                                    if(len > lenIdeal)
                                       break;
                                 }
                              }
                           }
                           l = c;
                        }
                     }
                     // Dyac_host_printf("xxx ring even zcLast=%u\n", zcLast);
                  }
                  else if(STSAMPLE_LIVEREC_OSC_MODE_TUNED == liverec_osc_mode)
                  {
                     for(i = 1u; i < scanSz; i++)
                     {
                        sBool c = (smpDat[(ringOff + i) % liverec_ring_size] <= 0.0f);
                        if(l ^ c)
                        {
                           l = c;
                           break;
                        }
                        l = c;
                     }
                     // Dyac_host_printf("xxx odd zcFirst=%u\n", i);
                     zcFirst = i;
                     zcLast = i;

                     // Dyac_host_printf("xxx liverec_ref_cyclelen=%f liverec_ring_size=%u expect at least %u\n", liverec_ref_cyclelen, liverec_num_frames, sUI(liverec_ref_cyclelen * 2u));

                     if((i != scanSz) &&
                        (liverec_ref_cyclelen > 0.0f) &&
                        (scanSz >= sUI(liverec_ref_cyclelen * 2u))
                        )
                     {
                        sSI bestMatch = -1;
                        sUI lenIdeal;
                        if(b_liverec_osc_singlecycle)
                           lenIdeal = sUI(liverec_ref_cyclelen+0.5f);
                        else
                           lenIdeal = sUI((sUI((scanSz / liverec_ref_cyclelen)+0.5f)-1u) * liverec_ref_cyclelen);
                        // Dyac_host_printf("xxx liverec lenIdeal=%u\n", lenIdeal);
                        sBool l = (smpDat[ (ringOff + scanSz -1u) % liverec_ring_size] <= 0.0f);
                        for(i = (scanSz - 2u); i < scanSz; i--)
                        {
                           sBool c = (smpDat[(ringOff + i) % liverec_ring_size] <= 0.0f);
                           if(l ^ c)
                           {
                              sUI len = i - zcFirst;
                              sSI dist = sABS(sSI(lenIdeal) - sSI(len));
                              if((-1 == bestMatch) || (dist < bestMatch) )
                              {
                                 bestMatch = dist;
                                 zcLast = i;
                              }
                           }
                           l = c;
                        }
                     }
                  }
                  else if(STSAMPLE_LIVEREC_OSC_MODE_EDGE_P_TUNED == liverec_osc_mode)
                  {
                     // rising edge
                     l = (smpDat[ringOff] >= 0.0f);
                     for(i = 1u; i < scanSz; i++)
                     {
                        sBool c = (smpDat[(ringOff + i) % liverec_ring_size] >= 0.0f);
                        if(c && (l ^ c))
                        {
                           break;
                        }
                        l = c;
                     }
                     // Dyac_host_printf("xxx odd zcFirst=%u\n", i);
                     zcFirst = i;
                     zcLast = i;

                     // Dyac_host_printf("xxx liverec_ref_cyclelen=%f liverec_ring_size=%u expect at least %u\n", liverec_ref_cyclelen, liverec_num_frames, sUI(liverec_ref_cyclelen * 2u));

                     if((i != scanSz) &&
                        (liverec_ref_cyclelen > 0.0f) &&
                        (scanSz >= sUI(liverec_ref_cyclelen * 2u))
                        )
                     {
                        sSI bestMatch = -1;
                        sUI lenIdeal;
                        if(b_liverec_osc_singlecycle)
                           lenIdeal = sUI(liverec_ref_cyclelen+0.5f);
                        else
                           lenIdeal = sUI((sUI((scanSz / liverec_ref_cyclelen)+0.5f)-1u) * liverec_ref_cyclelen);
                        // Dyac_host_printf("xxx liverec lenIdeal=%u\n", lenIdeal);
                        sBool l = (smpDat[ (ringOff + scanSz -1u) % liverec_ring_size] >= 0.0f);
                        for(i = (scanSz - 2u); i < scanSz; i--)
                        {
                           sBool c = (smpDat[(ringOff + i) % liverec_ring_size] >= 0.0f);
                           if(!c && (l ^ c))
                           {
                              sUI len = i - zcFirst + 1u;
                              sSI dist = sABS(sSI(lenIdeal) - sSI(len));
                              if((-1 == bestMatch) || (dist < bestMatch) )
                              {
                                 bestMatch = dist;
                                 zcLast = i;
                              }
                           }
                           l = c;
                        }
                     }
                  }
                  else if(STSAMPLE_LIVEREC_OSC_MODE_EDGE_P_SLOPE_TUNED == liverec_osc_mode)
                  {
                     // rising edge
#define SLOPE_NUM_FRAMES  (12u)
                     l = (smpDat[ringOff] >= 0.0f);
                     sF32 slopeStart = 0.0f;
                     for(i = 1u; i < scanSz; i++)
                     {
                        sBool c = (smpDat[(ringOff + i) % liverec_ring_size] >= 0.0f);
                        if(c && (l ^ c))
                        {
                           for(sUI j = 0u; j < SLOPE_NUM_FRAMES; j++)
                           {
                              slopeStart += smpDat[(ringOff + i + j) % liverec_ring_size];
                           }
                           l = c;  // => true
                           break;
                        }
                        l = c;
                     }
                     // Dyac_host_printf("xxx odd zcFirst=%u\n", i);
                     zcFirst = i;
                     zcLast = i;

                     // Dyac_host_printf("xxx liverec_ref_cyclelen=%f liverec_ring_size=%u expect at least %u\n", liverec_ref_cyclelen, liverec_num_frames, sUI(liverec_ref_cyclelen * 2u));

                     if((i != scanSz) &&
                        (liverec_ref_cyclelen > 0.0f) &&
                        (scanSz >= sUI(liverec_ref_cyclelen * 3u))
                        )
                     {
                        sSI bestMatch = -1;
                        // (note) looping more than one cycle can result in buzzing bg noise
                        sUI lenIdeal;
                        if(b_liverec_osc_singlecycle)
                           lenIdeal = sUI(liverec_ref_cyclelen+0.5f);
                        else
                           lenIdeal = sUI((sUI((scanSz / liverec_ref_cyclelen)+0.5f)-2u) * liverec_ref_cyclelen);
                        // Dyac_host_printf("xxx liverec lenIdeal=%u\n", lenIdeal);
                        sUI liveCycleLenH = sUI(liverec_ref_cyclelen) >> 1u;
                        sUI idealEndI = i + lenIdeal;
                        zcLast = idealEndI;
                        sF32 bestSlopeDiff = 999999.0f;
                        // Forward
                        l = (smpDat[(ringOff + idealEndI) % liverec_ring_size] >= 0.0f);
                        for(sUI idealDeltaIdx = 1u; idealDeltaIdx < liveCycleLenH; idealDeltaIdx++)
                        {
                           i = idealEndI + idealDeltaIdx;
                           sBool c = (smpDat[(ringOff + i) % liverec_ring_size] >= 0.0f);
                           if(c && (l ^ c))
                           {
                              sF32 slopeCur = 0.0f;
                              for(sUI j = 0u; j < SLOPE_NUM_FRAMES; j++)
                              {
                                 slopeCur += smpDat[(ringOff + i + j) % liverec_ring_size];
                              }

                              sF32 slopeDiff = sABS(slopeCur - slopeStart);
                              if( (-1 == bestMatch) || (slopeDiff < bestSlopeDiff) )
                              {
                                 sUI len = i - zcFirst;
                                 sSI dist = sABS(sSI(lenIdeal) - sSI(len));
                                 bestMatch = dist;
                                 bestSlopeDiff = slopeDiff;
                                 zcLast = i;
                              }
                           }
                           l = c;
                        }
                        // Backward
                        l = (smpDat[(ringOff + idealEndI) % liverec_ring_size] >= 0.0f);
                        for(sUI idealDeltaIdx = 1u; idealDeltaIdx < liveCycleLenH; idealDeltaIdx++)
                        {
                           i = idealEndI - idealDeltaIdx;
                           sBool c = (smpDat[(ringOff + i) % liverec_ring_size] >= 0.0f);
                           if(!c && (l ^ c))
                           {
                              i++;
                              sF32 slopeCur = 0.0f;
                              for(sUI j = 0u; j < SLOPE_NUM_FRAMES; j++)
                              {
                                 slopeCur += smpDat[(ringOff + i + j) % liverec_ring_size];
                              }

                              sF32 slopeDiff = sABS(slopeCur - slopeStart);
                              if( (-1 == bestMatch) || (slopeDiff < bestSlopeDiff) )
                              {
                                 sUI len = i - zcFirst;
                                 sSI dist = sABS(sSI(lenIdeal) - sSI(len));
                                 bestMatch = dist;
                                 bestSlopeDiff = slopeDiff;
                                 zcLast = i;
                              }
                           }
                           l = c;
                        }
                        // Dyac_host_printf("xxx lenIdeal=%u liverec_ref_cyclelen=%u bestDist=%d => len=%d\n", lenIdeal, sUI(liverec_ref_cyclelen), bestMatch, int(zcLast) - int(zcFirst));
                     }
                  }
                  else if(STSAMPLE_LIVEREC_OSC_MODE_PEAK_TUNED == liverec_osc_mode)
                  {
                     sUI refCycleLen = sUI(liverec_ref_cyclelen);
                     sUI refCycleLen2 = sUI(2.0f * liverec_ref_cyclelen);
                     if(scanSz >= refCycleLen2)
                     {
                        // Find peak within first cycle
                        sF32 fMax1 = 0.0f;
                        sUI idxMax1 = 0u;
                        for(i = 0u; i < refCycleLen; i++)
                        {
                           sF32 f = smpDat[(ringOff + i) % liverec_ring_size];
                           if(f > fMax1)
                           {
                              fMax1 = f;
                              idxMax1 = i;
                           }
                        }

                        if(fMax1 > 0.0f)
                        {
                           // Find second peak (+-6.25% around refCycleLen)
                           i = idxMax1 + ((refCycleLen * 30u) >> 5);  // +refLen*0.9375
                           sUI iEnd = idxMax1 + ((refCycleLen * 34u) >> 5);  // +refLen*1.0625

                           sF32 fMax2 = 0.0f;
                           sUI idxMax2 = 0u;
                           for(; i < iEnd; i++)
                           {
                              sF32 f = smpDat[(ringOff + i) % liverec_ring_size];
                              if(f > fMax2)
                              {
                                 fMax2 = f;
                                 idxMax2 = i + 1;
                              }
                           }
                              
                           if(fMax2 > 0.0f)
                           {
                              zcFirst = idxMax1;
                              zcLast  = idxMax2;

                              // Dyac_host_printf("xxx ring PEAK_TUNED scanSz=%u ringOff=%u zcFirst=%u(max1=%f) zcLast=%u(max2=%f) len=%u (ref=%u)\n", scanSz, ringOff, zcFirst, fMax1, zcLast, fMax2, (zcLast - zcFirst), refCycleLen);
                           }
                           else
                           {
                              Dyac_host_printf("xxx PEAK_TUNED: failed to find fMax2\n");
                           }

                        } // if fMax1
                        else
                        {
                           Dyac_host_printf("xxx PEAK_TUNED: failed to find fMax1\n");
                        }

                     } // if scanSz >= refCycleLen*2
                     else
                     {
                        Dyac_host_printf("xxx PEAK_TUNED: scanSz=%u too small\n", scanSz);
                     }
                  }
                  else if(STSAMPLE_LIVEREC_OSC_MODE_ZC_PEAK_TUNED == liverec_osc_mode)
                  {
                     sUI refCycleLen = sUI(liverec_ref_cyclelen);
                     sUI refCycleLen2 = sUI(2.0f * liverec_ref_cyclelen);
                     sUI refCycleLen3 = sUI(3.0f * liverec_ref_cyclelen);
                     if(scanSz >= refCycleLen3)
                     {
                        // Find peak within first cycle
                        sF32 fMax1 = 0.0f;
                        sUI idxMax1 = 0u;
                        for(i = 0u; i < refCycleLen; i++)
                        {
                           sF32 f = smpDat[(ringOff + i) % liverec_ring_size];
                           if(f > fMax1)
                           {
                              fMax1 = f;
                              idxMax1 = i;
                           }
                        }

                        if(fMax1 > 0.0f)
                        {
                           // Find zero-crossing after peak
                           sBool l = YAC_TRUE;
                           i = idxMax1 + 1u;
                           for(; i < refCycleLen2; i++)
                           {
                              sBool c = (smpDat[(ringOff + i) % liverec_ring_size] > 0.0f);
                              if(!c && (l ^ c))
                              {
                                 break;
                              }
                              l = c;
                           }
                           // Dyac_host_printf("xxx ring ZC_PEAK_TUNED idx1=%u i=%u zc off=%u\n", idxMax1, i, (i - idxMax1));
                           zcFirst = i;
                           zcLast  = i;

                           // Find second peak (+-6.25% around refCycleLen)
                           i = idxMax1 + ((refCycleLen * 30u) >> 5);  // +refLen*0.9375
                           sUI iEnd = idxMax1 + ((refCycleLen * 34u) >> 5);  // +refLen*1.0625

                           sF32 fMax2 = 0.0f;
                           sUI idxMax2 = 0u;
                           for(; i < iEnd; i++)
                           {
                              sF32 f = smpDat[(ringOff + i) % liverec_ring_size];
                              if(f > fMax2)
                              {
                                 fMax2 = f;
                                 idxMax2 = i;
                              }
                           }
                              
                           if(fMax2 > 0.0f)
                           {
                              // Find zero crossing after 2nd peak
                              l = YAC_TRUE;
                              i = idxMax2 + 1u;
                              for(; i < refCycleLen3; i++)
                              {
                                 sBool c = (smpDat[(ringOff + i) % liverec_ring_size] > 0.0f);
                                 if(!c && (l ^ c))
                                 {
                                    break;
                                 }
                                 l = c;
                              }
                              // Dyac_host_printf("xxx ring ZC_PEAK_TUNED idx2=%u zc i=%u refCyclenLen2=%u off=%u  idx1<>2dist=%u\n", idxMax2, i, refCycleLen2, (i - idxMax2), idxMax2-idxMax1);
                              zcLast = i;

                              // Dyac_host_printf("xxx ring ZC_PEAK_TUNED scanSz=%u ringOff=%u zcFirst=%u(max1=%f) zcLast=%u(max2=%f) len=%u (ref=%u)\n", scanSz, ringOff, zcFirst, fMax1, zcLast, fMax2, (zcLast - zcFirst), refCycleLen);
                           }
                           else
                           {
                              Dyac_host_printf("xxx ZC_PEAK_TUNED: failed to find fMax2\n");
                           }

                        } // if fMax1
                        else
                        {
                           Dyac_host_printf("xxx ZC_PEAK_TUNED: failed to find fMax1\n");
                        }

                     } // if scanSz >= refCycleLen*2
                     else
                     {
                        Dyac_host_printf("xxx ZC_PEAK_TUNED: scanSz=%u too small\n", scanSz);
                     }
                  }
                  else
                  {
                     for(i = 1u; i < scanSz; i++)
                     {
                        sBool c = (smpDat[(ringOff + i) % liverec_ring_size] <= 0.0f);
                        if(l ^ c)
                        {
                           l = c;
                           break;
                        }
                        l = c;
                     }
                     // Dyac_host_printf("xxx odd zcFirst=%u\n", i);
                     zcFirst = i;

                     if(i != scanSz)
                     {
                        sBool l = (smpDat[scanSz - 1u] <= 0.0f);
                        for(i = (liverec_frame_offset - 2u); i < liverec_frame_offset; i--)
                        {
                           sBool c = (smpDat[(ringOff + i) % liverec_ring_size] <= 0.0f);
                           if(l ^ c)
                              break;
                           l = c;
                        }
                        if(i < liverec_frame_offset)
                        {
                           zcLast = i;
                        }
                     }

                     // Dyac_host_printf("xxx odd zcLast=%u\n", zcLast);
                  }

                  if(zcLast > zcFirst)
                  {
                     liverec_queued_override_loop_offset = liverec_loop_offset + sSI(zcFirst);
                     liverec_queued_override_loop_len    = sSI(zcLast - zcFirst);
                     // Dyac_host_printf("xxx new liverec ring osc area=(%d; %d)\n", liverec_queued_override_loop_offset, liverec_queued_override_loop_len);
                  }
                  else
                  {
                     // Sample too short ?
                     Dyac_host_printf("[~~~] liverec ring loopdetect failed: target loop too short ?! (have=%u ideal=%u)\n", liverec_ring_size, sUI(liverec_ref_cyclelen * 3u));
                     liverec_queued_override_loop_offset = zcFirst;
                     liverec_queued_override_loop_len    = liverec_ring_size - zcFirst;
                     if(liverec_queued_override_loop_len <= 0)
                     {
                        liverec_queued_override_loop_offset = -1;
                        liverec_queued_override_loop_len    = 0;
                     }
                  }
               }
            }
         }
      }
   }
   else // if osc mode && ring_size>0
   {
      // Should not be reachable
      liverec_queued_override_loop_offset = liverec_loop_offset;
      liverec_queued_override_loop_len    = liverec_loop_len;
   }
}

void StSample::liveRecCopyRingToOverride(sF32 *_smpDatDest,
                                         sUI   _waveOff,
                                         sUI   _copyLoopOffset,
                                         sUI   _curSampleLen
                                         ) {
   // // if((liverec_num_frames > 0u) && (liverec_ring_size > 0u) && (liverec_num_frames >= _curSampleLen))
   if((liverec_num_frames > 0u) && (liverec_ring_size > 0u) && (liverec_num_frames >= _curSampleLen))
   {
      sUI numCh = waveform->num_channels;

      sUI ringOff = (liverec_last_osc_ring_frame_offset + _copyLoopOffset) % liverec_ring_size;

      if(1u == numCh)
      {
         // Dyac_host_printf("xxx copy %u mono frames from ring to override buf, liverec_last_osc_ring_frame_offset=%u, waveOff=%u recIdx=%u copyLoopOffset=%u curSampleLen=%u\n", _curSampleLen, liverec_last_osc_ring_frame_offset, _waveOff, liverec_doublebuffer_rec_idx, _copyLoopOffset, _curSampleLen);
         sUI k = 0u;
         // (note) waveform+sample_data cannot be NULL at this point (or at least shouldn't.)
         const sF32 *smpSrc = waveform->sample_data->elements + _waveOff + liverec_loop_offset;
         for(sUI i = 0u; i < _curSampleLen; i++)
         {
            _smpDatDest[k++] = smpSrc[ringOff];
            ringOff = (ringOff + 1u) % liverec_ring_size;
         }
      }

   }
   else
   {
      Dyac_host_printf("[!!!] liverec_num_frames < curSampleLen (%u < %u)\n", liverec_num_frames, _curSampleLen);
   }
}

void StSample::liveRecFadeRingToOverride(sF32 *_smpDatDest,
                                         sUI   _waveOff,
                                         sUI   _copyLoopOffset,  // srcOffset
                                         sUI   _curSampleLen,    // srcLen
                                         sF32  _newAmt
                                         ) {
   if(_curSampleLen > 0u)
   {
      // // if((liverec_num_frames > 0u) && (liverec_ring_size > 0u) && (liverec_num_frames >= _curSampleLen))
      if((liverec_num_frames > 0u) && (liverec_ring_size > 0u) && (liverec_num_frames >= _curSampleLen))
      {
         sUI numCh = waveform->num_channels;

         sUI ringOff = (liverec_last_osc_ring_frame_offset + _copyLoopOffset) % liverec_ring_size;

         if(1u == numCh)
         {
            // // if(_waveOff > 0u)
            // //    Dyac_host_printf("xxx waveOff=%u\n", _waveOff);

            // Dyac_host_printf("xxx copy %u mono frames from ring to override buf, liverec_last_osc_ring_frame_offset=%u, waveOff=%u recIdx=%u copyLoopOffset=%u curSampleLen=%u\n", _curSampleLen, liverec_last_osc_ring_frame_offset, _waveOff, liverec_doublebuffer_rec_idx, _copyLoopOffset, _curSampleLen);
            sUI k = 0u;
            // (note) waveform+sample_data cannot be NULL at this point (or at least shouldn't.)
            const sF32 *smpSrc = waveform->sample_data->elements + _waveOff + liverec_loop_offset;
            sF32 newAmt = _newAmt * _newAmt;
            for(sUI i = 0u; i < _curSampleLen; i++)
            {
               _smpDatDest[k] = _smpDatDest[k] + (smpSrc[ringOff] - _smpDatDest[k]) * newAmt;
               k++;
               ringOff = (ringOff + 1u) % liverec_ring_size;
            }
         }

      }
      else
      {
         Dyac_host_printf("[!!!] liverec_num_frames < curSampleLen (%u < %u)\n", liverec_num_frames, _curSampleLen);
      }
   }
}

void StSample::liveRecResampleAndFadeRingToOverride(sF32 *_smpDatDest,
                                                    sUI   _waveOff,
                                                    sUI   _srcOffset,
                                                    sUI   _srcLen,  // == liverec_override_loop_len
                                                    sUI   _dstOffset,  // == 0u
                                                    sUI   _dstLen,  // == copy_loop_len
                                                    sF32  _newAmt
                                                    ) {
   if( (_srcLen > 0u) && (_dstLen > 0u) )
   {
      if((liverec_num_frames > 0u) && (liverec_ring_size > 0u) && (liverec_num_frames >= _srcLen))
      {
         sUI numCh = waveform->num_channels;

         if(1u == numCh)
         {
            // if(_waveOff > 0u)
            //    Dyac_host_printf("xxx waveOff=%u\n", _waveOff);

            // Dyac_host_printf("xxx resample+fade from ring to override buf: this=%p liverec_last_osc_ring_frame_offset=%u waveOff=%u recIdx=%u srcOff=%u srcLen=%u dstOff=%u dstLen=%u\n", this, liverec_last_osc_ring_frame_offset, _waveOff, liverec_doublebuffer_rec_idx, _srcOffset, _srcLen, _dstOffset, _dstLen);

            sUI k = _dstOffset;  // should be 0
            // (note) waveform+sample_data cannot be NULL at this point (or at least shouldn't.)
            const sF32 *smpSrc = waveform->sample_data->elements + _waveOff/*should be 0*/;
            sF32 srcOff = sF32(_srcOffset);
            sF32 srcRate = sF32(_srcLen) / _dstLen;
            sF32 newAmt = _newAmt * _newAmt;
            for(sUI i = 0u; i < _dstLen; i++)
            {
               sUI srcOffC = sUI(srcOff);
               sUI srcOffN = sUI(srcOff) + 1u;
               sUI ringOffC = (liverec_last_osc_ring_frame_offset + srcOffC) % liverec_ring_size;
               sUI ringOffN = (liverec_last_osc_ring_frame_offset + srcOffN) % liverec_ring_size;
               sF32 srcOffFrac = srcOff - sUI(srcOff);
               sF32 srcVal = smpSrc[ringOffC];
               srcVal += (smpSrc[ringOffN] - srcVal) * srcOffFrac;
               _smpDatDest[k] += (srcVal - _smpDatDest[k]) * newAmt;
               k++;
               srcOff += srcRate;
            }
         }

      }
      else
      {
         Dyac_host_printf("[!!!] liverec_num_frames < srcLen (%u < %u)\n", liverec_num_frames, _srcLen);
      }
   }
   else
   {
      Dyac_host_printf("[!!!] srcLen=%u dstLen=%u\n", _srcLen, _dstLen);
   }
}

void StSample::_setEnableSumInput(sBool _bEnable) {
   b_sum_input = _bEnable;
}

sBool StSample::_getEnableSumInput(void) {
   return b_sum_input;
}

void StSample::_setSumInputXFade(sF32 _xfade) {
   sum_input_xfade = _xfade;
}

sF32 StSample::_getSumInputXFade(void) {
   return sum_input_xfade;
}

void StSample::_setVoicePluginShared(sUI _pluginIdx, YAC_Object *_pluginShared) {
   if(_pluginIdx < STSAMPLE_NUM_PLUGINS)
   {
      YAC_DEREF_ARG(StPluginShared, pluginShared, _pluginShared);
      if((NULL != pluginShared) && pluginShared_deleteme)
      {
         if(!(YAC_CHK(pluginShared, clid_StPluginShared)))
         {
            // Not an StPluginShared object
            YAC_DELETE_SAFE(pluginShared);
            pluginShared = NULL;
         }
         if((NULL != plugins[_pluginIdx].shared) && (plugins[_pluginIdx].shared != pluginShared))
         {
            // Plugin type changed, delete old shared instance
            YAC_DELETE(plugins[_pluginIdx].shared);
         }
         plugins[_pluginIdx].shared = pluginShared;
      }
      else
      {
         YAC_DELETE_SAFE(plugins[_pluginIdx].shared);
         plugin_mask_def &= ~(1u << _pluginIdx);
         plugin_mask_sr  &= ~(1u << _pluginIdx);
         plugin_mask_am  &= ~(1u << _pluginIdx);
         plugin_mask_vb  &= ~(1u << _pluginIdx);
         plugin_mask_vm  &= ~(1u << _pluginIdx);
      }
      updatePluginMask(_pluginIdx);
      updatePluginAlgInt();
   }
}

YAC_Object *StSample::_getVoicePluginShared(sUI _pluginIdx) {
   if(_pluginIdx < STSAMPLE_NUM_PLUGINS)
   {
      return plugins[_pluginIdx].shared;
   }
   return NULL;
}

void StSample::_setVoicePluginSharedMissing(sUI _pluginIdx, YAC_Object *_pluginSharedMissing) {
   if(_pluginIdx < STSAMPLE_NUM_PLUGINS)
   {
      YAC_DEREF_ARG(StPluginSharedMissing, pluginSharedMissing, _pluginSharedMissing);
      if((NULL != pluginSharedMissing) && pluginSharedMissing_deleteme)
      {
         if(!(YAC_CHK(pluginSharedMissing, clid_StPluginSharedMissing)))
         {
            // Not an StPluginSharedMissing object
            YAC_DELETE_SAFE(pluginSharedMissing);
            pluginSharedMissing = NULL;
         }
         if((NULL != plugins[_pluginIdx].shared_missing) && (plugins[_pluginIdx].shared_missing != pluginSharedMissing))
         {
            // Plugin type changed, delete old shared missing instance
            YAC_DELETE(plugins[_pluginIdx].shared_missing);
         }
         plugins[_pluginIdx].shared_missing = pluginSharedMissing;
      }
      else
      {
         YAC_DELETE_SAFE(plugins[_pluginIdx].shared_missing);
         plugin_mask_def &= ~(1u << _pluginIdx);
         plugin_mask_sr  &= ~(1u << _pluginIdx);
         plugin_mask_am  &= ~(1u << _pluginIdx);
         plugin_mask_vb  &= ~(1u << _pluginIdx);
         plugin_mask_vm  &= ~(1u << _pluginIdx);
      }
      updatePluginMask(_pluginIdx);
      updatePluginAlgInt();
   }
}

YAC_Object *StSample::_getVoicePluginSharedMissing(sUI _pluginIdx) {
   if(_pluginIdx < STSAMPLE_NUM_PLUGINS)
   {
      return plugins[_pluginIdx].shared_missing;
   }
   return NULL;
}

void StSample::_setEnableVoicePluginPreFilter(sUI _pluginIdx, sBool _bPreFilter) {
   if(_pluginIdx < STSAMPLE_NUM_PLUGINS)
   {
      plugins[_pluginIdx].b_pre_filter = _bPreFilter;
   }
}

sBool StSample::_getEnableVoicePluginPreFilter(sUI _pluginIdx) {
   if(_pluginIdx < STSAMPLE_NUM_PLUGINS)
   {
      return plugins[_pluginIdx].b_pre_filter;
   }
   return YAC_FALSE;
}

void StSample::updatePluginMask(sUI _pluginIdx) {
   plugin_mask_def = (plugin_mask_def & ~(1u << _pluginIdx));
   plugin_mask_sr  = (plugin_mask_sr  & ~(1u << _pluginIdx));
   plugin_mask_am  = (plugin_mask_am  & ~(1u << _pluginIdx));
   plugin_mask_vb  = (plugin_mask_vb  & ~(1u << _pluginIdx));
   plugin_mask_vm  = (plugin_mask_vm  & ~(1u << _pluginIdx));
   if(!plugins[_pluginIdx].b_skip)
   {
      if(NULL != plugins[_pluginIdx].shared)
      {
         if(NULL != plugins[_pluginIdx].shared->info->info->process_replace)
         {
            if(STSAMPLE_VOICEPLUGIN_DST_DEF == plugins[_pluginIdx].dst)
               plugin_mask_def |= (1u << _pluginIdx);
            else if(STSAMPLE_VOICEPLUGIN_DST_SR == plugins[_pluginIdx].dst)
               plugin_mask_sr  |= (1u << _pluginIdx);
            else if(STSAMPLE_VOICEPLUGIN_DST_AM == plugins[_pluginIdx].dst)
               plugin_mask_am  |= (1u << _pluginIdx);
            else if(plugins[_pluginIdx].dst >= STSAMPLE_VOICEPLUGIN_DST_VB1 &&
                    plugins[_pluginIdx].dst <= STSAMPLE_VOICEPLUGIN_DST_VB8
                    )
               plugin_mask_vb  |= (1u << _pluginIdx);
            else if(plugins[_pluginIdx].dst >= STSAMPLE_VOICEPLUGIN_DST_VM1 &&
                    plugins[_pluginIdx].dst <= STSAMPLE_VOICEPLUGIN_DST_VM8
                    )
               plugin_mask_vm |= (1u << _pluginIdx);
         }
      }
   }
}

void StSample::updatePluginAlgInt(void) {
   if(plugin_alg > 0u)
      plugin_alg_int = ((plugin_alg-1u) << 10) + (plugin_alg_outer << 8) + (plugin_alg_filter_pos << 5) + (plugin_mask_def << 1) + 0u/*_bMonoIn*/;
   else
      plugin_alg_int = 0u;
   // Dyac_host_printf("[dbg] StSamplePluginAlgInt: algInt=%u\n", plugin_alg_int);
}

void StSample::_setEnableVoicePlugin(sUI _pluginIdx, sBool _bEnable) {
   if(_pluginIdx < STSAMPLE_NUM_PLUGINS)
   {
      plugins[_pluginIdx].b_skip = !_bEnable;
      updatePluginMask(_pluginIdx);
      updatePluginAlgInt();
   }
}

sBool StSample::_getEnableVoicePlugin(sUI _pluginIdx) {
   if(_pluginIdx < STSAMPLE_NUM_PLUGINS)
   {
      return !plugins[_pluginIdx].b_skip;
   }
   return YAC_FALSE;
}

void StSample::updateVoicePluginLevels(sUI _pluginIdx) {
   float lvl = plugins[_pluginIdx].drive * (plugins[_pluginIdx].b_invert_phase ? -1.0f : 1.0f);
   float pan = plugins[_pluginIdx].pan;
   plugins[_pluginIdx].level[0] = lvl * ((pan < 0.0f) ? 1.0f : (1.0f - pan));
   plugins[_pluginIdx].level[1] = lvl * ((pan > 0.0f) ? 1.0f : (1.0f + pan));
   if(Dyac_host_yacGetDebugLevel() > 1)
      Dyac_host_printf("xxx StSample::updateVoicePluginLevels(%u): lvl=%f pan=%f => lvl_lr=(%f; %f)\n", _pluginIdx, lvl, pan, plugins[_pluginIdx].level[0], plugins[_pluginIdx].level[1]);
}

void StSample::_setVoicePluginLevel(sUI _pluginIdx, sF32 _level) {
   if(_pluginIdx < STSAMPLE_NUM_PLUGINS)
   {
      plugins[_pluginIdx].drive = _level;
      updateVoicePluginLevels(_pluginIdx);
   }
}

sF32 StSample::_getVoicePluginLevel(sUI _pluginIdx) {
   if(_pluginIdx < STSAMPLE_NUM_PLUGINS)
   {
      return plugins[_pluginIdx].drive;
   }
   return 0.0f;
}

void StSample::_setVoicePluginDst(sUI _pluginIdx, sUI _dst) {
   if(_pluginIdx < STSAMPLE_NUM_PLUGINS)
   {
      plugins[_pluginIdx].dst = _dst;
      updatePluginMask(_pluginIdx);
   }
}

sUI StSample::_getVoicePluginDst(sUI _pluginIdx) {
   if(_pluginIdx < STSAMPLE_NUM_PLUGINS)
   {
      return plugins[_pluginIdx].dst;
   }
   return STSAMPLE_VOICEPLUGIN_DST_DEF/*0u*/;
}

void StSample::_setVoicePluginPan (sUI _pluginIdx, sF32 _pan) {
   if(_pluginIdx < STSAMPLE_NUM_PLUGINS)
   {
      if(_pan < -1.0f)
         _pan = -1.0f;
      else if(_pan > 1.0f)
         _pan = 1.0f;
      plugins[_pluginIdx].pan = _pan;
      updateVoicePluginLevels(_pluginIdx);
   }
}

sF32 StSample::_getVoicePluginPan (sUI _pluginIdx) {
   if(_pluginIdx < STSAMPLE_NUM_PLUGINS)
   {
      return plugins[_pluginIdx].pan;
   }
   return 0.0f;
}

void StSample::_setEnableVoicePluginInvertPhase(sUI _pluginIdx, sBool _bInvertPhase) {
   if(_pluginIdx < STSAMPLE_NUM_PLUGINS)
   {
      plugins[_pluginIdx].b_invert_phase = _bInvertPhase;
      updateVoicePluginLevels(_pluginIdx);
   }
}

sBool StSample::_getEnableVoicePluginInvertPhase(sUI _pluginIdx) {
   if(_pluginIdx < STSAMPLE_NUM_PLUGINS)
   {
      return plugins[_pluginIdx].b_invert_phase;
   }
   return YAC_FALSE;
}

void StSample::_setVoicePluginAlg(sUI _alg) {
   plugin_alg = _alg;
   updatePluginAlgInt();
}

sUI StSample::_getVoicePluginAlg(void) {
   return plugin_alg;
}

void StSample::_setVoicePluginAlgFilterPos(sUI _pos) {
   plugin_alg_filter_pos = _pos;
   updatePluginAlgInt();
}

sUI StSample::_getVoicePluginAlgFilterPos(void) {
   return plugin_alg_filter_pos;
}

void StSample::_setVoicePluginAlgOuter(sUI _num) {
   plugin_alg_outer = _num;
   updatePluginAlgInt();
}

sUI StSample::_getVoicePluginAlgOuter(void) {
   return plugin_alg_outer;
}

sF32 StSample::calcMMNoteRel(sF32 _note) const {
   // MM_SRC_KEYBOARD_P/M/PM
   sF32 mmNote = (_note - mm_keyboard_center);
   if(mmNote > mm_keyboard_max)
      mmNote = mm_keyboard_max;
   else if(mmNote < -mm_keyboard_min)
      mmNote = -mm_keyboard_min;
   return mmNote;
}

sF32 StSample::calcMMNoteAbs(sF32 _note) const {
   // MM_SRC_KEYBOARD_ABS
   sF32 mmNote = _note;
   sF32 mmMax = (mm_keyboard_center + mm_keyboard_max);
   if(mmNote > mmMax)
      mmNote = mmMax;
   else
   {
      sF32 mmMin = (mm_keyboard_center - mm_keyboard_min);
      if(mmNote < mmMin)
         mmNote = mmMin;
   }
   return mmNote;
}

void StSample::reorderPluginModMatrixEntries(const sUI *_newOrder) {
   StSampleMMEntry *mm = modmatrix;
   sUI revOrder[STSAMPLE_NUM_PLUGINS/*4*/];
   for(sUI i = 0u; i < STSAMPLE_NUM_PLUGINS; i++)
   {
      revOrder[_newOrder[i]] = i;
   }
   for(sUI i = 0u; i < STSAMPLE_NUM_MODMATRIX_ENTRIES; i++, mm++)
   {
      if( (mm->dst >= STSAMPLE_MM_DST_PLUGIN_1_MOD_1) && (mm->dst <= STSAMPLE_MM_DST_PLUGIN_4_MOD_8) )
      {
         // (note) 8 mods per plugin slot
         sUI pluginIdx = (mm->dst - STSAMPLE_MM_DST_PLUGIN_1_MOD_1) >> 3;
         sUI modIdx    = (mm->dst - STSAMPLE_MM_DST_PLUGIN_1_MOD_1) & 7u;
         pluginIdx = revOrder[pluginIdx];
         mm->dst = STSAMPLE_MM_DST_PLUGIN_1_MOD_1 + (pluginIdx << 3) + modIdx;
      }
      else if( (mm->dst >= STSAMPLE_MM_DST_PLUGIN_1_LEVEL) && (mm->dst <= STSAMPLE_MM_DST_PLUGIN_4_LEVEL) )
      {
         mm->dst = STSAMPLE_MM_DST_PLUGIN_1_LEVEL + revOrder[mm->dst - STSAMPLE_MM_DST_PLUGIN_1_LEVEL];
      }
   }
}

void StSample::swapPluginSlots(sUI _pluginIdxA, sUI _pluginIdxB) {
   if( (_pluginIdxA < STSAMPLE_NUM_PLUGINS) &&
       (_pluginIdxB < STSAMPLE_NUM_PLUGINS) &&
       (_pluginIdxA != _pluginIdxB)
       )
   {
      st_plugin_slot_t t = plugins[_pluginIdxA];
      plugins[_pluginIdxA] = plugins[_pluginIdxB];
      plugins[_pluginIdxB] = t;

      sUI newOrder[STSAMPLE_NUM_PLUGINS/*4*/];
      newOrder[0] = 0u;
      newOrder[1] = 1u;
      newOrder[2] = 2u;
      newOrder[3] = 3u;
      newOrder[_pluginIdxA] = _pluginIdxB;
      newOrder[_pluginIdxB] = _pluginIdxA;
      reorderPluginModMatrixEntries(newOrder);

      updatePluginMask(_pluginIdxA);
      updatePluginMask(_pluginIdxB);
      updatePluginAlgInt();
   }
}

void StSample::reorderPluginSlots(YAC_Object *_ia) {
   if(YAC_Is_IntArray(_ia))
   {
      YAC_CAST_ARG(YAC_IntArray, ia, _ia);
      if(ia->num_elements >= STSAMPLE_NUM_PLUGINS)
      {
         st_plugin_slot_t orig[STSAMPLE_NUM_PLUGINS];
         memcpy((void*)orig, (void*)plugins, sizeof(plugins));
         for(sUI pluginIdx = 0u; pluginIdx < STSAMPLE_NUM_PLUGINS; pluginIdx++)
         {
            plugins[pluginIdx] = orig[ia->elements[pluginIdx]];
            updatePluginMask(pluginIdx);
         }
         // Dyac_host_printf("xxx reorderPluginSlots: ia=(%d; %d; %d; %d)\n", ia->elements[0], ia->elements[1], ia->elements[2], ia->elements[3]);
         reorderPluginModMatrixEntries((const sUI *)ia->elements);
         updatePluginAlgInt();
      }
   }
}

void StSample::_uiSetLastPluginRowIdx(sUI _rowIdx) {
   ui_last_plugin_row_idx = _rowIdx;
}

sUI StSample::_uiGetLastPluginRowIdx(void) {
   return ui_last_plugin_row_idx;
}

sBool StSample::hasPlugins(void) const {
   // called by StSampleVoice::renderBlockNormal()
   for(sUI pluginIdx = 0u; pluginIdx < STSAMPLE_NUM_PLUGINS; pluginIdx++)
   {
      if(NULL != plugins[pluginIdx].shared)
         return YAC_TRUE;
   }
   return YAC_FALSE;
}

sBool StSample::hasPluginsEnabled(void) const {
   for(sUI pluginIdx = 0u; pluginIdx < STSAMPLE_NUM_PLUGINS; pluginIdx++)
   {
      if(NULL != plugins[pluginIdx].shared)
         if(!plugins[pluginIdx].b_skip)
            return YAC_TRUE;
   }
   return YAC_FALSE;
}

sBool StSample::areAllPluginSlotsUsed(void) const {
   sBool r = YAC_TRUE;
   for(sUI pluginIdx = 0u; pluginIdx < STSAMPLE_NUM_PLUGINS; pluginIdx++)
   {
      r = r && (NULL != plugins[pluginIdx].shared);
   }
   return r;
}

void StSample::freePlugins(void) {
   for(sUI pluginIdx = 0u; pluginIdx < STSAMPLE_NUM_PLUGINS; pluginIdx++)
   {
      if(NULL != plugins[pluginIdx].shared)
      {
         YAC_DELETE_SAFE(plugins[pluginIdx].shared);
      }

      if(NULL != plugins[pluginIdx].shared_missing)
      {
         YAC_DELETE_SAFE(plugins[pluginIdx].shared_missing);
      }
   }

   plugin_mask_def = 0u;
   plugin_mask_sr  = 0u;
   plugin_mask_am  = 0u;
   plugin_mask_vb  = 0u;
   plugin_mask_vm  = 0u;
   updatePluginAlgInt();
}

void StSample::_setEnableFX(sBool _bEnable) {
   b_enable_fx = _bEnable;
}

sBool StSample::_getEnableFX(void) {
   return b_enable_fx;
}

void StSample::_setVoiceBus(sSI _voiceBus) {
   voice_bus = _voiceBus;
}

sSI StSample::_getVoiceBus(void) {
   return voice_bus;
}

void StSample::setUiSVOffset(sF32 _offset) {
   uiSVOffset = _offset;
}

sF32 StSample::getUiSVOffset(void) {
   return uiSVOffset;
}

void StSample::setUiSVZoom(YAC_Object *_zoom) {
   if(YAC_Is_Double(_zoom))
   {
      YAC_CAST_ARG(YAC_Double, zoom, _zoom);
      uiSVZoom = zoom->value;
   }
}

void StSample::getUiSVZoom(YAC_Value *_r) {
   YAC_Double *zoom = YAC_New_Double();
   zoom->value = uiSVZoom;
   _r->initObject(zoom, 1);
}

void StSample::setUiSVZoomYIdx(sUI _zoomYIdx) {
   uiSVZoomYIdx = _zoomYIdx;
}

sUI StSample::getUiSVZoomYIdx(void) {
   return uiSVZoomYIdx;
}

void StSample::setUiEnableAuxToWav(sBool _bEnable) {
   ui_b_auxtowav = _bEnable;
}

sBool StSample::getUiEnableAuxToWav(void) {
   return ui_b_auxtowav;
}

void StSample::setVoiceCalibrationValue(sUI _laneIdx, sUI _voiceIdx, sF32 _value) {
   if(_laneIdx > 5u)
      _laneIdx = 5u;
   voice_calibration[_laneIdx][_voiceIdx & 7u] = _value;
}

sF32 StSample::getVoiceCalibrationValue(sUI _laneIdx, sUI _voiceIdx) {
   if(_laneIdx > 5u)
      _laneIdx = 5u;
   return voice_calibration[_laneIdx][_voiceIdx & 7u];
}

void StSample::setVoiceCalibrationModulo(sUI _laneIdx, sUI _modulo) {
   if(_laneIdx > 5u)
      _laneIdx = 5u;
   voice_calibration_modulo[_laneIdx] = sRANGE(_modulo, 1u, 8u);
}

sUI StSample::getVoiceCalibrationModulo(sUI _laneIdx) {
   if(_laneIdx > 5u)
      _laneIdx = 5u;
   return voice_calibration_modulo[_laneIdx];
}

void StSample::setVoiceCalibrationAmount(sUI _laneIdx, sF32 _amt) {
   if(_laneIdx > 5u)
      _laneIdx = 5u;
   voice_calibration_amount[_laneIdx] = _amt;
}

sF32 StSample::getVoiceCalibrationAmount(sUI _laneIdx) {
   if(_laneIdx > 5u)
      _laneIdx = 5u;
   return voice_calibration_amount[_laneIdx];
}

void StSample::setVoiceCalibrationEnable(sUI _laneIdx, sBool _bEnable) {
   if(_laneIdx > 5u)
      _laneIdx = 5u;
   voice_calibration_enable[_laneIdx] = _bEnable;
}

sBool StSample::getVoiceCalibrationEnable(sUI _laneIdx) {
   if(_laneIdx > 5u)
      _laneIdx = 5u;
   return voice_calibration_enable[_laneIdx];
}

void StSample::setAdditiveCfgValid(sUI _cfgIdx, sBool _bEnable) {
   if(_cfgIdx >= STSAMPLE_ADDITIVE_CFG_NUM)
      _cfgIdx = STSAMPLE_ADDITIVE_CFG_NUM - 1u;
   additive_cfg_valid[_cfgIdx] = _bEnable;
}

sBool StSample::getAdditiveCfgValid(sUI _cfgIdx) {
   if(_cfgIdx >= STSAMPLE_ADDITIVE_CFG_NUM)
      _cfgIdx = STSAMPLE_ADDITIVE_CFG_NUM - 1u;
   return additive_cfg_valid[_cfgIdx];
}

void StSample::setAdditiveEditCfg(sF32 _cfgIdx) {
   additive_edit_cfg = _cfgIdx;
}

sF32 StSample::getAdditiveEditCfg(void) {
   return additive_edit_cfg;
}

void StSample::setEnableForceAdditiveEditCfg(sBool _bEnable) {
   b_force_additive_edit_cfg = _bEnable;
}

sBool StSample::getEnableForceAdditiveEditCfg(void) {
   return b_force_additive_edit_cfg;
}

void StSample::setAdditiveNormalize(sUI _cfgIdx, sBool _bEnable) {
   if(_cfgIdx >= STSAMPLE_ADDITIVE_CFG_NUM)
      _cfgIdx = STSAMPLE_ADDITIVE_CFG_NUM - 1u;
   additive_normalize[_cfgIdx] = _bEnable;
}

sBool StSample::getAdditiveNormalize(sUI _cfgIdx) {
   if(_cfgIdx >= STSAMPLE_ADDITIVE_CFG_NUM)
      _cfgIdx = STSAMPLE_ADDITIVE_CFG_NUM - 1u;
   return additive_normalize[_cfgIdx];
}

void StSample::setAdditiveLevel(sUI _cfgIdx, sUI _oscIdx, sF32 _lvl) {
   if(_cfgIdx >= STSAMPLE_ADDITIVE_CFG_NUM)
      _cfgIdx = STSAMPLE_ADDITIVE_CFG_NUM - 1u;
   if(_oscIdx >= STSAMPLE_ADDITIVE_OSC_NUM)
      _oscIdx = STSAMPLE_ADDITIVE_OSC_NUM - 1u;
   additive_osc_lvl[_cfgIdx][_oscIdx] = _lvl;
}

sF32 StSample::getAdditiveLevel(sUI _cfgIdx, sUI _oscIdx) {
   if(_cfgIdx >= STSAMPLE_ADDITIVE_CFG_NUM)
      _cfgIdx = STSAMPLE_ADDITIVE_CFG_NUM - 1u;
   if(_oscIdx >= STSAMPLE_ADDITIVE_OSC_NUM)
      _oscIdx = STSAMPLE_ADDITIVE_OSC_NUM - 1u;
   return additive_osc_lvl[_cfgIdx][_oscIdx];
}

void StSample::setAdditivePhase(sUI _cfgIdx, sUI _oscIdx, sF32 _phase) {
   if(_cfgIdx >= STSAMPLE_ADDITIVE_CFG_NUM)
      _cfgIdx = STSAMPLE_ADDITIVE_CFG_NUM - 1u;
   if(_oscIdx >= STSAMPLE_ADDITIVE_OSC_NUM)
      _oscIdx = STSAMPLE_ADDITIVE_OSC_NUM - 1u;
   additive_osc_pha[_cfgIdx][_oscIdx] = _phase;
}

sF32 StSample::getAdditivePhase(sUI _cfgIdx, sUI _oscIdx) {
   if(_cfgIdx >= STSAMPLE_ADDITIVE_CFG_NUM)
      _cfgIdx = STSAMPLE_ADDITIVE_CFG_NUM - 1u;
   if(_oscIdx >= STSAMPLE_ADDITIVE_OSC_NUM)
      _oscIdx = STSAMPLE_ADDITIVE_OSC_NUM - 1u;
   return additive_osc_pha[_cfgIdx][_oscIdx];
}

void StSample::setAdditiveExpA(sUI _cfgIdx, sUI _oscIdx, sF32 _exp) {
   if(_cfgIdx >= STSAMPLE_ADDITIVE_CFG_NUM)
      _cfgIdx = STSAMPLE_ADDITIVE_CFG_NUM - 1u;
   if(_oscIdx >= STSAMPLE_ADDITIVE_OSC_NUM)
      _oscIdx = STSAMPLE_ADDITIVE_OSC_NUM - 1u;
   additive_osc_exa[_cfgIdx][_oscIdx] = _exp;
}

sF32 StSample::getAdditiveExpA(sUI _cfgIdx, sUI _oscIdx) {
   if(_cfgIdx >= STSAMPLE_ADDITIVE_CFG_NUM)
      _cfgIdx = STSAMPLE_ADDITIVE_CFG_NUM - 1u;
   if(_oscIdx >= STSAMPLE_ADDITIVE_OSC_NUM)
      _oscIdx = STSAMPLE_ADDITIVE_OSC_NUM - 1u;
   return additive_osc_exa[_cfgIdx][_oscIdx];
}

void StSample::setAdditiveFilter(sUI _cfgIdx, sUI _oscIdx, sF32 _f) {
   if(_cfgIdx >= STSAMPLE_ADDITIVE_CFG_NUM)
      _cfgIdx = STSAMPLE_ADDITIVE_CFG_NUM - 1u;
   if(_oscIdx >= STSAMPLE_ADDITIVE_OSC_NUM)
      _oscIdx = STSAMPLE_ADDITIVE_OSC_NUM - 1u;
   additive_osc_flt[_cfgIdx][_oscIdx] = _f;
}

sF32 StSample::getAdditiveFilter(sUI _cfgIdx, sUI _oscIdx) {
   if(_cfgIdx >= STSAMPLE_ADDITIVE_CFG_NUM)
      _cfgIdx = STSAMPLE_ADDITIVE_CFG_NUM - 1u;
   if(_oscIdx >= STSAMPLE_ADDITIVE_OSC_NUM)
      _oscIdx = STSAMPLE_ADDITIVE_OSC_NUM - 1u;
   return additive_osc_flt[_cfgIdx][_oscIdx];
}

void StSample::setAdditiveExpL(sUI _cfgIdx, sUI _oscIdx, sF32 _exp) {
   if(_cfgIdx >= STSAMPLE_ADDITIVE_CFG_NUM)
      _cfgIdx = STSAMPLE_ADDITIVE_CFG_NUM - 1u;
   if(_oscIdx >= STSAMPLE_ADDITIVE_OSC_NUM)
      _oscIdx = STSAMPLE_ADDITIVE_OSC_NUM - 1u;
   additive_osc_exl[_cfgIdx][_oscIdx] = _exp;
}

sF32 StSample::getAdditiveExpL(sUI _cfgIdx, sUI _oscIdx) {
   if(_cfgIdx >= STSAMPLE_ADDITIVE_CFG_NUM)
      _cfgIdx = STSAMPLE_ADDITIVE_CFG_NUM - 1u;
   if(_oscIdx >= STSAMPLE_ADDITIVE_OSC_NUM)
      _oscIdx = STSAMPLE_ADDITIVE_OSC_NUM - 1u;
   return additive_osc_exl[_cfgIdx][_oscIdx];
}

void StSample::_setAdditiveBitReductionPreAmp(sUI _cfgIdx, sF32 _amp) {
   if(_cfgIdx >= STSAMPLE_ADDITIVE_CFG_NUM)
      _cfgIdx = STSAMPLE_ADDITIVE_CFG_NUM - 1u;
   additive_br_preamp[_cfgIdx] = _amp;
}

sF32 StSample::_getAdditiveBitReductionPreAmp(sUI _cfgIdx) {
   if(_cfgIdx >= STSAMPLE_ADDITIVE_CFG_NUM)
      _cfgIdx = STSAMPLE_ADDITIVE_CFG_NUM - 1u;
   return additive_br_preamp[_cfgIdx];
}

void StSample::_setAdditiveBitReduction(sUI _cfgIdx, sUI _shift) {
   if(_cfgIdx >= STSAMPLE_ADDITIVE_CFG_NUM)
      _cfgIdx = STSAMPLE_ADDITIVE_CFG_NUM - 1u;
   additive_br_bits[_cfgIdx] = _shift;
   additive_br_mask[_cfgIdx] = ((1u << _shift) - 1u);
}

sUI StSample::_getAdditiveBitReduction(sUI _cfgIdx) {
   if(_cfgIdx >= STSAMPLE_ADDITIVE_CFG_NUM)
      _cfgIdx = STSAMPLE_ADDITIVE_CFG_NUM - 1u;
   return additive_br_bits[_cfgIdx];
}

void  StSample::_setAdditiveBitReductionMask(sUI _cfgIdx, sUI _mask) {
   if(_cfgIdx >= STSAMPLE_ADDITIVE_CFG_NUM)
      _cfgIdx = STSAMPLE_ADDITIVE_CFG_NUM - 1u;
   additive_br_mask[_cfgIdx] = (sU16)_mask;
}

sUI StSample::_getAdditiveBitReductionMask(sUI _cfgIdx) {
   if(_cfgIdx >= STSAMPLE_ADDITIVE_CFG_NUM)
      _cfgIdx = STSAMPLE_ADDITIVE_CFG_NUM - 1u;
   return (sU16)additive_br_mask[_cfgIdx];
}

void StSample::_setAdditiveRateReduction(sUI _cfgIdx, sUI _shift) {
   if(_cfgIdx >= STSAMPLE_ADDITIVE_CFG_NUM)
      _cfgIdx = STSAMPLE_ADDITIVE_CFG_NUM - 1u;
   additive_smpoff_bits[_cfgIdx] = _shift;
   additive_smpoff_mask[_cfgIdx] = ~((1u << _shift) - 1u);
}

sUI  StSample::_getAdditiveRateReduction(sUI _cfgIdx) {
   if(_cfgIdx >= STSAMPLE_ADDITIVE_CFG_NUM)
      _cfgIdx = STSAMPLE_ADDITIVE_CFG_NUM - 1u;
   return additive_smpoff_bits[_cfgIdx];
}

void StSample::_setAdditiveRateReductionMask(sUI _cfgIdx, sUI _mask) {
   if(_cfgIdx >= STSAMPLE_ADDITIVE_CFG_NUM)
      _cfgIdx = STSAMPLE_ADDITIVE_CFG_NUM - 1u;
   additive_smpoff_mask[_cfgIdx] = ~_mask;
}

sUI StSample::_getAdditiveRateReductionMask(sUI _cfgIdx) {
   if(_cfgIdx >= STSAMPLE_ADDITIVE_CFG_NUM)
      _cfgIdx = STSAMPLE_ADDITIVE_CFG_NUM - 1u;
   return ~additive_smpoff_mask[_cfgIdx];
}

void StSample::_setAdditiveStereoSpread(sF32 _amt) {
   additive_stereo_spread = _amt;
}

sF32 StSample::_getAdditiveStereoSpread(void) {
   return additive_stereo_spread;
}

#if 0
struct ChamberlinFlt {
   sF32 f1;
   sF32 q1;
   sF32 d1;
   sF32 d2;

   ChamberlinFlt(void) {
      f1 = d1 = d2 = 0.0f;
      q1 = 1.0f;
   }

   void init(sF32 _fc, sF32 _q) {
      // fc/q: 0..1
      f1 = _fc * 0.49999f * 0.5f;
      q1 = 1.0f - _q;
   }

   sF32 filter(sF32 _smp) {
      sF32 l = d2 + f1 * d1;
      sF32 h = _smp - l - q1 * d1;
      sF32 b = f1 * h + d1;
      // sF32 n = h + l;

      d1 = b;
      d2 = l;
      return l;
   }
};
#endif

void StSample::recalcAdditiveTblCfg(sUI _cfgIdx) {
   if(_cfgIdx >= STSAMPLE_ADDITIVE_CFG_NUM)
      _cfgIdx = STSAMPLE_ADDITIVE_CFG_NUM - 1u;

   // Dyac_host_printf("xxx recalcAdditiveTblCfg: cfgId=%u valid=%d\n", _cfgIdx, additive_cfg_valid[_cfgIdx]);

   sF32 *d = &additive_tbl_cfg_osc[_cfgIdx][0];
   // StBiquad flt;
   // ChamberlinFlt flt;
   sF32 dt[4096];

   // Sine
   {
      sF32 w = 1.0f / 4096.0f;
      sF32 s  = additive_osc_lvl[_cfgIdx][STSAMPLE_ADDITIVE_OSC_SIN];
      sF32 ea = additive_osc_exa[_cfgIdx][STSAMPLE_ADDITIVE_OSC_SIN];
      sF32 fc = additive_osc_flt[_cfgIdx][STSAMPLE_ADDITIVE_OSC_SIN];
      sF32 el = additive_osc_exl[_cfgIdx][STSAMPLE_ADDITIVE_OSC_SIN];
      sF32 a  = additive_osc_pha[_cfgIdx][STSAMPLE_ADDITIVE_OSC_SIN];
      // sBool bFlt = (fc < 1.0f);
      sF32 fc2 = (powf(2.0f, fc * 10.0f) - 1.0f) / 1024.0f;
      // // flt.init(fc2, 0.0f);
      // // if(bFlt)
      // // {
      // //    Dyac_host_printf("xxx recalcAdditiveTbl: sine fc=%f fc2=%f\n", fc, fc2);
      // //    flt.reset();
      // //    // for(sUI k = 0u; k < 2u; k++)
      // //    //    flt.calcParams(1u/*blkSz*/,
      // //    //                   StBiquad::LPF,
      // //    //                   0.0f/*dbGain*/,
      // //    //                   fc2,
      // //    //                   0.0f/*res*/
      // //    //                   );
      // // }
      for(sUI i = 0u; i < 4096u; i++)
      {
         sF32 t = powf(a, ea);
         sF32 l = sinf(t * sF32(sM_PI * 2.0f));
         dt[i] = l;
         a += w;
         if(a >= 1.0f)
            a -= 1.0f;
      }

      // // flt.filter(d[4094u]);
      // // flt.filter(d[4095u]);
      sF64 p = dt[4095u];
      for(sUI i = 0u; i < 4096u; i++)
      {
         sF32 l = dt[i];
         sF64 lh = p + (l - p) * fc2;
         p = lh;
         l = (sF32)lh;

         // // flt.calcParams(1u/*blkSz*/,
         // //                StBiquad::LPF,
         // //                0.0f/*dbGain*/,
         // //                fc2,
         // //                0.0f/*res*/
         // //                );
         // // l = flt.filter(l);

         if(l < 0.0f)
            l = -powf(-l, el);
         else
            l =  powf( l, el);
         d[i] = l * s;
         // Dyac_host_printf("d[%u] = %f\n", i, d[i]);
      }
   }

   // Triangle
   {
      sF32 w = 1.0f / 4096.0f;
      // sUI i = 0u;
      sF32 s  = additive_osc_lvl[_cfgIdx][STSAMPLE_ADDITIVE_OSC_TRI];
      if(s > 0.0f)
      {
         sF32 ea = additive_osc_exa[_cfgIdx][STSAMPLE_ADDITIVE_OSC_TRI];
         sF32 fc = additive_osc_flt[_cfgIdx][STSAMPLE_ADDITIVE_OSC_TRI];
         sF32 el = additive_osc_exl[_cfgIdx][STSAMPLE_ADDITIVE_OSC_TRI];
         sF32 a  = additive_osc_pha[_cfgIdx][STSAMPLE_ADDITIVE_OSC_TRI];
         // sBool bFlt = (fc < 1.0f);
         sF32 fc2 = (powf(2.0f, fc * 10.0f) - 1.0f) / 1024.0f;
         for(sUI i = 0u; i < 4096u; i++)
         {
            sF32 t = powf(a, ea) * 4.0f;
            sF32 l = (t <= 2.0f) ? (-1.0f + t) : (1.0f - (t - 2.0f));
            dt[i] = l;
            a += w;
            if(a >= 1.0f)
               a -= 1.0f;
         }

         sF64 p = dt[4095u];
         for(sUI i = 0u; i < 4096u; i++)
         {
            sF32 l = dt[i];
            sF64 lh = p + (l - p) * fc2;
            p = lh;
            l = (sF32)lh;

            if(l < 0.0f)
               l = -powf(-l, el);
            else
               l =  powf( l, el);
            d[i] += l * s;
         }
      }
   }

   // Saw Up
   {
      sF32 w = 1.0f / 4096.0f;
      // sUI i = 0u;
      sF32 s  = additive_osc_lvl[_cfgIdx][STSAMPLE_ADDITIVE_OSC_SUP];
      if(s > 0.0f)
      {
         sF32 ea = additive_osc_exa[_cfgIdx][STSAMPLE_ADDITIVE_OSC_SUP];
         sF32 fc = additive_osc_flt[_cfgIdx][STSAMPLE_ADDITIVE_OSC_SUP];
         sF32 el = additive_osc_exl[_cfgIdx][STSAMPLE_ADDITIVE_OSC_SUP];
         sF32 a  = additive_osc_pha[_cfgIdx][STSAMPLE_ADDITIVE_OSC_SUP];
         // sBool bFlt = (fc < 1.0f);
         sF32 fc2 = (powf(2.0f, fc * 10.0f) - 1.0f) / 1024.0f;
         for(sUI i = 0u; i < 4096u; i++)
         {
            sF32 t = powf(a, ea) * 2.0f;
            sF32 l = -1.0f + t;
            dt[i] = l;
            a += w;
            if(a >= 1.0f)
               a -= 1.0f;
         }

         sF64 p = dt[4095u];
         for(sUI i = 0u; i < 4096u; i++)
         {
            sF32 l = dt[i];
            sF64 lh = p + (l - p) * fc2;
            p = lh;
            l = (sF32)lh;

            if(l < 0.0f)
               l = -powf(-l, el);
            else
               l =  powf( l, el);
            d[i] += l * s;
         }
      }
   }

   // Saw Down
   {
      sF32 w = 1.0f / 4096.0f;
      // sUI i = 0u;
      sF32 s  = additive_osc_lvl[_cfgIdx][STSAMPLE_ADDITIVE_OSC_SDN];
      if(s > 0.0f)
      {
         sF32 ea = additive_osc_exa[_cfgIdx][STSAMPLE_ADDITIVE_OSC_SDN];
         sF32 fc = additive_osc_flt[_cfgIdx][STSAMPLE_ADDITIVE_OSC_SDN];
         sF32 el = additive_osc_exl[_cfgIdx][STSAMPLE_ADDITIVE_OSC_SDN];
         sF32 a  = additive_osc_pha[_cfgIdx][STSAMPLE_ADDITIVE_OSC_SDN];
         // sBool bFlt = (fc < 1.0f);
         sF32 fc2 = (powf(2.0f, fc * 10.0f) - 1.0f) / 1024.0f;
         for(sUI i = 0u; i < 4096u; i++)
         {
            sF32 t = powf(a, ea) * 2.0f;
            sF32 l = 1.0f - t;
            dt[i] = l;
            a += w;
            if(a >= 1.0f)
               a -= 1.0f;
         }

         sF64 p = dt[4095u];
         for(sUI i = 0u; i < 4096u; i++)
         {
            sF32 l = dt[i];
            sF64 lh = p + (l - p) * fc2;
            p = lh;
            l = (sF32)lh;

            if(l < 0.0f)
               l = -powf(-l, el);
            else
               l =  powf( l, el);
            d[i] += l * s;
         }
      }
   }

   // Pulse
   {
      sF32 w = 1.0f / 4096.0f;
      // sUI i = 0u;
      sF32 s  = additive_osc_lvl[_cfgIdx][STSAMPLE_ADDITIVE_OSC_PUL];
      if(s > 0.0f)
      {
         sF32 ea = additive_osc_exa[_cfgIdx][STSAMPLE_ADDITIVE_OSC_PUL];
         sF32 fc = additive_osc_flt[_cfgIdx][STSAMPLE_ADDITIVE_OSC_PUL];
         sF32 el = additive_osc_exl[_cfgIdx][STSAMPLE_ADDITIVE_OSC_PUL];
         sF32 a  = additive_osc_pha[_cfgIdx][STSAMPLE_ADDITIVE_OSC_PUL];
         // sBool bFlt = (fc < 1.0f);
         sF32 fc2 = (powf(2.0f, fc * 10.0f) - 1.0f) / 1024.0f;
         // Dyac_host_printf("xxx pulse ea=%f el=%f\n", ea, el);
         // (todo) generate "analog" pulse
         for(sUI i = 0u; i < 4096u; i++)
         {
            sF32 t = powf(a, ea);
            sF32 l = (t <= 0.5f) ? 1.0f : -1.0f;
            dt[i] = l;
            a += w;
            if(a >= 1.0f)
               a -= 1.0f;
         }

         sF64 p = dt[4095u];
         for(sUI i = 0u; i < 4096u; i++)
         {
            sF32 l = dt[i];
            sF64 lh = p + (l - p) * fc2;
            p = lh;
            l = (sF32)lh;

            if(l < 0.0f)
               l = -powf(-l, el);
            else
               l =  powf( l, el);
            d[i] += l * s;
         }
      }
   }

   // Normalize (write to temp)
   if(additive_normalize[_cfgIdx])
   {
      // find absmax
      sF32 m = 0.0f;
      for(sUI i = 0u; i < 4096u; i++)
      {
         sF32 l = d[i];
         if(l < 0.0f)
            l = -l;
         if(l > m)
            m = l;
      }
      // normalize
      sF32 s = (m > 0.0f) ? (1.0f / m) : 0.0f;
      for(sUI i = 0u; i < 4096u; i++)
         dt[i] = d[i] * s;
   }
   else
   {
      memcpy(dt, d, sizeof(sF32)*4096u);
   }

   // Apply SR+BR (read from temp, write to dest)
   {
      sUI  srMask   =  additive_smpoff_mask[_cfgIdx];
      sF32 brPreamp =  additive_br_preamp  [_cfgIdx];
      sUI  brMask   =  additive_br_mask    [_cfgIdx];

      // Dyac_host_printf("xxx srMask=0x%08x brPreamp=%f brMask=0x%04x\n", srMask, brPreamp, brMask);

      for(sUI i = 0u; i < 4096u; i++)
      {
         sF32 f = dt[i & srMask];
         if((brPreamp > 1.0f) && (brMask > 0u))
            f = tanhf(f * brPreamp);
         else
            f *= brPreamp;

         if(brMask)
         {
            sS16 sam16 = sS16(f * 32767.0f);
            if(sam16 >= 0)
            {
               if(sam16 > 32767)
                  sam16 = 32767;
               sam16 &= ~brMask;
               f = sam16 / 32767.0f;
            }
            else
            {
               sam16 = -sam16;
               if(sam16 > 32767)
                  sam16 = 32767;
               sam16 &= ~brMask;
               f = sam16 / -32767.0f;
            }
         }

         d[i] = f;
      }
   }

   additive_tbl_cfg[_cfgIdx] = d;
}

void StSample::recalcAdditiveTbl_int(sSI _cfgIdx, sF32 *_dOrNull, sF32 _fIdx) {
   // (note) 'fIdx' must be valid when 'dOrNull' is not NULL

   if(_cfgIdx < 0)
   {
      for(sUI i = 0u; i < STSAMPLE_ADDITIVE_CFG_NUM; i++)
      {
         if(additive_cfg_valid[i])
            recalcAdditiveTblCfg(i);
      }
   }
   else if(sUI(_cfgIdx) < 8u)
   {
      sUI i = sUI(_cfgIdx);
      if(additive_cfg_valid[i])
         recalcAdditiveTblCfg(i);
   }
   // else: just mix the existing osc tables

   sF32 fIdx = b_force_additive_edit_cfg ? additive_edit_cfg : _fIdx;
   // Dyac_host_printf("xxx b_force_additive_edit_cfg=%d fIdx=%f\n", b_force_additive_edit_cfg, fIdx);
   sF32 bAmt = fIdx - sSI(fIdx);
   const sF32 *srcA = &additive_tbl_cfg[sUI(fIdx) & 7u][0];
   const sF32 *srcB = &additive_tbl_cfg[(sUI(fIdx) + 1u) & 7u][0];
   sF32 *d = (b_force_additive_edit_cfg || (NULL == _dOrNull)) ? additive_tbl_mix : _dOrNull;
   for(sUI i = 0u; i < 4096u; i++)
   {
      d[i] = srcA[i] + (srcB[i] - srcA[i]) * bAmt;
   }

   if(d == additive_tbl_mix)
      additive_tbl = additive_tbl_mix;
}

void StSample::recalcAdditiveTbl(sSI _cfgIdx) {
   recalcAdditiveTbl_int(_cfgIdx, NULL/*dOrNull*/, additive_edit_cfg);
}

void StSample::exportAdditiveWavetable(YAC_FloatArray *_d, sUI _numCh, sUI _outCycleLen, sF32 _rateFactor, sUI _numWaves) {
   if(NULL != partial_magnitudes && NULL != partial_speeds && NULL != partial_phases && _outCycleLen >= 64u)
   {
      sF32 *d = _d->elements;

      const sUI outCycleLen = sUI(_outCycleLen);

      const sBool bXFadeIn  = 1;
      const sBool bXFadeOut = 1;

      sU16 partialPhases16Fade[256];
      sU16 partialPhases16Cur[256];
      sU16 partialSpeeds16 [256];

      const sF32 rateFactor = 65536.0f * 0.5f * _rateFactor;

      recalcAdditiveTbl_int(-1, NULL, additive_edit_cfg);
      const sF32 *atbl = additive_tbl_mix;

      sF32 additiveStereoSpread = voice_calibration_enable[1/*pan*/] ? additive_stereo_spread : 0.0f;
      additiveStereoSpread = sRANGE(additiveStereoSpread, 0.0f, 1.0f);
      const sBool bAdditiveStereoSpread = (0.0f != additiveStereoSpread) && (2u == _numCh);
      const sF32 fBoost = powf(2.0f, timestretch_granularity_amount * 8.0f);

      sF32 numPartialsF = num_partials;
      numPartialsF = sRANGE(numPartialsF, 1.0f, 256.0f);
      sUI numPartials = sUI(numPartialsF);
      sF32 lastPartialAmt = numPartialsF - numPartials;

      Dyac_host_printf("xxx numPartials=%u lastPartialAmt=%f\n", numPartials, lastPartialAmt);

      sUI k = 0u;
      sUI waveIdx = 0u;

      for(; waveIdx < _numWaves; waveIdx++)
      {
         sUI j = waveIdx * 256u;

         for(sUI partialIdx = 0u; partialIdx < 256u; partialIdx++)
         {
            partialSpeeds16[partialIdx] = (sU16) (partial_speeds->elements[j + partialIdx] * rateFactor);
         }

         if(b_partial_phase_0)
         {
            memset(partialPhases16Cur, 0, sizeof(partialPhases16Cur));
            // ::memcpy((void*)partialPhases16Cur, (const void*)partialSpeeds16, sizeof(partialSpeeds16));
         }
         else
         {
            for(sUI partialIdx = 0u; partialIdx < 256u; partialIdx++)
               partialPhases16Cur[partialIdx] = sU16(partial_phases->elements[j + partialIdx] * 65536);
         }

         const sF32 *partialAmps = partial_magnitudes->elements + j;

         sUI frameIdx = 0u;

         if(bXFadeIn)
         {
            // x-fade first 32 frames of next cycle iteration (declick / 'fdrst')

            for(sUI partialIdx = 0u; partialIdx < 256u; partialIdx++)
            {
               // partialPhases16Fade[partialIdx] = partialPhases16Cur[partialIdx] + (partialSpeeds16[partialIdx] * _outCycleLen);  // prec. issue
               partialPhases16Fade[partialIdx] = 
                  partialPhases16Cur[partialIdx] +
                  sU16( partial_speeds->elements[j + partialIdx] * _outCycleLen * rateFactor )
                  ;
            }

            for(; frameIdx < 32u; frameIdx++)
            {
               if(bAdditiveStereoSpread)
               {
                  sF32 newSmpL;
                  sF32 newSmpR;
                  StSampleVoice::sum_partials_stereo_srr_br(atbl, partialPhases16Fade, partialSpeeds16, partialAmps,
                                                            numPartials, lastPartialAmt,
                                                            &voice_calibration[1/*pan*/][0], additiveStereoSpread,
                                                            (smpoff_mask<<6), br_preamp, br_mask,
                                                            newSmpL, newSmpR
                                                            );

                  sF32 newSmpLB;
                  sF32 newSmpRB;
                  StSampleVoice::sum_partials_stereo_srr_br(atbl, partialPhases16Cur, partialSpeeds16, partialAmps,
                                                            numPartials, lastPartialAmt,
                                                            &voice_calibration[1/*pan*/][0], additiveStereoSpread,
                                                            (smpoff_mask<<6), br_preamp, br_mask,
                                                            newSmpLB, newSmpRB
                                                            );

                  d[k++] = (newSmpL + (newSmpLB - newSmpL) * (frameIdx / 32.0f)) * fBoost;
                  d[k++] = (newSmpR + (newSmpRB - newSmpR) * (frameIdx / 32.0f)) * fBoost;
               }
               else
               {
                  sF32 newSmp = StSampleVoice::sum_partials_srr_br(atbl, partialPhases16Fade, partialSpeeds16, partialAmps,
                                                                   numPartials, lastPartialAmt,
                                                                   smpoff_mask<<6, br_preamp, br_mask
                                                                   );


                  sF32 newSmpB = StSampleVoice::sum_partials_srr_br(atbl, partialPhases16Cur, partialSpeeds16, partialAmps,
                                                                    numPartials, lastPartialAmt,
                                                                    smpoff_mask<<6, br_preamp, br_mask
                                                                    );

                  d[k++] = (newSmp + (newSmpB - newSmp) * (frameIdx / 32.0f)) * fBoost;
               }
            }
         }

         sUI numCenter = bXFadeOut ? (outCycleLen - 32u) : outCycleLen;

         for(; frameIdx < numCenter; frameIdx++)
         {
            if(bAdditiveStereoSpread)
            {
               sF32 newSmpLB;
               sF32 newSmpRB;
               StSampleVoice::sum_partials_stereo_srr_br(atbl, partialPhases16Cur, partialSpeeds16, partialAmps,
                                                         numPartials, lastPartialAmt,
                                                         &voice_calibration[1/*pan*/][0], additiveStereoSpread,
                                                         (smpoff_mask<<6), br_preamp, br_mask,
                                                         newSmpLB, newSmpRB
                                                         );

               d[k++] = newSmpLB * fBoost;
               d[k++] = newSmpRB * fBoost;
            }
            else
            {
               sF32 newSmpB = StSampleVoice::sum_partials_srr_br(atbl, partialPhases16Cur, partialSpeeds16, partialAmps,
                                                                 numPartials, lastPartialAmt,
                                                                 smpoff_mask<<6, br_preamp, br_mask
                                                                 );

               d[k++] = newSmpB * fBoost;
            }
         }

         if(bXFadeOut)
         {
            // [14Oct2024] X-fade last 32 frames
            if(b_partial_phase_0)
            {
               memset(partialPhases16Fade, 0, sizeof(partialPhases16Fade));
               // ::memcpy((void*)partialPhases16Fade, (const void*)partialSpeeds16, sizeof(partialSpeeds16));
            }
            else
            {
               for(sUI partialIdx = 0u; partialIdx < 256u; partialIdx++)
                  partialPhases16Fade[partialIdx] = sU16(partial_phases->elements[j + partialIdx] * 65536);
            }

            for(sUI partialIdx = 0u; partialIdx < 256u; partialIdx++)
            {
               // partialPhases16Fade[partialIdx] = sU16(partialPhases16Fade[partialIdx] - (partialSpeeds16[partialIdx] * 32));
               // 
               partialPhases16Fade[partialIdx] = sU16(partialPhases16Fade[partialIdx] - 
                                                      sUI(partial_speeds->elements[j + partialIdx] * 32.0f * rateFactor)
                                                      )
                                                      ;
            }

            sF32 fadeOutAmt = 32.0f/32.0f;

            for(; frameIdx < outCycleLen; frameIdx++)
            {
               if(bAdditiveStereoSpread)
               {
                  sF32 newSmpL;
                  sF32 newSmpR;
                  StSampleVoice::sum_partials_stereo_srr_br(atbl, partialPhases16Fade, partialSpeeds16, partialAmps,
                                                            numPartials, lastPartialAmt,
                                                            &voice_calibration[1/*pan*/][0], additiveStereoSpread,
                                                            (smpoff_mask<<6), br_preamp, br_mask,
                                                            newSmpL, newSmpR
                                                            );

                  sF32 newSmpLB;
                  sF32 newSmpRB;
                  StSampleVoice::sum_partials_stereo_srr_br(atbl, partialPhases16Cur, partialSpeeds16, partialAmps,
                                                            numPartials, lastPartialAmt,
                                                            &voice_calibration[1/*pan*/][0], additiveStereoSpread,
                                                            (smpoff_mask<<6), br_preamp, br_mask,
                                                            newSmpLB, newSmpRB
                                                            );

                  d[k++] = (newSmpL + (newSmpLB - newSmpL) * fadeOutAmt) * fBoost;
                  d[k++] = (newSmpR + (newSmpRB - newSmpR) * fadeOutAmt) * fBoost;
               }
               else
               {
                  sF32 newSmp = StSampleVoice::sum_partials_srr_br(atbl, partialPhases16Fade, partialSpeeds16, partialAmps,
                                                                   numPartials, lastPartialAmt,
                                                                   smpoff_mask<<6, br_preamp, br_mask
                                                                   );


                  sF32 newSmpB = StSampleVoice::sum_partials_srr_br(atbl, partialPhases16Cur, partialSpeeds16, partialAmps,
                                                                    numPartials, lastPartialAmt,
                                                                    smpoff_mask<<6, br_preamp, br_mask
                                                                    );

                  d[k++] = (newSmp + (newSmpB - newSmp) * fadeOutAmt) * fBoost;
               }
               fadeOutAmt -= 1.0f / 32.0f;
            }
         }

      } // loop waves

   }
}

void StSample::_setAiNumPoles(sUI _numPoles) {
   ai_num_poles = _numPoles;
}

sBool StSample::_getAiNumPoles(void) {
   return ai_num_poles;
}

void StSample::_setAiQ(sF32 _q) {
   ai_q = sRANGE(_q, 0.0f, 1.0f);
}

sF32 StSample::_getAiQ(void) {
   return ai_q;
}

void StSample::_setAiExpUp(sF32 _exp) {
   ai_exp_up = _exp;
}

sF32 StSample::_getAiExpUp(void) {
   return ai_exp_up;
}

void StSample::_setAiExpDown(sF32 _exp) {
   ai_exp_down = _exp;
}

sF32 StSample::_getAiExpDown(void) {
   return ai_exp_down;
}

void StSample::_setAiLinOct(sF32 _oct) {
   ai_lin_oct = _oct;
}

sF32 StSample::_getAiLinOct(void) {
   return ai_lin_oct;
}

void StSample::_setAiLinMax(sF32 _max) {
   ai_lin_max = _max;
}

sF32 StSample::_getAiLinMax(void) {
   return ai_lin_max;
}

void StSample::_setStartEndFadeNumFrames(sUI _numFrames) {
   if(_numFrames > 32768u)
      _numFrames = 32768u;
   startend_fade_num_frames = _numFrames;
}

sUI StSample::_getStartEndFadeNumFrames(void) {
   return startend_fade_num_frames;
}

void StSample::_uiSetEnableAutoSelect(sBool _bEnable) {
   b_ui_autoselect = _bEnable;
}

sBool StSample::_uiGetEnableAutoSelect(void) {
   return b_ui_autoselect;
}

sBool StSample::_setTuningTable(sUI _idx, YAC_Object *_fa) {
   if(_idx < STSAMPLE_MAX_TUNING_TABLES)
   {
      if(YAC_Is_FloatArray(_fa))
      {
         YAC_CAST_ARG(YAC_FloatArray, fa, _fa);
         if(fa->num_elements >= 128u)
         {
            if(NULL == tuning_tables[_idx])
               tuning_tables[_idx] = new(std::nothrow)sF32[128];
            ::memcpy((void*)tuning_tables[_idx], (const void*)fa->elements, sizeof(sF32)*128);
            return YAC_TRUE;
         }
      }

      // Failed to update table (e.g. 'fa' is null), free old one
      if(NULL != tuning_tables[_idx])
      {
         delete [] tuning_tables[_idx];
         tuning_tables[_idx] = NULL;
      }
   }
   return YAC_FALSE;
}

void StSample::_getTuningTable(sUI _idx, YAC_Value *_r) {
   _r->initNull();
   if(_idx < STSAMPLE_MAX_TUNING_TABLES)
   {
      if(NULL != tuning_tables[_idx])
      {
         YAC_FloatArray *fa = YAC_New_FloatArray();
         if(NULL != fa && fa->yacArrayAlloc(128))
         {
            ::memcpy((void*)fa->elements, (const void*)tuning_tables[_idx], sizeof(sF32)*128);
            fa->num_elements = 128u;
            _r->initObject(fa, YAC_TRUE);
         }
      }
   }
}

void StSample::_setDefaultTuningTableIdx(sSI _idx) {
   default_tuning_table_idx = _idx;
}

sSI StSample::_getDefaultTuningTableIdx(void) {
   return default_tuning_table_idx;
}

void StSample::_setForcedTuningTableIdx(sSI _idx) {
   forced_tuning_table_idx = _idx;
}

sSI StSample::_getForcedTuningTableIdx(void) {
   return forced_tuning_table_idx;
}

void StSample::_setTuningTableMetaData(sUI _idx, YAC_Object *_voOrNull) {
   if(_idx < STSAMPLE_MAX_TUNING_TABLES)
   {
      if(NULL != tuning_tables_meta[_idx])
      {
         YAC_DELETE_SAFE(tuning_tables_meta[_idx]);
      }

      if(YAC_VALID(_voOrNull))
      {
         YAC_DEREF_ARG(YAC_Object, o, _voOrNull);
         if(o_deleteme)
         {
            tuning_tables_meta[_idx] = o;
         }
      }
   }
}

YAC_Object *StSample::_getTuningTableMetaData(sUI _idx) {
   if(_idx < STSAMPLE_MAX_TUNING_TABLES)
      return tuning_tables_meta[_idx];
   return NULL;
}

void StSample::_freeTuningTables(void) {
   for(sUI i = 0u; i < STSAMPLE_MAX_TUNING_TABLES; i++)
   {
      if(NULL != tuning_tables[i])
      {
         delete [] tuning_tables[i];
         tuning_tables[i] = NULL;
      }
   }
}

void StSample::_freeTuningTablesMetaData(void) {
   for(sUI i = 0u; i < STSAMPLE_MAX_TUNING_TABLES; i++)
   {
      if(NULL != tuning_tables_meta[i])
      {
         YAC_DELETE_SAFE(tuning_tables_meta[i]);
      }
   }
}

const sF32 *StSample::getCurrentTuningTableOrNull(void) {
   const sF32 *ret = NULL;
   if(NULL == parent_samplebank || parent_samplebank->forced_tuning_table_idx < 0)
   {
      if(forced_tuning_table_idx >= 0 && forced_tuning_table_idx < STSAMPLE_MAX_TUNING_TABLES)
      {
         ret = tuning_tables[forced_tuning_table_idx];
      }
      else if(default_tuning_table_idx >= 0 && default_tuning_table_idx < STSAMPLE_MAX_TUNING_TABLES)
      {
         ret = tuning_tables[default_tuning_table_idx];
      }
   }
   return ret;
}

void StSample::_mmVarSetEnable(sBool _bEnable) {
   b_mmvar_enable = _bEnable;
}

sBool StSample::_mmVarGetEnable(void) {
   return b_mmvar_enable;
}

void StSample::_mmVarSetNum(sUI _num) {
   mmvar_num = _num;
}

sUI StSample::_mmVarGetNum(void) {
   return mmvar_num;
}

void StSample::_mmVarSetEnableSmooth(sBool _bEnable) {
   b_mmvar_smooth = _bEnable;
}

sBool StSample::_mmVarGetEnableSmooth(void) {
   return b_mmvar_smooth;
}

void StSample::_mmVarSetEditIdx(sUI _editIdx) {
   mmvar_edit_idx = _editIdx;
}

sUI StSample::_mmVarGetEditIdx(void) {
   return mmvar_edit_idx;
}

void StSample::_mmVarSetEnableForceEditIdx(sBool _bEnable) {
   b_mmvar_force_edit_idx = _bEnable;
}

sBool StSample::_mmVarGetEnableForceEditIdx(void) {
   return b_mmvar_force_edit_idx;
}

void StSample::_mmVarGetData(YAC_Value *_r) {
   YAC_FloatArray *fa = YAC_New_FloatArray();
   fa->max_elements = STSAMPLE_NUM_MODMATRIX_ENTRIES * STSAMPLE_MMVAR_NUM;;
   fa->num_elements = fa->max_elements;
   fa->elements = mmvar_data;
   _r->initObject(fa, 1);
}
