// ----
// ---- file   : RectFillStrokeAA.h
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

class RectFillStrokeAA : public ShaderVG_Shape {

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
      "uniform vec2  u_size_i; \n"
      "uniform vec2  u_size_o; \n"
      "uniform float u_aa_range; \n"
      "uniform float u_aa_exp; \n"
      "uniform vec4  u_color_fill; \n"
      "uniform vec4  u_color_stroke; \n"
      "uniform float u_debug; \n"
      " \n"
      "VARYING_IN vec2 v_p; \n"
      " \n"
      "void main(void) { \n"
      "  float aRectI = 0.0; \n"
      "  float aRectO = 0.0; \n"
      "  vec4 colorI = vec4(0,0,0,0); \n"
      "  vec4 colorO = vec4(0,0,0,0); \n"
      " \n"
      "  // outer \n"
      "  vec2 vd = abs(v_p - u_center); \n"
      "  if(vd.x < u_size_o.x && vd.y < u_size_o.y) \n"
      "  { \n"
      "    aRectO  = 1.0 - smoothstep(u_size_o.x-u_aa_range, u_size_o.x, vd.x); \n"
      "    aRectO *= 1.0 - smoothstep(u_size_o.y-u_aa_range, u_size_o.y, vd.y); \n"
      "    colorO = u_color_stroke; \n"
      "  } \n"
      " \n"
      "  // inner \n"
      "  vd = abs(v_p - u_center); \n"
      "  if(vd.x < u_size_i.x && vd.y < u_size_i.y) \n"
      "  { \n"
      "    aRectI  = 1.0 - smoothstep(u_size_i.x - u_aa_range, u_size_i.x, vd.x); \n"
      "    aRectI *= 1.0 - smoothstep(u_size_i.y - u_aa_range, u_size_i.y, vd.y); \n"
      "    colorI = u_color_fill; \n"
      "  } \n"
      " \n"
      "  float aI = aRectI; \n"
      "  float aO = aRectO; \n"
      " \n"
      "#if 1 \n"
      "  aI = pow(aI, u_aa_exp); \n"
      "#endif \n"
      "  vec4 color = mix(colorO, colorI, aI); \n"
      " \n"
      "  // a = smoothstep(0.0, 1.0, a); \n"
      "#if 1 \n"
      "  aO = pow(aO, u_aa_exp); \n"
      "#endif \n"
      " \n"
      "  FRAGCOLOR = vec4(color.xyz, color.a*aO); \n"
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

