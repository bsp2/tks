/// MIDIIn.cpp
///
/// (c) 2006-2023 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///
/// changed: 09Sep2006, 02Jan2008, 04Jan2008, 13Jan2008, 09Nov2012, 13Nov2012, 23Mar2013
///          14Apr2013, 23May2013, 04Feb2014, 05Feb2014, 24Feb2014, 20Aug2014, 22Aug2014
///          11Feb2015, 12Aug2017, 19Jan2018, 22Jan2019, 22Jul2023, 01Aug2023
///
///
///

#include <windows.h>

#include <string.h>

#define YAC_BIGSTRING defined
#include <yac.h>

#include "MIDIIn.h"
#include "ying_midi.h"


// #define DP if(0);else printf
#define DP if(1);else printf



// ----------------------------------------------------------------------------
// --                                                                        --
// -- midiInCallback                                                         --
// --                                                                        --
// ----------------------------------------------------------------------------
static void CALLBACK midiInCallback(HMIDIIN handle, UINT uMsg, MIDIIn *thiz, DWORD* dwParam1, DWORD* dwParam2) 
{
   // yac_host->printf("xxx midiInCallback\n");

   sUI tEvent = yac_host->yacMilliSeconds();

   // dwParam2: timestamp (millisec since midiInStart)
   switch(uMsg)
   {
   default:
      DP("\n[trc] MIDIIN: unhandled uMsg=%d\n", uMsg);
      break;

   case MIM_OPEN:
      DP("\n[trc] MIDIIN: MIM_OPEN\n");
      break;

   case MIM_CLOSE:
      DP("\n[trc] MIDIIN: MIM_CLOSE\n");
      break;

   case MIM_DATA:
      {
         //printf("\n[trc] MIDIIN: MIM_DATA. data=0x%08x\n", dwParam1);
         sBool bFiltered;
         sU8 status = (((DWORD)dwParam1)&0xF0);
         sU8 midich = (((DWORD)dwParam1)&0x0F);
         sU8 midimapType = 0; // (note) see MIDIMapEventType.tks for type enumeration
         sU16 nRpn = 0;       // (note) (N)RPN parameter id
         
         // (note) running status is handled by MIDI driver
         // if(!(status & 0x80))
         // {
         //    // Running status
         //    status = running_status;
         // }

         sUI size = 0;
         switch((DWORD)dwParam1)
         {
         default:
            DP("\n[trc] MIDIIN: MIM_DATA. data=0x%08x\n", (sUI)dwParam1);
            bFiltered = 0;
            switch( status )
            {
            case 0x80:
               size = 3;
               bFiltered = thiz->b_filter_note_off;
               midimapType = 0;
               break;

            case 0x90:
               size = 3;
               bFiltered = thiz->b_filter_note_on;
               midimapType = 1;
               break;

            case 0xA0:
               size = 3;
               bFiltered = thiz->b_filter_poly_pressure; // polyphonic aftertouch
               midimapType = 2;
               break;

            case 0xB0:
               size = 3;
               bFiltered = thiz->b_filter_control_change;
               midimapType = 3;

               // Is (N)RPN select or value ?
               if(thiz->b_parse_param[midich])
               {
                  sU8 ccNr  = (sU8) ( (((DWORD)dwParam1)&0x00007F00u) >>  8 );
                  sU8 ccVal = (sU8) ( (((DWORD)dwParam1)&0x007F0000u) >> 16 );

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

                        if(MIDI_DATAENTRYMODE_MSB == thiz->data_entry_mode[midich])
                        {
                           // Synth only supports MSB data, write to _lower_ param bits
                           //  (more convenient than always leaving the lower bits at 0)
                           thiz->data_entry[midich] = (ccVal);
                        }
                        else
                        {
                           // Merge with LSB data
                           thiz->data_entry[midich] = (thiz->data_entry[midich]&0x7f) | (ccVal << 7);
                        }

                        if((MIDI_DATAENTRYMODE_LSBMSB == thiz->data_entry_mode[midich]) ||
                           (MIDI_DATAENTRYMODE_MSB == thiz->data_entry_mode[midich])
                           )
                        {
                           if(-1 != thiz->rpn[midich])
                           {
                              midimapType = 22;
                              nRpn        = thiz->rpn[midich];
                              bFiltered   = thiz->b_filter_rpn;
                           }
                           else if(-1 != thiz->nrpn[midich])
                           {
                              midimapType = 23;
                              nRpn        = thiz->nrpn[midich];
                              bFiltered   = thiz->b_filter_nrpn;
                           }
                        }
                        else
                        {
                           // Wait for LSB
                           bFiltered = 1;
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

                           if(MIDI_DATAENTRYMODE_MSBLSB == thiz->data_entry_mode[midich])
                           {
                              if(-1 != thiz->rpn[midich])
                              {
                                 midimapType = 22;
                                 nRpn        = thiz->rpn[midich];
                                 bFiltered   = thiz->b_filter_rpn;
                              }
                              else if(-1 != thiz->nrpn[midich])
                              {
                                 midimapType = 23;
                                 nRpn        = thiz->nrpn[midich];
                                 bFiltered   = thiz->b_filter_nrpn;
                              }
                           }
                           else
                           {
                              // Wait for MSB (DATAENTRYMODE_LSBMSB)
                              bFiltered = 1;
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
                        bFiltered = thiz->b_filter_rpn_select;
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
                        bFiltered = thiz->b_filter_rpn_select;
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
                        bFiltered = thiz->b_filter_nrpn_select;
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
                        bFiltered = thiz->b_filter_nrpn_select;
                        break;
                  }
               }
               break;

            case 0xC0:
               size = 2;
               bFiltered = thiz->b_filter_program_change;
               midimapType = 4;
               break;

            case 0xD0:
               size = 2;
               bFiltered = thiz->b_filter_channel_pressure; // aftertouch
               midimapType = 5;
               break;

            case 0xE0:
               size = 3;
               bFiltered = thiz->b_filter_pitch_bend;
               midimapType = 6;
               break;

            case 0xF0:
               // whoops?! should be in a long message
               size = 0;
               midimapType = 7;
               break;
            }
            break;

         case 0xF1:
            bFiltered = thiz->b_filter_cm_time_code;
            // TODO
            size = 0;
            midimapType = 8;
            break;

         case 0xF2:
            bFiltered = thiz->b_filter_cm_song_position;
            size = 3;
            midimapType = 9;
            break;

         case 0xF3:
            bFiltered = thiz->b_filter_cm_song_select;
            size = 2;
            midimapType = 10;
            break;

         case 0xF4:
            // undefined
            bFiltered = 1;
            size = 1;
            midimapType = 11;
            break;

         case 0xF5:
            // undefined
            bFiltered = 1;
            size = 1;
            midimapType = 12;
            break;

         case 0xF6:
            bFiltered = thiz->b_filter_cm_tune_request;
            size = 1;
            midimapType = 13;
            break;

         case 0xF8:
            bFiltered = thiz->b_filter_rt_timing_clock;
            size = 1;
            midimapType = 14;
            break;
            
         case 0xF9:
            // undefined
            bFiltered = 1;
            size = 1;
            midimapType = 15;
            break;

         case 0xFA:
            bFiltered = thiz->b_filter_rt_start;
            size = 1;
            midimapType = 16;
            break;
            
         case 0xFB:
            bFiltered = thiz->b_filter_rt_continue;
            size = 1;
            midimapType = 17;
            break;
            
         case 0xFC:
            bFiltered = thiz->b_filter_rt_stop;
            size = 1;
            midimapType = 18;
            break;
            
         case 0xFD:
            // undefined
            bFiltered = 1;
            size = 1;
            midimapType = 19;
            break;

         case 0xFE:
            bFiltered = thiz->b_filter_rt_active_sensing;
            size = 1;
            midimapType = 20;
            break;
            
         case 0xFF:
            bFiltered = thiz->b_filter_rt_system_reset;
            size = 1;
            midimapType = 21;
            break;
         }

         if( !bFiltered && (size>0) )
         {
            ////thiz->addRecordedShortEvent((sUI)dwParam2/*millisec*/, (sUI)dwParam1, size, midimapType, nRpn);
            thiz->addRecordedShortEvent(tEvent, (sUI)dwParam1, size, midimapType, nRpn);
         }
      }
      break;

   case MIM_LONGDATA:
      {
         DP("\n[trc] MIDIIN: MIM_LONGDATA dev=\"%s\"\n", (const char*)thiz->device_name.chars);
         MIDIHDR *hdr = (MIDIHDR*) dwParam1;
         sU8 *d = (sU8*) hdr->lpData;
         sUI num = hdr->dwBytesRecorded;
         DP("   -> adr=%p, num=%d\n", d, num);

         if(thiz->prepareNextBuffer())
         {
            if(!thiz->b_filter_sysex)
            {
               ////thiz->addRecordedLongEvent((sUI)dwParam2/*millisec*/, (MIDIHDR*)dwParam1);
               thiz->addRecordedLongEvent(tEvent, (MIDIHDR*)dwParam1);
            }
         }
      }
      break;

   case MIM_MOREDATA:
      DP("\n[trc] MIDIIN: MIM_MOREDATA\n");
      break;

   case MIM_ERROR:
      DP("\n[trc] MIDIIN: MIM_ERROR\n");
      break;

   case MIM_LONGERROR:
      DP("\n[trc] MIDIIN: MIM_LONGERROR\n");
      break;
   }
}


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

sSI RecordedMIDIEvent::isLongMessage(void) {
   return b_long;
}

sSI RecordedMIDIEvent::getSize(void) {
   return (sSI) size;
}

sSI RecordedMIDIEvent::getShortMessage(void) {
   if(b_long)
   {
      return 0;
   }
   else
   {
      return (sSI) data.msg;
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
         _stream->yacStreamWrite((sU8*)&data.msg, size);
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
   // Called by parseBuffer() (LocalMIDI mode)
   switch(data.bytes[0] & 0xF0u)
   {
      default:
         size = 0;
         *bFiltered = YAC_TRUE;
         break;

      case 0x80:
         size = 3;
         *bFiltered = thiz->b_filter_note_off;
         midimap_event_type = 0;
         break;
         
      case 0x90:
         size = 3;
         *bFiltered = thiz->b_filter_note_on;
         midimap_event_type = 1;
         break;
         
      case 0xA0:
         size = 3;
         *bFiltered = thiz->b_filter_poly_pressure; // polyphonic aftertouch
         midimap_event_type = 2;
         break;
         
      case 0xB0:
         size = 3;
         *bFiltered = thiz->b_filter_control_change;
         midimap_event_type = 3;

         // also see classifyCtlChange()
         break;
         
      case 0xC0:
         size = 2;
         *bFiltered = thiz->b_filter_program_change;
         midimap_event_type = 4;
         break;
         
      case 0xD0:
         size = 2;
         *bFiltered = thiz->b_filter_channel_pressure; // aftertouch
         midimap_event_type = 5;
         break;

      case 0xE0:
         size = 3;
         *bFiltered = thiz->b_filter_pitch_bend;
         midimap_event_type = 6;
         break;

      case 0xF0:
         switch(data.bytes[0])
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
               midimap_event_type = 8;
               break;

            case 0xF2:
               *bFiltered = thiz->b_filter_cm_song_position;
               size = 3;
               midimap_event_type = 9;
               break;

            case 0xF3:
               *bFiltered = thiz->b_filter_cm_song_select;
               size = 2;
               midimap_event_type = 10;
               break;

            case 0xF4:
               // undefined
               *bFiltered = YAC_TRUE;
               size = 1;
               midimap_event_type = 11;
               break;

            case 0xF5:
               // undefined
               *bFiltered = YAC_TRUE;
               size = 1;
               midimap_event_type = 12;
               break;

            case 0xF6:
               *bFiltered = thiz->b_filter_cm_tune_request;
               size = 1;
               midimap_event_type = 13;
               break;

            case 0xF8:
               *bFiltered = thiz->b_filter_rt_timing_clock;
               size = 1;
               midimap_event_type = 14;
               break;
            
            case 0xF9:
               // undefined
               *bFiltered = YAC_TRUE;
               size = 1;
               midimap_event_type = 15;
               break;

            case 0xFA:
               *bFiltered = thiz->b_filter_rt_start;
               size = 1;
               midimap_event_type = 16;
               break;
            
            case 0xFB:
               *bFiltered = thiz->b_filter_rt_continue;
               size = 1;
               midimap_event_type = 17;
               break;
            
            case 0xFC:
               *bFiltered = thiz->b_filter_rt_stop;
               size = 1;
               midimap_event_type = 18;
               break;
            
            case 0xFD:
               // undefined
               *bFiltered = YAC_TRUE;
               size = 1;
               midimap_event_type = 19;
               break;

            case 0xFE:
               *bFiltered = thiz->b_filter_rt_active_sensing;
               size = 1;
               midimap_event_type = 20;
               break;
            
            case 0xFF:
               *bFiltered = thiz->b_filter_rt_system_reset;
               size = 1;
               midimap_event_type = 21;
               break;
         }
         break;
   }

}

void RecordedMIDIEvent::classifyCtlChange(MIDIIn *thiz, sBool *bFiltered) {
   // Called by parseBuffer() (LocalMIDI mode)
   // Called when event has been fully received

   // Is (N)RPN select or value ?
   sU8 midich = data.bytes[0] & 0x0Fu;
   sU8 ccNr   = data.bytes[1];
   sU8 ccVal  = data.bytes[2];
   
   DP("[trc] MIDIIn<portmidi>::RecordedMIDIEvent::classifyCtlChange: ch=%u ccNr=%u ccVal=%u b_parse=%d\n", midich, ccNr, ccVal, thiz->b_parse_param[midich]);

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
   midiin_handle = 0;
   b_open = YAC_FALSE;
   device_idx = -1;

   record_readoff   = 0u;
   record_writeoff  = 0u;
   record_numevents = 0u;
   {
      sUI i;
      for(i=0; i<MIDIIN_MAXEVENTS; i++)
      {
         recorded_events[i] = NULL;
      }
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

   for(sUI i = 0u; i < 16u; i++)
   {
      b_parse_param[i] = YAC_TRUE;
      rpn [i] = -1;
      nrpn[i] = -1;
      data_entry[i] = 0u;
      data_entry_mode[i] = MIDI_DATAENTRYMODE_MSBLSB; // MSB,LSB order
   }

   // // running_status = 0;

   event_recv = ::CreateEvent(NULL, 1/*_bManualReset*/, 0, NULL);

   virt_state.init();
}


MIDIIn::~MIDIIn() {
   _close();

   ::CloseHandle(event_recv);
   event_recv = NULL;

   freeEventsAndMutex();  // in case device was not opened and parseBuffer() was used
}

sBool MIDIIn::isOpen(void) {
   return (NULL != midiin_handle);
}

void MIDIIn::initMidiBuffers(void) {
   memset(midi_buffer_data, 0, MIDIIN_NUMBUFFERS*MIDIIN_BLOCKSIZE);
   for(int i=0; i<MIDIIN_NUMBUFFERS; i++)
   {
      MIDIHDR *hdr = &midi_buffers[i];
      memset(hdr, 0, sizeof(MIDIHDR));
      hdr->dwBufferLength = MIDIIN_BLOCKSIZE;
      hdr->lpData = (char*) (midi_buffer_data + (i*MIDIIN_BLOCKSIZE));
   }
   curr_buffer_index = 0;
}

void MIDIIn::unprepareAllHeaders(void) {
   for(int i=0; i<MIDIIN_NUMBUFFERS; i++)
   {
      MIDIHDR *hdr = &midi_buffers[i];
      if( (hdr->dwFlags&MHDR_PREPARED)==MHDR_PREPARED )
      {
         midiInUnprepareHeader(midiin_handle, hdr, sizeof(MIDIHDR));
      }
   }
}

void MIDIIn::addRecordedLongEvent(sUI _millisec, MIDIHDR *_hdr) {
   // Called by MIDI callback function

   if(NULL != mtx_record)
      yac_host->yacMutexLock(mtx_record);

   RecordedMIDIEvent *ev = recorded_events[record_writeoff];

   ev->b_long   = 1;
   ev->millisec = _millisec;
   ev->size     = (sUI) (_hdr->dwBytesRecorded - 2); // -2 to remove 0xF0 .. 0xF7
   ev->data.ptr = ((sU8*) _hdr->lpData) + 1; // skip 0xF0
   ev->midimap_event_type = 7/*MIDIMapDefs.TYPE_SYSEX*/;

   record_writeoff = (record_writeoff +1) % MIDIIN_MAXEVENTS;
   record_numevents++;
   if(record_numevents > MIDIIN_MAXEVENTS)
   {
      // Overflow..
      record_numevents = MIDIIN_MAXEVENTS;
   }

   ::SetEvent(event_recv);

   if(NULL != mtx_record)
      yac_host->yacMutexUnlock(mtx_record);
}

void MIDIIn::addRecordedShortEvent(sUI _millisec, sUI _msg, sUI _size, sU8 _midimapEventType, sU16 _nRpn) {
   // Called by MIDI callback function

   if(NULL != mtx_record)
      yac_host->yacMutexLock(mtx_record);

   RecordedMIDIEvent *ev = recorded_events[record_writeoff];

   ev->b_long   = 0;
   ev->millisec = _millisec;
   ev->size     = _size;
   ev->data.msg = _msg;
   ev->midimap_event_type = _midimapEventType;
   ev->n_rpn    = _nRpn;
   ev->data_entry = data_entry[_msg & 0x0000000Fu];

   record_writeoff = (record_writeoff +1) % MIDIIN_MAXEVENTS;
   record_numevents++;
   if(record_numevents > MIDIIN_MAXEVENTS)
   {
      // Overflow..
      record_numevents = MIDIIN_MAXEVENTS;
   }

   ::SetEvent(event_recv);

   if(NULL != mtx_record)
      yac_host->yacMutexUnlock(mtx_record);
}

YAC_Object *MIDIIn::_getNextEvent(void) {
   YAC_Object *r;
   sBool bOpen = isOpen();

   // Return RecordedMIDIEvent* or NULL if no event is available
   if(NULL != mtx_record)
      yac_host->yacMutexLock(mtx_record);

   if(record_numevents > 0u)
   {
      r = recorded_events[record_readoff]; // RecordedMIDIEvent object
      record_readoff = (record_readoff + 1u) % MIDIIN_MAXEVENTS;
      record_numevents--;

      if(bOpen && 0u == record_numevents)
      {
         ::ResetEvent(event_recv);
      }
   }
   else if(bOpen)
   {
      ::ResetEvent(event_recv);
      r = NULL;
   }
   if(NULL != mtx_record) {
      yac_host->yacMutexUnlock(mtx_record);
   }
   return r;
}

YAC_Object *MIDIIn::_waitNextEvent(sUI _timeout) {
   if(isOpen())
   {
      ::WaitForSingleObject(event_recv, _timeout);
      
      return _getNextEvent();
   }
   else
   {
      yac_host->printf("[---] MIDIIn::waitNextEvent: device is not open!\n");
   }

   return NULL;
}

sSI MIDIIn::_getNumEvents(void) {
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

   record_readoff = 0u;
   record_writeoff = 0u;
   record_numevents = 0u;

   if(NULL != mtx_record)
      yac_host->yacMutexUnlock(mtx_record);
}

////static sUI total_allocs = 0;
sUI MIDIIn::allocBufferIndex(void) {
   midi_buffers[curr_buffer_index].dwBytesRecorded = 0;
   ////yac_host->printf("xxx MIDIIn::allocBufferIndex: Alloc Buffer %i (%i total allocs)\n", curr_buffer_index, total_allocs++);
   sUI r = curr_buffer_index;
   curr_buffer_index = (curr_buffer_index + 1u) % MIDIIN_NUMBUFFERS;
   return r;
}

sU8 *MIDIIn::getBufferData(sSI _idx) {
   return (sU8*) midi_buffers[_idx].lpData;
}

sUI MIDIIn::getBufferSize(sSI _idx) {
   return (sUI) midi_buffers[_idx].dwBytesRecorded;
}

sBool MIDIIn::receiveBufferIndex(sSI _idx) {
   MMRESULT res;
   char midiErr[1024];
   if( (midi_buffers[_idx].dwFlags&MHDR_PREPARED)==MHDR_PREPARED )
   {
      if( (res=midiInUnprepareHeader(midiin_handle, &midi_buffers[_idx], sizeof(MIDIHDR))) != MMSYSERR_NOERROR)
      {
         midiInGetErrorText(res, midiErr, 1024);
         yac_host->printf("[---] MIDIIn::receiveBufferIndex(%i) failed (midiInUnprepareHeader: \"%s\").\n", _idx, midiErr);
         return YAC_FALSE;
      }
   }
   memset(&midi_buffer_data[_idx*MIDIIN_BLOCKSIZE], 0, MIDIIN_BLOCKSIZE);
   if( (res=midiInPrepareHeader(midiin_handle, &midi_buffers[_idx], sizeof(MIDIHDR))) == MMSYSERR_NOERROR)
   {
      if( (res=midiInAddBuffer(midiin_handle, &midi_buffers[_idx], sizeof(MIDIHDR)))  == MMSYSERR_NOERROR)
      {
         ///yac_host->printf("[+++] buffer added.\n");
         return YAC_TRUE;
      }
      else
      {
         midiInGetErrorText(res, midiErr, 1024);
         yac_host->printf("[---] MIDIIn::receiveBufferIndex(%i) failed (midiInAddBuffer: \"%s\").\n", _idx, midiErr);
      }
   }
   else
   {
      midiInGetErrorText(res, midiErr, 1024);
      yac_host->printf("[---] MIDIIn::receiveBufferIndex(%i) failed (midiInPrepareHeader: \"%s\").\n", _idx, midiErr);
   }
   return YAC_FALSE;
}

sSI MIDIIn::_prepareSysEx(void) {
   // // midiInReset(midiin_handle);
   sSI idx = allocBufferIndex();
   if(receiveBufferIndex(idx))
   {
      midiInStop(midiin_handle);
      midiInStart(midiin_handle);
      return idx;
   }
   return -1;
}

sBool MIDIIn::_receiveSysEx(sSI _idx, YAC_Object *_rcvBuf) {
   if( (_idx>=0) && (_idx<8) )
   {
      if(YAC_VALID(_rcvBuf))
      {
         if(YAC_CHK(_rcvBuf, YAC_CLID_BUFFER))
         {
            YAC_Buffer *buf = (YAC_Buffer*) _rcvBuf;
            Sleep(30);
            midiInStop(midiin_handle);
            sU8 *s = getBufferData(_idx);
            sUI num = getBufferSize(_idx);
            if(num>0)
            {
               yac_host->printf("receiveSysEx: num=%i\n", num);
               if(buf->size >= num)
               {
                  sUI i;
                  for(i=0; i<num; i++)
                  {
                     buf->buffer[i] = s[i];
                  }
                  for(; i<buf->size; i++)
                  {
                     buf->buffer[i] = 0;
                  }
                  buf->io_offset = num;
                  return 1;
               }
               else
               {
                  yac_host->printf("[---] MIDIIn::receiveSysEx: buffer size is smaller than received packet (%i<%i).\n",
                     buf->size, num);
               }
            }
         }
      }
   }
   return 0;
}

void MIDIIn::_reset(void) {
   midiInReset(midiin_handle);
   _clearEvents();
}

sBool MIDIIn::prepareNextBuffer(void) {
   sBool ret = YAC_FALSE;

   if(b_open)
   {
      sSI idx = allocBufferIndex();
      ret = receiveBufferIndex(idx);
      //printf("xxx MIDIIn::prepareNextBuffer\n");
   }

   return ret;
}

void MIDIIn::_start(void) {
   _clearEvents();
   midiInReset(midiin_handle);
   midiInStart(midiin_handle);
   prepareNextBuffer();
}

void MIDIIn::_stop(void) {
   midiInReset(midiin_handle);
   midiInStop(midiin_handle);
}

sBool MIDIIn::_isAlive(void) {
   MIDIINCAPS caps;

   if(MMSYSERR_NOERROR == midiInGetDevCaps(device_idx, &caps, sizeof(caps)))
   {
      // // DWORD dwFlags = 0;
      // // if(GetHandleInformation(midiin_handle, &dwFlags))
      // // {
      // // }
      // // else
      // // {
      // //    yac_host->printf("[---] MIDIIn::isAlive: GetHandleInformation failed.\n");
      // // }

      HMIDIIN hMidiIn;
      MMRESULT mmres = midiInOpen(&hMidiIn, device_idx, NULL/*cbk*/, (DWORD) 0, CALLBACK_NULL);
      if(mmres == MMSYSERR_NOERROR)
      {
         midiInClose(hMidiIn);
         // // yac_host->printf("[---] MIDIIn::isAlive: midiInOpen OK.\n");
      }
      else
      {
         // mmres=0x4 (MMSYSERR_ALLOCATED) : device exists but cannot be openend multiple times
         // mmres=0x3 (MMSYSERR_NOTENABLED): device does not exist
         // // yac_host->printf("[---] MIDIIn::isAlive: midiInOpen failed. mmres=%08x\n", mmres);
         if(0x3 == mmres)
         {
            return 0;
         }
      }

      return 1;
   }
   else
   {
      return 0;
   }
}


sBool MIDIIn::_openByName(YAC_Object *_devName) {
   _close();

   if(YAC_VALID(_devName))
   {
      if(YAC_Is_String(_devName))
      {
         YAC_String *devName = (YAC_String*) _devName;
         const char *MIDIIn_device_name = (const char*) devName->chars;

         UINT numDevs = midiInGetNumDevs();

         for(UINT i=0; i<numDevs; i++)
         {
            if(MMSYSERR_NOERROR == midiInGetDevCaps(i, &midiin_caps, sizeof(midiin_caps)))
            {
               if(yac_host->yacGetDebugLevel() > 1)
               {
                  yac_host->printf("[trc] MIDIIn[%i]: name=\"%s\"\n", 
                                   i,
                                   midiin_caps.szPname 
                                   );
               }

               if(!strcmp(midiin_caps.szPname, MIDIIn_device_name))
               {
                  if(yac_host->yacGetDebugLevel() > 1)
                  {
                     yac_host->printf("*FOUND*\n");
                  }

                  if(MMSYSERR_NOERROR == midiInOpen(&midiin_handle, i, (DWORD_PTR)&midiInCallback, (DWORD_PTR) this, CALLBACK_FUNCTION))
                  {
                     if(yac_host->yacGetDebugLevel() > 1)
                     {
                        yac_host->printf("[trc] MIDIIn::open: MIDI device \"%s\" opened.\n", MIDIIn_device_name);
                     }

                     device_name.copy((sChar*)MIDIIn_device_name);

                     device_idx = (sSI)i;
                     
                     open2();

                     return YAC_TRUE;
                  }
                  else
                  {
                     yac_host->printf("[---] MIDIIn::open: failed to open MIDI device \"%s\".\n", MIDIIn_device_name);

                     return YAC_FALSE;
                  }
               }
            }
         }

         if(yac_host->yacGetDebugLevel() > 1)
         {
            yac_host->printf("[---] MIDIIn::open: MIDI device \"%s\" not present.\n", MIDIIn_device_name);
         }

         return YAC_FALSE;
      }
   }

   return YAC_FALSE;
}

void MIDIIn::_getDeviceName(YAC_Value *_r) {
   YAC_String *s = (YAC_String *) yac_host->yacNewByID(YAC_CLID_STRING);
   s->yacCopy(&device_name);
   _r->initString(s, 1);
}

sSI MIDIIn::getDeviceIdx(void) {
   if(b_open)
   {
      return device_idx;
   }
   else
   {
      return -1;
   }
}

sBool MIDIIn::_openByIdx(sUI _idx) {
   _close();

   if(midiInOpen(&midiin_handle, _idx, NULL, NULL, CALLBACK_NULL) == MMSYSERR_NOERROR)
   {
      if(midiInGetDevCaps(_idx, &midiin_caps, sizeof(midiin_caps)) == MMSYSERR_NOERROR)
      {
         device_name.copy((sChar*)midiin_caps.szPname);

         device_idx = _idx;

         yac_host->printf("[...] MIDIIn::open: MIDI device #%u \"%s\" opened.\n", _idx, midiin_caps.szPname);

         open2();
        
         return YAC_TRUE;
      }
      else
      {
         yac_host->printf("[---] MIDIIn::open: failed to query dev caps for MIDIIn device #%u.\n", _idx);
      }

   }
   else
   {
      yac_host->printf("[---] MIDIIn::open: failed to open MIDI device #%u.\n", _idx);
   }

   return YAC_FALSE;
}

void MIDIIn::_openLocalMIDI(void) {
   // pseudo-open function for shared memory STX_LocalMIDIInDevice
   _close();
   open2();
   b_open = YAC_FALSE;
}

void MIDIIn::open2(void) {
   initMidiBuffers();
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
   if(midiin_handle)
   {
      b_open = YAC_FALSE;

      midiInStop(midiin_handle);
      midiInReset(midiin_handle);
      unprepareAllHeaders();
      midiInReset(midiin_handle);
      midiInClose(midiin_handle);
      midiin_handle = NULL;

      freeEventsAndMutex();
   }
}

sSI MIDIIn::GetNumDevices(void) {
   return (sSI) midiInGetNumDevs();
}

void MIDIIn::GetDeviceNameByIdx(sUI _idx, YAC_Value *_r) {

   if(MMSYSERR_NOERROR == midiInGetDevCaps(_idx, &midiin_caps, sizeof(midiin_caps)))
   {
      YAC_String *s = YAC_New_String();
      s->copy(midiin_caps.szPname);
      _r->initString(s, YAC_TRUE);
   }
   else
   {
      _r->initNull();
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

// sSI MIDIIn::getRpn(void) {
//    return rpn;
// }

// sSI MIDIIn::getNrpn(void) {
//    return nrpn;
// }

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
               st.ev->data.bytes[0] = c;

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

                     st.ev->data.bytes[0] = c;

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

               st.ev->data.bytes[0] = st.running_status;
               st.ev->data.bytes[1] = c;

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
                                st.ev->data.bytes[0]
                                );

               st.running_status = 0u;
               st.state = ST_ANY;
               goto restart_msg;
            }
         }
         else
         {
            // Received last byte of MIDI short message
            st.ev->data.bytes[st.off] = c;

            if(0xB0u == (st.ev->data.bytes[0] & 0xF0u))
            {
               st.ev->classifyCtlChange(this, &st.b_filtered);
            }

            if(YAC_FALSE == st.b_filtered)
            {
               st.ev->data_entry = data_entry[st.ev->data.bytes[0] & 15u];
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
                                st.ev->data.bytes[0]
                                );
               st.running_status = 0u;
               st.state = ST_ANY;
               goto restart_msg;
            }
         }
         else
         {
            // Received second last byte
            st.ev->data.bytes[st.off++] = c;

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

RecordedMIDIEvent *MIDIIn::beginRecordedEvent(void) {
   // Called by parseBuffer() (LocalMIDI mode)

   if(NULL != mtx_record)
      yac_host->yacMutexLock(mtx_record);

   RecordedMIDIEvent *ev = recorded_events[record_writeoff];

   if(NULL != mtx_record)
      yac_host->yacMutexUnlock(mtx_record);

   return ev;
}

void MIDIIn::endRecordedEvent(RecordedMIDIEvent *_ev) {
   // Called by parseBuffer() (LocalMIDI mode)

   if(NULL != mtx_record)
      yac_host->yacMutexLock(mtx_record);

   if(_ev == recorded_events[record_writeoff])
   {
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
   // Called by parseBuffer() (LocalMIDI mode)
   sU8 *ret = (midi_buffer_data + (curr_buffer_index * MIDIIN_BLOCKSIZE));
   curr_buffer_index = (curr_buffer_index + 1u) % MIDIIN_NUMBUFFERS;
   return ret;
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
