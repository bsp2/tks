/// shader.cpp
///
/// (c) 2013-2025 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

//#define SHADER_DEBUG defined


// http://www.khronos.org/opengles/sdk/docs/man/

#include <stdlib.h>
#include <math.h>
#include <string.h>

#define YAC_PRINTF defined
#include <yac.h>

extern YAC_Host *yac_host;

#include "inc_opengl.h"

#include "tkopengl.h"

#include "glerror.h"

#include "tks-list.h"

#include "bitmap.h"
#include "image.h"
#include "tkstexture.h"
#include "tksopengl.h"


#ifdef DX_EMU_FIXFXN

#include "attrib.h"
#include "matrix.h"
#include "immediate.h"
#include "shader.h"

sBool b_tkopengl_custom_shader;

#define Dattrvalid(a) (current_shader->attr_map[a] >= 0)


// ---------------------------------------------------------------------------- shader enumeration
static sUI num_shaders;


static gles_shader_t shaders[TKOPENGL_MAX_SHADERS];

static gles_shader_t *current_shader;

#else
#include "shader.h"

#endif // DX_EMU_FIXFXN

/* ----------------------------------------------------------------------------- gles_shader_destroy() */
static void gles_shader_destroy(gles_shader_t *_shader) {
   if(_shader->prg_id > 0u)
   {
      _glUseProgram_int(0);

      _glDetachShader(_shader->prg_id, _shader->fp_id);

      _glDetachShader(_shader->prg_id, _shader->vp_id);

      _glDeleteProgram(_shader->prg_id);
      _shader->prg_id = 0u;

      _glDeleteShader(_shader->fp_id);
      _shader->fp_id = 0u;

      _glDeleteShader(_shader->vp_id);
      _shader->vp_id = 0u;
   }
}


/* ----------------------------------------------------------------------------- gles_shader_bind() */
static void gles_shader_bind(gles_shader_t *_shader) {
   _glUseProgram_int(_shader->prg_id);
}


/* ----------------------------------------------------------------------------- gles_shader_uid_get() */
#if 0
static int gles_shader_uid_get(gles_shader_t *_shader,
                               const char *_uniformName,
                               GLint *_retUniformId
                               ) {

   *_retUniformId = _glGetUniformLocation_int(_shader->prg_id,
                                              _uniformName
                                              );
   return 0;
}
#endif

/* ----------------------------------------------------------------------------- gles_shader_create() */
static sSI gles_shader_create(gles_shader_t *_shader,
                              const sChar *_vpSource,
                              const sChar *_fpSource
                              ) {
   sSI ret;

#ifdef DX_EMU_FIXFXN
   memset(&_shader->attr_map, -1, sizeof(GLint) * NUM_ATTRIBS);
   memset(&_shader->uni_map, -1, sizeof(GLint) * NUM_UNIFORMS);
#endif // DX_EMU_FIXFXN

   if(Dyac_host_yacGetDebugLevel() >= 10u)
   {
      Dyac_host_printf("xxx gles_shader_create:\nxxx vpSource=%s\n\nxxx fpSource=%s\n", (const char*)_vpSource, (const char*)_fpSource);
   }

   /* Create vertex shader */
   _shader->vp_id = _glCreateShader(GL_VERTEX_SHADER);

   if(Dyac_host_yacGetDebugLevel() >= 2u)
   {
      yac_host->printf("xxx vpSource=\"%s\"\n", _vpSource);
   }

   _glShaderSource_int(_shader->vp_id, 1, &_vpSource, NULL /* lengths */);

   /* Create fragment shader */
   _shader->fp_id = _glCreateShader(GL_FRAGMENT_SHADER);

   if(Dyac_host_yacGetDebugLevel() >= 2u)
   {
      yac_host->printf("xxx fpSource:\n\"%s\"\n", _fpSource);
   }

   _glShaderSource_int(_shader->fp_id, 1, &_fpSource, NULL /* lengths */);


   /* Create program */
   _shader->prg_id = _glCreateProgram();

   _glAttachShader(_shader->prg_id, _shader->vp_id);

   _glAttachShader(_shader->prg_id, _shader->fp_id);

   /* Compile and attach shaders */
   _glCompileShader(_shader->vp_id);

   {
      GLint val;
      char buf[4096];

      buf[0] = 0;

      _glGetShaderiv_int(_shader->vp_id, GL_COMPILE_STATUS, &val);

      _glGetShaderInfoLog_int(_shader->vp_id, 4096, NULL, buf);

      if(0 != buf[0])
      {
         yac_host->printf("[~~~] tkopengl::gles_shader_create: glGetShaderInfoLog[VS] returned:\n");
         yac_host->printf(buf);
      }
   }

   _glCompileShader(_shader->fp_id);

   {
      GLint val;
      char buf[4096];

      _glGetShaderiv_int(_shader->fp_id, GL_COMPILE_STATUS, &val);

      _glGetShaderInfoLog_int(_shader->fp_id, 4096, NULL, buf);

      if(0 != buf[0])
      {
         yac_host->printf("[~~~] tkopengl::gles_shader_create: glGetShaderInfoLog[FS] returned:\n");
         yac_host->printf(buf);
      }
   }


#ifdef DX_EMU_FIXFXN
#ifdef DX_AMD_ATTRIB_WORKAROUND
   // AMD/ATI driver bug workaround
   //   (glVertexAttrib*f() for attrib location 0 assigned by driver does not work, this exact mapping must be used instead)
   _glBindAttribLocation_int(_shader->prg_id, 0, "a_zgl_vertex");
   _glBindAttribLocation_int(_shader->prg_id, 1, "a_zgl_normal");
   _glBindAttribLocation_int(_shader->prg_id, 2, "a_zgl_color");
   _glBindAttribLocation_int(_shader->prg_id, 3, "a_zgl_texcoord");
#endif // DX_AMD_ATTRIB_WORKAROUND
#endif // DX_EMU_FIXFXN

   /* Link program */
   _glLinkProgram(_shader->prg_id);

   {
      GLint val;

      _glGetProgramiv_int(_shader->prg_id, GL_LINK_STATUS, &val);

      if(GL_TRUE == val)
      {
#ifdef SHADER_DEBUG
         yac_host->printf("[...] tkopengl::gles_shader_create: ok, program linked successfully.\n");
#endif

         /* Succeeded */
         ret = 0;
      }
      else
      {
         yac_host->printf("[---] tkopengl::gles_shader_create: FAILED to link program.\n");

         {
            char buf[4096];

            buf[0] = 0;

            _glGetProgramInfoLog_int(_shader->prg_id, 4096, NULL, buf);

            if(0 != buf[0])
            {
               yac_host->printf("[~~~] tkopengl::gles_shader_create: glGetProgramInfoLog returned:\n");
               yac_host->printf(buf);
            }
         }

         /* Failed */
         ret = 10;
      }
   }

   return ret;
}

