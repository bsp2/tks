// ----
// ---- file   : sr_test.cpp
// ---- author : bsp
// ---- legal  : Distributed under terms of the MIT LICENSE (MIT).
// ----
// ----    Permission is hereby granted, free of charge, to any person obtaining a copy
// ----    of this software and associated documentation files (the "Software"), to deal
// ----    in the Software without restriction, including without limitation the rights
// ----    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// ----    copies of the Software, and to permit persons to whom the Software is
// ----    furnished to do so, subject to the following conditions:
// ----
// ----    The above copyright notice and this permission notice shall be included in
// ----    all copies or substantial portions of the Software.
// ----
// ----    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// ----    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// ----    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// ----    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// ----    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// ----    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// ----    THE SOFTWARE.
// ----
// ---- info   : Standalone Synergy Replay test
// ----           - must clean tksampler before building
// ----           - must build plugins without st_plugin_init:
// ----              % cd tksampler/plugins
// ----              % m STFX_SKIP_MAIN_INIT=1 bin
// ----           - .mid must be exported in multi-track, vst-dev-per-track format
// ----              ('mixed' Multi-Track checkbox state in Synergy ExportSMFDialog)
// ----
// ---- created: 12Apr2023
// ---- changed: 13Apr2023, 14Apr2023, 15Apr2023, 20Apr2023, 21Apr2023, 22Apr2023, 23Apr2023
// ----          11Aug2023, 08Sep2023, 19Sep2023, 22Sep2023, 18Nov2023, 03Oct2024, 07Dec2024
// ----          04Jan2025, 09Jan2026, 10Apr2026, 09May2026, 15May2026, 17May2026, 18May2026
// ----          19May2026, 20May2026
// ----
// ----
// ----

// #undef SR_TRACK_FX
// #undef SR_VOICE_FX

// #define SONGNAME "sr_melodic_demo-18Apr2023c"
// #define SONGNAME "mb_rng_convolve_epiano-27Apr2022b"
#define SONGNAME "demo_2-r-sr"  // 18May2026

#define SR_MAX_TRACKS          48
#define SR_MAX_SAMPLES        128
#define SR_MAX_PLUGIN_DEFS     32
#define SR_MAX_TRACK_OUTPUTS    8
#define SR_MAX_TRACK_PLUGINS    4

#define SR_MIX_RATE              48000.0f
#define SR_VOLUME                0.94f
#define SR_MAX_FRAMES_PER_CALL   64u
#define SR_RENDER_TEST           defined    // when defined, add code to render song to mem buffer (+optionally write to file)

#ifdef SR_STDIO
static void loc_printf(const char *_fmt, ...);
#define Dprintf if(0) ; else loc_printf
#define Derror  if(0) ; else loc_printf
#define Dinfo   if(0) ; else loc_printf
#define Dsong   if(1) ; else loc_printf  // SMF/.mid loader debugging

// verbose .syn loader debugging:
#ifdef SR_TRACE
#define Dtrace       if(0) ; else loc_printf
#define Ddtor        if(0) ; else loc_printf
#else
#define Dtrace       if(1) ; else loc_printf
#define Ddtor        if(1) ; else loc_printf
#endif // SR_TRACE

// verbose SMF/.mid loader debugging:
#ifdef SR_SONGTRACE
#define Dsongtrace   if(0) ; else loc_printf
#else
#define Dsongtrace   if(1) ; else loc_printf
#endif // SR_SONGTRACE

// verbose replayer debugging:
#ifdef SR_REPLAYTRACE
#define Dreplay     if(0) ; else loc_printf
#define Dreplay2    if(1) ; else loc_printf
#else
#define Dreplay     if(1) ; else loc_printf
#define Dreplay2    if(1) ; else loc_printf
#endif // SR_REPLAYTRACE

#else
#define Dprintf     if(1) ; else loc_printf
#define Derror      if(1) ; else loc_printf
#define Dinfo       if(1) ; else loc_printf
#define Dtrace      if(1) ; else loc_printf
#define Dsongtrace  if(1) ; else loc_printf
#define Dsong       if(1) ; else loc_printf
#define Dreplay     if(1) ; else loc_printf
#endif // SR_STDIO

#include <stdio.h>
#include <string.h> // strcmp

#define YAC_PRINTF
#define YAC_EPSILONCOMPARE_ABS defined
#define YAC_EPSILONCOMPARE_ABS_DEFAULT defined
#include <yac.h>
#include <yac_host.cpp>

#include "../tksampler/tksampler.h"
#include "../tksampler/ying_tksampler_StADSR.cpp"
#include "../tksampler/ying_tksampler_StEnvelope.cpp"
#include "../tksampler/ying_tksampler_StLFO.cpp"
#include "../tksampler/ying_tksampler_StLFOPlayer.cpp"
#include "../tksampler/ying_tksampler_StModSeq.cpp"
#include "../tksampler/ying_tksampler_StRange.cpp"
#include "../tksampler/ying_tksampler_StWaveform.cpp"
#include "../tksampler/ying_tksampler_StSample.cpp"
#include "../tksampler/ying_tksampler_StSampleBank.cpp"
#include "../tksampler/ying_tksampler_StSampleVoice.cpp"
#include "../tksampler/ying_tksampler_StSampleMutexGroup.cpp"
#include "../tksampler/ying_tksampler_StSamplePlayer.cpp"
#include "../tksampler/ying_tksampler_StFFT.cpp"
#include "../tksampler/ying_tksampler_StFFT_BandParams.cpp"
#include "../tksampler/ying_tksampler_StPluginInfo.cpp"
#include "../tksampler/ying_tksampler_StPluginShared.cpp"
#include "../tksampler/ying_tksampler_StPluginSharedMissing.cpp"
#include "../tksampler/ying_tksampler_StPluginVoice.cpp"
#include "../tksampler/ying_tksampler_StPluginLibrary.cpp"
// #include "../tksampler/ying_tksampler.cpp"
// // sUI exid_InvalidPointer;

#ifdef SR_STDIO
void loc_printf(const char *_fmt, ...) {
   static char buf[8*1024];
   va_list va;
   va_start(va, _fmt);
#ifdef YAC_VC
   _vsnprintf((char*)buf, sizeof(buf), _fmt, va);
#elif defined(HAVE_VSNPRINTF)
   vsnprintf((char*)buf, sizeof(buf), _fmt, va);
#else
#warning "compiling without VSNPRINTF() support\n"
   vsprintf((char*)buf, _fmt, va);
#endif
   va_end(va);
   ::fputs(buf, stdout);
   ::fflush(stdout);
}
#endif // SR_STDIO

#ifdef SR_PORTAUDIO
#include <portaudio.h>
#include <unistd.h>
#endif // SR_PORTAUDIO

#ifdef SR_PROFILE
#include <sys/time.h>
static struct timeval sr_profile_tv_start;
static void loc_profile_ms_init(void) {
   ::gettimeofday(&sr_profile_tv_start, 0);
}
static sUI loc_profile_ms_get(void) {
   struct timeval c; ::gettimeofday(&c, 0);
   struct timeval *s = &sr_profile_tv_start;
   return (sUI)  ( (c.tv_sec-s->tv_sec)*1000 + (c.tv_usec-s->tv_usec)/1000 );
}
#endif // SR_PROFILE

static YAC_FloatArray loc_sr_freq_table;

#if defined(SR_TRACK_FX) || defined(SR_VOICE_FX)
#define SR_FX  defined
#endif

#ifdef SR_FX
class SR_PluginDef {
   // plugin descriptor
public:
   st_plugin_info_t *info;
#ifdef SR_VOICE_FX
   StPluginInfo info_oo;  // for sample zone per-voice plugins (C++ wrapper)
#endif // SR_VOICE_FX

public:
   SR_PluginDef(void) {
      info = NULL;
   }

   void unloadPlugin(void) {
      if(NULL != info)
      {
         Dtrace("[trc] unloadPlugin: info=%p\n", info);
         if(NULL != info->plugin_exit)
            info->plugin_exit(info);
         info = NULL;
      }
   }

   ~SR_PluginDef() {
      Ddtor("~SR_PluginDef()\n");
      unloadPlugin();
   }
};

class SR_PluginInstance {
public:
   st_plugin_info_t   *info;
   st_plugin_shared_t *shared;
   st_plugin_voice_t  *voice;

public:
   SR_PluginInstance(void) {
      info   = NULL;
      shared = NULL;
      voice  = NULL;
   }

   ~SR_PluginInstance() {
      Ddtor("~SR_PluginInstance()\n");
      if(NULL != info)
      {
         if(NULL != voice)
         {
            info->voice_delete(voice);
            voice = NULL;
         }

         if(NULL != shared)
         {
            info->shared_delete(shared);
            shared = NULL;
         }
      }
   }
};

static SR_PluginDef loc_plugin_defs[SR_MAX_PLUGIN_DEFS];
typedef st_plugin_info_t *(*ST_PLUGIN_API st_plugin_single_init_fxn_t) (void);

sBool sr_register_plugin(st_plugin_single_init_fxn_t _initFxn) {
   for(sUI defIdx = 0u; defIdx < SR_MAX_PLUGIN_DEFS; defIdx++)
   {
      SR_PluginDef *def = &loc_plugin_defs[defIdx];
      if(NULL == def->info)
      {
         def->info = _initFxn();
         // Dprintf("[dbg] sr_register_plugin: info=%p\n", def->info);
         if(NULL != def->info)
         {
            Dprintf("[dbg] sr_register_plugin: id=\"%s\" author=\"%s\" name=\"%s\"\n", def->info->id, def->info->author, def->info->name);
#ifdef SR_VOICE_FX
            def->info_oo.info = def->info;
#endif // SR_VOICE_FX
            return YAC_TRUE;
         }
      }
   }
   Derror("[--] sr_register_plugin: max plugin defs (%u) exceeded !!\n", SR_MAX_PLUGIN_DEFS);
   return YAC_FALSE;
}

SR_PluginDef *sr_find_plugin_def_by_id(const char *_id) {
   if(NULL != _id)
   {
      for(sUI defIdx = 0u; defIdx < SR_MAX_PLUGIN_DEFS; defIdx++)
      {
         SR_PluginDef *def = &loc_plugin_defs[defIdx];
         if(NULL != def->info)
         {
            if(!strcmp(def->info->id, _id))
               return def;
         }
      }
   }
   return NULL;
}

st_plugin_voice_t *sr_create_plugin_instance_by_def(SR_PluginDef *_def) {
   st_plugin_voice_t *voice = NULL;
   if(NULL != _def)
   {
      st_plugin_info_t *info = _def->info;
      st_plugin_shared_t *shared = info->shared_new(info);
      if(NULL != shared)
      {
         voice = info->voice_new(info, 0u/*voiceIdx*/);
         if(NULL != voice)
         {
            voice->shared = shared;
            if(NULL != info->set_sample_rate)
               info->set_sample_rate(voice, SR_MIX_RATE);
            if(NULL != info->note_on)
               info->note_on(voice, YAC_FALSE/*bGlide*/, 12u*5u/*middle C*/, 1.0f/*vel*/);
            if(NULL != info->prepare_block)
               info->prepare_block(voice,
                                   0u/*numFrames*/,
                                   261.63f/*freqHz*/,
                                   5.0f*12.0f/*freqNote=middle C*/,
                                   1.0f/*vol*/,
                                   0.0f/*pan*/
                                   );
         }
         else
         {
            // Failed to create voice instance
            info->shared_delete(shared);
         }
      }
      else
      {
         // Failed to create shared instance
      }
   }
   return voice;
}

st_plugin_voice_t *sr_create_plugin_instance_by_id(const char *_id) {
   SR_PluginDef *def = sr_find_plugin_def_by_id(_id);
   return sr_create_plugin_instance_by_def(def);
}

void sr_unload_plugins(void) {
   for(sUI defIdx = 0u; defIdx < SR_MAX_PLUGIN_DEFS; defIdx++)
   {
      SR_PluginDef *def = &loc_plugin_defs[defIdx];
      def->unloadPlugin();
   }
}

extern "C" st_plugin_info_t *amp_init (void);
extern "C" st_plugin_info_t *dly_2_init (void);
extern "C" st_plugin_info_t *dly_flt_2_mod_init (void);
extern "C" st_plugin_info_t *biquad_lpf_1_init (void);
extern "C" st_plugin_info_t *biquad_lpf_2_init (void);
extern "C" st_plugin_info_t *biquad_hpf_1_init (void);
extern "C" st_plugin_info_t *biquad_hpf_2_init (void);
extern "C" st_plugin_info_t *biquad_lsh_1_init (void);
extern "C" st_plugin_info_t *biquad_lsh_2_init (void);
extern "C" st_plugin_info_t *biquad_hsh_1_init (void);
extern "C" st_plugin_info_t *biquad_hsh_2_init (void);
extern "C" st_plugin_info_t *eq3_init (void);
extern "C" st_plugin_info_t *gain_init (void);
extern "C" st_plugin_info_t *resample_linear_init (void);
extern "C" st_plugin_info_t *ws_tanh_asym_init (void);
extern "C" st_plugin_info_t *ws_fm_init (void);
extern "C" st_plugin_info_t *schroederverb_init (void);
extern "C" st_plugin_info_t *valley_plateau_init (void);
extern "C" st_plugin_info_t *cycle_rms_comp_stereo_1_init (void);
extern "C" st_plugin_info_t *chorus_tri_1_init (void);
extern "C" st_plugin_info_t *gm_rng_convolve_init (void);
extern "C" {
// extern st_plugin_info_t *noiseplethora_ab_init (void);
// extern st_plugin_info_t *noiseplethora_cwhite_init (void);
// extern st_plugin_info_t *noiseplethora_cgrit_init (void);
extern st_plugin_info_t *np_svf_init (void);
}
extern "C" {
void fm_stack_init_common (void);
st_plugin_info_t *fm_stack_init_loresh (void);
st_plugin_info_t *fm_stack_init_medres (void);
st_plugin_info_t *fm_stack_init_medresh (void);
st_plugin_info_t *fm_stack_init_hires (void);
}

#endif // SR_FX

sUI yac_host_yacMilliSeconds(void) {
   return 0u;
}

sUI yac_host_yacGetDebugLevel(void) {
   return 0u;
}

static void loc_sr_init_freq_table(void) {
   loc_sr_freq_table.alloc(128u);
   for(sUI i = 0u; i < 128u; i++)
      loc_sr_freq_table.elements[i] = sF32( ((440.0f/32.0f)*exp( ((sF32(i)-9.0)/12.0)*log(2.0) )) );
}

void sr_init(void) {
   StSampleVoice::InitLanczosTables();
   StSampleVoice::InitAdditiveTables();
   StSampleVoice::InitMMCurveLUT();
   loc_sr_init_freq_table();
}


// <class.png>
class SR_BufferStreamLE : public YAC_Buffer {
   // (note) little endian input
public:
   SR_BufferStreamLE(void) {}
   ~SR_BufferStreamLE() {}

   sU8 u8(void) {
      if(io_offset < size)
         return buffer[((sUI)io_offset++)];
      return 0u;
   }

   sS8 s8(void) {
      if(io_offset < size)
         return (sS8)buffer[((sUI)io_offset++)];
      return 0;
   }

   sU16 u16(void) {
      if((io_offset + 1u) < size)
      {
         sU16 r = *(sU16*)&buffer[io_offset];
         io_offset += 2u;
         return r;
      }
      return 0u;
   }

   sS16 s16(void) {
      if((io_offset + 1u) < size)
      {
         sS16 r = *(sS16*)&buffer[io_offset];
         io_offset += 2u;
         return r;
      }
      return 0;
   }

   sU32 u32(void) {
      if((io_offset + 3u) < size)
      {
         sU32 r = *(sU32*)&buffer[io_offset];
         io_offset += 4u;
         return r;
      }
      return 0u;
   }

   sF32 f32(void) {
      if((io_offset + 3u) < size)
      {
         sF32 r = *(sF32*)&buffer[io_offset];
         io_offset += 4u;
         return r;
      }
      return 0.0f;
   }

   sBool readString(char *d, sUI dsz) {
      d[0] = 0;
      sU32 num = u32();
      sU32 readLen = sMIN(dsz, num);
      sUI i = 0u;
      for(; i < readLen; i++)
         d[i] = (char)u8();
      for(; i < num; i++)
         (void)u8();
      d[dsz - 1u] = 0u;
      return YAC_TRUE;
   }
};


// <class.png>
class SR_BufferStreamBE : public YAC_Buffer {
   // (note) big endian input
public:
   SR_BufferStreamBE(void) {}
   ~SR_BufferStreamBE() {}

   sU8 u8(void) {
      if(io_offset < size)
         return buffer[((sUI)io_offset++)];
      return 0u;
   }

   sS8 s8(void) {
      if(io_offset < size)
         return (sS8)buffer[((sUI)io_offset++)];
      return 0;
   }

   sU16 u16(void) {
      if((io_offset + 1u) < size)
      {
         sU16 r = buffer[io_offset + 1u];
         r |= (buffer[io_offset + 0u] << 8);
         io_offset += 2u;
         return r;
      }
      return 0u;
   }

   sS16 s16(void) {
      return (sS16)u16();
   }

   sU32 u32(void) {
      if((io_offset + 3u) < size)
      {
         sU32 r = buffer[io_offset + 3u];
         r |= (buffer[io_offset + 2u] << 8);
         r |= (buffer[io_offset + 1u] << 16);
         r |= (buffer[io_offset + 0u] << 24);
         io_offset += 4u;
         return r;
      }
      return 0u;
   }

   sF32 f32(void) {
      if((io_offset + 3u) < size)
      {
         sF32 r = *(sF32*)&buffer[io_offset];
         io_offset += 4u;
         return r;
      }
      return 0.0f;
   }

   sBool readString(char *d, sUI dsz) {
      d[0] = 0;
      sU32 num = u32();
      sU32 readLen = sMIN(dsz, num);
      sUI i = 0u;
      for(; i < readLen; i++)
         d[i] = (char)u8();
      for(; i < num; i++)
         (void)u8();
      d[dsz - 1u] = 0u;
      return YAC_TRUE;
   }

   void seekRel(sSI _inc) {
      io_offset = sUI(io_offset + _inc);
   }

   void seekAbs(sUI _offset) {
      io_offset = _offset;
   }
};


// <class.png>
class SR_BitStream4 {
   SR_BufferStreamLE *stream;
   sU8 bit_avail;
   sU8 bit_buffer;

public:
   SR_BitStream4(SR_BufferStreamLE *_stream) {
      stream = _stream;
      bit_avail  = 0u;
      bit_buffer = 0u;
   }

   ~SR_BitStream4() {}

   sU8 readu4(void) {
      sU8 r;
      if(bit_avail > 0u)
      {
         r = (bit_buffer >> 4) & 15u;
         bit_avail = 0u;
      }
      else
      {
         bit_buffer = stream->u8();
         r = bit_buffer & 15u;
         bit_avail = 4u;
      }
      return r;
   }

