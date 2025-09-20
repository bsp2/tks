/// VST2Plugin.cpp
///
/// (c) 2010-2024 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///
/// created: 02Oct2010
/// changed: 03Oct2010, 04Oct2010, 05Oct2010, 09Oct2010, 10Oct2010, 13Oct2010, 15Oct2010
///          16Oct2010, 20Oct2010, 02Jul2011, 09Jan2012, 09May2013, 12May2013, 13May2013
///          08Mar2014, 19Jan2018, 20Jan2018, 24Jan2018, 25Jan2018, 26Jan2018, 27Jan2018
///          31Jan2018, 12Feb2018, 14Feb2018, 15Feb2018, 23Feb2018, 27Feb2018, 28Feb2018
///          02Mar2018, 03Apr2018, 22Jun2018, 29Jun2018, 06Jul2018, 19Aug2018, 07Jul2019
///          17Jul2019, 30Jul2019, 05Aug2019, 26Aug2019, 20May2021, 31Dec2021, 08Jan2022
///          11Jan2022, 27Feb2022, 14Feb2023, 24Oct2023, 01Jul2024
///
///
///

#include "tkvst2.h"

#include "VST2PinProperties.h"
#include "VST2ParameterProperties.h"

#include "VST2Plugin.h"
#include "../tkmidipipe/HostMIDIEvents.h"
#include "../tkmidipipe/ModInputFilter.h"

#include <math.h>

// (note) resume() >> startProcess() >> processReplacing() 
// (note) processReplacing() >> stopProcess() >> suspend() 

// If defined, check whether plugin tries to write beyond specified string boundaries (some buggy plugins do)
#define PARANOIA_CHECK_DISPATCH_STRING_OVERFLOW defined

// If defined, check if processReplacing() produced any output
#define PARANOIA_CHECK_REPLACING_OUTPUT defined

#ifdef YAC_WIN32
#define DATABRIDGE_HOST_EVENT_NAME   "vst_databridge_ev"   // raised by any consumer, databridge-only host can wait for it
#define effDataBridgeGetNumChunksAvail  0x44894489
HANDLE VST2Plugin::databridge_host_ev = 0;
#endif // YAC_WIN32

sUI VST2Plugin::static_shell_plugin_uid = 0u;
pthread_mutex_t VST2Plugin::mtx_shell_plugin_uid;

YAC_FunctionHandle tkvst2_audio_master_signal_funs[VST2PLUGIN_NUM_SIGNALS];

YAC_ContextHandle tkvst2_audio_master_script_context;

static sBool b_hide_vst_window_when_closed = YAC_TRUE;

#define Dopprintf if(1);else yac_host->printf

#ifdef PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP
static pthread_mutex_t loc_pthread_mutex_t_init = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
#elif defined(PTHREAD_RECURSIVE_MUTEX_INITIALIZER)
//macOS
static pthread_mutex_t loc_pthread_mutex_t_init = PTHREAD_RECURSIVE_MUTEX_INITIALIZER;
#else
// recursive initializer not available, just use the normal one and hope it works ;)
static pthread_mutex_t loc_pthread_mutex_t_init = PTHREAD_MUTEX_INITIALIZER;
#endif // PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP

typedef AEffect* (*PluginEntryProc) (audioMasterCallback audioMaster);

