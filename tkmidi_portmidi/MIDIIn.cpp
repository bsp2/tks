/// MIDIIn.cpp
///
/// (c) 2006-2023 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///
/// changed: 09Sep2006, 02Jan2008, 04Jan2008, 13Jan2008, 09Nov2012, 13Nov2012, 23Mar2013
///          14Apr2013, 23May2013, 04Feb2014, 05Feb2014, 24Feb2014, 20Aug2014, 22Aug2014
///          11Feb2015, 12Aug2017, 19Jan2018, 22Jan2019, 20Jan2023, 04Mar2023, 18Jul2023
///          22Jul2023, 01Aug2023, 17Sep2023
///
///
///

#include <string.h>

#include "inc_portmidi.h"

#define YAC_BIGSTRING defined
#include <yac.h>

#include "MIDIIn.h"
#include "ying_midi.h"

#ifdef YAC_WIN32
#include <windows.h>
#else
#include <unistd.h>  // usleep
#endif // YAC_WIN32


// #define DP if(0);else yac_host->printf
#define DP if(1);else yac_host->printf


// ----------------------------------------------------------------------------
// --                                                                        --
// -- RecordedMIDIEvent                                                      --
// --                                                                        --
// ----------------------------------------------------------------------------
RecordedMIDIEvent::RecordedMIDIEvent(void) {
   millisec = 0;
   b_long   = false;
   data.ptr = NULL;
   size     = 0;
}

RecordedMIDIEvent::~RecordedMIDIEvent() {
}

sSI RecordedMIDIEvent::getMillisec(void) {
   return (sSI) millisec;
}

sBool RecordedMIDIEvent::isLongMessage(void) {
   return b_long;
}

sSI RecordedMIDIEvent::getSize(void) {
   return (sSI) size;
}

sUI RecordedMIDIEvent::getShortMessage(void) {
   if(b_long)
   {
      return 0;
   }
   else
   {
      return data.u32;
   }
}

void RecordedMIDIEvent::copyToStream(YAC_Object *_stream) {
   if(YAC_VALID(_stream))
   {
      if(b_long)
      {
         _stream->yacStreamWrite((sU8*)data.ptr, size);
      }
      else
      {
         _stream->yacStreamWrite((sU8*)&data.u32, size);
      }
   }
   else
   {
      yac_host->printf("[---] RecordedMIDIEvent::copyToStream: invalid stream object.\n");
   }
}

sSI RecordedMIDIEvent::getMidiMapEventType(void) {
   return (sSI) midimap_event_type;
}

sSI RecordedMIDIEvent::getNrpnId(void) {
   return (sSI) n_rpn;
}

sUI RecordedMIDIEvent::getDataEntry(void) {
   return data_entry;
}

void RecordedMIDIEvent::classifyShortMessage(MIDIIn *thiz, sBool *bFiltered) {
   switch(data.u32 & 0xF0u)
   {
      default:
         size = 0;
         *bFiltered = YAC_TRUE;
         break;

      case 0x80:
         size = 3;
         *bFiltered = thiz->b_filter_note_off;
         midimap_event_type = 0/*MIDIMapDefs.TYPE_NOTE_OFF*/;
         break;

      case 0x90:
         size = 3;
         *bFiltered = thiz->b_filter_note_on;
         midimap_event_type = 1/*MIDIMapDefs.TYPE_NOTE_ON*/;
         break;

      case 0xA0:
         size = 3;
         *bFiltered = thiz->b_filter_poly_pressure; // polyphonic aftertouch
         midimap_event_type = 2/*MIDIMapDefs.TYPE_POLY_PRESSURE*/;
         break;

      case 0xB0:
         size = 3;
         *bFiltered = thiz->b_filter_control_change;
         midimap_event_type = 3/*MIDIMapDefs.TYPE_CC*/;

         // also see classifyCtlChange()
         break;

      case 0xC0:
         size = 2;
         *bFiltered = thiz->b_filter_program_change;
         midimap_event_type = 4/*MIDIMapDefs.TYPE_PROGRAM_CHANGE*/;
         break;

      case 0xD0:
         size = 2;
         *bFiltered = thiz->b_filter_channel_pressure; // aftertouch
         midimap_event_type = 5/*MIDIMapDefs.TYPE_CHANNEL_PRESSURE*/;
         break;

      case 0xE0:
         size = 3;
         *bFiltered = thiz->b_filter_pitch_bend;
         midimap_event_type = 6/*MIDIMapDefs.TYPE_PITCHBEND*/;
         break;

      case 0xF0:
         switch(data.u32 & 255u)
         {
            default:
            case 0xF0:
               // whoops?! should be in a long message
               size = 0;
               midimap_event_type = 7/*MIDIMapDefs.TYPE_SYSEX*/;
               *bFiltered = YAC_TRUE;
               break;

            case 0xF1:
               *bFiltered = thiz->b_filter_cm_time_code;
               // TODO
               size = 0;
               midimap_event_type = 8/*MIDIMapDefs.TYPE_SYSCOM_TIMECODE*/;
               break;

            case 0xF2:
               *bFiltered = thiz->b_filter_cm_song_position;
               size = 3;
               midimap_event_type = 9/*MIDIMapDefs.TYPE_SYSCOM_SONG_POSITION*/;
               break;

            case 0xF3:
               *bFiltered = thiz->b_filter_cm_song_select;
               size = 2;
               midimap_event_type = 10/*MIDIMapDefs.TYPE_SYSCOM_SONG_SELECT*/;
               break;

            case 0xF4:
               // undefined
               *bFiltered = YAC_TRUE;
               size = 1;
               midimap_event_type = 11/*MIDIMapDefs.TYPE_SYSCOM_F4*/;
               break;

            case 0xF5:
               // undefined
               *bFiltered = YAC_TRUE;
               size = 1;
               midimap_event_type = 12/*MIDIMapDefs.TYPE_SYSCOM_F5*/;
               break;

            case 0xF6:
               *bFiltered = thiz->b_filter_cm_tune_request;
               size = 1;
               midimap_event_type = 13/*MIDIMapDefs.TYPE_SYSCOM_TUNE_REQUEST*/;
               break;

            case 0xF8:
               *bFiltered = thiz->b_filter_rt_timing_clock;
               size = 1;
               midimap_event_type = 14/*MIDIMapDefs.TYPE_SYSRT_TIMING_CLOCK*/;
               break;

            case 0xF9:
               // undefined
               *bFiltered = YAC_TRUE;
               size = 1;
               midimap_event_type = 15/*MIDIMapDefs.TYPE_SYSRT_F9*/;
               break;

            case 0xFA:
               *bFiltered = thiz->b_filter_rt_start;
               size = 1;
               midimap_event_type = 16/*MIDIMapDefs.TYPE_SYSRT_START*/;
               break;

            case 0xFB:
               *bFiltered = thiz->b_filter_rt_continue;
               size = 1;
               midimap_event_type = 17/*MIDIMapDefs.TYPE_SYSRT_CONTINUE*/;
               break;

            case 0xFC:
               *bFiltered = thiz->b_filter_rt_stop;
               size = 1;
               midimap_event_type = 18/*MIDIMapDefs.TYPE_SYSRT_STOP*/;
               break;

            case 0xFD:
               // undefined
               *bFiltered = YAC_TRUE;
               size = 1;
               midimap_event_type = 19/*MIDIMapDefs.TYPE_SYSRT_FD*/;
               break;

            case 0xFE:
               *bFiltered = thiz->b_filter_rt_active_sensing;
               size = 1;
               midimap_event_type = 20/*MIDIMapDefs.TYPE_SYSRT_ACTIVE_SENSING*/;
               break;

            case 0xFF:
               *bFiltered = thiz->b_filter_rt_system_reset;
               size = 1;
               midimap_event_type = 21/*MIDIMapDefs.TYPE_SYSRT_SYSTEM_RESET*/;
               break;
         }
         break;
   }

}

