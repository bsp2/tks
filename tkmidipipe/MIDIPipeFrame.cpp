// ----
// ---- file   : MIDIPipeFrame.cpp
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2014-2025 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : This is part of the "syntracker" sequencer.
// ----
// ---- created: 01Sep2014
// ---- changed: 03Sep2014, 06Sep2014, 07Sep2014, 08Sep2014, 09Sep2014, 11Jan2015, 14Feb2015
// ----          16Feb2015, 21Mar2015, 26Mar2015, 10Apr2015, 12Apr2015, 14Apr2015, 19Apr2015
// ----          27Apr2015, 15May2015, 30Jun2015, 26Jul2015, 01Nov2015, 02Nov2015, 04Nov2015
// ----          27Feb2016, 28Feb2016, 09Sep2016, 03Dec2016, 19Jan2017, 20Jan2017, 21Jan2017
// ----          06Feb2017, 10Feb2017, 03Mar2017, 06Mar2017, 02Jul2017, 13Aug2017, 18Aug2017
// ----          21Aug2017, 22Nov2017, 25Nov2017, 28Apr2018, 02Jun2018, 17Jun2018, 16Sep2018
// ----          29Dec2018, 22Jan2019, 01May2019, 25May2019, 26May2019, 30May2019, 02Aug2019
// ----          20Oct2019, 24Oct2019, 16Apr2020, 24Apr2020, 25Apr2020, 27Oct2020, 29Oct2020
// ----          15Nov2020, 15May2021, 30May2021, 02Jun2021, 07Jan2022, 24Apr2022, 02Nov2022
// ----          14Jun2023, 08Jul2023, 18Jul2023, 03Dec2023, 06Oct2024, 15Jan2025, 28Mar2025
// ----          05Apr2025, 13Jun2025
// ----
// ----
// ----

#include "tkmidipipe.h"

sUI MIDIPipeFrame::minimum_note_duration = 0u;

MIDIPipeFrame::MIDIPipeFrame(void) {
   parent_pipe    = NULL;
   root           = NULL;
   io_filter      = MIDIPIPE_EVENT_TYPE_NONE;
   usr_frame_idx  = -1;
   time_stamp     = ~0u;
   time_stamp_tmp = 0u;  // (note) NodeSeq assumes that initial tmp state is "false" (0)
   marker         = 0;
   b_dont_send    = YAC_FALSE;
}

MIDIPipeFrame::~MIDIPipeFrame() {
   free();
}

void MIDIPipeFrame::SetMinimumNoteDuration(sUI _numTicks) {
   minimum_note_duration = _numTicks;
}

sUI MIDIPipeFrame::GetMinimumNoteDuration(void) {
   return minimum_note_duration;
}

void MIDIPipeFrame::setPipe(MIDIPipe *_parentPipe) {
   parent_pipe = _parentPipe;

   if(NULL != parent_pipe)
   {
      root = parent_pipe->root;
   }
   else
   {
      root = NULL;
   }
}

MIDIPipe *MIDIPipeFrame::getPipe(void) const {
   return parent_pipe;
}

void MIDIPipeFrame::setRoot(MIDIPipeRoot *_root) {
   root = _root;
}

MIDIPipeRoot *MIDIPipeFrame::getRoot(void) const {
   return root;
}

void MIDIPipeFrame::setTimeStamp(sUI _millisec) {
   time_stamp = _millisec;
}

sUI MIDIPipeFrame::getTimeStamp(void) {
   return time_stamp;
}

void MIDIPipeFrame::setTimeStampTmp(sSI _ticks) {
   time_stamp_tmp = _ticks;
}

sSI MIDIPipeFrame::getTimeStampTmp(void) {
   return time_stamp_tmp;
}

void MIDIPipeFrame::setMarker(sSI _marker) {
   marker = _marker;
}

sSI MIDIPipeFrame::getMarker(void) {
   return marker;
}

void MIDIPipeFrame::setEnableDontSend(sBool _bEnable) {
   b_dont_send = _bEnable;
}

sBool MIDIPipeFrame::getEnableDontSend(void) {
   return b_dont_send;
}

void MIDIPipeFrame::updateDontSendFlags(sBool _bDontSend) {
   prgchg_array       .updateDontSendFlags(_bDontSend);
   chpressure_array   .updateDontSendFlags(_bDontSend);
   pitchbend_array    .updateDontSendFlags(_bDontSend);
   cc_array           .updateDontSendFlags(_bDontSend);
   rpn_array          .updateDontSendFlags(_bDontSend);
   nrpn_array         .updateDontSendFlags(_bDontSend);
   all_notes_off_array.updateDontSendFlags(_bDontSend);
   note_off_array     .updateDontSendFlags(_bDontSend);
   note_on_array      .updateDontSendFlags(_bDontSend);
   polypressure_array .updateDontSendFlags(_bDontSend);
   sysex_array        .updateDontSendFlags(_bDontSend);
   sysrt_array        .updateDontSendFlags(_bDontSend);
}

sBool MIDIPipeFrame::prgChg(sBool _bSet, sSI _devIdx, sUI _midiCh, sSI _prog) {
   if(_devIdx >= 0)
   {
      midipipe_frame_val_t v(_devIdx, _midiCh);

      if(_bSet)
      {
         v.value = (sF32) (_prog < 0 ? 0 : _prog > 127 ? 127 : _prog);
         v.b_dont_send = b_dont_send;

         // printf("xxx MIDIPipeFrame::prgChg: v.midi_ch=%u v.value=%u\n", v.midi_ch, v.value);

         return prgchg_array.set(&v);
      }
      else
      {
         return prgchg_array.unset(&v);
      }
   }
   else
   {
      return YAC_FALSE;
   }
}

sBool MIDIPipeFrame::chPressure(sBool _bSet, sSI _devIdx, sUI _midiCh, sF32 _pressure) {
   if(_devIdx >= 0)
   {
      midipipe_frame_val_t v(_devIdx, _midiCh);

      if(_bSet)
      {
         v.value = (_pressure < 0.0f ? 0.0f : _pressure > 127.0f ? 127.0f : _pressure);
         v.b_dont_send = b_dont_send;

         return chpressure_array.set(&v);
      }
      else
      {
         return chpressure_array.unset(&v);
      }
   }
   else
   {
      return YAC_FALSE;
   }
}

sBool MIDIPipeFrame::pitchbend(sBool _bSet, sSI _devIdx, sUI _midiCh, sF32 _pitchbend) {
   if(_devIdx >= 0)
   {
      midipipe_frame_val_t v(_devIdx, _midiCh);

      if(_bSet)
      {
         v.value = (_pitchbend < 0.0f ? 0.0f : _pitchbend > 16383.0f ? 16383.0f : _pitchbend);

         return pitchbend_array.set(&v);
      }
      else
      {
         return pitchbend_array.unset(&v);
      }
   }
   else
   {
      return YAC_FALSE;
   }
}

sBool MIDIPipeFrame::cc(sBool _bSet, sSI _devIdx, sUI _midiCh, sUI _ccId, sF32 _value) {
   if(_devIdx >= 0)
   {
      midipipe_frame_ctl_t v(_devIdx, _midiCh, (_ccId & 127u));

      if(_bSet)
      {
         v.value = (_value < 0.0f ? 0.0f : _value > 127.0f ? 127.0f : _value);
         v.b_dont_send = b_dont_send;

         return cc_array.set(&v);
      }
      else
      {
         return cc_array.unset(&v);
      }
   }
   else
   {
      return YAC_FALSE;
   }
}

sBool MIDIPipeFrame::rpn(sBool _bSet, sSI _devIdx, sUI _midiCh, sUI _rpn, sF32 _value) {
   if(_devIdx >= 0)
   {
      midipipe_frame_ctl_t v(_devIdx, _midiCh, (sSI(_rpn) & 16383u));

      if(_bSet)
      {
         v.value = (_value < 0.0f ? 0.0f : _value > 16383.0f ? 16383.0f : _value);
         v.b_dont_send = b_dont_send;

         // printf("xxx rpn_array.set(rpn=%u value=%f)\n", v.ctl_id, v.value);

         return rpn_array.set(&v);
      }
      else
      {
         return rpn_array.unset(&v);
      }
   }
   else
   {
      return YAC_FALSE;
   }
}

sBool MIDIPipeFrame::nrpn(sBool _bSet, sSI _devIdx, sUI _midiCh, sUI _nrpn, sF32 _value) {
   if(_devIdx >= 0)
   {
      midipipe_frame_ctl_t v(_devIdx, _midiCh, (sSI(_nrpn) & 16383u));

      if(_bSet)
      {
         v.value = (_value < 0.0f ? 0.0f : _value > 16383.0f ? 16383.0f : _value);
         v.b_dont_send = b_dont_send;

         return nrpn_array.set(&v);
      }
      else
      {
         return nrpn_array.unset(&v);
      }
   }
   else
   {
      return YAC_FALSE;
   }
}

sBool MIDIPipeFrame::allNotesOff(sBool _bSet, sSI _devIdx, sUI _midiCh) {
   if( (_devIdx >= 0) && (_midiCh >= 0) && (_midiCh < 16) )
   {
      midipipe_frame_base_t v(_devIdx, _midiCh);
      v.b_dont_send = b_dont_send;

      if(_bSet)
      {
         return all_notes_off_array.set(&v);
      }
      else
      {
         return all_notes_off_array.unset(&v);
      }
   }
   else
   {
      return YAC_FALSE;
   }
}

sBool MIDIPipeFrame::noteOff(sBool _bSet, sSI _devIdx, sUI _midiCh, sSI _noteNr, sF32 _vel) {
   if( (_devIdx >= 0) && (_midiCh >= 0) && (_midiCh < 16) && (_noteNr >= 0) && (_noteNr <= 127) )
   {
      // (note) caller should handle this
      // // noteOn(0/*clear*/, _devIdx, _midiCh, _noteNr, 0/*vel*/, 0/*dur*/);

      midipipe_frame_note_t v(_devIdx, _midiCh,
                              ////(sUI) (_noteNr < 0 ? 0 : _noteNr > 127 ? 127 : _noteNr)
                              (sUI) _noteNr
                              );

      if(_bSet)
      {
         v.velocity = (_vel < 0.0f ? 0.0f : _vel > 127.0f ? 127.0f : _vel);
         v.duration = 0;
         v.b_pre    = YAC_FALSE;
         v.b_dont_send = b_dont_send;

         return note_off_array.set(&v);
      }
      else
      {
         return note_off_array.unset(&v);
      }
   }
   else
   {
      return YAC_FALSE;
   }
}

sBool MIDIPipeFrame::noteOffPre(sBool _bSet, sSI _devIdx, sUI _midiCh, sSI _noteNr, sF32 _vel) {
   if( (_devIdx >= 0) && (_midiCh >= 0) && (_midiCh < 16) && (_noteNr >= 0) && (_noteNr <= 127) )
   {
      // (note) caller should handle this
      // // noteOn(0/*clear*/, _devIdx, _midiCh, _noteNr, 0/*vel*/, 0/*dur*/);

      midipipe_frame_note_t v(_devIdx, _midiCh,
                              ////(sUI) (_noteNr < 0 ? 0 : _noteNr > 127 ? 127 : _noteNr)
                              (sUI) _noteNr
                              );

      if(_bSet)
      {
         v.velocity = (_vel < 0.0f ? 0.0f : _vel > 127.0f ? 127.0f : _vel);
         v.duration = 0;
         v.b_pre    = YAC_TRUE;
         v.b_dont_send = b_dont_send;

         return note_off_array.set(&v);
      }
      else
      {
         return note_off_array.unset(&v);
      }
   }
   else
   {
      return YAC_FALSE;
   }
}

sBool MIDIPipeFrame::noteOffReplace(sSI _devIdx, sUI _midiCh, sSI _oldNoteNr, sSI _newNoteNr, sF32 _vel) {

   sBool bReplaced = YAC_FALSE;

   if( (_devIdx >= 0) && (_midiCh >= 0) && (_midiCh < 16) )
   {
      _oldNoteNr &= 127;
      _newNoteNr &= 127;
      _vel = (_vel < 0.0f ? 0.0f : _vel > 127.0f ? 127.0f : _vel);

      sUI i;
      midipipe_frame_note_t *ev = note_off_array.values;
      sUI numLeft = note_off_array.num_values;

      for(i=0; (i < note_off_array.max_values) && (numLeft > 0); i++, ev++)
      {
         if(-1 != ev->dev_idx)
         {
            if(ev->dev_idx == _devIdx)
            {
               if(ev->midi_ch == _midiCh)
               {
                  if(sSI(ev->note_nr) == _oldNoteNr)
                  {
                     if(bReplaced)
                     {
                        // Delete duplicate note
                        ev->dev_idx = -1;
                        note_off_array.num_values--;
                        break;
                     }
                     else
                     {
                        ev->note_nr  = _newNoteNr;
                        ev->velocity = _vel;
                        bReplaced = YAC_TRUE;
                     }
                  }
               }
            }

            numLeft--;
         }
      }
   }

   return bReplaced;
}

sBool MIDIPipeFrame::noteOn(sBool _bSet, sSI _devIdx, sUI _midiCh, sSI _noteNr, sF32 _vel, sSI _dur) {
   if( (_devIdx >= 0) && (_midiCh >= 0) && (_midiCh < 16) && (_noteNr >= 0) && (_noteNr <= 127) )
   {
      midipipe_frame_note_t v(_devIdx, _midiCh,
                              ////(sUI) (_noteNr < 0 ? 0 : _noteNr > 127 ? 127 : _noteNr)
                              (sUI) _noteNr
                              );

      if(_bSet)
      {
         v.velocity = (_vel < 0.0f ? 0.0f : _vel > 127.0f ? 127.0f : _vel);

         if(minimum_note_duration > 0u)
         {
            if(_dur > 0)
               if(_dur < sSI(minimum_note_duration))
                  _dur = minimum_note_duration;
         }

         v.duration = (sUI) (_dur < 0 ? 0 : _dur);
         v.b_pre    = YAC_FALSE;
         v.b_dont_send = b_dont_send;

         return note_on_array.set(&v);
      }
      else
      {
         return note_on_array.unset(&v);
      }
   }
   else
   {
      return YAC_FALSE;
   }
}

sBool MIDIPipeFrame::noteOnReplace(sSI _devIdx, sUI _midiCh, sSI _oldNoteNr, sSI _newNoteNr, sF32 _vel, sSI _dur) {

   sBool bReplaced = YAC_FALSE;

   if( (_devIdx >= 0) && (_midiCh >= 0) && (_midiCh < 16) )
   {
      _oldNoteNr &= 127;
      _newNoteNr &= 127;
      _vel = (_vel < 0.0f ? 0.0f : _vel > 127.0f ? 127.0f : _vel);

      sUI i;
      midipipe_frame_note_t *ev = note_on_array.values;
      sUI numLeft = note_on_array.num_values;

      for(i=0; (i < note_on_array.max_values) && (numLeft > 0); i++, ev++)
      {
         if(-1 != ev->dev_idx)
         {
            if(ev->dev_idx == _devIdx)
            {
               if(ev->midi_ch == _midiCh)
               {
                  if(sSI(ev->note_nr) == _oldNoteNr)
                  {
                     if(bReplaced)
                     {
                        // Delete duplicate note
                        ev->dev_idx = -1;
                        note_on_array.num_values--;
                        break;
                     }
                     else
                     {
                        ev->note_nr  = _newNoteNr;
                        ev->velocity = _vel;
                        ev->duration = (_dur < 0) ? 0 : _dur;
                        bReplaced = YAC_TRUE;
                     }
                  }
               }
            }

            numLeft--;
         }
      }
   }

   return bReplaced;
}

sUI MIDIPipeFrame::noteOnVelReplace(sSI _fltDev, sSI _fltCh, sF32 _vel) {
   sBool numChanged = 0;

   if(_vel < 1.0f)
      _vel = 1.0f;
   else if(_vel > 127.0f)
      _vel = 127.0f;

   sUI i;
   midipipe_frame_note_t *ev = note_on_array.values;
   sUI numLeft = note_on_array.num_values;

   for(i=0; (i < note_on_array.max_values) && (numLeft > 0); i++, ev++)
   {
      if(-1 != ev->dev_idx)
      {
         if((-1 == _fltDev) || (_fltDev == ev->dev_idx))
         {
            if((-1 == _fltCh) || (_fltCh == sSI(ev->midi_ch)))
            {
               ev->velocity = _vel;
               numChanged++;
            }
         }

         numLeft--;
      }
   }

   return numChanged;
}

sUI MIDIPipeFrame::noteOnDeleteZeroVel(void) {
   sUI numDeleted = 0u;
   sUI i;
   midipipe_frame_note_t *ev = note_on_array.values;
   sUI numLeft = note_on_array.num_values;

   for(i=0; (i < note_on_array.max_values) && (numLeft > 0); i++, ev++)
   {
      if(-1 != ev->dev_idx)
      {
         if(ev->velocity <= 0.0f)
         {
            ev->dev_idx = -1;
            note_on_array.num_values--;
            numDeleted++;
         }

         numLeft--;
      }
   }

   return numDeleted;
}

sUI MIDIPipeFrame::noteOnVelMul(sSI _fltDev, sSI _fltCh, sF32 _s) {
   sBool numChanged = 0;

   sUI i;
   midipipe_frame_note_t *ev = note_on_array.values;
   sUI numLeft = note_on_array.num_values;

   for(i=0; (i < note_on_array.max_values) && (numLeft > 0); i++, ev++)
   {
      if(-1 != ev->dev_idx)
      {
         if((-1 == _fltDev) || (_fltDev == ev->dev_idx))
         {
            if((-1 == _fltCh) || (_fltCh == sSI(ev->midi_ch)))
            {
               sF32 v = ev->velocity;
               v = (v * _s);

               if(v < 1.0f)
                  v = 1.0f;
               else if(v > 127.0f)
                  v = 127.0f;

               ev->velocity = v;
               numChanged++;
            }
         }

         numLeft--;
      }
   }

   return numChanged;
}

sUI MIDIPipeFrame::noteOnVelAdd(sSI _fltDev, sSI _fltCh, sF32 _vel) {
   sBool numChanged = 0;

   sUI i;
   midipipe_frame_note_t *ev = note_on_array.values;
   sUI numLeft = note_on_array.num_values;

   for(i=0; (i < note_on_array.max_values) && (numLeft > 0); i++, ev++)
   {
      if(-1 != ev->dev_idx)
      {
         if((-1 == _fltDev) || (_fltDev == ev->dev_idx))
         {
            if((-1 == _fltCh) || (_fltCh == sSI(ev->midi_ch)))
            {
               sF32 v = ev->velocity;
               v += _vel;

               if(v < 1.0f)
                  v = 1.0f;
               else if(v > 127.0f)
                  v = 127.0f;

               ev->velocity = v;
               numChanged++;
            }
         }

         numLeft--;
      }
   }

   return numChanged;
}

sBool MIDIPipeFrame::noteOnVelReplaceByIdxAndFlt(sUI _idx, sSI _fltDev, sSI _fltCh, sF32 _vel) {

   // (note) allow note on to become note off
   if(_vel < 0.0f)
      _vel = 0.0f;
   else if(_vel > 127.0f)
      _vel = 127.0f;

   sUI i;
   midipipe_frame_note_t *ev = note_on_array.values;
   sUI numLeft = note_on_array.num_values;
   sUI evIdx = 0u;

   for(i=0; (i < note_on_array.max_values) && (numLeft > 0); i++, ev++)
   {
      if(-1 != ev->dev_idx)
      {
         if((-1 == _fltDev) || (_fltDev == ev->dev_idx))
         {
            if((-1 == _fltCh) || (_fltCh == sSI(ev->midi_ch)))
            {
               if(_idx == evIdx)
               {
                  ev->velocity = _vel;
                  return YAC_TRUE;
               }
               else
               {
                  evIdx++;
               }
            }
         }

         numLeft--;
      }
   }

   return YAC_FALSE;
}

