// ----
// ---- file   : hal.h
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

#define VP_W  640
#define VP_H  480

#include "../../tksdl2/inc_sdl.h"

#ifdef YAC_WIN32
extern void load_gl_extensions (void);
#endif // YAC_WIN32

extern SDL_Window    *sdl_window;
extern SDL_GLContext  sdl_glcontext;
extern sBool          b_hal_running;

extern sBool hal_window_init (void);
extern void  hal_window_exit (void);
extern void  hal_window_swap (void);
extern void  hal_window_loop (void);
extern void  hal_on_draw (void);  // implemented by app
extern void  hal_on_key_down (sU32 _code, sU32 _mod);  // implemented by app
