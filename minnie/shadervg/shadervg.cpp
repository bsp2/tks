// ----
// ---- file   : shadervg.cpp
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
// ----
// ----
// ----

#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#define YAC_BIGSTRING
#define YAC_PRINTF
#include <yac.h>

#define MINNIE_SKIP_TYPEDEFS  defined
#include "../minnie.h"

#include "../inc_opengl.h"

#include "shadervg.h"
#include "shadervg_internal.h"
#include "Shader.h"
#include "Shape.h"
#ifdef SHADERVG_TEXT
#include "text.h"
#endif // SHADERVG_TEXT

#ifdef SHADERVG_SCRIPT_API
#include "../tkminnie/ying_shadervg.h"
#endif // SHADERVG_SCRIPT_API

// Default framebuffer size
//  (note) see sdvg_SetFramebufferSize()
#define SHADERVG_DEFAULT_FB_W  640
#define SHADERVG_DEFAULT_FB_H  480

#define SHADERVG_MAX_FBOS             16u
#define SHADERVG_MAX_CUSTOM_SHADERS   32u
#define SHADERVG_VIEWPORT_STACK_SIZE   8u
#define SHADERVG_SCISSOR_STACK_SIZE    8u

// (note) must match minnie.h settings
#define SHADERVG_AA_RANGE_OFF  0.001f   // forced aa_range when b_aa = false
#define SHADERVG_LINE_STROKE_W_OFFSET           0.75f
#define SHADERVG_RECT_FILL_AA_SIZE_OFFSET       1.0f
#define SHADERVG_RECT_AA_SIZE_OFFSET            0.75f
#define SHADERVG_RECT_AA_STROKE_OFFSET          0.25f
#define SHADERVG_ELLIPSE_FILL_AA_SIZE_OFFSET    1.0f
#define SHADERVG_ELLIPSE_AA_SIZE_OFFSET         0.75f
#define SHADERVG_ELLIPSE_AA_STROKE_OFFSET       0.25f
#define SHADERVG_ROUNDRECT_FILL_AA_SIZE_OFFSET  1.0f
#define SHADERVG_ROUNDRECT_AA_SIZE_OFFSET       0.75f
#define SHADERVG_ROUNDRECT_AA_STROKE_OFFSET     0.25f
#define SHADERVG_POINTS_AA_RADIUS_OFFSET        0.75f

#define Dsdvg_pixel_scl(a) ((a) * sdvg_pixel_scl)

#include "TrianglesFillFlat32.h"
#include "TrianglesFillFlat32Linear.h"
#include "TrianglesFillFlat32Radial.h"
#include "TrianglesFillFlat32Conic.h"
#include "TrianglesFillFlat14_2.h"
#include "TrianglesFillGouraud32.h"
#include "TrianglesFillGouraud14_2.h"
#include "TrianglesFillFlatEdgeAA32.h"
#include "TrianglesFillFlatEdgeAA14_2.h"
#include "TrianglesFillGouraudEdgeAA32.h"
#include "TrianglesFillGouraudEdgeAA14_2.h"
#include "PolygonFillFlat32.h"
// #include "PolygonFillFlat32Linear.h"
// #include "PolygonFillFlat32Radial.h"
// #include "PolygonFillFlat32Conic.h"
#include "PolygonFillFlat14_2.h"
#include "PolygonFillGouraud32.h"
#include "PolygonFillGouraud14_2.h"
#include "RectFillAA.h"
#include "RectStrokeAA.h"
#include "RectFillStrokeAA.h"
#include "EllipseFillAA.h"
#include "EllipseStrokeAA.h"
#include "EllipseFillStrokeAA.h"
#include "RoundRectFillAA.h"
#include "RoundRectStrokeAA.h"
#include "RoundRectFillStrokeAA.h"
// #include "RoundRectFillStrokeSymAA.h"
#include "TrianglesTexUVFlat32.h"
#include "TrianglesTexUVGouraud32.h"
#include "TrianglesTexUVFlatDecal32.h"
#include "TrianglesTexUVGouraudDecal32.h"
#include "TrianglesTexUVFlat32Alpha.h"
#include "TrianglesTexUVGouraud32Alpha.h"
#include "TrianglesTexUVFlatDecal32Alpha.h"
#include "TrianglesTexUVGouraudDecal32Alpha.h"
#include "TrianglesTexUVFlat32AlphaSDF.h"
#include "LineStripFlat14_2.h"
#include "LineStripFlat32.h"
#include "LineStripFlatAA14_2.h"
#include "LineStripFlatAA32.h"
#include "LineStripFlatBevelAA14_2.h"
#include "LineStripFlatBevelAA32.h"
#include "LinesFlatAA14_2.h"
#include "LinesFlatAA32.h"
#include "PointsSquareAA32.h"
#include "PointsRoundAA32.h"

// Currently bound VBO (0=none)
static sUI current_vbo_id = 0u;

// GL VBO id or 0
// (todo) unmap VBO before rendering (keeping it mapped works on macOS, though)
// (todo) use glBufferStorage() / GL_MAP_PERSISTENT_BIT (GLES3.1 / GL4.x)
static sUI scratch_vbo_id = 0u;

// (note) see SetScratchBufferSize()
#define DEFAULT_SCRATCH_BUFFER_SZ  65536
// #define DEFAULT_SCRATCH_BUFFER_SZ  (2*3*2*4)  // debug: auto-flush after each quad
static sUI scratch_buffer_sz = 0u;

// mapped VBO (valid during onDraw())
//  (note) this is used as a ring buffer.
//  (note) when a wrap-around occurs, the buffer is temporarily unmapped and the GL pipeline is flushed before the buffer is remapped
//          (see ShaderVG_Shape::allocScratchBuffer())
static Dsdvg_buffer_ref_t scratch_buffer = NULL;

// mapped user-defined VBO. has precedence over scratch_buffer (sdvg_Begin*())
//  size = 0 when not mapped
static Dsdvg_buffer_ref_t user_vbo_buffer = NULL;
static int mapped_user_vbo_id;

static Dsdvg_buffer_ref_t attrib_write_buffer = NULL;

static sBool b_debug_write_vbo;

static TrianglesFillFlat32               triangles_fill_flat_32;
static TrianglesFillFlat32Linear         triangles_fill_flat_32_linear;
static TrianglesFillFlat32Radial         triangles_fill_flat_32_radial;
static TrianglesFillFlat32Conic          triangles_fill_flat_32_conic;
static TrianglesFillFlat14_2             triangles_fill_flat_14_2;
static TrianglesFillGouraud32            triangles_fill_gouraud_32;
static TrianglesFillGouraud14_2          triangles_fill_gouraud_14_2;
static TrianglesFillFlatEdgeAA32         triangles_fill_flat_edgeaa_32;
static TrianglesFillFlatEdgeAA14_2       triangles_fill_flat_edgeaa_14_2;
static TrianglesFillGouraudEdgeAA32      triangles_fill_gouraud_edgeaa_32;
static TrianglesFillGouraudEdgeAA14_2    triangles_fill_gouraud_edgeaa_14_2;
static PolygonFillFlat32                 polygon_fill_flat_32;
static PolygonFillFlat14_2               polygon_fill_flat_14_2;
static PolygonFillGouraud32              polygon_fill_gouraud_32;
static PolygonFillGouraud14_2            polygon_fill_gouraud_14_2;
static RectFillAA                        rect_fill_aa;
static RectStrokeAA                      rect_stroke_aa;
static RectFillStrokeAA                  rect_fill_stroke_aa;
static EllipseFillAA                     ellipse_fill_aa;
static EllipseStrokeAA                   ellipse_stroke_aa;
static EllipseFillStrokeAA               ellipse_fill_stroke_aa;
static RoundRectFillAA                   roundrect_fill_aa;
static RoundRectStrokeAA                 roundrect_stroke_aa;
static RoundRectFillStrokeAA             roundrect_fill_stroke_aa;
// static RoundRectFillStrokeSym         roundrect_fill_stroke_sym;
static TrianglesTexUVFlat32              triangles_tex_uv_flat_32;
static TrianglesTexUVGouraud32           triangles_tex_uv_gouraud_32;
static TrianglesTexUVFlatDecal32         triangles_tex_uv_flat_decal_32;
static TrianglesTexUVGouraudDecal32      triangles_tex_uv_gouraud_decal_32;
static TrianglesTexUVFlat32Alpha         triangles_tex_uv_flat_32_alpha;
static TrianglesTexUVGouraud32Alpha      triangles_tex_uv_gouraud_32_alpha;
static TrianglesTexUVFlatDecal32Alpha    triangles_tex_uv_flat_decal_32_alpha;
static TrianglesTexUVGouraudDecal32Alpha triangles_tex_uv_gouraud_decal_32_alpha;
static TrianglesTexUVFlat32AlphaSDF      triangles_tex_uv_flat_32_alpha_sdf;
static LineStripFlat14_2                 line_strip_flat_14_2;
static LineStripFlat32                   line_strip_flat_32;
static LineStripFlatAA14_2               line_strip_flat_aa_14_2;
static LineStripFlatAA32                 line_strip_flat_aa_32;
static LineStripFlatBevelAA14_2          line_strip_flat_bevel_aa_14_2;
static LineStripFlatBevelAA32            line_strip_flat_bevel_aa_32;
static LinesFlatAA14_2                   lines_flat_aa_14_2;
static LinesFlatAA32                     lines_flat_aa_32;
static PointsSquareAA32                  points_square_aa_32;
static PointsRoundAA32                   points_round_aa_32;

#define SHADERVG_NUM_SHAPES  43
static ShaderVG_Shape *all_shapes[SHADERVG_NUM_SHAPES] = {
   &triangles_fill_flat_32,
   &triangles_fill_flat_32_linear,
   &triangles_fill_flat_32_radial,
   &triangles_fill_flat_32_conic,
   &triangles_fill_flat_14_2,
   &triangles_fill_gouraud_32,
   &triangles_fill_gouraud_14_2,
   &triangles_fill_flat_edgeaa_32,
   &triangles_fill_flat_edgeaa_14_2,
   &triangles_fill_gouraud_edgeaa_32,
   &triangles_fill_gouraud_edgeaa_14_2,
   &polygon_fill_flat_32,
   &polygon_fill_flat_14_2,
   &polygon_fill_gouraud_32,
   &polygon_fill_gouraud_14_2,
   &rect_fill_aa,
   &rect_stroke_aa,
   &rect_fill_stroke_aa,
   &ellipse_fill_aa,
   &ellipse_stroke_aa,
   &ellipse_fill_stroke_aa,
   &roundrect_fill_aa,
   &roundrect_stroke_aa,
   &roundrect_fill_stroke_aa,
   // &roundrect_fill_stroke_sym_aa,
   &triangles_tex_uv_flat_32,
   &triangles_tex_uv_gouraud_32,
   &triangles_tex_uv_flat_decal_32,
   &triangles_tex_uv_gouraud_decal_32,
   &triangles_tex_uv_flat_32_alpha,
   &triangles_tex_uv_gouraud_32_alpha,
   &triangles_tex_uv_flat_decal_32_alpha,
   &triangles_tex_uv_gouraud_decal_32_alpha,
   &triangles_tex_uv_flat_32_alpha_sdf,
   &line_strip_flat_14_2,
   &line_strip_flat_32,
   &line_strip_flat_aa_14_2,
   &line_strip_flat_aa_32,
   &line_strip_flat_bevel_aa_14_2,
   &line_strip_flat_bevel_aa_32,
   &lines_flat_aa_14_2,
   &lines_flat_aa_32,
   &points_square_aa_32,
   &points_round_aa_32,
};

struct ShaderVG_FBO {
   sUI fbo_id;  // 0=unused
   sUI fbo_msaa_id;  // 0=no MSAA
   sUI tex_id;
   sUI sx;
   sUI sy;
};

static ShaderVG_FBO fbos[SHADERVG_MAX_FBOS];

class ShaderVG_CustomShape : public ShaderVG_Shape {
public:
   ShaderVG_CustomShape(void) {
      b_builtin = YAC_FALSE;
   }

   ~ShaderVG_CustomShape() {
   }

   sBool validateShapeShader(void) {
      return
         (-1 != shape_a_vertex)       &&
         (-1 != shape_u_transform)
         ;
   }

   sBool onOpen(void) {
      shape_shader.destroy();
      return YAC_TRUE;
   }
};

static ShaderVG_CustomShape custom_shapes[SHADERVG_MAX_CUSTOM_SHADERS];  // (note) shape shader 0 is 'none'
static ShaderVG_Shape *current_shape = NULL;
static sUI current_draw_num_vertices;   // see sdvg_BeginDraw()
static sUI current_draw_stride;
static sUI current_draw_num_bytes;
static sUI current_draw_start_offset;
static sUI current_draw_attrib_offset;  // incs with each AttribOffset*() call
static sUI current_draw_lines_vertex_index;   // incs with each Vertex2f() call in DRAW_MODE_LINES* (0, 6)
static sUI current_draw_vertex_index;         // incs with each Vertex2f() call

#define DRAW_MODE_LINE_STRIP           7000
#define DRAW_MODE_LINE_STRIP_AA        7001
#define DRAW_MODE_LINE_STRIP_BEVEL     7002
#define DRAW_MODE_LINE_STRIP_BEVEL_AA  7003
#define DRAW_MODE_LINES                7004
#define DRAW_MODE_LINES_AA             7005
#define DRAW_MODE_POINTS_SQUARE        7006
#define DRAW_MODE_POINTS_SQUARE_AA     7007
#define DRAW_MODE_POINTS_ROUND         7008
#define DRAW_MODE_POINTS_ROUND_AA      7009
static GLenum current_draw_mode;  // GL_TRIANGLES=0x0004, GL_TRIANGLE_STRIP=0x0005, GL_TRIANGLE_FAN=0x0006

#define SHADERVG_MAX_ATTRIB_ENABLES 16
static sSI current_draw_attrib_enables[SHADERVG_MAX_ATTRIB_ENABLES];
static sUI num_draw_attrib_enables;

#define PAINT_SOLID   0
#define PAINT_LINEAR  1
#define PAINT_RADIAL  2
#define PAINT_CONIC   3
static sSI  paint_mode;
static sF32 paint_start_x;
static sF32 paint_start_y;
static sF32 paint_end_x;
static sF32 paint_end_y;
static sF32 paint_angle;  // 0..1

// true=use GL core profile (GLSL 3.x, VAO)
sBool sdvg_b_glcore = YAC_FALSE;
static sUI vao_id = 0u;  // when sdvg_b_glcore=1

// framebuffer size
static sUI fb_w = 0u;
static sUI fb_h = 0u;

static sSI viewport_stack[SHADERVG_VIEWPORT_STACK_SIZE * 4];  // x,y,w,h
static sSI viewport_stacki;  // element offset (stack index * 4)
static sSI viewport_x;
static sSI viewport_y;  // 0=bottom (GL)
static sSI viewport_w;
static sSI viewport_h;

static sSI scissor_stack[SHADERVG_SCISSOR_STACK_SIZE * 4];  // x,y,w,h
static sSI scissor_stacki;  // element offset (stack index * 4)
static sSI scissor_x;
static sSI scissor_y;  // 0=bottom (GL)
static sSI scissor_w;
static sSI scissor_h;

// render state
static YAC_Object *mvp_matrix;  // Matrix4f  row major
static sBool  b_aa;
static sF32   aa_range;
static sF32   aa_exp;        // (todo) remove
static sF32   alpha_sdf_min;
static sF32   alpha_sdf_max;
static sF32   alpha_sdf_maxmin_scale;
static sF32   alpha_sdf_exp;
static sF32   stroke_w;      // px
static sF32   point_radius;  // px
sF32 sdvg_pixel_scl;         // vp/proj (aa_range, stroke_w)
static sF32   fill_r;
static sF32   fill_g;
static sF32   fill_b;
static sF32   fill_a;
static sF32   stroke_r;
static sF32   stroke_g;
static sF32   stroke_b;
static sF32   stroke_a;
static sF32   global_a;
static sF32   texture_decal_alpha;

// see SetGLSLVersion()
static YAC_String *s_glsl_version = NULL;
static YAC_String *s_glsl_attribute;
static YAC_String *s_glsl_varying_out;
static YAC_String *s_glsl_varying_in;
static YAC_String *s_glsl_fragcolor_def;
static YAC_String *s_glsl_fragcolor;
static YAC_String *s_glsl_texture2d;
static YAC_String *s_glsl_texture2dproj;
static YAC_String *s_glsl_texture3d;
static YAC_String *s_glsl_texturecube;
static YAC_String *s_glsl_tex_alpha;

// solid-fill shader, shared among all render classes
static ShaderVG_Shader fill_shader;
static sSI       fill_a_vertex;
static sSI       fill_u_transform;
static sSI       fill_u_color;

// ------------ fill vertex shader --------------
static const char *fill_vs_src =
   "uniform mat4 u_transform; \n"
   " \n"
   "ATTRIBUTE vec2 a_vertex; \n"
   " \n"
   "void main(void) { \n"
   "   gl_Position = u_transform * vec4(a_vertex,0,1); \n"
   "} \n"
   ;

// ------------ fill fragment shader ------------
static const char *fill_fs_src =
   "uniform vec4 u_color; \n"
   " \n"
   "void main(void) { \n"
   "  FRAGCOLOR = u_color; \n"
   "} \n"
   ;


static void loc_alloc_glsl_strings(void) {
   s_glsl_version       = YAC_New_String();
   s_glsl_attribute     = YAC_New_String();
   s_glsl_varying_out   = YAC_New_String();
   s_glsl_varying_in    = YAC_New_String();
   s_glsl_fragcolor_def = YAC_New_String();
   s_glsl_fragcolor     = YAC_New_String();
   s_glsl_texture2d     = YAC_New_String();
   s_glsl_texture2dproj = YAC_New_String();
   s_glsl_texture3d     = YAC_New_String();
   s_glsl_texturecube   = YAC_New_String();
   s_glsl_tex_alpha     = YAC_New_String();
}

// (note) must be called before sdvg_Init() (or this defaults to Desktop GL 2.x)
void YAC_CALL sdvg_SetGLSLVersion(sBool _bV3, sBool _bGLES, YAC_String *_sVersionStringOrNull) {

   if(NULL == s_glsl_version)
   {
      loc_alloc_glsl_strings();
   }

   if(NULL == _sVersionStringOrNull)
   {
      s_glsl_version->visit(
         _bV3
         ? _bGLES ? "#version 300 es\n" : "#version 410 core\n"
         : _bGLES ? "#version 100\n"    : "#version 120\n"
                           );
   }
   else
   {
      YAC_String t; t.copy("#version ");
      t.append(_sVersionStringOrNull);
      t.append("\n");
      s_glsl_version->yacCopy(&t);
   }

   s_glsl_attribute     ->visit(_bV3 ? "in"                      : "attribute");
   s_glsl_varying_out   ->visit(_bV3 ? "out"                     : "varying");
   s_glsl_varying_in    ->visit(_bV3 ? "in"                      : "varying");
   s_glsl_fragcolor_def ->visit(_bV3 ? "out vec4 o_FragColor;\n" : "");
   s_glsl_fragcolor     ->visit(_bV3 ? "o_FragColor"             : "gl_FragColor");
   s_glsl_texture2d     ->visit(_bV3 ? "texture"                 : "texture2D");
   s_glsl_texture2dproj ->visit(_bV3 ? "textureProj"             : "texture2DProj");
   s_glsl_texture3d     ->visit(_bV3 ? "texture"                 : "texture3D");
   s_glsl_texturecube   ->visit(_bV3 ? "texture"                 : "textureCube");
   s_glsl_tex_alpha     ->visit(_bV3 ? "r"                       : "a");
}

void YAC_CALL sdvg_SetScratchBufferSize(sUI _szBytes) {
   scratch_buffer_sz = _szBytes;
}

sBool YAC_CALL sdvg_Init(sBool _bGLCore) {
   sBool r = YAC_TRUE;

   Dprintf("[...] sdvg_Init(bGLCore=%d)\n", _bGLCore);

   sdvg_b_glcore = _bGLCore;

   // Import shared functions from tkopengl
   tkopengl_shared_resolve();
   if(NULL == tkopengl_shared)
   {
      Dsdvg_errorprintf("[---] sdvg_Init: tkopengl_shared_resolve() failed (plugin not loaded?)\n");
      return YAC_FALSE;
   }

   if(NULL == s_glsl_version)
   {
      sdvg_SetGLSLVersion(sdvg_b_glcore/*bV3*/,
                          YAC_FALSE/*bGLES*/,
                          NULL/*sVersionStringOrNull*/
                          );
   }

   current_vbo_id = 0u;
   scratch_vbo_id = ~0u;
   scratch_buffer = YAC_New_Buffer();
#ifdef SHADERVG_USE_SCRATCHBUFFERSUBDATA
   scratch_buffer->yacArrayAlloc(scratch_buffer_sz, 0,0,0);
#endif // SHADERVG_USE_SCRATCHBUFFERSUBDATA

   mapped_user_vbo_id = 0u;
   user_vbo_buffer = YAC_New_Buffer();

   attrib_write_buffer = scratch_buffer;

   b_debug_write_vbo = YAC_TRUE;

   mvp_matrix = yac_host->yacNew(NULL/*nsp*/, "Matrix4f");

   b_aa                = YAC_TRUE;
   aa_range            = 1.5f;
   aa_exp              = 1.0f;
   stroke_w            = 2.0f;
   fill_r              = 0.1f;
   fill_g              = 0.2f;
   fill_b              = 0.4f;
   fill_a              = 1.0f;
   stroke_r            = 1.0f;
   stroke_g            = 1.0f;
   stroke_b            = 1.0f;
   stroke_a            = 1.0f;
   global_a            = 1.0f;
   texture_decal_alpha = 1.0f;

   sdvg_SetAlphaSDFRange(0.0f, 0.0f);  // load default range
   sdvg_SetAlphaSDFExp(0.7f);

   ::memset((void*)fbos, 0, sizeof(fbos));

   for(sUI shaderIdx = 0u; shaderIdx < SHADERVG_MAX_CUSTOM_SHADERS; shaderIdx++)
   {
      // (todo) remove ? nothing to do here
   }
   current_shape = NULL;
   current_draw_mode = GL_NONE;

   fb_w = SHADERVG_DEFAULT_FB_W;
   fb_h = SHADERVG_DEFAULT_FB_H;

   ::memset(viewport_stack, 0, sizeof(viewport_stack));
   viewport_stacki = SHADERVG_VIEWPORT_STACK_SIZE * 4;
   viewport_x = 0;
   viewport_y = 0;
   viewport_w = 0;
   viewport_h = 0;

   ::memset(scissor_stack, 0, sizeof(scissor_stack));
   scissor_stacki = SHADERVG_SCISSOR_STACK_SIZE*4;
   scissor_x = 0;
   scissor_y = 0;
   scissor_w = fb_w;
   scissor_h = fb_h;

#ifdef SHADERVG_TEXT
   sdvg_int_reset_font();
#endif // SHADERVG_TEXT

   paint_mode = PAINT_SOLID;
   paint_start_x = 0.0f;
   paint_start_y = 0.0f;
   paint_end_x = 640.0f;
   paint_end_y = 480.0f;
   paint_angle = 0.0f;

   return r;
}

void YAC_CALL sdvg_Exit(void) {

   Dsdvg_tracecall("[trc] sdvg_Exit()\n");

   if(Dyac_host_yacGetDebugLevel() >= 2u)
   {
      Dsdvg_debugprintf("[dbg] sdvg_Exit: ENTER\n");
   }

   YAC_DELETE_SAFE(mvp_matrix);

   YAC_DELETE_SAFE(scratch_buffer);

   YAC_DELETE_SAFE(s_glsl_version);
   YAC_DELETE_SAFE(s_glsl_attribute);
   YAC_DELETE_SAFE(s_glsl_varying_out);
   YAC_DELETE_SAFE(s_glsl_varying_in);
   YAC_DELETE_SAFE(s_glsl_fragcolor_def);
   YAC_DELETE_SAFE(s_glsl_fragcolor);
   YAC_DELETE_SAFE(s_glsl_texture2d);
   YAC_DELETE_SAFE(s_glsl_texture2dproj);
   YAC_DELETE_SAFE(s_glsl_texture3d);
   YAC_DELETE_SAFE(s_glsl_texturecube);
   YAC_DELETE_SAFE(s_glsl_tex_alpha);

   if(Dyac_host_yacGetDebugLevel() >= 2u)
   {
      Dsdvg_debugprintf("[dbg] sdvg_Exit: LEAVE\n");
   }
}

sBool YAC_CALL sdvg_GetEnableDebug(void) {
   return ellipse_fill_aa.b_debug;
}

void YAC_CALL sdvg_SetEnableDebug(sBool _bEnable) {
   for(sUI i = 0u; i < SHADERVG_NUM_SHAPES; i++)
   {
      ShaderVG_Shape *shape = all_shapes[i];
      shape->b_debug = _bEnable;
   }
}

sBool YAC_CALL sdvg_GetEnableDrawInner(void) {
   // debug
   return ellipse_fill_aa.b_draw_inner;
}

void YAC_CALL sdvg_SetEnableDrawInner(sBool _bEnable) {
   // debug
   ellipse_fill_aa         .b_draw_inner = _bEnable;
   ellipse_fill_stroke_aa  .b_draw_inner = _bEnable;
   roundrect_fill_aa       .b_draw_inner = _bEnable;
   roundrect_fill_stroke_aa.b_draw_inner = _bEnable;
   rect_fill_aa            .b_draw_inner = _bEnable;
   rect_fill_stroke_aa     .b_draw_inner = _bEnable;
}

sBool YAC_CALL sdvg_GetEnableDrawBorder(void) {
   // debug
   return ellipse_fill_aa.b_draw_border;
}

void YAC_CALL sdvg_SetEnableDrawBorder(sBool _bEnable) {
   // debug
   ellipse_fill_aa         .b_draw_border = _bEnable;
   ellipse_fill_stroke_aa  .b_draw_border = _bEnable;
   roundrect_fill_aa       .b_draw_border = _bEnable;
   roundrect_fill_stroke_aa.b_draw_border = _bEnable;
   rect_fill_aa            .b_draw_border = _bEnable;
   rect_fill_stroke_aa     .b_draw_border = _bEnable;
}

void YAC_CALL sdvg_DebugSetEnableWriteVBO(sBool _bEnable) {
   b_debug_write_vbo = _bEnable;
}

sUI sdvg_CreateVBO(sUI _numBytes) {
   sUI id = Dsdvg_glcall(zglGenBuffer());
   if(id >= 0u)
   {
      sdvg_BindVBO(id);
      Dsdvg_glcall(zglBufferData(GL_ARRAY_BUFFER, _numBytes, NULL, GL_DYNAMIC_DRAW)); // GL_STATIC_DRAW
      // // // Dsdvg_glcall(glBindBuffer(GL_ARRAY_BUFFER, 0));
   }
   return id;
}

void YAC_CALL sdvg_UpdateVBO(sUI _vboId, sUI _offset, sUI _numBytes, YAC_Object *_data) {
   Dsdvg_tracecall("[trc] sdvg_UpdateVBO: vboId=%u offset=%u numBytes=%u data=%p\n", _vboId, _offset, _numBytes, (void*)_data);
   if(current_vbo_id != _vboId)
      sdvg_BindVBO(_vboId);
   Dsdvg_glcall(zglBufferSubData(GL_ARRAY_BUFFER, _offset, _numBytes, _data));
   // // // Dsdvg_glcall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void YAC_CALL sdvg_BindVBO(sUI _vboId) {
   Dsdvg_tracecall("[trc] sdvg_BindVBO: current_vbo_id=%u vboId=%u\n", current_vbo_id, _vboId);
   if(current_vbo_id != _vboId)
   {
      if(current_vbo_id == scratch_vbo_id && 0u != scratch_vbo_id && scratch_vbo_id != _vboId)
         UnbindScratchBuffer();

      Dsdvg_glcall(glBindBuffer(GL_ARRAY_BUFFER, _vboId));
      current_vbo_id = _vboId;
   }
}

