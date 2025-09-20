// ----
// ---- file   : sample_utils.cpp
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2009-2025 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   :
// ----
// ---- created: 25Oct2009
// ---- changed: 29Apr2010, 11May2010, 25Feb2011, 04May2013, 08May2013, 12Feb2014, 07Aug2016
// ----          23Feb2018, 24Feb2018, 30Mar2018, 09Apr2018, 22Jun2018, 21Jul2018, 18Dec2018
// ----          22Dec2018, 04May2019, 09May2019, 14Feb2020, 15May2020, 06Feb2021, 22Dec2021
// ----          10Jan2022, 13May2022, 14May2022, 21Sep2024, 10Jan2025
// ----
// ----
// ----

#define Dprintf if(1);else yac_host->printf
// #define Dprintf if(0);else yac_host->printf

#include "tksampleedit.h"


PeakAvgTracker::PeakAvgTracker(void) {
   peak_timeout = 512;
   peak_decay   = 0.99f;
   reset();
}

PeakAvgTracker::~PeakAvgTracker() {
}

void PeakAvgTracker::reset(void) {
   peak_count = 0;
   peak = 0.0f;
   avg = 0.0f;
}

void PeakAvgTracker::process(sF32 _curPeak) {
   if(_curPeak > peak)
   {
      peak = _curPeak;
      peak_count = peak_timeout;
   }
   else
   {
      if(--peak_count <= 0)
      {
         peak_count = 0;
         peak *= peak_decay;
         if(peak < 0.001f)
            peak = 0.0f;
      }
   }

   avg += (_curPeak - avg) * 0.05f;
   // avg = _curPeak;
}

sF32 PeakAvgTracker::getPeak(void) {
   return peak;
}

sF32 PeakAvgTracker::getAvg(void) {
   return avg;
}


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
struct tksampleedit_Biquad2 {
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
   }

   sF64 a0, a1, a2, b1, b2;

   void calcParams(sSI _type,
                   sF32 _dbGain,
                   sF32 _freq,   // 0..1
                   sF32 _res     // 0..1
                   ) {
#define Q _res
#define Fc _freq

      // (note) sound dies after a short while with fmax=1.0
      // // _freq = sMIN(0.992f, sMAX(_freq, 0.001f));
      _freq = sMIN(0.9999f, sMAX(_freq, 0.001f));

      // (note) sound becomes very quiet with res < 0.5
      _res += 0.5f;
      if(_res < 0.5f)
         _res = 0.5f;
      else if(_res > 1.5f)
         _res = 1.5f;

      sF64 norm;
      sF64 V = pow(10, fabs(_dbGain) / 20.0);  // PEQ, LSH, HSH
      sF64 K = tan(sM_PI * Fc);
      switch(_type)
      {
         case LPF:
            norm = 1 / (1 + K / Q + K * K);
            a0 = K * K * norm;
            a1 = 2 * a0;
            a2 = a0;
            b1 = 2 * (K * K - 1) * norm;
            b2 = (1 - K / Q + K * K) * norm;
            break;

         case HPF:
            norm = 1 / (1 + K / Q + K * K);
            a0 = 1 * norm;
            a1 = -2 * a0;
            a2 = a0;
            b1 = 2 * (K * K - 1) * norm;
            b2 = (1 - K / Q + K * K) * norm;
#if 0
            {
               static int xxx = 0;
               if(0 == (++xxx & 511))
               {
                  yac_host->printf("xxx hpf a0=%f a1=%f b1=%f b2=%f\n", a0, a1, b1, b2);
               }
            }
#endif
            break;

         case BPF:
            norm = 1 / (1 + K / Q + K * K);
            a0 = K / Q * norm;
            a1 = 0;
            a2 = -a0;
            b1 = 2 * (K * K - 1) * norm;
            b2 = (1 - K / Q + K * K) * norm;
            break;

         case BRF:
            norm = 1 / (1 + K / Q + K * K);
            a0 = (1 + K * K) * norm;
            a1 = 2 * (K * K - 1) * norm;
            a2 = a0;
            b1 = a1;
            b2 = (1 - K / Q + K * K) * norm;
            break;

         case PEQ:
            if(_dbGain >= 0)
            {
               // boost
               norm = 1 / (1 + 1/Q * K + K * K);
               a0 = (1 + V/Q * K + K * K) * norm;
               a1 = 2 * (K * K - 1) * norm;
               a2 = (1 - V/Q * K + K * K) * norm;
               b1 = a1;
               b2 = (1 - 1/Q * K + K * K) * norm;
            }
            else
            {
               // cut
               norm = 1 / (1 + V/Q * K + K * K);
               a0 = (1 + 1/Q * K + K * K) * norm;
               a1 = 2 * (K * K - 1) * norm;
               a2 = (1 - 1/Q * K + K * K) * norm;
               b1 = a1;
               b2 = (1 - V/Q * K + K * K) * norm;
            }
            break;

         case LSH:
            if(_dbGain >= 0)
            {
               // boost
               norm = 1 / (1 + sqrt(2) * K + K * K);
               a0 = (1 + sqrt(2*V) * K + V * K * K) * norm;
               a1 = 2 * (V * K * K - 1) * norm;
               a2 = (1 - sqrt(2*V) * K + V * K * K) * norm;
               b1 = 2 * (K * K - 1) * norm;
               b2 = (1 - sqrt(2) * K + K * K) * norm;
            }
            else
            {
               // cut
               norm = 1 / (1 + sqrt(2*V) * K + V * K * K);
               a0 = (1 + sqrt(2) * K + K * K) * norm;
               a1 = 2 * (K * K - 1) * norm;
               a2 = (1 - sqrt(2) * K + K * K) * norm;
               b1 = 2 * (V * K * K - 1) * norm;
               b2 = (1 - sqrt(2*V) * K + V * K * K) * norm;
            }
            break;

         case HSH:
            if(_dbGain >= 0)
            {
               // boost
               norm = 1 / (1 + sqrt(2) * K + K * K);
               a0 = (V + sqrt(2*V) * K + K * K) * norm;
               a1 = 2 * (K * K - V) * norm;
               a2 = (V - sqrt(2*V) * K + K * K) * norm;
               b1 = 2 * (K * K - 1) * norm;
               b2 = (1 - sqrt(2) * K + K * K) * norm;
            }
            else
            {
               // cut
               norm = 1 / (V + sqrt(2*V) * K + K * K);
               a0 = (1 + sqrt(2) * K + K * K) * norm;
               a1 = 2 * (K * K - 1) * norm;
               a2 = (1 - sqrt(2) * K + K * K) * norm;
               b1 = 2 * (K * K - V) * norm;
               b2 = (V - sqrt(2*V) * K + K * K) * norm;
            }
            break;
      }
#undef Q
#undef Fc
   }

   sF32 filter(const sF32 _inSmp) {
      sF64 out = _inSmp * a0 + z1;
      z1 = _inSmp * a1 + z2 - b1 * out;
      z2 = _inSmp * a2 - b2 * out;

      sF32 out32 = sF32(out);
      return Dyac_denorm_32(out32);
   }
};


void tksampleedit_ringbuf_copy_samples(YAC_Object *_dst, sUI _dstNumCh, sUI _dstChOff, sUI _dstFrameOff,
                                       sUI _numFrames, sUI _numCh,
                                       YAC_Object *_src, sUI _srcNumCh, sUI _srcChOff, sUI _srcFrameOff
                                       ) {
   if(YAC_Is_FloatArray(_dst))
   {
      if(YAC_Is_FloatArray(_src))
      {
         YAC_CAST_ARG(YAC_FloatArray, src, _src);
         YAC_CAST_ARG(YAC_FloatArray, dst, _dst);

         sF32 *d = dst->elements + _dstChOff;
         sF32 *s = src->elements + _srcChOff;

         sUI srcSmpOff = _srcFrameOff * _srcNumCh;
         sUI dstSmpOff = _dstFrameOff * _dstNumCh;
         sUI i;

         for(i=0; i<_numFrames; i++)
         {
            if(dstSmpOff >= dst->num_elements)
            {
               dstSmpOff -= dst->num_elements;
            }

            if(srcSmpOff >= src->num_elements)
            {
               srcSmpOff -= src->num_elements;
            }

            for(sUI chIdx = 0; chIdx < _numCh; chIdx++)
            {
               d[dstSmpOff+chIdx] = s[srcSmpOff+chIdx];
            }

            dstSmpOff += _dstNumCh;
            srcSmpOff += _srcNumCh;
         }
      }
   }
}


void tksampleedit_copy_looped_samples_mono(YAC_Object *_dst, sSI _dstFrameOff,
                                           YAC_Object *_src, sSI _srcFrameOff,
                                           sSI _loopStart, sSI _loopEnd,
                                           sUI _numFrames
                                           ) {
   if(YAC_Is_FloatArray(_dst))
   {
      if(YAC_Is_FloatArray(_src))
      {
         YAC_CAST_ARG(YAC_FloatArray, src, _src);
         YAC_CAST_ARG(YAC_FloatArray, dst, _dst);

         if(dst->max_elements >= (sUI(_dstFrameOff) + _numFrames))
         {
            sF32 *d = dst->elements;
            sF32 *s = src->elements;

            // printf("xxx srcFrameOffStart=%d loop start=%d end=%d\n", _srcFrameOff, _loopStart, _loopEnd);

            for(sUI i = 0; (i < _numFrames); i++)
            {
               sSI srcIdxW;

               if(_srcFrameOff < _loopStart)
               {
                  srcIdxW = (_srcFrameOff - _loopStart) + _loopEnd;
               }
               else if(_srcFrameOff >= _loopEnd)
               {
                  srcIdxW = (_srcFrameOff - _loopEnd) + _loopStart;
               }
               else
               {
                  srcIdxW = _srcFrameOff;
               }

               if((srcIdxW >= 0) && (sUI(srcIdxW) < src->max_elements))
               {
                  d[_dstFrameOff] = s[srcIdxW];
               }
               else
               {
                  d[_dstFrameOff] = 0.0f;
               }

               // Next sample frame
               _srcFrameOff++;
               _dstFrameOff++;
            }
         }
      }
   }
}


sF32 YAC_CALL tksampleedit_buf_get_max(YAC_Object *_src, sUI _numFrames, sUI _numCh, sUI _chOff) {
   sF32 r = 0.0f;

   if(YAC_Is_FloatArray(_src))
   {
      YAC_CAST_ARG(YAC_FloatArray, src, _src);

      sF32 *s = src->elements;

      sUI i;
      sUI off =  _chOff;
      for(i=0; i<_numFrames; i++)
      {
         sF32 f = s[off];

         if(f < 0)
            f = -f;

         if(f > r)
            r = f;

         off += _numCh;
      }
   }

   return r;
}

