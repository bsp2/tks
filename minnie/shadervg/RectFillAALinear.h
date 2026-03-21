// ----
// ---- file   : RectFillAALinear.h
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

class RectFillAALinear : public ShaderVG_Shape {

  public:
   // ------------ vertex shader --------------
   const char *vs_src =
      "uniform mat4 u_transform; \n"
      "uniform vec2 u_paint_start; \n"
      " \n"
      "ATTRIBUTE vec2 a_vertex; \n"
      " \n"
      "VARYING_OUT vec2 v_p; \n"
      "VARYING_OUT vec2 v_paint_pos; \n"
      " \n"
      "void main(void) { \n"
      "  v_p = a_vertex; \n"
      "  gl_Position = u_transform * vec4(a_vertex,0,1); \n"
      "  v_paint_pos = (a_vertex - u_paint_start); \n"
      "} \n"
      ;

   // ------------ fragment shader ------------
   const char *fs_src =
      "uniform vec2  u_center; \n"
      "uniform vec2  u_size; \n"
      "uniform float u_aa_range; \n"
#ifdef SHADERVG_AA_EXP
      "uniform float u_aa_exp; \n"
#endif // SHADERVG_AA_EXP
      "uniform vec4  u_color_fill; \n"
#ifdef SHADERVG_DEBUG_FRAG
      "uniform float u_debug; \n"
#endif // SHADERVG_DEBUG_FRAG
      "uniform sampler2D u_paint_tex; \n"
      "uniform vec2  u_paint_ndir; \n"
      "uniform float u_paint_ob_len; \n"
      " \n"
      "VARYING_IN vec2 v_p; \n"
      "VARYING_IN vec2 v_paint_pos; \n"
      " \n"
      "void main(void) { \n"
      "  float aRect = 0.0; \n"
      "  vec4 color = vec4(0,0,0,0); \n"
      "  // outer \n"
      "  vec2 vd = abs(v_p - u_center); \n"
      // // if(vd.x < u_size.x && vd.y < u_size.y) \n"
      "  { \n"
      "    aRect  = 1.0 - smoothstep(u_size.x-u_aa_range, u_size.x, vd.x); \n"
      "    aRect *= 1.0 - smoothstep(u_size.y-u_aa_range, u_size.y, vd.y); \n"
      "    color = u_color_fill; \n"
      "  } \n"
      "  \n"
      "  float a = aRect; \n"
      "  \n"
      "  // a = smoothstep(0.0, 1.0, a); \n"
#ifdef SHADERVG_AA_EXP
      "  a = pow(a, u_aa_exp); \n"
#endif // SHADERVG_AA_EXP
      "  float d = dot(v_paint_pos, u_paint_ndir) * u_paint_ob_len; \n"
      "  vec4 c = TEXTURE2D(u_paint_tex, vec2(d, 0.0)); \n"
      "  FRAGCOLOR = vec4(c.rgb * color.rgb, c.a * color.a * a); \n"
#ifdef SHADERVG_DEBUG_FRAG
      "  if(u_debug > 0.0) \n"
      "    FRAGCOLOR = vec4(1,0,0,1); \n"
#endif // SHADERVG_DEBUG_FRAG
      "} \n"
      ;

   sBool onOpen(void) {
      if(createShapeShader(vs_src, fs_src))
      {
         return YAC_TRUE;
      }
      return YAC_FALSE;
   }

};
