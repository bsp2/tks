// ----
// ---- file   : MinnieVG.c
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
// ---- info   : "minnie" test case utilities
// ---- note   :
// ----
// ----
// ----

#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#include "../inc_minnie.h"
#include "hal.h"

#include "MinnieVG.h"


// ----------------------------------------------------------------------------
static sBool b_profile = 1;
static sBool b_aa      = 1;     // 'a'
static sF32  aa_range  = 2.5f;  // (todo) m/lshift-m
static sF32  aa_exp    = 1.0f;  // (todo) 'k', lshift-k

static YAC_Buffer minnie_scratch;

// ----------------------------------------------------------------------------
void MinnieVG_Init(sBool _bGLCore) {
   MinnieVG_InitShaderVG(_bGLCore);
   MinnieVG_InitMinnie();
   MinnieVG_InitMinnieScratchBuffers();
}

// ----------------------------------------------------------------------------
void MinnieVG_InitShaderVG(sBool _bGLCore) {
   sdvg_SetScratchBufferSize(64*1024);
#if 0
   sdvg_SetGLSLVersion(1/*b_glcore*//*bV3*/, YAC_FALSE/*bGLES*/, NULL/*sVersionStringOrNull*/);
#endif // 0
   sdvg_Init(_bGLCore);
}

// ----------------------------------------------------------------------------
void MinnieVG_InitMinnie(void) {
   minnie_impl_init();

   minSetEnableDraw(1);
   minSetEnableDrawSW(0);
   minSetEnableRenderFillConcave(1);       // '1'
   minSetEnableRenderFillComplex(1);       // '2'
   minSetEnableRenderStrokes(1);           // '3'
   minSetEnableRenderJoinCap(1);           // '4'
   sdvg_SetEnableDrawInner(1);             // '5'
   sdvg_SetEnableDrawBorder(1);            // '6'
   minSetEnableEdgeAA(0);                  // 'e'  (experimental)
   minSetStrokeWLineStripThreshold(1*99);  // 's'  (0=tesselate to triangles)
   minSetStrokeWLineJoinThreshold(0.2f);   //      (disable bevel+round line joins below this stroke-width threshold)
   minSetEnableUniformColors(1);           // 'u'  (1=less VRAM/more draw calls  0=faster)
   minSetEnablePolygonAA(1);               // 'l'  (requires GPU polygon tesselation)

   if(0)
   {
      // (note) 27Oct2025 used for mixed cpu/gpu benchmarks
      // (note) UniformColors=off
      // (note) LineStrip=off
      // (note) should enable multisample-antialiasing
      minSetEnableForceConcaveComplex(0);       // 'h'  (note) concave tesselator does not handle all paths (=> force SGI tesselator)
      minSetEnableForceComplexConcave(0);
      minSetEnableTesselateConcave(0);          // 'i'  0=GPU tesselation  1=CPU tesselation
      minSetSwTesselateSizeThreshold(128*8*8);  // 0=disable   >0: force concave path if concave or evenodd-without-subpaths bbox is <= sizeThreshold, else force evenodd path (SW tesselate)
      minSetEnableMultiPathHWPolygons(0);
   }
   else if(0)
   {
      // use SW tesselator for all path types
      // (note) fastest rendering (m2pro: Tiger @~43000 fps)
      // (note) longest setup time
      // (note) should enable multisample-antialiasing
      minSetEnableForceConcaveComplex(1);
      minSetEnableForceComplexConcave(0);
      minSetEnableTesselateConcave(1);
      minSetSwTesselateSizeThreshold(0);
      minSetEnableMultiPathHWPolygons(0);
   }
   else
   {
      // use GPU tesselator for all path types
      // (note) slowest rendering (m2pro: Tiger @~1200 fps)
      // (note) fastest setup time
      // (note) supports PolygonAA w/o MSAA
      minSetEnableForceConcaveComplex(0);
      minSetEnableForceComplexConcave(0);
      minSetEnableTesselateConcave(0);
      minSetSwTesselateSizeThreshold(9999999);
      minSetEnableMultiPathHWPolygons(1);
   }
}

