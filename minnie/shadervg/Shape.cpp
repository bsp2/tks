// ----
// ---- file   : Shape.cpp
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
// ---- note   :
// ----
// ----
// ----

#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#include "../inc_yac.h"

#define MINNIE_SKIP_TYPEDEFS  defined
#include "../minnie.h"

#include "../inc_opengl.h"

#include "shadervg.h"
#include "shadervg_internal.h"
#include "Shader.h"
#include "Shape.h"

#define Dpaintprintf if( MINNIE_PRINTF);else Dsdvg_printf


ShaderVG_Shape::ShaderVG_Shape(void) {

   b_builtin = YAC_TRUE;

   shape_a_vertex     = -1;
   shape_a_vertex_n   = -1;
   shape_a_vertex_nn  = -1;
   shape_a_pattern    = -1;
   shape_a_pattern_n  = -1;
   shape_a_index      = -1;
   shape_a_color      = -1;
   shape_a_color_n    = -1;
   shape_a_edge       = -1;
   shape_a_scale      = -1;
   shape_a_bc         = -1;
   shape_a_uv         = -1;

   shape_u_transform        = -1;
   shape_u_last_instance    = -1;
   shape_u_aa               = -1;
   shape_u_aa_range         = -1;
#ifdef SHADERVG_AA_EXP
   shape_u_aa_exp           = -1;
#endif // SHADERVG_AA_EXP
   shape_u_center           = -1;
   shape_u_size             = -1;
   shape_u_size_i           = -1;
   shape_u_size_o           = -1;
   shape_u_radius           = -1;
   shape_u_radius_i         = -1;
   shape_u_radius_o         = -1;
   shape_u_ob_radius_i      = -1;
   shape_u_ob_radius_o      = -1;
   shape_u_ob_radius_i_max  = -1;
   shape_u_ob_radius_o_max  = -1;
   shape_u_radius_i_max     = -1;
   shape_u_radius_o_max     = -1;
   shape_u_ob_radius        = -1;
   shape_u_ob_radius_max    = -1;
   shape_u_radius_max       = -1;
   shape_u_point_radius     = -1;
   shape_u_color_fill       = -1;
   shape_u_color_stroke     = -1;
   shape_u_global_alpha     = -1;
   shape_u_decal_alpha      = -1;
   shape_u_sampler          = -1;
   shape_u_stroke_w         = -1;
   shape_u_line_pattern_scl = -1;
   shape_u_line_pattern_off = -1;
   shape_u_line_miter_limit = -1;
#ifdef SHADERVG_DEBUG_FRAG
   shape_u_debug            = -1;
#endif // SHADERVG_DEBUG_FRAG
   shape_u_tex_0            = -1;
   shape_u_tex_1            = -1;
   shape_u_a_min            = -1;
   shape_u_a_max            = -1;
   shape_u_a_maxmin_scale   = -1;
   shape_u_a_exp            = -1;
   shape_u_paint_tex        = -1;
   shape_u_paint_start      = -1;
   shape_u_paint_scale      = -1;
   shape_u_paint_ndir       = -1;
   shape_u_paint_ob_len     = -1;
   shape_u_paint_angle01    = -1;
   shape_u_paint_ob_size    = -1;

   // debug:
   b_draw_inner  = YAC_TRUE;
   b_draw_border = YAC_TRUE;
   b_debug       = YAC_FALSE;
}

ShaderVG_Shape::~ShaderVG_Shape() {
}

void ShaderVG_Shape::allocScratchBuffer(sSI _aVertex, Dsdvg_buffer_ref_t _scratchBuf, sUI _numBytes) {
   sdvg_int_AllocScratchBuffer(_aVertex, _scratchBuf, _numBytes);
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
   shape_a_pattern   = shape_shader.getAttribLocation("a_pattern");    // optional
   shape_a_pattern_n = shape_shader.getAttribLocation("a_pattern_n");  // optional
   shape_a_index     = shape_shader.getAttribLocation("a_index");      // optional
   shape_a_color     = shape_shader.getAttribLocation("a_color");      // optional
   shape_a_color_n   = shape_shader.getAttribLocation("a_color_n");    // optional
   shape_a_edge      = shape_shader.getAttribLocation("a_edge");       // optional
   shape_a_scale     = shape_shader.getAttribLocation("a_scale");      // optional
   shape_a_bc        = shape_shader.getAttribLocation("a_bc");         // optional
   shape_a_uv        = shape_shader.getAttribLocation("a_uv");         // optional

   shape_u_transform        = shape_shader.getUniformLocation("u_transform");
   shape_u_last_instance    = shape_shader.getUniformLocation("u_last_instance");     // optional
   shape_u_aa               = shape_shader.getUniformLocation("u_aa");                // optional
   shape_u_aa_range         = shape_shader.getUniformLocation("u_aa_range");          // optional for non-AA shader
#ifdef SHADERVG_AA_EXP
   shape_u_aa_exp           = shape_shader.getUniformLocation("u_aa_exp");            // optional
#endif // SHADERVG_AA_EXP
   shape_u_center           = shape_shader.getUniformLocation("u_center");            // optional for non-AA shader
   shape_u_size             = shape_shader.getUniformLocation("u_size");              // optional
   shape_u_size_i           = shape_shader.getUniformLocation("u_size_i");            // optional
   shape_u_size_o           = shape_shader.getUniformLocation("u_size_o");            // optional
   shape_u_radius           = shape_shader.getUniformLocation("u_radius");            // optional
   shape_u_radius_i         = shape_shader.getUniformLocation("u_radius_i");          // optional
   shape_u_radius_o         = shape_shader.getUniformLocation("u_radius_o");          // optional
   shape_u_ob_radius_i      = shape_shader.getUniformLocation("u_ob_radius_i");       // optional
   shape_u_ob_radius_o      = shape_shader.getUniformLocation("u_ob_radius_o");       // optional
   shape_u_ob_radius_i_max  = shape_shader.getUniformLocation("u_ob_radius_i_max");   // optional
   shape_u_ob_radius_o_max  = shape_shader.getUniformLocation("u_ob_radius_o_max");   // optional
   shape_u_radius_i_max     = shape_shader.getUniformLocation("u_radius_i_max");      // optional
   shape_u_radius_o_max     = shape_shader.getUniformLocation("u_radius_o_max");      // optional
   shape_u_ob_radius        = shape_shader.getUniformLocation("u_ob_radius");         // optional
   shape_u_ob_radius_max    = shape_shader.getUniformLocation("u_ob_radius_max");     // optional
   shape_u_radius_max       = shape_shader.getUniformLocation("u_radius_max");        // optional
   shape_u_point_radius     = shape_shader.getUniformLocation("u_point_radius");      // optional
   shape_u_color_fill       = shape_shader.getUniformLocation("u_color_fill");        // optional
   shape_u_color_stroke     = shape_shader.getUniformLocation("u_color_stroke");      // optional
   shape_u_global_alpha     = shape_shader.getUniformLocation("u_global_alpha");      // optional
   shape_u_decal_alpha      = shape_shader.getUniformLocation("u_decal_alpha");       // optional
   shape_u_sampler          = shape_shader.getUniformLocation("u_sampler");           // optional
   shape_u_stroke_w         = shape_shader.getUniformLocation("u_stroke_w");          // optional
   shape_u_line_pattern_scl = shape_shader.getUniformLocation("u_line_pattern_scl");  // optional
   shape_u_line_pattern_off = shape_shader.getUniformLocation("u_line_pattern_off");  // optional
   shape_u_line_miter_limit = shape_shader.getUniformLocation("u_line_miter_limit");  // optional
#ifdef SHADERVG_DEBUG_FRAG
   shape_u_debug            = shape_shader.getUniformLocation("u_debug");             // optional
#endif // SHADERVG_DEBUG_FRAG
   shape_u_tex_0            = shape_shader.getUniformLocation("u_tex_0");             // optional
   shape_u_tex_1            = shape_shader.getUniformLocation("u_tex_1");             // optional
   shape_u_a_min            = shape_shader.getUniformLocation("u_a_min");             // optional
   shape_u_a_max            = shape_shader.getUniformLocation("u_a_max");             // optional
   shape_u_a_maxmin_scale   = shape_shader.getUniformLocation("u_a_maxmin_scale");    // optional
   shape_u_a_exp            = shape_shader.getUniformLocation("u_a_exp");             // optional
   shape_u_paint_tex        = shape_shader.getUniformLocation("u_paint_tex");         // optional
   shape_u_paint_start      = shape_shader.getUniformLocation("u_paint_start");       // optional
   shape_u_paint_scale      = shape_shader.getUniformLocation("u_paint_scale");       // optional
   shape_u_paint_ndir       = shape_shader.getUniformLocation("u_paint_ndir");        // optional
   shape_u_paint_ob_len     = shape_shader.getUniformLocation("u_paint_ob_len");      // optional
   shape_u_paint_angle01    = shape_shader.getUniformLocation("u_paint_angle01");     // optional
   shape_u_paint_ob_size    = shape_shader.getUniformLocation("u_paint_ob_size");     // optional

   sBool r = validateShapeShader();
   if(!r)
   {
      Dsdvg_debugprintfvv("[trc] ShaderVG_Shape: shape_a_vertex=%d shape_u_transform=%d\n", shape_a_vertex, shape_u_transform);
      Dsdvg_errorprintf("[---] ShaderVG_Shape::createShapeShader: validateShapeShader() failed\n");
   }
   return r;
}

