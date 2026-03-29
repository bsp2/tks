// ----
// ---- file   : PointsRoundAA14_2Conic.h
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

class PointsRoundAA14_2Conic : public ShaderVG_Shape {

  public:
   // ------------ vertex shader --------------
   const char *vs_src =
      "uniform mat4  u_transform; \n"
      "uniform float u_point_radius; \n"
#ifdef SHADERVG_UNIFORM_ARRAY
      "uniform vec2  u_a_offset[6]; \n"
#endif // SHADERVG_UNIFORM_ARRAY
      "uniform vec2  u_paint_start; \n"
      "uniform vec2  u_paint_scale; \n"
      " \n"
      "ATTRIBUTE vec2 a_vertex; \n"
      " \n"
      "VARYING_OUT vec2 v_vertex_mp; \n"
      "VARYING_OUT vec2 v_paint_pos; \n"
      " \n"
      "void main(void) { \n"
      "  vec2 vCtr = a_vertex * 0.25; \n"
      "  vec2 v; \n"
      " \n"
#ifdef SHADERVG_UNIFORM_ARRAY
      "  v = vCtr + u_a_offset[int(gl_VertexID)]; \n"
#else
      "  float index = float(gl_VertexID); \n"
      " \n"
      "  if(index > 4.9) { \n"
      "    v = vec2(vCtr.x - u_point_radius, vCtr.y + u_point_radius); \n"  // LB
      "  } \n"
      "  else if(index > 3.9) { \n"
      "    v = vCtr + vec2(u_point_radius, u_point_radius); \n"  // RB
      "  } \n"
      "  else if(index > 2.9) { \n"
      "    v = vCtr - vec2(u_point_radius, u_point_radius); \n"  // LT
      "  } \n"
      "  else if(index > 1.9) { \n"
      "    v = vCtr + vec2(u_point_radius, u_point_radius); \n"  // RB
      "  } \n"
      "  else if(index > 0.9) { \n"
      "    v = vec2(vCtr.x + u_point_radius, vCtr.y - u_point_radius); \n"  // RT
      "  } \n"
      "  else { \n"
      "    v = vCtr - vec2(u_point_radius, u_point_radius); \n"  // LT
      "  } \n"
#endif // SHADERVG_UNIFORM_ARRAY
      " \n"
      "  gl_Position = u_transform * vec4(v,0,1); \n"
      "  v_vertex_mp = v - vCtr; \n"
      "  v_paint_pos = (v - u_paint_start) * u_paint_scale; \n"
      "} \n"
      ;

   // ------------ fragment shader ------------
   const char *fs_src =
      "uniform vec4  u_color_stroke; \n"
      "uniform float u_point_radius; \n"
      "uniform float u_aa_range; \n"
#ifdef SHADERVG_DEBUG_FRAG
      "uniform float u_debug; \n"
#endif // SHADERVG_DEBUG_FRAG
      "uniform sampler2D u_paint_tex; \n"
      "uniform float     u_paint_angle01; \n"
      " \n"
      "VARYING_IN vec2 v_vertex_mp; \n"
      "VARYING_IN vec2 v_paint_pos; \n"
      " \n"
      "void main(void) { \n"
      "  vec2 vd = abs(v_vertex_mp); \n"
      "  float d = length(vd); \n"
      // aa
      "  float a = 1.0 - smoothstep(u_point_radius - u_aa_range, u_point_radius, d); \n"
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
      "  vec4 cp = TEXTURE2D(u_paint_tex, vec2(ap, 0.0)); \n"
      "  FRAGCOLOR = vec4(u_color_stroke.rgb * cp.rgb, u_color_stroke.a * cp.a * a); \n"
#ifdef SHADERVG_DEBUG_FRAG
      "  if(u_debug > 0.0) { \n"
      "    FRAGCOLOR = vec4(1.0, a, a, 1.0); \n"
      "  } \n"
#endif // SHADERVG_DEBUG_FRAG
      "} \n"
      ;

   sBool validateShapeShader(void) {
      return
            (-1 != shape_a_vertex)
         && (-1 != shape_u_transform)
         && (-1 != shape_u_color_stroke)
         && (-1 != shape_u_point_radius)
#ifdef SHADERVG_UNIFORM_ARRAY
         && (-1 != shape_u_a_offset)
#endif // SHADERVG_UNIFORM_ARRAY
         && (-1 != shape_u_aa_range)
         && (-1 != shape_u_paint_start)
         && (-1 != shape_u_paint_tex)
         && (-1 != shape_u_paint_angle01)
         ;
   }

   sBool onOpen(void) {
      if(createShapeShader(vs_src, fs_src))
      {
         return YAC_TRUE;
      }
      return YAC_FALSE;
   }

   // see also: Shape::drawPointsRoundAAVBO14_2Paint()
};
