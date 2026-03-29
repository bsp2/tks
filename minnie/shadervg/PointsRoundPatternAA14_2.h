// ----
// ---- file   : PointsRoundPatternAA14_2.h
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

class PointsRoundPatternAA14_2 : public ShaderVG_Shape {

  public:
   // ------------ vertex shader --------------
   const char *vs_src =
      "uniform mat4  u_transform; \n"
      "uniform float u_point_radius; \n"
#ifdef SHADERVG_UNIFORM_ARRAY
      "uniform vec2  u_a_offset[6]; \n"
#endif // SHADERVG_UNIFORM_ARRAY
      " \n"
      "ATTRIBUTE vec2  a_vertex; \n"
      "ATTRIBUTE vec2  a_vertex_n; \n"
      "ATTRIBUTE float a_pattern; \n"
      " \n"
      "VARYING_OUT vec2 v_vertex_mp; \n"
      "flat VARYING_OUT vec2 v_plane_n; \n"
      "flat VARYING_OUT float v_pat; \n"
      " \n"
      "void main(void) { \n"
      "  vec2 v1 = a_vertex * 0.25; \n"
      "  vec2 v2 = a_vertex_n * 0.25; \n"
      "  v_plane_n = normalize(v2 - v1); \n"
      "  vec2 v; \n"
      "  v_pat = a_pattern * 0.25; \n"
      " \n"
#ifdef SHADERVG_UNIFORM_ARRAY
      "  v = v1 + u_a_offset[int(gl_VertexID)]; \n"
#else
      "  float index = float(gl_VertexID); \n"
      " \n"
      "  if(index > 4.9) { \n"
      "    v = vec2(v1.x - u_point_radius, v1.y + u_point_radius); \n"  // LB
      "  } \n"
      "  else if(index > 3.9) { \n"
      "    v = v1 + vec2(u_point_radius, u_point_radius); \n"  // RB
      "  } \n"
      "  else if(index > 2.9) { \n"
      "    v = v1 - vec2(u_point_radius, u_point_radius); \n"  // LT
      "  } \n"
      "  else if(index > 1.9) { \n"
      "    v = v1 + vec2(u_point_radius, u_point_radius); \n"  // RB
      "  } \n"
      "  else if(index > 0.9) { \n"
      "    v = vec2(v1.x + u_point_radius, v1.y - u_point_radius); \n"  // RT
      "  } \n"
      "  else { \n"
      "    v = v1 - vec2(u_point_radius, u_point_radius); \n"  // LT
      "  } \n"
#endif // SHADERVG_UNIFORM_ARRAY
      " \n"
      "  gl_Position = u_transform * vec4(v,0,1); \n"
      "  v_vertex_mp = v - v1; \n"
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
      "uniform sampler2D u_sampler; \n"
      "uniform float u_line_pattern_scl; \n"
      "uniform float u_line_pattern_off; \n"
      " \n"
      "VARYING_IN vec2 v_vertex_mp; \n"
      "flat VARYING_IN vec2 v_plane_n; \n"
      "flat VARYING_IN float v_pat; \n"
      " \n"
      "void main(void) { \n"
      "  vec2 vd = abs(v_vertex_mp); \n"
      "  float d = length(vd); \n"
      // aa
      "  float dp = dot(v_vertex_mp, v_plane_n); \n"
      "  float a = 1.0 - smoothstep(u_point_radius - u_aa_range, u_point_radius, d); \n"
      "  float u = (dp + v_pat) * u_line_pattern_scl + u_line_pattern_off; \n"
      "  float patA = TEXTURE2D(u_sampler, vec2(u, 0.0)).TEX_ALPHA; \n"
      "  FRAGCOLOR = vec4(u_color_stroke.rgb, u_color_stroke.a * a * patA); \n"
#ifdef SHADERVG_DEBUG_FRAG
      "  if(u_debug > 0.0) { \n"
      "    FRAGCOLOR = vec4(1.0/*patA*/, u, a, 1.0); \n"
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
         && (-1 != shape_u_sampler)
         && (-1 != shape_u_line_pattern_scl)
         && (-1 != shape_u_line_pattern_off)
         ;
   }

   sBool onOpen(void) {
      if(createShapeShader(vs_src, fs_src))
      {
         return YAC_TRUE;
      }
      return YAC_FALSE;
   }

   void drawPointsRoundPatternAAVBO14_2(sUI              _vboId,
                                        sUI              _byteOffset,
                                        sUI              _numVertices,
                                        Dsdvg_mat4_ref_t _mvpMatrix,
                                        sF32             _strokeR, sF32 _strokeG, sF32 _strokeB, sF32 _strokeA,
                                        sF32             _pointRadius,
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

      Ddebugprintf("[trc] DrawPointsRoundPatternAAVBO14_2: vboId=%u byteOffset=%u numVertices=%u pointRadius=%f aaRange=%f patternScale=%f patternOffset=%f\n", _vboId, _byteOffset, _numVertices, _pointRadius, _aaRange, _linePatternScale, _linePatternOffset);

      if(_numVertices >= 2u)
      {
         sdvg_BindVBO(_vboId);

         shape_shader.bind();

         Dsdvg_uniform_mat4(shape_u_transform, _mvpMatrix);
         Dsdvg_uniform_4f(shape_u_color_stroke, _strokeR, _strokeG, _strokeB, _strokeA);
         Dsdvg_uniform_1f(shape_u_point_radius, _pointRadius);
#ifdef SHADERVG_UNIFORM_ARRAY
         updateUniformOffsetArray(_pointRadius);
#endif // SHADERVG_UNIFORM_ARRAY
         Dsdvg_uniform_1f(shape_u_aa_range, _aaRange);
#ifdef SHADERVG_DEBUG_FRAG
         if(-1 != shape_u_debug)
         {
            Dsdvg_uniform_1f(shape_u_debug, b_debug ? 1.0f : 0.0f);
         }
#endif // SHADERVG_DEBUG_FRAG
         Dsdvg_uniform_1i(shape_u_sampler, 0);
         Dsdvg_uniform_1f(shape_u_line_pattern_scl, _linePatternScale);
         Dsdvg_uniform_1f(shape_u_line_pattern_off, _linePatternOffset);

         Dsdvg_attrib_offset(shape_a_vertex,   2/*size*/, GL_SHORT, GL_FALSE/*normalize*/, 6/*stride*/, _byteOffset + 0);
         Dsdvg_attrib_offset(shape_a_vertex_n, 2/*size*/, GL_SHORT, GL_FALSE/*normalize*/, 6/*stride*/, _byteOffset + 6);
         Dsdvg_attrib_offset(shape_a_pattern,  1/*size*/, GL_SHORT, GL_FALSE/*normalize*/, 6/*stride*/, _byteOffset + 4);

         Dsdvg_attrib_enable(shape_a_vertex);
         Dsdvg_attrib_enable(shape_a_vertex_n);
         Dsdvg_attrib_enable(shape_a_pattern);

         Dsdvg_attrib_divisor(shape_a_vertex, 1);
         Dsdvg_attrib_divisor(shape_a_vertex_n, 1);
         Dsdvg_attrib_divisor(shape_a_pattern, 1);

         const sUI numInstances = (_numVertices - 1);
         Dsdvg_draw_triangles_instanced_vbo(6, numInstances);

         Dsdvg_attrib_disable(shape_a_vertex);
         Dsdvg_attrib_disable(shape_a_vertex_n);
         Dsdvg_attrib_disable(shape_a_pattern);

         Dsdvg_attrib_divisor_reset(shape_a_vertex);
         Dsdvg_attrib_divisor_reset(shape_a_vertex_n);
         Dsdvg_attrib_divisor_reset(shape_a_pattern);
      }
   }

};
