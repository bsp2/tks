// ----
// ---- file   : StRange.h
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2009-2024 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : This is part of the "syntracker" midi sequencer.
// ----
// ---- created: 23Sep2009
// ---- changed: 23Dec2018, 06Jan2019, 03Oct2024
// ----
// ----
// ----

#ifndef ST_RANGE_H__
#define ST_RANGE_H__

YG("tksampler")


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