/* ----------------------------------------------------------------------------- shader_src_gen() */
#ifdef DX_EMU_FIXFXN
static void loc_shader_src_gen(gles_shader_t *_shader, YAC_String *_vpSrc, YAC_String *_fpSrc) {

   // (note) #version 120+ deprecates gl_FragData
   // (note) should use in/out instead of varying in recent GLSL versions (130+?)

   // (note) for DX_EMU_TEXTURE_RECTANGLE, it is assumed that the GL_OES_texture_npot/GL_IMG_texture_npot extension is available

#ifdef SHADER_DEBUG
   printf("[dbg] tkopengl::shader_src_gen: vpFeatures=0x%08x fpFeatures=0x%08x\n", _shader->vp_features, _shader->fp_features);
#endif

   sBool bLighting = (0 != (_shader->vp_features & TKOPENGL_VP_FEATURE_LIGHT_MASK));

   if(_ZGLShader::def_version_override.length <= 1u)
   {
#ifdef DX_GLES
#ifdef DX_GLES_3
      _vpSrc->append("#version 300 es\n");
      _fpSrc->append("#version 300 es\n");
#else
      _vpSrc->append("#version 100\n");
      _fpSrc->append("#version 100\n");
#endif // DX_GLES_3
      _fpSrc->append("precision mediump float;\n");
#endif  // DX_GLES
   }
   else
   {
      _vpSrc->append("#version ");
      _vpSrc->append(&_ZGLShader::def_version_override);
      // Dyac_host_printf("xxx shader ver=\"%s\"\n", _ZGLShader::def_version_override.chars);

      _fpSrc->append("#version ");
      _fpSrc->append(&_ZGLShader::def_version_override);
   }


#if 0
   // (todo) remove (19Jul2025: assume GL4 core profile when emulating compatible API on Desktop GL)
#ifndef DX_GLES
   if(TKOPENGL_FP_FEATURE_TEX_RECT == (_shader->fp_features & TKOPENGL_FP_FEATURE_TEX_MASK))
   {
#ifndef DX_EMU_TEXTURE_RECTANGLE
      _fpSrc->append("#version 140\n");
      _fpSrc->append("#extension GL_ARB_texture_rectangle : enable\n");
#endif
   }
   else if(0 != (_shader->vp_features & TKOPENGL_VP_FEATURE_POINT_SPRITE))
   {
      _fpSrc->append("#version 120\n");
   }
#endif // !DX_GLES
#endif // 0


   // u_zgl_transform (== gl_ModelViewProjectionMatrix)
   _vpSrc->append("uniform mat4 u_zgl_transform;\n");

   // a_zgl_vertex (== gl_Vertex)
#ifdef DX_GLES
   _vpSrc->append(GLSL_ATTRIB_IN " highp vec4 a_zgl_vertex;\n");
#else
   _vpSrc->append(GLSL_ATTRIB_IN " vec4 a_zgl_vertex;\n");
#endif // DX_GLES

   // a_zgl_color (== gl_Color)
   if(0 != (_shader->vp_features & TKOPENGL_VP_FEATURE_COLOR))
   {
#ifdef DX_GLES
      _vpSrc->append(GLSL_ATTRIB_IN " mediump vec4 a_zgl_color;\n");
#else
      _vpSrc->append(GLSL_ATTRIB_IN " vec4 a_zgl_color;\n");
#endif // DX_GLES
   }

   if(0 != (_shader->fp_features & TKOPENGL_FP_FEATURE_COLOR))
   {
      _vpSrc->append(GLSL_VARYING_OUT " vec4 v_zgl_color;\n");
      _fpSrc->append(GLSL_VARYING_IN  " vec4 v_zgl_color;\n");
   }

   if(bLighting)
   {
#ifdef DX_GLES
      _vpSrc->append(GLSL_ATTRIB_IN " mediump vec3 a_zgl_normal;\n");
#else
      _vpSrc->append(GLSL_ATTRIB_IN " vec3 a_zgl_normal;\n");
#endif // DX_GLES
   }

   switch(_shader->fp_features & TKOPENGL_FP_FEATURE_TEX_MASK)
   {
      default:
      case TKOPENGL_FP_FEATURE_TEX_NONE:
         break;
      case TKOPENGL_FP_FEATURE_TEX_1D:
#ifdef DX_EMU_TEXTURE_1D
         // (note) must emulate 1D textures via 2D
         _fpSrc->append("uniform sampler2D u_zgl_sampler;\n");
#else
         _fpSrc->append("uniform sampler1D u_zgl_sampler;\n");
#endif // DX_EMU_TEXTURE_1D
         break;
      case TKOPENGL_FP_FEATURE_TEX_2D:
         _fpSrc->append("uniform sampler2D u_zgl_sampler;\n");
         break;
      case TKOPENGL_FP_FEATURE_TEX_3D:
         _fpSrc->append("uniform sampler3D u_zgl_sampler;\n");
         break;
      case TKOPENGL_FP_FEATURE_TEX_CUBE:
         _fpSrc->append("uniform samplerCube u_zgl_sampler;\n");
         break;
      case TKOPENGL_FP_FEATURE_TEX_RECT:
#ifdef DX_EMU_TEXTURE_RECTANGLE
         _fpSrc->append("uniform sampler2D u_zgl_sampler;\n");
         _vpSrc->append("uniform vec2 u_zgl_obtexsize;\n");
#else
         _fpSrc->append("uniform sampler2DRect u_zgl_sampler;\n");
#endif // DX_EMU_TEXTURE_RECTANGLE
         break;
   }

   // a_zgl_texcoord (== gl_MultiTexCoord0?)
   if(TKOPENGL_VP_FEATURE_TEXGEN_COORD == (_shader->vp_features & TKOPENGL_VP_FEATURE_TEXGEN_MASK))
   {
#ifdef DX_GLES
      _vpSrc->append(GLSL_ATTRIB_IN " highp vec4 a_zgl_texcoord;\n");
#else
      _vpSrc->append(GLSL_ATTRIB_IN " vec4 a_zgl_texcoord;\n");
#endif // DX_GLES
      _vpSrc->append(GLSL_VARYING_OUT " vec4 v_zgl_texcoord;\n");
      _fpSrc->append(GLSL_VARYING_IN  " vec4 v_zgl_texcoord;\n");

      if(0 != (_shader->vp_features & TKOPENGL_VP_FEATURE_TEXTUREMATRIX))
      {
         _vpSrc->append("uniform mat4 u_zgl_texturematrix;\n");
      }
   }

   // texenv color
   if(TKOPENGL_FP_FEATURE_TEXENV_BLEND == (_shader->fp_features & TKOPENGL_FP_FEATURE_TEXENV_MASK))
   {
      _vpSrc->append("uniform vec4 u_zgl_envcolor;\n");
   }

   // alphatest reference value
   if(0 != (_shader->fp_features & TKOPENGL_FP_FEATURE_ALPHATEST_MASK))
   {
      _fpSrc->append("uniform float u_zgl_alpharef;\n");
   }

   // point size
   if(0 != (_shader->vp_features & TKOPENGL_VP_FEATURE_POINT_SIZE))
   {
      _vpSrc->append("uniform float u_zgl_pointsize;\n");

      if(0 != (_shader->vp_features & TKOPENGL_VP_FEATURE_POINT_SPRITE))
      {
         _vpSrc->append("uniform vec2 u_zgl_pointminmax;\n");
         _vpSrc->append("uniform vec3 u_zgl_pointatt;\n");
      }
   }

   // fog
   if(0 != (_shader->vp_features & TKOPENGL_VP_FEATURE_FOG_MASK))
   {
      if(TKOPENGL_VP_FEATURE_FOG_LINEAR == (_shader->vp_features & TKOPENGL_VP_FEATURE_FOG_MASK))
      {
         _vpSrc->append("uniform float u_zgl_fogobdelta;\n"); // LINEAR  (1.0f / (end - start))
         _vpSrc->append("uniform float u_zgl_fogend;\n");     // LINEAR
      }
      else
      {
         _vpSrc->append("uniform float u_zgl_fogdensity;\n"); // EXP, EXP2
      }
      _fpSrc->append("uniform vec4  u_zgl_fogcolor;\n");
      _vpSrc->append(GLSL_VARYING_OUT " float v_zgl_fog;\n");
      _fpSrc->append(GLSL_VARYING_IN  " float v_zgl_fog;\n");
   }

   // light
   if(bLighting)
   {
      _vpSrc->append("uniform mat3 u_zgl_normalmatrix;\n");
      _vpSrc->append("uniform vec4 u_zgl_lightecm;\n");
      if(0 == (_shader->vp_features & TKOPENGL_VP_FEATURE_LIGHTCOLORMATERIAL))
      {
         _vpSrc->append("uniform vec4 u_zgl_lightacm;\n");
         _vpSrc->append("uniform vec4 u_zgl_lightdcm;\n");
      }
      _vpSrc->append("uniform vec4 u_zgl_lightacs;\n");

      if(0 != (_shader->vp_features & TKOPENGL_VP_FEATURE_LIGHTSPECULAR))
      {
         _vpSrc->append("uniform vec4 u_zgl_lightscm;\n");
         _vpSrc->append("uniform float u_zgl_lightsrm;\n");

         if(0 != (_shader->vp_features & TKOPENGL_VP_FEATURE_LIGHTSPECULARSEP))
         {
            _vpSrc->append("varying vec4 v_zgl_lightcol2;\n");
            _fpSrc->append("varying vec4 v_zgl_lightcol2;\n");
         }
      }

      if(
         (0 != (_shader->vp_features & TKOPENGL_VP_FEATURE_LIGHTPOSITIONAL)) || // not direction light?
         (0 != (_shader->vp_features & TKOPENGL_VP_FEATURE_LIGHTLOCALVIEWER))
         )
      {
         _vpSrc->append("uniform mat4 u_zgl_modelview;\n");
      }

      if(0 != (_shader->vp_features & TKOPENGL_VP_FEATURE_LIGHT0))
      {
         _vpSrc->append("uniform vec4 u_zgl_light0pos;\n");
         _vpSrc->append("uniform vec4 u_zgl_light0acl;\n");
         _vpSrc->append("uniform vec4 u_zgl_light0dcl;\n");

         if(0 != (_shader->vp_features & TKOPENGL_VP_FEATURE_LIGHTSPECULAR))
         {
            _vpSrc->append("uniform vec4 u_zgl_light0scl;\n");
         }

         if(0 != (_shader->vp_features & TKOPENGL_VP_FEATURE_LIGHTPOSITIONAL))
         {
            _vpSrc->append("uniform vec3 u_zgl_light0attl;\n");
         }

         if(0 != (_shader->vp_features & TKOPENGL_VP_FEATURE_LIGHTSPOT))
         {
            _vpSrc->append("uniform vec3 u_zgl_light0sdl;\n");
            _vpSrc->append("uniform vec3 u_zgl_light0srl;\n");
            _vpSrc->append("uniform vec3 u_zgl_light0crlcos;\n");
         }
      }
   }

   //
   // calc_light() function
   //
   if(bLighting)
   {
      _vpSrc->append("\nvoid calc_light(\n");
      _vpSrc->append("     in vec3  _eyeNorm\n");
      _vpSrc->append("   , in vec4  _acm\n");
      _vpSrc->append("   , in vec4  _dcm\n");
      _vpSrc->append("   , in vec4  _acl\n");
      _vpSrc->append("   , in vec4  _dcl\n");
      _vpSrc->append("   , in vec3  _posl\n");
      if(0 != (_shader->vp_features & TKOPENGL_VP_FEATURE_LIGHTSPECULAR))
      {
         _vpSrc->append("   , in vec4  _scl\n");
      }
      if(0 != (_shader->vp_features & TKOPENGL_VP_FEATURE_LIGHTPOSITIONAL))
      {
         _vpSrc->append("   , in vec3  _attl\n");
      }
      if(
         (0 != (_shader->vp_features & TKOPENGL_VP_FEATURE_LIGHTPOSITIONAL)) ||
         (0 != (_shader->vp_features & TKOPENGL_VP_FEATURE_LIGHTLOCALVIEWER))
         )
      {
         _vpSrc->append("   , in vec3  _eyeVtx\n");
      }
      if(0 != (_shader->vp_features & TKOPENGL_VP_FEATURE_LIGHTSPOT))
      {
         _vpSrc->append("   , in vec3  _sdl\n"); // spot light direction
         _vpSrc->append("   , in float _srl\n"); // spot light exponent
         _vpSrc->append("   , in float _crlCos\n"); // cos(spot light cutoff angle)
      }

      _vpSrc->append("   , inout vec4 _col1\n");
      // // if(0 != (_shader->vp_features & TKOPENGL_VP_FEATURE_LIGHTSPECULARSEP))
      // // {
      // //    _vpSrc->append("   , inout vec4 _col2\n");
      // // }

      _vpSrc->append("   ) {\n");

      if(0 != (_shader->vp_features & TKOPENGL_VP_FEATURE_LIGHTCOLORMATERIAL))
      {
         if(0 == (_shader->vp_features & TKOPENGL_VP_FEATURE_LIGHTPOSITIONAL))
         {
            // Directional light, no attenuation

            // GLspec: 'Note that if P2 has a zero w coordinate and P1 has non-zero w coordinate, then P1P2 is the unit vector corresponding to the direction specified by the x, y, and z coordinates of P2'

            // Emission + ambient
            _vpSrc->append("   _col1 = u_zgl_lightecm + (_acm * u_zgl_lightacs);\n"); // ecm + acm*acs
            _vpSrc->append("   _col1 = _col1 + (_acm * _acl);\n");  // acm * acli

            //////_vpSrc->append("   vec3 lnorm = normalize(u_zgl_light0pos.xyz - eyeVtx.xyz);\n");
            _vpSrc->append("   vec3 vpli = _posl;\n");
            _vpSrc->append("   float ldot = dot(_eyeNorm, vpli);\n");  // n DOT VPpli

            _vpSrc->append("   if(ldot > 0.0) {\n");

            // Diffuse
            ////_vpSrc->append("   ldot = max(ldot, 0.0);\n");
            _vpSrc->append("      _col1 = _col1 + (_dcm * _dcl) * ldot;\n"); // * dcm * dcli

            if(0 != (_shader->vp_features & TKOPENGL_VP_FEATURE_LIGHTSPECULAR))
            {
               // Specular half vector
               if(0 == (_shader->vp_features & TKOPENGL_VP_FEATURE_LIGHTLOCALVIEWER))
               {
                  // local_viewer=FALSE
                  _vpSrc->append("      vec3 hi = vec3(vpli.xy, vpli.z + 1.0);\n");
               }
               else
               {
                  // local_viewer=TRUE
                  _vpSrc->append("      vec3 hi = normalize(vpli.xyz) - normalize(_eyeVtx);\n");  // ((0,0,0,1) - _eyeVtx)
               }
               _vpSrc->append("      float spec = max(dot(_eyeNorm, normalize(hi)), 0.0);\n");
               _vpSrc->append("      spec = pow(spec, u_zgl_lightsrm);\n");

               if(0 == (_shader->vp_features & TKOPENGL_VP_FEATURE_LIGHTSPECULARSEP))
               {
                  // GL_SINGLE_COLOR
                  _vpSrc->append("      _col1 = _col1 + spec * (u_zgl_lightscm * u_zgl_light0scl);\n");
               }
               else
               {
                  // GL_SEPARATE_SPECULAR_COLOR
                  // (todo) multiply by atti / spoti
                  _vpSrc->append("      v_zgl_lightcol2 = v_zgl_lightcol2 + spec * (u_zgl_lightscm * u_zgl_light0scl);\n");
               }
            }

            // (todo) multiply col1 by atti / spoti

            _vpSrc->append("   }\n"); // if ldot > 0.0
         }
         else
         {
            // Positional light
         }
      }

      _vpSrc->append("}\n\n");
   }

   // Fragment shader output
#if (!defined(DX_GLES) || defined(DX_GLES_3)) && !defined(DX_EMU_GLSL_1)
   _fpSrc->append("out vec4 o_FragColor;\n");
   _fpSrc->append("#define O_FRAGCOLOR o_FragColor\n\n");
#else
   _fpSrc->append("#define O_FRAGCOLOR gl_FragColor\n\n");
#endif


   //
   // main functions
   //
   _vpSrc->append("void main() {\n");
   _fpSrc->append("void main() {\n");

   _vpSrc->append("   gl_Position = u_zgl_transform * a_zgl_vertex;\n");

   if(0 != (_shader->vp_features & TKOPENGL_VP_FEATURE_POINT_SIZE))
   {
      if(0 != (_shader->vp_features & TKOPENGL_VP_FEATURE_POINT_SPRITE))
      {
         _vpSrc->append("   float psz = u_zgl_pointsize * sqrt(1.0 / (u_zgl_pointatt.x + (u_zgl_pointatt.y * gl_Position.z) + (u_zgl_pointatt.z * (gl_Position.z * gl_Position.z))));\n");
         _vpSrc->append("   gl_PointSize = clamp(psz, u_zgl_pointminmax.x, u_zgl_pointminmax.y);\n");
      }
      else
      {
         _vpSrc->append("   gl_PointSize = u_zgl_pointsize;\n");
      }
   }

   if(0 != (_shader->vp_features & TKOPENGL_VP_FEATURE_COLOR))
   {
      if(0 != (_shader->vp_features & TKOPENGL_VP_FEATURE_COLORINIT))
      {
         // (note) e.g. texture w/o modulate and fog enabled
         _vpSrc->append("   v_zgl_color = vec4(1,1,1,1);\n");
      }
      else
      {
         // (note) actually the value needs to be clamped to [1,1,1,1] but let's leave that to the application..
         _vpSrc->append("   v_zgl_color = a_zgl_color;\n");
      }
   }

   if(TKOPENGL_FP_FEATURE_TEX_NONE != (_shader->fp_features & TKOPENGL_FP_FEATURE_TEX_MASK))
   {
#ifdef SHADER_DEBUG
      printf("xxx FP_FEATURE_TEX=0x%08x\n", (_shader->fp_features & TKOPENGL_FP_FEATURE_TEX_MASK));
#endif

      if(0 != (_shader->vp_features & TKOPENGL_VP_FEATURE_TEXTUREMATRIX))
      {
         _vpSrc->append("   v_zgl_texcoord = (u_zgl_texturematrix * a_zgl_texcoord);\n");
      }
      else
      {
         _vpSrc->append("   v_zgl_texcoord = a_zgl_texcoord;\n");
      }
   }

   if(0 != (_shader->vp_features & TKOPENGL_VP_FEATURE_FOG_MASK))
   {
      switch(_shader->vp_features & TKOPENGL_VP_FEATURE_FOG_MASK)
      {
         default:
            break;
         case TKOPENGL_VP_FEATURE_FOG_LINEAR:
            _vpSrc->append("   v_zgl_fog = (u_zgl_fogend - gl_Position.z) * u_zgl_fogobdelta;\n");
            break;
         case TKOPENGL_VP_FEATURE_FOG_EXP:
            _vpSrc->append("   v_zgl_fog = exp(-(u_zgl_fogdensity * gl_Position.z));\n");
            break;
         case TKOPENGL_VP_FEATURE_FOG_EXP2:
            _vpSrc->append("   float fogExp = (u_zgl_fogdensity * gl_Position.z);\n");
            _vpSrc->append("   v_zgl_fog = exp(-(fogExp*fogExp));\n");
            break;
      }
      _vpSrc->append("   v_zgl_fog = clamp(v_zgl_fog, 0.0, 1.0);\n");
   }

   // lighting
   if(bLighting)
   {
      if(
         (0 != (_shader->vp_features & TKOPENGL_VP_FEATURE_LIGHTPOSITIONAL)) ||
         (0 != (_shader->vp_features & TKOPENGL_VP_FEATURE_LIGHTLOCALVIEWER))
         )
      {
#ifdef DX_EMU_LIGHT_SKIPDIVW
         _vpSrc->append("   vec3 eyeVtx = (u_zgl_modelview * a_zgl_vertex).xyz;\n");
#else
         _vpSrc->append("   vec4 eyeVtx4 = u_zgl_modelview * a_zgl_vertex;\n");
         _vpSrc->append("   vec3 eyeVtx = eyeVtx4.xyz / eyeVtx4.w;\n");
#endif // DX_EMU_LIGHT_SKIPDIVW
      }

      _vpSrc->append("   vec3 eyeNorm = (u_zgl_normalmatrix * a_zgl_normal);\n");

      // light output colors:
      _vpSrc->append("   vec4 lightCol1;\n");
      _vpSrc->append("   v_zgl_color = vec4(0.0, 0.0, 0.0, 0.0);\n");

      if(0 != (_shader->vp_features & TKOPENGL_VP_FEATURE_LIGHTSPECULARSEP))
      {
         _vpSrc->append("   v_zgl_lightcol2 = vec4(0.0, 0.0, 0.0, 0.0);\n");
      }

      // call light fxn
      if(0 == (_shader->vp_features & TKOPENGL_VP_FEATURE_LIGHTPOSITIONAL))
      {
         // Directional light, no attenuation
         _vpSrc->append("   calc_light(eyeNorm\n");

         if(0 != (_shader->vp_features & TKOPENGL_VP_FEATURE_LIGHTCOLORMATERIAL))
         {
            _vpSrc->append("              , a_zgl_color/*acm*/, a_zgl_color/*dcm*/\n");
         }
         else
         {
            _vpSrc->append("              , u_zgl_lightacm, u_zgl_lightdcm\n");
         }

         _vpSrc->append("              , u_zgl_light0acl, u_zgl_light0dcl, u_zgl_light0pos.xyz\n");
         if(0 != (_shader->vp_features & TKOPENGL_VP_FEATURE_LIGHTSPECULAR))
         {
            _vpSrc->append("              , u_zgl_light0scl\n");
         }
         if(0 != (_shader->vp_features & TKOPENGL_VP_FEATURE_LIGHTPOSITIONAL))
         {
            _vpSrc->append("              , u_zgl_light0attl\n");
         }
         if(
            (0 != (_shader->vp_features & TKOPENGL_VP_FEATURE_LIGHTPOSITIONAL)) ||
            (0 != (_shader->vp_features & TKOPENGL_VP_FEATURE_LIGHTLOCALVIEWER))
            )
         {
            _vpSrc->append("              , eyeVtx\n");
         }
         if(0 != (_shader->vp_features & TKOPENGL_VP_FEATURE_LIGHTSPOT))
         {
            _vpSrc->append("              , u_zgl_light0sdl\n");
            _vpSrc->append("              , u_zgl_light0srl\n");
            _vpSrc->append("              , u_zgl_light0crlCos\n");
         }

         _vpSrc->append("              , lightCol1\n");

         _vpSrc->append("              );\n\n");
      }
      else
      {
         // Positional light
      }


      _vpSrc->append("   v_zgl_color = v_zgl_color + lightCol1;\n");

      // already done by calc_light()
      // // if(0 != (_shader->vp_features & TKOPENGL_VP_FEATURE_LIGHTSPECULARSEP))
      // // {
      // //    _vpSrc->append("   v_zgl_lightcol2 = v_zgl_lightcol2 + lightCol2\n");
      // // }

      // light output:
      _vpSrc->append("   v_zgl_color = clamp(v_zgl_color, 0.0, 1.0);\n");
   }

   switch(_shader->fp_features & TKOPENGL_FP_FEATURE_TEX_MASK)
   {
      default:
      case TKOPENGL_FP_FEATURE_TEX_NONE:
         _fpSrc->append("   O_FRAGCOLOR = v_zgl_color;\n");
         break;
      case TKOPENGL_FP_FEATURE_TEX_1D:
#ifdef DX_EMU_TEXTURE_1D
         // (todo) sample texel center (must know texture.sy)
         _fpSrc->append("   O_FRAGCOLOR = texture2D(u_zgl_sampler, vec2(v_zgl_texcoord.x, 0.0));\n");
#else
         _fpSrc->append("   O_FRAGCOLOR = texture1D(u_zgl_sampler, v_zgl_texcoord.x);\n");
#endif // DX_EMU_TEXTURE_1D
         break;
      case TKOPENGL_FP_FEATURE_TEX_2D:
         if(0 != (_shader->vp_features & TKOPENGL_VP_FEATURE_POINT_TEXCOORD))
         {
            _fpSrc->append("   O_FRAGCOLOR = texture2D(u_zgl_sampler, gl_PointCoord.xy);\n");
         }
         else
         {
            _fpSrc->append("   O_FRAGCOLOR = texture2D(u_zgl_sampler, v_zgl_texcoord.xy);\n");
            ///_fpSrc->append("   O_FRAGCOLOR = vec4(1,1,1,texture2D(u_zgl_sampler, v_zgl_texcoord.xy).a);\n");
         }
         break;
      case TKOPENGL_FP_FEATURE_TEX_3D:
         _fpSrc->append("   O_FRAGCOLOR = texture3D(u_zgl_sampler, v_zgl_texcoord.xyz);\n");
         break;
      case TKOPENGL_FP_FEATURE_TEX_CUBE:
         _fpSrc->append("   O_FRAGCOLOR = textureCube(u_zgl_sampler, v_zgl_texcoord.xyz);\n");
         break;
      case TKOPENGL_FP_FEATURE_TEX_RECT:
#ifdef DX_EMU_TEXTURE_RECTANGLE
         _vpSrc->append("   v_zgl_texcoord.xy = (v_zgl_texcoord.xy * u_zgl_obtexsize);\n");
         _fpSrc->append("   O_FRAGCOLOR = texture2D(u_zgl_sampler, v_zgl_texcoord.xy);\n");
#else
         _fpSrc->append("   O_FRAGCOLOR = texture2DRect(u_zgl_sampler, v_zgl_texcoord.xy);\n");
#endif // DX_EMU_TEXTURE_RECTANGLE
         break;
   }

   if(0 != (_shader->fp_features & TKOPENGL_FP_FEATURE_TEX_ALPHA))
   {
      // emulate GL_INTENSITY
      _fpSrc->append("   O_FRAGCOLOR = vec4(O_FRAGCOLOR.a, O_FRAGCOLOR.a, O_FRAGCOLOR.a, O_FRAGCOLOR.a);\n");
   }

   if(TKOPENGL_FP_FEATURE_TEX_NONE != (_shader->fp_features & TKOPENGL_FP_FEATURE_TEX_MASK))
   {
      switch(_shader->fp_features & TKOPENGL_FP_FEATURE_TEXENV_MASK)
      {
         default:
            ////case TKOPENGL_FP_FEATURE_TEXENV_NONE:
         case TKOPENGL_FP_FEATURE_TEXENV_REPLACE:
            // (todo) use v_zgl_color.rgb if tex is GL_ALPHA
            // (todo) use v_zgl_color.a if tex is GL_LUMINANCE or GL_RGB
            if(0 != (_shader->vp_features & TKOPENGL_VP_FEATURE_COLORINIT)) // e.g. fog
            {
               _fpSrc->append("   O_FRAGCOLOR = O_FRAGCOLOR * v_zgl_color;\n");
            }
            break;
         case TKOPENGL_FP_FEATURE_TEXENV_MODULATE:
            _fpSrc->append("   O_FRAGCOLOR = O_FRAGCOLOR * v_zgl_color;\n");
            break;
         case TKOPENGL_FP_FEATURE_TEXENV_DECAL:
            // (todo) use v_zgl_color.a if tex is GL_RGB
            // Cv = Cp*(1-As) + Cs*As
            // Av = Ap
            _fpSrc->append("   O_FRAGCOLOR = vec4(mix(v_zgl_color.rgb, O_FRAGCOLOR.rgb, O_FRAGCOLOR.a), v_zgl_color.a);\n");
            break;
         case TKOPENGL_FP_FEATURE_TEXENV_BLEND:
            // (todo) use v_zgl_color.rgb if tex is GL_ALPHA
            // Cv = Cp*(1-Cs) + Cc*Cs
            // Av = Ap*As
            _fpSrc->append("   O_FRAGCOLOR = vec4(clamp(vec3(v_zgl_color.r * (1.0 - O_FRAGCOLOR.r), v_zgl_color.g * (1.0 - O_FRAGCOLOR.g), v_zgl_color.b * (1.0 - O_FRAGCOLOR.b)) + (u_zgl_envcolor.rgb * O_FRAGCOLOR.rgb), vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0)), v_zgl_color.a * O_FRAGCOLOR.a);\n");
            break;
         case TKOPENGL_FP_FEATURE_TEXENV_ADD:
            // (todo) use v_zgl_color.rgb if tex is GL_ALPHA
            // (todo) use v_zgl_color.a if tex is GL_LUMINANCE
            _fpSrc->append("   O_FRAGCOLOR = vec4(clamp(v_zgl_color.rgb + O_FRAGCOLOR.rgb, vec(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0)), v_zgl_color.a * O_FRAGCOLOR.a);\n");
            break;
      }
   }

   if(0 != (_shader->vp_features & TKOPENGL_VP_FEATURE_LIGHTSPECULARSEP))
   {
      _fpSrc->append("   O_FRAGCOLOR = O_FRAGCOLOR + v_zgl_lightcol2;\n");
   }

   switch(_shader->fp_features & TKOPENGL_FP_FEATURE_ALPHATEST_MASK)
   {
      default:
      case TKOPENGL_FP_FEATURE_ALPHATEST_ALWAYS:
         break;
      case TKOPENGL_FP_FEATURE_ALPHATEST_LESS:
         _fpSrc->append("   if(O_FRAGCOLOR.a >= u_zgl_alpharef) discard;\n");
         break;
      case TKOPENGL_FP_FEATURE_ALPHATEST_EQUAL:
         _fpSrc->append("   if(O_FRAGCOLOR.a != u_zgl_alpharef) discard;\n");
         break;
      case TKOPENGL_FP_FEATURE_ALPHATEST_LEQUAL:
         _fpSrc->append("   if(O_FRAGCOLOR.a > u_zgl_alpharef) discard;\n");
         break;
      case TKOPENGL_FP_FEATURE_ALPHATEST_GREATER:
         _fpSrc->append("   if(O_FRAGCOLOR.a <= u_zgl_alpharef) discard;\n");
         break;
      case TKOPENGL_FP_FEATURE_ALPHATEST_NOTEQUAL:
         _fpSrc->append("   if(O_FRAGCOLOR.a == u_zgl_alpharef) discard;\n");
         break;
      case TKOPENGL_FP_FEATURE_ALPHATEST_GEQUAL:
         _fpSrc->append("   if(O_FRAGCOLOR.a < u_zgl_alpharef) discard;\n");
         break;
      case TKOPENGL_FP_FEATURE_ALPHATEST_NEVER:
         _fpSrc->append("   discard;\n"); // useless?
         break;
   }

   if(0 != (_shader->vp_features & TKOPENGL_VP_FEATURE_FOG_MASK))
   {
      switch(_shader->fp_features & TKOPENGL_FP_FEATURE_FOG_ALPHA_MASK)
      {
         default:
         case TKOPENGL_FP_FEATURE_FOG_ALPHA_REPL:
            _fpSrc->append("   O_FRAGCOLOR = vec4(mix(u_zgl_fogcolor.rgb, O_FRAGCOLOR.rgb, v_zgl_fog), O_FRAGCOLOR.a);\n");
            break;

         case TKOPENGL_FP_FEATURE_FOG_ALPHA_BLEND:
            _fpSrc->append("   O_FRAGCOLOR = mix(u_zgl_fogcolor, O_FRAGCOLOR, v_zgl_fog);\n");
            break;

         case TKOPENGL_FP_FEATURE_FOG_ALPHA_MULT:
            _fpSrc->append("   O_FRAGCOLOR = vec4(mix(u_zgl_fogcolor.rgb, O_FRAGCOLOR.rgb, v_zgl_fog), mix(O_FRAGCOLOR.a*u_zgl_fogcolor.a, O_FRAGCOLOR.a, v_zgl_fog));\n");
            break;
      }
   }

   _vpSrc->append("}\n");
   _fpSrc->append("}\n");

#ifdef SHADER_DEBUG
   yac_host->printf("[dbg] tkopengl::shader_src_gen:\n\tvp_src=\"\n%s\"\n\tfp_src=\"\n%s\"\n",
                    _vpSrc->chars,
                    _fpSrc->chars
                    );
#endif // SHADER_DEBUG
}


