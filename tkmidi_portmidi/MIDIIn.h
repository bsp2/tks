/// MIDIIn.h
///
/// (c) 2006-2023 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///
/// changed: 09Sep2006, 04Nov2007, 02Jan2008, 13Jan2008, 29Dec2008, 09Nov2012, 23Mar2013
///          14Apr2013, 04Feb2014, 24Feb2014, 11Feb2015, 14Feb2015, 02Jun2018, 22Jan2019
///          12Nov2020, 20Jan2023, 22Jul2023, 01Aug2023
///
///
///

YG("midi");

#define MIDIIN_BLOCKSIZE 32768 // Maximum size of sysex message  // [12Nov2020] increased from 16k to 32k for Elektron Model Cycles kit dumps
#define MIDIIN_NUMBUFFERS 32
//#define MIDIIN_MAXEVENTS 256 // Size of RecordedMIDIEvent history
#define MIDIIN_MAXEVENTS 1024 // Size of RecordedMIDIEvent history

class MIDIIn;


class RecordedMIDIEvent;
typedef struct midiin_reader_state_s {
#define ST_ANY        (0u)  // Waiting for first byte of next message
#define ST_SHORTMSG1  (1u)  // One short message byte to go
#define ST_SHORTMSG2  (2u)  // Two short message bytes to go
#define ST_SYSEX      (3u)  // Read until end of sysex (0xF7) or next status byte (faulty message)
   sU8                running_status; // 0 = none, 0x9x=note on, ...
   sUI                state;
   RecordedMIDIEvent *ev;
   sUI                off;
   sBool              b_filtered;
   void init(void) {
      running_status = 0u;
      state          = ST_ANY;
      ev             = NULL;
      off            = 0u;
      b_filtered     = YAC_FALSE;
   }
} midiin_reader_state_t;


/* @class RecordedMIDIEvent,Object

Represents a time-stamped incoming MIDI event.
*/
YC class RecordedMIDIEvent : public YAC_Object {
   public:
      sUI millisec;    // milliseconds (via yac_host->yacMilliSeconds())
      sBool b_long;    // true==long (sysex) message (data.ptr), size), false=short msg (data.msg)
      union {
         sU8 *ptr;     // b_long==true, Reference to MIDIIn::midi_buffer_data element (excl F0/F7)
         sUI u32;      // b_long==false, e.g. 0x007b48b1
         sU8 u8[4];
      } data;
      sUI size;        // Number of data bytes (only valid when b_long == true)
      sU8 midimap_event_type;
      sU16 n_rpn;      // (non-)registered parameter id (valid if midimap_event_type is TYPE_RPN or TYPE_NRPN)
      sU16 data_entry; // (N)RPN data value

  public:
      void classifyShortMessage (MIDIIn *thiz, sBool *bFiltered);
      void classifyCtlChange (MIDIIn *thiz, sBool *bFiltered);

   public:
      YAC(RecordedMIDIEvent);

      RecordedMIDIEvent(void);
      ~RecordedMIDIEvent();

      /*@method getMillisec:int
        Query timestamp

        @return timestamp
      */
      YM sSI  getMillisec      (void);


      /* @method isLongMessage:boolean
        Query whether this is a "long" MIDI message.

       @return true(1) if message is longer than 4 bytes, false(0) otherwise.
      */
      YM sBool isLongMessage    (void);


      /* @method getShortMessage:int
        Return <=4 byte message as host byte order integer

       @return Message data
      */
      YM sUI  getShortMessage  (void);


      /* @method getSize:int
        Query message size

       @return Message size
      */
      YM sSI  getSize          (void);


      /* @method copyToStream,Stream stream
       Copy message data to stream object (e.g. §Buffer, §File etc..)

      @arg stream Where to write the message data
      */
      YM void copyToStream     (YAC_Object *_stream); // Copy message data to stream object


      /* @method getMidiMapEventType:byte
        Return §MIDIMapEventType type id

       @return Event type
      */
      YM sSI getMidiMapEventType (void);


      /* @method getNrpnId:short
         Return (N)RPN id. Only valid if midiMapEventType is TYPE_RPN or TYPE_NRPN.

         @return (N)RPN id
      */
      YM sSI getNrpnId(void);


      /* @method getDataEntry:short
         Return (N)RPN data entry value. Only valid if midiMapEventType is TYPE_RPN or TYPE_NRPN.

         @return (N)RPN data entry value
      */
      YM sUI getDataEntry(void);

};



