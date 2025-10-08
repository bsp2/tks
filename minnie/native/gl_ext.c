// ----
// ---- file   : gl_ext.cpp
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : Distributed under terms of the MIT license (https://opensource.org/licenses/MIT)
// ----          Copyright 2001-2025 by bsp
// ----
// ----          Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
// ----          associated documentation files (the "Software"), to deal in the Software without restriction, including
// ----          without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// ----          copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to
// ----          the following conditions:
// ----
// ----          The above copyright notice and this permission notice shall be included in all copies or substantial
// ----          portions of the Software.
// ----
// ----          THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
// ----          NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// ----          IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// ----          WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// ----          SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// ----
// ---- info   :
// ---- note   :
// ----
// ----
// ----

#include <stdio.h>

#include "../inc_yac.h"
#include "../inc_opengl.h"


typedef void      (APIENTRY *glanyfun_t)              (void);

static glanyfun_t int_GetProcAddress(const char *_name) {
#ifdef YAC_WIN32
   return (glanyfun_t) wglGetProcAddress(_name);
#elif defined(YAC_MACOS)
   return (glanyfun_t) MyNSGLGetProcAddress(_name);
#else
   return (glanyfun_t) glXGetProcAddress((const GLubyte*)_name);
#endif // YAC_WIN32
}

glBindBuffer_t                     glBindBuffer                       = NULL;
glIsBuffer_t                       glIsBuffer                         = NULL;
glDeleteBuffers_t                  glDeleteBuffers                    = NULL;
glGenBuffers_t                     glGenBuffers                       = NULL;
glBufferData_t                     glBufferData                       = NULL;
glBufferSubData_t                  glBufferSubData                    = NULL;
glDrawArraysInstanced_t            glDrawArraysInstanced              = NULL;
glGetBufferParameteriv_t           glGetBufferParameteriv             = NULL;
glMapBuffer_t                      glMapBuffer                        = NULL;
glMapBufferRange_t                 glMapBufferRange                   = NULL;
glFlushMappedBufferRange_t         glFlushMappedBufferRange           = NULL;
glUnmapBuffer_t                    glUnmapBuffer                      = NULL;
glActiveTexture_t                  glActiveTexture                    = NULL;
glBlendFuncSeparate_t              glBlendFuncSeparate                = NULL;
glBindRenderbuffer_t               glBindRenderbuffer                 = NULL;
glDeleteRenderbuffers_t            glDeleteRenderbuffers              = NULL;
glGenRenderbuffers_t               glGenRenderbuffers                 = NULL;
glBindFramebuffer_t                glBindFramebuffer                  = NULL;
glDeleteFramebuffers_t             glDeleteFramebuffers               = NULL;
glGenFramebuffers_t                glGenFramebuffers                  = NULL;
glCheckFramebufferStatus_t         glCheckFramebufferStatus           = NULL;
glFramebufferTexture2D_t           glFramebufferTexture2D             = NULL;
glBlitFramebuffer_t                glBlitFramebuffer                  = NULL;
glCreateProgram_t                  glCreateProgram                    = NULL;
glDeleteProgram_t                  glDeleteProgram                    = NULL;
glCreateShader_t                   glCreateShader                     = NULL;
glDeleteShader_t                   glDeleteShader                     = NULL;
glShaderSource_t                   glShaderSource                     = NULL;
glCompileShader_t                  glCompileShader                    = NULL;
glAttachShader_t                   glAttachShader                     = NULL;
glDetachShader_t                   glDetachShader                     = NULL;
glLinkProgram_t                    glLinkProgram                      = NULL;
glValidateProgram_t                glValidateProgram                  = NULL;
glGetShaderiv_t                    glGetShaderiv                      = NULL;
glGetShaderInfoLog_t               glGetShaderInfoLog                 = NULL;
glGetProgramInfoLog_t              glGetProgramInfoLog                = NULL;
glGetProgramiv_t                   glGetProgramiv                     = NULL;
glUseProgram_t                     glUseProgram                       = NULL;
glBindAttribLocation_t             glBindAttribLocation               = NULL;
glGetAttribLocation_t              glGetAttribLocation                = NULL;
glBindFragDataLocation_t           glBindFragDataLocation             = NULL;
glBindFragDataLocationIndexed_t    glBindFragDataLocationIndexed      = NULL;
glGetFragDataLocation_t            glGetFragDataLocation              = NULL;
glGetFragDataIndex_t               glGetFragDataIndex                 = NULL;
glVertexAttrib1f_t                 glVertexAttrib1f                   = NULL;
glVertexAttrib2f_t                 glVertexAttrib2f                   = NULL;
glVertexAttrib3f_t                 glVertexAttrib3f                   = NULL;
glVertexAttrib4f_t                 glVertexAttrib4f                   = NULL;
glVertexAttrib4Nub_t               glVertexAttrib4Nub                 = NULL;
glVertexAttrib2fv_t                glVertexAttrib2fv                  = NULL;
glVertexAttrib3fv_t                glVertexAttrib3fv                  = NULL;
glVertexAttrib4fv_t                glVertexAttrib4fv                  = NULL;
glVertexAttribPointer_t            glVertexAttribPointer              = NULL;
glEnableVertexAttribArray_t        glEnableVertexAttribArray          = NULL;
glDisableVertexAttribArray_t       glDisableVertexAttribArray         = NULL;
glBindVertexArray_t                glBindVertexArray                  = NULL;
glDeleteVertexArrays_t             glDeleteVertexArrays               = NULL;
glGenVertexArrays_t                glGenVertexArrays                  = NULL;
glIsVertexArray_t                  glIsVertexArray                    = NULL;
glVertexAttribDivisor_t            glVertexAttribDivisor              = NULL;
glGetUniformLocation_t             glGetUniformLocation               = NULL;
glGetActiveUniform_t               glGetActiveUniform                 = NULL;
glUniform1f_t                      glUniform1f                        = NULL;
glUniform2f_t                      glUniform2f                        = NULL;
glUniform3f_t                      glUniform3f                        = NULL;
glUniform4f_t                      glUniform4f                        = NULL;
glUniform2fv_t                     glUniform2fv                       = NULL;
glUniform3fv_t                     glUniform3fv                       = NULL;
glUniform4fv_t                     glUniform4fv                       = NULL;
glUniform1i_t                      glUniform1i                        = NULL;
glUniform2i_t                      glUniform2i                        = NULL;
glUniform3i_t                      glUniform3i                        = NULL;
glUniform4i_t                      glUniform4i                        = NULL;
glUniformMatrix2fv_t               glUniformMatrix2fv                 = NULL;
glUniformMatrix3fv_t               glUniformMatrix3fv                 = NULL;
glUniformMatrix4fv_t               glUniformMatrix4fv                 = NULL;
glRenderbufferStorage_t            glRenderbufferStorage              = NULL;
glFramebufferRenderbuffer_t        glFramebufferRenderbuffer          = NULL;
glRenderbufferStorageMultisample_t glRenderbufferStorageMultisample   = NULL;

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
      printf("[---] failed to resolve extension fxn \"%s\" / \"%s\" / \"%s\".\n", _name1, _name2, _name3);
   }

   return r;
}

