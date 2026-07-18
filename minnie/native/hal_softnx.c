// ----
// ---- file   : hal_softnx.c
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : Distributed under terms of the MIT license (https://opensource.org/licenses/MIT)
// ----          Copyright 2025-2026 by bsp
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
// ---- info   : "minnie" test hardware abstraction layer
// ---- note   :
// ----
// ----
// ----

#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#define WIN32_LEAN_AND_MEAN defined
#define VC_EXTRALEAN defined
#include <windows.h>
#include <time.h>

#ifdef SOFTNX_EVAL_KIT
#include <display/display.h>
#else
#include <display.h>
#endif // SOFTNX_EVAL_KIT

#define Dprintf       if(!MINNIE_PRINTF);else printf
#define Derrorprintf  if(!MINNIE_PRINTF);else printf

#include "../inc_yac.h"
#include "../inc_opengl.h"
#include <EGL/egl.h>
#include "hal.h"

#ifdef GL_TES_spirv_program_loader
void sdvg_int_find_spirv_program_by_name (const char *_name, const void **retAddr, uint32_t *retSize);
#endif // GL_TES_spirv_program_loader

static EGLBoolean loc_config_init    (EGLDisplay _display, EGLConfig *_config);
static EGLBoolean loc_egl_init       (EGLDisplay _display);
static EGLSurface loc_surface_create (EGLDisplay _display, EGLConfig _config, sUI _w, sUI _h);
static EGLContext loc_context_create (EGLDisplay _display, EGLConfig _config);

static EGLDisplay display;
static EGLConfig  config;
static EGLContext context;
static EGLSurface surface;

static int32_t egl_surface_w = 0;
static int32_t egl_surface_h = 0;

static sBool b_hal_running = YAC_FALSE;


// ---------------------------------------------------------------------------- time_get_milliseconds_f64
static sF64 loc_ms_start = 0.0;
static sF64 loc_time_get_milliseconds_f64(void) {
   LARGE_INTEGER pfcFrequency;

   if(0 == QueryPerformanceFrequency(&pfcFrequency)) {

      pfcFrequency.LowPart  = 0;
      pfcFrequency.HighPart = 0;
   }

   LARGE_INTEGER pfcCurrent;
   QueryPerformanceCounter(&pfcCurrent);

   sF64 ret = (1000.0 * (sF64)pfcCurrent.QuadPart) / ((sF64)pfcFrequency.QuadPart);

   return ret;
}

// ---------------------------------------------------------------------------- loc_map_key_sym
static sU32 loc_map_key_sym(sU32 _sym) {
   // printf("xxx loc_map_key_sym: sym=%u (0x%08x)\n", _sym, _sym);
   if(_sym >= 65 && _sym <= 91)
      _sym |= 32;  // to lowercase
   switch(_sym)
   {
      case 8:  _sym = VKEY_BACKSPACE; break;
      case 9:  _sym = VKEY_TAB;       break;
      case 13: _sym = VKEY_RETURN;    break;
      case 27: _sym = VKEY_ESCAPE;    break;
      case 32: _sym = VKEY_SPACE;     break;
      case 38: _sym = VKEY_UP;        break;
      case 40: _sym = VKEY_DOWN;      break;
      case 39: _sym = VKEY_RIGHT;     break;
      case 37: _sym = VKEY_LEFT;      break;
      case 45: _sym = VKEY_INSERT;    break;
      case 46: _sym = VKEY_DELETE;    break;
      case 36: _sym = VKEY_HOME;      break;
      case 35: _sym = VKEY_END;       break;
      case 33: _sym = VKEY_PAGEUP;    break;
      case 34: _sym = VKEY_PAGEDOWN;  break;
   }
   return _sym;
}

// ---------------------------------------------------------------------------- loc_map_key_mod
#if 0
static sU32 loc_map_key_mod(sU32 _mod) {
   switch(_mod)
   {
      case KMOD_LSHIFT:  _mod = VMOD_LSHIFT; break;
      case KMOD_RSHIFT:  _mod = VMOD_RSHIFT; break;
      case KMOD_LCTRL:   _mod = VMOD_LCTRL; break;
      case KMOD_RCTRL:   _mod = VMOD_RCTRL; break;
      case KMOD_LALT:    _mod = VMOD_LALT; break;
      case KMOD_RALT:    _mod = VMOD_RALT; break;
   }
   return _mod;
}
#endif


// ---------------------------------------------------------------------------- loc_keyboard_handler
void loc_keyboard_handler(uint32_t _keyUp, uint32_t _keyDown) {
   (void)_keyUp;
   sUI code = loc_map_key_sym(_keyDown);
   hal_on_key_down(code, 0u/*mod*/);
}


// ---------------------------------------------------------------------------- loc_mouse_handler
void loc_mouse_handler(int32_t _mouseX, int32_t _mouseY, uint32_t _mouseButtons) {
   (void)_mouseX;
   (void)_mouseY;
   (void)_mouseButtons;
}

// ---------------------------------------------------------------------------- precompiled SPIR-V test
#ifdef GL_TES_spirv_program_loader
#include "all_spirv_shaders.c"

static void loc_spirv_program_loader(const char *programName, const void **retOrNull, uint32_t *retSize) {
   sdvg_int_find_spirv_program_by_name(programName, retOrNull, retSize);
}
#endif // GL_TES_spirv_program_loader

// ---------------------------------------------------------------------------- loc_config_init
static EGLBoolean loc_config_init(EGLDisplay _display, EGLConfig *_config) {
   EGLBoolean ret = EGL_FALSE;

   EGLint attribs[] = {
      EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
#if 1
      EGL_BUFFER_SIZE,     16,
      EGL_RED_SIZE,        5,
      EGL_GREEN_SIZE,      6,
      EGL_BLUE_SIZE,       5,
      EGL_ALPHA_SIZE,      0,
#elif 0
      EGL_BUFFER_SIZE,     16,
      EGL_RED_SIZE,        4,
      EGL_GREEN_SIZE,      4,
      EGL_BLUE_SIZE,       4,
      EGL_ALPHA_SIZE,      0,
#else
      EGL_BUFFER_SIZE,     8,
      EGL_RED_SIZE,        2,
      EGL_GREEN_SIZE,      2,
      EGL_BLUE_SIZE,       2,
      EGL_ALPHA_SIZE,      2,
#endif
#ifdef SHADERVG_DEPTH
      EGL_DEPTH_SIZE,      16,
#else
      EGL_DEPTH_SIZE,      0,
#endif // SHADERVG_DEPTH
#ifdef SHADERVG_STENCIL
      EGL_STENCIL_SIZE,    8,
#else
      EGL_STENCIL_SIZE,    0,
#endif // SHADERVG_STENCIL
      EGL_SAMPLE_BUFFERS,  0,
#ifdef SHADERVG_MSAA
      EGL_SAMPLES,         4,
#else
      EGL_SAMPLES,         1,
#endif // SHADERVG_MSAA
      EGL_NONE
   };

   EGLConfig configs[1024];
   EGLint numCfgs;
   *_config = NULL;

#if 0
   if(EGL_TRUE == eglChooseConfig(_display, attribs, configs, 1024, &numCfgs)) {

      if(numCfgs > 0) {

         for(int32_t cfgIdx = 0; cfgIdx < numCfgs; cfgIdx++) {

            EGLint alphaSz;
            EGLint redSz;
            EGLint greenSz;
            EGLint blueSz;

            if(EGL_TRUE == eglGetConfigAttrib(_display, configs[cfgIdx], EGL_ALPHA_SIZE, &alphaSz) &&
               EGL_TRUE == eglGetConfigAttrib(_display, configs[cfgIdx], EGL_RED_SIZE,   &redSz)   &&
               EGL_TRUE == eglGetConfigAttrib(_display, configs[cfgIdx], EGL_GREEN_SIZE, &greenSz) &&
               EGL_TRUE == eglGetConfigAttrib(_display, configs[cfgIdx], EGL_BLUE_SIZE,  &blueSz)
               ) {

               if(2 == alphaSz && 2 == redSz && 2 == greenSz && 2 == blueSz) {
                  
                  *_config = configs[cfgIdx];
                  ret = EGL_TRUE;
                  break;
               }
            }
         }

      }
   }
#else
   if(EGL_TRUE == eglChooseConfig(_display, attribs, configs, 1, &numCfgs)) {

      if(numCfgs > 0) {
         *_config = configs[0];
         ret = EGL_TRUE;
      }
   }
#endif

   return ret;
}

// ---------------------------------------------------------------------------- loc_egl_init
static EGLBoolean loc_egl_init(EGLDisplay _display) {
   EGLint verMajor = -1;
   EGLint verMinor = -1;

   if(EGL_FALSE == eglInitialize(_display, &verMajor, &verMinor)) {
      return EGL_FALSE;
   }

   printf("EGL verMajor=%d verMinor=%d\n", verMajor, verMinor);
   printf("EGL version=\"%s\".\n",     (const char*)eglQueryString(_display, EGL_VERSION));

   return EGL_TRUE;
}

// ---------------------------------------------------------------------------- loc_surface_create
static EGLSurface loc_surface_create(EGLDisplay _display, EGLConfig _config, sUI _w, sUI _h) {
   EGLSurface surf;
   EGLint attribs[] = {
      EGL_RENDER_BUFFER,  EGL_BACK_BUFFER,
      EGL_WIDTH,          _w,
      EGL_HEIGHT,         _h,
      EGL_NONE,           EGL_NONE
   };

   surf = eglCreateWindowSurface(_display, _config, (EGLNativeWindowType)NULL, attribs);

   if(NULL != surf) {
      // (note) actual surface size may differ from requested size (EGL_W / EGL_H runtime options)
      egl_surface_w = 0u;
      egl_surface_h = 0u;
      if(EGL_TRUE == eglQuerySurface(_display, surf, EGL_WIDTH,  (EGLint*)&egl_surface_w) &&
         EGL_TRUE == eglQuerySurface(_display, surf, EGL_HEIGHT, (EGLint*)&egl_surface_h)
         ) {
         printf("[...] loc_surface_create: EGL surface size is (%u,%u)\n", egl_surface_w, egl_surface_h);
      }
      else {
         printf("[---] loc_surface_create: eglQuerySurface() failed\n");
      }
   }

   return surf;
}

// ---------------------------------------------------------------------------- loc_context_create
static EGLContext loc_context_create(EGLDisplay _display, EGLConfig _config) {
   EGLContext ctx;
   static EGLint attribs[] = {
      EGL_CONTEXT_CLIENT_VERSION, 2,
      EGL_NONE
   };

   ctx = eglCreateContext(_display, _config, NULL/*share_context*/, attribs);
   return ctx;
}

// ---------------------------------------------------------------------------- hal_window_init
sBool hal_window_init(sUI _w, sUI _h) {

   display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
   if(EGL_NO_DISPLAY == display) {
      printf("EGL: failed to get display\n");
      return YAC_FALSE;
   }

   if(EGL_FALSE == loc_egl_init(display)) {
      printf("EGL: failed to initialize EGL\n");
      return YAC_FALSE;
   }

   if(EGL_FALSE == loc_config_init(display, &config)) {
      printf("EGL: failed to find config\n");
      return YAC_FALSE;
   }

   surface = loc_surface_create(display, config, _w, _h);
   if(EGL_NO_SURFACE == surface) {
      printf("EGL: failed to create window surface\n");
      return YAC_FALSE;
   }

   context = loc_context_create(display, config);
   if(EGL_NO_CONTEXT == context) {
      printf("EGL: failed to create window surface\n");
      return YAC_FALSE;
   }

   eglMakeCurrent(display, surface/*draw*/, surface/*read*/, context);

#ifdef GL_TES_spirv_program_loader
   glSPIRVProgramLoaderCallbackTES(&loc_spirv_program_loader);
#endif // GL_TES_spirv_program_loader

   printf("GLES version=\"%s\".\n",    (const char*)glGetString(GL_VERSION));
   printf("GLES extensions=\"%s\".\n", (const char*)glGetString(GL_EXTENSIONS));

   display_callbacks(&loc_keyboard_handler, &loc_mouse_handler);

   return YAC_TRUE;
}

// ---------------------------------------------------------------------------- hal_window_get_size
sBool hal_window_get_size(sUI *_retW, sUI *_retH) {
   *_retW = (sUI)egl_surface_w;
   *_retH = (sUI)egl_surface_h;
   return (0u != egl_surface_w) && (0u != egl_surface_h);
}

// ---------------------------------------------------------------------------- hal_window_set_title
void hal_window_set_title(const char *_s) {
   display_set_window_title(_s);
}

// ---------------------------------------------------------------------------- hal_get_ticks
sU32 hal_get_ticks(void) {
   if(0.0 == loc_ms_start)
   {
      loc_ms_start = loc_time_get_milliseconds_f64();
      return 0u;
   }
   return (sU32)(loc_time_get_milliseconds_f64() - loc_ms_start);
}

// ---------------------------------------------------------------------------- hal_set_swap_interval
void hal_set_swap_interval(sU32 _interval) {
   eglSwapInterval(display, _interval);
}

// ---------------------------------------------------------------------------- hal_window_loop
void hal_window_loop(void) {
   Dprintf("[...] hal: entering event loop..\n");
   b_hal_running = YAC_TRUE;
   while(b_hal_running)
   {
#if 0
      SDL_Event ev;
      int r;
      r = SDL_PollEvent(&ev);
      if(r)
      {
         switch(ev.type)
         {
            default:
               break;

            case SDL_KEYDOWN:
               hal_on_key_down(loc_map_key_sym(ev.key.keysym.sym), loc_map_key_mod(ev.key.keysym.mod));
               break;

            case SDL_QUIT:
               Dprintf("[dbg] received SDL_QUIT, exiting..\n");
               b_hal_running = YAC_FALSE;
               break;
         }

      }
#endif
      hal_on_draw();
   }
}

// ---------------------------------------------------------------------------- hal_window_swap
void hal_window_swap(void) {
   eglSwapBuffers(display, surface);
   display_events();
}

// ---------------------------------------------------------------------------- hal_window_quit
void hal_window_quit(void) {
   b_hal_running = YAC_FALSE;
}

// ---------------------------------------------------------------------------- hal_window_exit
void hal_window_exit(void) {
   eglTerminate(display);
}