void RecordedMIDIEvent::classifyCtlChange(MIDIIn *thiz, sBool *bFiltered) {

   // Called when event has been fully received

   // Is (N)RPN select or value ?
   sU8 midich = data.u32 & 0x0Fu;
   sU8 ccNr   = (data.u32 >>  8) & 255u;
   sU8 ccVal  = (data.u32 >> 16) & 255u;

   DP("[trc] MIDIIn<portmidi>::RecordedMIDIEvent::classifyCtlChange: ch=%u ccNr=%u ccVal=%u b_parse=%d\n", midich, ccNr, ccVal, thiz->b_parse_param[midich]);
   // yac_host->printf("xxx MIDIIn<portmidi>::RecordedMIDIEvent::classifyCtlChange: ch=%u ccNr=%u ccVal=%u b_parse=%d\n", midich, ccNr, ccVal, thiz->b_parse_param[midich]);

   if(thiz->b_parse_param[midich])
   {
      switch(ccNr)
      {
         case 6:   // (N)RPN data entry MSB
            // example sequence (dsi prophet 08):
            // [...] MIDIIN: MIM_DATA. data=0x000063b0  NRPN MSB
            // [...] MIDIIN: MIM_DATA. data=0x000f62b0  NRPN LSB
            // [...] MIDIIN: MIM_DATA. data=0x000006b0  DATA ENTRY MSB
            // [...] MIDIIN: MIM_DATA. data=0x007826b0  DATA ENTRY LSB
            //
            // [...] MIDIIN: MIM_DATA. data=0x000063b0
            // [...] MIDIIN: MIM_DATA. data=0x000f62b0
            // [...] MIDIIN: MIM_DATA. data=0x000106b0
            // [...] MIDIIN: MIM_DATA. data=0x002426b0

            DP("[trc] MIDIIn<portmidi>::RecordedMIDIEvent::classifyCtlChange: thiz->data_entry_mode[midich]=%u\n", thiz->data_entry_mode[midich]);
            // yac_host->printf("xxx recv (N)RPN data entry MSB\n");

            if(MIDI_DATAENTRYMODE_MSB == thiz->data_entry_mode[midich])
            {
               // Synth only supports MSB data, write to _lower_ param bits
               //  (more convenient than always leaving the lower bits at 0)
               thiz->data_entry[midich] = (ccVal);
            }
            else
            {
#ifdef YAC_MACOS
               // Store MSB (+clear LSB)
               if(MIDI_DATAENTRYMODE_MSBLSB == thiz->data_entry_mode[midich])
                  thiz->data_entry[midich] = (ccVal << 7);  // LSB may never be received (even though it was sent)
#else
               // Merge with LSB data
               thiz->data_entry[midich] = (thiz->data_entry[midich]&0x7f) | (ccVal << 7);
#endif // YAC_MACOS
            }

            if((MIDI_DATAENTRYMODE_LSBMSB == thiz->data_entry_mode[midich]) ||
               (MIDI_DATAENTRYMODE_MSB == thiz->data_entry_mode[midich])
               )
            {
               if(-1 != thiz->rpn[midich])
               {
                  midimap_event_type = 22/*MIDIMapDefs.TYPE_RPN*/;
                  n_rpn       = thiz->rpn[midich];
                  *bFiltered  = thiz->b_filter_rpn;
               }
               else if(-1 != thiz->nrpn[midich])
               {
                  midimap_event_type = 23/*MIDIMapDefs.TYPE_NRPN*/;
                  n_rpn       = thiz->nrpn[midich];
                  *bFiltered  = thiz->b_filter_nrpn;
               }
            }
            else
            {
#ifdef YAC_MACOS
               // on macOS, the LSB is never received if the data value is 0. sigh.
               //   => record (N)RPN event with LSB=0 (+maybe another one with LSB>0 in case the value was not 0)
               if(-1 != thiz->rpn[midich])
               {
                  midimap_event_type = 22/*MIDIMapDefs.TYPE_RPN*/;
                  n_rpn       = thiz->rpn[midich];
                  *bFiltered  = thiz->b_filter_rpn;
               }
               else if(-1 != thiz->nrpn[midich])
               {
                  midimap_event_type = 23/*MIDIMapDefs.TYPE_NRPN*/;
                  n_rpn       = thiz->nrpn[midich];
                  *bFiltered  = thiz->b_filter_nrpn;
               }
#else
               // Wait for LSB
               *bFiltered = YAC_TRUE;
#endif // YAC_MACOS
            }
            break;

         case 38:  // (N)RPN value LSB

            if(MIDI_DATAENTRYMODE_MSB == thiz->data_entry_mode[midich])
            {
               // Ignore data entry LSB / treat as regular CC (e.g. Novation Peak)
            }
            else
            {
               // Merge with MSB data
               thiz->data_entry[midich] = (thiz->data_entry[midich]&0x3f80) | ccVal;
               // yac_host->printf("xxx MIDIIn<portmidi>::RecordedMIDIEvent::classifyCtlChange: CC38 ccVal=%u, update data_entry[%u] to 0x%04x\n", ccVal, midich, thiz->data_entry[midich]);

               if(MIDI_DATAENTRYMODE_MSBLSB == thiz->data_entry_mode[midich])
               {
                  if(-1 != thiz->rpn[midich])
                  {
                     midimap_event_type = 22/*MIDIMapDefs.TYPE_RPN*/;
                     n_rpn        = thiz->rpn[midich];
                     *bFiltered   = thiz->b_filter_rpn;
                  }
                  else if(-1 != thiz->nrpn[midich])
                  {
                     midimap_event_type = 23/*MIDIMapDef.TYPE_NRPN*/;
                     n_rpn        = thiz->nrpn[midich];
                     *bFiltered   = thiz->b_filter_nrpn;
                  }
               }
               else
               {
                  // Wait for MSB (DATAENTRYMODE_LSBMSB)
                  *bFiltered = YAC_TRUE;
               }
            }
            break;

         case 101: // RPN select MSB
            if(-1 == thiz->rpn[midich])
            {
               thiz->rpn[midich] = (ccVal << 7);
            }
            else
            {
               thiz->rpn[midich] = (thiz->rpn[midich] & 0x007f) | (ccVal << 7);
            }
            thiz->nrpn[midich] = -1;
            *bFiltered = thiz->b_filter_rpn_select;
            break;

         case 100: // RPN select LSB
            if(-1 == thiz->rpn[midich])
            {
               thiz->rpn[midich] = ccVal;
            }
            else
            {
               thiz->rpn[midich] = (thiz->rpn[midich] & 0x3f80) | ccVal;
            }
            thiz->nrpn[midich] = -1;
            *bFiltered = thiz->b_filter_rpn_select;
            break;

         case 99:  // NRPN select MSB
            if(-1 == thiz->nrpn[midich])
            {
               thiz->nrpn[midich] = (ccVal << 7);
            }
            else
            {
               thiz->nrpn[midich] = (thiz->nrpn[midich] & 0x007f) | (ccVal << 7);
            }
            thiz->rpn[midich] = -1;
            *bFiltered = thiz->b_filter_nrpn_select;
            break;

         case 98:  // NRPN select MSB
            if(-1 == thiz->nrpn[midich])
            {
               thiz->nrpn[midich] = ccVal;
            }
            else
            {
               thiz->nrpn[midich] = (thiz->nrpn[midich] & 0x3f80) | ccVal;
            }
            thiz->rpn[midich] = -1;
            *bFiltered = thiz->b_filter_nrpn_select;
            break;

      } // switch ccNr
   }
}