sBool MIDIPipeFrame::noteOffVelReplaceByIdxAndFlt(sUI _idx, sSI _fltDev, sSI _fltCh, sF32 _vel) {

   if(_vel < 0.0f)
      _vel = 0.0f;
   else if(_vel > 127.0f)
      _vel = 127.0f;

   sUI i;
   midipipe_frame_note_t *ev = note_off_array.values;
   sUI numLeft = note_off_array.num_values;
   sUI evIdx = 0u;

   for(i=0; (i < note_off_array.max_values) && (numLeft > 0); i++, ev++)
   {
      if(-1 != ev->dev_idx)
      {
         if((-1 == _fltDev) || (_fltDev == ev->dev_idx))
         {
            if((-1 == _fltCh) || (_fltCh == sSI(ev->midi_ch)))
            {
               if(_idx == evIdx)
               {
                  ev->velocity = _vel;
                  return YAC_TRUE;
               }
               else
               {
                  evIdx++;
               }
            }
         }

         numLeft--;
      }
   }

   return YAC_FALSE;
}

void MIDIPipeFrame::noteOnUpdateByIdx(sSI _fltDev, sSI _fltCh, sUI _evIdx, sSI _newNote) {
   // called by NodeSeqTrack.transposeSelectedNoteFrames()

   // note-on
   sUI i;
   midipipe_frame_note_t *ev = note_on_array.values;
   sUI numLeft = note_on_array.num_values;
   sUI evIdx = 0u;

   for(i=0; (i < note_on_array.max_values) && (numLeft > 0); i++, ev++)
   {
      if(-1 != ev->dev_idx)
      {
         if((-1 == _fltDev) || (_fltDev == ev->dev_idx))
         {
            if((-1 == _fltCh) || (_fltCh == sSI(ev->midi_ch)))
            {
               if(evIdx == _evIdx)
               {
                  ev->note_nr = _newNote;
                  return;
               }
               evIdx++;
            }
         }

         numLeft--;
      }
   }
}


sBool MIDIPipeFrame::polyPressure(sBool _bSet, sSI _devIdx, sUI _midiCh, sSI _noteNr, sF32 _pressure) {
   if(_devIdx >= 0)
   {
      midipipe_frame_pat_t v(_devIdx, _midiCh);
      v.note_nr = (sUI) (_noteNr < 0 ? 0 : _noteNr > 127 ? 127 : _noteNr);

      if(_bSet)
      {
         v.value = (_pressure < 0.0f ? 0.0f : _pressure > 127.0f ? 127.0f : _pressure);
         v.b_dont_send = b_dont_send;

         // yac_host->printf("xxx midipipeframe::polypressure: set devIdx=%d midiCh=%u noteNr=%d pressure=%d\n", _devIdx, _midiCh, _noteNr, _pressure);

         return polypressure_array.set(&v);
      }
      else
      {
         return polypressure_array.unset(&v);
      }
   }
   else
   {
      return YAC_FALSE;
   }
}

sBool MIDIPipeFrame::polyPressureReplace(sSI _devIdx, sUI _midiCh, sSI _oldNoteNr, sSI _newNoteNr, sF32 _pressure) {

   sBool bReplaced = YAC_FALSE;

   if( (_devIdx >= 0) && (_midiCh >= 0) && (_midiCh < 16) )
   {
      _oldNoteNr &= 127;
      _newNoteNr &= 127;

      sUI i;
      midipipe_frame_pat_t *ev = polypressure_array.values;
      sUI numLeft = polypressure_array.num_values;

      for(i=0; (i < polypressure_array.max_values) && (numLeft > 0); i++, ev++)
      {
         if(-1 != ev->dev_idx)
         {
            if(ev->dev_idx == _devIdx)
            {
               if(ev->midi_ch == _midiCh)
               {
                  if(sSI(ev->note_nr) == _oldNoteNr)
                  {
                     if(bReplaced)
                     {
                        // Delete duplicate event
                        ev->dev_idx = -1;
                        polypressure_array.num_values--;
                        break;
                     }
                     else
                     {
                        ev->note_nr   = _newNoteNr;
                        ev->value = (_pressure < 0.0f) ? ev->value : ( (_pressure > 127.0f ? 127.0f : _pressure));
                        bReplaced = YAC_TRUE;
                     }
                  }
               }
            }

            numLeft--;
         }
      }
   }

   return bReplaced;
}

sBool MIDIPipeFrame::sysex(sBool _bSet, sSI _devIdx, YAC_Buffer *_buffer) { // (note) including 0xF0/0xF7 bytes
   if(_devIdx >= 0)
   {
      midipipe_frame_sysex_t v(_devIdx, 0/*_midiCh,ignored*/);

      if(_bSet)
      {
         if(YAC_Is_Buffer(_buffer))
         {
            // (note) set() method copies buffer content
            v.buffer = _buffer;
            // yac_host->printf("MIDIPipeFrame::sysex: v.buffer.size=%u, io_offset=%u\n", v.buffer->size, v.buffer->io_offset);

            return sysex_array.set(&v);
         }
         else
         {
            return YAC_FALSE;
         }
      }
      else
      {
         return sysex_array.unset(&v);
      }
   }
   else
   {
      return YAC_FALSE;
   }
}

sBool MIDIPipeFrame::sysexAddF0F7(sBool _bSet, sSI _devIdx, YAC_Buffer *_buffer, sBool _bDontSend) { // (note) add 0xF0/0xF7 bytes

   // yac_host->printf("xxx MIDIPipeFrame::sysexAddF0F7: bSet=%d devIdx=%d buffer=0x%p\n", _bSet, _devIdx, _buffer);

   if(_devIdx >= 0)
   {
      midipipe_frame_sysex_t v(_devIdx, 0/*_midiCh,ignored*/);

      if(_bSet)
      {
         if(YAC_Is_Buffer(_buffer))
         {
            // yac_host->printf("xxx MIDIPipeFrame::sysexAddF0F7: enclose buffer in F0..F7\n");

            v.b_dont_send = _bDontSend;

            v.buffer = YAC_New_Buffer();
            v.buffer->yacArrayAlloc(_buffer->io_offset + 2);
            v.buffer->io_offset = 0;
            v.buffer->yacStreamWriteI8(0xF0);
            v.buffer->yacStreamWriteBuffer(_buffer, 0/*srcoff*/, _buffer->io_offset);
            v.buffer->yacStreamWriteI8(0xF7);

            // yac_host->printf("MIDIPipeFrame::sysexAddF0F7: v.buffer=%p v.buffer.size=%u, io_offset=%u\n", v.buffer, v.buffer->size, v.buffer->io_offset);
            sBool ret = sysex_array.set(&v);

            // yac_host->printf("xxx sysex_array.set() returned\n");
            YAC_DELETE_SAFE(v.buffer);

            return ret;
         }
         else
         {
            // // v.buffer->yacArrayRealloc(0, 0, 0, 0);
            return YAC_FALSE;
         }
      }
      else
      {
         return sysex_array.unset(&v);
      }
   }
   else
   {
      return YAC_FALSE;
   }
}

sBool MIDIPipeFrame::sysrt(sBool _bSet, sSI _devIdx, sUI _byte) {
   if(_devIdx >= 0)
   {
      midipipe_frame_sysrt_t v(_devIdx, 0/*_midiCh,ignored*/);
      v.sysrt_byte = (sU8)(_byte & 255u);

      if(_bSet)
      {
         v.b_dont_send = b_dont_send;

         return sysrt_array.set(&v);
      }
      else
      {
         return sysrt_array.unset(&v);
      }
   }
   else
   {
      return YAC_FALSE;
   }
}

sBool MIDIPipeFrame::hasEvents(void) const {
   return
      (prgchg_array       .num_values > 0u) ||
      (chpressure_array   .num_values > 0u) ||
      (pitchbend_array    .num_values > 0u) ||
      (cc_array           .num_values > 0u) ||
      (rpn_array          .num_values > 0u) ||
      (nrpn_array         .num_values > 0u) ||
      (all_notes_off_array.num_values > 0u) ||
      (note_off_array     .num_values > 0u) ||
      (note_on_array      .num_values > 0u) ||
      (polypressure_array .num_values > 0u) ||
      (sysex_array        .num_values > 0u) ||
      (sysrt_array        .num_values > 0u) ;
}

sUI MIDIPipeFrame::getNumEvents(void) const {
   return
      prgchg_array       .num_values +
      chpressure_array   .num_values +
      pitchbend_array    .num_values +
      cc_array           .num_values +
      rpn_array          .num_values +
      nrpn_array         .num_values +
      all_notes_off_array.num_values +
      note_off_array     .num_values +
      note_on_array      .num_values +
      polypressure_array .num_values +
      sysrt_array        .num_values +
      sysex_array        .num_values ;
}

sUI MIDIPipeFrame::getNumEventsPrgChg(void) const {
   return prgchg_array       .num_values;
}

sUI MIDIPipeFrame::getNumEventsChPressure(void) const {
   return chpressure_array   .num_values;
}

sUI MIDIPipeFrame::getNumEventsPitchbend(void) const {
   return pitchbend_array    .num_values;
}

sUI MIDIPipeFrame::getNumEventsCC(void) const {
   return cc_array           .num_values;
}

sUI MIDIPipeFrame::getNumEventsRPN(void) const {
   return rpn_array          .num_values;
}

sUI MIDIPipeFrame::getNumEventsNRPN(void) const {
   return nrpn_array         .num_values;
}

sUI MIDIPipeFrame::getNumEventsAllNotesOff(void) const {
   return all_notes_off_array.num_values;
}

sUI MIDIPipeFrame::getNumEventsNoteOffPre(void) const {
   sUI r = 0u;
   sUI numLeft = note_off_array.num_values;
   midipipe_frame_note_t *ev = note_off_array.values;

   for(sUI i=0; (i<note_off_array.max_values) && (numLeft > 0); i++, ev++)
   {
      if(-1 != ev->dev_idx)
      {
         if(ev->b_pre)
            r++;

         numLeft--;
      }
   }

   return r;
}

sUI MIDIPipeFrame::getNumEventsNoteOffPost(void) const {
   sUI r = 0u;
   sUI numLeft = note_off_array.num_values;
   midipipe_frame_note_t *ev = note_off_array.values;

   for(sUI i=0; (i<note_off_array.max_values) && (numLeft > 0); i++, ev++)
   {
      if(-1 != ev->dev_idx)
      {
         if(!ev->b_pre)
            r++;

         numLeft--;
      }
   }

   return r;
}

sUI MIDIPipeFrame::getNumEventsNoteOff(void) const {
   return note_off_array     .num_values;
}

sUI MIDIPipeFrame::getNumEventsNoteOn(void) const {
   return note_on_array      .num_values;
}

sUI MIDIPipeFrame::getNumEventsPolyPressure(void) const {
   return polypressure_array .num_values;
}

sUI MIDIPipeFrame::getNumEventsSysEx(void) const {
   return sysex_array        .num_values;
}

sUI MIDIPipeFrame::getNumEventsSysRT(void) const {
   return sysrt_array        .num_values;
}

midipipe_frame_u *MIDIPipeFrame::getEventLocByIdxAndFlt(sUI  _idx,
                                                        sUI *_retRelIdx,
                                                        sUI *_retAbsIdx,
                                                        sUI *_retEventType,
                                                        sSI  _fltType,
                                                        sSI  _fltDev,
                                                        sSI  _fltCh,
                                                        sSI  _globalCh // alt ch
                                                        ) const {
   sUI relIdx = _idx;

   *_retRelIdx = 0;
   *_retAbsIdx = 0;
   *_retEventType = 0;

   sBool bFltAll = ((-1 == _fltDev) && (-1 == _fltCh) && (-1 == _globalCh));

   sUI fltNumEv;

   // Program change
   if( (-1 == _fltType) || (MIDIPIPE_EVENT_TYPE_PRGCHG == _fltType) )
   {
      if(bFltAll ? (relIdx < prgchg_array.num_values) : 1)
      {
         const midipipe_frame_val_t *ev = prgchg_array.getByUsedIdxAndFlt(relIdx, _fltDev, _fltCh, _globalCh, fltNumEv);

         if(NULL != ev)
         {
            *_retRelIdx = relIdx;
            *_retEventType = MIDIPIPE_EVENT_TYPE_PRGCHG;
            *_retAbsIdx += relIdx;

            return (midipipe_frame_u*)ev;
         }
         else
         {
            relIdx -= fltNumEv;
         }
      }
      else
      {
         relIdx -= prgchg_array.num_values;
      }
   }

   *_retAbsIdx += prgchg_array.num_values;


   // Channel pressure
   if( (-1 == _fltType) || (MIDIPIPE_EVENT_TYPE_CHPRESSURE == _fltType) )
   {
      if(bFltAll ? (relIdx < chpressure_array.num_values) : 1)
      {
         const midipipe_frame_val_t *ev = chpressure_array.getByUsedIdxAndFlt(relIdx, _fltDev, _fltCh, _globalCh, fltNumEv);

         if(NULL != ev)
         {
            *_retRelIdx = relIdx;
            *_retEventType = MIDIPIPE_EVENT_TYPE_CHPRESSURE;
            *_retAbsIdx += relIdx;

            return (midipipe_frame_u*)ev;
         }
         else
         {
            relIdx -= fltNumEv;
         }
      }
      else
      {
         relIdx -= chpressure_array.num_values;
      }
   }

   *_retAbsIdx += chpressure_array.num_values;


   // Pitchbend
   if( (-1 == _fltType) || (MIDIPIPE_EVENT_TYPE_PITCHBEND == _fltType) )
   {
      if(bFltAll ? (relIdx < pitchbend_array.num_values) : 1)
      {
         const midipipe_frame_val_t *ev = pitchbend_array.getByUsedIdxAndFlt(relIdx, _fltDev, _fltCh, _globalCh, fltNumEv);

         if(NULL != ev)
         {
            *_retRelIdx = relIdx;
            *_retEventType = MIDIPIPE_EVENT_TYPE_PITCHBEND;
            *_retAbsIdx += relIdx;

            return (midipipe_frame_u*)ev;
         }
         else
         {
            relIdx -= fltNumEv;
         }
      }
      else
      {
         relIdx -= pitchbend_array.num_values;
      }
   }

   *_retAbsIdx += pitchbend_array.num_values;


   // Continuous controller
   if( (-1 == _fltType) || (MIDIPIPE_EVENT_TYPE_CC == _fltType) )
   {
      if(bFltAll ? (relIdx < cc_array.num_values) : 1)
      {
         const midipipe_frame_ctl_t *ev = cc_array.getByUsedIdxAndFlt(relIdx, _fltDev, _fltCh, _globalCh, fltNumEv);

         if(NULL != ev)
         {
            *_retRelIdx = relIdx;
            *_retEventType = MIDIPIPE_EVENT_TYPE_CC;
            *_retAbsIdx += relIdx;

            return (midipipe_frame_u*)ev;
         }
         else
         {
            relIdx -= fltNumEv;
         }
      }
      else
      {
         relIdx -= cc_array.num_values;
      }
   }

   *_retAbsIdx += cc_array.num_values;


   // RPN
   if( (-1 == _fltType) || (MIDIPIPE_EVENT_TYPE_RPN == _fltType) )
   {
      if(bFltAll ? (relIdx < rpn_array.num_values) : 1)
      {
         const midipipe_frame_ctl_t *ev = rpn_array.getByUsedIdxAndFlt(relIdx, _fltDev, _fltCh, _globalCh, fltNumEv);

         if(NULL != ev)
         {
            *_retRelIdx = relIdx;
            *_retEventType = MIDIPIPE_EVENT_TYPE_RPN;
            *_retAbsIdx += relIdx;

            return (midipipe_frame_u*)ev;
         }
         else
         {
            relIdx -= fltNumEv;
         }
      }
      else
      {
         relIdx -= rpn_array.num_values;
      }
   }

   *_retAbsIdx += rpn_array.num_values;


   // NRPN
   if( (-1 == _fltType) || (MIDIPIPE_EVENT_TYPE_NRPN == _fltType) )
   {
      if(bFltAll ? (relIdx < nrpn_array.num_values) : 1)
      {
         const midipipe_frame_ctl_t *ev = nrpn_array.getByUsedIdxAndFlt(relIdx, _fltDev, _fltCh, _globalCh, fltNumEv);

         if(NULL != ev)
         {
            *_retRelIdx = relIdx;
            *_retEventType = MIDIPIPE_EVENT_TYPE_NRPN;
            *_retAbsIdx += relIdx;

            return (midipipe_frame_u*)ev;
         }
         else
         {
            relIdx -= fltNumEv;
         }
      }
      else
      {
         relIdx -= nrpn_array.num_values;
      }
   }

   *_retAbsIdx += nrpn_array.num_values;


   // All Notes Off
   if( (-1 == _fltType) || (MIDIPIPE_EVENT_TYPE_ALL_NOTES_OFF == _fltType) )
   {
      if(bFltAll ? (relIdx < all_notes_off_array.num_values) : 1)
      {
         const midipipe_frame_base_t *ev = all_notes_off_array.getByUsedIdxAndFlt(relIdx, _fltDev, _fltCh, _globalCh, fltNumEv);

         if(NULL != ev)
         {
            *_retRelIdx = relIdx;
            *_retEventType = MIDIPIPE_EVENT_TYPE_ALL_NOTES_OFF;
            *_retAbsIdx += relIdx;

            return (midipipe_frame_u*)ev;
         }
         else
         {
            relIdx -= fltNumEv;
         }
      }
      else
      {
         relIdx -= all_notes_off_array.num_values;
      }
   }

   *_retAbsIdx += all_notes_off_array.num_values;


   // Note Off
   if( (-1 == _fltType) || (MIDIPIPE_EVENT_TYPE_NOTE_OFF == _fltType) )
   {
      if(bFltAll ? (relIdx < note_off_array.num_values) : 1)
      {
         const midipipe_frame_note_t *ev = note_off_array.getByUsedIdxAndFlt(relIdx, _fltDev, _fltCh, _globalCh, fltNumEv);

         if(NULL != ev)
         {
            *_retRelIdx = relIdx;
            *_retEventType = MIDIPIPE_EVENT_TYPE_NOTE_OFF;
            *_retAbsIdx += relIdx;

            return (midipipe_frame_u*)ev;
         }
         else
         {
            relIdx -= fltNumEv;
         }
      }
      else
      {
         relIdx -= note_off_array.num_values;
      }
   }

   *_retAbsIdx += note_off_array.num_values;


   // Note On
   if( (-1 == _fltType) || (MIDIPIPE_EVENT_TYPE_NOTE_ON == _fltType) )
   {
      if(bFltAll ? (relIdx < note_on_array.num_values) : 1)
      {
         const midipipe_frame_note_t *ev = note_on_array.getByUsedIdxAndFlt(relIdx, _fltDev, _fltCh, _globalCh, fltNumEv);

         if(NULL != ev)
         {
            *_retRelIdx = relIdx;
            *_retEventType = MIDIPIPE_EVENT_TYPE_NOTE_ON;
            *_retAbsIdx += relIdx;

            return (midipipe_frame_u*)ev;
         }
         else
         {
            relIdx -= fltNumEv;
         }
      }
      else
      {
         relIdx -= note_on_array.num_values;
      }
   }

   *_retAbsIdx += note_on_array.num_values;


   // Poly pressure
   if( (-1 == _fltType) || (MIDIPIPE_EVENT_TYPE_POLYPRESSURE == _fltType) )
   {
      if(bFltAll ? (relIdx < polypressure_array.num_values) : 1)
      {
         const midipipe_frame_pat_t *ev = polypressure_array.getByUsedIdxAndFlt(relIdx, _fltDev, _fltCh, _globalCh, fltNumEv);

         if(NULL != ev)
         {
            *_retRelIdx = relIdx;
            *_retEventType = MIDIPIPE_EVENT_TYPE_POLYPRESSURE;
            *_retAbsIdx += relIdx;

            return (midipipe_frame_u*)ev;
         }
         else
         {
            relIdx -= fltNumEv;
         }
      }
      else
      {
         relIdx -= polypressure_array.num_values;
      }
   }

   *_retAbsIdx += polypressure_array.num_values;


   // SysEx
   if( (-1 == _fltType) || (MIDIPIPE_EVENT_TYPE_SYSEX == _fltType) )
   {
      if(bFltAll ? (relIdx < sysex_array.num_values) : 1)
      {
         const midipipe_frame_sysex_t *ev = sysex_array.getByUsedIdxAndFlt(relIdx, _fltDev, -1/*fltCh,ignored*/, -1/*globalCh,ignored*/, fltNumEv);

         if(NULL != ev)
         {
            *_retRelIdx = relIdx;
            *_retEventType = MIDIPIPE_EVENT_TYPE_SYSEX;
            *_retAbsIdx += relIdx;

            // yac_host->printf("xxx MIDIPipeFrame::getEventByIdx<sysex>: ev->midi_ch=%d\n", ev->midi_ch);

            return (midipipe_frame_u*)ev;
         }
         else
         {
            relIdx -= fltNumEv;
         }
      }
      else
      {
         //relIdx -= sysex_array.num_values;
      }
   }

   // SysRT
   if( (-1 == _fltType) || (MIDIPIPE_EVENT_TYPE_SYSRT == _fltType) )
   {
      if(bFltAll ? (relIdx < sysrt_array.num_values) : 1)
      {
         const midipipe_frame_sysrt_t *ev = sysrt_array.getByUsedIdxAndFlt(relIdx, _fltDev, -1/*fltCh,ignored*/, -1/*globalCh,ignored*/, fltNumEv);

         if(NULL != ev)
         {
            *_retRelIdx = relIdx;
            *_retEventType = MIDIPIPE_EVENT_TYPE_SYSRT;
            *_retAbsIdx += relIdx;

            return (midipipe_frame_u*)ev;
         }
         else
         {
            relIdx -= fltNumEv;
         }
      }
      else
      {
         //relIdx -= sysex_array.num_values;
      }
   }

   // *_retAbsIdx += sysex_array.num_values;


   return NULL;
}