void YAC_CALL tksampleedit_mix_stereo_input_buf(YAC_Object *_faOutput,
                                                YAC_Object *_faInput,
                                                YAC_Object *_faMixerLevels,
                                                sUI _numInputChannels,
                                                sUI _numOutputChannels,
                                                sUI _numFrames,
                                                sUI _outChOff,
                                                sUI _inChOff
                                                ) {
   if(_numInputChannels > 0)
   {
      if(YAC_Is_FloatArray(_faOutput))
      {
         YAC_CAST_ARG(YAC_FloatArray, output, _faOutput);

         if(YAC_Is_FloatArray(_faInput))
         {
            YAC_CAST_ARG(YAC_FloatArray, input, _faInput);

            if(YAC_Is_FloatArray(_faMixerLevels))
            {
               YAC_CAST_ARG(YAC_FloatArray, mixerLevels, _faMixerLevels);

               sUI ooff = _outChOff;
               sUI ioff = _inChOff;

               for(sUI frameNr=0; frameNr<_numFrames; frameNr++)
               {
                  sF32 l = input->elements[ioff + 0] * mixerLevels->elements[_inChOff + 0];
                  sF32 r = input->elements[ioff + 1] * mixerLevels->elements[_inChOff + 1];

                  output->elements[ooff + 0] = l;
                  output->elements[ooff + 1] = r;

                  // Next frame
                  ooff += _numOutputChannels;
                  ioff += _numInputChannels;
               }
            }
         }
      }
   }
}


void YAC_CALL tksampleedit_mixclip_stereo_mixbuf(YAC_Object *_faOutput,
                                                 YAC_Object *_faMixBuf,
                                                 sF32 _levelL,
                                                 sF32 _levelR,
                                                 sF32 _clipLevel,
                                                 sUI _numOutputChannels,
                                                 sUI _numFrames,
                                                 sUI _outChOff
                                                 ) {

   if(YAC_Is_FloatArray(_faOutput))
   {
      YAC_CAST_ARG(YAC_FloatArray, output, _faOutput);

      if(YAC_Is_FloatArray(_faMixBuf))
      {
         YAC_CAST_ARG(YAC_FloatArray, mixBuf, _faMixBuf);

         sUI inOff = 0;

         for(sUI frameNr=0; frameNr<_numFrames; frameNr++)
         {
            sF32 l = output->elements[_outChOff + 0] + mixBuf->elements[inOff + 0] * _levelL;
            if(l < -_clipLevel)
            {
               l = -_clipLevel;
            }
            else if(l > _clipLevel)
            {
               l = _clipLevel;
            }
            output->elements[_outChOff + 0] = l;


            sF32 r = output->elements[_outChOff + 1] + mixBuf->elements[inOff + 1] * _levelR;
            if(r < -_clipLevel)
            {
               r = -_clipLevel;
            }
            else if(r > _clipLevel)
            {
               r = _clipLevel;
            }
            output->elements[_outChOff + 1] = r;

            // Next frame
            _outChOff += _numOutputChannels;
            inOff += 2;
         }

      }
   }
}


void YAC_CALL tksampleedit_fill_mono(YAC_Object *_faOut, sUI _outOff, sF32 _val, sUI _numFrames) {
   sF32 *faOut = (sF32*)_faOut->yacArrayGetPointer();

   for(sUI i = 0u; i < _numFrames; i++)
   {
      faOut[_outOff++] = _val;
   }
}


void YAC_CALL tksampleedit_copy_mono_to_mono(YAC_Object *_faOut, sUI _outOff, YAC_Object *_faIn, sUI _inOff, sUI _numFrames) {
   sF32 *faOut = (sF32*)_faOut->yacArrayGetPointer();
   const sF32 *faIn = (const sF32*)_faIn->yacArrayGetPointer();

   for(sUI i = 0u; i < _numFrames; i++)
   {
      faOut[_outOff++] = faIn[_inOff++];
   }
}

void YAC_CALL tksampleedit_amp_mono_to_mono(YAC_Object *_faOut, sUI _outOff, YAC_Object *_faIn, sUI _inOff, sUI _numFrames, sF32 _amp) {
   sF32 *faOut = (sF32*)_faOut->yacArrayGetPointer();
   const sF32 *faIn = (const sF32*)_faIn->yacArrayGetPointer();

   for(sUI i = 0u; i < _numFrames; i++)
   {
      faOut[_outOff++] = faIn[_inOff++] * _amp;
   }
}

void YAC_CALL tksampleedit_copy_mono_adv_to_mono(YAC_Object *_faOut, sUI _outOff, YAC_Object *_faIn, sUI _inOff, sUI _inAdv, sUI _numFrames) {
   sF32 *faOut = (sF32*)_faOut->yacArrayGetPointer();
   const sF32 *faIn = (const sF32*)_faIn->yacArrayGetPointer();

   for(sUI i = 0u; i < _numFrames; i++)
   {
      faOut[_outOff++] = faIn[_inOff];
      _inOff += _inAdv;
   }
}

void YAC_CALL tksampleedit_copy_mono_adv_to_mono_adv(YAC_Object *_faOut, sUI _outOff, sUI _outAdv, YAC_Object *_faIn, sUI _inOff, sUI _inAdv, sUI _numFrames) {
   sF32 *faOut = (sF32*)_faOut->yacArrayGetPointer();
   const sF32 *faIn = (const sF32*)_faIn->yacArrayGetPointer();

   for(sUI i = 0u; i < _numFrames; i++)
   {
      faOut[_outOff] = faIn[_inOff];
      _outOff += _outAdv;
      _inOff  += _inAdv;
   }
}

void YAC_CALL tksampleedit_copy_mono_adv_to_mono_adv_ring(YAC_Object *_faOut, sUI _outOff, sUI _outAdv, sUI _outRingSize, YAC_Object *_faIn, sUI _inOff, sUI _inAdv, sUI _numFrames) {
   sF32 *faOut = (sF32*)_faOut->yacArrayGetPointer();
   const sF32 *faIn = (const sF32*)_faIn->yacArrayGetPointer();

   for(sUI i = 0u; i < _numFrames; i++)
   {
      _outOff = _outOff % _outRingSize;
      faOut[_outOff] = faIn[_inOff];
      _outOff += _outAdv;
      _inOff  += _inAdv;
   }
}

void YAC_CALL tksampleedit_copy_mono_adv_ring_to_mono_adv(YAC_Object *_faOut, sUI _outOff, sUI _outAdv, YAC_Object *_faIn, sUI _inOff, sUI _inAdv, sUI _inRingSize, sUI _numFrames) {
   sF32 *faOut = (sF32*)_faOut->yacArrayGetPointer();
   const sF32 *faIn = (const sF32*)_faIn->yacArrayGetPointer();

   for(sUI i = 0u; i < _numFrames; i++)
   {
      _inOff = _inOff % _inRingSize;
      faOut[_outOff] = faIn[_inOff];
      _outOff += _outAdv;
      _inOff  += _inAdv;
   }
}

void YAC_CALL tksampleedit_scale_mono_adv_to_mono_adv(YAC_Object *_faOut, sUI _outOff, sUI _outAdv, YAC_Object *_faIn, sUI _inOff, sUI _inAdv, sF32 _scale, sUI _numFrames) {
   sF32 *faOut = (sF32*)_faOut->yacArrayGetPointer();
   const sF32 *faIn = (const sF32*)_faIn->yacArrayGetPointer();

   for(sUI i = 0u; i < _numFrames; i++)
   {
      faOut[_outOff] = faIn[_inOff] * _scale;
      _outOff += _outAdv;
      _inOff  += _inAdv;
   }
}

void YAC_CALL tksampleedit_copy_mono_to_dualmono(YAC_Object *_faOutL, YAC_Object *_faOutR, sUI _outOff, YAC_Object *_faIn, sUI _inOff, sUI _numFrames) {
   sF32 *faOutL = (sF32*)_faOutL->yacArrayGetPointer();
   sF32 *faOutR = (sF32*)_faOutR->yacArrayGetPointer();
   const sF32 *faIn  = (const sF32*)_faIn->yacArrayGetPointer();

   for(sUI i = 0u; i < _numFrames; i++)
   {
      faOutL[_outOff] = faIn[_inOff];
      faOutR[_outOff] = faIn[_inOff++];
      _outOff++;
   }
}

void YAC_CALL tksampleedit_copy_mono_adv_to_dualmono(YAC_Object *_faOutL, YAC_Object *_faOutR, sUI _outOff, YAC_Object *_faIn, sUI _inOff, sUI _inAdv, sUI _numFrames) {
   sF32 *faOutL = (sF32*)_faOutL->yacArrayGetPointer();
   sF32 *faOutR = (sF32*)_faOutR->yacArrayGetPointer();
   const sF32 *faIn  = (const sF32*)_faIn->yacArrayGetPointer();

   for(sUI i = 0u; i < _numFrames; i++)
   {
      faOutL[_outOff] = faIn[_inOff];
      faOutR[_outOff] = faIn[_inOff];
      _inOff += _inAdv;
      _outOff++;
   }
}

void YAC_CALL tksampleedit_copy_mono_to_stereo(YAC_Object *_faOut, sUI _outOff, YAC_Object *_faIn, sUI _inOff, sUI _numFrames) {
   sF32 *faOut = (sF32*)_faOut->yacArrayGetPointer();
   const sF32 *faIn = (const sF32*)_faIn->yacArrayGetPointer();

   for(sUI i = 0u; i < _numFrames; i++)
   {
      faOut[_outOff++] = faIn[_inOff];
      faOut[_outOff++] = faIn[_inOff++];
   }
}

void YAC_CALL tksampleedit_copy_pan_mono_to_stereo(YAC_Object *_faOut, sUI _outOff, YAC_Object *_faIn, sUI _inOff, sF32 _lvlL, sF32 _lvlR, sUI _numFrames) {
   sF32 *faOut = (sF32*)_faOut->yacArrayGetPointer();
   const sF32 *faIn = (const sF32*)_faIn->yacArrayGetPointer();

   for(sUI i = 0u; i < _numFrames; i++)
   {
      faOut[_outOff++] = faIn[_inOff] * _lvlL;
      faOut[_outOff++] = faIn[_inOff++] * _lvlR;
   }
}

void YAC_CALL tksampleedit_copy_dualmono_to_dualmono(YAC_Object *_faOutL, YAC_Object *_faOutR, sUI _outOff, YAC_Object *_faInL, YAC_Object *_faInR, sUI _inOff, sUI _numFrames) {
   sF32 *faOutL = (sF32*)_faOutL->yacArrayGetPointer();
   sF32 *faOutR = (sF32*)_faOutR->yacArrayGetPointer();
   const sF32 *faInL = (const sF32*)_faInL->yacArrayGetPointer();
   const sF32 *faInR = (const sF32*)_faInR->yacArrayGetPointer();

   for(sUI i = 0u; i < _numFrames; i++)
   {
      faOutL[_outOff]   = faInL[_inOff];
      faOutR[_outOff++] = faInR[_inOff++];
   }
}

