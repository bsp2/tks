// ----
// ---- file   : TrianglesTexUVFlat32AlphaSDF.h
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

class TrianglesTexUVFlat32AlphaSDF : public ShaderVG_Shape {

  public:
   // ------------ vertex shader --------------
   const char *vs_src =
      "uniform mat4 u_transform; \n"
      " \n"
      "ATTRIBUTE vec2 a_vertex; \n"
      "ATTRIBUTE vec2 a_uv; \n"
      " \n"
      "VARYING_OUT vec2 v_uv; \n"
      " \n"
      "void main() { \n"
      "  gl_Position = u_transform * vec4(a_vertex,0,1); \n"
      "  v_uv = a_uv; \n"
      "} \n"
      ;

   // ------------ fragment shader ------------
   const char *fs_src =
      "uniform sampler2D u_sampler; \n"
      "uniform vec4      u_color_fill; \n"
      "uniform float     u_a_min; \n"
      "uniform float     u_a_max; \n"
      "uniform float     u_a_maxmin_scale; \n"
      "uniform float     u_a_exp; \n"
      " \n"
      "VARYING_IN vec2 v_uv; \n"
      " \n"
      "void main() { \n"
      "  float a = TEXTURE2D(u_sampler, v_uv).TEX_ALPHA; \n"
      " \n"
      "  if(a < u_a_min) a = 0.0; \n"
      "  else if(a > u_a_max) a = 1.0; \n"
      "  else a = (a - u_a_min) * (u_a_maxmin_scale); \n"
      " \n"
      "  a = pow(a, u_a_exp); \n"
      "  FRAGCOLOR = vec4(u_color_fill.rgb, a * u_color_fill.a); \n"
      "} \n"
      ;

   sBool validateShapeShader(void) {
      return
         (-1 != shape_a_vertex)
         && (-1 != shape_a_uv)
         && (-1 != shape_u_transform)
         && (-1 != shape_u_sampler)
         && (-1 != shape_u_color_fill)
         && (-1 != shape_u_a_min)
         && (-1 != shape_u_a_max)
         && (-1 != shape_u_a_maxmin_scale)
         && (-1 != shape_u_a_exp)
         ;
   }

   sBool onOpen(void) {
      if(createShapeShader(vs_src, fs_src))
      {
         return YAC_TRUE;
      }
      return YAC_FALSE;
   }

   void drawTrianglesTexUVFlatVBO32AlphaSDF(sUI              _vboId,
                                            sUI              _byteOffset,
                                            sUI              _numVerts,
                                            Dsdvg_mat4_ref_t _mvpMatrix,
                                            sF32             _fillR, sF32 _fillG, sF32 _fillB, sF32 _fillA,
                                            sF32             _aMin,
                                            sF32             _aMax,
                                            sF32             _aMaxMinScale,  // (1.0/(aMax-aMin))
                                            sF32             _aExp
                                            ) {
      //
      // VBO vertex format (16 bytes per vertex):
      //     +0  f32 x
      //     +4  f32 y
      //     +8  f32 u
      //     +12 f32 v
      //

      sdvg_BindVBO(_vboId);

      shape_shader.bind();

      Dsdvg_uniform_mat4(shape_u_transform, _mvpMatrix);
      Dsdvg_uniform_4f(shape_u_color_fill, _fillR, _fillG, _fillB, _fillA);
      Dsdvg_uniform_1i(shape_u_sampler, 0);
      Dsdvg_uniform_1f(shape_u_a_min, _aMin);
      Dsdvg_uniform_1f(shape_u_a_max, _aMax);
      Dsdvg_uniform_1f(shape_u_a_maxmin_scale, _aMaxMinScale);
      Dsdvg_uniform_1f(shape_u_a_exp, _aExp);

      Dsdvg_attrib_offset(shape_a_vertex, 2/*size*/, GL_FLOAT, GL_FALSE/*normalize*/, 16/*stride*/, _byteOffset + 0);
      Dsdvg_attrib_offset(shape_a_uv,     2/*size*/, GL_FLOAT, GL_FALSE/*normalize*/, 16/*stride*/, _byteOffset + 8);

      Dsdvg_attrib_enable(shape_a_vertex);
      Dsdvg_attrib_enable(shape_a_uv);

      Dsdvg_draw_triangles_vbo(0, _numVerts);

      Dsdvg_attrib_disable(shape_a_uv);
      Dsdvg_attrib_disable(shape_a_vertex);
   }

};
