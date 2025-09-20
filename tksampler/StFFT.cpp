// ----
// ---- file   : StFFT.cpp
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 1998-2023 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : This is part of the "syntracker" midi sequencer.
// ----
// ---- created: ?????1998
// ---- changed: 22Mar2011, 23Mar2011, 03Apr2013, 01Aug2016, 02Aug2016, 03May2019, 04May2019
// ----          06May2019, 10May2019, 02Jun2020, 03Jun2020, 04Oct2021, 06Feb2023, 12Apr2023
// ----
// ----
// ----

#include "tksampler.h"

#include "StFFT.h"

#define Dprintf if(1); else Dyac_host_printf

#define PRINT_RESYNTH_MILLISEC defined


// ------------------------------------------------------------ FFT test code -----------------------------------------------
// originally written in 1998, interface/code cleanup done in march 2011
// testifft code added in 2019

// (note) maximum window size is (1<<14) = 16384


StFFT_BandParams::StFFT_BandParams(void) {
}

StFFT_BandParams::~StFFT_BandParams() {
}

void StFFT_BandParams::init(sBool _bEnable, sF32 _center, sSI _harmonics, sF32 _width, sF32 _widthUp, sF32 _widthDown, sF32 _magScl, sF32 _magDcyLin, sF32 _magDcyQuad) {
   b_enable     = _bEnable;
   center       = _center;
   harmonics    = _harmonics;
   width        = _width;
   width_up     = _widthUp;
   width_down   = _widthDown;
   mag_scl      = _magScl;
   mag_dcy_lin  = _magDcyLin;
   mag_dcy_quad = _magDcyQuad;

   band_freq = sF32((440.0f/32.0f)*expf( ((center-9.0f)/12.0f)*logf(2.0f) ));
   band_freq_up = sF32((440.0f/32.0f)*expf( ((center + width_up*width -9.0f)/12.0f)*logf(2.0f) ));
   band_freq_down = sF32((440.0f/32.0f)*expf( ((center - width_down*width -9.0f)/12.0f)*logf(2.0f) ));
}


StFFT::StFFT(void) {
   in_real     = NULL;
   in_img      = NULL;
   num_samples = 0;
   window_bits = 0;
   window_size = 0;
   rev_tab     = NULL;
   sin_tab     = NULL;
   fft_real    = NULL;
   fft_img     = NULL;
   tmp_real    = NULL;
   tmp_img     = NULL;

   setEnableWindow(1/*cos*/);

   test_ifft_minbin = 0.0f;
   test_ifft_maxbin = 1.0f;

   b_scl_magnitudes = YAC_TRUE;
}

StFFT::~StFFT() {

   // Free sine/cosine LUT
   if(NULL != sin_tab)
   {
      delete [] sin_tab;
      sin_tab = NULL;
   }

   freeFFTBuffers();
}

void StFFT::freeFFTBuffers(void) {

   // Free bitreversed index LUT
   if(NULL != rev_tab)
   {
      delete [] rev_tab;
      rev_tab = NULL;
   }

   // Free FFT result (real part)
   if(NULL != fft_real)
   {
      delete [] fft_real;
      fft_real = NULL;
   }

   // Free FFT result (imaginary part)
   if(NULL != fft_img)
   {
      delete [] fft_img;
      fft_img = NULL;
   }

   // Free FFT temporary (real part)
   if(NULL != tmp_real)
   {
      delete [] tmp_real;
      tmp_real = NULL;
   }

   // Free FFT temporary (imaginary part)
   if(NULL != tmp_img)
   {
      delete [] tmp_img;
      tmp_img = NULL;
   }
}

sBool StFFT::setWindowSize(sUI _bits) {

   freeFFTBuffers();

   //if(_bits <= 14)
   if(_bits <= 16) // [01Aug2016]
   {
      window_bits = _bits;
      window_size = (1 << _bits);

      if(NULL == sin_tab)
      {
         // Initialize sine/cosine LUT
         sin_tab = new sF32[65536+16384];
         if(NULL != sin_tab)
         {
            sF64 a = 0.0f;
            sF64 w = (2.0 * sM_PI) / 65536.0f;
            for(sSI i=0; i<(65536+16384); i++)
            {
               // // sin_tab[i]= (sF32) sin( (2.0f*sM_PI*((float)i))/ 65536.0f );
               sin_tab[i]= (sF32) sin(a);
               a += w;
            }
         }
      }

      // Allocate and initialize bit reverse LUT
      {
         rev_tab = new sUI[window_size];
         if(NULL != rev_tab)
         {
            for(sSI i=0; i<window_size; i++)
            {
               sSI k = i;
               sUI zi = 0;
               for(sUI j=0; j<window_bits; j++)
               {
                  zi= (zi<<1)|(k&1);
                  k = k >> 1;
               }
               rev_tab[i] = zi;
            }
         }
      }

      // Allocate real part result buffer
      fft_real = new sF32[window_size];

      // Allocate imaginary part result buffer
      fft_img  = new sF32[window_size];

      // Succeeded (if all allocations were carried out)
      sBool bOK =
         (NULL != sin_tab)  &&
         (NULL != rev_tab)  &&
         (NULL != fft_real) &&
         (NULL != fft_img)  ;

      if(!bOK)
      {
         window_bits = 0;
         window_size = 0;
         bOK = 0;
      }

      return bOK;
   }
   else
   {
      Dyac_host_printf("[---] StFFT::setWindowSize: illegal window size (%d bits > 14)\n", _bits);
   }

   // Failed
   return 0;
}

void StFFT::setEnableWindow(sSI _type) {
   window_type = _type;

   switch(window_type)
   {
      case 0:
         window_fxn = &StFFT::getSafeWindow;
         break;

      case 1:
         window_fxn = &StFFT::getSafeCosWindow;
         break;

      case 2:
         // window_fxn = &StFFT::getTriWindow;
         window_fxn = &StFFT::getSafeTriWindow;
         break;

      case 3:
         window_fxn = &StFFT::getSineWindow;
         break;
   }
}

sSI StFFT::getEnableWindow(void) {
   return window_type;
}

sF32 StFFT::GetHammingPlatform(sSI win_l) {
   // (todo) shouldn't this be a simply table lookup (index= window_bits) ?

   sF32 ham_platform;
   if(win_l>32768)
      ham_platform=0.0f;
   else
      if(win_l>16384)
         ham_platform=0.0f;
      else
         if(win_l>8192)
            ham_platform=0.0f;
         else
            if(win_l>4096)
               ham_platform=0.0f; //012;
            else
               if(win_l>2048)
                  ham_platform=0.010f;
               else
                  if(win_l>1024)
                     ham_platform=0.0046f;
                  else
                     if(win_l>512)
                        ham_platform=0.017f;
                     else
                        if(win_l>256)
                           ham_platform=0.026f;
                        else
                           if(win_l>128)
                              ham_platform=0.046f;
                           else
                              if(win_l>64)
                                 ham_platform=0.085f;
                              else
                                 if(win_l>32)
                                    ham_platform=0.085f;
                                 else
                                    if(win_l>16)
                                       ham_platform=0.15f;
                                    else
                                       ham_platform=0.33f;
   return ham_platform;
}

void StFFT::Clip(sSI length, sSI &left, sSI &right, sSI &pre_0, sSI &post_0) {
   if((left >= length) || (right < 0))
   {
      sSI winSz = right - left;
      pre_0  = winSz;
      post_0 = 0;
      left   = 0;
      right  = 0;
   }
   else
   {
      if(left < 0)
      {
         pre_0 = -left;
         left  = 0;
      }
      else
      {
         pre_0 = 0;
      }
      if(right >= length)
      {
         post_0 = right - length;
         right -= post_0;
      }
      else
      {
         post_0 = 0;
      }
   }
}

