/// MIDIOut.cpp
///
/// (c) 2006-2022 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///
/// changed: 08Sep2006, 23Jan2011, 23May2013, 04Feb2014, 13Feb2015, 14Feb2015, 17Jan2016
//           21Apr2022
///
///
///

#include <windows.h>

#ifndef MOD_WAVETABLE // VC6
#define MOD_WAVETABLE   6  /* hardware wavetable synth */
#endif
#ifndef MOD_SWSYNTH
#define MOD_SWSYNTH     7  /* software synth */
#endif

#include <string.h>

#define YAC_BIGSTRING defined
#include <yac.h>

#include "MIDIOut.h"

#define W1(_1)                *d++=_1
#define W2(_1,_2)             *d++=_1; *d++=_2
#define W3(_1,_2,_3)          *d++=_1; *d++=_2; *d++=_3
#define W4(_1,_2,_3,_4)       *d++=_1; *d++=_2; *d++=_3; *d++=_4
#define W5(_1,_2,_3,_4,_5)    *d++=_1; *d++=_2; *d++=_3; *d++=_4; *d++=_5
#define W6(_1,_2,_3,_4,_5,_6) *d++=_1; *d++=_2; *d++=_3; *d++=_4; *d++=_5; *d++=_6


MIDIOut::MIDIOut(void) {
   midiout_handle = NULL;
   device_idx = -1;

   send_fail_count     = 0;
   b_reopen_after_fail = YAC_FALSE;
   reopen_device_idx   = -1;

   curr_buffer_index = 0;
}


MIDIOut::~MIDIOut() {
   _close();
}

void MIDIOut::initMidiBuffers(void) {
   memset(midi_buffer_data, 0, MIDIOUT_NUMBUFFERS * MIDIOUT_BUFSIZE);

   for(sUI i = 0u; i < MIDIOUT_NUMBUFFERS; i++)
   {
      MIDIHDR *hdr = &midi_buffers[i];
      memset(hdr, 0, sizeof(MIDIHDR));
      hdr->dwBufferLength = MIDIOUT_BUFSIZE;
      hdr->lpData = (char*) (midi_buffer_data + (i*MIDIOUT_BUFSIZE));
   }

   curr_buffer_index = 0;
}

sSI MIDIOut::allocBufferIndex(sUI _size) {

   MMRESULT res = midiOutUnprepareHeader(midiout_handle, &midi_buffers[curr_buffer_index], sizeof(MIDIHDR));

   if(MMSYSERR_NOERROR == res)
   {
      // (note) app hangs when only dwBytesRecorded is updated
      midi_buffers[curr_buffer_index].dwBytesRecorded = _size;
      midi_buffers[curr_buffer_index].dwBufferLength = _size; // This is is used for MIDIOut..

      sSI ret = (sSI)curr_buffer_index;

      curr_buffer_index = (curr_buffer_index + 1u) % MIDIOUT_NUMBUFFERS;

      return ret;
   }
   else
   {
      yac_host->printf("[---] MIDIOut::allocBufferIndex(sz=%u): midiOutUnprepareHeader() failed (res=0x%08x)\n", _size, res);
      return -1;
   }
}

sU8 *MIDIOut::getBufferData(sSI _idx) {
   return (sU8*) midi_buffers[_idx].lpData;
}

void MIDIOut::sendBufferIndex(sSI _idx) {

   if(b_reopen_after_fail)
   {
      if(!openByIdxInt((sUI)reopen_device_idx, YAC_TRUE/*bQuiet*/))
      {
         return;
      }
      else
      {
         yac_host->printf("[dbg] MIDIOut: device idx=%d (\"%s\") reopened after disconnect/failure.\n", device_idx, (const char*)device_name.chars);
      }
   }

   if(MMSYSERR_NOERROR == midiOutPrepareHeader(midiout_handle, &midi_buffers[_idx], sizeof(MIDIHDR)))
   {
      if(MMSYSERR_NOERROR == midiOutLongMsg(midiout_handle, &midi_buffers[_idx], sizeof(MIDIHDR)))
      {
         //Sleep(150);
         //yac_host->printf("[+++] sent buffer index=%u.\n", _idx);
      }
      else
      {
         if(send_fail_count < 4)
         {
            yac_host->printf("[---] MIDIOut::sendBufferIndex(%i) failed (midiOutLongMsg).\n", _idx);

            if(++send_fail_count == 4)
            {
               b_reopen_after_fail = YAC_TRUE;
               reopen_device_idx = device_idx;
               _close();
            }
         }
      }
   }
   else
   {
      yac_host->printf("[---] MIDIOut::sendBufferIndex(%i) failed (midiOutPrepareHeader).\n", _idx);
   }

}

