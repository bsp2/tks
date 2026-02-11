// ----
// ---- file   : hal_sdl2.c
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
// ---- note   : via SDL2
// ----
// ----
// ----

#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#define Dprintf       if(!MINNIE_PRINTF);else printf
#define Derrorprintf  if(!MINNIE_PRINTF);else printf

#include "../inc_yac.h"
#include "../../tksdl2/inc_sdl.h"
#include "hal.h"

SDL_Window    *sdl_window = NULL;
SDL_GLContext  sdl_glcontext = NULL;
sBool          b_hal_running = YAC_FALSE;

// ---------------------------------------------------------------------------- loc_map_key_sym
static sU32 loc_map_key_sym(sU32 _sym) {
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
   return _sym;
}

// ---------------------------------------------------------------------------- loc_map_key_mod
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

// ---------------------------------------------------------------------------- hal_window_init
sBool hal_window_init(void) {
   sBool ret = YAC_FALSE;

   SDL_InitSubSystem(SDL_INIT_VIDEO);
   SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");
   SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5 );   // require at least 5 bits per channel
   SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
   SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
   SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#ifdef SHADERVG_MSAA
   SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
   SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
#endif
#ifdef SHADERVG_NO_ZS
   SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
   SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
#else
   SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
   SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
#endif // SHADERVG_NO_ZS
   sUI flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
   sdl_window = SDL_CreateWindow("minnie",
                                 SDL_WINDOWPOS_UNDEFINED,
                                 SDL_WINDOWPOS_UNDEFINED,
                                 VP_W, VP_H,
                                 flags
                                 );
   if(NULL != sdl_window)
   {
      sdl_glcontext = SDL_GL_CreateContext(sdl_window);
      Dprintf("[dbg] hal_window_init: SDL_GL_CreateContext -> sdl_glcontext=%p\n", sdl_glcontext);
      if(NULL != sdl_glcontext)
      {
         SDL_GL_MakeCurrent(sdl_window, sdl_glcontext);
         SDL_GL_SetSwapInterval(1);

#ifndef YAC_MACOS
         load_gl_extensions();
#endif // !YAC_MACOS

         ret = YAC_TRUE;
      }
      else
      {
         Derrorprintf("[---] hal_window_init: SDL_GL_CreateContext() failed\n");

         SDL_DestroyWindow(sdl_window);
         sdl_window = NULL;
      }
   }
   else
   {
      Derrorprintf("[---] hal_window_init: SDL_CreateWindow() failed\n");
   }

   return ret;
}

// ---------------------------------------------------------------------------- hal_window_set_title
void hal_window_set_title(const char *_s) {
   SDL_SetWindowTitle(sdl_window, _s);
}

// ---------------------------------------------------------------------------- hal_get_ticks
sU32 hal_get_ticks(void) {
   return SDL_GetTicks();
}

// ---------------------------------------------------------------------------- hal_set_swap_interval
void hal_set_swap_interval(sU32 _interval) {
   SDL_GL_SetSwapInterval(_interval);
}

// ---------------------------------------------------------------------------- hal_window_loop
void hal_window_loop(void) {
   Dprintf("[...] hal: entering event loop..\n");
   b_hal_running = YAC_TRUE;
   while(b_hal_running)
   {
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

      hal_on_draw();
   }
}

// ---------------------------------------------------------------------------- hal_window_swap
void hal_window_swap(void) {
   SDL_GL_SwapWindow(sdl_window);
}

// ---------------------------------------------------------------------------- hal_window_quit
void hal_window_quit(void) {
   b_hal_running = YAC_FALSE;
}

// ---------------------------------------------------------------------------- hal_window_exit
void hal_window_exit(void) {
   if(NULL != sdl_glcontext)
   {
      SDL_GL_DeleteContext(sdl_glcontext);
      sdl_glcontext = NULL;
   }

   if(NULL != sdl_window)
   {
      SDL_DestroyWindow(sdl_window);
      sdl_window = NULL;
   }

   SDL_QuitSubSystem(SDL_INIT_VIDEO);
}