sUI YAC_CALL sdvg_GetVBOSize(void) {
   int vboSize = Dsdvg_glcall(zglGetBufferParameter(GL_ARRAY_BUFFER, GL_BUFFER_SIZE));
   return sUI(vboSize);
}

sUI YAC_CALL sdvg_GetMappedVBOOffset(void) {
   if(0u != mapped_user_vbo_id)
   {
      return user_vbo_buffer->io_offset;
   }
   else if(current_vbo_id == scratch_vbo_id)
   {
      return scratch_buffer->io_offset;
   }
   return 0u;
}

static void loc_map_buffer(Dsdvg_buffer_ref_t _buf, sUI _size) {
   if(sdvg_b_glcore)
   {
      Dsdvg_tracecall("[trc] loc_map_buffer: call zglMapBufferRange: mapped_user_vbo_id=%u current_vbo_id=%u(scratch=%u) _size=%u\n", mapped_user_vbo_id, current_vbo_id, scratch_vbo_id, _size);
      Dsdvg_glcall(zglMapBufferRange(GL_ARRAY_BUFFER,
                                     0u/*offset*/, _size,
                                     GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT | GL_MAP_UNSYNCHRONIZED_BIT,  // GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT
                                     _buf
                                     )
                   );
      _buf->io_offset = 0u;
   }
   else
   {
      Dsdvg_glcall(zglMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY, _buf, _size));
   }
}

#ifdef SHADERVG_UNMAP_SCRATCHVBO_DURING_DRAW
void sdvg_unmap_scratch_before_draw(void) {
   if(current_vbo_id == scratch_vbo_id)
   {
      Dsdvg_glcall(glUnmapBuffer(GL_ARRAY_BUFFER));
   }
}

void sdvg_remap_scratch_after_draw(void) {
   if(current_vbo_id == scratch_vbo_id)
   {
      sUI oldOffset = scratch_buffer->io_offset;
      loc_map_buffer(scratch_buffer, scratch_buffer_sz);
      scratch_buffer->io_offset = oldOffset;
   }
}
#endif // SHADERVG_UNMAP_SCRATCHVBO_DURING_DRAW

void YAC_CALL sdvg_MapVBO(sUI _vboId) {
   if(mapped_user_vbo_id != _vboId)
   {
      sdvg_BindVBO(_vboId);
      // map GPU buffer to virtual address and reset io_offset
      sUI vboSize = (sUI)Dsdvg_glcall(zglGetBufferParameter(GL_ARRAY_BUFFER, GL_BUFFER_SIZE));
      Dsdvg_debugprintfv("[trc] sdvg_MapVBO: vboId=%u vboSize=%u\n", _vboId, vboSize);
      loc_map_buffer(user_vbo_buffer, vboSize);
      mapped_user_vbo_id = _vboId;
   }
}

void YAC_CALL sdvg_UnmapVBO(void) {
   Dsdvg_tracecall("[trc] sdvg_UnmapVBO: mapped_user_vbo_id=%u\n", mapped_user_vbo_id);
   if(0u != mapped_user_vbo_id)
   {
      Dsdvg_glcall(glUnmapBuffer(GL_ARRAY_BUFFER));
      mapped_user_vbo_id = 0u;
   }
}

void YAC_CALL sdvg_UnbindVBO(void) {
   Dsdvg_tracecall("[trc] sdvg_UnbindVBO  current_vbo_id=%u\n", current_vbo_id);
   if(current_vbo_id == scratch_vbo_id)
      Dsdvg_glcall(glUnmapBuffer(GL_ARRAY_BUFFER));
   Dsdvg_glcall(glBindBuffer(GL_ARRAY_BUFFER, 0u));
   current_vbo_id = 0u;
}

void YAC_CALL sdvg_DestroyVBO(sUI _vboId) {
   if(_vboId > 0u)
   {
      Dsdvg_glcall(zglDeleteBuffer(_vboId));
   }
}

sUI YAC_CALL sdvg_CreateTexture2D(sUI _texfmt, sUI _w, sUI _h, const void *_data, sUI _dataSz) {
   sUI texId = 0u;

   GLenum intFormat;
   GLenum pixFormat;
   GLenum type = GL_NONE;
   sUI bytesPerPixel;

   switch(_texfmt)
   {
      default:
         Dsdvg_errorprintf("[---] sdvg_CreateTexture2D: invalid texfmt=%u\n", _texfmt);
         break;

      case SDVG_TEXFMT_ALPHA8:
         bytesPerPixel = 1u;
#ifdef SHADERVG_GLES
         intFormat = GL_ALPHA;
         pixFormat = GL_ALPHA;
         type      = GL_UNSIGNED_BYTE;
#else
         if(sdvg_b_glcore)
         {
            intFormat = GL_R8;
            pixFormat = GL_RED;
            type      = GL_UNSIGNED_BYTE;
         }
#endif // SHADERVG_GLES
         break;

      case SDVG_TEXFMT_RGB565:
         bytesPerPixel = 2u;
#ifdef SHADERVG_GLES
         intFormat = GL_RGB;
         pixFormat = GL_RGB;
         type = GL_UNSIGNED_SHORT_5_6_5;
#else
         intFormat = GL_RGB5;
         pixFormat = GL_BGRA;
         type      = GL_UNSIGNED_SHORT; //GL_5_6_5;
#endif // SHADERVG_GLES
         break;

      case SDVG_TEXFMT_resvd_3:
         break;

      case SDVG_TEXFMT_BGRA8888:
         bytesPerPixel = 4u;
#ifdef SHADERVG_GLES
         intFormat = GL_BGRA8_EXT;
         pixFormat = GL_BGRA8_EXT;
         type      = GL_UNSIGNED_BYTE;
#else
         intFormat = GL_RGBA;
         pixFormat = GL_BGRA_EXT;
         type      = GL_UNSIGNED_BYTE;
#endif // SHADERVG_GLES
         break;

      case SDVG_TEXFMT_RGBA8888:
         bytesPerPixel = 4u;
         intFormat = GL_RGBA;
         pixFormat = GL_RGBA;
         type      = GL_UNSIGNED_BYTE;
         break;
   }

   if(GL_NONE != type)
   {
      const sUI reqSz = (bytesPerPixel * _w * _h);
      if(0u == _dataSz || _dataSz >= reqSz)
      {
         texId = Dsdvg_glcall(zglGenTexture());

         Dsdvg_glcall(glBindTexture(GL_TEXTURE_2D, texId));
         Dsdvg_glcall(glPixelStorei(GL_UNPACK_ALIGNMENT, bytesPerPixel));
         Dsdvg_glcall(glTexImage2D(GL_TEXTURE_2D,
                                   0/*level*/,
                                   intFormat,
                                   _w,
                                   _h,
                                   0, // border
                                   pixFormat,
                                   type,
                                   _data
                                   )
                      );
         Dsdvg_glcall(glBindTexture(GL_TEXTURE_2D, 0u));
      }
      else
      {
         Dsdvg_errorprintf("[---] sdvg_CreateTexture2D: insufficient dataSz (expect=%u have=%u) (texfmt=%u w=%u h=%u)\n", reqSz, _dataSz, _texfmt, _w, _h);
      }
   }
   else
   {
      Dsdvg_errorprintf("[---] sdvg_CreateTexture2D: invalid texfmt=%u (w=%u h=%u)\n", _texfmt, _w, _h);
   }
   return texId;
}

#ifdef SHADERVG_SCRIPT_API
sUI YAC_CALL _sdvg_CreateTexture2D(sUI _texfmt, sUI _w, sUI _h, YAC_Object *_data) {
   sUI texId = 0u;
   const void *data = NULL;
   sUI dataSz = 0u;
   if(YAC_VALID(_data))
   {
      data = (const void*)_data->yacArrayGetPointer();
      const sUI dataBytesPerPixel = _data->yacArrayGetElementByteSize();
      const sUI dataW = _data->yacArrayGetWidth();
      const sUI dataH = _data->yacArrayGetHeight();
      dataSz = dataBytesPerPixel * dataW * dataH;
   }
   texId = sdvg_CreateTexture2D(_texfmt, _w, _h, data, dataSz);
   return texId;
}
#endif // SHADERVG_SCRIPT_API

void YAC_CALL sdvg_UpdateTexture2D(sUI _texfmt, sUI _w, sUI _h, const void *_data, sUI _dataSz) {
   // GLenum intFormat;
   GLenum pixFormat;
   GLenum type = GL_NONE;
   sUI bytesPerPixel;
   switch(_texfmt)
   {
      default:
         Dsdvg_errorprintf("[---] sdvg_UpdateTexture2D: invalid texfmt=%u\n", _texfmt);
         break;

      case SDVG_TEXFMT_ALPHA8:
         bytesPerPixel = 1u;
#ifdef SHADERVG_GLES
         // intFormat = GL_ALPHA;
         pixFormat = GL_ALPHA;
         type      = GL_UNSIGNED_BYTE;
#else
         if(sdvg_b_glcore)
         {
            // intFormat = GL_R8;
            pixFormat = GL_RED;
            type      = GL_UNSIGNED_BYTE;
         }
#endif // SHADERVG_GLES
         break;

      case SDVG_TEXFMT_RGB565:
         bytesPerPixel = 2u;
#ifdef SHADERVG_GLES
         // intFormat = GL_RGB;
         pixFormat = GL_RGB;
         type = GL_UNSIGNED_SHORT_5_6_5;
#else
         // intFormat = GL_RGB5;
         pixFormat = GL_BGRA;
         type      = GL_UNSIGNED_SHORT; //GL_5_6_5;
#endif // SHADERVG_GLES
         break;

      case SDVG_TEXFMT_resvd_3:
         break;

      case SDVG_TEXFMT_BGRA8888:
         bytesPerPixel = 4u;
#ifdef SHADERVG_GLES
         // intFormat = GL_BGRA8_EXT;
         pixFormat = GL_BGRA8_EXT;
         type      = GL_UNSIGNED_BYTE;
#else
         // intFormat = GL_RGBA;
         pixFormat = GL_BGRA_EXT;
         type      = GL_UNSIGNED_BYTE;
#endif // SHADERVG_GLES
         break;

      case SDVG_TEXFMT_RGBA8888:
         bytesPerPixel = 4u;
         // intFormat = GL_RGBA;
         pixFormat = GL_RGBA;
         type      = GL_UNSIGNED_BYTE;
         break;
   }

   if(GL_NONE != type)
   {
      const sUI reqSz = (bytesPerPixel * _w * _h);
      if(_dataSz >= reqSz)
      {
         Dsdvg_glcall(glPixelStorei(GL_UNPACK_ALIGNMENT, bytesPerPixel));
         Dsdvg_glcall(glTexSubImage2D(GL_TEXTURE_2D,
                                      0/*level*/,
                                      0/*xoffset*/,
                                      0/*yoffset*/,
                                      _w,
                                      _h,
                                      pixFormat,
                                      type,
                                      _data
                                      )
                      );
      }
      else
      {
         Dsdvg_errorprintf("[---] sdvg_UpdateTexture2D: insufficient dataSz (expect=%u have=%u) (texfmt=%u w=%u h=%u)\n", reqSz, _dataSz, _texfmt, _w, _h);
      }
   }
   else
   {
      Dsdvg_errorprintf("[---] sdvg_UpdateTexture2D: invalid texfmt=%u (w=%u h=%u)\n", _texfmt, _w, _h);
   }
}

#ifdef SHADERVG_SCRIPT_API
void YAC_CALL _sdvg_UpdateTexture2D(sUI _texfmt, sUI _w, sUI _h, YAC_Object *_data) {
   if(YAC_VALID(_data))
   {
      const void *data = (const void*)_data->yacArrayGetPointer();
      const sUI dataBytesPerPixel = _data->yacArrayGetElementByteSize();
      const sUI dataW = _data->yacArrayGetWidth();
      const sUI dataH = _data->yacArrayGetHeight();
      sUI dataSz = dataBytesPerPixel * dataW * dataH;
      sdvg_UpdateTexture2D(_texfmt, _w, _h, data, dataSz);
   }
}
#endif // SHADERVG_SCRIPT_API

void YAC_CALL sdvg_BindMultiTexture2D(sUI _unitIdx, sUI _texId, sBool _bRepeat, sBool _bFilter) {

   Dsdvg_glcall(glActiveTexture(GL_TEXTURE0 + GLenum(_unitIdx)));

   Dsdvg_glcall(glBindTexture(GL_TEXTURE_2D, _texId));

   sSI wrap = _bRepeat ? GL_REPEAT : GL_CLAMP_TO_EDGE;
   Dsdvg_glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap));
   Dsdvg_glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap));

   sSI filter = _bFilter ? GL_LINEAR : GL_NEAREST;
   Dsdvg_glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter));
   Dsdvg_glcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter));
}

void YAC_CALL sdvg_UnbindMultiTexture2D(sUI _unitIdx) {
   Dsdvg_glcall(glActiveTexture(GL_TEXTURE0 + GLenum(_unitIdx)));
   Dsdvg_glcall(glBindTexture(GL_TEXTURE_2D, 0));
}

void YAC_CALL sdvg_BindTexture2D(sUI _texId, sBool _bRepeat, sBool _bFilter) {
   sdvg_BindMultiTexture2D(0u/*unitIdx*/, _texId, _bRepeat, _bFilter);
}

void YAC_CALL sdvg_UnbindTexture2D(void) {
   sdvg_UnbindMultiTexture2D(0u/*unitIdx*/);
}

void YAC_CALL sdvg_DestroyTexture2D(sUI _texId) {
   Dsdvg_glcall(zglDeleteTexture(_texId));
}

void YAC_CALL sdvg_BufferAddRectTexUVFlat32(YAC_Buffer *_b, sF32 _x, sF32 _y, sF32 _w, sF32 _h, sF32 _ul, sF32 _vt, sF32 _ur, sF32 _vb) {
   Dstream_write_f32(_b, _x);       Dstream_write_f32(_b, _y);
   Dstream_write_f32(_b, _ul);      Dstream_write_f32(_b, _vt);

   Dstream_write_f32(_b, _x + _w);  Dstream_write_f32(_b, _y);
   Dstream_write_f32(_b, _ur);      Dstream_write_f32(_b, _vt);

   Dstream_write_f32(_b, _x + _w);  Dstream_write_f32(_b, _y + _h);
   Dstream_write_f32(_b, _ur);      Dstream_write_f32(_b, _vb);

   Dstream_write_f32(_b, _x);       Dstream_write_f32(_b, _y);
   Dstream_write_f32(_b, _ul);      Dstream_write_f32(_b, _vt);

   Dstream_write_f32(_b, _x + _w);  Dstream_write_f32(_b, _y + _h);
   Dstream_write_f32(_b, _ur);      Dstream_write_f32(_b, _vb);

   Dstream_write_f32(_b, _x);       Dstream_write_f32(_b, _y + _h);
   Dstream_write_f32(_b, _ul);      Dstream_write_f32(_b, _vb);
}

void YAC_CALL sdvg_WriteC32AsRGBA8(YAC_Buffer *_b, sUI _c32) {
   Dstream_write_i8(_b, (_c32 >> 16) & 255u );  // r
   Dstream_write_i8(_b, (_c32 >>  8) & 255u );  // g
   Dstream_write_i8(_b, (_c32      ) & 255u );  // b
   Dstream_write_i8(_b, (_c32 >> 24) & 255u );  // a
}

void YAC_CALL sdvg_BufferAddRectTexUVGouraud32(YAC_Buffer *_b,
                                               sF32 _x, sF32 _y,
                                               sF32 _w, sF32 _h,
                                               sF32 _ul, sF32 _vt, sF32 _ur, sF32 _vb,
                                               sUI _c32LT, sUI _c32RT, sUI _c32LB, sUI _c32RB
                                               ) {
   Dstream_write_2f(_b, _x,  _y);
   Dstream_write_2f(_b, _ul, _vt);
   sdvg_WriteC32AsRGBA8(_b, _c32LT);

   Dstream_write_2f(_b, _x + _w, _y);
   Dstream_write_2f(_b, _ur, _vt);
   sdvg_WriteC32AsRGBA8(_b, _c32RT);

   Dstream_write_2f(_b, _x + _w, _y + _h);
   Dstream_write_2f(_b, _ur, _vb);
   sdvg_WriteC32AsRGBA8(_b, _c32RB);

   Dstream_write_2f(_b, _x, _y);
   Dstream_write_2f(_b, _ul, _vt);
   sdvg_WriteC32AsRGBA8(_b, _c32LT);

   Dstream_write_2f(_b, _x + _w, _y + _h);
   Dstream_write_2f(_b, _ur, _vb);
   sdvg_WriteC32AsRGBA8(_b, _c32RB);

   Dstream_write_2f(_b, _x, _y + _h);
   Dstream_write_2f(_b, _ul, _vb);
   sdvg_WriteC32AsRGBA8(_b, _c32LB);
}

void YAC_CALL sdvg_BufferAddLinePointFlat14_2(YAC_Buffer *_b, sF32 _x, sF32 _y) {
#ifndef TKMINNIE_DUPLICATE_POINT_VERTICES
   Dstream_write_s16(_b, sS16(_x * 4.0f));
   Dstream_write_s16(_b, sS16(_y * 4.0f));
#else
   sU16 vtxIdx = 0u;
   for(sUI i = 0u; i < 6u; i++)
   {
      Dstream_write_s16(_b, sS16(_x * 4.0f));
      Dstream_write_s16(_b, sS16(_y * 4.0f));
      Dstream_write_i16(_b, vtxIdx++);
   }
#endif // TKMINNIE_DUPLICATE_POINT_VERTICES
}

void YAC_CALL sdvg_BufferAddLinePointFlat32(YAC_Buffer *_b, sF32 _x, sF32 _y) {
#ifndef TKMINNIE_DUPLICATE_POINT_VERTICES
   Dstream_write_f32(_b, _x);
   Dstream_write_f32(_b, _y);
#else
   sU16 vtxIdx = 0u;
   for(sUI i = 0u; i < 6u; i++)
   {
      Dstream_write_f32(_b, _x);
      Dstream_write_f32(_b, _y);
      Dstream_write_i16(_b, vtxIdx++);
   }
#endif // TKMINNIE_DUPLICATE_POINT_VERTICES
}

void YAC_CALL sdvg_BufferAddLinePointFlatBevel14_2(YAC_Buffer *_b, sF32 _x, sF32 _y) {
#ifndef TKMINNIE_DUPLICATE_POINT_VERTICES
   Dstream_write_s16(_b, sS16(_x * 4.0f));
   Dstream_write_s16(_b, sS16(_y * 4.0f));
#else
   sU16 vtxIdx = 0u;
   for(sUI i = 0u; i < 9u; i++)
   {
      Dstream_write_s16(_b, sS16(_x * 4.0f));
      Dstream_write_s16(_b, sS16(_y * 4.0f));
      Dstream_write_i16(_b, vtxIdx++);
   }
#endif // TKMINNIE_DUPLICATE_POINT_VERTICES
}

void YAC_CALL sdvg_BufferAddLinePointFlatBevel32(YAC_Buffer *_b, sF32 _x, sF32 _y) {
#ifndef TKMINNIE_DUPLICATE_POINT_VERTICES
   Dstream_write_f32(_b, _x);
   Dstream_write_f32(_b, _y);
#else
   sU16 vtxIdx = 0u;
   for(sUI i = 0u; i < 9u; i++)
   {
      Dstream_write_f32(_b, _x);
      Dstream_write_f32(_b, _y);
      Dstream_write_i16(_b, vtxIdx++);
   }
#endif // TKMINNIE_DUPLICATE_POINT_VERTICES
}

void YAC_CALL sdvg_BufferAddLinesPointsFlat14_2(YAC_Buffer *_b, sF32 _x1, sF32 _y1, sF32 _x2, sF32 _y2) {
#ifndef TKMINNIE_DUPLICATE_POINT_VERTICES
   Dstream_write_s16(_b, sS16(_x1 * 4.0f));
   Dstream_write_s16(_b, sS16(_y1 * 4.0f));

   Dstream_write_s16(_b, sS16(_x2 * 4.0f));
   Dstream_write_s16(_b, sS16(_y2 * 4.0f));
#else
   sU16 vtxIdx = 0u;
   for(sUI i = 0u; i < 6u; i++)
   {
      Dstream_write_s16(_b, sS16(_x1 * 4.0f));
      Dstream_write_s16(_b, sS16(_y1 * 4.0f));
      Dstream_write_i16(_b, vtxIdx++);
   }
   for(sUI i = 0u; i < 6u; i++)
   {
      Dstream_write_s16(_b, sS16(_x2 * 4.0f));
      Dstream_write_s16(_b, sS16(_y2 * 4.0f));
      Dstream_write_i16(_b, vtxIdx++);
   }
#endif // TKMINNIE_DUPLICATE_POINT_VERTICES
}

void YAC_CALL sdvg_BufferAddLinesPointsFlat32(YAC_Buffer *_b, sF32 _x1, sF32 _y1, sF32 _x2, sF32 _y2) {
#ifndef TKMINNIE_DUPLICATE_POINT_VERTICES
   Dstream_write_f32(_b, _x1);
   Dstream_write_f32(_b, _y1);

   Dstream_write_f32(_b, _x2);
   Dstream_write_f32(_b, _y2);
#else
   sU16 vtxIdx = 0u;
   for(sUI i = 0u; i < 6u; i++)
   {
      Dstream_write_f32(_b, _x1);
      Dstream_write_f32(_b, _y1);
      Dstream_write_i16(_b, vtxIdx++);
   }
   for(sUI i = 0u; i < 6u; i++)
   {
      Dstream_write_f32(_b, _x2);
      Dstream_write_f32(_b, _y2);
      Dstream_write_i16(_b, vtxIdx++);
   }
#endif // TKMINNIE_DUPLICATE_POINT_VERTICES
}

#ifdef SHADERVG_USE_DEFAULT_LINE_14_2
static void loc_BufferAddLinesPointFlat14_2(YAC_Buffer *_b, sF32 _x, sF32 _y) {
   // called by sdvg_Vertex2f() in DRAW_MODE_LINES*
#ifndef TKMINNIE_DUPLICATE_POINT_VERTICES
   Dstream_write_s16(_b, sS16(_x * 4.0f));
   Dstream_write_s16(_b, sS16(_y * 4.0f));
   current_draw_lines_vertex_index += 1u;
   if(current_draw_lines_vertex_index >= 2u)
      current_draw_lines_vertex_index = 0u;
#else
   sU16 vtxIdx = current_draw_lines_vertex_index;
   for(sUI i = 0u; i < 6u; i++)
   {
      Dstream_write_s16(_b, sS16(_x * 4.0f));
      Dstream_write_s16(_b, sS16(_y * 4.0f));
      Dstream_write_i16(_b, vtxIdx++);
   }
   current_draw_lines_vertex_index += 6u;
   if(current_draw_lines_vertex_index >= 12u)
      current_draw_lines_vertex_index = 0u;
#endif // TKMINNIE_DUPLICATE_POINT_VERTICES
}
#else
static void loc_BufferAddLinesPointFlat32(YAC_Buffer *_b, sF32 _x, sF32 _y) {
   // called by sdvg_Vertex2f() in DRAW_MODE_LINES*
#ifndef TKMINNIE_DUPLICATE_POINT_VERTICES
   Dstream_write_f32(_b, _x);
   Dstream_write_f32(_b, _y);
   current_draw_lines_vertex_index += 1u;
   if(current_draw_lines_vertex_index >= 2u)
      current_draw_lines_vertex_index = 0u;
#else
   sU16 vtxIdx = current_draw_lines_vertex_index;
   for(sUI i = 0u; i < 6u; i++)
   {
      Dstream_write_f32(_b, _x);
      Dstream_write_f32(_b, _y);
      Dstream_write_i16(_b, vtxIdx++);
   }
   current_draw_lines_vertex_index += 6u;
   if(current_draw_lines_vertex_index >= 12u)
      current_draw_lines_vertex_index = 0u;
#endif // TKMINNIE_DUPLICATE_POINT_VERTICES
}
#endif // SHADERVG_USE_DEFAULT_LINE_14_2

void YAC_CALL sdvg_DrawTrianglesFillFlatVBO32(sUI _vboId, sUI _byteOffset, sUI _numVerts) {
   //
   // VBO vertex format (8 bytes per vertex):
   //   +0 f32 x
   //   +4 f32 y
   //
   triangles_fill_flat_32.drawTrianglesFillFlatVBO32(_vboId,
                                                     _byteOffset,
                                                     _numVerts,
                                                     mvp_matrix,
                                                     fill_r, fill_g, fill_b, fill_a * global_a
                                                     );
}

void YAC_CALL sdvg_DrawTrianglesFillFlatVBO14_2(sUI _vboId, sUI _byteOffset, sUI _numVerts) {
   //
   // VBO vertex format (4 bytes per vertex):
   //   s14.2 x
   //   s14.2 y
   //
   triangles_fill_flat_14_2.drawTrianglesFillFlatVBO14_2(_vboId,
                                                         _byteOffset,
                                                         _numVerts,
                                                         mvp_matrix,
                                                         fill_r, fill_g, fill_b, fill_a * global_a
                                                         );
}

void YAC_CALL sdvg_DrawTrianglesFillGouraudVBO32(sUI _vboId, sUI _byteOffset, sUI _numVerts) {
   // VBO vertex format (12 bytes per vertex):
   //   f32 x
   //   f32 y
   //   u8  r
   //   u8  g
   //   u8  b
   //   u8  a
   triangles_fill_gouraud_32.drawTrianglesFillGouraudVBO32(_vboId,
                                                           _byteOffset,
                                                           _numVerts,
                                                           mvp_matrix,
                                                           global_a
                                                           );
}

void YAC_CALL sdvg_DrawTrianglesFillGouraudVBO14_2(sUI _vboId, sUI _byteOffset, sUI _numVerts) {
   //
   // VBO vertex format (8 bytes per vertex):
   //   +0 s14.2 x
   //   +2 s14.2 y
   //   +4 u8    r
   //   +5 u8    g
   //   +6 u8    b
   //   +7 u8    a
   //
   triangles_fill_gouraud_14_2.drawTrianglesFillGouraudVBO14_2(_vboId,
                                                               _byteOffset,
                                                               _numVerts,
                                                               mvp_matrix,
                                                               global_a
                                                               );
}

void YAC_CALL sdvg_DrawTrianglesFillFlatEdgeAAVBO32(sUI _vboId, sUI _byteOffset, sUI _numVerts) {
   //
   // VBO vertex format (26 bytes per vertex):
   //     0 f32 x
   //     4 f32 y
   //     8 u8  edge_bc
   //     9 u8  edge_ca
   //    10 u8  edge_ab
   //    11 u8  __pad1
   //    12 f32 scale_s
   //    16 f32 scale_t
   //    20 f32 scale_u
   //    24 u8  barycentric_s
   //    25 u8  barycentric_t
   //
   triangles_fill_flat_edgeaa_32.drawTrianglesFillFlatEdgeAAVBO32(_vboId,
                                                                  _byteOffset,
                                                                  _numVerts,
                                                                  mvp_matrix,
                                                                  fill_r, fill_g, fill_b, fill_a * global_a,
                                                                  b_aa
                                                                  );
}

