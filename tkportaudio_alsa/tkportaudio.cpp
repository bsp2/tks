
#include "tkportaudio.h"

YAC_Host *yac_host;


#include "ying_portaudio_PortAudio.cpp"
#include "ying_portaudio_PaHostApiInfo.cpp"
#include "ying_portaudio_PaDeviceInfo.cpp"
#include "ying_portaudio_PaStreamParameters.cpp"
#include "ying_portaudio_PaStream.cpp"
#include "ying_portaudio_PaStreamMacOSWorkgroupJoinToken.cpp"

#include "ying_portaudio.cpp"


// Implement standard exception ID variables (see yac.h)
Dyac_std_exid_impl;


// ---------------------------------------------------------------------------- YAC_Init
void YAC_CALL YAC_Init(YAC_Host *_host) {
	yac_host=_host;

   // Resolve "standard" exception IDs
   Dyac_std_exid_resolve;

   YAC_Init_portaudio(_host);
}

void YAC_CALL YAC_Exit(YAC_Host *_host) {
   YAC_Exit_portaudio(_host);
}


#include <yac_host.cpp>
