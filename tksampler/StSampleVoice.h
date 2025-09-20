// ----
// ---- file   : StSampleVoice.h
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
// ----          19Jan2010, 21Jan2010, 27Jan2010, 25Feb2010, 28Feb2010, 01Mar2010, 11Apr2010
// ----          29Apr2010, 04May2010, 28Jun2010, 29Jun2010, 19Jul2010, 04Sep2010, 05Sep2010
// ----          09Sep2010, 05Mar2011, 10Mar2011, 28Mar2011, 29Mar2011, 09May2013, 13Sep2015
// ----          19Dec2018, 29Dec2018, 04Jan2019, 07Jan2019, 08Jan2019, 17Jan2019, 24Mar2019
// ----          24Mar2019, 30Mar2019, 31Mar2019, 11May2019, 12May2019, 13May2019, 15May2019
// ----          16May2019, 18Jun2019, 12Jul2019, 13Jul2019, 01Aug2019, 29Nov2019, 15Feb2020
// ----          16Feb2020, 19Feb2020, 21Feb2020, 23Feb2020, 24Feb2020, 26Feb2020, 27Feb2020
// ----          17May2020, 18May2020, 20May2020, 22May2020, 23May2020, 24May2020, 25May2020
// ----          28May2020, 01Jun2020, 06Jun2020, 08Jun2020, 26Apr2021, 28Apr2021, 30Apr2021
// ----          03May2021, 20May2021, 26Jun2021, 01Aug2021, 30Aug2021, 04Oct2021, 20Oct2021
// ----          23Nov2022, 24Nov2022, 10Dec2022, 20Dec2022, 22Dec2022, 23Dec2022, 27Dec2022
// ----          28Dec2022, 29Dec2022, 30Dec2022, 31Dec2022, 09Mar2023, 03Apr2023, 14Aug2023
// ----          16Aug2023, 14Sep2023, 10Jan2024, 11Jan2024, 13Jan2024, 14Jan2024, 15Jan2024
// ----          16Jan2024, 05Aug2024, 28Sep2024, 01Oct2024, 02Oct2024, 03Oct2024, 03Nov2024
// ----          03Jan2025, 04Jan2025
// ----
// ----
// ----

#ifndef ST_SAMPLEVOICE_H__
#define ST_SAMPLEVOICE_H__

YG("tksampler")


class StSamplePlayer;

class StSampleVoice;
typedef void(StSampleVoice::*stsamplevoice_process_alg_fxn_t)(void);

/// When defined, use crossfade buffer.
/// When undefined, use last frame interpolation
#define USE_SAMPLESHIFT_EOL_XFADE_BUF  defined

/// When defined, use xfade buffer if loop length is smaller than ramp size
#define USE_SAMPLESHIFT_EOL_XFADE_BUF_THRESHOLD  defined

// Maximum size of voice bus buffer
//  (note) also determines the maximum buffer size for StSamplePlayer::render()
#define STSAMPLEVOICE_MAX_VOICE_BUS_FRAMES  128u

// Must equal plugin.h:ST_PLUGIN_MAX_LAYERS
//   == maximum number of voices buses
#define STSAMPLEVOICE_MAX_LAYERS  32u

#define STSAMPLEVOICE_AI_MAX_POLES  8


class StSampleVoiceNoteOnParams {
   // Used for delayed note-ons (if voice is still playing-> fadeout first)
  public:
   sBool b_valid;

   StSample   *_sample;
   const sF32 *_freqTable;

   sF32 _mixRate;
   sSI  _note;

   // Initial ctls/params
   sF32 _vel;
   sF32 _mod;
   sF32 _vol;
   sF32 _vol2;        // randomization
   sF32 _pan;
   sF32 _pan2;        // randomization
   sF32 _freq;
   sF32 _freq2;       // randomization
   sF32 _timestretch;
   sF32 _timestretch_bend;
   sF32 _sampleoff;
   sF32 _sampleoff_ms;  // for audio-track like samples (remote profile RPNs 98+99, see ModSample)
   sF32 _sampleoff2;  // randomization
   sF32 _cyclelen;
   sF32 _sampleshift;
   sF32 _sampleshift_endofloop;
   sF32 _altvolume;
   sUI  _retrigmask;
   sF32 _lfofreqspd;
   sF32 _lfofreqlvl;
   sF32 _lfofreqamt;
   sF32 _lfovolspd;
   sF32 _lfovollvl;
   sF32 _lfovolamt;
   sF32 _lfopanspd;
   sF32 _lfopanlvl;
   sF32 _lfopanamt;
   sF32 _lfoauxspd;
   sF32 _lfoauxlvl;
   sF32 _lfoauxfltamt;
   sF32 _adsrfreqspd;
   sF32 _adsrfreqlvl;
   sF32 _adsrfreqint;
   sF32 _adsrvolspd;
   sF32 _adsrvollvl;
   sF32 _adsrvolint;
   sF32 _adsrpanspd;
   sF32 _adsrpanlvl;
   sF32 _adsrpanint;
   sF32 _adsrauxspd;
   sF32 _adsrauxlvl;
   sF32 _adsrauxint;
   sSI  _jumptoloop;
   sSI  _wavepathidx;
   sF32 _afreqspd;
   sF32 _sfreqspd;
   sF32 _rfreqspd;
   sF32 _avolspd;
   sF32 _svolspd;
   sF32 _rvolspd;
   sF32 _apanspd;
   sF32 _spanspd;
   sF32 _rpanspd;
   sF32 _aauxspd;
   sF32 _sauxspd;
   sF32 _rauxspd;
   sF32 _filter_cutoff;
   sF32 _filter_pan;
   sF32 _filter_offset;
   sF32 _filter_resonance;
   sBool _glide_switch;
   sF32  _glide_speed;
   sF32  _perfctl_poly_pressure;  // -1=use sampleplayer perfctl (channel pressure), 0..127 otherwise
   sUI  _modseq_retrigmask;
   sF32 _modseq_speed   [STSAMPLE_NUM_MODSEQ];
   sF32 _modseq_level   [STSAMPLE_NUM_MODSEQ];
   sF32 _modseq_numsteps[STSAMPLE_NUM_MODSEQ];
   sF32 _modseq_advance [STSAMPLE_NUM_MODSEQ];
   sUI  _modseq_patch   [STSAMPLE_NUM_MODSEQ];  // 0..(STSAMPLE_MAX_MODSEQ_PATCHES-1)
};


struct tksampler_mmdst_t {
   sF32  vol;
   sF32  pan;

   sF32  freq;            // -1..1 => -48.0..+48.0
   sF32  freq_oct;
   sF32  freq_fifth;
   sF32  freq_fourth;
   sF32  freq_semi;
   sF32  freq_fine;
   sF32  freq_beat;
   sF32  tuning_tbl_abs;  // 0=no change, 1/16..16/16 => tuning table 1..16
   sF32  tuning_tbl_rel;  // 0..f => tuning table 0..f  (interpolated)
   sF32  note_2;          // 0..1 => 0..127.0  (0=disable secondary freq. centered around C-5=60.0)
   sF32  freq_2;          // -127..127 (_not_ scaled)

