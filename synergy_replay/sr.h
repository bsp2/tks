// ----
// ---- file   : sr.h
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
// ---- info   : Standalone Synergy Replay
// ----
// ---- created: 12Apr2023
// ---- changed: 13Apr2023, 14Apr2023, 15Apr2023, 20Apr2023, 21Apr2023, 22Apr2023, 23Apr2023
// ----          11Aug2023, 08Sep2023, 19Sep2023, 22Sep2023, 18Nov2023, 03Oct2024, 07Dec2024
// ----          04Jan2025, 09Jan2026, 10Apr2026, 09May2026, 15May2026, 17May2026, 18May2026
// ----          19May2026, 20May2026, 22May2026, 24May2026, 26May2026
// ----
// ----
// ----

#pragma once

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

// ---------------------------------------- limits
// #undef SR_TRACK_FX
// #undef SR_VOICE_FX

#define SR_MAX_TRACKS          48
#define SR_MAX_SAMPLES        128
#define SR_MAX_PLUGIN_DEFS     32
#define SR_MAX_TRACK_OUTPUTS    8
#define SR_MAX_TRACK_PLUGINS    4

// ---------------------------------------- default sample rate and output volume
#ifndef SR_MIX_RATE
#define SR_MIX_RATE     48000.0f
#endif // SR_MIX_RATE

#ifndef SR_VOLUME
// (note) default=0.94 => ~-0.54dB headroom
#define SR_VOLUME       0.94f
#endif // SR_VOLUME

// ---------------------------------------- log output
#ifdef SR_STDIO
#include <stdio.h>
extern void sr_printf(const char *_fmt, ...);
#define Dprintf  if(0) ; else sr_printf
#define Derror   if(0) ; else sr_printf
#define Dsuccess if(0) ; else sr_printf
#ifdef SR_INFO
#define Dinfo    if(0) ; else sr_printf
#else
#define Dinfo    if(1) ; else sr_printf
#endif // SR_INFO
#ifdef SR_INFOV
#define Dinfov   if(0) ; else sr_printf
#else
#define Dinfov   if(1) ; else sr_printf
#endif // SR_INFO
#define Dsong    if(1) ; else sr_printf  // SMF/.mid loader debugging

// verbose .syn loader debugging:
#ifdef SR_TRACE
#define Dtrace       if(0) ; else sr_printf
#define Ddtor        if(0) ; else sr_printf
#else
#define Dtrace       if(1) ; else sr_printf
#define Ddtor        if(1) ; else sr_printf
#endif // SR_TRACE

// verbose SMF/.mid loader debugging:
#ifdef SR_SONGTRACE
#define Dsongtrace   if(0) ; else sr_printf
#else
#define Dsongtrace   if(1) ; else sr_printf
#endif // SR_SONGTRACE

// verbose replayer debugging:
#ifdef SR_REPLAYTRACE
#define Dreplay     if(0) ; else sr_printf
#define Dreplay2    if(1) ; else sr_printf
#else
#define Dreplay     if(1) ; else sr_printf
#define Dreplay2    if(1) ; else sr_printf
#endif // SR_REPLAYTRACE

#else
#define Dprintf     if(1) ; else sr_printf
#define Derror      if(1) ; else sr_printf
#define Dinfo       if(1) ; else sr_printf
#define Dinfov      if(1) ; else sr_printf
#define Dsuccess    if(1) ; else sr_printf
#define Dtrace      if(1) ; else sr_printf
#define Dsongtrace  if(1) ; else sr_printf
#define Dsong       if(1) ; else sr_printf
#define Dreplay     if(1) ; else sr_printf
#endif // SR_STDIO

// ---------------------------------------- private
// (note) must be 64
#define SR_MAX_FRAMES_PER_BLOCK   64u
// see tksampler/plugin.h (STFX plugin API)
typedef struct st_plugin_info_s st_plugin_info_t;
#define ST_PLUGIN_API
typedef st_plugin_info_t *(*ST_PLUGIN_API st_plugin_single_init_fxn_t) (void);

// see eureka/autogen_cycle/autogen_calc_*-sr_float.c (written by "Export SYN")
typedef void (*sr_cycle_calc_waveform_fxn_t) (float *_wfAddr);

typedef void *sr_proj_t;
typedef void *sr_song_t;
typedef int sr_bool_t;

#if defined(SR_TRACK_FX) || defined(SR_VOICE_FX)
#define SR_FX  defined
#endif

