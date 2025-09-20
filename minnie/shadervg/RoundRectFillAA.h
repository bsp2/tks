// ----
// ---- file   : RoundRectFillAA.h
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

class RoundRectFillAA : public ShaderVG_Shape {

  public:
   // ------------ vertex shader --------------
   const char *vs_src =
      "uniform mat4 u_transform; \n"
      " \n"
      "ATTRIBUTE vec2 a_vertex; \n"
      " \n"
      "VARYING_OUT vec2 v_p; \n"
      " \n"
      "void main(void) { \n"
      "  v_p = a_vertex; \n"
      "  gl_Position = u_transform * vec4(a_vertex,0,1); \n"
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
      "uniform float u_debug; \n"
      " \n"
      "VARYING_IN vec2 v_p; \n"
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
      "  FRAGCOLOR = vec4(color.xyz, color.a*a); \n"
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

   void setupRoundRectFillAAVBO32(Dsdvg_buffer_ref_t _vb, Dsdvg_buffer_ref_t _dl,
                                  sF32 _centerX, sF32 _centerY,
                                  sF32 _sizeX,   sF32 _sizeY,
                                  sF32 _radiusX, sF32 _radiusY,
                                  sF32 _aaRange
                                  ) {
      //
      //  +0  u16 aaRange * 256
      //  +2  i32 vbOffInner
      //  +6  u16 numVertsInner  (GL_TRIANGLES)
      //  +8  i32 vbOffBorder
      //  +12 u16 numVertsBorder
      //  +14 u16 primTypeBorder (GL_TRIANGLE_FAN(0x0006) or GL_TRIANGLES(0x0004))
      //

      Dstream_write_i16(_dl, sU16(_aaRange * 256));

      if(_radiusX > _sizeX)
         _radiusX = _sizeX;

      if(_radiusY > _sizeY)
         _radiusY = _sizeY;

      sUI numVerts;
      sUI numTris;

      sBool bSingle = ((_sizeX*_sizeY) <= ROUNDRECT_SINGLE_AREA_THRESHOLD);

      // Inner
      sBool bInner = !bSingle && b_draw_inner;
      if(bInner)
      {
         numTris = 14u;

         numTris = EmitRoundRectInnerVertices(_vb,
                                              _centerX, _centerY,
                                              _sizeX,   _sizeY,
                                              _radiusX, _radiusY,
                                              0.0f/*strokeW*/,
                                              _aaRange
                                              );

         numVerts = numTris * 3u;

         Dstream_write_i32(_dl, Dstream_get_offset(_vb));
         Dstream_write_i16(_dl, numVerts);
      }
      else
      {
         Dstream_write_i32(_dl, Dstream_get_offset(_vb));
         Dstream_write_i16(_dl, 0u/*numVerts*/);
      }

      if(bSingle)
      {
         numVerts = 4u;

         Dstream_write_i32(_dl, Dstream_get_offset(_vb));
         Dstream_write_i16(_dl, numVerts);
         Dstream_write_i16(_dl, GL_TRIANGLE_FAN/*0x0006*/);

         EmitQuadVertices(_vb,
                          _centerX - _sizeX,
                          _centerY - _sizeY,
                          _sizeX * 2.0f,
                          _sizeY * 2.0f
                          );
      }
      else if(b_draw_border)
      {
         // (note) 24..28 tris

         numTris = EmitRoundRectBorderVertices(_vb,
                                               _centerX, _centerY,
                                               _sizeX,   _sizeY,
                                               _radiusX, _radiusY,
                                               0.0f/*strokeW*/,
                                               _aaRange
                                               );
         numVerts = numTris * 3u;

         Dstream_write_i32(_dl, Dstream_get_offset(_vb));
         Dstream_write_i16(_dl, numVerts);
         Dstream_write_i16(_dl, GL_TRIANGLES/*0x0004*/);
      }
      else
      {
         Dstream_write_i32(_dl, Dstream_get_offset(_vb));
         Dstream_write_i16(_dl, 0u/*numVerts*/);
         Dstream_write_i16(_dl, GL_TRIANGLE_FAN/*0x0006*/);
      }
   }

