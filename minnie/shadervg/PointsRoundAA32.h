// ----
// ---- file   : PointsRoundAA32.h
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

class PointsRoundAA32 : public ShaderVG_Shape {

  public:
   // ------------ vertex shader --------------
   const char *vs_src =
      "uniform mat4  u_transform; \n"
      "uniform float u_point_radius; \n"
      " \n"
      "ATTRIBUTE vec2  a_vertex; \n"
#ifndef USE_VERTEX_ATTRIB_DIVISOR
      "ATTRIBUTE float a_index; \n"
#endif // USE_VERTEX_ATTRIB_DIVISOR
      " \n"
      "VARYING_OUT vec2 v_vertex_mp; \n"
      " \n"
      "void main(void) { \n"
      "  vec2 vCtr = a_vertex; \n"
      "  vec2 v; \n"
      " \n"
#ifdef USE_VERTEX_ATTRIB_DIVISOR
      "  float index = float(gl_VertexID % 6); \n"
#else
      "  float index = a_index; \n"
#endif // USE_VERTEX_ATTRIB_DIVISOR
      " \n"
      "  if(index > 5.9) { \n"
      "    v = vec2(0,0); \n"
      "  } \n"
      "  else if(index > 4.9) { \n"
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
      " \n"
      "  gl_Position = u_transform * vec4(v,0,1); \n"
      "  v_vertex_mp = v - vCtr; \n"
      "} \n"
      ;

   // ------------ fragment shader ------------
   const char *fs_src =
      "uniform vec4  u_color_stroke; \n"
      "uniform float u_point_radius; \n"
      "uniform float u_aa_range; \n"
      "uniform float u_debug; \n"
      " \n"
      "VARYING_IN vec2 v_vertex_mp; \n"
      " \n"
      "void main(void) { \n"
      "  vec2 vd = abs(v_vertex_mp); \n"
      "  float d = length(vd); \n"
      // aa
      "  float a = 1.0 - smoothstep(u_point_radius - u_aa_range, u_point_radius, d); \n"
      "  FRAGCOLOR = vec4(u_color_stroke.rgb, u_color_stroke.a * a); \n"
      "  if(u_debug > 0.0) { \n"
      "    FRAGCOLOR = vec4(1.0, a, a, 1.0); \n"
      "  } \n"
      "} \n"
      ;

   sBool validateShapeShader(void) {
      return
            (-1 != shape_a_vertex)
#ifndef USE_VERTEX_ATTRIB_DIVISOR
         && (-1 != shape_a_index)
#endif // USE_VERTEX_ATTRIB_DIVISOR
         && (-1 != shape_u_transform)
         && (-1 != shape_u_color_stroke)
         && (-1 != shape_u_point_radius)
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

   void drawPointsRoundAAVBO32(sUI              _vboId,
                               sUI              _byteOffset,
                               sUI              _numPoints,
                               Dsdvg_mat4_ref_t _mvpMatrix,
                               sF32             _strokeR, sF32 _strokeG, sF32 _strokeB, sF32 _strokeA,
                               sF32             _pointRadius,
                               sF32             _aaRange
                               ) {
      //
      // VBO vertex format (10 bytes per vertex w/o attrib divisor, else 8):
      //   +0 f32 x
      //   +4 f32 y
      //   +8 i16   index (0..5)
      //
      // (note) duplicate vertices * 6 when !defined(USE_VERTEX_ATTRIB_DIVISOR)
      // (note) numVerts          = (numPoints * 6)
      // (note) numTri            = numPoints * 4
      // (note) numBytesPerPoint  = 6*10 = 60
      //

      /* Dprintf("xxx DrawPointsRoundAAVBO32: vboId=%u byteOffset=%u numPoints=%u pointRadius=%f aaRange=%f\n", _vboId, _byteOffset, _numPoints, _pointRadius, _aaRange); */

      sdvg_BindVBO(_vboId);

      shape_shader.bind();

      Dsdvg_uniform_mat4(shape_u_transform, _mvpMatrix);
      Dsdvg_uniform_4f(shape_u_color_stroke, _strokeR, _strokeG, _strokeB, _strokeA);
      Dsdvg_uniform_1f(shape_u_point_radius, _pointRadius);
      Dsdvg_uniform_1f(shape_u_aa_range, _aaRange);
      if(-1 != shape_u_debug)
      {
         Dsdvg_uniform_1f(shape_u_debug, b_debug ? 1.0f : 0.0f);
      }

#ifdef USE_VERTEX_ATTRIB_DIVISOR
      Dsdvg_attrib_offset(shape_a_vertex, 2/*size*/, GL_FLOAT,          GL_FALSE/*normalize*/,  8/*stride*/, _byteOffset +  0);
#else
      Dsdvg_attrib_offset(shape_a_vertex, 2/*size*/, GL_FLOAT,          GL_FALSE/*normalize*/, 10/*stride*/, _byteOffset +  0);
      Dsdvg_attrib_offset(shape_a_index,  1/*size*/, GL_UNSIGNED_SHORT, GL_FALSE/*normalize*/, 10/*stride*/, _byteOffset +  8);
#endif // USE_VERTEX_ATTRIB_DIVISOR

      Dsdvg_attrib_enable(shape_a_vertex);
#ifdef USE_VERTEX_ATTRIB_DIVISOR
      Dsdvg_attrib_divisor(shape_a_vertex, 1);
#else
      Dsdvg_attrib_enable(shape_a_index);
#endif // USE_VERTEX_ATTRIB_DIVISOR

#ifdef USE_VERTEX_ATTRIB_DIVISOR
      const sUI numInstances = _numPoints;
      Dsdvg_draw_triangles_instanced_vbo(6, numInstances);
#else
      const sUI numVerts = _numPoints * 6u;
      Dsdvg_draw_triangles_vbo(0, numVerts);
#endif // USE_VERTEX_ATTRIB_DIVISOR

      Dsdvg_attrib_disable(shape_a_vertex);
#ifdef USE_VERTEX_ATTRIB_DIVISOR
      Dsdvg_attrib_divisor_reset(shape_a_vertex);
#else
      Dsdvg_attrib_disable(shape_a_index);
#endif // USE_VERTEX_ATTRIB_DIVISOR
   }

};