void YAC_CALL tksampleedit_copy_dualmono_to_stereo(YAC_Object *_faOut, sUI _outOff, YAC_Object *_faInL, YAC_Object *_faInR, sUI _inOff, sUI _numFrames) {
   sF32 *faOut = (sF32*)_faOut->yacArrayGetPointer();
   const sF32 *faInL = (const sF32*)_faInL->yacArrayGetPointer();
   const sF32 *faInR = (const sF32*)_faInR->yacArrayGetPointer();

   for(sUI i = 0u; i < _numFrames; i++)
   {
      faOut[_outOff++] = faInL[_inOff];
      faOut[_outOff++] = faInR[_inOff];
      _inOff++;
   }
}

void YAC_CALL tksampleedit_copy_pan_dualmono_to_dualmono(YAC_Object *_faOutL, YAC_Object *_faOutR, sUI _outOff, YAC_Object *_faInL, YAC_Object *_faInR, sUI _inOff, sF32 _lvlL, sF32 _lvlR, sUI _numFrames) {
   sF32 *faOutL = (sF32*)_faOutL->yacArrayGetPointer();
   sF32 *faOutR = (sF32*)_faOutR->yacArrayGetPointer();
   const sF32 *faInL = (const sF32*)_faInL->yacArrayGetPointer();
   const sF32 *faInR = (const sF32*)_faInR->yacArrayGetPointer();

   for(sUI i = 0u; i < _numFrames; i++)
   {
      faOutL[_outOff] = faInL[_inOff] * _lvlL;
      faOutR[_outOff] = faInR[_inOff] * _lvlR;
      _outOff++;
      _inOff++;
   }
}

void YAC_CALL tksampleedit_copy_pan_stereowidth_dualmono_to_dualmono(YAC_Object *_faOutL, YAC_Object *_faOutR, sUI _outOff, YAC_Object *_faInL, YAC_Object *_faInR, sUI _inOff, sF32 _lvlL, sF32 _lvlR, sF32 _stereoWidth, sUI _numFrames) {
   sF32 *faOutL = (sF32*)_faOutL->yacArrayGetPointer();
   sF32 *faOutR = (sF32*)_faOutR->yacArrayGetPointer();
   const sF32 *faInL = (const sF32*)_faInL->yacArrayGetPointer();
   const sF32 *faInR = (const sF32*)_faInR->yacArrayGetPointer();

   for(sUI i = 0u; i < _numFrames; i++)
   {
      sF32 l = faInL[_inOff];
      sF32 r = faInR[_inOff];
      sF32 m = (l + r) * 0.5f;
      l = m + ((l - m) * _stereoWidth);
      r = m + ((r - m) * _stereoWidth);

      faOutL[_outOff] = l * _lvlL;
      faOutR[_outOff] = r * _lvlR;
      _outOff++;
      _inOff++;
   }
}

void YAC_CALL tksampleedit_copy_pan_dualmono_to_stereo(YAC_Object *_faOut, sUI _outOff, YAC_Object *_faInL, YAC_Object *_faInR, sUI _inOff, sF32 _lvlL, sF32 _lvlR, sUI _numFrames) {
   sF32 *faOut = (sF32*)_faOut->yacArrayGetPointer();
   const sF32 *faInL = (const sF32*)_faInL->yacArrayGetPointer();
   const sF32 *faInR = (const sF32*)_faInR->yacArrayGetPointer();

   for(sUI i = 0u; i < _numFrames; i++)
   {
      faOut[_outOff++] = faInL[_inOff] * _lvlL;
      faOut[_outOff++] = faInR[_inOff] * _lvlR;
      _inOff++;
   }
}

void YAC_CALL tksampleedit_copy_stereo_to_dualmono(YAC_Object *_faOutL, YAC_Object *_faOutR, sUI _outOff, YAC_Object *_faIn, sUI _inOff, sUI _numFrames) {
   sF32 *faOutL = (sF32*)_faOutL->yacArrayGetPointer();
   sF32 *faOutR = (sF32*)_faOutR->yacArrayGetPointer();
   const sF32 *faIn  = (const sF32*)_faIn->yacArrayGetPointer();

   for(sUI i = 0u; i < _numFrames; i++)
   {
      faOutL[_outOff] = faIn[_inOff++];
      faOutR[_outOff] = faIn[_inOff++];
      _outOff++;
   }
}

void YAC_CALL tksampleedit_copy_stereo_to_stereo(YAC_Object *_faOut, sUI _outOff, YAC_Object *_faIn, sUI _inOff, sUI _numFrames) {
   sF32 *faOut = (sF32*)_faOut->yacArrayGetPointer();
   const sF32 *faIn  = (const sF32*)_faIn->yacArrayGetPointer();

   for(sUI i = 0u; i < _numFrames; i++)
   {
      faOut[_outOff++] = faIn[_inOff++];
      faOut[_outOff++] = faIn[_inOff++];
   }
}

void YAC_CALL tksampleedit_copy_pan_stereo_to_stereo(YAC_Object *_faOut, sUI _outOff, YAC_Object *_faIn, sUI _inOff, sF32 _lvlL, sF32 _lvlR, sUI _numFrames) {
   sF32 *faOut = (sF32*)_faOut->yacArrayGetPointer();
   const sF32 *faIn  = (const sF32*)_faIn->yacArrayGetPointer();

   for(sUI i = 0u; i < _numFrames; i++)
   {
      faOut[_outOff++] = faIn[_inOff++] * _lvlL;
      faOut[_outOff++] = faIn[_inOff++] * _lvlR;
   }
}

void YAC_CALL tksampleedit_copy_pan_stereo_adv_to_stereo_adv(YAC_Object *_faOut, sUI _outOff, sUI _outAdv, YAC_Object *_faIn, sUI _inOff, sUI _inAdv, sF32 _lvlL, sF32 _lvlR, sUI _numFrames) {
   sF32 *faOut = (sF32*)_faOut->yacArrayGetPointer();
   const sF32 *faIn  = (const sF32*)_faIn->yacArrayGetPointer();

   for(sUI i = 0u; i < _numFrames; i++)
   {
      faOut[_outOff+0] = faIn[_inOff+0] * _lvlL;
      faOut[_outOff+1] = faIn[_inOff+1] * _lvlR;

      _inOff  += _inAdv;
      _outOff += _outAdv;
   }
}

void YAC_CALL tksampleedit_copy_pan_tanh_dim_stereo_adv_to_stereo_adv(YAC_Object *_faOut, sUI _outOff, sUI _outAdv, YAC_Object *_faIn, sUI _inOff, sUI _inAdv, sF32 _lvlL, sF32 _lvlR, sF32 _dim, sUI _numFrames) {
   sF32 *faOut = (sF32*)_faOut->yacArrayGetPointer();
   const sF32 *faIn  = (const sF32*)_faIn->yacArrayGetPointer();

   for(sUI i = 0u; i < _numFrames; i++)
   {
      sF32 x = faIn[_inOff] * _lvlL;
      if((x > 1.0f) || (x < -1.0f))
      {
#if 0
         x = (sF32) (
            (expf(sinf(x)*4.0f) - expf(-sinf(x)*4.0f*1.25f)) /
            (expf(sinf(x)*4.0f) + expf(-sinf(x)*4.0f))
                     );
#else
         x = tanhf(x);
#endif
      }
      faOut[_outOff] = x * _dim;

      x = faIn[_inOff+1] * _lvlR;
      if((x > 1.0f) || (x < -1.0f))
      {
#if 0
         x = (sF32) (
            (expf(sinf(x)*4.0f) - expf(-sinf(x)*4.0f*1.25f)) /
            (expf(sinf(x)*4.0f) + expf(-sinf(x)*4.0f))
                     );
#else
         x = tanhf(x);
#endif
      }
      faOut[_outOff+1] = x * _dim;

      _inOff  += _inAdv;
      _outOff += _outAdv;
   }
}

void YAC_CALL tksampleedit_add_pan_mono_adv_to_dualmono(YAC_Object *_faOutL, YAC_Object *_faOutR, sUI _outOff, YAC_Object *_faIn, sUI _inOff, sUI _inAdv, sF32 _lvlL, sF32 _lvlR, sUI _numFrames) {
   sF32 *faOutL = (sF32*)_faOutL->yacArrayGetPointer();
   sF32 *faOutR = (sF32*)_faOutR->yacArrayGetPointer();
   const sF32 *faIn = (const sF32*)_faIn->yacArrayGetPointer();

   for(sUI i = 0u; i < _numFrames; i++)
   {
      faOutL[_outOff] = faOutL[_outOff] + faIn[_inOff] * _lvlL;
      faOutR[_outOff] = faOutR[_outOff] + faIn[_inOff] * _lvlR;
      _outOff++;
      _inOff += _inAdv;
   }
}

void YAC_CALL tksampleedit_add_dualmono_to_stereo(YAC_Object *_faOut, sUI _outOff, YAC_Object *_faInL, YAC_Object *_faInR, sUI _inOff, sUI _numFrames) {
   sF32 *faOut = (sF32*)_faOut->yacArrayGetPointer();
   const sF32 *faInL = (const sF32*)_faInL->yacArrayGetPointer();
   const sF32 *faInR = (const sF32*)_faInR->yacArrayGetPointer();

   for(sUI i = 0u; i < _numFrames; i++)
   {
      faOut[_outOff] = faOut[_outOff] + faInL[_inOff];
      _outOff++;
      faOut[_outOff] = faOut[_outOff] + faInR[_inOff++];
      _outOff++;
   }
}


void YAC_CALL tksampleedit_add_stereo_to_stereo(YAC_Object *_faOut, sUI _outOff, YAC_Object *_faIn, sUI _inOff, sUI _numFrames) {
   sF32 *faOut = (sF32*)_faOut->yacArrayGetPointer();
   const sF32 *faIn  = (const sF32*)_faIn->yacArrayGetPointer();

   for(sUI i = 0u; i < _numFrames; i++)
   {
      faOut[_outOff] = faOut[_outOff] + faIn[_inOff++];
      _outOff++;
      faOut[_outOff] = faOut[_outOff] + faIn[_inOff++];
      _outOff++;
   }
}


void YAC_CALL tksampleedit_add_amp_mono_adv_to_mono_adv(YAC_Object *_faOut, sUI _outOff, sUI _outAdv, YAC_Object *_faIn, sUI _inOff, sUI _inAdv, sF32 _lvl, sUI _numFrames) {
   sF32 *faOut = (sF32*)_faOut->yacArrayGetPointer();
   const sF32 *faIn  = (const sF32*)_faIn->yacArrayGetPointer();

   for(sUI i = 0u; i < _numFrames; i++)
   {
      faOut[_outOff] = faOut[_outOff] + faIn[_inOff] * _lvl;
      _outOff += _outAdv;
      _inOff  += _inAdv;
   }
}


