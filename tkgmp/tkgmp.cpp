
#define YAC_BIGSTRING
#define YAC_PRINTF
#include <yac.h>
YAC_Host *yac_host;

#include "tkgmp.h"

#include "ying_tkgmp.h"
#include "ying_tkgmp_BigInt.cpp"
#include "ying_tkgmp_BigRat.cpp"
#include "ying_tkgmp_BigFloat.cpp"

#include "ying_tkgmp.cpp"

void YAC_CALL YAC_Init(YAC_Host *_host) {
	yac_host=_host;
	YAC_Init_tkgmp(_host);
	yac_host->printf("[tkgmp] Initialised.\n");
}

void YAC_CALL YAC_Exit(YAC_Host *_host) {
	YAC_Exit_tkgmp(_host);
}

#include "BigInt.cpp"
#include "BigRat.cpp"
#include "BigFloat.cpp"

#include <yac_host.cpp>
