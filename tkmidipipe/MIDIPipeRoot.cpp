// ----
// ---- file   : MIDIPipeRoot.cpp
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2014-2024 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : This is part of the "syntracker" sequencer.
// ----
// ---- created: 01Sep2014
// ---- changed: 03Sep2014, 04Sep2014, 05Sep2014, 08Sep2014, 09Sep2014, 11Sep2014, 26Sep2014
// ----          27Sep2014, 08Feb2015, 23Feb2015, 08Mar2015, 28Apr2015, 14Sep2015, 09Sep2016
// ----          17Feb2017, 03Mar2017, 29Mar2019, 25Jun2019, 26Aug2020, 20Jun2021, 24Apr2022
// ----          18Nov2022, 26Jun2023, 16Nov2023, 17Nov2023, 03Dec2023, 02Feb2024, 06Oct2024
// ----
// ----
// ----

#include "tkmidipipe.h"


MIDIPipeRoot::MIDIPipeRoot(void) {
   devices     = NULL;
   max_devices = 0u;
   pipes       = NULL;
   max_pipes   = 0u;
   solo_pipe   = NULL;

   default_pipe_delay = 0u;
   current_pipe_delay = 0u;

   voice_modulo_index = -1;
   voice_modulo_num   = 1u;

   b_delete_note_ons_and_offs = YAC_FALSE;
}

MIDIPipeRoot::~MIDIPipeRoot() {
   // printf("xxx ~MIDIPipeRoot this=%p\n", this);

   freePipes();
   freeDevices();
}

void MIDIPipeRoot::allocDeviceSlots(sUI _maxDevices) {
   devices = new MIDIPipeDevice*[_maxDevices];
   max_devices = _maxDevices;
   memset(devices, 0, sizeof(MIDIPipeDevice*) * max_devices);
}

sUI MIDIPipeRoot::getMaxDevices(void) const {
   return max_devices;
}

MIDIPipeDevice *MIDIPipeRoot::allocDevice(sUI _devIdx) {
   MIDIPipeDevice *r = NULL;

   if(_devIdx < max_devices)
   {
      if(NULL == devices[_devIdx])
      {
         r = YAC_NEW(MIDIPipeDevice);

         if(NULL != r)
         {
            r->setDevIdx(_devIdx);

            devices[_devIdx] = r;
         }
      }
   }

   return r;
}

void MIDIPipeRoot::freeDevices(void) {
   if(NULL != devices)
   {
      for(sUI i = 0u; i < max_devices; i++)
      {
         MIDIPipeDevice *dev = devices[i];

         if(NULL != dev)
         {
            YAC_DELETE(dev);
         }
      }

      memset(devices, 0, sizeof(MIDIPipeDevice*) * max_devices);

      delete [] devices;
      devices = NULL;
      max_devices = 0u;
   }
}

MIDIPipeDevice *MIDIPipeRoot::getDeviceByIdx(sUI _devIdx) const {
   if(_devIdx < max_devices)
   {
      return devices[_devIdx];
   }
   return NULL;
}

void MIDIPipeRoot::allocPipeSlots(sUI _maxPipes) {
   pipes = new MIDIPipe*[_maxPipes];
   max_pipes = _maxPipes;
   memset(pipes, 0, sizeof(MIDIPipe*) * max_pipes);
}

sUI MIDIPipeRoot::getMaxPipes(void) const {
   return max_pipes;
}

sUI MIDIPipeRoot::getNumPipes(void) const {
   sUI num = 0u;
   for(sUI i = 0u; i < max_pipes; i++)
   {
      if(NULL != pipes[i])
      {
         num++;
      }
   }
   return num;
}

sBool MIDIPipeRoot::canAllocPipe(sUI _pipeIdx) {
   if(_pipeIdx < max_pipes)
   {
      return (NULL == pipes[_pipeIdx]);
   }
   return YAC_FALSE;
}

sBool MIDIPipeRoot::shiftPipesDownAt(sUI _pipeIdx) {
   if( _pipeIdx < (max_pipes - 1u) )
   {
      sUI cur = max_pipes - 1u;
      if(NULL == pipes[cur])
      {
         while(cur > _pipeIdx)
         {
            pipes[cur] = pipes[cur - 1u];
            if(NULL != pipes[cur])
            {
               pipes[cur]->pipe_idx = cur;
            }
            cur--;
         }
         pipes[_pipeIdx] = NULL;
         return YAC_TRUE;
      }
   }
   return YAC_FALSE;
}

