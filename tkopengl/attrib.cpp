/// attrib.cpp
///
/// (c) 2013-2025 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

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

#ifdef DX_EMU_ATTRIB

#include "attrib.h"
#include "matrix.h"
#include "immediate.h"


static tkopengl_attrib_t attrib_stack[TKOPENGL_ATTRIB_STACK_SIZE];
static sSI attrib_stacki;
tkopengl_attrib_t *tkopengl_attrib = NULL;

tkopengl_texture_t tkopengl_textures[TKOPENGL_MAX_TEXTURES];


/* ----------------------------------------------------------------------------- tkopengl_attrib_init() */
void tkopengl_attrib_init(void) {

   memset(attrib_stack, 0, sizeof(tkopengl_attrib_t) * TKOPENGL_ATTRIB_STACK_SIZE);

   // Attrib stack
   attrib_stacki = (TKOPENGL_ATTRIB_STACK_SIZE - 1);
   tkopengl_attrib = &attrib_stack[attrib_stacki];

#ifdef DX_EMU_FOG
   /* GL_FOG */
   tkopengl_attrib->fog.b_enable   = YAC_FALSE;
   tkopengl_attrib->fog.mode       = GL_EXP;
   tkopengl_attrib->fog.alpha_mode = GL_REPLACE;
   tkopengl_attrib->fog.density    = 1.0f;
   tkopengl_attrib->fog.start      = 0.0f;
   tkopengl_attrib->fog.end        = 1.0f;
   tkopengl_attrib->fog.color[0]   = 0.0f;
   tkopengl_attrib->fog.color[1]   = 0.0f;
   tkopengl_attrib->fog.color[2]   = 0.0f;
   tkopengl_attrib->fog.color[3]   = 0.0f;
#endif // DX_EMU_FOG

   /* GL_ALPHA_TEST */
   tkopengl_attrib->alphatest.b_enable = YAC_FALSE;
   tkopengl_attrib->alphatest.func     = GL_ALWAYS;
   tkopengl_attrib->alphatest.ref      = 0.0f;

   /* texture */
   {
      sUI i;

      tkopengl_attrib->texture.active_unit = 0;

      for(i=0; i<TKOPENGL_MAX_TEXTURE_UNITS; i++)
      {
         tkopengl_attrib->texture.units[i].mode = 0;
         tkopengl_attrib->texture.units[i].target = 0;
         tkopengl_attrib->texture.units[i].bound_nr = -1;
         tkopengl_attrib->texture.units[i].env_mode = GL_REPLACE;
         tkopengl_attrib->texture.units[i].env_color[0] = 0.0f;
         tkopengl_attrib->texture.units[i].env_color[1] = 0.0f;
         tkopengl_attrib->texture.units[i].env_color[2] = 0.0f;
         tkopengl_attrib->texture.units[i].env_color[3] = 0.0f;
      }

      for(i=0; i<TKOPENGL_MAX_TEXTURES; i++)
      {
#ifdef DX_EMU_ATTRIB_TEXTURE_ID_MAP
         tkopengl_textures[i].id              = (GLuint)-1;
#endif
         tkopengl_textures[i].internal_format = (GLenum) 0; // unused
         tkopengl_textures[i].sx              = 0;
         tkopengl_textures[i].sy              = 0;
      }
   }


   /* polygon mode */
   tkopengl_attrib->polygon_mode = GL_FILL;

   /* points */
   tkopengl_attrib->point.size     = 1.0f;
   tkopengl_attrib->point.min_size = 0.0f;
   tkopengl_attrib->point.max_size = 1.0f;
   tkopengl_attrib->point.fade_threshold_size = 1.0f;
   tkopengl_attrib->point.attenuation.constant  = 1.0f;
   tkopengl_attrib->point.attenuation.linear    = 0.0f;
   tkopengl_attrib->point.attenuation.quadratic = 0.0f;
   tkopengl_attrib->point.sprite.b_coord_replace = YAC_FALSE;
   tkopengl_attrib->point.sprite.b_enable        = YAC_FALSE;

#ifdef DX_EMU_LIGHT
   /* lighting */
   sUI i;
   tkopengl_attrib->light.b_enable       = YAC_FALSE;
   tkopengl_attrib->light.enable_mask    = 0;
   tkopengl_attrib->light.color_control  = GL_SINGLE_COLOR;
   tkopengl_attrib->light.b_local_viewer = YAC_FALSE;

   tkopengl_attrib->light.model_ambient[0] = 0.2f;
   tkopengl_attrib->light.model_ambient[1] = 0.2f;
   tkopengl_attrib->light.model_ambient[2] = 0.2f;
   tkopengl_attrib->light.model_ambient[3] = 1.0f;

   tkopengl_attrib->light.material.ambient_color[0] = 0.2f;
   tkopengl_attrib->light.material.ambient_color[1] = 0.2f;
   tkopengl_attrib->light.material.ambient_color[2] = 0.2f;
   tkopengl_attrib->light.material.ambient_color[3] = 1.0f;

   tkopengl_attrib->light.material.diffuse_color[0] = 0.8f;
   tkopengl_attrib->light.material.diffuse_color[1] = 0.8f;
   tkopengl_attrib->light.material.diffuse_color[2] = 0.8f;
   tkopengl_attrib->light.material.diffuse_color[3] = 1.0f;

   tkopengl_attrib->light.material.specular_color[0] = 0.0f;
   tkopengl_attrib->light.material.specular_color[1] = 0.0f;
   tkopengl_attrib->light.material.specular_color[2] = 0.0f;
   tkopengl_attrib->light.material.specular_color[3] = 1.0f;

   tkopengl_attrib->light.material.emission_color[0] = 0.0f;
   tkopengl_attrib->light.material.emission_color[1] = 0.0f;
   tkopengl_attrib->light.material.emission_color[2] = 0.0f;
   tkopengl_attrib->light.material.emission_color[3] = 1.0f;

   tkopengl_attrib->light.material.specular_exponent = 0.0f; // shininess

   for(i=0; i<TKOPENGL_MAX_LIGHTS; i++)
   {
      tkopengl_light_t *light = &tkopengl_attrib->light.lights[i];
      sF32 diffSpec0 = (0 == i) ? 1.0f : 0.0f;

      light->ambient_color[0] = 0.0f;
      light->ambient_color[1] = 0.0f;
      light->ambient_color[2] = 0.0f;
      light->ambient_color[3] = 1.0f;
      light->diffuse_color[0] = diffSpec0;
      light->diffuse_color[1] = diffSpec0;
      light->diffuse_color[2] = diffSpec0;
      light->diffuse_color[3] = diffSpec0;
      light->specular_color[0] = diffSpec0;
      light->specular_color[1] = diffSpec0;
      light->specular_color[2] = diffSpec0;
      light->specular_color[3] = diffSpec0;
      light->position[0] = 0.0f;
      light->position[1] = 0.0f;
      light->position[2] = 1.0f;
      light->position[3] = 0.0f; // directional
      light->position_transform[0] = 0.0f;
      light->position_transform[1] = 0.0f;
      light->position_transform[2] = 1.0f;
      light->position_transform[3] = 0.0f; // directional
      light->spot_direction[0] =  0.0f;
      light->spot_direction[1] =  0.0f;
      light->spot_direction[2] = -1.0f;
      light->spot_exponent         = 0.0f;
      light->spot_cutoff           = 180.0f;
      light->constant_attenuation  = 1.0f;
      light->linear_attenuation    = 0.0f;
      light->quadratic_attenuation = 0.0f;
   }
#endif // DX_EMU_LIGHT

}