void YAC_CALL sdvg_DrawTrianglesFillFlatEdgeAAVBO14_2(sUI _vboId, sUI _byteOffset, sUI _numVerts) {
   //
   // VBO vertex format (22 bytes per vertex):
   //     0 s14.2 x
   //     2 s14.2 y
   //     4 u8    edge_bc
   //     5 u8    edge_ca
   //     6 u8    edge_ab
   //     7 u8    __pad1
   //     8 f32   scale_s
   //    12 f32   scale_t
   //    16 f32   scale_u
   //    20 u8    barycentric_s
   //    21 u8    barycentric_t
   //
   triangles_fill_flat_edgeaa_14_2.drawTrianglesFillFlatEdgeAAVBO14_2(_vboId,
                                                                      _byteOffset,
                                                                      _numVerts,
                                                                      mvp_matrix,
                                                                      fill_r, fill_g, fill_b, fill_a * global_a,
                                                                      b_aa
                                                                      );
}

void YAC_CALL sdvg_DrawTrianglesFillGouraudEdgeAAVBO32(sUI _vboId, sUI _byteOffset, sUI _numVerts) {
   //
   // VBO vertex format (30 bytes per vertex):
   //     0 f32 x
   //     4 f32 y
   //     8 u8  r
   //     9 u8  g
   //    10 u8  b
   //    11 u8  a
   //    12 u8  edge_bc
   //    13 u8  edge_ca
   //    14 u8  edge_ab
   //    15 u8  __pad1
   //    16 f32 scale_s
   //    20 f32 scale_t
   //    24 f32 scale_u
   //    28 u8  barycentric_s
   //    29 u8  barycentric_t
   //
   triangles_fill_gouraud_edgeaa_32.drawTrianglesFillGouraudEdgeAAVBO32(_vboId,
                                                                        _byteOffset,
                                                                        _numVerts,
                                                                        mvp_matrix,
                                                                        b_aa,
                                                                        global_a
                                                                        );
}

void YAC_CALL sdvg_DrawTrianglesFillGouraudEdgeAAVBO14_2(sUI _vboId, sUI _byteOffset, sUI _numVerts) {
   //
   // VBO vertex format (26 bytes per vertex):
   //     0 f32 x
   //     2 f32 y
   //     4 u8  r
   //     5 u8  g
   //     6 u8  b
   //     7 u8  a
   //     8 u8  edge_bc
   //     9 u8  edge_ca
   //    10 u8  edge_ab
   //    11 u8  __pad1
   //    12 f32 scale_s
   //    16 f32 scale_t
   //    20 f32 scale_u
   //    24 u8  barycentric_s
   //    25 u8  barycentric_t
   //
   triangles_fill_gouraud_edgeaa_14_2.drawTrianglesFillGouraudEdgeAAVBO14_2(_vboId,
                                                                            _byteOffset,
                                                                            _numVerts,
                                                                            mvp_matrix,
                                                                            b_aa,
                                                                            global_a
                                                                            );
}

void YAC_CALL sdvg_DrawPolygonFillFlatVBO32(sUI _vboId, sUI _byteOffset, sUI _numVerts) {
   //
   // VBO vertex format (8 bytes per vertex):
   //   +0 f32 x
   //   +4 f32 y
   //
   // (note) requires stencil buffer
   //
   polygon_fill_flat_32.drawPolygonFillFlatVBO32(_vboId,
                                                 _byteOffset,
                                                 _numVerts,
                                                 mvp_matrix,
                                                 fill_r, fill_g, fill_b, fill_a * global_a
                                                 );
}

void YAC_CALL sdvg_DrawPolygonFillFlatVBO14_2(sUI _vboId, sUI _byteOffset, sUI _numVerts) {
   //
   // VBO vertex format (4 bytes per vertex):
   //   s14.2 x
   //   s14.2 y
   //
   // (note) requires stencil buffer
   //
   polygon_fill_flat_14_2.drawPolygonFillFlatVBO14_2(_vboId,
                                                     _byteOffset,
                                                     _numVerts,
                                                     mvp_matrix,
                                                     fill_r, fill_g, fill_b, fill_a * global_a
                                                     );
}

void YAC_CALL sdvg_DrawPolygonFillGouraudVBO32(sUI _vboId, sUI _byteOffset, sUI _numVerts) {
   //
   // VBO vertex format (12 bytes per vertex):
   //   f32 x
   //   f32 y
   //   u8  r
   //   u8  g
   //   u8  b
   //   u8  a
   //
   // (note) requires stencil buffer
   //
   polygon_fill_gouraud_32.drawPolygonFillGouraudVBO32(_vboId,
                                                       _byteOffset,
                                                       _numVerts,
                                                       mvp_matrix,
                                                       global_a
                                                       );
}

void YAC_CALL sdvg_DrawPolygonFillGouraudVBO14_2(sUI _vboId, sUI _byteOffset, sUI _numVerts) {
   //
   // VBO vertex format (8 bytes per vertex):
   //   s14.2 x
   //   s14.2 y
   //   u8    r
   //   u8    g
   //   u8    b
   //   u8    a
   //
   // (note) requires stencil buffer
   //
   polygon_fill_gouraud_14_2.drawPolygonFillGouraudVBO14_2(_vboId,
                                                           _byteOffset,
                                                           _numVerts,
                                                           mvp_matrix,
                                                           global_a
                                                           );
}

void YAC_CALL sdvg_SetupRectFillAAVBO32(YAC_Buffer *_vb, YAC_Buffer *_dl,
                                        sF32 _centerX, sF32 _centerY,
                                        sF32 _sizeX,   sF32 _sizeY
                                        ) {
   // (note) 'size' is actually the radius (area is center-size..center+size)
   const sF32 aaOff = b_aa ? Dsdvg_pixel_scl(SHADERVG_RECT_FILL_AA_SIZE_OFFSET) : 0.0f;
   rect_fill_aa.setupRectFillAAVBO32(_vb, _dl,
                                     _centerX, _centerY,
                                     _sizeX + aaOff, _sizeY + aaOff,
                                     Dsdvg_pixel_scl(aa_range)
                                     );
}

void YAC_CALL sdvg_DrawRectFillAAVBO32(sUI _vboId,
                                       sUI _byteOffsetInner,
                                       sUI _numVertsInner,
                                       sUI _byteOffsetBorder,
                                       sUI _numVertsBorder,
                                       sUI _glPrimTypeBorder,
                                       sF32 _centerX, sF32 _centerY,
                                       sF32 _sizeX,   sF32 _sizeY
                                       ) {
   const sF32 aaOff = b_aa ? Dsdvg_pixel_scl(SHADERVG_RECT_FILL_AA_SIZE_OFFSET) : 0.0f;
   rect_fill_aa.drawRectFillAAVBO32(_vboId,
                                    _byteOffsetInner,
                                    _numVertsInner,
                                    _byteOffsetBorder,
                                    _numVertsBorder,
                                    _glPrimTypeBorder,
                                    mvp_matrix,
                                    _centerX, _centerY,
                                    _sizeX + aaOff, _sizeY + aaOff,
                                    fill_r, fill_g, fill_b, fill_a * global_a,
                                    b_aa ? Dsdvg_pixel_scl(aa_range) : SHADERVG_AA_RANGE_OFF,
                                    aa_exp
                                    );
}

void YAC_CALL sdvg_DrawRectFillAA(sF32 _centerX, sF32 _centerY,
                                  sF32 _sizeX,   sF32 _sizeY
                                  ) {
   const sF32 aaOff = b_aa ? Dsdvg_pixel_scl(SHADERVG_RECT_FILL_AA_SIZE_OFFSET) : 0.0f;
   rect_fill_aa.drawRectFillAA(scratch_buffer,
                               mvp_matrix,
                               _centerX, _centerY,
                               _sizeX + aaOff, _sizeY + aaOff,
                               fill_r, fill_g, fill_b, fill_a * global_a,
                               b_aa ? Dsdvg_pixel_scl(aa_range) : SHADERVG_AA_RANGE_OFF,
                               aa_exp
                               );
}

void YAC_CALL sdvg_SetupRectFillStrokeAAVBO32(YAC_Buffer *_vb, YAC_Buffer *_dl,
                                              sF32 _centerX, sF32 _centerY,
                                              sF32 _sizeX,   sF32 _sizeY,
                                              sF32 _strokeW
                                              ) {
   // (note) 'size' is actually the radius (area is center-size..center+size)
   const sF32 aaOffSize   = b_aa ? Dsdvg_pixel_scl(SHADERVG_RECT_AA_SIZE_OFFSET)   : 0.0f;
   const sF32 aaOffStroke = b_aa ? Dsdvg_pixel_scl(SHADERVG_RECT_AA_STROKE_OFFSET) : 0.0f;
   rect_fill_stroke_aa.setupRectFillStrokeAAVBO32(_vb, _dl,
                                                  _centerX, _centerY,
                                                  _sizeX + aaOffSize, _sizeY + aaOffSize,
                                                  Dsdvg_pixel_scl(_strokeW) + aaOffStroke,
                                                  Dsdvg_pixel_scl(aa_range)
                                                  );
}

void YAC_CALL sdvg_DrawRectFillStrokeAAVBO32(sUI _vboId,
                                             sUI _byteOffsetInner,
                                             sUI _numVertsInner,
                                             sUI _byteOffsetBorder,
                                             sUI _numVertsBorder,
                                             sUI _glPrimTypeBorder,
                                             sF32 _centerX, sF32 _centerY,
                                             sF32 _sizeX,   sF32 _sizeY
                                             ) {
   const sF32 aaOffSize   = b_aa ? Dsdvg_pixel_scl(SHADERVG_RECT_AA_SIZE_OFFSET)   : 0.0f;
   const sF32 aaOffStroke = b_aa ? Dsdvg_pixel_scl(SHADERVG_RECT_AA_STROKE_OFFSET) : 0.0f;
   rect_fill_stroke_aa.drawRectFillStrokeAAVBO32(_vboId,
                                                 _byteOffsetInner,
                                                 _numVertsInner,
                                                 _byteOffsetBorder,
                                                 _numVertsBorder,
                                                 _glPrimTypeBorder,
                                                 mvp_matrix,
                                                 _centerX, _centerY,
                                                 _sizeX + aaOffSize, _sizeY + aaOffSize,
                                                 fill_r, fill_g, fill_b, fill_a * global_a,
                                                 stroke_r, stroke_g, stroke_b, stroke_a * global_a,
                                                 Dsdvg_pixel_scl(stroke_w) + aaOffStroke,
                                                 b_aa ? Dsdvg_pixel_scl(aa_range) : SHADERVG_AA_RANGE_OFF,
                                                 aa_exp
                                                 );
}

void YAC_CALL sdvg_DrawRectFillStrokeAA(sF32 _centerX, sF32 _centerY,
                                        sF32 _sizeX,   sF32 _sizeY
                                        ) {
   const sF32 aaOffSize   = b_aa ? Dsdvg_pixel_scl(SHADERVG_RECT_AA_SIZE_OFFSET)   : 0.0f;
   const sF32 aaOffStroke = b_aa ? Dsdvg_pixel_scl(SHADERVG_RECT_AA_STROKE_OFFSET) : 0.0f;
   rect_fill_stroke_aa.drawRectFillStrokeAA(scratch_buffer,
                                            mvp_matrix,
                                            _centerX, _centerY,
                                            _sizeX + aaOffSize, _sizeY + aaOffSize,
                                            fill_r, fill_g, fill_b, fill_a * global_a,
                                            stroke_r, stroke_g, stroke_b, stroke_a * global_a,
                                            Dsdvg_pixel_scl(stroke_w) + aaOffStroke,
                                            b_aa ? Dsdvg_pixel_scl(aa_range) : SHADERVG_AA_RANGE_OFF,
                                            aa_exp
                                            );
}

void YAC_CALL sdvg_SetupRectStrokeAAVBO32(YAC_Buffer *_vb, YAC_Buffer *_dl,
                                          sF32 _centerX, sF32 _centerY,
                                          sF32 _sizeX,   sF32 _sizeY,
                                          sF32 _strokeW
                                          ) {
   // (note) 'size' is actually the radius (area is center-size..center+size)
   const sF32 aaOffSize   = b_aa ? Dsdvg_pixel_scl(SHADERVG_RECT_AA_SIZE_OFFSET)   : 0.0f;
   const sF32 aaOffStroke = b_aa ? Dsdvg_pixel_scl(SHADERVG_RECT_AA_STROKE_OFFSET) : 0.0f;
   rect_stroke_aa.setupRectStrokeAAVBO32(_vb, _dl,
                                         _centerX, _centerY,
                                         _sizeX + aaOffSize, _sizeY + aaOffSize,
                                         Dsdvg_pixel_scl(_strokeW) + aaOffStroke,
                                         Dsdvg_pixel_scl(aa_range)
                                         );
}

void YAC_CALL sdvg_DrawRectStrokeAAVBO32(sUI _vboId,
                                         sUI _byteOffsetBorder,
                                         sUI _numVertsBorder,
                                         sUI _glPrimTypeBorder,
                                         sF32 _centerX, sF32 _centerY,
                                         sF32 _sizeX,   sF32 _sizeY
                                         ) {
   const sF32 aaOffSize   = b_aa ? Dsdvg_pixel_scl(SHADERVG_RECT_AA_SIZE_OFFSET)   : 0.0f;
   const sF32 aaOffStroke = b_aa ? Dsdvg_pixel_scl(SHADERVG_RECT_AA_STROKE_OFFSET) : 0.0f;
   rect_stroke_aa.drawRectStrokeAAVBO32(_vboId,
                                        _byteOffsetBorder,
                                        _numVertsBorder,
                                        _glPrimTypeBorder,
                                        mvp_matrix,
                                        _centerX, _centerY,
                                        _sizeX + aaOffSize, _sizeY + aaOffSize,
                                        stroke_r, stroke_g, stroke_b, stroke_a * global_a,
                                        Dsdvg_pixel_scl(stroke_w) + aaOffStroke,
                                        b_aa ? Dsdvg_pixel_scl(aa_range) : SHADERVG_AA_RANGE_OFF,
                                        aa_exp
                                        );
}

void YAC_CALL sdvg_DrawRectStrokeAA(sF32 _centerX, sF32 _centerY,
                                    sF32 _sizeX,   sF32 _sizeY
                                    ) {
   const sF32 aaOffSize   = b_aa ? Dsdvg_pixel_scl(SHADERVG_RECT_AA_SIZE_OFFSET)   : 0.0f;
   const sF32 aaOffStroke = b_aa ? Dsdvg_pixel_scl(SHADERVG_RECT_AA_STROKE_OFFSET) : 0.0f;
   rect_stroke_aa.drawRectStrokeAA(scratch_buffer,
                                   mvp_matrix,
                                   _centerX, _centerY,
                                   _sizeX + aaOffSize, _sizeY + aaOffSize,
                                   stroke_r, stroke_g, stroke_b, stroke_a * global_a,
                                   Dsdvg_pixel_scl(stroke_w) + aaOffStroke,
                                   b_aa ? Dsdvg_pixel_scl(aa_range) : SHADERVG_AA_RANGE_OFF,
                                   aa_exp
                                   );
}

void YAC_CALL sdvg_SetupEllipseFillAAVBO32(YAC_Buffer *_vb, YAC_Buffer *_dl,
                                           sF32 _centerX, sF32 _centerY,
                                           sF32 _radiusX, sF32 _radiusY
                                           ) {
   const sF32 aaOff = b_aa ? Dsdvg_pixel_scl(SHADERVG_ELLIPSE_FILL_AA_SIZE_OFFSET) : 0.0f;
   ellipse_fill_aa.setupEllipseFillAAVBO32(_vb, _dl,
                                           _centerX, _centerY,
                                           _radiusX + aaOff, _radiusY + aaOff
                                           );
}

void YAC_CALL sdvg_DrawEllipseFillAAVBO32(sUI _vboId,
                                          sUI _byteOffsetInner,
                                          sUI _numVertsInner,
                                          sUI _byteOffsetBorder,
                                          sUI _numVertsBorder,
                                          sUI _glPrimTypeBorder,
                                          sF32 _centerX, sF32 _centerY,
                                          sF32 _radiusX, sF32 _radiusY
                                          ) {
   const sF32 aaOff = b_aa ? Dsdvg_pixel_scl(SHADERVG_ELLIPSE_FILL_AA_SIZE_OFFSET) : 0.0f;
   ellipse_fill_aa.drawEllipseFillAAVBO32(_vboId,
                                          _byteOffsetInner,
                                          _numVertsInner,
                                          _byteOffsetBorder,
                                          _numVertsBorder,
                                          _glPrimTypeBorder,
                                          mvp_matrix,
                                          _centerX, _centerY,
                                          _radiusX + aaOff, _radiusY + aaOff,
                                          fill_r, fill_g, fill_b, fill_a * global_a,
                                          b_aa ? Dsdvg_pixel_scl(aa_range) : SHADERVG_AA_RANGE_OFF,
                                          aa_exp
                                          );
}

void YAC_CALL sdvg_DrawEllipseFillAA(sF32 _centerX, sF32 _centerY,
                                     sF32 _sizeX,   sF32 _sizeY
                                     ) {
   const sF32 aaOff = b_aa ? Dsdvg_pixel_scl(SHADERVG_ELLIPSE_FILL_AA_SIZE_OFFSET) : 0.0f;
   ellipse_fill_aa.drawEllipseFillAA(scratch_buffer,
                                     mvp_matrix,
                                     _centerX, _centerY,
                                     _sizeX + aaOff, _sizeY + aaOff,
                                     fill_r, fill_g, fill_b, fill_a * global_a,
                                     b_aa ? Dsdvg_pixel_scl(aa_range) : SHADERVG_AA_RANGE_OFF,
                                     aa_exp
                                     );
}

void YAC_CALL sdvg_SetupEllipseFillStrokeAAVBO32(YAC_Buffer *_vb, YAC_Buffer *_dl,
                                                 sF32 _centerX, sF32 _centerY,
                                                 sF32 _radiusX, sF32 _radiusY,
                                                 sF32 _strokeW
                                                 ) {
   const sF32 aaOffSize   = b_aa ? Dsdvg_pixel_scl(SHADERVG_ELLIPSE_AA_SIZE_OFFSET)   : 0.0f;
   const sF32 aaOffStroke = b_aa ? Dsdvg_pixel_scl(SHADERVG_ELLIPSE_AA_STROKE_OFFSET) : 0.0f;
   ellipse_fill_stroke_aa.setupEllipseFillStrokeAAVBO32(_vb, _dl,
                                                        _centerX, _centerY,
                                                        _radiusX + aaOffSize, _radiusY + aaOffSize,
                                                        Dsdvg_pixel_scl(_strokeW) + aaOffStroke
                                                        );
}

void YAC_CALL sdvg_DrawEllipseFillStrokeAAVBO32(sUI _vboId,
                                                sUI _byteOffsetInner,
                                                sUI _numVertsInner,
                                                sUI _byteOffsetBorder,
                                                sUI _numVertsBorder,
                                                sUI _glPrimTypeBorder,
                                                sF32 _centerX, sF32 _centerY,
                                                sF32 _radiusX, sF32 _radiusY
                                                ) {
   const sF32 aaOffSize   = b_aa ? Dsdvg_pixel_scl(SHADERVG_ELLIPSE_AA_SIZE_OFFSET)   : 0.0f;
   const sF32 aaOffStroke = b_aa ? Dsdvg_pixel_scl(SHADERVG_ELLIPSE_AA_STROKE_OFFSET) : 0.0f;
   ellipse_fill_stroke_aa.drawEllipseFillStrokeAAVBO32(_vboId,
                                                       _byteOffsetInner,
                                                       _numVertsInner,
                                                       _byteOffsetBorder,
                                                       _numVertsBorder,
                                                       _glPrimTypeBorder,
                                                       mvp_matrix,
                                                       _centerX, _centerY,
                                                       _radiusX + aaOffSize, _radiusY + aaOffSize,
                                                       fill_r,   fill_g,   fill_b,   fill_a * global_a,
                                                       stroke_r, stroke_g, stroke_b, stroke_a * global_a,
                                                       Dsdvg_pixel_scl(stroke_w) + aaOffStroke,
                                                       b_aa ? Dsdvg_pixel_scl(aa_range) : SHADERVG_AA_RANGE_OFF,
                                                       aa_exp
                                                       );
}

void YAC_CALL sdvg_DrawEllipseFillStrokeAA(sF32 _centerX, sF32 _centerY,
                                           sF32 _sizeX,   sF32 _sizeY
                                           ) {
   const sF32 aaOffSize   = b_aa ? Dsdvg_pixel_scl(SHADERVG_ELLIPSE_AA_SIZE_OFFSET)   : 0.0f;
   const sF32 aaOffStroke = b_aa ? Dsdvg_pixel_scl(SHADERVG_ELLIPSE_AA_STROKE_OFFSET) : 0.0f;
   ellipse_fill_stroke_aa.drawEllipseFillStrokeAA(scratch_buffer,
                                                  mvp_matrix,
                                                  _centerX, _centerY,
                                                  _sizeX + aaOffSize, _sizeY + aaOffSize,
                                                  fill_r, fill_g, fill_b, fill_a * global_a,
                                                  stroke_r, stroke_g, stroke_b, stroke_a * global_a,
                                                  Dsdvg_pixel_scl(stroke_w) + aaOffStroke,
                                                  b_aa ? Dsdvg_pixel_scl(aa_range) : SHADERVG_AA_RANGE_OFF,
                                                  aa_exp
                                                  );
}

void YAC_CALL sdvg_SetupEllipseStrokeAAVBO32(YAC_Buffer *_vb, YAC_Buffer *_dl,
                                             sF32 _centerX, sF32 _centerY,
                                             sF32 _radiusX, sF32 _radiusY,
                                             sF32 _strokeW
                                             ) {
   const sF32 aaOffSize   = b_aa ? Dsdvg_pixel_scl(SHADERVG_ELLIPSE_AA_SIZE_OFFSET)   : 0.0f;
   const sF32 aaOffStroke = b_aa ? Dsdvg_pixel_scl(SHADERVG_ELLIPSE_AA_STROKE_OFFSET) : 0.0f;
   ellipse_stroke_aa.setupEllipseStrokeAAVBO32(_vb, _dl,
                                               _centerX, _centerY,
                                               _radiusX + aaOffSize, _radiusY + aaOffSize,
                                               Dsdvg_pixel_scl(_strokeW) + aaOffStroke
                                               );
}

void YAC_CALL sdvg_DrawEllipseStrokeAAVBO32(sUI _vboId,
                                            sUI _byteOffsetBorder,
                                            sUI _numVertsBorder,
                                            sUI _glPrimTypeBorder,
                                            sF32 _centerX, sF32 _centerY,
                                            sF32 _radiusX, sF32 _radiusY
                                            ) {
   const sF32 aaOffSize   = b_aa ? Dsdvg_pixel_scl(SHADERVG_ELLIPSE_AA_SIZE_OFFSET)   : 0.0f;
   const sF32 aaOffStroke = b_aa ? Dsdvg_pixel_scl(SHADERVG_ELLIPSE_AA_STROKE_OFFSET) : 0.0f;
   ellipse_stroke_aa.drawEllipseStrokeAAVBO32(_vboId,
                                              _byteOffsetBorder,
                                              _numVertsBorder,
                                              _glPrimTypeBorder,
                                              mvp_matrix,
                                              _centerX, _centerY,
                                              _radiusX + aaOffSize, _radiusY + aaOffSize,
                                              stroke_r, stroke_g, stroke_b, stroke_a * global_a,
                                              Dsdvg_pixel_scl(stroke_w) + aaOffStroke,
                                              b_aa ? Dsdvg_pixel_scl(aa_range) : SHADERVG_AA_RANGE_OFF,
                                              aa_exp
                                              );
}

void YAC_CALL sdvg_DrawEllipseStrokeAA(sF32 _centerX, sF32 _centerY,
                                       sF32 _sizeX, sF32 _sizeY
                                       ) {
   const sF32 aaOffSize   = b_aa ? Dsdvg_pixel_scl(SHADERVG_ELLIPSE_AA_SIZE_OFFSET)   : 0.0f;
   const sF32 aaOffStroke = b_aa ? Dsdvg_pixel_scl(SHADERVG_ELLIPSE_AA_STROKE_OFFSET) : 0.0f;
   ellipse_stroke_aa.drawEllipseStrokeAA(scratch_buffer,
                                         mvp_matrix,
                                         _centerX, _centerY,
                                         _sizeX + aaOffSize, _sizeY + aaOffSize,
                                         stroke_r, stroke_g, stroke_b, stroke_a * global_a,
                                         Dsdvg_pixel_scl(stroke_w) + aaOffStroke,
                                         b_aa ? Dsdvg_pixel_scl(aa_range) : SHADERVG_AA_RANGE_OFF,
                                         aa_exp
                                         );
}

void YAC_CALL sdvg_SetupRoundRectFillAAVBO32(YAC_Buffer *_vb, YAC_Buffer *_dl,
                                             sF32 _centerX, sF32 _centerY,
                                             sF32 _sizeX, sF32 _sizeY,
                                             sF32 _radiusX, sF32 _radiusY
                                             ) {
   const sF32 aaOff = b_aa ? Dsdvg_pixel_scl(SHADERVG_ROUNDRECT_FILL_AA_SIZE_OFFSET) : 0.0f;
   roundrect_fill_aa.setupRoundRectFillAAVBO32(_vb, _dl,
                                               _centerX, _centerY,
                                               _sizeX + aaOff, _sizeY + aaOff,
                                               _radiusX, _radiusY,
                                               Dsdvg_pixel_scl(aa_range)
                                               );
}

void YAC_CALL sdvg_DrawRoundRectFillAAVBO32(sUI _vboId,
                                            sUI _byteOffsetInner,
                                            sUI _numVertsInner,
                                            sUI _byteOffsetBorder,
                                            sUI _numVertsBorder,
                                            sUI _glPrimTypeBorder,
                                            sF32 _centerX, sF32 _centerY,
                                            sF32 _sizeX,   sF32 _sizeY,
                                            sF32 _radiusX, sF32 _radiusY
                                            ) {
   const sF32 aaOff = b_aa ? Dsdvg_pixel_scl(SHADERVG_ROUNDRECT_FILL_AA_SIZE_OFFSET) : 0.0f;
   roundrect_fill_aa.drawRoundRectFillAAVBO32(_vboId,
                                              _byteOffsetInner,
                                              _numVertsInner,
                                              _byteOffsetBorder,
                                              _numVertsBorder,
                                              _glPrimTypeBorder,
                                              mvp_matrix,
                                              _centerX, _centerY,
                                              _sizeX + aaOff, _sizeY + aaOff,
                                              _radiusX, _radiusY,
                                              fill_r, fill_g, fill_b, fill_a * global_a,
                                              b_aa ? Dsdvg_pixel_scl(aa_range) : SHADERVG_AA_RANGE_OFF,
                                              aa_exp
                                              );
}

void YAC_CALL sdvg_DrawRoundRectFillAA(sF32 _centerX, sF32 _centerY,
                                       sF32 _sizeX,   sF32 _sizeY,
                                       sF32 _radiusX, sF32 _radiusY
                                       ) {
   const sF32 aaOff = b_aa ? Dsdvg_pixel_scl(SHADERVG_ROUNDRECT_FILL_AA_SIZE_OFFSET) : 0.0f;
   roundrect_fill_aa.drawRoundRectFillAA(scratch_buffer,
                                         mvp_matrix,
                                         _centerX, _centerY,
                                         _sizeX + aaOff, _sizeY + aaOff,
                                         _radiusX, _radiusY,
                                         fill_r, fill_g, fill_b, fill_a * global_a,
                                         b_aa ? Dsdvg_pixel_scl(aa_range) : SHADERVG_AA_RANGE_OFF,
                                         aa_exp
                                         );
}

