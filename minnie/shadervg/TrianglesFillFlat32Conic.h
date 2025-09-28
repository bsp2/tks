// ----
// ---- file   : TrianglesFillFlat32Conic.h
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
// ---- info   : ShaderVG render utilities
// ----
// ----
// ----

class TrianglesFillFlat32Conic : public ShaderVG_Shape {

  public:
   // ------------ vertex shader --------------
   const char *vs_src =
      "uniform mat4 u_transform; \n"
      "uniform vec2 u_paint_start; \n"
      "uniform vec2 u_paint_scale; \n"
      " \n"
      "ATTRIBUTE vec2 a_vertex; \n"
      " \n"
      "VARYING_OUT vec2 v_paint_pos; \n"
      " \n"
      "void main(void) { \n"
      "  gl_Position = u_transform * vec4(a_vertex,0,1); \n"
      "  v_paint_pos = (a_vertex - u_paint_start) * u_paint_scale; \n"
      "} \n"
      ;

   // ------------ fragment shader ------------
   const char *fs_src =
      "uniform vec4 u_color_fill; \n"
      "uniform sampler2D u_paint_tex; \n"
      "uniform float u_paint_angle; \n"
      " \n"
      "VARYING_IN vec2 v_paint_pos; \n"
      " \n"
      "void main(void) { \n"
      "  vec2 n = normalize(abs(v_paint_pos)); \n"
      "  float a = atan(n.y / n.x) * (1.0 / 6.2831853071795864); \n"
      "  if(v_paint_pos.x > 0.0) {\n"
      "    if(v_paint_pos.y < 0.0) { \n"
      "      a = 1.0 - a; \n"
      "    } \n"
      "  } \n"
      "  else if(v_paint_pos.y < 0.0) { \n"
      "      a += 0.5; \n"
      "  } \n"
      "  else { \n"
      "    a = 0.5 - a; \n"
      "  } \n"
      "  a += u_paint_angle; \n"
      "  if(a >= 1.0) a -= 1.0; \n"   // (note) use texture repeat ?
      "  else if(a < 0.0) a += 1.0; \n"
      "  vec4 c = TEXTURE2D(u_paint_tex, vec2(a, 0.0)); \n"
      "  FRAGCOLOR = c * u_color_fill; \n"
      "} \n"
      ;

   sBool validateShapeShader(void) {
      return
         (-1 != shape_a_vertex)       &&
         (-1 != shape_u_transform)    &&
         (-1 != shape_u_color_fill)   &&
         (-1 != shape_u_paint_tex)    &&
         (-1 != shape_u_paint_start)  &&
         (-1 != shape_u_paint_scale)  &&
         (-1 != shape_u_paint_angle)
         ;
      return YAC_TRUE;
   }

   sBool onOpen(void) {
      if(createShapeShader(vs_src, fs_src))
      {
         return YAC_TRUE;
      }
      return YAC_FALSE;
   }

   void drawTrianglesFillFlatVBO32Conic(sUI              _vboId,
                                        sUI              _byteOffset,
                                        sUI              _numVerts,
                                        Dsdvg_mat4_ref_t _mvpMatrix,
                                        sF32             _fillR, sF32 _fillG, sF32 _fillB, sF32 _fillA
                                        ) {
      //
      // VBO vertex format (8 bytes per vertex):
      //   +0 f32 x
      //   +4 f32 y
      //

      sdvg_BindVBO(_vboId);

      shape_shader.bind();

      Dsdvg_uniform_mat4(shape_u_transform, _mvpMatrix);
      Dsdvg_uniform_4f(shape_u_color_fill, _fillR, _fillG, _fillB, _fillA);
      // (todo) paint uniforms

      Dsdvg_attrib_offset(shape_a_vertex, 2/*size*/, GL_FLOAT, GL_FALSE/*normalize*/, 8/*stride*/, _byteOffset);
      Dsdvg_attrib_enable(shape_a_vertex);

      Dsdvg_draw_triangles(0, _numVerts);

      Dsdvg_attrib_disable(shape_a_vertex);
   }

};
