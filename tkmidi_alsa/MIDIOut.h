/// MIDIOut.h
///
/// (c) 2006-2020 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///
/// changed: 08Sep2006, 04Nov2007, 29Dec2008, 04Feb2014, 05Feb2014, 29Mar2019, 12Nov2020
///
///

YG("midi");

/* #define MIDIOUT_BLOCKSIZE  (4 * 1024) */
#define MIDIOUT_BLOCKSIZE  (32 * 1024)  // [12Nov2020] increased from 4kk to 32k for Elektron Model Cycles kit dumps


/* @class MIDIOut,Object
Represents a MIDI output device
*/
YC class MIDIOut : public YAC_Object {
  public:
   snd_rawmidi_t *midiout_handle;
   sSI device_idx;

   YAC_String device_name; // e.g. "MIDISPORT 4x4 Anniv"
   YAC_String device_path; // "hw:<card>,<dev>,<subdev>"

  public:
   static AlsaDeviceInfo *alsa_device_infos;
   static sUI num_alsa_device_infos;
   

  public:
   YAC(MIDIOut);

   MIDIOut(void);
   ~MIDIOut();

  public:
   static void            AllocAlsaDeviceInfos   (void);
   static void            FreeAlsaDeviceInfos    (void);
   static AlsaDeviceInfo *GetAlsaDeviceInfoByIdx (sUI _idx);

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
   sBool alsaWrite(sU8 *_buf, sUI _numBytes);


public:
   /* @method openByName,String devName:boolean
     Open MIDI device by name

     @arg devName Device name (platform dependent)
     @return true(1) if the device has been opened, false(0) if an error occured
   */
   YM sBool _openByName  (YAC_Object *_devName);


   /* @method openByIdx,int idx:boolean
     Open MIDI device by index

     @arg idx Device index (0..(GetNumDevices-1))
     @return true(1) if the device has been opened, false(0) if an error occured
   */
   YM sBool _openByIdx   (sUI _idx);


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
   YM sUI  GetNumDevices (void);


   /* @method GetDeviceNameByIdx,int devIdx:String
     Query MIDI device name by index  (pseudo-static method)

     @arg devIdx MIDI output device index (0..GetNumDevices())
     @return MIDI device name
   */
   YM void GetDeviceNameByIdx (sUI _nr, YAC_Value *_r);


   /* @method GetDeviceTypeByIdx,int:int
     Query MIDI device caps (pseudo-static method)

     The returned value should be considered merely a "hint": Not all audio drivers may support this.

     @return Device capabilities/type. One of §MOD_MIDIPORT, §MOD_SYNTH, §MOD_SQSYNTH, §MOD_FMSYNTH, §MOD_MAPPER, §MOD_WAVETABLE, §MOD_SWSYNTH.
   */
   YM sSI  GetDeviceTypeByIdx (sUI _nr);

   
};
