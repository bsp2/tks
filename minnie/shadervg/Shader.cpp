// ----
// ---- file   : Shader.cpp
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : Distributed under terms of the MIT license (https://opensource.org/licenses/MIT)
// ----          Copyright 2014-2025 by bsp
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
// ---- info   : ShaderVG render utilities
// ---- note   :
// ----
// ----
// ----

#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#define YAC_BIGSTRING
#define YAC_PRINTF
#include <yac.h>

// #define MINNIE_SKIP_TYPEDEFS  defined
// #include "../minnie.h"

#include "../inc_opengl.h"

#include "shadervg.h"
#include "shadervg_internal.h"
#include "Shader.h"


ShaderVG_Shader::ShaderVG_Shader(void) {
   prg_id = 0u;
}

ShaderVG_Shader::~ShaderVG_Shader() {
#if 0
   // causes GL errors when GL context has already been destroyed (e.g. in OnOpen())
   destroy();
#endif
}

sBool ShaderVG_Shader::create(const char *_sVert, const char *_sFrag) {
#define MAX_SHADER_LOG_SIZE  4096
   sBool ret = YAC_FALSE;

   // Dsdvg_printf("xxx ShaderVG_Shader::create: ENTER\n");

#if 0
   // causes GL errors when GL context has already been destroyed (e.g. in OnOpen())
   if(prg_id > 0u)
      destroy();
#endif

   YAC_String sVert; sVert.visit(_sVert);
   YAC_String sFrag; sFrag.visit(_sFrag);
   YAC_String *sVertFix = YAC_New_String();  // alloc host-side String for yacStringReplace()
   YAC_String *sFragFix = YAC_New_String();
   FixShaderSourceVert(&sVert, sVertFix);
   FixShaderSourceFrag(&sFrag, sFragFix);

   // Create vertex shader
   sUI vpId = Dsdvg_glcall(glCreateShader(GL_VERTEX_SHADER));

   if(Dyac_host_yacGetDebugLevel() >= 20u)
   {
      Dsdvg_debugprintfv("[trc] ShaderVG_Shader::create: vpSource=\"%s\"\n", (const char*)sVertFix->chars);
   }

   const char *sVertFixChars = (const char*)sVertFix->chars;
   Dsdvg_glcall(glShaderSource(vpId, 1, &sVertFixChars, NULL/*lengths*/));

   // Create fragment shader
   sUI fpId = Dsdvg_glcall(glCreateShader(GL_FRAGMENT_SHADER));

   if(Dyac_host_yacGetDebugLevel() >= 20u)
   {
      Dsdvg_debugprintfv("[trc] ShaderVG_Shader::create: fpSource:\n\"%s\"\n", (const char*)sFragFix->chars);
   }

   const char *sFragFixChars = (const char*)sFragFix->chars;
   Dsdvg_glcall(glShaderSource(fpId, 1, &sFragFixChars, NULL/*lengths*/));

   // Create program
   prg_id = Dsdvg_glcall(glCreateProgram());

   Dsdvg_glcall(glAttachShader(prg_id, vpId));
   Dsdvg_glcall(glAttachShader(prg_id, fpId));

   /* Compile and attach shaders */
   Dsdvg_glcall(glCompileShader(vpId));
   {
      GLint val = 0;
      char buf[MAX_SHADER_LOG_SIZE];
      buf[0] = 0;

      Dsdvg_glcall(glGetShaderiv(vpId, GL_COMPILE_STATUS, &val));

      Dsdvg_glcall(glGetShaderInfoLog(vpId, MAX_SHADER_LOG_SIZE, NULL, buf));

      if(0 != buf[0])
      {
         Dsdvg_warnprintf("[~~~] ShaderVG_Shader::create: glGetShaderInfoLog[VS] returned:\n");
         Dsdvg_warnprintf(buf);
      }
   }

   Dsdvg_glcall(glCompileShader(fpId));
   {
      GLint val = 0;
      char buf[MAX_SHADER_LOG_SIZE];
      buf[0] = 0;

      Dsdvg_glcall(glGetShaderiv(fpId, GL_COMPILE_STATUS, &val));

      Dsdvg_glcall(glGetShaderInfoLog(fpId, MAX_SHADER_LOG_SIZE, NULL/*length*/, buf));

      if(0 != buf[0])
      {
         Dsdvg_warnprintf("[~~~] ShaderVG_Shader::create: glGetShaderInfoLog[FS] returned:\n");
         Dsdvg_warnprintf(buf);
      }
   }

   // Link program
   Dsdvg_glcall(glLinkProgram(prg_id));
   {
      GLint val = 0;

      Dsdvg_glcall(glGetProgramiv(prg_id, GL_LINK_STATUS, &val));

      if(GL_TRUE == val)
      {
         if(Dyac_host_yacGetDebugLevel() >= 2u)
         {
            Dsdvg_debugprintf("[dbg] ShaderVG_Shader::create: ok, program linked successfully (prg_id=%u).\n", prg_id);
         }

         // Succeeded
         ret = YAC_TRUE;
      }
      else
      {
         Dsdvg_errorprintf("[---] ShaderVG_Shader::create: FAILED to link program.\n");
         {
            char buf[MAX_SHADER_LOG_SIZE];
            buf[0] = 0;

            Dsdvg_glcall(glGetProgramInfoLog(prg_id, MAX_SHADER_LOG_SIZE, NULL, buf));

            if(0 != buf[0])
            {
               Dsdvg_warnprintf("[~~~] ShaderVG_Shader::create: glGetProgramInfoLog returned:\n");
               Dsdvg_warnprintf(buf);
            }
         }

         // Failed
      }
   }

   // (note) decrements ref-count (shaders will still be referenced by program)
   Dsdvg_glcall(glDeleteShader(vpId));
   Dsdvg_glcall(glDeleteShader(fpId));

   YAC_DELETE(sVertFix);
   YAC_DELETE(sFragFix);

   // Dsdvg_printf("xxx ShaderVG_Shader::create: LEAVE prg_id=%u\n", prg_id);

   return ret;
#undef MAX_SHADER_LOG_SIZE
}

void ShaderVG_Shader::bind(void) {
   Dsdvg_debugprintfv("[trc] ShaderVG_Shader::bind: prg_id=%u\n", prg_id);
   Dsdvg_glcall(glUseProgram(prg_id));
}

void ShaderVG_Shader::unbind(void) {
   Dsdvg_glcall(glUseProgram(0u));
}

void ShaderVG_Shader::destroy(void) {
   if(prg_id > 0u)
   {
      Dsdvg_glcall(glUseProgram(0u));
      Dsdvg_glcall(glDeleteProgram(prg_id));
      prg_id = 0u;
   }
}

sSI ShaderVG_Shader::getUniformLocation(const char *_name) {
   return Dsdvg_glcall(glGetUniformLocation(prg_id, _name));
}

sSI ShaderVG_Shader::getAttribLocation(const char *_name) {
   return Dsdvg_glcall(glGetAttribLocation(prg_id, _name));
}

#if 0
sUI ShaderVG_Shader::getProgramId(void) {
   return prg_id;
}
#endif
