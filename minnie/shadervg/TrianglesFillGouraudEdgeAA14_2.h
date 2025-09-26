// ----
// ---- file   : TrianglesFillGouraudEdgeAA14_2.h
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

class TrianglesFillGouraudEdgeAA14_2 : public ShaderVG_Shape {

  public:
   // ------------ vertex shader --------------
   const char *vs_src =
      "uniform mat4  u_transform; \n"
      "uniform float u_aa; \n"  // 1.0=on, 0.0=off
      " \n"
      "ATTRIBUTE vec2 a_vertex; \n"
      "ATTRIBUTE vec4 a_color; \n"
      "ATTRIBUTE vec3 a_edge; \n"   // x=bc y=ca z=ab
      "ATTRIBUTE vec3 a_scale; \n"  // x=s  y=t  z=u
      "ATTRIBUTE vec2 a_bc; \n"     // barycentric coordinates
      " \n"
      // .x/bc: opposite from A  ('s')   0.0=AA  1.0=noAA
      // .y/ca: opposite from B  ('u')   ..
      // .z/ab: opposite from C  ('t')   ..
      "VARYING_OUT vec3 v_edge; \n"
      "VARYING_OUT vec3 v_scale; \n"   // .x=s  .y=t  .z=u
      "VARYING_OUT vec4 v_color; \n"
      "VARYING_OUT vec2 v_bc; \n"
      " \n"
      "void main() { \n"
      "  gl_Position = u_transform * vec4(a_vertex*0.25,0,1); \n"
      "  v_color = a_color; \n"
      "  if(u_aa > 0.0) \n"
      "    v_edge  = a_edge; \n"
      "  else \n"
      "    v_edge  = vec3(1,1,1); \n"
      "  v_scale = a_scale; \n"
      "  v_bc    = a_bc; \n"
      "} \n"
      ;

   // ------------ fragment shader ------------
   const char *fs_src =
      "uniform float u_global_alpha; \n"
      " \n"
      // .x/bc: opposite from A  ('s')   0.0=AA  1.0=noAA
      // .y/ca: opposite from B  ('u')   ..
      // .z/ab: opposite from C  ('t')   ..
      "VARYING_IN vec3 v_edge; \n"
      "VARYING_IN vec3 v_scale; \n"    // .x=s  .y=t  .z=u
      "VARYING_IN vec4 v_color; \n"
      "VARYING_IN vec2 v_bc; \n"
      " \n"
      "void main() { \n"
      "  float s = v_bc.s; \n"
      "  float t = v_bc.t; \n"
      "  float u = s - t; \n"
      "  s = 1.0 - s; \n"
      " \n"
      // Edgeflag AA border (linear, min)
      "  float a = \n"
      "     min(max(v_edge.x/*bc*/, s * v_scale.x/*s*/), \n"
      "         min(max(v_edge.z/*ab*/, t * v_scale.y/*t*/), \n"
      "             max(v_edge.y/*ca*/, u * v_scale.z/*u*/) \n"
      "         ) \n"
      "     ) \n"
      "     ; \n"
      "  FRAGCOLOR = vec4(v_color.rgb, v_color.a * min(1.0,a) * u_global_alpha); \n"
      "} \n"
      ;

   sBool validateShapeShader(void) {
      return
         (-1 != shape_a_vertex)     &&
         (-1 != shape_a_color)      &&
         (-1 != shape_a_edge)       &&
         (-1 != shape_a_scale)      &&
         (-1 != shape_a_bc)         &&
         (-1 != shape_u_transform)  &&
         (-1 != shape_u_aa)         &&
         (-1 != shape_u_global_alpha)
         ;
   }

   sBool onOpen(void) {
      if(createShapeShader(vs_src, fs_src))
      {
         return YAC_TRUE;
      }
      return YAC_FALSE;
   }

   void drawTrianglesFillGouraudEdgeAAVBO14_2(sUI              _vboId,
                                              sUI              _byteOffset,
                                              sUI              _numVerts,
                                              Dsdvg_mat4_ref_t _mvpMatrix,
                                              sBool            _bAA,
                                              sF32             _alpha
                                              ) {
      //
      // VBO vertex format (26 bytes per vertex):
      //     +0 s14.2 x
      //     +2 s14.2 y
      //     +4 u8  r
      //     +5 u8  g
      //     +6 u8  b
      //     +7 u8  a
      //     +8 u8  edge_bc
      //     +9 u8  edge_ca
      //    +10 u8  edge_ab
      //    +11 u8  __pad1
      //    +12 f32 scale_s
      //    +16 f32 scale_t
      //    +20 f32 scale_u
      //    +24 u8  barycentric_s
      //    +25 u8  barycentric_t
      //

      sdvg_BindVBO(_vboId);

      shape_shader.bind();

      Dsdvg_uniform_mat4(shape_u_transform, _mvpMatrix);
      Dsdvg_uniform_1f(shape_u_aa, _bAA ? 1.0f : 0.0f);
      Dsdvg_uniform_1f(shape_u_global_alpha, _alpha);

      Dsdvg_attrib_offset(shape_a_vertex, 2/*size*/, GL_SHORT,         GL_FALSE/*normalize*/, 26/*stride*/, _byteOffset +  0);
      Dsdvg_attrib_offset(shape_a_color,  4/*size*/, GL_UNSIGNED_BYTE, GL_TRUE /*normalize*/, 26/*stride*/, _byteOffset +  4);
      Dsdvg_attrib_offset(shape_a_edge,   3/*size*/, GL_UNSIGNED_BYTE, GL_TRUE /*normalize*/, 26/*stride*/, _byteOffset +  8);
      Dsdvg_attrib_offset(shape_a_scale,  3/*size*/, GL_FLOAT,         GL_FALSE/*normalize*/, 26/*stride*/, _byteOffset + 12);
      Dsdvg_attrib_offset(shape_a_bc,     2/*size*/, GL_UNSIGNED_BYTE, GL_TRUE /*normalize*/, 26/*stride*/, _byteOffset + 24);

      Dsdvg_attrib_enable(shape_a_vertex);
      Dsdvg_attrib_enable(shape_a_color);
      Dsdvg_attrib_enable(shape_a_edge);
      Dsdvg_attrib_enable(shape_a_scale);
      Dsdvg_attrib_enable(shape_a_bc);

      Dsdvg_draw_triangles(0, _numVerts);

      Dsdvg_attrib_disable(shape_a_bc);
      Dsdvg_attrib_disable(shape_a_scale);
      Dsdvg_attrib_disable(shape_a_edge);
      Dsdvg_attrib_disable(shape_a_color);
      Dsdvg_attrib_disable(shape_a_vertex);
   }

};