static VstIntPtr VSTCALLBACK HostCallback (AEffect* effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt) {
   static VstInt32 lastOpcode = -1;
   static VstIntPtr lastTimeMask = ~0;
	VstIntPtr result = 0;
   VST2Plugin *plugin;

   if(NULL != effect)
   {
      plugin = (VST2Plugin*) effect->resvd1;
   }
   else
   {
      plugin = NULL;
   }

   // printf("xxx audioMasterGetTime=%d\n", audioMasterGetTime); // ==> 7

   // opcode 15: when Redux editor button is clicked to resize the window back to minimum
   //  opcode=15 index=868 value=0x00000000000000A1

   if( (44 != opcode) && (43 != opcode) && (3 != opcode) && (6 != opcode) && (4 != opcode) && (8 != opcode) && (7 != opcode) && (23 != opcode) && (0 != opcode) && (37 != opcode) && (33 != opcode) ) // filter audioMasterGetTime, audioMasterGetProcessLevel, audioMasterAutomate, audioMasterCanDo
   {
      if(1 && (lastOpcode != opcode))
         Dopprintf("[dbg] VST2Plugin::HostCallback: effect=0x%p opcode=%d index=%d value=0x%p ptr=0x%p opt=%f\n", effect, opcode, index, value, ptr, opt);
   }

   // (note) opcode documentation borrowed from <http://www.asseca.com/vst-24-specs/amCallbacks.html> ([20Jan2018] site is gone)

   switch(opcode)
   {
      case audioMasterAutomate: // 0
      {
         // plugin notifies host that the plugin has changed a parameter value thru its GUI or MIDI.
         // e[index]: parameter index
         // e[opt]: parameter value
         // x[return]: 0
         if(lastOpcode != audioMasterAutomate)
         {
            ////yac_host->printf("[dbg] tkvst2::HostCallback: rcvd audioMasterAutomate\n");
         }
         if(NULL != plugin)
         {
            plugin->callOnAudioMasterAutomate(index, opt);
         }
      }
      break;
         
      case audioMasterVersion: // 1, Allowed before VstPluginMain returns
         // plugin requests host VST-specification version, currently 2 (0 for older)
         // x[return]: Host VST version (for example 2400 for VST2.4)
         //yac_host->printf("[dbg] tkvst2::HostCallback: rcvd audioMasterVersion\n");
         // (note) Arturia Synclavier V expects "2" (else it won't load)
         result = 2;//kVstVersion;
         break;

      case audioMasterCurrentId: // 2
         // plugin requests host for current unique identifier
         // x[return]: current unique ID of a shell plug-in (in VST2.3 x[return]: the unique ID of the plug that is currently loaded)
         yac_host->printf("[dbg] tkvst2::HostCallback: rcvd audioMasterCurrentId\n");
         if(NULL != effect)
         {
            if(NULL != plugin)
            {
               if(0u != plugin->shell_plugin_uid)
               {
                  Dopprintf("[dbg] tkvst2::HostCallback<audioMasterCurrentId>: return shell_plugin_uid=0x%08x\n", plugin->shell_plugin_uid);
                  result = plugin->shell_plugin_uid;
               }
               else
               {
                  result = effect->uniqueID;
               }
            }
            else if(0u != VST2Plugin::static_shell_plugin_uid)
            {
               Dopprintf("[dbg] tksvst2::HostCallback<audioMasterCurrentId>: return static_shell_plugin_uid=0x%08x\n", (sUI)VST2Plugin::static_shell_plugin_uid);
               result = VST2Plugin::static_shell_plugin_uid;
            }
            else
            {
               result = effect->uniqueID;
            }
         }
         break;

      case audioMasterIdle: // 3
         // plugin calls host idle routine (this will call effEditIdle for all open editors too)
         // has no arguments
         if(lastOpcode != opcode)
         {
            //yac_host->printf("[dbg] tkvst2::HostCallback: rcvd audioMasterIdle (Subsequent calls will not be displayed!)");
         }
         break;

      case audioMasterPinConnected: // 4, **DEPRECATED**
         // plugin inquires host if an input or output is being connected
         // e[index]: enumerates input or output counting from zero
         // e[value]: 0 = input, else = output
         // x[return]: the return value is 0 for <true> such that older versions will always return true.
         if(lastOpcode != opcode)
         {
            // (note) GForce Oddity2 spams _a lot_ of these callbacks
            //yac_host->printf("[dbg] tkvst2::HostCallback: rcvd audioMasterPinConnected (**deprecated**)\n");
         }
         break;

         // ---- AEffectX:

      case audioMasterWantMidi: // 6, **DEPRECATED** . Allowed before VstPluginMain returns
         // (might need to be re-enabled to ensure 2.3/2.4 compatibility)
         // plugin (VSTi2.0 thru VSTi2.3) calls this to tell the host that the plugin is an instrument
         // e[value]: is a filter which is currently ignored
         if(lastOpcode != opcode)
         {
            // yac_host->printf("[dbg] tkvst2::HostCallback: rcvd audioMasterWantMidi (**deprecated**)\n");
         }
         result = 1;
         break;

      case audioMasterGetTime: // 7
         // e[value]: request mask, should contain a mask indicating which fields are required (see valid masks above), as some items may require extensive
         // conversions. For valid masks see VstTimeInfo.Flags
         // e[ptr]: returns VstTimeInfo* or null if not supported 

         // (note) Some plugins seem to call this for each sample during processReplacing() (??!!!)

         if((lastOpcode != audioMasterGetTime) || (lastTimeMask != value))
         {
            lastTimeMask = value;
            //yac_host->printf("[dbg] tkvst2::HostCallback: rcvd audioMasterGetTime requestMask=0x%08x\n", value); 
         }

         if(NULL != plugin)
         {
            plugin->time_info.tempo = tkvst2_bpm;
            plugin->time_info.ppqPos = tkvst2_song_pos_ppq;

            // (note) workaround for OB-E v2.0 (cuts off first note after replay has been started)
            sBool bReportTransportPlaying = plugin->b_report_transport_playing;

            if(bReportTransportPlaying)
            {
               if(tkvst2_song_playing)
                  plugin->time_info.flags |= kVstTransportPlaying;
               else
                  plugin->time_info.flags &= ~kVstTransportPlaying;

               if(tkvst2_song_playing != plugin->last_tkvst2_song_playing)
               {
                  plugin->time_info.flags |= kVstTransportChanged;
                  plugin->last_tkvst2_song_playing = tkvst2_song_playing;
                  // yac_host->printf("xxx audioMasterGetTime: kVstTransportChanged (%d)\n", tkvst2_song_playing);
               }
            }
            else
            {
               plugin->time_info.flags &= ~kVstTransportPlaying;
            }

            if(0)
            {
               static int xxx_audioMasterGetTime_count = 0;
               yac_host->printf("xxx audioMasterGetTime: [%d] BPM=%f\n", xxx_audioMasterGetTime_count++, plugin->time_info.tempo);
            }

            result = (VstIntPtr)  &plugin->time_info;
         }
         else
         {
            result = 0;
         }

         break;

      case audioMasterProcessEvents: // 8
         // plugin informs host that the plugin has MIDI events ready for the host to process
         // e[ptr]: pointer to VstEvents*
         // x[return]: 1 = supported and processed OK
         if(lastOpcode != opcode)
         {
            // yac_host->printf("[dbg] tkvst2::HostCallback: rcvd audioMasterProcessEvents\n");
         }
         break;

      case audioMasterSetTime: // **DEPRECATED**
         // e[value]: filter
         // e[ptr]: pointer to VstTimenfo*
         if(lastOpcode != opcode)
         {
            yac_host->printf("[dbg] tkvst2::HostCallback: rcvd audioMasterSetTime (**deprecated**)\n");
         }
         break;

      case audioMasterTempoAt: // **DEPRECATED**
         // ( might need to be enabled to ensure 2.3/2.4 compatibility)
         // e[value]: sample frame location
         // x[return]: tempo (in bpm * 10000)
         if(lastOpcode != opcode)
         {
            ////yac_host->printf("[dbg] tkvst2::HostCallback: rcvd audioMasterTempoAt (**deprecated**) BPM=%f\n", tkvst2_bpm);
         }
         result = (sSI) (10000 * tkvst2_bpm);
         break;

      case audioMasterGetNumAutomatableParameters: // **DEPRECATED**
         // plugin asks host number of automatable Parameters
         // x[return]: number of automatable Parameters (should be <= than numParams)
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterGetNumAutomatableParameters (**deprecated**)\n");
         }
         break;

      case audioMasterGetParameterQuantization: // **DEPRECATED**
         // e[value]: parameter index (-1: all, any)
         // x[return]: integer value for +1.0 representation, or 1 if full single float precision is maintained in automation. // connections, configuration
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterGetParameterQuantization (**deprecated**)\n");
         }
         break;
         
      case audioMasterIOChanged:
      {
         // plugin notifies host the numInputs and/or numOutputs of plugin has changed
         // x[return]: 1 = supported
         Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterIOChanged\n");

         if(NULL != plugin)
         {
            plugin->callOnAudioMasterIOChanged();
            result = 1;
         }
      }
      break;

      case audioMasterNeedIdle: // **DEPRECATED**
         // plugin needs idle calls (WM_TIMER)
         // has no arguments
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterNeedIdle (**deprecated**)\n");
         }
         break;

      case audioMasterSizeWindow:
         // plugin requests host to resize the window of the plugin editor
         // e[index]: new width in pixels
         // e[value]: new height in pizels
         // x[return]: 1 = supported
         // // if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterSizeWindow, geo=(%d; %d)\n", sSI(index), sSI(value));
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterSizeWindow, plugin=%p\n", plugin);

            if(NULL != plugin)
            {
            }
         }
         break;

      case audioMasterGetSampleRate:
         // x[return]: current sample rate
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterGetSampleRate\n");
         }
         break;

      case audioMasterGetBlockSize:
         // x[return]: current block size
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterGetBlockSize\n");
         }
         break;

      case audioMasterGetInputLatency:
         // plugin requests input latency from host
         // x[return]: input latency in samples
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterGetInputLatency\n");
         }
         break;

      case audioMasterGetOutputLatency:
         // plugin requests output latency from host
         // x[return]: output latency in samples
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterGetOutputLatency\n");
         }
         break;

      case audioMasterGetPreviousPlug: // **DEPRECATED**
         // e[value]: input pin (-1: first to come),
         // x[ptr]: host returns pointer to cEffect*
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterGetPreviousPlug (**deprecated**)\n");
         }
         break;

      case audioMasterGetNextPlug: // **DEPRECATED**
         // e[value]: output pin (-1: first to come),
         // x[ptr]: host returns pointer to cEffect*
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterGetNextPlug (**deprecated**)\n");
         }
         break;

      case audioMasterWillReplaceOrAccumulate: // **DEPRECATED**
         // x[return]: 0 = not supported, 1 = replace, 2 = accumulate
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterWillReplaceOrAccumulate (**deprecated**)\n");
         }
         break;

      case audioMasterGetCurrentProcessLevel:
         // x[return]: currentProcessLevel 
         //yac_host->printf("[dbg] tkvst2::HostCallback: rcvd audioMasterGetCurrentProcessLevel\n");
         break;

      case audioMasterGetAutomationState:
         // x[return]: 0 = not supported, 1 = off, 2 = read, 3 = write, 4 = read/write
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterGetAutomationState\n");
         }
         break;

      case audioMasterOfflineStart:
         // e[index]: number of new audio files to start
         // e[value]: number of audio files to start
         // e[ptr]: pointer to *audioFiles
         // x[return]: 0 = error, 1 = OK
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterOfflineStart\n");
         }
         break;

      case audioMasterOfflineRead:
         // e[index]: 1 = readSource
         // e[value]: option of type VstOfflineOption
         // e[ptr]: points to offline* offline structure, see below.
         // x[return]: 0 = error, 1 = OK
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterOfflineRead\n");
         }
         break;

      case audioMasterOfflineWrite:
         // e[index]: 1 = readSource
         // e[value]: option of type VstOfflineOption
         // e[ptr]: points to offline* offline structure, see below.
         // x[return]: 0 = error, 1 = OK
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterOfflineWrite\n");
         }
         break;

      case audioMasterOfflineGetCurrentPass:
         // x[return]: 0 = error, 1 = OK
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterOfflineGetCurrentPass\n");
         }
         break;

      case audioMasterOfflineGetCurrentMetaPass:
         // x[return]: 0 = error, 1 = OK
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterOfflineGetCurrentMetaPass\n");
         }
         break;

      case audioMasterSetOutputSampleRate: // **DEPRECATED**
         // for variable i/o
         // e[opt]: sample rate
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterSetOutputSampleRate (**deprecated**)\n");
         }
         break;

      case audioMasterGetOutputSpeakerArrangement: // **DEPRECATED**
         // x[return]: host returns a pointer to VstSpeakerArrangement* structure
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterGetOutputSpeakerArrangement (**deprecated**)\n");
         }
         break;

      case audioMasterGetVendorString: // 32, Allowed before VstPluginMain returns
         // x[ptr]: host returns a pointer to a string identifying the vendor (max 64 char)
         // x[return]: 0 = error, 1 = OK
         // if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterGetVendorString\n");
            char *d = (char*)ptr;
            ::snprintf(d, 32, "Unknown");
         }
         break;

      case audioMasterGetProductString: // Allowed before VstPluginMain returns
         // x[ptr]: host returns a pointer to a string with product name (max 64 char)
         // x[return]: 0 = error, 1 = OK
         // if(lastOpcode != opcode)
         {
            // Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterGetProductString\n");
            char *d = (char*)ptr;
            ::snprintf(d, 32, "Unknown");
         }
         break;

      case audioMasterGetVendorVersion: // Allowed before VstPluginMain returns
         // x[return]: vendor-specific version
         // if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterGetVendorVersion\n");
            result = 1;
         }
         break;

      case audioMasterVendorSpecific:
         // no definition, vendor specific handling
         // x[return]: 0 = not supported
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterVendorSpecific\n");
         }
         break;

      case audioMasterSetIcon: // **DEPRECATED**
         // e[ptr]: void*, format not defined yet
         // x[return]: 0 = error, 1 = OK
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterSetIcon (**deprecated**)\n");
         }
         break;

      case audioMasterCanDo: // 37
         // plugin queries if host 'cando'
         // e[ptr]: string-pointer to cando string
         // x[returns]: 0 = No, 1='cando', 0='don't know' (default)
         // // if(lastOpcode != opcode)
         {
            // "sendVstEvents",
            // "sendVstMidiEvent"
            // "sendVstTimeInfo"
            if(!strncmp((const char*)ptr, "sendVstTimeInfo", 15))
            {
               result = 1;
            }
            // "receiveVstEvents"
            // "receiveVstMidiEvent"
            // "receiveVstTimeInfo"
            // "reportConnectionChanges"
            // "acceptIOChanges"
            if(!strncmp((const char*)ptr, "sizeWindow", 10))
            {
               result = 1;
            }
            // "asyncProcessing"
            // "offline"
            // "supplyIdle"
            // "supportShell"
            // "openFileSelector"
            // "editFile"
            // "closeFileSelector"
            // "getChunkFile"
            else if(!strncmp((const char*)ptr, "NIMKPIVendorSpecificCallbacks", 29))
            {
               result = 0;
            }
            else
            {
               Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterCanDo (canDo=\"%s\")\n", (const char*)ptr);
            }
            // 
         }
         break;

      case audioMasterGetLanguage: // Allowed before VstPluginMain returns
         // plugin queries what language is set for the host
         // x[returns]: see enum VstHostLanguage
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterGetLanguage\n");
         }
         result = kVstLangEnglish;
         break;

      case audioMasterOpenWindow: // **DEPRECATED**
         // e[ptr]: pointer to window*
         // x[return]: host returns platform specific pointer*
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterOpenWindow (**deprecated**)\n");
         }
         break;

      case audioMasterCloseWindow: // **DEPRECATED**
         // close window,
         // e[ptr]: platform specific handle
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterCloseWindow (**deprecated**)\n");
         }
         break;

      case audioMasterGetDirectory: // Allowed before VstPluginMain returns
         // plugin requests host for plugin directory
         // x[ptr]: string-pointer to directory, FSSpec on MAC
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterGetDirectory\n");
         }
         break;

      case audioMasterUpdateDisplay:
      {
         // Something has changed, update the host's 'multi-fx' display.
         // x[return]: 1 = supported
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterUpdateDisplay\n");
         }

         result = 1;
      }
      break;

      case audioMasterBeginEdit:
         // plugin calls this before a setParameterAutomated with mouse move (one per Mouse Down). It tells the host that if it needs to, it has to record 
         // automation data for this control
         // e[index]: parameter index
         // x[return]: 1 = supported by host
         if(lastOpcode != opcode)
         {
            // Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterBeginEdit\n");
         }
         if(NULL != plugin)
         {
            plugin->callOnAudioMasterParamEdit(1/*bBegin*/, index);
         }
         break;

      case audioMasterEndEdit:
         // plugin calls this after a setParameterAutomated (on Mouse Up). It notifies the host that this control is no more moved by the mouse
         // e[index]: parameter index
         // x[return]: 1 = supported by host
         if(lastOpcode != opcode)
         {
            // Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterEndEdit\n");
         }
         if(NULL != plugin)
         {
            plugin->callOnAudioMasterParamEdit(0/*bBegin*/, index);
         }
         break;

      case audioMasterOpenFileSelector:
         // plugin requests: open a fileselector window
         // e[ptr]: pointer to VstFileSelect*
         // x[return]: 1 = supported by host
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterOpenFileSelector\n");
         }
         break;

      case audioMasterCloseFileSelector:
         // plugin requests: close a fileselector operation, must be always called after an openFileSelector !
         // e[ptr]: pointer to VstFileSelect*
         // x[return]: 1 = supported by host
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterCloseFileSelector\n");
         }
         break;

      case audioMasterEditFile: // **DEPRECATED**
         // open an editor for audio (defined by XML text in <ptr>)
         // e[ptr]: pointer to XML text
         // x[return]: 0 = error, 1 = OK
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterEditFile (**deprecated**)\n");
         }
         break;

      case audioMasterGetChunkFile: // **DEPRECATED**
         // plugin requests the native path of currently loading bank or project (called from writeChunk)
         // x[ptr]: pointer to char[2048] or (MAC)sizeof(FSSpec)
         // x[return]: 0 = error, 1 = OK
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterGetChunkFile (**deprecated**)\n");
         }
         break;

      case audioMasterGetInputSpeakerArrangement: // **DEPRECATED**
         // x[return]: host returns a pointer to VstSpeakerArrangement* structure
         if(lastOpcode != opcode)
         {
            Dopprintf("[dbg] tkvst2::HostCallback: rcvd audioMasterGetInputSpeakerArrangement (**deprecated**)\n");
         }
         break;

      default:
         if(lastOpcode != opcode)
         {
            Dopprintf ("[dbg] tkvst2::HostCallback: (opcode %d)\n index = %d, value = %p, ptr = %p, opt = %f\n", opcode, index, FromVstPtr<void> (value), ptr, opt);
         }
         break;
	}

   lastOpcode = opcode;

   // if( (opcode !=7) && (23 != opcode) ) // filter audioMasterGetTime, audioMasterGetProcessLevel
   // {
   //    Dopprintf("xxx end host callback opcode=%d\n", opcode);
   // }

	return result;
}


// (note) dispatcher args: index, value, ptr, opt(float)

