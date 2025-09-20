// immediate.cpp
///
/// (c) 2013-2025 Bastian Spiegel <bs@tkscript.de> 
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

#include "tkopengl.h"

#include "glerror.h"

#include "tks-list.h"

#include "bitmap.h"
#include "image.h"
#include "tkstexture.h"
#include "tksopengl.h"

#ifdef DX_EMU_IMM

#include "immediate.h"

#ifdef DX_EMU_FIXFXN
#include "shader.h"
#endif

imm_vertex_t   current_vertex;
imm_normal_t   current_normal;
imm_color_t    current_color;
imm_texcoord_t current_texcoord; // s,t,r,q

#ifdef DX_EMU_PRIM
sU16 *tkopengl_imm_quads_to_triangles_array;
sU16 *tkopengl_imm_quads_to_lines_array;
sU16 *tkopengl_imm_triangles_to_lines_array;
sU16 *tkopengl_imm_quadstrip_to_lines_array;
sU16 *tkopengl_imm_trianglestrip_to_lines_array;
#endif // DX_EMU_PRIM

sU16 *tkopengl_tmp_index_array;

sF32 *tkopengl_imm_vertex_array;
sF32 *tkopengl_imm_normal_array;
sF32 *tkopengl_imm_color_array;
sF32 *tkopengl_imm_texcoord_array;

#ifdef DX_EMU_USE_VBO
GLuint tkopengl_imm_index_buf_id;
GLuint tkopengl_imm_vertex_buf_id;
GLuint tkopengl_imm_normal_buf_id;
GLuint tkopengl_imm_color_buf_id;
GLuint tkopengl_imm_texcoord_buf_id;
#endif // DX_EMU_USE_VBO

static imm_vertex_t   *next_vertex;
static imm_normal_t   *next_normal;
static imm_color_t    *next_color;
static imm_texcoord_t *next_texcoord;

#define FLAG_NORMAL    (1u << 1)
#define FLAG_COLOR     (1u << 2)
#define FLAG_TEXCOORD  (1u << 3)

static sUI current_flags;

static sUI current_idx;

static GLenum imm_mode; // GL_TRIANGLES, ..

tkopengl_imm_vertex_fxn_t   tkopengl_imm_vertex_cbk;
tkopengl_imm_normal_fxn_t   tkopengl_imm_normal_cbk;
tkopengl_imm_color_fxn_t    tkopengl_imm_color_cbk;
tkopengl_imm_texcoord_fxn_t tkopengl_imm_texcoord_cbk;

// outside of begin/end: just set current vertex/normal/..
static void loc_imm_vertex_current   (sF32 _x, sF32 _y, sF32 _z, sF32 _w);
static void loc_imm_normal_current   (sF32 _x, sF32 _y, sF32 _z);
static void loc_imm_color_current    (sF32 _r, sF32 _g, sF32 _b, sF32 _a);
static void loc_imm_texcoord_current (sF32 _s, sF32 _t, sF32 _r, sF32 _q);

// inside begin/end: GL_POINTS, GL_LINE_STRIP, GL_LINE_LOOP, GL_LINES, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_TRIANGLES (ogles2.x)
static void loc_imm_vertex_def   (sF32 _x, sF32 _y, sF32 _z, sF32 _w);
static void loc_imm_normal_def   (sF32 _x, sF32 _y, sF32 _z);
static void loc_imm_color_def    (sF32 _r, sF32 _g, sF32 _b, sF32 _a);
static void loc_imm_texcoord_def (sF32 _s, sF32 _t, sF32 _r, sF32 _q);


/* ----------------------------------------------------------------------------- loc_cbk_select_current() */
static void loc_cbk_select_current(void) {
   // outside of glBegin..glEnd (set constant vertex/normal/color/uv)
   tkopengl_imm_vertex_cbk   = &loc_imm_vertex_current;
   tkopengl_imm_normal_cbk   = &loc_imm_normal_current;
   tkopengl_imm_color_cbk    = &loc_imm_color_current;
   tkopengl_imm_texcoord_cbk = &loc_imm_texcoord_current;
}

