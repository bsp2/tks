// ----
// ---- file   : AR_Pattern.cpp
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2016-2019 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : This is part of the "tkanalogrytm" plugin.
// ----
// ---- created: 27Feb2016
// ---- changed: 29Feb2016, 01Mar2016, 03Mar2016, 21Oct2019, 26Nov2023, 27Nov2023, 28Nov2023
// ----
// ----
// ----

#include "tkanalogrytm.h"


AR_Pattern::AR_Pattern(void) {
   current_buf = NULL;
   ar_pattern_version = 0u;
}

AR_Pattern::~AR_Pattern() {
}

void YAC_VCALL AR_Pattern::yacOperator(sSI _cmd, YAC_Object *_robj, YAC_Value *_r) {
   if(YAC_OP_ASSIGN == _cmd)
   {
      if(YAC_CHK(_robj, clid_AR_Pattern))
      {
         YAC_CAST_ARG(AR_Pattern, o, _robj);

         memcpy(syx, o->syx, AR_PATTERN_SYX_MAX_SZ);
         memcpy(raw, o->raw, AR_PATTERN_MAX_SZ);

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

         ar_pattern_version = o->ar_pattern_version;
      }
   }
   else
   {
      YAC_Object::yacOperator(_cmd, _robj, _r);
   }
}

sUI AR_Pattern::getRequiredSizeSyx(void) {
   return
      (1u == ar_pattern_version) ? AR_PATTERN_SYX_V1_SZ :  // ancient (2014)
      (4u == ar_pattern_version) ? AR_PATTERN_SYX_V4_SZ :  // os v1.50..1.61b
      (5u == ar_pattern_version) ? AR_PATTERN_SYX_V5_SZ :  // os v1.70
      0u;  // unknown
}

sUI AR_Pattern::getRequiredSizeRaw(void) {
   return
      (1u == ar_pattern_version) ? AR_PATTERN_V1_SZ :  // ancient (2014)
      (4u == ar_pattern_version) ? AR_PATTERN_V4_SZ :  // os v1.50..1.61b
      (5u == ar_pattern_version) ? AR_PATTERN_V5_SZ :  // os v1.70
      0u;  // unknown
}

