/// VSTPlugin.cpp
///
/// (c) 2010-2023 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///
/// created: 02Oct2010
/// changed: 03Oct2010, 04Oct2010, 05Oct2010, 09Oct2010, 10Oct2010, 13Oct2010, 15Oct2010
///          16Oct2010, 20Oct2010, 09Feb2014, 31Dec2021, 06Feb2023, 18Jul2023, 24Oct2023
///
///
///

#include "tkvst2.h"

#include "VST2PinProperties.h"
#include "VST2ParameterProperties.h"

#include "VST2Plugin.h"

#include <math.h>


// (note) resume() >> startProcess() >> processReplacing() 
// (note) processReplacing() >> stopProcess() >> suspend() 


void *tkvst2_audio_master_signal_funs[VST2PLUGIN_NUM_SIGNALS];

YAC_ContextHandle tkvst2_audio_master_script_context;



VST2Plugin::VST2Plugin(void) {
}

VST2Plugin::~VST2Plugin() {
}

void YAC_VCALL VST2Plugin::yacGetSignalStringList(YAC_String *_sig) {
   // ---- the number represents the encoded argument type identifiers used by this function
   // ---- 1=int, 2=float, 3=object. bits 0+1 represent argument #0, bits 2+3 represent argument #1 and so on
   _sig->visit("onAudioMasterUpdateDisplay:3 onAudioMasterAutomate:39 onAudioMasterIOChanged:3 onWin32KeyEvent:7 onMacOSKeyDown:7 onAudioMasterParamEdit:23");
}

void YAC_VCALL VST2Plugin::yacRegisterSignal(sUI _id, void *_fun) {
   if(_id < VST2PLUGIN_NUM_SIGNALS)
   {
      tkvst2_audio_master_signal_funs[_id] = _fun;
   }
}

void VST2Plugin::StaticInit(void) {
}

sBool VST2Plugin::WaitForDatabridgeHostEvent(sUI _timeout) {
   (void)_timeout;
   return YAC_FALSE;
}

void VST2Plugin::StaticExit(void) {
}

void VST2Plugin::setEnableDebug(sBool _bEnable) {
   (void)_bEnable;
}

sBool VST2Plugin::loadPlugin(YAC_String *_pathName, sUI _selectShellPluginUID) {
   (void)_pathName;
   (void)_selectShellPluginUID;

   // Failed
   return 0;
}

void VST2Plugin::open(void) {
}

void VST2Plugin::close(void) {
}

sSI VST2Plugin::getCategory(void) {
   return 0;//kPlugCategUnknown;
}

sBool VST2Plugin::isEffect(void) {
   return YAC_FALSE;
}

sBool VST2Plugin::isInstrument(void) {
   return YAC_FALSE;
}

void VST2Plugin::suspend(void) {
}

void VST2Plugin::resume(void) {
}

void VST2Plugin::startProcess(void) {
}

void VST2Plugin::stopProcess(void) {
}

void VST2Plugin::setSampleRate(sF32 _rate) {
}

void VST2Plugin::setBlockSize(sUI _blkSize) {
   block_size = _blkSize;
   
   reallocateIOBuffers();
}

void VST2Plugin::setBlockSizeCollect(sUI _blockSize, sUI _num) {
   // if(NULL != effect)
   {
      block_size = _blockSize;
      // collect_num_chunks = _num;
      // collect_chunk_idx = 0u;

      reallocateIOBuffers();
   }
}

void VST2Plugin::freeIOBuffers(void) {

   // Inputs
   printf("[dbg] VST2Plugin::freeIOBuffers: freeing %d input buffers\n", num_input_buffers);
   if(NULL != input_buffers)
   {
      sUI i = 0;
      for(i=0; i<num_input_buffers; i++)
      {
         delete [] input_buffers[i];
         input_buffers[i] = NULL;
      }
      delete [] input_buffers;
      input_buffers = NULL;
      num_input_buffers = 0;
   }

   // Outputs
   printf("[dbg] VST2Plugin::freeIOBuffers: freeing %d output buffers\n", num_output_buffers);
   if(NULL != output_buffers)
   {
      sUI i = 0;
      for(i=0; i<num_output_buffers; i++)
      {
         delete [] output_buffers[i];
         output_buffers[i] = NULL;
      }
      delete [] output_buffers;
      output_buffers = NULL;
      num_output_buffers = 0;
   }
}