// ---------------------------------------- public interface
void sr_init (void);
void sr_set_mix_rate (float _mixRate);
float sr_get_mix_rate (void);
void sr_set_volume (float _volume);
float sr_get_volume (void);
void sr_exit (void);
sr_bool_t sr_register_plugin (st_plugin_single_init_fxn_t _initFxn);
sr_proj_t sr_proj_new (void);
sr_song_t sr_song_new (void);
sr_bool_t sr_proj_load_file (sr_proj_t _proj, const char *_pathName);
sr_bool_t sr_proj_load_buffer (sr_proj_t _proj,
                               const unsigned char *_bufPtr,
                               unsigned int         _bufSize
                               );
float *sr_proj_get_wf_dat (sr_proj_t _proj);
unsigned int sr_proj_get_wf_size (sr_proj_t _proj);
sr_bool_t sr_song_load_file (sr_song_t _song, const char *_pathName);
sr_bool_t sr_song_load_buffer (sr_song_t _song,
                               const unsigned char *_bufPtr,
                               unsigned int         _bufSize
                               );
unsigned int sr_song_get_total_num_ev (sr_song_t _song);  // debug
float sr_song_get_bpm (sr_song_t _song);
unsigned int sr_song_get_ppq (sr_song_t _song);
void sr_proj_set_tempo (sr_proj_t    _proj,
                        float        _bpm,
                        unsigned int _ppq
                        );
float sr_song_get_frames_per_tick (sr_song_t song);
unsigned int sr_song_get_num_ticks (sr_song_t song);
sr_bool_t sr_proj_alloc_mix_buffers (sr_proj_t _proj);
void sr_song_restart (sr_song_t _song);
void sr_calc_cycle_waveforms (sr_proj_t _proj, sr_song_t _song, sr_cycle_calc_waveform_fxn_t _cycleCalcWaveformFxn);
void sr_process (sr_proj_t _proj,
                 sr_song_t _song,
                 float        *_mixBuf,
                 unsigned int  _numFrames,
                 sr_bool_t     _bClearMixBuf
                 );
void sr_song_delete (sr_song_t _song);
void sr_proj_delete (sr_proj_t _proj);

// external MIDI control API
//  (note) can be used to modulate song playback, trigger+modulate sounds effects, ..
//  (note) must be called from the same thread that calls sr_process()
//  (note) 'port' is in the range 0..15 (dev_idx 'a'..'p')
//  (note) 'ch' is in the range 0..15 (midi channel)
//  (note) 'note' is in the range 0..127 (C-0..G-A)
//  (note) 'vel' is in the range 0..127 (MIDI velocity)
//  (note) 'pb' is in the range 0..16383 (-8192..0..+8192)
//  (note) 'cc' is in the range 0..127 (MIDI continuous controller number, e.g. 1=modwheel)
//  (note) 'rpn' is in the range 0..16383 (MIDI registered-parameter-number, see Synergy remote.msp)
//           (note) RPN#90 selects sample by unique id (14 bit)
//           (note) RPN#300..307 changes track output 1..8 level (e.g. FX sends)
//  (note) 'v' is in the range 0..127 (program,pressure,CC) / 0..16383 (RPN)
void sr_midi_program_change   (sr_proj_t _proj, unsigned char _port, unsigned char _ch, unsigned char _program);
void sr_midi_note_on          (sr_proj_t _proj, unsigned char _port, unsigned char _ch, unsigned char _note, unsigned char _vel);
void sr_midi_note_off         (sr_proj_t _proj, unsigned char _port, unsigned char _ch, unsigned char _note, unsigned char _vel);
void sr_midi_midi_pitchbend   (sr_proj_t _proj, unsigned char _port, unsigned char _ch, unsigned short _pb);
void sr_midi_channel_pressure (sr_proj_t _proj, unsigned char _port, unsigned char _ch, unsigned char _v);
void sr_midi_poly_pressure    (sr_proj_t _proj, unsigned char _port, unsigned char _ch, unsigned char _note, unsigned char _v);
void sr_midi_cc               (sr_proj_t _proj, unsigned char _port, unsigned char _ch, unsigned char _cc, unsigned char _v);
void sr_midi_rpn              (sr_proj_t _proj, unsigned char _port, unsigned char _ch, unsigned short _rpn, unsigned short _v);

#if defined(__cplusplus)
} // extern "C"
#endif // __cplusplus