/* ----------------------------------------------------------------------------- loc_shader_resolve() */
static void loc_shader_resolve(gles_shader_t *_shader) {

   _shader->attr_map[ATTR_IDX_VERTEX]   = _glGetAttribLocation_int(_shader->prg_id, "a_zgl_vertex");
   _shader->attr_map[ATTR_IDX_NORMAL]   = _glGetAttribLocation_int(_shader->prg_id, "a_zgl_normal");
   _shader->attr_map[ATTR_IDX_COLOR]    = _glGetAttribLocation_int(_shader->prg_id, "a_zgl_color");
   _shader->attr_map[ATTR_IDX_TEXCOORD] = _glGetAttribLocation_int(_shader->prg_id, "a_zgl_texcoord");

   _shader->uni_map[UNI_IDX_TRANSFORM]    = _glGetUniformLocation_int(_shader->prg_id, "u_zgl_transform");
   _shader->uni_map[UNI_IDX_SAMPLER]      = _glGetUniformLocation_int(_shader->prg_id, "u_zgl_sampler");
   _shader->uni_map[UNI_IDX_OBTEXSIZE]    = _glGetUniformLocation_int(_shader->prg_id, "u_zgl_obtexsize");
   _shader->uni_map[UNI_IDX_ENVCOLOR]     = _glGetUniformLocation_int(_shader->prg_id, "u_zgl_envcolor");
   _shader->uni_map[UNI_IDX_ALPHAREF]     = _glGetUniformLocation_int(_shader->prg_id, "u_zgl_alpharef");
   _shader->uni_map[UNI_IDX_POINT_SIZE]   = _glGetUniformLocation_int(_shader->prg_id, "u_zgl_pointsize");
   _shader->uni_map[UNI_IDX_POINT_MINMAX] = _glGetUniformLocation_int(_shader->prg_id, "u_zgl_pointminmax");
   _shader->uni_map[UNI_IDX_POINT_ATT]    = _glGetUniformLocation_int(_shader->prg_id, "u_zgl_pointatt");
   _shader->uni_map[UNI_IDX_FOGOBDELTA]   = _glGetUniformLocation_int(_shader->prg_id, "u_zgl_fogobdelta");
   _shader->uni_map[UNI_IDX_FOGEND]       = _glGetUniformLocation_int(_shader->prg_id, "u_zgl_fogend");
   _shader->uni_map[UNI_IDX_FOGDENSITY]   = _glGetUniformLocation_int(_shader->prg_id, "u_zgl_fogdensity");
   _shader->uni_map[UNI_IDX_FOGCOLOR]     = _glGetUniformLocation_int(_shader->prg_id, "u_zgl_fogcolor");
   _shader->uni_map[UNI_IDX_NORMALMATRIX] = _glGetUniformLocation_int(_shader->prg_id, "u_zgl_normalmatrix");
   _shader->uni_map[UNI_IDX_MODELVIEW]    = _glGetUniformLocation_int(_shader->prg_id, "u_zgl_modelview");
   _shader->uni_map[UNI_IDX_LIGHTACS]     = _glGetUniformLocation_int(_shader->prg_id, "u_zgl_lightacs");
   _shader->uni_map[UNI_IDX_LIGHTECM]     = _glGetUniformLocation_int(_shader->prg_id, "u_zgl_lightecm");
   _shader->uni_map[UNI_IDX_LIGHTACM]     = _glGetUniformLocation_int(_shader->prg_id, "u_zgl_lightacm");
   _shader->uni_map[UNI_IDX_LIGHTDCM]     = _glGetUniformLocation_int(_shader->prg_id, "u_zgl_lightdcm");
   _shader->uni_map[UNI_IDX_LIGHTSCM]     = _glGetUniformLocation_int(_shader->prg_id, "u_zgl_lightscm");
   _shader->uni_map[UNI_IDX_LIGHTSRM]     = _glGetUniformLocation_int(_shader->prg_id, "u_zgl_lightsrm");
   _shader->uni_map[UNI_IDX_LIGHT0POS]    = _glGetUniformLocation_int(_shader->prg_id, "u_zgl_light0pos");
   _shader->uni_map[UNI_IDX_LIGHT0ACL]    = _glGetUniformLocation_int(_shader->prg_id, "u_zgl_light0acl");
   _shader->uni_map[UNI_IDX_LIGHT0DCL]    = _glGetUniformLocation_int(_shader->prg_id, "u_zgl_light0dcl");
   _shader->uni_map[UNI_IDX_LIGHT0SCL]    = _glGetUniformLocation_int(_shader->prg_id, "u_zgl_light0scl");
   _shader->uni_map[UNI_IDX_LIGHT0ATTL]   = _glGetUniformLocation_int(_shader->prg_id, "u_zgl_light0attl");
   _shader->uni_map[UNI_IDX_LIGHT0SDL]    = _glGetUniformLocation_int(_shader->prg_id, "u_zgl_light0sdl");
   _shader->uni_map[UNI_IDX_LIGHT0SRL]    = _glGetUniformLocation_int(_shader->prg_id, "u_zgl_light0srl");
   _shader->uni_map[UNI_IDX_LIGHT0CRLCOS] = _glGetUniformLocation_int(_shader->prg_id, "u_zgl_light0crlcos");
   _shader->uni_map[UNI_IDX_TEXTUREMATRIX]= _glGetUniformLocation_int(_shader->prg_id, "u_zgl_texturematrix");

#ifdef SHADER_DEBUG
   {
      sUI i;
      for(i=0; i<NUM_ATTRIBS; i++)
      {
         yac_host->printf("[dbg] tkopengl::loc_shader_resolve: attr_map[%u] = %d\n", i, _shader->attr_map[i]);
      }
   }
#endif

#ifdef SHADER_DEBUG
   {
      sUI i;
      for(i=0; i<NUM_UNIFORMS; i++)
      {
         yac_host->printf("[dbg] tkopengl::loc_shader_resolve: uni_map[%u] = %d\n", i, _shader->uni_map[i]);
      }
   }
#endif
}

