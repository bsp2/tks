// ----
// ---- file   : StSampleBank.cpp
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2009-2024 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : This is part of the "syntracker" midi sequencer.
// ----
// ---- created: 23Sep2009
// ----
// ---- changed: 26Dec2009, 27Dec2009, 29Dec2009, 02Jan2010, 05Jan2010, 07Jan2010, 15Jan2010
// ----          19Jan2010, 21Jan2010, 27Jan2010, 24Mar2013, 23Dec2018, 27Dec2018, 31Dec2018
// ----          17Jan2019, 23Apr2019, 15Feb2020, 25Feb2020, 08Aug2021, 12Apr2023, 18Nov2023
// ----          15Aug2024, 28Sep2024, 30Sep2024, 22Oct2024, 12Nov2024, 11Dec2024
// ----
// ----
// ----

#include "tksampler.h"


StSampleBank::StSampleBank(void) {
   name                         = NULL;
   first_sample                 = NULL;
   first_mutex_group            = NULL;
   num_samples                  = 0u;
   max_voices                   = 16u;
   transpose                    = 0.0f;
   freq_rand_amount             = 0.0f;
   volume                       = 1.0f;
   volume_rand_amount           = 0.0f;
   pan                          = 0.0f;
   pan_rand_amount              = 0.0f;
   sampleoffset_rand_amount     = 0.0f;
   b_realloc                    = YAC_FALSE;
   b_unique_notes               = YAC_FALSE;
   b_perfctl_freeze_noteoff     = YAC_FALSE;
   liverec_last_process_tick_nr = ~0u;

   ::memset(tuning_tables, 0, sizeof(tuning_tables));
   ::memset(tuning_tables_meta, 0, sizeof(tuning_tables_meta));
   default_tuning_table_idx = -1;
   forced_tuning_table_idx = -1;
}

StSampleBank::~StSampleBank() {
   YAC_DELETE_SAFE(name);

   _freeSamples();
   _freeMutexGroups();
   _freeTuningTables();
   _freeTuningTablesMetaData();
}

void StSampleBank::_freeSamples(void) {
   if(NULL != first_sample)
   {
      StSample *c = first_sample;
      while(NULL != c)
      {
         StSample *n = c->next;
         YAC_DELETE(c);
         c = n;
      }
      first_sample = NULL;
      num_samples = 0u;
   }
}

void StSampleBank::_freeMutexGroups(void) {
   if(NULL != first_mutex_group)
   {
      StSampleMutexGroup *c = first_mutex_group;
      while(NULL != c)
      {
         StSampleMutexGroup *n = c->next;
         YAC_DELETE(c);
         c = n;
      }
      first_mutex_group = NULL;
   }
}

sUI StSampleBank::_getNumSamples(void) {
   return num_samples;
}

YAC_Object *StSampleBank::_getFirstSample(void) {
   return first_sample;
}

void StSampleBank::_unlinkFirstSample(YAC_Value *_r) {
   if(NULL != first_sample)
   {
      StSample *s = first_sample;
      _r->initObject(s, YAC_TRUE/*bDeleteMe*/);
      first_sample = s->next;
      s->next = NULL;
      num_samples--;
   }
   else
   {
      _r->initNull();
   }
}

YAC_Object *StSampleBank::_getSampleByIdx(sUI _idx) {
   StSample *s = first_sample;
   sUI i = 0u;
   while( (i++ < _idx) && (NULL != s) )
      s = s->next;
   return s;
}

sSI StSampleBank::_getSampleIdx(YAC_Object *_s) {
   StSample *c = first_sample;
   sUI i = 0u;
   while(NULL != c)
   {
      if((void*)c == (void*)_s)
         return sSI(i);
      i++;
      c = c->next;
   }
   return -1;
}

void StSampleBank::_addSample(YAC_Object *_sample) {
   YAC_DEREF_ARG(StSample, sample, _sample);
   if(sample_deleteme)
   {
      if(YAC_CHK(sample, clid_StSample))
      {
         sample->parent_samplebank = this;

         // Add sample to end of list
         StSample *s = first_sample;
         if(NULL == s)
         {
            first_sample = sample;
         }
         else
         {
            while(NULL != s->next)
               s = s->next;
            s->next = sample;
         }
         num_samples++;
         updateAltGroups();
      }
      else
      {
         YAC_DELETE(sample);
         Dyac_throw_def(InvalidPointer, "_sample is not a valid StSample object");
      }
   }
   else
   {
      Dyac_throw_def(InvalidPointer, "_sample is not a deletable StSample object");
   }
}

