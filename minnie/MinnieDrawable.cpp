// ----
// ---- file   : MinnieDrawable.cpp
// ---- author : bsp
// ---- legal  : Distributed under terms of the MIT LICENSE.
// ----
// ---- info   : "minnie" API
// ---- note   :
// ----
// ----
// ----

#ifdef USE_MINNIE_MIB_SETUP

#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#include "inc_yac.h"

#define MINNIE_SKIP_TYPEDEFS defined
#include "minnie.h"
#include "MinnieDrawable.h"

#include "inc_opengl.h"

#include "shadervg/shadervg.h"
#include "shadervg/shadervg_internal.h"
#include "shadervg/Shader.h"
#include "shadervg/Shape.h"

#ifndef MINNIE_LIB
#include "../tkopengl/tkopengl_shared.h"
#endif

static sBool b_debug_draw_list = 0;

#define Ddebug_draw_list_printf if(!b_debug_draw_list);else Dprintf
#define Ddebug_draw_list_printf if(!b_debug_draw_list);else Dprintf


// ---------------------------------------------------------------------------- MinnieDrawable
_MinnieDrawable::_MinnieDrawable(void) {
   buf_gl = NULL;
   gl_buf_id = 0u;
   b_gl_buf_update_pending = YAC_FALSE;

   buf_draw = NULL;
   c32_bg_color = 0u;

   size_x = 640.0f;
   size_y = 480.0f;

   scale_x = 1.0f;
   scale_y = 1.0f;
   rot_ang = 0.0f;
   trans_x = 0.0f;
   trans_y = 0.0f;
}

_MinnieDrawable::~_MinnieDrawable() {
   free();
}

void _MinnieDrawable::init(void) {
}

sBool _MinnieDrawable::alloc(sUI _maxGLBufSize, sUI _maxDrawBufSize) {
   sBool ret = YAC_FALSE;

   if(_maxGLBufSize < 4096)
      _maxGLBufSize = MINNIE_DEFAULT_GL_BUF_SIZE;

   if(_maxDrawBufSize < 1024)
      _maxDrawBufSize = MINNIE_DEFAULT_DRAW_BUF_SIZE;

   if(NULL == buf_gl)
   {
      buf_gl   = YAC_New_Buffer();
      buf_draw = YAC_New_Buffer();
   }

#ifdef SHADERVG_SCRIPT_API
   if(buf_gl->yacArrayAlloc(_maxGLBufSize, 0,0,0))
   {
      if(buf_draw->yacArrayAlloc(_maxDrawBufSize, 0,0,0))
      {
         // Succeeded
         ret = YAC_TRUE;
      }
   }
#else
   // MINNIE_LIB
   if(buf_gl->alloc(_maxGLBufSize))
   {
      if(buf_draw->alloc(_maxDrawBufSize))
      {
         // Succeeded
         ret = YAC_TRUE;
      }
   }
#endif // SHADERVG_SCRIPT_API

   return ret;
}

void _MinnieDrawable::reset(void) {
   Dstream_set_offset(buf_gl, 0u);
   Dstream_set_offset(buf_draw, 0u);
}

void _MinnieDrawable::setEnableDebug(sBool _bEnable) {
   b_debug = _bEnable;
}

sBool _MinnieDrawable::getEnableDebug(void) {
   return b_debug;
}

void _MinnieDrawable::freeGL(void) {
   // must be called by onOpen()
   if(0u != gl_buf_id)
   {
      Dsdvg_glcall(zglDeleteBuffer(gl_buf_id));
      gl_buf_id = 0u;
   }
}

void _MinnieDrawable::free(void) {
   freeGL();

   YAC_DELETE_SAFE(buf_gl);
   YAC_DELETE_SAFE(buf_draw);
}

void _MinnieDrawable::lazyAllocGL(void) {
   // Create vertex attrib buffer
   if(0u == gl_buf_id && NULL != buf_gl && buf_gl->size > 0u)
   {
      gl_buf_id = Dsdvg_glcall(zglGenBuffer());
      Dsdvg_glcall(glBindBuffer(GL_ARRAY_BUFFER, gl_buf_id));
      Dsdvg_glcall(zglBufferData(GL_ARRAY_BUFFER, buf_gl->size, NULL, GL_STATIC_DRAW));
      Dsdvg_glcall(glBindBuffer(GL_ARRAY_BUFFER, 0)); // remove binding
      Dyac_host_printf("[dbg] MinnieDrawable::lazyAllocGL: VBO last error is %d\n", Dsdvg_glcall(glGetError()));
   }
}

YAC_Buffer *_MinnieDrawable::getGLBuffer(void) {
   return buf_gl;
}

sUI _MinnieDrawable::getGLOffset(void) {
   return (NULL != buf_gl) ? Dstream_get_offset(buf_gl) : 0u;
}

