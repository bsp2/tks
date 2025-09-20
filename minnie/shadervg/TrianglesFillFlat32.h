// ----
// ---- file   : TrianglesFillFlat32.h
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

class TrianglesFillFlat32 : public ShaderVG_Shape {

  public:
   // ------------ vertex shader --------------
   const char *vs_src =
      "uniform mat4 u_transform; \n"
      " \n"
      "ATTRIBUTE vec2 a_vertex; \n"
      " \n"
      "void main(void) { \n"
      "  gl_Position = u_transform * vec4(a_vertex,0,1); \n"
      "} \n"
      ;

   // ------------ fragment shader ------------
   const char *fs_src =
      "uniform vec4 u_color_fill; \n"
      " \n"
      "void main(void) { \n"
      "  FRAGCOLOR = u_color_fill; \n"
      "} \n"
      ;

   sBool validateShapeShader(void) {
      return
         (-1 != shape_a_vertex)       &&
         (-1 != shape_u_transform)    &&
         (-1 != shape_u_color_fill)
         ;
   }

   sBool onOpen(void) {
      if(createShapeShader(vs_src, fs_src))
      {
         return YAC_TRUE;
      }
      return YAC_FALSE;
   }

   void drawTrianglesFillFlatVBO32(sUI              _vboId,
                                   sUI              _byteOffset,
                                   sUI              _numTris,
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

      Dsdvg_attrib_offset(shape_a_vertex, 2/*size*/, GL_FLOAT, GL_FALSE/*normalize*/, 8/*stride*/, _byteOffset);
      Dsdvg_attrib_enable(shape_a_vertex);

      Dsdvg_draw_triangles(0, _numTris * 3);

      Dsdvg_attrib_disable(shape_a_vertex);
   }

};
