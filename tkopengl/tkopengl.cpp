/// tkopengl.cpp
///
/// (c) 2001-2025 by Bastian Spiegel <bs@tkscript.de>
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

#include "tks-list.h"
#include <yac_host.cpp>

#include "inc_opengl.h"
#include "tkopengl.h"

#include "bitmap.h"
#include "image.h"
#include "tkstexture.h"
#include "tksopengl.h"

#include "hook.h"

#ifdef DX_EMU_ATTRIB
#include "attrib.h"
#endif

#ifdef DX_EMU_MATRIX
#include "matrix.h"
#endif

#ifdef DX_EMU_IMM
#include "immediate.h"
#endif

#include "shader.h"

extern sUI clid_Texture;

YAC_Host *yac_host;

Dyac_std_exid_impl;


sBool *tksdl_b_glcore = NULL;
void tkopengl_lazyinit_tksdl_b_glcore(void) {
   // Resolve address of tksdl b_glcore flag  (GL 4.x core profile)
   tksdl_b_glcore = NULL;

   YAC_CallbackId id = yac_host->yacCallbackGetIdByName("tksdl_b_glcore");

   if(-1 != id)
   {
      YAC_CFunctionPtr addr = yac_host->yacCallbackGetFunById(id);

      if(NULL != addr)
      {
         tksdl_b_glcore = (sBool*) addr;
      }
   }
}

#ifdef TRACE_GL_ERROR
sBool *tksdl_b_glerror = NULL;

void tkopengl_lazyinit_tksdl_b_glerror(void) {
   // Resolve address of tksdl b_glerror flag
   tksdl_b_glerror = NULL;

   YAC_CallbackId id = yac_host->yacCallbackGetIdByName("tksdl_b_glerror");

   if(-1 != id)
   {
      YAC_CFunctionPtr addr = yac_host->yacCallbackGetFunById(id);

      if(NULL != addr)
      {
         tksdl_b_glerror = (sBool*) addr;
      }
   }
}
#endif // TRACE_GL_ERROR

/// ---------------------------------------------------------------------------
#include "ying_OpenGL.h"
#include "ying_OpenGL.cpp"
#include "ying_OpenGL_DisplayList.cpp"
#include "ying_Texture.cpp"
#include "ying_Texture_Texture.cpp"

#include "ying_Shader.h"
#include "ying_Shader.cpp"
#include "ying_Shader_ZGLShader.cpp"


// ---------------------------------------------------------------------------
#include "tkopengl_shared.h"
static YAC_CallbackId tkopengl_shared_callback_id;
static tkopengl_shared_t tkopengl_shared;