/* ----------------------------------------------------------------------------- loc_cbk_select_def() */
static void loc_cbk_select_def(void) {
   // inside of glBegin..glEnd (=> emit next vertex/normal/color/uv)
   tkopengl_imm_vertex_cbk   = &loc_imm_vertex_def;
   tkopengl_imm_normal_cbk   = &loc_imm_normal_def;
   tkopengl_imm_color_cbk    = &loc_imm_color_def;
   tkopengl_imm_texcoord_cbk = &loc_imm_texcoord_def;
}

/* ----------------------------------------------------------------------------- tkopengl_imm_init() */
void tkopengl_imm_init(void) {

#ifdef DX_EMU_PRIM

#ifdef DX_EMU_USE_VBO
   tkopengl_imm_index_buf_id     = 0u;
   tkopengl_imm_vertex_buf_id    = 0u;
   tkopengl_imm_normal_buf_id    = 0u;
   tkopengl_imm_color_buf_id     = 0u;
   tkopengl_imm_texcoord_buf_id  = 0u;
#endif // DX_EMU_USE_VBO

   tkopengl_imm_quads_to_triangles_array = new sU16[ (TKOPENGL_IMM_ARRAY_SIZE/4) * 6];
   {
      sUI i;
      sU16 *d = tkopengl_imm_quads_to_triangles_array;
      for(i=0; i<TKOPENGL_IMM_ARRAY_SIZE; i+=4)
      {
         *d++ = i + 0;
         *d++ = i + 1;
         *d++ = i + 2;
         *d++ = i + 0;
         *d++ = i + 2;
         *d++ = i + 3;
      }
   }

#ifdef DX_EMU_PRIM_QUAD_LINE
   tkopengl_imm_quads_to_lines_array = new sU16[ (TKOPENGL_IMM_ARRAY_SIZE*2)];
   {
      sUI i;
      sU16 *d = tkopengl_imm_quads_to_lines_array;
      for(i=0; i<TKOPENGL_IMM_ARRAY_SIZE; i+=4)
      {
         *d++ = i + 0;
         *d++ = i + 1;
         *d++ = i + 1;
         *d++ = i + 2;
         *d++ = i + 2;
         *d++ = i + 3;
         *d++ = i + 3;
         *d++ = i + 0;
      }
   }
#endif // DX_EMU_PRIM_QUAD_LINE

#ifdef DX_EMU_PRIM_TRI_LINE
   tkopengl_imm_triangles_to_lines_array = new sU16[ (TKOPENGL_IMM_ARRAY_SIZE*2) +3];
   {
      sUI i;
      sU16 *d = tkopengl_imm_triangles_to_lines_array;
      for(i=0; i<TKOPENGL_IMM_ARRAY_SIZE; i+=3)
      {
         *d++ = i + 0;
         *d++ = i + 1;
         *d++ = i + 1;
         *d++ = i + 2;
         *d++ = i + 2;
         *d++ = i + 0;
      }
   }
#endif // DX_EMU_PRIM_TRI_LINE

#ifdef DX_EMU_PRIM_TRISTRIP_LINE
   tkopengl_imm_trianglestrip_to_lines_array = new sU16[ (TKOPENGL_IMM_ARRAY_SIZE-2) * 6];
   {
      sUI i;
      sU16 *d = tkopengl_imm_trianglestrip_to_lines_array;
      for(i=0; i<(TKOPENGL_IMM_ARRAY_SIZE-2); i++)
      {
         *d++ = i + 0;
         *d++ = i + 1;

         *d++ = i + 1;
         *d++ = i + 2;

         *d++ = i + 2;
         *d++ = i + 0;
      }
   }
#endif // DX_EMU_PRIM_TRISTRIP_LINE

#ifdef DX_EMU_PRIM_QUADSTRIP_LINE
   tkopengl_imm_quadstrip_to_lines_array = new sU16[ (TKOPENGL_IMM_ARRAY_SIZE/*-4*/) * (8/2) +2];
   {
      sUI i;
      sU16 *d = tkopengl_imm_quadstrip_to_lines_array;
      for(i=0; i<=(TKOPENGL_IMM_ARRAY_SIZE-4); i+=2)
      {
         *d++ = i + 0;
         *d++ = i + 1;

         *d++ = i + 1;
         *d++ = i + 3;

         *d++ = i + 3;
         *d++ = i + 2;

         *d++ = i + 2;
         *d++ = i + 0;
      }
   }
#endif // DX_EMU_PRIM_QUADSTRIP_LINE

#endif // DX_EMU_PRIM

   tkopengl_tmp_index_array    = new sU16[(TKOPENGL_IMM_ARRAY_SIZE-2) * 6];

   tkopengl_imm_vertex_array   = new sF32[TKOPENGL_IMM_ARRAY_SIZE * 4];
   tkopengl_imm_normal_array   = new sF32[(TKOPENGL_IMM_ARRAY_SIZE + 1) * 3];
   tkopengl_imm_color_array    = new sF32[(TKOPENGL_IMM_ARRAY_SIZE + 1) * 4];
   tkopengl_imm_texcoord_array = new sF32[(TKOPENGL_IMM_ARRAY_SIZE + 1) * 4];

   tkopengl_imm_normal_array   += 3;
   tkopengl_imm_color_array    += 4;
   tkopengl_imm_texcoord_array += 4;

   tkopengl_imm_reset();
}