   sS8 read4(void) {
      sU8 u = readu4();
      if(u & 8u)
         u |= 0xF0u;
      return (sS8)u;
   }

   sS8 read8(void) {
      sS8 r = readu4();
      r |= readu4() << 4;
      return r;
   }

   sS16 read12(void) {
      sU16 r = readu4();
      r |= readu4() << 4;
      r |= readu4() << 8;
      if(r & (1u << 11))
         r |= 0xF000u;
      return (sS16)r;
   }

};


// <class.png>
class SR_Sample {

#define SR_PERFCTL_CC1_MODWHEEL  0
#define SR_PERFCTL_CC2_BREATHCTL 1
#define SR_PERFCTL_CC4_FOOTCTL   2
#define SR_PERFCTL_CC11_EXPR     3
#define SR_PERFCTL_CC16_GEN_1    4
#define SR_PERFCTL_CC17_GEN_2    5
#define SR_PERFCTL_CC18_GEN_3    6
#define SR_PERFCTL_CC19_GEN_4    7
#define SR_PERFCTL_NUM           8

#define SR_MODSRC_CC_NONE          0
#define SR_MODSRC_CC_MODWHEEL      1
#define SR_MODSRC_CC_BREATH        2
#define SR_MODSRC_CC_FOOT          3
#define SR_MODSRC_CC_EXPR          4
#define SR_MODSRC_CC_BRIGHTNESS    5
#define SR_MODSRC_CHPRESSURE       6
#define SR_MODSRC_POLYPRESSURE     7
#define SR_MODSRC_POLYPRESSURE_MAX 8
public:
   sBool b_procedural;
   StWaveform waveform;
   sS8 init_perf_ctl[SR_PERFCTL_NUM];
   sU8 prg_id;  // MIDI program number (0..127)
   sU16 unique_id;  // for RPN#90(smp_uid)
   sUI poly;   // 1..256
   StSampleBank sample_bank;
   sS8 transpose_oct;   // (todo) replace by temp var
   sS8 transpose_semi;  // (todo) replace by temp var
   sS8 transpose_cents; // (todo) replace by temp var
   sS8 pitchbend_up;
   sS8 pitchbend_down;
   sU8 mod_src;
   sBool b_mod_src_noteon;

public:
   SR_Sample(void) {
   }

   ~SR_Sample() {
      Ddtor("~SR_Sample()\n");
   }

   void init(void) {
      YAC_ValueObject vo;
      for(sUI i = 0u; i < 4u/*A..D*/; i++)
      {
         vo.initObject(YAC_NEW(StSampleMutexGroup), YAC_TRUE/*deleteme*/);
         sample_bank._addMutexGroup(&vo);
      }
   }

   void initZone(StSample *s) {
      // printf("xxx initZone: s=%p waveform=%p\n", s, &waveform);
      YAC_ValueObject vo;
      vo.initObject(&waveform, YAC_FALSE/*deleteme*/);
      s->_setWaveform(&vo);
      s->_setVolume(1.0f);

      StADSR *adsr;
      StEnvelope *env;
      // BezierEditState *beState;
      TKS_Envelope *beData;
      // StRange *range;
      // StLFO *lfo;

      // // Key range
      // range = (StRange*)s->_getOrCreateKeyRange();
      // range->_setLo(0.0f);
      // range->_setHi(127.0f);

      // // Velocity range
      // range = (StRange*)s->_getOrCreateVelRange();
      // range->_setLo(0.0f);
      // range->_setHi(1.01f);

      // // Mod range
      // range = (StRange*)s->_getOrCreateModRange();
      // range->_setLo(0.0f);
      // range->_setHi(1.01f);

      // ## Freq ADSR
      adsr = (StADSR*)s->_getOrCreateFreqADSR();
      adsr->_setIntensity(1.0f);
      adsr->_setLevel(12.0f);
      adsr->_setTimescaleMillisec(50000);
      adsr->_setStageOp(STADSR_STAGEOP_MUL_INTENSITY);
      adsr->_setEnableSustainLoop(YAC_TRUE);

      // Freq Attack
      env = (StEnvelope*)adsr->_getOrCreateEnvAttack();
      beData = (TKS_Envelope*)env->_getOrCreateData();
      env->_setIntensity(1.0f);
      env->_setTimescale(200.0f);

      // Freq Sustain
      env = (StEnvelope*)adsr->_getOrCreateEnvSustain();
      beData = (TKS_Envelope*)env->_getOrCreateData();
      env->_setIntensity(1.0f);
      env->_setTimescale(100.0f);

      // Freq Release
      env = (StEnvelope*)adsr->_getOrCreateEnvRelease();
      beData = (TKS_Envelope*)env->_getOrCreateData();
      env->_setIntensity(1.0f);
      env->_setTimescale(70.0f);

      // ## Volume ADSR
      adsr = (StADSR*)s->_getOrCreateVolADSR();
      adsr->_setIntensity(1.0f);
      adsr->_setLevel(1.0f);
      adsr->_setTimescaleMillisec(50000);
      adsr->_setStageOp(STADSR_STAGEOP_MUL);
      adsr->_setEnableSustainLoop(YAC_TRUE);

      // Vol Attack
      env = (StEnvelope*)adsr->_getOrCreateEnvAttack();
      beData = (TKS_Envelope*)env->_getOrCreateData();
      env->_setIntensity(1.0f);
      env->_setTimescale(200.0f);

      // Vol Sustain
      env = (StEnvelope*)adsr->_getOrCreateEnvSustain();
      beData = (TKS_Envelope*)env->_getOrCreateData();
      env->_setIntensity(1.0f);
      env->_setTimescale(100.0f);

      // Vol Release
      env = (StEnvelope*)adsr->_getOrCreateEnvRelease();
      beData = (TKS_Envelope*)env->_getOrCreateData();
      env->_setIntensity(1.0f);
      env->_setTimescale(70.0f);

      // ## Pan ADSR
      adsr = (StADSR*)s->_getOrCreatePanADSR();
      adsr->_setIntensity(1.0f);
      adsr->_setLevel(1.0f);
      adsr->_setTimescaleMillisec(50000);
      adsr->_setStageOp(STADSR_STAGEOP_ADD);
      adsr->_setEnableSustainLoop(YAC_TRUE);

      // Pan Attack
      env = (StEnvelope*)adsr->_getOrCreateEnvAttack();
      beData = (TKS_Envelope*)env->_getOrCreateData();
      env->_setIntensity(1.0f);
      env->_setTimescale(200.0f);

      // Pan Sustain
      env = (StEnvelope*)adsr->_getOrCreateEnvSustain();
      beData = (TKS_Envelope*)env->_getOrCreateData();
      env->_setIntensity(1.0f);
      env->_setTimescale(100.0f);

      // Pan Release
      env = (StEnvelope*)adsr->_getOrCreateEnvRelease();
      beData = (TKS_Envelope*)env->_getOrCreateData();
      env->_setIntensity(1.0f);
      env->_setTimescale(70.0f);

      // ## Aux ADSR
      adsr = (StADSR*)s->_getOrCreateAuxADSR();
      adsr->_setIntensity(1.0f);
      adsr->_setLevel(1.0f);
      adsr->_setTimescaleMillisec(50000);
      adsr->_setStageOp(STADSR_STAGEOP_MUL);
      adsr->_setEnableSustainLoop(YAC_TRUE);

      // Aux Attack
      env = (StEnvelope*)adsr->_getOrCreateEnvAttack();
      beData = (TKS_Envelope*)env->_getOrCreateData();
      env->_setIntensity(1.0f);
      env->_setTimescale(200.0f);

      // Aux Sustain
      env = (StEnvelope*)adsr->_getOrCreateEnvSustain();
      beData = (TKS_Envelope*)env->_getOrCreateData();
      env->_setIntensity(1.0f);
      env->_setTimescale(100.0f);

      // Aux Release
      env = (StEnvelope*)adsr->_getOrCreateEnvRelease();
      beData = (TKS_Envelope*)env->_getOrCreateData();
      env->_setIntensity(1.0f);
      env->_setTimescale(70.0f);

      (void)beData;
   }

   sBool loadRangeKey(SR_BufferStreamLE &ifs, StRange *range) {
      range->_setLo(ifs.s8());
      range->_setHi(ifs.s8());
      range->_setFadeIn(ifs.s8());
      range->_setFadeOut(ifs.s8());
      return YAC_TRUE;
   }

   sBool loadRangeVelMod(SR_BufferStreamLE &ifs, StRange *range) {
      range->_setLo(ifs.s8()/127.0f);
      range->_setHi(ifs.s8()/127.0f);
      range->_setFadeIn(ifs.s8()/127.0);
      range->_setFadeOut(ifs.s8()/127.0);
      range->_setEnableNoteOnFilter(ifs.s8());
      return YAC_TRUE;
   }

   sBool loadEnv(SR_BufferStreamLE &ifs, StEnvelope *env, sU8 ver) {
      if(ver >= 2u)
      {
         // static int xxxFirst = 1;

         // v1: pre-calced deltatime/value array
         //      (todo) remove and use bezier control points instead
         TKS_Envelope *beData = (TKS_Envelope*)env->_getData();
#if 0
         // (note) superceded by cubic spline data
         sUI numElements = ifs.u32();
         Dtrace("[...] SR_Sample::loadEnv: #elements=%u io_offset=%u\n", numElements, ifs.io_offset);
         sUI numEv = (numElements / 2u);
         sSI ct16 = 0;
         sSI cv16 = 0;
         beData->own_data     = (numElements > 0u);
         beData->max_elements = numElements;
         beData->num_elements = numElements;
         beData->elements     = (numElements > 0u) ? new(std::nothrow)sF32[numElements] : NULL;
         if(numEv > 0u && (NULL == beData->elements))
         {
            Derror("[---] SR_Sample::loadEnv: failed to allocate events (%u elements)\n", numElements);
            return YAC_FALSE;
         }
         sF32 *d = beData->elements;
         // sF32 tAbs = 0.0f;
         for(sUI i = 0u; i < numEv; i++)
         {
            // Delta time
            ct16 += ifs.s16();
            *d++ = ct16 / 32768.0f;

            // Value
            cv16 += ifs.s16();
            *d++ = cv16 / 8192.0f;

            // tAbs += d[-2];

            // if(xxxFirst)
            // {
            //    Dprintf("xxx bezier precalc ev[%u] dt=%f v=%f\n", i, d[-2], d[-1]);
            // }
         }

         // Dprintf("xxx bezier precalc tAbs=%f\n", tAbs);
#endif

         // v2: bezier control points
#define SR_CURVE_SZ 1024
         if(!beData->alloc(2 * SR_CURVE_SZ))
         {
            Derror("[---] SR_Sample::loadEnv: failed to allocate bezier curve data (%u bytes)\n", sUI(2u*SR_CURVE_SZ*sizeof(sF32)));
            return YAC_FALSE;
         }
         sF32 *d = beData->elements;
         for(sUI i = 0u; i < SR_CURVE_SZ; i++)
            d[i*2u+1u] = -9999.0f;
         const sUI numCtl = ifs.u8() + 1u;
         sF32 p1x;
         sF32 p1y;
         sF32 c1x;
         sF32 c1y;
         sF32 c2x;
         sF32 c2y;
         p1x = ifs.u16() / 65535.0f;  // p.x (0..1)
         p1y = ifs.s16() / 16383.0f;  // p.y (-2..+2)
         c1x = ifs.s16() / 16383.0f;  // c1.x (-2..+2)
         c1y = ifs.s16() / 16383.0f;  // c1.y (-2..+2)
         c2x = ifs.s16() / 16383.0f;  // c2.x (-2..+2)
         c2y = ifs.s16() / 16383.0f;  // c2.y (-2..+2)
         const sF32 dt = 1.0f / SR_CURVE_SZ;  // (todo) replace Envelope by simple FloatArray (winLinear())
         // Dprintf("xxx bezier numCtl=%u p1=(%f; %f) dt=%f\n", numCtl, p1x, p1y, dt);
         // tAbs = 0.0f;
         for(sUI ctlIdx = 1u; ctlIdx < numCtl; ctlIdx++)
         {
            sF32 p2x = ifs.u16() / 65535.0f;  // p.x (0..1)
            sF32 p2y = ifs.s16() / 16383.0f;  // p.y (-2..+2)

            // Calc cubic bezier curve segment
            // if(xxxFirst)
            // {
            //    Dprintf("p1=(%f; %f) c1=(%f; %f) c2=(%f; %f) p2=(%f; %f)\n", p1x, p1y, c1x, c1y, c2x, c2y, p2x, p2y);
            // }
            sSI startIdx = sSI(p1x * SR_CURVE_SZ);
            sSI endIdx = sSI(p2x * SR_CURVE_SZ);
            // if(xxxFirst)
            // {
            //    Dprintf("xxx bezier startIdx=%d endIdx=%d\n", startIdx, endIdx);
            // }

            sF32 x = 0.0f;
            const sF32 xStep = (endIdx > startIdx) ? (1.0f / (endIdx - startIdx)) : 0.0f;
            sSI idx = startIdx;
            while(idx < endIdx)
            {
               sF32 p1c1x = p1x   + (c1x   - p1x)   * x;  // l0 : p1..c1
               sF32 p1c1y = p1y   + (c1y   - p1y)   * x;  //
               sF32 c1c2x = c1x   + (c2x   - c1x)   * x;  // l1 : c1..c2
               sF32 c1c2y = c1y   + (c2y   - c1y)   * x;  //
               sF32 c2p2x = c2x   + (p2x   - c2x)   * x;  // l2 : c2..p2
               sF32 c2p2y = c2y   + (p2y   - c2y)   * x;  //
               sF32 q0x   = p1c1x + (c1c2x - p1c1x) * x;  // q0 : l0..l1
               sF32 q0y   = p1c1y + (c1c2y - p1c1y) * x;  //
               sF32 q1x   = c1c2x + (c2p2x - c1c2x) * x;  // q1 : l1..l2
               sF32 q1y   = c1c2y + (c2p2y - c1c2y) * x;  //
               sF32 cx    = q0x   + (q1x   - q0x)   * x;  //  p : q0..q1
               sF32 cy    = q0y   + (q1y   - q0y)   * x;  //
               sSI di = sSI(cx * SR_CURVE_SZ);
               di = sRANGE(di, 0, SR_CURVE_SZ-1);
               d[di*2+1] = cy;
               // if(xxxFirst)
               {
                  // Dprintf("xxx bezier data[%d] = %f    x=%f\n", idx, cy, x);
               }
               // tAbs += d[idx*2+0];
               // Next envelope event
               idx++;
               x += xStep;
            }

            // (todo) c1/c2 of last ctl point are unused. don't export ?
            c1x = ifs.s16() / 16383.0f;  // c1.x (-2..+2)
            c1y = ifs.s16() / 16383.0f;  // c1.y (-2..+2)
            c2x = ifs.s16() / 16383.0f;  // c2.x (-2..+2)
            c2y = ifs.s16() / 16383.0f;  // c2.y (-2..+2)

            // Next segment
            p1x = p2x;
            p1y = p2y;
         }

         // Fill in (potential) gaps
         sF32 l = 0.0f;
         for(sUI i = 0u; i < SR_CURVE_SZ; i++)
         {
            d[i*2+0] = dt;
            if(d[i*2+1] <= -9998.0f)
            {
               // Dprintf("xxx bezier fill gap idx=%u l=%f\n", i, l);
               d[i*2+1] = l;
            }
            else
               l = d[i*2+1];
         }
         d[0] = 0.0f;  // first deltatime

         // xxxFirst = 0;
         // Dprintf("xxx bezier       pLast=(%f; %f)\n", p1x, p1y);
         // Dprintf("xxx bezier tAbs=%f\n", tAbs);

         env->_setIntensity(ifs.u8()/255.0f);
         env->_setTimescaleMillisec(ifs.f32());
         env->_setExponent(ifs.f32());

         Dtrace("[...] SR_Sample::loadEnv: #ctl=%u intensity=%f timescaleMS=%f exponent=%f\n", numCtl, env->_getIntensity(), env->_getTimescaleMillisec(), env->_getExponent());

         return YAC_TRUE;
      }
      else
      {
         Derror("[---] SR_Sample::loadEnv: invalid ver=%u\n", ver);
      }
      return YAC_FALSE;
   }

   sBool loadADSR(SR_BufferStreamLE &ifs, StADSR *adsr) {
      StEnvelope *env;

      // Attack
      sU8 verEnv = ifs.u8();
      Dtrace("[dbg] SR_Sample::loadADSR: attack verEnv=%u\n", verEnv);
      if(verEnv > 0u)
      {
         env = (StEnvelope*)adsr->_getOrCreateEnvAttack();
         if(!loadEnv(ifs, env, verEnv))
         {
            Derror("[---] SR_Sample::loadADSR: failed to load attack envelope\n");
            return YAC_FALSE;
         }
      }

      // Sustain
      verEnv = ifs.u8();
      Dtrace("[dbg] SR_Sample::loadADSR: sustain verEnv=%u\n", verEnv);
      if(verEnv > 0u)
      {
         env = (StEnvelope*)adsr->_getOrCreateEnvSustain();
         if(!loadEnv(ifs, env, verEnv))
         {
            Derror("[---] SR_Sample::loadADSR: failed to load sustain envelope\n");
            return YAC_FALSE;
         }
      }

      // Release
      verEnv = ifs.u8();
      Dtrace("[dbg] SR_Sample::loadADSR: release verEnv=%u\n", verEnv);
      if(verEnv > 0u)
      {
         env = (StEnvelope*)adsr->_getOrCreateEnvRelease();
         if(!loadEnv(ifs, env, verEnv))
         {
            Derror("[---] SR_Sample::loadADSR: failed to load release envelope\n");
            return YAC_FALSE;
         }
      }

      adsr->_setIntensity(ifs.u8()/255.0);
      adsr->_setTimescaleMillisec(ifs.f32());
      adsr->_setLevel(ifs.f32());
      adsr->_setStageOp(ifs.u8());
      adsr->_setEnableSustainLoop(ifs.s8());
      if(verEnv >= 2u)
         adsr->_setEnableAttackOneshot(ifs.s8());

      Dtrace("[trc] SR_Sample::loadADSR: intensity=%f timescaleMillisec=%f level=%f stageOp=%u enableSustainLoop=%d enableAttackOneshot=%d\n",
             adsr->_getIntensity(),
             adsr->_getTimescaleMillisec(),
             adsr->_getLevel(),
             adsr->_getStageOp(),
             adsr->_getEnableSustainLoop(),
             adsr->_getEnableAttackOneshot()
             );

      return YAC_TRUE;
   }

   StSample *newZone(void) {
      StSample *s = YAC_NEW(StSample);
      initZone(s);
      YAC_ValueObject vo; vo.initObject(s, YAC_TRUE/*deleteme*/);
      sample_bank._addSample(&vo);
      return s;
   }

