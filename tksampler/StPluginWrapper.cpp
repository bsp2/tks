// ----
// ---- file   : StPluginWrapper.h
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2020-2024 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : Plugin descriptor for StSampleVoice plugin
// ----
// ---- created: 16May2020
// ---- changed: 17May2020, 19May2020, 20May2020, 08Jun2020, 09Jun2020, 03Feb2021, 30Jul2021
// ----          12Apr2023, 20Apr2023, 29Jul2023, 16Aug2023, 24Aug2023, 03Sep2023, 30Nov2023
// ----          02Dec2023, 07Dec2023, 22Dec2023, 06Jan2024, 12Jan2024, 19Jan2024, 21Jan2024
// ----          10Aug2024, 15Sep2024, 07Dec2024
// ----
// ----
// ----

#include "tksampler.h"

#ifdef YAC_WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

static float loc_voicebus_interleaved_zeroes[2/*stereo*/ * 128/*STSAMPLEVOICE_MAX_VOICE_BUS_FRAMES*/];
static sBool b_loc_voicebus_zeroes = YAC_FALSE;


// <class.png>
StPluginInfo::StPluginInfo(void) {
   info = NULL;
   index = 0u;

   // Lazy-init voicebus "null" source
   if(!b_loc_voicebus_zeroes)
   {
      b_loc_voicebus_zeroes = YAC_TRUE;
      memset((void*)loc_voicebus_interleaved_zeroes, 0, sizeof(loc_voicebus_interleaved_zeroes));
   }
}

StPluginInfo::~StPluginInfo() {
   if(Dyac_host_yacGetDebugLevel() >= 3u)
      Dyac_host_printf("[trc] StPluginInfo: DTOR this=%p info=%p\n", this, info);
   info = NULL;
}

void StPluginInfo::init(st_plugin_info_t *_info) {
   info = _info;
}

sUI StPluginInfo::getIndex(void) const {
   return index;
}

void StPluginInfo::setIndex(sUI _index) {
   index = _index;
}

void StPluginInfo::getId(YAC_Value *_r) {
   _r->initEmptyString();
   _r->value.string_val->visit(info->id);
}

void StPluginInfo::getAuthor(YAC_Value *_r) {
   _r->initEmptyString();
   _r->value.string_val->visit(info->author);
}

void StPluginInfo::getName(YAC_Value *_r) {
   _r->initEmptyString();
   _r->value.string_val->visit(info->name);
}

void StPluginInfo::getShortName(YAC_Value *_r) {
   _r->initEmptyString();
   _r->value.string_val->visit(info->short_name);
}

sUI StPluginInfo::getFlags(void) {
   return info->flags;
}

sBool StPluginInfo::doVoiceBus(void) {
   return (0u != (info->flags & ST_PLUGIN_FLAG_XMOD));
}

sUI StPluginInfo::getCategoryId(void) {
   return info->category;
}

const char *StPluginInfo::GetCategoryNameC(sUI _category) {
   // (todo) handle multiple categories
   switch(_category)
   {
      case ST_PLUGIN_CAT_UNKNOWN:
      default:
         break;

      case ST_PLUGIN_CAT_RINGMOD:    return "Ring Modulator";
      case ST_PLUGIN_CAT_WAVESHAPER: return "Wave Shaper";
      case ST_PLUGIN_CAT_FILTER:     return "Filter";
      case ST_PLUGIN_CAT_EQ:         return "EQ";
      case ST_PLUGIN_CAT_COMPRESSOR: return "Compressor";
      case ST_PLUGIN_CAT_LIMITER:    return "Limiter";
      case ST_PLUGIN_CAT_ENHANCE:    return "Enhance";
      case ST_PLUGIN_CAT_DAC:        return "DAC";
      case ST_PLUGIN_CAT_LOFI:       return "LoFi";
      case ST_PLUGIN_CAT_GATE:       return "Gate";
      case ST_PLUGIN_CAT_AMP:        return "Amp";
      case ST_PLUGIN_CAT_PAN:        return "Pan";
      case ST_PLUGIN_CAT_PITCH:      return "Pitch";
      case ST_PLUGIN_CAT_CHORUS:     return "Chorus";
      case ST_PLUGIN_CAT_FLANGER:    return "Flanger";
      case ST_PLUGIN_CAT_DELAY:      return "Delay";
      case ST_PLUGIN_CAT_REVERB:     return "Reverb";
      case ST_PLUGIN_CAT_TRANSIENT:  return "Transient";
      case ST_PLUGIN_CAT_CONVOLVER:  return "Convolver";
      case ST_PLUGIN_CAT_MIXER:      return "Mixer";
      case ST_PLUGIN_CAT_COMPARATOR: return "Comparator";
      case ST_PLUGIN_CAT_UTILITY:    return "Utility";
   }

   return "<unknown>";
}

void StPluginInfo::getCategoryName(YAC_Value *_r) {
   _r->initEmptyString();
   _r->value.string_val->copy(GetCategoryNameC(info->category));
}

