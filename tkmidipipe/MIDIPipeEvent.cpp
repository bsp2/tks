// ----
// ---- file   : MIDIPipeEvent.cpp
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2014-2024 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : This is part of the "syntracker" sequencer.
// ----
// ---- created: 01Sep2014
// ---- changed: 06Sep2014, 07Sep2014, 11Jan2015, 16Feb2015, 27Feb2016, 28Feb2016, 10Feb2017
// ----          25Jun2019, 02Aug2019, 07Jan2022
// ----
// ----
// ----

#include "tkmidipipe.h"


MIDIPipeEvent::MIDIPipeEvent(void) {
   event_index = 0u;
   event_type  = MIDIPIPE_EVENT_TYPE_NONE;
}

MIDIPipeEvent::~MIDIPipeEvent(void) {
}

sUI MIDIPipeEvent::getIndex(void) {
   return event_index;
}

sUI MIDIPipeEvent::getType(void) {
   return event_type;
}

void MIDIPipeEvent::getTypeName(YAC_Value *_r) {
   _r->initEmptyString();

   switch(event_type)
   {
      default:
      case MIDIPIPE_EVENT_TYPE_NONE:
         _r->value.string_val->visit("<none>");
         break;

      case MIDIPIPE_EVENT_TYPE_PRGCHG:
         _r->value.string_val->visit("Program Change");
         break;

      case MIDIPIPE_EVENT_TYPE_CHPRESSURE:
         _r->value.string_val->visit("Ch.Pressure");
         break;

      case MIDIPIPE_EVENT_TYPE_PITCHBEND:
         _r->value.string_val->visit("Pitchbend");
         break;

      case MIDIPIPE_EVENT_TYPE_CC:
         _r->value.string_val->visit("CC");
         break;

      case MIDIPIPE_EVENT_TYPE_RPN:
         _r->value.string_val->visit("RPN");
         break;

      case MIDIPIPE_EVENT_TYPE_NRPN:
         _r->value.string_val->visit("NRPN");
         break;

      case MIDIPIPE_EVENT_TYPE_NOTE_OFF:
         _r->value.string_val->visit("Note Off");
         break;

      case MIDIPIPE_EVENT_TYPE_NOTE_ON:
         _r->value.string_val->visit("Note On");
         break;

      case MIDIPIPE_EVENT_TYPE_POLYPRESSURE:
         _r->value.string_val->visit("PolyPressure");
         break;

      case MIDIPIPE_EVENT_TYPE_SYSEX:
         _r->value.string_val->visit("SysEx");
         break;

      case MIDIPIPE_EVENT_TYPE_SYSRT:
         _r->value.string_val->visit("SysRT");
         break;
   }
}

sSI MIDIPipeEvent::getDevIdx(void) {
   return data.base.dev_idx;
}

sSI MIDIPipeEvent::getMidiCh(void) {
   return data.base.midi_ch;
}

sSI MIDIPipeEvent::getPrgChg(void) {
   if(MIDIPIPE_EVENT_TYPE_PRGCHG == event_type)
   {
      return sSI(data.prgchg.value);
   }
   else
   {
      return -1;
   }
}

sF32 MIDIPipeEvent::getChPressure(void) {
   if(MIDIPIPE_EVENT_TYPE_CHPRESSURE == event_type)
   {
      return data.chpressure.value;
   }
   else
   {
      return -1.0f;
   }
}

sF32 MIDIPipeEvent::getPitchbend(void) {
   if(MIDIPIPE_EVENT_TYPE_PITCHBEND == event_type)
   {
      return data.pitchbend.value;
   }
   else
   {
      return -1.0f;
   }
}

sSI MIDIPipeEvent::getCcId(void) {
   if(MIDIPIPE_EVENT_TYPE_CC == event_type)
   {
      return data.cc.ctl_id;
   }
   else
   {
      return -1;
   }
}

sF32 MIDIPipeEvent::getCcValue(void) {
   if(MIDIPIPE_EVENT_TYPE_CC == event_type)
   {
      return data.cc.value;
   }
   else
   {
      return -1.0f;
   }
}

sSI MIDIPipeEvent::getRpn(void) {
   if(MIDIPIPE_EVENT_TYPE_RPN == event_type)
   {
      return data.rpn.ctl_id;
   }
   else
   {
      return -1;
   }
}

sF32 MIDIPipeEvent::getRpnValue(void) {
   if(MIDIPIPE_EVENT_TYPE_RPN == event_type)
   {
      return data.nrpn.value;
   }
   else
   {
      return -1.0f;
   }
}

sSI MIDIPipeEvent::getNrpn(void) {
   if(MIDIPIPE_EVENT_TYPE_NRPN == event_type)
   {
      return data.nrpn.ctl_id;
   }
   else
   {
      return -1;
   }
}

sF32 MIDIPipeEvent::getNrpnValue(void) {
   if(MIDIPIPE_EVENT_TYPE_NRPN == event_type)
   {
      return data.nrpn.value;
   }
   else
   {
      return -1.0f;
   }
}

