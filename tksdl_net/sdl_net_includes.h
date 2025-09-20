/// sdl_net_includes.h
///
/// (c) 2008-2009 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#ifndef __TKSDL_NET_INCLUDES_H__
#define __TKSDL_NET_INCLUDES_H__


#ifdef YAC_WIN32

#include <SDL.h>
#include <SDL_net.h>

#else

#include <SDL/SDL.h>
#include <SDL/SDL_net.h>

#endif // YAC_WIN32



#endif // __TKSDL_NET_INCLUDES_H__
