/// HostMIDIEvents.cpp
///
/// (c) 2018-2024 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///
/// created: 31Jan2018
/// changed: 17Mar2018, 26Mar2018, 27Mar2018, 29Mar2018, 05May2018, 02Jun2018, 26Aug2019
///          14Feb2023, 01Jul2024
///
///
///

#include "tkmidipipe.h"

#include "HostMIDIEvents.h"

#include <math.h>


HostMIDIEvents::HostMIDIEvents(void) {
   num_midi_events = 0u;
   ::memset((void*)midi_events, 0, sizeof(midi_events));
   ::memset((void*)last_seen_events, 0, sizeof(last_seen_events));
}

HostMIDIEvents::~HostMIDIEvents() {
}

void YAC_VCALL HostMIDIEvents::yacOperator(sSI _cmd, YAC_Object *_o, YAC_Value *_r) {
   if(_o != ((YAC_Object*)this))
   {
      if(YAC_OP_ASSIGN == _cmd)
      {
         if(YAC_BCHK(_o, clid_HostMIDIEvents))
         {
            YAC_CAST_ARG(HostMIDIEvents, o, _o);

            num_midi_events = o->num_midi_events;

            for(sUI evIdx = 0u; evIdx < num_midi_events; evIdx++)
            {
               midi_events[evIdx] = o->midi_events[evIdx];
            }
         }
      }
   }
}

void HostMIDIEvents::empty(void) {
   num_midi_events = 0u;
}

void HostMIDIEvents::addMIDIShortEvent(sUI _millisec, sUI _devIdx, sUI _b0, sUI _b1, sUI _b2, sUI _b3) {

   if(num_midi_events < MAX_MIDI_EVENTS)
   {
      HostMIDIEvent *hev = &midi_events[num_midi_events++];

      hev->millisec = _millisec;
      hev->dev_idx  = _devIdx;

      hev->midi_data[0] = (sU8)_b0;
      hev->midi_data[1] = (sU8)_b1;
      hev->midi_data[2] = (sU8)_b2;
      hev->midi_data[3] = (sU8)_b3;
   }
   else
   {
      yac_host->printf("[~~~] HostMIDIEvents::addMIDIShortEvent: MAX_MIDI_EVENTS (%u) exceeded, discarding incoming event.\n", MAX_MIDI_EVENTS);
   }
}

void HostMIDIEvents::addMIDIShortEvent32(sUI _millisec, sUI _devIdx, sUI _shortMsg) {

   if(num_midi_events < MAX_MIDI_EVENTS)
   {
      HostMIDIEvent *hev = &midi_events[num_midi_events++];

      hev->millisec = _millisec;
      hev->dev_idx  = _devIdx;

      hev->midi_data[0] = (sU8)((_shortMsg      )&255u);
      hev->midi_data[1] = (sU8)((_shortMsg >>  8)&255u);
      hev->midi_data[2] = (sU8)((_shortMsg >> 16)&255u);
      hev->midi_data[3] = (sU8)((_shortMsg >> 24)&255u);
   }
   else
   {
      yac_host->printf("[~~~] HostMIDIEvents::addMIDIShortEvent32: MAX_MIDI_EVENTS (%u) exceeded, discarding incoming event.\n", MAX_MIDI_EVENTS);
   }
}

sUI HostMIDIEvents::getNumMIDIEvents(void) {
   return num_midi_events;
}

sUI HostMIDIEvents::getFirstMillisec(void) {
   // Debug
   sUI r = 0u;
   if(num_midi_events > 0u)
      r = midi_events[0].millisec;
   return r;
}

void HostMIDIEvents::printMillisecs(void) {
   // Debug
   yac_host->printf("xxx HostMIDIEvents: millisec=");

   for(sUI i = 0u; i < num_midi_events; i++)
   {
      yac_host->printf(", %u", midi_events[i].millisec);
   }
   yac_host->printf("\n");
}