const char *StPluginInfo::GetCategoryShortNameC(sUI _category) {
   // (todo) handle multiple categories
   switch(_category)
   {
      case ST_PLUGIN_CAT_UNKNOWN:
      default:
         break;

      case ST_PLUGIN_CAT_RINGMOD:    return "RM";
      case ST_PLUGIN_CAT_WAVESHAPER: return "WS";
      case ST_PLUGIN_CAT_FILTER:     return "Flt";
      case ST_PLUGIN_CAT_EQ:         return "EQ";
      case ST_PLUGIN_CAT_COMPRESSOR: return "Comp";
      case ST_PLUGIN_CAT_LIMITER:    return "Lim";
      case ST_PLUGIN_CAT_ENHANCE:    return "Enh";
      case ST_PLUGIN_CAT_DAC:        return "DAC";
      case ST_PLUGIN_CAT_LOFI:       return "LoFi";
      case ST_PLUGIN_CAT_GATE:       return "Gate";
      case ST_PLUGIN_CAT_AMP:        return "Amp";
      case ST_PLUGIN_CAT_PAN:        return "Pan";
      case ST_PLUGIN_CAT_PITCH:      return "Pitch";
      case ST_PLUGIN_CAT_CHORUS:     return "Cho";
      case ST_PLUGIN_CAT_FLANGER:    return "Fla";
      case ST_PLUGIN_CAT_DELAY:      return "Dly";
      case ST_PLUGIN_CAT_REVERB:     return "Rev";
      case ST_PLUGIN_CAT_TRANSIENT:  return "Trns";
      case ST_PLUGIN_CAT_CONVOLVER:  return "Cnvr";
      case ST_PLUGIN_CAT_MIXER:      return "Mix";
      case ST_PLUGIN_CAT_COMPARATOR: return "Cmp";
      case ST_PLUGIN_CAT_UTILITY:    return "Utl";
   }

   return "?";
}

void StPluginInfo::getCategoryShortName(YAC_Value *_r) {
   _r->initEmptyString();
   _r->value.string_val->copy(GetCategoryShortNameC(info->category));
}

sUI StPluginInfo::getNumMods(void) {
   return info->num_mods;
}

void StPluginInfo::getModName(sUI _modIdx, YAC_Value *_r) {
   _r->initEmptyString();
   if(_modIdx < info->num_mods)
   {
      if(NULL != info->get_mod_name)
      {
         const char *modName = info->get_mod_name(info, _modIdx);
         if(NULL != modName)
         {
            _r->value.string_val->visit(modName);
         }
      }
      else
      {
         Dyac_host_printf("[---] StPluginInfo::getModName: plugin does not implement get_mod_name() (modIdx=%u, id=\"%s\")\n", _modIdx, info->id);
      }
   }
}

sUI StPluginInfo::getNumParams(void) {
   return info->num_params;
}

void StPluginInfo::getParamName(sUI _paramIdx, YAC_Value *_r) {
   _r->initEmptyString();
   if(_paramIdx < info->num_params)
   {
      if(NULL != info->get_param_name)
      {
         const char *paramName = info->get_param_name(info, _paramIdx);
         if(NULL != paramName)
         {
            _r->value.string_val->visit(paramName);
         }
      }
      else
      {
         Dyac_host_printf("[---] StPluginInfo::getParamName: plugin does not implement get_param_name() (paramIdx=%u, id=\"%s\")\n", _paramIdx, info->id);
      }
   }
}

sF32 StPluginInfo::getParamReset(sUI _paramIdx) {
   if(_paramIdx < info->num_params)
   {
      if(NULL != info->get_param_reset)
      {
         return info->get_param_reset(info, _paramIdx);
      }
   }
   return 0.0f;
}

void StPluginInfo::getParamGroupName(sUI _paramGroupIdx, YAC_Value *_r) {
   _r->initNull();
   if(NULL != info->get_param_group_name)
   {
      const char *groupName = info->get_param_group_name(info, _paramGroupIdx);
      if(NULL != groupName)
      {
         _r->initEmptyString();
         _r->value.string_val->visit(groupName);
      }
   }
}

sUI StPluginInfo::getParamGroupIdx(sUI _paramIdx) {
   if(_paramIdx < info->num_params)
   {
      if(NULL != info->get_param_group_idx)
         return info->get_param_group_idx(info, _paramIdx);
   }
   return ~0u;
}



void StPluginInfo::getParamSectionName(sUI _paramSectionIdx, YAC_Value *_r) {
   _r->initNull();
   if(NULL != info->get_param_section_name)
   {
      const char *sectionName = info->get_param_section_name(info, _paramSectionIdx);
      if(NULL != sectionName)
      {
         _r->initEmptyString();
         _r->value.string_val->visit(sectionName);
      }
   }
}

sUI StPluginInfo::getParamSectionIdx(sUI _paramIdx) {
   if(_paramIdx < info->num_params)
   {
      if(NULL != info->get_param_section_idx)
         return info->get_param_section_idx(info, _paramIdx);
   }
   return ~0u;
}

sUI StPluginInfo::getArrayParamSize(sUI _paramIdx) {
   if(_paramIdx < info->num_params)
   {
      if(NULL != info->get_array_param_size)
         return info->get_array_param_size(info, _paramIdx);
   }
   return 0u;
}

sUI StPluginInfo::getArrayParamNumVariations(sUI _paramIdx) {
   if(_paramIdx < info->num_params)
   {
      if(NULL != info->get_array_param_num_variations)
         return info->get_array_param_num_variations(info, _paramIdx);
   }
   return 0u;
}