sBool MIDIPipeRoot::shiftPipesUpAt(sUI _pipeIdx) {
   if(_pipeIdx >= 0u)
   {
      sUI cur = _pipeIdx;
      if(NULL == pipes[cur])
      {
         while(cur < (max_pipes - 1u))
         {
            pipes[cur] = pipes[cur + 1u];
            if(NULL != pipes[cur])
            {
               pipes[cur]->pipe_idx = cur;
            }
            cur++;
         }
         pipes[max_pipes - 1u] = NULL;
         return YAC_TRUE;
      }
   }
   return YAC_FALSE;
}

MIDIPipe *MIDIPipeRoot::allocPipe(sUI _pipeIdx) {
   MIDIPipe *r = NULL;

   if(_pipeIdx < max_pipes)
   {
      if(NULL == pipes[_pipeIdx])
      {
         r = YAC_NEW(MIDIPipe);

         if(NULL != r)
         {
            r->pipe_idx = _pipeIdx;

            pipes[_pipeIdx] = r;

            r->init(this);
         }
      }
   }

   return r;
}

void MIDIPipeRoot::freePipe(MIDIPipe *_pipe) {
   for(sUI i = 0u; i < max_pipes; i++)
   {
      if(pipes[i] == _pipe)
      {
         YAC_DELETE_SAFE(pipes[i]);
         break;
      }
   }
}


void MIDIPipeRoot::freePipes(void) {
   if(NULL != pipes)
   {
      for(sUI i = 0u; i < max_pipes; i++)
      {
         MIDIPipe *pipe = pipes[i];

         if(NULL != pipe)
         {
            YAC_DELETE(pipe);
         }
      }

      memset(pipes, 0, sizeof(MIDIPipe*) * max_pipes);

      delete [] pipes;
      pipes = NULL;
      max_pipes = 0u;
   }
}

sSI MIDIPipeRoot::findFirstUsedIdx(void) const {
   for(sUI i = 0; i < max_pipes; i++)
   {
      if(NULL != pipes[i])
      {
         return (sSI)i;
      }
   }
   return -1;
}

sSI MIDIPipeRoot::findFirstUnusedIdx(void) const {
   for(sUI i = 0u; i < max_pipes; i++)
   {
      if(NULL == pipes[i])
      {
         return (sSI)i;
      }
   }
   return -1;
}

sSI MIDIPipeRoot::findLastUsedIdx(void) const {
   sSI r = -1;
   for(sUI i = 0u; i < max_pipes; i++)
   {
      if(NULL != pipes[i])
      {
         r = (sSI)i;
      }
   }
   return r;
}

MIDIPipe *MIDIPipeRoot::getPipeByIdx(sUI _pipeIdx) const {
   if(_pipeIdx < max_pipes)
   {
      return pipes[_pipeIdx];
   }
   return NULL;
}

MIDIPipe *MIDIPipeRoot::getPipeByUsedIdx(sUI _pipeUsedIdx) const {
   if(_pipeUsedIdx < max_pipes)
   {
      sUI ui = 0u;
      for(sUI i = 0u; i < max_pipes; i++)
      {
         if(NULL != pipes[i])
         {
            if(ui == _pipeUsedIdx)
            {
               return pipes[i];
            }
            ui++;
         }
      }
   }
   return NULL;
}

void MIDIPipeRoot::swapPipeSlots(sUI _a, sUI _b) {
   MIDIPipe *t = pipes[_a];
   pipes[_a] = pipes[_b];
   pipes[_b] = t;

   if(NULL != pipes[_a])
   {
      pipes[_a]->pipe_idx = _a;
   }

   if(NULL != pipes[_b])
   {
      pipes[_b]->pipe_idx = _b;
   }
}