   sBool loadLFO(SR_BufferStreamLE &ifs, StLFO *lfo) {
      lfo->_setTimebase(ifs.f32());
      sU8 type = ifs.u8();
      lfo->_setType(type);
      lfo->_setPhase(ifs.u8()/255.0f);
      // // Dprintf("xxx lfo->_getPhase()=%f\n", lfo->_getPhase());
      lfo->_setFreq(ifs.f32());
      if(STLFO_TYPE_PULSE == type)
         lfo->_setPulseWidth(ifs.u8()/255.0f);
      lfo->_setDelay(ifs.f32());
      lfo->_setFadeInTime(ifs.f32());
      lfo->_setStartLevel(ifs.f32());
      lfo->_setFadedLevel(ifs.f32());
      lfo->_setFadedSpeedFactor(ifs.f32());
      lfo->_setDcOffset(ifs.f32());
      if(STLFO_TYPE_NOISE == type)
      {
         lfo->_setNoiseMinAmp(ifs.f32());
         lfo->_setNoiseMaxAmp(ifs.f32());
         lfo->_setNoiseMinSpeed(ifs.f32());
         lfo->_setNoiseMaxSpeed(ifs.f32());
      }
      sU8 flags = ifs.u8();
      lfo->_setEnableNoiseInterpol( (flags >> 0) & 1u );
      lfo->_setEnableModDelay     ( (flags >> 1) & 1u );
      lfo->_setEnableOneShot      ( (flags >> 2) & 1u );
      lfo->_setEnableGlobal       ( (flags >> 3) & 1u );
      lfo->_setEnableReset        ( (flags >> 4) & 1u );

      Dtrace("[...] SR_Sample::loadLFO: timebase=%f type=%u phase=%f freq=%f pw=%f delay=%f flags=%02x\n",
             lfo->_getTimebase(),
             lfo->_getType(),
             lfo->_getPhase(),
             lfo->_getFreq(),
             lfo->_getPulseWidth(),
             lfo->_getDelay(),
             flags
             );

      Dtrace("[...] SR_Sample::loadLFO: fadeInTime=%f startLevel=%f fadedLevel=%f fadedSpeedFactor=%f dcOffset=%f\n",
              lfo->_getFadeInTime(),
              lfo->_getStartLevel(),
              lfo->_getFadedLevel(),
              lfo->_getFadedSpeedFactor(),
              lfo->_getDcOffset()
              );

      Dtrace("[...] SR_Sample::loadLFO: noise minAmp=%f maxAmp=%f minSpd=%f maxSpd=%f interpol=%d\n",
              lfo->_getNoiseMinAmp(),
              lfo->_getNoiseMaxAmp(),
              lfo->_getNoiseMinSpeed(),
              lfo->_getNoiseMaxSpeed(),
              lfo->_getEnableNoiseInterpol()
              );

      Dtrace("[...] SR_Sample::loadLFO: modDelay=%d oneshot=%d global=%d reset=%d\n",
              lfo->_getEnableModDelay(),
              lfo->_getEnableOneShot(),
              lfo->_getEnableGlobal(),
              lfo->_getEnableReset()
              );

      return YAC_TRUE;
   }

   sBool loadMSeq(SR_BufferStreamLE &ifs, StModSeq *mseq) {
      mseq->_setPlayMode(ifs.u8());
      mseq->_setStepMode(ifs.u8());
      mseq->_setNumSteps(ifs.u8());  // play len
      mseq->_setRepeatOffset(ifs.u8());

      sUI numSteps = ifs.u8();

      Dtrace("[dbg] SR_Sample::loadMSeq: playMode=%u stepMode=%u len=%u/%u rep=%u\n",
              mseq->_getPlayMode(),
              mseq->_getStepMode(),
              mseq->_getNumSteps(),
              numSteps,
              mseq->_getRepeatOffset()
              );

      for(sUI stepIdx = 0u; stepIdx < numSteps; stepIdx++)
      {
         mseq->_setStepValue(stepIdx, ifs.f32());
      }
      for(sUI stepIdx = 0u; stepIdx < numSteps; stepIdx++)
      {
         mseq->_setStepDuration(stepIdx, ifs.u8());
      }

      mseq->_setSpeed(ifs.f32());
      mseq->_setValueScl(ifs.f32());
      mseq->_setValueOff(ifs.f32());
      mseq->_setSlewAmt(ifs.u8()/255.0);

      sU8 flags = ifs.u8();
      mseq->_setEnableTempoSync( (flags >> 0) & 1u );

      Dtrace("[dbg] SR_Sample::loadMSeq: speed=%f tempoSync=%d valueScl=%f valueOff=%f slew=%f\n",
              mseq->_getSpeed(),
              mseq->_getEnableTempoSync(),
              mseq->_getValueScl(),
              mseq->_getValueOff(),
              mseq->_getSlewAmt()
              );

      mseq->_setEnableOneShot( (flags >> 1) & 1u );
      mseq->_setEnableGlobal ( (flags >> 1) & 1u );
      sU8 t = (flags >> 3) & 3u;
      mseq->_setEnableReset( (t >= 2u) ? YAC_MAYBE/*-1*/ : t );
      t = (flags >> 5) & 3u;
      mseq->_setEnablePulse( (t >= 2u) ? YAC_MAYBE/*-1*/ : t );

      Dtrace("[dbg] SR_Sample::loadMSeq: oneshot=%d global=%d reset=%d pulse=%d\n",
              mseq->_getEnableOneShot(),
              mseq->_getEnableGlobal(),
              mseq->_getEnableReset(),
              mseq->_getEnablePulse()
              );

      return YAC_TRUE;
   }

#ifdef SR_VOICE_FX
   StPluginShared *newVoicePluginById(const char *_id) {
      StPluginShared *shared = NULL;
      SR_PluginDef *pluginDef = sr_find_plugin_def_by_id(_id);
      if(NULL != pluginDef)
      {
         YAC_Value r;
         pluginDef->info_oo.createSharedInstance(&r);
         if( (r.type >= YAC_TYPE_OBJECT) && (NULL != r.value.object_val) )
         {
            Dtrace("[...] SR_Sample::newVoicePluginById: added plugin id=\"%s\"\n", _id);
            r.deleteme = YAC_FALSE;  // unlink
            shared = (StPluginShared*)r.value.object_val;
         }
         else
         {
            Dprintf("[~~~] SR_Sample::newVoicePluginById: failed to instantiate plugin id=\"%s\"\n", _id);
         }
      }
      else
      {
         Dprintf("[~~~] SR_Sample::newVoicePluginById: unknown plugin id=\"%s\"\n", _id);
      }
      return shared;
   }
#endif // SR_VOICE_FX

