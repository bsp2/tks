// tri_raster_aa.cpp
///
/// (c) 2024 Bastian Spiegel <bs@tkscript.de> 
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#define YAC_PRINTF defined
#include <yac.h>

#include <stdlib.h>

#include "inc_opengl.h"

#include "tks-list.h"

#include "bitmap.h"
#include "image.h"
#include "tksopengl.h"
#include "glerror.h"
#include "tkstexture.h"

extern sUI clid_Texture;
extern YAC_Host *yac_host;