#define Dresolveext(a) a## = (a##_t) int_GetProcAddressA3(#a, #a "ARB", #a "EXT")

void load_gl_extensions(void) {
   Dresolveext(glBindBuffer);
   Dresolveext(glIsBuffer);
   Dresolveext(glDeleteBuffers);
   Dresolveext(glGenBuffers);
   Dresolveext(glBufferData);
   Dresolveext(glBufferSubData);
   Dresolveext(glDrawArraysInstanced);
   Dresolveext(glGetBufferParameteriv);
   Dresolveext(glMapBuffer);
   Dresolveext(glMapBufferRange);
   Dresolveext(glFlushMappedBufferRange);
   Dresolveext(glUnmapBuffer);
   Dresolveext(glActiveTexture);
   Dresolveext(glBlendFuncSeparate);
   Dresolveext(glBindRenderbuffer);
   Dresolveext(glDeleteRenderbuffers);
   Dresolveext(glGenRenderbuffers);
   Dresolveext(glBindFramebuffer);
   Dresolveext(glDeleteFramebuffers);
   Dresolveext(glGenFramebuffers);
   Dresolveext(glCheckFramebufferStatus);
   Dresolveext(glFramebufferTexture2D);
   Dresolveext(glBlitFramebuffer);
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
   Dresolveext(glBindVertexArray);
   Dresolveext(glDeleteVertexArrays);
   Dresolveext(glGenVertexArrays);
   Dresolveext(glIsVertexArray);
   Dresolveext(glVertexAttribDivisor);
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
   Dresolveext(glRenderbufferStorage);
   Dresolveext(glFramebufferRenderbuffer);
   Dresolveext(glRenderbufferStorageMultisample);
   // printf("xxx load_gl_extensions: LEAVE  glBindBuffer=%p glGenVertexArrays=%p\n", glBindBuffer, glGenVertexArrays);
}