// ----------------------------------------------------------------------------
static sUI loc_MinnieVG_InitScratchBuffers_int(void) {
   // (todo) add defPointsPerPath / maxPointsPerPath / maxClippedTrisPerPath / maxExtrudedVerticesPerPath / maxClippedTrisPerPath args
   if(0)  // sz=1179744 bytes (~1.13MB)
      return minInitScratchBuffers(&minnie_scratch,
                                   4* 256/*defPointsPerPath*/,
                                   4*1024/*maxPointsPerPath*/,
                                   4*1536/*maxClippedPointsPerPath*/,
                                   4*3072/*maxExtrudedVerticesPerPath*/,
                                   4*4096/*maxClippedTrisPerPath*/
                                   );
   else if(0)  // tiger  sz=45152 bytes (~44k)
      return minInitScratchBuffers(&minnie_scratch,
                                   1*  32/*defPointsPerPath*/,
                                   4* 256/*maxPointsPerPath*/,
                                   0* 256/*maxClippedPointsPerPath*/,
                                   16* 256/*maxExtrudedVerticesPerPath*/,
                                   0* 256/*maxClippedTrisPerPath*/
                                   );
   else if(0)  // tiger + (most) mib tests
      return minInitScratchBuffers(&minnie_scratch,
                                   1*  32/*defPointsPerPath*/,
                                   5* 256/*maxPointsPerPath*/,
                                   5* 256/*maxClippedPointsPerPath*/,
                                   5* 256/*maxExtrudedVerticesPerPath*/,
                                   5* 256/*maxClippedTrisPerPath*/
                                   );
   else if(0)  // map2  sz=43064 bytes (~42k)
      return minInitScratchBuffers(&minnie_scratch,
                                   1*  32/*defPointsPerPath*/,
                                   850/*maxPointsPerPath*/,
                                   0* 256/*maxClippedPointsPerPath*/,
                                   4*1024/*maxExtrudedVerticesPerPath*/,
                                   0* 256/*maxClippedTrisPerPath*/
                                   );
   else if(0)  // sz=2343008 bytes (~2.3MB)
      return minInitScratchBuffers(&minnie_scratch,
                                   4* 256/*defPointsPerPath*/,
                                   20*1024/*maxPointsPerPath*/,
                                   8*1024/*maxClippedPointsPerPath*/,
                                   8*3072/*maxExtrudedVerticesPerPath*/,
                                   8*4096/*maxClippedTrisPerPath*/
                                   );
   else if(1)  // world map  sz=11206752 bytes (~10.69MB)
      return minInitScratchBuffers(&minnie_scratch,
                                   2* 256/*defPointsPerPath*/,
                                   38*1024/*maxPointsPerPath*/,
                                   38*1536/*maxClippedPointsPerPath*/,
                                   38*3072/*maxExtrudedVerticesPerPath*/,
                                   38*4096/*maxClippedTrisPerPath*/
                                   );

   // world.svg / test050_world.mib
   //   [...] total #points=95376 #lines=0 #tris=91936 buf_gl.offset=8274240 FPS.current=58.8235
   //   [...] minnie_alloc_debug_print_stats: #alloc=6404 #free=1533 live=4871 max=183752.55k cur=183738.77k
   return minInitScratchBuffers(&minnie_scratch,
                                4* 256/*defPointsPerPath*/,
                                4*4096/*maxPointsPerPath*/,
                                4*2048/*maxClippedPointsPerPath*/,
                                4*2048/*maxExtrudedVerticesPerPath*/,
                                4*2048/*maxClippedTrisPerPath*/
                                );
}

// ----------------------------------------------------------------------------
sBool MinnieVG_InitMinnieScratchBuffers(void) {
   // (todo) add defPointsPerPath / maxPointsPerPath / maxClippedTrisPerPath / maxExtrudedVerticesPerPath / maxClippedTrisPerPath args
   memset(&minnie_scratch, 0, sizeof(minnie_scratch));

   sUI reqSz = loc_MinnieVG_InitScratchBuffers_int();
   if(minnie_scratch.size < reqSz)
   {
      Dprintf("[...] MinnieVG_InitMinnieScratchBuffers: adjusting scratch.size=%u to reqSz=%u\n", minnie_scratch.size, reqSz);
      (void)yac_buffer_alloc(&minnie_scratch, reqSz);
   }
   reqSz = loc_MinnieVG_InitScratchBuffers_int();

   if(minnie_scratch.size >= reqSz)
   {
      // Succeeded
      Dprintf("[...] MinnieVG_InitMinnieScratchBuffers: using %u/%u scratch bytes\n", reqSz, minnie_scratch.size);
      return YAC_TRUE;
   }
   else
   {
      Dprintf("[---] MinnieVG_InitMinnieScratchBuffers: insufficient scratch size. have=%u need=%u\n", minnie_scratch.size, reqSz);
   }

   return YAC_FALSE;
}

// ----------------------------------------------------------------------------
sBool MinnieVG_OnOpen(void) {

   // zglLoadExtensions();

   if(sdvg_OnOpen())
   {
      // Succeeded
      return YAC_TRUE;
   }
   else
   {
      Dprintf("[---] MinnieVG_OnOpen: sdvg_OnOpen() failed\n");
   }

   return YAC_FALSE;
}

