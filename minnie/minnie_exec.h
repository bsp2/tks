// ----
// ---- file   : minnie_exec.h
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
// ---- info   : "minnie" draw-list renderer
// ----
// ----
// ----

#define Ddebug_draw_list_printf       if(0);else Dprintf
#define Ddebug_draw_list_printfv      if(1);else Dprintf
#define Ddebug_draw_list_errorprintfv if(0);else Dprintf

/* @function minExecDrawListEx,Buffer bufDraw,int glBufId,boolean bDebug
Execute draw-list
@arg bufDraw Draw-list buffer (offset determines size) (in system RAM)
@arg glBufId Vertex buffer id (in video RAM)
@arg bDebug true=print draw command statistics

@groupref Draw
*/
void minExecDrawListEx(YAC_Buffer *_bufDraw, sUI _glBufId, sBool _bDebug) {

#ifdef SHADERVG_SCRIPT_API
   if(!YAC_Is_Buffer(_bufDraw))
   {
      Dprintf("[---] minExecDrawListEx: bufDraw is not a valid Buffer\n");
      return;
   }
#endif // SHADERVG_SCRIPT_API

   // Parse draw-list
   sUI dlSize = Dstream_get_offset(_bufDraw);
   Dstream_set_offset(_bufDraw, 0u);
   Ddebug_draw_list_printfv("[trc] ~~~~~~~~~~~~~~~~~~~~~~~~~~ draw-list dlSize=%u\n", dlSize);
   sUI numOpsPoly                = 0u;
   sUI numOpsTri                 = 0u;
   sUI numOpsTriTex              = 0u;
   sUI numOpsLineStrip           = 0u;
   sUI numOpsLineStripBevel      = 0u;
   sUI numOpsLineStripMiter      = 0u;
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

   while(Dstream_get_offset(_bufDraw) < dlSize)
   {
      sU16 op = Dstream_read_i16(_bufDraw);
      Ddebug_draw_list_printfv("[trc] minExecDrawList: draw-list off=%u op=%u\n", (Dstream_get_offset(_bufDraw)-2u), op);
      sF32 aaRange;
      sUI vbOff;
      sUI numVerts;
      sF32 r, g, b, a;
      sUI c32;
      sF32 strokeW;
      sF32 miterLimit;
      sUI c32Stroke;
      sU8 flags;
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
            Ddebug_draw_list_errorprintfv("[~~~] minExecDrawList: unhandled op %u\n", op);
            Dstream_set_offset(_bufDraw, dlSize);
            break;

         case 0x00:  // end
            Dstream_set_offset(_bufDraw, dlSize);
            break;

         case MINNIE_DRAWOP_TRIANGLES_FILL_FLAT_UNIFORM_32:
            vbOff    = Dstream_read_i32(_bufDraw);
            numVerts = Dstream_read_i32(_bufDraw);
            r        = Dstream_read_f32(_bufDraw);
            g        = Dstream_read_f32(_bufDraw);
            b        = Dstream_read_f32(_bufDraw);
            a        = Dstream_read_f32(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: draw-triangles-fill-flat-uniform<f32>: vbOff=%u numVerts=%u\n", vbOff, numVerts);
            sdvg_SetFillColor4f(r, g, b, a);
            sdvg_DrawTrianglesFillFlatUniformVBO32(_glBufId,
                                                   vbOff,
                                                   numVerts
                                                   );
            numOpsTri++;
            break;

         case MINNIE_DRAWOP_TRIANGLES_FILL_FLAT_UNIFORM_14_2:
            vbOff    = Dstream_read_i32(_bufDraw);
            numVerts = Dstream_read_i32(_bufDraw);
            r        = Dstream_read_f32(_bufDraw);
            g        = Dstream_read_f32(_bufDraw);
            b        = Dstream_read_f32(_bufDraw);
            a        = Dstream_read_f32(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: draw-triangles-fill-flat-uniform<14_2>: vbOff=%u numVerts=%u\n", vbOff, numVerts);
            sdvg_SetFillColor4f(r, g, b, a);
            sdvg_DrawTrianglesFillFlatUniformVBO14_2(_glBufId,
                                                     vbOff,
                                                     numVerts
                                                     );
            numOpsTri++;
            break;

         case MINNIE_DRAWOP_TRIANGLES_FILL_FLAT_32:
            vbOff    = Dstream_read_i32(_bufDraw);
            numVerts = Dstream_read_i32(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: draw-triangles-fill-flat<f32>: vbOff=%u numVerts=%u\n", vbOff, numVerts);
            sdvg_DrawTrianglesFillFlatVBO32(_glBufId,
                                            vbOff,
                                            numVerts
                                            );
            numOpsTri++;
            break;

         case MINNIE_DRAWOP_TRIANGLES_FILL_FLAT_14_2:
            vbOff    = Dstream_read_i32(_bufDraw);
            numVerts = Dstream_read_i32(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: draw-triangles-fill-flat<s14.2>: vbOff=%u numVerts=%u\n", vbOff, numVerts);
            sdvg_DrawTrianglesFillFlatVBO14_2(_glBufId,
                                              vbOff,
                                              numVerts
                                              );
            numOpsTri++;
            break;

         case MINNIE_DRAWOP_TRIANGLES_FILL_GOURAUD_32:
            vbOff    = Dstream_read_i32(_bufDraw);
            numVerts = Dstream_read_i32(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: draw-triangles-fill-gouraud<f32>: vbOff=%u numVerts=%u\n", vbOff, numVerts);
            sdvg_DrawTrianglesFillGouraudVBO32(_glBufId,
                                               vbOff,
                                               numVerts
                                               );
            numOpsTri++;
            break;

         case MINNIE_DRAWOP_TRIANGLES_FILL_GOURAUD_14_2:
            vbOff    = Dstream_read_i32(_bufDraw);
            numVerts = Dstream_read_i32(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: draw-triangles-fill-gouraud<s14.2>: vbOff=%u numVerts=%u\n", vbOff, numVerts);
            sdvg_DrawTrianglesFillGouraudVBO14_2(_glBufId,
                                                 vbOff,
                                                 numVerts
                                                 );
            numOpsTri++;
            break;

         case MINNIE_DRAWOP_TRIANGLES_FILL_GOURAUD_EDGEAA_32:
            vbOff    = Dstream_read_i32(_bufDraw);
            numVerts = Dstream_read_i32(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: draw-triangles-fill-gouraud<f32_edgeaa>: vbOff=%u numVerts=%u\n", vbOff, numVerts);
            sdvg_DrawTrianglesFillGouraudEdgeAAVBO32(_glBufId,
                                                     vbOff,
                                                     numVerts
                                                     );
            numOpsTri++;
            break;

         case MINNIE_DRAWOP_TRIANGLES_FILL_GOURAUD_EDGEAA_14_2:
            vbOff    = Dstream_read_i32(_bufDraw);
            numVerts = Dstream_read_i32(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: draw-triangles-fill-gouraud<s14.2_edgeaa>: vbOff=%u numVerts=%u\n", vbOff, numVerts);
            sdvg_DrawTrianglesFillGouraudEdgeAAVBO14_2(_glBufId,
                                                       vbOff,
                                                       numVerts
                                                       );
            numOpsTri++;
            break;

         case MINNIE_DRAWOP_POLYGON_FILL_FLAT_UNIFORM_32:
            vbOff    = Dstream_read_i32(_bufDraw);
            numVerts = Dstream_read_i32(_bufDraw);
            r        = Dstream_read_f32(_bufDraw);
            g        = Dstream_read_f32(_bufDraw);
            b        = Dstream_read_f32(_bufDraw);
            a        = Dstream_read_f32(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: draw-polygon-fill-flat<f32>: vbOff=%u numVerts=%u rgba=(%f;%f;%f;%f)\n", vbOff, numVerts, r,g,b,a);
            sdvg_SetFillColor4f(r, g, b, a);
            sdvg_DrawPolygonFillFlatUniformVBO32(_glBufId,
                                                 vbOff,
                                                 numVerts
                                                 );
            numOpsPoly++;
            break;

         case MINNIE_DRAWOP_POLYGON_FILL_FLAT_UNIFORM_14_2:
            vbOff    = Dstream_read_i32(_bufDraw);
            numVerts = Dstream_read_i32(_bufDraw);
            r        = Dstream_read_f32(_bufDraw);
            g        = Dstream_read_f32(_bufDraw);
            b        = Dstream_read_f32(_bufDraw);
            a        = Dstream_read_f32(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: draw-polygon-fill-flat<14.2>: vbOff=%u numVerts=%u rgba=(%f;%f;%f;%f)\n", vbOff, numVerts, r,g,b,a);
            sdvg_SetFillColor4f(r, g, b, a);
            sdvg_DrawPolygonFillFlatUniformVBO14_2(_glBufId,
                                                   vbOff,
                                                   numVerts
                                                   );
            numOpsPoly++;
            break;

         case MINNIE_DRAWOP_RECT_FILL:
            cx             = Dstream_read_f32(_bufDraw);  // center (_not_ top/left corner)
            cy             = Dstream_read_f32(_bufDraw);
            rx             = Dstream_read_f32(_bufDraw);  // radius, not size
            ry             = Dstream_read_f32(_bufDraw);
            c32            = Dstream_read_i32(_bufDraw);
            aaRange        = Dstream_read_i16(_bufDraw) * (1.0f / 256);
            vbOff          = Dstream_read_i32(_bufDraw);
            numVerts       = Dstream_read_i16(_bufDraw);
            vbOffBorder    = Dstream_read_i32(_bufDraw);
            numVertsBorder = Dstream_read_i16(_bufDraw);
            glPrimType     = Dstream_read_i16(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: draw-rect-fill<f32>: vbOff=%u numVerts=%u numVertsBorder=%u\n", vbOff, numVerts, numVertsBorder);
            sdvg_SetFillColorARGB(c32);
            sdvg_SetAARange(aaRange);
            sdvg_DrawRectFillAAVBO32(_glBufId,
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
            cx             = Dstream_read_f32(_bufDraw);  // center (_not_ top/left corner)
            cy             = Dstream_read_f32(_bufDraw);
            rx             = Dstream_read_f32(_bufDraw);  // radius, not size
            ry             = Dstream_read_f32(_bufDraw);
            c32Stroke      = Dstream_read_i32(_bufDraw);
            strokeW        = Dstream_read_f32(_bufDraw);
            aaRange        = Dstream_read_i16(_bufDraw) * (1.0f / 256);
            vbOffBorder    = Dstream_read_i32(_bufDraw);
            numVertsBorder = Dstream_read_i16(_bufDraw);
            glPrimType     = Dstream_read_i16(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: draw-rect-stroke<f32>: vbOffBorder=%u numVertsBorder=%u\n", vbOffBorder, numVertsBorder);
            sdvg_SetStrokeColorARGB(c32Stroke);
            sdvg_SetStrokeRadius(strokeW);
            sdvg_SetAARange(aaRange);
            sdvg_DrawRectStrokeAAVBO32(_glBufId,
                                       vbOffBorder,
                                       numVertsBorder,
                                       glPrimType,
                                       cx, cy,
                                       rx, ry
                                       );
            numOpsRectStroke++;
            break;

         case MINNIE_DRAWOP_RECT_FILL_STROKE:
            cx             = Dstream_read_f32(_bufDraw);  // center (_not_ top/left corner)
            cy             = Dstream_read_f32(_bufDraw);
            rx             = Dstream_read_f32(_bufDraw);  // radius, not size
            ry             = Dstream_read_f32(_bufDraw);
            c32            = Dstream_read_i32(_bufDraw);
            c32Stroke      = Dstream_read_i32(_bufDraw);
            strokeW        = Dstream_read_f32(_bufDraw);
            aaRange        = Dstream_read_i16(_bufDraw) * (1.0f / 256);
            vbOff          = Dstream_read_i32(_bufDraw);
            numVerts       = Dstream_read_i16(_bufDraw);
            vbOffBorder    = Dstream_read_i32(_bufDraw);
            numVertsBorder = Dstream_read_i16(_bufDraw);
            glPrimType     = Dstream_read_i16(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: draw-rect-fill-stroke<f32>: vbOff=%u numVerts=%u vbOffBorder=%u numVertsBorder=%u\n", vbOff, numVerts, vbOffBorder, numVertsBorder);
            sdvg_SetFillColorARGB(c32);
            sdvg_SetStrokeColorARGB(c32Stroke);
            sdvg_SetStrokeRadius(strokeW);
            sdvg_SetAARange(aaRange);
            sdvg_DrawRectFillStrokeAAVBO32(_glBufId,
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
            cx             = Dstream_read_f32(_bufDraw);
            cy             = Dstream_read_f32(_bufDraw);
            rx             = Dstream_read_f32(_bufDraw);
            ry             = Dstream_read_f32(_bufDraw);
            c32            = Dstream_read_i32(_bufDraw);
            vbOff          = Dstream_read_i32(_bufDraw);
            numVerts       = Dstream_read_i16(_bufDraw);
            vbOffBorder    = Dstream_read_i32(_bufDraw);
            numVertsBorder = Dstream_read_i16(_bufDraw);
            glPrimType     = Dstream_read_i16(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: draw-ellipse-fill<f32>: vbOff=%u numVerts=%u vbOffBorder=%u numVertsBorder=%u\n", vbOff, numVerts, vbOffBorder, numVertsBorder);
            sdvg_SetFillColorARGB(c32);
            sdvg_DrawEllipseFillAAVBO32(_glBufId,
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
            cx             = Dstream_read_f32(_bufDraw);
            cy             = Dstream_read_f32(_bufDraw);
            rx             = Dstream_read_f32(_bufDraw);
            ry             = Dstream_read_f32(_bufDraw);
            c32Stroke      = Dstream_read_i32(_bufDraw);
            strokeW        = Dstream_read_f32(_bufDraw);
            vbOffBorder    = Dstream_read_i32(_bufDraw);
            numVertsBorder = Dstream_read_i16(_bufDraw);
            glPrimType     = Dstream_read_i16(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: draw-ellipse-stroke<f32>: vbOffBorder=%u numVertsBorder=%u\n", vbOffBorder, numVertsBorder);
            sdvg_SetStrokeColorARGB(c32Stroke);
            sdvg_SetStrokeRadius(strokeW);
            sdvg_DrawEllipseStrokeAAVBO32(_glBufId,
                                          vbOffBorder,
                                          numVertsBorder,
                                          glPrimType,
                                          cx, cy,
                                          rx, ry
                                          );
            numOpsEllipseStroke++;
            break;

         case MINNIE_DRAWOP_ELLIPSE_FILL_STROKE:
            cx             = Dstream_read_f32(_bufDraw);
            cy             = Dstream_read_f32(_bufDraw);
            rx             = Dstream_read_f32(_bufDraw);
            ry             = Dstream_read_f32(_bufDraw);
            c32            = Dstream_read_i32(_bufDraw);
            c32Stroke      = Dstream_read_i32(_bufDraw);
            strokeW        = Dstream_read_f32(_bufDraw);
            vbOff          = Dstream_read_i32(_bufDraw);
            numVerts       = Dstream_read_i16(_bufDraw);
            vbOffBorder    = Dstream_read_i32(_bufDraw);
            numVertsBorder = Dstream_read_i16(_bufDraw);
            glPrimType     = Dstream_read_i16(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: draw-ellipse-fill-stroke<f32>: vbOff=%u numVerts=%u vbOffBorder=%u numVertsBorder=%u\n", vbOff, numVerts, vbOffBorder, numVertsBorder);
            sdvg_SetFillColorARGB(c32);
            sdvg_SetStrokeColorARGB(c32Stroke);
            sdvg_SetStrokeRadius(strokeW);
            sdvg_DrawEllipseFillStrokeAAVBO32(_glBufId,
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
            cx             = Dstream_read_f32(_bufDraw);  // center (_not_ top/left corner)
            cy             = Dstream_read_f32(_bufDraw);
            sx             = Dstream_read_f32(_bufDraw);  // rect size (center-size..center+size)
            sy             = Dstream_read_f32(_bufDraw);
            rx             = Dstream_read_f32(_bufDraw);  // corner radius
            ry             = Dstream_read_f32(_bufDraw);
            c32            = Dstream_read_i32(_bufDraw);
            aaRange        = Dstream_read_i16(_bufDraw) * (1.0f / 256);
            vbOff          = Dstream_read_i32(_bufDraw);
            numVerts       = Dstream_read_i16(_bufDraw);
            vbOffBorder    = Dstream_read_i32(_bufDraw);
            numVertsBorder = Dstream_read_i16(_bufDraw);
            glPrimType     = Dstream_read_i16(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: draw-roundrect-fill<f32>: vbOff=%u numVerts=%u vbOffBorder=%u numVertsBorder=%u\n", vbOff, numVerts, vbOffBorder, numVertsBorder);
            sdvg_SetFillColorARGB(c32);
            sdvg_SetAARange(aaRange);
            sdvg_DrawRoundRectFillAAVBO32(_glBufId,
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
            cx             = Dstream_read_f32(_bufDraw);  // center (_not_ top/left corner)
            cy             = Dstream_read_f32(_bufDraw);
            sx             = Dstream_read_f32(_bufDraw);  // rect size (center-size..center+size)
            sy             = Dstream_read_f32(_bufDraw);
            rx             = Dstream_read_f32(_bufDraw);  // corner radius
            ry             = Dstream_read_f32(_bufDraw);
            c32Stroke      = Dstream_read_i32(_bufDraw);
            strokeW        = Dstream_read_f32(_bufDraw);
            aaRange        = Dstream_read_i16(_bufDraw) * (1.0f / 256);
            vbOffBorder    = Dstream_read_i32(_bufDraw);
            numVertsBorder = Dstream_read_i16(_bufDraw);
            glPrimType     = Dstream_read_i16(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: draw-roundrect-stroke<f32>: vbOffBorder=%u numVertsBorder=%u\n", vbOffBorder, numVertsBorder);
            sdvg_SetStrokeColorARGB(c32Stroke);
            sdvg_SetStrokeRadius(strokeW);
            sdvg_SetAARange(aaRange);
            sdvg_DrawRoundRectStrokeAAVBO32(_glBufId,
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
            cx             = Dstream_read_f32(_bufDraw);  // center (_not_ top/left corner)
            cy             = Dstream_read_f32(_bufDraw);
            sx             = Dstream_read_f32(_bufDraw);  // rect size (center-size..center+size)
            sy             = Dstream_read_f32(_bufDraw);
            rx             = Dstream_read_f32(_bufDraw);  // corner radius
            ry             = Dstream_read_f32(_bufDraw);
            c32            = Dstream_read_i32(_bufDraw);
            c32Stroke      = Dstream_read_i32(_bufDraw);
            strokeW        = Dstream_read_f32(_bufDraw);
            aaRange        = Dstream_read_i16(_bufDraw) * (1.0f / 256);
            vbOff          = Dstream_read_i32(_bufDraw);
            numVerts       = Dstream_read_i16(_bufDraw);
            vbOffBorder    = Dstream_read_i32(_bufDraw);
            numVertsBorder = Dstream_read_i16(_bufDraw);
            glPrimType     = Dstream_read_i16(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: draw-roundrect-fill-stroke<f32>: vbOff=%u numVerts=%u vbOffBorder=%u numVertsBorder=%u\n", vbOff, numVerts, vbOffBorder, numVertsBorder);
            sdvg_SetFillColorARGB(c32);
            sdvg_SetStrokeColorARGB(c32Stroke);
            sdvg_SetStrokeRadius(strokeW);
            sdvg_SetAARange(aaRange);
            sdvg_DrawRoundRectFillStrokeAAVBO32(_glBufId,
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
            dlTexId     = Dstream_read_i32(_bufDraw);
            dlTexRepeat = Dstream_read_i8(_bufDraw);
            dlTexFilter = Dstream_read_i8(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: bind-texture: texId=%u texRep=%d texFlt=%d\n", dlTexId, dlTexRepeat, dlTexFilter);
            sdvg_BindTexture2D(dlTexId, dlTexRepeat, dlTexFilter);
            break;

         case MINNIE_DRAWOP_UNBIND_TEXTURE:
            dlTexId = 0;
            Ddebug_draw_list_printfv("[trc] minExecDrawList: unbind-texture\n");
            sdvg_UnbindTexture2D();
            break;

         case MINNIE_DRAWOP_TEXTURE_DECAL_ALPHA:
            dlTexDecalAlpha = Dstream_read_f32(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: texture-decal-alpha: decalAlpha=%f\n", dlTexDecalAlpha);
            sdvg_SetTextureDecalAlpha(dlTexDecalAlpha);
            break;

         case MINNIE_DRAWOP_TRIANGLES_TEX_UV_FLAT_32:
            vbOff    = Dstream_read_i32(_bufDraw);
            numVerts = Dstream_read_i32(_bufDraw);
            c32      = Dstream_read_i32(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: draw-tri-tex-uv-flat<f32>: vbOff=%u numVerts=%u texId=%u texRep=%d texFlt=%d c32=#%08x\n", vbOff, numVerts, dlTexId, dlTexRepeat, dlTexFilter, c32);
            sdvg_SetFillColorARGB(c32);
            sdvg_DrawTrianglesTexUVFlatVBO32(_glBufId,
                                             vbOff,
                                             numVerts
                                             );
            numOpsTriTex++;
            break;

         case MINNIE_DRAWOP_TRIANGLES_TEX_UV_FLAT_DECAL_32:
            vbOff    = Dstream_read_i32(_bufDraw);
            numVerts = Dstream_read_i32(_bufDraw);
            c32      = Dstream_read_i32(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: draw-tri-tex-uv-flat-decal<f32>: vbOff=%u numVerts=%u texId=%u texRep=%d texFlt=%d\n", vbOff, numVerts, dlTexId, dlTexRepeat, dlTexFilter);
            sdvg_SetFillColorARGB(c32);
            sdvg_DrawTrianglesTexUVFlatDecalVBO32(_glBufId,
                                                  vbOff,
                                                  numVerts
                                                  );
            numOpsTriTex++;
            break;

         case MINNIE_DRAWOP_TRIANGLES_TEX_UV_GOURAUD_32:
            vbOff    = Dstream_read_i32(_bufDraw);
            numVerts = Dstream_read_i32(_bufDraw);
            c32      = Dstream_read_i32(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: draw-tri-tex-uv-gouraud<f32>: vbOff=%u numVerts=%u texId=%u texRep=%d texFlt=%d\n", vbOff, numVerts, dlTexId, dlTexRepeat, dlTexFilter);
            sdvg_SetFillColorARGB(c32);
            sdvg_DrawTrianglesTexUVGouraudVBO32(_glBufId,
                                                vbOff,
                                                numVerts
                                                );
            numOpsTriTex++;
            break;

         case MINNIE_DRAWOP_TRIANGLES_TEX_UV_GOURAUD_DECAL_32:
            vbOff   = Dstream_read_i32(_bufDraw);
            numVerts = Dstream_read_i32(_bufDraw);
            c32     = Dstream_read_i32(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: draw-tri-tex-uv-gouraud-decal<f32>: vbOff=%u numVerts=%u texId=%u texRep=%d texFlt=%d\n", vbOff, numVerts, dlTexId, dlTexRepeat, dlTexFilter);
            sdvg_SetFillColorARGB(c32);
            sdvg_DrawTrianglesTexUVGouraudDecalVBO32(_glBufId,
                                                     vbOff,
                                                     numVerts
                                                     );
            numOpsTriTex++;
            break;

         case MINNIE_DRAWOP_LINE_STRIP_FLAT_14_2:
            vbOff    = Dstream_read_i32(_bufDraw);
            numVerts = Dstream_read_i32(_bufDraw);
            c32      = Dstream_read_i32(_bufDraw);
            strokeW  = Dstream_read_f32(_bufDraw);
            flags    = Dstream_read_i8(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: draw-line-strip-flat<s14.2>: vbOff=%u numVerts=%u strokeW=%f\n", vbOff, numVerts, strokeW);
            sdvg_SetStrokeColorARGB(c32);
            sdvg_SetStrokeRadius(strokeW);
            sdvg_DrawLineStripFlatAAVBO14_2(_glBufId,
                                            vbOff,
                                            numVerts/*numPoints*/
                                            );
            sdvg_SetPointRadius(strokeW);
            if(0u != (flags & MINNIE_DRAWOP_LINE_STRIP_FLAG_CLOSED))
            {
               // closed polyline
               if(0u != (flags & MINNIE_DRAWOP_LINE_STRIP_FLAG_ROUND_JOIN))
               {
                  sdvg_DrawPointsRoundAAVBO14_2(_glBufId, vbOff, numVerts);
               }
            }
            else
            {
               // open polyline
               if(0u != (flags & MINNIE_DRAWOP_LINE_STRIP_FLAG_ROUND_JOIN))
               {
                  if(0u != (flags & MINNIE_DRAWOP_LINE_STRIP_FLAG_ROUND_CAP))
                  {
                     // round line joints and caps
                     sdvg_DrawPointsRoundAAVBO14_2(_glBufId, vbOff, numVerts);
                  }
                  else
                  {
                     // round line joints, no caps
                     if(numVerts > 2u)
                     {
                        sdvg_DrawPointsRoundAAVBO14_2(_glBufId, vbOff + (1u*4u), numVerts-2u);
                     }
                  }
               }
               else if(0u != (flags & MINNIE_DRAWOP_LINE_STRIP_FLAG_ROUND_CAP))
               {
                  // round line caps
                  if(numVerts >= 2u)
                  {
                     sdvg_DrawPointsRoundAAVBO14_2(_glBufId, vbOff, 1u);
                     sdvg_DrawPointsRoundAAVBO14_2(_glBufId, vbOff + ((numVerts-1u)*4u), 1u);
                  }
               }
            }
            numOpsLineStrip++;
            break;

         case MINNIE_DRAWOP_LINE_STRIP_FLAT_BEVEL_14_2:
            vbOff    = Dstream_read_i32(_bufDraw);
            numVerts = Dstream_read_i32(_bufDraw);
            c32      = Dstream_read_i32(_bufDraw);
            strokeW  = Dstream_read_f32(_bufDraw);
            flags    = Dstream_read_i8(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: draw-line-strip-flat-bevel<s14.2>: vbOff=%u numVerts=%u strokeW=%f flags=0x%02x\n", vbOff, numVerts, strokeW, flags);
            sdvg_SetStrokeColorARGB(c32);
            sdvg_SetStrokeRadius(strokeW);
            sdvg_DrawLineStripFlatBevelAAVBO14_2(_glBufId,
                                                 vbOff,
                                                 numVerts/*numPoints*/,
                                                 (0u == (flags & MINNIE_DRAWOP_LINE_STRIP_FLAG_CLOSED))/*bSkipLastLineJoint*/
                                                 );
            if(0u != (flags & MINNIE_DRAWOP_LINE_STRIP_FLAG_ROUND_CAP))
            {
               // round line caps
               if(numVerts >= 3u)
               {
                  sdvg_SetPointRadius(strokeW);
                  sdvg_DrawPointsRoundAAVBO14_2(_glBufId, vbOff, 1u);
                  sdvg_DrawPointsRoundAAVBO14_2(_glBufId, vbOff + ((numVerts-2u)*4u), 1u);
               }
            }
            numOpsLineStripBevel++;
            break;

         case MINNIE_DRAWOP_LINE_STRIP_FLAT_MITER_14_2:
            vbOff      = Dstream_read_i32(_bufDraw);
            numVerts   = Dstream_read_i32(_bufDraw);
            c32        = Dstream_read_i32(_bufDraw);
            strokeW    = Dstream_read_f32(_bufDraw);
            miterLimit = Dstream_read_f32(_bufDraw);
            flags      = Dstream_read_i8(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: draw-line-strip-flat-miter<s14.2>: vbOff=%u numVerts=%u strokeW=%f flags=0x%02x\n", vbOff, numVerts, strokeW, flags);
            sdvg_SetStrokeColorARGB(c32);
            sdvg_SetStrokeRadius(strokeW);
            sdvg_SetLineMiterLimit(miterLimit);
            sdvg_DrawLineStripFlatMiterAAVBO14_2(_glBufId,
                                                 vbOff,
                                                 numVerts/*numPoints*/,
                                                 (0u == (flags & MINNIE_DRAWOP_LINE_STRIP_FLAG_CLOSED))/*bSkipLastLineJoint*/
                                                 );
            if(0u != (flags & MINNIE_DRAWOP_LINE_STRIP_FLAG_ROUND_CAP))
            {
               // round line caps
               if(numVerts >= 3u)
               {
                  sdvg_SetPointRadius(strokeW);
                  sdvg_DrawPointsRoundAAVBO14_2(_glBufId, vbOff, 1u);
                  sdvg_DrawPointsRoundAAVBO14_2(_glBufId, vbOff + ((numVerts-2u)*4u), 1u);
               }
            }
            numOpsLineStripMiter++;
            break;

      } // switch op
   } // iterate draw ops

   if(_bDebug)
   {
      Dprintf("[dbg] minExecDrawList: #Tri=%u #Poly=%u #TriTex=%u #LineStrip=%u #LineStripBevel=%u #LineStripMiter=%u #RectFill=%u #RectStroke=%u #RectFillStroke=%u #EllipseFill=%u #EllipseStroke=%u #EllipseFillStroke=%u #RoundRectFill=%u #RoundRectStroke=%u #RoundRectFillStroke=%u dl-size=%u\n", numOpsTri, numOpsPoly, numOpsTriTex, numOpsLineStrip, numOpsLineStripBevel, numOpsLineStripMiter, numOpsRectFill, numOpsRectStroke, numOpsRectFillStroke, numOpsEllipseFill, numOpsEllipseStroke, numOpsEllipseFillStroke, numOpsRoundRectFill, numOpsRoundRectStroke, numOpsRoundRectFillStroke, Dstream_get_offset(_bufDraw));
   }
}

/* @function minExecDrawList,Buffer bufDraw,int glBufId
Execute draw-list
@arg bufDraw Draw-list buffer (offset determines size) (in system RAM)
@arg glBufId Vertex buffer id (in video RAM)

@groupref Draw
*/
void minExecDrawList(YAC_Buffer *_bufDraw, sUI _glBufId) {
   minExecDrawListEx(_bufDraw, _glBufId, YAC_FALSE/*bDebug*/);
}
