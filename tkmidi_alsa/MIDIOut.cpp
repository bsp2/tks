/// MIDIOut.cpp
///
/// (c) 2006-2023 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///
/// changed: 08Sep2006, 04Feb2014, 05Feb2014, 11Feb2014, 29Mar2019, 18Jan2023, 20Jul2023
///
///

#include <string.h>

#define YAC_BIGSTRING defined
#include <yac.h>

#include "tkmidi.h"
#include "MIDIOut.h"

#define Dprintf if(0);else yac_host->printf
// #define Dprintf if(1);else yac_host->printf

#define Dtracef if(0);else yac_host->printf
// #define Dtracef if(1);else yac_host->printf

#define W1(_1)                *d++=_1
#define W2(_1,_2)             *d++=_1; *d++=_2
#define W3(_1,_2,_3)          *d++=_1; *d++=_2; *d++=_3
#define W4(_1,_2,_3,_4)       *d++=_1; *d++=_2; *d++=_3; *d++=_4
#define W5(_1,_2,_3,_4,_5)    *d++=_1; *d++=_2; *d++=_3; *d++=_4; *d++=_5
#define W6(_1,_2,_3,_4,_5,_6) *d++=_1; *d++=_2; *d++=_3; *d++=_4; *d++=_5; *d++=_6


AlsaDeviceInfo *MIDIOut::alsa_device_infos = NULL;
sUI MIDIOut::num_alsa_device_infos = 0;

// see <http://www.alsa-project.org/alsa-doc/alsa-lib/>
// see <http://www.alsa-project.org/alsa-doc/alsa-lib/group___raw_midi.html> 
// also see: <http://home.roadrunner.com/~jgglatt/tech/arawmidplay.htm>