   void setupRectFillStrokeAAVBO32(Dsdvg_buffer_ref_t _vb, Dsdvg_buffer_ref_t _dl,
                                   sF32 _centerX, sF32 _centerY,
                                   sF32 _sizeX, sF32 _sizeY,
                                   sF32 _strokeW,
                                   sF32 _aaRange
                                   ) {
      //
      //  +0  u16 aaRange * 256
      //  +2  i32 vbOffInner
      //  +6  u16 numVertsInner  (0 or 6. GL_TRIANGLES)
      //  +8  i32 vbOffBorder
      //  +12 u16 numVertsBorder
      //  +14 u16 primTypeBorder
      //

      Dstream_write_i16(_dl, sU16(_aaRange * 256));

      sBool bSingle = ((_sizeX*_sizeY) <= RECT_SINGLE_AREA_THRESHOLD);
      sUI numVerts;

      // Inner
      sBool bInner = !bSingle && b_draw_inner;
      if(bInner)
      {
         // (note) GL_TRIANGLES
         numVerts = 6u;

         Dstream_write_i32(_dl, Dstream_get_offset(_vb));
         Dstream_write_i16(_dl, numVerts);

         // left/top
         Dstream_write_f32(_vb, _centerX - _sizeX + _strokeW + _aaRange);
         Dstream_write_f32(_vb, _centerY - _sizeY + _strokeW + _aaRange);

         Dstream_write_f32(_vb, _centerX + _sizeX - _strokeW - _aaRange);
         Dstream_write_f32(_vb, _centerY - _sizeY + _strokeW + _aaRange);

         Dstream_write_f32(_vb, _centerX - _sizeX + _strokeW + _aaRange);
         Dstream_write_f32(_vb, _centerY + _sizeY - _strokeW - _aaRange);

         // right/bottom
         Dstream_write_f32(_vb, _centerX - _sizeX + _strokeW + _aaRange);
         Dstream_write_f32(_vb, _centerY + _sizeY - _strokeW - _aaRange);

         Dstream_write_f32(_vb, _centerX + _sizeX - _strokeW - _aaRange);
         Dstream_write_f32(_vb, _centerY - _sizeY + _strokeW + _aaRange);

         Dstream_write_f32(_vb, _centerX + _sizeX - _strokeW - _aaRange);
         Dstream_write_f32(_vb, _centerY + _sizeY - _strokeW - _aaRange);
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
                          _centerX - _sizeX - _strokeW,
                          _centerY - _sizeY - _strokeW,
                          (_sizeX + _strokeW) * 2.0f,
                          (_sizeY + _strokeW) * 2.0f
                          );
      }
      else if(b_draw_border)
      {
         numVerts = 8u * 3u;

         Dstream_write_i32(_dl, Dstream_get_offset(_vb));
         Dstream_write_i16(_dl, numVerts);
         Dstream_write_i16(_dl, GL_TRIANGLES/*0x0004*/);

         EmitRectBorderVertices(_vb,
                                _centerX, _centerY,
                                _sizeX, _sizeY,
                                _strokeW,
                                _aaRange
                                );
      }
      else
      {
         Dstream_write_i32(_dl, Dstream_get_offset(_vb));
         Dstream_write_i16(_dl, 0u/*numVerts*/);
         Dstream_write_i16(_dl, GL_NONE/*0x0000*/);
      }
   }

   void drawRectFillStrokeAAVBO32(sUI              _vboId,
                                  sUI              _byteOffsetInner,
                                  sUI              _numVertsInner,
                                  sUI              _byteOffsetBorder,
                                  sUI              _numVertsBorder,
                                  sUI              _glPrimTypeBorder,
                                  Dsdvg_mat4_ref_t _mvpMatrix,
                                  sF32             _centerX, sF32 _centerY,
                                  sF32             _sizeX,   sF32 _sizeY,
                                  sF32             _fillR,   sF32 _fillG,   sF32 _fillB,   sF32 _fillA,
                                  sF32             _strokeR, sF32 _strokeG, sF32 _strokeB, sF32 _strokeA,
                                  sF32             _strokeW,
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
         Dsdvg_uniform_2f(shape_u_center,   _centerX, _centerY);
         Dsdvg_uniform_2f(shape_u_size_i,   _sizeX - _strokeW, _sizeY - _strokeW);
         Dsdvg_uniform_2f(shape_u_size_o,   _sizeX + _strokeW, _sizeY + _strokeW);
         Dsdvg_uniform_1f(shape_u_aa_range, _aaRange);

         if(-1 != shape_u_aa_exp)
         {
            Dsdvg_uniform_1f(shape_u_aa_exp, _aaExp);
         }

         Dsdvg_uniform_4f(shape_u_color_fill,   _fillR,   _fillG,   _fillB,   _fillA);
         Dsdvg_uniform_4f(shape_u_color_stroke, _strokeR, _strokeG, _strokeB, _strokeA);

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

   void drawRectFillStrokeAA(Dsdvg_buffer_ref_t _scratchBuf,
                             Dsdvg_mat4_ref_t _mvpMatrix,
                             sF32 _centerX, sF32 _centerY,
                             sF32 _sizeX,   sF32 _sizeY,
                             sF32 _fillR,   sF32 _fillG,   sF32 _fillB,   sF32 _fillA,
                             sF32 _strokeR, sF32 _strokeG, sF32 _strokeB, sF32 _strokeA,
                             sF32 _strokeW,
                             sF32 _aaRange,
                             sF32 _aaExp
                             ) {

      BindScratchBuffer();

      sBool bSingle = ((_sizeX*_sizeY) <= RECT_SINGLE_AREA_THRESHOLD);

      // Inner
      sBool bInner = !bSingle && b_draw_inner;
      if(bInner)
      {
         sSI aVertexFill = BindFillShader();

         sUI numTris = 2u;
         allocScratchBuffer(aVertexFill, _scratchBuf, (numTris*3*2/*xy*/*4/*float*/));

         // left/top
         Dstream_write_f32(_scratchBuf, _centerX - _sizeX + _strokeW + _aaRange);
         Dstream_write_f32(_scratchBuf, _centerY - _sizeY + _strokeW + _aaRange);

         Dstream_write_f32(_scratchBuf, _centerX + _sizeX - _strokeW - _aaRange);
         Dstream_write_f32(_scratchBuf, _centerY - _sizeY + _strokeW + _aaRange);

         Dstream_write_f32(_scratchBuf, _centerX - _sizeX + _strokeW + _aaRange);
         Dstream_write_f32(_scratchBuf, _centerY + _sizeY - _strokeW - _aaRange);

         // right/bottom
         Dstream_write_f32(_scratchBuf, _centerX - _sizeX + _strokeW + _aaRange);
         Dstream_write_f32(_scratchBuf, _centerY + _sizeY - _strokeW - _aaRange);

         Dstream_write_f32(_scratchBuf, _centerX + _sizeX - _strokeW - _aaRange);
         Dstream_write_f32(_scratchBuf, _centerY - _sizeY + _strokeW + _aaRange);

         Dstream_write_f32(_scratchBuf, _centerX + _sizeX - _strokeW - _aaRange);
         Dstream_write_f32(_scratchBuf, _centerY + _sizeY - _strokeW - _aaRange);

         Dsdvg_draw_triangles(0, numTris*3);

         EndFillShader();
      }

      // Outer corners
      shape_shader.bind();

      Dsdvg_uniform_mat4(shape_u_transform, _mvpMatrix);
      Dsdvg_uniform_2f(shape_u_center,   _centerX, _centerY);
      Dsdvg_uniform_2f(shape_u_size_i,   _sizeX - _strokeW, _sizeY - _strokeW);
      Dsdvg_uniform_2f(shape_u_size_o,   _sizeX + _strokeW, _sizeY + _strokeW);
      Dsdvg_uniform_1f(shape_u_aa_range, _aaRange);

      if(-1 != shape_u_aa_exp)
      {
         Dsdvg_uniform_1f(shape_u_aa_exp, _aaExp);
      }

      Dsdvg_uniform_4f(shape_u_color_fill,   _fillR,   _fillG,   _fillB,   _fillA);
      Dsdvg_uniform_4f(shape_u_color_stroke, _strokeR, _strokeG, _strokeB, _strokeA);

      if(-1 != shape_u_debug)
      {
         Dsdvg_uniform_1f(shape_u_debug, b_debug ? 1.0f : 0.0f);
      }

      Dsdvg_attrib_enable(shape_a_vertex);

      if(bSingle)
      {
         setQuadVertices(shape_a_vertex, _scratchBuf,
                         _centerX - _sizeX - _strokeW,
                         _centerY - _sizeY - _strokeW,
                         (_sizeX + _strokeW) * 2.0f,
                         (_sizeY + _strokeW) * 2.0f
                         );

         Dsdvg_draw_triangle_fan(0, 4);
      }
      else if(b_draw_border)
      {
         allocScratchBuffer(shape_a_vertex, _scratchBuf, (8*3*2/*xy*/*4/*float*/));

         EmitRectBorderVertices(_scratchBuf,
                                _centerX, _centerY,
                                _sizeX, _sizeY,
                                _strokeW,
                                _aaRange
                                );

         Dsdvg_draw_triangles(0, 8*3);
      }

      Dsdvg_attrib_disable(shape_a_vertex);
   }

};
