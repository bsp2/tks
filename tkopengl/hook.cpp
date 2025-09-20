/// hook.cpp
///
/// (c) 2013-2025 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#define HOOK_DEBUG defined

#include <stdlib.h>
#include <math.h>
#include <string.h>

#define YAC_PRINTF defined
#include <yac.h>

extern YAC_Host *yac_host;

#ifdef YAC_WIN32
// (modified) SDL 1.x on Windows
#include "../tksdl/tksdl_hooks.h"
#else
// SDL 2.x on macOS, Linux
#include "../tksdl2/tksdl_hooks.h"
#endif // YAC_WIN32

#include "inc_opengl.h"
#include "tkopengl.h"

#include "glerror.h"

#include "tks-list.h"

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

#ifdef DX_EMU_FIXFXN
#include "shader.h"
#endif


static sBool b_hook_init = YAC_FALSE;

static tksdl_hook_t hook_open;
static tksdl_hook_t hook_close;


static void loc_load_gl_defaults(void) {
   // yac_host->printf("xxx loc_load_gl_defaults ENTER glGetError=%d\n", glGetError());
   tkopengl_lazyinit_tksdl_b_glcore();
   sBool bGLCore = (NULL != tksdl_b_glcore) && *tksdl_b_glcore;
   sBool bGLCompat = !bGLCore;

   _glDisable(GL_BLEND);

   if(bGLCompat)
      _glDisable(GL_ALPHA_TEST);

#if !defined(DX_GLES) && !defined(DX_EMU_FIXFXN)
   if(bGLCompat)
      _glDisable(GL_COLOR_LOGIC_OP);
#endif // DX_GLES

   if(bGLCompat)
      _glDisable(GL_COLOR_MATERIAL);

   _glDisable(GL_CULL_FACE);
   _glDisable(GL_DEPTH_TEST);
   _glDisable(GL_SCISSOR_TEST);
   _glDisable(GL_STENCIL_TEST);
   _glDepthFunc(GL_LESS);
   _glClearDepth(1.0f);
   _glDisable(GL_DITHER);

   if(bGLCompat)
      _glDisable(GL_FOG);

#ifndef DX_GLES
   if(bGLCompat)
      _glDisable(GL_INDEX_LOGIC_OP);
#endif // DX_GLES

   if(bGLCompat)
   {
      _glDisable(GL_LIGHT0);
      _glDisable(GL_LIGHT1);
      _glDisable(GL_LIGHT2);
      _glDisable(GL_LIGHT3);
      _glDisable(GL_LIGHT4);
      _glDisable(GL_LIGHT5);
      _glDisable(GL_LIGHT6);
      _glDisable(GL_LIGHT7);
      _glDisable(GL_LIGHTING);
      _glDisable(GL_LINE_STIPPLE);
   }

   _glDisable(GL_LINE_SMOOTH);

   // _glDisable(GL_MAP1_COLOR_4);
   // _glDisable(GL_MAP1_INDEX);
   // _glDisable(GL_MAP1_NORMAL);
   // _glDisable(GL_MAP1_TEXTURE_COORD_1);
   // _glDisable(GL_MAP1_TEXTURE_COORD_2);
   // _glDisable(GL_MAP1_TEXTURE_COORD_3);
   // _glDisable(GL_MAP1_TEXTURE_COORD_4);
   // _glDisable(GL_MAP1_VERTEX_3);
   // _glDisable(GL_MAP1_VERTEX_4);
   // _glDisable(GL_MAP2_COLOR_4);
   // _glDisable(GL_MAP2_INDEX);
   // _glDisable(GL_MAP2_NORMAL);
   // _glDisable(GL_MAP2_TEXTURE_COORD_1);
   // _glDisable(GL_MAP2_TEXTURE_COORD_2);
   // _glDisable(GL_MAP2_TEXTURE_COORD_3);
   // _glDisable(GL_MAP2_TEXTURE_COORD_4);
   // _glDisable(GL_MAP2_VERTEX_3);
   // _glDisable(GL_MAP2_VERTEX_4);

#ifndef DX_GLES
   if(bGLCompat)
   {
      _glDisable(GL_NORMALIZE);
      _glDisable(GL_POINT_SMOOTH);
   }
#endif // DX_GLES

   if(bGLCompat)
      _glDisable(GL_POINT_SPRITE);

   // _glDisable(GL_POLYGON_OFFSET_FILL);
   // _glDisable(GL_POLYGON_OFFSET_LINE);
   // _glDisable(GL_POLYGON_OFFSET_POINT);

   if(bGLCompat)
   {
      _glDisable(GL_POLYGON_SMOOTH);
      _glDisable(GL_POLYGON_STIPPLE);
      _glDisable(GL_TEXTURE_1D);
      _glDisable(GL_TEXTURE_2D);
      _glDisable(GL_TEXTURE_3D);
   }

   if(bGLCompat)
   {
      _glDisable(GL_TEXTURE_GEN_Q);
      _glDisable(GL_TEXTURE_GEN_R);
      _glDisable(GL_TEXTURE_GEN_S);
      _glDisable(GL_TEXTURE_GEN_T);

      ////glShadeModel(GL_FLAT);
      _glShadeModel(GL_SMOOTH);

      _glHint(GL_FOG_HINT, GL_NICEST);
      _glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
      _glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
   }

   _glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
   _glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

   _glBindFramebuffer(GL_FRAMEBUFFER, 0);
   _glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
   _glDepthMask(GL_TRUE);

   _glDisable(GL_SAMPLE_COVERAGE);
}