void YAC_CALL tksampleedit_amp_ramp_mono_adv_to_mono_adv(YAC_Object *_faOut, sUI _outOff, sUI _outAdv, YAC_Object *_faIn, sUI _inOff, sUI _inAdv, sF32 _lvlStart, sF32 _lvlEnd, sUI _numFrames) {
   if(_numFrames > 0u)
   {
      sF32 *faOut = (sF32*)_faOut->yacArrayGetPointer();
      const sF32 *faIn  = (const sF32*)_faIn->yacArrayGetPointer();
      const sF32 lvlAdd = (_lvlEnd - _lvlStart) / _numFrames;
      sF32 lvl = _lvlStart;
      for(sUI i = 0u; i < _numFrames; i++)
      {
         faOut[_outOff] = faIn[_inOff] * lvl;
         lvl     += lvlAdd;
         _outOff += _outAdv;
         _inOff  += _inAdv;
      }
   }
}


void YAC_CALL tksampleedit_add_amp_mono_adv_ring_to_mono_adv(YAC_Object *_faOut, sUI _outOff, sUI _outAdv, YAC_Object *_faIn, sUI _inOff, sUI _inAdv, sUI _inRingSz, sF32 _lvl, sUI _numFrames) {
   sF32 *faOut = (sF32*)_faOut->yacArrayGetPointer();
   const sF32 *faIn  = (const sF32*)_faIn->yacArrayGetPointer();

   sUI j = _inOff;

   for(sUI i = 0u; i < _numFrames; i++)
   {
      j = j % _inRingSz;
      faOut[_outOff] = faOut[_outOff] + faIn[j] * _lvl;
      _outOff += _outAdv;
      j += _inAdv;
   }
}

void YAC_CALL tksampleedit_add_mono_to_mono(YAC_Object *_faOut, sUI _outOff, YAC_Object *_faIn, sUI _inOff, sUI _numFrames) {
   sF32 *faOut = (sF32*)_faOut->yacArrayGetPointer();
   const sF32 *faIn  = (const sF32*)_faIn->yacArrayGetPointer();

   for(sUI i = 0u; i < _numFrames; i++)
   {
      faOut[_outOff++] += faIn[_inOff++];
   }
}

void YAC_CALL tksampleedit_xfade_mono(YAC_Object *_faOut, YAC_Object *_faIn, sUI _numFrames, sUI _xfadeFrames) {
   if((_numFrames > 0u) && (_numFrames >= (_xfadeFrames * 2u)))
   {
      sF32 *faOut = (sF32*)_faOut->yacArrayGetPointer();
      const sF32 *faIn  = (const sF32*)_faIn->yacArrayGetPointer();

      sUI numOut = _numFrames - _xfadeFrames;
      sUI numUnfaded = numOut - _xfadeFrames;

      sUI j = _xfadeFrames;
      sUI i = 0u;

      for(; i < numUnfaded; i++)
      {
         faOut[i] = faIn[j++];
      }

      sUI k = 0u;
      sF64 t = 0.0;
      sF64 step = 1.0 / _xfadeFrames;
      // yac_host->printf("xxx (numOut-i)=%u _xfadeFrames=%u\n", (numOut - i), _xfadeFrames);

      for(; i < numOut; i++)
      {
         float a = faIn[j++];
         float b = faIn[k++];
         faOut[i] = sF32(a + t * (b - a));
         t += step;
      }
   }
}

void YAC_CALL tksampleedit_add_pan_mono_to_stereo(YAC_Object *_faOut, sUI _outOff, YAC_Object *_faIn, sUI _inOff, sF32 _lvlL, sF32 _lvlR, sUI _numFrames) {
   sF32 *faOut = (sF32*)_faOut->yacArrayGetPointer();
   const sF32 *faIn  = (const sF32*)_faIn->yacArrayGetPointer();

   for(sUI i = 0u; i < _numFrames; i++)
   {
      faOut[_outOff] = faOut[_outOff] + faIn[_inOff] * _lvlL;
      _outOff++;
      faOut[_outOff] = faOut[_outOff] + faIn[_inOff++] * _lvlR;
      _outOff++;
   }
}

void YAC_CALL tksampleedit_add_pan_mono_adv_to_stereo(YAC_Object *_faOut, sUI _outOff, YAC_Object *_faIn, sUI _inOff, sUI _inAdv, sF32 _lvlL, sF32 _lvlR, sUI _numFrames) {
   sF32 *faOut = (sF32*)_faOut->yacArrayGetPointer();
   const sF32 *faIn  = (const sF32*)_faIn->yacArrayGetPointer();

   for(sUI i = 0u; i < _numFrames; i++)
   {
      faOut[_outOff] = faOut[_outOff] + faIn[_inOff] * _lvlL;
      _outOff++;
      faOut[_outOff] = faOut[_outOff] + faIn[_inOff] * _lvlR;
      _outOff++;
      _inOff += _inAdv;
   }
}


void YAC_CALL tksampleedit_add_pan_mono_adv_to_stereo_adv(YAC_Object *_faOut, sUI _outOff, sUI _outAdv, YAC_Object *_faIn, sUI _inOff, sUI _inAdv, sF32 _lvlL, sF32 _lvlR, sUI _numFrames) {
   sF32 *faOut = (sF32*)_faOut->yacArrayGetPointer();
   const sF32 *faIn  = (const sF32*)_faIn->yacArrayGetPointer();

   for(sUI i = 0u; i < _numFrames; i++)
   {
      faOut[_outOff + 0u] = faOut[_outOff + 0u] + faIn[_inOff] * _lvlL;
      faOut[_outOff + 1u] = faOut[_outOff + 1u] + faIn[_inOff] * _lvlR;
      _outOff += _outAdv;
      _inOff  += _inAdv;
   }
}


void YAC_CALL tksampleedit_add_pan_mono_to_stereo_adv(YAC_Object *_faOut, sUI _outOff, sUI _outAdv, YAC_Object *_faIn, sUI _inOff, sF32 _lvlL, sF32 _lvlR, sUI _numFrames) {
   sF32 *faOut = (sF32*)_faOut->yacArrayGetPointer();
   const sF32 *faIn  = (const sF32*)_faIn->yacArrayGetPointer();

   for(sUI i = 0u; i < _numFrames; i++)
   {
      faOut[_outOff + 0u] = faOut[_outOff + 0u] + faIn[_inOff] * _lvlL;
      faOut[_outOff + 1u] = faOut[_outOff + 1u] + faIn[_inOff] * _lvlR;
      _outOff += _outAdv;
      _inOff++;
   }
}


void YAC_CALL tksampleedit_add_pan_dualmono_to_stereo(YAC_Object *_faOut, sUI _outOff, YAC_Object *_faInL, YAC_Object *_faInR, sUI _inOff, sF32 _lvlL, sF32 _lvlR, sUI _numFrames) {
   sF32 *faOut = (sF32*)_faOut->yacArrayGetPointer();
   const sF32 *faInL = (const sF32*)_faInL->yacArrayGetPointer();
   const sF32 *faInR = (const sF32*)_faInR->yacArrayGetPointer();

   for(sUI i = 0u; i < _numFrames; i++)
   {
      faOut[_outOff] = faOut[_outOff] + faInL[_inOff] * _lvlL;
      _outOff++;
      faOut[_outOff] = faOut[_outOff] + faInR[_inOff++] * _lvlR;
      _outOff++;
   }
}

void YAC_CALL tksampleedit_add_pan_dualmono_to_stereo_adv(YAC_Object *_faOut, sUI _outOff, sUI _outAdv, YAC_Object *_faInL, YAC_Object *_faInR, sUI _inOff, sF32 _lvlL, sF32 _lvlR, sUI _numFrames) {
   sF32 *faOut = (sF32*)_faOut->yacArrayGetPointer();
   const sF32 *faInL = (const sF32*)_faInL->yacArrayGetPointer();
   const sF32 *faInR = (const sF32*)_faInR->yacArrayGetPointer();

   for(sUI i = 0u; i < _numFrames; i++)
   {
      faOut[_outOff + 0u] = faOut[_outOff + 0u] + faInL[_inOff] * _lvlL;
      faOut[_outOff + 1u] = faOut[_outOff + 1u] + faInR[_inOff] * _lvlR;
      _outOff += _outAdv;
      _inOff++;
   }
}

void YAC_CALL tksampleedit_add_pan_stereo_to_dualmono(YAC_Object *_faOutL, YAC_Object *_faOutR, sUI _outOff, YAC_Object *_faIn, sUI _inOff, sF32 _lvlL, sF32 _lvlR, sUI _numFrames) {
   sF32 *faOutL = (sF32*)_faOutL->yacArrayGetPointer();
   sF32 *faOutR = (sF32*)_faOutR->yacArrayGetPointer();
   const sF32 *faIn  = (const sF32*)_faIn->yacArrayGetPointer();

   for(sUI i = 0u; i < _numFrames; i++)
   {
      faOutL[_outOff] = faOutL[_outOff] + faIn[_inOff++] * _lvlL;
      faOutR[_outOff] = faOutR[_outOff] + faIn[_inOff++] * _lvlR;
      _outOff++;
   }
}

void YAC_CALL tksampleedit_add_pan_stereo_to_stereo(YAC_Object *_faOut, sUI _outOff, YAC_Object *_faIn, sUI _inOff, sF32 _lvlL, sF32 _lvlR, sUI _numFrames) {
   sF32 *faOut = (sF32*)_faOut->yacArrayGetPointer();
   const sF32 *faIn  = (const sF32*)_faIn->yacArrayGetPointer();

   for(sUI i = 0u; i < _numFrames; i++)
   {
      faOut[_outOff] = faOut[_outOff] + faIn[_inOff++] * _lvlL;
      _outOff++;
      faOut[_outOff] = faOut[_outOff] + faIn[_inOff++] * _lvlR;
      _outOff++;
   }
}

void YAC_CALL tksampleedit_add_pan_stereo_adv_to_stereo_adv(YAC_Object *_faOut, sUI _outOff, sUI _outAdv, YAC_Object *_faIn, sUI _inOff, sUI _inAdv, sF32 _lvlL, sF32 _lvlR, sUI _numFrames) {
   sF32 *faOut = (sF32*)_faOut->yacArrayGetPointer();
   const sF32 *faIn  = (const sF32*)_faIn->yacArrayGetPointer();

   for(sUI i = 0u; i < _numFrames; i++)
   {
      faOut[_outOff + 0u] = faOut[_outOff + 0u] + faIn[_inOff + 0u] * _lvlL;
      faOut[_outOff + 1u] = faOut[_outOff + 1u] + faIn[_inOff + 1u] * _lvlR;
      _outOff += _outAdv;
      _inOff  += _inAdv;
   }
}

sF32 YAC_CALL tksampleedit_calc_peak_mono_adv(YAC_Object *_faIn, sUI _inOff, sUI _inAdv, sUI _numFrames) {
   const sF32 *faIn  = (const sF32*)_faIn->yacArrayGetPointer();

   sF32 peak = 0.0f;

   if(_numFrames > 0)
   {
      sUI k = _inOff;

      for(sUI i = 0; i < _numFrames; i++, k += _inAdv)
      {
         sF32 f = faIn[k];

         if(f < 0.0f)
            f = -f;

         if(f > peak)
            peak = f;
      }
   }

   return peak;
}

