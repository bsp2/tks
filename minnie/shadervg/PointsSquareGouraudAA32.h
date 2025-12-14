// ----
// ---- file   : PointsSquareGouraudAA32.h
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

class PointsSquareGouraudAA32 : public ShaderVG_Shape {

  public:
   // ------------ vertex shader --------------
   const char *vs_src =
      "uniform mat4  u_transform; \n"
      "uniform float u_point_radius; \n"
      " \n"
      "ATTRIBUTE vec2 a_vertex; \n"
      "ATTRIBUTE vec4 a_color; \n"
      " \n"
      "VARYING_OUT vec2 v_vertex_mp; \n"
      "VARYING_OUT vec4 v_color; \n"
      " \n"
      "void main(void) { \n"
      "  vec2 vCtr = a_vertex; \n"
      "  vec2 v; \n"
      " \n"
      "  float index = float(gl_VertexID % 6); \n"
      " \n"
      "  if(index > 5.9) { \n"
      "    v = vec2(0,0); \n"
      "  } \n"
      "  else if(index > 4.9) { \n"
      "    v = vec2(vCtr.x - u_point_radius, vCtr.y + u_point_radius); \n"  // LB
      "  } \n"
      "  else if(index > 3.9) { \n"
      "    v = vCtr + vec2(u_point_radius, u_point_radius); \n"  // RB
      "  } \n"
      "  else if(index > 2.9) { \n"
      "    v = vCtr - vec2(u_point_radius, u_point_radius); \n"  // LT
      "  } \n"
      "  else if(index > 1.9) { \n"
      "    v = vCtr + vec2(u_point_radius, u_point_radius); \n"  // RB
      "  } \n"
      "  else if(index > 0.9) { \n"
      "    v = vec2(vCtr.x + u_point_radius, vCtr.y - u_point_radius); \n"  // RT
      "  } \n"
      "  else { \n"
      "    v = vCtr - vec2(u_point_radius, u_point_radius); \n"  // LT
      "  } \n"
      " \n"
      "  gl_Position = u_transform * vec4(v,0,1); \n"
      "  v_vertex_mp = v - vCtr; \n"
      "  v_color = a_color; \n"
      "} \n"
      ;

   // ------------ fragment shader ------------
   const char *fs_src =
      "uniform vec4  u_color_stroke; \n"
      "uniform float u_point_radius; \n"
      "uniform float u_aa_range; \n"
      "uniform float u_debug; \n"
      " \n"
      "VARYING_IN vec2 v_vertex_mp; \n"
      "VARYING_IN vec4 v_color; \n"
      " \n"
      "void main(void) { \n"
      // based on https://iquilezles.org/articles/distfunctions2d/ sdBox()
      "  vec2 vd = abs(v_vertex_mp) - vec2(u_point_radius - u_aa_range, u_point_radius - u_aa_range); \n"
      "  float d = length(max(vd, 0.0)) + min(max(vd.x, vd.y), 0.0); \n"
      // aa
      "  float a = 1.0 - smoothstep(0.0, u_aa_range, d); \n"
      "  FRAGCOLOR = vec4(u_color_stroke.rgb * v_color.rgb, u_color_stroke.a * v_color.a * a); \n"
      "  if(u_debug > 0.0) { \n"
      "    FRAGCOLOR = vec4(1.0, a, a, 1.0); \n"
      "  } \n"
      "} \n"
      ;

   sBool validateShapeShader(void) {
      return
            (-1 != shape_a_vertex)
         && (-1 != shape_a_color)
         && (-1 != shape_u_transform)
         && (-1 != shape_u_color_stroke)
         && (-1 != shape_u_point_radius)
         && (-1 != shape_u_aa_range)
         ;
   }

   sBool onOpen(void) {
      if(createShapeShader(vs_src, fs_src))
      {
         return YAC_TRUE;
      }
      return YAC_FALSE;
   }

   void drawPointsSquareGouraudAAVBO32(sUI              _vboId,
                                       sUI              _byteOffset,
                                       sUI              _numPoints,
                                       Dsdvg_mat4_ref_t _mvpMatrix,
                                       sF32             _strokeR, sF32 _strokeG, sF32 _strokeB, sF32 _strokeA,
                                       sF32             _pointRadius,
                                       sF32             _aaRange
                                       ) {
      //
      // VBO vertex format (12 bytes per vertex):
      //   +0  u8  r
      //   +1  u8  g
      //   +2  u8  b
      //   +3  u8  a
      //   +4  f32 x
      //   +8  f32 y
      //

      sdvg_BindVBO(_vboId);

      shape_shader.bind();

      Dsdvg_uniform_mat4(shape_u_transform, _mvpMatrix);
      Dsdvg_uniform_4f(shape_u_color_stroke, _strokeR, _strokeG, _strokeB, _strokeA);
      Dsdvg_uniform_1f(shape_u_point_radius, _pointRadius);
      Dsdvg_uniform_1f(shape_u_aa_range, _aaRange);
      if(-1 != shape_u_debug)
      {
         Dsdvg_uniform_1f(shape_u_debug, b_debug ? 1.0f : 0.0f);
      }

      Dsdvg_attrib_offset(shape_a_color,    4/*size*/, GL_UNSIGNED_BYTE,  GL_TRUE/*normalize*/,  12/*stride*/, _byteOffset +  0);
      Dsdvg_attrib_offset(shape_a_vertex,   2/*size*/, GL_FLOAT,          GL_FALSE/*normalize*/, 12/*stride*/, _byteOffset +  4);

      Dsdvg_attrib_enable(shape_a_color);
      Dsdvg_attrib_enable(shape_a_vertex);

      Dsdvg_attrib_divisor(shape_a_color, 1);
      Dsdvg_attrib_divisor(shape_a_vertex, 1);

      const sUI numInstances = _numPoints;
      Dsdvg_draw_triangles_instanced_vbo(6, numInstances);

      Dsdvg_attrib_disable(shape_a_color);
      Dsdvg_attrib_disable(shape_a_vertex);

      Dsdvg_attrib_divisor_reset(shape_a_color);
      Dsdvg_attrib_divisor_reset(shape_a_vertex);
   }

};