sBool StPluginInfo::getArrayParamElementValueRange(sUI _paramIdx, sUI _elementIdx, YAC_Object *_retStorageMin, YAC_Object *_retStorageMax, YAC_Object *_retDisplayMin, YAC_Object *_retDisplayMax, YAC_Object *_retDisplayPrecision) {
   if(_paramIdx < info->num_params)
   {
      if(NULL != info->get_array_param_size)
      {
         const sUI arraySz = info->get_array_param_size(info, _paramIdx);
         if(_elementIdx < arraySz)
         {
            if(NULL != info->get_array_param_element_value_range)
            {
               float retStorageMin       = 0.0f;
               float retStorageMax       = 1.0f;
               float retDisplayMin       = 0.0f;
               float retDisplayMax       = 1.0f;
               sUI   retDisplayPrecision = 5u;
               int bOk = info->get_array_param_element_value_range(info,
                                                                   _paramIdx,
                                                                   _elementIdx,
                                                                   &retStorageMin,
                                                                   &retStorageMax,
                                                                   &retDisplayMin,
                                                                   &retDisplayMax,
                                                                   &retDisplayPrecision
                                                                   );
               if(YAC_FALSE != bOk)
               {
                  // Copy results to value / number object(s)

                  if(YAC_VALID(_retStorageMin))
                  {
                     _retStorageMin->yacValueOfF32(retStorageMin);
                  }

                  if(YAC_VALID(_retStorageMax))
                  {
                     _retStorageMax->yacValueOfF32(retStorageMax);
                  }

                  if(YAC_VALID(_retDisplayMin))
                  {
                     _retDisplayMin->yacValueOfF32(retDisplayMin);
                  }

                  if(YAC_VALID(_retDisplayMax))
                  {
                     _retDisplayMax->yacValueOfF32(retDisplayMax);
                  }

                  if(YAC_VALID(_retDisplayPrecision))
                  {
                     _retDisplayPrecision->yacValueOfI(retDisplayPrecision);
                  }

                  return YAC_TRUE;
               }
            }
         }
      }
   }
   return YAC_FALSE;
}

void StPluginInfo::getArrayParamElementName(sUI _paramIdx, sUI _elementIdx, YAC_Value *_r) {
   _r->initNull();
   if(_paramIdx < info->num_params)
   {
      if(NULL != info->get_array_param_size)
      {
         const sUI arraySz = info->get_array_param_size(info, _paramIdx);
         if(_elementIdx < arraySz)
         {
            if(NULL != info->get_array_param_element_name)
            {
               const char *elementName = info->get_array_param_element_name(info, _paramIdx, _elementIdx);
               if(NULL != elementName)
               {
                  _r->initEmptyString();
                  _r->value.string_val->visit(elementName);
               }
            }
         }
      }
   }
}

sF32 StPluginInfo::getArrayParamElementReset(sUI _paramIdx, sUI _elementIdx) {
   sF32 r = -999999.0f;
   if(_paramIdx < info->num_params)
   {
      if(NULL != info->get_array_param_size)
      {
         const sUI arraySz = info->get_array_param_size(info, _paramIdx);
         if(_elementIdx < arraySz)
         {
            if(NULL != info->get_array_param_element_reset)
            {
               r = info->get_array_param_element_reset(info, _paramIdx, _elementIdx);
            }
         }
      }
   }
   return r;
}

void StPluginInfo::updateLUT(sUI _curveIdx, YAC_Object *_fa) {
   if(YAC_VALID(_fa))
   {
      if(NULL != info->update_lut)
      {
         const sUI ne = _fa->yacArrayGetNumElements();
         if(ne > 0u && (YAC_TYPE_FLOAT == _fa->yacArrayGetElementType()))
         {
            const sF32 *fa = (sF32*)_fa->yacArrayGetPointer();
            if(NULL != fa)
            {
               info->update_lut(info, _curveIdx, fa, ne);
            }
#ifndef YAC_NO_HOST  // not in synergy_replay build ?
            else
            {
               sF32 *faNew = new(std::nothrow)sF32[ne];
               if(NULL != faNew)
               {
                  YAC_Value v;
                  for(sUI i = 0u; i < ne; i++)
                  {
                     _fa->yacArrayGet(NULL/*context*/, i, &v);
                     v.typecast(YAC_TYPE_FLOAT);
                     faNew[i] = v.value.float_val;
                  }
                  info->update_lut(info, _curveIdx, faNew, ne);
                  delete [] faNew;
               }
            }
#endif // YAC_NO_HOST
         }
      }
   }
}

void StPluginInfo::createSharedInstance(YAC_Value *_r) {

   _r->initNull();

   if(NULL != info->shared_new)
   {
      st_plugin_shared_t *sharedC = info->shared_new(info);

      if(NULL != sharedC)
      {
         StPluginShared *shared = YAC_NEW(StPluginShared);
         shared->init(this, sharedC);
         _r->initObject(shared, 1);
      }
      else
      {
         Dyac_host_printf("[---] StPluginInfo::createSharedInstance: plugin shared_new() returned NULL (id=\"%s\")\n", info->id);
      }
   }
   else
   {
      Dyac_host_printf("[---] StPluginInfo::createSharedInstance: plugin does not implement shared_new() (id=\"%s\")\n", info->id);
   }
}



// <class.png>
StPluginShared::StPluginShared(void) {
   info   = NULL;
   shared = NULL;
}

StPluginShared::~StPluginShared() {
   // Dyac_host_printf("xxx ~StPluginShared() called\n");
   freeShared();
}

void StPluginShared::init(StPluginInfo *_info, st_plugin_shared_t *_shared) {
   info   = _info;
   shared = _shared;
}

void StPluginShared::freeShared(void) {
   if(Dyac_host_yacGetDebugLevel() >= 3u)
      Dyac_host_printf("[trc] StPluginShared::freeShared: this=%p shared=%p\n", this, shared);
   if(NULL != shared)
   {
      if(NULL != info->info->shared_delete)
      {
         info->info->shared_delete(shared);
         shared = NULL;
      }
      else
      {
         Dyac_host_printf("[---] StPluginShared::freeShared: plugin does not implement shared_delete() (id=\"%s\")\n", info->info->id);
      }
   }
}

