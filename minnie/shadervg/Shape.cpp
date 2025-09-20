// ----
// ---- file   : shadervg.cpp
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
// ---- note   :
// ----
// ----
// ----

#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#define YAC_BIGSTRING
#define YAC_PRINTF
#include <yac.h>

#define MINNIE_SKIP_TYPEDEFS  defined
#include "../minnie.h"

#include "../inc_opengl.h"

#include "shadervg.h"
#include "shadervg_internal.h"
#include "Shader.h"
#include "Shape.h"


ShaderVG_Shape::ShaderVG_Shape(void) {

   b_builtin = YAC_TRUE;

   shape_a_vertex     = -1;
   shape_a_vertex_n   = -1;
   shape_a_vertex_nn  = -1;
   shape_a_index      = -1;
   shape_a_color      = -1;
   shape_a_edge       = -1;
   shape_a_scale      = -1;
   shape_a_bc         = -1;
   shape_a_uv         = -1;

   shape_u_transform       = -1;
   shape_u_aa              = -1;
   shape_u_aa_range        = -1;
   shape_u_aa_exp          = -1;
   shape_u_center          = -1;
   shape_u_size            = -1;
   shape_u_size_i          = -1;
   shape_u_size_o          = -1;
   shape_u_radius          = -1;
   shape_u_radius_i        = -1;
   shape_u_radius_o        = -1;
   shape_u_ob_radius_i     = -1;
   shape_u_ob_radius_o     = -1;
   shape_u_ob_radius_i_max = -1;
   shape_u_ob_radius_o_max = -1;
   shape_u_radius_i_max    = -1;
   shape_u_radius_o_max    = -1;
   shape_u_ob_radius       = -1;
   shape_u_ob_radius_max   = -1;
   shape_u_radius_max      = -1;
   shape_u_point_radius    = -1;
   shape_u_color_fill      = -1;
   shape_u_color_stroke    = -1;
   shape_u_global_alpha    = -1;
   shape_u_decal_alpha     = -1;
   shape_u_sampler         = -1;
   shape_u_stroke_w        = -1;
   shape_u_debug           = -1;
   shape_u_tex_0           = -1;
   shape_u_tex_1           = -1;

   // debug:
   b_draw_inner  = YAC_TRUE;
   b_draw_border = YAC_TRUE;
   b_debug       = YAC_FALSE;
}

ShaderVG_Shape::~ShaderVG_Shape() {
}

void ShaderVG_Shape::allocScratchBuffer(sSI _aVertex, Dsdvg_buffer_ref_t _scratchBuf, sUI _numBytes) {
   AllocScratchBuffer(_aVertex, _scratchBuf, _numBytes);
}

void ShaderVG_Shape::EmitQuadVertices(Dsdvg_buffer_ref_t _vb,
                                      sF32 _x, sF32 _y, sF32 _w, sF32 _h
                                      ) {
   // (note) GL_TRIANGLE_FAN
   Dstream_write_2f(_vb, _x,      _y     );
   Dstream_write_2f(_vb, _x + _w, _y     );
   Dstream_write_2f(_vb, _x + _w, _y + _h);
   Dstream_write_2f(_vb, _x,      _y + _h);
}

void ShaderVG_Shape::setQuadVertices(sSI _aVertex,
                                     Dsdvg_buffer_ref_t _scratchBuf,
                                     sF32 _x, sF32 _y, sF32 _w, sF32 _h
                                     ) {
   allocScratchBuffer(_aVertex, _scratchBuf, (4*2/*xy*/*4/*float*/));
   Dstream_write_2f(_scratchBuf, _x,      _y     );
   Dstream_write_2f(_scratchBuf, _x + _w, _y     );
   Dstream_write_2f(_scratchBuf, _x + _w, _y + _h);
   Dstream_write_2f(_scratchBuf, _x,      _y + _h);
}