// ----------------------------------------------------------------------------
// --                                                                        --
// -- MIDIIn                                                                 --
// --                                                                        --
// ----------------------------------------------------------------------------
MIDIIn::MIDIIn(void) {
   pm_stream  = NULL;
   b_open     = YAC_FALSE;
   device_idx = -1;

   record_readoff   = 0u;
   record_writeoff  = 0u;
   record_numevents = 0u;
   {
      for(sUI i = 0u; i < MIDIIN_MAXEVENTS; i++)
         recorded_events[i] = NULL;
   }
   mtx_record = NULL;

   // Set MIDI filter defaults
   b_filter_note_off           = 0; // 0x8x
   b_filter_note_on            = 0; // 0x9x
   b_filter_poly_pressure      = 0; // 0xAx
   b_filter_control_change     = 0; // 0xBx
   b_filter_program_change     = 0; // 0xCx
   b_filter_channel_pressure   = 0; // 0xDx
   b_filter_pitch_bend         = 0; // 0xEx
   b_filter_sysex              = 0; // 0xF0
   b_filter_cm_time_code       = 1; // 0xF1
   b_filter_cm_song_position   = 1; // 0xF2
   b_filter_cm_song_select     = 1; // 0xF3
   b_filter_cm_tune_request    = 1; // 0xF6
   b_filter_rt_timing_clock    = 1; // 0xF8
   b_filter_rt_start           = 1; // 0xFA
   b_filter_rt_continue        = 1; // 0xFB;
   b_filter_rt_stop            = 1; // 0xFC
   b_filter_rt_active_sensing  = 1; // 0xFE;
   b_filter_rt_system_reset    = 1; // 0xFF;
   b_filter_rpn_select         = 1; // CC#101(MSB) + CC#100(LSB)
   b_filter_nrpn_select        = 1; // CC#099(MSB) + CC#098(LSB)
   b_filter_rpn                = 0; // CC#006(MSB) + CC#038(LSB) (if -1 != rpn)
   b_filter_nrpn               = 0; // CC#006(MSB) + CC#038(LSB) (if -1 != nrpn)

   for(sUI midiCh = 0u; midiCh < 16u; midiCh++)
   {
      b_parse_param[midiCh]   = YAC_TRUE;
      rpn [midiCh]            = -1;
      nrpn[midiCh]            = -1;
      data_entry[midiCh]      = 0;
      data_entry_mode[midiCh] = MIDI_DATAENTRYMODE_MSBLSB; // MSB,LSB order
   }

   virt_state.init();
}


MIDIIn::~MIDIIn() {
   _close();

   freeEventsAndMutex();  // in case device was not opened and parseBuffer() was used
}

