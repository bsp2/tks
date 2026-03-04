#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#define YAC_BIGSTRING
#define YAC_PRINTF
#include <yac.h>
YAC_Host *yac_host;

#include "tkbluetooth.h"

#include "ying_tkbluetooth.h"

#include "ying_tkbluetooth_BlueToothAddress.cpp"
#include "ying_tkbluetooth_BlueToothClient.cpp"
#include "ying_tkbluetooth_BlueToothServer.cpp"

#include "ying_tkbluetooth.cpp"

// Implement standard exception ID variables (see yac.h)
Dyac_std_exid_impl;


// ---------------------------------------------------------------------------- YAC_Init
void YAC_CALL YAC_Init(YAC_Host *_host) {
	yac_host=_host;

   // Resolve "standard" exception IDs
   Dyac_std_exid_resolve;

   YAC_Init_tkbluetooth(_host);
   yac_host->printf("tkbluetooth init done.\n");
}

void YAC_CALL YAC_Exit(YAC_Host *_host) {
   YAC_Exit_tkbluetooth(_host);
}


#include <yac_host.cpp>
