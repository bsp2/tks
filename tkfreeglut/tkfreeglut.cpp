/// tkfreeglut.cpp
///
/// (c) 2007-2008 by Carsten Busse <carsten.busse@googlemail.com> and Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define YAC_PRINTF defined
#define YAC_BIGSTRING
#include <yac.h>

#include <yac_host.cpp>

#ifdef YAC_WIN32
#include <windows.h>
#endif // YAC_WIN32

#include "tks-opengl.h"
#include "freeglut_geometry_tks.h"

YAC_Host *yac_host;

/// ---------------------------------------------------------------------------
#include "ying_freeglut_geometry.h"
#include "ying_freeglut_geometry.cpp"


// ---------------------------------------------------------------------------
void YAC_CALL YAC_Init(YAC_Host *_host) {
	if(_host->yacGetDebugLevel())
		_host->printf("[dbg] tkfreeglut::YAC_Init called.\n");
	yac_host=_host;

   YAC_Init_freeglut_geometry(_host);

	if(yac_host->yacGetDebugLevel())
		yac_host->yacPrint("[dbg] tkfreeglut::YAC_Init finished.\n");
}

void YAC_CALL YAC_Exit(YAC_Host *_host) {

   YAC_Exit_freeglut_geometry(_host);

	if(yac_host->yacGetDebugLevel())
		yac_host->yacPrint("[dbg] tkfreeglut::YAC_Exit() finished.\n");
}

sUI YAC_CALL YAC_Version(void) {
	return 0x00010002;
}