sBool MIDIIn::isOpen(void) {
   return (NULL != pm_stream) && b_open;
}

RecordedMIDIEvent *MIDIIn::beginRecordedEvent(void) {
   // Called by MIDI reader thread

   if(NULL != mtx_record)
      yac_host->yacMutexLock(mtx_record);

   RecordedMIDIEvent *ev = recorded_events[record_writeoff];

   // ev->b_long   = 0;
   // ev->millisec = _millisec;
   // ev->size     = _size;
   // ev->data.msg = _msg;
   // ev->midimap_event_type = _midimapEventType;
   // ev->n_rpn    = _nRpn;
   // ev->data_entry = data_entry[_msg & 0x0000000Fu];

   if(NULL != mtx_record)
      yac_host->yacMutexUnlock(mtx_record);

   return ev;
}

void MIDIIn::endRecordedEvent(RecordedMIDIEvent *_ev) {
   // Called by MIDI reader thread

   if(NULL != mtx_record)
      yac_host->yacMutexLock(mtx_record);

   if(_ev == recorded_events[record_writeoff])
   {
#if 0
      printf("[trc] MIDIIn<portmidi>::endRecordedEvent: ev.msg= %02x %02x %02x %02x\n",
             _ev->data.u8[0],
             _ev->data.u8[1],
             _ev->data.u8[2],
             _ev->data.u8[3]
             );
#endif

      record_writeoff = (record_writeoff + 1u) % MIDIIN_MAXEVENTS;

      record_numevents++;

      if(record_numevents > MIDIIN_MAXEVENTS)
      {
         // Overflow..
         record_numevents = MIDIIN_MAXEVENTS;
      }
   }
   else
   {
      // Discard event if clearEvents() was called since begin*Event()
   }

   if(NULL != mtx_record)
      yac_host->yacMutexUnlock(mtx_record);
}

sU8 *MIDIIn::getNextSysExBuffer(void) {
   sU8 *ret = (midi_buffer_data + (curr_buffer_index * MIDIIN_BLOCKSIZE));
   curr_buffer_index = (curr_buffer_index + 1u) % MIDIIN_NUMBUFFERS;
   return ret;
}

void MIDIIn::resetState(void) {
   state.ev = NULL; // current MIDI event
   state.b_sysex = YAC_FALSE;
   state.sysex_off = 0u;
}

void MIDIIn::consumeMessage(const sU32 _msg) {
   // (note) PortMidi reports events in dword (32bit) packets (pmEv.message)
   // (note) each packet stores between 1 (SysExRT) .. 4 (SysEx) bytes
   // (note) "A real-time message will always occupy a full PmEvent with the status byte in the low-order byte of the PmEvent message field"
   // (note) 'data' always starts with a status byte, or is a continued SysEx message
   sU8 readOff = 0u;
   RecordedMIDIEvent *ev;

   DP("[trc] MIDIIn<portmidi>::consumeMessage: msg=0x%08x\n", _msg);

   sU8 c = (_msg & 255u);

   if(0u != (c & 0x80u))
   {
      if(0xF7u == c)
      {
         // end of sysex (handled below)
         if(!state.b_sysex)
         {
            yac_host->printf("[~~~] MIDIIn<portmidi>::consumeMessage: found $F7 outside of sysex message, skipping..\n");
         }
      }
      else if(0xF0u != c)
      {
         // 1..3 byte message

         state.b_sysex = YAC_FALSE;

         ev = beginRecordedEvent();

         ev->b_long     = YAC_FALSE;
         ev->millisec   = yac_host->yacMilliSeconds();
         ev->data.u32   = _msg;

         // Determine size, midimapType, filter, ..
         sBool bFiltered = YAC_FALSE;
         ev->classifyShortMessage(this, &bFiltered);

         if(!bFiltered)
         {
            if(0xB0u == (ev->data.u32 & 0xF0u))
            {
               ev->classifyCtlChange(this, &bFiltered);
            }

            if(!bFiltered)
            {
               ev->data_entry = data_entry[c & 15u];

               endRecordedEvent(ev);
            }
         }
      }
      else
      {
         // Start of SysEx message
         state.b_sysex = YAC_TRUE;

         state.ev = ev = beginRecordedEvent();

         ev->b_long   = YAC_TRUE;
         ev->millisec = yac_host->yacMilliSeconds();
         ev->data.ptr = getNextSysExBuffer();
         ev->size     = 0u;
         ev->midimap_event_type = 7/*MIDIMapDefs.TYPE_SYSEX*/;
         readOff = 1u;
      }
   }  // if status byte

   if(state.b_sysex)
   {
      ev = state.ev;
      const sU8 *data = (const sU8*)&_msg;
#ifdef YAC_BIG_ENDIAN
      // swap bytes
      sU8 dataSwapped[4];
      dataSwapped[0] = data[3];
      dataSwapped[1] = data[2];
      dataSwapped[2] = data[1];
      dataSwapped[3] = data[0];
      data = dataSwapped;
#endif
      while(readOff < 4u)
      {
         sU8 c = data[readOff++];
         if(0xF7u == c)
         {
            // End of SysEx message
            DP("[trc] MIDIIn<portmidi>::consumeMessage: end of sysex msg: ev->size=%u b_filter_sysex=%d\n", ev->size, b_filter_sysex);
            if(ev->size > 0u)
            {
               if(!b_filter_sysex)
                  endRecordedEvent(ev);
            }
            else
            {
               yac_host->printf("[~~~] MIDIIn<portmidi>::consumeMessage: SysEx buffer size is 0, discarding message\n");
            }
            state.b_sysex = YAC_FALSE;
            break;
         }
         else
         {
            // Data byte
            if(ev->size < MIDIIN_BLOCKSIZE)
            {
               ev->data.ptr[ev->size++] = c;
            }
            else
            {
               // SysEx buffer size overflow, discard event
               yac_host->printf("[~~~] MIDIIn<portmidi>::consumeMessage: SysEx buffer size overflow (%u bytes)\n", MIDIIN_BLOCKSIZE);
               state.b_sysex = YAC_FALSE;
            }
         }
      }
   } // if state.b_sysex

}