midipipe_frame_u *MIDIPipeFrame::getEventLocByIdx(sUI  _idx,
                                                  sUI *_retRelIdx,
                                                  sUI *_retAbsIdx,
                                                  sUI *_retEventType,
                                                  sSI  _fltType
                                                  ) const {
   return getEventLocByIdxAndFlt(_idx, _retRelIdx, _retAbsIdx, _retEventType, _fltType, -1/*fltDev*/, -1/*fltCh*/, -1/*globalCh*/);
}

sUI MIDIPipeFrame::getNumEventsByFlt(sSI _fltDev, sSI _fltCh) const {
   return
      getNumEventsPrgChgByFlt      (_fltDev, _fltCh) +
      getNumEventsChPressureByFlt  (_fltDev, _fltCh) +
      getNumEventsPitchbendByFlt   (_fltDev, _fltCh) +
      getNumEventsCCByFlt          (_fltDev, _fltCh) +
      getNumEventsRPNByFlt         (_fltDev, _fltCh) +
      getNumEventsNRPNByFlt        (_fltDev, _fltCh) +
      getNumEventsAllNotesOffByFlt (_fltDev, _fltCh) +
      getNumEventsNoteOffByFlt     (_fltDev, _fltCh) +
      getNumEventsNoteOnByFlt      (_fltDev, _fltCh) +
      getNumEventsPolyPressureByFlt(_fltDev, _fltCh) +
      getNumEventsSysExByFlt       (_fltDev)         +
      getNumEventsSysRTByFlt       (_fltDev)         ;
}

sUI MIDIPipeFrame::getNumEventsPrgChgByFlt(sSI _fltDev, sSI _fltCh) const {
   return prgchg_array.getNumEventsByFlt(_fltDev, _fltCh);
}

sUI MIDIPipeFrame::getNumEventsChPressureByFlt(sSI _fltDev, sSI _fltCh) const {
   return chpressure_array.getNumEventsByFlt(_fltDev, _fltCh);
}

sUI MIDIPipeFrame::getNumEventsPitchbendByFlt(sSI _fltDev, sSI _fltCh) const {
   return pitchbend_array.getNumEventsByFlt(_fltDev, _fltCh);
}

sUI MIDIPipeFrame::getNumEventsCCByFlt(sSI _fltDev, sSI _fltCh) const {
   return cc_array.getNumEventsByFlt(_fltDev, _fltCh);
}

sUI MIDIPipeFrame::getNumEventsRPNByFlt(sSI _fltDev, sSI _fltCh) const {
   return rpn_array.getNumEventsByFlt(_fltDev, _fltCh);
}

sUI MIDIPipeFrame::getNumEventsNRPNByFlt(sSI _fltDev, sSI _fltCh) const {
   return nrpn_array.getNumEventsByFlt(_fltDev, _fltCh);
}

sUI MIDIPipeFrame::getNumEventsAllNotesOffByFlt(sSI _fltDev, sSI _fltCh) const {
   return all_notes_off_array.getNumEventsByFlt(_fltDev, _fltCh);
}

sUI MIDIPipeFrame::getNumEventsNoteOffByFlt(sSI _fltDev, sSI _fltCh) const {
   return note_off_array.getNumEventsByFlt(_fltDev, _fltCh);
}

sUI MIDIPipeFrame::getNumEventsNoteOnByFlt(sSI _fltDev, sSI _fltCh) const {
   return note_on_array.getNumEventsByFlt(_fltDev, _fltCh);
}

sUI MIDIPipeFrame::getNumEventsPolyPressureByFlt(sSI _fltDev, sSI _fltCh) const {
   return polypressure_array.getNumEventsByFlt(_fltDev, _fltCh);
}

sUI MIDIPipeFrame::getNumEventsSysExByFlt(sSI _fltDev) const {
   return sysex_array.getNumEventsByFlt(_fltDev, -1/*_fltCh,ignored*/);
}

sUI MIDIPipeFrame::getNumEventsSysRTByFlt(sSI _fltDev) const {
   return sysrt_array.getNumEventsByFlt(_fltDev, -1/*_fltCh,ignored*/);
}

sBool MIDIPipeFrame::getNumEventsNoteOnInRangeByFlt(sSI _fltDev, sSI _fltCh, sSI _minNote, sSI _maxNote) const {
   sUI num = 0u;
   if(note_on_array.num_values > 0)
   {
      sUI numLeft = note_on_array.num_values;
      sUI i;
      midipipe_frame_note_t *ev = note_on_array.values;

      for(i=0; (i < note_on_array.max_values) && (numLeft > 0); i++, ev++)
      {
         if(-1 != ev->dev_idx)
         {
            if((-1 == _fltDev) || (_fltDev == ev->dev_idx))
            {
               if((-1 == _fltCh) || (_fltCh == (sSI)ev->midi_ch))
               {
                  if((ev->note_nr >= _minNote) && (ev->note_nr <= _maxNote))
                  {
                     num++;
                  }
               }
            }
            numLeft--;
         }
      }
   }
   return num;
}

sBool MIDIPipeFrame::setEventDevByIdx(sUI _idx, sUI _devIdx) {
   sUI relIdx;
   sUI absIdx;
   sUI evType;
   midipipe_frame_u *evu;

   evu = getEventLocByIdx(_idx, &relIdx, &absIdx, &evType, -1/*fltType*/);

   if(NULL != evu)
   {
      evu->base.dev_idx = (sSI)_devIdx;

      return YAC_TRUE;
   }
   return YAC_FALSE;
}

sBool MIDIPipeFrame::setEventMidiChByIdx(sUI _idx, sUI _midiCh) {
   sUI relIdx;
   sUI absIdx;
   sUI evType;
   midipipe_frame_u *evu;

   evu = getEventLocByIdx(_idx, &relIdx, &absIdx, &evType, -1/*fltType*/);

   if(NULL != evu)
   {
      evu->base.midi_ch = _midiCh;
      return YAC_TRUE;
   }
   return YAC_FALSE;
}

sBool MIDIPipeFrame::getEventByIdxAndFlt(sUI _idx, MIDIPipeEvent *_ret, sSI _fltType, sSI _fltDev, sSI _fltCh) const {

   sUI relIdx;
   sUI absIdx;
   sUI evType;
   midipipe_frame_u *evu;

   evu = getEventLocByIdxAndFlt(_idx, &relIdx, &absIdx, &evType, _fltType, _fltDev, _fltCh, -1/*globalCh*/);

   // printf("xxx getEventByIdx: idx=%u relIdx=%u evType=%u evu=%p\n", _idx, relIdx, evType, evu);

   if(NULL != evu)
   {
      _ret->event_index = absIdx;
      _ret->event_type  = evType;

      switch(evType)
      {
         default:
         case MIDIPIPE_EVENT_TYPE_NONE:
            return YAC_FALSE;

         case MIDIPIPE_EVENT_TYPE_PRGCHG:
            _ret->data.prgchg = evu->prgchg;
            // printf("xxx MIDIPipeFrame: _ret->data.prgchg.midi_ch=%u value=%u\n", _ret->data.prgchg.midi_ch, _ret->data.prgchg.value);
            break;

         case MIDIPIPE_EVENT_TYPE_CHPRESSURE:
            _ret->data.chpressure = evu->chpressure;
            break;

         case MIDIPIPE_EVENT_TYPE_PITCHBEND:
            _ret->data.pitchbend = evu->pitchbend;
            break;

         case MIDIPIPE_EVENT_TYPE_CC:
            _ret->data.cc = evu->cc;
            break;

         case MIDIPIPE_EVENT_TYPE_RPN:
            _ret->data.rpn = evu->rpn;
            break;

         case MIDIPIPE_EVENT_TYPE_NRPN:
            _ret->data.nrpn = evu->nrpn;
            break;

         case MIDIPIPE_EVENT_TYPE_ALL_NOTES_OFF:
            _ret->data.all_notes_off = evu->all_notes_off;
            break;

         case MIDIPIPE_EVENT_TYPE_NOTE_OFF:
            _ret->data.note = evu->note;
            break;

         case MIDIPIPE_EVENT_TYPE_NOTE_ON:
            _ret->data.note = evu->note;
            break;

         case MIDIPIPE_EVENT_TYPE_POLYPRESSURE:
            _ret->data.polypressure = evu->polypressure;
            break;

         case MIDIPIPE_EVENT_TYPE_SYSEX:
            _ret->data.sysex = evu->sysex; // Return reference to buffer
            // // _ret->data.sysex.buffer = YAC_New_Buffer();
            // // _ret->data.sysex.buffer->yacCopy(evu->sysex.buffer);
            // // // _ret->data.sysex.buffer = evu->sysex.buffer;
            break;

         case MIDIPIPE_EVENT_TYPE_SYSRT:
            _ret->data.sysrt = evu->sysrt;
            break;
      }

      return YAC_TRUE;
   }

   return YAC_FALSE;
}

sBool MIDIPipeFrame::getEventByIdx(sUI _idx, MIDIPipeEvent *_ret, sSI _fltType) const {
   return getEventByIdxAndFlt(_idx, _ret, _fltType, -1/*fltDev*/, -1/*fltCh*/);
}

sBool MIDIPipeFrame::getCCEventByIdxAndFlt(sUI _idx, MIDIPipeEvent *_ret, sUI _cc, sSI _fltDev, sSI _fltCh) {
   sUI matchIdx = 0u;
   sUI numLeft = cc_array.num_values;
   midipipe_frame_ctl_t *ev = cc_array.values;

   for(sUI i=0; (i<cc_array.max_values) && (numLeft > 0); i++, ev++)
   {
      if(-1 != ev->dev_idx)
      {
         if( (-1 == _fltDev) || (_fltDev == ev->dev_idx) )
         {
            if( (-1 == _fltCh) || (_fltCh == sSI(ev->midi_ch)) )
            {
               if(ev->ctl_id == _cc)
               {
                  if(matchIdx == _idx)
                  {
                     _ret->event_index = i;
                     _ret->event_type  = MIDIPIPE_EVENT_TYPE_CC;
                     _ret->data.cc = *ev;
                     return YAC_TRUE;
                  }
                  else
                  {
                     matchIdx++;
                  }
               }
            }
         }
      }
   }
   return YAC_FALSE;
}

sBool MIDIPipeFrame::getRPNEventByIdxAndFlt(sUI _idx, MIDIPipeEvent *_ret, sUI _nr, sSI _fltDev, sSI _fltCh) {
   sUI matchIdx = 0u;
   sUI numLeft = rpn_array.num_values;
   midipipe_frame_ctl_t *ev = rpn_array.values;

   for(sUI i=0; (i<rpn_array.max_values) && (numLeft > 0); i++, ev++)
   {
      if(-1 != ev->dev_idx)
      {
         if( (-1 == _fltDev) || (_fltDev == ev->dev_idx) )
         {
            if( (-1 == _fltCh) || (_fltCh == sSI(ev->midi_ch)) )
            {
               if(ev->ctl_id == _nr)
               {
                  if(matchIdx == _idx)
                  {
                     _ret->event_index = i;
                     _ret->event_type  = MIDIPIPE_EVENT_TYPE_RPN;
                     _ret->data.rpn = *ev;
                     // yac_host->printf("xxx getRPNeventByIdxAndFlt: devIdx=%d midiCh=%d ctl_id=%d\n", ev->dev_idx, ev->midi_ch, ev->ctl_id);
                     return YAC_TRUE;
                  }
                  else
                  {
                     matchIdx++;
                  }
               }
            }
         }
         numLeft--;
      }
   }
   return YAC_FALSE;
}

sBool MIDIPipeFrame::getNRPNEventByIdxAndFlt(sUI _idx, MIDIPipeEvent *_ret, sUI _nr, sSI _fltDev, sSI _fltCh) {
   sUI matchIdx = 0u;
   sUI numLeft = nrpn_array.num_values;
   midipipe_frame_ctl_t *ev = nrpn_array.values;

   for(sUI i=0; (i<nrpn_array.max_values) && (numLeft > 0); i++, ev++)
   {
      if(-1 != ev->dev_idx)
      {
         if( (-1 == _fltDev) || (_fltDev == ev->dev_idx) )
         {
            if( (-1 == _fltCh) || (_fltCh == sSI(ev->midi_ch)) )
            {
               if(ev->ctl_id == _nr)
               {
                  if(matchIdx == _idx)
                  {
                     _ret->event_index = i;
                     _ret->event_type  = MIDIPIPE_EVENT_TYPE_NRPN;
                     _ret->data.nrpn = *ev;
                     return YAC_TRUE;
                  }
                  else
                  {
                     matchIdx++;
                  }
               }
            }
         }
         numLeft--;
      }
   }
   return YAC_FALSE;
}

sBool MIDIPipeFrame::getNoteOnEventByFlt(MIDIPipeEvent *_ret, sSI _noteNr, sSI _fltDev, sSI _fltCh) {
   if(note_on_array.num_values > 0)
   {
      sUI numLeft = note_on_array.num_values;
      sUI i;
      const midipipe_frame_note_t *ev = note_on_array.values;

      for(i=0; (i < note_on_array.max_values) && (numLeft > 0); i++, ev++)
      {
         if(-1 != ev->dev_idx)
         {
            if(ev->note_nr == _noteNr)
            {
               if( (-1 == _fltDev) || (_fltDev == ev->dev_idx) )
               {
                  if( (-1 == _fltCh) || (_fltCh == (sSI)ev->midi_ch) )
                  {
                     _ret->event_index = i;
                     _ret->event_type  = MIDIPIPE_EVENT_TYPE_NOTE_ON;
                     _ret->data.note = *ev;
                     return YAC_TRUE;
                  }
               }
            }

            numLeft--;
         }
      }
   }
   return YAC_FALSE;
}

sBool MIDIPipeFrame::getNoteOffEventByFlt(MIDIPipeEvent *_ret, sSI _noteNr, sSI _fltDev, sSI _fltCh) {
   if(note_off_array.num_values > 0)
   {
      sUI numLeft = note_off_array.num_values;
      sUI i;
      const midipipe_frame_note_t *ev = note_off_array.values;

      for(i=0; (i < note_off_array.max_values) && (numLeft > 0); i++, ev++)
      {
         if(-1 != ev->dev_idx)
         {
            if(ev->note_nr == _noteNr)
            {
               if( (-1 == _fltDev) || (_fltDev == ev->dev_idx) )
               {
                  if( (-1 == _fltCh) || (_fltCh == (sSI)ev->midi_ch) )
                  {
                     _ret->event_index = i;
                     _ret->event_type  = MIDIPIPE_EVENT_TYPE_NOTE_OFF;
                     _ret->data.note = *ev;
                     return YAC_TRUE;
                  }
               }
            }

            numLeft--;
         }
      }
   }
   return YAC_FALSE;
}

sSI MIDIPipeFrame::getFilteredPrgChg(sSI _fltDev, sSI _fltCh) {
   midipipe_frame_val_t *ev = prgchg_array.filter(_fltDev, _fltCh);

   if(NULL != ev)
   {
      return (sSI)ev->value;
   }
   else
   {
      return -1;
   }
}

sF32 MIDIPipeFrame::getFilteredCC(sSI _fltDev, sSI _fltCh, sUI _cc) {
   midipipe_frame_ctl_t *ev = cc_array.filter(_fltDev, _fltCh, _cc);

   if(NULL != ev)
   {
      return ev->value;
   }
   else
   {
      return -1.0f;
   }
}

sF32 MIDIPipeFrame::getFilteredRPN(sSI _fltDev, sSI _fltCh, sUI _rpn) {
   midipipe_frame_ctl_t *ev = rpn_array.filter(_fltDev, _fltCh, _rpn);

   if(NULL != ev)
   {
      return ev->value;
   }
   else
   {
      return -1.0f;
   }
}

sF32 MIDIPipeFrame::getFilteredNRPN(sSI _fltDev, sSI _fltCh, sUI _nrpn) {
   midipipe_frame_ctl_t *ev = nrpn_array.filter(_fltDev, _fltCh, _nrpn);

   if(NULL != ev)
   {
      return ev->value;
   }
   else
   {
      return -1.0f;
   }
}

sF32 MIDIPipeFrame::getFilteredPitchbend(sSI _fltDev, sSI _fltCh) {
   midipipe_frame_val_t *ev = pitchbend_array.filter(_fltDev, _fltCh);

   if(NULL != ev)
   {
      return ev->value;
   }
   else
   {
      return -1.0f;
   }
}

sF32 MIDIPipeFrame::getFilteredChPressure(sSI _fltDev, sSI _fltCh) {
   midipipe_frame_val_t *ev = chpressure_array.filter(_fltDev, _fltCh);

   if(NULL != ev)
   {
      return ev->value;
   }
   else
   {
      return -1;
   }
}

sF32 MIDIPipeFrame::getFilteredNoteOnVelocity(sSI _fltDev, sSI _fltCh, sSI _noteNr) {
   sUI numLeft = note_on_array.num_values;
   midipipe_frame_note_t *ev = note_on_array.values;

   for(sUI i = 0u; (i < note_on_array.max_values) && (numLeft > 0u); i++, ev++)
   {
      if(-1 != ev->dev_idx)
      {
         if( (-1 == _fltDev) || (_fltDev == ev->dev_idx) )
         {
            if( (-1 == _fltCh) || (_fltCh == sSI(ev->midi_ch)) )
            {
               if(ev->note_nr == _noteNr)
               {
                  return ev->velocity;
               }
            }
         }
         numLeft--;
      }
   }
   return -1.0f;
}

sF32 MIDIPipeFrame::getFilteredNoteOffVelocity(sSI _fltDev, sSI _fltCh, sSI _noteNr) {
   sUI numLeft = note_off_array.num_values;
   midipipe_frame_note_t *ev = note_off_array.values;

   for(sUI i = 0u; (i < note_off_array.max_values) && (numLeft > 0u); i++, ev++)
   {
      if(-1 != ev->dev_idx)
      {
         if( (-1 == _fltDev) || (_fltDev == ev->dev_idx) )
         {
            if( (-1 == _fltCh) || (_fltCh == sSI(ev->midi_ch)) )
            {
               if(ev->note_nr == _noteNr)
               {
                  return ev->velocity;
               }
            }
         }
         numLeft--;
      }
   }
   return -1.0f;
}

