// ----
// ---- file   : AR_Kit.h
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2016-2019 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : This is part of the "tkanalogrytm" plugin.
// ----
// ---- changed: 28Feb2016, 01Mar2016, 03Mar2016, 29Mar2016, 21Nov2016, 08Jul2017, 09Jul2017
// ----          21Aug2017, 19Nov2018, 21Oct2019, 25Oct2019
// ----
// ----
// ----

#ifndef __TK_AR_KIT_H__
#define __TK_AR_KIT_H__

YG("analogrytm")


/* @class AR_Kit,Object
 */
YC class AR_Kit : public YAC_Object {
  public:
   sU8 syx[AR_KIT_SYX_MAX_SZ];
   sU8 raw[AR_KIT_MAX_SZ];

   sU8 *current_buf; // points to either syx or raw buffer, whichever was last written

   sU8 ar_kit_version;  // 1=v1 (syx sz=3095)  2=v2 (syx sz=3041), 3=v3 (syx sz=3079) 4=v4 (syx sz=3081), 5=v5 (syx sz=2998)

  public:
   AR_Kit(void);
   ~AR_Kit();

   YAC(AR_Kit);

  public:
   static sBool CanMix (AR_Kit *_a, AR_Kit *_b);
   static sU8 GetRAWVersion (AR_Kit *_a);

  public:
   void YAC_VCALL yacOperator (sSI _cmd, YAC_Object *_robj, YAC_Value *_r);


   YM sUI getRequiredSizeSyx (void);
   YM sUI getRequiredSizeRaw (void);

   // 0=none, 1=syx, 2=raw
   YM sSI getCurrentDataIdx (void);

   YM void saveSyxToStream (YAC_Object *_ofs);
   YM void saveRawToStream (YAC_Object *_ofs);

   YM sBool loadSyxFromStream (YAC_Object *_ifs);
   YM sBool loadRawFromStream (YAC_Object *_ifs);

   YM sBool addKitRequestX (YAC_Buffer *_d, sUI _devId, sUI _kitNr);

   YM sBool syxToRaw (YAC_Buffer *_s);

   YM sBool sendKitX (YAC_Buffer *_d, sUI _devId, sUI _kitNr);

   YM sBool sendSoundX (YAC_Buffer *_d, sUI _devId, sUI _trackIdx);

   // Return reference to 'raw' array in 'ret'
   YM sBool getRawBuffer (YAC_Buffer *_ret);

   YM sUI getVersion (void);

   YM sBool copyPerfCtlFrom (AR_Kit *_o);
   YM sBool copySceneCtlFrom (AR_Kit *_o);
   YM sBool copyFXFrom (AR_Kit *_o);
   YM sBool copyTrackSoundFrom (sUI _dstTrackIdx, AR_Kit *_o, sUI _srcTrackIdx);
   YM sBool copyTrackSoundFromSound (sUI _dstTrackIdx, AR_Sound *_o);

   YM sUI  getTrackSampleNr (sUI _trackIdx);
   YM void setTrackSampleNr (sUI _trackIdx, sUI _sampleNr);

   // retARSound will reference kit raw buffer
   YM sBool getSoundRefByIndex (sUI _trackIdx, YAC_Object *_retARSound);

   YM sUI  getTrackLevel (sUI _trackIdx);
   YM void setTrackLevel (sUI _trackIdx, sUI _level);
};


#endif // __TK_AR_KIT_H__
