/// HAL.cpp
///
/// (c) 2001-2025 by Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///

/// Experimental: Define this to implement waitVBlank() on Linux. Does not work with NVidia closedsource drivers.
//#define USE_DRM_IOCTL_WAIT_VBLANK defined

//#define DEBUG_WINEVENT_CREATE_DESTROY defined

// Restore GL context in onDraw
//  (note) attempt to fix redraw issues after Arturia plugin window was shown (==> did not fix it)
// #define RESTORE_GL_CONTEXT  defined

#define USE_ICONS      defined

#include <stdio.h>
#include <stdlib.h>

#define YAC_PRINTF defined
#include <yac.h>

#if defined(YAC_LINUX) || defined(YAC_MACOS)
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include <errno.h>

#ifdef HAVE_TIMERFD
#include <sys/timerfd.h>
#endif // HAVE_TIMERFD

#ifdef YAC_LINUX
#include <syscall.h>
#endif // YAC_LINUX

#ifdef HAVE_X11
#include <X11/Xlib.h>
#include <X11/keysym.h>
#endif // HAVE_X11

static struct timeval tv_start;

////#include <sched.h>
#ifdef USE_DRM_IOCTL_WAIT_VBLANK
#include <fcntl.h>
extern "C" {
#define class class__
#include <drm/drm.h>
#undef class
}
#include <sys/ioctl.h>
#endif // USE_DRM_IOCTL_WAIT_VBLANK

#endif // defined(YAC_LINUX) || defined(YAC_MACOS)

#include "inc_sdl.h"

#include "HAL.h"
#include "Joystick.h"
#include "Key.h"
#include "Mouse.h"

extern YAC_Host *yac_host;
extern sUI clid_Joystick;
extern sUI clid_Key;

#include "../tkopengl/inc_opengl.h"

#ifdef DX_GL4_CORE
extern "C" { const GLubyte * APIENTRY glGetStringi (GLenum name, GLuint index); }
#endif // DX_GL4_CORE

#if 0
// SDL2:
// (note) SDL2 SDL_Window is a different struct than SDL12-compat SDL_Window (do not dereference)
extern "C" SDL_Window *SDL_GL_GetCurrentWindow (void);
extern "C" void SDL_SetWindowPosition (SDL_Window * window, int x, int y);
extern "C" void SDL_GetWindowPosition (SDL_Window * window, int *x, int *y);
extern "C" void SDL_SetWindowSize (SDL_Window * window, int w, int h);
extern "C" void SDL_GetWindowSize (SDL_Window * window, int *w, int *h);
extern "C" int  SDL_GetWindowWMInfo (SDL_Window * window, SDL_SysWMinfo * info);
extern "C" void*/*SDL_GLContext SDLCALL*/ SDL_GL_GetCurrentContext (void);
extern "C" int SDL_GL_MakeCurrent(SDL_Window * window, void */*SDL_GLContext*/ context);
#endif

static SDL_SysWMinfo syswminfo;

#include "Bitmap.h"
#include "HAL.h"

#include "tksdl_hooks.h"

tksdl_hook_t tksdl_hook_open;
tksdl_hook_t tksdl_hook_close;

extern _HAL *tks_HAL;

extern void*sc_context; // script execution context for HAL callbacks (onReopen, onKeyboard, onMouse, ...)

// ---- SDL "scrap" clipboard support (x11/qnx/win32)
#ifdef TKSDL_SCRAP_CLIPBOARD
extern int init_scrap(void);
#endif


#include "../tkopengl/tkopengl_shared.h"
#include "../tkopengl/tkopengl_shared.cpp"


// Statically allocated strings for sendEvent()
//  (note) dynamically allocating strings in sendEvent and deleting it in the event handler
//          causes random memory leaks since apparently not every sent event is handled/received
#define TKSDL_MAX_EVENT_STRINGS 256  // must be POT
static YAC_String *event_strings[TKSDL_MAX_EVENT_STRINGS];
static sUI next_event_string;


#ifdef YAC_MACOS
extern void hal_macos_window_to_front (void/*NSWindow*/ *_window);
#endif // YAC_MACOS


#ifdef YAC_WIN32
static HANDLE win32_timerthread = NULL;
static HANDLE win32_timerevent = NULL;

static DWORD WINAPI tksdl_timer__threadentry(LPVOID _hal) {
   _SDL *hal = (_SDL*)_hal;

   if(yac_host->yacGetDebugLevel() >= 2)
      yac_host->printf("[dbg] HAL::timerthread: threadId=%d\n", GetCurrentThreadId());

   for(;;)
   {
      DWORD res = ::WaitForSingleObject(win32_timerevent, hal->timer_interval);
      if(WAIT_TIMEOUT == res)
      {
         hal->sendTimerEvent();
      }
      else
      {
         if(yac_host->yacGetDebugLevel() >= 2)
            yac_host->printf("[dbg] HAL::timerthread: exiting (res=0x%08x)\n", res);
         break;
      }
   }

   return 0;
}
#endif // YAC_WIN32


#if defined(YAC_LINUX) || defined(YAC_MACOS)
static volatile sBool b_timerthread_running = YAC_FALSE;
static pthread_t timerthread_id;

#ifdef HAVE_TIMERFD
static int timerthread_fd = -1;
#endif // HAVE_TIMERFD

static void *tksdl_timer__threadentry(void *_hal) {
   _SDL *hal = (_SDL*)_hal;

   if(yac_host->yacGetDebugLevel() >= 2)
   {
#ifdef YAC_LINUX
      yac_host->printf("[dbg] HAL::timerthread: threadId=" YAC_PRINTF_PTRPREFIX "%p LWPid=%d\n", pthread_self(), syscall(SYS_gettid));
#else
      yac_host->printf("[dbg] HAL::timerthread: threadId=" YAC_PRINTF_PTRPREFIX "%p\n", pthread_self());
#endif // YAC_LINUX
   }

#ifdef HAVE_TIMERFD
   timerthread_fd = timerfd_create(CLOCK_MONOTONIC, 0/*flags*/);

   if(-1 != timerthread_fd)
   {
      struct itimerspec it;

      it.it_value.tv_sec = hal->timer_interval / 1000;
      it.it_value.tv_nsec = (hal->timer_interval % 1000) * (1000 * 1000);

      it.it_interval = it.it_value;

      int err = timerfd_settime(timerthread_fd,
                                0, /* flags */
                                &it,
                                NULL /* old */
                                );
      if(0 == err)
      {
         b_timerthread_running = YAC_TRUE;

         while(b_timerthread_running)
         {
            sU64 numExp;

            ssize_t numRead = read(timerthread_fd, &numExp, 8);

            if(8 == numRead)
            {
               hal->sendTimerEvent();
            }
            else
            {
               yac_host->printf("[~~~] HAL::timerthread: read() failed. numRead=%d, errno=%d(\"%s\").\n",
                                numRead, errno, strerror(errno)
                                );
            }
         }

         close(timerthread_fd);
         timerthread_fd = -1;
      }
      else
      {
         yac_host->printf("[---] HAL::timerthread: timerfd_settime() failed. errno=%d(\"%s\").\n",
                          errno, strerror(errno)
                          );
      }
   }
   else
   {
      yac_host->printf("[---] HAL::timerthread: timerfd_create() failed. errno=%d(\"%s\").\n",
                       errno, strerror(errno)
                       );
   }

#else
   // Fallback implementation (e.g. macOS)
   b_timerthread_running = YAC_TRUE;

   while(b_timerthread_running)
   {
      // yac_host->printf("xxx timer: wait %u ms\n", hal->timer_interval);
      ::usleep(1000u * hal->timer_interval/*ms*/);
      hal->sendTimerEvent();
   }

#endif // HAVE_TIMERFD

   return NULL;
}
#endif // defined(YAC_LINUX) || defined(YAC_MACOS)


static void hook_call(tksdl_hook_t *hook) {

   while(NULL != hook)
   {
      if(NULL != hook->fxn)
      {
         ////printf("xxx tksdl::hook_call: hook.fxn=0x%p\n", hook->fxn);
         hook->fxn();
      }

      hook = hook->next;
   }
}


static sU32 loc_sdl2_keycode_to_vkey(sU32 _kc) {
   // returns 'vkey' (== SDL1 keycode)
   sU32 sc = ::SDL_GetScancodeFromKey(_kc);
   // yac_host->printf("xxx loc_sdl2_keycode_to_vkey(0x%08x) => 0x%08x\n", _kc, sc);
   switch(sc)
   {
      case SDL_SCANCODE_A:            return (sU32)'a';
      case SDL_SCANCODE_B:            return (sU32)'b';
      case SDL_SCANCODE_C:            return (sU32)'c';
      case SDL_SCANCODE_D:            return (sU32)'d';
      case SDL_SCANCODE_E:            return (sU32)'e';
      case SDL_SCANCODE_F:            return (sU32)'f';
      case SDL_SCANCODE_G:            return (sU32)'g';
      case SDL_SCANCODE_H:            return (sU32)'h';
      case SDL_SCANCODE_I:            return (sU32)'i';
      case SDL_SCANCODE_J:            return (sU32)'j';
      case SDL_SCANCODE_K:            return (sU32)'k';
      case SDL_SCANCODE_L:            return (sU32)'l';
      case SDL_SCANCODE_M:            return (sU32)'m';
      case SDL_SCANCODE_N:            return (sU32)'n';
      case SDL_SCANCODE_O:            return (sU32)'o';
      case SDL_SCANCODE_P:            return (sU32)'p';
      case SDL_SCANCODE_Q:            return (sU32)'q';
      case SDL_SCANCODE_R:            return (sU32)'r';
      case SDL_SCANCODE_S:            return (sU32)'s';
      case SDL_SCANCODE_T:            return (sU32)'t';
      case SDL_SCANCODE_U:            return (sU32)'u';
      case SDL_SCANCODE_V:            return (sU32)'v';
      case SDL_SCANCODE_W:            return (sU32)'w';
      case SDL_SCANCODE_X:            return (sU32)'x';
      case SDL_SCANCODE_Y:            return (sU32)'y';
      case SDL_SCANCODE_Z:            return (sU32)'z';
      case SDL_SCANCODE_APOSTROPHE:   return VKEY_QUOTE;  // was SDLK_QUOTE in SDL1  (de:ä)
      case SDL_SCANCODE_RETURN:       return VKEY_RETURN;
      case SDL_SCANCODE_ESCAPE:       return VKEY_ESCAPE;
      case SDL_SCANCODE_BACKSPACE:    return VKEY_BACKSPACE;
      case SDL_SCANCODE_TAB:          return VKEY_TAB;
      case SDL_SCANCODE_SPACE:        return VKEY_SPACE;
      // case SDL_SCANCODE_EXCLAIM:      return VKEY_EXCLAIM;
      // case SDL_SCANCODE_QUOTEDBL:     return VKEY_QUOTEDBL;
      // case SDL_SCANCODE_HASH:         return VKEY_HASH;
      // case SDL_SCANCODE_PERCENT:      return (sU32)'%';  // (todo) VKEY_PERCENT
      // case SDL_SCANCODE_DOLLAR:       return VKEY_DOLLAR;
      // case SDL_SCANCODE_AMPERSAND:    return VKEY_AMPERSAND;
      // case SDL_SCANCODE_QUOTE:        return VKEY_QUOTE;
      // case SDL_SCANCODE_LEFTPAREN:    return VKEY_LEFTPAREN;
      // case SDL_SCANCODE_RIGHTPAREN:   return VKEY_RIGHTPAREN;
      // case SDL_SCANCODE_ASTERISK:     return VKEY_ASTERISK;
      // case SDL_SCANCODE_PLUS:         return VKEY_PLUS;
      case SDL_SCANCODE_COMMA:        return VKEY_COMMA;
      case SDL_SCANCODE_MINUS:        return VKEY_MINUS;
      case SDL_SCANCODE_PERIOD:       return VKEY_PERIOD;
      case SDL_SCANCODE_SLASH:        return VKEY_SLASH;
      case SDL_SCANCODE_0:            return VKEY_0;
      case SDL_SCANCODE_1:            return VKEY_1;
      case SDL_SCANCODE_2:            return VKEY_2;
      case SDL_SCANCODE_3:            return VKEY_3;
      case SDL_SCANCODE_4:            return VKEY_4;
      case SDL_SCANCODE_5:            return VKEY_5;
      case SDL_SCANCODE_6:            return VKEY_6;
      case SDL_SCANCODE_7:            return VKEY_7;
      case SDL_SCANCODE_8:            return VKEY_8;
      case SDL_SCANCODE_9:            return VKEY_9;
      // // case SDL_SCANCODE_COLON:        return VKEY_COLON;
      case SDL_SCANCODE_SEMICOLON:    return VKEY_SEMICOLON;
      // // case SDL_SCANCODE_LESS:         return VKEY_LESS;
      case SDL_SCANCODE_NONUSBACKSLASH: return VKEY_LESS;  // "This is the additional key that ISO keyboards have over ANSI ones, located between left shift and Y"
      case SDL_SCANCODE_EQUALS:       return VKEY_EQUALS;
      // case SDL_SCANCODE_GREATER:      return VKEY_GREATER;
      // // case SDL_SCANCODE_QUESTION:     return VKEY_QUESTION;
      // // case SDL_SCANCODE_AT:           return VKEY_AT;
      case SDL_SCANCODE_LEFTBRACKET:  return VKEY_LEFTBRACKET;
      case SDL_SCANCODE_BACKSLASH:    return VKEY_BACKSLASH;
      case SDL_SCANCODE_RIGHTBRACKET: return VKEY_RIGHTBRACKET;
      // // case SDL_SCANCODE_CARET:        return VKEY_CARET;
      // // case SDL_SCANCODE_UNDERSCORE:   return VKEY_UNDERSCORE;
      // case SDL_SCANCODE_BACKQUOTE:    return VKEY_BACKQUOTE;
      case SDL_SCANCODE_GRAVE:        return VKEY_BACKQUOTE;  // "Located in the top left corner (on both ANSI and ISO keyboards)"
      case SDL_SCANCODE_CAPSLOCK:     return VKEY_CAPSLOCK;
      case SDL_SCANCODE_F1:           return VKEY_F1;
      case SDL_SCANCODE_F2:           return VKEY_F2;
      case SDL_SCANCODE_F3:           return VKEY_F3;
      case SDL_SCANCODE_F4:           return VKEY_F4;
      case SDL_SCANCODE_F5:           return VKEY_F5;
      case SDL_SCANCODE_F6:           return VKEY_F6;
      case SDL_SCANCODE_F7:           return VKEY_F7;
      case SDL_SCANCODE_F8:           return VKEY_F8;
      case SDL_SCANCODE_F9:           return VKEY_F9;
      case SDL_SCANCODE_F10:          return VKEY_F10;
      case SDL_SCANCODE_F11:          return VKEY_F11;
      case SDL_SCANCODE_F12:          return VKEY_F12;
      case SDL_SCANCODE_F13:          return VKEY_F13;
      case SDL_SCANCODE_F14:          return VKEY_F14;
      case SDL_SCANCODE_F15:          return VKEY_F15;
      case SDL_SCANCODE_PRINTSCREEN:  return VKEY_PRINT;
      case SDL_SCANCODE_SCROLLLOCK:   return VKEY_SCROLLOCK;
      case SDL_SCANCODE_PAUSE:        return VKEY_PAUSE;
      case SDL_SCANCODE_INSERT:       return VKEY_INSERT;
      case SDL_SCANCODE_HOME:         return VKEY_HOME;
      case SDL_SCANCODE_PAGEUP:       return VKEY_PAGEUP;
      case SDL_SCANCODE_DELETE:       return VKEY_DELETE;
      case SDL_SCANCODE_END:          return VKEY_END;
      case SDL_SCANCODE_PAGEDOWN:     return VKEY_PAGEDOWN;
      case SDL_SCANCODE_RIGHT:        return VKEY_RIGHT;
      case SDL_SCANCODE_LEFT:         return VKEY_LEFT;
      case SDL_SCANCODE_DOWN:         return VKEY_DOWN;
      case SDL_SCANCODE_UP:           return VKEY_UP;
      case SDL_SCANCODE_NUMLOCKCLEAR: return VKEY_NUMLOCK;
      case SDL_SCANCODE_KP_DIVIDE:    return VKEY_KP_DIVIDE;
      case SDL_SCANCODE_KP_MULTIPLY:  return VKEY_KP_MULTIPLY;
      case SDL_SCANCODE_KP_MINUS:     return VKEY_KP_MINUS;
      case SDL_SCANCODE_KP_PLUS:      return VKEY_KP_PLUS;
      case SDL_SCANCODE_KP_ENTER:     return VKEY_KP_ENTER;
      case SDL_SCANCODE_KP_1:         return VKEY_KP_1;
      case SDL_SCANCODE_KP_2:         return VKEY_KP_2;
      case SDL_SCANCODE_KP_3:         return VKEY_KP_3;
      case SDL_SCANCODE_KP_4:         return VKEY_KP_4;
      case SDL_SCANCODE_KP_5:         return VKEY_KP_5;
      case SDL_SCANCODE_KP_6:         return VKEY_KP_6;
      case SDL_SCANCODE_KP_7:         return VKEY_KP_7;
      case SDL_SCANCODE_KP_8:         return VKEY_KP_8;
      case SDL_SCANCODE_KP_9:         return VKEY_KP_9;
      case SDL_SCANCODE_KP_0:         return VKEY_KP_0;
      case SDL_SCANCODE_KP_PERIOD:    return VKEY_KP_PERIOD;
      case SDL_SCANCODE_KP_EQUALS:    return VKEY_KP_EQUALS;  // remove?
      case SDL_SCANCODE_HELP:         return VKEY_HELP;  // remove?
      case SDL_SCANCODE_APPLICATION:  return VKEY_MENU;  // was SDLK_MENU in SDL1
      // case SDL_SCANCODE_KP_COMMA:     return VKEY_KP_COMMA;
      case SDL_SCANCODE_SYSREQ:       return VKEY_SYSREQ;
      case SDL_SCANCODE_LCTRL:        return VKEY_LCTRL;
      case SDL_SCANCODE_LSHIFT:       return VKEY_LSHIFT;
      case SDL_SCANCODE_LALT:         return VKEY_LALT;
      case SDL_SCANCODE_LGUI:         return VKEY_LMETA;
      case SDL_SCANCODE_RCTRL:        return VKEY_RCTRL;
      case SDL_SCANCODE_RSHIFT:       return VKEY_RSHIFT;
      case SDL_SCANCODE_RALT:         return VKEY_RALT;
      case SDL_SCANCODE_RGUI:         return VKEY_RMETA;
      case SDL_SCANCODE_MODE:         return VKEY_MODE;  // remove?
   }
   yac_host->printf("[~~~] tksdl: unhandled scancode 0x%08x\n", sc);
   return (sc & 0x3FFFffffu);  // filter out SDLK_SCANCODE_MASK
}


