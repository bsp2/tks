/// MIDIOut.cpp
///
/// (c) 2006-2023 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///
/// changed: 08Sep2006, 23Jan2011, 23May2013, 04Feb2014, 13Feb2015, 14Feb2015, 17Jan2016
///          21Apr2022, 20Jan2023, 08Feb2023, 17Mar2023, 26Jun2023, 18Jul2023, 20Jul2023
///
///
///

#include <string.h>

#include "inc_portmidi.h"

#define YAC_BIGSTRING defined
#include <yac.h>

#include "MIDIOut.h"

// #define DP if(0);else yac_host->printf
#define DP if(1);else yac_host->printf


MIDIOut::MIDIOut(void) {
   pm_stream = NULL;
   device_idx = -1;

   send_fail_count     = 0;
   b_reopen_after_fail = YAC_FALSE;
   reopen_device_idx   = -1;
}


MIDIOut::~MIDIOut() {
   _close();
}

sBool MIDIOut::_openByName(YAC_Object *_devName) {
   _close();

   if(YAC_VALID(_devName) && YAC_Is_String(_devName))
   {
      YAC_CAST_ARG(YAC_String, devName, _devName);
      const char *devNameChars = (const char*) devName->chars;

      sSI numIODevs = Pm_CountDevices();
      sSI outIdx = 0;
      for(PmDeviceID ioIdx = 0; ioIdx < numIODevs; ioIdx++)
      {
         const PmDeviceInfo *info = Pm_GetDeviceInfo(ioIdx);
         if(0 != info->output)
         {
            if(!strcmp(info->name, devNameChars))
            {
               PmError err = Pm_OpenOutput(&pm_stream,
                                           ioIdx,
                                           NULL/*outputDriverInfo*/,
                                           MIDIOUT_MAXEVENTS/*bufferSize*/,
                                           NULL/*time_proc*/,  // "a procedure that returns time in milliseconds"
                                           NULL/*time_info*/,  // "a pointer passed to time_proc"
                                           0/*latency*/
                                           );
               if(0 == err)
               {
                  device_name.copy(info->name);
                  device_idx = (sSI)outIdx;
                  open2();

                  if(yac_host->yacGetDebugLevel() > 1)
                  {
                     yac_host->printf("[trc] MIDIOut<portmidi>::open: MIDI device \"%s\" opened.\n", devNameChars);
                  }
                  return YAC_TRUE;
               }
               else
               {
                  yac_host->printf("[---] MIDIOut<portmidi>::openByName: failed to open devName=\"%s\" error=\"%s\"\n", devNameChars, Pm_GetErrorText(err));
               }
            }

            outIdx++;
         }
      }
   }
   return YAC_FALSE;
}

void MIDIOut::getDeviceName(YAC_Value *_r) {
   YAC_String *s = YAC_New_String();
   s->yacCopy(&device_name);
   _r->initString(s, 1);
}

sSI MIDIOut::getDeviceIdx(void) {
   return (NULL != pm_stream) ? device_idx : -1;
}

sBool MIDIOut::openByIdxInt(sUI _idx, sBool _bQuiet) {
   _close();

   sSI numIODevs = Pm_CountDevices();
   sSI outIdx = 0;
   for(PmDeviceID ioIdx = 0; ioIdx < numIODevs; ioIdx++)
   {
      const PmDeviceInfo *info = Pm_GetDeviceInfo(ioIdx);
      if(0 != info->output)
      {
         if(outIdx == sSI(_idx))
         {
            PmError err = Pm_OpenOutput(&pm_stream,
                                        ioIdx,
                                        NULL/*outputDriverInfo*/,
                                        MIDIOUT_MAXEVENTS/*bufferSize*/,
                                        NULL/*time_proc*/,  // "a procedure that returns time in milliseconds"
                                        NULL/*time_info*/,  // "a pointer passed to time_proc"
                                        0/*latency*/
                                        );
            if(0 == err)
            {
               device_name.copy(info->name);
               device_idx = (sSI)outIdx;
               open2();

               if(!_bQuiet && yac_host->yacGetDebugLevel() > 1)
               {
                  yac_host->printf("[trc] MIDIOut<portmidi>::open: MIDI device idx=%d name=\"%s\" opened.\n", outIdx, info->name);
               }
               return YAC_TRUE;
            }
            else if(!_bQuiet)
            {
               yac_host->printf("[---] MIDIOut<portmidi>::openByName: failed to open MIDI device idx=%d error=\"%s\"\n", outIdx, Pm_GetErrorText(err));
            }
         }

         outIdx++;
      }
   }

   return YAC_FALSE;
}