/* ----------------------------------------------------------------------------- loc_alloc_array_buffer */
#ifdef DX_EMU_USE_VBO
static void loc_alloc_array_buffer(GLuint *_bufId, sUI _numBytes) {
   *_bufId = _zglGenBuffer();
   _glBindBuffer(GL_ARRAY_BUFFER, sSI(*_bufId));
   _zglBufferData(GL_ARRAY_BUFFER, _numBytes, NULL/*data*/, GL_STATIC_DRAW);
   _glBindBuffer(GL_ARRAY_BUFFER, 0);
}
#endif // DX_EMU_USE_VBO

/* ----------------------------------------------------------------------------- loc_alloc_element_buffer */
#ifdef DX_EMU_USE_VBO
static void loc_alloc_element_buffer(GLuint *_bufId, sUI _numBytes) {
   *_bufId = _zglGenBuffer();
   _glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sSI(*_bufId));
   _zglBufferData(GL_ELEMENT_ARRAY_BUFFER, _numBytes, NULL/*data*/, GL_STATIC_DRAW);
   _glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
#endif // DX_EMU_USE_VBO

/* ----------------------------------------------------------------------------- tkopengl_imm_lazy_init_buffers() */
#ifdef DX_EMU_USE_VBO
void tkopengl_imm_lazy_init_buffers(void) {
#ifdef DX_EMU_PRIM
   if(0u == tkopengl_imm_index_buf_id)
   {
      loc_alloc_element_buffer(&tkopengl_imm_index_buf_id,  sUI((TKOPENGL_IMM_ARRAY_SIZE-2) * 6 * sizeof(sU16) ));
      loc_alloc_array_buffer(&tkopengl_imm_vertex_buf_id,   sUI(  TKOPENGL_IMM_ARRAY_SIZE      * 4 * sizeof(sF32) ));
      loc_alloc_array_buffer(&tkopengl_imm_normal_buf_id,   sUI( (TKOPENGL_IMM_ARRAY_SIZE + 1) * 3 * sizeof(sF32) ));
      loc_alloc_array_buffer(&tkopengl_imm_color_buf_id,    sUI( (TKOPENGL_IMM_ARRAY_SIZE + 1) * 4 * sizeof(sF32) ));
      loc_alloc_array_buffer(&tkopengl_imm_texcoord_buf_id, sUI( (TKOPENGL_IMM_ARRAY_SIZE + 1) * 4 * sizeof(sF32) ));
   }
#endif // DX_EMU_PRIM
}
#endif // DX_EMU_USE_VBO

/* ----------------------------------------------------------------------------- tkopengl_imm_reset() */
void tkopengl_imm_reset(void) {
   current_vertex.x = 0.0f;
   current_vertex.y = 0.0f;
   current_vertex.z = 0.0f;
   current_vertex.w = 1.0f;

   current_normal.x = 0.0f;
   current_normal.y = 0.0f;
   current_normal.z = 1.0f;

   current_color.r = 1.0f;
   current_color.g = 1.0f;
   current_color.b = 1.0f;
   current_color.a = 1.0f;

   current_texcoord.s = 0.0f;
   current_texcoord.t = 0.0f;
   current_texcoord.r = 0.0f;
   current_texcoord.q = 1.0f;

   imm_mode = (GLenum) -1;

   loc_cbk_select_current();
}