// ----------------------------------------------------------------------------
void MinnieVG_Exit(void) {

   yac_buffer_free(&minnie_scratch);

   Dprintf("[dbg] call minnie_impl_exit()\n");
   minnie_impl_exit();
}

// ----------------------------------------------------------------------------
void MinnieVG_BeginFrame(void) {
   sdvg_BeginFrame();
   sdvg_SetEnableAA(b_aa);
   sdvg_SetAARange(b_aa ? aa_range : 0.01f);
   sdvg_SetAAExp(aa_exp);
}

// ----------------------------------------------------------------------------
void MinnieVG_EndFrame(void) {
   sdvg_EndFrame();
}

// ----------------------------------------------------------------------------
sBool MinnieVG_SetupDrawableFromBuffer(MinnieDrawable *_drawable, YAC_Buffer *_mib) {

   sUI tStart = hal_get_ticks();

   minDrawableBegin(_drawable);

   _mib->io_offset = 0u;
   if(minParseBuffer(_mib))
   {
      sUI tDelta = hal_get_ticks() - tStart;

      if(b_profile)
      {
         Dprintf("[...] MinnieVG::SetupDrawableFromBuffer: MIB stream processed in %u ms. total #points=%u #lines=%u #lineStrips=%u #tris=%u #trisTex=%u #ellipses=%u buf_gl.offset=%u buf_draw.offset=%u\n",
                 tDelta,
                 minGetTotalNumPoints(),
                 minGetTotalNumLines(),
                 minGetTotalNumLineStrips(),
                 minGetTotalNumTris(),
                 minGetTotalNumTrisTex(),
                 minGetTotalNumEllipses(),
                 minDrawableGetGLOffset(_drawable),
                 minDrawableGetDrawOffset(_drawable)
                 );
      }

      minDrawableSetSize2f(_drawable, (sF32)minGetWidth(), (sF32)minGetHeight());
      minDrawableEnd(_drawable);

      // Succeeded
      return YAC_TRUE;
   }

   return YAC_FALSE;
}

// ----------------------------------------------------------------------------
void MinnieVG_DebugPrintMinnieAndDrawableStats(MinnieDrawable *_drawable) {
   if(b_profile)
   {
      Dprintf("[...] MinnieVG::DebugPrintMinnieAndDrawableStats: total #points=%u #lines=%u #lineStrips=%u #tris=%u #trisTex=%u #rects=%u #ellipses=%u buf_gl.offset=%u (%f MB) buf_draw.offset=%u\n",
              minGetTotalNumPoints(),
              minGetTotalNumLines(),
              minGetTotalNumLineStrips(),
              minGetTotalNumTris(),
              minGetTotalNumTrisTex(),
              minGetTotalNumRects(),
              minGetTotalNumEllipses(),
              minDrawableGetGLOffset(_drawable),
              ((sSI)((100.0*minDrawableGetGLOffset(_drawable))/(1024.0*1024.0))/100.0),
              minDrawableGetDrawOffset(_drawable)
              );
   }
   minDebugPrintAllocStats();
   minDebugPrintPathStats();
}

// ----------------------------------------------------------------------------
void MinnieVG_CalcModelTransform(MinnieDrawable *_d, Matrix4f *_mat) {
   minnie_matrix4f_initIdentity(_mat);
   minnie_matrix4f_translatef(_mat,
                              minDrawableGetSizeX(_d)*0.5f + minDrawableGetTranslateX(_d),
                              minDrawableGetSizeY(_d)*0.5f + minDrawableGetTranslateY(_d),
                              0.0f
                              );
   minnie_matrix4f_rotatef(_mat, 0, 0, minDrawableGetRotation(_d));
   minnie_matrix4f_scalef(_mat,
                          minDrawableGetScaleX(_d),
                          minDrawableGetScaleY(_d),
                          1.0f
                          );
   minnie_matrix4f_translatef(_mat,
                              -minDrawableGetSizeX(_d)*0.5f,
                              -minDrawableGetSizeY(_d)*0.5f,
                              0.0f
                              );
}

// ----------------------------------------------------------------------------
void MinnieVG_SetTransformForDrawable(MinnieDrawable *_d) {
   MinnieVG_CalcModelTransform(_d, sdvg_GetModelTransformRef());
   sdvg_UpdateTransform();
}

