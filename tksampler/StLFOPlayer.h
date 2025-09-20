// ----
// ---- file   : StLFOPlayer.h
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2009-2024 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : This is part of the "syntracker" midi sequencer.
// ----
// ---- created: 23Sep2009
// ----
// ---- changed: 21Jan2010, 20Feb2010, 29Jun2010, 01Aug2010, 04Apr2011, 22Apr2011, 23Apr2011
// ----          17Jan2019, 30Mar2019, 12May2019, 17May2019, 13Jul2019, 25Jul2019, 15Feb2020
// ----          18Feb2020, 03Oct2024
// ----
// ----
// ----

#ifndef ST_LFOPLAYER_H__
#define ST_LFOPLAYER_H__

YG("tksampler")


// internal helper class, not exposed to scripts
class StLFOPlayer {
  public:

   sF32  phase; // 0..1
   sF32  level; // current level
   sF32  fade_time;  //  0..1
   sF32  delay_countdown;
   sF32  fadein_speed;
   sF32  faded_speed_amt; // current speed, 0..1 (lerp between regular speed and regular_speed*faded_speed_factor)

   sF32 mod_speed;
   sF32 mod_level;

   // sample_player modulation
   sF32 *sp_mod_speed;
   sF32 *sp_mod_level;

   // modmatrix modulation
   sF32 mmdst_speed;  // 1.0 = no mod

   sF32 noise_time; // 0..1
   sF32 noise_spd;  // per tick 'add', normalized to 0..1 range
   sF32 noise_cval;
   sF32 noise_nval;

   sF32 last_level;

  public:
   StLFO *lfo;

  protected:
   tksampler_lfsr_t lfsr;

  public:
   void calcNextNoise (void);
   void resetPhase (void);

  public:
   StLFOPlayer (void);
   ~StLFOPlayer ();

   void startLFO (StLFO *_lfo, sBool _bResetMod);
   void stopLFO (void);
   sF32 tick (void); // called when a new block is started, returns current value
};


// script wrapper for use with macroplayers (not used by sampler engine)
YC class _StLFOPlayer : public YAC_Object {

  protected:
   StLFOPlayer lfo_player;

  public:
   _StLFOPlayer(void);
   ~_StLFOPlayer();

   YAC(_StLFOPlayer);

  public:

   YM void _startLFO    (YAC_Object *_lfo, sBool _bNoteOn);
   YM void _stopLFO     (void);
   YM sF32 _calc        (void);
   YM void _tick        (void);
   YM void _setModSpeed (sF32 _modSpeed);
   YM void _setModLevel (sF32 _modLevel);
   YM sF32 _getLastLevel (void);
};


#endif // ST_LFOPLAYER_H__