VST2Plugin::VST2Plugin(void) {
   ////yac_host->->printf("xxx VST2Plugin::VST2Plugin: this=0x%p\n", this);
#ifdef YAC_MACOS
   cf_bundle_ref = NULL;
#endif // YAC_MACOS
   effect        = NULL;

   shell_plugins = NULL;
   num_shell_plugins = 0u;
   current_shell_plugin_idx = -1;

   block_size         = 0;
   num_input_buffers  = 0;
   num_output_buffers = 0;
   b_ext_input_buffers = YAC_FALSE;
   b_ext_output_buffers = YAC_FALSE;

   for(sUI i = 0u; i < VST2PLUGIN_MAX_IO_BUFFERS; i++)
   {
      input_buffers[i] = NULL;
      output_buffers[i] = NULL;
      collect_input_buffers[i] = NULL;
      collect_output_buffers[i] = NULL;
   }

   mtx_dispatcher = loc_pthread_mutex_t_init;
   mtx_events     = loc_pthread_mutex_t_init;

   initEvents();

   b_debug = YAC_FALSE;

   collect_num_chunks = 0u;
   collect_chunk_idx  = 0u;

   last_tkvst2_song_playing = YAC_FALSE;
   b_report_transport_playing = YAC_TRUE;
}

VST2Plugin::~VST2Plugin() {
   // yac_host->->printf("xxx VST2Plugin::~VST2Plugin: DTOR this=0x%p effect=0x%p\n", this, effect);
   // yac_host->->printf("xxx VST2Plugin::~VST2Plugin: DTOR effect=0x%p\n", effect);
   close();

   freeShellPlugins();
}

void VST2Plugin::StaticInit(void) {
   mtx_shell_plugin_uid = loc_pthread_mutex_t_init;
   static_shell_plugin_uid = 0u;

#ifdef YAC_WIN32
   // Create/open databridge host event handle
   databridge_host_ev = ::CreateEvent(NULL/*lpEventAttributes*/,
                                      FALSE/*bManualReset*/,
                                      FALSE/*bInitialState*/,
                                      DATABRIDGE_HOST_EVENT_NAME
                                      );

   if(NULL == databridge_host_ev)
   {
      yac_host->printf("[---] VST2Plugin::StaticInit: failed to create databridge_host_ev !!!\n");
   }
#endif // YAC_WIN32
}

void VST2Plugin::StaticExit(void) {
   // Close databridge host event handle
#ifdef YAC_WIN32
   ::CloseHandle(databridge_host_ev);
   databridge_host_ev = NULL;
#endif // YAC_WIN32
}

sBool VST2Plugin::WaitForDatabridgeHostEvent(sUI _timeout) {
#ifdef YAC_WIN32
   DWORD res = ::WaitForSingleObject(databridge_host_ev, _timeout);
   return (WAIT_OBJECT_0 == res) ? YAC_TRUE : YAC_FALSE;
#else
   (void)_timeout;
   return YAC_FALSE;
#endif // YAC_WIN32
}

void VST2Plugin::freeShellPlugins(void) {
   if(NULL != shell_plugins)
   {
      delete [] shell_plugins;
      shell_plugins = NULL;
      num_shell_plugins = 0u;
   }
}

void VST2Plugin::initEvents(void) {
   max_events      = 4096;
   max_events_mask = 4095;

   events = (VstEvents*) ::malloc(sizeof(VstEvents) + sizeof(VstMidiEvent*) * max_events);
   memset(events, 0, sizeof(VstEvents) + sizeof(VstMidiEvent*) * max_events);

   midi_events = (VstMidiEvent*) ::malloc(sizeof(VstMidiEvent) * max_events);
   memset(midi_events, 0, sizeof(VstMidiEvent) * max_events);

   sUI i;
   for(i=0; i<max_events; i++)
   {
      events->events[i] = (VstEvent*) &midi_events[i];

      midi_events[i].type            = kVstMidiType;
      midi_events[i].byteSize        = sizeof(VstMidiEvent);
      midi_events[i].deltaFrames     = 0;
      midi_events[i].flags           = kVstMidiEventIsRealtime;
      midi_events[i].noteLength      = 0;
      midi_events[i].noteOffset      = 0;
      midi_events[i].detune          = 0;
      midi_events[i].noteOffVelocity = 0;
      midi_events[i].reserved1       = 0;
      midi_events[i].reserved2       = 0;
   }

   next_event = 0;

   memset(&time_info, 0, sizeof(VstTimeInfo));

   time_info.tempo  = 125.0; // (note) updated during audioMasterGetTime
   time_info.ppqPos = 0.0;
   time_info.flags  = kVstTempoValid | kVstPpqPosValid;  // (note) kVstTransportPlaying is updated when queried
}

// void VST2Plugin::eventTest(void) {
//    printf("xxx VST2Plugin::eventTest\n");

//    static VstEvents events;
//    static VstEvents events2;
//    static VstMidiEvent midi[2];
//    memset(&events,  0, sizeof(VstEvents));
//    memset(&events2, 0, sizeof(VstEvents));

//    // Send two single events
//    //  ==> VSTs do not seem to like this (either pitchbend does not work or output is silent)

//    // // Note-on
//    // events.events[0] = (VstEvent*) & midi[0];
//    // events.numEvents = 1;

//    // midi[0].type     = kVstMidiType;
//    // midi[0].byteSize = sizeof(VstMidiEvent);
//    // midi[0].flags    = kVstMidiEventIsRealtime;
//    // midi[0].midiData[0] = 0x90;
//    // midi[0].midiData[1] = 48;
//    // midi[0].midiData[2] = 127;

//    // effect->dispatcher(effect, effProcessEvents, 0, 0, &events, 0.0f);

//    // // Pitch-bend
//    // events2.events[0] = (VstEvent*) & midi[1];
//    // events2.numEvents = 1;
//    // midi[1].type     = kVstMidiType;
//    // midi[1].byteSize = sizeof(VstMidiEvent);
//    // midi[1].flags    = kVstMidiEventIsRealtime;
//    // midi[1].midiData[0] = 0xE0;
//    // midi[1].midiData[1] = 0x00;
//    // midi[1].midiData[2] = 0x20;

//    // effect->dispatcher(effect, effProcessEvents, 0, 0, &events2, 0.0f);


//    Send two events at once

//    events.numEvents = 2;
//    events.events[0] = (VstEvent*) & midi[0];
//    events.events[1] = (VstEvent*) & midi[1];

//    midi[0].type     = kVstMidiType;
//    midi[0].byteSize = sizeof(VstMidiEvent);
//    midi[0].flags    = kVstMidiEventIsRealtime;
//    midi[0].midiData[0] = 0x90;
//    midi[0].midiData[1] = 48;
//    midi[0].midiData[2] = 127;

//    midi[1].type     = kVstMidiType;
//    midi[1].byteSize = sizeof(VstMidiEvent);
//    midi[1].flags    = kVstMidiEventIsRealtime;
//    midi[1].midiData[0] = 0xE0;
//    midi[1].midiData[1] = 0x00;
//    midi[1].midiData[2] = 0x20;

//    effect->dispatcher(effect, effProcessEvents, 0, 0, &events, 0.0f);
//}



void YAC_VCALL VST2Plugin::yacGetSignalStringList(YAC_String *_sig) {
   // ---- the number represents the encoded argument type identifiers used by this function
   // ---- 1=int, 2=float, 3=object. bits 0+1 represent argument #0, bits 2+3 represent argument #1 and so on
   _sig->visit("onAudioMasterUpdateDisplay:3 onAudioMasterAutomate:39 onAudioMasterIOChanged:3 onWin32KeyEvent:7 onAudioMasterParamEdit:23");
}

void YAC_VCALL VST2Plugin::yacRegisterSignal(sUI _id, YAC_FunctionHandle _fun) {
   if(_id < VST2PLUGIN_NUM_SIGNALS)
   {
      tkvst2_audio_master_signal_funs[_id] = _fun;
   }
}

void VST2Plugin::callOnAudioMasterUpdateDisplay(void) {
   void *f = tkvst2_audio_master_signal_funs[VST2PLUGIN_SIGNAL_ONAUDIOMASTERUPDATEDISPLAY];
   ////yac_host->printf("xxx callOnAudioMasterUpdateDisplay f=0x%p\n", f);
   if(f)
   {
      YAC_Value args[1];
      args[0].initObject(this, 0);
      ////yac_host->yacMutexFindByName("__DEBUG");
      yac_host->yacEvalFunction(tkvst2_audio_master_script_context, f, 1, args);
   }
}

void VST2Plugin::callOnAudioMasterAutomate(sSI _paramIdx, sF32 _value) {

   // yac_host->printf("xxx VST2Plugin::callOnAudioMasterAutomate: call setParameter: idx=%d val=%f\n", _paramIdx, _value);
#if 0
   // (note) [11Jan2022] breaks OPS7 (v1.006) patch loading (plugin sends stale parameter value to host during effSetChunk).
   //                    however: why echo the parameter change back to the plugin in the first place ?
   beginDispatch();
   effect->setParameter(effect, _paramIdx, _value);
   endDispatch();
#endif

   void *f = tkvst2_audio_master_signal_funs[VST2PLUGIN_SIGNAL_ONAUDIOMASTERAUTOMATE];

   ////yac_host->->printf("xxx callOnAudioMasterAutomate: f=0x%p\n", f);
   if(f)
   {
      YAC_Value args[3];
      args[0].initObject(this, 0);
      args[1].initInt   (_paramIdx);
      args[2].initFloat (_value);
      yac_host->yacEvalFunction(tkvst2_audio_master_script_context, f, 3, args);
      ////yac_host->->printf("xxx callOnAudioMasterAutomate: f call returned\n");
   }
}

void VST2Plugin::callOnAudioMasterIOChanged(void) {
#if 0
   // problematic with Softube Tape (calls IOChanged during effSetBlockSize)
   beginEvents(); // to make sure that processReplacing() is not running concurrently
   reallocateIOBuffers();
   endEvents();
#endif

   void *f = tkvst2_audio_master_signal_funs[VST2PLUGIN_SIGNAL_ONAUDIOMASTERIOCHANGED];

   ////yac_host->->printf("xxx callOnAudioMasterIOChanged: f=0x%p\n", f);
   if(f)
   {
      YAC_Value args[1];
      args[0].initObject(this, 0);
      yac_host->yacEvalFunction(tkvst2_audio_master_script_context, f, 1, args);
      ////yac_host->->printf("xxx callOnAudioMasterIOChanged: f call returned\n");
   }
}

void VST2Plugin::callOnAudioMasterParamEdit(sSI _bBegin, sSI _paramIdx) {
   void *f = tkvst2_audio_master_signal_funs[VST2PLUGIN_SIGNAL_ONAUDIOMASTERPARAMEDIT];
   ////yac_host->printf("xxx callOnAudioMasterParamEdit f=0x%p\n", f);
   if(f)
   {
      YAC_Value args[3];
      args[0].initObject(this, 0);
      args[1].initInt(_bBegin);
      args[2].initInt(_paramIdx);
      yac_host->yacEvalFunction(tkvst2_audio_master_script_context, f, 3, args);
   }
}


