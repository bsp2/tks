// ----
// ---- file   : gain.c
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2018-2026 by Bastian Spiegel. 
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See 
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : amplifier
// ----
// ---- created: 24Feb2018
// ---- changed: 27Feb2018, 02Mar2018, 17Mar2018, 26Mar2018, 21Jul2018, 29Mar2019, 30Jul2019
// ----          06Feb2021, 27Dec2021, 10Jan2022, 06Mar2023, 19Sep2024, 23Sep2024, 07Jan2025
// ----          13Dec2025, 20Dec2025, 24Apr2026, 19May2026
// ----
// ----
// ----

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../../../plugin.h"

#define PARAM_NONE          0  // for compatibility with ModGain
#define PARAM_LEVEL         1  // 0..0.5(*1.0)..1(*48)
#define PARAM_PAN           2  // 0..0.5..1
#define PARAM_STEREO_WIDTH  3  // 0..0.5..1
#define PARAM_FLIP_PHASE    4  // >=0.5
#define PARAM_FLIP_CHANNELS 5  // >=0.5
#define NUM_PARAMS          6
static const char *loc_param_names[NUM_PARAMS] = {
   "-",
   "Level",
   "Pan",
   "Stereo Width",
   "Flip Phase",
   "Flip Channels",
};
static float loc_param_resets[NUM_PARAMS] = {
   0.0f,     // NONE
   0.5f,     // LEVEL
   0.5f,     // PAN
   0.5f,     // STEREO_WIDTH
   0.0f,     // FLIP_PHASE
   0.0f,     // FLIP_CHANNELS
};

#define MOD_NONE          0
#define MOD_LEVEL         1
#define MOD_PAN           2
#define MOD_STEREO_WIDTH  3
#define NUM_MODS          4
static const char *loc_mod_names[NUM_MODS] = {
   "-",
   "Level",
   "Pan",
   "Stereo Width"
};

typedef struct gain_info_s {
   st_plugin_info_t base;
} gain_info_t;

typedef struct gain_shared_s {
   st_plugin_shared_t base;
   float params[NUM_PARAMS];
} gain_shared_t;

typedef struct gain_voice_s {
   st_plugin_voice_t base;
   float    mods[NUM_MODS];
   float    mod_level_cur;
   float    mod_level_inc;
   float    mod_pan_cur;
   float    mod_pan_inc;
   float    mod_stereo_width_cur;
   float    mod_stereo_width_inc;
} gain_voice_t;


static const char *ST_PLUGIN_API loc_get_param_name(st_plugin_info_t *_info,
                                                    unsigned int      _paramIdx
                                                    ) {
   (void)_info;
   return loc_param_names[_paramIdx];
}

static float ST_PLUGIN_API loc_get_param_reset(st_plugin_info_t *_info,
                                               unsigned int      _paramIdx
                                               ) {
   (void)_info;
   return loc_param_resets[_paramIdx];
}

static float ST_PLUGIN_API loc_get_param_value(st_plugin_shared_t *_shared,
                                               unsigned int        _paramIdx
                                               ) {
   ST_PLUGIN_SHARED_CAST(gain_shared_t);
   return shared->params[_paramIdx];
}

static void ST_PLUGIN_API loc_set_param_value(st_plugin_shared_t *_shared,
                                              unsigned int        _paramIdx,
                                              float               _value
                                              ) {
   ST_PLUGIN_SHARED_CAST(gain_shared_t);
   shared->params[_paramIdx] = _value;
}

static const char *ST_PLUGIN_API loc_get_mod_name(st_plugin_info_t *_info,
                                                  unsigned int      _modIdx
                                                  ) {
   (void)_info;
   return loc_mod_names[_modIdx];
}

static void ST_PLUGIN_API loc_note_on(st_plugin_voice_t  *_voice,
                                      int                 _bGlide,
                                      unsigned char       _note,
                                      float               _vel
                                      ) {
   ST_PLUGIN_VOICE_CAST(gain_voice_t);
   (void)_bGlide;
   (void)_note;
   (void)_vel;
   if(!_bGlide)
   {
      memset((void*)voice->mods, 0, sizeof(voice->mods));
   }
}