sSI MIDIPipeEvent::getNote(void) {
   if((MIDIPIPE_EVENT_TYPE_NOTE_ON == event_type) ||
      (MIDIPIPE_EVENT_TYPE_NOTE_OFF == event_type)
      )
   {
      return data.note.note_nr;
   }
   else
   {
      return -1;
   }
}

sF32 MIDIPipeEvent::getVelocity(void) {
   if((MIDIPIPE_EVENT_TYPE_NOTE_ON == event_type) ||
      (MIDIPIPE_EVENT_TYPE_NOTE_OFF == event_type)
      )
   {
      return data.note.velocity;
   }
   else
   {
      return -1.0f;
   }
}

sSI MIDIPipeEvent::getDuration(void) {
   if(MIDIPIPE_EVENT_TYPE_NOTE_ON == event_type)
   {
      return data.note.duration;
   }
   else
   {
      return -1;
   }
}

sSI MIDIPipeEvent::getPolyPressureNote(void) {
   if(MIDIPIPE_EVENT_TYPE_POLYPRESSURE == event_type)
   {
      return data.polypressure.note_nr;
   }
   else
   {
      return -1;
   }
}

sF32 MIDIPipeEvent::getPolyPressureValue(void) {
   if(MIDIPIPE_EVENT_TYPE_POLYPRESSURE == event_type)
   {
      return data.polypressure.value;
   }
   else
   {
      return -1.0f;
   }
}

sF32 MIDIPipeEvent::getArg1(void) {
   switch(event_type)
   {
      default:
      case MIDIPIPE_EVENT_TYPE_NONE:
         return -1.0f;

      case MIDIPIPE_EVENT_TYPE_PRGCHG:
         return data.prgchg.value;

      case MIDIPIPE_EVENT_TYPE_CHPRESSURE:
         return data.chpressure.value;

      case MIDIPIPE_EVENT_TYPE_PITCHBEND:
         return data.pitchbend.value;

      case MIDIPIPE_EVENT_TYPE_CC:
         return sF32(data.cc.ctl_id);

      case MIDIPIPE_EVENT_TYPE_RPN:
         return sF32(data.rpn.ctl_id);

      case MIDIPIPE_EVENT_TYPE_NRPN:
         return sF32(data.nrpn.ctl_id);

      case MIDIPIPE_EVENT_TYPE_NOTE_OFF:
         return sF32(data.note.note_nr);

      case MIDIPIPE_EVENT_TYPE_NOTE_ON:
         return sF32(data.note.note_nr);

      case MIDIPIPE_EVENT_TYPE_POLYPRESSURE:
         return sF32(data.polypressure.note_nr);

      case MIDIPIPE_EVENT_TYPE_SYSRT:
         return sF32(data.sysrt.sysrt_byte);
   }
}

sF32 MIDIPipeEvent::getArg2(void) {
   switch(event_type)
   {
      default:
      case MIDIPIPE_EVENT_TYPE_NONE:
         return -1.0f;

      case MIDIPIPE_EVENT_TYPE_PRGCHG:
         return -1.0f;

      case MIDIPIPE_EVENT_TYPE_CHPRESSURE:
         return -1.0f;

      case MIDIPIPE_EVENT_TYPE_PITCHBEND:
         return -1.0f;

      case MIDIPIPE_EVENT_TYPE_CC:
         return data.cc.value;

      case MIDIPIPE_EVENT_TYPE_RPN:
         return data.rpn.value;

      case MIDIPIPE_EVENT_TYPE_NRPN:
         return data.nrpn.value;

      case MIDIPIPE_EVENT_TYPE_NOTE_OFF:
         return data.note.velocity;

      case MIDIPIPE_EVENT_TYPE_NOTE_ON:
         return data.note.velocity;

      case MIDIPIPE_EVENT_TYPE_POLYPRESSURE:
         return data.polypressure.value;
   }
}

sSI MIDIPipeEvent::getArg3(void) {
   switch(event_type)
   {
      default:
      case MIDIPIPE_EVENT_TYPE_NONE:
         return -1;

      case MIDIPIPE_EVENT_TYPE_PRGCHG:
         return -1;

      case MIDIPIPE_EVENT_TYPE_CHPRESSURE:
         return -1;

      case MIDIPIPE_EVENT_TYPE_PITCHBEND:
         return -1;

      case MIDIPIPE_EVENT_TYPE_CC:
         return -1;

      case MIDIPIPE_EVENT_TYPE_RPN:
         return -1;

      case MIDIPIPE_EVENT_TYPE_NRPN:
         return -1;

      case MIDIPIPE_EVENT_TYPE_NOTE_OFF:
         return data.note.duration;

      case MIDIPIPE_EVENT_TYPE_NOTE_ON:
         return data.note.duration;

      case MIDIPIPE_EVENT_TYPE_POLYPRESSURE:
         return -1;
   }
}

