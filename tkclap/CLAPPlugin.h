/// CLAPPlugin.h
///
/// (c) 2024 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///
/// created: 01Jul2024
/// changed: 02Jul2024, 03Jul2024, 04Jul2024, 05Jul2024, 06Jul2024, 22Sep2024, 27Sep2024
///
///
///

#ifndef CLAPPLUGIN_H__
#define CLAPPLUGIN_H__

YG("clap");

#define CLAPPLUGIN_MAX_AUDIO_IN   48
#define CLAPPLUGIN_MAX_AUDIO_OUT  48
#define CLAPPLUGIN_MAX_BLOCK_SIZE 256   // #frames (usually 64)

#define CLAPPLUGIN_MAX_EVENTS 4096

#define CLAPPLUGIN_SIGNAL_ONPARAMEDIT      0  // Gesture begin/end
#define CLAPPLUGIN_SIGNAL_ONAUTOMATE       1  // param value change
#define CLAPPLUGIN_SIGNAL_ONWIN32KEYEVENT  2  // key pressed (Windows build)
#define CLAPPLUGIN_SIGNAL_ONMACOSKEYDOWN   3  // key pressed (macOS build)
#define CLAPPLUGIN_SIGNAL_ONRESCANPARAMS   4  // params changed
#define CLAPPLUGIN_NUM_SIGNALS             5

extern YAC_FunctionHandle tkclap_signal_funs[CLAPPLUGIN_NUM_SIGNALS];
extern YAC_ContextHandle tkclap_script_context;


// <class.png>
YC class CLAPPluginBundle : public YAC_Object {

  public:
   YAC_String             path_name;      // e.g. "/Library/Audio/Plug-Ins/CLAP/Reels.clap"

#ifdef YAC_MACOS
   CFBundleRef            cf_bundle_ref;
#elif defined(YAC_WIN32)
   HINSTANCE              dll_handle;
#else
   // Linux (dlfcn API)
   void                  *dll_handle;
#endif

   clap_plugin_entry_t   *plugin_entry;   // "clap_entry" symbol
   clap_plugin_factory_t *plugin_factory;
   sUI                    num_plugins;

   sSI num_open_plugins;  // 0=safe to unload bundle   

   sBool b_debug;

  public:
   YAC(CLAPPluginBundle);

   CLAPPluginBundle(void);
   ~CLAPPluginBundle();

   // Decrement num_open_plugins
   //  (note) called by CLAPPlugin::destroyPluginInstance() (after plugin->destroy(plugin))
   void handleDestroyPlugin (void);

  public:
   YM void setEnableDebug (sBool _bEnable);

   // Load plugin bundle / DSO
   YM sBool openPluginBundle (YAC_String *_pathName);

   // Close plugin bundle / DSO
   //  (note) caller MUST delete all plugin instances beforehand
   YM void closePluginBundle (void);

   // Query number of plugins
   YM sUI getNumPlugins (void);

   // Create CLAPPlugin object
   //  (note) returns new CLAPPlugin object (or null)
   //  (note) does NOT create actual CLAP plugin instance (call plugin->create() afterwards)
   //  (note) increments num_open_plugins
   YM void createPluginByIndex (sUI _idx, YAC_Value *_r);

   // Query number of currently active plugins
   //  (note) 0=safe to unload plugin bundle
   YM sSI getNumOpenPlugins (void);
};


// <class.png>
struct CLAPAudioPort {
   char  name[CLAP_NAME_SIZE];  // copied from clap_audio_port_info_t.name
   sUI   num_ch;        // 0=unused, 1=mono, 2=stereo
   sF32  data[CLAPPLUGIN_MAX_BLOCK_SIZE * 2/*stereo*/];
   sF32 *data_ptrs[2];  // ref to data or external channel buffer(s)
};


// <class.png>
typedef union tkclap_event_u {
   clap_event_header_t          header;
   clap_event_note_t            note;
   clap_event_note_expression_t note_expr;
   clap_event_param_value_t     param_value;
   clap_event_param_mod_t       mod;
   clap_event_param_gesture_t   gesture;
   clap_event_transport_t       transport;
   clap_event_midi_t            midi;
   clap_event_midi_sysex_t      midi_sysex;
   clap_event_midi2_t           midi2;
} tkclap_event_t;