void StFFT::getSafeCosWindow(sF32 *_s,            // Input samples (possibly interleaved)
                             sF32 *_d,            // receives windowed (non-interleaved) samples (tightly packed)
                             sSI _inBegOff,       // sample start offset (unclipped)
                             sSI _inEndOff,       // sample end offset (unclipped)
                             sUI _inSkip,         // number of interleaved channels (1=tightly packed)
                             sUI _inChIdx         // which channel
                             )
{
   // (note) uses part of cosine table as window function

   sF32 *ct = &sin_tab[16384]; // reference to cosine table
   sU32 stepx = ((_inEndOff - _inBegOff)<<1);

   // adjustable hamming window
   sF32 ham_platform = GetHammingPlatform(_inEndOff - _inBegOff);

   // Clip sample selection (may modify start/end offsets)
   sSI pre0, post0;
   Clip((sSI)num_samples, _inBegOff, _inEndOff, pre0, post0);

   int i;

   // Add clipped samples (left hand side)
   for(i=0; i<pre0; i++)
   {
      *_d++ = 0.0f;
   }

   // Get reference to first sample (clipped)
   sF32 *a = _s + (_inBegOff * _inSkip) + _inChIdx;

   int rl = _inEndOff - _inBegOff;

   sU32 cntx = ((stepx * pre0)) - (16384<<16);

   // Write windowed samples
   for(i=0; i<rl; i++)
   {
      sF32 w = ham_platform + (1.0f - ham_platform) * ct[cntx>>16];
      w *= 8.0f;  // much more quiet than triangle window
      *_d++= *a * w;
      cntx += stepx;
      a += _inSkip;
   }

   // Add clipped samples (right hand side)
   for(i=0; i<post0; i++)
   {
      *_d++ = 0.0f;
   }
}

void StFFT::getTriWindow(sF32 *_s,            // Input samples (possibly interleaved)
                         sF32 *_d,            // receives windowed (non-interleaved) samples (tightly packed)
                         sSI _inBegOff,       // sample start offset (clipped)
                         sSI _inEndOff,       // sample end offset (clipped)
                         sUI _inSkip,         // number of interleaved channels (1=tightly packed)
                         sUI _inChIdx         // which channel
                         )
{
   sSI wl = (_inEndOff - _inBegOff);
   sSI hl = wl / 2;
   sF32 t = 0.0f;
   sF32 tStep = 1.0f / hl;

   // Get reference to first sample
   sF32 *a = _s + (_inBegOff * _inSkip) + _inChIdx;

   // Write windowed samples (ramp up)
   for(sSI i=0; i < hl; i++)
   {
      *_d++= *a * t;
      a += _inSkip;
      t += tStep;
   }

   hl = (wl - hl);
   t = 1.0f;

   // Write windowed samples (ramp down)
   for(sSI i=0; i < hl; i++)
   {
      *_d++= *a * t;
      a += _inSkip;
      t -= tStep;
   }
}

void StFFT::getSafeTriWindow(sF32 *_s,            // Input samples (possibly interleaved)
                             sF32 *_d,            // receives windowed (non-interleaved) samples (tightly packed)
                             sSI _inBegOff,       // sample start offset (clipped)
                             sSI _inEndOff,       // sample end offset (clipped)
                             sUI _inSkip,         // number of interleaved channels (1=tightly packed)
                             sUI _inChIdx         // which channel
                             )
{
   sSI wl = (_inEndOff - _inBegOff);
   sSI hl = wl / 2;
   sF32 t = 0.0f;
   sF32 tStep = 1.0f / hl;

   sSI i;
   // sSI downOff = 0;

   // Clip sample selection (may modify start/end offsets)
   sSI pre0, post0;
   Clip((sSI)num_samples, _inBegOff, _inEndOff, pre0, post0);

   // Dyac_host_printf("xxx inBegOff=%d inEndOff=%d pre0=%d post0=%d\n", _inBegOff, _inEndOff, pre0, post0);

   // Add clipped samples (left hand side)
   for(i=0; i < pre0; i++)
   {
      *_d++ = 0.0f;
   }

   if(pre0 < hl)
   {
      t = float(pre0) / hl;
   }
   else
   {
      t = 1.0f - (float(pre0 - hl) / hl);
      // downOff = (pre0 - hl);
   }

   // Get reference to first sample
   sF32 *a = _s + (_inBegOff * _inSkip) + _inChIdx;

   sUI readOff = _inBegOff;

   if(pre0 < hl)
   {
      // Write windowed samples (ramp up)
      sSI numRamp;
      if((_inEndOff - _inBegOff) < hl)
      {
         numRamp = (_inEndOff - _inBegOff);
      }
      else
      {
         numRamp = hl;
      }
      for(sSI i=0; i < numRamp; i++)
      {
         *_d++= *a * t;
         a += _inSkip;
         t += tStep;
      }
      readOff += numRamp;

      hl = (wl - hl);
      t = 1.0f;
   }

   // Write windowed samples (ramp down)
   {
      sSI numRamp;

      if( (sSI(readOff) + hl) > sSI(_inEndOff) )
      {
         if(sSI(readOff) >= _inEndOff)
         {
            numRamp = 0;
         }
         else
         {
            numRamp = sSI(_inEndOff) - sSI(readOff);
         }
      }
      else
      {
         numRamp = hl;
      }

      for(sSI i=0; i < numRamp; i++)
      {
         *_d++= *a * t;
         a += _inSkip;
         t -= tStep;
      }
      readOff += numRamp;
   }

   // Add clipped samples (right hand side)
   for(i=0; i < post0; i++)
   {
      *_d++ = 0.0f;
   }
}

void StFFT::getSineWindow(sF32 *_s,            // Input samples (possibly interleaved)
                          sF32 *_d,            // receives windowed (non-interleaved) samples (tightly packed)
                          sSI _inBegOff,       // sample start offset (clipped)
                          sSI _inEndOff,       // sample end offset (clipped)
                          sUI _inSkip,         // number of interleaved channels (1=tightly packed)
                          sUI _inChIdx         // which channel
                          )
{
   sSI wl = (_inEndOff - _inBegOff);
   sF32 t = 0.0f;
   sF32 tStep = 32768.0f / wl;
   const sF32 *ct = &sin_tab[0];

   // Get reference to first sample
   sF32 *a = _s + (_inBegOff * _inSkip) + _inChIdx;

   // Write windowed samples
   for(sSI i=0; i < wl; i++)
   {
      *_d++= *a * ct[sUI(t)];
      a += _inSkip;
      t += tStep;
   }
}

void StFFT::getSafeWindow(sF32 *_s,            // Input samples (possibly interleaved)
                          sF32 *_d,            // receives windowed (non-interleaved) samples (tightly packed)
                          sSI _inBegOff,       // sample start offset (unclipped)
                          sSI _inEndOff,       // sample end offset (unclipped)
                          sUI _inSkip,         // number of interleaved channels (1=tightly packed)
                          sUI _inChIdx         // which channel
                          )
{
   // (note) uses part of cosine table as window function

   // Clip sample selection (may modify start/end offsets)
   sSI pre0, post0;
   Clip((sSI)num_samples, _inBegOff, _inEndOff, pre0, post0);

   // Add clipped samples (left hand side)
   memset(_d, 0, sizeof(sF32) * pre0);
   _d += pre0;

   int rl = _inEndOff - _inBegOff;

   // Get reference to first sample (clipped)
   sF32 *a = _s + (_inBegOff * _inSkip) + _inChIdx;

   // Write windowed samples
   for(sSI i=0; i<rl; i++)
   {
      *_d++ = *a;
      a += _inSkip;
   }

   // Add clipped samples (right hand side)
   memset(_d, 0, sizeof(sF32) * post0);
}

