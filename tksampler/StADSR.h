// ----
// ---- file   : StADSR.h
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2009-2024 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : This is part of the "syntracker" midi sequencer.
// ----
// ---- created: 23Sep2009
// ----
// ---- changed: 21Jan2010, 08Feb2010, 09Feb2010, 14Feb2010, 15Feb2010, 29Jun2010, 28Dec2018
// ----          29Dec2018, 11May2019, 31May2020, 03Oct2024
// ----
// ----
// ----

#ifndef ST_ADSR_H__
#define ST_ADSR_H__

YG("tksampler")

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
};


#endif // ST_ADSR_H__