void MIDIIn::readPMEvents(void) {
   for(;;)
   {
      PmEvent pmEv;
      DP("[trc] MIDIIn<portmidi>::readPMEvents: call\n");
      int numRead = Pm_Read(pm_stream, &pmEv, 1);
      DP("[trc] MIDIIn<portmidi>::readPMEvents: numRead=%d\n", numRead);
      if(1 == numRead)
      {
         consumeMessage(pmEv.message);
      }
      else
         break;
   }
}

YAC_Object *MIDIIn::_getNextEvent(void) {
   YAC_Object *r = NULL;
   if(isOpen())
   {
      readPMEvents();
   }

   // Return RecordedMIDIEvent* or NULL if no event is available
   if(NULL != mtx_record)
      yac_host->yacMutexLock(mtx_record);

   if(record_numevents > 0u)
   {
      r = recorded_events[record_readoff]; // RecordedMIDIEvent object
      DP("[trc] MIDIIn<portmidi>::getNextEvent: record_readoff=%u r=%p\n", record_readoff, r);
      record_readoff = (record_readoff + 1u) % MIDIIN_MAXEVENTS;
      record_numevents--;
   }
   // else: no new events

   if(NULL != mtx_record)
      yac_host->yacMutexUnlock(mtx_record);
   return r;
}

YAC_Object *MIDIIn::_waitNextEvent(sUI _timeout) {
   if(isOpen())
   {
      sUI msCount = 0u;

      YAC_Object *r = NULL;
      do
      {
         r = _getNextEvent();
         if(NULL == r)
         {
            if(msCount++ == _timeout)
               break;
#ifdef YAC_WIN32
            ::Sleep(1);
#else
            ::usleep(1000);
#endif // YAC_WIN32
         }
      }
      while(NULL == r);

      return r;
   }
   else
   {
      yac_host->printf("[---] MIDIIn::waitNextEvent: device is not open!\n");
   }

   return NULL;
}

sSI MIDIIn::_getNumEvents(void) {
   if(isOpen())
   {
      readPMEvents();
   }
   // (note) parseBuffer() can add (virtual) MIDI input events without opening the device
   sSI r;
   if(NULL != mtx_record)
      yac_host->yacMutexLock(mtx_record);

   r = (sSI) record_numevents;

   if(NULL != mtx_record)
      yac_host->yacMutexUnlock(mtx_record);
   return r;
}

void MIDIIn::_clearEvents(void) {
   if(NULL != mtx_record)
      yac_host->yacMutexLock(mtx_record);

   record_readoff   = 0u;
   record_writeoff  = 0u;
   record_numevents = 0u;

   if(NULL != mtx_record)
      yac_host->yacMutexUnlock(mtx_record);
}

void MIDIIn::_reset(void) {
   _clearEvents();
}

void MIDIIn::_start(void) {
   // (note) no-op in PortMidi
}

void MIDIIn::_stop(void) {
   // (note) no-op in PortMidi
}

sBool MIDIIn::_isAlive(void) {
   // (todo)
   return YAC_TRUE;
}

sBool MIDIIn::_openByName(YAC_Object *_devName) {
   _close();

   if(YAC_VALID(_devName) && YAC_Is_String(_devName))
   {
      YAC_CAST_ARG(YAC_String, devName, _devName);
      const char *devNameChars = (const char*) devName->chars;

      sSI numIODevs = Pm_CountDevices();
      sSI inIdx = 0;
      for(PmDeviceID ioIdx = 0; ioIdx < numIODevs; ioIdx++)
      {
         const PmDeviceInfo *info = Pm_GetDeviceInfo(ioIdx);
         if(0 != info->input)
         {
            if(!strcmp(info->name, devNameChars))
            {
               PmError err = Pm_OpenInput(&pm_stream,
                                          ioIdx,
                                          NULL/*inputDriverInfo*/,
                                          MIDIIN_MAXEVENTS/*bufferSize*/,
                                          NULL/*time_proc*/,  // "a procedure that returns time in milliseconds"
                                          NULL/*time_info*/   // "a pointer passed to time_proc"
                                          );
               if(0 == err)
               {
                  device_name.copy(info->name);
                  device_idx = (sSI)inIdx;
                  open2();

                  if(yac_host->yacGetDebugLevel() > 1)
                  {
                     yac_host->printf("[trc] MIDIIn::open: MIDI device \"%s\" opened.\n", devNameChars);
                  }
                  return YAC_TRUE;
               }
               else
               {
                  yac_host->printf("[---] MIDIIn::openByName: failed to open devName=\"%s\" error=\"%s\"\n", devNameChars, Pm_GetErrorText(err));
               }
            }

            inIdx++;
         }
      }
   }
   return YAC_FALSE;
}

void MIDIIn::_getDeviceName(YAC_Value *_r) {
   YAC_String *s = YAC_New_String();
   s->yacCopy(&device_name);
   _r->initString(s, 1);
}

sSI MIDIIn::getDeviceIdx(void) {
   return b_open ? device_idx : -1;
}

sBool MIDIIn::_openByIdx(sUI _idx) {
   _close();

   sSI numIODevs = Pm_CountDevices();
   sSI inIdx = 0;
   for(PmDeviceID ioIdx = 0; ioIdx < numIODevs; ioIdx++)
   {
      const PmDeviceInfo *info = Pm_GetDeviceInfo(ioIdx);
      if(0 != info->input)
      {
         if(inIdx == sSI(_idx))
         {
            PmError err = Pm_OpenInput(&pm_stream,
                                       ioIdx,
                                       NULL/*inputDriverInfo*/,
                                       MIDIIN_MAXEVENTS/*bufferSize*/,
                                       NULL/*time_proc*/,  // "a procedure that returns time in milliseconds"
                                       NULL/*time_info*/   // "a pointer passed to time_proc"
                                       );
            if(0 == err)
            {
               device_name.copy(info->name);
               device_idx = (sSI)inIdx;
               open2();

               if(yac_host->yacGetDebugLevel() > 1)
               {
                  yac_host->printf("[trc] MIDIIn::open: MIDI device idx=%d name=\"%s\" opened.\n", inIdx, info->name);
               }
               return YAC_TRUE;
            }
            else
            {
               yac_host->printf("[---] MIDIIn::openByName: failed to open MIDI device idx=%d error=\"%s\"\n", inIdx, Pm_GetErrorText(err));
            }
         }

         inIdx++;
      }
   }

   return YAC_FALSE;
}