void ShaderVG_Shape::updatePaintUniforms(const shadervg_paint_t *_paint) {
   sSI loc = shape_u_paint_tex;
   if(loc >= 0)
   {
      Dsdvg_uniform_1i(loc, 0/*tex_unit*/);
   }

   loc = shape_u_paint_start;
   if(loc >= 0)
   {
      Dpaintprintf("[trc] paint_start=(%f;%f)\n", _paint->start_x, _paint->start_y);
      Dsdvg_uniform_2f(loc, _paint->start_x, _paint->start_y);
   }

   loc = shape_u_paint_scale;
   if(loc >= 0)
   {
      const sF32 sclX = (0.0f != _paint->dir_x) ? (1.0f / _paint->dir_x) : 0.0f;
      const sF32 sclY = (0.0f != _paint->dir_y) ? (1.0f / _paint->dir_y) : 0.0f;
      Dpaintprintf("[trc] paint_scale=(%f;%f)\n", sclX, sclY);
      Dsdvg_uniform_2f(loc, sclX, sclY);
   }

   loc = shape_u_paint_ndir;
   if(loc >= 0)
   {
      sF32 dx = _paint->dir_x;
      sF32 dy = _paint->dir_y;
      sF32 l = sqrtf(dx*dx + dy*dy);
      if(l > 0.0f)
      {
         l = 1.0f / l;
         dx *= l;
         dy *= l;
      }
      else
      {
         dx = 0.0f;
         dy = 0.0f;
      }
      Dpaintprintf("[trc] paint_ndir=(%f; %f)  (start=(%f;%f) dir=(%f;%f))\n", dx, dy, _paint->start_x, _paint->start_y, _paint->dir_x, _paint->dir_y);
      Dsdvg_uniform_2f(loc, dx, -dy);
   }

   loc = shape_u_paint_ob_len;
   if(loc >= 0)
   {
      const sF32 dx = _paint->dir_x;
      const sF32 dy = _paint->dir_y;
      sF32 l = sqrtf(dx*dx + dy*dy);
      if(l > 0.0f)
      {
         l = 1.0f / l;
      }
      Dpaintprintf("[trc] paint_ob_len=%f\n", l);
      Dsdvg_uniform_1f(loc, l);
   }

   loc = shape_u_paint_angle01;
   if(loc >= 0)
   {
      Dpaintprintf("[trc] paint_angle01=%f\n", _paint->angle01);
      Dsdvg_uniform_1f(loc, _paint->angle01);
   }

   loc = shape_u_paint_ob_size;
   if(loc >= 0)
   {
      Dpaintprintf("[trc] paint_ob_size(%f;%f)\n", _paint->ob_size_x, _paint->ob_size_y);
      Dsdvg_uniform_2f(loc, _paint->ob_size_x, _paint->ob_size_y);
   }
}

sBool ShaderVG_Shape::onOpen(void) {
   // implemented by derived classes
   return YAC_FALSE;
}

sSI ShaderVG_Shape::bindAndReturnVertexAttrib(void) {
   shape_shader.bind();
   return shape_a_vertex;
}

void ShaderVG_Shape::drawTrianglesFillFlatUniformVBO32Paint(sUI              _vboId,
                                                            sUI              _byteOffset,
                                                            sUI              _numVerts,
                                                            Dsdvg_mat4_ref_t _mvpMatrix,
                                                            sF32             _fillR,   sF32 _fillG,   sF32 _fillB,   sF32 _fillA,
                                                            sF32             _strokeR, sF32 _strokeG, sF32 _strokeB, sF32 _strokeA,
                                                            const shadervg_paint_t *_paint
                                                            ) {
   //
   // VBO vertex format (8 bytes per vertex):
   //   +0 f32 x
   //   +4 f32 y
   //

   sdvg_BindVBO(_vboId);

   shape_shader.bind();

   Dsdvg_uniform_mat4(shape_u_transform, _mvpMatrix);
   Dsdvg_uniform_4f(shape_u_color_fill, _fillR, _fillG, _fillB, _fillA);
   if(shape_u_color_stroke >= 0)
   {
      Dsdvg_uniform_4f(shape_u_color_stroke, _strokeR, _strokeG, _strokeB, _strokeA);
   }

   updatePaintUniforms(_paint);

   Dsdvg_attrib_offset(shape_a_vertex, 2/*size*/, GL_FLOAT, GL_FALSE/*normalize*/, 8/*stride*/, _byteOffset);
   Dsdvg_attrib_enable(shape_a_vertex);

   Dsdvg_draw_triangles(0, _numVerts);

   Dsdvg_attrib_disable(shape_a_vertex);
}

void ShaderVG_Shape::drawTrianglesFillFlatUniformVBO14_2Paint(sUI              _vboId,
                                                              sUI              _byteOffset,
                                                              sUI              _numVerts,
                                                              Dsdvg_mat4_ref_t _mvpMatrix,
                                                              sF32             _fillR,   sF32 _fillG,   sF32 _fillB,   sF32 _fillA,
                                                              sF32             _strokeR, sF32 _strokeG, sF32 _strokeB, sF32 _strokeA,
                                                              const shadervg_paint_t *_paint
                                                              ) {
   //
   // VBO vertex format (4 bytes per vertex):
   //   +0 s14.2 x
   //   +2 s14.2 y
   //

   sdvg_BindVBO(_vboId);

   shape_shader.bind();

   Dsdvg_uniform_mat4(shape_u_transform, _mvpMatrix);
   Dsdvg_uniform_4f(shape_u_color_fill, _fillR, _fillG, _fillB, _fillA);
   if(shape_u_color_stroke >= 0)
   {
      Dsdvg_uniform_4f(shape_u_color_stroke, _strokeR, _strokeG, _strokeB, _strokeA);
   }

   updatePaintUniforms(_paint);

   Dsdvg_attrib_offset(shape_a_vertex, 2/*size*/, GL_SHORT, GL_FALSE/*normalize*/, 4/*stride*/, _byteOffset);
   Dsdvg_attrib_enable(shape_a_vertex);

   Dsdvg_draw_triangles(0, _numVerts);

   Dsdvg_attrib_disable(shape_a_vertex);
}