void YAC_CALL sdvg_SetupRoundRectFillStrokeAAVBO32(YAC_Buffer *_vb, YAC_Buffer *_dl,
                                                   sF32 _centerX, sF32 _centerY,
                                                   sF32 _sizeX,   sF32 _sizeY,
                                                   sF32 _radiusX, sF32 _radiusY,
                                                   sF32 _strokeW
                                                   ) {
   const sF32 aaOffSize   = b_aa ? Dsdvg_pixel_scl(SHADERVG_ROUNDRECT_AA_SIZE_OFFSET)   : 0.0f;
   const sF32 aaOffStroke = b_aa ? Dsdvg_pixel_scl(SHADERVG_ROUNDRECT_AA_STROKE_OFFSET) : 0.0f;
   roundrect_fill_stroke_aa.setupRoundRectFillStrokeAAVBO32(_vb, _dl,
                                                            _centerX, _centerY,
                                                            _sizeX + aaOffSize, _sizeY + aaOffSize,
                                                            _radiusX, _radiusY,
                                                            Dsdvg_pixel_scl(_strokeW) + aaOffStroke,
                                                            Dsdvg_pixel_scl(aa_range)
                                                            );
}

void YAC_CALL sdvg_DrawRoundRectFillStrokeAAVBO32(sUI _vboId,
                                                  sUI _byteOffsetInner,
                                                  sUI _numVertsInner,
                                                  sUI _byteOffsetBorder,
                                                  sUI _numVertsBorder,
                                                  sUI _glPrimTypeBorder,
                                                  sF32 _centerX, sF32 _centerY,
                                                  sF32 _sizeX,   sF32 _sizeY,
                                                  sF32 _radiusX, sF32 _radiusY
                                                  ) {
   const sF32 aaOffSize   = b_aa ? Dsdvg_pixel_scl(SHADERVG_ROUNDRECT_AA_SIZE_OFFSET)   : 0.0f;
   const sF32 aaOffStroke = b_aa ? Dsdvg_pixel_scl(SHADERVG_ROUNDRECT_AA_STROKE_OFFSET) : 0.0f;
   roundrect_fill_stroke_aa.drawRoundRectFillStrokeAAVBO32(_vboId,
                                                           _byteOffsetInner,
                                                           _numVertsInner,
                                                           _byteOffsetBorder,
                                                           _numVertsBorder,
                                                           _glPrimTypeBorder,
                                                           mvp_matrix,
                                                           _centerX, _centerY,
                                                           _sizeX + aaOffSize, _sizeY + aaOffSize,
                                                           _radiusX, _radiusY,
                                                           fill_r, fill_g, fill_b, fill_a * global_a,
                                                           stroke_r, stroke_g, stroke_b, stroke_a * global_a,
                                                           Dsdvg_pixel_scl(stroke_w) + aaOffStroke,
                                                           b_aa ? Dsdvg_pixel_scl(aa_range) : SHADERVG_AA_RANGE_OFF,
                                                           aa_exp
                                                           );
}

void YAC_CALL sdvg_DrawRoundRectFillStrokeAA(sF32 _centerX, sF32 _centerY,
                                             sF32 _sizeX,   sF32 _sizeY,
                                             sF32 _radiusX, sF32 _radiusY
                                             ) {
   const sF32 aaOffSize   = b_aa ? Dsdvg_pixel_scl(SHADERVG_ROUNDRECT_AA_SIZE_OFFSET)   : 0.0f;
   const sF32 aaOffStroke = b_aa ? Dsdvg_pixel_scl(SHADERVG_ROUNDRECT_AA_STROKE_OFFSET) : 0.0f;
   roundrect_fill_stroke_aa.drawRoundRectFillStrokeAA(scratch_buffer,
                                                      mvp_matrix,
                                                      _centerX, _centerY,
                                                      _sizeX + aaOffSize, _sizeY + aaOffSize,
                                                      _radiusX, _radiusY,
                                                      fill_r, fill_g, fill_b, fill_a * global_a,
                                                      stroke_r, stroke_g, stroke_b, stroke_a * global_a,
                                                      Dsdvg_pixel_scl(stroke_w) + aaOffStroke,
                                                      b_aa ? Dsdvg_pixel_scl(aa_range) : SHADERVG_AA_RANGE_OFF,
                                                      aa_exp
                                                      );
}

void YAC_CALL sdvg_SetupRoundRectStrokeAAVBO32(YAC_Buffer *_vb, YAC_Buffer *_dl,
                                               sF32 _centerX, sF32 _centerY,
                                               sF32 _sizeX,   sF32 _sizeY,
                                               sF32 _radiusX, sF32 _radiusY,
                                               sF32 _strokeW
                                               ) {
   const sF32 aaOffSize   = b_aa ? Dsdvg_pixel_scl(SHADERVG_ROUNDRECT_AA_SIZE_OFFSET)   : 0.0f;
   const sF32 aaOffStroke = b_aa ? Dsdvg_pixel_scl(SHADERVG_ROUNDRECT_AA_STROKE_OFFSET) : 0.0f;
   roundrect_stroke_aa.setupRoundRectStrokeAAVBO32(_vb, _dl,
                                                   _centerX, _centerY,
                                                   _sizeX + aaOffSize, _sizeY + aaOffSize,
                                                   _radiusX, _radiusY,
                                                   Dsdvg_pixel_scl(_strokeW) + aaOffStroke,
                                                   Dsdvg_pixel_scl(aa_range)
                                                   );
}

void YAC_CALL sdvg_DrawRoundRectStrokeAAVBO32(sUI _vboId,
                                              sUI _byteOffsetBorder,
                                              sUI _numVertsBorder,
                                              sUI _glPrimTypeBorder,
                                              sF32 _centerX, sF32 _centerY,
                                              sF32 _sizeX,   sF32 _sizeY,
                                              sF32 _radiusX, sF32 _radiusY
                                              ) {
   const sF32 aaOffSize   = b_aa ? Dsdvg_pixel_scl(SHADERVG_ROUNDRECT_AA_SIZE_OFFSET)   : 0.0f;
   const sF32 aaOffStroke = b_aa ? Dsdvg_pixel_scl(SHADERVG_ROUNDRECT_AA_STROKE_OFFSET) : 0.0f;
   roundrect_stroke_aa.drawRoundRectStrokeAAVBO32(_vboId,
                                                  _byteOffsetBorder,
                                                  _numVertsBorder,
                                                  _glPrimTypeBorder,
                                                  mvp_matrix,
                                                  _centerX, _centerY,
                                                  _sizeX + aaOffSize, _sizeY + aaOffSize,
                                                  _radiusX, _radiusY,
                                                  stroke_r, stroke_g, stroke_b, stroke_a * global_a,
                                                  Dsdvg_pixel_scl(stroke_w) + aaOffStroke,
                                                  b_aa ? Dsdvg_pixel_scl(aa_range) : SHADERVG_AA_RANGE_OFF,
                                                  aa_exp
                                                  );
}

void YAC_CALL sdvg_DrawRoundRectStrokeAA(sF32 _centerX, sF32 _centerY,
                                         sF32 _sizeX,   sF32 _sizeY,
                                         sF32 _radiusX, sF32 _radiusY
                                         ) {
   const sF32 aaOffSize   = b_aa ? Dsdvg_pixel_scl(SHADERVG_ROUNDRECT_AA_SIZE_OFFSET)   : 0.0f;
   const sF32 aaOffStroke = b_aa ? Dsdvg_pixel_scl(SHADERVG_ROUNDRECT_AA_STROKE_OFFSET) : 0.0f;
   roundrect_stroke_aa.drawRoundRectStrokeAA(scratch_buffer,
                                             mvp_matrix,
                                             _centerX, _centerY,
                                             _sizeX + aaOffSize, _sizeY + aaOffSize,
                                             _radiusX, _radiusY,
                                             stroke_r, stroke_g, stroke_b, stroke_a * global_a,
                                             Dsdvg_pixel_scl(stroke_w) + aaOffStroke,
                                             b_aa ? Dsdvg_pixel_scl(aa_range) : SHADERVG_AA_RANGE_OFF,
                                             aa_exp
                                             );
}

void YAC_CALL sdvg_DrawTrianglesTexUVFlatVBO32(sUI _vboId, sUI _byteOffset, sUI _numVerts) {
   //
   // VBO vertex format (16 bytes per vertex):
   //     +0  f32 x
   //     +4  f32 y
   //     +8  f32 u
   //     +12 f32 v
   //
   triangles_tex_uv_flat_32.drawTrianglesTexUVFlatVBO32(_vboId,
                                                        _byteOffset,
                                                        _numVerts,
                                                        mvp_matrix,
                                                        fill_r, fill_g, fill_b, fill_a * global_a
                                                        );
}

void YAC_CALL sdvg_DrawTrianglesTexUVGouraudVBO32(sUI _vboId, sUI _byteOffset, sUI _numVerts) {
   //
   // VBO vertex format (20 bytes per vertex):
   //     +0  f32 x
   //     +4  f32 y
   //     +8  f32 u
   //     +12 f32 v
   //     +16  u8 r
   //     +17  u8 g
   //     +18  u8 b
   //     +19  u8 a
   //
   triangles_tex_uv_gouraud_32.drawTrianglesTexUVGouraudVBO32(_vboId,
                                                              _byteOffset,
                                                              _numVerts,
                                                              mvp_matrix,
                                                              fill_r, fill_g, fill_b, fill_a * global_a
                                                              );
}

void YAC_CALL sdvg_DrawTrianglesTexUVFlatDecalVBO32(sUI _vboId, sUI _byteOffset, sUI _numVerts) {
   //
   // VBO vertex format (16 bytes per vertex):
   //     +0  f32 x
   //     +4  f32 y
   //     +8  f32 u
   //     +12 f32 v
   //
   triangles_tex_uv_flat_decal_32.drawTrianglesTexUVFlatDecalVBO32(_vboId,
                                                                   _byteOffset,
                                                                   _numVerts,
                                                                   mvp_matrix,
                                                                   fill_r, fill_g, fill_b, fill_a * global_a,
                                                                   texture_decal_alpha
                                                                   );
}

void YAC_CALL sdvg_DrawTrianglesTexUVGouraudDecalVBO32(sUI _vboId, sUI _byteOffset, sUI _numVerts) {
   //
   // VBO vertex format (20 bytes per vertex):
   //     +0  f32 x
   //     +4  f32 y
   //     +8  f32 u
   //     +12 f32 v
   //     +16  u8 r
   //     +17  u8 g
   //     +18  u8 b
   //     +19  u8 a
   //
   triangles_tex_uv_gouraud_decal_32.drawTrianglesTexUVGouraudDecalVBO32(_vboId,
                                                                         _byteOffset,
                                                                         _numVerts,
                                                                         mvp_matrix,
                                                                         fill_r, fill_g, fill_b, fill_a * global_a,
                                                                         texture_decal_alpha
                                                                         );
}

void YAC_CALL sdvg_DrawTrianglesTexUVFlatVBO32Alpha(sUI _vboId, sUI _byteOffset, sUI _numVerts) {
   //
   // VBO vertex format (16 bytes per vertex):
   //     +0  f32 x
   //     +4  f32 y
   //     +8  f32 u
   //     +12 f32 v
   //
   triangles_tex_uv_flat_32_alpha.drawTrianglesTexUVFlatVBO32Alpha(_vboId,
                                                                   _byteOffset,
                                                                   _numVerts,
                                                                   mvp_matrix,
                                                                   fill_r, fill_g, fill_b, fill_a * global_a
                                                                   );
}

void YAC_CALL sdvg_DrawTrianglesTexUVGouraudVBO32Alpha(sUI _vboId, sUI _byteOffset, sUI _numVerts) {
   //
   // VBO vertex format (20 bytes per vertex):
   //     +0  f32 x
   //     +4  f32 y
   //     +8  f32 u
   //     +12 f32 v
   //     +16  u8 r
   //     +17  u8 g
   //     +18  u8 b
   //     +19  u8 a
   //
   triangles_tex_uv_gouraud_32_alpha.drawTrianglesTexUVGouraudVBO32Alpha(_vboId,
                                                                         _byteOffset,
                                                                         _numVerts,
                                                                         mvp_matrix,
                                                                         fill_r, fill_g, fill_b, fill_a * global_a
                                                                         );
}

void YAC_CALL sdvg_DrawTrianglesTexUVFlatDecalVBO32Alpha(sUI _vboId, sUI _byteOffset, sUI _numVerts) {
   //
   // VBO vertex format (16 bytes per vertex):
   //     +0  f32 x
   //     +4  f32 y
   //     +8  f32 u
   //     +12 f32 v
   //
   triangles_tex_uv_flat_decal_32_alpha.drawTrianglesTexUVFlatDecalVBO32Alpha(_vboId,
                                                                              _byteOffset,
                                                                              _numVerts,
                                                                              mvp_matrix,
                                                                              fill_r, fill_g, fill_b, fill_a * global_a,
                                                                              stroke_r, stroke_g, stroke_b, stroke_a,
                                                                              texture_decal_alpha
                                                                              );
}

void YAC_CALL sdvg_DrawTrianglesTexUVGouraudDecalVBO32Alpha(sUI _vboId, sUI _byteOffset, sUI _numVerts) {
   //
   // VBO vertex format (20 bytes per vertex):
   //     +0  f32 x
   //     +4  f32 y
   //     +8  f32 u
   //     +12 f32 v
   //     +16  u8 r
   //     +17  u8 g
   //     +18  u8 b
   //     +19  u8 a
   //
   triangles_tex_uv_gouraud_decal_32_alpha.drawTrianglesTexUVGouraudDecalVBO32Alpha(_vboId,
                                                                                    _byteOffset,
                                                                                    _numVerts,
                                                                                    mvp_matrix,
                                                                                    fill_r, fill_g, fill_b, fill_a * global_a,
                                                                                    stroke_r, stroke_g, stroke_b, stroke_a,
                                                                                    texture_decal_alpha
                                                                                    );
}

void YAC_CALL sdvg_DrawTrianglesTexUVFlatVBO32AlphaSDF(sUI _vboId, sUI _byteOffset, sUI _numVerts) {
   //
   // VBO vertex format (16 bytes per vertex):
   //     +0  f32 x
   //     +4  f32 y
   //     +8  f32 u
   //     +12 f32 v
   //
   triangles_tex_uv_flat_32_alpha_sdf.drawTrianglesTexUVFlatVBO32AlphaSDF(_vboId,
                                                                          _byteOffset,
                                                                          _numVerts,
                                                                          mvp_matrix,
                                                                          fill_r, fill_g, fill_b, fill_a * global_a,
                                                                          alpha_sdf_min,
                                                                          alpha_sdf_max,
                                                                          alpha_sdf_maxmin_scale,
                                                                          alpha_sdf_exp
                                                                          );
}

void YAC_CALL sdvg_DrawLineStripFlatVBO14_2(sUI _vboId, sUI _byteOffset, sUI _numPoints) {
   //
   // VBO vertex format (6 bytes per vertex):
   //   +0 s14.2 x
   //   +2 s14.2 y
   //   +4 i16   index
   //
   // (note) numVerts = 6*numPoints
   // (note) numSeg   = (numPoints - 1)
   //
   const sF32 aaOff = (0 && b_aa) ? Dsdvg_pixel_scl(SHADERVG_LINE_STROKE_W_OFFSET) : 0.0f;
   line_strip_flat_14_2.drawLineStripFlatVBO14_2(_vboId,
                                                 _byteOffset,
                                                 _numPoints,
                                                 mvp_matrix,
                                                 stroke_r, stroke_g, stroke_b, stroke_a * global_a,
                                                 Dsdvg_pixel_scl(stroke_w) + aaOff
                                                 );
}

void YAC_CALL sdvg_DrawLineStripFlatVBO32(sUI _vboId, sUI _byteOffset, sUI _numPoints) {
   //
   // VBO vertex format (10 bytes per vertex):
   //   +0 f32 x
   //   +4 f32 y
   //   +8 i16   index
   //
   // (note) numVerts         = (numPoints * 6)
   // (note) numSeg           = (numPoints - 1)
   // (note) numTri           = (numPoints - 1) * 6
   // (note) numBytesPerPoint = 6*10 = 60
   //
   const sF32 aaOff = (0 && b_aa) ? Dsdvg_pixel_scl(SHADERVG_LINE_STROKE_W_OFFSET) : 0.0f;
   line_strip_flat_32.drawLineStripFlatVBO32(_vboId,
                                             _byteOffset,
                                             _numPoints,
                                             mvp_matrix,
                                             stroke_r, stroke_g, stroke_b, stroke_a * global_a,
                                             Dsdvg_pixel_scl(stroke_w) + aaOff
                                             );
}

void YAC_CALL sdvg_DrawLineStripFlatAAVBO14_2(sUI _vboId, sUI _byteOffset, sUI _numPoints) {
   //
   // VBO vertex format (6 bytes per vertex):
   //   +0 s14.2 x
   //   +2 s14.2 y
   //   +4 i16   index
   //
   // (note) numVerts = 6*numPoints
   // (note) numSeg   = (numPoints - 1)
   //
   const sF32 aaOff = b_aa ? Dsdvg_pixel_scl(SHADERVG_LINE_STROKE_W_OFFSET) : 0.0f;
   line_strip_flat_aa_14_2.drawLineStripFlatAAVBO14_2(_vboId,
                                                      _byteOffset,
                                                      _numPoints,
                                                      mvp_matrix,
                                                      stroke_r, stroke_g, stroke_b, stroke_a * global_a,
                                                      Dsdvg_pixel_scl(stroke_w) + aaOff,
                                                      b_aa ? Dsdvg_pixel_scl(aa_range) : SHADERVG_AA_RANGE_OFF
                                                      );
}

void YAC_CALL sdvg_DrawLineStripFlatAAVBO32(sUI _vboId, sUI _byteOffset, sUI _numPoints) {
   //
   // VBO vertex format (10 bytes per vertex):
   //   +0 f32 x
   //   +4 f32 y
   //   +8 i16   index
   //
   // (note) numVerts         = (numPoints * 6)
   // (note) numSeg           = (numPoints - 1)
   // (note) numTri           = (numPoints - 1) * 6
   // (note) numBytesPerPoint = 6*10 = 60
   //
   const sF32 aaOff = b_aa ? Dsdvg_pixel_scl(SHADERVG_LINE_STROKE_W_OFFSET) : 0.0f;
   line_strip_flat_aa_32.drawLineStripFlatAAVBO32(_vboId,
                                                  _byteOffset,
                                                  _numPoints,
                                                  mvp_matrix,
                                                  stroke_r, stroke_g, stroke_b, stroke_a * global_a,
                                                  Dsdvg_pixel_scl(stroke_w) + aaOff,
                                                  b_aa ? Dsdvg_pixel_scl(aa_range) : SHADERVG_AA_RANGE_OFF
                                                  );
}

void YAC_CALL sdvg_DrawLineStripFlatBevelVBO14_2(sUI _vboId, sUI _byteOffset, sUI _numPoints) {
   //
   // VBO vertex format (6 bytes per vertex):
   //   +0 s14.2 x
   //   +2 s14.2 y
   //   +4 i16   index
   //
   // (note) numVerts = 9*numPoints
   // (note) numSeg   = (numPoints - 1)
   //
   line_strip_flat_bevel_aa_14_2.drawLineStripFlatBevelAAVBO14_2(_vboId,
                                                                 _byteOffset,
                                                                 _numPoints,
                                                                 mvp_matrix,
                                                                 stroke_r, stroke_g, stroke_b, stroke_a * global_a,
                                                                 Dsdvg_pixel_scl(stroke_w),
                                                                 SHADERVG_AA_RANGE_OFF
                                                                 );
}

void YAC_CALL sdvg_DrawLineStripFlatBevelVBO32(sUI _vboId, sUI _byteOffset, sUI _numPoints) {
   //
   // VBO vertex format (10 bytes per vertex):
   //   +0 f32 x
   //   +4 f32 y
   //   +8 i16 index
   //
   // (note) numVerts         = (numPoints * 9)
   // (note) numSeg           = (numPoints - 1)
   // (note) numTri           = (numPoints - 1) * 2 + (numPoints - 2)
   // (note) numBytesPerPoint = 9*10 = 90
   //
   line_strip_flat_bevel_aa_32.drawLineStripFlatBevelAAVBO32(_vboId,
                                                             _byteOffset,
                                                             _numPoints,
                                                             mvp_matrix,
                                                             stroke_r, stroke_g, stroke_b, stroke_a * global_a,
                                                             Dsdvg_pixel_scl(stroke_w),
                                                             SHADERVG_AA_RANGE_OFF
                                                             );
}

void YAC_CALL sdvg_DrawLineStripFlatBevelAAVBO14_2(sUI _vboId, sUI _byteOffset, sUI _numPoints) {
   //
   // VBO vertex format (6 bytes per vertex):
   //   +0 s14.2 x
   //   +2 s14.2 y
   //   +4 i16   index
   //
   // (note) numVerts = 9*numPoints
   // (note) numSeg   = (numPoints - 1)
   //
   const sF32 aaOff = b_aa ? Dsdvg_pixel_scl(SHADERVG_LINE_STROKE_W_OFFSET) : 0.0f;
   line_strip_flat_bevel_aa_14_2.drawLineStripFlatBevelAAVBO14_2(_vboId,
                                                                 _byteOffset,
                                                                 _numPoints,
                                                                 mvp_matrix,
                                                                 stroke_r, stroke_g, stroke_b, stroke_a * global_a,
                                                                 Dsdvg_pixel_scl(stroke_w) + aaOff,
                                                                 b_aa ? Dsdvg_pixel_scl(aa_range) : SHADERVG_AA_RANGE_OFF
                                                                 );
}

void YAC_CALL sdvg_DrawLineStripFlatBevelAAVBO32(sUI _vboId, sUI _byteOffset, sUI _numPoints) {
   //
   // VBO vertex format (10 bytes per vertex):
   //   +0 f32 x
   //   +4 f32 y
   //   +8 i16 index
   //
   // (note) numVerts         = (numPoints * 9)
   // (note) numSeg           = (numPoints - 1)
   // (note) numTri           = (numPoints - 1) * 2 + (numPoints - 2)
   // (note) numBytesPerPoint = 9*10 = 90
   //
   const sF32 aaOff = b_aa ? Dsdvg_pixel_scl(SHADERVG_LINE_STROKE_W_OFFSET) : 0.0f;
   line_strip_flat_bevel_aa_32.drawLineStripFlatBevelAAVBO32(_vboId,
                                                             _byteOffset,
                                                             _numPoints,
                                                             mvp_matrix,
                                                             stroke_r, stroke_g, stroke_b, stroke_a * global_a,
                                                             Dsdvg_pixel_scl(stroke_w) + aaOff,
                                                             b_aa ? Dsdvg_pixel_scl(aa_range) : SHADERVG_AA_RANGE_OFF
                                                             );
}

void YAC_CALL sdvg_DrawLinesFlatVBO14_2(sUI _vboId, sUI _byteOffset, sUI _numPoints) {
   //
   // VBO vertex format (6 bytes per vertex):
   //   +0 s14.2 x
   //   +2 s14.2 y
   //   +4 i16   index (0..11)
   //
   // (note) numVerts         = (numPoints-1) * 6
   // (note) numSeg           = (numPoints / 2)
   // (note) numTri           = (numPoints-1) * 2
   // (note) numBytesPerPoint = 6*6 = 36
   // (note) 2 visible tris are followed by 2 degenerated tris (before next segment)
   //
   lines_flat_aa_14_2.drawLinesFlatAAVBO14_2(_vboId,
                                             _byteOffset,
                                             _numPoints,
                                             mvp_matrix,
                                             stroke_r, stroke_g, stroke_b, stroke_a * global_a,
                                             Dsdvg_pixel_scl(stroke_w),
                                             SHADERVG_AA_RANGE_OFF
                                             );
}

void YAC_CALL sdvg_DrawLinesFlatVBO32(sUI _vboId, sUI _byteOffset, sUI _numPoints) {
   //
   // VBO vertex format (10 bytes per vertex):
   //   +0 f32 x
   //   +4 f32 y
   //   +8 i16 index (0..11)
   //
   // (note) numVerts         = (numPoints-1) * 6
   // (note) numSeg           = (numPoints / 2)
   // (note) numTri           = (numPoints-1) * 2
   // (note) numBytesPerPoint = 6*10 = 60
   // (note) 2 visible tris are followed by 2 degenerated tris (before next segment)
   //
   lines_flat_aa_32.drawLinesFlatAAVBO32(_vboId,
                                         _byteOffset,
                                         _numPoints,
                                         mvp_matrix,
                                         stroke_r, stroke_g, stroke_b, stroke_a * global_a,
                                         Dsdvg_pixel_scl(stroke_w),
                                         SHADERVG_AA_RANGE_OFF
                                         );
}

void YAC_CALL sdvg_DrawLinesFlatAAVBO14_2(sUI _vboId, sUI _byteOffset, sUI _numPoints) {
   //
   // VBO vertex format (6 bytes per vertex):
   //   +0 s14.2 x
   //   +2 s14.2 y
   //   +4 i16   index (0..11)
   //
   // (note) numVerts         = (numPoints-1) * 6
   // (note) numSeg           = (numPoints / 2)
   // (note) numTri           = (numPoints-1) * 2
   // (note) numBytesPerPoint = 6*6 = 36
   // (note) 2 visible tris are followed by 2 degenerated tris (before next segment)
   //
   const sF32 aaOff = b_aa ? Dsdvg_pixel_scl(SHADERVG_LINE_STROKE_W_OFFSET) : 0.0f;
   lines_flat_aa_14_2.drawLinesFlatAAVBO14_2(_vboId,
                                             _byteOffset,
                                             _numPoints,
                                             mvp_matrix,
                                             stroke_r, stroke_g, stroke_b, stroke_a * global_a,
                                             Dsdvg_pixel_scl(stroke_w) + aaOff,
                                             b_aa ? Dsdvg_pixel_scl(aa_range) : SHADERVG_AA_RANGE_OFF
                                             );
}

void YAC_CALL sdvg_DrawLinesFlatAAVBO32(sUI _vboId, sUI _byteOffset, sUI _numPoints) {
   //
   // VBO vertex format (10 bytes per vertex):
   //   +0 f32 x
   //   +4 f32 y
   //   +8 i16 index (0..11)
   //
   // (note) numVerts         = (numPoints-1) * 6
   // (note) numSeg           = (numPoints / 2)
   // (note) numTri           = (numPoints-1) * 2
   // (note) numBytesPerPoint = 6*10 = 60
   // (note) 2 visible tris are followed by 2 degenerated tris (before next segment)
   //
   const sF32 aaOff = b_aa ? Dsdvg_pixel_scl(SHADERVG_LINE_STROKE_W_OFFSET) : 0.0f;
   lines_flat_aa_32.drawLinesFlatAAVBO32(_vboId,
                                         _byteOffset,
                                         _numPoints,
                                         mvp_matrix,
                                         stroke_r, stroke_g, stroke_b, stroke_a * global_a,
                                         Dsdvg_pixel_scl(stroke_w) + aaOff,
                                         b_aa ? Dsdvg_pixel_scl(aa_range) : SHADERVG_AA_RANGE_OFF
                                         );
}

void YAC_CALL sdvg_DrawPointsSquareVBO32(sUI _vboId, sUI _byteOffset, sUI _numPoints) {
   //
   // VBO vertex format (10 bytes per vertex):
   //   +0 f32 x
   //   +4 f32 y
   //   +8 i16   index (0..5)
   //
   // (note) numVerts          = (numPoints * 6)
   // (note) numTri            = numPoints * 4
   // (note) numBytesPerPoint  = 6*10 = 60
   //
   points_square_aa_32.drawPointsSquareAAVBO32(_vboId,
                                               _byteOffset,
                                               _numPoints,
                                               mvp_matrix,
                                               stroke_r, stroke_g, stroke_b, stroke_a * global_a,
                                               Dsdvg_pixel_scl(point_radius),
                                               SHADERVG_AA_RANGE_OFF
                                               );
}

