// ----
// ---- file   : LineStripPatternAA14_2.h
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

class LineStripPatternAA14_2 : public ShaderVG_Shape {

  public:
   // ------------ vertex shader --------------
   const char *vs_src =
      "uniform mat4  u_transform; \n"
      "uniform float u_stroke_w; \n"
      "uniform float u_line_pattern_scl; \n"
      "uniform float u_line_pattern_off; \n"
      " \n"
      "ATTRIBUTE vec2  a_vertex; \n"
      "ATTRIBUTE vec2  a_vertex_n; \n"
      "ATTRIBUTE float a_pattern; \n"
      "ATTRIBUTE float a_pattern_n; \n"
      " \n"
      "VARYING_OUT vec2 v_vertex_mp; \n"
      "VARYING_OUT vec2 v_plane_n; \n"
      "VARYING_OUT vec2 v_uv; \n"
      " \n"
      "void main(void) { \n"
      "  vec2 v1 = a_vertex * 0.25; \n"
      "  vec2 v2 = a_vertex_n * 0.25; \n"
      "  vec2 vN = normalize(v2 - v1); \n"
      "  vec2 vD = vN * u_stroke_w; \n"
      "  vec2 v1L = vec2(v1.x + vD.y, v1.y - vD.x); \n"
      "  vec2 v2L = vec2(v2.x + vD.y, v2.y - vD.x); \n"
      "  vec2 v1R = vec2(v1.x - vD.y, v1.y + vD.x); \n"
      "  vec2 v2R = vec2(v2.x - vD.y, v2.y + vD.x); \n"
      "  vec2 v; \n"
      "  float pat1 = a_pattern; \n"
      "  float pat2 = a_pattern_n; \n"
      "  vec2 uv; \n"
      " \n"
      "  float index = float(gl_VertexID); \n"
      " \n"
      "  if(index > 4.9) { \n"
      "    v = v1R; \n"
      "    uv = vec2(pat1, 1.0); \n"
      "  } \n"
      "  else if(index > 3.9) { \n"
      "    v = v2R; \n"
      "    uv = vec2(pat2, 1.0); \n"
      "  } \n"
      "  else if(index > 2.9) { \n"
      "    v = v1L; \n"
      "    uv = vec2(pat1, 0.0); \n"
      "  } \n"
      "  else if(index > 1.9) { \n"
      "    v = v2R; \n"
      "    uv = vec2(pat2, 1.0); \n"
      "  } \n"
      "  else if(index > 0.9) { \n"
      "    v = v2L; \n"
      "    uv = vec2(pat2, 0.0); \n"
      "  } \n"
      "  else { \n"
      "    v = v1L; \n"
      "    uv = vec2(pat1, 0.0); \n"
      "  } \n"
      " \n"
      "  gl_Position = u_transform * vec4(v,0,1); \n"
      "  v_vertex_mp = v - v1; \n"
      "  v_plane_n   = vec2(vN.y, -vN.x); \n"
      "  uv.x *= u_line_pattern_scl; \n"
      "  uv.x += u_line_pattern_off; \n"
      "  v_uv = uv; \n"
      "} \n"
      ;

   // ------------ fragment shader ------------
   const char *fs_src =
      "uniform vec4      u_color_stroke; \n"
      "uniform float     u_stroke_w; \n"
      "uniform float     u_aa_range; \n"
      "uniform float     u_debug; \n"
      "uniform sampler2D u_sampler; \n"
      " \n"
      "VARYING_IN vec2 v_vertex_mp; \n"
      "VARYING_IN vec2 v_plane_n; \n"
      "VARYING_IN vec2 v_uv; \n"
      " \n"
      "void main(void) { \n"
      "  float d = abs(dot(v_vertex_mp, v_plane_n)); \n"
      "  float a = 1.0 - smoothstep(u_stroke_w - u_aa_range, u_stroke_w, d); \n"
      "  float patA = TEXTURE2D(u_sampler, v_uv).TEX_ALPHA; \n"
      "  FRAGCOLOR = vec4(u_color_stroke.rgb, u_color_stroke.a * a * patA); \n"
      "  if(u_debug > 0.0) { \n"
      "    FRAGCOLOR = vec4(a, fract(v_uv.x), fract(v_uv.y), 1); \n"
      "  } \n"
      "} \n"
      ;

