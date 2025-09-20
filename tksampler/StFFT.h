// ----
// ---- file   : StFFT.h
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 1998-2024 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : This is part of the "syntracker" midi sequencer.
// ----
// ---- created: ?????1998
// ---- changed: 22Mar2011, 23Mar2011, 03Apr2013, 02Aug2016, 03May2019, 04May2019, 09May2019
// ----          10May2019, 02Jun2020, 03Jun2020, 04Oct2021, 03Oct2024
// ----
// ----
// ----

#ifndef ST_FFT_H__
#define ST_FFT_H__

YG("tksampler")


YC class StFFT_BandParams : public YAC_Object {

  public:
   sBool b_enable;
   sF32  center;
   sSI   harmonics;
   sF32  width;
   sF32  width_up;
   sF32  width_down;
   sF32  mag_scl;     // >=0: center is attenuated   <0: sides are attenuated
   sF32  mag_dcy_lin;
   sF32  mag_dcy_quad;

  public:
   sF32 band_freq;  // valid after init()
   sF32 band_freq_up;
   sF32 band_freq_down;


  public:
   StFFT_BandParams(void);
   ~StFFT_BandParams();

   YAC(StFFT_BandParams);

  public:
   // Init and update band_freq*
   YM void init(sBool _bEnable, sF32 _center, sSI _harmonics, sF32 _width, sF32 _widthUp, sF32 _widthDown, sF32 _magScl, sF32 _magDcyLin, sF32 _magDcyQuad);
};


