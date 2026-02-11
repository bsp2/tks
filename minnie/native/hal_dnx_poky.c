// ----
// ---- file   : hal_dnx_poky.c
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
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#ifdef SOFTNX_EVAL_KIT
#include <display/display.h>
#else
#include <display.h>
#endif // SOFTNX_EVAL_KIT

#define DISPLAY_WIDTH   800
#define DISPLAY_HEIGHT  600

#define Dprintf       if(!MINNIE_PRINTF);else printf
#define Derrorprintf  if(!MINNIE_PRINTF);else printf

#include "../inc_yac.h"
#include "../inc_opengl.h"
#include <EGL/egl.h>
#include "hal.h"

static EGLBoolean loc_config_init    (EGLDisplay _display, EGLConfig *_config);
static EGLBoolean loc_egl_init       (EGLDisplay _display);
static EGLSurface loc_surface_create (EGLDisplay _display, EGLConfig _config);
static EGLContext loc_context_create (EGLDisplay _display, EGLConfig _config);

static EGLDisplay display;
static EGLConfig  config;
static EGLContext context;
static EGLSurface surface;

sBool b_hal_running = YAC_FALSE;

// ---------------------------------------------------------------------------- time_get_milliseconds_f64
static sF64 loc_ms_start = 0.0;
static sF64 loc_time_get_milliseconds_f64(void) {

   struct timeval tvnano; gettimeofday(&tvnano, NULL);
   sF64 ret = (tvnano.tv_usec/1000.0) + ((sF64)(tvnano.tv_sec)) * 1000.0;

   return ret;
}

// ---------------------------------------------------------------------------- loc_map_key_sym
static sU32 loc_map_key_sym(sU32 _sym) {
#if 0
   switch(_sym)
   {
      case SDLK_BACKSPACE: _sym = VKEY_BACKSPACE; break;
      case SDLK_TAB:       _sym = VKEY_TAB;       break;
      case SDLK_RETURN:    _sym = VKEY_RETURN;    break;
      case SDLK_ESCAPE:    _sym = VKEY_ESCAPE;    break;
      case SDLK_SPACE:     _sym = VKEY_SPACE;     break;
      case SDLK_UP:        _sym = VKEY_UP;        break;
      case SDLK_DOWN:      _sym = VKEY_DOWN;      break;
      case SDLK_RIGHT:     _sym = VKEY_RIGHT;     break;
      case SDLK_LEFT:      _sym = VKEY_LEFT;      break;
      case SDLK_INSERT:    _sym = VKEY_INSERT;    break;
      case SDLK_HOME:      _sym = VKEY_HOME;      break;
      case SDLK_END:       _sym = VKEY_END;       break;
      case SDLK_PAGEUP:    _sym = VKEY_PAGEUP;    break;
      case SDLK_PAGEDOWN:  _sym = VKEY_PAGEDOWN;  break;
   }
#endif
   return _sym;
}

// ---------------------------------------------------------------------------- loc_map_key_mod
static sU32 loc_map_key_mod(sU32 _mod) {
#if 0
   switch(_mod)
   {
      case KMOD_LSHIFT:  _mod = VMOD_LSHIFT; break;
      case KMOD_RSHIFT:  _mod = VMOD_RSHIFT; break;
      case KMOD_LCTRL:   _mod = VMOD_LCTRL; break;
      case KMOD_RCTRL:   _mod = VMOD_RCTRL; break;
      case KMOD_LALT:    _mod = VMOD_LALT; break;
      case KMOD_RALT:    _mod = VMOD_RALT; break;
   }
#endif
   return _mod;
}


// ---------------------------------------------------------------------------- loc_config_init
static EGLBoolean loc_config_init(EGLDisplay _display, EGLConfig *_config) {
   EGLint attribs[] = {
      EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
      EGL_BUFFER_SIZE,     32,
      EGL_RED_SIZE,        5,
      EGL_GREEN_SIZE,      6,
      EGL_BLUE_SIZE,       5,
      EGL_ALPHA_SIZE,      0,
      EGL_DEPTH_SIZE,      0,
      EGL_STENCIL_SIZE,    0,
      EGL_SAMPLE_BUFFERS,  0,
#ifdef SHADERVG_MSAA
      EGL_SAMPLES,         4,
#else
      EGL_SAMPLES,         1,
#endif // SHADERVG_MSAA
      EGL_NONE
   };

   EGLConfig configs[1];
   EGLint numCfgs;

   if(EGL_TRUE == eglChooseConfig(_display, attribs, configs, 1, &numCfgs)) {

      if(numCfgs > 0) {
         *_config = configs[0];
         return EGL_TRUE;
      }
   }

   return EGL_FALSE;
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
static EGLSurface loc_surface_create(EGLDisplay _display, EGLConfig _config) {
   EGLSurface surf;
   EGLint attribs[] = {
      EGL_RENDER_BUFFER,  EGL_BACK_BUFFER,
      EGL_WIDTH,          DISPLAY_WIDTH,
      EGL_HEIGHT,         DISPLAY_HEIGHT,
      EGL_NONE,           EGL_NONE
   };

   surf = eglCreateWindowSurface(_display, _config, (EGLNativeWindowType)NULL, attribs);
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
sBool hal_window_init(void) {

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

   surface = loc_surface_create(display, config);
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

   printf("GLES version=\"%s\".\n",    (const char*)glGetString(GL_VERSION));
   printf("GLES extensions=\"%s\".\n", (const char*)glGetString(GL_EXTENSIONS));

   return YAC_TRUE;
}

// ---------------------------------------------------------------------------- hal_window_set_title
void hal_window_set_title(const char *_s) {
   (void)_s;
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
}

// ---------------------------------------------------------------------------- hal_window_quit
void hal_window_quit(void) {
   b_hal_running = YAC_FALSE;
}

// ---------------------------------------------------------------------------- hal_window_exit
void hal_window_exit(void) {
   eglTerminate(display);
}