/* ----------------------------------------------------------------------------- loc_shader_find() */
static gles_shader_t *loc_shader_find(sUI _vpFeatures, sUI _fpFeatures) {
   gles_shader_t *shader = shaders;

   // (todo) use hashtable?
   sUI i;
   for(i=0; i<num_shaders; i++)
   {
      if(_vpFeatures == shader->vp_features)
      {
         if(_fpFeatures == shader->fp_features)
         {
            break;
         }
      }

      // Try next shader
      shader++;
   }

   if(num_shaders == i)
   {
      // Shader not found, create new one
      if(num_shaders < TKOPENGL_MAX_SHADERS)
      {
         num_shaders++;
         shader->vp_features = _vpFeatures;
         shader->fp_features = _fpFeatures;

         // Create vertex and fragment shader sources
         YAC_String vpSrc;
         YAC_String fpSrc;
         loc_shader_src_gen(shader, &vpSrc, &fpSrc);

         gles_shader_create(shader,
                            (const char*)vpSrc.chars,
                            (const char*)fpSrc.chars
                            );

         loc_shader_resolve(shader);
      }
      else
      {
         yac_host->printf("[---] tkopengl: maximum number of emulation shaders exceeded (%u)\n", TKOPENGL_MAX_SHADERS);
         shader = NULL;
      }
   }

   return shader;
}

