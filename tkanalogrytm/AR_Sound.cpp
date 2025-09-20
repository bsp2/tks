// ----
// ---- file   : AR_Sound.cpp
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

#include "tkanalogrytm.h"


AR_Sound::AR_Sound(void) {
   current_buf = NULL;
   b_raw_buf_ref = YAC_FALSE;
}

AR_Sound::~AR_Sound() {
}

void YAC_VCALL AR_Sound::yacOperator(sSI _cmd, YAC_Object *_robj, YAC_Value *_r) {
   if(YAC_OP_ASSIGN == _cmd)
   {
      if(YAC_CHK(_robj, clid_AR_Sound))
      {
         YAC_CAST_ARG(AR_Sound, o, _robj);

         memcpy(syx, o->syx, AR_SOUND_SYX_SZ);
         memcpy((void*)getSoundPtr(), (void*)o->getSoundPtr(), AR_SOUND_SZ);

         if(o->current_buf == o->syx)
         {
            current_buf = syx;
            b_raw_buf_ref = YAC_FALSE;
         }
         else if(o->current_buf == o->raw)
         {
            if(current_buf == syx)
            {
               current_buf = raw;
               b_raw_buf_ref = YAC_FALSE;
            }
         }
         else if(o->b_raw_buf_ref)
         {
            if(current_buf == syx)
            {
               current_buf = raw;
               b_raw_buf_ref = YAC_FALSE;
            }
         }
         else
         {
            current_buf = NULL;
            b_raw_buf_ref = YAC_FALSE;
         }
      }
   }
   else
   {
      YAC_Object::yacOperator(_cmd, _robj, _r);
   }
}

sUI AR_Sound::getRequiredSizeSyx(void) {
   return AR_SOUND_SYX_SZ;
}

sUI AR_Sound::getRequiredSizeRaw(void) {
   return AR_SOUND_SZ;
}

sSI AR_Sound::getCurrentDataIdx(void) {
   if(syx == current_buf)
   {
      return 1;
   }
   else if(b_raw_buf_ref || (raw == current_buf))
   {
      return 2;
   }
   else
   {
      return 0;
   }
}

ar_sound_t *AR_Sound::getSoundPtr(void) {
   ar_sound_t *sound = (ar_sound_t*) (b_raw_buf_ref ? current_buf : raw);
   return sound;
}

void AR_Sound::saveSyxToStream(YAC_Object *_ofs) {
   if(YAC_IS_STREAM(_ofs))
   {
      _ofs->yacStreamWriteI16(1); // version

      _ofs->yacStreamWriteI16(AR_SOUND_SYX_SZ);

      for(sUI i=0; i < AR_SOUND_SYX_SZ; i++)
      {
         _ofs->yacStreamWriteI8(syx[i]);
      }
   }
}

void AR_Sound::saveRawToStream(YAC_Object *_ofs) {
   if(YAC_IS_STREAM(_ofs))
   {
      _ofs->yacStreamWriteI16(1); // version

      _ofs->yacStreamWriteI16(AR_SOUND_SZ);

      for(sUI i=0; i < AR_SOUND_SZ; i++)
      {
         _ofs->yacStreamWriteI8(raw[i]);
      }
   }
}

sBool AR_Sound::loadSyxFromStream(YAC_Object *_ifs) {
   if(YAC_IS_STREAM(_ifs))
   {
      sU16 ver = _ifs->yacStreamReadI16();

      if(ver >= 1u)
      {
         sU16 sz = _ifs->yacStreamReadI16();

         if(AR_SOUND_SYX_SZ == sz)
         {
            for(sUI i=0; i < AR_SOUND_SYX_SZ; i++)
            {
               syx[i] = _ifs->yacStreamReadI8();
            }

            current_buf = syx;

            // Succeeded
            return YAC_TRUE;
         }
         else
         {
            yac_host->printf("[---] AR_Sound::loadSyxFromStream: bad sz=%u\n", sz);
         }
      }
      else
      {
         yac_host->printf("[---] AR_Sound::loadSyxFromStream: bad ver=%u\n", ver);
      }
   }

   // Failed
   return YAC_FALSE;
}

sBool AR_Sound::loadRawFromStream(YAC_Object *_ifs) {
   if(YAC_IS_STREAM(_ifs))
   {
      sU16 ver = _ifs->yacStreamReadI16();

      if(ver >= 1u)
      {
         sU16 sz = _ifs->yacStreamReadI16();

         if(AR_SOUND_SZ == sz)
         {
            for(sUI i=0; i < AR_SOUND_SZ; i++)
            {
               raw[i] = _ifs->yacStreamReadI8();
            }

            current_buf = raw;

            // Succeeded
            return YAC_TRUE;
         }
         else
         {
            yac_host->printf("[---] AR_Sound::loadRawFromStream: bad sz=%u\n", sz);
         }
      }
      else
      {
         yac_host->printf("[---] AR_Sound::loadRawFromStream: bad ver=%u\n", ver);
      }
   }

   // Failed
   return YAC_FALSE;
}