sBool MIDIOut::_openByName(YAC_Object *_devName) {

   // (note) even though GetNumDevices() skips non-MIDIPORT devices, this method can still
   //         open non-MIDIPORT devs, if their names are known.. 

   _close();

   if(YAC_VALID(_devName))
   {
      if(YAC_Is_String(_devName))
      {
         YAC_String *devName = (YAC_String*) _devName;
         const char *midiout_device_name = (const char*) devName->chars;

         if(NULL != midiout_device_name)
         {
            UINT numDevs = midiOutGetNumDevs();

            for(UINT i=0; i<numDevs; i++)
            {
               if(midiOutGetDevCaps(i, &midiout_caps, sizeof(midiout_caps)) == MMSYSERR_NOERROR)
               {
                  if(yac_host->yacGetDebugLevel() > 1)
                  {
                     yac_host->printf("[...] midiout[%i]: name=\"%s\" tech=%s chMask=%04x \n", 
                                      i, midiout_caps.szPname, 
                                      (midiout_caps.wTechnology==MOD_MIDIPORT)  ? "MOD_MIDIPORT" :
                                      (midiout_caps.wTechnology==MOD_SYNTH)     ? "MOD_SYNTH" :
                                      (midiout_caps.wTechnology==MOD_SQSYNTH)   ? "MOD_SQSYNTH" :
                                      (midiout_caps.wTechnology==MOD_FMSYNTH)   ? "MOD_FMSYNTH" :
                                      (midiout_caps.wTechnology==MOD_MAPPER)    ? "MOD_MAPPER" :
                                      (midiout_caps.wTechnology==MOD_WAVETABLE) ? "MOD_WAVETABLE" :
                                      (midiout_caps.wTechnology==MOD_SWSYNTH)   ? "MOD_SWSYNTH" :
                                      "<unknown>",
                                      midiout_caps.wChannelMask
                                      );
                  }

                  if(!strcmp(midiout_caps.szPname, midiout_device_name))
                  {
                     if(yac_host->yacGetDebugLevel() > 1)
                     {
                        yac_host->printf("*FOUND*\n");
                     }

                     if(midiOutOpen(&midiout_handle, i, NULL, NULL, CALLBACK_NULL) == MMSYSERR_NOERROR)
                     {
                        if(yac_host->yacGetDebugLevel() > 1)
                        {
                           yac_host->printf("[...] MIDIOut::open: MIDI device \"%s\" opened.\n", midiout_device_name);
                        }

                        send_fail_count = 0;
                        b_reopen_after_fail = YAC_FALSE;
                        reopen_device_idx = -1;

                        device_name.copy((sChar*)midiout_device_name);

                        device_idx = (sSI)i - getFirstMIDIPortIndex();

                        initMidiBuffers();
                        // //memset(&midiout_hdr, 0, sizeof(MIDIHDR));
                        // //current_hdr = 0;
                     
                        /* Succeeded */
                        return YAC_TRUE;
                     }
                     else
                     {
                        yac_host->printf("[---] MIDIOut::open: failed to open MIDI device \"%s\".\n", midiout_device_name);

                        return YAC_FALSE;
                     }
                  }
               }
            }
         }

         if(yac_host->yacGetDebugLevel())
         {
            yac_host->printf("[---] MIDIOut::open: MIDI device \"%s\" not present.\n", midiout_device_name);
         }

      } // YAC_Is_String
   } // YAC_VALID(_devName)

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

sBool MIDIOut::openByIdxInt(sUI _nr, sBool _bQuiet) {
   _close();

   device_idx = _nr;
      
   _nr += getFirstMIDIPortIndex();

   if(MMSYSERR_NOERROR == midiOutOpen(&midiout_handle, _nr, NULL, NULL, CALLBACK_NULL))
   {
      if(MMSYSERR_NOERROR == midiOutGetDevCaps(_nr, &midiout_caps, sizeof(midiout_caps)))
      {
         if(!_bQuiet)
         {
            yac_host->printf("[...] MIDIOut::open: MIDI device #%u \"%s\" opened.\n", _nr, midiout_caps.szPname);
         }

         send_fail_count = 0;
         b_reopen_after_fail = YAC_FALSE;
         reopen_device_idx = -1;
      }

      return YAC_TRUE;
   }
   else
   {
      if(!_bQuiet)
      {
         yac_host->printf("[---] MIDIOut::openByIdxInt: failed to opend MIDI device #%u.\n", _nr);
      }

      device_idx = -1;

      return YAC_FALSE;
   }
}

sBool MIDIOut::_openByIdx(sUI _nr) {

   return openByIdxInt(_nr, YAC_FALSE/*bQuiet*/);
}

// // void MIDIOut::msg3(sU8 _1, sU8 _2, sU8 _3) {
// //    midiOutShortMsg(midiout_handle, ((sUI)_3<<16) | ((sUI)_2<<8) | _1);
// // }

// // void MIDIOut::shortMsg(sSI _msg) {
// //    midiOutShortMsg(midiout_handle, (DWORD)_msg);
// // }

void MIDIOut::noteOn(sUI _midiCh, sUI _note, sUI _vel) {
   if(_vel>127)
      _vel = 127;

   midiOutShortMsg(midiout_handle, (DWORD) ( (_vel<<16) | (_note<<8) | (0x90+_midiCh) ) );
}

void MIDIOut::noteOff(sUI _midiCh, sUI _note, sUI _vel) {
   if(_vel>127)
      _vel = 127;

   midiOutShortMsg(midiout_handle, (DWORD) ( (_vel<<16) | (_note<<8) | (0x80+_midiCh) ) );
}

void MIDIOut::allNotesOff(sUI _midiCh) {
   midiOutShortMsg(midiout_handle, (DWORD) ( (0<<16) | (0x7b<<8) | (0xB0+_midiCh) ) );
}

void MIDIOut::bankSelect(sUI _midiCh, sUI _bank) {
   midiOutShortMsg(midiout_handle, (0xb0+_midiCh) | (0<<8) | ((_bank&0xFF00)<<8) );
   midiOutShortMsg(midiout_handle, (0xb0+_midiCh) | (32<<8) | ((_bank&0x00FF)<<16) );
}

void MIDIOut::programChange(sUI _midiCh, sUI _prog) {
   midiOutShortMsg(midiout_handle, (0xC0+_midiCh) | (_prog<<8) );
}

int MIDIOut::sendBuffer(YAC_Object *_buffer) {
   if(YAC_VALID(_buffer))
   {
      if(YAC_CHK(_buffer, YAC_CLID_BUFFER))
      {
         YAC_Buffer *buf = (YAC_Buffer*) _buffer;

         if(buf->io_offset > 0)
         {
            if(buf->io_offset <= MIDIOUT_BUFSIZE)
            {
               sSI bufH = allocBufferIndex(buf->io_offset);

               // yac_host->printf("xxx bufH=%d\n", bufH);
               
               if(bufH >= 0)
               {
                  sU8 *d = getBufferData(bufH);

                  for(sUI i = 0u; i < buf->io_offset; i++)
                  {
                     *d++ = buf->buffer[i];
                  }

                  sendBufferIndex(bufH);

                  return YAC_TRUE;
               }
               else
               {
                  yac_host->printf("[---] MIDIOut::sendBuffer: allocBufferIndex(sz=%u) failed.\n", buf->io_offset);
               }
            }
            else
            {
               yac_host->printf("[---] MIDIOut::sendBuffer: buffer size too big (%u>%u).\n", buf->size, MIDIOUT_BUFSIZE);
            }
         }
         else
         {
            return YAC_TRUE; // Buffer is empty..
         }
      }
   }

   return YAC_FALSE;
}

#if 0
void MIDIOut::dataRequest1_Roland_JunoG (sSI _midiCh, sSI _addr, sSI _size) {

   // (todo) move to script

   sSI bufh = allocBufferIndex(17);
   sU8 *d = getBufferData(bufh);
   W3(0xF0, 0x41, 0x10); // 41=Roland, 10=DeviceId (default)
   W3(0x00, 0x00, 0x15); // model id (juno-g)
   W1(0x11);
   W4((_addr&0xFF000000)>>24, (_addr&0x00FF0000)>>16, (_addr&0x0000FF00)>>8, (_addr&0x000000FF));
   W4((_size&0xFF000000)>>24, (_size&0x00FF0000)>>16, (_size&0x0000FF00)>>8, (_size&0x000000FF) );
   // Write checksum 
   sU8 chksum = 0x80 - 
                (
                  ( ((_addr&0xFF000000)>>24) +
                    ((_addr&0x00FF0000)>>16) +
                    ((_addr&0x0000FF00)>> 8) +
                    ((_addr&0x000000FF)    ) +
                    ((_size&0xFF000000)>>24) +
                    ((_size&0x00FF0000)>>16) +
                    ((_size&0x0000FF00)>> 8) +
                    ((_size&0x000000FF)    )
                  ) & 0x7F
                );
   W2(chksum&0x7F, 0xF7);
   d = getBufferData(bufh);
   printf("d=%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
      d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7], d[8], d[9], d[10], d[11], d[12], d[13], d[14], d[15], d[16]
      );
   sendBufferIndex(bufh);
}