void MIDIIn::_openLocalMIDI(void) {
   _close();
   open2();
   b_open = YAC_FALSE;
}

void MIDIIn::open2(void) {

   // Init MIDI buffers
   // yac_host->printf("xxx MIDIIn::open2: midi_buffer_data=%p..%p size=%lu\n", midi_buffer_data, midi_buffer_data+sizeof(midi_buffer_data), sizeof(midi_buffer_data));
   memset(midi_buffer_data, 0, sizeof(midi_buffer_data));
   curr_buffer_index = 0u;

   allocEventsAndMutex();

   b_open = YAC_TRUE;
}

void MIDIIn::allocEventsAndMutex(void) {
   // Allocate RecordedMIDIEvent objects
   for(sUI i = 0u; i < MIDIIN_MAXEVENTS; i++)
   {
      recorded_events[i] = YAC_NEW(RecordedMIDIEvent);
   }

   // Create record ringbuf mutex
   mtx_record = yac_host->yacMutexCreate();

   resetState();
}

void MIDIIn::freeEventsAndMutex(void) {
   // Free recorded MIDI event objects
   for(sUI i = 0u; i < MIDIIN_MAXEVENTS; i++)
   {
      YAC_DELETE_SAFE(recorded_events[i]);
   }

   // Delete record ringbuf mutex
   if(NULL != mtx_record)
   {
      yac_host->yacMutexDestroy(mtx_record);
      mtx_record = NULL;
   }
}

void MIDIIn::_close(void) {
   if(NULL != pm_stream)
   {
      b_open = YAC_FALSE;

      Pm_Close(pm_stream);
      pm_stream = NULL;

      freeEventsAndMutex();
   }
}

sSI MIDIIn::GetNumDevices(void) {
   sSI r = 0;
   sSI numIODevs = Pm_CountDevices();
   for(PmDeviceID ioIdx = 0; ioIdx < numIODevs; ioIdx++)
   {
      const PmDeviceInfo *info = Pm_GetDeviceInfo(ioIdx);
      if(0 != info->input)
         r++;
   }
   return r;
}

void MIDIIn::GetDeviceNameByIdx(sUI _idx, YAC_Value *_r) {
   _r->initNull();
   sUI inputIdx = 0u;
   sSI numIODevs = Pm_CountDevices();
   for(PmDeviceID ioIdx = 0; ioIdx < numIODevs; ioIdx++)
   {
      const PmDeviceInfo *info = Pm_GetDeviceInfo(ioIdx);
      if(0 != info->input)
      {
         if(inputIdx == _idx)
         {
            YAC_String *s = YAC_New_String();
            s->copy(info->name);
            _r->initString(s, YAC_TRUE);
            return;
         }
         inputIdx++;
      }
   }
}

void MIDIIn::setFltNoteOff(sSI _bEnable) {
   b_filter_note_off = _bEnable;
}

sSI MIDIIn::getFltNoteOff(void) {
   return b_filter_note_off;
}

void MIDIIn::setFltNoteOn(sSI _bEnable) {
   b_filter_note_on = _bEnable;
}

sSI MIDIIn::getFltNoteOn(void) {
   return b_filter_note_on;
}

void MIDIIn::setFltPolyPressure(sSI _bEnable) {
   b_filter_poly_pressure = _bEnable;
}

sSI MIDIIn::getFltPolyPressure(void) {
   return b_filter_poly_pressure;
}

void MIDIIn::setFltCC(sSI _bEnable) {
   b_filter_control_change = _bEnable;
}

sSI MIDIIn::getFltCC(void) {
   return b_filter_control_change;
}

void MIDIIn::setFltProgramChange(sSI _bEnable) {
   b_filter_program_change = _bEnable;
}

sSI MIDIIn::getFltProgramChange(void) {
   return b_filter_program_change;
}

void MIDIIn::setFltChannelPressure(sSI _bEnable) {
   b_filter_channel_pressure = _bEnable;
}

sSI MIDIIn::getFltChannelPressure(void) {
   return b_filter_channel_pressure;
}

void MIDIIn::setFltPitchbend(sSI _bEnable) {
   b_filter_pitch_bend = _bEnable;
}

sSI MIDIIn::getFltPitchbend(void) {
   return b_filter_pitch_bend;
}

void MIDIIn::setFltSysEx(sSI _bEnable) {
   b_filter_sysex = _bEnable;
}

sSI MIDIIn::getFltSysEx(void) {
   return b_filter_sysex;
}

void MIDIIn::setFltSysComTimeCode(sSI _bEnable) {
   b_filter_cm_time_code = _bEnable;
}

sSI MIDIIn::getFltSysComTimeCode(void) {
   return b_filter_cm_time_code;
}

void MIDIIn::setFltSysComSongPosition(sSI _bEnable) {
   b_filter_cm_song_position = _bEnable;
}

sSI MIDIIn::getFltSysComSongPosition(void) {
   return b_filter_cm_song_position;
}

void MIDIIn::setFltSysComSongSelect(sSI _bEnable) {
   b_filter_cm_song_select = _bEnable;
}

sSI MIDIIn::getFltSysComSongSelect(void) {
   return b_filter_cm_song_select;
}

void MIDIIn::setFltSysComTuneRequest(sSI _bEnable) {
   b_filter_cm_tune_request = _bEnable;
}

sSI MIDIIn::getFltSysComTuneRequest(void) {
   return b_filter_cm_tune_request;
}

void MIDIIn::setFltSysRTTimingClock(sSI _bEnable) {
   b_filter_rt_timing_clock = _bEnable;
}

