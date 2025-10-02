/// inc_sdl.h
///
/// (c) 2008-2025 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#ifndef TKSDL_INCLUDES_H__
#define TKSDL_INCLUDES_H__


#ifdef YAC_WIN32
#include <windows.h>
#include <ddraw.h>
#include <Commctrl.h>

#include <SDL.h>
#include <SDL_audio.h>
#include <SDL_syswm.h>
#include <SDL_keyboard.h>
#include <SDL_keycode.h>
#include <SDL_scancode.h>
#include <SDL_mouse.h>
#include <SDL_joystick.h>
#include <SDL_syswm.h>

#else

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_joystick.h>
#include <SDL2/SDL_syswm.h>

#endif // YAC_WIN32


// SDL2:
/* extern "C" int   SDL_GL_SetSwapInterval (int interval); */


#endif // TKSDL_INCLUDES_H__