YAC_Buffer *_MinnieDrawable::getDrawBuffer(void) {
   return buf_draw;
}

sUI _MinnieDrawable::getDrawOffset (void) {
   return (NULL != buf_draw) ? Dstream_get_offset(buf_draw) : 0u;
}

void _MinnieDrawable::onOpen(void) {
   freeGL();
}

void _MinnieDrawable::setSize2f(sF32 _w, sF32 _h) {
   size_x = _w;
   size_y = _h;
}

void _MinnieDrawable::setSizeX(sF32 _w) {
   size_x = _w;
}

void _MinnieDrawable::setSizeY(sF32 _h) {
   size_y = _h;
}

sF32 _MinnieDrawable::getSizeX(void) {
   return size_x;
}

sF32 _MinnieDrawable::getSizeY(void) {
   return size_y;
}

void _MinnieDrawable::setBackgroundColor(sUI _c32) {
   c32_bg_color = _c32;
}

sUI _MinnieDrawable::getBackgroundColor(void) {
   return c32_bg_color;
}

void _MinnieDrawable::setScale2f(sF32 _sx, sF32 _sy) {
   scale_x = _sx;
   scale_y = _sy;
}

void _MinnieDrawable::setScaleX(sF32 _s) {
   scale_x = _s;
}

void _MinnieDrawable::setScaleY(sF32 _s) {
   scale_y = _s;
}

sF32 _MinnieDrawable::getScaleX(void) {
   return scale_x;
}

sF32 _MinnieDrawable::getScaleY(void) {
   return scale_y;
}

void _MinnieDrawable::setRotation(sF32 _a) {
   rot_ang = _a;
}

sF32 _MinnieDrawable::getRotation(void) {
   return rot_ang;
}

void _MinnieDrawable::setTranslate2f(sF32 _tx, sF32 _ty) {
   trans_x = _tx;
   trans_y = _ty;
}

void _MinnieDrawable::setTranslateX(sF32 _tx) {
   trans_x = _tx;
}

void _MinnieDrawable::setTranslateY(sF32 _ty) {
   trans_y = _ty;
}

sF32 _MinnieDrawable::getTranslateX(void) {
   return trans_x;
}

sF32 _MinnieDrawable::getTranslateY(void) {
   return trans_y;
}

void _MinnieDrawable::queueGLBufUpdate(void) {
   b_gl_buf_update_pending = YAC_TRUE;
}

sBool _MinnieDrawable::isComplete(void) {
   return (Dstream_get_offset(buf_gl) > 0u) && (Dstream_get_offset(buf_draw) > 0u);
}

