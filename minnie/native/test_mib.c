// ----
// ---- file   : test_mib.c
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
// ---- info   : "minnie" binary (.mib) loader + OpenGL viewer
// ----
// ----
// ----

/// When defined (in Makefile), auto-exit after <n> frames (benchmark single test)
// #define SHADERVG_AUTO_EXIT_FRAMES  100

//    UP : select next test case
//  DOWN : select previous test case
//     e : toggle edgeAA
//     g : toggle OpenGL rendering
//     h : toggle force-concave-to-holes (SGI tesselator)
//     l : toggle FPS limiter (60 / unlimited)
//     m : toggle MSAA
//     o : toggle Minnie per-frame .mib stream processing
//     p : save screenshot to "screenshots/minnievg/minnie_<testnr>.png"
//     s : toggle line strips
//     u : toggle uniform colors
// SPACE : toggle rotation

#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#define DISPLAY_WIDTH   800
#define DISPLAY_HEIGHT  600

#define VP_W  800
#define VP_H  600

#if 1
#define NUM_ITER 1
#else
// benchmark
#define NUM_ITER 10000
#endif

#include "../inc_minnie.h"
#include "hal.h"
#include "MinnieVG.h"


// ---------------------------------------------------------------------------- config
static sBool b_draw_gl     = 1;     // 'g'
static sBool b_gl_buf_once = 1;     // 'o'  (0=update Drawable each frame)
static sBool b_vsync       = 1;     // 'v'
static sBool b_rotate      = 1;     // SPACE  (RETURN resets rot_angle)
static sF32  stroke_scale  = 1.0f;  // LEFT/RIGHT
static sBool b_line_strip  = 1;     // 's'  (0=tesselate via CPU)
static sBool b_benchmark   = (NUM_ITER > 1);


// ---------------------------------------------------------------------------- tests
#ifndef MIB_PATH
#define MIB_PATH  "../../../tools/mib/mib/"
#endif

