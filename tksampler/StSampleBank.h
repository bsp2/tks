// ----
// ---- file   : StSampleBank.h
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2009-2024 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : This is part of the "syntracker" midi sequencer.
// ----
// ---- created: 23Sep2009
// ----
// ---- changed: 26Dec2009, 27Dec2009, 29Dec2009, 02Jan2010, 05Jan2010, 07Jan2010, 15Jan2010
// ----          19Jan2010, 21Jan2010, 29Apr2010, 24Mar2013, 23Dec2018, 27Dec2018, 31Dec2018
// ----          17Jan2019, 23Apr2019, 15Feb2020, 25Feb2020, 08Aug2021, 18Nov2023, 15Aug2024
// ----          28Sep2024, 30Sep2024, 12Nov2024
// ----
// ----
// ----

#ifndef ST_SAMPLEBANK_H__
#define ST_SAMPLEBANK_H__

YG("tksampler")


YC class StSampleBank : public YAC_Object {
  public:
   StSample *first_sample;  // deletable, single-linked list of samples

   StSampleMutexGroup *first_mutex_group; // deletable list of mutex groups

   YAC_String *name;

   sUI max_voices; // maximum number of times this samplebank can be played in one generator

   sF32 transpose;  // semi-tones (fractional)
   sF32 freq_rand_amount;
   sF32 volume;
   sF32 volume_rand_amount;
   sF32 pan;
   sF32 pan_rand_amount;
   sF32 sampleoffset_rand_amount;

   sBool b_realloc;      // 1=try to realloc (mono) voice (e.g. when using analog-style envelopes)
   sBool b_unique_notes; // 0=allow multiple voices to play the same note, 1=soft-stop prev. voice when note is retriggered
   sBool b_perfctl_freeze_noteoff;  // 1=freeze performance controllers at note-off

   sUI liverec_last_process_tick_nr; // see Replay.process_tick_nr (don't record input buffer more than once)

   // NULL=use default (StSamplePlayer) freq table. 128 frequencies (MIDI notes) per table.
   //  (note) sample zone (StSample) may override these
   sF32       *tuning_tables[STSAMPLE_MAX_TUNING_TABLES/*16*/];
   YAC_Object *tuning_tables_meta[STSAMPLE_MAX_TUNING_TABLES/*16*/];  // editor info (SampleTuningTable script objects or NULL)
   sSI         default_tuning_table_idx;  // -1=use default table (StSamplePlayer)
   sSI         forced_tuning_table_idx;       // -1=use default_tuning_table_idx

  protected:
   sUI num_samples; // optimization

  public:
   StSampleBank(void);
   ~StSampleBank();

   YAC(StSampleBank);

  public:
   const sF32 *getCurrentTuningTableOrNull (void);

  public:
   YM void _freeSamples (void);
   YM void _freeMutexGroups (void);

   YM YAC_Object *_getFirstSample (void);
   YM void        _unlinkFirstSample (YAC_Value *_r);
   YM YAC_Object *_getSampleByIdx (sUI _idx);
   YM sSI         _getSampleIdx (YAC_Object *_sample);
   YM void        _addSample      (YAC_Object *_sample);
   YM void        _addSampleAfter (YAC_Object *_sample, YAC_Object *_prevSample);
   YM void        _removeSample   (YAC_Object *_sample);
   YM sUI         _getNumSamples  (void);
   YM void        _setEnableSkipRange (sBool _bEnable);
   YM void        _swapSamples    (sUI _idxLeft);

   YM YAC_Object *_getFirstMutexGroup         (void);
   YM YAC_Object *_getMutexGroupByIdx         (sUI _idx);
   YM void        _addMutexGroup              (YAC_Object *_mtxgrp);
   YM void        _removeMutexGroup           (YAC_Object *_mtxgrp);
   YM sSI         _getMutexGroupIndexByObject (YAC_Object *_mtxgrp);
   YM sUI         _getNumMutexGroups          (void);

   YM YAC_Object *_getName (void);
   YM void        _setName (YAC_Object *_str);

   YM void        _setMaxVoices (sUI _maxVoices);
   YM sUI         _getMaxVoices (void);

   YM void        _setTranspose (sF32 _transpose);
   YM sF32        _getTranspose (void);

   YM void        _setFreqRandAmount (sF32 _amt);
   YM sF32        _getFreqRandAmount (void);

   YM void        _setVolume (sF32 _vol);
   YM sF32        _getVolume (void);

   YM void        _setVolumeRandAmount (sF32 _amt);
   YM sF32        _getVolumeRandAmount (void);

   YM void        _setPan (sF32 _pan);
   YM sF32        _getPan (void);

   YM void        _setPanRandAmount (sF32 _amt);
   YM sF32        _getPanRandAmount (void);

   YM void        _setSampleOffsetRandAmount (sF32 _amt);
   YM sF32        _getSampleOffsetRandAmount (void);

   YM void        _setEnableRealloc (sBool _bEnable);
   YM sSI         _getEnableRealloc (void);

   YM void        _setEnableUniqueNotes (sBool _bEnable);
   YM sSI         _getEnableUniqueNotes (void);

   // Called internally when a sample is added, deleted, or moved up/down.
   // Must be called externally when a range is changed.
   YM void updateAltGroups (void);

   // For UI/Debugging, count the number of alt groups
   YM sSI _getNumAltGroups (void);

   YM void  _setEnablePerfCtlFreezeNoteOff (sBool _bEnable);
   YM sBool _getEnablePerfCtlFreezeNoteOff (void);

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
};


#endif // ST_SAMPLEBANK_H__