void MIDIOut::identityRequest_Roland_JunoG (void) {

   // (todo) move to script

   sSI bufh = allocBufferIndex(6);
   sU8 *d = getBufferData(bufh);
   W6(0xF0, 0x7E, 0x10, 0x06, 0x01, 0xF7);
   sendBufferIndex(bufh);

   //msg3(0xF0, 0x7E, 0x10); // 10=deviceid (default)
   //msg3(0x06, 0x01, 0xF7); // GeneralInformation->IdentityRequest, EOX
}
#endif


void MIDIOut::_close(void) {

   if(NULL != midiout_handle)
   {
      for(int i=0; i<16; i++)
      {
         allNotesOff(i);
      }
      // // midiOutReset(midiout_handle);

      for(;;)
      {
         MMRESULT err = midiOutClose(midiout_handle);
         if(MIDIERR_STILLPLAYING == err )
         {
            ::Sleep(250/*millisec*/);
            continue;
         }
         else if(MMSYSERR_NOERROR != err)
         {
            yac_host->printf("[---] MIDIOut::close: MMRESULT<err>=0x%08x\n", err);
         }
         break;
      }
      midiout_handle = NULL;
      device_idx = -1;
   }
}

sSI MIDIOut::getFirstMIDIPortIndex(void) {

   // (note) this assumes that any non-MIDIPORT output devices come first (e.g. the GS softsynth)

   sUI numWinDevs = midiOutGetNumDevs();
   sUI winIdx;

   for(winIdx=0; winIdx < numWinDevs; winIdx++)
   {
      MIDIOUTCAPS caps;
      if(midiOutGetDevCaps(winIdx, &caps, sizeof(caps)) == MMSYSERR_NOERROR)
      {
         if(MOD_MIDIPORT == caps.wTechnology)
         {
            return (sSI)winIdx;
         }
      }
   }

   return 0;
}

