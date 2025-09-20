// ----
// ---- file   : tkspeexdsp.cpp
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2020-2023 by Bastian Spiegel. 
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See 
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : libspeexdsp interface
// ----
// ---- created: 31Oct2020
// ---- changed: 31Jul2021, 18Jul2023
// ----
// ----
// ----

#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#define YAC_BIGSTRING
#define YAC_PRINTF
#include <yac.h>
YAC_Host *yac_host;

#define EXPORT
#include <config.h>
#include <speex/speex_resampler.h>


#include "tkspeexdsp.h"

#include "ying_speexdsp.h"
#include "ying_speexdsp_SpeexResampler.cpp"
#include "ying_speexdsp.cpp"


// Implement standard exception ID variables (see yac.h)
Dyac_std_exid_impl;


// ---------------------------------------------------------------------------- SpeexResampler
_SpeexResampler::_SpeexResampler(void) {
   srs = NULL;
}

_SpeexResampler::~_SpeexResampler() {
   exit();
}

sBool _SpeexResampler::init(sUI _numCh, sUI _sampleRateIn, sUI _sampleRateOut, sUI _quality) {
   exit();

   int err = 0;
   srs = speex_resampler_init(_numCh,
                              _sampleRateIn,
                              _sampleRateOut,
                              _quality,
                              &err
                              );

   return (NULL != srs);
}

sUI _SpeexResampler::GetOutputDelayInFramesByQualityAndFactor(sUI _quality, sF32 _factor) {
   sUI r = 0u;
   if(_factor < 1.0f)
      _factor = 1.0f;
   switch(_quality)
   {
      default:
      case  0u: r =   4u; break;
      case  1u: r =   8u; break;
      case  2u: r =  16u; break;
      case  3u: r =  24u; break;
      case  4u: r =  32u; break;
      case  5u: r =  40u; break;
      case  6u: r =  48u; break;
      case  7u: r =  64u; break;
      case  8u: r =  80u; break;
      case  9u: r =  96u; break;
      case 10u: r = 128u; break;
   }
   r = sUI(r * _factor);
   return r;
}

void _SpeexResampler::process(sUI _chIdx, YAC_Object *_faIn, sUI _numFramesIn, YAC_Object *_faOut, sUI _numFramesOut) {
   
   if(NULL != srs)
   {
      if(YAC_Is_FloatArray(_faIn))
      {
         if(YAC_Is_FloatArray(_faOut))
         {
            YAC_CAST_ARG(YAC_FloatArray, faIn, _faIn);
            YAC_CAST_ARG(YAC_FloatArray, faOut, _faOut);

            sUI maxFramesIn = faIn->yacArrayGetNumElements();
            sUI maxFramesOut = faOut->yacArrayGetNumElements();

            if(_numFramesIn > maxFramesIn)
               _numFramesIn = maxFramesIn;

            if(_numFramesOut > maxFramesOut)
               _numFramesOut = maxFramesOut;

            sF32 *s = (sF32*)faIn->yacArrayGetPointer();
            sF32 *d = (sF32*)faOut->yacArrayGetPointer();

            int err = speex_resampler_process_float(srs,
                                                    _chIdx,
                                                    s,
                                                    &_numFramesIn,
                                                    d,
                                                    &_numFramesOut
                                                    );
            // yac_host->printf("xxx err=%d numFramesOut=%u\n", err, _numFramesOut);
            (void)err;
         }
      }
   }
}

void _SpeexResampler::exit(void) {
   if(NULL != srs)
   {
      speex_resampler_destroy(srs);
      srs = NULL;
   }
}


// ---------------------------------------------------------------------------- YAC_Init
void YAC_CALL YAC_Init(YAC_Host *_host) {
	yac_host = _host;

   // Resolve "standard" exception IDs
   Dyac_std_exid_resolve;

   YAC_Init_speexdsp(_host);

   if(yac_host->yacGetDebugLevel() > 0)
      yac_host->printf("tkspeexdsp: init done.\n");
}

void YAC_CALL YAC_Exit(YAC_Host *_host) {
   YAC_Exit_speexdsp(_host);
}


#include <yac_host.cpp>