   sF32  flt_cutoff;
   sF32  flt_pan;
   sF32  flt_offset;
   sF32  flt_res;

   sF32  jumptoloop;
   sBool b_jumptoloop_valid;
   sF32  sampleshift_endofloop;
   sBool b_sampleshift_endofloop_valid;
   sF32  looprestart;
   sF32  wt2d_x;
   sBool b_wt2d_x_valid;
   sF32  wt2d_y;
   sBool b_wt2d_y_valid;

   sF32  freq_lfo_freq_amt;
   sF32  vol_lfo_vol_amt;
   sF32  pan_lfo_pan_amt;
   sF32  aux_lfo_flt_amt;

   sF32  retrig_freq_env;
   sF32  retrig_vol_env;
   sF32  retrig_pan_env;
   sF32  retrig_aux_env;

   sF32  retrig_freq_lfo;
   sF32  retrig_vol_lfo;
   sF32  retrig_pan_lfo;
   sF32  retrig_aux_lfo;

   sF32  retrig_modseq[STSAMPLE_NUM_MODSEQ];

   sF32  lfo_freq_level;
   sF32  lfo_vol_level;
   sF32  lfo_pan_level;
   sF32  lfo_aux_level;

   sF32  env_freq_level;
   sF32  env_vol_level;
   sF32  env_pan_level;
   sF32  env_aux_level;

   sF32  modseq_level[STSAMPLE_NUM_MODSEQ];
   sF32  modseq_patch[STSAMPLE_NUM_MODSEQ];  // 0...STSAMPLE_MAX_MODSEQ_PATCHES-1.  modmatrix amount should be set to 12700%.
   sF32  modseq[STSAMPLE_NUM_MODSEQ];  // final modseq output (scaled by level). after modmatrix processing.

   // Final output (after mod matrix processing)
   struct {
      sF32 freq;
      sF32 vol;
      sF32 pan;
      sF32 aux;
   } lfo;

   struct {
      sF32 freq;
      sF32 vol;
      sF32 pan;
      sF32 aux;
   } env;

   sF32 liverec_start;     // >= 0.5: (re-)trigger live recording
   sF32 liverec_continue;  // >= 0.5: (re-)start live recording when not already active
   sF32 liverec_stop;      // >= 0.5: stop live recording

   sUI  plugin_mod_mask;  // one bit per plugin modulation target (8 bits per plugin, 8*4=32 bits total). 1=plugin modulation changed
   sF32 plugins[STSAMPLE_NUM_PLUGINS][STSAMPLE_NUM_MODS_PER_PLUGIN];
   sF32 plugin_levels[STSAMPLE_NUM_PLUGINS];  // *
};


/* @class StSampleVoice,Object
 *
 *  The replayer (linearly) interpolates frequency/volume and panning across a "sample block".
 *  Samples are also interpolated linearly.
 *  Volume ramping can be done via the A(DS)R envelopes.
 *
 *  Updated vol/pan/freq modulation info is read when a new block is started (copied from "queued" shadow members).
 *  The default block size/length is STSAMPLEVOICE_BLOCK_LEN seconds, or 44.1 samples @44100Hz mixrate.
 *
 *  The update mechanism is not "sample realtime" but behaves more like a hardware MIDI/USB sampler.
 *  The latency depends on the output buffer size.
 *  I recommend 256 samples @ 48Khz, i.e. 5 millisec latency. This should be fast enough for LFOs and arpeggiators..
 *  (also see the tkportaudio plugin for low latency audio I/O via Steinberg's ASIO2 API on Win32)
 *
 *  The voice is marked inactive when
 *    a) The end of the sample has been reached and there are no more loops
 *    b) The volume release envelope finishes (i.e. some time after a note-off or immediately if there is no vol. envelope)
 *
 *  The current replay note+finetune is mapped via a user-defined frequency table (can be changed per-voice).
 *
 *  Reverse play and "scratch" fx currently only work with loop samples (i.e. the whole sample will be looped).
 */
