/// VST2Plugin.h
///
/// (c) 2010-2024 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///
/// created: 01Oct2010
/// changed: 02Oct2010, 03Oct2010, 04Oct2010, 05Oct2010, 06Oct2010, 08Oct2010, 09Oct2010
///          10Oct2010, 13Oct2010, 15Oct2010, 16Oct2010, 02Jul2011, 13May2013, 26May2013
///          08Mar2014, 20Jan2018, 24Jan2018, 25Jan2018, 26Jan2018, 27Jan2018, 28Jan2018
///          29Jan2018, 31Jan2018, 12Feb2018, 23Feb2018, 27Feb2018, 28Feb2018, 02Mar2018
///          03Apr2018, 22Jun2018, 29Jun2018, 26Aug2019, 31Dec2021, 27Feb2022, 14Feb2023
///          24Oct2023, 01Jul2024
///
///
///

#ifndef __VST2PLUGIN_H__
#define __VST2PLUGIN_H__


YG("vst2");


#define VST2PLUGIN_SIGNAL_ONAUDIOMASTERUPDATEDISPLAY 0
#define VST2PLUGIN_SIGNAL_ONAUDIOMASTERAUTOMATE      1
#define VST2PLUGIN_SIGNAL_ONAUDIOMASTERIOCHANGED     2
#define VST2PLUGIN_SIGNAL_ONWIN32KEYEVENT            3
#define VST2PLUGIN_SIGNAL_ONAUDIOMASTERPARAMEDIT     4
#define VST2PLUGIN_NUM_SIGNALS                       5

extern YAC_FunctionHandle tkvst2_audio_master_signal_funs[VST2PLUGIN_NUM_SIGNALS];

extern YAC_ContextHandle tkvst2_audio_master_script_context;

#define VST2PLUGIN_MAX_STRING_LEN  (1024u)

typedef struct vst_shell_plugin_s {
   char effect_name[VST2PLUGIN_MAX_STRING_LEN];
   sU32 uid;
} vst2_shell_plugin_t;


