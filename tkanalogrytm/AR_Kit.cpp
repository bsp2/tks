// ----
// ---- file   : AR_Kit.cpp
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2016-2023 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : This is part of the "tkanalogrytm" plugin.
// ----
// ---- changed: 28Feb2016, 01Mar2016, 03Mar2016, 21Nov2016, 08Jul2017, 09Jul2017, 21Aug2017
// ----          19Nov2018, 21Oct2019, 25Oct2019, 25Nov2023
// ----
// ----
// ----

#include "tkanalogrytm.h"

// #define DEBUG  if(0)
#define DEBUG


AR_Kit::AR_Kit(void) {
   current_buf = NULL;
   ar_kit_version = 0u;
}

AR_Kit::~AR_Kit() {
}

void YAC_VCALL AR_Kit::yacOperator(sSI _cmd, YAC_Object *_robj, YAC_Value *_r) {
   if(YAC_OP_ASSIGN == _cmd)
   {
      if(YAC_CHK(_robj, clid_AR_Kit))
      {
         YAC_CAST_ARG(AR_Kit, o, _robj);

         memcpy(syx, o->syx, AR_KIT_SYX_MAX_SZ);
         memcpy(raw, o->raw, AR_KIT_MAX_SZ);

         if(o->current_buf == o->syx)
         {
            current_buf = syx;
         }
         else if(o->current_buf == o->raw)
         {
            current_buf = raw;
         }
         else
         {
            current_buf = NULL;
         }

         ar_kit_version = o->ar_kit_version;
      }
   }
   else
   {
      YAC_Object::yacOperator(_cmd, _robj, _r);
   }
}

sUI AR_Kit::getRequiredSizeSyx(void) {
   return
      (1 == ar_kit_version) ? AR_KIT_SYX_V1_SZ :  // ancient (2014)
      (2 == ar_kit_version) ? AR_KIT_SYX_V2_SZ :  // os v1.31b
      (3 == ar_kit_version) ? AR_KIT_SYX_V3_SZ :  // os v1.31b
      (4 == ar_kit_version) ? AR_KIT_SYX_V4_SZ :  // os v1.50
      AR_KIT_SYX_V5_SZ;  // os v1.70
}

sUI AR_Kit::getRequiredSizeRaw(void) {
   return
      (1 == ar_kit_version) ? AR_KIT_V1_SZ :  // ancient (2014)
      (2 == ar_kit_version) ? AR_KIT_V2_SZ :  // os v1.31b
      (3 == ar_kit_version) ? AR_KIT_V3_SZ :  // os v1.31b
      (4 == ar_kit_version) ? AR_KIT_V4_SZ :  // os v1.50
      AR_KIT_V5_SZ;  // os v1.70
}

sSI AR_Kit::getCurrentDataIdx(void) {
   if(syx == current_buf)
   {
      return 1;
   }
   else if(raw == current_buf)
   {
      return 2;
   }
   else
   {
      return 0;
   }
}

void AR_Kit::saveSyxToStream(YAC_Object *_ofs) {
   if(YAC_IS_STREAM(_ofs))
   {
      _ofs->yacStreamWriteI16(2); // version

      _ofs->yacStreamWriteI8(ar_kit_version); // v2+

      sUI sz = getRequiredSizeSyx();

      _ofs->yacStreamWriteI16(sz);

      for(sUI i = 0u; i < sz; i++)
      {
         _ofs->yacStreamWriteI8(syx[i]);
      }
   }
}

void AR_Kit::saveRawToStream(YAC_Object *_ofs) {
   if(YAC_IS_STREAM(_ofs))
   {
      _ofs->yacStreamWriteI16(2); // version

      _ofs->yacStreamWriteI8(ar_kit_version); // v2+

      sUI sz = getRequiredSizeRaw();

      _ofs->yacStreamWriteI16(sz);

      for(sUI i = 0u; i < sz; i++)
      {
         _ofs->yacStreamWriteI8(raw[i]);
      }
   }
}