sSI MIDIIn::getFltSysRTTimingClock(void) {
   return b_filter_rt_timing_clock;
}

void MIDIIn::setFltSysRTStart(sSI _bEnable) {
   b_filter_rt_start = _bEnable;
}

sSI MIDIIn::getFltSysRTStart(void) {
   return b_filter_rt_start;
}

void MIDIIn::setFltSysRTContinue(sSI _bEnable) {
   b_filter_rt_continue = _bEnable;
}

sSI MIDIIn::getFltSysRTContinue(void) {
   return b_filter_rt_continue;
}

void MIDIIn::setFltSysRTStop(sSI _bEnable) {
   b_filter_rt_stop = _bEnable;
}

sSI MIDIIn::getFltSysRTStop(void) {
   return b_filter_rt_stop;
}

void MIDIIn::setFltSysRTActiveSensing(sSI _bEnable) {
   b_filter_rt_active_sensing = _bEnable;
}

sSI MIDIIn::getFltSysRTActiveSensing(void) {
   return b_filter_rt_active_sensing;
}

void MIDIIn::setFltSysRTSystemReset(sSI _bEnable) {
   b_filter_rt_system_reset = _bEnable;
}

sSI MIDIIn::getFltSysRTSystemReset(void) {
   return b_filter_rt_system_reset;
}

void MIDIIn::setFltRpnSelect(sSI _bEnable) {
   b_filter_rpn_select = _bEnable;
}

sSI MIDIIn::getFltRpnSelect(void) {
   return b_filter_rpn_select;
}

void MIDIIn::setFltNrpnSelect(sSI _bEnable) {
   b_filter_nrpn_select = _bEnable;
}

sSI MIDIIn::getFltNrpnSelect(void) {
   return b_filter_nrpn_select;
}

void MIDIIn::setFltRpn(sSI _bEnable) {
   b_filter_rpn = _bEnable;
}

sSI MIDIIn::getFltRpn(void) {
   return b_filter_rpn;
}

void MIDIIn::setFltNrpn(sSI _bEnable) {
   b_filter_nrpn = _bEnable;
}

sSI MIDIIn::getFltNrpn(void) {
   return b_filter_nrpn;
}

void MIDIIn::setEnableParam(sSI _ch, sSI _bEnable) {
   b_parse_param[_ch & 15] = _bEnable;
}

sSI MIDIIn::getEnableParam(sSI _ch) {
   return b_parse_param[_ch & 15];
}

void MIDIIn::setDataEntryMode(sSI _ch, sSI _mode) {
   switch(_mode)
   {
      default:
      case MIDI_DATAENTRYMODE_MSBLSB: // 0
         data_entry_mode[_ch & 15] = MIDI_DATAENTRYMODE_MSBLSB;
         break;

      case MIDI_DATAENTRYMODE_LSBMSB: // 1
         data_entry_mode[_ch & 15] = MIDI_DATAENTRYMODE_LSBMSB;
         break;

      case MIDI_DATAENTRYMODE_MSB:    // 2
         data_entry_mode[_ch & 15] = MIDI_DATAENTRYMODE_MSB;
         break;
   }
}

sSI MIDIIn::getDataEntryMode(sSI _ch) {
   return data_entry_mode[_ch & 15];
}