// ----------------------------------------------------------------------------
sBool MinnieVG_HandleDebugKey(sU32 _code, sU32 _mod) {
   // Dprintf("[dbg] MinnieVG::HandleDebugKey k.code=%u k.mod=%u\n", _code, _mod);
   switch(_code)
   {
      case '1':
         minSetEnableRenderFillConcave(!minGetEnableRenderFillConcave());
         Dprintf("[...] MinnieVG_HandleDebugKey: minnie::setup.enableRenderFillConcave is %d\n", minGetEnableRenderFillConcave());
         return YAC_TRUE;

      case '2':
         minSetEnableRenderFillComplex(!minGetEnableRenderFillComplex());
         Dprintf("[...] MinnieVG:_HandleDebugKey: minnie::setup.enableRenderFillComplex is %d\n", minGetEnableRenderFillComplex());
         return YAC_TRUE;

      case '3':
         minSetEnableRenderStrokes(!minGetEnableRenderStrokes());
         Dprintf("[...] MinnieVG_HandleDebugKey: minnie::setup.enableRenderStrokes is %d\n", minGetEnableRenderStrokes());
         return YAC_TRUE;

      case '4':
         minSetEnableRenderJoinCap(!minGetEnableRenderJoinCap());
         Dprintf("[...] MinnieVG_HandleDebugKey: minnie::setup.enableRenderJoinCap is %d\n", minGetEnableRenderJoinCap());
         return YAC_TRUE;

      case '5':
         sdvg_SetEnableDrawInner(!sdvg_GetEnableDrawInner());
         Dprintf("[...] MinnieVG_HandleDebugKey: minnie::setup.enableDrawInner is %d\n", sdvg_GetEnableDrawInner());
         return YAC_TRUE;

      case '6':
         sdvg_SetEnableDrawBorder(!sdvg_GetEnableDrawBorder());
         Dprintf("[...] MinnieVG_HandleDebugKey: minnie::setup.enableDrawBorder is %d\n", sdvg_GetEnableDrawBorder());
         return YAC_TRUE;

      case 'a':
         b_aa = !b_aa;
         Dprintf("[...] MinnieVG_HandleDebugKey: b_aa is %d\n", b_aa);
         break;

      case 'd':
         sdvg_SetEnableDebug(!sdvg_GetEnableDebug());
         Dprintf("[...] MinnieVG_HandleDebugKey: sdvg_GetEnableDebug() is %d\n", sdvg_GetEnableDebug());
         return YAC_TRUE;

      case 'e':
         minSetEnableEdgeAA(!minGetEnableEdgeAA());
         Dprintf("[...] MinnieVG_HandleDebugKey: minnie::setup.enableEdgeAA is %d\n", minGetEnableEdgeAA());
         return YAC_TRUE;

      case 'h':
         minSetEnableForceConcaveComplex(!minGetEnableForceConcaveComplex());
         Dprintf("[...] MinnieVG_HandleDebugKey: minnie::setup.enableForceConcaveComplex is %d\n", minGetEnableForceConcaveComplex());
         return YAC_TRUE;

      case 'k':
         if(_mod)
            aa_exp = sMAX(aa_exp - 0.03125f, 0.03125f);
         else
            aa_exp = sMIN(aa_exp + 0.03125f, 1.5f);
         Dprintf("[...] MinnieVG_HandleDebugKey: aa_exp is %f\n", aa_exp);
         break;

      case 'i':
         minSetEnableTesselateConcave(!minGetEnableTesselateConcave());
         Dprintf("[...] MinnieVG_HandleDebugKey: minnie::setup.enableTesselateConcave is %d\n", minGetEnableTesselateConcave());
         return YAC_TRUE;

      case 'l':
         minSetEnablePolygonAA(!minGetEnablePolygonAA());
         Dprintf("[...] MinnieVG_HandleDebugKey: minnie::setup.enablePolygonAA is %d\n", minGetEnablePolygonAA());
         return YAC_TRUE;

      case 'n':
         minSetEnableMultiPathHWPolygons(!minGetEnableMultiPathHWPolygons());
         Dprintf("[...] MinnieVG_HandleDebugKey: minnie::setup.enableMultiPathHWPolygons is %d\n", minGetEnableMultiPathHWPolygons());
         return YAC_TRUE;

      case 's':
         if(minGetStrokeWLineStripThreshold() < 0.1f)
            minSetStrokeWLineStripThreshold(99.0f);
         else
            minSetStrokeWLineStripThreshold(0.0f);
         Dprintf("[...] MinnieVG_HandleDebugKey: minnie::setup.strokeWLineStripThreshold is %f\n", minGetStrokeWLineStripThreshold());
         return YAC_TRUE;

      case 'u':
         minSetEnableUniformColors(!minGetEnableUniformColors());
         Dprintf("[...] MinnieVG::HandleDebugKey: minnie::setup.enableUniformColors is %d\n", minGetEnableUniformColors());
         return YAC_TRUE;
   }
   return YAC_FALSE;
}