void YAC_CALL sdvg_DrawPointsSquareAAVBO32(sUI _vboId, sUI _byteOffset, sUI _numPoints) {
   //
   // VBO vertex format (10 bytes per vertex):
   //   +0 f32 x
   //   +4 f32 y
   //   +8 i16   index (0..5)
   //
   // (note) numVerts          = (numPoints * 6)
   // (note) numTri            = numPoints * 4
   // (note) numBytesPerPoint  = 6*10 = 60
   //
   const sF32 aaOff = b_aa ? Dsdvg_pixel_scl(SHADERVG_POINTS_AA_RADIUS_OFFSET) : 0.0f;
   points_square_aa_32.drawPointsSquareAAVBO32(_vboId,
                                               _byteOffset,
                                               _numPoints,
                                               mvp_matrix,
                                               stroke_r, stroke_g, stroke_b, stroke_a * global_a,
                                               Dsdvg_pixel_scl(point_radius) + aaOff,
                                               b_aa ? Dsdvg_pixel_scl(aa_range) : SHADERVG_AA_RANGE_OFF
                                               );
}

void YAC_CALL sdvg_DrawPointsRoundVBO32(sUI _vboId, sUI _byteOffset, sUI _numPoints) {
   //
   // VBO vertex format (10 bytes per vertex):
   //   +0 f32 x
   //   +4 f32 y
   //   +8 i16   index (0..5)
   //
   // (note) numVerts          = (numPoints * 6)
   // (note) numTri            = numPoints * 4
   // (note) numBytesPerPoint  = 6*10 = 60
   //
   points_round_aa_32.drawPointsRoundAAVBO32(_vboId,
                                             _byteOffset,
                                             _numPoints,
                                             mvp_matrix,
                                             stroke_r, stroke_g, stroke_b, stroke_a * global_a,
                                             Dsdvg_pixel_scl(point_radius),
                                             SHADERVG_AA_RANGE_OFF
                                             );
}

void YAC_CALL sdvg_DrawPointsRoundAAVBO32(sUI _vboId, sUI _byteOffset, sUI _numPoints) {
   //
   // VBO vertex format (10 bytes per vertex):
   //   +0 f32 x
   //   +4 f32 y
   //   +8 i16   index (0..5)
   //
   // (note) numVerts          = (numPoints * 6)
   // (note) numTri            = numPoints * 4
   // (note) numBytesPerPoint  = 6*10 = 60
   //
   const sF32 aaOff = b_aa ? Dsdvg_pixel_scl(SHADERVG_POINTS_AA_RADIUS_OFFSET) : 0.0f;
   Dsdvg_tracecallv("[trc] sdvg_DrawPointsRoundAAVBO32: vboId=%u byteOffset=%u numPoints=%u point_radius=%f (scaled=%f)\n", _vboId, _byteOffset, _numPoints, point_radius, Dsdvg_pixel_scl(point_radius));
   points_round_aa_32.drawPointsRoundAAVBO32(_vboId,
                                             _byteOffset,
                                             _numPoints,
                                             mvp_matrix,
                                             stroke_r, stroke_g, stroke_b, stroke_a * global_a,
                                             Dsdvg_pixel_scl(point_radius) + aaOff,
                                             b_aa ? Dsdvg_pixel_scl(aa_range) : SHADERVG_AA_RANGE_OFF
                                             );
}

static void loc_CreateScratchBuffer(void) {
   if(Dyac_host_yacGetDebugLevel() >= 1u)
   {
      Dsdvg_debugprintf("[dbg] shadervg:loc_CreateScratchBuffer: scratch_buffer_sz=%u\n", scratch_buffer_sz);
   }
   scratch_vbo_id = sdvg_CreateVBO(scratch_buffer_sz);
}

// static GetScratchBuffer() : Buffer {
//       return scratch_buffer;
//    }

void BindScratchBuffer(void) {
   Dsdvg_tracecall("[trc] sdvg:BindScratchBuffer current_vbo_id=%u scratch_vbo_id=%u\n", current_vbo_id, scratch_vbo_id);
   if(current_vbo_id != scratch_vbo_id)
   {
      Dsdvg_debugprintfv("[trc] sdvg:BindScratchBuffer: scratch_vbo_id=%u\n", scratch_vbo_id);
      Dsdvg_glcall(glBindBuffer(GL_ARRAY_BUFFER, scratch_vbo_id));
      current_vbo_id = scratch_vbo_id;
      // map GPU buffer to virtual address and reset io_offset
      sUI oldOffset = scratch_buffer->io_offset;
      Dsdvg_debugprintfv("[trc] sdvg:BindScratchBuffer: map scratch_buffer_sz=%u\n", scratch_buffer_sz);
#ifndef SHADERVG_USE_SCRATCHBUFFERSUBDATA
      loc_map_buffer(scratch_buffer, scratch_buffer_sz);
#endif // SHADERVG_USE_SCRATCHBUFFERSUBDATA
      Dsdvg_debugprintfv("[trc] sdvg:BindScratchBuffer: map => scratch_buffer.size=%u\n", scratch_buffer->size);
      scratch_buffer->io_offset = oldOffset;
   }
}

void UnbindScratchBuffer(void) {
   // called by EndFrame() via glBindVBO(0)
   if(scratch_vbo_id == current_vbo_id)
   {
#ifndef SHADERVG_USE_SCRATCHBUFFERSUBDATA
      Dsdvg_tracecall("[trc] sdvg: call glUnmapBuffer()\n");
      Dsdvg_glcall(glUnmapBuffer(GL_ARRAY_BUFFER));
#endif // SHADERVG_USE_SCRATCHBUFFERSUBDATA
      Dsdvg_tracecall("[trc] sdvg:UnbindScratchBuffer scratch_vbo_id=%u\n", scratch_vbo_id);
      Dsdvg_glcall(glBindBuffer(GL_ARRAY_BUFFER, 0));
      current_vbo_id = 0u;
   }
}

void AllocScratchBuffer(sSI _aVertex, Dsdvg_buffer_ref_t _scratchBuf, sUI _numBytes) {
   Dsdvg_debugprintfv("[trc] sdvg:AllocScratchBuffer: io_offset=%u size=%u numBytes=%u\n", _scratchBuf->io_offset, _scratchBuf->size, _numBytes);
   if(_scratchBuf->io_offset + _numBytes > _scratchBuf->size)
   {
#ifndef SHADERVG_USE_SCRATCHBUFFERSUBDATA
      Dsdvg_glcall(glUnmapBuffer(GL_ARRAY_BUFFER));
#endif
      Dsdvg_glcall(glFinish());
      // map GPU buffer to virtual address and reset io_offset
#ifndef SHADERVG_USE_SCRATCHBUFFERSUBDATA
      loc_map_buffer(_scratchBuf, _scratchBuf->size);
#endif // SHADERVG_USE_SCRATCHBUFFERSUBDATA
      _scratchBuf->io_offset = 0u;  // (todo) already done by zglMapBuffer()
   }
   Dsdvg_glcall(zglVertexAttribOffset(_aVertex, 2/*size*/, GL_FLOAT, GL_FALSE/*normalize*/, 0/*stride*/, _scratchBuf->io_offset));
#ifdef SHADERVG_USE_SCRATCHBUFFERSUBDATA
   current_draw_start_offset = _scratchBuf->io_offset;
#endif // SHADERVG_USE_SCRATCHBUFFERSUBDATA
}

sUI YAC_CALL sdvg_CreateFBOEx(sUI _w, sUI _h, sBool _bMSAA, sBool _bDepth, sBool _bStencil) {
   sUI fboIdx = 1u;
   for(; fboIdx < SHADERVG_MAX_FBOS - 1u; )
   {
      ShaderVG_FBO *fbo = &fbos[fboIdx];
      if(0u == fbo->fbo_id)
      {
         fbo->sx = _w;
         fbo->sy = _h;

         fbo->tex_id = Dsdvg_glcall(zglGenTexture());
         Dsdvg_glcall(glActiveTexture(GL_TEXTURE0));
         Dsdvg_glcall(glBindTexture(GL_TEXTURE_2D, fbo->tex_id));

         Dsdvg_glcall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
                                   _w, _h,
                                   0, GL_BGRA, GL_UNSIGNED_BYTE, NULL
                                   )
                      );

         // Create framebuffer and attach texture (renderbuffer) to it
         fbo->fbo_id = Dsdvg_glcall(zglGenFramebuffer());
         Dsdvg_glcall(glBindFramebuffer(GL_FRAMEBUFFER, fbo->fbo_id));
         Dsdvg_glcall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo->tex_id, 0));

         if(_bMSAA)
         {
            fbo->fbo_msaa_id = Dsdvg_glcall(zglGenFramebuffer());
            Dsdvg_glcall(glBindFramebuffer(GL_FRAMEBUFFER, fbo->fbo_msaa_id));

            sUI msaaRb = Dsdvg_glcall(zglGenRenderbuffer());
            Dsdvg_glcall(glBindRenderbuffer(GL_RENDERBUFFER, msaaRb));
            Dsdvg_glcall(glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGBA8, _w, _h));
            Dsdvg_glcall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, msaaRb));
            Dsdvg_glcall(glBindRenderbuffer(GL_RENDERBUFFER, 0));

            if(_bDepth)
            {
               sUI msaaDepthRb = Dsdvg_glcall(zglGenRenderbuffer());
               Dsdvg_glcall(glBindRenderbuffer(GL_RENDERBUFFER, msaaDepthRb));

               if(_bStencil)
               {
                  // not available in OpenGL 4.1 core (e.g. macOS)
                  Dsdvg_glcall(glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, _w, _h));
                  Dsdvg_glcall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, msaaDepthRb));
               }
               else
               {
                  Dsdvg_glcall(glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT24, _w, _h));
                  Dsdvg_glcall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,  GL_RENDERBUFFER, msaaDepthRb));
               }

               Dsdvg_glcall(glBindRenderbuffer(GL_RENDERBUFFER, 0));
            }
            else if(_bStencil)
            {
               // not available in OpenGL 4.1 core (macOS)
               sUI msaaStencilRb = Dsdvg_glcall(zglGenRenderbuffer());
               Dsdvg_glcall(glBindRenderbuffer(GL_RENDERBUFFER, msaaStencilRb));

               Dsdvg_glcall(glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_STENCIL_INDEX8, _w, _h));
               Dsdvg_glcall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, msaaStencilRb));

               Dsdvg_glcall(glBindRenderbuffer(GL_RENDERBUFFER, 0));
            }
         }
         else
         {
            if(_bDepth/* || _bStencil*/)
            {
               sUI depthRb = Dsdvg_glcall(zglGenRenderbuffer());
               Dsdvg_glcall(glBindRenderbuffer(GL_RENDERBUFFER, depthRb));

               if(_bStencil)
               {
                  // not available in OpenGL 4.1 core (macOS)
                  Dsdvg_glcall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _w, _h));
                  Dsdvg_glcall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthRb));
               }
               else
               {
                  Dsdvg_glcall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, _w, _h));
                  Dsdvg_glcall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRb));
               }

               Dsdvg_glcall(glBindRenderbuffer(GL_RENDERBUFFER, 0));
            }
            else if(_bStencil)
            {
               // not available in OpenGL 4.1 core (macOS)
               sUI stencilRb = Dsdvg_glcall(zglGenRenderbuffer());
               Dsdvg_glcall(glBindRenderbuffer(GL_RENDERBUFFER, stencilRb));

               Dsdvg_glcall(glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, _w, _h));
               Dsdvg_glcall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, stencilRb));

               Dsdvg_glcall(glBindRenderbuffer(GL_RENDERBUFFER, 0));
            }
         }

         Dsdvg_glcall(glBindFramebuffer(GL_FRAMEBUFFER, 0));

         return fboIdx;
      }

      // Next FBO
      fboIdx++;
   }
   // Failed
   return 0u;
}

sUI YAC_CALL sdvg_CreateFBO(sUI _w, sUI _h) {
   return sdvg_CreateFBOEx(_w, _h, YAC_FALSE/*bMSAA*/, YAC_FALSE/*bDepth*/, YAC_TRUE/*bStencil*/);
}

void YAC_CALL sdvg_BindFBO(sUI _fboIdx) {
   if(_fboIdx < SHADERVG_MAX_FBOS)
   {
      ShaderVG_FBO *fbo = &fbos[_fboIdx];
      Dsdvg_glcall(glBindFramebuffer(GL_FRAMEBUFFER, (0 != fbo->fbo_msaa_id) ? fbo->fbo_msaa_id : fbo->fbo_id));
   }
   else
   {
      Dsdvg_glcall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
   }
}

void YAC_CALL sdvg_ResolveFBO(sUI _fboIdx) {
   if(_fboIdx < SHADERVG_MAX_FBOS)
   {
      ShaderVG_FBO *fbo = &fbos[_fboIdx];
      if(0u != fbo->fbo_msaa_id)
      {
         Dsdvg_glcall(glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo->fbo_msaa_id));
         Dsdvg_glcall(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo->fbo_id));
         Dsdvg_glcall(glBlitFramebuffer(0, 0, fbo->sx, fbo->sy,
                                        0, 0, fbo->sx, fbo->sy,
                                        GL_COLOR_BUFFER_BIT, GL_NEAREST
                                        )
                      );
         // Re-bind backbuffer
         Dsdvg_glcall(glBindFramebuffer(GL_READ_FRAMEBUFFER, 0));
         Dsdvg_glcall(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
      }
   }
   else
   {
      Dsdvg_glcall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
   }
}

void YAC_CALL sdvg_UnbindFBO(void) {
   Dsdvg_glcall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

sUI YAC_CALL sdvg_GetFBOTextureId(sUI _fboIdx) {
   if(_fboIdx < SHADERVG_MAX_FBOS)
   {
      ShaderVG_FBO *fbo = &fbos[_fboIdx];
      return fbo->tex_id;
   }
   return 0u;
}

void YAC_CALL sdvg_BindFBOTexture(sUI _fboIdx, sBool _bRepeat, sBool _bFilter) {
   sUI texId = sdvg_GetFBOTextureId(_fboIdx);
   sdvg_BindTexture2D(texId, _bRepeat, _bFilter);
}

void YAC_CALL sdvg_DestroyFBO(sUI _fboIdx) {
   if(_fboIdx < SHADERVG_MAX_FBOS)
   {
      ShaderVG_FBO *fbo = &fbos[_fboIdx];
      if(0u != fbo->fbo_msaa_id)
      {
         Dsdvg_glcall(zglDeleteFramebuffer(fbo->fbo_msaa_id));
         fbo->fbo_msaa_id = 0u;
      }
      if(0u != fbo->fbo_id)
      {
         Dsdvg_glcall(zglDeleteFramebuffer(fbo->fbo_id));
         fbo->fbo_id = 0u;
      }
      if(0u != fbo->tex_id)
      {
         Dsdvg_glcall(zglDeleteTexture(fbo->tex_id));
         fbo->tex_id = 0u;
      }
   }
}

   // OpenGL Version 	GLSL Version
   //            2.0    110
   //            2.1    120           #version 120
   //            3.0    130           #version 130
   //            3.1    140
   //            3.2    150           #version 150 compatibility
   //            3.3    330
   //            4.0    400
   //            4.1    410
   //            4.2    420
   //            4.3    430
void FixShaderSourceVert(YAC_String *_s, YAC_String *_r) {
   YAC_String t;
   t.copy(s_glsl_version);
   if(sdvg_b_glcore)
      t.append("precision mediump float; \n");
   t.append(" \n\n");
   t.append(_s);
   _r->yacCopy(&t);

   // (todo) use simplified overwrite-replace
   YAC_String k;
   k.visit("ATTRIBUTE");     yac_host->yacStringReplace(_r, &k, s_glsl_attribute);
   k.visit("VARYING_OUT");   yac_host->yacStringReplace(_r, &k, s_glsl_varying_out);
   k.visit("TEXTURE2D");     yac_host->yacStringReplace(_r, &k, s_glsl_texture2d);
   k.visit("TEXTURE2DPROJ"); yac_host->yacStringReplace(_r, &k, s_glsl_texture2dproj);
   k.visit("TEXTURE3D");     yac_host->yacStringReplace(_r, &k, s_glsl_texture3d);
   k.visit("TEXTURECUBE");   yac_host->yacStringReplace(_r, &k, s_glsl_texturecube);
   k.visit("TEX_ALPHA");     yac_host->yacStringReplace(_r, &k, s_glsl_tex_alpha);
}

void FixShaderSourceFrag(YAC_String *_s, YAC_String *_r) {
   YAC_String t;
   t.copy(s_glsl_version);
   if(sdvg_b_glcore)
      t.append("precision mediump float; \n");
   t.append(" \n\n");
   t.append(s_glsl_fragcolor_def);
   t.append(" \n");
   t.append(_s);
   _r->yacCopy(&t);

   // (todo) use simplified overwrite-replace
   YAC_String k;
   k.visit("VARYING_IN");    yac_host->yacStringReplace(_r, &k, s_glsl_varying_in);
   // // k.visit("FRAGCOLOR_DEF"); yac_host->yacStringReplace(_r, &k, s_glsl_fragcolor_def);
   k.visit("FRAGCOLOR");     yac_host->yacStringReplace(_r, &k, s_glsl_fragcolor);
   k.visit("TEXTURE2D");     yac_host->yacStringReplace(_r, &k, s_glsl_texture2d);
   k.visit("TEXTURE3D");     yac_host->yacStringReplace(_r, &k, s_glsl_texture3d);
   k.visit("TEX_ALPHA");     yac_host->yacStringReplace(_r, &k, s_glsl_tex_alpha);
}

static sBool loc_CreateFillShader() {
   sBool ret = YAC_FALSE;
   Dsdvg_debugprintfvv("[trc] sdvg:loc_CreateFillShader: ENTER\n");

   if(fill_shader.create(fill_vs_src, fill_fs_src))
   {
      fill_a_vertex = fill_shader.getAttribLocation("a_vertex");

      fill_u_transform = fill_shader.getUniformLocation("u_transform");
      fill_u_color     = fill_shader.getUniformLocation("u_color");

      // Succeeded
      ret = YAC_TRUE;
   }
   else
   {
      Dsdvg_errorprintf("[---] shadervg:loc_CreateFillShader: fill_shader.create() failed\n");
   }

   Dsdvg_debugprintfvv("[trc] loc_CreateFillShader: LEAVE ret=%d\n", ret);
   return ret;
}

void UniformMatrix4(sSI _location, Dsdvg_mat4_ref_t _o) {
   // 'o' is row-major matrix object (e.g. Matrix4f or FloatArray)
   if(NULL != _o)
   {
      if( (YAC_TYPE_FLOAT == _o->yacArrayGetElementType()) &&
          (_o->yacArrayGetNumElements() >= 16)
          )
      {
         const GLfloat *fa = (const GLfloat *)_o->yacArrayGetPointer();
         if(NULL != fa)
         {
#if 0
            Dsdvg_debugprintfvv("[trc] sdvg:UniformMatrix4: fa={%f;%f;%f;%f  %f;%f;%f;%f  %f;%f;%f;%f  %f;%f;%f;%f}\n", fa[0], fa[1], fa[2], fa[3], fa[4], fa[5], fa[6], fa[7], fa[8], fa[9], fa[10], fa[11], fa[12], fa[13], fa[14], fa[15]);
#endif
            Dsdvg_glcall(glUniformMatrix4fv(_location, 1/*count*/, GL_TRUE/*transpose*/, fa));
         }
         else
         {
            Dsdvg_errorprintf("[---] shadervg:UniformMatrix4: yacArrayGetPointer() returned NULL !!\n");
         }
      }
      else
      {
         Dsdvg_errorprintf("[---] shadervg:UniformMatrix4: wrong matrix element type or num_elements !!\n");
      }
   }
   else
   {
      Dsdvg_errorprintf("[---] shadervg:UniformMatrix4: matrix is NULL !!\n");
   }
}

sSI BindFillShader(void) {
   // returns vertex attribute id (for allocScratchBuffer() calls)
   fill_shader.bind();

   if(NULL != mvp_matrix)
   {
      Dsdvg_uniform_mat4(fill_u_transform, mvp_matrix);
      Dsdvg_uniform_4f(fill_u_color, fill_r, fill_g, fill_b, fill_a * global_a);
      Dsdvg_attrib_enable(fill_a_vertex);
   }
   else
   {
      Dsdvg_errorprintf("[---] BindFillShader: mvp_matrix is NULL !!\n");
   }

   return fill_a_vertex;
}

void EndFillShader(void) {
   Dsdvg_attrib_disable(fill_a_vertex);
}

void UnbindFillShader(void) {
   EndFillShader();
   fill_shader.unbind();
}

sBool YAC_CALL sdvg_OnOpen(void) {
   Dsdvg_tracecall("[trc] sdvg_OnOpen\n");

   if(Dyac_host_yacGetDebugLevel() >= 1u)
   {
      Dsdvg_debugprintf("[dbg] sdvg_OnOpen: ENTER\n");
   }

   Dsdvg_glcall(zglLoadExtensions());

   if(sdvg_b_glcore)
      vao_id = Dsdvg_glcall(zglGenVertexArray());

   loc_CreateScratchBuffer();

   if(Dyac_host_yacGetDebugLevel() >= 1u)
   {
      Dsdvg_debugprintfv("[dbg] sdvg_OnOpen 2\n");
   }

   if(!loc_CreateFillShader())
   {
      Dsdvg_errorprintf("[---] sdvg_OnOpen: CreateFillShader() failed\n");
      return YAC_FALSE;
   }

   if(Dyac_host_yacGetDebugLevel() >= 1u)
   {
      Dsdvg_debugprintfv("[dbg] sdvg_OnOpen 3\n");
   }

   for(sUI i = 0u; i < SHADERVG_NUM_SHAPES; i++)
   {
      ShaderVG_Shape *shape = all_shapes[i];
      if(!shape->onOpen())
      {
         Dsdvg_errorprintf("[---] sdvg_OnOpen: shape[%u].onOpen() failed\n", i);
         return YAC_FALSE;
      }
   }

   // unload / destroy custom shaders (app must re-create them)
   for(sUI shaderIdx = 0u; shaderIdx < SHADERVG_MAX_CUSTOM_SHADERS; shaderIdx++)
   {
      ShaderVG_CustomShape *cs = &custom_shapes[shaderIdx];
      cs->onOpen();
   }
   current_shape = NULL;

   current_vbo_id = 0u;

   ::memset((void*)fbos, 0, sizeof(fbos));

   if(Dyac_host_yacGetDebugLevel() >= 1u)
   {
      Dsdvg_debugprintf("[dbg] sdvg_OnOpen: LEAVE\n");
   }

   return YAC_TRUE;
}

void YAC_CALL sdvg_SetFramebufferSize(sUI _w, sUI _h) {
   fb_w = _w;
   fb_h = _h;
}

void YAC_CALL sdvg_SetViewport(sUI _x, sUI _y, sUI _w, sUI _h) {

   // (note) (0;0) = left/top  =>  (0;0) = left/bottom (GL)
   viewport_x = sSI(_x);
   viewport_y = sSI(fb_h) - sSI(_y + _h);
   viewport_w = sSI(_w);
   viewport_h = sSI(_h);

   Dsdvg_tracecallv("[trc] sdvg_SetViewport: viewport_x=%d y=%d w=%d h=%d\n", viewport_x, viewport_y, viewport_w, viewport_h);

   Dsdvg_glcall(glViewport(viewport_x, viewport_y,
                           viewport_w, viewport_h
                           )
                );
}

void YAC_CALL sdvg_PushViewport(sUI _x, sUI _y, sUI _w, sUI _h) {
   if(viewport_stacki >= 4)
   {
      viewport_stack[--viewport_stacki] = viewport_h;
      viewport_stack[--viewport_stacki] = viewport_w;
      viewport_stack[--viewport_stacki] = viewport_y;
      viewport_stack[--viewport_stacki] = viewport_x;

      // (note) (0;0) = left/top  =>  (0;0) = left/bottom (GL)
      viewport_x = sSI(_x);
      viewport_y = sSI(fb_h) - sSI(_y + _h);
      viewport_w = sSI(_w);
      viewport_h = sSI(_h);

      Dsdvg_tracecallv("[trc] sdvg_PushViewport: viewport_x=%d y=%d w=%d h=%d\n", viewport_x, viewport_y, viewport_w, viewport_h);

      Dsdvg_glcall(glViewport(viewport_x, viewport_y,
                              viewport_w, viewport_h
                              )
                   );
   }
   else
   {
      Dsdvg_errorprintf("[---] sdvg_PushViewport: stack overflow");
   }
}

void YAC_CALL sdvg_PopViewport(void) {
   if(viewport_stacki <= ((SHADERVG_VIEWPORT_STACK_SIZE-1)*4))
   {
      viewport_x = viewport_stack[viewport_stacki++];
      viewport_y = viewport_stack[viewport_stacki++];
      viewport_w = viewport_stack[viewport_stacki++];
      viewport_h = viewport_stack[viewport_stacki++];

      Dsdvg_tracecallv("[trc] sdvg_PopViewport: viewport_x=%d y=%d w=%d h=%d\n", viewport_x, viewport_y, viewport_w, viewport_h);

      Dsdvg_glcall(glViewport(viewport_x, viewport_y,
                              viewport_w, viewport_h
                              )
                   );
   }
   else
   {
      Dsdvg_errorprintf("[---] sdvg_PopViewport: stack underflow");
   }
}

sUI YAC_CALL sdvg_GetViewportWidth(void) {
   return sUI(viewport_w);
}

sUI YAC_CALL sdvg_GetViewportHeight(void) {
   return sUI(viewport_h);
}

void YAC_CALL sdvg_SetScissor(sUI _x, sUI _y, sUI _w, sUI _h) {

   // (note) (0;0) = left/top  =>  (0;0) = left/bottom (GL)
   scissor_x = sSI(_x);
   scissor_y = sSI(fb_h) - sSI(_y + _h);
   scissor_w = sSI(_w);
   scissor_h = sSI(_h);

   Dsdvg_tracecallv("[trc] sdvg_SetScissor: scissor_x=%d y=%d w=%d h=%d\n", scissor_x, scissor_y, scissor_w, scissor_h);

   Dsdvg_glcall(glScissor(scissor_x, scissor_y,
                          scissor_w, scissor_h
                          )
                );
}

void YAC_CALL sdvg_PushScissor(sUI _x, sUI _y, sUI _w, sUI _h) {
   if(scissor_stacki >= 4)
   {
      scissor_stack[--scissor_stacki] = scissor_h;
      scissor_stack[--scissor_stacki] = scissor_w;
      scissor_stack[--scissor_stacki] = scissor_y;
      scissor_stack[--scissor_stacki] = scissor_x;

      // (note) (0;0) = left/top  =>  (0;0) = left/bottom (GL)
      sSI x = sSI(_x);
      sSI y = sSI(fb_h) - sSI(_y + _h);
      sSI w = sSI(_w);
      sSI h = sSI(_h);

      if(x < 0)
      {
         w += x;
         x = 0;
      }

      if(y < 0)
      {
         h += y;
         y = 0;
      }

      if(scissor_w > 0)
      {
         if(x < scissor_x)
            w -= (scissor_x - x);

         if(y < scissor_y)
            h -= (scissor_y - y);

         // intersect with last scissor region
         if(x > scissor_x)
         {
            scissor_w -= (x - scissor_x);
            scissor_x = x;
         }

         if(y > scissor_y)
         {
            scissor_h -= (y - scissor_y);
            scissor_y = y;
         }

         if(w < scissor_w)
            scissor_w = w;

         else if(scissor_w < 0)
            scissor_w = 0;

         if(h < scissor_h)
            scissor_h = h;
         else if(scissor_h < 0)
            scissor_h = 0;
      }
      else
      {
         scissor_x = x;
         scissor_y = y;
         scissor_w = w;
         scissor_h = h;
      }

      if(scissor_w < 0)
         scissor_w = 0;

      if(scissor_h < 0)
         scissor_h = 0;

      Dsdvg_tracecallv("[trc] sdvg_PushScissor: scissor_x=%d y=%d w=%d h=%d\n", scissor_x, scissor_y, scissor_w, scissor_h);
      Dsdvg_glcall(glScissor(scissor_x, scissor_y,
                             scissor_w, scissor_h
                             )
                   );
   }
   else
   {
      Dsdvg_errorprintf("[---] sdvg_PushScissor: stack overflow");
   }
}

void YAC_CALL sdvg_PopScissor(void) {
   if(scissor_stacki <= ((SHADERVG_SCISSOR_STACK_SIZE-1)*4))
   {
      scissor_x = scissor_stack[scissor_stacki++];
      scissor_y = scissor_stack[scissor_stacki++];
      scissor_w = scissor_stack[scissor_stacki++];
      scissor_h = scissor_stack[scissor_stacki++];

      Dsdvg_tracecallv("[trc] sdvg_PopScissor: scissor_x=%d y=%d w=%d h=%d\n", scissor_x, scissor_y, scissor_w, scissor_h);
      Dsdvg_glcall(glScissor(scissor_x, scissor_y,
                             scissor_w, scissor_h
                             )
                   );
   }
   else
   {
      Dsdvg_errorprintf("[---] sdvg_PopScissor: stack underflow");
   }
}

void YAC_CALL sdvg_EnableScissor(void) {
   Dsdvg_glcall(glEnable(GL_SCISSOR_TEST));
}

void YAC_CALL sdvg_DisableScissor(void) {
   Dsdvg_glcall(glDisable(GL_SCISSOR_TEST));
}

void YAC_CALL sdvg_BeginFrame(void) {

   Dsdvg_tracecall("[trc] sdvg_BeginFrame ~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

   viewport_stacki = SHADERVG_VIEWPORT_STACK_SIZE * 4;
   viewport_x = 0;
   viewport_y = 0;
   viewport_w = fb_w;
   viewport_h = fb_h;
   Dsdvg_glcall(glViewport(viewport_x, viewport_y,
                           viewport_w, viewport_h
                           )
                );

   scissor_stacki = SHADERVG_SCISSOR_STACK_SIZE*4;
   scissor_x = 0;
   scissor_y = 0;
   scissor_w = fb_w;
   scissor_h = fb_h;
   Dsdvg_glcall(glScissor(scissor_x, scissor_y,
                          scissor_w, scissor_h
                          )
                );
   Dsdvg_glcall(glEnable(GL_SCISSOR_TEST));

   Dsdvg_glcall(glDisable(GL_DEPTH_TEST));
   Dsdvg_glcall(glCullFace(GL_BACK));
   Dsdvg_glcall(glFrontFace(GL_CW));
   Dsdvg_glcall(glDisable(GL_CULL_FACE));

   // ShaderVG.SetProjection2D(minnie_setup.width, minnie_setup.height);
   // ShaderVG.SetAARangeAndExp(b_aa ? aa_range : 0.01f, aa_exp);
   // ShaderVG.SetStrokeWidth(1.5);
   // ShaderVG.SetFillColor4f(0.1, 0.4, 0.25, 1.0 * alpha);
   // ShaderVG.SetStrokeColor4f(1.0, 1.0, 1.0, 1.0 * alpha);
   sdvg_EnableBlending();

   sdvg_UnbindVBO();

   current_draw_mode = GL_NONE;

   num_draw_attrib_enables = 0u;

   sdvg_pixel_scl = 1.0f;

#ifdef SHADERVG_TEXT
   sdvg_int_reset_font();
#endif // SHADERVG_TEXT

   paint_mode = PAINT_SOLID;

   if(sdvg_b_glcore)
      Dsdvg_glcall(glBindVertexArray(vao_id));
}

void YAC_CALL sdvg_Flush(void) {
   Dsdvg_tracecall("[trc] sdvg_Flush\n");
   if(scratch_buffer->io_offset > 0u)
   {
      UnbindScratchBuffer();
      Dsdvg_glcall(glFinish());
   }
}

void YAC_CALL sdvg_ReturnToGL(void) {
   Dsdvg_tracecall("[trc] sdvg_ReturnToGL\n");
   if(0 != mapped_user_vbo_id)
      sdvg_UnmapVBO();
   // // Dsdvg_glcall(glUnmapBuffer(GL_ARRAY_BUFFER));
   // // Dsdvg_glcall(glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER));
   // sdvg_BindVBO(0);
   sdvg_UnbindVBO();
   // // Dsdvg_glcall(glUseProgram(0));
   sdvg_UnbindShader();
   if(sdvg_b_glcore)
      Dsdvg_glcall(glBindVertexArray(vao_id));
}

void YAC_CALL sdvg_EndFrame(void) {
   Dsdvg_tracecall("[trc] sdvg_EndFrame\n");
   sdvg_DisableBlending();
   if(0 != mapped_user_vbo_id)
      sdvg_UnmapVBO();
   sdvg_BindVBO(0);
   Dsdvg_glcall(glUseProgram(0));
   Dsdvg_glcall(glDisable(GL_SCISSOR_TEST));
}

void YAC_CALL sdvg_SetTransform(YAC_Object *_mat4) {
   mvp_matrix->yacOperatorAssign(_mat4);
}

YAC_Object *YAC_CALL sdvg_GetTransformRef(void) {
   return mvp_matrix;
}

void YAC_CALL sdvg_SetEnableAA(sBool _bEnable) {
   b_aa = _bEnable;
}

void YAC_CALL sdvg_SetAARange(sF32 _aaRange) {
   aa_range = _aaRange;
}

void YAC_CALL sdvg_SetAAExp(sF32 _aaExp) {
   // (todo) remove
   aa_exp   = _aaExp;
}

void YAC_CALL sdvg_SetAlphaSDFRange(sF32 _aMin, sF32 _aMax) {
   if(_aMax > _aMin)
   {
      alpha_sdf_min = _aMin;
      alpha_sdf_max = _aMax;
   }
   else
   {
      // default
      alpha_sdf_min = (128.0f - 26.0f) / 128.0f;
      alpha_sdf_max = (128.0f -  6.0f) / 128.0f;
   }
   alpha_sdf_maxmin_scale = 1.0f / (alpha_sdf_max - alpha_sdf_min);
   Dsdvg_tracecallv("[trc] sdvg_SetAlphaSDFRange: alpha_sdf min=%f max=%f scale=%f\n", alpha_sdf_min, alpha_sdf_max, alpha_sdf_maxmin_scale);
}

void YAC_CALL sdvg_SetAlphaSDFExp(sF32 _aExp) {
   alpha_sdf_exp = _aExp;
}

void YAC_CALL sdvg_SetFillColor4f(sF32 _fillR, sF32 _fillG, sF32 _fillB, sF32 _fillA) {
   fill_r = _fillR;
   fill_g = _fillG;
   fill_b = _fillB;
   fill_a = _fillA;
}

void YAC_CALL sdvg_SetFillAlpha(sF32 _fillA) {
   fill_a = _fillA;
}

void YAC_CALL sdvg_SetFillColorARGB(sUI _c32) {
   fill_r = ((_c32 >> 16) & 255u) * (1.0f / 255.0f);
   fill_g = ((_c32 >>  8) & 255u) * (1.0f / 255.0f);
   fill_b = ((_c32      ) & 255u) * (1.0f / 255.0f);
   fill_a = ((_c32 >> 24) & 255u) * (1.0f / 255.0f);
}

void YAC_CALL sdvg_SetColorARGB(sUI _c32) {
   sdvg_SetFillColorARGB(_c32);
   sdvg_SetStrokeColorARGB(_c32);
}

void YAC_CALL sdvg_SetStrokeWidth(sF32 _strokeW) {
   stroke_w = _strokeW;
}

void YAC_CALL sdvg_SetPointRadius(sF32 _radius) {
   point_radius = _radius;
}

void YAC_CALL sdvg_SetPixelScaling(sF32 _s) {
   sdvg_pixel_scl = _s;
}

void YAC_CALL sdvg_SetStrokeColor4f(sF32 _strokeR, sF32 _strokeG, sF32 _strokeB, sF32 _strokeA) {
   stroke_r = _strokeR;
   stroke_g = _strokeG;
   stroke_b = _strokeB;
   stroke_a = _strokeA;
   Dsdvg_tracecall("[trc] sdvg_SetStrokeColor4f: stroke_a=%f\n", stroke_a);
}

void YAC_CALL sdvg_SetStrokeColorARGB(sU32 _c32) {
   stroke_r = ((_c32 >> 16) & 255u) * (1.0f / 255.0f);
   stroke_g = ((_c32 >>  8) & 255u) * (1.0f / 255.0f);
   stroke_b = ((_c32      ) & 255u) * (1.0f / 255.0f);
   stroke_a = ((_c32 >> 24) & 255u) * (1.0f / 255.0f);
   Dsdvg_tracecall("[trc] sdvg_SetStrokeColorARGB: stroke_a=%f\n", stroke_a);
}

void YAC_CALL sdvg_SetGlobalAlpha(sF32 _a) {
   global_a = _a;
}

void YAC_CALL sdvg_SetTextureDecalAlpha(sF32 _decalAlpha) {
   texture_decal_alpha = _decalAlpha;
}

void YAC_CALL sdvg_Clear4f(sF32 _r, sF32 _g, sF32 _b, sF32 _a) {
   Dsdvg_glcall(glClearColor(_r, _g, _b, _a));
   Dsdvg_glcall(glClearStencil(0));
   Dsdvg_glcall(glClearDepth(0.0f));
   Dsdvg_glcall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
}

void YAC_CALL sdvg_ClearARGB(sUI _c32) {
   sF32 r = ((_c32 >> 16) & 255) * (1.0f / 255.0f);
   sF32 g = ((_c32 >>  8) & 255) * (1.0f / 255.0f);
   sF32 b = ((_c32      ) & 255) * (1.0f / 255.0f);
   sF32 a = ((_c32 >> 24) & 255) * (1.0f / 255.0f);
   sdvg_Clear4f(r, g, b, a);
}

void YAC_CALL sdvg_EnableBlending(void) {
   Dsdvg_glcall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
   Dsdvg_glcall(glEnable(GL_BLEND));
}

void YAC_CALL sdvg_EnableBlendingKeepAlpha(void) {
   Dsdvg_glcall(glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
                                    GL_ZERO,      GL_ONE
                                    )
                );
   Dsdvg_glcall(glEnable(GL_BLEND));
}

void YAC_CALL sdvg_EnableBlendingAdditive(void) {
   Dsdvg_glcall(glBlendFunc(GL_SRC_ALPHA, GL_ONE));
   Dsdvg_glcall(glEnable(GL_BLEND));
}

void YAC_CALL sdvg_EnableBlendingAdditiveKeepAlpha(void) {
   Dsdvg_glcall(glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE,
                                    GL_ZERO, GL_ONE
                                    )
                );
   Dsdvg_glcall(glEnable(GL_BLEND));
}