static sU32 loc_utf8_to_u32(const sU8 *_s) {
   sU8 c = _s[0];
   // yac_host->printf("xxx loc_utf8_to_u32: s[0]=0x%08x s[1]=0x%08x s[2]=0x%08x s[3]=0x%08x\n", c, _s[1], _s[2], _s[3]);
   if( (c & 0xF8u) == 0xF0u )
   {
      // 4 bytes
      return ((c&7u) << (6+6+6)) | ((_s[1]&63u) << (6+6)) | ((_s[2]&63u) << 6) | (_s[3]&63u);
   }
   else if( (c & 0xF0u) == 0xE0u )
   {
      // 3 bytes
      return ((c&15u) << (6+6)) | ((_s[1]&63u) << 6) | (_s[2]&63u);
   }
   else if( (c & 0xE0u) == 0xC0u )
   {
      // 2 bytes
      // ä:  s[0] = $C3 = 110 00011
      //     s[1] = $A4 = 10 100100
      return ((c&31u) << 6) | (_s[1]&63u);
   }
   else
   {
      // 1 byte
      return (c&127u);
   }
}


#ifdef USE_ICONS
// window icon (todo) make this configurable from the application
//  (note) required for macOS (??)
const unsigned int  mem_size_loc_icon_bmp = 578;
const unsigned char mem_base_loc_icon_bmp[578] = {
      66u,  77u,  66u,   2u,   0u,   0u,   0u,   0u,   0u,   0u,  66u,   0u,   0u,   0u,  40u,   0u
   ,   0u,   0u,  32u,   0u,   0u,   0u,  32u,   0u,   0u,   0u,   1u,   0u,   4u,   0u,   0u,   0u
   ,   0u,   0u,   0u,   2u,   0u,   0u, 109u,  11u,   0u,   0u, 109u,  11u,   0u,   0u,   3u,   0u
   ,   0u,   0u,   3u,   0u,   0u,   0u, 255u, 255u, 255u,   0u,   0u,   0u,   0u,   0u,   0u, 255u
   , 255u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u
   ,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u
   ,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u
   ,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u
   ,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,  17u,  17u,  17u,  17u,   0u,   0u,   0u,   0u
   ,   0u,   0u,   0u,   0u,   0u,   0u,   0u,  17u,  34u,  34u,  34u,  34u,  17u,   0u,   0u,   0u
   ,   0u,   0u,   0u,   0u,   0u,   0u,   1u,  34u,  33u,  17u,  17u,  18u,  34u,  16u,   0u,   0u
   ,   0u,   0u,   0u,   0u,   0u,   0u,  18u,  33u,  17u,  17u,  17u,  17u,  18u,  33u,   0u,   0u
   ,   0u,   0u,   0u,   0u,   0u,   1u,  34u,  17u,  17u,  17u,  17u,  17u,  17u,  34u,  16u,   0u
   ,   0u,   0u,   0u,   0u,   0u,  18u,  33u,  17u,  34u,  34u,  34u,  34u,  17u,  18u,  33u,   0u
   ,   0u,   0u,   0u,   0u,   0u,  18u,  34u,  34u,  34u,  34u,  34u,  34u,  34u,  34u,  33u,   0u
   ,   0u,   0u,   0u,   0u,   1u,  34u,  34u,  34u,  34u,  34u,  34u,  34u,  34u,  34u,  34u,  16u
   ,   0u,   0u,   0u,   0u,   1u,  34u,  34u,  34u,  34u,  33u,  18u,  34u,  34u,  34u,  34u,  16u
   ,   0u,   0u,   0u,   0u,   1u,  34u,  34u,  34u,  34u,  33u,  18u,  34u,  34u,  34u,  34u,  16u
   ,   0u,   0u,   0u,   0u,   1u,  34u,  34u,  34u,  34u,  34u,  34u,  34u,  34u,  34u,  34u,  16u
   ,   0u,   0u,   0u,   0u,   1u,  34u,  34u,  34u,  34u,  34u,  34u,  34u,  34u,  34u,  34u,  16u
   ,   0u,   0u,   0u,   0u,   1u,  34u,  34u,  17u,  17u,  34u,  34u,  17u,  17u,  34u,  34u,  16u
   ,   0u,   0u,   0u,   0u,   1u,  34u,  34u,  17u,   1u,  34u,  34u,  17u,   1u,  34u,  34u,  16u
   ,   0u,   0u,   0u,   0u,   0u,  18u,  34u,  17u,  17u,  34u,  34u,  17u,  17u,  34u,  33u,   0u
   ,   0u,   0u,   0u,   0u,   0u,  18u,  34u,  34u,  34u,  34u,  34u,  34u,  34u,  34u,  33u,   0u
   ,   0u,   0u,   0u,   0u,   0u,   1u,  34u,  34u,  34u,  34u,  34u,  34u,  34u,  34u,  16u,   0u
   ,   0u,   0u,   0u,   0u,   0u,   0u,  18u,  34u,  34u,  34u,  34u,  34u,  34u,  33u,   0u,   0u
   ,   0u,   0u,   0u,   0u,   0u,   0u,   1u,  34u,  34u,  34u,  34u,  34u,  34u,  16u,   0u,   0u
   ,   0u,   0u,   0u,   0u,   0u,   0u,   0u,  17u,  34u,  34u,  34u,  34u,  17u,   0u,   0u,   0u
   ,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,  17u,  17u,  17u,  17u,   0u,   0u,   0u,   0u
   ,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u
   ,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u
   ,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u
   ,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u
   ,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u
   ,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u
   ,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u
   ,   0u,   0u                                                                                    
};

// borrowed from SDL/test/testwm.c:
static SDL_Surface *LoadIconSurface(const unsigned char *_data, const unsigned int _dataSz, Uint8 **maskp)
{
	SDL_Surface *icon;
	Uint8       *pixels;
	Uint8       *mask;
	int          mlen, i, j;

	*maskp = NULL;

	/* Load the icon surface */
	icon = SDL_LoadBMP_RW(SDL_RWFromConstMem(_data, _dataSz), 0/*freesrc*/);
   if(Dyac_host_yacGetDebugLevel() >= 2)
      yac_host->printf("[trc] HAL::LoadIconSurface: icon=%p\n", icon);
	if ( icon == NULL ) {
		yac_host->printf("[---] tksdl::LoadIconSurface: SDL_LoadBMP_RW() failed: %s\n", SDL_GetError());
		return(NULL);
	}

	/* Check width and height 
	if ( (icon->w%8) != 0 ) {
		fprintf(stderr, "Icon width must be a multiple of 8!\n");
		SDL_FreeSurface(icon);
		return(NULL);
	}
*/

	if ( icon->format->palette == NULL ) {
		fprintf(stderr, "[---] tksdl::LoadIconSurface: Icon must have a palette!\n");
		SDL_FreeSurface(icon);
		return NULL;
	}

	/* Set the colorkey */
	SDL_SetColorKey(icon, SDL_TRUE/*flag*/, *((Uint8 *)icon->pixels));

	/* Create the mask */
	pixels = (Uint8 *)icon->pixels;
#if 0
	printf("Transparent pixel: (%d,%d,%d)\n",
				icon->format->palette->colors[*pixels].r,
				icon->format->palette->colors[*pixels].g,
				icon->format->palette->colors[*pixels].b);
#endif
	mlen = (icon->w*icon->h + 7) / 8;
	mask = (Uint8 *)malloc(mlen);
	if ( mask == NULL ) {
		fprintf(stderr, "[---] tksdl::LoadIconSurface: Out of memory!\n");
		SDL_FreeSurface(icon);
		return(NULL);
	}
	memset(mask, 0, mlen);
	for ( i=0; i < icon->h; i++)
        for (j=0; j < icon->w; j++) {
            int pindex = i * icon->pitch + j;
            int mindex = i * icon->w + j;
            if ( pixels[pindex] != *pixels )
                mask[mindex>>3] |= 1 << (7 - (mindex & 7));
        }
	*maskp = mask;
	return(icon);
}

static void loc_set_icon(SDL_Window *_sdlWindow) {
	/* Set the icon -- this must be done before the first mode set */
	SDL_Surface *icon;
	Uint8 *icon_mask;
	icon = LoadIconSurface(mem_base_loc_icon_bmp, mem_size_loc_icon_bmp, &icon_mask);
	if ( icon != NULL ) {
		// SDL_WM_SetIcon(icon, icon_mask);
      SDL_SetWindowIcon(_sdlWindow, icon);
	}
   else {
      yac_host->printf("[---] tksdl::loc_set_icon: LoadIconSurface(\"icon.bmp\") failed");
   }
	if ( icon_mask != NULL )
		free(icon_mask);
}
#endif // USE_ICONS


_HAL::_HAL(void) {
   b_running         = YAC_FALSE;
   b_graphics_active = YAC_FALSE;
   b_need_redraw     = YAC_FALSE;
   b_event_polling   = YAC_TRUE;
   b_double_buffer   = YAC_TRUE;
   b_redrawing       = YAC_FALSE;
   b_allow_redraw    = YAC_TRUE;
   b_glerror         = YAC_FALSE;
#ifdef DX_GL4_CORE
   b_glcore          = YAC_TRUE;
#else
   b_glcore          = YAC_FALSE;
#endif // DX_GL4_CORE
   b_onclose_pending = YAC_FALSE;
   swap_interval     = 1;

   b_key_repeat      = YAC_TRUE;
   last_vmod         = 0u;
   last_vkey         = 0u;

   mtx_event_queued = yac_host->yacMutexCreate();
   b_timer_event_queued  = YAC_FALSE;
   b_redraw_event_queued = YAC_FALSE;
   b_reset_queued_events = YAC_FALSE;

   max_frame_deltatime = 0;

   multisample_buffers = 0;
   multisample_samples = 0;

   stencil_bits = 0;

   for(int i = 0; i < TKS_HAL_NUMSIGNALS; i++)
      signal_funs[i] = NULL;

   mouse.show                  = 1;
   mouse.def_auto_hide_timeout = 1500;
   mouse.auto_hide_timeout     = 0;
   mouse.pointer_mode          = TKS_MOUSE_POINTER_MOVE;
   mouse.b_touch_input         = YAC_FALSE;

   joysticks.enabled  = YAC_FALSE;
   joysticks.num      = 0u;
   joysticks.elements = NULL;

   redraw_flag = TKS_REDRAW_ALL;

   win_sx     = 0;
   win_sy     = 0;
   fs_sx      = 0; // default = desktop resolution
   fs_sy      = 0;
   fs_z       = 32;
   view_sx    = 0;
   view_sy    = 0;
   view_z     = 0;
   view_flags = 0;
   sdl1_flags = 16/*SDL_RESIZABLE*/;

   desktop_sx = 0;
   desktop_sy = 0;

   min_w = 64;
   min_h = 48;

#ifdef YAC_WIN32
   lpDD           = NULL;
   dw_monitorfreq = 0;
#endif

   audio.dsp_frequency = 44100.0f; // overwritten by AudioDevice::openDSP()

   fps.tick_buffer   = 10;
   fps.tick_interval = 1000.0f/60;
   fps.limit_fps     = 0;

#if defined(YAC_LINUX) || defined(YAC_MACOS)
   gettimeofday(&tv_start, NULL);
#endif // defined(YAC_LINUX) || defined(YAC_MACOS)

   timer_interval = 0; // Disable timer

   // Export address of b_glerror flag for tkopengl plugin ('abuses' the callback API..)
   yac_host->yacCallbackSetFunByName("tksdl_b_glerror", (YAC_CFunctionPtr)&b_glerror);

   // Export address of b_glcore flag for tkopengl plugin ('abuses' the callback API..)
   yac_host->yacCallbackSetFunByName("tksdl_b_glcore", (YAC_CFunctionPtr)&b_glcore);

   // Init event strings
   {
      for(sUI i = 0u; i < TKSDL_MAX_EVENT_STRINGS; i++)
      {
         event_strings[i] = YAC_New_String();
      }
      next_event_string = 0;
   }

   b_window_visible = false;

   queued_window_move_px = -1;
   queued_window_move_py = -1;

   local_to_raw_key_table = NULL;

   pending_resize_ms = -1;
   ignore_resize_timeout = 0u;

   sdl_window    = NULL;
   sdl_glcontext = NULL;

   drop_file_names = NULL;

   cursor_zoom = 1u;
}

_HAL::~_HAL() {

   // yac_host->printf("xxx ~HAL 1 (ENTER)\n");

   if(::SDL_WasInit(SDL_INIT_VIDEO))
   {
      yac_host->printf("[dbg] tksdl::~HAL: SDL_QuitSubSystem(SDL_INIT_VIDEO)\n");
      ::SDL_QuitSubSystem(SDL_INIT_VIDEO);
      b_window_visible = false;
   }

   // yac_host->printf("xxx ~HAL 2\n");


#ifdef YAC_WIN32
   if(lpDD)
   {
      lpDD->Release();
      lpDD = NULL;
   }
#endif

   // yac_host->printf("xxx ~HAL 3 (LEAVE)\n");

   // Delete event strings
   {
      sUI i;
      for(i=0; i<TKSDL_MAX_EVENT_STRINGS; i++)
      {
         YAC_DELETE(event_strings[i]);
         event_strings[i] = NULL;
      }
   }

   yac_host->yacMutexDestroy(mtx_event_queued);
}

void YAC_VCALL _HAL::yacGetSignalStringList(YAC_String *_sig) {
   // ---- the number represents the encoded argument type identifiers used by this function
   // ---- 1=int, 2=float, 3=object. bits 0+1 represent argument #0, bits 2+3 represent argument #1 and so on
   _sig->visit("onSignal:1 onKeyboard:3 onMouse:85 onJoyAxisMotion:21 onJoyButton:21 onJoyHatMotion:21 onJoyBallMotion:85 onDraw:0 onTick:0 onClose:0 onEvent:3 onOpen:0 onResize:0 onExpose:0 onApplicationActive:1 onMouseFocusActive:1 onIdle:0 onInputFocusActive:1 onDropFiles:53 onTimer:0 onCreateOtherWindow:1 onFocusOtherWindow:1 onMoveWindow:5 onDrainEventQueue:0");
}

void YAC_VCALL _HAL::yacRegisterSignal(sUI _id, YAC_FunctionHandle _fun) {
   ////printf("xxx tksdl::yacRegisterSignal: id=%u fun=0x%p\n", _id, _fun);
   if(_id < TKS_HAL_NUMSIGNALS)
   {
      signal_funs[_id] = _fun;
   }
}

sSI _HAL::_getNumJoysticks(void) {
   if(!joysticks.enabled)
      openJoysticks();
   return joysticks.num;
}

void _HAL::_getJoystick(sSI _idx, YAC_Value *_r) {
   if(!joysticks.enabled)
      openJoysticks();

   if(((sUI)_idx) < ((sUI)joysticks.num))
   {
      _Joystick *j = &((_Joystick*)joysticks.elements)[_idx];
      YAC_RETO(j, 0);
   }
   else
   {
      YAC_RETO(0, 0);
   }
}

void _HAL::_updateJoysticks(void) {
   if(joysticks.enabled)
   {
#ifdef USE_JOYSTICKS
      ::SDL_JoystickUpdate();
#endif // USE_JOYSTICKS
   }
}


void _HAL::_triggerEvent(YAC_Object *_s) {
   YAC_String *s;

   if(YAC_BCHK(_s, YAC_CLID_STRING))
   {
      s = (YAC_String*)_s;
   }
   else
   {
      s = NULL;
   }

   if(NULL != s)
   {
      YAC_FunctionHandle f = signal_funs[TKS_SIG_HALEVENT];

      if(NULL != f)
      {
         YAC_Value args[1];
         args[0].initString(s, 0);
         yac_host->yacEvalFunction(sc_context, f, 1, args);
      }
   }
}

void _HAL::_sendEvent(YAC_Object *_s) {
   // yac_host->printf("xxx SDL::sendEvent\n");

   if(YAC_BCHK(_s, YAC_CLID_STRING))
   {
      SDL_Event user_event;

      YAC_String *es = event_strings[next_event_string];
      next_event_string = (next_event_string + 1) & (TKSDL_MAX_EVENT_STRINGS - 1);
      es->yacCopy((YAC_String*)_s);
      // yac_host->printf("xxx HAL<tksdl2>::sendEvent: es->chars=\"%s\"\n", (const char*)es->chars);

      user_event.type       = SDL_USEREVENT;
      user_event.user.code  = TKS_EVENT_USER;
      user_event.user.data1 = (void *)es;
      user_event.user.data2 = (void *)size_t(_getTicks());

      ::SDL_PushEvent(&user_event);
   }
}

void _HAL::sendTimerEvent(void) {

   // yac_host->printf("xxx SDL::sendTimerEvent: ENTER b_timer_event_queued=%d\n", b_timer_event_queued);

   yac_host->yacMutexLock(mtx_event_queued);

   // static int bLogAlreadyQueued = 1;

   if(!b_timer_event_queued)
   {
      b_timer_event_queued = 1;

      SDL_Event user_event;
      
      // yac_host->printf("xxx SDL::sendTimerEvent\n");

      // called from timerthread

      user_event.type       = SDL_USEREVENT;
      user_event.user.code  = TKS_EVENT_TIMER;
      user_event.user.data1 = 0;
      user_event.user.data2 = 0;//(void *)_getTicks();

      ::SDL_PushEvent(&user_event);

      // bLogAlreadyQueued = 1;
   }
   else
   {
      // if(bLogAlreadyQueued)
      //    yac_host->printf("xxx SDL::sendTimerEvent: already queued\n");
      // bLogAlreadyQueued = 0;
   }

   yac_host->yacMutexUnlock(mtx_event_queued);

   // yac_host->printf("xxx SDL::sendTimerEvent: LEAVE\n");
}