/* ----------------------------------------------------------------------------- tkopengl_imm_exit() */
void tkopengl_imm_exit(void) {
   tkopengl_imm_normal_array   -= 3;
   tkopengl_imm_color_array    -= 4;
   tkopengl_imm_texcoord_array -= 4;

#ifdef DX_EMU_PRIM

#ifdef DX_EMU_PRIM_QUADSTRIP_LINE
   delete [] tkopengl_imm_quadstrip_to_lines_array;
   tkopengl_imm_quadstrip_to_lines_array = NULL;
#endif // DX_EMU_PRIM_QUADSTRIP_LINE

#ifdef DX_EMU_PRIM_TRISTRIP_LINE
   delete [] tkopengl_imm_trianglestrip_to_lines_array;
   tkopengl_imm_trianglestrip_to_lines_array = NULL;
#endif // DX_EMU_PRIM_TRISTRIP_LINE

#ifdef DX_EMU_PRIM_TRI_LINE
   delete [] tkopengl_imm_triangles_to_lines_array;
   tkopengl_imm_triangles_to_lines_array = NULL;
#endif // DX_EMU_PRIM_TRI_LINE

#ifdef DX_EMU_PRIM_QUAD_LINE
   delete [] tkopengl_imm_quads_to_lines_array;
   tkopengl_imm_quads_to_lines_array = NULL;
#endif // DX_EMU_PRIM_QUAD_LINE

   delete [] tkopengl_imm_quads_to_triangles_array;
   tkopengl_imm_quads_to_triangles_array = NULL;
#endif // DX_EMU_PRIM

   delete [] tkopengl_tmp_index_array;
   tkopengl_tmp_index_array = NULL;
   
   delete [] tkopengl_imm_vertex_array;
   tkopengl_imm_vertex_array = NULL;

   delete [] tkopengl_imm_normal_array;
   tkopengl_imm_normal_array = NULL;

   delete [] tkopengl_imm_color_array;
   tkopengl_imm_color_array = NULL;

   delete [] tkopengl_imm_texcoord_array;
   tkopengl_imm_texcoord_array = NULL;
}

/* ----------------------------------------------------------------------------- tkopengl_imm_begin() */
void tkopengl_imm_begin(GLenum _type) {

   Demuprintfv("[trc] tkopengl_imm_begin: type=0x%04x\n", _type);

   imm_mode = _type;

   next_vertex   = (imm_vertex_t*)   tkopengl_imm_vertex_array;
   next_normal   = (imm_normal_t*)   tkopengl_imm_normal_array;
   next_color    = (imm_color_t*)    tkopengl_imm_color_array;
   next_texcoord = (imm_texcoord_t*) tkopengl_imm_texcoord_array;

   next_normal[-1]   = current_normal;
   next_color[-1]    = current_color;
   next_texcoord[-1] = current_texcoord;

   current_flags = 0;

   current_idx = 0u;

   loc_cbk_select_def();
}

/* ----------------------------------------------------------------------------- loc_fill_implicit_values() */
YAC_INLINE void loc_fill_implicit_values(void) {

   if(0 == (current_flags & FLAG_NORMAL))
   {
      *next_normal = next_normal[-1];
   }

   if(0 == (current_flags & FLAG_COLOR))
   {
      *next_color = next_color[-1];
   }

   if(0 == (current_flags & FLAG_TEXCOORD))
   {
      *next_texcoord = next_texcoord[-1];
   }
}

/* ----------------------------------------------------------------------------- loc_next_element() */
YAC_INLINE void loc_next_element(void) {

   loc_fill_implicit_values();

   if(current_idx < (TKOPENGL_IMM_ARRAY_SIZE -1u))
   {
      current_idx++;

      next_vertex++;
      next_normal++;
      next_color++;
      next_texcoord++;
   }
   else
   {
      yac_host->printf("[---] tkopengl::immediate: max array size (%u) exceeded.\n", TKOPENGL_IMM_ARRAY_SIZE);
   }

   current_flags = 0;
}