sSI MIDIOut::GetNumDevices(void) {
   // Filter out non-MIDIPORT devices
   //  (note) there is no way to programmatically find an output device for an input device (or vice versa)
   //          so we have to rely on an 1:1 mapping between in- and outdevices.
   //  (note) usually Windows lists the SWSYNTH general MIDI device ("Microsoft GS Wavetable Synth") as
   //          the first output device (either that or it is not present at all)
   //  (note) the MIDIOut class expects that starting with the first MIDIPORT-type device, only other
   //          MIDIPORT-type devices follow
   sSI r = 0;
   sUI numWinDevs = midiOutGetNumDevs();
   sUI winIdx;
   sSI firstMIDI = -1;

   for(winIdx=0; winIdx < numWinDevs; winIdx++)
   {
      MIDIOUTCAPS caps;
      if(midiOutGetDevCaps(winIdx, &caps, sizeof(caps)) == MMSYSERR_NOERROR)
      {
         if(MOD_MIDIPORT == caps.wTechnology)
         {
            if(-1 == firstMIDI)
            {
               firstMIDI = (sSI)winIdx;
            }

            r++;
         }
         else
         {
            if(-1 != firstMIDI)
            {
               // This should never be reached since it would mean that there isn't just a constant
               //  offset to the first MIDIPORT device but swsynth/real MIDI devices are mixed.
               yac_host->printf("[!!!] MIDIOut::GetNumDevices: unexpected non-MIDIPORT device at winIdx=%u (firstMIDI=%d)\n", winIdx, firstMIDI);
            }
         }
      }
   }

   return r;
}

void MIDIOut::GetDeviceNameByIdx(sUI _nr, YAC_Value *_r) {

   _nr += getFirstMIDIPortIndex();

   if(MMSYSERR_NOERROR == midiOutGetDevCaps(_nr, &midiout_caps, sizeof(midiout_caps)))
   {
      YAC_String *s = YAC_New_String();
      s->copy(midiout_caps.szPname);
      _r->initString(s, 1);
   }
   else
   {
      _r->initNull();
   }
}

sSI MIDIOut::GetDeviceTypeByIdx(sUI _nr) {

   // (note) this should be removed. only MIDIPORT type devices are supported.
   
   _nr += getFirstMIDIPortIndex();

   if(MMSYSERR_NOERROR == midiOutGetDevCaps(_nr, &midiout_caps, sizeof(midiout_caps)))
   {
      return (sSI) midiout_caps.wTechnology;
   }
   else
   {
      return -1;
   }
}
