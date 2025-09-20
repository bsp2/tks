// ----
// ---- file   : StWaveform.cpp
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
// ----          19Jan2010, 21Jan2010, 05Sep2010, 21Nov2010, 17Feb2011, 22Mar2011, 04May2013
// ----          26May2013, 01Jun2013, 02Jun2013, 15Dec2018, 16Dec2018, 23Dec2018, 01Aug2021
// ----          24Mar2023, 12Apr2023, 28Sep2024, 03Oct2024
// ----
// ----
// ----

#include "tksampler.h"

#include "StWaveform.h"


StWaveform::StWaveform(void) {
   sample_data    = NULL;
   b_own_data     = YAC_FALSE;
   num_channels   = 0u;
   loop_offset    = 0u;
   loop_len       = 0u;
   base_frequency = 440.0f; // or set 261.63/*c-5*/ ??
   sample_rate    = 44100.0f;
   sample_name    = NULL;
   ring_offset    = 0u;

   ui_last_played_offset = -1.0f;
   ui_offset             = 0;
   ui_zoom               = 1.0f;
   ui_type               = 0;
   ui_pathname           = NULL;
   ui_id                 = -1;
   ui_undo               = NULL;
   ui_last_started_voice = NULL;

   bwf_description = NULL;
   bwf_originator  = NULL;
   bwf_date        = NULL;
   bwf_time        = NULL;

   cue_points = NULL;
}

StWaveform::~StWaveform() {
   _free();
}

void StWaveform::_free(void) {
   freeSampleData();

   YAC_DELETE_SAFE(sample_name);

   YAC_DELETE_SAFE(ui_pathname);
   YAC_DELETE_SAFE(ui_undo);

   YAC_DELETE_SAFE(bwf_description);
   YAC_DELETE_SAFE(bwf_originator);
   YAC_DELETE_SAFE(bwf_date);
   YAC_DELETE_SAFE(bwf_time);

   YAC_DELETE_SAFE(cue_points);
}

YAC_Object *StWaveform::_getSampleData(void) {
   return sample_data;
}

sSI StWaveform::_getNumFrames(void) {
   if(num_channels > 0)
   {
      if(NULL != sample_data)
      {
         return sample_data->num_elements / num_channels;
      }
   }
   return 0;
}

sUI StWaveform::_getNumChannels(void) {
   return num_channels;
}

void StWaveform::_setNumChannels(sUI _ch) {
   num_channels = _ch;
}

sSI StWaveform::_getLoopOffset(void) {
   return (sSI) loop_offset;
}

void StWaveform::_setLoopOffset(sSI _loopOff) {
   loop_offset = (sUI) _loopOff;
}

sSI StWaveform::_getLoopLen(void) {
   return (sSI) loop_len;
}

void StWaveform::_setLoopLen(sSI _loopLen) {
   loop_len = (sUI) _loopLen;
}

sF32 StWaveform::_getBaseFrequency(void) {
   return base_frequency;
}

void StWaveform::_setBaseFrequency(sF32 _baseFreq) {
   base_frequency = _baseFreq;
}

sF32 StWaveform::_getSampleRate(void) {
   return sample_rate;
}

void StWaveform::_setSampleRate(sF32 _sampleRate) {
   sample_rate = _sampleRate;
}

void StWaveform::freeSampleData(void) {
   if((NULL != sample_data) && b_own_data)
   {
      YAC_DELETE_SAFE(sample_data);
   }
   sample_data = NULL;
   b_own_data = YAC_FALSE;
}

void StWaveform::_setSampleData(YAC_Object *_fa, sUI _numChannels) {

   YAC_DEREF_ARG(YAC_FloatArray, fa, _fa);

   if(YAC_Is_FloatArray(fa))
   {
      if((void*)sample_data != (void*)fa)
      {
         if((NULL != sample_data) && b_own_data)
         {
            YAC_DELETE_SAFE(sample_data);
         }
         sample_data = fa;
         b_own_data = fa_deleteme;
      }
      num_channels = _numChannels;
   }
   else
   {
      if((NULL != sample_data) && b_own_data)
      {
         YAC_DELETE_SAFE(sample_data);
      }
      sample_data = NULL;
      num_channels = 0;
      b_own_data = YAC_FALSE;
      if(fa_deleteme)
         YAC_DELETE(fa);
   }
}