sF32 MIDIPipeFrame::getNormalizedNoteOnVelocityByIdxAndFlt(sUI _idx, sSI _fltDev, sSI _fltCh) {
   sUI numLeft = note_on_array.num_values;
   midipipe_frame_note_t *ev = note_on_array.values;
   sUI evIdx = 0u;

   for(sUI i=0; (i<note_on_array.max_values) && (numLeft > 0); i++, ev++)
   {
      if(-1 != ev->dev_idx)
      {
         if( (-1 == _fltDev) || (_fltDev == ev->dev_idx) )
         {
            if( (-1 == _fltCh) || (_fltCh == sSI(ev->midi_ch)) )
            {
               if(evIdx == _idx)
               {
                  return (ev->velocity / 127.0f);
               }
               else
               {
                  evIdx++;
               }
            }
         }
         numLeft--;
      }
   }
   return -1.0f;
}

sF32 MIDIPipeFrame::getNormalizedNoteOffVelocityByIdxAndFlt(sUI _idx, sSI _fltDev, sSI _fltCh) {
   sUI numLeft = note_off_array.num_values;
   midipipe_frame_note_t *ev = note_off_array.values;
   sUI evIdx = 0u;

   for(sUI i=0; (i<note_off_array.max_values) && (numLeft > 0); i++, ev++)
   {
      if(-1 != ev->dev_idx)
      {
         if( (-1 == _fltDev) || (_fltDev == ev->dev_idx) )
         {
            if( (-1 == _fltCh) || (_fltCh == sSI(ev->midi_ch)) )
            {
               if(evIdx == _idx)
               {
                  return (ev->velocity / 127.0f);
               }
               else
               {
                  evIdx++;
               }
            }
         }
         numLeft--;
      }
   }
   return -1.0f;
}

sBool MIDIPipeFrame::setEventArg1ByIdx(sUI _idx, sF32 _val) {
   sUI relIdx;
   sUI absIdx;
   sUI evType;
   midipipe_frame_u *evu;

   evu = getEventLocByIdx(_idx, &relIdx, &absIdx, &evType, -1/*fltType*/);

   if(NULL != evu)
   {
      switch(evType)
      {
         default:
         case MIDIPIPE_EVENT_TYPE_NONE:
            return YAC_FALSE;

         case MIDIPIPE_EVENT_TYPE_PRGCHG:
            evu->prgchg.value = _val;
            break;

         case MIDIPIPE_EVENT_TYPE_CHPRESSURE:
            evu->chpressure.value = _val;
            break;

         case MIDIPIPE_EVENT_TYPE_PITCHBEND:
            evu->pitchbend.value = _val;
            break;

         case MIDIPIPE_EVENT_TYPE_CC:
            evu->cc.ctl_id = sUI((_val < 0.0f) ? 0.0f : (_val > 127.0f) ? 127.0f : _val);
            break;

         case MIDIPIPE_EVENT_TYPE_RPN:
            evu->rpn.ctl_id = sUI(_val) & 16383u;
            break;

         case MIDIPIPE_EVENT_TYPE_NRPN:
            evu->nrpn.ctl_id = sUI(_val) & 16383u;
            break;

         case MIDIPIPE_EVENT_TYPE_NOTE_OFF:
            evu->note.note_nr = sUI(_val);
            break;

         case MIDIPIPE_EVENT_TYPE_NOTE_ON:
            evu->note.note_nr = sUI(_val);
            break;

         case MIDIPIPE_EVENT_TYPE_POLYPRESSURE:
            evu->polypressure.note_nr = sUI(_val);
            break;
      }

      return YAC_TRUE;
   }

   return YAC_FALSE;
}

sF32 MIDIPipeFrame::getEventArg1ByIdx(sUI _idx) {
   sUI relIdx;
   sUI absIdx;
   sUI evType;
   midipipe_frame_u *evu;
   sF32 r = -1.0f;

   evu = getEventLocByIdx(_idx, &relIdx, &absIdx, &evType, -1/*fltType*/);

   if(NULL != evu)
   {
      switch(evType)
      {
         default:
         case MIDIPIPE_EVENT_TYPE_NONE:
            break;

         case MIDIPIPE_EVENT_TYPE_PRGCHG:
            r = evu->prgchg.value;
            break;

         case MIDIPIPE_EVENT_TYPE_CHPRESSURE:
            r = evu->chpressure.value;
            break;

         case MIDIPIPE_EVENT_TYPE_PITCHBEND:
            r = evu->pitchbend.value;
            break;

         case MIDIPIPE_EVENT_TYPE_CC:
            r = (sF32) evu->cc.ctl_id;
            break;

         case MIDIPIPE_EVENT_TYPE_RPN:
            r = (sF32) evu->rpn.ctl_id;
            break;

         case MIDIPIPE_EVENT_TYPE_NRPN:
            r = (sF32) evu->nrpn.ctl_id;
            break;

         case MIDIPIPE_EVENT_TYPE_NOTE_OFF:
            r = (sF32) evu->note.note_nr;
            break;

         case MIDIPIPE_EVENT_TYPE_NOTE_ON:
            r = (sF32) evu->note.note_nr;
            break;

         case MIDIPIPE_EVENT_TYPE_POLYPRESSURE:
            r = (sF32) evu->polypressure.note_nr;
            break;
      }
   }

   return r;
}

sBool MIDIPipeFrame::setEventArg2ByIdx(sUI _idx, sF32 _val) {
   sUI relIdx;
   sUI absIdx;
   sUI evType;
   midipipe_frame_u *evu;

   evu = getEventLocByIdx(_idx, &relIdx, &absIdx, &evType, -1/*fltType*/);

   if(NULL != evu)
   {
      switch(evType)
      {
         default:
         case MIDIPIPE_EVENT_TYPE_NONE:
            return YAC_FALSE;

         case MIDIPIPE_EVENT_TYPE_PRGCHG:
            return YAC_FALSE;

         case MIDIPIPE_EVENT_TYPE_CHPRESSURE:
            return YAC_FALSE;

         case MIDIPIPE_EVENT_TYPE_PITCHBEND:
            return YAC_FALSE;

         case MIDIPIPE_EVENT_TYPE_CC:
            evu->cc.value = _val;
            break;

         case MIDIPIPE_EVENT_TYPE_RPN:
            evu->rpn.value = _val;
            break;

         case MIDIPIPE_EVENT_TYPE_NRPN:
            evu->nrpn.value = _val;
            break;

         case MIDIPIPE_EVENT_TYPE_NOTE_OFF:
            evu->note.velocity = _val;
            break;

         case MIDIPIPE_EVENT_TYPE_NOTE_ON:
            evu->note.velocity = _val;
            break;

         case MIDIPIPE_EVENT_TYPE_POLYPRESSURE:
            evu->polypressure.value = _val;
            break;
      }

      return YAC_TRUE;
   }

   return YAC_FALSE;
}

sF32 MIDIPipeFrame::getEventArg2ByIdx(sUI _idx) {
   sUI relIdx;
   sUI absIdx;
   sUI evType;
   midipipe_frame_u *evu;
   sF32 r = -1.0f;

   evu = getEventLocByIdx(_idx, &relIdx, &absIdx, &evType, -1/*fltType*/);

   if(NULL != evu)
   {
      switch(evType)
      {
         default:
         case MIDIPIPE_EVENT_TYPE_NONE:
         case MIDIPIPE_EVENT_TYPE_PRGCHG:
         case MIDIPIPE_EVENT_TYPE_CHPRESSURE:
         case MIDIPIPE_EVENT_TYPE_PITCHBEND:
            break;

         case MIDIPIPE_EVENT_TYPE_CC:
            r = evu->cc.value;
            break;

         case MIDIPIPE_EVENT_TYPE_RPN:
            r = evu->rpn.value;
            break;

         case MIDIPIPE_EVENT_TYPE_NRPN:
            r = evu->nrpn.value;
            break;

         case MIDIPIPE_EVENT_TYPE_NOTE_OFF:
            r = evu->note.velocity;
            break;

         case MIDIPIPE_EVENT_TYPE_NOTE_ON:
            r = evu->note.velocity;
            break;

         case MIDIPIPE_EVENT_TYPE_POLYPRESSURE:
            r = evu->polypressure.value;
            break;
      }

   }

   return r;
}

sBool MIDIPipeFrame::setEventArg3ByIdx(sUI _idx, sSI _val) {
   sUI relIdx;
   sUI absIdx;
   sUI evType;
   midipipe_frame_u *evu;

   evu = getEventLocByIdx(_idx, &relIdx, &absIdx, &evType, -1/*fltType*/);

   if(NULL != evu)
   {
      switch(evType)
      {
         default:
         case MIDIPIPE_EVENT_TYPE_NONE:
            return YAC_FALSE;

         case MIDIPIPE_EVENT_TYPE_PRGCHG:
            return YAC_FALSE;

         case MIDIPIPE_EVENT_TYPE_CHPRESSURE:
            return YAC_FALSE;

         case MIDIPIPE_EVENT_TYPE_PITCHBEND:
            return YAC_FALSE;

         case MIDIPIPE_EVENT_TYPE_CC:
            return YAC_FALSE;

         case MIDIPIPE_EVENT_TYPE_RPN:
            return YAC_FALSE;

         case MIDIPIPE_EVENT_TYPE_NRPN:
            return YAC_FALSE;

         case MIDIPIPE_EVENT_TYPE_NOTE_OFF:
            evu->note.duration = _val;
            break;

         case MIDIPIPE_EVENT_TYPE_NOTE_ON:
            evu->note.duration = _val;
            break;

         case MIDIPIPE_EVENT_TYPE_POLYPRESSURE:
            return YAC_FALSE;
      }

      return YAC_TRUE;
   }

   return YAC_FALSE;
}

sSI MIDIPipeFrame::getEventArg3ByIdx(sUI _idx) {
   sUI relIdx;
   sUI absIdx;
   sUI evType;
   midipipe_frame_u *evu;
   sSI r = -1;

   evu = getEventLocByIdx(_idx, &relIdx, &absIdx, &evType, -1/*fltType*/);

   if(NULL != evu)
   {
      switch(evType)
      {
         default:
         case MIDIPIPE_EVENT_TYPE_NONE:
         case MIDIPIPE_EVENT_TYPE_PRGCHG:
         case MIDIPIPE_EVENT_TYPE_CHPRESSURE:
         case MIDIPIPE_EVENT_TYPE_PITCHBEND:
         case MIDIPIPE_EVENT_TYPE_CC:
         case MIDIPIPE_EVENT_TYPE_RPN:
         case MIDIPIPE_EVENT_TYPE_NRPN:
            break;

         case MIDIPIPE_EVENT_TYPE_NOTE_OFF:
            r = (sSI) evu->note.duration;
            break;

         case MIDIPIPE_EVENT_TYPE_NOTE_ON:
            r = (sSI) evu->note.duration;
            break;

         case MIDIPIPE_EVENT_TYPE_POLYPRESSURE:
            break;
      }

   }

   return r;
}

sBool MIDIPipeFrame::deleteEventByIdx(sUI _idx, sSI _fltType) {
   sUI relIdx;
   sUI absIdx;
   sUI evType;
   midipipe_frame_u *evu;

   evu = getEventLocByIdx(_idx, &relIdx, &absIdx, &evType, _fltType);

   if(NULL != evu)
   {
      switch(evType)
      {
         default:
         case MIDIPIPE_EVENT_TYPE_NONE:
            return YAC_FALSE;

         case MIDIPIPE_EVENT_TYPE_PRGCHG:
            prgchg_array.num_values--;
            break;

         case MIDIPIPE_EVENT_TYPE_CHPRESSURE:
            chpressure_array.num_values--;
            break;

         case MIDIPIPE_EVENT_TYPE_PITCHBEND:
            pitchbend_array.num_values--;
            break;

         case MIDIPIPE_EVENT_TYPE_CC:
            cc_array.num_values--;
            break;

         case MIDIPIPE_EVENT_TYPE_RPN:
            rpn_array.num_values--;
            break;

         case MIDIPIPE_EVENT_TYPE_NRPN:
            nrpn_array.num_values--;
            break;

         case MIDIPIPE_EVENT_TYPE_ALL_NOTES_OFF:
            all_notes_off_array.num_values--;
            break;

         case MIDIPIPE_EVENT_TYPE_NOTE_OFF:
            note_off_array.num_values--;
            break;

         case MIDIPIPE_EVENT_TYPE_NOTE_ON:
            note_on_array.num_values--;
            break;

         case MIDIPIPE_EVENT_TYPE_POLYPRESSURE:
            polypressure_array.num_values--;
            break;

         case MIDIPIPE_EVENT_TYPE_SYSEX:
            sysex_array.num_values--;
            break;

         case MIDIPIPE_EVENT_TYPE_SYSRT:
            sysrt_array.num_values--;
            break;
      }

      evu->base.dev_idx = -1;

      return YAC_TRUE;
   }

   return YAC_FALSE;
}

sUI MIDIPipeFrame::deletePrgChgByFlt(sSI _fltDev, sSI _fltCh) {
   return prgchg_array.deleteByFlt(_fltDev, _fltCh);
}

sUI MIDIPipeFrame::deleteChPressureByFlt(sSI _fltDev, sSI _fltCh) {
   return chpressure_array.deleteByFlt(_fltDev, _fltCh);
}

sUI MIDIPipeFrame::deletePitchbendByFlt(sSI _fltDev, sSI _fltCh) {
   return pitchbend_array.deleteByFlt(_fltDev, _fltCh);
}

sUI MIDIPipeFrame::deleteCCByFlt(sSI _fltDev, sSI _fltCh, sSI _cc) {
   if(-1 == _cc)
   {
      return cc_array.deleteByFlt(_fltDev, _fltCh);
   }
   else
   {
      return cc_array.deleteByFltAndNr(_fltDev, _fltCh, _cc);
   }
}

sUI MIDIPipeFrame::deleteRPNByFlt(sSI _fltDev, sSI _fltCh, sSI _rpn) {
   if(-1 == _rpn)
   {
      return rpn_array.deleteByFlt(_fltDev, _fltCh);
   }
   else
   {
      return rpn_array.deleteByFltAndNr(_fltDev, _fltCh, _rpn);
   }
}

sUI MIDIPipeFrame::deleteNRPNByFlt(sSI _fltDev, sSI _fltCh, sSI _nrpn) {
   if(-1 == _nrpn)
   {
      return nrpn_array.deleteByFlt(_fltDev, _fltCh);
   }
   else
   {
      return nrpn_array.deleteByFltAndNr(_fltDev, _fltCh, _nrpn);
   }
}

sUI MIDIPipeFrame::deletePolyPressureByFlt(sSI _fltDev, sSI _fltCh) {
   return polypressure_array.deleteByFlt(_fltDev, _fltCh);
}

sUI MIDIPipeFrame::deleteSysExByFlt(sSI _fltDev) {
   return sysex_array.deleteByFlt(_fltDev, -1/*ch,ignored*/);
}

sUI MIDIPipeFrame::deleteSysRTByFlt(sSI _fltDev) {
   return sysrt_array.deleteByFlt(_fltDev, -1/*ch,ignored*/);
}

void MIDIPipeFrame::deleteNoteOffsByFlt(sSI _fltDevIdx, sSI _fltMidiCh) {
   if(note_off_array.num_values > 0)
   {
      sUI numLeft = note_off_array.num_values;
      sUI i;
      midipipe_frame_note_t *ev = note_off_array.values;

      for(i=0; (i < note_off_array.max_values) && (numLeft > 0); i++, ev++)
      {
         if(-1 != ev->dev_idx)
         {
            sBool bDel = YAC_TRUE;

            if(-1 != _fltDevIdx)
            {
               bDel = bDel && (_fltDevIdx == ev->dev_idx);
            }

            if(-1 != _fltMidiCh)
            {
               bDel = bDel && (_fltMidiCh == sSI(ev->midi_ch));
            }

            if(bDel)
            {
               ev->dev_idx = -1;

               note_off_array.num_values--;
            }

            numLeft--;
         }
      }

   }

   if(all_notes_off_array.num_values > 0)
   {
      sUI numLeft = all_notes_off_array.num_values;
      sUI i;
      midipipe_frame_base_t *ev = all_notes_off_array.values;

      for(i=0; (i < all_notes_off_array.max_values) && (numLeft > 0); i++, ev++)
      {
         if(-1 != ev->dev_idx)
         {
            sBool bDel = YAC_TRUE;

            if(-1 != _fltDevIdx)
            {
               bDel = bDel && (_fltDevIdx == ev->dev_idx);
            }

            if(-1 != _fltMidiCh)
            {
               bDel = bDel && (_fltMidiCh == sSI(ev->midi_ch));
            }

            if(bDel)
            {
               ev->dev_idx = -1;

               all_notes_off_array.num_values--;
            }

            numLeft--;
         }
      }
   }
}

void MIDIPipeFrame::deleteNoteOnsByFlt(sSI _fltDevIdx, sSI _fltMidiCh) {
   if(note_on_array.num_values > 0)
   {
      sUI numLeft = note_on_array.num_values;
      sUI i;
      midipipe_frame_note_t *ev = note_on_array.values;

      for(i=0; (i < note_on_array.max_values) && (numLeft > 0); i++, ev++)
      {
         if(-1 != ev->dev_idx)
         {
            sBool bDel = YAC_TRUE;

            if(-1 != _fltDevIdx)
            {
               bDel = bDel && (_fltDevIdx == ev->dev_idx);
            }

            if(-1 != _fltMidiCh)
            {
               bDel = bDel && (_fltMidiCh == sSI(ev->midi_ch));
            }

            if(bDel)
            {
               ev->dev_idx = -1;

               note_on_array.num_values--;
            }

            numLeft--;
         }
      }
   }
}

void MIDIPipeFrame::keepEventsByFlt(sSI _fltDev, sSI _fltCh) {
   prgchg_array       .keepByFlt(_fltDev, _fltCh);
   chpressure_array   .keepByFlt(_fltDev, _fltCh);
   pitchbend_array    .keepByFlt(_fltDev, _fltCh);
   cc_array           .keepByFlt(_fltDev, _fltCh);
   rpn_array          .keepByFlt(_fltDev, _fltCh);
   nrpn_array         .keepByFlt(_fltDev, _fltCh);
   all_notes_off_array.keepByFlt(_fltDev, _fltCh);
   note_off_array     .keepByFlt(_fltDev, _fltCh);
   note_on_array      .keepByFlt(_fltDev, _fltCh);
   polypressure_array .keepByFlt(_fltDev, _fltCh);
}

void MIDIPipeFrame::deleteEventsByFlt(sSI _fltDev, sSI _fltCh) {
   prgchg_array       .deleteByFlt(_fltDev, _fltCh);
   chpressure_array   .deleteByFlt(_fltDev, _fltCh);
   pitchbend_array    .deleteByFlt(_fltDev, _fltCh);
   cc_array           .deleteByFlt(_fltDev, _fltCh);
   rpn_array          .deleteByFlt(_fltDev, _fltCh);
   nrpn_array         .deleteByFlt(_fltDev, _fltCh);
   all_notes_off_array.deleteByFlt(_fltDev, _fltCh);
   note_off_array     .deleteByFlt(_fltDev, _fltCh);
   note_on_array      .deleteByFlt(_fltDev, _fltCh);
   polypressure_array .deleteByFlt(_fltDev, _fltCh);
}