sBool MIDIPipeRoot::getDeviceBuffersByEvent(const midipipe_frame_base_t *_ev,
                                            const sBool                  _bOverflow,
                                            YAC_Buffer**                 _retBufCtl,
                                            YAC_Buffer**                 _retBufNote,
                                            midipipe_device_ch_t**       _retDevCh,
                                            midipipe_device_ch_t**       _retDevChShadow,
                                            MIDIPipeDevice**             _retPipeDev
                                            ) const {
   if((sUI)_ev->dev_idx < max_devices)
   {
      MIDIPipeDevice *dev = devices[_ev->dev_idx];

      if(NULL != dev)
      {
         if(!dev->b_virtual)
         {
            if( (YAC_TRUE == _bOverflow) || ((YAC_MAYBE == _bOverflow) && dev->b_overflow) )
            {
               *_retBufCtl       = dev->overflow_cmd_buffers [dev->write_overflow_buffer_idx];
               *_retBufNote      = dev->overflow_note_buffers[dev->write_overflow_buffer_idx];
            }
            else
            {
               *_retBufCtl       = dev->cmd_buffer;
               *_retBufNote      = dev->note_buffer;
            }
            *_retDevCh        = &dev->channels[_ev->midi_ch & 15u];
            *_retDevChShadow  = &dev->shadow_channels[_ev->midi_ch & 15u];
            *_retPipeDev      = dev;

            return (NULL != dev->cmd_buffer) && (NULL != dev->note_buffer);
         }
      }
   }
   return YAC_FALSE;
}

void MIDIPipeRoot::emitAutoNoteOffs(void) {
   for(sUI devIdx = 0u; devIdx < max_devices; devIdx++)
   {
      MIDIPipeDevice *dev = devices[devIdx];

      if(NULL != dev)
      {
         sUI midiCh;
         midipipe_device_ch_t *devCh = dev->channels;

         for(midiCh = 0u; midiCh < 16u; midiCh++, devCh++)
         {
            sBool bFirst = YAC_TRUE;

            if(devCh->num_notes > 0)
            {
               sUI nnLo = (sUI)devCh->note_low;
               if(nnLo > 127u)
               {
                  nnLo = 0u;
               }

               sUI nnHi = (sUI)devCh->note_hi;
               if(nnHi > 127u)
               {
                  nnHi = 127u;
               }

               // printf("xxx midiCh=%u nnLo=%u nnHi=%u\n", midiCh, nnLo, nnHi);

               for(sUI nn = nnLo; nn <= nnHi; nn++)
               {
                  if(-1 != devCh->note_states[nn].ticks_left)
                  {
                     // Active note
                     sBool bContNote = YAC_TRUE;

                     if(devCh->note_states[nn].ticks_left > 0)
                     {
                        if(--devCh->note_states[nn].ticks_left <= 0)
                        {
                           // Auto Note-Off
                           devCh->note_states[nn].ticks_left = -1;

                           if(NULL != dev->note_buffer)
                           {
                              if(bFirst || !dev->running_status[midiCh])
                              {
                                 dev->note_buffer->yacStreamWriteI8(0x80 | midiCh);
                                 bFirst = YAC_FALSE;
                              }
                              dev->note_buffer->yacStreamWriteI8(nn);
                              dev->note_buffer->yacStreamWriteI8(0/*vel*/);
                              // yac_host->printf("xxx send auto note off devIdx=%u ch=%u nn=%u\n", devIdx, midiCh, nn);
                           }

                           if(nn == (sUI)devCh->note_low)
                           {
                              devCh->note_low = -1;
                           }

                           if(nn == (sUI)devCh->note_hi)
                           {
                              devCh->note_hi = 0xFFFFu;
                           }

                           if(devCh->num_notes > 0)
                           {
                              devCh->num_notes--;
                           }

                           bContNote = YAC_FALSE;
                        }
                     }

                     if(bContNote)
                     {
                        // Continue playing note
                        if(-1 == devCh->note_low)
                        {
                           devCh->note_low = (sSI)nn;
                        }

                        if((0xFFFFu == devCh->note_hi) || (nn > devCh->note_hi))
                        {
                           devCh->note_hi = (sSI)nn;
                        }
                     }
                  }
               }
            } // if num_notes > 0
         } // for midiCh
      } // if dev
   }

}

void MIDIPipeRoot::setSoloPipe(YAC_Object *_pipe) {
   solo_pipe = (MIDIPipe*)_pipe;
}

MIDIPipe *MIDIPipeRoot::getSoloPipe(void) const {
   return solo_pipe;
}