void StSampleBank::_addSampleAfter(YAC_Object *_sample, YAC_Object *_prevSample) {
   YAC_DEREF_ARG(StSample, sample, _sample);
   if(sample_deleteme)
   {
      if(YAC_CHK(sample, clid_StSample))
      {
         sample->parent_samplebank = this;

         sBool bAdded = YAC_FALSE;

         StSample *s = first_sample;
         while(NULL != s)
         {
            if(s == _prevSample)
            {
               // SLL insert
               sample->next = s->next;
               s->next = sample;
               bAdded = YAC_TRUE;
               break;
            }
            s = s->next;
         }

         if(bAdded)
         {
            num_samples++;
            updateAltGroups();
         }
         else
         {
            Dyac_host_printf("[!!!] StSampleBank::addSamplerAfter: failed to find prevSample=%p\n", _prevSample);
         }
      }
      else
      {
         YAC_DELETE(sample);
         Dyac_throw_def(InvalidPointer, "_sample is not a valid StSample object");
      }
   }
   else
   {
      Dyac_throw_def(InvalidPointer, "_sample is not a deletable StSample object");
   }
}

void StSampleBank::_removeSample(YAC_Object *_sample) {
   if(YAC_CHK(_sample, clid_StSample))
   {
      StSample *s = first_sample;
      if(NULL == s)
      {
         return;
      }
      else if(_sample == s)
      {
         first_sample = s->next;
         YAC_DELETE(_sample);
         num_samples--;
         updateAltGroups();
      }
      else
      {
         while( (NULL != s) && (s->next != _sample) )
         {
            s = s->next;
         }
         if(NULL != s)
         {
            StSample *n = s->next;
            s->next = n->next;
            YAC_DELETE(n);
            num_samples--;
            updateAltGroups();
         }
      }
   }
   else
   {
      Dyac_throw_def(InvalidPointer, "_sample is not a valid StSample object");
   }
}

void StSampleBank::_setEnableSkipRange(sBool _bEnable) {
   StSample *s = first_sample;
   while(NULL != s)
   {
      s->b_skip_range = _bEnable;
      s = s->next;
   }
}

void StSampleBank::_swapSamples(sUI _idxLeft) {
   if( (_idxLeft < num_samples) && ((_idxLeft + 1u) < num_samples) )
   {
      if(0u == _idxLeft)
      {
         StSample *oldFirst = first_sample;
         StSample *oldNext = first_sample->next->next;
         first_sample = oldFirst->next;
         first_sample->next = oldFirst;
         oldFirst->next = oldNext;
         updateAltGroups();
      }
      else
      {
         StSample *s = first_sample;
         StSample *p = NULL;
         sUI idx = 0u;
         while(NULL != s)
         {
            if(idx == _idxLeft)
            {
               StSample *oldFirst = p->next;
               StSample *oldNext = p->next->next->next;
               p->next = oldFirst->next;
               p->next->next = oldFirst;
               oldFirst->next = oldNext;
               updateAltGroups();
               break;
            }
            else
            {
               idx++;
               p = s;
               s = s->next;
            }
         }
      }
   }
}

YAC_Object *StSampleBank::_getFirstMutexGroup(void) {
   return first_mutex_group;
}

YAC_Object *StSampleBank::_getMutexGroupByIdx(sUI _idx) {
   StSampleMutexGroup *mgrp = first_mutex_group;
   sUI i = 0u;
   while( (i++ < _idx) && (NULL != mgrp) )
      mgrp = mgrp->next;
   return mgrp;
}

