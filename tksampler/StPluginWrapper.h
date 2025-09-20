// ----
// ---- file   : StPluginWrapper.h
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2020-2024 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : Plugin descriptor for StSampleVoice plugin
// ----
// ---- created: 16May2020
// ---- changed: 17May2020, 19May2020, 20May2020, 06Jun2020, 03Feb2021, 30Jul2021, 16Aug2023
// ----          03Sep2023, 30Nov2023, 07Dec2023, 22Dec2023, 06Jan2024, 12Jan2024, 19Jan2024
// ----          21Jan2024, 10Aug2024, 27Sep2024, 03Oct2024, 07Nov2024
// ----
// ----
// ----

#ifndef ST_PLUGIN_WRAPPER_H__
#define ST_PLUGIN_WRAPPER_H__

YG("tksampler")


// <class.png>
// (note) used by StSamplePlayer instances
// (note) ref stored in StSampleVoice
struct st_plugin_cache_entry_t;
struct st_plugin_cache_entry_t {
   st_plugin_cache_entry_t *next;
   st_plugin_voice_t       *voice;
   sBool                    b_used;  // true=in use by voice. false=unused
};


// <class.png>
YC class StPluginInfo : public YAC_Object {

  public:
   st_plugin_info_t *info;

  protected:
   sUI index;

  protected:
   static const char *GetCategoryNameC (sUI _category);
   static const char *GetCategoryShortNameC (sUI _category);

  public:
   StPluginInfo(void);
   ~StPluginInfo();

   YAC(StPluginInfo);

   void init (st_plugin_info_t *_info);

   YM sUI   getIndex (void) const;
   YM void  setIndex (sUI _index);
   YM void  getId (YAC_Value *_r);
   YM void  getAuthor (YAC_Value *_r);
   YM void  getName (YAC_Value *_r);
   YM void  getShortName (YAC_Value *_r);
   YM sUI   getFlags (void);
   YM sBool doVoiceBus (void);
   YM sUI   getCategoryId (void);
   YM void  getCategoryName (YAC_Value *_r);
   YM void  getCategoryShortName (YAC_Value *_r);
   YM sUI   getNumMods (void);
   YM void  getModName (sUI _modIdx, YAC_Value *_r);
   YM sUI   getNumParams (void);
   YM void  getParamName (sUI _paramIdx, YAC_Value *_r);
   YM sF32  getParamReset (sUI _paramIdx);
   YM void  getParamGroupName (sUI _paramGroupIdx, YAC_Value *_r);
   YM sUI   getParamGroupIdx (sUI _paramIdx);
   YM void  getParamSectionName (sUI _paramSectionIdx, YAC_Value *_r);
   YM sUI   getParamSectionIdx (sUI _paramIdx);
   YM sUI   getArrayParamSize (sUI _paramIdx);
   YM sUI   getArrayParamNumVariations (sUI _paramIdx);
   YM sBool getArrayParamElementValueRange (sUI _paramIdx, sUI _elementIdx, YAC_Object *_retStorageMin, YAC_Object *_retStorageMax, YAC_Object *_retDisplayMin, YAC_Object *_retDisplayMax, YAC_Object *_retDisplayPrecision);
   YM void  getArrayParamElementName (sUI _paramIdx, sUI _elementIdx, YAC_Value *_r);
   YM sF32  getArrayParamElementReset (sUI _paramIdx, sUI _elementIdx);
   YM void  updateLUT (sUI _curveIdx, YAC_Object *_fa);

   YM void createSharedInstance (YAC_Value *_r);
};


// <class.png>
YC class StPluginShared : public YAC_Object {

  public:
   StPluginInfo       *info;
   st_plugin_shared_t *shared;

  public:
   StPluginShared(void);
   ~StPluginShared();

   YAC(StPluginShared);

   void init (StPluginInfo *_info, st_plugin_shared_t *_shared);
   void freeShared (void);

   YM YAC_Object *getInfo (void);

   // Returns new String
   YM void queryDynamicParamName (sUI _paramIdx, YAC_Value *_r);
   YM void queryDynamicModName (sUI _modIdx, YAC_Value *_r);

   // Returns new FloatArray or null (no presets)
   YM void queryDynamicParamPresetValues (sUI _paramIdx, YAC_Value *_r);

   // Returns new String or null (no presets / no valid presetIdx)
   YM void queryDynamicParamPresetName (sUI _paramIdx, sUI _presetIdx, YAC_Value *_r);

   // Returns new FloatArray or null
   //  (array elements become invalid when shared instance is deleted)
   YM void getArrayParamVariationPtr (sUI _paramIdx, sUI _variationIdx, YAC_Value *_r);

   // -1=not editing array param, >=0: constantly refresh effective voice array from variation array(s)
   YM void setArrayParamEditVariationIdx (sUI _paramIdx, sSI _variationIdx);

   YM sF32 getParamValue (sUI _paramIdx);
   YM void setParamValue (sUI _paramIdx, sF32 _value);

   // Convert param value to string (e.g. milliseconds, decibels, ..)
   //  (note) may return null when not supported by plugin
   YM void getParamValueString (sUI _paramIdx, YAC_Value *_r);

   YM void benchmarkSingle (YAC_FloatArray *_ioBuf, sUI _numFramesPerChunk, sUI _numChunks);
   YM void benchmarkChunk (YAC_FloatArray *_ioBuf, sUI _numFramesPerChunk, sUI _numChunks);

   YM void createVoiceInstance (sF32 _sampleRate, sUI _voiceIdx, YAC_Value *_r);
};


