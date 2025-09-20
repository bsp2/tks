// ----
// ---- file   : tkanalogrytm.cpp
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2016-2023 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : This is part of the "tkanalogrytm" plugin.
// ----
// ---- created: 27Feb2016
// ---- changed: 28Feb2016, 21Aug2017, 03Jan2018, 25Nov2023
// ----
// ----
// ----

#include "tkanalogrytm.h"

#include <yac_host.cpp>
YAC_Host *yac_host;

#include "ying_analogrytm.cpp"
#include "ying_analogrytm_AR_Pattern.cpp"
#include "ying_analogrytm_AR_Kit.cpp"
#include "ying_analogrytm_AR_Sound.cpp"
#include "ying_analogrytm_AR_Global.cpp"
#include "ying_analogrytm_AR_Settings.cpp"


void YAC_CALL YAC_Init(YAC_Host *_host) {
	yac_host = _host;

	if(yac_host->yacGetDebugLevel())
   {
      yac_host->printf("tkanalogrytm::YAC_Init called.\n");
      yac_host->printf("tkanalogrytm::YAC_Init:  sizeof(ar_kit_t)=%u\n", sizeof(ar_kit_t));
      yac_host->printf("tkanalogrytm::YAC_Init:  sizeof(ar_sound_t)=%u\n", sizeof(ar_sound_t));
      yac_host->printf("tkanalogrytm::YAC_Init:  sizeof(ar_pattern_t)=%u\n", sizeof(ar_pattern_t));
      yac_host->printf("tkanalogrytm::YAC_Init:  sizeof(ar_pattern_track_t)=%u\n", sizeof(ar_pattern_track_t));
      yac_host->printf("tkanalogrytm::YAC_Init:  sizeof(ar_global_t)=%u\n", sizeof(ar_global_t));
      yac_host->printf("tkanalogrytm::YAC_Init:  sizeof(ar_settings_t)=%u\n", sizeof(ar_settings_t));
   }

   YAC_Init_analogrytm(_host);
}

void YAC_CALL YAC_Exit(YAC_Host *_host) {

	if(yac_host->yacGetDebugLevel())
      yac_host->printf("[dbg] tkanalogrytm::YAC_Exit called.\n");

   YAC_Exit_analogrytm(_host);

}

sUI YAC_CALL YAC_Version(void) {
	return 0x00010004;
}
