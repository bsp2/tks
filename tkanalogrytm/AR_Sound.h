// ----
// ---- file   : AR_Sound.h
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2017-2023 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : This is part of the "tkanalogrytm" plugin.
// ----
// ---- created: 21Aug2017
// ---- changed: 25Oct2019, 26Oct2019, 15Jul2021, 16Jul2021, 25Nov2023, 28Nov2023
// ----
// ----
// ----

#ifndef __TK_AR_SOUND_H__
#define __TK_AR_SOUND_H__

YG("analogrytm")


/* @class AR_Sound,Object
 */
YC class AR_Sound : public YAC_Object {

   // (note) do not remove the following lines (parsed by YInG)
//#define AR_SOUND_SYX_SZ        YCI AR_SOUND_SYX_SZ
//#define AR_SOUND_SZ            YCI AR_SOUND_SZ

  public:
   sU8 syx[AR_SOUND_SYX_SZ];
   sU8 raw[AR_SOUND_SZ];

   sU8 *current_buf; // points to either syx or raw buffer, whichever was last written

   sBool b_raw_buf_ref;  // true= current_buf is reference to ar_kit.track[x]

  public:
   AR_Sound(void);
   ~AR_Sound();

   YAC(AR_Sound);

  public:
   ar_sound_t *getSoundPtr (void);

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

   YM sBool addSoundRequestX (YAC_Buffer *_d, sUI _devId, sUI _soundNr);

   YM sBool syxToRaw (YAC_Buffer *_s);
   YM sBool rawToSyx (YAC_Buffer *_d, sUI _trackIdx);

   YM sBool copyFromKitTrack (YAC_Object *_kit, sUI _srcTrackIdx);

   YM sBool sendSound (YAC_Buffer *_d, sUI _devId, sUI _soundNr, sBool _bEditBuf);

   // Return reference to 'raw' array in 'ret'
   YM sBool getRawBuffer (YAC_Buffer *_ret);

   // Return reference to 'syx' array in 'ret'
   YM sBool getSyxBuffer (YAC_Buffer *_ret);

   // Get number of machine types (static)
   YM sUI GetNumMachines (void);

   // Get machine name by index (static)
   YM void GetMachineNameById (sUI _machineId, YAC_Value *_r);

   // Get compatible machine id by track and UI list index (static)
   YM sSI GetMachineIdByTrackAndListIndex (sUI _trackIdx, sUI _listIdx);

   YM sSI GetListIndexByTrackIndexAndMachineId (sUI _trackIdx, sUI _machineId);

   // Get machine (type index)
   YM sSI getMachine (void);

   // Set machine (type index)
   YM void setMachine (sUI _machine);

   // Get machine name
   YM void getMachineName (YAC_Value *_r);

   // Get 16bit machine parameter value
   //  (note) most params use only the MSB
   YM sSI getMachineParam (sUI _paramIdx);

   // Set 16bit machine parameter value
   //  (note) most params use only the MSB
   YM void setMachineParam (sUI _paramIdx, sUI _value);

   // Get machine parameter name
   YM void getMachineParamShortName (sUI _paramIdx, YAC_Value *_r);

   // Velocity to volume (0=off or 1=on)
   YM sBool getVelToVol (void);
   YM void  setVelToVol (sBool _value);

   // Accent level
   YM sUI  getAccentLevel (void);
   YM void setAccentLevel (sUI _v);

   // Chromatic mode (0=off, 1=synth, 2=sample, 3=syn+smp)
   YM sUI  getChromaticMode (void);
   YM void setChromaticMode (sUI _value);

   // Legacy FX send (0=off or 1=on)
   YM sBool getLegacyFXSend (void);
   YM void  setLegacyFXSend (sBool _bEnable);

   // Env reset (0=off or 1=on/filter)
   YM sBool getEnvReset (void);
   YM void  setEnvReset (sBool _bEnable);

   // Get number of LFO destinations (#UI list indices)
   YM sUI GetNumLfoDestUI (void);

   // Get LFO destination name (machine-dependent)
   YM void getLfoDestName (YAC_Value *_r);
   YM void getLfoDestNameByListIdx (sUI _listIdx, YAC_Value *_r);

   YM sSI  getLfoDestListIdx   (void);
   YM void setLfoDestByListIdx (sUI _idx);

   // Get number of (velocity/aftertouch) mod destinations (#UI list indices)
   YM sUI GetNumModDestUI (void);
   YM void getModDestNameByListIdx (sUI _listIdx, YAC_Value *_r);

   // Get velocity mod destination name (machine-dependent) (4 slots)
   YM void getVelocityModDestName (sUI _slotIdx, YAC_Value *_r);
   YM sSI  getVelocityModDestListIdx   (sUI _slotIdx);
   YM void setVelocityModDestByListIdx (sUI _slotIdx, sUI _idx);
   // in range 0..255, 2s complement (0x80=-128, 0x7F=+127)
   YM sSI  getVelocityModAmt (sUI _slotIdx);
   YM void setVelocityModAmt (sUI _slotIdx, sSI _amt);

   // Get aftertouch mod destination name (machine-dependent) (4 slots)
   YM void getAftertouchModDestName (sUI _slotIdx, YAC_Value *_r);
   YM sSI  getAftertouchModDestListIdx   (sUI _slotIdx);
   YM void setAftertouchModDestByListIdx (sUI _slotIdx, sUI _idx);
   // in range 0..255, 2s complement (0x80=-128, 0x7F=+127)
   YM sSI  getAftertouchModAmt (sUI _slotIdx);
   YM void setAftertouchModAmt (sUI _slotIdx, sSI _amt);

   YM sUI getSmpTUN (void);
   YM sUI getSmpFIN (void);
   YM sUI getSmpBR  (void);
   YM sUI getSmpSMP (void);

   // STA/END have 16bit precision since FW1.70 (0..65535)
   YM sUI getSmpSTA (void);
   YM sUI getSmpEND (void);
   YM sUI getSmpLOP (void);
   YM sUI getSmpLEV (void);

   YM void setSmpTUN (sUI _v);
   YM void setSmpFIN (sUI _v);
   YM void setSmpBR  (sUI _v);
   YM void setSmpSMP (sUI _v);

   YM void setSmpSTA (sUI _v);
   YM void setSmpEND (sUI _v);
   YM void setSmpLOP (sUI _v);
   YM void setSmpLEV (sUI _v);

   YM sUI getFltATK (void);
   YM sUI getFltDEC (void);
   YM sUI getFltSUS (void);
   YM sUI getFltREL (void);

   YM sUI getFltFRQ (void);
   YM sUI getFltRES (void);
   YM sUI getFltTYP (void);
   YM sUI getFltENV (void);

   YM void setFltATK (sUI _v);
   YM void setFltDEC (sUI _v);
   YM void setFltSUS (sUI _v);
   YM void setFltREL (sUI _v);

   YM void setFltFRQ (sUI _v);
   YM void setFltRES (sUI _v);
   YM void setFltTYP (sUI _v);
   YM void setFltENV (sUI _v);

   YM sUI getAmpATK (void);
   YM sUI getAmpHLD (void);
   YM sUI getAmpDEC (void);
   YM sUI getAmpOVR (void);

   YM sUI getAmpDEL (void);
   YM sUI getAmpREV (void);
   YM sUI getAmpPAN (void);
   YM sUI getAmpVOL (void);

   YM void setAmpATK (sUI _v);
   YM void setAmpHLD (sUI _v);
   YM void setAmpDEC (sUI _v);
   YM void setAmpOVR (sUI _v);

   YM void setAmpDEL (sUI _v);
   YM void setAmpREV (sUI _v);
   YM void setAmpPAN (sUI _v);
   YM void setAmpVOL (sUI _v);

   YM sUI getLfoSPD (void);
   YM sUI getLfoMUL (void);
   YM sUI getLfoFAD (void);
   YM sUI getLfoDST (void);

   YM sUI getLfoWAV (void);
   YM sUI getLfoSPH (void);
   YM sUI getLfoMOD (void);
   YM sUI getLfoDEP (void);  // 16bit

   YM void setLfoSPD (sUI _v);
   YM void setLfoMUL (sUI _v);
   YM void setLfoFAD (sUI _v);
   YM void setLfoDST (sUI _v);

   YM void setLfoWAV (sUI _v);
   YM void setLfoSPH (sUI _v);
   YM void setLfoMOD (sUI _v);
   YM void setLfoDEP (sUI _v);  // 16bit
};


#endif // __TK_AR_SOUND_H__
