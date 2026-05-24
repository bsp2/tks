// ----
// ---- file   : StRange.cpp
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : Distributed under terms of the MIT license (https://opensource.org/licenses/MIT)
// ----          Copyright 2009-2026 by bsp
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
// ---- created: 23Sep2009
// ---- changed: 21Jan2010, 23Dec2018, 05Jan2019, 06Jan2019, 05Mar2019, 12Apr2023, 03Oct2024
// ----          24May2026
// ----
// ----
// ----

#include "tksampler.h"


StRange::StRange(void) {
   lo = hi = 0.0f;
   fade_in = fade_out = 0.0f;

   b_noteon_filter = YAC_TRUE;
   b_high_inclusive = YAC_FALSE;
}

StRange::~StRange() {
}

sF32 StRange::_getLo (void) {
   return lo;
}

void StRange::_setLo (sF32 _lo) {
   lo = _lo;
}

sF32 StRange::_getHi (void) {
   return hi;
}

void StRange::_setHi (sF32 _hi) {
   hi = _hi;
}

sF32 StRange::_getFadeIn(void) {
   return fade_in;
}

void StRange::_setFadeIn(sF32 _off) {
   fade_in = _off;
}

sF32 StRange::_getFadeOut(void) {
   return fade_out;
}

void StRange::_setFadeOut(sF32 _off) {
   fade_out = _off;
}

void StRange::_setEnableNoteOnFilter(sSI _bEnabled) {
   b_noteon_filter = _bEnabled;
}

sBool StRange::_getEnableNoteOnFilter(void) {
   return b_noteon_filter;
}

sBool StRange::_isNoteOnFilterEnabled(void) {
   // (TODO) remove this method
   Dyac_host_printf("[---] StRange::_isNoteOnFilterEnabled: **DEPRECATED**\n");
   return b_noteon_filter;
}

void StRange::_setEnableHighInclusive(sSI _bEnabled) {
   b_high_inclusive = _bEnabled;
}

sBool StRange::_getEnableHighInclusive(void) {
   return b_high_inclusive;
}

sF32 StRange::calcAmount(sF32 _val) {
   // 1=fully faded in, 0=faded out
   if(b_noteon_filter)
      return 1.0f;

   // // Dyac_host_printf("xxx calcAmount: lo=%f hi=%f fade_in=%f fade_out=%f val=%f\n", lo, hi, fade_in, fade_out, _val);

   sF32 cLo = (lo - fade_in);
   sF32 cFadeIn = fade_in;

   sF32 cHi = (hi + fade_out);
   sF32 cFadeOut = fade_out;

   // // Dyac_host_printf("xxx calcAmount: cLo=%f cHi=%f cFadeIn=%f cFadeOut=%f val=%f\n", cLo, cHi, cFadeIn, cFadeOut, _val);

   if(_val < cLo)
      return 0.0f;

   if(b_high_inclusive ? (_val > cHi) : (_val >= cHi))
      return 0.0f;

   // // Dyac_host_printf("xxx 2 calcAmount: cLo=%f cHi=%f cFadeIn=%f cFadeOut=%f val=%f\n", cLo, cHi, cFadeIn, cFadeOut, _val);

   if((cFadeIn > 0.0f) && (_val < (cLo+cFadeIn)))
   {
      // Fade in
      sF32 t = (_val-cLo) / cFadeIn;
      t = t*t*(3.0f-2.0f*t);
      // // Dyac_host_printf("xxx calcAmount: fade-in, t=%f\n", t);
      return t;
   }
   else if((cFadeOut > 0.0f) && (_val >= (cHi-cFadeOut)))
   {
      // Fade out
      sF32 t = (1.0f - ((_val - (cHi-cFadeOut)) / cFadeOut));
      t = t*t*(3.0f-2.0f*t);
      // // Dyac_host_printf("xxx calcAmount: fade-out, t=%f\n", t);
      return t;
   }
   else
   {
      // Fully faded in
      return 1.0f;
   }
}

sBool StRange::filterNoteOn(sF32 _val) {
   if(b_noteon_filter)
      return (_val >= lo) && (b_high_inclusive ? (_val <= hi) : (_val < hi));
   return YAC_TRUE;
}