void _MinnieDrawable::draw() {
   // Dyac_host_printf("xxx _MinnieDrawable::draw()\n");
   lazyAllocGL();

   if(0 != gl_buf_id && isComplete())
   {
      if(b_gl_buf_update_pending)
      {
         b_gl_buf_update_pending = false;
         Dsdvg_glcall(glBindBuffer(GL_ARRAY_BUFFER, gl_buf_id));
         Dsdvg_glcall(zglBufferSubData(GL_ARRAY_BUFFER, 0, Dstream_get_offset(buf_gl), buf_gl));
         Dsdvg_glcall(glBindBuffer(GL_ARRAY_BUFFER, 0));
      }

      // Parse draw-list
      sUI dlSize = Dstream_get_offset(buf_draw);
      Dstream_set_offset(buf_draw, 0u);
      Ddebug_draw_list_printf("[trc] ~~~~~~~~~~~~~~~~~~~~~~~~~~ draw-list");
      sUI numOpsPoly                = 0u;
      sUI numOpsTri                 = 0u;
      sUI numOpsTriTex              = 0u;
      sUI numOpsLineStrip           = 0u;
      sUI numOpsLineStripBevel      = 0u;
      sUI numOpsRectFill            = 0u;
      sUI numOpsRectStroke          = 0u;
      sUI numOpsRectFillStroke      = 0u;
      sUI numOpsRoundRectFill       = 0u;
      sUI numOpsRoundRectStroke     = 0u;
      sUI numOpsRoundRectFillStroke = 0u;
      sUI numOpsEllipseFill         = 0u;
      sUI numOpsEllipseStroke       = 0u;
      sUI numOpsEllipseFillStroke   = 0u;

      sUI dlTexId = 0;
      sBool dlTexRepeat;
      sBool dlTexFilter;
      sF32 dlTexDecalAlpha;

      while(Dstream_get_offset(buf_draw) < dlSize)
      {
         sU16 op = Dstream_read_i16(buf_draw);
         Ddebug_draw_list_printf("[trc] MinnieDrawable::draw: draw-list off=%u op=%u\n", (Dstream_get_offset(buf_draw)-2u), op);
         sF32 aaRange;
         sUI vbOff;
         sUI numVerts;
         sF32 r, g, b, a;
         sUI c32;
         sF32 strokeW;
         sUI c32Stroke;
         sF32 cx;
         sF32 cy;
         sF32 rx;
         sF32 ry;
         sF32 sx;
         sF32 sy;
         sUI vbOffBorder;
         sUI numVertsBorder;
         sUI glPrimType;

         switch(op)
         {
            default:
               Ddebug_draw_list_printf("[~~~] MinnieDrawable::draw: unhandled op %u\n", op);
               Dstream_set_offset(buf_draw, dlSize);
               break;

            case 0x00:  // end
               Dstream_set_offset(buf_draw, dlSize);
               break;

            case MINNIE_DRAWOP_TRIANGLES_FILL_FLAT_32:
               vbOff    = Dstream_read_i32(buf_draw);
               numVerts = Dstream_read_i32(buf_draw);
               r        = Dstream_read_f32(buf_draw);
               g        = Dstream_read_f32(buf_draw);
               b        = Dstream_read_f32(buf_draw);
               a        = Dstream_read_f32(buf_draw);
               Ddebug_draw_list_printf("[trc] MinnieDrawable::draw: draw-triangles-fill-flat<f32>: vbOff=%u numVerts=%u\n", vbOff, numVerts);
               sdvg_SetFillColor4f(r, g, b, a);
               sdvg_DrawTrianglesFillFlatVBO32(gl_buf_id,
                                               vbOff,
                                               numVerts
                                               );
               numOpsTri++;
               break;

            case MINNIE_DRAWOP_TRIANGLES_FILL_FLAT_14_2:
               vbOff    = Dstream_read_i32(buf_draw);
               numVerts = Dstream_read_i32(buf_draw);
               r        = Dstream_read_f32(buf_draw);
               g        = Dstream_read_f32(buf_draw);
               b        = Dstream_read_f32(buf_draw);
               a        = Dstream_read_f32(buf_draw);
               Ddebug_draw_list_printf("[trc] MinnieDrawable::draw: draw-triangles-fill-flat<14_2>: vbOff=%u numVerts=%u\n", vbOff, numVerts);
               sdvg_SetFillColor4f(r, g, b, a);
               sdvg_DrawTrianglesFillFlatVBO14_2(gl_buf_id,
                                                 vbOff,
                                                 numVerts
                                                 );
               numOpsTri++;
               break;

            case MINNIE_DRAWOP_TRIANGLES_FILL_GOURAUD_32:
               vbOff    = Dstream_read_i32(buf_draw);
               numVerts = Dstream_read_i32(buf_draw);
               Ddebug_draw_list_printf("[trc] MinnieDrawable::draw: draw-triangles-fill-gouraud<f32>: vbOff=%u numVerts=%u\n", vbOff, numVerts);
               sdvg_DrawTrianglesFillGouraudVBO32(gl_buf_id,
                                                  vbOff,
                                                  numVerts
                                                  );
               numOpsTri++;
               break;

            case MINNIE_DRAWOP_TRIANGLES_FILL_GOURAUD_14_2:
               vbOff    = Dstream_read_i32(buf_draw);
               numVerts = Dstream_read_i32(buf_draw);
               Ddebug_draw_list_printf("[trc] MinnieDrawable::draw: draw-triangles-fill-gouraud<s14.2>: vbOff=%u numVerts=%u\n", vbOff, numVerts);
               sdvg_DrawTrianglesFillGouraudVBO14_2(gl_buf_id,
                                                    vbOff,
                                                    numVerts
                                                    );
               numOpsTri++;
               break;

            case MINNIE_DRAWOP_TRIANGLES_FILL_GOURAUD_EDGEAA_32:
               vbOff    = Dstream_read_i32(buf_draw);
               numVerts = Dstream_read_i32(buf_draw);
               Ddebug_draw_list_printf("[trc] MinnieDrawable::draw: draw-triangles-fill-gouraud<f32_edgeaa>: vbOff=%u numVerts=%u\n", vbOff, numVerts);
               sdvg_DrawTrianglesFillGouraudEdgeAAVBO32(gl_buf_id,
                                                        vbOff,
                                                        numVerts
                                                        );
               numOpsTri++;
               break;

            case MINNIE_DRAWOP_TRIANGLES_FILL_GOURAUD_EDGEAA_14_2:
               vbOff    = Dstream_read_i32(buf_draw);
               numVerts = Dstream_read_i32(buf_draw);
               Ddebug_draw_list_printf("[trc] MinnieDrawable::draw: draw-triangles-fill-gouraud<s14.2_edgeaa>: vbOff=%u numVerts=%u\n", vbOff, numVerts);
               sdvg_DrawTrianglesFillGouraudEdgeAAVBO14_2(gl_buf_id,
                                                          vbOff,
                                                          numVerts
                                                          );
               numOpsTri++;
               break;

            case MINNIE_DRAWOP_POLYGON_FILL_FLAT_32:
               vbOff    = Dstream_read_i32(buf_draw);
               numVerts = Dstream_read_i32(buf_draw);
               r        = Dstream_read_f32(buf_draw);
               g        = Dstream_read_f32(buf_draw);
               b        = Dstream_read_f32(buf_draw);
               a        = Dstream_read_f32(buf_draw);
               Ddebug_draw_list_printf("[trc] MinnieDrawable::draw: draw-polygon-fill-flat<f32>: vbOff=%u numVerts=%u\n", vbOff, numVerts);
               sdvg_SetFillColor4f(r, g, b, a);
               sdvg_DrawPolygonFillFlatVBO32(gl_buf_id,
                                             vbOff,
                                             numVerts
                                             );
               numOpsPoly++;
               break;

            case MINNIE_DRAWOP_POLYGON_FILL_FLAT_14_2:
               vbOff    = Dstream_read_i32(buf_draw);
               numVerts = Dstream_read_i32(buf_draw);
               r        = Dstream_read_f32(buf_draw);
               g        = Dstream_read_f32(buf_draw);
               b        = Dstream_read_f32(buf_draw);
               a        = Dstream_read_f32(buf_draw);
               Ddebug_draw_list_printf("[trc] MinnieDrawable::draw: draw-polygon-fill-flat<14.2>: vbOff=%u numVerts=%u\n", vbOff, numVerts);
               sdvg_SetFillColor4f(r, g, b, a);
               sdvg_DrawPolygonFillFlatVBO14_2(gl_buf_id,
                                               vbOff,
                                               numVerts
                                               );
               numOpsPoly++;
               break;

            case MINNIE_DRAWOP_POLYGON_FILL_GOURAUD_32:
               vbOff    = Dstream_read_i32(buf_draw);
               numVerts = Dstream_read_i32(buf_draw);
               Ddebug_draw_list_printf("[trc] MinnieDrawable::draw: draw-polygon-fill-gouraud<f32>: vbOff=%u numVerts=%u\n", vbOff, numVerts);
               sdvg_DrawPolygonFillGouraudVBO32(gl_buf_id,
                                                vbOff,
                                                numVerts
                                                );
               numOpsPoly++;
               break;

            case MINNIE_DRAWOP_POLYGON_FILL_GOURAUD_14_2:
               vbOff    = Dstream_read_i32(buf_draw);
               numVerts = Dstream_read_i32(buf_draw);
               Ddebug_draw_list_printf("[trc] MinnieDrawable::draw: draw-polygon-fill-gouraud<s14.2>: vbOff=%u numVerts=%u\n", vbOff, numVerts);
               sdvg_DrawPolygonFillGouraudVBO14_2(gl_buf_id,
                                                  vbOff,
                                                  numVerts
                                                  );
               numOpsPoly++;
               break;

            case MINNIE_DRAWOP_RECT_FILL:
               cx             = Dstream_read_f32(buf_draw);  // center (_not_ top/left corner)
               cy             = Dstream_read_f32(buf_draw);
               rx             = Dstream_read_f32(buf_draw);  // radius, not size
               ry             = Dstream_read_f32(buf_draw);
               c32            = Dstream_read_i32(buf_draw);
               aaRange        = Dstream_read_i16(buf_draw) * (1.0f / 256);
               vbOff          = Dstream_read_i32(buf_draw);
               numVerts       = Dstream_read_i16(buf_draw);
               vbOffBorder    = Dstream_read_i32(buf_draw);
               numVertsBorder = Dstream_read_i16(buf_draw);
               glPrimType     = Dstream_read_i16(buf_draw);
               Ddebug_draw_list_printf("[trc] MinnieDrawable::draw: draw-rect-fill<f32>: vbOff=%u numVerts=%u numVertsBorder=%u\n", vbOff, numVerts, numVertsBorder);
               sdvg_SetFillColorARGB(c32);
               sdvg_SetAARange(aaRange);
               sdvg_DrawRectFillAAVBO32(gl_buf_id,
                                        vbOff,
                                        numVerts,
                                        vbOffBorder,
                                        numVertsBorder,
                                        glPrimType,
                                        cx, cy,
                                        rx, ry
                                        );
               numOpsRectFill++;
               break;

            case MINNIE_DRAWOP_RECT_STROKE:
               cx             = Dstream_read_f32(buf_draw);  // center (_not_ top/left corner)
               cy             = Dstream_read_f32(buf_draw);
               rx             = Dstream_read_f32(buf_draw);  // radius, not size
               ry             = Dstream_read_f32(buf_draw);
               c32Stroke      = Dstream_read_i32(buf_draw);
               strokeW        = Dstream_read_f32(buf_draw);
               aaRange        = Dstream_read_i16(buf_draw) * (1.0f / 256);
               vbOffBorder    = Dstream_read_i32(buf_draw);
               numVertsBorder = Dstream_read_i16(buf_draw);
               glPrimType     = Dstream_read_i16(buf_draw);
               Ddebug_draw_list_printf("[trc] MinnieDrawable::draw: draw-rect-stroke<f32>: vbOffBorder=%u numVertsBorder=%u\n", vbOffBorder, numVertsBorder);
               sdvg_SetStrokeColorARGB(c32Stroke);
               sdvg_SetStrokeWidth(strokeW);
               sdvg_SetAARange(aaRange);
               sdvg_DrawRectStrokeAAVBO32(gl_buf_id,
                                          vbOffBorder,
                                          numVertsBorder,
                                          glPrimType,
                                          cx, cy,
                                          rx, ry
                                          );
               numOpsRectStroke++;
               break;

            case MINNIE_DRAWOP_RECT_FILL_STROKE:
               cx             = Dstream_read_f32(buf_draw);  // center (_not_ top/left corner)
               cy             = Dstream_read_f32(buf_draw);
               rx             = Dstream_read_f32(buf_draw);  // radius, not size
               ry             = Dstream_read_f32(buf_draw);
               c32            = Dstream_read_i32(buf_draw);
               c32Stroke      = Dstream_read_i32(buf_draw);
               strokeW        = Dstream_read_f32(buf_draw);
               aaRange        = Dstream_read_i16(buf_draw) * (1.0f / 256);
               vbOff          = Dstream_read_i32(buf_draw);
               numVerts       = Dstream_read_i16(buf_draw);
               vbOffBorder    = Dstream_read_i32(buf_draw);
               numVertsBorder = Dstream_read_i16(buf_draw);
               glPrimType     = Dstream_read_i16(buf_draw);
               Ddebug_draw_list_printf("[trc] MinnieDrawable::draw: draw-rect-fill-stroke<f32>: vbOff=%u numVerts=%u vbOffBorder=%u numVertsBorder=%u\n", vbOff, numVerts, vbOffBorder, numVertsBorder);
               sdvg_SetFillColorARGB(c32);
               sdvg_SetStrokeColorARGB(c32Stroke);
               sdvg_SetStrokeWidth(strokeW);
               sdvg_SetAARange(aaRange);
               sdvg_DrawRectFillStrokeAAVBO32(gl_buf_id,
                                              vbOff,
                                              numVerts,
                                              vbOffBorder,
                                              numVertsBorder,
                                              glPrimType,
                                              cx, cy,
                                              rx, ry
                                              );
               numOpsRectFillStroke++;
               break;

            case MINNIE_DRAWOP_ELLIPSE_FILL:
               cx             = Dstream_read_f32(buf_draw);
               cy             = Dstream_read_f32(buf_draw);
               rx             = Dstream_read_f32(buf_draw);
               ry             = Dstream_read_f32(buf_draw);
               c32            = Dstream_read_i32(buf_draw);
               vbOff          = Dstream_read_i32(buf_draw);
               numVerts       = Dstream_read_i16(buf_draw);
               vbOffBorder    = Dstream_read_i32(buf_draw);
               numVertsBorder = Dstream_read_i16(buf_draw);
               glPrimType     = Dstream_read_i16(buf_draw);
               Ddebug_draw_list_printf("[trc] MinnieDrawable::draw: draw-ellipse-fill<f32>: vbOff=%u numVerts=%u vbOffBorder=%u numVertsBorder=%u\n", vbOff, numVerts, vbOffBorder, numVertsBorder);
               sdvg_SetFillColorARGB(c32);
               sdvg_DrawEllipseFillAAVBO32(gl_buf_id,
                                           vbOff,
                                           numVerts,
                                           vbOffBorder,
                                           numVertsBorder,
                                           glPrimType,
                                           cx, cy,
                                           rx, ry
                                           );
               numOpsEllipseFill++;
               break;

            case MINNIE_DRAWOP_ELLIPSE_STROKE:
               cx             = Dstream_read_f32(buf_draw);
               cy             = Dstream_read_f32(buf_draw);
               rx             = Dstream_read_f32(buf_draw);
               ry             = Dstream_read_f32(buf_draw);
               c32Stroke      = Dstream_read_i32(buf_draw);
               strokeW        = Dstream_read_f32(buf_draw);
               vbOffBorder    = Dstream_read_i32(buf_draw);
               numVertsBorder = Dstream_read_i16(buf_draw);
               glPrimType     = Dstream_read_i16(buf_draw);
               Ddebug_draw_list_printf("[trc] MinnieDrawable::draw: draw-ellipse-stroke<f32>: vbOffBorder=%u numVertsBorder=%u\n", vbOffBorder, numVertsBorder);
               sdvg_SetStrokeColorARGB(c32Stroke);
               sdvg_SetStrokeWidth(strokeW);
               sdvg_DrawEllipseStrokeAAVBO32(gl_buf_id,
                                             vbOffBorder,
                                             numVertsBorder,
                                             glPrimType,
                                             cx, cy,
                                             rx, ry
                                             );
               numOpsEllipseStroke++;
               break;

            case MINNIE_DRAWOP_ELLIPSE_FILL_STROKE:
               cx             = Dstream_read_f32(buf_draw);
               cy             = Dstream_read_f32(buf_draw);
               rx             = Dstream_read_f32(buf_draw);
               ry             = Dstream_read_f32(buf_draw);
               c32            = Dstream_read_i32(buf_draw);
               c32Stroke      = Dstream_read_i32(buf_draw);
               strokeW        = Dstream_read_f32(buf_draw);
               vbOff          = Dstream_read_i32(buf_draw);
               numVerts       = Dstream_read_i16(buf_draw);
               vbOffBorder    = Dstream_read_i32(buf_draw);
               numVertsBorder = Dstream_read_i16(buf_draw);
               glPrimType     = Dstream_read_i16(buf_draw);
               Ddebug_draw_list_printf("[trc] MinnieDrawable::draw: draw-ellipse-fill-stroke<f32>: vbOff=%u numVerts=%u vbOffBorder=%u numVertsBorder=%u\n", vbOff, numVerts, vbOffBorder, numVertsBorder);
               sdvg_SetFillColorARGB(c32);
               sdvg_SetStrokeColorARGB(c32Stroke);
               sdvg_SetStrokeWidth(strokeW);
               sdvg_DrawEllipseFillStrokeAAVBO32(gl_buf_id,
                                                 vbOff,
                                                 numVerts,
                                                 vbOffBorder,
                                                 numVertsBorder,
                                                 glPrimType,
                                                 cx, cy,
                                                 rx, ry
                                                 );
               numOpsEllipseFillStroke++;
               break;

            case MINNIE_DRAWOP_ROUNDRECT_FILL:
               cx             = Dstream_read_f32(buf_draw);  // center (_not_ top/left corner)
               cy             = Dstream_read_f32(buf_draw);
               sx             = Dstream_read_f32(buf_draw);  // rect size (center-size..center+size)
               sy             = Dstream_read_f32(buf_draw);
               rx             = Dstream_read_f32(buf_draw);  // corner radius
               ry             = Dstream_read_f32(buf_draw);
               c32            = Dstream_read_i32(buf_draw);
               aaRange        = Dstream_read_i16(buf_draw) * (1.0f / 256);
               vbOff          = Dstream_read_i32(buf_draw);
               numVerts       = Dstream_read_i16(buf_draw);
               vbOffBorder    = Dstream_read_i32(buf_draw);
               numVertsBorder = Dstream_read_i16(buf_draw);
               glPrimType     = Dstream_read_i16(buf_draw);
               Ddebug_draw_list_printf("[trc] MinnieDrawable::draw: draw-roundrect-fill<f32>: vbOff=%u numVerts=%u vbOffBorder=%u numVertsBorder=%u\n", vbOff, numVerts, vbOffBorder, numVertsBorder);
               sdvg_SetFillColorARGB(c32);
               sdvg_SetAARange(aaRange);
               sdvg_DrawRoundRectFillAAVBO32(gl_buf_id,
                                             vbOff,
                                             numVerts,
                                             vbOffBorder,
                                             numVertsBorder,
                                             glPrimType,
                                             cx, cy,
                                             sx, sy,
                                             rx, ry
                                             );
               numOpsRoundRectFill++;
               break;

            case MINNIE_DRAWOP_ROUNDRECT_STROKE:
               cx             = Dstream_read_f32(buf_draw);  // center (_not_ top/left corner)
               cy             = Dstream_read_f32(buf_draw);
               sx             = Dstream_read_f32(buf_draw);  // rect size (center-size..center+size)
               sy             = Dstream_read_f32(buf_draw);
               rx             = Dstream_read_f32(buf_draw);  // corner radius
               ry             = Dstream_read_f32(buf_draw);
               c32Stroke      = Dstream_read_i32(buf_draw);
               strokeW        = Dstream_read_f32(buf_draw);
               aaRange        = Dstream_read_i16(buf_draw) * (1.0f / 256);
               vbOffBorder    = Dstream_read_i32(buf_draw);
               numVertsBorder = Dstream_read_i16(buf_draw);
               glPrimType     = Dstream_read_i16(buf_draw);
               Ddebug_draw_list_printf("[trc] MinnieDrawable::draw: draw-roundrect-stroke<f32>: vbOffBorder=%u numVertsBorder=%u\n", vbOffBorder, numVertsBorder);
               sdvg_SetStrokeColorARGB(c32Stroke);
               sdvg_SetStrokeWidth(strokeW);
               sdvg_SetAARange(aaRange);
               sdvg_DrawRoundRectStrokeAAVBO32(gl_buf_id,
                                               vbOffBorder,
                                               numVertsBorder,
                                               glPrimType,
                                               cx, cy,
                                               sx, sy,
                                               rx, ry
                                               );
               numOpsRoundRectStroke++;
               break;

            case MINNIE_DRAWOP_ROUNDRECT_FILL_STROKE:
               cx             = Dstream_read_f32(buf_draw);  // center (_not_ top/left corner)
               cy             = Dstream_read_f32(buf_draw);
               sx             = Dstream_read_f32(buf_draw);  // rect size (center-size..center+size)
               sy             = Dstream_read_f32(buf_draw);
               rx             = Dstream_read_f32(buf_draw);  // corner radius
               ry             = Dstream_read_f32(buf_draw);
               c32            = Dstream_read_i32(buf_draw);
               c32Stroke      = Dstream_read_i32(buf_draw);
               strokeW        = Dstream_read_f32(buf_draw);
               aaRange        = Dstream_read_i16(buf_draw) * (1.0f / 256);
               vbOff          = Dstream_read_i32(buf_draw);
               numVerts       = Dstream_read_i16(buf_draw);
               vbOffBorder    = Dstream_read_i32(buf_draw);
               numVertsBorder = Dstream_read_i16(buf_draw);
               glPrimType     = Dstream_read_i16(buf_draw);
               Ddebug_draw_list_printf("[trc] MinnieDrawable::draw: draw-roundrect-fill-stroke<f32>: vbOff=%u numVerts=%u vbOffBorder=%u numVertsBorder=%u\n", vbOff, numVerts, vbOffBorder, numVertsBorder);
               sdvg_SetFillColorARGB(c32);
               sdvg_SetStrokeColorARGB(c32Stroke);
               sdvg_SetStrokeWidth(strokeW);
               sdvg_SetAARange(aaRange);
               sdvg_DrawRoundRectFillStrokeAAVBO32(gl_buf_id,
                                                   vbOff,
                                                   numVerts,
                                                   vbOffBorder,
                                                   numVertsBorder,
                                                   glPrimType,
                                                   cx, cy,
                                                   sx, sy,
                                                   rx, ry
                                                   );
               numOpsRoundRectFillStroke++;
               break;

            case MINNIE_DRAWOP_BIND_TEXTURE:
               dlTexId     = Dstream_read_i32(buf_draw);
               dlTexRepeat = Dstream_read_i8(buf_draw);
               dlTexFilter = Dstream_read_i8(buf_draw);
               Ddebug_draw_list_printf("[trc] MinnieDrawable::draw: bind-texture: texId=%u texRep=%d texFlt=%d\n", dlTexId, dlTexRepeat, dlTexFilter);
               sdvg_BindTexture2D(dlTexId, dlTexRepeat, dlTexFilter);
               break;

            case MINNIE_DRAWOP_UNBIND_TEXTURE:
               dlTexId = 0;
               Ddebug_draw_list_printf("[trc] MinnieDrawable::draw: unbind-texture\n");
               sdvg_UnbindTexture2D();
               break;

            case MINNIE_DRAWOP_TEXTURE_DECAL_ALPHA:
               dlTexDecalAlpha = Dstream_read_f32(buf_draw);
               Ddebug_draw_list_printf("[trc] MinnieDrawable::draw: texture-decal-alpha: decalAlpha=%f\n", dlTexDecalAlpha);
               sdvg_SetTextureDecalAlpha(dlTexDecalAlpha);
               break;

            case MINNIE_DRAWOP_TRIANGLES_TEX_UV_FLAT_32:
               vbOff    = Dstream_read_i32(buf_draw);
               numVerts = Dstream_read_i32(buf_draw);
               c32      = Dstream_read_i32(buf_draw);
               Ddebug_draw_list_printf("[trc] MinnieDrawable::draw: draw-tri-tex-uv-flat<f32>: vbOff=%u numVerts=%u texId=%u texRep=%d texFlt=%d\n", vbOff, numVerts, dlTexId, dlTexRepeat, dlTexFilter);
               sdvg_SetFillColorARGB(c32);
               sdvg_DrawTrianglesTexUVFlatVBO32(gl_buf_id,
                                                vbOff,
                                                numVerts
                                                );
               numOpsTriTex++;
               break;

            case MINNIE_DRAWOP_TRIANGLES_TEX_UV_FLAT_DECAL_32:
               vbOff    = Dstream_read_i32(buf_draw);
               numVerts = Dstream_read_i32(buf_draw);
               c32      = Dstream_read_i32(buf_draw);
               Ddebug_draw_list_printf("[trc] MinnieDrawable::draw: draw-tri-tex-uv-flat-decal<f32>: vbOff=%u numVerts=%u texId=%u texRep=%d texFlt=%d\n", vbOff, numVerts, dlTexId, dlTexRepeat, dlTexFilter);
               sdvg_SetFillColorARGB(c32);
               sdvg_DrawTrianglesTexUVFlatDecalVBO32(gl_buf_id,
                                                     vbOff,
                                                     numVerts
                                                     );
               numOpsTriTex++;
               break;

            case MINNIE_DRAWOP_TRIANGLES_TEX_UV_GOURAUD_32:
               vbOff    = Dstream_read_i32(buf_draw);
               numVerts = Dstream_read_i32(buf_draw);
               c32      = Dstream_read_i32(buf_draw);
               Ddebug_draw_list_printf("[trc] MinnieDrawable::draw: draw-tri-tex-uv-gouraud<f32>: vbOff=%u numVerts=%u texId=%u texRep=%d texFlt=%d\n", vbOff, numVerts, dlTexId, dlTexRepeat, dlTexFilter);
               sdvg_SetFillColorARGB(c32);
               sdvg_DrawTrianglesTexUVGouraudVBO32(gl_buf_id,
                                                   vbOff,
                                                   numVerts
                                                   );
               numOpsTriTex++;
               break;

            case MINNIE_DRAWOP_TRIANGLES_TEX_UV_GOURAUD_DECAL_32:
               vbOff   = Dstream_read_i32(buf_draw);
               numVerts = Dstream_read_i32(buf_draw);
               c32     = Dstream_read_i32(buf_draw);
               Ddebug_draw_list_printf("[trc] MinnieDrawable::draw: draw-tri-tex-uv-gouraud-decal<f32>: vbOff=%u numVerts=%u texId=%u texRep=%d texFlt=%d\n", vbOff, numVerts, dlTexId, dlTexRepeat, dlTexFilter);
               sdvg_SetFillColorARGB(c32);
               sdvg_DrawTrianglesTexUVGouraudDecalVBO32(gl_buf_id,
                                                        vbOff,
                                                        numVerts
                                                        );
               numOpsTriTex++;
               break;

            case MINNIE_DRAWOP_LINE_STRIP_FLAT_14_2:
               vbOff    = Dstream_read_i32(buf_draw);
               numVerts = Dstream_read_i32(buf_draw);
               c32      = Dstream_read_i32(buf_draw);
               strokeW  = Dstream_read_f32(buf_draw);
               Ddebug_draw_list_printf("[trc] MinnieDrawable::draw: draw-line-strip-flat<s14.2>: vbOff=%u numVerts=%u strokeW=%f\n", vbOff, numVerts, strokeW);
               sdvg_SetStrokeColorARGB(c32);
               sdvg_SetStrokeWidth(strokeW);
               sdvg_DrawLineStripFlatAAVBO14_2(gl_buf_id,
                                               vbOff,
                                               numVerts/*numPoints*/
                                               );
               numOpsLineStrip++;
               break;

            case MINNIE_DRAWOP_LINE_STRIP_FLAT_BEVEL_14_2:
               vbOff    = Dstream_read_i32(buf_draw);
               numVerts = Dstream_read_i32(buf_draw);
               c32      = Dstream_read_i32(buf_draw);
               strokeW  = Dstream_read_f32(buf_draw);
               Ddebug_draw_list_printf("[trc] MinnieDrawable::draw: draw-line-strip-flat-bevel<s14.2>: vbOff=%u numVerts=%u strokeW=%f\n", vbOff, numVerts, strokeW);
               sdvg_SetStrokeColorARGB(c32);
               sdvg_SetStrokeWidth(strokeW);
               sdvg_DrawLineStripFlatBevelAAVBO14_2(gl_buf_id,
                                                    vbOff,
                                                    numVerts/*numPoints*/
                                                    );
               numOpsLineStripBevel++;
               break;

         } // switch op
      } // iterate draw ops

      if(b_debug)
      {
         Dprintf("[dbg] MinnieDrawable::draw: #Tri=%u #Poly=%u #TriTex=%u #LineStrip=%u #LineStripBevel=%u #RectFill=%u #RectStroke=%u #RectFillStroke=%u #EllipseFill=%u #EllipseStroke=%u #EllipseFillStroke=%u #RoundRectFill=%u #RoundRectStroke=%u #RoundRectFillStroke=%u dl-size=%u\n", numOpsTri, numOpsPoly, numOpsTriTex, numOpsLineStrip, numOpsLineStripBevel, numOpsRectFill, numOpsRectStroke, numOpsRectFillStroke, numOpsEllipseFill, numOpsEllipseStroke, numOpsEllipseFillStroke, numOpsRoundRectFill, numOpsRoundRectStroke, numOpsRoundRectFillStroke, Dstream_get_offset(buf_draw));
      }
   }
}

#endif // USE_MINNIE_MIB_SETUP
