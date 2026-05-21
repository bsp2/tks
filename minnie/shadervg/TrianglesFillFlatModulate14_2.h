// ----
// ---- file   : TrianglesFillFlatModulate14_2.h
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : Distributed under terms of the MIT license (https://opensource.org/licenses/MIT)
// ----          Copyright 2026 by bsp
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

class TrianglesFillFlatModulate14_2 : public ShaderVG_Shape {

  public:

   // ------------ vertex shader --------------
   const char *vs_src =
      "uniform mat4 u_transform; \n"
      "uniform vec4 u_color_fill; \n"
      " \n"
      "ATTRIBUTE vec4 a_color; \n"
      "ATTRIBUTE vec2 a_vertex; \n"
      " \n"
      "VARYING_OUT vec4 v_color; \n"
      " \n"
      "void main(void) { \n"
      "  v_color = a_color * u_color_fill; \n"
      "  gl_Position = u_transform * vec4(a_vertex*0.25,0,1); \n"
      "} \n"
      ;

   // ------------ fragment shader ------------
   const char *fs_src =
      "VARYING_IN vec4 v_color; \n"
      " \n"
      "void main(void) { \n"
      "  FRAGCOLOR = v_color; \n"
      "} \n"
      ;

   sBool validateShapeShader(void) {
      return
         (-1 != shape_a_color)      &&
         (-1 != shape_a_vertex)     &&
         (-1 != shape_u_transform)
         ;
   }

   sBool onOpen(void) {
      if(createShapeShader(vs_src, fs_src))
      {
         return YAC_TRUE;
      }
      return YAC_FALSE;
   }

   void drawTrianglesFillFlatModulateVBO14_2(sUI              _vboId,
                                             sUI              _byteOffset,
                                             sUI              _numVerts,
                                             Dsdvg_mat4_ref_t _mvpMatrix,
                                             sF32             _fillR, sF32 _fillG, sF32 _fillB, sF32 _fillA
                                             ) {
      //
      // VBO vertex format (8 bytes per vertex):
      //   +0 u8    r
      //   +1 u8    g
      //   +2 u8    b
      //   +3 u8    a
      //   +4 s14.2 x
      //   +6 s14.2 y
      //

      /* Dyac_host_printf("xxx drawTrianglesFillGouraudVBO14_2: vboId=%u byteOffset=%u numVerts=%u\n", _vboId, _byteOffset, _numVerts); */

      sdvg_BindVBO(_vboId);

      if(!bindShader())
         return;

      /* Dyac_host_printf("xxx u_transform=%d a_vertex=%d a_color=%d mvpMatrix=%p\n", shape_u_transform, shape_a_vertex, shape_a_color, _mvpMatrix); */
      Dsdvg_uniform_mat4(shape_u_transform, _mvpMatrix);
      Dsdvg_uniform_4f(shape_u_color_fill, _fillR, _fillG, _fillB, _fillA);

      Dsdvg_attrib_offset(shape_a_color,  4/*size*/, GL_UNSIGNED_BYTE, GL_TRUE/*normalize*/,  8/*stride*/, _byteOffset + 0);
      Dsdvg_attrib_offset(shape_a_vertex, 2/*size*/, GL_SHORT,         GL_FALSE/*normalize*/, 8/*stride*/, _byteOffset + 4);

      Dsdvg_attrib_enable(shape_a_color);
      Dsdvg_attrib_enable(shape_a_vertex);

      Dsdvg_draw_triangles(0, _numVerts);

      Dsdvg_attrib_disable(shape_a_color);
      Dsdvg_attrib_disable(shape_a_vertex);
   }

};