YC class VST2Plugin : public YAC_Object {

   // 32 = 16 stereo ins + outs
   //  (note) UVI Falcon for example uses 34 output buffers
#define VST2PLUGIN_MAX_IO_BUFFERS  YCI (48u)

#define VST2PLUGIN_MAX_SHELL_PLUGINS  YCI  (1024u)

#define VST2PLUGIN_CATEGORY_UNKNOWN        YCI  (0u)
#define VST2PLUGIN_CATEGORY_EFFECT         YCI  (1u)
#define VST2PLUGIN_CATEGORY_SYNTH          YCI  (2u)
#define VST2PLUGIN_CATEGORY_ANALYSIS       YCI  (3u)
#define VST2PLUGIN_CATEGORY_MASTERING      YCI  (4u)
#define VST2PLUGIN_CATEGORY_SPACIALIZERS   YCI  (5u)
#define VST2PLUGIN_CATEGORY_ROOMFX         YCI  (6u)
#define VST2PLUGIN_CATEGORY_SURROUNDFX     YCI  (7u)
#define VST2PLUGIN_CATEGORY_RESTORATION    YCI  (8u)
#define VST2PLUGIN_CATEGORY_OFFLINE        YCI  (9u)
#define VST2PLUGIN_CATEGORY_SHELL          YCI  (10u)
#define VST2PLUGIN_CATEGORY_GENERATOR      YCI  (11u)

  public:
   AEffect *effect;
   char effect_vendor[VST2PLUGIN_MAX_STRING_LEN]; // 64 according to specs..
   char effect_name[VST2PLUGIN_MAX_STRING_LEN]; // 32 according to specs..

#ifdef YAC_MACOS
   CFBundleRef cf_bundle_ref;
#else
#error (todo) implement linux version
#endif

   pthread_mutex_t mtx_dispatcher;

   inline void beginDispatch (void) { pthread_mutex_lock(&mtx_dispatcher); }
   inline void endDispatch   (void) { pthread_mutex_unlock(&mtx_dispatcher); }

   pthread_mutex_t mtx_events;
   inline void beginEvents(void) { pthread_mutex_lock(&mtx_events); }
   inline void endEvents(void)   { pthread_mutex_unlock(&mtx_events); }

   sF32 *input_buffers[VST2PLUGIN_MAX_IO_BUFFERS];
   sF32 *output_buffers[VST2PLUGIN_MAX_IO_BUFFERS];

   sBool b_ext_input_buffers;
   sBool b_ext_output_buffers;

   sBool b_ui_thread; // if true, run VST2 UI in a separate thread. if false, use main thread
                      //  (note) this is a workaround for (very few) plugins that won't work properly with the threaded approach
                      //  (note) however, as the non-threaded option is more compatible, UI threads are disabled by default

   sBool b_debug;
   sBool b_debug_paramproperties_once;

   vst2_shell_plugin_t *shell_plugins;
   sUI num_shell_plugins;
   sSI current_shell_plugin_idx;  // -1=none selected
   sUI shell_plugin_uid;  // during loadPlugin() to respond to audioMasterCurrentId
   static sUI static_shell_plugin_uid;    // (note) Waves queries audioMasterCurrentId before returning the effect instance
   static pthread_mutex_t mtx_shell_plugin_uid;

   // when >1, collect n chunks before calling processReplacing()
   //  (note) workaround for some Zynaptic plugins that require buffer sizes of at least 1024 sample frames
   //  (note) output starts at idx=0, input at idx=collect_num_chunks
   sUI   collect_num_chunks;  // 16 => collect 16*block_size sample frames
   sUI   collect_chunk_idx;   // 0..collect_num_chunks*2 (dblbuf)
   sF32 *collect_input_buffers[VST2PLUGIN_MAX_IO_BUFFERS];
   sF32 *collect_output_buffers[VST2PLUGIN_MAX_IO_BUFFERS];


  protected:
   
   sUI block_size; // (maximum) block size (#sample frames)
   sUI num_input_buffers;
   sUI num_output_buffers;


   // (note) the VST2 API allows plugins to keep references to VstEvents/VstMidiEvents after effProcessEvents has returned.
   //        It is not clearly specified, when it becomes safe to delete/re-use the events but I assume it is after
   //        the next call to processReplacing()..
   //
   // (note) (most?) VSTs do not seem to work properly when effProcessEvents is called more than once before each processReplacing() call.
   //        Therefore, events are queued and should be sent to the VST all at once before each call to processReplacing()

   VstEvents    *events;          // sizeof(VstEvents) + (max_events * sizeof(VstMidiEvent*))
   VstMidiEvent *midi_events;
   sUI           next_event;      // #queued events / index of next free event
   sUI           max_events;
   sUI           max_events_mask;

   YAC_Buffer *patch_data;      // allocated by queryPatchData()

  public:
   sBool last_tkvst2_song_playing;
   sBool b_report_transport_playing;

  public:
   VstTimeInfo time_info;


public:
   YAC(VST2Plugin);

   VST2Plugin(void);
   ~VST2Plugin();

  protected:
   void freeShellPlugins (void);

   void initEvents (void);

   void closeEffect         (void);
   void closeLibrary        (void);
   void freeIOBuffers       (void);
   void reallocateIOBuffers (void);
   void freeEvents          (void);

   void reallocateIOBuffersCollect (void);
   void freeIOBuffersCollect       (void);

   void dispatchGetString (sSI _opcode, sSI _idx, sSI _maxLen, YAC_Value *_r);
   sSI  dispatchGetInt    (sSI _opcode);

   void queueEvent(sU8 _0, sU8 _1, sU8 _2=0, sU8 _3=0);

public:

   void YAC_VCALL yacGetSignalStringList(YAC_String *);
   void YAC_VCALL yacRegisterSignal(sUI _sigid, YAC_FunctionHandle _fun);

   void callOnAudioMasterUpdateDisplay (void);
   void callOnAudioMasterAutomate      (sSI _paramIdx, sF32 _value);
   void callOnAudioMasterIOChanged     (void);
   void callOnWin32KeyEvent            (sUI _lparam);
   void callOnAudioMasterParamEdit     (sSI _bBegin, sSI _paramIdx);

public:
   YM void  StaticInit (void);
   YM sBool WaitForDatabridgeHostEvent (sUI _timeout);
   YM void  StaticExit (void);

   YM void setEnableDebug (sBool _bEnable);

   // (note) selectShellPluginUID==0: scan, >0:select
   YM sBool loadPlugin (YAC_String *_pathName, sUI _selectShellPluginUID);

   YM void open    (void);
   YM void close   (void);
   YM void suspend (void);
   YM void resume  (void);

   YM sSI  getCategory (void);
   YM sBool isEffect (void);
   YM sBool isInstrument (void);

   YM void startProcess (void);
   YM void stopProcess  (void);

   YM void setSampleRate (sF32 _rate);
   YM void setBlockSize  (sUI _blkSize);
   // set number of block_size sampleframe chunks to be collected before processReplacing() is called
   //  (note) when num=0, free collect buffers
   YM void setBlockSizeCollect (sUI _blockSize, sUI _num);

   YM void  getEffectName            (YAC_Value *_r);
   YM void  getVendorString          (YAC_Value *_r);
   YM void  getProductString         (YAC_Value *_r);
   YM sSI   getVendorVersion         (void);
   YM sSI   getUniqueID              (void);
   YM sSI   getVersion               (void);

   YM sSI   canDo             (YAC_String *_text);
   YM sUI   getFlags          (void);
   YM sBool isSynth           (void);
   YM sBool hasEditor         (void);
   YM sBool canReplacing      (void);
   YM sBool programsAreChunks (void);
   YM sBool noSoundInStop     (void);

   YM sSI   getNumMidiInputChannels  (void);
   YM sSI   getNumMidiOutputChannels (void);

   YM sSI   getInitialDelay (void);

   YM void  idle (void); // deprecated, plugin UIs use it to update LEDs etc

   YM void  queueNoteOn            (sSI _midiChannel, sSI  _note,     sSI _velocity);
   YM void  queueNoteOff           (sSI _midiChannel, sSI  _note,     sSI _velocity);
   YM void  queueChannelPressure   (sSI _midiChannel, sSI  _value                  );
   YM void  queueCtlChange         (sSI _midiChannel, sSI  _ctl,      sSI _value   );
   YM void  queueBankSelect        (sSI _midiChannel, sSI  _bankMSB,  sSI _bankLSB );
   YM void  queueProgramChange     (sSI _midiChannel, sSI  _program                );
   YM void  queueChannelVolumef    (sSI _midiChannel, sF32 _vol                    );
   YM void  queueChannelPanf       (sSI _midiChannel, sF32 _pan                    );
   YM void  queuePitchbend         (sSI _midiChannel, sSI  _pitchMSB, sSI _pitchLSB);
   YM void  queuePitchbendf        (sSI _midiChannel, sF32 _pitchbend              );

   YM sUI   queueHostMIDIEventsByFlt (YAC_Object *_hostMIDIEvents, sSI _fltDev, sSI _fltCh, sSI _forceCh, sSI _globalCh, YAC_Object *_modInputFilterOrNull, sUI _minMillisec, sUI _maxMillisec);

   YM sSI   getNumInputs       (void);
   YM void  getInputProperties (sUI _idx, YAC_Value *_r);

   YM sSI   getNumOutputs       (void);
   YM void  getOutputProperties (sUI _idx, YAC_Value *_r);

   YM sSI   getNumPrograms        (void);
   YM void  getProgramName        (YAC_Value *_r);
   YM void  getProgramNameIndexed (sSI _program, YAC_Value *_r);
   YM void  setProgramName        (YAC_String *_s);
   YM sSI   getProgram            (void);
   YM void  setProgram            (sSI _program);

   YM sSI  getNumParameters       (void);
   YM void setParameter           (sSI _index, sF32 _value);
   YM sF32 getParameter           (sSI _index);
   YM void getParameterName       (sSI _index, YAC_Value *_r);
   YM void getParameterLabel      (sSI _index, YAC_Value *_r);
   YM void getParameterProperties (sUI _index, YAC_Value *_r);

   // Process queued events
   YM void processEvents (void);

   // Fill output buffer(s)
   YM void processReplacing (sUI _numFrames, sUI _off);
   YM void processReplacingSilence (sUI _numFrames, sUI _off);

   // bProgram: 1=dump program, 0=dump bank
   YM sBool getChunk (YAC_Object *_buffer, sBool _bProgram);
   YM void  setChunk (YAC_Object *_buffer, sBool _bProgram);

   ///void eventTest(void);

   YM void  setParentNativeWindowHandle (sUI _hWnd);
   YM void  showEditor      (void);
   YM void  hideEditor      (void);
   YM sBool isEditorVisible (void);
   YM sBool isEditorOpen    (void);
   YM void  closeEditor     (void);

   // pseudo-static method
   YM sUI  GetNumOpenWindows (void);

   // for test environment
   YM void PumpEvents(void);            // must be called from UI thread

   YM void HandleQueuedWindowResize(void);  // must be called from UI thread

   YM sBool setExtInputBuffer(sUI _idx, YAC_Object *_fa);
   YM sBool setExtOutputBuffer(sUI _idx, YAC_Object *_fa);

   YM sUI   getNumShellPlugins              (void);
   YM void  getShellPluginNameByIndex       (sUI _index, YAC_Value *_r);
   YM sUI   getShellPluginUIDByIndex        (sUI _index);
   YM sBool selectShellPluginByUID          (YAC_String *_name, sUI _uid);
   YM sUI   getCurrentShellPluginUID        (void);
   YM void  getCurrentShellPluginEffectName (YAC_Value *_r);

   // proprietary effect opcode, used by data-bridge only host
   YM sUI dataBridgeGetNumChunksAvail (void);

   YM void SetEnableHideVSTEditorWhenClosed (sBool _bEnable);

   YM sUI getNativeWindowHandle (void);

   YM void setEnableReportTransportPlaying (sBool _bEnable);
   YM sBool getEnableReportTransportPlaying (void);

   YM void windowToFront (void);

   YM void callEffEditIdle (void);
};


#endif // __VST2PLUGIN_H__
