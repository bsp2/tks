// ----
// ---- file   : StWaveform.h
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2009-2024 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : This is part of the "syntracker" midi sequencer.
// ----
// ---- created: 23Sep2009
// ---- changed: 26Dec2009, 27Dec2009, 29Dec2009, 02Jan2010, 05Jan2010, 07Jan2010, 15Jan2010
// ----          19Jan2010, 21Jan2010, 11May2010, 05Sep2010, 21Nov2010, 17Feb2011, 22Mar2011
// ----          04May2013, 26May2013, 01Jun2013, 15Dec2018, 16Dec2018, 01Aug2021, 24Mar2023
// ----          03Oct2024
// ----
// ----
// ----

#ifndef ST_WAVEFORM_H__
#define ST_WAVEFORM_H__

YG("tksampler")


YC class StWaveform : public YAC_Object {
  public:
   YAC_FloatArray *sample_data;  // the actual sample data
   sBool           b_own_data;
   sUI             num_channels; // number of interleaved channels
   sUI             loop_offset;
   sUI             loop_len;
   sF32            base_frequency;
   sF32            sample_rate;
   sUI             ring_offset;

   YAC_String *sample_name; // for UI

   sF32        ui_last_played_offset; // for sampleview
   sF32        ui_offset;
   sF32        ui_zoom;
   sUI         ui_type;               // 0=embedded, 1=procedural, 2=repository, 3=project
   YAC_String *ui_pathname;           // for types 2,3
   sSI         ui_id;
   YAC_Object *ui_undo;

   YAC_String  *bwf_description;  // up to 256 chars (excluding ASCIIZ)
   YAC_String  *bwf_originator;   // up to 32 chars, e.g. "TASCAM PCM Recoder DR-05X"
   YAC_String  *bwf_date;         // up to 10 chars, e.g. "2023-03-24"
   YAC_String  *bwf_time;         // up to 8 chars, e.g. "12:17:09"

   YAC_IntArray *cue_points;

   void *ui_last_started_voice;   // StSampleVoice ref (never dereferenced). used when updating UI last read wave pos.

  public:
   StWaveform(void);
   ~StWaveform();

   YAC(StWaveform);

  protected:
   void freeSampleData (void);
  public:

   YM YAC_Object *_getSampleData    (void);
   YM sSI         _getNumFrames     (void);

   YM void        _setSampleData    (YAC_Object *_fa, sUI _numChannels);

   YM sUI         _getNumChannels   (void);
   YM void        _setNumChannels   (sUI _ch);

   YM sSI         _getLoopOffset    (void);
   YM void        _setLoopOffset    (sSI _loopOff);

   YM sSI         _getLoopLen       (void);
   YM void        _setLoopLen       (sSI _loopLen);

   YM sF32        _getBaseFrequency (void);
   YM void        _setBaseFrequency (sF32 _baseFreq);

   YM sF32        _getSampleRate    (void);
   YM void        _setSampleRate    (sF32 _sampleRate);

   YM void        _free             (void);
   YM sSI         _alloc            (sSI _numChannels, sSI _numFrames);

   YM YAC_Object *_getName          (void);
   YM void        _setName          (YAC_Object *);

   YM sUI         _getRingOffset    (void);
   YM void        _setRingOffset    (sUI _off);

   // ---- UI related setters/getters ----

   YM sF32 _uiGetLastPlayedOffset (void);
   YM void _uiInvalidateLastPlayedOffset (void);

   YM void _uiSetOffset (sF32 _offset);
   YM sF32 _uiGetOffset (void);

   YM void _uiSetZoom (sF32 _zoom);
   YM sF32 _uiGetZoom (void);

   YM void _uiSetType (sUI _type);
   YM sUI  _uiGetType (void);

   YM void        _uiSetPathName (YAC_Object *);
   YM YAC_Object *_uiGetPathName (void);

   YM void _uiSetId (sSI _id);
   YM sSI  _uiGetId (void);

   YM void        _uiSetUndo (YAC_Object *_undo);
   YM YAC_Object *_uiGetUndo (void);

   // ---- broadcast wave format extension ----
   YM YAC_Object *_getBwfDescription   (void);
   YM void        _setBwfDescription   (YAC_Object *);

   YM YAC_Object *_getBwfOriginator   (void);
   YM void        _setBwfOriginator   (YAC_Object *);

   YM YAC_Object *_getBwfDate   (void);
   YM void        _setBwfDate   (YAC_Object *);

   YM YAC_Object *_getBwfTime   (void);
   YM void        _setBwfTime   (YAC_Object *);

   // ---- cue points ----
   YM YAC_Object *_getCuePoints (void);
   YM YAC_Object *_getOrCreateCuePoints (void);
};


#endif // ST_WAVEFORM_H__