sBool MIDIOut::_openByIdx(sUI _idx) {
   return openByIdxInt(_idx, YAC_FALSE/*bQuiet*/);
}

void MIDIOut::open2(void) {
   send_fail_count = 0;
   b_reopen_after_fail = YAC_FALSE;
   reopen_device_idx = -1;
}

void MIDIOut::noteOn(sUI _midiCh, sUI _note, sUI _vel) {
   if(_vel>127)
      _vel = 127;

   // midiOutShortMsg(midiout_handle, (DWORD) ( (_vel<<16) | (_note<<8) | (0x90+_midiCh) ) );
}

void MIDIOut::noteOff(sUI _midiCh, sUI _note, sUI _vel) {
   if(_vel>127)
      _vel = 127;

   // midiOutShortMsg(midiout_handle, (DWORD) ( (_vel<<16) | (_note<<8) | (0x80+_midiCh) ) );
}

void MIDIOut::allNotesOff(sUI _midiCh) {
   // midiOutShortMsg(midiout_handle, (DWORD) ( (0<<16) | (0x7b<<8) | (0xB0+_midiCh) ) );
}

void MIDIOut::bankSelect(sUI _midiCh, sUI _bank) {
   // midiOutShortMsg(midiout_handle, (0xb0+_midiCh) | (0<<8) | ((_bank&0xFF00)<<8) );
   // midiOutShortMsg(midiout_handle, (0xb0+_midiCh) | (32<<8) | ((_bank&0x00FF)<<16) );
}

void MIDIOut::programChange(sUI _midiCh, sUI _prog) {
   // midiOutShortMsg(midiout_handle, (0xC0+_midiCh) | (_prog<<8) );
}

