// ----
// ---- file   : tksamplechain.cpp
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2016 by Bastian Spiegel. 
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See 
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : This is part of the "syntracker" sequencer.
// ----
// ---- changed: 25Mar2016
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

	if(yac_host->yacGetDebugLevel()) 
      yac_host->printf("tksamplechain::YAC_Init called.\n");

   YAC_Init_samplechain(_host);
}

void YAC_CALL YAC_Exit(YAC_Host *_host) {

	if(yac_host->yacGetDebugLevel()) 
      yac_host->printf("tksamplechain::YAC_Exit called.\n");

   YAC_Exit_samplechain(_host);

}

sUI YAC_CALL YAC_Version(void) {
	return 0x00010001;
}