sBool AR_Sound::addSoundRequestX(YAC_Buffer *_d, sUI _devId, sUI _soundNr) {
   sBool ret = YAC_FALSE;

   if(YAC_Is_Buffer(_d))
   {
      if((_d->io_offset + AR_SYSEX_REQUEST_MSG_SZ) <= _d->size)
      {
         if(AR_ERR_OK == ar_sound_request_x(_d->buffer + _d->io_offset, _devId&127u, _soundNr&127u))
         {
            _d->io_offset += AR_SYSEX_REQUEST_MSG_SZ;

            // Succeeded
            ret = YAC_TRUE;
         }
      }
   }

   return ret;
}

sBool AR_Sound::syxToRaw(YAC_Buffer *_s) {
   sBool ret = YAC_FALSE;

   if(YAC_Is_Buffer(_s))
   {
      sU8 *rawPtr = (sU8*) getSoundPtr();

      yac_host->printf("[trc] AR_Sound::syxToRaw: _s->size=%u\n", _s->size);

      if(AR_SOUND_SYX_V4_SZ/*207*/ == _s->size)
      {
         // old FW1.50..1.61b SysEx buffer => convert to v5
         sU8 *rawPtrOld = new(std::nothrow)sU8[AR_SOUND_V4_SZ/*168*/];
         if(NULL != rawPtrOld)
         {
            const sU8 *srcBuf = _s->buffer;
            sU32 srcBufSz = _s->io_offset;
            sU32 retRawBufSzOld = AR_SOUND_V4_SZ;
            ar_sysex_meta_t metaOld;
            if(AR_ERR_OK == ar_sysex_to_raw(rawPtrOld, &srcBuf, &srcBufSz, &retRawBufSzOld, &metaOld))
            {
               yac_host->printf("[dbg] AR_Sound::syxToRaw: converted old V4 syx to raw, retRawBufSzOld=%u.\n", retRawBufSzOld);

               sU32 retRawBufSz = AR_SOUND_SZ;

               // Write V5 header(?)
               rawPtr[0x00u] = 0xBEu;
               rawPtr[0x01u] = 0xEFu;
               rawPtr[0x02u] = 0xBAu;
               rawPtr[0x03u] = 0xCEu;
               rawPtr[0x04u] = 0x00u;
               rawPtr[0x05u] = 0x00u;
               rawPtr[0x06u] = 0x00u;
               rawPtr[0x07u] = 0x04u;
               rawPtr[0x08u] = 0x00u;
               rawPtr[0x09u] = 0x00u;
               rawPtr[0x0Au] = 0x00u;
               rawPtr[0x0Bu] = 0x00u;

               // Copy data until new "mode_flags" field
               memcpy((void*)(rawPtr + 0x0Cu), (const void*)(rawPtrOld + 0x0Cu), (0x7Du - 0x0Cu));

               // cb classic or metallic ?
               if( (12u == rawPtrOld[0x7Cu]) || (20u == rawPtrOld[0x7Cu]) )
               {
                  // Set new pulse width parameters to center pos (64=+0)
                  rawPtr[0x24u] = 64u;
                  rawPtr[0x26u] = 64u;
               }
               else if(7u == rawPtrOld[0x7Cu]) // bt classic ?
               {
                  // Init new SWD parameter
                  rawPtr[0x28u] = 0u;
               }

               sU8 modeFlags = 0u;

               // V4 vel_to_vol (0 or 1) => modeFlags
               if(rawPtrOld[0x7Du])
                  modeFlags |= (1u << 6);

               // V4 chromatic mode (2 bits) => modeFlags
               if(rawPtrOld[0x7Eu])
                  modeFlags |= rawPtrOld[0x7Eu] << 4;

               // V4 legacy fx send => modeFlags
               if(rawPtrOld[0x7Fu])
                  modeFlags |= (1u << 2);

               // V4 env reset filter => modeFlags
               if(rawPtrOld[0x80u])
                  modeFlags |= (1u << 1);

               // write modeFlags to V5 ar_sound_t data
               rawPtr[0x7Du] = modeFlags;

               // Zero unknown data at $7E..$8D
               memset((void*)(rawPtr + 0x7Eu), 0, (0x8Eu - 0x7Eu));

               // Copy vel+aftertouch modulation (vel_amt_1 .. at_target_4)
               memcpy((void*)(rawPtr + 0x8Eu), (void*)(rawPtrOld + 0x94u), 16u);

               // Zero unknown data at $9E..$A1
               rawPtr[0x9Eu] = 0u;
               rawPtr[0x9Fu] = 0u;
               rawPtr[0xA0u] = 0u;
               rawPtr[0xA1u] = 0u;

               yac_host->printf("[dbg] AR_Sound::syxToRaw: converted old V4 raw to V5 raw, retRawBufSz=%u.\n", retRawBufSz);

               // Succeeded
               ret = YAC_TRUE;

               current_buf = rawPtr;
            }
         }
         else
         {
            yac_host->printf("[---] AR_Sound::syxToRaw: failed to allocate V4 raw buffer.\n");
         }
      }
      else if(_s->size >= AR_SOUND_SYX_SZ)
      {
         const sU8 *srcBuf = _s->buffer;
         sU32 srcBufSz = _s->io_offset;
         sU32 retRawBufSz = AR_SOUND_SZ;
         ar_sysex_meta_t meta;

         if(AR_ERR_OK == ar_sysex_to_raw(rawPtr, &srcBuf, &srcBufSz, &retRawBufSz, &meta))
         {
            // Succeeded
            ret = YAC_TRUE;

            yac_host->printf("[dbg] AR_Sound::syxToRaw: OK, retRawBufSz=%u.\n", retRawBufSz);

            current_buf = rawPtr;
         }
         else
         {
            yac_host->printf("[---] AR_Sound::syxToRaw: ar_sysex_to_raw() failed.\n");
            current_buf = NULL;
         }
      }
      else
      {
         yac_host->printf("[---] AR_Sound::syxToRaw: source buffer too small (have=%u, need=%u).\n", _s->size, AR_SOUND_SYX_SZ);
         current_buf = NULL;
      }
   }

   return ret;
}

