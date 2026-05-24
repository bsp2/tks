// ----
// ---- file   : StEnvelope.cpp
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : Distributed under terms of the MIT license (https://opensource.org/licenses/MIT)
// ----          Copyright 2010-2026 by bsp
// ----
// ----          Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
// ----          associated documentation files (the "Software"), to deal in the Software without restriction, including
// ----          without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// ----          copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to
// ----          the following conditions:
// ----
// ----          The above copyright notice and this permission notice shall be included in all copies or substantial
// ----          portions of the Software.
// ----
// ----          THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
// ----          NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// ----          IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// ----          WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// ----          SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// ----
// ---- info   : This is part of the "syntracker" midi sequencer.
// ----
// ---- created: 14Feb2010
// ---- changed: 23Aug2021, 12Apr2023, 03Oct2024, 22Apr2026, 24May2026
// ----
// ----
// ----

#include "tksampler.h"


StEnvelope::StEnvelope(void) {
   data = NULL;

   intensity = 1.0f;
   timescale = 1.0f; // scaling is relative to ADSR timescale
   exponent  = 1.0f;

   metadata_deleteme = 0;
   metadata_object   = NULL;

   ui_preset_name = NULL;

   last_played_position = 0.0f;
}

StEnvelope::~StEnvelope() {
   YAC_DELETE_SAFE(data);
   deleteMetaData();
   deleteUIPresetName();
}

YAC_Object *StEnvelope::_getOrCreateData(void) {
   if(NULL == data)
   {
#ifndef LIBSYNERGY_BUILD
      data = (TKS_Envelope*) yac_host->yacNew(NULL, "Envelope");
#else
      data = YAC_NEW(TKS_Envelope);
#endif // LIBSYNERGY_BUILD
      data->env_fun = TKS_Envelope::TKS_ENVELOPE_LINEAR;
   }
   return data;
}

YAC_Object *StEnvelope::_getData(void) {
   return data;
}

void StEnvelope::deleteMetaData(void) {
   if(metadata_deleteme)
   {
      YAC_DELETE_SAFE(metadata_object);
   }
}

void StEnvelope::_setMetaData(YAC_Object *_v) {
   YAC_DEREF_ARG(YAC_Object, o, _v);

   deleteMetaData();
   metadata_deleteme = o_deleteme;
   metadata_object   = o;
}

YAC_Object *StEnvelope::_getMetaData(void) {
   return metadata_object;
}

sF32 StEnvelope::_getIntensity(void) {
   return intensity;
}

void StEnvelope::_setIntensity(sF32 _v) {
   intensity = _v;
}

sF32 StEnvelope::_getTimescale(void) {
   return timescale;
}

void StEnvelope::_setTimescale(sF32 _v) {
   if(_v < STENVELOPE_MIN_TIMESCALE)
   {
      _v = STENVELOPE_MIN_TIMESCALE;
   }
   timescale = _v;
}

sF32 StEnvelope::_getTimescaleMillisec(void) {
   // Note: - Unit envelope duration is 1.0
   //       - Time is increased by 1.0 each tick()
   if(timescale > 0.0f)
   {
      return (sF32) ( (1000.0 / timescale) * STSAMPLEVOICE_BLOCK_LEN );
   }
   else
   {
      return 0.0f;
   }
}

void StEnvelope::_setTimescaleMillisec(sF32 _millisec) {
   ////sF32 ts;
   if(_millisec > 0.0f)
   {
      if(_millisec > STENVELOPE_MAX_TIMESCALE_MS)
      {
         timescale = STENVELOPE_MIN_TIMESCALE;
      }
      else
      {
         timescale = 1.0f / (_millisec / (1000.0f*STSAMPLEVOICE_BLOCK_LEN));
         // printf("xxx StEnvelope: millisec=%f timescale=%f\n", _millisec, timescale);
      }
   }
   else
   {
      timescale = 1;
   }
}

sF32 StEnvelope::_getExponent(void) {
   return exponent;
}

void StEnvelope::_setExponent(sF32 _v) {
   exponent = _v;
}

void StEnvelope::deleteUIPresetName(void) {
   YAC_DELETE_SAFE(ui_preset_name);
}

void StEnvelope::_setUiPresetName(YAC_Object *_s) {
   if(YAC_VALID(_s))
   {
      if(NULL == ui_preset_name)
      {
         ui_preset_name = YAC_New_String();
      }
      ui_preset_name->yacCopy(_s);
   }
   else
   {
      deleteUIPresetName();
   }
}

YAC_Object *StEnvelope::_getUiPresetName(void) {
   return ui_preset_name;
}

sF32 StEnvelope::_getLastPlayedPosition(void) {
   // Dyac_host_printf("xxx StEnvelope::_getLastPlayedPosition: last_played_position=%f\n", last_played_position);
   return last_played_position;
}