void StFFT::calcFFT(sBool _bInverse) {
   // Non-recursive implementation of Sande-Tukey FFT
   //
   //  (note) when doing an inverse transformation, the sign of the angle in the complex from polar calculation
   //          needs to be changed (otherwise the samples are resynthesized in reverse order)

   {
      sSI n2 = (sSI) window_size;

      sF32 sign = _bInverse ? 1.0f : -1.0f;

      for(sUI k=0; k<window_bits; k++)
      {
         sSI n1 = n2;
         n2 = n2 / 2;

         sSI angle    = 0;
         sSI argument = 0x10000 / n1;

         for(sSI j=0; j<n2; j++)
         {
            sF32 c =        sin_tab[(sU16) (16384+angle)];
            sF32 s = sign * sin_tab[(sU16) (angle)];

            for(sSI i=j; i<window_size; i+=n1)
            {
               sSI l = i + n2;

               // t = fft[i] - fft[i+n2]
               sF32 treal  = fft_real[i] - fft_real[l];
               sF32 timg   = fft_img [i] - fft_img [l];

               // fft[i] = fft[i] + fft[i+n2]
               fft_real[i] = fft_real[i] + fft_real[l];
               fft_img [i] = fft_img [i] + fft_img [l];

               //  (resonate / multiply by twiddle factor)
               fft_real[l] = (treal * c) - (timg  * s);
               fft_img [l] = (timg  * c) + (treal * s);
            }

            angle = (j+1) * argument;
         }
      }
   }
}

void StFFT::fft(sSI _inOffset, // input sampleoffset
                sSI _inChIdx   // channel index
                )
{

   // Get zero-padded samples (real part)
   (this->*window_fxn)(in_real,
                       fft_real,
                       _inOffset - window_size/2,
                       _inOffset + window_size/2,
                       in_real_skip,
                       _inChIdx
                       );

   if(NULL != in_img)
   {
      // Get zero-padded samples (imaginary part)
      (this->*window_fxn)(in_img,
                          fft_img,
                          _inOffset - window_size/2,
                          _inOffset + window_size/2,
                          in_img_skip,
                          _inChIdx
                          );
   }
   else
   {
      // No imaginary input available, use 0s
      memset(fft_img, 0, sizeof(sF32) * window_size);
   }

   calcFFT(0);
}

sUI StFFT::ifft(sF32 *_d, sUI _maxSamples) {

   // (todo) optimize me
   // (todo) add support for interleaved channels

   // (todo) output frequency is OK but amplitude is too low (and fades out towards the end of the window)
   //         ? a scaling issue. missing twiddle factors ?!

   if(window_size > 0)
   {
      if(NULL == tmp_real)
      {
         // Allocate real part temporary buffer
         tmp_real = new sF32[window_size];
      }

      if(NULL == tmp_img)
      {
         // Allocate imaginary part temporary buffer
         tmp_img  = new sF32[window_size];
      }

      // Re-order bins
      sF32 scl = 1.0f / window_size;
      for(sSI i=0; i<window_size; i++)
      {
         sUI zi = rev_tab[i];

         tmp_real[i] = scl * fft_real[zi];
         tmp_img [i] = scl * fft_img [zi];
      }

      memcpy(fft_real, tmp_real, sizeof(sF32) * window_size);
      memcpy(fft_img , tmp_img , sizeof(sF32) * window_size);

      calcFFT(1);

      // Write real part to output buffer
      sUI numSmp = (_maxSamples < (sUI)window_size) ? _maxSamples : (sUI)window_size;
      for(sUI i=0; i<numSmp; i++)
      {
         *_d++ = fft_real[rev_tab[i]];
      }

      return numSmp;
   }
   return 0;
}

void StFFT::_ifft(YAC_FloatArray *_d) {
   if(YAC_Is_FloatArray(_d))
   {
      YAC_CAST_ARG(YAC_FloatArray, d, _d);

      sUI num = ifft(d->elements+d->num_elements, (d->max_elements-d->num_elements));

      d->num_elements += num;
   }
}

sUI StFFT::calcMagnitudes_32(sF32 *_d, sUI _maxSamples) {

   if(window_size > 0)
   {
      sF32 scl = b_scl_magnitudes ? (2.0f / window_size) : 1.0f;

      sUI numBins = (window_size/2 +1);
      if(_maxSamples < numBins)
         numBins = _maxSamples;

      for(sUI i=0; i<numBins; i++)
      {
         sUI zi = rev_tab[i];

         sF32 cv = scl * sqrtf( fft_real[zi]*fft_real[zi] + fft_img[zi]*fft_img[zi] );

         *_d++ = cv;
      }

      return numBins;
   }
   else
   {
      Dyac_host_printf("[---] StFFT::calcMagnitudes_32: invalid window_size=%d\n", window_size);
   }
   return 0;
}

sUI StFFT::calcMagnitudes_64(sF32 *_d, sUI _maxSamples) {

   if(window_size > 0)
   {
      sF64 scl = b_scl_magnitudes ? (2.0 / window_size) : 1.0;

      sUI numBins = (window_size/2 +1);
      if(_maxSamples < numBins)
         numBins = _maxSamples;

      for(sUI i=0; i<numBins; i++)
      {
         sUI zi = rev_tab[i];

         sF32 cv = sF32(scl * sqrt( sF64(fft_real[zi])*sF64(fft_real[zi]) + sF64(fft_img[zi])*sF64(fft_img[zi]) ));

         *_d++ = cv;
      }

      return numBins;
   }
   else
   {
      Dyac_host_printf("[---] StFFT::calcMagnitudes_64: invalid window_size=%d\n", window_size);
   }
   return 0;
}

void StFFT::_setEnableMagnitudeScaling(sBool _bEnable) {
   b_scl_magnitudes = _bEnable;
}

sBool StFFT::_getEnableMagnitudeScaling(void) {
   return b_scl_magnitudes;
}

sUI StFFT::_calcMagnitudes(YAC_FloatArray *_d) {
   if(YAC_Is_FloatArray(_d))
   {
      YAC_CAST_ARG(YAC_FloatArray, d, _d);

      sUI num = calcMagnitudes_64(d->elements, d->max_elements);

      if(num > d->num_elements)
      {
         d->num_elements = num;
      }

      return num;
   }
   return 0u;
}

sUI StFFT::calcPhases(sF32 *_d, sUI _maxSamples) {

   if(window_size > 0)
   {
      sUI numBins = (window_size/2 +1);
      if(_maxSamples < numBins)
         numBins = _maxSamples;

      for(sUI i=0; i<numBins; i++)
      {
         sUI zi = rev_tab[i];

         *_d++ = atan2f(fft_img[zi], fft_real[zi]);
      }

      return numBins;
   }
   return 0;
}

void StFFT::_calcPhases(YAC_FloatArray *_d) {
   if(YAC_Is_FloatArray(_d))
   {
      YAC_CAST_ARG(YAC_FloatArray, d, _d);

      sUI num = calcPhases(d->elements, d->max_elements);

      if(num > d->num_elements)
      {
         d->num_elements = num;
      }
   }
}

void StFFT::_blueNoiseFilter(void) {
   // +3dB/oct filter, e.g. for blue noise generation
   // sF32 _sampleRate = 44100.f;

   if(window_size > 0)
   {
      sUI numBins = (window_size/2 +1);
      // Dyac_host_printf("xxx highpass3dB: window_size=%u / window_bits=%u => numBins=%u\n", window_size, window_bits, numBins);

      sF32 *nreal = new sF32[window_size];
      sF32 *nimg  = new sF32[window_size];
      memset(nreal, 0, sizeof(sF32)*window_size);
      memset(nimg , 0, sizeof(sF32)*window_size);

      // float freq = 0.0f;
      // float freqStep = _sampleRate / float(window_size);

      // -3dB = *0.70710678118800000000
      // bin 8192/16384 = 11025 Hz

      for(sUI i = 0u; i < numBins; i++)
      {
         sUI zi = rev_tab[i];

         sF32 fr = fft_real[zi];
         sF32 fi = fft_img[zi];

         sF64 phase = atan2(fi, fr); // => -PI..PI
         sF64 mag   = sqrt(fr*fr + fi*fi);

         if(i == 0)
            mag = 0.0f;
         else
         {
            // (note) +3dB/oct power increase
            float s = (float(numBins) / float(i));
            s = logf(s) / logf(2.0f);
            s = powf(0.707106781188f/*dbToLevel(-3dB)*/, s);
            mag *= s;
            // if(i < 4000)
            //    Dyac_host_printf("s[%u]=%f => mag=%f  freq=%f dB=%f\n", i, s, mag, freq, (20.0f * log(s) / log(10.0f)));
         }

         nreal[zi] = (sF32) (mag * cos(phase));
         nimg [zi] = (sF32) (mag * sin(phase));

         // Dyac_host_printf("old[%u] = (%f; %f)  new[%u] = (%f; %f)\n", i, fr, fi, i, nreal[zi], nimg[zi]);

         // freq += freqStep;
      }

      delete [] fft_real;
      delete [] fft_img;

      fft_real = nreal;
      fft_img  = nimg;
   }
}