void ShaderVG_Shape::EmitRectBorderVertices(Dsdvg_buffer_ref_t _vb,
                                            sF32 _centerX, sF32 _centerY,
                                            sF32 _sizeX, sF32 _sizeY,
                                            sF32 _strokeW,
                                            sF32 _aaRange
                                            ) {
   // A_______________________B
   // | C___________________D |
   // | |                   | |
   // | |                   | |
   // | |                   | |
   // | E-------------------F |
   // G_______________________H

   const sF32 ax = _centerX - _sizeX - _strokeW;
   const sF32 ay = _centerY - _sizeY - _strokeW;

   const sF32 bx = _centerX + _sizeX + _strokeW;
   const sF32 by = ay;

   const sF32 cx = _centerX - _sizeX + _strokeW + _aaRange;
   const sF32 cy = _centerY - _sizeY + _strokeW + _aaRange;

   const sF32 dx = _centerX + _sizeX - _strokeW - _aaRange;
   const sF32 dy = cy;

   const sF32 ex = cx;
   const sF32 ey = _centerY + _sizeY - _strokeW - _aaRange;

   const sF32 fx = dx;
   const sF32 fy = ey;

   const sF32 gx = ax;
   const sF32 gy = _centerY + _sizeY + _strokeW;

   const sF32 hx = bx;
   const sF32 hy = gy;

   Dstream_write_2f(_vb, ax, ay);  Dstream_write_2f(_vb, bx, by);  Dstream_write_2f(_vb, cx, cy);
   Dstream_write_2f(_vb, cx, cy);  Dstream_write_2f(_vb, bx, by);  Dstream_write_2f(_vb, dx, dy);
   Dstream_write_2f(_vb, ax, ay);  Dstream_write_2f(_vb, cx, cy);  Dstream_write_2f(_vb, ex, ey);
   Dstream_write_2f(_vb, ax, ay);  Dstream_write_2f(_vb, ex, ey);  Dstream_write_2f(_vb, gx, gy);
   Dstream_write_2f(_vb, dx, dy);  Dstream_write_2f(_vb, bx, by);  Dstream_write_2f(_vb, fx, fy);
   Dstream_write_2f(_vb, fx, fy);  Dstream_write_2f(_vb, bx, by);  Dstream_write_2f(_vb, hx, hy);
   Dstream_write_2f(_vb, fx, fy);  Dstream_write_2f(_vb, hx, hy);  Dstream_write_2f(_vb, ex, ey);
   Dstream_write_2f(_vb, ex, ey);  Dstream_write_2f(_vb, hx, hy);  Dstream_write_2f(_vb, gx, gy);
}

void ShaderVG_Shape::EmitRectangleVertices(Dsdvg_buffer_ref_t _vb,
                                           sF32 _x, sF32 _y,
                                           sF32 _w, sF32 _h,
                                           sF32 _b
                                           ) {
   // A_______________________B
   // | C___________________D |
   // | |                   | |
   // | |                   | |
   // | |                   | |
   // | E-------------------F |
   // G_______________________H

   const sF32 ax = _x;
   const sF32 ay = _y;

   const sF32 bx = _x + _w;
   const sF32 by = ay;

   const sF32 cx = _x + _b;
   const sF32 cy = _y + _b;

   const sF32 dx = _x + _w - _b;
   const sF32 dy = cy;

   const sF32 ex = cx;
   const sF32 ey = _y + _h - _b;

   const sF32 fx = dx;
   const sF32 fy = ey;

   const sF32 gx = ax;
   const sF32 gy = _y + _h;

   const sF32 hx = bx;
   const sF32 hy = gy;

   Dstream_write_2f(_vb, ax, ay);  Dstream_write_2f(_vb, bx, by);  Dstream_write_2f(_vb, cx, cy);
   Dstream_write_2f(_vb, cx, cy);  Dstream_write_2f(_vb, bx, by);  Dstream_write_2f(_vb, dx, dy);
   Dstream_write_2f(_vb, ax, ay);  Dstream_write_2f(_vb, cx, cy);  Dstream_write_2f(_vb, ex, ey);
   Dstream_write_2f(_vb, ax, ay);  Dstream_write_2f(_vb, ex, ey);  Dstream_write_2f(_vb, gx, gy);
   Dstream_write_2f(_vb, dx, dy);  Dstream_write_2f(_vb, bx, by);  Dstream_write_2f(_vb, fx, fy);
   Dstream_write_2f(_vb, fx, fy);  Dstream_write_2f(_vb, bx, by);  Dstream_write_2f(_vb, hx, hy);
   Dstream_write_2f(_vb, fx, fy);  Dstream_write_2f(_vb, hx, hy);  Dstream_write_2f(_vb, ex, ey);
   Dstream_write_2f(_vb, ex, ey);  Dstream_write_2f(_vb, hx, hy);  Dstream_write_2f(_vb, gx, gy);
}

