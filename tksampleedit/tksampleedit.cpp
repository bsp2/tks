// ----
// ---- file   : tksampleedit.cpp
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2009-2026 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   :
// ----
// ---- created: 25Oct2009
// ---- changed: 29Apr2010, 11May2010, 20Jul2010, 23May2013, 07Mar2014, 15May2020, 11Jan2022
// ----          24Feb2024, 31Oct2026
// ----
// ----
// ----

#include "tksampleedit.h"

#include <yac_host.cpp>

YAC_Host *yac_host;

#include "ying_tksampleedit.h"
#include "ying_tksampleedit.cpp"

#include "ying_tksampleedit_PeakAvgTracker.cpp"

#ifdef USE_TKMINNIE
#include "../minnie/tkminnie/tkminnie_shared.h"
#include "../minnie/tkminnie/tkminnie_shared.cpp"
#else
#include "../tkopengl/tkopengl_shared.h"
#include "../tkopengl/tkopengl_shared.cpp"
#endif // USE_TKMINNIE


void YAC_CALL YAC_Init(YAC_Host *_host) {
	yac_host = _host;

	if(Dyac_host_yacGetDebugLevel() > 0)
		Dyac_host_log2("tksampleedit::YAC_Init called.");

   YAC_Init_tksampleedit(_host);

   tksampleedit_rms_init();

	if(Dyac_host_yacGetDebugLevel() > 0)
		Dyac_host_log2("tksampleedit::YAC_Init finished.");
}

void YAC_CALL YAC_Exit(YAC_Host *_host) {

   YAC_Exit_tksampleedit(_host);

	if(Dyac_host_yacGetDebugLevel() > 0)
		Dyac_host_log2("tksampleedit::YAC_Exit() finished.");
}

sUI YAC_CALL YAC_Version(void) {
	return 0x0001000E;
}