void StSampleBank::_addMutexGroup(YAC_Object *_mtxgrp) {
   YAC_DEREF_ARG(StSampleMutexGroup, mtxgrp, _mtxgrp);

   if(mtxgrp_deleteme)
   {
      if(YAC_CHK(mtxgrp, clid_StSampleMutexGroup))
      {
         StSampleMutexGroup *mg = first_mutex_group;
         if(NULL == mg)
         {
            first_mutex_group = mtxgrp;
         }
         else
         {
            while(NULL != mg->next)
               mg = mg->next;
            mg->next = mtxgrp;
         }
      }
      else
      {
         YAC_DELETE(mtxgrp);
         Dyac_throw_def(InvalidPointer, "_mtxgrp is not a valid StSampleMutexGroup object");
      }
   }
   else
   {
      Dyac_throw_def(InvalidPointer, "_mtxgrp is not a deletable StSampleMutexGroup object");
   }
}

void StSampleBank::_removeMutexGroup(YAC_Object *_mtxgrp) {
   if(YAC_CHK(_mtxgrp, clid_StSampleMutexGroup))
   {
      StSampleMutexGroup *mg = first_mutex_group;
      if(NULL == mg)
      {
         return;
      }
      else if(_mtxgrp == mg)
      {
         first_mutex_group = mg->next;
         YAC_DELETE(_mtxgrp);
      }
      else
      {
         while( (NULL != mg) && (mg->next != _mtxgrp) )
         {
            mg = mg->next;
         }
         if(NULL != mg)
         {
            StSampleMutexGroup *n = mg->next;
            mg->next = n->next;
            YAC_DELETE(n);
         }
      }
   }
   else
   {
      Dyac_throw_def(InvalidPointer, "_mtxgrp is not a valid StSampleMutexGroup object");
   }
}

sSI StSampleBank::_getMutexGroupIndexByObject(YAC_Object *_mtxgrp) {
   StSampleMutexGroup *g = first_mutex_group;
   sSI r = 0;
   while(NULL != g)
   {
      if(_mtxgrp == g)
         return r;
      r++;
      g = g->next;
   }
   return r;
}

sUI StSampleBank::_getNumMutexGroups(void) {
   sUI r = 0;
   StSampleMutexGroup *g = first_mutex_group;
   while(NULL != g)
   {
      r++;
      g = g->next;
   }
   return r;
}

YAC_Object *StSampleBank::_getName(void) {
   return name;
}

void StSampleBank::_setName(YAC_Object *_str) {
   if(NULL == name)
      name = YAC_New_String();
   name->yacCopy(_str);
}

void StSampleBank::_setMaxVoices(sUI _maxVoices) {
   max_voices = _maxVoices;
}

sUI StSampleBank::_getMaxVoices(void) {
   return max_voices;
}

void StSampleBank::_setTranspose(sF32 _transpose) {
   transpose = _transpose;
}

sF32 StSampleBank::_getTranspose(void) {
   return transpose;
}

void StSampleBank::_setFreqRandAmount(sF32 _amt) {
   freq_rand_amount = _amt;
}

sF32 StSampleBank::_getFreqRandAmount(void) {
   return freq_rand_amount;
}

void StSampleBank::_setVolume(sF32 _vol) {
   volume = _vol;
}

sF32 StSampleBank::_getVolume(void) {
   return volume;
}

void StSampleBank::_setVolumeRandAmount(sF32 _amt) {
   volume_rand_amount = _amt;
}

sF32 StSampleBank::_getVolumeRandAmount(void) {
   return volume_rand_amount;
}

void StSampleBank::_setPan(sF32 _pan) {
   pan = _pan;
}

sF32 StSampleBank::_getPan(void) {
   return pan;
}

void StSampleBank::_setPanRandAmount(sF32 _amt) {
   pan_rand_amount = _amt;
}

sF32 StSampleBank::_getPanRandAmount(void) {
   return pan_rand_amount;
}

void StSampleBank::_setSampleOffsetRandAmount(sF32 _amt) {
   sampleoffset_rand_amount = _amt;
}

sF32 StSampleBank::_getSampleOffsetRandAmount(void) {
   return sampleoffset_rand_amount;
}

void StSampleBank::_setEnableRealloc(sBool _bEnable) {
   b_realloc = _bEnable;
}

sSI StSampleBank::_getEnableRealloc(void) {
   return b_realloc;
}

void StSampleBank::_setEnableUniqueNotes(sBool _bEnable) {
   b_unique_notes = _bEnable;
}

sSI StSampleBank::_getEnableUniqueNotes(void) {
   return b_unique_notes;
}