static void ST_PLUGIN_API loc_set_mod_value(st_plugin_voice_t *_voice,
                                            unsigned int       _modIdx,
                                            float              _value,
                                            unsigned           _frameOffset
                                            ) {
   ST_PLUGIN_VOICE_CAST(gain_voice_t);
   (void)_frameOffset;
   voice->mods[_modIdx] = _value;
}

static void ST_PLUGIN_API loc_prepare_block(st_plugin_voice_t *_voice,
                                            unsigned int       _numFrames,
                                            float              _freqHz,
                                            float              _note,
                                            float              _vol,
                                            float              _pan
                                            ) {
   ST_PLUGIN_VOICE_CAST(gain_voice_t);
   ST_PLUGIN_VOICE_SHARED_CAST(gain_shared_t);
   (void)_freqHz;
   (void)_note;
   (void)_vol;
   (void)_pan;

   float modLevel = shared->params[PARAM_LEVEL];
   if(modLevel <= 0.5f)
      modLevel *= 2.0f;
   else
      modLevel = 1.0f + ((modLevel-0.5f)*(2.0f*(48.0f-1.0f)));
   modLevel += voice->mods[MOD_LEVEL];
   modLevel = Dstplugin_clamp(modLevel, 0.0f, 48.0f/*+50dB*/);

   float modPan = ((shared->params[PARAM_PAN] - 0.5f) * 2.0f) + voice->mods[MOD_PAN];
   modPan = Dstplugin_clamp(modPan, -1.0f, 1.0f);

   float modStereoWidth = (shared->params[PARAM_STEREO_WIDTH] * 2.0f) + voice->mods[MOD_STEREO_WIDTH];
   modStereoWidth = Dstplugin_clamp(modStereoWidth, 0.0f, 2.0f);

   if(_numFrames > 0u)
   {
      // lerp
      float recBlockSize = (1.0f / _numFrames);
      voice->mod_level_inc        = (modLevel       - voice->mod_level_cur)        * recBlockSize;
      voice->mod_pan_inc          = (modPan         - voice->mod_pan_cur)          * recBlockSize;
      voice->mod_stereo_width_inc = (modStereoWidth - voice->mod_stereo_width_cur) * recBlockSize;
   }
   else
   {
      // initial params/modulation (first block, not rendered)
      voice->mod_level_cur        = modLevel;
      voice->mod_level_inc        = 0.0f;
      voice->mod_pan_cur          = modPan;
      voice->mod_pan_inc          = 0.0f;
      voice->mod_stereo_width_cur = modStereoWidth;
      voice->mod_stereo_width_inc = 0.0f;
   }
}

static void ST_PLUGIN_API loc_process_replace(st_plugin_voice_t  *_voice,
                                              int                 _bMonoIn,
                                              const float        *_samplesIn,
                                              float              *_samplesOut, 
                                              unsigned int        _numFrames
                                              ) {
   // Ring modulate at (modulated) note frequency
   ST_PLUGIN_VOICE_CAST(gain_voice_t);
   ST_PLUGIN_VOICE_SHARED_CAST(gain_shared_t);

   unsigned int k = 0u;

   const float flipPhase = (shared->params[PARAM_FLIP_PHASE] >= 0.5f) ? -1.0f : 1.0f;
   const unsigned int chL = (shared->params[PARAM_FLIP_CHANNELS] >= 0.5f) ? 1u : 0u;
   const unsigned int chR = (shared->params[PARAM_FLIP_CHANNELS] >= 0.5f) ? 0u : 1u;

   if(_bMonoIn)
   {
      // Mono input, stereo output
      for(unsigned int i = 0u; i < _numFrames; i++)
      {
         float l = _samplesIn[k];
         float outL = l * voice->mod_level_cur * flipPhase;
         const float pan = voice->mod_pan_cur;
         const float levelL = (pan < 0.0f) ? 1.0f : (1.0f - pan);
         const float levelR = (pan > 0.0f) ? 1.0f : (1.0f + pan);
         outL *= levelL;
         float outR = outL * levelR;
         const float m = (outL + outR) * 0.5f;
         outL = m + ((outL - m) * voice->mod_stereo_width_cur);
         outR = m + ((outR - m) * voice->mod_stereo_width_cur);
         _samplesOut[k + chL] = outL;
         _samplesOut[k + chR] = outR;

         // Next frame
         k += 2u;
         voice->mod_level_cur        += voice->mod_level_inc;
         voice->mod_pan_cur          += voice->mod_pan_inc;
         voice->mod_stereo_width_cur += voice->mod_stereo_width_inc;
      }
   }
   else
   {
      // Stereo input, stereo output
      for(unsigned int i = 0u; i < _numFrames; i++)
      {
         float l = _samplesIn[k + 0u];
         float r = _samplesIn[k + 1u];
         float outL = l * voice->mod_level_cur * flipPhase;
         float outR = r * voice->mod_level_cur * flipPhase;
         const float pan = voice->mod_pan_cur;
         const float levelL = (pan < 0.0f) ? 1.0f : (1.0f - pan);
         const float levelR = (pan > 0.0f) ? 1.0f : (1.0f + pan);
         outL *= levelL;
         outR *= levelR;
         const float m = (outL + outR) * 0.5f;
         outL = m + ((outL - m) * voice->mod_stereo_width_cur);
         outR = m + ((outR - m) * voice->mod_stereo_width_cur);
         _samplesOut[k + chL] = outL;
         _samplesOut[k + chR] = outR;

         // Next frame
         k += 2u;
         voice->mod_level_cur        += voice->mod_level_inc;
         voice->mod_pan_cur          += voice->mod_pan_inc;
         voice->mod_stereo_width_cur += voice->mod_stereo_width_inc;
      }
   }

}

