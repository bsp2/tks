/// VST2Plugin.h
///
/// (c) 2010-2024 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///
/// created: 01Oct2010
/// changed: 02Oct2010, 03Oct2010, 04Oct2010, 05Oct2010, 06Oct2010, 08Oct2010, 09Oct2010
///          10Oct2010, 13Oct2010, 15Oct2010, 16Oct2010, 09Feb2014, 01Nov2021, 31Dec2021
///          18Jul2023, 24Oct2023, 01Jul2024
///
///

#ifndef __VST2PLUGIN_H__
#define __VST2PLUGIN_H__


/** WARNING: This is a DUMMY PLUGIN **/


YG("vst2");


#define VST2PLUGIN_SIGNAL_ONAUDIOMASTERUPDATEDISPLAY 0
#define VST2PLUGIN_SIGNAL_ONAUDIOMASTERAUTOMATE      1
#define VST2PLUGIN_SIGNAL_ONAUDIOMASTERIOCHANGED     2
#define VST2PLUGIN_SIGNAL_ONWIN32KEYEVENT            3
#define VST2PLUGIN_SIGNAL_ONAUDIOMASTERPARAMEDIT     4
#define VST2PLUGIN_NUM_SIGNALS                       5

extern YAC_FunctionHandle tkvst2_audio_master_signal_funs[VST2PLUGIN_NUM_SIGNALS];

extern YAC_ContextHandle tkvst2_audio_master_script_context;


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

   sF32 **input_buffers;
   sF32 **output_buffers;

  protected:
   
   sUI block_size; // (maximum) block size (#sample frames)
   sUI num_input_buffers;
   sUI num_output_buffers;

public:
   YAC(VST2Plugin);

   VST2Plugin(void);
   ~VST2Plugin();

  protected:
   void freeIOBuffers       (void);
   void reallocateIOBuffers (void);

   void queueEvent(sU8 _0, sU8 _1, sU8 _2=0, sU8 _3=0);

public:

   void YAC_VCALL yacGetSignalStringList(YAC_String *);
   void YAC_VCALL yacRegisterSignal(sUI _sigid, YAC_FunctionHandle _fun);

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
