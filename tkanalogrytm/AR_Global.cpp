// ----
// ---- file   : AR_Global.cpp
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

#include "tkanalogrytm.h"


AR_Global::AR_Global(void) {
   current_buf = NULL;
}

AR_Global::~AR_Global() {
}

void YAC_VCALL AR_Global::yacOperator(sSI _cmd, YAC_Object *_robj, YAC_Value *_r) {
   if(YAC_OP_ASSIGN == _cmd)
   {
      if(YAC_CHK(_robj, clid_AR_Global))
      {
         YAC_CAST_ARG(AR_Global, o, _robj);

         memcpy(syx, o->syx, AR_GLOBAL_SYX_SZ);
         memcpy((void*)getGlobalPtr(), (void*)o->getGlobalPtr(), AR_GLOBAL_SZ);

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
      }
   }
   else
   {
      YAC_Object::yacOperator(_cmd, _robj, _r);
   }
}

sUI AR_Global::getRequiredSizeSyx(void) {
   return AR_GLOBAL_SYX_SZ;
}

sUI AR_Global::getRequiredSizeRaw(void) {
   return AR_GLOBAL_SZ;
}

sSI AR_Global::getCurrentDataIdx(void) {
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

ar_global_t *AR_Global::getGlobalPtr(void) {
   ar_global_t *global = (ar_global_t*) raw;
   return global;
}

void AR_Global::saveSyxToStream(YAC_Object *_ofs) {
   if(YAC_IS_STREAM(_ofs))
   {
      _ofs->yacStreamWriteI16(1); // version

      _ofs->yacStreamWriteI16(AR_GLOBAL_SYX_SZ);

      for(sUI i=0; i < AR_GLOBAL_SYX_SZ; i++)
      {
         _ofs->yacStreamWriteI8(syx[i]);
      }
   }
}

void AR_Global::saveRawToStream(YAC_Object *_ofs) {
   if(YAC_IS_STREAM(_ofs))
   {
      _ofs->yacStreamWriteI16(1); // version

      _ofs->yacStreamWriteI16(AR_GLOBAL_SZ);

      for(sUI i=0; i < AR_GLOBAL_SZ; i++)
      {
         _ofs->yacStreamWriteI8(raw[i]);
      }
   }
}

sBool AR_Global::loadSyxFromStream(YAC_Object *_ifs) {
   if(YAC_IS_STREAM(_ifs))
   {
      sU16 ver = _ifs->yacStreamReadI16();

      if(ver >= 1u)
      {
         sU16 sz = _ifs->yacStreamReadI16();

         if(AR_GLOBAL_SYX_SZ == sz)
         {
            for(sUI i=0; i < AR_GLOBAL_SYX_SZ; i++)
            {
               syx[i] = _ifs->yacStreamReadI8();
            }

            current_buf = syx;

            // Succeeded
            return YAC_TRUE;
         }
         else
         {
            yac_host->printf("[---] AR_Global::loadSyxFromStream: bad sz=%u\n", sz);
         }
      }
      else
      {
         yac_host->printf("[---] AR_Global::loadSyxFromStream: bad ver=%u\n", ver);
      }
   }

   // Failed
   return YAC_FALSE;
}

sBool AR_Global::loadRawFromStream(YAC_Object *_ifs) {
   if(YAC_IS_STREAM(_ifs))
   {
      sU16 ver = _ifs->yacStreamReadI16();

      if(ver >= 1u)
      {
         sU16 sz = _ifs->yacStreamReadI16();

         if(AR_GLOBAL_SZ == sz)
         {
            for(sUI i=0; i < AR_GLOBAL_SZ; i++)
            {
               raw[i] = _ifs->yacStreamReadI8();
            }

            current_buf = raw;

            // Succeeded
            return YAC_TRUE;
         }
         else
         {
            yac_host->printf("[---] AR_Global::loadRawFromStream: bad sz=%u\n", sz);
         }
      }
      else
      {
         yac_host->printf("[---] AR_Global::loadRawFromStream: bad ver=%u\n", ver);
      }
   }

   // Failed
   return YAC_FALSE;
}

sBool AR_Global::addGlobalRequestX(YAC_Buffer *_d, sUI _devId, sUI _globalSlot) {
   sBool ret = YAC_FALSE;

   if(YAC_Is_Buffer(_d))
   {
      if((_d->io_offset + AR_SYSEX_REQUEST_MSG_SZ) <= _d->size)
      {
         if(AR_ERR_OK == ar_global_request_x(_d->buffer + _d->io_offset, _devId&127u, _globalSlot&127u))
         {
            _d->io_offset += AR_SYSEX_REQUEST_MSG_SZ;

            // Succeeded
            ret = YAC_TRUE;
         }
      }
   }

   return ret;
}

sBool AR_Global::syxToRaw(YAC_Buffer *_s) {
   sBool ret = YAC_FALSE;

   if(YAC_Is_Buffer(_s))
   {
      sU8 *rawPtr = (sU8*) getGlobalPtr();

      yac_host->printf("[trc] AR_Global::syxToRaw: _s->size=%u\n", _s->size);

      if(_s->size >= AR_GLOBAL_SYX_SZ)
      {
         const sU8 *srcBuf = _s->buffer;
         sU32 srcBufSz = _s->io_offset;
         sU32 retRawBufSz = AR_GLOBAL_SZ;
         ar_sysex_meta_t meta;

         if(AR_ERR_OK == ar_sysex_to_raw(rawPtr, &srcBuf, &srcBufSz, &retRawBufSz, &meta))
         {
            // Succeeded
            ret = YAC_TRUE;

            yac_host->printf("[dbg] AR_Global::syxToRaw: OK, retRawBufSz=%u.\n", retRawBufSz);

            current_buf = rawPtr;
         }
         else
         {
            yac_host->printf("[---] AR_Global::syxToRaw: ar_sysex_to_raw() failed.\n");
            current_buf = NULL;
         }
      }
      else
      {
         yac_host->printf("[---] AR_Global::syxToRaw: source buffer too small (have=%u, need=%u).\n", _s->size, AR_GLOBAL_SYX_SZ);
         current_buf = NULL;
      }
   }

   return ret;
}

sBool AR_Global::rawToSyx(YAC_Buffer *_d, sUI _globalSlot) {
   sBool ret = YAC_FALSE;

   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      if(YAC_Is_Buffer(_d))
      {
         sUI reqSizeSyx = getRequiredSizeSyx();
         if(_d->size >= reqSizeSyx)
         {
            const sU8 *rawPtr = (sU8*) getGlobalPtr();
            sUI reqSizeRaw = getRequiredSizeRaw();
            _d->io_offset = 0u;
            ar_sysex_meta_t meta;
            AR_SYSEX_META_INIT(meta, 0/*devId*/, AR_TYPE_GLOBAL, ((true/*bEditBuf*/ ? 128u : 0u) + (_globalSlot & 127u)));
            if(AR_ERR_OK == ar_global_raw_to_syx(_d->buffer, rawPtr, reqSizeRaw, &_d->io_offset, &meta))
            {
               ret = YAC_TRUE;
            }
         }
      }
   }
   return ret;
}