static const char *tests[] = {
   MIB_PATH "test000_rect.mib",
   MIB_PATH "test001_lines.mib",
   MIB_PATH "test002_fill.mib",
   MIB_PATH "test003_concave.mib",
   MIB_PATH "test004_fill.mib",
   MIB_PATH "test005_concave.mib",
   MIB_PATH "test006_cubic.mib",
   MIB_PATH "test007_svg.mib",
   MIB_PATH "test008_ellipse.mib",
   MIB_PATH "test009_cake.mib",
   MIB_PATH "test010_d2d.mib",
   MIB_PATH "test011_cursor.mib",
   MIB_PATH "test012_stars.mib",
   MIB_PATH "test013_reverse.mib",
   MIB_PATH "test014_mask.mib",
   MIB_PATH "test015_minnie.mib",
   MIB_PATH "test016_aa.mib",
   MIB_PATH "test017_minnie_aa.mib",
   MIB_PATH "test018_cake_aa.mib",
   MIB_PATH "test019_3d_aa.mib",
   MIB_PATH "test020_clip2d.mib",                // clip pre-tesselate polyline
   MIB_PATH "test021_clip2d.mib",                // clip post-tesselate polyline
   MIB_PATH "test022_clip2d.mib",                // clip thin line
   MIB_PATH "test023_clip2d.mib",                // clip concave pre-tesselate
   MIB_PATH "test024_clip2d.mib",                // clip concave post-tesselate
   MIB_PATH "test025_clip2d.mib",                // clip concave post-tesselate ("minnie")
   MIB_PATH "test026_clip2d.mib",                // transform2d + clip concave post-tesselate
   MIB_PATH "test027_clip2dfb.mib",              // clip against fb
   MIB_PATH "test028_3d.mib",                    // fill convex transform3d clip3d
   MIB_PATH "test029_3d.mib",                    // extrude polyline transform3d clip3d
   MIB_PATH "test030_minnie_3d.mib",             // concave transform3d clip3d
   MIB_PATH "test031_tf2dclip.mib",
   MIB_PATH "test032_subpath.mib",
   MIB_PATH "test033_valentines.mib",            // SGI/VG tesselator
   MIB_PATH "test034_ducky.mib",
   MIB_PATH "test035_bicycle.mib",               // SGI/VG tesselator
   MIB_PATH "test036_crab.mib",                  // SGI/VG tesselator
   MIB_PATH "test037_logo.mib",
   MIB_PATH "test038_logo_noaa.mib",
   MIB_PATH "test039_wildboar.mib",              // SGI/VG tesselator
   MIB_PATH "test040_tiger.mib",                 // SGI/VG tesselator
   MIB_PATH "test041_rabbit.mib",                // SGI/VG tesselator
   MIB_PATH "test042_oil_lamp.mib",              // SGI/VG tesselator
   MIB_PATH "test043_elefant.mib",               // SGI/VG tesselator
   MIB_PATH "test044_linejoin_miterlimit.mib",   // SGI/VG tesselator
   MIB_PATH "test045_linejoin.mib",              // SGI/VG tesselator
   MIB_PATH "test046_evenodd.mib",               // SGI/VG tesselator
   MIB_PATH "test047_shapes.mib",                // SGI/VG tesselator
   MIB_PATH "test048_nano.mib",                  // SGI/VG tesselator
   MIB_PATH "test049_debug.mib",                 // SGI/VG tesselator
   MIB_PATH "test050_world.mib",                 // huge. >100mb sysRAM. SGI/VG tesselator
   MIB_PATH "test051_rect.mib",                  // SVG-exported rects
   MIB_PATH "test052_ellipse.mib",               // SVG-exported ellipses
   MIB_PATH "test053_elephant.mib",              // SVG-exported rects, ellipses, polylines, polygons
   MIB_PATH "test054_365845.mib",                // SGI/VG tesselator
   MIB_PATH "test055_dragon_986054.mib",         // SGI/VG tesselator
   MIB_PATH "test056_satellite.mib",             // SGI/VG tesselator. ~18mb sysRAM, ~0.8mb vidRAM
   MIB_PATH "test057_lightbulb.mib",             // SGI/VG tesselator
   MIB_PATH "test058_motorbike.mib",             // SGI/VG tesselator
   MIB_PATH "test059_abstractbike.mib",          // SGI/VG tesselator
   MIB_PATH "test060_watermelon.mib",            // nok (stray polygons)
   MIB_PATH "test061_man.mib",                   //
   MIB_PATH "test062_gorilla.mib",               // nok (stray polygons)
   MIB_PATH "test063_quill.mib",                 //
   MIB_PATH "test064_fairy.mib",                 //
   MIB_PATH "test065_fish.mib",                  //
   MIB_PATH "test066_cat.mib",                   // nok (broken mesh when path_seg > 20)
   MIB_PATH "test067_reaper.mib",                //
   MIB_PATH "test068_pidgeon.mib",               // (SVG) <style> classes
   MIB_PATH "test069_bluejay.mib",               // (SVG) <style> classes
   MIB_PATH "test070_fox1.mib",                  // (SVG) <style> classes
   MIB_PATH "test071_fox2.mib",                  // (SVG) <style> classes
   MIB_PATH "test072_fox3.mib",                  // (SVG) <style> classes
   MIB_PATH "test073_cookie.mib",                // nok (minor tesselation errors?) (SVG) <style> classes
   MIB_PATH "test074_roundrect.mib",             // rounded rectangles (op 0xA4)
   MIB_PATH "test075_roundrect_svg.mib",         // rounded rectangles (exported from SVG)
   MIB_PATH "test076_hamster.mib",               //
   MIB_PATH "test077_giraffe.mib",               //
   MIB_PATH "test078_butterfly.mib",             //
   MIB_PATH "test079_minimal.mib",               //
   MIB_PATH "test080_debug_l1.mib",              //
   MIB_PATH "test081_debug_l2.mib",              //
   MIB_PATH "test082_circle.mib",                //
   MIB_PATH "test083_ellipse_fill.mib",          //
   MIB_PATH "test084_ellipse_stroke.mib",        //
   MIB_PATH "test085_ellipse_fill_stroke.mib",   //
   MIB_PATH "test086_rect_fill.mib",             //
   MIB_PATH "test087_rect_fill_stroke.mib",      //
   MIB_PATH "test088_rect_stroke.mib",           //
   MIB_PATH "test089_roundrect_fill_stroke.mib", //
   MIB_PATH "test090_roundrect_fill.mib",        //
   MIB_PATH "test091_roundrect_stroke.mib",      //
   MIB_PATH "test092_linejoin_round.mib",        // 
};
#define NUM_TESTS  (sizeof(tests) / sizeof(const char*))