sSI AR_Pattern::getCurrentDataIdx(void) {
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

void AR_Pattern::saveSyxToStream(YAC_Object *_ofs) {
   if(YAC_IS_STREAM(_ofs))
   {
      _ofs->yacStreamWriteI16(2); // version

      _ofs->yacStreamWriteI8(ar_pattern_version); // v2+

      sUI sz = getRequiredSizeSyx();

      _ofs->yacStreamWriteI16(sz);

      for(sUI i=0; i < sz; i++)
      {
         _ofs->yacStreamWriteI8(syx[i]);
      }
   }
}

void AR_Pattern::saveRawToStream(YAC_Object *_ofs) {
   if(YAC_IS_STREAM(_ofs))
   {
      _ofs->yacStreamWriteI16(2); // version

      _ofs->yacStreamWriteI8(ar_pattern_version); // v2+

      sUI sz = getRequiredSizeRaw();

      _ofs->yacStreamWriteI16(sz);

      for(sUI i=0; i < sz; i++)
      {
         _ofs->yacStreamWriteI8(raw[i]);
      }
   }
}

sBool AR_Pattern::loadSyxFromStream(YAC_Object *_ifs) {
   if(YAC_IS_STREAM(_ifs))
   {
      sU16 ver = _ifs->yacStreamReadI16();

      if(ver >= 1u)
      {
         if(ver >= 2u)
         {
            ar_pattern_version = _ifs->yacStreamReadI8();
         }
         else
         {
            ar_pattern_version = 1u;
         }

         sU16 sz = _ifs->yacStreamReadI16();

         if(getRequiredSizeSyx() == sz)
         {
            for(sUI i=0; i < sz; i++)
            {
               syx[i] = _ifs->yacStreamReadI8();
            }

            current_buf = syx;

            // Succeeded
            return YAC_TRUE;
         }
         else
         {
            yac_host->printf("[---] AR_Pattern::loadSyxFromStream: bad sz=%u\n", sz);
         }
      }
      else
      {
         yac_host->printf("[---] AR_Pattern::loadSyxFromStream: bad ver=%u\n", ver);
      }
   }

   // Failed
   return YAC_FALSE;
}

sBool AR_Pattern::loadRawFromStream(YAC_Object *_ifs) {
   if(YAC_IS_STREAM(_ifs))
   {
      sU16 ver = _ifs->yacStreamReadI16();

      if(ver >= 1u)
      {
         if(ver >= 2u)
         {
            ar_pattern_version = _ifs->yacStreamReadI8();
         }
         else
         {
            ar_pattern_version = 1u;
         }

         sU16 sz = _ifs->yacStreamReadI16();

         if(getRequiredSizeRaw() == sz)
         {
            for(sUI i=0; i < sz; i++)
            {
               raw[i] = _ifs->yacStreamReadI8();
            }

            current_buf = raw;

            // Succeeded
            return YAC_TRUE;
         }
         else
         {
            yac_host->printf("[---] AR_Pattern::loadRawFromStream: bad sz=%u\n", sz);
         }
      }
      else
      {
         yac_host->printf("[---] AR_Pattern::loadRawFromStream: bad ver=%u\n", ver);
      }
   }

   // Failed
   return YAC_FALSE;
}

sBool AR_Pattern::addPatternRequestX(YAC_Buffer *_d, sUI _devId, sUI _patternNr) {
   sBool ret = YAC_FALSE;

   if(YAC_Is_Buffer(_d))
   {
      if((_d->io_offset + AR_SYSEX_REQUEST_MSG_SZ) <= _d->size)
      {
         if(AR_ERR_OK == ar_pattern_request_x(_d->buffer + _d->io_offset, _devId&127u, _patternNr&127u))
         {
            _d->io_offset += AR_SYSEX_REQUEST_MSG_SZ;

            // Succeeded
            ret = YAC_TRUE;
         }
      }
   }

   return ret;
}

sBool AR_Pattern::syxToRaw(YAC_Buffer *_s) {
   sBool ret = YAC_FALSE;

   if(YAC_Is_Buffer(_s))
   {
      if(_s->size >= AR_PATTERN_SYX_MIN_SZ)
      {
         const sU8 *srcBuf = _s->buffer;
         // // sU32 srcBufSz = _s->io_offset;
         sU32 srcBufSz = _s->size;

         sU32 retRawBufSz = AR_PATTERN_MAX_SZ;

         ar_sysex_meta_t meta;

         if(AR_ERR_OK == ar_sysex_to_raw(raw, &srcBuf, &srcBufSz, &retRawBufSz, &meta))
         {
            yac_host->printf("xxx AR_Pattern::syxToRaw: syxSz=%u rawSz=%u  sizeof(pat)=%u  sizeof(pat_track)=%u\n", srcBufSz, retRawBufSz, sizeof(ar_pattern_t), sizeof(ar_pattern_track_t));

            if(AR_PATTERN_V1_SZ == retRawBufSz)
            {
               ar_pattern_version = 1u;
            }
            else if(AR_PATTERN_V4_SZ == retRawBufSz)
            {
               ar_pattern_version = 4u;
            }
            else if(AR_PATTERN_V5_SZ == retRawBufSz)
            {
               ar_pattern_version = 5u;
            }
            else
            {
               yac_host->printf("[---] AR_Pattern::syxToRaw: unknown pattern size (%u)\n", retRawBufSz);
               ar_pattern_version = 0u;
            }

            if(ar_pattern_version > 0)
            {
               // Succeeded
               ret = YAC_TRUE;

               yac_host->printf("[dbg] AR_Pattern::syxToRaw: OK, retRawBufSz=%u.\n", retRawBufSz);
            }

            current_buf = raw;
         }
         else
         {
            yac_host->printf("[---] AR_Pattern::syxToRaw: ar_sysex_to_raw() failed.\n");
            current_buf = NULL;
         }
      }
      else
      {
         yac_host->printf("[---] AR_Pattern::syxToRaw: source buffer too small (have=%u, need at least %u).\n", _s->size, AR_PATTERN_SYX_MIN_SZ);
         current_buf = NULL;
      }
   }

   return ret;
}


sBool AR_Pattern::sendPattern(YAC_Buffer *_d, sUI _devId, sUI _patternNr, sBool _bEditBuf) {
   sBool ret = YAC_FALSE;

   if(YAC_Is_Buffer(_d))
   {
      sUI reqSizeSyx = getRequiredSizeSyx();

      if(_d->size >= reqSizeSyx)
      {
         sU8 *dstBuf = _d->buffer;
         sU32 retSyxBufSz = 0;
         ar_sysex_meta_t meta;

         AR_SYSEX_META_INIT(meta, _devId, AR_TYPE_PATTERN, ((_bEditBuf ? 128u : 0u) + (_patternNr & 127u)));

         if(current_buf == raw)
         {
            if(AR_ERR_OK == ar_raw_to_sysex(dstBuf, raw, getRequiredSizeRaw(), &retSyxBufSz, &meta))
            {
               _d->io_offset = retSyxBufSz;

               // Succeeded
               ret = YAC_TRUE;

               yac_host->printf("[dbg] AR_Pattern::sendPattern: OK, retSyxBufSz=%u.\n", retSyxBufSz);
            }
            else
            {
               yac_host->printf("[---] AR_Pattern::sendPattern: ar_raw_to_sysex() failed\n");
            }
         }
         else
         {
            yac_host->printf("[---] AR_Pattern::sendPattern: current_buf != raw\n");
         }
      }
      else
      {
         yac_host->printf("[---] AR_Pattern::sendPattern: dest buffer too small (have=%u, need=%u).\n", _d->size, reqSizeSyx);
      }

   }

   return ret;
}

sBool AR_Pattern::getRawBuffer(YAC_Buffer *_ret) {
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

sUI AR_Pattern::getVersion(void) {
   return ar_pattern_version;
}

sUI AR_Pattern::getMasterLength(void) {
   if(raw == current_buf)
   {
      const ar_pattern_t *pat = (const ar_pattern_t*)raw;
      return (pat->master_length.b.hi << 8) | pat->master_length.b.lo;
   }
   return 0u;
}

void AR_Pattern::setMasterLength(sUI _masterLen) {
   if(raw == current_buf)
   {
      ar_pattern_t *pat = (ar_pattern_t*)raw;
      pat->master_length.b.hi = sU8(_masterLen >> 8);
      pat->master_length.b.lo = sU8(_masterLen & 255u);
   }
}

sUI AR_Pattern::getMasterSpeed(void) {
   if(raw == current_buf)
   {
      const ar_pattern_t *pat = (const ar_pattern_t*)raw;
      return pat->master_speed;
   }
   return 0u;
}

void AR_Pattern::setMasterSpeed(sUI _speed) {
   if(raw == current_buf)
   {
      ar_pattern_t *pat = (ar_pattern_t*)raw;
      pat->master_speed = (sU8)_speed;
   }
}

sUI AR_Pattern::getDefaultNote(sUI _trackIdx) {
   if(raw == current_buf)
   {
      if(_trackIdx < 13u)
      {
         const ar_pattern_t *pat = (const ar_pattern_t*)raw;
         const ar_pattern_track_t *track = &pat->tracks[_trackIdx];
         return track->default_note;
      }
   }
   return 0u;
}

void AR_Pattern::setDefaultNote(sUI _trackIdx, sUI _v) {
   if(raw == current_buf)
   {
      if(_trackIdx < 13u)
      {
         ar_pattern_t *pat = (ar_pattern_t*)raw;
         ar_pattern_track_t *track = &pat->tracks[_trackIdx];
         track->default_note = (sU8)_v;
      }
   }
}

sUI AR_Pattern::getDefaultVelocity(sUI _trackIdx) {
   if(raw == current_buf)
   {
      if(_trackIdx < 13u)
      {
         const ar_pattern_t *pat = (const ar_pattern_t*)raw;
         const ar_pattern_track_t *track = &pat->tracks[_trackIdx];
         return track->default_velocity;
      }
   }
   return 0u;
}

void AR_Pattern::setDefaultVelocity(sUI _trackIdx, sUI _v) {
   if(raw == current_buf)
   {
      if(_trackIdx < 13u)
      {
         ar_pattern_t *pat = (ar_pattern_t*)raw;
         ar_pattern_track_t *track = &pat->tracks[_trackIdx];
         track->default_velocity = (sU8)_v;
      }
   }
}

sUI AR_Pattern::getDefaultNoteLength(sUI _trackIdx) {
   if(raw == current_buf)
   {
      if(_trackIdx < 13u)
      {
         const ar_pattern_t *pat = (const ar_pattern_t*)raw;
         const ar_pattern_track_t *track = &pat->tracks[_trackIdx];
         return track->default_note_length;
      }
   }
   return 0u;
}

void AR_Pattern::setDefaultNoteLength(sUI _trackIdx, sUI _v) {
   if(raw == current_buf)
   {
      if(_trackIdx < 13u)
      {
         ar_pattern_t *pat = (ar_pattern_t*)raw;
         ar_pattern_track_t *track = &pat->tracks[_trackIdx];
         track->default_note_length = (sU8)_v;
      }
   }
}

sUI AR_Pattern::getDefaultTrigFlags(sUI _trackIdx) {
   if(raw == current_buf)
   {
      if(_trackIdx < 13u)
      {
         const ar_pattern_t *pat = (const ar_pattern_t*)raw;
         const ar_pattern_track_t *track = &pat->tracks[_trackIdx];
         return track->default_trig_flags.b.lo | (track->default_trig_flags.b.hi << 8);
      }
   }
   return 0u;
}

void AR_Pattern::setDefaultTrigFlags(sUI _trackIdx, sUI _v) {
   if(raw == current_buf)
   {
      if(_trackIdx < 13u)
      {
         ar_pattern_t *pat = (ar_pattern_t*)raw;
         ar_pattern_track_t *track = &pat->tracks[_trackIdx];
         track->default_trig_flags.b.lo = (sU8)(_v & 255u);
         track->default_trig_flags.b.hi = (sU8)((_v >> 8) & 255u);
      }
   }
}

sUI AR_Pattern::getNumSteps(sUI _trackIdx) {
   if(raw == current_buf)
   {
      if(_trackIdx < 13u)
      {
         const ar_pattern_t *pat = (const ar_pattern_t*)raw;
         const ar_pattern_track_t *track = &pat->tracks[_trackIdx];
         return track->num_steps;
      }
   }
   return 0u;
}

void AR_Pattern::setNumSteps(sUI _trackIdx, sUI _numSteps) {
   if(raw == current_buf)
   {
      if(_trackIdx < 13u)
      {
         ar_pattern_t *pat = (ar_pattern_t*)raw;
         ar_pattern_track_t *track = &pat->tracks[_trackIdx];
         track->num_steps = (sU8)_numSteps;
      }
   }
}

void AR_Pattern::setFlagsAndSpeed(sUI _trackIdx, sUI _v) {
   if(raw == current_buf)
   {
      if(_trackIdx < 13u)
      {
         ar_pattern_t *pat = (ar_pattern_t*)raw;
         ar_pattern_track_t *track = &pat->tracks[_trackIdx];
         track->flags_and_speed = (sU8)_v;
      }
   }
}

sUI AR_Pattern::getFlagsAndSpeed(sUI _trackIdx) {
   if(raw == current_buf)
   {
      if(_trackIdx < 13u)
      {
         const ar_pattern_t *pat = (const ar_pattern_t*)raw;
         const ar_pattern_track_t *track = &pat->tracks[_trackIdx];
         return track->flags_and_speed;
      }
   }
   return 0u;
}

sUI AR_Pattern::getTrig(sUI _trackIdx, sUI _stepIdx) {
   if(raw == current_buf)
   {
      if(_trackIdx < 13u)
      {
         if(_stepIdx < 64u)
         {
            const ar_pattern_t *pat = (const ar_pattern_t*)raw;
            const ar_pattern_track_t *track = &pat->tracks[_trackIdx];
            return ar_pattern_track_get_trig_flags(track, _stepIdx);
         }
      }
   }
   return 0u;
}

void AR_Pattern::setTrig(sUI _trackIdx, sUI _stepIdx, sUI _val) {
   if(raw == current_buf)
   {
      if(_trackIdx < 13u)
      {
         if(_stepIdx < 64u)
         {
            ar_pattern_t *pat = (ar_pattern_t*)raw;
            ar_pattern_track_t *track = &pat->tracks[_trackIdx];
            ar_pattern_track_set_trig_flags(track, _stepIdx, (sU16)_val);
         }
      }
   }
}

sUI AR_Pattern::getStepNote(sUI _trackIdx, sUI _stepIdx) {
   if(raw == current_buf)
   {
      if(_trackIdx < 13u)
      {
         if(_stepIdx < 64u)
         {
            const ar_pattern_t *pat = (ar_pattern_t*)raw;
            const ar_pattern_track_t *track = &pat->tracks[_trackIdx];
            return (sUI)ar_pattern_track_get_step_note(track, _stepIdx);
         }
      }
   }
   return 0xFFu;
}

void AR_Pattern::setStepNote(sUI _trackIdx, sUI _stepIdx, sUI _val) {
   if(raw == current_buf)
   {
      if(_trackIdx < 13u)
      {
         if(_stepIdx < 64u)
         {
            ar_pattern_t *pat = (ar_pattern_t*)raw;
            ar_pattern_track_t *track = &pat->tracks[_trackIdx];
            ar_pattern_track_set_step_note(track, _stepIdx, (sU8)_val);
         }
      }
   }
}

sUI AR_Pattern::getStepVelocity(sUI _trackIdx, sUI _stepIdx) {
   if(raw == current_buf)
   {
      if(_trackIdx < 13u)
      {
         if(_stepIdx < 64u)
         {
            const ar_pattern_t *pat = (ar_pattern_t*)raw;
            const ar_pattern_track_t *track = &pat->tracks[_trackIdx];
            return (sUI)ar_pattern_track_get_step_velocity(track, _stepIdx);
         }
      }
   }
   return 0xFFu;
}

void AR_Pattern::setStepVelocity(sUI _trackIdx, sUI _stepIdx, sUI _val) {
   if(raw == current_buf)
   {
      if(_trackIdx < 13u)
      {
         if(_stepIdx < 64u)
         {
            ar_pattern_t *pat = (ar_pattern_t*)raw;
            ar_pattern_track_t *track = &pat->tracks[_trackIdx];
            ar_pattern_track_set_step_velocity(track, _stepIdx, (sU8)_val);
         }
      }
   }
}

sUI AR_Pattern::getStepNoteLength(sUI _trackIdx, sUI _stepIdx) {
   if(raw == current_buf)
   {
      if(_trackIdx < 13u)
      {
         if(_stepIdx < 64u)
         {
            const ar_pattern_t *pat = (ar_pattern_t*)raw;
            const ar_pattern_track_t *track = &pat->tracks[_trackIdx];
            return (sUI)ar_pattern_track_get_step_note_length(track, _stepIdx);
         }
      }
   }
   return 0u;
}

void AR_Pattern::setStepNoteLength(sUI _trackIdx, sUI _stepIdx, sUI _val) {
   if(raw == current_buf)
   {
      if(_trackIdx < 13u)
      {
         if(_stepIdx < 64u)
         {
            ar_pattern_t *pat = (ar_pattern_t*)raw;
            ar_pattern_track_t *track = &pat->tracks[_trackIdx];
            ar_pattern_track_set_step_note_length(track, _stepIdx, (sU8)_val);
         }
      }
   }
}

sSI AR_Pattern::getStepMicroTiming(sUI _trackIdx, sUI _stepIdx) {
   if(raw == current_buf)
   {
      if(_trackIdx < 13u)
      {
         if(_stepIdx < 64u)
         {
            const ar_pattern_t *pat = (ar_pattern_t*)raw;
            const ar_pattern_track_t *track = &pat->tracks[_trackIdx];
            return (sSI)ar_pattern_track_get_step_micro_timing(track, _stepIdx);
         }
      }
   }
   return 0;
}

void AR_Pattern::setStepMicroTiming(sUI _trackIdx, sUI _stepIdx, sSI _val) {
   if(raw == current_buf)
   {
      if(_trackIdx < 13u)
      {
         if(_stepIdx < 64u)
         {
            ar_pattern_t *pat = (ar_pattern_t*)raw;
            ar_pattern_track_t *track = &pat->tracks[_trackIdx];
            ar_pattern_track_set_step_micro_timing(track, _stepIdx, (sS8)_val);
         }
      }
   }
}

// 0..127
sUI AR_Pattern::getStepRetrigLength(sUI _trackIdx, sUI _stepIdx) {
   if(raw == current_buf)
   {
      if(_trackIdx < 13u)
      {
         if(_stepIdx < 64u)
         {
            const ar_pattern_t *pat = (ar_pattern_t*)raw;
            const ar_pattern_track_t *track = &pat->tracks[_trackIdx];
            return (sUI)ar_pattern_track_get_step_retrig_length(track, _stepIdx);
         }
      }
   }
   return 0u;
}

void AR_Pattern::setStepRetrigLength(sUI _trackIdx, sUI _stepIdx, sUI _retrigLength) {
   if(raw == current_buf)
   {
      if(_trackIdx < 13u)
      {
         if(_stepIdx < 64u)
         {
            ar_pattern_t *pat = (ar_pattern_t*)raw;
            ar_pattern_track_t *track = &pat->tracks[_trackIdx];
            ar_pattern_track_set_step_retrig_length(track, _stepIdx, (sU8)_retrigLength);
         }
      }
   }
}

// 0..16
sUI AR_Pattern::getStepRetrigRate(sUI _trackIdx, sUI _stepIdx) {
   if(raw == current_buf)
   {
      if(_trackIdx < 13u)
      {
         if(_stepIdx < 64u)
         {
            const ar_pattern_t *pat = (ar_pattern_t*)raw;
            const ar_pattern_track_t *track = &pat->tracks[_trackIdx];
            return (sUI)ar_pattern_track_get_step_retrig_rate(track, _stepIdx);
         }
      }
   }
   return 0u;
}

void AR_Pattern::setStepRetrigRate(sUI _trackIdx, sUI _stepIdx, sUI _retrigRate) {
   if(raw == current_buf)
   {
      if(_trackIdx < 13u)
      {
         if(_stepIdx < 64u)
         {
            ar_pattern_t *pat = (ar_pattern_t*)raw;
            ar_pattern_track_t *track = &pat->tracks[_trackIdx];
            ar_pattern_track_set_step_retrig_rate(track, _stepIdx, (sU8)_retrigRate);
         }
      }
   }
}

// -128..127
sSI AR_Pattern::getStepRetrigVelocityOffset(sUI _trackIdx, sUI _stepIdx) {
   if(raw == current_buf)
   {
      if(_trackIdx < 13u)
      {
         if(_stepIdx < 64u)
         {
            const ar_pattern_t *pat = (ar_pattern_t*)raw;
            const ar_pattern_track_t *track = &pat->tracks[_trackIdx];
            return (sSI)ar_pattern_track_get_step_retrig_velocity_offset(track, _stepIdx);
         }
      }
   }
   return 0;
}

void AR_Pattern::setStepRetrigVelocityOffset(sUI _trackIdx, sUI _stepIdx, sSI _retrigVelocityOffset) {
   if(raw == current_buf)
   {
      if(_trackIdx < 13u)
      {
         if(_stepIdx < 64u)
         {
            ar_pattern_t *pat = (ar_pattern_t*)raw;
            ar_pattern_track_t *track = &pat->tracks[_trackIdx];
            ar_pattern_track_set_step_retrig_velocity_offset(track, _stepIdx, (sS8)_retrigVelocityOffset);
         }
      }
   }
}

// 0..64
sUI AR_Pattern::getStepTrigCondition(sUI _trackIdx, sUI _stepIdx) {
   if(raw == current_buf)
   {
      if(_trackIdx < 13u)
      {
         if(_stepIdx < 64u)
         {
            const ar_pattern_t *pat = (ar_pattern_t*)raw;
            const ar_pattern_track_t *track = &pat->tracks[_trackIdx];
            return (sUI)ar_pattern_track_get_step_trig_condition(track, _stepIdx);
         }
      }
   }
   return 0u;
}

void AR_Pattern::setStepTrigCondition(sUI _trackIdx, sUI _stepIdx, sSI _condition) {
   if(raw == current_buf)
   {
      if(_trackIdx < 13u)
      {
         if(_stepIdx < 64u)
         {
            ar_pattern_t *pat = (ar_pattern_t*)raw;
            ar_pattern_track_t *track = &pat->tracks[_trackIdx];
            ar_pattern_track_set_step_trig_condition(track, _stepIdx, (sU8)_condition);
         }
      }
   }
}

sBool AR_Pattern::getTrigEnable(sUI _trackIdx, sUI _stepIdx) {
   sUI v = getTrig(_trackIdx, _stepIdx);
   return (0u != (v & AR_TRIG_ENABLE));
}

void AR_Pattern::setTrigEnable (sUI _trackIdx, sUI _stepIdx, sBool _bEnable) {
   sUI v = getTrig(_trackIdx, _stepIdx);
   setTrig(_trackIdx, _stepIdx, v | AR_TRIG_ENABLE);
}

sBool AR_Pattern::getTrigRetrig (sUI _trackIdx, sUI _stepIdx) {
   sUI v = getTrig(_trackIdx, _stepIdx);
   return (0u != (v & AR_TRIG_RETRIG));
}

void AR_Pattern::setTrigRetrig (sUI _trackIdx, sUI _stepIdx, sBool _bEnable) {
   sUI v = getTrig(_trackIdx, _stepIdx);
   setTrig(_trackIdx, _stepIdx, v | AR_TRIG_RETRIG);
}

sBool AR_Pattern::getTrigMute (sUI _trackIdx, sUI _stepIdx) {
   sUI v = getTrig(_trackIdx, _stepIdx);
   return (0u != (v & AR_TRIG_MUTE));
}

void AR_Pattern::setTrigMute (sUI _trackIdx, sUI _stepIdx, sBool _bEnable) {
   sUI v = getTrig(_trackIdx, _stepIdx);
   setTrig(_trackIdx, _stepIdx, v | AR_TRIG_MUTE);
}

sBool AR_Pattern::getTrigAccent (sUI _trackIdx, sUI _stepIdx) {
   sUI v = getTrig(_trackIdx, _stepIdx);
   return (0u != (v & AR_TRIG_ACCENT));
}

void AR_Pattern::setTrigAccent (sUI _trackIdx, sUI _stepIdx, sBool _bEnable) {
   sUI v = getTrig(_trackIdx, _stepIdx);
   setTrig(_trackIdx, _stepIdx, v | AR_TRIG_ACCENT);
}

sBool AR_Pattern::getTrigSwing (sUI _trackIdx, sUI _stepIdx) {
   sUI v = getTrig(_trackIdx, _stepIdx);
   return (0u != (v & AR_TRIG_SWING));
}

void AR_Pattern::setTrigSwing (sUI _trackIdx, sUI _stepIdx, sBool _bEnable) {
   sUI v = getTrig(_trackIdx, _stepIdx);
   setTrig(_trackIdx, _stepIdx, v | AR_TRIG_SWING);
}

sBool AR_Pattern::getTrigSlide (sUI _trackIdx, sUI _stepIdx) {
   sUI v = getTrig(_trackIdx, _stepIdx);
   return (0u != (v & AR_TRIG_SLIDE));
}

void AR_Pattern::setTrigSlide (sUI _trackIdx, sUI _stepIdx, sBool _bEnable) {
   sUI v = getTrig(_trackIdx, _stepIdx);
   setTrig(_trackIdx, _stepIdx, v | AR_TRIG_SLIDE);
}

sBool AR_Pattern::getTrigLFOPLockSW (sUI _trackIdx, sUI _stepIdx) {
   sUI v = getTrig(_trackIdx, _stepIdx);
   return (0u != (v & AR_TRIG_LFO_PL_SW));
}

void  AR_Pattern::setTrigLFOPlockSW (sUI _trackIdx, sUI _stepIdx, sBool _bEnable) {
   sUI v = getTrig(_trackIdx, _stepIdx);
   setTrig(_trackIdx, _stepIdx, v | AR_TRIG_LFO_PL_SW);
}

sBool AR_Pattern::getTrigLFOPLockEN (sUI _trackIdx, sUI _stepIdx) {
   sUI v = getTrig(_trackIdx, _stepIdx);
   return (0u != (v & AR_TRIG_LFO_PL_EN));
}

void  AR_Pattern::setTrigLFOPlockEN (sUI _trackIdx, sUI _stepIdx, sBool _bEnable) {
   sUI v = getTrig(_trackIdx, _stepIdx);
   setTrig(_trackIdx, _stepIdx, v | AR_TRIG_LFO_PL_EN);
}

sBool AR_Pattern::getTrigSYNPLockSW (sUI _trackIdx, sUI _stepIdx) {
   sUI v = getTrig(_trackIdx, _stepIdx);
   return (0u != (v & AR_TRIG_SYN_PL_SW));
}

void  AR_Pattern::setTrigSYNPlockSW (sUI _trackIdx, sUI _stepIdx, sBool _bEnable) {
   sUI v = getTrig(_trackIdx, _stepIdx);
   setTrig(_trackIdx, _stepIdx, v | AR_TRIG_SYN_PL_SW);
}

sBool AR_Pattern::getTrigSYNPLockEN (sUI _trackIdx, sUI _stepIdx) {
   sUI v = getTrig(_trackIdx, _stepIdx);
   return (0u != (v & AR_TRIG_SYN_PL_EN));
}

void  AR_Pattern::setTrigSYNPlockEN (sUI _trackIdx, sUI _stepIdx, sBool _bEnable) {
   sUI v = getTrig(_trackIdx, _stepIdx);
   setTrig(_trackIdx, _stepIdx, v | AR_TRIG_SYN_PL_EN);
}

sBool AR_Pattern::getTrigSMPPLockSW (sUI _trackIdx, sUI _stepIdx) {
   sUI v = getTrig(_trackIdx, _stepIdx);
   return (0u != (v & AR_TRIG_SMP_PL_SW));
}

void  AR_Pattern::setTrigSMPPlockSW (sUI _trackIdx, sUI _stepIdx, sBool _bEnable) {
   sUI v = getTrig(_trackIdx, _stepIdx);
   setTrig(_trackIdx, _stepIdx, v | AR_TRIG_SMP_PL_SW);
}

sBool AR_Pattern::getTrigSMPPLockEN (sUI _trackIdx, sUI _stepIdx) {
   sUI v = getTrig(_trackIdx, _stepIdx);
   return (0u != (v & AR_TRIG_SMP_PL_EN));
}

void AR_Pattern::setTrigSMPPlockEN (sUI _trackIdx, sUI _stepIdx, sBool _bEnable) {
   sUI v = getTrig(_trackIdx, _stepIdx);
   setTrig(_trackIdx, _stepIdx, v | AR_TRIG_SMP_PL_EN);
}

sBool AR_Pattern::getTrigENVPLockSW (sUI _trackIdx, sUI _stepIdx) {
   sUI v = getTrig(_trackIdx, _stepIdx);
   return (0u != (v & AR_TRIG_ENV_PL_SW));
}

void AR_Pattern::setTrigENVPlockSW (sUI _trackIdx, sUI _stepIdx, sBool _bEnable) {
   sUI v = getTrig(_trackIdx, _stepIdx);
   setTrig(_trackIdx, _stepIdx, v | AR_TRIG_ENV_PL_SW);
}

sBool AR_Pattern::getTrigENVPLockEN(sUI _trackIdx, sUI _stepIdx) {
   sUI v = getTrig(_trackIdx, _stepIdx);
   return (0u != (v & AR_TRIG_ENV_PL_EN));
}

void AR_Pattern::setTrigENVPlockEN (sUI _trackIdx, sUI _stepIdx, sBool _bEnable) {
   sUI v = getTrig(_trackIdx, _stepIdx);
   setTrig(_trackIdx, _stepIdx, v | AR_TRIG_ENV_PL_EN);
}

sBool AR_Pattern::getTrigless(sUI _trackIdx, sUI _stepIdx) {
   sUI v = getTrig(_trackIdx, _stepIdx);
   const sUI mask =
      (AR_TRIG_LFO_PL_SW | AR_TRIG_SYN_PL_SW | AR_TRIG_SMP_PL_SW | AR_TRIG_ENV_PL_SW) |
      (AR_TRIG_LFO_PL_EN | AR_TRIG_SYN_PL_EN | AR_TRIG_SMP_PL_EN | AR_TRIG_ENV_PL_EN) ;
   sBool r = ((AR_TRIG_LFO_PL_EN | AR_TRIG_SYN_PL_EN | AR_TRIG_SMP_PL_EN | AR_TRIG_ENV_PL_EN) == (v & mask)) ? YAC_TRUE : YAC_FALSE;
   // yac_host->printf("xxx getTrigLess(trackIdx=%u stepIdx=%u) mask=0x%04x v=0x%04x return=%d\n", _trackIdx, _stepIdx, mask, v, r);
   return r;
}

void AR_Pattern::setTrigless(sUI _trackIdx, sUI _stepIdx, sBool _bEnable) {
   sUI v = getTrig(_trackIdx, _stepIdx);
   v &= ~(AR_TRIG_LFO_PL_SW | AR_TRIG_SYN_PL_SW | AR_TRIG_SMP_PL_SW | AR_TRIG_ENV_PL_SW);
   v |=  (AR_TRIG_LFO_PL_EN | AR_TRIG_SYN_PL_EN | AR_TRIG_SMP_PL_EN | AR_TRIG_ENV_PL_EN);
   setTrig(_trackIdx, _stepIdx, v);
}
