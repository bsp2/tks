/// immediate.h
///
/// (c) 2013-2025 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#ifndef TKOPENGL_IMMEDIATE_H__
#define TKOPENGL_IMMEDIATE_H__

// Maximum array size
#ifndef TKOPENGL_IMM_ARRAY_SIZE
#define TKOPENGL_IMM_ARRAY_SIZE  (32768)
// #define TKOPENGL_IMM_ARRAY_SIZE  (1024*4096)
#endif // TKOPENGL_IMM_ARRAY_SIZE

typedef struct {
   sF32 x, y, z, w;
} imm_vertex_t;

typedef struct {
   sF32 x, y, z;
} imm_normal_t;

typedef struct {
   sF32 r, g, b, a;
} imm_color_t;

typedef struct {
   sF32 s, t, r, q;
} imm_texcoord_t;

extern imm_vertex_t   current_vertex;
extern imm_normal_t   current_normal;
extern imm_color_t    current_color;
extern imm_texcoord_t current_texcoord; // s,t,r,q

#ifdef DX_EMU_PRIM
extern sU16 *tkopengl_imm_quads_to_triangles_array;

#ifdef DX_EMU_PRIM_QUAD_LINE
extern sU16 *tkopengl_imm_quads_to_lines_array;         // for glPolygonMode(.., GL_LINE)
#endif

#ifdef DX_EMU_PRIM_TRI_LINE
extern sU16 *tkopengl_imm_triangles_to_lines_array;     // for glPolygonMode(.., GL_LINE)
#endif // DX_EMU_PRIM_TRI_LINE

#ifdef DX_EMU_PRIM_TRISTRIP_LINE
extern sU16 *tkopengl_imm_trianglestrip_to_lines_array; // for glPolygonMode(.., GL_LINE)
#endif // DX_EMU_PRIM_TRISTRIP_LINE

#ifdef DX_EMU_PRIM_QUADSTRIP_LINE
extern sU16 *tkopengl_imm_quadstrip_to_lines_array;     // for glPolygonMode(.., GL_LINE)
#endif // DX_EMU_PRIM_QUADSTRIP_LINE

#endif // DX_EMU_PRIM

extern sU16 *tkopengl_tmp_index_array; // temporary array to e.g. convert quad to triangle indices

extern sF32 *tkopengl_imm_vertex_array;
extern sF32 *tkopengl_imm_normal_array;
extern sF32 *tkopengl_imm_color_array;
extern sF32 *tkopengl_imm_texcoord_array;

#ifdef DX_EMU_USE_VBO
extern GLuint tkopengl_imm_index_buf_id;
extern GLuint tkopengl_imm_vertex_buf_id;
extern GLuint tkopengl_imm_normal_buf_id;
extern GLuint tkopengl_imm_color_buf_id;
extern GLuint tkopengl_imm_texcoord_buf_id;
#endif // DX_EMU_USE_VBO

extern void tkopengl_imm_init  (void);
#ifdef DX_EMU_USE_VBO
extern void tkopengl_imm_lazy_init_buffers (void);
#endif // DX_EMU_USE_VBO
extern void tkopengl_imm_reset (void);
extern void tkopengl_imm_exit  (void);

extern void tkopengl_imm_begin(GLenum _type);

typedef void (*tkopengl_imm_vertex_fxn_t)   (sF32 _x, sF32 _y, sF32 _z, sF32 _w);
typedef void (*tkopengl_imm_normal_fxn_t)   (sF32 _x, sF32 _y, sF32 _z);
typedef void (*tkopengl_imm_color_fxn_t)    (sF32 _r, sF32 _g, sF32 _b, sF32 _a);
typedef void (*tkopengl_imm_texcoord_fxn_t) (sF32 _s, sF32 _t, sF32 _r, sF32 _q);

extern tkopengl_imm_vertex_fxn_t   tkopengl_imm_vertex_cbk;
extern tkopengl_imm_normal_fxn_t   tkopengl_imm_normal_cbk;
extern tkopengl_imm_color_fxn_t    tkopengl_imm_color_cbk;
extern tkopengl_imm_texcoord_fxn_t tkopengl_imm_texcoord_cbk;

extern void tkopengl_imm_end(void);

#endif // TKOPENGL_IMMEDIATE_H__
