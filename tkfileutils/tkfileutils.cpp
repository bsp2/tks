#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#include <yac.h>
YAC_Host *yac_host;


#include "tkfileutils.h"

#include "ying_fileutils.h"
#include "ying_fileutils.cpp"


// ---------------------------------------------------------------------------- YAC_Init
void YAC_CALL YAC_Init(YAC_Host *_host) {
	yac_host=_host;

   YAC_Init_fileutils(_host);
}

void YAC_CALL YAC_Exit(YAC_Host *_host) {
   YAC_Exit_fileutils(_host);
}


#include <yac_host.cpp>