// <class.png>
typedef struct tkclap_input_events_s {
   clap_input_events_t  clap_input_events;
   tkclap_event_t       events[CLAPPLUGIN_MAX_EVENTS];
   sUI                  num_ev;
} tkclap_input_events_t;


// <class.png>
typedef struct tkclap_output_events_s {
   clap_output_events_t  clap_output_events;
   tkclap_event_t        events[CLAPPLUGIN_MAX_EVENTS];
   sUI                   num_ev;
} tkclap_output_events_t;


// <class.png>
typedef struct tkclap_param_info_s {
   clap_param_info_t clap_param_info;
   sF32              cur_mod;          // last sent offset modulation
} tkclap_param_info_t;


// <class.png>
YC class CLAPPlugin : public YAC_Object {

  public:
   CLAPPluginBundle               *plugin_bundle;  // ref to parent bundle
   const clap_plugin_descriptor_t *plugin_desc;
   const clap_plugin_t            *plugin;  // after create()

   // Passed to plugin_factory->create_plugin() (plugin => host callbacks)
   clap_host_t host;

   // read/reset by callOnMainThread(), set by queueCallOnMainThread() via loc_host_request_callback()
   sBool          b_queued_call_on_main_thread;
   tkclap_mutex_t mtx_call_on_main_thread;

   // CLAP_EXT_AUDIO_PORTS extension (see clap-main/include/clap/ext/audio-ports.h)
   clap_plugin_audio_ports_t *ext_audio_ports;
   sUI num_audio_in;  // number of ports (mono or multichannel)
   sUI num_audio_out;
   CLAPAudioPort audio_in [CLAPPLUGIN_MAX_AUDIO_IN];
   CLAPAudioPort audio_out[CLAPPLUGIN_MAX_AUDIO_OUT];
   clap_audio_buffer_t clap_audio_buffers_in [CLAPPLUGIN_MAX_AUDIO_IN];
   clap_audio_buffer_t clap_audio_buffers_out[CLAPPLUGIN_MAX_AUDIO_OUT];
   sUI num_input_channels;  // number of (mono) input channels (stereo counts as two channels)
   sUI num_output_channels;

   // CLAP_EXT_NOTE_PORTS extension (see clap-main/include/clap/ext/note-ports.h)
   clap_plugin_note_ports_t *ext_note_ports;
   clap_note_port_info_t     note_in;  // first note input port

   // CLAP_EXT_PARAMS extension (see clap-main/include/clap/ext/params.h)
   clap_plugin_params_t *ext_params;
   sUI                   num_params;
   tkclap_param_info_t  *param_infos;

   // CLAP_EXT_STATE extension (see clap-main/include/clap/ext/state.h)
   clap_plugin_state_t *ext_state;

   // CLAP_EXT_LATENCY extension (see clap-main/include/clap/ext/latency.h)
   clap_plugin_latency_t *ext_latency;

   // CLAP_EXT_GUI (see clap-main/include/clap/ext/gui.h)
   clap_plugin_gui_t *ext_gui;

   // Input event queue / list (param/mod changes, MIDI events, ..)
   tkclap_input_events_t input_events;         // process()
   tkclap_input_events_t input_events_flush;   // flushParams()

   // Output event queue / list (sent from plugin)
   tkclap_output_events_t output_events;
   tkclap_output_events_t output_events_flush; // flushParams()

   // Synchronizes access to input_events / output_events
   tkclap_mutex_t mtx_events;

   // true=startProcessing() called. false=initial (or stopProcessing() called)
   sBool b_processing;

   sBool b_debug;

   // (note) NSWindow* on macOS
   // (todo) remove ? (non-embedded / floating windows don't seem to work with any plugin)
   void *transient_native_window_handle;  // for ext_gui->set_transient(). see setNativeWindowHandle()

  public:
   sF32 sample_rate;  // def=48000
   sU32 block_size;   // def=64

  public:
   // Last seen window geometry
   sSI ui_window_x;
   sSI ui_window_y;
   sSI ui_window_w;
   sSI ui_window_h;

  public:
   sBool b_report_transport_playing;

  public:
   YAC(CLAPPlugin);

   CLAPPlugin(void);
   ~CLAPPlugin();

  public:
   // (note) plugin initially inherits b_debug setting from parent bundle
   YM void setEnableDebug (sBool _bEnable);

   // (note) must be called before createPluginInstance()
   YM void setSampleRate (sF32 _sampleRate);

   // (note) must be called before createPluginInstance()
   YM void setBlockSize  (sUI _blockSize);

  public:
   YM CLAPPluginBundle *getPluginBundle (void);

   // Query id/name/vendor from plugin descriptor
   //  (note) may be called before create()
   YM void getId (YAC_Value *_r);
   YM void getName (YAC_Value *_r);
   YM void getVendor (YAC_Value *_r);

   // Create + activate CLAP plugin instance
   YM sBool createPluginInstance (void);

   // Deactivate + destroy CLAP plugin instance
   YM void destroyPluginInstance (void);

  protected:
   sBool setExtChannelBuffer (const sUI _channelIdx, YAC_Object *_fa, CLAPAudioPort *_ports, const sUI _numPorts);

   void freeParamInfos (void);

  public:
   // Query number of input/output ports (mono or multichannel)
   YM sUI getNumInputPorts (void);
   YM sUI getNumOutputPorts (void);

   // Query number of input/output port channels
   YM sUI getNumInputPortChannels (sUI _portIdx);
   YM sUI getNumOutputPortChannels (sUI _portIdx);

   // Query number of input/output channels (stereo port counts as two channels)
   YM sUI getNumInputChannels (void);
   YM sUI getNumOutputChannels (void);

   void getChannelName(const sUI _channelIdx, CLAPAudioPort *_ports, const sUI _numPorts, YAC_Value *_r);

   // input port name + " L" / " R" (or "<n>")
   YM void getInputChannelName (sUI _channelIdx, YAC_Value *_r);

   // output port name + " L" / " R" (or "<n>")
   YM void getOutputChannelName (sUI _channelIdx, YAC_Value *_r);

   // Override I/O buffers
   YM sBool setExtInputChannelBuffer (sUI _channelIdx, YAC_Object *_fa);
   YM sBool setExtOutputChannelBuffer (sUI _channelIdx, YAC_Object *_fa);

   // Query latency (#frames)
   YM sUI getLatency (void);

   // Rescan parameters
   //  - automatically during init()
   //  - host must call this after receiving CLAPPLUGIN_SIGNAL_ONRESCANPARAMS
   YM void rescanParamInfos (void);

  protected:
   tkclap_event_t *getNextInputEvent (void);

  public:
   // (note) locked during process()
   // (note) must be called when setting parameters from main/other thread
   YM void lockEvents (void);
   YM void unlockEvents (void);

   // Return 1=instrument/synth/generator  0=effect
   YM sBool isInstrument (void);

   // Return 1=effect 0=instrument/synth/generator
   YM sBool isEffect (void);

   // Query if plugin has a UI (ext_gui)
   //  (note) createPluginInstance() must be called, first
   YM sBool hasEditor (void);

   // Query number of parameters
   YM sUI  getNumParameters       (void);

   // Returns true when one or many bits of 'flagMask' are set
   sBool checkParameterFlag       (const sUI _index, const sUI _flagMask);

   // Query whether param is an integer (or enum)
   YM sBool isParameterStepped    (sUI _index);

   // Query whether param is periodic (e.g. phase)
   YM sBool isParameterPeriodic   (sUI _index);

   // Query whether param is hidden (CLAP_PARAM_IS_HIDDEN)
   YM sBool isParameterHidden     (sUI _index);

   // Query whether param is read only (CLAP_PARAM_IS_READONLY)
   YM sBool isParameterReadOnly   (sUI _index);

   // Query whether param is read only (CLAP_PARAM_IS_BYPASS)
   YM sBool isParameterBypass     (sUI _index);

   // Query whether param is modulatable (setParameterMod() / CLAP_EVENT_PARAM_MOD)
   YM sBool isParameterModulatable (sUI _index);

   // Add parameter-change event to input events list
   YM void setParameter           (sUI _index, sF32 _value);

   // Lock input events mutex, add single parameter-change event, unlock input events mutex
   YM void setParameter_Sync      (sUI _index, sF32 _value);

   // Add relative parameter modulation (offset) event to input events list
   //  (note) "The value heard is: param_value + param_mod." (see clap-main/include/clap/events.h)
   YM void setParameterMod        (sUI _index, sF32 _offset);

   // Lock input events mutex, add single parameter-mod event, unlock input events mutex
   YM void setParameterMod_Sync   (sUI _index, sF32 _offset);

   // Reset all parameter modulation (offsets)
   //  (note) sends mod/offset 0.0 for all parameters whose (last sent) modulation offest does not equal 0.0f
   YM void resetParameterMods     (void);
   YM void resetParameterMods_Sync(void);

   // Query current parameter value
   YM sF32 getParameter           (sUI _index);

   // Query current (last sent) parameter modulation offset value
   YM sF32 getParameterMod        (sUI _index);

   // Query parameter name
   YM void getParameterName       (sUI _index, YAC_Value *_r);

   // "The module path containing the param, eg: 'Oscillators/Wavetable 1'" (see clap-main/include/clap/ext/params.h)
   YM void getParameterPath       (sUI _index, YAC_Value *_r);

   // Query parameter range and default value
   YM sF32 getParameterMinValue   (sUI _index);
   YM sF32 getParameterMaxValue   (sUI _index);
   YM sF32 getParameterDefValue   (sUI _index);

   // Convert parameter value to text
   YM void parameterValueToText (sUI _index, sF32 _value, YAC_Value *_r);

   // Convert text to parameter value
   YM sF32 parameterTextToValue (sUI _index, YAC_String *_text);

   // Filter and queue HostMIDIEvents to plugin (add to input_events list)
   //  (note) usually called from audio thread immediately before process()
   YM sUI queueHostMIDIEventsByFlt (YAC_Object *_hostMIDIEvents, sSI _fltDev, sSI _fltCh, sSI _forceCh, sSI _globalCh, YAC_Object *_modInputFilterOrNull, sUI _minMillisec, sUI _maxMillisec);

  public:
   // Save / load plugin state to / from stream (see clap-main/include/clap/ext/state.h)
   YM sBool saveState (YAC_Object *_ofs);
   YM sBool loadState (YAC_Object *_ifs);

  public:
   YM void startProcessing (void);
   YM void stopProcessing (void);

   // see clap-main/include/clap/plugin.h
   //   - Clears all buffers, performs a full reset of the processing state (filters, oscillators,
   //     envelopes, lfo, ...) and kills all voices.
   //   - The parameter's value remain unchanged.
   //   - clap_process.steady_time may jump backward.
   YM void reset (void);

  public:
   void YAC_VCALL yacGetSignalStringList(YAC_String *);
   void YAC_VCALL yacRegisterSignal(sUI _sigid, YAC_FunctionHandle _fun);

   void callOnParamEdit (sBool _bBegin, sSI _paramIdx);
   void callOnAutomate (sSI _paramIdx, sF32 _value);
   void callOnWin32KeyEvent (sUI _lparam);
   void callOnMacOSKeyDown (sUI _keyCode);
   void callOnRescanParams (void);


  public:
   // Usually called by plugin via host_request_callback()
   YM void queueCallOnMainThread (void);

   // Called from main thread in response to host_request_callback()
   YM void callOnMainThread (void);

  public:
   sBool handleRequestResize (sUI _sizeX, sUI _sizeY);

   // for set_transient() (when letting plugin create its own top-level window)
   //  (note) see tksdl2:SDL.getNativeWindowHandle()
   //  (todo) REMOVE
   YM void  setTransientNativeWindowHandle (YAC_String *_handle);

   YM void  showEditor      (void);
   YM void  hideEditor      (void);
   YM void  closeEditor     (void);
   YM sBool isEditorVisible (void);
   YM sBool isEditorOpen    (void);

  public:
   YM void setEnableReportTransportPlaying (sBool _bEnable);
   YM sBool getEnableReportTransportPlaying (void);

  public:
   // (note) returns parameter idx or -1 when mapping failed
   sSI mapParamIdToParamIdx (clap_id _paramId);

   // Called by process() / flushParameters()
   //  (note) invokes callOnParamEdit() / callOnAutomate()
   //  (note) clears the event list afterwards
   void processOutputEvents (void);

   // Called when plugin is not processing (i/o events are handled in audio thread process() otherwise)
   void flushParameters (void);

   // Process events and fill output buffers with 0
   YM void processSilence (sUI _numFrames);

   // Process events and render buffer fragment
   YM void process (sUI _numFrames);

};


#endif // CLAPPLUGIN_H__
