// ----
// ---- file   : RoundRectStrokeAAConic.h
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : Distributed under terms of the MIT license (https://opensource.org/licenses/MIT)
// ----          Copyright 2025-2026 by bsp
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

class RoundRectStrokeAAConic : public ShaderVG_Shape {

  public:
   // ------------ vertex shader --------------
   const char *vs_src =
      "uniform mat4 u_transform; \n"
      "uniform vec2 u_paint_start; \n"
      "uniform vec2 u_paint_ob_size; \n"
      " \n"
      "ATTRIBUTE vec2 a_vertex; \n"
      " \n"
      "VARYING_OUT vec2 v_p; \n"
      "VARYING_OUT vec2 v_paint_pos; \n"
      " \n"
      "void main(void) { \n"
      "  v_p = a_vertex; \n"
      "  gl_Position = u_transform * vec4(a_vertex,0,1); \n"
      "  v_paint_pos = (a_vertex - u_paint_start) * u_paint_ob_size; \n"
      "} \n"
      ;

   // ------------ fragment shader ------------
   const char *fs_src =
      "uniform vec2  u_center; \n"
      "uniform vec2  u_size_i; \n"
      "uniform vec2  u_size_o; \n"
      "uniform vec2  u_size; \n"
      "uniform vec2  u_radius; \n"
      "uniform vec2  u_radius_i; \n"
      "uniform vec2  u_radius_o; \n"
      "uniform vec2  u_ob_radius_i; \n"
      "uniform vec2  u_ob_radius_o; \n"
      "uniform float u_ob_radius_i_max; \n"
      "uniform float u_ob_radius_o_max; \n"
      "uniform float u_radius_i_max; \n"
      "uniform float u_radius_o_max; \n"
      "uniform float u_aa_range; \n"
#ifdef SHADERVG_AA_EXP
      "uniform float u_aa_exp; \n"
#endif // SHADERVG_AA_EXP
      "uniform vec4  u_color_stroke; \n"
#ifdef SHADERVG_DEBUG_FRAG
      "uniform float u_debug; \n"
#endif // SHADERVG_DEBUG_FRAG
      "uniform sampler2D u_paint_tex; \n"
      "uniform float u_paint_angle01; \n"
      " \n"
      "VARYING_IN vec2 v_p; \n"
      "VARYING_IN vec2 v_paint_pos; \n"
      " \n"
      "void main(void) { \n"
      "  float aRectI = 0.0; \n"
      "  float aRectO = 0.0; \n"
      "  float aRoundI = 1.0; \n"
      "  float aRoundO = 1.0; \n"
      "  vec4 colorO = vec4(0,0,0,0); \n"
      " \n"
      "  // outer \n"
      "  vec2 vd = abs(v_p - u_center); \n"
      "  // // if(vd.x < u_size_o.x && vd.y < u_size_o.y) \n"
      "  { \n"
      "    aRectO  = 1.0 - smoothstep(u_size_o.x-u_aa_range, u_size_o.x, vd.x); \n"
      "    aRectO *= 1.0 - smoothstep(u_size_o.y-u_aa_range, u_size_o.y, vd.y); \n"
      "    colorO = u_color_stroke; \n"
      " \n"
      "    vd = vd - (u_size - u_radius); \n"
      " \n"
      "    if(vd.x > 0.0 && vd.y > 0.0) \n"
      "    { \n"
      "      vec2 vdn = vd * u_ob_radius_o; \n"
      "      float as = asin(vdn.x) * (1.0 / 3.14159265359); \n"
      "      float r = mix(u_radius_o.y, u_radius_o.x, as); \n"
      "      float r2 = r * u_ob_radius_o_max; \n"
      "      float aaR = u_aa_range * r2; \n"
      "      aRoundO = 1.0 - smoothstep( (u_radius_o_max - aaR) * u_ob_radius_o_max, 1.0, length(vdn)); \n"
      "    } \n"
      "  } \n"
      " \n"
      "  // inner \n"
      "  vd = abs(v_p - u_center); \n"
      "  if(vd.x < u_size_i.x && vd.y < u_size_i.y) \n"
      "  { \n"
      "    aRectI  = 1.0 - smoothstep(u_size_i.x - u_aa_range, u_size_i.x, vd.x); \n"
      "    aRectI *= 1.0 - smoothstep(u_size_i.y - u_aa_range, u_size_i.y, vd.y); \n"
      " \n"
      "    vd = vd - (u_size - u_radius); \n"
      " \n"
      "    if(vd.x > 0.0 && vd.y > 0.0) \n"
      "    { \n"
      "      vec2 vdn = vd * u_ob_radius_i; \n"
      "      float as = asin(vdn.x) * (1.0 / 3.14159265359); \n"
      "      float r = mix(u_radius_i.y, u_radius_i.x, as); \n"
      "      float r2 = r * u_ob_radius_i_max; \n"
      "      float aaR = u_aa_range * r2; \n"
      "      aRoundI = 1.0 - smoothstep( (u_radius_i_max - aaR) * u_ob_radius_i_max, 1.0, length(vdn) ); \n"
      "    } \n"
      "  } \n"
      " \n"
      "  float aI = 1.0 - (aRectI * aRoundI); \n"
      "  float aO = aRectO * aRoundO; \n"
      " \n"
#ifdef SHADERVG_AA_EXP
      "  aI = pow(aI, u_aa_exp); \n"
#endif // SHADERVG_AA_EXP
      "  // vec4 color = mix(colorO, colorI, aI); \n"
      "  vec4 color = vec4(colorO.xyz, colorO.a * aI); \n"
      " \n"
#ifdef SHADERVG_AA_EXP
      "  aO = pow(aO, u_aa_exp); \n"
#endif // SHADERVG_AA_EXP
      " \n"
      "  vec2 n = normalize(abs(v_paint_pos)); \n"
      "  float ap = atan(n.y / n.x) * (1.0 / 6.2831853071795864); \n"
      "  if(v_paint_pos.x > 0.0) {\n"
      "    if(v_paint_pos.y < 0.0) { \n"
      "      ap = 1.0 - ap; \n"
      "    } \n"
      "  } \n"
      "  else if(v_paint_pos.y < 0.0) { \n"
      "      ap += 0.5; \n"
      "  } \n"
      "  else { \n"
      "    ap = 0.5 - ap; \n"
      "  } \n"
      "  ap += u_paint_angle01; \n"
      "  if(ap >= 1.0) ap -= 1.0; \n"   // (note) use texture repeat ?
      "  else if(ap < 0.0) ap += 1.0; \n"
      "  vec4 c = TEXTURE2D(u_paint_tex, vec2(ap, 0.0)); \n"
      "  FRAGCOLOR = vec4(c.rgb * color.rgb, c.a * color.a * aO); \n"
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