/* ----------------------------------------------------------------------------- tkopengl_shader_init() */
void tkopengl_shader_init(void) {

   num_shaders = 0;
   memset(shaders, 0, sizeof(gles_shader_t) * TKOPENGL_MAX_SHADERS);

   b_tkopengl_custom_shader = YAC_FALSE;
}

/* ----------------------------------------------------------------------------- tkopengl_shader_exit() */
void tkopengl_shader_exit(void) {
   sUI i;

   for(i=0; i<num_shaders; i++)
   {
      gles_shader_destroy(&shaders[i]);
   }
}

/* ----------------------------------------------------------------------------- loc_select_shader() */
static void loc_select_shader(GLenum _drawMode) {
   sUI vpFeatures = 0;
   sUI fpFeatures = 0;

   switch(Dactivetextureunit(mode))
   {
      default:
         vpFeatures |= TKOPENGL_VP_FEATURE_COLOR;
         fpFeatures |= TKOPENGL_FP_FEATURE_COLOR;
         fpFeatures |= TKOPENGL_FP_FEATURE_TEX_NONE;
         break;

      case GL_TEXTURE_1D:
         vpFeatures |= TKOPENGL_VP_FEATURE_TEXGEN_COORD;
         fpFeatures |= TKOPENGL_FP_FEATURE_TEXCOORD;
         fpFeatures |= TKOPENGL_FP_FEATURE_TEX_1D;
         break;

      case GL_TEXTURE_2D:
         vpFeatures |= TKOPENGL_VP_FEATURE_TEXGEN_COORD;
         fpFeatures |= TKOPENGL_FP_FEATURE_TEXCOORD;
         fpFeatures |= TKOPENGL_FP_FEATURE_TEX_2D;
         break;

      case GL_TEXTURE_3D:
         vpFeatures |= TKOPENGL_VP_FEATURE_TEXGEN_COORD;
         fpFeatures |= TKOPENGL_FP_FEATURE_TEXCOORD;
         fpFeatures |= TKOPENGL_FP_FEATURE_TEX_3D;
         break;

      case GL_TEXTURE_CUBE_MAP:
         vpFeatures |= TKOPENGL_VP_FEATURE_TEXGEN_COORD;
         fpFeatures |= TKOPENGL_FP_FEATURE_TEXCOORD;
         fpFeatures |= TKOPENGL_FP_FEATURE_TEX_CUBE;
         break;

      case GL_TEXTURE_RECTANGLE:
         vpFeatures |= TKOPENGL_VP_FEATURE_TEXGEN_COORD;
         fpFeatures |= TKOPENGL_FP_FEATURE_TEXCOORD;
         fpFeatures |= TKOPENGL_FP_FEATURE_TEX_RECT;
         break;
   }

   if(0 != Dactivetextureunit(mode))
   {
      // glTexEnv
      switch(Dactivetextureunit(env_mode))
      {
         default:
         case GL_REPLACE:
            fpFeatures |= TKOPENGL_FP_FEATURE_TEXENV_REPLACE;
            break;
         case GL_MODULATE:
            vpFeatures |= TKOPENGL_VP_FEATURE_COLOR;
            fpFeatures |= TKOPENGL_FP_FEATURE_COLOR;
            fpFeatures |= TKOPENGL_FP_FEATURE_TEXENV_MODULATE;
            break;
         case GL_DECAL:
            vpFeatures |= TKOPENGL_VP_FEATURE_COLOR;
            fpFeatures |= TKOPENGL_FP_FEATURE_COLOR;
            fpFeatures |= TKOPENGL_FP_FEATURE_TEXENV_DECAL;
            break;
         case GL_BLEND:
            vpFeatures |= TKOPENGL_VP_FEATURE_COLOR;
            fpFeatures |= TKOPENGL_FP_FEATURE_COLOR;
            fpFeatures |= TKOPENGL_FP_FEATURE_TEXENV_BLEND; // +envcolor
            break;
         case GL_ADD:
            vpFeatures |= TKOPENGL_VP_FEATURE_COLOR;
            fpFeatures |= TKOPENGL_FP_FEATURE_COLOR;
            fpFeatures |= TKOPENGL_FP_FEATURE_TEXENV_ADD;
            break;
      }
   }

   if(tkopengl_attrib->alphatest.b_enable)
   {
      switch(tkopengl_attrib->alphatest.func)
      {
         case GL_NEVER:
            fpFeatures |= TKOPENGL_FP_FEATURE_ALPHATEST_NEVER;
            break;
         case GL_LESS:
            fpFeatures |= TKOPENGL_FP_FEATURE_ALPHATEST_LESS;
            break;
         case GL_EQUAL:
            fpFeatures |= TKOPENGL_FP_FEATURE_ALPHATEST_EQUAL;
            break;
         case GL_LEQUAL:
            fpFeatures |= TKOPENGL_FP_FEATURE_ALPHATEST_LEQUAL;
            break;
         case GL_GREATER:
            fpFeatures |= TKOPENGL_FP_FEATURE_ALPHATEST_GREATER;
            break;
         case GL_NOTEQUAL:
            fpFeatures |= TKOPENGL_FP_FEATURE_ALPHATEST_NOTEQUAL;
            break;
         case GL_GEQUAL:
            fpFeatures |= TKOPENGL_FP_FEATURE_ALPHATEST_GEQUAL;
            break;
         case GL_ALWAYS:
            /* nothing to do */
            break;
      }
   }

   if(tkopengl_attrib->fog.b_enable)
   {
      // // if(0 == (vpFeatures & TKOPENGL_VP_FEATURE_COLOR))
      // // {
      // //    vpFeatures |= TKOPENGL_VP_FEATURE_COLORINIT;
      // // }
      // // vpFeatures |= TKOPENGL_VP_FEATURE_COLOR;
      // // fpFeatures |= TKOPENGL_FP_FEATURE_COLOR;

      switch(tkopengl_attrib->fog.mode)
      {
         default:
         case GL_LINEAR:
            vpFeatures |= TKOPENGL_VP_FEATURE_FOG_LINEAR;
            break;
         case GL_EXP:
            vpFeatures |= TKOPENGL_VP_FEATURE_FOG_EXP;
            break;
         case GL_EXP2:
            vpFeatures |= TKOPENGL_VP_FEATURE_FOG_EXP2;
            break;
      }

      switch(tkopengl_attrib->fog.alpha_mode)
      {
         default:
         case GL_REPLACE:
            fpFeatures |= TKOPENGL_FP_FEATURE_FOG_ALPHA_REPL;
            break;

         case GL_BLEND:
            fpFeatures |= TKOPENGL_FP_FEATURE_FOG_ALPHA_BLEND;
            break;

         case GL_MULT:
            fpFeatures |= TKOPENGL_FP_FEATURE_FOG_ALPHA_MULT;
            break;
      }
   }

   if(tkopengl_attrib->light.b_enable)
   {
      if(0 != tkopengl_attrib->light.enable_mask)
      {
         vpFeatures |= (tkopengl_attrib->light.enable_mask << TKOPENGL_VP_FEATURE_LIGHT0_SHIFT);
         fpFeatures |= TKOPENGL_FP_FEATURE_COLOR;
         vpFeatures |= TKOPENGL_VP_FEATURE_COLOR; // (todo) actually only when colormaterial is enabled

         if(tkopengl_attrib->light.b_color_material)
         {
            vpFeatures |= TKOPENGL_VP_FEATURE_LIGHTCOLORMATERIAL;
         }

         if(GL_SEPARATE_SPECULAR_COLOR == tkopengl_attrib->light.color_control)
         {
            vpFeatures |= TKOPENGL_VP_FEATURE_LIGHTSPECULARSEP;
         }

         if(tkopengl_attrib->light.b_local_viewer)
         {
            vpFeatures |= TKOPENGL_VP_FEATURE_LIGHTLOCALVIEWER;
         }

         // enable attenuation ? (any light is positional)
         {
            sBool bAnyAtt = YAC_FALSE;
            sUI i;

            for(i=0; i<TKOPENGL_MAX_LIGHTS; i++)
            {
               if(tkopengl_attrib->light.enable_mask & (1 << i))
               {
                  if(tkopengl_attrib->light.lights[i].position[3] != 0.0f) // w != 0.0f
                  {
                     bAnyAtt = YAC_TRUE;
                     break;
                  }
               }
            }

            if(bAnyAtt)
            {
               vpFeatures |= TKOPENGL_VP_FEATURE_LIGHTPOSITIONAL;
            }
         }

         // enable spot light ?
         {
            sBool bAnySpot = YAC_FALSE;
            sUI i;

            for(i=0; i<TKOPENGL_MAX_LIGHTS; i++)
            {
               if(tkopengl_attrib->light.enable_mask & (1 << i))
               {
                  if(tkopengl_attrib->light.lights[i].spot_cutoff < 90.0f) // != 180.0f
                  {
                     bAnySpot = YAC_TRUE;
                     break;
                  }
               }
            }

            if(bAnySpot)
            {
               vpFeatures |= TKOPENGL_VP_FEATURE_LIGHTSPOT;
            }
         }

         // enable specular component ?
         if(
            (tkopengl_attrib->light.material.specular_color[0] > 0.0f) ||
            (tkopengl_attrib->light.material.specular_color[1] > 0.0f) ||
            (tkopengl_attrib->light.material.specular_color[2] > 0.0f)
            )
         {
            sBool bAnySpec = YAC_FALSE;
            sUI i;
            for(i=0; i<TKOPENGL_MAX_LIGHTS; i++)
            {
               if(tkopengl_attrib->light.enable_mask & (1 << i))
               {
                  if(
                     (tkopengl_attrib->light.lights[i].specular_color[0] > 0.0f) ||
                     (tkopengl_attrib->light.lights[i].specular_color[1] > 0.0f) ||
                     (tkopengl_attrib->light.lights[i].specular_color[2] > 0.0f)
                     )
                  {
                     bAnySpec = YAC_TRUE;
                     break;
                  }
               }
            }

            if(bAnySpec)
            {
               vpFeatures |= TKOPENGL_VP_FEATURE_LIGHTSPECULAR;
            }
         }

      } // if 0 != enable_mask

   } // if light enable

   if(GL_POINTS == _drawMode)
   {
      vpFeatures |= TKOPENGL_VP_FEATURE_POINT_SIZE;

      if(tkopengl_attrib->point.sprite.b_enable)
      {
         vpFeatures |= TKOPENGL_VP_FEATURE_POINT_SPRITE;

         if(tkopengl_attrib->point.sprite.b_coord_replace)
         {
            vpFeatures |= TKOPENGL_VP_FEATURE_POINT_TEXCOORD;
         }
      }
   }

   if(0 != Dactivetextureunit(mode))
   {
      if(0 == (tkopengl_matrix_flags[TKOPENGL_MATRIX_MODE_TEXTURE] & TKOPENGL_MATRIX_FLAG_IDENTITY))
      {
         vpFeatures |= TKOPENGL_VP_FEATURE_TEXTUREMATRIX;
      }

#ifdef DX_GLES
      if(-1 != Dactivetextureunit(bound_nr))
      {
         if(GL_ALPHA == Dboundtexture(internal_format))
         {
            // (note) GLES has no GL_INTENSITY format and alpha values are read as (0,0,0,a)
            //         but we want rgba=(a,a,a,a) so multiplying with a color does not always result in black
            fpFeatures |= TKOPENGL_FP_FEATURE_TEX_ALPHA;
         }
      }
#endif // DX_GLES

   }

   // (todo) add remaining features (multitexturing, texture combiner, ..)

   // Find or create shader that matches the feature set
   current_shader = loc_shader_find(vpFeatures, fpFeatures);
}