void MIDIPipeFrame::mirrorNoteOnOffs(MIDIPipeFrame *_dst, sSI _srcDev, sSI _srcCh, sSI _dstDev, sSI _dstCh, sSI _transp, sUI _velMode, sBool _bFixedVel) {
   if(YAC_VALID(_dst))
   {
      if(_dstDev >= 0)
      {
         if((_dstCh >= 0) && (_dstCh < 16))
         {

            if(note_on_array.num_values > 0)
            {
               sUI numLeft = note_on_array.num_values;
               sUI i;
               midipipe_frame_note_t *ev = note_on_array.values;

               for(i=0; (i < note_on_array.max_values) && (numLeft > 0); i++, ev++)
               {
                  if(-1 != ev->dev_idx)
                  {
                     if( (_srcDev == ev->dev_idx) && (_srcCh == sSI(ev->midi_ch)) )
                     {
                        _dst->noteOn(1/*bSet*/, _dstDev, (sUI)_dstCh,
                                     ev->note_nr + _transp,
                                     _bFixedVel ? 127.0f : ev->velocity,
                                     ev->duration
                                     );

                        if(1 == _velMode)
                        {
                           _dst->chPressure(1/*bSet*/, _dstDev, (sUI)_dstCh, ev->velocity);
                        }
                        else if(2 == _velMode)
                        {
                           _dst->cc(1/*bSet*/, _dstDev, (sUI)_dstCh, 1/*modulation MSB*/, ev->velocity);
                        }
                     }

                     numLeft--;
                  }
               }
            }

            if(note_off_array.num_values > 0)
            {
               sUI numLeft = note_off_array.num_values;
               sUI i;
               midipipe_frame_note_t *ev = note_off_array.values;

               for(i=0; (i < note_off_array.max_values) && (numLeft > 0); i++, ev++)
               {
                  if(-1 != ev->dev_idx)
                  {
                     if( (_srcDev == ev->dev_idx) && (_srcCh == sSI(ev->midi_ch)) )
                     {
                        _dst->noteOff(1/*bSet*/, _dstDev, (sUI)_dstCh,
                                      ev->note_nr + _transp,
                                      _bFixedVel ? 0.0f : ev->velocity
                                      );
                     }

                     numLeft--;
                  }
               }
            }

         } // dstch valid
      } // dstdev valid
   } // dst frame valid
}

sUI MIDIPipeFrame::convertRPNUIPrgChgToPrgChg(sSI _fltDev, sSI _fltCh) {
   sUI r = 0u;
   sUI numLeft = rpn_array.num_values;
   midipipe_frame_ctl_t *ev = rpn_array.values;
   for(sUI i=0; (i<rpn_array.max_values) && (numLeft > 0); i++, ev++)
   {
      if(-1 != ev->dev_idx)
      {
         if( (-1 == _fltDev) || (_fltDev == ev->dev_idx) )
         {
            if( (-1 == _fltCh) || (_fltCh == sSI(ev->midi_ch)) )
            {
               if(524/*Node.RPN_COMMON_UI_PROGRAM_CHANGE*/ == ev->ctl_id)
               {
                  prgChg(true/*bSet*/, ev->dev_idx, ev->midi_ch, sU8(ev->value));
                  r++;
               }
            }
         }
         numLeft--;
      }
   }

   deleteRPNByFlt(_fltDev, _fltCh, 524/*Node.RPN_COMMON_UI_PROGRAM_CHANGE*/);
   return r;
}

void MIDIPipeFrame::setNoteDurations(sUI _numTicks) {
   if(note_on_array.num_values > 0)
   {
      sUI numLeft = note_on_array.num_values;
      sUI i;
      midipipe_frame_note_t *ev = note_on_array.values;

      for(i=0; (i < note_on_array.max_values) && (numLeft > 0); i++, ev++)
      {
         ev->duration = _numTicks;

         if(-1 != ev->dev_idx)
         {
            numLeft--;
         }
      }
   }
}

void MIDIPipeFrame::setMinNoteDurations(sUI _numTicks) {
   if(note_on_array.num_values > 0)
   {
      sUI numLeft = note_on_array.num_values;
      sUI i;
      midipipe_frame_note_t *ev = note_on_array.values;

      for(i=0; (i < note_on_array.max_values) && (numLeft > 0); i++, ev++)
      {
         if(ev->duration < _numTicks)
            ev->duration = _numTicks;

         if(-1 != ev->dev_idx)
         {
            numLeft--;
         }
      }
   }
}

void MIDIPipeFrame::updateNoteDuration(sSI _fltDevIdx, sSI _fltMidiCh, sSI _fltNote, sUI _numTicks) {
   if(note_on_array.num_values > 0)
   {
      sUI numLeft = note_on_array.num_values;
      sUI i;
      midipipe_frame_note_t *ev = note_on_array.values;

      for(i=0; (i < note_on_array.max_values) && (numLeft > 0); i++, ev++)
      {
         if(-1 != ev->dev_idx)
         {
            if( (-1 == _fltDevIdx) || (ev->dev_idx == _fltDevIdx) )
            {
               if( (-1 == _fltMidiCh) || (sSI(ev->midi_ch) == _fltMidiCh) )
               {
                  if((-1 == _fltNote) || (ev->note_nr == sSI(_fltNote)))
                  {
                     ev->duration = _numTicks;
                  }
               }
            }

            numLeft--;
         }
      }
   }
}

void MIDIPipeFrame::scaleNoteDurations(sSI _fltDevIdx, sSI _fltMidiCh, sF32 _scl, sF32 _off) {
   if(note_on_array.num_values > 0)
   {
      sUI numLeft = note_on_array.num_values;
      sUI i;
      midipipe_frame_note_t *ev = note_on_array.values;

      for(i=0; (i < note_on_array.max_values) && (numLeft > 0); i++, ev++)
      {
         if(-1 != ev->dev_idx)
         {
            if( (-1 == _fltDevIdx) || (ev->dev_idx == _fltDevIdx) )
            {
               if( (-1 == _fltMidiCh) || (sSI(ev->midi_ch) == _fltMidiCh) )
               {
                  if(ev->duration > 0)
                  {
                     sF32 dur = (ev->duration * _scl + _off);
                     if(dur < 1.0f)
                        dur = 1.0f;
                     ev->duration = (sUI) dur;
                  }
               }
            }

            numLeft--;
         }
      }
   }
}

sF32 MIDIPipeFrame::getFirstNoteOnVelocity(sSI _fltDevIdx, sSI _fltMidiCh) {
   if(note_on_array.num_values > 0)
   {
      sUI numLeft = note_on_array.num_values;
      sUI i;
      midipipe_frame_note_t *ev = note_on_array.values;

      for(i=0; (i < note_on_array.max_values) && (numLeft > 0); i++, ev++)
      {
         if(-1 != ev->dev_idx)
         {
            if( (-1 == _fltDevIdx) || (ev->dev_idx == _fltDevIdx) )
            {
               if( (-1 == _fltMidiCh) || (sSI(ev->midi_ch) == _fltMidiCh) )
               {
                  return ev->velocity;
               }
            }

            numLeft--;
         }
      }
   }
   return -1.0f;
}

void MIDIPipeFrame::updateNoteOnVelocities(sSI _fltDevIdx, sSI _fltMidiCh, sF32 _vel) {
   if(note_on_array.num_values > 0)
   {
      _vel = (_vel < 0.0f) ? 0.0f : (_vel > 127.0f) ? 127.0f : _vel;

      sUI numLeft = note_on_array.num_values;
      sUI i;
      midipipe_frame_note_t *ev = note_on_array.values;

      for(i=0; (i < note_on_array.max_values) && (numLeft > 0); i++, ev++)
      {
         if(-1 != ev->dev_idx)
         {
            if( (-1 == _fltDevIdx) || (ev->dev_idx == _fltDevIdx) )
            {
               if( (-1 == _fltMidiCh) || (sSI(ev->midi_ch) == _fltMidiCh) )
               {
                  ev->velocity = _vel;
               }
            }

            numLeft--;
         }
      }
   }
}

void MIDIPipeFrame::scaleNoteVelocities(sSI _fltDevIdx, sSI _fltMidiCh, sF32 _scl, sF32 _off) {
   if(note_on_array.num_values > 0)
   {
      sUI numLeft = note_on_array.num_values;
      sUI i;
      midipipe_frame_note_t *ev = note_on_array.values;

      for(i=0; (i < note_on_array.max_values) && (numLeft > 0); i++, ev++)
      {
         if(-1 != ev->dev_idx)
         {
            if( (-1 == _fltDevIdx) || (ev->dev_idx == _fltDevIdx) )
            {
               if( (-1 == _fltMidiCh) || (sSI(ev->midi_ch) == _fltMidiCh) )
               {
                  sF32 vel = (ev->velocity * _scl + _off);
                  if(vel < 0.0f)
                     vel = 0.0f;
                  else if(vel > 127.0f)
                     vel = 127.0f;
                  ev->velocity = vel;
               }
            }

            numLeft--;
         }
      }
   }
}

void MIDIPipeFrame::setDevIdxAndMidiCh(sSI _devIdx, sSI _midiCh) {
   prgchg_array       .setDevIdxAndMidiCh(_devIdx, _midiCh);
   chpressure_array   .setDevIdxAndMidiCh(_devIdx, _midiCh);
   pitchbend_array    .setDevIdxAndMidiCh(_devIdx, _midiCh);
   cc_array           .setDevIdxAndMidiCh(_devIdx, _midiCh);
   rpn_array          .setDevIdxAndMidiCh(_devIdx, _midiCh);
   nrpn_array         .setDevIdxAndMidiCh(_devIdx, _midiCh);
   all_notes_off_array.setDevIdxAndMidiCh(_devIdx, _midiCh);
   note_off_array     .setDevIdxAndMidiCh(_devIdx, _midiCh);
   note_on_array      .setDevIdxAndMidiCh(_devIdx, _midiCh);
   polypressure_array .setDevIdxAndMidiCh(_devIdx, _midiCh);
   sysex_array        .setDevIdxAndMidiCh(_devIdx, _midiCh);
   sysrt_array        .setDevIdxAndMidiCh(_devIdx, _midiCh);
}