void MIDIIn::parseMIDIByte(const sU8 c, midiin_reader_state_t &st) {
   switch(st.state)
   {
      restart_msg:
      case ST_ANY:
         if(0u != (c & 0x80u))
         {
            // Status byte
            if(0xF0u != (c & 0xF0u))
            {
               st.running_status = c;

               st.ev = beginRecordedEvent();

               st.ev->b_long     = YAC_FALSE;
               st.ev->millisec   = yac_host->yacMilliSeconds();
               st.ev->data_entry = data_entry[c & 0x0Fu];
               st.ev->data.u8[0] = c;

               // Determine size, midimapType, filter, ..
               st.b_filtered = YAC_FALSE;
               st.ev->classifyShortMessage(this, &st.b_filtered);

               if(st.ev->size > 0u)
               {
                  if(1u == st.ev->size)
                  {
                     // Can't happen (not a SysEx realtime message)
                     st.running_status = 0u;
                  }
                  else
                  {
                     // Read more bytes
                     st.state = ST_SHORTMSG1 + (st.ev->size - 2u);
                     st.off = 1u;
                  }
               }
               else
               {
                  // Unknown event type => skip
               }
            }
            else
            {
               if((c & 0x0Fu) < 8u)
               {
                  // SysEx cancels running status
                  st.running_status = 0u;
               }
               else
               {
                  // Single-byte Realtime message (keep running status)
               }

               st.ev = beginRecordedEvent();

               st.ev->millisec = yac_host->yacMilliSeconds();

               if(0xF0u == c)
               {
                  // Start of SysEx message
                  st.b_filtered = b_filter_sysex;

                  st.ev->b_long = YAC_TRUE;
                  st.ev->data.ptr = getNextSysExBuffer();
                  st.ev->midimap_event_type = 7/*MIDIMapDefs.TYPE_SYSEX*/;
                  st.ev->size = 0u;
                  st.off = 0u;

                  // (note) 0xF0 (or 0xF7) are not stored in the message buffer

                  // Parse sysex bytes
                  st.state = ST_SYSEX;
               }
               else
               {
                  if(0xF7u != c)
                  {
                     st.ev->b_long = YAC_FALSE;

                     st.ev->data.u8[0] = c;

                     st.b_filtered = YAC_FALSE;
                     st.ev->classifyShortMessage(this, &st.b_filtered);

                     if(st.ev->size > 1u)
                     {
                        // Continue parsing realtime event (song select, ..)
                        if(st.ev->size <= 3u)
                        {
                           st.state = ST_SHORTMSG1 + (st.ev->size - 2u);
                           st.off = 1u;
                        }
                        else
                        {
                           // (internal error) Unknown event size => skip
                        }
                     }
                     else if(1u == st.ev->size)
                     {
                        // Done, message size is 1 byte
                        if(YAC_FALSE == st.b_filtered)
                        {
                           endRecordedEvent(st.ev);
                        }

                        // (note) stay in state ST_ANY
                     }
                     else
                     {
                        // (internal error) Unknown event type => skip
                     }
                  }
                  else
                  {
                     // Unexpected End of SysEx => skip
                  }
               }
            }
         }
         else
         {
            // Data byte
            if(0u != st.running_status)
            {
               st.ev = beginRecordedEvent();

               st.ev->b_long     = YAC_FALSE;
               st.ev->millisec   = yac_host->yacMilliSeconds();
               st.ev->data_entry = data_entry[c & 0x0Fu];

               st.ev->data.u8[0] = st.running_status;
               st.ev->data.u8[1] = c;

               // Determine size, midimapType, filter, ..
               st.b_filtered = YAC_FALSE;
               st.ev->classifyShortMessage(this, &st.b_filtered);

               if(3u == st.ev->size)
               {
                  // One more byte to go
                  st.state = ST_SHORTMSG1;
                  st.off = 2u;
               }
               else if(2u == st.ev->size)
               {
                  // Done, message size is 2 bytes
                  if(YAC_FALSE == st.b_filtered)
                  {
                     endRecordedEvent(st.ev);
                  }

                  st.state = ST_ANY;
               }
               else
               {
                  // (internal error) Unknown event size => skip
                  st.running_status = 0u;
                  st.state = ST_ANY;
               }
            }
            else
            {
               // Unknown running status, can't parse MIDI byte => skip
            }
         }
         break;

      case ST_SHORTMSG1:
         if(0u != (c & 0x80u))
         {
            // Status byte
            if( (0xF0u == (c & 0xF0u)) && ((c & 0x0Fu) >= 8u) )
            {
               // Single-byte Realtime message in the middle of a shortmsg
               //  (todo) parse/add realtime message, then resume shortmsg
               yac_host->printf("[~~~] midiin_reader: RT msg 0x%02x during shortmsg, ignoring..\n", c);
            }
            else
            {
               yac_host->printf("[---] midiin_reader: unexpected status byte 0x%02x during shortmsg1 %02x, discarding event.\n",
                                c,
                                st.ev->data.u8[0]
                                );

               st.running_status = 0u;
               st.state = ST_ANY;
               goto restart_msg;
            }
         }
         else
         {
            // Received last byte of MIDI short message
            st.ev->data.u8[st.off] = c;

            if(0xB0u == (st.ev->data.u8[0] & 0xF0u))
            {
               st.ev->classifyCtlChange(this, &st.b_filtered);
            }

            if(YAC_FALSE == st.b_filtered)
            {
               st.ev->data_entry = data_entry[st.ev->data.u8[0] & 15u];
               // yac_host->printf("xxx MIDIIn<portmidi>::parseMIDIByte: ch=%u st.ev->data_entry=0x%04x\n", (st.ev->data.u8[0] & 15u), st.ev->data_entry);
               endRecordedEvent(st.ev);
            }

            st.state = ST_ANY;
         }
         break;

      case ST_SHORTMSG2:
         if(0u != (c & 0x80u))
         {
            if( (0xF0u == (c & 0xF0u)) && ((c & 0x0Fu) >= 8u) )
            {
               // Single-byte Realtime message in the middle of a shortmsg
               //  (todo) parse/add realtime message, then resume shortmsg
               yac_host->printf("[~~~] midiin_reader: RT msg 0x%02x during shortmsg, ignoring..\n", c);
            }
            else
            {
               yac_host->printf("[---] midiin_reader: unexpected status byte 0x%02x during shortmsg2 %02x, discarding event.\n",
                                c,
                                st.ev->data.u8[0]
                                );
               st.running_status = 0u;
               st.state = ST_ANY;
               goto restart_msg;
            }
         }
         else
         {
            // Received second last byte
            st.ev->data.u8[st.off++] = c;

            // One more byte to go
            st.state = ST_SHORTMSG1;
         }
         break;

      case ST_SYSEX:
         if(0u != (c & 0x80u))
         {
            // Status byte
            if( (0xF0u == (c & 0xF0u)) && ((c & 0x0Fu) >= 8u) )
            {
               // Single-byte Realtime message in the middle of a sysex transfer
               //  (todo) parse/add realtime message event, then resume sysex
               yac_host->printf("[~~~] midiin_reader: RT msg 0x%02x during sysex transfer, ignoring..\n", c);
            }
            else
            {
               // End of SysEx and start of next message

               if(YAC_FALSE == st.b_filtered)
               {
                  endRecordedEvent(st.ev);
               }
               else if(st.ev->size > MIDIIN_BLOCKSIZE)
               {
                  yac_host->printf("[dbg] midiin_reader: found end of discarded SysEx event (size %u exceeded %u)\n",
                                   st.ev->size,
                                   MIDIIN_BLOCKSIZE
                                   );
               }

               st.state = ST_ANY;

               if(0xF7u != c)
               {
                  goto restart_msg;
               }
            }
         }
         else
         {
            // Data byte
            if(YAC_FALSE == st.b_filtered)
            {
               if(MIDIIN_BLOCKSIZE == st.ev->size)
               {
                  yac_host->printf("[~~~] midiin_reader: SysEx buffer size (%u) exceeded, discarding event.\n",
                                   MIDIIN_BLOCKSIZE
                                   );

                  st.b_filtered = YAC_TRUE;
               }
            }

            if(YAC_FALSE == st.b_filtered)
            {
               st.ev->data.ptr[st.off++] = c;
            }

            st.ev->size++;
         }

         // Parse next SysEx byte (stay in ST_SYSEX)
         break;
   }
}

void MIDIIn::parseBuffer(YAC_Buffer *_buffer) {
   if(YAC_Is_Buffer(_buffer))
   {
      if(NULL == recorded_events[0])
         allocEventsAndMutex();

      sUI num = _buffer->yacStreamGetOffset();
      _buffer->yacStreamSetOffset(0u);
      for(sUI i = 0u; i < num; i++)
      {
         sU8 c = _buffer->yacStreamReadI8();
         parseMIDIByte(c, virt_state);
      }
   }
}