sF32 YAC_CALL tksampleedit_calc_peak_mono_adv_ring(YAC_Object *_faIn, sUI _inOff, sUI _inAdv, sUI _ringSize, sUI _numFrames) {
   const sF32 *faIn  = (const sF32*)_faIn->yacArrayGetPointer();

   sF32 peak = 0.0f;

   if(_numFrames > 0)
   {
      sUI k = _inOff;

      for(sUI i = 0; i < _numFrames; i++, k += _inAdv)
      {
         k = k % _ringSize;

         sF32 f = faIn[k];

         if(f < 0.0f)
            f = -f;

         if(f > peak)
            peak = f;
      }
   }

   return peak;
}

sF32 YAC_CALL tksampleedit_calc_rms_mono_adv(YAC_Object *_faIn, sUI _inOff, sUI _inAdv, sUI _numFrames) {
   const sF32 *faIn  = (const sF32*)_faIn->yacArrayGetPointer();

   sF32 rms = 0.0f;

   if(_numFrames > 0)
   {
      sUI k = _inOff;

      for(sUI i = 0; i < _numFrames; i++, k += _inAdv)
      {
         rms += faIn[k] * faIn[k];
      }

      rms = sqrtf(rms / _numFrames);

      rms += 0.01f;
      rms -= 0.01f;

   }

   return rms;
}

sF32 YAC_CALL tksampleedit_calc_rms_mono_adv_ring(YAC_Object *_faIn, sUI _inOff, sUI _inAdv, sUI _ringSize, sUI _numFrames) {
   const sF32 *faIn  = (const sF32*)_faIn->yacArrayGetPointer();

   sF64 rms = 0.0;

   if(_numFrames > 0)
   {
      sUI k = _inOff;

      for(sUI i = 0; i < _numFrames; i++, k += _inAdv)
      {
         k = k % _ringSize;
         rms += sF64(faIn[k]) * sF64(faIn[k]);
      }

      rms = sqrt(rms / _numFrames);

      rms += 0.01;
      rms -= 0.01;
   }

   return sF32(rms);
}

void YAC_CALL tksampleedit_ms_convert_dualmono(YAC_Object *_faOutL, YAC_Object *_faOutR, sUI _outOff, YAC_Object *_faInL, YAC_Object *_faInR, sUI _inOff, sF32 _lvlOut, sUI _numFrames) {

   sF32 *faOutL = (sF32*)_faOutL->yacArrayGetPointer();
   sF32 *faOutR = (sF32*)_faOutR->yacArrayGetPointer();
   const sF32 *faInL = (const sF32*)_faInL->yacArrayGetPointer();
   const sF32 *faInR = (const sF32*)_faInR->yacArrayGetPointer();

   for(sUI i = 0u; i < _numFrames; i++)
   {
      // (note) M = L+R  (*0.5)
      // (note) S = L-R  (*0.5)
      // (note) L = M+S
      // (note) R = M-S

      sF32 a = faInL[_inOff];
      sF32 b = faInR[_inOff++];

      faOutL[_outOff]   = (a + b) * _lvlOut;
      faOutR[_outOff++] = (a - b) * _lvlOut;
   }
}

void YAC_CALL tksampleedit_monomix_restore_dualmono(YAC_Object *_faMonoMixOut, YAC_Object *_faRestoreOut, YAC_Object *_faInL, YAC_Object *_faInR, YAC_Object *_faRestoreIn, sUI _numFrames) {

   sF32 *faMonoMixOut = (sF32*)_faMonoMixOut->yacArrayGetPointer();
   sF32 *faRestoreOut = (sF32*)_faRestoreOut->yacArrayGetPointer();
   const sF32 *faInL = (const sF32*)_faInL->yacArrayGetPointer();
   const sF32 *faInR = (const sF32*)_faInR->yacArrayGetPointer();
   const sF32 *faRestoreIn = (const sF32*)_faRestoreIn->yacArrayGetPointer();

   for(sUI i = 0u; i < _numFrames; i++)
   {
      faMonoMixOut[i] = (faInL[i] + faInR[i]) * 0.5f;
      faRestoreOut[i] = faRestoreIn[i];
   }
}

void YAC_CALL tksampleedit_autochop(YAC_Object *_retStartOffsets, YAC_Object *_retEndOffsets, YAC_Object *_smp, sUI _numChannels, sF32 _startThreshold, sF32 _startMinThreshold, sUI _startMinNumFramesBack, sF32 _endThreshold, sUI _endNumFramesThreshold, sF32 _endThresholdRestart, sUI _restartNumFramesThreshold) {

   //        startThreshold:  level that triggers region start
   //     startMinThreshold:  level that determines actual region start (up to 'startMinNumFramesBack' frames before region start trigger)
   // startMinNumFramesBack:  maximum number of frames before region start trigger (catch transients)
   //          endThreshold:  (after region start) consider frame as possible region end when level falls below this value
   // endNumFramesThreshold:  number of frames that level must fall below 'endThreshold' to trigger region end
   //   endThresholdRestart:  start next region when level is in range 'endThreshold'..'endThresholdRestart' for 'restartNumFramesThreshold' and
   //                          then rises to 'startTreshold' again

   if(YAC_Is_IntArray(_retStartOffsets))
   {
      if(YAC_Is_IntArray(_retEndOffsets))
      {
         if(YAC_Is_FloatArray(_smp))
         {
            YAC_CAST_ARG(YAC_IntArray, startOffsets, _retStartOffsets);
            YAC_CAST_ARG(YAC_IntArray, endOffsets, _retEndOffsets);
            YAC_CAST_ARG(YAC_FloatArray, samples, _smp);

            const sF32 *smpData = samples->elements;
            if(NULL == smpData)
               return;

            if(0 == _numChannels)
               return;

            sUI numFrames = samples->num_elements / _numChannels;
            if(0 == numFrames)
               return;

            // trace "xxx processData: numFrames="+numFrames;

            int frameIdx = 0;
            int sliceIdx = 0;
            int frameIdxBwd;

            int state = 0; // 0=find start, 1=find end

            // float lastStartMinSmp = 99999;
            int lastStartMinIdx = -1;

            sUI endNumFramesCnt;

            sBool bAllowRestart = YAC_FALSE;
            int allowRestartCnt = 0;

            YAC_Value vi;
            vi.initInt(0);

            int startIdx = 0;
            int endIdx = 0;
            sF32 smp;

            for(sUI i = 0u; i < numFrames; i++)
            {
               for(sUI chIdx = 0u; chIdx < 1u/*_numChannels*/; chIdx++)
               {
                  smp = smpData[(frameIdx*_numChannels+chIdx)];
                  if(smp < 0)
                     smp = -smp;

                  if(0 == state)
                  {
                     // State 0: Find start
                     if(smp > _startThreshold)
                     {
                        if(-1 == lastStartMinIdx)
                        {
                           startIdx = frameIdx;
                        }
                        else
                        {
                           startIdx = lastStartMinIdx;
                        }

                        Dprintf("[dbg] slice %d start at %d (curNum=%u)\n", sliceIdx, startIdx, startOffsets->num_elements);
                        // if(autochop_start_offsets.numElements < maxSlices)
                        {
                           // if( (-1 == pickSlice) || (sliceIdx == pickSlice) )
                           {
                              vi.value.int_val = startIdx;
                              startOffsets->yacArraySet(NULL/*context*/, startOffsets->num_elements, &vi);
                           }
                        }
                        endNumFramesCnt = 0u;
                        state = 1;
                        lastStartMinIdx = -1;
                        allowRestartCnt = 0;
                        bAllowRestart = YAC_FALSE;
                     }
                     else if(smp > _startMinThreshold)
                     {
                        if(-1 == lastStartMinIdx)
                        {
                           // Force update (first candidate)
                           // lastStartMinSmp = smp;
                           lastStartMinIdx = frameIdx;
                        }
                        else if( lastStartMinIdx < (frameIdx - sSI(_startMinNumFramesBack)) )
                        {
                           // Force update (last start candidate is too old)
                           // lastStartMinSmp = smp;
                           lastStartMinIdx = frameIdx;
                        }
                     }
                     else if( lastStartMinIdx < (frameIdx - sSI(_startMinNumFramesBack)) )
                     {
                        // Discard last candidate (too old)
                        lastStartMinIdx = -1;
                     }
                  }
                  else
                  {
                     // State 1: Find end or restart
                     if(smp < _endThresholdRestart)
                     {
                        allowRestartCnt++;

                        if(allowRestartCnt >= sSI(_restartNumFramesThreshold))
                        {
                           bAllowRestart = YAC_TRUE; // b_autochop_restart/*true*/;
                        }
                     }
                     else
                     {
                        allowRestartCnt = 0;
                     }

                     if(smp < _endThreshold)
                     {
                        endNumFramesCnt++;
                        if(endNumFramesCnt >= _endNumFramesThreshold)
                        {
                           Dprintf("[dbg] slice end at %d  (after %u frames of silence)\n", frameIdx, endNumFramesCnt);

                           // if(endOffsets.numElements < maxSlices)
                           {
                              // if( (-1 == pickSlice) || (sliceIdx == pickSlice) )
                              {
                                 // Search backward and skip zero samples
                                 endIdx = frameIdx;
                                 frameIdxBwd = frameIdx;
                                 while(--frameIdxBwd > startIdx)
                                 {
                                    smp = smpData[(frameIdxBwd*_numChannels+chIdx)];
                                    if(0 != smp)
                                    {
                                       endIdx = frameIdxBwd + 1;
                                       break;
                                    }
                                 }
                                 vi.value.int_val = endIdx;
                                 endOffsets->yacArraySet(NULL/*context*/, endOffsets->num_elements, &vi);
                              }
                              sliceIdx++;
                           }
                           state = 0;
                        }
                     }
                     else
                     {
                        endNumFramesCnt = 0u;

                        if(bAllowRestart)
                        {
                           // trace "xxx bAllowRestart: smp="+smp+" startThreshold="+startThreshold;
                           if(smp > _startThreshold)
                           {
                              vi.value.int_val = frameIdx;
                              endOffsets->yacArraySet(NULL/*context*/, endOffsets->num_elements, &vi);

                              startIdx = frameIdx;

                              Dprintf("[dbg] slice restart at %d\n", startIdx);

                              // if(autochop_start_offsets.numElements < maxSlices)
                              {
                                 // if( (-1 == pickSlice) || (sliceIdx == pickSlice) )
                                 {
                                    vi.value.int_val = startIdx;
                                    startOffsets->yacArraySet(NULL/*context*/, startOffsets->num_elements, &vi);
                                 }
                              }
                              endNumFramesCnt = 0u;
                              //state = 1;
                              lastStartMinIdx = -1;
                              allowRestartCnt = 0;
                              bAllowRestart = YAC_FALSE;
                           }

                        }
                     }
                  }

               }

               frameIdx++;
            }

            if(1 == state)
            {
               // if(autochop_end_offsets.numElements < maxSlices)
               {
                  // if( (-1 == pickSlice) || (sliceIdx == pickSlice) )
                  {
                     // Search backward and skip zero samples
                     endIdx = frameIdx;
                     frameIdxBwd = frameIdx;
                     while(--frameIdxBwd > startIdx)
                     {
                        smp = smpData[(frameIdxBwd*_numChannels+0/*chIdx*/)];
                        if(0 != smp)
                        {
                           endIdx = frameIdxBwd + 1;
                           break;
                        }
                     }
                     vi.value.int_val = endIdx;
                     endOffsets->yacArraySet(NULL/*context*/, endOffsets->num_elements, &vi);
                  }
               }
            }

         } // if smp
      } // if retEndOffsets
   } // if retStartOffsets
}