sBool VST2Plugin::loadPlugin(YAC_String *_pathName, sUI _selectShellPluginUID) {

   effect_vendor[0] = 0;
   effect_name[0] = 0;
   current_shell_plugin_idx = -1;

   freeShellPlugins();

   b_debug_paramproperties_once = true;

   shell_plugin_uid = _selectShellPluginUID;

   if(YAC_Is_String(_pathName))
   {
      yac_host->printf("[dbg] VST2Plugin:loadPlugin(\"%s\", shellUID=0x%08x)\n", (const char*)_pathName->chars, _selectShellPluginUID);

#ifdef YAC_MACOS
      // open-bundle code adapted from <http://teragonaudio.com/article/How-to-make-your-own-VST-host.html>
      CFStringRef pluginPathStringRef = CFStringCreateWithCString(NULL,
                                                                  (char*)_pathName->chars,
                                                                  kCFStringEncodingASCII
                                                                  );
      CFURLRef bundleUrl = CFURLCreateWithFileSystemPath(kCFAllocatorDefault,
                                                         pluginPathStringRef,
                                                         kCFURLPOSIXPathStyle,
                                                         true
                                                         );
      if(NULL == bundleUrl)
      {
         yac_host->printf("[---] VST2Plugin::loadPlugin: failed to create CFURLRef for plugin path\n");
         return YAC_FALSE;
      }
    
      cf_bundle_ref = CFBundleCreate(kCFAllocatorDefault, bundleUrl);
      CFRelease(pluginPathStringRef);
      CFRelease(bundleUrl);
      if(NULL == cf_bundle_ref)
      {
         yac_host->printf("[---] VST2Plugin::loadPlugin: failed to create bundle reference\n");
         return YAC_FALSE;
      }
     
      PluginEntryProc mainProc = (PluginEntryProc) CFBundleGetFunctionPointerForName(cf_bundle_ref,
                                                                                     CFSTR("VSTPluginMain")

                                                                                     );

      if(NULL == mainProc)
      {
         mainProc = (PluginEntryProc) CFBundleGetFunctionPointerForName(cf_bundle_ref,
                                                                        CFSTR("main_macho")
                                                                        );
      }

      if(NULL != mainProc)
      {
         yac_host->printf("[dbg] VST2Plugin::loadPlugin: creating effect. mainProc=0x%p\n", mainProc);

         if(0u != _selectShellPluginUID)
         {
            pthread_mutex_lock(&mtx_shell_plugin_uid);
            static_shell_plugin_uid = _selectShellPluginUID;
         }

         effect = mainProc(HostCallback);

         yac_host->printf("[dbg] VST2Plugin::loadPlugin: mainProc() returned. effect=%p\n", effect);

         if(0u != _selectShellPluginUID)
         {
            static_shell_plugin_uid = 0u;
            pthread_mutex_unlock(&mtx_shell_plugin_uid);
         }

         if(NULL != effect)
         {
            yac_host->printf("[...] VST2Plugin::loadPlugin: OK, plugin openend and effect instance created.\n");

            effect->resvd1 = (VstIntPtr) this;
            // // effect->resvd2 = (VstIntPtr) _selectShellPluginUID;
            effect->resvd2 = (VstIntPtr) NULL;

            yac_host->printf("[dbg] VST2Plugin::loadPlugin: call effGetVendorString\n");
            beginDispatch();
            effect->dispatcher(effect, effGetVendorString, 0, 0, (void*)effect_vendor, 0.0f);
            endDispatch();
            effect_vendor[kVstMaxVendorStrLen-1] = 0;
            yac_host->printf("[dbg] VST2Plugin::loadPlugin: effGetVendorString returned (\"%s\")\n", effect_vendor);

            sSI category = getCategory();

            if(kPlugCategShell == category)
            {
               sUI shellPluginIdx = 0;
               yac_host->printf("[dbg] VST2Plugin::loadPlugin: ** is shell plugin **\n");
               shell_plugins = new vst2_shell_plugin_t[VST2PLUGIN_MAX_SHELL_PLUGINS];
               memset((void*)shell_plugins, 0, sizeof(vst2_shell_plugin_t) * VST2PLUGIN_MAX_SHELL_PLUGINS);
               for(;;)
               {
                  vst2_shell_plugin_t *shellPlugin = &shell_plugins[shellPluginIdx];

                  shellPlugin->effect_name[0] = 0;
                  beginDispatch();
                  sUI shellPluginUID = (sUI)effect->dispatcher(effect, effShellGetNextPlugin, 0, 0, (void*)shellPlugin->effect_name, 0.0f);
                  endDispatch();
                  shellPlugin->uid = shellPluginUID;

                  if(b_debug && (0u == _selectShellPluginUID))
                  {
                     yac_host->printf("[dbg] VST2Plugin::loadPlugin: shell plugin [%u] name=\"%s\" UID=0x%08x\n", shellPluginIdx, shellPlugin->effect_name, shellPlugin->uid);
                  }

                  if((0 == shellPluginUID) || (0 == shellPlugin->effect_name[0]))
                     break;

                  // // if((0u != _selectShellPluginUID) && (shellPluginUID == _selectShellPluginUID))
                  // // {
                  // //    current_shell_plugin_idx = (sSI)shellPluginIdx;
                  // //    break;
                  // // }

                  // Next sub-plugin
                  shellPluginIdx++;
               }
               num_shell_plugins = shellPluginIdx;
               // // if(0u != _selectShellPluginUID)
               // // {
               // //    yac_host->printf("[dbg] VST2Plugin::loadPlugin: select sub-plugin UID=0x%08x in shell vendor=\"%s\"\n", _selectShellPluginUID, effect_vendor);
               // // }
               // // else
               // // {
               yac_host->printf("[dbg] VST2Plugin::loadPlugin: found %u plugins in shell vendor=\"%s\"\n", num_shell_plugins, effect_vendor);
               // // }
            }
            else
            {
               yac_host->printf("[dbg] VST2Plugin::loadPlugin: call effGetEffectName\n");
               beginDispatch();
               effect->dispatcher(effect, effGetEffectName, 0, 0, (void*)effect_name, 0.0f);
               endDispatch();
               effect_name[kVstMaxEffectNameLen-1] = 0;
               yac_host->printf("[dbg] VST2Plugin::loadPlugin: effGetEffectName returned (\"%s\")\n", effect_name);
            }

            // // if((0u != _selectShellPluginUID) && (-1 == current_shell_plugin_idx))
            // // {
            // //    yac_host->printf("[---] VST2Plugin::loadPlugin: failed to select shell plugin UID=0x%08x, vendor=\"%s\".\n", _selectShellPluginUID, effect_vendor);
            // //    // Failed
            // //    return YAC_FALSE;
            // // }
            // // else
            // // {
            // Succeeded
            return YAC_TRUE;
            // // }
         }

         yac_host->printf("[---] VST2Plugin::loadPlugin: failed to create effect instance! (pathName=\"%s\")\n", _pathName->chars);
      }
      else
      {
         yac_host->printf("[---] VST2Plugin::loadPlugin: mainProc is NULL (pathName=\"%s\").\n", (const char*)_pathName->chars);
      }
#else
#error (todo) implement linux/win32 version
#endif // YAC_MACOS

   }
   else
   {
      yac_host->printf("[---] VST2Plugin::loadPlugin: pathName argument is not a String.\n");
   }

   closeLibrary();

   // Failed
   return YAC_FALSE;
}

void VST2Plugin::open(void) {
   if(NULL != effect)
   {
      beginDispatch();
      effect->dispatcher(effect, effOpen, 0, 0, NULL, 0.0f);
      endDispatch();
   }
}

void VST2Plugin::freeIOBuffers(void) {

   // Inputs
   if(num_input_buffers > 0u)
   {
      if(YAC_FALSE == b_ext_input_buffers)
      {
         yac_host->printf("[dbg] VST2Plugin::freeIOBuffers: freeing %d input buffers\n", num_input_buffers);

         sUI i = 0;
         for(i=0; i<num_input_buffers; i++)
         {
            delete [] input_buffers[i];
            input_buffers[i] = NULL;
         }
      }

      num_input_buffers = 0;
   }

   // Outputs
   if(num_output_buffers > 0u)
   {
      if(YAC_FALSE == b_ext_output_buffers)
      {
         yac_host->printf("[dbg] VST2Plugin::freeIOBuffers: freeing %d output buffers\n", num_output_buffers);

         sUI i = 0;
         for(i=0; i<num_output_buffers; i++)
         {
            delete [] output_buffers[i];
            output_buffers[i] = NULL;
         }
      }
      num_output_buffers = 0;
   }

   freeIOBuffersCollect();
}

void VST2Plugin::closeEffect(void) {
   // Shut down effect
   if(NULL != effect)
   {
      yac_host->printf("[dbg] VST2Plugin::close: closing effect..\n");
      beginDispatch();
      effect->dispatcher(effect, effClose, 0, 0, NULL, 0.0f);
      endDispatch();
      yac_host->printf("[...] VST2Plugin::close: OK, effect closed.\n");
      effect = NULL;
   }
}

void VST2Plugin::closeLibrary(void) {
   // Close library
   if(NULL != cf_bundle_ref)
   {
      yac_host->printf("[dbg] VST2Plugin::close: closing bundle..\n");

      // CFBundleUnloadExecutable(cf_bundle_ref);

      // (note) does not really unload the plugin, resulting in warnings like
      //   objc[6065]: Class CSoftubeAccessibilityMacElement is implemented in both /Library/Audio/Plug-Ins/VST/Softube/Tonelux Tilt Live.vst/Contents/MacOS/TiltLive_VST_AU_Protect (0x2eafe69a8) and /Library/Audio/Plug-Ins/VST/Softube/Transient Shaper.vst/Contents/MacOS/TransientShaper_VST_AU_Protect (0x38ffb69a0). One of the two will be used. Which one is undefined.
      CFRelease(cf_bundle_ref);

      cf_bundle_ref = NULL;

      yac_host->printf("[...] VST2Plugin::close: OK, bundle closed..\n");
   }
}

void VST2Plugin::freeEvents(void) {

   if(NULL != events)
   {
      ::free(events);
      events = NULL;
   }

   if(NULL != midi_events)
   {
      ::free(midi_events);
      midi_events = NULL;
   }

   max_events = 0;
   next_event = 0;
}

void VST2Plugin::close(void) {

   closeEditor();
   closeEffect();
   closeLibrary();
   freeIOBuffers();
   freeEvents();

   b_ext_input_buffers = YAC_FALSE;
   b_ext_output_buffers = YAC_FALSE;
}

sSI VST2Plugin::getCategory(void) {
   if(NULL != effect)
   {
      sSI i;

      if(0 == effect->numInputs)
      {
         // Fix Waves CODEX category
         i = VST2PLUGIN_CATEGORY_SYNTH;
      }
      else
      {
         beginDispatch();
         VstIntPtr ip = effect->dispatcher(effect, effGetPlugCategory, 0, 0, NULL, 0.0f);
         i = sSI(ip);
         endDispatch();
      }

      return i;
   }
   return kPlugCategUnknown;
}

sBool VST2Plugin::isEffect(void) {
#if 0
   sSI cat = getCategory();
   sBool r;
   if(kPlugCategUnknown != cat)
   {
      r = ! ((kPlugCategSynth == cat) || (kPlugCategGenerator == cat));
   }
   else
   {
      r = YAC_FALSE;
   }
#else
   sBool r = (0 == (effect->flags & effFlagsIsSynth));
#endif
   return r;
}