void VST2Plugin::reallocateIOBuffers(void) {
   // called by setBlockSize and audioMasterIOChanged host opcode dispatcher

   // Free old I/O buffers 
   freeIOBuffers();

   // (Re-) allocate sample buffers
   if(block_size > 0)
   {
      // Allocate input buffers
      num_input_buffers = 2;//effect->numInputs;
      printf("[dbg] VST2Plugin::reallocateIOBuffers: allocating %d input buffers (%d frames)\n", num_input_buffers, block_size);
      if(num_input_buffers > 0)
      {
         input_buffers = new sF32*[num_input_buffers];
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
               printf("[---] VST2Plugin::reallocateIOBuffers: failed to allocate input buffer #%u (%u samples)\n", i, block_size);
            }
         }
      }
      
      // Allocate output buffers
      num_output_buffers = 2;//effect->numOutputs;
      printf("[dbg] VST2Plugin::reallocateIOBuffers: allocating %d output buffers (%d frames)\n", num_output_buffers, block_size);
      if(num_output_buffers > 0)
      {
         output_buffers = new sF32*[num_output_buffers];
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
               printf("[---] VST2Plugin::reallocateIOBuffers: failed to allocate output buffer #%u (%u samples)\n", i, block_size);
            }
         }
      }
   }
   else
   {
      printf("[~~~] VST2Plugin::reallocateIOBuffers: _blkSize is 0 ?!!\n");
   }
}

void VST2Plugin::getEffectName(YAC_Value *_r) {
}

void VST2Plugin::getVendorString(YAC_Value *_r) {
}

void VST2Plugin::getProductString(YAC_Value *_r) {
}

sSI VST2Plugin::getVendorVersion(void) {
   return 0;
}

sSI VST2Plugin::getVersion(void) {
   return 0;
}

sSI VST2Plugin::getUniqueID(void) {
   return 0;
}

sSI VST2Plugin::canDo(YAC_String *_text) {
   return 0;
}

sUI VST2Plugin::getFlags(void) {
   return 0;
}

sBool VST2Plugin::isSynth(void) {
   return 0;
}

sBool VST2Plugin::hasEditor(void) {
   return 0;
}

sBool VST2Plugin::canReplacing(void) {
   return 0;
}

sBool VST2Plugin::programsAreChunks(void) {
   return 0;
}

sBool VST2Plugin::noSoundInStop(void) {
   return 0;
}

sSI VST2Plugin::getNumMidiInputChannels(void) {
   return 0;
}

sSI VST2Plugin::getNumMidiOutputChannels(void) {
   return 0;
}

sSI VST2Plugin::getInitialDelay(void) {
   return 0;
}

void VST2Plugin::idle(void) {
}

void VST2Plugin::queueEvent(sU8 _0, sU8 _1, sU8 _2, sU8 _3) {
   (void)_0;
   (void)_1;
   (void)_2;
   (void)_3;
}