void StFFT::_pinkNoiseFilter(void) {
   // -3dB/oct filter

   if(window_size > 0)
   {
      sUI numBins = (window_size/2 +1);
      // Dyac_host_printf("xxx highpass3dB: window_size=%u / window_bits=%u => numBins=%u\n", window_size, window_bits, numBins);

      sF32 *nreal = new sF32[window_size];
      sF32 *nimg  = new sF32[window_size];
      memset(nreal, 0, sizeof(sF32)*window_size);
      memset(nimg , 0, sizeof(sF32)*window_size);

      // float freq = 0.0f;
      // sF32 _sampleRate = 44100.f;
      // float freqStep = _sampleRate / float(window_size);

      // -3dB = *0.70710678118800000000
      // bin 8192/16384 = 11025 Hz
      float powMax = float(1 << ((window_bits-1)/2));

      for(sUI i = 0u; i < numBins; i++)
      {
         sUI zi = rev_tab[i];

         sF32 fr = fft_real[zi];
         sF32 fi = fft_img[zi];

         sF64 phase = atan2(fi, fr); // => -PI..PI
         sF64 mag   = sqrt(fr*fr + fi*fi);

         if(i == 0)
            mag = 1.0f;
         else
         {
            // (note) -3dB/oct power decay
            float s = (float(i) / float(numBins));
            s = logf(s) / logf(2.0f);
            s = powf(0.707106781188f/*dbToLevel(-3dB)*/, s);
            s /= powMax;
            mag *= s;
            // if(i < 4000)
            //    Dyac_host_printf("s[%u]=%f => mag=%f  freq=%f dB=%f\n", i, s, mag, freq, (20.0f * log(s) / log(10.0f)));
         }

         nreal[zi] = (sF32) (mag * cos(phase));
         nimg [zi] = (sF32) (mag * sin(phase));

         // Dyac_host_printf("old[%u] = (%f; %f)  new[%u] = (%f; %f)\n", i, fr, fi, i, nreal[zi], nimg[zi]);

         // freq += freqStep;
      }

      delete [] fft_real;
      delete [] fft_img;

      fft_real = nreal;
      fft_img  = nimg;
   }
}

void StFFT::_brownNoiseFilter(void) {
   // -6dB/oct filter

   if(window_size > 0)
   {
      sUI numBins = (window_size/2 +1);
      // Dyac_host_printf("xxx highpass3dB: window_size=%u / window_bits=%u => numBins=%u\n", window_size, window_bits, numBins);

      sF32 *nreal = new sF32[window_size];
      sF32 *nimg  = new sF32[window_size];
      memset(nreal, 0, sizeof(sF32)*window_size);
      memset(nimg , 0, sizeof(sF32)*window_size);

      float freq = 0.0f;
      sF32 _sampleRate = 44100.f;
      float freqStep = _sampleRate / float(window_size);

      // -6dB = *0.5
      // bin 8192/16384 = 11025 Hz
      float powMax = float(1 << ((window_bits-1)));

      for(sUI i = 0u; i < numBins; i++)
      {
         sUI zi = rev_tab[i];

         sF32 fr = fft_real[zi];
         sF32 fi = fft_img[zi];

         sF64 phase = atan2(fi, fr); // => -PI..PI
         sF64 mag   = sqrt(fr*fr + fi*fi);

         if(i == 0)
            mag = 1.0f;
         else
         {
            // (note) -6dB/oct power decay
            float s = (float(i) / float(numBins));
            s = logf(s) / logf(2.0f);
            s = powf(0.5f/*dbToLevel(-6dB)*/, s);
            s /= powMax;
            mag *= s;
            // if(i < 4000)
            //    Dyac_host_printf("s[%u]=%f => mag=%f  freq=%f dB=%f\n", i, s, mag, freq, (20.0f * log(s) / log(10.0f)));
         }

         nreal[zi] = (sF32) (mag * cos(phase));
         nimg [zi] = (sF32) (mag * sin(phase));

         // Dyac_host_printf("old[%u] = (%f; %f)  new[%u] = (%f; %f)\n", i, fr, fi, i, nreal[zi], nimg[zi]);

         freq += freqStep;
      }
      (void)freq;

      delete [] fft_real;
      delete [] fft_img;

      fft_real = nreal;
      fft_img  = nimg;
   }
}

void StFFT::_violetNoiseFilter(void) {
   // +6dB/oct filter

   if(window_size > 0)
   {
      sUI numBins = (window_size/2 +1);
      // Dyac_host_printf("xxx highpass3dB: window_size=%u / window_bits=%u => numBins=%u\n", window_size, window_bits, numBins);

      sF32 *nreal = new sF32[window_size];
      sF32 *nimg  = new sF32[window_size];
      memset(nreal, 0, sizeof(sF32)*window_size);
      memset(nimg , 0, sizeof(sF32)*window_size);

      float freq = 0.0f;
      sF32 _sampleRate = 44100.f;
      float freqStep = _sampleRate / float(window_size);

      // 6dB = 0.5
      // bin 8192/16384 = 11025 Hz

      for(sUI i = 0u; i < numBins; i++)
      {
         sUI zi = rev_tab[i];

         sF32 fr = fft_real[zi];
         sF32 fi = fft_img[zi];

         sF64 phase = atan2(fi, fr); // => -PI..PI
         sF64 mag   = sqrt(fr*fr + fi*fi);

         if(i == 0)
            mag = 0.0f;
         else
         {
            // (note) +6dB/oct power increase
            float s = (float(numBins) / float(i));
            s = logf(s) / logf(2.0f);
            s = powf(0.5f/*dbToLevel(-6dB)*/, s);
            mag *= s;
            if(i < 4000)
               Dyac_host_printf("s[%u]=%f => mag=%f  freq=%f dB=%f\n", i, s, mag, freq, (20.0f * log(s) / log(10.0f)));
         }

         nreal[zi] = (sF32) (mag * cos(phase));
         nimg [zi] = (sF32) (mag * sin(phase));

         // Dyac_host_printf("old[%u] = (%f; %f)  new[%u] = (%f; %f)\n", i, fr, fi, i, nreal[zi], nimg[zi]);

         freq += freqStep;
      }

      delete [] fft_real;
      delete [] fft_img;

      fft_real = nreal;
      fft_img  = nimg;
   }
}

void StFFT::_setInputSamples(YAC_Object *_inReal, sUI _inRealSkip,
                             YAC_Object *_inImg, sUI _inImgSkip
                             ) {

   if(YAC_Is_FloatArray(_inReal))
   {
      if(_inRealSkip > 0)
      {
         YAC_CAST_ARG(YAC_FloatArray, inReal, _inReal);

         in_real      = inReal->elements;
         in_real_skip = _inRealSkip;

         num_samples = inReal->num_elements / in_real_skip;

         if(YAC_Is_FloatArray(_inImg))
         {
            // Ok, have imaginary part (freq => time)
            YAC_CAST_ARG(YAC_FloatArray, inImg, _inImg);

            in_img      = inImg->elements;
            in_img_skip = _inImgSkip;

            if(0 == in_img_skip)
            {
               Dyac_host_printf("[~~~] StFFT::setInputSamples: inImgSkip is 0 (=>substituting with 1..)\n");
               in_img_skip = 1;
            }

            sUI imgNumSamples = (inImg->num_elements / in_img_skip);

            if(imgNumSamples < num_samples)
            {
               Dyac_host_printf("[~~~] StFFT::setInputSamples: in_img #elements (%u) is < in_real #elements (%u) !\n",
                                inImg->num_elements, inReal->num_elements
                                );

               num_samples = imgNumSamples;
            }
         }
         else
         {
            // Only have real part (time => frequency)
            in_img      = NULL;
            in_img_skip = 0;
         }
      }
   }
   else
   {
      Dyac_throw_def(InvalidPointer, "StFFT::setInputSamples: inReal is not a valid object");
   }

}