sBool VST2Plugin::isInstrument(void) {
#if 0
   sSI cat = getCategory();
   sBool r;
   if(kPlugCategUnknown != cat)
   {
      r = ((kPlugCategSynth == cat) || (kPlugCategGenerator == cat));
   }
   else
   {
      r = YAC_FALSE;
   }
#else
   sBool r = (0 != (effect->flags & effFlagsIsSynth));
#endif
   return r;
}

void VST2Plugin::suspend(void) {
   if(NULL != effect)
   {
      beginDispatch();
      effect->dispatcher(effect, effMainsChanged, 0, 0, NULL, 0.0f);
      endDispatch();
   }
}

void VST2Plugin::resume(void) {
   if(NULL != effect)
   {
      beginDispatch();
      effect->dispatcher(effect, effMainsChanged, 0, 1, NULL, 0.0f);
      endDispatch();
   }
}

void VST2Plugin::startProcess(void) {
   if(NULL != effect)
   {
      beginDispatch();
      effect->dispatcher(effect, effStartProcess, 0, 0, NULL, 0.0f);
      endDispatch();
   }
}

void VST2Plugin::stopProcess(void) {
   if(NULL != effect)
   {
      beginDispatch();
      effect->dispatcher(effect, effStopProcess, 0, 0, NULL, 0.0f);
      endDispatch();
   }
}

void VST2Plugin::setSampleRate(sF32 _rate) {
   if(NULL != effect)
   {
      beginDispatch();
      effect->dispatcher(effect, effSetSampleRate, 0, 0, NULL, _rate);
      endDispatch();

      time_info.sampleRate = _rate;
   }
}

void VST2Plugin::setBlockSize(sUI _blkSize) {
   // printf("xxx VST2Plugin::setBlockSize(blkSize=%u) effect=%p\n", _blkSize, effect);
   if(NULL != effect)
   {
      // Inform plugin (MUST be suspended!)
      beginDispatch();
      effect->dispatcher(effect, effSetBlockSize,  0, (sSI)_blkSize, NULL, 0.0f);
      endDispatch();

      block_size = _blkSize;
      collect_num_chunks = 0u;

      reallocateIOBuffers();
   }
   // printf("xxx VST2Plugin::setBlockSize: END block_size=%u _blkSize=%u\n", block_size, _blkSize);
}

void VST2Plugin::setBlockSizeCollect(sUI _blockSize, sUI _num) {

   if(NULL != effect)
   {
      beginEvents();  // sync with processReplacing()

      // Inform plugin (MUST be suspended!)
      beginDispatch();
      effect->dispatcher(effect, effSetBlockSize,  0, (sSI)block_size * _num, NULL, 0.0f);
      endDispatch();

      block_size = _blockSize;
      collect_num_chunks = _num;
      collect_chunk_idx = 0u;

      reallocateIOBuffers();

      endEvents();
   }
}

sBool VST2Plugin::setExtInputBuffer(sUI _idx, YAC_Object *_fa) {
   sBool r = YAC_FALSE;

   if(_idx < VST2PLUGIN_MAX_IO_BUFFERS)
   {
      if(YAC_BCHK(_fa, YAC_CLID_FLOATARRAY))
      {
         YAC_CAST_ARG(YAC_FloatArray, fa, _fa);
         input_buffers[_idx] = (sF32*)fa->yacArrayGetPointer();
         b_ext_input_buffers = YAC_TRUE;
      }
      else
      {
         input_buffers[_idx] = NULL;
      }

      r = true;
   }
   else
   {
      yac_host->printf("[---] VST2Plugin::setExtInputBuffer: idx=%u >= %u\n", _idx, VST2PLUGIN_MAX_IO_BUFFERS);
   }

   return r;
}

sBool VST2Plugin::setExtOutputBuffer(sUI _idx, YAC_Object *_fa) {
   sBool r = YAC_FALSE;

   if(_idx < VST2PLUGIN_MAX_IO_BUFFERS)
   {
      if(YAC_BCHK(_fa, YAC_CLID_FLOATARRAY))
      {
         YAC_CAST_ARG(YAC_FloatArray, fa, _fa);
         output_buffers[_idx] = (sF32*)fa->yacArrayGetPointer();
         b_ext_output_buffers = YAC_TRUE;
      }
      else
      {
         output_buffers[_idx] = NULL;
      }

      r = true;
   }
   else
   {
      yac_host->printf("[---] VST2Plugin::setExtOutputBuffer: idx=%u >= %u\n", _idx, VST2PLUGIN_MAX_IO_BUFFERS);
   }

   return r;
}

void VST2Plugin::reallocateIOBuffers(void) {
   // called by setBlockSize and audioMasterIOChanged host opcode dispatcher
   
   // Free old I/O buffers 
   freeIOBuffers();

   // (Re-) allocate sample buffers
   if(block_size > 0)
   {
      num_input_buffers = effect->numInputs;

      if(YAC_FALSE == b_ext_input_buffers)
      {
         // Allocate input buffers
         yac_host->printf("[dbg] VST2Plugin::reallocateIOBuffers: allocating %d input buffers (%d frames)\n", num_input_buffers, block_size);
         if(num_input_buffers > 0)
         {
            sUI i;
            for(i=0; i<num_input_buffers; i++)
            {
               input_buffers[i] = new sF32[block_size];
               if(NULL != input_buffers[i])
               {
                  memset(input_buffers[i], 0, sizeof(sF32) * block_size);
               }
               else
               {
                  yac_host->printf("[---] VST2Plugin::reallocateIOBuffers: failed to allocate input buffer #%u (%u samples)\n", i, block_size);
               }
            }
            for(;i < VST2PLUGIN_MAX_IO_BUFFERS; i++)
            {
               input_buffers[i] = NULL;
            }
         }
      }

      num_output_buffers = effect->numOutputs;

      if(YAC_FALSE == b_ext_output_buffers)
      {
         // Allocate output buffers
         yac_host->printf("[dbg] VST2Plugin::reallocateIOBuffers: allocating %d output buffers (%d frames)\n", num_output_buffers, block_size);
         if(num_output_buffers > 0)
         {
            sUI i;
            for(i=0; i<num_output_buffers; i++)
            {
               output_buffers[i] = new sF32[block_size];
               if(NULL != output_buffers[i])
               {
                  memset(output_buffers[i], 0, sizeof(sF32) * block_size);
               }
               else
               {
                  yac_host->printf("[---] VST2Plugin::reallocateIOBuffers: failed to allocate output buffer #%u (%u samples)\n", i, block_size);
               }
            }
            for(;i < VST2PLUGIN_MAX_IO_BUFFERS; i++)
            {
               output_buffers[i] = NULL;
            }
         }
      }

      reallocateIOBuffersCollect();
   }
   else
   {
      yac_host->printf("[~~~] VST2Plugin::reallocateIOBuffers: _blkSize is 0 ?!!\n");
   }
}

void VST2Plugin::reallocateIOBuffersCollect(void) {

   freeIOBuffersCollect();

   if(block_size > 0)
   {
      if(collect_num_chunks > 1u)
      {
         for(sUI i = 0u; i < num_input_buffers; i++)
         {
            collect_input_buffers[i] = new sF32[block_size * collect_num_chunks * 2];
            memset(collect_input_buffers[i], 0, sizeof(sF32) * block_size * collect_num_chunks * 2);
         }
         
         for(sUI i = 0u; i < num_output_buffers; i++)
         {
            collect_output_buffers[i] = new sF32[block_size * collect_num_chunks * 2];
            memset(collect_output_buffers[i], 0, sizeof(sF32) * block_size * collect_num_chunks * 2);
         }
      }
   }
}

void VST2Plugin::freeIOBuffersCollect(void) {
   for(sUI i = 0u; i < num_input_buffers; i++)
   {
      if(NULL != collect_input_buffers[i])
      {
         delete [] collect_input_buffers[i];
         collect_input_buffers[i] = NULL;
      }
   }

   for(sUI i = 0u; i < num_output_buffers; i++)
   {
      if(NULL != collect_output_buffers[i])
      {
         delete [] collect_output_buffers[i];
         collect_output_buffers[i] = NULL;
      }
   }
}

void VST2Plugin::dispatchGetString(sSI _opcode, sSI _idx, sSI _maxLen, YAC_Value *_r) {
   if(NULL != effect)
   {
      char buf[VST2PLUGIN_MAX_STRING_LEN]; // because of buggy plugins that cause buffer overflows
      memset(buf, 0, VST2PLUGIN_MAX_STRING_LEN);

      YAC_String *s = YAC_New_String();
      s->alloc(_maxLen + 1);

      beginDispatch();
      effect->dispatcher(effect, _opcode, _idx, 0, (void*)buf, 0.0f);
      endDispatch();

      sUI i;
      for(i=0; i<((sUI)_maxLen); i++)
      {
         s->chars[i] = buf[i];
      }
      s->chars[i] = 0;
      s->fixLength();

#ifdef PARANOIA_CHECK_DISPATCH_STRING_OVERFLOW
      i = _maxLen;
      for(; i<VST2PLUGIN_MAX_STRING_LEN; i++)
      {
         if(buf[i])
         {
            yac_host->printf("[~~~] VST2Plugin::dispatchGetString: plugin tries to cause buffer overflow!! (writeIdx %u > maxLen %d) (opcode=%d idx=%d maxLen=%d)\n", i, _maxLen, _opcode, _idx, _maxLen);
            break;
         }
      }
#endif // PARANOIA_CHECK_DISPATCH_STRING_OVERFLOW 
      
      _r->initString(s, 1);
   }
}

sSI VST2Plugin::dispatchGetInt(sSI _opcode) {
   if(NULL != effect)
   {
      beginDispatch();
      VstIntPtr ip;
      ip = effect->dispatcher(effect, _opcode, 0, 0, NULL, 0.0f);
      sSI i = sSI(ip);
      endDispatch();
      return i;
   }
   else
   {
      yac_host->printf("[---] VST2Plugin::dispatchGetInt: effect is NULL.\n");
      return 0;
   }
}

void VST2Plugin::getEffectName(YAC_Value *_r) {
   dispatchGetString(effGetEffectName, 0, kVstMaxEffectNameLen, _r);
}

void VST2Plugin::getVendorString(YAC_Value *_r) {
   dispatchGetString(effGetVendorString, 0, kVstMaxVendorStrLen, _r);
}

void VST2Plugin::getProductString(YAC_Value *_r) {
   dispatchGetString(effGetProductString, 0, kVstMaxProductStrLen, _r);
}

sSI VST2Plugin::getVendorVersion(void) {
   return dispatchGetInt(effGetVendorVersion);
}

sSI VST2Plugin::getVersion(void) {
   if(NULL != effect)
   {
      return effect->version;
   }
   return 0;
}

sSI VST2Plugin::getUniqueID(void) {
   if(NULL != effect)
   {
      return effect->uniqueID;
   }
   return 0;
}

sSI VST2Plugin::canDo(YAC_String *_text) {
   if(YAC_Is_String(_text))
   {
      // "sendVstEvents"
      // "sendVstMidiEvent"
      // "sendVstTimeInfo"
      // "receiveVstEvents"
      // "receiveVstMidiEvent"
      // "receiveVstTimeInfo"
      // "offline"
      // "plugAsChannelInsert"
      // "plugAsSend"
      // "mixDryWet"
      // "noRealTime"
      // "multipass"
      // "metapass"
      // "1in1out"
      // "1in2out"
      // "2in1out"
      // "2in2out"
      // "2in4out"
      // "4in2out"
      // "4in4out"
      // "4in8out"
      // "8in4out"
      // "8in8out"
      // "midiProgramNames"
      // "conformsToWindowRules"

      if(NULL != effect)
      {
         sSI i;
         beginDispatch();
         i = (VstInt32) effect->dispatcher(effect, effCanDo, 0, 0, (void*)_text->chars, 0.0f);
         endDispatch();
         return i;
      }
   }
   else
   {
      yac_host->printf("[---] VST2Plugin::canDo: _text argument is not a String.\n");
   }
   return 0;
}