// --------------------------------------------------------------------------- loc_shared_export()
static void loc_shared_export(void) {
   tkopengl_shared_callback_id = yac_host->yacCallbackCreate(TKOPENGL_SHARED_NAME);
   tkopengl_shared._glActiveTexture            = &_glActiveTexture;
   tkopengl_shared._glAttachShader             = &_glAttachShader;
   tkopengl_shared._glBegin                    = &_glBegin;
   tkopengl_shared._glBindAttribLocation       = &_glBindAttribLocation_int;
   tkopengl_shared._glBindBuffer               = &_glBindBuffer;
   tkopengl_shared._glBindFramebuffer          = &_glBindFramebuffer;
   tkopengl_shared._glBindRenderbuffer         = &_glBindRenderbuffer;
   tkopengl_shared._glBindTexture              = &_glBindTexture;
   tkopengl_shared._glBindVertexArray          = &_glBindVertexArray;
   tkopengl_shared._glBlendColor               = &_glBlendColor;
   tkopengl_shared._glBlendEquation            = &_glBlendEquation;
   tkopengl_shared._glBlendFunc                = &_glBlendFunc;
   tkopengl_shared._glBlendFuncSeparate        = &_glBlendFuncSeparate;
   tkopengl_shared._glBlitFramebuffer          = &_glBlitFramebuffer;
   tkopengl_shared._glBufferData               = &_glBufferData;
   tkopengl_shared._glCheckFramebufferStatus   = &_glCheckFramebufferStatus;
   tkopengl_shared._glClear                    = &_glClear;
   tkopengl_shared._glClearColor               = &_glClearColor;
   tkopengl_shared._glClearDepth               = &_glClearDepth;
   tkopengl_shared._glClearStencil             = &_glClearStencil;
   tkopengl_shared._glColor3f                  = &_glColor3f;
   tkopengl_shared._glColor4f                  = &_glColor4f;
   tkopengl_shared._glColorMask                = &_glColorMask;
   tkopengl_shared._glCompileShader            = &_glCompileShader;
   tkopengl_shared._glCreateProgram            = &_glCreateProgram;
   tkopengl_shared._glCreateShader             = &_glCreateShader;
   tkopengl_shared._glCullFace                 = &_glCullFace;
   tkopengl_shared._glDeleteProgram            = &_glDeleteProgram;
   tkopengl_shared._glDeleteShader             = &_glDeleteShader;
   tkopengl_shared._glDepthFunc                = &_glDepthFunc;
   tkopengl_shared._glDepthMask                = &_glDepthMask;
   tkopengl_shared._glDetachShader             = &_glDetachShader;
   tkopengl_shared._glDisable                  = &_glDisable;
   tkopengl_shared._glDisableVertexAttribArray = &_glDisableVertexAttribArray;
   tkopengl_shared._glDrawArrays               = &_glDrawArrays;
   tkopengl_shared._glDrawArraysInstanced      = &_glDrawArraysInstanced;
   tkopengl_shared._glDrawBuffer               = &_glDrawBuffer;
   tkopengl_shared._glDrawElements             = &_glDrawElements_int;
   tkopengl_shared._glEnable                   = &_glEnable;
   tkopengl_shared._glEnableVertexAttribArray  = &_glEnableVertexAttribArray;
   tkopengl_shared._glEnd                      = &_glEnd;
   tkopengl_shared._glFinish                   = &_glFinish;
   tkopengl_shared._glFlush                    = &_glFlush;
   tkopengl_shared._glFlushMappedBufferRange   = &_glFlushMappedBufferRange;
   tkopengl_shared._glFramebufferRenderbuffer  = &_glFramebufferRenderbuffer;
   tkopengl_shared._glFramebufferTexture2D     = &_glFramebufferTexture2D;
   tkopengl_shared._glFrontFace                = &_glFrontFace;
   tkopengl_shared._glGetAttribLocation        = &_glGetAttribLocation_int;
   tkopengl_shared._glGetError                 = &_glGetError_int;
   tkopengl_shared._glGetProgramiv             = &_glGetProgramiv_int;
   tkopengl_shared._glGetProgramInfoLog        = &_glGetProgramInfoLog_int;
   tkopengl_shared._glGetShaderiv              = &_glGetShaderiv_int;
   tkopengl_shared._glGetShaderInfoLog         = &_glGetShaderInfoLog_int;
   tkopengl_shared._glGetUniformLocation       = &_glGetUniformLocation_int;
   tkopengl_shared._glHint                     = &_glHint;
   tkopengl_shared._glLightfv                  = &_glLightfv_int;
   tkopengl_shared._glLightModeli              = &_glLightModeli;
   tkopengl_shared._glLinkProgram              = &_glLinkProgram;
   tkopengl_shared._glLoadIdentity             = &_glLoadIdentity;
   tkopengl_shared._glLoadMatrixf              = &_glLoadMatrixf_int;
   tkopengl_shared._glMaterialfv               = &_glMaterialfv_int;
   tkopengl_shared._glMatrixMode               = &_glMatrixMode;
   tkopengl_shared._glMultMatrixf              = &_glMultMatrixf_int;
   tkopengl_shared._glNormal3f                 = &_glNormal3f;
   tkopengl_shared._glPixelStorei              = &_glPixelStorei_int;
   tkopengl_shared._glPointParameterf          = &_glPointParameterf;
   tkopengl_shared._glPointParameterfv         = &_glPointParameterfv_int;
   tkopengl_shared._glPointSize                = &_glPointSize;
   tkopengl_shared._glPopAttrib                = &_glPopAttrib;
   tkopengl_shared._glPopMatrix                = &_glPopMatrix;
   tkopengl_shared._glPushAttrib               = &_glPushAttrib;
   tkopengl_shared._glPushMatrix               = &_glPushMatrix;
   tkopengl_shared._glRenderbufferStorage      = &_glRenderbufferStorage;
   tkopengl_shared._glRenderbufferStorageMultisample = &_glRenderbufferStorageMultisample;  // ext
   tkopengl_shared._glScalef                   = &_glScalef;
   tkopengl_shared._glScissor                  = &_glScissor;
   tkopengl_shared._glShadeModel               = &_glShadeModel;
   tkopengl_shared._glShaderSource             = &_glShaderSource_int;
   tkopengl_shared._glStencilFunc              = &_glStencilFunc;
   tkopengl_shared._glStencilMask              = &_glStencilMask;
   tkopengl_shared._glStencilOp                = &_glStencilOp;
   tkopengl_shared._glTexCoord1f               = &_glTexCoord1f;
   tkopengl_shared._glTexCoord2f               = &_glTexCoord2f;
   tkopengl_shared._glTexCoord3f               = &_glTexCoord3f;
   tkopengl_shared._glTexEnvfv                 = &_glTexEnvfv_int;
   tkopengl_shared._glTexImage1D               = &_glTexImage1D_int;
   tkopengl_shared._glTexImage2D               = &_glTexImage2D_int;
   tkopengl_shared._glTexImage3D               = &_glTexImage3D_int;
   tkopengl_shared._glTexParameteri            = &_glTexParameteri;
   tkopengl_shared._glTexSubImage2D            = &_glTexSubImage2D_int;
   tkopengl_shared._glTranslatef               = &_glTranslatef;
   tkopengl_shared._glUniform1i                = &_glUniform1i;
   tkopengl_shared._glUniform2i                = &_glUniform2i;
   tkopengl_shared._glUniform3i                = &_glUniform3i;
   tkopengl_shared._glUniform4i                = &_glUniform4i;
   tkopengl_shared._glUniform1f                = &_glUniform1f;
   tkopengl_shared._glUniform2f                = &_glUniform2f;
   tkopengl_shared._glUniform3f                = &_glUniform3f;
   tkopengl_shared._glUniform4f                = &_glUniform4f;
   tkopengl_shared._glUniform2fv               = &_glUniform2fv_int;
   tkopengl_shared._glUniform3fv               = &_glUniform3fv_int;
   tkopengl_shared._glUniform4fv               = &_glUniform4fv_int;
   tkopengl_shared._glUniformMatrix2fv         = &_glUniformMatrix2fv_int;
   tkopengl_shared._glUniformMatrix3fv         = &_glUniformMatrix3fv_int;
   tkopengl_shared._glUniformMatrix4fv         = &_glUniformMatrix4fv_int;
   tkopengl_shared._glUseProgram               = &_glUseProgram_int;
   tkopengl_shared._glValidateProgram          = &_glValidateProgram;
   tkopengl_shared._glVertex2f                 = &_glVertex2f;
   tkopengl_shared._glVertex3f                 = &_glVertex3f;
   tkopengl_shared._glVertex4f                 = &_glVertex4f;
   tkopengl_shared._glVertexAttrib1f           = &_glVertexAttrib1f;
   tkopengl_shared._glVertexAttrib2f           = &_glVertexAttrib2f;
   tkopengl_shared._glVertexAttrib3f           = &_glVertexAttrib3f;
   tkopengl_shared._glVertexAttrib4f           = &_glVertexAttrib4f;
   tkopengl_shared._glVertexAttrib2fv          = &_glVertexAttrib2fv_int;
   tkopengl_shared._glVertexAttrib3fv          = &_glVertexAttrib3fv_int;
   tkopengl_shared._glVertexAttrib4fv          = &_glVertexAttrib4fv_int;
   tkopengl_shared._glVertexAttribDivisor      = &_glVertexAttribDivisor;
   tkopengl_shared._glVertexAttribPointer      = &_glVertexAttribPointer_int;
   tkopengl_shared._glUnmapBuffer              = &_glUnmapBuffer;
   tkopengl_shared._glViewport                 = &_glViewport;
   tkopengl_shared._project                    = &_project_int;
   tkopengl_shared._unproject                  = &_unproject_int;
   tkopengl_shared._zglBufferData              = &_zglBufferData;
   tkopengl_shared._zglBufferSubData           = &_zglBufferSubData;
   tkopengl_shared._zglBufferSubDataOffset     = &_zglBufferSubDataOffset;
   tkopengl_shared._zglClearColorARGB          = &_zglClearColorARGB;
   tkopengl_shared._zglColorARGB               = &_zglColorARGB;
   tkopengl_shared._zglDeleteBuffer            = &_zglDeleteBuffer;
   tkopengl_shared._zglDeleteFramebuffer       = &_zglDeleteFramebuffer;
   tkopengl_shared._zglDeleteRenderbuffer      = &_zglDeleteRenderbuffer;
   tkopengl_shared._zglDeleteTexture           = &_zglDeleteTexture;
   tkopengl_shared._zglDeleteVertexArray       = &_zglDeleteVertexArray;
   tkopengl_shared._zglEnableGLCore            = &_zglEnableGLCore;
   tkopengl_shared._zglFogAlphaMode            = &_zglFogAlphaMode;
   tkopengl_shared._zglFogColor4f              = &_zglFogColor4f;
   tkopengl_shared._zglFogDensity              = &_zglFogDensity;
   tkopengl_shared._zglFogEnd                  = &_zglFogEnd;
   tkopengl_shared._zglFogMode                 = &_zglFogMode;
   tkopengl_shared._zglFogStart                = &_zglFogStart;
   tkopengl_shared._zglGenBuffer               = &_zglGenBuffer;
   tkopengl_shared._zglGenFramebuffer          = &_zglGenFramebuffer;
   tkopengl_shared._zglGenRenderbuffer         = &_zglGenRenderbuffer;
   tkopengl_shared._zglGenTexture              = &_zglGenTexture;
   tkopengl_shared._zglGenVertexArray          = &_zglGenVertexArray;
   tkopengl_shared._zglGetBufferParameter      = &_zglGetBufferParameter;
   tkopengl_shared._zglLightAmbient4f                   = &_zglLightAmbient4f;
   tkopengl_shared._zglLightDiffuse4f                   = &_zglLightDiffuse4f;
   tkopengl_shared._zglLightSpecular4f                  = &_zglLightSpecular4f;
   tkopengl_shared._zglLightPositionf                   = &_zglLightPositionf;
   tkopengl_shared._zglLightSpotDirectionf              = &_zglLightSpotDirectionf;
   tkopengl_shared._zglLightSpotExponent                = &_zglLightSpotExponent;
   tkopengl_shared._zglLightSpotCutOff                  = &_zglLightSpotCutOff;
   tkopengl_shared._zglLightConstantAttenuation         = &_zglLightConstantAttenuation;
   tkopengl_shared._zglLightLinearAttenuation           = &_zglLightLinearAttenuation;
   tkopengl_shared._zglLightQuadraticAttenuation        = &_zglLightQuadraticAttenuation;
   tkopengl_shared._zglLightModelAmbient4f              = &_zglLightModelAmbient4f;
   tkopengl_shared._zglLightModelLocalViewer            = &_zglLightModelLocalViewer;
   tkopengl_shared._zglLightModelTwoSide                = &_zglLightModelTwoSide;
   tkopengl_shared._zglLightModelColorControl           = &_zglLightModelColorControl;
   tkopengl_shared._zglLightMaterialAmbient4f           = &_zglLightMaterialAmbient4f;
   tkopengl_shared._zglLightMaterialDiffuse4f           = &_zglLightMaterialDiffuse4f;
   tkopengl_shared._zglLightMaterialSpecular4f          = &_zglLightMaterialSpecular4f;
   tkopengl_shared._zglLightMaterialEmission4f          = &_zglLightMaterialEmission4f;
   tkopengl_shared._zglLightMaterialShininess           = &_zglLightMaterialShininess;
   tkopengl_shared._zglLightMaterialAmbientAndDiffuse4f = &_zglLightMaterialAmbientAndDiffuse4f;
   tkopengl_shared._zglLoadExtensions      = &_zglLoadExtensions;
   tkopengl_shared._zglMapBuffer           = &_zglMapBuffer;
   tkopengl_shared._zglMapBufferRange      = &_zglMapBufferRange;
   tkopengl_shared._zglPointParameter3f    = &_zglPointParameter3f;
   tkopengl_shared._zglShaderSource        = &_zglShaderSource;
   tkopengl_shared._zglStoreMatrixf        = &_zglStoreMatrixf_int;
   tkopengl_shared._zglVertexAttribOffset  = &_zglVertexAttribOffset;

   (void)yac_host->yacCallbackSetFunById(tkopengl_shared_callback_id, (YAC_CFunctionPtr) &tkopengl_shared);
}