sBool AR_Sound::rawToSyx(YAC_Buffer *_d, sUI _trackIdx) {
   sBool ret = YAC_FALSE;

   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      if(YAC_Is_Buffer(_d))
      {
         sUI reqSizeSyx = getRequiredSizeSyx();
         if(_d->size >= reqSizeSyx)
         {
            const sU8 *rawPtr = (sU8*) getSoundPtr();
            sUI reqSizeRaw = getRequiredSizeRaw();
            _d->io_offset = 0u;
            ar_sysex_meta_t meta;
            AR_SYSEX_META_INIT(meta, 0/*devId*/, AR_TYPE_SOUND, ((true/*bEditBuf*/ ? 128u : 0u) + (_trackIdx & 127u)));
            if(AR_ERR_OK == ar_sound_raw_to_syx(_d->buffer, rawPtr, reqSizeRaw, &_d->io_offset, &meta))
            {
               ret = YAC_TRUE;
            }
         }
      }
   }
   return ret;
}

sBool AR_Sound::copyFromKitTrack(YAC_Object *_kit, sUI _srcTrackIdx) {
   if(YAC_CHK(_kit, clid_AR_Kit))
   {
      YAC_CAST_ARG(AR_Kit, o, _kit);

      if(2u == o->ar_kit_version)
      {
         const ar_kit_t *datO = (const ar_kit_t *)o->raw;
         ar_sound_t *dat = (ar_sound_t *)raw;

         b_raw_buf_ref = YAC_FALSE;
         current_buf = raw;

         memcpy((void*)dat, &datO->tracks[_srcTrackIdx % 12], sizeof(ar_sound_t));

         return YAC_TRUE;
      }
   }
   return YAC_FALSE;
}

sBool AR_Sound::sendSound(YAC_Buffer *_d, sUI _devId, sUI _soundNr, sBool _bEditBuf) {
   sBool ret = YAC_FALSE;

   if(YAC_Is_Buffer(_d))
   {
      if(_d->size >= AR_SOUND_SYX_SZ)
      {
         sU8 *dstBuf = _d->buffer;
         sU32 retSyxBufSz = 0;
         ar_sysex_meta_t meta;

         AR_SYSEX_META_INIT(meta, _devId, AR_TYPE_SOUND, ((_bEditBuf ? 128u : 0u) + (_soundNr & 127u)));

         if(b_raw_buf_ref || (current_buf == raw))
         {
            const sU8 *rawPtr = (sU8*)getSoundPtr();

            if(AR_ERR_OK == ar_raw_to_sysex(dstBuf, rawPtr, AR_SOUND_SZ, &retSyxBufSz, &meta))
            {
               _d->io_offset = retSyxBufSz;

               // Succeeded
               ret = YAC_TRUE;

               yac_host->printf("[dbg] AR_Sound::sendSound: OK, retSyxBufSz=%u.\n", retSyxBufSz);
            }
            else
            {
               yac_host->printf("[---] AR_Sound::sendSound: ar_raw_to_sysex() failed\n");
            }
         }
         else
         {
            yac_host->printf("[---] AR_Sound::sendSound: current_buf != raw\n");
         }
      }
      else
      {
         yac_host->printf("[---] AR_Sound::sendSound: dest buffer too small (have=%u, need=%u).\n", _d->size, AR_SOUND_SYX_SZ);
      }

   }

   return ret;
}

sBool AR_Sound::getRawBuffer(YAC_Buffer *_ret) {
   sBool ret = YAC_FALSE;

   if(YAC_Is_Buffer(_ret))
   {
      // Free previous contents (if any)
      _ret->yacArrayAlloc(0, 0, 0, 0);

      _ret->size      = AR_SOUND_SZ;
      _ret->io_offset = AR_SOUND_SZ;
      _ret->buffer    = (sU8*)getSoundPtr();
      _ret->deleteme  = YAC_FALSE;

      ret = YAC_TRUE;
   }

   return ret;
}

sBool AR_Sound::getSyxBuffer(YAC_Buffer *_ret) {
   sBool ret = YAC_FALSE;

   if(YAC_Is_Buffer(_ret))
   {
      // Free previous contents (if any)
      _ret->yacArrayAlloc(0, 0, 0, 0);

      _ret->size      = getRequiredSizeSyx();
      _ret->io_offset = _ret->size;
      _ret->buffer    = syx;
      _ret->deleteme  = YAC_FALSE;

      ret = YAC_TRUE;
   }

   return ret;
}

