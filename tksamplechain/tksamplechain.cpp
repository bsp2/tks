// ----
// ---- file   : tksamplechain.cpp
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2016-2026 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : This is part of the "syntracker" sequencer.
// ----
// ---- created: 25Mar2016
// ---- changed: 31Oct2026
// ----
// ----
// ----

#include "tksamplechain.h"

#include <yac_host.cpp>
YAC_Host *yac_host;

#include "ying_samplechain.cpp"
#include "ying_samplechain_SampleChain.cpp"


void YAC_CALL YAC_Init(YAC_Host *_host) {
	yac_host = _host;

	if(Dyac_host_yacGetDebugLevel() > 1)
      Dyac_host_log2("tksamplechain::YAC_Init called.");

   YAC_Init_samplechain(_host);
}

void YAC_CALL YAC_Exit(YAC_Host *_host) {

	if(Dyac_host_yacGetDebugLevel() > 1)
      Dyac_host_log2("tksamplechain::YAC_Exit called.");

   YAC_Exit_samplechain(_host);

}

sUI YAC_CALL YAC_Version(void) {
	return 0x00010001;
}
