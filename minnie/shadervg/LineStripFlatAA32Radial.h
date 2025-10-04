// ----
// ---- file   : LineStripFlatAA32Radial.h
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

class LineStripFlatAA32Radial : public ShaderVG_Shape {

  public:
   // ------------ vertex shader --------------
   const char *vs_src =
      "uniform mat4  u_transform; \n"
      "uniform float u_stroke_w; \n"
      "uniform vec2 u_paint_start; \n"
      " \n"
      "ATTRIBUTE vec2  a_vertex; \n"
      "ATTRIBUTE vec2  a_vertex_n; \n"
#ifndef USE_VERTEX_ATTRIB_DIVISOR
      "ATTRIBUTE float a_index; \n"  // (todo) remove
#endif // USE_VERTEX_ATTRIB_DIVISOR
      " \n"
      "VARYING_OUT vec2 v_vertex_mp; \n"
      "VARYING_OUT vec2 v_plane_n; \n"
      "VARYING_OUT vec2 v_paint_pos; \n"
      " \n"
      "void main(void) { \n"
      "  vec2 v1 = a_vertex; \n"
      "  vec2 v2 = a_vertex_n; \n"
      "  vec2 vN = normalize(v2 - v1); \n"
      "  vec2 vD = vN * u_stroke_w; \n"
      "  vec2 v1L = vec2(v1.x + vD.y, v1.y - vD.x); \n"
      "  vec2 v2L = vec2(v2.x + vD.y, v2.y - vD.x); \n"
      "  vec2 v1R = vec2(v1.x - vD.y, v1.y + vD.x); \n"
      "  vec2 v2R = vec2(v2.x - vD.y, v2.y + vD.x); \n"
      "  vec2 v; \n"
      " \n"
#ifdef USE_VERTEX_ATTRIB_DIVISOR
      "  float index = float(gl_VertexID % 6); \n"
#else
      "  float index = a_index; \n"  // (todo) remove
#endif // USE_VERTEX_ATTRIB_DIVISOR
      " \n"
      "  if(index > 4.9) { \n"
      "    v = v1R; \n"
      "  } \n"
      "  else if(index > 3.9) { \n"
      "    v = v2R; \n"
      "  } \n"
      "  else if(index > 2.9) { \n"
      "    v = v1L; \n"
      "  } \n"
      "  else if(index > 1.9) { \n"
      "    v = v2R; \n"
      "  } \n"
      "  else if(index > 0.9) { \n"
      "    v = v2L; \n"
      "  } \n"
      "  else { \n"
      "    v = v1L; \n"
      "  } \n"
      " \n"
      "  gl_Position = u_transform * vec4(v,0,1); \n"
      "  v_vertex_mp = v - v1; \n"
      "  v_plane_n   = vec2(vN.y, -vN.x); \n"
      "  v_paint_pos = (v - u_paint_start); \n"
      "} \n"
      ;

   // ------------ fragment shader ------------
   const char *fs_src =
      "uniform vec4      u_color_stroke; \n"
      "uniform float     u_stroke_w; \n"
      "uniform float     u_aa_range; \n"
      "uniform float     u_debug; \n"
      "uniform sampler2D u_paint_tex; \n"
      "uniform float     u_paint_ob_len; \n"
      " \n"
      "VARYING_IN vec2 v_vertex_mp; \n"
      "VARYING_IN vec2 v_plane_n; \n"
      "VARYING_IN vec2 v_paint_pos; \n"
      " \n"
      "void main(void) { \n"
      "  float d = abs(dot(v_vertex_mp, v_plane_n)); \n"
      "  float a = 1.0 - smoothstep(u_stroke_w - u_aa_range, u_stroke_w, d); \n"
      "  float dp = length(v_paint_pos) * u_paint_ob_len; \n"
      "  vec4 cp = TEXTURE2D(u_paint_tex, vec2(dp, 0.0)); \n"
      "  FRAGCOLOR = vec4(u_color_stroke.rgb * cp.rgb, u_color_stroke.a * cp.a * a); \n"
      "  if(u_debug > 0.0) { \n"
      "    FRAGCOLOR = vec4(u_color_stroke.r, a, u_color_stroke.b, u_color_stroke.a); \n"
      "  } \n"
      "} \n"
      ;

   sBool validateShapeShader(void) {
      return
            (-1 != shape_a_vertex)
         && (-1 != shape_a_vertex_n)
#ifndef USE_VERTEX_ATTRIB_DIVISOR
         && (-1 != shape_a_index)  // (todo) remove
#endif // USE_VERTEX_ATTRIB_DIVISOR
         && (-1 != shape_u_transform)
         && (-1 != shape_u_color_stroke)
         && (-1 != shape_u_stroke_w)
         && (-1 != shape_u_aa_range)
         && (-1 != shape_u_paint_start)
         && (-1 != shape_u_paint_tex)
         && (-1 != shape_u_paint_ob_len)
         ;
   }

   sBool onOpen(void) {
      if(createShapeShader(vs_src, fs_src))
      {
         return YAC_TRUE;
      }
      return YAC_FALSE;
   }

   // (note) see also: shadervg.cpp:loc_DrawLineStripFlatAAVBOGradient()

};
