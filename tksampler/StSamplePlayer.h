// ----
// ---- file   : StSamplePlayer.h
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
// ----          19Jan2010, 21Jan2010, 01Mar2010, 29Apr2010, 28Jun2010, 29Jun2010, 01Jul2010
// ----          05Sep2010, 09Sep2010, 10Mar2011, 28Mar2011, 13Dec2018, 29Dec2018, 05Jan2019
// ----          23Mar2019, 24Mar2019, 30Mar2019, 12May2019, 15May2019, 06Jul2019, 13Jul2019
// ----          22Jul2019, 25Jul2019, 01Aug2019, 15Feb2020, 16Feb2020, 20Feb2020, 21Feb2020
// ----          22Feb2020, 04Mar2020, 17May2020, 19May2020, 24May2020, 08Jun2020, 12Sep2020
// ----          28Apr2021, 20May2021, 17Jul2021, 01Aug2021, 10Aug2021, 30Aug2021, 10Dec2022
// ----          20Dec2022, 12Apr2023, 20Aug2023, 07Sep2023, 21Jan2024, 28Sep2024, 01Oct2024
// ----          03Oct2024, 31Oct2024, 15Nov2024, 14Jan2025, 28May2025, 29May2025, 30May2025
// ----          13Jun2025
// ----
// ----
// ----

#ifndef ST_SAMPLEPLAYER_H__
#define ST_SAMPLEPLAYER_H__

YG("tksampler");


/* @class StSamplePlayer,Object
  Manages a finite number of §StSampleVoice objects
 */