void StSampleBank::updateAltGroups(void) {
   StSample *s;
   StSample *curAltHdr = NULL;

   // Reset all alt groups
   s = first_sample;
   while(NULL != s)
   {
      s->alt.num = 0u;
      s->alt.hdr = NULL;
      s = s->next;
   }

   // Scan samples and build alt groups
   s = first_sample;
   while(NULL != s)
   {
      // Dyac_host_printf("xxx curAltHdr=%p s=%p altEn=%d\n", curAltHdr, s, s->alt.b_enable);

      if(s->alt.b_enable)
      {
         sBool bNewHdr = (NULL == curAltHdr);

         if(bNewHdr)
         {
            curAltHdr = s;
         }
         else
         {
            sBool bMatch = YAC_TRUE;

            if(NULL != curAltHdr->key_range)
            {
               if(NULL != s->key_range)
               {
                  bMatch = bMatch &&
                     (sSI(curAltHdr->key_range->lo) == sSI(s->key_range->lo)) &&
                     (sSI(curAltHdr->key_range->hi) == sSI(s->key_range->hi)) ;
                  // Dyac_host_printf("xxx match key=%d\n", bMatch);
               }
            }

            if(NULL != curAltHdr->vel_range)
            {
               if(NULL != s->vel_range)
               {
                  bMatch = bMatch &&
                     (sSI(curAltHdr->vel_range->lo * 127) == sSI(s->vel_range->lo * 127)) &&
                     (sSI(curAltHdr->vel_range->hi * 127) == sSI(s->vel_range->hi * 127)) ;
                  // Dyac_host_printf("xxx match vel=%d\n", bMatch);
               }
            }

            if(NULL != curAltHdr->mod_range)
            {
               if(NULL != s->mod_range)
               {
                  bMatch = bMatch &&
                     (sSI(curAltHdr->mod_range->lo * 127) == sSI(s->mod_range->lo * 127)) &&
                     (sSI(curAltHdr->mod_range->hi * 127) == sSI(s->mod_range->hi * 127)) ;
                  // Dyac_host_printf("xxx match mod=%d\n", bMatch);
                  // Dyac_host_printf("xxx hdr.mod=(%d; %d) s.mod=(%d; %d)\n",
                  //                  sSI(curAltHdr->mod_range->lo * 127),
                  //                  sSI(curAltHdr->mod_range->hi * 127),
                  //                  sSI(s->mod_range->lo * 127),
                  //                  sSI(s->mod_range->hi * 127)
                  //                  );
               }
            }

            if(bMatch)
            {
               // Matches alt group header sample ranges, add to group
               if(0u == curAltHdr->alt.num)
               {
                  // lazy-add alt header sample
                  curAltHdr->alt.samples[curAltHdr->alt.num++] = curAltHdr;
               }
               if(curAltHdr->alt.num < STSAMPLE_MAX_ALT_SAMPLES)
               {
                  curAltHdr->alt.samples[curAltHdr->alt.num++] = s;
                  s->alt.hdr = curAltHdr;
               }
               else
               {
                  Dyac_host_printf("[~~~] StSampleBank::updateAltGroups: MAX_ALT_SAMPLES(%u) reached.\n", STSAMPLE_MAX_ALT_SAMPLES);
                  // // curAltHdr = s;  // force-start new alt group
               }
            }
            else
            {
               // Key/Vel/Mod ranges don't match, start new alt group
               curAltHdr = s;
            }
         }
      } // if alt.b_enable
      else
      {
         curAltHdr = NULL;
      }

      s = s->next;
   }

   // Init non-repeating random number generator(s)
   s = first_sample;
   while(NULL != s)
   {
      if(s->alt.b_enable && (s->alt.num > 0u))
      {
         s->alt.rand_num_left = s->alt.num;
         for(sUI i = 0u; i < s->alt.rand_num_left; i++)
            s->alt.rand_tbl[i] = i;
      }
      s = s->next;
   }
}

sSI StSampleBank::_getNumAltGroups(void) {
   sSI r = 0;
   StSample *s = first_sample;
   while(NULL != s)
   {
      if(s->alt.b_enable && (s->alt.num > 0u))
         r++;
      s = s->next;
   }
   return r;
}