YAC_Object *StPluginShared::getInfo(void) {
   return info;
}

void StPluginShared::queryDynamicParamName(sUI _paramIdx, YAC_Value *_r) {
   _r->initEmptyString();
   if(_paramIdx < info->info->num_params)
   {
      if(NULL != info->info->query_dynamic_param_name)
      {
         char paramName[16];
         memset(paramName, 0, 16);
         (void)info->info->query_dynamic_param_name(shared, _paramIdx, paramName, 16);
         _r->value.string_val->copy(paramName);
      }
      else if(NULL != info->info->get_param_name)
      {
         // Fallback to static parameter name
         const char *paramName = info->info->get_param_name(info->info, _paramIdx);
         if(NULL != paramName)
         {
            _r->value.string_val->visit(paramName);
         }
      }
      else
      {
         Dyac_host_printf("[~~~] StPluginInfo::queryDynamicParamName: plugin implements neither query_dynamic_param_name() nor get_param_name() (paramIdx=%u, id=\"%s\")\n", _paramIdx, info->info->id);
      }
   }
}

void StPluginShared::queryDynamicModName(sUI _modIdx, YAC_Value *_r) {
   _r->initEmptyString();
   if(_modIdx < info->info->num_mods)
   {
      if(NULL != info->info->query_dynamic_mod_name)
      {
         char modName[16];
         memset(modName, 0, 16);
         (void)info->info->query_dynamic_mod_name(shared, _modIdx, modName, 16);
         _r->value.string_val->copy(modName);
      }
      else if(NULL != info->info->get_mod_name)
      {
         // Fallback to static modulation target name
         const char *modName = info->info->get_mod_name(info->info, _modIdx);
         if(NULL != modName)
         {
            _r->value.string_val->visit(modName);
         }
      }
      else
      {
         Dyac_host_printf("[~~~] StPluginInfo::queryDynamicModName: plugin implements neither query_dynamic_mod_name() nor get_mod_name() (paramIdx=%u, id=\"%s\")\n", _modIdx, info->info->id);
      }
   }
}

void StPluginShared::queryDynamicParamPresetValues(sUI _paramIdx, YAC_Value *_r) {
   _r->initNull();
   if(_paramIdx < info->info->num_params)
   {
      // Dyac_host_printf("xxx StPluginShared::queryDynamicParamPresetValues: paramIdx=%u fxn=%p\n", _paramIdx, info->info->query_dynamic_param_preset_values);
      if(NULL != info->info->query_dynamic_param_preset_values)
      {
         sUI numPresets = info->info->query_dynamic_param_preset_values(shared, _paramIdx, NULL/*retValues*/, 0u/*retValuesSize*/);
         // Dyac_host_printf("xxx StPluginShared::queryDynamicParamPresetValues: paramIdx=%u => numPresets=%u\n", _paramIdx, numPresets);
         if(numPresets > 0u)
         {
            YAC_FloatArray *fa = YAC_New_FloatArray();
            if(NULL != fa)
            {
               if(fa->yacArrayAlloc(numPresets))
               {
                  (void)info->info->query_dynamic_param_preset_values(shared, _paramIdx, fa->elements, numPresets);
                  fa->num_elements = numPresets;
               }
               _r->initObject(fa, 1);
            }
         }
      }
      else
      {
         if(Dyac_host_yacGetDebugLevel() > 2u)
         {
            Dyac_host_printf("[~~~] StPluginInfo::queryDynamicParamPresetValues: plugin does not implement query_dynamic_param_preset_values() (paramIdx=%u, id=\"%s\")\n", _paramIdx, info->info->id);
         }
      }
   }
}

void StPluginShared::queryDynamicParamPresetName(sUI _paramIdx, sUI _presetIdx, YAC_Value *_r) {
   _r->initNull();
   if(_paramIdx < info->info->num_params)
   {
      if(NULL != info->info->query_dynamic_param_preset_values)
      {
         sUI numPresets = info->info->query_dynamic_param_preset_values(shared, _paramIdx, NULL/*retValues*/, 0u/*retValuesSize*/);
         if(numPresets > 0u)
         {
            if(_presetIdx < numPresets)
            {
               YAC_String *s = YAC_New_String();
               if(NULL != s)
               {
                  sUI sSize = info->info->query_dynamic_param_preset_name(shared, _paramIdx, _presetIdx, NULL/*retBuf*/, 0u/*retBufSz*/);
                  if(s->yacArrayAlloc(sSize))
                  {
                     (void)info->info->query_dynamic_param_preset_name(shared, _paramIdx, _presetIdx, (char*)s->chars, sSize);
                     s->length = sSize;
                     s->key = YAC_LOSTKEY;
                  }
                  _r->initString(s, 1);
               }
            }
         }
      }
      else
      {
         if(Dyac_host_yacGetDebugLevel() > 2u)
         {
            Dyac_host_printf("[~~~] StPluginInfo::queryDynamicParamPresetName: plugin does not implement query_dynamic_param_preset_name() (paramIdx=%u, presetIdx=%u, id=\"%s\")\n", _paramIdx, _presetIdx, info->info->id);
         }
      }
   }
}

void StPluginShared::getArrayParamVariationPtr(sUI _paramIdx, sUI _variationIdx, YAC_Value *_r) {
   _r->initNull();
   if(_paramIdx < info->info->num_params)
   {
      sUI arraySize = info->getArrayParamSize(_paramIdx);
      if(arraySize > 0u)
      {
         sUI numVar = info->getArrayParamNumVariations(_paramIdx);
         if(_variationIdx < numVar)
         {
            if(NULL != info->info->get_array_param_variation_ptr)
            {
               float *a = info->info->get_array_param_variation_ptr(shared, _paramIdx, _variationIdx);
               if(NULL != a)
               {
                  YAC_FloatArray *fa = YAC_New_FloatArray();
                  if(NULL != fa)
                  {
                     fa->elements     = a;
                     fa->num_elements = arraySize;
                     fa->max_elements = arraySize;
                     fa->own_data     = YAC_FALSE;
                     _r->initObject(fa, 1);
                  }
               }
            }
         }
      }
   }
}