sSI HostMIDIEvents::modMatrixCheckEventByFlt(sUI _evIdx, sSI _fltDev, sSI _fltCh, sSI _forceCh, sSI _globalCh, sUI _minMillisec, sUI _maxMillisec, YAC_Object *_retVal, YAC_Object *_retVel) {

   sSI src = -1;

   if( YAC_Is_Float(_retVal) && YAC_Is_Float(_retVel) )
   {
      YAC_CAST_ARG(YAC_Float, retVal, _retVal);
      YAC_CAST_ARG(YAC_Float, retVel, _retVel);  // alternative val (e.g. note on/on_vel)

      if(_evIdx < num_midi_events)
      {
         HostMIDIEvent *hev = &midi_events[_evIdx];

         if( (hev->millisec >= _minMillisec) && (hev->millisec < _maxMillisec) )
         {
            if( (-1 == _fltDev) || (sUI(_fltDev) == hev->dev_idx) )
            {
               if( (-1 == _fltCh) || (sUI(_fltCh) == (hev->midi_data[0] & 15u)) | (sUI(_globalCh) == (hev->midi_data[0] & 15u)) )
               {
                  // yac_host->printf("xxx update last_seen_events[%u][%u] data[1]=0x%08x\n", hev->dev_idx, (hev->vst.midiData[0] & 15u), hev->vst.midiData[1]);
                  last_seen_events[hev->dev_idx][(hev->midi_data[0] & 15u)] = *hev;

                  // (note) the defines match those in ModMatrixEntry.tks
#define OFF_MACROCTL        0
#define OFF_NOTE_ON        32
#define OFF_NOTE_ON_VEL    32   // same as OFF_NOTE_ON
#define OFF_NOTE_OFF       33
#define OFF_NOTE_OFF_VEL   33   // same as OFF_NOTE_OFF
#define OFF_PRGCHG         34
#define OFF_PITCHBEND      35
#define OFF_PRESSURE       36
#define OFF_CC             37
#define TOTAL_NUM_SOURCES  (OFF_CC + 128)  // => 164 src indices

                  switch(hev->midi_data[0] & 0xF0u)
                  {
                     default:
                        break;

                     case 0x80: // Note Off
                        src = OFF_NOTE_OFF;
                        retVal->value = hev->midi_data[1] / 127.0f;  // NoteNr
                        retVel->value = hev->midi_data[2] / 127.0f;  // Velocity
                        break;

                     case 0x90: // Note On
                        src = OFF_NOTE_ON;
                        retVal->value = hev->midi_data[1] / 127.0f;  // NoteNr
                        retVel->value = hev->midi_data[2] / 127.0f;  // Velocity
                        break;

                     case 0xA0: // Polyphonic Aftertouch
                        src = OFF_PRESSURE;
                        retVal->value = hev->midi_data[2] / 127.0f;  // Pressure
                        break;

                     case 0xB0: // CC
                        src = OFF_CC + hev->midi_data[1]/*ccnr*/;
                        retVal->value = hev->midi_data[2] / 127.0f;  // Value
                        break;

                     case 0xC0: // Prg.Chg
                        src = OFF_PRGCHG;
                        retVal->value = hev->midi_data[1] / 127.0f;  // PrgNr
                        break;

                     case 0xD0: // Channel Pressure
                        src = OFF_PRESSURE;
                        retVal->value = hev->midi_data[1] / 127.0f;  // Pressure
                        break;

                     case 0xE0: // Pitch Bend
                        src = OFF_PITCHBEND;
                        retVal->value = ((hev->midi_data[2]<<7)|hev->midi_data[1]) / 16383.0f;  // Pitchbend
                        break;
                  }
               }
            }
         }
      }
   }

   return src;
}

sF32 HostMIDIEvents::getNormalizedNoteOnVelocityByEventIdx(sUI _evIdx, sSI _fltDev, sSI _fltCh, sSI _forceCh, sSI _globalCh, sUI _minMillisec, sUI _maxMillisec) {

   if(_evIdx < num_midi_events)
   {
      HostMIDIEvent *hev = &midi_events[_evIdx];

      if( (hev->millisec >= _minMillisec) && (hev->millisec < _maxMillisec) )
      {
         if( (-1 == _fltDev) || (sUI(_fltDev) == hev->dev_idx) )
         {
            if( (-1 == _fltCh) || (sUI(_fltCh) == (hev->midi_data[0] & 15u)) | (sUI(_globalCh) == (hev->midi_data[0] & 15u)) )
            {
               switch(hev->midi_data[0] & 0xF0u)
               {
                  default:
                     break;

                  case 0x90u: // Note On
                     return hev->midi_data[2] / 127.0f;  // Velocity
               }
            }
         }
      }
   }

   return -1.0f;
}

