#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#define YAC_BIGSTRING
#define YAC_PRINTF
#include <yac.h>
YAC_Host *yac_host;


// // Implement standard exception ID variables (see yac.h)
// Dyac_std_exid_impl;



// ---------------------------------------------------------------------------- YAC_Init
void YAC_CALL YAC_Init(YAC_Host *_host) {
	yac_host=_host;

//    // Resolve "standard" exception IDs
//    Dyac_std_exid_resolve;

//    YAC_Init_test(_host);
//    yac_host->printf("init done.\n");
}

void YAC_CALL YAC_Exit(YAC_Host *_host) {
//    YAC_Exit_test(_host);
}


#include <yac_host.cpp>