sBool AR_Kit::loadSyxFromStream(YAC_Object *_ifs) {
   if(YAC_IS_STREAM(_ifs))
   {
      sU16 ver = _ifs->yacStreamReadI16();

      if(ver >= 1u)
      {
         if(ver >= 2u)
         {
            ar_kit_version = _ifs->yacStreamReadI8();
         }
         else
         {
            ar_kit_version = 1u;
         }

         sU16 sz = _ifs->yacStreamReadI16();

         if(getRequiredSizeSyx() == sz)
         {
            for(sUI i = 0u; i < sz; i++)
            {
               syx[i] = _ifs->yacStreamReadI8();
            }

            current_buf = syx;

            // Succeeded
            return YAC_TRUE;
         }
         else
         {
            yac_host->printf("[---] AR_Kit::loadSyxFromStream: bad sz=%u\n", sz);
         }
      }
      else
      {
         yac_host->printf("[---] AR_Kit::loadSyxFromStream: bad ver=%u\n", ver);
      }
   }

   // Failed
   return YAC_FALSE;
}

sBool AR_Kit::loadRawFromStream(YAC_Object *_ifs) {
   if(YAC_IS_STREAM(_ifs))
   {
      sU16 ver = _ifs->yacStreamReadI16();

      if(ver >= 1u)
      {
         if(ver >= 2u)
         {
            ar_kit_version = _ifs->yacStreamReadI8();
         }
         else
         {
            ar_kit_version = 1u;
         }

         sU16 sz = _ifs->yacStreamReadI16();

         if(getRequiredSizeRaw() == sz)
         {
            for(sUI i = 0u; i < sz; i++)
            {
               raw[i] = _ifs->yacStreamReadI8();
            }

            current_buf = raw;

            // Succeeded
            return YAC_TRUE;
         }
         else
         {
            yac_host->printf("[---] AR_Kit::loadRawFromStream: bad sz=%u\n", sz);
         }
      }
      else
      {
         yac_host->printf("[---] AR_Kit::loadRawFromStream: bad ver=%u\n", ver);
      }
   }

   // Failed
   return YAC_FALSE;
}

sBool AR_Kit::addKitRequestX(YAC_Buffer *_d, sUI _devId, sUI _kitNr) {
   sBool ret = YAC_FALSE;

   if(YAC_Is_Buffer(_d))
   {
      if((_d->io_offset + AR_SYSEX_REQUEST_MSG_SZ) <= _d->size)
      {
         if(AR_ERR_OK == ar_kit_request_x(_d->buffer + _d->io_offset, _devId&127u, _kitNr&127u))
         {
            _d->io_offset += AR_SYSEX_REQUEST_MSG_SZ;

            // Succeeded
            ret = YAC_TRUE;
         }
      }
   }

   return ret;
}

sBool AR_Kit::syxToRaw(YAC_Buffer *_s) {
   sBool ret = YAC_FALSE;

   if(YAC_Is_Buffer(_s))
   {
      if(_s->size >= AR_KIT_SYX_MIN_SZ)
      {
         const sU8 *srcBuf = _s->buffer;
         sU32 srcBufSz = _s->io_offset;

         sU32 retRawBufSz = AR_KIT_MAX_SZ;

         ar_sysex_meta_t meta;

         if(AR_ERR_OK == ar_sysex_to_raw(raw, &srcBuf, &srcBufSz, &retRawBufSz, &meta))
         {
            DEBUG yac_host->printf("[dbg] AR_Kit::syxToRaw: kit raw size is %u (0x%08x) (sizeof(ar_kit_t)=%u)\n", retRawBufSz, retRawBufSz, sUI(sizeof(ar_kit_t)));

            if(AR_KIT_V1_SZ == retRawBufSz)
            {
               ar_kit_version = 1;
            }
            else if(AR_KIT_V2_SZ == retRawBufSz)
            {
               ar_kit_version = 2;
            }
            else if(AR_KIT_V3_SZ == retRawBufSz)
            {
               ar_kit_version = 3;
            }
            else if(AR_KIT_V4_SZ == retRawBufSz)
            {
               ar_kit_version = 4;
            }
            else if(AR_KIT_V5_SZ == retRawBufSz)
            {
               ar_kit_version = 5;
            }
            else
            {
               yac_host->printf("[---] AR_Kit::syxToRaw: unknown kit size (%u)\n", retRawBufSz);
               ar_kit_version = 0;
            }

            if(ar_kit_version > 0)
            {
               // Succeeded
               ret = YAC_TRUE;

               DEBUG yac_host->printf("[dbg] AR_Kit::syxToRaw: OK, retRawBufSz=%u.\n", retRawBufSz);
            }

            current_buf = raw;
         }
         else
         {
            yac_host->printf("[---] AR_Kit::syxToRaw: ar_sysex_to_raw() failed.\n");
            current_buf = NULL;
         }
      }
      else
      {
         yac_host->printf("[---] AR_Kit::syxToRaw: source buffer too small (have=%u, need at least %u).\n", _s->size, AR_KIT_SYX_MIN_SZ);
         current_buf = NULL;
      }
   }

   return ret;
}


