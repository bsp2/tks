/// MIDIIn.cpp
///
/// (c) 2006-2023 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///
/// changed: 09Sep2006, 02Jan2008, 04Jan2008, 13Jan2008, 09Nov2012, 13Nov2012, 23Mar2013
///          14Apr2013, 23May2013, 04Feb2014, 05Feb2014, 09Feb2014, 11Feb2014, 18Aug2014
///          22Aug2014, 23Aug2014, 11Feb2015, 29Mar2019, 18Jan2023, 22Jul2023, 01Aug2023
///
///
///

#include <string.h>
#include <pthread.h>

#define YAC_BIGSTRING defined
#include <yac.h>

#ifdef YAC_LINUX
#include <unistd.h>
#include <syscall.h>
#endif

#include "tkmidi.h"
#include "MIDIIn.h"

#include "ying_midi.h"

//#define ENABLE_DEBUG defined
//#define ENABLE_TRACE defined

//#define DP if(0);else printf
#define DP if(1);else printf


/// Define to simulate random MIDI input (test MIDI parser)
//#define STRESSTEST_RANDOM defined



/*
 * /etc/security/limits.conf:
 *  <username> - rtprio 95
 *  <username> - memlock 512000
 *  <username> - nice -19
 *
 *  # ulimit -r unlimited
 */
static sBool b_customsched_init = YAC_FALSE;
static pthread_attr_t custom_sched_attr;
static int fifo_max_prio, fifo_min_prio;
static struct sched_param fifo_param;

static void *midiin_reader__threadentry(void *_MIDIIn) {
   MIDIIn *midiin = (MIDIIn*)_MIDIIn;
   midiin_reader_state_t readerState;
   readerState.init();

#ifdef YAC_LINUX
   yac_host->printf("[dbg] MIDIIn<alsa><%s>::threadentry: threadId=" YAC_PRINTF_PTRPREFIX "%p LWPid=%d\n",
                    (const char*)midiin->device_name.chars,
                    pthread_self(),
                    syscall(SYS_gettid)
                    );
#else
   yac_host->printf("[dbg] MIDIIn<alsa><%s>::threadentry: threadId=" YAC_PRINTF_PTRPREFIX "%p\n",
                    (const char*)midiin->device_name.chars,
                    pthread_self()
                    );
#endif

   midiin->b_thread_busy = 0;

   while(midiin->b_running)
   {
      sU8 c;

#ifdef STRESSTEST_RANDOM
      int err  = 0;
      c = (sU8) (rand()&255);
#else
      // Get next MIDI byte (blocking read)
      // yac_host->printf("xxx MIDIIn<alsa>: call snd_rawmidi_read()\n", c);
      int err = snd_rawmidi_read(midiin->midiin_handle, (void*)&c, 1);
#endif

      if(err >= 0)
      {
         // yac_host->printf("xxx MIDIIn<alsa>: snd_rawmidi_read() returned c=0x%02x\n", c);

         midiin->b_thread_busy = YAC_TRUE;

         midiin->parseMIDIByte(c, readerState);

         midiin->b_thread_busy = YAC_FALSE;

      }
      else
      {
         yac_host->printf("[~~~] midiin_reader: snd_rawmidi_read() failed (err=%d).\n", err);
      }
   }

   return NULL;
}



RecordedMIDIEvent::RecordedMIDIEvent(void) {
   millisec = 0;
   b_long   = YAC_FALSE;
   data.ptr = NULL;
   size     = 0;
   midimap_event_type = 0;
}

RecordedMIDIEvent::~RecordedMIDIEvent() {
}

sUI RecordedMIDIEvent::yacArrayGetNumElements(void) {
   return size;
}