static sSI test_idx = -1;
static sUI auto_cycle_num_frames =  // >0:auto-cycle tests (any key stroke interrupts this)
#ifdef AUTO_CYCLE_NUM_FRAMES
   AUTO_CYCLE_NUM_FRAMES
#else
   0u
#endif // AUTO_CYCLE_NUM_FRAMES
   ;


// ---------------------------------------------------------------------------- vars
static sU32 last_ticks = 0u;  // 1000 ticks per second
static sU32 ticks_start = 0u;
static sUI num_frames_rendered = 0u;
static sUI total_num_frames_rendered = 0u;

static MinnieDrawable *drawable;

static sF32 rot_ang = 0.0f;

#if 0
static sBool b_reopen = YAC_FALSE;
#endif

static const char *pathNameIn = NULL;
static YAC_Buffer fileBuf = {0};


// ---------------------------------------------------------------------------- LoadFileBuf
static sBool LoadFileBuf(void) {
   sBool ret = YAC_FALSE;
   yac_buffer_free(&fileBuf);
   FILE *f = fopen(pathNameIn, "rb");
   if(NULL != f)
   {
      fseek(f, 0, SEEK_END);
      sUI sz = (sUI) ftell(f);
      fseek(f, 0, SEEK_SET);

      if(yac_buffer_alloc(&fileBuf, sz))
      {
         size_t numRead = (size_t)fread(fileBuf.buffer, 1, sz/*nmemb*/, f);
         (void)numRead;
         Dprintf("[dbg] LoadFileBuf: read %u bytes from \"%s\"\n", fileBuf.size, pathNameIn);
         hal_window_set_title(pathNameIn);
         minDrawableReset(drawable);
         rot_ang = 0.0f;
         num_frames_rendered = 0u;
         ret = YAC_TRUE;
      }

      fclose(f);
   }
   else
   {
      Dprintf("[---] LoadFileBuf: failed to open \"%s\" for reading\n", pathNameIn);
   }
   return ret;
}

// ---------------------------------------------------------------------------- SelectNextTest
static void SelectNextTest() {
   test_idx = sWRAP(test_idx + 1, 0, NUM_TESTS);
   pathNameIn = tests[test_idx];
   (void)LoadFileBuf();
   Dprintf("[...] test_idx is %d (\"%s\")\n", test_idx, pathNameIn);
}