   sBool loadZone(SR_BufferStreamLE &ifs, StSample *s) {
      const sUI startOff = ifs.io_offset;
      sU16 ver = ifs.u16();
      if(ver >= 1)
      {
         StADSR *adsr;
         // StEnvelope *env;
         // BezierEditState *beState;
         // TKS_Envelope *beData;
         StRange *range;
         StLFO *lfo;

         s->_setPlayMode(ifs.u8());
         s->_setSampleRateRatio(ifs.f32());

         // Zone name and mutex group
         char zoneName[1024];
         ifs.readString(zoneName, 1024);
         sS8 mtxGrpIdx = ifs.s8();
         StSampleMutexGroup *mtxGrp = (StSampleMutexGroup*)sample_bank._getMutexGroupByIdx(mtxGrpIdx);
         Dtrace("[dbg] SR_Sample::loadZone: ioOff=%u name=\"%s\" mtxGrp=%d (%p)\n", startOff, zoneName, mtxGrpIdx, mtxGrp);
         s->_setMutexGroup(mtxGrp);

         // Key range
         sU8 verRange = ifs.u8();
         range = (StRange*)s->_getOrCreateKeyRange();
         range->_setEnableHighInclusive(YAC_TRUE);
         if(verRange > 0)
         {
            if(!loadRangeKey(ifs, range))
            {
               Derror("[---] SR_Sample::loadZone: failed to load key range (verRange=%u)\n", verRange);
               return YAC_FALSE;
            }
         }
         else
         {
            // note 0..127, no fading
            range->_setHi(127.0f);
         }
         Dtrace("[...] SR_Sample::loadZone: key range lo=%f hi=%f fadeIn=%f fadeOut=%f noteOnFilter=%d highInclusive=%d\n", range->_getLo(), range->_getHi(), range->_getFadeIn(), range->_getFadeOut(), range->_getEnableNoteOnFilter(), range->_getEnableHighInclusive());

         // Velocity range
         verRange = ifs.u8();
         range = (StRange*)s->_getOrCreateVelRange();
         if(verRange > 0)
         {
            if(!loadRangeVelMod(ifs, range))
            {
               Derror("[---] SR_Sample::loadZone: failed to load velocity range (verRange=%u)\n", verRange);
               return YAC_FALSE;
            }
         }
         else
         {
            // 0..1, no fading, note filter=on
            range->_setHi(1.01f);
         }
         Dtrace("[...] SR_Sample::loadZone: vel range lo=%f hi=%f fadeIn=%f fadeOut=%f noteOnFilter=%d highInclusive=%d\n", range->_getLo(), range->_getHi(), range->_getFadeIn(), range->_getFadeOut(), range->_getEnableNoteOnFilter(), range->_getEnableHighInclusive());

         // Mod range
         verRange = ifs.u8();
         range = (StRange*)s->_getOrCreateModRange();
         if(verRange > 0)
         {
            if(!loadRangeVelMod(ifs, range))
            {
               Derror("[---] SR_Sample::loadZone: failed to load mod range (verRange=%u)\n", verRange);
               return YAC_FALSE;
            }
         }
         else
         {
            range->_setLo(0.0f);
            range->_setHi(1.01f);
            range->_setEnableHighInclusive(YAC_FALSE);
         }
         Dtrace("[...] SR_Sample::loadZone: mod range lo=%f hi=%f fadeIn=%f fadeOut=%f noteOnFilter=%d highInclusive=%d\n", range->_getLo(), range->_getHi(), range->_getFadeIn(), range->_getFadeOut(), range->_getEnableNoteOnFilter(), range->_getEnableHighInclusive());

         // Freq ADSR
         Dtrace("[dbg] SR_Sample::loadZone: freq adsr io_offset=%u\n", ifs.io_offset);
         sU8 verADSR = ifs.u8();
         Dtrace("[dbg] SR_Sample::loadZone: freq verADSR=%u\n", verADSR);
         adsr = (StADSR*)s->_getOrCreateFreqADSR();
         if(!loadADSR(ifs, adsr))
         {
            Derror("[---] SR_Sample::loadZone: failed to load freq ADSR\n");
            return YAC_FALSE;
         }

         // Vol ADSR
         verADSR = ifs.u8();
         Dtrace("[dbg] SR_Sample::loadZone: volume verADSR=%u\n", verADSR);
         adsr = (StADSR*)s->_getOrCreateVolADSR();
         if(!loadADSR(ifs, adsr))
         {
            Derror("[---] SR_Sample::loadZone: failed to load volume ADSR\n");
            return YAC_FALSE;
         }

         // Pan ADSR
         verADSR = ifs.u8();
         Dtrace("[dbg] SR_Sample::loadZone: pan verADSR=%u\n", verADSR);
         adsr = (StADSR*)s->_getOrCreatePanADSR();
         if(!loadADSR(ifs, adsr))
         {
            Derror("[---] SR_Sample::loadZone: failed to load pan ADSR\n");
            return YAC_FALSE;
         }

         // Aux ADSR
         verADSR = ifs.u8();
         Dtrace("[dbg] SR_Sample::loadZone: aux verADSR=%u\n", verADSR);
         adsr = (StADSR*)s->_getOrCreateAuxADSR();
         if(!loadADSR(ifs, adsr))
         {
            Derror("[---] SR_Sample::loadZone: failed to load pan ADSR\n");
            return YAC_FALSE;
         }

         // Freq LFO
         Dtrace("[dbg] SR_Sample::loadZone: load freq LFO io_offset=%u\n", ifs.io_offset);
         sU8 verLFO = ifs.u8();
         Dtrace("[dbg] SR_Sample::loadZone: freq verLFO=%u\n", verLFO);
         lfo = (StLFO*)s->_getOrCreateFreqLFO();
         if(verLFO > 0u)
         {
            if(!loadLFO(ifs, lfo))
            {
               Derror("[---] SR_Sample::loadZone: failed to load freq LFO\n");
               return YAC_FALSE;
            }
         }
         else
            lfo->_setType(STLFO_TYPE_NONE);


         // Volume LFO
         verLFO = ifs.u8();
         Dtrace("[dbg] SR_Sample::loadZone: volume verLFO=%u\n", verLFO);
         lfo = (StLFO*)s->_getOrCreateVolLFO();
         if(verLFO > 0u)
         {
            if(!loadLFO(ifs, lfo))
            {
               Derror("[---] SR_Sample::loadZone: failed to load volume LFO\n");
               return YAC_FALSE;
            }
         }
         else
            lfo->_setType(STLFO_TYPE_NONE);

         // Pan LFO
         verLFO = ifs.u8();
         Dtrace("[dbg] SR_Sample::loadZone: pan verLFO=%u\n", verLFO);
         lfo = (StLFO*)s->_getOrCreatePanLFO();
         if(verLFO > 0u)
         {
            if(!loadLFO(ifs, lfo))
            {
               Derror("[---] SR_Sample::loadZone: failed to load pan LFO\n");
               return YAC_FALSE;
            }
         }
         else
            lfo->_setType(STLFO_TYPE_NONE);

         // Aux LFO
         verLFO = ifs.u8();
         Dtrace("[dbg] SR_Sample::loadZone: aux verLFO=%u\n", verLFO);
         lfo = (StLFO*)s->_getOrCreateAuxLFO();
         if(verLFO > 0u)
         {
            if(!loadLFO(ifs, lfo))
            {
               Derror("[---] SR_Sample::loadZone: failed to load aux LFO\n");
               return YAC_FALSE;
            }
         }
         else
            lfo->_setType(STLFO_TYPE_NONE);

         // ModSeq
         for(sUI mseqIdx = 0u; mseqIdx < STSAMPLE_NUM_MODSEQ; mseqIdx++)
         {
            sUI numModSeqPatches = ifs.u8();
            for(sUI patchIdx = 0u; patchIdx < numModSeqPatches; patchIdx++)
            {
               sU8 verModSeq = ifs.u8();
               if(verModSeq > 0u)
               {
                  StModSeq *mseq = (StModSeq*)s->_getOrCreateModSeqByIndexAndPatch(mseqIdx, patchIdx);
                  if(!loadMSeq(ifs, mseq))
                  {
                     Derror("[---] SR_Sample::loadZone: failed to load mseqIdx=%u patchIdx=%u (verModSeq=%u)\n", mseqIdx, patchIdx, verModSeq);
                     return YAC_FALSE;
                  }
               }
            }

            s->_setDefaultModSeqPatch(mseqIdx, ifs.u8());
            s->_setEnableGlideRetrigModSeq(mseqIdx, ifs.s8());
         }

         // Sample Loops
         sUI numLoopElements = ifs.u32();
         if(numLoopElements > 0u)
         {
            YAC_IntArray *loops = (YAC_IntArray*)s->_getOrCreateSampleLoops();
            if(loops->alloc(numLoopElements))
            {
               for(sUI i = 0u; i < numLoopElements; i++)
                  loops->elements[i] = (sSI)ifs.u32();
            }
            else
            {
               Derror("[---] SR_Sample::loadZone: failed to allocate loops (%u elements)\n", numLoopElements);
               return YAC_FALSE;
            }
         }

         // Loop Fade #frames (v74+)
         s->_setStartEndFadeNumFrames(ifs.u16());

         // ModMatrix
         s->_setMMKeyboardCenter(ifs.u8() * 0.5f);  // v76+
         s->_setMMKeyboardMin   (ifs.u8() * 0.5f);  // v77+
         s->_setMMKeyboardMax   (ifs.u8() * 0.5f);  // v77+

         sU16 mmMask = ifs.u16();
         if(0u != mmMask)
         {
            Dtrace("[...] SR_Sample::loadZone: mmMask=%04x\n", mmMask);
            sUI mmNumUsed = 0u;
            for(sUI mmIdx = 0u; mmIdx < STSAMPLE_NUM_MODMATRIX_ENTRIES/*16*/; mmIdx++)
            {
               if(mmMask & (1u << mmIdx))
               {
                  // // sU16 verMM = ifs.u16();
                  s->_setMMEnable(mmIdx, YAC_TRUE);
                  s->_setMMOp(mmIdx, ifs.u8());
                  s->_setMMSlewUp(mmIdx, ifs.u8() * (1.0f / 255.0f));
                  s->_setMMSlewDown(mmIdx, ifs.u8() * (1.0f / 255.0f));
                  s->_setMMSlewReset(mmIdx, ifs.u8());
                  s->_setMMSrc(mmIdx, ifs.u8());
                  s->_setMMCurve(mmIdx, ifs.f32());
                  s->_setMMAmt(mmIdx, ifs.f32());
                  s->_setMMDst(mmIdx, ifs.u8());
                  Dtrace("[trc] SR_Sample::loadZone: mm[%u] op=%u slewUp=%f slewDown=%f slewReset=%U src=%u curve=%f amt=%f dst=%u\n", mmIdx, s->_getMMOp(mmIdx), s->_getMMSlewUp(mmIdx), s->_getMMSlewDown(mmIdx), s->_getMMSlewReset(mmIdx), s->_getMMSrc(mmIdx), s->_getMMCurve(mmIdx), s->_getMMAmt(mmIdx), s->_getMMDst(mmIdx));
                  mmNumUsed++;
               }
               else
               {
                  s->_setMMEnable(mmIdx, YAC_FALSE);
               }
            }

            sU8 mmVarNumAndFlags = ifs.u8();
            if(0u != mmVarNumAndFlags)
            {
               s->_mmVarSetEnable(YAC_TRUE);
               s->_mmVarSetEnableSmooth( (0u != (mmVarNumAndFlags & (1u << 7))) );
               mmVarNumAndFlags &= 31u;
               sF32 *mmVarData = s->mmvar_data;
               for(sUI mmVarIdx = 0u; mmVarIdx < mmVarNumAndFlags; mmVarIdx++)
               {
                  for(sUI mmIdx = 0u; mmIdx < mmNumUsed; mmIdx++)
                  {
                     mmVarData[mmVarIdx * STSAMPLE_NUM_MODMATRIX_ENTRIES + mmIdx] = ifs.f32();
                  }
               }
            }
         }

         s->_setFirstLoopIndex(ifs.u8());
         s->_setNoteOffLoopIndex(ifs.u8());
         s->_setEnableNoteOffLoopImmediately(ifs.s8());
         s->_setEnableJumpToLoopImmediately(ifs.s8());
         Dtrace("[dbg] SR_Sample::loadZone: loop first=%u noteOff=%u offImm=%d jumpImm=%d\n",
                 s->_getFirstLoopIndex(),
                 s->_getNoteOffLoopIndex(),
                 s->_getEnableNoteOffLoopImmediately(),
                 s->_getEnableJumpToLoopImmediately()
                 );

         // // s->_setOffset(ifs.u32());
         // // s->_setLen(ifs.u32());
         // // Dtrace("[dbg] SR_Sample::loadZone: offset=%u len=%u\n",
         // //         s->_getOffset(),
         // //         s->_getLen()
         // //         );

         s->_setMaxVoices(ifs.u8() + 1u);
         s->_setVolume(ifs.f32());
         s->_setVolLFOAmt(ifs.f32());
         s->_setVolumeVelocityAmount(ifs.f32());
         Dtrace("[dbg] SR_Sample::loadZone: poly=%u vol=%f volLFOAmt=%f volVelAmt=%f\n",
                 s->_getMaxVoices(),
                 s->_getVolume(),
                 s->_getVolLFOAmt(),
                 s->_getVolumeVelocityAmount()
                 );
         s->_setSampleOffsetVelocityAmount(ifs.f32());
         s->_setEnableInvertSampleOffsetVelocityAmount(ifs.s8());
         s->_setSampleOffsetRandAmount(ifs.f32());
         s->_setSampleOffsetRel(ifs.f32());
         s->_setEnableSampleOffsetZC(ifs.s8());  // v83+

         Dtrace("[dbg] SR_Sample::loadZone: offVelAmt=%f invOffVel=%d offRandAmt=%f offRel=%f offZC=%d\n",
                s->_getSampleOffsetVelocityAmount(),
                s->_getEnableInvertSampleOffsetVelocityAmount(),
                s->_getSampleOffsetRandAmount(),
                s->_getSampleOffsetRel(),
                s->_getEnableSampleOffsetZC()
                );

         s->_setPan(ifs.f32());
         s->_setPanEnvAmt(ifs.f32());
         s->_setPanLFOAmt(ifs.f32());
         Dtrace("[dbg] SR_Sample::loadZone: pan=%f panEnvAmt=%f panLFOAmt=%f\n",
                 s->_getPan(),
                 s->_getPanEnvAmt(),
                 s->_getPanLFOAmt()
                 );

         s->_setUiTransposeOct(ifs.s8());
         s->_setUiTransposeSemi(ifs.s8());
         s->_setUiTransposeCents(ifs.f32());
         s->_setBeatFrequency(ifs.f32());
         Dtrace("[dbg] SR_Sample::loadZone: transpose oct=%d semi=%d cents=%f beat=%f\n",
                 s->_getUiTransposeOct(),
                 s->_getUiTransposeSemi(),
                 s->_getUiTransposeCents(),
                 s->_getBeatFrequency()
                 );

         s->_setFreqEnvAmt(ifs.f32());
         s->_setFreqLFOAmt(ifs.f32());
         s->_setDelay(ifs.f32());
         s->_setDelayMultiplier(ifs.f32());
         Dtrace("[dbg] SR_Sample::loadZone: freqEnvAmt=%f freqLFOAmt=%f delay=%f delayMul=%f\n",
                 s->_getFreqEnvAmt(),
                 s->_getFreqLFOAmt(),
                 s->_getDelay(),
                 s->_getDelayMultiplier()
                 );

         s->setEnableTimestretch(ifs.s8());
         Dtrace("[dbg] SR_Sample::loadZone: enableWT=%d\n", s->getEnableTimestretch());
         if(s->getEnableTimestretch())
         {
            s->setEnableTimestretchXFade(ifs.s8());
            s->setTimestretch(ifs.f32());
            s->setTimestretchGranularity(ifs.f32());
            s->setTimestretchGranularityAmount(ifs.f32());
            s->setEnableStaticCyclelen(ifs.s8());
            s->setEnableAbsoluteTimestretch(ifs.s8());
            s->setTimestretchInterpolType(ifs.s8());
            s->setTimestretchGrainWindowType(ifs.s8());
            s->setTimestretchSmpOffInterpolMode(ifs.u8());  // v86+
            s->setTimestretchBend(ifs.f32());
            s->setTimestretchStartPhaseRandAmount(ifs.f32());
            s->setTimestretch2DWidth(ifs.u8());
            s->setTimestretch2DHeight(ifs.u8());
         }

         s->_setEnableTimedLoop(ifs.u8());
         s->_setEnableTimedLoopFade(ifs.u8());
         s->_setTimedLoopBase(ifs.f32());
         s->_setEnableOneShotLoopStep(ifs.s8());
         Dtrace("[dbg] SR_Sample::loadZone: timedLoop enable=%d fade=%d base=%f oneshot=%d\n",
                 s->_getEnableTimedLoop(),
                 s->_getEnableTimedLoopFade(),
                 s->_getTimedLoopBase(),
                 s->_getEnableOneShotLoopStep()
                 );

         s->_setVolumeRampStepsMillisecIn(ifs.f32());
         s->_setVolumeRampStepsMillisecOut(ifs.f32());
         s->_setEnableFreezeSampleOffWhenQuiet(ifs.s8());
         s->_setEnableModShiftOrig(ifs.s8());
         s->_setEnableSampleOffNoteOn(ifs.s8());
         Dtrace("[dbg] SR_Sample::loadZone: volRampIn=%f volRampOut=%f freeze=%d shiftOrig=%d offNoteOn=%d\n",
                 s->_getVolumeRampStepsMillisecIn(),
                 s->_getVolumeRampStepsMillisecOut(),
                 s->_getEnableFreezeSampleOffWhenQuiet(),
                 s->_getEnableModShiftOrig(),
                 s->_getEnableSampleOffNoteOn()
                 );

         s->_setEnabled(ifs.s8());

         s->_setInterpolType(ifs.s8());
         s->_setInterpolOrder(ifs.s8());

         Dtrace("[dbg] SR_Sample::loadZone: enable=%d interpolType=%d interpolOrder=%d\n",
                 s->_getEnabled(),
                 s->_getInterpolType(),
                 s->_getInterpolOrder()
                 );

         s->_setAiNumPoles(ifs.s8());
         s->_setAiExpUp(ifs.f32());
         s->_setAiExpDown(ifs.f32());
         s->_setAiQ(ifs.f32());
         s->_setAiLinOct(ifs.f32());
         s->_setAiLinMax(ifs.f32());
         Dtrace("[dbg] SR_Sample::loadZone: ai #poles=%d expUp=%f expDown=%f q=%f linOct=%f linMax=%f\n",
                 s->_getAiNumPoles(),
                 s->_getAiExpUp(),
                 s->_getAiExpDown(),
                 s->_getAiQ(),
                 s->_getAiLinOct(),
                 s->_getAiLinMax()
                 );

         s->_setBitReductionPreAmp(ifs.f32());
         s->_setBitReduction(ifs.u8());
         s->_setBitReductionMask(ifs.u16());
         Dtrace("[dbg] SR_Sample::loadZone: br preAmp=%f br=%u mask=%u\n",
                 s->_getBitReductionPreAmp(),
                 s->_getBitReduction(),
                 s->_getBitReductionMask()
                 );

         s->_setRateReduction(ifs.u8());
         s->_setRateReductionMask(ifs.u8());
         Dtrace("[dbg] SR_Sample::loadZone: srr=%u mask=%u\n",
                 s->_getRateReduction(),
                 s->_getRateReductionMask()
                 );

         s->_setEnableLerpFreq(ifs.s8());
         s->_setEnableLerpVol(ifs.s8());
         Dtrace("[dbg] SR_Sample::loadZone: lerp freq=%d vol=%d\n",
                 s->_getEnableLerpFreq(),
                 s->_getEnableLerpVol()
                 );

         s->_setEnableFromStart(ifs.s8());
         s->_setEnableAlt(ifs.s8());
         Dtrace("[dbg] SR_Sample::loadZone: fromStart=%d alt=%d\n",
                 s->_getEnableFromStart(),
                 s->_getEnableAlt()
                 );

         sU8 filterType = ifs.u8();
         if(filterType > 0u)
         {
            s->_setEnableFilter(YAC_TRUE);
            s->_setFilterType(filterType);
            s->_setFilterCutOff(ifs.s8()/127.0f);
            s->_setFilterPan(ifs.s8()/127.0f);
            s->_setFilterOffset(ifs.s8()/127.0f);
            s->_setFilterEQGain(ifs.f32());
            s->_setFilterResonance(ifs.u8()/255.0f);
            Dtrace("[dbg] SR_Sample::loadZone: filter enable=%d type=%d coff=%f pan=%f off=%f eqGain=%f res=%f\n",
                   s->_getEnableFilter(),
                   s->_getFilterType(),
                   s->_getFilterCutOff(),
                   s->_getFilterPan(),
                   s->_getFilterOffset(),
                   s->_getFilterEQGain(),
                   s->_getFilterResonance()
                   );
            s->_setFilterAuxEnvAmount(ifs.s8()/127.0f);
            s->_setFilterAuxEnvVelocityAmount(ifs.s8()/63.0f);
            s->_setFilterAuxLFOAmount(ifs.s8()/127.0f);
            s->_setFilterKeyboardAmount(ifs.s8()/127.0f);
            Dtrace("[dbg] SR_Sample::loadZone: filter envAmt=%f envVelAmt=%f lfoAmt=%f kbdAmt=%f\n",
                   s->_getFilterAuxEnvAmount(),
                   s->_getFilterAuxEnvVelocityAmount(),
                   s->_getFilterAuxLFOAmount(),
                   s->_getFilterKeyboardAmount()
                   );
         }
         else
         {
            s->_setEnableFilter(YAC_FALSE);
         }

         s->_setGlideType(ifs.s8());
         s->_setGlideSpeedTimeUp(ifs.f32());
         s->_setGlideSpeedTimeDown(ifs.f32());
         s->_setGlideSpeedFreqUp(ifs.f32());
         s->_setGlideSpeedFreqDown(ifs.f32());
         s->_setEnableGlideRelease(ifs.s8());
         s->_setGlideMaxNoteDist(ifs.s8());
         s->_setEnableGlideGlissando(ifs.s8());
         s->_setGlideMinTime(ifs.u16());
         s->_setGlideMaxTime(ifs.u16());
         sU16 glideFXFlags = ifs.u16();
         s->_setEnableGlideRetrigSample  ( (glideFXFlags >>  0) & 1u );
         s->_setEnableGlideRetrigEnvFreq ( (glideFXFlags >>  1) & 1u );
         s->_setEnableGlideRetrigEnvVol  ( (glideFXFlags >>  2) & 1u );
         s->_setEnableGlideRetrigEnvPan  ( (glideFXFlags >>  3) & 1u );
         s->_setEnableGlideRetrigEnvAux  ( (glideFXFlags >>  4) & 1u );
         s->_setEnableGlideRetrigLFOFreq ( (glideFXFlags >>  5) & 1u );
         s->_setEnableGlideRetrigLFOVol  ( (glideFXFlags >>  6) & 1u );
         s->_setEnableGlideRetrigLFOPan  ( (glideFXFlags >>  7) & 1u );
         s->_setEnableGlideRetrigLFOAux  ( (glideFXFlags >>  8) & 1u );
         s->_setEnableFX                 ( (glideFXFlags >>  9) & 1u );
         s->_setEnableFreeRunningOsc     ( (glideFXFlags >> 10) & 1u );
         s->_setVoiceBus(ifs.s8());
         Dtrace("[dbg] SR_Sample::loadZone: fx=%d voiceBus=%d\n",
                 s->_getEnableFX(),
                 s->_getVoiceBus()
                 );

         // Voice Calibration tables
         for(sUI vcalLaneIdx = 0u; vcalLaneIdx < 6u; vcalLaneIdx++)
         {
            s->setVoiceCalibrationModulo(vcalLaneIdx, ifs.u8());
            s->setVoiceCalibrationAmount(vcalLaneIdx, ifs.f32());
            s->setVoiceCalibrationEnable(vcalLaneIdx, ifs.u8());
            for(sUI vcalVoiceIdx = 0u; vcalVoiceIdx < 8u; vcalVoiceIdx++)
            {
               s->setVoiceCalibrationValue(vcalLaneIdx, vcalVoiceIdx, ifs.f32());
            }
         }

         // sUI xxxOffPlugin = ifs.io_offset;

         // Voice plugins
         for(sUI pluginIdx = 0u; pluginIdx < STSAMPLE_NUM_PLUGINS/*4*/; pluginIdx++)
         {
            sU8 verAndEnable = ifs.u8();
            if(verAndEnable > 0u)
            {
               char pluginId[128];
               (void)ifs.readString(pluginId, 128);
               // Dprintf("[trc] SR_Sample::loadZone: read plugin id=\"%s\"\n", pluginId);

#ifdef SR_VOICE_FX
               StPluginShared *shared = newVoicePluginById(pluginId);
               if(NULL != shared)
#endif // SR_VOICE_FX
               {
                  sU16 numParams = ifs.u16();

                  // Param values
                  for(sUI paramIdx = 0u; paramIdx < numParams; paramIdx++)
                  {
#ifdef SR_VOICE_FX
                     shared->setParamValue(paramIdx, ifs.f32());
#else
                     (void)ifs.f32();
#endif // SR_VOICE_FX
                  }

                  // Install plugin
#ifdef SR_VOICE_FX
                  YAC_ValueObject vo;
                  vo.initObject(shared, YAC_TRUE/*deleteme*/);
                  s->_setVoicePluginShared(pluginIdx, &vo);

                  // Enable plugin
                  s->_setEnableVoicePlugin(pluginIdx, YAC_TRUE);
#endif // SR_VOICE_FX

                  // Pre-Filter flag
                  s->_setEnableVoicePluginPreFilter(pluginIdx, ifs.s8());

                  // Level (-f..+f) + phase invert (baked in)
                  s->_setVoicePluginLevel(pluginIdx, ifs.f32());

                  // Pan (-1..1)
                  s->_setVoicePluginPan(pluginIdx, ifs.s8() / 127.0f);

                  Dprintf("[...] SR_Sample::loadZone: plugin[%u] id=\"%s\" #params=%u preFilter=%d level=%f pan=%f\n",
                          pluginIdx,
                          pluginId,
                          numParams,
                          s->_getEnableVoicePluginPreFilter(pluginIdx),
                          s->_getVoicePluginLevel(pluginIdx),
                          s->_getVoicePluginPan(pluginIdx)
                          );
               }
#ifdef SR_VOICE_FX
               else
               {
                  Derror("[---] SR_Sample::loadZone: failed to create shared plugin instance (plugin id=\"%s\")\n", pluginId);
                  return YAC_FALSE;
               }
#endif // SR_VOICE_FX
            }
         }

         // xxxOffPlugin = ifs.io_offset - xxxOffPlugin;
         // Dprintf("xxx zone plugin sz=%u\n", xxxOffPlugin);


         const sUI zoneSz = ifs.io_offset - startOff;
         Dtrace("[dbg] SR_Sample::loadZone: zoneSz=%u\n", zoneSz);

         return YAC_TRUE;
      }
      else
      {
         Derror("[---] SR_Sample::loadZone: invalid ver=%u\n", ver);
      }
      return YAC_FALSE;
   }

   sBool load(SR_BufferStreamLE &ifs) {
      // see Sample::exportSYNSmpInfo()
      sU16 ver = ifs.u16();
      if(ver >= 1)
      {
         b_procedural = ifs.s8();

         sUI  wfNumFrames  = ifs.u32();
         sUI  wfNumCh      = ifs.u8();
         sF32 wfSampleRate = ifs.f32();

         YAC_FloatArray *smpDat = YAC_NEW(YAC_FloatArray);
         smpDat->own_data = YAC_FALSE;
         smpDat->num_elements = wfNumFrames * wfNumCh;
         smpDat->max_elements = smpDat->num_elements;

         YAC_ValueObject vo;
         vo.initObject(smpDat, YAC_TRUE/*deleteme*/);
         waveform._setSampleData(&vo, wfNumCh);
         waveform._setSampleRate(wfSampleRate);
         waveform._setBaseFrequency(261.63f/*c-5 = BASE_FREQ_MIDDLE_C*/);

         char smpName[1024];
         ifs.readString(smpName, 1024);

         Dprintf("[...] SR_Sample::load: smpName=\"%s\" proc=%d #frames=%u #ch=%u rate=%5.2f\n", smpName, b_procedural, wfNumFrames, wfNumCh, wfSampleRate);

         sU8 perfMask = ifs.u8();
         for(sUI i = 0u; i < 8u; i++)
         {
            if(perfMask & (1u << i))
               init_perf_ctl[i] = ifs.s8();
            else
               init_perf_ctl[i] = -1;
         }
         Dtrace("SR_Sample::load: perfMask=%02x => [mw=%d; bc=%d; fc=%d; ex=%d; gen1=%d; gen2=%d; gen3=%d; gen4=%d]\n",
                 perfMask,
                 init_perf_ctl[0], init_perf_ctl[1], init_perf_ctl[2], init_perf_ctl[3],
                 init_perf_ctl[4], init_perf_ctl[5], init_perf_ctl[6], init_perf_ctl[7]
                 );

         // see Sample::saveStateInt()
         prg_id = ifs.u8();
         unique_id = ifs.u16();  // v9+
         poly = ifs.u8() + 1u;
         sample_bank._setEnableRealloc(ifs.s8());
         sample_bank._setEnableUniqueNotes(ifs.s8());

         sample_bank._setEnablePerfCtlFreezeNoteOff(ifs.s8());

         Dtrace("SR_Sample::load: prg_id=%u poly=%u unique_notes=%d perfCtlFreezeNoteOff=%d\n", prg_id, poly, sample_bank._getEnableUniqueNotes(), sample_bank._getEnablePerfCtlFreezeNoteOff());

         transpose_oct   = ifs.s8();
         transpose_semi  = ifs.s8();
         transpose_cents = ifs.s8();
         Dtrace("SR_Sample::load: transpose oct=%d semi=%d cents=%d\n", transpose_oct, transpose_semi, transpose_cents);

         pitchbend_up   = ifs.s8();
         pitchbend_down = ifs.s8();
         Dtrace("SR_Sample::load: pitchbend up=%d down=%d\n", pitchbend_up, pitchbend_down);
         sample_bank._setTranspose(transpose_oct * 12 + transpose_semi + transpose_cents/100.0f);

         sample_bank._setVolume(ifs.f32());
         sample_bank._setPan(ifs.f32());
         Dtrace("SR_Sample::load: vol=%f pan=%f\n", sample_bank._getVolume(), sample_bank._getPan());

         sample_bank._setSampleOffsetRandAmount(ifs.f32());
         sample_bank._setVolumeRandAmount(ifs.f32());
         sample_bank._setPanRandAmount(ifs.f32());
         sample_bank._setFreqRandAmount(ifs.f32());
         Dtrace("SR_Sample::load: rand off=%f vol=%f pan=%f freq=%f\n",
                 sample_bank._getSampleOffsetRandAmount(),
                 sample_bank._getVolumeRandAmount(),
                 sample_bank._getPanRandAmount(),
                 sample_bank._getFreqRandAmount()
                 );

         mod_src = ifs.u8();
         b_mod_src_noteon = ifs.s8();
         Dtrace("SR_Sample::load: mod_src=%u noteon=%d\n", mod_src, b_mod_src_noteon);

         const sUI numZones = ifs.u8();
         for(sUI zoneIdx = 0u; zoneIdx < numZones; zoneIdx++)
         {
            Dtrace("SR_Sample::load: zone %2u/%2u ..................................\n", zoneIdx+1u, numZones);
            StSample *s = newZone();
            if(!loadZone(ifs, s))
            {
               Derror("[---] SR_Sample::load: failed to load zone %u/%u\n", zoneIdx+1u, numZones);
               return YAC_FALSE;
            }
         }

         return YAC_TRUE;
      }
      else
      {
         Derror("[---] SR_Sample::load: invalid ver=%u\n", ver);
      }
      return YAC_FALSE;
   }
};