void YAC_CALL sdvg_EnableBlendingSrcColorKeepAlpha(void) {
   Dsdvg_glcall(glBlendFuncSeparate(GL_SRC_COLOR,  GL_ONE_MINUS_SRC_ALPHA,
                                    GL_ZERO, GL_ONE
                                    )
                );
   Dsdvg_glcall(glEnable(GL_BLEND));
}

void YAC_CALL sdvg_EnableBlendingDstColorKeepAlpha(void) {
   Dsdvg_glcall(glBlendFuncSeparate(GL_SRC_ALPHA,  GL_DST_COLOR,
                                    GL_ZERO, GL_ONE
                                    )
                );
   Dsdvg_glcall(glEnable(GL_BLEND));
}

void YAC_CALL sdvg_DisableBlending(void) {
   Dsdvg_glcall(glDisable(GL_BLEND));
}

void YAC_CALL sdvg_AlphaWrite(sBool _bEnable) {
   Dsdvg_glcall(glColorMask(1,1,1, _bEnable));
}

// ----------- custom shaders ------------
sUI YAC_CALL sdvg_CreateShader(const char *vs, const char *fs) {
   sUI shaderIdx = 1u;
   while(shaderIdx < SHADERVG_MAX_CUSTOM_SHADERS - 1u)
   {
      ShaderVG_CustomShape *cs = &custom_shapes[shaderIdx];
      ShaderVG_Shader *shapeShader = &cs->shape_shader;
      if(0u == shapeShader->prg_id)
      {
         if(cs->createShapeShader(vs, fs))
         {
            return shaderIdx;
         }
         else
         {
            Dsdvg_errorprintf("[---] sdvg_CreateShader: createShapeShader() failed\n");
            return 0u;
         }
      }

      // Next shader
      shaderIdx++;
   }

   // Failed
   Dsdvg_errorprintf("[---] sdvg_CreateShader: failed to allocate shader (MAX_CUSTOM_SHADERS=%u exceeded)\n", SHADERVG_MAX_CUSTOM_SHADERS);
   return 0u;
}

#ifdef SHADERVG_SCRIPT_API
sUI YAC_CALL _sdvg_CreateShader(YAC_String *_vs, YAC_String *_fs) {
   if(YAC_Is_String(_vs) && YAC_Is_String(_fs))
   {
      YAC_CAST_STRING(vs, _vs);
      YAC_CAST_STRING(fs, _fs);
      return sdvg_CreateShader((const char*)vs->chars, (const char*)fs->chars);
   }
   return 0u;
}
#endif // SHADERVG_SCRIPT_API

void YAC_CALL sdvg_DestroyShader(sUI _shaderIdx) {
   if(_shaderIdx < SHADERVG_MAX_CUSTOM_SHADERS)
   {
      ShaderVG_CustomShape *cs = &custom_shapes[_shaderIdx];
      ShaderVG_Shader *shapeShader = &cs->shape_shader;
      shapeShader->destroy();
      if(cs == current_shape)
         current_shape = NULL;
   }
}

void BindShape(ShaderVG_Shape *_shape) {
   ShaderVG_Shader *shapeShader = &_shape->shape_shader;
   shapeShader->bind();
   current_shape = _shape;
}

void UnsetShapeIfBuiltIn(void) {
   // (note) do _not_ actually unbind it in GL
   if(NULL != current_shape && current_shape->b_builtin)
      current_shape = NULL;
}

void YAC_CALL sdvg_BindShader(sUI _shaderIdx) {
   Dsdvg_tracecall("[trc] sdvg_BindShader(shaderIdx=%u)\n", _shaderIdx);
   if(_shaderIdx >= 1u && _shaderIdx < SHADERVG_MAX_CUSTOM_SHADERS)
   {
      ShaderVG_CustomShape *cs = &custom_shapes[_shaderIdx];
      BindShape(cs);
   }
   else
   {
      // (note) do NOT unbind from GL
      current_shape = NULL;
   }
}

void YAC_CALL sdvg_UnbindShader(void) {
   Dsdvg_glcall(glUseProgram(0));
   current_shape = NULL;
}

sSI YAC_CALL sdvg_GetUniformLocation(const char *_name) {
   sSI loc = -1;
   if(NULL != current_shape)
   {
      ShaderVG_Shader *shapeShader = &current_shape->shape_shader;
      loc = Dsdvg_glcall(glGetUniformLocation(shapeShader->prg_id, _name));
   }
   return loc;
}

sSI YAC_CALL sdvg_GetAttribLocation(const char *_name) {
   sSI a = -1;
   if(NULL != current_shape)
   {
      ShaderVG_Shader *shapeShader = &current_shape->shape_shader;
      a = Dsdvg_glcall(glGetAttribLocation(shapeShader->prg_id, _name));
   }
   return a;
}

void YAC_CALL sdvg_Uniform1f(const char *_name, sF32 _f) {
   if(NULL != current_shape)
   {
      ShaderVG_Shader *shapeShader = &current_shape->shape_shader;
      sSI loc = shapeShader->getUniformLocation(_name);
      if(loc >= 0)
      {
         Dsdvg_uniform_1f(loc, _f);
      }
   }
}

void YAC_CALL sdvg_Uniform2f(const char *_name, sF32 _f1, sF32 _f2) {
   if(NULL != current_shape)
   {
      ShaderVG_Shader *shapeShader = &current_shape->shape_shader;
      sSI loc = shapeShader->getUniformLocation(_name);
      if(loc >= 0)
      {
         Dsdvg_uniform_2f(loc, _f1, _f2);
      }
   }
}

void YAC_CALL sdvg_Uniform3f(const char *_name, sF32 _f1, sF32 _f2, sF32 _f3) {
   if(NULL != current_shape)
   {
      ShaderVG_Shader *shapeShader = &current_shape->shape_shader;
      sSI loc = shapeShader->getUniformLocation(_name);
      if(loc >= 0)
      {
         Dsdvg_uniform_3f(loc, _f1, _f2, _f3);
      }
   }
}

void YAC_CALL sdvg_Uniform4f(const char *_name, sF32 _f1, sF32 _f2, sF32 _f3, sF32 _f4) {
   if(NULL != current_shape)
   {
      ShaderVG_Shader *shapeShader = &current_shape->shape_shader;
      sSI loc = shapeShader->getUniformLocation(_name);
      if(loc >= 0)
      {
         Dsdvg_uniform_4f(loc, _f1, _f2, _f3, _f4);
      }
   }
}

void YAC_CALL sdvg_Uniform1i(const char *_name, sSI _i) {
   if(NULL != current_shape)
   {
      ShaderVG_Shader *shapeShader = &current_shape->shape_shader;
      sSI loc = shapeShader->getUniformLocation(_name);
      if(loc >= 0)
      {
         Dsdvg_uniform_1i(loc, _i);
      }
   }
}

#ifdef SHADERVG_SCRIPT_API
sSI YAC_CALL _sdvg_GetUniformLocation(YAC_String *_name) {
   if(YAC_Is_String(_name))
   {
      YAC_CAST_STRING(name, _name);
      return sdvg_GetUniformLocation((const char*)name->chars);
   }
   return -1;
}

sSI YAC_CALL _sdvg_GetAttribLocation(YAC_String *_name) {
   if(YAC_Is_String(_name))
   {
      YAC_CAST_STRING(name, _name);
      return sdvg_GetAttribLocation((const char*)name->chars);
   }
   return -1;
}

void YAC_CALL _sdvg_UniformMatrix4(YAC_String *_name, YAC_Object *_matRowMajor) {
   if(NULL != current_shape)
   {
      ShaderVG_Shader *shapeShader = &current_shape->shape_shader;

      if(YAC_Is_String(_name))
      {
         YAC_CAST_STRING(name, _name);

         if(YAC_VALID(_matRowMajor))
         {
            sSI a = Dsdvg_glcall(glGetUniformLocation(shapeShader->prg_id, (const char*)name->chars));
            if(a >= 0)
            {
               UniformMatrix4(a, _matRowMajor);
            }
         }
         else
         {
            Dprintf("[---] sdvg_UniformMatrix4: 'matRowMajor' is not a valid Object\n");
         }
      }
      else
      {
         Dprintf("[---] sdvg_UniformMatrix4: 'name' is not a String\n");
      }
   }
   else
   {
      Dprintf("[---] sdvg_UniformMatrix4: current_shape is NULL\n");
   }
}

void YAC_CALL _sdvg_Uniform1f(YAC_String *_name, sF32 _f) {
   if(YAC_Is_String(_name))
   {
      YAC_CAST_STRING(name, _name);
      sdvg_Uniform1f((const char*)name->chars, _f);
   }
}

void YAC_CALL _sdvg_Uniform2f(YAC_String *_name, sF32 _f1, sF32 _f2) {
   if(YAC_Is_String(_name))
   {
      YAC_CAST_STRING(name, _name);
      sdvg_Uniform2f((const char*)name->chars, _f1, _f2);
   }
}

void YAC_CALL _sdvg_Uniform3f(YAC_String *_name, sF32 _f1, sF32 _f2, sF32 _f3) {
   if(YAC_Is_String(_name))
   {
      YAC_CAST_STRING(name, _name);
      sdvg_Uniform3f((const char*)name->chars, _f1, _f2, _f3);
   }
}

void YAC_CALL _sdvg_Uniform4f(YAC_String *_name, sF32 _f1, sF32 _f2, sF32 _f3, sF32 _f4) {
   if(YAC_Is_String(_name))
   {
      YAC_CAST_STRING(name, _name);
      sdvg_Uniform4f((const char*)name->chars, _f1, _f2, _f3, _f4);
   }
}

void YAC_CALL _sdvg_Uniform1i(YAC_String *_name, sSI _i) {
   if(YAC_Is_String(_name))
   {
      YAC_CAST_STRING(name, _name);
      sdvg_Uniform1i((const char*)name->chars, _i);
   }
}
#endif // SHADERVG_SCRIPT_API

static void loc_get_vbo_size(sUI &bufSize, sBool &bScratch) {
   if(current_vbo_id == scratch_vbo_id)
   {
      bufSize = scratch_buffer->size;
      bScratch = YAC_TRUE;
#if 0
      {
         sUI scrbufSize = Dsdvg_glcall(zglGetBufferParameter(GL_ARRAY_BUFFER, GL_BUFFER_SIZE));
         Dsdvg_debugprintvv("[trc] BeginDraw: scratch_vbo scrbufSize=%u\n", scrbufSize);
      }
#endif
   }
   else
   {
      // User VBO
      bufSize = Dsdvg_glcall(zglGetBufferParameter(GL_ARRAY_BUFFER, GL_BUFFER_SIZE));
   }
}

static sBool BeginDraw(sUI _numVertices, sUI _stride) {
   Dsdvg_tracecall("[trc] sdvg:BeginDraw: mapped_user_vbo_id=%u current_vbo_id=%u (scratch_vbo_id=%u)\n", mapped_user_vbo_id, current_vbo_id, scratch_vbo_id);

   if(0u == mapped_user_vbo_id && 0u == current_vbo_id)
      BindScratchBuffer();

   current_draw_num_vertices       = _numVertices;
   current_draw_stride             = _stride;
   current_draw_num_bytes          = _numVertices * _stride;
   current_draw_vertex_index       = 0u;
   current_draw_lines_vertex_index = 0u;

   sUI bufSize;
   sBool bScratch = YAC_FALSE;
   loc_get_vbo_size(bufSize, bScratch);

   if(current_draw_num_bytes <= bufSize)
   {
      if(bScratch)
      {
         Dsdvg_debugprintfv("[trc] sdvg:BeginDraw: scratch offset=%u size=%u #vert=%u stride=%u => numBytes=%u\n", scratch_buffer->io_offset, scratch_buffer->size, _numVertices, _stride, current_draw_num_bytes);
         if(scratch_buffer->io_offset + current_draw_num_bytes > scratch_buffer->size)
         {
#ifndef SHADERVG_USE_SCRATCHBUFFERSUBDATA
            Dsdvg_glcall(glUnmapBuffer(GL_ARRAY_BUFFER));
            Dsdvg_glcall(glFinish());
            loc_map_buffer(scratch_buffer, scratch_buffer->size);
#else
            Dsdvg_glcall(glFinish());
#endif // SHADERVG_USE_SCRATCHBUFFERSUBDATA
            scratch_buffer->io_offset = 0u;
         }
         current_draw_attrib_offset = scratch_buffer->io_offset;
      }
      else
      {
         // User VBO
         current_draw_attrib_offset = 0u;
         if(0u != mapped_user_vbo_id)
            user_vbo_buffer->io_offset = 0u;
         else
            current_draw_vertex_index = current_draw_num_vertices;
      }

      attrib_write_buffer = bScratch ? scratch_buffer : user_vbo_buffer;
      current_draw_start_offset = current_draw_attrib_offset;

      return YAC_TRUE;
   }
   else
   {
      Dsdvg_errorprintf("[---] shadervg:BeginDraw: bufSize exceeded (%u > %u)\n", current_draw_num_bytes, bufSize);
      return YAC_FALSE;
   }
}

void YAC_CALL sdvg_PaintSolid(void) {
   paint_mode = PAINT_SOLID;
}

void YAC_CALL sdvg_PaintLinear(sF32 _startX, sF32 _startY, sF32 _endX, sF32 _endY) {
   paint_mode = PAINT_LINEAR;
   paint_start_x = _startX;
   paint_start_y = _startY;
   paint_end_x = _endX;
   paint_end_y = _endY;
}

void YAC_CALL sdvg_PaintRadial(sF32 _startX, sF32 _startY, sF32 _radiusX, sF32 _radiusY) {
   paint_mode = PAINT_RADIAL;
   paint_start_x = _startX;
   paint_start_y = _startY;
   paint_end_x = _startX + _radiusX;
   paint_end_y = _startY + _radiusY;
}

void YAC_CALL sdvg_PaintConic(sF32 _startX, sF32 _startY, sF32 _radiusX, sF32 _radiusY, sF32 _angle01) {
   paint_mode = PAINT_CONIC;
   paint_start_x = _startX;
   paint_start_y = _startY;
   paint_end_x = _startX + _radiusX;
   paint_end_y = _startY + _radiusY;
   paint_angle = _angle01 + 0.25f/*north*/;
}

sBool YAC_CALL sdvg_BeginVBO(sUI _numVertices, sUI _stride) {
   // prepare-buffer mode (no rendering)
   current_draw_mode = GL_NONE;
   return BeginDraw(_numVertices, _stride);
}

sBool YAC_CALL sdvg_BeginTriangles(sUI _numVertices, sUI _stride) {
   current_draw_mode = GL_TRIANGLES;
   return BeginDraw(_numVertices, _stride);
}

sBool YAC_CALL sdvg_BeginTriangleFan(sUI _numVertices, sUI _stride) {
   current_draw_mode = GL_TRIANGLE_FAN;
   return BeginDraw(_numVertices, _stride);
}

sBool YAC_CALL sdvg_BeginTriangleStrip(sUI _numVertices, sUI _stride) {
   current_draw_mode = GL_TRIANGLE_STRIP;
   return BeginDraw(_numVertices, _stride);
}

static void loc_bind_default_triangles_fill_flat_shape(void) {
   ShaderVG_Shape *shape;
   switch(paint_mode)
   {
      default:
      case PAINT_SOLID:   shape = &triangles_fill_flat_32;        break;
      case PAINT_LINEAR:  shape = &triangles_fill_flat_32_linear; break;
      case PAINT_RADIAL:  shape = &triangles_fill_flat_32_radial; break;
      case PAINT_CONIC:   shape = &triangles_fill_flat_32_conic;  break;
   }
   BindShape(shape);
}

sBool YAC_CALL sdvg_BeginFilledTriangles(sUI _numVertices) {
   //
   // VBO vertex format (8 bytes per vertex):
   //     +0 f32 x
   //     +4 f32 y
   //

   if(NULL == current_shape)
      loc_bind_default_triangles_fill_flat_shape();

   if(sdvg_BeginTriangles(_numVertices, (2*4)/*stride*/))
   {
      sdvg_VertexOffset2f();
      return YAC_TRUE;
   }
   return YAC_FALSE;
}

sBool YAC_CALL sdvg_BeginFilledTriangleFan(sUI _numVertices) {
   //
   // VBO vertex format (8 bytes per vertex):
   //     +0 f32 x
   //     +4 f32 y
   //

   if(NULL == current_shape)
      loc_bind_default_triangles_fill_flat_shape();

   if(sdvg_BeginTriangleFan(_numVertices, (2*4)/*stride*/))
   {
      sdvg_VertexOffset2f();
      return YAC_TRUE;
   }
   else
   {
      Dprintf("[---] sdvg_BeginFilledTriangleFan: sdvg_BeginTriangleFan(numVertices=%u) failed\n", _numVertices);
   }
   return YAC_FALSE;
}