// ---------------------------------------------------------------------------- SelectPreviousTest
static void SelectPreviousTest() {
   test_idx = sWRAP(test_idx - 1, 0, NUM_TESTS);
   pathNameIn = tests[test_idx];
   (void)LoadFileBuf();
   Dprintf("[...] test_idx is %d (\"%s\")\n", test_idx, pathNameIn);
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

   if(!b_gl_buf_once || !minDrawableIsComplete(drawable))
   {
      minSetStrokeWLineStripThreshold(b_line_strip*99.0f);  // 0=tesselate to triangles
      minSetStrokeWLineJoinThreshold(1.0f);  // disable bevel+round line joins below this threshold
      minSetStrokeScale(stroke_scale);

      if(MinnieVG_SetupDrawableFromBuffer(drawable, &fileBuf))
      {
         // Dprintf("xxx drawable.sz=(%f; %f)\n", minDrawableGetSizeX(drawable), minDrawableGetSizeY(drawable));
      }

      if(b_benchmark)
      {
         ticks_start = hal_get_ticks();
      }
   }

   MinnieVG_BeginFrame();

   // sdvg_ClearARGB(0x00ffffffu);
   // sdvg_ClearARGB(0x00e1cdb7u);
   // clear color+stencil  (#00e1cdb7)
   sdvg_ClearARGB(minDrawableGetBackgroundColor(drawable));
   sdvg_ClearStencil(0);

   if(1 && b_draw_gl)
   {
      sF32 dw = minDrawableGetSizeX(drawable);
      sF32 dh = minDrawableGetSizeY(drawable);
      if(1)
         sdvg_ProjInit2D(dw, dh);
      else
         sdvg_ProjInit2D(VP_W, VP_H);
      if(dw > 0.0f)
      {
         // (note) low-level "sdvg" API line widths are in viewport/pixel coordinate system
         //         (independent of projection matrix, similar to OpenGL)
         //         (e.g. Bezier01Edit widget uses normalized (1;1) orthographic projection matrix and stroke width = 1.0f)
         // (note) high-level "min" API SW-tesselated lines (triangles) are in drawable/projection coordinate system
         // (note) => align low-level and high-level line widths
         //            (e.g. inc line widths when drawable size is (640;480) but viewport size is (800;600))
         sF32 strokeScale = ((sF32)DISPLAY_WIDTH) / dw;
         sdvg_SetStrokeScale(strokeScale);
         sdvg_SetPointScale(strokeScale);
      }
      minDrawableSetScale2f(drawable, 1.0f, 1.0f);
      minDrawableSetRotation(drawable, rot_ang);
      minDrawableSetTranslate2f(drawable, 0.0f, 0.0f);
      MinnieVG_SetTransformForDrawable(drawable);

      minDrawableSetEnableDebug(drawable, !b_benchmark && (0u == (num_frames_rendered & 255u)));

      for(sUI iter = 0u; iter < NUM_ITER; iter++)
      {
         minDrawableDraw(drawable);
      }
   }
   else
   {
      sdvg_ProjInit2D(VP_W, VP_H);
      sdvg_SetStrokeWidth(3.0f);
      sdvg_SetColorARGB(0xFFff0000u);
      sdvg_UnbindVBO();
      if(sdvg_BeginLinesAA(2))
      {
         sdvg_Vertex2f(0.0f, 0.0f);
         sdvg_Vertex2f(VP_W, VP_H);
         sdvg_End();
      }
   }

   MinnieVG_EndFrame();

   // if(b_glcore)
      glBindVertexArray(0);

   if(0u == (num_frames_rendered & 255u))
   {
      MinnieVG_DebugPrintMinnieAndDrawableStats(drawable);
   }

   if(b_benchmark)
      glFinish();

   if(b_benchmark || 0u == (total_num_frames_rendered & 127u))
   {
      sUI tDelta = hal_get_ticks() - ticks_start;
      if(tDelta > 0u)
      {
         sF32 fps = (sF32)((1000.0 * total_num_frames_rendered * NUM_ITER) / tDelta);
         Dprintf("[...] FPS=%f\n", fps);
      }
   }

#ifdef SHADERVG_NO_DISPLAY
   glFlush();
#else
   hal_window_swap();
#endif // SHADERVG_NO_DISPLAY

   num_frames_rendered++;
   total_num_frames_rendered++;

#ifdef SHADERVG_AUTO_EXIT_FRAMES
   if(SHADERVG_AUTO_EXIT_FRAMES == num_frames_rendered)
   {
      glFinish();
      sU32 tDelta = hal_get_ticks() - ticks_start;
      sF32 fps = (1000.0 * SHADERVG_AUTO_EXIT_FRAMES) / tDelta;
      Dprintf("[...] auto_exit after %u frames / %u millisec => %3.2f fps\n", num_frames_rendered, tDelta, ((sSI)(fps*100))/100.0f);
      hal_window_quit();
   }
#endif // SHADERVG_AUTO_EXIT_FRAMES

   if(!b_benchmark && b_rotate)
   {
      rot_ang += dt * 0.0001023f;
      rot_ang = sWRAP(rot_ang, 0.0f, sM_2PIf);
   }

   if(auto_cycle_num_frames > 0u)
   {
      if(0u == (num_frames_rendered % auto_cycle_num_frames))
      {
         SelectNextTest();
      }
   }
}

