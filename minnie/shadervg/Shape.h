// ----
// ---- file   : Shape.h
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

#ifndef SHADERVG_SHAPE_H__
#define SHADERVG_SHAPE_H__

#define YG(a)
YG("shadervg");


#define ELLIPSE_EXTRUDE_I  2.0f
#define ELLIPSE_EXTRUDE_O  6.0f
#define ELLIPSE_NUM_SEG    16
#define ELLIPSE_SINGLE_RADIUS_THRESHOLD  10.0f

#define RECT_SINGLE_AREA_THRESHOLD  256

#define ROUNDRECT_SINGLE_AREA_THRESHOLD  256


class ShaderVG_Shape {

  public:
   ShaderVG_Shader shape_shader;
   sBool b_builtin;

   sSI shape_a_vertex;
   sSI shape_a_vertex_n;   // optional
   sSI shape_a_vertex_nn;  // optional
   sSI shape_a_index;      // optional
   sSI shape_a_color;      // optional
   sSI shape_a_edge;       // optional
   sSI shape_a_scale;      // optional
   sSI shape_a_bc;         // optional
   sSI shape_a_uv;         // optional

   sSI shape_u_transform;
   sSI shape_u_aa;               // optional
   sSI shape_u_aa_range;         // optional for non-AA shader
   sSI shape_u_aa_exp;           // optional
   sSI shape_u_center;           // optional for non-AA shader
   sSI shape_u_size;             // optional
   sSI shape_u_size_i;           // optional
   sSI shape_u_size_o;           // optional
   sSI shape_u_radius;           // optional
   sSI shape_u_radius_i;         // optional
   sSI shape_u_radius_o;         // optional
   sSI shape_u_ob_radius_i;      // optional
   sSI shape_u_ob_radius_o;      // optional
   sSI shape_u_ob_radius_i_max;  // optional
   sSI shape_u_ob_radius_o_max;  // optional
   sSI shape_u_radius_i_max;     // optional
   sSI shape_u_radius_o_max;     // optional
   sSI shape_u_ob_radius;        // optional
   sSI shape_u_ob_radius_max;    // optional
   sSI shape_u_radius_max;       // optional
   sSI shape_u_point_radius;     // optional
   sSI shape_u_color_fill;       // optional
   sSI shape_u_color_stroke;     // optional
   sSI shape_u_global_alpha;     // optional
   sSI shape_u_decal_alpha;      // optional
   sSI shape_u_sampler;          // optional
   sSI shape_u_stroke_w;         // optional
   sSI shape_u_debug;            // optional
   sSI shape_u_tex_0;            // optional
   sSI shape_u_tex_1;            // optional
   sSI shape_u_a_min;            // optional
   sSI shape_u_a_max;            // optional
   sSI shape_u_a_maxmin_scale;   // optional
   sSI shape_u_a_exp;            // optional
   sSI shape_u_paint_tex;        // optional
   sSI shape_u_paint_start;      // optional
   sSI shape_u_paint_end;        // optional
   sSI shape_u_paint_scale;      // optional
   sSI shape_u_paint_ndir;       // optional
   sSI shape_u_paint_ob_len;     // optional
   sSI shape_u_paint_angle;      // optional

   // debug:
   sBool b_draw_inner;
   sBool b_draw_border;
   sBool b_debug;


  public:
   ShaderVG_Shape(void);
   virtual ~ShaderVG_Shape();

   void allocScratchBuffer (sSI _aVertex, Dsdvg_buffer_ref_t _scratchBuf, sUI _numBytes);

   static void EmitQuadVertices (Dsdvg_buffer_ref_t _vb,
                                 sF32 _x, sF32 _y, sF32 _w, sF32 _h
                                 );

   void setQuadVertices (sSI _aVertex,
                         Dsdvg_buffer_ref_t _scratchBuf,
                         sF32 _x, sF32 _y, sF32 _w, sF32 _h
                         );

   static void EmitRectBorderVertices (Dsdvg_buffer_ref_t _vb,
                                       sF32 _centerX, sF32 _centerY,
                                       sF32 _sizeX, sF32 _sizeY,
                                       sF32 _strokeW,
                                       sF32 _aaRange
                                       );

   static void EmitRectangleVertices (Dsdvg_buffer_ref_t _vb,
                                      sF32 _x, sF32 _y,
                                      sF32 _w, sF32 _h,
                                      sF32 _b
                                      );

   static sUI EmitRoundRectInnerVertices (Dsdvg_buffer_ref_t _vb,
                                          sF32 _centerX, sF32 _centerY,
                                          sF32 _sizeX,   sF32 _sizeY,
                                          sF32 _radiusX, sF32 _radiusY,
                                          sF32 _strokeW,
                                          sF32 _aaRange
                                          );

   static sUI EmitRoundRectBorderVertices (Dsdvg_buffer_ref_t _vb,
                                           sF32 _centerX, sF32 _centerY,
                                           sF32 _sizeX,   sF32 _sizeY,
                                           sF32 _radiusX, sF32 _radiusY,
                                           sF32 _strokeW,
                                           sF32 _aaRange
                                           );

   virtual sBool validateShapeShader (void);

   sBool createShapeShader (const char *_sVS, const char *_sFS);

   void updatePaintUniforms (const shadervg_paint_t *_paint);

   virtual sBool onOpen (void);
};


#endif // SHADERVG_SHAPE_H__