sBool YAC_CALL sdvg_BeginFilledTriangleStrip(sUI _numVertices) {
   //
   // VBO vertex format (8 bytes per vertex):
   //     +0 f32 x
   //     +4 f32 y
   //

   if(NULL == current_shape)
      loc_bind_default_triangles_fill_flat_shape();

   if(sdvg_BeginTriangleStrip(_numVertices, (2*4)/*stride*/))
   {
      sdvg_VertexOffset2f();
      return YAC_TRUE;
   }
   return YAC_FALSE;
}

sBool YAC_CALL sdvg_BeginFilledGouraudTriangles(sUI _numVertices) {
   //
   // VBO vertex format (12 bytes per vertex):
   //     +0 u8  r
   //     +1 u8  g
   //     +2 u8  b
   //     +3 u8  a
   //     +4 f32 x
   //     +8 f32 y
   //
   if(NULL == current_shape)
   {
      BindShape(&triangles_fill_gouraud_32);
   }
   if(sdvg_BeginTriangles(_numVertices, 4+(2*4)/*stride*/))
   {
      sdvg_AttribOffsetARGB("a_color");
      sdvg_VertexOffset2f();
      return YAC_TRUE;
   }
   return YAC_FALSE;
}

sBool YAC_CALL sdvg_BeginFilledGouraudTriangleFan(sUI _numVertices) {
   //
   // VBO vertex format (12 bytes per vertex):
   //     +0 u8  r
   //     +1 u8  g
   //     +2 u8  b
   //     +3 u8  a
   //     +4 f32 x
   //     +8 f32 y
   //
   if(NULL == current_shape)
   {
      BindShape(&triangles_fill_gouraud_32);
   }
   if(sdvg_BeginTriangleFan(_numVertices, 4+(2*4)/*stride*/))
   {
      sdvg_AttribOffsetARGB("a_color");
      sdvg_VertexOffset2f();
      return YAC_TRUE;
   }
   return YAC_FALSE;
}

sBool YAC_CALL sdvg_BeginFilledGouraudTriangleStrip(sUI _numVertices) {
   //
   // VBO vertex format (12 bytes per vertex):
   //     +0 u8  r
   //     +1 u8  g
   //     +2 u8  b
   //     +3 u8  a
   //     +4 f32 x
   //     +8 f32 y
   //
   if(NULL == current_shape)
   {
      BindShape(&triangles_fill_gouraud_32);
   }
   if(sdvg_BeginTriangleStrip(_numVertices, 4+(2*4)/*stride*/))
   {
      sdvg_AttribOffsetARGB("a_color");
      sdvg_VertexOffset2f();
      return YAC_TRUE;
   }
   return YAC_FALSE;
}

static sBool loc_BeginTexturedTriangles(sUI _numVertices, ShaderVG_Shape *_defaultShape) {
   //
   // VBO vertex format (16 bytes per vertex):
   //     +0  f32 u
   //     +4  f32 v
   //     +8  f32 x
   //     +12 f32 y
   //
   if(NULL == current_shape)
   {
      BindShape(_defaultShape);
   }
   if(sdvg_BeginTriangles(_numVertices, (4*4)/*stride*/))
   {
      sdvg_AttribOffset2f("a_uv");
      sdvg_VertexOffset2f();
      return YAC_TRUE;
   }
   return YAC_FALSE;
}

static sBool loc_BeginTexturedTriangleFan(sUI _numVertices, ShaderVG_Shape *_defaultShape) {
   //
   // VBO vertex format (16 bytes per vertex):
   //     +0  f32 u
   //     +4  f32 v
   //     +8  f32 x
   //     +12 f32 y
   //
   if(NULL == current_shape)
   {
      BindShape(_defaultShape);
   }
   if(sdvg_BeginTriangleFan(_numVertices, (4*4)/*stride*/))
   {
      sdvg_AttribOffset2f("a_uv");
      sdvg_VertexOffset2f();
      return YAC_TRUE;
   }
   return YAC_FALSE;
}

static sBool loc_BeginTexturedTriangleStrip(sUI _numVertices, ShaderVG_Shape *_defaultShape) {
   //
   // VBO vertex format (16 bytes per vertex):
   //     +0  f32 u
   //     +4  f32 v
   //     +8  f32 x
   //     +12 f32 y
   //
   if(NULL == current_shape)
   {
      BindShape(_defaultShape);
   }
   if(sdvg_BeginTriangleStrip(_numVertices, (4*4)/*stride*/))
   {
      sdvg_AttribOffset2f("a_uv");
      sdvg_VertexOffset2f();
      return YAC_TRUE;
   }
   return YAC_FALSE;
}

static sBool loc_BeginTexturedGouraudTriangles(sUI _numVertices, ShaderVG_Shape *_defaultShape) {
   //
   // VBO vertex format (20 bytes per vertex):
   //     +0  f32 u
   //     +4  f32 v
   //     +8  u8  r
   //     +9  u8  g
   //     +10 u8  b
   //     +11 u8  a
   //     +12 f32 x
   //     +16 f32 y
   //
   if(NULL == current_shape)
   {
      BindShape(_defaultShape);
   }
   if(sdvg_BeginTriangles(_numVertices, (2*4)+4+(2*4)/*stride*/))
   {
      sdvg_AttribOffset2f("a_uv");
      sdvg_AttribOffsetARGB("a_color");
      sdvg_VertexOffset2f();
      return YAC_TRUE;
   }
   return YAC_FALSE;
}

static sBool loc_BeginTexturedGouraudTriangleFan(sUI _numVertices, ShaderVG_Shape *_defaultShape) {
   //
   // VBO vertex format (20 bytes per vertex):
   //     +0  f32 u
   //     +4  f32 v
   //     +8  u8  r
   //     +9  u8  g
   //     +10 u8  b
   //     +11 u8  a
   //     +12 f32 x
   //     +16 f32 y
   //
   if(NULL == current_shape)
   {
      BindShape(_defaultShape);
   }
   if(sdvg_BeginTriangleFan(_numVertices, 4+(4*4)/*stride*/))
   {
      sdvg_AttribOffset2f("a_uv");
      sdvg_AttribOffsetARGB("a_color");
      sdvg_VertexOffset2f();
      return YAC_TRUE;
   }
   return YAC_FALSE;
}

static sBool loc_BeginTexturedGouraudTriangleStrip(sUI _numVertices, ShaderVG_Shape *_defaultShape) {
   //
   // VBO vertex format (20 bytes per vertex):
   //     +0  f32 u
   //     +4  f32 v
   //     +8  u8  r
   //     +9  u8  g
   //     +10 u8  b
   //     +11 u8  a
   //     +12 f32 x
   //     +16 f32 y
   //
   if(NULL == current_shape)
   {
      BindShape(_defaultShape);
   }
   if(sdvg_BeginTriangleStrip(_numVertices, 4+(4*4)/*stride*/))
   {
      sdvg_AttribOffset2f("a_uv");
      sdvg_AttribOffsetARGB("a_color");
      sdvg_VertexOffset2f();
      return YAC_TRUE;
   }
   return YAC_FALSE;
}

sBool YAC_CALL sdvg_BeginTexturedTriangles(sUI _numVertices) {
   //
   // VBO vertex format (16 bytes per vertex):
   //     +0  f32 u
   //     +4  f32 v
   //     +8  f32 x
   //     +12 f32 y
   //
   return loc_BeginTexturedTriangles(_numVertices, &triangles_tex_uv_flat_32);
}

sBool YAC_CALL sdvg_BeginTexturedTriangleFan(sUI _numVertices) {
   //
   // VBO vertex format (16 bytes per vertex):
   //     +0  f32 u
   //     +4  f32 v
   //     +8  f32 x
   //     +12 f32 y
   //
   return loc_BeginTexturedTriangleFan(_numVertices, &triangles_tex_uv_flat_32);
}

sBool YAC_CALL sdvg_BeginTexturedTriangleStrip(sUI _numVertices) {
   //
   // VBO vertex format (16 bytes per vertex):
   //     +0  f32 u
   //     +4  f32 v
   //     +8  f32 x
   //     +12 f32 y
   //
   return loc_BeginTexturedTriangleStrip(_numVertices, &triangles_tex_uv_flat_32);
}

sBool YAC_CALL sdvg_BeginTexturedGouraudTriangles(sUI _numVertices) {
   //
   // VBO vertex format (20 bytes per vertex):
   //     +0  f32 u
   //     +4  f32 v
   //     +8  u8  r
   //     +9  u8  g
   //     +10 u8  b
   //     +11 u8  a
   //     +12 f32 x
   //     +16 f32 y
   //
   return loc_BeginTexturedGouraudTriangles(_numVertices, &triangles_tex_uv_gouraud_32);
}

sBool YAC_CALL sdvg_BeginTexturedGouraudTriangleFan(sUI _numVertices) {
   //
   // VBO vertex format (20 bytes per vertex):
   //     +0  f32 u
   //     +4  f32 v
   //     +8  u8  r
   //     +9  u8  g
   //     +10 u8  b
   //     +11 u8  a
   //     +12 f32 x
   //     +16 f32 y
   //
   return loc_BeginTexturedGouraudTriangleFan(_numVertices, &triangles_tex_uv_gouraud_32);
}

sBool YAC_CALL sdvg_BeginTexturedGouraudTriangleStrip(sUI _numVertices) {
   //
   // VBO vertex format (20 bytes per vertex):
   //     +0  f32 u
   //     +4  f32 v
   //     +8  u8  r
   //     +9  u8  g
   //     +10 u8  b
   //     +11 u8  a
   //     +12 f32 x
   //     +16 f32 y
   //
   return loc_BeginTexturedGouraudTriangleStrip(_numVertices, &triangles_tex_uv_gouraud_32);
}

sBool YAC_CALL sdvg_BeginTexturedTrianglesAlpha(sUI _numVertices) {
   //
   // VBO vertex format (16 bytes per vertex):
   //     +0  f32 u
   //     +4  f32 v
   //     +8  f32 x
   //     +12 f32 y
   //
   return loc_BeginTexturedTriangles(_numVertices, &triangles_tex_uv_flat_32_alpha);
}

sBool YAC_CALL sdvg_BeginTexturedTriangleFanAlpha(sUI _numVertices) {
   //
   // VBO vertex format (16 bytes per vertex):
   //     +0  f32 u
   //     +4  f32 v
   //     +8  f32 x
   //     +12 f32 y
   //
   return loc_BeginTexturedTriangleFan(_numVertices, &triangles_tex_uv_flat_32_alpha);
}

sBool YAC_CALL sdvg_BeginTexturedTriangleStripAlpha(sUI _numVertices) {
   //
   // VBO vertex format (16 bytes per vertex):
   //     +0  f32 u
   //     +4  f32 v
   //     +8  f32 x
   //     +12 f32 y
   //
   return loc_BeginTexturedTriangleStrip(_numVertices, &triangles_tex_uv_flat_32_alpha);
}

sBool YAC_CALL sdvg_BeginTexturedGouraudTrianglesAlpha(sUI _numVertices) {
   //
   // VBO vertex format (20 bytes per vertex):
   //     +0  f32 u
   //     +4  f32 v
   //     +8  u8  r
   //     +9  u8  g
   //     +10 u8  b
   //     +11 u8  a
   //     +12 f32 x
   //     +16 f32 y
   //
   return loc_BeginTexturedGouraudTriangles(_numVertices, &triangles_tex_uv_gouraud_32_alpha);
}

sBool YAC_CALL sdvg_BeginTexturedGouraudTriangleFanAlpha(sUI _numVertices) {
   //
   // VBO vertex format (20 bytes per vertex):
   //     +0  f32 u
   //     +4  f32 v
   //     +8  u8  r
   //     +9  u8  g
   //     +10 u8  b
   //     +11 u8  a
   //     +12 f32 x
   //     +16 f32 y
   //
   return loc_BeginTexturedGouraudTriangleFan(_numVertices, &triangles_tex_uv_gouraud_32_alpha);
}

sBool YAC_CALL sdvg_BeginTexturedGouraudTriangleStripAlpha(sUI _numVertices) {
   //
   // VBO vertex format (20 bytes per vertex):
   //     +0  f32 u
   //     +4  f32 v
   //     +8  u8  r
   //     +9  u8  g
   //     +10 u8  b
   //     +11 u8  a
   //     +12 f32 x
   //     +16 f32 y
   //
   return loc_BeginTexturedGouraudTriangleStrip(_numVertices, &triangles_tex_uv_gouraud_32_alpha);
}

sBool YAC_CALL sdvg_BeginTexturedTrianglesAlphaSDF(sUI _numVertices) {
   //
   // VBO vertex format (16 bytes per vertex):
   //     +0  f32 u
   //     +4  f32 v
   //     +8  f32 x
   //     +12 f32 y
   //
   return loc_BeginTexturedTriangles(_numVertices, &triangles_tex_uv_flat_32_alpha_sdf);
}

sBool YAC_CALL sdvg_BeginLineStrip(sUI _numPoints) {
   current_draw_mode = DRAW_MODE_LINE_STRIP;
#ifdef SHADERVG_USE_DEFAULT_LINE_14_2
#ifdef USE_VERTEX_ATTRIB_DIVISOR
   return BeginDraw(_numPoints, 4u/*stride*/);
#else
   return BeginDraw(_numPoints, 36u/*stride*/);
#endif // USE_VERTEX_ATTRIB_DIVISOR
#else
#ifdef USE_VERTEX_ATTRIB_DIVISOR
   return BeginDraw(_numPoints, 8u/*stride*/);
#else
   return BeginDraw(_numPoints, 60u/*stride*/);
#endif // USE_VERTEX_ATTRIB_DIVISOR
#endif // SHADERVG_USE_DEFAULT_LINE_14_2
}

sBool YAC_CALL sdvg_BeginLineStripAA(sUI _numPoints) {
   current_draw_mode = DRAW_MODE_LINE_STRIP_AA;
#ifdef SHADERVG_USE_DEFAULT_LINE_14_2
#ifdef USE_VERTEX_ATTRIB_DIVISOR
   return BeginDraw(_numPoints, 4u/*stride*/);
#else
   return BeginDraw(_numPoints, 36u/*stride*/);
#endif // USE_VERTEX_ATTRIB_DIVISOR
#else
#ifdef USE_VERTEX_ATTRIB_DIVISOR
   return BeginDraw(_numPoints, 8u/*stride*/);
#else
   return BeginDraw(_numPoints, 60u/*stride*/);
#endif // USE_VERTEX_ATTRIB_DIVISOR
#endif // SHADERVG_USE_DEFAULT_LINE_14_2
}

sBool YAC_CALL sdvg_BeginLineStripBevel(sUI _numPoints) {
   current_draw_mode = DRAW_MODE_LINE_STRIP_BEVEL;
#ifdef SHADERVG_USE_DEFAULT_LINE_14_2
#ifdef USE_VERTEX_ATTRIB_DIVISOR
   return BeginDraw(_numPoints, 4u/*stride*/);
#else
   return BeginDraw(_numPoints, 54u/*stride*/);
#endif // USE_VERTEX_ATTRIB_DIVISOR
#else
#ifdef USE_VERTEX_ATTRIB_DIVISOR
   return BeginDraw(_numPoints, 8u/*stride*/);
#else
   return BeginDraw(_numPoints, 90u/*stride*/);
#endif // USE_VERTEX_ATTRIB_DIVISOR
#endif // SHADERVG_USE_DEFAULT_LINE_14_2
}

sBool YAC_CALL sdvg_BeginLineStripBevelAA(sUI _numPoints) {
   current_draw_mode = DRAW_MODE_LINE_STRIP_BEVEL_AA;
#ifdef SHADERVG_USE_DEFAULT_LINE_14_2
#ifdef USE_VERTEX_ATTRIB_DIVISOR
   return BeginDraw(_numPoints, 4u/*stride*/);
#else
   return BeginDraw(_numPoints, 54u/*stride*/);
#endif // USE_VERTEX_ATTRIB_DIVISOR
#else
#ifdef USE_VERTEX_ATTRIB_DIVISOR
   return BeginDraw(_numPoints, 8u/*stride*/);
#else
   return BeginDraw(_numPoints, 90u/*stride*/);
#endif // USE_VERTEX_ATTRIB_DIVISOR
#endif // SHADERVG_USE_DEFAULT_LINE_14_2
}

sBool YAC_CALL sdvg_BeginLines(sUI _numPoints) {
   current_draw_mode = DRAW_MODE_LINES;
#ifdef SHADERVG_USE_DEFAULT_LINE_14_2
#ifdef USE_VERTEX_ATTRIB_DIVISOR
   return BeginDraw(_numPoints, 4u/*stride*/);
#else
   return BeginDraw(_numPoints, 36u/*stride*/);
#endif // USE_VERTEX_ATTRIB_DIVISOR
#else
#ifdef USE_VERTEX_ATTRIB_DIVISOR
   return BeginDraw(_numPoints, 8u/*stride*/);
#else
   return BeginDraw(_numPoints, 60u/*stride*/);
#endif // USE_VERTEX_ATTRIB_DIVISOR
#endif // SHADERVG_USE_DEFAULT_LINE_14_2
}

sBool YAC_CALL sdvg_BeginLinesAA(sUI _numPoints) {
   current_draw_mode = DRAW_MODE_LINES_AA;
#ifdef USE_VERTEX_ATTRIB_DIVISOR
   return BeginDraw(_numPoints, 8u/*stride*/);
#else
   return BeginDraw(_numPoints, 60u/*stride*/);
#endif // USE_VERTEX_ATTRIB_DIVISOR
}

sBool YAC_CALL sdvg_BeginPointsSquare(sUI _numPoints) {
   current_draw_mode = DRAW_MODE_POINTS_SQUARE;
#ifdef USE_VERTEX_ATTRIB_DIVISOR
   return BeginDraw(_numPoints, 8u/*stride*/);
#else
   return BeginDraw(_numPoints, 60u/*stride*/);
#endif // USE_VERTEX_ATTRIB_DIVISOR
}

sBool YAC_CALL sdvg_BeginPointsSquareAA(sUI _numPoints) {
   current_draw_mode = DRAW_MODE_POINTS_SQUARE_AA;
#ifdef USE_VERTEX_ATTRIB_DIVISOR
   return BeginDraw(_numPoints, 8u/*stride*/);
#else
   return BeginDraw(_numPoints, 60u/*stride*/);
#endif // USE_VERTEX_ATTRIB_DIVISOR
}

sBool YAC_CALL sdvg_BeginPointsRound(sUI _numPoints) {
   current_draw_mode = DRAW_MODE_POINTS_ROUND;
#ifdef USE_VERTEX_ATTRIB_DIVISOR
   return BeginDraw(_numPoints, 8u/*stride*/);
#else
   return BeginDraw(_numPoints, 60u/*stride*/);
#endif // USE_VERTEX_ATTRIB_DIVISOR
}

sBool YAC_CALL sdvg_BeginPointsRoundAA(sUI _numPoints) {
   current_draw_mode = DRAW_MODE_POINTS_ROUND_AA;
#ifdef USE_VERTEX_ATTRIB_DIVISOR
   return BeginDraw(_numPoints, 8u/*stride*/);
#else
   return BeginDraw(_numPoints, 60u/*stride*/);
#endif // USE_VERTEX_ATTRIB_DIVISOR
}

void YAC_CALL sdvg_VertexOffset2f(void) {
   if(NULL != current_shape && 0u == mapped_user_vbo_id)
   {
      sSI a = current_shape->shape_a_vertex;
      Dsdvg_attrib_offset(a, 2/*size*/, GL_FLOAT, GL_FALSE/*normalize*/, current_draw_stride, current_draw_attrib_offset);
      Dsdvg_attrib_enable(a);
      if(SHADERVG_MAX_ATTRIB_ENABLES != num_draw_attrib_enables)
      {
         current_draw_attrib_enables[num_draw_attrib_enables++] = a;
         current_draw_attrib_offset += 2u * 4u;
      }
      else
      {
         Dsdvg_errorprintf("[---] sdvg_VertexOffset2f: SHADERVG_MAX_ATTRIB_ENABLES(%u) exceeded\n", SHADERVG_MAX_ATTRIB_ENABLES);
      }
   }
}

void YAC_CALL sdvg_AttribOffsetf(const char *_name, sUI _size) {
   if(NULL != current_shape)
   {
      ShaderVG_Shader *shapeShader = &current_shape->shape_shader;
      sSI a = shapeShader->getAttribLocation(_name);
      if(a >= 0)
      {
         Dsdvg_attrib_offset(a, _size, GL_FLOAT, GL_FALSE/*normalize*/, current_draw_stride, current_draw_attrib_offset);
         Dsdvg_attrib_enable(a);
         if(SHADERVG_MAX_ATTRIB_ENABLES != num_draw_attrib_enables)
         {
            current_draw_attrib_enables[num_draw_attrib_enables++] = a;
            current_draw_attrib_offset += _size * 4u;
         }
         else
         {
            Dsdvg_errorprintf("[---] sdvg_AttribOffsetf: SHADERVG_MAX_ATTRIB_ENABLES(%u) exceeded\n", SHADERVG_MAX_ATTRIB_ENABLES);
         }
      }
      else
      {
         Dsdvg_errorprintf("[---] sdvg_AttribOffsetf: failed to resolve attribute \"%s\" (size=%u)\n", _name, _size);
      }
   }
}

void YAC_CALL sdvg_AttribOffset1f(const char *_name) {
   sdvg_AttribOffsetf(_name, 1u);
}

void YAC_CALL sdvg_AttribOffset2f(const char *_name) {
   sdvg_AttribOffsetf(_name, 2u);
}

void YAC_CALL sdvg_AttribOffset3f(const char *_name) {
   sdvg_AttribOffsetf(_name, 3u);
}

void YAC_CALL sdvg_AttribOffset4f(const char *_name) {
   sdvg_AttribOffsetf(_name, 4u);
}

void YAC_CALL sdvg_AttribOffsetARGB(const char *_name) {
   if(NULL != current_shape)
   {
      ShaderVG_Shader *shapeShader = &current_shape->shape_shader;
      sSI a = shapeShader->getAttribLocation(_name);
      if(a >= 0)
      {
         Dsdvg_attrib_offset(a, 4, GL_UNSIGNED_BYTE, GL_TRUE/*normalize*/, current_draw_stride, current_draw_attrib_offset);
         Dsdvg_attrib_enable(a);
         if(SHADERVG_MAX_ATTRIB_ENABLES != num_draw_attrib_enables)
         {
            current_draw_attrib_enables[num_draw_attrib_enables++] = a;
            current_draw_attrib_offset += 4u;
         }
         else
         {
            Dsdvg_errorprintf("[---] sdvg_AttribOffsetARGB: SHADERVG_MAX_ATTRIB_ENABLES(%u) exceeded\n", SHADERVG_MAX_ATTRIB_ENABLES);
         }
      }
      else
      {
         Dsdvg_errorprintf("[---] sdvg_AttribOffsetARGB: failed to resolve attribute \"%s\"\n", _name);
      }
   }
}

#ifdef SHADERVG_SCRIPT_API
void YAC_CALL _sdvg_AttribOffsetf(YAC_String *_name, sUI _size) {
   if(YAC_Is_String(_name))
   {
      YAC_CAST_STRING(name, _name);
      sdvg_AttribOffsetf((const char*)name->chars, _size);
   }
}

void YAC_CALL _sdvg_AttribOffset1f(YAC_String *_name) {
   _sdvg_AttribOffsetf(_name, 1u);
}

void YAC_CALL _sdvg_AttribOffset2f(YAC_String *_name) {
   _sdvg_AttribOffsetf(_name, 2u);
}

void YAC_CALL _sdvg_AttribOffset3f(YAC_String *_name) {
   _sdvg_AttribOffsetf(_name, 3u);
}

void YAC_CALL _sdvg_AttribOffset4f(YAC_String *_name) {
   _sdvg_AttribOffsetf(_name, 4u);
}

void YAC_CALL _sdvg_AttribOffsetARGB(YAC_String *_name) {
   if(YAC_Is_String(_name))
   {
      YAC_CAST_STRING(name, _name);
      sdvg_AttribOffsetARGB((const char*)name->chars);
   }
}
#endif // SHADERVG_SCRIPT_API

void YAC_CALL sdvg_Attribi16(sS16 _i) {
   Dstream_write_s16(attrib_write_buffer, _i);
}

void YAC_CALL _sdvg_Attribi16(sSI _i) {
   Dstream_write_s16(attrib_write_buffer, sS16(_i));
}

void YAC_CALL sdvg_Attrib1f(sF32 _f) {
   Dstream_write_f32(attrib_write_buffer, _f);
   if(!b_debug_write_vbo)
   {
      attrib_write_buffer->io_offset -= 4;
      return;
   }
}

void YAC_CALL sdvg_Attrib2f(sF32 _f1, sF32 _f2) {
   Dstream_write_2f(attrib_write_buffer, _f1, _f2);
   if(!b_debug_write_vbo)
   {
      attrib_write_buffer->io_offset -= 2*4;
      return;
   }
}

void YAC_CALL sdvg_Vertex2f(sF32 _x, sF32 _y) {

   sUI oldOff = attrib_write_buffer->io_offset;

   // (todo) use fxnptr ?
   switch(current_draw_mode)
   {
      default:
         sdvg_Attrib2f(_x, _y);
         break;

      case DRAW_MODE_LINE_STRIP:
      case DRAW_MODE_LINE_STRIP_AA:
#ifdef SHADERVG_USE_DEFAULT_LINE_14_2
         sdvg_BufferAddLinePointFlat14_2(attrib_write_buffer, _x, _y);
#else
         sdvg_BufferAddLinePointFlat32(attrib_write_buffer, _x, _y);
#endif // SHADERVG_USE_DEFAULT_LINE_14_2
         break;

      case DRAW_MODE_LINES:
      case DRAW_MODE_LINES_AA:
#ifdef SHADERVG_USE_DEFAULT_LINE_14_2
         loc_BufferAddLinesPointFlat14_2(attrib_write_buffer, _x, _y);
#else
         loc_BufferAddLinesPointFlat32(attrib_write_buffer, _x, _y);
#endif // SHADERVG_USE_DEFAULT_LINE_14_2
         break;

      case DRAW_MODE_LINE_STRIP_BEVEL:
      case DRAW_MODE_LINE_STRIP_BEVEL_AA:
#ifdef SHADERVG_USE_DEFAULT_LINE_14_2
         sdvg_BufferAddLinePointFlatBevel14_2(attrib_write_buffer, _x, _y);
#else
         sdvg_BufferAddLinePointFlatBevel32(attrib_write_buffer, _x, _y);
#endif // SHADERVG_USE_DEFAULT_LINE_14_2
         break;

      case DRAW_MODE_POINTS_SQUARE:
      case DRAW_MODE_POINTS_SQUARE_AA:
      case DRAW_MODE_POINTS_ROUND:
      case DRAW_MODE_POINTS_ROUND_AA:
         sdvg_BufferAddLinePointFlat32(attrib_write_buffer, _x, _y);
         break;
   }

   if(!b_debug_write_vbo)
   {
      attrib_write_buffer->io_offset = oldOff;
      return;
   }

   current_draw_vertex_index++;
}

void YAC_CALL sdvg_TexCoord2f(sF32 _u, sF32 _v) {
   sdvg_Attrib2f(_u, _v);
}

void YAC_CALL sdvg_Attrib3f(sF32 _f1, sF32 _f2, sF32 _f3) {
   Dstream_write_f32(attrib_write_buffer, _f1);
   Dstream_write_f32(attrib_write_buffer, _f2);
   Dstream_write_f32(attrib_write_buffer, _f3);
   if(!b_debug_write_vbo)
   {
      attrib_write_buffer->io_offset -= 3*4;
      return;
   }
}

void YAC_CALL sdvg_Vertex3f(sF32 _x, sF32 _y, sF32 _z) {
   sdvg_Attrib3f(_x, _y, _z);
}

