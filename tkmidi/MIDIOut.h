/// MIDIOut.h
///
/// (c) 2006-2020 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///
/// changed: 08Sep2006, 04Nov2007, 29Dec2008, 23Jan2011, 04Feb2014, 13Feb2015, 14Feb2015
///          17Jan2016, 29Feb2016, 04Mar2016, 12Nov2020
///
///
///

YG("midi");

//#define MIDIOUT_BUFSIZE 4096  // this one seems to work reliably (on win7)
//#define MIDIOUT_BUFSIZE 8192   // win7 driver bug ?
/* #define MIDIOUT_BUFSIZE 16384 */
#define MIDIOUT_BUFSIZE 32768  // [12Nov2020] increased from 16k to 32k for Elektron Model Cycles kit dumps
/* #define MIDIOUT_NUMBUFFERS 256 */
#define MIDIOUT_NUMBUFFERS 32


/* @class MIDIOut,Object
Represents a MIDI output device
*/
YC class MIDIOut : public YAC_Object {
public:
   HMIDIOUT midiout_handle;

   MIDIOUTCAPS midiout_caps;

   YAC_String device_name;

   sSI device_idx;

   sUI   send_fail_count;  // after disconnect, force device reopen after >=4 fails 
   sBool b_reopen_after_fail;
   sSI   reopen_device_idx;

public:
   YAC(MIDIOut);

   MIDIOut(void);
   ~MIDIOut();

public:
   sBool openByIdxInt (sUI _nr, sBool _bQuiet);

public:
   /* @constant MOD_MIDIPORT,int
    */
   //#define MOD_MIDIPORT  YCI 1


   /* @constant MOD_SYNTH,int
    */
   //#define MOD_SYNTH     YCI 2


   /* @constant MOD_SQSYNTH,int
    */
   //#define MOD_SQSYNTH   YCI 3


   /* @constant MOD_FMSYNTH,int
    */
   //#define MOD_FMSYNTH   YCI 4


   /* @constant MOD_MAPPER,int
    */
   //#define MOD_MAPPER    YCI 5


   /* @constant MOD_WAVETABLE,int
    */
   //#define MOD_WAVETABLE YCI 6


   /* @constant MOD_SWSYNTH,int
    */
   //#define MOD_SWSYNTH   YCI 7

   
public:
   ////void msg3(sU8 _1, sU8 _2, sU8 _3);

   MIDIHDR midi_buffers[MIDIOUT_NUMBUFFERS];
   sU8 midi_buffer_data[MIDIOUT_NUMBUFFERS * MIDIOUT_BUFSIZE];
   sU8 curr_buffer_index;

   sSI getFirstMIDIPortIndex(void); // for skipping non-MIDIPORT devs (e.g. the MS wavetable synth)

   void initMidiBuffers  (void);
   sSI  allocBufferIndex (sUI _size);
   void sendBufferIndex  (sSI _idx);
   sU8* getBufferData    (sSI _idx);


public:
   /* @method openByName,String devName:boolean
     Open MIDI device by name

     @arg devName Device name (platform dependent)
     @return true(1) if the device has been opened, false(0) if an error occured
   */
   YM sBool _openByName  (YAC_Object *_devName);


   /* @method openByIdx,int devIdx:boolean
     Open MIDI device by index
 
     @arg devIdx Device index
     @return true(1) if the device has been opened, false(0) if an error occurred
   */
   YM sBool _openByIdx   (sUI _nr);


   /* @method close
     Close MIDI device
   */
   YM void  _close       (void);


   /* @method noteOn,byte midiCh,byte note,byte vel
     Send note-on MIDI message

    @arg midiCh MIDI channel (0..15)
    @arg note MIDI note (0..127)
    @arg vel Note-on velocity (0..127)
   */
   YM void  noteOn       (sUI _midiCh, sUI _note, sUI _vel);


   /* @method noteOff,byte midiCh,byte note,byte vel
     Send note-off MIDI message

    @arg midiCh MIDI channel (0..15)
    @arg note MIDI note (0..127)
    @arg vel Note-off velocity (0..127)
   */
   YM void  noteOff      (sUI _midiCh, sUI _note, sUI _vel);


   /* @method allNotesOff,byte midiCh
     Send all-notes-off MIDI message

     @arg midiCh MIDI channel (0..15)
   */
   YM void  allNotesOff  (sUI _midiCh);


   /* @method bankSelect,byte midiCh,byte bank
    Send bank-select MIDI message

    @arg midiCh MIDI channel (0..15)
    @arg bank Bank (0..127)
   */
   YM void  bankSelect   (sUI _midiCh, sUI _bank);


   /* @method programChange,byte midiCh,byte prog
    Send program-select MIDI message

    @arg midiCh MIDI channel (0..15)
    @arg prog Program (0..127)
   */
   YM void  programChange(sUI _midiCh, sUI _prog);


   /* @method sendBuffer,Buffer b:boolean
    Send buffer (regular or system exclusive data)

    This method can also be used to send a series of regular MIDI commands.

    @arg b Buffer that holds the MIDI data to be send. The "offset" field is used to determine how many bytes to send.
    @return true(1) if the data has been sent, false(0) otherwise
   */
   YM int   sendBuffer   (YAC_Object *_buffer);


   /* @method getDeviceName:String
    Return name of currently opened MIDI device
 
    @return MIDI device name
   */
   YM void  getDeviceName(YAC_Value *_r);


   /* @method getDeviceIdx:int
    Return device index.

    This can be used for finding the matching input device for an output device.
   */
   YM sSI getDeviceIdx(void);


   // Pseudo-Static methods:


   /* @method GetNumDevices:int
     Query number of available MIDI output device (pseudo-static method)

     @return Number of available MIDI output devices
   */
   YM sSI  GetNumDevices (void);


   /* @method GetDeviceNameByIdx,int devIdx:String
     Query MIDI device name by index  (pseudo-static method)

     @arg devIdx MIDI output device index (0..GetNumDevices())
     @return MIDI device name
   */
   YM void GetDeviceNameByIdx (sUI _nr, YAC_Value *_r);


   /* @method GetDeviceTypeByIdx,int:int
     Query MIDI device caps (pseudo-static method)

     The returned value should be considered merely a "hint": Not all audio drivers may support this.

     @return Device capabilities/type. One of §MOD_MIDIPORT, §MOD_SYNTH, §MOD_SQSYNTH, §MOD_FMSYNTH, §MOD_MAPPER, §MOD_WAVETABLE, §MOD_SWSYNTH. -1 if this call is not supported.
   */
   YM sSI  GetDeviceTypeByIdx (sUI _nr);

};