/* ----------------------------------------------------------------------------- loc_bind_attribs() */
static void loc_bind_attribs(GLenum _drawMode, sBool _bAllowImm) {
   tkopengl_userarray_t *arr;
   sBool bUseImmArray = _bAllowImm;

   // Bind vertex array
   if(Dattrvalid(ATTR_IDX_VERTEX))
   {
      arr = &tkopengl_attrib->userarray[TKOPENGL_USERARRAY_VERTEX];

      if(arr->b_enable)
      {
         //printf("xxx uservtx num_comp=%u type=0x%04x stride=%d\n", arr->num_comp, (sU16)arr->type, arr->stride);
         _glVertexAttribPointer_int_vao(current_shader->attr_map[ATTR_IDX_VERTEX],
                                        arr->num_comp, arr->type,
                                        (GL_FLOAT != arr->type) ? GL_TRUE : GL_FALSE,    /* normalize */
                                        arr->stride,
                                        arr->addr.pany
                                        );

         bUseImmArray = YAC_FALSE;
      }
      else
      {
         //printf("xxx use imm vertexarray\n");

         _glVertexAttribPointer_int_vao(current_shader->attr_map[ATTR_IDX_VERTEX],
                                        4, GL_FLOAT,
                                        GL_FALSE,    /* normalize */
                                        0,           /* stride */
                                        tkopengl_imm_vertex_array
                                        );
      }

      _glEnableVertexAttribArray_int_vao(current_shader->attr_map[ATTR_IDX_VERTEX]);
   }

   if(GL_POLYGON != _drawMode)
   {
      // Bind normal array
      if(Dattrvalid(ATTR_IDX_NORMAL))
      {
         arr = &tkopengl_attrib->userarray[TKOPENGL_USERARRAY_NORMAL];

         if(arr->b_enable)
         {
            _glVertexAttribPointer_int_vao(current_shader->attr_map[ATTR_IDX_NORMAL],
                                           arr->num_comp, arr->type,
                                           (GL_FLOAT != arr->type) ? GL_TRUE : GL_FALSE,    /* normalize */
                                           arr->stride,
                                           arr->addr.pany
                                           );

            _glEnableVertexAttribArray_int_vao(current_shader->attr_map[ATTR_IDX_NORMAL]);
         }
         else
         {
            if(bUseImmArray)
            {
               _glVertexAttribPointer_int_vao(current_shader->attr_map[ATTR_IDX_NORMAL],
                                              3, GL_FLOAT,
                                              GL_FALSE,    /* normalize */
                                              0,           /* stride */
                                              tkopengl_imm_normal_array
                                              );

               _glEnableVertexAttribArray_int_vao(current_shader->attr_map[ATTR_IDX_NORMAL]);
            }
            else
            {
               // Constant normal
               _glVertexAttrib3f(current_shader->attr_map[ATTR_IDX_NORMAL],
                                 current_normal.x,
                                 current_normal.y,
                                 current_normal.z
                                 );
            }
         }

      }


      // Bind color array
      if(Dattrvalid(ATTR_IDX_COLOR))
      {
         arr = &tkopengl_attrib->userarray[TKOPENGL_USERARRAY_COLOR];

         if(arr->b_enable)
         {
            _glVertexAttribPointer_int_vao(current_shader->attr_map[ATTR_IDX_COLOR],
                                           arr->num_comp, arr->type,
                                           (GL_FLOAT != arr->type) ? GL_TRUE : GL_FALSE,    /* normalize */
                                           arr->stride,
                                           arr->addr.pany
                                           );

            _glEnableVertexAttribArray_int_vao(current_shader->attr_map[ATTR_IDX_COLOR]);
         }
         else
         {
            if(bUseImmArray)
            {
               _glVertexAttribPointer_int_vao(current_shader->attr_map[ATTR_IDX_COLOR],
                                              4, GL_FLOAT,
                                              GL_FALSE,    /* normalize */
                                              0,           /* stride */
                                              tkopengl_imm_color_array
                                              );

               _glEnableVertexAttribArray_int_vao(current_shader->attr_map[ATTR_IDX_COLOR]);
            }
            else
            {
#if 0
               printf("xxx use constcolor rgba=(%f, %f, %f, %f)\n",
                      current_color.r, current_color.g, current_color.b, current_color.a);
#endif
               // Constant color
               _glVertexAttrib4f(current_shader->attr_map[ATTR_IDX_COLOR],
                                 current_color.r,
                                 current_color.g,
                                 current_color.b,
                                 current_color.a
                                 );
            }
         }

      }


      // Bind texture coordinate array
      if(Dattrvalid(ATTR_IDX_TEXCOORD))
      {
         arr = &tkopengl_attrib->userarray[TKOPENGL_USERARRAY_TEXCOORD];

         if(arr->b_enable)
         {
            _glVertexAttribPointer_int_vao(current_shader->attr_map[ATTR_IDX_TEXCOORD],
                                           arr->num_comp, arr->type,
                                           (GL_FLOAT != arr->type) ? GL_TRUE : GL_FALSE,    /* normalize */
                                           arr->stride,
                                           arr->addr.pany
                                           );

            _glEnableVertexAttribArray_int_vao(current_shader->attr_map[ATTR_IDX_TEXCOORD]);
         }
         else
         {
            if(bUseImmArray)
            {
               _glVertexAttribPointer_int_vao(current_shader->attr_map[ATTR_IDX_TEXCOORD],
                                              4, GL_FLOAT,
                                              GL_FALSE,    /* normalize */
                                              0,           /* stride */
                                              tkopengl_imm_texcoord_array
                                              );

               _glEnableVertexAttribArray_int_vao(current_shader->attr_map[ATTR_IDX_TEXCOORD]);
            }
            else
            {
               // Constant texcoord
               _glVertexAttrib4f(current_shader->attr_map[ATTR_IDX_TEXCOORD],
                                 current_texcoord.s,
                                 current_texcoord.t,
                                 current_texcoord.r,
                                 current_texcoord.q
                                 );
            }
         }
      }
   }
   else
   {
      // (convex) Polygon

      if(Dattrvalid(ATTR_IDX_NORMAL))
      {
         _glDisableVertexAttribArray_int_vao(current_shader->attr_map[ATTR_IDX_NORMAL]);

         if(_bAllowImm)
         {
            _glVertexAttrib3fv_int(current_shader->attr_map[ATTR_IDX_NORMAL],
                                   tkopengl_imm_normal_array
                                   );
         }
         else
         {
            _glVertexAttrib3fv_int(current_shader->attr_map[ATTR_IDX_NORMAL],
                                   (sF32*)&current_normal
                                   );
         }
      }

      if(Dattrvalid(ATTR_IDX_COLOR))
      {
         _glDisableVertexAttribArray_int_vao(current_shader->attr_map[ATTR_IDX_COLOR]);

         if(_bAllowImm)
         {
            _glVertexAttrib4fv_int(current_shader->attr_map[ATTR_IDX_COLOR],
                                   tkopengl_imm_color_array
                                   );
         }
         else
         {
            _glVertexAttrib4fv_int(current_shader->attr_map[ATTR_IDX_COLOR],
                                   (sF32*)&current_color
                                   );
         }
      }

      if(Dattrvalid(ATTR_IDX_TEXCOORD))
      {
         _glDisableVertexAttribArray_int_vao(current_shader->attr_map[ATTR_IDX_TEXCOORD]);

         if(_bAllowImm)
         {
            _glVertexAttrib4fv_int(current_shader->attr_map[ATTR_IDX_TEXCOORD],
                                   tkopengl_imm_texcoord_array
                                   );
         }
         else
         {
            _glVertexAttrib4fv_int(current_shader->attr_map[ATTR_IDX_TEXCOORD],
                                   (sF32*)&current_texcoord
                                   );
         }
      }
   }

#ifdef DX_EMU_USE_VAO
   if(tkopengl_builtin_vao_id > 0u)
      _glBindVertexArray(tkopengl_builtin_vao_id);
#endif // DX_EMU_USE_VAO
}

