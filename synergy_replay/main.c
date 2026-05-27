// ----
// ---- file   : main.c
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
// ----              % m clean
// ----              % m STFX_SKIP_MAIN_INIT=1 bin
// ----           - .mid must be exported in multi-track, vst-dev-per-track format
// ----              ('mixed' Multi-Track checkbox state in Synergy ExportSMFDialog)
// ----
// ---- created: 12Apr2023
// ---- changed: 13Apr2023, 14Apr2023, 15Apr2023, 20Apr2023, 21Apr2023, 22Apr2023, 23Apr2023
// ----          11Aug2023, 08Sep2023, 19Sep2023, 22Sep2023, 18Nov2023, 03Oct2024, 07Dec2024
// ----          04Jan2025, 09Jan2026, 10Apr2026, 09May2026, 15May2026, 17May2026, 18May2026
// ----          19May2026, 20May2026, 22May2026, 26May2026
// ----
// ----
// ----

#include <stdlib.h>  // malloc,free
#include <stdio.h>   // printf
#include <string.h>  // strcmp
#include <math.h>    // sinf

#ifdef SR_PORTAUDIO
#include <portaudio.h>
#include <unistd.h>
#endif // SR_PORTAUDIO

#include "sr.h"

// ------------------------------------ config
#define SONGNAME "demo_2-r-sr"

extern void cycle_calc_waveform_demo_2_r_sr (float *_wfAddr);

// when defined, add code to render song to mem buffer (+optionally write to file)
//  (note) if SR_SAVE_SONG_WF_DAT is not defined, no file will be written
#define SR_OUT_FILE  "song_wf.dat"
#define SR_BATCH_RENDER  defined    // enabled at runtime by passing any cmdline arg


// ------------------------------------ Cycle: common arrays
float cycle_sine_tbl_f[16384];
short cycle_sine_tbl_i[16384];
float cycle_xfade_buf[4096];

static void loc_cycle_calc_sine_tbl_f(void) {
   unsigned int i = 0u;
   for(i = 0u; i < 16384u; i++)
   {
      float a = (i*3.14159265359f*2.0f)/16384.0f;
      cycle_sine_tbl_f[i] = sinf(a);
   }
}

static void loc_cycle_calc_sine_tbl_i(void) {
   unsigned int i = 0u;
   for(i = 0u; i < 16384u; i++)
   {
      float a = (i*3.14159265359f*2.0f)/16384.0f;
      cycle_sine_tbl_i[i] = (short)(sinf(a) * 2048.0f/*FX_ONE*/);
   }
}

static void loc_cycle_init(void) {
   loc_cycle_calc_sine_tbl_f();
   loc_cycle_calc_sine_tbl_i();
}

// ------------------------------------ STFX voice plugins
extern st_plugin_info_t *amp_init (void);
extern st_plugin_info_t *biquad_lpf_1_init (void);
extern st_plugin_info_t *biquad_lpf_2_init (void);
extern st_plugin_info_t *biquad_hpf_1_init (void);
extern st_plugin_info_t *biquad_hpf_2_init (void);
extern st_plugin_info_t *biquad_lsh_1_init (void);
extern st_plugin_info_t *biquad_lsh_2_init (void);
extern st_plugin_info_t *biquad_hsh_1_init (void);
extern st_plugin_info_t *biquad_hsh_2_init (void);
extern st_plugin_info_t *chorus_tri_1_init (void);
extern st_plugin_info_t *cycle_rms_comp_stereo_1_init (void);
extern st_plugin_info_t *dly_2_init (void);
extern st_plugin_info_t *dly_flt_2_mod_init (void);
extern st_plugin_info_t *eq3_init (void);
extern st_plugin_info_t *gain_init (void);
// extern st_plugin_info_t *gm_rng_convolve_init (void);
extern st_plugin_info_t *resample_linear_init (void);
extern st_plugin_info_t *schroederverb_init (void);
extern st_plugin_info_t *valley_plateau_init (void);
extern st_plugin_info_t *ws_tanh_asym_init (void);
extern st_plugin_info_t *ws_fm_init (void);
// extern st_plugin_info_t *noiseplethora_ab_init (void);
// extern st_plugin_info_t *noiseplethora_cwhite_init (void);
// extern st_plugin_info_t *noiseplethora_cgrit_init (void);
extern st_plugin_info_t *np_svf_init (void);

extern void fm_stack_init_common (void);
extern st_plugin_info_t *fm_stack_init_loresh (void);
extern st_plugin_info_t *fm_stack_init_medres (void);
extern st_plugin_info_t *fm_stack_init_medresh (void);
extern st_plugin_info_t *fm_stack_init_hires (void);