void MIDIOut::AllocAlsaDeviceInfos(void) {
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
         yac_host->printf("[---] MIDIOut::AllocAlsaDeviceInfos: snd_card_next() failed (err=%d)\n", err);
         break;
      }

      Dtracef("[trc] MIDIOut::AllocAlsaDeviceInfos: snd_card_next() returned cardNr=%d.\n", cardNr);

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
         yac_host->printf("[---] MIDIOut::AllocAlsaDeviceInfos: snd_ctl_open(cardStr=\"%s\") failed (err=%d)\n",
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
            yac_host->printf("[---] MIDIOut::AllocAlsaDeviceInfos: snd_ctl_rawmidi_next_device(cardStr=\"%s\" devNr=%d) failed (err=%d)\n",
                             cardStr,
                             devNr,
                             err
                             );

            break;
         }

         Dtracef("[trc] MIDIOut::AllocAlsaDeviceInfos: snd_ctl_rawmidi_next_device(cardStr=\"%s\") returned devNr=%d.\n",
                 cardStr,
                 devNr
                 );

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
            yac_host->printf("[---] MIDIOut::AllocAlsaDeviceInfos: snd_ctl_rawmidi_info(\"%s,%d,0\") failed (err=%d)\n",
                             cardStr,
                             devNr
                             );

            /* Try next device */
            continue;
         }

         const char *hwDrvName = snd_rawmidi_info_get_name(rawMidiInfo);

         Dprintf("[dbg] MIDIOut::AllocAlsaDeviceInfos: snd_rawmidi_info_get_name(cardStr=\"%s,%d\") returned hwDrvName=\"%s\".\n",
                 cardStr,
                 devNr,
                 hwDrvName
                 );

         int subDevNr = 0;
         int subDevNum = snd_rawmidi_info_get_subdevices_count(rawMidiInfo);

         Dtracef("[trc] MIDIOut::AllocAlsaDeviceInfos: snd_rawmidi_info_get_card(cardStr=\"%s,%d\") returned cardNr=%d.\n",
                 cardStr,
                 devNr,
                 snd_rawmidi_info_get_card(rawMidiInfo)
                 );

         Dtracef("[trc] MIDIOut::AllocAlsaDeviceInfos: snd_rawmidi_info_get_subdevices_count(cardStr=\"%s,%d\") returned subDevNum=%d.\n",
                 cardStr,
                 devNr,
                 subDevNum
                 );

         while(subDevNr < subDevNum)
         {
            snd_rawmidi_info_set_subdevice(rawMidiInfo, subDevNr);

            err = snd_ctl_rawmidi_info(cardCtl, rawMidiInfo);

            if(err < 0)
            {
               yac_host->printf("[---] MIDIOut::AllocAlsaDeviceInfos: snd_ctl_rawmidi_info(\"%s,%d,%d\") failed (err=%d)\n",
                                cardStr,
                                devNr,
                                subDevNr
                                );

               /* Try next sub-device */
               continue;
            }
            else
            {
               const char *subDevName = snd_rawmidi_info_get_subdevice_name(rawMidiInfo);
               Dtracef("[dbg] MIDIOut::AllocAlsaDeviceInfos: found rawmidi output device \"%s,%d,%d\" (devInfoIdx=%u).\n",
                       cardStr,
                       devNr,
                       subDevNr,
                       num_alsa_device_infos
                       );

               Dtracef("[dbg]    subdevice_name: \"%s\"\n", subDevName);

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

   Dtracef("[dbg] MIDIOut::AllocAlsaDeviceInfos: found %u rawmidi output devices.\n", num_alsa_device_infos);

   ////snd_config_update_free_global(); // ?
}

void MIDIOut::FreeAlsaDeviceInfos(void) {
   AlsaDeviceInfo *info = alsa_device_infos;

   while(NULL != info)
   {
      AlsaDeviceInfo *ninfo = info->next;

      delete info;

      info = ninfo;
   }

   alsa_device_infos = NULL;
}

AlsaDeviceInfo *MIDIOut::GetAlsaDeviceInfoByIdx(sUI _idx) {
   if(_idx < num_alsa_device_infos)
   {
      AlsaDeviceInfo *info = alsa_device_infos;
      sUI idx = 0;

      while(NULL != info)
      {
         if(idx == _idx)
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

MIDIOut::MIDIOut(void) {
   midiout_handle = NULL;
   device_idx = -1;
}


MIDIOut::~MIDIOut() {
   _close();
}

sBool MIDIOut::_openByName(YAC_Object *_devName) {
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

               Dtracef("[trc] MIDIOut::open: trying to open MIDI device \"%s\" (path=\"%s\").\n",
                       (const char*)devInfo->device_name.chars,
                       (const char*)devInfo->device_path.chars
                       );

               err = snd_rawmidi_open(NULL, //retInputHandle
                                      &midiout_handle,
                                      (const char*)devInfo->device_path.chars, // hw:x,y,z
                                      SND_RAWMIDI_NONBLOCK
                                      );

               if(err >= 0)
               {
                  device_idx = devIdx;
                  device_name.copy(&devInfo->device_name);
                  device_path.copy(&devInfo->device_path);

                  Dprintf("[...] MIDIOut::open: MIDI device \"%s\" opened (path=\"%s\").\n",
                          (const char*)device_name.chars,
                          (const char*)device_path.chars
                          );

                  /* Configure ring buffer size */
                  {
                     snd_rawmidi_params_t *params;

                     (void)snd_rawmidi_params_malloc(&params);

                     memset(params, 0, snd_rawmidi_params_sizeof());

                     (void)snd_rawmidi_params_current(midiout_handle, params);

                     err = snd_rawmidi_params_set_buffer_size(midiout_handle, params, MIDIOUT_BLOCKSIZE);

                     if(err < 0)
                     {
                        yac_host->printf("[~~~] MIDIOut::open: snd_rawmidi_params_set_buffer_size(path=\"%s\", sz=%u) failed (err=%d)\n",
                                         (const char*)device_path.chars,
                                         MIDIOUT_BLOCKSIZE,
                                         err
                                         );
                     }

                     snd_rawmidi_params_free(params);
                  }

                  /* Succeeded */
                  return YAC_TRUE;
               }
               else
               {
                  if(yac_host->yacGetDebugLevel() > 2)
                  {
                     /* should be printed by caller */
                     yac_host->printf("[---] MIDIOut::open: failed to open MIDI device \"%s\" (path=\"%s\") (err=%d)\n",
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
            yac_host->printf("[---] MIDIOut::open: MIDI device \"%s\" not present.\n", (const char*)devName->chars);
         }

      } // YAC_Is_String
   } // YAC_VALID(_devName)

   return YAC_FALSE;
}

sBool MIDIOut::_openByIdx(sUI _idx) {
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
      yac_host->printf("[---] MIDIOut::openByIdx: invalid device index (%u>=%u).\n",
                       _idx,
                       num_alsa_device_infos
                       );
   }

   return YAC_FALSE;
}


void MIDIOut::getDeviceName(YAC_Value *_r) {
   YAC_String *s = (YAC_String *) yac_host->yacNewByID(YAC_CLID_STRING);
   s->yacCopy(&device_name);
   _r->initString(s, 1);
}

sSI MIDIOut::getDeviceIdx(void) {
   if(NULL != midiout_handle)
   {
      return device_idx;
   }
   else
   {
      return -1;
   }
}

sBool MIDIOut::alsaWrite(sU8 *_buf, sUI _numBytes) {

   if(_numBytes <= MIDIOUT_BLOCKSIZE)
   {
      sUI totalNumWritten = 0;
      sUI totalTries = 0;
      
      while(totalNumWritten < _numBytes)
      {
         sUI curNumBytes = (size_t) (_numBytes - totalNumWritten);
         
         ssize_t curNumWritten = snd_rawmidi_write(midiout_handle,
                                                   (const void*) (_buf + totalNumWritten),
                                                   curNumBytes
                                                   );
         if(curNumWritten > 0)
         {
            totalNumWritten += (sUI)curNumWritten;
         }
         else
         {
            Dtracef("[~~~] MIDIOut::alsaWrite: snd_rawmidi_write(path=\"%s\", sz=%u) returned curNumWritten=%d (off=%u)\n",
                    (const char*)device_path.chars,
                    curNumBytes,
                    curNumWritten,
                    totalNumWritten
                    );

            if(++totalTries > 50000)
            {
               yac_host->printf("[---] MIDIOut::alsaWrite: snd_rawmidi_write(path=\"%s\", sz=%u) returned curNumWritten=%d (off=%u). Giving up after %u retries.\n",
                                (const char*)device_path.chars,
                                curNumBytes,
                                curNumWritten,
                                totalNumWritten,
                                50000
                                );

               return YAC_FALSE;
            }
         }
      }

      return YAC_TRUE;
   }
   else
   {
      yac_host->printf("[---] MIDIOut::sendBuffer: buffer size too big (%u>%u).\n", _numBytes, MIDIOUT_BLOCKSIZE);
   }

   return YAC_FALSE;
}

void MIDIOut::noteOn(sUI _midiCh, sUI _note, sUI _vel) {
   if(_vel>127)
      _vel = 127;
   else if(_vel<0)
      _vel =0;

   sU8 buf[3];

   buf[0] = (sU8) (0x90u + (sU8)_midiCh);
   buf[1] = (sU8) _note;
   buf[2] = (sU8) _vel;

   (void)alsaWrite(buf, 3);
}

void MIDIOut::noteOff(sUI _midiCh, sUI _note, sUI _vel) {
   if(_vel>127)
      _vel = 127;
   else if(_vel<0)
      _vel =0;

   sU8 buf[3];

   buf[0] = (sU8) (0x80u | (sU8)_midiCh);
   buf[1] = (sU8) _note;
   buf[2] = (sU8) _vel;

   (void)alsaWrite(buf, 3);
}

void MIDIOut::allNotesOff(sUI _midiCh) {
   sU8 buf[3];

   buf[0] = (sU8) (0xB0u | (sU8)_midiCh);
   buf[1] = 0x7bu;
   buf[2] = 0;

   (void)alsaWrite(buf, 3);
}

void MIDIOut::bankSelect(sUI _midiCh, sUI _bank) {
   sU8 buf[6];
   buf[0] = (sU8) (0xB0u | (sU8)_midiCh);
   buf[1] = 0x0u;
   buf[2] = (_bank & 0x7F00u) >> 8;

   buf[3] = (sU8) (0xB0u | (sU8)_midiCh);
   buf[4] = 0x20u;
   buf[5] = (_bank & 0x007Fu);

   alsaWrite(buf, 6);
}

void MIDIOut::programChange(sUI _midiCh, sUI _prog) {
   sU8 buf[2];

   buf[0] = (sU8) (0xC0u | (sU8)_midiCh);
   buf[1] = _prog;

   alsaWrite(buf, 2);
}

int MIDIOut::sendBuffer(YAC_Object *_buffer) {
   if(YAC_VALID(_buffer))
   {
      if(YAC_CHK(_buffer, YAC_CLID_BUFFER))
      {
         YAC_Buffer *buf = (YAC_Buffer*) _buffer;

         if(buf->io_offset > 0)
         {
            return alsaWrite(buf->buffer, buf->io_offset);
         }
         else
         {
            return YAC_TRUE; // Buffer is empty..
         }
      }
   }

   return YAC_FALSE;
}

void MIDIOut::_close(void) {

   if(NULL != midiout_handle)
   {
      for(int i=0; i<16; i++)
      {
         allNotesOff(i);
      }

      // Flush pending MIDI bytes
      snd_rawmidi_drain(midiout_handle);

      snd_rawmidi_close(midiout_handle);
      midiout_handle = NULL;
   }
}

sUI MIDIOut::GetNumDevices(void) {
   return num_alsa_device_infos;
}

void MIDIOut::GetDeviceNameByIdx(sUI _idx, YAC_Value *_r) {

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

sSI MIDIOut::GetDeviceTypeByIdx(sUI _idx) {

   (void)_idx;

   return -1;
}