/* @class MIDIIn,Object
Represents a MIDI input device.


The MIDIIn class uses a ring buffer to record incoming MIDI events.

Each event is time-stamped for later evaluation in e.g. a sequencer application.
*/
YC class MIDIIn : public YAC_Object {
public:

#define MIDI_DATAENTRYMODE_MSBLSB  YCI 0   // e.g. DSI Prophet 12
#define MIDI_DATAENTRYMODE_LSBMSB  YCI 1
#define MIDI_DATAENTRYMODE_MSB     YCI 2   // e.g. Novation Peak


   PortMidiStream *pm_stream;
   YAC_String      device_name;
   sSI             device_idx;
   sBool           b_open;

   // MIDI filter, 1=filter out event, 0=pass event
   sBool b_filter_note_off;          // 0x8x
   sBool b_filter_note_on;           // 0x9x
   sBool b_filter_poly_pressure;     // 0xAx
   sBool b_filter_control_change;    // 0xBx
   sBool b_filter_program_change;    // 0xCx
   sBool b_filter_channel_pressure;  // 0xDx
   sBool b_filter_pitch_bend;        // 0xEx
   sBool b_filter_sysex;             // 0xF0
   sBool b_filter_cm_time_code;      // 0xF1
   sBool b_filter_cm_song_position;  // 0xF2
   sBool b_filter_cm_song_select;    // 0xF3
   sBool b_filter_cm_tune_request;   // 0xF6
   sBool b_filter_rt_timing_clock;   // 0xF8
   sBool b_filter_rt_start;          // 0xFA
   sBool b_filter_rt_continue;       // 0xFB;
   sBool b_filter_rt_stop;           // 0xFC
   sBool b_filter_rt_active_sensing; // 0xFE;
   sBool b_filter_rt_system_reset;   // 0xFF;
   sBool b_filter_rpn_select;        // CC#101(MSB) + CC#100(LSB)
   sBool b_filter_nrpn_select;       // CC#099(MSB) + CC#098(LSB)
   sBool b_filter_rpn;               // CC#006(MSB) + CC#038(LSB) (if -1 != rpn)
   sBool b_filter_nrpn;              // CC#006(MSB) + CC#038(LSB) (if -1 != nrpn)

   sBool b_parse_param[16];  // 1=parse rpn+nrpn messages. 0=treat (n)rpn msgs like regular CCs

   sS16 rpn [16]; // last seen RPN# or -1 if NRPN is (possibly) active
   sS16 nrpn[16]; // last seen NRPN# or -1 if RPN is (possibly) active
   sU16 data_entry[16]; // joint MSB/LSB dataentry

   /* sBool b_dataentry_msblast[16]; // 0=device sends data entry in MSB,LSB order; 1=LSB,MSB order */
   sU8 data_entry_mode[16];  // 0=device sends data entry in MSB,LSB order; 1=LSB,MSB order; 2=MSB only

   // sU8 running_status; // 0 = none, 0x9x=note on, ...

   midiin_reader_state_t virt_state;  // see parseBuffer()

public:
   YAC(MIDIIn);

   MIDIIn(void);
   ~MIDIIn();

   void allocEventsAndMutex (void);
   void freeEventsAndMutex (void);
   void parseMIDIByte (const sU8 c, midiin_reader_state_t &st);

public:
   sU8 midi_buffer_data[MIDIIN_NUMBUFFERS * MIDIIN_BLOCKSIZE];
   sU8 curr_buffer_index;
   RecordedMIDIEvent *recorded_events[MIDIIN_MAXEVENTS]; // Used as a ringbuffer (writeoff wraps around)
   sUI record_readoff;   // Offset of first unread event
   sUI record_writeoff;  // Offset of next event to be written
   sUI record_numevents; // Number of unread events. Note: remember to wrap around (ringbuf!) when reading events
   YAC_MutexHandle mtx_record;

   void  open2                (void); // Called when device has been openend
   /* void  addRecordedLongEvent (sUI _millisec, MIDIHDR *_hdr); */
   /* void  addRecordedShortEvent(sUI _millisec, sUI _msg, sUI _size, sU8 _midimapType, sU16 _nRpn); // size= 0..3 */
   sBool isOpen               (void);

protected:
   RecordedMIDIEvent *beginRecordedEvent (void);
   void               endRecordedEvent   (RecordedMIDIEvent *_ev);
   sU8  *getNextSysExBuffer (void);
   void resetState (void);

   // input reader state
   struct {
      RecordedMIDIEvent *ev; // current MIDI event
      sBool b_sysex;  // true=parsing sysex message
      sUI sysex_off;  // SysEx buffer write offset
   } state;

   void consumeMessage (const sU32 _msg);
   void readPMEvents (void);  // read next available portmidi events and store in recorded_events ring buffer

public:
   /* @method openByName,String devName:boolean
    Open MIDI input device by name

    @arg devName Name of the input device (platform dependent)
    @return true(1) if the device has been opened, false(0) if an error occured.
   */
   YM sBool _openByName  (YAC_Object *_devName);


   /* @method openByIdx,int devIdx:boolean
    Open MIDI input device by index

    @arg devIdx Index of the input device
    @return true(1) if the device has been opened, false(0) if an error occured.
   */
   YM sBool _openByIdx   (sUI _nr);


   /* @method openLocalMIDI
    Pseudo-open function for shared memory LocalMIDI input
    @see parseBuffer
   */
   YM void _openLocalMIDI (void);


   /* @method close
    Close MIDI device
   */
   YM void  _close       (void);

   /* @method start
    Start input event recording
   */
   YM void        _start         (void);


   YM sBool _isAlive (void);


   /* @method stop
    Stop input event recording
   */
   YM void        _stop          (void);


   /* @method reset
    Reset MIDI device
   */
   YM void        _reset         (void);


   /* @method clearEvents
    Discard previously recorded MIDI events
   */
   YM void        _clearEvents   (void);


   /* @method getDeviceName:String
    Query MIDI device name

    @return MIDI device name
   */
   YM void        _getDeviceName (YAC_Value *_r);


   /* @method getDeviceIdx:int
    Return device index.

    This can be used for finding the matching output device for an input device.
   */
   YM sSI getDeviceIdx(void);


   /* @method getNextEvent:RecordedMIDIEvent
     Return RecordedMIDIEvent* or null if no event is available

     @return Next MIDI event or null
   */
   YM YAC_Object *_getNextEvent  (void);


   /* @method waitNextEvent,int timeout:RecordedMIDIEvent
     Wait for next event and return RecordedMIDIEvent* or null if no event is available and the timeout expired.

     (note) mainly used for debugging purposes (test programs)

     @arg timeout Maximum wait time in milliseconds
     @return Next MIDI event or null
   */
   YM YAC_Object *_waitNextEvent  (sUI _timeout);


   /* @method getNumEvents:int
     Query number of recorded input events

    @return Number of recorded input events
   */
   YM sSI         _getNumEvents  (void); // Get number of queued RecordedMIDIEvents


   // Pseudo-Static methods
   /* @method GetNumDevices:int
     Query number of available MIDI input devices (pseudo-static method)

     @return Number of available MIDI input devices
   */
   YM sSI  GetNumDevices (void);


   /* @method GetDeviceNameByIdx,int devIdx:String
     Query device name by index (pseudo-static method)

     @arg devIdx MIDI input device index (0..GetNumDevices())
     @return Device name string
   */
   YM void GetDeviceNameByIdx (sUI _nr, YAC_Value *_r);


   /* @method setFltNoteOff,boolean
      Configure note-off filter.

      @arg bEnable true to enable note off filter, false to disable it.
   */
   YM void setFltNoteOff(sSI _bEnable);


   /* @method getFltNoteOff:boolean
      Query note-off filter.

      @return true if note off filter is currently enabled
   */
   YM sSI getFltNoteOff(void);


   /* @method setFltNoteOn,boolean
      Configure note-off filter.

      @arg bEnable true to enable filter, false to disable it.
   */
   YM void setFltNoteOn(sSI _bEnable);


   /* @method getFltNoteOn:boolean
      Query note-on filter.

      @return true if filter is currently enabled
   */
   YM sSI getFltNoteOn(void);


   /* @method setFltPolyPressure,boolean
      Configure polyphonic key pressure filter.

      @arg bEnable true to enable filter, false to disable it.
   */
   YM void setFltPolyPressure(sSI _bEnable);


   /* @method getFltPolyPressure:boolean
      Query polyphonic key pressure filter.

      @return true if filter is currently enabled
   */
   YM sSI getFltPolyPressure(void);


   /* @method setFltCC,boolean
      Configure continuous controller filter.

      @arg bEnable true to enable filter, false to disable it.
   */
   YM void setFltCC(sSI _bEnable);


   /* @method getFltCC:boolean
      Query continuous controller filter.

      @return true if filter is currently enabled
   */
   YM sSI getFltCC(void);


   /* @method setFltProgramChange,boolean
      Configure program change filter.

      @arg bEnable true to enable filter, false to disable it.
   */
   YM void setFltProgramChange(sSI _bEnable);


   /* @method getFltProgramChange:boolean
      Query program change filter.

      @return true if filter is currently enabled
   */
   YM sSI getFltProgramChange(void);


   /* @method setFltChannelPressure,boolean
      Configure channel pressure filter.

      @arg bEnable true to enable filter, false to disable it.
   */
   YM void setFltChannelPressure(sSI _bEnable);


   /* @method getFltChannelPressure:boolean
      Query channel pressure filter.

      @return true if filter is currently enabled
   */
   YM sSI getFltChannelPressure(void);


   /* @method setFltPitchbend,boolean
      Configure pitch bend filter.

      @arg bEnable true to enable filter, false to disable it.
   */
   YM void setFltPitchbend(sSI _bEnable);


   /* @method getFltPitchbend:boolean
      Query pitch bend filter.

      @return true if filter is currently enabled
   */
   YM sSI getFltPitchbend(void);


   /* @method setFltSysEx,boolean
      Configure system exclusive filter.

      @arg bEnable true to enable filter, false to disable it.
   */
   YM void setFltSysEx(sSI _bEnable);


   /* @method getFltSysEx:boolean
      Query system exclusive filter.

      @return true if filter is currently enabled
   */
   YM sSI getFltSysEx(void);


   /* @method setFltSysComTimeCode,boolean
      Configure system common MIDI quarter tick time code filter.

      @arg bEnable true to enable filter, false to disable it.
   */
   YM void setFltSysComTimeCode(sSI _bEnable);


   /* @method getFltSysComTimeCode:boolean
      Query system common MIDI quarter tick time code filter.

      @return true if filter is currently enabled
   */
   YM sSI getFltSysComTimeCode(void);


   /* @method setFltSysComSongPosition,boolean
      Configure system common song position filter.

      @arg bEnable true to enable filter, false to disable it.
   */
   YM void setFltSysComSongPosition(sSI _bEnable);


   /* @method getFltSysComSongPosition:boolean
      Query system common song position filter.

      @return true if filter is currently enabled
   */
   YM sSI getFltSysComSongPosition(void);


   /* @method setFltSysComSongSelect,boolean
      Configure system common song select filter.

      @arg bEnable true to enable filter, false to disable it.
   */
   YM void setFltSysComSongSelect(sSI _bEnable);


   /* @method getFltSysComSongSelect:boolean
      Query system common song select filter.

      @return true if filter is currently enabled
   */
   YM sSI getFltSysComSongSelect(void);


   /* @method setFltSysComTuneRequest,boolean
      Configure system common tune request filter.

      @arg bEnable true to enable filter, false to disable it.
   */
   YM void setFltSysComTuneRequest(sSI _bEnable);


   /* @method getFltSysComTuneRequest:boolean
      Query system common tune request filter.

      @return true if filter is currently enabled
   */
   YM sSI getFltSysComTuneRequest(void);


   /* @method setFltSysRTTimingClock,boolean
      Configure system realtime timing clock filter.

      @arg bEnable true to enable filter, false to disable it.
   */
   YM void setFltSysRTTimingClock(sSI _bEnable);


   /* @method getFltSysRTTimingClock:boolean
      Query system realtime timing clock filter.

      @return true if filter is currently enabled
   */
   YM sSI getFltSysRTTimingClock(void);


   /* @method setFltSysRTStart,boolean
      Configure system realtime start filter.

      @arg bEnable true to enable filter, false to disable it.
   */
   YM void setFltSysRTStart(sSI _bEnable);


   /* @method getFltSysRTStart:boolean
      Query system realtime start filter.

      @return true if filter is currently enabled
   */
   YM sSI getFltSysRTStart(void);


   /* @method setFltSysRTContinue,boolean
      Configure system realtime continue filter.

      @arg bEnable true to enable filter, false to disable it.
   */
   YM void setFltSysRTContinue(sSI _bEnable);


   /* @method getFltSysRTContinue:boolean
      Query system realtime continue filter.

      @return true if filter is currently enabled
   */
   YM sSI getFltSysRTContinue(void);


   /* @method setFltSysRTStop,boolean
      Configure system realtime stop filter.

      @arg bEnable true to enable filter, false to disable it.
   */
   YM void setFltSysRTStop(sSI _bEnable);


   /* @method getFltSysRTStop:boolean
      Query system realtime stop filter.

      @return true if filter is currently enabled
   */
   YM sSI getFltSysRTStop(void);


   /* @method setFltSysRTActiveSensing,boolean
      Configure system realtime active sensing filter.

      @arg bEnable true to enable filter, false to disable it.
   */
   YM void setFltSysRTActiveSensing(sSI _bEnable);


   /* @method getFltSysRTActiveSensing:boolean
      Query system realtime active sensing filter.

      @return true if filter is currently enabled
   */
   YM sSI getFltSysRTActiveSensing(void);


   /* @method setFltSysRTSystemReset,boolean
      Configure system realtime system reset filter.

      @arg bEnable true to enable filter, false to disable it.
   */
   YM void setFltSysRTSystemReset(sSI _bEnable);


   /* @method getFltSysRTSystemReset:boolean
      Query system realtime system reset filter.

      @return true if filter is currently enabled
   */
   YM sSI getFltSysRTSystemReset(void);


   /* @method setFltRpnSelect,boolean
      Configure RPN select filter.

      @arg bEnable true to enable filter, false to disable it.
   */
   YM void setFltRpnSelect(sSI _bEnable);


   /* @method getFltRpnSelect:boolean
      Query RPN select filter.

      @return true if filter is currently enabled
   */
   YM sSI getFltRpnSelect(void);


   /* @method setFltNrpnSelect,boolean
      Configure NRPN select filter.

      @arg bEnable true to enable filter, false to disable it.
   */
   YM void setFltNrpnSelect(sSI _bEnable);


   /* @method getFltNrpnSelect:boolean
      Query NRPN select filter.

      @return true if filter is currently enabled
   */
   YM sSI getFltNrpnSelect(void);


   /* @method setFltRpn,boolean
      Configure RPN filter.

      @arg bEnable true to enable filter, false to disable it.
   */
   YM void setFltRpn(sSI _bEnable);


   /* @method getFltRpn:boolean
      Query RPN filter.

      @return true if filter is currently enabled
   */
   YM sSI getFltRpn(void);


   /* @method setFltNrpn,boolean
      Configure NRPN filter.

      @arg bEnable true to enable filter, false to disable it.
   */
   YM void setFltNrpn(sSI _bEnable);


   /* @method getFltNrpn:boolean
      Query NRPN filter.

      @return true if filter is currently enabled
   */
   YM sSI getFltNrpn(void);


   /* @method getRpn:short
      Query last seen RPN.

      @return RPN or -1 if NRPN is (possibly) active
   */
   /* sSI getRpn(void); */


   /* @method getNrpn:short
      Query last seen NRPN.

      @return NRPN or -1 if RPN is (possibly) active
   */
    /* sSI getNrpn(void); */


   /* @method setEnableParam,boolean
      Switch (N)RPN parser on or off.

      @arg bEnable true to enable parser, false to disable it.
   */
   YM void setEnableParam(sSI _ch, sSI _bEnable);


   /* @method getEnableParam:boolean
      Query (N)RPN parser state (on/off).

      @return true if parser is currently enabled
   */
   YM sSI getEnableParam(sSI _ch);


   /* @method setDataEntryMode,int
      Set (N)RPN data entry mode.

      @arg mode 0=MSBLSB, 1=LSBMSB, 2=MSB
   */
   YM void setDataEntryMode (sSI _ch, sSI _mode);


   /* @method getDataEntryMode:int
      Query (N)RPN data entry mode.

      @return 0=MSBLSB, 1=LSBMSB, 2=MSB
   */
   YM sSI getDataEntryMode (sSI _ch);


   /* @method parseBuffer,Buffer buffer
      Parse b.offset bytes and add events. Does not require MIDIIn to be opened.
      Used by LocalMIDI module.
      @arg buffer Input buffer
   */
   YM void parseBuffer (YAC_Buffer *_buffer);

};
