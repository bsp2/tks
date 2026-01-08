// ----
// ---- file   : hal.h
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

#ifndef VP_W
#define VP_W  640
#endif

#ifndef VP_H
#define VP_H  480
#endif

#define VKEY_BACKSPACE    8
#define VKEY_TAB          9
#define VKEY_RETURN      13
#define VKEY_ESCAPE      27
#define VKEY_SPACE       32
#define VKEY_UP         273
#define VKEY_DOWN       274
#define VKEY_RIGHT      275
#define VKEY_LEFT       276
#define VKEY_INSERT     277
#define VKEY_HOME       278
#define VKEY_END        279
#define VKEY_PAGEUP     280
#define VKEY_PAGEDOWN   281

#define VMOD_LSHIFT       1
#define VMOD_RSHIFT       2
#define VMOD_LCTRL       64
#define VMOD_RCTRL      128
#define VMOD_LALT       256
#define VMOD_RALT       512


#ifndef YAC_MACOS
extern void load_gl_extensions (void);
#endif // !YAC_MACOS

extern sBool hal_window_init (void);   // initialize
extern void  hal_window_exit (void);   // shutdown
extern void  hal_window_swap (void);   // swap front/back buffers
extern void  hal_window_loop (void);   // enter event loop
extern void  hal_window_quit (void);   // exit event loop
extern void  hal_window_set_title (const char *_s);
extern sU32  hal_get_ticks (void);     // query milliseconds
extern void  hal_set_swap_interval (sU32 _interval);   // 0=vsync off, 1=vsync on
extern void  hal_on_draw (void);  // implemented by app
extern void  hal_on_key_down (sU32 _code, sU32 _mod);  // implemented by app
