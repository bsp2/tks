// ----
// ---- file   : RoundRectFillAAPatternDecal.h
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

class RoundRectFillAAPatternDecal : public ShaderVG_Shape {

  public:
   // ------------ vertex shader --------------
   const char *vs_src =
      "uniform mat4 u_transform; \n"
      "uniform vec2 u_paint_start; \n"
      "uniform vec2 u_paint_ob_size; \n"
      "uniform float u_paint_ob_len; \n"
      " \n"
      "ATTRIBUTE vec2 a_vertex; \n"
      " \n"
      "VARYING_OUT vec2 v_p; \n"
      "VARYING_OUT vec2 v_paint_uv; \n"
      " \n"
      "void main(void) { \n"
      "  v_p = a_vertex; \n"
      "  gl_Position = u_transform * vec4(a_vertex,0,1); \n"
      "  v_paint_uv  = (a_vertex - u_paint_start) * u_paint_ob_size * u_paint_ob_len; \n"
      "} \n"
      ;

   // ------------ fragment shader ------------
   const char *fs_src =
      "uniform vec2  u_center; \n"
      "uniform vec2  u_size; \n"
      "uniform vec2  u_radius; \n"
      "uniform vec2  u_ob_radius; \n"
      "uniform float u_radius_max; \n"
      "uniform float u_ob_radius_max; \n"
      "uniform float u_aa_range; \n"
      "uniform float u_aa_exp; \n"
      "uniform vec4  u_color_fill; \n"
      "uniform vec4  u_color_stroke; \n"
      "uniform float u_decal_alpha; \n"
      "uniform float u_debug; \n"
      "uniform sampler2D u_paint_tex; \n"
      "uniform vec2 u_paint_ndir; \n"
      " \n"
      "VARYING_IN vec2 v_p; \n"
      "VARYING_IN vec2 v_paint_uv; \n"
      " \n"
      "void main(void) { \n"
      "  float aRect = 0.0; \n"
      "  float aRound = 1.0; \n"
      "  vec4 color = vec4(0,0,0,0); \n"
      " \n"
      "  // outer \n"
      "  vec2 vd = abs(v_p - u_center); \n"
      // // if(vd.x < u_size.x && vd.y < u_size.y)
      "  { \n"
      "    aRect  = 1.0 - smoothstep(u_size.x - u_aa_range, u_size.x, vd.x); \n"
      "    aRect *= 1.0 - smoothstep(u_size.y - u_aa_range, u_size.y, vd.y); \n"
      "    color = u_color_fill; \n"
      " \n"
      "    vd = vd - (u_size - u_radius); \n"
      " \n"
      "    if(vd.x > 0.0 && vd.y > 0.0) \n"
      "    { \n"
      "      vec2 vdn = vd * u_ob_radius; \n"
      "      float as = asin(vdn.x) * (1.0 / 3.14159265359); \n"
      "      float r = mix(u_radius.y, u_radius.x, as); \n"
      "      float r2 = r * u_ob_radius_max; \n"
      "      float aaR = u_aa_range * r2; \n"
      "      aRound = 1.0 - smoothstep( (u_radius_max - aaR) * u_ob_radius_max, 1.0, length(vdn)); \n"
      "    } \n"
      "  } \n"
      " \n"
      "  float a = aRect * aRound; \n"
      "#if 1 \n"
      "  a = pow(a, u_aa_exp); \n"
      "#endif \n"
      " \n"
      "  vec2 uv; \n"
      "  uv.x = v_paint_uv.x * u_paint_ndir.x - v_paint_uv.y * u_paint_ndir.y; \n"
      "  uv.y = v_paint_uv.x * u_paint_ndir.y + v_paint_uv.y * u_paint_ndir.x; \n"
      "  vec4 cp = TEXTURE2D(u_paint_tex, uv); \n"
      "  FRAGCOLOR = vec4(mix(color.rgb, cp.rgb * u_color_stroke.rgb, u_color_stroke.a * cp.a * u_decal_alpha), color.a * a); \n"
      "  if(u_debug > 0.0) \n"
      "    FRAGCOLOR = vec4(1,0,0,1); \n"
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
