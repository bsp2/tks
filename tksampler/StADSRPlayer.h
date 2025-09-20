// ----
// ---- file   : StADSRPlayer.h
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2009-2024 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : This is part of the "syntracker" midi sequencer.
// ----
// ---- created: 23Sep2009
// ----
// ---- changed: 21Jan2010, 14Feb2010, 16Feb2010, 20Feb2010, 13May2010, 29Jun2010, 01Aug2010
// ----          09Sep2010, 28Dec2018, 17Jan2019, 11May2019, 12May2019, 13May2019, 13Jul2019
// ----          25Jul2019, 19Feb2020, 10Dec2022, 03Oct2024
// ----
// ----
// ----

#ifndef ST_ADSRPLAYER_H__
#define ST_ADSRPLAYER_H__

YG("tksampler")


// internal helper class, not exposed to scripts
class StADSRPlayer {
  public:

   enum {
      ENV_ATTACK  = 0,
      ENV_SUSTAIN = 1,
      ENV_RELEASE = 2
   };

  public:
   StADSR       *adsr;           // reference to currently playing ADSR object
   sBool         b_finished;
   sBool         b_sustain_finished;
   sBool         b_skip_sustain;  // when retriggering envelope during after note-off

   sF32 min_level;  // unscaled env min level. def=-2 but volume ADSRs are clamped to 0.

   sF32 mod_speed;
   sF32 mod_level;
   sF32 mod_intensity;
   sF32 mod_aspeed; // attack speed
   sF32 mod_sspeed; // sustain speed
   sF32 mod_rspeed; // release speed

   sF32 *sp_mod_speed;      // *  (0..n, 1.0 = no mod)
   sF32 *sp_mod_level;      // +  (-1..1)
   sF32 *sp_mod_intensity;  // +
   sF32 *sp_mod_aspeed;     // *
   sF32 *sp_mod_rspeed;     // *
   sF32 *sp_mod_sspeed;     // *

   sF32 mmdst_speed;        // *  1.0 = no mod
   /* /\* sF32 mmdst_level;        // *  1.0 = no mod  ([19Feb2020]: was additive) *\/ */
   sF32 mmdst_intensity;    // +  relative (-1..1)
   sF32 mmdst_aspeed;       // *  attack speed
   sF32 mmdst_sspeed;       // *  sustain speed
   sF32 mmdst_rspeed;       // *  release speed

  protected:
   sF64          time;           // 0..current_env_totaltime, = current_env.time
   sUI           env_index;      // current envelope index (attack, sustain, release)
   sF32          attack_level;   // last seen attack level
   sF32          sustain_level;  // last seen sustain level
   sF32          default_level;  // volume:1.0 , freq:0.0, pan:0.0
   sF32          final_level;    // valid when all stages finished

  public:// xxxxxxxxxxxxxxx should be protected
   TKS_Envelope *current_env;           // temporary Envelope
   StEnvelope   *orig_st_env;
   TKS_Envelope *orig_env;
  protected:
   sF32          current_env_totaltime; // unscaled total time
   sF64          current_env_timescale; // = current_env.time_advance
   sF32          current_env_intensity;

  protected:
   void            visitEnv (StEnvelope *_env);
   /* /\* YAC_FloatArray *getEnv   (sUI _envIdx); *\/ */
   /* /\* StEnvelope *    getStEnv (sUI _envIdx); *\/ */

  public:
   StADSRPlayer(void);
   ~StADSRPlayer();

   void startADSR (StADSR *_adsr, sF32 _defaultLevel, sBool _bResetMod, sBool _bRelease);
   void stopADSR  (void);
   void noteOff   (void);
   sF32 tick      (void); // called when a new block is started, returns current value
   sBool isFinished (void);
   sBool isInRelease (void) const;
   void noteOffRestartRelease (void);
};


#endif // ST_ADSRPLAYER_H__