void StPluginShared::setArrayParamEditVariationIdx(sUI _paramIdx, sSI _variationIdx) {
   if(_paramIdx < info->info->num_params)
   {
      if(NULL != info->info->set_array_param_edit_variation_idx)
      {
         sUI numVar = info->getArrayParamNumVariations(_paramIdx);
         if( (_variationIdx >= 0) && sUI(_variationIdx) < numVar )
         {
            info->info->set_array_param_edit_variation_idx(shared, _paramIdx, _variationIdx);
         }
         else
         {
            info->info->set_array_param_edit_variation_idx(shared, _paramIdx, -1);
         }
      }
   }
}

sF32 StPluginShared::getParamValue(sUI _paramIdx) {
   // yac_host->printf("xxx StPluginShared::getParamValue: paramIdx=%u num=%u\n", _paramIdx, info->info->num_params);
   if(_paramIdx < info->info->num_params)
   {
      if(NULL != info->info->get_param_value)
      {
         return info->info->get_param_value(shared, _paramIdx);
      }
   }
   return 0.0f;
}

void StPluginShared::setParamValue(sUI _paramIdx, sF32 _value) {
   if(_paramIdx < info->info->num_params)
   {
      if(NULL != info->info->set_param_value)
      {
         info->info->set_param_value(shared, _paramIdx, _value);
      }
   }
}

void StPluginShared::getParamValueString(sUI _paramIdx, YAC_Value *_r) {
   _r->initNull();
   if(_paramIdx < info->info->num_params)
   {
      if(NULL != info->info->get_param_value_string)
      {
         _r->initEmptyString();
         _r->value.string_val->alloc(32);
         info->info->get_param_value_string(shared, _paramIdx,
                                            (char*)_r->value.string_val->chars,
                                            _r->value.string_val->buflen
                                            );
         _r->value.string_val->fixLength();
      }
   }
}

void StPluginShared::benchmarkSingle(YAC_FloatArray *_ioBuf, sUI _numFramesPerChunk, sUI _numChunks) {
   sF32 ioBuf[2];
   st_plugin_info_t *cinfo = info->info;
   st_plugin_voice_t *voice = cinfo->voice_new(cinfo, 0u/*voiceIdx*/);
   voice->info = info->info;
   voice->shared = shared;
   float freqHz = 440.0f;
   float freqNote = 12.0f*4.0f+9.0f;
   float vol = 1.0f;
   float pan = 0.0f;
   if(NULL != voice->info->set_sample_rate)
      voice->info->set_sample_rate(voice, 44100.0f);
   voice->info->prepare_block(voice,
                              0u/*numFrames*/,
                              freqHz,
                              freqNote,
                              vol,
                              pan
                              );

   for(sUI j = 0u; j < _numChunks; j++)
   {
      voice->info->prepare_block(voice,
                                 _numFramesPerChunk,
                                 freqHz,
                                 freqNote,
                                 vol,
                                 pan
                                 );

      for(sUI i = 0u; i < _numFramesPerChunk; i++)
      {
         // yac_host->printf("xxx benchmarkSingle: chunkIdx=%u frameIdx=%u  numChunks=%u  numFramesPerChunk=%u\n", j, i, _numChunks, _numFramesPerChunk);
         ioBuf[0] = ioBuf[1] = 0.0f;
         voice->voice_bus_read_offset = 0u;
         voice->voice_bus_buffers = NULL; // (todo)
         voice->info->process_replace(voice,
                                      YAC_FALSE/*bMonoIn*/,
                                      ioBuf,
                                      ioBuf,
                                      1u/*numFrames*/
                                      );
         // yac_host->printf("xxx benchmarkSingle:   END chunkIdx=%u frameIdx=%u\n", j, i);
      }
   }
   voice->info->voice_delete(voice);
}

void StPluginShared::benchmarkChunk(YAC_FloatArray *_ioBuf, sUI _numFramesPerChunk, sUI _numChunks) {
   sF32 *ioBuf = _ioBuf->elements;
   st_plugin_info_t *cinfo = info->info;
   st_plugin_voice_t *voice = cinfo->voice_new(cinfo, 0u/*voiceIdx*/);
   voice->info = info->info;
   voice->shared = shared;
   float freqHz = 440.0f;
   float freqNote = 12.0f*4.0f+9.0f;
   float vol = 1.0f;
   float pan = 0.0f;
   if(NULL != voice->info->set_sample_rate)
      voice->info->set_sample_rate(voice, 44100.0f);
   voice->info->prepare_block(voice,
                              0u/*numFrames*/,
                              freqHz,
                              freqNote,
                              vol,
                              pan
                              );

   for(sUI j = 0u; j < _numChunks; j++)
   {
      voice->info->prepare_block(voice,
                                 _numFramesPerChunk,
                                 freqHz,
                                 freqNote,
                                 vol,
                                 pan
                                 );

      voice->voice_bus_read_offset = 0u;
      voice->voice_bus_buffers = NULL; // (todo)
      voice->info->process_replace(voice,
                                   YAC_FALSE/*bMonoIn*/,
                                   ioBuf,
                                   ioBuf,
                                   _numFramesPerChunk
                                   );
   }

   voice->info->voice_delete(voice);
}