/* ----------------------------------------------------------------------------- loc_attrib_restore() */
static void loc_attrib_restore(void) {
   // restore viewport
   ::glViewport(tkopengl_attrib->viewport.x,
                tkopengl_attrib->viewport.y,
                tkopengl_attrib->viewport.w,
                tkopengl_attrib->viewport.h
                );
}

/* ----------------------------------------------------------------------------- tkopengl_attrib_push() */
void tkopengl_attrib_push(void) {
   if(attrib_stacki > 0)
   {
      attrib_stacki--;
      tkopengl_attrib--;
      *tkopengl_attrib = tkopengl_attrib[1];

      loc_attrib_restore();
   }
   else
   {
      yac_host->printf("[---] tkopengl_attrib_push: stack underflow\n");
   }
}

/* ----------------------------------------------------------------------------- tkopengl_attrib_pop() */
void tkopengl_attrib_pop(void) {
   if(attrib_stacki < (TKOPENGL_ATTRIB_STACK_SIZE - 1))
   {
      attrib_stacki++;
      tkopengl_attrib++;

      loc_attrib_restore();
   }
   else
   {
      yac_host->printf("[---] tkopengl_attrib_push: stack overflow (stacksize=%d)\n", TKOPENGL_ATTRIB_STACK_SIZE);
   }
}

#endif // DX_EMU_ATTRIB