sBool AR_Kit::sendKitX(YAC_Buffer *_d, sUI _devId, sUI _kitNr) {
   sBool ret = YAC_FALSE;

   if(YAC_Is_Buffer(_d))
   {
      sUI reqSizeSyx = getRequiredSizeSyx();

      if(_d->size >= reqSizeSyx)
      {
         sU8 *dstBuf = _d->buffer;
         sU32 retSyxBufSz = 0;
         ar_sysex_meta_t meta;

         AR_SYSEX_META_INIT(meta, _devId, AR_TYPE_KIT, (128u + (_kitNr & 127u)));

         if(current_buf == raw)
         {
            if(AR_ERR_OK == ar_raw_to_sysex(dstBuf, raw, getRequiredSizeRaw(), &retSyxBufSz, &meta))
            {
               _d->io_offset = retSyxBufSz;

               // Succeeded
               ret = YAC_TRUE;

               DEBUG yac_host->printf("[dbg] AR_Kit::sendKitX: OK, retSyxBufSz=%u.\n", retSyxBufSz);
            }
            else
            {
               yac_host->printf("[---] AR_Kit::sendKitX: ar_raw_to_sysex() failed\n");
            }
         }
         else
         {
            yac_host->printf("[---] AR_Kit::sendKitX: current_buf != raw\n");
         }
      }
      else
      {
         yac_host->printf("[---] AR_Kit::sendKitX: dest buffer too small (have=%u, need=%u).\n", _d->size, reqSizeSyx);
      }
   }

   return ret;
}

sBool AR_Kit::sendSoundX(YAC_Buffer *_d, sUI _devId, sUI _trackIdx) {

   sBool ret = YAC_FALSE;

   if(YAC_Is_Buffer(_d))
   {
      if(_d->size >= AR_SOUND_SYX_SZ)
      {
         if(_trackIdx < 12u)
         {
            sU8 *dstBuf = _d->buffer;
            sU32 retSyxBufSz = 0;
            ar_sysex_meta_t meta;

            AR_SYSEX_META_INIT(meta, _devId, AR_TYPE_SOUND, 128u + _trackIdx);

            if(current_buf == raw)
            {
               ar_kit_t *dat = (ar_kit_t*)raw;

               if(AR_ERR_OK == ar_raw_to_sysex(dstBuf, (sU8*)&dat->tracks[_trackIdx], AR_SOUND_SZ, &retSyxBufSz, &meta))
               {
                  _d->io_offset = retSyxBufSz;

                  // Succeeded
                  ret = YAC_TRUE;

                  DEBUG yac_host->printf("[dbg] AR_Kit::sendSoundX: OK, retSyxBufSz=%u.\n", retSyxBufSz);
               }
               else
               {
                  yac_host->printf("[---] AR_Kit::sendSoundX: ar_raw_to_sysex() failed\n");
               }
            }
            else
            {
               yac_host->printf("[---] AR_Kit::sendSoundX: current_buf != raw\n");
            }
         }
         else
         {
            yac_host->printf("[---] AR_Kit::sendSoundX: invalid trackIdx=%u\n", _trackIdx);
         }
      }
      else
      {
         yac_host->printf("[---] AR_Kit::sendSoundX: dest buffer too small (have=%u, need=%u).\n", _d->size, AR_SOUND_SYX_SZ);
      }

   }

   return ret;
}

