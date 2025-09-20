/// tkclap.cpp
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

#include "tkclap.h"

#include <yac_host.cpp>
YAC_Host *yac_host;

#include "CLAPPlugin.h"

#include "ying_clap.cpp"
#include "ying_clap_CLAPPluginBundle.cpp"
#include "ying_clap_CLAPPlugin.cpp"

sF32  tkclap_bpm            = 125.0f;
sF32  tkclap_song_pos_beats = 0.0f;
sBool tkclap_song_playing   = YAC_FALSE;

void tkclap_set_bpm(sF32 _bpm) {
   tkclap_bpm = _bpm;
}

void tkclap_set_song_pos_beats(sF32 _songPosBeats, sBool _bPlaying) {
   tkclap_song_pos_beats = _songPosBeats;
   tkclap_song_playing   = _bPlaying;
}

void YAC_CALL YAC_Init(YAC_Host *_host) {
	yac_host = _host;
   
	if(yac_host->yacGetDebugLevel()) 
      yac_host->printf("[dbg] tkclap::YAC_Init called.\n");
   
   YAC_Init_clap(_host);

   for(sUI i = 0u; i < CLAPPLUGIN_NUM_SIGNALS; i++)
   {
      tkclap_signal_funs[i] = NULL;
   }

   tkclap_script_context = yac_host->yacContextCreate();
}

void YAC_CALL YAC_Exit(YAC_Host *_host) {

   YAC_Exit_clap(_host);

   yac_host->yacContextDestroy(tkclap_script_context);
   tkclap_script_context = NULL;
}

sUI YAC_CALL YAC_Version(void) {
	return 0x00010000;
}