void ShaderVG_Shape::drawRectFillAAVBO32Paint(sUI              _vboId,
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
                                              sF32             _aaRange,
                                              sF32             _aaExp,
                                              const shadervg_paint_t *_paint
                                              ) {

   sdvg_BindVBO(_vboId);

   if(_numVertsInner > 0u && b_draw_inner)
   {
      sSI aVertexFill = sdvg_int_BindFillShader();

      Dsdvg_attrib_offset(aVertexFill, 2/*size*/, GL_FLOAT, GL_FALSE/*normalize*/, 0/*stride*/, _byteOffsetInner);
      Dsdvg_draw_arrays_vbo(GL_TRIANGLES, 0, _numVertsInner);

      sdvg_int_EndFillShader();
   }

   // Outer border
   if(_numVertsBorder > 0u && b_draw_border)
   {
      shape_shader.bind();

      Dsdvg_uniform_mat4(shape_u_transform, _mvpMatrix);
      Dsdvg_uniform_2f(shape_u_center,   _centerX, _centerY);
      Dsdvg_uniform_2f(shape_u_size,     _sizeX, _sizeY);
      Dsdvg_uniform_1f(shape_u_aa_range, _aaRange);

#ifdef SHADERVG_AA_EXP
      if(-1 != shape_u_aa_exp)
      {
         Dsdvg_uniform_1f(shape_u_aa_exp, _aaExp);
      }
#endif // SHADERVG_AA_EXP

      Dsdvg_uniform_4f(shape_u_color_fill, _fillR, _fillG, _fillB, _fillA);

#ifdef SHADERVG_DEBUG_FRAG
      if(-1 != shape_u_debug)
      {
         Dsdvg_uniform_1f(shape_u_debug, b_debug ? 1.0f : 0.0f);
      }
#endif // SHADERVG_DEBUG_FRAG

      updatePaintUniforms(_paint);

      Dsdvg_attrib_offset(shape_a_vertex, 2/*size*/, GL_FLOAT, GL_FALSE/*normalize*/, 0/*stride*/, _byteOffsetBorder);
      Dsdvg_attrib_enable(shape_a_vertex);

      Dsdvg_draw_arrays_vbo(_glPrimTypeBorder, 0, _numVertsBorder);

      Dsdvg_attrib_disable(shape_a_vertex);
   }
}

void ShaderVG_Shape::drawRectFillAAPaint(Dsdvg_buffer_ref_t _scratchBuf,
                                         Dsdvg_mat4_ref_t _mvpMatrix,
                                         sF32 _centerX, sF32 _centerY,
                                         sF32 _sizeX,   sF32 _sizeY,
                                         sF32 _fillR,   sF32 _fillG,   sF32 _fillB,   sF32 _fillA,
                                         sF32 _strokeR, sF32 _strokeG, sF32 _strokeB, sF32 _strokeA,
                                         sF32 _decalAlpha,
                                         sF32 _aaRange,
                                         sF32 _aaExp,
                                         const shadervg_paint_t *_paint
                                         ) {
   /* Dyac_host_printf("xxx drawRectFill center=(%f;%f) size=(%f;%f)\n", _centerX, _centerY, _sizeX, _sizeY); */

   sdvg_int_BindScratchBuffer();

   sBool bSingle = ((_sizeX*_sizeY) <= RECT_SINGLE_AREA_THRESHOLD);

   // Inner
   sBool bInner = !bSingle && b_draw_inner;
   if(bInner)
   {
      sSI aVertexFill = sdvg_int_BindFillShader();

      sUI numTris = 2u;
      allocScratchBuffer(aVertexFill, _scratchBuf, (numTris*3*2/*xy*/*4/*float*/));

      // left/top
      Dstream_write_f32(_scratchBuf, _centerX - _sizeX + _aaRange);
      Dstream_write_f32(_scratchBuf, _centerY - _sizeY + _aaRange);

      Dstream_write_f32(_scratchBuf, _centerX + _sizeX - _aaRange);
      Dstream_write_f32(_scratchBuf, _centerY - _sizeY + _aaRange);

      Dstream_write_f32(_scratchBuf, _centerX - _sizeX + _aaRange);
      Dstream_write_f32(_scratchBuf, _centerY + _sizeY - _aaRange);

      // right/bottom
      Dstream_write_f32(_scratchBuf, _centerX - _sizeX + _aaRange);
      Dstream_write_f32(_scratchBuf, _centerY + _sizeY - _aaRange);

      Dstream_write_f32(_scratchBuf, _centerX + _sizeX - _aaRange);
      Dstream_write_f32(_scratchBuf, _centerY - _sizeY + _aaRange);

      Dstream_write_f32(_scratchBuf, _centerX + _sizeX - _aaRange);
      Dstream_write_f32(_scratchBuf, _centerY + _sizeY - _aaRange);

      /* Dyac_host_printf("xxx rectfill: scratchBuf.offset=%u size=%u numTris=%u\n", Dstream_get_offset(_scratchBuf), _scratchBuf->size, numTris); */

      Dsdvg_draw_triangles(0, numTris*3);

      sdvg_int_EndFillShader();
   }

   // Outer corners
   shape_shader.bind();

   Dsdvg_uniform_mat4(shape_u_transform, _mvpMatrix);
   Dsdvg_uniform_2f(shape_u_center,   _centerX, _centerY);
   Dsdvg_uniform_2f(shape_u_size,     _sizeX, _sizeY);
   Dsdvg_uniform_1f(shape_u_aa_range, _aaRange);

#ifdef SHADERVG_AA_EXP
   if(-1 != shape_u_aa_exp)
   {
      Dsdvg_uniform_1f(shape_u_aa_exp, _aaExp);
   }
#endif // SHADERVG_AA_EXP

   Dsdvg_uniform_4f(shape_u_color_fill, _fillR, _fillG, _fillB, _fillA);

   if(-1 != shape_u_color_stroke)
   {
      Dsdvg_uniform_4f(shape_u_color_stroke, _strokeR, _strokeG, _strokeB, _strokeA);
   }

   if(-1 != shape_u_decal_alpha)
   {
      Dsdvg_uniform_1f(shape_u_decal_alpha, _decalAlpha);
   }

#ifdef SHADERVG_DEBUG_FRAG
   if(-1 != shape_u_debug)
   {
      Dsdvg_uniform_1f(shape_u_debug, b_debug ? 1.0f : 0.0f);
   }
#endif // SHADERVG_DEBUG_FRAG

   updatePaintUniforms(_paint);

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
      allocScratchBuffer(shape_a_vertex, _scratchBuf, (8*3*2/*xy*/*4/*float*/));

      EmitRectBorderVertices(_scratchBuf,
                             _centerX, _centerY,
                             _sizeX, _sizeY,
                             0.0f/*_strokeW*/,
                             _aaRange
                             );

      Dsdvg_draw_triangles(0, 8*3);
   }

   Dsdvg_attrib_disable(shape_a_vertex);
}