void _HAL::sendRedrawEvent(void) {

   yac_host->yacMutexLock(mtx_event_queued);

   if(!b_redraw_event_queued)
   {
      b_redraw_event_queued = 1;

      SDL_Event user_event;

      // yac_host->printf("xxx SDL::sendRedrawEvent\n");

      // usually called from UI thread (via Viewport.needRedraw())

      user_event.type       = SDL_USEREVENT;
      user_event.user.code  = TKS_EVENT_REDRAW;
      user_event.user.data1 = 0;
      user_event.user.data2 = 0;

      ::SDL_PushEvent(&user_event);
   }

   yac_host->yacMutexUnlock(mtx_event_queued);
}

void _HAL::startTimerThread(void) {

   stopTimerThread();

#ifdef YAC_WIN32
   win32_timerevent = ::CreateEvent(NULL, 1/*_bManualReset*/, 0, NULL);
   win32_timerthread = ::CreateThread(
      NULL,              // default security attributes
      0,                 // use default stack size
      tksdl_timer__threadentry,// thread function
      (LPVOID)this,      // argument to thread function
      0,                 // use default creation flags
      NULL               // returns the thread identifier
                                       );
#endif // YAC_WIN32

#if defined(YAC_LINUX) || defined(YAC_MACOS)
   (void)pthread_create(&timerthread_id, NULL, tksdl_timer__threadentry, (void*) this);
#endif // defined(YAC_LINUX) || defined(YAC_MACOS)
}

void _HAL::stopTimerThread(void) {
#ifdef YAC_WIN32
   if(NULL != win32_timerthread)
   {
      ::SetEvent(win32_timerevent);
      WaitForSingleObject(win32_timerthread, 3000); // wait for thread to exit
      CloseHandle(win32_timerthread);
      CloseHandle(win32_timerevent);
      win32_timerthread = NULL;
      win32_timerevent = NULL;
   }
#endif // YAC_WIN32

#if defined(YAC_LINUX) || defined(YAC_MACOS)
   if(b_timerthread_running)
   {
      b_timerthread_running = YAC_FALSE;

      pthread_join(timerthread_id, NULL);
   }
#endif // defined(YAC_LINUX) || defined(YAC_MACOS)
}

void _HAL::_enableKeyRepeat(sSI _d, sSI _r) {
   b_key_repeat = (_r > 0);
   // // ::SDL_EnableKeyRepeat(_d, _r);
}

void _HAL::openJoysticks(void) {
#ifdef USE_JOYSTICKS
   // ---- do not open twice ----
   if(!joysticks.enabled)
   {
      SDL_InitSubSystem(SDL_INIT_JOYSTICK);

      joysticks.enabled = YAC_TRUE;
      joysticks.num     = ::SDL_NumJoysticks();

      _Joystick *jo = new _Joystick[joysticks.num]();
      joysticks.elements = (void*)jo;

      for(sUI i = 0u; i < joysticks.num; i++)
      {
         jo[i].class_ID = clid_Joystick;
         jo[i].open(i);
      }
   }
#endif // USE_JOYSTICKS
}

void _HAL::closeJoysticks(void) {
#ifdef USE_JOYSTICKS
   if(joysticks.enabled)
   {
      joysticks.enabled = YAC_FALSE;

      _Joystick *jo = (_Joystick*) joysticks.elements;

      if(NULL != jo)
      {
         for(int i = 0; i < joysticks.num; i++)
         {
            jo[i].close();
         }

         delete [] jo;

         joysticks.elements = NULL;
         joysticks.num      = 0;
      }
   }
#endif // USE_JOYSTICKS
}


void _HAL::_exitEventLoop(void) {

   // printf("xxx HAL::exitEventLoop 1\n");

   if(b_graphics_active)
   {
      showCursor(1);
   }

   closeView();

   // printf("xxx HAL::exitEventLoop 2\n");

   stopTimerThread();

   b_running = YAC_FALSE;

   // printf("xxx HAL::exitEventLoop 3 (LEAVE)\n");
}

void _HAL::tick(void) {
   void *f = signal_funs[TKS_SIG_HALTICK];
   if(f)
   {
      yac_host->yacEvalFunction(sc_context, f, 0, 0);
   }
}

void _HAL::updateFPSAndLimit(void) {
   sSI dt;

   // (note) 'now' is set in loopDelay()

   // (note) this is called after SwapBuffers() has been called (in endPaint())
   //         nowadays GL drivers do not seem to wait for vsync in SwapBuffers() so
   //         to avoid high CPU load the thread is put to sleep for a while

   // yac_host->printf("xxx nfr=%d\n", (fps.num_frames_rendered&127));

   if(fps.limit_fps)
   {
      // dt = Delta between now (after graphics calls have been issued/swapbuffer has been called) and start of frame
      dt = _getTicks() - now;

      // yac_host->printf("xxx dt since framestart=%d\n", dt);

      if(dt < 0) dt = 0; // wrap-around

      sSI limit = 1000 / fps.limit_fps; // desired tickinterval
      if(dt < limit)
      {
#if 0 //def YAC_LINUX

         // Note: YMMV, as they say. On my relatively new Core2Duo notebook,
         //       this loop pushes the CPUload through the roof.
         //       On my old Celeron notebook, this works fine.

         sUI al = fps.last_frametime + (sUI)limit;
         while(_getTicks() < al)
         {
            // sched_yield()
            usleep(0);
         }
#else
         sUI millisecDelay = (sUI)(limit-dt);
         // if(!(fps.num_frames_rendered&127))
         // {
         //    yac_host->printf("xxx delay=%u\n", millisecDelay);
         // }
         ////::Sleep(millisecDelay);
         _delay(millisecDelay);
#endif // YAC_LINUX
      }
      else
      {
         // yac_host->printf("xxx dt=%d limit=%d\n", dt, limit);
      }

      // now = _getTicks();
      // dt  = now - fps.last_frametime;
   }

   // Delta between start of this frame / start of last frame
   dt = now - fps.last_frametime;
   // yac_host->printf("xxx dt=%d\n", dt);
   if(dt<0) dt=0; // wrap-around

   // Calculate precision and extrapolated frames per second
   if(tks_HAL->fps.b_fullframe)
   {
      dt = (sSI) (1000.0f / 60.0f);
   }

   sF32 np = ((sF32)dt) / fps.tick_interval;
   fps.precision      = fps.precision+(np-fps.precision)*0.75f;
   if(dt > 0)
   {
      fps.current_fps    = 1000.0f/((sF32)dt);
   }
   fps.last_frametime = now;
   fps.total_time    += dt;
   if(fps.num_frames_rendered > 0)
   {
      fps.average_fps    = ((sF32)fps.total_time)/((sF32)fps.num_frames_rendered);
      fps.average_fps    = 1000.0f/fps.average_fps;
   }

   // Calculate frames per second
   dt = now - fps.last_secondtime;

   if(dt < 0)
   {
      dt = 0; // wrap-around
   }

   if(dt >= 1000 && fps.second_frames_rendered)
   {
      fps.real_fps               = ((sF32)dt) / ((sF32)fps.second_frames_rendered);
      fps.real_fps               = (1000.0f / fps.real_fps);
      fps.last_secondtime        = now;
      fps.second_frames_rendered = 0;
   }
}

void _HAL::draw(void) {

   if(b_graphics_active)
   {

      if(!fps.last_frametime)
      {
         fps.last_frametime = now;
         fps.precision      = 1.0f;
      }

      if(!fps.last_secondtime)
         fps.last_secondtime = now;

      if(TKS_REDRAW_ALL == redraw_flag)
      {
         if(b_allow_redraw)
         {
            void *f = signal_funs[TKS_SIG_HALDRAW];
            if(f)
            {
               callOnIdle();

               // yac_host->printf("xxx TKS_REDRAW_ALL\n");
               beginPaint();
               // sSI dt = _getTicks();
               yac_host->yacEvalFunction(sc_context, f, 0, 0);
               /////glGetError(); // testcase, takes ~16ms w/ NVidia win32 driver (285.62, GF 9800 GT)
               // dt = _getTicks() - dt;
               // yac_host->printf("xxx onDraw took %d ms\n", dt);
               endPaint();
            }
         }
      }
      else
      {
         if(b_allow_redraw)
         {
            if(b_need_redraw)
            {
               b_need_redraw = YAC_FALSE;

               void *f = signal_funs[TKS_SIG_HALDRAW];
               if(f)
               {
                  beginPaint();
                  yac_host->yacEvalFunction(sc_context, f, 0, 0);
                  endPaint();
               }
            }
         }

         // Call idle functions 120 times per second (max)
         if( (now - fps.last_idletime) >= (1000/120) )
         {
            fps.last_idletime = now;

            callOnIdle();
         }

         //
         // (note) the Sleep(8) / SDL_Delay(8) call seems to burn a lot of cycles (!) (WinVista 32bit)
         //        CPU load was reduced from ~8% to ~2% (Syntracker2 application) simply by commenting out the following line !
         // (note) bizarre: if SDL_Delay(8) is called, CPU load goes to 100% after moving a window (resizing is OK.) wth !?
         //        w/ SDL_Delay(1) this does not happen!
         //
         _delay(1);
      }

   }
}

sBool _HAL::loopDelay(void) {

   now = _getTicks();

   ////printf("xxx now=%d last_time=%d\n", now, last_time);

   if(!last_time)
   {
      last_time = now;
      return 0;
   }
   else
   {
      if((last_time + ((sU32)(fps.tick_buffer*fps.tick_interval))) <= now)
      {
         last_time = now;
         sU16 i;
         for(i=0; i<fps.tick_buffer; i++)
         {
            tick();
         }
         return 1;
      }
      else
      {
         if((last_time+((sU32)fps.tick_interval))<=now)
         {
            while(last_time<now)
            {
               tick();
               last_time += ((sU32)fps.tick_interval);
               if((last_time+fps.tick_interval*0.75f) >= now)
               {
                  return 1;
               }
            }
            ////last_time = now;
            return 1;
         }
         else
         {
            return 0;
         }
      }
   }
}

void _HAL::initFPS(void) {
   fps.num_frames_rendered     = 0;
   fps.total_time              = 0;
   fps.last_frametime          = 0;
   fps.average_fps             = 0.0f;
   fps.current_fps             = 0.0f;
   fps.real_fps                = 0.0f;
   fps.second_time             = 0;
   fps.last_secondtime         = 0;
   fps.second_frames_rendered  = 0;
   fps.precision               = 1.0;
   fps.last_idletime           = 0;

   now = last_time = 0;

   if(::getenv("TKSDL_FULLFRAME"))
   {
      printf("[dbg] tksdl::HAL: forcing precision=1.0 (relying on vsync)\n");
      fps.b_fullframe = 1;
   }
   else
   {
      fps.b_fullframe = 0;
   }

}

void _HAL::callOnKeyboard(sU32 _vkey, sU32 _vmod, sUI _unicode) {
   // yac_host->printf("xxx HAL::callOnKeyboard: vkey=0x%08x vmod=0x%08x unicode=0x%08x\n", _vkey, _vmod, _unicode);

   // macOS: translate localized key to raw scancode
   sUI localKey = _vkey & ~VKEY_PRESSED;
   if(NULL != local_to_raw_key_table)
   {
      for(sUI i = 0u, j = 0u; i < 100u; i++, j+=2u)
      {
         if(local_to_raw_key_table[j+1u] == localKey)
         {
            if(yac_host->yacGetDebugLevel() >= 10)
               yac_host->printf("[trc] HAL::callOnKeyboard: map local vkey=%u to scancode %u\n", _vkey, local_to_raw_key_table[j+0u]);
            _vkey = local_to_raw_key_table[j+0u] | (_vkey & VKEY_PRESSED);
            break;
         }
      }
   }

   if( (0u != _vkey) || (0u != _unicode) )
   {
      _Key k;

      k.class_ID = clid_Key;
      k.vkey     = _vkey;
      k.vmod     = _vmod;
      k.unicode  = _unicode;

      YAC_FunctionHandle f = signal_funs[TKS_SIG_HALKEYBOARD];

      if(NULL != f)
      {
         YAC_Value args[1];
         args[0].initObject(&k, 0);
         yac_host->yacEvalFunction(sc_context, f, 1, args);
      }
   }
}

void _HAL::recreateGLContext(void) {

   if(NULL != sdl_glcontext)
   {
      ::SDL_GL_DeleteContext(sdl_glcontext);
      sdl_glcontext = NULL;
   }

   sdl_glcontext = ::SDL_GL_CreateContext(sdl_window);
   yac_host->printf("[dbg] tksdl::HAL::recreateGLContext: SDL_GL_CreateContext -> sdl_glcontext=%p\n", sdl_glcontext);
   ::SDL_GL_MakeCurrent(sdl_window, sdl_glcontext);
}

void _HAL::callOnOpen(void) {

   // yac_host->printf("xxx tksdl: callOnOpen ENTER\n");

#ifdef RESTORE_GL_CONTEXT
   yac_host->printf("[dbg] tksdl::HAL::callOnOpen: sdl2_gl_context=%p\n", sdl_glcontext);
#endif // RESTORE_GL_CONTEXT

   b_reset_queued_events = true;

   ::SDL_SetWindowTitle(sdl_window, (const char*)viewport_caption.chars);

   pushGLContext();

   hook_call(&tksdl_hook_open);

   YAC_FunctionHandle f = signal_funs[TKS_SIG_HALOPEN];

   if(NULL != f)
   {
      yac_host->yacEvalFunction(sc_context, f, 0, 0);
   }

   b_onclose_pending = YAC_TRUE;

#ifdef YAC_MACOS
   // restore vsync setting (lost after window zoom in macOS 13.2)
   SDL_GL_SetSwapInterval(swap_interval);
#endif // YAC_MACOS

   popGLContext();

   // yac_host->printf("xxx tksdl: callOnOpen LEAVE\n");
}

sBool _HAL::callOnClose(sBool _bAllowScriptCbk) {
   sBool ret = YAC_TRUE;

   // yac_host->printf("xxx tksdl: callOnClose pending=%d bAllowScriptCbk=%d\n", b_onclose_pending, _bAllowScriptCbk);

   if(b_onclose_pending)
   {
      YAC_FunctionHandle f = signal_funs[TKS_SIG_HALCLOSE];

      // // tkopengl_shared->_glFinish();

      pushGLContext();

      if(_bAllowScriptCbk)
      {
         if(NULL != f)
         {
            YAC_Value r;
            r.initInt(YAC_TRUE);
            ////yac_host->printf("xxx tksdl: callOnClose yacEvalFunctionReturn 1\n");
            yac_host->yacEvalFunctionReturn(sc_context, f, 0, 0, &r);
            ////yac_host->printf("xxx tksdl: callOnClose yacEvalFunctionReturn 2\n");
            ret = (YAC_FALSE != r.value.int_val);
            r.unsetFast();
         }
      }

      if(ret)
      {
         ////yac_host->printf("xxx tksdl: callOnClose hook_call 1\n");
         hook_call(&tksdl_hook_close);
         ////yac_host->printf("xxx tksdl: callOnClose hook_call 2\n");

         b_onclose_pending = YAC_FALSE;
      }

      popGLContext();
   }

   // yac_host->printf("xxx tksdl: callOnClose LEAVE\n");

   return ret;
}

void _HAL::callOnResize(void) {

   // yac_host->printf("[dbg] tksdl: callOnResize\n");

#ifdef YAC_MACOS
   recreateGLContext();
#endif // YAC_MACOS

   callOnOpen();

   YAC_FunctionHandle f = signal_funs[TKS_SIG_HALRESIZE];

   if(NULL != f)
   {
      yac_host->yacEvalFunction(sc_context, f, 0, 0);
   }

}

void _HAL::callOnExpose(void) {
   YAC_FunctionHandle f = signal_funs[TKS_SIG_HALEXPOSE];

   if(NULL != f)
   {
      yac_host->yacEvalFunction(sc_context, f, 0, 0);
   }
}

void _HAL::callOnApplicationActive(sSI _gain) {
   YAC_FunctionHandle f = signal_funs[TKS_SIG_HALAPPLICATIONACTIVE];

   // yac_host->printf("HAL::callOnApplicationActive gain=%d\n", _gain);

   // // if(!_gain)
   {
      last_vmod &= ~(VMOD_LALT|VMOD_LMETA);
   }

   if(NULL != f)
   {
      YAC_Value args[1];
      args[0].initInt(_gain);
      yac_host->yacEvalFunction(sc_context, f, 1, args);
   }
}

void _HAL::callOnMouseFocusActive(sSI _gain) {
   YAC_FunctionHandle f = signal_funs[TKS_SIG_HALMOUSEFOCUSACTIVE];

   if(NULL != f)
   {
      YAC_Value args[1];
      args[0].initInt(_gain);
      yac_host->yacEvalFunction(sc_context, f, 1, args);
   }
}

void _HAL::callOnInputFocusActive(sSI _gain) {
   YAC_FunctionHandle f = signal_funs[TKS_SIG_HALINPUTFOCUSACTIVE];

   if(NULL != f)
   {
      YAC_Value args[1];
      args[0].initInt(_gain);
      yac_host->yacEvalFunction(sc_context, f, 1, args);
   }
}

void _HAL::callOnIdle(void) {
   YAC_FunctionHandle f = signal_funs[TKS_SIG_HALIDLE];

   if(NULL != f)
   {
      yac_host->yacEvalFunction(sc_context, f, 0, 0);
   }
}

void _HAL::callOnTimer(void) {
   YAC_FunctionHandle f = signal_funs[TKS_SIG_HALTIMER];

   if(NULL != f)
   {
      yac_host->yacEvalFunction(sc_context, f, 0, 0);
   }
}

