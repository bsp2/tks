// ----
// ---- file   : main.cpp
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : Distributed under terms of the MIT license (https://opensource.org/licenses/MIT)
// ----          Copyright 2025 by bsp
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
// ---- info   : "minnie" test main
// ---- note   : C99
// ----
// ----
// ----

#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#include "../inc_minnie.h"
#include "hal.h"

static sBool b_anim       = 1;     // SPACE
static sBool b_anim_xy    = 1;     // TAB
static sBool b_anim_whc   = 1;     // lctrl-TAB
static sBool b_slomo      = 0;     // lctrl-SPACE
static sBool b_aa         = 1;     // 'a'
static sF32  aa_range     = 2.5f;  // (todo) m/lshift-m
static sF32  aa_exp       = 1.0f;  // (todo) 'k', lshift-k
static sF32  stroke_w     = 4.0f;  // LEFT/RIGHT   half width
static sF32  fill_alpha   = 1.0f;  // 'r', 't'
static sF32  decal_alpha  = 1.0f;  // (todo) lshift-'r', lshift-'t'
static sF32  radius_sclx  = 1.0f;  // (todo) 'f', 'g'
static sF32  radius_scly  = 1.0f;  // (todo) 'h', 'j'
static sBool b_sym_radius = 0;     // 'l'
static sF32  size_sclx    = 1.0f;  // 'q', 'w'
static sF32  size_scly    = 1.0f;  // 'z', 'x'

static sU32 last_sdl_ticks = 0u;

static sF32 ang_x = 0.0f;
static sF32 ang_y = 0.0f;
static sF32 ang_w = 0.0f;
static sF32 ang_h = 0.0f;
static sF32 ang_c = 0.0f;

// (note) UP/DOWN
#define RENDER_RECT_FILL_AA                                 0
#define RENDER_RECT_FILL_STROKE_AA                          1
#define RENDER_RECT_STROKE_AA                               2
#define RENDER_ELLIPSE_FILL_AA                              3
#define RENDER_ELLIPSE_FILL_STROKE_AA                       4
#define RENDER_ELLIPSE_STROKE_AA                            5
#define RENDER_ROUNDRECT_FILL_AA                            6
#define RENDER_ROUNDRECT_FILL_STROKE_AA                     7
#define RENDER_ROUNDRECT_STROKE_AA                          8
#define RENDER_TRIANGLES_TEX_UV_FLAT                        9
#define RENDER_TRIANGLES_TEX_UV_GOURAUD                    10
#define RENDER_TRIANGLES_TEX_UV_FLAT_DECAL                 11
#define RENDER_TRIANGLES_TEX_UV_GOURAUD_DECAL              12
#define RENDER_LINE_STRIP_FLAT_1                           13
#define RENDER_LINE_STRIP_FLAT_2                           14
#define RENDER_LINE_STRIP_FLAT_AA_1                        15
#define RENDER_LINE_STRIP_FLAT_AA_2                        16
#define RENDER_LINE_STRIP_FLAT_BEVEL                       17
#define RENDER_LINE_STRIP_FLAT_BEVEL_AA                    18
#define RENDER_CUSTOM_SHADER_1                             19
#define RENDER_CUSTOM_SHADER_2                             20
#define RENDER_CUSTOM_SHADER_3_VBO                         21
#define RENDER_LINES                                       22
#define RENDER_LINES_AA                                    23
#define RENDER_BEGIN_LINE_STRIP                            24
#define RENDER_BEGIN_LINE_STRIP_AA                         25
#define RENDER_BEGIN_LINE_STRIP_BEVEL                      26
#define RENDER_BEGIN_LINE_STRIP_BEVEL_AA                   27
#define RENDER_BEGIN_LINES                                 28
#define RENDER_BEGIN_LINES_AA                              29
#define RENDER_FILLED_RECTANGLE                            30
#define RENDER_BEGIN_POINTS_SQUARE                         31
#define RENDER_BEGIN_POINTS_SQUARE_AA                      32
#define RENDER_BEGIN_POINTS_ROUND                          33
#define RENDER_BEGIN_POINTS_ROUND_AA                       34
#define RENDER_BEGIN_FILLED_TRIANGLES                      35
#define RENDER_BEGIN_FILLED_TRIANGLE_FAN                   36
#define RENDER_BEGIN_FILLED_TRIANGLE_STRIP                 37
#define RENDER_BEGIN_FILLED_GOURAUD_TRIANGLES              38
#define RENDER_BEGIN_FILLED_GOURAUD_TRIANGLE_FAN           39
#define RENDER_BEGIN_FILLED_GOURAUD_TRIANGLE_STRIP         40
#define RENDER_BEGIN_TEXTURED_TRIANGLES                    41
#define RENDER_BEGIN_TEXTURED_TRIANGLES_ALPHA              42
#define RENDER_BEGIN_TEXTURED_TRIANGLE_FAN                 43
#define RENDER_BEGIN_TEXTURED_TRIANGLE_FAN_ALPHA           44
#define RENDER_BEGIN_TEXTURED_TRIANGLE_STRIP               45
#define RENDER_BEGIN_TEXTURED_TRIANGLE_STRIP_ALPHA         46
#define RENDER_BEGIN_TEXTURED_GOURAUD_TRIANGLES            47
#define RENDER_BEGIN_TEXTURED_GOURAUD_TRIANGLES_ALPHA      48
#define RENDER_BEGIN_TEXTURED_GOURAUD_TRIANGLE_FAN         49
#define RENDER_BEGIN_TEXTURED_GOURAUD_TRIANGLE_FAN_ALPHA   50
#define RENDER_BEGIN_TEXTURED_GOURAUD_TRIANGLE_STRIP       51
#define RENDER_BEGIN_TEXTURED_GOURAUD_TRIANGLE_STRIP_ALPHA 52
#define RENDER_TEXT_1                                      53
#define RENDER_TEXT_2_CLIP                                 54
#define RENDER_TEXT_3_SWIRL                                55
#define RENDER_TEXT_4_UL                                   56
#define RENDER_TEXT_5_UL_CLIP                              57
#define RENDER_TEXT_6_ACCEL                                58
#define RENDER_TEXT_7_ACCEL_CLIP                           59
#define RENDER_TEXT_8_ZOOM                                 60
#define RENDER_TEXT_9_ZOOM_SDF                             61
#define RENDER_GRADIENT_TO_TEXTURE                         62
#define RENDER_TRIANGLES_GRADIENT_LINEAR                   63
#define RENDER_TRIANGLES_GRADIENT_RADIAL                   64
#define RENDER_TRIANGLES_GRADIENT_CONIC                    65
#define RENDER_POLYGON_VBO                                 66
#define RENDER_POLYGON_AA_VBO                              67
#define RENDER_BEGIN_POLYGON                               68
#define RENDER_BEGIN_POLYGON_AA                            69
#define RENDER_BEGIN_POLYGON_GRADIENT_LINEAR               70
#define RENDER_BEGIN_POLYGON_GRADIENT_LINEAR_AA            71
#define RENDER_BEGIN_POLYGON_GRADIENT_RADIAL               72
#define RENDER_BEGIN_POLYGON_GRADIENT_RADIAL_AA            73
#define RENDER_BEGIN_POLYGON_GRADIENT_CONIC                74
#define RENDER_BEGIN_POLYGON_GRADIENT_CONIC_AA             75
#define RENDER_BEGIN_POLYGON_CUSTOM_SHADER                 76
#define RENDER_BEGIN_POLYGON_PATTERN                       77
#define RENDER_BEGIN_POLYGON_PATTERN_AA                    78
#define RENDER_BEGIN_POLYGON_PATTERN_ALPHA                 79
#define RENDER_BEGIN_POLYGON_PATTERN_ALPHA_AA              80
#define RENDER_BEGIN_POLYGON_PATTERN_DECAL                 81
#define RENDER_BEGIN_POLYGON_PATTERN_DECAL_AA              82
#define RENDER_BEGIN_POLYGON_PATTERN_DECAL_ALPHA           83
#define RENDER_BEGIN_POLYGON_PATTERN_DECAL_ALPHA_AA        84
#define NUM_RENDER_MODES                                   85  // UP/DOWN

static sSI render_mode = RENDER_RECT_FILL_AA;

static const char *mode_names[NUM_RENDER_MODES] = {
   /*  0 */ "rect_fill_aa",
   /*  1 */ "rect_fill_stroke_aa",
   /*  2 */ "rect_stroke_aa",
   /*  3 */ "ellipse_fill_aa",
   /*  4 */ "ellipse_fill_stroke_aa",
   /*  5 */ "ellipse_stroke_aa",
   /*  6 */ "roundrect_fill_aa",
   /*  7 */ "roundrect_fill_stroke_aa",
   /*  8 */ "roundrect_stroke_aa",
   /*  9 */ "triangles_tex_uv_flat",
   /* 10 */ "triangles_tex_uv_gouraud",
   /* 11 */ "triangles_tex_uv_flat_decal",
   /* 12 */ "triangles_tex_uv_gouraud_decal",
   /* 13 */ "line_strip_flat_1",
   /* 14 */ "line_strip_flat_2",
   /* 15 */ "line_strip_flat_aa_1",
   /* 16 */ "line_strip_flat_aa_2",
   /* 17 */ "line_strip_flat_bevel",
   /* 18 */ "line_strip_flat_bevel_aa",
   /* 19 */ "custom_shader_1",
   /* 20 */ "custom_shader_2",
   /* 21 */ "custom_shader_3(vbo)",
   /* 22 */ "lines",
   /* 23 */ "lines_aa",
   /* 24 */ "begin_line_strip",
   /* 25 */ "begin_line_strip_aa",
   /* 26 */ "begin_line_strip_bevel",
   /* 27 */ "begin_line_strip_bevel_aa",
   /* 28 */ "begin_lines",
   /* 29 */ "begin_lines_aa",
   /* 30 */ "draw_filled_rectangle",
   /* 31 */ "begin_points_square",
   /* 32 */ "begin_points_square_aa",
   /* 33 */ "begin_points_round",
   /* 34 */ "begin_points_round_aa",
   /* 35 */ "begin_filled_triangles",
   /* 36 */ "begin_filled_triangle_fan",
   /* 37 */ "begin_filled_triangle_strip",
   /* 38 */ "begin_filled_gouraud_triangles",
   /* 39 */ "begin_filled_gouraud_triangle_fan",
   /* 40 */ "begin_filled_gouraud_triangle_strip",
   /* 41 */ "begin_textured_triangles",
   /* 42 */ "begin_textured_triangles_alpha",
   /* 43 */ "begin_textured_triangle_fan",
   /* 44 */ "begin_textured_triangle_fan_alpha",
   /* 45 */ "begin_textured_triangle_strip",
   /* 46 */ "begin_textured_triangle_strip_alpha",
   /* 47 */ "begin_textured_gouraud_triangles",
   /* 48 */ "begin_textured_gouraud_triangles_alpha",
   /* 49 */ "begin_textured_gouraud_triangle_fan",
   /* 50 */ "begin_textured_gouraud_triangle_fan_alpha",
   /* 51 */ "begin_textured_gouraud_triangle_strip",
   /* 52 */ "begin_textured_gouraud_triangle_strip_alpha",
   /* 53 */ "text_1",
   /* 54 */ "text_2_clip",
   /* 55 */ "text_3_swirl",
   /* 56 */ "text_4_ul",
   /* 57 */ "text_5_ul_clip",
   /* 58 */ "text_6_accel",
   /* 59 */ "text_7_accel_clip",
   /* 60 */ "text_8_zoom",
   /* 61 */ "text_9_zoom_sdf",
   /* 62 */ "gradient_to_texture",
   /* 63 */ "triangles_gradient_linear",
   /* 64 */ "triangles_gradient_radial",
   /* 65 */ "triangles_gradient_conic",
   /* 66 */ "polygon_vbo",
   /* 67 */ "polygon_aa_vbo",
   /* 68 */ "begin_polygon",
   /* 69 */ "begin_polygon_aa",
   /* 70 */ "begin_polygon_gradient_linear",
   /* 71 */ "begin_polygon_gradient_linear_aa",
   /* 72 */ "begin_polygon_gradient_radial",
   /* 73 */ "begin_polygon_gradient_radial_aa",
   /* 74 */ "begin_polygon_gradient_conic",
   /* 75 */ "begin_polygon_gradient_conic_aa",
   /* 76 */ "begin_polygon_custom_shader",
   /* 77 */ "begin_polygon_pattern",
   /* 78 */ "begin_polygon_pattern_aa",
   /* 79 */ "begin_polygon_pattern_alpha",
   /* 80 */ "begin_polygon_pattern_alpha_aa",
   /* 81 */ "begin_polygon_pattern_decal",
   /* 82 */ "begin_polygon_pattern_decal_aa",
   /* 83 */ "begin_polygon_pattern_decal_alpha",
   /* 84 */ "begin_polygon_pattern_decal_alpha_aa",
};

static YAC_Buffer buf_vbo;
static sUI        buf_vbo_id;

#define TEX_SX  498
#define TEX_SY  365
#define BIN2C_DECL
#include "res/escher.c"                    // const unsigned char mem_base_tex_escher_argb32[727080]
#include "res/escher_alpha.c"              // const unsigned char mem_base_tex_escher_alpha[181770]
#include "res/font_default_30.c"           // const unsigned char mem_base_font_default_30[1368]
#include "res/font_default_30_tex.c"       // const unsigned char mem_base_font_default_30_tex[103424]
#include "res/font_default_256.c"          // const unsigned char mem_base_font_default_256[1368]
#include "res/font_default_256_tex.c"      // const unsigned char mem_base_font_default_256_tex[299008]
#include "res/font_default_256_sdf.c"      // const unsigned char mem_base_font_default_256_sdf[1368]
#include "res/font_default_256_sdf_tex.c"  // const unsigned char mem_base_font_default_256_sdf_tex[299008]
#include "res/pattern_1_alpha.c"           // const unsigned char mem_base_tex_pattern_1_alpha[65536]
#undef BIN2C_DECL

static sUI tex_id = 0u;
static sUI tex_alpha_id = 0u;

static sUI tex_pattern_1_alpha_id = 0u;

static sdvg_font_t font;
static sdvg_font_t font_zoom;
static sdvg_font_t font_zoom_sdf;

static sU32 gradient_colors[5] = {0xFF00ffffu, 0xFF10cf40u, 0xFFefdf30u, 0xFF2040ffu, 0xFF00ffffu};
static sSI  gradient_starts[5] = {0,           200,         400,         500,         1000};
static sU32 tex_gradient[256];
static sUI tex_gradient_id = 0u;


// ---------------------------------------------------------------------------- TestLineStripFlat_1 (13+15)
static void TestLineStripFlat_1(sBool _bAA) {
   buf_vbo.io_offset = 0u;
   sdvg_BufferAddLinePointFlat14_2(&buf_vbo, 100.0f, 360.0f);
   sdvg_BufferAddLinePointFlat14_2(&buf_vbo, 540.0f, 120.0f);
   sdvg_UpdateVBO(buf_vbo_id, 0u/*offset*/, buf_vbo.io_offset/*size*/, &buf_vbo);
   if(_bAA)
      sdvg_DrawLineStripFlatAAVBO14_2(buf_vbo_id, 0u/*offset*/, 2u/*numPoints*/);
   else
      sdvg_DrawLineStripFlatVBO14_2(buf_vbo_id, 0u/*offset*/, 2u/*numPoints*/);
}

// ---------------------------------------------------------------------------- TestLineStripFlat_2 (14+16)
static void TestLineStripFlat_2(sBool _bAA) {
   sUI numSeg = 64u;
   sUI numPoints = numSeg + 1u;
   sF32 w = (sM_2PIf / numSeg);
   sF32 a = ang_x;
   sF32 x = 100.0f;
   sF32 xStep = 440.0f / numSeg;
   buf_vbo.io_offset = 0u;
   for(sUI i = 0u; i < numPoints; i++)
   {
      sF32 y = sinf(a) * 120.0f + 240.0f;
      sdvg_BufferAddLinePointFlat14_2(&buf_vbo, x, y);
      a += w;
      x += xStep;
   }
   sdvg_UpdateVBO(buf_vbo_id, 0u/*offset*/, buf_vbo.io_offset/*size*/, &buf_vbo);
   if(_bAA)
      sdvg_DrawLineStripFlatAAVBO14_2(buf_vbo_id, 0u/*offset*/, numPoints);
   else
      sdvg_DrawLineStripFlatVBO14_2(buf_vbo_id, 0u/*offset*/, numPoints);
}

// ---------------------------------------------------------------------------- TestLineStripFlatBevel (17+18)
static void TestLineStripFlatBevel(sBool _bAA) {
   sUI numSeg = 64u;
   sUI numPoints = numSeg + 1u;
   sF32 w = (sM_2PIf / numSeg);
   sF32 a = ang_x;
   sF32 x = 100.0f;
   sF32 xStep = 440.0f / numSeg;
   buf_vbo.io_offset = 0u;
   for(sUI i = 0u; i < numPoints; i++)
   {
      sF32 y = sinf(a) * 120.0f + 240.0f;
      sdvg_BufferAddLinePointFlatBevel14_2(&buf_vbo, x, y);
      a += w;
      x += xStep;
   }
   sdvg_UpdateVBO(buf_vbo_id, 0u/*offset*/, buf_vbo.io_offset/*size*/, &buf_vbo);
   if(_bAA)
      sdvg_DrawLineStripFlatBevelAAVBO14_2(buf_vbo_id, 0u, numPoints);
   else
      sdvg_DrawLineStripFlatBevelVBO14_2(buf_vbo_id, 0u, numPoints);
}

// ---------------------------------------------------------------------------- TestCustomShader_1 (19)
static sUI custom_shader_idx_1 = 0u;
static void LazyCreateCustomShader_1() {
   if(0u == custom_shader_idx_1)
   {
      const char *vs =
         "uniform mat4 u_transform; \n"
         " \n"
         "ATTRIBUTE vec2 a_vertex; \n"
         " \n"
         "void main(void) { \n"
         "  gl_Position = u_transform * vec4(a_vertex,0,1); \n"
         "} \n"
         ;
      const char *fs =
         "uniform vec4 u_color_fill; \n"
         "uniform float u_off; \n"
         "uniform float u_l2; \n"
         "uniform float u_mod; \n"
         "uniform float u_ioff; \n"
         "uniform float u_scl_y; \n"
        " \n"
         "void main(void) { \n"
         "   float l = (gl_FragCoord.y * u_scl_y) * 16.0f; \n"
         "   float l2 = (length(gl_FragCoord.xy - vec2(320,240)) * u_scl_y) * 16.0f; \n"
         "   l = l + (l2 - l)*u_l2; \n"
         "   float d = floor( (l ) / u_mod); \n"
         "   l += d * u_ioff; \n"
         "   l += (gl_FragCoord.y * u_scl_y) * 64.0f; \n"
         "   l = sin(l - u_off)*1.5+1.5; \n"
         "   float cr = (l >= 2.0) ? (l - 2.0) : 0.0; \n"
         "   float cg = (l >= 1.0) ? min(1.0, (l - 1.0)) : 0.0; \n"
         "   cg *= cg; \n"
         "   float cb = (l  < 1.0) ? l : 1.0; \n"
         "   vec3 c = vec3(cr, cg, cb); \n"
         "   FRAGCOLOR = vec4(u_color_fill.rgb * c, u_color_fill.a); \n"
         "} \n"
         ;
      custom_shader_idx_1 = sdvg_CreateShader(vs, fs);
   }
}
static sBool BindCustomShader_1() {
   sBool r = (custom_shader_idx_1 >= 0u);
   if(r)
   {
      sdvg_BindShader(custom_shader_idx_1);
      sdvg_Uniform1f("u_off", ang_x*2.0f);
      sdvg_Uniform1f("u_l2", (sinf(ang_c)*0.5f+0.5f)*(sinf(ang_y*0.5f+4.5f)*0.71f+0.71f));
      sdvg_Uniform1f("u_mod", 0.73f);
      sdvg_Uniform1f("u_ioff", 2.15f);
      sdvg_Uniform1f("u_scl_y", 0.5f/480.0f);
      sdvg_SetFillColorARGB(0xffffffffu);
      sdvg_SetFillAlpha(fill_alpha);
   }
   return r;
}
static void TestCustomShader_1(sF32 sizeX, sF32 sizeY) {
   LazyCreateCustomShader_1();
   if(BindCustomShader_1())
   {
      if(sdvg_BeginTriangles(6u, 2u*4u))
      {
         sizeX *= 2.0f;
         sizeY *= 2.0f;
         float x = sinf(ang_x*0.5f) * 170.0f + 320.0f - sizeX*0.5f;
         float y = sinf(ang_y*0.5f) * 170.0f + 240.0f - sizeY*0.5f;

         sdvg_VertexOffset2f();

         // tri 1
         sdvg_Vertex2f(x,       y);
         sdvg_Vertex2f(x+sizeX, y);
         sdvg_Vertex2f(x+sizeX, y+sizeY);

         // tri 2
         sdvg_Vertex2f(x,       y      );
         sdvg_Vertex2f(x+sizeX, y+sizeY);
         sdvg_Vertex2f(x,       y+sizeY);

         sdvg_End();
      }
   }
}

// ---------------------------------------------------------------------------- TestCustomShader_2 (20)
static sUI custom_shader_idx_2 = 0u;
static void TestCustomShader_2(sF32 sizeX, sF32 sizeY) {
   if(0u == custom_shader_idx_2)
   {
      const char *vs =
         "uniform mat4 u_transform; \n"
         " \n"
         "ATTRIBUTE vec2 a_vertex; \n"
         "ATTRIBUTE vec4 a_color; \n"
         " \n"
         "VARYING_OUT vec4 v_color; \n"
         " \n"
         "void main(void) { \n"
         "  gl_Position = u_transform * vec4(a_vertex,0,1); \n"
         "  v_color = a_color; \n"
         "} \n"
         ;
      const char *fs =
         "uniform float u_global_alpha; \n"
         " \n"
         "VARYING_IN vec4 v_color; \n"
         " \n"
         "void main(void) { \n"
         "   FRAGCOLOR = vec4(v_color.rgb, v_color.a * u_global_alpha); \n"
         "} \n"
         ;
      custom_shader_idx_2 = sdvg_CreateShader(vs, fs);
   }
   if(custom_shader_idx_2 >= 0u)
   {
      sdvg_BindShader(custom_shader_idx_2);
      sdvg_SetGlobalAlpha( (sinf(ang_w) * 0.5f + 0.5f) * fill_alpha );

      if(sdvg_BeginTriangleFan(4u, 4u*1u + 2u*4u))
      {
         float x = sinf(ang_x*0.5f) * 170.0f + 320.0f - sizeX*0.5f;
         float y = sinf(ang_y*0.5f) * 170.0f + 240.0f - sizeY*0.5f;

         sdvg_AttribOffsetARGB("a_color");
         sdvg_VertexOffset2f();

         sdvg_ColorARGB(0xffff0000u);
         sdvg_Vertex2f(x, y);

         sdvg_ColorARGB(0xffffFF00u);
         sdvg_Vertex2f(x+sizeX, y);

         sdvg_ColorARGB(0xff00FFFFu);
         sdvg_Vertex2f(x+sizeX, y+sizeY);

         sdvg_ColorARGB(0xff5FaFFFu);
         sdvg_Vertex2f(x, y+sizeY);

         sdvg_End();
      }
   }
}

// ---------------------------------------------------------------------------- TestCustomShader_3_VBO (21)
static sUI custom_shader_idx_3 = 0u;
static sUI custom_vbo_id_3 = 0u;
static void TestCustomShader_3_VBO(Matrix4f *mProj, sF32 sizeX, sF32 sizeY) {
   if(0u == custom_shader_idx_3)
   {
      const char *vs =
         "uniform mat4 u_transform; \n"
         " \n"
         "ATTRIBUTE vec2 a_vertex; \n"
         "ATTRIBUTE vec4 a_color; \n"
         " \n"
         "VARYING_OUT vec4 v_color; \n"
         " \n"
         "void main(void) { \n"
         "  gl_Position = u_transform * vec4(a_vertex,0,1); \n"
         "  v_color = a_color; \n"
         "} \n"
         ;
      const char *fs =
         "uniform float u_global_alpha; \n"
         " \n"
         "VARYING_IN vec4 v_color; \n"
         " \n"
         "void main(void) { \n"
         "   FRAGCOLOR = vec4(v_color.rgb, v_color.a * u_global_alpha); \n"
         "} \n"
         ;
      custom_shader_idx_3 = sdvg_CreateShader(vs, fs);

      // Create + initialize VBO
      custom_vbo_id_3 = sdvg_CreateVBO(256);
      sdvg_MapVBO(custom_vbo_id_3);
      if(sdvg_BeginVBO(4u, 4u*1u + 2u*sizeof(sF32)))
      {
         sdvg_AttribOffsetARGB("a_color");  // ignored
         sdvg_VertexOffset2f();             // ignored

         sdvg_ColorARGB(0xffff0000u);
         sdvg_Vertex2f(0, 0);

         sdvg_ColorARGB(0xffffFF00u);
         sdvg_Vertex2f(1, 0);

         sdvg_ColorARGB(0xff00FFFFu);
         sdvg_Vertex2f(1, 1);

         sdvg_ColorARGB(0xff5FaFFFu);
         sdvg_Vertex2f(0, 1);

         sdvg_End();
      }
      // trace "[dbg] TestCustomShader_3_VBO: sdvg_GetMappedVBOOffset()="+sdvg_GetMappedVBOOffset();  // => 48
      sdvg_UnmapVBO();
      // trace "[dbg] TestCustomShader_3_VBO: sdvg_GetVBOSize()="+sdvg_GetVBOSize();  // => 256
   }
   if(custom_shader_idx_3 >= 0u)
   {
      sdvg_BindShader(custom_shader_idx_3);
      sdvg_SetGlobalAlpha( (sinf(ang_w) * 0.5f + 0.5f) * fill_alpha );

      sF32 x = sinf(ang_x*0.5f) * 170.0f + 320.0f - sizeX*0.5f;
      sF32 y = sinf(ang_y*0.5f) * 170.0f + 240.0f - sizeY*0.5f;

      Matrix4f mModel;
      minnie_matrix4f_initIdentity(&mModel);
      minnie_matrix4f_translatef(&mModel, x, y, 0.0f);
      minnie_matrix4f_scalef(&mModel, sizeX, sizeY, 1.0f);

      Matrix4f mvp;
      minnie_matrix4f_mulr(mProj, &mModel, &mvp);
      sdvg_SetTransform(&mvp);

      // Draw pre-initialized VBO
      sdvg_BindVBO(custom_vbo_id_3);
      if(sdvg_BeginTriangleFan(4u, 4u*1u + 2u*4u))
      {
         sdvg_AttribOffsetARGB("a_color");
         sdvg_VertexOffset2f();

         sdvg_End();
      }
      sdvg_BindVBO(0u);
   }
}

// ---------------------------------------------------------------------------- TestLinesFlat (22+23)
static void TestLinesFlat(sBool _bAA) {
   sUI numSeg = 64u;
   sUI numPoints = numSeg * 2u;
   sF32 w = (sM_2PIf / numSeg);
   sF32 a = ang_x * 0.5f;
   buf_vbo.io_offset = 0u;
   for(sUI i = 0u; i < (numPoints / 2u); i++)
   {
      sF32 x1 = sinf(a) * 200.0f + (VP_W*0.5f);
      sF32 y1 = cosf(a) * 200.0f + (VP_H*0.5f);
      sF32 x2 = sinf(a) * 120.0f + (VP_W*0.5f);
      sF32 y2 = cosf(a) * 120.0f + (VP_H*0.5f);
      sdvg_BufferAddLinesPointsFlat14_2(&buf_vbo, x1, y1, x2, y2);
      a += w;
   }
   sdvg_UpdateVBO(buf_vbo_id, 0u/*offset*/, buf_vbo.io_offset/*size*/, &buf_vbo);
   if(_bAA)
      sdvg_DrawLinesFlatAAVBO14_2(buf_vbo_id, 0u/*offset*/, numPoints);
   else
      sdvg_DrawLinesFlatVBO14_2(buf_vbo_id, 0u/*offset*/, numPoints);
}

// ---------------------------------------------------------------------------- TestBeginLineStripFlat (24+25)
static void TestBeginLineStripFlat(sBool _bAA) {
   sUI numSeg = 64u;
   sUI numPoints = numSeg + 1u;
   sF32 w = (sM_2PIf / numSeg);
   sF32 a = ang_x;
   sF32 x = 100.0f;
   sF32 xStep = 440.0f / numSeg;
   if(_bAA
      ? sdvg_BeginLineStripAA(numPoints)
      : sdvg_BeginLineStrip(numPoints)
      )
   {
      for(sUI i = 0u; i < numPoints; i++)
      {
         sF32 y = sinf(a) * 120.0f + 240.0f;
         sdvg_Vertex2f(x, y);
         a += w;
         x += xStep;
      }
      sdvg_End();
   }
}

// ---------------------------------------------------------------------------- TestBeginLineStripFlatBevel (26+27)
static void TestBeginLineStripFlatBevel(sBool _bAA) {
   sUI numSeg = 64u;
   sUI numPoints = numSeg + 1u;
   sF32 w = (sM_2PIf / numSeg);
   sF32 a = ang_x;
   sF32 x = 100.0f;
   sF32 xStep = 440.0f / numSeg;
   if(_bAA
      ? sdvg_BeginLineStripBevelAA(numPoints)
      : sdvg_BeginLineStripBevel(numPoints)
      )
   {
      for(sUI i = 0u; i < numPoints; i++)
      {
         sF32 y = sinf(a) * 120.0f + 240.0f;
         sdvg_Vertex2f(x, y);
         a += w;
         x += xStep;
      }
      sdvg_End();
   }
}

// ---------------------------------------------------------------------------- TestBeginLinesFlat (28+29)
static void TestBeginLinesFlat(sBool _bAA) {
   sdvg_SetStrokeWidth(stroke_w * 0.25f);
   sSI numSeg = 96;
   sSI numPoints = numSeg * 2;
   sF32 w = (sM_2PIf / numSeg);
   sF32 a = ang_x * 0.5f;
   sF32 x1, y1;
   sF32 x2, y2;
   if(1)
   {
      sSI maxPoints = 4096;
      sSI pointIdx = 0;
      if(_bAA
         ? sdvg_BeginLinesAA(maxPoints)
         : sdvg_BeginLines(maxPoints)
         )
      {
         for(sSI i = 0; i < numPoints/2; i++)
         {
            if(pointIdx == maxPoints)
            {
               sdvg_End();

               if(_bAA)
                  sdvg_BeginLinesAA(maxPoints);
               else
                  sdvg_BeginLines(maxPoints);
               pointIdx = 0;
            }

            x1 = sinf(a) * 200.0f + (VP_W/2);
            y1 = cosf(a) * 200.0f + (VP_H/2);
            sdvg_Vertex2f(x1, y1);

            x2 = sinf(a) * 120.0f + (VP_W/2);
            y2 = cosf(a) * 120.0f + (VP_H/2);
            sdvg_Vertex2f(x2, y2);

            a += w;
            pointIdx += 2;
         }
         // trace "xxx t="+(milliSeconds()-t);
         sdvg_End();
      }
   }
   else
   {
      if(_bAA
         ? sdvg_BeginLinesAA(numPoints)
         : sdvg_BeginLines(numPoints)
         )
      {
         for(sSI i = 0; i < numPoints / 2; i++)
         {
            x1 = sinf(a) * 200.0f + (VP_W/2);
            y1 = cosf(a) * 200.0f + (VP_H/2);
            sdvg_Vertex2f(x1, y1);

            x2 = sinf(a) * 120.0f + (VP_W/2);
            y2 = cosf(a) * 120.0f + (VP_H/2);
            sdvg_Vertex2f(x2, y2);

            a += w;
         }
         sdvg_End();
      }
   }
}

// ---------------------------------------------------------------------------- TestBeginPointsSquare (31+32)
static void TestBeginPointsSquare(sBool _bAA) {
   sdvg_SetPointRadius(stroke_w * 2.0f);
   sUI numPoints = 32u;
   sF32 w = (sM_2PIf / numPoints);
   sF32 a = ang_x * 0.5f;
   if(_bAA
      ? sdvg_BeginPointsSquareAA(numPoints)
      : sdvg_BeginPointsSquare(numPoints)
      )
   {
      for(sUI i = 0u; i < numPoints; i++)
      {
         sF32 x = sinf(a) * 200.0f + (VP_W*0.5f);
         sF32 y = cosf(a) * 200.0f + (VP_H*0.5f);
         sdvg_Vertex2f(x, y);

         a += w;
      }
      sdvg_End();
   }
}

// ---------------------------------------------------------------------------- TestBeginPointsRound (33+34)
static void TestBeginPointsRound(sBool _bAA) {
   sdvg_SetPointRadius(stroke_w * 2.0f);
   sUI numPoints = 32u;
   sF32 w = (sM_2PIf / numPoints);
   sF32 a = ang_x * 0.5f;
   if(_bAA
      ? sdvg_BeginPointsRoundAA(numPoints)
      : sdvg_BeginPointsRound(numPoints)
      )
   {
      for(sUI i = 0u; i < numPoints; i++)
      {
         sF32 x = sinf(a) * 200.0f + (VP_W*0.5f);
         sF32 y = cosf(a) * 200.0f + (VP_H*0.5f);
         sdvg_Vertex2f(x, y);

         a += w;
      }
      sdvg_End();
   }
}

// ---------------------------------------------------------------------------- TestBeginFilledTriangles (35)
static void TestBeginFilledTriangles(void) {
   sdvg_SetFillColor4f(1.0f, 0.5f, 0.5f, fill_alpha);
   sF32 a = ang_x * 0.5f;
   sF32 w = 240.0f;
   sF32 h = 180.0f;
   sF32 x = sinf(a) * 200.0f + (VP_W/2) - w*0.5f;
   sF32 y = cosf(a) * 200.0f + (VP_H/2) - h*0.5f;
   sdvg_BindShader(0u);  // use built-in shader
   if(sdvg_BeginFilledTriangles(2u*3u))
   {
      // 1
      sdvg_Vertex2f(x, y);
      sdvg_Vertex2f(x+w, y);
      sdvg_Vertex2f(x+w, y+h);

      // 2
      sdvg_Vertex2f(x, y);
      sdvg_Vertex2f(x+w, y+h);
      sdvg_Vertex2f(x, y+h);

      sdvg_End();
   }
}

// ---------------------------------------------------------------------------- TestBeginFilledTriangleFan (36)
static void TestBeginFilledTriangleFan(void) {
   sdvg_SetFillColor4f(0.5f, 0.5f, 1.0f, fill_alpha);
   sF32 a = ang_x * 0.5f;
   sF32 w = 240.0f;
   sF32 h = 180.0f;
   sF32 x = sinf(a) * 200.0f + (VP_W*0.5f) - w*0.5f;
   sF32 y = cosf(a) * 200.0f + (VP_H*0.5f) - h*0.5f;
   sdvg_BindShader(0u);  // use built-in shader
   if(sdvg_BeginFilledTriangleFan(4u))
   {
      sdvg_Vertex2f(x, y);
      sdvg_Vertex2f(x+w, y);
      sdvg_Vertex2f(x+w, y+h);
      sdvg_Vertex2f(x, y+h);

      sdvg_End();
   }
}

// ---------------------------------------------------------------------------- TestBeginFilledTriangleStrip (37)
static void TestBeginFilledTriangleStrip(void) {
   sdvg_SetFillColor4f(0.5f, 1.0f, 0.5f, fill_alpha);
   sF32 a = ang_x * 0.5f;
   sF32 w = 240.0f;
   sF32 h = 180.0f;
   sF32 x = sinf(a) * 200.0f + (VP_W*0.5f) - w*0.5f;
   sF32 y = cosf(a) * 200.0f + (VP_H*0.5f) - h*0.5f;
   sdvg_BindShader(0u);  // use built-in shader
   if(sdvg_BeginFilledTriangleStrip(4u))
   {
      sdvg_Vertex2f(x+w, y);
      sdvg_Vertex2f(x+w, y+h);
      sdvg_Vertex2f(x,   y);
      sdvg_Vertex2f(x,   y+h);  // caution: CCW

      sdvg_End();
   }
}

// ---------------------------------------------------------------------------- TestBeginFilledGouraudTriangles (38)
static void TestBeginFilledGouraudTriangles(void) {
   sdvg_SetGlobalAlpha(fill_alpha);
   sF32 a = ang_x * 0.5f;
   sF32 w = 240.0f;
   sF32 h = 180.0f;
   sF32 x = sinf(a) * 200.0f + (VP_W*0.5f) - w*0.5f;
   sF32 y = cosf(a) * 200.0f + (VP_H*0.5f) - h*0.5f;
   sdvg_BindShader(0u);  // use built-in shader
   if(sdvg_BeginFilledGouraudTriangles(2u*3u))
   {
      // 1
      sdvg_ColorARGB(0xffff0000u);
      sdvg_Vertex2f(x, y);

      sdvg_ColorARGB(0xffffFF00u);
      sdvg_Vertex2f(x+w, y);

      sdvg_ColorARGB(0xff00FFFFu);
      sdvg_Vertex2f(x+w, y+h);

      // 2
      sdvg_ColorARGB(0xffff0000u);
      sdvg_Vertex2f(x, y);

      sdvg_ColorARGB(0xff00FFFFu);
      sdvg_Vertex2f(x+w, y+h);

      sdvg_ColorARGB(0xff5FaFFFu);
      sdvg_Vertex2f(x, y+h);

      sdvg_End();
   }
}

// ---------------------------------------------------------------------------- TestBeginFilledGouraudTriangleFan (39)
static void TestBeginFilledGouraudTriangleFan() {
   sdvg_SetGlobalAlpha(fill_alpha);
   sF32 a = ang_x * 0.5f;
   sF32 w = 240.0f;
   sF32 h = 180.0f;
   sF32 x = sinf(a) * 200.0f + (VP_W*0.5f) - w*0.5f;
   sF32 y = cosf(a) * 200.0f + (VP_H*0.5f) - h*0.5f;
   sdvg_BindShader(0u);  // use built-in shader
   if(sdvg_BeginFilledGouraudTriangleFan(4u))
   {
      sdvg_ColorARGB(0xffff0000u);
      sdvg_Vertex2f(x, y);

      sdvg_ColorARGB(0xffffFF00u);
      sdvg_Vertex2f(x+w, y);

      sdvg_ColorARGB(0xff00FFFFu);
      sdvg_Vertex2f(x+w, y+h);

      sdvg_ColorARGB(0xff5FaFFFu);
      sdvg_Vertex2f(x, y+h);

      sdvg_End();
   }
}

// ---------------------------------------------------------------------------- TestBeginFilledGouraudTriangleStrip (40)
static void TestBeginFilledGouraudTriangleStrip(void) {
   sdvg_SetGlobalAlpha(fill_alpha);
   sF32 a = ang_x * 0.5f;
   sF32 w = 240.0f;
   sF32 h = 180.0f;
   sF32 x = sinf(a) * 200.0f + (VP_W*0.5f) - w*0.5f;
   sF32 y = cosf(a) * 200.0f + (VP_H*0.5f) - h*0.5f;
   sdvg_BindShader(0u);  // use built-in shader
   if(sdvg_BeginFilledGouraudTriangleStrip(4u))
   {
      sdvg_ColorARGB(0xffffFF00u);
      sdvg_Vertex2f(x+w, y);

      sdvg_ColorARGB(0xff00FFFFu);
      sdvg_Vertex2f(x+w, y+h);

      sdvg_ColorARGB(0xffff0000u);
      sdvg_Vertex2f(x, y);

      sdvg_ColorARGB(0xff5FaFFFu);
      sdvg_Vertex2f(x, y+h);  // caution: CCW

      sdvg_End();
   }
}

// ---------------------------------------------------------------------------- TestBeginTexturedTriangles (41+42)
static void TestBeginTexturedTriangles(sBool _bAlpha) {
   sdvg_BindTexture2D(_bAlpha ? tex_alpha_id : tex_id, YAC_FALSE/*bRepeat*/, YAC_TRUE/*bFilter*/);
   sdvg_SetFillColor4f(1.0f, 1.0f, 1.0f, fill_alpha);
   sF32 a = ang_x * 0.5f;
   sF32 w = 240.0f;
   sF32 h = 180.0f;
   sF32 x = sinf(a) * 200.0f + (VP_W*0.5f) - w*0.5f;
   sF32 y = cosf(a) * 200.0f + (VP_H*0.5f) - h*0.5f;
   sdvg_BindShader(0u);  // use built-in shader
   if(_bAlpha
      ? sdvg_BeginTexturedTrianglesAlpha(2u*3u)
      : sdvg_BeginTexturedTriangles(2u*3u)
      )
   {
      // 1
      sdvg_TexCoord2f(0.0f, 0.0f);
      sdvg_Vertex2f(x, y);

      sdvg_TexCoord2f(1.0f, 0.0f);
      sdvg_Vertex2f(x+w, y);

      sdvg_TexCoord2f(1.0f, 1.0f);
      sdvg_Vertex2f(x+w, y+h);

      // 2
      sdvg_TexCoord2f(0.0f, 0.0f);
      sdvg_Vertex2f(x, y);

      sdvg_TexCoord2f(1.0f, 1.0f);
      sdvg_Vertex2f(x+w, y+h);

      sdvg_TexCoord2f(0.0f, 1.0f);
      sdvg_Vertex2f(x, y+h);

      sdvg_End();
   }
   sdvg_UnbindTexture2D();
}

// ---------------------------------------------------------------------------- TestBeginTexturedTriangleFan (43+44)
static void TestBeginTexturedTriangleFan(sBool _bAlpha) {
   sdvg_BindTexture2D(_bAlpha ? tex_alpha_id : tex_id, YAC_FALSE/*bRepeat*/, YAC_TRUE/*bFilter*/);
   sdvg_SetFillColor4f(1.0f, 1.0f, 1.0f, fill_alpha);
   sF32 a = ang_x * 0.5f;
   sF32 w = 240.0f;
   sF32 h = 180.0f;
   sF32 x = sinf(a) * 200.0f + (VP_W*0.5f) - w*0.5f;
   sF32 y = cosf(a) * 200.0f + (VP_H*0.5f) - h*0.5f;
   sdvg_BindShader(0u);  // use built-in shader
   if(_bAlpha
      ? sdvg_BeginTexturedTriangleFanAlpha(4u)
      : sdvg_BeginTexturedTriangleFan(4u)
      )
   {
      sdvg_TexCoord2f(0.0f, 0.0f);
      sdvg_Vertex2f(x, y);

      sdvg_TexCoord2f(1.0f, 0.0f);
      sdvg_Vertex2f(x+w, y);

      sdvg_TexCoord2f(1.0f, 1.0f);
      sdvg_Vertex2f(x+w, y+h);

      sdvg_TexCoord2f(0.0f, 1.0f);
      sdvg_Vertex2f(x, y+h);

      sdvg_End();
   }
   sdvg_UnbindTexture2D();
}

// ---------------------------------------------------------------------------- TestBeginTexturedTriangleStrip (45+46)
static void TestBeginTexturedTriangleStrip(sBool _bAlpha) {
   sdvg_BindTexture2D(_bAlpha ? tex_alpha_id : tex_id, YAC_FALSE/*bRepeat*/, YAC_TRUE/*bFilter*/);
   sdvg_SetFillColor4f(1.0f, 1.0f, 1.0f, fill_alpha);
   sF32 a = ang_x * 0.5f;
   sF32 w = 240.0f;
   sF32 h = 180.0f;
   sF32 x = sinf(a) * 200.0f + (VP_W*0.5f) - w*0.5f;
   sF32 y = cosf(a) * 200.0f + (VP_H*0.5f) - h*0.5f;
   sdvg_BindShader(0u);  // use built-in shader
   if(_bAlpha
      ? sdvg_BeginTexturedTriangleStripAlpha(4u)
      : sdvg_BeginTexturedTriangleStrip(4u)
      )
   {
      sdvg_TexCoord2f(1.0f, 0.0f);
      sdvg_Vertex2f(x+w, y);

      sdvg_TexCoord2f(1.0f, 1.0f);
      sdvg_Vertex2f(x+w, y+h);

      sdvg_TexCoord2f(0.0f, 0.0f);
      sdvg_Vertex2f(x, y);

      sdvg_TexCoord2f(0.0f, 1.0f);
      sdvg_Vertex2f(x, y+h);  // caution: CCW

      sdvg_End();
   }
   sdvg_UnbindTexture2D();
}

// ---------------------------------------------------------------------------- TestBeginTexturedGouraudTriangles (47)
static void TestBeginTexturedGouraudTriangles(sBool _bAlpha) {
   sdvg_BindTexture2D(_bAlpha ? tex_alpha_id : tex_id, YAC_FALSE/*bRepeat*/, YAC_TRUE/*bFilter*/);
   sdvg_SetFillColor4f(1.0f, 1.0f, 1.0f, fill_alpha);
   sF32 a = ang_x * 0.5f;
   sF32 w = 240.0f;
   sF32 h = 180.0f;
   sF32 x = sinf(a) * 200.0f + (VP_W*0.5f) - w*0.5f;
   sF32 y = cosf(a) * 200.0f + (VP_H*0.5f) - h*0.5f;
   sdvg_BindShader(0u);  // use built-in shader
   if(_bAlpha
      ? sdvg_BeginTexturedGouraudTrianglesAlpha(2u*3u)
      : sdvg_BeginTexturedGouraudTriangles(2u*3u)
      )
   {
      // 1
      sdvg_TexCoord2f(0.0f, 0.0f);
      sdvg_ColorARGB(0xFFFF4040u);
      sdvg_Vertex2f(x, y);

      sdvg_TexCoord2f(1.0f, 0.0f);
      sdvg_ColorARGB(0xFFFFFF40u);
      sdvg_Vertex2f(x+w, y);

      sdvg_TexCoord2f(1.0f, 1.0f);
      sdvg_ColorARGB(0xFF4040FFu);
      sdvg_Vertex2f(x+w, y+h);

      // 2
      sdvg_TexCoord2f(0.0f, 0.0f);
      sdvg_ColorARGB(0xFFFF4040u);
      sdvg_Vertex2f(x, y);

      sdvg_TexCoord2f(1.0f, 1.0f);
      sdvg_ColorARGB(0xFF4040FFu);
      sdvg_Vertex2f(x+w, y+h);

      sdvg_TexCoord2f(0.0f, 1.0f);
      sdvg_ColorARGB(0xFF40FFFFu);
      sdvg_Vertex2f(x, y+h);

      sdvg_End();
   }
   sdvg_UnbindTexture2D();
}

// ---------------------------------------------------------------------------- TestBeginTexturedGouraudTriangleFan (49+50)
static void TestBeginTexturedGouraudTriangleFan(sBool _bAlpha) {
   sdvg_BindTexture2D(_bAlpha ? tex_alpha_id : tex_id, YAC_FALSE/*bRepeat*/, YAC_TRUE/*bFilter*/);
   sdvg_SetFillColor4f(1.0f, 1.0f, 1.0f, fill_alpha);
   sF32 a = ang_x * 0.5f;
   sF32 w = 240.0f;
   sF32 h = 180.0f;
   sF32 x = sinf(a) * 200.0f + (VP_W*0.5f) - w*0.5f;
   sF32 y = cosf(a) * 200.0f + (VP_H*0.5f) - h*0.5f;
   sdvg_BindShader(0u);  // use built-in shader
   if(_bAlpha
      ? sdvg_BeginTexturedGouraudTriangleFanAlpha(4u)
      : sdvg_BeginTexturedGouraudTriangleFan(4u)
      )
   {
      sdvg_TexCoord2f(0.0f, 0.0f);
      sdvg_ColorARGB(0xFFFF4040u);
      sdvg_Vertex2f(x, y);

      sdvg_TexCoord2f(1.0f, 0.0f);
      sdvg_ColorARGB(0xFFFFFF40u);
      sdvg_Vertex2f(x+w, y);

      sdvg_TexCoord2f(1.0f, 1.0f);
      sdvg_ColorARGB(0xFF4040FFu);
      sdvg_Vertex2f(x+w, y+h);

      sdvg_TexCoord2f(0.0f, 1.0f);
      sdvg_ColorARGB(0xFF40FFFFu);
      sdvg_Vertex2f(x, y+h);

      sdvg_End();
   }
   sdvg_UnbindTexture2D();
}

// ---------------------------------------------------------------------------- TestBeginTexturedGouraudTriangleStrip (51+52)
static void TestBeginTexturedGouraudTriangleStrip(sBool _bAlpha) {
   sdvg_BindTexture2D(_bAlpha ? tex_alpha_id : tex_id, YAC_FALSE/*bRepeat*/, YAC_TRUE/*bFilter*/);
   sdvg_SetFillColor4f(1.0f, 1.0f, 1.0f, fill_alpha);
   sF32 a = ang_x * 0.5f;
   sF32 w = 240.0f;
   sF32 h = 180.0f;
   sF32 x = sinf(a) * 200.0f + (VP_W*0.5f) - w*0.5f;
   sF32 y = cosf(a) * 200.0f + (VP_H*0.5f) - h*0.5f;
   sdvg_BindShader(0u);  // use built-in shader
   if(_bAlpha
      ? sdvg_BeginTexturedGouraudTriangleStripAlpha(4u)
      : sdvg_BeginTexturedGouraudTriangleStrip(4u)
      )
   {
      sdvg_TexCoord2f(1.0f, 0.0f);
      sdvg_ColorARGB(0xFFFFFF40u);
      sdvg_Vertex2f(x+w, y);

      sdvg_TexCoord2f(1.0f, 1.0f);
      sdvg_ColorARGB(0xFF4040FFu);
      sdvg_Vertex2f(x+w, y+h);

      sdvg_TexCoord2f(0.0f, 0.0f);
      sdvg_ColorARGB(0xFFFF4040u);
      sdvg_Vertex2f(x, y);

      sdvg_TexCoord2f(0.0f, 1.0f);
      sdvg_ColorARGB(0xFF40FFFFu);
      sdvg_Vertex2f(x, y+h);  // caution: CCW

      sdvg_End();
   }
   sdvg_UnbindTexture2D();
}

// ---------------------------------------------------------------------------- TestText_1 (53)
static void TestText_1(void) {
   sdvg_BindFont(&font, YAC_FALSE/*bFilter*/);
   sdvg_EnableBlending();
   sdvg_SetFillColor4f(1.0f, 1.0f, 1.0f, fill_alpha);
   const char *s = "hello, world.\nmulti\nline\ntext.";

   sSI minX, maxX, minY, maxY;
   sdvg_TextBBox(&font, s, &minX, &maxX, &minY, &maxY);
   sSI w = (maxX - minX);
   sSI h = (maxY - minY);
   // trace "xxx min=("+minX+","+minY+") max=("+maxX+","+maxY+")";

   sF32 x = sinf(ang_x*0.5f) * (640.0f-w)*0.5f + 320.0f - w*0.5f;
   sF32 y = sinf(ang_y*0.5f) * (480.0f-h)*0.5f + 240.0f - h*0.5f;

   sdvg_DrawText(s, x - minX, y - minY);

   sdvg_UnbindFont();
}

// ---------------------------------------------------------------------------- TestText_2_Clip (54)
static void TestText_2_Clip(void) {
   sdvg_BindFont(&font, YAC_FALSE/*bFilter*/);
   sdvg_EnableBlending();
   sdvg_SetFillColor4f(1.0f, 1.0f, 1.0f, fill_alpha);

   const char *s = "############\ntext*text*text*text\nclip*clip*clip*clip*\ntest*test*test*test.\n############";

   sSI minX, maxX, minY, maxY;
   sdvg_TextBBox(&font, s, &minX, &maxX, &minY, &maxY);
   sSI w = (maxX - minX);
   sSI h = (maxY - minY);

   sF32 x = sinf(ang_x*0.5f) * (640.0f-w)*0.5f + 320.0f - w*0.5f - minX;
   sF32 y = sinf(ang_y*0.5f) * (480.0f-h)*0.5f + 240.0f - h*0.5f - minY;
   w += minX;
   h += minY;

   sF32 clipW = sinf(ang_w*8.0f) * w*0.5f + w*0.5f;
   sF32 clipH = sinf(ang_h*8.0f) * h*0.5f + h*0.5f;

   sF32 clipLeft  = x + w*0.5f - clipW*0.5f;
   sF32 clipRight = clipLeft + clipW;

   sF32 clipTop    = y + h*0.5f - clipH*0.5f;
   sF32 clipBottom = clipTop + clipH;

   sdvg_DrawTextClipped(s, x, y, clipLeft, clipTop, clipRight, clipBottom);

   sdvg_UnbindFont();
}

// ---------------------------------------------------------------------------- TestText_3_Swirl (55)
static sUI test_text_3_shader_idx = 0u;
static void TestText_3_draw(const char *_s, sF32 _scl, sF32 _ang, sF32 _deltaC, sU8 _a, sF32 _hue) {
   char sFloor[256]; 
   char sCeil[256]; 
   sSI i = 0;
   sF32 deltaCF = 0;
   if(_deltaC >= 0.5f)
      deltaCF = (_deltaC-0.5f) * 2.0f * 26.0f;
   else if(_deltaC <= -0.5f)
      deltaCF = -(_deltaC+0.5f) * 2.0f * -26.0f;
   sF32 deltaCFrac = sFRAC(deltaCF);
   if(deltaCFrac < 0.0f)
      deltaCFrac += 1.0f;
   sSI deltaCFloor = (sSI)floorf(deltaCF);
   sSI deltaCCeil = (sSI)ceilf(deltaCF);
   sUI numChars = (sUI)strlen(_s);
   for(sUI charIdx = 0u; charIdx < numChars; charIdx++)
   {
      sFloor[i] = _s[i];
      sCeil [i] = _s[i];
      if(' ' != _s[i])
      {
         char c = sFloor[i] + deltaCFloor;
         if(c < 33)
            c = (c - 33) + 'z';
         else if(c > 'z')
            c = (c - 'z') + 33;
         sFloor[i] = c;

         c = sCeil[i] + deltaCCeil;
         if(c < 33)
            c = (c - 33) + 'z';
         else if(c > 'z')
            c = (c - 'z') + 33;
         sCeil[i] = c;
      }
      i++;
   }

   sdvg_Uniform2f("u_scale", _scl, _scl);
   sdvg_Uniform1f("u_ang", _ang);
   sU8 a;

   if(_hue < 0.0f)
      _hue += 360.0f;
   sU32 c24 = sdvg_HSVAToARGB(_hue, 0.3f/*s*/, 1.0f/*v*/, 0u/*a*/);

   deltaCFrac = deltaCFrac + ((deltaCFrac * deltaCFrac * (3.0f - 2.0f * deltaCFrac))-deltaCFrac) * 0.5f;

   // if(255 == _a) trace "xxx deltaCFloor="+deltaCFloor+" deltaCCeil="+deltaCCeil+" deltaCFrac="+deltaCFrac;
   a = (sU8)(_a * (1.0f - deltaCFrac) * fill_alpha);
   if(a > 0u)
   {
      sdvg_Uniform1f("u_scale_to_ang", sM_2PIf / sdvg_TextWidth(&font, sFloor));
      sdvg_SetColorARGB( (a << 24) | c24 );
      sdvg_DrawText(sFloor, 0, 0);
   }

   a = (sU8)(_a * deltaCFrac * fill_alpha);
   if(a > 0u)
   {
      sdvg_Uniform1f("u_scale_to_ang", sM_2PIf / sdvg_TextWidth(&font, sCeil));
      sdvg_SetColorARGB( (a << 24) | c24 );
      sdvg_DrawText(sCeil, 0, 0);
   }
}
static void TestText_3_Swirl(void) {
   if(0u == test_text_3_shader_idx)
   {
      const char *vs =
         "uniform mat4 u_transform; \n"
         "uniform float u_ang; \n"
         "uniform float u_scale_to_ang; \n"
         "uniform float u_scale_to_dist; \n"
         "uniform vec2  u_scale; \n"
         "uniform vec2 u_center; \n"
         " \n"
         "ATTRIBUTE vec2 a_vertex; \n"
         "ATTRIBUTE vec2 a_uv; \n"
         " \n"
         "VARYING_OUT vec2 v_uv; \n"
         " \n"
         "void main(void) { \n"
         "  float a = a_vertex.x * u_scale_to_ang; \n"
         "  vec2 n = vec2(sin(u_ang + a), cos(u_ang + a)); \n"
         "  vec2 v = vec2(n.x * u_scale.x, n.y * u_scale.y); \n"
         "  v += n * a_vertex.y * u_scale_to_dist; \n"
         "  v += u_center; \n"
         "  gl_Position = u_transform * vec4(v,0,1); \n"
         "  v_uv = a_uv; \n"
         "} \n"
         ;
      const char *fs =
         "uniform sampler2D u_sampler; \n"
         "uniform vec4      u_color_fill; \n"
         " \n"
         "VARYING_IN vec2 v_uv; \n"
         " \n"
         "void main(void) { \n"
         "  FRAGCOLOR = vec4(u_color_fill.rgb, TEXTURE2D(u_sampler, v_uv).TEX_ALPHA * u_color_fill.a); \n"
         "} \n"
         ;
      test_text_3_shader_idx = sdvg_CreateShader(vs, fs);
      if(0u == test_text_3_shader_idx)
      {
         Derrorprintf("failed to created test_text_3_shader_idx");
      }
   }
   if(test_text_3_shader_idx >= 1u)
   {
      sdvg_BindFont(&font, YAC_TRUE/*bFilter*/);
      sdvg_EnableBlending();

      sdvg_BindShader(test_text_3_shader_idx);
      sdvg_Uniform2f("u_center", 320.0f, 240.0f);
      sdvg_Uniform1f("u_scale_to_dist", 1.3f);

      const char *s = "hello, world. round and round.";  // 26 glyphs =>  (26*2*7*2 = 728 triangles)

      sF32 rotBaseA = ang_h * 2.0f;
      sF32 rotStepA = sM_2PIf / 16.0f;
      sF32 rotA;

      sF32 rotBaseB = ang_w * 4.0f;
      sF32 rotStepB = sM_2PIf / 24.0f;

      sF32 rotBaseC = ang_c;
      sF32 rotStepC = sM_2PIf / 43.0f;

      sF32 baseScale = 240.0f;
      sF32 scaleStep = 40.0f;
      sF32 scaleMod  = 25.0f;

      sF32 rotBaseHue = ang_x * (360.0f / sM_2PIf);
      sF32 hueStep = 360.0 / 5.0;

      rotA = sinf(rotBaseA + rotStepA*0.0f) * sM_PIf;
      TestText_3_draw(s,
                      baseScale - scaleStep*0.0f + sinf(rotBaseB + rotStepB*0.0f)*scaleMod+scaleMod,
                      rotA,
                      sinf(rotBaseC + rotStepC*0.0f),
                      255u,
                      rotBaseHue + sinf(rotBaseB + rotStepB*0.0f)*hueStep
                      );

      rotA = sinf(rotBaseA + rotStepA*1.0f) * sM_PIf;
      TestText_3_draw(s,
                      baseScale - scaleStep*1.0f + sinf(rotBaseB + rotStepB*1.0f)*scaleMod+scaleMod,
                      rotA,
                      sinf(rotBaseC + rotStepC*1.0f),
                      175u,
                      rotBaseHue + sinf(rotBaseB + rotStepB*1.0f)*hueStep
                      );

      rotA = sinf(rotBaseA + rotStepA*2.0f) * sM_PIf;
      TestText_3_draw(s,
                      baseScale - scaleStep*2.0f + sinf(rotBaseB + rotStepB*2.0f)*scaleMod+scaleMod,
                      rotA,
                      sinf(rotBaseC + rotStepC*2.0f),
                      127u,
                      rotBaseHue + sinf(rotBaseB + rotStepB*2.0f)*hueStep
                      );

      rotA = sinf(rotBaseA + rotStepA*3.0f) * sM_PIf;
      TestText_3_draw(s,
                      baseScale - scaleStep*3.0f + sinf(rotBaseB + rotStepB*3.0f)*scaleMod+scaleMod,
                      rotA,
                      sinf(rotBaseC + rotStepC*3.0f),
                      79u,
                      rotBaseHue + sinf(rotBaseB + rotStepB*3.0f)*hueStep
                      );

      rotA = sinf(rotBaseA + rotStepA*4.0f) * sM_PIf;
      TestText_3_draw(s,
                      baseScale - scaleStep*4.0f + sinf(rotBaseB + rotStepB*4.0f)*scaleMod+scaleMod,
                      rotA,
                      sinf(rotBaseC + rotStepC*4.0f),
                      31u,
                      rotBaseHue + sinf(rotBaseB + rotStepB*4.0f)*45.0f
                      );

      rotA = sinf(rotBaseA + rotStepA*5.0f) * sM_PIf;
      TestText_3_draw(s,
                      baseScale - scaleStep*5.0f + sinf(rotBaseB + rotStepB*4.0f)*scaleMod+scaleMod,
                      rotA,
                      sinf(rotBaseC + rotStepC*5.0f),
                      18u,
                      rotBaseHue + sinf(rotBaseB + rotStepB*5.0f)*45.0f
                      );

      rotA = sinf(rotBaseA + rotStepA*6.0f) * sM_PIf;
      TestText_3_draw(s,
                      baseScale - scaleStep*6.0f + sinf(rotBaseB + rotStepB*4.0f)*scaleMod+scaleMod,
                      rotA,
                      sinf(rotBaseC + rotStepC*6.0f),
                      10u,
                      rotBaseHue + sinf(rotBaseB + rotStepB*6.0f)*45.0f
                      );

      sdvg_UnbindFont();
      sdvg_UnbindShader();
   }
}

// ---------------------------------------------------------------------------- TestText_4_Ul (56)
static void TestText_4_Ul(void) {
   sdvg_BindFont(&font, YAC_FALSE/*bFilter*/);
   sdvg_EnableBlending();

   const char *s = "hell\ao, wo\arld.\n\amulti\nlin\ae\nte\axt.";

   sSI minX, maxX, minY, maxY;
   sdvg_TextBBox(&font, s, &minX, &maxX, &minY, &maxY);
   sSI w = (maxX - minX);
   sSI h = (maxY - minY);

   sF32 x = sinf(ang_x*0.5f) * (640.0f-w)*0.5f + 320.0f - w*0.5f;
   sF32 y = sinf(ang_y*0.5f) * (480.0f-h)*0.5f + 240.0f - h*0.5f;

   x -= minX;
   y -= minY;

   // trace "xxx TestText_4_Ul: p=("+x+";"+y+") s=("+w+","+h+") minX="+minX+" minY="+minY;

   sdvg_SetFillColor4f(1.0f, 1.0f, 1.0f, fill_alpha*0.25f);
   sdvg_DrawTextUnderline(s, x, y);

   sdvg_SetFillColor4f(1.0f, 1.0f, 1.0f, fill_alpha);
   sdvg_DrawText(s, x, y);

   sdvg_UnbindFont();
}

// ---------------------------------------------------------------------------- TestText_5_Ul_Clip (57)
static void TestText_5_Ul_Clip(void) {
   sdvg_BindFont(&font, YAC_FALSE/*bFilter*/);
   sdvg_EnableBlending();

   const char *s = "############\ntext*text*text*text\nclip*clip*clip*clip*\ntest*test*test*test.\n############";

   sSI minX, maxX, minY, maxY;
   sdvg_TextBBox(&font, s, &minX, &maxX, &minY, &maxY);
   sSI w = (maxX - minX);
   sSI h = (maxY - minY);

   sF32 x = sinf(ang_x*0.5f) * (640.0f-w)*0.5f + 320.0f - w*0.5f - minX;
   sF32 y = sinf(ang_y*0.5f) * (480.0f-h)*0.5f + 240.0f - h*0.5f - minY;
   w += minX;
   h += minY;

   sF32 clipW = sinf(ang_w*8.0f) * w*0.5f + w*0.5f;
   sF32 clipH = sinf(ang_h*8.0f) * h*0.5f + h*0.5f;

   sF32 clipLeft  = x + w*0.5f - clipW*0.5f;
   sF32 clipRight = clipLeft + clipW;

   sF32 clipTop    = y + h*0.5f - clipH*0.5f;
   sF32 clipBottom = clipTop + clipH;

   sdvg_SetFillColor4f(1.0f, 1.0f, 1.0f, fill_alpha*0.25f);
   sdvg_DrawTextUnderlineClipped(s, x, y, clipLeft, clipTop, clipRight, clipBottom);

   sdvg_SetFillColor4f(1.0f, 1.0f, 1.0f, fill_alpha);
   sdvg_DrawTextClipped(s, x, y, clipLeft, clipTop, clipRight, clipBottom);

   sdvg_UnbindFont();
}

// ---------------------------------------------------------------------------- TestText_6_Accel (58)
static void TestText_6_Accel(void) {
   sdvg_BindFont(&font, YAC_FALSE/*bFilter*/);
   sdvg_EnableBlending();

   const char *s = "hell\ao, wo\arld.\n\amulti\nlin\ae\nte\axt.";

   sSI minX, maxX, minY, maxY;
   sdvg_TextBBox(&font, s, &minX, &maxX, &minY, &maxY);
   sSI w = (maxX - minX);
   sSI h = (maxY - minY);

   sF32 x = sinf(ang_x*0.5f) * (640.0f-w)*0.5f + 320.0f - w*0.5f;
   sF32 y = sinf(ang_y*0.5f) * (480.0f-h)*0.5f + 240.0f - h*0.5f;

   x -= minX;
   y -= minY;

   sdvg_SetFillColor4f(1.0f, 1.0f, 1.0f, fill_alpha*0.5f);
   sdvg_DrawTextAccel(s, x, y);

   sdvg_SetFillColor4f(1.0f, 1.0f, 1.0f, fill_alpha);
   sdvg_DrawText(s, x, y);

   sdvg_UnbindFont();
}

// ---------------------------------------------------------------------------- TestText_7_Accel_Clip (59)
static void TestText_7_Accel_Clip(void) {
   sdvg_BindFont(&font, YAC_FALSE/*bFilter*/);
   sdvg_EnableBlending();

   const char *s = "############\n\atext*t\aext*te\axt*tex\at\n\aclip*c\alip*cl\aip*cli\ap*\n\atest*t\aest*te\ast*tes\at.\n############";

   sSI minX, maxX, minY, maxY;
   sdvg_TextBBox(&font, s, &minX, &maxX, &minY, &maxY);
   sSI w = (maxX - minX);
   sSI h = (maxY - minY);

   sF32 x = sinf(ang_x*0.5f) * (640.0f-w)*0.5f + 320.0f - w*0.5f - minX;
   sF32 y = sinf(ang_y*0.5f) * (480.0f-h)*0.5f + 240.0f - h*0.5f - minY;
   w += minX;
   h += minY;

   sF32 clipW = sinf(ang_w*8.0f) * w*0.5f + w*0.5f;
   sF32 clipH = sinf(ang_h*8.0f) * h*0.5f + h*0.5f;

   sF32 clipLeft  = x + w*0.5f - clipW*0.5f;
   sF32 clipRight = clipLeft + clipW;

   sF32 clipTop    = y + h*0.5f - clipH*0.5f;
   sF32 clipBottom = clipTop + clipH;

   sdvg_SetFillColor4f(1.0f, 1.0f, 1.0f, fill_alpha*0.5f);
   sdvg_DrawTextAccelClipped(s, x, y, clipLeft, clipTop, clipRight, clipBottom);

   sdvg_SetFillColor4f(1.0f, 1.0f, 1.0f, fill_alpha);
   sdvg_DrawTextClipped(s, x, y, clipLeft, clipTop, clipRight, clipBottom);

   sdvg_UnbindFont();
}

// ---------------------------------------------------------------------------- TestText_8_Zoom (60+61)
static void TestText_8_Zoom(sBool _bSDF) {
   sdvg_font_t *f = _bSDF ? &font_zoom_sdf : &font_zoom;
   sdvg_BindFont(f, YAC_TRUE/*bFilter*/);
   sdvg_EnableBlending();
   sdvg_SetFillColor4f(1.0f, 1.0f, 1.0f, fill_alpha);

   const char *s = "Hello, world.\nmulti-line\ntext-test.";

   sSI minX, maxX, minY, maxY;
   sdvg_TextBBox(f, s, &minX, &maxX, &minY, &maxY);
   sSI w = (maxX - minX);
   sSI h = (maxY - minY);

   sF32 x = w * -0.5f;
   sF32 y = h * -0.5f;

   sF32 zoomRel = sinf(ang_x*0.5f) * 0.5f + 0.5f;
   sF32 zoom = (1.0f + zoomRel * 8.0f);

   if(_bSDF)
   {
      sdvg_SetAlphaSDFRange( sLERP(0.83f, 0.90f, 1.0f - zoomRel),
                             sLERP(0.99f, 0.98f, 1.0f - zoomRel)
                             );
      sdvg_SetAlphaSDFExp( sLERP(0.7f, 0.5f, 1.0f - zoomRel) );
   }

   sF32 shift = sinf(ang_y)*200.0f + 200.0f;

   sF32 g = 20.0f*8.0f;
   sF32 aspect = ((sF32)(VP_W)) / VP_H;
   Matrix4f mProj;
   minnie_matrix4f_initOrtho(&mProj,
                             -g * zoom * aspect, g * zoom * aspect,
                             g * zoom/*bottom*/,  -g * zoom/*top*/,
                             0.0f/*znear*/,  10.0f/*zfar*/
                             );
   minnie_matrix4f_rotatef(&mProj, 0.0f, 0.0f, sinf(ang_h*4.0f)*(sM_PIf*0.25f));
   minnie_matrix4f_translatef(&mProj, shift, shift, 0.0f);
   sdvg_SetTransform(&mProj);

   sdvg_DrawText(s, x - minX, y - minY);

   sdvg_UnbindFont();
}

// ---------------------------------------------------------------------------- TestGradientToTexture (62)
static void TestGradientToTexture(void) {

   sF32 start1 = sinf(ang_x) * 200.0f + 220.0f;
   sF32 len1   = sinf(ang_y) * 500.0f + 520.0f;
   sF32 len2   = sinf(ang_w) * 300.0f + 320.0f;
   sF32 len3   = sinf(ang_h) * 400.0f + 420.0f;

   gradient_starts[1] = (sSI)(start1);
   gradient_starts[2] = (sSI)(start1 + len1);
   gradient_starts[3] = (sSI)(start1 + len1 + len2);
   gradient_starts[4] = (sSI)(start1 + len1 + len2 + len3);

   sdvg_BindTexture2D(tex_gradient_id, YAC_FALSE/*bRepeat*/, YAC_TRUE/*bFilter*/);
   sdvg_GradientToTexture(tex_gradient, 256u,
                          gradient_colors, 5u,
                          gradient_starts, 5u,
                          YAC_TRUE/*bSmoothStep*/
                          );
   sdvg_UpdateTexture2D(SDVG_TEXFMT_ARGB32, 256u, 1u,
                        tex_gradient, 256u*sizeof(sU32)
                        );

   sdvg_SetFillColor4f(1.0f, 1.0f, 1.0f, fill_alpha);
   sF32 w = 240.0f;
   sF32 h = 180.0f;
   sF32 x = (640.0f-w)*0.5f;
   sF32 y = (480.0f-h)*0.5f;
   sdvg_BindShader(0u);  // use built-in shader
   if(sdvg_BeginTexturedTriangles(2u*3u))
   {
      // 1
      sdvg_TexCoord2f(0.0f, 0.0f);
      sdvg_Vertex2f(x, y);

      sdvg_TexCoord2f(1.0f, 0.0f);
      sdvg_Vertex2f(x+w, y);

      sdvg_TexCoord2f(1.0f, 1.0f);
      sdvg_Vertex2f(x+w, y+h);

      // 2
      sdvg_TexCoord2f(0.0f, 0.0f);
      sdvg_Vertex2f(x, y);

      sdvg_TexCoord2f(1.0f, 1.0f);
      sdvg_Vertex2f(x+w, y+h);

      sdvg_TexCoord2f(0.0f, 1.0f);
      sdvg_Vertex2f(x, y+h);

      sdvg_End();
   }
   sdvg_UnbindTexture2D();
}

// ---------------------------------------------------------------------------- SetupGradient*
void SetupGradientLinear(void) {
   sF32 pdx = VP_W;
   sF32 pdy = VP_H;
   sF32 sa = sinf(ang_w*0.5f);
   sF32 ca = cosf(ang_w*0.5f);
   sF32 rx = pdx * ca - pdy * sa;
   sF32 ry = pdx * sa + pdy * ca;

   sdvg_PaintLinear(VP_W*0.5f, VP_H*0.5f, rx, ry);
   sdvg_BindTexture2D(tex_gradient_id, YAC_TRUE/*bRepeat*/, YAC_TRUE/*bFilter*/);
}

void SetupGradientRadial(void) {
   sF32 px = sinf(ang_w*2.0f)*(VP_W*0.5f) + (VP_W*0.5f);
   sF32 py = sinf(ang_h*2.0f)*(VP_H*0.5f) + (VP_H*0.5f);

   sdvg_PaintRadial(px, py, VP_W, VP_H);
   sdvg_BindTexture2D(tex_gradient_id, YAC_TRUE/*bRepeat*/, YAC_TRUE/*bFilter*/);
}

void SetupGradientConic(void) {
   sF32 px = sinf(ang_w*2.0f)*(VP_W*0.5f) + (VP_W*0.5f);
   sF32 py = sinf(ang_h*2.0f)*(VP_H*0.5f) + (VP_H*0.5f);

   sF32 ang = sWRAP((ang_c * 8.0f) / sM_2PIf, 0.0f, 1.0f);

   sdvg_PaintConic(px, py, VP_W, VP_H, ang);
   sdvg_BindTexture2D(tex_gradient_id, YAC_TRUE/*bRepeat*/, YAC_TRUE/*bFilter*/);
}

void SetupPaintPattern(void) {
   sF32 zoom = sinf(ang_y*0.5f) * 7.0f + 8.0f;
   sF32 rx = sinf(ang_x*0.5f) * zoom;
   sF32 ry = cosf(ang_x*0.5f) * zoom;

   sF32 x = VP_W*0.5f;
   sF32 y = VP_H*0.5f;

   sdvg_PaintPattern(x, y, x+rx, y+ry, 256.0f, 256.0f);
   sdvg_BindTexture2D(tex_id, YAC_TRUE/*bRepeat*/, YAC_TRUE/*bFilter*/);
}

void SetupPaintPatternAlpha(void) {
   sF32 zoom = sinf(ang_y*0.5f) * 2.0f + 3.0f;
   sF32 rx = sinf(ang_x*0.5f) * zoom;
   sF32 ry = cosf(ang_x*0.5f) * zoom;

   sF32 x = VP_W*0.5f;
   sF32 y = VP_H*0.5f;

   sdvg_PaintPatternAlpha(x, y, x+rx, y+ry, 256.0f, 256.0f);
   sdvg_BindTexture2D(tex_pattern_1_alpha_id, YAC_TRUE/*bRepeat*/, YAC_TRUE/*bFilter*/);
}

void SetupPaintPatternDecal(void) {
   sF32 zoom = sinf(ang_y*0.5f) * 7.0f + 8.0f;
   sF32 rx = sinf(ang_x*0.5f) * zoom;
   sF32 ry = cosf(ang_x*0.5f) * zoom;

   sF32 x = VP_W*0.5f;
   sF32 y = VP_H*0.5f;

   sdvg_PaintPatternDecal(x, y, x+rx, y+ry, 256.0f, 256.0f);
   sdvg_BindTexture2D(tex_id, YAC_TRUE/*bRepeat*/, YAC_TRUE/*bFilter*/);
}

void SetupPaintPatternDecalAlpha(void) {
   sF32 zoom = sin(ang_y*0.5f) * 2.0f + 3.5f;
   sF32 rx = sinf(ang_x*0.5f) * zoom;
   sF32 ry = cosf(ang_x*0.5f) * zoom;

   sF32 x = VP_W*0.5f;
   sF32 y = VP_H*0.5f;

   sdvg_PaintPatternDecalAlpha(x, y, x+rx, y+ry, 256.0f, 256.0f);
   sdvg_BindTexture2D(tex_pattern_1_alpha_id, YAC_TRUE/*bRepeat*/, YAC_TRUE/*bFilter*/);
}

// ---------------------------------------------------------------------------- DrawPaintBackground
void DrawPaintBackground(void) {
   if(sdvg_BeginFilledTriangles(2u*3u))
   {
      // 1
      sdvg_Vertex2f(0.0f, 0.0f);
      sdvg_Vertex2f(VP_W, 0.0f);
      sdvg_Vertex2f(VP_W, VP_H);

      // 2
      sdvg_Vertex2f(0.0f, 0.0f);
      sdvg_Vertex2f(VP_W, VP_H);
      sdvg_Vertex2f(0.0f, VP_H);

      sdvg_End();
   }
}

// ---------------------------------------------------------------------------- TestTrianglesGradientLinear (63)
static void TestTrianglesGradientLinear(void) {

   SetupGradientLinear();
   sdvg_BindShader(0u);  // use built-in shader
   sdvg_SetFillColor4f(0.5f, 0.5f, 0.5f, 1.0f);
   DrawPaintBackground();

   sF32 w = 400.0f;
   sF32 h = 300.0f;
   sF32 x = sinf(ang_x * 0.5f) * 200.0f + (VP_W*0.5f) - w*0.5f;
   sF32 y = cosf(ang_y * 0.5f) * 200.0f + (VP_H*0.5f) - h*0.5f;

   sdvg_BindShader(0u);  // use built-in shader

   sdvg_SetFillColor4f(1.0f, 1.0f, 1.0f, fill_alpha);
   if(sdvg_BeginFilledTriangles(2u*3u))
   {
      // 1
      sdvg_Vertex2f(x, y);
      sdvg_Vertex2f(x+w, y);
      sdvg_Vertex2f(x+w, y+h);

      // 2
      sdvg_Vertex2f(x, y);
      sdvg_Vertex2f(x+w, y+h);
      sdvg_Vertex2f(x, y+h);

      sdvg_End();
   }
}

// ---------------------------------------------------------------------------- TestTrianglesGradientRadial (64)
static void TestTrianglesGradientRadial(void) {

   SetupGradientRadial();
   sdvg_BindShader(0u);  // use built-in shader
   sdvg_SetFillColor4f(0.5f, 0.5f, 0.5f, 1.0f);
   DrawPaintBackground();

   sF32 w = 400.0f;
   sF32 h = 300.0f;
   sF32 x = sinf(ang_x * 0.5f) * 200.0f + (VP_W*0.5f) - w*0.5f;
   sF32 y = cosf(ang_y * 0.5f) * 200.0f + (VP_H*0.5f) - h*0.5f;

   sdvg_BindShader(0u);  // use built-in shader

   sdvg_SetFillColor4f(1.0f, 1.0f, 1.0f, fill_alpha);
   if(sdvg_BeginFilledTriangles(2u*3u))
   {
      // 1
      sdvg_Vertex2f(x, y);
      sdvg_Vertex2f(x+w, y);
      sdvg_Vertex2f(x+w, y+h);

      // 2
      sdvg_Vertex2f(x, y);
      sdvg_Vertex2f(x+w, y+h);
      sdvg_Vertex2f(x, y+h);

      sdvg_End();
   }
}

// ---------------------------------------------------------------------------- TestTrianglesGradientConic (65)
static void TestTrianglesGradientConic(void) {

   SetupGradientConic();
   sdvg_BindShader(0u);  // use built-in shader
   sdvg_SetFillColor4f(0.5f, 0.5f, 0.5f, 1.0f);
   DrawPaintBackground();

   sF32 w = 400.0f;
   sF32 h = 300.0f;
   sF32 x = sinf(ang_x * 0.5f) * 200.0f + (VP_W*0.5f) - w*0.5f;
   sF32 y = cosf(ang_y * 0.5f) * 200.0f + (VP_H*0.5f) - h*0.5f;

   sdvg_BindShader(0u);  // use built-in shader

   sdvg_SetFillColor4f(1.0f, 1.0f, 1.0f, fill_alpha);
   if(sdvg_BeginFilledTriangles(2u*3u))
   {
      // 1
      sdvg_Vertex2f(x, y);
      sdvg_Vertex2f(x+w, y);
      sdvg_Vertex2f(x+w, y+h);

      // 2
      sdvg_Vertex2f(x, y);
      sdvg_Vertex2f(x+w, y+h);
      sdvg_Vertex2f(x, y+h);

      sdvg_End();
   }
}

// ---------------------------------------------------------------------------- TestPolygon_VBO (66)
static void TestPolygon_SetupMVP(void) {
   Matrix4f *m = sdvg_GetTransformRef();
   minnie_matrix4f_translatef(m, VP_W*0.5f, VP_H*0.5f, 0.0f);
   minnie_matrix4f_rotatef(m, 0.0f, 0.0f, ang_w);
   minnie_matrix4f_translatef(m, -VP_W*0.5f, -VP_H*0.5f, 0.0f);
}

static sUI polygon_vbo_id = 0u;
static void TestPolygon_VBO(sBool _bAA) {
   if(0u == polygon_vbo_id)
   {
      // Create + initialize VBO
      polygon_vbo_id = sdvg_CreateVBO((8u+1u)*2u*sizeof(sF32));
      sdvg_MapVBO(polygon_vbo_id);
      if(sdvg_BeginVBO(8u+1u, 2u*sizeof(sF32)))
      {
         sdvg_Vertex2f( 30.0f,  90.0f);
         sdvg_Vertex2f(320.0f,  20.0f);
         sdvg_Vertex2f(640.0f,  80.0f);
         sdvg_Vertex2f(600.0f, 200.0f);
         sdvg_Vertex2f(300.0f, 220.0f);
         sdvg_Vertex2f(500.0f, 360.0f);
         sdvg_Vertex2f(300.0f, 470.0f);
         sdvg_Vertex2f(120.0f, 300.0f);
         sdvg_Vertex2f( 30.0f,  90.0f);  // closed polyline, AA only

         sdvg_End();
      }
      sdvg_UnmapVBO();
   }
   if(polygon_vbo_id >= 1u)
   {
      sdvg_EnableBlending();

      sdvg_SetFillColor4f(0.1f, 0.7f, _bAA ? 0.7f : 0.6f, fill_alpha);

      // Draw pre-initialized VBO
      sdvg_DrawPolygonFillFlatVBO32(polygon_vbo_id, 0u/*byteOffset*/, 8u/*numVerts*/);
      sdvg_BindVBO(0);

      if(_bAA)
      {
         // Draw AA outline
         sdvg_SetStrokeWidth(1.0f);
         if(sdvg_GetEnableDebug())
            sdvg_SetStrokeColor4f(1.0f, 1.0f, 1.0f, fill_alpha);
         else
            sdvg_SetStrokeColor4f(0.1f, 0.7f, 0.7f, fill_alpha);
         sdvg_DrawLineStripFlatAAVBO32(polygon_vbo_id, 0/*byteOffset*/, 8+1/*numVerts*/);
      }
   }
}

// ---------------------------------------------------------------------------- TestBeginPolygon (68+69)
static void TestBeginPolygon(sBool _bAA) {
   if(_bAA
      ? sdvg_BeginFilledPolygonAA(8u)
      : sdvg_BeginFilledPolygon(8u)
      )
   {
      sdvg_Vertex2f( 30.0f,  90.0f);
      sdvg_Vertex2f(320.0f,  20.0f);
      sdvg_Vertex2f(640.0f,  80.0f);
      sdvg_Vertex2f(600.0f, 200.0f);
      sdvg_Vertex2f(300.0f, 220.0f);
      sdvg_Vertex2f(500.0f, 360.0f);
      sdvg_Vertex2f(300.0f, 470.0f);
      sdvg_Vertex2f(120.0f, 300.0f);

      sdvg_End();
   }
}

// ---------------------------------------------------------------------------- hal_on_draw
void hal_on_draw(void) {

   sU32 ticks = SDL_GetTicks();
   // Dprintf("xxx ticks=%u last_sdl_ticks=%u\n", ticks, last_sdl_ticks);
   sF32 dt;
   if(0u != last_sdl_ticks)
      dt = (ticks - last_sdl_ticks) / (1000.0f / 60.0f);
   else
      dt = 01.0f;
   last_sdl_ticks = ticks;
   // Dprintf("xxx hal_on_draw dt=%f\n", dt);

   sdvg_SetFramebufferSize(VP_W, VP_H);
   sdvg_BeginFrame();

   sdvg_SetEnableAA(b_aa);
   sdvg_SetAARange(b_aa ? aa_range : 0.01f);
   sdvg_SetAAExp(aa_exp);
   sdvg_SetStrokeWidth(stroke_w);
   sdvg_SetFillColor4f(0.1f, 0.4f, 0.25f, fill_alpha);
   sdvg_SetStrokeColor4f(1.0f, 1.0f, 1.0f, fill_alpha);
   sdvg_SetGlobalAlpha(1.0f);
   sdvg_PaintSolid();

   sdvg_ClearARGB(0xff000064u);

   Matrix4f *mProj = sdvg_GetTransformRef();
   sF32 aspect = (((sF32)(VP_W)) / VP_H) / 1.333f;
   sF32 extra = ((VP_W*aspect) - VP_W) * 0.5f;
   minnie_matrix4f_initOrtho(mProj,
                             0.0f-extra/*left*/, VP_W+extra/*right*/,
                             VP_H/*bottom*/,  0.0f/*top*/,
                             0.0f/*znear*/,  10.0f/*zfar*/
                             );

   sF32 centerX = VP_W * 0.5f;
   sF32 centerY = VP_H * 0.5f;

   sF32 sizeX = VP_W*0.25f + 70.0f*sinf(ang_w);
   sF32 sizeY = VP_H*0.25f + 70.0f*sinf(ang_h);

   sF32 cornerX = 80.0f + sinf(ang_x)*40.0f;
   sF32 cornerY = 80.0f + sinf(ang_y)*40.0f;

   if(b_sym_radius)
   {
      cornerX = sMIN(cornerX, cornerY);
      cornerY = cornerX;
   }

   switch(render_mode)
   {
      default:
         break;

      case RENDER_RECT_FILL_AA: // 0
         sdvg_DrawRectFillAA(centerX, centerY,
                             sizeX * size_sclx, sizeY * size_scly
                             );
         break;

      case RENDER_RECT_FILL_STROKE_AA: // 1
         sdvg_DrawRectFillStrokeAA(centerX, centerY,
                                   sizeX * size_sclx, sizeY * size_scly
                                   );
         break;

      case RENDER_RECT_STROKE_AA: // 2
         sdvg_DrawRectStrokeAA(centerX, centerY,
                               sizeX * size_sclx, sizeY * size_scly
                               );
         break;

      case RENDER_ELLIPSE_FILL_AA: // 3
         sdvg_DrawEllipseFillAA(centerX, centerY,
                                sizeX * size_sclx, sizeY * size_scly
                                );
         break;

      case RENDER_ELLIPSE_FILL_STROKE_AA: // 4
         sdvg_DrawEllipseFillStrokeAA(centerX, centerY,
                                      sizeX * size_sclx, sizeY * size_scly
                                      );
         break;

      case RENDER_ELLIPSE_STROKE_AA: // 5
         sdvg_DrawEllipseStrokeAA(centerX, centerY,
                                  sizeX * size_sclx, sizeY * size_scly
                                  );
         break;

      case RENDER_ROUNDRECT_FILL_AA: // 6
         sdvg_DrawRoundRectFillAA(centerX, centerY,
                                  sizeX * size_sclx, sizeY * size_scly,
                                  cornerX * radius_sclx, cornerY * radius_scly
                                  );
         break;

      case RENDER_ROUNDRECT_FILL_STROKE_AA: // 7
         sdvg_DrawRoundRectFillStrokeAA(centerX, centerY,
                                        sizeX * size_sclx, sizeY * size_scly,
                                        cornerX * radius_sclx, cornerY * radius_scly
                                        );
         break;

      case RENDER_ROUNDRECT_STROKE_AA: // 8
         sdvg_DrawRoundRectStrokeAA(centerX, centerY,
                                    sizeX * size_sclx, sizeY * size_scly,
                                    cornerX * radius_sclx, cornerY * radius_scly
                                    );
         break;

      case RENDER_TRIANGLES_TEX_UV_FLAT: // 9
         buf_vbo.io_offset = 0u;
         sdvg_BufferAddRectTexUVFlat32(&buf_vbo,
                                       centerX - sizeX * size_sclx, centerY - sizeY * size_scly,
                                       sizeX * size_sclx * 2.0f,    sizeY * size_scly * 2.0f,
                                       0.0f, 0.0f,
                                       1.0f, 1.0f
                                       );
         sdvg_UpdateVBO(buf_vbo_id, 0u/*offset*/, buf_vbo.io_offset/*size*/, &buf_vbo);
         sdvg_BindTexture2D(tex_id, YAC_FALSE/*bRepeat*/, YAC_TRUE/*bFilter*/);
         sdvg_SetFillColor4f(1.0f, 1.0f, 1.0f, fill_alpha);
         sdvg_DrawTrianglesTexUVFlatVBO32(buf_vbo_id, 0u, 2u*3u);
         sdvg_UnbindTexture2D();
         break;

      case RENDER_TRIANGLES_TEX_UV_GOURAUD: // 10
         buf_vbo.io_offset = 0u;
         sdvg_BufferAddRectTexUVGouraud32(&buf_vbo,
                                          centerX - sizeX * size_sclx, centerY - sizeY * size_scly,
                                          sizeX * size_sclx * 2.0f,    sizeY * size_scly * 2.0f,
                                          0.0f, 0.0f,
                                          1.0f, 1.0f,
                                          0xffffffffu, 0xFfff0000u,
                                          0x3f00ff00u, 0x3f0000ffu
                                         );
         sdvg_UpdateVBO(buf_vbo_id, 0u/*offset*/, 0u/*numBytes=buf_vbo.size*/, &buf_vbo);
         sdvg_BindTexture2D(tex_id, YAC_FALSE/*bRepeat*/, YAC_TRUE/*bFilter*/);
         sdvg_SetFillColor4f(1.0f, 1.0f, 1.0f, fill_alpha);
         sdvg_DrawTrianglesTexUVGouraudVBO32(buf_vbo_id, 0u, 2u*3u);
         sdvg_UnbindTexture2D();
         break;

      case RENDER_TRIANGLES_TEX_UV_FLAT_DECAL: // 1
         buf_vbo.io_offset = 0u;
         sdvg_BufferAddRectTexUVFlat32(&buf_vbo,
                                       centerX - sizeX * size_sclx, centerY - sizeY * size_scly,
                                       sizeX * size_sclx * 2.0f,    sizeY * size_scly * 2.0f,
                                       0.0f, 0.0f,
                                       1.0f, 1.0f
                                       );
         sdvg_UpdateVBO(buf_vbo_id, 0u/*offset*/, 0u/*numBytes=buf_vbo.size*/, &buf_vbo);
         sdvg_BindTexture2D(tex_id, YAC_FALSE/*bRepeat*/, YAC_TRUE/*bFilter*/);
         sdvg_SetTextureDecalAlpha(decal_alpha);
         sdvg_DrawTrianglesTexUVFlatDecalVBO32(buf_vbo_id, 0u, 2u*3u);
         sdvg_UnbindTexture2D();
         break;

      case RENDER_TRIANGLES_TEX_UV_GOURAUD_DECAL: // 12
         buf_vbo.io_offset = 0u;
         sdvg_BufferAddRectTexUVGouraud32(&buf_vbo,
                                          centerX - sizeX * size_sclx, centerY - sizeY * size_scly,
                                          sizeX * size_sclx * 2.0f,    sizeY * size_scly * 2.0f,
                                          0.0f, 0.0f,
                                          1.0f, 1.0f,
                                          0xffffffffu, 0xFfff0000u,
                                          0x3f00ff00u, 0x3f0000ffu
                                         );
         sdvg_UpdateVBO(buf_vbo_id, 0u/*offset*/, 0u/*numBytes=buf_vbo.size*/, &buf_vbo);
         sdvg_BindTexture2D(tex_id, YAC_FALSE/*bRepeat*/, YAC_TRUE/*bFilter*/);
         sdvg_SetTextureDecalAlpha(decal_alpha);
         sdvg_DrawTrianglesTexUVGouraudDecalVBO32(buf_vbo_id, 0u, 2u*3u);
         sdvg_UnbindTexture2D();
         break;

      case RENDER_LINE_STRIP_FLAT_1: // 13
         TestLineStripFlat_1(YAC_FALSE/*bAA*/);
         break;

      case RENDER_LINE_STRIP_FLAT_2: // 14
         TestLineStripFlat_2(YAC_FALSE/*bAA*/);
         break;

      case RENDER_LINE_STRIP_FLAT_AA_1: // 15
         TestLineStripFlat_1(YAC_TRUE/*bAA*/);
         break;

      case RENDER_LINE_STRIP_FLAT_AA_2: // 16
         TestLineStripFlat_2(YAC_TRUE/*bAA*/);
         break;

      case RENDER_LINE_STRIP_FLAT_BEVEL: // 17
         TestLineStripFlatBevel(YAC_FALSE/*bAA*/);
         break;

      case RENDER_LINE_STRIP_FLAT_BEVEL_AA: // 18
         TestLineStripFlatBevel(YAC_TRUE/*bAA*/);
         break;

      case RENDER_CUSTOM_SHADER_1: // 19
         TestCustomShader_1(sizeX, sizeY);
         break;

      case RENDER_CUSTOM_SHADER_2: // 20
         TestCustomShader_2(sizeX, sizeY);
         break;

      case RENDER_CUSTOM_SHADER_3_VBO: // 21
         TestCustomShader_3_VBO(mProj, sizeX, sizeY);
         break;

      case RENDER_LINES: // 22
         TestLinesFlat(YAC_FALSE/*bAA*/);
         break;

      case RENDER_LINES_AA: // 23
         TestLinesFlat(YAC_TRUE/*bAA*/);
         break;

      case RENDER_BEGIN_LINE_STRIP: // 24
         TestBeginLineStripFlat(YAC_FALSE/*bAA*/);
         break;

      case RENDER_BEGIN_LINE_STRIP_AA: // 25
         TestBeginLineStripFlat(YAC_TRUE/*bAA*/);
         break;

      case RENDER_BEGIN_LINE_STRIP_BEVEL: // 26
         TestBeginLineStripFlatBevel(YAC_FALSE/*bAA*/);
         break;

      case RENDER_BEGIN_LINE_STRIP_BEVEL_AA: // 27
         TestBeginLineStripFlatBevel(YAC_TRUE/*bAA*/);
         break;

      case RENDER_BEGIN_LINES: // 28
         TestBeginLinesFlat(YAC_FALSE/*bAA*/);
         break;

      case RENDER_BEGIN_LINES_AA: // 29
         TestBeginLinesFlat(YAC_TRUE/*bAA*/);
         break;

      case RENDER_FILLED_RECTANGLE: // 30
         sdvg_SetFillColor4f(1.0f, 1.0f, 1.0f, fill_alpha);
         sdvg_DrawFilledRectangle(centerX - sizeX * size_sclx * 0.5f,
                                  centerY - sizeY * size_scly * 0.5f,
                                  sizeX * size_sclx,
                                  sizeY * size_scly
                                  );
         break;

      case RENDER_BEGIN_POINTS_SQUARE: // 31
         TestBeginPointsSquare(YAC_FALSE/*bAA*/);
         break;

      case RENDER_BEGIN_POINTS_SQUARE_AA: // 32
         TestBeginPointsSquare(YAC_TRUE/*bAA*/);
         break;

      case RENDER_BEGIN_POINTS_ROUND: // 33
         TestBeginPointsRound(YAC_FALSE/*bAA*/);
         break;

      case RENDER_BEGIN_POINTS_ROUND_AA: // 34
         TestBeginPointsRound(YAC_TRUE/*bAA*/);
         break;

      case RENDER_BEGIN_FILLED_TRIANGLES: // 35
         TestBeginFilledTriangles();
         break;

      case RENDER_BEGIN_FILLED_TRIANGLE_FAN: // 36
         TestBeginFilledTriangleFan();
         break;

      case RENDER_BEGIN_FILLED_TRIANGLE_STRIP: // 37
         TestBeginFilledTriangleStrip();
         break;

      case RENDER_BEGIN_FILLED_GOURAUD_TRIANGLES: // 38
         TestBeginFilledGouraudTriangles();
         break;

      case RENDER_BEGIN_FILLED_GOURAUD_TRIANGLE_FAN: // 39
         TestBeginFilledGouraudTriangleFan();
         break;

      case RENDER_BEGIN_FILLED_GOURAUD_TRIANGLE_STRIP: // 40
         TestBeginFilledGouraudTriangleStrip();
         break;

      case RENDER_BEGIN_TEXTURED_TRIANGLES: // 41
         TestBeginTexturedTriangles(YAC_FALSE/*bAlpha*/);
         break;

      case RENDER_BEGIN_TEXTURED_TRIANGLES_ALPHA: // 42
         TestBeginTexturedTriangles(YAC_TRUE/*bAlpha*/);
         break;

      case RENDER_BEGIN_TEXTURED_TRIANGLE_FAN: // 43
         TestBeginTexturedTriangleFan(YAC_FALSE/*bAlpha*/);
         break;

      case RENDER_BEGIN_TEXTURED_TRIANGLE_FAN_ALPHA: // 44
         TestBeginTexturedTriangleFan(YAC_TRUE/*bAlpha*/);
         break;

      case RENDER_BEGIN_TEXTURED_TRIANGLE_STRIP: // 45
         TestBeginTexturedTriangleStrip(YAC_FALSE/*bAlpha*/);
         break;
      case RENDER_BEGIN_TEXTURED_TRIANGLE_STRIP_ALPHA: // 46
         TestBeginTexturedTriangleStrip(YAC_TRUE/*bAlpha*/);
         break;

      case RENDER_BEGIN_TEXTURED_GOURAUD_TRIANGLES: // 47
         TestBeginTexturedGouraudTriangles(YAC_FALSE/*bAlpha*/);
         break;

      case RENDER_BEGIN_TEXTURED_GOURAUD_TRIANGLES_ALPHA: // 48
         TestBeginTexturedGouraudTriangles(YAC_TRUE/*bAlpha*/);
         break;

      case RENDER_BEGIN_TEXTURED_GOURAUD_TRIANGLE_FAN: // 49
         TestBeginTexturedGouraudTriangleFan(YAC_FALSE/*bAlpha*/);
         break;

      case RENDER_BEGIN_TEXTURED_GOURAUD_TRIANGLE_FAN_ALPHA: // 50
         TestBeginTexturedGouraudTriangleFan(YAC_TRUE/*bAlpha*/);
         break;

      case RENDER_BEGIN_TEXTURED_GOURAUD_TRIANGLE_STRIP: // 51
         TestBeginTexturedGouraudTriangleStrip(YAC_FALSE/*bAlpha*/);
         break;

      case RENDER_BEGIN_TEXTURED_GOURAUD_TRIANGLE_STRIP_ALPHA: // 52
         TestBeginTexturedGouraudTriangleStrip(YAC_TRUE/*bAlpha*/);
         break;

      case RENDER_TEXT_1: // 53
         TestText_1();
         break;

      case RENDER_TEXT_2_CLIP: // 54
         TestText_2_Clip();
         break;

      case RENDER_TEXT_3_SWIRL: // 55
         TestText_3_Swirl();
         break;

      case RENDER_TEXT_4_UL: // 56
         TestText_4_Ul();
         break;

      case RENDER_TEXT_5_UL_CLIP: // 57
         TestText_5_Ul_Clip();
         break;

      case RENDER_TEXT_6_ACCEL: // 58
         TestText_6_Accel();
         break;

      case RENDER_TEXT_7_ACCEL_CLIP: // 59
         TestText_7_Accel_Clip();
         break;

      case RENDER_TEXT_8_ZOOM: // 60
         TestText_8_Zoom(YAC_FALSE/*bSDF*/);
         break;

      case RENDER_TEXT_9_ZOOM_SDF: // 61
         TestText_8_Zoom(YAC_TRUE/*bSDF*/);
         break;

      case RENDER_GRADIENT_TO_TEXTURE: // 62
         TestGradientToTexture();
         break;

      case RENDER_TRIANGLES_GRADIENT_LINEAR: // 63
         TestTrianglesGradientLinear();
         break;

      case RENDER_TRIANGLES_GRADIENT_RADIAL: // 64
         TestTrianglesGradientRadial();
         break;

      case RENDER_TRIANGLES_GRADIENT_CONIC: // 65
         TestTrianglesGradientConic();
         break;

      case RENDER_POLYGON_VBO: // 66
         sdvg_BindShader(0u);  // use built-in shader
         sdvg_EnableBlending();
         sdvg_PaintSolid();
         TestPolygon_SetupMVP();
         TestPolygon_VBO(YAC_FALSE/*bAA*/);
         break;

      case RENDER_POLYGON_AA_VBO: // 67
         sdvg_BindShader(0u);  // use built-in shader
         sdvg_EnableBlending();
         TestPolygon_SetupMVP();
         TestPolygon_VBO(YAC_TRUE/*bAA*/);
         break;

      case RENDER_BEGIN_POLYGON: // 68
         TestPolygon_SetupMVP();
         sdvg_BindShader(0u);  // use built-in shader
         sdvg_EnableBlending();
         sdvg_SetColor4f(0.1f, 0.7f, 0.6f, fill_alpha);
         TestBeginPolygon(YAC_FALSE/*bAA*/);
         break;

      case RENDER_BEGIN_POLYGON_AA: // 69
         sdvg_BindShader(0u);  // use built-in shader
         sdvg_EnableBlending();
         TestPolygon_SetupMVP();
         sdvg_SetColor4f(0.1f, 0.7f, 0.7f, fill_alpha);
         TestBeginPolygon(YAC_TRUE/*bAA*/);
         break;

      case RENDER_BEGIN_POLYGON_GRADIENT_LINEAR: // 70
         sdvg_BindShader(0u);  // use built-in shader
         sdvg_EnableBlending();
         SetupGradientLinear();
         sdvg_SetFillColor4f(0.5f, 0.5f, 0.5f, 1.0f);
         DrawPaintBackground();
         TestPolygon_SetupMVP();
         sdvg_SetColor4f(1.0f, 1.0f, 1.0f, fill_alpha);
         TestBeginPolygon(YAC_FALSE/*bAA*/);
         break;

      case RENDER_BEGIN_POLYGON_GRADIENT_LINEAR_AA: // 71
         sdvg_BindShader(0u);  // use built-in shader
         sdvg_EnableBlending();
         SetupGradientLinear();
         sdvg_SetFillColor4f(0.5f, 0.5f, 0.5f, 1.0f);
         DrawPaintBackground();
         TestPolygon_SetupMVP();
         sdvg_SetColor4f(1.0f, 1.0f, 1.0f, fill_alpha);
         TestBeginPolygon(YAC_TRUE/*bAA*/);
         break;

      case RENDER_BEGIN_POLYGON_GRADIENT_RADIAL: // 72
         sdvg_BindShader(0u);  // use built-in shader
         sdvg_EnableBlending();
         SetupGradientRadial();
         sdvg_SetFillColor4f(0.5f, 0.5f, 0.5f, 1.0f);
         DrawPaintBackground();
         TestPolygon_SetupMVP();
         sdvg_SetColor4f(1.0f, 1.0f, 1.0f, fill_alpha);
         TestBeginPolygon(YAC_FALSE/*bAA*/);
         break;

      case RENDER_BEGIN_POLYGON_GRADIENT_RADIAL_AA: // 73
         sdvg_BindShader(0u);  // use built-in shader
         sdvg_EnableBlending();
         SetupGradientRadial();
         sdvg_SetFillColor4f(0.5f, 0.5f, 0.5f, 1.0f);
         DrawPaintBackground();
         TestPolygon_SetupMVP();
         sdvg_SetColor4f(1.0f, 1.0f, 1.0f, fill_alpha);
         TestBeginPolygon(YAC_TRUE/*bAA*/);
         break;

      case RENDER_BEGIN_POLYGON_GRADIENT_CONIC: // 74
         sdvg_BindShader(0u);  // use built-in shader
         sdvg_EnableBlending();
         SetupGradientConic();
         sdvg_SetFillColor4f(0.5f, 0.5f, 0.5f, 1.0f);
         DrawPaintBackground();
         TestPolygon_SetupMVP();
         sdvg_SetColor4f(1.0f, 1.0f, 1.0f, fill_alpha);
         TestBeginPolygon(YAC_FALSE/*bAA*/);
         break;

      case RENDER_BEGIN_POLYGON_GRADIENT_CONIC_AA: // 75
         sdvg_BindShader(0u);  // use built-in shader
         sdvg_EnableBlending();
         SetupGradientConic();
         sdvg_SetFillColor4f(0.5f, 0.5f, 0.5f, 1.0f);
         DrawPaintBackground();
         TestPolygon_SetupMVP();
         sdvg_SetColor4f(1.0f, 1.0f, 1.0f, fill_alpha);
         TestBeginPolygon(YAC_TRUE/*bAA*/);
         break;

      case RENDER_BEGIN_POLYGON_CUSTOM_SHADER: // 76
         LazyCreateCustomShader_1();
         if(BindCustomShader_1())
         {
            sdvg_EnableBlending();
            TestPolygon_SetupMVP();
            sdvg_SetColor4f(1.0f, 1.0f, 1.0f, fill_alpha);
            TestBeginPolygon(YAC_FALSE/*bAA*/);
         }
         break;

      case RENDER_BEGIN_POLYGON_PATTERN: // 77
         sdvg_BindShader(0u);  // use built-in shader
         sdvg_EnableBlending();
         SetupPaintPattern();
         sdvg_SetFillColor4f(0.5f, 0.5f, 0.5f, 1.0f);
         DrawPaintBackground();
         TestPolygon_SetupMVP();
         sdvg_SetFillColor4f(1.0f, 1.0f, 1.0f, fill_alpha);
         TestBeginPolygon(YAC_FALSE/*bAA*/);
         break;

      case RENDER_BEGIN_POLYGON_PATTERN_AA: // 78
         sdvg_BindShader(0u);  // use built-in shader
         sdvg_EnableBlending();
         SetupPaintPattern();
         sdvg_SetFillColor4f(0.5f, 0.5f, 0.5f, 1.0f);
         DrawPaintBackground();
         TestPolygon_SetupMVP();
         sdvg_SetFillColor4f(1.0f, 1.0f, 1.0f, fill_alpha);
         TestBeginPolygon(YAC_TRUE/*bAA*/);
         break;

      case RENDER_BEGIN_POLYGON_PATTERN_ALPHA: // 79
         sdvg_BindShader(0u);  // use built-in shader
         sdvg_EnableBlending();
         SetupPaintPatternAlpha();
         sdvg_SetFillColor4f(0.5f, 0.5f, 0.5f, 1.0f);
         DrawPaintBackground();
         TestPolygon_SetupMVP();
         sdvg_SetFillColor4f(1.0f, 1.0f, 1.0f, fill_alpha);
         TestBeginPolygon(YAC_FALSE/*bAA*/);
         break;

      case RENDER_BEGIN_POLYGON_PATTERN_ALPHA_AA: // 80
         sdvg_BindShader(0u);  // use built-in shader
         sdvg_EnableBlending();
         SetupPaintPatternAlpha();
         sdvg_SetFillColor4f(0.5f, 0.5f, 0.5f, 1.0f);
         DrawPaintBackground();
         TestPolygon_SetupMVP();
         sdvg_SetFillColor4f(1.0f, 1.0f, 1.0f, fill_alpha);
         TestBeginPolygon(YAC_TRUE/*bAA*/);
         break;

      case RENDER_BEGIN_POLYGON_PATTERN_DECAL: // 81
         sdvg_BindShader(0u);  // use built-in shader
         sdvg_EnableBlending();
         sdvg_SetTextureDecalAlpha(decal_alpha);
         SetupPaintPatternDecal();
         sdvg_SetFillColor4f(0.5f, 0.5f, 0.5f, 1.0f);
         sdvg_SetStrokeColor4f(0.75f, 0.75f, 0.75f, 1.0f);
         DrawPaintBackground();
         TestPolygon_SetupMVP();
         sdvg_SetColor4f(1.0f, 1.0f, 1.0f, fill_alpha);
         TestBeginPolygon(YAC_FALSE/*bAA*/);
         break;

      case RENDER_BEGIN_POLYGON_PATTERN_DECAL_AA: // 82
         sdvg_BindShader(0u);  // use built-in shader
         sdvg_EnableBlending();
         sdvg_SetTextureDecalAlpha(decal_alpha);
         SetupPaintPatternDecal();
         sdvg_SetFillColor4f(0.5f, 0.5f, 0.5f, 1.0f);
         sdvg_SetStrokeColor4f(0.75f, 0.75f, 0.75f, 1.0f);
         DrawPaintBackground();
         TestPolygon_SetupMVP();
         sdvg_SetColor4f(1.0f, 1.0f, 1.0f, fill_alpha);
         TestBeginPolygon(YAC_TRUE/*bAA*/);
         break;

      case RENDER_BEGIN_POLYGON_PATTERN_DECAL_ALPHA: // 83
         sdvg_BindShader(0u);  // use built-in shader
         sdvg_EnableBlending();
         sdvg_SetTextureDecalAlpha(decal_alpha);
         SetupPaintPatternDecalAlpha();
         sdvg_SetFillColor4f(0.25f, 0.25f, 0.25f, 1.0f);
         sdvg_SetStrokeColor4f(0.5f, 0.5f, 0.5f, 1.0f);
         DrawPaintBackground();
         sdvg_SetFillColorARGB(0x3c5996u | (((sUI)(fill_alpha*255))<<24));
         sdvg_SetStrokeColorARGB(0xfff900u | (((sUI)(fill_alpha*255))<<24));
         TestPolygon_SetupMVP();
         TestBeginPolygon(YAC_FALSE/*bAA*/);
         break;

      case RENDER_BEGIN_POLYGON_PATTERN_DECAL_ALPHA_AA: // 84
         sdvg_BindShader(0u);  // use built-in shader
         sdvg_EnableBlending();
         sdvg_SetTextureDecalAlpha(decal_alpha);
         SetupPaintPatternDecalAlpha();
         sdvg_SetFillColor4f(0.25f, 0.25f, 0.25f, 1.0f);
         sdvg_SetStrokeColor4f(0.5f, 0.5f, 0.5f, 1.0f);
         DrawPaintBackground();
         sdvg_SetFillColorARGB(0x3c5996u | (((sUI)(fill_alpha*255))<<24));
         sdvg_SetStrokeColorARGB(0xfff900u | (((sUI)(fill_alpha*255))<<24));
         TestPolygon_SetupMVP();
         TestBeginPolygon(YAC_TRUE/*bAA*/);
         break;
   }

   sdvg_Flush();
   sdvg_EndFrame();
   hal_window_swap();

   sF32 spd = b_anim ? b_slomo ? 0.1f : 1.0f : 0.0f;

   if(b_anim_xy)
   {
      ang_x += dt * spd * 0.03f;
      ang_y += dt * spd * 0.02634f;
      if(ang_x >= 2*sM_2PIf)
         ang_x -= 2*sM_2PIf;
      if(ang_y >= 2*sM_2PIf)
         ang_y -= 2*sM_2PIf;
   }

   if(b_anim_whc)
   {
      ang_w += dt * spd * 0.0054112634f;
      ang_h += dt * spd * 0.0031312634f;
      ang_c += dt * spd * 0.0014132354f;
      if(ang_w >= 2*sM_2PIf)
         ang_w -= 2*sM_2PIf;
      if(ang_h >= 2*sM_2PIf)
         ang_h -= 2*sM_2PIf;
      if(ang_c >= sM_2PIf)
         ang_c -= sM_2PIf;
   }
}

// ---------------------------------------------------------------------------- ResetParams
static void ResetParams(void) {
   aa_range    = 2.5f;// / vp_scale;
   aa_exp      = 1.0f;
   stroke_w    = 1.5f;
   fill_alpha  = 1.0f;
   decal_alpha = 1.0f;
   radius_sclx = 1.0f;
   radius_scly = 1.0f;
   ang_x       = 0.0f;
   ang_y       = 0.0f;
   ang_w       = 0.0f;
   ang_h       = 0.0f;
   ang_c       = 0.0f;
   Dprintf("[...] reset\n");
   // if(2 == test_idx)
   // {
   //    stroke_w = 0.375f;
   //    aa_range = 0.75f;
   // }
}

// ---------------------------------------------------------------------------- SelectRenderMode
static void SelectRenderMode(sSI _mode) {
   render_mode = _mode;
   char buf[256];
   snprintf(buf,256,"%d:%s",render_mode,mode_names[render_mode]);
   SDL_SetWindowTitle(sdl_window, buf);
}

// ---------------------------------------------------------------------------- hal_on_key_down
void hal_on_key_down(sU32 _code, sU32 _mod) {
   switch(_code)
   {
      case SDLK_ESCAPE:
         Dprintf("[dbg] SDLK_ESCAPE, exiting..\n");
         b_hal_running = YAC_FALSE;
         break;

      case SDLK_SPACE:
         if(_mod)
         {
            b_slomo = !b_slomo;
            Dprintf("[...] b_slomo is %d\n", b_slomo);
         }
         else
         {
            b_anim = !b_anim;
            Dprintf("[...] b_anim is %d\n", b_anim);
         }
         break;

      case SDLK_RETURN:
         ResetParams();
         break;

      case SDLK_TAB:
         if(_mod)
         {
            b_anim_whc = !b_anim_whc;
            Dprintf("[...] b_anim_whc is %d\n", b_anim_whc);
         }
         else
         {
            b_anim_xy = !b_anim_xy;
            Dprintf("[...] b_anim_xy is %d\n", b_anim_xy);
         }
         break;

      case SDLK_UP:
         SelectRenderMode(sWRAP(render_mode + 1, 0, NUM_RENDER_MODES));
         break;

      case SDLK_DOWN:
         SelectRenderMode(sWRAP(render_mode - 1, 0, NUM_RENDER_MODES));
         break;

      case SDLK_RIGHT:
         stroke_w = sMIN(stroke_w + 0.125f, 16.0f);
         Dprintf("[...] stroke_w is %f\n", stroke_w);
         break;

      case SDLK_LEFT:
         stroke_w = sMAX(stroke_w - 0.125f, 0.125f);
         Dprintf("[...] stroke_w is %f\n", stroke_w);
         break;

      case 'a':
         b_aa = !b_aa;
         Dprintf("[...] b_aa is %d\n", b_aa);
         break;

      case 'd':
         sdvg_SetEnableDebug(!sdvg_GetEnableDebug());
         Dprintf("[...] sdvg_SetEnableDebug(%d)\n", sdvg_GetEnableDebug());
         break;

      case 'l':
         b_sym_radius = !b_sym_radius;
         Dprintf("[...] b_sym_radius is %d\n", b_sym_radius);
         break;

      case 'r':
         if(_mod)
         {
            decal_alpha = sMAX(decal_alpha - 0.0625f, 0.0f);
            Dprintf("[...] decal_alpha is %f\n", decal_alpha);
         }
         else
         {
            fill_alpha = sMAX(fill_alpha - 0.0625f, 0.0f);
            Dprintf("[...] fill_alpha is %f\n", fill_alpha);
         }
         break;

      case 't':
         if(_mod)
         {
            decal_alpha = sMIN(decal_alpha + 0.0625f, 1.0f);
            Dprintf("[...] decal_alpha is %f\n", decal_alpha);
         }
         else
         {
            fill_alpha = sMIN(fill_alpha + 0.0625f, 1.0f);
            Dprintf("[...] fill_alpha is %f\n", fill_alpha);
         }
         break;
   }
}

// ---------------------------------------------------------------------------- main
int main(int argc, char**argv) {
   Dprintf("main: ENTER\n");

   if(argc >= 2)
   {
      render_mode = atoi(argv[1]);
      if(render_mode >= NUM_RENDER_MODES)
         render_mode = 0;
      else if(render_mode < 0)
         render_mode = 0;
   }

   if(hal_window_init())
   {
      // Update window title
      SelectRenderMode(render_mode);

#ifdef USE_MINNIE_MIB_SETUP
      minnie_impl_init();
#endif // USE_MINNIE_MIB_SETUP

      sdvg_SetScratchBufferSize(4096*1024);
      sdvg_SetGLSLVersion(1/*b_glcore*//*bV3*/, YAC_FALSE/*bGLES*/, NULL/*sVersionStringOrNull*/);
      sdvg_Init(1/*b_glcore*/);

      Dprintf("[...] init OK, initializing fonts..\n");

      sdvg_InitFont(&font,
                    mem_base_font_default_30, mem_size_font_default_30,
                    mem_base_font_default_30_tex, 1024, 101
                    );
      sdvg_InitFont(&font_zoom,
                    mem_base_font_default_256, mem_size_font_default_256,
                    mem_base_font_default_256_tex, 512, 584
                    );
      sdvg_InitFont(&font_zoom_sdf,
                    mem_base_font_default_256_sdf, mem_size_font_default_256_sdf,
                    mem_base_font_default_256_sdf_tex, 1024, 292
                    );

      Dprintf("[...] initializing textures and VBOs..\n");
      sdvg_OnOpen();

      yac_buffer_alloc(&buf_vbo, 16384u);
      buf_vbo_id = sdvg_CreateVBO(buf_vbo.size);

      tex_id = sdvg_CreateTexture2D(SDVG_TEXFMT_ARGB32, TEX_SX, TEX_SY,
                                    (const void*)mem_base_tex_escher_argb32,
                                    mem_size_tex_escher_argb32
                                    );

      tex_alpha_id = sdvg_CreateTexture2D(SDVG_TEXFMT_ALPHA8, TEX_SX, TEX_SY,
                                          (const void*)mem_base_tex_escher_alpha,
                                          mem_size_tex_escher_alpha
                                          );

      tex_pattern_1_alpha_id = sdvg_CreateTexture2D(SDVG_TEXFMT_ALPHA8, 256, 256,
                                                    (const void*)mem_base_tex_pattern_1_alpha,
                                                    mem_size_tex_pattern_1_alpha
                                                    );

      sdvg_GradientToTexture(tex_gradient, 256u,
                             gradient_colors, 5u,
                             gradient_starts, 5u,
                             YAC_TRUE/*bSmoothStep*/
                             );
      tex_gradient_id = sdvg_CreateTexture2D(SDVG_TEXFMT_ARGB32, 256u, 1u, (const void*)tex_gradient, 256*sizeof(sU32));

      sdvg_FontOnOpen(&font);
      sdvg_FontOnOpen(&font_zoom);
      sdvg_FontOnOpen(&font_zoom_sdf);

      custom_shader_idx_1 = 0u;
      custom_shader_idx_2 = 0u;
      custom_shader_idx_3 = 0u;
      test_text_3_shader_idx = 0u;
      custom_vbo_id_3 = 0u;
      polygon_vbo_id = 0u;

      hal_window_loop();

#ifdef USE_MINNIE_MIB_SETUP
      minnie_impl_exit();
#endif // USE_MINNIE_MIB_SETUP

      hal_window_exit();
   }

   Dprintf("main: LEAVE\n");

   return 0;
}