void RecordedMIDIEvent::yacArrayGet(void *_context, sUI _index, YAC_Value *_r) {
   if(size > 0)
   {
      if(_index < size)
      {
         if(b_long)
         {
            _r->initInt((sSI)data.ptr[_index]);
         }
         else
         {
            _r->initInt((sSI)data.msg[_index]);
         }
         return;
      }
   }
   _r->initVoid();
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

sUI RecordedMIDIEvent::getShortMessage(void) {
   if(b_long)
   {
      return 0;
   }
   else
   {
      return data.msg32;
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
   switch(data.msg[0] & 0xF0u)
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
         switch(data.msg[0])
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
   sU8 midich = data.msg[0] & 0x0Fu;
   sU8 ccNr   = data.msg[1];
   sU8 ccVal  = data.msg[2];

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
               // Wait for LSB
               *bFiltered = YAC_TRUE;
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



AlsaDeviceInfo *MIDIIn::alsa_device_infos = NULL;
sUI MIDIIn::num_alsa_device_infos = 0;

// see <http://www.alsa-project.org/alsa-doc/alsa-lib/>
// see <http://www.alsa-project.org/alsa-doc/alsa-lib/group___raw_midi.html>
// also see: <http://home.roadrunner.com/~jgglatt/tech/arawmidrec.htm>

void MIDIIn::AllocAlsaDeviceInfos(void) {
   alsa_device_infos = NULL;
   num_alsa_device_infos = 0;
   int cardNr = -1;

   AlsaDeviceInfo *linfo = NULL;

   /* Iterate sound cards */
   for(;;)
   {
      snd_ctl_t *cardCtl;
      int err;

      /* Query next (or first) sound card */
      err = snd_card_next(&cardNr);

      if(err < 0)
      {
         yac_host->printf("[---] MIDIIn::AllocAlsaDeviceInfos: snd_card_next() failed (err=%d)\n", err);
         break;
      }

#ifdef ENABLE_TRACE
      yac_host->printf("[trc] MIDIIn::AllocAlsaDeviceInfos: snd_card_next() returned cardNr=%d.\n", cardNr);
#endif

      if(cardNr < 0)
      {
         /* No more cards to enumerate */
         break;
      }

      /* Open sound card ctl */
      char cardStr[64];
      sprintf(cardStr, "hw:%d", cardNr);
      err = snd_ctl_open(&cardCtl, cardStr, 0);

      if(err < 0)
      {
         yac_host->printf("[---] MIDIIn::AllocAlsaDeviceInfos: snd_ctl_open(cardStr=\"%s\") failed (err=%d)\n",
                          cardStr,
                          err
                          );

         /* Try next card */
         continue;
      }

      /* Enumerate card devices */
      int devNr = -1;

      for(;;)
      {
         /* Query next (or first) rawmidi device for current card */
         err = snd_ctl_rawmidi_next_device(cardCtl, &devNr);

         if(err < 0)
         {
            yac_host->printf("[---] MIDIIn::AllocAlsaDeviceInfos: snd_ctl_rawmidi_next_device(cardStr=\"%s\" devNr=%d) failed (err=%d)\n",
                             cardStr,
                             devNr,
                             err
                             );

            break;
         }

#ifdef ENABLE_TRACE
         yac_host->printf("[trc] MIDIIn::AllocAlsaDeviceInfos: snd_ctl_rawmidi_next_device(cardStr=\"%s\") returned devNr=%d.\n",
                          cardStr,
                          devNr
                          );
#endif

         if(devNr < 0)
         {
            /* No more devices to enumerate */
            break;
         }

         /* Allocate and initialize rawMidiInfo struct */
         snd_rawmidi_info_t *rawMidiInfo;
         snd_rawmidi_info_alloca(&rawMidiInfo);
         memset(rawMidiInfo, 0, snd_rawmidi_info_sizeof());
         snd_rawmidi_info_set_device(rawMidiInfo, devNr);
         snd_rawmidi_info_set_stream(rawMidiInfo, SND_RAWMIDI_STREAM_OUTPUT);
         /*xxx*/snd_rawmidi_info_set_subdevice(rawMidiInfo, 0);

         err = snd_ctl_rawmidi_info(cardCtl, rawMidiInfo);

         if(err < 0)
         {
            yac_host->printf("[---] MIDIIn::AllocAlsaDeviceInfos: snd_ctl_rawmidi_info(\"%s,%d,0\") failed (err=%d)\n",
                             cardStr,
                             devNr
                             );

            /* Try next device */
            continue;
         }

         const char *hwDrvName = snd_rawmidi_info_get_name(rawMidiInfo);

#ifdef ENABLE_DEBUG
         yac_host->printf("[dbg] MIDIIn::AllocAlsaDeviceInfos: snd_rawmidi_info_get_name(cardStr=\"%s,%d\") returned hwDrvName=\"%s\".\n",
                          cardStr,
                          devNr,
                          hwDrvName
                          );
#endif

         int subDevNr = 0;
         int subDevNum = snd_rawmidi_info_get_subdevices_count(rawMidiInfo);

#ifdef ENABLE_TRACE
         yac_host->printf("[trc] MIDIIn::AllocAlsaDeviceInfos: snd_rawmidi_info_get_card(cardStr=\"%s,%d\") returned cardNr=%d.\n",
                          cardStr,
                          devNr,
                          snd_rawmidi_info_get_card(rawMidiInfo)
                          );
#endif

#ifdef ENABLE_TRACE
         yac_host->printf("[trc] MIDIIn::AllocAlsaDeviceInfos: snd_rawmidi_info_get_subdevices_count(cardStr=\"%s,%d\") returned subDevNum=%d.\n",
                          cardStr,
                          devNr,
                          subDevNum
                          );
#endif

         while(subDevNr < subDevNum)
         {
            snd_rawmidi_info_set_subdevice(rawMidiInfo, subDevNr);

            err = snd_ctl_rawmidi_info(cardCtl, rawMidiInfo);

            if(err < 0)
            {
               yac_host->printf("[---] MIDIIn::AllocAlsaDeviceInfos: snd_ctl_rawmidi_info(\"%s,%d,%d\") failed (err=%d)\n",
                                cardStr,
                                devNr,
                                subDevNr
                                );

               /* Try next sub-device */
               continue;
            }
            else
            {
#ifdef ENABLE_TRACE
               const char *subDevName = snd_rawmidi_info_get_subdevice_name(rawMidiInfo);
               yac_host->printf("[dbg] MIDIIn::AllocAlsaDeviceInfos: found rawmidi output device \"%s,%d,%d\" (devInfoIdx=%u).\n",
                                cardStr,
                                devNr,
                                subDevNr,
                                num_alsa_device_infos
                                );

               yac_host->printf("[dbg]    subdevice_name: \"%s\"\n", subDevName);
#endif

               AlsaDeviceInfo *info = new AlsaDeviceInfo;
               info->next = NULL;
               info->device_path.alloc(64);
               info->device_path.printf("%s,%d,%d", cardStr, devNr, subDevNr);
               info->device_name.copy(snd_rawmidi_info_get_subdevice_name(rawMidiInfo));
               info->device_name.fixLength();

               /* Build subdevice name (e.g. required for virtual MIDI devices (ALSA version 1.0.23)) */
               if(info->device_name.length <= 1)
               {
                  YAC_String t;
                  t.alloc(16);
                  t.printf(" %u,%u", devNr, subDevNr);
                  info->device_name.copy(hwDrvName);
                  info->device_name.append(&t);
                  info->device_name.fixLength();
               }

               if(NULL != linfo)
               {
                  linfo->next = info;
               }
               else
               {
                  alsa_device_infos = info;
               }

               linfo = info;

               num_alsa_device_infos++;
            }

            // Next sub device
            subDevNr++;

         } /* Enumerate sub-devices */

      } /* Enumerate devices */

      /* Close sound card ctl */
      snd_ctl_close(cardCtl);

   } /* Enumerate cards */

#ifdef ENABLE_TRACE
   yac_host->printf("[dbg] MIDIIn::AllocAlsaDeviceInfos: found %u rawmidi input devices.\n", num_alsa_device_infos);
#endif

   ////snd_config_update_free_global(); // ?
}

void MIDIIn::FreeAlsaDeviceInfos(void) {
   AlsaDeviceInfo *info = alsa_device_infos;

   while(NULL != info)
   {
      AlsaDeviceInfo *ninfo = info->next;

      delete info;

      info = ninfo;
   }

   alsa_device_infos = NULL;
}

AlsaDeviceInfo *MIDIIn::GetAlsaDeviceInfoByIdx(sUI _nr) {
   if(_nr < num_alsa_device_infos)
   {
      AlsaDeviceInfo *info = alsa_device_infos;
      sUI idx = 0;

      while(NULL != info)
      {
         if(idx == _nr)
         {
            return info;
         }
         else
         {
            info = info->next;
            idx++;
         }
      }
   }

   return NULL;
}

MIDIIn::MIDIIn(void) {
   midiin_handle = NULL;
   device_idx = -1;

   b_running = YAC_FALSE;

   record_readoff   = 0;
   record_writeoff  = 0;
   record_numevents = 0;

   for(sUI i = 0u; i < MIDIIN_MAXEVENTS; i++)
      recorded_events[i] = NULL;

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

   {
      sUI i;
      for(i=0; i<16; i++)
      {
         b_parse_param[i] = YAC_TRUE;
         rpn [i] = -1;
         nrpn[i] = -1;
         data_entry[i] = 0;
         data_entry_mode[i] = MIDI_DATAENTRYMODE_MSBLSB; // MSB,LSB order
      }
   }

   virt_state.init();
}


MIDIIn::~MIDIIn() {
   _close();
   freeEventsAndMutex();  // in case device was not opened and parseBuffer() was used
}

sBool MIDIIn::isOpen(void) {
   return (NULL != midiin_handle);
}

void MIDIIn::initSysExBuffers(void) {
   memset(midi_buffer_data, 0, (MIDIIN_NUMBUFFERS * MIDIIN_BLOCKSIZE));

   curr_buffer_index = 0;
}

sU8 *MIDIIn::getNextSysExBuffer(void) {
   sU8 *ret = (midi_buffer_data + (curr_buffer_index * MIDIIN_BLOCKSIZE));

   curr_buffer_index = (curr_buffer_index + 1) % MIDIIN_NUMBUFFERS;

   return ret;
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
      printf("xxx endRecordedEvent: ev.msg= %02x %02x %02x %02x\n",
             _ev->data.msg[0],
             _ev->data.msg[1],
             _ev->data.msg[2],
             _ev->data.msg[3]
             );
#endif

      record_writeoff = (record_writeoff +1) % MIDIIN_MAXEVENTS;

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

YAC_Object *MIDIIn::_getNextEvent(void) {
   YAC_Object *r;

   // Return RecordedMIDIEvent* or NULL if no event is available
   if(NULL != mtx_record)
      yac_host->yacMutexLock(mtx_record);

   if(record_numevents > 0u)
   {
      r = recorded_events[record_readoff]; // RecordedMIDIEvent object
      record_readoff = (record_readoff + 1u) % MIDIIN_MAXEVENTS;
      record_numevents--;
   }
   else
   {
      r = NULL;
   }

   if(NULL != mtx_record)
      yac_host->yacMutexUnlock(mtx_record);

   return r;
}

sUI MIDIIn::_getNumEvents(void) {
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

void MIDIIn::_reset(void) {

   _clearEvents();
}

void MIDIIn::_start(void) {
   //_clearEvents();
}

void MIDIIn::_stop(void) {
}

sBool MIDIIn::_isAlive(void) {
   return isOpen();
}

sBool MIDIIn::_openByName(YAC_Object *_devName) {
   _close();

   if(YAC_VALID(_devName))
   {
      if(YAC_Is_String(_devName))
      {
         YAC_String *devName = (YAC_String*) _devName;

         AlsaDeviceInfo *devInfo = alsa_device_infos;
         sSI devIdx = 0;

         while(NULL != devInfo)
         {
            if(devInfo->device_name.compare(devName))
            {
               if(yac_host->yacGetDebugLevel() > 1)
               {
                  yac_host->printf("*FOUND*\n");
               }

               int err;

#ifdef ENABLE_TRACE
               yac_host->printf("[trc] MIDIIn::open: trying to open MIDI device \"%s\" (path=\"%s\").\n",
                                (const char*)devInfo->device_name.chars,
                                (const char*)devInfo->device_path.chars
                                );
#endif

               err = snd_rawmidi_open(&midiin_handle,
                                      NULL, //retOutputHandle
                                      (const char*)devInfo->device_path.chars, // hw:x,y,z
                                      0////SND_RAWMIDI_NONBLOCK
                                      );

               if(err >= 0)
               {
                  device_name.copy(&devInfo->device_name);
                  device_name.fixLength();

                  device_path.copy(&devInfo->device_path);
                  device_path.fixLength();

                  device_idx = devIdx;

#ifdef ENABLE_DEBUG
                  //if(yac_host->yacGetDebugLevel() > 1)
                  {
                     yac_host->printf("[...] MIDIIn::open: MIDI device \"%s\" opened (path=\"%s\").\n",
                                      (const char*)device_name.chars,
                                      (const char*)device_path.chars
                                      );
                  }
#endif

                  /* Configure ring buffer size */
                  {
                     snd_rawmidi_params_t *params;

                     (void)snd_rawmidi_params_malloc(&params);

                     memset(params, 0, snd_rawmidi_params_sizeof());

                     (void)snd_rawmidi_params_current(midiin_handle, params);

                     err = snd_rawmidi_params_set_buffer_size(midiin_handle, params, MIDIIN_BLOCKSIZE);

                     if(err < 0)
                     {
                        yac_host->printf("[~~~] MIDIIn::open: snd_rawmidi_params_set_buffer_size(path=\"%s\", sz=%u) failed (err=%d)\n",
                                         (const char*)device_path.chars,
                                         MIDIIN_BLOCKSIZE,
                                         err
                                         );
                     }

                     snd_rawmidi_params_free(params);
                  }

                  initSysExBuffers();
                  allocEventsAndMutex();

                  // Start reader thread
                  b_running = YAC_TRUE;

                  if(!b_customsched_init)
                  {
                     b_customsched_init = YAC_TRUE;

                     pthread_attr_init(&custom_sched_attr);
                     pthread_attr_setinheritsched(&custom_sched_attr, PTHREAD_EXPLICIT_SCHED);
                     pthread_attr_setschedpolicy(&custom_sched_attr, SCHED_FIFO);

                     fifo_max_prio = sched_get_priority_max(SCHED_FIFO);
                     fifo_min_prio = sched_get_priority_min(SCHED_FIFO);

                     yac_host->printf("[dbg] MIDIIn: pthread SCHED_FIFO min_prio=%d max_prio=%d\n",
                                      fifo_min_prio,
                                      fifo_max_prio
                                      );

                     ////fifo_mid_prio = (fifo_min_prio + fifo_max_prio)/2;
                     fifo_param.sched_priority = fifo_max_prio > 95 ? 95 : fifo_max_prio;

                     pthread_attr_setschedparam(&custom_sched_attr, &fifo_param);
                  }

                  if(0 == pthread_create(&pthread_id, &custom_sched_attr, midiin_reader__threadentry, (void*) this))
                  {
                     /* Succeeded (SCHED_FIFO) */
                     return YAC_TRUE;
                  }
                  else if(0 == pthread_create(&pthread_id, NULL, midiin_reader__threadentry, (void*) this))
                  {
                     /* Succeeded */
                     yac_host->printf("[~~~] MIDIIn::open: failed to create SCHED_FIFO thread, falling back to default..\n");
                     return YAC_TRUE;
                  }
                  else
                  {
                     yac_host->printf("[---] MIDIIn::open: pthread_create() failed (errno=%d(\"%s\")).\n",
                                      errno,
                                      strerror(errno)
                                      );

                     // Close input device
                     snd_rawmidi_close(midiin_handle);
                     midiin_handle = NULL;

                     return YAC_FALSE;
                  }
               }
               else
               {
                  if(yac_host->yacGetDebugLevel() > 2)
                  {
                     /* should be printed by caller */
                     yac_host->printf("[---] MIDIIn::open: failed to open MIDI device \"%s\" (path=\"%s\") (err=%d)\n",
                                      (const char*)devName->chars,
                                      (const char*)devInfo->device_path.chars,
                                      err
                                      );
                  }

                  return YAC_FALSE;
               }

            } // compare devName

            // Try next device info
            devInfo = devInfo->next;
            devIdx++;

         } // iterate device infos

         if(yac_host->yacGetDebugLevel() > 1)
         {
            yac_host->printf("[---] MIDIIn::open: MIDI device \"%s\" not present.\n", (const char*)devName->chars);
         }

      } // YAC_Is_String
   } // YAC_VALID(_devName)

   return YAC_FALSE;
}

sSI MIDIIn::getDeviceIdx(void) {
   if(isOpen())
   {
      return device_idx;
   }
   else
   {
      return -1;
   }
}

sBool MIDIIn::_openByIdx(sUI _idx) {
   if(_idx < num_alsa_device_infos)
   {
      AlsaDeviceInfo *devInfo = alsa_device_infos;
      sUI cIdx = 0;

      while(NULL != devInfo)
      {
         if(cIdx == _idx)
         {
            return _openByName(&devInfo->device_name);
         }

         /* Try next device info */
         cIdx++;

         devInfo = devInfo->next;
      }
   }
   else
   {
      yac_host->printf("[---] MIDIIn::openByIdx: invalid device index (%u>=%u).\n",
                       _idx,
                       num_alsa_device_infos
                       );
   }

   return YAC_FALSE;
}

void MIDIIn::_openLocalMIDI(void) {
   _close();
   initSysExBuffers();
   allocEventsAndMutex();
}

void MIDIIn::_getDeviceName(YAC_Value *_r) {
   YAC_String *s = (YAC_String *) yac_host->yacNewByID(YAC_CLID_STRING);
   s->yacCopy(&device_name);
   _r->initString(s, 1);
}

void MIDIIn::waitThreadBusy(void) {
   sUI waitCnt = 0;

   while(b_thread_busy)
   {
      usleep(1000);

      if(waitCnt++ == 2000)
      {
         yac_host->printf("[~~~] MIDIIn::waitThreadFinished: thread still busy after 2s!\n");
         break;
      }
   }
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

   if(NULL != midiin_handle)
   {
      b_running = YAC_FALSE;

      waitThreadBusy();

      yac_host->printf("[dbg] MIDIIn::close: killing reader thread..\n");
      pthread_detach(pthread_id);
      pthread_cancel(pthread_id);
      pthread_id = 0;
      yac_host->printf("[dbg] MIDIIn::close: ..ok, reader thread terminated.\n");

      snd_rawmidi_close(midiin_handle);
      midiin_handle = NULL;

      // Wait for reader thread
      ////yac_host->printf("[dbg] MIDIIn::close: waiting for reader thread..\n");
      ////pthread_join(pthread_id, NULL);

      freeEventsAndMutex();
   }
}

sUI MIDIIn::GetNumDevices(void) {
   return num_alsa_device_infos;
}

void MIDIIn::GetDeviceNameByIdx(sUI _idx, YAC_Value *_r) {

   AlsaDeviceInfo *devInfo = GetAlsaDeviceInfoByIdx(_idx);

   if(NULL != devInfo)
   {
      YAC_String *s = YAC_New_String();
      s->yacCopy(&devInfo->device_name);
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
   // printf("xxx parseMIDIByte c=%u\n", c);
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
               st.ev->data.msg[0] = c;

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

                     st.ev->data.msg[0] = c;

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

               st.ev->data.msg[0] = st.running_status;
               st.ev->data.msg[1] = c;

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
                                st.ev->data.msg[0]
                                );

               st.running_status = 0u;
               st.state = ST_ANY;
               goto restart_msg;
            }
         }
         else
         {
            // Received last byte of MIDI short message
            st.ev->data.msg[st.off] = c;

            if(0xB0u == (st.ev->data.msg[0] & 0xF0u))
            {
               st.ev->classifyCtlChange(this, &st.b_filtered);
            }

            if(YAC_FALSE == st.b_filtered)
            {
               st.ev->data_entry = data_entry[st.ev->data.msg[0] & 15u];
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
                                st.ev->data.msg[0]
                                );
               st.running_status = 0u;
               st.state = ST_ANY;
               goto restart_msg;
            }
         }
         else
         {
            // Received second last byte
            st.ev->data.msg[st.off++] = c;

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