void StPluginShared::createVoiceInstance(sF32 _sampleRate, sUI _voiceIdx, YAC_Value *_r) {

   _r->initNull();

   if(NULL != shared)
   {
      StPluginVoice *voice = YAC_NEW(StPluginVoice);
      voice->init(this, _sampleRate, _voiceIdx);
      _r->initObject(voice, 1);
   }
   else
   {
      Dyac_host_printf("[---] StPluginShared::createVoiceInstance: shared is NULL\n");
   }
}


// <class.png>
StPluginSharedMissing::StPluginSharedMissing(void) {
   plugin_id = NULL;
   num_params = 0u;
   memset((void*)param_values, 0, sizeof(param_values));
   memset((void*)array_params, 0, sizeof(array_params));
}

StPluginSharedMissing::~StPluginSharedMissing(void) {
   YAC_DELETE_SAFE(plugin_id);
   freeArrayParams();
}

void StPluginSharedMissing::freeArrayParam(const sUI _paramIdx) {
   if(NULL != array_params[_paramIdx].fa_data)
   {
      delete [] array_params[_paramIdx].fa_data;
      array_params[_paramIdx].fa_data        = NULL;
      array_params[_paramIdx].array_sz       = 0u;
      array_params[_paramIdx].num_variations = 0u;
   }
}

void StPluginSharedMissing::freeArrayParams(void) {
   for(sUI paramIdx = 0u; paramIdx < MAX_MISSING_PARAMS; paramIdx++)
   {
      freeArrayParam(paramIdx);
   }
}

void StPluginSharedMissing::setPluginId(YAC_String *_pluginId) {
   if(NULL == plugin_id)
   {
      plugin_id = YAC_New_String();
   }
   plugin_id->yacCopy(_pluginId);
}

YAC_String *StPluginSharedMissing::getPluginId(void) {
   return plugin_id;
}

void StPluginSharedMissing::setNumParams(sUI _num) {
   if(_num > MAX_MISSING_PARAMS)
      _num = MAX_MISSING_PARAMS;
   num_params = _num;
}

sUI StPluginSharedMissing::getNumParams(void) {
   return num_params;
}

sF32 StPluginSharedMissing::getParamValue(sUI _paramIdx) {
   if(_paramIdx < MAX_MISSING_PARAMS)
      return param_values[_paramIdx];
   return 0.0f;
}

void StPluginSharedMissing::setParamValue(sUI _paramIdx, sF32 _value) {
   if(_paramIdx < MAX_MISSING_PARAMS)
      param_values[_paramIdx] = _value;
}

sBool StPluginSharedMissing::allocArrayParam(sUI _paramIdx, sUI _arraySz, sUI _numVariations) {
   sBool r = YAC_FALSE;
   if(_paramIdx < MAX_MISSING_PARAMS)
   {
      freeArrayParam(_paramIdx);

      if(_arraySz > 0u && _arraySz <= 256u)
      {
         if(_numVariations > 0u && _numVariations <= 32u)
         {
            sF32 *fa = new(std::nothrow)sF32[_arraySz * _numVariations];
            if(NULL != fa)
            {
               array_params[_paramIdx].array_sz       = _arraySz;
               array_params[_paramIdx].num_variations = _numVariations;
               array_params[_paramIdx].fa_data        = fa;
               r = YAC_TRUE;
            }
         }
      }
   }
   return r;
}

sUI StPluginSharedMissing::getArrayParamSize(sUI _paramIdx) {
   if(_paramIdx < MAX_MISSING_PARAMS)
   {
      return array_params[_paramIdx].array_sz;
   }
   return 0u;
}

sUI StPluginSharedMissing::getArrayParamNumVariations(sUI _paramIdx) {
   if(_paramIdx < MAX_MISSING_PARAMS)
   {
      return array_params[_paramIdx].num_variations;
   }
   return 0u;
}

void StPluginSharedMissing::getArrayParamVariationPtr(sUI _paramIdx, sUI _variationIdx, YAC_Value *_r) {
   _r->initNull();
   if(_paramIdx < MAX_MISSING_PARAMS)
   {
      const sUI arraySz = array_params[_paramIdx].array_sz;
      if(arraySz > 0u)
      {
         if(_variationIdx < array_params[_paramIdx].num_variations)
         {
            if(NULL != array_params[_paramIdx].fa_data)
            {
               YAC_FloatArray *fa = YAC_New_FloatArray();
               if(NULL != fa)
               {
                  fa->elements     = array_params[_paramIdx].fa_data + (_variationIdx * arraySz);
                  fa->num_elements = arraySz;
                  fa->max_elements = arraySz;
                  fa->own_data     = YAC_FALSE;
                  _r->initObject(fa, 1);
               }
            }
         }
      }
   }
}

sUI StPluginSharedMissing::getNumArrayParams(void) {
   sUI r = 0u;
   for(sUI paramIdx = 0u; paramIdx < MAX_MISSING_PARAMS; paramIdx++)
   {
      if(array_params[paramIdx].array_sz > 0u)
         r++;
   }
   return r;
}


// <class.png>
StPluginVoice::StPluginVoice(void) {
   shared = NULL;
   voice  = NULL;
}

StPluginVoice::~StPluginVoice() {
   freeVoice();
}

