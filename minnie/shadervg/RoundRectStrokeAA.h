// ----
// ---- file   : RoundRectStrokeAA.h
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : Distributed under terms of the MIT license (https://opensource.org/licenses/MIT)
// ----          Copyright 2014-2026 by bsp
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

class RoundRectStrokeAA : public ShaderVG_Shape {

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
      " \n"
      "VARYING_IN vec2 v_p; \n"
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
      "  OUT_FRAGCOLOR = vec4(color.xyz, color.a*aO); \n"
#ifdef SHADERVG_DEBUG_FRAG
      "  if(u_debug > 0.0) \n"
      "    OUT_FRAGCOLOR = vec4(1,0,0,1); \n"
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

   void setupRoundRectStrokeAAVBO32(Dsdvg_buffer_ref_t _vb, Dsdvg_buffer_ref_t _dl,
                                    sF32 _centerX, sF32 _centerY,
                                    sF32 _sizeX,   sF32 _sizeY,
                                    sF32 _radiusX, sF32 _radiusY,
                                    sF32 _strokeW,
                                    sF32 _aaRange
                                    ) {
      //
      //  +0  u16 aaRange * 256
      //  +2  i32 vbOffBorder
      //  +6  u16 numVertsBorder
      //  +8  u16 primTypeBorder (GL_TRIANGLE_FAN(0x0006) or GL_TRIANGLES(0x0004))
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
                                              _strokeW,
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
                          _centerX - _sizeX - _strokeW,
                          _centerY - _sizeY - _strokeW,
                          (_sizeX + _strokeW) * 2.0f,
                          (_sizeY + _strokeW) * 2.0f
                          );
      }
      else if(b_draw_border)
      {
         // (note) 24..28 tris

         numTris = EmitRoundRectBorderVertices(_vb,
                                               _centerX, _centerY,
                                               _sizeX,   _sizeY,
                                               _radiusX, _radiusY,
                                               _strokeW,
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

   // see also: ShaderVG_Shape::drawRoundRectFillAAVBO32Paint()
};
