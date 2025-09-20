// ----
// ---- file   : StSample.h
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
// ----          19Jan2010, 21Jan2010, 21Feb2010, 01Mar2010, 29Apr2010, 02May2010, 07Jul2010
// ----          18Jul2010, 04Sep2010, 05Mar2011, 10Mar2011, 14Mar2011, 20Mar2011, 28Mar2011
// ----          08May2013, 09May2013, 23Dec2018, 27Dec2018, 30Dec2018, 31Dec2018, 02Jan2019
// ----          04Jan2019, 07Jan2019, 08Jan2019, 09Jan2019, 10Jan2019, 30Mar2019, 31Mar2019
// ----          23Apr2019, 11May2019, 12May2019, 13May2019, 15May2019, 17May2019, 18Jun2019
// ----          13Jul2019, 21Jul2019, 25Jul2019, 29Nov2019, 15Feb2020, 16Feb2020, 17Feb2020
// ----          18Feb2020, 19Feb2020, 20Feb2020, 21Feb2020, 22Feb2020, 23Feb2020, 24Feb2020
// ----          25Feb2020, 26Feb2020, 27Feb2020, 04Mar2020, 17May2020, 18May2020, 19May2020
// ----          21May2020, 22May2020, 23May2020, 24May2020, 28May2020, 31May2020, 01Jun2020
// ----          04Jun2020, 05Jun2020, 06Jun2020, 08Jun2020, 10Jun2020, 23Aug2020, 03Sep2020
// ----          06Sep2020, 06Oct2020, 13Oct2020, 29Mar2021, 30Apr2021, 20May2021, 21May2021
// ----          27Jul2021, 10Aug2021, 30Aug2021, 04Oct2021, 05Oct2021, 20Oct2021, 22Oct2021
// ----          24Oct2021, 28Oct2021, 23Nov2022, 24Nov2022, 20Dec2022, 21Dec2022, 22Dec2022
// ----          23Dec2022, 27Dec2022, 28Dec2022, 30Dec2022, 31Dec2022, 09Mar2023, 10Mar2023
// ----          11Aug2023, 14Aug2023, 16Aug2023, 20Aug2023, 08Sep2023, 13Sep2023, 19Sep2023
// ----          22Sep2023, 17Nov2023, 18Nov2023, 08Jan2024, 10Jan2024, 13Jan2024, 14Jan2024
// ----          15Jan2024, 16Jan2024, 19Apr2024, 04Aug2024, 15Aug2024, 28Sep2024, 30Sep2024
// ----          02Oct2024, 03Oct2024, 13Oct2024, 09Nov2024, 03Jan2025
// ----
// ----
// ----

#ifndef ST_SAMPLE_H__
#define ST_SAMPLE_H__

YG("tksampler")


class StSampleBank;
class StSamplePlayer;


typedef struct StSampleMMEntry_s {
   sBool b_enable;      // 1=process modmatrix entry
   sU8   op;            // 0=add, 1=mul, .. (see STSAMPLE_MM_OP_xxx)
   sU8   slew_reset;    // 0=off, 1=0.0, 2=1.0  (see STSAMPLE_MM_SLEW_RESET_xxx)
   sF32  curve;         // <0=log, 0=lin, >0=exp
   sF32  slew_up;       // 0..1 (displayed as 0..9 in UI)
   sF32  slew_down;     // 0..1 (displayed as 0..9 in UI)
   sU16  src;           // see STSAMPLE_MM_SRC_xxx
   sF32  amt;           // arbitrary float
   sU16  dst;           // see STSAMPLE_MM_DST_xxx
   sBool b_signal_tap;  // 1=debug-print dest value to console. not persistent.
} StSampleMMEntry;

struct st_plugin_slot_t {
   StPluginShared        *shared;           // instance
   StPluginSharedMissing *shared_missing;   // place holder when plugin is missing
   sBool                  b_pre_filter;
   sBool                  b_skip;
   sF32                   level[2];         // l/r (calculated from drive+pan)
   sUI                    dst;              // see STSAMPLE_VOICEPLUGIN_DST_xxx
   sF32                   drive;            // 0..f  (1.0=no change)
   sF32                   pan;              // -1..1
   sBool                  b_invert_phase;
};