sUI AR_Sound::GetNumMachines(void) {
   return AR_NUM_SOUND_MACHINES;
}

void AR_Sound::GetMachineNameById(sUI _machineId, YAC_Value *_r) {
   if(_machineId < AR_NUM_SOUND_MACHINES)
   {
      _r->initEmptyString();
      _r->value.string_val->visit((const char*)ar_sound_machine_names[_machineId]);
   }
}

sSI AR_Sound::GetMachineIdByTrackAndListIndex(sUI _trackIdx, sUI _listIdx) {
   return ar_sound_get_machine_id_by_track_and_list_idx(_trackIdx, _listIdx);
}

sSI AR_Sound::GetListIndexByTrackIndexAndMachineId(sUI _trackIdx, sUI _machineId) {
   return ar_sound_get_list_index_by_track_idx_and_machine_id(_trackIdx, _machineId);
}

void AR_Sound::getMachineName(YAC_Value *_r) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      const ar_sound_t *sound = getSoundPtr();
      _r->initEmptyString();
      _r->value.string_val->visit(ar_sound_get_machine_name(sound));
   }
}

sSI AR_Sound::getMachine(void) {
   sSI ret;
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      const ar_sound_t *sound = getSoundPtr();
      ret = sSI(sound->machine_type);
   }
   else
   {
      ret = -1;
   }
   return ret;
}

void AR_Sound::setMachine(sUI _machine) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      if(_machine < AR_NUM_SOUND_MACHINES)
      {
         ar_sound_t *sound = getSoundPtr();
         sound->machine_type = sU8(_machine);
         yac_host->printf("xxx AR_Sound::setMachine(%u)\n", _machine);
      }
   }
}

sSI AR_Sound::getMachineParam(sUI _paramIdx) {
   sSI ret = -1;
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      if(_paramIdx < 8u)
      {
         const ar_sound_t *sound = getSoundPtr();
         const s_u16_t *s = &sound->synth_param_1 + _paramIdx;
         ret = (s->b.hi << 8) | s->b.lo;
      }
   }
   return ret;
}

void AR_Sound::setMachineParam(sUI _paramIdx, sUI _value16) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      if(_paramIdx < 8u)
      {
         ar_sound_t *sound = getSoundPtr();
         s_u16_t *d = &sound->synth_param_1 + _paramIdx;
         d->b.hi = sU8(_value16 >> 8);
         d->b.lo = sU8(_value16 & 255u);
      }
   }
}

void AR_Sound::getMachineParamShortName(sUI _paramIdx, YAC_Value *_r) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      if(_paramIdx < 8u)
      {
         const ar_sound_t *sound = getSoundPtr();
         _r->initEmptyString();
         _r->value.string_val->visit(ar_sound_get_machine_param_short_name(sound, _paramIdx));
      }
   }
}

// Velocity to volume (0=off or 1=on)
sBool AR_Sound::getVelToVol(void) {
   sBool r = YAC_FALSE;
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      const ar_sound_t *sound = getSoundPtr();
      r = (0u != (sound->mode_flags & AR_SOUND_MODE_MASK_VELTOVOL));
   }
   return r;
}

void AR_Sound::setVelToVol(sBool _bEnable) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      ar_sound_t *sound = getSoundPtr();
      sound->mode_flags = (sound->mode_flags & ~AR_SOUND_MODE_MASK_VELTOVOL) | (_bEnable ? AR_SOUND_MODE_MASK_VELTOVOL : 0u);
   }
}

sUI AR_Sound::getAccentLevel(void) {
   sUI r = 0u;
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      const ar_sound_t *sound = getSoundPtr();
      r = sound->accent_level;
   }
   return r;
}

void AR_Sound::setAccentLevel(sUI _v) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      ar_sound_t *sound = getSoundPtr();
      sound->accent_level = sU8(_v);
   }
}

// Chromatic mode (0=off, 1=synth, 2=sample, 3=syn+smp)
sUI AR_Sound::getChromaticMode(void) {
   sUI r = 0u;
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      const ar_sound_t *sound = getSoundPtr();
      r = (sound->mode_flags & AR_SOUND_MODE_MASK_CHROMATIC) >> AR_SOUND_MODE_SHIFT_CHROMATIC;
   }
   return r;
}

void AR_Sound::setChromaticMode(sUI _value) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      ar_sound_t *sound = getSoundPtr();
      sound->mode_flags = (sound->mode_flags &~AR_SOUND_MODE_MASK_CHROMATIC) | ((sU8(_value) << AR_SOUND_MODE_SHIFT_CHROMATIC) & AR_SOUND_MODE_MASK_CHROMATIC);
   }
}

// Legacy FX send (0=off or 1=on)
sBool AR_Sound::getLegacyFXSend(void) {
   sBool r = YAC_FALSE;
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      const ar_sound_t *sound = getSoundPtr();
      r = (sound->mode_flags & AR_SOUND_MODE_MASK_LEGACY_FX) >> AR_SOUND_MODE_SHIFT_LEGACY_FX;
   }
   return r;
}

void AR_Sound::setLegacyFXSend(sBool _bEnable) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      ar_sound_t *sound = getSoundPtr();
      sound->mode_flags = (sound->mode_flags & ~AR_SOUND_MODE_MASK_LEGACY_FX) | (_bEnable ? AR_SOUND_MODE_MASK_LEGACY_FX : 0u);
   }
}

