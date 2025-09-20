// ----
// ---- file   : StLFO.h
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2009-2024 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : This is part of the "syntracker" midi sequencer.
// ----
// ---- created: 23Sep2009
// ---- changed: 21Jan2010, 20Feb2010, 21Feb2010, 04Apr2011, 23Apr2011, 07Jan2012, 29Dec2018
// ----          30Mar2019, 03Oct2024
// ----
// ----
// ----

#ifndef ST_LFO_H__
#define ST_LFO_H__

YG("tksampler")


/* @class StLFO,Object
 */
YC class StLFO : public YAC_Object {

#define STLFO_TYPE_NONE  YCI 0
#define STLFO_TYPE_SIN   YCI 1
#define STLFO_TYPE_SAWUP YCI 2
#define STLFO_TYPE_SAWDN YCI 3
#define STLFO_TYPE_TRI   YCI 4
#define STLFO_TYPE_PULSE YCI 5
#define STLFO_TYPE_NOISE YCI 6
#define STLFO_NUM_TYPES  YCI 7

  public:
   sUI  type;
   sF32 phase; // 0..1
   sF32 speed; // 0..1. set/get freq methods convert to Hz
   sF32 pulse_width; // type == PULSE

   sUI  delay; // initial delay in "ticks", fading starts after delay! set/get methods convert to milliseconds
   sF32 fadein_time; // in milliseconds

   sF32 start_level; // Fade start
   sF32 faded_level; // Fade end

   sF32 faded_speed_factor;

   sF32 dc_offset;

   sF32 noise_minamp;
   sF32 noise_maxamp;
   sF32 noise_minspd;
   sF32 noise_maxspd;
   sBool b_noise_interpol;

   sF32 timebase; // ticks per second
   sBool b_mod_delay;  // 0=absolute delay/fadein speed, 1=delay/fadein modulated by StLFOPlayer::mod_speed

   sBool b_oneshot;
   sBool b_global;
   sBool b_reset;


  public:
   sF32 calcFadeInSpeed (void); // block size and start/fadedlevel dependent

  public:
   StLFO(void);
   ~StLFO();

   YAC(StLFO);

   YM void _setTimebase (sF32 _timebase);
   YM sF32 _getTimebase (void);

   YM void _setType (sUI _type);
   YM sUI  _getType (void);

   YM void _setPhase (sF32 _phase);
   YM sF32 _getPhase (void);

   YM void _setFreq (sF32 _hz);
   YM sF32 _getFreq (void);

   YM void _setPulseWidth (sF32 _pw);
   YM sF32 _getPulseWidth (void);

   YM void _setDelay (sF32 _millisec);
   YM sF32 _getDelay (void);

   YM void _setFadeInTime (sF32 _millisec);
   YM sF32 _getFadeInTime (void);

   YM void _setStartLevel (sF32 _start);
   YM sF32 _getStartLevel (void);

   YM void _setFadedLevel (sF32 _end);
   YM sF32 _getFadedLevel (void);

   YM void _setFadedSpeedFactor (sF32 _factor);
   YM sF32 _getFadedSpeedFactor (void);

   YM void _setDcOffset (sF32 _off);
   YM sF32 _getDcOffset (void);

   YM void _setNoiseMinAmp (sF32 _minAmp);
   YM sF32 _getNoiseMinAmp (void);

   YM void _setNoiseMaxAmp (sF32 _maxAmp);
   YM sF32 _getNoiseMaxAmp (void);

   YM void _setNoiseMinSpeed (sF32 _minAmp);
   YM sF32 _getNoiseMinSpeed (void);

   YM void _setNoiseMaxSpeed (sF32 _maxSpd);
   YM sF32 _getNoiseMaxSpeed (void);

   YM void  _setEnableNoiseInterpol (sBool _bEnabled);
   YM sBool _getEnableNoiseInterpol (void);

   YM void  _setEnableModDelay (sBool _bEnabled);
   YM sBool _getEnableModDelay (void);

   YM void  _setEnableOneShot (sBool _bEnabled);
   YM sBool _getEnableOneShot (void);

   YM void  _setEnableGlobal (sBool _bEnabled);
   YM sBool _getEnableGlobal (void);

   YM void  _setEnableReset (sBool _bEnabled);
   YM sBool _getEnableReset (void);
};


#endif // ST_LFO_H__