void MIDIPipeRoot::processAllPipesAndEmit(MIDIPipeFrame *_framePlayMaster,
                                          MIDIPipeFrame *_framePlay,
                                          MIDIPipeFrame *_frameRec,
                                          sBool          _bPlaySeq,
                                          YAC_Object     *_frameExportOrNull
                                          ) {

   MIDIPipeFrame *frameExportOrNull;
   if(YAC_VALID(_frameExportOrNull))
      frameExportOrNull = (MIDIPipeFrame *)_frameExportOrNull;
   else
      frameExportOrNull = NULL;

   MIDIPipeFrame frameGlobal;
   frameGlobal.root = this;

   _framePlay->empty();

   // Combined output of all pipes (to avoid duplicate notes, e.g. when using "global" devs)
   //  (note) caveat: limited control over order of MIDI bytes
   //  (note) used when there's a "final output" pipe (pipeOut)
   MIDIPipeFrame frameOut;
   MIDIPipe *pipeOut = NULL;

   MIDIPipeFrame frameEmit;
   frameEmit.root = this;

   sUI pipeIdx;

   // (todo) cache ? (just 16 pipes max, though)
   for(pipeIdx = 0u; pipeIdx < max_pipes; pipeIdx++)
   {
      if(NULL != pipes[pipeIdx])
      {
         MIDIPipe *pipe = pipes[pipeIdx];
         if(pipe->b_final_output)
            pipeOut = pipe;
      }
   }

   // if(_framePlayMaster->getNumEvents() > 0)
   //    yac_host->printf("xxx MIDIPipeRoot: 1 framePlayMaster #noteOn=%u #noteOff=%u\n", _framePlayMaster->getNumEventsNoteOn(), _framePlayMaster->getNumEventsNoteOff());

   for(pipeIdx = 0u; pipeIdx < max_pipes; pipeIdx++)
   {
      process_pipe_idx = pipeIdx;

      if(NULL != pipes[pipeIdx])
      {
         MIDIPipe *pipe = pipes[pipeIdx];
         current_pipe_delay = 0u;

         if(!pipe->b_final_output)
         {
            // // printf("xxx process pipe=%p idx=%u\n", pipe, pipeIdx);

            if(0u == pipeIdx)
            {
               // // printf("xxx 2 process pipe=%p idx=%u\n", pipe, pipeIdx);
               pipe->processFrame(_framePlayMaster, _frameRec, _bPlaySeq);

               // (note) 'master' pipe 0 is meant for virtual / internal events only
               //         however, Arranger node audio-clips send events to the audio host
               //          => emit them once, then set b_dont_send flag for all events so
               //              they won't be emitted again by the following nodes
               //              (which all inherit the events, usually for playlist-type of control,
               //               i.e. pattern changes, mutes, ..)
               if(b_delete_note_ons_and_offs)
               {
                  _framePlayMaster->deleteNoteOffsByFlt(-1/*fltDev*/, -1/*fltCh*/);
                  _framePlayMaster->deleteNoteOnsByFlt (-1/*fltDev*/, -1/*fltCh*/);
               }
               _framePlayMaster->emit();
               _framePlayMaster->updateDontSendFlags(YAC_TRUE);
            }
            else
            {
               current_pipe_delay = default_pipe_delay;

               _framePlay->empty();
               _framePlay->mergeFrame(&frameGlobal);
               _framePlay->mergeFrame(_framePlayMaster);

               pipe->processFrame(_framePlay, _frameRec, _bPlaySeq);

               // Move all "global" device events to frameGlobal (for next pipe)
               frameGlobal.empty();

               for(sUI devIdx = 0u; devIdx < max_devices; devIdx++)
               {
                  MIDIPipeDevice *pipeDev = devices[devIdx];

                  if(NULL != pipeDev)
                  {
                     if(pipeDev->b_global)
                     {
                        frameGlobal.mergeFrameFlt(_framePlay,
                                                  pipeDev->dev_idx/*fltDev*/,
                                                  -1/*fltCh*/
                                                  );
                        _framePlay->deleteEventsByFlt(pipeDev->dev_idx, -1/*fltCh*/);

                        // if(frameGlobal.getNumEvents() > 0)
                        //    yac_host->printf("xxx MIDIPipeRoot: frameGlobal has %u events\n", frameGlobal.getNumEvents());
                     }
                  }
               }

               // Add current frame to delay buffer
               if(current_pipe_delay > 0u)
               {
                  sUI dly = (current_pipe_delay >= MAX_PIPE_DELAY) ? (MAX_PIPE_DELAY - 1u) : current_pipe_delay;
                  sUI dlyIdx = (dly + pipe->current_delay_frame_idx) & MAX_PIPE_DELAY_MASK;
                  MIDIPipeFrame *frDelay = &pipe->delay_frames[dlyIdx];
                  frDelay->mergeFrame(_framePlay);
                  _framePlay->empty();
               }

               // Merge delayed frame
               //  (note) current frame has precedence over delayed frame
               frameEmit.empty();
               MIDIPipeFrame *frDelayCur = &pipe->delay_frames[pipe->current_delay_frame_idx];
               frameEmit.mergeFrame(frDelayCur);
               frDelayCur->empty();
               frameEmit.mergeFrame(_framePlay);

               if(NULL == pipeOut)
               {
                  // // frameOut.mergeFrame(_framePlay);
                  if(NULL == frameExportOrNull)
                  {
                     applyVoiceModulo(&frameEmit);

                     if(b_delete_note_ons_and_offs)
                     {
                        frameEmit.deleteNoteOffsByFlt(-1/*fltDev*/, -1/*fltCh*/);
                        frameEmit.deleteNoteOnsByFlt (-1/*fltDev*/, -1/*fltCh*/);
                     }

                     frameEmit.emit();
                  }
                  else
                  {
                     frameExportOrNull->mergeFrame(&frameEmit);
                  }
               }
               else
               {
                  frameOut.mergeFrame(&frameEmit);
               }
            } // !master
         } // !finalOutput

         pipe->current_delay_frame_idx = (pipe->current_delay_frame_idx + 1u) & MAX_PIPE_DELAY_MASK;

      } // if pipe
   } // loop pipes

   if(NULL != pipeOut)
   {
      // Route entire output (framePlay) through final output pipe
      _framePlay->empty();
      _framePlay->mergeFrame(&frameGlobal);
      _framePlay->mergeFrame(_framePlayMaster);
      _framePlay->mergeFrame(&frameOut);

      pipeOut->processFrame(_framePlay, _frameRec, _bPlaySeq);

      if(NULL == frameExportOrNull)
      {
         applyVoiceModulo(_framePlay);

         if(b_delete_note_ons_and_offs)
         {
            _framePlay->deleteNoteOffsByFlt(-1/*fltDev*/, -1/*fltCh*/);
            _framePlay->deleteNoteOnsByFlt (-1/*fltDev*/, -1/*fltCh*/);
         }

         _framePlay->emit();
      }
      else
      {
         frameExportOrNull->mergeFrame(_framePlay);
      }
   }
   else
   {
      // // Merge "SysEx-out Ch1 to Play Frame" events
      // //  (note) prioritize remote_audio output by not transforming input events in node editor
      // //  (note) these events are added to frame_master_inject and then to framePlayMaster in replay.tks
      // //  (note) normally these frames don't need to be emitted and are instead used for internal (virtual devices) only
      // // if(_framePlayMaster->getNumEvents() > 0)
      // //    yac_host->printf("xxx MIDIPipeRoot: emit framePlayMaster #noteOn=%u #noteOff=%u b_delete_note_ons_and_offs=%d\n", _framePlayMaster->getNumEventsNoteOn(), _framePlayMaster->getNumEventsNoteOff(), b_delete_note_ons_and_offs);
      // // frameGlobal.mergeFrame(_framePlayMaster);

      applyVoiceModulo(&frameGlobal);

      if(b_delete_note_ons_and_offs)
      {
         frameGlobal.deleteNoteOffsByFlt(-1/*fltDev*/, -1/*fltCh*/);
         frameGlobal.deleteNoteOnsByFlt (-1/*fltDev*/, -1/*fltCh*/);
      }

      frameGlobal.emit();
   }

}