// Env reset (0=off or 1=on/filter)
sBool AR_Sound::getEnvReset(void) {
   sBool r = YAC_FALSE;
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      const ar_sound_t *sound = getSoundPtr();
      r = (sound->mode_flags & AR_SOUND_MODE_MASK_ENVRESET_FILTER) >> AR_SOUND_MODE_SHIFT_ENVRESET_FILTER;
   }
   return r;
}

void AR_Sound::setEnvReset(sBool _bEnable) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      ar_sound_t *sound = getSoundPtr();
      sound->mode_flags = (sound->mode_flags & ~AR_SOUND_MODE_MASK_ENVRESET_FILTER) | (_bEnable ? AR_SOUND_MODE_MASK_ENVRESET_FILTER : 0u);
   }
}

sUI AR_Sound::GetNumLfoDestUI(void) {
   return AR_NUM_SOUND_LFO_DEST_UI;
}

void AR_Sound::getLfoDestName(YAC_Value *_r) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      const ar_sound_t *sound = getSoundPtr();
      _r->initEmptyString();
      _r->value.string_val->alloc(AR_SOUND_MAX_LFO_DEST_NAME);
      if(ar_sound_get_lfo_dest_name(sound, (char*)_r->value.string_val->chars, AR_SOUND_MAX_LFO_DEST_NAME))
      {
         _r->value.string_val->fixLength();
      }
      else
      {
         yac_host->printf("[---] AR_Sound::getLfoDestName: ar_sound_get_lfo_dest_name() failed\n");
         _r->unset();
      }
   }
}

void AR_Sound::getLfoDestNameByListIdx(sUI _listIdx, YAC_Value *_r) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      const ar_sound_t *sound = getSoundPtr();
      _r->initEmptyString();
      _r->value.string_val->alloc(AR_SOUND_MAX_LFO_DEST_NAME);
      if(ar_sound_get_lfo_dest_name_by_list_idx(sound, _listIdx, (char*)_r->value.string_val->chars, AR_SOUND_MAX_LFO_DEST_NAME))
      {
         _r->value.string_val->fixLength();
      }
      else
      {
         yac_host->printf("[---] AR_Sound::getLfoDestNameByListIdx: ar_sound_get_lfo_dest_name_by_list_idx() failed\n");
         _r->unset();
      }
   }
}

sSI AR_Sound::getLfoDestListIdx(void) {
   sSI r = 0;
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      ar_sound_t *sound = getSoundPtr();
      r = ar_sound_get_list_idx_by_lfo_dest(sound->lfo_dest);
   }
   return r;
}

void AR_Sound::setLfoDestByListIdx(sUI _idx) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      ar_sound_t *sound = getSoundPtr();
      sSI lfoDest = ar_sound_get_lfo_dest_by_list_idx(_idx);
      if(-1 != lfoDest)
      {
         sound->lfo_dest = sU8(lfoDest);
      }
   }
}



// -------------------------------------------

sUI AR_Sound::GetNumModDestUI(void) {
   return AR_NUM_SOUND_MOD_DEST_UI;
}

void AR_Sound::getModDestNameByListIdx(sUI _listIdx, YAC_Value *_r) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      const ar_sound_t *sound = getSoundPtr();
      _r->initEmptyString();
      _r->value.string_val->alloc(AR_SOUND_MAX_MOD_DEST_NAME);
      if(ar_sound_get_mod_dest_name_by_list_idx(sound, _listIdx, (char*)_r->value.string_val->chars, AR_SOUND_MAX_MOD_DEST_NAME))
      {
         _r->value.string_val->fixLength();
      }
      else
      {
         yac_host->printf("[---] AR_Sound::getModDestNameByListIdx: ar_sound_get_mod_dest_name_by_list_idx() failed\n");
         _r->unset();
      }
   }
}

void AR_Sound::getVelocityModDestName(sUI _slotIdx, YAC_Value *_r) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      const ar_sound_t *sound = getSoundPtr();
      _r->initEmptyString();
      _r->value.string_val->alloc(AR_SOUND_MAX_MOD_DEST_NAME);
      if(ar_sound_get_mod_dest_name(sound,
                                    ((sU8*)&sound->vel_target_1)[2u*_slotIdx],
                                    (char*)_r->value.string_val->chars,
                                    AR_SOUND_MAX_MOD_DEST_NAME
                                    )
         )
      {
         _r->value.string_val->fixLength();
      }
      else
      {
         yac_host->printf("[---] AR_Sound::getVelocityModDestName: ar_sound_get_mod_dest_name(slotIdx=%u) failed\n", _slotIdx);
         _r->unset();
      }
   }
}

sSI AR_Sound::getVelocityModDestListIdx(sUI _slotIdx) {
   sSI r = 0;
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      const ar_sound_t *sound = getSoundPtr();
      r = ar_sound_get_list_idx_by_mod_dest( ((sU8*)&sound->vel_target_1)[2u*_slotIdx] );
   }
   return r;
}

