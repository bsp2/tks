
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define YAC_PRINTF defined
#include <yac.h>

//#include <tks-list.h>
#include <yac_host.cpp>

#include <ft2build.h>
#include <freetype/ftdriver.h>
#include <freetype/ftmodapi.h>
#include FT_FREETYPE_H

//#include <freetype/freetype.h>

#include "NativeFT2Face.h"


YAC_Host *yac_host;

YG("freetype");

// --------------------------------------------------------------------------- ModulatorFactory
#include "ying_freetype_NativeFT2Face.cpp"

#include "ying_freetype.cpp"


FT_Library ft_library;

// ---------------------------------------------------------------------------
void YAC_CALL YAC_Init(YAC_Host *_host) {
	yac_host=_host;
	if(yac_host->yacGetDebugLevel() > 1)
		yac_host->yacPrint("[dbg] tkfreetype2::YAC_Init() called.\n");

   int error = FT_Init_FreeType(&ft_library);
   if(0 != error)
   {
      yac_host->printf("[---] tkfreetype2: FT_Init_FreeType failed with error code %i\n", error);
   }
   else
   {
	   YAC_Init_freetype(_host);

      {
         FT_UInt interpreter_version = TT_INTERPRETER_VERSION_35;  // renders tkui default font correctly (hinting)
         // FT_UInt interpreter_version = TT_INTERPRETER_VERSION_38;
         // FT_UInt interpreter_version = TT_INTERPRETER_VERSION_40;
         FT_Property_Set(ft_library, "truetype",
                         "interpreter-version",
                         &interpreter_version
                         );
      }
   }
}

void YAC_CALL YAC_Exit(YAC_Host *_host) {
	YAC_Exit_freetype(_host);

   FT_Done_FreeType(ft_library);

	if(yac_host->yacGetDebugLevel() > 1)
		yac_host->yacPrint("[dbg] tkfreetype2::YAC_Exit() finished.\n");
}

sUI YAC_CALL YAC_Version(void) {
	return 0x00010000;
}