sSI StWaveform::_alloc(sSI _numChannels, sSI _numFrames) {
   sSI r = 0;
   if((NULL == sample_data) || !b_own_data)
   {
      sample_data = YAC_New_FloatArray();
      b_own_data = YAC_TRUE;
   }
   r = sample_data->yacArrayAlloc( ((sUI)_numChannels) * ((sUI)_numFrames), 0,0,0);
   sample_data->num_elements = _numChannels * _numFrames;
   num_channels = _numChannels;
   return r;
}

YAC_Object *StWaveform::_getName(void) {
   return sample_name;
}

void StWaveform::_setName(YAC_Object *_name) {
   if(NULL == sample_name)
   {
      sample_name = YAC_New_String();
   }
   sample_name->yacCopy(_name);
}

void StWaveform::_uiSetType(sUI _type) {
   ui_type = _type;
}

sUI StWaveform::_uiGetType(void) {
   return ui_type;
}

sF32 StWaveform::_uiGetLastPlayedOffset(void) {
   return ui_last_played_offset;
}

void StWaveform::_uiInvalidateLastPlayedOffset(void) {
   ui_last_played_offset = -1.0f;
}

void StWaveform::_uiSetOffset(sF32 _offset) {
   ui_offset = _offset;
}

sF32 StWaveform::_uiGetOffset(void) {
   return ui_offset;
}

void StWaveform::_uiSetZoom(sF32 _zoom) {
   ui_zoom = _zoom;
}

sF32 StWaveform::_uiGetZoom(void) {
   return ui_zoom;
}

void StWaveform::_uiSetPathName(YAC_Object *_pathName) {
   if(NULL == ui_pathname)
   {
      ui_pathname = YAC_New_String();
   }
   ui_pathname->yacCopy(_pathName);
}

YAC_Object *StWaveform::_uiGetPathName(void) {
   return ui_pathname;
}

sUI StWaveform::_getRingOffset(void) {
   return ring_offset;
}

void StWaveform::_setRingOffset(sUI _off) {
   ring_offset = _off;
}

void StWaveform::_uiSetId(sSI _id) {
   ui_id = _id;
}

sSI StWaveform::_uiGetId(void) {
   return ui_id;
}

void StWaveform::_uiSetUndo(YAC_Object *_undo) {
   YAC_DEREF_ARG(YAC_Object, undo, _undo);

   YAC_DELETE_SAFE(ui_undo);

   if(1 == undo_deleteme)
   {
      ui_undo = undo;
   }
}

YAC_Object *StWaveform::_uiGetUndo(void) {
   return ui_undo;
}

YAC_Object *StWaveform::_getBwfDescription(void) {
   return bwf_description;
}

void StWaveform::_setBwfDescription(YAC_Object *_s) {
   if(YAC_VALID(_s))
   {
      if(NULL == bwf_description)
      {
         bwf_description = YAC_New_String();
      }
      bwf_description->yacCopy(_s);
   }
   else
   {
      YAC_DELETE_SAFE(bwf_description);
   }
}

YAC_Object *StWaveform::_getBwfOriginator(void) {
   return bwf_originator;
}

void StWaveform::_setBwfOriginator(YAC_Object *_s) {
   if(YAC_VALID(_s))
   {
      if(NULL == bwf_originator)
      {
         bwf_originator = YAC_New_String();
      }
      bwf_originator->yacCopy(_s);
   }
   else
   {
      YAC_DELETE_SAFE(bwf_originator);
   }
}

YAC_Object *StWaveform::_getBwfDate(void) {
   return bwf_date;
}

void StWaveform::_setBwfDate(YAC_Object *_s) {
   if(YAC_VALID(_s))
   {
      if(NULL == bwf_date)
      {
         bwf_date = YAC_New_String();
      }
      bwf_date->yacCopy(_s);
   }
   else
   {
      YAC_DELETE_SAFE(bwf_date);
   }
}

YAC_Object *StWaveform::_getBwfTime(void) {
   return bwf_time;
}

void StWaveform::_setBwfTime(YAC_Object *_s) {
   if(YAC_VALID(_s))
   {
      if(NULL == bwf_time)
      {
         bwf_time = YAC_New_String();
      }
      bwf_time->yacCopy(_s);
   }
   else
   {
      YAC_DELETE_SAFE(bwf_time);
   }
}

YAC_Object *StWaveform::_getCuePoints(void) {
   return cue_points;
}

YAC_Object *StWaveform::_getOrCreateCuePoints(void) {
   if(NULL == cue_points)
   {
      cue_points = YAC_New_IntArray();
   }
   return cue_points;
}