static st_plugin_shared_t *ST_PLUGIN_API loc_shared_new(st_plugin_info_t *_info) {
   gain_shared_t *ret = (gain_shared_t *)malloc(sizeof(gain_shared_t));
   if(NULL != ret)
   {
      memset((void*)ret, 0, sizeof(*ret));
      ret->base.info  = _info;
      memcpy((void*)ret->params, (void*)loc_param_resets, NUM_PARAMS * sizeof(float));
   }
   return &ret->base;
}

static void ST_PLUGIN_API loc_shared_delete(st_plugin_shared_t *_shared) {
   free((void*)_shared);
}

static st_plugin_voice_t *ST_PLUGIN_API loc_voice_new(st_plugin_info_t *_info, unsigned int _voiceIdx) {
   (void)_voiceIdx;
   gain_voice_t *ret = (gain_voice_t *)malloc(sizeof(gain_voice_t));
   if(NULL != ret)
   {
      memset((void*)ret, 0, sizeof(*ret));
      ret->base.info = _info;
   }
   return &ret->base;
}

static void ST_PLUGIN_API loc_voice_delete(st_plugin_voice_t *_voice) {
   free((void*)_voice);
}

static void ST_PLUGIN_API loc_plugin_exit(st_plugin_info_t *_info) {
   free((void*)_info);
}

st_plugin_info_t *gain_init(void) {
   gain_info_t *ret = NULL;

   ret = (gain_info_t *)malloc(sizeof(gain_info_t));

   if(NULL != ret)
   {
      memset((void*)ret, 0, sizeof(*ret));

      ret->base.api_version = ST_PLUGIN_API_VERSION;
      ret->base.id          = "bsp gain";  // unique id. don't change this in future builds.
      ret->base.author      = "bsp";
      ret->base.name        = "gain";
      ret->base.short_name  = "gain";
      ret->base.flags       = ST_PLUGIN_FLAG_FX;
      ret->base.category    = ST_PLUGIN_CAT_AMP;
      ret->base.num_params  = NUM_PARAMS;
      ret->base.num_mods    = NUM_MODS;

      ret->base.shared_new         = &loc_shared_new;
      ret->base.shared_delete      = &loc_shared_delete;
      ret->base.voice_new          = &loc_voice_new;
      ret->base.voice_delete       = &loc_voice_delete;
      ret->base.get_param_name     = &loc_get_param_name;
      ret->base.get_param_reset    = &loc_get_param_reset;
      ret->base.get_param_value    = &loc_get_param_value;
      ret->base.set_param_value    = &loc_set_param_value;
      ret->base.get_mod_name       = &loc_get_mod_name;
      ret->base.note_on            = &loc_note_on;
      ret->base.set_mod_value      = &loc_set_mod_value;
      ret->base.prepare_block      = &loc_prepare_block;
      ret->base.process_replace    = &loc_process_replace;
      ret->base.plugin_exit        = &loc_plugin_exit;
   }

   return &ret->base;
}