sBool AR_Kit::getRawBuffer(YAC_Buffer *_ret) {
   sBool ret = YAC_FALSE;

   if(YAC_Is_Buffer(_ret))
   {
      // Free previous contents (if any)
      _ret->yacArrayAlloc(0, 0, 0, 0);

      _ret->size      = getRequiredSizeRaw();
      _ret->io_offset = getRequiredSizeRaw();
      _ret->buffer    = raw;
      _ret->deleteme  = YAC_FALSE;

      ret = YAC_TRUE;
   }

   return ret;
}

sUI AR_Kit::getVersion(void) {
   return ar_kit_version;
}

sBool AR_Kit::CanMix(AR_Kit *_a, AR_Kit *_b) {
   if(_a->ar_kit_version >= 5u)
   {
      if(_b->ar_kit_version >= 5u)
      {
         if(2u/*RAW*/ == _a->getCurrentDataIdx())
         {
            if(2u/*RAW*/ == _b->getCurrentDataIdx())
            {
               return YAC_TRUE;
            }
         }
      }
   }

   DEBUG yac_host->printf("[dbg] AR_Kit::CanMix: failed. a.version=%u b.version=%u a.dataIdx=%u b.dataIdx=%u\n", _a->ar_kit_version, _b->ar_kit_version, _a->getCurrentDataIdx(), _b->getCurrentDataIdx());

   return YAC_FALSE;
}

sU8 AR_Kit::GetRAWVersion(AR_Kit *_a) {
   if(2u/*RAW*/ == _a->getCurrentDataIdx())
   {
      DEBUG yac_host->printf("[dbg] AR_Kit::GetRAWVersion: version=%u dataIdx=%u\n", _a->ar_kit_version, _a->getCurrentDataIdx());
      return _a->ar_kit_version;
   }
   return 0u;
}

sBool AR_Kit::copyPerfCtlFrom(AR_Kit *_o) {
   if(YAC_CHK(_o, clid_AR_Kit))
   {
      YAC_CAST_ARG(AR_Kit, o, _o);

      if(CanMix(this, o))
      {
         const ar_kit_t *datO = (const ar_kit_t *)o->raw;
         ar_kit_t *dat = (ar_kit_t *)raw;

         memcpy((void*)dat->perf_ctl, (void*)datO->perf_ctl, 48*4);

         return YAC_TRUE;
      }
   }
   return YAC_FALSE;
}

sBool AR_Kit::copySceneCtlFrom(AR_Kit *_o) {
   if(YAC_CHK(_o, clid_AR_Kit))
   {
      YAC_CAST_ARG(AR_Kit, o, _o);

      if(CanMix(this, o))
      {
         const ar_kit_t *datO = (const ar_kit_t *)o->raw;
         ar_kit_t *dat = (ar_kit_t *)raw;

         memcpy((void*)dat->scene_ctl, (void*)datO->scene_ctl, 48*4);

         return YAC_TRUE;
      }
   }
   return YAC_FALSE;
}

sBool AR_Kit::copyFXFrom(AR_Kit *_o) {
   if(YAC_CHK(_o, clid_AR_Kit))
   {
      YAC_CAST_ARG(AR_Kit, o, _o);

      if(CanMix(this, o))
      {
         const ar_kit_t *datO = (const ar_kit_t *)o->raw;
         ar_kit_t *dat = (ar_kit_t *)raw;

         memcpy((void*)&dat->fx_delay_time,
                (void*)&datO->fx_delay_time,
                sUI(((sU8*)dat->__unknown_arr3) - ((sU8*)&dat->fx_delay_time))
                );

         return YAC_TRUE;
      }
   }

   return YAC_FALSE;
}