// ---------------------------------------------------------------------------- OnOpen
#if 0
static void OnOpen() {
   trace "[trc] onOpen ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";

   if(!MinnieVG.OnOpen())
   {
      die "MinnieVG.OnOpen() failed";
   }

   sdvg_SetFramebufferSize(Viewport.width, Viewport.height);

   drawable.onOpen();
}
#endif

// ---------------------------------------------------------------------------- hal_on_key_down
void hal_on_key_down(sU32 _code, sU32 _mod) {
   // Dprintf("[>>>] hal_on_key_down: code=0x%08x  mod=0x%08x\n", _code, _mod);
   if('v' != _code)
      auto_cycle_num_frames = 0u;

   switch(_code)
   {
      default:
         if(MinnieVG_HandleDebugKey(_code, _mod))  // 'a', 'd', 'e', 'h', 'k', 'i', 'u', '1', '2', '3', '4', ..
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
         b_rotate = !b_rotate;
         Dprintf("[...] b_rotate is %d\n", b_rotate);
         break;

      case VKEY_RETURN:
         rot_ang = 0.0f;
         Dprintf("[...] reset rotation\n");
         break;

      case 'g':
         b_draw_gl = !b_draw_gl;
         Dprintf("[...] b_draw_gl is %d\n", b_draw_gl);
         break;

#if 0
      case 'm':
         b_msaa = !b_msaa;
         Dprintf("[...] b_msaa is %d\n", b_msaa);
         b_reopen = YAC_TRUE;
         minDrawableReset(drawable);
         hal_window_quit();
         break;
#endif

      case 'o':
         b_gl_buf_once = !b_gl_buf_once;
         Dprintf("[...] b_gl_buf_once is %d\n", b_gl_buf_once);
         break;

#if 0
      case 'p':
         b_queued_screenshot = YAC_TRUE;
         Dprintf("[...] queue screenshot\n");
         break;
#endif

      case 's':
         b_line_strip = !b_line_strip;
         Dprintf("[...] b_line_strip is %d\n", b_line_strip);
         minDrawableReset(drawable);
         num_frames_rendered = 0u;
         break;

      case 'v':
         b_vsync = !b_vsync;
         Dprintf("[...] b_vsync is %d\n", b_vsync);
         hal_set_swap_interval((b_vsync && !b_benchmark) ? 1 : 0);
         break;

      case VKEY_UP:
         SelectNextTest();
         break;

      case VKEY_DOWN:
         SelectPreviousTest();
         break;

      case VKEY_RIGHT:
         stroke_scale = sMIN(stroke_scale + 0.125f, 16.0f);
         Dprintf("[...] stroke_scale is %f\n", stroke_scale);
         minDrawableReset(drawable);
         num_frames_rendered = 0u;
         break;

      case VKEY_LEFT:
         stroke_scale = sMAX(stroke_scale - 0.125f, 0.125f);
         Dprintf("[...] stroke_scale is %f\n", stroke_scale);
         minDrawableReset(drawable);
         num_frames_rendered = 0u;
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
         pathNameIn = tests[test_idx];
      }
      else
      {
         Dprintf("[---] invalid test_idx=%d\n", test_idx);
         return 10;
      }
   }
   else
   {
      pathNameIn = tests[0];
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

      MinnieVG_Init();
      MinnieVG_InitScratchBuffers();
      MinnieVG_OnOpen();

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

      memset(&fileBuf, 0, sizeof(YAC_Buffer));
      if(!LoadFileBuf())
          exit(10);

      // Main loop
      Dprintf("[...] entering event loop\n");

      hal_set_swap_interval((b_vsync && !b_benchmark) ? 1 : 0);

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
}