void MIDIPipeFrame::emit(void) {

#define Dctlst(v) bufCtl->yacStreamWriteI8((v) | (ev->midi_ch))
#define Dctl(v)   bufCtl->yacStreamWriteI8(sU8(v))

#define Dnotest(v) bufNote->yacStreamWriteI8((v) | (ev->midi_ch))
#define Dnote(v)   bufNote->yacStreamWriteI8((v))

#define Dsysex_data(v) bufCtl->yacStreamWriteI8((v))

#define Dgetbufs() root->getDeviceBuffersByEvent(ev, YAC_MAYBE/*bOverflow*/, &bufCtl, &bufNote, &devCh, &devChShadow, &pipeDev)
#define Dgetbufs_no_overflow() root->getDeviceBuffersByEvent(ev, YAC_FALSE/*bOverflow*/, &bufCtl, &bufNote, &devCh, &devChShadow, &pipeDev)

#define Dsysrt(v) bufCtl->yacStreamWriteI8((v))

// (note) enabling running status on macOS causes NRPN glitches (e.g. missing param updates)
#ifdef YAC_MACOS
#define Drs() (YAC_FALSE)
#else
#define Drs() (pipeDev->running_status[ev->midi_ch & 15u])
#endif // YAC_MACOS

#if MIDIPIPEDEVICE_MAX_OVERFLOW_BUFFERS > 0u
#define Devfits(SZ)       ( (pipeDev->num_bytes_written + (SZ)) <= MIDIPIPEDEVICE_OVERFLOW_BUFFER_SZ )
#define Dnextoverflowbuf  pipeDev->b_overflow = YAC_TRUE; pipeDev->selectNextOverflowBuffer(); Dgetbufs()
#define Dwritebytes(SZ)   if(pipeDev->b_use_overflow_buffers) { Dprintf("xxx this=%p  Dwritebytes(%u) curNum=%u\n", this, (SZ), pipeDev->num_bytes_written); if(Devfits(SZ)) { Dprintf("xxx    bufCtl->offset=%u\n", bufCtl->io_offset); pipeDev->num_bytes_written += (SZ); } else { Dnextoverflowbuf; pipeDev->num_bytes_written = (SZ); } } else while(0)
#else
#define Dwritebytes(SZ)   while(0)
#endif // MIDIPIPEDEVICE_MAX_OVERFLOW_BUFFERS > 0u

   if(NULL != root)
   {
      MIDIPipeDevice *pipeDev;

      // Emit next pending overflow buffer from previous emit() call(s)
      for(sUI i = 0u; i < root->max_devices; i++)
      {
         pipeDev = root->devices[i];
         if(NULL != pipeDev)
         {
#if MIDIPIPEDEVICE_MAX_OVERFLOW_BUFFERS > 0u
            if(pipeDev->b_use_overflow_buffers && pipeDev->num_used_overflow_buffers > 0u)
            {
               sUI overflowNumBytes;

               // Copy 'cmd' overflow bytes to device buffer
               {
                  YAC_Buffer *bufSrc = pipeDev->overflow_cmd_buffers[pipeDev->read_overflow_buffer_idx];
                  YAC_Buffer *bufDst = pipeDev->cmd_buffer;
                  overflowNumBytes = bufSrc->io_offset;
                  for(sUI j = 0u; j < bufSrc->io_offset; j++)
                  {
                     bufDst->yacStreamWriteI8(bufSrc->buffer[j]);
                  }
               }

               // Copy 'note' overflow bytes to device buffer
               {
                  YAC_Buffer *bufSrc = pipeDev->overflow_note_buffers[pipeDev->read_overflow_buffer_idx];
                  YAC_Buffer *bufDst = pipeDev->note_buffer;
                  overflowNumBytes = sMAX(overflowNumBytes, bufSrc->io_offset);
                  for(sUI j = 0u; j < bufSrc->io_offset; j++)
                  {
                     bufDst->yacStreamWriteI8(bufSrc->buffer[j]);
                  }
               }

               // Cycle read index
               pipeDev->read_overflow_buffer_idx =
                  (pipeDev->read_overflow_buffer_idx + 1u) % MIDIPIPEDEVICE_MAX_OVERFLOW_BUFFERS;

               pipeDev->b_overflow = (0u != --pipeDev->num_used_overflow_buffers);

               pipeDev->num_bytes_written = overflowNumBytes;

               Dprintf("[>>>] MIDIPipeFrame::emit: new read_overflow_buffer_idx=%u, wrote %u overflow bytes. b_overflow=%d\n", pipeDev->read_overflow_buffer_idx, overflowNumBytes, pipeDev->b_overflow);
            }
            else
            {
               pipeDev->num_bytes_written = 0u;
               pipeDev->b_overflow = YAC_FALSE;
            }
#else
            pipeDev->num_bytes_written = 0u;
            pipeDev->b_overflow = YAC_FALSE;
#endif // MIDIPIPEDEVICE_MAX_OVERFLOW_BUFFERS > 0u
         } // if pipeDev
      } // loop devices

      if(getNumEvents() > 0u)
      {
         sUI numLeft;
         sUI i;
         YAC_Buffer *bufCtl;
         YAC_Buffer *bufNote;
         midipipe_device_ch_t *devCh;
         midipipe_device_ch_t *devChShadow;
         pipeDev = NULL;

         // System Realtime Events
         numLeft = sysrt_array.num_values;
         if(numLeft > 0u)
         {
            midipipe_frame_sysrt_t *ev = sysrt_array.values;

            for(i = 0u; (i < sysrt_array.max_values) && (numLeft > 0u); i++, ev++)
            {
               if(-1 != ev->dev_idx)
               {
                  if(!ev->b_dont_send)
                  {
                     if(Dgetbufs_no_overflow())
                     {
                        Dsysrt(ev->sysrt_byte);
                     }
                  }

                  numLeft--;
               }
            }
         }

         // SysEx
         numLeft = sysex_array.num_values;
         if(numLeft > 0u)
         {
            // yac_host->printf("xxx MIDIPipeFrame::emit: numSysEx=%u\n", numLeft);

            midipipe_frame_sysex_t *ev = sysex_array.values;

            for(i = 0u; (i < sysex_array.max_values) && (numLeft > 0u); i++, ev++)
            {
               if(-1 != ev->dev_idx)
               {
                  if(!ev->b_dont_send)
                  {
                     if(NULL != ev->buffer)
                     {
                        // yac_host->printf("[trc] MIDIPipeFrame::emit: this=%p send sysex sz=%u dev_idx=%d b_dont_send=%d\n", this, ev->buffer->io_offset, ev->dev_idx, ev->b_dont_send);

                        if(Dgetbufs_no_overflow())
                        {
                           // yac_host->printf("[trc] MIDIPipeFrame::emit: bufCtl start=%u\n", bufCtl->io_offset);

                           for(sUI datIdx = 0u; datIdx < ev->buffer->io_offset; datIdx++)
                           {
                              // yac_host->printf("%02x ", ev->buffer->buffer[datIdx]);
                              Dsysex_data(ev->buffer->buffer[datIdx]);
                           }
                           // yac_host->printf("\n");
                           // yac_host->printf("[trc] MIDIPipeFrame::emit: bufCtl end=%u\n", bufCtl->io_offset);
                        }
                     }
                  }
                  else
                  {
                     yac_host->printf("[trc] MIDIPipeFrame::emit: sysex buffer is NULL (i=%u)\n", i);
                  }

                  numLeft--;
               }
            }
         }

         // Continuous controller events: Bank Select MSB + LSB
         //  (note) [02Jun2021] fix for Nord Drum 3P program change echo
         numLeft = cc_array.num_values;
         // printf("xxx CC numLeft=%u\n", numLeft);
         if(numLeft > 0u)
         {
            midipipe_frame_ctl_t *ev = cc_array.values;

            for(i = 0u; (i < cc_array.max_values) && (numLeft > 0u); i++, ev++)
            {
               if(-1 != ev->dev_idx)
               {
                  if(!ev->b_dont_send)
                  {
                     sUI ccId = (ev->ctl_id & 127u);

                     if( (0 == ccId) || (32 == ccId) )  // bank select MSB or LSB
                     {
                        if(Dgetbufs())
                        {
                           if( !pipeDev->b_midicache || (devCh->cc_states[ccId].value != (sS16)ev->value) )
                           {
                              Dwritebytes(3u);
                              Dctlst(0xB0);
                              Dctl(ccId);
                              Dctl(ev->value);
                              devCh->cc_states[ccId].value = devChShadow->cc_states[ccId].value = (sS16)ev->value;
                           }
                        }
                     }
                  }

                  numLeft--;
               }
            }
         }

         // Program changes
         numLeft = prgchg_array.num_values;
         // printf("xxx prg numLeft=%u\n", numLeft);
         if(numLeft > 0u)
         {
            midipipe_frame_val_t *ev = prgchg_array.values;

            for(i = 0u; (i < prgchg_array.max_values) && (numLeft > 0u); i++, ev++)
            {
               if(-1 != ev->dev_idx)
               {
                  if(!ev->b_dont_send)
                  {
                     if(Dgetbufs())
                     {
                        if( !pipeDev->b_midicache || (devCh->prg != (sS16)ev->value) )
                        {
                           Dwritebytes(2u);
                           Dctlst(0xC0);
                           Dctl(ev->value);
                           devCh->prg = devChShadow->prg = (sS16)ev->value;
                        }
                     }
                  }

                  numLeft--;
               }
            }
         }

         // Pitchbend events
         numLeft = pitchbend_array.num_values;
         if(numLeft > 0u)
         {
            midipipe_frame_val_t *ev = pitchbend_array.values;

            for(i = 0u; (i < pitchbend_array.max_values) && (numLeft > 0u); i++, ev++)
            {
               if(-1 != ev->dev_idx)
               {
                  if(!ev->b_dont_send)
                  {
                     if(Dgetbufs())
                     {
                        if( !pipeDev->b_midicache || (devCh->pitchbend != (sS16)ev->value) )
                        {
                           Dwritebytes(3u);
                           Dctlst(0xE0);
                           Dctl(sU16(ev->value) & 127u);
                           Dctl((sU16(ev->value) >> 7) & 127u);
                           devCh->pitchbend = devChShadow->pitchbend = (sS16)ev->value;
                        }
                     }
                  }

                  numLeft--;
               }
            }
         }

         // Continuous controller events
         numLeft = cc_array.num_values;
         // printf("xxx CC numLeft=%u\n", numLeft);
         if(numLeft > 0u)
         {
            midipipe_frame_ctl_t *ev = cc_array.values;

            for(i = 0u; (i < cc_array.max_values) && (numLeft > 0u); i++, ev++)
            {
               if(-1 != ev->dev_idx)
               {
                  if(!ev->b_dont_send)
                  {
                     sUI ccId = (ev->ctl_id & 127u);

                     if( (0 != ccId) && (32 != ccId) ) // skip bank select MSB + LSB
                     {
                        if(Dgetbufs())
                        {
                           if( !pipeDev->b_midicache || (devCh->cc_states[ccId].value != (sS16)ev->value) )
                           {
                              Dwritebytes(3u);
                              Dctlst(0xB0);
                              Dctl(ccId);
                              Dctl(ev->value);
                              devCh->cc_states[ccId].value = devChShadow->cc_states[ccId].value = (sS16)ev->value;
                           }

                           // Send performance controller LSB ?
                           if(pipeDev->b_cc_perf_lsb[ev->midi_ch])
                           {
                              if( ( 1u/*mw*/ == ccId) ||
                                  ( 2u/*bc*/ == ccId) ||
                                  ( 4u/*fc*/ == ccId) ||
                                  (11u/*ex*/ == ccId)
                                  )
                              {
                                 sU8 valLSB = sUI(ev->value * 128) & 127u;
                                 sU8 ccIdLSB = ccId | 32u;
                                 if(!pipeDev->b_midicache || (devCh->cc_states[ccIdLSB].value != valLSB))
                                 {
                                    Dwritebytes(3u);
                                    Dctlst(0xB0);
                                    Dctl(ccIdLSB);
                                    Dctl(valLSB);
                                    devCh->cc_states[ccIdLSB].value = devChShadow->cc_states[ccIdLSB].value = valLSB;
                                 }
                              }
                           }
                        }
                     }
                  }

                  numLeft--;
               }
            }
         }

         // Registered parameter events
         numLeft = rpn_array.num_values;
         if(numLeft > 0u)
         {
            midipipe_frame_ctl_t *ev = rpn_array.values;

            for(i = 0u; (i < rpn_array.max_values) && (numLeft > 0u); i++, ev++)
            {
               if(-1 != ev->dev_idx)
               {
                  if(!ev->b_dont_send)
                  {
                     if(Dgetbufs())
                     {
                        sUI rpn = (ev->ctl_id & 16383u);

                        if(!pipeDev->b_midicache ||
                           (devCh->rpn_states[rpn].value != (sS16)ev->value) ||
                           (98/*smpoffms_lo*/ ==  rpn) || (99/*smpoffms_hi*/ ==  rpn)
                           )
                        {
                           // yac_host->printf("xxx MIDIPipeFrame::emit: rpn=%u ev->value=%u\n", rpn, (sU16)ev->value);
                           sBool bRS = Drs();
                           sU8 paramMode = pipeDev->param_mode[ev->midi_ch & 15u];
                           sUI numBytes =
                              bRS
                              ? (3u + 2u + 2u + ((2u/*MSB*/ == paramMode) ? 0u : 2u))
                              : (3u + 3u + 3u + ((2u/*MSB*/ == paramMode) ? 0u : 3u))
                              ;
                           Dwritebytes(numBytes);

                           Dctlst(0xB0);
                           Dctl(101);  // RPN select MSB
                           Dctl(rpn >> 7);

                           if(!bRS)
                           {
                              Dctlst(0xB0);
                           }
                           Dctl(100);  // RPN select LSB
                           Dctl(rpn & 127);

                           switch(pipeDev->param_mode[ev->midi_ch & 15u])
                           {
                              default:
                              case 0:  // MSBLSB
                                 if(!bRS)
                                 {
                                    Dctlst(0xB0);
                                 }
                                 Dctl(6);   // data entry MSB
                                 Dctl(sU16(ev->value) >> 7);

                                 if(!bRS)
                                 {
                                    Dctlst(0xB0);
                                 }
                                 Dctl(38);  // data entry LSB
                                 Dctl(sU16(ev->value) & 127u);
                                 break;

                              case 1:  // LSBMSB
                                 if(!bRS)
                                 {
                                    Dctlst(0xB0);
                                 }
                                 Dctl(38);  // data entry LSB
                                 Dctl(sU16(ev->value) & 127u);

                                 if(!bRS)
                                 {
                                    Dctlst(0xB0);
                                 }
                                 Dctl(6);   // data entry MSB
                                 Dctl(sU16(ev->value) >> 7);
                                 break;

                              case 2:  // MSB
                                 if(!bRS)
                                 {
                                    Dctlst(0xB0);
                                 }
                                 Dctl(6);   // data entry MSB
                                 Dctl(sU16(ev->value) & 127u);  // LSB => MSB  (there is no LSB)
                                 break;
                           }

                           devCh->rpn_states[rpn].value = devChShadow->rpn_states[rpn].value = (sS16)ev->value;
                        }
                     }
                  }

                  numLeft--;
               }
            }
         }

         // Non-registered parameter events
         numLeft = nrpn_array.num_values;
         if(numLeft > 0u)
         {
            midipipe_frame_ctl_t *ev = nrpn_array.values;

            for(i = 0u; (i < nrpn_array.max_values) && (numLeft > 0u); i++, ev++)
            {
               if(-1 != ev->dev_idx)
               {
                  if(!ev->b_dont_send)
                  {
                     if(Dgetbufs())
                     {
                        sUI nrpn = (ev->ctl_id & 16383u);

                        if(!pipeDev->b_midicache || (devCh->nrpn_states[nrpn].value != (sS16)ev->value))
                        {
                           sBool bRS = Drs();
                           sU8 paramMode = pipeDev->param_mode[ev->midi_ch & 15u];
                           sUI numBytes =
                              bRS
                              ? (3u + 2u + 2u + ((2u/*MSB*/ == paramMode) ? 0u : 2u))
                              : (3u + 3u + 3u + ((2u/*MSB*/ == paramMode) ? 0u : 3u))
                              ;
                           Dwritebytes(numBytes);

                           Dctlst(0xB0);
                           Dctl(99);  // NRPN select MSB
                           Dctl(nrpn >> 7);

                           if(!bRS)
                           {
                              Dctlst(0xB0);
                           }
                           Dctl(98);  // NRPN select LSB
                           Dctl(nrpn & 127);

                           switch(pipeDev->param_mode[ev->midi_ch & 15u])
                           {
                              default:
                              case 0:  // MSBLSB
                                 if(!bRS)
                                 {
                                    Dctlst(0xB0);
                                 }
                                 Dctl(6);  // data entry MSB
                                 Dctl(sU16(ev->value) >> 7);

                                 if(!bRS)
                                 {
                                    Dctlst(0xB0);
                                 }
                                 Dctl(38); // data entry LSB
                                 Dctl(sU16(ev->value) & 127u);
                                 break;

                              case 1:  // LSBMSB
                                 if(!bRS)
                                 {
                                    Dctlst(0xB0);
                                 }
                                 Dctl(38); // data entry LSB
                                 Dctl(sU16(ev->value) & 127u);

                                 if(!bRS)
                                 {
                                    Dctlst(0xB0);
                                 }
                                 Dctl(6);  // data entry MSB
                                 Dctl(sU16(ev->value) >> 7);
                                 break;

                              case 2:  // MSB
                                 if(!bRS)
                                 {
                                    Dctlst(0xB0);
                                 }
                                 Dctl(6);   // data entry MSB
                                 Dctl(sU16(ev->value) & 127u);  // LSB => MSB  (there is no LSB)
                                 break;
                           }

                           devCh->nrpn_states[nrpn].value = devChShadow->nrpn_states[nrpn].value = (sS16)ev->value;
                        }
                     }
                  }

                  numLeft--;
               }
            }
         }


         // All Notes Off events
         numLeft = all_notes_off_array.num_values;
         if(numLeft > 0u)
         {
            midipipe_frame_base_t *ev = all_notes_off_array.values;

            for(i = 0u; (i < all_notes_off_array.max_values) && (numLeft > 0u); i++, ev++)
            {
               if(-1 != ev->dev_idx)
               {
                  if(!ev->b_dont_send)
                  {
                     if(Dgetbufs())
                     {
                        if(devCh->num_notes > 0)
                        {
                           for(sUI noteIdx = 0u; noteIdx < 128u; noteIdx++)
                           {
                              if(-1 != devCh->note_states[noteIdx].ticks_left)
                              {
                                 Dwritebytes(3u);

                                 if(pipeDev->b_note_off)
                                 {
                                    Dnotest(0x80);
                                 }
                                 else
                                 {
                                    Dnotest(0x90);  // send note off as note on with velocity=0
                                 }
                                 Dnote(noteIdx);
                                 Dnote(0/*vel*/);

                                 devCh->note_states[noteIdx].ticks_left = -1;
                              }
                           }
                        }
                        devCh->num_notes = 0u;
                     }
                  }

                  numLeft--;
               }
            }
         }

         // Note-Off events (pre note on)
         numLeft = note_off_array.num_values;
         if(numLeft > 0u)
         {
            sS8 lastNoteOffCh = -1;
            sSI lastDevIdx = -1;
            midipipe_frame_note_t *ev = note_off_array.values;

            for(i = 0u; (i < note_off_array.max_values) && (numLeft > 0u); i++, ev++)
            {
               if(-1 != ev->dev_idx)
               {
                  if(ev->b_pre)
                  {
                     if(Dgetbufs())
                     {
                        sUI noteNr = (ev->note_nr & 127u);

                        if(-1 != devCh->note_states[noteNr].ticks_left)
                        {
                           if(!ev->b_dont_send)
                           {
                              if(pipeDev->b_note_off)
                              {
                                 if(!Drs() || (lastNoteOffCh != (sS8)ev->midi_ch) || (lastDevIdx != ev->dev_idx))
                                 {
                                    Dwritebytes(3u);
                                    Dnotest(0x80);
                                    lastNoteOffCh = ev->midi_ch;
                                    lastDevIdx = -1;
                                 }
                                 else
                                 {
                                    Dwritebytes(2u);
                                 }
                                 Dnote(noteNr);
                                 Dnote(sU16(ev->velocity) & 127u);
                              }
                              else
                              {
                                 Dwritebytes(3u);
                                 Dnotest(0x90);
                                 Dnote(noteNr);
                                 Dnote(0);
                                 lastNoteOffCh = -1;
                                 lastDevIdx = -1;
                              }

                              devCh->note_states[noteNr].ticks_left = -1;
                              if(devCh->note_low == (sS16)noteNr)
                              {
                                 devCh->note_low = -1;

                                 // Find next higher note
                                 sUI nn = noteNr + 1;
                                 while(nn < 128)
                                 {
                                    if(-1 != devCh->note_states[noteNr].ticks_left)
                                    {
                                       devCh->note_low = nn;
                                       break;
                                    }
                                    nn++;
                                 }
                              }
                              if(devCh->note_hi == noteNr)
                              {
                                 devCh->note_hi = 0xFFFFu;

                                 // Find next lower note
                                 sSI nn = (sSI)noteNr -1;
                                 while(nn >= 0)
                                 {
                                    if(-1 != devCh->note_states[nn].ticks_left)
                                    {
                                       devCh->note_hi = nn;
                                       break;
                                    }
                                    nn--;
                                 }
                              }

                              if(devCh->num_notes > 0)
                              {
                                 devCh->num_notes--;
                              }
                           }
                        }
                     }

                     // Remove event
                     ev->dev_idx = -1;
                     note_off_array.num_values--;
                  }

                  numLeft--;
               }
            }
         }


         // Note-On events
         numLeft = note_on_array.num_values;
         // if(numLeft > 0u)
         //    yac_host->printf("xxx noteOn numLeft=%u\n", numLeft);
         if(numLeft > 0u)
         {
            midipipe_frame_note_t *ev = note_on_array.values;
            sS8 lastNoteOnCh = -1;
            sSI lastDevIdx = -1;

            // Handle forced note-offs first
            //  (note) mainly to mark note-offs as "pre-noteon" when parsing MIDI input in 'Eureka'

            for(i = 0u; (i < note_on_array.max_values) && (numLeft > 0u); i++, ev++)
            {
               if(-1 != ev->dev_idx)
               {
                  if(!ev->b_dont_send)
                  {
                     if(Dgetbufs())
                     {
                        sUI noteNr = (ev->note_nr & 127u);

                        // Note already playing ?
                        //  (note) some synths can trigger the same note multiple times but this is not supported here
                        if(devCh->note_states[noteNr].ticks_left >= 0)
                        {
                           // Immediate note-off
                           if(pipeDev->b_note_off)
                           {
                              Dwritebytes(3u);
                              Dnotest(0x80);
                              lastNoteOnCh = -1;
                              lastDevIdx = -1;
                           }
                           else if(!Drs() || (lastNoteOnCh != (sS8)ev->midi_ch) || (lastDevIdx != ev->dev_idx))
                           {
                              Dwritebytes(3u);
                              Dnotest(0x90);
                              lastNoteOnCh = ev->midi_ch;
                              lastDevIdx = ev->dev_idx;
                           }
                           else
                           {
                              Dwritebytes(2u);
                           }
                           Dnote(noteNr);
                           Dnote(0);

                           devCh->note_states[noteNr].ticks_left = -1;
                        }

                     }
                  }
               }
            }

            // Handle actual note-ons
            ev = note_on_array.values;
            numLeft = note_on_array.num_values;

            for(i = 0u; (i < note_on_array.max_values) && (numLeft > 0u); i++, ev++)
            {
               if(-1 != ev->dev_idx)
               {
                  if(!ev->b_dont_send)
                  {
                     if(Dgetbufs())
                     {
                        sUI noteNr = (ev->note_nr & 127u);

                        // yac_host->printf("xxx emit noteon devIdx=%d devCh=%u noteNr=%u vel=%u\n",
                        //                  ev->dev_idx,
                        //                  ev->midi_ch,
                        //                  noteNr,
                        //                  ev->velocity
                        //                  );

                        if(!Drs() || (lastNoteOnCh != (sS8)ev->midi_ch) || (lastDevIdx != ev->dev_idx))
                        {
                           Dwritebytes(3u);
                           Dnotest(0x90);
                           lastNoteOnCh = ev->midi_ch;
                           lastDevIdx = ev->dev_idx;
                        }
                        else
                        {
                           Dwritebytes(2u);
                        }
                        Dnote(noteNr);
                        Dnote(sU16(ev->velocity) & 127);

                        if(ev->velocity > 0)
                        {
                           // Note-On
                           devCh->note_states[noteNr].ticks_left = ev->duration;

                           if( /*(-1 == devCh->note_low) ||*/ (noteNr < (sUI)devCh->note_low))
                           {
                              devCh->note_low = (sSI)noteNr;
                           }
                           if( (0xFFFFu == devCh->note_hi) || (noteNr > devCh->note_hi))
                           {
                              devCh->note_hi = noteNr;
                           }

                           devCh->num_notes++;
                        }
                        else
                        {
                           // Note-Off
                           devCh->note_states[noteNr].ticks_left = -1;

                           if(devCh->note_low == (sS16)noteNr)
                           {
                              devCh->note_low = -1;

                              // Find next higher note
                              sUI nn = noteNr + 1;
                              while(nn < 128u)
                              {
                                 if(-1 != devCh->note_states[noteNr].ticks_left)
                                 {
                                    devCh->note_low = nn;
                                    break;
                                 }
                                 nn++;
                              }
                           }

                           if(devCh->note_hi == noteNr)
                           {
                              devCh->note_hi = 0xFFFFu;

                              // Find next lower note
                              sSI nn = (sSI)noteNr -1;
                              while(nn >= 0)
                              {
                                 if(-1 != devCh->note_states[nn].ticks_left)
                                 {
                                    devCh->note_hi = nn;
                                    break;
                                 }
                                 nn--;
                              }
                           }

                           if(devCh->num_notes > 0)
                              devCh->num_notes--;
                        }
                     }
                  }

                  numLeft--;
               }
            }
         }


         // Channel pressure events
         numLeft = chpressure_array.num_values;
         if(numLeft > 0u)
         {
            midipipe_frame_val_t *ev = chpressure_array.values;

            for(i = 0u; (i < chpressure_array.max_values) && (numLeft > 0u); i++, ev++)
            {
               if(-1 != ev->dev_idx)
               {
                  if(!ev->b_dont_send)
                  {
                     if(Dgetbufs())
                     {
                        if(!pipeDev->b_midicache || (devCh->ch_pressure != (sS16)ev->value))
                        {
                           // Workaround for Novation Peak (hanging notes when channel pressure occurs immediately before note off)
                           //  (note) no issues with poly pressure
                           sBool bAllowChannelPressure = (0u == note_off_array.num_values) || pipeDev->b_channel_pressure_and_note_off;
                           if(bAllowChannelPressure)
                           {
                              Dwritebytes(2u);
                              Dnotest(0xD0);
                              Dnote((sU8)ev->value);
                           }
                           devCh->ch_pressure = devChShadow->ch_pressure = (sS16)ev->value;
                        }
                     }
                  }

                  numLeft--;
               }
            }
         }


         // Poly pressure events
         numLeft = polypressure_array.num_values;
         if(numLeft > 0u)
         {
            midipipe_frame_pat_t *ev = polypressure_array.values;

            // yac_host->printf("xxx midipipeframe: emit poly pressure numLeft=%u\n", numLeft);

            for(i=0; (i<polypressure_array.max_values) && (numLeft > 0); i++, ev++)
            {
               if(-1 != ev->dev_idx)
               {
                  if(!ev->b_dont_send)
                  {
                     if(Dgetbufs())
                     {
                        // yac_host->printf("xxx midipipeframe: emit poly pressure pipeDev=%p\n", pipeDev);

                        // (note) DSI Prophet12 does not cache poly pressure (BUG!) => need resend => must disable MIDI cache
                        if(!pipeDev->b_midicache || (devCh->note_states[ev->note_nr].poly_pressure != (sS16)ev->value))
                        {
                           Dwritebytes(3u);
                           Dnotest(0xA0);
                           Dnote(ev->note_nr);
                           Dnote((sU8)ev->value);
                           devCh->note_states[ev->note_nr].poly_pressure =
                              devChShadow->note_states[ev->note_nr].poly_pressure = (sS16)ev->value;
                        }
                     }
                     // else
                     //    yac_host->printf("xxx midipipeframe: skip polypressure event due to filter, note=%d ev->value=%d note_state.value=%d\n", ev->note_nr, ev->value, devCh->note_states[ev->note_nr].poly_pressure);
                  }

                  numLeft--;
               }
            }
         }


         // Note-Off events (post note on (default))
         numLeft = note_off_array.num_values;
         if(numLeft > 0u)
         {
            midipipe_frame_note_t *ev = note_off_array.values;
            sS8 lastNoteOffCh = -1;
            sSI lastDevIdx = -1;

            for(i = 0u; (i < note_off_array.max_values) && (numLeft > 0u); i++, ev++)
            {
               if(-1 != ev->dev_idx)
               {
                  if(!ev->b_dont_send)
                  {
                     if(Dgetbufs())
                     {
                        sUI noteNr = (ev->note_nr & 127u);

                        if(-1 != devCh->note_states[noteNr].ticks_left)
                        {
                           if(pipeDev->b_note_off)
                           {
                              if(!Drs() || (lastNoteOffCh != (sS8)ev->midi_ch) || (lastDevIdx != ev->dev_idx))
                              {
                                 Dwritebytes(3u);
                                 Dnotest(0x80);
                                 lastNoteOffCh = ev->midi_ch;
                                 lastDevIdx = ev->dev_idx;
                              }
                              else
                              {
                                 Dwritebytes(2u);
                              }
                              Dnote(noteNr);
                              Dnote(sU16(ev->velocity) & 127u);
                           }
                           else
                           {
                              Dwritebytes(3u);
                              Dnotest(0x90);
                              Dnote(noteNr);
                              Dnote(0);
                              lastNoteOffCh = -1;
                              lastDevIdx = -1;
                           }

                           devCh->note_states[noteNr].ticks_left = -1;

                           if(devCh->note_low == (sS16)noteNr)
                           {
                              devCh->note_low = -1;

                              // Find next higher note
                              sUI nn = noteNr + 1;
                              while(nn < 128u)
                              {
                                 if(-1 != devCh->note_states[noteNr].ticks_left)
                                 {
                                    devCh->note_low = nn;
                                    break;
                                 }
                                 nn++;
                              }
                           }
                           if(devCh->note_hi == noteNr)
                           {
                              devCh->note_hi = 0xFFFFu;

                              // Find next lower note
                              sSI nn = (sSI)noteNr -1;

                              while(nn >= 0)
                              {
                                 if(-1 != devCh->note_states[nn].ticks_left)
                                 {
                                    devCh->note_hi = nn;
                                    break;
                                 }
                                 nn--;
                              }
                           }

                           if(devCh->num_notes > 0)
                              devCh->num_notes--;
                        }
                     }
                  }

                  numLeft--;
               }
            }
         }


      } // if getNumEvents() > 0
   } // if root
}