void MIDIPipeRoot::seek(sUI _ticks, sBool _bSeekAll) {
   for(sUI pipeIdx = 0u; pipeIdx < max_pipes; pipeIdx++)
   {
      if(NULL != pipes[pipeIdx])
      {
         if( (0u == pipeIdx) || _bSeekAll )
         {
            MIDIPipe *pipe = pipes[pipeIdx];
            pipe->seek(_ticks);
         }
      }
   }
}

void MIDIPipeRoot::reset(sBool _bSoft) {

   default_pipe_delay = 0u;
   current_pipe_delay = 0u;

   for(sUI pipeIdx = 0u; pipeIdx < max_pipes; pipeIdx++)
   {
      if(NULL != pipes[pipeIdx])
      {
         MIDIPipe *pipe = pipes[pipeIdx];
         pipe->reset(_bSoft);
      }
   }
}

void MIDIPipeRoot::songSelect(void) {
   for(sUI pipeIdx = 0u; pipeIdx < max_pipes; pipeIdx++)
   {
      if(NULL != pipes[pipeIdx])
      {
         MIDIPipe *pipe = pipes[pipeIdx];
         pipe->songSelect();
      }
   }
}

void MIDIPipeRoot::clearMIDICache(void) {
   for(sUI i = 0u; i < max_devices; i++)
   {
      MIDIPipeDevice *dev = devices[i];
      if(NULL != dev)
         dev->clearMIDICache();
   }
}

