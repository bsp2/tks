// ----
// ---- file   : LineStripFlatAA14_2.h
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

class LineStripFlatAA14_2 : public ShaderVG_Shape {

  public:
   // ------------ vertex shader --------------
   const char *vs_src =
      "uniform mat4  u_transform; \n"
      "uniform float u_stroke_w; \n"
      " \n"
      "ATTRIBUTE vec2  a_vertex; \n"
      "ATTRIBUTE vec2  a_vertex_n; \n"
#ifndef USE_VERTEX_ATTRIB_DIVISOR
      "ATTRIBUTE float a_index; \n"
#endif // USE_VERTEX_ATTRIB_DIVISOR
      " \n"
      "VARYING_OUT vec2 v_vertex_mp; \n"
      "VARYING_OUT vec2 v_plane_n; \n"
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
      " \n"
#ifdef USE_VERTEX_ATTRIB_DIVISOR
      "  float index = float(gl_VertexID % 6); \n"
#else
      "  float index = a_index; \n"
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
      "} \n"
      ;

   // ------------ fragment shader ------------
   const char *fs_src =
      "uniform vec4  u_color_stroke; \n"
      "uniform float u_stroke_w; \n"
      "uniform float u_aa_range; \n"
      "uniform float u_debug; \n"
      " \n"
      "VARYING_IN vec2 v_vertex_mp; \n"
      "VARYING_IN vec2 v_plane_n; \n"
      " \n"
      "void main(void) { \n"
      "  float d = abs(dot(v_vertex_mp, v_plane_n)); \n"
      "  float a = 1.0 - smoothstep(u_stroke_w - u_aa_range, u_stroke_w, d); \n"
      "  FRAGCOLOR = vec4(u_color_stroke.rgb, u_color_stroke.a * a); \n"
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
         && (-1 != shape_a_index)
#endif // USE_VERTEX_ATTRIB_DIVISOR
         && (-1 != shape_u_transform)
         && (-1 != shape_u_color_stroke)
         && (-1 != shape_u_stroke_w)
         && (-1 != shape_u_aa_range)
         ;
   }

   sBool onOpen(void) {
      if(createShapeShader(vs_src, fs_src))
      {
         return YAC_TRUE;
      }
      return YAC_FALSE;
   }

   void drawLineStripFlatAAVBO14_2(sUI              _vboId,
                                   sUI              _byteOffset,
                                   sUI              _numPoints,
                                   Dsdvg_mat4_ref_t _mvpMatrix,
                                   sF32             _strokeR, sF32 _strokeG, sF32 _strokeB, sF32 _strokeA,
                                   sF32             _strokeW,
                                   sF32             _aaRange
                                   ) {
      //
      // VBO vertex format (6 bytes per vertex w/o attrib divisor, else 4):
      //   +0 s14.2 x
      //   +2 s14.2 y
      //   +4 i16   index
      //
      // (note) duplicate vertices * 6 when !defined(USE_VERTEX_ATTRIB_DIVISOR)
      // (note) numVerts         = (numPoints * 6)
      // (note) numSeg           = (numPoints - 1)
      // (note) numTri           = (numPoints - 1) * 6
      // (note) numBytesPerPoint = 6*6 = 36
      //

      /* Dprintf("xxx drawLineStripFlatAAVBO14_2\n"); */

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

#ifdef USE_VERTEX_ATTRIB_DIVISOR
      Dsdvg_attrib_offset(shape_a_vertex,   2/*size*/, GL_SHORT, GL_FALSE/*normalize*/, 4/*stride*/, _byteOffset +  0);
      Dsdvg_attrib_offset(shape_a_vertex_n, 2/*size*/, GL_SHORT, GL_FALSE/*normalize*/, 4/*stride*/, _byteOffset +  4);
#else
      Dsdvg_attrib_offset(shape_a_vertex,   2/*size*/, GL_SHORT, GL_FALSE/*normalize*/, 6/*stride*/, _byteOffset +  0);
      Dsdvg_attrib_offset(shape_a_vertex_n, 2/*size*/, GL_SHORT, GL_FALSE/*normalize*/, 6/*stride*/, _byteOffset + 36);
#endif // USE_VERTEX_ATTRIB_DIVISOR

      Dsdvg_attrib_enable(shape_a_vertex);
      Dsdvg_attrib_enable(shape_a_vertex_n);
#ifdef USE_VERTEX_ATTRIB_DIVISOR
      Dsdvg_attrib_divisor(shape_a_vertex, 1);
      Dsdvg_attrib_divisor(shape_a_vertex_n, 1);
#else
      Dsdvg_attrib_enable(shape_a_index);
#endif // USE_VERTEX_ATTRIB_DIVISOR

#ifdef USE_VERTEX_ATTRIB_DIVISOR
      const sUI numInstances = (_numPoints - 1);
      Dsdvg_draw_triangles_instanced_vbo(6, numInstances);
#else
      const sUI numVerts = (_numPoints - 1) * 6;
      Dsdvg_draw_triangles_vbo(0, numVerts);
#endif // USE_VERTEX_ATTRIB_DIVISOR

      Dsdvg_attrib_disable(shape_a_vertex_n);
      Dsdvg_attrib_disable(shape_a_vertex);
#ifdef USE_VERTEX_ATTRIB_DIVISOR
      Dsdvg_attrib_divisor_reset(shape_a_vertex);
      Dsdvg_attrib_divisor_reset(shape_a_vertex_n);
#else
      Dsdvg_attrib_disable(shape_a_index);
#endif // USE_VERTEX_ATTRIB_DIVISOR
   }

};