YC class StSamplePlayer : public YAC_Object {


/* @constant STSAMPLEPLAYER_MAXVOICES,int
 */
#define STSAMPLEPLAYER_MAXVOICES YCI 256


/* @constant STSAMPLEPLAYER_POLYMODE_OLDEST,int
 */
#define STSAMPLEPLAYER_POLYMODE_OLDEST  YCI 0


/* @constant STSAMPLEPLAYER_POLYMODE_LOWEST,int
 */
#define STSAMPLEPLAYER_POLYMODE_LOWEST  YCI 1


/* @constant STSAMPLEPLAYER_POLYMODE_HIGHEST,int
 */
#define STSAMPLEPLAYER_POLYMODE_HIGHEST YCI 2


/* @constant STSAMPLEPLAYER_NUM_POLYMODES,int
 */
#define STSAMPLEPLAYER_NUM_POLYMODES    YCI 3


#define NOTE_HISTORY_LEN       (32u)
#define NOTE_HISTORY_LEN_MASK  (31u)


  public:
   StSampleVoice **voices; // StSampleVoice instances
   sUI num_voices;

   sSI *voice_order;  // -1=end of array (no more active voices)

   tksampler_lfsr_t lfsr;

   sF32 volume; // post-sample scale factor, 1=no scaling

   sUI poly_mode; // Voice alloc. strategy, See STSAMPLEPLAYER_POLYMODE_xxx

   sF32            default_mixrate;
   YAC_FloatArray *default_freq_table;  // must not be NULL. 128 (MIDI note) frequencies.

   sUI next_voice_key;        // monotonically increasing each time a voice is allocated or reallocated
   sUI next_voice_alloc_idx;  // monotonically increasing each time a voice is allocated (_not_ reallocated). used for e.g. voice calibration tables

   sBool b_glide_switch;

   sF32 mod_vol;  // all voices modifiers
   sF32 mod_pan;
   sF32 mod_freq;   // e.g. pitch randomization
   sF32 mod_freq2;  // e.g. pitch bend
   sF32 mod_mod;
   sF32 mod_timestretch;
   sF32 mod_timestretch_bend;
   sF32 mod_sampleoff;
   sF32 mod_cyclelen;
   sF32 mod_sampleshift;
   sF32 mod_sampleshift_endofloop;
   sF32 mod_altvolume;

   sF32 mod_lfo_freq_amt;
   sF32 mod_lfo_vol_amt;
   sF32 mod_lfo_pan_amt;
   sF32 mod_lfo_aux_flt_amt;

   sF32 mod_lfo_freq_spd;  // *
   sF32 mod_lfo_freq_lvl;  // +

   sF32 mod_lfo_vol_spd;
   sF32 mod_lfo_vol_lvl;

   sF32 mod_lfo_pan_spd;
   sF32 mod_lfo_pan_lvl;

   sF32 mod_lfo_aux_spd;
   sF32 mod_lfo_aux_lvl;

   sF32 mod_adsr_freq_spd;   // *
   sF32 mod_adsr_freq_lvl;   // +
   sF32 mod_adsr_freq_int;   // +
   sF32 mod_adsr_freq_aspd;  // *
   sF32 mod_adsr_freq_rspd;  // *
   sF32 mod_adsr_freq_sspd;  // *

   sF32 mod_adsr_vol_spd;    // *
   sF32 mod_adsr_vol_lvl;    // +
   sF32 mod_adsr_vol_int;    // +
   sF32 mod_adsr_vol_aspd;   // *
   sF32 mod_adsr_vol_rspd;   // *
   sF32 mod_adsr_vol_sspd;   // *

   sF32 mod_adsr_pan_spd;    // *
   sF32 mod_adsr_pan_lvl;    // +
   sF32 mod_adsr_pan_int;    // +
   sF32 mod_adsr_pan_aspd;   // *
   sF32 mod_adsr_pan_rspd;   // *
   sF32 mod_adsr_pan_sspd;   // *

   sF32 mod_adsr_aux_spd;    // *
   sF32 mod_adsr_aux_lvl;    // +
   sF32 mod_adsr_aux_int;    // +
   sF32 mod_adsr_aux_aspd;   // *
   sF32 mod_adsr_aux_rspd;   // *
   sF32 mod_adsr_aux_sspd;   // *

   sF32 mod_filter_cutoff;     // +
   sF32 mod_filter_pan;        // +
   sF32 mod_filter_offset;     // +
   sF32 mod_filter_resonance;  // +

   struct {
      sF32 speed;     // *
      sF32 level;     // +
      sF32 numsteps;  // *
      sF32 advance;   // +
   } mod_modseq[STSAMPLE_NUM_MODSEQ];

#ifndef LIBSYNERGY_BUILD
   sSI mod_liverec_loop_shift; // +
#endif // LIBSYNERGY_BUILD

   sF32 perf_ctl[STSAMPLEPLAYER_NUM_PERFCTL];  // see StSampleVoice

   sBool sustain_pedal;
   sBool soft_pedal;

   sF32 float_block_off;
   sF32 float_block_size;
   sUI  global_tick_nr;
   sUI  global_tick_advance;  // valid during render()

   // used for highlighting last pressed drumpads (PageSample UI)
   sUI note_history_ms[NOTE_HISTORY_LEN];
   sSI note_history_note[NOTE_HISTORY_LEN];
   sUI note_history_write_idx;

   sF32 bpm;
   sSI  ppq;
   sF32 bpm_audiotick_scl;  // 1 audiotick=1ms, 1 seq tick @125bpm/192ppq = ~2.5ms

   StSampleBank *last_started_samplebank;   // reset to NULL in resetVoices()

#ifndef LIBSYNERGY_BUILD
   YAC_MutexHandle mtx_process_tick_nr;
#endif // LIBSYNERGY_BUILD

   // (todo) optimize cache search
   struct {
      st_plugin_cache_entry_t *first;
   } plugin_cache;

  public:
   StSamplePlayer(void);
   ~StSamplePlayer();

   YAC(StSamplePlayer);

   void freeVoices (void);

   void sortVoices (void);

   st_plugin_cache_entry_t *findOrCreatePluginCacheEntry (st_plugin_info_t *_info, sUI _voiceIdx);
   void lazyCreateVoicePlugins (StSample *_sample, StSampleVoice *_nv);
   void freePluginCache (void);

   void addToNoteHistory (sSI _note);

   void applyMaxVoicesConstraint (StSample *_sample, sUI _voiceKey);
   void applyMaxSampleBankVoicesConstraint (StSampleBank *_sb);

   void stopSampleBankNotes (StSampleBank *_sb, sSI _note);

   StSampleVoice *findBestMatchVoice (sUI _polyMode, sBool _bFadedOnly, sBool _bSoftStoppedOnly, sBool _bReleaseOnly, sBool _bForce);
   StSampleVoice *allocSampleVoice (StSample *_sample, sSI _note);
   StSampleVoice *allocSampleVoiceGlide (StSample *_sample, sSI _note);

   void renderInt (YAC_FloatArray *buf, const sF32*const*_inputsOrNull);

#ifndef LIBSYNERGY_BUILD
   void handleLiveRecording (sF32 *_out, const sF32 *const*_inputs, sUI _numFrames, sUI _processTickNr);
   void stopOtherLiveRecordingsExceptFor (StSample *_c);

   void mtxLazyInitProcessTickNr (void);
   void mtxLockProcessTickNr (void);
   void mtxUnlockProcessTickNr (void);
#endif // LIBSYNERGY_BUILD

   StSampleVoice *findActiveVoiceBySample (StSample *_s, sBool _bAllowInRelease);
   StSampleVoice *findVoiceByKeyAndSample (sSI _key, StSample *_s);


   /* @method resetModulator
      Reset all modulators
   */
   YM void resetModulators (void);

   /* @method allocVoices,int maxPolyphony:boolean
      Allocate internal voice pool

     @arg mayPolyphony Number of voices to allocate
     @return 1(true) if the allocation succeeded, 0(false) otherwise.
   */
   YM sSI allocVoices (sUI _maxPolyphony);


   /* @method getNumVoices:int
      Query polyphony
   */
   YM sUI getNumVoices (void);


   /* @method getNumUsedVoices:int
      Query number of currently used voices
   */
   YM sUI getNumUsedVoices (void);


   /* @method getNumActiveVoices:int
      Query number of currently used (allocated) voices
   */
   YM sUI getNumActiveVoices (void);


   /* @method getNumPlayingVoices:int
      Query number of currently used (allocated+playing) voices
   */
   YM sUI getNumPlayingVoices (void);


   /* @method getNumActiveNotes:int
      Query number of currently (pressed) notes
   */
   YM sUI getNumActiveNotes (void);


   /* @method getVoice,int idx:StSampleVoice
      Query voice object
   */
   YM YAC_Object *_getVoice (sUI _idx);


   /* @method findFirstActiveSampleVoice,StSample sample:StSampleVoice
      Query voice object
   */
   YM YAC_Object *_findFirstActiveVoiceBySample (YAC_Object *_sample);


   /* @method findLastStartedVoice,StSample sample:StSampleVoice
      Query voice object
   */
   YM YAC_Object *_findLastStartedVoiceBySample (YAC_Object *_sample);


   /* @method setDefaultMixRate,float mixRate
      Set default mixrate for new sample voices

      (initial value is 48000Hz)
   */
   YM void setDefaultMixRate (sF32 _mixRate);


   /* @method getDefaultMixRate:float
      Query default mixrate
   */
   YM sF32 getDefaultMixRate (void);


   /* @method setDefaultFreqTable,FloatArray freqTable
      Set default frequency table for new sample voices

      This table is used when the freqTable argument in calls to §startSample or §startSampleBank is null
       and the sample bank's default_tuning_table_idx / forced_tuning_table_idx does ot map to a valid table.
   */
   YM void setDefaultFreqTable (YAC_Object *_freqTable);


   /* @method getDefaultFreqTable:FloatArray
      Query default frequency table
   */
   YM YAC_Object *getDefaultFreqTable (void);


   /* @method setVolume,float volume
    */
   YM void setVolume (sF32 _volume);


   /* @method getVolume:float
    */
   YM sF32 getVolume (void);


   /* @method setPolyMode,int mode
      Select voice allocation strategy

      @mode See STSAMPLEPLAYER_POLYMODE_xxx constants, e.g. STSAMPLEPLAYER_POLYMODE_OLDEST
   */
   YM void setPolyMode (sUI _mode);


   /* @method getPolyMode:int
      Query voice allocation strategy

      @return Current poly mode (default = §STSAMPLEPLAYER_POLYMODE_OLDEST)
   */
   YM sUI getPolyMode (void);


   /* @method allNotesOff
      Stop all currently playing voices (with fadeout but no release)
   */
   YM void allNotesOff (void);


   /* @method allNotesOffRelease
      Release all currently playing voices (like regular note offs)
   */
   YM void allNotesOffRelease (void);


   /* @method reset
      Immediately reset / stop all voices (no fadeout)
   */
   YM void resetVoices (void);


   /* @method stopVoicesBySample,StSample sample
      Immediately stop all voices that play the given sample (no fadeout)
      This is called when modifying the sample's waveform.
   */
   YM void resetVoicesBySample (YAC_Object *_sample);


   /* @method stopVoicesByKey,int voiceKey
      Immediately stop all voices that are assigned the given key (no fadeout)
      This is called when muting a track.
   */
   YM void stopVoicesByKey (sSI _key);


   /* @method stopVoicesBySample,StSample sample
      Stop all voices that play the given sample (with fadeout)
   */
   YM void stopVoicesBySample (YAC_Object *_sample);


   /* @method stopLFOVolBySample,StSample sample
      Immediately stop all volume LFOs that belong to voices that play the given sample.
      This is called when deleting an LFO in the UI while the voice is playing.
   */
   YM void stopLFOVolBySample (YAC_Object *_sample);


   /* @method stopLFOPanBySample,StSample sample
      Immediately stop all pan LFOs that belong to voices that play the given sample.
      This is called when deleting an LFO in the UI while the voice is playing.
   */
   YM void stopLFOPanBySample (YAC_Object *_sample);


   /* @method stopLFOFreqBySample,StSample sample
      Immediately stop all frequency LFOs that belong to voices that play the given sample.
      This is called when deleting an LFO in the UI while the voice is playing.
   */
   YM void stopLFOFreqBySample (YAC_Object *_sample);


   /* @method stopADSRVolBySample,StSample sample
      Immediately stop all volume ADSRs that belong to voices that play the given sample.
      This is called when deleting an ADSR in the UI while the voice is playing.
   */
   YM void stopADSRVolBySample (YAC_Object *_sample);


   /* @method stopADSRPanBySample,StSample sample
      Immediately stop all pan ADSRs that belong to voices that play the given sample.
      This is called when deleting an LFO in the UI while the voice is playing.
   */
   YM void stopADSRPanBySample (YAC_Object *_sample);


   /* @method stopADSRFreqBySample,StSample sample
      Immediately stop all frequency ADSRs that belong to voices that play the given sample.
      This is called when deleting an ADSR in the UI while the voice is playing.
   */
   YM void stopADSRFreqBySample (YAC_Object *_sample);


   /* @method noteOff,int note,float velocity:int
      Begin release phase of all voices that were triggered on the given key

      @arg note MIDI note (0..127)
      @arg velocity Note-off velocity (0..1)
      @return Number of voices affected (0..n)
   */
   YM sSI noteOff (sSI _note, sF32 velocity);


   /* @method noteOffByKey,int voiceKey,float velocity:int
      Begin release phase of all voices that share the given voice key

      @arg voiceKey Voice key
      @arg velocity Note-off velocity (0..1)
      @return Number of voices affected (0..n)
   */
   YM sSI noteOffByKey (sSI _key, sF32 velocity);


   /* @method startSample,StSample sample,FloatArray freqTableOrNull,int note,float velocity,float mod:int
      Allocate voice and prepare to play the given sample.

      Any voice that plays a sample belonging to the same mutex group will be canceled.

      initStartedVoicesByKey() must be called to actually initialize the voice.

      @arg sample Which sample to play
      @arg freqTable Frequency table override (scale)
      @arg note Which note to play (0..127)
      @arg velocity Note-on velocity (0..1)
      @arg mod Modulation
      @return Voice key (incremental)

      @see startSampleBank
      @see noteOff
      @see allNotesOff
   */
   YM sSI startSample (YAC_Object *_sample, YAC_Object *_freqTableOrNull, sSI _note, sF32 _velocity, sF32 _mod, sF32 _vol, sF32 _pan, sF32 _freq);


   /* @method startSampleBank,StSampleBank sampleBank,FloatArray freqTableOrNull,int note,float velocity,float mod:int
      Allocate voice(s) and play the samplebank samples that match the given note/velo/mod zones.

      Any voice that plays a sample belonging to the same mutex group(s) will be canceled.

      initStartedVoicesByKey() must be called to actually initialize the voice.

      @arg sample Which sample to play
      @arg freqTableOrNull Frequency table override (scale)
      @arg note Which note to play (0..127)
      @arg velocity Note-on velocity (0..1)
      @arg mod Modulation
      @return Voice key (incremental)

      @see startSample
      @see noteOff
      @see allNotesOff
   */
   YM sSI startSampleBank (YAC_Object *_sampleBank, YAC_Object *_freqTableOrNull, sSI _note, sF32 _velocity, sF32 _mod, sF32 _vol, sF32 _pan, sF32 _freq);


   /* @method findVoicesByKey,int key,PointerArray ret:int
      Find all voices that are assigned the given key.

      @return Number of voices added to array.
   */
   YM sSI _findVoicesByKey (sSI _key, YAC_Object *_retPA);


   /* @method findPreviousGlideNoteByKey,int key:int historyIdx:int
      Return -1 or glide_prev_note for first gliding voice that is assigned the given voicekey.

      @arg key voice key
      @arg historyIdx 0=newest, 'n'=oldest
   */
   YM sSI _findPreviousGlideNoteByKeyAndHistoryIdx (sSI _key, sSI _historyIdx);


   /* @method removeFromGlideNoteHistory,int note,boolean bLastOnly
      Iterate voices and remove given note from glide note histories

      @arg bLastOnly Only remove when note is latest in history
      @return true if one or more voice histories have changed
   */
   YM sBool _removeFromGlideNoteHistory (sSI _note, sBool _bLastOnly);


   /* @method initStartedVoicesByKey,int key
      Really trigger note-on. Should be called after (initial) params have been set.
    */
   YM void _initStartedVoicesByKey (sSI _key);


   /* @method restartVoicesByKey,int key
      Reset playing voices.
    */
   YM void _restartVoicesByKey (sSI _key);


   /* @method render,FloatArray buf
      Render currently playing voices to the given (stereo) buffer (add).

      @arg buf Render buffer. numElements/2 determines the number of samples to render.
   */
   YM void _render (YAC_Object *_fa);


   /* @method renderWithInputs,FloatArray buf,PointerArray paInputs,boolean bRender
      Render currently playing voices to the given (stereo) buffer (add).

      @arg buf Render buffer. numElements/2 determines the number of samples to render.
      @arg paInputs PointerArray of FloatArrays (mono inputs, up to STSAMPLE_MAX_INPUTS (4) FloatArrays)
      @arg bRender 1=generate outputs, 0=process inputs only
      @arg processTickNr Used to limit input processing to one sampleplayer per samplebank
   */
   YM void _renderWithInputs (YAC_Object *_fa, YAC_Object *_paInputs, sBool _bRender, sUI _processTickNr);

   /* @method updateVol,float v
      Update volume modulation of all active voices
   */
   YM void updateVol (sF32 _v);
   YM void updateVolByKey (sSI _voiceKey, sF32 _v);


   /* @method updatePan,float v
      Update panning modulation of all active voices
   */
   YM void updatePan (sF32 _v);
   YM void updatePanByKey (sSI _voiceKey, sF32 _v);


   /* @method updateFreq,float v
      Update frequency modulation of all active voices
   */
   YM void updateFreq (sF32 _v);
   YM void updateFreqByKey (sSI _voiceKey, sF32 _v);


   /* @method updateFreq2,float v
      Update frequency modulation #2 of all active voices
   */
   YM void updateFreq2 (sF32 _v);
   YM void updateFreq2ByKey (sSI _voiceKey, sF32 _v);



   /* @method updateMod,float v
      Update modulation of all active voices
   */
   YM void updateMod (sF32 _v);
   YM void updateModByKey (sSI _voiceKey, sF32 _v);


   /* @method updateTimestretch,float v
      Update timestretch modulation of all active voices
   */
   YM void updateTimestretch (sF32 _v);
   YM void updateTimestretchByKey (sSI _voiceKey, sF32 _v);


   /* @method updateTimestretchBend,float v
      Update timestretch phase bend modulation of all active voices
   */
   YM void updateTimestretchBend (sF32 _v);
   YM void updateTimestretchBendByKey (sSI _voiceKey, sF32 _v);



   /* @method updateSampleOff,float v
      Update sample offset modulation of all active voices
   */
   YM void updateSampleOff (sF32 _v);
   YM void updateSampleOffByKey (sSI _voiceKey, sF32 _v);
   YM void setInitialSampleOffsetMsByKey (sSI _voiceKey, sF32 _ms);


   /* @method updateCycleLen,float v
      Update timestretch granularity modulation of all active voices
   */
   YM void updateCycleLen (sF32 _v);
   YM void updateCycleLenByKey (sSI _voiceKey, sF32 _v);



   /* @method updateSampleShift,float v
      Update sample shift modulation of all active voices
   */
   YM void updateSampleShift (sF32 _v);
   YM void updateSampleShiftByKey (sSI _voiceKey, sF32 _v);


   /* @method updateSampleShiftEndOfLoop,float v
      Update sample shift modulation of all active voices (queue until end of loop)
   */
   YM void updateSampleShiftEndOfLoop (sF32 _v);
   YM void updateSampleShiftEndOfLoopByKey (sSI _voiceKey, sF32 _v);


   /* @method updateAltVolume,float v
      Update alternative volume modulation of all active voices
   */
   YM void updateAltVolume (sF32 _v);
   YM void updateAltVolumeByKey (sSI _voiceKey, sF32 _v);


   /* @method updateRetrigMask,int v
      Retrigger envelope/LFO of all active voices
         0x01: reset freq envelope
         0x02: reset volume envelope
         0x04: reset pan envelope
         0x08: reset aux envelope
         0x10: reset freq LFO
         0x20: reset volume LFO
         0x40: reset pan LFO
         0x80: reset aux LFO
   */
   YM void updateRetrigMask (sUI _v);
   YM void updateRetrigMaskByKey (sSI _voiceKey, sUI _v);


   /* @method updateLFOFreqSpd,float spd
      Update frequency LFO speed of all active voices
   */
   YM void updateLFOFreqSpd (sF32 _spd);
   YM void updateLFOFreqSpdByKey (sSI _voiceKey, sF32 _spd);


   /* @method updateLFOFreqLvl,float lvl
      Update frequency LFO level of all active voices
   */
   YM void updateLFOFreqLvl (sF32 _lvl);
   YM void updateLFOFreqLvlByKey (sSI _voiceKey, sF32 _lvl);


   /* @method updateLFOFreqAmt,float amt
      Update frequency LFO => frequency amount modifier
    */
   YM void updateLFOFreqAmt (sF32 _amt);
   YM void updateLFOFreqAmtByKey (sSI _voiceKey, sF32 _amt);


   /* @method updateLFOVolSpd,float spd
      Update volume LFO speed of all active voices
   */
   YM void updateLFOVolSpd (sF32 _spd);
   YM void updateLFOVolSpdByKey (sSI _voiceKey, sF32 _spd);


   /* @method updateLFOVolLvl,float lvl
      Update volume LFO level of all active voices
   */
   YM void updateLFOVolLvl (sF32 _lvl);
   YM void updateLFOVolLvlByKey (sSI _voiceKey, sF32 _lvl);


   /* @method updateLFOVolAmt,float amt
      Update volume LFO => volume amount modifier
    */
   YM void updateLFOVolAmt (sF32 _amt);
   YM void updateLFOVolAmtByKey (sSI _voiceKey, sF32 _amt);


   /* @method updateLFOPanSpd,float spd
      Update pan LFO speed of all active voices
   */
   YM void updateLFOPanSpd (sF32 _spd);
   YM void updateLFOPanSpdByKey (sSI _voiceKey, sF32 _spd);


   /* @method updateLFOPanLvl,float lvl
      Update pan LFO level of all active voices
   */
   YM void updateLFOPanLvl (sF32 _lvl);
   YM void updateLFOPanLvlByKey (sSI _voiceKey, sF32 _lvl);


   /* @method updateLFOPanAmt,float amt
      Update pan LFO => pan amount modifier
    */
   YM void updateLFOPanAmt (sF32 _amt);
   YM void updateLFOPanAmtByKey (sSI _voiceKey, sF32 _amt);


   /* @method updateLFOAuxSpd,float spd
      Update aux LFO speed of all active voices
   */
   YM void updateLFOAuxSpd (sF32 _spd);
   YM void updateLFOAuxSpdByKey (sSI _voiceKey, sF32 _spd);


   /* @method updateLFOAuxLvl,float lvl
      Update aux LFO level of all active voices
   */
   YM void updateLFOAuxLvl (sF32 _lvl);
   YM void updateLFOAuxLvlByKey (sSI _voiceKey, sF32 _lvl);


   /* @method updateLFOAuxFltAmt,float amt
      Update aux LFO => filter cutoff amount modifier
    */
   YM void updateLFOAuxFltAmt (sF32 _amt);
   YM void updateLFOAuxFltAmtByKey (sSI _voiceKey, sF32 _amt);


   /* @method updateADSRFreqSpd,float spd
      Update frequency ADSR speed of all active voices
   */
   YM void updateADSRFreqSpd (sF32 _spd);
   YM void updateADSRFreqSpdByKey (sSI _voiceKey, sF32 _spd);


   /* @method updateADSRFreqLvl,float lvl
      Update frequency ADSR level of all active voices
   */
   YM void updateADSRFreqLvl (sF32 _lvl);
   YM void updateADSRFreqLvlByKey (sSI _voiceKey, sF32 _lvl);


   /* @method updateADSFreqInt,float int
      Update frequency ADSR intensity of all active voices
   */
   YM void updateADSRFreqInt (sF32 _int);
   YM void updateADSRFreqIntByKey (sSI _voiceKey, sF32 _int);


   /* @method updateADSRVolSpd,float spd
      Update volume ADSR speed of all active voices
   */
   YM void updateADSRVolSpd (sF32 _spd);
   YM void updateADSRVolSpdByKey (sSI _voiceKey, sF32 _spd);


   /* @method updateADSRVolLvl,float lvl
      Update volume ADSR level of all active voices
   */
   YM void updateADSRVolLvl (sF32 _lvl);
   YM void updateADSRVolLvlByKey (sSI _voiceKey, sF32 _lvl);


   /* @method updateADSRVolInt,float int
      Update volume ADSR intensity of all active voices
   */
   YM void updateADSRVolInt (sF32 _int);
   YM void updateADSRVolIntByKey (sSI _voiceKey, sF32 _int);


   /* @method updateADSRPanSpd,float spd
      Update pan ADSR speed of all active voices
   */
   YM void updateADSRPanSpd (sF32 _spd);
   YM void updateADSRPanSpdByKey (sSI _voiceKey, sF32 _spd);


   /* @method updateADSRPanLvl,float lvl
      Update pan ADSR level of all active voices
   */
   YM void updateADSRPanLvl (sF32 _lvl);
   YM void updateADSRPanLvlByKey (sSI _voiceKey, sF32 _lvl);


   /* @method updateADSRPanInt,float int
      Update pan ADSR intensity of all active voices
   */
   YM void updateADSRPanInt (sF32 _int);
   YM void updateADSRPanIntByKey (sSI _voiceKey, sF32 _int);


   /* @method updateADSRAuxSpd,float spd
      Update aux ADSR speed of all active voices
   */
   YM void updateADSRAuxSpd (sF32 _spd);
   YM void updateADSRAuxSpdByKey (sSI _voiceKey, sF32 _spd);


   /* @method updateADSRAuxLvl,float lvl
      Update aux ADSR level of all active voices
   */
   YM void updateADSRAuxLvl (sF32 _lvl);
   YM void updateADSRAuxLvlByKey (sSI _voiceKey, sF32 _lvl);


   /* @method updateADSRAuxInt,float int
      Update aux ADSR intensity of all active voices
   */
   YM void updateADSRAuxInt (sF32 _int);
   YM void updateADSRAuxIntByKey (sSI _voiceKey, sF32 _int);


   /* @method updateJumpToLoop,int idx
      Make all playing voices jump to the given loop
   */
   YM void updateJumpToLoop (sSI _idx);
   YM void updateJumpToLoopByKey (sSI _voiceKey, sSI _idx);


   /* @method updateJumpToLoopRel,int idx
      Make all playing voices jump to the given loop (0..1 => 0..num_loops-1)
   */
   YM void updateJumpToLoopRel (sF32 _relIdx);
   YM void updateJumpToLoopRelByKey (sSI _voiceKey, sF32 _relIdx);


   /* @method updateWavepathIndex,int idx
      Make all playing voices jump to the given cycle index (granular mode)
   */
   YM void updateWavepathIndex (sSI _idx);
   YM void updateWavepathIndexByKey (sSI _voiceKey, sSI _idx);


   /* @method updateAFreqSpd,float int
      Update freq attack speed of all active voices
   */
   YM void updateAFreqSpd (sF32 _spd);
   YM void updateAFreqSpdByKey (sSI _voiceKey, sF32 _spd);


   /* @method updateSFreqSpd,float int
      Update freq sustain speed of all active voices
   */
   YM void updateSFreqSpd (sF32 _spd);
   YM void updateSFreqSpdByKey (sSI _voiceKey, sF32 _spd);


   /* @method updateRFreqSpd,float int
      Update freq release speed of all active voices
   */
   YM void updateRFreqSpd (sF32 _spd);
   YM void updateRFreqSpdByKey (sSI _voiceKey, sF32 _spd);


   /* @method updateAVolSpd,float int
      Update volume attack speed of all active voices
   */
   YM void updateAVolSpd (sF32 _spd);
   YM void updateAVolSpdByKey (sSI _voiceKey, sF32 _spd);


   /* @method updateSVolSpd,float int
      Update volume sustain speed of all active voices
   */
   YM void updateSVolSpd (sF32 _spd);
   YM void updateSVolSpdByKey (sSI _voiceKey, sF32 _spd);


   /* @method updateRVolSpd,float int
      Update volume release speed of all active voices
   */
   YM void updateRVolSpd (sF32 _spd);
   YM void updateRVolSpdByKey (sSI _voiceKey, sF32 _spd);


   /* @method updateAPanSpd,float int
      Update pan attack speed of all active voices
   */
   YM void updateAPanSpd (sF32 _spd);
   YM void updateAPanSpdByKey (sSI _voiceKey, sF32 _spd);


   /* @method updateSPanSpd,float int
      Update pan sustain speed of all active voices
   */
   YM void updateSPanSpd (sF32 _spd);
   YM void updateSPanSpdByKey (sSI _voiceKey, sF32 _spd);


   /* @method updateRPanSpd,float int
      Update pan release speed of all active voices
   */
   YM void updateRPanSpd (sF32 _spd);
   YM void updateRPanSpdByKey (sSI _voiceKey, sF32 _spd);


   /* @method updateAAuxSpd,float int
      Update aux attack speed of all active voices
   */
   YM void updateAAuxSpd (sF32 _spd);
   YM void updateAAuxSpdByKey (sSI _voiceKey, sF32 _spd);


   /* @method updateSAuxSpd,float int
      Update aux sustain speed of all active voices
   */
   YM void updateSAuxSpd (sF32 _spd);
   YM void updateSAuxSpdByKey (sSI _voiceKey, sF32 _spd);


   /* @method updateRAuxSpd,float int
      Update aux release speed of all active voices
   */
   YM void updateRAuxSpd (sF32 _spd);
   YM void updateRAuxSpdByKey (sSI _voiceKey, sF32 _spd);


   /* @method updateGlideSwitch,boolean bEnable
      Update glide switch of all active voices
   */
   YM void updateGlideSwitch (sBool _bEnable);
   YM void updateGlideSwitchByKey (sSI _voiceKey, sBool _bEnable);


   /* @method updateGlideSpeed,float speed
      Update glide speed of all active voices
   */
   YM void updateGlideSpeed (sF32 _speed);
   YM void updateGlideSpeedByKey (sSI _voiceKey, sF32 _speed);


   /* @method updateFltCutOff,float f
      Update filter cutoff of all active voices
   */
   YM void updateFltCutOff (sF32 _f);
   YM void updateFltCutOffByKey (sSI _voiceKey, sF32 _f);


   /* @method updateFltPan,float f
      Update filter panning of all active voices
   */
   YM void updateFltPan (sF32 _f);
   YM void updateFltPanByKey (sSI _voiceKey, sF32 _f);


   /* @method updateFltOffset,float f
      Update filter offset of all active voices
   */
   YM void updateFltOffset (sF32 _f);
   YM void updateFltOffsetByKey (sSI _voiceKey, sF32 _f);


   /* @method updateFltRes,float f
      Update filter resonance of all active voices
   */
   YM void updateFltRes (sF32 _f);
   YM void updateFltResByKey (sSI _voiceKey, sF32 _f);


   /* @method resetLFOFreqPhase
      Reset frequency LFO phase
   */
   YM void resetLFOFreqPhase (void);
   YM void resetLFOFreqPhaseByKey (sSI _key);


   /* @method resetLFOVolPhase
      Reset volume LFO phase
   */
   YM void resetLFOVolPhase (void);
   YM void resetLFOVolPhaseByKey (sSI _key);


   /* @method resetLFOPanPhase
      Reset panning LFO phase
   */
   YM void resetLFOPanPhase (void);
   YM void resetLFOPanPhaseByKey (sSI _key);


   /* @method resetLFOAuxPhase
      Reset aux LFO phase
   */
   YM void resetLFOAuxPhase (void);
   YM void resetLFOAuxPhaseByKey (sSI _key);


   /* @method updatePerfCtl,int idx,float val
      Update performance controller (original MIDI range, 0..127 or 0..16383)
   */
   YM void updatePerfCtl (sUI _idx, sF32 _val);
   YM sF32 updatePerfCtlMSB7 (sUI _idx, sUI _val7);
   YM sF32 updatePerfCtlLSB7 (sUI _idx, sUI _val7);

   /* @method updatePerfCtls,FloatArray initialPerfCtl
      Update performance controllers. Skip when value is <0.
   */
   YM void updatePerfCtls (YAC_Object *_initialPerfCtl);

   // for poly-pressure (STSAMPLEPLAYER_PERFCTL_PRESSURE)
   YM void updatePerfCtlByKey (sSI _voiceKey, sUI _idx, sF32 _val);


   /* @method setSustainPedal,float val
      Update sustain pedal state (0..1) (for zone mod matrix)
   */
   YM void setSustainPedal (sBool _bPressed);
   YM sBool getSustainPedal (void);

   /* @method setSoftPedal,float val
      Update soft pedal state (0..1) (for zone mod matrix)
   */
   YM void setSoftPedal (sBool _bPressed);
   YM sBool getSoftPedal (void);

   // Get last played voice key
   YM sUI getLastVoiceKey (void);

   // Find unique samples triggered by last voice key
   //  (used to select zone-by-midi in Eureka PageSample UI)
   YM void findUniqueSamplesByVoiceKey (YAC_Object *_retSamplesPA, sUI _voiceKey);

   // Get last played note history (max. 32 entries)
   YM void getNoteHistory (YAC_Object *_ia, sUI _maxDeltaMilliSec);

   // Update BPM+PPQ (for tempo-based mod sequencers)
   YM void setTempo (sF32 _bpm, sSI _ppq);

   // Start live recording (find samples by last started sample bank)
   YM void startLiveRecording (sUI _zoneMaskOrIndex, sBool _bIndex, sBool _bRestart);

   // Stop live recording (find samples by last started sample bank)
   YM void stopLiveRecording (sUI _zoneMaskOrIndex, sBool _bIndex);

   // Set live recording loop shift (added to zone target loop idx) (-64..+63)
   YM void setLiveRecLoopShift (sSI _shift);

   // Set samplebank for liverec processing (when no voices have been started, yet)
   YM void setLiveRecSampleBank (YAC_Object *_sampleBank);

   // Set live recording double buffer index (0 or 1) (in DBLBUF_MODE_PARAM)
   YM void setLiveRecDoubleBufferIndex (sUI _idx);

   // Iterate all samples/zones and check for redraw (live rec)
   YM sBool uiCheckResetAnyRedrawFlag (void);

   // Stop voices and unload voice plugin instances
   YM void unloadVoicePlugins (void);

   // Called when all plugin slots are reordered (and they are all in use)
   YM void handleReorderVoicePlugins (YAC_Object *_sb, YAC_Object *_ia);

   // Set last_started_samplebank hint (for live-recording)
   YM void setLastStartedSampleBankHint (YAC_Object *_sb);

   // Get last_started_samplebank hint
   YM YAC_Object *getLastStartedSampleBankHint (void);
};

extern sF32 sp_voice_bus_null_buffer[STSAMPLEVOICE_MAX_VOICE_BUS_FRAMES * 2u];


#endif // ST_SAMPLEPLAYER_H__