void AR_Sound::setVelocityModDestByListIdx(sUI _slotIdx, sUI _idx) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      ar_sound_t *sound = getSoundPtr();
      sSI modDest = ar_sound_get_mod_dest_by_list_idx(_idx);
      if(-1 != modDest)
      {
         ((sU8*)&sound->vel_target_1)[2u*_slotIdx] = sU8(modDest);
      }
   }
}

sSI AR_Sound::getVelocityModAmt(sUI _slotIdx) {
   sSI r = 0;
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      if(_slotIdx < 4u)
      {
         const ar_sound_t *sound = getSoundPtr();
         r = ((sS8*)&sound->vel_amt_1)[2u*_slotIdx];
      }
   }
   return r;
}

void AR_Sound::setVelocityModAmt(sUI _slotIdx, sSI _amt) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      if(_slotIdx < 4u)
      {
         ar_sound_t *sound = getSoundPtr();
         ((sS8*)&sound->vel_amt_1)[2u*_slotIdx] = (sS8)_amt;
      }
   }
}


// -------------------------------------------

void AR_Sound::getAftertouchModDestName(sUI _slotIdx, YAC_Value *_r) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      const ar_sound_t *sound = getSoundPtr();
      _r->initEmptyString();
      _r->value.string_val->alloc(AR_SOUND_MAX_MOD_DEST_NAME);
      if(ar_sound_get_mod_dest_name(sound,
                                    ((sU8*)&sound->at_target_1)[2u*_slotIdx],
                                    (char*)_r->value.string_val->chars,
                                    AR_SOUND_MAX_MOD_DEST_NAME
                                    )
         )
      {
         _r->value.string_val->fixLength();
      }
      else
      {
         yac_host->printf("[---] AR_Sound::getAftertouchModDestName: ar_sound_get_mod_dest_name(slotIdx=%u) failed\n", _slotIdx);
         _r->unset();
      }
   }
}

sSI AR_Sound::getAftertouchModDestListIdx(sUI _slotIdx) {
   sSI r = 0;
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      const ar_sound_t *sound = getSoundPtr();
      r = ar_sound_get_list_idx_by_mod_dest( ((sU8*)&sound->at_target_1)[2u*_slotIdx] );
   }
   return r;
}

void AR_Sound::setAftertouchModDestByListIdx(sUI _slotIdx, sUI _idx) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      ar_sound_t *sound = getSoundPtr();
      sSI modDest = ar_sound_get_mod_dest_by_list_idx(_idx);
      if(-1 != modDest)
      {
         ((sU8*)&sound->at_target_1)[2u*_slotIdx] = sU8(modDest);
      }
   }
}

sSI AR_Sound::getAftertouchModAmt(sUI _slotIdx) {
   sSI r = 0;
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      if(_slotIdx < 4u)
      {
         const ar_sound_t *sound = getSoundPtr();
         r = ((sS8*)&sound->at_amt_1)[2u*_slotIdx];
      }
   }
   return r;
}

void AR_Sound::setAftertouchModAmt(sUI _slotIdx, sSI _amt) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      if(_slotIdx < 4u)
      {
         ar_sound_t *sound = getSoundPtr();
         ((sS8*)&sound->at_amt_1)[2u*_slotIdx] = (sS8)_amt;
      }
   }
}

// -------------------------------------------

sUI AR_Sound::getSmpTUN(void) {
   sUI ret = 0u;
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      const ar_sound_t *sound = getSoundPtr();
      ret = sound->sample_tune;
   }
   return ret;
}

void AR_Sound::setSmpTUN(sUI _v) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      ar_sound_t *sound = getSoundPtr();
      sound->sample_tune = sU8(_v);
   }
}

sUI AR_Sound::getSmpFIN(void) {
   sUI ret = 0u;
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      const ar_sound_t *sound = getSoundPtr();
      ret = sound->sample_fine_tune;
   }
   return ret;
}

void AR_Sound::setSmpFIN(sUI _v) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      ar_sound_t *sound = getSoundPtr();
      sound->sample_fine_tune = sU8(_v);
   }
}

sUI AR_Sound::getSmpBR(void) {
   sUI ret = 0u;
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      const ar_sound_t *sound = getSoundPtr();
      ret = sound->sample_br;
   }
   return ret;
}

void AR_Sound::setSmpBR(sUI _v) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      ar_sound_t *sound = getSoundPtr();
      sound->sample_br = sU8(_v);
   }
}

sUI AR_Sound::getSmpSMP(void) {
   sUI ret = 0u;
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      const ar_sound_t *sound = getSoundPtr();
      ret = sound->sample_nr;
   }
   return ret;
}

void AR_Sound::setSmpSMP(sUI _v) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      ar_sound_t *sound = getSoundPtr();
      sound->sample_nr = sU8(_v);
   }
}

sUI AR_Sound::getSmpSTA(void) {
   sUI ret = 0u;
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      const ar_sound_t *sound = getSoundPtr();
      ret = (sound->sample_start.b.hi << 8) | (sound->sample_start.b.lo);
   }
   return ret;
}

void AR_Sound::setSmpSTA(sUI _v) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      ar_sound_t *sound = getSoundPtr();
      sound->sample_start.b.hi = sU8(_v >> 8);
      sound->sample_start.b.lo = sU8(_v & 255u);
   }
}

