// ----
// ---- file   : PolygonFillFlat14_2PatternAlpha.h
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

class PolygonFillFlat14_2PatternAlpha : public ShaderVG_Shape {

  public:
   // ------------ vertex shader --------------
   const char *vs_src =
      "uniform mat4 u_transform; \n"
      " \n"
      "ATTRIBUTE vec2 a_vertex; \n"
      " \n"
      "void main(void) { \n"
      "  gl_Position = u_transform * vec4(a_vertex*0.25,0,1); \n"
      "} \n"
      ;

   // ------------ fragment shader ------------
   const char *fs_src =
      "uniform vec4      u_color_fill; \n"
      "uniform sampler2D u_paint_tex; \n"
      "uniform vec2      u_paint_start; \n"
      "uniform vec2      u_paint_ndir; \n"
      "uniform vec2      u_paint_size; \n"
      "uniform vec2      u_paint_ob_size; \n"
      "uniform mat4      u_paint_mat_unproject; \n"
      "uniform vec4      u_paint_vp_unproject; \n"
      " \n"
      "void main(void) { \n"
      "  vec2 v = gl_FragCoord.xy; \n"
      "  v -= u_paint_vp_unproject.xy; \n"
      "  v *= u_paint_vp_unproject.zw; \n"
      "  v = (u_paint_mat_unproject * vec4(v,0,1)).xy; \n"
      "  vec2 paintUV = (v - u_paint_start) * u_paint_ob_size; \n"
      "  vec2 uv; \n"
      "  uv.x = paintUV.x * u_paint_ndir.x - paintUV.y * u_paint_ndir.y; \n"
      "  uv.y = paintUV.x * u_paint_ndir.y + paintUV.y * u_paint_ndir.x; \n"
      "  float a = TEXTURE2D(u_paint_tex, uv).TEX_ALPHA; \n"
      "  FRAGCOLOR = vec4(u_color_fill.rgb, u_color_fill.a * a); \n"
      "} \n"
      ;

   sBool validateShapeShader(void) {
      return
            (-1 != shape_a_vertex)
         && (-1 != shape_u_transform)
         && (-1 != shape_u_color_fill)
         && (-1 != shape_u_paint_start)
         && (-1 != shape_u_paint_ob_size)
         && (-1 != shape_u_paint_ndir)
         && (-1 != shape_u_paint_tex)
         && (-1 != shape_u_paint_mat_unproject)
         && (-1 != shape_u_paint_vp_unproject)
         ;
   }

   sBool onOpen(void) {
      if(createShapeShader(vs_src, fs_src))
      {
         return YAC_TRUE;
      }
      return YAC_FALSE;
   }

};
