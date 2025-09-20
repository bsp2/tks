// ----
// ---- file   : MIDIPipeFrame.h
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2014-2025 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : This is part of the "syntracker" sequencer.
// ----
// ---- created: 01Sep2014
// ---- changed: 03Sep2014, 06Sep2014, 07Sep2014, 08Sep2014, 09Sep2014, 13Sep2014, 26Sep2014
// ----          11Jan2015, 14Feb2015, 16Feb2015, 21Mar2015, 26Mar2015, 10Apr2015, 12Apr2015
// ----          14Apr2015, 19Apr2015, 27Apr2015, 15May2015, 30Jun2015, 26Jul2015, 01Nov2015
// ----          02Nov2015, 04Nov2015, 27Feb2016, 29Feb2016, 08Jul2016, 20Jan2017, 21Jan2017
// ----          06Feb2017, 10Feb2017, 03Mar2017, 06Mar2017, 18Aug2017, 21Aug2017, 25Nov2017
// ----          22Dec2017, 28Apr2018, 02Jun2018, 17Jun2018, 29Dec2018, 01May2019, 12May2019
// ----          30May2019, 24Jun2019, 02Aug2019, 20Oct2019, 16Apr2020, 25Apr2020, 10Nov2020
// ----          15Nov2020, 15May2021, 30May2021, 07Jan2022, 02Nov2022, 14Jun2023, 03Dec2023
// ----          06Oct2024, 28Mar2025, 05Apr2025, 13Jun2025
// ----
// ----
// ----

#ifndef TK_MIDIPIPEFRAME_H__
#define TK_MIDIPIPEFRAME_H__

YG("midipipe")


class MIDIPipeEvent;


typedef union {
   midipipe_frame_base_t  base;
   midipipe_frame_val_t   prgchg;
   midipipe_frame_val_t   chpressure;
   midipipe_frame_val_t   pitchbend;
   midipipe_frame_ctl_t   cc;
   midipipe_frame_ctl_t   rpn;
   midipipe_frame_ctl_t   nrpn;
   midipipe_frame_base_t  all_notes_off;
   midipipe_frame_note_t  note;
   midipipe_frame_pat_t   polypressure;
   midipipe_frame_sysex_t sysex;
   midipipe_frame_sysrt_t sysrt;
} midipipe_frame_u;



/* @class MIDIPipeFrame,Object
 */