void MIDIPipeFrame::free(void) {
   prgchg_array       .free();
   chpressure_array   .free();
   pitchbend_array    .free();
   cc_array           .free();
   rpn_array          .free();
   nrpn_array         .free();
   all_notes_off_array.free();
   note_off_array     .free();
   note_on_array      .free();
   polypressure_array .free();
   // if(sysex_array.num_values > 0u)
   //    yac_host->printf("xxx MIDIPipeFrame::free: sysex num=%u\n", sysex_array.num_values);
   sysex_array        .free();
   sysrt_array        .free();

   marker = 0;
}

void MIDIPipeFrame::optimize(void) {
   prgchg_array       .optimize();
   chpressure_array   .optimize();
   pitchbend_array    .optimize();
   cc_array           .optimize();
   rpn_array          .optimize();
   nrpn_array         .optimize();
   all_notes_off_array.optimize();
   note_off_array     .optimize();
   note_on_array      .optimize();
   polypressure_array .optimize();
   sysex_array        .optimize();
   sysrt_array        .optimize();
}

void MIDIPipeFrame::empty(void) {
   // yac_host->printf("xxx MIDIPipeFrame::empty ENTER\n");
   prgchg_array       .empty();
   chpressure_array   .empty();
   pitchbend_array    .empty();
   cc_array           .empty();
   rpn_array          .empty();
   nrpn_array         .empty();
   all_notes_off_array.empty();
   note_off_array     .empty();
   note_on_array      .empty();
   polypressure_array .empty();
   sysex_array        .empty();
   sysrt_array        .empty();
   // yac_host->printf("xxx MIDIPipeFrame::empty LEAVE\n");

   marker = 0;
}

void MIDIPipeFrame::mergeFrame(MIDIPipeFrame *_o) {
   prgchg_array       .mergeArray(&_o->prgchg_array);
   chpressure_array   .mergeArray(&_o->chpressure_array);
   pitchbend_array    .mergeArray(&_o->pitchbend_array);
   cc_array           .mergeArray(&_o->cc_array);
   rpn_array          .mergeArray(&_o->rpn_array);
   nrpn_array         .mergeArray(&_o->nrpn_array);
   all_notes_off_array.mergeArray(&_o->all_notes_off_array);
   note_off_array     .mergeArray(&_o->note_off_array);
   note_on_array      .mergeArray(&_o->note_on_array);
   polypressure_array .mergeArray(&_o->polypressure_array);
   sysex_array        .mergeArray(&_o->sysex_array);
   sysrt_array        .mergeArray(&_o->sysrt_array);

   if(0 == marker)
   {
      marker = _o->marker;
   }
}

void MIDIPipeFrame::mergeFrameTranspose(MIDIPipeFrame *_o, sSI _transpose, sF32 _durationScale) {
   prgchg_array       .mergeArray(&_o->prgchg_array);
   chpressure_array   .mergeArray(&_o->chpressure_array);
   pitchbend_array    .mergeArray(&_o->pitchbend_array);
   cc_array           .mergeArray(&_o->cc_array);
   rpn_array          .mergeArray(&_o->rpn_array);
   nrpn_array         .mergeArray(&_o->nrpn_array);
   all_notes_off_array.mergeArray(&_o->all_notes_off_array);
   note_on_array      .mergeArrayTranspose(&_o->note_on_array, _transpose, _durationScale);
   note_off_array     .mergeArrayTranspose(&_o->note_off_array, _transpose, _durationScale);
   polypressure_array .mergeArrayTranspose(&_o->polypressure_array, _transpose, _durationScale/*ignored*/);
   sysex_array        .mergeArray(&_o->sysex_array);
   sysrt_array        .mergeArray(&_o->sysrt_array);

   if(0 == marker)
   {
      marker = _o->marker;
   }
}

void MIDIPipeFrame::mergeFrameTransposePrgChg(MIDIPipeFrame *_o, sSI _transpose, sSI _prgChgBaseNote, sSI _noteOffPrgOff) {
   prgchg_array       .mergeArray(&_o->prgchg_array);
   chpressure_array   .mergeArray(&_o->chpressure_array);
   pitchbend_array    .mergeArray(&_o->pitchbend_array);
   cc_array           .mergeArray(&_o->cc_array);
   rpn_array          .mergeArray(&_o->rpn_array);
   nrpn_array         .mergeArray(&_o->nrpn_array);
   all_notes_off_array.mergeArray(&_o->all_notes_off_array);

   // Iterate other note ons and convert to program change
   if(NULL != _o->note_on_array.values)
   {
      if(_o->note_on_array.num_values > 0)
      {
         sUI i;
         midipipe_frame_note_t *ev = _o->note_on_array.values;
         sUI numLeft = _o->note_on_array.num_values;

         for(i=0; (i < _o->note_on_array.max_values) && (numLeft > 0); i++, ev++)
         {
            if(-1 != ev->dev_idx)
            {
               midipipe_frame_val_t evPC;

               evPC.dev_idx = ev->dev_idx;
               evPC.midi_ch = ev->midi_ch;
               evPC.value   = sF32(ev->note_nr + _transpose - _prgChgBaseNote);
               evPC.b_dont_send = ev->b_dont_send;

               if(evPC.value > 127.0f)
               {
                  evPC.value = 127.0f;
               }

               // printf("xxx transform note %u to PC %u\n", ev->note_nr, evPC.value);

               prgchg_array.set(&evPC);

               numLeft--;
            }
         }
      }
   }


   if(0 != _noteOffPrgOff)
   {
      if(0 != _o->note_off_array.num_values)
      {
         // (todo) iterate all note offs
         sUI relIdx;
         sUI absIdx;
         sUI evType;
         midipipe_frame_u *nfEv = getEventLocByIdx(0, &relIdx, &absIdx, &evType, MIDIPIPE_EVENT_TYPE_NOTE_OFF);

         midipipe_frame_val_t evPC;

         evPC.dev_idx = nfEv->note.dev_idx;
         evPC.midi_ch = nfEv->note.midi_ch;
         evPC.value   = sF32(nfEv->note.note_nr + _transpose - _prgChgBaseNote + _noteOffPrgOff);
         evPC.b_dont_send = nfEv->note.b_dont_send;

         if(evPC.value > 127.0f)
         {
            evPC.value = 127.0f;
         }

         prgchg_array.set(&evPC);
      }
   }

   polypressure_array.mergeArray(&_o->polypressure_array);
   sysex_array       .mergeArray(&_o->sysex_array);
   sysrt_array       .mergeArray(&_o->sysrt_array);

   if(0 == marker)
   {
      marker = _o->marker;
   }
}

void MIDIPipeFrame::mergeFrameFltDst(MIDIPipeFrame *_o, sSI _fltDev, sSI _fltCh, sSI _dstDev, sSI _dstCh, sBool _bMergeNotes, sBool _bMergeRPN) {
   prgchg_array      .mergeArrayFltDst(&_o->prgchg_array, _fltDev, _fltCh, _dstDev, _dstCh);
   chpressure_array  .mergeArrayFltDst(&_o->chpressure_array, _fltDev, _fltCh, _dstDev, _dstCh);
   pitchbend_array   .mergeArrayFltDst(&_o->pitchbend_array, _fltDev, _fltCh, _dstDev, _dstCh);
   cc_array          .mergeArrayFltDst(&_o->cc_array, _fltDev, _fltCh, _dstDev, _dstCh);
   if(_bMergeRPN)
   {
      rpn_array          .mergeArrayFltDst(&_o->rpn_array, _fltDev, _fltCh, _dstDev, _dstCh);
   }
   nrpn_array        .mergeArrayFltDst(&_o->nrpn_array, _fltDev, _fltCh, _dstDev, _dstCh);
   if(_bMergeNotes)
   {
      note_on_array      .mergeArrayFltDst(&_o->note_on_array, _fltDev, _fltCh, _dstDev, _dstCh);
      note_off_array     .mergeArrayFltDst(&_o->note_off_array, _fltDev, _fltCh, _dstDev, _dstCh);
      all_notes_off_array.mergeArrayFltDst(&_o->all_notes_off_array, _fltDev, _fltCh, _dstDev, _dstCh);
   }
   polypressure_array.mergeArrayFltDst(&_o->polypressure_array, _fltDev, _fltCh, _dstDev, _dstCh);
   sysex_array       .mergeArrayFltDst(&_o->sysex_array, _fltDev, -1, _dstDev, -1);
   sysrt_array       .mergeArrayFltDst(&_o->sysrt_array, _fltDev, -1, _dstDev, -1);

   if(0 == marker)
   {
      marker = _o->marker;
   }
}

void MIDIPipeFrame::mergeFrameFlt(MIDIPipeFrame *_o, sSI _fltDev, sSI _fltCh) {
   prgchg_array       .mergeArrayFlt(&_o->prgchg_array,        _fltDev, _fltCh);
   chpressure_array   .mergeArrayFlt(&_o->chpressure_array,    _fltDev, _fltCh);
   pitchbend_array    .mergeArrayFlt(&_o->pitchbend_array,     _fltDev, _fltCh);
   cc_array           .mergeArrayFlt(&_o->cc_array,            _fltDev, _fltCh);
   rpn_array          .mergeArrayFlt(&_o->rpn_array,           _fltDev, _fltCh);
   nrpn_array         .mergeArrayFlt(&_o->nrpn_array,          _fltDev, _fltCh);
   note_on_array      .mergeArrayFlt(&_o->note_on_array,       _fltDev, _fltCh);
   note_off_array     .mergeArrayFlt(&_o->note_off_array,      _fltDev, _fltCh);
   all_notes_off_array.mergeArrayFlt(&_o->all_notes_off_array, _fltDev, _fltCh);
   polypressure_array .mergeArrayFlt(&_o->polypressure_array,  _fltDev, _fltCh);
   sysex_array        .mergeArrayFlt(&_o->sysex_array,         _fltDev, -1);
   sysrt_array        .mergeArrayFlt(&_o->sysrt_array,         _fltDev, -1);

   if(0 == marker)
   {
      marker = _o->marker;
   }
}

void YAC_VCALL MIDIPipeFrame::yacOperatorAssign(YAC_Object *_robj) {
   if(YAC_BCHK(_robj, clid_MIDIPipeFrame))
   {
      YAC_CAST_ARG(MIDIPipeFrame, o, _robj);

      parent_pipe = o->parent_pipe;
      root = o->root;

      prgchg_array       .copyFrom(&o->prgchg_array);
      chpressure_array   .copyFrom(&o->chpressure_array);
      pitchbend_array    .copyFrom(&o->pitchbend_array);
      cc_array           .copyFrom(&o->cc_array);
      rpn_array          .copyFrom(&o->rpn_array);
      nrpn_array         .copyFrom(&o->nrpn_array);
      all_notes_off_array.copyFrom(&o->all_notes_off_array);
      note_off_array     .copyFrom(&o->note_off_array);
      note_on_array      .copyFrom(&o->note_on_array);
      polypressure_array .copyFrom(&o->polypressure_array);
      sysex_array        .copyFrom(&o->sysex_array);
      sysrt_array        .copyFrom(&o->sysrt_array);

      time_stamp = o->time_stamp;
      marker     = o->marker;
   }
}

void MIDIPipeFrame::setIoFilter(sUI _evType) {
   io_filter = _evType;
}

void MIDIPipeFrame::saveState(YAC_Object *_ofs) {

   if(MIDIPIPE_EVENT_TYPE_NONE == io_filter)
   {
      // Write version
      _ofs->yacStreamWriteI16(8);

      _ofs->yacStreamWriteI32(*(sSI*)&time_stamp); // v6+
      _ofs->yacStreamWriteI32(marker); // v7+

      prgchg_array       .saveState(_ofs);
      chpressure_array   .saveState(_ofs);
      pitchbend_array    .saveState(_ofs);
      cc_array           .saveState(_ofs);
      rpn_array          .saveState(_ofs); // v3+
      nrpn_array         .saveState(_ofs);
      note_on_array      .saveState(_ofs);
      note_off_array     .saveState(_ofs);
      polypressure_array .saveState(_ofs); // v2+
      all_notes_off_array.saveState(_ofs); // v4+
      sysex_array        .saveState(_ofs); // v5+
      sysrt_array        .saveState(_ofs); // v8+
   }
   else
   {
      // Currently used only by NodeAuto to save/load note data
      //  (todo) remove this, NodeAuto does not exist anymore
      switch(io_filter)
      {
         default:
            break;

         case MIDIPIPE_EVENT_TYPE_PRGCHG:
            prgchg_array       .saveState(_ofs);
            break;

         case MIDIPIPE_EVENT_TYPE_CHPRESSURE:
            chpressure_array   .saveState(_ofs);
            break;

         case MIDIPIPE_EVENT_TYPE_PITCHBEND:
            pitchbend_array    .saveState(_ofs);
            break;

         case MIDIPIPE_EVENT_TYPE_CC:
            cc_array           .saveState(_ofs);
            break;

         case MIDIPIPE_EVENT_TYPE_RPN:
            rpn_array          .saveState(_ofs);
            break;

         case MIDIPIPE_EVENT_TYPE_NRPN:
            nrpn_array         .saveState(_ofs);
            break;

         case MIDIPIPE_EVENT_TYPE_ALL_NOTES_OFF:
            all_notes_off_array.saveState(_ofs);
            break;

         case MIDIPIPE_EVENT_TYPE_NOTE_OFF:
            note_off_array     .saveState(_ofs);
            break;

         case MIDIPIPE_EVENT_TYPE_NOTE_ON:
            note_on_array      .saveState(_ofs);
            break;

         case MIDIPIPE_EVENT_TYPE_POLYPRESSURE:
            polypressure_array .saveState(_ofs);
            break;

         case MIDIPIPE_EVENT_TYPE_NOTES:
            note_off_array     .saveState(_ofs);
            note_on_array      .saveState(_ofs);
            break;

         case MIDIPIPE_EVENT_TYPE_SYSEX:
            sysex_array        .saveState(_ofs);
            break;

         // // case MIDIPIPE_EVENT_TYPE_SYSRT:
         // //    sysrt_array        .saveState(_ofs);
         // //    break;
      }
   }
}

sBool MIDIPipeFrame::loadState(YAC_Object *_ifs) {

   // yac_host->printf("xxx MIDIPipeFrame::loadState: io_filter=0x%08x\n", io_filter);

   if(MIDIPIPE_EVENT_TYPE_NONE == io_filter)
   {
      sU16 ver = _ifs->yacStreamReadI16();

      // yac_host->printf("xxx MIDIPipeFrame::loadState: ver=%u\n", ver);

      // Time stamp (v6+)
      if(ver >= 6)
      {
         sSI i = _ifs->yacStreamReadI32();
         time_stamp = *(sUI*)&i;
      }

      // Marker (v7+)
      if(ver >= 7)
      {
         marker = _ifs->yacStreamReadI32();
      }

      if(ver >= 1)
      {
         prgchg_array    .loadState(_ifs);
         chpressure_array.loadState(_ifs);
         pitchbend_array .loadState(_ifs);
         cc_array        .loadState(_ifs);
         if(ver >= 3)
         {
            rpn_array.loadState(_ifs);
         }
         nrpn_array      .loadState(_ifs);
         note_on_array   .loadState(_ifs);
         note_off_array  .loadState(_ifs);

         if(ver >= 2)
         {
            polypressure_array.loadState(_ifs);
         }

         if(ver >= 4)
         {
            all_notes_off_array.loadState(_ifs);
         }

         // yac_host->printf("xxx MIDIPipeFrame::loadState: ver=%u\n", ver);

         if(ver >= 5)
         {
            sysex_array.loadState(_ifs);
         }

         if(ver >= 8)
         {
            sysrt_array.loadState(_ifs);
         }

         return YAC_TRUE;
      }

      return YAC_FALSE;
   }
   else
   {
      // (todo) remove this, NodeAuto does not exist anymore
      sBool r;

      switch(io_filter)
      {
         default:
            r = YAC_FALSE;
            break;

         case MIDIPIPE_EVENT_TYPE_PRGCHG:
            r = prgchg_array       .loadState(_ifs);
            break;

         case MIDIPIPE_EVENT_TYPE_CHPRESSURE:
            r = chpressure_array   .loadState(_ifs);
            break;

         case MIDIPIPE_EVENT_TYPE_PITCHBEND:
            r = pitchbend_array    .loadState(_ifs);
            break;

         case MIDIPIPE_EVENT_TYPE_CC:
            r = cc_array           .loadState(_ifs);
            break;

         case MIDIPIPE_EVENT_TYPE_RPN:
            r = rpn_array          .loadState(_ifs);
            break;

         case MIDIPIPE_EVENT_TYPE_NRPN:
            r = nrpn_array         .loadState(_ifs);
            break;

         case MIDIPIPE_EVENT_TYPE_ALL_NOTES_OFF:
            r = all_notes_off_array.loadState(_ifs);
            break;

         case MIDIPIPE_EVENT_TYPE_NOTE_OFF:
            r = note_off_array     .loadState(_ifs);
            break;

         case MIDIPIPE_EVENT_TYPE_NOTE_ON:
            r = note_on_array      .loadState(_ifs);
            break;

         case MIDIPIPE_EVENT_TYPE_POLYPRESSURE:
            r = polypressure_array .loadState(_ifs);
            break;

         case MIDIPIPE_EVENT_TYPE_NOTES:
            r = note_off_array     .loadState(_ifs);
            if(r)
            {
               r = note_on_array   .loadState(_ifs);
            }
            break;

         case MIDIPIPE_EVENT_TYPE_SYSEX:
            r = sysex_array        .loadState(_ifs);
            break;
      }

      return r;
   }
}

void MIDIPipeFrame::remapDeviceIndices(YAC_Object *_outDevIdxMap, YAC_Object *_outDevUseCount) {

   if(YAC_Is_IntArray(_outDevIdxMap))
   {
      if(YAC_Is_IntArray(_outDevUseCount))
      {
         YAC_CAST_ARG(YAC_IntArray, outDevIdxMap, _outDevIdxMap);
         YAC_CAST_ARG(YAC_IntArray, outDevUseCount, _outDevUseCount);

         prgchg_array       .remapDeviceIndices(outDevIdxMap->elements, outDevIdxMap->num_elements, outDevUseCount->elements);
         chpressure_array   .remapDeviceIndices(outDevIdxMap->elements, outDevIdxMap->num_elements, outDevUseCount->elements);
         pitchbend_array    .remapDeviceIndices(outDevIdxMap->elements, outDevIdxMap->num_elements, outDevUseCount->elements);
         cc_array           .remapDeviceIndices(outDevIdxMap->elements, outDevIdxMap->num_elements, outDevUseCount->elements);
         rpn_array          .remapDeviceIndices(outDevIdxMap->elements, outDevIdxMap->num_elements, outDevUseCount->elements);
         nrpn_array         .remapDeviceIndices(outDevIdxMap->elements, outDevIdxMap->num_elements, outDevUseCount->elements);
         all_notes_off_array.remapDeviceIndices(outDevIdxMap->elements, outDevIdxMap->num_elements, outDevUseCount->elements);
         note_off_array     .remapDeviceIndices(outDevIdxMap->elements, outDevIdxMap->num_elements, outDevUseCount->elements);
         note_on_array      .remapDeviceIndices(outDevIdxMap->elements, outDevIdxMap->num_elements, outDevUseCount->elements);
         polypressure_array .remapDeviceIndices(outDevIdxMap->elements, outDevIdxMap->num_elements, outDevUseCount->elements);
         sysex_array        .remapDeviceIndices(outDevIdxMap->elements, outDevIdxMap->num_elements, outDevUseCount->elements);
         sysrt_array        .remapDeviceIndices(outDevIdxMap->elements, outDevIdxMap->num_elements, outDevUseCount->elements);
      }
   }
}

