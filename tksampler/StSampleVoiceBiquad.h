// ----
// ---- file   : StSampleVoiceBiquad.h
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2009-2024 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : This is part of the "syntracker" midi sequencer.
// ----
// ---- created: 23Sep2009
// ----
// ---- changed: 26Dec2009, 27Dec2009, 29Dec2009, 02Jan2010, 05Jan2010, 07Jan2010, 15Jan2010
// ----          19Jan2010, 21Jan2010, 27Jan2010, 25Feb2010, 28Feb2010, 01Mar2010, 11Apr2010
// ----          29Apr2010, 04May2010, 28Jun2010, 29Jun2010, 19Jul2010, 04Sep2010, 05Sep2010
// ----          09Sep2010, 05Mar2011, 10Mar2011, 28Mar2011, 29Mar2011, 09May2013, 13Sep2015
// ----          19Dec2018, 29Dec2018, 04Jan2019, 07Jan2019, 08Jan2019, 17Jan2019, 24Mar2019
// ----          24Mar2019, 30Mar2019, 31Mar2019, 11May2019, 12May2019, 13May2019, 15May2019
// ----          16May2019, 18Jun2019, 12Jul2019, 13Jul2019, 01Aug2019, 29Nov2019, 15Feb2020
// ----          16Feb2020, 19Feb2020, 21Feb2020, 23Feb2020, 24Feb2020, 26Feb2020, 27Feb2020
// ----          17May2020, 18May2020, 20May2020, 22May2020, 23May2020, 24May2020, 25May2020
// ----          28May2020, 01Jun2020, 06Jun2020, 08Jun2020, 26Apr2021, 28Apr2021, 30Apr2021
// ----          03May2021, 20May2021, 26Jun2021, 01Aug2021, 30Aug2021, 04Oct2021, 20Oct2021
// ----          28Oct2021, 03Oct2024
// ----
// ----
// ----

#ifndef ST_SAMPLEVOICEBIQUAD_H__
#define ST_SAMPLEVOICEBIQUAD_H__

//
// Biquad filter based on Robert Bristow-Johnson's Cookbook formulae for audio EQ biquad filter coefficients
//  <http://musicdsp.org/files/Audio-EQ-Cookbook.txt>
//
// and Tom St Denis' "biquad.c" public domain implemention
//  http://musicdsp.org/files/biquad.c, <http://tomstdenis.home.dhs.org>
//
struct StSampleVoiceBiquad {
   // per channel
   tksampler_lfsr_t lfsr;

   // Filter type
   enum {
      LPF  = 1,  // Low Pass
      HPF  = 2,  // High Pass
      BPF  = 3,  // Band Pass
      BRF  = 4,  // Notch */
   };

   struct {
      sF32 x1, x2;  // input delayed by 1/2 samples
      sF32 y1, y2;  // output delayed by 1/2 samples
   } state;

   inline void reset(void) {
      state.x1 = state.x2 = 0.0f;
      state.y1 = state.y2 = 0.0f;
   }

   struct {
      sF32 a0, a1, a2, a3, a4;
   } biquad;

   sF32 bandwidth, qScale;  // calc'd in calcQ()

   StSampleVoiceBiquad(void);
   ~StSampleVoiceBiquad();

   void calcQ(sF32 &freq, sF32 &bandwidth, sF32 &qScale, sF32 q);

   void calcCoeff(sSI type,
                  sF32 dbGain,   // gain of filter
                  sF32 freq,     // center frequency
                  sF32 srate,    // sampling rate
                  sF32 bandwidth // bandwidth in octaves
                  );

   // calls calcQ() and calcCoeff()
   void calcParams (sSI _type,
                    sF32 _dbGain,
                    sF32 _freq,   // 0..1
                    sF32 _res,    // 0..1
                    sF32 _srate
                    );

   sF32 filter (const sF32 _inSmp);
};


// based on http://www.earlevel.com/main/2012/11/26/biquad-c-source-code/
//
//  Created by Nigel Redmon on 11/24/12
//  EarLevel Engineering: earlevel.com
//  Copyright 2012 Nigel Redmon
//
//  For a complete explanation of the Biquad code:
//  http://www.earlevel.com/main/2012/11/26/biquad-c-source-code/
//
//  License:
//
//  This source code is provided as is, without warranty.
//  You may copy and distribute verbatim copies of this document.
//  You may modify and use this source code to create binary code
//  for your own purposes, free or commercial.
//
struct StSampleVoiceBiquad2 {
   // per channel

   // Filter type
   enum {
      LPF  = 1,  // Low Pass
      HPF  = 2,  // High Pass
      BPF  = 3,  // Band Pass
      BRF  = 4,  // Band Reject / Notch
      PEQ  = 5,  // Parametric EQ
      LSH  = 6,  // Low Shelf
      HSH  = 7   // High Shelf
   };

   sF64 x1, x2;  // input delayed by 1/2 samples
   sF64 y1, y2;  // output delayed by 1/2 samples
   sF64 z1, z2;

   inline void reset(void) {
      x1 = x2 = 0.0f;
      y1 = y2 = 0.0f;
      z1 = z2 = 0.0;

      a0 = 1.0;
      a1 = a2 = b1 = b2 = 0.0;

      cur_a0 = 1.0;
      cur_a1 = cur_a2 = cur_b1 = cur_b2 = 0.0;

      step_a0 = step_a1 = step_a2 = step_b1 = step_b2 = 0.0f;
   }

   sF64 a0, a1, a2, b1, b2;
   sF64 cur_a0, cur_a1, cur_a2, cur_b1, cur_b2;
   sF64 step_a0, step_a1, step_a2, step_b1, step_b2;

   void calcParams (sF32 _blkSz,
                    sSI _type,
                    sF32 _dbGain,
                    sF32 _freq,   // 0..1
                    sF32 _res     // 0..1
                    );

   // fc: 0..0.5, q=0..1
   void calcParamsLPFStatic (sF32 _fc, sF32 _q);

   void noStep (void) {
      cur_a0 = a0;
      cur_a1 = a1;
      cur_a2 = a2;
      cur_b1 = b1;
      cur_b2 = b2;
      step_a0 = 0.0f;
      step_a1 = 0.0f;
      step_a2 = 0.0f;
      step_b1 = 0.0f;
      step_b2 = 0.0f;
   }

   void singleStep (void) {
      cur_a0 = step_a0;
      cur_a1 = step_a1;
      cur_a2 = step_a2;
      cur_b1 = step_b1;
      cur_b2 = step_b2;
      step_a0 = 0.0f;
      step_a1 = 0.0f;
      step_a2 = 0.0f;
      step_b1 = 0.0f;
      step_b2 = 0.0f;
   }

   sF32 filter (const sF32 _inSmp);
   sF32 filterNoStep (const sF32 _inSmp);
   sF32 filterNoStepNoDenorm (const sF32 _inSmp);
};


#endif // ST_SAMPLEVOICEBIQUAD_H__