void ShaderVG_Shape::drawRectStrokeAAVBO32Paint(sUI              _vboId,
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
                                                sF32             _aaExp,
                                                const shadervg_paint_t *_paint
                                                ) {
   sdvg_BindVBO(_vboId);

   // Outer border
   if(_numVertsBorder > 0u)
   {
      shape_shader.bind();

      Dsdvg_uniform_mat4(shape_u_transform, _mvpMatrix);
      Dsdvg_uniform_2f(shape_u_center,   _centerX, _centerY);
      Dsdvg_uniform_2f(shape_u_size_i,   _sizeX - _strokeW, _sizeY - _strokeW);
      Dsdvg_uniform_2f(shape_u_size_o,   _sizeX + _strokeW, _sizeY + _strokeW);
      Dsdvg_uniform_1f(shape_u_aa_range, _aaRange);

#ifdef SHADERVG_AA_EXP
      if(-1 != shape_u_aa_exp)
      {
         Dsdvg_uniform_1f(shape_u_aa_exp, _aaExp);
      }
#endif // SHADERVG_AA_EXP

      Dsdvg_uniform_4f(shape_u_color_stroke, _strokeR, _strokeG, _strokeB, _strokeA);

#ifdef SHADERVG_DEBUG_FRAG
      if(-1 != shape_u_debug)
      {
         Dsdvg_uniform_1f(shape_u_debug, b_debug ? 1.0f : 0.0f);
      }
#endif // SHADERVG_DEBUG_FRAG

      updatePaintUniforms(_paint);

      Dsdvg_attrib_offset(shape_a_vertex, 2/*size*/, GL_FLOAT, GL_FALSE/*normalize*/, 0/*stride*/, _byteOffsetBorder);

      Dsdvg_attrib_enable(shape_a_vertex);

      Dsdvg_draw_arrays_vbo(_glPrimTypeBorder, 0, _numVertsBorder);

      Dsdvg_attrib_disable(shape_a_vertex);
   }
}

