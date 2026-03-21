// ----
// ---- file   : test_min.c
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : Distributed under terms of the MIT license (https://opensource.org/licenses/MIT)
// ----          Copyright 2025-2026 by bsp
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
// ---- info   : "minnie" high-level API test case / benchmark
// ---- note   : C99
// ----
// ----
// ----

/// When defined (in Makefile), auto-exit after <n> frames (benchmark single test)
// #define SHADERVG_AUTO_EXIT_FRAMES  100

//
//          UP : next test
//        DOWN : previous test
//       SPACE : toggle animation
// lctrl-SPACE : toggle slow-motion
//        LEFT : decrement stroke scale
//       RIGHT : increment stroke scale
//           c : toggle draw-copy
//           g : toggle draw-gl
//           l : toggle symmetry lock (**removed**)
//           m : toggle MSAA
//           o : cycle fill/stroke modes
//           p : save screenshot to "screenshots/api/minnie_api_test.png"
//           s : toggle line strips (off=tesselate via CPU)
//           t : toggle texture filter
//           v : toggle vsync
//

#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#if 1
#define DISPLAY_WIDTH  800
#define DISPLAY_HEIGHT 600

#define VP_W 800
#define VP_H 600

#else
#define DISPLAY_WIDTH  454
#define DISPLAY_HEIGHT 454

#define VP_W DISPLAY_WIDTH
#define VP_H DISPLAY_HEIGHT
#endif

#include "../inc_minnie.h"
#include "hal.h"
#include "MinnieVG.h"

// ---------------------------------------------------------------------------- config
static sBool b_draw_gl       = 1;     // 'g'
static sBool b_gl_buf_once   = 0;     // 0=generate VB+DL once   1=generate VB+DL every frame
static sBool b_vsync         = 1;     // 'v'
#if 0
static sBool b_msaa          = 0;     // 1=use MSAA
#endif

static sBool b_anim          = 1;     // SPACE
static sBool b_slomo         = 0;     // lctrl-SPACE
static sUI   fill_mode       = 3u;    // 'o'  bit0:fill bit1:stroke
static sSI   test_idx        = 0;     // UP/DOWN
static sUI   auto_cycle_num_frames =  // >0:auto-cycle tests (any key stroke interrupts this)
#ifdef AUTO_CYCLE_NUM_FRAMES
   AUTO_CYCLE_NUM_FRAMES
#else
   0u
#endif // AUTO_CYCLE_NUM_FRAMES
   ;

static sBool b_sym_radius    = 1;     // 'l'(removed)  (rx=ry)
static sF32  stroke_scale    = 1.0f;  // LEFT/RIGHT
static sBool b_tex_filter    = 1;     // 't'
static sBool b_copy          = 0;     // 'c'  draw shifted copy (mvp translatef)

// ---------------------------------------------------------------------------- vars
#define TEX_SX  498
#define TEX_SY  365
#define BIN2C_DECL
#include "res/escher.c"                    // const unsigned char mem_base_tex_escher_argb32[727080]
#undef BIN2C_DECL

static MinnieDrawable *drawable;

static sUI tex_id = 0u;

static sU32 gradient_colors[5] = {0xFFFF0000u, 0xFFFFFF00u, 0xFF00FF00u, 0xFF00FFffu, 0xFF0000ffu};
static sSI  gradient_starts[5] = {0,           200,         400,         500,         1000};
static sU32 tex_gradient[256];
static sUI tex_gradient_id = 0u;

static sU32 last_ticks = 0u;  // 1000 ticks per second
static sU32 ticks_start = 0u;
static sUI num_frames_rendered = 0u;
static sUI total_num_frames_rendered = 0u;
static sUI num_iter         = 1u;
static sUI auto_exit_frames = 0u;  // auto-exit after <n> frames (benchmark single test)
static sBool b_benchmark    = 0;

static sF32 anim_1;
static sF32 anim_2;
static sF32 anim_3;  // w
static sF32 anim_4;
static sF32 anim_5;
static sF32 anim_6;  // seg

static const char *test_names[] = {
   "00: concave path",
   "01: sub-path",
   "02: textured, flat shaded rectangles",
   "03: textured, gouraud shaded rectangles",
   "04: sine lines",
   "05: round-rect filled+stroked (tesselated)",
   "06: round-rect filled+stroked (shadervg)",
   "07: round-rect stroked (tesselated)",
   "08: round-rect stroked (shadervg)",
   "09: round-rect filled (tesselated)",
   "10: round-rect filled (shadervg)",
   "11: rect filled+stroked (tesselated)",
   "12: rect filled+stroked (shadervg)",
   "13: rect stroked (tesselated)",
   "14: rect stroked (shadervg)",
   "15: rect filled (tesselated)",
   "16: rect filled (shadervg)",
   "17: ellipse filled+stroked (tesselated)",
   "18: ellipse filled+stroked (shadervg)",
   "19: ellipse stroked (tesselated)",
   "20: ellipse stroked (shadervg)",
   "21: ellipse filled (tesselated)",
   "22: ellipse filled (shadervg)",
   "23: arc filled",
   "24: arc stroked",
   "25: arc filled+stroked",
   "26: arc stroke+pattern",
   "27: textured, flat shaded AA rectangles",
   "28: textured, flat shaded AA rectangles 200x200",
};
#define NUM_TESTS  (sizeof(test_names)/sizeof(const char *))

// ---------------------------------------------------------------------------- rand
static sU32 loc_rand = 0u;
static void loc_rand_seed(sU32 _seed) {
   loc_rand = _seed ^ 78163u;
}

static sU32 loc_randu(void) {
   sU32 x = ( (loc_rand >> 16) + 3715436908ul ) * 0x1fd8dae7u;
   loc_rand += x;
   return loc_rand;
}

#if 0
static sS32 loc_rands(void) {
   union {
      sU32 u32;
      sS32 s32;
   } ret;
   ret.u32 = loc_randu();
   return ret.s32;
}
#endif

static sF32 loc_randf(sF32 _max) {
   union {
      sU32 u32;
      sF32 f32;
   } ret;
   ret.u32 = 0x3f800000u | (loc_randu() & ((1u << 23) - 1u));
   ret.f32 = (ret.f32 - 1.0f) * _max;
   return ret.f32;
}

// ----------------------------------------------------------------------------
static void Test_00(void) {
   // concave path

   sSI pid = minBeginPath();
   minSeg(32u);

   const sF32 vpSclX = VP_W / 800.0f;
   const sF32 vpSclY = VP_H / 600.0f;

   sF32 rx = sinf(anim_1) *  40.0f;
   sF32 ry = cosf(anim_2) *  30.0f;
   sF32 sx = sinf(anim_4) * 140.0f;
   sF32 sy = cosf(anim_5) * 130.0f;

   minMoveTo((100.0f+rx)*vpSclX, (300.0f+ry)*vpSclY);

   minCubicTo((300.0f+sx)*vpSclX, (100.0f+sy)*vpSclY,  // c1
              (500.0f-sy)*vpSclX, (100.0f-sx)*vpSclY,  // c2
              (700.0f-ry)*vpSclX, (300.0f-rx)*vpSclY   // dst
              );

   minCubicTo((500.0f+sx)*vpSclX, (300.0f+sy)*vpSclY,  // c1
              (250.0f-sy)*vpSclX, (500.0f-sx)*vpSclY,  // c2
              (100.0f+rx)*vpSclX, (300.0f+ry)*vpSclY   // dst
              );

   minEndPath(YAC_TRUE/*bClosed*/);

   if(fill_mode & 1)
   {
      minFill();
      minColor(0xff324f75u);
      minDrawPath(pid);
   }

   if(fill_mode & 2)
   {
      minStrokeWidth( 2.0f * (sinf(anim_3)*2.0f+3.0f)*vpSclX );
      minColor(0xffffffffu);
      minJoinBevel();
      minDrawPath(pid);
   }
}

// ----------------------------------------------------------------------------
static void Test_01(void) {
   // sub-path

   sSI pid = minBeginPath();

   sF32 segAmt = 0.5f + 0.5f*sinf(anim_6);
   segAmt *= segAmt;
   segAmt *= segAmt;
   minSeg((sUI)(8 + 120*segAmt));

   const sF32 vpSclX = VP_W / 800.0f;
   const sF32 vpSclY = VP_H / 600.0f;

   sF32 rx = sinf(anim_1) *  40.0f;
   sF32 ry = cosf(anim_2) *  30.0f;
   sF32 sx = sinf(anim_4) * 140.0f;
   sF32 sy = cosf(anim_5) * 130.0f;

   // main
   minMoveTo((100.0f+rx)*vpSclX, (300.0f+ry)*vpSclY);

   sF32 relx1 = (300.0f+sx) - (100.0f+rx);
   sF32 rely1 = (100.0f+sy) - (300.0f+ry);
   minCubicTo((300.0f+sx)*vpSclX, (100.0f+sy)*vpSclY,
              (500.0f-sy)*vpSclX, (100.0f-sx)*vpSclY,
              (700.0f-ry)*vpSclX, (300.0f-rx)*vpSclY
              );

   sF32 relx2 = (700.0f-ry) - (500.0f-sy);
   sF32 rely2 = (300.0f-rx) - (100.0f-sx);
   minCubicTo((700.0f-ry+relx2)*vpSclX, (300.0f-rx+rely2)*vpSclY,
              (100.0f+rx-relx1)*vpSclX, (300.0f+ry-rely1)*vpSclY,
              (100.0f+rx)*vpSclX,       (300.0f+ry)*vpSclY
              );

   // sub 1
   minBeginSub();
   minMoveTo((300.0f-ry)*vpSclX, (250.0f+rx)*vpSclY);
   minEllipse(150.0f*vpSclX, 150.0f*vpSclY);
   minEndSubClosed();

   minEndPathClosed();

   if(fill_mode & 1)
   {
      minFill();
      minColor(0xff324f75u);
      minDrawPath(pid);
   }

   if(fill_mode & 2)
   {
      minStrokeWidth( 2.0f * (sinf(anim_3)*2.0f+3.0f) * vpSclX );
      minColor(0xffffffffu);
      minJoinBevel();
      minDrawPath(pid);
   }
}

// ----------------------------------------------------------------------------
static void Test_02(void) {
   // textured, flat shaded rectangles

   minBindTexture(tex_id, YAC_FALSE/*bRepeat*/, b_tex_filter);
   minColor(0xffffffffu);

   const sF32 vpSclX = VP_W / 800.0f;
   const sF32 vpSclY = VP_H / 600.0f;

   sF32 x = sinf(anim_1) * 400.0f - 400.0f + 125.0f/4;
   sF32 y = cosf(anim_2) * 300.0f - 300.0f + 100.0f/4;
   sF32 ty = y;
   sF32 ang5 = anim_5;
   for(sUI iy = 0u; iy < 16u; iy++)
   {
      sF32 tx = x;
      sF32 ang4 = anim_4;
      for(sUI ix = 0u; ix < 16u; ix++)
      {
         sF32 nx = sABS((tx - 400.0f) / 400.0f);
         sF32 ny = sABS((ty - 300.0f) / 300.0f);
         nx = 1.0f - nx;
         ny = 1.0f - ny;
         nx = 2.0f * nx - 1.0f;
         ny = 2.0f * ny - 1.0f;

         ang4 = sM_PI_2f;
         ang5 = 0.0f;
         sF32 w = 125.0f/2 + (sinf(ang4) *  75.0f/2)*nx;
         sF32 h = 100.0f/2 + (cosf(ang5) *  50.0f/2)*ny;

         minRectTexUVFlat( (tx - w*0.5f)*vpSclX, (ty - h*0.5f)*vpSclY, w*vpSclX, h*vpSclY,
                          0.0f, 0.0f,
                          1.0f, 1.0f
                          );
         tx += 200.0f/2;
         ang4 += 0.9323f;
      }
      ty += 150.0f/2;
      ang5 += 0.715f;
   }
}

// ----------------------------------------------------------------------------
static sU32 Test_03_calc_c32_at(sF32 x, sF32 y) {
   sF32 nx = sABS((x - 400.0f) / 400.0f);  // => -1..1
   sF32 ny = sABS((y - 300.0f) / 300.0f);
   nx = sMAX(0.0f, cosf(nx * sM_PI_2f));
   ny = sMAX(0.0f, cosf(ny * sM_PI_2f));
   sF32 a = sRANGE(nx * ny, 0.0f, 1.0f);
   return sdvg_ARGB((sU8)(a*255u), 255u, 255u, 255u);
}

static void Test_03(void) {
   // textured, gouraud shaded rectangles

   minBindTexture(tex_id, YAC_FALSE/*bRepeat*/, b_tex_filter);
   minColor(0xffffffffu);

   const sF32 vpSclX = VP_W / 800.0f;
   const sF32 vpSclY = VP_H / 600.0f;

   sF32 x = sinf(anim_1) * 400.0f - 400.0f + 125.0f/4;
   sF32 y = cosf(anim_2) * 300.0f - 300.0f + 100.0f/4;
   sF32 ty = y;
   sF32 ang5 = anim_5;
   for(sUI iy = 0u; iy < 16u; iy++)
   {
      sF32 tx = x;
      sF32 ang4 = anim_4;
      for(sUI ix = 0u; ix < 16u; ix++)
      {
         sF32 nx = sABS((tx - 400.0f) / 400.0f);
         sF32 ny = sABS((ty - 300.0f) / 300.0f);
         nx = 1.0f - nx;
         ny = 1.0f - ny;
         nx = 2.0f * nx - 1.0f;
         ny = 2.0f * ny - 1.0f;

         ang4 = sM_PI_2f;
         ang5 = 0.0f;
         sF32 w = 125.0f/2 + (sinf(ang4) *  75.0f/2)*nx;
         sF32 h = 100.0f/2 + (cosf(ang5) *  50.0f/2)*ny;

         minRectTexUVGouraud((tx - w*0.5f)*vpSclX, (ty - h*0.5f)*vpSclY, w*vpSclX, h*vpSclY,
                             0.0f, 0.0f,
                             1.0f, 1.0f,
                             Test_03_calc_c32_at(tx - w*0.5f, ty - h*0.5f),  // lt
                             Test_03_calc_c32_at(tx + w*0.5f, ty - h*0.5f),  // rt
                             Test_03_calc_c32_at(tx + w*0.5f, ty + h*0.5f),  // rb
                             Test_03_calc_c32_at(tx - w*0.5f, ty + h*0.5f)   // lb
                             );
         tx += 200.0f/2;
         ang4 += 0.9323f;
      }
      ty += 150.0f/2;
      ang5 += 0.715f;
   }
}

// ----------------------------------------------------------------------------
static void Test_04(void) {
   // sine lines

   const sF32 vpSclX = VP_W / 800.0f;
   const sF32 vpSclY = VP_H / 600.0f;

   sSI pid = minBeginPath();

   sUI numSeg = 64u;
   sUI numPoints = numSeg + 1u;
   sF32 w = (sM_2PIf / numSeg);
   sF32 a = anim_1;
   sF32 x = 100.0f;
   sF32 xStep = 600.0f / numSeg;
   for(sUI segIdx = 0u; segIdx < numPoints; segIdx++)
   {
      sF32 y = sinf(a) * 150.0f + 300.0f;
      if(0u == segIdx)
         minMoveTo(x*vpSclX, y*vpSclY);
      else
         minLineTo(x*vpSclX, y*vpSclY);
      a += w;
      x += xStep;
   }

   minEndPathOpen();

   minColor(0xffffffffu);
   minStrokeWidth( 2.0f * 4.0f*vpSclX);
   minJoinBevel();
   minCapNone();
   minDrawPath(pid);
}

// ----------------------------------------------------------------------------
static void Test_05(sBool _bFill, sBool _bStroke) {
   // round-rect filled+stroked (tesselated)

   const sF32 vpSclX = VP_W / 800.0f;
   const sF32 vpSclY = VP_H / 600.0f;

   sF32 rx = 4.0f+40.0f + sinf(anim_1) * 40.0f;
   sF32 ry = 4.0f+30.0f + cosf(anim_2) * 30.0f;
   sF32 sx = 320.0f + sinf(anim_4*0.25f) * 200.0f;
   sF32 sy = 240.0f + cosf(anim_5*0.25f) * 150.0f;

   if(b_sym_radius)
   {
      rx = sMIN(rx, ry);
      ry = rx;
   }

   if(rx > (sx*0.495f))
      rx = (sx*0.495f);
   if(ry > (sy*0.495f))
      ry = (sy*0.495f);

   sSI pid = minBeginPath();
   minSeg(8u);
   minMoveTo((400.0f-sx*0.5f)*vpSclX, (300.0f-sy*0.5f)*vpSclY);
   minRoundRect(sx*vpSclX, sy*vpSclY, rx*vpSclX, ry*vpSclY);
   minEndPath(YAC_TRUE/*bClosed*/);

   if(_bFill)
   {
      minColor(0xff324f75u);
      minFill();
      minDrawPath(pid);
   }

   if(_bStroke)
   {
      minColor(0xffffffffu);
      minStrokeWidth( 2.0f * (sinf(anim_3)*2.0f+3.0f)*vpSclX );
      minJoinMiter();
      minDrawPath(pid);
   }
}

// ----------------------------------------------------------------------------
static void Test_06(sBool _bFill, sBool _bStroke) {  // 10
   // round-rect filled+stroked (shadervg)

   const sF32 vpSclX = VP_W / 800.0f;
   const sF32 vpSclY = VP_H / 600.0f;

   sF32 rx = 4.0f+40.0f + sinf(anim_1) * 40.0f;
   sF32 ry = 4.0f+30.0f + cosf(anim_2) * 30.0f;
   sF32 sx = 320.0f + sinf(anim_4*0.25f) * 200.0f;
   sF32 sy = 240.0f + cosf(anim_5*0.25f) * 150.0f;

   if(b_sym_radius)
   {
      rx = sMIN(rx, ry);
      ry = rx;
   }

   if(rx > (sx*0.495f))
      rx = (sx*0.495f);
   if(ry > (sy*0.495f))
      ry = (sy*0.495f);

   minBeginImmediate();
   minMoveTo((400.0f-sx*0.5f)*vpSclX, (300.0f-sy*0.5f)*vpSclY);

   if(_bFill)
   {
      minColor(0xff324f75u);
      minFill();
      minRoundRect(sx*vpSclX, sy*vpSclY, rx*vpSclX, ry*vpSclY);
   }

   if(_bStroke)
   {
      minColor(0xffffffffu);
      minStrokeWidth( 2.0f * (sinf(anim_3)*2.0f+3.0f)*vpSclX );
      minRoundRect(sx*vpSclX, sy*vpSclY, rx*vpSclX, ry*vpSclY);
   }

   minEndImmediate();
}

// ----------------------------------------------------------------------------
static void Test_11(sBool _bFill, sBool _bStroke) {
   // rect filled+stroked (tesselated)

   const sF32 vpSclX = VP_W / 800.0f;
   const sF32 vpSclY = VP_H / 600.0f;

   sF32 rx = 4.0f+40.0f + sinf(anim_1) * 40.0f;
   sF32 ry = 4.0f+30.0f + cosf(anim_2) * 30.0f;
   sF32 sx = 320.0f + sinf(anim_4*0.25f) * 200.0f;
   sF32 sy = 240.0f + cosf(anim_5*0.25f) * 150.0f;

   if(b_sym_radius)
   {
      rx = sMIN(rx, ry);
      ry = rx;
   }

   if(rx > (sx*0.495f))
      rx = (sx*0.495f);
   if(ry > (sy*0.495f))
      ry = (sy*0.495f);

   sSI pid = minBeginPath();
   minSeg(8u);
   minMoveTo( (400.0f-sx*0.5f)*vpSclX, (300.0f-sy*0.5f)*vpSclY );
   minRect(sx*vpSclX, sy*vpSclY);
   minEndPath(YAC_TRUE/*bClosed*/);

   if(_bFill)
   {
      minColor(0xff324f75u);
      minFill();
      minDrawPath(pid);
   }

   if(_bStroke)
   {
      minColor(0xffffffffu);
      minStrokeWidth( 2.0f * (sinf(anim_3)*2.0f+3.0f)*vpSclX );
      minJoinMiter();
      minDrawPath(pid);
   }
}

// ----------------------------------------------------------------------------
static void Test_12(sBool _bFill, sBool _bStroke) {
   // rect filled+stroked (shadervg)

   const sF32 vpSclX = VP_W / 800.0f;
   const sF32 vpSclY = VP_H / 600.0f;

   sF32 rx = 4.0f+40.0f + sinf(anim_1) * 40.0f;
   sF32 ry = 4.0f+30.0f + cosf(anim_2) * 30.0f;
   sF32 sx = 320.0f + sinf(anim_4*0.25f) * 200.0f;
   sF32 sy = 240.0f + cosf(anim_5*0.25f) * 150.0f;

   if(b_sym_radius)
   {
      rx = sMIN(rx, ry);
      ry = rx;
   }

   if(rx > (sx*0.495f))
      rx = (sx*0.495f);
   if(ry > (sy*0.495f))
      ry = (sy*0.495f);

   minBeginImmediate();
   minMoveTo((400.0f-sx*0.5f)*vpSclX, (300.0f-sy*0.5f)*vpSclY);

   if(_bFill)
   {
      minColor(0xff324f75u);
      minFill();
      minRect(sx*vpSclX, sy*vpSclY);
   }

   if(_bStroke)
   {
      minColor(0xffffffffu);
      minStrokeWidth( 2.0f * (sinf(anim_3)*2.0f+3.0f)*vpSclX );
      minRect(sx*vpSclX, sy*vpSclY);
   }

   minEndImmediate();
}

// ----------------------------------------------------------------------------
static void Test_17(sBool _bFill, sBool _bStroke) {
   // ellipse filled+stroked (tesselated)

   const sF32 vpSclX = VP_W / 800.0f;
   const sF32 vpSclY = VP_H / 600.0f;

   sF32 rx = 4.0f+40.0f + sinf(anim_1) * 40.0f;
   sF32 ry = 4.0f+30.0f + cosf(anim_2) * 30.0f;
   sF32 sx = 320.0f + sinf(anim_4*0.25f) * 200.0f;
   sF32 sy = 240.0f + cosf(anim_5*0.25f) * 150.0f;

   if(b_sym_radius)
   {
      rx = sMIN(rx, ry);
      ry = rx;

      sx = sMIN(sx, sy);
      sy = sx;
   }

   if(rx > (sx*0.495f))
      rx = (sx*0.495f);
   if(ry > (sy*0.495f))
      ry = (sy*0.495f);

   sSI pid = minBeginPath();
   minSeg(64u);
   minMoveTo(400.0f*vpSclX, 300.0f*vpSclY);
   minEllipse(sx*0.5f*vpSclX, sy*0.5f*vpSclY);
   minEndPath(YAC_TRUE/*bClosed*/);

   if(_bFill)
   {
      minColor(0xff324f75u);
      minFill();
      minDrawPath(pid);
   }

   if(_bStroke)
   {
      minColor(0xffffffffu);
      minStrokeWidth( 2.0f * (sinf(anim_3)*2.0f+3.0f)*vpSclX );
      minJoinBevel();
      minDrawPath(pid);
   }
}

// ----------------------------------------------------------------------------
static void Test_18(sBool _bFill, sBool _bStroke) {
   // ellipse filled+stroked (shadervg)

   const sF32 vpSclX = VP_W / 800.0f;
   const sF32 vpSclY = VP_H / 600.0f;

   sF32 rx = 4.0f+40.0f + sinf(anim_1) * 40.0f;
   sF32 ry = 4.0f+30.0f + cosf(anim_2) * 30.0f;
   sF32 sx = 320.0f + sinf(anim_4*0.25f) * 200.0f;
   sF32 sy = 240.0f + cosf(anim_5*0.25f) * 150.0f;

   if(b_sym_radius)
   {
      rx = sMIN(rx, ry);
      ry = rx;

      sx = sMIN(sx, sy);
      sy = sx;
   }

   if(rx > (sx*0.495f))
      rx = (sx*0.495f);
   if(ry > (sy*0.495f))
      ry = (sy*0.495f);

   minBeginImmediate();
   minMoveTo(400.0f*vpSclX, 300.0f*vpSclY);

   if(_bFill)
   {
      minColor(0xff324f75u);
      minFill();
      minEllipse(sx*0.5f*vpSclX, sy*0.5f*vpSclY);
   }

   if(_bStroke)
   {
      minColor(0xffffffffu);
      minStrokeWidth( 2.0f * (sinf(anim_3)*2.0f+3.0f)*vpSclX );
      minEllipse(sx*0.5f*vpSclX, sy*0.5f*vpSclY);
   }

   minEndImmediate();
}

// ----------------------------------------------------------------------------
static void Test_23(sBool _bFill, sBool _bStroke) {
   // arc path

   sSI pid = minBeginPath();
   minSeg(32u);

   const sF32 vpSclX = VP_W / 800.0f;
   const sF32 vpSclY = VP_H / 600.0f;

   sF32 rx = sinf(anim_1) * 40.0f + 41.0f;
   sF32 ry = cosf(anim_2) * 30.0f + 31.0f;
   sF32 sx = sinf(anim_4) * 240.0f;
   sF32 sy = cosf(anim_5) * 230.0f;

   sF32 px = 800.0f/2;
   sF32 py = 400.0f/2;

   minMoveTo(px*vpSclX, py*vpSclY);

   sF32 rot = 0.0f;

   minArcTo(rx*vpSclX, ry*vpSclY,
            rot,
            YAC_FALSE/*bLargeArc*/,
            YAC_FALSE/*bArcSweep*/,
            (px + sx)*vpSclX, (py + sy)*vpSclY
            );

   minEndPath(YAC_TRUE/*bClosed*/);

   if(_bFill)
   {
      minFill();
      minColor(0xff324f75u);  // green=6bd2b7
      minDrawPath(pid);
   }

   if(_bStroke)
   {
      minStrokeWidth( 2.0f * 2.0f*vpSclX);
      minColor(0xffffffffu);
      minJoinBevel();
      minDrawPath(pid);
   }
}

// ----------------------------------------------------------------------------
static void Test_26(void) {
   // arc stroke+pattern (WIP)

   const sF32 vpSclX = VP_W / 454.0f;
   const sF32 vpSclY = VP_H / 454.0f;

   sSI paintId = minPaintCreate();
   minPaintPattern(0.0f, 0.0f,
                   1.0f, 0.0f,
                   VP_W, VP_H
                   );

   sSI pid = minBeginPath();
   minSeg(20);

   const sF32 px =  71;
   const sF32 py =  71;
   const sF32 rx = 229;
   const sF32 ry = 244;
   const sF32 dx = 385;
   const sF32 dy =  71;

   minMoveTo(px*vpSclX, py*vpSclY);

   float rot = 0;

   minArcTo(rx*vpSclX, ry*vpSclY,
            rot,
            YAC_FALSE/*bLargeArc*/,
            YAC_TRUE/*bArcSweep*/,
            dx*vpSclX, dy*vpSclY
            );

   minEndPath(YAC_FALSE/*bClosed*/);

   minStrokeWidth( 2.0f * 3.5f*vpSclX);
   minBindTexture(tex_gradient_id, YAC_TRUE/*bRepeat*/, b_tex_filter);
   minPaint(paintId);
   minColor(0xffffffffu);
   minJoinBevel();
   minCapNone();
   minDrawPath(pid);
}

// ----------------------------------------------------------------------------
static void Test_27(void) {
   // textured, flat shaded AA rectangles

   minBindTexture(tex_id, 0/*bRepeat*/, b_tex_filter);
   sSI paintId = minPaintCreate();
   minColor(0xffffffffu);
   minFill();

   float aaBorder = 1.5f;
   minAARange(aaBorder);
   sF32 numPix = 0.0f;

   minBeginImmediate();

   const sF32 vpSclX = VP_W / 800.0f;
   const sF32 vpSclY = VP_H / 600.0f;

   sF32 x = sinf(anim_1) * 400.0f - 400.0f + 125.0f/4;
   sF32 y = cosf(anim_2) * 300.0f - 300.0f + 100.0f/4;
   sF32 ty = y;
   sF32 ang5 = anim_5;
   for(sUI iy = 0u; iy < 16u; iy++)
   {
      sF32 tx = x;
      sF32 ang4 = anim_4;
      for(sUI ix = 0u; ix < 16u; ix++)
      {
         sF32 nx = sABS((tx - 400.0f) / 400.0f);
         sF32 ny = sABS((ty - 300.0f) / 300.0f);
         nx = 1.0f - nx;
         ny = 1.0f - ny;
         nx = 2.0f * nx - 1.0f;
         ny = 2.0f * ny - 1.0f;

         ang4 = sM_PI_2f;
         ang5 = 0.0f;
         sF32 w = 125.0f/2 + (sinf(ang4) *  75.0f/2)*nx;
         sF32 h = 100.0f/2 + (cosf(ang5) *  50.0f/2)*ny;
         if(w > 0.0f && h > 0.0f)
         {
            sF32 wh = w * 0.5f;
            sF32 hh = h * 0.5f;
            sF32 px = (tx - wh)*vpSclX;
            sF32 py = (ty - hh)*vpSclY;

            minPaintUpdate(paintId);
            minPaintPattern(px, py,
                            1.0f, 0.0f,
                            w, h
                            );
            minPaint(paintId);

            minMoveTo(px+aaBorder, py+aaBorder);
            sF32 sw = (w*vpSclX);
            sF32 sh = (h*vpSclY);
            numPix += sw * sh;
            minRect(sw-aaBorder*2.0f, sh-aaBorder*2.0f);
         }

         tx += 200.0f/2;
         ang4 += 0.9323f;
      }
      ty += 150.0f/2;
      ang5 += 0.715f;
   }

   minEndImmediate();

   if(0)
   {
      Dprintf("[trc] test_27: mpix=%3.2f\n", (numPix/1000000.0f));  // => 0.23 (first frame / b_anim=0)
   }
}

// ----------------------------------------------------------------------------
static void Test_28(void) {
   // textured, flat shaded AA rectangles 200x200

   minBindTexture(tex_id, 0/*bRepeat*/, b_tex_filter);
   sSI paintId = minPaintCreate();
   minColor(0xffffffffu);
   minFill();

   float aaBorder = 1.5f;
   minAARange(aaBorder);
   sF32 numPix = 0.0f;

   minBeginImmediate();

   loc_rand_seed(0x9123db1au);
   sF32 tx = sinf(anim_1) * 4.0f + 2.0f;
   sF32 ty = cosf(anim_2) * 4.0f + 2.0f;
   const sF32 w = 200.0f;
   const sF32 h = 200.0f;
   for(sUI i = 0u; i < 128u; i++)
   {
      sF32 px = loc_randf(VP_W - w - aaBorder) + tx;
      sF32 py = loc_randf(VP_H - h - aaBorder) + ty;

      minPaintUpdate(paintId);
      minPaintPattern(px, py,
                      1.0f, 0.0f,
                      w, h
                      );
      minPaint(paintId);

      minMoveTo(px, py);
      numPix += (w+aaBorder*2.0f) * (h+aaBorder*2.0f);
      minRect(w, h);
   }

   minEndImmediate();

   if(0)
   {
      Dprintf("[trc] test_28: mpix=%3.2f\n", (numPix/1000000.0f));  // => 5.27
   }
}

// ---------------------------------------------------------------------------- SelectTest
static void SelectTest(sSI _idx) {
   test_idx = _idx;
   Dprintf("[...] SelectTest: %d \"%s\"\n", test_idx, test_names[test_idx]);
   hal_window_set_title(test_names[test_idx]);
   num_frames_rendered = 0u;
}

// ---------------------------------------------------------------------------- hal_on_draw
void hal_on_draw(void) {
   // trace "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ onDraw";

   sU32 ticks = hal_get_ticks();
   // Dprintf("xxx ticks=%u last_ticks=%u\n", ticks, last_ticks);
   sF32 dt;
   if(0u != last_ticks)
      dt = (ticks - last_ticks) / (1000.0f / 60.0f);
   else
      dt = 1.0f;
   last_ticks = ticks;
   // Dprintf("xxx hal_on_draw dt=%f\n", dt);
   (void)dt;

   MinnieVG_BeginFrame();

   // sdvg_ClearARGB(0x00ffffffu);
   // sdvg_ClearARGB(0x00e1cdb7u);
   sdvg_ClearARGB(minDrawableGetBackgroundColor(drawable));
   sdvg_ClearDepth(1.0f);
   sdvg_ClearStencil(0);

   if(!b_gl_buf_once || !minDrawableIsComplete(drawable))
   {
      minSetStrokeScale(stroke_scale);

      minDrawableBegin(drawable);

      switch(test_idx)
      {
         case  0: Test_00(); break;  // concave path
         case  1: Test_01(); break;  // sub-path
         case  2: Test_02(); break;  // textured, flat shaded rectangles
         case  3: Test_03(); break;  // textured, gouraud shaded rectangles
         case  4: Test_04(); break;  // sine lines
         case  5: Test_05(1/*bFill*/, 1/*bStroke*/); break;  // round-rect filled+stroked (tesselated)
         case  6: Test_06(1/*bFill*/, 1/*bStroke*/); break;  // round-rect filled+stroked (shadervg)
         case  7: Test_05(0/*bFill*/, 1/*bStroke*/); break;  // round-rect stroked (tesselated)
         case  8: Test_06(0/*bFill*/, 1/*bStroke*/); break;  // round-rect stroked (shadervg)
         case  9: Test_05(1/*bFill*/, 0/*bStroke*/); break;  // round-rect filled (tesselated)
         case 10: Test_06(1/*bFill*/, 0/*bStroke*/); break;  // round-rect filled (shadervg)
         case 11: Test_11(1/*bFill*/, 1/*bStroke*/); break;  // rect filled+stroked (tesselated)
         case 12: Test_12(1/*bFill*/, 1/*bStroke*/); break;  // rect filled+stroked (shadervg)
         case 13: Test_11(0/*bFill*/, 1/*bStroke*/); break;  // rect stroked (tesselated)
         case 14: Test_12(0/*bFill*/, 1/*bStroke*/); break;  // rect stroked (shadervg)
         case 15: Test_11(1/*bFill*/, 0/*bStroke*/); break;  // rect filled (tesselated)
         case 16: Test_12(1/*bFill*/, 0/*bStroke*/); break;  // rect filled (shadervg)
         case 17: Test_17(1/*bFill*/, 1/*bStroke*/); break;  // ellipse filled+stroked (tesselated)
         case 18: Test_18(1/*bFill*/, 1/*bStroke*/); break;  // ellipse filled+stroked (shadervg)
         case 19: Test_17(0/*bFill*/, 1/*bStroke*/); break;  // ellipse stroked (tesselated)
         case 20: Test_18(0/*bFill*/, 1/*bStroke*/); break;  // ellipse stroked (shadervg)
         case 21: Test_17(1/*bFill*/, 0/*bStroke*/); break;  // ellipse filled (tesselated)
         case 22: Test_18(1/*bFill*/, 0/*bStroke*/); break;  // ellipse filled (shadervg)
         case 23: Test_23(YAC_TRUE/*bFill*/, YAC_FALSE/*bStroke*/); break;  // arc filled
         case 24: Test_23(YAC_FALSE/*bFill*/, YAC_TRUE/*bStroke*/); break;  // arc stroked
         case 25: Test_23(YAC_TRUE/*bFill*/, YAC_TRUE/*bStroke*/); break;   // arc filled+stroked
         case 26: Test_26(); break;                          // arc stroke+pattern (WIP)
         case 27: Test_27(); break;                          // textured, flat shaded AA rectangles
         case 28: Test_28(); break;                          // textured, flat shaded AA rectangles 2
      }

      minDrawableEnd(drawable);

      if(!b_benchmark && 0u == (num_frames_rendered & 255u))
      {
         MinnieVG_DebugPrintMinnieAndDrawableStats(drawable);
      }

      minDrawableSetSize2f(drawable, VP_W, VP_H);
   }

   if(b_draw_gl)
   {
      sdvg_ProjInit2D(minDrawableGetSizeX(drawable),
                      minDrawableGetSizeY(drawable)
                      );
      minDrawableSetScale2f(drawable, 1.0f, 1.0f);
      minDrawableSetRotation(drawable, 0.0f);
      minDrawableSetTranslate2f(drawable, 0.0f, 0.0f);
      MinnieVG_SetTransformForDrawable(drawable);

      sdvg_SetGlobalAlpha(1.0f);

      for(sUI iter = 0u; iter < num_iter; iter++)
      {
         minDrawableSetEnableDebug(drawable, (0u == iter) && (0u == (num_frames_rendered & 255u)));
         minDrawableDraw(drawable);
      }

      if(0u == (num_frames_rendered & 255u))
      {
         MinnieVG_DebugPrintMinnieAndDrawableStats(drawable);
      }

      if(b_copy)
      {
         Matrix4f *mvp = sdvg_GetTransformRef();
         minnie_matrix4f_translatef(mvp, 6.0f, 6.0f, 0.0f);
         sdvg_SetGlobalAlpha(0.33f);
         minDrawableDraw(drawable);
         // minnie_matrix4f_translatef(mvp, -6.0f, -6.0f, 0.0f);
      }
   }

   MinnieVG_EndFrame();

   // // if(b_glcore)
      glBindVertexArray(0);

#ifdef SHADERVG_NO_DISPLAY
   glFlush();
#else
   hal_window_swap();
#endif // SHADERVG_NO_DISPLAY

   num_frames_rendered++;
   total_num_frames_rendered++;

   if(auto_exit_frames > 0u && auto_exit_frames == num_frames_rendered)
   {
      MinnieVG_DebugPrintMinnieAndDrawableStats(drawable);
      glFinish();
      sU32 tDelta = hal_get_ticks() - ticks_start;
      sF32 fps = (sF32)( (1000.0 * auto_exit_frames * num_iter) / tDelta );
      Dprintf("[...] auto_exit after %u frames / %u millisec => %3.2f fps\n", num_frames_rendered, tDelta, ((sSI)(fps*100))/100.0f);
      hal_window_quit();
   }
   else if(0u == (num_frames_rendered & 63u))
   {
      sUI tDelta = hal_get_ticks() - ticks_start;
      if(tDelta > 0u)
      {
         sF32 fps = (sF32)((1000.0 * total_num_frames_rendered * num_iter) / tDelta);
         Dprintf("[...] FPS=%f\n", fps);
      }
   }

   if(b_anim)
   {
      sF32 spd = b_slomo ? 0.1f : 0.7f;
      anim_1 = sWRAP(anim_1 + spd * 0.23f*0.1f,   0.0f, sM_2PIf*4.0f);
      anim_2 = sWRAP(anim_2 + spd * 0.23f*0.134f, 0.0f, sM_2PIf*4.0f);
      anim_3 = sWRAP(anim_3 + spd * 0.3f*0.173f,  0.0f, sM_2PIf*4.0f);
      anim_4 = sWRAP(anim_4 + spd * 0.3f*0.123f,  0.0f, sM_2PIf*4.0f);
      anim_5 = sWRAP(anim_5 + spd * 0.3f*0.112f,  0.0f, sM_2PIf*4.0f);
      anim_6 = sWRAP(anim_6 + spd * 0.1f*0.1f,    0.0f, sM_2PIf*4.0f);
   }

   if(auto_cycle_num_frames > 0u)
   {
      if(0u == (num_frames_rendered % auto_cycle_num_frames))
      {
         SelectTest(sWRAP(test_idx + 1, 0, NUM_TESTS));
      }
   }
}

// ---------------------------------------------------------------------------- hal_on_key_down
void hal_on_key_down(sU32 _code, sU32 _mod) {
   // Dprintf("[>>>] hal_on_key_down: code=0x%08x  mod=0x%08x\n", _code, _mod);
   if('v' != _code)
      auto_cycle_num_frames = 0u;

   switch(_code)
   {
      default:
         if(MinnieVG_HandleDebugKey(_code, _mod))  // 'a', 'd', 'e', 'h', 'i', 'l', 's', 'u', '1'..'6', ..
         {
            minDrawableReset(drawable);
            num_frames_rendered = 0u;
            return;
         }
         break;

      case VKEY_ESCAPE:
         Dprintf("[dbg] ESCAPE, exiting..\n");
         hal_window_quit();
         break;

      case VKEY_SPACE:
         if(_mod)
         {
            b_slomo = !b_slomo;
            Dprintf("[...] b_slomo is %d\n", b_slomo);
         }
         else
         {
            b_anim = !b_anim;
            Dprintf("[...] b_anim is %d\n", b_anim);
         }
         break;

      case VKEY_UP:
         SelectTest(sWRAP(test_idx + 1, 0, NUM_TESTS));
         break;

      case VKEY_DOWN:
         SelectTest(sWRAP(test_idx - 1, 0, NUM_TESTS));
         break;

      case VKEY_RIGHT:
         stroke_scale = sMIN(stroke_scale + 0.0625f, 16.0f);
         Dprintf("[...] stroke_scale is %f\n", stroke_scale);
         break;

      case VKEY_LEFT:
         stroke_scale = sMAX(stroke_scale - 0.0625f, 0.0625f);
         Dprintf("[...] stroke_scale is %f\n", stroke_scale);
         break;

      case 'c':
         b_copy = !b_copy;
         Dprintf("[...] b_copy is %d\n", b_copy);
         break;

      case 'g':
         b_draw_gl = !b_draw_gl;
         Dprintf("[...] b_draw_gl is %d\n", b_draw_gl);
         break;

      // // case 'l':
      // //    b_sym_radius = !b_sym_radius;
      // //    Dprintf("[...] b_sym_radius is %d\n", b_sym_radius);
      // //    break;

#if 0
      case 'm':
         b_msaa = !b_msaa;
         Dprintf("[...] b_msaa is %d\n", b_msaa);
         b_reopen = YAC_TRUE;
         SDL.exitEventLoop();
         break;
#endif

      case 'o':
         fill_mode = sWRAP(fill_mode +1u, 1u, 3u+1u);
         num_frames_rendered = 0u;
         Dprintf("[...] fill_mode is %u\n", fill_mode);
         break;

#if 0
      case 'p':
         b_queued_screenshot = YAC_TRUE;
         trace "[...] queue screenshot";
         break;
#endif

      case 't':
         b_tex_filter = !b_tex_filter;
         Dprintf("[...] b_tex_filter is %d\n", b_tex_filter);
         break;

      case 'v':
         b_vsync = !b_vsync;
         Dprintf("[...] b_vsync is %d\n", b_vsync);
         hal_set_swap_interval(b_vsync);
         num_frames_rendered = 0u;
         total_num_frames_rendered = 0u;
         ticks_start = hal_get_ticks();
         break;
   }

}

// ---------------------------------------------------------------------------- main
int main(int argc, char**argv) {
   Dprintf("main: ENTER\n");

   if(argc >= 2)
   {
      test_idx = atoi(argv[1]);
      if(test_idx >= 0 && test_idx < NUM_TESTS)
      {
      }
      else
      {
         Dprintf("[---] invalid test_idx=%d\n", test_idx);
         return 10;
      }

      if(argc >= 3)
      {
         auto_exit_frames = (sUI)atoi(argv[2]);

         if(argc >= 4)
         {
            num_iter = (sUI)atoi(argv[3]);
            b_benchmark = (num_iter > 1);
            b_gl_buf_once = b_benchmark;
         }
      }
   }
   else
   {
      test_idx = 0;
   }

   if(hal_window_init(DISPLAY_WIDTH, DISPLAY_HEIGHT))
   {
      sdvg_SetScratchBufferSize(64*1024);
#if 0
      sdvg_SetGLSLVersion(1/*b_glcore*//*bV3*/, YAC_FALSE/*bGLES*/, NULL/*sVersionStringOrNull*/);
#endif // 0
      sdvg_Init(1/*b_glcore*/);
      sdvg_SetFramebufferSize(DISPLAY_WIDTH, DISPLAY_HEIGHT);
      sdvg_SetViewport(0, 0, VP_W, VP_H);
      sdvg_SetStrokeRadiusAAOffset(1.5f);

      MinnieVG_Init();
      MinnieVG_InitScratchBuffers();

      Dprintf("[...] init OK, initializing textures..\n");
      tex_id = sdvg_CreateTexture2D(SDVG_TEXFMT_ARGB32, TEX_SX, TEX_SY,
                                    (const void*)mem_base_tex_escher_argb32,
                                    mem_size_tex_escher_argb32
                                    );

      sdvg_GradientToTexture(tex_gradient, 256u,
                             gradient_colors, 5u,
                             gradient_starts, 5u,
                             YAC_TRUE/*bSmoothStep*/
                             );
      tex_gradient_id = sdvg_CreateTexture2D(SDVG_TEXFMT_ARGB32, 256u, 1u, (const void*)tex_gradient, 256*sizeof(sU32));

      Dprintf("[...] initializing shaders and VBOs..\n");
      {
         sUI t = hal_get_ticks();
         sdvg_OnOpen();
         t = hal_get_ticks() - t;
         Dprintf("[...] sdvg_onOpen took %u ms\n", t);
      }

      drawable = minDrawableNew();
      minDrawableInit(drawable);
      if(!minDrawableAlloc(drawable,
                           6*1024*1024/*maxGLBufSize*/,
                           32*1024/*maxDrawBufSize*/
                           )
         )
      {
         Derrorprintf("[---] failed to allocate drawable\n");
         exit(10);
      }

      SelectTest(test_idx);

      // Main loop
      Dprintf("[...] entering event loop\n");

      hal_set_swap_interval(b_vsync ? 1 : 0);

      ticks_start = hal_get_ticks();
      hal_window_loop();

      // Exit
      Dprintf("[dbg] exiting..\n");

      minDrawableFree(drawable);
      minDrawableDelete(drawable);

      MinnieVG_Exit();

      hal_window_exit();
   }

   Dprintf("cu\n");

   return 0;
}