void YAC_CALL tksampleedit_autochop_rms(YAC_Object *_retStartOffsets, YAC_Object *_retEndOffsets, YAC_Object *_smp, sUI _numChannels, sF32 _startThreshold, sF32 _startMinThreshold, sUI _startMinNumFramesBack, sF32 _endThreshold, sUI _endNumFramesThreshold, sUI _restartNumFramesThreshold, YAC_FloatArray *_restartRatios, sUI _restartRMSRatioLongNumFrames, sF32 _restartThresholdImm, sSI _restartRewindNumFrames, sF32 _rmsAvgSlew) {

   //            startThreshold:  level that triggers region start
   //         startMinThreshold:  level that determines actual region start (up to 'startMinNumFramesBack' frames before region start trigger)
   //     startMinNumFramesBack:  maximum number of frames before region start trigger (catch transients)
   //              endThreshold:  (after region start) consider frame as possible region end when level falls below this value
   //     endNumFramesThreshold:  number of frames that level must fall below 'endThreshold' to trigger region end
   // restartNumFramesThreshold:  number of frames that RMS must fall below last region start RMS before restart is allowed again
   //           restartRMSRatio:  start next region when RMS rises to 'restartRatio'*avgRMS
   //    restartRewindNumFrames:  rewind restart position <n> frames (catch transient) (e.g. 90)
   //                rmsAvgSlew:  avg rise rate (smaller=slower) (e.g. 0.01f)

   // (note) better suited for splitting drumloops

   // yac_host->printf("xxx autochop_rms: ENTER\n");

   sF32 _restartRMSRatioShort = _restartRatios->elements[0];
   sF32 _restartRMSRatioLong  = _restartRatios->elements[1];
   sF32 _restartRMSRatioImm   = _restartRatios->elements[2];
   sF32 _rmsPeakRatio         = _restartRatios->elements[3]; // 2.3f;

   if(YAC_Is_IntArray(_retStartOffsets))
   {
      if(YAC_Is_IntArray(_retEndOffsets))
      {
         if(YAC_Is_FloatArray(_smp))
         {
            YAC_CAST_ARG(YAC_IntArray, startOffsets, _retStartOffsets);
            YAC_CAST_ARG(YAC_IntArray, endOffsets, _retEndOffsets);
            YAC_CAST_ARG(YAC_FloatArray, samples, _smp);

            const sF32 *smpData = samples->elements;
            if(NULL == smpData)
               return;

            if(0 == _numChannels)
               return;

            sUI numFrames = samples->num_elements / _numChannels;
            if(0 == numFrames)
               return;

            // yac_host->printf("xxx autochop_rms: numFrames=%u\n", numFrames);

            int frameIdx = 0;
            int sliceIdx = 0;
            int frameIdxBwd;

            int state = 0; // 0=find start, 1=find end

            // float lastStartMinSmp = 99999;
            int lastStartMinIdx = -1;

            sUI endNumFramesCnt = 0u;

            sBool bAllowRestart = YAC_FALSE;
            sUI allowRestartCnt = 0u;

            YAC_Value vi;
            vi.initInt(0);

            int startIdx = 0;
            int endIdx = 0;
            sF32 smp;

            static const int AUTOCHOP_RMS_HISTORY_SZ = 256;  // POT
            float rmsHistory[AUTOCHOP_RMS_HISTORY_SZ];  // squared RMS values for past <n> frames
            sUI rmsHistorySz = 0u;
            sF32 smpSq;
            sF32 rmsSquared = 0.0f;
            sF32 rmsAvg = 0.0f;
            sF32 rms;
            // sF32 rmsStart = 0.0f;  // last slice start RMS
            sF32 rmsPeak = 0.0f;  // RMS peak since last slice start
            sUI numFramesSinceStart = 0u;

            for(sUI i = 0u; i < numFrames; i++)
            {
               // max(ch1, ch2)
               smp = 0.0f;
               for(sUI chIdx = 0u; chIdx < _numChannels; chIdx++)
               {
                  sF32 t = smpData[(frameIdx*_numChannels+chIdx)];
                  if(t < 0.0f)
                     smp = -t;
                  if(t > smp)
                     smp = t;
               }

               if(rmsHistorySz == AUTOCHOP_RMS_HISTORY_SZ)
               {
                  // Subtract oldest (squared) RMS value
                  rmsSquared -= rmsHistory[(frameIdx - AUTOCHOP_RMS_HISTORY_SZ) & (AUTOCHOP_RMS_HISTORY_SZ-1)];
               }
               else
                  rmsHistorySz++;
               // add to ring buffer
               smpSq = smp * smp;
               smpSq = Dyac_denorm_32(smpSq);
               rmsHistory[frameIdx & (AUTOCHOP_RMS_HISTORY_SZ-1)] = smpSq;
               rmsSquared += smpSq;
               // calc rms and average rms
               rms = sqrtf(rmsSquared / rmsHistorySz);
               rms = Dyac_denorm_32(rms);
               rmsAvg = rmsAvg + (rms - rmsAvg) * _rmsAvgSlew;
               rmsAvg = Dyac_denorm_32(rmsAvg);

               if(rms > rmsPeak)
                  rmsPeak = rms;

               if(0 == state)
               {
                  numFramesSinceStart = 0u;

                  // State 0: Find start
                  if(smp > _startThreshold)
                  {
                     if(-1 == lastStartMinIdx)
                     {
                        startIdx = frameIdx;
                     }
                     else
                     {
                        startIdx = lastStartMinIdx;
                     }

                     Dprintf("[dbg] slice %d start at %d (curNum=%u) rms=%f rmsAvg=%f\n", sliceIdx, startIdx, startOffsets->num_elements, rms, rmsAvg);
                     vi.value.int_val = startIdx;
                     startOffsets->yacArraySet(NULL/*context*/, startOffsets->num_elements, &vi);
                     endNumFramesCnt = 0u;
                     state = 1;
                     lastStartMinIdx = -1;
                     allowRestartCnt = 0u;
                     bAllowRestart = YAC_FALSE;
                     rmsAvg = rms;
                     // rmsStart = rms;
                     rmsPeak = rms;
                  }
                  else if(smp > _startMinThreshold)
                  {
                     if(-1 == lastStartMinIdx)
                     {
                        // Force update (first candidate)
                        // lastStartMinSmp = smp;
                        lastStartMinIdx = frameIdx;
                     }
                     else if( lastStartMinIdx < (frameIdx - sSI(_startMinNumFramesBack)) )
                     {
                        // Force update (last start candidate is too old)
                        // lastStartMinSmp = smp;
                        lastStartMinIdx = frameIdx;
                     }
                  }
                  else if( lastStartMinIdx < (frameIdx - sSI(_startMinNumFramesBack)) )
                  {
                     // Discard last candidate (too old)
                     lastStartMinIdx = -1;
                  }
               }
               else
               {
                  numFramesSinceStart++;

                  // State 1: Find end or restart
                  // if(rmsAvg < rmsStart)
                  if((rms*_rmsPeakRatio) < rmsPeak)
                  {
                     // allow restart once rmsAvg falls below slice start rmsAvg
                     allowRestartCnt++;

                     if(allowRestartCnt >= _restartNumFramesThreshold)
                     {
                        bAllowRestart = YAC_TRUE; // b_autochop_restart/*true*/;
                     }
                  }
                  else
                  {
                     allowRestartCnt = 0u;
                  }

                  // boolean bImm = (rms > (rmsAvg * _restartRMSRatioImm));  // large peak jump => restart immediately
                  sBool bImm = (numFramesSinceStart > _restartNumFramesThreshold) && (smp > _restartThresholdImm) && (smpSq > (rms * _restartRMSRatioImm));  // large peak jump => restart immediately

                  if(smp < _endThreshold)
                  {
                     endNumFramesCnt++;
                     if(endNumFramesCnt >= _endNumFramesThreshold)
                     {
                        Dprintf("[dbg] slice end at %d  (after %u frames of silence)\n", frameIdx, endNumFramesCnt);

                        // Search backward and skip zero samples
                        endIdx = frameIdx;
                        frameIdxBwd = frameIdx;
                        while(--frameIdxBwd > startIdx)
                        {
                           // max(ch1, ch2)
                           sF32 smpBW = 0.0f;
                           for(sUI chIdxBW = 0u; chIdxBW < _numChannels; chIdxBW++)
                           {
                              sF32 t = smpData[frameIdxBwd*_numChannels+chIdxBW];
                              if(t < 0)
                                 smpBW = -t;
                              if(t > smpBW)
                                 smpBW = t;
                           }

                           if(0 != smpBW)
                           {
                              endIdx = frameIdxBwd + 1;
                              break;
                           }
                        }
                        vi.value.int_val = endIdx;
                        endOffsets->yacArraySet(NULL/*context*/, endOffsets->num_elements, &vi);
                        sliceIdx++;
                        state = 0;
                     }
                  }
                  else
                  {
                     endNumFramesCnt = 0u;

                     // if( (frameIdx > 14466) && (frameIdx < 14927) )
                     // {
                     //    yac_host->printf("xxx frame[%d] smpSq=%f rms=%f rmsAvg=%f ratio=%f bAllowRestart=%d rmsStart=%f smp=%f immRatio=%f rmsPeak=%f peakRatio=%f\n", frameIdx, smpSq, rms, rmsAvg, (rmsAvg > 0.0f) ? (rms / rmsAvg) : 999999.0f, bAllowRestart, rmsStart, smp, (rms > 0.0f) ? (smpSq/rms):999999.0f, rmsPeak, (rms > 0.0f)?(rmsPeak/rms):999999.0f);
                     // }

                     if(bAllowRestart || bImm)
                     {
                        // tweaked for funkydrummer (long) and tabla+tambourine loop (short)
                        sF32 restartRatio = (numFramesSinceStart > _restartRMSRatioLongNumFrames) ? _restartRMSRatioLong : _restartRMSRatioShort;

                        // trace "xxx bAllowRestart: smp="+smp+" startThreshold="+startThreshold;
                        if(((rms > (rmsAvg * restartRatio)) || bImm) && (smp > _startThreshold))
                        {
                           sUI frameIdxEnd = (frameIdx > _restartRewindNumFrames) ? (frameIdx - _restartRewindNumFrames) : frameIdx;
                           vi.value.int_val = frameIdxEnd;
                           endOffsets->yacArraySet(NULL/*context*/, endOffsets->num_elements, &vi);

                           startIdx = frameIdxEnd;

                           Dprintf("[dbg] slice restart at %d (rmsRatio=%f) rms=%f rmsAvg=%f rmsPeak=%f peakRatio=%f bImm=%d smpSq=%f\n", startIdx, (rmsAvg > 0.0f) ? (rms / rmsAvg) : 999999.0f, rms, rmsAvg, rmsPeak, (rmsPeak / rms), bImm, smpSq);

                           vi.value.int_val = startIdx;
                           startOffsets->yacArraySet(NULL/*context*/, startOffsets->num_elements, &vi);

                           endNumFramesCnt = 0u;
                           //state = 1;
                           lastStartMinIdx = -1;
                           allowRestartCnt = 0u;
                           bAllowRestart = YAC_FALSE;
                           rmsAvg = rms;
                           // rmsStart = rms;
                           rmsPeak = rms;
                           numFramesSinceStart = 0u;
                        }
                     }  // if allow restart
                  }
               }

               frameIdx++;
            }

            if(1 == state)
            {
               // Search backward and skip zero samples
               endIdx = frameIdx;
               frameIdxBwd = frameIdx;
               while(--frameIdxBwd > startIdx)
               {
                  sF32 smpBW = 0.0f;
                  for(sUI chIdxBW = 0u; chIdxBW < _numChannels; chIdxBW++)
                  {
                     sF32 t = smpData[frameIdxBwd*_numChannels+chIdxBW];
                     if(t < 0)
                        t = -smpBW;
                     if(t > smpBW)
                        smpBW = t;
                  }
                  if(0 != smpBW)
                  {
                     endIdx = frameIdxBwd + 1;
                     break;
                  }
               }
               vi.value.int_val = endIdx;
               endOffsets->yacArraySet(NULL/*context*/, endOffsets->num_elements, &vi);
            }

         } // if smp
      } // if retEndOffsets
   } // if retStartOffsets
}

