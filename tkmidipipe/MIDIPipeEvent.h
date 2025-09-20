// ----
// ---- file   : MIDIPipeEvent.h
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2014-2024 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : This is part of the "syntracker" sequencer.
// ----
// ---- created: 01Sep2014
// ---- changed: 06Sep2014, 07Sep2014, 11Jan2015, 16Feb2015, 26Mar2015, 30Jun2015, 27Feb2016
// ----          10Feb2017, 25Jun2019, 02Aug2019, 07Jan2022, 06Oct2024
// ----
// ----
// ----

#ifndef TK_MIDIPIPEEVENT_H__
#define TK_MIDIPIPEEVENT_H__

YG("midipipe")


/* @class MIDIPipeEvent,Object
 */
YC class MIDIPipeEvent : public YAC_Object {

#define MIDIPIPE_EVENT_TYPE_NONE          YCI  (0u)
#define MIDIPIPE_EVENT_TYPE_PRGCHG        YCI  (1u)
#define MIDIPIPE_EVENT_TYPE_CHPRESSURE    YCI  (2u)
#define MIDIPIPE_EVENT_TYPE_PITCHBEND     YCI  (3u)
#define MIDIPIPE_EVENT_TYPE_CC            YCI  (4u)
#define MIDIPIPE_EVENT_TYPE_RPN           YCI  (5u)
#define MIDIPIPE_EVENT_TYPE_NRPN          YCI  (6u)
#define MIDIPIPE_EVENT_TYPE_NOTE_OFF      YCI  (7u)
#define MIDIPIPE_EVENT_TYPE_NOTE_ON       YCI  (8u)
#define MIDIPIPE_EVENT_TYPE_POLYPRESSURE  YCI  (9u)

#define MIDIPIPE_EVENT_TYPE_NOTES         YCI  (10u)  // special io_filter case

#define MIDIPIPE_EVENT_TYPE_ALL_NOTES_OFF YCI  (11u)
#define MIDIPIPE_EVENT_TYPE_SYSEX         YCI  (12u)
#define MIDIPIPE_EVENT_TYPE_SYSRT         YCI  (13u)


#define MIDIPIPE_EVENT_SYSRT_TIMING_CLOCK    YCI (0xF8)  // 0xF8 timing clock (24ppq)
#define MIDIPIPE_EVENT_SYSRT_F9              YCI (0xF9)  // 0xF9 (undefined)
#define MIDIPIPE_EVENT_SYSRT_START           YCI (0xFA)  // 0xFA start
#define MIDIPIPE_EVENT_SYSRT_CONTINUE        YCI (0xFB)  // 0xFB continue
#define MIDIPIPE_EVENT_SYSRT_STOP            YCI (0xFC)  // 0xFC stop
#define MIDIPIPE_EVENT_SYSRT_FD              YCI (0xFD)  // 0xFD (undefined)
#define MIDIPIPE_EVENT_SYSRT_ACTIVE_SENSING  YCI (0xFE)  // 0xFE active sensing
#define MIDIPIPE_EVENT_SYSRT_SYSTEM_RESET    YCI (0xFF)  // 0xFF system reset

  public:
   sUI event_index;   // absolute index in MIDIPipeFrame
   sUI event_type;

   midipipe_frame_u data;

  public:
   MIDIPipeEvent(void);
   ~MIDIPipeEvent();

   YAC(MIDIPipeEvent);

  public:

  public:
   YM sUI  getIndex (void);
   YM sUI  getType (void);
   YM void getTypeName (YAC_Value *_r);
   YM sSI  getDevIdx (void);
   YM sSI  getMidiCh (void);
   YM sSI  getPrgChg (void);
   YM sF32 getChPressure (void);
   YM sF32 getPitchbend (void);
   YM sSI  getCcId (void);
   YM sF32 getCcValue (void);
   YM sSI  getRpn (void);
   YM sF32 getRpnValue (void);
   YM sSI  getNrpn (void);
   YM sF32 getNrpnValue (void);
   YM sSI  getNote (void);
   YM sF32 getVelocity (void);
   YM sSI  getDuration (void);
   YM sSI  getPolyPressureNote (void);
   YM sF32 getPolyPressureValue (void);

   YM sF32 getArg1 (void);
   YM sF32 getArg2 (void);
   YM sSI  getArg3 (void);

   YM void getArg1Name (YAC_Value *_r);
   YM void getArg2Name (YAC_Value *_r);
   YM void getArg3Name (YAC_Value *_r);

   YM sBool isNote (void);
   YM sBool isNoteOffPre (void);

   /* void setArg1(sSI _val); */
   /* void setArg2(sSI _val); */
   /* void setArg3(sSI _val); */

   YM YAC_Buffer *getSysExBuffer (void);

   YM sUI getSysRTByte (void);
};


#endif // TK_MIDIPIPEEVENT_H__