int MIDIOut::sendBuffer(YAC_Object *_buffer) {
   // (note) PortMidi expects everything to be split into 1..4 byte (dword) messages
   // (note) 'buffer' must be complete, i.e. must not contain partial MIDI messages
   // (note) 'buffer' sysex messages must not contain MIDI realtime messages
   if(NULL != pm_stream)
   {
      if(YAC_VALID(_buffer))
      {
         if(YAC_CHK(_buffer, YAC_CLID_BUFFER))
         {
            YAC_CAST_ARG(YAC_Buffer, buf, _buffer);

            if(buf->io_offset > 0)
            {
               if(buf->io_offset <= MIDIOUT_BUFSIZE)
               {
                  PmEvent pmEv;
                  pmEv.timestamp = 0; // ignored since stream is opened with latency=0
                  PmError pmErr;
                  sUI writeOff = 0u;
                  const sU8 *s = buf->buffer;
                  union {
                     sU8 u8[4];
                     sU32 u32;
                  } d;
#define Demit \
                  pmEv.message = d.u32; \
                  DP("[trc] MIDIOut<portmidi>::sendBuffer: emit pmEv.message=0x%08x\n", pmEv.message); \
                  pmErr = Pm_Write(pm_stream, &pmEv, 1);                \
                  if(0 != pmErr) \
                  { \
                     yac_host->printf("[~~~] MIDIOut<portmidi>::sendBuffer: Pm_Write() failed with err=%d (devName=\"%s\")\n", pmErr, (const char*)device_name.chars); \
                     if(pmHostError == pmErr)\
                     { \
                        char hostErrMsg[256]; \
                        Pm_GetHostErrorText(hostErrMsg, 255); \
                        yac_host->printf("[~~~] MIDIOut<portmidi>::sendBuffer: Pm_Write() pmHostError message=\"%s\"\n", hostErrMsg); \
                     } \
                  }

#define Derrincomplete(ST) \
                  yac_host->printf("[~~~] MIDIOut<portmidi>::sendBuffer: incomplete buffer (readOff=%u status=0x%02x)\n", readOff, ST); \
                  return YAC_FALSE

                  sBool bSysEx = YAC_FALSE;

                  sUI totalNumIterations = 0u;  // paranoia
                  sU8 runningStatus = 0u;
                  sUI readOff = 0u;

                  while( (readOff < buf->io_offset) && (totalNumIterations++ < buf->io_offset) )
                  {
                     sBool bAllowRunningStatus = (0u != runningStatus);
                     sU8 c = s[readOff];
                     DP("[trc] MIDIOut<portmidi>::sendBuffer: readOff=%u/%u c=$%02x bSysEx=%d\n", readOff, buf->io_offset, c, bSysEx);
                     switch(c & 0xF0u)
                     {
                        case 0x80u:
                        case 0x90u:
                        case 0xA0u:
                        case 0xB0u:
                        case 0xE0u:
                           if( (readOff + 3u) <= buf->io_offset )
                           {
                              bSysEx = YAC_FALSE;
                              d.u8[0] = s[readOff + 0u];
                              d.u8[1] = s[readOff + 1u];
                              d.u8[2] = s[readOff + 2u];
                              d.u8[3] = 0u; // unused
                              readOff += 3u;
                              Demit;
                              runningStatus = c;
                              bAllowRunningStatus = YAC_FALSE;
                           }
                           else
                           {
                              Derrincomplete(c);
                           }
                           break;

                        case 0xC0u:
                        case 0xD0u:
                           if( (readOff + 2u) <= buf->io_offset )
                           {
                              bSysEx = YAC_FALSE;
                              d.u8[0] = s[readOff + 0u];
                              d.u8[1] = s[readOff + 1u];
                              d.u8[2] = 0u;  // unused
                              d.u8[3] = 0u;  // unused
                              readOff += 2u;
                              Demit;
                              runningStatus = c;
                              bAllowRunningStatus = YAC_FALSE;
                           }
                           else
                           {
                              Derrincomplete(c);
                           }
                           break;

                        case 0xF0u:
                           if(0xF2u == c)
                           {
                              if( (readOff + 2u) <= buf->io_offset )
                              {
                                 // http://midi.teragonaudio.com/tech/midispec/ssp.htm
                                 bSysEx = YAC_FALSE;
                                 d.u8[0] = s[readOff + 0u];  // $F2
                                 d.u8[1] = s[readOff + 1u];  // SPP LSB
                                 d.u8[2] = s[readOff + 2u];  // SPP MSB
                                 d.u8[3] = 0u;  // unused
                                 readOff += 3u;
                                 Demit;
                                 bAllowRunningStatus = YAC_FALSE;
                              }
                              else
                              {
                                 Derrincomplete(c);
                              }
                           }
                           else if(0xF3u == c)
                           {
                              if( (readOff + 1u) <= buf->io_offset )
                              {
                                 // http://midi.teragonaudio.com/tech/midispec/songsel.htm
                                 bSysEx = YAC_FALSE;
                                 d.u8[0] = s[readOff + 0u];  // $F2
                                 d.u8[1] = s[readOff + 1u];  // song number
                                 d.u8[2] = 0u;  // unused
                                 d.u8[3] = 0u;  // unused
                                 readOff += 2u;
                                 Demit;
                                 bAllowRunningStatus = YAC_FALSE;
                              }
                              else
                              {
                                 Derrincomplete(c);
                              }
                           }
                           else if(0xF7u == c)
                           {
                              // End of SysEx
                              DP("[trc] MIDIOut<portmidi>::sendBuffer: sysex end writeOff=%u readOff=%u\n", writeOff, readOff);
                              d.u8[writeOff] = 0xF7u;
                              Demit;
                              bSysEx = YAC_FALSE;
                              readOff++;
                              bAllowRunningStatus = YAC_FALSE;
                           }
                           else if(0u != (c & 0x0Fu))
                           {
                              // SysRT
                              bSysEx = YAC_FALSE;
                              d.u8[0] = s[readOff];
                              d.u8[1] = 0u;  // unused
                              d.u8[2] = 0u;  // unused
                              d.u8[3] = 0u;  // unused
                              readOff += 1u;
                              Demit;
                              bAllowRunningStatus = YAC_FALSE;
                           }
                           else
                           {
                              // Start of SysEx
                              DP("[trc] MIDIOut<portmidi>::sendBuffer: sysex start readOff=%u\n", readOff);
                              writeOff = 0u;
                              d.u32 = 0u;  // unused
                              bSysEx = YAC_TRUE;
                              runningStatus = 0u;
                           }
                           break;

                     } // switch status byte

                     if(bSysEx)
                     {
                        DP("[trc] MIDIOut<portmidi>::sendBuffer: sysex writeOff=%u readOff=%u\n", writeOff, readOff);
                        d.u8[writeOff++] = c;
                        if(4u == writeOff)
                        {
                           // Send partial SysEx message
                           Demit;
                           writeOff = 0u;
                           d.u32 = 0u;
                        }
                        readOff++;
                     }
                     else if(bAllowRunningStatus)
                     {
                        DP("[trc] MIDIOut<portmidi>::sendBuffer: parse runningStatus=0x%02x\n", runningStatus);
                        switch(runningStatus & 0xF0u)
                        {
                           case 0x80u:
                           case 0x90u:
                           case 0xA0u:
                           case 0xB0u:
                           case 0xE0u:
                              if( (readOff + 2u) <= buf->io_offset )
                              {
                                 bSysEx = YAC_FALSE;
                                 d.u8[0] = runningStatus;
                                 d.u8[1] = s[readOff + 0u];
                                 d.u8[2] = s[readOff + 1u];
                                 d.u8[3] = 0u; // unused
                                 readOff += 2u;
                                 Demit;
                              }
                              else
                              {
                                 Derrincomplete(runningStatus);
                              }
                              break;

                           case 0xC0u:
                           case 0xD0u:
                              bSysEx = YAC_FALSE;
                              d.u8[0] = runningStatus;
                              d.u8[1] = s[readOff + 0u];
                              d.u8[2] = 0u;  // unused
                              d.u8[3] = 0u;  // unused
                              readOff += 1u;
                              Demit;
                              break;
                        } // switch running status

                     } // if running_status

                  } // loop buffer bytes

                  if(buf->io_offset > 1u && (totalNumIterations > buf->io_offset))
                     yac_host->printf("[!!!] MIDIOut<portmidi>::sendBuffer: error while parsing output buffer (?) size=%u #iter=%u\n", buf->io_offset, totalNumIterations);
               }
               else
               {
                  yac_host->printf("[---] MIDIOut::sendBuffer: buffer size too large (%u>%u).\n", buf->io_offset, MIDIOUT_BUFSIZE);
               }
            }
            else
            {
               return YAC_TRUE; // Buffer is empty..
            }
         }
         // else: not a Buffer
      }
      // else: not a valid object
   }
   // else: stream not open

   return YAC_FALSE;
}