sSI HostMIDIEvents::getLastSeenModMatrixEventTypeByFlt(sUI _dev, sSI _fltCh, YAC_Object *_retCCNr) {
   sSI r = -1;

   if(_dev < MAX_MIDI_DEVS)
   {
      if(YAC_Is_Integer(_retCCNr))
      {
         YAC_CAST_ARG(YAC_Integer, retCCNr, _retCCNr);

         const HostMIDIEvent *hevRet = NULL;

         // yac_host->printf("xxx getLastSeenModMatrixEventTypeByFlt: dev=%u fltCh=%d\n", _dev, _fltCh);

         if(-1 == _fltCh)
         {
            sSI bestMatchCh = -1;
            sUI bestMatchMS = 0u;

            for(sUI ch = 0u; ch < 16u; ch++)
            {
               const HostMIDIEvent *hev = &last_seen_events[_dev][ch];
               if(0u != hev->millisec)
               {
                  if((-1 == bestMatchCh) || (hev->millisec > bestMatchMS))
                  {
                     bestMatchCh = sSI(ch);
                     bestMatchMS = hev->millisec;
                  }
               }
            }

            if(-1 != bestMatchCh)
            {
               hevRet = &last_seen_events[_dev][bestMatchCh];
            }
         }
         else if(sUI(_fltCh) < 16u)
         {
            const HostMIDIEvent *hev = &last_seen_events[_dev][_fltCh];
            if(0u != hev->millisec)
            {
               hevRet = hev;
            }
         }

         // yac_host->printf("xxx getLastSeenModMatrixEventTypeByFlt: hevRet=%p\n", hevRet);

         if(NULL != hevRet)
         {
            switch(hevRet->midi_data[0] & 0xF0u)
            {
               default:
                  break;

               case 0x80: // Note Off
                  r = EV_TYPE_NOTE_OFF;
                  break;

               case 0x90: // Note On
                  r = EV_TYPE_NOTE_ON;
                  break;

               case 0xA0: // Polyphonic Aftertouch
                  r = EV_TYPE_PRESSURE;
                  break;

               case 0xB0: // CC
                  r = EV_TYPE_CC;
                  retCCNr->value = hevRet->midi_data[1];
                  break;

               case 0xC0: // Prg.Chg
                  r = EV_TYPE_PRGCHG;
                  break;

               case 0xD0: // Channel Pressure
                  r = EV_TYPE_PRESSURE;
                  break;

               case 0xE0: // Pitch Bend
                  r = EV_TYPE_PITCHBEND;
                  break;
            }
         }
      }
   }
   return r;
}

sBool HostMIDIEvents::setNormalizedNoteOnVelocityByEventIdx(sUI _evIdx, sSI _fltDev, sSI _fltCh, sSI _forceCh, sSI _globalCh, sUI _minMillisec, sUI _maxMillisec, sF32 _vel) {
   sSI vel = (sSI) (127.0f * _vel);
   if(vel < 1)
      vel = 1;
   else if(vel > 127)
      vel = 127;

   if(_evIdx < num_midi_events)
   {
      HostMIDIEvent *hev = &midi_events[_evIdx];

      if( (hev->millisec >= _minMillisec) && (hev->millisec < _maxMillisec) )
      {
         if( (-1 == _fltDev) || (sUI(_fltDev) == hev->dev_idx) )
         {
            if( (-1 == _fltCh) || (sUI(_fltCh) == (hev->midi_data[0] & 15u)) | (sUI(_globalCh) == (hev->midi_data[0] & 15u)) )
            {
               switch(hev->midi_data[0] & 0xF0u)
               {
                  default:
                     break;

                  case 0x90: // Note On
                     hev->midi_data[2] = vel;
                     return YAC_TRUE;
               }
            }
         }
      }
   }

   return YAC_FALSE;
}

sF32 HostMIDIEvents::getNormalizedNoteOffVelocityByEventIdx(sUI _evIdx, sSI _fltDev, sSI _fltCh, sSI _forceCh, sSI _globalCh, sUI _minMillisec, sUI _maxMillisec) {

   if(_evIdx < num_midi_events)
   {
      HostMIDIEvent *hev = &midi_events[_evIdx];

      if( (hev->millisec >= _minMillisec) && (hev->millisec < _maxMillisec) )
      {
         if( (-1 == _fltDev) || (sUI(_fltDev) == hev->dev_idx) )
         {
            if( (-1 == _fltCh) || (sUI(_fltCh) == (hev->midi_data[0] & 15u)) | (sUI(_globalCh) == (hev->midi_data[0] & 15u)) )
            {
               switch(hev->midi_data[0] & 0xF0u)
               {
                  default:
                     break;

                  case 0x80u: // Note Off
                     return hev->midi_data[2] / 127.0f;  // Velocity
               }
            }
         }
      }
   }

   return -1.0f;
}

sBool HostMIDIEvents::setNormalizedNoteOffVelocityByEventIdx(sUI _evIdx, sSI _fltDev, sSI _fltCh, sSI _forceCh, sSI _globalCh, sUI _minMillisec, sUI _maxMillisec, sF32 _vel) {
   sSI vel = (sSI) (127.0f * _vel);
   if(vel < 0)
      vel = 0;
   else if(vel > 127)
      vel = 127;

   if(_evIdx < num_midi_events)
   {
      HostMIDIEvent *hev = &midi_events[_evIdx];

      if( (hev->millisec >= _minMillisec) && (hev->millisec < _maxMillisec) )
      {
         if( (-1 == _fltDev) || (sUI(_fltDev) == hev->dev_idx) )
         {
            if( (-1 == _fltCh) || (sUI(_fltCh) == (hev->midi_data[0] & 15u)) | (sUI(_globalCh) == (hev->midi_data[0] & 15u)) )
            {
               switch(hev->midi_data[0] & 0xF0u)
               {
                  default:
                     break;

                  case 0x80: // Note Off
                     hev->midi_data[2] = vel;
                     return YAC_TRUE;
               }
            }
         }
      }
   }

   return YAC_FALSE;
}