sUI VST2Plugin::getFlags(void) {
   if(NULL != effect)
   {
      return effect->flags;
   }
   return 0u;
}

sBool VST2Plugin::isSynth(void) {
   if(NULL != effect)
   {
      return (effFlagsIsSynth == (effect->flags & effFlagsIsSynth));
   }
   return YAC_FALSE;
}

sBool VST2Plugin::hasEditor(void) {
   if(NULL != effect)
   {
      // // return (effFlagsHasEditor == (effect->flags & effFlagsHasEditor));
   }
   return YAC_FALSE;
}

sBool VST2Plugin::canReplacing(void) {
   if(NULL != effect)
   {
      return (effFlagsCanReplacing == (effect->flags & effFlagsCanReplacing));
   }
   return YAC_FALSE;
}

sBool VST2Plugin::programsAreChunks(void) {
   if(NULL != effect)
   {
      return (effFlagsProgramChunks == (effect->flags & effFlagsProgramChunks));
   }
   return YAC_FALSE;
}

sBool VST2Plugin::noSoundInStop(void) {
   if(NULL != effect)
   {
      return (effFlagsNoSoundInStop == (effect->flags & effFlagsNoSoundInStop));
   }
   return YAC_FALSE;
}

sSI VST2Plugin::getNumMidiInputChannels(void) {
   if(NULL != effect)
   {
      beginDispatch();
      VstIntPtr ip;
      ip = effect->dispatcher(effect, effGetNumMidiInputChannels, 0, 0, NULL, 0.0f);
      sSI i = sSI(ip);
      endDispatch();
      return i;
   }
   return 0;
}

sSI VST2Plugin::getNumMidiOutputChannels(void) {
   if(NULL != effect)
   {
      beginDispatch();
      VstIntPtr ip;
      ip = effect->dispatcher(effect, effGetNumMidiOutputChannels, 0, 0, NULL, 0.0f);
      sSI i = sSI(ip);
      endDispatch();
      return i;
   }
   return 0;
}

sSI VST2Plugin::getInitialDelay(void) {
   if(NULL != effect)
   {
      return effect->initialDelay;
   }
   return 0;
}

void VST2Plugin::idle(void) {
   if(NULL != effect)
   {
      // **deprecated**
      beginDispatch();
      effect->dispatcher(effect, effIdle, 0, 0, NULL, 0.0f);
      endDispatch();
   }
}

void VST2Plugin::queueEvent(sU8 _0, sU8 _1, sU8 _2, sU8 _3) {

   beginEvents();

   if(NULL != effect)
   {
      VstMidiEvent *ev = &midi_events[next_event];

      ev->midiData[0] = (char) _0;
      ev->midiData[1] = (char) _1;
      ev->midiData[2] = (char) _2;
      ev->midiData[3] = (char) _3;

      // yac_host->printf("xxx VST2Plugin::queueEvent: data= 0x%02x 0x%02x 0x%02x 0x%02x\n", 
      //        (sU8) ev->midiData[0], 
      //        (sU8) ev->midiData[1], 
      //        (sU8) ev->midiData[2], 
      //        (sU8) ev->midiData[3]
      //        );

      next_event = (next_event + 1) & max_events_mask;
   }

   endEvents();
}

void VST2Plugin::queueNoteOn(sSI _midiChannel, sSI _note, sSI _velocity) {
   ////yac_host->printf("xxx queueNoteOn note=%d vel=%d\n", _note, _velocity);

   if(_velocity < 0)
      _velocity = 0;
   else if(_velocity > 127)
      _velocity = 127;

   queueEvent((sU8) (0x90 | (_midiChannel & 15)),
              (sU8)(_note & 127),
              (sU8)(_velocity & 127)
              );
}

void VST2Plugin::queueNoteOff(sSI _midiChannel, sSI _note, sSI _velocity) {
   if(_velocity < 0)
      _velocity = 0;
   else if(_velocity > 127)
      _velocity = 127;

   queueEvent((sU8)(0x80 | (_midiChannel & 15)),
              (sU8)(_note & 127),
              (sU8)(_velocity & 127)
              );
}

void VST2Plugin::queueChannelPressure(sSI _midiChannel, sSI _value) {
   if(_value < 0)
      _value = 0;
   else if(_value > 127)
      _value = 127;

   queueEvent((sU8) (0xD0 | (_midiChannel & 15)),
              (sU8) (_value & 127)
              );
}

void VST2Plugin::queueCtlChange(sSI _midiChannel, sSI _ctl, sSI _value) {
   queueEvent((sU8) (0xB0 | (_midiChannel & 15)),
              (sU8) (_ctl & 127),
              (sU8) (_value & 127)
              );
}

void VST2Plugin::queueBankSelect(sSI _midiChannel, sSI _bankMSB, sSI _bankLSB) {
   queueCtlChange(_midiChannel,    0, _bankMSB);
   queueCtlChange(_midiChannel, 0x20, _bankLSB);
}

void VST2Plugin::queueProgramChange(sSI _midiChannel, sSI _program) {
   queueEvent((sU8) (0xC0 | (_midiChannel & 15)),
              (sU8) (_program & 127)
              );
}

void VST2Plugin::queuePitchbend(sSI _midiChannel, sSI _pitchMSB, sSI _pitchLSB) {
   queueEvent((sU8) (0xE0 | (_midiChannel & 15)),
              (sU8) (_pitchLSB & 127),
              (sU8) (_pitchMSB & 127)
              );
}

void VST2Plugin::queuePitchbendf(sSI _midiChannel, sF32 _pitchbend) {
   // _pitchbend must be in -1..1 range

   sU16 val;

   if(_pitchbend >= 0.0f)
   {
      val = 0x2000 + (sU16) (_pitchbend * 8191);
   }
   else
   {
      val = 0x2000 + (sU16) (_pitchbend * 8192);
   }

   queueEvent((sU8) (0xE0 | (_midiChannel & 15)),
              (sU8) (val & 127),
              (sU8) ((val>>7) & 127)
              );
}

void VST2Plugin::queueChannelVolumef(sSI _midiChannel, sF32 _vol) {
   // _vol must be in 0..127 range

   if(_vol >= 1.0f)
   {
      _vol = 1.0f;
   }

   queueEvent((sU8) (0xB0 | (_midiChannel & 15)),
              (sU8) (0x07),
              (sU8) (_vol * 127)
              );
}

void VST2Plugin::queueChannelPanf(sSI _midiChannel, sF32 _pan) {
   // _pan must be in -1..1 range

   sU8 val;

   if(_pan >= 0.0f)
   {
      val = 64 + ((sU8)(_pan * 63));
   }
   else
   {
      val = 64 + ((sU8)(_pan * 64));
   }

   queueEvent((sU8) (0xB0 | (_midiChannel & 15)),
              (sU8) (0x08),
              (sU8) val
              );
}

sUI VST2Plugin::queueHostMIDIEventsByFlt(YAC_Object *_hostMIDIEvents,
                                         sSI _fltDev, sSI _fltCh,
                                         sSI _forceCh,
                                         sSI _globalCh,
                                         YAC_Object *_modInputFilterOrNull,
                                         sUI _minMillisec, sUI _maxMillisec
                                         ) {
   sUI r = 0u;

   // // if(YAC_BCHK(_hostMIDIEvents, clid_HostMIDIEvents))  // (note) moved to tkmidipipe (clid_HostMIDIEvents not available here)
   {
      YAC_CAST_ARG(HostMIDIEvents, host, _hostMIDIEvents);
      YAC_CAST_ARG(ModInputFilter, modInputFilter, _modInputFilterOrNull);

      sBool bBeginEv = YAC_FALSE;

      for(sUI i = 0u; i < host->num_midi_events; i++)
      {
         HostMIDIEvent *hev = &host->midi_events[i];

         if( (hev->millisec >= _minMillisec) && (hev->millisec < _maxMillisec) )
         {
            if( (-1 == _fltDev) || (sUI(_fltDev) == hev->dev_idx) )
            {
               if( (-1 == _fltCh) || (sUI(_fltCh) == (hev->midi_data[0] & 15u)) | (sUI(_globalCh) == (hev->midi_data[0] & 15u)) )
               {
                  if(NULL != effect)
                  {
                     sBool bFilterOk = YAC_TRUE;

                     if(NULL != modInputFilter)
                     {
                        bFilterOk = modInputFilter->checkMessage(hev->midi_data[0],
                                                                 hev->midi_data[1],
                                                                 hev->midi_data[2]
                                                                 );
                     }

                     if(bFilterOk)
                     {
                        if(YAC_FALSE == bBeginEv)
                        {
                           beginEvents();
                           bBeginEv = YAC_TRUE;
                        }

                        // if((_minMillisec > 0) && (_maxMillisec < ~0u))
                        //    printf("xxx VST2Plugin::queueHostMIDIEventsByFlt: min=%u max=%u\n", _minMillisec, _maxMillisec);

                        VstMidiEvent *ev = &midi_events[next_event];

                        if(-1 != _forceCh)
                        {
                           // Used by LPC/MPE mode (single lane VSTi always expects ch0)
                           ev->midiData[0] = (hev->midi_data[0] & 0xF0u) | (sUI(_forceCh) & 15u);
                        }
                        else
                        {
                           ev->midiData[0] = hev->midi_data[0];
                        }

                        ev->midiData[1] = hev->midi_data[1];
                        ev->midiData[2] = hev->midi_data[2];
                        ev->midiData[3] = hev->midi_data[3];

                        // Post range-filter transpose
                        if(NULL != modInputFilter)
                        {
                           if( ((ev->midiData[0] & 0xF0u) == 0x90u) ||
                               ((ev->midiData[0] & 0xF0u) == 0x80u)
                               ) // note on or off ?
                           {
                              sS16 xNote = sS16(ev->midiData[1]) + modInputFilter->note_transpose;
                              ev->midiData[1] = (sU8) sRANGE(xNote, 0, 127);
                           }
                        }

                        next_event = (next_event + 1) & max_events_mask;
                        r++;
                     }
                  }
               }
            }
         }
      }

      if(YAC_TRUE == bBeginEv)
      {
         endEvents();
      }
   }
   
   return r;
}

void VST2Plugin::processEvents(void) {
   if(NULL != effect)
   {
      beginEvents();

      ////yac_host->printf("xxx processEvents: #queued events=%u\n", next_event);
      if(next_event > 0)
      {

         sUI i;
         for(i=0; i<next_event; i++)
         {
            // VstMidiEvent *ev = (VstMidiEvent*) events->events[i];
            // yac_host->printf("xxx VST2Plugin::processEvents: event[%u]: 0x%02x 0x%02x 0x%02x 0x%02x\n", 
            //        (sU8) ev->midiData[0], 
            //        (sU8) ev->midiData[1], 
            //        (sU8) ev->midiData[2], 
            //        (sU8) ev->midiData[3]
            //        );
         }

         events->numEvents = next_event;

         // // beginDispatch();
         effect->dispatcher(effect, effProcessEvents, 0, 0, events, 0.0f);
         // // endDispatch();

         next_event = 0;
      }

      endEvents();
   }
}