void ShaderVG_Shape::drawRectStrokeAAPaint(Dsdvg_buffer_ref_t _scratchBuf,
                                           Dsdvg_mat4_ref_t _mvpMatrix,
                                           sF32 _centerX, sF32 _centerY,
                                           sF32 _sizeX,   sF32 _sizeY,
                                           sF32 _fillR, sF32 _fillG, sF32 _fillB, sF32 _fillA,
                                           sF32 _strokeR, sF32 _strokeG, sF32 _strokeB, sF32 _strokeA,
                                           sF32 _strokeW,
                                           sF32 _decalAlpha,
                                           sF32 _aaRange,
                                           sF32 _aaExp,
                                           const shadervg_paint_t *_paint
                                           ) {

   sdvg_int_BindScratchBuffer();

   sBool bSingle = ((_sizeX*_sizeY) <= RECT_SINGLE_AREA_THRESHOLD);

   // Outer corners
   shape_shader.bind();

   Dsdvg_uniform_mat4(shape_u_transform, _mvpMatrix);
   Dsdvg_uniform_2f(shape_u_center,   _centerX, _centerY);
   Dsdvg_uniform_2f(shape_u_size_i,   _sizeX - _strokeW, _sizeY - _strokeW);
   Dsdvg_uniform_2f(shape_u_size_o,   _sizeX + _strokeW, _sizeY + _strokeW);
   Dsdvg_uniform_1f(shape_u_aa_range, _aaRange);

#ifdef SHADERVG_AA_EXP
   if(-1 != shape_u_aa_exp)
   {
      Dsdvg_uniform_1f(shape_u_aa_exp, _aaExp);
   }
#endif // SHADERVG_AA_EXP

   if(-1 != shape_u_color_fill)
   {
      Dsdvg_uniform_4f(shape_u_color_fill, _fillR, _fillG, _fillB, _fillA);
   }

   Dsdvg_uniform_4f(shape_u_color_stroke, _strokeR, _strokeG, _strokeB, _strokeA);

   if(-1 != shape_u_decal_alpha)
   {
      Dsdvg_uniform_1f(shape_u_decal_alpha, _decalAlpha);
   }

#ifdef SHADERVG_DEBUG_FRAG
   if(-1 != shape_u_debug)
   {
      Dsdvg_uniform_1f(shape_u_debug, b_debug ? 1.0f : 0.0f);
   }
#endif // SHADERVG_DEBUG_FRAG

   updatePaintUniforms(_paint);

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
   else
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

void ShaderVG_Shape::drawEllipseFillAAVBO32Paint(sUI              _vboId,
                                                 sUI              _byteOffsetInner,
                                                 sUI              _numVertsInner,
                                                 sUI              _byteOffsetBorder,
                                                 sUI              _numVertsBorder,
                                                 sUI              _glPrimTypeBorder,
                                                 Dsdvg_mat4_ref_t _mvpMatrix,
                                                 sF32    _centerX, sF32 _centerY,
                                                 sF32    _radiusX, sF32 _radiusY,
                                                 sF32    _fillR,   sF32 _fillG,   sF32 _fillB,   sF32 _fillA,
                                                 sF32    _strokeR, sF32 _strokeG, sF32 _strokeB, sF32 _strokeA,
                                                 sF32    _aaRange,
                                                 sF32    _aaExp,
                                                 const shadervg_paint_t *_paint
                                                 ) {

   sdvg_BindVBO(_vboId);

   if(_numVertsInner > 0u && b_draw_inner)
   {
      sSI aVertexFill = sdvg_int_BindFillShader();

      Dsdvg_attrib_offset(aVertexFill, 2/*size*/, GL_FLOAT, GL_FALSE/*normalize*/, 0/*stride*/, _byteOffsetInner);
      Dsdvg_draw_triangle_fan_vbo(0, _numVertsInner);

      sdvg_int_EndFillShader();
   }

   // Outer border
   if(_numVertsBorder > 0u && b_draw_border)
   {
      shape_shader.bind();

      Dsdvg_uniform_mat4(shape_u_transform, _mvpMatrix);
      Dsdvg_uniform_2f(shape_u_center,        _centerX, _centerY);
      Dsdvg_uniform_2f(shape_u_radius,        _radiusX, _radiusY);
      Dsdvg_uniform_2f(shape_u_ob_radius,     1.0f / _radiusX, 1.0f / _radiusY);
      Dsdvg_uniform_1f(shape_u_ob_radius_max, (_radiusX > _radiusY) ? (1.0f / _radiusX) : (1.0f / _radiusY));
      Dsdvg_uniform_1f(shape_u_radius_max,    (_radiusX > _radiusY) ? _radiusX : _radiusY);
      Dsdvg_uniform_1f(shape_u_aa_range,      _aaRange);

#ifdef SHADERVG_AA_EXP
      if(-1 != shape_u_aa_exp)
      {
         Dsdvg_uniform_1f(shape_u_aa_exp, _aaExp);
      }
#endif // SHADERVG_AA_EXP

      Dsdvg_uniform_4f(shape_u_color_fill, _fillR, _fillG, _fillB, _fillA);

#ifdef SHADERVG_DEBUG_FRAG
      if(-1 != shape_u_debug)
      {
         Dsdvg_uniform_1f(shape_u_debug, b_debug ? 1.0f : 0.0f);
      }
#endif // SHADERVG_DEBUG_FRAG

      updatePaintUniforms(_paint);

      Dsdvg_attrib_offset(shape_a_vertex, 2/*size*/, GL_FLOAT, GL_FALSE/*normalize*/, 0/*stride*/, _byteOffsetBorder);
      Dsdvg_attrib_enable(shape_a_vertex);

      Dsdvg_draw_arrays_vbo(_glPrimTypeBorder, 0, _numVertsBorder);

      Dsdvg_attrib_disable(shape_a_vertex);
   }
}

void ShaderVG_Shape::drawEllipseFillAAPaint(Dsdvg_buffer_ref_t _scratchBuf,
                                            Dsdvg_mat4_ref_t _mvpMatrix,
                                            sF32 _centerX, sF32 _centerY,
                                            sF32 _radiusX, sF32 _radiusY,
                                            sF32 _fillR,   sF32 _fillG,   sF32 _fillB,   sF32 _fillA,
                                            sF32 _strokeR, sF32 _strokeG, sF32 _strokeB, sF32 _strokeA,
                                            sF32 _decalAlpha,
                                            sF32 _aaRange,
                                            sF32 _aaExp,
                                            const shadervg_paint_t *_paint
                                            ) {

   sdvg_int_BindScratchBuffer();

   /* Dprintf("xxx EllipseFillAA: c=(%f;%f) r=(%f;%f)\n", _centerX, _centerY, _radiusX, _radiusY); */

   const sUI numSeg = 16u;
   const sF32 rxI = _radiusX - ELLIPSE_EXTRUDE_I;
   const sF32 rxO = _radiusX + ELLIPSE_EXTRUDE_O;
   const sF32 ryI = _radiusY - ELLIPSE_EXTRUDE_I;
   const sF32 ryO = _radiusY + ELLIPSE_EXTRUDE_O;
   const sF32 aStep = sF32(sM_2PI / numSeg);
   sF32 a;
   sUI numVerts;
   sUI allocSz;

   const sBool bSingle =
      (_radiusX < ELLIPSE_SINGLE_RADIUS_THRESHOLD) ||
      (_radiusY < ELLIPSE_SINGLE_RADIUS_THRESHOLD)
      ;

   // Inner
   if(!bSingle && b_draw_inner)
   {
      sSI aVertexFill = sdvg_int_BindFillShader();

      // Calc inner mesh
      a = aStep;
      numVerts = (2u + numSeg);

      allocSz = numVerts * 2u/*xy*/ * 4u/*float*/;
      allocScratchBuffer(aVertexFill, _scratchBuf, allocSz);

      Dstream_write_f32(_scratchBuf, _centerX);
      Dstream_write_f32(_scratchBuf, _centerY);
      Dstream_write_f32(_scratchBuf, _centerX + rxI);
      Dstream_write_f32(_scratchBuf, _centerY);

      for(sUI segIdx = 0u; segIdx < numSeg; segIdx++)
      {
         Dstream_write_f32(_scratchBuf, _centerX + rxI * cosf(a));
         Dstream_write_f32(_scratchBuf, _centerY + ryI * sinf(a));
         a += aStep;
      }

      Dsdvg_draw_triangle_fan(0, numVerts);

      sdvg_int_EndFillShader();
   }

   // Outer border
   shape_shader.bind();

   Dsdvg_uniform_mat4(shape_u_transform, _mvpMatrix);
   Dsdvg_uniform_2f(shape_u_center,        _centerX, _centerY);
   Dsdvg_uniform_2f(shape_u_radius,        _radiusX, _radiusY);
   Dsdvg_uniform_2f(shape_u_ob_radius,     1.0f / _radiusX, 1.0f / _radiusY);
   Dsdvg_uniform_1f(shape_u_ob_radius_max, (_radiusX > _radiusY) ? (1.0f / _radiusX) : (1.0f / _radiusY));
   Dsdvg_uniform_1f(shape_u_radius_max,    (_radiusX > _radiusY) ? _radiusX : _radiusY);
   Dsdvg_uniform_1f(shape_u_aa_range,      _aaRange);

#ifdef SHADERVG_AA_EXP
   if(-1 != shape_u_aa_exp)
   {
      Dsdvg_uniform_1f(shape_u_aa_exp, _aaExp);
   }
#endif // SHADERVG_AA_EXP

   Dsdvg_uniform_4f(shape_u_color_fill, _fillR, _fillG, _fillB, _fillA);

   if(-1 != shape_u_color_stroke)
   {
      Dsdvg_uniform_4f(shape_u_color_stroke, _strokeR, _strokeG, _strokeB, _strokeA);
   }

   if(-1 != shape_u_decal_alpha)
   {
      Dsdvg_uniform_1f(shape_u_decal_alpha, _decalAlpha);
   }

#ifdef SHADERVG_DEBUG_FRAG
   if(-1 != shape_u_debug)
   {
      Dsdvg_uniform_1f(shape_u_debug, b_debug ? 1.0f : 0.0f);
   }
#endif // SHADERVG_DEBUG_FRAG

   Dsdvg_attrib_enable(shape_a_vertex);

   updatePaintUniforms(_paint);

   // Calc border mesh
   if(b_draw_border || bSingle)
   {
      if(bSingle)
      {
         numVerts = 4u;

         allocSz = numVerts * 2u/*xy*/ * 4u/*float*/;
         allocScratchBuffer(shape_a_vertex, _scratchBuf, allocSz);

         const sF32 x = _centerX - _radiusX;
         const sF32 y = _centerY - _radiusY;
         const sF32 w = _radiusX * 2.0f;
         const sF32 h = _radiusY * 2.0f;

         Dstream_write_f32(_scratchBuf, x);       Dstream_write_f32(_scratchBuf, y);
         Dstream_write_f32(_scratchBuf, x + w);   Dstream_write_f32(_scratchBuf, y);
         Dstream_write_f32(_scratchBuf, x + w);   Dstream_write_f32(_scratchBuf, y + h);
         Dstream_write_f32(_scratchBuf, x);       Dstream_write_f32(_scratchBuf, y + h);

         Dsdvg_draw_triangle_fan(0, numVerts);
      }
      else
      {
         a = aStep;
         numVerts = (2u + numSeg * 2u);

         allocSz = numVerts * 2u/*xy*/ * 4u/*float*/;
         allocScratchBuffer(shape_a_vertex, _scratchBuf, allocSz);

         Dstream_write_f32(_scratchBuf, _centerX + rxI);
         Dstream_write_f32(_scratchBuf, _centerY +   0);
         Dstream_write_f32(_scratchBuf, _centerX + rxO);
         Dstream_write_f32(_scratchBuf, _centerY +   0);

         for(sUI segIdx = 0u; segIdx < numSeg; segIdx++)
         {
            Dstream_write_f32(_scratchBuf, _centerX + rxI * cosf(a));
            Dstream_write_f32(_scratchBuf, _centerY + ryI * sinf(a));
            Dstream_write_f32(_scratchBuf, _centerX + rxO * cosf(a));
            Dstream_write_f32(_scratchBuf, _centerY + ryO * sinf(a));
            a += aStep;
         }

         Dsdvg_draw_triangle_strip(0, numVerts);
      }
   }

   Dsdvg_attrib_disable(shape_a_vertex);
}

void ShaderVG_Shape::drawEllipseStrokeAAVBO32Paint(sUI              _vboId,
                                                   sUI              _byteOffsetBorder,
                                                   sUI              _numVertsBorder,
                                                   sUI              _glPrimTypeBorder,
                                                   Dsdvg_mat4_ref_t _mvpMatrix,
                                                   sF32    _centerX, sF32 _centerY,
                                                   sF32    _radiusX, sF32 _radiusY,
                                                   sF32    _fillR,   sF32 _fillG,   sF32 _fillB,   sF32 _fillA,
                                                   sF32    _strokeR, sF32 _strokeG, sF32 _strokeB, sF32 _strokeA,
                                                   sF32    _strokeW,
                                                   sF32    _aaRange,
                                                   sF32    _aaExp,
                                                   const shadervg_paint_t *_paint
                                                   ) {

   sdvg_BindVBO(_vboId);

   // Outer border
   if(_numVertsBorder > 0u)
   {
      shape_shader.bind();

      Dsdvg_uniform_mat4(shape_u_transform, _mvpMatrix);
      Dsdvg_uniform_2f(shape_u_center,   _centerX, _centerY);
      Dsdvg_uniform_2f(shape_u_size_i,   _radiusX - _strokeW, _radiusY - _strokeW);
      Dsdvg_uniform_2f(shape_u_size_o,   _radiusX + _strokeW, _radiusY + _strokeW);
      Dsdvg_uniform_2f(shape_u_radius,   _radiusX, _radiusY);
      Dsdvg_uniform_2f(shape_u_radius_i, _radiusX - _strokeW, _radiusY - _strokeW);
      Dsdvg_uniform_2f(shape_u_radius_o, _radiusX + _strokeW, _radiusY + _strokeW);
      const sF32 radiusIx = _radiusX - _strokeW;
      const sF32 radiusIy = _radiusY - _strokeW;
      const sF32 radiusOx = _radiusX + _strokeW;
      const sF32 radiusOy = _radiusY + _strokeW;
      Dsdvg_uniform_2f(shape_u_radius_i,        radiusIx, radiusIy);
      Dsdvg_uniform_2f(shape_u_radius_o,        radiusOx, radiusOy);
      Dsdvg_uniform_2f(shape_u_ob_radius_i,     1.0f / radiusIx, 1.0f / radiusIy);
      Dsdvg_uniform_2f(shape_u_ob_radius_o,     1.0f / radiusOx, 1.0f / radiusOy);
      Dsdvg_uniform_1f(shape_u_ob_radius_i_max, (radiusIx > radiusIy) ? (1.0f / radiusIx) : (1.0f / radiusIy));
      Dsdvg_uniform_1f(shape_u_ob_radius_o_max, (radiusOx > radiusOy) ? (1.0f / radiusOx) : (1.0f / radiusOy));
      Dsdvg_uniform_1f(shape_u_radius_i_max,    (radiusIx > radiusIy) ? radiusIx : radiusIy);
      Dsdvg_uniform_1f(shape_u_radius_o_max,    (radiusOx > radiusOy) ? radiusOx : radiusOy);
      Dsdvg_uniform_1f(shape_u_aa_range, _aaRange);

#ifdef SHADERVG_AA_EXP
      if(-1 != shape_u_aa_exp)
      {
         Dsdvg_uniform_1f(shape_u_aa_exp, _aaExp);
      }
#endif // SHADERVG_AA_EXP

      Dsdvg_uniform_4f(shape_u_color_stroke, _strokeR, _strokeG, _strokeB, _strokeA);

#ifdef SHADERVG_DEBUG_FRAG
      if(-1 != shape_u_debug)
      {
         Dsdvg_uniform_1f(shape_u_debug, b_debug ? 1.0f : 0.0f);
      }
#endif // SHADERVG_DEBUG_FRAG

      updatePaintUniforms(_paint);

      Dsdvg_attrib_offset(shape_a_vertex, 2/*size*/, GL_FLOAT, GL_FALSE/*normalize*/, 0/*stride*/, _byteOffsetBorder);
      Dsdvg_attrib_enable(shape_a_vertex);

      Dsdvg_draw_arrays_vbo(_glPrimTypeBorder, 0, _numVertsBorder);

      Dsdvg_attrib_disable(shape_a_vertex);

      shape_shader.unbind();
   }
}

void ShaderVG_Shape::drawEllipseStrokeAAPaint(Dsdvg_buffer_ref_t _scratchBuf,
                                              Dsdvg_mat4_ref_t _mvpMatrix,
                                              sF32 _centerX, sF32 _centerY,
                                              sF32 _radiusX, sF32 _radiusY,
                                              sF32 _fillR, sF32 _fillG, sF32 _fillB, sF32 _fillA,
                                              sF32 _strokeR, sF32 _strokeG, sF32 _strokeB, sF32 _strokeA,
                                              sF32 _strokeW,
                                              sF32 _decalAlpha,
                                              sF32 _aaRange,
                                              sF32 _aaExp,
                                              const shadervg_paint_t *_paint
                                              ) {

   sdvg_int_BindScratchBuffer();

   const sUI numSeg = ELLIPSE_NUM_SEG;
   const sF32 rxI = _radiusX - _strokeW - ELLIPSE_EXTRUDE_I;
   const sF32 rxO = _radiusX + _strokeW + ELLIPSE_EXTRUDE_O;
   const sF32 ryI = _radiusY - _strokeW - ELLIPSE_EXTRUDE_I;
   const sF32 ryO = _radiusY + _strokeW + ELLIPSE_EXTRUDE_O;
   const sF32 aStep = sF32(sM_2PI / numSeg);
   sF32 a;
   sUI numVerts;
   sUI allocSz;

   const sBool bSingle =
      (_radiusX < ELLIPSE_SINGLE_RADIUS_THRESHOLD) ||
      (_radiusY < ELLIPSE_SINGLE_RADIUS_THRESHOLD)
      ;

   // Outer corners
   shape_shader.bind();

   Dsdvg_uniform_mat4(shape_u_transform, _mvpMatrix);
   Dsdvg_uniform_2f(shape_u_center,   _centerX, _centerY);
   Dsdvg_uniform_2f(shape_u_size_i,   _radiusX - _strokeW, _radiusY - _strokeW);
   Dsdvg_uniform_2f(shape_u_size_o,   _radiusX + _strokeW, _radiusY + _strokeW);
   Dsdvg_uniform_2f(shape_u_radius,   _radiusX, _radiusY);
   Dsdvg_uniform_2f(shape_u_radius_i, _radiusX - _strokeW, _radiusY - _strokeW);
   Dsdvg_uniform_2f(shape_u_radius_o, _radiusX + _strokeW, _radiusY + _strokeW);
   const sF32 radiusIx = _radiusX - _strokeW;
   const sF32 radiusIy = _radiusY - _strokeW;
   const sF32 radiusOx = _radiusX + _strokeW;
   const sF32 radiusOy = _radiusY + _strokeW;
   Dsdvg_uniform_2f(shape_u_radius_i,        radiusIx, radiusIy);
   Dsdvg_uniform_2f(shape_u_radius_o,        radiusOx, radiusOy);
   Dsdvg_uniform_2f(shape_u_ob_radius_i,     1.0f / radiusIx, 1.0f / radiusIy);
   Dsdvg_uniform_2f(shape_u_ob_radius_o,     1.0f / radiusOx, 1.0f / radiusOy);
   Dsdvg_uniform_1f(shape_u_ob_radius_i_max, (radiusIx > radiusIy) ? (1.0f / radiusIx) : (1.0f / radiusIy));
   Dsdvg_uniform_1f(shape_u_ob_radius_o_max, (radiusOx > radiusOy) ? (1.0f / radiusOx) : (1.0f / radiusOy));
   Dsdvg_uniform_1f(shape_u_radius_i_max,    (radiusIx > radiusIy) ? radiusIx : radiusIy);
   Dsdvg_uniform_1f(shape_u_radius_o_max,    (radiusOx > radiusOy) ? radiusOx : radiusOy);
   Dsdvg_uniform_1f(shape_u_aa_range, _aaRange);

#ifdef SHADERVG_AA_EXP
   if(-1 != shape_u_aa_exp)
   {
      Dsdvg_uniform_1f(shape_u_aa_exp, _aaExp);
   }
#endif // SHADERVG_AA_EXP

   if(-1 != shape_u_color_fill)
   {
      Dsdvg_uniform_4f(shape_u_color_fill, _fillR, _fillG, _fillB, _fillA);
   }

   Dsdvg_uniform_4f(shape_u_color_stroke, _strokeR, _strokeG, _strokeB, _strokeA);

   if(-1 != shape_u_decal_alpha)
   {
      Dsdvg_uniform_1f(shape_u_decal_alpha, _decalAlpha);
   }

#ifdef SHADERVG_DEBUG_FRAG
   if(-1 != shape_u_debug)
   {
      Dsdvg_uniform_1f(shape_u_debug, b_debug ? 1.0f : 0.0f);
   }
#endif // SHADERVG_DEBUG_FRAG

   updatePaintUniforms(_paint);

   Dsdvg_attrib_enable(shape_a_vertex);

   // Calc border mesh
   if(bSingle)
   {
      numVerts = 4u;
      allocSz = numVerts * 2u/*xy*/ * 4u/*float*/;
      allocScratchBuffer(shape_a_vertex, _scratchBuf, allocSz);

      const sF32 x = _centerX - _radiusX - _strokeW;
      const sF32 y = _centerY - _radiusY - _strokeW;
      const sF32 w = (_radiusX + _strokeW) * 2.0f;
      const sF32 h = (_radiusY + _strokeW) * 2.0f;

      Dstream_write_f32(_scratchBuf, x);      Dstream_write_f32(_scratchBuf, y);
      Dstream_write_f32(_scratchBuf, x + w);  Dstream_write_f32(_scratchBuf, y);
      Dstream_write_f32(_scratchBuf, x + w);  Dstream_write_f32(_scratchBuf, y + h);
      Dstream_write_f32(_scratchBuf, x);      Dstream_write_f32(_scratchBuf, y + h);

      Dsdvg_draw_triangle_fan(0, numVerts);
   }
   else
   {
      a = aStep;
      numVerts = (2u + numSeg * 2u);

      allocSz = numVerts * 2u/*xy*/ * 4u/*float*/;
      allocScratchBuffer(shape_a_vertex, _scratchBuf, allocSz);

      Dstream_write_f32(_scratchBuf, _centerX + rxI);
      Dstream_write_f32(_scratchBuf, _centerY +   0);
      Dstream_write_f32(_scratchBuf, _centerX + rxO);
      Dstream_write_f32(_scratchBuf, _centerY +   0);

      for(sUI segIdx = 0u; segIdx < numSeg; segIdx++)
      {
         Dstream_write_f32(_scratchBuf, _centerX + rxI * cosf(a));
         Dstream_write_f32(_scratchBuf, _centerY + ryI * sinf(a));
         Dstream_write_f32(_scratchBuf, _centerX + rxO * cosf(a));
         Dstream_write_f32(_scratchBuf, _centerY + ryO * sinf(a));
         a += aStep;
      }

      Dsdvg_draw_triangle_strip(0, numVerts);
   }

   Dsdvg_attrib_disable(shape_a_vertex);
}

void ShaderVG_Shape::drawRoundRectFillAAVBO32Paint(sUI              _vboId,
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
                                                   sF32             _strokeR, sF32 _strokeG, sF32 _strokeB, sF32 _strokeA,
                                                   sF32             _aaRange,
                                                   sF32             _aaExp,
                                                   const shadervg_paint_t *_paint
                                                   ) {

   sdvg_BindVBO(_vboId);

   if(_numVertsInner > 0u && b_draw_inner)
   {
      sSI aVertexFill = sdvg_int_BindFillShader();

      Dsdvg_attrib_offset(aVertexFill, 2/*size*/, GL_FLOAT, GL_FALSE/*normalize*/, 0/*stride*/, _byteOffsetInner);
      Dsdvg_draw_triangles_vbo(0, _numVertsInner);

      sdvg_int_EndFillShader();
   }

   // Outer border
   if(_numVertsBorder > 0u && b_draw_border)
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

#ifdef SHADERVG_AA_EXP
      if(-1 != shape_u_aa_exp)
      {
         Dsdvg_uniform_1f(shape_u_aa_exp, _aaExp);
      }
#endif // SHADERVG_AA_EXP

      Dsdvg_uniform_4f(shape_u_color_fill, _fillR, _fillG, _fillB, _fillA);

#ifdef SHADERVG_DEBUG_FRAG
      if(-1 != shape_u_debug)
      {
         Dsdvg_uniform_1f(shape_u_debug, b_debug ? 1.0f : 0.0f);
      }
#endif // SHADERVG_DEBUG_FRAG

      updatePaintUniforms(_paint);

      Dsdvg_attrib_offset(shape_a_vertex, 2/*size*/, GL_FLOAT, GL_FALSE/*normalize*/, 0/*stride*/, _byteOffsetBorder);
      Dsdvg_attrib_enable(shape_a_vertex);

      Dsdvg_draw_arrays_vbo(_glPrimTypeBorder, 0, _numVertsBorder);

      Dsdvg_attrib_disable(shape_a_vertex);
   }
}