/* ----------------------------------------------------------------------------- loc_imm_vertex_current() */
void loc_imm_vertex_current(sF32 _x, sF32 _y, sF32 _z, sF32 _w) {
   current_vertex.x = _x;
   current_vertex.y = _y;
   current_vertex.z = _z;
   current_vertex.w = _w;

#ifndef DX_EMU_FIXFXN
   ::glVertex4fv((sF32*)&current_vertex);
#endif
} 

/* ----------------------------------------------------------------------------- loc_imm_vertex_def() */
void loc_imm_vertex_def(sF32 _x, sF32 _y, sF32 _z, sF32 _w) {

   next_vertex->x = _x;
   next_vertex->y = _y;
   next_vertex->z = _z;
   next_vertex->w = _w;
   
   // Next array element
   loc_next_element();
} 

/* ----------------------------------------------------------------------------- loc_imm_normal_current() */
void loc_imm_normal_current(sF32 _x, sF32 _y, sF32 _z) {
   current_normal.x = _x;
   current_normal.y = _y;
   current_normal.z = _z;

#ifndef DX_EMU_FIXFXN
   ::glNormal3fv((sF32*)&current_normal);
#endif
}

/* ----------------------------------------------------------------------------- loc_imm_normal_def() */
void loc_imm_normal_def(sF32 _x, sF32 _y, sF32 _z) {

   next_normal->x = _x;
   next_normal->y = _y;
   next_normal->z = _z;
   
   current_flags |= FLAG_NORMAL;
}

/* ----------------------------------------------------------------------------- loc_imm_color_current() */
void loc_imm_color_current(sF32 _r, sF32 _g, sF32 _b, sF32 _a) {
   current_color.r = _r;
   current_color.g = _g;
   current_color.b = _b;
   current_color.a = _a;

#ifndef DX_EMU_FIXFXN
   ::glColor4fv((sF32*)&current_color);
#endif
}

/* ----------------------------------------------------------------------------- loc_imm_color_def() */
void loc_imm_color_def(sF32 _r, sF32 _g, sF32 _b, sF32 _a) {

   next_color->r = _r;
   next_color->g = _g;
   next_color->b = _b;
   next_color->a = _a;
   
   current_flags |= FLAG_COLOR;
}

/* ----------------------------------------------------------------------------- loc_imm_texcoord_current() */
void loc_imm_texcoord_current(sF32 _s, sF32 _t, sF32 _r, sF32 _q) {
   current_texcoord.s = _s;
   current_texcoord.t = _t;
   current_texcoord.r = _r;
   current_texcoord.q = _q;

#ifndef DX_EMU_FIXFXN
   ::glTexCoord4fv((sF32*)&current_texcoord);
#endif
}

/* ----------------------------------------------------------------------------- loc_imm_texcoord_def() */
void loc_imm_texcoord_def(sF32 _s, sF32 _t, sF32 _r, sF32 _q) {

   next_texcoord->s = _s;
   next_texcoord->t = _t;
   next_texcoord->r = _r;
   next_texcoord->q = _q;
   
   current_flags |= FLAG_TEXCOORD;
}

/* ----------------------------------------------------------------------------- tkopengl_imm_end() */
void tkopengl_imm_end(void) {

   Demuprintfv("[trc] tkopengl_imm_end: imm_mode=0x%04x\n", imm_mode);

   // (note) imm_mode can be -1 if imm_begin() was not called because of an unsupported primitive type
   if((GLenum)-1 != imm_mode)
   {
      current_normal   = next_normal[-1];
      current_color    = next_color[-1];
      current_texcoord = next_texcoord[-1];

      _glDrawArrays_priv(imm_mode, 0/*first*/, current_idx/*count*/, YAC_TRUE/*bAllowImm*/);
      Dtraceglerror("glDrawArrays::imm_end");

      imm_mode = (GLenum) -1;

   } // if -1 != imm_mode

   // Switch back to regular vertex/normal/color/texcoord function set
   loc_cbk_select_current();
}


#endif // DX_EMU_IMM