YC class StFFT : public YAC_Object {
  public:
   sF32 *in_real;       // real / cosines
   sF32 *in_img;        // imaginary / sines (if NULL, assume 0.0f. useful when converting from time to frequency domain)
   sUI   in_real_skip;  // number of interleaved channels (1=tightly packed)
   sUI   in_img_skip;   // number of interleaved channels (1=tightly packed)
   sUI   num_samples;   // total number of samples (for clipping)

   // Sine / cosine LUT
   sF32 *sin_tab;

   sUI window_bits;
   sSI window_size;

   sF32 *fft_real; // Result (real part / cosines) (uses bitreversed indices)
   sF32 *fft_img;  // Result (imaginary part / sines) (uses bitreversed indices)

   sF32 *tmp_real; // Used during inverse FFT (real part / cosines) (identity indices)
   sF32 *tmp_img;  // Used during inverse FFT (imaginary part / sines) (identity indices)

   // Look up table for index bit reversion
   sUI *rev_tab;

   sSI window_type; // 0:none, 1:cos, 2:tri  (useful for overlap add)

   // Reference to current window function
   void (StFFT::*window_fxn)(sF32*, sF32*, sSI, sSI, sUI, sUI);

   sF32 test_ifft_minbin;  // logarithmic (0..1)
   sF32 test_ifft_maxbin;

   sBool b_scl_magnitudes;


  protected:
   static sF32 GetHammingPlatform(sSI win_l);

   static void Clip(sSI length, sSI &left, sSI &right, sSI &pre_0, sSI &post_0);

   // Read samples (no window function used)
   void getSafeWindow(sF32 *_s,            // Input samples (possibly interleaved)
                      sF32 *_d,            // receives windowed (non-interleaved) samples (tightly packed)
                      sSI _inBegOff,       // sample start offset (unclipped)
                      sSI _inEndOff,       // sample end offset (unclipped)
                      sUI _inSkip,         // number of interleaved channels (1=tightly packed)
                      sUI _inChIdx         // which channel
                      );

   // Read samples and apply cosine window
   void getSafeCosWindow(sF32 *_s,            // Input samples (possibly interleaved)
                         sF32 *_d,            // receives windowed (non-interleaved) samples (tightly packed)
                         sSI _inBegOff,       // sample start offset (unclipped)
                         sSI _inEndOff,       // sample end offset (unclipped)
                         sUI _inSkip,         // number of interleaved channels (1=tightly packed)
                         sUI _inChIdx         // which channel
                         );

   // Read samples and apply triangle window (unclipped)
   void getTriWindow(sF32 *_s,            // Input samples (possibly interleaved)
                     sF32 *_d,            // receives windowed (non-interleaved) samples (tightly packed)
                     sSI _inBegOff,       // sample start offset (clipped)
                     sSI _inEndOff,       // sample end offset (clipped)
                     sUI _inSkip,         // number of interleaved channels (1=tightly packed)
                     sUI _inChIdx         // which channel
                     );

   // Read samples and apply triangle window (clipped)
   void getSafeTriWindow(sF32 *_s,            // Input samples (possibly interleaved)
                         sF32 *_d,            // receives windowed (non-interleaved) samples (tightly packed)
                         sSI _inBegOff,       // sample start offset (unclipped)
                         sSI _inEndOff,       // sample end offset (unclipped)
                         sUI _inSkip,         // number of interleaved channels (1=tightly packed)
                         sUI _inChIdx         // which channel
                         );

   // Read samples and apply sine window (unclipped)
   void getSineWindow(sF32 *_s,            // Input samples (possibly interleaved)
                      sF32 *_d,            // receives windowed (non-interleaved) samples (tightly packed)
                      sSI _inBegOff,       // sample start offset (clipped)
                      sSI _inEndOff,       // sample end offset (clipped)
                      sUI _inSkip,         // number of interleaved channels (1=tightly packed)
                      sUI _inChIdx         // which channel
                      );

   void calcFFT(sBool _bInverse);

   void fft(sSI _inOffset, sSI _inChIdx);

   // Returns number of magnitudes written to _d
   sUI calcMagnitudes_32(sF32 *_d, sUI _maxSamples);  // 32bit
   sUI calcMagnitudes_64(sF32 *_d, sUI _maxSamples); // 64bit

   // Returns number of phases written to _d
   sUI calcPhases(sF32 *_d, sUI _maxSamples);

   sUI ifft(sF32 *_d, sUI _maxSamples);

   void ifftApplyBandParams (sF64 &mag, const sF32 freq, const sF32 _pitchScl, const sF32 _freqAdd, YAC_Object *_stfftBandParamsArray, const sUI _numBandParams);

  public:
   StFFT(void);
   ~StFFT();

   YAC(StFFT);


  public:

   // Set window size and allocate FFT buffers. Returns 0 if something went wrong (param out of range, alloc failed, ..)
   YM sBool setWindowSize (sUI _bits);

   // Set input samples and determine num_samples
   YM void _setInputSamples (YAC_Object *_inReal, sUI _inRealSkip, YAC_Object *_inImg, sUI _inImgSkip);

   // Perform the FFT using the current settings (result stored in fft_real/fft_img)
   YM sBool _fft (sSI _inOffset, sSI _inChIdx);

   YM void  _setEnableMagnitudeScaling (sBool _bEnable);
   YM sBool _getEnableMagnitudeScaling (void);

   // Calc bin magnitudes. Returns (window_size/2 +1) elements + number of bins (0u = error).
   YM sUI _calcMagnitudes (YAC_FloatArray *_d);

   // Calc bin phases. Returns (window_size/2 +1) elements.
   YM void _calcPhases (YAC_FloatArray *_d);

   // Apply +3dB/oct filter, e.g. for blue noise generation
   YM void _blueNoiseFilter (void);

   // Apply -3dB/oct filter, e.g. for pink noise generation
   YM void _pinkNoiseFilter (void);

   // Apply -6dB/oct filter, e.g. for brown noise generation
   YM void _brownNoiseFilter (void);

   // Apply +6dB/oct filter, e.g. for violet noise generation
   YM void _violetNoiseFilter (void);

   // Inverse FFT transformation
   YM void _ifft (YAC_FloatArray *_d);

   // Free internal buffers (automatically called in d'tor)
   YM void freeFFTBuffers (void);

   // 0: no window, 1:cos, 2:tri
   YM void setEnableWindow (sSI _type);
   YM sSI  getEnableWindow (void);

   // Can be used for pitch-shifting / time-stretching
   YM void shiftBins (sSI _num);

   YM void _testConvPolar (void);

   // freqRandExp: 1..~16  (e.g. 8)
   // freqRandScl: ~1..10  (e.g. 6)
   YM void _testIFFTSin (YAC_Object *_fa, sSI _frameOffset, sUI _numFrames, sF32 _sampleRate, sF32 _pitchScl, sF32 _freqAdd, sF32 _amp, sF32 _freqRandExp, sF32 _freqRandScl, sF32 _phaseRandAmt, YAC_Object *_stfftBandParamsArray);

   // generates single-cycle like loops
   YM void _testIFFTSin_MaxBin (YAC_Object *_fa, sUI _numFrames, sF32 _sampleRate, sF32 _pitchScl, sF32 _amp);

   // just a test: modulate bin frequencies by sine LFOs (=> pretty useless)
   YM void _testIFFTSin_ModLFOSin (YAC_Object *_fa, sUI _numFrames, sF32 _sampleRate, sF32 _pitchScl, sF32 _amp, sF32 _freqRandExp, sF32 _freqRandScl, sF32 _lfoSpeed);

   // Calculate true bin frequencies by looking at phase delta of fft that was taken "frameShift" sample frames later
   // freqRandExp: 1..~16  (e.g. 8)
   // freqRandScl: ~1..10  (e.g. 6)
   YM void _testIFFTSin_TrueFreq (YAC_Object *_fa, sSI _frameOffset, sUI _numFrames, sF32 _sampleRate, sF32 _pitchScl, sF32 _freqAdd, sF32 _amp, sF32 _freqRandExp, sF32 _freqRandScl, YAC_Object *_fftO, sUI _fftOFrameShift, sF32 _phaseRandAmt, YAC_Object *_stfftBandParamsArray);

   // relative (0..1)
   YM void _setTestIFFTMinBin (sF32 _f);
   YM void _setTestIFFTMaxBin (sF32 _f);
   YM sF32 _getTestIFFTMinBin (void);
   YM sF32 _getTestIFFTMaxBin (void);
};


#endif // ST_FFT_H__