sBool StFFT::_fft(sSI _inOff,      // in_real/in_img sample offset
                  sSI _inChIdx     // which channel
                  ) {

   if((NULL != in_real) && (in_real_skip > 0) && (num_samples > 0) && (window_size > 0))
   {
      fft(_inOff, _inChIdx);
      return 1;
   }
   else
   {
      Dyac_host_printf("[---] StFFT::fft: invalid input parameters\n");

      printf("xxx in_real=%p in_real_skip=%d #samples=%d window_size=%d\n",
             in_real, in_real_skip, num_samples, window_size);

      // Failed
      return 0;
   }
}

void StFFT::shiftBins(sSI _num) {

   // (note) not of any practical use since this simply shifts the frequencies

   if(window_size > 0)
   {
      if(_num >= 0)
      {
         sF32 *nreal = new sF32[window_size];
         sF32 *nimg  = new sF32[window_size];

         memset(nreal, 0, sizeof(sF32)*window_size); //xxx
         memset(nimg , 0, sizeof(sF32)*window_size); //xxx

         // sSI j = window_size - _num;
         // for(sSI i=0; i<_num; i++)
         // {
         //    //nreal[rev_tab[i]] = fft_real[rev_tab[j]];
         //    //nimg [rev_tab[i]] = fft_img [rev_tab[j]];
         //    nreal[rev_tab[i]] = 0.0f;
         //    nimg [rev_tab[i]] = 0.0f;
         //    j++;
         // }

         // sSI doff = 1+window_size/2 - 1;
         // sSI soff = 1+window_size/2 - 1 - _num;

         sSI doff = 1+window_size/2 - 1;
         sSI soff = 1+window_size/2 - 1 - _num;

         while(soff >= 0)
         {
            nreal[rev_tab[doff]] = fft_real[rev_tab[soff]];
            nimg [rev_tab[doff]] = fft_img [rev_tab[soff]];

            doff--;
            soff--;
         }

         delete [] fft_real;
         delete [] fft_img;

         fft_real = nreal;
         fft_img  = nimg;
      }
      else if(_num < 0)
      {
      }
   }
}

void StFFT::_testConvPolar(void) {

   // convert to polar and back (=> ifft will still resynthesize original waveform)

   sSI binIdx = 0;

   for(; binIdx < window_size/2; binIdx++)
   {
      sF32 fr    = fft_real[rev_tab[binIdx]];
      sF32 fi    = fft_img[rev_tab[binIdx]];
      sF64 phase = atan2(fi, fr); // => -PI..PI
      sF64 mag   = 0.5f * sqrt(fr*fr + fi*fi);

      // printf("xxx [%i] phase=%f mag=%f\n", binIdx, phase, mag);

      // if(phase < 0)
      //    phase += 2.0 * sM_PI;

      fft_real[rev_tab[binIdx]] = (sF32) (mag * cos(phase));
      fft_img [rev_tab[binIdx]] = (sF32) (mag * sin(phase));
   }
}

void StFFT::_setTestIFFTMinBin(sF32 _f) {
   test_ifft_minbin = _f;
}

void StFFT::_setTestIFFTMaxBin(sF32 _f) {
   test_ifft_maxbin = _f;
}

sF32 StFFT::_getTestIFFTMinBin(void) {
   return test_ifft_minbin;
}

sF32 StFFT::_getTestIFFTMaxBin(void) {
   return test_ifft_maxbin;
}

#if 0
void StFFT::_testIFFTSin(YAC_Object *_fad, sUI _numFrames, sF32 _sampleRate, sF32 _scl) {
   // OLD unfinished code (pre 2013)
   if(YAC_Is_FloatArray(_fad))
   {
      YAC_CAST_ARG(YAC_FloatArray, fad, _fad);

      sUI numSamples = (window_size);

      if(fad->num_elements <= (fad->max_elements - numSamples))
      {
         sF32 *d = fad->elements + fad->num_elements;

         sF64 scl = _scl * 2.0 / window_size;

         memset(d, 0, sizeof(sF32) * numSamples);

         sUI binIdx = 0;

         for(; binIdx < numSamples; binIdx++)
         {
            sF64 fr    = fft_real[rev_tab[binIdx]];
            sF64 fi    = fft_img[rev_tab[binIdx]];
            sF64 phase = atan2(fi, fr); // => -PI..PI
            sF64 mag   = scl * sqrt(fr*fr + fi*fi);

            sF64 a = phase;
            // sF64 w = (sM_PI * 2 * (1+binIdx) * (_sampleRate / numSamples)) / _sampleRate;
            sF64 freq = (binIdx * _sampleRate) / window_size;
            sF64 w = (freq * sM_2PI) / _sampleRate;

// (binIdx * 2 * pi * 0.5) / (window_size * sample_rate)

            sUI i = 0;
            for(; i<numSamples; i++)
            {
               sF32 smp = d[i];

               smp = (sF32) (smp + mag * sin(a));

               d[i] = smp;

               a += w;
            }
         }

         // // {
         // //    sUI i = 0;
         // //    for(; i<numSamples; i++)
         // //    {
         // //       d[i] *= _scl/*0.002f*/;
         // //    }
         // // }

         fad->num_elements += numSamples;
      }
   }
}
#endif

static sF32 randf(sF32 _max) {
   return (sF32) ((rand()*(0.999999999999f/((sF32)RAND_MAX)))*_max);
}

// (note) according to https://www.music.mcgill.ca/~gary/307/week11/ola.html this needs overlap-add resynthesis to prevent clicks / pops (=> done, see "research/spectral_resynthesis/spec.tks")

