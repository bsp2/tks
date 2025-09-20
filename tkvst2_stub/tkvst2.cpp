/// tkvst.cpp
///
/// (c) 2010-2024 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///
/// created: 01Oct2010
/// changed: 02Oct2010, 06Oct2010, 10Oct2010, 13Oct2010, 05Feb2023, 01Jul2024
///
///
///

#include "tkvst2.h"

#include <yac_host.cpp>
YAC_Host *yac_host;

#include "VST2Plugin.h"
#include "VST2PinProperties.h"
#include "VST2ParameterProperties.h"
#include "../tkmidipipe/HostMIDIEvents.h"
#include "../tkmidipipe/ModInputFilter.h"

#include "ying_vst2.cpp"
#include "ying_vst2_VST2Plugin.cpp"
#include "ying_vst2_VST2PinProperties.cpp"
#include "ying_vst2_VST2ParameterProperties.cpp"

sF32 tkvst2_bpm = 125.0f;
sF32 tkvst2_song_pos_ppq = 0.0f;
sBool tkvst2_song_playing = YAC_FALSE;

void tkvst2_set_bpm(sF32 _bpm) {
   tkvst2_bpm = _bpm;
}

void tkvst2_set_song_pos_ppq(sF32 _songPosPPQ, sBool _bPlaying) {
   tkvst2_song_pos_ppq = _songPosPPQ;
   tkvst2_song_playing = _bPlaying;
}
void YAC_CALL YAC_Init(YAC_Host *_host) {
	yac_host = _host;
   
	if(yac_host->yacGetDebugLevel()) 
      yac_host->printf("[dbg] tkvst2::YAC_Init called.\n");
   
   YAC_Init_vst2(_host);

   for(sUI i = 0u; i < VST2PLUGIN_NUM_SIGNALS; i++)
   {
      tkvst2_audio_master_signal_funs[i] = NULL;
   }

   tkvst2_audio_master_script_context = yac_host->yacContextCreate();

   // (note) tksynth must have been loaded before loading tkvst, otherwise certain references cannot be resolved
   // (note) Linux supports a nifty linker feature (RTLD_LAZY) for which I did not find a Windows equivalent
   //         therefore we need to take the long route via YAC_Host

   // clid_SyModule = yac_host->yacGetClassIDByName("SyModule");
   // if(0 == clid_SyModule)
   // {
   //    printf("[---] tkvst::YAC_Init: FATAL ERROR: tksynth plugin has not been loaded, yet !\n");
   // }
   // else
   // {
   //    yac_host->cpp_typecast_map[clid_SyModuleVST] [clid_SyModule] = 1;
   // }

   // clid_SyInput      = yac_host->yacGetClassIDByName("SyInput");
   // clid_SyOutput     = yac_host->yacGetClassIDByName("SyOutput");
   // clid_SyModule     = yac_host->yacGetClassIDByName("SyModule");
   // clid_SyConnection = yac_host->yacGetClassIDByName("SyConnection");
}

void YAC_CALL YAC_Exit(YAC_Host *_host) {

   YAC_Exit_vst2(_host);

   yac_host->yacContextDestroy(tkvst2_audio_master_script_context);
   tkvst2_audio_master_script_context = NULL;
}

sUI YAC_CALL YAC_Version(void) {
	return 0x00020008;
}
