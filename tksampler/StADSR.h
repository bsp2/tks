// ----
// ---- file   : StADSR.h
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : Distributed under terms of the MIT license (https://opensource.org/licenses/MIT)
// ----          Copyright 2009-2026 by bsp
// ----
// ----          Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
// ----          associated documentation files (the "Software"), to deal in the Software without restriction, including
// ----          without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// ----          copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to
// ----          the following conditions:
// ----
// ----          The above copyright notice and this permission notice shall be included in all copies or substantial
// ----          portions of the Software.
// ----
// ----          THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
// ----          NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// ----          IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// ----          WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// ----          SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// ----
// ---- info   : This is part of the "syntracker" midi sequencer.
// ----
// ---- created: 23Sep2009
// ---- changed: 21Jan2010, 08Feb2010, 09Feb2010, 14Feb2010, 15Feb2010, 29Jun2010, 28Dec2018
// ----          29Dec2018, 11May2019, 31May2020, 03Oct2024, 09May2026
// ----
// ----
// ----

#ifndef ST_ADSR_H__
#define ST_ADSR_H__

YG("tksampler")

/* @class StADSR,Object
*/
YC class StADSR : public YAC_Object {

#define STADSR_STAGEOP_MUL           YCI 0
#define STADSR_STAGEOP_MUL_INTENSITY YCI 1   // abs when intensity=1
#define STADSR_STAGEOP_ADD           YCI 2

/* #define STADSR_SUSTAIN_HOLD          YCI 0 */
/* #define STADSR_SUSTAIN_LOOP_NOTEOFF  YCI 0   // loop sustain envelope until note off */
/* #define STADSR_SUSTAIN_LOOP_INF      YCI 1   // loop sustain envelope indefinitely (not valid for VOL env) */
/* #define STADSR_SUSTAIN_HOLD_RELEASE  YCI 2   // play sustain envelope once + hold last value. jump to sustain loop at note off. */
/* #define STADSR_SUSTAIN_RELEASE       YCI 3   // play sustain envelope once. jump to sustain loop at note off. */

   // Note:
   //   - env_attack is actually attack+decay
   //

  public:
   StEnvelope *env_attack;  // if null, attack_level is 1.0f
   StEnvelope *env_sustain; // if null, sustain_level is last value of env_attack
   StEnvelope *env_release; // if null, release level is 0.0f

   sF32 intensity; // 0..1, overall influence of ADSR
   sF32 level;     // scaling of output ADSR output value
   sF64 timescale;
   /* /\* sBool b_abs_levels;   // 0=scale sustain by last attack lvl and release by last sustain*lastattack (default), 1=use absolute env levels (intensity blends between prev stage scaling (0) and current stage (1)) *\/ */
   sSI stage_op; // STADSR_STAGEOP_xxx
   sSI b_sustain_loop;  // 1=loop sustain envelope (default), 0=oneshot sustain envelope, -1=loop sustain infinitely
   sSI b_attack_oneshot;  // 1=always finish attack before entering release phase (see StADSRPlayer::b_queued_release_during_attack)

  public:

   StADSR(void);
   ~StADSR();

   YAC(StADSR);

   YM YAC_Object *_getEnvAttack  (void);
   YM YAC_Object *_getEnvSustain (void);
   YM YAC_Object *_getEnvRelease (void);

   YM YAC_Object *_getOrCreateEnvAttack  (void);
   YM YAC_Object *_getOrCreateEnvSustain (void);
   YM YAC_Object *_getOrCreateEnvRelease (void);

   YM YAC_Object *_getEnvAttackMetaData  (void); // shortcut to get current BezierEditState object
   YM YAC_Object *_getEnvSustainMetaData (void);
   YM YAC_Object *_getEnvReleaseMetaData (void);

   YM YAC_Object *_getOrCreateEnvAttackData  (void); // shortcut to get current/create a new StEnvelope->TKS_Envelope object
   YM YAC_Object *_getOrCreateEnvSustainData (void);
   YM YAC_Object *_getOrCreateEnvReleaseData (void);

   YM void _deleteEnvAttack  (void);
   YM void _deleteEnvSustain (void);
   YM void _deleteEnvRelease (void);

   YM sF32 _getIntensity (void);
   YM void _setIntensity (sF32 _v);

   YM sF32 _getLevel (void);
   YM void _setLevel (sF32 _v);

   YM sF32 _getTimescale (void);
   YM void _setTimescale (sF32 _v);

   YM sF32 _getTimescaleMillisec (void);
   YM void _setTimescaleMillisec (sF32 _v);

   YM sSI   _getStageOp (void);
   YM void  _setStageOp (sSI _op);

   YM sSI   _getEnableSustainLoop (void);
   YM void  _setEnableSustainLoop (sSI _bEnabled);

   YM sSI   _getEnableAttackOneshot (void);
   YM void  _setEnableAttackOneshot (sSI _bEnabled);
};


#endif // ST_ADSR_H__