void YAC_CALL sdvg_Color3f(sF32 _r, sF32 _g, sF32 _b) {
   sdvg_Attrib3f(_r, _g, _b);
}

void YAC_CALL sdvg_Attrib4f(sF32 _f1, sF32 _f2, sF32 _f3, sF32 _f4) {
   Dstream_write_f32(attrib_write_buffer, _f1);
   Dstream_write_f32(attrib_write_buffer, _f2);
   Dstream_write_f32(attrib_write_buffer, _f3);
   Dstream_write_f32(attrib_write_buffer, _f4);
   if(!b_debug_write_vbo)
   {
      attrib_write_buffer->io_offset -= 4*4;
      return;
   }
}

void YAC_CALL sdvg_Color4f(sF32 _r, sF32 _g, sF32 _b, sF32 _a) {
   sdvg_Attrib4f(_r, _g, _b, _a);
}

void YAC_CALL sdvg_AttribARGB(sUI _c32) {
   sdvg_WriteC32AsRGBA8(attrib_write_buffer, _c32);
   if(!b_debug_write_vbo)
   {
      attrib_write_buffer->io_offset -= 4;
      return;
   }
}

void YAC_CALL sdvg_ColorARGB(sUI _c32) {
   sdvg_AttribARGB(_c32);
}

static sBool UpdateShaderUniforms(void) {
   Dsdvg_tracecallv("[trc] sdvg:UpdateShaderUniforms: current_shape=%p\n", current_shape);
   if(NULL != current_shape)
   {
      sSI loc;

      loc = current_shape->shape_u_color_fill;
      Dsdvg_debugprintfvv("[trc] sdvg:UpdateShaderUniforms: shape_u_color_fill=%d\n", current_shape->shape_u_color_fill);
      if(loc >= 0)
      {
         Dsdvg_debugprintfvv("[trc] sdvg:UpdateShaderUniforms: fill=(%f; %f; %f; %f) global_a=%f\n", fill_r, fill_g, fill_b, fill_a, global_a);
         Dsdvg_uniform_4f(loc, fill_r, fill_g, fill_b, fill_a * global_a);
      }

      loc = current_shape->shape_u_color_stroke;
      Dsdvg_debugprintfvv("[trc] sdvg:UpdateShaderUniforms: shape_u_color_stroke=%d\n", current_shape->shape_u_color_stroke);
      if(loc >= 0)
      {
         Dsdvg_uniform_4f(loc, stroke_r, stroke_g, stroke_b, stroke_a * global_a);
      }

      loc = current_shape->shape_u_global_alpha;
      if(loc >= 0)
      {
         Dsdvg_uniform_1f(loc, global_a);
      }

      loc = current_shape->shape_u_decal_alpha;
      if(loc >= 0)
      {
         Dsdvg_uniform_1f(loc, texture_decal_alpha);
      }

      loc = current_shape->shape_u_debug;
      if(loc >= 0)
      {
         Dsdvg_uniform_1f(loc, ellipse_fill_aa.b_debug ? 1.0f : 0.0f);
      }

      loc = current_shape->shape_u_sampler;
      if(loc >= 0)
      {
         Dsdvg_uniform_1i(loc, 0);
      }

      loc = current_shape->shape_u_tex_0;
      if(loc >= 0)
      {
         Dsdvg_uniform_1i(loc, 0);
      }

      loc = current_shape->shape_u_tex_1;
      if(loc >= 0)
      {
         Dsdvg_uniform_1i(loc, 1);
      }

      // (note) TrianglesTexUVFlat32AlphaSDF
      loc = current_shape->shape_u_a_min;
      if(loc >= 0)
      {
         Dsdvg_uniform_1f(loc, alpha_sdf_min);
      }

      loc = current_shape->shape_u_a_max;
      if(loc >= 0)
      {
         Dsdvg_uniform_1f(loc, alpha_sdf_max);
      }

      loc = current_shape->shape_u_a_maxmin_scale;
      if(loc >= 0)
      {
         Dsdvg_uniform_1f(loc, alpha_sdf_maxmin_scale);
      }

      loc = current_shape->shape_u_a_exp;
      if(loc >= 0)
      {
         Dsdvg_uniform_1f(loc, alpha_sdf_exp);
      }

      loc = current_shape->shape_u_paint_tex;
      if(loc >= 0)
      {
         Dsdvg_uniform_1i(loc, 0/*tex_unit*/);
      }

      loc = current_shape->shape_u_paint_start;
      if(loc >= 0)
      {
         Dsdvg_uniform_2f(loc, paint_start_x, paint_start_y);
      }

      loc = current_shape->shape_u_paint_end;
      if(loc >= 0)
      {
         Dsdvg_uniform_2f(loc, paint_end_x, paint_end_y);
      }

      loc = current_shape->shape_u_paint_scale;
      if(loc >= 0)
      {
         const sF32 sclX = (paint_end_x - paint_start_x > 0.0f) ? (1.0f / (paint_end_x - paint_start_x)) : 0.0f;
         const sF32 sclY = (paint_end_y - paint_start_y > 0.0f) ? (1.0f / (paint_end_y - paint_start_y)) : 0.0f;
         Dsdvg_uniform_2f(loc, sclX, sclY);
      }

      loc = current_shape->shape_u_paint_ndir;
      if(loc >= 0)
      {
         sF32 dx = paint_end_x - paint_start_x;
         sF32 dy = paint_end_y - paint_start_y;
         sF32 l = sqrt(dx*dx + dy*dy);
         if(l > 0.0f)
         {
            l = 1.0f / l;
            dx *= l;
            dy *= l;
         }
         else
         {
            dx = 0.0f;
            dy = 0.0f;
         }
         Dsdvg_uniform_2f(loc, dx, dy);
      }

      loc = current_shape->shape_u_paint_ob_len;
      if(loc >= 0)
      {
         const sF32 dx = paint_end_x - paint_start_x;
         const sF32 dy = paint_end_y - paint_start_y;
         sF32 l = sqrt(dx*dx + dy*dy);
         if(l > 0.0f)
         {
            l = 1.0f / l;
         }
         Dsdvg_uniform_1f(loc, l);
      }

      loc = current_shape->shape_u_paint_angle;
      if(loc >= 0)
      {
         Dsdvg_uniform_1f(loc, paint_angle);
      }

      loc = current_shape->shape_u_transform;
      Dsdvg_debugprintfvv("[trc] sdvg:UpdateShaderUniforms: shape_u_transform=%d\n", current_shape->shape_u_transform);
      if(loc >= 0)
      {
         Dsdvg_uniform_mat4(loc, mvp_matrix);

         // (todo) unmap / remap scratch ?
         return YAC_TRUE;
      }
   }
   return YAC_FALSE;
}

#ifdef SHADERVG_USE_SCRATCHBUFFERSUBDATA
void UploadScratchToVBO(void) {
   const sUI off = current_draw_start_offset;
   const sUI len = scratch_buffer->io_offset - current_draw_start_offset;
   Dsdvg_debugprintfvv("[trc] sdvg:UploadScratchToVBO: offset=%u numBytes=%u\n", off, len);
   Dsdvg_glcall(zglBufferSubDataOffset(GL_ARRAY_BUFFER,
                                       off, len,
                                       scratch_buffer, off
                                       )
                );
}

void UpdateScratchOffset(void) {
   current_draw_start_offset = scratch_buffer->io_offset;
}
#endif // SHADERVG_USE_SCRATCHBUFFERSUBDATA

void YAC_CALL sdvg_End(void) {
   if(GL_NONE != current_draw_mode || 0u != mapped_user_vbo_id)
   {
      sUI bytesAvail;
      sBool bSizeOk;

      if(0u == mapped_user_vbo_id && 0u != current_vbo_id)
      {
         if(current_vbo_id == scratch_vbo_id)
         {
            bytesAvail = attrib_write_buffer->io_offset - current_draw_start_offset;
            bSizeOk = (bytesAvail <= current_draw_num_bytes);
         }
         else
         {
            // pre-initialized VBO
            sUI bufSize;
            sBool bScratch = YAC_FALSE;
            loc_get_vbo_size(bufSize, bScratch);
            bSizeOk = (current_draw_num_bytes <= bufSize);
         }
      }
      else
      {
         // initializing VBO (don't draw)
         bytesAvail = attrib_write_buffer->io_offset - current_draw_start_offset;
         bSizeOk = (bytesAvail <= current_draw_num_bytes);
      }

      if(bSizeOk)
      {
         if(0u != current_vbo_id)
         {
            if(bytesAvail > 0u)
            {
#ifdef SHADERVG_USE_SCRATCHBUFFERSUBDATA
               if(scratch_vbo_id == current_vbo_id)
               {
                  Dsdvg_debugprintfvv("[trc] sdvg_End: call zglBufferSubDataOffset offset=%u bytesAvail=%u scratch_buffer->size=%u\n", current_draw_start_offset, bytesAvail, scratch_buffer->size);
                  UploadScratchToVBO();
               }
               else
#endif // SHADERVG_USE_SCRATCHBUFFERSUBDATA
               {
                  if(sdvg_b_glcore && 0u != mapped_user_vbo_id)
                     Dsdvg_glcall(glFlushMappedBufferRange(GL_ARRAY_BUFFER, current_draw_start_offset, bytesAvail));
               }
            }
         }

         Dsdvg_debugprintfvv("[trc] sdvg_End: drawMode=%d current_draw_vertex_index=%u\n", current_draw_mode, current_draw_vertex_index);
         if(0u == mapped_user_vbo_id && current_draw_vertex_index > 0)
         {
            Dsdvg_unmap_scratch_before_draw;

            switch(current_draw_mode)
            {
               default:
                  if(UpdateShaderUniforms())
                  {
                     Dsdvg_glcall(glDrawArrays(current_draw_mode, 0/*first*/, current_draw_vertex_index));
                  }
                  break;

               case DRAW_MODE_LINE_STRIP:
#ifdef SHADERVG_USE_DEFAULT_LINE_14_2
                  sdvg_DrawLineStripFlatVBO14_2(current_vbo_id,
                                                current_draw_start_offset,
                                                current_draw_num_vertices
                                                );
#else
                  sdvg_DrawLineStripFlatVBO32(current_vbo_id,
                                              current_draw_start_offset,
                                              current_draw_vertex_index
                                              );
#endif // SHADERVG_USE_DEFAULT_LINE_14_2
                  break;

               case DRAW_MODE_LINE_STRIP_AA:
#ifdef SHADERVG_USE_DEFAULT_LINE_14_2
                  sdvg_DrawLineStripFlatAAVBO14_2(current_vbo_id,
                                                  current_draw_start_offset,
                                                  current_draw_num_vertices
                                                  );
#else
                  sdvg_DrawLineStripFlatAAVBO32(current_vbo_id,
                                                current_draw_start_offset,
                                                current_draw_vertex_index
                                                );
#endif // SHADERVG_USE_DEFAULT_LINE_14_2
                  break;

               case DRAW_MODE_LINE_STRIP_BEVEL:
#ifdef SHADERVG_USE_DEFAULT_LINE_14_2
                  sdvg_DrawLineStripFlatBevelVBO14_2(current_vbo_id,
                                                     current_draw_start_offset,
                                                     current_draw_num_vertices
                                                     );
#else
                  sdvg_DrawLineStripFlatBevelVBO32(current_vbo_id,
                                                   current_draw_start_offset,
                                                   current_draw_vertex_index
                                                   );
#endif // SHADERVG_USE_DEFAULT_LINE_14_2
                  break;

               case DRAW_MODE_LINE_STRIP_BEVEL_AA:
#ifdef SHADERVG_USE_DEFAULT_LINE_14_2
                  sdvg_DrawLineStripFlatBevelAAVBO14_2(current_vbo_id,
                                                       current_draw_start_offset,
                                                       current_draw_num_vertices
                                                       );
#else
                  sdvg_DrawLineStripFlatBevelAAVBO32(current_vbo_id,
                                                     current_draw_start_offset,
                                                     current_draw_vertex_index
                                                     );
#endif // SHADERVG_USE_DEFAULT_LINE_14_2
                  break;

               case DRAW_MODE_LINES:
#ifdef SHADERVG_USE_DEFAULT_LINE_14_2
                  sdvg_DrawLinesFlatVBO14_2(current_vbo_id,
                                            current_draw_start_offset,
                                            current_draw_num_vertices
                                            );
#else
                  sdvg_DrawLinesFlatVBO32(current_vbo_id,
                                          current_draw_start_offset,
                                          current_draw_vertex_index
                                          );
#endif // SHADERVG_USE_DEFAULT_LINE_14_2
                  break;

               case DRAW_MODE_LINES_AA:
#ifdef SHADERVG_USE_DEFAULT_LINE_14_2
                  sdvg_DrawLinesFlatAAVBO14_2(current_vbo_id,
                                              current_draw_start_offset,
                                              current_draw_num_vertices
                                              );
#else
                  sdvg_DrawLinesFlatAAVBO32(current_vbo_id,
                                            current_draw_start_offset,
                                            current_draw_vertex_index
                                            );
#endif // SHADERVG_USE_DEFAULT_LINE_14_2
                  break;

               case DRAW_MODE_POINTS_SQUARE:
                  sdvg_DrawPointsSquareVBO32(current_vbo_id,
                                             current_draw_start_offset,
                                             current_draw_vertex_index
                                             );
                  break;

               case DRAW_MODE_POINTS_SQUARE_AA:
                  sdvg_DrawPointsSquareAAVBO32(current_vbo_id,
                                               current_draw_start_offset,
                                               current_draw_vertex_index
                                               );
                  break;

               case DRAW_MODE_POINTS_ROUND:
                  sdvg_DrawPointsRoundVBO32(current_vbo_id,
                                            current_draw_start_offset,
                                            current_draw_vertex_index
                                            );
                  break;

               case DRAW_MODE_POINTS_ROUND_AA:
                  sdvg_DrawPointsRoundAAVBO32(current_vbo_id,
                                              current_draw_start_offset,
                                              current_draw_vertex_index
                                              );
                  break;

            } // switch draw_mode

            Dsdvg_remap_scratch_after_draw;
         }
      }
      else
      {
         Dsdvg_errorprintf("[---] sdvg_End: data size mismatch (expect=%u(%u verts), have=%u(%u verts), stride=%u) draw_mode=%d current_vbo_id=%u(scratch=%u) mapped_user_vbo_id=%u \n", current_draw_num_bytes, current_draw_num_bytes/current_draw_stride, bytesAvail, bytesAvail/current_draw_stride, current_draw_stride, current_draw_mode, current_vbo_id, scratch_vbo_id, mapped_user_vbo_id);
      }

      if(0u == mapped_user_vbo_id && current_draw_mode < DRAW_MODE_LINE_STRIP)
      {
         for(sUI enableIdx = 0u; enableIdx < num_draw_attrib_enables; enableIdx++)
         {
            sSI a = current_draw_attrib_enables[enableIdx];
            Dsdvg_attrib_disable(a);
         }
         num_draw_attrib_enables = 0u;
      }

      current_draw_mode = GL_NONE;
   }

   UnsetShapeIfBuiltIn();
}

void YAC_CALL sdvg_DrawFilledRectangle(sF32 _x, sF32 _y, sF32 _w, sF32 _h) {

   BindScratchBuffer();

   sSI aVertexFill = BindFillShader();

   AllocScratchBuffer(aVertexFill, scratch_buffer, (4*2/*xy*/*4/*float*/));

   Dstream_write_2f(scratch_buffer, _x,      _y     );
   Dstream_write_2f(scratch_buffer, _x + _w, _y     );
   Dstream_write_2f(scratch_buffer, _x + _w, _y + _h);
   Dstream_write_2f(scratch_buffer, _x,      _y + _h);

   Dsdvg_draw_triangle_fan(0, 4);

   EndFillShader();
}

void YAC_CALL sdvg_DrawRectangle(sF32 _x, sF32 _y, sF32 _w, sF32 _h, sF32 _b) {

   BindScratchBuffer();

   sSI aVertexFill = BindFillShader();

   AllocScratchBuffer(aVertexFill, scratch_buffer, (8*3*2/*xy*/*4/*float*/));

   ShaderVG_Shape::EmitRectangleVertices(scratch_buffer,
                                         _x, _y,
                                         _w, _h,
                                         _b
                                         );

   Dsdvg_draw_triangles(0, 8*3);

   EndFillShader();
}

sU32 sdvg_ARGB(sU8 _a, sU8 _r, sU8 _g, sU8 _b) {
   return (_a << 24) | (_r << 16) | (_g << 8) | _b;
}

sU32 sdvg_ARGBf(sF32 _a, sF32 _r, sF32 _g, sF32 _b) {
   sUI a = sUI(_a * 255.0f); if(a > 255u) a = 255u;
   sUI r = sUI(_r * 255.0f); if(r > 255u) r = 255u;
   sUI g = sUI(_g * 255.0f); if(g > 255u) g = 255u;
   sUI b = sUI(_b * 255.0f); if(b > 255u) b = 255u;
   return (a << 24) | (r << 16) | (g << 8) | b;
}

sU32 sdvg_MixARGBx(sU32 _x, sU32 _y, sU16 _t) {

   // (todo) optimize ag / rb

   const sSI ax = sSI((_x >> 24) & 255u);
   const sSI rx = sSI((_x >> 16) & 255u);
   const sSI gx = sSI((_x >>  8) & 255u);
   const sSI bx = sSI((_x      ) & 255u);

   const sSI ay = sSI((_y >> 24) & 255u);
   const sSI ry = sSI((_y >> 16) & 255u);
   const sSI gy = sSI((_y >>  8) & 255u);
   const sSI by = sSI((_y      ) & 255u);

   const sUI a = sUI(ax + ((sS16(ay - ax) * _t) >> 8));
   const sUI r = sUI(rx + ((sS16(ry - rx) * _t) >> 8));
   const sUI g = sUI(gx + ((sS16(gy - gx) * _t) >> 8));
   const sUI b = sUI(bx + ((sS16(by - bx) * _t) >> 8));

   return (a << 24) | (r << 16) | (g << 8) | b;
}

sU32 sdvg_MixARGBf(sU32 _x, sU32 _y, sF32 _t) {
   return sdvg_MixARGBx(_x, _y, sU16(_t * 256.0f));
}

sU32 sdvg_MixRGBx(sU32 _x, sU32 _y, sU16 _t) {
   // (todo) optimize g / rb

   const sSI rx = sSI((_x >> 16) & 255u);
   const sSI gx = sSI((_x >>  8) & 255u);
   const sSI bx = sSI((_x      ) & 255u);

   const sSI ry = sSI((_y >> 16) & 255u);
   const sSI gy = sSI((_y >>  8) & 255u);
   const sSI by = sSI((_y      ) & 255u);

   const sUI r = sUI(rx + ((sS16(ry - rx) * _t) >> 8));
   const sUI g = sUI(gx + ((sS16(gy - gx) * _t) >> 8));
   const sUI b = sUI(bx + ((sS16(by - bx) * _t) >> 8));

   return (r << 16) | (g << 8) | b;
}

sU32 sdvg_MixRGBf(sU32 _x, sU32 _y, sF32 _t) {
   return sdvg_MixARGBx(_x, _y, sU16(_t * 256.0f));
}

sU32 sdvg_TintARGB(sU32 _x, sU32 _y) {
   sU16 t16 = ((_y >> 24) & 255u);
   t16 += (t16 >> 7);
   return sdvg_MixARGBx(_x, (_y | 0xFF000000u), t16);
}

sU32 sdvg_TintRGBAlpha(sU32 _x, sU32 _y, sU8 _a8) {
   sU16 t16 = ((_y >> 24) & 255u);
   t16 += (t16 >> 7);
   return sdvg_MixRGBx(_x, _y, t16) | (_a8 << 24);
}

sU32 sdvg_RGBAlpha(sU32 _c32, sU8 _a8) {
   return (_c32 & 0x00FFFFFFu) | (_a8 << 24);
}

sU32 sdvg_HSVAToARGB(sF32 _h, sF32 _s, sF32 _v, sU8 _a8) {
   // converts alpha '_a' (0..255) + hue (0..360) + saturation/value (0..1) to ARGB32 int

   if(_h >= 360.0f)
      _h -= 360.0f;

   _h *= (1.0f / 60.0f);

   const sSI i = (sSI)_h;
   const sF32 f = _h - i;

   const sSI p = sSI( 255.0f * _v * ( 1.0f - _s ) );
   const sSI q = sSI( 255.0f * _v * ( 1.0f - _s * f ) );
   const sSI t = sSI( 255.0f * _v * ( 1.0f - _s * ( 1.0f - f ) ) );

   _v *= 255.0f;

   switch(i)
   {
      case 0:
         return sdvg_ARGB(_a8, sU8(_v), sU8(t), sU8(p));
      case 1:
         return sdvg_ARGB(_a8, sU8(q), sU8(_v), sU8(p));
      case 2:
         return sdvg_ARGB(_a8, sU8(p), sU8(_v), sU8(t));
      case 3:
         return sdvg_ARGB(_a8, sU8(p), sU8(q), sU8(_v));
      case 4:
         return sdvg_ARGB(_a8, sU8(t), sU8(p), sU8(_v));
      default:
         return sdvg_ARGB(_a8, sU8(_v), sU8(p), sU8(q));
   }
}

sU8 sdvg_ARGBToHSVA(sU32 _c32, sF32 *_retH, sF32 *_retS, sF32 *_retV) {

   sF32 r = ( (_c32 >> 16) & 255u ) * (1.0f / 255.0f);
   sF32 g = ( (_c32 >>  8) & 255u ) * (1.0f / 255.0f);
   sF32 b = ( (_c32      ) & 255u ) * (1.0f / 255.0f);

   sF32 h, s, v;

   v = sMAX(r, g);
   v = sMAX(v, b);

   sF32 t = sMIN(r, g);
   t = sMIN(t, b);

   if(v < 0.00001f)
      s = 0.0f;
   else
      s = (v - t) / v;

   const sF32 cr = (v - r) / (v - t);
   const sF32 cg = (v - g) / (v - t);
   const sF32 cb = (v - b) / (v - t);

   if(r == v)
      h = cb - cg;
   else if(g == v)
      h = 2.0f + cr - cb;
   else // if(b == v)
      h = 4.0f + cg - cr;

   h *= 60.0f;

   if(h < 0.0f)
      h += 360.0f;

   *_retH = h;
   *_retS = s;
   *_retV = v;
   return (_c32 >> 24) & 255u;
}

void YAC_CALL sdvg_GradientToTexture(sU32 *_dst, sU32 _dstW,
                                     const sU32 *_colors, sU32 _numColors,
                                     const sSI *_starts, sUI _numStarts,
                                     sBool _bSmoothStep
                                     ) {
   // Dprintf("xxx sdvg_GradientToTexture: dstW=%u numColors=%U numStarts=%u\n", _dstW, _numColors, _numStarts);
   if(_dstW > 0u && _numColors >= 2u && _numStarts >= _numColors)
   {
      const sSI gradientSize = _starts[_numStarts - 1u];
      const sF32 gradientToTexScl = sF32(_dstW) / gradientSize;
      sUI num = sMIN(_numStarts, _numColors);
      sSI texPosPrev = 0u;
      sU32 c32Prev = _colors[0];
      sSI startPrev = _starts[0];
      sUI gradientIdx = 1u;
      while(gradientIdx < num)
      {
         // Dprintf("xxx gradientIdx=%u num=%u gradientSize=%d\n", gradientIdx, num, gradientSize);
         sU32 c32 = _colors[gradientIdx];
         sSI start = _starts[gradientIdx];
         sSI texPos = sSI(start * gradientToTexScl + 0.5f);
         sSI spanSz = start - startPrev;
         // Dprintf("xxx gradientIdx=%u c32=#%08x start=%d texPos=%d spanSz=%d\n", gradientIdx, c32, start, texPos, spanSz);

         if(spanSz > 0)
         {
            sSI texNum = texPos - texPosPrev;
            sSI texCur = texPosPrev;
            if(texPos >= 0 && texNum > 0)
            {
               sF32 t = 0.0f;
               sF32 tStep = 1.0f / sF32(texNum);
               while(texCur < texPos)
               {
                  // Dprintf("xxx texCur=%d texW=%U\n", texCur, _dstW);
                  if(_bSmoothStep)
                  {
                     sF32 u = t * t * (3.0f - 2.0f * t);
                     _dst[texCur++] = sdvg_MixARGBf(c32Prev, c32, u);
                  }
                  else
                  {
                     _dst[texCur++] = sdvg_MixARGBf(c32Prev, c32, t);
                  }
                  t += tStep;
               }
            }
         }

         // Next gradient element
         texPosPrev = texPos;
         c32Prev = c32;
         startPrev = start;
         gradientIdx++;
      }
   }
}

#ifdef SHADERVG_SCRIPT_API
sU32 _sdvg_ARGB(sUI _a, sUI _r, sUI _g, sUI _b) {
   return sdvg_ARGB(sU8(_a), sU8(_r), sU8(_g), sU8(_b));
}

sU32 _sdvg_MixARGBx(sU32 _x, sU32 _y, sUI _t) {
   return sdvg_MixARGBx(_x, _y, sU16(_t));
}

sU32 _sdvg_MixRGBx(sU32 _x, sU32 _y, sUI _t) {
   return sdvg_MixRGBx(_x, _y, sU16(_t));
}

sU32 _sdvg_TintRGBAlpha(sU32 _x, sU32 _y, sUI _a8) {
   return sdvg_TintRGBAlpha(_x, _y, sU8(_a8));
}

sU32 _sdvg_RGBAlpha(sU32 _c32, sUI _a8) {
   return sdvg_RGBAlpha(_c32, sU8(_a8));
}

sU32 _sdvg_HSVAToARGB(sF32 _h, sF32 _s, sF32 _v, sUI _a8) {
   return sdvg_HSVAToARGB(_h, _s, _v, sU8(_a8));
}

sUI _sdvg_ARGBToHSVA(sU32 _c32, YAC_Object *_retH, YAC_Object *_retS, YAC_Object *_retV) {
   if(YAC_VALID(_retH) &&
      YAC_VALID(_retS) &&
      YAC_VALID(_retV)
      )
   {
      sF32 h, s, v;
      sU8 a8 = sdvg_ARGBToHSVA(_c32, &h, &s, &v);
      _retH->yacValueOfF32(h);
      _retS->yacValueOfF32(s);
      _retV->yacValueOfF32(v);
      return a8;
   }
   return 0u;
}

void YAC_CALL _sdvg_GradientToTexture (YAC_Object *_tex, YAC_Object *_colors, YAC_Object *_starts, sBool _bSmoothStep) {
   if(YAC_VALID(_tex))
   {
      if(YAC_TYPE_INT == _tex->yacArrayGetElementType() && sizeof(sU32) == _tex->yacArrayGetElementByteSize())
      {
         sU32 *texData = (sU32*)_tex->yacArrayGetPointer();

         if(YAC_VALID(_colors))
         {
            const sU32 *colorData = (sU32*)_colors->yacArrayGetPointer();

            if(YAC_TYPE_INT == _colors->yacArrayGetElementType() && sizeof(sU32) == _colors->yacArrayGetElementByteSize())
            {
               const sSI *startData = (sSI*)_starts->yacArrayGetPointer();

               if(YAC_VALID(_starts))
               {
                  if(YAC_TYPE_INT == _starts->yacArrayGetElementType() && sizeof(sSI) == _starts->yacArrayGetElementByteSize())
                  {
                     const sUI texW = _tex->yacArrayGetWidth();
                     const sUI numColors = _colors->yacArrayGetNumElements();
                     const sUI numStarts = _starts->yacArrayGetNumElements();

                     sdvg_GradientToTexture(texData, texW, colorData, numColors, startData, numStarts, _bSmoothStep);
                  }
               }
            }
         }
      }
   }
}

#endif // SHADERVG_SCRIPT_API