sSI VST2Plugin::getNumInputs(void) {
   if(NULL != effect)
   {
      return effect->numInputs;
   }
   return 0;
}

void VST2Plugin::getInputProperties(sUI _idx, YAC_Value *_r) {
   if(NULL != effect)
   {
      if(_idx < ((sUI)effect->numInputs))
      {
         VST2PinProperties *pin = YAC_NEW(VST2PinProperties);

         beginDispatch();
         if(effect->dispatcher(effect, effGetInputProperties, _idx, 0, (void*) &pin->properties, 0.0f))
         {
            _r->initObject(pin, 1);
         }
         else
         {
            yac_host->printf("[~~~] VST2Plugin::getInputProperties: feature not supported by plugin.\n");

            // Generate a pin (old-style plugins)
            if(0 == _idx)
            {
               pin->initFallbackPin((2 == effect->numInputs));
               _r->initObject(pin, 1);
            }
            else
            {
               YAC_DELETE(pin);
            }
         }
         endDispatch();

      }
      else
      {
         yac_host->printf("[---] VST2Plugin::getInputProperties: _idx out of range (%u >= %u)\n", _idx, effect->numInputs);
      }
   }
}

sSI VST2Plugin::getNumOutputs(void) {
   if(NULL != effect)
   {
      return effect->numOutputs;
   }
   return 1;
}

void VST2Plugin::getOutputProperties(sUI _idx, YAC_Value *_r) {
   if(NULL != effect)
   {
      if(_idx < ((sUI)effect->numOutputs))
      {
         VST2PinProperties *pin = YAC_NEW(VST2PinProperties);

         beginDispatch();
         if(effect->dispatcher(effect, effGetOutputProperties, _idx, 0, (void*) &pin->properties, 0.0f))
         {
            _r->initObject(pin, 1);
         }
         else
         {
            yac_host->printf("[~~~] VST2Plugin::getOutputProperties: feature not supported by plugin.\n");

            // Generate a pin (old-style plugins)
            if(0 == _idx)
            {
               pin->initFallbackPin((2 == effect->numOutputs));
               _r->initObject(pin, 1);
            }
            else
            {
               YAC_DELETE(pin);
            }
         }
         endDispatch();
      }
      else
      {
         yac_host->printf("[---] VST2Plugin::getOutputProperties: _idx out of range (%u >= %u)\n", _idx, effect->numOutputs);
      }
   }
}

sSI VST2Plugin::getNumPrograms(void) {
   if(NULL != effect)
   {
      return effect->numPrograms;
   }
   return 0;
}

void VST2Plugin::getProgramName(YAC_Value *_r) {
   dispatchGetString(effGetProgramName, 0, kVstMaxProgNameLen, _r);
}

void VST2Plugin::setProgramName(YAC_String *_s) {
   if(YAC_Is_String(_s))
   {
      if(NULL != _s->chars)
      {
         beginDispatch();
         effect->dispatcher(effect, effSetProgramName, 0, 0, (void*)_s->chars, 0.0f);
         endDispatch();
      }
   }
}

void VST2Plugin::getProgramNameIndexed(sSI _idx, YAC_Value *_r) {
   dispatchGetString(effGetProgramNameIndexed, _idx, kVstMaxProgNameLen, _r);
}

void VST2Plugin::setProgram(sSI _program) {
   if(NULL != effect)
   {
      beginDispatch();
      effect->dispatcher(effect, effSetProgram, 0, _program, NULL, 0.0f);
      endDispatch();
   }
}

sSI VST2Plugin::getProgram(void) {
   sSI r = 0;
   if(NULL != effect)
   {
      beginDispatch();
      VstIntPtr ip;
      ip = effect->dispatcher(effect, effGetProgram, 0, 0, NULL, 0.0f);
      r = sSI(ip);
      endDispatch();
   }
   return r;
}

sSI VST2Plugin::getNumParameters(void) {
   if(NULL != effect)
   {
      return effect->numParams;
   }
   return 0;
}

void VST2Plugin::setParameter(sSI _index, sF32 _value) {
   if(NULL != effect)
   {
      if(NULL != effect->setParameter)
      {
         // yac_host->printf("xxx VST2Plugin::setParameter: idx=%d val=%f\n", _index, _value);
         effect->setParameter(effect, _index, _value);
      }
      else
      {
         yac_host->printf("[~~~] VST2Plugin::setParameter: AEffectSetParameterProc is NULL\n");
      }
   }
}

sF32 VST2Plugin::getParameter(sSI _index) {
   if(NULL != effect)
   {
      if(NULL != effect->getParameter)
      {
         return effect->getParameter(effect, _index);
      }
      else
      {
         yac_host->printf("[~~~] VST2Plugin::getParameter: AEffectGetParameterProc is NULL\n");
      }
   }
   return 0.0f;
}

void VST2Plugin::getParameterName(sSI _index, YAC_Value *_r) {
   dispatchGetString(effGetParamName, _index, 31/*kVstMaxParamStrLen*/, _r); // note: spec says 8 chars max, reality is 31
}

void VST2Plugin::getParameterLabel(sSI _index, YAC_Value *_r) {
   dispatchGetString(effGetParamLabel, _index, kVstMaxParamStrLen, _r);
}

void VST2Plugin::getParameterProperties(sUI _idx, YAC_Value *_r) {
   if(NULL != effect)
   {
      if(_idx < ((sUI)effect->numParams))
      {
         VST2ParameterProperties *param = YAC_NEW(VST2ParameterProperties);

         beginDispatch();
         if(effect->dispatcher(effect, effGetParameterProperties, _idx, 0, (void*) &param->properties, 0.0f))
         {
            _r->initObject(param, 1);
         }
         else
         {
            if(b_debug && b_debug_paramproperties_once)
            {
               b_debug_paramproperties_once = YAC_FALSE;
               yac_host->printf("[~~~] VST2Plugin::getParameterProperties: feature not supported by plugin (uid=0x%08x).\n", effect->uniqueID);
            }
            YAC_DELETE(param);
            _r->initNull();
         }
         endDispatch();
      }
      else
      {
         yac_host->printf("[---] VST2Plugin::getParameterProperties: _idx out of range (%u >= %u)\n", _idx, effect->numParams);
      }
   }
}


static float xxx_a = 0.0f;
static float xxx_w = 440.0f / 44100.0f;

void VST2Plugin::processReplacing(sUI _numFrames, sUI _off) {
   if(NULL != effect)
   {
      // yac_host->printf("VST2Plugin::processReplacing: ENTER, numFrames=%u off=%u\n", _numFrames, _off);
      beginEvents();
      // yac_host->printf("VST2Plugin::processReplacing: 2, numFrames=%u off=%u\n", _numFrames, _off);

#if 0
      if(!b_ext_output_buffers)
         yac_host->printf("\n\n!!!!!!!!! ERRROR processReplacing (b_ext_output_buffers=0) !!!!!!!!!!!!!!!!\n\n");
#endif

      if(_numFrames > block_size)
      {
         yac_host->printf("[~~~] VST2Plugin::processReplacing: _numFrames exceeds block_size (%u > %u). clipping..\n", _numFrames, block_size);
         _numFrames = block_size;
      }
      ////yac_host->printf("xxx process output_buffers[0]=0x%p [1]=0x%p\n", output_buffers[0], output_buffers[1]);
     
      sBool bCanReplacing = (effFlagsCanReplacing == (effect->flags & effFlagsCanReplacing));
      // yac_host->printf("VST2Plugin::processReplacing: 3, bCanReplacing=%d\n", bCanReplacing);

      sF32 *inputBuffers[VST2PLUGIN_MAX_IO_BUFFERS];
      sF32 *outputBuffers[VST2PLUGIN_MAX_IO_BUFFERS];

      sUI i;
      for(i = 0u; i < num_input_buffers; i++)
      {
         inputBuffers[i] = input_buffers[i] + _off;
         // yac_host->printf("xxx VST inputBuffers[%u]=%p\n", i, inputBuffers[i]);
      }
      for(; i < VST2PLUGIN_MAX_IO_BUFFERS; i++)
         inputBuffers[i] = NULL;

      for(i = 0u; i < num_output_buffers; i++)
      {
         outputBuffers[i] = output_buffers[i] + _off;
         // yac_host->printf("xxx VST outputBuffers[%u]=%p\n", i, outputBuffers[i]);
      }
      for(; i < VST2PLUGIN_MAX_IO_BUFFERS; i++)
         outputBuffers[i] = NULL;

      // yac_host->printf("VST2Plugin::processReplacing: 4 (%u inputBuffers + %u outputBuffers set)\n", num_input_buffers, num_output_buffers);

      if(bCanReplacing)
      {
#ifdef PARANOIA_CHECK_REPLACING_OUTPUT
      // Fill output buffers with NaN
         sUI i;
         for(i=0; i<num_output_buffers; i++)
         {
            memset(outputBuffers[i], 0xcd, sizeof(sF32) * _numFrames);  // -431602080.0f
         }
#endif // PARANOIA_CHECK_REPLACING_OUTPUT
      }
      else
      {
         // Fill output buffers with 0
         sUI i;
         for(i=0; i<num_output_buffers; i++)
         {
            memset(outputBuffers[i], 0, sizeof(sF32) * _numFrames);
         }
      }

      if(bCanReplacing)
      {
         // Replace output buffer samples
         if(NULL != effect->processReplacing)
         {
            if(collect_num_chunks > 1u)
            {
               // Buffered mode
               //  (note) workaround for Zynaptic plugins (e.g. PITCHMAP)
               //  (note) both Reason and Eureka process VST plugins in 64 sample frame chunks
               if(block_size == _numFrames)
               {
                  sF32 *inputBuffersCollect[VST2PLUGIN_MAX_IO_BUFFERS];
                  sF32 *outputBuffersCollect[VST2PLUGIN_MAX_IO_BUFFERS];

                  if(0u == (collect_chunk_idx % collect_num_chunks))
                  {
                     for(i = 0u ; i < num_input_buffers; i++)
                     {
                        inputBuffersCollect[i] = collect_input_buffers[i] + (block_size * ((collect_chunk_idx + collect_num_chunks) % (collect_num_chunks*2))) + _off;
                     }

                     for(i = 0u ; i < num_output_buffers; i++)
                     {
                        outputBuffersCollect[i] = collect_output_buffers[i] + (block_size * ((collect_chunk_idx + collect_num_chunks) % (collect_num_chunks*2))) + _off;
                     }

                     // yac_host->printf("xxx VST2Plugin: process %u buffered sample frames. collect_chunk_idx=%u off=%u\n", block_size * collect_num_chunks, collect_chunk_idx, _off);
#if 1
                     effect->processReplacing(effect, inputBuffersCollect, outputBuffersCollect, (VstInt32)block_size * collect_num_chunks);
#else
                     for(i = 0u; i < 2u; i++)
                     {
                        for(sUI k = 0u; k < (block_size * collect_num_chunks); k++)
                        {
                           outputBuffersCollect[i][k] = inputBuffersCollect[i][k];
                        }
                     }
#endif
                  }

                  // Collect current input chunk
                  for(i = 0u ; i < num_input_buffers; i++)
                  {
                     sF32 *d = collect_input_buffers[i] + (block_size * collect_chunk_idx) + _off;

                     for(sUI k = 0u; k < _numFrames; k++)
                     {
                        d[k] = inputBuffers[i][k];
                     }
                  }

                  // Write current output chunk
                  for(i = 0u ; i < num_output_buffers; i++)
                  {
                     sF32 *s = collect_output_buffers[i] + (block_size * collect_chunk_idx) + _off;

                     for(sUI k = 0u; k < _numFrames; k++)
                     {
                        outputBuffers[i][k] = s[k];
                     }
                  }

                  collect_chunk_idx = (collect_chunk_idx + 1u) % (collect_num_chunks * 2);
               }
            }
            else
            {
               // Unbuffered mode
               // yac_host->printf("xxx call effect->processReplacing() @0x%p inBufs=0x%p outBufs=0x%p\n", effect->processReplacing, input_buffers, output_buffers);
               effect->processReplacing(effect, inputBuffers, outputBuffers, (VstInt32)_numFrames);
               // yac_host->printf("xxx OK\n");
            }
         }
         else
         {
            yac_host->printf("[---] VST2Plugin::processReplacing: fxn not implemented by plugin\n");
         }
      }
      else
      {
         // Add to output buffer samples
         if(NULL != effect->process)
         {
            // yac_host->printf("xxx call effect->process() @0x%p inBufs=0x%p outBufs=0x%p\n", effect->process, input_buffers, output_buffers);
            effect->process(effect, inputBuffers, outputBuffers, (VstInt32)_numFrames);
            // yac_host->printf("xxx OK\n");
         }
         else
         {
            yac_host->printf("[---] VST2Plugin::processReplacing: process fallback fxn not implemented by plugin\n");
         }
      }

      if(0)
      {
         if(num_output_buffers >= 2u)
         {
            sUI x=0;
            // yac_host->printf("xxx render sine outputBuffers=(%p; %p)\n", outputBuffers[0], outputBuffers[1]);
            for(; x < _numFrames; x++)
            {
               outputBuffers[0][x] = ::sinf(xxx_a);
               outputBuffers[1][x] = ::sinf(xxx_a);
               xxx_a += xxx_w;
            }
         }
      }

      // // yac_host->printf("xxx output_buffers[0][0]=%f\n", output_buffers[0][0]);
      // // yac_host->printf("xxx output_buffers[1][0]=%f\n", output_buffers[1][0]);
      // yac_host->printf("xxx end call effect->processReplacing(numFrames=%u off=%u)\n", _numFrames, _off);

#ifdef PARANOIA_CHECK_REPLACING_OUTPUT
      if(bCanReplacing && (num_output_buffers > 0))
      {
         if( 0xcdcdcdcdu == *((sUI*)&outputBuffers[0][0]) )
         {
            yac_host->printf("[~~~] VST2Plugin::processReplacing: effect/instrument did not produce any output\n");

            sUI i;
            for(i=0; i<num_output_buffers; i++)
            {
               memset(outputBuffers[i], 0, sizeof(sF32) * _numFrames);
            }
         }
      }
#endif // PARANOIA_CHECK_REPLACING_OUTPUT

      // Debug print output buffer
      if(0)
      {
         if(num_output_buffers >= 2u)
         {
            sUI i = 0;
            yac_host->printf("xxx output_buffers[0]=");
            for(; i < _numFrames; i++)
            {
               yac_host->printf("%f ", outputBuffers[0][i]);
            }
            yac_host->printf("\n");
         }
      }

      endEvents();
   }

}