sUI AR_Sound::getSmpEND(void) {
   sUI ret = 0u;
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      const ar_sound_t *sound = getSoundPtr();
      ret = (sound->sample_end.b.hi << 8) | (sound->sample_start.b.lo);
   }
   return ret;
}

void AR_Sound::setSmpEND(sUI _v) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      ar_sound_t *sound = getSoundPtr();
      sound->sample_end.b.hi = sU8(_v >> 8);
      sound->sample_end.b.lo = sU8(_v & 255u);
   }
}

sUI AR_Sound::getSmpLOP(void) {
   sUI ret = 0u;
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      const ar_sound_t *sound = getSoundPtr();
      ret = sound->sample_loop_flag;
   }
   return ret;
}

void AR_Sound::setSmpLOP(sUI _v) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      ar_sound_t *sound = getSoundPtr();
      sound->sample_loop_flag = sU8(_v);
   }
}

sUI AR_Sound::getSmpLEV(void) {
   sUI ret = 0u;
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      const ar_sound_t *sound = getSoundPtr();
      ret = sound->sample_volume;
   }
   return ret;
}

void AR_Sound::setSmpLEV(sUI _v) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      ar_sound_t *sound = getSoundPtr();
      sound->sample_volume = sU8(_v);
   }
}

// -------------------------------------------

sUI AR_Sound::getFltATK(void) {
   sUI ret = 0u;
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      const ar_sound_t *sound = getSoundPtr();
      ret = sound->flt_attack;
   }
   return ret;
}

void AR_Sound::setFltATK(sUI _v) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      ar_sound_t *sound = getSoundPtr();
      sound->flt_attack = sU8(_v);
   }
}

sUI AR_Sound::getFltDEC(void) {
   sUI ret = 0u;
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      const ar_sound_t *sound = getSoundPtr();
      ret = sound->flt_decay;
   }
   return ret;
}

void AR_Sound::setFltDEC(sUI _v) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      ar_sound_t *sound = getSoundPtr();
      sound->flt_decay = sU8(_v);
   }
}

sUI AR_Sound::getFltSUS(void) {
   sUI ret = 0u;
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      const ar_sound_t *sound = getSoundPtr();
      ret = sound->flt_sustain;
   }
   return ret;
}

void AR_Sound::setFltSUS(sUI _v) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      ar_sound_t *sound = getSoundPtr();
      sound->flt_sustain = sU8(_v);
   }
}

sUI AR_Sound::getFltREL(void) {
   sUI ret = 0u;
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      const ar_sound_t *sound = getSoundPtr();
      ret = sound->flt_release;
   }
   return ret;
}

void AR_Sound::setFltREL(sUI _v) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      ar_sound_t *sound = getSoundPtr();
      sound->flt_release = sU8(_v);
   }
}

sUI AR_Sound::getFltFRQ(void) {
   sUI ret = 0u;
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      const ar_sound_t *sound = getSoundPtr();
      ret = sound->flt_cutoff;
   }
   return ret;
}

void AR_Sound::setFltFRQ(sUI _v) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      ar_sound_t *sound = getSoundPtr();
      sound->flt_cutoff = sU8(_v);
   }
}

sUI AR_Sound::getFltRES(void) {
   sUI ret = 0u;
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      const ar_sound_t *sound = getSoundPtr();
      ret = sound->flt_res;
   }
   return ret;
}

void AR_Sound::setFltRES(sUI _v) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      ar_sound_t *sound = getSoundPtr();
      sound->flt_res = sU8(_v);
   }
}

sUI AR_Sound::getFltTYP(void) {
   sUI ret = 0u;
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      const ar_sound_t *sound = getSoundPtr();
      ret = sound->flt_type;
   }
   return ret;
}

void AR_Sound::setFltTYP(sUI _v) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      ar_sound_t *sound = getSoundPtr();
      sound->flt_type = sU8(_v);
   }
}

sUI AR_Sound::getFltENV(void) {
   sUI ret = 0u;
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      const ar_sound_t *sound = getSoundPtr();
      ret = sound->flt_env;
   }
   return ret;
}

void AR_Sound::setFltENV(sUI _v) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      ar_sound_t *sound = getSoundPtr();
      sound->flt_env = sU8(_v);
   }
}

// ---------------------------------------------

sUI AR_Sound::getAmpATK(void) {
   sUI ret = 0u;
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      const ar_sound_t *sound = getSoundPtr();
      ret = sound->amp_attack;
   }
   return ret;
}

void AR_Sound::setAmpATK(sUI _v) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      ar_sound_t *sound = getSoundPtr();
      sound->amp_attack = sU8(_v);
   }
}

sUI AR_Sound::getAmpHLD(void) {
   sUI ret = 0u;
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      const ar_sound_t *sound = getSoundPtr();
      ret = sound->amp_hold;
   }
   return ret;
}

void AR_Sound::setAmpHLD(sUI _v) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      ar_sound_t *sound = getSoundPtr();
      sound->amp_hold = sU8(_v);
   }
}

sUI AR_Sound::getAmpDEC(void) {
   sUI ret = 0u;
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      const ar_sound_t *sound = getSoundPtr();
      ret = sound->amp_decay;
   }
   return ret;
}

