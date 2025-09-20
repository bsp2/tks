/// glerror.cpp
///
/// (c) 2001-2014 Bastian Spiegel <bs@tkscript.de> 
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#include <stdlib.h>
#include <math.h>

#define YAC_PRINTF defined
#include <yac.h>

extern YAC_Host *yac_host;

#include "inc_opengl.h"

#include "glerror.h"


#ifdef TRACE_GL_ERROR
// flag that is used to detected whether it is safe to call glGetError()
sBool tkopengl_b_glBegin = 0;

// (note) need to store last GL error code so it can be reported to the app. when TRACE_GL_ERROR is enabled
GLenum last_gl_error = GL_NO_ERROR;


void tkopengl_handle_gl_error(GLenum _err, const char *_fxnName) {
   const char *errString = "n/a";
   switch(_err)
   {
      case GL_INVALID_ENUM:
         errString = "GL_INVALID_ENUM";
         break;
      case GL_INVALID_VALUE:
         errString = "GL_INVALID_VALUE";
         break;
      case GL_INVALID_OPERATION:
         errString = "GL_INVALID_OPERATION";
         break;
      case GL_STACK_OVERFLOW:
         errString = "GL_STACK_OVERFLOW";
         break;
      case GL_STACK_UNDERFLOW:
         errString = "GL_INVALID_UNDERFLOW";
         break;
      case GL_OUT_OF_MEMORY:
         errString = "GL_OUT_OF_MEMORY";
         break;
   }
   yac_host->printf("[~~~] tkopengl: GL error %d (\"%s\") occured after call to \"%s\".\n", _err, errString, _fxnName);
}
#endif // TRACE_GL_ERROR