/* ----------------------------------------------------------------------------- loc_bind_uniforms() */
static void loc_bind_uniforms(void) {
   // Bind transformation (modelview*projection) matrix
   _glUniformMatrix4fv_int(current_shader->uni_map[UNI_IDX_TRANSFORM],
                           1/*count*/,
                           GL_FALSE/*transpose*/,
                           (GLfloat*)tkopengl_transform
                           );

   if(current_shader->uni_map[UNI_IDX_SAMPLER] >= 0)
   {
      _glUniform1i(current_shader->uni_map[UNI_IDX_SAMPLER],
                   0 /* texture unit */
                   );

#ifdef DX_EMU_TEXTURE_RECTANGLE
      if(current_shader->uni_map[UNI_IDX_OBTEXSIZE] >= 0)
      {
         if(-1 != Dactivetextureunit(bound_nr))
         {
            _glUniform2f(current_shader->uni_map[UNI_IDX_OBTEXSIZE],
                         1.0f / Dboundtexture(sx),
                         1.0f / Dboundtexture(sy)
                         );
         }
         else
         {
            _glUniform2f(current_shader->uni_map[UNI_IDX_OBTEXSIZE], 0.0f, 0.0f);
            yac_host->printf("[---] tkopengl::shader::bind_uniforms: no texture bound to active texture unit, cannot set obtexsize\n");
         }
      }
#endif // DX_EMU_TEXTURE_RECTANGLE
   }

   if(current_shader->uni_map[UNI_IDX_ENVCOLOR] >= 0)
   {
      _glUniform4fv_int(current_shader->uni_map[UNI_IDX_ENVCOLOR],
                        1,
                        Dactivetextureunit(env_color)
                        );
   }

   if(current_shader->uni_map[UNI_IDX_ALPHAREF] >= 0)
   {
      _glUniform1f(current_shader->uni_map[UNI_IDX_ALPHAREF], tkopengl_attrib->alphatest.ref);
   }

   if(current_shader->uni_map[UNI_IDX_POINT_SIZE] >= 0)
   {
      _glUniform1f(current_shader->uni_map[UNI_IDX_POINT_SIZE], tkopengl_attrib->point.size);

      if(current_shader->uni_map[UNI_IDX_POINT_MINMAX] >= 0)
      {
         _glUniform2f(current_shader->uni_map[UNI_IDX_POINT_MINMAX],
                      tkopengl_attrib->point.min_size,
                      tkopengl_attrib->point.max_size
                      );
      }

      if(current_shader->uni_map[UNI_IDX_POINT_ATT] >= 0)
      {
         _glUniform3f(current_shader->uni_map[UNI_IDX_POINT_ATT],
                      tkopengl_attrib->point.attenuation.constant,
                      tkopengl_attrib->point.attenuation.linear,
                      tkopengl_attrib->point.attenuation.quadratic
                      );
      }
   }

#ifdef DX_EMU_FOG
   if(current_shader->uni_map[UNI_IDX_FOGCOLOR] >= 0)
   {
      _glUniform4fv_int(current_shader->uni_map[UNI_IDX_FOGCOLOR], 1, tkopengl_attrib->fog.color);

      if(current_shader->uni_map[UNI_IDX_FOGOBDELTA] >= 0)
      {
         sF32 obdelta = (tkopengl_attrib->fog.end - tkopengl_attrib->fog.start);
         if(obdelta != 0.0f)
         {
            obdelta = 1.0f / obdelta;
         }
         _glUniform1f(current_shader->uni_map[UNI_IDX_FOGOBDELTA], obdelta);
      }

      if(current_shader->uni_map[UNI_IDX_FOGEND] >= 0)
      {
         _glUniform1f(current_shader->uni_map[UNI_IDX_FOGEND], tkopengl_attrib->fog.end);
      }

      if(current_shader->uni_map[UNI_IDX_FOGDENSITY] >= 0)
      {
         _glUniform1f(current_shader->uni_map[UNI_IDX_FOGDENSITY], tkopengl_attrib->fog.density);
      }
   }
#endif // DX_EMU_FOG


#ifdef DX_EMU_LIGHT
   if(current_shader->uni_map[UNI_IDX_NORMALMATRIX] >= 0) // (note) only required for light shaders
   {
      _glUniformMatrix3fv_int(current_shader->uni_map[UNI_IDX_NORMALMATRIX],
                              1/*count*/,
                              GL_FALSE/*transpose*/,
                              (GLfloat*)tkopengl_normalmatrix
                              );

      //printf("xxx mview[0]=%f\n", tkopengl_modelview_matrix_stack[tkopengl_modelview_matrix_stacki << 4]);

      if(current_shader->uni_map[UNI_IDX_MODELVIEW] >= 0)
      {
         _glUniformMatrix4fv_int(current_shader->uni_map[UNI_IDX_MODELVIEW],
                                 1/*count*/,
                                 GL_FALSE/*transpose*/,
                                 (GLfloat*) &tkopengl_modelview_matrix_stack[tkopengl_modelview_matrix_stacki << 4]
                                 );
      }

      if(current_shader->uni_map[UNI_IDX_LIGHTACS] >= 0)
      {
         _glUniform4fv_int(current_shader->uni_map[UNI_IDX_LIGHTACS],
                           1/*count*/,
                           tkopengl_attrib->light.model_ambient
                           );
      }

      if(current_shader->uni_map[UNI_IDX_LIGHTECM] >= 0)
      {
         _glUniform4fv_int(current_shader->uni_map[UNI_IDX_LIGHTECM],
                           1/*count*/,
                           tkopengl_attrib->light.material.emission_color
                           );
      }

      if(current_shader->uni_map[UNI_IDX_LIGHTACM] >= 0)
      {
         _glUniform4fv_int(current_shader->uni_map[UNI_IDX_LIGHTACM],
                           1/*count*/,
                           tkopengl_attrib->light.material.ambient_color
                           );
      }

      if(current_shader->uni_map[UNI_IDX_LIGHTDCM] >= 0)
      {
         _glUniform4fv_int(current_shader->uni_map[UNI_IDX_LIGHTDCM],
                           1/*count*/,
                           tkopengl_attrib->light.material.diffuse_color
                           );
      }

      if(current_shader->uni_map[UNI_IDX_LIGHTSCM] >= 0)
      {
         _glUniform4fv_int(current_shader->uni_map[UNI_IDX_LIGHTSCM],
                           1/*count*/,
                           tkopengl_attrib->light.material.specular_color
                           );

         if(current_shader->uni_map[UNI_IDX_LIGHTSRM] >= 0)
         {
            _glUniform1f(current_shader->uni_map[UNI_IDX_LIGHTSRM],
                         tkopengl_attrib->light.material.specular_exponent
                         );
         }
      }

      if(current_shader->uni_map[UNI_IDX_LIGHT0POS] >= 0)
      {
         _glUniform4fv_int(current_shader->uni_map[UNI_IDX_LIGHT0POS],
                           1/*count*/,
                           tkopengl_attrib->light.lights[0].position_transform
                           );

#if 0
         printf("xxx tkopengl_attrib->light.lights[0].position_transform = %f %f %f %f\n",
                tkopengl_attrib->light.lights[0].position_transform[0],
                tkopengl_attrib->light.lights[0].position_transform[1],
                tkopengl_attrib->light.lights[0].position_transform[2],
                tkopengl_attrib->light.lights[0].position_transform[3]
                );
#endif

         if(current_shader->uni_map[UNI_IDX_LIGHT0ACL] >= 0)
         {
            _glUniform4fv_int(current_shader->uni_map[UNI_IDX_LIGHT0ACL],
                              1/*count*/,
                              tkopengl_attrib->light.lights[0].ambient_color
                              );
         }

         if(current_shader->uni_map[UNI_IDX_LIGHT0DCL] >= 0)
         {
            _glUniform4fv_int(current_shader->uni_map[UNI_IDX_LIGHT0DCL],
                              1/*count*/,
                              tkopengl_attrib->light.lights[0].diffuse_color
                              );
         }

         if(current_shader->uni_map[UNI_IDX_LIGHT0SCL] >= 0)
         {
            _glUniform4fv_int(current_shader->uni_map[UNI_IDX_LIGHT0SCL],
                              1/*count*/,
                              tkopengl_attrib->light.lights[0].specular_color
                              );
         }

         if(current_shader->uni_map[UNI_IDX_LIGHT0ATTL] >= 0)
         {
            _glUniform3f(current_shader->uni_map[UNI_IDX_LIGHT0ATTL],
                         tkopengl_attrib->light.lights[0].constant_attenuation,
                         tkopengl_attrib->light.lights[0].linear_attenuation,
                         tkopengl_attrib->light.lights[0].quadratic_attenuation
                         );
         }

         if(current_shader->uni_map[UNI_IDX_LIGHT0SDL] >= 0)
         {
            _glUniform3fv_int(current_shader->uni_map[UNI_IDX_LIGHT0SDL],
                              1/*count*/,
                              tkopengl_attrib->light.lights[0].spot_direction
                              );

            if(current_shader->uni_map[UNI_IDX_LIGHT0SRL] >= 0)
            {
               _glUniform1f(current_shader->uni_map[UNI_IDX_LIGHT0SRL],
                            tkopengl_attrib->light.lights[0].spot_exponent
                            );
            }

            if(current_shader->uni_map[UNI_IDX_LIGHT0CRLCOS] >= 0)
            {
               _glUniform1f(current_shader->uni_map[UNI_IDX_LIGHT0CRLCOS],
                            (sF32)cos(tkopengl_attrib->light.lights[0].spot_cutoff)
                            );
            }
         }

      } // light0
   }
#endif // DX_EMU_LIGHT

   if(current_shader->uni_map[UNI_IDX_TEXTUREMATRIX] >= 0)
   {
      _glUniformMatrix4fv_int(current_shader->uni_map[UNI_IDX_TEXTUREMATRIX],
                              1/*count*/,
                              GL_FALSE/*transpose*/,
                              (GLfloat*)&tkopengl_texture_matrix_stack[tkopengl_texture_matrix_stacki << 4]
                              );
   }

}