void MIDIPipeEvent::getArg1Name(YAC_Value *_r) {
   _r->initEmptyString();

   switch(event_type)
   {
      default:
      case MIDIPIPE_EVENT_TYPE_NONE:
         _r->value.string_val->visit("-");
         break;

      case MIDIPIPE_EVENT_TYPE_PRGCHG:
         _r->value.string_val->visit("Program");
         break;

      case MIDIPIPE_EVENT_TYPE_CHPRESSURE:
         _r->value.string_val->visit("Ch.Pressure");
         break;

      case MIDIPIPE_EVENT_TYPE_PITCHBEND:
         _r->value.string_val->visit("Pitchbend");
         break;

      case MIDIPIPE_EVENT_TYPE_CC:
         _r->value.string_val->visit("CC Id");
         break;

      case MIDIPIPE_EVENT_TYPE_RPN:
         _r->value.string_val->visit("RPN");
         break;

      case MIDIPIPE_EVENT_TYPE_NRPN:
         _r->value.string_val->visit("NRPN");
         break;

      case MIDIPIPE_EVENT_TYPE_NOTE_OFF:
         _r->value.string_val->visit("Note");
         break;

      case MIDIPIPE_EVENT_TYPE_NOTE_ON:
         _r->value.string_val->visit("Note");
         break;

      case MIDIPIPE_EVENT_TYPE_POLYPRESSURE:
         _r->value.string_val->visit("PolyAT.Note");
         break;

      case MIDIPIPE_EVENT_TYPE_SYSRT:
         _r->value.string_val->visit("SysRT.Byte");
         break;
   }
}

void MIDIPipeEvent::getArg2Name(YAC_Value *_r) {
   _r->initEmptyString();

   switch(event_type)
   {
      default:
      case MIDIPIPE_EVENT_TYPE_NONE:
         _r->value.string_val->visit("-");
         break;

      case MIDIPIPE_EVENT_TYPE_PRGCHG:
         _r->value.string_val->visit("-");
         break;

      case MIDIPIPE_EVENT_TYPE_CHPRESSURE:
         _r->value.string_val->visit("-");
         break;

      case MIDIPIPE_EVENT_TYPE_PITCHBEND:
         _r->value.string_val->visit("-");
         break;

      case MIDIPIPE_EVENT_TYPE_CC:
         _r->value.string_val->visit("CC Value");
         break;

      case MIDIPIPE_EVENT_TYPE_RPN:
         _r->value.string_val->visit("RPN Value");
         break;

      case MIDIPIPE_EVENT_TYPE_NRPN:
         _r->value.string_val->visit("NRPN Value");
         break;

      case MIDIPIPE_EVENT_TYPE_NOTE_OFF:
         _r->value.string_val->visit("Velocity");
         break;

      case MIDIPIPE_EVENT_TYPE_NOTE_ON:
         _r->value.string_val->visit("Velocity");
         break;

      case MIDIPIPE_EVENT_TYPE_POLYPRESSURE:
         _r->value.string_val->visit("PolyAT.Value");
         break;
   }
}

void MIDIPipeEvent::getArg3Name(YAC_Value *_r) {
   _r->initEmptyString();

   switch(event_type)
   {
      default:
      case MIDIPIPE_EVENT_TYPE_NONE:
         _r->value.string_val->visit("-");
         break;

      case MIDIPIPE_EVENT_TYPE_PRGCHG:
         _r->value.string_val->visit("-");
         break;

      case MIDIPIPE_EVENT_TYPE_CHPRESSURE:
         _r->value.string_val->visit("-");
         break;

      case MIDIPIPE_EVENT_TYPE_PITCHBEND:
         _r->value.string_val->visit("-");
         break;

      case MIDIPIPE_EVENT_TYPE_CC:
         _r->value.string_val->visit("-");
         break;

      case MIDIPIPE_EVENT_TYPE_RPN:
         _r->value.string_val->visit("-");
         break;

      case MIDIPIPE_EVENT_TYPE_NRPN:
         _r->value.string_val->visit("-");
         break;

      case MIDIPIPE_EVENT_TYPE_NOTE_OFF:
         _r->value.string_val->visit("Duration");
         break;

      case MIDIPIPE_EVENT_TYPE_NOTE_ON:
         _r->value.string_val->visit("Duration");
         break;

      case MIDIPIPE_EVENT_TYPE_POLYPRESSURE:
         _r->value.string_val->visit("-");
         break;
   }
}

sBool MIDIPipeEvent::isNote(void) {
   return
      (MIDIPIPE_EVENT_TYPE_NOTE_OFF == event_type) ||
      (MIDIPIPE_EVENT_TYPE_NOTE_ON  == event_type) ;
}

sBool MIDIPipeEvent::isNoteOffPre(void) {
   if(MIDIPIPE_EVENT_TYPE_NOTE_OFF == event_type)
   {
      return data.note.b_pre;
   }
   return YAC_FALSE;
}

YAC_Buffer *MIDIPipeEvent::getSysExBuffer(void) {
   YAC_Buffer *ret = NULL;

   if(MIDIPIPE_EVENT_TYPE_SYSEX == event_type)
   {
      ret = data.sysex.buffer;
   }

   return ret;
}

sUI MIDIPipeEvent::getSysRTByte(void) {
   sUI ret = 0u;

   if(MIDIPIPE_EVENT_TYPE_SYSRT == event_type)
   {
      ret = data.sysrt.sysrt_byte;
   }

   return ret;
}