void _HAL::callOnJoyAxisMotion(sSI _dev, sSI _axis, sSI _val) {
   YAC_FunctionHandle f = signal_funs[TKS_SIG_HALJOYAXISMOTION];

   if(NULL != f)
   {
      YAC_Value args[3];
      args[0].initInt(_dev);
      args[1].initInt(_axis);
      args[2].initInt(_val);
      yac_host->yacEvalFunction(sc_context, f, 3, args);
   }
}

void _HAL::callOnJoyButton(sSI _dev, sSI _button, sSI _pressed) {
   YAC_FunctionHandle f = signal_funs[TKS_SIG_HALJOYBUTTON];

   if(NULL != f)
   {
      YAC_Value args[3];
      args[0].initInt(_dev);
      args[1].initInt(_button);
      args[2].initInt(_pressed);
      yac_host->yacEvalFunction(sc_context, f, 3, args);
   }
}

void _HAL::callOnJoyHatMotion(sSI _dev, sSI _hat, sSI _state) {
   YAC_FunctionHandle f = signal_funs[TKS_SIG_HALJOYHATMOTION];

   if(NULL != f)
   {
      YAC_Value args[3];
      args[0].initInt(_dev);
      args[1].initInt(_hat);
      args[2].initInt(_state);
      yac_host->yacEvalFunction(sc_context, f, 3, args);
   }
}

void _HAL::callOnJoyBallMotion(sSI _dev, sSI _hat, sSI _dx, sSI _dy) {
   YAC_FunctionHandle f = signal_funs[TKS_SIG_HALJOYBALLMOTION];

   if(NULL != f)
   {
      YAC_Value args[4];
      args[0].initInt(_dev);
      args[1].initInt(_hat);
      args[2].initInt(_dx);
      args[3].initInt(_dy);
      yac_host->yacEvalFunction(sc_context, f, 4, args);
   }
}

void _HAL::callOnMouse(sSI _mx, sSI _my, sSI _cstate, sSI _nstate) {
   mouse.cstate = (sU16)_cstate;
   mouse.nstate = (sU16)_nstate;

   // yac_host->printf("tksdl:HAL::callOnMouse: _mx=%i _my=%i absx=%i absy=%i cstate=%04x nstate=%04x grabbed=%i\n", _mx, _my, mouse.absx, mouse.absy, _cstate, _nstate, mouse.grabbed);

   if(mouse.b_warp_changed && (0==_nstate))
   {
      mouse.absx = tks_HAL->view_sx / 2;
      mouse.absy = tks_HAL->view_sy / 2;
      mouse.dx = 0;
      mouse.dy = 0;
      mouse.b_warp_changed = 0;
      return;
   }

   if(mouse.grabbed)
   {
      if(mouse.b_warp_changed)
      {
         // (note) not reachable
         // yac_host->printf("xxx mouse.b_warp_changed=1, m=(%d; %d) grab_start=(%d; %d)\n", _mx, _my, tks_HAL->mouse.grab_start_x, tks_HAL->mouse.grab_start_y);
         mouse.dx = 0;
         mouse.dy = 0;
         mouse.b_warp_changed = YAC_FALSE;
      }
      else if(mouse.b_grab_changed)
      {
         mouse.b_grab_changed = 0;
         // yac_host->printf("xxx mouse.b_grab_changed=1, m=(%d; %d) grab_start=(%d; %d)\n", _mx, _my, tks_HAL->mouse.grab_start_x, tks_HAL->mouse.grab_start_y);
         // // // mouse.dx = _mx - tks_HAL->mouse.grab_start_x;
         // // // mouse.dy = _my - tks_HAL->mouse.grab_start_y;
         // mouse.dx = _mx - tks_HAL->view_sx / 2;
         // mouse.dy = _my - tks_HAL->view_sy / 2;
         mouse.dx = 0;
         mouse.dy = 0;
      }
      else
      {
         // yac_host->printf("xxx mouse m=(%d; %d) grab_start=(%d; %d)\n", _mx, _my, tks_HAL->mouse.grab_start_x, tks_HAL->mouse.grab_start_y);
         mouse.dx = _mx - tks_HAL->view_sx / 2;
         mouse.dy = _my - tks_HAL->view_sy / 2;
      }

      mouse.warp_x = tks_HAL->view_sx / 2;
      mouse.warp_y = tks_HAL->view_sy / 2;
      _mx = mouse.dx;
      _my = mouse.dy;
      mouse.absx += _mx;
      mouse.absy += _my;

      if(mouse.absx < 0)
      {
         mouse.absx = 0;
      }
      else if(mouse.absx >= tks_HAL->view_sx)
      {
         mouse.absx = tks_HAL->view_sx-1;
      }
      if(mouse.absy < 0)
      {
         mouse.absy = 0;
      }
      else if(mouse.absy >= tks_HAL->view_sy)
      {
         mouse.absy = tks_HAL->view_sy-1;
      }

      _mx = mouse.absx;
      _my = mouse.absy;
   }
   else
   {
      if(mouse.b_warp_changed)
      {
         mouse.dx = 0;
         mouse.dy = 0;
         mouse.b_warp_changed = 0;
      }
      else if(mouse.b_grab_changed)
      {
         mouse.b_grab_changed = 0;
         mouse.dx = _mx - mouse.grab_start_x;
         mouse.dy = _my - mouse.grab_start_y;
      }
      else
      {
         // yac_host->printf("xxx calc mouse delta old abs=(%d;%d) new=(%d;%d)\n", mouse.absx, mouse.absy, _mx, _my);
         mouse.dx = _mx-mouse.absx;
         mouse.dy = _my-mouse.absy;
      }

      mouse.absx = _mx;
      mouse.absy = _my;
   }

   YAC_FunctionHandle f = signal_funs[TKS_SIG_HALMOUSE];

   if(NULL != f)
   {
      YAC_Value args[4];
      args[0].initInt(_mx);
      args[1].initInt(_my);
      args[2].initInt(_cstate);
      args[3].initInt(_nstate);
      yac_host->yacEvalFunction(sc_context, f, 4, args);
   }
}

void _HAL::callOnCreateOtherWindow(sUI _hwnd) {
   YAC_FunctionHandle f = signal_funs[TKS_SIG_HALCREATEOTHERWINDOW];

   if(NULL != f)
   {
      YAC_Value args[1];
      args[0].initInt(sSI(_hwnd));
      yac_host->yacEvalFunction(sc_context, f, 1, args);
   }
}

void _HAL::callOnFocusOtherWindow(sUI _hwnd) {
   YAC_FunctionHandle f = signal_funs[TKS_SIG_HALFOCUSOTHERWINDOW];

   if(NULL != f)
   {
      YAC_Value args[1];
      args[0].initInt(sSI(_hwnd));
      yac_host->yacEvalFunction(sc_context, f, 1, args);
   }
}

void _HAL::callOnMoveWindow(sSI _x, sSI _y) {
   YAC_FunctionHandle f = signal_funs[TKS_SIG_HALMOVEWINDOW];

   if(NULL != f)
   {
      YAC_Value args[2];
      args[0].initInt(_x);
      args[1].initInt(_y);
      yac_host->yacEvalFunction(sc_context, f, 2, args);
   }
}

void _HAL::callOnDrainEventQueue(void) {
   YAC_FunctionHandle f = signal_funs[TKS_SIG_HALDRAINEVENTQUEUE];
   if(NULL != f)
   {
      yac_host->yacEvalFunction(sc_context, f, 0, 0);
   }
}

sBool _HAL::_pollEvent(void) {
   // hack to work around hanging VST plugins
   SDL_Event ev;
   int r = ::SDL_PollEvent(&ev);
   return r;
}

void _HAL::drainEventQueue(void) {
   SDL_Event ev;
   while(::SDL_PollEvent(&ev))
      ;
   b_timer_event_queued  = YAC_FALSE;
   b_redraw_event_queued = YAC_FALSE;

   callOnDrainEventQueue();
}