void MIDIPipeRoot::clearMIDICacheForDev(sSI _devIdx) {
   if(_devIdx >= 0)
   {
      if(sUI(_devIdx) < max_devices)
      {
         MIDIPipeDevice *dev = devices[_devIdx];
         if(NULL != dev)
            dev->clearMIDICache();
      }
   }
}

MIDIPipe *MIDIPipeRoot::uiGetPipeBeforeIdx(sUI _pipeIdx) {
   if(_pipeIdx < max_pipes)
   {
      if(_pipeIdx > 0u)
      {
         sSI i = sSI(_pipeIdx - 1u);

         while(i >= 0)
         {
            if(NULL != pipes[i])
               return pipes[i];
            i--;
         }
      }
   }
   return NULL;
}

MIDIPipe *MIDIPipeRoot::uiGetPipeAfterIdx(sUI _pipeIdx) {
   if(_pipeIdx < max_pipes)
   {
      sUI i = (_pipeIdx + 1u);

      while(i < max_pipes)
      {
         if(NULL != pipes[i])
            return pipes[i];
         i++;
      }
   }

   return NULL;
}

void MIDIPipeRoot::handleSongSpeedChanged(sBool _bPPQ, sBool _bBPM) {
   for(sUI pipeIdx = 0u; pipeIdx < max_pipes; pipeIdx++)
   {
      if(NULL != pipes[pipeIdx])
         pipes[pipeIdx]->handleSongSpeedChanged(_bPPQ, _bBPM);
   }
}

void MIDIPipeRoot::setPipeDelay(sUI _delay) {
   if(0u == process_pipe_idx)
      default_pipe_delay = _delay;
   current_pipe_delay = _delay;
}

sUI MIDIPipeRoot::getPipeDelay(void) {
   return current_pipe_delay;
}

void MIDIPipeRoot::configureVoiceModulo(sSI _startVoiceIdx, sUI _numVoices) {
   voice_modulo_index = _startVoiceIdx;
   voice_modulo_num   = _numVoices;

   if(voice_modulo_index >= 0)
      voice_modulo_index = sSI(sUI(voice_modulo_index) % _numVoices);

   // yac_host->printf("xxx MIDIPipeRoot::configureVoiceModulo: start=%d num=%u\n", voice_modulo_index, voice_modulo_num);
}

void MIDIPipeRoot::applyVoiceModulo(MIDIPipeFrame *_frameEmit) {
   // Apply voice modulo (see Synergy AudioLiveRecForm)
   // if(numNoteOn > 0u)
   //    yac_host->printf("xxx MIDIPipeRoot::applyVoiceModulo: numNoteOn=%u voice_modulo_index=%d num=%u\n", numNoteOn, voice_modulo_index, voice_modulo_num);
   if(voice_modulo_index >= 0)
   {
      const sUI numNoteOn = _frameEmit->getNumEventsNoteOn();
      if(numNoteOn > 0u)
      {
         // (note) assume that at most one note-on passes (i.e. max note-ons per tick must not exceed voice_modulo_num)
         sUI evIdx = 0u;
         for(sUI noteIdx = 0u; noteIdx < numNoteOn; noteIdx++)
         {
            // yac_host->printf("xxx MIDIPipeRoot::applyVoiceModulo: voice_modulo_index=%d num=%u\n", voice_modulo_index, voice_modulo_num);
            if(0u == voice_modulo_index)
            {
               // keep
               evIdx++;
            }
            else
            {
               // delete event
               _frameEmit->deleteEventByIdx(evIdx, MIDIPIPE_EVENT_TYPE_NOTE_ON);
            }
            voice_modulo_index = sSI(sUI(voice_modulo_index + 1) % voice_modulo_num);
         }
      }
   }
}

void MIDIPipeRoot::setEnableDeleteNoteOnsAndOffs(sBool _bEnable) {
   b_delete_note_ons_and_offs = _bEnable;
}
