/// tksdl.cpp
///
/// (c) 2001-2024 by Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define YAC_PRINTF defined
#define YAC_BIGSTRING defined
#include <yac.h>

#include "inc_sdl.h"

#include <yac_host.cpp>
YAC_Host *yac_host;

#include "../tkopengl/inc_opengl.h"

#include "HAL.h"
#include "Viewport.h"
#include "FPS.h"
#include "Mouse.h"
#include "Key.h"
#include "Joystick.h"
#include "Bitmap.h"
#include "Cursor.h"
#include "AudioDevice.h"
#include "AudioSample.h"
#include "AudioSampleVoice.h"
#include "SamplePlayer.h"
#include "Clipboard.h"

_SDL     *tks_HAL;

#include "ying_hal.h"
#include "ying_hal1.h"
#include "ying_audio_AudioDevice.cpp"
#include "ying_audio_AudioSample.cpp"
#include "ying_audio_SamplePlayer.cpp"
#include "ying_audio.h"
#include "ying_audio.cpp"
#include "ying_hal1_Cursor.cpp"
#include "ying_hal1_FPS.cpp"
#include "ying_hal1_Joystick.cpp"
#include "ying_hal1_Key.cpp"
#include "ying_hal1_Mouse.cpp"
#include "ying_hal1_Viewport.cpp"
#include "ying_hal1.h"
#include "ying_hal1.cpp"
#include "ying_hal_SDL.cpp"
#include "ying_hal.cpp"

void *sc_context;

#include "tksdl_hooks.h"


// ---------------------------------------------------------------------------
void YAC_CALL YAC_Init(YAC_Host *_host) {
	yac_host=_host;

	if(_host->yacGetDebugLevel() > 1) 
   {
	  _host->printf("[dbg] tksdl::YAC_Init called.\n");

#ifdef YAC_GLOBAL_NEWDELETE
     _host->printf("[dbg] tksdl: yac_global_newdelete_counter=%d\n", yac_host->yacNewDeleteGetCounter());
#endif // YAC_GLOBAL_NEWDELETE
   }

   sc_context = yac_host->yacContextGetDefault();

#if defined(YAC_WIN32)
	// ::SDL_SetModuleHandle(::GetModuleHandle(NULL)); 
#endif // YAC_WIN32

#if 1
	::SDL_Init(SDL_INIT_AUDIO | SDL_INIT_NOPARACHUTE);
#else
   // (note) SDL_INIT_JOYSTICK takes ~4 seconds on macOS (!)
   //         => lazy-init on demand
   ::SDL_Init(SDL_INIT_AUDIO | SDL_INIT_JOYSTICK | SDL_INIT_NOPARACHUTE);
#endif

	YAC_Init_hal(_host);
   tks_HAL = &t_SDL->ctemplate;

	YAC_Init_hal1(_host);

	YAC_Init_audio(_host);

#if 0
   // (note) [29Mar2024] lazy-init (getNumJoysticks(), getJoystick())
	tks_HAL->openJoysticks();
#endif

   _AudioDevice::cbid_SDL_FillAudioBuffer = yac_host->yacCallbackCreate("SDL_FillAudioBuffer");

   // Export address of 'open' hook list
   tksdl_hook_open.fxn = NULL;
   tksdl_hook_open.next = NULL;
   yac_host->yacCallbackSetFunByName(TKSDL_HOOK_NAME_OPEN, (YAC_CFunctionPtr)&tksdl_hook_open);

   // Export address of 'close' hook list
   tksdl_hook_close.fxn = NULL;
   tksdl_hook_close.next = NULL;
   yac_host->yacCallbackSetFunByName(TKSDL_HOOK_NAME_CLOSE, (YAC_CFunctionPtr)&tksdl_hook_close);

	if(yac_host->yacGetDebugLevel() > 1)
		yac_host->yacPrint("tksdl::YAC_Init() finished.\n");
}

void YAC_CALL YAC_Exit(YAC_Host *_host) {
   sBool bDebug = (yac_host->yacGetDebugLevel() > 1);

	if(bDebug)
		yac_host->yacPrint("[dbg] tksdl::YAC_Exit() called.\n");

   if(bDebug)
      yac_host->yacPrint("[dbg] tksdl: exit audio.\n");
   YAC_Exit_audio        (_host);

   if(bDebug)
      yac_host->yacPrint("[dbg] tksdl: exit hal1.\n");
   YAC_Exit_hal1         (_host);

   if(bDebug)
      yac_host->yacPrint("[dbg] tksdl: exit hal.\n");
   YAC_Exit_hal          (_host);

	::SDL_Quit();

	if(bDebug)
		yac_host->yacPrint("[dbg] tksdl::YAC_Exit() finished.\n");

}

sUI YAC_CALL YAC_Version(void) {
	return 0x00090900;
}