/* ----------------------------------------------------------------------------- tkopengl_hook_install() */
void tkopengl_hook_install(void) {
   hook_open.fxn = &tkopengl_hook_open_cbk;
   hook_open.next = NULL;
   tksdl_hook_add(TKSDL_HOOK_NAME_OPEN, &hook_open);

   hook_close.fxn = &tkopengl_hook_close_cbk;
   hook_close.next = NULL;
   tksdl_hook_add(TKSDL_HOOK_NAME_CLOSE, &hook_close);
}

/* ----------------------------------------------------------------------------- tkopengl_hook_remove() */
void tkopengl_hook_remove(void) {

   tksdl_hook_remove(TKSDL_HOOK_NAME_CLOSE, &hook_close);
   tksdl_hook_remove(TKSDL_HOOK_NAME_OPEN,  &hook_open);
}

/* ----------------------------------------------------------------------------- tkopengl_hook_open_cbk() */
void tkopengl_hook_open_cbk(void) {
   // called when viewport is (re-)opened

#ifdef HOOK_DEBUG
   if(yac_host->yacGetDebugLevel() >= 2)
      yac_host->printf("[dbg] tkopengl_hook_open_cbk() called.\n");
#endif

   if(!b_hook_init)
   {
      // yac_host->printf("xxx tkopengl_hook_open_cbk 1 glGetError=%d\n", glGetError());

      _zglLoadExtensions();

#ifdef DX_EMU_IMM
      tkopengl_imm_reset();
#endif // DX_EMU_IMM

#ifdef DX_EMU_ATTRIB
      tkopengl_attrib_init();
#endif

#ifdef DX_EMU_FIXFXN
      tkopengl_shader_init();
#endif // DX_EMU_FIXFXN

      loc_load_gl_defaults();

      b_hook_init = YAC_TRUE;
   }

}

/* ----------------------------------------------------------------------------- tkopengl_hook_close_cbk() */
void tkopengl_hook_close_cbk(void) {

#ifdef HOOK_DEBUG
   if(yac_host->yacGetDebugLevel() >= 2)
      yac_host->printf("[dbg] tkopengl_hook_close_cbk() called.\n");
#endif

   // called when viewport is closed or eventloop terminates

   if(b_hook_init)
   {
#ifdef DX_EMU_FIXFXN
      // yac_host->printf("[dbg] tkopengl_shader_exit() 1\n");
      tkopengl_shader_exit();
      // yac_host->printf("[dbg] tkopengl_shader_exit() 2\n");
#endif // DX_EMU_FIXFXN

      b_hook_init = YAC_FALSE;
   }
}
