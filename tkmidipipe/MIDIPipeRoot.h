// ----
// ---- file   : MIDIPipeRoot.h
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2014-2023 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : This is part of the "syntracker" sequencer.
// ----
// ---- created: 01Sep2014
// ---- changed: 03Sep2014, 04Sep2014, 05Sep2014, 08Sep2014, 09Sep2014, 11Sep2014, 14Sep2014
// ----          19Sep2014, 26Sep2014, 27Sep2014, 08Feb2015, 16Feb2015, 23Feb2015, 08Mar2015
// ----          28Apr2015, 11Sep2015, 14Sep2015, 09Sep2016, 03Dec2016, 17Feb2017, 25Jun2019
// ----          26Aug2020, 20Jun2021, 18Nov2022, 26Jun2023, 16Nov2023, 17Nov2023, 03Dec2023
// ----
// ----
// ----

#ifndef TK_MIDIPIPEROOT_H__
#define TK_MIDIPIPEROOT_H__

YG("midipipe")


/* @class MIDIPipeRoot,Object
 */
YC class MIDIPipeRoot : public YAC_Object {
  public:
   MIDIPipeDevice**devices;
   sUI max_devices;

   MIDIPipe**pipes;
   sUI max_pipes;

   MIDIPipe *solo_pipe;

   sUI default_pipe_delay;  // set by writing CTL_GROOVE in master pipe
   sUI current_pipe_delay;  // current pipe delay
   sUI process_pipe_idx;    // temporary during emit(). 0 = master pipe

   sSI voice_modulo_index;  // -1=off, or 0..voice_modulo_num-1
   sUI voice_modulo_num;

   sBool b_delete_note_ons_and_offs;  // used by e.g. NodeArranger to skip note ons/offs when edit offset has changed

  public:
   MIDIPipeRoot(void);
   ~MIDIPipeRoot();

   YAC(MIDIPipeRoot);

  public:

   sBool getDeviceBuffersByEvent(const midipipe_frame_base_t *_ev,
                                 const sBool                  _bOverflow,
                                 YAC_Buffer**                 _retBufCtl,
                                 YAC_Buffer**                 _retBufNote,
                                 midipipe_device_ch_t**       _retDevCh,
                                 midipipe_device_ch_t**       _retDevChShadow,
                                 MIDIPipeDevice **            _retPipeDev
                                 ) const;

   void swapPipeSlots(sUI _a, sUI _b);

  protected:
   void applyVoiceModulo (MIDIPipeFrame *_frameEmit);

  public:
   YM void allocDeviceSlots (sUI _maxDevices);
   YM sUI getMaxDevices (void) const;
   YM MIDIPipeDevice *allocDevice (sUI _devIdx);
   YM MIDIPipeDevice *getDeviceByIdx (sUI _devIdx) const;
   YM void freeDevices (void);

   YM void allocPipeSlots (sUI _maxPipes);
   YM sUI getMaxPipes (void) const;
   YM sUI getNumPipes (void) const;
   YM sBool canAllocPipe (sUI _pipeIdx);
   YM sBool shiftPipesDownAt (sUI _pipeIdx);
   YM sBool shiftPipesUpAt (sUI _pipeIdx);
   YM MIDIPipe *allocPipe (sUI _pipeIdx);
   YM MIDIPipe *getPipeByIdx (sUI _pipeIdx) const;
   YM MIDIPipe *getPipeByUsedIdx (sUI _pipeIdx) const;
   YM void freePipes (void);
   YM void freePipe (MIDIPipe *_pipe);
   YM sSI findFirstUnusedIdx (void) const;
   YM sSI findFirstUsedIdx (void) const;
   YM sSI findLastUsedIdx (void) const;

   YM void setSoloPipe (YAC_Object *_pipe);
   YM MIDIPipe *getSoloPipe (void) const;

   YM void emitAutoNoteOffs (void);

   YM void processAllPipesAndEmit (MIDIPipeFrame *_framePlayMaster, MIDIPipeFrame *_framePlay, MIDIPipeFrame *_frameRec, sBool _bPlaySeq, YAC_Object *_frameExportOrNull);

   YM void seek (sUI _ticks, sBool _bSeekAll);

   YM void reset (sBool _bSoft);

   YM void songSelect (void);

   YM void clearMIDICache (void);
   YM void clearMIDICacheForDev (sSI _devIdx);

   YM MIDIPipe *uiGetPipeBeforeIdx (sUI _pipeIdx);
   YM MIDIPipe *uiGetPipeAfterIdx (sUI _pipeIdx);

   YM void handleSongSpeedChanged (sBool _bPPQ, sBool _bBPM);

   // temporary during emit()
   //  - set via NodeTracker CTL_GROOVE
   YM void setPipeDelay (sUI _delay);
   YM sUI getPipeDelay (void);

   YM void configureVoiceModulo (sSI _startVoiceIdx, sUI _numVoices);

   YM void setEnableDeleteNoteOnsAndOffs (sBool _bEnable);

};


#endif // TK_MIDIPIPEROOT_H__