void VST2Plugin::queueNoteOn(sSI _midiChannel, sSI _note, sSI _velocity) {
   ////printf("xxx queueNoteOn note=%d vel=%d\n", _note, _velocity);

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

void VST2Plugin::processEvents(void) {
}

sUI VST2Plugin::queueHostMIDIEventsByFlt(YAC_Object *_hostMIDIEvents, sSI _fltDev, sSI _fltCh, sSI _forceCh, sSI _globalCh, YAC_Object *_modInputFilterOrNull, sUI _minMillisec, sUI _maxMillisec) {
   return 0u;
}

sSI VST2Plugin::getNumInputs(void) {
   return 0;
}

void VST2Plugin::getInputProperties(sUI _idx, YAC_Value *_r) {
}

sSI VST2Plugin::getNumOutputs(void) {
   return 1;
}

void VST2Plugin::getOutputProperties(sUI _idx, YAC_Value *_r) {
}

sSI VST2Plugin::getNumPrograms(void) {
   return 0;
}

void VST2Plugin::getProgramName(YAC_Value *_r) {
}

void VST2Plugin::setProgramName(YAC_String *_s) {
}

void VST2Plugin::getProgramNameIndexed(sSI _idx, YAC_Value *_r) {
}

void VST2Plugin::setProgram(sSI _program) {
}

sSI VST2Plugin::getProgram(void) {
   return 0;
}

sSI VST2Plugin::getNumParameters(void) {
   return 0;
}

void VST2Plugin::setParameter(sSI _index, sF32 _value) {
}

sF32 VST2Plugin::getParameter(sSI _index) {
   return 0.0f;
}

void VST2Plugin::getParameterName(sSI _index, YAC_Value *_r) {
}

void VST2Plugin::getParameterLabel(sSI _index, YAC_Value *_r) {
}

void VST2Plugin::getParameterProperties(sUI _idx, YAC_Value *_r) {
}

void VST2Plugin::processReplacing(sUI _numFrames, sUI _off) {
   // Fill output buffers with 0
   sUI i;
   for(i=0; i<num_output_buffers; i++)
   {
      memset(output_buffers[i] + _off, 0, sizeof(sF32) * _numFrames);
   }
}

void VST2Plugin::processReplacingSilence(sUI _numFrames, sUI _off) {
   // Fill output buffers with 0
   sUI i;
   for(i=0; i<num_output_buffers; i++)
   {
      memset(output_buffers[i] + _off, 0, sizeof(sF32) * _numFrames);
   }
}

sBool VST2Plugin::getChunk(YAC_Object *_buffer, sBool _bProgram) {
   // Failed
   return 0;
}

void VST2Plugin::setChunk(YAC_Object *_buffer, sBool _bProgram) {
}

void VST2Plugin::setParentNativeWindowHandle(sUI _hWnd) {
   (void)_hWnd;
}

void VST2Plugin::showEditor(void) {
}

void VST2Plugin::hideEditor(void) {
}

sBool VST2Plugin::isEditorVisible(void) {
   return YAC_FALSE;
}

sBool VST2Plugin::isEditorOpen(void) {
   return YAC_FALSE;
}

void VST2Plugin::closeEditor(void) {
}

sUI VST2Plugin::GetNumOpenWindows(void) {
   return 0;
}

void VST2Plugin::PumpEvents(void) {
   // for test environment
}

void VST2Plugin::HandleQueuedWindowResize(void) {
}

sBool VST2Plugin::setExtInputBuffer(sUI _idx, YAC_Object *_fa) {
   (void)_idx;
   (void)_fa;
   return YAC_FALSE;
}

sBool VST2Plugin::setExtOutputBuffer(sUI _idx, YAC_Object *_fa) {
   (void)_idx;
   (void)_fa;
   return YAC_FALSE;
}

sUI VST2Plugin::getNumShellPlugins(void) {
   return 0u;
}

void VST2Plugin::getShellPluginNameByIndex(sUI _index, YAC_Value *_r) {
   (void)_index;
   _r->initNull();
}

sUI VST2Plugin::getShellPluginUIDByIndex(sUI _index) {
   (void)_index;
   return 0u;
}

sBool VST2Plugin::selectShellPluginByUID(YAC_String *_name, sUI _uid) {
   (void)_name;
   (void)_uid;
   return YAC_FALSE;
}

sUI VST2Plugin::getCurrentShellPluginUID(void) {
   return 0u;
}

void VST2Plugin::getCurrentShellPluginEffectName(YAC_Value *_r) {
   _r->initNull();
}

sUI VST2Plugin::dataBridgeGetNumChunksAvail(void) {
   return 0u;
}

void VST2Plugin::SetEnableHideVSTEditorWhenClosed(sBool _bEnable) {
   (void)_bEnable;
}

sUI VST2Plugin::getNativeWindowHandle(void) {
   return 0u;
}

void VST2Plugin::setEnableReportTransportPlaying(sBool _bEnable) {
   (void)_bEnable;
}

sBool VST2Plugin::getEnableReportTransportPlaying(void) {
   return YAC_FALSE;
}

void VST2Plugin::windowToFront(void) {
}

void VST2Plugin::callEffEditIdle(void) {
}