   sBool validateShapeShader(void) {
      return
            (-1 != shape_a_vertex)
         && (-1 != shape_a_vertex_n)
         && (-1 != shape_u_transform)
         && (-1 != shape_u_color_stroke)
         && (-1 != shape_u_stroke_w)
         && (-1 != shape_u_aa_range)
         && (-1 != shape_u_sampler)
         && (-1 != shape_a_pattern)
         && (-1 != shape_a_pattern_n)
         ;
   }

   sBool onOpen(void) {
      if(createShapeShader(vs_src, fs_src))
      {
         return YAC_TRUE;
      }
      return YAC_FALSE;
   }

   void drawLineStripPatternAAVBO14_2(sUI              _vboId,
                                      sUI              _byteOffset,
                                      sUI              _numPoints,
                                      Dsdvg_mat4_ref_t _mvpMatrix,
                                      sF32             _strokeR, sF32 _strokeG, sF32 _strokeB, sF32 _strokeA,
                                      sF32             _strokeW,
                                      sF32             _aaRange,
                                      sF32             _linePatternScale,
                                      sF32             _linePatternOffset
                                      ) {
      //
      // VBO vertex format (6 bytes per vertex):
      //   +0  s14.2 x
      //   +2  s14.2 y
      //   +4  s14.2 patternOff
      //
      // (note) numSeg = (numPoints - 1)
      // (note) numTri = numSeg * 2
      //

      sdvg_BindVBO(_vboId);

      shape_shader.bind();

      Dsdvg_uniform_mat4(shape_u_transform, _mvpMatrix);
      Dsdvg_uniform_4f(shape_u_color_stroke, _strokeR, _strokeG, _strokeB, _strokeA);
      Dsdvg_uniform_1f(shape_u_stroke_w, _strokeW);
      Dsdvg_uniform_1f(shape_u_aa_range, _aaRange);
      if(-1 != shape_u_debug)
      {
         Dsdvg_uniform_1f(shape_u_debug, b_debug ? 1.0f : 0.0f);
      }
      Dsdvg_uniform_1i(shape_u_sampler, 0);
      Dsdvg_uniform_1f(shape_u_line_pattern_scl, _linePatternScale * 0.25f);
      Dsdvg_uniform_1f(shape_u_line_pattern_off, _linePatternOffset);

      Dsdvg_attrib_offset(shape_a_vertex,    2/*size*/, GL_SHORT, GL_FALSE/*normalize*/, 6/*stride*/, _byteOffset +  0);
      Dsdvg_attrib_offset(shape_a_vertex_n,  2/*size*/, GL_SHORT, GL_FALSE/*normalize*/, 6/*stride*/, _byteOffset +  6);
      Dsdvg_attrib_offset(shape_a_pattern,   1/*size*/, GL_SHORT, GL_FALSE/*normalize*/, 6/*stride*/, _byteOffset +  4);
      Dsdvg_attrib_offset(shape_a_pattern_n, 1/*size*/, GL_SHORT, GL_FALSE/*normalize*/, 6/*stride*/, _byteOffset + 10);

      Dsdvg_attrib_enable(shape_a_vertex);
      Dsdvg_attrib_enable(shape_a_vertex_n);
      Dsdvg_attrib_enable(shape_a_pattern);
      Dsdvg_attrib_enable(shape_a_pattern_n);

      Dsdvg_attrib_divisor(shape_a_vertex, 1);
      Dsdvg_attrib_divisor(shape_a_vertex_n, 1);
      Dsdvg_attrib_divisor(shape_a_pattern, 1);
      Dsdvg_attrib_divisor(shape_a_pattern_n, 1);

      const sUI numInstances = (_numPoints - 1);
      Dsdvg_draw_triangles_instanced_vbo(6, numInstances);

      Dsdvg_attrib_disable(shape_a_vertex_n);
      Dsdvg_attrib_disable(shape_a_vertex);
      Dsdvg_attrib_disable(shape_a_pattern_n);
      Dsdvg_attrib_disable(shape_a_pattern);

      Dsdvg_attrib_divisor_reset(shape_a_vertex);
      Dsdvg_attrib_divisor_reset(shape_a_vertex_n);
      Dsdvg_attrib_divisor_reset(shape_a_pattern);
      Dsdvg_attrib_divisor_reset(shape_a_pattern_n);
   }

};