sUI MIDIPipeFrame::replaceDevIdxAndMidiCh(sSI _fltDevIdx, sSI _fltCh, sSI _devIdx, sSI _ch) {
   sUI r = 0u;

   r += prgchg_array       .replaceDevIdxAndMidiCh(_fltDevIdx, _fltCh, _devIdx, _ch);
   r += chpressure_array   .replaceDevIdxAndMidiCh(_fltDevIdx, _fltCh, _devIdx, _ch);
   r += pitchbend_array    .replaceDevIdxAndMidiCh(_fltDevIdx, _fltCh, _devIdx, _ch);
   r += cc_array           .replaceDevIdxAndMidiCh(_fltDevIdx, _fltCh, _devIdx, _ch);
   r += rpn_array          .replaceDevIdxAndMidiCh(_fltDevIdx, _fltCh, _devIdx, _ch);
   r += nrpn_array         .replaceDevIdxAndMidiCh(_fltDevIdx, _fltCh, _devIdx, _ch);
   r += all_notes_off_array.replaceDevIdxAndMidiCh(_fltDevIdx, _fltCh, _devIdx, _ch);
   r += note_off_array     .replaceDevIdxAndMidiCh(_fltDevIdx, _fltCh, _devIdx, _ch);
   r += note_on_array      .replaceDevIdxAndMidiCh(_fltDevIdx, _fltCh, _devIdx, _ch);
   r += polypressure_array .replaceDevIdxAndMidiCh(_fltDevIdx, _fltCh, _devIdx, _ch);
   r += sysex_array        .replaceDevIdxAndMidiCh(_fltDevIdx, _fltCh, _devIdx, _ch);
   r += sysrt_array        .replaceDevIdxAndMidiCh(_fltDevIdx, _fltCh, _devIdx, _ch);

   return r;
}

void MIDIPipeFrame::setUsrFrameIdx(sSI _idx) {
   usr_frame_idx = _idx;
}

sSI MIDIPipeFrame::getUsrFrameIdx(void) {
   return usr_frame_idx;
}

sUI MIDIPipeFrame::decodePolystepRPN(sSI _fltDevIdx, sSI _fltCh,
                                     YAC_Object *_stepnr, YAC_Object *_patnr, YAC_Object *_tracknr
                                     ) {
   sUI r = 0;

   sUI numLeft = rpn_array.num_values;

   if(numLeft > 0)
   {
      // yac_host->printf("xxx decodePolystepRPN: numLeft=%u\n", numLeft);

      YAC_CAST_ARG(YAC_IntArray, stepnr,  _stepnr);
      YAC_CAST_ARG(YAC_IntArray, patnr,   _patnr);
      YAC_CAST_ARG(YAC_IntArray, tracknr, _tracknr);

      midipipe_frame_ctl_t *ev = rpn_array.values;

      for(sUI i = 0u; (i < rpn_array.max_values) && (numLeft > 0u); i++, ev++)
      {
         if(-1 != ev->dev_idx)
         {
            // yac_host->printf("xxx decodePolystepRPN: ev->dev_idx=%d fltDevIdx=%d\n", ev->dev_idx, _fltDevIdx);

            if( (-1 == _fltDevIdx) || (_fltDevIdx == ev->dev_idx) )
            {
               // yac_host->printf("xxx decodePolystepRPN: ev->midi_ch=%d fltCh=%d\n", ev->midi_ch, _fltCh);
               if( (-1 == _fltCh) || (_fltCh == sSI(ev->midi_ch)) )
               {
                  sUI idx = ~0u;

                  // yac_host->printf("xxx decodePolystepRPN: ev->ctl_id=%u value=%u\n", ev->ctl_id, ev->value);

                  if( (ev->ctl_id >= 600/*RPN_COMMON_POLY_STEP_1*/) && (ev->ctl_id <= 615/*RPN_COMMON_POLY_STEP_16*/) )
                  {
                     idx = (sUI)(ev->ctl_id - 600);
                     if(idx < stepnr->num_elements)
                        stepnr->elements[idx] = (sSI)ev->value;
                  }
                  else if( (ev->ctl_id >= 616/*RPN_COMMON_POLY_PAT_1*/) && (ev->ctl_id <= 631/*RPN_COMMON_POLY_PAT_16*/) )
                  {
                     idx = (sUI)(ev->ctl_id - 616);
                     if(idx < patnr->num_elements)
                        patnr->elements[idx] = (sSI)ev->value;
                  }
                  else if( (ev->ctl_id >= 632/*RPN_COMMON_POLY_TRACK_1*/) && (ev->ctl_id <= 647/*RPN_COMMON_POLY_TRACK_16*/) )
                  {
                     idx = (sUI)(ev->ctl_id - 632);
                     if(idx < tracknr->num_elements)
                        tracknr->elements[idx] = (sSI)ev->value;
                  }

                  if(~0u != idx)
                  {
                     if(idx >= r)
                     {
                        r = (idx + 1u);
                     }
                  }
               }
            }

            numLeft--;
         }
      }
   }

   return r;
}

sBool MIDIPipeFrame::isControllerStateDifferentFrom(MIDIPipeFrame *_o) {

   sBool bDiffers = YAC_FALSE;

   bDiffers |= cc_array.isStateDifferentFrom(&_o->cc_array);

   if(YAC_FALSE == bDiffers)
   {
      bDiffers |= rpn_array.isStateDifferentFrom(&_o->rpn_array);

      if(YAC_FALSE == bDiffers)
      {
         bDiffers |= nrpn_array.isStateDifferentFrom(&_o->nrpn_array);

         if(YAC_FALSE == bDiffers)
         {
            bDiffers |= prgchg_array.isStateDifferentFrom(&_o->prgchg_array);

            if(YAC_FALSE == bDiffers)
            {
               bDiffers |= chpressure_array.isStateDifferentFrom(&_o->chpressure_array);

               if(YAC_FALSE == bDiffers)
               {
                  bDiffers |= pitchbend_array.isStateDifferentFrom(&_o->pitchbend_array);

                  if(YAC_FALSE == bDiffers)
                  {
                     bDiffers |= polypressure_array.isStateDifferentFrom(&_o->polypressure_array);

                     if(YAC_FALSE == bDiffers)
                     {
                        bDiffers |= all_notes_off_array.isStateDifferentFrom(&_o->all_notes_off_array);
                     }
                  }
               }
            }
         }
      }
   }

   return bDiffers;
}

sUI MIDIPipeFrame::getPrgChgShortMsgByIdx(sUI _idx, YAC_Object *_retDevIdx) {

   sUI i;
   midipipe_frame_val_t *ev = prgchg_array.values;
   sUI numLeft = prgchg_array.num_values;
   sUI evIdx = 0u;

   for(i=0; (i < prgchg_array.max_values) && (numLeft > 0); i++, ev++)
   {
      if(-1 != ev->dev_idx)
      {
         if(evIdx == _idx)
         {
            if(YAC_BCHK(_retDevIdx, YAC_CLID_INTEGER))
            {
               YAC_CAST_ARG(YAC_Integer, retDevIdx, _retDevIdx);
               retDevIdx->value = ev->dev_idx;
            }
            return (0xC0 | ev->midi_ch) | (sU8(ev->value) << 8);
         }

         evIdx++;
         numLeft--;
      }
   }

   // Should not be reachable
   return 0xFEFEFEFE;  // active sensing fallback
}

sUI MIDIPipeFrame::getChPressureShortMsgByIdx(sUI _idx, YAC_Object *_retDevIdx) {

   sUI i;
   midipipe_frame_val_t *ev = chpressure_array.values;
   sUI numLeft = chpressure_array.num_values;
   sUI evIdx = 0u;

   for(i=0; (i < chpressure_array.max_values) && (numLeft > 0); i++, ev++)
   {
      if(-1 != ev->dev_idx)
      {
         if(evIdx == _idx)
         {
            if(YAC_BCHK(_retDevIdx, YAC_CLID_INTEGER))
            {
               YAC_CAST_ARG(YAC_Integer, retDevIdx, _retDevIdx);
               retDevIdx->value = ev->dev_idx;
            }
            return (0xD0 | ev->midi_ch) | (sU8(ev->value) << 8);
         }

         evIdx++;
         numLeft--;
      }
   }

   // Should not be reachable
   return 0xFEFEFEFE;  // active sensing fallback
}

sUI MIDIPipeFrame::getPitchbendShortMsgByIdx(sUI _idx, YAC_Object *_retDevIdx) {

   sUI i;
   midipipe_frame_val_t *ev = pitchbend_array.values;
   sUI numLeft = pitchbend_array.num_values;
   sUI evIdx = 0u;

   for(i=0; (i < pitchbend_array.max_values) && (numLeft > 0); i++, ev++)
   {
      if(-1 != ev->dev_idx)
      {
         if(evIdx == _idx)
         {
            if(YAC_BCHK(_retDevIdx, YAC_CLID_INTEGER))
            {
               YAC_CAST_ARG(YAC_Integer, retDevIdx, _retDevIdx);
               retDevIdx->value = ev->dev_idx;
            }
            return (0xE0 | ev->midi_ch) | ((sU16(ev->value) & 127u) << 8) | (((sU16(ev->value)>>7)&127u) << 16);
         }

         evIdx++;
         numLeft--;
      }
   }

   // Should not be reachable
   return 0xFEFEFEFE;  // active sensing fallback
}

sUI MIDIPipeFrame::getCCShortMsgByIdx(sUI _idx, YAC_Object *_retDevIdx) {

   sUI i;
   midipipe_frame_ctl_t *ev = cc_array.values;
   sUI numLeft = cc_array.num_values;
   sUI evIdx = 0u;

   for(i=0; (i < cc_array.max_values) && (numLeft > 0); i++, ev++)
   {
      if(-1 != ev->dev_idx)
      {
         if(evIdx == _idx)
         {
            if(YAC_BCHK(_retDevIdx, YAC_CLID_INTEGER))
            {
               YAC_CAST_ARG(YAC_Integer, retDevIdx, _retDevIdx);
               retDevIdx->value = ev->dev_idx;
            }
            return (0xB0 | ev->midi_ch) | ((ev->ctl_id&127u) << 8) | ((sU8(ev->value)&127u) << 16);
         }

         evIdx++;
         numLeft--;
      }
   }

   // Should not be reachable
   return 0xFEFEFEFE;  // active sensing fallback
}

sUI MIDIPipeFrame::getAllNotesOffShortMsgByIdx(sUI _idx, YAC_Object *_retDevIdx) {

   sUI i;
   midipipe_frame_base_t *ev = all_notes_off_array.values;
   sUI numLeft = all_notes_off_array.num_values;
   sUI evIdx = 0u;

   for(i=0; (i < all_notes_off_array.max_values) && (numLeft > 0); i++, ev++)
   {
      if(-1 != ev->dev_idx)
      {
         if(evIdx == _idx)
         {
            if(YAC_BCHK(_retDevIdx, YAC_CLID_INTEGER))
            {
               YAC_CAST_ARG(YAC_Integer, retDevIdx, _retDevIdx);
               retDevIdx->value = ev->dev_idx;
            }
            return (0xB0 | ev->midi_ch) | (123/*allnotesoff*/ << 8) | ((0&127) << 16);
         }

         evIdx++;
         numLeft--;
      }
   }

   // Should not be reachable
   return 0xFEFEFEFE;  // active sensing fallback
}

sUI MIDIPipeFrame::getNoteOffPreShortMsgByIdx(sUI _idx, YAC_Object *_retDevIdx) {
   sUI i;
   midipipe_frame_note_t *ev = note_off_array.values;
   sUI numLeft = note_off_array.num_values;
   sUI evIdx = 0u;

   for(i=0; (i < note_off_array.max_values) && (numLeft > 0); i++, ev++)
   {
      if(-1 != ev->dev_idx)
      {
         if(ev->b_pre)
         {
            if(evIdx == _idx)
            {
               if(YAC_BCHK(_retDevIdx, YAC_CLID_INTEGER))
               {
                  YAC_CAST_ARG(YAC_Integer, retDevIdx, _retDevIdx);
                  retDevIdx->value = ev->dev_idx;
               }
               return (0x80 | ev->midi_ch) | ((ev->note_nr&127) << 8) | ((sU8(ev->velocity)&127) << 16);
            }

            evIdx++;
         }
         numLeft--;
      }
   }

   // Should not be reachable
   return 0xFEFEFEFE;  // active sensing fallback
}

sUI MIDIPipeFrame::getNoteOffPostShortMsgByIdx(sUI _idx, YAC_Object *_retDevIdx) {
   sUI i;
   midipipe_frame_note_t *ev = note_off_array.values;
   sUI numLeft = note_off_array.num_values;
   sUI evIdx = 0u;

   for(i=0; (i < note_off_array.max_values) && (numLeft > 0); i++, ev++)
   {
      if(-1 != ev->dev_idx)
      {
         if(!ev->b_pre)
         {
            if(evIdx == _idx)
            {
               if(YAC_BCHK(_retDevIdx, YAC_CLID_INTEGER))
               {
                  YAC_CAST_ARG(YAC_Integer, retDevIdx, _retDevIdx);
                  retDevIdx->value = ev->dev_idx;
               }
               return (0x80 | ev->midi_ch) | ((ev->note_nr&127) << 8) | ((sU8(ev->velocity)&127) << 16);
            }

            evIdx++;
         }
         numLeft--;
      }
   }

   // Should not be reachable
   return 0xFEFEFEFE;  // active sensing fallback
}

sUI MIDIPipeFrame::getNoteOffShortMsgByIdx(sUI _idx, YAC_Object *_retDevIdx) {
   sUI i;
   midipipe_frame_note_t *ev = note_off_array.values;
   sUI numLeft = note_off_array.num_values;
   sUI evIdx = 0u;

   for(i=0; (i < note_off_array.max_values) && (numLeft > 0); i++, ev++)
   {
      if(-1 != ev->dev_idx)
      {
         if(evIdx == _idx)
         {
            if(YAC_BCHK(_retDevIdx, YAC_CLID_INTEGER))
            {
               YAC_CAST_ARG(YAC_Integer, retDevIdx, _retDevIdx);
               retDevIdx->value = ev->dev_idx;
            }
            return (0x80 | ev->midi_ch) | ((ev->note_nr&127) << 8) | ((sU8(ev->velocity)&127) << 16);
         }

         evIdx++;
         numLeft--;
      }
   }

   // Should not be reachable
   return 0xFEFEFEFE;  // active sensing fallback
}

sUI MIDIPipeFrame::getPolyPressureShortMsgByIdx(sUI _idx, YAC_Object *_retDevIdx) {

   sUI i;
   midipipe_frame_pat_t *ev = polypressure_array.values;
   sUI numLeft = polypressure_array.num_values;
   sUI evIdx = 0u;

   for(i=0; (i < polypressure_array.max_values) && (numLeft > 0); i++, ev++)
   {
      if(-1 != ev->dev_idx)
      {
         if(evIdx == _idx)
         {
            if(YAC_BCHK(_retDevIdx, YAC_CLID_INTEGER))
            {
               YAC_CAST_ARG(YAC_Integer, retDevIdx, _retDevIdx);
               retDevIdx->value = ev->dev_idx;
            }
            return (0xA0 | ev->midi_ch) | ((ev->note_nr&127) << 8) | ((sU8(ev->value)&127u) << 16);
         }

         evIdx++;
         numLeft--;
      }
   }

   // Should not be reachable
   return 0xFEFEFEFE;  // active sensing fallback
}

sUI MIDIPipeFrame::getNoteOnShortMsgByIdx(sUI _idx, YAC_Object *_retDevIdx) {

   sUI i;
   midipipe_frame_note_t *ev = note_on_array.values;
   sUI numLeft = note_on_array.num_values;
   sUI evIdx = 0u;

   for(i=0; (i < note_on_array.max_values) && (numLeft > 0); i++, ev++)
   {
      if(-1 != ev->dev_idx)
      {
         if(evIdx == _idx)
         {
            if(YAC_BCHK(_retDevIdx, YAC_CLID_INTEGER))
            {
               YAC_CAST_ARG(YAC_Integer, retDevIdx, _retDevIdx);
               retDevIdx->value = ev->dev_idx;
            }
            return (0x90u | ev->midi_ch) | ((ev->note_nr&127u) << 8) | ((sU8(ev->velocity)&127u) << 16);
         }

         evIdx++;
         numLeft--;
      }
   }

   // Should not be reachable
   return 0xFEFEFEFE;  // active sensing fallback
}

sSI MIDIPipeFrame::modMatrixCheckEventByFlt(sUI _evIdx,
                                            sSI _fltDev, sSI _fltCh, sSI _globalCh,
                                            YAC_Object *_retVal, YAC_Object *_retVel
                                            ) {
   sSI src = -1;

   if( YAC_Is_Float(_retVal) && YAC_Is_Float(_retVel) )
   {
      YAC_CAST_ARG(YAC_Float, retVal, _retVal);
      YAC_CAST_ARG(YAC_Float, retVel, _retVel);  // alternative val (e.g. note on/on_vel)

      // (note) the defines match those in ModMatrixEntry.tks
#define MAX_NRPN 16384
#define OFF_MACROCTL        0
#define OFF_NOTE_ON        32
#define OFF_NOTE_ON_VEL    32   // same as OFF_NOTE_ON
#define OFF_NOTE_OFF       33
#define OFF_NOTE_OFF_VEL   33   // same as OFF_NOTE_OFF
#define OFF_PRGCHG         34
#define OFF_PITCHBEND      35
#define OFF_PRESSURE       36
#define OFF_CC             37
#define OFF_RPN            (OFF_CC + 128)  // => 164
#define OFF_NRPN           (OFF_RPN + MAX_NRPN)   // => 292
#define TOTAL_NUM_SOURCES  (OFF_NRPN + MAX_NRPN)  // => 420 src indices


      midipipe_frame_u *evu;
      sUI relIdx;
      sUI absIdx;
      sUI evType;
      evu = getEventLocByIdxAndFlt(_evIdx, &relIdx, &absIdx, &evType, -1/*fltType*/, _fltDev, _fltCh, _globalCh);

      if(NULL != evu)
      {
         switch(evType)
         {
            default:
            case MIDIPIPE_EVENT_TYPE_NONE:
               break;

            case MIDIPIPE_EVENT_TYPE_PRGCHG:
               retVal->value = evu->prgchg.value / 127.0f;
               src = OFF_PRGCHG;
               break;

            case MIDIPIPE_EVENT_TYPE_CHPRESSURE:
               retVal->value = evu->chpressure.value / 127.0f;
               src = OFF_PRESSURE;
               break;

            case MIDIPIPE_EVENT_TYPE_PITCHBEND:
               retVal->value = evu->pitchbend.value / 16383.0f;
               src = OFF_PITCHBEND;
               break;

            case MIDIPIPE_EVENT_TYPE_CC:
               retVal->value = evu->cc.value / 127.0f;
               src = OFF_CC + evu->cc.ctl_id;
               break;

            case MIDIPIPE_EVENT_TYPE_RPN:
               retVal->value = evu->rpn.value / 16383.0f;
               if(evu->rpn.ctl_id < MAX_NRPN)
               {
                  src = OFF_RPN + evu->rpn.ctl_id;
               }
               break;

            case MIDIPIPE_EVENT_TYPE_NRPN:
               retVal->value = evu->nrpn.value / 16383.0f;
               if(evu->nrpn.ctl_id < MAX_NRPN)
               {
                  src = OFF_NRPN + evu->nrpn.ctl_id;
               }
               break;

            case MIDIPIPE_EVENT_TYPE_ALL_NOTES_OFF:
               break;

            case MIDIPIPE_EVENT_TYPE_NOTE_OFF:
               retVal->value = evu->note.note_nr  / 127.0f;
               retVel->value = evu->note.velocity / 127.0f;
               src = OFF_NOTE_OFF;
               break;

            case MIDIPIPE_EVENT_TYPE_NOTE_ON:
               retVal->value = evu->note.note_nr  / 127.0f;
               retVel->value = evu->note.velocity / 127.0f;
               src = OFF_NOTE_ON;
               break;

            case MIDIPIPE_EVENT_TYPE_POLYPRESSURE:
               retVal->value = evu->polypressure.value / 127.0f;
               src = OFF_PRESSURE;
               break;

            case MIDIPIPE_EVENT_TYPE_SYSEX:
               break;
         } // switch evType
      } // if evu
   } // if retVal/retVel are Float objects

   return src;
}