sUI ShaderVG_Shape::EmitRoundRectInnerVertices(Dsdvg_buffer_ref_t _vb,
                                               sF32 _centerX, sF32 _centerY,
                                               sF32 _sizeX,   sF32 _sizeY,
                                               sF32 _radiusX, sF32 _radiusY,
                                               sF32 _strokeW,
                                               sF32 _aaRange
                                               ) {
   sUI retNumTris = 8u;  // 8..14 tris

   //  A_______B________C______D
   //  | E_____F________G____H |
   //  | |    /|      / |\   | |
   //  | |   / |    /   | \  | |
   //  | |  /  |  /     |  \ | |
   //  | | /   |/       |   \| |
   // I1 J/----K--------L----M I2
   //  | |     |  <var> |    | |  center area is omitted when (_sizeY <= _radiusY)
   // N1 O-----P--------Q----R N2
   //  | |\    |      / |   /| |
   //  | | \   |    /   |  / | |
   //  | |  \  |  /     | /  | |
   //  | |   \ |/       |/   | |
   //  | S----\T--------U----V |
   //  W_______X________Y______Z
   //

   const sF32 ex = _centerX - _sizeX + _strokeW + _aaRange;
   const sF32 ey = _centerY - _sizeY + _strokeW + _aaRange;

   const sF32 fx = _centerX - _sizeX + _radiusX;
   const sF32 fy = ey;

   const sF32 gx = _centerX + _sizeX - _radiusX;
   const sF32 gy = ey;

   const sF32 hx = _centerX + _sizeX - _strokeW - _aaRange;
#if 0
   const sF32 hy = ey;
#endif

   const sF32 jx = ex;
   const sF32 jy = _centerY - _sizeY + _radiusY;

   const sF32 kx = fx;
   const sF32 ky = jy;

   const sF32 lx = gx;
   const sF32 ly = jy;

   const sF32 mx = hx;
   const sF32 my = jy;

   const sF32 ox = jx;
   const sF32 oy = _centerY + _sizeY - _radiusY;

   const sF32 px = kx;
   const sF32 py = oy;

   const sF32 qx = lx;
   const sF32 qy = oy;

   const sF32 rx = mx;
   const sF32 ry = oy;

#if 0
   const sF32 sx = ex;
#endif
   const sF32 sy = _centerY + _sizeY - _strokeW - _aaRange;

   const sF32 tx = px;
   const sF32 ty = sy;

   const sF32 ux = qx;
   const sF32 uy = sy;

#if 0
   const sF32 vx = rx;
   const sF32 vy = sy;
#endif

   // emit triangles
   Dstream_write_2f(_vb, jx, jy);  Dstream_write_2f(_vb, fx, fy);  Dstream_write_2f(_vb, kx, ky);
   Dstream_write_2f(_vb, kx, ky);  Dstream_write_2f(_vb, fx, fy);  Dstream_write_2f(_vb, gx, gy);
   Dstream_write_2f(_vb, gx, gy);  Dstream_write_2f(_vb, lx, ly);  Dstream_write_2f(_vb, kx, ky);
   Dstream_write_2f(_vb, gx, gy);  Dstream_write_2f(_vb, mx, my);  Dstream_write_2f(_vb, lx, ly);

   if(_sizeY > _radiusY)
   {
      // center area
      Dstream_write_2f(_vb, jx, jy);  Dstream_write_2f(_vb, kx, ky);  Dstream_write_2f(_vb, ox, oy);
      Dstream_write_2f(_vb, ox, oy);  Dstream_write_2f(_vb, kx, ky);  Dstream_write_2f(_vb, px, py);
      Dstream_write_2f(_vb, kx, ky);  Dstream_write_2f(_vb, lx, ly);  Dstream_write_2f(_vb, px, py);
      Dstream_write_2f(_vb, px, py);  Dstream_write_2f(_vb, lx, ly);  Dstream_write_2f(_vb, qx, qy);
      Dstream_write_2f(_vb, qx, qy);  Dstream_write_2f(_vb, lx, ly);  Dstream_write_2f(_vb, mx, my);
      Dstream_write_2f(_vb, mx, my);  Dstream_write_2f(_vb, rx, ry);  Dstream_write_2f(_vb, qx, qy);

      retNumTris += 6u;
   }

   Dstream_write_2f(_vb, ox, oy);  Dstream_write_2f(_vb, px, py);  Dstream_write_2f(_vb, tx, ty);
   Dstream_write_2f(_vb, tx, ty);  Dstream_write_2f(_vb, px, py);  Dstream_write_2f(_vb, qx, qy);
   Dstream_write_2f(_vb, qx, qy);  Dstream_write_2f(_vb, ux, uy);  Dstream_write_2f(_vb, tx, ty);
   Dstream_write_2f(_vb, qx, qy);  Dstream_write_2f(_vb, rx, ry);  Dstream_write_2f(_vb, ux, uy);

   return retNumTris;
}