void StFFT::_testIFFTSin(YAC_Object *_fad, sSI _frameOffset, sUI _numFrames, sF32 _sampleRate, sF32 _pitchScl, sF32 _freqAdd, sF32 _amp, sF32 _freqRandExp, sF32 _freqRandScl, sF32 _phaseRandAmt, YAC_Object *_stfftBandParamsArray) {
   // works but sounds like a badly looped sample (neighbouring bins cause phase cancelation??)
   //  even though the phases are free-running, the sample loops at the fft block size
   if(YAC_Is_FloatArray(_fad))
   {
      YAC_CAST_ARG(YAC_FloatArray, fad, _fad);

      sUI skipFrames;
      if(_frameOffset < 0)
      {
         skipFrames = sUI(-_frameOffset);
         if(skipFrames >= _numFrames)
            return;
         _frameOffset = 0u;
         _numFrames -= skipFrames;
      }
      else
      {
         skipFrames = 0u;
      }

      sUI numBandParams = 0u;
      if(YAC_VALID(_stfftBandParamsArray))
      {
         numBandParams = _stfftBandParamsArray->yacArrayGetNumElements();
      }

      if((sUI(_frameOffset) + _numFrames) <= fad->max_elements)
      {
         sF32 *d = fad->elements + sUI(_frameOffset);

         sF64 scl = _amp * 2.0 / window_size;

         sUI numBins = sUI(window_size / 2);

         // sF64 *phases = new sF64[window_size];
         sU16 *phases = new sU16[numBins];
         sF64 *mags   = new sF64[numBins];
         // sF64 *speeds = new sF64[window_size];
         sU16 *speeds = new sU16[numBins];
         sUI binIdx;

         sF32 fMinBin = logf(2.0f + 2.0f*test_ifft_minbin) / logf(2.0f) - 1.0f;
         sF32 fMaxBin = logf(2.0f + 2.0f*test_ifft_maxbin) / logf(2.0f) - 1.0f;
         sUI minBin = sUI(fMinBin * numBins);
         sUI maxBin = sUI(fMaxBin * numBins);

         // Calc start phases and magnitudes
         for(binIdx = minBin; binIdx < maxBin; binIdx++)
         {
            sF64 fr    = fft_real[rev_tab[binIdx]];
            sF64 fi    = fft_img[rev_tab[binIdx]];
            sF64 phase;

            {
               phase = atan2(fi, fr); // => -PI..PI
               // // if(phase < 0.0)
               // //    phase += sM_PI * 2.0;
               phase /= (sM_PI * 2.0);  // => -0.5..0.5
               if(phase < 0.0)
                  phase += 1.0;
            }

            if(binIdx > 0)
            {
               if(_phaseRandAmt > 0.0f)
                  phase += randf(_phaseRandAmt);
            }

            sF64 mag   = scl * sqrt(fr*fr + fi*fi);
            // (todo) remove _sampleRate (assume that src sample rate is same as out sample rate)
            sF64 freq = (binIdx * _sampleRate) / window_size;

            if(binIdx > 0)  // don't randomize DC
            {
#if 1
               // freq randomization to avoid "beating" sampleloop effect
               // sF32 randMax = (sF32(binIdx)/numBins) * 62.0f;
               sF32 randMax = powf(2.0f, _freqRandExp * (sF32(1.0f + binIdx)/numBins)) * _freqRandScl;
               freq += randf( randMax * 2.0f ) - randMax;
#endif
            }

            // // sF64 w = (freq * (sM_PI * 2.0)) / _sampleRate;
            if(binIdx > 0)
            {
               freq = freq * _pitchScl + _freqAdd;
            }

            sF64 w = freq / _sampleRate;
            phases[binIdx] = sU16(phase * 65536);
            speeds[binIdx] = sU16(w * 65536);

            ifftApplyBandParams(mag, sF32(freq), _pitchScl, _freqAdd, _stfftBandParamsArray, numBandParams);

            if((0 == binIdx) || ((freq > 0.0f) && (freq <= _sampleRate*0.5)))
            {
               mags[binIdx] = mag;
            }
            else
            {
               mags[binIdx] = 0;
            }

            Dprintf("xxx ifftsin: bin[%4u] freq=%f w=%f speed.16=%u mag=%f\n", binIdx, freq, w, speeds[binIdx], mag);
         }

#ifdef PRINT_RESYNTH_MILLISEC
         sUI ms = Dyac_host_yacMilliSeconds();
#endif // PRINT_RESYNTH_MILLISEC

         // const sF32 *ctab = &sin_tab[16384];
         const sF32 *stab = &sin_tab[0];

         if(skipFrames > 0u)
         {
            for(binIdx = minBin; binIdx < maxBin; binIdx++)
            {
               if(mags[binIdx] > 0)
               {
                  phases[binIdx] = sU16(phases[binIdx] + skipFrames * speeds[binIdx]);
               }
            }
         }

         for(sUI frameIdx = 0u; frameIdx < _numFrames; frameIdx++)
         {
            sF64 smp = 0.0f;

            for(binIdx = minBin; binIdx < maxBin; binIdx++)
            {
               if(mags[binIdx] > 0)
               {
                  smp += (mags[binIdx] * stab[phases[binIdx]]);

                  phases[binIdx] += speeds[binIdx];
                  // phases[binIdx] = (sU32(phases[binIdx]) + sU32(speeds[binIdx])) & 65535u;
               }
            }

            d[frameIdx] += sF32(smp);
         } // loop frames

#ifdef PRINT_RESYNTH_MILLISEC
         ms = Dyac_host_yacMilliSeconds() - ms;
         sF32 realtimePct = 0.0f;
         if(ms > 0)
         {
            sF32 numFramesPerSec = sF32(1000.0f * _numFrames) / ms;
            realtimePct = numFramesPerSec / _sampleRate;
         }
         Dyac_host_printf("xxx StFFFT::testIFFTSin: %u frames took %u milliseconds (%f x realtime => %f%% DSP load)\n", _numFrames, ms, realtimePct, (realtimePct > 0.0f) ? (100.0f / realtimePct) : 0.0f);
#endif // PRINT_RESYNTH_MILLISEC

         // // fad->num_elements = _numFrames;

         delete [] phases;
         delete [] mags;
         delete [] speeds;
      } // if fits dest array
   } // if is floatarray
}

void StFFT::_testIFFTSin_MaxBin(YAC_Object *_fad, sUI _numFrames, sF32 _sampleRate, sF32 _pitchScl, sF32 _amp) {
   // test code. find max bin and reset phases at bin frequency (generates single-cycle like loops)
   if(YAC_Is_FloatArray(_fad))
   {
      YAC_CAST_ARG(YAC_FloatArray, fad, _fad);

      if(_numFrames <= fad->max_elements)
      {
         sF32 *d = fad->elements;

         sF64 scl = _amp * 2.0 / window_size;
         // // sF64 scl = _amp / window_size;

         sUI numBins = sUI(window_size / 2);
         // sUI numBins = sUI(window_size);

         sU16 *phases = new sU16[numBins];
         sU16 *phasesRst = new sU16[numBins];
         sF64 *mags   = new sF64[numBins];
         sU16 *speeds = new sU16[numBins];
         sUI binIdx;

         sUI maxBin = 0u;
         sF64 maxMag = 0.0f;
         sF64 maxFreq = 0.0f;

         // Calc start phases and magnitudes
         for(binIdx = 0u; binIdx < numBins; binIdx++)
         {
            sF64 fr    = fft_real[rev_tab[binIdx]];
            sF64 fi    = fft_img[rev_tab[binIdx]];
            sF64 phase = atan2(fi, fr); // => -PI..PI
            if(phase < 0.0)
               phase += sM_PI * 2.0;
            sF64 mag   = scl * sqrt(fr*fr + fi*fi);
            // // if(mag < 0.01)
            // //    mag = 0.0f; // xxx
            // (todo) remove _sampleRate (assume that src sample rate is same as out sample rate)
            sF64 freq = (binIdx * _sampleRate) / window_size;
            // sF64 w = (freq * (sM_PI * 2.0)) / _sampleRate;
            sF64 w = (freq * _pitchScl) / _sampleRate;
            phases[binIdx] = phasesRst[binIdx] = sU16(phase * 65536);
            mags[binIdx] = mag;
            speeds[binIdx] = sU16(w * 65536);

            if(mag > maxMag)
            {
               maxBin = binIdx;
               maxMag = mag;
               maxFreq = freq;
            }
         }

         if(maxBin < 1)
            return;

         Dyac_host_printf("xxx ifftSin: maxFreq=%f\n", maxFreq);

         sF32 loopSize = sF32(_sampleRate / maxFreq);
         while(loopSize < 4096)
            loopSize *= 2;
         sF32 loopFrameIdx = 0.0f;

         for(sUI frameIdx = 0u; frameIdx < _numFrames; frameIdx++)
         {
            sF32 smp = 0.0f;

            // for(binIdx = 20u; binIdx < 24; binIdx++)
            for(binIdx = 0u; binIdx < numBins; binIdx++)
            {
               // smp += (sF32) (mags[binIdx] * sin(phases[binIdx]));
               smp += (sF32) (mags[binIdx] * sin_tab[phases[binIdx]]);
               phases[binIdx] += speeds[binIdx];
            }

            d[frameIdx] = smp;

            loopFrameIdx += 1.0f;
            if(loopFrameIdx >= loopSize)
            {
               loopFrameIdx -= loopSize;
               memcpy((void*)phases, (void*)phasesRst, sizeof(sU16) * numBins);
            }

         } // loop frames

         // // fad->num_elements = _numFrames;

         delete [] phasesRst;
         delete [] phases;
         delete [] mags;
         delete [] speeds;
      }
   }
}