// ------------------------------------ profiling
#ifdef SR_PROFILE
#include <sys/time.h>
static struct timeval sr_profile_tv_start;
static void loc_profile_ms_init(void) {
   gettimeofday(&sr_profile_tv_start, 0);
}
static unsigned int loc_profile_ms_get(void) {
   struct timeval c; gettimeofday(&c, 0);
   struct timeval *s = &sr_profile_tv_start;
   return (unsigned int) ( (c.tv_sec-s->tv_sec)*1000 + (c.tv_usec-s->tv_usec)/1000 );
}
#endif // SR_PROFILE

// ------------------------------------ replay_state
#ifdef SR_PORTAUDIO
typedef struct replay_state_s {
   sr_proj_t proj;
   sr_song_t song;
} replay_state_t;
static replay_state_t replay_state;

// ------------------------------------ stream_callback
static int stream_callback(const void *input, void *output,
                           unsigned long frameCount,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData
                           ) {
   // static unsigned int preSilenceCount = 2048u;  // workaround for macOS "first three seconds after stream start lost" issue
   static unsigned int preSilenceCount = 0u;
   (void) timeInfo;
   (void) statusFlags;

   if(0u == preSilenceCount)
   {
      replay_state_t *st = (replay_state_t*) userData;
      // printf("xxx frameCount=%u\n", frameCount);
      sr_process(st->proj, st->song, (float*)output, (unsigned int)frameCount, 1/*bClearMixBuf*/);
   }
   else
   {
      preSilenceCount--;
      float *d = (float*)output;
      for(unsigned int frameIdx = 0u; frameIdx < (unsigned int)frameCount; frameIdx++)
      {
         *d++ = 0.0f;
         *d++ = 0.0f;
      }
   }
   return paContinue;
}
#endif // SR_PORTAUDIO


// ------------------------------------ loc_open_audio_device
#ifdef SR_PORTAUDIO
static PaStream *loc_pa_stream;

static int loc_open_audio_device(void) {
   Pa_Initialize();
   Dtrace("[trc] PA_Initialize() ok\n");

   // (note) no inputs used
   PaStreamParameters isp;
   memset((void*)&isp, 0, sizeof(PaStreamParameters));
   // isp.device = 0;
   // isp.channelCount = 0;
   // isp.sampleFormat = paFloat32;
   // isp.suggestedLatency = 0.1f;

   PaStreamParameters osp;
   memset((void*)&osp, 0, sizeof(PaStreamParameters));
#ifdef SR_PORTAUDIO_DEVIDX
   osp.device = (PaDeviceIndex) SR_PORTAUDIO_DEVIDX;
#else
   osp.device = (PaDeviceIndex) Pa_GetDefaultOutputDevice();
#endif // SR_PORTAUDIO_DEVIDX
   osp.channelCount = 2;
   osp.sampleFormat = paFloat32;
   osp.suggestedLatency = 0.1f;

   PaError err = Pa_OpenStream(&loc_pa_stream,
                               NULL,//&isp,
                               &osp,
                               (double)SR_MIX_RATE,
                               SR_MAX_FRAMES_PER_BLOCK/*framesPerBuffer=64*/,// 0 == paFramesPerBufferUnspecified
                               0, // StreamFlags
                               stream_callback,
                               (void*)&replay_state // User data
                               );

   if(paNoError == err)
   {
      // Succeeded
      return 1;
   }
   else
   {
      Derror("[---] Pa_OpenStream() failed (err=%d)\n", err);
   }
   return 0;
}

// ------------------------------------ loc_start_audio_device
static void loc_start_audio_device(void) {
   Dprintf("[...] main: starting stream. mix_rate=%5.1f, fmt=f32\n", SR_MIX_RATE);
   Pa_StartStream(loc_pa_stream);
}

// ------------------------------------ loc_close_audio_device
static void loc_close_audio_device(void) {
   Pa_CloseStream(loc_pa_stream);
}

#endif // SR_PORTAUDIO