sBool AR_Kit::copyTrackSoundFrom(sUI _dstTrackIdx, AR_Kit *_o, sUI _srcTrackIdx) {
   if(YAC_CHK(_o, clid_AR_Kit))
   {
      YAC_CAST_ARG(AR_Kit, o, _o);

      if(CanMix(this, o))
      {
         if(_dstTrackIdx < 12u && _srcTrackIdx < 12u)
         {
            const ar_kit_t *datO = (const ar_kit_t *)o->raw;
            ar_kit_t *dat = (ar_kit_t *)raw;

            // yac_host->printf("xxx AR_Kit::copyTrackSoundFrom(dst=%u src=%u)\n", _dstTrackIdx, _srcTrackIdx);

            memcpy((void*)&dat->tracks[_dstTrackIdx], (void*)&datO->tracks[_srcTrackIdx], sizeof(ar_kit_track_t));

            // Copy track level
            dat->track_levels[_dstTrackIdx] = datO->track_levels[_srcTrackIdx];
            // // dat->track_levels[_dstTrackIdx]. __unused_pad1 = datO->track_levels[_srcTrackIdx]. __unused_pad1;
            return YAC_TRUE;
         }
      }
   }
   return YAC_FALSE;
}

sBool AR_Kit::copyTrackSoundFromSound(sUI _dstTrackIdx, AR_Sound *_o) {
   if(YAC_CHK(_o, clid_AR_Sound))
   {
      YAC_CAST_ARG(AR_Sound, o, _o);

      if(ar_kit_version >= 5u)
      {
         if(_dstTrackIdx < 12u)
         {
            const ar_sound_t *datO = (const ar_sound_t *)o->raw;
            ar_kit_t *dat = (ar_kit_t *)raw;

            // yac_host->printf("xxx AR_Kit::copyTrackSoundFrom(dst=%u src=%u)\n", _dstTrackIdx, _srcTrackIdx);

            memcpy((void*)&dat->tracks[_dstTrackIdx], (void*)datO, sizeof(ar_sound_t));

            return YAC_TRUE;
         }
      }
   }
   return YAC_FALSE;
}

sUI AR_Kit::getTrackSampleNr(sUI _trackIdx) {
   sUI ret = 0u;

   if(GetRAWVersion(this) >= 5u)
   {
      if(_trackIdx < 12u)
      {
         ar_kit_t *dat = (ar_kit_t *)raw;
         ret = dat->tracks[_trackIdx].sample_nr;
      }
   }

   return ret;
}

void AR_Kit::setTrackSampleNr(sUI _trackIdx, sUI _sampleNr) {

   if(GetRAWVersion(this) >= 5u)
   {
      if(_trackIdx < 12u)
      {
         ar_kit_t *dat = (ar_kit_t *)raw;
         dat->tracks[_trackIdx].sample_nr = _sampleNr & 127u;
      }
   }

}

sBool AR_Kit::getSoundRefByIndex(sUI _trackIdx, YAC_Object *_retARSound) {
   sBool ret = YAC_FALSE;
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      if(_trackIdx < 12u)
      {
         if(YAC_CHK(_retARSound, clid_AR_Sound))
         {
            YAC_CAST_ARG(AR_Sound, retARSound, _retARSound);

            if(ar_kit_version >= 5u)
            {
               const ar_sound_t *sound = & ((const ar_kit_t*)raw)->tracks[_trackIdx];
               retARSound->current_buf = (sU8*)sound;
               retARSound->b_raw_buf_ref = YAC_TRUE;
               // // ::memcpy(retARSound->current_buf,
               // //          sound,
               // //          sizeof(ar_sound_t)
               // //          );
               ret = YAC_TRUE;
            }
         }
      }
   }
   return ret;
}

sUI AR_Kit::getTrackLevel(sUI _trackIdx) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      if(_trackIdx < 12u)
      {
         const ar_kit_t *kit = (const ar_kit_t*)raw;
         return kit->track_levels[_trackIdx].b.hi;
      }
   }
   return 0u;
}

void AR_Kit::setTrackLevel(sUI _trackIdx, sUI _level) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      if(_trackIdx < 12u)
      {
         ar_kit_t *kit = (ar_kit_t*)raw;
         if(_level > 127u)
            _level = 127u;
         kit->track_levels[_trackIdx].b.hi = (sU8)_level;
      }
   }
}