void StPluginVoice::init(StPluginShared *_shared, sF32 _sampleRate, sUI _voiceIdx) {
   shared = _shared;
   if(NULL != shared)
   {
      // Create new voice instance
      {
         st_plugin_info_t *cinfo = shared->shared->info;
         if(cinfo->api_version < 2u)
         {
            // v1: no 'voiceIdx' parameter
            st_plugin_voice_t *(ST_PLUGIN_API *voiceNew) (st_plugin_info_t *_info) =
               (st_plugin_voice_t *(ST_PLUGIN_API *) (st_plugin_info_t *_info)) cinfo->voice_new;
            voice = voiceNew(cinfo);
         }
         else
         {
            // v2+: pass 'voiceIdx' parameter
            voice = cinfo->voice_new(cinfo, _voiceIdx);
         }
      }

      if(Dyac_host_yacGetDebugLevel() >= 3u)
         Dyac_host_printf("[trc] StPluginVoice::init: info=%p voice=%p idx=%u\n", shared->shared->info, voice, _voiceIdx);

      if(NULL != voice)
      {
         voice->shared = shared->shared;

         // Initialize / reset voice bus(es)
         // // if(0u != (shared->shared->info->flags & ST_PLUGIN_FLAG_XMOD))
         for(sUI voiceBusIdx = 0u; voiceBusIdx < ST_PLUGIN_MAX_LAYERS/*32*/; voiceBusIdx++)
            sc_voice_bus_buffers[voiceBusIdx] = loc_voicebus_interleaved_zeroes;

         if(NULL != voice->info->set_sample_rate)
            voice->info->set_sample_rate(voice, _sampleRate);

         shared->shared->info->note_on(voice,
                                       YAC_FALSE/*bGlide*/,
                                       (12*5)/*note*/,
                                       1.0f/*vel*/
                                       );
      }
   }
   else
   {
      voice = NULL;
   }
}

void StPluginVoice::freeVoice(void) {
   if(Dyac_host_yacGetDebugLevel() >= 3u)
      Dyac_host_printf("[trc] StPluginVoice::freeVoice: this=%p voice=%p\n", this, voice);
   if(NULL != voice)
   {
      if(NULL != shared->shared->info->voice_delete)
      {
         shared->shared->info->voice_delete(voice);
         voice = NULL;
      }
      else
      {
         Dyac_host_printf("[---] StPluginVoice::freeVoice: plugin does not implement voice_delete() (id=\"%s\")\n", shared->shared->info->id);
      }
   }
}

YAC_Object *StPluginVoice::getShared(void) {
   return shared;
}

void StPluginVoice::noteOn(sBool _bGlide, sUI _note, sF32 _vel) {
   if(NULL != voice)
   {
      voice->info->note_on(voice, _bGlide, sU8(_note), _vel);
   }
}

void StPluginVoice::setModValue(sUI _modIdx, sF32 _value, sUI _frameOff) {
   if(_modIdx < 8u)
   {
      if(NULL != voice)
      {
         voice->info->set_mod_value(voice, _modIdx, _value, _frameOff);
      }
   }
}

void StPluginVoice::processReplace(YAC_FloatArray *_ioInterleavedBuf, sUI _numFrames) {
   if(NULL != voice)
   {
      if(_ioInterleavedBuf->max_elements >= (_numFrames * 2u))
      {
         sF32 *ioBuf = _ioInterleavedBuf->elements;

         if(NULL != ioBuf)
         {
            voice->info->prepare_block(voice,
                                       _numFrames,
                                       261.63f/*freq=c-5*/,
                                       (12*5)/*note*/,
                                       1.0f/*vol*/,
                                       0.0f/*pan*/
                                       );

            voice->voice_bus_read_offset = 0u;
            voice->voice_bus_buffers = sc_voice_bus_buffers;
            sc_voice_bus_buffers[0] = loc_voicebus_interleaved_zeroes;

            voice->info->process_replace(voice,
                                         YAC_FALSE/*bMonoIn*/,
                                         ioBuf,
                                         ioBuf,
                                         _numFrames
                                         );
         }
      }
   }
}

void StPluginVoice::processReplaceVoiceBus1SideChain(YAC_FloatArray *_ioInterleavedBuf, YAC_FloatArray *_ioInterleavedBufSC, sUI _numFrames) {
   if(NULL != voice)
   {
      if(_ioInterleavedBuf->max_elements >= (_numFrames * 2u))
      {
         if(_ioInterleavedBufSC->max_elements >= (_numFrames * 2u))
         {
            sF32 *ioBuf   = _ioInterleavedBuf  ->elements;
            sF32 *ioBufSC = _ioInterleavedBufSC->elements;

            if(NULL != ioBuf && NULL != ioBufSC)
            {
               voice->info->prepare_block(voice,
                                          _numFrames,
                                          261.63f/*freq=c-5*/,
                                          (12*5)/*note*/,
                                          1.0f/*vol*/,
                                          0.0f/*pan*/
                                          );

               voice->voice_bus_read_offset = 0u;
               voice->voice_bus_buffers = sc_voice_bus_buffers;
               sc_voice_bus_buffers[0] = ioBufSC;

               voice->info->process_replace(voice,
                                            YAC_FALSE/*bMonoIn*/,
                                            ioBuf,
                                            ioBuf,
                                            _numFrames
                                            );
            }
         }
      }
   }
}


// <class.png>
StPluginLibrary::StPluginLibrary(void) {
   dll_handle = NULL;
   init_fxn   = NULL;
   memset((void*)infos, 0, sizeof(infos));
   num_infos = 0u;
}

StPluginLibrary::~StPluginLibrary() {
   if(Dyac_host_yacGetDebugLevel() >= 3u)
      Dyac_host_printf("[trc] StPluginLibrary: DTOR this=%p num_infos=%u\n", this, num_infos);
   close();
}