sBool AR_Global::sendGlobal(YAC_Buffer *_d, sUI _devId, sUI _globalSlot, sBool _bEditBuf) {
   sBool ret = YAC_FALSE;

   if(YAC_Is_Buffer(_d))
   {
      if(_d->size >= AR_GLOBAL_SYX_SZ)
      {
         sU8 *dstBuf = _d->buffer;
         sU32 retSyxBufSz = 0;
         ar_sysex_meta_t meta;

         AR_SYSEX_META_INIT(meta, _devId, AR_TYPE_GLOBAL, ((_bEditBuf ? 128u : 0u) + (_globalSlot & 127u)));

         if(current_buf == raw)
         {
            const sU8 *rawPtr = (sU8*)getGlobalPtr();

            if(AR_ERR_OK == ar_raw_to_sysex(dstBuf, rawPtr, AR_GLOBAL_SZ, &retSyxBufSz, &meta))
            {
               _d->io_offset = retSyxBufSz;

               // Succeeded
               ret = YAC_TRUE;

               yac_host->printf("[dbg] AR_Global::sendGlobal: OK, retSyxBufSz=%u.\n", retSyxBufSz);
            }
            else
            {
               yac_host->printf("[---] AR_Global::sendGlobal: ar_raw_to_sysex() failed\n");
            }
         }
         else
         {
            yac_host->printf("[---] AR_Global::sendGlobal: current_buf != raw\n");
         }
      }
      else
      {
         yac_host->printf("[---] AR_Global::sendGlobal: dest buffer too small (have=%u, need=%u).\n", _d->size, AR_GLOBAL_SYX_SZ);
      }

   }

   return ret;
}

sBool AR_Global::getRawBuffer(YAC_Buffer *_ret) {
   sBool ret = YAC_FALSE;

   if(YAC_Is_Buffer(_ret))
   {
      // Free previous contents (if any)
      _ret->yacArrayAlloc(0, 0, 0, 0);

      _ret->size      = AR_GLOBAL_SZ;
      _ret->io_offset = AR_GLOBAL_SZ;
      _ret->buffer    = (sU8*)getGlobalPtr();
      _ret->deleteme  = YAC_FALSE;

      ret = YAC_TRUE;
   }

   return ret;
}

sBool AR_Global::getSyxBuffer(YAC_Buffer *_ret) {
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

void AR_Global::setPadDest(sUI _dst) {
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      ar_global_t *global = getGlobalPtr();
      global->pad_dest = sU8(_dst);
      // yac_host->printf("xxx AR_Global::setPadDest(%u)\n", _dst);
   }
}

sUI AR_Global::getPadDest(void) {
   // yac_host->printf("xxx AR_Global::getPadDest: getCurrentDataIdx()=%u\n", getCurrentDataIdx());
   if(2u/*RAW*/ == getCurrentDataIdx())
   {
      const ar_global_t *global = getGlobalPtr();
      return (sUI)global->pad_dest;
   }
   return 0u;
}