void StFFT::_testIFFTSin_ModLFOSin(YAC_Object *_fad, sUI _numFrames, sF32 _sampleRate, sF32 _pitchScl, sF32 _amp, sF32 _freqRandExp, sF32 _freqRandScl, sF32 _lfoSpeed) {
   // works but sounds like a badly looped sample (neighbouring bins cause phase cancelation??)
   //  even though the phases are free-running, the sample loops at the fft block size
   if(YAC_Is_FloatArray(_fad))
   {
      YAC_CAST_ARG(YAC_FloatArray, fad, _fad);

      if(_numFrames <= fad->max_elements)
      {
         sF32 *d = fad->elements;

         sF64 scl = _amp * 2.0 / window_size;

         sUI numBins = sUI(window_size / 2);
         // numBins /= 4;

         // sF64 *phases = new sF64[window_size];
         sU16 *phases = new sU16[numBins];
         sF64 *mags   = new sF64[numBins];
         // sF64 *speeds = new sF64[window_size];
         sU16 *speeds = new sU16[numBins];
         sU16 *lfo_speeds = new sU16[numBins];
         sU16 *lfo_phases = new sU16[numBins];
         sU16 *lfo_amt = new sU16[numBins];
         sUI binIdx;

         sF32 fMinBin = logf(2.0f + 2.0f*test_ifft_minbin) / logf(2.0f) - 1.0f;
         sF32 fMaxBin = logf(2.0f + 2.0f*test_ifft_maxbin) / logf(2.0f) - 1.0f;
         sUI minBin = sUI(fMinBin * numBins);
         sUI maxBin = sUI(fMaxBin * numBins);

         // Calc start phases and magnitudes
         for(binIdx = minBin; binIdx < maxBin; binIdx++)
         {
            sF64 fr    = fft_real[rev_tab[binIdx]];
            sF64 fi    = fft_img[rev_tab[binIdx]];
            sF64 phase;
#if 0
            phase = atan2(fi, fr); // => -PI..PI
            phase /= (sM_PI * 2.0);  // => -0.5..0.5
            if(phase < 0.0)
               phase += 1.0;
#else
            phase = randf(1.0f);
#endif

            sF64 mag   = scl * sqrt(fr*fr + fi*fi);
            // (todo) remove _sampleRate (assume that src sample rate is same as out sample rate)
            sF64 freq = (binIdx * _sampleRate) / window_size;

#if 0
            {
               // freq randomization to avoid "beating" sampleloop effect
               // sF32 randMax = (sF32(binIdx)/numBins) * 62.0f;
               sF32 randMax = powf(2.0f, _freqRandExp * (sF32(1.0f + binIdx)/numBins)) * _freqRandScl;
               freq += randf( randMax * 2.0f ) - randMax;
            }
#endif

            lfo_speeds[binIdx] = (sU16)randf((_lfoSpeed * 65536) / _sampleRate);
            lfo_phases[binIdx] = (sU16)randf(65535);
            sF32 randMax = powf(2.0f, _freqRandExp * (sF32(1.0f + binIdx)/numBins)) * _freqRandScl;
            lfo_amt[binIdx] = sU16(randf(randMax)) + sU16(randf(1.0f));

            // sF64 w = (freq * (sM_PI * 2.0)) / _sampleRate;
            sF64 w = (freq * _pitchScl) / _sampleRate;
            phases[binIdx] = sU16(phase * 65536);
            mags[binIdx] = mag;
            speeds[binIdx] = sU16(w * 65536);
            Dyac_host_printf("xxx ifftsin: bin[%4u] freq=%f w=%f speed.16=%u mag=%f\n", binIdx, freq, w, speeds[binIdx], mag);
         }


         // const sF32 *ctab = &sin_tab[16384];
         const sF32 *stab = &sin_tab[0];

         for(sUI frameIdx = 0u; frameIdx < _numFrames; frameIdx++)
         {
            sF64 smp = 0.0f;

            // for(binIdx = 10u; binIdx < 16; binIdx++)
            // for(binIdx = 24u; binIdx < 155u; binIdx++)
            for(binIdx = minBin; binIdx < maxBin; binIdx++)
            {
               smp += (mags[binIdx] * stab[phases[binIdx]]);
               // smp += (mags[binIdx] * stab[phases[binIdx] + sU16(2048+sS16(stab[lfo_phases[binIdx]]*2048)) ]);

               // phases[binIdx] += speeds[binIdx];
               // phases[binIdx] = (sU32(phases[binIdx]) + sU32(speeds[binIdx])) & 65535u;
               phases[binIdx] = (sU32(phases[binIdx]) + sU32(speeds[binIdx] + sU16(sS16(stab[lfo_phases[binIdx]]*lfo_amt[binIdx])) )) & 65535u;


               // speeds[binIdx] = speeds[binIdx] + stab[lfo_phases[binIdx]] * 0.02f;
               lfo_phases[binIdx] += lfo_speeds[binIdx];
            }

            d[frameIdx] = sF32(smp);
         } // loop frames

         // // fad->num_elements = _numFrames;

         delete [] lfo_speeds;
         delete [] lfo_phases;
         delete [] lfo_amt;
         delete [] phases;
         delete [] mags;
         delete [] speeds;
      } // if fits dest array
   } // if is floatarray
}


