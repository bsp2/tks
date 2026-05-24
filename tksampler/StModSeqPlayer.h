// ----
// ---- file   : StModSeqPlayer.h
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : Distributed under terms of the MIT license (https://opensource.org/licenses/MIT)
// ----          Copyright 2020-2026 by bsp
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
// ---- created: 15Feb2020
// ---- changed: 18Feb2020, 19Feb2020, 05Jun2020, 03Sep2020, 01Aug2021, 30Aug2021, 03Oct2024
// ----          24May2026
// ----
// ----
// ----

#ifndef ST_MODSEQPLAYER_H__
#define ST_MODSEQPLAYER_H__

YG("tksampler")


// internal class, not exposed to scripts
class StModSeqPlayer {
  public:
   StModSeq *modseq;

  public:
   sUI   step_idx;
   sF32  step_ticks;  // 1000Hz (i.e. 1=(1/1000 sec))
   sUI   num_steps_played;
   sBool b_pulse;     // true after playing new step
   sBool b_running;

   sF32 mod_speed;      // 1=no mod
   sF32 mod_level;      // 1=no mod
   sF32 mod_numsteps;   // 0..1
   sF32 mod_advance;    // any value (edge detect)

   // sample_player modulation
   sF32 *sp_mod_speed;
   sF32 *sp_mod_level;
   sF32 *sp_mod_numsteps;
   sF32 *sp_mod_advance;

   // modmatrix modulation
   sF32  mmdst_speed;      // 1.0 = no mod
   sF32  mmdst_numsteps;   // relative (-1..1)
   sF32  mmdst_advance;    // relative (-n..n) (edge detect)
   sF32  mmdst_step;       // -1=none, 0..1=select step 0..n or 0..(n-1) in oneshot mode
   sBool b_mmdst_step;

   sF32 last_level;  // for global ModSeq

   sF32 tempo_speed_scl;


  protected:
   tksampler_lfsr_t lfsr;
   tksampler_rand_nr_t rand_nr;
   sF32 last_advance;

   void *ui_sample_voice;

  protected:
   void resetMod (void);
   sUI calcNumSteps (void) const;
   sUI calcStepIdx (sUI _idx, sUI _numSteps) const;
   void nextStepIdx (sUI _numSteps);
   void handleModAdvance (sUI _numSteps);
   void handleRestart (void);

  public:
   StModSeqPlayer (void);
   ~StModSeqPlayer ();

   void startModSeq (StModSeq *_modseq, sBool _bResetMod, sF32 _tempoSpeedScl, void *_sampleVoice);
   void updateModSeq (StModSeq *_modseq);
   void stopModSeq (void);
   sF32 tick (sBool _bPulseGate); // called when a new block is started, returns current value
   void noteOn (void);
   void noteOff (void);
};


#endif // ST_MODSEQPLAYER_H__