void VST2Plugin::processReplacingSilence(sUI _numFrames, sUI _off) {
   if(NULL != effect)
   {
      // yac_host->printf("VST2Plugin::processReplacing: ENTER, numFrames=%u off=%u\n", _numFrames, _off);
      // yac_host->printf("VST2Plugin::processReplacing: 2, numFrames=%u off=%u\n", _numFrames, _off);

      if(!b_ext_output_buffers)
         yac_host->printf("\n\n!!!!!!!!! ERRROR processReplacing (b_ext_output_buffers=0) !!!!!!!!!!!!!!!!\n\n");

      if(_numFrames > block_size)
      {
         yac_host->printf("[~~~] VST2Plugin::processReplacing: _numFrames exceeds block_size (%u > %u). clipping..\n", _numFrames, block_size);
         _numFrames = block_size;
      }

      // Fill output buffers with 0
      for(sUI i = 0u; i < num_output_buffers; i++)
      {
         sF32 *outBuf = output_buffers[i] + _off;
         memset(outBuf, 0, sizeof(sF32) * _numFrames);
      }
   }

}

sBool VST2Plugin::getChunk(YAC_Object *_buffer, sBool _bProgram) {
   if(YAC_Is_Buffer(_buffer))
   {
      YAC_Buffer *buffer = (YAC_Buffer*) _buffer;

      if(NULL != effect)
      {
         void *addr = NULL;

         // (note) plugin allocates chunk memory and is responsible for freeing it
         beginDispatch();
         VstIntPtr ip = effect->dispatcher(effect, effGetChunk, _bProgram, 0, (void*)(&addr), 0.0f);
         sUI size = sUI(ip);
         endDispatch();

         if(NULL != addr)
         {
            // Copy to buffer
            if(buffer->yacArrayAlloc(size, 0,0,0))
            {
               ::memcpy(buffer->buffer, addr, size);

               // Succeeded
               return 1;
            }
            else
            {
               yac_host->printf("[---] VST2Plugin::getChunk: failed to allocate buffer (%u bytes).\n", size);
            }
         }
         else
         {
            yac_host->printf("[~~~] VST2Plugin::getChunk: %s\n",
                   (!programsAreChunks()) ? "feature not supported by plugin" : "plugin did not allocate chunk (addr==NULL)");
         }
      }
   }
   else
   {
      yac_host->printf("[---] VST2Plugin::getChunk: _buffer argument is not a Buffer.\n");
   }

   // Failed
   return 0;
}

void VST2Plugin::setChunk(YAC_Object *_buffer, sBool _bProgram) {
   if(YAC_Is_Buffer(_buffer))
   {
      YAC_Buffer *buffer = (YAC_Buffer*) _buffer;

      if(NULL != effect)
      {
         beginDispatch();
         effect->dispatcher(effect, effSetChunk, _bProgram, (VstInt32) buffer->size, (void*)buffer->buffer, 0.0f);
         endDispatch();
      }
   }
   else
   {
      yac_host->printf("[---] VST2Plugin::getChunk: _buffer argument is not a Buffer.\n");
   }
}

void VST2Plugin::setParentNativeWindowHandle(sUI _hWnd) {
   (void)_hWnd;
}

void VST2Plugin::showEditor(void) {
   if(NULL != effect)
   {
      yac_host->printf("[dbg] VST2Plugin::showEditor: uid=0x%08x\n", getUniqueID());

   } // if effect
}

void VST2Plugin::hideEditor(void) {
   yac_host->printf("[dbg] VST2Plugin::hideEditor: uid=0x%08x\n", getUniqueID());
}

void VST2Plugin::closeEditor(void) {
   sUI uid = getUniqueID();
   if(0u != uid)  // not the template instance ?
   {
      yac_host->printf("[dbg] VST2Plugin::closeEditor: uid=0x%08x\n", uid);
   }
}

sBool VST2Plugin::isEditorVisible(void) {
   if(NULL != effect)
   {
   }
   return YAC_FALSE;
}

sBool VST2Plugin::isEditorOpen(void) {
   if(NULL != effect)
   {
   }
   return YAC_FALSE;
}

sUI VST2Plugin::GetNumOpenWindows(void) {
   sUI r = 0;

   return r;
}

void VST2Plugin::PumpEvents(void) {
}

void VST2Plugin::HandleQueuedWindowResize(void) {
}

void VST2Plugin::setEnableDebug(sBool _bEnable) {
   b_debug = _bEnable;
}

sUI VST2Plugin::getNumShellPlugins(void) {
   return num_shell_plugins;
}

void VST2Plugin::getShellPluginNameByIndex(sUI _index, YAC_Value *_r) {
   if(_index < num_shell_plugins)
   {
      const vst2_shell_plugin_t *shellPlugin = &shell_plugins[_index];
      YAC_String *s = YAC_New_String();
      // // s->alloc(VST2PLUGIN_MAX_STRING_LEN);
      s->copy(shellPlugin->effect_name);
      _r->initString(s, YAC_TRUE/*bDelete*/);
   }
   else
   {
      _r->initNull();
   }
}

sUI VST2Plugin::getShellPluginUIDByIndex(sUI _index) {
   sUI r = 0u;

   if(_index < num_shell_plugins)
   {
      const vst2_shell_plugin_t *shellPlugin = &shell_plugins[_index];
      r = shellPlugin->uid;
   }

   return r;
}

sBool VST2Plugin::selectShellPluginByUID(YAC_String *_name, sUI _uid) {
   // (note) not used, selection is done in loadPlugin()
   sBool r = YAC_FALSE;
   current_shell_plugin_idx = -1;

   for(sUI shellIdx = 0u; shellIdx < num_shell_plugins; shellIdx++)
   {
      const vst2_shell_plugin_t *shellPlugin = &shell_plugins[shellIdx];

      if(shellPlugin->uid == _uid)
      {
         current_shell_plugin_idx = shellIdx;
         r = YAC_TRUE;
         break;
      }
   }

   return r;
}

sUI VST2Plugin::getCurrentShellPluginUID(void) {
   sUI r = 0u;

   if(-1 != current_shell_plugin_idx)
   {
      if(sUI(current_shell_plugin_idx) < num_shell_plugins)
      {
         const vst2_shell_plugin_t *shellPlugin = &shell_plugins[sUI(current_shell_plugin_idx)];
         r = shellPlugin->uid;
      }
   }

   return r;
}

void VST2Plugin::getCurrentShellPluginEffectName(YAC_Value *_r) {
  _r->initNull();

   if(-1 != current_shell_plugin_idx)
   {
      if(sUI(current_shell_plugin_idx) < num_shell_plugins)
      {
         const vst2_shell_plugin_t *shellPlugin = &shell_plugins[sUI(current_shell_plugin_idx)];
         _r->initEmptyString();
         _r->value.string_val->copy(shellPlugin->effect_name);
      }
   }
}

sUI VST2Plugin::dataBridgeGetNumChunksAvail(void) {
#ifdef YAC_WIN32
   beginDispatch();
   sUI r = effect->dispatcher(effect, effDataBridgeGetNumChunksAvail, 0, 0, (void*)NULL, 0.0f);
   endDispatch();
   return r;
#else
   return 0u;
#endif // YAC_WIN32
}
  
void VST2Plugin::SetEnableHideVSTEditorWhenClosed(sBool _bEnable) {
   b_hide_vst_window_when_closed = (YAC_FALSE != _bEnable);
}

sUI VST2Plugin::getNativeWindowHandle(void) {
   return 0u;
}

void VST2Plugin::setEnableReportTransportPlaying(sBool _bEnable) {
   b_report_transport_playing = _bEnable;
}

sBool VST2Plugin::getEnableReportTransportPlaying(void) {
   return b_report_transport_playing;
}

void VST2Plugin::windowToFront(void) {
   // not needed on Windows (macOS-only workaround)
}

void VST2Plugin::callEffEditIdle(void) {
   if(isEditorOpen())
   {
      effect->dispatcher(effect, effEditIdle, 0, 0, 0, 0);
   }
}