sBool _HAL::processSDLEvent(sBool _poll) {
   if(0)
   {
      static int xxx  = 0;
      yac_host->printf("processSDLEvent %d\n", ++xxx);
   }
   SDL_Event ev;
   int r;
   if(_poll) r=::SDL_PollEvent(&ev);
   else r=::SDL_WaitEvent(&ev);

   if(r)
   {
      pushGLContext();

#if 0
      if( (24/*timer*/ != ev.type) && (SDL_USEREVENT != ev.type) && (4/*mousemove*/ != ev.type) )
      // if( (24/*timer*/ != ev.type) && (SDL_USEREVENT != ev.type) )
         yac_host->printf("[trc] got SDL event. type=%i\n", ev.type);
#endif

      sU32 vkey = 0u;
      sU32 vmod = 0u;

      switch(ev.type)
      {
#ifdef USE_JOYSTICKS
         case SDL_JOYAXISMOTION:
            callOnJoyAxisMotion(ev.jaxis.which, ev.jaxis.axis, ev.jaxis.value);
            break;

         case SDL_JOYBUTTONDOWN:
         case SDL_JOYBUTTONUP:
            callOnJoyButton(ev.jbutton.which, ev.jbutton.button, ev.jbutton.state==SDL_PRESSED);
            break;

         case SDL_JOYHATMOTION:
            callOnJoyHatMotion(ev.jhat.which, ev.jhat.hat, ev.jhat.value);
            break;

         case SDL_JOYBALLMOTION:
            callOnJoyBallMotion(ev.jball.which, ev.jball.ball, ev.jball.xrel, ev.jball.yrel);
            break;
#endif // USE_JOYSTICKS

         case SDL_WINDOWEVENT:
            switch(ev.window.event)
            {
               case SDL_WINDOWEVENT_MOVED:
                  queued_window_move_px = ev.window.data1;
                  queued_window_move_py = ev.window.data2;
                  break;

               case SDL_WINDOWEVENT_RESIZED:
               {
                  // yac_host->printf("xxx SDL_WINDOWEVENT_RESIZED (%d; %d) ignore_resize_timeout=%u ms=%u pending_resize_ms=%d\n", ev.window.data1, ev.window.data2, ignore_resize_timeout, yac_host->yacMilliSeconds(), pending_resize_ms);
                  if( (0u == ignore_resize_timeout) || (ignore_resize_timeout < yac_host->yacMilliSeconds()) )
                  {
                     if(-1 == pending_resize_ms)
                        pending_resize_ms = sSI(yac_host->yacMilliSeconds()) + 500;
                     pending_resize_w = ev.window.data1;
                     pending_resize_h = ev.window.data2;
                  }
               }
               break;

               case SDL_WINDOWEVENT_SHOWN:
                  // (note) not received on macOS when window becomes visible (again) (cmd-tab)
                  b_graphics_active = YAC_TRUE;
                  // Hack to work-around lost user events
                  b_timer_event_queued = YAC_FALSE;
                  b_redraw_event_queued = YAC_FALSE;
                  callOnApplicationActive(YAC_TRUE);
                  break;

               case SDL_WINDOWEVENT_HIDDEN:
                  callOnApplicationActive(YAC_FALSE);
                  break;

               case SDL_WINDOWEVENT_EXPOSED:
                  b_graphics_active = YAC_TRUE;
                  callOnExpose();
                  break;

               case SDL_WINDOWEVENT_ENTER:
                  if(TKS_MOUSE_POINTER_ENTER == mouse.pointer_mode)
                  {
                     mouse.auto_hide_timeout = mouse.def_auto_hide_timeout;
                     mouse.show = TKS_MOUSE_SHOW_ENABLE;
                  }
                  callOnMouseFocusActive(YAC_TRUE);
                  break;

               case SDL_WINDOWEVENT_LEAVE:
                  callOnMouseFocusActive(YAC_FALSE);
                  break;

               case SDL_WINDOWEVENT_FOCUS_GAINED:
#ifdef YAC_MACOS
                  b_graphics_active = YAC_TRUE;
                  // Hack to work-around lost user events
                  b_timer_event_queued = YAC_FALSE;
                  b_redraw_event_queued = YAC_FALSE;
                  callOnApplicationActive(YAC_TRUE);
#endif // YAC_MACOS
                  callOnInputFocusActive(YAC_TRUE);
                  break;

               case SDL_WINDOWEVENT_FOCUS_LOST:
#ifdef YAC_MACOS
                  callOnApplicationActive(YAC_FALSE);
#endif // YAC_MACOS
                  callOnInputFocusActive(YAC_FALSE);
                  break;
            } // switch ev.window.event
            break;

         case SDL_USEREVENT:
            if(TKS_EVENT_USER == ev.user.code)
            {
               if(ev.user.data1)
               {
                  // ---- deliver delayed onAction event ----
                  _triggerEvent((YAC_Object*)ev.user.data1);
                  // YAC_DELETE( ((YAC_Object*)ev.user.data1) ); // memleak! (now using event_strings instead)
                  ev.user.data1 = 0;
               }
            }
            else if(TKS_EVENT_TIMER == ev.user.code)
            {
               yac_host->yacMutexLock(mtx_event_queued);
               b_timer_event_queued = 0;
               yac_host->yacMutexUnlock(mtx_event_queued);

               callOnTimer();
            }
            else
            {
               // TKS_EVENT_REDRAW (just to wakeup from WaitEvent)
               yac_host->yacMutexLock(mtx_event_queued);
               b_redraw_event_queued = YAC_FALSE;
               yac_host->yacMutexUnlock(mtx_event_queued);
            }
            break;

         case SDL_KEYDOWN:
            // (note) KEYDOWN occurs before TEXTINPUT if key is unicode
            if(b_key_repeat || (0 == ev.key.repeat))
            {
               if(0u != last_vkey)
               {
                  // non-unicode key
                  // yac_host->printf("vkey=%08x vmod=%08x.\n", vkey, vmod);
                  callOnKeyboard(last_vkey, last_vmod, (sUI)0u/*unicode*/);
                  last_vkey = 0u;
               }

               // yac_host->printf("xxx KEYDOWN ev.key.keysym.sym=0x%08x\n", ev.key.keysym.sym);
               vkey = loc_sdl2_keycode_to_vkey(ev.key.keysym.sym) | VKEY_PRESSED;
               // yac_host->printf("xxx ev.type=%s vkey=0x%08x\n", (ev.type == SDL_KEYUP) ? "up":"down", vkey);
               vmod  = ev.key.keysym.mod & ~KMOD_NUM;
               last_vmod = vmod;
               last_vkey = vkey;
            }
            break;

         case SDL_TEXTINPUT:
            // yac_host->printf("xxx SDL_TEXTINPUT\n");
            {
               sU32 unicode = loc_utf8_to_u32((const sU8*)ev.text.text);
               if(0u != last_vkey)
               {
                  vkey = last_vkey;
                  last_vkey = 0u;
               }
               else
               {
                  vkey = 1u;
               }
               callOnKeyboard(vkey, last_vmod, unicode);
            }
            break;

         case SDL_KEYUP:
            if(b_key_repeat || (0 == ev.key.repeat))
            {
               if(0u != last_vkey)
               {
                  // non-unicode key
                  // yac_host->printf("vkey=%08x vmod=%08x.\n", vkey, vmod);
                  callOnKeyboard(last_vkey, last_vmod, (sUI)0u/*unicode*/);
                  last_vkey = 0u;
               }

               // yac_host->printf("xxx KEYUP ev.key.keysym.sym=0x%08x\n", ev.key.keysym.sym);
               vkey = loc_sdl2_keycode_to_vkey(ev.key.keysym.sym);
               // yac_host->printf("xxx ev.type=%s vkey=0x%08x\n", (ev.type == SDL_KEYUP) ? "up":"down", vkey);
               vmod  = ev.key.keysym.mod & ~KMOD_NUM;
               callOnKeyboard(vkey, vmod, (sUI)0u/*unicode*/);
            }
            break;

         case SDL_MOUSEBUTTONUP:
            switch(ev.button.button)
            {
               case SDL_BUTTON_LEFT:
                  mouse.cstate &= ~TKS_MOUSE_LBUTTON;
                  mouse.nstate  =  TKS_MOUSE_LBUTTON;
                  break;
               case SDL_BUTTON_RIGHT:
                  mouse.cstate &= ~TKS_MOUSE_RBUTTON;
                  mouse.nstate  =  TKS_MOUSE_RBUTTON;
                  break;
               case SDL_BUTTON_MIDDLE:
                  mouse.cstate &= ~TKS_MOUSE_MBUTTON;
                  mouse.nstate  =  TKS_MOUSE_MBUTTON;
                  break;
#if 0
               case SDL_BUTTON_WHEELUP:
                  mouse.cstate &= ~TKS_MOUSE_WHEELUP;
                  mouse.nstate  =  TKS_MOUSE_WHEELUP;
                  break;
               case SDL_BUTTON_WHEELDOWN:
                  mouse.cstate &= ~TKS_MOUSE_WHEELDOWN;
                  mouse.nstate  =  TKS_MOUSE_WHEELDOWN;
                  break;
#endif
            }

            if(!mouse.b_touch_input)
            {
               callOnMouse(ev.button.x, ev.button.y, mouse.cstate, mouse.nstate);
            }
            break;

         case SDL_MOUSEBUTTONDOWN:
            switch(ev.button.button)
            {
               case SDL_BUTTON_LEFT:
                  mouse.cstate |= TKS_MOUSE_LBUTTON;
                  mouse.nstate  = TKS_MOUSE_LBUTTON;
                  break;
               case SDL_BUTTON_RIGHT:
                  mouse.cstate |= TKS_MOUSE_RBUTTON;
                  mouse.nstate  = TKS_MOUSE_RBUTTON;
                  break;
               case SDL_BUTTON_MIDDLE:
                  mouse.cstate |= TKS_MOUSE_MBUTTON;
                  mouse.nstate  = TKS_MOUSE_MBUTTON;
                  break;

#if 0
               case SDL_BUTTON_WHEELUP:
                  mouse.cstate |= TKS_MOUSE_WHEELUP;
                  mouse.nstate  = TKS_MOUSE_WHEELUP;
                  break;
               case SDL_BUTTON_WHEELDOWN:
                  mouse.cstate |= TKS_MOUSE_WHEELDOWN;
                  mouse.nstate  = TKS_MOUSE_WHEELDOWN;
                  break;
#endif
            }

            // yac_host->printf("xxx SDL_MOUSEBUTTONDOWN ev.button.button=%d cstate=0x%08x nstate=0x%08x\n", ev.button.button, mouse.cstate, mouse.nstate);

            if(!mouse.b_touch_input)
            {
               callOnMouse(ev.button.x, ev.button.y, mouse.cstate, mouse.nstate);
            }
            break;

         case SDL_MOUSEWHEEL:
         {
            sSI mouseX = 0;
            sSI mouseY = 0;

            // "if you use a traditional mouse with a scroll wheel in Mac OS X and you need to scroll horizontally, all you need to do is hold down the Shift key and then use the scroll wheel" [osxdaily]
            //    ==> annoying feature, breaks (lctrl-)lshift wheel up/down e.g. in Arranger Timeline editor
#if 0
            yac_host->printf("xxx HAL: ev.wheel.x=%d ev.wheel.y=%d\n", ev.wheel.x, ev.wheel.y);
#endif
            sBool bDown = YAC_FALSE;
            sBool bUp   = YAC_FALSE;
            if(0 == ev.wheel.y)
            {
               bDown = (last_vmod & VMOD_LSHIFT) ? (ev.wheel.x > 0) : YAC_FALSE;
               bUp   = (last_vmod & VMOD_LSHIFT) ? (ev.wheel.x < 0) : YAC_FALSE;
            }
            else
            {
               bDown = (ev.wheel.y < 0);
               bUp   = (ev.wheel.y > 0);
            }

#if SDL_VERSION_ATLEAST(2,26,0)
            mouseX = ev.wheel.mouseX;
            mouseY = ev.wheel.mouseY;
#else
            // (note) Debian SDL2.0.14 package does not have these fields
            (void)SDL_GetMouseState(&mouseX, &mouseY);
#endif

            if(bDown)
            {
               mouse.cstate |= TKS_MOUSE_WHEELDOWN;
               mouse.nstate  = TKS_MOUSE_WHEELDOWN;
               callOnMouse(mouseX, mouseY, mouse.cstate, mouse.nstate);

               mouse.cstate &= ~TKS_MOUSE_WHEELDOWN;
               mouse.nstate  =  TKS_MOUSE_WHEELDOWN;
               callOnMouse(mouseX, mouseY, mouse.cstate, mouse.nstate);
            }
            else if(bUp)
            {
               mouse.cstate |= TKS_MOUSE_WHEELUP;
               mouse.nstate  = TKS_MOUSE_WHEELUP;
               callOnMouse(mouseX, mouseY, mouse.cstate, mouse.nstate);

               mouse.cstate &= ~TKS_MOUSE_WHEELUP;
               mouse.nstate  =  TKS_MOUSE_WHEELUP;
               callOnMouse(mouseX, mouseY, mouse.cstate, mouse.nstate);
            }
         }
         break;

         case SDL_MOUSEMOTION:
            if(!mouse.b_touch_input)
            {
               callOnMouse(ev.motion.x, ev.motion.y, mouse.cstate, 0);
            }

            if(!mouse.grabbed && mouse.pointer_mode==TKS_MOUSE_POINTER_MOVE)
            {
               mouse.auto_hide_timeout = mouse.def_auto_hide_timeout;
               mouse.show              = TKS_MOUSE_SHOW_ENABLE;
            }
            else
            {
            }
            break;

         case SDL_DROPBEGIN:
            yac_host->printf("xxx SDL_DROPBEGIN\n");
            drop_file_names = YAC_New_StringArray();
            break;

         case SDL_DROPFILE:
            yac_host->printf("xxx SDL_DROPFILE\n");
            if(NULL != ev.drop.file)
            {
               yac_host->printf("xxx SDL_DROPFILE: ev.drop.file=\"%s\"\n", ev.drop.file);
               if(NULL != drop_file_names)
               {
                  // Add filename copy to array
                  YAC_String s;
                  s.visit(ev.drop.file);
                  YAC_Value r;
                  r.initString(&s, 0);
                  drop_file_names->yacArraySet(sc_context, drop_file_names->num_elements, &r);
               }
               ::SDL_free(ev.drop.file);
               ev.drop.file = NULL;
            }
            break;

         case SDL_DROPTEXT:
            yac_host->printf("[trc] tksdl: SDL_DROPTEXT\n");
            break;

         case SDL_DROPCOMPLETE:
            yac_host->printf("xxx SDL_DROPCOMPLETE\n");
            if(NULL != drop_file_names)
            {
               YAC_FunctionHandle f = signal_funs[TKS_SIG_HALDROPFILES];
               if(NULL != f)
               {
                  YAC_Value args[3];
                  sSI px = 0;
                  sSI py = 0;
                  (void)SDL_GetMouseState(&px, &py);
                  args[0].initInt(px);
                  args[1].initInt(py);
                  args[2].initObject(drop_file_names, 1); // let runtime take care of array deletion
                  yac_host->yacEvalFunction(sc_context, f, 3, args);
                  drop_file_names = NULL;
               }
            }
            break;

#ifdef YAC_WIN32
         case SDL_SYSWMEVENT:
         {
            SDL_SysWMEvent *wm = &ev.syswm;
            // yac_host->printf("xxx tksdl: SDL_SYSWMEVENT: hwnd=0x%08x msg=0x%08x (%d)\n", wm->msg->hwnd, wm->msg->msg, wm->msg->msg);
            switch(wm->msg->msg.win.msg)
            {
#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))
#define GET_POINTERID_WPARAM(wParam)                (LOWORD(wParam))
#define IS_POINTER_FLAG_SET_WPARAM(wParam, flag)    (((DWORD)HIWORD(wParam) & (flag)) == (flag))
#define IS_POINTER_NEW_WPARAM(wParam)               IS_POINTER_FLAG_SET_WPARAM(wParam, POINTER_MESSAGE_FLAG_NEW)
#define IS_POINTER_INRANGE_WPARAM(wParam)           IS_POINTER_FLAG_SET_WPARAM(wParam, POINTER_MESSAGE_FLAG_INRANGE)
#define IS_POINTER_INCONTACT_WPARAM(wParam)         IS_POINTER_FLAG_SET_WPARAM(wParam, POINTER_MESSAGE_FLAG_INCONTACT)
#define IS_POINTER_FIRSTBUTTON_WPARAM(wParam)       IS_POINTER_FLAG_SET_WPARAM(wParam, POINTER_MESSAGE_FLAG_FIRSTBUTTON)
#define IS_POINTER_SECONDBUTTON_WPARAM(wParam)      IS_POINTER_FLAG_SET_WPARAM(wParam, POINTER_MESSAGE_FLAG_SECONDBUTTON)
#define IS_POINTER_THIRDBUTTON_WPARAM(wParam)       IS_POINTER_FLAG_SET_WPARAM(wParam, POINTER_MESSAGE_FLAG_THIRDBUTTON)
#define IS_POINTER_FOURTHBUTTON_WPARAM(wParam)      IS_POINTER_FLAG_SET_WPARAM(wParam, POINTER_MESSAGE_FLAG_FOURTHBUTTON)
#define IS_POINTER_FIFTHBUTTON_WPARAM(wParam)       IS_POINTER_FLAG_SET_WPARAM(wParam, POINTER_MESSAGE_FLAG_FIFTHBUTTON)
#define IS_POINTER_PRIMARY_WPARAM(wParam)           IS_POINTER_FLAG_SET_WPARAM(wParam, POINTER_MESSAGE_FLAG_PRIMARY)
#define HAS_POINTER_CONFIDENCE_WPARAM(wParam)       IS_POINTER_FLAG_SET_WPARAM(wParam, POINTER_MESSAGE_FLAG_CONFIDENCE)
#define IS_POINTER_CANCELED_WPARAM(wParam)          IS_POINTER_FLAG_SET_WPARAM(wParam, POINTER_MESSAGE_FLAG_CANCELED)
#endif

               case WM_MOVE:  // 3
               {
                  queued_window_move_px = GET_X_LPARAM(wm->msg->msg.win.lParam);
                  queued_window_move_py = GET_Y_LPARAM(wm->msg->msg.win.lParam);
               }
               break;

               case WM_POINTERDOWN:
               {
                  POINT p;
                  p.x = GET_X_LPARAM(wm->msg->msg.win.lParam);
                  p.y = GET_Y_LPARAM(wm->msg->msg.win.lParam);
                  (void)::ScreenToClient(syswminfo.info.win.window, &p);
                  // sBool bPress = (0 != IS_POINTER_FIRSTBUTTON_WPARAM(wm->msg->wParam));
                  // yac_host->printf("xxx tksdl:: WM_POINTERDOWN p=(%d; %d) bPress=%d\n", x, y, bPress);
                  mouse.cstate = 1;
                  mouse.nstate = 1;
                  callOnMouse(p.x, p.y, mouse.cstate, mouse.nstate);
               }
               break;

               case WM_POINTERUP:
               {
                  POINT p;
                  p.x = GET_X_LPARAM(wm->msg->msg.win.lParam);
                  p.y = GET_Y_LPARAM(wm->msg->msg.win.lParam);
                  (void)::ScreenToClient(syswminfo.info.win.window, &p);
                  // sBool bPress = (0 != IS_POINTER_FIRSTBUTTON_WPARAM(wm->msg->wParam));
                  // yac_host->printf("xxx tksdl:: WM_POINTERUP p=(%d; %d) bPress=%d\n", x, y, bPress);
                  mouse.cstate = 0;
                  mouse.nstate = 1;
                  callOnMouse(p.x, p.y, mouse.cstate, mouse.nstate);
               }
               break;

               case WM_POINTERENTER:
               {
                  POINT p;
                  p.x = GET_X_LPARAM(wm->msg->msg.win.lParam);
                  p.y = GET_Y_LPARAM(wm->msg->msg.win.lParam);
                  (void)::ScreenToClient(syswminfo.info.win.window, &p);
                  // sBool bPress = (0 != IS_POINTER_FIRSTBUTTON_WPARAM(wm->msg->wParam));
                  // yac_host->printf("xxx tksdl:: WM_POINTERENTER p=(%d; %d) bPress=%d\n", x, y, bPress);
                  // mouse.nstate = bPress;
                  // callOnMouse(x, y, mouse.cstate, mouse.nstate);
                  // mouse.cstate = mouse.nstate;
               }
               break;

               case WM_POINTERLEAVE:
               {
                  POINT p;
                  p.x = GET_X_LPARAM(wm->msg->msg.win.lParam);
                  p.y = GET_Y_LPARAM(wm->msg->msg.win.lParam);
                  (void)::ScreenToClient(syswminfo.info.win.window, &p);
                  // sBool bPress = (0 != IS_POINTER_FIRSTBUTTON_WPARAM(wm->msg->wParam));
                  // yac_host->printf("xxx tksdl:: WM_POINTERLEAVE p=(%d; %d) bPress=%d\n", x, y, bPress);
               }
               break;

               case WM_POINTERUPDATE:
               {
                  POINT p;
                  p.x = GET_X_LPARAM(wm->msg->msg.win.lParam);
                  p.y = GET_Y_LPARAM(wm->msg->msg.win.lParam);
                  (void)::ScreenToClient(syswminfo.info.win.window, &p);
                  // sBool bPress = (0 != IS_POINTER_FIRSTBUTTON_WPARAM(wm->msg->wParam));
                  // yac_host->printf("xxx tksdl:: WM_POINTERUPDATE p=(%d; %d) bPress=%d\n", x, y, bPress);
                  callOnMouse(p.x, p.y, mouse.cstate, 0);
               }
               break;

               case WM_DROPFILES:
               {
                  HDROP hDrop = (HDROP) wm->msg->msg.win.wParam;
                  POINT p;
                  if(TRUE == ::DragQueryPoint(hDrop, &p))
                  {
                     ////printf("xxx tksdl: WM_DROPFILES: p=(%d; %d)\n", p.x, p.y);

                     UINT numFiles = ::DragQueryFile(hDrop, 0xffffffff, NULL, 0);
                     ////printf("xxx tksdl: WM_DROPFILES: dropped %u file(s)\n", numFiles);
                     if(numFiles > 0)
                     {
                        static char buf[MAX_PATH];
                        UINT i;

                        YAC_FunctionHandle f = signal_funs[TKS_SIG_HALDROPFILES];

                        if(NULL != f)
                        {
                           YAC_StringArray *a = YAC_New_StringArray();
                           a->yacArrayAlloc(numFiles, 0,0,0);
                           a->num_elements = a->max_elements;
                           for(i=0; i<numFiles; i++)
                           {
                              ::DragQueryFile(hDrop, i, buf, MAX_PATH);
                              ////printf("xxx tksdl: WM_DROPFILES: file[%d] = \"%s\"\n", i, buf);

                              YAC_Value r;
                              a->yacArrayGet(sc_context, i, &r);
                              YAC_String *s= r.value.string_val;
                              s->copy(buf);
                           }
                           YAC_Value args[3];
                           args[0].initInt(p.x);
                           args[1].initInt(p.y);
                           args[2].initObject(a, 1); // let runtime take care of array deletion
                           yac_host->yacEvalFunction(sc_context, f, 3, args);
                        }
                     }
                  }
               }
               break;

               // case WM_DEVICECHANGE:

#if 0
               case 0x2cc:  // WM_TABLET_FIRST+0xc, sent when touchscreen is touched
                  break;
#endif

            }
         }
         break;
#endif // YAC_WIN32

         case SDL_QUIT:
            // printf("xxx SDL_QUIT: hook_call hook=0x%p\n", &tksdl_hook_close);

            if(callOnClose(YAC_TRUE/*bAllowScriptCbk*/))
            {
               b_graphics_active = YAC_FALSE;

               // printf("xxx SDL_QUIT: stopTimerThread 1\n");

               stopTimerThread();

               // printf("xxx SDL_QUIT: stopTimerThread 2\n");

               b_running = YAC_FALSE;
            }
            break;

         default:
            break;
      } // switch ev.type

      popGLContext();
      return YAC_TRUE;
   }
   return YAC_FALSE;
}

void _HAL::resetMouse(void) {
   mouse.absx           = 0;
   mouse.absy           = 0;
   mouse.dx             = 0;
   mouse.dy             = 0;
   mouse.cstate         = 0;
   mouse.nstate         = 0;
   mouse.grabbed        = YAC_FALSE;
   mouse.grab_start_x   = 0;
   mouse.grab_start_y   = 0;
   mouse.b_warp_changed = YAC_FALSE;
   mouse.warp_x         = -1;
   mouse.warp_y         = -1;
}

void _HAL::_eventLoop(void) {
   static sU32 last_ticks = 0u;

   b_running = 1;
   last_time = 0;
   now       = 0;

   // Start timer
   if(timer_interval > 0)
   {
      startTimerThread();
   }

   resetMouse();

   sUI numEventsSinceRedraw = 0u;

   while(b_running)
   {
      b_running = yac_host->yacRunning();

      if(b_graphics_active)
      {
         // invoke trace function, redraws rt-graphics screen

         if(loopDelay())
         {
            if(b_running)
            {
               if(mouse.warp_x >= 0)
               {
                  // yac_host->printf("xxx SDL_WarpMouse to %d %d\n", mouse.warp_sx, mouse.warp_sy);
                  // // ::SDL_WarpMouse(mouse.warp_sx, mouse.warp_sy);
                  ::SDL_WarpMouseInWindow(sdl_window, mouse.warp_x, mouse.warp_y);
                  mouse.warp_x         = -1;
                  mouse.b_warp_changed = YAC_TRUE;
               }

               //yac_host->printf("xxx processSDLEvent polling=%i\n", b_event_polling);
               if(processSDLEvent(b_event_polling))
               {
                  do
                  {
                     numEventsSinceRedraw++; // Hack to refresh the screen if the event queue is flooded with mouse events

                     if(numEventsSinceRedraw >= 8)
                     {
                        numEventsSinceRedraw = 0u;

                        if(b_need_redraw)
                        {
                           draw();
                        }
                     }

                     // Note: hack to keep the cursor disabled/enabled
                     if(TKS_MOUSE_SHOW_ENABLE == mouse.show)
                     {
                        showCursor(YAC_TRUE);
                     }
                     else if(TKS_MOUSE_SHOW_DISABLE == mouse.show)
                     {
                        showCursor(YAC_FALSE);
                     }

                  } while( b_running && processSDLEvent(YAC_TRUE/*bPoll*/) );
               } // if processSDLEvent()
               else
               {
                  // Does not necessarely draw anything but
                  //  also handles the# delay (via usleep) until the next frame has been reached (REDRAW_ALL).
                  //
                  //  In "dirty" redraw mode, this will call onDraw() if a redraw request is pending.
                  if(TKS_REDRAW_ALL != redraw_flag)
                  {
                     if(b_need_redraw)
                     {
                        draw();
                     }
                  }
               }

               if(0u != last_vkey)
               {
                  // non-unicode key
                  // yac_host->printf("vkey=%08x vmod=%08x.\n", vkey, vmod);
                  callOnKeyboard(last_vkey, last_vmod, (sUI)0u/*unicode*/);
                  last_vkey = 0u;
               }

               // Mouse pointer auto-hiding
               sU32 delta_ticks;
               if(!last_ticks)
               {
                  last_ticks = _getTicks();
                  delta_ticks=0;
               }
               else
               {
                  delta_ticks = last_ticks;
                  last_ticks  = _getTicks();
                  delta_ticks = last_ticks - delta_ticks;
               }

               if(mouse.auto_hide_timeout)
               {
                  mouse.auto_hide_timeout -= delta_ticks;
                  if(((sSI)mouse.auto_hide_timeout)<=0)
                  {
                     mouse.auto_hide_timeout = 0;
                     mouse.show = TKS_MOUSE_SHOW_DISABLE;
                  }
               }

               if(b_reset_queued_events)
               {
                  // after resizing/re-opening view
                  //  (fix/workaround stuck events on macOS)
                  yac_host->yacMutexLock(mtx_event_queued);
                  b_reset_queued_events = YAC_FALSE;
                  b_timer_event_queued  = YAC_FALSE;
                  b_redraw_event_queued = YAC_FALSE;
                  yac_host->yacMutexUnlock(mtx_event_queued);
               }

               if(-1 != pending_resize_ms)
               {
                  if(sSI(yac_host->yacMilliSeconds()) >= pending_resize_ms)
                  {
                     pending_resize_ms = -1;
                     sU16 w = (sU16)sMAX(pending_resize_w, (sSI)min_w);
                     sU16 h = (sU16)sMAX(pending_resize_h, (sSI)min_h);
                     // yac_host->printf("xxx resize s=(%u; %u) min=(%u; %u)\n", w, h, min_w, min_h);

                     if(pending_resize_w < (sSI)min_w || pending_resize_h < (sSI)min_h)
                     {
                        // (note) causes window to revert to maximized state (with visible dock)
                        int x = -1, y;
                        if(NULL != sdl_window)
                        {
                           SDL_GetWindowPosition(sdl_window, &x, &y);
                           closeView();
                        }
                        openView(w, h, 0/**z**/, 0/**fs**/, 1/**resize**/);
                        if(-1 != x)  // (todo) window position may have changed after resize
                           SDL_SetWindowPosition(sdl_window, x, y);
                        callOnResize();
                     }
                     else
                     {
                        // simply update the viewport size (tested on macOS)
                        {
                           int w = 0, h = 0;
                           ::SDL_GetWindowSize(sdl_window, &w, &h);
                           view_sx = (sU16)w;
                           view_sy = (sU16)h;
                        }
                        callOnResize();
                     }
                  }
               }

            } // if b_running
         } // if loopDelay

         numEventsSinceRedraw = 0u;
         draw();

         if(-1 != queued_window_move_px)
         {
            static int last_wm_move_x = -1;
            static int last_wm_move_y;
            if(queued_window_move_px != last_wm_move_x || queued_window_move_py != last_wm_move_y)
            {
               last_wm_move_x = queued_window_move_px;
               last_wm_move_y = queued_window_move_py;
               callOnMoveWindow(last_wm_move_x, last_wm_move_y);
            }
            queued_window_move_px = -1;
            queued_window_move_py = -1;
         }

      } // if b_graphics_active
      else
      {
         // ---- no graphics output ----
         _delay(10);
      }
   } // while b_running

   ///////(void)callOnClose();
}