YC class MIDIPipeFrame : public YAC_Object {
  public:
   MIDIPipe *parent_pipe;
   MIDIPipeRoot *root;

   MIDIPipeFrameValueArray<midipipe_frame_val_t>   prgchg_array;
   MIDIPipeFrameValueArray<midipipe_frame_val_t>   chpressure_array;
   MIDIPipeFrameValueArray<midipipe_frame_val_t>   pitchbend_array;
   MIDIPipeFrameValueArray<midipipe_frame_ctl_t>   cc_array;
   MIDIPipeFrameValueArray<midipipe_frame_ctl_t>   rpn_array;
   MIDIPipeFrameValueArray<midipipe_frame_ctl_t>   nrpn_array;
   MIDIPipeFrameValueArray<midipipe_frame_base_t>  all_notes_off_array;
   MIDIPipeFrameValueArray<midipipe_frame_note_t>  note_off_array;
   MIDIPipeFrameValueArray<midipipe_frame_note_t>  note_on_array;
   MIDIPipeFrameValueArray<midipipe_frame_pat_t>   polypressure_array; // aka polyphonic aftertouch
   MIDIPipeFrameValueArray<midipipe_frame_sysex_t> sysex_array;
   MIDIPipeFrameValueArray<midipipe_frame_sysrt_t> sysrt_array;

   sSI io_filter; // MIDIPIPE_EVENT_TYPE_xxx

   sSI usr_frame_idx; // not loaded/saved. for application use.

   sUI time_stamp;  // millisec
   sSI time_stamp_tmp;  // ticks (used by Monitor node). not saved/restored.

   sUI marker;  // arbitrary user defined value. Used by e.g. NodeMonitor

   sBool b_dont_send;  // default for prgChg(), chPressure(), .. calls

   static sUI minimum_note_duration;


  public:
   MIDIPipeFrame(void);
   ~MIDIPipeFrame();

   YAC(MIDIPipeFrame);

   void YAC_VCALL yacOperatorAssign (YAC_Object *_robj);

  public:
   midipipe_frame_u *getEventLocByIdxAndFlt (sUI _idx, sUI *_retRelIdx, sUI *_retAbsIdx, sUI *_retEventType, sSI _fltType, sSI _fltDev, sSI _fltCh, sSI _globalCh) const;
   midipipe_frame_u *getEventLocByIdx (sUI _idx, sUI *_retRelIdx, sUI *_retAbsIdx, sUI *_retEventType, sSI _fltType) const;

  public:
   YM void empty (void);
   YM void free (void);
   YM void optimize (void); // remove gaps

   YM void setPipe (MIDIPipe *_parentPipe);
   YM MIDIPipe *getPipe (void) const;

   YM void setRoot (MIDIPipeRoot *_root);
   YM MIDIPipeRoot *getRoot (void) const;

   YM void setTimeStamp (sUI _millisec);
   YM sUI getTimeStamp (void);

   YM void setTimeStampTmp (sSI _ticks);
   YM sSI getTimeStampTmp (void);

   YM void setMarker (sSI _marker);
   YM sSI getMarker (void);

   // workaround for "disappearing VST notes" issue (clip duration)
   YM void SetMinimumNoteDuration (sUI _numTicks);
   YM sUI GetMinimumNoteDuration (void);

   // Sets default for next prgChg(), chPressure(), pitchbend(), .. calls
   YM void setEnableDontSend (sBool _bEnable);
   YM sBool getEnableDontSend (void);

   // Update all (used) events
   YM void updateDontSendFlags (sBool _bDontSend);

   YM sBool prgChg       (sBool _bSet, sSI _devIdx, sUI _midiCh, sSI _prog);
   YM sBool chPressure   (sBool _bSet, sSI _devIdx, sUI _midiCh, sF32 _pressure);
   YM sBool pitchbend    (sBool _bSet, sSI _devIdx, sUI _midiCh, sF32 _pitchbend);
   YM sBool cc           (sBool _bSet, sSI _devIdx, sUI _midiCh, sUI _ccId, sF32 _value);
   YM sBool rpn          (sBool _bSet, sSI _devIdx, sUI _midiCh, sUI _rpn,  sF32 _value);
   YM sBool nrpn         (sBool _bSet, sSI _devIdx, sUI _midiCh, sUI _nrpn, sF32 _value);
   YM sBool allNotesOff  (sBool _bSet, sSI _devIdx, sUI _midiCh);
   YM sBool noteOff      (sBool _bSet, sSI _devIdx, sUI _midiCh, sSI _note, sF32 _vel);
   YM sBool noteOffPre   (sBool _bSet, sSI _devIdx, sUI _midiCh, sSI _note, sF32 _vel);
   YM sBool noteOn       (sBool _bSet, sSI _devIdx, sUI _midiCh, sSI _note, sF32 _vel, sSI _dur);
   YM sBool polyPressure (sBool _bSet, sSI _devIdx, sUI _midiCh, sSI _noteNr, sF32 _pressure);
   YM sBool sysex        (sBool _bSet, sSI _devIdx, YAC_Buffer *_buffer); // (note) including 0xF0/0xF7 bytes (or not)
   YM sBool sysexAddF0F7 (sBool _bSet, sSI _devIdx, YAC_Buffer *_buffer, sBool _bDontSend); // (note) add 0xF0/0xF7 bytes
   YM sBool sysrt        (sBool _bSet, sSI _devIdx, sUI _byte);

   // keep evIdx:
   YM sBool noteOffReplace (sSI _devIdx, sUI _midiCh, sSI _oldNoteNr, sSI _newNoteNr, sF32 _vel);
   YM sBool noteOnReplace (sSI _devIdx, sUI _midiCh, sSI _oldNoteNr, sSI _newNoteNr, sF32 _vel, sSI _dur);
   YM sBool polyPressureReplace (sSI _devIdx, sUI _midiCh, sSI _oldNoteNr, sSI _newNoteNr, sF32 _pressure);  // pressure=-1: keep

   YM sUI noteOnVelReplace (sSI _fltDev, sSI _fltCh, sF32 _vel);
   YM sUI noteOnVelMul (sSI _fltDev, sSI _fltCh, sF32 _s);
   YM sUI noteOnVelAdd (sSI _fltDev, sSI _fltCh, sF32 _vel);
   YM sBool noteOnVelReplaceByIdxAndFlt (sUI _idx, sSI _fltDev, sSI _fltCh, sF32 _vel);
   YM sUI noteOnDeleteZeroVel (void);
   YM sBool noteOffVelReplaceByIdxAndFlt (sUI _idx, sSI _fltDev, sSI _fltCh, sF32 _vel);
   YM void noteOnUpdateByIdx (sSI _fltDev, sSI _fltCh, sUI _evIdx, sSI _newNote);

   YM sBool hasEvents (void) const;

   YM sUI getNumEvents             (void) const;
   YM sUI getNumEventsPrgChg       (void) const;
   YM sUI getNumEventsChPressure   (void) const;
   YM sUI getNumEventsPitchbend    (void) const;
   YM sUI getNumEventsCC           (void) const;
   YM sUI getNumEventsRPN          (void) const;
   YM sUI getNumEventsNRPN         (void) const;
   YM sUI getNumEventsAllNotesOff  (void) const;
   YM sUI getNumEventsNoteOff      (void) const;
   YM sUI getNumEventsNoteOffPre   (void) const;
   YM sUI getNumEventsNoteOffPost  (void) const;
   YM sUI getNumEventsNoteOn       (void) const;
   YM sUI getNumEventsPolyPressure (void) const;
   YM sUI getNumEventsSysEx        (void) const;
   YM sUI getNumEventsSysRT        (void) const;

   // fltType= MIDIPIPE_EVENT_TYPE_xxx
   YM sBool getEventByIdx (sUI _idx, MIDIPipeEvent *_ret, sSI _fltType) const;

   // Filter by dev+ch
   YM sBool getEventByIdxAndFlt (sUI _idx, MIDIPipeEvent *_ret, sSI _fltType, sSI _devIdx, sSI _devCh) const;

   YM sBool getCCEventByIdxAndFlt   (sUI _idx, MIDIPipeEvent *_ret, sUI _cc, sSI _fltDev, sSI _fltCh);
   YM sBool getRPNEventByIdxAndFlt  (sUI _idx, MIDIPipeEvent *_ret, sUI _nr, sSI _fltDev, sSI _fltCh);
   YM sBool getNRPNEventByIdxAndFlt (sUI _idx, MIDIPipeEvent *_ret, sUI _nr, sSI _fltDev, sSI _fltCh);
   YM sBool getNoteOnEventByFlt (MIDIPipeEvent *_ret, sSI _noteNr, sSI _fltDev, sSI _fltCh);
   YM sBool getNoteOffEventByFlt (MIDIPipeEvent *_ret, sSI _noteNr, sSI _fltDev, sSI _fltCh);

   YM sUI getNumEventsByFlt             (sSI _fltDev, sSI _fltCh) const;
   YM sUI getNumEventsPrgChgByFlt       (sSI _fltDev, sSI _fltCh) const;
   YM sUI getNumEventsChPressureByFlt   (sSI _fltDev, sSI _fltCh) const;
   YM sUI getNumEventsPitchbendByFlt    (sSI _fltDev, sSI _fltCh) const;
   YM sUI getNumEventsCCByFlt           (sSI _fltDev, sSI _fltCh) const;
   YM sUI getNumEventsRPNByFlt          (sSI _fltDev, sSI _fltCh) const;
   YM sUI getNumEventsNRPNByFlt         (sSI _fltDev, sSI _fltCh) const;
   YM sUI getNumEventsAllNotesOffByFlt  (sSI _fltDev, sSI _fltCh) const;
   YM sUI getNumEventsNoteOffByFlt      (sSI _fltDev, sSI _fltCh) const;
   YM sUI getNumEventsNoteOnByFlt       (sSI _fltDev, sSI _fltCh) const;
   YM sUI getNumEventsPolyPressureByFlt (sSI _fltDev, sSI _fltCh) const;
   YM sUI getNumEventsSysExByFlt        (sSI _fltDev) const;
   YM sUI getNumEventsSysRTByFlt        (sSI _fltDev) const;

   YM sBool getNumEventsNoteOnInRangeByFlt (sSI _fltDev, sSI _fltCh, sSI _minNote, sSI _maxNote) const;

   // Return value or -1 if frame does not contain a matching event
   //  the filter -1 (dev and/or ch) matches all devices resp. channels
   YM sSI  getFilteredPrgChg          (sSI _fltDev, sSI _fltCh);
   YM sF32 getFilteredCC              (sSI _fltDev, sSI _fltCh, sUI _cc);
   YM sF32 getFilteredRPN             (sSI _fltDev, sSI _fltCh, sUI _rpn);
   YM sF32 getFilteredNRPN            (sSI _fltDev, sSI _fltCh, sUI _nrpn);
   YM sF32 getFilteredPitchbend       (sSI _fltDev, sSI _fltCh);
   YM sF32 getFilteredChPressure      (sSI _fltDev, sSI _fltCh);
   YM sF32 getFilteredNoteOnVelocity  (sSI _fltDev, sSI _fltCh, sSI _noteNr);
   YM sF32 getFilteredNoteOffVelocity (sSI _fltDev, sSI _fltCh, sSI _noteNr);

   YM sF32 getNormalizedNoteOnVelocityByIdxAndFlt  (sUI _idx, sSI _fltDev, sSI _fltCh);
   YM sF32 getNormalizedNoteOffVelocityByIdxAndFlt (sUI _idx, sSI _fltDev, sSI _fltCh);

   YM sBool setEventDevByIdx    (sUI _idx, sUI _devIdx);
   YM sBool setEventMidiChByIdx (sUI _idx, sUI _midiCh);

   YM sBool setEventArg1ByIdx (sUI _idx, sF32 _val);  // prgchg / chpressure / pitchbend / ccnr / rpn / nrpn / note
   YM sBool setEventArg2ByIdx (sUI _idx, sF32 _val);  // ccval / rpnval / nrpnval / velocity / polypressure
   YM sBool setEventArg3ByIdx (sUI _idx, sSI  _val);  // duration

   YM sF32 getEventArg1ByIdx (sUI _idx);  // prgchg / chpressure / pitchbend / ccnr / rpn / nrpn / note
   YM sF32 getEventArg2ByIdx (sUI _idx);  // ccval / rpnval / nrpnval / velocity / polypressure
   YM sSI  getEventArg3ByIdx (sUI _idx);  // duration

   YM sBool deleteEventByIdx (sUI _idx, sSI _fltType);

   YM sUI deletePrgChgByFlt       (sSI _fltDev, sSI _fltCh);
   YM sUI deleteChPressureByFlt   (sSI _fltDev, sSI _fltCh);
   YM sUI deletePitchbendByFlt    (sSI _fltDev, sSI _fltCh);
   YM sUI deleteCCByFlt           (sSI _fltDev, sSI _fltCh, sSI _cc);
   YM sUI deleteRPNByFlt          (sSI _fltDev, sSI _fltCh, sSI _rpn);
   YM sUI deleteNRPNByFlt         (sSI _fltDev, sSI _fltCh, sSI _rpn);
   YM sUI deletePolyPressureByFlt (sSI _fltDev, sSI _fltCh);
   YM sUI deleteSysExByFlt        (sSI _fltDev);
   YM sUI deleteSysRTByFlt        (sSI _fltDev);

   YM void deleteNoteOffsByFlt (sSI _fltDevIdx, sSI _fltMidiCh);
   YM void deleteNoteOnsByFlt  (sSI _fltDevIdx, sSI _fltMidiCh);

   YM void keepEventsByFlt   (sSI _fltDev, sSI _fltCh); // Delete all events not matching the filter
   YM void deleteEventsByFlt (sSI _fltDev, sSI _fltCh); // Delete all events matching the filter

   // velMode: 0=normal, 1=aftertouch, 2=modwheel
   YM void mirrorNoteOnOffs (MIDIPipeFrame *_dst, sSI _srcDev, sSI _srcCh, sSI _dstDev, sSI _dstCh, sSI _transp, sUI _velMode, sBool _bFixedVel);

   YM sUI convertRPNUIPrgChgToPrgChg (sSI _fltDev, sSI _fltCh);

   YM void setNoteDurations (sUI _numTicks);
   YM void setMinNoteDurations (sUI _numTicks);
   YM void updateNoteDuration (sSI _fltDevIdx, sSI _fltMidiCh, sSI _fltNote, sUI _numTicks);
   YM void scaleNoteDurations (sSI _fltDevIdx, sSI _fltMidiCh, sF32 _scl, sF32 _off);

   YM sF32 getFirstNoteOnVelocity (sSI _fltDevIdx, sSI _fltMidiCh);
   YM void updateNoteOnVelocities (sSI _fltDevIdx, sSI _fltMidiCh, sF32 _vel);
   YM void scaleNoteVelocities (sSI _fltDevIdx, sSI _fltMidiCh, sF32 _scl, sF32 _off);

   YM void setDevIdxAndMidiCh (sSI _devIdx, sSI _midiCh);

   YM void mergeFrame (MIDIPipeFrame *_o);
   YM void mergeFrameTranspose (MIDIPipeFrame *_o, sSI _transpose, sF32 _durationScale);
   YM void mergeFrameTransposePrgChg (MIDIPipeFrame *_o, sSI _transpose, sSI _prgChgBaseNote, sSI _noteOffPrgOff);
   // (note) dstDev/dstCh must be != -1 or nothing will be merged
   YM void mergeFrameFltDst (MIDIPipeFrame *_o, sSI _fltDev, sSI _fltCh, sSI _dstDev, sSI _dstCh, sBool _bMergeNotes, sBool _bMergeRPN);
   // Merge all events matching fltDev/Ch (-1 == match all)
   YM void mergeFrameFlt (MIDIPipeFrame *_o, sSI _fltDev, sSI _fltCh);

   YM void emit (void);

   // (note) the IO filter is currently used when loading/saving states
   YM void setIoFilter (sUI _evType);
   YM void saveState (YAC_Object *_ofs);
   YM sBool loadState (YAC_Object *_ifs);

   YM void remapDeviceIndices (YAC_Object *_outDevIdxMap, YAC_Object *_outDevUseCount);

   YM void setUsrFrameIdx (sSI _idx); // NodeAuto
   YM sSI  getUsrFrameIdx (void);

   YM sUI decodePolystepRPN (sSI _fltDevIdx, sSI _fltCh, YAC_Object *_stepnr, YAC_Object *_patnr, YAC_Object *_tracknr);

   // (note) ch=-1 => keep
   YM sUI replaceDevIdxAndMidiCh (sSI _fltDevIdx, sSI _fltCh, sSI _devIdx, sSI _ch);

   YM sBool isControllerStateDifferentFrom (MIDIPipeFrame *_o);

   YM sUI getPrgChgShortMsgByIdx       (sUI _idx, YAC_Object *_retDevIdx);
   YM sUI getChPressureShortMsgByIdx   (sUI _idx, YAC_Object *_retDevIdx);
   YM sUI getPitchbendShortMsgByIdx    (sUI _idx, YAC_Object *_retDevIdx);
   YM sUI getCCShortMsgByIdx           (sUI _idx, YAC_Object *_retDevIdx);
   YM sUI getAllNotesOffShortMsgByIdx  (sUI _idx, YAC_Object *_retDevIdx);
   YM sUI getNoteOffPreShortMsgByIdx   (sUI _idx, YAC_Object *_retDevIdx);
   YM sUI getNoteOffPostShortMsgByIdx  (sUI _idx, YAC_Object *_retDevIdx);
   YM sUI getNoteOffShortMsgByIdx      (sUI _idx, YAC_Object *_retDevIdx);
   YM sUI getPolyPressureShortMsgByIdx (sUI _idx, YAC_Object *_retDevIdx);
   YM sUI getNoteOnShortMsgByIdx       (sUI _idx, YAC_Object *_retDevIdx);

   // see (Eureka) ModMatrixEntry, Mod, and ModVST script classes. returns srcIdx or -1.
   YM sSI modMatrixCheckEventByFlt (sUI _evIdx, sSI _fltDev, sSI _fltCh, sSI _globalCh, YAC_Object *_retVal, YAC_Object *_retVel);

};


#endif // TK_MIDIPIPEFRAME_H__