YC class StSampleVoice : public YAC_Object {

// number of ticks per sec => tickduration/blocklen is
//     0.0005f sec @ 2000 ticks / 48KHz samplerate
//  or 0.0010f sec @ 1000 ticks
//  or 0.0020f sec @ 500 ticks
#define STSAMPLEVOICE_TICKSPERSEC YCF 1000.0f

// in sec., determines e.g. envelope resolution (0.001= 1 millisec)
#define STSAMPLEVOICE_BLOCK_LEN YCF (1.0f/STSAMPLEVOICE_TICKSPERSEC)

// Number of samples to fade in/out when switching loop cycles
//  higher values=increased latency but lower freq. clicks
// (todo) should this be dependent on the mixrate (256 = tuned for 44.1kHz)
#define STSAMPLEVOICE_VOLRAMP_NUMSTEPS 256

#define STSAMPLEVOICE_LOOPRESTART_NUMSTEPS 64

#define STSAMPLEPLAYER_PERFCTL_PITCHBEND        YCI  0
#define STSAMPLEPLAYER_PERFCTL_PRESSURE         YCI  1
#define STSAMPLEPLAYER_PERFCTL_CC1_MODWHEEL     YCI  2
#define STSAMPLEPLAYER_PERFCTL_CC2_BREATHCTL    YCI  3
#define STSAMPLEPLAYER_PERFCTL_CC4_FOOTCTL      YCI  4
#define STSAMPLEPLAYER_PERFCTL_CC7_VOLUME       YCI  5
#define STSAMPLEPLAYER_PERFCTL_CC8_BALANCE      YCI  6
#define STSAMPLEPLAYER_PERFCTL_CC10_PAN         YCI  7
#define STSAMPLEPLAYER_PERFCTL_CC11_EXPRESSION  YCI  8
#define STSAMPLEPLAYER_PERFCTL_CC16_GENERAL_1   YCI  9
#define STSAMPLEPLAYER_PERFCTL_CC17_GENERAL_2   YCI 10
#define STSAMPLEPLAYER_PERFCTL_CC18_GENERAL_3   YCI 11
#define STSAMPLEPLAYER_PERFCTL_CC19_GENERAL_4   YCI 12
#define STSAMPLEPLAYER_PERFCTL_CC74_BRIGHTNESS  YCI 13
#define STSAMPLEPLAYER_PERFCTL_CC80_GENERAL_5   YCI 14
#define STSAMPLEPLAYER_PERFCTL_CC81_GENERAL_6   YCI 15
#define STSAMPLEPLAYER_PERFCTL_CC82_GENERAL_7   YCI 16
#define STSAMPLEPLAYER_PERFCTL_CC83_GENERAL_8   YCI 17
#define STSAMPLEPLAYER_NUM_PERFCTL              YCI 18

  public:
   StSample       *sample;        // ref, currently playing sample
   StSamplePlayer *sample_player; // set after voice allocation in StSamplePlayer

   // ref, used to convert MIDI note to Hz.
   //  may be overridden by tuning_tables[].
   //  NULL=use sample_player->default_freq_table (fallback)
   const sF32 *freq_table;
   sF32        interpolated_freq_table[128];  // STSAMPLE_MM_DST_TUNING_TABLE_REL

   // Same format as sample->sample_loops but overrides these
   //  (originally added for Synergy replay)
   //  nowadays [26Feb2020] used for liverec loopdetect sustain mode
   YAC_IntArray *override_sample_loops;

#define STSAMPLEVOICE_OVERRIDE_SMPDAT_SZ       (128*1024)  // #frames. must match STSAMPLE_LIVEREC_FRONT_BUFFER_SZ
#define STSAMPLEVOICE_OVERRIDE_SMPDAT_SZ_MASK  (0x1FFFFu)
   sF32 *override_smpdat;
   sUI   override_smpdat_sz;
   sBool b_override_smpdat_copied;
   sSI   liverec_copy_loop_offset;
   sSI   liverec_copy_loop_len;
   sUI   liverec_copy_doublebuffer_rec_idx;
   sF32  liverec_osc_pitch_factor;  // in osc mode, compensate detected loop cycle pitch change

   tksampler_lfsr_t lfsr;

   sBool b_used;       // 1=voice is currently active, 0=voice may still be retriggered but is free for allocation
   sBool b_playing;    // 1:voice is active and initialized/playing, 0=voice -may- be active or unused
   sBool b_trig_pulse; // 1 for 1 tick when voice is started (also in glide mode)
   sBool b_release;    // 1=voice is in release phase (after note-off)
   sBool b_release_pulse;  // 1 for 1 tick when release phase (note off) is started
   sBool b_alloc;      // 1=voice has just been allocated (valid for current tick)
   sBool b_glide;      // 1=voice has been allocated via allocSampleVoiceGlide() (=> apply retrig flags in initStartedVoicesByKey())
   sBool b_allow_smpoff;  // [03Nov2024] workaround for glide+smpoff modmatrix mod (becomes 0 when note is released)

   sUI voice_key;       // used to identify voices after (samplebank-) noteon. >0: voice is active. assigned in prepareToPlay()
   sUI last_voice_key;  // valid after noteoff/stop. used by findLastStartedVoiceBySample()
   sUI voice_idx;       // index into StSamplePlayer voices array
   sUI replay_ticks;    // replay time, number of blocks (default block size= 1 millisec)
   sUI voice_alloc_idx; // see StSamplePlayer::next_voice_alloc_idx

   sF32 float_block_off;      // subprecision correction factor
   sF32 float_block_size;     // #frames, depends on mixrate and STSAMPLEVOICE_BLOCK_LEN
   sUI  current_block_size;   // #frames, depends on mixrate and STSAMPLEVOICE_BLOCK_LEN
   sUI  current_block_offset; // #frames

   tksampler_mmdst_t mmdst;

   sF64 play_offset_orig_speed;  // monotonically increasing (never resets)
   sF64 play_offset_actual;      //
   sF32 mmdst_sync_speed;        // 0=off, 1=fastest
   sF32 mmdst_sync_slew;         // 1.0=default
   sF64 sync_factor_avg;

   sSI  current_delay_countdown; // #frames
   sF64 current_play_offset; // 0..current_sample_len
   sF32 last_relsmpoff; // used to calculate current_play_offset, see mod_sampleoff
   sUI  current_sample_offset;
   sUI  current_sample_len;  // calc'd each frame, modulated by mod_cyclelen if granular mode is disabled
   sUI  current_orig_sample_len;
   sSI  current_loop_idx;
   sSI  current_loop_countdown; // 0=inf
   sUI  current_clipend_off;  // in "clip sampleend to loopend" mode. 0=initial (no clipping)
   sUI  current_smpdat_shift;   // in frames. used for clipping crossfade start when smpDat was shifted.
   sSI samples_until_end_of_loopstep; // if loopms=1
   sBool b_timedloop; // sample->b_timedloop + sample_len must be 0 + sample_loops must be available
                      //  (note) this is reset to 0 to make handleEndOfLoop() correctly handle endless repeat
   sF32 timedloop_base; // multiply loop repeatcount by 1sec/timedloop_base. requires b_loopms=1. Default is 1000.0f (=> 1millisec)
   sBool b_queued_delayed_loop;
   sF64 coff_clang_workaround;

   sSI fadein_countdown; // used if b_timedloop=1, fadein new loop
   sSI fadeout_countdown; // used if b_timedloop=1, fadeout current loop
   sBool b_zero;
   sBool b_fading_out;
   sBool b_restartfadein; // true after voice restart to avoid clicks

   sSI  num_sample_loops;

   sF32 mod_timestretch;
   sF32 mmdst_timestretch;
   sF32 mod_timestretch_bend;
   sF32 mmdst_timestretch_bend;  // added to sample->timestretch_bend
   sF32 current_ts_offset;
   sF32 mod_sampleoff;      // offset, 0..1 (==samplelen)
   sF32 mod_sampleoff2;     // offset from randomization and velocity (0..1)
   sF32 mmdst_sampleoff;    // from modmatrix
   sF32 mod_cyclelen;
   sF32 mmdst_cyclelen;     // from modmatrix
   sF32 mmdst_cyclelen_eol; // from modmatrix
   sBool b_mmdst_cyclelen_eol_valid;
   sF32 queued_cyclelen_eol;
   sF32 cur_cyclelen_eol;
   sF32 mmdst_cyclelen_sampleoff;
   sF32 mod_sampleshift;    // additional offset, 0..(orig_samplelen-sampleoff)
   sF32 mmdst_sampleshift;  // from modmatrix
   sF32 mod_sampleshift_endofloop;    // copied to mod_sampleshift at end of loop. autoresets to -1.
   sF32 mmdst_sampleshift_endofloop_last;  // -1 initially, used to check if sampleshift_eol has really changed
   sF32 mod_altvolume;      // alternative volume
   sF32 mmdst_loop_repeats_scale;
   sF32 wt2d_x;    // 0..1 => 0..2DWidth-1  valid when b_queued_wt2d
   sF32 wt2d_y;    // 0..1 => 0..2DHeight-1
   sF32 mod_sampleoff_wt2d;
   sBool b_queued_wt2d;

   sUI  samples_since_cycle;
   sF32 fragment_interpol;

   sF32 mod_vol;
   sF32 mod_vol2;    // volume randomization (see StSampleBank::volume_rand_amount)
   sF32 current_vol;
   sF32 next_vol;
   sF32 cached_vel_vol; // velocity amount

   sF32 mod_pan;
   sF32 mod_pan2;    // pan randomization (see StSampleBank::pan_rand_amount)
   sF32 current_pan;
   sF32 next_pan;

   sF32 mod_freq;   // e.g. pitch bend
   sF32 mod_freq2;  // pitch randomization (see StSampleBank::freq_rand_amount)
   sF32 current_freq; // offset to "note", in linear "MIDI-note" space
   sF32 next_freq; // offset to "note", in linear "MIDI-note" space

   sF32 mod_mod;

   sF32 perfctl_poly_pressure;  // -1=use sampleplayer perfctl (channel pressure), 0..127 otherwise
   sF32 perf_ctl_on [STSAMPLEPLAYER_NUM_PERFCTL];  // copied from SamplePlayer @note-on (for STSAMPLE_MM_SRC_*_ON)
   sF32 perf_ctl_off[STSAMPLEPLAYER_NUM_PERFCTL];  // copied from SamplePlayer @note-off (for b_perfctl_freeze_noteoff mode)
   sF32 perfctl_pressure_max;

   sF32 mix_rate; // in Hz
   sSI  note; // in "MIDI-note" space (0..127) (set in reallyStartVoice())
   sF32 release_velocity;

   sUI  smpoff_mask;
   sF32 mmdst_srr;  // sample rate reduction (mask)

   sU16 br_mask;
   sF32 mmdst_br;  // bit reduction (16bit mask)

   StSampleVoiceNoteOnParams queued_noteon;
   sSI  volramp_fadeout_countdown;
   sF32 volramp_fade_vol;
   sSI  volramp_fadein_countdown;

   sUI  anticlick_fadeout_countdown;
   sUI  anticlick_granular_smpoffinterpol_countdown;
   sUI  anticlick_granular_smpoffinterpol_winoff;      // last winOff
   sUI  anticlick_granular_smpoffinterpol_nextwinoff;  // last winOffNext
   sF32 anticlick_granular_smpoffinterpol_tsoff;       // last current_ts_off
   sF32 anticlick_granular_smpoffinterpol_fragoff;     // last fragment_interpol
   sF32 last_sample[2];  // 0=left, 1=right
   sF32 last_sample_sampleshift_eol_l;
   sF32 last_sample_sampleshift_eol_r;
   sUI  sampleshift_eol_xfade_frames_left;
#ifdef USE_SAMPLESHIFT_EOL_XFADE_BUF
   sF32 sampleshift_eol_xfade_buf_l[STSAMPLEVOICE_VOLRAMP_NUMSTEPS];
   sF32 sampleshift_eol_xfade_buf_r[STSAMPLEVOICE_VOLRAMP_NUMSTEPS];
   sUI  sampleshift_eol_xfade_loopstart_frameidx;  // 0..STSAMPLEVOICE_VOLRAMP_NUMSTEPS-1
#endif // USE_SAMPLESHIFT_EOL_XFADE_BUF

   sUI looprestart_xfade_frames_left;
   sF32 last_sample_looprestart_l;
   sF32 last_sample_looprestart_r;

   StLFOPlayer lfo_freq;
   StLFOPlayer lfo_vol;
   StLFOPlayer lfo_pan;
   StLFOPlayer lfo_aux;

   sF32 mod_lfo_freq_amt;
   sF32 mod_lfo_vol_amt;
   sF32 mod_lfo_pan_amt;
   sF32 mod_lfo_aux_flt_amt;

   StADSRPlayer adsr_freq;
   StADSRPlayer adsr_vol;
   StADSRPlayer adsr_pan;
   StADSRPlayer adsr_aux;

   StModSeqPlayer modseq[STSAMPLE_NUM_MODSEQ];
   sUI modseq_patch[STSAMPLE_NUM_MODSEQ];  // 0..(STSAMPLE_MAX_MODSEQ_PATCHES-1).

   sUI  loop_vars[16];
   sSI  mod_jumptoloop; // -1 = no jump. auto-resets to -1.

   sUI wavepath_idx;        // current cycleindex (or index into wavepath_table, if available)
   sUI next_wavepath_idx;   // next cycleindex (or index into wavepath_table, if available)
   sUI queued_wavepath_idx; // queued cycleindex (or index into wavepath_table, if available)
   sUI wavepath_next_play_offset;

   sF32 current_aux_adsr;
   sF32 next_aux_adsr;

   sF32 current_aux_lfo;
   sF32 next_aux_lfo;

   sSI  filter_type;  // copied during note on
   sF32 mod_filter_cutoff;
   sF32 mod_filter_pan;
   sF32 mod_filter_offset;
   sF32 mod_filter_resonance;

   sBool mod_glide_switch;
   sF32  mod_glide_speed;
   sF32  mmdst_glide_speed;

   // (note) LPF cutoff calculation seems off (maxfreq cuts off too many highs)
   StSampleVoiceBiquad biquad_a_1[2/*channels*/];
   StSampleVoiceBiquad biquad_a_2[2/*channels*/];   // for dual-filter configuration (LP+HP)

   StSampleVoiceBiquad2 biquad_b_1[2/*channels*/];
   StSampleVoiceBiquad2 biquad_b_2[2/*channels*/];   // for dual-filter configuration (LP+HP)

   StSampleVoiceBiquad2 biquad_ai[STSAMPLEVOICE_AI_MAX_POLES];  // anti-alias/imaging filter (LP)
   sF32 ai_fc;
   sUI ai_num_poles;

   sF32 glide_src_note;  // modified in STSAMPLE_GLIDE_FREQ mode, constant in TIME mode
   sF32 glide_src_note_orig;
   sF32 glide_time;      // 0..1, increases @1000Hz rate in STSAMPLE_GLIDE_TIME mode
#define GLIDE_NOTE_HISTORY_SZ 16
   sSI  glide_note_history[GLIDE_NOTE_HISTORY_SZ];  // -1=none. set to prev 'note' when allocating/re-using gliding voice
   sSI  glide_note_history_idx;  // initially -1, 0=first ..GLIDE_NOTE_HISTORY_SZ-1 (LIFO)

   sF32 noteon_rand_pm[4];
   sF32 noteon_rand_p[4];

   sUI active_note_idx;
   sUI layer_idx;  // nth samplebank zone selected for playing (0..n)
   sUI num_layers; // number of sample zones selected for playing (valid for first voice in a layer group)

   sF32  mmdst_var;  // 0..1 => 0..STSAMPLE_MMVAR_NUM. targeted by STSAMPLE_MM_DST_VARIATION
   sUI   mmvar_last_seen_var_idx;  // UI / editor

#define STSAMPLEVOICE_NUM_REGS  YCI 4
   sF32 mmdst_reg[STSAMPLEVOICE_NUM_REGS];  // Reset to 0 at note on. e.g. for slew
   sF32 mmdst_cc;  // condition code. 0=equal, 1=greater, -1=less than or equal

   sF32 modmatrix_amt[STSAMPLE_NUM_MODMATRIX_ENTRIES];

   sBool b_fwd;  // for PINGPONG play mode

   // windowed sinc tables
#define LANCZOS_TSZ_A2   4096
#define LANCZOS_TSZ_A4   8192
#define LANCZOS_TSZ_A8  16384
#define LANCZOS_TSZ_A16 32768
#define LANCZOS_TSZ_A32 65536
   static sF32 lanczos_tbl_a2 [LANCZOS_TSZ_A2  + 1];
   static sF32 lanczos_tbl_a4 [LANCZOS_TSZ_A4  + 1];
   static sF32 lanczos_tbl_a8 [LANCZOS_TSZ_A8  + 1];
   static sF32 lanczos_tbl_a16[LANCZOS_TSZ_A16 + 1];
   static sF32 lanczos_tbl_a32[LANCZOS_TSZ_A32 + 1];

   st_plugin_cache_entry_t *plugin_cache_entries[STSAMPLE_NUM_PLUGINS];
   sF32 plugin_levels[STSAMPLE_NUM_PLUGINS][2];  // valid after mod matrix

   sF32   voice_bus_buffer_self[STSAMPLEVOICE_MAX_VOICE_BUS_FRAMES * 2u];
   sF32  *voice_bus_buffers_self[STSAMPLEVOICE_MAX_LAYERS];  // ptrs to interleaved stereo buffers (voice_bus_buffer_self or null_buf)
   sF32 **voice_bus_buffers;         // ref to first layer voice_bus_buffers_self
   StSampleVoice *voice_bus_first_voice;
   StSampleVoice *voice_bus_voices[STSAMPLEVOICE_MAX_LAYERS];  // valid for first layer voice. range is 0..first_voice->num_layers-1
   sUI render_buf_sample_offset;  // current offset in StSamplePlayer render buffers (used for calculating voice bus buffer addresses)
   sBool b_sort_done; // see StSamplePlayer::sortVoices()

   sBool b_allow_xfade; // false during attack section of "from start" infinite loop

   // for additive/resynth wavetable mode, even and odd
   sU16 partial_phases[256 * 256];  // one set per wave  (todo) alloc dynamically ?
   sU16 partial_speeds[256 * 2];
   sF32 partial_phase_offset;  // 0..cycle_len
   /* sF32 slew_partial_phases[256]; */
   sF32 slew_partial_speeds[256];
   sF32 slew_partial_magnitudes[256];
   sBool b_slew_partial_first;  // after voice start, fill slew arrays with initial wave (to avoid "fade in")
   sU16 fade_partial_phases[256 * 2];  // for crossfading back to cycle reset
   sF32 fade_partial_amt;              // 0=no fading, 1..0 while fading

   // for additive/resynth wavetable mode
   static sF32 additive_sin_tbl[4096];

   sF32  mmdst_additive_cfg;
   sF32  last_additive_cfg;  // for checking whether additive_tbl_mix needs to be updated. -1 initially.
   sF32 *additive_tbl;  // initially null. allocated when using per-voice additive wt osc modulation.
   sF32  mmdst_additive_stereo_spread;
   sF32  mmdst_additive_num_partials;

   // optimization hint (no sample playback, (osc) voice plugins only, e.g. FM Stack)
   //  updated in reallyStartVoice()
   sBool b_voiceplugin_osc_hint;

   sF64 modmatrix_slewed_src_values[STSAMPLE_NUM_MODMATRIX_ENTRIES];

  public:
   static void calc_mm_curve (sF32 _c, sUI _off);
   static sF32 mm_curve_lut[32*32*256];

   static sF32 FracSmooth (sF32 _s, sU8 _order);

  public:
   // Get current interpolated glide note
   sF32 getCurrentGlideNote (void);

   static void InitLanczosTables (void);
   static void InitAdditiveTables (void);
   static void InitMMCurveLUT (void);

  public:
   void clearGlideNoteHistory (void);
   void addToGlideNoteHistory (sSI _note);
   sBool removeFromGlideNoteHistory (sSI _note, sBool _bLastOnly);

  protected:
   sBool isInstantGlide (void) const;
   void tickGlide (void);  // called in handleNextBlock()

   // allow modmatrix override
   const sF32 *getCurrentFreqTableMM (void) const;

   // return forced or default table (ignoring modmatrix)
   const sF32 *getCurrentFreqTable (void) const;

   void filter (sF32 *_smp);

   void calcModMatrix (tksampler_mmdst_t &mmdst);

   void lazyAllocOverrideSmpDat (sUI _sz);

  public:
   static sF32 BitReduce (sF32 f, const sF32 _brPreAmp, const sU16 _brMask);

   static sF32 sum_partials (const sF32 *_tbl, sU16 *_phases, const sU16 *_speeds, const sF32 *_amps,
                             const sUI _numPartials, const sF32 _lastPartialAmt
                             );

   static sF32 sum_partials_srr (const sF32 *_tbl, sU16 *_phases, const sU16 *_speeds, const sF32 *_amps,
                                 const sUI _numPartials, const sF32 _lastPartialAmt,
                                 const sUI _srMask
                                 );

   static sF32 sum_partials_srr_br (const sF32 *_tbl, sU16 *_phases, const sU16 *_speeds, const sF32 *_amps,
                                    const sUI _numPartials, const sF32 _lastPartialAmt,
                                    const sUI _srMask, const sF32 _brPreAmp, const sU16 _brMask
                                    );

   static void sum_partials_stereo (const sF32 *_tbl,
                                    sU16 *_phases, const sU16 *_speeds, const sF32 *_amps,
                                    const sUI _numPartials, const sF32 _lastPartialAmt,
                                    const sF32 *_vcalPan, const sF32 _stereoSpread,
                                    sF32 &retL, sF32 &retR
                                    );

   static void sum_partials_stereo_srr_br (const sF32 *_tbl,  sU16 *_phases, const sU16 *_speeds, const sF32 *_amps,
                                           const sUI _numPartials, const sF32 _lastPartialAmt,
                                           const sF32 *_vcalPan, const sF32 _stereoSpread,
                                           const sUI _srMask, const sF32 _brPreAmp, const sU16 _brMask,
                                           sF32 &retL, sF32 &retR
                                           );

  protected:

   void updatePluginModulationInitial (const sUI _pluginMask);
   void updatePluginModulationFromMM (tksampler_mmdst_t &mmdst, const sUI _pluginMask);

   // dst=VM1..8. render single frame (for STSAMPLE_MM_SRC_VOICE_BUS_*) and clear the remaining bus frames
   void processPluginsVM (const sUI _numFrames);

   // dst=SR
   sF32 processPluginsSR (const sF32 _cRate);

   // dst=VB1..8
   void processPluginsVB (const sUI _numFrames);

   // dst=def, fixed 1*2*3*4 alg
   void processPluginsDef (sBool  _bPreFilter,
                           sBool  _bMonoIn
                           );

   // dst=AM/VB1..8
   void processPluginsAM (sF32 *_pluginAmp);

   void updateWt2dOffset (void);

   void renderBlockAdditive (sF32 *     buf,
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
                             );

   void renderBlockTimestretch (sF32 *     buf,
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
                                );

   sUI  renderBlockNormal (sF32 *     buf,
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
                           );

   sUI renderFragmentOptFwdMonoSimple (sF32 *&    buf,
                                       const sUI  _fragSize,
                                       sUI &      genSizeHint,
                                       sF32 *&    smpDat,
                                       sF32 &     cVolL,
                                       sF32 &     cVolR,
                                       sF32 &     cRate,
                                       const sF32**_inputsOrNull/*sz=STSAMPLE_MAX_INPUTS*/
                                       );

   sUI renderFragmentGeneric (sF32 *&    buf,
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
                              );

   // - infinite sample loop <= 32 frames
   // - no volume ramp up or down
   // - no filter
   // - 1*2*3*4 alg
   // - all plugins routed to 'def'
   sUI renderFragmentOptVoicePluginOsc (sF32 *&    buf,
                                        const sUI  _fragSize,
                                        sF32 &     cVolL,
                                        sF32 &     cVolR,
                                        const sF32 sVolL,
                                        const sF32 sVolR
                                        );

   sF32 current_cyclelen; // calc'd at block start
   void calcCurrentCycleLen (sF32 &ts);

   void calcCurrentSampleLen (void);
   void alignSampleLenToCyclelen (sUI &_sampleLen);
   void calcCurrentOffset (void);

   void liveRecCopyToOverride (sF32 *_smpDatBase, sUI _waveOff);
   void liveRecResampleToOverride (sF32 *_smpDatBase, sUI _waveOff);

   void calcSmpDat(sF32*&_smpDat,
                   sF32*&_smpDatLRX,
                   const sUI _curSampleOffset,
                   const sUI _curOrigSampleLen,
                   const sUI _curSampleLen
                   );

   void reallyStartVoice(const StSampleVoiceNoteOnParams *_params,
                         sBool _bDelayed
                         );

   sUI handleNextBlock(void);

   void startADSRAndLFO(sUI _mask, sBool _bNoteOn);
   void startModSeq(sUI _mask, sBool _bNoteOn);

   void calcFragmentInterpolPhase(sF32 _phase, sF32 _width);
   sF32 getEffectiveMod(void); // voice+sampleplayer, clipped to 0..1 range
   void applyFragmentInterpol(void);

   YAC_IntArray *getCurrentSampleLoops(void) const;

   void recalcAdditiveTblMix (void);

  public:
   StSampleVoice(void);
   ~StSampleVoice();

   YAC(StSampleVoice);

   void setSamplePlayer (StSamplePlayer *_samplePlayer);

   sBool isAllocated (void);
   sBool canAllocate (void);
   sBool isSoftStopped (void);
   sBool isFadedOut (void);

   void startVoiceInt (StSample *_sample,
                       const sF32 *_freqTable,
                       sF32 _mixRate,
                       sSI _note, sF32 _vel,
                       sF32 _mod, sF32 _vol, sF32 _pan, sF32 _freq
                       );

   void initStartedVoice (void);

   void prepareToPlay (StSample *_sample, sSI _voiceKey, sUI _voiceAllocIdx); // called after voicealloc

   sUI renderBlock (sF32 *buf, sUI blkSz, sF32 a, sF32 b, sF32 _volScale, const sF32**_inputsOrNull/*sz=STSAMPLE_MAX_INPUTS*/);

   void handleEndOfLoopOver (const sF32 *_smpDatCur, sUI _frameOffOver, const sF32 **_smpDatNew, sUI *_frameOffNew) const;
   sBool handleEndOfLoop (sF64 *cOff, sF32**smpDat, sF32**smpDatLRX, sBool _bFromStart, sBool _bAllowCOffReset);

   void calcTSWindowSin (sUI _len);
   void calcTSWindowTri (sUI _len);

   void readWindowedCycleSample (const sF32 *smpDat, const sF32 *smpDatLRX, sUI _off, sF32 _tsOff, sF32 *_l, sF32 *_r, const sUI _curSampleLen);
   void readWindowedSample (const sF32 *smpDat, const sF32 *smpDatLRX, sF32 _off, sF32 *_l, sF32 *_r, sBool _bAllowInterpol, const sUI _curSampleLen);

   sF32 readSample1IntAI (const sF32 *smpDat, sSI _off) /*const*/;  // mono
   sF32 readSample1IntRaw (const sF32 *smpDat, sSI _off) /*const*/;  // mono
   sF32 readSample1IntWrap (const sF32 *smpDat, sSI _off) const;  // mono

   sF32 readSample2IntAI (const sF32 *smpDat, sSI _off) /*const*/;  // stereo
   sF32 readSample2IntRaw (const sF32 *smpDat, sSI _off) /*const*/;  // stereo
   sF32 readSample2IntWrap (const sF32 *smpDat, sSI _off) const;  // stereo

   sF32 readSample1LRX(const sF32 *smpDat, const sF32 *smpDatLRX, sSI _off) /*const*/;  // mono
   sF32 readSample2LRX(const sF32 *smpDat, const sF32 *smpDatLRX, sSI _off) /*const*/;  // stereo

   sF32 readSample1(const sF32 *smpDat, const sF32 *smpDatLRX, sSI _off) /*const*/;  // mono
   sF32 readSample2(const sF32 *smpDat, const sF32 *smpDatLRX, sSI _off) /*const*/;  // stereo

   void handleReorderVoicePlugins (const sUI *_ia);

   void resetBiquad (void);

   sF32 FreqToNote (sF32 _freq, const sF32 *_freqTable);


  public:

   // Immediately stop playing
   YM void _resetVoice (void);

   /* @method getVoiceIndex:int
    Query index of sample voice in sample player voice array
   */
   YM sUI getVoiceIndex (void);


   /* @method init,StSample sample,FloatArray freqTable,float mixRate,int note
      Initialize sample voice.

      @arg sample - Which sample to play
      @arg freqTable - Used to convert MIDI note number to frequency (Hz). At least 128 entries.
      @arg note - Which MIDI note to play (fraction is used for detune)
      @arg mod - Initial modulation
      @arg vol - Initial volume modulation
      @arg pan - Initial pan modulation
      @arg freq - Initial frequency modulation
   */
   YM void _startVoice (YAC_Object *_sample, YAC_Object *_freqTable, sF32 _mixRate, sSI _note, sF32 _vel, sF32 _mod, sF32 _vol, sF32 _pan, sF32 _freq);

   /* @method restartVoice
      Reset currently playing voice
   */
   YM void _restartVoice (void);


   /* @method stopVoice
      Mark voice inactive (no fadeout)
   */
   YM void stopVoice (void);


   /* @method softStopVoice
      Fadeout voice and stop it
   */
   YM void softStopVoice (void);


   /* @method noteOff,float velocity
      Start release phase

      If there is no release envelope, the voice is stopped immediately

      @arg velocity Note-off velocity (0..1)
   */
   YM void noteOff (sF32 _velocity);


   /* @method noteToFreq,float note:float
      Map MIDI note to frequency (Hz) using currently set freq_table.

      @arg note MIDI note number (fraction for detune)
      @return Frequency (Hz)
   */
   YM sF32 noteToFreq (sF32 _note);

   sF32 noteToSampleRate (sF32 _note);
   sF32 freqToSampleRate (sF32 _hz);

   // convert normalized cutoff note (0..1) to linear freq (0..1 => 0..max_note_hz) via freq_table
   sF32 normToCutOffFreq (sF32 _norm);

   /* sF32 calcLiveRecOscTuningFactor (void); */

   void calcNextBlockState (sBool _bFirst);


   /* @method setVol,float vol
      Set volume modulation
   */
   YM void _setVol (sF32 _vol);


   /* @method getVol:float
      Return volume modulation
   */
   YM sF32 _getVol (void);


   /* @method setVol2,float vol
      Set volume modulation 2 (randomization)
   */
   YM void _setVol2 (sF32 _vol);


   /* @method getVol2:float
      Return volume modulation (randomization)
   */
   YM sF32 _getVol2 (void);


   /* @method setPan,float pan
      Set panning modulation
   */
   YM void _setPan (sF32 _pan);


   /* @method getPan:float
      Return panning modulation
   */
   YM sF32 _getPan (void);


   /* @method setPan2,float pan
      Set panning modulation (randomization)
   */
   YM void _setPan2 (sF32 _pan);


   /* @method getPan2:float
      Return panning modulation (randomization)
   */
   YM sF32 _getPan2 (void);


   /* @method setFreq,float freq
      Set frequency modulation
   */
   YM void _setFreq (sF32 _freq);


   /* @method getFreq:float
      Return frequency modulation
   */
   YM sF32 _getFreq (void);


   /* @method setFreq2,float freq
      Set frequency modulation 2 (additive, randomization)
   */
   YM void _setFreq2 (sF32 _freq);


   /* @method getFreq2:float
      Return frequency modulation
   */
   YM sF32 _getFreq2 (void);


   /* @method setMod,float mod
      Set modulation

      This only has an effect when there is a mod range defined
   */
   YM void _setMod (sF32 _mod);


   /* @method getMod:float
      Return modulation
   */
   YM sF32 _getMod (void);


   /* @method setTimestretch,float ts
      Set timestretch modulation

      This only has an effect when timestretching is enabled
   */
   YM void _setTimestretch (sF32 _ts);


   /* @method getTimestretch:float
      Return timestretch modulation
   */
   YM sF32 _getTimestretch (void);


   /* @method setTimestretchBend,float ts
      Set timestretch phase bend modulation

      This only has an effect when timestretching is enabled
   */
   YM void _setTimestretchBend (sF32 _ts);


   /* @method getTimestretchBend:float
      Return timestretch phase bend modulation
   */
   YM sF32 _getTimestretchBend (void);


   /* @method setSampleOff,float so
      Set sample offset modulation

   */
   YM void _setSampleOff (sF32 _so);
   YM void _setSampleOffMs (sF32 _ms);


   /* @method getSampleOff:float
      Return sample offset modulation
   */
   YM sF32 _getSampleOff (void);


   /* @method setSampleOff2,float so
      Set sample offset modulation (randomization)

   */
   YM void _setSampleOff2 (sF32 _so);


   /* @method getSampleOff2:float
      Return sample offset modulation (randomization)
   */
   YM sF32 _getSampleOff2 (void);


   /* @method setCycleLen,float so
      Set granular cycle length modulation

   */
   YM void _setCycleLen (sF32 _cl);


   /* @method getCycleLen:float
      Return sample cycle length modulation
   */
   YM sF32 _getCycleLen (void);


   /* @method setSampleShift,float sh
      Set sample shift modulation

   */
   YM void _setSampleShift (sF32 _sh);


   /* @method getSampleShift:float
      Return sample shift modulation
   */
   YM sF32 _getSampleShift (void);


   /* @method setSampleShiftEndOfLoop,float sh
      Set sample shift modulation (queued until end of loop)

   */
   YM void _setSampleShiftEndOfLoop (sF32 _sh);


   /* @method getSampleShiftEndOfLoop:float
      Return sample shift modulation (queued until end of loop)
   */
   YM sF32 _getSampleShiftEndOfLoop (void);


   /* @method setAltVolume,float av
      Set alternative volume modulation

   */
   YM void _setAltVolume (sF32 _av);


   /* @method getAltVolume:float
      Return alternative volume modulation
   */
   YM sF32 _getAltVolume (void);


   /* @method retrigEnvLFO,int mask
      Retrigger envelopes/LFOs

   */
   YM void _setRetrigMask (sUI _mask);


   /* @method setTimedLoopBase,float base
      Set timedloop repeat frequency (1sec/base)

      This only has an effect when the sample uses timed loops.
    */
   YM void _setTimedLoopBase (sF32 _base);


   /* @method getTimedLoopBase:float
    */
   YM sF32 _getTimedLoopBase (void);


   /* @method setLFOFreqSpd,float spd
      Set frequency LFO speed modifier
    */
   YM void _setLFOFreqSpd (sF32 _spd);


   /* @method setLFOFreqLvl,float lvl
      Set frequency LFO level modifier
    */
   YM void _setLFOFreqLvl (sF32 _lvl);


   /* @method setLFOFreqAmt,float amt
      Set frequency LFO => frequency amount modifier
    */
   YM void _setLFOFreqAmt(sF32 _amt);


   /* @method setLFOVolSpd,float spd
      Set volume LFO speed modifier
    */
   YM void _setLFOVolSpd (sF32 _spd);


   /* @method setLFOVolLvl,float lvl
      Set volume LFO level modifier
    */
   YM void _setLFOVolLvl (sF32 _lvl);


   /* @method setLFOVolAmt,float amt
      Set volume LFO => volume amount modifier
    */
   YM void _setLFOVolAmt(sF32 _amt);


   /* @method setLFOPanSpd,float spd
      Set pan LFO speed modifier
    */
   YM void _setLFOPanSpd (sF32 _spd);


   /* @method setLFOPanLvl,float lvl
      Set pan LFO level modifier
    */
   YM void _setLFOPanLvl (sF32 _lvl);


   /* @method setLFOPanAmt,float amt
      Set pan LFO => pan amount modifier
    */
   YM void _setLFOPanAmt(sF32 _amt);


   /* @method setLFOAuxSpd,float spd
      Set aux LFO speed modifier
    */
   YM void _setLFOAuxSpd (sF32 _spd);


   /* @method setLFOAuxLvl,float lvl
      Set aux LFO level modifier
    */
   YM void _setLFOAuxLvl (sF32 _lvl);


   /* @method setLFOAuxFltAmt,float amt
      Set aux LFO => flt amount modifier
    */
   YM void _setLFOAuxFltAmt(sF32 _amt);


   /* @method setADSRFreqSpd,float spd
      Set frequency ADSR speed modifier
    */
   YM void _setADSRFreqSpd (sF32 _spd);


   /* @method setADSRFreqLvl,float lvl
      Set frequency ADSR level modifier
    */
   YM void _setADSRFreqLvl (sF32 _lvl);


   /* @method setADSRFreqInt,float int
      Set frequency ADSR intensity modifier
    */
   YM void _setADSRFreqInt (sF32 _int);


   /* @method setADSRVolSpd,float spd
      Set volume ADSR speed modifier
    */
   YM void _setADSRVolSpd (sF32 _spd);


   /* @method setADSRVolLvl,float lvl
      Set volume ADSR level modifier
    */
   YM void _setADSRVolLvl (sF32 _lvl);


   /* @method setADSRVolInt,float int
      Set volume ADSR intensity modifier
    */
   YM void _setADSRVolInt (sF32 _int);


   /* @method setADSRPanSpd,float spd
      Set pan ADSR speed modifier
    */
   YM void _setADSRPanSpd (sF32 _spd);


   /* @method setADSRPanLvl,float lvl
      Set pan ADSR level modifier
    */
   YM void _setADSRPanLvl (sF32 _lvl);


   /* @method setADSRPanInt,float int
      Set pan ADSR intensity modifier
    */
   YM void _setADSRPanInt (sF32 _int);


   /* @method setADSRAuxSpd,float spd
      Set aux ADSR speed modifier
    */
   YM void _setADSRAuxSpd (sF32 _spd);


   /* @method setADSRAuxLvl,float lvl
      Set aux ADSR level modifier
    */
   YM void _setADSRAuxLvl (sF32 _lvl);


   /* @method setADSRAuxInt,float int
      Set aux ADSR intensity modifier
    */
   YM void _setADSRAuxInt (sF32 _int);


   /* @method setAFreqSpd,float int
      Set freq attack speed modifier
    */
   YM void _setAFreqSpd (sF32 _spd);


   /* @method setSFreqSpd,float int
      Set freq sustain speed modifier
    */
   YM void _setSFreqSpd (sF32 _spd);


   /* @method setRFreqSpd,float int
      Set freq release speed modifier
    */
   YM void _setRFreqSpd (sF32 _spd);


   /* @method setAVolSpd,float int
      Set volume attack speed modifier
    */
   YM void _setAVolSpd (sF32 _spd);


   /* @method setSVolSpd,float int
      Set volume sustain speed modifier
    */
   YM void _setSVolSpd (sF32 _spd);


   /* @method setRVolSpd,float int
      Set volume release speed modifier
    */
   YM void _setRVolSpd (sF32 _spd);


   /* @method setAPanSpd,float int
      Set pan attack speed modifier
    */
   YM void _setAPanSpd (sF32 _spd);


   /* @method setSPanSpd,float int
      Set pan sustain speed modifier
    */
   YM void _setSPanSpd (sF32 _spd);


   /* @method setRPanSpd,float int
      Set pan release speed modifier
    */
   YM void _setRPanSpd (sF32 _spd);


   /* @method setAAuxSpd,float int
      Set aux attack speed modifier
    */
   YM void _setAAuxSpd (sF32 _spd);


   /* @method setSAuxSpd,float int
      Set aux sustain speed modifier
    */
   YM void _setSAuxSpd (sF32 _spd);


   /* @method setRAuxSpd,float int
      Set aux release speed modifier
    */
   YM void _setRAuxSpd (sF32 _spd);


   /* @method setJumpToLoop,int idx
      Queue jump to loop
    */
   YM void _setJumpToLoop (sSI _int);


   /* @method setJumpToLoopRel,float relIdx
      Queue jump to loop (0..1 => 0..num_loops-1)
    */
   YM void _setJumpToLoopRel (sF32 _relIdx);


   /* @method setWavepathIndex,int idx
      Queue next cycle
    */
   YM void _setWavepathIndex (sSI _int);

   /* @method setModSeqRetrigMask,int mask
    */
   YM void _setModSeqRetrigMask(sUI _mask);

   /* @method setModSeqSpeed,float speed
    */
   YM void _setModSeqSpeed(sUI _idx, sF32 _speed);

   /* @method setModSeqLevel,float level
    */
   YM void _setModSeqLevel(sUI _idx, sF32 _level);

   /* @method setModSeqNumStepsRel,float numStepsRel
    */
   YM void _setModSeqNumStepsRel(sUI _idx, sF32 _numStepsRel);

   /* @method setModSeqAdvance,float advance
    */
   YM void _setModSeqAdvance(sUI _idx, sF32 _advance);

   /* @method render,FloatArray buf
    */
   YM void _render (YAC_Object *_fa, sF32 _volScale); // "add" to stereo buffer
   void renderInt (YAC_Object *_fa, sF32 _volScale, const sF32**_inputsOrNull/*sz=STSAMPLE_MAX_INPUTS*/);


   /* @method PrintDebugStats
      Pseudo-static method that prints mixer codepath statistics (for debugging/optimization)
   */
   YM void _PrintDebugStats (void);

   // (todo) unused, remove ?
   YM sF32 _FreqToNote (sF32 _freq, YAC_Object *_freqTable);

   // used by liverec sustain mode
   YM void        _allocOverrideSampleLoops       (void);
   YM YAC_Object *_getOrCreateOverrideSampleLoops (void);
   YM YAC_Object *_getOverrideSampleLoops         (void);
   YM void        _freeOverrideSampleLoops        (void);

   YM sBool       _verifyOverrideSampleAreas      (void);

   YM sSI _liveRecGetOverrideLoopOffset (void);
   YM sSI _liveRecGetOverrideLoopLen (void);
   YM sSI _liveRecGetOverridePlayBufIdx (void);
   YM sUI _liveRecGetOverrideSmpDatSz (void);

   YM sBool copyOverrideBufferToWaveform (void);
   YM sBool copyOverrideBufferToFloatArray (YAC_Object *_d);


   /* @method setFilterCutOff,float f
      Set filter cutoff modifier
    */
   YM void _setFilterCutOff (sF32 _f);


   /* @method setFilterPan,float f
      Set filter stereo pan modifier
    */
   YM void _setFilterPan (sF32 _f);


   /* @method setFilterOffset,float f
      Set filter offset modifier
    */
   YM void _setFilterOffset (sF32 _f);


   /* @method setFilterResonance,float f
      Set filter resonance modifier
    */
   YM void _setFilterResonance (sF32 _f);


   /* @method setGlideSwitch,boolean bEnable
      Set glide switch
    */
   YM void _setGlideSwitch (sBool _bEnable);


   /* @method setGlideSpeed,float speed
      Set glide speed
    */
   YM void _setGlideSpeed (sF32 _speed);


   /* @method resetLFOFreqPhase
      Reset frequency LFO phase
   */
   YM void _resetLFOFreqPhase (void);


   /* @method resetLFOVolPhase
      Reset volume LFO phase
   */
   YM void _resetLFOVolPhase (void);


   /* @method resetLFOPanPhase
      Reset panning LFO phase
   */
   YM void _resetLFOPanPhase (void);


   /* @method resetLFOAuxPhase
      Reset aux LFO phase
   */
   YM void _resetLFOAuxPhase (void);


   /* @method updatePerfCtlPolyPressure,float value
      Update poly pressure (0..127)
    */
   YM void _updatePerfCtlPolyPressure (sF32 _value);


   /* @method mmVarGetIdx:int
      Returns currently playing mod matrix variation index (0..15)
   */
   YM sUI _mmVarGetLastSeenIdx (void);


   #include "autogen_stsamplevoice_alg_int/index.h"

};


#endif // ST_SAMPLEVOICE_H__
