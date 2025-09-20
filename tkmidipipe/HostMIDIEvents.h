/// HostMIDIEvents.h
///
/// (c) 2018-2024 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///
/// created: 31Jan2018
/// changed: 17Mar2018, 27Mar2018, 29Mar2018, 05May2018, 02Jun2018, 26Aug2019, 01Jul2024
///
///
///

#ifndef HOSTMIDIEVENTS_H__
#define HOSTMIDIEVENTS_H__

YG("midipipe");


struct HostMIDIEvent {
   sUI millisec;
   sUI dev_idx;
   sU8 midi_data[4];
};


YC class HostMIDIEvents : public YAC_Object {

   static const sUI MAX_MIDI_EVENTS = 1024u;

   static const sUI MAX_MIDI_DEVS = 8;  // a..h

   static const sSI EV_TYPE_NONE      = 0;
   static const sSI EV_TYPE_NOTE_ON   = 1;
   static const sSI EV_TYPE_NOTE_OFF  = 2;
   static const sSI EV_TYPE_PRGCHG    = 3;
   static const sSI EV_TYPE_PITCHBEND = 4;
   static const sSI EV_TYPE_PRESSURE  = 5;
   static const sSI EV_TYPE_CC        = 6;

//#define HOSTMIDIEVENT_TYPE_NONE       YCI 0
//#define HOSTMIDIEVENT_TYPE_NOTE_ON    YCI 1
//#define HOSTMIDIEVENT_TYPE_NOTE_OFF   YCI 2
//#define HOSTMIDIEVENT_TYPE_PRGCHG     YCI 3
//#define HOSTMIDIEVENT_TYPE_PITCHBEND  YCI 4
//#define HOSTMIDIEVENT_TYPE_PRESSURE   YCI 5
//#define HOSTMIDIEVENT_TYPE_CC         YCI 6

  public:
   HostMIDIEvent midi_events[MAX_MIDI_EVENTS];
   sUI num_midi_events;

   HostMIDIEvent last_seen_events[MAX_MIDI_DEVS][16];

  public:
   YAC(HostMIDIEvents);

   HostMIDIEvents(void);
   ~HostMIDIEvents();

   void  YAC_VCALL yacOperator (sSI, YAC_Object*, YAC_Value*);

  public:
   YM void empty (void);

   // also see VST2Plugin::queueHostVstMidiEventByFlt()
   YM void addMIDIShortEvent (sUI _millisec, sUI _devIdx, sUI _b0, sUI _b1, sUI _b2, sUI _b3);
   YM void addMIDIShortEvent32 (sUI _millisec, sUI _devIdx, sUI _shortMsg);

   YM sUI getNumMIDIEvents (void);

   YM sUI getFirstMillisec (void);
   YM void printMillisecs (void);

   // see ModMatrixEntry, Mod, and ModVST script classes. returns srcIdx or -1.
   //  also updates last_seen_events (MIDI learn)
   YM sSI modMatrixCheckEventByFlt (sUI _evIdx, sSI _fltDev, sSI _fltCh, sSI _forceCh, sSI _globalCh, sUI _minMillisec, sUI _maxMillisec, YAC_Object *_retVal, YAC_Object *_retVel);

   YM sSI getLastSeenModMatrixEventTypeByFlt(sUI _dev, sSI _fltCh, YAC_Object *_retCCNr);

   YM sF32 getNormalizedNoteOnVelocityByEventIdx(sUI _evIdx, sSI _fltDev, sSI _fltCh, sSI _forceCh, sSI _globalCh, sUI _minMillisec, sUI _maxMillisec);
   YM sBool setNormalizedNoteOnVelocityByEventIdx(sUI _evIdx, sSI _fltDev, sSI _fltCh, sSI _forceCh, sSI _globalCh, sUI _minMillisec, sUI _maxMillisec, sF32 _vel);

   YM sF32 getNormalizedNoteOffVelocityByEventIdx(sUI _evIdx, sSI _fltDev, sSI _fltCh, sSI _forceCh, sSI _globalCh, sUI _minMillisec, sUI _maxMillisec);
   YM sBool setNormalizedNoteOffVelocityByEventIdx(sUI _evIdx, sSI _fltDev, sSI _fltCh, sSI _forceCh, sSI _globalCh, sUI _minMillisec, sUI _maxMillisec, sF32 _vel);

};


#endif // HOSTMIDIEVENTS_H__