void _HAL::_reportJoystickEvents(sSI _b) {
#ifdef USE_JOYSTICKS
   ::SDL_JoystickEventState(_b?SDL_ENABLE:SDL_DISABLE);
#endif // USE_JOYSTICKS
}

void _HAL::_delay(sSI _to) {
   ::SDL_Delay((sU32)_to);
}

sUI _HAL::_getTicks(void) {
   /* (todo) use yac_host->yacMilliSeconds() */
#ifdef YAC_LINUX
   // Get current time and calc delta to window open
   struct timeval tv;
   gettimeofday(&tv, NULL);
   tv.tv_sec  = tv.tv_sec  - tv_start.tv_sec;
   tv.tv_usec = tv.tv_usec - tv_start.tv_usec;
   return (sUI) (tv.tv_sec*1000 + (tv.tv_usec / 1000));
#else
   return ::SDL_GetTicks();
#endif // YAC_LINUX
}

void _HAL::_setRedrawFlag(sSI _fl) {
   // yac_host->printf("xxx setRedrawFlag fl=%d\n", _fl);
   redraw_flag = (sU8)_fl;
}

sSI _HAL::_getRedrawFlag(void) {
   return redraw_flag;
}

void _HAL::loadGLDefaults(void) {
   sBool bGLCompat = !b_glcore;

   if(Dyac_host_yacGetDebugLevel() >= 2u)
   {
      Dyac_host_printf("[trc] HAL::loadGLDefaults ENTER bGLCompat=%d\n", bGLCompat);
   }

   tkopengl_shared->_glDisable(GL_BLEND);

   if(bGLCompat)
      tkopengl_shared->_glDisable(GL_ALPHA_TEST);

#if !defined(DX_GLES)
   if(bGLCompat)
      tkopengl_shared->_glDisable(GL_COLOR_LOGIC_OP);
#endif // DX_GLES

   if(bGLCompat)
      tkopengl_shared->_glDisable(GL_COLOR_MATERIAL);

   tkopengl_shared->_glDisable(GL_CULL_FACE);
   tkopengl_shared->_glDisable(GL_DEPTH_TEST);
   tkopengl_shared->_glDisable(GL_SCISSOR_TEST);
   tkopengl_shared->_glDisable(GL_STENCIL_TEST);
   tkopengl_shared->_glDepthFunc(GL_LESS);
   tkopengl_shared->_glClearDepth(1.0f);
   tkopengl_shared->_glDisable(GL_DITHER);

   if(bGLCompat)
      tkopengl_shared->_glDisable(GL_FOG);

#if !defined(DX_GLES)
   if(bGLCompat)
      tkopengl_shared->_glDisable(GL_INDEX_LOGIC_OP);
#endif // DX_GLES

   if(bGLCompat)
   {
      tkopengl_shared->_glDisable(GL_LIGHT0);
      tkopengl_shared->_glDisable(GL_LIGHT1);
      tkopengl_shared->_glDisable(GL_LIGHT2);
      tkopengl_shared->_glDisable(GL_LIGHT3);
      tkopengl_shared->_glDisable(GL_LIGHT4);
      tkopengl_shared->_glDisable(GL_LIGHT5);
      tkopengl_shared->_glDisable(GL_LIGHT6);
      tkopengl_shared->_glDisable(GL_LIGHT7);
      tkopengl_shared->_glDisable(GL_LIGHTING);

      tkopengl_shared->_glDisable(GL_LINE_STIPPLE);
   }

   tkopengl_shared->_glDisable(GL_LINE_SMOOTH);

   // tkopengl_shared->_glDisable(GL_MAP1_COLOR_4);
   // tkopengl_shared->_glDisable(GL_MAP1_INDEX);
   // tkopengl_shared->_glDisable(GL_MAP1_NORMAL);
   // tkopengl_shared->_glDisable(GL_MAP1_TEXTURE_COORD_1);
   // tkopengl_shared->_glDisable(GL_MAP1_TEXTURE_COORD_2);
   // tkopengl_shared->_glDisable(GL_MAP1_TEXTURE_COORD_3);
   // tkopengl_shared->_glDisable(GL_MAP1_TEXTURE_COORD_4);
   // tkopengl_shared->_glDisable(GL_MAP1_VERTEX_3);
   // tkopengl_shared->_glDisable(GL_MAP1_VERTEX_4);
   // tkopengl_shared->_glDisable(GL_MAP2_COLOR_4);
   // tkopengl_shared->_glDisable(GL_MAP2_INDEX);
   // tkopengl_shared->_glDisable(GL_MAP2_NORMAL);
   // tkopengl_shared->_glDisable(GL_MAP2_TEXTURE_COORD_1);
   // tkopengl_shared->_glDisable(GL_MAP2_TEXTURE_COORD_2);
   // tkopengl_shared->_glDisable(GL_MAP2_TEXTURE_COORD_3);
   // tkopengl_shared->_glDisable(GL_MAP2_TEXTURE_COORD_4);
   // tkopengl_shared->_glDisable(GL_MAP2_VERTEX_3);
   // tkopengl_shared->_glDisable(GL_MAP2_VERTEX_4);

#ifndef DX_GLES
   if(bGLCompat)
      tkopengl_shared->_glDisable(GL_NORMALIZE);
#endif // DX_GLES

   if(bGLCompat)
   {
      tkopengl_shared->_glDisable(GL_POINT_SMOOTH);
      tkopengl_shared->_glDisable(GL_POINT_SPRITE);

      tkopengl_shared->_glDisable(GL_POLYGON_SMOOTH);
      tkopengl_shared->_glDisable(GL_POLYGON_STIPPLE);
   }

   // tkopengl_shared->_glDisable(GL_POLYGON_OFFSET_FILL);
   // tkopengl_shared->_glDisable(GL_POLYGON_OFFSET_LINE);
   // tkopengl_shared->_glDisable(GL_POLYGON_OFFSET_POINT);

   if(bGLCompat)
   {
      tkopengl_shared->_glDisable(GL_TEXTURE_1D);
      tkopengl_shared->_glDisable(GL_TEXTURE_2D);
      tkopengl_shared->_glDisable(GL_TEXTURE_3D);

      tkopengl_shared->_glDisable(GL_TEXTURE_GEN_Q);
      tkopengl_shared->_glDisable(GL_TEXTURE_GEN_R);
      tkopengl_shared->_glDisable(GL_TEXTURE_GEN_S);
      tkopengl_shared->_glDisable(GL_TEXTURE_GEN_T);
   }

   if(bGLCompat)
   {
      ////glShadeModel(GL_FLAT);
      tkopengl_shared->_glShadeModel(GL_SMOOTH);
      tkopengl_shared->_glHint(GL_FOG_HINT, GL_NICEST);

      tkopengl_shared->_glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
      tkopengl_shared->_glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
   }

   tkopengl_shared->_glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
   tkopengl_shared->_glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

   tkopengl_shared->_glBindFramebuffer(GL_FRAMEBUFFER, 0);
   tkopengl_shared->_glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
   tkopengl_shared->_glDepthMask(GL_TRUE);

   tkopengl_shared->_glDisable(GL_SAMPLE_COVERAGE);

   // yac_host->printf("xxx HAL::loadGLDefaults LEAVE\n");
}

void _HAL::beginPaint(void) {
   sBool bGLCompat = !b_glcore;

   if(!b_redrawing)
   {
      // // SDL_GLContext_Rebind(sdl_window);  // hack for Softube and Waves VST plugin UIs
      (void)SDL_GL_MakeCurrent(sdl_window, sdl_glcontext);

      // Import shared functions from tkopengl
      tkopengl_shared_resolve();
      tkopengl_shared->_zglEnableGLCore(b_glcore);

      pushGLContext();

#ifdef YAC_MACOS
      if(0)
      {
         int rb = -9999; ::glGetIntegerv(GL_READ_BUFFER, &rb);
         int db = -9999; ::glGetIntegerv(GL_DRAW_BUFFER, &db);
         yac_host->printf("xxx beginPaint: rb=%d db=%d\n", rb, db);
         // => initial rb=36064 db=36064   == GL_COLOR_ATTACHMENT0 0x8CE0. see USE_MACOS_FRAMEBUFFER_WORKAROUND in tkopengl
         //     (note) glReadPixels does not work on macOS
      }
#endif // YAC_MACOS

      loadGLDefaults();

#ifndef DX_GLES
      if(!b_glcore && tks_HAL->b_double_buffer)
      {
         tkopengl_shared->_glDrawBuffer(GL_BACK);  // !b_glcore
      }
#endif // DX_GLES

      tkopengl_shared->_glBindFramebuffer(GL_FRAMEBUFFER, 0);  // [13Feb2018]
      tkopengl_shared->_glDisable(GL_SCISSOR_TEST);

      tkopengl_shared->_glViewport(0, 0, tks_HAL->view_sx, tks_HAL->view_sy);

#if !defined(DX_GLES) || defined(DX_GLES_GLEMU)
      if(bGLCompat)
      {
         tkopengl_shared->_glMatrixMode(GL_TEXTURE);
         tkopengl_shared->_glLoadIdentity();

         tkopengl_shared->_glMatrixMode(GL_MODELVIEW);
         tkopengl_shared->_glLoadIdentity();
      }
#endif // DX_GLES

      b_redrawing = YAC_TRUE;
   }
}

void _HAL::endPaint(void) {
   if(b_redrawing)
   {
      if(b_graphics_active)
      {
         //
         // (note) glFlush() call is **NOT** superfluous, at least not with desktop opengl
         //       (commenting out this call breaks the tkui library!)
         //

         //
         // (note) NVidia SwapBuffers() blocks when vsync is enabled.
         //       ATI's implementation seems to just queue the commands and return immediately.
         //       Unless the framerate is limited (FPS.limit=60;), this means that more
         //       frames are queued for rendering than are made visible.
         //       The solution to this is to synchronize with the GL and wait for the last
         //       SwapBuffers() to finish before queueing a new SwapBuffers call.
         //
         // (update) 29Dec2011: apparently the NVidia SwapBuffers behaviour changed, SwapBuffers() does not block anymore
         //                     (tested on Geforce 9800 GT / WinVista 32bit, latest drivers)
         //

         // sUI tickStart = _getTicks();

         ::glFlush();

#ifdef DX_GLES
         // (todo) generic GLES swapbuffers
         yac_host->printf("[~~~] HAL::endPaint: missing swapBuffers call\n");
#else
         // yac_host->printf("xxx swap\n");
         ::SDL_GL_SwapWindow(sdl_window);  // may block (NVidia) or simply queue and return (ATI)
#endif // DX_GLES

         // // ::SDL_Delay(1);

          // sUI dt = _getTicks() - tickStart;
          // yac_host->printf("[dbg] SwapBuffers took %u ticks\n", dt); // => 0 millisec on

         // Increase framecounter
         fps.second_frames_rendered++;
         fps.num_frames_rendered++;

         updateFPSAndLimit();
      }

      popGLContext();

      b_redrawing = YAC_FALSE;
   }
}

void _HAL::pushGLContext(void) {
#ifdef _WIN32
   // (note) needs patched SDL2 library from tksdl2/SDL2-2.32.4/ folder
   SDL_GL_Context_Push(sdl_window);  // save previous GL context (e.g. Softube and Waves VST plugin UIs)
#endif // _WIN32

#ifdef RESTORE_GL_CONTEXT
   (void)SDL_GL_MakeCurrent(SDL_GL_GetCurrentWindow(), sdl_glcontext);
#endif // RESTORE_GL_CONTEXT
}

void _HAL::popGLContext(void) {
#ifdef _WIN32
   // (note) needs patched SDL2 library from tksdl2/SDL2-2.32.4/ folder
   SDL_GL_Context_Pop(sdl_window);  // restore previous GL context (e.g. Softube and Waves VST plugin UIs)
#endif // _WIN32

#ifdef RESTORE_GL_CONTEXT
   (void)SDL_GL_MakeCurrent(sdl_window, sdl_glcontext);
#endif // RESTORE_GL_CONTEXT
}

sBool _HAL::openWindow(sSI _sx, sSI _sy) {
   win_sx = (sU16)_sx;
   win_sy = (sU16)_sy;

   b_fullscreen = 0;

   return openView(_sx, _sy, 0, 0, 0);
}

sBool _HAL::openScreen(sSI _sx, sSI _sy, sSI _z) {
   fs_sx = (sU16)_sx;
   fs_sy = (sU16)_sy;

   if(!win_sx)
   {
      win_sx = fs_sx;
      win_sy = fs_sy;
   }

   fs_z         = (sU8)_z;
   b_fullscreen = 1;

   return openView(_sx, _sy, _z, 1, 0);
}

sBool _HAL::openView(sU16 _sx, sU16 _sy, sU8 _z,
                     sBool _bFullscreen, sBool _bResize
                     ) {

   if(yac_host->yacGetDebugLevel() >= 1)
      yac_host->printf("[dbg] HAL::openView: s=(%u; %u) z=%u bFS=%d bResize=%d\n",
                       _sx, _sy, _z, _bFullscreen, _bResize
                       );

   SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");

   if(!_bResize)
   {
      // close previously opened window. calls onClose()
      closeView();

      const char *viewSize = ::getenv("TKSDL_VIEWSIZE");
      if(NULL != viewSize)
      {
         sUI vsx = 0;
         sUI vsy = 0;
         sscanf(viewSize, "%u;%u", &vsx, &vsy);
         if(yac_host->yacGetDebugLevel() >= 1)
            yac_host->printf("[...] tksdl::HAL::openView: override view size=(%u; %u)\n", vsx, vsy);
         _sx = (sU16)vsx;
         _sy = (sU16)vsy;
      }
   }
   else
   {
      // just resize the current window
      callOnClose(YAC_FALSE/*bAllowScriptCbk*/);
   }

   // SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");

   if(!::SDL_WasInit(SDL_INIT_VIDEO))
   {
      if(yac_host->yacGetDebugLevel() >= 2)
         yac_host->printf("[dbg] tksdl::HAL::openView: SDL_InitSubSystem(SDL_INIT_VIDEO)\n");
      ::SDL_InitSubSystem(SDL_INIT_VIDEO);
      if(yac_host->yacGetDebugLevel() >= 2)
         yac_host->printf("[dbg] tksdl::HAL::openView: SDL_InitSubSystem(SDL_INIT_VIDEO) ..DONE\n");
   }

   view_sx = _sx;
   view_sy = _sy;

#ifdef USE_ICONS
   if(0)
      loc_set_icon(sdl_window);
#endif // USE_ICONS

   // ::SDL_SetWindowTitle(sdl_window, "tksdl window");//, "tksdl"/*icon*/);

   sUI flags = SDL_WINDOW_OPENGL;

   ////flags |= SDL_HWSURFACE | SDL_ASYNCBLIT | SDL_HWACCEL;

   if(_bFullscreen)
   {
      view_z  = _z;
      // flags  |= SDL_WINDOW_FULLSCREEN;
      flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;  // SDL2 (don't trap mouse)
   }
   else
   {
      SDL_DisplayMode DM;
      int r = SDL_GetCurrentDisplayMode(0, &DM);
      (void)r;
      desktop_sx = (sSI)DM.w;
      desktop_sy = (sSI)DM.h;
      // (todo) DM.format, SDL_PixelFormatEnumToMasks()
      view_z = SDL_BITSPERPIXEL(DM.format);

      if(yac_host->yacGetDebugLevel() > 7)
      {
         yac_host->printf("\n[...] openView: desktop resolution=(%i; %i) color depth=%ibit\n",
                          desktop_sx,
                          desktop_sy,
                          view_z
                          );
      }
   }

   // //if(!_bResize)
   {
      ::SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );   // require at least 5 bits per channel
      ::SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5);
      ::SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5);
      ::SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16);
      ::SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, b_double_buffer);

      // Force GL 4.x core profile (no glBegin/glEnd, ..)
      //   (note) returns GL 4.1 context on macOS 15.x ("#version 410 core\n" shaders)
      {
         char *s = ::getenv("TKSDL_GLCORE");
         if(NULL != s)
         {
            b_glcore = atoi(s) ? 1 : 0;
            if(yac_host->yacGetDebugLevel() >= 1)
               yac_host->printf("[dbg] tksdl::HAL: %s GL 4.x core profile\n", b_glcore ? "enabling" : "disabling");
         }

         if(b_glcore)
         {
            ::SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
            ::SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
            ::SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
         }
      }

      // yac_host->printf("xxx tksdl: multisample_buffers=%u multisample_samples=%u\n", multisample_buffers, multisample_samples);
      if( (multisample_buffers>0) && (multisample_samples>0) )
      {
         ::SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, multisample_buffers);
         ::SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, multisample_samples);
      }
      if(stencil_bits > 0)
      {
         ::SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, stencil_bits);
      }
   }

   sUI vpFlags = 0u;
   if(sdl1_flags & 16/*SDL_RESIZABLE*/)
      vpFlags |= SDL_WINDOW_RESIZABLE;
   if(sdl1_flags & 32/*SDL_NOFRAME*/)
      vpFlags |= SDL_WINDOW_BORDERLESS;

   if(yac_host->yacGetDebugLevel() >= 1)
      yac_host->printf("[dbg] tksdl::HAL::openView: SDL_CreateWindow(%u, %u, %u, 0x%08x)\n",
                       view_sx, view_sy,
                       view_z,
                       flags | vpFlags
                       );

   flags |= SDL_WINDOW_SHOWN;

   sdl_window = ::SDL_CreateWindow("tksdl",
                                   SDL_WINDOWPOS_UNDEFINED,
                                   SDL_WINDOWPOS_UNDEFINED,
                                   view_sx, view_sy,
                                   flags | vpFlags
                                   );

   if(yac_host->yacGetDebugLevel() >= 2)
      yac_host->printf("[dbg] tksdl::HAL::openView: SDL_CreateWindow -> sdl_window=%p\n", sdl_window);

   if(NULL == sdl_window)
   {
      yac_host->printf("\n[---] SDL_CreateWindow(%i, %i, %i, %08x) failed.\n",
                       view_sx, view_sy, view_z, flags
                       );
      return YAC_FALSE;
   }

   sdl_glcontext = ::SDL_GL_CreateContext(sdl_window);
   if(yac_host->yacGetDebugLevel() >= 2)
      yac_host->printf("[dbg] tksdl::HAL::openView: SDL_GL_CreateContext -> sdl_glcontext=%p\n", sdl_glcontext);
   ::SDL_GL_MakeCurrent(sdl_window, sdl_glcontext);

   // workaround for very small windows
   {
      int w = 0, h = 0;
      ::SDL_GetWindowSize(sdl_window, &w, &h);
      view_sx = (sU16)w;
      view_sy = (sU16)h;
   }

   ////yac_host->printf("[...] SDL_SetViewMode() OK: surface geo=(%i; %i) pitch=%i clipw=%i cliph=%i glh=%i glw=%i.\n", view_sx, view_sy, surf->pitch, surf->clip_rect.w, surf->clip_rect.h, clipbox[2], clipbox[3]);

   // ---- video mode is set
   if(_bFullscreen)
   {
      view_flags |= _HAL::VP_FULLSCREEN;
   }

   view_flags |= _HAL::VP_OPENED;

