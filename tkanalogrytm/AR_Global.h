// ----
// ---- file   : AR_Global.h
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2023 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : This is part of the "tkanalogrytm" plugin.
// ----
// ---- created: 01Dec2023
// ---- changed:
// ----
// ----
// ----

#ifndef __TK_AR_GLOBAL_H__
#define __TK_AR_GLOBAL_H__

YG("analogrytm")


/* @class AR_Global,Object
 */
YC class AR_Global : public YAC_Object {

   // (note) do not remove the following lines (parsed by YInG)
//#define AR_GLOBAL_SYX_SZ        YCI AR_GLOBAL_SYX_SZ
//#define AR_GLOBAL_SZ            YCI AR_GLOBAL_SZ

  public:
   sU8 syx[AR_GLOBAL_SYX_SZ];
   sU8 raw[AR_GLOBAL_SZ];

   sU8 *current_buf; // points to either syx or raw buffer, whichever was last written

  public:
   AR_Global(void);
   ~AR_Global();

   YAC(AR_Global);

  public:
   ar_global_t *getGlobalPtr (void);

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

   YM sBool addGlobalRequestX (YAC_Buffer *_d, sUI _devId, sUI _globalSlot);

   YM sBool syxToRaw (YAC_Buffer *_s);
   YM sBool rawToSyx (YAC_Buffer *_d, sUI _trackIdx);

   YM sBool sendGlobal (YAC_Buffer *_d, sUI _devId, sUI _globalSlot, sBool _bEditBuf);

   // Return reference to 'raw' array in 'ret'
   YM sBool getRawBuffer (YAC_Buffer *_ret);

   // Return reference to 'syx' array in 'ret'
   YM sBool getSyxBuffer (YAC_Buffer *_ret);

   // Pad destination
   YM void setPadDest (sUI _dst);
   YM sUI  getPadDest (void);
};


#endif // __TK_AR_GLOBAL_H__