/* ----------------------------------------------------------------------------- tkopengl_shader_draw_begin() */
void tkopengl_shader_draw_begin(GLenum _drawMode,
                                sBool _bAllowImm,
                                sUI   _copyNumIndices,
                                sUI   _copyFirstVertex,
                                sUI   _copyNumVertices
                                ) {
   // called by glDrawArrays, glDrawElements, glBegin..glEnd DX_EMU_FIXFXN
   Demuprintfv("[trc] tkopengl_shader_draw_begin: drawMode=%d bAllowImm=%d copyNumIndices=%u copyFirstVertex=%u copyNumVertices=%u\n", _drawMode, _bAllowImm, _copyNumIndices, _copyFirstVertex, _copyNumVertices);

#if defined(DX_EMU_USE_VBO)
   if(0u == tkopengl_imm_index_buf_id)
      tkopengl_imm_lazy_init_buffers();
#endif // DX_EMU_USE_VBO

   if(!b_tkopengl_custom_shader)
   {
      loc_select_shader(_drawMode);
   }
   // else
   // {
      //yac_host->printf("[dbg] tkopengl_shader_draw_begin: use custom shader\n");
      ////return;
   // }

   if(NULL == current_shader)
   {
      // happens if max.num shaders is exceeded
      return;
   }

#ifndef DX_GLES
   if(0 != (current_shader->vp_features & TKOPENGL_VP_FEATURE_POINT_SIZE))
   {
      ::glEnable(GL_PROGRAM_POINT_SIZE);
      Dtraceglerror("glEnable(GL_PROGRAM_POINT_SIZE)");
   }
#endif // DX_GLES

   gles_shader_bind(current_shader);

   loc_bind_attribs(_drawMode, _bAllowImm);

   loc_bind_uniforms();
}

/* ----------------------------------------------------------------------------- tkopengl_shader_draw_end() */
void tkopengl_shader_draw_end(void) {

   // // if(b_tkopengl_custom_shader)
   // // {
   // //    return;
   // // }

   if(NULL == current_shader)
   {
      // happens if max.num shaders is exceeded
      return;
   }

#ifndef DX_GLES
   if(0 != (current_shader->vp_features & TKOPENGL_VP_FEATURE_POINT_SIZE))
   {
      ::glDisable(GL_PROGRAM_POINT_SIZE);
      Dtraceglerror("glDisable(GL_PROGRAM_POINT_SIZE)");
   }
#endif // DX_GLES

   if(Dattrvalid(ATTR_IDX_TEXCOORD))
   {
      _glDisableVertexAttribArray(current_shader->attr_map[ATTR_IDX_TEXCOORD]);
   }

   if(Dattrvalid(ATTR_IDX_COLOR))
   {
      _glDisableVertexAttribArray(current_shader->attr_map[ATTR_IDX_COLOR]);
   }

   if(Dattrvalid(ATTR_IDX_NORMAL))
   {
      _glDisableVertexAttribArray(current_shader->attr_map[ATTR_IDX_NORMAL]);
   }

   if(Dattrvalid(ATTR_IDX_VERTEX))
   {
      _glDisableVertexAttribArray(current_shader->attr_map[ATTR_IDX_VERTEX]);
   }

#ifdef DX_EMU_USE_VAO
   if(tkopengl_builtin_vao_id > 0u)
      _glBindVertexArray(0u);
#endif // DX_EMU_USE_VAO

   if(!b_tkopengl_custom_shader)
   {
      _glUseProgram_int(0);
   }
}

#endif // DX_EMU_FIXFXN


/* ----------------------------------------------------------------------------- tkopengl_shader_draw_end() */
YAC_String _ZGLShader::def_version_override;

void _ZGLShader::SetDefaultPlatformVersion(void) {
   // called during YAC_Init()
#ifdef DX_GL4_CORE
   def_version_override.visit("410 core\n");
#else
#ifdef YAC_MACOS
   // (todo) macOS also supports GL 4.1 core profile ("#version 410 core\n" shaders)
   def_version_override.visit("120 \n");
#endif // YAC_MACOS
#endif // DX_GL4_CORE
}

_ZGLShader::_ZGLShader(void) {
   memset(&gles_shader, 0, sizeof(gles_shader_t));
}

_ZGLShader::~_ZGLShader() {
   // yac_host->printf("[dbg] ~_ZGLShader 1 (ENTER)\n");
   gles_shader_destroy(&gles_shader);
   // yac_host->printf("[dbg] ~_ZGLShader 2 (LEAVE)\n");
}

void _ZGLShader::lazyAddVersionHeader(sBool _bFrag, YAC_String *_src, YAC_String *_origSrc) {
   // called by setVertexSource(), setFragmentSource()
   if((NULL != _origSrc) && (NULL != _origSrc->chars) && (_origSrc->chars[0] != '#'))
   {
      _src->append("#version ");

      if(def_version_override.length > 1u)
      {
         _src->append(&def_version_override);
#ifdef DX_GLES
         if(_bFrag)
            _src->append("precision mediump float;\n");
         else
            _src->append("precision highp float;\n");
#endif // DX_GLES
      }
      else
      {
#ifdef DX_GLES
#ifdef DX_GLES_3
         _src->append("300 es\n");
#else
         _src->append("100\n");
#endif // DX_GLES_3
         if(_bFrag)
            _src->append("precision mediump float;\n");
         else
            _src->append("precision highp float;\n");
#else
         _src->append("130\n");
#endif // DX_GLES
      }
      // yac_host->printf("xxx ZGLShader::lazyAddVersionHeader: bFrag=%d _src=\"%s\" def_version_override=\"%s\"\n", _bFrag, (const char*)_src->chars, (const char*)def_version_override.chars);
   }
}

void _ZGLShader::SetDefaultVersion(YAC_String *_ver) {
   if(YAC_Is_String(_ver))
   {
      // yac_host->printf("xxx ZGLShader::SetDefaultVersion: ver=\"%s\" def_version_override=\"%s\n", (const char*)_ver->chars, (const char*)def_version_override.chars);
      def_version_override.copy(_ver);
   }
   else
   {
      def_version_override.free();
   }
}

void _ZGLShader::setVertexSource(YAC_String *_src) {
   vert_src.empty();
   lazyAddVersionHeader(YAC_FALSE/*bFrag*/, &vert_src, _src);
   vert_src.append(_src);
}

void _ZGLShader::getVertexSource(YAC_Value *_r) {
   YAC_RETSC(&vert_src);
}

void _ZGLShader::setFragmentSource(YAC_String *_src) {
   frag_src.empty();
   lazyAddVersionHeader(YAC_TRUE/*bFrag*/, &frag_src, _src);
   frag_src.append(_src);
}

void _ZGLShader::getFragmentSource(YAC_Value *_r) {
   YAC_RETSC(&frag_src);
}

sBool _ZGLShader::create(void) {
   sSI ret;

   ret = gles_shader_create(&gles_shader,
                            (const sChar*)vert_src.chars,
                            (const sChar*)frag_src.chars
                            );

#ifdef DX_EMU_FIXFXN
   if(0 == ret)
   {
      loc_shader_resolve(&gles_shader);
   }
#endif // DX_EMU_FIXFXN

   return (0 == ret);
}

void _ZGLShader::bind(void) {
#ifdef DX_EMU_FIXFXN
   current_shader = &gles_shader;
#endif // DX_EMU_FIXFXN

   gles_shader_bind(&gles_shader);

#ifdef DX_EMU_FIXFXN
   b_tkopengl_custom_shader = YAC_TRUE;
#endif // DX_EMU_FIXFXN
}

sSI _ZGLShader::getUniformLocation(YAC_String *_name) {
   return _glGetUniformLocation(gles_shader.prg_id, _name);
}

sSI _ZGLShader::getAttribLocation(YAC_String *_name) {
   return _glGetAttribLocation(gles_shader.prg_id, _name);
}

sUI _ZGLShader::getProgramId(void) {
   return gles_shader.prg_id;
}

sUI _ZGLShader::getFragmentShaderId(void) {
   return gles_shader.fp_id;
}

sUI _ZGLShader::getVertexShaderId(void) {
   return gles_shader.vp_id;
}

sUI _ZGLShader::getNumActiveAttributes(void) {
   sSI ret;
   _glGetProgramiv_int(gles_shader.prg_id,
                       GL_ACTIVE_ATTRIBUTES,
                       &ret
                       );
   return sUI(ret);
}

sUI _ZGLShader::getNumActiveUniforms(void) {
   sSI ret;
   _glGetProgramiv_int(gles_shader.prg_id,
                       GL_ACTIVE_UNIFORMS,
                       &ret
                       );
   return sUI(ret);
}

sUI _ZGLShader::getProgramBinaryLength(void) {
   sSI ret;
   _glGetProgramiv_int(gles_shader.prg_id,
                       GL_PROGRAM_BINARY_LENGTH,
                       &ret
                       );
   return sUI(ret);
}

void _ZGLShader::unbind(void) {
   _glUseProgram_int(0);

#ifdef DX_EMU_FIXFXN
   b_tkopengl_custom_shader = YAC_FALSE;
   current_shader = NULL;
#endif // DX_EMU_FIXFXN
}

void _ZGLShader::destroy(void) {
   gles_shader_destroy(&gles_shader);
}