static const sU8 loc_sr_perfctl_map[SR_PERFCTL_NUM] = {
   STSAMPLEPLAYER_PERFCTL_CC1_MODWHEEL,
   STSAMPLEPLAYER_PERFCTL_CC2_BREATHCTL,
   STSAMPLEPLAYER_PERFCTL_CC4_FOOTCTL,
   STSAMPLEPLAYER_PERFCTL_CC11_EXPRESSION,
   STSAMPLEPLAYER_PERFCTL_CC16_GENERAL_1,
   STSAMPLEPLAYER_PERFCTL_CC17_GENERAL_2,
   STSAMPLEPLAYER_PERFCTL_CC18_GENERAL_3,
   STSAMPLEPLAYER_PERFCTL_CC19_GENERAL_4
};


// <class.png>
class SR_Track {
public:
   sU8 midi_port;  // 0..7
   sS8 midi_ch;    // 0..15, -1=*
   sS8 def_prg;    // 0..127 (MIDI program number), -1=not set
   sS8 cur_prg;
   sF32 track_vol;
   sF32 track_pan;
   sBool b_fx_auto_note_on;  // (todo)
   sBool b_receive_patch_changes;
   sU8 note_min;
   sU8 note_max;
   sS8 note_trp;   // transpose (after note range filter)
   sU8 vel_min;
   sU8 vel_max;
   StSamplePlayer sample_player;
#ifdef SR_TRACK_FX
   YAC_FloatArray mix_buffer;  // view into SR_Project.track_mix_buffers
   struct {
      sUI  out_dest;  // 0=default, or track 1..n
      sSI  orig_track_output_idx;  // or -1. for RPN#300.307 modulation.
      sF32 level_track;  // may be modulated by RPN#300.307 (0..1)
      sF32 level_lane;  // 0..1
      sF32 level_l;  // effective level (0..1)
      sF32 level_r;
      sF32 pan;      // -1..1
   } outputs[SR_MAX_TRACK_OUTPUTS];
   sUI num_outputs;
   struct {
      st_plugin_info_t   *info;
      st_plugin_shared_t *shared;
      st_plugin_voice_t  *voice;
   } plugins[SR_MAX_TRACK_PLUGINS];
   sUI num_plugins;
#endif // SR_TRACK_FX
   sU8 cc_modwheel;
   sU8 cc_breath;
   sU8 cc_foot;
   sU8 cc_expr;
   sU8 cc_brightness;
   sU8 aftertouch;
   sF32 cc_vol_norm;
   sF32 cc_pan_norm;
   sF32 pitchbend_norm;
   sF32 voicepressure[128];
   sF32 voicepressure_max[128];
public:

   void resetAllControllers(void) {
      Dreplay2("[trc] SR_Track::resetAllControllers\n");
      sample_player.resetModulators();
      sample_player.updateSampleOff(0.0f);
      sample_player.updateFreq(0.0f);
      sample_player.updateVol(track_vol);
      sample_player.updatePan(track_pan);

      cc_modwheel    = 0u;
      cc_breath      = 0u;
      cc_foot        = 0u;
      cc_vol_norm    = 1.0f;
      cc_pan_norm    = 0.0f;
      cc_expr        = 0u;
      cc_brightness  = 0u;
      aftertouch     = 0u;
      // pitchbend      = 8192;
      pitchbend_norm = 0.0f;

      // (todo)
      // b_sustain = YAC_FALSE;
      // sustain_noteoff_queue.empty();

      // (todo)
      // sostenuto_notes.empty();

      for(sUI noteIdx = 0u; noteIdx < 128u; noteIdx++)
      {
         voicepressure    [noteIdx] = 0.0f;
         voicepressure_max[noteIdx] = 0.0f;
      }

#ifdef SR_TRACK_FX
      num_outputs = 0u;
      memset(&plugins, 0, sizeof(plugins));
      num_plugins = 0u;
#endif // SR_TRACK_FX

      track_vol = 0.0f;
      track_pan = 0.0f;

      b_fx_auto_note_on = YAC_FALSE;
      b_receive_patch_changes = YAC_TRUE;
   }

#ifdef SR_FX
   void freePlugins(void) {
      // Voice plugins: free shared instances
      sample_player.unloadVoicePlugins();

#ifdef SR_TRACK_FX
      // Track plugins
      for(sUI pluginIdx = 0u; pluginIdx < num_plugins; pluginIdx++)
      {
         st_plugin_info_t *info = plugins[pluginIdx].info;
         if(NULL != info)
         {
            Dprintf("[...] SR_Track::freePlugins: free plugin id=\"%s\"\n", info->id);
            st_plugin_voice_t *voice = plugins[pluginIdx].voice;
            if(NULL != voice)
            {
               if(NULL != info->voice_delete)
               {
                  info->voice_delete(voice);
               }
               plugins[pluginIdx].voice = NULL;
            }

            st_plugin_shared_t *shared = plugins[pluginIdx].shared;
            if(NULL != shared)
            {
               if(NULL != info->shared_delete)
               {
                  info->shared_delete(shared);
               }
               plugins[pluginIdx].shared = NULL;
            }

            plugins[pluginIdx].info = NULL;
         }
      }
      num_plugins = 0u;
#endif // SR_TRACK_FX
   }

#ifdef SR_TRACK_FX
   st_plugin_voice_t *addPluginById(const char *_id) {
      st_plugin_voice_t *voice = NULL;
      if(num_plugins < SR_MAX_TRACK_PLUGINS)
      {
         SR_PluginDef *def = sr_find_plugin_def_by_id(_id);
         if(NULL != def)
         {
            voice = sr_create_plugin_instance_by_def(def);
            if(NULL != voice)
            {
               plugins[num_plugins].info   = def->info;
               plugins[num_plugins].shared = voice->shared;
               plugins[num_plugins].voice  = voice;
               num_plugins++;
               Dprintf("[...] SR_Track::addPluginById: added plugin id=\"%s\"\n", _id);
            }
            else
            {
               Dprintf("[~~~] SR_Track::addPluginById: failed to instantiate plugin id=\"%s\"\n", _id);
            }
         }
         else
         {
            Dprintf("[~~~] SR_Track::addPluginById: unknown plugin id=\"%s\"\n", _id);
         }
      }
      else
      {
         Dprintf("[~~~] SR_Track::addPluginById: maximum number of plugins (%u) exceeded\n", SR_MAX_TRACK_PLUGINS);
      }
      return voice;
   }
#endif // SR_TRACK_FX

#endif // SR_FX

   SR_Track(void) {
      resetAllControllers();
   }

   ~SR_Track(void) {
      Ddtor("~SR_Track()\n");

#ifdef SR_FX
      freePlugins();
#endif // SR_FX
   }

   void init(sUI _poly, sF32 _mixRate, sF32 _volume) {
      sample_player.allocVoices(_poly);
      sample_player.setVolume(_volume);
      sample_player.setDefaultMixRate(_mixRate);
      sample_player.setDefaultFreqTable(&loc_sr_freq_table);
   }

   sF32 getModVal(sU8 _modSrc) {
      sF32 modVal = 0.0f;

      switch(_modSrc)
      {
         default:
         case SR_MODSRC_CC_NONE:
            break;

         case SR_MODSRC_CC_MODWHEEL:
            modVal = cc_modwheel / 127.0f;
            break;

         case SR_MODSRC_CC_BREATH:
            modVal = cc_breath / 127.0f;
            break;

         case SR_MODSRC_CC_FOOT:
            modVal = cc_foot / 127.0f;
            break;

         case SR_MODSRC_CC_EXPR:
            modVal = cc_expr / 127.0f;
            break;

         case SR_MODSRC_CC_BRIGHTNESS:
            modVal = cc_brightness / 127.0f;
            break;

         case SR_MODSRC_CHPRESSURE:
            modVal = aftertouch / 127.0f;
            break;
      }
      return modVal;
   }

   void process(YAC_FloatArray &outBuf
#ifdef SR_TRACK_FX
                , sF32 *trackOutputBuffers
                , sUI outOffsetInSamples/*in samples*/
#endif // SR_TRACK_FX
                ) {
#ifdef SR_TRACK_FX
      YAC_FloatArray fxBuf;
      YAC_FloatArray *d = &fxBuf;
      d->elements = mix_buffer.elements + outOffsetInSamples;
      d->num_elements = outBuf.num_elements;
#else
      YAC_FloatArray *d = &outBuf;
#endif // SR_TRACK_FX

      sample_player._render(d);

      // Dprintf("d->elements[0]=%f\n", d->elements[0]);

// #if 1 // xxxxyy
//       for(sUI xxx = 0u; xxx < outBuf.num_elements; xxx++)
//          outBuf.elements[xxx] += d->elements[xxx];
//       return;
// #endif

#ifdef SR_TRACK_FX
      const sUI numFrames = d->num_elements / 2u;

      for(sUI pluginIdx = 0u; pluginIdx < num_plugins; pluginIdx++)
      {
         st_plugin_info_t  *info  = plugins[pluginIdx].info;
         st_plugin_voice_t *voice = plugins[pluginIdx].voice;

         voice->voice_bus_read_offset = 0u;
         voice->voice_bus_buffers = NULL; // (todo)

         if(NULL != info->prepare_block)
            info->prepare_block(voice,
                                numFrames,
                                261.63f/*freqHz*/,
                                5.0f*12.0f/*freqNote=middle C*/,
                                1.0f/*vol*/,
                                0.0f/*pan*/
                                );

         if(NULL != info->process_replace)
            info->process_replace(voice,
                                  YAC_FALSE/*bMonoIn*/,
                                  d->elements/*samplesIn*/,
                                  d->elements/*samplesOut*/,
                                  numFrames
                                  );

      } // loop plugins

#if 0
         if(3u == midi_port && 0u == midi_ch) // xxxxyy
         {
            Dprintf("xxx drums num_outputs=%u\n", num_outputs);
         }
#endif

      for(sUI outputIdx = 0u; outputIdx < num_outputs; outputIdx++)
      {
         sF32 *out;
         if(0u/*default*/ == outputs[outputIdx].out_dest)
         {
            out = outBuf.elements;
         }
         else
         {
            out = trackOutputBuffers + ((outputs[outputIdx].out_dest - 1u) * SR_MAX_FRAMES_PER_CALL * 2u/*stereo*/);
            // Dprintf("xxx out_dest=%u trackOutOff=%u\n", outputs[outputIdx].out_dest, ((outputs[outputIdx].out_dest - 1u) * SR_MAX_FRAMES_PER_CALL * 2u));
         }

         // add track buffer to FX send or default output buffer
         sUI k = 0u;
         const sF32 volL = outputs[outputIdx].level_l;
         const sF32 volR = outputs[outputIdx].level_r;
#if 0
         if(0u == midi_port && 0u == midi_ch) // xxxxyy
         {
            Dprintf("xxx send to outputNr=%u/%u level=(%f; %f) level_track=%f level_lane=%f out_dest=%u out=%p trackOutputBuffers=%p outEnd=%p\n", (outputIdx+1u), num_outputs, volL, volR, outputs[outputIdx].level_track, outputs[outputIdx].level_lane, outputs[outputIdx].out_dest, out, trackOutputBuffers, out+(numFrames*2));
         }
#endif
         for(sUI frameIdx = 0u; frameIdx < numFrames; frameIdx++)
         {
            out[k] += d->elements[k] * volL;
            k++;
            out[k] += d->elements[k] * volR;
            k++;
         }
      } // loop outputs
#endif // SR_TRACK_FX
   }

};


// <class.png>
class SR_Project {
public:
   SR_Track tracks[SR_MAX_TRACKS];
   sUI num_tracks;

   SR_Sample samples[SR_MAX_SAMPLES];
   sUI num_samples;
   sF32 *wf_dat;  // all waveforms

#ifdef SR_TRACK_FX
   YAC_FloatArray track_mix_buffers;  // <num_tracks> view into SR_Project.track_mix_buffers
#endif // SR_TRACK_FX

public:
   SR_Project(void) {
      num_tracks = 0u;
      num_samples = 0u;
      wf_dat = NULL;
   }

   ~SR_Project(void) {
      Ddtor("~SR_Project()\n");
      YAC_DELETE_SAFE(wf_dat);
      Ddtor("~SR_Project() 2\n");
   }

#ifdef SR_FX
   void freePlugins(void) {
      // Voice+track plugins: free shared instances
      for(sUI trackIdx = 0u; trackIdx < num_tracks; trackIdx++)
      {
         SR_Track *track = &tracks[trackIdx];
         track->freePlugins();
      }

#ifdef SR_VOICE_FX
      // Voice plugins: free voice instances (+other sample data)
      for(sUI smpIdx = 0u; smpIdx < num_samples; smpIdx++)
      {
         SR_Sample *smp = &samples[smpIdx];
         smp->sample_bank._freeSamples();
      }
#endif // SR_VOICE_FX
   }
#endif // SR_FX

   sBool allocTrackMixBuffers(void) {
      sBool r = YAC_TRUE;
#ifdef SR_TRACK_FX
      if(track_mix_buffers.alloc(SR_MAX_FRAMES_PER_CALL * 2u * num_tracks))
      {
         // Dprintf("xxx alloc track_mix_buffers ne=%u\n", track_mix_buffers.num_elements);
         // Create per-track view into mix buffers
         sF32 *d = track_mix_buffers.elements;
         for(sUI trackIdx = 0u; trackIdx < num_tracks; trackIdx++)
         {
            SR_Track *track = &tracks[trackIdx];
            YAC_FloatArray *fa = &track->mix_buffer;
            fa->elements     = d;
            fa->own_data     = YAC_FALSE;
            fa->num_elements = fa->max_elements = SR_MAX_FRAMES_PER_CALL * 2u;

            // Next track
            d += SR_MAX_FRAMES_PER_CALL * 2u;
         }
      }
      else
      {
         Derror("[---] failed to allocate track_mix_buffers (%u bytes)\n", (SR_MAX_FRAMES_PER_CALL * 2u * num_tracks));
         r = YAC_FALSE;
      }
#endif // SR_TRACK_FX
      return r;
   }

   SR_Sample *findSampleByPrgId(sU8 _prg) {
      for(sUI i = 0u; i < num_samples; i++)
      {
         SR_Sample *s = &samples[i];
         if(s->prg_id == _prg)
            return s;
      }
      return NULL;
   }

   SR_Sample *findSampleByUniqueId(sU16 _uid) {
      for(sUI i = 0u; i < num_samples; i++)
      {
         SR_Sample *s = &samples[i];
         if(s->unique_id == _uid)
            return s;
      }
      return NULL;
   }

   void selectTrackDefaultSamples(void) {
   }

   void setTempo(sF32 _bpm, sUI _ppq) {
      for(sUI trackIdx = 0u; trackIdx < num_tracks; trackIdx++)
      {
         SR_Track *track = &tracks[trackIdx];
         track->sample_player.setTempo(_bpm, _ppq);  // synced ModSequencers
      }
   }

   void midiNoteOn(sU8 _port, sU8 _ch, sU8 _note, sU8 _vel) {
      for(sUI trackIdx = 0u; trackIdx < num_tracks; trackIdx++)
      {
         SR_Track *track = &tracks[trackIdx];
         if(track->midi_port == _port)
         {
            if( (track->midi_ch < 0) || (track->midi_ch == _ch) )
            {
               // Dprintf("xxx vel=%u min=%u max=%u\n", _vel, track->vel_min, track->vel_max);
               if( (_vel >= track->vel_min) && (_vel <= track->vel_max) )
               {
                  // Dprintf("xxx note=%u min=%u max=%u trp=%d\n", _note, track->note_min, track->note_max, track->note_trp);
                  if( (_note >= track->note_min) && (_note <= track->note_max) )
                  {
                     sSI note = sSI(_note) + track->note_trp;
                     note = sRANGE(note, 0, 127);

                     SR_Sample *smp = findSampleByPrgId(track->cur_prg);
                     Dreplay("midiNoteOn: port=%u(%c) ch=%u note=%d vel=%u prg=%u smp=%p\n", _port, (char)(_port+'a'), _ch, note, _vel, track->cur_prg, smp);
                     if(NULL != smp)
                     {
                        StSamplePlayer *sp = &track->sample_player;

                        sF32 modVal = track->getModVal(smp->mod_src);

                        sSI voicekey = sp->startSampleBank(&smp->sample_bank,
                                                           NULL/*freqTable=def*/,
                                                           note,
                                                           _vel / 127.0f,
                                                           modVal,
                                                           1.0f/*volume*/,
                                                           0.0f/*pan*/,
                                                           0.0f/*freq*/
                                                           );
                        if(voicekey > 0)
                        {
                           // Dprintf("xxx noteon update vol=%f\n", track->cc_vol_norm * track->track_vol);
                           sp->updateVol(track->cc_vol_norm * track->track_vol);
                           sp->updatePan(track->cc_pan_norm + track->track_pan);

                           sF32 freq;
                           if(track->pitchbend_norm < 0)
                              freq = smp->pitchbend_down * track->pitchbend_norm;
                           else
                              freq = smp->pitchbend_up * track->pitchbend_norm;
                           sp->updateFreq(freq);

                           Dreplay("midiNoteOn: voicekey=%d ccvol=%f trkvol=%f ccpan=%f trkpan=%f freq=%f\n", voicekey, track->cc_vol_norm, track->track_vol, track->cc_pan_norm, track->track_pan, freq);

                           if(SR_MODSRC_POLYPRESSURE == smp->mod_src)
                           {
                              sp->updateModByKey(voicekey, track->voicepressure[note]);
                           }
                           else if(SR_MODSRC_POLYPRESSURE_MAX == smp->mod_src)
                           {
                              // Reset pressure and only update when next new value is higher than last
                              sp->updateModByKey(voicekey, 0.0f);
                              track->voicepressure[note] = 0.0f;
                              track->voicepressure_max[note] = 0.0f;
                           }

                           sp->_initStartedVoicesByKey(voicekey);
                        }
                     }
                  } // note range
               } // vel range
            } // if match track midi ch
         } // if match track midi port
      } // loop tracks
   }

