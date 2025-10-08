// ----
// ---- file   : hal.c
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : Distributed under terms of the MIT license (https://opensource.org/licenses/MIT)
// ----          Copyright 2025 by bsp
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

#define Dprintf       if(0);else printf
#define Derrorprintf  if(0);else printf

#include "../inc_yac.h"
#include "hal.h"

SDL_Window    *sdl_window = NULL;
SDL_GLContext  sdl_glcontext = NULL;
sBool          b_hal_running = YAC_FALSE;

// ---------------------------------------------------------------------------- hal_window_init
sBool hal_window_init(void) {
   sBool ret = YAC_FALSE;

   SDL_InitSubSystem(SDL_INIT_VIDEO);
   SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");
   SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );   // require at least 5 bits per channel
   SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5);
   SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5);
   SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16);
   SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#if 0
   SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, multisample_buffers);
   SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, multisample_samples);
#endif
   SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 8);
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

#ifdef YAC_WIN32
         load_gl_extensions();
#endif // YAC_WIN32

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
               hal_on_key_down(ev.key.keysym.sym, ev.key.keysym.mod);
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