   void drawRoundRectFillAAVBO32(sUI              _vboId,
                                 sUI              _byteOffsetInner,
                                 sUI              _numVertsInner,
                                 sUI              _byteOffsetBorder,
                                 sUI              _numVertsBorder,
                                 sUI              _glPrimTypeBorder,
                                 Dsdvg_mat4_ref_t _mvpMatrix,
                                 sF32             _centerX, sF32 _centerY,
                                 sF32             _sizeX,   sF32 _sizeY,
                                 sF32             _radiusX, sF32 _radiusY,
                                 sF32             _fillR,   sF32 _fillG,   sF32 _fillB,   sF32 _fillA,
                                 sF32             _aaRange,
                                 sF32             _aaExp
                                 ) {

      sdvg_BindVBO(_vboId);

      if(_numVertsInner > 0u)
      {
         sSI aVertexFill = BindFillShader();

         Dsdvg_attrib_offset(aVertexFill, 2/*size*/, GL_FLOAT, GL_FALSE/*normalize*/, 0/*stride*/, _byteOffsetInner);
         Dsdvg_draw_triangles_vbo(0, _numVertsInner);

         EndFillShader();
      }

      // Outer border
      if(_numVertsBorder > 0u)
      {
         shape_shader.bind();

         Dsdvg_uniform_mat4(shape_u_transform, _mvpMatrix);
         Dsdvg_uniform_2f(shape_u_center,        _centerX, _centerY);
         Dsdvg_uniform_2f(shape_u_size,          _sizeX, _sizeY);
         Dsdvg_uniform_2f(shape_u_radius,        _radiusX, _radiusY);
         Dsdvg_uniform_2f(shape_u_ob_radius,     1.0f / _radiusX, 1.0f / _radiusY);
         Dsdvg_uniform_1f(shape_u_ob_radius_max, (_radiusX > _radiusY) ? (1.0f / _radiusX) : (1.0f / _radiusY));
         Dsdvg_uniform_1f(shape_u_radius_max,    (_radiusX > _radiusY) ? _radiusX : _radiusY);
         Dsdvg_uniform_1f(shape_u_aa_range,      _aaRange);

         if(-1 != shape_u_aa_exp)
         {
            Dsdvg_uniform_1f(shape_u_aa_exp, _aaExp);
         }

         Dsdvg_uniform_4f(shape_u_color_fill, _fillR, _fillG, _fillB, _fillA);

         if(-1 != shape_u_debug)
         {
            Dsdvg_uniform_1f(shape_u_debug, b_debug ? 1.0f : 0.0f);
         }

         Dsdvg_attrib_offset(shape_a_vertex, 2/*size*/, GL_FLOAT, GL_FALSE/*normalize*/, 0/*stride*/, _byteOffsetBorder);
         Dsdvg_attrib_enable(shape_a_vertex);

         Dsdvg_draw_arrays_vbo(_glPrimTypeBorder, 0, _numVertsBorder);

         Dsdvg_attrib_disable(shape_a_vertex);
      }
   }

   void drawRoundRectFillAA(Dsdvg_buffer_ref_t _scratchBuf,
                            Dsdvg_mat4_ref_t _mvpMatrix,
                            sF32 _centerX, sF32 _centerY,
                            sF32 _sizeX,   sF32 _sizeY,
                            sF32 _radiusX, sF32 _radiusY,
                            sF32 _fillR,   sF32 _fillG,   sF32 _fillB,   sF32 _fillA,
                            sF32 _aaRange,
                            sF32 _aaExp
                            ) {

      BindScratchBuffer();

      if(_radiusX > _sizeX)
         _radiusX = _sizeX;

      if(_radiusY > _sizeY)
         _radiusY = _sizeY;

      sUI numTris;

      sBool bSingle = ((_sizeX*_sizeY) <= ROUNDRECT_SINGLE_AREA_THRESHOLD);

      // Inner
      sBool bInner = !bSingle && b_draw_inner;
      if(bInner)
      {
         sSI aVertexFill = BindFillShader();

         numTris = 14u;
         allocScratchBuffer(aVertexFill, _scratchBuf, (numTris*3*2/*xy*/*4/*float*/));

         numTris = EmitRoundRectInnerVertices(_scratchBuf,
                                              _centerX, _centerY,
                                              _sizeX,   _sizeY,
                                              _radiusX, _radiusY,
                                              0.0f/*strokeW*/,
                                              _aaRange
                                              );

         Dsdvg_draw_triangles(0, numTris*3);

         EndFillShader();
      }

      // Outer corners
      shape_shader.bind();

      Dsdvg_uniform_mat4(shape_u_transform, _mvpMatrix);
      Dsdvg_uniform_2f(shape_u_center,        _centerX, _centerY);
      Dsdvg_uniform_2f(shape_u_size,          _sizeX, _sizeY);
      Dsdvg_uniform_2f(shape_u_radius,        _radiusX, _radiusY);
      Dsdvg_uniform_2f(shape_u_ob_radius,     1.0f / _radiusX, 1.0f / _radiusY);
      Dsdvg_uniform_1f(shape_u_ob_radius_max, (_radiusX > _radiusY) ? (1.0f / _radiusX) : (1.0f / _radiusY));
      Dsdvg_uniform_1f(shape_u_radius_max,    (_radiusX > _radiusY) ? _radiusX : _radiusY);
      Dsdvg_uniform_1f(shape_u_aa_range,      _aaRange);

      if(-1 != shape_u_aa_exp)
      {
         Dsdvg_uniform_1f(shape_u_aa_exp, _aaExp);
      }

      Dsdvg_uniform_4f(shape_u_color_fill, _fillR, _fillG, _fillB, _fillA);

      if(-1 != shape_u_debug)
      {
         Dsdvg_uniform_1f(shape_u_debug, b_debug ? 1.0f : 0.0f);
      }

      Dsdvg_attrib_enable(shape_a_vertex);

      if(bSingle)
      {
         setQuadVertices(shape_a_vertex, _scratchBuf,
                         _centerX - _sizeX,
                         _centerY - _sizeY,
                         _sizeX * 2.0f,
                         _sizeY * 2.0f
                         );

         Dsdvg_draw_triangle_fan(0, 4);
      }
      else if(b_draw_border)
      {
         numTris = 28u;  // 24..28
         allocScratchBuffer(shape_a_vertex, _scratchBuf, (numTris*3*2/*xy*/*4/*float*/));

         numTris = EmitRoundRectBorderVertices(_scratchBuf,
                                               _centerX, _centerY,
                                               _sizeX,   _sizeY,
                                               _radiusX, _radiusY,
                                               0.0f/*strokeW*/,
                                               _aaRange
                                               );

         Dsdvg_draw_triangles(0, numTris * 3u);
      }

      Dsdvg_attrib_disable(shape_a_vertex);
   }

};