void ShaderVG_Shape::drawRoundRectFillAAPaint(Dsdvg_buffer_ref_t _scratchBuf,
                                              Dsdvg_mat4_ref_t _mvpMatrix,
                                              sF32 _centerX, sF32 _centerY,
                                              sF32 _sizeX,   sF32 _sizeY,
                                              sF32 _radiusX, sF32 _radiusY,
                                              sF32 _fillR,   sF32 _fillG,   sF32 _fillB,   sF32 _fillA,
                                              sF32 _strokeR, sF32 _strokeG, sF32 _strokeB, sF32 _strokeA,
                                              sF32 _decalAlpha,
                                              sF32 _aaRange,
                                              sF32 _aaExp,
                                              const shadervg_paint_t *_paint
                                              ) {

   sdvg_int_BindScratchBuffer();

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
      sSI aVertexFill = sdvg_int_BindFillShader();

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

      sdvg_int_EndFillShader();
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

#ifdef SHADERVG_AA_EXP
   if(-1 != shape_u_aa_exp)
   {
      Dsdvg_uniform_1f(shape_u_aa_exp, _aaExp);
   }
#endif // SHADERVG_AA_EXP

   Dsdvg_uniform_4f(shape_u_color_fill, _fillR, _fillG, _fillB, _fillA);

   if(-1 != shape_u_color_stroke)
   {
      Dsdvg_uniform_4f(shape_u_color_stroke, _strokeR, _strokeG, _strokeB, _strokeA);
   }

   if(-1 != shape_u_decal_alpha)
   {
      Dsdvg_uniform_1f(shape_u_decal_alpha, _decalAlpha);
   }

#ifdef SHADERVG_DEBUG_FRAG
   if(-1 != shape_u_debug)
   {
      Dsdvg_uniform_1f(shape_u_debug, b_debug ? 1.0f : 0.0f);
   }
#endif // SHADERVG_DEBUG_FRAG

   updatePaintUniforms(_paint);

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

void ShaderVG_Shape::drawRoundRectStrokeAAVBO32Paint(sUI              _vboId,
                                                     sUI              _byteOffsetBorder,
                                                     sUI              _numVertsBorder,
                                                     sUI              _glPrimTypeBorder,
                                                     Dsdvg_mat4_ref_t _mvpMatrix,
                                                     sF32    _centerX, sF32 _centerY,
                                                     sF32    _sizeX,   sF32 _sizeY,
                                                     sF32    _radiusX, sF32 _radiusY,
                                                     sF32    _fillR,   sF32 _fillG,   sF32 _fillB,   sF32 _fillA,
                                                     sF32    _strokeR, sF32 _strokeG, sF32 _strokeB, sF32 _strokeA,
                                                     sF32    _strokeW,
                                                     sF32    _aaRange,
                                                     sF32    _aaExp,
                                                     const shadervg_paint_t *_paint
                                                     ) {

   sdvg_BindVBO(_vboId);

   // Outer border
   if(_numVertsBorder > 0u)
   {
      shape_shader.bind();

      Dsdvg_uniform_mat4(shape_u_transform, _mvpMatrix);
      Dsdvg_uniform_2f(shape_u_center,   _centerX, _centerY);
      Dsdvg_uniform_2f(shape_u_size_i,   _sizeX - _strokeW, _sizeY - _strokeW);
      Dsdvg_uniform_2f(shape_u_size_o,   _sizeX + _strokeW, _sizeY + _strokeW);
      Dsdvg_uniform_2f(shape_u_size,     _sizeX, _sizeY);
      Dsdvg_uniform_2f(shape_u_radius,   _radiusX, _radiusY);
      const sF32 radiusIx = _radiusX - _strokeW;
      const sF32 radiusIy = _radiusY - _strokeW;
      const sF32 radiusOx = _radiusX + _strokeW;
      const sF32 radiusOy = _radiusY + _strokeW;
      Dsdvg_uniform_2f(shape_u_radius_i,        radiusIx, radiusIy);
      Dsdvg_uniform_2f(shape_u_radius_o,        radiusOx, radiusOy);
      Dsdvg_uniform_2f(shape_u_ob_radius_i,     1.0f / radiusIx, 1.0f / radiusIy);
      Dsdvg_uniform_2f(shape_u_ob_radius_o,     1.0f / radiusOx, 1.0f / radiusOy);
      Dsdvg_uniform_1f(shape_u_ob_radius_i_max, (radiusIx > radiusIy) ? (1.0f / radiusIx) : (1.0f / radiusIy));
      Dsdvg_uniform_1f(shape_u_ob_radius_o_max, (radiusOx > radiusOy) ? (1.0f / radiusOx) : (1.0f / radiusOy));
      Dsdvg_uniform_1f(shape_u_radius_i_max,    (radiusIx > radiusIy) ? radiusIx : radiusIy);
      Dsdvg_uniform_1f(shape_u_radius_o_max,    (radiusOx > radiusOy) ? radiusOx : radiusOy);
      Dsdvg_uniform_1f(shape_u_aa_range,         _aaRange);

#ifdef SHADERVG_AA_EXP
      if(-1 != shape_u_aa_exp)
      {
         Dsdvg_uniform_1f(shape_u_aa_exp, _aaExp);
      }
#endif // SHADERVG_AA_EXP

      Dsdvg_uniform_4f(shape_u_color_stroke, _strokeR, _strokeG, _strokeB, _strokeA);

#ifdef SHADERVG_DEBUG_FRAG
      if(-1 != shape_u_debug)
      {
         Dsdvg_uniform_1f(shape_u_debug, b_debug ? 1.0f : 0.0f);
      }
#endif // SHADERVG_DEBUG_FRAG

      updatePaintUniforms(_paint);

      Dsdvg_attrib_offset(shape_a_vertex, 2/*size*/, GL_FLOAT, GL_FALSE/*normalize*/, 0/*stride*/, _byteOffsetBorder);

      Dsdvg_attrib_enable(shape_a_vertex);

      Dsdvg_draw_arrays_vbo(_glPrimTypeBorder, 0, _numVertsBorder);

      Dsdvg_attrib_disable(shape_a_vertex);
   }
}

void ShaderVG_Shape::drawRoundRectStrokeAAPaint(Dsdvg_buffer_ref_t _scratchBuf,
                                                Dsdvg_mat4_ref_t _mvpMatrix,
                                                sF32 _centerX, sF32 _centerY,
                                                sF32 _sizeX,   sF32 _sizeY,
                                                sF32 _radiusX, sF32 _radiusY,
                                                sF32 _fillR, sF32 _fillG, sF32 _fillB, sF32 _fillA,
                                                sF32 _strokeR, sF32 _strokeG, sF32 _strokeB, sF32 _strokeA,
                                                sF32 _strokeW,
                                                sF32 _decalAlpha,
                                                sF32 _aaRange,
                                                sF32 _aaExp,
                                                const shadervg_paint_t *_paint
                                                ) {

   sdvg_int_BindScratchBuffer();

   if(_radiusX > _sizeX)
      _radiusX = _sizeX;

   if(_radiusY > _sizeY)
      _radiusY = _sizeY;

   sUI numTris;

   sBool bSingle = ((_sizeX*_sizeY) <= 256);

   // Outer corners
   shape_shader.bind();

   Dsdvg_uniform_mat4(shape_u_transform, _mvpMatrix);
   Dsdvg_uniform_2f(shape_u_center,   _centerX, _centerY);
   Dsdvg_uniform_2f(shape_u_size_i,   _sizeX - _strokeW, _sizeY - _strokeW);
   Dsdvg_uniform_2f(shape_u_size_o,   _sizeX + _strokeW, _sizeY + _strokeW);
   Dsdvg_uniform_2f(shape_u_size,     _sizeX, _sizeY);
   Dsdvg_uniform_2f(shape_u_radius,   _radiusX, _radiusY);
   const sF32 radiusIx = _radiusX - _strokeW;
   const sF32 radiusIy = _radiusY - _strokeW;
   const sF32 radiusOx = _radiusX + _strokeW;
   const sF32 radiusOy = _radiusY + _strokeW;
   Dsdvg_uniform_2f(shape_u_radius_i,        radiusIx, radiusIy);
   Dsdvg_uniform_2f(shape_u_radius_o,        radiusOx, radiusOy);
   Dsdvg_uniform_2f(shape_u_ob_radius_i,     1.0f / radiusIx, 1.0f / radiusIy);
   Dsdvg_uniform_2f(shape_u_ob_radius_o,     1.0f / radiusOx, 1.0f / radiusOy);
   Dsdvg_uniform_1f(shape_u_ob_radius_i_max, (radiusIx > radiusIy) ? (1.0f / radiusIx) : (1.0f / radiusIy));
   Dsdvg_uniform_1f(shape_u_ob_radius_o_max, (radiusOx > radiusOy) ? (1.0f / radiusOx) : (1.0f / radiusOy));
   Dsdvg_uniform_1f(shape_u_radius_i_max,    (radiusIx > radiusIy) ? radiusIx : radiusIy);
   Dsdvg_uniform_1f(shape_u_radius_o_max,    (radiusOx > radiusOy) ? radiusOx : radiusOy);
   Dsdvg_uniform_1f(shape_u_aa_range,        _aaRange);

#ifdef SHADERVG_AA_EXP
   if(-1 != shape_u_aa_exp)
   {
      Dsdvg_uniform_1f(shape_u_aa_exp, _aaExp);
   }
#endif // SHADERVG_AA_EXP

   if(-1 != shape_u_color_fill)
   {
      Dsdvg_uniform_4f(shape_u_color_fill, _fillR, _fillG, _fillB, _fillA);
   }

   Dsdvg_uniform_4f(shape_u_color_stroke, _strokeR, _strokeG, _strokeB, _strokeA);

   if(-1 != shape_u_decal_alpha)
   {
      Dsdvg_uniform_1f(shape_u_decal_alpha, _decalAlpha);
   }

#ifdef SHADERVG_DEBUG_FRAG
   if(-1 != shape_u_debug)
   {
      Dsdvg_uniform_1f(shape_u_debug, b_debug ? 1.0f : 0.0f);
   }
#endif // SHADERVG_DEBUG_FRAG

   updatePaintUniforms(_paint);

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
   else
   {
      numTris = 28u;  // 24..28
      allocScratchBuffer(shape_a_vertex, _scratchBuf, (numTris*3*2/*xy*/*4/*float*/));

      numTris = EmitRoundRectBorderVertices(_scratchBuf,
                                            _centerX, _centerY,
                                            _sizeX,   _sizeY,
                                            _radiusX, _radiusY,
                                            _strokeW,
                                            _aaRange
                                            );

      Dsdvg_draw_triangles(0, numTris * 3u);
   }

   Dsdvg_attrib_disable(shape_a_vertex);
}
