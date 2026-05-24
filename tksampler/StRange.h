// ----
// ---- file   : StRange.h
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
// ---- changed: 23Dec2018, 06Jan2019, 03Oct2024, 24May2026
// ----
// ----
// ----

#ifndef ST_RANGE_H__
#define ST_RANGE_H__

YG("tksampler")


/* @class StRange,Object
 */
YC class StRange : public YAC_Object {
  public:
   sF32 lo;
   sF32 hi;
   sF32 fade_in;
   sF32 fade_out;

   sBool b_noteon_filter; // true=filter samples whose initial note/vel and/or mod is out of range
   sBool b_high_inclusive;  // true: (lo <= val <= hi), false: (lo <= val < hi)  (enabled for key ranges, disabled for mod+vel ranges)

  public:
   StRange(void);
   ~StRange();

   YAC(StRange);

   sBool filterNoteOn(sF32 _val);


   YM sF32 _getLo (void);
   YM void _setLo (sF32 _lo);

   YM sF32 _getHi (void);
   YM void _setHi (sF32 _hi);

   YM sF32 _getFadeIn (void);
   YM void _setFadeIn (sF32 _off);

   YM sF32 _getFadeOut (void);
   YM void _setFadeOut (sF32 _off);

   YM void  _setEnableNoteOnFilter (sSI _bEnabled);
   YM sBool _getEnableNoteOnFilter (void);
   YM sBool _isNoteOnFilterEnabled (void); // (TODO) remove this method

   YM void _setEnableHighInclusive (sSI _bEnabled);
   YM sBool _getEnableHighInclusive (void);

   YM sF32 calcAmount (sF32 _val); // 1=fully faded in, 0=faded out
};


#endif // ST_RANGE_H__