#ifdef YAC_WIN32
sBool StPluginLibrary::plafOpenLibrary(const char *_pathName) {
   HINSTANCE dllHandle = ::LoadLibrary((char*)_pathName);

   dll_handle = (void*) dllHandle;

   return (NULL != dll_handle);
}

void StPluginLibrary::plafCloseLibrary(void) {
   HINSTANCE dllHandle = (HINSTANCE) dll_handle;

   ::FreeLibrary(dllHandle);

   dll_handle = NULL;
}

void *StPluginLibrary::plafFindFunction(const char *_name) const {
   HINSTANCE dllHandle = (HINSTANCE) dll_handle;

   FARPROC fxnHandle = ::GetProcAddress(dllHandle, _name);

   return (void*) fxnHandle;
}

const char *StPluginLibrary::plafGetErrorString(void) {
   static char buf[1024];
   DWORD lastError = ::GetLastError();
#ifdef YAC_CYGWIN
   snprintf(buf, 1024, "%u", lastError);
#else
   _snprintf(buf, 1024, "%u", lastError);
#endif // YAC_CYGWIN
   return buf;
}

#else
// Linux, MacOSX
sBool StPluginLibrary::plafOpenLibrary(const char *_pathName) {
   sUI flags = RTLD_NOW;
   // sUI flags = RTLD_GLOBAL;

   dll_handle = ::dlopen(_pathName, flags);

   return (NULL != dll_handle);
}

void StPluginLibrary::plafCloseLibrary(void) {
   if(NULL != dll_handle)
   {
	   ::dlclose(dll_handle);
      dll_handle = NULL;
   }
}

void *StPluginLibrary::plafFindFunction(const char *_name) const {
   void *fxnHandle = ::dlsym(dll_handle, _name);
   return fxnHandle;
}

const char *StPluginLibrary::plafGetErrorString (void) {
   const char *msg = ::dlerror();
   return msg;
}

#endif // YAC_WIN32


sBool StPluginLibrary::open(YAC_String *_pathName) {
   sBool ret = YAC_FALSE;

   if(NULL != dll_handle)
      plafCloseLibrary();

   if(YAC_Is_String(_pathName))
   {
      YAC_CAST_ARG(YAC_String, pathName, _pathName);

      const char *pathNameChars = (const char*)pathName->chars;

      if(plafOpenLibrary(pathNameChars))
      {
         init_fxn = (st_plugin_init_fxn_t) plafFindFunction("st_plugin_init");

         sBool bVerbose = (Dyac_host_yacGetDebugLevel() >= 2u);

         if(NULL != init_fxn)
         {
            // Query plugin infos
            sUI pluginIdx = 0u;
            for(;;)
            {
               st_plugin_info_t *info;
               info = init_fxn(pluginIdx);

               if(NULL != info)
               {
                  if(pluginIdx < ST_PLUGIN_MAX)
                  {
                     if(bVerbose)
                     {
                        Dyac_host_printf("[dbg] StPluginLibrary::open: found plugin[%u] id=\"%s\" author=\"%s\" name=\"%s\"\n", pluginIdx, info->id, info->author, info->name);
                     }
                     infos[pluginIdx++] = info;
                  }
                  else
                  {
                     Dyac_host_printf("[---] StPluginLibrary::open: max number of plugins-per-library exceed (%u)\n", pluginIdx);
                     if(NULL != info->plugin_exit)
                        info->plugin_exit(info);
                     break;
                  }
               }
               else
               {
                  // No more plugins
                  break;
               }
            }

            if(pluginIdx >= 1u)
            {
               // Succeeded
               num_infos = pluginIdx;
               if(bVerbose)
               {
                  Dyac_host_printf("[dbg] StPluginLibrary::open: found %u plugin(s) in library \"%s\"\n", num_infos, pathNameChars);
               }
               ret = YAC_TRUE;
            }
            else
            {
               Dyac_host_printf("[---] StPluginLibrary::open: no plugins found in library \"%s\"\n", pathNameChars);
               plafCloseLibrary();
            }
         }
         else
         {
            Dyac_host_printf("[---] StPluginLibrary::open: failed to resolve \"st_plugin_init\"\n");
            plafCloseLibrary();
         }
      }
      else
      {
         Dyac_host_printf("[---] StPluginLibrary::open: failed to open plugin \"%s\"\n", pathNameChars);
      }
   }

   return ret;
}

void StPluginLibrary::close(void) {
   if(NULL != dll_handle)
   {
      for(sUI infoIdx = 0u; infoIdx < num_infos; infoIdx++)
      {
         if(Dyac_host_yacGetDebugLevel() >= 3u)
            Dyac_host_printf("xxx StPluginLibrary::close: free infoidx=%u id=\"%s\"\n", infoIdx, infos[infoIdx]->id);
         if(NULL != infos[infoIdx]->plugin_exit)
         {
            infos[infoIdx]->plugin_exit(infos[infoIdx]);
         }
         else
         {
            Dyac_host_printf("[---] StPluginLibrary::close: plugin does not implement plugin_exit() (id=\"%s\")\n", infos[infoIdx]->id);
         }
      }

      plafCloseLibrary();

      init_fxn = NULL;
      memset((void*)infos, 0, sizeof(infos));
      num_infos = 0u;
   }
}

void StPluginLibrary::getPluginInfoByIndex(sUI _index, YAC_Value *_r) {
   _r->initNull();

   if(NULL != dll_handle)
   {
      if(_index < num_infos)
      {
         StPluginInfo *r = YAC_NEW(StPluginInfo);
         r->init(infos[_index]);
         _r->initObject(r, 1);
      }
   }
}