void YAC_CALL tksampleedit_autochop_rms_windowed(YAC_Object *_retStartOffsets, YAC_Object *_retEndOffsets, YAC_Object *_smp, sUI _numChannels, sF32 _hpf, sUI _rmsWindowSize, sUI _timeout, sF32 _rmsSlew, sF32 _avgPeakRatioSlew, sF32 _avgPeakRatioThreshold, sUI _maxMarkers) {
   // sUI _rmsPeakHistorySize

   // yac_host->printf("xxx autochop_rms_windowed: ENTER\n");

   if(YAC_Is_IntArray(_retStartOffsets))
   {
      if(YAC_Is_IntArray(_retEndOffsets))
      {
         if(YAC_Is_FloatArray(_smp))
         {
            YAC_CAST_ARG(YAC_IntArray, startOffsets, _retStartOffsets);
            YAC_CAST_ARG(YAC_IntArray, endOffsets, _retEndOffsets);
            YAC_CAST_ARG(YAC_FloatArray, samples, _smp);

            if(_rmsWindowSize < 1u)
               _rmsWindowSize = 1u;

            if(_timeout < 100u)
               _timeout = 100u;

            const sF32 *smpData = samples->elements;
            if(NULL == smpData)
               return;

            if(0 == _numChannels)
               return;

            sUI numFrames = samples->num_elements / _numChannels;
            // if(numFrames < (_rmsWindowSize * _rmsPeakHistorySize))
            if(numFrames < _timeout)
               return;

            // yac_host->printf("xxx autochop_rms: numFrames=%u\n", numFrames);

            sF64 *rmsHistory = new sF64[_rmsWindowSize];
            sUI rmsHistorySz = 0u;

            // sF64 *rmsPeakHistory = new sF64[_rmsPeakHistorySize];
            // sF64 rmsPeakAvg = 0.0;
            // sUI rmsPeakHistoryIdx = 0u;

            for(sUI i = 0u; i < _rmsWindowSize; i++)
               rmsHistory[i] = 0.0;

            // for(sUI i = 0u; i < _rmsPeakHistorySize; i++)
            //    rmsPeakHistory[i] = 0.0;

            tksampleedit_Biquad2 flt;
            flt.reset();
            flt.calcParams(2/*type=HPF*/, 0.0f/*dbGain*/, _hpf, 0.0f/*res*/);

            int frameIdx = 0;
            // int sliceIdx = 0;
            // int frameIdxBwd;

            // int state = 0; // 0=find start, 1=find end

            // // float lastStartMinSmp = 99999;
            // int lastStartMinIdx = -1;

            // sUI endNumFramesCnt = 0u;

            // sBool bAllowRestart = YAC_FALSE;
            // sUI allowRestartCnt = 0u;

            YAC_Value vi;
            vi.initInt(0);

            int startIdx = 0;
            int endIdx = 0;
            sF32 smp;

            sF64 smpSq;
            sF64 rmsSquared = 0.0f;
            // sF64 rmsAvg = 0.0f;
            sF64 rms;
            // sF32 rmsStart = 0.0f;  // last slice start RMS
            // sF64 rmsPeak = 0.0f;  // RMS peak since last slice start
            // sUI numFramesSinceStart = 0u;

            // sUI rmsWindowIdx = 0u;

            // sF64 rmsWindowPeak = 0.0;
            sF64 rmsWindowPeakSlewed = 0.0;

            sF64 rmsSlewed = 0.0;

            for(sUI i = 0u; i < numFrames; i++)
            {
               // max(ch1, ch2)
               smp = 0.0f;
               sF32 smpOrig = 0.0f;
               for(sUI chIdx = 0u; chIdx < _numChannels; chIdx++)
               {
                  sF32 t = smpData[(frameIdx*_numChannels+chIdx)];
                  sF32 t2 = t;
                  if(t < 0.0f)
                     t = -t;
                  if(t > smp)
                  {
                     smp = t;
                     smpOrig = t2;
                  }
               }

               smp = flt.filter(smpOrig);
               if(smp < 0.0f)
                  smp = -smp;

               if(rmsHistorySz == _rmsWindowSize)
               {
                  // Subtract oldest (squared) RMS value
                  sSI t = ((sSI)frameIdx) - (sSI)_rmsWindowSize;
                  if(t < 0)
                     t += (sSI)_rmsWindowSize;
                  else
                     t = (frameIdx - _rmsWindowSize) % _rmsWindowSize;
                  rmsSquared -= rmsHistory[t];
               }
               else
                  rmsHistorySz++;

               // calc rms and average rms
               smpSq = smp * smp;
               smpSq = Dyac_denorm_64(smpSq);

               rms = sqrt(rmsSquared / rmsHistorySz);
               rms = Dyac_denorm_64(rms);

               // if(frameIdx >= 88916 && frameIdx < 89258)
               // if(frameIdx >= 2074 && frameIdx < 2262)
               //    yac_host->printf("xxx frameIdx=%d rms=%f rmsWindowPeak=%f rmsWindowPeakSlewed=%f\n", frameIdx, rms, rmsWindowPeak, rmsWindowPeakSlewed);

               // if(rms > rmsWindowPeak)
               //    rmsWindowPeak = rms;

               rmsSlewed += (rms - rmsSlewed) * _rmsSlew;

               // if(rmsWindowPeak > (rmsWindowPeakSlewed * _avgPeakRatioThreshold))
               if(rmsSlewed > (rmsWindowPeakSlewed * _avgPeakRatioThreshold))
               {
                  //////timeout=sSI(_rmsWindowSize*_rmsPeakHistorySize)
                  if( (0u == startOffsets->num_elements) || (sUI(frameIdx) >= (sUI(startIdx) + _timeout)) )
                  {
                     startIdx = frameIdx;

                     // New marker / cue point
                     if(startOffsets->num_elements <= _maxMarkers)
                     {
                        if( (startOffsets->num_elements > 0u) && (startIdx > endIdx) )
                        {
                           // Finish previous zone
                           vi.value.int_val = startIdx - 1;
                           endOffsets->yacArraySet(NULL/*context*/, endOffsets->num_elements, &vi);
                        }

                        // Start new zone
                        vi.value.int_val = startIdx;
                        startOffsets->yacArraySet(NULL/*context*/, startOffsets->num_elements, &vi);
                     }

                     endIdx = startIdx;
                     // rmsWindowPeakSlewed = rmsWindowPeak;
                     rmsWindowPeakSlewed = rmsSlewed;
                  }
               }

               // if(0u == (frameIdx % _rmsWindowSize))
               // {
               //    rmsPeakHistory[rmsPeakHistoryIdx++ % _rmsPeakHistorySize] = rmsWindowPeak;
               //    // rmsWindowPeakSlewed += (rmsWindowPeak - rmsWindowPeakSlewed) * _avgPeakRatioSlew;

               //    rmsWindowIdx++;
               //    rmsWindowPeak = 0.0;
               // }

               rmsWindowPeakSlewed += (rmsSlewed - rmsWindowPeakSlewed) * _avgPeakRatioSlew;

               // add to ring buffer
               rmsHistory[frameIdx % _rmsWindowSize] = smpSq;
               rmsSquared += smpSq;

               // rmsAvg = rmsAvg + (rms - rmsAvg) * _rmsAvgSlew;
               // rmsAvg = Dyac_denorm_32(rmsAvg);

               // if(rms > rmsPeak)
               //    rmsPeak = rms;

               frameIdx++;
            }

            if(startOffsets->num_elements > endOffsets->num_elements)
            {
               // Finish last zone
               vi.value.int_val = frameIdx - 1;
               endOffsets->yacArraySet(NULL/*context*/, endOffsets->num_elements, &vi);
            }

            delete [] rmsHistory;
            // delete [] rmsPeakHistory;

         } // if smp
      } // if retEndOffsets
   } // if retStartOffsets
}

   // sUI numFrames = _fa->yacArrayGetNumElements() / _numChannels;
   // if(numFrames < 2u)
   //    return _off;  // sample too short

   // const sF32 *fa = (const sF32*)_fa->yacArrayGetPointer();
   // sUI k = frameIdx * _numChannels + _chOff;
   // if(sABS(fa[k]) <= _distThreshold)
   //    return _off;