YC class StSample : public YAC_Object {

/* @constant STSAMPLE_DEFAULT_MAXVOICES,int
 */
#define STSAMPLE_DEFAULT_MAXVOICES YCI 8

#define STSAMPLE_DEFAULT_TIMEDLOOP_BASE 1000.0f

#define STSAMPLE_MAX_ALT_SAMPLES  (128u)

// Maximum number of tuning tables per sample zone / sample bank
#define STSAMPLE_MAX_TUNING_TABLES  YCI 16

  public:
   StSample           *next;
   StSampleBank       *parent_samplebank;
   YAC_String         *name;
   StWaveform         *waveform;
   sBool               b_own_waveform;
   sF32                sample_rate_ratio;
   StSampleMutexGroup *mutex_group;

   YAC_FloatArray     *partial_magnitudes;
   YAC_FloatArray     *partial_speeds;
   YAC_FloatArray     *partial_phases;
   sF32                num_partials;
   sF32                partial_cyclelen_reset;  // typically equals cyclelen but may differ. <8=free run
   sBool               b_partial_phase_0;

   StRange *key_range; // Owned by sample instance
   StRange *vel_range;
   StRange *mod_range;

   sBool b_skip_range;  // 1=ignore key/vel/mod ranges, 0=enable(when present).  used by editor when solo'ing zones.

   sUI max_voices; // max voices per SamplePlayer instance. 0=only apply samplebank voice limiter

   sBool b_enabled; // 1=sample is allowed to play, 0=sample muted (mainly for editor)

   sF32 sample_delay; // #sec
   sF32 sample_delay_multiplier; // factor

   sUI  sample_offset;
   sUI  sample_len;
   YAC_IntArray *sample_loops; // offset/len/times tuples, NULL=use waveform default loop
                               //  actually all unsigned values
                               //  times bit 31 indicates loop jump
                               //        bit 30 indicates a countdown check (branch until countdown reaches 0)
                               //        bit 29 indicates a countdown initializer (no branch)
                               //        bits 28..25 identify a countdown variable (0..15)
                               //        bit 24 indicates a var-copy/move (from VAL to VAR)
                               //        bits 23..16 are reserved for future extensions
                               //        bits 0..15 determine number of repeats, branch target or countdown value

   sSI first_loop_index;  // mainly for editor (solo loop entry)
   sBool ui_sync_first_loop_index;

   sSI noteoff_loop_index; // !=-1: jump to specified loop step when note key is released
   sBool b_noteoff_immediate_loop_jump; // 0=play until end of cycle, 1=start new loop immediately. default=0

   sBool b_jumptoloop_immediately;  // 1=jumptoloop modulation immediately selects new loop. 0=wait for current loop to finish
#define STSAMPLE_PLAY_MODE_FWDNOLOOP   YCI  0
#define STSAMPLE_PLAY_MODE_FORWARD     YCI  1
#define STSAMPLE_PLAY_MODE_BACKWARD    YCI  2
#define STSAMPLE_PLAY_MODE_PINGPONG    YCI  3
#define STSAMPLE_PLAY_MODE_XFADE_ALL   YCI  4
#define STSAMPLE_PLAY_MODE_XFADE_16    YCI  5
#define STSAMPLE_PLAY_MODE_XFADE_32    YCI  6
#define STSAMPLE_PLAY_MODE_XFADE_64    YCI  7
#define STSAMPLE_PLAY_MODE_XFADE_128   YCI  8
#define STSAMPLE_PLAY_MODE_XFADE_256   YCI  9
#define STSAMPLE_PLAY_MODE_XFADE_512   YCI 10
#define STSAMPLE_PLAY_MODE_XFADE_1024  YCI 11
#define STSAMPLE_PLAY_MODE_XFADE_2048  YCI 12
#define STSAMPLE_PLAY_MODE_XFADE_4096  YCI 13
#define STSAMPLE_PLAY_MODE_XFADE_8192  YCI 14
#define STSAMPLE_PLAY_MODE_XFADE_16384 YCI 15
   sUI play_mode;

   sBool b_freeze_sampleoff_when_quiet; // 1=stop rendering when vol is 0, 0=process loops etc. even if not audible (default)
   sUI   startend_fade_num_frames;      // when >0, fade area around loop start/end in/out
   sBool b_modshiftorig;                // modshift sweep style: 1=0..(wavlen-(smpoff+smplen))  , 0=0..(wavlen-(smpoff+modsmplen))
   sBool b_sampleoffnoteon; // 1=allow mod_sampleoff only before voice is started, 0=allow continuous modulation (may cause clicks)

   enum {
      LOOP_OP_MASK  = ((1u<<31)|(1u<<30)|(1u<<29)|(1u<<28)),
      LOOP_OP_SHIFT = 28,
      // loop_op_mask==0, =>
      //    loopptr+0 = sample offset
      //    loopptr+1 = sample len
      //    loopptr+2 = loop op / #repeats
      LOOP_OP_JMP   = (1u<<LOOP_OP_SHIFT), // Jump to loop #val
      LOOP_OP_JNZ   = (2u<<LOOP_OP_SHIFT), // Decrement var and jump to loop #val if not zero
      LOOP_OP_SET   = (3u<<LOOP_OP_SHIFT), // Copy 16bit val to var
      LOOP_OP_MOV   = (4u<<LOOP_OP_SHIFT), // Move var#val to var

      LOOP_VAR_MASK = ((1u<<24)|(1u<<25)|(1u<<26)|(1u<<27)),
      LOOP_VAR_SHIFT = 24,

      LOOP_VAL_MASK = 0xffffu
   };

//#define STSAMPLE_LOOP_OP_MASK   YCI ((1u<<31)|(1u<<30)|(1u<<29)|(1u<<28))
//#define STSAMPLE_LOOP_OP_SHIFT  YCI 28
//#define STSAMPLE_LOOP_OP_JMP    YCI (1u<<LOOP_OP_SHIFT)
//#define STSAMPLE_LOOP_OP_JNZ    YCI (2u<<LOOP_OP_SHIFT)
//#define STSAMPLE_LOOP_OP_SET    YCI (3u<<LOOP_OP_SHIFT)
//#define STSAMPLE_LOOP_OP_MOV    YCI (4u<<LOOP_OP_SHIFT)
//#define STSAMPLE_LOOP_VAR_MASK  YCI ((1u<<24)|(1u<<25)|(1u<<26)|(1u<<27))
//#define STSAMPLE_LOOP_VAR_SHIFT YCI 24
//#define STSAMPLE_LOOP_VAL_MASK  YCI 0xffffu

   sF32 volume;
   sF32 vol_lfo_amt;
   sF32 pan;
   sF32 pan_env_amt;
   sF32 pan_lfo_amt;
   sF32 transpose; // trp/finetune +-1 = 1 semitone. sum of ui_transpose_*
   sS8  ui_transpose_oct;    // +-4
   sS8  ui_transpose_semi;   // +-12
   sF32 ui_transpose_cents;  // +-100
   sF32 freq_env_amt;
   sF32 freq_lfo_amt;

   sF32 beat_frequency;  // -100.100, note-locked detune

   sBool b_lerp_freq;  // 1=interpolate frequency between 1000Hz blocks [true]
   sBool b_lerp_vol;   // 1=interpolate volume between 1000Hz blocks [true]

   // default=0.0 (velocity does not affect volume)
   //        -1.0 = decrease volume
   //         1.0 = increase volume
   sF32 volume_velocity_amount;

   // 0..1
   sF32 sampleoffset_velocity_amount;
   sBool b_invert_sampleoffset_velocity_amount;

   // 0..1
   sF32 sampleoffset_rel;

   // 0..1
   sF32 sampleoffset_rand_amount;

   // (todo) rename 'timestretch' => 'wavetable' (it can be used as a lo-fi timestretch, though)
   sBool b_timestretch;                  // 1=enable timestretch
   sBool b_timestretch_additive;         // 1=enable resynthesis based wavetable mode
   sBool b_timestretch_xfade;            // 1=crossfade cycle start/end (suppress hifreq clicks)
   sF32  timestretch;                    // -1..1
   sF32  timestretch_granularity;        // cycle length, in samples
   sF32  timestretch_granularity_amount; // 0..1, 1=small granularity if close to orig pitch
   sF32  timestretch_additive_xfade;     // 0..1. Used in Sample::analyzePartials() (xfade cycles)
   sBool b_static_cyclelen;              // 1=force cycle length (chiptunes), 0=pitch dependent cyclelength (normal samples)
   sBool b_absolute_timestretch;         // 1=absolute timestretch (1.0=1 sec to reach next cycle), 0=pitch dependent cycle walking
   sSI   timestretch_interpol_type;      // see STSAMPLE_TIMESTRETCH_INTERPOL_xxx
   sSI   timestretch_grain_window_type;  // see STSAMPLE_TIMESTRETCH_GRAIN_WINDOW_xxx
   sF32  timestretch_bend;               // -1..1  (phase bend)
   sBool b_wavepath;                     // 1=user defined wavetable path
   sSI   wavepath_idx;                   // initial cycle index
   YAC_FloatArray *wavepath_table;
   sF32  timestretch_startphase_rand_amount;
   sUI   timestretch_2d_w;               // 2D wavetable size. smpoff= (y*(cyclelen*w))+(cyclelen*x)
   sUI   timestretch_2d_h;               // w/h = 0: use regular wavetable mode (select cycle via smpoffset mod)


#define STSAMPLE_TIMESTRETCH_INTERPOL_NONE        YCI  0
#define STSAMPLE_TIMESTRETCH_INTERPOL_LINEAR      YCI  1
#define STSAMPLE_TIMESTRETCH_INTERPOL_SMOOTHSTEP  YCI  2
#define STSAMPLE_TIMESTRETCH_INTERPOL_CLIP25      YCI  3
#define STSAMPLE_TIMESTRETCH_INTERPOL_CLIP50      YCI  4
#define STSAMPLE_TIMESTRETCH_INTERPOL_CLIP75      YCI  5
#define STSAMPLE_TIMESTRETCH_INTERPOL_CLIPMOD     YCI  6
#define STSAMPLE_TIMESTRETCH_INTERPOL_CLIPVEL     YCI  7
#define STSAMPLE_TIMESTRETCH_INTERPOL_OFF25       YCI  8
#define STSAMPLE_TIMESTRETCH_INTERPOL_OFF50       YCI  9
#define STSAMPLE_TIMESTRETCH_INTERPOL_OFF75       YCI 10
#define STSAMPLE_TIMESTRETCH_INTERPOL_OFFMOD      YCI 11
#define STSAMPLE_TIMESTRETCH_INTERPOL_OFFVEL      YCI 12
#define STSAMPLE_TIMESTRETCH_INTERPOL_PHASE25W00  YCI 13
#define STSAMPLE_TIMESTRETCH_INTERPOL_PHASE25W25  YCI 14
#define STSAMPLE_TIMESTRETCH_INTERPOL_PHASE25W50  YCI 15
#define STSAMPLE_TIMESTRETCH_INTERPOL_PHASE25W75  YCI 16
#define STSAMPLE_TIMESTRETCH_INTERPOL_PHASE50W00  YCI 17
#define STSAMPLE_TIMESTRETCH_INTERPOL_PHASE50W25  YCI 18
#define STSAMPLE_TIMESTRETCH_INTERPOL_PHASE50W50  YCI 19
#define STSAMPLE_TIMESTRETCH_INTERPOL_PHASE50W75  YCI 20
#define STSAMPLE_TIMESTRETCH_INTERPOL_PHASE75W00  YCI 21
#define STSAMPLE_TIMESTRETCH_INTERPOL_PHASE75W25  YCI 22
#define STSAMPLE_TIMESTRETCH_INTERPOL_PHASE75W50  YCI 23
#define STSAMPLE_TIMESTRETCH_INTERPOL_PHASE75W75  YCI 24
#define STSAMPLE_TIMESTRETCH_INTERPOL_PHASEMODW00 YCI 25
#define STSAMPLE_TIMESTRETCH_INTERPOL_PHASEMODW25 YCI 26
#define STSAMPLE_TIMESTRETCH_INTERPOL_PHASEMODW50 YCI 27
#define STSAMPLE_TIMESTRETCH_INTERPOL_PHASEMODW75 YCI 28
#define STSAMPLE_TIMESTRETCH_INTERPOL_PHASEMODVEL YCI 29
#define STSAMPLE_TIMESTRETCH_INTERPOL_PHASEVELW00 YCI 30
#define STSAMPLE_TIMESTRETCH_INTERPOL_PHASEVELW25 YCI 31
#define STSAMPLE_TIMESTRETCH_INTERPOL_PHASEVELW50 YCI 32
#define STSAMPLE_TIMESTRETCH_INTERPOL_PHASEVELW75 YCI 33
#define STSAMPLE_TIMESTRETCH_INTERPOL_PHASEVELMOD YCI 34
#define STSAMPLE_TIMESTRETCH_INTERPOL_ORDER2      YCI 35
#define STSAMPLE_TIMESTRETCH_INTERPOL_ORDER3      YCI 36
#define STSAMPLE_TIMESTRETCH_INTERPOL_ORDER4      YCI 37
#define STSAMPLE_TIMESTRETCH_INTERPOL_ORDER5      YCI 38
#define STSAMPLE_TIMESTRETCH_INTERPOL_ORDER6      YCI 39
#define STSAMPLE_TIMESTRETCH_INTERPOL_ORDER7      YCI 40

#define STSAMPLE_TIMESTRETCH_GRAIN_WINDOW_NONE    YCI 0
#define STSAMPLE_TIMESTRETCH_GRAIN_WINDOW_SINE    YCI 1
#define STSAMPLE_TIMESTRETCH_GRAIN_WINDOW_TRI     YCI 2

   sBool b_timedloop;        // 1= pitch independent number of repeats (volramping), 0=loop at cycle boundaries(pitch dependent)
   sBool b_timedloop_fade;   // 1=fade in/out when switching waveforms / loop steps, 0=toggle hard (perc.) (TODO) rename this to b_fade ?
   sBool b_oneshot_loopstep; // 1= auto-insert silence after oneshot replay of (timed) loopstep
   sF32  timedloop_base;     // multiply loop repeatcount by 1sec/timedloop_base. requires b_loopms=1. Default is 1000.0f (=> 1millisec)

   StADSR *adsr_freq;   // Owned by sample instance
   StADSR *adsr_vol;
   StADSR *adsr_pan;
   StADSR *adsr_aux;

   StLFO *lfo_freq;
   StLFO *lfo_vol;
   StLFO *lfo_pan;
   StLFO *lfo_aux;

   // (note) unlike all other state, the global LFOs link back to the parent sample_player (for modulation)
   StLFOPlayer lfo_freq_global;
   StLFOPlayer lfo_vol_global;
   StLFOPlayer lfo_pan_global;
   StLFOPlayer lfo_aux_global;
   sUI         global_lfo_tick_nr;

   // in TSR (Amiga replay):
   //  0: volume
   //  1: freq_fine
   //  2: freq_coarse
   //  3: loop
#define STSAMPLE_NUM_MODSEQ          YCI  4
#define STSAMPLE_MAX_MODSEQ_PATCHES  YCI 16

   StModSeqPlayer modseq_global[STSAMPLE_NUM_MODSEQ];
   sUI global_modseq_tick_nr;

   StModSeq *modseq[STSAMPLE_NUM_MODSEQ][STSAMPLE_MAX_MODSEQ_PATCHES];

   sU8 default_modseq_patches[STSAMPLE_NUM_MODSEQ];  // 0..15

   sUI num_volramp_steps_in;  // 0=no ramping, >0: fadein new voice in n blocks. default=1
   sUI num_volramp_steps_out; // 0=no ramping, >0: delay next voice re-alloc by n blocks and fadeout. default=1

#define STSAMPLE_INTERPOL_NONE      YCI 0
#define STSAMPLE_INTERPOL_LINEAR    YCI 1
#define STSAMPLE_INTERPOL_VSR2      YCI 2
#define STSAMPLE_INTERPOL_VSR4      YCI 3
#define STSAMPLE_INTERPOL_VSR8      YCI 4
#define STSAMPLE_INTERPOL_VSR16     YCI 5
#define STSAMPLE_INTERPOL_VSR32     YCI 6
#define STSAMPLE_INTERPOL_VSR64     YCI 7
#define STSAMPLE_INTERPOL_LANCZOS4  YCI 8  // [12Jan2025] changed from 2 to 8
#define STSAMPLE_INTERPOL_LANCZOS8  YCI 9
#define STSAMPLE_INTERPOL_LANCZOS16 YCI 10
#define STSAMPLE_INTERPOL_LANCZOS32 YCI 11
#define STSAMPLE_INTERPOL_LANCZOS64 YCI 12
   sSI interpol_type;

   sUI interpol_order;  // 0=linear .. 6=6th order smoothstep. def=0

   sUI   br_bits;    // 0..15  (simulate bit-reduced output by converting sample frames after reading)
   sU16  br_mask;    // 0..32767
   sF32  br_preamp;  // 0..100

   sUI   smpoff_bits;  // 0 = full res, 1=/2 sample-rate reduction, 2=/4 reduction, 3=/8 reduction, ..
   sUI   smpoff_mask;  // ~0 = full res, ~1=/2 sample-rate reduction, ~3=/4 reduction, ~7=/8 reduction, ..

   // true(1)=play first iteration from sample start (to loop end) before entering loop (PT-like)
   // maybe(-1)=play first iteration from last loop offset (useful for multisamples)
   sBool b_from_start;

   struct {
      sBool     b_enable;  // 1=alternate between samples/zones in the same key/vel/mod range
      StSample *hdr;  // group hdr. NULL = first or only sample in alt group
      StSample *samples[STSAMPLE_MAX_ALT_SAMPLES];  // see StSampleBank::updateAltGroups()
      sUI       num;  // number of other samples in alt group
      sUI       rand_tbl[STSAMPLE_MAX_ALT_SAMPLES];  // for non-repeating random number generator
      sUI       rand_num_left;
   } alt;

#define STSAMPLE_FLT_TYPE_NONE  YCI  0

#define STSAMPLE_FLT_TYPE_LP    YCI  1
#define STSAMPLE_FLT_TYPE_LPLP  YCI  2
#define STSAMPLE_FLT_TYPE_HP    YCI  3
#define STSAMPLE_FLT_TYPE_HPHP  YCI  4
#define STSAMPLE_FLT_TYPE_BP    YCI  5
#define STSAMPLE_FLT_TYPE_BPBP  YCI  6
#define STSAMPLE_FLT_TYPE_BR    YCI  7
#define STSAMPLE_FLT_TYPE_BRBR  YCI  8
#define STSAMPLE_FLT_TYPE_PEQ   YCI  9
#define STSAMPLE_FLT_TYPE_PEQ2  YCI 10
#define STSAMPLE_FLT_TYPE_LSH   YCI 11
#define STSAMPLE_FLT_TYPE_LSH2  YCI 12
#define STSAMPLE_FLT_TYPE_HSH   YCI 13
#define STSAMPLE_FLT_TYPE_HSH2  YCI 14

#define STSAMPLE_FLT_TYPE_LPHP  YCI 15
#define STSAMPLE_FLT_TYPE_LPBP  YCI 16
#define STSAMPLE_FLT_TYPE_LPBR  YCI 17

#define STSAMPLE_FLT_TYPE_HPLP  YCI 18
#define STSAMPLE_FLT_TYPE_HPBP  YCI 19
#define STSAMPLE_FLT_TYPE_HPBR  YCI 20

#define STSAMPLE_FLT_TYPE_BPLP  YCI 21
#define STSAMPLE_FLT_TYPE_BPHP  YCI 22
#define STSAMPLE_FLT_TYPE_BPBR  YCI 23

#define STSAMPLE_FLT_TYPE_BRLP  YCI 24
#define STSAMPLE_FLT_TYPE_BRHP  YCI 25
#define STSAMPLE_FLT_TYPE_BRBP  YCI 26


   sBool b_filter;
   sUI   filter_type;         // STSAMPLE_FLT_TYPE_xxx
   sF32  filter_cutoff;       // -1..1
   sF32  filter_pan;          // cutoff_r + pan , cutoff_l - pan  (-1..1)
   sF32  filter_offset;       // -1..1, for dual filter cfg (cutoff+offset for 1st, cutoff-offset for 2nd)
   sF32  filter_eq_gain;      // -50..50, for PEAK, LSH, and HSH filters
   sF32  filter_resonance;    // 0..1
   sF32  filter_aux_env_amount;  // -1..1
   sF32  filter_aux_env_velocity_amount;  // -2..2  (added to aux_env_amount)
   sF32  filter_aux_lfo_amount;  // -1..1
   sF32  filter_keyboard_amount; // -1..1


#define STSAMPLE_GLIDE_NONE       YCI 0
#define STSAMPLE_GLIDE_TIME_LIN   YCI 1
#define STSAMPLE_GLIDE_TIME_EXP2  YCI 2
#define STSAMPLE_GLIDE_TIME_EXP3  YCI 3
#define STSAMPLE_GLIDE_TIME_EXP4  YCI 4
#define STSAMPLE_GLIDE_FREQ_LIN   YCI 5
#define STSAMPLE_GLIDE_FREQ_EXP2  YCI 6
#define STSAMPLE_GLIDE_FREQ_EXP3  YCI 7
#define STSAMPLE_GLIDE_FREQ_EXP4  YCI 8
   sUI   glide_type;
   sF32  glide_speed_time_up;    // 1=instantly, 0=never (delta per 1000Hz tick)
   sF32  glide_speed_time_down;
   sF32  glide_speed_freq_up;    // linear frequency delta per 1000Hz tick (1=semitone)
   sF32  glide_speed_freq_down;
   /* /\* sBool b_glide_invert;         // false=(CC#65>=64) enables glide, true=(CC#65>=64) disables glide *\/ */
   sBool b_glide_release;        // true=allow glide for notes in release
   sSI   glide_max_note_dist;    // max note delta for glide voice reallocation (if exceeded, alloc new voice) (1..127)
   sBool b_glide_glissando;      // true=quantize to semitones
   sUI   glide_min_time;         // #millisec threshold since note-on (less=alloc new voice, greater=slide). 0=off
   sUI   glide_max_time;         // #millisec threshold for b_glide_release: less=restart release env at last level, greater=alloc new voice
   sBool b_glide_retrig_sample;
   sBool b_glide_retrig_env_freq;
   sBool b_glide_retrig_env_vol;
   sBool b_glide_retrig_env_pan;
   sBool b_glide_retrig_env_aux;
   sBool b_glide_retrig_lfo_freq;
   sBool b_glide_retrig_lfo_vol;
   sBool b_glide_retrig_lfo_pan;
   sBool b_glide_retrig_lfo_aux;
   sBool b_glide_retrig_modseq[STSAMPLE_NUM_MODSEQ];

#define STSAMPLE_NUM_MODMATRIX_ENTRIES  YCI  16

#define STSAMPLE_MMVAR_NUM YCI 16
   sBool b_mmvar_enable;
   sUI   mmvar_num;       // 1..16(MMVAR_NUM)
   sBool b_mmvar_smooth;  // 0=nearest, 1=lerp(clamp to last), -1=lerp(last-to-first)
   sUI   mmvar_edit_idx;  // 0..15(MMVAR_NUM-1)
   sBool b_mmvar_force_edit_idx;
   // off = varIdx*num_modmatrix_entries + mmIdx  
   // value is added to mm->amt (relative amount modulation)
   sF32  mmvar_data[STSAMPLE_NUM_MODMATRIX_ENTRIES * STSAMPLE_MMVAR_NUM];

#define STSAMPLE_MM_SLEW_RESET_OFF      YCI   0
#define STSAMPLE_MM_SLEW_RESET_ZERO     YCI   1
#define STSAMPLE_MM_SLEW_RESET_ONE      YCI   2

#define STSAMPLE_MM_OP_AUTO             YCI   0
#define STSAMPLE_MM_OP_ADD              YCI   1
#define STSAMPLE_MM_OP_MUL              YCI   2
#define STSAMPLE_MM_OP_REPLACE          YCI   3
#define STSAMPLE_MM_OP_REPLACE_INIT     YCI   4  // writes dest on first tick (0 == replay_ticks, reset when note starts, also in glide mode)
#define STSAMPLE_MM_OP_BLEND_SRC        YCI   5
#define STSAMPLE_MM_OP_BLEND_AMT        YCI   6
#define STSAMPLE_MM_OP_COMPARE          YCI   7
#define STSAMPLE_MM_OP_ZEROCROSSING     YCI   8
#define STSAMPLE_MM_OP_MODULO           YCI   9
#define STSAMPLE_MM_OP_TRI              YCI  10
#define STSAMPLE_MM_OP_TRI_NR           YCI  11
#define STSAMPLE_MM_OP_STEP             YCI  12
#define STSAMPLE_MM_NUM_OP              YCI  13

#define STSAMPLE_MM_SRC_NONE                  YCI    0
#define STSAMPLE_MM_SRC_CONST_1               YCI    1
#define STSAMPLE_MM_SRC_CONST_M1              YCI    2
#define STSAMPLE_MM_SRC_CONST_0               YCI    3
#define STSAMPLE_MM_SRC_CONST_4               YCI    4
#define STSAMPLE_MM_SRC_CONST_16              YCI    5
#define STSAMPLE_MM_SRC_CONST_100             YCI    6
#define STSAMPLE_MM_SRC_CONST_127             YCI    7
#define STSAMPLE_MM_SRC_CONST_255             YCI    8
#define STSAMPLE_MM_SRC_RAND_1_PM             YCI    9
#define STSAMPLE_MM_SRC_RAND_1_P              YCI   10
#define STSAMPLE_MM_SRC_RAND_2_PM             YCI   11
#define STSAMPLE_MM_SRC_RAND_2_P              YCI   12
#define STSAMPLE_MM_SRC_RAND_3_PM             YCI   13
#define STSAMPLE_MM_SRC_RAND_3_P              YCI   14
#define STSAMPLE_MM_SRC_RAND_4_PM             YCI   15
#define STSAMPLE_MM_SRC_RAND_4_P              YCI   16
#define STSAMPLE_MM_SRC_KEYBOARD_PM           YCI   17
#define STSAMPLE_MM_SRC_KEYBOARD_P            YCI   18
#define STSAMPLE_MM_SRC_KEYBOARD_M            YCI   19
#define STSAMPLE_MM_SRC_KEYBOARD_ABS          YCI   20
#define STSAMPLE_MM_SRC_VOICE_NR              YCI   21
#define STSAMPLE_MM_SRC_VOICE_NR_EVEN         YCI   22
#define STSAMPLE_MM_SRC_VOICE_NR_ODD          YCI   23
#define STSAMPLE_MM_SRC_VOICE_NR_EVENODD      YCI   24
#define STSAMPLE_MM_SRC_VOICE_NR_ODDEVEN      YCI   25
#define STSAMPLE_MM_SRC_ACTIVE_NOTES          YCI   26
#define STSAMPLE_MM_SRC_ACTIVE_NOTE_EVEN      YCI   27
#define STSAMPLE_MM_SRC_ACTIVE_NOTE_ODD       YCI   28
#define STSAMPLE_MM_SRC_ACTIVE_NOTE_EVENODD   YCI   29
#define STSAMPLE_MM_SRC_ACTIVE_NOTE_ODDEVEN   YCI   30
#define STSAMPLE_MM_SRC_LAYER_NR              YCI   31
#define STSAMPLE_MM_SRC_LAYER_NR_EVEN         YCI   32
#define STSAMPLE_MM_SRC_LAYER_NR_ODD          YCI   33
#define STSAMPLE_MM_SRC_LAYER_NR_EVENODD      YCI   34
#define STSAMPLE_MM_SRC_LAYER_NR_ODDEVEN      YCI   35
#define STSAMPLE_MM_SRC_VELOCITY              YCI   36
#define STSAMPLE_MM_SRC_VELOCITY_HI           YCI   37
#define STSAMPLE_MM_SRC_VELOCITY_EXP2         YCI   38
#define STSAMPLE_MM_SRC_VELOCITY_EXP3         YCI   39
#define STSAMPLE_MM_SRC_VELOCITY_SMOOTHSTEP   YCI   40
#define STSAMPLE_MM_SRC_VELOCITY_INV          YCI   41   // (1-vel)
#define STSAMPLE_MM_SRC_VELOCITY_BIP          YCI   42   // (vel*2-1)
#define STSAMPLE_MM_SRC_RELEASE_VELOCITY      YCI   43
#define STSAMPLE_MM_SRC_RELEASE_VELOCITY_HI   YCI   44
#define STSAMPLE_MM_SRC_GATE                  YCI   45   // 1 while key is pressed, 0 otherwise. added in MM v32.
#define STSAMPLE_MM_SRC_TRIG_PULSE            YCI   46   // 1 for one tick when voice starts (also in glide mode), 0 otherwise. added in MM v32.
#define STSAMPLE_MM_SRC_RELEASE               YCI   47   // 1 when voice is in release phase, 0 otherwise
#define STSAMPLE_MM_SRC_RELEASE_PULSE         YCI   48   // 1 for one tick when release phase starts, 0 otherwise
#define STSAMPLE_MM_SRC_SUSTAIN_PEDAL         YCI   49   // added in MM v39  (CC#64)
#define STSAMPLE_MM_SRC_SOFT_PEDAL            YCI   50   // added in MM v39  (CC#67)
#define STSAMPLE_MM_SRC_PITCHBEND_PM          YCI   51 
#define STSAMPLE_MM_SRC_PITCHBEND_PM_ON       YCI   52 
#define STSAMPLE_MM_SRC_PITCHBEND_P           YCI   53
#define STSAMPLE_MM_SRC_PITCHBEND_P_ON        YCI   54
#define STSAMPLE_MM_SRC_PITCHBEND_M           YCI   55
#define STSAMPLE_MM_SRC_PITCHBEND_M_ON        YCI   56
#define STSAMPLE_MM_SRC_PRESSURE              YCI   57
#define STSAMPLE_MM_SRC_PRESSURE_ON           YCI   58
#define STSAMPLE_MM_SRC_PRESSURE_HI           YCI   59
#define STSAMPLE_MM_SRC_PRESSURE_HI_ON        YCI   60
#define STSAMPLE_MM_SRC_PRESSURE_MAX          YCI   61
#define STSAMPLE_MM_SRC_CC1_MODWHEEL          YCI   62
#define STSAMPLE_MM_SRC_CC1_MODWHEEL_ON       YCI   63
#define STSAMPLE_MM_SRC_CC1_MODWHEEL_INV      YCI   64  // added in MM v28
#define STSAMPLE_MM_SRC_CC1_MODWHEEL_INV_ON   YCI   65  // added in MM v28
#define STSAMPLE_MM_SRC_CC1_MODWHEEL_BIP      YCI   66  // added in MM v28
#define STSAMPLE_MM_SRC_CC1_MODWHEEL_BIP_ON   YCI   67  // added in MM v28
#define STSAMPLE_MM_SRC_CC2_BREATHCTL         YCI   68
#define STSAMPLE_MM_SRC_CC2_BREATHCTL_ON      YCI   69
#define STSAMPLE_MM_SRC_CC2_BREATHCTL_INV     YCI   70  // added in MM v29
#define STSAMPLE_MM_SRC_CC2_BREATHCTL_INV_ON  YCI   71  // added in MM v29
#define STSAMPLE_MM_SRC_CC2_BREATHCTL_BIP     YCI   72  // added in MM v29
#define STSAMPLE_MM_SRC_CC2_BREATHCTL_BIP_ON  YCI   73  // added in MM v29
#define STSAMPLE_MM_SRC_CC4_FOOTCTL           YCI   74
#define STSAMPLE_MM_SRC_CC4_FOOTCTL_ON        YCI   75
#define STSAMPLE_MM_SRC_CC4_FOOTCTL_INV       YCI   76  // added in MM v30
#define STSAMPLE_MM_SRC_CC4_FOOTCTL_INV_ON    YCI   77  // added in MM v30
#define STSAMPLE_MM_SRC_CC4_FOOTCTL_BIP       YCI   78  // added in MM v30
#define STSAMPLE_MM_SRC_CC4_FOOTCTL_BIP_ON    YCI   79  // added in MM v30
#define STSAMPLE_MM_SRC_CC7_VOLUME            YCI   80
#define STSAMPLE_MM_SRC_CC7_VOLUME_ON         YCI   81
#define STSAMPLE_MM_SRC_CC8_BALANCE           YCI   82
#define STSAMPLE_MM_SRC_CC8_BALANCE_ON        YCI   83
#define STSAMPLE_MM_SRC_CC10_PAN              YCI   84
#define STSAMPLE_MM_SRC_CC10_PAN_ON           YCI   85
#define STSAMPLE_MM_SRC_CC11_EXPR             YCI   86
#define STSAMPLE_MM_SRC_CC11_EXPR_ON          YCI   87
#define STSAMPLE_MM_SRC_CC11_EXPR_INV         YCI   88  // added in MM v31
#define STSAMPLE_MM_SRC_CC11_EXPR_INV_ON      YCI   89  // added in MM v31
#define STSAMPLE_MM_SRC_CC11_EXPR_BIP         YCI   90  // added in MM v31
#define STSAMPLE_MM_SRC_CC11_EXPR_BIP_ON      YCI   91  // added in MM v31
#define STSAMPLE_MM_SRC_CC16_GENERAL_1        YCI   92
#define STSAMPLE_MM_SRC_CC16_GENERAL_1_ON     YCI   93
#define STSAMPLE_MM_SRC_CC17_GENERAL_2        YCI   94
#define STSAMPLE_MM_SRC_CC17_GENERAL_2_ON     YCI   95
#define STSAMPLE_MM_SRC_CC18_GENERAL_3        YCI   96
#define STSAMPLE_MM_SRC_CC18_GENERAL_3_ON     YCI   97
#define STSAMPLE_MM_SRC_CC19_GENERAL_4        YCI   98
#define STSAMPLE_MM_SRC_CC19_GENERAL_4_ON     YCI   99
#define STSAMPLE_MM_SRC_CC74_BRIGHT           YCI  100
#define STSAMPLE_MM_SRC_CC74_BRIGHT_ON        YCI  101
#define STSAMPLE_MM_SRC_CC80_GENERAL_5        YCI  102
#define STSAMPLE_MM_SRC_CC80_GENERAL_5_ON     YCI  103
#define STSAMPLE_MM_SRC_CC81_GENERAL_6        YCI  104
#define STSAMPLE_MM_SRC_CC81_GENERAL_6_ON     YCI  105
#define STSAMPLE_MM_SRC_CC82_GENERAL_7        YCI  106
#define STSAMPLE_MM_SRC_CC82_GENERAL_7_ON     YCI  107
#define STSAMPLE_MM_SRC_CC83_GENERAL_8        YCI  108
#define STSAMPLE_MM_SRC_CC83_GENERAL_8_ON     YCI  109
#define STSAMPLE_MM_SRC_LFO_FREQ              YCI  110
#define STSAMPLE_MM_SRC_LFO_VOL               YCI  111
#define STSAMPLE_MM_SRC_LFO_PAN               YCI  112
#define STSAMPLE_MM_SRC_LFO_AUX               YCI  113
#define STSAMPLE_MM_SRC_ENV_FREQ              YCI  114
#define STSAMPLE_MM_SRC_ENV_VOL               YCI  115
#define STSAMPLE_MM_SRC_ENV_PAN               YCI  116
#define STSAMPLE_MM_SRC_ENV_AUX               YCI  117
#define STSAMPLE_MM_SRC_MODSEQ1               YCI  118
#define STSAMPLE_MM_SRC_MODSEQ2               YCI  119
#define STSAMPLE_MM_SRC_MODSEQ3               YCI  120
#define STSAMPLE_MM_SRC_MODSEQ4               YCI  121
#define STSAMPLE_MM_SRC_MODSEQ1_BY_VOICEKEY   YCI  122
#define STSAMPLE_MM_SRC_MODSEQ2_BY_VOICEKEY   YCI  123
#define STSAMPLE_MM_SRC_MODSEQ3_BY_VOICEKEY   YCI  124
#define STSAMPLE_MM_SRC_MODSEQ4_BY_VOICEKEY   YCI  125
#define STSAMPLE_MM_SRC_MODSEQ4_STEP_1        YCI  126  // v36+
#define STSAMPLE_MM_SRC_MODSEQ4_STEP_2        YCI  127  // ..
#define STSAMPLE_MM_SRC_MODSEQ4_STEP_3        YCI  128  // ..
#define STSAMPLE_MM_SRC_MODSEQ4_STEP_4        YCI  129  // ..
#define STSAMPLE_MM_SRC_MODSEQ4_STEP_5        YCI  130  // ..
#define STSAMPLE_MM_SRC_MODSEQ4_STEP_6        YCI  131  // ..
#define STSAMPLE_MM_SRC_MODSEQ4_STEP_7        YCI  132  // ..
#define STSAMPLE_MM_SRC_MODSEQ4_STEP_8        YCI  133  // ..
#define STSAMPLE_MM_SRC_MODSEQ4_STEP_9        YCI  134  // ..
#define STSAMPLE_MM_SRC_MODSEQ4_STEP_10       YCI  135  // ..
#define STSAMPLE_MM_SRC_MODSEQ4_STEP_11       YCI  136  // ..
#define STSAMPLE_MM_SRC_MODSEQ4_STEP_12       YCI  137  // ..
#define STSAMPLE_MM_SRC_MODSEQ4_STEP_13       YCI  138  // ..
#define STSAMPLE_MM_SRC_MODSEQ4_STEP_14       YCI  139  // ..
#define STSAMPLE_MM_SRC_MODSEQ4_STEP_15       YCI  140  // ..
#define STSAMPLE_MM_SRC_MODSEQ4_STEP_16       YCI  141  // ..
#define STSAMPLE_MM_SRC_REG1                  YCI  142
#define STSAMPLE_MM_SRC_REG2                  YCI  143
#define STSAMPLE_MM_SRC_REG3                  YCI  144
#define STSAMPLE_MM_SRC_REG4                  YCI  145
#define STSAMPLE_MM_SRC_CC                    YCI  146
#define STSAMPLE_MM_SRC_VOICE_CALIBRATION_1   YCI  147
#define STSAMPLE_MM_SRC_VOICE_CALIBRATION_1_R YCI  148
#define STSAMPLE_MM_SRC_VOICE_CALIBRATION_2   YCI  149
#define STSAMPLE_MM_SRC_VOICE_CALIBRATION_2_R YCI  150
#define STSAMPLE_MM_SRC_VOICE_CALIBRATION_3   YCI  151
#define STSAMPLE_MM_SRC_VOICE_CALIBRATION_3_R YCI  152
#define STSAMPLE_MM_SRC_VOICE_CALIBRATION_4   YCI  153
#define STSAMPLE_MM_SRC_VOICE_CALIBRATION_4_R YCI  154
#define STSAMPLE_MM_SRC_VOICE_CALIBRATION_5   YCI  155
#define STSAMPLE_MM_SRC_VOICE_CALIBRATION_5_R YCI  156
#define STSAMPLE_MM_SRC_VOICE_CALIBRATION_6   YCI  157
#define STSAMPLE_MM_SRC_VOICE_CALIBRATION_6_R YCI  158
#define STSAMPLE_MM_SRC_PLAYPOS_Z1            YCI  159
#define STSAMPLE_MM_SRC_PLAYPOS_Z2            YCI  160
#define STSAMPLE_MM_SRC_PLAYPOS_Z3            YCI  161
#define STSAMPLE_MM_SRC_PLAYPOS_Z4            YCI  162
#define STSAMPLE_MM_SRC_GLIDE_TIME            YCI  163  // 0..1 while gliding towards target note. added in MM v31.
#define STSAMPLE_MM_SRC_GLIDE_TIME_INV        YCI  164  // 1..0 while gliding towards target note. added in MM v31.
#define STSAMPLE_MM_SRC_GLIDE_TIME_TRI        YCI  165  // 0..1..0 while gliding towards target note. added in MM v31.
#define STSAMPLE_MM_SRC_GLIDE_TIME_TRI_INV    YCI  166  // 1..0..1 while gliding towards target note. added in MM v31.
#define STSAMPLE_MM_SRC_VOICE_BUS_1           YCI  167  // VOICE_BUS_1..8(_ABS) added in MM v34
#define STSAMPLE_MM_SRC_VOICE_BUS_1_ABS       YCI  168
#define STSAMPLE_MM_SRC_VOICE_BUS_2           YCI  169
#define STSAMPLE_MM_SRC_VOICE_BUS_2_ABS       YCI  170
#define STSAMPLE_MM_SRC_VOICE_BUS_3           YCI  171
#define STSAMPLE_MM_SRC_VOICE_BUS_3_ABS       YCI  172
#define STSAMPLE_MM_SRC_VOICE_BUS_4           YCI  173
#define STSAMPLE_MM_SRC_VOICE_BUS_4_ABS       YCI  174
#define STSAMPLE_MM_SRC_VOICE_BUS_5           YCI  175
#define STSAMPLE_MM_SRC_VOICE_BUS_5_ABS       YCI  176
#define STSAMPLE_MM_SRC_VOICE_BUS_6           YCI  177
#define STSAMPLE_MM_SRC_VOICE_BUS_6_ABS       YCI  178
#define STSAMPLE_MM_SRC_VOICE_BUS_7           YCI  179
#define STSAMPLE_MM_SRC_VOICE_BUS_7_ABS       YCI  180
#define STSAMPLE_MM_SRC_VOICE_BUS_8           YCI  181
#define STSAMPLE_MM_SRC_VOICE_BUS_8_ABS       YCI  182
#define STSAMPLE_MM_NUM_SRC                   YCI  183

#define STSAMPLE_MM_DST_NONE                        YCI   0
#define STSAMPLE_MM_DST_FREQ                        YCI   1
#define STSAMPLE_MM_DST_FREQ_OCT                    YCI   2
#define STSAMPLE_MM_DST_FREQ_FIFTH                  YCI   3
#define STSAMPLE_MM_DST_FREQ_FOURTH                 YCI   4
#define STSAMPLE_MM_DST_FREQ_SEMI                   YCI   5
#define STSAMPLE_MM_DST_FREQ_FINE                   YCI   6
#define STSAMPLE_MM_DST_FREQ_BEAT                   YCI   7
#define STSAMPLE_MM_DST_TUNING_TBL_ABS              YCI   8  // v35+
#define STSAMPLE_MM_DST_TUNING_TBL_REL              YCI   9  // v35+
#define STSAMPLE_MM_DST_NOTE_2                      YCI   10 // v37+
#define STSAMPLE_MM_DST_FREQ_2                      YCI   11 // v37+
#define STSAMPLE_MM_DST_VOLUME                      YCI   12   
#define STSAMPLE_MM_DST_PAN                         YCI   13   
#define STSAMPLE_MM_DST_FILTER_CUTOFF               YCI   14 
#define STSAMPLE_MM_DST_FILTER_PAN                  YCI   15 
#define STSAMPLE_MM_DST_FILTER_OFFSET               YCI   16 
#define STSAMPLE_MM_DST_FILTER_RESONANCE            YCI   17 
#define STSAMPLE_MM_DST_SAMPLE_OFFSET               YCI   18 
#define STSAMPLE_MM_DST_SAMPLE_SHIFT                YCI   19 
#define STSAMPLE_MM_DST_SAMPLE_SHIFT_EOL            YCI   20 
#define STSAMPLE_MM_DST_LOOP_RESTART                YCI   21 // v7+
#define STSAMPLE_MM_DST_CYCLE_LEN                   YCI   22 
#define STSAMPLE_MM_DST_CYCLE_LEN_EOL               YCI   23 // v26+
#define STSAMPLE_MM_DST_CYCLE_LEN_SAMPLE_OFFSET     YCI   24 // v17+
#define STSAMPLE_MM_DST_WT_2D_X_ABS                 YCI   25 // v21+
#define STSAMPLE_MM_DST_WT_2D_X_REL                 YCI   26 // v21+
#define STSAMPLE_MM_DST_WT_2D_Y_ABS                 YCI   27 // v21+
#define STSAMPLE_MM_DST_WT_2D_Y_REL                 YCI   28 // v21+
#define STSAMPLE_MM_DST_LOOP_REPEATS_SCALE          YCI   29 // v15+
#define STSAMPLE_MM_DST_JUMPTOLOOP_REL              YCI   30 
#define STSAMPLE_MM_DST_JUMPTOLOOP_ABS127           YCI   31 // v6+   [02Dec2023]: *127 prescaling now done in modmatrix code (modamt should now be 100%)
#define STSAMPLE_MM_DST_JUMPTOLOOP_ABS              YCI   32 // v34+  [26Apr2024]: re-add unscaled (non-127-scaled) destination
#define STSAMPLE_MM_DST_FREQ_LFO_SPEED              YCI   33 
#define STSAMPLE_MM_DST_FREQ_LFO_SPEED_ABS          YCI   34 
#define STSAMPLE_MM_DST_FREQ_LFO_LEVEL              YCI   35 
#define STSAMPLE_MM_DST_FREQ_LFO_FREQ_AMT           YCI   36 // v4+
#define STSAMPLE_MM_DST_VOL_LFO_SPEED               YCI   37 
#define STSAMPLE_MM_DST_VOL_LFO_SPEED_ABS           YCI   38 
#define STSAMPLE_MM_DST_VOL_LFO_LEVEL               YCI   39 
#define STSAMPLE_MM_DST_VOL_LFO_VOL_AMT             YCI   40 // v4+
#define STSAMPLE_MM_DST_PAN_LFO_SPEED               YCI   41 
#define STSAMPLE_MM_DST_PAN_LFO_SPEED_ABS           YCI   42 
#define STSAMPLE_MM_DST_PAN_LFO_LEVEL               YCI   43 
#define STSAMPLE_MM_DST_PAN_LFO_PAN_AMT             YCI   44 // v4+
#define STSAMPLE_MM_DST_AUX_LFO_SPEED               YCI   45 
#define STSAMPLE_MM_DST_AUX_LFO_SPEED_ABS           YCI   46 
#define STSAMPLE_MM_DST_AUX_LFO_LEVEL               YCI   47 
#define STSAMPLE_MM_DST_AUX_LFO_FLT_AMT             YCI   48 // v4+
#define STSAMPLE_MM_DST_FREQ_ENV_SPEED              YCI   49 
#define STSAMPLE_MM_DST_FREQ_ENV_SPEED_ABS          YCI   50 
#define STSAMPLE_MM_DST_FREQ_ENV_LEVEL              YCI   51 
#define STSAMPLE_MM_DST_FREQ_ENV_INTENSITY          YCI   52 
#define STSAMPLE_MM_DST_FREQ_ENV_ATTACK_SPEED       YCI   53 
#define STSAMPLE_MM_DST_FREQ_ENV_ATTACK_SPEED_ABS   YCI   54 
#define STSAMPLE_MM_DST_FREQ_ENV_SUSTAIN_SPEED      YCI   55 
#define STSAMPLE_MM_DST_FREQ_ENV_SUSTAIN_SPEED_ABS  YCI   56 
#define STSAMPLE_MM_DST_FREQ_ENV_RELEASE_SPEED      YCI   57 
#define STSAMPLE_MM_DST_FREQ_ENV_RELEASE_SPEED_ABS  YCI   58 
#define STSAMPLE_MM_DST_VOL_ENV_SPEED               YCI   59 
#define STSAMPLE_MM_DST_VOL_ENV_SPEED_ABS           YCI   60 
#define STSAMPLE_MM_DST_VOL_ENV_LEVEL               YCI   61 
#define STSAMPLE_MM_DST_VOL_ENV_INTENSITY           YCI   62 
#define STSAMPLE_MM_DST_VOL_ENV_ATTACK_SPEED        YCI   63 
#define STSAMPLE_MM_DST_VOL_ENV_ATTACK_SPEED_ABS    YCI   64 
#define STSAMPLE_MM_DST_VOL_ENV_SUSTAIN_SPEED       YCI   65 
#define STSAMPLE_MM_DST_VOL_ENV_SUSTAIN_SPEED_ABS   YCI   66 
#define STSAMPLE_MM_DST_VOL_ENV_RELEASE_SPEED       YCI   67 
#define STSAMPLE_MM_DST_VOL_ENV_RELEASE_SPEED_ABS   YCI   68 
#define STSAMPLE_MM_DST_PAN_ENV_SPEED               YCI   69 
#define STSAMPLE_MM_DST_PAN_ENV_SPEED_ABS           YCI   70 
#define STSAMPLE_MM_DST_PAN_ENV_LEVEL               YCI   71 
#define STSAMPLE_MM_DST_PAN_ENV_INTENSITY           YCI   72 
#define STSAMPLE_MM_DST_PAN_ENV_ATTACK_SPEED        YCI   73 
#define STSAMPLE_MM_DST_PAN_ENV_ATTACK_SPEED_ABS    YCI   74 
#define STSAMPLE_MM_DST_PAN_ENV_SUSTAIN_SPEED       YCI   75 
#define STSAMPLE_MM_DST_PAN_ENV_SUSTAIN_SPEED_ABS   YCI   76 
#define STSAMPLE_MM_DST_PAN_ENV_RELEASE_SPEED       YCI   77 
#define STSAMPLE_MM_DST_PAN_ENV_RELEASE_SPEED_ABS   YCI   78 
#define STSAMPLE_MM_DST_AUX_ENV_SPEED               YCI   79 
#define STSAMPLE_MM_DST_AUX_ENV_SPEED_ABS           YCI   80 
#define STSAMPLE_MM_DST_AUX_ENV_LEVEL               YCI   81 
#define STSAMPLE_MM_DST_AUX_ENV_INTENSITY           YCI   82 
#define STSAMPLE_MM_DST_AUX_ENV_ATTACK_SPEED        YCI   83 
#define STSAMPLE_MM_DST_AUX_ENV_ATTACK_SPEED_ABS    YCI   84 
#define STSAMPLE_MM_DST_AUX_ENV_SUSTAIN_SPEED       YCI   85 
#define STSAMPLE_MM_DST_AUX_ENV_SUSTAIN_SPEED_ABS   YCI   86 
#define STSAMPLE_MM_DST_AUX_ENV_RELEASE_SPEED       YCI   87 
#define STSAMPLE_MM_DST_AUX_ENV_RELEASE_SPEED_ABS   YCI   88 
#define STSAMPLE_MM_DST_MOD_1_AMOUNT                YCI   89 
#define STSAMPLE_MM_DST_MOD_2_AMOUNT                YCI   90 
#define STSAMPLE_MM_DST_MOD_3_AMOUNT                YCI   91 
#define STSAMPLE_MM_DST_MOD_4_AMOUNT                YCI   92 
#define STSAMPLE_MM_DST_MOD_5_AMOUNT                YCI   93 
#define STSAMPLE_MM_DST_MOD_6_AMOUNT                YCI   94 
#define STSAMPLE_MM_DST_MOD_7_AMOUNT                YCI   95 
#define STSAMPLE_MM_DST_MOD_8_AMOUNT                YCI   96 
#define STSAMPLE_MM_DST_MOD_9_AMOUNT                YCI   97 
#define STSAMPLE_MM_DST_MOD_10_AMOUNT               YCI   98 
#define STSAMPLE_MM_DST_MOD_11_AMOUNT               YCI   99 
#define STSAMPLE_MM_DST_MOD_12_AMOUNT               YCI   100
#define STSAMPLE_MM_DST_MOD_13_AMOUNT               YCI   101
#define STSAMPLE_MM_DST_MOD_14_AMOUNT               YCI   102
#define STSAMPLE_MM_DST_MOD_15_AMOUNT               YCI   103
#define STSAMPLE_MM_DST_MOD_16_AMOUNT               YCI   104
#define STSAMPLE_MM_DST_GLIDE_SPEED                 YCI   105
#define STSAMPLE_MM_DST_TIMESTRETCH                 YCI   106
#define STSAMPLE_MM_DST_TIMESTRETCH_BEND            YCI   107
#define STSAMPLE_MM_DST_MODSEQ1_PATCH               YCI   108
#define STSAMPLE_MM_DST_MODSEQ1_SPEED               YCI   109
#define STSAMPLE_MM_DST_MODSEQ1_SPEED_ABS           YCI   110
#define STSAMPLE_MM_DST_MODSEQ1_LEVEL               YCI   111
#define STSAMPLE_MM_DST_MODSEQ1_NUMSTEPS            YCI   112
#define STSAMPLE_MM_DST_MODSEQ1_ADVANCE             YCI   113
#define STSAMPLE_MM_DST_MODSEQ1_STEP                YCI   114
#define STSAMPLE_MM_DST_MODSEQ2_PATCH               YCI   115
#define STSAMPLE_MM_DST_MODSEQ2_SPEED               YCI   116
#define STSAMPLE_MM_DST_MODSEQ2_SPEED_ABS           YCI   117
#define STSAMPLE_MM_DST_MODSEQ2_LEVEL               YCI   118
#define STSAMPLE_MM_DST_MODSEQ2_NUMSTEPS            YCI   119
#define STSAMPLE_MM_DST_MODSEQ2_ADVANCE             YCI   120
#define STSAMPLE_MM_DST_MODSEQ2_STEP                YCI   121
#define STSAMPLE_MM_DST_MODSEQ3_PATCH               YCI   122
#define STSAMPLE_MM_DST_MODSEQ3_SPEED               YCI   123
#define STSAMPLE_MM_DST_MODSEQ3_SPEED_ABS           YCI   124
#define STSAMPLE_MM_DST_MODSEQ3_LEVEL               YCI   125
#define STSAMPLE_MM_DST_MODSEQ3_NUMSTEPS            YCI   126
#define STSAMPLE_MM_DST_MODSEQ3_ADVANCE             YCI   127
#define STSAMPLE_MM_DST_MODSEQ3_STEP                YCI   128
#define STSAMPLE_MM_DST_MODSEQ4_PATCH               YCI   129
#define STSAMPLE_MM_DST_MODSEQ4_SPEED               YCI   130
#define STSAMPLE_MM_DST_MODSEQ4_SPEED_ABS           YCI   131
#define STSAMPLE_MM_DST_MODSEQ4_LEVEL               YCI   132
#define STSAMPLE_MM_DST_MODSEQ4_NUMSTEPS            YCI   133
#define STSAMPLE_MM_DST_MODSEQ4_ADVANCE             YCI   134
#define STSAMPLE_MM_DST_MODSEQ4_STEP                YCI   135
#define STSAMPLE_MM_DST_RETRIG_FREQ_ENV             YCI   136
#define STSAMPLE_MM_DST_RETRIG_VOL_ENV              YCI   137
#define STSAMPLE_MM_DST_RETRIG_PAN_ENV              YCI   138
#define STSAMPLE_MM_DST_RETRIG_AUX_ENV              YCI   139
#define STSAMPLE_MM_DST_RETRIG_FREQ_LFO             YCI   140
#define STSAMPLE_MM_DST_RETRIG_VOL_LFO              YCI   141
#define STSAMPLE_MM_DST_RETRIG_PAN_LFO              YCI   142
#define STSAMPLE_MM_DST_RETRIG_AUX_LFO              YCI   143
#define STSAMPLE_MM_DST_RETRIG_MODSEQ1              YCI   144
#define STSAMPLE_MM_DST_RETRIG_MODSEQ2              YCI   145
#define STSAMPLE_MM_DST_RETRIG_MODSEQ3              YCI   146
#define STSAMPLE_MM_DST_RETRIG_MODSEQ4              YCI   147
#define STSAMPLE_MM_DST_REG_1_ON                    YCI   148  // updates register only on first tick (0 == replay_ticks, reset at note on, also in glide mode)
#define STSAMPLE_MM_DST_REG_2_ON                    YCI   149
#define STSAMPLE_MM_DST_REG_3_ON                    YCI   150
#define STSAMPLE_MM_DST_REG_4_ON                    YCI   151
#define STSAMPLE_MM_DST_REG_1                       YCI   152
#define STSAMPLE_MM_DST_REG_2                       YCI   153
#define STSAMPLE_MM_DST_REG_3                       YCI   154
#define STSAMPLE_MM_DST_REG_4                       YCI   155
#define STSAMPLE_MM_DST_LIVEREC_START               YCI   156
#define STSAMPLE_MM_DST_LIVEREC_CONTINUE            YCI   157
#define STSAMPLE_MM_DST_LIVEREC_STOP                YCI   158
#define STSAMPLE_MM_DST_PLUGIN_1_MOD_1              YCI   159
#define STSAMPLE_MM_DST_PLUGIN_1_MOD_2              YCI   160
#define STSAMPLE_MM_DST_PLUGIN_1_MOD_3              YCI   161
#define STSAMPLE_MM_DST_PLUGIN_1_MOD_4              YCI   162
#define STSAMPLE_MM_DST_PLUGIN_1_MOD_5              YCI   163
#define STSAMPLE_MM_DST_PLUGIN_1_MOD_6              YCI   164
#define STSAMPLE_MM_DST_PLUGIN_1_MOD_7              YCI   165
#define STSAMPLE_MM_DST_PLUGIN_1_MOD_8              YCI   166
#define STSAMPLE_MM_DST_PLUGIN_2_MOD_1              YCI   167
#define STSAMPLE_MM_DST_PLUGIN_2_MOD_2              YCI   168
#define STSAMPLE_MM_DST_PLUGIN_2_MOD_3              YCI   169
#define STSAMPLE_MM_DST_PLUGIN_2_MOD_4              YCI   170
#define STSAMPLE_MM_DST_PLUGIN_2_MOD_5              YCI   171
#define STSAMPLE_MM_DST_PLUGIN_2_MOD_6              YCI   172
#define STSAMPLE_MM_DST_PLUGIN_2_MOD_7              YCI   173
#define STSAMPLE_MM_DST_PLUGIN_2_MOD_8              YCI   174
#define STSAMPLE_MM_DST_PLUGIN_3_MOD_1              YCI   175
#define STSAMPLE_MM_DST_PLUGIN_3_MOD_2              YCI   176
#define STSAMPLE_MM_DST_PLUGIN_3_MOD_3              YCI   177
#define STSAMPLE_MM_DST_PLUGIN_3_MOD_4              YCI   178
#define STSAMPLE_MM_DST_PLUGIN_3_MOD_5              YCI   179
#define STSAMPLE_MM_DST_PLUGIN_3_MOD_6              YCI   180
#define STSAMPLE_MM_DST_PLUGIN_3_MOD_7              YCI   181
#define STSAMPLE_MM_DST_PLUGIN_3_MOD_8              YCI   182
#define STSAMPLE_MM_DST_PLUGIN_4_MOD_1              YCI   183
#define STSAMPLE_MM_DST_PLUGIN_4_MOD_2              YCI   184
#define STSAMPLE_MM_DST_PLUGIN_4_MOD_3              YCI   185
#define STSAMPLE_MM_DST_PLUGIN_4_MOD_4              YCI   186
#define STSAMPLE_MM_DST_PLUGIN_4_MOD_5              YCI   187
#define STSAMPLE_MM_DST_PLUGIN_4_MOD_6              YCI   188
#define STSAMPLE_MM_DST_PLUGIN_4_MOD_7              YCI   189
#define STSAMPLE_MM_DST_PLUGIN_4_MOD_8              YCI   190
#define STSAMPLE_MM_DST_PLUGIN_1_LEVEL              YCI   191  // DST:level AM:intensity
#define STSAMPLE_MM_DST_PLUGIN_2_LEVEL              YCI   192
#define STSAMPLE_MM_DST_PLUGIN_3_LEVEL              YCI   193
#define STSAMPLE_MM_DST_PLUGIN_4_LEVEL              YCI   194
#define STSAMPLE_MM_DST_SYNC_SPEED                  YCI   195
#define STSAMPLE_MM_DST_SYNC_SLEW                   YCI   196
#define STSAMPLE_MM_DST_SAMPLE_RATE_REDUCTION       YCI   197
#define STSAMPLE_MM_DST_BIT_REDUCTION               YCI   198
#define STSAMPLE_MM_DST_WT_ADDITIVE_CFG             YCI   199
#define STSAMPLE_MM_DST_WT_ADDITIVE_STEREO_SPREAD   YCI   200
#define STSAMPLE_MM_DST_WT_ADDITIVE_PARTIALS        YCI   201
#define STSAMPLE_MM_DST_VARIATION                   YCI   202  // should be first MM entry (when StSample::b_mmvar_enable=1)
#define STSAMPLE_MM_NUM_DST                         YCI   203

   sF32 mm_keyboard_center;  // MM_SRC_KEYBOARD_*  def=60(C-5). Quantized to multiples of 0.5 in standalone replay.
   sF32 mm_keyboard_min;     // min note = center - min
   sF32 mm_keyboard_max;     // max note = center + max
   StSampleMMEntry modmatrix[STSAMPLE_NUM_MODMATRIX_ENTRIES];

   tksampler_lfsr_t lfsr;

#define STSAMPLE_MAX_INPUTS  YCI 4
   sBool b_liverec;
   sBool b_liverec_active;
   sBool b_liverec_pause;
   sUI   liverec_input_idx;
   sBool b_liverec_stereo;  // true=use stereo input
   sF32  liverec_input_pan; // -1=pan left .. 0=center .. 1=pan right
   sUI   liverec_loop_idx;
   sBool b_liverec_copy;    // true=copy buffer at note on. maybe=continously update buffer (at end of loop)
   sBool b_liverec_clear;   // true=clear dest area in non-OSC copy mode. maybe=overdub mode
   sSI   liverec_skip_input_num_frames;  // number of input frames to skip when live recording starts (latency correction)
   sSI   liverec_skip_input_frames_left;
#define STSAMPLE_LIVEREC_DBLBUF_MODE_OFF    YCI 0
#define STSAMPLE_LIVEREC_DBLBUF_MODE_END    YCI 1
#define STSAMPLE_LIVEREC_DBLBUF_MODE_VOICE  YCI 2
#define STSAMPLE_LIVEREC_DBLBUF_MODE_REC    YCI 3
#define STSAMPLE_LIVEREC_DBLBUF_MODE_PARAM  YCI 4


   sUI   liverec_doublebuffer_mode;
#define STSAMPLE_LIVEREC_MODE_TRIG         YCI 0
#define STSAMPLE_LIVEREC_MODE_ZERO         YCI 1   // zero sample until actual rec start ("in time")
#define STSAMPLE_LIVEREC_MODE_KEEP         YCI 2   // keep samples before actual rec start ("in time")
#define STSAMPLE_LIVEREC_MODE_WAIT_ONCE    YCI 3   // only start writing samples once threshold is exceeded
#define STSAMPLE_LIVEREC_MODE_WAIT_REPEAT  YCI 4   // only start writing samples once threshold is exceeded
#define STSAMPLE_LIVEREC_MODE_WAIT_SILENCE_REPEAT  YCI 5   // same as WAIT_REPEAT but wait for silence after recording
#define STSAMPLE_LIVEREC_MODE_CONTINUOUS           YCI 6
#define STSAMPLE_LIVEREC_MODE_CONTINUOUS_RING      YCI 7   // dblbuf should be off, copy on
   sBool b_liverec_doublebuffer_xfade;
   sUI   liverec_mode;
   sF32  liverec_threshold_on;
   sF32  liverec_threshold_off;
#define STSAMPLE_LIVEREC_OSC_MODE_OFF                YCI 0
#define STSAMPLE_LIVEREC_OSC_MODE_ON                 YCI 1
#define STSAMPLE_LIVEREC_OSC_MODE_EVEN               YCI 2
#define STSAMPLE_LIVEREC_OSC_MODE_TUNED              YCI 3
#define STSAMPLE_LIVEREC_OSC_MODE_EVEN_TUNED         YCI 4  // best all-round mode ?
#define STSAMPLE_LIVEREC_OSC_MODE_EDGE_P_TUNED       YCI 5
#define STSAMPLE_LIVEREC_OSC_MODE_EDGE_P_SLOPE_TUNED YCI 6
#define STSAMPLE_LIVEREC_OSC_MODE_PEAK_TUNED         YCI 7
#define STSAMPLE_LIVEREC_OSC_MODE_ZC_PEAK_TUNED      YCI 8
   sUI   liverec_osc_mode;
   sBool b_liverec_osc_singlecycle;
   sF32  liverec_osc_tune;       // def=12*5  (12*4=center octave on Rubicon2)
   sBool b_liverec_osc_sustain;  // aka multi-loop
   sBool b_liverec_monitor;      // true=replace output with input, maybe=add input to output

   sUI   liverec_loop_offset;
   sUI   liverec_loop_len;
   sUI   liverec_doublebuffer_rec_idx;        // 0u=first half of waveform, 1u=2nd half
   sUI   liverec_frame_offset;                // write offset
   sUI   liverec_num_frames;                  // number of frames recorded (clips to dstLen in ring mode)
   sUI   liverec_num_ring_frames;             // resets to 0 after dstLen frames (+ do loop detection)
   sUI   liverec_last_osc_ring_frame_offset;  // for ringbuf, updated when loop detection is run
   sUI   liverec_ring_size;                   // counts from 0..dstLen-1  (ring cycle detection)
   sBool b_liverec_threshold_reached;
   sBool b_liverec_silence_reached;
   sBool ui_redraw_queued;  // for live-recording
   sBool b_liverec_doublebuffer_swap_queued;
   sF32  liverec_frame_offset_xfade_amt;
   sSI   liverec_override_loop_offset;
   sSI   liverec_override_loop_len;
   sSI   liverec_queued_override_loop_offset;
   sSI   liverec_queued_override_loop_len;
   sF32  liverec_ref_cyclelen;
   YAC_IntArray liverec_sustain_sample_loops[2];
   sF32  liverec_osc_fade_amt;  // > 0.0: blend oscillator cycles
   sF32  liverec_osc_pre_filter;  // cutoff (0..1), 1=off
   sBool b_liverec_osc_resample;  // 1=maintain constant cycle, resample input

/* /\* #define STSAMPLE_LIVEREC_FRONT_BUFFER_SZ       (128*1024)  // #frames. must match STSAMPLEVOICE_OVERRIDE_SMPDAT_SZ *\/ */
   sF32 *liverec_front_buffer;  // in non-osc liverec copy mode. size must match StSampleVoice::override_smpdat.
   sUI   liverec_front_buffer_sz;  // matches waveform numFrames*numCh after lazy-init
   /* /\* sUI   liverec_front_buffer_src_offset;  // original frame offset into waveform *\/ */
   /* /\* sUI   liverec_front_buffer_len;         // last copied len *\/ */

   // true=add inputs[liverec_input_idx] to current sample frame (before filter/fx/env)
   // maybe=add inputs and mute regular sample playback while liverecording is active
   sBool b_sum_input;
   sF32  sum_input_xfade;  // -1=sample, 0=both, 1=input

#define STSAMPLE_NUM_PLUGINS                YCI 4
#define STSAMPLE_NUM_MODS_PER_PLUGIN        YCI 8
   st_plugin_slot_t plugins[STSAMPLE_NUM_PLUGINS];
   sUI plugin_mask_def; // bit is set when plugin!=null and !b_skip and process_fx_replace!=NULL and plugin is routed through alg
   sUI plugin_mask_sr;  // bit is set when plugin!=null and !b_skip and process_fx_replace!=NULL and plugin is routed to SR destination
   sUI plugin_mask_am;  // bit is set when plugin!=null and !b_skip and process_fx_replace!=NULL and plugin is routed to AM destination
   sUI plugin_mask_vb;  // bit is set when plugin!=null and !b_skip and process_fx_replace!=NULL and plugin is routed to VB1..8 destination
   sUI plugin_mask_vm;  // bit is set when plugin!=null and !b_skip and process_fx_replace!=NULL and plugin is routed to VM1..8 destination
   sUI ui_last_plugin_row_idx;
   sUI plugin_alg; // 0=1*2*3*4
   sUI plugin_alg_filter_pos;  // 0=pre1, 1=post1, 2=pre2, 3=post2, .., 7=post4
   sUI plugin_alg_outer;  // 0=def (immediately before/after plugin), 1=outer 1, 2=outer 2, 3=outer 3 (enclose entire chain)
   sUI plugin_alg_int;

   sBool b_enable_fx;  // 0=bypass fx+filter

   sSI voice_bus;  // -1=none, 0=self (voice->layer_idx), >0=(other) layer voice bus buffer

   sF64 uiSVZoom;  // last seen SampleView zoom/offset
   sF32 uiSVOffset;
   sUI  uiSVZoomYIdx;

   sBool ui_b_auxtowav;  // 1=render aux attack/decay spline to waveform when curve is edited (e.g. modular envs via ES-3) (editor hint)

#define LIVEREC_OSC_PRE_FILTER_BUFFER_SZ  (16384u)
   sF32 *liverec_osc_pre_filter_buffer;  // lazy-alloc, up to LIVEREC_OSC_PRE_FILTER_BUFFER_SZ mono frames
   StSampleVoiceBiquad liverec_osc_pre_filter_biquad;

   sF32  voice_calibration[6/*lane*/][8/*voiceIdx*/];  // -1..1 range. 6 lanes, 8 voices max. voiceIdx=voice_key%voice_calibration_modulo.
   sUI   voice_calibration_modulo[6/*lane*/];  // 1..8 (table len).
   sF32  voice_calibration_amount[6/*lane*/];  // hard-wired mod dest: 1=fine tune, 2=pan, 3=vol, 4=cutoff, 5=pan env lvl, 6=aux env lvl
   sBool voice_calibration_enable[6/*lane*/];

#define STSAMPLE_ADDITIVE_OSC_SIN  YCI 0
#define STSAMPLE_ADDITIVE_OSC_TRI  YCI 1
#define STSAMPLE_ADDITIVE_OSC_SUP  YCI 2
#define STSAMPLE_ADDITIVE_OSC_SDN  YCI 3
#define STSAMPLE_ADDITIVE_OSC_PUL  YCI 4
#define STSAMPLE_ADDITIVE_OSC_NUM  YCI 5

#define STSAMPLE_ADDITIVE_CFG_NUM  YCI 8
   const sF32 *additive_tbl;  // initially points to StSampleVoice::additive_sin_tbl
   sF32  additive_edit_cfg;  // 0..7.99999 = blend configs 0..8..0
   sBool b_force_additive_edit_cfg;
   sF32  additive_tbl_mix[4096];
   const sF32 *additive_tbl_cfg[STSAMPLE_ADDITIVE_CFG_NUM];  // initially points to StSampleVoice::additive_sin_tbl
   sF32 additive_tbl_cfg_osc[STSAMPLE_ADDITIVE_CFG_NUM][4096];
   sBool additive_cfg_valid[STSAMPLE_ADDITIVE_CFG_NUM];  // false=sine, true=custom osc (becomes valid when edited)
   sF32 additive_osc_lvl[STSAMPLE_ADDITIVE_CFG_NUM][STSAMPLE_ADDITIVE_OSC_NUM];
   sF32 additive_osc_pha[STSAMPLE_ADDITIVE_CFG_NUM][STSAMPLE_ADDITIVE_OSC_NUM];
   sF32 additive_osc_exa[STSAMPLE_ADDITIVE_CFG_NUM][STSAMPLE_ADDITIVE_OSC_NUM];
   sF32 additive_osc_flt[STSAMPLE_ADDITIVE_CFG_NUM][STSAMPLE_ADDITIVE_OSC_NUM];
   sF32 additive_osc_exl[STSAMPLE_ADDITIVE_CFG_NUM][STSAMPLE_ADDITIVE_OSC_NUM];

   sBool additive_normalize[STSAMPLE_ADDITIVE_CFG_NUM];

   sUI  additive_br_bits[STSAMPLE_ADDITIVE_CFG_NUM];    // 0..15  (simulate bit-reduced output)
   sU16 additive_br_mask[STSAMPLE_ADDITIVE_CFG_NUM];    // 0..32767
   sF32 additive_br_preamp[STSAMPLE_ADDITIVE_CFG_NUM];  // 0..100

   sUI  additive_smpoff_bits[STSAMPLE_ADDITIVE_CFG_NUM];  // 0 = full res, 1=/2 sample-rate reduction, 2=/4 reduction, 3=/8 reduction, ..
   sUI  additive_smpoff_mask[STSAMPLE_ADDITIVE_CFG_NUM];  // ~0 = full res, ~1=/2 sample-rate reduction, ~3=/4 reduction, ~7=/8 reduction, ..

   sF32 additive_stereo_spread;

   // anti-imaging/aliasing filter settings
   sUI  ai_num_poles; // 0=AI off
   sF32 ai_q;
   sF32 ai_exp_up;    // cRate > 1  (0 disables up filter)
   sF32 ai_exp_down;  // cRate < 1  (0 disables down filter)
   sF32 ai_lin_oct;   // linear octave range
   sF32 ai_lin_max;   // cutoff at lin_oct (added to exp result)

   sBool b_ui_autoselect;  // 1=autoselect zone when parent Sample(/SampleBank) is selected in editor

   // NULL=use default (StSampleBank) freq table. 128 frequencies (MIDI notes) per table.
   sF32       *tuning_tables[STSAMPLE_MAX_TUNING_TABLES/*16*/];
   YAC_Object *tuning_tables_meta[STSAMPLE_MAX_TUNING_TABLES/*16*/];  // editor info (SampleTuningTable script objects or NULL)
   sSI         default_tuning_table_idx;      // -1=use default table (StSamplePlayer)
   sSI         forced_tuning_table_idx;       // -1=use default_tuning_table_idx

  public:
   StSample(void);
   ~StSample();

   YAC(StSample);

   const sF32 *getCurrentTuningTableOrNull (void);

   void setSamplePlayerForGlobalModulation (StSamplePlayer *_samplePlayer);

   sBool filterNoteOn (sSI _note, sF32 _vel, sF32 _mod);

   static sBool verify_sample_area (sUI &offset, sUI &len, const sUI range);

   StSample *getNextAltRandSample (void);

   sUI liveRecGetCurrentRecBufIdx (void);
   sUI liveRecGetCurrentPlayBufIdx (void);
   void handleLiveRecording (sF32 *_out, const sF32*const*_inputs, sUI _numFrames, sSI _loopShift);
   sSI liveRecFindThresholdIdx (const sF32 *_input, sUI _numFrames, sF32 _threshold);
   void handleLiveRecDeferredBufferSwap (void);
   void handleLiveRecEnd (sBool _bRestart);
   void liveRecSwapBuffers (void);
   void liveRecCopyInputToOutput (sF32 *_out, const sF32*const*_inputs, sUI _numFrames);
   sBool hasLiveRecAdaptiveLoopSize (void);
   void liveRecAdjustOscLoopSize (void);
   void liveRecAdjustOscLoopSizeInt (sSI &_overrideLoopOffset,
                                     sSI &_overrideLoopLen,
                                     sUI _inputOffset,
                                     sUI _inputLen
                                     );
   sBool isLiveRecActive (void);
   void liveRecAdjustOscLoopSizeRing (void);
   void liveRecCopyRingToOverride (sF32 *_smpDatDest,
                                   sUI   _waveOff,
                                   sUI   _copyLoopOffset,
                                   sUI   _curSampleLen  // ==copyLoopLen
                                   );
   void liveRecFadeRingToOverride (sF32 *_smpDatDest,
                                   sUI   _waveOff,
                                   sUI   _copyLoopOffset,
                                   sUI   _curSampleLen,  // ==copyLoopLen
                                   sF32  _newAmt
                                   );
   void liveRecResampleAndFadeRingToOverride (sF32 *_smpDatDest,
                                              sUI   _waveOff,
                                              sUI   _srcOffset,
                                              sUI   _srcLen,  // == liverec_override_loop_len
                                              sUI   _dstOffset,
                                              sUI   _dstLen,  // == copy_loop_len
                                              sF32  _newAmt
                                              );
   void liveRecUpdateSustainModeSampleLoops (void);
   void freeLiveRecSustainSampleLoops (void);
   YAC_IntArray *getSampleLoopsOverride (void);

   sF32 calcMMNoteRel (sF32 _note) const;
   sF32 calcMMNoteAbs (sF32 _note) const;

#define STSAMPLE_VOICEPLUGIN_DST_DEF  YCI  0   // default (audio-rate) processing (after sample read)
#define STSAMPLE_VOICEPLUGIN_DST_SR   YCI  1   // sample read rate modulation (before sample read)
#define STSAMPLE_VOICEPLUGIN_DST_AM   YCI  2   // amplitude modulation (post)
#define STSAMPLE_VOICEPLUGIN_DST_VB1  YCI  3   // render to voice bus 1..8 (after sample read, before 'def')
#define STSAMPLE_VOICEPLUGIN_DST_VB2  YCI  4
#define STSAMPLE_VOICEPLUGIN_DST_VB3  YCI  5
#define STSAMPLE_VOICEPLUGIN_DST_VB4  YCI  6
#define STSAMPLE_VOICEPLUGIN_DST_VB5  YCI  7
#define STSAMPLE_VOICEPLUGIN_DST_VB6  YCI  8
#define STSAMPLE_VOICEPLUGIN_DST_VB7  YCI  9
#define STSAMPLE_VOICEPLUGIN_DST_VB8  YCI 10
#define STSAMPLE_VOICEPLUGIN_DST_VM1  YCI 11   // render single frame to voice bus 1..8 (before modmatrix)
#define STSAMPLE_VOICEPLUGIN_DST_VM2  YCI 12
#define STSAMPLE_VOICEPLUGIN_DST_VM3  YCI 13
#define STSAMPLE_VOICEPLUGIN_DST_VM4  YCI 14
#define STSAMPLE_VOICEPLUGIN_DST_VM5  YCI 15
#define STSAMPLE_VOICEPLUGIN_DST_VM6  YCI 16
#define STSAMPLE_VOICEPLUGIN_DST_VM7  YCI 17
#define STSAMPLE_VOICEPLUGIN_DST_VM8  YCI 18

  protected:
   void updatePluginMask (sUI _pluginIdx);
   void updatePluginAlgInt (void);
   void updateVoicePluginLevels (sUI _pluginIdx);
   void reorderPluginModMatrixEntries (const sUI *_newOrder);

  public:
   YM void reinit (void);
   YM void free (void);

   YM YAC_Object *_getNext     (void);
   YM void        _setNext     (YAC_Object *_next);

   YM YAC_Object *_getName     (void);
   YM void        _setName     (YAC_Object *_str); // copy

   YM YAC_Object *_getWaveform (void);
   YM void        _setWaveform (YAC_Object *_wav); // set ref

   // array size must be 256*256 (256 partials * 256 wavetable waves)
   YM YAC_Object *_getPartialMagnitudes (void);
   YM void        _setPartialMagnitudes (YAC_Object *_wav); // set ref

   YM YAC_Object *_getPartialSpeeds (void);
   YM void        _setPartialSpeeds (YAC_Object *_wav); // set ref

   YM YAC_Object *_getPartialPhases (void);
   YM void        _setPartialPhases (YAC_Object *_wav); // set ref

   // 1..256
   YM void        _setNumPartials (sF32 _num);
   YM sF32        _getNumPartials (void);

   // 0..4096
   YM void        _setPartialCycleLenReset (sF32 _cycleLenReset);
   YM sF32        _getPartialCycleLenReset (void);

   YM void        _setEnablePartialPhase0 (sBool _bEnable);
   YM sBool       _getEnablePartialPhase0 (void);

   YM void        _handleShiftSampleArea (sUI _offset, sSI _len);
   YM void        _handleMoveSampleArea (sUI _offset, sUI _len, sUI _newOffset);
   YM sBool       _verifySampleAreas (void);

   YM YAC_Object *_getMutexGroup (void);
   YM void        _setMutexGroup (YAC_Object *_mtxGrp); // set ref

   YM YAC_Object *_getKeyRange (void);
   YM YAC_Object *_getVelRange (void);
   YM YAC_Object *_getModRange (void);

   YM YAC_Object *_getOrCreateKeyRange (void);
   YM YAC_Object *_getOrCreateVelRange (void);
   YM YAC_Object *_getOrCreateModRange (void);

   YM YAC_Object *_getFreqADSR (void);
   YM YAC_Object *_getVolADSR  (void);
   YM YAC_Object *_getPanADSR  (void);
   YM YAC_Object *_getAuxADSR  (void);

   YM YAC_Object *_getOrCreateFreqADSR (void);
   YM YAC_Object *_getOrCreateVolADSR  (void);
   YM YAC_Object *_getOrCreatePanADSR  (void);
   YM YAC_Object *_getOrCreateAuxADSR  (void);

   YM YAC_Object *_getFreqLFO (void);
   YM YAC_Object *_getVolLFO  (void);
   YM YAC_Object *_getPanLFO  (void);
   YM YAC_Object *_getAuxLFO  (void);

   // 0=freq, 1=vol, 2=pan, 3=aux. used by PageSample / SampleLFOForm
   YM YAC_Object *_getLFOByIndex (sUI _idx);
   YM YAC_Object *_getOrCreateLFOByIndex (sUI _idx);

   YM YAC_Object *_getOrCreateFreqLFO (void);
   YM YAC_Object *_getOrCreateVolLFO  (void);
   YM YAC_Object *_getOrCreatePanLFO  (void);
   YM YAC_Object *_getOrCreateAuxLFO  (void);

   YM void        _deleteFreqLFO (void);
   YM void        _deleteVolLFO  (void);
   YM void        _deletePanLFO  (void);
   YM void        _deleteAuxLFO  (void);

   YM YAC_Object *_getModSeqByIndexAndPatch (sUI _idx, sUI _patchIdx);
   YM YAC_Object *_getOrCreateModSeqByIndexAndPatch (sUI _idx, sUI _patchIdx);
   YM void        _deleteModSeqByIndexAndPatch (sUI _idx, sUI _patchIdx);
   YM void        _setDefaultModSeqPatch (sUI _idx, sUI _patchIdx);
   YM sUI         _getDefaultModSeqPatch (sUI _idx);
   YM sUI         _findLastUsedModSeqPatch (sUI _idx);

   YM void        _allocSampleLoops       (void);
   YM YAC_Object *_getOrCreateSampleLoops (void);
   YM YAC_Object *_getSampleLoops         (void);
   YM void        _freeSampleLoops        (void);
   YM sUI         _getNumSampleLoops      (void);

   YM void  _uiSetEnableSyncFirstLoopIndex (sBool _bEnabled);
   YM sBool _uiGetEnableSyncFirstLoopIndex (void);

   YM void _setFirstLoopIndex (sSI _loopIdx);
   YM sSI  _getFirstLoopIndex (void);

   YM void _setNoteOffLoopIndex (sSI _loopIdx);
   YM sSI  _getNoteOffLoopIndex (void);
   YM void _setEnableNoteOffLoopImmediately (sSI _bEnabled);
   YM sSI  _getEnableNoteOffLoopImmediately (void);

   YM void _setEnableJumpToLoopImmediately (sSI _bEnabled);
   YM sSI  _getEnableJumpToLoopImmediately (void);

   YM void  _setPlayMode (sUI _mode);
   YM sUI   _getPlayMode (void);

   // first sampleLoops entry (lazy-alloc)
   YM sSI  _getFirstOffset (void);
   YM void _setFirstOffset (sSI _off);

   YM sSI  _getFirstLen    (void);
   YM void _setFirstLen    (sSI _len);

   // current sampleLoops entry (see firstLoopIndex)
   YM sSI  _getFirstLoopOffset (void);
   YM void _setFirstLoopOffset (sSI _off);

   YM sSI  _getFirstLoopLen    (void);
   YM void _setFirstLoopLen    (sSI _len);

   YM sSI  _getFirstLoopRep    (void);
   YM void _setFirstLoopRep    (sSI _num);

   YM sSI  _getLastLoopOffset (void);
   YM sSI  _getLastLoopLen    (void);

   // offset/len (no loops). can be (0; 0) when only using sampleLoops array
   YM sSI  _getOffset (void);
   YM void _setOffset (sSI _off);

   YM sSI  _getLen    (void);
   YM void _setLen    (sSI _len);

   // find sample waveform region. len=(maxOffset - minOffset)
   YM sUI  _getMinOffset (void);
   YM sUI  _getMaxOffset (void);

   YM void _setMaxVoices (sUI _maxVoices);
   YM sUI  _getMaxVoices (void);

   YM sF32 _getVolume (void);
   YM void _setVolume (sF32 _vol);

   YM void _setVolLFOAmt (sF32 _amt);
   YM sF32 _getVolLFOAmt (void);

   YM sF32 _getVolumeVelocityAmount (void);
   YM void _setVolumeVelocityAmount (sF32 _volVelAmount);

   YM void _setSampleOffsetRel (sF32 _pct);
   YM sF32 _getSampleOffsetRel (void);

   YM sF32 _getSampleOffsetVelocityAmount (void);
   YM void _setSampleOffsetVelocityAmount (sF32 _soffVelAmount);

   YM sSI  _getEnableInvertSampleOffsetVelocityAmount (void);
   YM void _setEnableInvertSampleOffsetVelocityAmount (sSI _bEnable);

   YM sF32 _getSampleOffsetRandAmount (void);
   YM void _setSampleOffsetRandAmount (sF32 _amt);

   YM sF32 _getPan (void);
   YM void _setPan (sF32 _pan);

   YM void _setPanEnvAmt (sF32 _amt);
   YM sF32 _getPanEnvAmt (void);

   YM void _setPanLFOAmt (sF32 _amt);
   YM sF32 _getPanLFOAmt (void);

   YM sF32 _getTranspose (void);
   YM void _setTranspose (sF32 _transpose);

   YM void _setTransposeAndUpdateOctSemiCents (sF32 _transpose);

   // (note) != 1.0 when zone/sample rate differs from (shared) waveform rate
   YM sF32 _getSampleRateRatio (void);
   YM void _setSampleRateRatio (sF32 _ratio);

  protected:
   // Called when oct/semi/cents have changed (update "transpose" field)
   void updateTranspose (void);

  public:
   YM void _setUiTransposeOct (sSI _oct);
   YM sSI  _getUiTransposeOct (void);
   YM void _setUiTransposeSemi (sSI _semi);
   YM sSI  _getUiTransposeSemi (void);
   YM void _setUiTransposeCents (sF32 _cents);
   YM sF32 _getUiTransposeCents (void);
   YM void _setTransposeAndUpdateOctSemi (sSI _key);

   YM void _setBeatFrequency (sF32 _cents);
   YM sF32 _getBeatFrequency (void);

   YM void _setEnableLerpFreq (sBool _bEnable);
   YM sBool _getEnableLerpFreq (void);

   YM void _setEnableLerpVol (sBool _bEnable);
   YM sBool _getEnableLerpVol (void);

   YM void _setFreqEnvAmt (sF32 _amt);
   YM sF32 _getFreqEnvAmt (void);

   YM void _setFreqLFOAmt (sF32 _amt);
   YM sF32 _getFreqLFOAmt (void);

   YM sF32 _getDelay (void);
   YM void _setDelay (sF32 _dly);

   YM sF32 _getDelayMultiplier (void);
   YM void _setDelayMultiplier (sF32 _mul);

   YM sF32 calcVelZoneVolume (sF32 _velocity);
   YM sF32 calcKeyModZoneVolume (sF32 _absFreq, sF32 _absMod);

   YM void setEnableTimestretch (sSI _bEnabled);
   YM sSI  getEnableTimestretch (void);

   YM void setEnableTimestretchAdditive (sSI _bEnabled);
   YM sSI  getEnableTimestretchAdditive (void);

   YM void setEnableTimestretchXFade (sSI _bEnabled);
   YM sSI  getEnableTimestretchXFade (void);

   YM void setTimestretch (sF32 _timestretch);
   YM sF32 getTimestretch (void);

   YM void setTimestretchGranularity (sF32 _granSec); // cyclelen
   YM sF32 getTimestretchGranularity (void);

   YM void setTimestretchGranularityAmount (sF32 _a);
   YM sF32 getTimestretchGranularityAmount (void);

   YM void setEnableStaticCyclelen (sSI _bEnabled);
   YM sSI  getEnableStaticCyclelen (void);

   YM void setEnableAbsoluteTimestretch (sSI _bEnabled);
   YM sSI  getEnableAbsoluteTimestretch (void);

   YM void setTimestretchInterpolType (sSI _type);
   YM sSI  getTimestretchInterpolType (void);

   YM void setTimestretchGrainWindowType (sSI _type);
   YM sSI  getTimestretchGrainWindowType (void);

   // -1..1
   YM void setTimestretchBend (sF32 _amount);
   YM sF32 getTimestretchBend (void);

   YM void setTimestretchStartPhaseRandAmount (sF32 _amount);
   YM sF32 getTimestretchStartPhaseRandAmount (void);

   YM void setTimestretch2DWidth (sUI _w);
   YM sUI  getTimestretch2DWidth (void);

   YM void setTimestretch2DHeight (sUI _h);
   YM sUI  getTimestretch2DHeight (void);

   YM void _setEnableTimedLoop (sSI _bEnable);
   YM sSI  _getEnableTimedLoop (void);
   YM void _setEnableTimedLoopFade (sSI _bEnable);
   YM sSI  _getEnableTimedLoopFade (void);
   YM void _setTimedLoopBase   (sF32 _base);
   YM sF32 _getTimedLoopBase   (void);
   YM void _setEnableOneShotLoopStep (sSI _bEnable);
   YM sSI  _getEnableOneShotLoopStep (void);

   YM void _setVolumeRampStepsMillisecIn (sF32 _millisec);
   YM sF32 _getVolumeRampStepsMillisecIn (void);

   YM void _setVolumeRampStepsMillisecOut (sF32 _millisec);
   YM sF32 _getVolumeRampStepsMillisecOut (void);

   YM void _setEnableFreezeSampleOffWhenQuiet (sSI _bEnabled);
   YM sSI  _getEnableFreezeSampleOffWhenQuiet (void);

   YM void _setEnableModShiftOrig (sSI _bEnabled);
   YM sSI  _getEnableModShiftOrig (void);

   YM void _setEnableSampleOffNoteOn (sSI _bEnabled);
   YM sSI  _getEnableSampleOffNoteOn (void);

   YM void _setEnabled (sSI _bEnabled);
   YM sSI  _getEnabled (void);

   YM void _setInterpolType (sSI _interpolType);
   YM sSI  _getInterpolType (void);

   YM void _setInterpolOrder (sUI _interpolOrder);
   YM sUI  _getInterpolOrder (void);

   YM void _setBitReductionPreAmp (sF32 _amp);
   YM sF32 _getBitReductionPreAmp (void);

   YM void  _setBitReduction (sUI _shift);
   YM sUI   _getBitReduction (void);

   YM void  _setBitReductionMask (sUI _mask);
   YM sUI   _getBitReductionMask (void);

   YM void  _setRateReduction (sUI _shift);
   YM sUI   _getRateReduction (void);

   YM void  _setRateReductionMask (sUI _mask);
   YM sUI   _getRateReductionMask (void);

   YM void  _setEnableFromStart (sBool _bEnabled);
   YM sBool _getEnableFromStart (void);

   YM YAC_Object *_getOrCreateWavepathTable (void);
   YM void        _setEnableWavepath        (sBool _bEnabled);
   YM sBool       _getEnableWavepath        (void);
   YM void        _setWavepathIndex         (sSI _idx);
   YM sSI         _getWavepathIndex         (void);

   // For editor, true=ignore ranges
   YM void _setEnableSkipRange (sSI _bEnable);

   YM void _setEnableAlt (sSI _bEnable);
   YM sSI  _getEnableAlt (void);

   YM void  _setEnableFilter (sBool _bEnable);
   YM sBool _getEnableFilter (void);

   YM void _setFilterType (sSI _fltType);
   YM sSI  _getFilterType (void);

   YM void _setFilterCutOff (sF32 _cutoff);
   YM sF32 _getFilterCutOff (void);

   YM void _setFilterPan (sF32 _pan);
   YM sF32 _getFilterPan (void);

   YM void _setFilterOffset (sF32 _off);
   YM sF32 _getFilterOffset (void);

   YM void _setFilterEQGain (sF32 _gain);
   YM sF32 _getFilterEQGain (void);

   YM void _setFilterResonance (sF32 _res);
   YM sF32 _getFilterResonance (void);

   YM void _setFilterAuxEnvAmount (sF32 _amount);
   YM sF32 _getFilterAuxEnvAmount (void);

   YM void _setFilterAuxEnvVelocityAmount (sF32 _amount);
   YM sF32 _getFilterAuxEnvVelocityAmount (void);

   YM void _setFilterAuxLFOAmount (sF32 _amount);
   YM sF32 _getFilterAuxLFOAmount (void);

   YM void _setFilterKeyboardAmount (sF32 _amount);
   YM sF32 _getFilterKeyboardAmount (void);

   YM void _setGlideType (sUI _type);
   YM sUI  _getGlideType (void);

   YM void _setGlideSpeedTimeUp (sF32 _millisec);
   YM sF32 _getGlideSpeedTimeUp (void);

   YM void _setGlideSpeedTimeDown (sF32 _millisec);
   YM sF32 _getGlideSpeedTimeDown (void);

   YM void _setGlideSpeedFreqUp (sF32 _semiPerSec);
   YM sF32 _getGlideSpeedFreqUp (void);

   YM void _setGlideSpeedFreqDown (sF32 _semiPerSec);
   YM sF32 _getGlideSpeedFreqDown (void);

   YM void  _setEnableGlideRelease (sBool _bEnable);
   YM sBool _getEnableGlideRelease (void);

   YM void _setGlideMaxNoteDist (sSI _dist);
   YM sSI  _getGlideMaxNoteDist (void);

   YM void  _setEnableGlideGlissando (sBool _bEnable);
   YM sBool _getEnableGlideGlissando (void);

   YM void _setGlideMinTime (sUI _millisec);
   YM sUI  _getGlideMinTime (void);

   YM void _setGlideMaxTime (sUI _millisec);
   YM sUI  _getGlideMaxTime (void);

   YM void  _setEnableGlideRetrigSample (sBool _bEnable);
   YM sBool _getEnableGlideRetrigSample (void);

   YM void  _setEnableGlideRetrigEnvFreq (sBool _bEnable);
   YM sBool _getEnableGlideRetrigEnvFreq (void);

   YM void  _setEnableGlideRetrigEnvVol (sBool _bEnable);
   YM sBool _getEnableGlideRetrigEnvVol (void);

   YM void  _setEnableGlideRetrigEnvPan (sBool _bEnable);
   YM sBool _getEnableGlideRetrigEnvPan (void);

   YM void  _setEnableGlideRetrigEnvAux (sBool _bEnable);
   YM sBool _getEnableGlideRetrigEnvAux (void);

   YM void  _setEnableGlideRetrigLFOFreq (sBool _bEnable);
   YM sBool _getEnableGlideRetrigLFOFreq (void);

   YM void  _setEnableGlideRetrigLFOVol (sBool _bEnable);
   YM sBool _getEnableGlideRetrigLFOVol (void);

   YM void  _setEnableGlideRetrigLFOPan (sBool _bEnable);
   YM sBool _getEnableGlideRetrigLFOPan (void);

   YM void  _setEnableGlideRetrigLFOAux (sBool _bEnable);
   YM sBool _getEnableGlideRetrigLFOAux (void);

   YM void  _setEnableGlideRetrigModSeq(sUI _idx, sBool _bEnable);
   YM sBool _getEnableGlideRetrigModSeq(sUI _idx);

   YM void  _setMMKeyboardCenter (sF32 _ctr);
   YM sF32  _getMMKeyboardCenter (void);

   YM void  _setMMKeyboardMin (sF32 _min);
   YM sF32  _getMMKeyboardMin (void);

   YM void  _setMMKeyboardMax (sF32 _max);
   YM sF32  _getMMKeyboardMax (void);

   YM void  _setMMEnable (sUI _idx, sBool _bEnable);
   YM sBool _getMMEnable (sUI _idx);

   YM void  _setMMOp (sUI _idx, sUI _op);
   YM sUI   _getMMOp (sUI _idx);

   // in 0..1 range (displayed as 0..9 in UI)
   YM void  _setMMSlewUp (sUI _idx, sF32 _amtNorm);
   YM sF32  _getMMSlewUp (sUI _idx);

   // in 0..1 range (displayed as 0..9 in UI)
   YM void  _setMMSlewDown (sUI _idx, sF32 _amtNorm);
   YM sF32  _getMMSlewDown (sUI _idx);

   // see ST_SAMPLE_MM_SLEW_RESET_xxx
   YM void  _setMMSlewReset (sUI _idx, sSI _mode);
   YM sSI   _getMMSlewReset (sUI _idx);

   YM void  _setMMSrc (sUI _idx, sUI _src);
   YM sUI   _getMMSrc (sUI _idx);

   YM void  _setMMCurve (sUI _idx, sF32 _c);
   YM sF32  _getMMCurve (sUI _idx);

   YM void  _setMMDst (sUI _idx, sUI _dst);
   YM sUI   _getMMDst (sUI _idx);

   YM void  _setMMAmt (sUI _idx, sF32 _amt);
   YM sF32  _getMMAmt (sUI _idx);

   YM void  _setMMSignalTap (sUI _idx, sBool _bEnable);
   YM sBool _getMMSignalTap (sUI _idx);

   YM void  _setLiveRecEnable (sBool _bEnable);
   YM sBool _getLiveRecEnable (void);

   YM void  _setLiveRecPause (sBool _bEnable);
   YM sBool _getLiveRecPause (void);

   YM void  _setLiveRecMonitor (sBool _bEnable);
   YM sBool _getLiveRecMonitor (void);

   YM void  _setLiveRecInput (sUI _idx);
   YM sUI   _getLiveRecInput (void);

   YM void  _setLiveRecStereo (sBool _bEnable);
   YM sBool _getLiveRecStereo (void);

   YM void  _setLiveRecInputPan (sF32 _pan);
   YM sF32  _getLiveRecInputPan (void);

   YM void  _setLiveRecLoopIdx (sUI _idx);
   YM sUI   _getLiveRecLoopIdx (void);

   YM void  _setLiveRecCopy (sBool _bEnable);
   YM sBool _getLiveRecCopy (void);

   YM void  _setLiveRecClear (sBool _bEnable);
   YM sBool _getLiveRecClear (void);

   YM void  _setLiveRecDoubleBufferMode (sUI _mode);
   YM sUI   _getLiveRecDoubleBufferMode (void);

   YM void  _setLiveRecDoubleBufferXFade (sBool _bEnable);
   YM sBool _getLiveRecDoubleBufferXFade (void);

   YM void  _setLiveRecOscMode (sUI _mode);
   YM sUI   _getLiveRecOscMode (void);

   YM void  _setLiveRecOscSingleCycle (sBool _bEnable);
   YM sBool _getLiveRecOscSingleCycle (void);

   YM void  _setLiveRecOscTune (sF32 _tune);
   YM sF32  _getLiveRecOscTune (void);

   YM void  _setLiveRecOscFadeAmt (sF32 _newAmt);
   YM sF32  _getLiveRecOscFadeAmt (void);

   YM void _setLiveRecSkipInputNumFrames (sUI _numFrames);
   YM sUI  _getLiveRecSkipInputNumFrames (void);

   sBool keepInitialOscCopyOverrideBuffer (void) const;

   YM void  _setLiveRecOscPreFilter (sF32 _cutoff01);
   YM sF32  _getLiveRecOscPreFilter (void);

   YM void  _setLiveRecOscResample (sBool _bEnable);
   YM sBool _getLiveRecOscResample (void);

   YM void  _setLiveRecOscSustain (sBool _bEnable);
   YM sBool _getLiveRecOscSustain (void);

   YM void  _setLiveRecMode (sUI _mode);
   YM sUI   _getLiveRecMode (void);

   YM void  _setLiveRecThresholdOn (sF32 _threshold);
   YM sF32  _getLiveRecThresholdOn (void);

   YM void  _setLiveRecThresholdOff (sF32 _threshold);
   YM sF32  _getLiveRecThresholdOff (void);

   YM void  _startLiveRecording (void);
   YM void   _stopLiveRecording (void);

   YM void  _setEnableSumInput (sBool _bEnable);
   YM sBool _getEnableSumInput (void);

   YM void _setSumInputXFade (sF32 _xfade);
   YM sF32 _getSumInputXFade (void);

   YM sBool hasPlugins  (void) const;
   YM sBool hasPluginsEnabled (void) const;
   YM sBool areAllPluginSlotsUsed  (void) const;
   YM void  freePlugins (void);

   // (note) sample takes ownership of 'pluginShared', i.e. the param must be wrapped in a Value object (#(deref pluginShared))
   YM void        _setVoicePluginShared (sUI _pluginIdx, YAC_Object *_pluginShared);
   YM YAC_Object *_getVoicePluginShared (sUI _pluginIdx);

   // (note) sample takes ownership of 'pluginSharedMissing', i.e. the param must be wrapped in a Value object (#(deref pluginSharedMissing))
   YM void        _setVoicePluginSharedMissing (sUI _pluginIdx, YAC_Object *_pluginSharedMissing);
   YM YAC_Object *_getVoicePluginSharedMissing (sUI _pluginIdx);

   YM void  _setEnableVoicePluginPreFilter (sUI _pluginIdx, sBool _bPreFilter);
   YM sBool _getEnableVoicePluginPreFilter (sUI _pluginIdx);

   YM void  _setVoicePluginAlg (sUI _alg);
   YM sUI   _getVoicePluginAlg (void);

   YM void  _setVoicePluginAlgFilterPos (sUI _pos);
   YM sUI   _getVoicePluginAlgFilterPos (void);

   YM void  _setVoicePluginAlgOuter (sUI _num);
   YM sUI   _getVoicePluginAlgOuter (void);

   YM void  _setEnableVoicePlugin (sUI _pluginIdx, sBool _bEnable);
   YM sBool _getEnableVoicePlugin (sUI _pluginIdx);

   YM void _setVoicePluginLevel (sUI _pluginIdx, sF32 _level);
   YM sF32 _getVoicePluginLevel (sUI _pluginIdx);

   YM void _setVoicePluginDst (sUI _pluginIdx, sUI _dst);
   YM sUI _getVoicePluginDst (sUI _pluginIdx);

   YM void _setVoicePluginPan (sUI _pluginIdx, sF32 _pan);
   YM sF32 _getVoicePluginPan (sUI _pluginIdx);

   YM void  _setEnableVoicePluginInvertPhase (sUI _pluginIdx, sBool _bInvertPhase);
   YM sBool _getEnableVoicePluginInvertPhase (sUI _pluginIdx);

   YM void swapPluginSlots (sUI _pluginIdxA, sUI _pluginIdxB);
   YM void reorderPluginSlots (YAC_Object *_ia);

   YM void  _uiSetLastPluginRowIdx (sUI _rowIdx);
   YM sUI   _uiGetLastPluginRowIdx (void);

   YM void  _setEnableFX (sBool _bEnable);
   YM sBool _getEnableFX (void);

   YM void  _setVoiceBus (sSI _voiceBus);
   YM sSI   _getVoiceBus (void);

   YM sBool uiCheckResetRedrawFlag (void);

   YM void setUiSVOffset (sF32 _offset);
   YM sF32 getUiSVOffset (void);

   YM void setUiSVZoom (YAC_Object *_zoom);
   YM void getUiSVZoom (YAC_Value *_r);

   YM void setUiSVZoomYIdx (sUI _zoomYIdx);
   YM sUI  getUiSVZoomYIdx (void);

   YM void  setUiEnableAuxToWav (sBool _bEnable);
   YM sBool getUiEnableAuxToWav (void);

   YM void setVoiceCalibrationValue (sUI _laneIdx, sUI _voiceIdx, sF32 _value);
   YM sF32 getVoiceCalibrationValue (sUI _laneIdx, sUI _voiceIdx);

   YM void setVoiceCalibrationModulo (sUI _laneIdx, sUI _modulo);
   YM sUI  getVoiceCalibrationModulo (sUI _laneIdx);

   YM void setVoiceCalibrationAmount (sUI _laneIdx, sF32 _amt);
   YM sF32 getVoiceCalibrationAmount (sUI _laneIdx);

   YM void  setVoiceCalibrationEnable (sUI _laneIdx, sBool _bEnable);
   YM sBool getVoiceCalibrationEnable (sUI _laneIdx);

   YM void setTimestretchAdditiveXFade (sF32 _amt);
   YM sF32 getTimestretchAdditiveXFade (void);

   YM void  setAdditiveCfgValid (sUI _cfgIdx, sBool _bEnable);
   YM sBool getAdditiveCfgValid (sUI _cfgIdx);

   YM void setAdditiveEditCfg (sF32 _cfgIdx);
   YM sF32 getAdditiveEditCfg (void);

   YM void  setEnableForceAdditiveEditCfg (sBool _bEnable);
   YM sBool getEnableForceAdditiveEditCfg (void);

   YM void  setAdditiveNormalize (sUI _cfgIdx, sBool _bEnable);
   YM sBool getAdditiveNormalize (sUI _cfgIdx);

   YM void setAdditiveLevel (sUI _cfgIdx, sUI _oscIdx, sF32 _amt);
   YM sF32 getAdditiveLevel (sUI _cfgIdx, sUI _oscIdx);

   YM void setAdditivePhase (sUI _cfgIdx, sUI _oscIdx, sF32 _phase);
   YM sF32 getAdditivePhase (sUI _cfgIdx, sUI _oscIdx);

   YM void setAdditiveExpA (sUI _cfgIdx, sUI _oscIdx, sF32 _exp);
   YM sF32 getAdditiveExpA (sUI _cfgIdx, sUI _oscIdx);

   YM void setAdditiveFilter (sUI _cfgIdx, sUI _oscIdx, sF32 _f);
   YM sF32 getAdditiveFilter (sUI _cfgIdx, sUI _oscIdx);

   YM void setAdditiveExpL (sUI _cfgIdx, sUI _oscIdx, sF32 _exp);
   YM sF32 getAdditiveExpL (sUI _cfgIdx, sUI _oscIdx);

   YM void _setAdditiveBitReductionPreAmp (sUI _cfgIdx, sF32 _amp);
   YM sF32 _getAdditiveBitReductionPreAmp (sUI _cfgIdx);

   YM void  _setAdditiveBitReduction (sUI _cfgIdx, sUI _shift);
   YM sUI   _getAdditiveBitReduction (sUI _cfgIdx);

   YM void  _setAdditiveBitReductionMask (sUI _cfgIdx, sUI _mask);
   YM sUI   _getAdditiveBitReductionMask (sUI _cfgIdx);

   YM void  _setAdditiveRateReduction (sUI _cfgIdx, sUI _shift);
   YM sUI   _getAdditiveRateReduction (sUI _cfgIdx);

   YM void  _setAdditiveRateReductionMask (sUI _cfgIdx, sUI _mask);
   YM sUI   _getAdditiveRateReductionMask (sUI _cfgIdx);

   YM void _setAdditiveStereoSpread (sF32 _amt);
   YM sF32 _getAdditiveStereoSpread (void);

   void recalcAdditiveTblCfg (sUI _cfgIdx);

   // cfgIdx=-1 => recalc all tables
   void recalcAdditiveTbl_int (sSI _cfgIdx, sF32 *_dOrNull, sF32 _fIdx);
   YM void recalcAdditiveTbl (sSI _cfgIdx);

   YM void exportAdditiveWavetable (YAC_FloatArray *_d, sUI _numCh, sUI _outCycleLen, sF32 _rateFactor, sUI _numWaves);

   YM void  _setAiNumPoles (sUI _num);
   YM sBool _getAiNumPoles (void);

   YM void _setAiQ (sF32 _q);
   YM sF32 _getAiQ (void);

   YM void _setAiExpUp (sF32 _exp);
   YM sF32 _getAiExpUp (void);

   YM void _setAiExpDown (sF32 _exp);
   YM sF32 _getAiExpDown (void);

   YM void _setAiLinOct (sF32 _oct);
   YM sF32 _getAiLinOct (void);

   YM void _setAiLinMax (sF32 _max);
   YM sF32 _getAiLinMax (void);

   YM void _setStartEndFadeNumFrames (sUI _numFrames);
   YM sUI  _getStartEndFadeNumFrames (void);

   YM void  _uiSetEnableAutoSelect (sBool _bEnable);
   YM sBool _uiGetEnableAutoSelect (void);

   // Lazy-alloc tuning table and copy MIDI note frequencies from FloatArray 'fa'.
   //  Frees tuning table when 'fa' is null.
   YM sBool _setTuningTable (sUI _idx, YAC_Object *_fa);

   // Returns a new FloatArray that references the table data,
   //  or null when the table is not allocated or idx is out of bounds
   YM void _getTuningTable (sUI _idx, YAC_Value *_r);

   YM void _setDefaultTuningTableIdx (sSI _idx);
   YM sSI  _getDefaultTuningTableIdx (void);

   YM void _setForcedTuningTableIdx (sSI _idx);
   YM sSI  _getForcedTuningTableIdx (void);

   // Editor info (deletable SampleTuningTable script object)
   YM void        _setTuningTableMetaData (sUI _idx, YAC_Object *_voOrNull);
   YM YAC_Object *_getTuningTableMetaData (sUI _idx);

   YM void _freeTuningTables (void);
   YM void _freeTuningTablesMetaData (void);

   YM void  _mmVarSetEnable (sBool _bEnable);
   YM sBool _mmVarGetEnable (void);

   YM void  _mmVarSetNum (sUI _num);
   YM sUI   _mmVarGetNum (void);

   YM void  _mmVarSetEnableSmooth (sBool _bEnable);
   YM sBool _mmVarGetEnableSmooth (void);

   YM void  _mmVarSetEditIdx (sUI _editIdx);
   YM sUI   _mmVarGetEditIdx (void);

   YM void  _mmVarSetEnableForceEditIdx (sBool _bEnable);
   YM sBool _mmVarGetEnableForceEditIdx (void);

   // Returns new FloatArray object that references mmvar_data
   YM void  _mmVarGetData (YAC_Value *_r);

};


#endif // ST_SAMPLE_H__