void MIDIOut::_close(void) {

   if(NULL != pm_stream)
   {
      Pm_Close(pm_stream);
      pm_stream = NULL;

      device_idx = -1;
   }
}

sSI MIDIOut::GetNumDevices(void) {
   sSI r = 0;
   sSI numIODevs = Pm_CountDevices();
   for(PmDeviceID ioIdx = 0; ioIdx < numIODevs; ioIdx++)
   {
      const PmDeviceInfo *info = Pm_GetDeviceInfo(ioIdx);
      if(0 != info->output)
         r++;
   }
   return r;
}

void MIDIOut::GetDeviceNameByIdx(sUI _idx, YAC_Value *_r) {
   _r->initNull();
   sUI outputIdx = 0u;
   sSI numIODevs = Pm_CountDevices();
   for(PmDeviceID ioIdx = 0; ioIdx < numIODevs; ioIdx++)
   {
      const PmDeviceInfo *info = Pm_GetDeviceInfo(ioIdx);
      if(0 != info->output)
      {
         if(outputIdx == _idx)
         {
            YAC_String *s = YAC_New_String();
            s->copy(info->name);
            _r->initString(s, YAC_TRUE);
            return;
         }
         outputIdx++;
      }
   }
}

sSI MIDIOut::GetDeviceTypeByIdx(sUI _idx) {
   // (note) this should be removed. only MIDIPORT type devices are supported.
   (void)_idx;
   return 1/*MOD_MIDIPORT*/;
}