sBool tksampleedit_is_zerocrossing(YAC_Object *_fa, sUI _chOff, sUI _numChannels, sUI _off) {
   const sUI numFrames = _fa->yacArrayGetNumElements() / _numChannels;
   if(numFrames < 2u)
      return YAC_TRUE;  // sample too short
   const sF32 *fa = (const sF32*)_fa->yacArrayGetPointer();
   const sUI k = _off * _numChannels + _chOff;
   const sF32 fCur = fa[k];
   if(k >= _numChannels)
   {
      const sF32 fPrev = fa[k - _numChannels];
      if(fPrev <= 0.0f && fCur >= 0.0f)
         return YAC_TRUE;
      if(fPrev >= 0.0f && fCur <= 0.0f)
         return YAC_TRUE;
   }
   if(_off + 1u < numFrames)
   {
      const sF32 fNext = fa[k + _numChannels];
      if(fNext <= 0.0f && fCur >= 0.0f)
         return YAC_TRUE;
      if(fNext >= 0.0f && fCur <= 0.0f)
         return YAC_TRUE;
   }
   return YAC_FALSE;
}

sUI tksampleedit_find_zerocrossing_near(YAC_Object *_fa, sUI _chOff, sUI _numChannels, sUI _off, sF32 _distThreshold) {
   sUI offFwd = tksampleedit_find_edge_near_fwd(_fa, _chOff, _numChannels, _off, YAC_TRUE/*bRise*/, YAC_TRUE/*bFall*/, _distThreshold);
   sUI offBwd = tksampleedit_find_edge_near_bwd(_fa, _chOff, _numChannels, _off, YAC_TRUE/*bRise*/, YAC_TRUE/*bFall*/, _distThreshold);
   sUI r = _off;
   if(offFwd != _off)
   {
      if(offBwd != _off)
      {
         sUI distFwd = offFwd - _off;
         sUI distBwd = _off - offBwd;
         if(distFwd < distBwd)
            r = offFwd;
         else
            r = offBwd;
      }
      else
      {
         r = offFwd;
      }
   }
   else
   {
      r = offBwd;
   }
   return r;
}

sUI tksampleedit_find_edge_near_fwd(YAC_Object *_fa, sUI _chOff, sUI _numChannels, sUI _off, sBool _bRise, sBool _bFall, sF32 _distThreshold) {
   const sUI numFrames = _fa->yacArrayGetNumElements() / _numChannels;
   if(numFrames < 2u)
      return _off;  // sample too short
   const sF32 *fa = (const sF32*)_fa->yacArrayGetPointer();
   sUI frameIdx = _off + 1u;
   sUI k = frameIdx * _numChannels + _chOff;
   sF32 lastVal = fa[k - _numChannels];

   while(frameIdx < numFrames)
   {
      sF32 cVal = fa[k];
      sF32 dist = cVal - lastVal;
      dist = sABS(dist);

      if(dist >= _distThreshold)
      {
         if( (_bRise && ((lastVal <= 0.0f) && (cVal >= 0.0f))) ||
             (_bFall && ((lastVal >= 0.0f) && (cVal <= 0.0f)))
             )
         {
            return frameIdx;
         }
      }

      // Next frame
      lastVal = cVal;
      frameIdx++;
      k += _numChannels;
   }

   // No zero-crossing found
   return _off;
}

sUI tksampleedit_find_edge_near_bwd(YAC_Object *_fa, sUI _chOff, sUI _numChannels, sUI _off, sBool _bRise, sBool _bFall, sF32 _distThreshold) {
   // // yac_host->printf("xxx tksampleedit_find_edge_near_bwd: off=%u\n", _off);

   const sUI numFrames = _fa->yacArrayGetNumElements() / _numChannels;
   if(numFrames < 2u)
      return _off;  // sample too short
   const sF32 *fa = (const sF32*)_fa->yacArrayGetPointer();
   sUI frameIdx = _off - 1;
   sUI k = frameIdx * _numChannels + _chOff;
   sF32 lastVal = fa[k + _numChannels];

   while(frameIdx < numFrames)  // >= 0 ?
   {
      sF32 cVal = fa[k];
      sF32 dist = cVal - lastVal;
      dist = sABS(dist);

      if(dist >= _distThreshold)
      {
         if( (_bFall && ((lastVal <= 0.0f) && (cVal >= 0.0f))) ||
             (_bRise && ((lastVal >= 0.0f) && (cVal <= 0.0f)))
             )
         {
            // // yac_host->printf("xxx tksampleedit_find_edge_near_bwd: return %u\n", frameIdx+1u);
            return frameIdx + 1u;
         }
      }

      // Next frame
      lastVal = cVal;
      frameIdx--;
      k -= _numChannels;
   }

   // No zero-crossing found
   // // yac_host->printf("xxx tksampleedit_find_edge_near_bwd: none found, return off=%u\n", _off);
   return _off;
}

sUI tksampleedit_find_edge_near(YAC_Object *_fa, sUI _chOff, sUI _numChannels, sUI _off, sBool _bRise, sBool _bFall, sF32 _distThreshold) {
   // unused ?
   const sUI offFwd = tksampleedit_find_edge_near_fwd(_fa, _chOff, _numChannels, _off, _bRise, _bFall, _distThreshold);
   const sUI offBwd = tksampleedit_find_edge_near_bwd(_fa, _chOff, _numChannels, _off, _bRise, _bFall, _distThreshold);
   sUI r = _off;

   if(offFwd != _off)
   {
      if(offBwd != _off)
      {
         sUI distFwd = offFwd - _off;
         sUI distBwd = _off - offBwd;
         if(distFwd < distBwd)
            r = offFwd;
         else
            r = offBwd;
      }
      else
      {
         r = offFwd;
      }
   }
   else
   {
      r = offBwd;
   }
   return r;
}

sSI YAC_CALL tksampleedit_find_min_threshold_fwd(YAC_Object *_fa, sUI _chOff, sUI _numChannels, sF32 _min) {
   // sSI r = -1;
   const sUI numFrames = _fa->yacArrayGetNumElements() / _numChannels;
   const sF32 *fa = (const sF32*)_fa->yacArrayGetPointer();
   sUI frameIdx = 0u;
   sUI k = _chOff;
   while(frameIdx < numFrames)
   {
      if(fa[k] >= _min)
         return sSI(frameIdx);
      k += _numChannels;
      frameIdx++;
   }
   return -1;
}

sSI YAC_CALL tksampleedit_find_min_threshold_bwd(YAC_Object *_fa, sUI _chOff, sUI _numChannels, sF32 _min) {
   // sSI r = -1;
   const sUI numFrames = _fa->yacArrayGetNumElements() / _numChannels;
   if(numFrames > 0u)
   {
      const sF32 *fa = (const sF32*)_fa->yacArrayGetPointer();
      sUI frameIdx = numFrames - 1u;
      sUI k = frameIdx * _numChannels + _chOff;
      while(frameIdx < numFrames)
      {
         if(fa[k] >= _min)
            return sSI(frameIdx);
         k -= _numChannels;
         frameIdx--;
      }
   }
   return -1;
}

sUI tksampleedit_find_max_near(YAC_Object *_fa, sUI _chOff, sUI _numChannels, sUI _off, sUI _frameRange) {
   sUI offFwd = tksampleedit_find_max_near_fwd(_fa, _chOff, _numChannels, _off, _frameRange);
   sUI offBwd = tksampleedit_find_max_near_bwd(_fa, _chOff, _numChannels, _off, _frameRange);
   sUI r = _off;
   if(offFwd != _off)
   {
      if(offBwd != _off)
      {
         sUI distFwd = offFwd - _off;
         sUI distBwd = _off - offBwd;
         if(distFwd < distBwd)
            r = offFwd;
         else
            r = offBwd;
      }
      else
      {
         r = offFwd;
      }
   }
   else
   {
      r = offBwd;
   }
   return r;
}

sUI tksampleedit_find_max_near_fwd (YAC_Object *_fa, sUI _chOff, sUI _numChannels, sUI _off, sUI _frameRange) {
   const sUI numFrames = _fa->yacArrayGetNumElements() / _numChannels;
   const sF32 *fa = (const sF32*)_fa->yacArrayGetPointer();
   // // sUI frameIdx = 0u;
   sUI frameIdx = _off;
   sUI k = _chOff + (_off * _numChannels);
   const sUI frameLimit = sMIN(numFrames, _off + _frameRange);
   sF32 cMax = 0.0f;
   sUI maxOff = _off;
   while(frameIdx < frameLimit)
   {
      if(fa[k] > cMax)
      {
         cMax = fa[k];
         maxOff = frameIdx;
      }
      k += _numChannels;
      frameIdx++;
   }
   return maxOff;
}

sUI tksampleedit_find_absmax_near_fwd (YAC_Object *_fa, sUI _chOff, sUI _numChannels, sUI _off, sUI _frameRange) {
   sUI numFrames = _fa->yacArrayGetNumElements() / _numChannels;
   const sF32 *fa = (const sF32*)_fa->yacArrayGetPointer();
   // // sUI frameIdx = 0u;
   sUI frameIdx = _off;
   sUI k = _chOff + (_off * _numChannels);
   sUI frameLimit = sMIN(numFrames, _off + _frameRange);
   sF32 cMax = 0.0f;
   sUI maxOff = _off;
   while(frameIdx < frameLimit)
   {
      sF32 f = fa[k];
      if(f < 0.0f)
         f = -f;
      if(f > cMax)
      {
         cMax = f;
         maxOff = frameIdx;
      }
      k += _numChannels;
      frameIdx++;
   }
   return maxOff;
}

sUI tksampleedit_find_max_near_bwd (YAC_Object *_fa, sUI _chOff, sUI _numChannels, sUI _off, sUI _frameRange) {
   sUI numFrames = _fa->yacArrayGetNumElements() / _numChannels;
   sUI maxOff = _off;
   if(numFrames > 0u)
   {
      const sF32 *fa = (const sF32*)_fa->yacArrayGetPointer();
      sUI frameIdx = numFrames - 1u;
      sUI k = frameIdx * _numChannels + _chOff;
      sUI frameLimit = sMAX(0, _off - _frameRange);
      sF32 cMax = 0.0f;
      while((frameIdx < numFrames) && (frameIdx >= frameLimit))
      {
         if(fa[k] >= cMax)
         {
            cMax = fa[k];
            maxOff = frameIdx;
         }
         k -= _numChannels;
         frameIdx--;
      }
   }
   return maxOff;
}

sUI tksampleedit_find_min_near_fwd (YAC_Object *_fa, sUI _chOff, sUI _numChannels, sUI _off, sUI _frameRange) {
   sUI numFrames = _fa->yacArrayGetNumElements() / _numChannels;
   const sF32 *fa = (const sF32*)_fa->yacArrayGetPointer();
   // // sUI frameIdx = 0u;
   sUI frameIdx = _off;
   sUI k = _chOff + (_off * _numChannels);
   sUI frameLimit = sMIN(numFrames, _off + _frameRange);
   sF32 cMax = 0.0f;
   sUI maxOff = _off;
   while(frameIdx < frameLimit)
   {
      if(fa[k] < cMax)
      {
         cMax = fa[k];
         maxOff = frameIdx;
      }
      k += _numChannels;
      frameIdx++;
   }
   return maxOff;
}