sUI ShaderVG_Shape::EmitRoundRectBorderVertices(Dsdvg_buffer_ref_t _vb,
                                                sF32 _centerX, sF32 _centerY,
                                                sF32 _sizeX,   sF32 _sizeY,
                                                sF32 _radiusX, sF32 _radiusY,
                                                sF32 _strokeW,
                                                sF32 _aaRange
                                                ) {
   sUI retNumTris = 24u;  // 24..28 tris

   //  A_______B________C______D
   //  | E_____F________G____H |
   //  | |    /|      / |\   | |
   //  | |   / |    /   | \  | |
   //  | |  /  |  /     |  \ | |
   //  | | /   |/       |   \| |
   // I1 J/----K--------L----M I2
   //  | |     |  <var> |    | |  center area is omitted when (_sizeY <= _radiusY)
   // N1 O-----P--------Q----R N2
   //  | |\    |      / |   /| |
   //  | | \   |    /   |  / | |
   //  | |  \  |  /     | /  | |
   //  | |   \ |/       |/   | |
   //  | S----\T--------U----V |
   //  W_______X________Y______Z
   //

   const sF32 ax = _centerX - _sizeX - _strokeW;
   const sF32 ay = _centerY - _sizeY - _strokeW;

   const sF32 bx = _centerX - _sizeX + _radiusX;
   const sF32 by = ay;

   const sF32 cx = _centerX + _sizeX - _radiusX;
   const sF32 cy = ay;

   const sF32 dx = _centerX + _sizeX + _strokeW;
   const sF32 dy = ay;

   const sF32 ex = _centerX - _sizeX + _strokeW + _aaRange;
   const sF32 ey = _centerY - _sizeY + _strokeW + _aaRange;

   const sF32 fx = _centerX - _sizeX + _radiusX;
   const sF32 fy = ey;

   const sF32 gx = _centerX + _sizeX - _radiusX;
   const sF32 gy = ey;

   const sF32 hx = _centerX + _sizeX - _strokeW - _aaRange;
   const sF32 hy = ey;

   const sF32 i1x = ax;
   const sF32 i1y = _centerY - _sizeY + _radiusY;

   const sF32 jx = ex;
   const sF32 jy = i1y;

   const sF32 kx = fx;
#if 0
   const sF32 ky = jy;
#endif

   const sF32 lx = gx;
#if 0
   const sF32 ly = jy;
#endif

   const sF32 mx = hx;
   const sF32 my = jy;

   const sF32 i2x = dx;
   const sF32 i2y = my;

   const sF32 n1x = ax;
   const sF32 n1y = _centerY + _sizeY - _radiusY;

   const sF32 ox = jx;
   const sF32 oy = n1y;

   const sF32 px = kx;
#if 0
   const sF32 py = oy;
#endif

   const sF32 qx = lx;
#if 0
   const sF32 qy = oy;
#endif

   const sF32 rx = mx;
   const sF32 ry = oy;

   const sF32 n2x = i2x;
   const sF32 n2y = ry;

   const sF32 sx = ex;
   const sF32 sy = _centerY + _sizeY - _strokeW - _aaRange;

   const sF32 tx = px;
   const sF32 ty = sy;

   const sF32 ux = qx;
   const sF32 uy = sy;

   const sF32 vx = rx;
   const sF32 vy = sy;

   const sF32 wx = ax;
   const sF32 wy = _centerY + _sizeY + _strokeW;

   const sF32 xx = tx;
   const sF32 xy = wy;

   const sF32 yx = ux;
   const sF32 yy = wy;

   const sF32 zx = dx;
   const sF32 zy = wy;

   // top
   Dstream_write_2f(_vb, ax, ay);  Dstream_write_2f(_vb, bx, by);  Dstream_write_2f(_vb, ex, ey);
   Dstream_write_2f(_vb, ex, ey);  Dstream_write_2f(_vb, bx, by);  Dstream_write_2f(_vb, fx, fy);

   Dstream_write_2f(_vb, fx, fy);  Dstream_write_2f(_vb, bx, by);  Dstream_write_2f(_vb, cx, cy);
   Dstream_write_2f(_vb, cx, cy);  Dstream_write_2f(_vb, gx, gy);  Dstream_write_2f(_vb, fx, fy);

   Dstream_write_2f(_vb, cx, cy);  Dstream_write_2f(_vb, dx, dy);  Dstream_write_2f(_vb, gx, gy);
   Dstream_write_2f(_vb, gx, gy);  Dstream_write_2f(_vb, dx, dy);  Dstream_write_2f(_vb, hx, hy);

   // left top
   Dstream_write_2f(_vb, ax,  ay);  Dstream_write_2f(_vb, ex, ey);  Dstream_write_2f(_vb, i1x, i1y);
   Dstream_write_2f(_vb, i1x, i1y); Dstream_write_2f(_vb, ex, ey);  Dstream_write_2f(_vb, jx,  jy);

   Dstream_write_2f(_vb, hx,  hy);  Dstream_write_2f(_vb, dx, dy);  Dstream_write_2f(_vb, i2x, i2y);
   Dstream_write_2f(_vb, i2x, i2y); Dstream_write_2f(_vb, mx, my);  Dstream_write_2f(_vb, hx,  hy);

   if(_sizeY > _radiusY)
   {
      // center area borders
      Dstream_write_2f(_vb, i1x, i1y);  Dstream_write_2f(_vb, jx, jy);  Dstream_write_2f(_vb, n1x, n1y);
      Dstream_write_2f(_vb, n1x, n1y);  Dstream_write_2f(_vb, jx, jy);  Dstream_write_2f(_vb, ox,  oy);

      Dstream_write_2f(_vb, rx,  ry);   Dstream_write_2f(_vb, mx,  my);  Dstream_write_2f(_vb, i2x, i2y);
      Dstream_write_2f(_vb, i2x, i2y);  Dstream_write_2f(_vb, n2x, n2y); Dstream_write_2f(_vb, rx,  ry);

      retNumTris += 4u;
   }

   Dstream_write_2f(_vb, n1x, n1y);  Dstream_write_2f(_vb, ox, oy);  Dstream_write_2f(_vb, wx, wy);
   Dstream_write_2f(_vb, wx,  wy);   Dstream_write_2f(_vb, ox, oy);  Dstream_write_2f(_vb, sx, sy);

   Dstream_write_2f(_vb, wx,  wy);   Dstream_write_2f(_vb, sx, sy);  Dstream_write_2f(_vb, xx, xy);
   Dstream_write_2f(_vb, xx,  xy);   Dstream_write_2f(_vb, sx, sy);  Dstream_write_2f(_vb, tx, ty);

   Dstream_write_2f(_vb, tx,  ty);   Dstream_write_2f(_vb, ux, uy);  Dstream_write_2f(_vb, xx, xy);
   Dstream_write_2f(_vb, xx,  xy);   Dstream_write_2f(_vb, ux, uy);  Dstream_write_2f(_vb, yx, yy);

   Dstream_write_2f(_vb, ux,  uy);   Dstream_write_2f(_vb, vx, vy);  Dstream_write_2f(_vb, zx, zy);
   Dstream_write_2f(_vb, zx,  zy);   Dstream_write_2f(_vb, yx, yy);  Dstream_write_2f(_vb, ux, uy);

   Dstream_write_2f(_vb, vx,  vy);   Dstream_write_2f(_vb, rx, ry);  Dstream_write_2f(_vb, n2x, n2y);
   Dstream_write_2f(_vb, n2x, n2y);  Dstream_write_2f(_vb, zx, zy);  Dstream_write_2f(_vb, vx, vy);

   // corners / diagonals
   Dstream_write_2f(_vb, ex, ey);  Dstream_write_2f(_vb, fx, fy);  Dstream_write_2f(_vb, jx, jy);
   Dstream_write_2f(_vb, gx, gy);  Dstream_write_2f(_vb, hx, hy);  Dstream_write_2f(_vb, mx, my);
   Dstream_write_2f(_vb, ox, oy);  Dstream_write_2f(_vb, tx, ty);  Dstream_write_2f(_vb, sx, sy);
   Dstream_write_2f(_vb, rx, ry);  Dstream_write_2f(_vb, vx, vy);  Dstream_write_2f(_vb, ux, uy);

   return retNumTris;
}