#ifdef YAC_WIN32
   ::InitCommonControls();
#endif

   // yac_host->printf("xxx HAL::openView: glGetError=%d\n", glGetError());

   viewOpened(_bResize);

   // OK
   return YAC_TRUE;
}

void _HAL::closeView(void) {

   if(0 != (view_flags & VP_OPENED))
   {
      if(yac_host->yacGetDebugLevel() >= 2)
         yac_host->printf("[dbg] HAL::closeView: sdl_window=%p sdl_glcontext=%p\n", sdl_window, sdl_glcontext);
   }

   if(0 != (view_flags & VP_OPENED))
   {
      // yac_host->printf("[dbg] HAL::closeView: callOnClose(0)\n");

      callOnClose(YAC_TRUE/*bAllowScriptCbk*/);

      view_sx           = 0;
      view_sy           = 0;
      view_z            = 0;
      view_flags        = 0;
      b_graphics_active = YAC_FALSE;
   }

   if(NULL != sdl_window)
   {
      ::SDL_DestroyWindow(sdl_window);
      sdl_window = NULL;
   }

   if(NULL != sdl_glcontext)
   {
      ::SDL_GL_DeleteContext(sdl_glcontext);
      sdl_glcontext = NULL;
   }

#if 1
   // [23Mar2018] actually close window when Viewport.close() is called
   if(::SDL_WasInit(SDL_INIT_VIDEO))
   {
      if(yac_host->yacGetDebugLevel() >= 1)
         yac_host->printf("[dbg] tksdl::HAL::closeView: SDL_QuitSubSystem(SDL_INIT_VIDEO)\n");
      ::SDL_QuitSubSystem(SDL_INIT_VIDEO);
   }
#endif

   b_window_visible = false;

#ifdef YAC_WIN32
   if(lpDD)
   {
      lpDD->Release();
      lpDD = NULL;
   }
#endif

   // yac_host->printf("[dbg] HAL::closeView (LEAVE)\n");
}

void _HAL::viewOpened(sBool _bResize) {

   ignore_resize_timeout = yac_host->yacMilliSeconds() + 1000;
   pending_resize_ms = -1;

   b_window_visible = YAC_TRUE;

   SDL_VERSION(&syswminfo.version);
   if(::SDL_GetWindowWMInfo(sdl_window, &syswminfo))
   {
#ifdef YAC_MACOS
      if(yac_host->yacGetDebugLevel() >= 2)
         yac_host->printf("[dbg] tksdl::HAL::viewOpened<macos>: syswminfo.info.cocoa.window=%llu\n", (size_t)syswminfo.info.cocoa.window);
#endif // YAC_MACOS
   }

#ifdef YAC_WIN32
   if(lpDD)
   {
      lpDD->Release();
   }
   // ---- get monitor refresh rate ----
   ::DirectDrawCreate((LPGUID)0, &lpDD, NULL);
   lpDD->GetMonitorFrequency(&dw_monitorfreq);
   if(yac_host->yacGetDebugLevel() >= 2)
   {
      yac_host->printf("[...] Monitor frequency=%iHz\n", dw_monitorfreq);
   }

   // Enable drag'n'drop
   SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);
   ::DragAcceptFiles(syswminfo.info.win.window, TRUE);
#endif

   resetMouse();

   b_graphics_active = YAC_TRUE;

   // show GL driver info
   if(yac_host->yacGetDebugLevel() >= 2)
   {
      const GLubyte *gls = ::glGetString(GL_VENDOR);
      yac_host->printf("[...] OpenGL vendor    : \"%s\"\n", (const char*)gls);
      gls = ::glGetString(GL_RENDERER);
      yac_host->printf("[...]        renderer  : \"%s\"\n", (const char*)gls);
      gls = ::glGetString(GL_VERSION);
      yac_host->printf("[...]        version   : \"%s\"\n", (const char*)gls);

      // (note) GLES2.x: must use glGetString (space separated list)
      // (note) GLES3.x+: must use glGetStringi
#ifdef DX_GL4_CORE
      if(b_glcore)
      {
         yac_host->printf("[...]        extensions: \"");
         sUI extIdx = 0u;
         for(;;)
         {
            gls = ::glGetStringi(GL_EXTENSIONS, extIdx);
            if(NULL != gls)
            {
               if(extIdx > 0u)
                  yac_host->printf(" ");
               yac_host->printf("%s", (const char*)gls);
               // Next extension
               extIdx++;
            }
            else
            {
               (void)::glGetError();
               break;
            }
         }
         yac_host->printf("\"\n", (const char*)gls);
      }
      else
      {
         gls = ::glGetString(GL_EXTENSIONS);
         // yac_host->printf("xxx HAL::viewOpened: 1d glGetError=%d\n", glGetError());
         yac_host->printf("[...]        extensions: \"%s\"\n", (const char*)gls);
      }
#else
      gls = ::glGetString(GL_EXTENSIONS);
      // yac_host->printf("xxx HAL::viewOpened: 1d glGetError=%d\n", glGetError());
      yac_host->printf("[...]        extensions: \"%s\"\n", (const char*)gls);
#endif  // DX_GL4_CORE
   }

   initFPS();

   // Override glGetError() calls
   {
      char *s = ::getenv("TKSDL_GLERROR");
      if(NULL != s)
      {
         b_glerror = atoi(s) ? 1 : 0;
         if(yac_host->yacGetDebugLevel() >= 1)
            yac_host->printf("[dbg] tksdl::HAL: %s glGetError() calls via environment variable.\n", b_glerror ? "enabling" : "disabling");
      }
   }

   /////hook_call(&tksdl_hook_open);

   // Reset timer/redraw queue flag (event queue was re-created)
   yac_host->yacMutexLock(mtx_event_queued);
   b_timer_event_queued  = YAC_FALSE;
   b_redraw_event_queued = YAC_FALSE;
   yac_host->yacMutexUnlock(mtx_event_queued);


   if(!_bResize)
   {
      // (note) in case of resize, onResize() is called, followed by onOpen()
      callOnOpen();
   }

}

sBool _HAL::toggleFullScreen(void) {
   sBool r = YAC_FALSE;

   closeView();

   if(b_fullscreen)
   {
      r = openWindow(win_sx, win_sy);
   }
   else
   {
      YAC_Value cret;

      //yac_host->printf("xxx tksdl:toggleFullSCreen: fx_sx=%u fs_sy=%u\n", fs_sx, fs_sy);
      r = openScreen(fs_sx, fs_sy, fs_z);

      if(!r)
      {
         r = openWindow(win_sx, win_sy);
      }
      else
      {
#if SDL_VERSION_ATLEAST(2,0,16)
         // // ::SDL_WM_GrabInput(SDL_GRAB_OFF);
         ::SDL_SetWindowMouseGrab(sdl_window, SDL_FALSE);
#else
         ::SDL_SetWindowGrab(sdl_window, SDL_FALSE);
#endif
      }
   }

   callOnResize();

   return r;
}

void _HAL::showWindow(void) {
   if(!b_window_visible)
   {
      b_window_visible = YAC_TRUE;

#if defined(YAC_WIN32)
      ::ShowWindow(syswminfo.info.win.window, SW_SHOWNORMAL);
#else
      // (todo) XMapWindow() x11
#endif

      ::SDL_SetWindowTitle(sdl_window, (char*)viewport_caption.chars);
   }
}

void _HAL::hideWindow(void) {
   if(b_window_visible)
   {
      b_window_visible = false;

#if defined(YAC_WIN32)
      ::ShowWindow(syswminfo.info.win.window, SW_HIDE);
#else
      // (todo) x11
#endif
   }
}

void _HAL::useWindow(sU16 _sx, sU16 _sy) {
   win_sx  = (sU16)_sx;
   win_sy  = (sU16)_sy;
   view_sx = _sx;
   view_sy = _sy;

}

#ifdef YAC_LINUX
void _HAL::interruptScreenSaverX11(void) {
#ifdef HAVE_X11
   XKeyEvent event;

   // printf("xxx syswminfo.info.x11.display=%p\n", syswminfo.info.x11.display);

   /* see http://www.doctort.org/adam/nerd-notes/x11-fake-keypress-event.html */
   event.display     = syswminfo.info.x11.display;
   event.window      = XDefaultRootWindow(syswminfo.info.x11.display);//0/*root*/;//syswminfo.info.window;
   event.root        = XDefaultRootWindow(syswminfo.info.x11.display);
   event.subwindow   = None;
   event.time        = CurrentTime;
   event.x           = 1;
   event.y           = 1;
   event.x_root      = 1;
   event.y_root      = 1;
   event.same_screen = True;
   event.keycode     = XKeysymToKeycode(event.display, XK_Down/*keycode*/);
   event.state       = 0/*modifiers*/;

   event.type = KeyPress;
   XSendEvent(event.display, event.window, True, KeyPressMask, (XEvent *)&event);

   event.type = KeyRelease;
   XSendEvent(event.display, event.window, True, KeyPressMask, (XEvent *)&event);
#endif // HAVE_X11
}
#endif // YAC_LINUX

void _HAL::waitVBlank(void) {
#ifdef YAC_WIN32
   if(lpDD)
   {
      lpDD->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, NULL);
   }
#elif defined(YAC_LINUX)

/*
  - http://readlist.com/lists/lists.freedesktop.org/xorg/0/4871.html:
     card_fd = open( "/dev/dri/card0", O_RDONLY );
     drm_wait_vblank_t wait_vblank;
     wait_vblank.request.type = _DRM_VBLANK_RELATIVE;
     wait_vblank.request.sequence = 1;
     wait_vblank.request.signal = 0;
     int rc = ioctl( card_fd, DRM_IOCTL_WAIT_VBLANK, &wait_vblank );
*/
#ifdef USE_DRM_IOCTL_WAIT_VBLANK
   static int card_fd = -2;

   if(-2 == card_fd)
   {
      // TODO: device is never closed
      card_fd = open( "/dev/dri/card0", O_RDONLY );
      // Open/Exists ?
      if(-1 != card_fd)
      {
         drm_wait_vblank_t wait_vblank;
         wait_vblank.request.type = _DRM_VBLANK_RELATIVE;
         wait_vblank.request.sequence = 1;
         wait_vblank.request.signal = 0;
         int rc = ioctl( card_fd, DRM_IOCTL_WAIT_VBLANK, &wait_vblank );
         if(-1 == rc)
         {
            // Failed.. :(
            close(card_fd);
            card_fd = -1; // don't try again
         }
      }
   }
#endif // USE_DRM_IOCTL_WAIT_VBLANK
#endif // YAC_LINUX
}

sUI _HAL::getMonitorFrequency(void) {
#ifdef YAC_WIN32
   if(lpDD)
   {
      lpDD->GetMonitorFrequency(&dw_monitorfreq);
      return dw_monitorfreq;
   }
   yac_host->printf("[...] Viewport::getFrequency: no ddraw pointer set, no window/screen opened?\n");
#endif
   return 60; // xxx default frequency
}

void _HAL::_setEventPolling(sSI _b) {
   b_event_polling = (0 != _b);
}

sSI _HAL::_getEventPolling(void) {
   return b_event_polling;
}

void _HAL::showCursor(sBool _st) {
   if(mouse.show != _st)
   {
      mouse.show = _st;
      ::SDL_ShowCursor(_st?SDL_ENABLE:SDL_DISABLE);
   }
}

void _HAL::setCaption(YAC_String *_s) {
   // yac_host->printf("xxx HAL::setCaption 1\n");
   viewport_caption.copy(_s);
   // yac_host->printf("xxx HAL::setCaption 2 b_window_visible=%d\n", b_window_visible);

   if(b_window_visible)  // (note) hangs when window is currently hidden
   {
      ::SDL_SetWindowTitle(sdl_window, (const char*)viewport_caption.chars);
   }

   // yac_host->printf("xxx HAL::setCaption LEAVE\n");
}

sBool _HAL::_enableUNICODE(sSI _bEnabled) {
   (void)_bEnabled;
   // (note) no-op in SDL2 build
   return YAC_TRUE;
}

sSI _HAL::_isApplicationActive(void) {
   return b_graphics_active;
}

void _HAL::_setTimerInterval(sUI _intervalMillisec) {
   timer_interval = _intervalMillisec;
}

sUI _HAL::_getTimerInterval(void) {
   return timer_interval;
}

void _HAL::_getNativeWindowHandle(YAC_Value *_r) {
   // (note) this should be interchangeable between 32 and 64bit applications
   _r->initEmptyString();
   _r->value.string_val->yacArrayRealloc(32, 0,0,0);
   char *s = (char*)_r->value.string_val->chars;
#ifdef YAC_MACOS
   Dyac_snprintf(s, 32, "%llu", (sU64)(size_t)syswminfo.info.cocoa.window);
#elif defined(YAC_WIN32)
   Dyac_snprintf(s, 32, "%u", (sUI)syswminfo.info.win.window);
#elif defined(HAVE_X11)
   Dyac_snprintf(s, 32, "%u", (sUI)syswminfo.info.x11.window);
#else
   Dyac_snprintf(s, 32, "0");
#endif // YAC_MACOS
   _r->value.string_val->fixLength();
}

sUI _HAL::_getNativeProcessId(void) {
#ifdef YAC_WIN32
   return ::GetCurrentProcessId();
#endif
   return 0;
}

void _HAL::_allowShowNativeWindow(sUI _otherProcessId) {
#ifdef YAC_WIN32
   (void)::AllowSetForegroundWindow(_otherProcessId);
#endif // YAC_WIN32
}

void _HAL::_showNativeWindow(YAC_String *_nativeWindowHandle, sBool _bFocus) {
   if(YAC_VALID(_nativeWindowHandle))
   {
      YAC_CAST_ARG(YAC_String,sNativeWindowHandle,_nativeWindowHandle);
      yacmem64 m; m.any = NULL;
      sscanf((const char*)sNativeWindowHandle->chars, "%llu", &m.u64);
      if(NULL != m.any)
      {
#ifdef YAC_MACOS
         yac_host->printf("[dbg] HAL::showNativeWindow<macos>(%llu)\n", m.u64);
         hal_macos_window_to_front(m.any/*NSWindow*/);
         if(!_bFocus)
            hal_macos_window_to_front(syswminfo.info.cocoa.window);
#elif defined(YAC_WIN32)
         // (note) requires permission by other process (see allowShowNativeWindow())
         // ::SetFocus((HWND)_nativeWindowHandle);
         // ::SetActiveWindow((HWND)_nativeWindowHandle);
         ::SetForegroundWindow((HWND)m.any);
         if(!_bFocus)
            ::SetForegroundWindow((HWND)syswminfo.info.win.window);
#endif // YAC_WIN32
      }
   }
}