void StFFT::_testIFFTSin_TrueFreq(YAC_Object *_fad, sSI _frameOffset, sUI _numFrames, sF32 _sampleRate, sF32 _pitchScl, sF32 _freqAdd, sF32 _amp, sF32 _freqRandExp, sF32 _freqRandScl, YAC_Object *_fftO, sUI _fftOFrameShift, sF32 _phaseRandAmt, YAC_Object *_stfftBandParamsArray) {
   // (note) e.g. 64 frames later means that @44.1kHz we can measure phase difference up to 689Hz
   // (note) the most problematic frequencies are below ~200Hz
   if(YAC_Is_FloatArray(_fad))
   {
      YAC_CAST_ARG(YAC_FloatArray, fad, _fad);
      YAC_CAST_ARG(StFFT, fftO, _fftO);

      sUI skipFrames;
      if(_frameOffset < 0)
      {
         skipFrames = sUI(-_frameOffset);
         if(skipFrames >= _numFrames)
            return;
         _frameOffset = 0u;
         _numFrames -= skipFrames;
      }
      else
      {
         skipFrames = 0u;
      }

      if(((sUI(_frameOffset) + _numFrames) <= fad->max_elements) && (_fftOFrameShift >= 1u))
      {
         sF32 *d = fad->elements + sUI(_frameOffset);

         sF64 scl = _amp * 2.0 / window_size;

         sUI numBins = sUI(window_size / 2);

         sU16 *phases = new sU16[numBins];
         sF64 *mags   = new sF64[numBins];
         sU16 *speeds = new sU16[numBins];
         sUI binIdx;

         sF32 fMinBin = logf(2.0f + 2.0f*test_ifft_minbin) / logf(2.0f) - 1.0f;
         sF32 fMaxBin = logf(2.0f + 2.0f*test_ifft_maxbin) / logf(2.0f) - 1.0f;
         sUI minBin = sUI(fMinBin * numBins);
         sUI maxBin = sUI(fMaxBin * numBins);

         sF32 maxFreq = _sampleRate / _fftOFrameShift;

         sUI numBandParams = 0u;
         if(YAC_VALID(_stfftBandParamsArray))
         {
            numBandParams = _stfftBandParamsArray->yacArrayGetNumElements();
         }

         // Calc start phases and magnitudes
         for(binIdx = minBin; binIdx < maxBin; binIdx++)
         {
            sF64 fr = fft_real[rev_tab[binIdx]];
            sF64 fi = fft_img[rev_tab[binIdx]];

            sF64 frO = fftO->fft_real[rev_tab[binIdx]];
            sF64 fiO = fftO->fft_img[rev_tab[binIdx]];

            sF64 mag   = scl * sqrt(fr*fr + fi*fi);

            // (todo) remove _sampleRate (assume that src sample rate is same as out sample rate)
            sF64 freq = (binIdx * _sampleRate) / window_size;
            sF64 wBin = freq / _sampleRate;

            sF64 phase;

            phase = atan2(fi, fr); // => -PI..PI
            phase /= (sM_PI * 2.0);  // => -0.5..0.5

            sF64 phaseO;
            sF64 phaseDelta;
            sF64 freqDelta;
            sF64 phaseIdeal;

            if((binIdx > 0u) && (freq < maxFreq))
            {
               phaseIdeal = phase + wBin * _fftOFrameShift;

               phaseO = atan2(fiO, frO); // => -PI..PI
               phaseO /= (sM_PI * 2.0);  // => -0.5..0.5
               // if(phaseO < 0.0)
               //    phaseO += 1.0;

               sF64 intPart;
               phaseDelta = (phaseO - modf(phaseIdeal, &intPart));
               if(phaseDelta < -0.5)
                  phaseDelta += 1.0;
               else if(phaseDelta > 0.5)
                  phaseDelta -= 1.0;

               freqDelta = phaseDelta * freq;//(_sampleRate / window_size);
               freq = freq + freqDelta;
            }
            else
            {
               phaseO = 0.0;
               phaseDelta = 0.0;
               freqDelta = 0.0;
               phaseIdeal = 0.0;
            }

            if(phase < 0.0)
               phase += 1.0;

            if(binIdx > 0)  // don't randomize DC
            {
               if(_phaseRandAmt > 0.0f)
                  phase += randf(_phaseRandAmt);

#if 1
               // freq randomization to avoid "beating" sampleloop effect
               // sF32 randMax = (sF32(binIdx)/numBins) * 62.0f;
               sF32 randMax = powf(2.0f, _freqRandExp * (sF32(1.0f + binIdx)/numBins)) * _freqRandScl;
               freq += randf( randMax * 2.0f ) - randMax;
#endif
            }

            // sF64 w = (freq * (sM_PI * 2.0)) / _sampleRate;
            if(binIdx > 0)
            {
               freq = freq * _pitchScl + _freqAdd;
            }
            sF64 w = freq / _sampleRate;

            phases[binIdx] = sU16(phase * 65536);
            speeds[binIdx] = sU16(w * 65536);

            ifftApplyBandParams(mag, sF32(freq), _pitchScl, _freqAdd, _stfftBandParamsArray, numBandParams);

            if((0 == binIdx) || ((freq > 0.0f) && (freq <= _sampleRate*0.5)))
            {
               mags[binIdx] = mag;
            }
            else
            {
               mags[binIdx] = 0;
            }

            // Dprintf("xxx ifftsin: bin[%4u] freq=%f w=%f speed.16=%u mag=%f\n", binIdx, freq, w, speeds[binIdx], mag);
            Dprintf("xxx ifftsin: bin[%4u] freq=%f p=%f pI=%f pO=%f pDt=%f fDt=%f mag=%f\n", binIdx, freq, phase, phaseIdeal, phaseO, phaseDelta, freqDelta, mag);
         }


#ifdef PRINT_RESYNTH_MILLISEC
         sUI ms = Dyac_host_yacMilliSeconds();
#endif // PRINT_RESYNTH_MILLISEC


         // 495904 * 4096 = 2031222784 calc
         // (495904 * 4096) / 2966  = 684835.732974 calc/ms
         // (3400 * 1000) = 3400000  cycles/ms @3.4GHz
         // (3400 * 1000) / ((495904 * 4096) / 2966) = 4.96 cycles/calc

         // const sF32 *ctab = &sin_tab[16384];
         const sF32 *stab = &sin_tab[0];

         if(skipFrames > 0u)
         {
            for(binIdx = minBin; binIdx < maxBin; binIdx++)
            {
               if(mags[binIdx] > 0)
               {
                  phases[binIdx] = sU16(phases[binIdx] + skipFrames * speeds[binIdx]);
               }
            }
         }

         for(sUI frameIdx = 0u; frameIdx < _numFrames; frameIdx++)
         {
            sF64 smp = 0.0f;

            for(binIdx = minBin; binIdx < maxBin; binIdx++)
            {
               if(mags[binIdx] > 0)
               {
                  smp += (mags[binIdx] * stab[phases[binIdx]]);
                  phases[binIdx] += speeds[binIdx];
               }
            }

            d[frameIdx] += sF32(smp);
         } // loop frames

#ifdef PRINT_RESYNTH_MILLISEC
         ms = Dyac_host_yacMilliSeconds() - ms;
         sF32 realtimePct = 0.0f;
         if(ms > 0)
         {
            sF32 numFramesPerSec = sF32(1000.0f * _numFrames) / ms;
            realtimePct = numFramesPerSec / _sampleRate;
         }
         Dyac_host_printf("xxx StFFFT::testIFFTSin_TrueFreq: %u frames took %u milliseconds (%f x realtime => %f%% DSP load)\n", _numFrames, ms, realtimePct, (realtimePct > 0.0f) ? (100.0f / realtimePct) : 0.0f);
#endif // PRINT_RESYNTH_MILLISEC

         // // fad->num_elements = _numFrames;

         delete [] phases;
         delete [] mags;
         delete [] speeds;
      } // if fits dest array
   } // if is floatarray
}

void StFFT::ifftApplyBandParams(sF64 &mag, const sF32 freq, const sF32 _pitchScl, const sF32 _freqAdd, YAC_Object *_stfftBandParamsArray, const sUI _numBandParams) {
   for(sUI bandIdx = 0u; bandIdx < _numBandParams; bandIdx++)
   {
      YAC_Value v;
      _stfftBandParamsArray->yacArrayGet(NULL/*context*/, bandIdx, &v);
      if(YAC_TYPE_OBJECT == v.type)
      {
         YAC_CAST_ARG(StFFT_BandParams, bandParams, v.value.object_val);
         if(NULL != bandParams)
         {
            if(bandParams->b_enable)
            {
               sF32 baseBandFreq     = bandParams->band_freq * _pitchScl + _freqAdd;
               sF32 baseBandFreqUp   = bandParams->band_freq_up * _pitchScl + _freqAdd;
               sF32 baseBandFreqDown = bandParams->band_freq_down * _pitchScl + _freqAdd;

               for(sUI harmonicIdx = 1u; harmonicIdx <= sUI(bandParams->harmonics); harmonicIdx++)
               {
                  sF32 bandFreq = baseBandFreq * harmonicIdx;
                  sF32 bandFreqUp = baseBandFreqUp * harmonicIdx;
                  sF32 bandFreqDown = baseBandFreqDown * harmonicIdx;

                  if((bandFreq != bandFreqUp) && (bandFreq != bandFreqDown))
                  {
                     sF32 bandDist = -1.0f;
                     if((freq >= bandFreqDown) && (freq < bandFreq))
                     {
                        bandDist = bandFreq - sF32(freq);
                        bandDist /= (bandFreq - bandFreqDown);  // => 0(near)..1(far)
                     }
                     else if((freq >= bandFreq) && (freq <= bandFreqUp))
                     {
                        bandDist = sF32(freq) - bandFreq;
                        bandDist /= (bandFreqUp - bandFreq);  // => 0(near)..1(far)
                     }

                     if(bandDist >= 0.0f)
                     {
                        sF32 bandSclLin = 1.0f - bandDist;      // => 1(near)..0(far)
                        sF32 bandSclQuad = bandSclLin * bandSclLin;
                        bandSclLin = 1.0f + (bandSclLin - 1.0f) * bandParams->mag_dcy_lin;
                        bandSclQuad = 1.0f + (bandSclQuad - 1.0f) * bandParams->mag_dcy_quad;
                        // mag *= bandSclLin * bandSclQuad * bandParams->mag_scl;
                        sF32 bandScl = bandSclLin * bandSclQuad;
                        if(bandParams->mag_scl >= 0.0f)
                        {
                           // Center has maximum attenuation, falls off to the sides
                           bandScl = 1.0f + (bandParams->mag_scl - 1.0f) * bandScl;
                        }
                        else
                        {
                           // Center has full level, lerped to mag_scl on the sides
                           bandScl = 1.0f + (-bandParams->mag_scl - 1.0f) * (1.0f - bandScl);
                        }
                        // Dyac_host_printf("xxx band[%u] dwn=%f ctr=%f up=%f freq=%f bandScl=%f\n", bandIdx, bandFreqDown, bandFreq, bandFreqUp, freq, bandScl);
                        mag *= bandScl;
                     }
                  }
               } // loop harmonics
            } // if enable
         } // if bandParams
      }
   }
}