// ---------------------------------------------------------------------------
void YAC_CALL YAC_Init(YAC_Host *_host) {
	if(_host->yacGetDebugLevel() > 1)
		_host->printf("[dbg] tkopengl::YAC_Init called.\n");
	yac_host = _host;

   Dyac_std_exid_resolve;

   tkopengl_b_glcore = YAC_FALSE;

	YAC_Init_OpenGL  (_host);
	YAC_Init_Texture (_host);
   YAC_Init_Shader  (_host);

   // -DDX_GL4_CORE: force #version 410 core
   // macOS default: force #version 120
   _ZGLShader::SetDefaultPlatformVersion();

   tkopengl_vao_init();

#ifdef DX_EMU_MATRIX
   tkopengl_matrix_init();
#endif

#ifdef DX_EMU_IMM
   tkopengl_imm_init();
#endif

   // Export shared functions
   loc_shared_export();

	if(yac_host->yacGetDebugLevel() > 1)
		yac_host->yacPrint("[dbg] tkopengl::YAC_Init() finished.\n");

   // Add tksdl viewport open/close hooks
   tkopengl_hook_install();
}

void YAC_CALL YAC_Exit(YAC_Host *_host) {

   // Remove tksdl viewport open/close hooks
   //  (note) this is to prevent hard crash when UI application exits via die() and tkopengl is unloaded before tksdl
   tkopengl_hook_remove();

#ifdef DX_EMU_IMM
   tkopengl_imm_exit();
#endif
	YAC_Exit_Shader  (_host);
	YAC_Exit_Texture (_host);
	YAC_Exit_OpenGL  (_host);

	if(yac_host->yacGetDebugLevel() > 1)
		yac_host->yacPrint("[dbg] tkopengl::YAC_Exit() finished.\n");
}

sUI YAC_CALL YAC_Version(void) {
	return 0x00090B00;
}