sBool ShaderVG_Shape::validateShapeShader(void) {
   return
      (-1 != shape_a_vertex)       &&
      (-1 != shape_u_transform)    &&
      (-1 != shape_u_aa_range)     &&
      (-1 != shape_u_center)
      ;
}

sBool ShaderVG_Shape::createShapeShader(const char *_sVS, const char *_sFS) {

   Dsdvg_debugprintfvv("[trc] ShaderVG_Shape::createShapeShader: ENTER\n");

   if(!shape_shader.create(_sVS, _sFS))
   {
      Dsdvg_errorprintf("[---] ShaderVG_Shape::createShapeShader: failed to create shape_shader\n");
      return YAC_FALSE;
   }

   shape_a_vertex    = shape_shader.getAttribLocation("a_vertex");
   shape_a_vertex_n  = shape_shader.getAttribLocation("a_vertex_n");   // optional
   shape_a_vertex_nn = shape_shader.getAttribLocation("a_vertex_nn");  // optional
   shape_a_index     = shape_shader.getAttribLocation("a_index");      // optional
   shape_a_color     = shape_shader.getAttribLocation("a_color");      // optional
   shape_a_edge      = shape_shader.getAttribLocation("a_edge");       // optional
   shape_a_scale     = shape_shader.getAttribLocation("a_scale");      // optional
   shape_a_bc        = shape_shader.getAttribLocation("a_bc");         // optional
   shape_a_uv        = shape_shader.getAttribLocation("a_uv");         // optional

   shape_u_transform       = shape_shader.getUniformLocation("u_transform");
   shape_u_aa              = shape_shader.getUniformLocation("u_aa");               // optional
   shape_u_aa_range        = shape_shader.getUniformLocation("u_aa_range");         // optional for non-AA shader
   shape_u_aa_exp          = shape_shader.getUniformLocation("u_aa_exp");           // optional
   shape_u_center          = shape_shader.getUniformLocation("u_center");           // optional for non-AA shader
   shape_u_size            = shape_shader.getUniformLocation("u_size");             // optional
   shape_u_size_i          = shape_shader.getUniformLocation("u_size_i");           // optional
   shape_u_size_o          = shape_shader.getUniformLocation("u_size_o");           // optional
   shape_u_radius          = shape_shader.getUniformLocation("u_radius");           // optional
   shape_u_radius_i        = shape_shader.getUniformLocation("u_radius_i");         // optional
   shape_u_radius_o        = shape_shader.getUniformLocation("u_radius_o");         // optional
   shape_u_ob_radius_i     = shape_shader.getUniformLocation("u_ob_radius_i");      // optional
   shape_u_ob_radius_o     = shape_shader.getUniformLocation("u_ob_radius_o");      // optional
   shape_u_ob_radius_i_max = shape_shader.getUniformLocation("u_ob_radius_i_max");  // optional
   shape_u_ob_radius_o_max = shape_shader.getUniformLocation("u_ob_radius_o_max");  // optional
   shape_u_radius_i_max    = shape_shader.getUniformLocation("u_radius_i_max");     // optional
   shape_u_radius_o_max    = shape_shader.getUniformLocation("u_radius_o_max");     // optional
   shape_u_ob_radius       = shape_shader.getUniformLocation("u_ob_radius");        // optional
   shape_u_ob_radius_max   = shape_shader.getUniformLocation("u_ob_radius_max");    // optional
   shape_u_radius_max      = shape_shader.getUniformLocation("u_radius_max");       // optional
   shape_u_point_radius    = shape_shader.getUniformLocation("u_point_radius");     // optional
   shape_u_color_fill      = shape_shader.getUniformLocation("u_color_fill");       // optional
   shape_u_color_stroke    = shape_shader.getUniformLocation("u_color_stroke");     // optional
   shape_u_global_alpha    = shape_shader.getUniformLocation("u_global_alpha");     // optional
   shape_u_decal_alpha     = shape_shader.getUniformLocation("u_decal_alpha");      // optional
   shape_u_sampler         = shape_shader.getUniformLocation("u_sampler");          // optional
   shape_u_stroke_w        = shape_shader.getUniformLocation("u_stroke_w");         // optional
   shape_u_debug           = shape_shader.getUniformLocation("u_debug");            // optional
   shape_u_tex_0           = shape_shader.getUniformLocation("u_tex_0");            // optional
   shape_u_tex_1           = shape_shader.getUniformLocation("u_tex_1");            // optional

   sBool r = validateShapeShader();
   if(!r)
   {
      Dsdvg_debugprintfvv("[trc] ShaderVG_Shape: shape_a_vertex=%d shape_u_transform=%d\n", shape_a_vertex, shape_u_transform);
      Dsdvg_errorprintf("[---] ShaderVG_Shape::createShapeShader: validateShapeShader() failed\n");
   }
   return r;
}

sBool ShaderVG_Shape::onOpen(void) {
   // implemented by derived classes
   return YAC_FALSE;
}
