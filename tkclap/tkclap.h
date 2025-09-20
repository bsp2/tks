/// tkclap.h
///
/// (c) 2024 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///
/// created: 01Jul2024
/// changed: 05Jul2024
///
///
///

#ifndef __TKCLAP_H__
#define __TKCLAP_H__

#include "clap-main/include/clap/all.h"

#define YAC_BIGSTRING defined
#include <yac.h>

#if defined(YAC_MACOS) || defined(YAC_LINUX)
#include <pthread.h> 
#endif

#ifdef YAC_MACOS
#include "CoreFoundation/CoreFoundation.h"
#endif // YAC_MACOS

#ifdef YAC_WIN32
#include <windows.h>
#endif // YAC_WIN32

#include "ying_clap.h"

class CLAPPlugin;
#include "tkclap_window.h"
#include "CLAPPlugin.h"


YG("clap");


extern sF32  tkclap_bpm;
extern sF32  tkclap_song_pos_beats;  // #quarter notes since replay start
extern sBool tkclap_song_playing;

YF void YAC_CALL tkclap_set_bpm (sF32 _bpm);
YF void YAC_CALL tkclap_set_song_pos_beats (sF32 _songPosBeats, sBool _bPlaying);

#define Dprintf         if(0);else yac_host->printf
#define Dprintf_error   if(0);else yac_host->printf
#define Dprintf_verbose if(1);else yac_host->printf
#define Dprintf_debug   if(b_debug) yac_host->printf


#endif // __TKCLAP_H__