   void midiNoteOff(sU8 _port, sU8 _ch, sU8 _note, sU8 _vel) {
      for(sUI trackIdx = 0u; trackIdx < num_tracks; trackIdx++)
      {
         SR_Track *track = &tracks[trackIdx];
         if(track->midi_port == _port)
         {
            if( (track->midi_ch < 0) || (track->midi_ch == _ch) )
            {
               if( (_note >= track->note_min) && (_note <= track->note_max) )
               {
                  sSI note = sSI(_note) + track->note_trp;
                  note = sRANGE(note, 0, 127);

                  // SR_Sample *smp = findSampleByPrgId(track->cur_prg);
                  Dreplay("midiNoteOff: port=%u ch=%u note=%u vel=%u\n", _port, _ch, _note, _vel);
                  track->sample_player.noteOff(note, _vel/127.0f);
               }
            }
         }
      }
   }

   void midiPitchbend(sU8 _port, sU8 _ch, sU16 _v) {
      for(sUI trackIdx = 0u; trackIdx < num_tracks; trackIdx++)
      {
         SR_Track *track = &tracks[trackIdx];
         if(track->midi_port == _port)
         {
            if( (track->midi_ch < 0) || (track->midi_ch == _ch) )
            {
               StSamplePlayer *sp = &track->sample_player;
               sp->updatePerfCtl(STSAMPLEPLAYER_PERFCTL_PITCHBEND, _v);
               track->pitchbend_norm = (_v >= 8192) ? ((_v - 8192)/8192.0f) : ((_v - 8192) / 8191.0f);
               SR_Sample *smp = findSampleByPrgId(track->cur_prg);
               if(NULL != smp)
               {
                  sF32 freq;
                  if(track->pitchbend_norm < 0)
                     freq = smp->pitchbend_down * track->pitchbend_norm;
                  else
                     freq = smp->pitchbend_up * track->pitchbend_norm;
                  sp->updateFreq(freq);
                  // Dprintf("xxx pb freq=%f port=%u ch=%u v=%u\n", freq, _port, _ch, _v);
               }
            }
         }
      }
   }

   void midiChannelPressure(sU8 _port, sU8 _ch, sU8 _v) {
      for(sUI trackIdx = 0u; trackIdx < num_tracks; trackIdx++)
      {
         SR_Track *track = &tracks[trackIdx];
         if(track->midi_port == _port)
         {
            if( (track->midi_ch < 0) || (track->midi_ch == _ch) )
            {
               StSamplePlayer *sp = &track->sample_player;
               sp->updatePerfCtl(STSAMPLEPLAYER_PERFCTL_PRESSURE, _v);
            }
         }
      }
   }

   void midiPolyPressure(sU8 _port, sU8 _ch, sU8 _note, sU8 _v) {
      for(sUI trackIdx = 0u; trackIdx < num_tracks; trackIdx++)
      {
         SR_Track *track = &tracks[trackIdx];
         if(track->midi_port == _port)
         {
            if( (track->midi_ch < 0) || (track->midi_ch == _ch) )
            {
               StSamplePlayer *sp = &track->sample_player;
               sp->updatePerfCtlByNote(_note, STSAMPLEPLAYER_PERFCTL_PRESSURE, _v);
            }
         }
      }
   }

   void midiCC(sU8 _port, sU8 _ch, sU8 _cc, sU8 _v) {
      for(sUI trackIdx = 0u; trackIdx < num_tracks; trackIdx++)
      {
         SR_Track *track = &tracks[trackIdx];
         if(track->midi_port == _port)
         {
            if( (track->midi_ch < 0) || (track->midi_ch == _ch) )
            {
               StSamplePlayer *sp = &track->sample_player;
               sBool bUpdateMod = YAC_FALSE;
               switch(_cc)
               {
                  default:
                     break;

                  case 1u: // modwheel
                     track->cc_modwheel = _v;
                     bUpdateMod = YAC_TRUE;
                     sp->updatePerfCtl(STSAMPLEPLAYER_PERFCTL_CC1_MODWHEEL, _v);
                     // // Dprintf("xxx update modwheel v=%u port=%u ch=%u\n", _v, _port, _ch);
                     break;

                  case 2u: // breath control
                     track->cc_breath = _v;
                     bUpdateMod = YAC_TRUE;
                     sp->updatePerfCtl(STSAMPLEPLAYER_PERFCTL_CC2_BREATHCTL, _v);
                     break;

                  case 4u: // foot control
                     track->cc_breath = _v;
                     bUpdateMod = YAC_TRUE;
                     sp->updatePerfCtl(STSAMPLEPLAYER_PERFCTL_CC4_FOOTCTL, _v);
                     break;

                  case 7u: // volume
                     track->cc_vol_norm = _v / 127.0f;
                     sp->updateVol(track->cc_vol_norm * track->track_vol);
                     sp->updatePerfCtl(STSAMPLEPLAYER_PERFCTL_CC7_VOLUME, _v);
                     break;

                  case 8u: // balance
                     sp->updatePerfCtl(STSAMPLEPLAYER_PERFCTL_CC8_BALANCE, _v);
                     break;

                  case 10u: // panorama
                     track->cc_pan_norm = (_v < 64) ? ((_v - 64)/64.0f) : ((_v - 64) / 63.0f);
                     sp->updatePan(track->cc_pan_norm + track->track_pan);
                     sp->updatePerfCtl(STSAMPLEPLAYER_PERFCTL_CC10_PAN, _v);
                     break;

                  case 11u: // expression
                     track->cc_expr = _v;
                     bUpdateMod = YAC_TRUE;
                     sp->updatePerfCtl(STSAMPLEPLAYER_PERFCTL_CC11_EXPRESSION, _v);
                     break;

                  case 16u: // general purpose 1
                  case 17u: // general purpose 2
                  case 18u: // general purpose 3
                  case 19u: // general purpose 4
                     sp->updatePerfCtl(STSAMPLEPLAYER_PERFCTL_CC16_GENERAL_1 + (_cc - 16u), _v);
                     break;

                  case 64u: // sustain pedal (todo)
                     break;

                  case 65u: // portamento pedal
                     sp->updateGlideSwitch(_v >= 64);
                     break;

                  case 66u: // sostenuto (todo)
                     break;

                  case 74u: // brightness
                     track->cc_brightness = _v;
                     bUpdateMod = true;
                     sp->updatePerfCtl(STSAMPLEPLAYER_PERFCTL_CC74_BRIGHTNESS, _v);
                     break;

                  case 80u: // general purpose 5
                  case 81u: // general purpose 6
                  case 82u: // general purpose 7
                  case 83u: // general purpose 8
                     sp->updatePerfCtl(STSAMPLEPLAYER_PERFCTL_CC80_GENERAL_5 + (_cc - 80u), _v);
                     break;

                  case 121u:  // Controller Reset
                     track->resetAllControllers();
                     break;

                  case 123u:  // All Notes off
                     sp->allNotesOff();
                     break;

               } // switch cc

               if(bUpdateMod)
               {
                  SR_Sample *smp = findSampleByPrgId(track->cur_prg);
                  if(NULL != smp)
                  {
                     if(!smp->b_mod_src_noteon)
                     {
                        sF32 modVal = track->getModVal(smp->mod_src);
                        sp->updateMod(modVal);
                     }
                  }
               }

            } // if match track midi ch
         } // if match track midi port
      } // loop tracks
   }

   void midiProgramChange(sU8 _port, sU8 _ch, sU8 _program) {
      for(sUI trackIdx = 0u; trackIdx < num_tracks; trackIdx++)
      {
         SR_Track *track = &tracks[trackIdx];
         if(track->midi_port == _port)
         {
            if(track->b_receive_patch_changes)
            {
               if( (track->midi_ch < 0) || (track->midi_ch == _ch) )
               {
                  track->cur_prg = (sS8)_program;

                  SR_Sample *smp = findSampleByPrgId(track->cur_prg);
                  if(NULL != smp)
                  {
                     Dreplay("xxx midiProgramChange: port=%u ch=%u select program=0x%02x => smp=%p transpose(oct=%d semi=%d cents=%d)\n", _port, _ch, _program, smp, smp->transpose_oct, smp->transpose_semi, smp->transpose_cents);
                     StSamplePlayer *sp = &track->sample_player;
                     sp->setLastStartedSampleBankHint(&smp->sample_bank);
                     for(sUI srPerfCtlIdx = 0u; srPerfCtlIdx < SR_PERFCTL_NUM; srPerfCtlIdx++)
                     {
                        if(smp->init_perf_ctl[srPerfCtlIdx] >= 0)
                           sp->updatePerfCtl(loc_sr_perfctl_map[srPerfCtlIdx], smp->init_perf_ctl[srPerfCtlIdx]);
                     }
                  }
                  else
                  {
                     Dreplay("xxx midiProgramChange: port=%u ch=%u select program=0x%2x => smp=%p\n", _port, _ch, smp);
                  }
               } // if match track midi ch
            } // if b_receive_patch_changes
         } // if match track midi port
      } // loop tracks
   }

   void midiRPN(sU8 _port, sU8 _ch, sU16 _rpn, sU16 _v) {
      Dreplay2("xxx midiRPN: port=%u ch=%u rpn=%u v=%u\n", _port, _ch, _rpn, _v);
      for(sUI trackIdx = 0u; trackIdx < num_tracks; trackIdx++)
      {
         SR_Track *track = &tracks[trackIdx];
         if(track->midi_port == _port)
         {
            if( (track->midi_ch < 0) || (track->midi_ch == _ch) )
            {
               StSamplePlayer *sp = &track->sample_player;
               // // sBool bUpdateMod = YAC_FALSE;
               switch(_rpn)
               {
                  default:
                     break;

                  case 90u: // smp_uid (see remote.msp)
                  {
                     if(track->b_receive_patch_changes)
                     {
                        SR_Sample *smp = findSampleByUniqueId(_v);
                        // Dreplay("xxx select smp by uid: port=%u ch=%u uid=%u(0x%04x) => smp=%p\n", _port, _ch, _v, _v, smp);
                        if(NULL != smp)
                        {
                           track->cur_prg = smp->prg_id;
                           // // track->cur_prg = 11;//xxxxxxxxxxxxxxx
                           // Dreplay("xxx select smp by uid:   => smp->prg_id=%d\n", smp->prg_id);

                           if(!smp->b_mod_src_noteon)
                           {
                              sF32 modVal = track->getModVal(smp->mod_src);
                              sp->updateMod(modVal);
                           }
                        }
                        else
                        {
                           track->cur_prg = -1;
                        }
                     }
                     break;
                  }

                  case 300u: // trk_1_lvl
                  case 301u: // trk_2_lvl
                  case 302u: // trk_3_lvl
                  case 303u: // trk_4_lvl
                  case 304u: // trk_5_lvl
                  case 305u: // trk_6_lvl
                  case 306u: // trk_7_lvl
                  case 307u: // trk_8_lvl
                  {
                     const sUI origTrackOutputIdx = _rpn - 300u;
                     for(sUI outputIdx = 0u; outputIdx < SR_MAX_TRACK_OUTPUTS; outputIdx++)
                     {
                        if(track->outputs[outputIdx].orig_track_output_idx == (sSI)origTrackOutputIdx)
                        {
                           track->outputs[outputIdx].level_track = _v / 16383.0f;
                           const sF32 outVol = track->outputs[outputIdx].level_track * track->outputs[outputIdx].level_lane;
                           const sF32 outPan = track->outputs[outputIdx].pan;
                           track->outputs[outputIdx].level_l =
                              outVol * ((outPan < 0.0f) ? 1.0f : (1.0f - outPan));
                           track->outputs[outputIdx].level_r =
                              outVol * ((outPan > 0.0f) ? 1.0f : (1.0f + outPan));
                           Dreplay2("xxx   RPN trk_%u_lvl outputNr=%u/%u port=%u ch=%u v=%u levelTrack=%f levelLane=%f outVol=%f outPan=%f out_dest=%u\n", (origTrackOutputIdx+1u), (outputIdx+1u), SR_MAX_TRACK_OUTPUTS, _port, _ch, _v, track->outputs[outputIdx].level_track, track->outputs[outputIdx].level_lane, outVol, outPan, track->outputs[outputIdx].out_dest);
                        }
                     }
                     break;
                  }

               } // switch rpn

               // // if(bUpdateMod)
               // // {
               // //    SR_Sample *smp = findSampleByPrgId(track->cur_prg);
               // //    if(NULL != smp)
               // //    {
               // //       if(!smp->b_mod_src_noteon)
               // //       {
               // //          sF32 modVal = track->getModVal(smp->mod_src);
               // //          sp->updateMod(modVal);
               // //       }
               // //    }
               // // }

            } // if match track midi ch
         } // if match track midi port
      } // loop tracks
   }

};


// <class.png>
struct SR_Event {
   sU32 abs_time;
   union {
      sU32 u32;
      sU8 bytes[4];
   } data;
};


// <class.png>
class SR_SongTrack {
   // (note) one track per SR_Track::midi_port
public:
   sUI midi_port;  // vst_a..vst_p => 0..15
   sUI num_ticks;  // e.g. 3072 at ppq=192 = 4 4/4 bars
   SR_Event *events;
   sUI num_events;
   sUI ev_idx;

   struct {
      sU16 rpn;      // 14bit. msb received first.  (CC#101, CC#100)
      sU16 rpn_val;  // 14bit. msb received first.  (CC#6, CC#38)
   } channels[16];

public:
   SR_SongTrack(void) {
      num_ticks  = 0u;
      events     = NULL;
      num_events = 0u;
      memset(channels, 0, sizeof(channels));
   }

   ~SR_SongTrack() {
      Ddtor("~SR_SongTrack()\n");
      if(NULL != events)
      {
         delete [] events;
         events = NULL;
         num_events = 0u;
      }
   }

   void start(void) {
      ev_idx = 0u;
   }

   void tick(SR_Project &proj, const sUI absTime) {
      while(ev_idx < num_events)
      {
         const SR_Event *ev = &events[ev_idx];
         Dreplay2("[trc] tick ev_idx=%u numEv=%u ev->abs_time=%u absTime=%u evHdr=0x%2x\n", ev_idx, num_events, ev->abs_time, absTime, ev->data.bytes[0]);
         if(absTime >= ev->abs_time)
         {
            sS8 midiCh = ev->data.bytes[0] & 15u;
            switch(ev->data.bytes[0] & 0xF0u)
            {
               case 0x80u:
               {
                  Dreplay("[trc] tick [%8u] note off %u vel=%u\n", absTime, ev->data.bytes[1], ev->data.bytes[2]);
                  proj.midiNoteOff((sU8)midi_port,
                                   midiCh,
                                   ev->data.bytes[1]/*note*/,
                                   ev->data.bytes[2]/*vel*/
                                   );
               }
               break;

               case 0x90u:
               {
                  Dreplay("[trc] tick [%8u] note on %u vel=%u port=%u(%c) ch=%d\n", absTime, ev->data.bytes[1], ev->data.bytes[2], (sU8)midi_port, (char)(midi_port+'a'), midiCh);
                  proj.midiNoteOn((sU8)midi_port,
                                  midiCh,
                                  ev->data.bytes[1]/*note*/,
                                  ev->data.bytes[2]/*vel*/
                                  );
               }
               break;

               case 0xA0u: // Poly Pressure
               {
                  const sU8 note = ev->data.bytes[1];
                  const sU8 val  = ev->data.bytes[2];
                  proj.midiPolyPressure((sU8)midi_port,
                                        midiCh,
                                        note,
                                        val
                                        );
                  break;
               }

               case 0xB0u:
               {
                  Dreplay("[trc] tick [%8u] CC nr=%u val=%u port=%u(%c) ch=%d\n", absTime, ev->data.bytes[1], ev->data.bytes[2], (sU8)midi_port, (char)(midi_port+'a'), midiCh);
                  const sU8 ccNr  = ev->data.bytes[1];
                  const sU8 ccVal = ev->data.bytes[2];
                  if(101 == ccNr)
                  {
                     channels[midiCh].rpn = (channels[midiCh].rpn & 127u) | (ccVal << 7);
                  }
                  else if(100 == ccNr)
                  {
                     channels[midiCh].rpn = (channels[midiCh].rpn & 0x3F80u) | ccVal;
                  }
                  else if(6 == ccNr)
                  {
                     channels[midiCh].rpn_val = (channels[midiCh].rpn_val & 127u) | (ccVal << 7);
                  }
                  else if(38 == ccNr)
                  {
                     channels[midiCh].rpn_val = (channels[midiCh].rpn_val & 0x3F80u) | ccVal;
                     // (note) RPN message complete
                     proj.midiRPN((sU8)midi_port,
                                  midiCh,
                                  channels[midiCh].rpn,
                                  channels[midiCh].rpn_val
                                  );
                  }
                  else
                  {
                     proj.midiCC((sU8)midi_port,
                                 midiCh,
                                 ccNr,
                                 ccVal
                                 );
                  }
               }
               break;

               case 0xC0u:
               {
                  Dreplay("[trc] tick [%8u] program change to #%u(0x%x) port=%u(%c) ch=%d\n", absTime, (ev->data.bytes[1]+1u), ev->data.bytes[1], (sU8)midi_port, (char)(midi_port+'a'), midiCh);
                  proj.midiProgramChange((sU8)midi_port,
                                         midiCh,
                                         ev->data.bytes[1]/*program*/
                                         );
               }
               break;

               case 0xD0u:  // Channel Pressure
               {
                  const sU8 val = ev->data.bytes[1];
                  proj.midiChannelPressure((sU8)midi_port,
                                           midiCh,
                                           val
                                           );
                  break;
               }

               case 0xE0u:
               {
                  const sU16 val = (ev->data.bytes[2] << 7) | ev->data.bytes[1];
                  Dreplay("[trc] tick [%8u] PB val=%u port=%u(%c) ch=%d\n", absTime, val, (sU8)midi_port, (char)(midi_port+'a'), midiCh);
                  proj.midiPitchbend((sU8)midi_port,
                                     midiCh,
                                     val
                                     );
                  break;
               }
            }

            // Next event
            ev_idx++;
         }
         else
            break;
      }
   }

};


// <class.png>
class SR_Song {
#define SR_SONG_MAX_TRACKS  16
public:
   SR_SongTrack tracks[SR_SONG_MAX_TRACKS];
   sUI  num_tracks;
   sF32 bpm;
   sUI  ppq;
   sUI  num_ticks;
   sF32 frames_per_tick;
   sF32 tick_frames_left;
   sUI  tick_idx;

public:
   SR_Song(void) {
      bpm = 120.0f;
      ppq = 192u;
   }

   ~SR_Song() {
      Ddtor("~SR_Song()\n");
   }

   void start(void) {
      tick_idx = 0u;
      tick_frames_left = 0.0f;
      for(sUI trackIdx = 0u; trackIdx < num_tracks; trackIdx++)
      {
         tracks[trackIdx].start();
      }
   }