void AR_Sound::setAmpDEC(sUI _v) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      ar_sound_t *sound = getSoundPtr();
      sound->amp_decay = sU8(_v);
   }
}

sUI AR_Sound::getAmpOVR(void) {
   sUI ret = 0u;
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      const ar_sound_t *sound = getSoundPtr();
      ret = sound->amp_overdrive;
   }
   return ret;
}

void AR_Sound::setAmpOVR(sUI _v) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      ar_sound_t *sound = getSoundPtr();
      sound->amp_overdrive = sU8(_v);
   }
}

sUI AR_Sound::getAmpDEL(void) {
   sUI ret = 0u;
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      const ar_sound_t *sound = getSoundPtr();
      ret = sound->amp_delay_send;
   }
   return ret;
}

void AR_Sound::setAmpDEL(sUI _v) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      ar_sound_t *sound = getSoundPtr();
      sound->amp_delay_send = sU8(_v);
   }
}

sUI AR_Sound::getAmpREV(void) {
   sUI ret = 0u;
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      const ar_sound_t *sound = getSoundPtr();
      ret = sound->amp_reverb_send;
   }
   return ret;
}

void AR_Sound::setAmpREV(sUI _v) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      ar_sound_t *sound = getSoundPtr();
      sound->amp_reverb_send = sU8(_v);
   }
}

sUI AR_Sound::getAmpPAN(void) {
   sUI ret = 0u;
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      const ar_sound_t *sound = getSoundPtr();
      ret = sound->amp_pan;
   }
   return ret;
}

void AR_Sound::setAmpPAN(sUI _v) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      ar_sound_t *sound = getSoundPtr();
      sound->amp_pan = sU8(_v);
   }
}

sUI AR_Sound::getAmpVOL(void) {
   sUI ret = 0u;
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      const ar_sound_t *sound = getSoundPtr();
      ret = sound->amp_volume;
   }
   return ret;
}

void AR_Sound::setAmpVOL(sUI _v) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      ar_sound_t *sound = getSoundPtr();
      sound->amp_volume = sU8(_v);
   }
}


// ---------------------------------------------

sUI AR_Sound::getLfoSPD(void) {
   sUI ret = 0u;
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      const ar_sound_t *sound = getSoundPtr();
      ret = sound->lfo_speed;
   }
   return ret;
}

void AR_Sound::setLfoSPD(sUI _v) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      ar_sound_t *sound = getSoundPtr();
      sound->lfo_speed = sU8(_v);
   }
}

sUI AR_Sound::getLfoMUL(void) {
   sUI ret = 0u;
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      const ar_sound_t *sound = getSoundPtr();
      ret = sound->lfo_multiplier;
   }
   return ret;
}

void AR_Sound::setLfoMUL(sUI _v) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      ar_sound_t *sound = getSoundPtr();
      sound->lfo_multiplier = sU8(_v);
   }
}

sUI AR_Sound::getLfoFAD(void) {
   sUI ret = 0u;
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      const ar_sound_t *sound = getSoundPtr();
      ret = sound->lfo_fade;
   }
   return ret;
}

void AR_Sound::setLfoFAD(sUI _v) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      ar_sound_t *sound = getSoundPtr();
      sound->lfo_fade = sU8(_v);
   }
}

sUI AR_Sound::getLfoDST(void) {
   sUI ret = 0u;
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      const ar_sound_t *sound = getSoundPtr();
      ret = sound->lfo_dest;
   }
   return ret;
}

void AR_Sound::setLfoDST(sUI _v) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      ar_sound_t *sound = getSoundPtr();
      sound->lfo_dest = sU8(_v);
   }
}

sUI AR_Sound::getLfoWAV(void) {
   sUI ret = 0u;
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      const ar_sound_t *sound = getSoundPtr();
      ret = sound->lfo_wav;
   }
   return ret;
}

void AR_Sound::setLfoWAV(sUI _v) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      ar_sound_t *sound = getSoundPtr();
      sound->lfo_wav = sU8(_v);
   }
}

sUI AR_Sound::getLfoSPH(void) {
   sUI ret = 0u;
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      const ar_sound_t *sound = getSoundPtr();
      ret = sound->lfo_start_phase;
   }
   return ret;
}

void AR_Sound::setLfoSPH(sUI _v) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      ar_sound_t *sound = getSoundPtr();
      sound->lfo_start_phase = sU8(_v);
   }
}

sUI AR_Sound::getLfoMOD(void) {
   sUI ret = 0u;
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      const ar_sound_t *sound = getSoundPtr();
      ret = sound->lfo_mode;
   }
   return ret;
}

void AR_Sound::setLfoMOD(sUI _v) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      ar_sound_t *sound = getSoundPtr();
      sound->lfo_mode = sU8(_v);
   }
}

sUI AR_Sound::getLfoDEP(void) {
   sUI ret = 0u;
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      const ar_sound_t *sound = getSoundPtr();
      ret = (sound->lfo_depth.b.hi << 8) | sound->lfo_depth.b.lo;
   }
   return ret;
}

void AR_Sound::setLfoDEP(sUI _v) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      ar_sound_t *sound = getSoundPtr();
      sound->lfo_depth.b.hi = sU8(_v >> 8);
      sound->lfo_depth.b.lo = sU8(_v & 255u);
   }
}
