// ----
// ---- file   : AR_Pattern.h
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2016-2023 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : This is part of the "tkanalogrytm" plugin.
// ----
// ---- created: 27Feb2016
// ---- changed: 29Feb2016, 01Mar2016, 03Mar2016, 29Mar2016, 21Oct2019, 27Nov2023, 28Nov2023
// ----          07Dec2023
// ----
// ----
// ----

#ifndef __TK_AR_PATTERN_H__
#define __TK_AR_PATTERN_H__

YG("analogrytm")


/* @class AR_Pattern,Object
 */
YC class AR_Pattern : public YAC_Object {
  public:
   sU8 syx[AR_PATTERN_SYX_MAX_SZ];
   sU8 raw[AR_PATTERN_MAX_SZ];

   sU8 *current_buf; // points to either syx or raw buffer, whichever was last written
   sU8 ar_pattern_version;  // v1 or v4 (v1.50)

  public:
   AR_Pattern(void);
   ~AR_Pattern();

   YAC(AR_Pattern);

  public:
   void YAC_VCALL yacOperator (sSI _cmd, YAC_Object *_robj, YAC_Value *_r);


   YM sUI getRequiredSizeSyx (void);
   YM sUI getRequiredSizeRaw (void);

   // 0=none, 1=syx, 2=raw
   YM sSI getCurrentDataIdx (void);

   YM void saveSyxToStream (YAC_Object *_ofs);
   YM void saveRawToStream (YAC_Object *_ofs);

   YM sBool loadSyxFromStream (YAC_Object *_ifs);
   YM sBool loadRawFromStream (YAC_Object *_ifs);

   YM sBool addPatternRequestX (YAC_Buffer *_d, sUI _devId, sUI _patternNr);

   YM sBool syxToRaw (YAC_Buffer *_s);

   YM sBool sendPattern (YAC_Buffer *_d, sUI _devId, sUI _patNr, sBool _bEditBuf);

   // Return reference to 'raw' array in 'ret'
   YM sBool getRawBuffer (YAC_Buffer *_ret);

   YM sUI getVersion (void);

   YM sUI  getMasterLength (void);
   YM void setMasterLength (sUI _masterLen);

   YM sUI  getMasterSpeed (void);
   YM void setMasterSpeed (sUI _speed);

   // bit7=send MIDI.
   // bit2..0: speed, 0=2x, 1=3/2x, 2=1x, 3=3/4x, 4=1/2x, 5=1/4x, 6=1/8x
   YM sUI  getFlagsAndSpeed (sUI _trackIdx);
   YM void setFlagsAndSpeed (sUI _trackIdx, sUI _v);

   YM sUI  getDefaultNote (sUI _trackIdx);
   YM void setDefaultNote (sUI _trackIdx, sUI _v);

   YM sUI  getDefaultVelocity (sUI _trackIdx);
   YM void setDefaultVelocity (sUI _trackIdx, sUI _v);

   YM sUI  getDefaultNoteLength (sUI _trackIdx);
   YM void setDefaultNoteLength (sUI _trackIdx, sUI _v);

   YM sUI  getDefaultTrigFlags (sUI _trackIdx);
   YM void setDefaultTrigFlags (sUI _trackIdx, sUI _v);

   YM sUI  getNumSteps (sUI _trackIdx);
   YM void setNumSteps (sUI _trackIdx, sUI _numSteps);

   // FW1.70 trig_bits 14bit access helpers
   YM sUI  getTrig (sUI _trackIdx, sUI _stepIdx);
   YM void setTrig (sUI _trackIdx, sUI _stepIdx, sUI _val);

   // 0xFF=unset, 0..127. bit7:0=have trig condition,1=no trig cond
   YM sUI  getStepNote (sUI _trackIdx, sUI _stepIdx);
   YM void setStepNote (sUI _trackIdx, sUI _stepIdx, sUI _val);

   // 0xFF=unset, 0..127. bit7:0=have trig condition,1=no trig cond
   YM sUI  getStepVelocity (sUI _trackIdx, sUI _stepIdx);
   YM void setStepVelocity (sUI _trackIdx, sUI _stepIdx, sUI _val);

   // 0xFF=unset 0..127(inf)
   YM sUI  getStepNoteLength (sUI _trackIdx, sUI _stepIdx);
   YM void setStepNoteLength (sUI _trackIdx, sUI _stepIdx, sUI _val);

   // -23..23
   YM sSI  getStepMicroTiming (sUI _trackIdx, sUI _stepIdx);
   YM void setStepMicroTiming (sUI _trackIdx, sUI _stepIdx, sSI _val);

   // 0..127
   YM sUI  getStepRetrigLength (sUI _trackIdx, sUI _stepIdx);
   YM void setStepRetrigLength (sUI _trackIdx, sUI _stepIdx, sUI _retrigLength);

   // 0..16
   YM sUI  getStepRetrigRate (sUI _trackIdx, sUI _stepIdx);
   YM void setStepRetrigRate (sUI _trackIdx, sUI _stepIdx, sUI _retrigRate);

   // -128..127
   YM sSI  getStepRetrigVelocityOffset (sUI _trackIdx, sUI _stepIdx);
   YM void setStepRetrigVelocityOffset (sUI _trackIdx, sUI _stepIdx, sSI _retrigVelocityOffset);

   // 0..64
   YM sUI  getStepTrigCondition (sUI _trackIdx, sUI _stepIdx);
   YM void setStepTrigCondition (sUI _trackIdx, sUI _stepIdx, sSI _condition);

   YM sBool getTrigEnable (sUI _trackIdx, sUI _stepIdx);
   YM void  setTrigEnable (sUI _trackIdx, sUI _stepIdx, sBool _bEnable);

   YM sBool getTrigRetrig (sUI _trackIdx, sUI _stepIdx);
   YM void  setTrigRetrig (sUI _trackIdx, sUI _stepIdx, sBool _bEnable);

   YM sBool getTrigMute (sUI _trackIdx, sUI _stepIdx);
   YM void  setTrigMute (sUI _trackIdx, sUI _stepIdx, sBool _bEnable);

   YM sBool getTrigAccent (sUI _trackIdx, sUI _stepIdx);
   YM void  setTrigAccent (sUI _trackIdx, sUI _stepIdx, sBool _bEnable);

   YM sBool getTrigSwing (sUI _trackIdx, sUI _stepIdx);
   YM void  setTrigSwing (sUI _trackIdx, sUI _stepIdx, sBool _bEnable);

   YM sBool getTrigSlide (sUI _trackIdx, sUI _stepIdx);
   YM void  setTrigSlide (sUI _trackIdx, sUI _stepIdx, sBool _bEnable);

   YM sBool getTrigLFOPLockSW (sUI _trackIdx, sUI _stepIdx);
   YM void  setTrigLFOPlockSW (sUI _trackIdx, sUI _stepIdx, sBool _bEnable);

   YM sBool getTrigLFOPLockEN (sUI _trackIdx, sUI _stepIdx);
   YM void  setTrigLFOPlockEN (sUI _trackIdx, sUI _stepIdx, sBool _bEnable);

   YM sBool getTrigSYNPLockSW (sUI _trackIdx, sUI _stepIdx);
   YM void  setTrigSYNPlockSW (sUI _trackIdx, sUI _stepIdx, sBool _bEnable);

   YM sBool getTrigSYNPLockEN (sUI _trackIdx, sUI _stepIdx);
   YM void  setTrigSYNPlockEN (sUI _trackIdx, sUI _stepIdx, sBool _bEnable);

   YM sBool getTrigSMPPLockSW (sUI _trackIdx, sUI _stepIdx);
   YM void  setTrigSMPPlockSW (sUI _trackIdx, sUI _stepIdx, sBool _bEnable);

   YM sBool getTrigSMPPLockEN (sUI _trackIdx, sUI _stepIdx);
   YM void  setTrigSMPPlockEN (sUI _trackIdx, sUI _stepIdx, sBool _bEnable);

   YM sBool getTrigENVPLockSW (sUI _trackIdx, sUI _stepIdx);
   YM void  setTrigENVPlockSW (sUI _trackIdx, sUI _stepIdx, sBool _bEnable);

   YM sBool getTrigENVPLockEN (sUI _trackIdx, sUI _stepIdx);
   YM void  setTrigENVPlockEN (sUI _trackIdx, sUI _stepIdx, sBool _bEnable);

   YM sBool getTrigless (sUI _trackIdx, sUI _stepIdx);
   YM void  setTrigless (sUI _trackIdx, sUI _stepIdx, sBool _bEnable);
};


#endif // __TK_AR_PATTERN_H__