   void process(SR_Project &proj, YAC_FloatArray &outBuf) {
      sUI framesLeft = outBuf.num_elements / 2u/*stereo*/;
      YAC_FloatArray frameBuf;
      frameBuf.elements = outBuf.elements;

#ifdef SR_TRACK_FX
      // Clear track mix buffers
      sF32 *trackBuffers = proj.track_mix_buffers.elements;
      // Dprintf("xxx start of trackBuffers=%p end=%p\n", trackBuffers, trackBuffers+proj.track_mix_buffers.num_elements);
      memset((void*)trackBuffers, 0, proj.track_mix_buffers.num_elements * sizeof(sF32));
      sUI outOffsetInSamples = 0u;
#endif // SR_TRACK_FX

      while(framesLeft > 0u)
      {
         if(tick_frames_left <= 0.0f)
         {
            if(tick_idx >= num_ticks)
               start();

            // Play sequence
            for(sUI trackIdx = 0u; trackIdx < num_tracks; trackIdx++)
            {
               SR_SongTrack *track = &tracks[trackIdx];
#if 0
               if(1u == track->midi_port)  // xxxxyy
#endif
               {
                  track->tick(proj, tick_idx);
               }
            }
            tick_frames_left += frames_per_tick;
            tick_idx++;
         }

         sUI numFrames = (framesLeft > 128u) ? 128u : framesLeft;
         if(sSI(numFrames) > sSI(tick_frames_left))
            numFrames = sUI(tick_frames_left);
         Dreplay2("[trc] render numFrames=%u framesLeft=%u tick_frames_left=%f\n", numFrames, framesLeft, tick_frames_left);
         if(numFrames < 1u)
            numFrames = 1u;

         // Render tracks
         frameBuf.num_elements = numFrames * 2u;
         frameBuf.max_elements = frameBuf.num_elements; // could be skipped (never read)
         for(sUI trackIdx = 0u; trackIdx < proj.num_tracks; trackIdx++)
         {
            proj.tracks[trackIdx].process(frameBuf
#ifdef SR_TRACK_FX
                                          , trackBuffers/*trackOutputBuffers*/
                                          , outOffsetInSamples
#endif // SR_TRACK_FX
                                          );
         }

         // Next chunk
         tick_frames_left -= numFrames;
         framesLeft -= numFrames;
         frameBuf.elements += frameBuf.num_elements;
#ifdef SR_TRACK_FX
         trackBuffers += frameBuf.num_elements;
         outOffsetInSamples += frameBuf.num_elements;
#endif // SR_TRACK_FX
      }

      // Dprintf("xxx trackBuffersOff=%u\n", (sUI)(size_t)(trackBuffers - proj.track_mix_buffers.elements));
   }
};


sBool sr_proj_load_buffer(SR_BufferStreamLE &ifs, SR_Project &proj) {
   sBool r = YAC_FALSE;

   sBool b = (ifs.u8() == 'S');
   b  = b && (ifs.u8() == 'Y');
   b  = b && (ifs.u8() == 'N');
   b  = b && (ifs.u8() == 'G');
   if(b)
   {
      const sU16 ver = ifs.u16();
      if(ver >= 1u)
      {
         const sUI wfTotal = ifs.u32();
         Dtrace("[dbg] sr_proj_load: wfTotal=%u (%4.2fk)\n", wfTotal, (wfTotal*4)/1024.0f);

         proj.wf_dat = new(std::nothrow)sF32[wfTotal];
         if(NULL != proj.wf_dat)
         {
            memset((void*)proj.wf_dat, 0, wfTotal*sizeof(sF32));

            proj.num_tracks = ifs.u8();
            Dtrace("[dbg] sr_proj_load: #tracks=%u\n", proj.num_tracks);

            sUI maxPoly = 0u;

            for(sUI trackIdx = 0u; trackIdx < proj.num_tracks; trackIdx++)
            {
               SR_Track *track = &proj.tracks[trackIdx];
               track->midi_port = ifs.u8();   // (todo) port/ch could be merged into one byte
               track->midi_ch   = ifs.s8();

               const sUI numMods = ifs.u8();
               // Dprintf("xxx track %u numMods=%u\n", trackIdx, numMods);
               for(sUI modIdx = 0u; modIdx < numMods; modIdx++)
               {
                  sU8 modType = ifs.u8();

                  if(0u/*sample*/ == modType)
                  {
                     const sUI poly = ifs.u8() + 1u;
                     maxPoly += poly;
                     track->init(poly, SR_MIX_RATE, SR_VOLUME);
                     track->def_prg   = ifs.s8();
                     track->cur_prg   = track->def_prg;

                     const sUI modSampleFlags = ifs.u8();
                     track->b_fx_auto_note_on       = (0u != (modSampleFlags & (1u << 0)));
                     track->b_receive_patch_changes = (0u != (modSampleFlags & (1u << 1)));
                     // // Dprintf("xxx track->b_receive_patch_changes=%d\n", track->b_receive_patch_changes);

                     track->note_min  = ifs.u8();
                     track->note_max  = ifs.u8();
                     track->note_trp  = ifs.s8();
                     track->vel_min   = ifs.u8();
                     track->vel_max   = ifs.u8();
                  }
                  else
                  {
                     // STFX plugin
                     char pluginId[128];
                     (void)ifs.readString(pluginId, 128);
                     Dtrace("[trc] read plugin id=\"%s\"\n", pluginId);
#ifdef SR_TRACK_FX
                     st_plugin_voice_t *voice = track->addPluginById(pluginId);
                     if(NULL == voice)
                        return YAC_FALSE;
                     st_plugin_shared_t *shared = (NULL != voice) ? voice->shared : NULL;
#endif // SR_TRACK_FX
                     const sUI numParams = ifs.u16();
                     for(sUI paramIdx = 0u; paramIdx < numParams; paramIdx++)
                     {
                        sF32 paramVal = ifs.f32();
#ifdef SR_TRACK_FX
                        if(NULL != shared)
                        {
                           if(paramIdx < shared->info->num_params)
                           {
                              Dtrace("[trc]   plugin param[%u] val=%f\n", paramIdx, paramVal);
                              shared->info->set_param_value(shared, paramIdx, paramVal);
                           }
                           else
                           {
                              Dprintf("[~~~] plugin \"%s\" paramIdx=%u >= numParams=%u, skipping..\n", pluginId, paramIdx, shared->info->num_params);
                           }
                        }
#else
                        (void)paramVal;
#endif // SR_TRACK_FX
                     } // loop params
                  }
               } // loop mods

               const sUI numOutputs = ifs.u8();
               for(sUI outputIdx = 0u; outputIdx < numOutputs; outputIdx++)
               {
                  sU8  outDest = ifs.u8();  // 0u == default, or track 1..n
                  sS8  outOrigTrackOutputIdx = ifs.s8();
                  sF32 outVolTrack = ifs.u8() / 255.0f;
                  sF32 outVolLane  = ifs.u8() / 255.0f;
                  sF32 outVol = outVolTrack * outVolLane;
                  sF32 outPan  = ifs.s8() / 127.0f;

                  Dtrace("[...] output %u/%u dest=%u volTrack=%f volLane=%f pan=%f\n", outputIdx+1u, numOutputs, outDest, outVolTrack, outVolLane, outPan);
#ifdef SR_TRACK_FX
                  if(outputIdx < SR_MAX_TRACK_OUTPUTS)
                  {
                     if(outDest > proj.num_tracks)
                     {
                        Dprintf("[~~~] track u/%u output %u/%u invalid outDest=%u, using default..\n", trackIdx+1u, proj.num_tracks, outputIdx+1u, numOutputs, outDest);
                        outDest = 0u;
                     }
                     track->outputs[outputIdx].out_dest    = outDest;
                     track->outputs[outputIdx].orig_track_output_idx = outOrigTrackOutputIdx;
                     track->outputs[outputIdx].level_track = outVolTrack;
                     track->outputs[outputIdx].level_lane  = outVolLane;
                     track->outputs[outputIdx].level_l     = outVol * ((outPan < 0.0f) ? 1.0f : (1.0f - outPan));
                     track->outputs[outputIdx].level_r     = outVol * ((outPan > 0.0f) ? 1.0f : (1.0f + outPan));
                     track->outputs[outputIdx].pan         = outPan;
                  }
                  else
                  {
                     Dprintf("[~~~] track %u/%u output %u/%u exceeds SR_MAX_TRACK_OUTPUTS, skipping..\n", trackIdx+1u, proj.num_tracks, outputIdx+1u, numOutputs);
                  }
#else
                  // // if(0u/*default*/ == outDest)
                  if(0u == outputIdx)
                  {
                     track->track_vol = outVol;
                     track->track_pan = outPan;
                  }
#endif // SR_TRACK_FX
               }

#ifdef SR_TRACK_FX
               track->num_outputs = sRANGE(numOutputs, 0u, SR_MAX_TRACK_OUTPUTS);
               track->track_vol = 1.0f;  // (note) vol/pan will be applied per output
               track->track_pan = 0.0f;
#endif // SR_TRACK_FX

               Dtrace("[dbg] sr_proj_load: track %u/%u midi_port=%u midi_ch=%d poly=%u def_prg=%d vol=%f pan=%f nr=(%u..%u) trp=%d vr=(%u..%u) #out=%u\n", trackIdx+1u, proj.num_tracks, track->midi_port, track->midi_ch, track->sample_player.getNumVoices(), track->def_prg, track->track_vol, track->track_pan, track->note_min, track->note_max, track->note_trp, track->vel_min, track->vel_max, numOutputs);
            }

            proj.num_samples = ifs.u8();
            Dtrace("[dbg] sr_proj_load: #samples=%u\n", proj.num_samples);

            for(sUI smpIdx = 0u; smpIdx < proj.num_samples; smpIdx++)
            {
               Dtrace("SR_Sample::load: smp %2u/%2u ----------------------------------\n", smpIdx+1u, proj.num_samples);
               SR_Sample *smp = &proj.samples[smpIdx];
               smp->init();
               if(!smp->load(ifs))
               {
                  Dtrace("SR_Sample::load: failed to load smp %u/%u\n", smpIdx+1u, proj.num_samples);
                  return YAC_FALSE;
               }
            }

            // Read and uncompress waveforms
            sF32 *wfDat = proj.wf_dat;
            for(sUI smpIdx = 0u; smpIdx < proj.num_samples; smpIdx++)
            {
               SR_Sample *smp = &proj.samples[smpIdx];
               YAC_FloatArray *smpDat = (YAC_FloatArray*)smp->waveform._getSampleData();
               smpDat->elements = wfDat;

               if(!smp->b_procedural)
               {
#define S16_FRAME_SZ 16  // see YAC_FloatArray::saveToStreamSigned12FrameEnc()
                  SR_BitStream4 bs(&ifs);
                  sUI ne = smpDat->num_elements;
                  sUI numFrames = ne / S16_FRAME_SZ;
                  sUI off = 0u;
                  sS16 cur = 0;
                  sF32 *d = smpDat->elements;

                  Dtrace("[...] read smp %u/%u waveform ioOff=%u wfDatOff=%u ne=%u #frames=%u\n",
                         smpIdx+1u, proj.num_samples,
                         ifs.io_offset,
                         (sUI)(size_t)(d - proj.wf_dat),
                         ne,
                         numFrames
                         );

                  for(sUI frameIdx = 0u; frameIdx < numFrames; frameIdx++)
                  {
                     sU8 type = bs.read4();
                     if( (off + S16_FRAME_SZ) > ne )
                     {
                        // Partial last frame, save raw delta values
                        // => 0u/*TYPE=12bit raw values*/)
                        while(off < ne)
                        {
                           sS16 t = bs.read12();
                           if(t >= 0)
                              d[off] = t / 2047.0f;
                           else
                              d[off] = t / 2048.0f;
                           off++;
                        }
                        break;
                     }
                     else
                     {
                        switch(type & 3u)
                        {
                           case 0u: // TYPE=12bit raw values
                              for(sUI i = 0u; i < S16_FRAME_SZ; i++)
                              {
                                 cur = bs.read12();
                                 if(cur >= 0)
                                    d[off] = cur / 2047.0f;
                                 else
                                    d[off] = cur / 2048.0f;
                                 off++;
                              }
                              break;

                           case 1u: // TYPE=12bit delta values
                              for(sUI i = 0u; i < S16_FRAME_SZ; i++)
                              {
                                 cur += bs.read12();
                                 if(cur >= 0)
                                    d[off] = cur / 2047.0f;
                                 else
                                    d[off] = cur / 2048.0f;
                                 // Dtrace("xxx d[%u] = %f (cur=%d)\n", off, d[off], cur);
                                 off++;
                              }
                              break;

                           case 2u: // TYPE=8bit delta values
                              for(sUI i = 0u; i < S16_FRAME_SZ; i++)
                              {
                                 cur += bs.read8();
                                 if(cur >= 0)
                                    d[off] = cur / 2047.0f;
                                 else
                                    d[off] = cur / 2048.0f;
                                 off++;
                              }
                              break;

                           case 3u: // TYPE=4bit delta values
                              for(sUI i = 0u; i < S16_FRAME_SZ; i++)
                              {
                                 cur += bs.read4();
                                 if(cur >= 0)
                                    d[off] = cur / 2047.0f;
                                 else
                                    d[off] = cur / 2048.0f;
                                 off++;
                              }
                              break;
                        }
                     }
                  } // loop numFrames
               } // if !b_procedural

               // Next sample waveform
               wfDat += smpDat->num_elements;
            }

            Dinfo("[+++] sr_proj_load: finished reading at io_offset=%u/%u. maxPoly=%u\n", ifs.io_offset, ifs.size, maxPoly);

#ifdef SR_SAVE_WF_DAT
            if(1)
            {
               FILE *fh = fopen("wf.dat", "wb");
               if(NULL != fh)
               {
                  fwrite((void*)proj.wf_dat, (wfTotal * sizeof(sF32)), 1, fh);
                  fclose(fh);
                  Dinfo("[...] wrote \"wf.dat\"\n");
               }
            }
#endif // SR_SAVE_WF_DAT

            r = YAC_TRUE;
         }
         else
         {
            Derror("[---] sr_proj_load: failed to allocate waveform data (%6.2fk)\n", (wfTotal*sizeof(sF32))/1024.0f);
         }
      }
      else
      {
         Derror("[---] sr_proj_load: invalid ver=%u\n", ver);
      }
   }
   else
   {
      Derror("[---] sr_proj_load: missing 'SYNG' header\n");
   }

   return r;
}

sBool sr_proj_load(SR_Project &proj, const char *_pathName) {
   sBool r = YAC_FALSE;

   FILE *fh = fopen(_pathName, "rb");
   if(NULL != fh)
   {
      SR_BufferStreamLE b;
      fseek(fh, 0, SEEK_END);
      b.size = (sUI)ftell(fh);
      fseek(fh, 0, SEEK_SET);
      Dinfo("[...] sr_proj_load: opened \"%s\", size=%u bytes\n", _pathName, b.size);
      b.buffer = new(std::nothrow) sU8[b.size];
      if(NULL != b.buffer)
      {
         b.io_offset = 0u;
         b.deleteme  = YAC_TRUE; // could skip this (never used)
         fread(b.buffer, 1, b.size, fh);
         r = sr_proj_load_buffer(b, proj);
         delete [] b.buffer;
      }
      else
      {
         Derror("[---] sr_proj_load: failed to allocate %u bytes\n", b.size);
      }

      fclose(fh);
   }
   else
   {
      Derror("[---] sr_proj_load: failed to open \"%s\" for reading\n", _pathName);
   }

   return r;
}

static sUI loc_sr_song_read_variable_length(SR_BufferStreamBE &ifs) {
   sUI ret = 0u;

   sU8 val1 = ifs.u8();
   sU8 val2 = 0u;
   sU8 val3 = 0u;
   sU8 val4 = 0u;
   sUI len = 1u;

   if(val1 & 128u)
   {
      val2 = ifs.u8();
      len++;

      if(val2 & 128u)
      {
         val3 = ifs.u8();
         len++;

         if(val3 & 128u)
         {
            val4 = ifs.u8();
            len++;
         }
      }
   }

   if(4u == len)
   {
      ret =
         ((val1 & 127u) << 21) |
         ((val2 & 127u) << 14) |
         ((val3 & 127u) << 7)  |
         ((val4 & 127u)     )  ;
   }
   else if(3u == len)
   {
      ret =
         ((val1 & 127u) << 14) |
         ((val2 & 127u) << 7)  |
         ((val3 & 127u)     )  ;
   }
   else if(2u == len)
   {
      ret =
         ((val1 & 127u) << 7)  |
         ((val2 & 127u)     )  ;
   }
   else
   {
      ret = val1;
   }

   return ret;
}