// <class.png>
YC class StPluginSharedMissing : public YAC_Object {

   static const sUI MAX_MISSING_PARAMS = 32u;

  public:
   YAC_String *plugin_id;

   sUI  num_params;  // usually 8
   sF32 param_values[MAX_MISSING_PARAMS];

   struct {
      sUI   array_sz;  // 0u = not an array param, 1..256 otherwise
      sUI   num_variations;  // 1..32
      sF32 *fa_data;
   } array_params[MAX_MISSING_PARAMS];

  protected:
   void freeArrayParam (const sUI _paramIdx);
   void freeArrayParams (void);

  public:
   StPluginSharedMissing(void);
   ~StPluginSharedMissing();

   YAC(StPluginSharedMissing);

  public:

   YM void        setPluginId (YAC_String *_pluginId);
   YM YAC_String *getPluginId (void);

   YM void setNumParams (sUI _num);
   YM sUI  getNumParams (void);

   YM sF32 getParamValue (sUI _paramIdx);
   YM void setParamValue (sUI _paramIdx, sF32 _value);

   YM sBool allocArrayParam (sUI _paramIdx, sUI _arraySz, sUI _numVariations);
   YM sUI   getArrayParamSize (sUI _paramIdx);
   YM sUI   getArrayParamNumVariations (sUI _paramIdx);
   YM void  getArrayParamVariationPtr (sUI _paramIdx, sUI _variationIdx, YAC_Value *_r);
   YM sUI   getNumArrayParams (void);
};


// <class.png>
YC class StPluginVoice : public YAC_Object {
   // (note) used by ModDelay
   // (note) calls note_on() during init

  public:
   StPluginShared    *shared;
   st_plugin_voice_t *voice;

   // voice bus inputs (interleaved stereo samples)
   //  (note) used by ModSTFX (module side chain inputs (2=SC_L, 3=SC_R) to voice bus #1)
   //  (note) initially points to loc_voicebus_interleaved_zeroes
   sF32 *sc_voice_bus_buffers[ST_PLUGIN_MAX_LAYERS/*32*/];

  public:
   StPluginVoice(void);
   ~StPluginVoice();

   YAC(StPluginVoice);

   void init (StPluginShared *_shared, sF32 _sampleRate, sUI _voiceIdx);
   void freeVoice (void);

   YM YAC_Object *getShared (void);

   YM void noteOn (sBool _bGlide, sUI _note, sF32 _vel);
   YM void setModValue (sUI _modIdx, sF32 _value, sUI _frameOff);

   YM void processReplace (YAC_FloatArray *_ioInterleavedBuf, sUI _numFrames);
   YM void processReplaceVoiceBus1SideChain (YAC_FloatArray *_ioInterleavedBuf, YAC_FloatArray *_ioInterleavedBufSC, sUI _numFrames);
};


// <class.png>
YC class StPluginLibrary : public YAC_Object {

  protected:
   void *dll_handle;
   st_plugin_init_fxn_t init_fxn;

   st_plugin_info_t *infos[ST_PLUGIN_MAX];
   sUI               num_infos;

  protected:
   sBool       plafOpenLibrary    (const char *_pathName);
   void        plafCloseLibrary   (void);
   void       *plafFindFunction   (const char *_name) const;
   const char *plafGetErrorString (void);

  public:
   StPluginLibrary(void);
   ~StPluginLibrary();

   YAC(StPluginLibrary);

   YM sBool open (YAC_String *_pathName);
   YM void close (void);

   YM void getPluginInfoByIndex (sUI _index, YAC_Value *_r);
};


#endif // ST_PLUGIN_WRAPPER_H__
