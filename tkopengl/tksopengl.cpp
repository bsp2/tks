/// tksopengl.cpp
///
/// (c) 2001-2025 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#include <stdlib.h>
#include <math.h>
#include <limits.h>

#define YAC_PRINTF defined
#include <yac.h>

extern YAC_Host *yac_host;

#define Dtrace_call  if(1);else Dyac_host_printf

#include "inc_opengl.h"
#include "tkopengl.h"
#include "glerror.h"
#include "tks-list.h"

#include "bitmap.h"
#include "image.h"
#include "tkstexture.h"
#include "tksopengl.h"

#ifdef DX_EMU_ATTRIB
#include "attrib.h"
#endif

#ifdef DX_EMU_MATRIX
#include "matrix.h"
#endif

#ifdef DX_EMU_IMM
#include "immediate.h"
#endif

#ifdef DX_EMU_FIXFXN
#include "shader.h"
#endif


#ifdef DX_QUIET_STUBS
//#define Dstub(a) void(0)
#define Dstub(a) \
   last_gl_error = GL_INVALID_OPERATION

#else

//#define Dstub(a) void(0)
#define Dstub(a) \
   yac_host->printf("[---] tkopengl: function stub \"%s\" called.\n", #a); \
   last_gl_error = GL_INVALID_OPERATION

#endif // DX_LOG_STUBS


//
// Utility macros to handle extension functions
//
#define Dresolveext(a) a##_xref = (a##_t) int_GetProcAddressA3(#a, #a "ARB", #a "EXT")

#define Ddbghaveext(a, n) \
      if(yac_host->yacGetDebugLevel() > 0)\
      {\
         yac_host->printf("[dbg]      " n " extension: %s\n",\
                          ((a##_xref) != NULL) ? "FOUND" : "!!NOT PRESENT!!");\
      }\
      (void)0

#define Dcheckext(a) if(NULL == a##_xref) \
   {                                                                    \
      yac_host->printf("[---] tkopengl: unresolved extension function \"%s\" called.\n", #a); \
      last_gl_error = GL_INVALID_OPERATION;                             \
   }                                                                    \
   else

#define Dglesbuiltin(a) a##_xref = (a##_t)&a

#define Dcompat(a) if(tkopengl_b_glcore) \
   {                                                                    \
      yac_host->printf("[---] tkopengl: illegal call to compatibility-profile function \"%s\" in core-profile context\n", #a); \
      last_gl_error = GL_INVALID_OPERATION;                             \
   }                                                                    \
   else

sBool tkopengl_b_glcore = YAC_FALSE;

glBindBuffer_t              glBindBuffer_xref              = NULL;
glIsBuffer_t                glIsBuffer_xref                = NULL;
glDeleteBuffers_t           glDeleteBuffers_xref           = NULL;
glGenBuffers_t              glGenBuffers_xref              = NULL;
glBufferData_t              glBufferData_xref              = NULL;
glBufferSubData_t           glBufferSubData_xref           = NULL;
glDrawArraysInstanced_t     glDrawArraysInstanced_xref     = NULL;
glGetBufferParameteriv_t    glGetBufferParameteriv_xref    = NULL;
glMapBuffer_t               glMapBuffer_xref               = NULL;
glMapBufferRange_t          glMapBufferRange_xref          = NULL;
glFlushMappedBufferRange_t  glFlushMappedBufferRange_xref  = NULL;
glUnmapBuffer_t             glUnmapBuffer_xref             = NULL;
glTexImage3D_t              glTexImage3D_xref              = NULL;
glTexStorage2D_t            glTexStorage2D_xref            = NULL;
glTexImage2DMultisample_t   glTexImage2DMultisample_xref   = NULL;
glTexStorage2DMultisample_t glTexStorage2DMultisample_xref = NULL;
glTexStorage3D_t            glTexStorage3D_xref            = NULL;
glTexImage3DMultisample_t   glTexImage3DMultisample_xref   = NULL;
glTexStorage3DMultisample_t glTexStorage3DMultisample_xref = NULL;
glCopyTexSubImage3D_t       glCopyTexSubImage3D_xref       = NULL;
glActiveTexture_t           glActiveTexture_xref           = NULL;
glClientActiveTexture_t     glClientActiveTexture_xref     = NULL;
glMultiTexCoord1f_t         glMultiTexCoord1f_xref         = NULL;
glMultiTexCoord2f_t         glMultiTexCoord2f_xref         = NULL;
glMultiTexCoord3f_t         glMultiTexCoord3f_xref         = NULL;
glBlendFuncSeparate_t       glBlendFuncSeparate_xref       = NULL;
glBlendEquation_t           glBlendEquation_xref           = NULL;
glBlendColor_t              glBlendColor_xref              = NULL;

// --- Framebuffer objects (FBO) ----
glIsRenderbuffer_t                      glIsRenderbuffer_xref                      = NULL;
glBindRenderbuffer_t                    glBindRenderbuffer_xref                    = NULL;
glDeleteRenderbuffers_t                 glDeleteRenderbuffers_xref                 = NULL;
glGenRenderbuffers_t                    glGenRenderbuffers_xref                    = NULL;
glRenderbufferStorage_t                 glRenderbufferStorage_xref                 = NULL;
glRenderbufferStorageMultisample_t      glRenderbufferStorageMultisample_xref      = NULL;
glGetRenderbufferParameteriv_t          glGetRenderbufferParameteriv_xref          = NULL;
glIsFramebuffer_t                       glIsFramebuffer_xref                       = NULL;
glBindFramebuffer_t                     glBindFramebuffer_xref                     = NULL;
glDeleteFramebuffers_t                  glDeleteFramebuffers_xref                  = NULL;
glGenFramebuffers_t                     glGenFramebuffers_xref                     = NULL;
glCheckFramebufferStatus_t              glCheckFramebufferStatus_xref              = NULL;
glFramebufferTexture1D_t                glFramebufferTexture1D_xref                = NULL;
glFramebufferTexture2D_t                glFramebufferTexture2D_xref                = NULL;
glFramebufferTexture3D_t                glFramebufferTexture3D_xref                = NULL;
glFramebufferRenderbuffer_t             glFramebufferRenderbuffer_xref             = NULL;
glGetFramebufferAttachmentParameteriv_t glGetFramebufferAttachmentParameteriv_xref = NULL;
glBlitFramebuffer_t                     glBlitFramebuffer_xref                     = NULL;
glGenerateMipmap_t                      glGenerateMipmap_xref                      = NULL;

glDrawBuffers_t              glDrawBuffers_xref              = NULL;

// --- GLSL ---
glCreateProgram_t            glCreateProgram_xref            = NULL;
glDeleteProgram_t            glDeleteProgram_xref            = NULL;
glCreateShader_t             glCreateShader_xref             = NULL;
glDeleteShader_t             glDeleteShader_xref             = NULL;
glShaderSource_t             glShaderSource_xref             = NULL;
glCompileShader_t            glCompileShader_xref            = NULL;
glAttachShader_t             glAttachShader_xref             = NULL;
glDetachShader_t             glDetachShader_xref             = NULL;
glLinkProgram_t              glLinkProgram_xref              = NULL;
glValidateProgram_t          glValidateProgram_xref          = NULL;
glGetShaderiv_t              glGetShaderiv_xref              = NULL;
glGetShaderInfoLog_t         glGetShaderInfoLog_xref         = NULL;
glGetProgramInfoLog_t        glGetProgramInfoLog_xref        = NULL;
glGetProgramiv_t             glGetProgramiv_xref             = NULL;
glUseProgram_t               glUseProgram_xref               = NULL;
glBindAttribLocation_t       glBindAttribLocation_xref       = NULL;
glGetAttribLocation_t        glGetAttribLocation_xref        = NULL;
glBindFragDataLocation_t     glBindFragDataLocation_xref     = NULL;
glBindFragDataLocationIndexed_t glBindFragDataLocationIndexed_xref = NULL;
glGetFragDataLocation_t      glGetFragDataLocation_xref      = NULL;
glGetFragDataIndex_t         glGetFragDataIndex_xref         = NULL;
glVertexAttrib1f_t           glVertexAttrib1f_xref           = NULL;
glVertexAttrib2f_t           glVertexAttrib2f_xref           = NULL;
glVertexAttrib3f_t           glVertexAttrib3f_xref           = NULL;
glVertexAttrib4f_t           glVertexAttrib4f_xref           = NULL;
glVertexAttrib4Nub_t         glVertexAttrib4Nub_xref         = NULL;
glVertexAttrib2fv_t          glVertexAttrib2fv_xref          = NULL;
glVertexAttrib3fv_t          glVertexAttrib3fv_xref          = NULL;
glVertexAttrib4fv_t          glVertexAttrib4fv_xref          = NULL;
glVertexAttribPointer_t      glVertexAttribPointer_xref      = NULL;
glEnableVertexAttribArray_t  glEnableVertexAttribArray_xref  = NULL;
glDisableVertexAttribArray_t glDisableVertexAttribArray_xref = NULL;
glVertexAttribDivisor_t      glVertexAttribDivisor_xref      = NULL;
glGetUniformLocation_t       glGetUniformLocation_xref       = NULL;
glGetActiveUniform_t         glGetActiveUniform_xref         = NULL;
glUniform1f_t                glUniform1f_xref                = NULL;
glUniform2f_t                glUniform2f_xref                = NULL;
glUniform3f_t                glUniform3f_xref                = NULL;
glUniform4f_t                glUniform4f_xref                = NULL;
glUniform2fv_t               glUniform2fv_xref               = NULL;
glUniform3fv_t               glUniform3fv_xref               = NULL;
glUniform4fv_t               glUniform4fv_xref               = NULL;
glUniform1i_t                glUniform1i_xref                = NULL;
glUniform2i_t                glUniform2i_xref                = NULL;
glUniform3i_t                glUniform3i_xref                = NULL;
glUniform4i_t                glUniform4i_xref                = NULL;
glUniformMatrix2fv_t         glUniformMatrix2fv_xref         = NULL;
glUniformMatrix3fv_t         glUniformMatrix3fv_xref         = NULL;
glUniformMatrix4fv_t         glUniformMatrix4fv_xref         = NULL;


// --- samplers ---
glGenSamplers_t       glGenSamplers_xref       = NULL;
glDeleteSamplers_t    glDeleteSamplers_xref    = NULL;
glBindSampler_t       glBindSampler_xref       = NULL;
glSamplerParameteri_t glSamplerParameteri_xref = NULL;


// --- point sprites ---
glPointParameterf_t   glPointParameterf_xref  = NULL;
glPointParameteriv_t  glPointParameteriv_xref = NULL;
glPointParameterfv_t  glPointParameterfv_xref = NULL;

// --- Vertex Array Objects (VAO) ----
glBindVertexArray_t    glBindVertexArray_xref    = NULL;
glDeleteVertexArrays_t glDeleteVertexArrays_xref = NULL;
glGenVertexArrays_t    glGenVertexArrays_xref    = NULL;
glIsVertexArray_t      glIsVertexArray_xref      = NULL;

// ---- glGetStringi
glGetStringi_t glGetStringi_xref = NULL;


// Currently bound VAO (0u = none)
sUI tkopengl_current_vao_id = 0u;
sUI tkopengl_builtin_vao_id = 0u;

void tkopengl_vao_init(void) {
   tkopengl_current_vao_id = 0u;
   tkopengl_builtin_vao_id = 0u;
}

#ifdef DX_EMU_USE_VAO

static sBool loc_lazy_create_builtin_vao(const char *_fxnName) {
   if(0u == tkopengl_builtin_vao_id)
   {
      tkopengl_builtin_vao_id = _zglGenVertexArray();
      if(0u == tkopengl_builtin_vao_id)
      {
         yac_host->printf("[---] tkopengl::%s: zglGenVertexArray() failed\n", _fxnName);
      }
   }
   return (tkopengl_builtin_vao_id > 0u);
}

void _glVertexAttribPointer_int_vao(GLuint _index, GLint _size, GLenum _type, GLboolean _normalized, GLsizei _stride, const GLvoid *_pointer) {
   if(0u == tkopengl_current_vao_id)
   {
      if(_index < TKOPENGL_MAX_GEN_VTX_ATTRIBS)
      {
         if(loc_lazy_create_builtin_vao("glVertexAttribPointer_int_vao"))
         {
            _glBindVertexArray(tkopengl_builtin_vao_id);
            _glVertexAttribPointer_int(_index, _size, _type, _normalized, _stride, _pointer);
            _glBindVertexArray(0u);
         }
      }
      else
      {
         yac_host->printf("[---] tkopengl::glVertexAttribPointer_int_vao: index out of bounds (%u >= %u)\n", _index, TKOPENGL_MAX_GEN_VTX_ATTRIBS);
      }
   }
   else
   {
      // application uses VAOs, cannot mix this with DX_EMU_USE_VAO
      yac_host->printf("[---] tkopengl::glVertexAttribPointer_int_vao: VAO already bound (id=%u)\n", tkopengl_current_vao_id);
   }
}

#else
// no VAOs
void _glVertexAttribPointer_int_vao(GLuint _index, GLint _size, GLenum _type, GLboolean _normalized, GLsizei _stride, const GLvoid *_pointer) {
   _glVertexAttribPointer_int(_index, _size, _type, _normalized, _stride, _pointer);
}
#endif // DX_EMU_USE_VAO




#ifdef YAC_MACOS
// see https://developer.apple.com/library/archive/documentation/GraphicsImaging/Conceptual/OpenGL-MacProgGuide/opengl_entrypts/opengl_entrypts.html
// see procaddress_macos.m
extern "C" void * MyNSGLGetProcAddress (const char *name);
#endif // YAC_MACOS


#ifndef DX_GLES
static glanyfun_t int_GetProcAddress(const char *_name) {
#ifdef YAC_WIN32
   return (glanyfun_t) wglGetProcAddress(_name);
#elif defined(YAC_MACOS)
   return (glanyfun_t) MyNSGLGetProcAddress(_name);
#else
   return (glanyfun_t) glXGetProcAddress((const GLubyte*)_name);
#endif // YAC_WIN32
}

static glanyfun_t int_GetProcAddressA3(const char *_name1, const char *_name2, const char *_name3) {
   glanyfun_t r = NULL;

   if(NULL != _name1)
   {
      r = int_GetProcAddress(_name1);
   }

   if(NULL == r)
   {
      if(NULL != _name2)
      {
         r = int_GetProcAddress(_name2);
      }
   }

   if(NULL == r)
   {
      if(NULL != _name3)
      {
         r = int_GetProcAddress(_name3);
      }
   }

   if(NULL == r)
   {
      yac_host->printf("[---] tkopengl: failed to resolve extension fxn \"%s\" / \"%s\" / \"%s\".\n", _name1, _name2, _name3);
   }

   return r;
}
#endif // DX_GLES

// ---------------------------------------------------------------------------- zglLoadExtensions
void YAC_CALL _zglLoadExtensions(void) {
   if(NULL == glBindBuffer_xref)
   {
      if(yac_host->yacGetDebugLevel() > 0)
      {
         yac_host->printf("[dbg] tkopengl::zglLoadExtensions:\n");
      }

      // Buffer objects
#ifdef DX_GLES
      Dglesbuiltin(glBindBuffer);
      Dglesbuiltin(glBindBuffer);
      Dglesbuiltin(glIsBuffer);
      Dglesbuiltin(glDeleteBuffers);
      Dglesbuiltin(glGenBuffers);
      Dglesbuiltin(glBufferData);
      Dglesbuiltin(glBufferSubData);
      Dglesbuiltin(glGetBufferParameteriv);
      glMapBuffer_xref     = NULL;
      glUnmapBuffer_xref   = NULL;
#else
      Dresolveext(glBindBuffer);
      Dresolveext(glIsBuffer);
      Dresolveext(glDeleteBuffers);
      Dresolveext(glGenBuffers);
      Dresolveext(glBufferData);
      Dresolveext(glBufferSubData);
      Dresolveext(glGetBufferParameteriv);
      Dresolveext(glMapBuffer);
      Dresolveext(glUnmapBuffer);
      Dresolveext(glMapBufferRange);
      Dresolveext(glFlushMappedBufferRange);
#endif // DX_GLES

      Ddbghaveext(glBindBuffer, " buffer object (VBO, PBO)");

#ifndef DX_GLES
      Dresolveext(glDrawBuffers); // GL_FRONT/GL_BACK/..
#endif

      // Multitexturing
#ifdef DX_GLES
      Dglesbuiltin(glActiveTexture);
#else
      Dresolveext(glActiveTexture);
      Dresolveext(glClientActiveTexture);
      Dresolveext(glMultiTexCoord1f);
      Dresolveext(glMultiTexCoord2f);
#endif // DX_GLES

      Ddbghaveext(glActiveTexture,   "          multi texturing");

      // 3D textures
#ifdef DX_GLES
      // (todo) gles 3.x
#else
      Dresolveext(glTexImage3D);
      Dresolveext(glTexImage3DMultisample);    // GL3.2
      Dresolveext(glCopyTexSubImage3D);
      Dresolveext(glMultiTexCoord3f);

      Dresolveext(glTexImage2DMultisample);    // GL3.2

      Dresolveext(glTexStorage2D);             // GL4.2
#ifndef YAC_MACOS
      Dresolveext(glTexStorage2DMultisample);  // GL4.2
#endif // YAC_MACOS

      Dresolveext(glTexStorage3D);             // GL4.2
#ifndef YAC_MACOS
      Dresolveext(glTexStorage3DMultisample);  // GL4.2
#endif // YAC_MACOS
#endif // DX_GLES

      Ddbghaveext(glTexImage3D, "               3D texture");

      // Extended blend functions
#ifdef DX_GLES
      Dglesbuiltin(glBlendFuncSeparate);
      Dglesbuiltin(glBlendEquation);
      Dglesbuiltin(glBlendColor);
#else
      Dresolveext(glBlendFuncSeparate);
      Dresolveext(glBlendEquation);
      Dresolveext(glBlendColor);
#endif // DX_GLES

      Ddbghaveext(glBlendFuncSeparate, "               blend func");

      // Framebuffer objects (FBO)
#ifdef DX_GLES
      Dglesbuiltin(glIsRenderbuffer);
      Dglesbuiltin(glBindRenderbuffer);
      Dglesbuiltin(glDeleteRenderbuffers);
      Dglesbuiltin(glGenRenderbuffers);
      Dglesbuiltin(glRenderbufferStorage);
      //Dglesbuiltin(glRenderbufferStorageMultisample);
      Dglesbuiltin(glGetRenderbufferParameteriv);
      Dglesbuiltin(glIsFramebuffer);
      Dglesbuiltin(glBindFramebuffer);
      Dglesbuiltin(glDeleteFramebuffers);
      Dglesbuiltin(glGenFramebuffers);
      Dglesbuiltin(glCheckFramebufferStatus);
      ////Dglesbuiltin(glFramebufferTexture1D);
      Dglesbuiltin(glFramebufferTexture2D);
      ////Dglesbuiltin(glFramebufferTexture3D);
      Dglesbuiltin(glFramebufferRenderbuffer);
      Dglesbuiltin(glGetFramebufferAttachmentParameteriv);

#if defined(DX_GLES_3)
#ifndef DX_DNX_POKY
      Dglesbuiltin(glBlitFramebuffer);
#else
      // (todo) implement
#pragma message "skipping Dglesbuiltin(glBlitFramebuffer) (GLES3.x)"
#endif // DX_DNX_POKY
#endif // DX_GLES_3

      Dglesbuiltin(glGenerateMipmap);
#else
      Dresolveext(glIsRenderbuffer);
      Dresolveext(glBindRenderbuffer);
      Dresolveext(glDeleteRenderbuffers);
      Dresolveext(glGenRenderbuffers);
      Dresolveext(glRenderbufferStorage);
      Dresolveext(glRenderbufferStorageMultisample);
      Dresolveext(glGetRenderbufferParameteriv);
      Dresolveext(glIsFramebuffer);
      Dresolveext(glBindFramebuffer);
      Dresolveext(glDeleteFramebuffers);
      Dresolveext(glGenFramebuffers);
      Dresolveext(glCheckFramebufferStatus);
      Dresolveext(glFramebufferTexture1D);
      Dresolveext(glFramebufferTexture2D);
      Dresolveext(glFramebufferTexture3D);
      Dresolveext(glFramebufferRenderbuffer);
      Dresolveext(glGetFramebufferAttachmentParameteriv);
      Dresolveext(glBlitFramebuffer);
      Dresolveext(glGenerateMipmap);
#endif // DX_GLES

      Ddbghaveext(glBindFramebuffer, "frame buffer object (FBO)");

      // GLSL
#ifdef DX_GLES
      Dglesbuiltin(glCreateProgram);
      Dglesbuiltin(glDeleteProgram);
      Dglesbuiltin(glCreateShader);
      Dglesbuiltin(glDeleteShader);
      Dglesbuiltin(glShaderSource);
      Dglesbuiltin(glCompileShader);
      Dglesbuiltin(glAttachShader);
      Dglesbuiltin(glDetachShader);
      Dglesbuiltin(glLinkProgram);
      Dglesbuiltin(glValidateProgram);
      Dglesbuiltin(glGetShaderiv);
      Dglesbuiltin(glGetShaderInfoLog);
      Dglesbuiltin(glGetProgramInfoLog);
      Dglesbuiltin(glGetProgramiv);
      Dglesbuiltin(glUseProgram);
      Dglesbuiltin(glBindAttribLocation);
      Dglesbuiltin(glGetAttribLocation);
      ////Dglesbuiltin(glBindFragDataLocation);
      ////Dglesbuiltin(glBindFragDataLocationIndexed);
      ////Dglesbuiltin(glGetFragDataLocation);
      ////Dglesbuiltin(glGetFragDataIndex);
      Dglesbuiltin(glVertexAttrib1f);
      Dglesbuiltin(glVertexAttrib2f);
      Dglesbuiltin(glVertexAttrib3f);
      Dglesbuiltin(glVertexAttrib4f);
      ////Dglesbuiltin(glVertexAttrib4Nub);
      Dglesbuiltin(glVertexAttrib2fv);
      Dglesbuiltin(glVertexAttrib3fv);
      Dglesbuiltin(glVertexAttrib4fv);
      Dglesbuiltin(glVertexAttribPointer);
      Dglesbuiltin(glEnableVertexAttribArray);
      Dglesbuiltin(glDisableVertexAttribArray);
      Dglesbuiltin(glGetUniformLocation);
      Dglesbuiltin(glGetActiveUniform);
      Dglesbuiltin(glUniform1f);
      Dglesbuiltin(glUniform2f);
      Dglesbuiltin(glUniform3f);
      Dglesbuiltin(glUniform4f);
      Dglesbuiltin(glUniform2fv);
      Dglesbuiltin(glUniform3fv);
      Dglesbuiltin(glUniform4fv);
      Dglesbuiltin(glUniform1i);
      Dglesbuiltin(glUniform2i);
      Dglesbuiltin(glUniform3i);
      Dglesbuiltin(glUniform4i);
      Dglesbuiltin(glUniformMatrix2fv);
      Dglesbuiltin(glUniformMatrix3fv);
      Dglesbuiltin(glUniformMatrix4fv);
#else
      Dresolveext(glCreateProgram);
      Dresolveext(glDeleteProgram);
      Dresolveext(glCreateShader);
      Dresolveext(glDeleteShader);
      Dresolveext(glShaderSource);
      Dresolveext(glCompileShader);
      Dresolveext(glAttachShader);
      Dresolveext(glDetachShader);
      Dresolveext(glLinkProgram);
      Dresolveext(glValidateProgram);
      Dresolveext(glGetShaderiv);
      Dresolveext(glGetShaderInfoLog);
      Dresolveext(glGetProgramInfoLog);
      Dresolveext(glGetProgramiv);
      Dresolveext(glUseProgram);
      Dresolveext(glBindAttribLocation);
      Dresolveext(glGetAttribLocation);
      Dresolveext(glBindFragDataLocation);
      Dresolveext(glBindFragDataLocationIndexed);
      Dresolveext(glGetFragDataLocation);
      Dresolveext(glGetFragDataIndex);
      Dresolveext(glVertexAttrib1f);
      Dresolveext(glVertexAttrib2f);
      Dresolveext(glVertexAttrib3f);
      Dresolveext(glVertexAttrib4f);
      Dresolveext(glVertexAttrib4Nub);
      Dresolveext(glVertexAttrib2fv);
      Dresolveext(glVertexAttrib3fv);
      Dresolveext(glVertexAttrib4fv);
      Dresolveext(glVertexAttribPointer);
      Dresolveext(glEnableVertexAttribArray);
      Dresolveext(glDisableVertexAttribArray);
      Dresolveext(glGetUniformLocation);
      Dresolveext(glGetActiveUniform);
      Dresolveext(glUniform1f);
      Dresolveext(glUniform2f);
      Dresolveext(glUniform3f);
      Dresolveext(glUniform4f);
      Dresolveext(glUniform2fv);
      Dresolveext(glUniform3fv);
      Dresolveext(glUniform4fv);
      Dresolveext(glUniform1i);
      Dresolveext(glUniform2i);
      Dresolveext(glUniform3i);
      Dresolveext(glUniform4i);
      Dresolveext(glUniformMatrix2fv);
      Dresolveext(glUniformMatrix3fv);
      Dresolveext(glUniformMatrix4fv);
#endif // DX_GLES
      Ddbghaveext(glCreateProgram, "                     GLSL");

      // samplers
#ifdef DX_GLES
      // (todo) gles
#else
      Dresolveext(glGenSamplers);
      Dresolveext(glDeleteSamplers);
      Dresolveext(glBindSampler);
      Dresolveext(glSamplerParameteri);
#endif // DX_GLES

      // point sprites
#ifdef DX_GLES
#else
      Dresolveext(glPointParameterf);
      Dresolveext(glPointParameterfv);
      Dresolveext(glPointParameteriv);
#endif // DX_GLES

      // vertex array objects (VAO)
      Dresolveext(glBindVertexArray);
      Dresolveext(glDeleteVertexArrays);
      Dresolveext(glGenVertexArrays);
      Dresolveext(glIsVertexArray);
      Ddbghaveext(glBindVertexArray, "vertex array object (VAO)");

      // GL3.3+, GLES3.0+
#ifdef DX_GLES_3
      Dglesbuiltin(glVertexAttribDivisor);
      Dglesbuiltin(glDrawArraysInstanced);
#else
      Dresolveext(glVertexAttribDivisor);
      Dresolveext(glDrawArraysInstanced);
#endif

      // glGetStringi
      Dresolveext(glGetStringi);
   }
}

// ---------------------------------------------------------------------------- loc_find_texnr_by_id()
#ifdef DX_EMU_ATTRIB_TEXTURE_ID_MAP
static sUI loc_find_texnr_by_id(GLuint _id) {
   sUI ret = (sUI)-1;

   if((GLuint)-1 != _id)
   {
      sUI texNr = 0;

      while(texNr < TKOPENGL_MAX_TEXTURES)
      {
         if(_id == tkopengl_textures[texNr].id)
         {
            ret = texNr;
            break;
         }

         texNr++;
      }
   }

   return ret;
}
#endif // DX_EMU_ATTRIB_TEXTURE_ID_MAP


// ---------------------------------------------------------------------------- loc_decompose_internalformat
static void loc_decompose_internalformat(GLenum _internalFormat, GLint &num, GLenum &baseFormat, GLenum &type) {
   // Called by glTexStorage*() fallback to glTexImage*()
   switch(_internalFormat)
   {
      default:
         yac_host->printf("[~~~] tkopengl::loc_internalformat_to_num_and_base: unhandled internalFormat=0x%08x, falling back to GL_RGBA8..\n", _internalFormat);
         num = 4; baseFormat = GL_RGBA; type = GL_UNSIGNED_BYTE;
         break;

      case GL_R8:
         num = 1; baseFormat = GL_RED;  type = GL_UNSIGNED_BYTE;
         break;

      case GL_RG8:
         num = 2; baseFormat = GL_RG;   type = GL_UNSIGNED_BYTE;
         break;

      case GL_RGB8:
         num = 3; baseFormat = GL_RGB;  type = GL_UNSIGNED_BYTE;
         break;

      case GL_RGBA8:
         num = 4; baseFormat = GL_RGBA; type = GL_UNSIGNED_BYTE;
         break;
   }
}


// ---------------------------------------------------------------------------- zglEnableGLCore
void YAC_CALL _zglEnableGLCore(sBool _bEnable) {
   tkopengl_b_glcore = _bEnable;
}

// ---------------------------------------------------------------------------- glActiveTexture
void YAC_CALL _glActiveTexture(sSI _texture) {

#ifdef DX_EMU_ATTRIB
   switch(_texture)
   {
      default:
         yac_host->printf("[---] tkopengl::glActiveTexture: unhandled texture unit 0x%04x\n", _texture);
         break;

      case GL_TEXTURE0:
      case GL_TEXTURE1:
      case GL_TEXTURE2:
      case GL_TEXTURE3:
      case GL_TEXTURE4:
      case GL_TEXTURE5:
      case GL_TEXTURE6:
      case GL_TEXTURE7:
         tkopengl_attrib->texture.active_unit = (sUI) (_texture - GL_TEXTURE0);
         break;
   }
#endif // DX_EMU_ATTRIB

   Dcheckext(glActiveTexture)
   {
      ::glActiveTexture_xref(_texture);
      Dtraceglerror("glActiveTexture");
   }
}

// ---------------------------------------------------------------------------- glAlphaFunc
void YAC_CALL _glAlphaFunc(sSI _func, sF32 _ref) {
#ifdef DX_EMU_FIXFXN
   tkopengl_attrib->alphatest.func = _func;
   tkopengl_attrib->alphatest.ref  = _ref;
#else
#ifdef DX_GLES
   Dstub(glAlphaFunc);
#else
   Dcompat(glAlphaFunc)
   {
      ::glAlphaFunc(_func, _ref);
      Dtraceglerror("glAlphaFunc");
   }
#endif // DX_GLES
#endif // DX_EMU_FIXFXN
}

// ---------------------------------------------------------------------------- glArrayElement
void YAC_CALL _glArrayElement(sSI _a) {
#ifdef DX_EMU_IMM
   // (todo) immediate
   Dstub(glArrayElement);
#else
#ifdef DX_GLES
   Dstub(glArrayElement);
#else
   Dcompat(glArrayElement)
   {
      ::glArrayElement(_a);
      Dtracegldrawerror("glArrayElement");
   }
#endif // DX_GLES
#endif // DX_EMU_IMM
}

// ---------------------------------------------------------------------------- glBegin
void YAC_CALL _glBegin(sSI _mode) {
   Dtrace_call("xxx glBegin(mode=%d) -----------------------\n", _mode);
#ifndef DONT_DRAW
   if(!tkopengl_b_glBegin)
   {
      tkopengl_b_glBegin = YAC_TRUE;
   }
   else
   {
      yac_host->printf("[~~~] tkopengl::glBegin: calling glBegin() again before glEnd().\n");
      //last_gl_error = GL_INVALID_OPERATION;
   }

   switch(_mode)
   {
      default:
         yac_host->printf("[---] tkopengl::glBegin: unhandled prim type 0x%04x\n", (sU16)_mode);
         last_gl_error = GL_INVALID_ENUM;
         break;

      case GL_POINTS:
      case GL_LINE_STRIP:
      case GL_LINE_LOOP:
      case GL_LINES:
      case GL_TRIANGLE_STRIP:
      case GL_TRIANGLE_FAN:
      case GL_TRIANGLES:
      case GL_POLYGON:
      case GL_QUADS:
      case GL_QUAD_STRIP:

#ifdef DX_EMU_MATRIX
         tkopengl_matrix_update();
#endif

#ifdef DX_EMU_IMM
         tkopengl_imm_begin(_mode);
#else
#ifdef DX_GLES
         Dstub(glBegin);
#else
         Dcompat(glBegin)
         {
            ::glBegin(_mode);
         }
#endif // DX_GLES
#endif // DX_EMU_IMM

         break;
   }

#endif // DONT_DRAW
}

// ---------------------------------------------------------------------------- glBindTexture
void YAC_CALL _glBindTexture(sSI _target, sUI _id) {

#ifdef DX_EMU_ATTRIB
#ifdef DX_EMU_ATTRIB_TEXTURE_ID_MAP
   sUI texNr = loc_find_texnr_by_id(_id);
#else
   sUI texNr = _id;
#endif // DX_EMU_ATTRIB_TEXTURE_ID_MAP

   if(texNr < TKOPENGL_MAX_TEXTURES)
   {
      Dactivetextureunit(bound_nr) = texNr;
   }
   else
   {
      yac_host->printf("[~~~] tkopengl::glBindTexture: texNr (id=%u) exceeds TKOPENGL_MAX_TEXTURES (%d >= %u)\n", _id, (sSI)texNr, TKOPENGL_MAX_TEXTURES);
   }
#endif // DX_EMU_ATTRIB

#ifdef DX_EMU_TEXTURE_1D
   if(GL_TEXTURE_1D == _target)
   {
      _target = GL_TEXTURE_2D;
   }
#endif // DX_EMU_TEXTURE_1D

#ifdef DX_EMU_TEXTURE_RECTANGLE
   if(GL_TEXTURE_RECTANGLE == _target)
   {
      _target = GL_TEXTURE_2D;
   }
#endif // DX_EMU_TEXTURE_RECTANGLE

   ::glBindTexture(_target, _id);
   Dtraceglerror("glBindTexture");
}

// ---------------------------------------------------------------------------- zglDeleteTexture
void YAC_CALL _zglDeleteTexture(sUI _id) {

#ifdef DX_EMU_ATTRIB
#ifdef DX_EMU_ATTRIB_TEXTURE_ID_MAP
   sUI texNr = loc_find_texnr_by_id(_id);
#else
   sUI texNr = _id;
#endif // DX_EMU_ATTRIB_TEXTURE_ID_MAP

   if(texNr < TKOPENGL_MAX_TEXTURES)
   {
#ifdef DX_EMU_ATTRIB_TEXTURE_ID_MAP
      tkopengl_textures[texNr].id              = (GLuint)-1;
#endif
      tkopengl_textures[texNr].internal_format = (GLenum) 0;
      tkopengl_textures[texNr].sx              = 0;
      tkopengl_textures[texNr].sy              = 0;
      tkopengl_textures[texNr].sz              = 0;
   }
   else
   {
      yac_host->printf("[~~~] tkopengl::zglDeleteTexture: texNr (id=%u) exceeds TKOPENGL_MAX_TEXTURES (%d >= %u)\n", _id, (sSI)texNr, TKOPENGL_MAX_TEXTURES);
   }
#endif // DX_EMU_ATTRIB

   ::glDeleteTextures(1, &_id);
   Dtraceglerror("zglDeleteTexture");
}

// ---------------------------------------------------------------------------- zglGenTexture
sUI YAC_CALL _zglGenTexture(void) {
   sUI texId;
   ::glGenTextures(1, &texId);
   Dtraceglerror("zglGenTexture");

#ifdef DX_EMU_ATTRIB_TEXTURE_ID_MAP
   sUI texNr = 0;

   // Find unused texture slot
   while(texNr < TKOPENGL_MAX_TEXTURES)
   {
      if((GLuint)-1 == tkopengl_textures[texNr].id)
      {
         yac_host->printf("[dbg] tkopengl::zglGenTexture: id=%u texNr=%u\n", texId, texNr);
         tkopengl_textures[texNr].id              = texId;
         tkopengl_textures[texNr].internal_format = (GLenum) 0;
         tkopengl_textures[texNr].sx              = 0;
         tkopengl_textures[texNr].sy              = 0;
         tkopengl_textures[texNr].sz              = 0;
         break;
      }

      texNr++;
   }

   if(texNr >= TKOPENGL_MAX_TEXTURES)
   {
      yac_host->printf("[~~~] tkopengl::zglGenTexture: texNr (id=%u) exceeds TKOPENGL_MAX_TEXTURES (%d >= %u)\n", texId, (sSI)texNr, TKOPENGL_MAX_TEXTURES);
   }
#endif // DX_EMU_ATTRIB_TEXTURE_ID_MAP

   return texId;
}

// ---------------------------------------------------------------------------- glBlendColor
void YAC_CALL _glBlendColor(sF32 _a, sF32 _b, sF32 _c, sF32 _d) {
   Dcheckext(glBlendColor)
   {
      ::glBlendColor_xref(_a, _b, _c, _d);
      Dtraceglerror("glBlendColor");
   }
}

// ---------------------------------------------------------------------------- glBlendEquation
void YAC_CALL _glBlendEquation(sSI _a) {
   Dcheckext(glBlendEquation)
   {
      ::glBlendEquation_xref(_a);
      Dtraceglerror("glBlendEquation");
   }
}

// ---------------------------------------------------------------------------- glBlendFunc
void YAC_CALL _glBlendFunc(sSI _a, sSI _b) {
   ::glBlendFunc(_a, _b);
   Dtraceglerror("glBlendFunc");
}

// ---------------------------------------------------------------------------- glBlendFuncSeparate
void YAC_CALL _glBlendFuncSeparate(sSI _a, sSI _b, sSI _c, sSI _d) {
   Dcheckext(glBlendFuncSeparate)
   {
      ::glBlendFuncSeparate_xref(_a, _b, _c, _d);
      Dtraceglerror("glBlendFuncSeparate");
   }
}

// ---------------------------------------------------------------------------- glCullFace
void YAC_CALL _glCullFace(sSI _a) {
   ::glCullFace(_a);
   Dtraceglerror("glCullFace");
}

// ---------------------------------------------------------------------------- glClearColor
void YAC_CALL _glClearColor(sF32 _r, sF32 _g, sF32 _b, sF32 _a)
{
   ::glClearColor(_r,_g,_b,_a);
   Dtraceglerror("glClearColor");
}

// ---------------------------------------------------------------------------- glClearColor
void YAC_CALL _zglClearColorARGB(sUI _argb) {
	const sF32 r = ((_argb>>16)&255u)*(1.0f/255.0f);
	const sF32 g = ((_argb>>8 )&255u)*(1.0f/255.0f);
	const sF32 b = ((_argb    )&255u)*(1.0f/255.0f);
	const sF32 a = ((_argb>>24)&255u)*(1.0f/255.0f);
   ::glClearColor(r, g, b, a);
   Dtracegldrawerror("zglClearColorARGB");
}

// ---------------------------------------------------------------------------- glClear
void YAC_CALL _glClear(sSI _a) {
   ::glClear(_a);
   Dtraceglerror("glClear");
}

// ---------------------------------------------------------------------------- glClearDepth
void YAC_CALL _glClearDepth(sF32 _a) {
#ifdef DX_GLES
   ::glClearDepthf(_a);
   Dtraceglerror("glClearDepthf");
#else
   ::glClearDepth(_a);
   Dtraceglerror("glClearDepth");
#endif // DX_GLES
}

// ---------------------------------------------------------------------------- glClearStencil
void YAC_CALL _glClearStencil(sSI _a) {
   ::glClearStencil(_a);
   Dtraceglerror("glClearStencil");
}

// ---------------------------------------------------------------------------- glClientActiveTexture
void YAC_CALL _glClientActiveTexture(sSI _a) {
#ifdef DX_EMU_MULTITEX
   // (todo) multitex
   Dstub(glClientActiveTexture);
#else
   Dcheckext(glClientActiveTexture)
   {
      ::glClientActiveTexture_xref(_a);
      Dtraceglerror("glClientActiveTexture");
   }
#endif // DX_EMU_MULTITEX
}

// ---------------------------------------------------------------------------- glColor3f
void YAC_CALL _glColor3f(sF32 _r, sF32 _g, sF32 _b) {
#ifdef DX_EMU_IMM
   tkopengl_imm_color_cbk(_r, _g, _b, 1.0f);
#else
#ifdef DX_GLES
   Dstub(glColor3f);
#else
   Dcompat(glColor3f)
   {
      ::glColor3f(_r, _g, _b);
      Dtracegldrawerror("glColor3f");
   }
#endif // DX_GLES
#endif // DX_EMU_IMM
}

// ---------------------------------------------------------------------------- glColor3ub
void YAC_CALL _glColor3ub(sSI _r, sSI _g, sSI _b) {
#ifdef DX_EMU_IMM
   sF32 r = ((sU8)_r) * (1.0f / 255.0f);
   sF32 g = ((sU8)_g) * (1.0f / 255.0f);
   sF32 b = ((sU8)_b) * (1.0f / 255.0f);
   tkopengl_imm_color_cbk(r, g, b, 1.0f);
#else
#ifdef DX_GLES
   Dstub(glColor3ub);
#else
   Dcompat(glColor3ub)
   {
      ::glColor3ub(_r, _g, _b);
      Dtracegldrawerror("glColor3ub");
   }
#endif // DX_GLES
#endif // DX_EMU_IMM
}

// ---------------------------------------------------------------------------- glColor4f
void YAC_CALL _glColor4f(sF32 _r, sF32 _g, sF32 _b, sF32 _a) {
#ifdef DX_EMU_IMM
   tkopengl_imm_color_cbk(_r, _g, _b, _a);
#else
#ifdef DX_GLES
   Dstub(glColor4f);
#else
   Dcompat(glColor4f)
   {
      ::glColor4f(_r, _g, _b, _a);
      Dtracegldrawerror("glColor4f");
   }
#endif // DX_GLES
#endif // DX_EMU_IMM
}

// ---------------------------------------------------------------------------- glColor4ub
void YAC_CALL _glColor4ub(sSI _r, sSI _g, sSI _b, sSI _a) {
#ifdef DX_EMU_IMM
   sF32 r = ((sU8)_r) * (1.0f / 255.0f);
   sF32 g = ((sU8)_g) * (1.0f / 255.0f);
   sF32 b = ((sU8)_b) * (1.0f / 255.0f);
   sF32 a = ((sU8)_a) * (1.0f / 255.0f);
   tkopengl_imm_color_cbk(r, g, b, a);
#else
#ifdef DX_GLES
   Dstub(glColor4ub);
#else
   Dcompat(glColor4ub)
   {
      ::glColor4ub(_r, _g, _b, _a);
   }
   Dtracegldrawerror("glColor4ub");
#endif // DX_GLES
#endif // DX_EMU_IMM
}

// ---------------------------------------------------------------------------- glColorMask
void YAC_CALL _glColorMask(sSI _a, sSI _b, sSI _c, sSI _d) {
   ::glColorMask(_a, _b, _c, _d);
   Dtraceglerror("glColorMask");
}

// ---------------------------------------------------------------------------- glCopyPixels
void YAC_CALL _glCopyPixels(sSI _a, sSI _b,
                            sSI _c, sSI _d,
                            sSI _e)
{
#ifdef DX_GLES
   // (todo) gles glCopyPixels
   Dstub(glCopyPixels);
#else
   ::glCopyPixels(_a, _b, _c, _d, _e);
   Dtraceglerror("glCopyPixels");
#endif // DX_GLES
}

// ---------------------------------------------------------------------------- glCopyTexImage2D
void YAC_CALL _glCopyTexImage2D(sSI _a, sSI _b,
                                sSI _c, sSI _d,
                                sSI _e, sSI _f,
                                sSI _g, sSI _h)
{
   ::glCopyTexImage2D(_a, _b, _c, _d, _e, _f, _g, _h);
   Dtraceglerror("glCopyTexImage2D");
}

// ---------------------------------------------------------------------------- glCopyTexSubImage2D
void YAC_CALL _glCopyTexSubImage2D(sSI _a, sSI _b,
                                   sSI _c, sSI _d,
                                   sSI _e, sSI _f,
                                   sSI _g, sSI _h)
{
   ::glCopyTexSubImage2D(_a, _b, _c, _d, _e, _f, _g, _h);
   Dtraceglerror("glCopyTexSubImage2D");
}

// ---------------------------------------------------------------------------- glCopyTexSubImage3D
void YAC_CALL _glCopyTexSubImage3D(sSI _a, sSI _b, sSI _c, sSI _d, sSI _e, sSI _f, sSI _g, sSI _h, sSI _i)
{
   Dcheckext(glCopyTexSubImage3D)
   {
      ::glCopyTexSubImage3D_xref(_a, _b, _c, _d, _e, _f, _g, _h, _i);
      Dtraceglerror("glCopyTexSubImage3D");
   }
}

// ---------------------------------------------------------------------------- glDepthFunc
void YAC_CALL _glDepthFunc(sSI _a) {
   ::glDepthFunc(_a);
   Dtraceglerror("glDepthFunc");
}

// ---------------------------------------------------------------------------- glDepthMask
void YAC_CALL _glDepthMask(sSI _a) {
   ::glDepthMask(_a);
   Dtraceglerror("glDepthMask");
}

// ---------------------------------------------------------------------------- glDisable
void YAC_CALL _glDisable(sSI _a) {
   Dtrace_call("xxx glDisable(a=0x%04x)\n", _a);
#ifdef DX_EMU_FIXFXN
   switch(_a)
   {
      default:
         yac_host->printf("[~~~] glDisable: unhandled enum %d\n", _a);
         break;

      case GL_TEXTURE_1D:
      case GL_TEXTURE_2D:
      case GL_TEXTURE_3D:
      case GL_TEXTURE_CUBE_MAP:
      case GL_TEXTURE_RECTANGLE:
         if(Dactivetextureunit(mode) == (GLenum)_a)
         {
            Dactivetextureunit(mode) = 0;
         }
         break;

      case GL_BLEND:
      case GL_CULL_FACE:
      case GL_DEPTH_TEST:
      case GL_DITHER:
      case GL_POLYGON_OFFSET_FILL:
      case GL_SAMPLE_ALPHA_TO_COVERAGE:
      case GL_SAMPLE_COVERAGE:
      case GL_SCISSOR_TEST:
      case GL_STENCIL_TEST:
         ::glDisable(_a);
         Dtraceglerror("glDisable");
         break;

      case GL_LINE_STIPPLE:
      case GL_POLYGON_STIPPLE:
      case GL_TEXTURE_GEN_Q:
      case GL_TEXTURE_GEN_S:
      case GL_TEXTURE_GEN_T:
      case GL_TEXTURE_GEN_R:
      case GL_COLOR_LOGIC_OP:
      case GL_INDEX_LOGIC_OP:  // 0x0bf1
      case GL_NORMALIZE:
      case GL_POINT_SMOOTH:
      case GL_LINE_SMOOTH:
      case GL_POLYGON_SMOOTH:
#if !defined(DX_GLES) && !defined(DX_GL4_CORE)
         // (todo) fixfxn
         ::glDisable(_a);
         Dtraceglerror("glDisable");
#endif // DX_GLES
         break;

#ifdef DX_EMU_FOG
      case GL_FOG:
         tkopengl_attrib->fog.b_enable = YAC_FALSE;
         break;
#endif // DX_EMU_FOG

      case GL_ALPHA_TEST:
         tkopengl_attrib->alphatest.b_enable = YAC_FALSE;
         break;

#ifdef DX_EMU_LIGHT
      case GL_LIGHTING:
         tkopengl_attrib->light.b_enable = YAC_FALSE;
         break;

      case GL_LIGHT0:
         tkopengl_attrib->light.enable_mask &= ~(1 << 0);
         break;

      case GL_LIGHT1:
         tkopengl_attrib->light.enable_mask &= ~(1 << 1);
         break;

      case GL_LIGHT2:
         tkopengl_attrib->light.enable_mask &= ~(1 << 2);
         break;

      case GL_LIGHT3:
         tkopengl_attrib->light.enable_mask &= ~(1 << 3);
         break;

      case GL_LIGHT4:
         tkopengl_attrib->light.enable_mask &= ~(1 << 4);
         break;

      case GL_LIGHT5:
         tkopengl_attrib->light.enable_mask &= ~(1 << 5);
         break;

      case GL_LIGHT6:
         tkopengl_attrib->light.enable_mask &= ~(1 << 6);
         break;

      case GL_LIGHT7:
         tkopengl_attrib->light.enable_mask &= ~(1 << 7);
         break;

      case GL_COLOR_MATERIAL:
         tkopengl_attrib->light.b_color_material = YAC_FALSE;
         break;
#endif // DX_EMU_LIGHT

      case GL_POINT_SPRITE:
         tkopengl_attrib->point.sprite.b_enable = YAC_FALSE;
#ifndef DX_GLES
         ::glDisable(_a); // to turn off gl_PointSize in VP
         Dtraceglerror("glDisable");
#endif // DX_GLES
         break;
   }

#else
#ifdef DX_GLES
   switch(_a)
   {
      default:
         ::glDisable(_a);
         break;

      case GL_TEXTURE_1D:
      case GL_TEXTURE_2D:
      case GL_TEXTURE_3D:
      case GL_TEXTURE_CUBE_MAP:
      case GL_TEXTURE_RECTANGLE:
      case GL_LINE_STIPPLE:
      case GL_POLYGON_STIPPLE:
      case GL_TEXTURE_GEN_Q:
      case GL_TEXTURE_GEN_S:
      case GL_TEXTURE_GEN_T:
      case GL_TEXTURE_GEN_R:
      case GL_POINT_SMOOTH:
      case GL_LINE_SMOOTH:
      case GL_POLYGON_SMOOTH:
      case GL_FOG:
      case GL_ALPHA_TEST:
      case GL_LIGHTING:
      case GL_LIGHT0:
      case GL_LIGHT1:
      case GL_LIGHT2:
      case GL_LIGHT3:
      case GL_LIGHT4:
      case GL_LIGHT5:
      case GL_LIGHT6:
      case GL_LIGHT7:
      case GL_COLOR_MATERIAL:
      case GL_POINT_SPRITE:
         // n/a in GLES, silently discard
         break;
   }
#else
   ::glDisable(_a);
   Dtraceglerror("glDisable");
#endif // DX_GLES
#endif // DX_EMU_FIXFXN
}

// ---------------------------------------------------------------------------- glDisableClientState
void YAC_CALL _glDisableClientState(sSI _array) {
#ifdef DX_EMU_FIXFXN
   switch(_array)
   {
   default:
   case GL_EDGE_FLAG_ARRAY:
   case GL_INDEX_ARRAY:
      yac_host->printf("[---] glDisableClientState: unsupported array type 0x%04x\n", (sU16)_array);
      last_gl_error = GL_INVALID_ENUM;
      break;

   case GL_COLOR_ARRAY:
      tkopengl_attrib->userarray[TKOPENGL_USERARRAY_COLOR].b_enable = YAC_FALSE;
      break;

   case GL_NORMAL_ARRAY:
      tkopengl_attrib->userarray[TKOPENGL_USERARRAY_NORMAL].b_enable = YAC_FALSE;
      break;

   case GL_TEXTURE_COORD_ARRAY:
      tkopengl_attrib->userarray[TKOPENGL_USERARRAY_TEXCOORD].b_enable = YAC_FALSE;
      break;

   case GL_VERTEX_ARRAY:
      tkopengl_attrib->userarray[TKOPENGL_USERARRAY_VERTEX].b_enable = YAC_FALSE;
      break;
   }
#else
#ifdef DX_GLES
   Dstub(glDisableClientState);
#else
   ::glDisableClientState(_array);
   Dtraceglerror("glDisableClientState");
#endif // DX_GLES
#endif // DX_EMU_FIXFXN
}

// ---------------------------------------------------------------------------- loc_remap_polymode
#ifdef DX_EMU_PRIM
static sSI loc_remap_polymode(sSI _mode) {
   switch(tkopengl_attrib->polygon_mode)
   {
      default:
      case GL_FILL:
         return _mode;

      case GL_POINT:
         return GL_POINTS;

      case GL_LINE:
         return GL_LINE_LOOP;
   }
   return _mode;
}
#endif // DX_EMU_PRIM

// ---------------------------------------------------------------------------- glDrawArrays
void YAC_CALL _glDrawArrays(sSI _mode, sSI _first, sSI _count) {
  _glDrawArrays_priv(_mode, _first, _count, YAC_FALSE/*bAllowImm*/);
}

// ---------------------------------------------------------------------------- loc_glDrawArrays
static void loc_glDrawArrays(sSI _mode, sSI _first, sSI _count, sBool _bAllowImm) {

   // Dyac_host_printf("xxx drawarrays: mode=0x%04x first=%d count=%d bAllowImm=%d\n", (sU16)_mode, _first, _count, _bAllowImm);

#ifdef DX_EMU_MATRIX
   tkopengl_matrix_update();
#endif

#ifdef DX_EMU_FIXFXN
   // Select/bind shader and vertex attrib arrays
   tkopengl_shader_draw_begin(_mode,
                              _bAllowImm,
                              0u/*copyNumIndices*/,
                              sUI(_first)/*copyFirstVertex*/,
                              sUI(_count)/*copyNumVertices*/
                              );
#else
   (void)_bAllowImm;
#endif

   ::glDrawArrays(_mode, _first, _count);
   Dtraceglerror("glDrawArrays");

#ifdef DX_EMU_FIXFXN
   // Unbind shader and vertex attrib arrays
   tkopengl_shader_draw_end();
#endif
}

// ---------------------------------------------------------------------------- loc_glDrawElements
static void loc_glDrawElements(GLenum _mode, GLsizei _count, GLenum _type, const GLvoid *_indices, sBool _bAllowImm) {

   // Dyac_host_printf("xxx loc_glDrawElements: mode=0x%04x count=%d type=0x%04x indices=%p bAllowImm=%d\n", _mode, _count, _type, _indices, _bAllowImm);

#ifdef DX_EMU_MATRIX
   tkopengl_matrix_update();
#endif

#ifdef DX_EMU_FIXFXN
   // Select/bind shader and vertex attrib arrays
   tkopengl_shader_draw_begin(_mode, _bAllowImm,
                              0u/*copyNumIndices*/,
                              0u/*copyFirstVertex*/,
                              0u/*copyNumVertices*/
                              );
#else
   (void)_bAllowImm;
#endif

   ::glDrawElements(_mode, _count, _type, _indices);
   Dtraceglerror("glDrawElements");

   // {
   //    static sUI ind[6] = { 0,1,2,3,4,5 };
   //    //::glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, ind);
   //    ::glDrawArrays(GL_TRIANGLES, 0, 3);
   // }

#ifdef DX_EMU_FIXFXN
   // Unbind shader and vertex attrib arrays
   tkopengl_shader_draw_end();
#endif
}

// ---------------------------------------------------------------------------- glDrawArrays
void YAC_CALL _glDrawArrays_priv(sSI _mode, sSI _first, sSI _count, sBool _bAllowImm) {

#ifndef DONT_DRAW

#ifdef DX_EMU_PRIM
   Demuprintfv("[trc] glDrawArrays_priv mode=0x%04x first=%d count=%d bAllowImm=%d\n", _mode, _first, _count, _bAllowImm);

   if(GL_QUADS == _mode)
   {
      if( sUI(_first) < TKOPENGL_IMM_ARRAY_SIZE )
      {
         if( sUI(_count) <= TKOPENGL_IMM_ARRAY_SIZE )
         {
            if( (sUI(_first) + sUI(_count)) <= TKOPENGL_IMM_ARRAY_SIZE )
            {
               _mode = loc_remap_polymode(GL_TRIANGLES);

               switch(_mode)
               {
                  default:
                  case GL_POINTS:
                     loc_glDrawArrays(GL_POINTS, _first, _count, _bAllowImm);
                     break;

                  case GL_TRIANGLES:
                     _first = sSI( (sUI(_first) >> 2) * 6u );
                     loc_glDrawElements(GL_TRIANGLES,
                                        (GLsizei) ( (sUI(_count) >> 2) * 6u ),
                                        GL_UNSIGNED_SHORT,
                                        (void*)(tkopengl_imm_quads_to_triangles_array + _first),
                                        _bAllowImm
                                        );
                     break;

                  case GL_LINE_LOOP:
#ifdef DX_EMU_PRIM_QUAD_LINE
                     _first = sSI( sUI(_first) << 1 );
                     loc_glDrawElements(GL_LINES,
                                        (GLsizei) (sUI(_count) << 1),
                                        GL_UNSIGNED_SHORT,
                                        tkopengl_imm_quads_to_lines_array + _first,
                                        _bAllowImm
                                        );
#else
                     yac_host->printf("[---] tkopengl::drawArrays: polygon mode GL_LINE for GL_QUADS is not compiled in.\n");
#endif // DX_EMU_PRIM_QUAD_LINE
                     break;
               }
            }
            else
            {
               last_gl_error = GL_INVALID_VALUE;
               yac_host->printf("[---] glDrawArrays<GL_QUADS>: (_first + _count) exceeds may array size (%u > %u)\n",
                                (sUI(_first) + sUI(_count)),
                                TKOPENGL_IMM_ARRAY_SIZE
                                );
            }
         }
         else
         {
            last_gl_error = GL_INVALID_VALUE;
            yac_host->printf("[---] glDrawArrays<GL_QUADS>: _count exceeds may array size (%u > %u)\n",
                             (sUI)_count,
                             TKOPENGL_IMM_ARRAY_SIZE
                             );
         }
      }
      else
      {
         last_gl_error = GL_INVALID_VALUE;
         yac_host->printf("[---] glDrawArrays<GL_QUADS>: _first exceeds may array size (%u > %u)\n",
                          (sUI)_first,
                          TKOPENGL_IMM_ARRAY_SIZE
                          );
      }
   }
   else if(GL_POLYGON == _mode)
   {
      _mode = loc_remap_polymode(GL_TRIANGLE_FAN);
      loc_glDrawArrays(_mode, _first, _count, _bAllowImm);
   }
   else if(GL_TRIANGLES == _mode)
   {
      _mode = loc_remap_polymode(GL_TRIANGLES);

      if(GL_LINE_LOOP == _mode)
      {
#ifdef DX_EMU_PRIM_TRI_LINE
         if( sUI(_first) < TKOPENGL_IMM_ARRAY_SIZE )
         {
            if( sUI(_count) <= TKOPENGL_IMM_ARRAY_SIZE )
            {
               if( (sUI(_first) + sUI(_count)) <= TKOPENGL_IMM_ARRAY_SIZE )
               {
                  _first = sSI( sUI(_first) << 1 );
                  loc_glDrawElements(GL_LINES,
                                     (GLsizei) (sUI(_count) << 1),
                                     GL_UNSIGNED_SHORT,
                                     (void*)(tkopengl_imm_triangles_to_lines_array + _first),
                                     _bAllowImm
                                     );
               }
               else
               {
                  last_gl_error = GL_INVALID_VALUE;
                  yac_host->printf("[---] glDrawArrays<GL_TRIANGLES>: (_first + _count) exceeds may array size (%u > %u)\n",
                                   (sUI(_first) + sUI(_count)),
                                   TKOPENGL_IMM_ARRAY_SIZE
                                   );
               }
            }
            else
            {
               last_gl_error = GL_INVALID_VALUE;
               yac_host->printf("[---] glDrawArrays<GL_TRIANGLES>: _count exceeds may array size (%u > %u)\n",
                                (sUI)_count,
                                TKOPENGL_IMM_ARRAY_SIZE
                                );
            }
         }
         else
         {
            last_gl_error = GL_INVALID_VALUE;
            yac_host->printf("[---] glDrawArrays<GL_TRIANGLES>: _first exceeds may array size (%u > %u)\n",
                             (sUI)_first,
                             TKOPENGL_IMM_ARRAY_SIZE
                             );
         }
#else
         yac_host->printf("[---] tkopengl::drawArrays: polygon mode GL_LINE for GL_TRIANGLES is not compiled in.\n");
#endif // DX_EMU_PRIM_TRI_LINE
      }
      else
      {
         if(_count >= 3)
         {
            loc_glDrawArrays(_mode, _first, _count, _bAllowImm);
         }
      }
   }
   else if(GL_QUAD_STRIP == _mode)
   {
      _mode = loc_remap_polymode(GL_TRIANGLE_STRIP);

      if(GL_LINE_LOOP == _mode)
      {
#ifdef DX_EMU_PRIM_QUADSTRIP_LINE
         if( sUI(_first) < TKOPENGL_IMM_ARRAY_SIZE )
         {
            if( sUI(_count) <= TKOPENGL_IMM_ARRAY_SIZE )
            {
               if( (sUI(_first) + sUI(_count)) <= TKOPENGL_IMM_ARRAY_SIZE )
               {
                  if(sUI(_count) >= 4u)
                  {
                     _first = sSI( (sUI(_first) >> 1) << 3 );
                     loc_glDrawElements(GL_LINES,
                                        (GLsizei) ( (sUI(_count) - 2u) << 2 ),
                                        GL_UNSIGNED_SHORT,
                                        (void*)(tkopengl_imm_quadstrip_to_lines_array + _first),
                                        _bAllowImm
                                        );
                  }
                  else
                  {
                     // nothing to render
                  }
               }
               else
               {
                  last_gl_error = GL_INVALID_VALUE;
                  yac_host->printf("[---] glDrawArrays<QUAD_STRIP>: (_first + _count) exceeds may array size (%u > %u)\n",
                                   (sUI(_first) + sUI(_count)),
                                   TKOPENGL_IMM_ARRAY_SIZE
                                   );
               }
            }
            else
            {
               last_gl_error = GL_INVALID_VALUE;
               yac_host->printf("[---] glDrawArrays<QUAD_STRIP>: _count exceeds may array size (%u > %u)\n",
                                (sUI)_count,
                                TKOPENGL_IMM_ARRAY_SIZE
                                );
            }
         }
         else
         {
            last_gl_error = GL_INVALID_VALUE;
            yac_host->printf("[---] glDrawArrays<QUAD_STRIP>: _first exceeds may array size (%u > %u)\n",
                             (sUI)_first,
                             TKOPENGL_IMM_ARRAY_SIZE
                             );
         }
#else
         yac_host->printf("[---] tkopengl::drawArrays: polygon mode GL_LINE for GL_QUAD_STRIP is not compiled in.\n");
#endif // DX_EMU_PRIM_QUADSTRIP_LINE
      }
      else
      {
         if(_count >= 4)
         {
            loc_glDrawArrays(_mode, _first, _count, _bAllowImm);
         }
      }
   }
   else if(GL_TRIANGLE_STRIP == _mode)
   {
      _mode = loc_remap_polymode(_mode);

      if(GL_LINE_LOOP == _mode)
      {
#ifdef DX_EMU_PRIM_TRISTRIP_LINE
         if( sUI(_first) < TKOPENGL_IMM_ARRAY_SIZE )
         {
            if( sUI(_count) <= TKOPENGL_IMM_ARRAY_SIZE )
            {
               if( (sUI(_first) + sUI(_count)) <= TKOPENGL_IMM_ARRAY_SIZE )
               {
                  if(sUI(_count) >= 3u)
                  {
                     _first = sSI( sUI(_first) * 6u );
                     loc_glDrawElements(GL_LINES,
                                        (GLsizei) ( (sUI(_count) - 2u) * 6u ),
                                        GL_UNSIGNED_SHORT,
                                        (void*)(tkopengl_imm_trianglestrip_to_lines_array + _first),
                                        _bAllowImm
                                        );
                  }
                  else
                  {
                     // nothing to render
                  }
               }
               else
               {
                  last_gl_error = GL_INVALID_VALUE;
                  yac_host->printf("[---] glDrawArrays<TRIANGLE_STRIP>: (_first + _count) exceeds may array size (%u > %u)\n",
                                   (sUI(_first) + sUI(_count)),
                                   TKOPENGL_IMM_ARRAY_SIZE
                                   );
               }
            }
            else
            {
               last_gl_error = GL_INVALID_VALUE;
               yac_host->printf("[---] glDrawArrays<TRIANGLE_STRIP>: _count exceeds may array size (%u > %u)\n",
                                (sUI)_count,
                                TKOPENGL_IMM_ARRAY_SIZE
                                );
            }
         }
         else
         {
            last_gl_error = GL_INVALID_VALUE;
            yac_host->printf("[---] glDrawArrays<TRIANGLE_STRIP>: _first exceeds may array size (%u > %u)\n",
                             (sUI)_first,
                             TKOPENGL_IMM_ARRAY_SIZE
                             );
         }
#else
         yac_host->printf("[---] tkopengl::drawArrays: polygon mode GL_LINE for GL_TRIANGLE_STRIP is not compiled in.\n");
#endif // DX_EMU_PRIM_TRISTRIP_LINE
      }
      else
      {
         if(_count >= 3)
         {
            loc_glDrawArrays(_mode, _first, _count, _bAllowImm);
         }
      }
   }
   else
   {
      // GL_POINTS, GL_LINES, GL_LINE_LOOP, GL_LINE_STRIP
      // // printf("xxx glDrawArrays mode=0x%04x\n", _mode);
      loc_glDrawArrays(_mode, _first, _count, _bAllowImm);
   }
#else
   loc_glDrawArrays(_mode, _first, _count, _bAllowImm);
#endif // DX_EMU_PRIM

#endif // DONT_DRAW
}

// ---------------------------------------------------------------------------- glDrawArraysInstanced
void YAC_CALL _glDrawArraysInstanced(sSI _mode, sSI _first, sSI _count, sSI _instancecount) {
   // GL3.1+
   Dcheckext(glDrawArraysInstanced)
   {
      glDrawArraysInstanced_xref(_mode, _first, _count, _instancecount);
   }
}

// ---------------------------------------------------------------------------- glDrawElements
void YAC_CALL _glDrawElements(sSI _mode, YAC_Object *_intarray) {
	if(NULL != _intarray)
	{
		if(YAC_TYPE_INT == _intarray->yacArrayGetElementType())
      {
         if(sizeof(sUI) == _intarray->yacArrayGetElementByteSize())
         {
            sUI num = _intarray->yacArrayGetNumElements();

            _glDrawElements_int(_mode, num, GL_UNSIGNED_INT, _intarray->yacArrayGetPointer());
         }
      }
	}
}

// ---------------------------------------------------------------------------- zglDrawElementsOffset
void YAC_CALL _zglDrawElementsOffset(sSI _mode, sUI _count, sSI _type, sUI _offset) {
   _glDrawElements_int(_mode, (GLsizei)_count, (GLenum)_type, (const GLvoid*)size_t(_offset));
}

// ---------------------------------------------------------------------------- glDrawElements_int
void YAC_CALL _glDrawElements_int(GLenum _mode, GLsizei _count, GLenum _type, const GLvoid *_indices) {
   _glDrawElements_priv(_mode, _count, _type, _indices, YAC_FALSE/*bAllowImm*/);
}

// ---------------------------------------------------------------------------- glDrawElements_int
void YAC_CALL _glDrawElements_priv(GLenum _mode, GLsizei _count, GLenum _type, const GLvoid *_indices, sBool _bAllowImm) {
#ifndef DONT_DRAW

   // // Dyac_host_printf("xxx glDrawElements_priv\n");

#ifdef DX_EMU_PRIM
   switch(_mode)
   {
      default:
      case GL_POINTS:
      case GL_LINES:
      case GL_LINE_LOOP:
      case GL_LINE_STRIP:
         loc_glDrawElements(_mode, _count, _type, _indices, _bAllowImm);
         break;

      case GL_TRIANGLES:
         _mode = loc_remap_polymode(GL_TRIANGLES);

         switch(_mode)
         {
            default:
            case GL_POINTS:
               loc_glDrawElements(GL_POINTS, _count, _type, _indices, _bAllowImm);
               break;

            case GL_TRIANGLES:
               loc_glDrawElements(GL_TRIANGLES, _count, _type, _indices, _bAllowImm);
               break;

            case GL_LINE_LOOP:
#ifdef DX_EMU_PRIM_TRI_LINE
               if(((sUI)_count) <= TKOPENGL_IMM_ARRAY_SIZE)
               {
                  if(GL_UNSIGNED_INT == _type)
                  {
                     // Convert triangle indices to line indices
                     sU16 *d = tkopengl_tmp_index_array;
                     sUI *s = (sUI*)_indices;
                     sUI i;

                     for(i=0; i<((sUI)_count); i+=3)
                     {
                        *d++ = s[i + 0];
                        *d++ = s[i + 1];
                        *d++ = s[i + 1];
                        *d++ = s[i + 2];
                        *d++ = s[i + 2];
                        *d++ = s[i + 0];
                     }

                     loc_glDrawElements(GL_LINES,
                                        (GLsizei) (((sUI)_count) << 1),
                                        GL_UNSIGNED_SHORT,
                                        tkopengl_tmp_index_array,
                                        _bAllowImm
                                        );
                  }
                  else
                  {
                     last_gl_error = GL_INVALID_ENUM;
                     yac_host->printf("[---] tkopengl::drawElements<TRIANGLES::LINE>: unsupported array type 0x%04x\n", (sU16)_type);
                  }
               }
               else
               {
                  last_gl_error = GL_INVALID_VALUE;
                  yac_host->printf("[---] glDrawElements<TRIANGLES::LINE>: _count exceeds may array size (%u > %u)\n",
                                   (sUI)_count,
                                   TKOPENGL_IMM_ARRAY_SIZE
                                   );
               }
#else
               yac_host->printf("[---] tkopengl::drawElements: polygon mode GL_LINE for GL_TRIANGLES is not compiled in.\n");
#endif // DX_EMU_PRIM_TRI_LINE
               break;
         }
         break;

      case GL_QUADS:
         _mode = loc_remap_polymode(GL_TRIANGLES);

         switch(_mode)
         {
            default:
            case GL_POINTS:
               loc_glDrawElements(GL_POINTS, _count, _type, _indices, _bAllowImm);
               break;

            case GL_TRIANGLES:
               if(((sUI)_count) <= TKOPENGL_IMM_ARRAY_SIZE)
               {
                  if(GL_UNSIGNED_INT == _type)
                  {
                     // Convert quad indices to triangle indices
                     sU16 *d = tkopengl_tmp_index_array;
                     sUI *s = (sUI*)_indices;
                     sUI i;

                     for(i=0; i<((sUI)_count); i+=4)
                     {
                        *d++ = s[i + 0];
                        *d++ = s[i + 1];
                        *d++ = s[i + 2];
                        *d++ = s[i + 0];
                        *d++ = s[i + 2];
                        *d++ = s[i + 3];
                     }

                     loc_glDrawElements(GL_TRIANGLES,
                                        (GLsizei) (((((sUI)_count) >> 2) * 6)),
                                        GL_UNSIGNED_SHORT,
                                        tkopengl_tmp_index_array,
                                        _bAllowImm
                                        );
                  }
                  else
                  {
                     last_gl_error = GL_INVALID_ENUM;
                     yac_host->printf("[---] tkopengl::drawElements<QUADS>: unsupported array type 0x%04x\n", (sU16)_type);
                  }
               }
               else
               {
                  last_gl_error = GL_INVALID_VALUE;
                  yac_host->printf("[---] glDrawElements<QUADS>: _count exceeds may array size (%u > %u)\n",
                                   (sUI)_count,
                                   TKOPENGL_IMM_ARRAY_SIZE
                                   );
               }
               break;

            case GL_LINE_LOOP:
#ifdef DX_EMU_PRIM_QUAD_LINE
               if(((sUI)_count) <= TKOPENGL_IMM_ARRAY_SIZE)
               {
                  if(GL_UNSIGNED_INT == _type)
                  {
                     // Convert quad indices to line indices
                     sU16 *d = tkopengl_tmp_index_array;
                     sUI *s = (sUI*)_indices;
                     sUI i;

                     for(i=0; i<((sUI)_count); i+=4)
                     {
                        *d++ = s[i + 0];
                        *d++ = s[i + 1];
                        *d++ = s[i + 1];
                        *d++ = s[i + 2];
                        *d++ = s[i + 2];
                        *d++ = s[i + 3];
                        *d++ = s[i + 3];
                        *d++ = s[i + 0];
                     }

                     loc_glDrawElements(GL_LINES,
                                        (GLsizei) (((sUI)_count) << 1),
                                        GL_UNSIGNED_SHORT,
                                        tkopengl_tmp_index_array,
                                        _bAllowImm
                                        );
                  }
                  else
                  {
                     last_gl_error = GL_INVALID_ENUM;
                     yac_host->printf("[---] tkopengl::drawElements<QUADS::LINE>: unsupported array type 0x%04x\n", (sU16)_type);
                  }
               }
               else
               {
                  last_gl_error = GL_INVALID_VALUE;
                  yac_host->printf("[---] glDrawElements<QUADS::LINE>: _count exceeds may array size (%u > %u)\n",
                                   (sUI)_count,
                                   TKOPENGL_IMM_ARRAY_SIZE
                                   );
               }
#else
               yac_host->printf("[---] tkopengl::drawElements: polygon mode GL_LINE for GL_QUADS is not compiled in.\n");
#endif // DX_EMU_PRIM_QUAD_LINE
               break;
         }
         break;

      case GL_QUAD_STRIP:
         _mode = loc_remap_polymode(GL_TRIANGLE_STRIP);

         switch(_mode)
         {
            default:
            case GL_POINTS:
               loc_glDrawElements(GL_POINTS, _count, _type, _indices, _bAllowImm);
               break;

            case GL_TRIANGLE_STRIP:
               loc_glDrawElements(GL_TRIANGLE_STRIP, _count, _type, _indices, _bAllowImm);
               break;

            case GL_LINE_LOOP:
#ifdef DX_EMU_PRIM_QUADSTRIP_LINE
               if(((sUI)_count) <= TKOPENGL_IMM_ARRAY_SIZE)
               {
                  if(GL_UNSIGNED_INT == _type)
                  {
                     // Convert quadstrip indices to line indices
                     sU16 *d = tkopengl_tmp_index_array;
                     sUI *s = (sUI*)_indices;
                     sUI i;

                     for(i=0; i<=(((sUI)_count)-4); i+=2)
                     {
                        *d++ = s[i + 0];
                        *d++ = s[i + 1];

                        *d++ = s[i + 1];
                        *d++ = s[i + 3];

                        *d++ = s[i + 3];
                        *d++ = s[i + 2];

                        *d++ = s[i + 2];
                        *d++ = s[i + 0];
                     }

                     loc_glDrawElements(GL_LINES,
                                        (GLsizei) ((((sUI)_count)-2) << 2),
                                        GL_UNSIGNED_SHORT,
                                        tkopengl_tmp_index_array,
                                        _bAllowImm
                                        );
                  }
                  else
                  {
                     last_gl_error = GL_INVALID_ENUM;
                     yac_host->printf("[---] tkopengl::drawElements<QUAD_STRIP::LINE>: unsupported array type 0x%04x\n", (sU16)_type);
                  }
               }
               else
               {
                  last_gl_error = GL_INVALID_VALUE;
                  yac_host->printf("[---] glDrawElements<QUAD_STRIP::LINE>: _count exceeds may array size (%u > %u)\n",
                                   (sUI)_count,
                                   TKOPENGL_IMM_ARRAY_SIZE
                                   );
               }
#else
               yac_host->printf("[---] tkopengl::drawElements: polygon mode GL_LINE for GL_QUAD_STRIP is not compiled in.\n");
#endif // DX_EMU_PRIM_QUADSTRIP_LINE
               break;
         }
         break;

      case GL_TRIANGLE_STRIP:
         _mode = loc_remap_polymode(GL_TRIANGLE_STRIP);
         ////printf("xxx draw GL_TRIANGLE_STRIP: remap mode=0x%04x\n", _mode);
         switch(_mode)
         {
            default:
            case GL_POINTS:
               loc_glDrawElements(GL_POINTS, _count, _type, _indices, _bAllowImm);
               break;

            case GL_TRIANGLE_STRIP:
               loc_glDrawElements(GL_TRIANGLE_STRIP, _count, _type, _indices, _bAllowImm);
               break;

            case GL_LINE_LOOP:
#ifdef DX_EMU_PRIM_TRISTRIP_LINE
               if(((sUI)_count) <= TKOPENGL_IMM_ARRAY_SIZE)
               {
                  if(GL_UNSIGNED_INT == _type)
                  {
                     // Convert trianglestrip indices to line indices
                     sU16 *d = tkopengl_tmp_index_array;
                     sUI *s = (sUI*)_indices;
                     sUI i;

                     for(i=0; i<(((sUI)_count)-2); i++)
                     {
                        *d++ = s[i + 0];
                        *d++ = s[i + 1];

                        *d++ = s[i + 1];
                        *d++ = s[i + 2];

                        *d++ = s[i + 2];
                        *d++ = s[i + 0];
                     }

                     loc_glDrawElements(GL_LINES,
                                        (GLsizei) ((((sUI)_count)-2) * 6),
                                        GL_UNSIGNED_SHORT,
                                        tkopengl_tmp_index_array,
                                        _bAllowImm
                                        );
                  }
                  else
                  {
                     last_gl_error = GL_INVALID_ENUM;
                     yac_host->printf("[---] tkopengl::drawElements<TRIANGLE_STRIP::LINE>: unsupported array type 0x%04x\n", (sU16)_type);
                  }
               }
               else
               {
                  last_gl_error = GL_INVALID_VALUE;
                  yac_host->printf("[---] glDrawElements<TRIANGLE_STRIP::LINE>: _count exceeds may array size (%u > %u)\n",
                                   (sUI)_count,
                                   TKOPENGL_IMM_ARRAY_SIZE
                                   );
               }
#else
               yac_host->printf("[---] tkopengl::drawElements: polygon mode GL_LINE for GL_QUAD_STRIP is not compiled in.\n");
#endif // DX_EMU_PRIM_TRISTRIP_LINE
               break;
         }
         break;

      case GL_TRIANGLE_FAN:
         _mode = loc_remap_polymode(GL_TRIANGLE_STRIP);
         loc_glDrawElements(_mode, _count, _type, _indices, _bAllowImm);
         break;

   } // switch(mode)

#else
   loc_glDrawElements(_mode, _count, _type, _indices, _bAllowImm);
#endif // DX_EMU_PRIM

#endif // DONT_DRAW
}

// ---------------------------------------------------------------------------- glDrawBuffer
void YAC_CALL _glDrawBuffer(sSI _mode) {
#ifdef DX_GLES
   // (todo) gles (GL_FRONT, ..)
   Dstub(glDrawBuffer);
#else
   Dcompat(glDrawBuffer)
   {
      ::glDrawBuffer(_mode);
      Dtraceglerror("glDrawBuffer");
   }
#endif // DX_GLES
}

// ---------------------------------------------------------------------------- glDrawBuffers
void YAC_CALL _glDrawBuffers(sUI _n, YAC_Object *_ia) {
#ifdef DX_GLES
   // (todo) gles (GL_FRONT, ..)
   Dstub(glDrawBuffers);
#else
   Dcheckext(glDrawBuffers)
   {
      const GLenum *a = NULL;
      if(YAC_VALID(_ia))
      {
         sUI numAvail = (_ia->yacArrayGetElementByteSize() * _ia->yacArrayGetMaxElements()) / sizeof(GLenum);
         if(numAvail >= (sUI)_n)
         {
            a = (const GLenum*) _ia->yacArrayGetPointer();
         }
      }
      if( ((_n > 0) && (NULL != a)) )
      {
         glDrawBuffers_xref(_n, a);
         Dtraceglerror("glDrawBuffers");
      }
   }
#endif // DX_GLES
}

// ---------------------------------------------------------------------------- glDrawPixels_int
void YAC_CALL _glDrawPixels_int(GLsizei _width, GLsizei _height, GLenum _format, GLenum _type, const GLvoid *_pixels) {
#ifdef DX_GLES
   // (todo) gles
   Dstub(glDrawPixels);
#else
   ::glDrawPixels(_width, _height, _format, _type, _pixels);
   Dtraceglerror("glDrawPixels");
#endif // DX_GLES
}

// ---------------------------------------------------------------------------- glEdgeFlag
void YAC_CALL _glEdgeFlag(sSI _a) {
#ifdef DX_GLES
   // (todo) gles glEdgeFlag (ignore?)
   Dstub(glEdgeFlag);
#else
   ::glEdgeFlag(_a);
   Dtraceglerror("glEdgeFlag");
#endif // DX_GLES
}

// ---------------------------------------------------------------------------- glEnable
void YAC_CALL _glEnable(sSI _a) {
   Dtrace_call("xxx glEnable(a=0x%04x)\n", _a);
#ifdef DX_EMU_FIXFXN
   switch(_a)
   {
      default:
         yac_host->printf("[~~~] glEnable: unhandled enum %d\n", _a);
         break;

      case GL_TEXTURE_1D:
      case GL_TEXTURE_2D:
      case GL_TEXTURE_3D:
      case GL_TEXTURE_CUBE_MAP:
      case GL_TEXTURE_RECTANGLE:
         Dactivetextureunit(mode) = _a;
         break;

      case GL_BLEND:
      case GL_CULL_FACE:
      case GL_DEPTH_TEST:
      case GL_DITHER:
      case GL_POLYGON_OFFSET_FILL:
      case GL_SAMPLE_ALPHA_TO_COVERAGE:
      case GL_SAMPLE_COVERAGE:
      case GL_SCISSOR_TEST:
      case GL_STENCIL_TEST:
         ::glEnable(_a);
         Dtraceglerror("glEnable");
         break;

      case GL_LINE_STIPPLE:
      case GL_POLYGON_STIPPLE:
      case GL_TEXTURE_GEN_Q:
      case GL_TEXTURE_GEN_S:
      case GL_TEXTURE_GEN_T:
      case GL_TEXTURE_GEN_R:
      case GL_COLOR_LOGIC_OP:
      case GL_INDEX_LOGIC_OP:  // 0x0bf1
      case GL_NORMALIZE:
      case GL_POINT_SMOOTH:
      case GL_LINE_SMOOTH:
      case GL_POLYGON_SMOOTH:
#if !defined(DX_GLES) && !defined(DX_GL4_CORE)
         // (todo) fixfxn
         ::glEnable(_a);
         Dtraceglerror("glEnable");
#endif // DX_GLES
         break;

#ifdef DX_EMU_FOG
      case GL_FOG:
         tkopengl_attrib->fog.b_enable = YAC_TRUE;
         break;
#endif // DX_EMU_FOG

      case GL_ALPHA_TEST:
         tkopengl_attrib->alphatest.b_enable = YAC_TRUE;
         break;

#ifdef DX_EMU_LIGHT
      case GL_LIGHTING:
         tkopengl_attrib->light.b_enable = YAC_TRUE;
         break;

      case GL_LIGHT0:
         tkopengl_attrib->light.enable_mask |= (1 << 0);
         break;

      case GL_LIGHT1:
         tkopengl_attrib->light.enable_mask |= (1 << 1);
         break;

      case GL_LIGHT2:
         tkopengl_attrib->light.enable_mask |= (1 << 2);
         break;

      case GL_LIGHT3:
         tkopengl_attrib->light.enable_mask |= (1 << 3);
         break;

      case GL_LIGHT4:
         tkopengl_attrib->light.enable_mask |= (1 << 4);
         break;

      case GL_LIGHT5:
         tkopengl_attrib->light.enable_mask |= (1 << 5);
         break;

      case GL_LIGHT6:
         tkopengl_attrib->light.enable_mask |= (1 << 6);
         break;

      case GL_LIGHT7:
         tkopengl_attrib->light.enable_mask |= (1 << 7);
         break;

      case GL_COLOR_MATERIAL:
         tkopengl_attrib->light.b_color_material = YAC_TRUE;
         break;
#endif // DX_EMU_LIGHT

      case GL_POINT_SPRITE:
         tkopengl_attrib->point.sprite.b_enable = YAC_TRUE;
#ifndef DX_GLES
         ::glEnable(_a);  // to turn on gl_PointSize in VP
         Dtraceglerror("glEnable");
#endif // DX_GLES
         break;
   }

#else
#ifdef DX_GLES
   switch(_a)
   {
      default:
         ::glEnable(_a);
         break;

      case GL_TEXTURE_1D:
      case GL_TEXTURE_2D:
      case GL_TEXTURE_3D:
      case GL_TEXTURE_CUBE_MAP:
      case GL_TEXTURE_RECTANGLE:
      case GL_LINE_STIPPLE:
      case GL_POLYGON_STIPPLE:
      case GL_TEXTURE_GEN_Q:
      case GL_TEXTURE_GEN_S:
      case GL_TEXTURE_GEN_T:
      case GL_TEXTURE_GEN_R:
      case GL_POINT_SMOOTH:
      case GL_LINE_SMOOTH:
      case GL_POLYGON_SMOOTH:
      case GL_FOG:
      case GL_ALPHA_TEST:
      case GL_LIGHTING:
      case GL_LIGHT0:
      case GL_LIGHT1:
      case GL_LIGHT2:
      case GL_LIGHT3:
      case GL_LIGHT4:
      case GL_LIGHT5:
      case GL_LIGHT6:
      case GL_LIGHT7:
      case GL_COLOR_MATERIAL:
      case GL_POINT_SPRITE:
         // n/a in GLES, silently discard
         break;
   }
#else
   ::glEnable(_a);
   Dtraceglerror("glEnable");
#endif // DX_GLES
#endif // DX_EMU_FIXFXN
}

// ---------------------------------------------------------------------------- glIsEnabled
sSI YAC_CALL _glIsEnabled(sSI _a) {
   sSI r = ::glIsEnabled((GLenum)_a);
   Dtraceglerror("glIsEnabled");
   return r;
}

// ---------------------------------------------------------------------------- glEnableClientState
void YAC_CALL _glEnableClientState(sSI _array) {
#ifdef DX_EMU_FIXFXN
   switch(_array)
   {
   default:
   case GL_EDGE_FLAG_ARRAY:
   case GL_INDEX_ARRAY:
      yac_host->printf("[---] glEnableClientState: unsupported array type 0x%04x\n", (sU16)_array);
      last_gl_error = GL_INVALID_ENUM;
      break;

   case GL_COLOR_ARRAY:
      tkopengl_attrib->userarray[TKOPENGL_USERARRAY_COLOR].b_enable = YAC_TRUE;
      break;

   case GL_NORMAL_ARRAY:
      tkopengl_attrib->userarray[TKOPENGL_USERARRAY_NORMAL].b_enable = YAC_TRUE;
      break;

   case GL_TEXTURE_COORD_ARRAY:
      tkopengl_attrib->userarray[TKOPENGL_USERARRAY_TEXCOORD].b_enable = YAC_TRUE;
      break;

   case GL_VERTEX_ARRAY:
      tkopengl_attrib->userarray[TKOPENGL_USERARRAY_VERTEX].b_enable = YAC_TRUE;
      break;
   }
#else
#ifdef DX_GLES
   Dstub(glEnableClientState);
#else
   ::glEnableClientState(_array);
   Dtraceglerror("glEnableClientState");
#endif // DX_GLES
#endif // DX_EMU_FIXFXN
}

// ---------------------------------------------------------------------------- glEnd
void YAC_CALL _glEnd(void) {

#ifndef DONT_DRAW

   if(tkopengl_b_glBegin)
   {
      tkopengl_b_glBegin = 0;

#ifdef DX_EMU_IMM
      tkopengl_imm_end();
#else
#ifdef DX_GLES
      Dstub(glEnd);
#else
      Dcompat(glEnd)
      {
         ::glEnd();
         Dtraceglerror("glEnd");
      }
#endif // DX_GLES
#endif // DX_EMU_IMM
   }
   else
   {
      yac_host->printf("[~~~] tkopengl::glEnd: calling glEnd() w/o glBegin().\n");
   }

#endif // DONT_DRAW
}

// ---------------------------------------------------------------------------- glFinish
void YAC_CALL _glFinish(void) {
   ::glFinish();
   Dtraceglerror("glFinish");
}

// ---------------------------------------------------------------------------- glFlush
void YAC_CALL _glFlush(void) {
   ::glFlush();
   Dtraceglerror("glFlush");
}

// ---------------------------------------------------------------------------- glFrontFace
void YAC_CALL _glFrontFace(sSI _a) {
   ::glFrontFace(_a);
   Dtraceglerror("glFrontFace");
}

// ---------------------------------------------------------------------------- glFrustum
void YAC_CALL _glFrustum(sF32 _left, sF32 _right,
                         sF32 _bottom, sF32 _top,
                         sF32 _znear, sF32 _zfar)
{
#ifdef DX_EMU_MATRIX
   tkopengl_matrix_frustum(tkopengl_matrix, _left, _right, _bottom, _top, _znear, _zfar);
   tkopengl_matrix_flags[tkopengl_matrix_mode] |= TKOPENGL_MATRIX_FLAG_DIRTY;
   tkopengl_matrix_flags[tkopengl_matrix_mode] &= ~(TKOPENGL_MATRIX_FLAG_ORTHOGONAL | TKOPENGL_MATRIX_FLAG_IDENTITY);
#else
#ifdef DX_GLES
   Dstub(glFrustum);
#else
   ::glFrustum(_left, _right, _bottom, _top, _znear, _zfar);
   Dtraceglerror("glFrustum");
#endif // DX_GLES
#endif // DX_EMU_MATRIX
}

// ---------------------------------------------------------------------------- glHint
void YAC_CALL _glHint(sSI _target, sSI _mode) {
#ifdef DX_GLES
   if(GL_GENERATE_MIPMAP_HINT == _target)
   {
      ::glHint(_target, _mode);
   }
   else
   {
      // ignore desktop-GL hints
      //  GL_FOG_HINT
      //  GL_LINE_SMOOTH_HINT
      //  GL_PERSPECTIVE_CORRECTION_HINT
      //  GL_POINT_SMOOTH_HINT
      //  GL_POLYGON_SMOOTH_HINT
   }
#else
   ::glHint(_target, _mode);
   Dtraceglerror("glHint");
#endif // DX_GLES
}

// ---------------------------------------------------------------------------- glLineWidth
void YAC_CALL _glLineWidth(sF32 _a) {
   ::glLineWidth(_a);
   Dtraceglerror("glLineWidth");
}

// ---------------------------------------------------------------------------- glLineStipple
void YAC_CALL _glLineStipple(sSI _a, sSI _b) {
#ifdef DX_GLES
   // (todo) gles
   Dstub(glLineStipple);
#else
   ::glLineStipple(_a, _b);
   Dtraceglerror("glLineStipple");
#endif // DX_GLES
}

// ---------------------------------------------------------------------------- glLoadIdentity
void YAC_CALL _glLoadIdentity(void) {
#ifdef DX_EMU_MATRIX
   tkopengl_matrix_identity(tkopengl_matrix);
   tkopengl_matrix_flags[tkopengl_matrix_mode] |= TKOPENGL_MATRIX_FLAG_DIRTY;
   tkopengl_matrix_flags[tkopengl_matrix_mode] |= TKOPENGL_MATRIX_FLAG_ORTHOGONAL;
   tkopengl_matrix_flags[tkopengl_matrix_mode] |= TKOPENGL_MATRIX_FLAG_IDENTITY;
#else
#ifdef DX_GLES
   Dstub(glLoadIdentity);
#else
   ::glLoadIdentity();
   Dtraceglerror("glLoadIdentity");
#endif // DX_GLES
#endif // DX_EMU_MATRIX
}

// ---------------------------------------------------------------------------- glLogicOp
void YAC_CALL _glLogicOp(sSI _a) {
#ifdef DX_EMU_FIXFXN
   // (todo) fixfxn
   Dstub(glLogicOp);
#else
#ifdef DX_GLES
   Dstub(glLogicOp);
#else
   ::glLogicOp(_a);
   Dtraceglerror("glLogicOp");
#endif // DX_GLES
#endif // DX_EMU_FIXFXN
}

// ---------------------------------------------------------------------------- glMatrixMode
void YAC_CALL _glMatrixMode(sSI _a) {
#ifdef DX_EMU_MATRIX
   switch(_a)
   {
      default:
         Dhandleglerror(GL_INVALID_ENUM, "glMatrixMode_emu");
         last_gl_error = GL_INVALID_ENUM;
         break;

      case GL_MODELVIEW:
         tkopengl_matrix = &tkopengl_modelview_matrix_stack[tkopengl_modelview_matrix_stacki << 4]; // *16 (4x4 mat)
         tkopengl_matrix_mode = TKOPENGL_MATRIX_MODE_MODELVIEW;
         tkopengl_matrix_stackip = &tkopengl_modelview_matrix_stacki;
         break;

      case GL_PROJECTION:
         tkopengl_matrix = &tkopengl_projection_matrix_stack[tkopengl_projection_matrix_stacki << 4]; // *16 (4x4 mat)
         tkopengl_matrix_mode = TKOPENGL_MATRIX_MODE_PROJECTION;
         tkopengl_matrix_stackip = &tkopengl_projection_matrix_stacki;
         break;

      case GL_TEXTURE:
         tkopengl_matrix = &tkopengl_texture_matrix_stack[tkopengl_texture_matrix_stacki << 4]; // *16 (4x4 mat)
         tkopengl_matrix_mode = TKOPENGL_MATRIX_MODE_TEXTURE;
         tkopengl_matrix_stackip = &tkopengl_texture_matrix_stacki;
         break;
   }
#else
#ifdef DX_GLES
   Dstub(glMatrixMode);
#else
   ::glMatrixMode(_a);
   Dtraceglerror("glMatrixMode");
#endif // DX_GLES
#endif // DX_EMU_MATRIX
}

// ---------------------------------------------------------------------------- glMultiTexCoord1f
void YAC_CALL _glMultiTexCoord1f(sSI _a, sF32 _b) {
#ifdef DX_EMU_MULTITEX
   // (todo) multitex
   Dstub(glMultiTexCoord1f);
#else
   Dcheckext(glMultiTexCoord1f)
   {
      ::glMultiTexCoord1f_xref(_a, _b);
      Dtracegldrawerror("glMultiTexCoord1f");
   }
#endif // DX_EMU_MULTITEX
}

// ---------------------------------------------------------------------------- glMultiTexCoord2f
void YAC_CALL _glMultiTexCoord2f(sSI _a, sF32 _b, sF32 _c)  {
#ifdef DX_EMU_MULTITEX
   // (todo) multitex
   Dstub(glMultiTexCoord2f);
#else
   Dcheckext(glMultiTexCoord2f)
   {
      ::glMultiTexCoord2f_xref(_a, _b, _c);
      Dtracegldrawerror("glMultiTexCoord2f");
   }
#endif // DX_EMU_MULTITEX
}

// ---------------------------------------------------------------------------- glMultiTexCoord3f
void YAC_CALL _glMultiTexCoord3f(sSI _a, sF32 _b, sF32 _c, sF32 _d) {
#ifdef DX_EMU_MULTITEX
   // (todo) multitex
   Dstub(glMultiTexCoord3f);
#else
   Dcheckext(glMultiTexCoord3f)
   {
      ::glMultiTexCoord3f_xref(_a, _b, _c, _d);
      Dtracegldrawerror("glMultiTexCoord3f");
   }
#endif // DX_EMU_MULTITEX
}

// ---------------------------------------------------------------------------- glNormal3f
void YAC_CALL _glNormal3f(sF32 _a, sF32 _b, sF32 _c) {
#ifdef DX_EMU_IMM
   tkopengl_imm_normal_cbk(_a, _b, _c);
#else
#ifdef DX_GLES
   Dstub(glNormal3f);
#else
   ::glNormal3f(_a, _b, _c);
   Dtracegldrawerror("glNormal3f");
#endif // DX_GLES
#endif // DX_EMU_IMM
}

// ---------------------------------------------------------------------------- glOrtho
void YAC_CALL _glOrtho(sF32 _left, sF32 _right,
                       sF32 _bottom, sF32 _top,
                       sF32 _znear, sF32 _zfar)
{
#ifdef DX_EMU_MATRIX
   tkopengl_matrix_ortho(tkopengl_matrix, _left, _right, _bottom, _top, _znear, _zfar);
   tkopengl_matrix_flags[tkopengl_matrix_mode] |= TKOPENGL_MATRIX_FLAG_DIRTY;
   tkopengl_matrix_flags[tkopengl_matrix_mode] &= ~(TKOPENGL_MATRIX_FLAG_ORTHOGONAL | TKOPENGL_MATRIX_FLAG_IDENTITY);
#else
#ifdef DX_GLES
   Dstub(glOrtho);
#else
   ::glOrtho(_left, _right, _bottom, _top, _znear, _zfar);
   Dtraceglerror("glOrtho");
#endif // DX_GLES
#endif // DX_EMU_MATRIX
}

// ---------------------------------------------------------------------------- glPixelStorei_int
void YAC_CALL _glPixelStorei_int(GLenum _pname, GLint _param) {
   // (note) Texture class overrides this
   ::glPixelStorei(_pname, _param);
   Dtraceglerror("glPixelStorei");
}

// ---------------------------------------------------------------------------- glPointSize
void YAC_CALL _glPointSize(sF32 _size) {
#ifdef DX_EMU_FIXFXN
   if(_size > 0.0f)
   {
      tkopengl_attrib->point.size = _size;
   }
   else
   {
      yac_host->printf("[---] tkopengl::glPointSize: invalid size %f\n", _size);
      last_gl_error = GL_INVALID_VALUE;
   }
#else
#ifdef DX_GLES
   // (note) must be set in vertex shader by writing to gl_PointSize
   Dstub(glPointSize);
#else
   ::glPointSize(_size);
   Dtraceglerror("glPointSize");
#endif // DX_GLES
#endif // DX_EMU_FIXFXN
}

// ---------------------------------------------------------------------------- glPointParameterf
void YAC_CALL _glPointParameterf(sSI _pname, sF32 _param) {
#ifdef DX_EMU_FIXFXN
   switch(_pname)
   {
   default:
      yac_host->printf("[---] tkopengl::glPointParameterf: unhandled pname 0x%04x\n", (sU16)_pname);
      last_gl_error = GL_INVALID_ENUM;
      break;

   case GL_POINT_SIZE_MIN:
      if(_param >= 0.0f)
      {
         tkopengl_attrib->point.min_size = _param;
      }
      else
      {
         yac_host->printf("[---] tkopengl::glPointParameterf: invalid GL_POINT_SIZE_MIN (%f < 0.0f)\n", _param);
         last_gl_error = GL_INVALID_VALUE;
      }
      break;

   case GL_POINT_SIZE_MAX:
      if(_param >= 0.0f)
      {
         tkopengl_attrib->point.max_size = _param;
      }
      else
      {
         yac_host->printf("[---] tkopengl::glPointParameterf: invalid GL_POINT_SIZE_MAX (%f < 0.0f)\n", _param);
         last_gl_error = GL_INVALID_VALUE;
      }
      break;

   case GL_POINT_FADE_THRESHOLD_SIZE:
      if(_param >= 0.0f)
      {
         tkopengl_attrib->point.fade_threshold_size = _param;
      }
      else
      {
         yac_host->printf("[---] tkopengl::glPointParameterf: invalid GL_POINT_FADE_THRESHOLD_SIZE (%f < 0.0f)\n", _param);
         last_gl_error = GL_INVALID_VALUE;
      }
      break;

   case GL_POINT_SPRITE_COORD_ORIGIN:
      if( ((sSI)_param) != GL_UPPER_LEFT)
      {
         yac_host->printf("[---] tkopengl::glPointParameterf: unhandled GL_POINT_SPRITE_COORD_ORIGIN param 0x%04x\n", (sU16)_param);
         last_gl_error = GL_INVALID_VALUE;
      }
      break;
   }
#else
#ifdef DX_GLES
   Dstub(glPointParameterf);
#else
   Dcheckext(glPointParameterf)
   {
      glPointParameterf_xref(_pname, _param);
      Dtraceglerror("glPointParameterf");
   }
#endif // DX_GLES
#endif // DX_EMU_FIXFXN
}

// ---------------------------------------------------------------------------- zglPointParameter3f
void YAC_CALL _zglPointParameter3f(sSI _pname, sF32 _v0, sF32 _v1, sF32 _v2) {
#ifdef DX_EMU_FIXFXN
   switch(_pname)
   {
   default:
      yac_host->printf("[---] tkopengl::zglPointParameter3f: unhandled pname 0x%04x\n", (sU16)_pname);
      last_gl_error = GL_INVALID_ENUM;
      break;

   case GL_POINT_DISTANCE_ATTENUATION:
      tkopengl_attrib->point.attenuation.constant  = _v0;
      tkopengl_attrib->point.attenuation.linear    = _v1;
      tkopengl_attrib->point.attenuation.quadratic = _v2;
      break;
   }
#else
#ifdef DX_GLES
   Dstub(glPointParameterfv);
#else
   Dcheckext(glPointParameterfv)
   {
      sF32 params[3];
      params[0] = _v0;
      params[1] = _v1;
      params[2] = _v2;
      glPointParameterfv_xref(_pname, params);
      Dtraceglerror("glPointParameterfv");
   }
#endif // DX_GLES
#endif // DX_EMU_FIXFXN
}

// ---------------------------------------------------------------------------- glPointParameterfv_int
void YAC_CALL _glPointParameterfv_int(sSI _pname, const sF32 *_params) {
#ifdef DX_EMU_FIXFXN
   if(GL_POINT_DISTANCE_ATTENUATION == _pname)
   {
      _zglPointParameter3f(_pname, _params[0], _params[1], _params[2]);
   }
   else
   {
      _glPointParameterf(_pname, _params[0]);
   }
#else
#ifdef DX_GLES
   Dstub(glPointParameterfv);
#else
   Dcheckext(glPointParameterfv)
   {
      glPointParameterfv_xref(_pname, _params);
      Dtraceglerror("glPointParameterfv");
   }
#endif // DX_GLES
#endif // DX_EMU_FIXFXN
}

// ---------------------------------------------------------------------------- glPixelZoom
void YAC_CALL _glPixelZoom(sF32 _a, sF32 _b) {
#ifdef DX_GLES
   // (todo) gles
   Dstub(glPixelZoom);
#else
   ::glPixelZoom(_a, _b);
   Dtraceglerror("glPixelZoom");
#endif // DX_GLES
}

// ---------------------------------------------------------------------------- glPolygonMode
void YAC_CALL _glPolygonMode(sSI _face, sSI _mode) {
#ifdef DX_EMU_FIXFXN
   switch(_face)
   {
      default:
         yac_host->printf("[---] tkopengl::glPolygonMode: invalid face 0x%04x\n", (sU16)_face);
         last_gl_error = GL_INVALID_ENUM;
         break;

      // case GL_FRONT:
      //    yac_host->printf("[---] tkopengl::glPolygonMode: invalid face GL_FRONT\n");
      //    last_gl_error = GL_INVALID_ENUM;
      //    break;

      // case GL_BACK:
      //    yac_host->printf("[---] tkopengl::glPolygonMode: invalid face GL_BACK\n");
      //    last_gl_error = GL_INVALID_ENUM;
      //    break;


         // (note) silently ignore GL_FRONT/GL_BACK face type
      case GL_FRONT:
      case GL_BACK:
      case GL_FRONT_AND_BACK:
         switch(_mode)
         {
            default:
               yac_host->printf("[---] tkopengl::glPolygonMode: invalid mode 0x%04x\n", (sU16)_mode);
               last_gl_error = GL_INVALID_ENUM;
               break;

            case GL_POINT:
            case GL_LINE:
            case GL_FILL:
               tkopengl_attrib->polygon_mode = _mode;
               break;
         }
         break;
   }
#else
#ifdef DX_GLES
   Dstub(glPolygonMode);
#else
   ::glPolygonMode(_face, _mode);
   Dtraceglerror("glPolygonMode");
#endif // DX_GLES
#endif // DX_EMU_FIXFXN
}

// ---------------------------------------------------------------------------- glPolygonOffset
void YAC_CALL _glPolygonOffset(sF32 _a, sF32 _b) {
   ::glPolygonOffset(_a, _b);
   Dtraceglerror("glPolygonOffset");
}

// ---------------------------------------------------------------------------- glPopAttrib
void YAC_CALL _glPopAttrib(void) {
#ifdef DX_EMU_FIXFXN
   tkopengl_attrib_pop();
#else
#ifdef DX_GLES
   Dstub(glPopAttrib);
#else
   ::glPopAttrib();
   Dtraceglerror("glPopAttrib");
#endif // DX_GLES
#endif // DX_EMU_FIXFXN
}

// ---------------------------------------------------------------------------- glPushAttrib
void YAC_CALL _glPushAttrib(sUI _a) {
#ifdef DX_EMU_FIXFXN
   (void)_a; // (todo) currently assume GL_ALL_ATTRIB_BITS
   tkopengl_attrib_push();
#else
#ifdef DX_GLES
   Dstub(glPushAttrib);
#else
   ::glPushAttrib(_a);
   Dtraceglerror("glPushAttrib");
#endif // DX_GLES
#endif // DX_EMU_FIXFXN
}

// ---------------------------------------------------------------------------- glPopClientAttrib
void YAC_CALL _glPopClientAttrib(void) {
#ifdef DX_EMU_FIXFXN
   // (todo) fixfxn
   Dstub(glPopClientAttrib);
#else
#ifdef DX_GLES
   Dstub(glPopClientAttrib);
#else
   ::glPopClientAttrib();
   Dtraceglerror("glPopClientAttrib");
#endif // DX_GLES
#endif // DX_EMU_FIXFXN
}

// ---------------------------------------------------------------------------- glPushClientAttrib
void YAC_CALL _glPushClientAttrib(sUI _a) {
#ifdef DX_EMU_FIXFXN
   // (todo) fixfxn
   Dstub(glPushClientAttrib);
#else
#ifdef DX_GLES
   Dstub(glPushClientAttrib);
#else
   ::glPushClientAttrib(_a);
   Dtraceglerror("glPushClientAttrib");
#endif // DX_GLES
#endif // DX_EMU_FIXFXN
}

// ---------------------------------------------------------------------------- glPopMatrix
void YAC_CALL _glPopMatrix(void) {
#ifdef DX_EMU_MATRIX
   if(*tkopengl_matrix_stackip < TKOPENGL_MATRIX_STACK_SIZE)
   {
     (*tkopengl_matrix_stackip)++;

     tkopengl_matrix += (4 * 4);
     tkopengl_matrix_flags[tkopengl_matrix_mode] |= TKOPENGL_MATRIX_FLAG_DIRTY;
     tkopengl_matrix_flags[tkopengl_matrix_mode] &= ~(TKOPENGL_MATRIX_FLAG_ORTHOGONAL | TKOPENGL_MATRIX_FLAG_IDENTITY);
     // (todo) pop matrix flags
   }
   else
   {
      Dhandleglerror(GL_STACK_UNDERFLOW, "glPopMatrix_emu");
   }
#else
#ifdef DX_GLES
   Dstub(glPopMatrix);
#else
   ::glPopMatrix();
   Dtraceglerror("glPopMatrix");
#endif // DX_GLES
#endif
}

// ---------------------------------------------------------------------------- glPushMatrix
void YAC_CALL _glPushMatrix(void) {
#ifdef DX_EMU_MATRIX
   if(*tkopengl_matrix_stackip > 0)
   {
     (*tkopengl_matrix_stackip)--;

     tkopengl_matrix -= (4 * 4);

     for(int i=0; i<16; i++)
     {
        tkopengl_matrix[i] = tkopengl_matrix[i+16];
     }

   }
   else
   {
      Dhandleglerror(GL_STACK_OVERFLOW, "glPushMatrix_emu");
   }
#else
#ifdef DX_GLES
   Dstub(glPushMatrix);
#else
   ::glPushMatrix();
   Dtraceglerror("glPushMatrix");
#endif // DX_GLES
#endif // DX_EMU_MATRIX
}

// ---------------------------------------------------------------------------- glRasterPos2i
void YAC_CALL _glRasterPos2i(sSI _a, sSI _b) {
#ifdef DX_GLES
   // (todo) gles
   Dstub(glRasterPos2i);
#else
   ::glRasterPos2i(_a, _b);
   Dtraceglerror("glRasterPos2i");
#endif // DX_GLES
}

// ---------------------------------------------------------------------------- glRasterPos3f
void YAC_CALL _glRasterPos3f(sF32 _a, sF32 _b, sF32 _c) {
#ifdef DX_GLES
   // (todo) gles
   Dstub(glRasterPos3f);
#else
   ::glRasterPos3f(_a, _b, _c);
   Dtraceglerror("glRasterPos3f");
#endif // DX_GLES
}

// ---------------------------------------------------------------------------- glReadBuffer
void YAC_CALL _glReadBuffer(sSI _a) {
   ::glReadBuffer(_a);
   Dtraceglerror("glReadBuffer");
}

// ---------------------------------------------------------------------------- glRectf
void YAC_CALL _glRectf(sF32 _x1, sF32 _y1, sF32 _x2, sF32 _y2) {
#ifdef DX_EMU_IMM
   _glBegin(GL_QUADS);
   _glVertex2f(_x1, _y1);
   _glVertex2f(_x2, _y1);
   _glVertex2f(_x2, _y2);
   _glVertex2f(_x1, _y2);
   _glEnd();
#else
#ifdef DX_GLES
   Dstub(glRectf);
#else
   ::glRectf(_x1, _y1, _x2, _y2);
   Dtraceglerror("glRectf");
#endif // DX_GLES
#endif // DX_EMU_IMM
}

// ---------------------------------------------------------------------------- glRotate3f
void YAC_CALL _glRotatef(sF32 _ang, sF32 _x, sF32 _y, sF32 _z) {
#ifdef DX_EMU_MATRIX
   tkopengl_matrix_rotate(tkopengl_matrix, _ang, _x, _y, _z);
   tkopengl_matrix_flags[tkopengl_matrix_mode] |= TKOPENGL_MATRIX_FLAG_DIRTY;
   tkopengl_matrix_flags[tkopengl_matrix_mode] &= ~TKOPENGL_MATRIX_FLAG_IDENTITY;
#else
#ifdef DX_GLES
   Dstub(glRotatef);
#else
   ::glRotatef(_ang, _x, _y, _z);
   Dtraceglerror("glRotatef");
#endif // DX_GLES
#endif // DX_EMU_MATRIX
}

// ---------------------------------------------------------------------------- glScalef
void YAC_CALL _glScalef(sF32 _x, sF32 _y, sF32 _z) {
#ifdef DX_EMU_MATRIX
   tkopengl_matrix_scale(tkopengl_matrix, _x, _y, _z);
   tkopengl_matrix_flags[tkopengl_matrix_mode] |= TKOPENGL_MATRIX_FLAG_DIRTY;
   tkopengl_matrix_flags[tkopengl_matrix_mode] &= ~TKOPENGL_MATRIX_FLAG_IDENTITY;

   if(! ( (_x == _y) && (_x == _z) ) )
   {
      tkopengl_matrix_flags[tkopengl_matrix_mode] &= ~TKOPENGL_MATRIX_FLAG_ORTHOGONAL;
   }
#else
#ifdef DX_GLES
   Dstub(glScalef);
#else
   ::glScalef(_x, _y, _z);
   Dtraceglerror("glScalef");
#endif // DX_GLES
#endif // DX_EMU_MATRIX
}
// ---------------------------------------------------------------------------- glScissor
void YAC_CALL _glScissor(sSI _x, sSI _y, sSI _w, sSI _h) {
   ::glScissor(_x, _y, _w, _h);
   Dtraceglerror("glScissor");
}

// ---------------------------------------------------------------------------- glShadeModel
void YAC_CALL _glShadeModel(sSI _mode) {
#ifdef DX_EMU_FIXFXN
   switch(_mode)
   {
   default:
      yac_host->printf("[---] tkopengl::glShadeModel: invalid mode 0x%04x\n", (sU16)_mode);
      last_gl_error = GL_INVALID_ENUM;
      break;

   case GL_FLAT:
      yac_host->printf("[---] tkopengl::glShadeModel: unsupported mode GL_FLAT\n");
      last_gl_error = GL_INVALID_ENUM;
      break;

   case GL_SMOOTH:
      /* nothing to do here */
      break;
   }
#else
#ifdef DX_GLES
   Dstub(glShadeModel);
#else
   ::glShadeModel(_mode);
   Dtraceglerror("glShadeModel");
#endif // DX_GLES
#endif // DX_EMU_FIXFXN
}

// ---------------------------------------------------------------------------- glStencilFunc
void YAC_CALL _glStencilFunc(sSI _func, sSI _ref, sUI _mask) {
   ::glStencilFunc(_func, _ref, _mask);
   Dtraceglerror("glStencilFunc");
}

// ---------------------------------------------------------------------------- glStencilOp
void YAC_CALL _glStencilOp(sSI _sfail, sSI _dpfail, sSI _dppass) {
   ::glStencilOp(_sfail, _dpfail, _dppass);
   Dtraceglerror("glStencilOp");
}

// ---------------------------------------------------------------------------- glStencilMask
void YAC_CALL _glStencilMask(sUI _mask) {
   ::glStencilMask(_mask);
   Dtraceglerror("glStencilMask");
}

// ---------------------------------------------------------------------------- glTexEnvf
void YAC_CALL _glTexEnvf(sSI _target, sSI _pname, sF32 _param) {
#ifdef DX_EMU_FIXFXN
   _glTexEnvfv_int(_target, _pname, &_param);
#else
#ifdef DX_GLES
   Dstub(glTexEnvf);
#else
   ::glTexEnvf(_target, _pname, _param);
   Dtraceglerror("glTexEnvf");
#endif // DX_GLES
#endif // DX_EMU_FIXFXN
}

// ---------------------------------------------------------------------------- glTexEnvfv_int
void YAC_CALL _glTexEnvfv_int(GLenum _target, GLenum _pname, const GLfloat *_params) {
#ifdef DX_EMU_FIXFXN
   switch(_target)
   {
      // case GL_POINT_SPRITE_OES:
      default:
         yac_host->printf("[---] tkopengl::glTexEnvfv_int: unhandled target 0x%04x\n", (sU16)_target);
         last_gl_error = GL_INVALID_ENUM;
         break;

      case GL_TEXTURE_ENV:
         switch(_pname)
         {
            default:
               yac_host->printf("[---] tkopengl::glTexEnvfv_int: unhandled GL_TEXTURE_ENV pname 0x%04x\n", (sU16)_pname);
               last_gl_error = GL_INVALID_ENUM;
               break;

            case GL_TEXTURE_ENV_MODE:
               switch((GLenum)_params[0])
               {
                  default:
                     yac_host->printf("[---] tkopengl::glTexEnvfv_int: unhandled GL_TEXTURE_ENV_MODE param 0x%04x\n", (sU16)_params[0]);
                     last_gl_error = GL_INVALID_ENUM;
                     break;

                  case GL_REPLACE:
                  case GL_MODULATE:
                  case GL_DECAL:
                  case GL_BLEND:
                  case GL_ADD:
                     Dactivetextureunit(env_mode) = (GLenum)_params[0];
                     break;
               }
               break;

            case GL_TEXTURE_ENV_COLOR:
               Dactivetextureunit(env_color[0]) = _params[0];
               Dactivetextureunit(env_color[1]) = _params[1];
               Dactivetextureunit(env_color[2]) = _params[2];
               Dactivetextureunit(env_color[3]) = _params[3];
               break;
         }
         break;
   }
#else
#ifdef DX_GLES
   Dstub(glTexEnvfv);
#else
   ::glTexEnvfv(_target, _pname, _params);
#endif // DX_GLES
#endif // DX_EMU_FIXFXN
}

// ---------------------------------------------------------------------------- zglTexEnvColor4f
void YAC_CALL _zglTexEnvColor4f(sF32 _r, sF32 _g, sF32 _b, sF32 _a) {
#ifdef DX_EMU_FIXFXN
   Dactivetextureunit(env_color[0]) = _r;
   Dactivetextureunit(env_color[1]) = _g;
   Dactivetextureunit(env_color[2]) = _b;
   Dactivetextureunit(env_color[3]) = _a;
#else
#ifdef DX_GLES
   Dstub(zglTexEnvColor4f);
#else
   sF32 params[4];
   params[0] = _r;
   params[1] = _g;
   params[2] = _b;
   params[3] = _a;
   ::glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, params);
   Dtraceglerror("zglTexEnvColor4f");
#endif // DX_GLES
#endif // DX_EMU_FIXFXN
}

// ---------------------------------------------------------------------------- glTexEnvi
void YAC_CALL _glTexEnvi(sSI _target, sSI _pname, sSI _param) {
#ifdef DX_EMU_FIXFXN
   switch(_pname)
   {
      default:
         break;

      case GL_TEXTURE_ENV_MODE:
         switch(_param)
         {
            default:
               yac_host->printf("[~~~] glTexEnvi: unhandled GL_TEXTURE_ENV_MODE %d\n", _param);
               break;

            case GL_REPLACE:
            case GL_MODULATE:
            case GL_DECAL:
            case GL_BLEND:
            case GL_ADD:
               Dactivetextureunit(env_mode) = _param;
               break;
         }
         break;

      case GL_COORD_REPLACE:
         tkopengl_attrib->point.sprite.b_coord_replace = (_param ? YAC_TRUE : YAC_FALSE);
         break;
   }
#else
#ifdef DX_GLES
   Dstub(glTexEnvi);
#else
   ////printf("xxx glTexEnvi(0x%04x, 0x%04x, 0x%04x)\n", _target, _pname, _param);
   Dcompat(glTexEnvi)
   {
      ::glTexEnvi(_target, _pname, _param);
      Dtraceglerror("glTexEnvi");
   }
#endif // DX_GLES
#endif // DX_EMU_FIXFXN
}

// ---------------------------------------------------------------------------- glTexCoord1f
void YAC_CALL _glTexCoord1f(sF32 _s) {
#ifdef DX_EMU_IMM
   tkopengl_imm_texcoord_cbk(_s, 0.0f, 0.0f, 1.0f);
#else
#ifdef DX_GLES
   Dstub(glTexCoord1f);
#else
   Dcompat(glTexCoord1f)
   {
      ::glTexCoord1f(_s);
      Dtracegldrawerror("glTexCoord1f");
   }
#endif // DX_GLES
#endif // DX_EMU_IMM
}

// ---------------------------------------------------------------------------- glTexCoord2f
void YAC_CALL _glTexCoord2f(sF32 _s, sF32 _t) {
#ifdef DX_EMU_IMM
   tkopengl_imm_texcoord_cbk(_s, _t, 0.0f, 1.0f);
#else
#ifdef DX_GLES
   Dstub(glTexCoord2f);
#else
   Dcompat(glTexCoord2f)
   {
      ::glTexCoord2f(_s, _t);
      Dtracegldrawerror("glTexCoord2f");
   }
#endif // DX_GLES
#endif // DX_EMU_IMM
}

// ---------------------------------------------------------------------------- zglTexCoord2v
void YAC_CALL _zglTexCoord2v(YAC_Object *_v) {
   if(YAC_VALID(_v) && _v->yacArrayGetNumElements() >= 2u)
   {
      YAC_Value r1; _v->yacArrayGet(NULL/*context*/, 0u, &r1); r1.typecast(YAC_TYPE_FLOAT);
      YAC_Value r2; _v->yacArrayGet(NULL/*context*/, 1u, &r2); r2.typecast(YAC_TYPE_FLOAT);

#ifdef DX_EMU_IMM
      tkopengl_imm_texcoord_cbk(r1.value.float_val, r2.value.float_val, 0.0f, 1.0f);
#else
#ifdef DX_GLES
      Dstub(zglTexCoord2v);
#else
      Dcompat(glTexCoord2f)
      {
         ::glTexCoord2f(r1.value.float_val, r2.value.float_val);
         Dtracegldrawerror("zglTexCoord2v");
      }
#endif // DX_GLES
#endif // DX_EMU_IMM
   }
}

// ---------------------------------------------------------------------------- glTexCoord3f
void YAC_CALL _glTexCoord3f(sF32 _s, sF32 _t, sF32 _r) {
#ifdef DX_EMU_IMM
   tkopengl_imm_texcoord_cbk(_s, _t, _r, 1.0f);
#else
#ifdef DX_GLES
   Dstub(glTexCoord3f);
#else
   Dcompat(glTexCoord3f)
   {
      ::glTexCoord3f(_s, _t, _r);
      Dtracegldrawerror("glTexCoord3f");
   }
#endif // DX_GLES
#endif // DX_EMU_IMM
}

// ---------------------------------------------------------------------------- zglTexCoord3v
void YAC_CALL _zglTexCoord3v(YAC_Object *_v) {
   if(YAC_VALID(_v) && _v->yacArrayGetNumElements() >= 3u)
   {
      YAC_Value r1; _v->yacArrayGet(NULL/*context*/, 0u, &r1); r1.typecast(YAC_TYPE_FLOAT);
      YAC_Value r2; _v->yacArrayGet(NULL/*context*/, 1u, &r2); r2.typecast(YAC_TYPE_FLOAT);
      YAC_Value r3; _v->yacArrayGet(NULL/*context*/, 2u, &r3); r3.typecast(YAC_TYPE_FLOAT);

#ifdef DX_EMU_IMM
      tkopengl_imm_texcoord_cbk(r1.value.float_val, r2.value.float_val, r3.value.float_val, 1.0f);
#else
#ifdef DX_GLES
      Dstub(zglTexCoord3v);
#else
      Dcompat(zglTexCoord3v)
      {
         ::glTexCoord3f(r1.value.float_val, r2.value.float_val, r3.value.float_val);
         Dtracegldrawerror("zglTexCoord3v");
      }
#endif // DX_GLES
#endif // DX_EMU_IMM
   }
}

// ---------------------------------------------------------------------------- glTexCoord4f
void YAC_CALL _glTexCoord4f(sF32 _s, sF32 _t, sF32 _r, sF32 _q) {
#ifdef DX_EMU_IMM
   tkopengl_imm_texcoord_cbk(_s, _t, _r, _q);
#else
#ifdef DX_GLES
   Dstub(glTexCoord4f);
#else
   Dcompat(glTexCoord4f)
   {
      ::glTexCoord4f(_s, _t, _r, _q);
      Dtracegldrawerror("glTexCoord4f");
   }
#endif // DX_GLES
#endif // DX_EMU_IMM
}

// ---------------------------------------------------------------------------- zglTexCoord4v
void YAC_CALL _zglTexCoord4v(YAC_Object *_v) {
   if(YAC_VALID(_v) && _v->yacArrayGetNumElements() >= 4u)
   {
      YAC_Value r1; _v->yacArrayGet(NULL/*context*/, 0u, &r1); r1.typecast(YAC_TYPE_FLOAT);
      YAC_Value r2; _v->yacArrayGet(NULL/*context*/, 1u, &r2); r2.typecast(YAC_TYPE_FLOAT);
      YAC_Value r3; _v->yacArrayGet(NULL/*context*/, 2u, &r3); r3.typecast(YAC_TYPE_FLOAT);
      YAC_Value r4; _v->yacArrayGet(NULL/*context*/, 3u, &r4); r4.typecast(YAC_TYPE_FLOAT);

#ifdef DX_EMU_IMM
      tkopengl_imm_texcoord_cbk(r1.value.float_val, r2.value.float_val, r3.value.float_val, r4.value.float_val);
#else
#ifdef DX_GLES
      Dstub(zglTexCoord4v);
#else
      Dcompat(zglTexCoord4v)
      {
         ::glTexCoord4f(r1.value.float_val, r2.value.float_val, r3.value.float_val, r4.value.float_val);
         Dtracegldrawerror("zglTexCoord4v");
      }
#endif // DX_GLES
#endif // DX_EMU_IMM
   }
}

// ---------------------------------------------------------------------------- glTexCoord1i
void YAC_CALL _glTexCoord1i(sSI _s) {
   // (todo) remove?
#ifdef DX_EMU_IMM
   tkopengl_imm_texcoord_cbk(_s / (sF32)INT_MAX, 0.0f, 0.0f, 1.0f);
#else
#ifdef DX_GLES
   Dstub(glTexCoord1i);
#else
   Dcompat(glTexCoord1i)
   {
      ::glTexCoord1i(_s);
      Dtracegldrawerror("glTexCoord1i");
   }
#endif // DX_GLES
#endif // DX_EMU_IMM
}

// ---------------------------------------------------------------------------- glTexCoord2i
void YAC_CALL _glTexCoord2i(sSI _s, sSI _t) {
   // (todo) remove?
#ifdef DX_EMU_IMM
   tkopengl_imm_texcoord_cbk(_s / (sF32)INT_MAX,
                             _t / (sF32)INT_MAX,
                             0.0f, 1.0f);
#else
#ifdef DX_GLES
   Dstub(glTexCoord2i);
#else
   Dcompat(glTexCoord2i)
   {
      ::glTexCoord2i(_s, _t);
      Dtracegldrawerror("glTexCoord2i");
   }
#endif // DX_GLES
#endif // DX_EMU_IMM
}

// ---------------------------------------------------------------------------- glTexGeni
void YAC_CALL _glTexGeni(sSI _a, sSI _b, sSI _c) {
#ifdef DX_EMU_FIXFXN
   // (todo) fixfxn
   Dstub(glTexGeni);
#else
#ifdef DX_GLES
   Dstub(glTexGeni);
#else
   Dcompat(glTexGeni)
   {
      ::glTexGeni(_a, _b, _c);
      Dtraceglerror("glTexGeni");
   }
#endif // DX_GLES
#endif // DX_EMU_FIXFXN
}

// ---------------------------------------------------------------------------- zglTexGen4f
void YAC_CALL _zglTexGen4f(sSI _a, sSI _b, sF32 _c, sF32 _d, sF32 _e, sF32 _f) {
#ifdef DX_EMU_FIXFXN
   // (todo) fixfxn
   Dstub(zglTexGen4f);
#else
#ifdef DX_GLES
   Dstub(zglTexGen4f);
#else
   Dcompat(zglTexGen4f)
   {
      sF32 params[4];
      params[0] = _c;
      params[1] = _d;
      params[2] = _e;
      params[3] = _f;
      ::glTexGenfv(_a, _b, params);
      Dtraceglerror("zglTexGen4f");
   }
#endif // DX_GLES
#endif // DX_EMU_FIXFXN
}

// ---------------------------------------------------------------------------- glTexParameteri
void YAC_CALL _glTexParameteri(sSI _target, sSI _pname, sSI _param) {
#ifdef DX_EMU_TEXTURE_1D
   if(GL_TEXTURE_1D == _target)
   {
      _target = GL_TEXTURE_2D;
   }
#endif // DX_EMU_TEXTURE_1D

#ifdef DX_EMU_TEXTURE_RECTANGLE
   if(GL_TEXTURE_RECTANGLE == _target)
   {
      _target = GL_TEXTURE_2D;
   }
#endif // DX_EMU_TEXTURE_RECTANGLE

   ::glTexParameteri(_target, _pname, _param);
   Dtraceglerror("glTexParameteri");
}

// ---------------------------------------------------------------------------- glTexImage1D
void YAC_CALL _glTexImage1D(sSI _target, sSI _level, sSI _internalFormat, sUI _width, sSI _border, sSI _format, sSI _type, YAC_Object *_pixels) {
   (void)_pixels; // ignored for now
   _glTexImage1D_int(_target, _level, _internalFormat, _width, _border, _format, _type, NULL);
}

// ---------------------------------------------------------------------------- glTexImage2D_int
void YAC_CALL _glTexImage1D_int(GLenum _target, GLint _level, GLint _internalFormat, GLsizei _width, GLint _border, GLenum _format, GLenum _type, const GLvoid *_data) {

#ifdef DX_EMU_ATTRIB
   Dactivetextureunit(target) = _target;

   if(-1 != Dactivetextureunit(bound_nr))
   {
      Dboundtexture(internal_format) = _internalFormat;
      Dboundtexture(sx)              = (sUI)_width;
      Dboundtexture(sy)              = (sUI)1;
      Dboundtexture(sz)              = (sUI)1;
   }
   else
   {
      yac_host->printf("[~~~] tkopengl::glTexImage1D_int: no texture bound\n");
   }
#endif // DX_EMU_ATTRIB

#ifdef DX_EMU_TEXTURE_1D
   (void)_target;
   _glTexImage2D_int(GL_TEXTURE_2D, _level, _internalFormat, _width, 1/*height*/, _border, _format, _type, _data);
   Dtraceglerror("glTexImage1D_emu");
#else
#ifdef DX_GLES
   Dstub(glTexImage1D);
#else
   ::glTexImage1D(_target, _level, _internalFormat, _width, _border, _format, _type, _data);
   Dtraceglerror("glTexImage1D");
#endif // DX_GLES

#endif // DX_EMU_TEXTURE_1D
}

// ---------------------------------------------------------------------------- glTexImage2D
void YAC_CALL _glTexImage2D(sSI _target, sSI _level, sSI _internalFormat, sUI _width, sUI _height, sSI _border, sSI _format, sSI _type, YAC_Object *_pixels) {
   (void)_pixels; // ignored for now
   _glTexImage2D_int(_target, _level, _internalFormat, _width, _height, _border, _format, _type, NULL/*just allocate*/);
}

// ---------------------------------------------------------------------------- glTexImage2D_int
void YAC_CALL _glTexImage2D_int(GLenum _target, GLint _level, GLint _internalFormat, GLsizei _width, GLsizei _height, GLint _border, GLenum _format, GLenum _type, const GLvoid *_data) {

#ifdef DX_GLES
   switch(_internalFormat)
   {
      case GL_RGB8:
         _internalFormat = GL_RGB;
         break;

      case GL_RGBA8:
         _internalFormat = GL_RGBA;
         break;
   }

   switch(_format)
   {
      default:
         break;
      case GL_BGRA:
         _format = GL_RGBA;
         break;
   }
#endif // DX_GLES

#ifdef DX_EMU_ATTRIB
   Dactivetextureunit(target) = _target;

   if(-1 != Dactivetextureunit(bound_nr))
   {
      Dboundtexture(internal_format) = _internalFormat;
      Dboundtexture(sx)              = (sUI)_width;
      Dboundtexture(sy)              = (sUI)_height;
      Dboundtexture(sz)              = (sUI)1;
   }
   else
   {
      yac_host->printf("[~~~] tkopengl::glTexImage2D_int: no texture bound\n");
   }
#endif // DX_EMU_ATTRIB

#ifdef DX_EMU_TEXTURE_RECTANGLE
   if(GL_TEXTURE_RECTANGLE == _target)
   {
      _target = GL_TEXTURE_2D;
   }
#endif // DX_EMU_TEXTURE_RECTANGLE

   ::glTexImage2D(_target, _level, _internalFormat, _width, _height, _border, _format, _type, _data);
   Dtraceglerror("glTexImage2D");
}

// ---------------------------------------------------------------------------- glTexImage3D
void YAC_CALL _glTexImage3D(sSI _target, sSI _level, sSI _internalFormat, sSI _width, sSI _height, sSI _depth, sSI _border, sSI _format, sSI _type, YAC_Object *_pixels) {
   (void)_pixels; // ignored for now
   _glTexImage3D_int(_target, _level, _internalFormat,
                     _width, _height, _depth,
                     _border, _format, _type,
                     NULL/*just allocate*/
                     );
}

// ---------------------------------------------------------------------------- glTexImage3D_int
void YAC_CALL _glTexImage3D_int(GLenum _target, GLint _level, GLint _internalFormat, GLsizei _width, GLsizei _height, GLsizei _depth, GLint _border, GLenum _format, GLenum _type, const GLvoid *_data) {

#ifdef DX_EMU_ATTRIB
   Dactivetextureunit(target) = _target;

   if(-1 != Dactivetextureunit(bound_nr))
   {
      Dboundtexture(internal_format) = _internalFormat;
      Dboundtexture(sx)              = (sUI)_width;
      Dboundtexture(sy)              = (sUI)_height;
      Dboundtexture(sz)              = (sUI)_depth;
   }
   else
   {
      yac_host->printf("[~~~] tkopengl::glTexImage3D_int: no texture bound\n");
   }
#endif // DX_EMU_ATTRIB

   Dcheckext(glTexImage3D)
   {
      ::glTexImage3D_xref(_target, _level, _internalFormat,
                          _width, _height, _depth,
                          _border, _format, _type, _data);
      Dtraceglerror("glTexImage3D");
   }
}


// ---------------------------------------------------------------------------- glTexStorage2D
void YAC_CALL _glTexStorage2D(sSI _target, sSI _levels, sSI _internalFormat, sUI _width, sUI _height) {
   if(NULL != glTexStorage2D_xref)
   {
      // GL 4.2
      ::glTexStorage2D_xref(_target, _levels, _internalFormat,
                            _width, _height
                            );
      Dtraceglerror("glTexStorage2D");
   }
   else
   {
      // Fallback
      GLint  num;
      GLenum baseFormat;
      GLenum type;
      loc_decompose_internalformat(_internalFormat, num, baseFormat, type);

      for(sSI level = 0; level < _levels; level++)
      {
         ::glTexImage2D(_target, level,
                        num,
                        _width, _height,
                        0/*border*/,
                        baseFormat,
                        type,
                        NULL/*pixels*/
                        );
         Dtraceglerror("glTexImage2D");
      }
   }
}


// ---------------------------------------------------------------------------- glTexStorage2DMultisample
void YAC_CALL _glTexStorage2DMultisample(sSI _target, sSI _samples, sSI _internalFormat, sUI _width, sUI _height, sBool _fixedsamplelocations) {
   if(NULL != glTexStorage2DMultisample_xref)
   {
      // GL 4.2
      ::glTexStorage2DMultisample_xref(_target, _samples, _internalFormat,
                                       _width, _height,
                                       _fixedsamplelocations
                                       );
      Dtraceglerror("glTexStorage2DMultisample");
   }
   else
   {
      // Fallback GL 3.2
      Dcheckext(glTexImage2DMultisample)
      {
         ::glTexImage2DMultisample_xref(_target, _samples, _internalFormat,
                                        _width, _height,
                                        _fixedsamplelocations
                                        );
         Dtraceglerror("glTexImage2DMultisample");
      }
   }
}


// ---------------------------------------------------------------------------- glTexStorage3D
void YAC_CALL _glTexStorage3D(sSI _target, sSI _levels, sSI _internalFormat, sUI _width, sUI _height, sUI _depth) {
   if(NULL != glTexStorage3D_xref)
   {
      // GL 4.2
      ::glTexStorage3D_xref(_target, _levels, _internalFormat,
                            _width, _height, _depth
                            );
      Dtraceglerror("glTexStorage3D");
   }
   else
   {
      // Fallback GL 2.0
      Dcheckext(glTexImage3D)
      {
         GLint  num;
         GLenum baseFormat;
         GLenum type;
         loc_decompose_internalformat(_internalFormat, num, baseFormat, type);

         for(sSI level = 0; level < _levels; level++)
         {
            ::glTexImage3D_xref(_target, level, num,
                                _width, _height, _depth,
                                0/*border*/,
                                baseFormat,
                                type,
                                NULL/*pixels*/
                                );
            Dtraceglerror("glTexImage3D");
         }
      }
   }
}


// ---------------------------------------------------------------------------- glTexStorage3DMultisample
void YAC_CALL _glTexStorage3DMultisample(sSI _target, sSI _samples, sSI _internalFormat, sUI _width, sUI _height, sUI _depth, sBool _fixedsamplelocations) {
   if(NULL != glTexStorage3DMultisample_xref)
   {
      // GL 4.2
      ::glTexStorage3DMultisample_xref(_target, _samples, _internalFormat,
                                       _width, _height, _depth,
                                       _fixedsamplelocations
                                       );
      Dtraceglerror("glTexStorage3DMultisample");
   }
   else
   {
      // Fallback GL 3.2
      Dcheckext(glTexImage3DMultisample)
      {
         ::glTexImage3DMultisample_xref(_target, _samples, _internalFormat,
                                        _width, _height, _depth,
                                        _fixedsamplelocations
                                        );
         Dtraceglerror("glTexImage3DMultisample");
      }
   }
}


// ---------------------------------------------------------------------------- glTexSubImage2D
void YAC_CALL _glTexSubImage2D(sSI _a, sSI _b, sSI _c, sSI _d, sSI _e, sSI _f, sSI _g, sSI _h, sSI _i) {
   // (todo) check if PBO is bound, otherwise make _i a NULL ptr..!!
   _glTexSubImage2D_int(_a, _b, _c, _d, _e, _f, _g, _h, (const GLvoid *)size_t(_i));
}

// ---------------------------------------------------------------------------- glTexSubImage2D_int
void YAC_CALL _glTexSubImage2D_int(GLenum _target, GLint _level, GLint _xoffset, GLint _yoffset, GLsizei _width, GLsizei _height, GLenum _format, GLenum _type, const GLvoid *_pixels) {
   ::glTexSubImage2D(_target, _level, _xoffset, _yoffset, _width, _height, _format, _type, _pixels);
   Dtraceglerror("glTexSubImage2D");
}

// ---------------------------------------------------------------------------- glTranslatef
void YAC_CALL _glTranslatef(sF32 _x, sF32 _y, sF32 _z) {
#ifdef DX_EMU_MATRIX
   tkopengl_matrix_translate(tkopengl_matrix, _x, _y, _z);
   tkopengl_matrix_flags[tkopengl_matrix_mode] |= TKOPENGL_MATRIX_FLAG_DIRTY;
   tkopengl_matrix_flags[tkopengl_matrix_mode] &= ~TKOPENGL_MATRIX_FLAG_IDENTITY;
#else
#ifdef DX_GLES
   Dstub(glTranslatef);
#else
   Dcompat(glTranslatef)
   {
      ::glTranslatef(_x, _y, _z);
      Dtraceglerror("glTranslatef");
   }
#endif // DX_GLES
#endif // DX_EMU_MATRIX
}

// ---------------------------------------------------------------------------- glVertex2i
void YAC_CALL _glVertex2i(sSI _a, sSI _b) {
#ifdef DX_EMU_IMM
   tkopengl_imm_vertex_cbk((sF32)_a, (sF32)_b, 0.0f, 1.0f);
#else
#ifdef DX_GLES
   Dstub(glVertex2i);
#else
   Dcompat(glVertex2i)
   {
      ::glVertex2i(_a, _b);
      Dtracegldrawerror("glVertex2i");
   }
#endif // DX_GLES
#endif // DX_EMU_IMM
}

// ---------------------------------------------------------------------------- glVertex2f
void YAC_CALL _glVertex2f(sF32 _a, sF32 _b) {
#ifdef DX_EMU_IMM
   tkopengl_imm_vertex_cbk(_a, _b, 0.0f, 1.0f);
#else
#ifdef DX_GLES
   Dstub(glVertex2f);
#else
   Dcompat(glVertex2f)
   {
      ::glVertex2f(_a, _b);
      Dtracegldrawerror("glVertex2f");
   }
#endif // DX_GLES
#endif // DX_EMU_IMM
}

// ---------------------------------------------------------------------------- zglVertex2v
void YAC_CALL _zglVertex2v(YAC_Object *_v) {
   if(YAC_VALID(_v) && _v->yacArrayGetNumElements() >= 2u)
   {
      YAC_Value r1; _v->yacArrayGet(NULL/*context*/, 0u, &r1); r1.typecast(YAC_TYPE_FLOAT);
      YAC_Value r2; _v->yacArrayGet(NULL/*context*/, 1u, &r2); r2.typecast(YAC_TYPE_FLOAT);

#ifdef DX_EMU_IMM
      tkopengl_imm_vertex_cbk(r1.value.float_val, r2.value.float_val, 0.0f, 1.0f);
#else
#ifdef DX_GLES
      Dstub(zglVertex2v);
#else
      Dcompat(zglVertex2v)
      {
         ::glVertex2f(r1.value.float_val, r2.value.float_val);
         Dtracegldrawerror("zglVertex2v");
      }
#endif // DX_GLES
#endif // DX_EMU_IMM
   }
}

// ---------------------------------------------------------------------------- glVertex3f
void YAC_CALL _glVertex3f(sF32 _a, sF32 _b, sF32 _c) {
#ifdef DX_EMU_IMM
   tkopengl_imm_vertex_cbk(_a, _b, _c, 1.0f);
#else
#ifdef DX_GLES
   Dstub(glVertex3f);
#else
   Dcompat(glVertex3f)
   {
      ::glVertex3f(_a, _b, _c);
      Dtracegldrawerror("glVertex3f");
   }
#endif // DX_GLES
#endif // DX_EMU_IMM
}

// ---------------------------------------------------------------------------- zglVertex3v
void YAC_CALL _zglVertex3v(YAC_Object *_v) {
   if(YAC_VALID(_v) && _v->yacArrayGetNumElements() >= 3u)
   {
      YAC_Value r1; _v->yacArrayGet(NULL/*context*/, 0u, &r1); r1.typecast(YAC_TYPE_FLOAT);
      YAC_Value r2; _v->yacArrayGet(NULL/*context*/, 1u, &r2); r2.typecast(YAC_TYPE_FLOAT);
      YAC_Value r3; _v->yacArrayGet(NULL/*context*/, 2u, &r3); r3.typecast(YAC_TYPE_FLOAT);

#ifdef DX_EMU_IMM
      tkopengl_imm_vertex_cbk(r1.value.float_val, r2.value.float_val, r3.value.float_val, 1.0f);
#else
#ifdef DX_GLES
      Dstub(zglVertex3v);
#else
      Dcompat(zglVertex3v)
      {
         ::glVertex3f(r1.value.float_val, r2.value.float_val, r3.value.float_val);
         Dtracegldrawerror("zglVertex3v");
      }
#endif // DX_GLES
#endif // DX_EMU_IMM
   }
}

// ---------------------------------------------------------------------------- glVertex4f
void YAC_CALL _glVertex4f(sF32 _a, sF32 _b, sF32 _c, sF32 _d) {
#ifdef DX_EMU_IMM
   tkopengl_imm_vertex_cbk(_a, _b, _c, _d);
#else
#ifdef DX_GLES
   Dstub(glVertex4f);
#else
   Dcompat(glVertex4f)
   {
      ::glVertex4f(_a, _b, _c, _d);
      Dtracegldrawerror("glVertex4f");
   }
#endif // DX_GLES
#endif // DX_EMU_IMM
}

// ---------------------------------------------------------------------------- zglVertex4v
void YAC_CALL _zglVertex4v(YAC_Object *_v) {
   if(YAC_VALID(_v) && _v->yacArrayGetNumElements() >= 4u)
   {
      YAC_Value r1; _v->yacArrayGet(NULL/*context*/, 0u, &r1); r1.typecast(YAC_TYPE_FLOAT);
      YAC_Value r2; _v->yacArrayGet(NULL/*context*/, 1u, &r2); r2.typecast(YAC_TYPE_FLOAT);
      YAC_Value r3; _v->yacArrayGet(NULL/*context*/, 2u, &r3); r3.typecast(YAC_TYPE_FLOAT);
      YAC_Value r4; _v->yacArrayGet(NULL/*context*/, 3u, &r4); r4.typecast(YAC_TYPE_FLOAT);

#ifdef DX_EMU_IMM
      tkopengl_imm_vertex_cbk(r1.value.float_val, r2.value.float_val, r3.value.float_val, r4.value.float_val);
#else
#ifdef DX_GLES
      Dstub(zglVertex4v);
#else
      Dcompat(zglVertex4v)
      {
         ::glVertex4f(r1.value.float_val, r2.value.float_val, r3.value.float_val, r4.value.float_val);
         Dtracegldrawerror("zglVertex4v");
      }
#endif // DX_GLES
#endif // DX_EMU_IMM
   }
}

// ---------------------------------------------------------------------------- glViewport
void YAC_CALL _glViewport(sSI _x, sSI _y,
                          sSI _w, sSI _h)
{
#ifdef DX_EMU_FIXFXN
   tkopengl_attrib->viewport.x = _x;
   tkopengl_attrib->viewport.y = _y;
   tkopengl_attrib->viewport.w = _w;
   tkopengl_attrib->viewport.h = _h;
#endif // DX_EMU_FIXFXN

   ::glViewport(_x, _y, _w, _h);
   Dtraceglerror("glViewport");
}

// ---------------------------------------------------------------------------- zglInitPerspective
void YAC_CALL _zglInitPerspective(sF32 _aspect, sF32 _fov, sF32 _znear, sF32 _zfar) {
   _glMatrixMode(GL_PROJECTION);
   _glLoadIdentity();
   sF32 top = ::tanf(_fov*((sF32)sM_PI) * 2.0f * (1.0f/360.0f) * 0.5f) * _znear;
   _glFrustum(_aspect*-top, _aspect*top, -top, top, _znear, _zfar);
   _glMatrixMode(GL_MODELVIEW);
}

// ---------------------------------------------------------------------------- zglInitOrtho
void YAC_CALL _zglInitOrtho(sF32 _sx, sF32 _sy) {
   _glMatrixMode(GL_PROJECTION);
   _glLoadIdentity();
   _glOrtho(-_sx/*left*/, _sx/*right*/, -_sy/*bottom*/, _sy/*top*/, -1.0f/*near*/, 1.0f/*far*/);
   _glMatrixMode(GL_MODELVIEW);
}

// ---------------------------------------------------------------------------- zglInit2D
void YAC_CALL _zglInit2D(sF32 _sx, sF32 _sy) {
   _glMatrixMode(GL_PROJECTION);
   _glLoadIdentity();
   _glOrtho(0.0f, _sx, _sy, 0.0f, -1.0f, 1.0f);
   _glMatrixMode(GL_MODELVIEW);
}

// ---------------------------------------------------------------------------- zglFrustum
void YAC_CALL _zglFrustum(sF32 _aspect, sF32 _fov, sF32 _znear, sF32 _zfar) {
   Dcompat(zglFrustum)
   {
      sF32 top=::tanf(_fov * ((sF32)sM_PI) * 2.0f * (1.0f/360.0f) * 0.5f) * _znear;
      _glFrustum(_aspect*-top, _aspect*top, -top, top, _znear, _zfar);
   }
}

//
//
// Lighting
//
//

// ---------------------------------------------------------------------------- glLightfv_int
void YAC_CALL _glLightfv_int(GLenum _light, GLenum _pname, const GLfloat *_params) {
#ifdef DX_EMU_LIGHT
   if(NULL != _params)
   {
      switch(_pname)
      {
         default:
            yac_host->printf("[---] tkopengl::glLightfv_int: invalid pname 0x%04x\n", (sU16)_pname);
            last_gl_error = GL_INVALID_ENUM;
            break;

         case GL_AMBIENT:
            _zglLightAmbient4f(_light, _params[0], _params[1], _params[2], _params[3]);
            break;

         case GL_DIFFUSE:
            _zglLightDiffuse4f(_light, _params[0], _params[1], _params[2], _params[3]);
            break;

         case GL_SPECULAR:
            _zglLightSpecular4f(_light, _params[0], _params[1], _params[2], _params[3]);
            break;

         case GL_POSITION:
            _zglLightPositionf(_light, _params[0], _params[1], _params[2], _params[3]);
            break;

         case GL_SPOT_CUTOFF:
            _zglLightSpotCutOff(_light, _params[0]);
            break;

         case GL_SPOT_DIRECTION:
            _zglLightSpotDirectionf(_light, _params[0], _params[1], _params[2]);
            break;

         case GL_SPOT_EXPONENT:
            _zglLightSpotExponent(_light, _params[0]);
            break;

         case GL_CONSTANT_ATTENUATION:
            _zglLightConstantAttenuation(_light, _params[0]);
            break;

         case GL_LINEAR_ATTENUATION:
            _zglLightLinearAttenuation(_light, _params[0]);
            break;

         case GL_QUADRATIC_ATTENUATION:
            _zglLightQuadraticAttenuation(_light, _params[0]);
            break;
      }
   }
   else
   {
      yac_host->printf("[---] tkopengl::glLightfv_int: params is NULL (pname=0x%04x)\n", (sU16)_pname);
      last_gl_error = GL_INVALID_VALUE;
   }
#else

#ifdef DX_EMU_MATRIX
   tkopengl_matrix_update();
#endif

#ifdef DX_GLES
   Dstub(glLightfv);
#else
   Dcompat(glLightfv)
   {
      ::glLightfv(_light, _pname, _params);
      Dtraceglerror("glLightfv");
   }
#endif // DX_GLES
#endif // DX_EMU_LIGHT
}

// ---------------------------------------------------------------------------- glLightModeli
void YAC_CALL _glLightModeli(sSI _pname, sSI _param) {
#ifdef DX_EMU_LIGHT
   switch(_pname)
   {
      default:
         yac_host->printf("[---] tkopengl::glLightModeli_int: invalid pname 0x%04x\n", (sU16)_pname);
         last_gl_error = GL_INVALID_ENUM;
         break;

      case GL_LIGHT_MODEL_LOCAL_VIEWER:
         _zglLightModelLocalViewer(_param);
         break;

      case GL_LIGHT_MODEL_COLOR_CONTROL:
         _zglLightModelColorControl(_param);
         break;

      case GL_LIGHT_MODEL_TWO_SIDE:
         _zglLightModelTwoSide(_param);
         break;
   }
#else
#ifdef DX_GLES
   Dstub(glLightModeli);
#else
   Dcompat(glLightModeli)
   {
      ::glLightModeli(_pname, _param);
      Dtraceglerror("glLightModeli");
   }
#endif // DX_GLES
#endif // DX_EMU_LIGHT
}

// ---------------------------------------------------------------------------- glMaterialfv_int
void YAC_CALL _glMaterialfv_int(GLenum _face, GLenum _pname, const GLfloat *_params) {
#ifdef DX_EMU_LIGHT
   switch(_face)
   {
      default:
         yac_host->printf("[---] tkopengl::glLightMaterialfv_int: invalid face 0x%04x\n", (sU16)_face);
         last_gl_error = GL_INVALID_ENUM;
         break;

      // case GL_FRONT:
      //    yac_host->printf("[---] tkopengl::glLightMaterialfv_int: invalid face GL_FRONT\n");
      //    last_gl_error = GL_INVALID_ENUM;
      //    break;

      // case GL_BACK:
      //    yac_host->printf("[---] tkopengl::glLightMaterialfv_int: invalid face GL_BACK\n");
      //    last_gl_error = GL_INVALID_ENUM;
      //    break;

         // (note) silently ignore GL_FRONT/GL_BACK face type
      case GL_FRONT:
      case GL_BACK:
      case GL_FRONT_AND_BACK:
         switch(_pname)
         {
            default:
               yac_host->printf("[---] tkopengl::glLightMaterialfv_int: invalid pname 0x%04x\n", (sU16)_pname);
               last_gl_error = GL_INVALID_ENUM;
               break;

            case GL_AMBIENT:
               _zglLightMaterialAmbient4f(_face, _params[0], _params[1], _params[2], _params[3]);
               break;

            case GL_DIFFUSE:
               _zglLightMaterialDiffuse4f(_face, _params[0], _params[1], _params[2], _params[3]);
               break;

            case GL_SPECULAR:
               _zglLightMaterialSpecular4f(_face, _params[0], _params[1], _params[2], _params[3]);
               break;

            case GL_EMISSION:
               _zglLightMaterialEmission4f(_face, _params[0], _params[1], _params[2], _params[3]);
               break;

            case GL_SHININESS:
               _zglLightMaterialShininess(_face, _params[0]);
               break;

            case GL_AMBIENT_AND_DIFFUSE:
               _zglLightMaterialAmbientAndDiffuse4f(_face, _params[0], _params[1], _params[2], _params[3]);
               break;
         }
         break;
   }
#else
#ifdef DX_GLES
   Dstub(glMaterialfv);
#else
   // for tkoldmath, **deprecated**
   Dcompat(glMaterialfv)
   {
      ::glMaterialfv(_face, _pname, _params);
   }
#endif // DX_GLES
#endif // DX_EMU_LIGHT
}

// ---------------------------------------------------------------------------- glColorMaterial
void YAC_CALL _glColorMaterial(sSI _face, sSI _mode) {
#ifdef DX_EMU_LIGHT
   switch(_mode)
   {
      default:
         yac_host->printf("[---] tkopengl::glColorMaterial: unhandled mode 0x%04x\n", (sU16)_mode);
         last_gl_error = GL_INVALID_ENUM;
         break;

      case GL_AMBIENT_AND_DIFFUSE:
         break;
   }
#else
#ifdef DX_GLES
   Dstub(glColorMaterial);
#else
   Dcompat(glColorMaterial)
   {
      ::glColorMaterial(_face, _mode);
      Dtraceglerror("glColorMaterial");
   }
#endif // DX_GLES
#endif // DX_EMU_LIGHT
}

// ---------------------------------------------------------------------------- zglLightAmbient4f
void YAC_CALL _zglLightAmbient4f(sSI _lightnr, sF32 _r, sF32 _g, sF32 _b, sF32 _a) {
#ifdef DX_EMU_LIGHT
   _lightnr -= GL_LIGHT0;

   if(((sUI)_lightnr) < TKOPENGL_MAX_LIGHTS)
   {
      tkopengl_light_t *light = &tkopengl_attrib->light.lights[_lightnr];

      light->ambient_color[0] = _r;
      light->ambient_color[1] = _g;
      light->ambient_color[2] = _b;
      light->ambient_color[3] = _a;
   }
   else
   {
      yac_host->printf("[---] zglLightAmbient4f: invalid lightnr %d (max=%u)\n", _lightnr, TKOPENGL_MAX_LIGHTS);
      last_gl_error = GL_INVALID_ENUM;
   }
#else
#ifdef DX_GLES
   Dstub(zglLightAmbient4f);
#else
   Dcompat(zglLightAmbient4f)
   {
      GLfloat rgba[4];
      rgba[0]=_r;
      rgba[1]=_g;
      rgba[2]=_b;
      rgba[3]=_a;
      ::glLightfv(_lightnr, GL_AMBIENT, rgba);
      Dtraceglerror("zglLightAmbient4f");
   }
#endif // DX_GLES
#endif // DX_EMU_LIGHT
}

// ---------------------------------------------------------------------------- zglLightDiffuse4f
void YAC_CALL _zglLightDiffuse4f(sSI _lightnr, sF32 _r, sF32 _g, sF32 _b, sF32 _a) {
#ifdef DX_EMU_LIGHT
   _lightnr -= GL_LIGHT0;

   if(((sUI)_lightnr) < TKOPENGL_MAX_LIGHTS)
   {
      tkopengl_light_t *light = &tkopengl_attrib->light.lights[_lightnr];

      light->diffuse_color[0] = _r;
      light->diffuse_color[1] = _g;
      light->diffuse_color[2] = _b;
      light->diffuse_color[3] = _a;
   }
   else
   {
      yac_host->printf("[---] zglLightDiffuse4f: invalid lightnr %d (max=%u)\n", _lightnr, TKOPENGL_MAX_LIGHTS);
      last_gl_error = GL_INVALID_ENUM;
   }
#else
#ifdef DX_GLES
   Dstub(zglLightDiffuse4f);
#else
   Dcompat(zglLightDiffuse4f)
   {
      GLfloat rgba[4];
      rgba[0]=_r;
      rgba[1]=_g;
      rgba[2]=_b;
      rgba[3]=_a;
      ::glLightfv(_lightnr, GL_DIFFUSE, rgba);
      Dtraceglerror("zglLightDiffuse4f");
   }
#endif // DX_GLES
#endif // DX_EMU_LIGHT
}

// ---------------------------------------------------------------------------- zglLightSpecular4f
void YAC_CALL _zglLightSpecular4f(sSI _lightnr, sF32 _r, sF32 _g, sF32 _b, sF32 _a) {
#ifdef DX_EMU_LIGHT
   _lightnr -= GL_LIGHT0;

   if(((sUI)_lightnr) < TKOPENGL_MAX_LIGHTS)
   {
      tkopengl_light_t *light = &tkopengl_attrib->light.lights[_lightnr];

      light->specular_color[0] = _r;
      light->specular_color[1] = _g;
      light->specular_color[2] = _b;
      light->specular_color[3] = _a;
   }
   else
   {
      yac_host->printf("[---] zglLightSpecular4f: invalid lightnr %d (max=%u)\n", _lightnr, TKOPENGL_MAX_LIGHTS);
      last_gl_error = GL_INVALID_ENUM;
   }
#else
#ifdef DX_GLES
   Dstub(zglLightSpecular4f);
#else
   Dcompat(zglLightSpecular4f)
   {
      GLfloat rgba[4];
      rgba[0]=_r;
      rgba[1]=_g;
      rgba[2]=_b;
      rgba[3]=_a;
      ::glLightfv(_lightnr, GL_SPECULAR, rgba);
      Dtraceglerror("zglLightSpecular4f");
   }
#endif // DX_GLES
#endif // DX_EMU_LIGHT
}

// ---------------------------------------------------------------------------- zglLightPositionf
void YAC_CALL _zglLightPositionf(sSI _lightnr, sF32 _x, sF32 _y, sF32 _z, sF32 _w) {
#ifdef DX_EMU_LIGHT
   _lightnr -= GL_LIGHT0;

   if(((sUI)_lightnr) < TKOPENGL_MAX_LIGHTS)
   {
      tkopengl_light_t *light = &tkopengl_attrib->light.lights[_lightnr];

      light->position[0] = _x;
      light->position[1] = _y;
      light->position[2] = _z;
      light->position[3] = _w;

      tkopengl_matrix_transform_vec4(light->position_transform,
                                     &tkopengl_modelview_matrix_stack[tkopengl_modelview_matrix_stacki << 4],
                                     light->position
                                     );
   }
   else
   {
      yac_host->printf("[---] zglLightPositionf: invalid lightnr %d (max=%u)\n", _lightnr, TKOPENGL_MAX_LIGHTS);
      last_gl_error = GL_INVALID_ENUM;
   }
#else

#ifdef DX_EMU_MATRIX
   tkopengl_matrix_update();
#endif

#ifdef DX_GLES
   Dstub(zglLightPositionf);
#else
   Dcompat(zglLightPositionf)
   {
      GLfloat xyz[4]={_x, _y, _z, _w};
      ::glLightfv(_lightnr, GL_POSITION, xyz);
      Dtraceglerror("zglLightPositionf");
   }
#endif // DX_GLES

#endif // DX_EMU_LIGHT
}

// ---------------------------------------------------------------------------- zglLightSpotDirectionf
void YAC_CALL _zglLightSpotDirectionf(sSI _lightnr, sF32 _x, sF32 _y, sF32 _z) {
#ifdef DX_EMU_LIGHT
   _lightnr -= GL_LIGHT0;

   if(((sUI)_lightnr) < TKOPENGL_MAX_LIGHTS)
   {
      tkopengl_light_t *light = &tkopengl_attrib->light.lights[_lightnr];

      light->spot_direction[0] = _x;
      light->spot_direction[1] = _y;
      light->spot_direction[2] = _z;
   }
   else
   {
      yac_host->printf("[---] zglLightSpotDirectionf: invalid lightnr %d (max=%u)\n", _lightnr, TKOPENGL_MAX_LIGHTS);
      last_gl_error = GL_INVALID_ENUM;
   }
#else

#ifdef DX_EMU_MATRIX
   tkopengl_matrix_update();
#endif

#ifdef DX_GLES
   Dstub(zglLightSpotDirectionf);
#else
   Dcompat(zglLightSpotDirection4f)
   {
      sF32 xyz[3]={_x, _y, _z};
      ::glLightfv(_lightnr, GL_SPOT_DIRECTION, xyz);
      Dtraceglerror("zglLightSpotDirection4f");
   }
#endif // DX_GLES

#endif // DX_EMU_LIGHT
}

// ---------------------------------------------------------------------------- zglLightSpotExponent
void YAC_CALL _zglLightSpotExponent(sSI _lightnr, sF32 _exponent) {
#ifdef DX_EMU_LIGHT
   _lightnr -= GL_LIGHT0;

   if(((sUI)_lightnr) < TKOPENGL_MAX_LIGHTS)
   {
      tkopengl_light_t *light = &tkopengl_attrib->light.lights[_lightnr];

      if(_exponent < 0.0f)
         _exponent = 0.0f;
      else if(_exponent >= 127.0f)
         _exponent = 127.0f;

      light->spot_exponent = _exponent;
   }
   else
   {
      yac_host->printf("[---] zglLightSpotExponent: invalid lightnr %d (max=%u)\n", _lightnr, TKOPENGL_MAX_LIGHTS);
      last_gl_error = GL_INVALID_ENUM;
   }
#else
#ifdef DX_GLES
   Dstub(zglLightSpotExponent);
#else
   Dcompat(zglLightSpotExponent)
   {
      ::glLightf(_lightnr, GL_SPOT_EXPONENT, _exponent);
      Dtraceglerror("zglLightSpotExponent");
   }
#endif // DX_GLES
#endif // DX_EMU_LIGHT
}

// ---------------------------------------------------------------------------- zglLightSpotCutOff
void YAC_CALL _zglLightSpotCutOff(sSI _lightnr, sF32 _cutoff) {
#ifdef DX_EMU_LIGHT
   _lightnr -= GL_LIGHT0;

   if(((sUI)_lightnr) < TKOPENGL_MAX_LIGHTS)
   {
      tkopengl_light_t *light = &tkopengl_attrib->light.lights[_lightnr];

      if(_cutoff < 0.0f)
         _cutoff = 0.0f;
      else if(_cutoff > 90.0f)
         _cutoff = 180.0f;

      light->spot_cutoff = _cutoff;
   }
   else
   {
      yac_host->printf("[---] zglLightSpotCutOff: invalid lightnr %d (max=%u)\n", _lightnr, TKOPENGL_MAX_LIGHTS);
      last_gl_error = GL_INVALID_ENUM;
   }
#else
#ifdef DX_GLES
   Dstub(zglLightSpotCutOff);
#else
   Dcompat(zglLightSpotCutOff)
   {
      ::glLightf(_lightnr, GL_SPOT_CUTOFF, _cutoff);
      Dtraceglerror("zglLightSpotCutOff");
   }
#endif // DX_GLES
#endif // DX_EMU_LIGHT
}

// ---------------------------------------------------------------------------- zglLightConstantAttenuation
void YAC_CALL _zglLightConstantAttenuation(sSI _lightnr, sF32 _v) {
#ifdef DX_EMU_LIGHT
   _lightnr -= GL_LIGHT0;

   if(((sUI)_lightnr) < TKOPENGL_MAX_LIGHTS)
   {
      tkopengl_light_t *light = &tkopengl_attrib->light.lights[_lightnr];

      light->constant_attenuation = _v;
   }
   else
   {
      yac_host->printf("[---] zglLightConstantAttenuation: invalid lightnr %d (max=%u)\n", _lightnr, TKOPENGL_MAX_LIGHTS);
      last_gl_error = GL_INVALID_ENUM;
   }
#else
#ifdef DX_GLES
   Dstub(zglLightConstantAttenuation);
#else
   Dcompat(zglLightConstantAttenuation)
   {
      ::glLightf(_lightnr, GL_CONSTANT_ATTENUATION, _v);
      Dtraceglerror("zglLightConstantAttenuation");
   }
#endif // DX_GLES
#endif // DX_EMU_LIGHT
}

// ---------------------------------------------------------------------------- zglLightLinearAttenuation
void YAC_CALL _zglLightLinearAttenuation(sSI _lightnr, sF32 _v) {
#ifdef DX_EMU_LIGHT
   _lightnr -= GL_LIGHT0;

   if(((sUI)_lightnr) < TKOPENGL_MAX_LIGHTS)
   {
      tkopengl_light_t *light = &tkopengl_attrib->light.lights[_lightnr];

      light->linear_attenuation = _v;
   }
   else
   {
      yac_host->printf("[---] zglLightLinearAttenuation: invalid lightnr %d (max=%u)\n", _lightnr, TKOPENGL_MAX_LIGHTS);
      last_gl_error = GL_INVALID_ENUM;
   }
#else
#ifdef DX_GLES
   Dstub(zglLightLinearAttenuation);
#else
   Dcompat(zglLightLinearAttenuation)
   {
      ::glLightf(_lightnr, GL_LINEAR_ATTENUATION, _v);
      Dtraceglerror("zglLightLinearAttenuation");
   }
#endif // DX_GLES
#endif // DX_EMU_LIGHT
}

// ---------------------------------------------------------------------------- zglLightQuadraticAttenuation
void YAC_CALL _zglLightQuadraticAttenuation(sSI _lightnr, sF32 _v) {
#ifdef DX_EMU_LIGHT
   _lightnr -= GL_LIGHT0;

   if(((sUI)_lightnr) < TKOPENGL_MAX_LIGHTS)
   {
      tkopengl_light_t *light = &tkopengl_attrib->light.lights[_lightnr];

      light->quadratic_attenuation = _v;
   }
   else
   {
      yac_host->printf("[---] zglLightQuadraticAttenuation: invalid lightnr %d (max=%u)\n", _lightnr, TKOPENGL_MAX_LIGHTS);
      last_gl_error = GL_INVALID_ENUM;
   }
#else
#ifdef DX_GLES
   Dstub(zglLightQuadraticAttenuation);
#else
   Dcompat(zglLightQuadraticAttenuation)
   {
      ::glLightf(_lightnr, GL_QUADRATIC_ATTENUATION, _v);
      Dtraceglerror("zglLightQuadraticAttenuation");
   }
#endif // DX_GLES
#endif // DX_EMU_LIGHT
}

// ---------------------------------------------------------------------------- zglLightModelAmbient4f
void YAC_CALL _zglLightModelAmbient4f(sF32 _r, sF32 _g, sF32 _b, sF32 _a) {
#ifdef DX_EMU_LIGHT
   tkopengl_attrib->light.model_ambient[0] = _r;
   tkopengl_attrib->light.model_ambient[1] = _g;
   tkopengl_attrib->light.model_ambient[2] = _b;
   tkopengl_attrib->light.model_ambient[3] = _a;
#else
#ifdef DX_GLES
   Dstub(zglLightModelAmbient4f);
#else
   Dcompat(zglLightModelAmbient4f)
   {
      GLfloat rgba[4];
      rgba[0]=_r;
      rgba[1]=_g;
      rgba[2]=_b;
      rgba[3]=_a;
      ::glLightModelfv(GL_LIGHT_MODEL_AMBIENT,  rgba);
      Dtraceglerror("zglLightModelAmbient4f");
   }
#endif // DX_GLES
#endif // DX_EMU_LIGHT
}

// ---------------------------------------------------------------------------- zglLightModelLocalViewer
void YAC_CALL _zglLightModelLocalViewer(sSI _eyeorigin) {
#ifdef DX_EMU_LIGHT
   tkopengl_attrib->light.b_local_viewer = (_eyeorigin ? YAC_TRUE : YAC_FALSE);
#else
#ifdef DX_GLES
   Dstub(zglLightModelLocalViewer);
#else
   Dcompat(zglLightModelLocalViewer)
   {
      ::glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,  _eyeorigin);
      Dtraceglerror("zglLightModelLocalViewer");
   }
#endif // DX_GLES
#endif // DX_EMU_LIGHT
}

// ---------------------------------------------------------------------------- zglLightModelTwoSide
void YAC_CALL _zglLightModelTwoSide(sSI _enabled) {
#ifdef DX_EMU_LIGHT
   if(0 != _enabled)
   {
      yac_host->printf("[~~~] tkopengl::zglLightModelTwoSide: not supported.\n");
   }
   else
   {
   }
#else
#ifdef DX_GLES
   Dstub(zglLightModelTwoSide);
#else
   Dcompat(zglLightModelTwoSide)
   {
      ::glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,  _enabled);
      Dtraceglerror("zglLightModelTwoSide");
   }
#endif // DX_GLES
#endif // DX_EMU_LIGHT
}

// ---------------------------------------------------------------------------- zglLightModelColorControl
void YAC_CALL _zglLightModelColorControl(sSI _mode) {
#ifdef DX_EMU_LIGHT
   switch(_mode)
   {
   default:
      yac_host->printf("[---] tkopengl::zglLightModelColorControl: unhandled mode 0x%04x\n", (sU16)_mode);
      last_gl_error = GL_INVALID_ENUM;
      break;

   case GL_SINGLE_COLOR:
   case GL_SEPARATE_SPECULAR_COLOR:
      tkopengl_attrib->light.color_control = _mode;
      break;
   }
#else
#ifdef DX_GLES
   Dstub(zglLightModelColorControl);
#else
   Dcompat(zglLightModelColorControl)
   {
      ::glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL,  _mode);
      Dtraceglerror("zglLightModelColorControl");
   }
#endif // DX_GLES
#endif // DX_EMU_LIGHT
}

// ---------------------------------------------------------------------------- zglLightMaterialAmbient4f
void YAC_CALL _zglLightMaterialAmbient4f(sSI _face, sF32 _r, sF32 _g, sF32 _b, sF32 _a) {
#ifdef DX_EMU_LIGHT
   (void)_face;
   tkopengl_attrib->light.material.ambient_color[0] = _r;
   tkopengl_attrib->light.material.ambient_color[1] = _g;
   tkopengl_attrib->light.material.ambient_color[2] = _b;
   tkopengl_attrib->light.material.ambient_color[3] = _a;
#else
#ifdef DX_GLES
   Dstub(zglLightMaterialAmbient4f);
#else
   Dcompat(zglLightMaterialAmbient4f)
   {
      GLfloat rgba[4];
      rgba[0] = _r;
      rgba[1] = _g;
      rgba[2] = _b;
      rgba[3] = _a;
      ::glMaterialfv(_face, GL_AMBIENT, rgba);
      Dtracegldrawerror("zglLightMaterialAmbient4f");
   }
#endif // DX_GLES
#endif // DX_EMU_LIGHT
}

// ---------------------------------------------------------------------------- zglLightMaterialDiffuse4f
void YAC_CALL _zglLightMaterialDiffuse4f(sSI _face, sF32 _r, sF32 _g, sF32 _b, sF32 _a) {
#ifdef DX_EMU_LIGHT
   (void)_face;
   tkopengl_attrib->light.material.diffuse_color[0] = _r;
   tkopengl_attrib->light.material.diffuse_color[1] = _g;
   tkopengl_attrib->light.material.diffuse_color[2] = _b;
   tkopengl_attrib->light.material.diffuse_color[3] = _a;
#else
#ifdef DX_GLES
   Dstub(zglLightMaterialDiffuse4f);
#else
   Dcompat(zglLightMaterialDiffuse4f)
   {
      GLfloat rgba[4];
      rgba[0] = _r;
      rgba[1] = _g;
      rgba[2] = _b;
      rgba[3] = _a;
      ::glMaterialfv(_face,GL_DIFFUSE, rgba);
      Dtracegldrawerror("zglLightMaterialDiffuse4f");
   }
#endif // DX_GLES
#endif // DX_EMU_LIGHT
}

// ---------------------------------------------------------------------------- zglLightMaterialSpecular4f
void YAC_CALL _zglLightMaterialSpecular4f(sSI _face, sF32 _r, sF32 _g, sF32 _b, sF32 _a) {
#ifdef DX_EMU_LIGHT
   (void)_face;
   tkopengl_attrib->light.material.specular_color[0] = _r;
   tkopengl_attrib->light.material.specular_color[1] = _g;
   tkopengl_attrib->light.material.specular_color[2] = _b;
   tkopengl_attrib->light.material.specular_color[3] = _a;
#else
#ifdef DX_GLES
   Dstub(zglLightMaterialSpecular4f);
#else
   Dcompat(zglLightMaterialSpecular4f)
   {
      GLfloat rgba[4];
      rgba[0] = _r;
      rgba[1] = _g;
      rgba[2] = _b;
      rgba[3] = _a;
      ::glMaterialfv(_face, GL_SPECULAR, rgba);
      Dtracegldrawerror("zglLightMaterialSpecular4f");
   }
#endif // DX_GLES
#endif // DX_EMU_LIGHT
}

// ---------------------------------------------------------------------------- zglLightMaterialEmission4f
void YAC_CALL _zglLightMaterialEmission4f(sSI _face, sF32 _r, sF32 _g, sF32 _b, sF32 _a) {
#ifdef DX_EMU_LIGHT
   (void)_face;
   tkopengl_attrib->light.material.emission_color[0] = _r;
   tkopengl_attrib->light.material.emission_color[1] = _g;
   tkopengl_attrib->light.material.emission_color[2] = _b;
   tkopengl_attrib->light.material.emission_color[3] = _a;
#else
#ifdef DX_GLES
   Dstub(zglLightMaterialEmission4f);
#else
   Dcompat(zglLightMaterialEmission4f)
   {
      GLfloat rgba[4];
      rgba[0] = _r;
      rgba[1] = _g;
      rgba[2] = _b;
      rgba[3] = _a;
      ::glMaterialfv(_face, GL_EMISSION, rgba);
      Dtracegldrawerror("zglLightMaterialEmission4f");
   }
#endif // DX_GLES
#endif // DX_EMU_LIGHT
}

// ---------------------------------------------------------------------------- zglLightMaterialShininess
void YAC_CALL _zglLightMaterialShininess(sSI _face, sF32 _exp) {
#ifdef DX_EMU_LIGHT
   (void)_face;
   tkopengl_attrib->light.material.specular_exponent = _exp; // shininess
#else
#ifdef DX_GLES
   Dstub(zglLightMaterialShininess);
#else
   Dcompat(zglLightMaterialShininess)
   {
      ::glMaterialf(_face, GL_SHININESS, _exp);
      Dtracegldrawerror("zglLightMaterialShininess");
   }
#endif // DX_GLES
#endif // DX_EMU_LIGHT
}

// ---------------------------------------------------------------------------- zglLightMaterialAmbientAndDiffuse4f
void YAC_CALL _zglLightMaterialAmbientAndDiffuse4f(sSI _face, sF32 _r, sF32 _g, sF32 _b, sF32 _a) {
#ifdef DX_EMU_LIGHT
   (void)_face;
   tkopengl_attrib->light.material.ambient_color[0] = _r;
   tkopengl_attrib->light.material.ambient_color[1] = _g;
   tkopengl_attrib->light.material.ambient_color[2] = _b;
   tkopengl_attrib->light.material.ambient_color[3] = _a;

   tkopengl_attrib->light.material.diffuse_color[0] = _r;
   tkopengl_attrib->light.material.diffuse_color[1] = _g;
   tkopengl_attrib->light.material.diffuse_color[2] = _b;
   tkopengl_attrib->light.material.diffuse_color[3] = _a;
#else
#ifdef DX_GLES
   Dstub(zglLightMaterialAmbientAndDiffuse4f);
#else
   Dcompat(zglLightMaterialAmbientAndDiffuse4f)
   {
      GLfloat rgba[4];
      rgba[0] = _r;
      rgba[1] = _g;
      rgba[2] = _b;
      rgba[3] = _a;
      ::glMaterialfv(_face, GL_AMBIENT_AND_DIFFUSE, rgba);
      Dtracegldrawerror("zglLightMaterialAmbientAndDiffuse4f");
   }
#endif // DX_GLES
#endif // DX_EMU_LIGHT
}

// ---------------------------------------------------------------------------- zglFogMode
void YAC_CALL _zglFogMode(sSI _mode) {
#ifdef DX_EMU_FOG
   switch(_mode)
   {
      default:
         yac_host->printf("[---] tkopengl::zglFogMode: unhandled mode 0x%04x\n", (sU16)_mode);
         last_gl_error = GL_INVALID_ENUM;
         break;

      case GL_LINEAR:
      case GL_EXP:
      case GL_EXP2:
         tkopengl_attrib->fog.mode = _mode;
         break;
   }
#else
#ifdef DX_GLES
   Dstub(zglFogMode);
#else
   Dcompat(zglFogMode)
   {
      ::glFogi(GL_FOG_MODE, _mode);
      Dtraceglerror("zglFogMode");
   }
#endif // DX_GLES
#endif // DX_EMU_FOG
}

// ---------------------------------------------------------------------------- zglFogAlphaMode
void YAC_CALL _zglFogAlphaMode(sSI _mode) {
#ifdef DX_EMU_FOG
   switch(_mode)
   {
      default:
         yac_host->printf("[---] tkopengl::zglFogAlphaMode: invalid mode 0x%04x\n", (sU16)_mode);
         break;

      case GL_REPLACE:
      case GL_BLEND:
      case GL_MULT:
         tkopengl_attrib->fog.alpha_mode = (GLenum)_mode;
         break;
   }
#else
   (void)_mode; // (note) feature not available in regular OpenGL => silently ignore
#endif // DX_EMU_FOG
}

// ---------------------------------------------------------------------------- zglFogDensity
void YAC_CALL _zglFogDensity(sF32 _d) {
#ifdef DX_EMU_FOG
   tkopengl_attrib->fog.density = _d;
#else
#ifdef DX_GLES
   Dstub(zglFogDensity);
#else
   Dcompat(zglFogDensity)
   {
      ::glFogf(GL_FOG_DENSITY, _d);
      Dtraceglerror("zglFogDensity");
   }
#endif // DX_GLES
#endif // DX_EMU_FOG
}

// ---------------------------------------------------------------------------- zglFogStart
void YAC_CALL _zglFogStart(sF32 _znear) {
#ifdef DX_EMU_FOG
   tkopengl_attrib->fog.start = _znear;
#else
#ifdef DX_GLES
   Dstub(zglFogStart);
#else
   Dcompat(zglFogStart)
   {
      ::glFogf(GL_FOG_START, _znear);
      Dtraceglerror("zglFogStart");
   }
#endif // DX_GLES
#endif // DX_EM
}

// ---------------------------------------------------------------------------- zglFogEnd
void YAC_CALL _zglFogEnd(sF32 _zfar) {
#ifdef DX_EMU_FOG
   tkopengl_attrib->fog.end = _zfar;
#else
#ifdef DX_GLES
   Dstub(zglFogEnd);
#else
   Dcompat(zglFogEnd)
   {
      ::glFogf(GL_FOG_END, _zfar);
      Dtraceglerror("zglFogEnd");
   }
#endif // DX_GLES
#endif // DX_EMU_FOG
}

// ---------------------------------------------------------------------------- zglFogColor4f
void YAC_CALL _zglFogColor4f(sF32 _r, sF32 _g, sF32 _b, sF32 _a) {
#ifdef DX_EMU_FOG
   tkopengl_attrib->fog.color[0] = _r;
   tkopengl_attrib->fog.color[1] = _g;
   tkopengl_attrib->fog.color[2] = _b;
   tkopengl_attrib->fog.color[3] = _a;
#else
#ifdef DX_GLES
   Dstub(zglFogColor4f);
#else
   Dcompat(zglFogColor4f)
   {
      GLfloat rgba[4];
      rgba[0] = _r;
      rgba[1] = _g;
      rgba[2] = _b;
      rgba[3] = _a;
      ::glFogfv(GL_FOG_COLOR, rgba);
      Dtraceglerror("zglFogColor4f");
   }
#endif // DX_GLES
#endif // DX_EMU_FOG
}

// ---------------------------------------------------------------------------- zglColorARGB
void YAC_CALL _zglColorARGB(sUI _argb) {
#ifdef DX_EMU_IMM
   sF32 r = ((sU8)(_argb>>16)) * (1.0f / 255.0f);
   sF32 g = ((sU8)(_argb>>8 )) * (1.0f / 255.0f);
   sF32 b = ((sU8)(_argb    )) * (1.0f / 255.0f);
   sF32 a = ((sU8)(_argb>>24)) * (1.0f / 255.0f);
   tkopengl_imm_color_cbk(r, g, b, a);
#else
#ifdef DX_GLES
   Dstub(zglColorARGB);
#else
   Dcompat(zglColorARGB)
   {
      sU8 rgba[4];
      rgba[0]=(sU8) (_argb>>16);
      rgba[1]=(sU8) (_argb>>8 );
      rgba[2]=(sU8) (_argb    );
      rgba[3]=(sU8) (_argb>>24);
      ::glColor4ubv(rgba);
      Dtracegldrawerror("zglColorARGB");
   }
#endif // DX_GLES
#endif // DX_EMU_IMM
}

// ---------------------------------------------------------------------------- zglVertexPointer
void YAC_CALL _zglVertexPointer(YAC_Object *_fa) {
	if(NULL != _fa)
   {
		if(YAC_TYPE_FLOAT == _fa->yacArrayGetElementType())
		{
			void *e = _fa->yacArrayGetPointer();

			if(NULL != e)
         {
#ifdef DX_EMU_FIXFXN
            tkopengl_userarray_t *arr = &tkopengl_attrib->userarray[TKOPENGL_USERARRAY_VERTEX];
            arr->type      = GL_FLOAT;
            arr->num_comp  = 3;
            arr->addr.pany = e;
#else
#ifdef DX_GLES
            Dstub(glVertexPointer);
#else
            Dcompat(zglVertexPointer)
            {
               ::glVertexPointer(3, GL_FLOAT, 0, e);
               Dtraceglerror("zglVertexPointer");
            }
#endif // DX_GLES
#endif // DX_EMU_FIXFXN
         }
		}
   }
}

// ---------------------------------------------------------------------------- zglVertexPointer2f
void YAC_CALL _zglVertexPointer2f(YAC_Object *_fa) {
	if(NULL != _fa)
   {
		if(YAC_TYPE_FLOAT == _fa->yacArrayGetElementType())
		{
			void *e = _fa->yacArrayGetPointer();

			if(NULL != e)
         {
#ifdef DX_EMU_FIXFXN
            tkopengl_userarray_t *arr = &tkopengl_attrib->userarray[TKOPENGL_USERARRAY_VERTEX];
            arr->type      = GL_FLOAT;
            arr->num_comp  = 2;
            arr->addr.pany = e;
#else
#ifdef DX_GLES
            Dstub(zglVertexPointer2f);
#else
            Dcompat(zglVertexPointer2f)
            {
               ::glVertexPointer(2, GL_FLOAT, 0, e);
               Dtraceglerror("zglVertexPointer2f");
            }
#endif // DX_GLES
#endif // DX_EMU_FIXFXN
         }
		}
   }
}

// ---------------------------------------------------------------------------- zglNormalPointer
void YAC_CALL _zglNormalPointer(YAC_Object *_fa) {
	if(NULL != _fa)
   {
		if(YAC_TYPE_FLOAT == _fa->yacArrayGetElementType())
		{
			void *e = _fa->yacArrayGetPointer();

			if(NULL != e)
         {
#ifdef DX_EMU_FIXFXN
            tkopengl_userarray_t *arr = &tkopengl_attrib->userarray[TKOPENGL_USERARRAY_NORMAL];
            arr->type      = GL_FLOAT;
            arr->num_comp  = 3;
            arr->addr.pany = e;
#else
#ifdef DX_GLES
            Dstub(zglNormalPointer);
#else
            Dcompat(zglNormalPointer)
            {
               ::glNormalPointer(GL_FLOAT, 0, e);
               Dtraceglerror("zglNormalPointer");
            }
#endif // DX_GLES
#endif // DX_EMU_FIXFXN
         }
		}
   }
}

// ---------------------------------------------------------------------------- zglColorPointer3f
void YAC_CALL _zglColorPointer3f(YAC_Object *_fa) {
	if(NULL != _fa)
   {
		if(YAC_TYPE_FLOAT == _fa->yacArrayGetElementType())
		{
			void *e = _fa->yacArrayGetPointer();

			if(NULL != e)
         {
#ifdef DX_EMU_FIXFXN
            tkopengl_userarray_t *arr = &tkopengl_attrib->userarray[TKOPENGL_USERARRAY_COLOR];
            arr->type      = GL_FLOAT;
            arr->num_comp  = 3;
            arr->addr.pany = e;
#else
#ifdef DX_GLES
            Dstub(zglColorPointer3f);
#else
            Dcompat(zglColorPointer3f)
            {
               ::glColorPointer(3, GL_FLOAT, 0, e);
               Dtraceglerror("zglColorPointer3f");
            }
#endif // DX_GLES
#endif // DX_EMU_FIXFXN
         }
		}
   }
}

// ---------------------------------------------------------------------------- zglColorPointer4f
void YAC_CALL _zglColorPointer4f(YAC_Object *_fa) {
	if(NULL != _fa)
   {
		if(YAC_TYPE_FLOAT == _fa->yacArrayGetElementType())
		{
			void *e = _fa->yacArrayGetPointer();

			if(NULL != e)
         {
#ifdef DX_EMU_FIXFXN
            tkopengl_userarray_t *arr = &tkopengl_attrib->userarray[TKOPENGL_USERARRAY_COLOR];
            arr->type      = GL_FLOAT;
            arr->num_comp  = 4;
            arr->addr.pany = e;
#else
#ifdef DX_GLES
            Dstub(zglColorPointer4f);
#else
            Dcompat(zglColorPointer4f)
            {
               ::glColorPointer(4, GL_FLOAT, 0, e);
               Dtraceglerror("zglColorPointer4f");
            }
#endif // DX_GLES
#endif // DX_EMU_FIXFXN
         }
		}
   }
}

// ---------------------------------------------------------------------------- zglColorPointer4b
void YAC_CALL _zglColorPointer4b(YAC_Object *_ia) {
   // (note) should've been called '4ub'..
   // (note) [27Jul2023] broken in macOS 13.5 (red is always 0)
	if(NULL != _ia)
   {
		if(YAC_TYPE_INT == _ia->yacArrayGetElementType())
		{
			void *e = _ia->yacArrayGetPointer();

			if(NULL != e)
         {
#ifdef DX_EMU_FIXFXN
            tkopengl_userarray_t *arr = &tkopengl_attrib->userarray[TKOPENGL_USERARRAY_COLOR];
            arr->type      = GL_UNSIGNED_BYTE;
            arr->num_comp  = 4;
            arr->addr.pany = e;
#else
#ifdef DX_GLES
            Dstub(zglColorPointer4b);
#else
            Dcompat(zglColorPointer4b)
            {
               ::glColorPointer(4, GL_UNSIGNED_BYTE, 0, e);
               Dtraceglerror("zglColorPointer4b");
            }
#endif // DX_GLES
#endif // DX_EMU_FIXFXN
         }
		}
   }
}

// ---------------------------------------------------------------------------- zglTexCoordPointer2f
void YAC_CALL _zglTexCoordPointer2f(YAC_Object *_fa) {
	if(NULL != _fa)
   {
		if(YAC_TYPE_FLOAT == _fa->yacArrayGetElementType())
		{
			void *e = _fa->yacArrayGetPointer();

			if(NULL != e)
         {
#ifdef DX_EMU_FIXFXN
            tkopengl_userarray_t *arr = &tkopengl_attrib->userarray[TKOPENGL_USERARRAY_TEXCOORD];
            arr->type      = GL_FLOAT;
            arr->num_comp  = 2;
            arr->addr.pany = e;
#else
#ifdef DX_GLES
            Dstub(zglTexCoordPointer2f);
#else
            Dcompat(zglTexCoordPointer2f)
            {
               ::glTexCoordPointer(2, GL_FLOAT, 0, e);
               Dtraceglerror("zglTexCoordPointer2f");
            }
#endif // DX_GLES
#endif // DX_EMU_FIXFXN
         }
		}
   }
}

// ---------------------------------------------------------------------------- zglTexCoordPointer1f
void YAC_CALL _zglTexCoordPointer1f(YAC_Object *_fa) {
	if(NULL != _fa)
   {
		if(YAC_TYPE_FLOAT == _fa->yacArrayGetElementType())
		{
			void *e = _fa->yacArrayGetPointer();

			if(NULL != e)
         {
#ifdef DX_EMU_FIXFXN
            tkopengl_userarray_t *arr = &tkopengl_attrib->userarray[TKOPENGL_USERARRAY_TEXCOORD];
            arr->type      = GL_FLOAT;
            arr->num_comp  = 1;
            arr->addr.pany = e;
#else
#ifdef DX_GLES
            Dstub(zglTexCoordPointer1f);
#else
            Dcompat(zglTexCoordPointer1f)
            {
               ::glTexCoordPointer(1, GL_FLOAT, 0, e);
               Dtraceglerror("zglTexCoordPointer1f");
            }
#endif // DX_GLES
#endif // DX_EMU_FIXFXN
         }
		}
   }
}

// ---------------------------------------------------------------------------- glLoadMatrixf
void YAC_CALL _glLoadMatrixf(YAC_Object *_fa) {
   if(YAC_VALID(_fa))
   {
      if(_fa->yacArrayGetNumElements() >= 16)
      {
#ifdef DX_EMU_MATRIX
         sUI i;
         for(i=0; i<16; i++)
         {
            YAC_Value v;
            _fa->yacArrayGet(NULL, i, &v);
            v.typecast(YAC_TYPE_FLOAT);
            tkopengl_matrix[i] = v.value.float_val;
         }
         tkopengl_matrix_flags[tkopengl_matrix_mode] |= TKOPENGL_MATRIX_FLAG_DIRTY;
         tkopengl_matrix_flags[tkopengl_matrix_mode] &= ~(TKOPENGL_MATRIX_FLAG_ORTHOGONAL | TKOPENGL_MATRIX_FLAG_IDENTITY);
#else
#ifdef DX_GLES
         Dstub(glLoadMatrixf);
#else
         Dcompat(glLoadMatrixf)
         {
            sF32 fa[16];
            sUI i;
            for(i=0; i<16; i++)
            {
               YAC_Value v;
               _fa->yacArrayGet(NULL, i, &v);
               v.typecast(YAC_TYPE_FLOAT);
               fa[i] = v.value.float_val;
            }
            ::glLoadMatrixf(fa);
            Dtraceglerror("glLoadMatrixf");
         }
#endif // DX_GLES
#endif // DX_EMU_MATRIX
      }
   }
}


// ---------------------------------------------------------------------------- zglLoadMatrixTransposef
void YAC_CALL _zglLoadMatrixTransposef(YAC_Object *_fa) {
   if(YAC_VALID(_fa))
   {
      if(_fa->yacArrayGetNumElements() >= 16)
      {
#ifdef DX_EMU_MATRIX
         sUI j = 0u;
         for(sUI rowIdx = 0u; rowIdx < 4u; rowIdx++)
         {
            for(sUI colIdx = 0u; colIdx < 4u; colIdx++)
            {
               YAC_Value v;
               _fa->yacArrayGet(NULL, j++, &v);
               v.typecast(YAC_TYPE_FLOAT);
               tkopengl_matrix[(colIdx<<2)+rowIdx] = v.value.float_val;
            }
         }
         tkopengl_matrix_flags[tkopengl_matrix_mode] |= TKOPENGL_MATRIX_FLAG_DIRTY;
         tkopengl_matrix_flags[tkopengl_matrix_mode] &= ~(TKOPENGL_MATRIX_FLAG_ORTHOGONAL | TKOPENGL_MATRIX_FLAG_IDENTITY);
#else
#ifdef DX_GLES
         Dstub(glLoadMatrixf);
#else
         Dcompat(glLoadMatrixf)
         {
            sF32 fa[16];
            sUI j = 0u;
            for(sUI rowIdx = 0u; rowIdx < 4u; rowIdx++)
            {
               for(sUI colIdx = 0u; colIdx < 4u; colIdx++)
               {
                  YAC_Value v;
                  _fa->yacArrayGet(NULL, j++, &v);
                  v.typecast(YAC_TYPE_FLOAT);
                  fa[(colIdx<<2)+rowIdx] = v.value.float_val;
               }
            }
            // Dyac_host_printf("xxx call ::glLoadMatrixf() (transpose)\n");
            ::glLoadMatrixf(fa);
            Dtraceglerror("glLoadMatrixf");
         }
#endif // DX_GLES
#endif // DX_EMU_MATRIX
      }
   }
}


// ---------------------------------------------------------------------------- glLoadMatrixf_int
void YAC_CALL _glLoadMatrixf_int(const sF32 *_mat) {
#ifdef DX_EMU_MATRIX
   sUI i;
   for(i=0; i<16; i++)
   {
      tkopengl_matrix[i] = _mat[i];
   }
   tkopengl_matrix_flags[tkopengl_matrix_mode] |= TKOPENGL_MATRIX_FLAG_DIRTY;
   tkopengl_matrix_flags[tkopengl_matrix_mode] &= ~(TKOPENGL_MATRIX_FLAG_ORTHOGONAL | TKOPENGL_MATRIX_FLAG_IDENTITY);
#else
#ifdef DX_GLES
   Dstub(glLoadMatrixf);
#else
   Dcompat(glLoadMatrixf_int)
   {
      ::glLoadMatrixf(_mat);
      Dtraceglerror("glLoadMatrixf_int");
   }
#endif // DX_GLES
#endif // DX_EMU_MATRIX
}

// ---------------------------------------------------------------------------- glMultMatrixf_int
void YAC_CALL _glMultMatrixf_int(const sF32 *_mat) {
#ifdef DX_EMU_MATRIX
   tkopengl_matrix_mul_inplace(tkopengl_matrix, _mat);
   tkopengl_matrix_flags[tkopengl_matrix_mode] |= TKOPENGL_MATRIX_FLAG_DIRTY;
   tkopengl_matrix_flags[tkopengl_matrix_mode] &= ~(TKOPENGL_MATRIX_FLAG_ORTHOGONAL | TKOPENGL_MATRIX_FLAG_IDENTITY);
#else
#ifdef DX_GLES
   Dstub(glMultMatrixf);
#else
   Dcompat(glMultMatrixf_int)
   {
      ::glMultMatrixf(_mat);
      Dtraceglerror("glMultMatrixf_int");
   }
#endif // DX_GLES
#endif // DX_EMU_MATRIX
}

// ---------------------------------------------------------------------------- zglStoreMatrixf_int
void YAC_CALL _zglStoreMatrixf_int(sF32 *_mat) {

#ifdef DX_EMU_MATRIX
   for(int i=0; i<16; i++)
   {
      _mat[i] = tkopengl_matrix[i];
   }
#else
#ifdef DX_GLES
   Dstub(zglStoreMatrixf);
#else
   Dcompat(zglStoreMatrixf)
   {
      GLint mm;
      ::glGetIntegerv(GL_MATRIX_MODE, &mm);

      switch(mm)
      {
         default: // (todo) color matrix?
         case GL_MODELVIEW:
            ::glGetFloatv(GL_MODELVIEW_MATRIX, _mat);
            break;

         case GL_PROJECTION:
            ::glGetFloatv(GL_PROJECTION_MATRIX, _mat);
            break;

         case GL_TEXTURE:
            ::glGetFloatv(GL_TEXTURE_MATRIX, _mat);
            break;
      }
#if 0
      yac_host->printf("xxx tkopengl::zglStoreMatrixf: ma={\n\t%f %f %f %f\n\t%f %f %f %f\n\t%f %f %f %f\n\t%f %f %f %f\n\t}\n",
                       _mat[0], _mat[4], _mat[8], _mat[12],
                       _mat[1], _mat[5], _mat[9], _mat[13],
                       _mat[2], _mat[6], _mat[10], _mat[14],
                       _mat[3], _mat[7], _mat[11], _mat[15]
                       );
#endif
   }
#endif // DX_GLES

#endif //  DX_EMU_MATRIX
}

// ---------------------------------------------------------------------------- glGetString
void YAC_CALL _glGetString(sSI _id, YAC_Value *_r) {
   if(tkopengl_b_glcore && GL_EXTENSIONS == _id)
   {
      YAC_String *s = YAC_New_String();
      _r->initString(s, 1);

      Dcheckext(glGetStringi)
      {
         YAC_String t;
         sUI numExt = _zglGetInteger(GL_NUM_EXTENSIONS);
         sUI extIdx = 0u;
         for(; extIdx < numExt; extIdx++)
         {
            const GLubyte *gls = ::glGetStringi_xref(GL_EXTENSIONS, extIdx);
            Dtraceglerror("glGetStringi");
            if(NULL != gls)
            {
               if(extIdx > 0u)
                  t.append(" ");
               t.append((const char*)gls);
               // Next extension
               extIdx++;
            }
            else
            {
               (void)::glGetError();
               break;
            }
         }
         _r->value.string_val->yacCopy(&t);
      }
   }
   else
   {
      const GLubyte *gls = glGetString(_id);
      Dtraceglerror("glGetString");
      if(NULL != gls)
      {
         YAC_String *s = YAC_New_String();
         s->copy((const sChar*)gls);
         _r->initString(s, 1);
      }
      else
      {
         _r->initString(NULL, 0);
      }
   }
}

// ---------------------------------------------------------------------------- zglGetFloat
sF32 YAC_CALL _zglGetFloat(sSI _type) {
   sF32 r[16]; // paranoia, in case someone tries to query a matrix
   ::glGetFloatv(_type, r);
   Dtraceglerror("zglGetFloat");
   return r[0];
}

// ---------------------------------------------------------------------------- glGetFloatv
void YAC_CALL _glGetFloatv(sSI _type, YAC_Object *_faRet) {
   if(YAC_BCHK(_faRet, YAC_CLID_FLOATARRAY))
   {
      sF32 r[16];
      ::glGetFloatv(_type, r);
      Dtraceglerror("glGetFloatv");

      // Copy to result array
      YAC_CAST_ARG(YAC_FloatArray, fa, _faRet);
      sUI i;
      for(i=0; (i<16) && (i<fa->num_elements); i++)
      {
         fa->elements[i] = r[i];
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "glGetFloatv: return object is not a FloatArray");
   }
}

// ---------------------------------------------------------------------------- zglGetInteger
sSI YAC_CALL _zglGetInteger(sSI _type) {
   switch(_type)
   {
      default:
      {
         sSI r[16]; // paranoia, in case someone tries to query a matrix
         ::glGetIntegerv(_type, r);
         Dtraceglerror("zglGetInteger");
         return r[0];
      }

      case GL_MAJOR_VERSION:  // GL3.x+
      {
         sSI r[16];
         r[0] = 2;
         ::glGetIntegerv(GL_MAJOR_VERSION, r);
         ::glGetError();
         // Dtraceglerror("zglGetInteger");
         return r[0];
      }

      case GL_MINOR_VERSION:
      {
         sSI r[16];
         r[0] = 1;
         ::glGetIntegerv(GL_MINOR_VERSION, r);
         ::glGetError();
         // Dtraceglerror("zglGetInteger");
         return r[0];
      }

#ifdef DX_EMU_LIGHT
      case GL_MAX_LIGHTS:
         return TKOPENGL_MAX_LIGHTS;
#endif // DX_EMU_LIGHT
   }
}

// ---------------------------------------------------------------------------- glGetIntegerv
void YAC_CALL _glGetIntegerv(sSI _type, YAC_Object *_iaRet) {
   if(YAC_BCHK(_iaRet, YAC_CLID_INTARRAY))
   {
      sSI r[16];
      ::glGetIntegerv(_type, r);
      Dtraceglerror("glGetIntegerv");

      // Copy to result array
      YAC_CAST_ARG(YAC_IntArray, ia, _iaRet);
      sUI i;
      for(i=0; (i<16) && (i<ia->num_elements); i++)
      {
         ia->elements[i] = r[i];
      }
   }
   else
   {
      Dyac_throw_def(NativeClassTypeMismatch, "glGetIntegerv: return object is not an IntArray");
   }
}

// ---------------------------------------------------------------------------- zglReadColorPixels
sUI YAC_CALL _zglReadColorPixel(sSI _x, sSI _y) {
	sUI pixel = 0xdeadc0deu;
   ::glReadPixels(_x, _y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, (void*)&pixel);
   // Convert R,G,B,A (ABGR32) to packed ARGB32 (B,G,R,A)
   pixel = (sUI)
      ( pixel & 0xFF000000u)        |
      ((pixel & 0x00FF0000u) >> 16) |
      ((pixel & 0x000000FFu) << 16) |
      ((pixel & 0x0000FF00u))       ;
   Dtraceglerror("zglReadColorPixel");
   return pixel;
}

// ---------------------------------------------------------------------------- zglReadColorPixelsX
void _zglReadColorPixelsX(YAC_Object *_tex, sUI _off, sUI _x, sUI _y, sUI _sx) {
   if(YAC_VALID(_tex))
   {
      sUI *pixels = new(std::nothrow) sUI[_sx];

      if(NULL != pixels)
      {
         YAC_Value pixVal;
         pixVal.initInt(0);

         ::glReadPixels(_x, _y, _sx, 1, GL_RGBA, GL_UNSIGNED_BYTE, (void*)pixels);
         Dtraceglerror("zglReadColorPixelsX");

         sUI cx;
         for(cx = 0u; cx < _sx; cx++)
         {
            // Convert R,G,B,A (ABGR32) to packed ARGB32 (B,G,R,A)
            sUI cp = pixels[cx];
            pixVal.value.int_val = (sSI)
               ( cp & 0xFF000000)        |
               ((cp & 0x00FF0000) >> 16) |
               ((cp & 0x000000FF) << 16) |
               ((cp & 0x0000FF00))       ;
            ///pixVal.value.int_val = cx | 0xFF000000;
            _tex->yacArraySet(NULL/*context*/, _off, &pixVal);
            _off++;
         }

         delete [] pixels;
      }
   }
}

// ---------------------------------------------------------------------------- zglReadDepthPixel
sF32 YAC_CALL _zglReadDepthPixel(sSI _x, sSI _y) {
#ifdef DX_GLES
   // (todo) gles
   Dstub(zglReadDepthPixel);
   return 0.0f;
#else
	sF32 pixel=0;
	::glReadPixels(_x, _y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, (void*)&pixel);
   Dtraceglerror("zglReadDepthPixel");
	return pixel;
#endif // DX_GLES
}

// ---------------------------------------------------------------------------- zglClipPlane
void YAC_CALL _zglClipPlane(sSI _i, sF32 _eq0, sF32 _eq1, sF32 _eq2, sF32 _eq3) {
#ifdef DX_GLES
   // (todo) gles
   Dstub(zglClipPlane);
#else
   Dcompat(zglClipPlane)
   {
      static sF64 eq[4];
      eq[0] = _eq0;
      eq[1] = _eq1;
      eq[2] = _eq2;
      eq[3] = _eq3;
      ::glClipPlane(_i, eq);
      Dtraceglerror("zglClipPlane");
   }
#endif // DX_GLES
}

// ---------------------------------------------------------------------------- zglColor1f
void YAC_CALL _zglColor1f(sF32 _cf) {
#ifdef DX_EMU_IMM
   tkopengl_imm_color_cbk(_cf, _cf, _cf, _cf);
#else
#ifdef DX_GLES
   Dstub(zglColor1f);
#else
   Dcompat(zglColor1f)
   {
      ::glColor4f(_cf, _cf, _cf, _cf);
      Dtracegldrawerror("zglColor1f");
   }
#endif // DX_GLES
#endif // DX_EMU_IMM
}

// ---------------------------------------------------------------------------- glGetError
sSI YAC_CALL _glGetError(void) {
   // (note) called by scripts
   GLenum err;
#ifdef TRACE_GL_ERROR
   err = last_gl_error;
   last_gl_error = GL_NO_ERROR;
#else
   err = ::glGetError();
#endif // TRACE_GL_ERROR
   return err;
}

// ---------------------------------------------------------------------------- glGetError_int
GLenum YAC_CALL _glGetError_int(void) {
   // (note) called by (native code) plugins
#ifdef TRACE_GL_ERROR
   last_gl_error = ::glGetError();
   return last_gl_error;
#else
   GLenum err = ::glGetError();
   return err;
#endif // TRACE_GL_ERROR
}


//
//
// OpenGL 1.5+ extension functions
//
//

// ---------------------------------------------------------------------------- zglGenBuffer
sUI YAC_CALL _zglGenBuffer(void) {
   sUI r = 0u;
   if(glGenBuffers_xref)
   {
      ::glGenBuffers_xref(1, &r);
      Dtraceglerror("zglGenBuffer");
   }
   return r;
}

// ---------------------------------------------------------------------------- zglDeleteBuffer
void YAC_CALL _zglDeleteBuffer(sSI _id) {
   if(glDeleteBuffers_xref)
   {
      ::glDeleteBuffers_xref(1, (sUI*) &_id);
      Dtraceglerror("zglDeleteBuffer");
   }
}

// ---------------------------------------------------------------------------- zglBuffer_GetSizeAndPtr
static void _zglBuffer_GetSizeAndPtr(YAC_Object *_data, sSI *_retSize, void**_retDataPtr) {
   if(YAC_BCHK(_data, YAC_CLID_FLOATARRAY))
   {
      YAC_FloatArray *fa = (YAC_FloatArray*) _data;
      if(*_retSize <= 0)
      {
         *_retSize = fa->num_elements * sizeof(sF32);
      }
      *_retDataPtr = (void*) fa->elements;
   }
   else if(YAC_BCHK(_data, YAC_CLID_INTARRAY))
   {
      YAC_IntArray *ia = (YAC_IntArray*) _data;
      if(*_retSize <= 0)
      {
         *_retSize = ia->num_elements * sizeof(sSI);
      }
      *_retDataPtr = (void*) ia->elements;
   }
   else if(YAC_BCHK(_data, YAC_CLID_BUFFER))
   {
      YAC_Buffer *b = (YAC_Buffer*) _data;
      if(*_retSize <= 0)
      {
         *_retSize = b->size;
      }
      *_retDataPtr = (void*) b->buffer;
   }
   else if(YAC_VALID(_data))
   {
      // Generic array object (e.g. a Texture)
      switch(_data->yacArrayGetElementType())
      {
         case YAC_TYPE_INT:
            if(*_retSize <= 0)
            {
               *_retSize = _data->yacArrayGetNumElements() * sizeof(sSI);
            }
            *_retDataPtr = (void*) _data->yacArrayGetPointer();
            break;

         case YAC_TYPE_FLOAT:
            if(*_retSize <= 0)
            {
               *_retSize = _data->yacArrayGetNumElements() * sizeof(sF32);
            }
            *_retDataPtr = (void*) _data->yacArrayGetPointer();
            break;

         default:
            // Array of objects or not an array at all
            return;
      }
   }
}

// ---------------------------------------------------------------------------- zglBufferData
void YAC_CALL _zglBufferData(sSI _target, sSI _size, YAC_Object *_data, sSI _usage) {
   if(glBufferData_xref)
   {
      void *dataPtr = NULL;
      _zglBuffer_GetSizeAndPtr(_data, &_size, &dataPtr);
      ::glBufferData_xref(_target, _size, dataPtr, _usage);
      Dtraceglerror("zglBufferData");
   }
}

void _glBufferData(GLenum _target, GLsizei _size, const GLvoid *_data, GLenum _usage) {
   if(glBufferData_xref)
   {
      ::glBufferData_xref(_target, _size, _data, _usage);
      Dtraceglerror("glBufferData");
   }
}

// ---------------------------------------------------------------------------- zglBufferSubData
void YAC_CALL _zglBufferSubData(sSI _target, sSI _offset, sSI _size, YAC_Object *_data) {
   if(glBufferSubData_xref)
   {
      void *dataPtr = NULL;
      _zglBuffer_GetSizeAndPtr(_data, &_size, &dataPtr);
      if(NULL != dataPtr)
      {
         ::glBufferSubData_xref(_target, _offset, _size, dataPtr);
         Dtraceglerror("zglBufferSubData");
      }
   }
}

// ---------------------------------------------------------------------------- zglBufferSubDataOffset
void YAC_CALL _zglBufferSubDataOffset(sSI _target, sSI _offset, sSI _size, YAC_Object *_data, sUI _dataOffset) {
   if(glBufferSubData_xref)
   {
      void *dataPtr = NULL;
      _zglBuffer_GetSizeAndPtr(_data, &_size, &dataPtr);
      if(NULL != dataPtr)
      {
         dataPtr = (void*) (((sU8*)dataPtr) + _dataOffset);
         ::glBufferSubData_xref(_target, _offset, _size, dataPtr);
         Dtraceglerror("zglBufferSubDataOffset");
      }
   }
}

// ---------------------------------------------------------------------------- glBindBuffer
void YAC_CALL _glBindBuffer(sSI _target, sUI _id) {
   if(glBindBuffer_xref)
   {
      Dtrace_call("xxx glBindBuffer. target=%d id=%d\n", _target, _id);
      ::glBindBuffer_xref(_target, _id);
      Dtraceglerror("glBindBuffer");
   }
}

// ---------------------------------------------------------------------------- zglGetBufferParameter
sSI YAC_CALL _zglGetBufferParameter(sSI _target, sSI _value) {
   sSI r = 0;
   if(glGetBufferParameteriv_xref)
   {
      ::glGetBufferParameteriv_xref(_target, _value, &r);
      Dtraceglerror("glGetBufferParameteriv");
   }
   return r;
}

// ---------------------------------------------------------------------------- zglMapBuffer
void YAC_CALL _zglMapBuffer(sSI _target, sSI _access, YAC_Object *_buffer, sUI _byteSize) {
   if(YAC_BCHK(_buffer, YAC_CLID_BUFFER))
   {
      YAC_CAST_ARG(YAC_Buffer, b, _buffer);

      if(glMapBuffer_xref)
      {
         // Map buffer object into client address space
         void *data = glMapBuffer_xref(_target, _access);
         Dtraceglerror("zglMapBuffer");

         // Free previous buffer data (if any)
         b->yacArrayAlloc(0, 0, 0, 0);

         /////yac_host->printf("xxx glMapBuffer data=%p\n", data);
         if(NULL != data)
         {
            b->buffer    = (sU8*)data;
            b->deleteme  = YAC_FALSE;
            b->size      = _byteSize;
            b->io_offset = 0u;
         }
      }
   }
   else
   {
      yac_host->printf("[---] zglMapBuffer: buffer %p is not a valid Buffer object.\n", (void*)_buffer);
   }
}

// ---------------------------------------------------------------------------- zglMapBuffer
void YAC_CALL _zglMapBufferRange(sSI _target, sUI _offset, sUI _length, sSI _access, YAC_Object *_buffer) {
   // (note) requires GL 4.1 core profile on macOS
   if(YAC_BCHK(_buffer, YAC_CLID_BUFFER))
   {
      YAC_CAST_ARG(YAC_Buffer, b, _buffer);

      if(glMapBufferRange_xref)
      {
         // Map buffer object into client address space
         void *data = glMapBufferRange_xref(_target, _offset, _length, _access);
         Dtraceglerror("zglMapBufferRange");

         // Free previous buffer data (if any)
         b->yacArrayAlloc(0, 0, 0, 0);

         // Dyac_host_printf("xxx call zglMapBufferRange: offset=%u length=%u => data=%p\n", _offset, _length, data);
         if(NULL != data)
         {
            b->buffer    = (sU8*)data;
            b->deleteme  = YAC_FALSE;
            b->size      = _length;
            b->io_offset = 0u;
         }
      }
      else
      {
         Dyac_host_printf("[!!!] glMapBufferRange_xref is NULL !!\n");
      }
   }
   else
   {
      yac_host->printf("[---] zglMapBufferRange: buffer %p is not a valid Buffer object.\n", (void*)_buffer);
   }
}

// ---------------------------------------------------------------------------- glFlushMappedBufferRange
void YAC_CALL _glFlushMappedBufferRange(sSI _target, sUI _offset, sUI _length) {
   // (note) requires GL 4.1 core profile on macOS
   if(glFlushMappedBufferRange_xref)
   {
      // Dyac_host_printf("xxx call glFlushMappedBufferRange: offset=%u length=%u\n", _offset, _length);
      glFlushMappedBufferRange_xref(_target, _offset, _length);
      Dtraceglerror("glFlushMappedBufferRange");
   }
}

// ---------------------------------------------------------------------------- glUnmapBuffer
sSI YAC_CALL _glUnmapBuffer(sSI _target) {
   if(glUnmapBuffer_xref)
   {
      return glUnmapBuffer_xref(_target);
      Dtraceglerror("glUnmapBuffer");
   }
   return 0;
}

// ---------------------------------------------------------------------------- glIsBuffer
sSI YAC_CALL _glIsBuffer(sSI _id) {
   Dcheckext(glIsBuffer)
   {
      sSI r = ::glIsBuffer_xref(_id);
      Dtraceglerror("glIsBuffer");
      return r;
   }
   return 0;
}

// ---------------------------------------------------------------------------- zglVertexOffset
void YAC_CALL _zglVertexOffset(sUI _size, sSI _type, sUI _stride, sUI _offset) {
   // (todo) check whether VBO is bound, otherwise set _offset to NULL
#ifdef DX_EMU_FIXFXN
   tkopengl_userarray_t *arr = &tkopengl_attrib->userarray[TKOPENGL_USERARRAY_VERTEX];
   arr->type        = (GLenum)_type;
   arr->num_comp    = _size;
   arr->stride      = _stride;
   arr->addr.offset = _offset;
#else
#ifdef DX_GLES
   Dstub(zglVertexOffset);
#else
   Dcompat(zglVertexOffset)
   {
      ::glVertexPointer(_size, _type, _stride, (void*)size_t(_offset));
      Dtraceglerror("zglVertexOffset");
   }
#endif // DX_GLES
#endif // DX_EMU_FIXFXN
}

// ---------------------------------------------------------------------------- zglNormalOffset
void YAC_CALL _zglNormalOffset(sSI _type, sUI _stride, sUI _offset) {
   // (todo) check whether VBO is bound, otherwise set _offset to NULL
#ifdef DX_EMU_FIXFXN
   tkopengl_userarray_t *arr = &tkopengl_attrib->userarray[TKOPENGL_USERARRAY_NORMAL];
   arr->type        = (GLenum)_type;
   arr->num_comp    = 3;
   arr->stride      = _stride;
   arr->addr.offset = _offset;
#else
#ifdef DX_GLES
   Dstub(zglNormalOffset);
#else
   Dcompat(zglNormalOffset)
   {
      ::glNormalPointer(_type, _stride, (void*)size_t(_offset));
      Dtraceglerror("zglNormalOffset");
   }
#endif // DX_GLES
#endif // DX_EMU_FIXFXN
}

// ---------------------------------------------------------------------------- zglColorOffset
void YAC_CALL _zglColorOffset(sUI _size, sSI _type, sUI _stride, sUI _offset) {
   // (todo) check whether VBO is bound, otherwise set _offset to NULL
#ifdef DX_EMU_FIXFXN
   tkopengl_userarray_t *arr = &tkopengl_attrib->userarray[TKOPENGL_USERARRAY_COLOR];
   arr->type        = (GLenum)_type;
   arr->num_comp    = _size;
   arr->stride      = _stride;
   arr->addr.offset = _offset;
#else
#ifdef DX_GLES
   Dstub(zglColorOffset);
#else
   Dcompat(zglColorOffset)
   {
      ::glColorPointer(_size, _type, _stride, (void*)size_t(_offset));
      Dtraceglerror("zglColorOffset");
   }
#endif // DX_GLES
#endif // DX_EMU_FIXFXN
}

// ---------------------------------------------------------------------------- zglTexCoordOffset
void YAC_CALL _zglTexCoordOffset(sUI _size, sSI _type, sUI _stride, sUI _offset) {
   // (todo) check whether VBO is bound, otherwise set _offset to NULL
#ifdef DX_EMU_FIXFXN
   tkopengl_userarray_t *arr = &tkopengl_attrib->userarray[TKOPENGL_USERARRAY_TEXCOORD];
   arr->type        = (GLenum)_type;
   arr->num_comp    = _size;
   arr->stride      = _stride;
   arr->addr.offset = _offset;
#else
#ifdef DX_GLES
   Dstub(zglTexCoordOffset);
#else
   Dcompat(zglTexCoordOffset)
   {
      ::glTexCoordPointer(_size, _type, _stride, (void*)size_t(_offset));
      Dtraceglerror("zglTexCoordOffset");
   }
#endif // DX_GLES
#endif // DX_EMU_FIXFXN
}


//
//
// Framebuffer objects (FBO)
//
//

// ---------------------------------------------------------------------------- glIsRenderbuffer
sSI YAC_CALL _glIsRenderbuffer(sUI _renderbuffer) {
   Dcheckext(glIsRenderbuffer)
   {
      sSI r = glIsRenderbuffer_xref(_renderbuffer);
      Dtraceglerror("glIsRenderbuffer");
      return r;
   }
   return 0;
}

// ---------------------------------------------------------------------------- glBindRenderbuffer
void YAC_CALL _glBindRenderbuffer(sSI _target, sUI _renderbuffer) {
   Dcheckext(glBindRenderbuffer)
   {
      glBindRenderbuffer_xref(_target, _renderbuffer);
      Dtraceglerror("glBindRenderbuffer");
   }
}

// ---------------------------------------------------------------------------- zglDeleteRenderbuffer
void YAC_CALL _zglDeleteRenderbuffer(sUI _renderbuffer) {
   Dcheckext(glDeleteRenderbuffers)
   {
      glDeleteRenderbuffers_xref(1, &_renderbuffer);
      Dtraceglerror("glDeleteRenderbuffers");
   }
}

// ---------------------------------------------------------------------------- zglGenRenderbuffer
sUI YAC_CALL _zglGenRenderbuffer(void) {
   Dcheckext(glGenRenderbuffers)
   {
      sUI r;
      glGenRenderbuffers_xref(1, &r);
      Dtraceglerror("glGenRenderbuffers");
      return r;
   }
   return 0;
}

// ---------------------------------------------------------------------------- glRenderbufferStorage
void YAC_CALL _glRenderbufferStorage(sSI _target, sSI _internalFormat, sUI _width, sUI _height) {

#ifdef DX_GLES
   switch(_internalFormat)
   {
      default:
         break;
      case GL_DEPTH_COMPONENT24:
      case GL_DEPTH_COMPONENT32:
         _internalFormat = GL_DEPTH_COMPONENT16;
         break;
   }
#endif // DX_GLES

   Dcheckext(glRenderbufferStorage)
   {
      glRenderbufferStorage_xref(_target, _internalFormat, _width, _height);
      Dtraceglerror("glRenderbufferStorage");
   }
}

// ---------------------------------------------------------------------------- glRenderbufferStorage
void YAC_CALL _glRenderbufferStorageMultisample(sSI _target, sUI _samples, sSI _internalFormat, sUI _width, sUI _height) {

#ifdef DX_GLES
   switch(_internalFormat)
   {
      default:
         break;
      case GL_DEPTH_COMPONENT24:
      case GL_DEPTH_COMPONENT32:
         _internalFormat = GL_DEPTH_COMPONENT16;
         break;
   }
#endif // DX_GLES

   Dcheckext(glRenderbufferStorageMultisample)
   {
      glRenderbufferStorageMultisample_xref(_target, _samples, _internalFormat, _width, _height);
      Dtraceglerror("glRenderbufferStorageMultisample");
   }
}

// ---------------------------------------------------------------------------- zglGetRenderbufferParameteri
sSI YAC_CALL _zglGetRenderbufferParameteri(sSI _target, sSI _pname) {
   Dcheckext(glGetRenderbufferParameteriv)
   {
      sSI r[4]; // (todo) always one single int or do we need to reserve more space ?
      glGetRenderbufferParameteriv_xref(_target, _pname, r);
      Dtraceglerror("glGetRenderbufferParameteriv");
      return r[0];
   }
   return 0;
}

// ---------------------------------------------------------------------------- glIsFramebuffer
sSI YAC_CALL _glIsFramebuffer(sUI _framebuffer) {
   Dcheckext(glIsFramebuffer)
   {
      sSI r = glIsFramebuffer_xref(_framebuffer);
      Dtraceglerror("glIsFramebuffer");
      return r;
   }
   return 0;
}

// ---------------------------------------------------------------------------- glBindFramebuffer
void YAC_CALL _glBindFramebuffer(sSI _target, sUI _framebuffer) {
   Dcheckext(glBindFramebuffer)
   {
      glBindFramebuffer_xref(_target, _framebuffer);
      Dtraceglerror("glBindFramebuffer");
   }
}

// ---------------------------------------------------------------------------- zglDeleteFramebuffer
void YAC_CALL _zglDeleteFramebuffer(sUI _framebuffer) {
   Dcheckext(glDeleteFramebuffers)
   {
      glDeleteFramebuffers_xref(1, &_framebuffer);
      Dtraceglerror("glDeleteFramebuffers");
   }
}

// ---------------------------------------------------------------------------- zglGenFramebuffer
sUI YAC_CALL _zglGenFramebuffer(void) {
   Dcheckext(glGenFramebuffers)
   {
      sUI r;
      glGenFramebuffers_xref(1, &r);
      Dtraceglerror("glGenFramebuffers");
      return r;
   }
   return 0;
}

// ---------------------------------------------------------------------------- glCheckFramebufferStatus
sSI YAC_CALL _glCheckFramebufferStatus(sSI _target) {
   Dcheckext(glCheckFramebufferStatus)
   {
      sSI r = glCheckFramebufferStatus_xref(_target);
      Dtraceglerror("glCheckFramebufferStatus");
      return r;
   }
   return 0;
}

// ---------------------------------------------------------------------------- glFramebufferTexture1D
void YAC_CALL _glFramebufferTexture1D(sSI _target, sSI _attachment, sSI _textarget, sUI _texture, sSI _level) {

#ifdef DX_EMU_TEXTURE_1D
   _textarget = GL_TEXTURE_2D;
#endif

   Dcheckext(glFramebufferTexture1D)
   {
      glFramebufferTexture1D_xref(_target, _attachment, _textarget, _texture, _level);
      Dtraceglerror("glFramebufferTexture1D");
   }
}

// ---------------------------------------------------------------------------- glFramebufferTexture2D
void YAC_CALL _glFramebufferTexture2D(sSI _target, sSI _attachment, sSI _textarget, sUI _texture, sSI _level) {

#ifdef DX_EMU_TEXTURE_RECTANGLE
   if(GL_TEXTURE_RECTANGLE == _textarget)
   {
      _textarget = GL_TEXTURE_2D;
   }
#endif // DX_EMU_TEXTURE_RECTANGLE

   Dcheckext(glFramebufferTexture2D)
   {
      glFramebufferTexture2D_xref(_target, _attachment, _textarget, _texture, _level);
      Dtraceglerror("glFramebufferTexture2D");
   }
}

// ---------------------------------------------------------------------------- glFramebufferTexture3D
void YAC_CALL _glFramebufferTexture3D(sSI _target, sSI _attachment, sSI _textarget, sUI _texture, sSI _level, sSI _zoffset) {
#if defined(DX_GLES) && !defined(DX_TEXTURE_3D)
#else
   Dcheckext(glFramebufferTexture3D)
   {
      glFramebufferTexture3D_xref(_target, _attachment, _textarget, _texture, _level, _zoffset);
      Dtraceglerror("glFramebufferTexture3D");
   }
#endif
}

// ---------------------------------------------------------------------------- glFramebufferRenderbuffer
void YAC_CALL _glFramebufferRenderbuffer(sSI _target, sSI _attachment, sSI _renderbuffertarget, sSI _renderbuffer) {
   Dcheckext(glFramebufferRenderbuffer)
   {
      glFramebufferRenderbuffer_xref(_target, _attachment, _renderbuffertarget, _renderbuffer);
      Dtraceglerror("glFramebufferRenderbuffer");
   }
}

// ---------------------------------------------------------------------------- zglGetFramebufferAttachmentParameteri
sSI YAC_CALL _zglGetFramebufferAttachmentParameteri(sSI _target, sSI _attachment, sSI _pname) {
   Dcheckext(glGetFramebufferAttachmentParameteriv)
   {
      sSI r[4];
      glGetFramebufferAttachmentParameteriv_xref(_target, _attachment, _pname, r);
      Dtraceglerror("glGetFramebufferAttachmentParameteriv");
      return r[0];  // (todo) always one single int ?
   }
   return 0;
}

// ---------------------------------------------------------------------------- zglGenFramebuffer
void YAC_CALL _glBlitFramebuffer(sSI _srcX0, sSI _srcY0, sSI _srcX1, sSI _srcY1, sSI _dstX0, sSI _dstY0, sSI _dstX1, sSI _dstY1, sUI _mask, sSI _filter) {
   Dcheckext(glBlitFramebuffer)
   {
      glBlitFramebuffer_xref(_srcX0, _srcY0, _srcX1, _srcY1, _dstX0, _dstY0, _dstX1, _dstY1, (GLbitfield)_mask, (GLenum)_filter);
      Dtraceglerror("glBlitFramebuffer");
   }
}

// ---------------------------------------------------------------------------- glGenerateMipmap
void YAC_CALL _glGenerateMipmap(sSI _a) {
   // Note: better use GL_GENERATE_MIPMAP instead since this seems to be broken on ATI (needs to be called twice!)
   Dcheckext(glGenerateMipmap)
   {
      glGenerateMipmap_xref(_a);
      Dtraceglerror("glGenerateMipMap");
   }
}


//
//
// GLSL
//
//

// ---------------------------------------------------------------------------- glCreateProgram
sUI YAC_CALL _glCreateProgram (void) {
   sUI r = 0;
   Dcheckext(glCreateProgram)
   {
      r = glCreateProgram_xref();

      Dtraceglerror("glCreateProgram");
   }
   return r;
}

// ---------------------------------------------------------------------------- glDeleteProgram
void YAC_CALL _glDeleteProgram (sUI _program) {
   Dcheckext(glDeleteProgram)
   {
      glDeleteProgram_xref(_program);

      Dtraceglerror("glDeleteProgram");
   }
}

// ---------------------------------------------------------------------------- glCreateShader
sUI YAC_CALL _glCreateShader (sUI _shaderType) {
   sUI r = 0;
   Dcheckext(glCreateShader)
   {
      r = glCreateShader_xref(_shaderType);

      Dtraceglerror("glCreateShader");
   }
   return r;
}

// ---------------------------------------------------------------------------- glDeleteShader
void YAC_CALL _glDeleteShader (sUI _shader) {
   Dcheckext(glDeleteShader)
   {
      glDeleteShader_xref(_shader);

      Dtraceglerror("glDeleteShader");
   }
}

// ---------------------------------------------------------------------------- zglShaderSource
void YAC_CALL _zglShaderSource (sUI _shader, YAC_Object *_source) {
   Dcheckext(glShaderSource)
   {
      if(YAC_Is_String(_source))
      {
         YAC_CAST_ARG(YAC_String, source, _source);
         const char*strings = (const char*) source->chars;
         const GLint lengths  = (GLint) (source->length - 1);

         _glShaderSource_int(_shader, 1, &strings, &lengths);
         // yac_host->printf("xxx zglShaderSource: strings=\"%s\"\n", strings);
      }
   }
}

// ---------------------------------------------------------------------------- glShaderSource_int
void YAC_CALL _glShaderSource_int(GLuint _shader, GLsizei _count, const char *const*_strings, const GLint *_lengths) {

   glShaderSource_xref(_shader, _count, _strings, _lengths);
   // yac_host->printf("xxx zglShaderSource: strings=\"%s\"\n", strings);

   Dtraceglerror("glShaderSource");
}

// ---------------------------------------------------------------------------- glCompileShader
void YAC_CALL _glCompileShader (sUI _shader) {
   Dcheckext(glCompileShader)
   {
      glCompileShader_xref(_shader);

      Dtraceglerror("glCompileShader");
   }
}

// ---------------------------------------------------------------------------- zglGetProgrami
sSI YAC_CALL _zglGetProgrami (sUI _program, sSI _pname) {
   sSI r = 0;

   _glGetProgramiv_int(_program, _pname, (GLint*)&r);

   return r;
}

// ---------------------------------------------------------------------------- glGetProgramiv_int
void YAC_CALL _glGetProgramiv_int(GLuint _program, GLenum _pname, GLint *_params) {

   Dcheckext(glGetProgramiv)
   {
      glGetProgramiv_xref(_program, _pname, _params);

      Dtraceglerror("glGetProgramiv");
   }
}

// ---------------------------------------------------------------------------- glGetProgramInfoLog
void YAC_CALL _glGetProgramInfoLog (sUI _program, YAC_Object *_retLog) {
   Dcheckext(glGetProgramInfoLog)
   {
      if(YAC_Is_String(_retLog))
      {
         YAC_CAST_ARG(YAC_String, retLog, _retLog);

         sUI logSize;
         glGetProgramiv_xref(_program, 0x8B84/*GL_INFO_LOG_LENGTH*/, (GLint*) &logSize);

         if(retLog->buflen < (logSize+1))
         {
            retLog->yacArrayAlloc(logSize+1, 0,0,0);
         }

         _glGetProgramInfoLog_int(_program, (GLsizei)retLog->buflen, (GLint*)&logSize, (char*)retLog->chars);

         retLog->length = logSize + 1;
         retLog->chars[logSize] = 0;
      }
   }
}

// ---------------------------------------------------------------------------- glGetProgramInfoLog_int
void YAC_CALL _glGetProgramInfoLog_int(GLuint _program, GLsizei _maxLength, GLsizei *_length, char *_infoLog) {
   glGetProgramInfoLog_xref(_program, _maxLength, _length, _infoLog);

   Dtraceglerror("glGetProgramInfoLog");
}

// ---------------------------------------------------------------------------- zglGetShaderi
sSI YAC_CALL _zglGetShaderi (sUI _shader, sSI _pname) {
   GLint r = 0;

   _glGetShaderiv_int((GLuint)_shader, (GLenum)_pname, &r);

   return (sSI)r;
}

// ---------------------------------------------------------------------------- glGetShaderiv_int
void YAC_CALL _glGetShaderiv_int(GLuint _shader, GLenum _pname, GLint *_r) {
   Dcheckext(glGetShaderiv)
   {
      glGetShaderiv_xref(_shader, _pname, _r);

      Dtraceglerror("glGetShaderiv");
   }
}

// ---------------------------------------------------------------------------- glGetShaderInfoLog
void YAC_CALL _glGetShaderInfoLog (sUI _shader, YAC_Object *_retLog) {
   Dcheckext(glGetShaderInfoLog)
   {
      if(YAC_Is_String(_retLog))
      {
         YAC_CAST_ARG(YAC_String, retLog, _retLog);

         sUI logSize;
         _glGetShaderiv_int(_shader, 0x8B84/*GL_INFO_LOG_LENGTH*/, (GLint*) &logSize);

         if(retLog->buflen < (logSize+1))
         {
            retLog->yacArrayAlloc(logSize+1, 0,0,0);
         }

         _glGetShaderInfoLog_int(_shader, retLog->buflen, (GLsizei*)&logSize, (char*)retLog->chars);

         retLog->length = logSize + 1;
         retLog->chars[logSize] = 0;
      }
   }
}

// ---------------------------------------------------------------------------- glGetShaderInfoLog_int
void YAC_CALL _glGetShaderInfoLog_int(GLuint _shader, GLsizei _maxLength, GLsizei *_length, char *_infoLog) {
   glGetShaderInfoLog_xref(_shader, _maxLength, _length, _infoLog);

   Dtraceglerror("glGetShaderInfoLog");
}

// ---------------------------------------------------------------------------- glAttachShader
void YAC_CALL _glAttachShader (sUI _program, sUI _shader) {
   Dcheckext(glAttachShader)
   {
      glAttachShader_xref(_program, _shader);

      Dtraceglerror("glAttachShader");
   }
}

// ---------------------------------------------------------------------------- glDetachShader
void YAC_CALL _glDetachShader (sUI _program, sUI _shader) {
   Dcheckext(glDetachShader)
   {
      glDetachShader_xref(_program, _shader);

      Dtraceglerror("glDetachShader");
   }
}

// ---------------------------------------------------------------------------- glLinkProgram
void YAC_CALL _glLinkProgram (sUI _program) {
   Dcheckext(glLinkProgram)
   {
      glLinkProgram_xref(_program);

      Dtraceglerror("glLinkProgram");
   }
}

// ---------------------------------------------------------------------------- glValidateProgram
void YAC_CALL _glValidateProgram (sUI _program) {
   Dcheckext(glValidateProgram)
   {
      glValidateProgram_xref(_program);

      Dtraceglerror("glValidateProgram");
   }
}

// ---------------------------------------------------------------------------- glUseProgram
void YAC_CALL _glUseProgram (sUI _program) {

#ifdef DX_EMU_FIXFXN
   if(0 != _program)
   {
      b_tkopengl_custom_shader = YAC_TRUE;
   }
   else
   {
      b_tkopengl_custom_shader = YAC_FALSE;
   }
#endif // DX_EMU_FIXFXN

   _glUseProgram_int(_program);
}

// ---------------------------------------------------------------------------- glUseProgram_int
void YAC_CALL _glUseProgram_int(GLuint _program) {

   Dcheckext(glUseProgram)
   {
      glUseProgram_xref(_program);

      Dtraceglerror("glUseProgram");
   }
}

// ---------------------------------------------------------------------------- glBindAttribLocation
void YAC_CALL _glBindAttribLocation (sUI _program, sUI _index, YAC_Object *_name) {
   Dcheckext(glBindAttribLocation)
   {
      if(YAC_Is_String(_name))
      {
         YAC_CAST_ARG(YAC_String, name, _name);

         _glBindAttribLocation_int(_program, _index, (const char*)name->chars);
      }
   }
}

// ---------------------------------------------------------------------------- glBindAttribLocation_int
void YAC_CALL _glBindAttribLocation_int(GLuint _program, GLuint _index, const char *_name) {
   Dcheckext(glBindAttribLocation)
   {
      glBindAttribLocation_xref(_program, _index, _name);

      Dtraceglerror("glBindAttribLocation");
   }
}

// ---------------------------------------------------------------------------- glGetAttribLocation
sSI YAC_CALL _glGetAttribLocation(sUI _program, YAC_Object *_name) {
   sSI r = -1;
   if(YAC_Is_String(_name))
   {
      YAC_CAST_ARG(YAC_String, name, _name);
      r = _glGetAttribLocation_int(_program, (const char*)name->chars);
   }
   return r;
}

// ---------------------------------------------------------------------------- glGetAttribLocation_int
GLint YAC_CALL _glGetAttribLocation_int(GLuint _program, const char *_name) {
   GLint r = -1;
   Dcheckext(glGetAttribLocation)
   {
      r = glGetAttribLocation_xref(_program, _name);
      Dtraceglerror("glGetAttribLocation");
   }
   return r;
}

// ---------------------------------------------------------------------------- glBindFragDataLocation
void YAC_CALL _glBindFragDataLocation(sUI _program, sUI _colorNumber, YAC_Object *_name) {
#ifdef DX_GLES
   Dstub(glBindFragDataLocation);
#else
   Dcheckext(glGetFragDataLocation)
   {
      if(YAC_Is_String(_name))
      {
         YAC_CAST_ARG(YAC_String, name, _name);
         glBindFragDataLocation_xref(_program, _colorNumber, (const char*)name->chars);

         Dtraceglerror("glBindFragDataLocation");
      }
   }
#endif // DX_GLES
}

// ---------------------------------------------------------------------------- glBindFragDataLocationIndexed
void YAC_CALL _glBindFragDataLocationIndexed(sUI _program, sUI _colorNumber, sUI _index, YAC_Object *_name) {
#ifdef DX_GLES
   Dstub(glBindFragDataLocationIndexed);
#else
   Dcheckext(glBindFragDataLocationIndexed)
   {
      if(YAC_Is_String(_name))
      {
         YAC_CAST_ARG(YAC_String, name, _name);
         glBindFragDataLocationIndexed_xref(_program, _colorNumber, _index, (const char*)name->chars);

         Dtraceglerror("glBindFragDataLocationIndexed");
      }
   }
#endif // DX_GLES
}

// ---------------------------------------------------------------------------- glGetFragDataLocation
sSI YAC_CALL _glGetFragDataLocation(sUI _program, YAC_Object *_name) {
   sSI r = -1;
#ifdef DX_GLES
   Dstub(glGetFragDataLocation);
#else
   Dcheckext(glGetFragDataLocation)
   {
      if(YAC_Is_String(_name))
      {
         YAC_CAST_ARG(YAC_String, name, _name);
         r = glGetFragDataLocation_xref(_program, (const char*)name->chars);

         Dtraceglerror("glGetFragDataLocation");
      }
   }
#endif // DX_GLES
   return r;
}

// ---------------------------------------------------------------------------- glGetFragDataIndex
sSI YAC_CALL _glGetFragDataIndex(sUI _program, YAC_Object *_name) {
   sSI r = -1;
#ifdef DX_GLES
   Dstub(glGetFragDataIndex);
#else
   Dcheckext(glGetFragDataIndex)
   {
      if(YAC_Is_String(_name))
      {
         YAC_CAST_ARG(YAC_String, name, _name);
         r = glGetFragDataIndex_xref(_program, (const char*)name->chars);

         Dtraceglerror("glGetFragDataIndex");
      }
   }
#endif // DX_GLES
   return r;
}

// ---------------------------------------------------------------------------- glVertexAttrib1f
void YAC_CALL _glVertexAttrib1f (sUI _index, sF32 _v0) {
   Dcheckext(glVertexAttrib1f)
   {
      glVertexAttrib1f_xref(_index, _v0);

      Dtraceglerror("glVertexAttrib1f");
   }
}

// ---------------------------------------------------------------------------- glVertexAttrib2f
void YAC_CALL _glVertexAttrib2f(sUI _index, sF32 _v0, sF32 _v1) {
   Dcheckext(glVertexAttrib2f)
   {
      glVertexAttrib2f_xref(_index, _v0, _v1);

      Dtraceglerror("glVertexAttrib2f");
   }
}

// ---------------------------------------------------------------------------- glVertexAttrib2fv_int
void YAC_CALL _glVertexAttrib2fv_int(GLuint _index, const GLfloat *_v) {
   Dcheckext(glVertexAttrib2fv)
   {
      glVertexAttrib2fv_xref(_index, _v);

      Dtraceglerror("glVertexAttrib2fv");
   }
}

// ---------------------------------------------------------------------------- glVertexAttrib3f
void YAC_CALL _glVertexAttrib3f(sUI _index, sF32 _v0, sF32 _v1, sF32 _v2) {
   Dcheckext(glVertexAttrib3f)
   {
      glVertexAttrib3f_xref(_index, _v0, _v1, _v2);

      Dtraceglerror("glVertexAttrib3f");
   }
}

// ---------------------------------------------------------------------------- glVertexAttrib3fv_int
void YAC_CALL _glVertexAttrib3fv_int(GLuint _index, const GLfloat *_v) {
   Dcheckext(glVertexAttrib3fv)
   {
      glVertexAttrib3fv_xref(_index, _v);

      Dtraceglerror("glVertexAttrib3fv");
   }
}

// ---------------------------------------------------------------------------- glVertexAttrib4f
void YAC_CALL _glVertexAttrib4f(sUI _index, sF32 _v0, sF32 _v1, sF32 _v2, sF32 _v3) {
   Dcheckext(glVertexAttrib4f)
   {
      glVertexAttrib4f_xref(_index, _v0, _v1, _v2, _v3);

      Dtraceglerror("glVertexAttrib4f");
   }
}

// ---------------------------------------------------------------------------- glVertexAttrib4fv_int
void YAC_CALL _glVertexAttrib4fv_int(GLuint _index, const GLfloat *_v) {
   Dcheckext(glVertexAttrib4fv)
   {
      glVertexAttrib4fv_xref(_index, _v);

      Dtraceglerror("glVertexAttrib4fv");
   }
}

// ---------------------------------------------------------------------------- glVertexAttrib4Nub
void YAC_CALL _glVertexAttrib4Nub(sUI _index, sUI _v0, sUI _v1, sUI _v2, sUI _v3) {
#ifdef DX_GLES
   Dcheckext(glVertexAttrib4f)
   {
      glVertexAttrib4f_xref(_index,
                            ((GLubyte)_v0) * (1.0f / 255.0f),
                            ((GLubyte)_v1) * (1.0f / 255.0f),
                            ((GLubyte)_v2) * (1.0f / 255.0f),
                            ((GLubyte)_v3) * (1.0f / 255.0f)
                            );

      Dtraceglerror("glVertexAttrib4f::glVertexAttrib4Nub_emu");
   }
#else
   Dcheckext(glVertexAttrib4Nub)
   {
      glVertexAttrib4Nub_xref(_index, (GLubyte)_v0, (GLubyte)_v1, (GLubyte)_v2, (GLubyte)_v3);

      Dtraceglerror("glVertexAttrib4Nub");
   }
#endif // DX_GLES
}

// ---------------------------------------------------------------------------- glVertexAttribPointer
void YAC_CALL _glVertexAttribPointer(sUI _index, sUI _size, sSI _type, sSI _bNormalized, sSI _stride, YAC_Object *_a) {

   void *data = NULL;
   if(YAC_VALID(_a))
   {
      data = _a->yacArrayGetPointer();
   }

   _glVertexAttribPointer_int(_index, (GLint)_size, (GLenum)_type, (GLboolean)_bNormalized, _stride, data);
}

// ---------------------------------------------------------------------------- glVertexAttribPointer
void YAC_CALL _zglVertexAttribOffset(sUI _index, sUI _size, sSI _type, sSI _bNormalized, sSI _stride, sSI _offset) {
   void *data = (void*)size_t(_offset);
   _glVertexAttribPointer_int(_index, (GLint)_size, (GLenum)_type, (GLboolean)_bNormalized, _stride, data);
}

// ---------------------------------------------------------------------------- glVertexAttribPointer_int
void YAC_CALL _glVertexAttribPointer_int(GLuint _index, GLint _size, GLenum _type, GLboolean _normalized, GLsizei _stride, const GLvoid *_pointer) {

   Dcheckext(glVertexAttribPointer)
   {
      glVertexAttribPointer_xref(_index, (GLint)_size, (GLenum)_type, (GLboolean)_normalized, _stride, _pointer);

      Dtraceglerror("glVertexAttribPointer");
   }
}

// ---------------------------------------------------------------------------- glEnableVertexAttribArray
void YAC_CALL _glEnableVertexAttribArray(sUI _index) {
   Dcheckext(glEnableVertexAttribArray)
   {
      glEnableVertexAttribArray_xref(_index);

      Dtraceglerror("glEnableVertexAttribArray");
   }
}

// ---------------------------------------------------------------------------- glEnableVertexAttribArray_int_vao
void _glEnableVertexAttribArray_int_vao(sUI _index) {
#ifdef DX_EMU_USE_VAO
   if(loc_lazy_create_builtin_vao("glEnableVertexAttribArray_int_vao"))
   {
      _glBindVertexArray(tkopengl_builtin_vao_id);
      _glEnableVertexAttribArray(_index);
      _glBindVertexArray(0u);
   }
#else
   _glEnableVertexAttribArray(_index);
#endif // DX_EMU_USE_VAO
}

// ---------------------------------------------------------------------------- glDisableVertexAttribArray
void YAC_CALL _glDisableVertexAttribArray(sUI _index) {
   Dcheckext(glDisableVertexAttribArray)
   {
      glDisableVertexAttribArray_xref(_index);

      Dtraceglerror("glDisableVertexAttribArray");
   }
}

// ---------------------------------------------------------------------------- glDisableVertexAttribArray_int_vao
void _glDisableVertexAttribArray_int_vao(sUI _index) {
#ifdef DX_EMU_USE_VAO
   if(loc_lazy_create_builtin_vao("glDisableVertexAttribArray_int_vao"))
   {
      _glBindVertexArray(tkopengl_builtin_vao_id);
      _glDisableVertexAttribArray(_index);
      _glBindVertexArray(0u);
   }
#else
   _glDisableVertexAttribArray(_index);
#endif // DX_EMU_USE_VAO
}

// // ---------------------------------------------------------------------------- glEnableVertexAttribArray
// void YAC_CALL _glEnableVertexAttribArray(sUI _index) {
//    if(0u == tkopengl_current_vao_id)
//    {
//       if(loc_lazy_create_builtin_vao("glVertexAttribPointer_int_vao"))
//       {
//          _glBindVertexArray(tkopengl_builtin_vao_id);
//          _glVertexAttribPointer_int(_index, _size, _type, _normalized, _stride, _pointer);
//          _glBindVertexArray(0u);
//       }

// ---------------------------------------------------------------------------- glGetUniformLocation
sSI YAC_CALL _glGetUniformLocation(sUI _program, YAC_Object *_name) {
   sSI r = -1;
   Dcheckext(glGetUniformLocation)
   {
      if(YAC_Is_String(_name))
      {
         YAC_CAST_ARG(YAC_String, name, _name);

         r = _glGetUniformLocation_int(_program, (char*)name->chars);
      }
   }
   return r;
}

// ---------------------------------------------------------------------------- glGetUniformLocation_int
sSI YAC_CALL _glGetUniformLocation_int(sUI _program, const char *_name) {
   sSI r = glGetUniformLocation_xref(_program, _name);

   Dtraceglerror("glGetUniformLocation");

   return r;
}

// ---------------------------------------------------------------------------- zglGetActiveUniform
sSI YAC_CALL _zglGetActiveUniform(sUI _program, sUI _index, YAC_Object *_retSize, YAC_Object *_retName) {
   sSI r = 0;
   Dcheckext(glGetActiveUniform)
   {
      if(YAC_Is_String(_retName))
      {
         YAC_CAST_ARG(YAC_String, retName, _retName);

         // Query maximum uniform length
         GLint maxLength;
         glGetProgramiv_xref(_program, 0x8B87/*GL_ACTIVE_UNIFORM_MAX_LENGTH*/, &maxLength);

         // Resize destination String if necessary
         if(retName->buflen < (sUI)(maxLength+1))
         {
            retName->yacArrayAlloc(maxLength+1, 0,0,0);
         }

         // Query uniform
         GLint size;
         GLsizei len;
         glGetActiveUniform_xref(_program, _index, maxLength, &len, &size, (GLenum*)&r/*type*/, (char*)retName->chars);

         Dtraceglerror("glGetActiveUniform");

         retName->length = len + 1;
         retName->chars[len] = 0;

         // Store uniform size
         if(YAC_VALID(_retSize))
         {
            _retSize->yacValueOfI(size);
         }
      }
   }
   return r;
}

// ---------------------------------------------------------------------------- glUniform1f
void YAC_CALL _glUniform1f(sSI _location, sF32 _v0) {
   Dcheckext(glUniform1f)
   {
      glUniform1f_xref(_location, _v0);

      Dtraceglerror("glUniform1f");
   }
}

// ---------------------------------------------------------------------------- glUniform2f
void YAC_CALL _glUniform2f(sSI _location, sF32 _v0, sF32 _v1) {
   Dcheckext(glUniform2f)
   {
      glUniform2f_xref(_location, _v0, _v1);

      Dtraceglerror("glUniform2f");
   }
}

// ---------------------------------------------------------------------------- glUniform3f
void YAC_CALL _glUniform3f(sSI _location, sF32 _v0, sF32 _v1, sF32 _v2) {
   Dcheckext(glUniform3f)
   {
      glUniform3f_xref(_location, _v0, _v1, _v2);

      Dtraceglerror("glUniform3f");
   }
}

// ---------------------------------------------------------------------------- glUniform4f
void YAC_CALL _glUniform4f(sSI _location, sF32 _v0, sF32 _v1, sF32 _v2, sF32 _v3) {
   Dcheckext(glUniform4f)
   {
      glUniform4f_xref(_location, _v0, _v1, _v2, _v3);

      Dtraceglerror("glUniform4f");
   }
}

// ---------------------------------------------------------------------------- glUniform2fv_int
void YAC_CALL _glUniform2fv_int(GLint _location, GLsizei _count, const GLfloat *_values) {
   Dcheckext(glUniform2fv)
   {
      glUniform2fv_xref(_location, _count, _values);

      Dtraceglerror("glUniform2fv");
   }
}

// ---------------------------------------------------------------------------- glUniform3fv_int
void YAC_CALL _glUniform3fv_int(GLint _location, GLsizei _count, const GLfloat *_values) {
   Dcheckext(glUniform3fv)
   {
      glUniform3fv_xref(_location, _count, _values);

      Dtraceglerror("glUniform3fv");
   }
}

// ---------------------------------------------------------------------------- glUniform4fv_int
void YAC_CALL _glUniform4fv_int(GLint _location, GLsizei _count, const GLfloat *_values) {
   Dcheckext(glUniform4fv)
   {
      glUniform4fv_xref(_location, _count, _values);

      Dtraceglerror("glUniform4fv");
   }
}

// ---------------------------------------------------------------------------- glUniform1i
void YAC_CALL _glUniform1i(sSI _location, sSI _v0) {
   Dcheckext(glUniform1i)
   {
      glUniform1i_xref(_location, _v0);

      Dtraceglerror("glUniform1i");
   }
}

// ---------------------------------------------------------------------------- glUniform2i
void YAC_CALL _glUniform2i(sSI _location, sSI _v0, sSI _v1) {
   Dcheckext(glUniform2i)
   {
      glUniform2i_xref(_location, _v0, _v1);

      Dtraceglerror("glUniform2i");
   }
}

// ---------------------------------------------------------------------------- glUniform3i
void YAC_CALL _glUniform3i(sSI _location, sSI _v0, sSI _v1, sSI _v2) {
   Dcheckext(glUniform3i)
   {
      glUniform3i_xref(_location, _v0, _v1, _v2);

      Dtraceglerror("glUniform3i");
   }
}

// ---------------------------------------------------------------------------- glUniform4i
void YAC_CALL _glUniform4i(sSI _location, sSI _v0, sSI _v1, sSI _v2, sSI _v3) {
   Dcheckext(glUniform4i)
   {
      glUniform4i_xref(_location, _v0, _v1, _v2, _v3);

      Dtraceglerror("glUniform4i");
   }
}

// ---------------------------------------------------------------------------- int_glUniformMatrixfv_prepare_array
#define INT_GLUNIFORMMATRIXFV_MAXELEMENTS 256

static sF32 int_glUniformMatrixfv_array[INT_GLUNIFORMMATRIXFV_MAXELEMENTS];

static void int_glUniformMatrixfv_prepare_array(sSI numElementsPerMatrix, sSI _count, YAC_Object *_fa) {
   if(YAC_VALID(_fa))
   {
      sUI k = 0;
      while(_count-- > 0)
      {
         for(sSI i=0; i<numElementsPerMatrix; i++)
         {
            YAC_Value r;
            _fa->yacArrayGet(NULL, k, &r);
            r.typecast(YAC_TYPE_FLOAT);
            int_glUniformMatrixfv_array[k] = r.value.float_val;
            k++;
            if(k >= INT_GLUNIFORMMATRIXFV_MAXELEMENTS)
            {
               return;
            }
         }
      }
   }
}

// ---------------------------------------------------------------------------- glUniformMatrix2fv
void YAC_CALL _glUniformMatrix2fv(sSI _location, sSI _count, sSI _bTranspose, YAC_Object *_fa) {

   int_glUniformMatrixfv_prepare_array(2*2, _count, _fa);

   _glUniformMatrix2fv_int(_location, _count, _bTranspose, int_glUniformMatrixfv_array);
}

// ---------------------------------------------------------------------------- glUniformMatrix2fv_int
void YAC_CALL _glUniformMatrix2fv_int(GLint _location, GLsizei _count, GLboolean _transpose, const GLfloat *_value) {

   Dcheckext(glUniformMatrix2fv)
   {
      glUniformMatrix2fv_xref(_location, _count, _transpose, _value);

      Dtraceglerror("glUniformMatrix2fv");
   }
}

// ---------------------------------------------------------------------------- glUniformMatrix3fv
void YAC_CALL _glUniformMatrix3fv(sSI _location, sSI _count, sSI _bTranspose, YAC_Object *_fa) {

   int_glUniformMatrixfv_prepare_array(3*3, _count, _fa);

   _glUniformMatrix3fv_int(_location, _count, _bTranspose, int_glUniformMatrixfv_array);
}

// ---------------------------------------------------------------------------- glUniformMatrix3fv_int
void YAC_CALL _glUniformMatrix3fv_int(GLint _location, GLsizei _count, GLboolean _transpose, const GLfloat *_value) {

   Dcheckext(glUniformMatrix3fv)
   {
      glUniformMatrix3fv_xref(_location, _count, _transpose, _value);

      Dtraceglerror("glUniformMatrix3fv");
   }
}

// ---------------------------------------------------------------------------- glUniformMatrix4fv
void YAC_CALL _glUniformMatrix4fv(sSI _location, sSI _count, sSI _bTranspose, YAC_Object *_fa) {
   int_glUniformMatrixfv_prepare_array(4*4, _count, _fa);

   _glUniformMatrix4fv_int(_location, _count, _bTranspose, int_glUniformMatrixfv_array);
}

// ---------------------------------------------------------------------------- glUniformMatrix4fv_int
void YAC_CALL _glUniformMatrix4fv_int(GLint _location, GLsizei _count, GLboolean _transpose, const GLfloat *_value) {

   Dcheckext(glUniformMatrix4fv)
   {
      glUniformMatrix4fv_xref(_location, _count, _transpose, _value);

      Dtraceglerror("glUniformMatrix4fv");
   }
}

// ---------------------------------------------------------------------------- zglGenSampler
sUI YAC_CALL _zglGenSampler(void) {
   sUI r = 0;
#ifdef DX_GLES
   // (todo) gles
   Dstub(zglGenSampler);
#else
   Dcheckext(glGenSamplers)
   {
      glGenSamplers_xref(1, &r);

      Dtraceglerror("glGenSamplers");
   }
#endif // DX_GLES
   return r;
}

// ---------------------------------------------------------------------------- zglDeleteSampler
void YAC_CALL _zglDeleteSampler(sUI _id) {
#ifdef DX_GLES
   // (todo) gles
   Dstub(zglDeleteSampler);
#else
   Dcheckext(glDeleteSamplers)
   {
      glDeleteSamplers_xref(1, &_id);

      Dtraceglerror("glDeleteSamplers");
   }
#endif // DX_GLES
}

// ---------------------------------------------------------------------------- glBindSampler
void YAC_CALL _glBindSampler(sUI _unit, sUI _sampler) {
#ifdef DX_GLES
   // (todo) gles
   Dstub(glBindSampler);
#else
   Dcheckext(glBindSampler)
   {
      glBindSampler_xref(_unit, _sampler);

      Dtraceglerror("glBindSampler");
   }
#endif // DX_GLES
}

// ---------------------------------------------------------------------------- glSamplerParameteri
void YAC_CALL _glSamplerParameteri(sUI _sampler, sSI _pname, sSI _param) {
#ifdef DX_GLES
   // (todo) gles
   Dstub(glSamplerParameteri);
#else
   Dcheckext(glSamplerParameteri)
   {
      glSamplerParameteri_xref(_sampler, (GLenum)_pname, _param);

      Dtraceglerror("glSamplerParameteri");
   }
#endif // DX_GLES
}


// ---------------------------------------------------------------------------- glBindVertexArray
void YAC_CALL _glBindVertexArray(sUI _array) {
   Dcheckext(glBindVertexArray)
   {
      // Dyac_host_printf("xxx glBindVertexArray(array=%u)\n", _array);
      glBindVertexArray_xref(_array);

      tkopengl_current_vao_id = _array;

      Dtraceglerror("glBindVertexArray");
   }
}

// ---------------------------------------------------------------------------- zglDeleteVertexArray
void YAC_CALL _zglDeleteVertexArray(sUI _array) {
   Dcheckext(glDeleteVertexArrays)
   {
      glDeleteVertexArrays_xref(1, &_array);

      Dtraceglerror("glDeleteVertexArrays");
   }
}

// ---------------------------------------------------------------------------- zglGenVertexArray
sUI YAC_CALL _zglGenVertexArray(void) {
   sUI retArray = 0u;
   Dcheckext(glGenVertexArrays)
   {
      glGenVertexArrays_xref(1, &retArray);
      // yac_host->printf("xxx glGenVertexArrays_xref=%p retArray=%u\n", glGenVertexArrays_xref, retArray);

      Dtraceglerror("glGenVertexArrays");
   }
   return retArray;
}

// ---------------------------------------------------------------------------- glIsVertexArray
sBool YAC_CALL _glIsVertexArray(sUI _array) {
   sBool ret = YAC_FALSE;
   Dcheckext(glIsVertexArray)
   {
      glIsVertexArray_xref(_array);

      Dtraceglerror("glIsVertexArray");
   }
   return ret;
}

// ---------------------------------------------------------------------------- glVertexAttribDivisor
void YAC_CALL _glVertexAttribDivisor(sUI index, sUI divisor) {
   Dcheckext(glVertexAttribDivisor)
   {
      // Dyac_host_printf("xxx call glVertexAttribDivisor(index=%u divisor=%u)\n", index, divisor);
      glVertexAttribDivisor_xref(index, divisor);

      Dtraceglerror("glVertexAttribDivisor");
   }
}



//
//
// GLU
//
//

// ---------------------------------------------------------------------------- zgluLookAt
void YAC_CALL _zgluLookAt(YAC_Object *_eye, YAC_Object *_center, YAC_Object *_up) {
	if(YAC_VALID(_eye))
	if(YAC_VALID(_center))
	if(YAC_VALID(_up))
	{
      if(
         (_eye->yacArrayGetNumElements() >= 3)    &&
         (_center->yacArrayGetNumElements() >= 3) &&
         (_up->yacArrayGetNumElements() >= 3)
         )
      {
         float eyeX, eyeY, eyeZ;
         float centerX, centerY, centerZ;
         float upX, upY, upZ;
         YAC_Value r;
#define Dreadelt(o,d,i) (o)->yacArrayGet(NULL, i, &r); r.typecast(YAC_TYPE_FLOAT); d = r.value.float_val
         Dreadelt(_eye, eyeX, 0);
         Dreadelt(_eye, eyeY, 1);
         Dreadelt(_eye, eyeZ, 2);
         Dreadelt(_center, centerX, 0);
         Dreadelt(_center, centerY, 1);
         Dreadelt(_center, centerZ, 2);
         Dreadelt(_up, upX, 0);
         Dreadelt(_up, upY, 1);
         Dreadelt(_up, upZ, 2);
#undef Dreadelt

#ifdef DX_EMU_MATRIX
         sF32 tm[16];
         sF32 f[3];
         float up[3];
         float s[3];
         float u[3];

         f[0] = (centerX - eyeX);
         f[1] = (centerY - eyeY);
         f[2] = (centerZ - eyeZ);

         // normalize f
         sF32 obflen = 1.0f / ((sF32)sqrtf(f[0]*f[0] + f[1]*f[1] + f[2]*f[2]));
         f[0] *= obflen;
         f[1] *= obflen;
         f[2] *= obflen;

         // normalize up
         float obuplen = 1.0f / ((sF32)sqrtf(upX*upX + upY*upY + upZ*upZ));
         up[0] = upX * obuplen;
         up[1] = upY * obuplen;
         up[2] = upZ * obuplen;

         // s = f X up
         s[0] = (f[1]*up[2] - f[2]*up[1]);
         s[1] = (f[2]*up[0] - f[0]*up[2]);
         s[2] = (f[0]*up[1] - f[1]*up[0]);

         // u = s X f
         u[0] = (s[1]*f[2] - s[2]*f[1]);
         u[1] = (s[2]*f[0] - s[0]*f[2]);
         u[2] = (s[0]*f[1] - s[1]*f[0]);

         tm[ 0] = s[0];  tm[ 4] = s[1];  tm[ 8] = s[2];  tm[12] = 0;
         tm[ 1] = u[0];  tm[ 5] = u[1];  tm[ 9] = u[2];  tm[13] = 0;
         tm[ 2] =-f[0];  tm[ 6] =-f[1];  tm[10] =-f[2];  tm[14] = 0;
         tm[ 3] = 0   ;  tm[ 7] =  0  ;  tm[11] = 0   ;  tm[15] = 1.0f;

         tkopengl_matrix_mul_inplace(tkopengl_matrix, tm);
         tkopengl_matrix_translate(tkopengl_matrix, -eyeX, -eyeY, -eyeZ);

         tkopengl_matrix_flags[tkopengl_matrix_mode] |= TKOPENGL_MATRIX_FLAG_DIRTY;
         tkopengl_matrix_flags[tkopengl_matrix_mode] &= ~TKOPENGL_MATRIX_FLAG_IDENTITY;
         // (note) still orthogonal
#else
#ifdef DX_GLES
         (void)eyeX;
         (void)eyeY;
         (void)eyeZ;
         (void)centerX;
         (void)centerY;
         (void)centerZ;
         (void)upX;
         (void)upY;
         (void)upZ;
         Dstub(zgluLookAt);
#else
         ::gluLookAt(eyeX, eyeY, eyeZ,
                     centerX, centerY, centerZ,
                     upX, upY, upZ
                     );
         Dtraceglerror("zgluLookAt");
#endif // DX_GLES
#endif // DX_EMU_MATRIX
      }
   }
}

// ---------------------------------------------------------------------------- unproject_int
void YAC_CALL _unproject_int(sF32 _x, sF32 _y,
                             sF32 *_nearx, sF32 *_neary, sF32 *_nearz,
                             sF32 *_farx, sF32 *_fary, sF32 *_farz
                             ) {
#ifdef DX_EMU_MATRIX
   // (todo) matrix
   Dstub(unproject_int);
#else
#ifdef DX_GLES
   Dstub(unproject);
#else
   GLdouble mvmatrix[4*4];
   GLdouble prmatrix[4*4];
   ::glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
   ::glGetDoublev(GL_PROJECTION_MATRIX, prmatrix);
   GLdouble x = (GLdouble)_x;

   GLint view[4];
   ::glGetIntegerv(GL_VIEWPORT, view);
   GLdouble y=(GLdouble)view[3]/*tks_HAL->view_sx??*/-_y;

   GLdouble rx,ry,rz;

   ::gluUnProject(x, y, 0, mvmatrix, prmatrix, view, &rx, &ry, &rz);
   *_nearx = (sF32)rx;
   *_neary = (sF32)ry;
   *_nearz = (sF32)rz;

   ::gluUnProject(x, y, 1, mvmatrix, prmatrix, view, &rx,&ry,&rz);
   *_farx = (sF32)rx;
   *_fary = (sF32)ry;
   *_farz = (sF32)rz;
#endif // DX_GLES

#endif //  DX_EMU_MATRIX
}

// ---------------------------------------------------------------------------- project_int
void YAC_CALL _project_int(sF32 _x, sF32 _y, sF32 _z,
                           sF32 *_px, sF32 *_py, sF32 *_pz
                           ) {
#ifdef DX_EMU_MATRIX
   sF32 tm[16];
   sF32 *m = &tkopengl_projection_matrix_stack[tkopengl_projection_matrix_stacki << 4];

   // multiply projection matrix by modelview matrix
   tkopengl_matrix_mul(tm, m, &tkopengl_modelview_matrix_stack[tkopengl_modelview_matrix_stacki << 4]);

   GLint view[4];
   ::glGetIntegerv(GL_VIEWPORT, view);

   // transform vector to window coordinates
   *_px = (tm[ 0] * _x) + (tm[ 4] * _y) + (tm[ 8] * _z) + (tm[12]);
   *_py = (tm[ 1] * _x) + (tm[ 5] * _y) + (tm[ 9] * _z) + (tm[13]);
   *_pz = (tm[ 2] * _x) + (tm[ 6] * _y) + (tm[10] * _z) + (tm[14]);

   *_px = view[0] + view[2] * (*_px + 1.0f) * 0.5f;
   *_py = view[1] + view[3] * (*_py + 1.0f) * 0.5f;
   *_pz = (*_pz + 1.0f) * 0.5f;

#else
#ifdef DX_GLES
   Dstub(project);
#else
   GLdouble prjmatrix[4*4];
   GLdouble mvmatrix[4*4];
   GLint view[4];
   ::glGetDoublev(GL_PROJECTION_MATRIX, prjmatrix);
   ::glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
   ::glGetIntegerv(GL_VIEWPORT, view);
   // project mmm
   GLdouble rx;
   GLdouble ry;
   GLdouble rz;
   ::gluProject((GLdouble)_x, (GLdouble)_y, (GLdouble)_z, mvmatrix, prjmatrix, view, &rx, &ry, &rz);
   *_px=(sF32)rx;
   *_py=(sF32)ry;
   *_pz=(sF32)rz;
#endif // DX_GLES
#endif // DX_EMU_MATRIX
}

//
//
// Display lists
//
//

// ---------------------------------------------------------------------------- glGenLists
GLuint YAC_CALL _glGenLists(GLsizei _range) {
   GLuint r = 0;
#ifdef DX_EMU_DLIST
   Dstub(glGenLists);
#else
#ifdef DX_GLES
   Dstub(glGenLists);
#else
   Dcompat(glGenLists)
   {
      r = ::glGenLists(_range);
      Dtraceglerror("glGenLists");
   }
#endif // DX_GLES
#endif // DX_EMU_DLIST
   return r;
}

// ---------------------------------------------------------------------------- glDeleteLists
void YAC_CALL _glDeleteLists(GLuint _list, GLsizei _range) {
#ifdef DX_EMU_DLIST
   Dstub(glDeleteLists);
#else
#ifdef DX_GLES
   Dstub(glDeleteLists);
#else
   Dcompat(glDeleteLists)
   {
      ::glDeleteLists(_list, _range);
      Dtraceglerror("glDeleteLists");
   }
#endif // DX_GLES
#endif // DX_EMU_DLIST
}

// ---------------------------------------------------------------------------- glNewList
void YAC_CALL _glNewList(GLuint _list, GLenum _mode) {
#ifdef DX_EMU_DLIST
   Dstub(glNewList);
#else
#ifdef DX_GLES
   Dstub(glNewList);
#else
   Dcompat(glNewList)
   {
      ::glNewList(_list, _mode);
      Dtraceglerror("glNewList");
   }
#endif // DX_GLES
#endif // DX_EMU_DLIST
}

// ---------------------------------------------------------------------------- glEndList
void YAC_CALL _glEndList(void) {
#ifdef DX_EMU_DLIST
   Dstub(glEndList);
#else
#ifdef DX_GLES
   Dstub(glEndList);
#else
   ::glEndList();
   Dtraceglerror("glEndList");
#endif // DX_GLES
#endif // DX_EMU_DLIST
}

// ---------------------------------------------------------------------------- glCallList
void YAC_CALL _glCallList(sSI _a) {
#ifdef DX_EMU_DLIST
   Dstub(glCallList);
#else
#ifdef DX_GLES
   Dstub(glCallList);
#else
   Dcompat(glCallList)
   {
      ::glCallList(_a);
      Dtraceglerror("glCallList");
   }
#endif // DX_GLES
#endif // DX_EMU_DLIST
}