#if 0
#ifdef YAC_WIN32
// (note) https://msdn.microsoft.com/en-us/library/windows/desktop/ms644990(v=vs.85).aspx (SetWindowsHookEx)
// (note) https://msdn.microsoft.com/en-us/library/windows/desktop/ms644977(v=vs.85).aspx (CBTProc)
// (note) https://stackoverflow.com/questions/18188002/hooking-for-create-window-and-want-to-get-class-name
static HHOOK g_cbt_hook = NULL;
//SUPPRESSDIALOGHOOK_API
LRESULT CALLBACK CBTProc(__in  int nCode, __in  WPARAM wParam, __in  LPARAM lParam)
{
   if(HCBT_CREATEWND == nCode)
   {
      // wParam = handle to the new window
      // lParam = long pointer to a CBT_CREATEWND structure
      HWND hwnd = (HWND)wParam;
      CHAR name[1024] = {0};
      GetClassName(hwnd, name, sizeof(name));
      printf("xxx tksdl::HAL::CBTProc: HCBT_CREATEWND className=\"%s\" hwnd=0x%08x\n", name, hwnd);
   }
   else if(HCBT_DESTROYWND == nCode)
   {
      HWND hwnd = (HWND)wParam;
      CHAR name[1024] = {0};
      GetClassName(hwnd, name, sizeof(name));
      printf("xxx tksdl::HAL::CBTProc: HCBT_DESTROYWND className=\"%s\" hwnd=0x%08x\n", name, hwnd);
   }
   return CallNextHookEx(g_cbt_hook, nCode, wParam, lParam);
}
#endif

void _HAL::_installWindowCreateAndDestroyHook(void) {
#ifdef YAC_WIN32
   if(NULL == g_cbt_hook)
   {
      yac_host->printf("[dbg] tksdl: installWindowCreateAndDestroyHook()\n");
      // g_cbt_hook = ::SetWindowsHookEx(WH_CBT, (HOOKPROC)CBTProc, ::GetModuleHandle(NULL), GetCurrentThreadId()/*dwThreadId*/);   // works but only for windows created by current thread
      // g_cbt_hook = ::SetWindowsHookEx(WH_CBT, (HOOKPROC)CBTProc, ::GetModuleHandle(NULL), 0/*dwThreadId*/); // does not work (1428)
      // g_cbt_hook = ::SetWindowsHookEx(WH_CBT, (HOOKPROC)CBTProc, NULL, GetCurrentThreadId()/*dwThreadId*/);  // works but only for windows created by current thread
      g_cbt_hook = ::SetWindowsHookEx(WH_CBT, (HOOKPROC)CBTProc, NULL, GetCurrentThreadId()/*dwThreadId*/);
      if(NULL == g_cbt_hook)
      {
         yac_host->printf("[---] tksdl: installWindowCreateAndDestroyHook: SetWindowsHookEx() failed. GetLastError()=%d\n", GetLastError());
      }
   }
#endif
}

void _HAL::_uninstallWindowCreateAndDestroyHook(void) {
#ifdef YAC_WIN32
   if(NULL != g_cbt_hook)
   {
      yac_host->printf("[dbg] tksdl: uninstallWindowCreateAndDestroyHook()\n");
      ::UnhookWindowsHookEx(g_cbt_hook);
      g_cbt_hook = NULL;
   }
#endif
}

#else

#ifdef YAC_WIN32
// (note) https://msdn.microsoft.com/en-us/library/ms697187.aspx (Event constants)
// (note) https://msdn.microsoft.com/en-us/library/ms696160.aspx (SetWinEventHook)
// (note) https://msdn.microsoft.com/en-us/library/ms697317.aspx (WinEventProc)

static HWINEVENTHOOK g_winevent_hook = NULL;
static VOID CALLBACK loc_winevent_proc(HWINEVENTHOOK hWinEventHook,
                                       DWORD event,
                                       HWND hwnd,
                                       LONG idObject,
                                       LONG idChild,
                                       DWORD dwEventThread,
                                       DWORD dwmsEventTime
                                       ) {
   // printf("xxx tksdl:loc_winevent_proc called event=0x%08x hwnd=0x%08x idObject=%d\n", event, hwnd, idObject);
   if(EVENT_OBJECT_CREATE == event)
   {
      if(0 != hwnd)
      {
         if(OBJID_WINDOW/*0*/ == idObject)
         {
            CHAR name[1024] = {0};
            GetClassName(hwnd, name, sizeof(name));

#ifdef DEBUG_WINEVENT_CREATE_DESTROY
            yac_host->printf("[dbg] tksdl::HAL::loc_winevent_proc: EVENT_OBJECT_CREATE className=\"%s\" hwnd=0x%08x\n", name, hwnd);
#endif // DEBUG_WINEVENT_CREATE_DESTROY

            tks_HAL->callOnCreateOtherWindow(sSI(hwnd));
         }
      }
   }
   else if(EVENT_OBJECT_DESTROY  == event)
   {
      if(0 != hwnd)
      {
         if(OBJID_WINDOW/*0*/ == idObject)
         {
            // (note) never reached for WINDOW objects ?!!
            CHAR name[1024] = {0};
            GetClassName(hwnd, name, sizeof(name));

#ifdef DEBUG_WINEVENT_CREATE_DESTROY
            yac_host->printf("[dbg] tksdl::HAL::loc_winevent_proc: EVENT_OBJECT_DESTROY className=\"%s\" hwnd=0x%08x\n", name, hwnd);
#endif // DEBUG_WINEVENT_CREATE_DESTROY
         }
      }
   }
   else if(EVENT_OBJECT_FOCUS == event)
   {
      if(0 != hwnd)
      {
         if(OBJID_CLIENT/*-4*/ == idObject)
         {
            CHAR name[1024] = {0};
            GetClassName(hwnd, name, sizeof(name));

#ifdef DEBUG_WINEVENT_CREATE_DESTROY
            yac_host->printf("[dbg] tksdl::HAL::loc_winevent_proc: EVENT_OBJECT_FOCUS className=\"%s\" hwnd=0x%08x\n", name, hwnd);
#endif // DEBUG_WINEVENT_CREATE_DESTROY
            tks_HAL->callOnFocusOtherWindow(sSI(hwnd));
         }
      }
   }
}
#endif

void _HAL::_installWindowCreateAndDestroyHook(void) {
#ifdef YAC_WIN32
   if(NULL == g_winevent_hook)
   {
      yac_host->printf("[dbg] tksdl: installWindowCreateAndDestroyHook()\n");
      g_winevent_hook = ::SetWinEventHook(EVENT_OBJECT_CREATE/*0x8000*/,
                                          EVENT_OBJECT_FOCUS/*0x8005*/,
                                          NULL/*hmodWinEventProc*/,
                                          &loc_winevent_proc,
                                          ::GetCurrentProcessId(),
                                          0/*idThread*/,
                                          0/*dwFlags*/
                                          );
      // g_winevent_hook = ::SetWinEventHook(0,
      //                                     0xFFFFu,
      //                                     NULL/*hmodWinEventProc*/,
      //                                     &loc_winevent_proc,
      //                                     ::GetCurrentProcessId(),
      //                                     0/*idThread*/,
      //                                     0/*dwFlags*/
      //                                     );

      if(NULL == g_winevent_hook)
      {
         yac_host->printf("[---] tksdl: installWindowCreateAndDestroyHook: SetWinEventHook() failed. GetLastError()=%d\n", GetLastError());
      }
   }
#endif // YAC_WIN32
}

void _HAL::_uninstallWindowCreateAndDestroyHook(void) {
#ifdef YAC_WIN32
   if(NULL != g_winevent_hook)
   {
      yac_host->printf("[dbg] tksdl: uninstallWindowCreateAndDestroyHook()\n");
      ::UnhookWinEvent(g_winevent_hook);
      g_winevent_hook = NULL;
   }
#endif
}

#endif // 0

void _HAL::_showOtherWindow(sUI _hwnd) {
   if(0 != _hwnd)
   {
#ifdef YAC_WIN32
      ::ShowWindow((HWND)_hwnd, SW_SHOWNORMAL);
      ::SetActiveWindow((HWND)_hwnd);
      ::SetForegroundWindow((HWND)_hwnd);
#else
      // (todo) x11
#endif // YAC_WIN32
   }
}

void _HAL::_hideOtherWindow(sUI _hwnd) {
   if(0 != _hwnd)
   {
#ifdef YAC_WIN32
      ::ShowWindow((HWND)_hwnd, SW_HIDE);
#else
      // (todo) x11
#endif // YAC_WIN32
   }
}

void _HAL::_bringWindowToTop(sUI _hwnd) {
   if(0 != _hwnd)
   {
#ifdef YAC_WIN32
      yac_host->printf("[trc] HAL::bringWindowToTop(hwnd=0x%08x)\n", _hwnd);
      ::ShowWindow((HWND)_hwnd, SW_RESTORE);
      ::SetForegroundWindow((HWND)_hwnd);
      ::BringWindowToTop((HWND)_hwnd);
#else
      // (todo) x11
#endif // YAC_WIN32
   }
}

#ifdef YAC_WIN32
extern "C" {
#ifndef DPI_ENUMS_DECLARED
typedef enum PROCESS_DPI_AWARENESS {
    PROCESS_DPI_UNAWARE = 0,
    PROCESS_SYSTEM_DPI_AWARE = 1,
    PROCESS_PER_MONITOR_DPI_AWARE = 2
} PROCESS_DPI_AWARENESS;
#endif // DPI_ENUMS_DECLARED
typedef HRESULT (*SetProcessDpiAwareness_fxn_t)(PROCESS_DPI_AWARENESS value);
typedef UINT (*GetDpiForWindow_fxn_t) (HWND hwnd);
typedef void (*EnableMouseInPointer_fxn_t)(BOOL fEnable);
}
#endif // YAC_WIN32

void _HAL::setDpiAwareness(sBool _bEnable) {
#ifdef YAC_WIN32
   HMODULE hDLL = ::LoadLibrary("Shcore.dll");
   if(NULL != hDLL)
   {
      SetProcessDpiAwareness_fxn_t setProcessDpiAwareness = (SetProcessDpiAwareness_fxn_t)
         ::GetProcAddress(hDLL, "SetProcessDpiAwareness");
      if(NULL != setProcessDpiAwareness)
      {
         yac_host->printf("[dbg] tksdl::HAL::setEnableDpiAwareness: calling SetProcessDpiAwareness() bEnable=%d\n", _bEnable);
         (void)setProcessDpiAwareness(_bEnable ? PROCESS_PER_MONITOR_DPI_AWARE : PROCESS_DPI_UNAWARE);
      }
      ::FreeLibrary(hDLL);
   }
#endif // YAC_WIN32
}

sUI _HAL::getDpi(void) {
   sUI r = 96;
#if defined(YAC_WIN32)
   HMODULE hDLL = ::LoadLibrary("User32.dll");
   if(NULL != hDLL)
   {
      GetDpiForWindow_fxn_t getDpiForWindow = (GetDpiForWindow_fxn_t)
         ::GetProcAddress(hDLL, "GetDpiForWindow");
      if(NULL != getDpiForWindow)
      {
         yac_host->printf("[dbg] tksdl::HAL::getDpi: calling GetDpiForWindow()\n");
         r = getDpiForWindow(syswminfo.info.win.window);
      }
      ::FreeLibrary(hDLL);
   }
#endif // YAC_WIN32
   return r;
}

// Enable WM_POINTER messages (Windows 8 or newer)
void _HAL::setTouchInput(sBool _bEnable) {
#ifdef YAC_WIN32
   HMODULE hDLL = ::LoadLibrary("User32.dll");
   if(NULL != hDLL)
   {
      EnableMouseInPointer_fxn_t enableMouseInPointer = (EnableMouseInPointer_fxn_t)
         ::GetProcAddress(hDLL, "EnableMouseInPointer");
      if(NULL != enableMouseInPointer)
      {
         yac_host->printf("[dbg] tksdl::HAL::setEnableTouchInput: EnableMouseInPointer() detected (win8+)\n");
         (void)enableMouseInPointer(_bEnable);
         tks_HAL->mouse.b_touch_input = _bEnable;
      }
      ::FreeLibrary(hDLL);
   }
#endif // YAC_WIN32
}

// void _HAL::waitForOtherWindow(void) {
// #ifdef YAC_WIN32
//    sBool bDone = YAC_FALSE;
//    while(!bDone)
//    {
//       MSG msg;

//       while(::PeekMessage(&msg, NULL, 0, 0, 1))
//       {
//          yac_host->printf("HAL::waitForOtherWindow: msg.hwnd=0x%08x msg.message=0x%08x\n", msg.hwnd, msg.message);
//          ::TranslateMessage(&msg);
//          ::DispatchMessage(&msg);

//          if(WM_CLOSE == msg.message)
//          {
//             yac_host->printf("HAL::waitForOtherWindow: got WM_CLOSE\n");
//             bDone = YAC_TRUE;
//          }
//       }
//    }
// #endif // YAC_WIN32
// }

void _HAL::setWindowPositionAndSize(sSI _x, sSI _y, sUI _w, sUI _h) {
#ifdef YAC_WIN32
   if(yac_host->yacGetDebugLevel() >= 2)
      yac_host->printf("[dbg] HAL::setWindowPosition: call SetWindowPos(%d; %d; %u; %u)\n", _x, _y, _w, _h);
   // // HWND oldHWND = GetActiveWindow();
   (void)SetWindowPos(syswminfo.info.win.window, NULL, _x, _y, (int)_w, (int)_h,
                      /*SWP_NOMOVE | */ (((0u != _w) && (0u != _h))?0:SWP_NOSIZE)
                      );////|SWP_SHOWWINDOW|SWP_NOACTIVATE|SWP_SHOWWINDOW);
   // // ::SetActiveWindow(oldHWND);
#else
   SDL_Window *win = SDL_GL_GetCurrentWindow();
   if(yac_host->yacGetDebugLevel() >= 2)
      yac_host->printf("[dbg] HAL::setWindowPosition: call SDL_SetWindowPosition(%d; %d)\n", _x, _y);
   SDL_SetWindowPosition(win, _x, _y);
   if(yac_host->yacGetDebugLevel() >= 2)
      yac_host->printf("[dbg] HAL::setWindowPosition: call SDL_SetWindowSize(%u; %u)\n", _w, _h);
   SDL_SetWindowSize(win, _w, _h);
#endif // YAC_WIN32
}

sSI _HAL::getWindowPositionX(void) {
#if 0 && defined(YAC_WIN32)
   // // RECT rect;
   // // if(::GetWindowRect(syswminfo.window, &rect))
   // // {
   // //    return (sSI)rect.left;
   // // }
   POINT p; p.x = 0; p.y = 0;
   if(ClientToScreen(syswminfo.info.win.window, &p))
      return (sSI)p.x;
#else
   SDL_Window *win = SDL_GL_GetCurrentWindow();
   int x, y;
   SDL_GetWindowPosition(win, &x, &y);
   return x;
#endif // YAC_WIN32
   return 0;
}

sSI _HAL::getWindowPositionY(void) {
#if 0 && defined(YAC_WIN32)
   // // RECT rect;
   // // if(::GetWindowRect(syswminfo.info.win.window, &rect))
   // // {
   // //    return (sSI)rect.top;
   // // }
   POINT p; p.x = 0; p.y = 0;
   if(ClientToScreen(syswminfo.info.win.window, &p))
      return (sSI)p.y;
#else
   SDL_Window *win = SDL_GL_GetCurrentWindow();
   int x, y;
   SDL_GetWindowPosition(win, &x, &y);
   return y;
#endif // YAC_WIN32
   return 0;
}

sSI _HAL::getWindowSizeX(void) {
#if 0 && defined(YAC_WIN32)
   RECT rect;
   if(::GetWindowRect(syswminfo.info.win.window, &rect))
      return (sSI)(rect.right - rect.left);
#else
   SDL_Window *win = SDL_GL_GetCurrentWindow();
   int w, h;
   SDL_GetWindowSize(win, &w, &h);
   return w;
#endif // YAC_WIN32
   return 0;
}

sSI _HAL::getWindowSizeY(void) {
#if 0 && defined(YAC_WIN32)
   RECT rect;
   if(::GetWindowRect(syswminfo.info.win.window, &rect))
      return (sSI)(rect.bottom - rect.top);
#else
   SDL_Window *win = SDL_GL_GetCurrentWindow();
   int w, h;
   SDL_GetWindowSize(win, &w, &h);
   return h;
#endif // YAC_WIN32
   return 0;
}

void _HAL::setEnableWindowTitleAndBorder(sBool _bTitle, sBool _bBorder) {
#ifdef YAC_WIN32
   LONG style = ::GetWindowLong(syswminfo.info.win.window, GWL_STYLE);
   LONG flags = (_bTitle ? (WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX) : 0) | (_bBorder ? WS_BORDER : 0);
   style = (style & ~(WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_BORDER)) | flags;
   ::SetWindowLong(syswminfo.info.win.window, GWL_STYLE, style);
   ::SetWindowPos(syswminfo.info.win.window, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);   // make changes visible
#endif // YAC_WIN32
}

sBool _HAL::setLocalToRawKeyTable(const sUI *_tbl) {
   local_to_raw_key_table = _tbl;
   return YAC_TRUE;
}

sBool _HAL::setIconFromBMP(YAC_Object *_bmpBuffer) {
#ifdef USE_ICONS
   if(YAC_Is_Buffer(_bmpBuffer))
   {
      YAC_CAST_ARG(YAC_Buffer, bmpBuffer, _bmpBuffer);
      if(bmpBuffer->size > 0u)
      {
         SDL_Surface *icon;
         Uint8 *icon_mask;
         icon = LoadIconSurface(bmpBuffer->buffer, bmpBuffer->size, &icon_mask);
         if(NULL != icon)
            // SDL_WM_SetIcon(icon, icon_mask);
            SDL_SetWindowIcon(sdl_window, icon);
         else
            yac_host->printf("[---] tksdl::setIconFromBMP: LoadIconSurface() failed");
         if(NULL != icon_mask)
            free(icon_mask);
         return YAC_TRUE;
      }
   }
#endif // USE_ICONS
   return YAC_FALSE;
}

void _HAL::setCursorZoom(sUI _zoom) {
   if(_zoom > 8u)
      _zoom = 8u;
   else if(_zoom < 1u)
      _zoom = 1u;
   cursor_zoom = _zoom;
}

sUI _HAL::getCursorZoom(void) {
   return cursor_zoom;
}