void StSampleBank::_setEnablePerfCtlFreezeNoteOff(sBool _bEnable) {
   b_perfctl_freeze_noteoff = _bEnable;
}

sBool StSampleBank::_getEnablePerfCtlFreezeNoteOff(void) {
   return b_perfctl_freeze_noteoff;
}

sBool StSampleBank::_setTuningTable(sUI _idx, YAC_Object *_fa) {
   if(_idx < STSAMPLE_MAX_TUNING_TABLES)
   {
      if(YAC_Is_FloatArray(_fa))
      {
         YAC_CAST_ARG(YAC_FloatArray, fa, _fa);
         if(fa->num_elements >= 128u)
         {
            if(NULL == tuning_tables[_idx])
               tuning_tables[_idx] = new(std::nothrow)sF32[128];
            ::memcpy((void*)tuning_tables[_idx], (const void*)fa->elements, sizeof(sF32)*128);
            return YAC_TRUE;
         }
      }

      // Failed to update table (e.g. 'fa' is null), free old one
      if(NULL != tuning_tables[_idx])
      {
         delete [] tuning_tables[_idx];
         tuning_tables[_idx] = NULL;
      }
   }
   return YAC_FALSE;
}

void StSampleBank::_getTuningTable(sUI _idx, YAC_Value *_r) {
   _r->initNull();
   if(_idx < STSAMPLE_MAX_TUNING_TABLES)
   {
      if(NULL != tuning_tables[_idx])
      {
         YAC_FloatArray *fa = YAC_New_FloatArray();
         if(NULL != fa && fa->yacArrayAlloc(128))
         {
            ::memcpy((void*)fa->elements, (const void*)tuning_tables[_idx], sizeof(sF32)*128);
            fa->num_elements = 128u;
            _r->initObject(fa, YAC_TRUE);
         }
      }
   }
}

void StSampleBank::_setDefaultTuningTableIdx(sSI _idx) {
   default_tuning_table_idx = _idx;
}

sSI StSampleBank::_getDefaultTuningTableIdx(void) {
   return default_tuning_table_idx;
}

void StSampleBank::_setForcedTuningTableIdx(sSI _idx) {
   forced_tuning_table_idx = _idx;
}

sSI StSampleBank::_getForcedTuningTableIdx(void) {
   return forced_tuning_table_idx;
}

void StSampleBank::_setTuningTableMetaData(sUI _idx, YAC_Object *_voOrNull) {
   if(_idx < STSAMPLE_MAX_TUNING_TABLES)
   {
      if(NULL != tuning_tables_meta[_idx])
      {
         YAC_DELETE_SAFE(tuning_tables_meta[_idx]);
      }

      if(YAC_VALID(_voOrNull))
      {
         YAC_DEREF_ARG(YAC_Object, o, _voOrNull);
         if(o_deleteme)
         {
            tuning_tables_meta[_idx] = o;
         }
      }
   }
}

YAC_Object *StSampleBank::_getTuningTableMetaData(sUI _idx) {
   if(_idx < STSAMPLE_MAX_TUNING_TABLES)
      return tuning_tables_meta[_idx];
   return NULL;
}

void StSampleBank::_freeTuningTables(void) {
   for(sUI i = 0u; i < STSAMPLE_MAX_TUNING_TABLES; i++)
   {
      if(NULL != tuning_tables[i])
      {
         delete [] tuning_tables[i];
         tuning_tables[i] = NULL;
      }
   }
}

void StSampleBank::_freeTuningTablesMetaData(void) {
   for(sUI i = 0u; i < STSAMPLE_MAX_TUNING_TABLES; i++)
   {
      if(NULL != tuning_tables_meta[i])
      {
         YAC_DELETE_SAFE(tuning_tables_meta[i]);
      }
   }
}

const sF32 *StSampleBank::getCurrentTuningTableOrNull(void) {
   const sF32 *ret = NULL;
   if(forced_tuning_table_idx >= 0 && forced_tuning_table_idx < STSAMPLE_MAX_TUNING_TABLES)
   {
      ret = tuning_tables[forced_tuning_table_idx];
   }
   else if(default_tuning_table_idx >= 0 && default_tuning_table_idx < STSAMPLE_MAX_TUNING_TABLES)
   {
      ret = tuning_tables[default_tuning_table_idx];
   }
   return ret;
}