sBool sr_song_load_buffer(SR_BufferStreamBE &ifs, SR_Song &song) {
   sBool r = YAC_TRUE;

   sBool b = (ifs.u8() == 'M');
   b  = b && (ifs.u8() == 'T');
   b  = b && (ifs.u8() == 'h');
   b  = b && (ifs.u8() == 'd');
   if(b)
   {
      sU32 hdrSz     = ifs.u32();
      sU16 format    = ifs.u16();  // should be 1 (multi-track)
      sU16 numTracks = ifs.u16();
      sU16 ppq       = ifs.u16();
      // sF32 bpm = 120.0f;  // (todo) read/write meta event(s)
      sF32 bpm = 100.0f;  // (todo) read/write meta event(s)
      sF32 secPerTick = 60.0f / (ppq * bpm);
      song.bpm = bpm;
      song.ppq = ppq;
      song.frames_per_tick = SR_MIX_RATE * secPerTick;
      Dsong("[...] sr_song_load: hdrSz=%u format=%u #tracks=%u bpm=%3.2f ppq=%u frames/tick=%4.2f\n", hdrSz, format, numTracks, bpm, ppq, song.frames_per_tick);
      if(ppq > 0u)
      {
         ifs.seekRel(hdrSz - 6u);

         song.num_tracks = numTracks;
         song.num_ticks  = 0u;

         for(sUI trackIdx = 0u; trackIdx < numTracks; trackIdx++)
         {
            SR_SongTrack *track = &song.tracks[trackIdx];
            track->midi_port = trackIdx;  // vst_a..vst_p (0..15)

            sUI mtrkOffset = ifs.io_offset;
            b      = (ifs.u8() == 'M');
            b = b && (ifs.u8() == 'T');
            b = b && (ifs.u8() == 'r');
            b = b && (ifs.u8() == 'k');
            if(b)
            {
               sU32 trkSz = ifs.u32();
               sU32 trkEnd = ifs.io_offset + trkSz;

               Dsong("[...] sr_song_load: --------------------------------------------\n");
               Dsongtrace("[...] sr_song_load: scanning track %u/%u at io_offset=%u trkSz=%u => trkEnd=%u\n", (trackIdx+1u), numTracks, mtrkOffset, trkSz, trkEnd);
               // Count #events
               sUI numEv = 0u;
               sUI absTime = 0u;
               sU8 runningStatus = 0u;
               while(ifs.io_offset < trkEnd)
               {
                  sUI evOff = ifs.io_offset;
                  sU32 evDeltaTime = loc_sr_song_read_variable_length(ifs);
                  absTime += evDeltaTime;

                  sU8 evHdr = ifs.u8();

                  Dsongtrace("[dbg] sr_song_load: [%u:%8u] scan evHdr=0x%2x deltaTime=%u io_offset=%u\n", trackIdx, absTime, evHdr, evDeltaTime, evOff);

                  if(255u == evHdr)
                  {
                     // meta_event = $FF + <meta_type> + <v_length> + <event_data_bytes>
                     //
                     //    0x00  Sequence number          0x20  MIDI channel prefix assignment
                     //    0x01  Text event               0x2F  End of track (*non optional*)
                     //    0x02  Copyright notice         0x51  Tempo setting
                     //    0x03  Sequence or track name   0x54  SMPTE offset
                     //    0x04  Instrument name          0x58  Time signature
                     //    0x05  Lyric text               0x59  Key signature
                     //    0x06  Marker text              0x7F  Sequencer specific event
                     //    0x07  Cue point
                     //
                     // (todo) FF 51 03 tttttt Set Tempo (in microseconds per MIDI quarter-note)
                     sU8 metaType = ifs.u8();
                     sUI metaSz = loc_sr_song_read_variable_length(ifs);
                     Dsong("[dbg] sr_song_load: [%u:%8u] meta type=%u sz=%u\n", trackIdx, absTime, metaType, metaSz);
                     ifs.seekRel(metaSz);
                  }
                  else if(0xF0u == evHdr)
                  {
                     // $F0..$F7
                     Derror("[~~~] sr_song_load: [%u:%8u] skipping SysEx event at io_offset=%u\n", trackIdx, absTime, ifs.io_offset - 1u);
                     while(ifs.io_offset < trkEnd)
                     {
                        sU8 eox = ifs.u8();
                        if(0xF7u == eox)
                           break;
                     }
                  }
#if 0
                  else if(0xF1u == evHdr)
                  {
                     // MIDI quarter frame time code (=> skip)
                  }
                  else if(0xF2u == evHdr)
                  {
                     // MIDI Song Position Pointer (=> skip)
                     ifs.seekRel(3);
                  }
                  else if(0xF3u == evHdr)
                  {
                     // MIDI Song Select (=> skip)
                     ifs.seekRel(2);
                  }
                  else if(0xF6u == evHdr)
                  {
                     // MIDI Tune Request (=> skip)
                     ifs.seekRel(1);
                  }
#endif
                  else
                  {
                     numEv++;

                     // Regular MIDI event
                     sU8 evData1;

                     if(evHdr & 128u)
                     {
                        runningStatus = evHdr;
                        evData1 = ifs.u8();
                     }
                     else
                     {
                        evData1 = evHdr;
                        evHdr = runningStatus;
                     }

                     sU8 evData2;
                     switch(evHdr & 0xF0u)
                     {
                        default:
                           Derror("[---] sr_song_load: [%u:%8u] unexpected evHdr=0x%2x near io_offset=%u\n", trackIdx, absTime, evHdr, ifs.io_offset - 1u);
                           r = YAC_FALSE;
                           break;

                        case 0x80u:
                        case 0x90u:
                        case 0xA0u:
                        case 0xB0u:
                        case 0xE0u:
                           evData2 = ifs.u8(); // Velocity, Poly Pressure Val, CC Val, Pitchbend MSB
                           break;

                        case 0xC0u:
                        case 0xD0u:
                           // Program, Channel Pressure
                           break;
                     }
                     (void)evData1;
                     (void)evData2;
                  }
               } // loop until end of track data

               Dsong("[...] sr_song_load: track %u/%u has %u events. absTime=%u (io_offset=%u)\n", trackIdx+1u, numTracks, numEv, absTime, ifs.io_offset);
               track->num_ticks = absTime;
               track->num_events = numEv;
               song.num_ticks = (absTime > song.num_ticks) ? absTime : song.num_ticks;

               if(numEv > 0u)
               {
                  track->events = new SR_Event[numEv];
                  if(NULL != track->events)
                  {
                     SR_Event *ev = track->events;

                     absTime = 0u;
                     runningStatus = 0u;
                     ifs.seekAbs(mtrkOffset + 8u);

                     Dsong("[...] sr_song_load: loading track %u/%u at io_offset=%u trkSz=%u => trkEnd=%u\n", (trackIdx+1u), numTracks, ifs.io_offset - 8u, trkSz, trkEnd);

                     while(ifs.io_offset < trkEnd)
                     {
                        sUI evOff = ifs.io_offset;
                        sU32 evDeltaTime = loc_sr_song_read_variable_length(ifs);
                        absTime += evDeltaTime;

                        sU8 evHdr = ifs.u8();

                        Dsongtrace("[dbg] sr_song_load: [%u:%8u] load evHdr=0x%2x deltaTime=%u io_offset=%u\n", trackIdx, absTime, evHdr, evDeltaTime, evOff);

                        if(255u == evHdr)
                        {
                           // End Of Track (or other meta data)
                           sU8 metaType = ifs.u8();
                           sUI metaSz = loc_sr_song_read_variable_length(ifs);
                           Dsongtrace("[dbg] sr_song_load: [%u:%8u] meta type=%u sz=%u\n", trackIdx, absTime, metaType, metaSz);
                           ifs.seekRel(metaSz);
                        }
                        else if(0xF0u == evHdr)
                        {
                           // $F0..$F7
                           Dsongtrace("[~~~] sr_song_load: [%u:%8u] skipping SysEx event at io_offset=%u\n", trackIdx, absTime, ifs.io_offset - 1u);
                           while(ifs.io_offset < trkEnd)
                           {
                              sU8 eox = ifs.u8();
                              if(0xF7u == eox)
                                 break;
                           }
                        }
#if 0
                        else if(0xF1u == evHdr)
                        {
                           // MIDI quarter frame time code (=> skip)
                        }
                        else if(0xF2u == evHdr)
                        {
                           // MIDI Song Position Pointer (=> skip)
                           ifs.seekRel(3);
                        }
                        else if(0xF3u == evHdr)
                        {
                           // MIDI Song Select (=> skip)
                           ifs.seekRel(2);
                        }
                        else if(0xF6u == evHdr)
                        {
                           // MIDI Tune Request (=> skip)
                           ifs.seekRel(1);
                        }
#endif
                        else
                        {
                           // Regular MIDI event
                           sU8 evData1;

                           if(evHdr & 128u)
                           {
                              runningStatus = evHdr;
                              evData1 = ifs.u8();
                           }
                           else
                           {
                              evData1 = evHdr;
                              evHdr = runningStatus;
                           }

                           sU8 evData2 = 0u;
                           switch(evHdr & 0xF0u)
                           {
                              default:
                                 Derror("[---] sr_song_load: [%u:%8u] unexpected evHdr=0x%2x near io_offset=%u\n", trackIdx, absTime, evHdr, ifs.io_offset - 1u);
                                 r = YAC_FALSE;
                                 break;

                              case 0x80u:
                              case 0x90u:
                              case 0xA0u:
                              case 0xB0u:
                              case 0xE0u:
                                 evData2 = ifs.u8(); // Velocity, Poly Pressure Val, CC Val, Pitchbend LSB
                                 if(0x90u == (evHdr & 0xF0u))
                                 {
                                    Dsongtrace("[trc] sr_song_load: [%u:%8u] note-on ch=%u note=%u vel=%u\n", trackIdx, absTime, evHdr&0x0F, evData1, evData2);
                                 }
                                 break;

                              case 0xC0u:
                              case 0xD0u:
                                 // Program, Channel Pressure
                                 break;
                           }

                           ev->abs_time = absTime;
                           ev->data.bytes[0] = evHdr;
                           ev->data.bytes[1] = evData1;
                           ev->data.bytes[2] = evData2;
                           ev->data.bytes[3] = 0u;

                           // Next event
                           ev++;
                        }

                     } // loop until end of track data

                     if(r)
                     {
                        Dinfo("[...] sr_song_load: track %u/%u: finished reading %u/%u track events (io_offset=%u)\n", (trackIdx+1u), numTracks, (sUI)(size_t)(ev-track->events), numEv, ifs.io_offset);
                     }
                  }
                  else
                  {
                     Derror("[---] sr_song_load: failed to allocate %u events\n", numEv);
                     r = YAC_FALSE;
                  }
               } // if numEv > 0u
               else
               {
                  Dinfo("[dbg] sr_song_load: track %u/%u has no (regular) events, skipping..\n", trackIdx+1u, numTracks);
               }
            }
            else
            {
               Derror("[---] sr_song_load: expected 'MTrk' header at io_offset=%u\n", mtrkOffset);
               r = YAC_FALSE;
               break;
            }
         }

         if(r)
         {
            Dinfo("[+++] sr_song_load: finished reading %u track(s) (io_offset=%u)\n", numTracks, ifs.io_offset);
         }
      }
      else
      {
         Derror("[---] sr_song_load: SMPTE division format is not supported (ppq = 0)\n");
         r = YAC_FALSE;
      }
   }
   else
   {
      Derror("[---] sr_song_load: missing 'MThd' header\n");
      r = YAC_FALSE;
   }
   return r;
}

sBool sr_song_load(SR_Song &song, const char *_pathName) {
   sBool r = YAC_FALSE;

   FILE *fh = fopen(_pathName, "rb");
   if(NULL != fh)
   {
      SR_BufferStreamBE b;
      fseek(fh, 0, SEEK_END);
      b.size = (sUI)ftell(fh);
      fseek(fh, 0, SEEK_SET);
      Dinfo("[...] sr_song_load: opened \"%s\", size=%u bytes\n", _pathName, b.size);
      b.buffer = new(std::nothrow) sU8[b.size];
      if(NULL != b.buffer)
      {
         b.io_offset = 0u;
         b.deleteme  = YAC_TRUE; // can skip this (never used)
         fread(b.buffer, 1, b.size, fh);
         r = sr_song_load_buffer(b, song);
         delete [] b.buffer;
      }
      else
      {
         Derror("[---] sr_song_load: failed to allocate %u bytes\n", b.size);
      }

      fclose(fh);
   }
   else
   {
      Derror("[---] sr_song_load: failed to open \"%s\" for reading\n", _pathName);
   }

   return r;
}

#ifdef SR_PORTAUDIO
struct replay_state_t {
   SR_Project *proj;
   SR_Song    *song;
   // // YAC_FloatArray mix_buf;
};
static replay_state_t replay_state;

// <function.png>
static int stream_callback(const void *input, void *output,
                           unsigned long frameCount,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData
                           )
{
   // static sUI preSilenceCount = 2048u;  // workaround for macOS "first three seconds after stream start lost" issue
   static sUI preSilenceCount = 0u;
   (void) timeInfo;
   (void) statusFlags;

   if(0u == preSilenceCount)
   {
      replay_state_t *st = (replay_state_t*) userData;
      YAC_FloatArray mixBuf;
      mixBuf.elements = (sF32*)output;
      mixBuf.own_data = YAC_FALSE;
      mixBuf.num_elements = mixBuf.max_elements = frameCount * 2u;
      memset((void*)mixBuf.elements, 0, mixBuf.num_elements * sizeof(sF32));
      // printf("xxx frameCount=%u\n", frameCount);
      st->song->process(*st->proj, mixBuf);
   }
   else
   {
      preSilenceCount--;
      sF32 *d = (sF32*)output;
      for(sUI frameIdx = 0u; frameIdx < frameCount; frameIdx++)
      {
         *d++ = 0.0f;
         *d++ = 0.0f;
      }
   }
   return paContinue;
}
#endif // SR_PORTAUDIO

// <function.png>
int main(int argc, char**argv) {
   int r = 10;

#ifdef SR_PROFILE
   loc_profile_ms_init();
#endif // SR_PROFILE

   sr_init();

#ifdef SR_FX
   sr_register_plugin(&amp_init);
   sr_register_plugin(&biquad_lpf_1_init);
   sr_register_plugin(&biquad_lpf_2_init);
   sr_register_plugin(&biquad_hpf_1_init);
   sr_register_plugin(&biquad_hpf_2_init);
   sr_register_plugin(&biquad_lsh_1_init);
   sr_register_plugin(&biquad_lsh_2_init);
   sr_register_plugin(&biquad_hsh_1_init);
   sr_register_plugin(&biquad_hsh_2_init);
   sr_register_plugin(&dly_2_init);
   sr_register_plugin(&dly_flt_2_mod_init);
   sr_register_plugin(&eq3_init);
   sr_register_plugin(&gain_init);
   sr_register_plugin(&resample_linear_init);
   sr_register_plugin(&ws_tanh_asym_init);
   sr_register_plugin(&ws_fm_init);
   sr_register_plugin(&schroederverb_init);
   sr_register_plugin(&valley_plateau_init);
   sr_register_plugin(&cycle_rms_comp_stereo_1_init);
   sr_register_plugin(&chorus_tri_1_init);
   sr_register_plugin(&gm_rng_convolve_init);
   sr_register_plugin(&np_svf_init);
   fm_stack_init_common();
   sr_register_plugin(&fm_stack_init_loresh);
   sr_register_plugin(&fm_stack_init_medres);
   sr_register_plugin(&fm_stack_init_medresh);
   sr_register_plugin(&fm_stack_init_hires);
#endif // SR_FX

   SR_Project proj;
   // sBool bOk = sr_proj_load(proj, "../tks-projects/eureka/autogen_cycle/autogen_debug2.syn");
   // sBool bOk = sr_proj_load(proj, "../tks-projects/eureka/autogen_cycle/autogen_sr_melodic_demo-18Apr2023.syn");
   // sBool bOk = sr_proj_load(proj, "../tks-projects/eureka/autogen_cycle/autogen_sr_melodic_demo-18Apr2023b.syn");
   // sBool bOk = sr_proj_load(proj, "../tks-projects/eureka/autogen_cycle/autogen_" SONGNAME ".syn");
   sBool bOk = sr_proj_load(proj, "music/autogen_" SONGNAME ".syn");

   if(bOk)
   {
      SR_Song song;
      // bOk = sr_song_load(song, "../tks-projects/flux/export.mid");
      // bOk = sr_song_load(song, "../tks-projects/flux/" SONGNAME ".mid");
      bOk = sr_song_load(song, "music/" SONGNAME ".mid");

      if(bOk)
      {
         song.start();
         proj.setTempo(song.bpm, song.ppq);

         bOk = proj.allocTrackMixBuffers();
         if(!bOk)
            return 20;

#ifdef SR_PORTAUDIO
         if(1 == argc)
         {
            Pa_Initialize();

            PaStreamParameters isp;
            memset((void*)&isp, 0, sizeof(PaStreamParameters));
            // isp.device = 0;
            // isp.channelCount = 0;
            // isp.sampleFormat = paFloat32;
            // isp.suggestedLatency = 0.1f;

            PaStreamParameters osp;
            memset((void*)&osp, 0, sizeof(PaStreamParameters));
#ifdef SR_PORTAUDIO_DEVIDX
            osp.device = (PaDeviceIndex) SR_PORTAUDIO_DEVIDX/*YC-307*/;
#else
            osp.device = (PaDeviceIndex) Pa_GetDefaultOutputDevice();
#endif // SR_PORTAUDIO_DEVIDX
            osp.channelCount = 2;
            osp.sampleFormat = paFloat32;
            osp.suggestedLatency = 0.1f;

            replay_state.proj = &proj;
            replay_state.song = &song;
            // replay_state.mix_buf.alloc(4096 * 2u);

            PaStream *stream;
            PaError err = Pa_OpenStream(&stream,
                                        NULL,//&isp,
                                        &osp,
                                        (double)SR_MIX_RATE,
                                        SR_MAX_FRAMES_PER_CALL/*framesPerBuffer*/,// 0 == paFramesPerBufferUnspecified
                                        0, // StreamFlags
                                        stream_callback,
                                        (void*)&replay_state // User data
                                        );

            if(paNoError == err)
            {
               Dinfo("[...] starting stream. mix_rate=%5.1f, fmt=f32\n", SR_MIX_RATE);
               Pa_StartStream(stream);

               for(;;)
               {
                  usleep(1000);
               }

               Pa_CloseStream(stream);
            }
            else
            {
               Derror("[---] Pa_OpenStream() failed (err=%d)\n", err);
            }
            return 0;
         }
#endif // SR_PORTAUDIO

#ifndef SR_RENDER_TEST
         if(0)
#endif // SR_RENDER_TEST
         {
#ifdef SR_PROFILE
            sUI tStart = loc_profile_ms_get();
#endif // SR_PROFILE

#ifdef SR_SAVE_SONG_WF_DAT
            FILE *fh = fopen("song_wf.dat", "wb");
#endif // SR_SAVE_SONG_WF_DAT

            sUI frameSz = SR_MAX_FRAMES_PER_CALL;
            YAC_FloatArray mixBuf;
            mixBuf.alloc(frameSz * 2u);
            sUI totalNumFrames = sUI(song.frames_per_tick * song.num_ticks + 0.5f);
            Dinfo("[...] render rate=%5.1f num_ticks=%u frames_per_tick=%f => totalNumFrames=%u\n", SR_MIX_RATE, song.num_ticks, song.frames_per_tick, totalNumFrames);
            for(sUI frameOff = 0u; frameOff < totalNumFrames; frameOff += frameSz)
            {
               Dreplay2("[trc] ............ process frame off=%u sz=%u ne=%u\n", frameOff, frameSz, mixBuf.num_elements);
               memset((void*)mixBuf.elements, 0, mixBuf.num_elements * sizeof(sF32));
               song.process(proj, mixBuf);

#ifdef SR_SAVE_SONG_WF_DAT
               if(NULL != fh)
               {
                  fwrite((void*)mixBuf.elements, mixBuf.num_elements * sizeof(sF32), 1, fh);
               }
#endif // SR_SAVE_SONG_WF_DAT
            }

#ifdef SR_SAVE_SONG_WF_DAT
            if(NULL != fh)
            {
               fclose(fh);
               Dinfo("[...] wrote \"song_wf.dat\"\n");
            }
#endif // SR_SAVE_SONG_WF_DAT

#ifdef SR_PROFILE
            sUI tDelta = loc_profile_ms_get() - tStart;
            sF32 load = ((float(tDelta / 1000.0f) * SR_MIX_RATE) / float(totalNumFrames)) * 100.0f;
            Dprintf("[pro] song render tDelta=%u ms => load=%3.2f%%\n", tDelta, load);
#endif // SR_PROFILE
         }

         r = 0;
      }
      else
      {
         Derror("[---] sr_song_load() failed\n");
      }
   }
   else
   {
      Derror("[---] sr_proj_load() failed\n");
   }

#ifdef SR_FX
   proj.freePlugins();
   sr_unload_plugins();
#endif // SR_FX

   Dprintf("exiting..\n");

   return r;
}