// ------------------------------------ loc_batch_render
static void loc_batch_render(sr_proj_t proj, sr_song_t song) {
#ifdef SR_PROFILE
   unsigned int tStart = loc_profile_ms_get();
#endif // SR_PROFILE

#ifdef SR_SAVE_SONG_WF_DAT
   FILE *fh = fopen(SR_OUT_FILE, "wb");
#endif // SR_SAVE_SONG_WF_DAT

   const unsigned int frameSz = SR_MAX_FRAMES_PER_BLOCK/*64*/;
   float *mixBuf = (float*)malloc(frameSz * 2u * sizeof(float));
   unsigned int totalNumFrames = (unsigned int)
      (sr_song_get_frames_per_tick(song)
       * sr_song_get_num_ticks(song)
       + 0.5f
       );
   Dprintf("[...] render rate=%5.1f num_ticks=%u frames_per_tick=%f => totalNumFrames=%u\n", sr_get_mix_rate(), sr_song_get_num_ticks(song), sr_song_get_frames_per_tick(song), totalNumFrames);
   for(unsigned int frameOff = 0u; frameOff < totalNumFrames; frameOff += frameSz)
   {
      Dreplay2("[trc] ............ process frame off=%u sz=%u\n", frameOff, frameSz);
      sr_process(proj, song, mixBuf, frameSz, 1/*bClearMixBuf*/);

#ifdef SR_SAVE_SONG_WF_DAT
      if(NULL != fh)
      {
         fwrite((void*)mixBuf, frameSz * 2u * sizeof(float), 1, fh);
      }
#endif // SR_SAVE_SONG_WF_DAT
   }

#ifdef SR_SAVE_SONG_WF_DAT
   if(NULL != fh)
   {
      fclose(fh);
      Dprintf("[...] wrote \"" SR_OUT_FILE "\"\n");
   }
#endif // SR_SAVE_SONG_WF_DAT

   free((void*)mixBuf);

#ifdef SR_PROFILE
   unsigned int tDelta = loc_profile_ms_get() - tStart;
   float load = ((((float)(tDelta / 1000.0f)) * SR_MIX_RATE) / ((float)totalNumFrames)) * 100.0f;
   Dprintf("[pro] song render tDelta=%u ms => load=%3.2f%%\n", tDelta, load);
#endif // SR_PROFILE
}

// ------------------------------------ loc_register_plugins
#ifdef SR_FX
static void loc_register_plugins(void) {
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
   // sr_register_plugin(&gm_rng_convolve_init);
   sr_register_plugin(&np_svf_init);
   fm_stack_init_common();
   sr_register_plugin(&fm_stack_init_loresh);
   sr_register_plugin(&fm_stack_init_medres);
   sr_register_plugin(&fm_stack_init_medresh);
   sr_register_plugin(&fm_stack_init_hires);
}
#endif // SR_FX

// ------------------------------------ main
int main(int argc, char**argv) {
   int r = 10;

#ifdef SR_PROFILE
   loc_profile_ms_init();
#endif // SR_PROFILE

   loc_cycle_init();

   sr_init();
   sr_set_mix_rate(SR_MIX_RATE);
   sr_set_volume(SR_VOLUME);

#ifdef SR_FX
   loc_register_plugins();
#endif // SR_FX

   sr_proj_t proj = sr_proj_new();
   sr_song_t song = sr_song_new();

   // Load instruments / samples / tracks
   sr_bool_t bOk = sr_proj_load_file(proj, "music/autogen_" SONGNAME ".syn");

   if(bOk)
   {
      // Load MIDI stream(s)
      bOk = sr_song_load_file(song, "music/" SONGNAME ".mid");

      if(bOk)
      {
         sr_song_restart(song);

         // (note) first MIDI (meta) event sets actual initial tempo
         sr_proj_set_tempo(proj,
                           sr_song_get_bpm(song),
                           sr_song_get_ppq(song)
                           );

         bOk = sr_proj_alloc_mix_buffers(proj);
         if(!bOk)
            return 20;

#if 1
         // Calc procedural samples
         {
            Dinfov("[...] synthesizing procedural samples: begin\n");
            unsigned int wfSz = sr_proj_get_wf_size(proj) * sizeof(float);
            unsigned int t = loc_profile_ms_get();
            sr_calc_cycle_waveforms(proj, song, &cycle_calc_waveform_demo_2_r_sr);
            t = loc_profile_ms_get() - t;
            Dinfo("[...] cycle: synthesized %4.2fk samples in %u ms\n", (wfSz / 1024.0f), t);
         }
#endif

#ifdef SR_PORTAUDIO
         // Render (soft) realtime audio
         if(1 == argc)
         {
            replay_state.proj = proj;
            replay_state.song = song;

            if(loc_open_audio_device())
            {
               loc_start_audio_device();
               for(;;)
               {
                  // (note) PortAudio calls stream_callback() repeatedly
                  usleep(1000);
               }
               loc_close_audio_device();
            }
         }
#endif // SR_PORTAUDIO

#ifndef SR_BATCH_RENDER
         if(0)
#else
         if(argc >= 2)
#endif // SR_BATCH_RENDER
         {
            loc_batch_render(proj, song);
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

   sr_song_delete(song);
   sr_proj_delete(proj);
   sr_exit();

   Dprintf("exiting..\n");

   return r;
}
