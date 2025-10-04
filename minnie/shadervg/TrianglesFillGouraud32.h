// ----
// ---- file   : TrianglesFillGouraud32.h
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

class TrianglesFillGouraud32 : public ShaderVG_Shape {

  public:
   // ------------ vertex shader --------------
   const char *vs_src =
      "uniform mat4 u_transform; \n"
      " \n"
      "ATTRIBUTE vec2 a_vertex; \n"
      "ATTRIBUTE vec4 a_color; \n"
      " \n"
      "VARYING_OUT vec4 v_color; \n"
      " \n"
      "void main(void) { \n"
      "  v_color = a_color; \n"
      "  gl_Position = u_transform * vec4(a_vertex,0,1); \n"
      "} \n"
      ;

   // ------------ fragment shader ------------
   const char *fs_src =
      "uniform float u_global_alpha; \n"
      " \n"
      "VARYING_IN vec4 v_color; \n"
      " \n"
      "void main(void) { \n"
      "  FRAGCOLOR = vec4(v_color.rgb, v_color.a * u_global_alpha); \n"
      "} \n"
      ;

   sBool validateShapeShader(void) {
      return
         (-1 != shape_a_vertex)       &&
         (-1 != shape_a_color)        &&
         (-1 != shape_u_transform)    &&
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

   void drawTrianglesFillGouraudVBO32(sUI              _vboId,
                                      sUI              _byteOffset,
                                      sUI              _numVerts,
                                      Dsdvg_mat4_ref_t _projMatrix,
                                      sF32             _alpha
                                      ) {
      //
      // VBO vertex format (12 bytes per vertex):
      //    +0 f32 x
      //    +4 f32 y
      //    +8 u8  r
      //    +9 u8  g
      //   +10 u8  b
      //   +11 u8  a
      //

      sdvg_BindVBO(_vboId);

      shape_shader.bind();

      Dsdvg_uniform_mat4(shape_u_transform, _projMatrix);
      Dsdvg_uniform_1f(shape_u_global_alpha, _alpha);

      Dsdvg_attrib_offset(shape_a_vertex, 2/*size*/, GL_FLOAT,         GL_FALSE/*normalize*/, 12/*stride*/, _byteOffset + 0);
      Dsdvg_attrib_offset(shape_a_color,  4/*size*/, GL_UNSIGNED_BYTE, GL_TRUE /*normalize*/, 12/*stride*/, _byteOffset + 8);

      Dsdvg_attrib_enable(shape_a_vertex);
      Dsdvg_attrib_enable(shape_a_color);

      Dsdvg_draw_triangles(0, _numVerts);

      Dsdvg_attrib_disable(shape_a_color);
      Dsdvg_attrib_disable(shape_a_vertex);
   }

#ifdef SHADERVG_STENCIL_POLYGONS
   void drawPolygonFillGouraudVBO32(sUI              _vboId,
                                    sUI              _byteOffset,
                                    sUI              _numVerts,
                                    Dsdvg_mat4_ref_t _projMatrix,
                                    sF32             _alpha
                                    ) {
      //
      // VBO vertex format (12 bytes per vertex):
      //    +0 f32 x
      //    +4 f32 y
      //    +8 u8  r
      //    +9 u8  g
      //   +10 u8  b
      //   +11 u8  a
      //

      sdvg_BindVBO(_vboId);

      shape_shader.bind();

      Dsdvg_uniform_mat4(shape_u_transform, _projMatrix);
      Dsdvg_uniform_1f(shape_u_global_alpha, _alpha);

      Dsdvg_attrib_offset(shape_a_vertex, 2/*size*/, GL_FLOAT,         GL_FALSE/*normalize*/, 12/*stride*/, _byteOffset + 0);
      Dsdvg_attrib_offset(shape_a_color,  4/*size*/, GL_UNSIGNED_BYTE, GL_TRUE /*normalize*/, 12/*stride*/, _byteOffset + 8);

      Dsdvg_attrib_enable(shape_a_vertex);
      Dsdvg_attrib_enable(shape_a_color);

      Dsdvg_stencil_poly_pass1();
      Dsdvg_draw_triangle_fan_vbo(0, _numVerts);

      Dsdvg_stencil_poly_pass2();
      Dsdvg_draw_triangle_fan_vbo(0, _numVerts);

      Dsdvg_stencil_poly_end();

      Dsdvg_attrib_disable(shape_a_color);
      Dsdvg_attrib_disable(shape_a_vertex);
   }
#endif // SHADERVG_STENCIL_POLYGONS

};
