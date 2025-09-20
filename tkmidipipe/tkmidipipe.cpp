// ----
// ---- file   : tkmidipipe.cpp
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2014-2025 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : This is part of the "syntracker" sequencer.
// ----
// ---- created: 01Sep2014
// ---- changed: 03Sep2014, 03Jan2018, 06Apr2020, 19Jun2021, 01Jul2024, 05Apr2025
// ----
// ----
// ----

#include "tkmidipipe.h"

#include <yac_host.cpp>
YAC_Host *yac_host;

#include "ying_midipipe.cpp"
#include "ying_midipipe_MIDIPipeFrame.cpp"
#include "ying_midipipe_MIDIPipeEvent.cpp"
#include "ying_midipipe_MIDIPipeNode.cpp"
#include "ying_midipipe_MIDIPipeDevice.cpp"
#include "ying_midipipe_MIDIPipe.cpp"
#include "ying_midipipe_MIDIPipeRoot.cpp"
#include "ying_midipipe_MIDIPipeNodeScriptProxy.cpp"
#include "ying_midipipe_ModInputFilter.cpp"
#include "ying_midipipe_HostMIDIEvents.cpp"


void YAC_CALL YAC_Init(YAC_Host *_host) {
	yac_host = _host;

	if(yac_host->yacGetDebugLevel())
      yac_host->printf("tkmidipipe::YAC_Init called.\n");

   YAC_Init_midipipe(_host);

   yac_host->cpp_typecast_map[clid_MIDIPipeNodeScriptProxy] [clid_MIDIPipeNode] = 1;
}

void YAC_CALL YAC_Exit(YAC_Host *_host) {
	if(yac_host->yacGetDebugLevel())
      yac_host->printf("[dbg] tkmidipipe::YAC_Exit called.\n");

   YAC_Exit_midipipe(_host);
}

sUI YAC_CALL YAC_Version(void) {
	return 0x00010014;
}
