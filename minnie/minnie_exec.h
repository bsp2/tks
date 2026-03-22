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

#define Ddebug_draw_list_printf       if(!1);else Dprintf
#define Ddebug_draw_list_printfv      if( 1);else Dprintf
#define Ddebug_draw_list_errorprintfv if(!1);else Dprintf

typedef struct minnie_subpath_s {
   sUI vb_off;
   sUI num_verts;
} minnie_subpath_t;

static minnie_subpath_t loc_minnie_subpaths[MINNIE_MAX_SUB_PATHS];  // temporary during exec

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

   sdvg_SetAARange(MINNIE_SHAPE_AA_RANGE);
   sdvg_SetFillRuleEvenOdd();

   // Parse draw-list
   sUI dlSize = Dstream_get_offset(_bufDraw);
   Dstream_set_offset(_bufDraw, 0u);
   Ddebug_draw_list_printfv("[trc] ~~~~~~~~~~~~~~~~~~~~~~~~~~ draw-list dlSize=%u\n", dlSize);
   sUI numOps                    = 0u;
   sUI numOpsPoly                = 0u;
   sUI numOpsPolyBegin           = 0u;
   sUI numOpsPolySub             = 0u;
   sUI numOpsPolySubAA           = 0u;
   sUI numOpsPolyEnd             = 0u;
   sUI numOpsPolyEndAA           = 0u;
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
   sUI numOpsAARange             = 0u;
   sUI numOpsFillRule            = 0u;

   sUI dlTexId = 0;
   sBool dlTexRepeat;
   sBool dlTexFilter;
   sF32 dlTexDecalAlpha;
   sUI polySubIdx = 0u;

   while(Dstream_get_offset(_bufDraw) < dlSize)
   {
      sU16 op = Dstream_read_i16(_bufDraw);
      Ddebug_draw_list_printfv("[trc] minExecDrawList: [%4u] draw-list off=%u op=%u\n", numOps, (Dstream_get_offset(_bufDraw)-2u), op);
      numOps++;
      sF32 aaRange;
      sUI vbOff;
      sUI numVerts;
      sU32 c32Fill;
      sU32 c32Stroke;
      sF32 strokeW;
      sF32 miterLimit;
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
      sF32 paintStartX;
      sF32 paintStartY;
      sF32 paintDirX;
      sF32 paintDirY;
      sF32 paintRadiusX;
      sF32 paintRadiusY;
      sF32 paintSizeX;
      sF32 paintSizeY;
      sF32 paintAngle01;

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
            vbOff     = Dstream_read_i32(_bufDraw);
            numVerts  = Dstream_read_i32(_bufDraw);
            c32Fill   = Dstream_read_i32(_bufDraw);
            c32Stroke = Dstream_read_i32(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: draw-triangles-fill-flat-uniform<f32>-paint: vbOff=%u numVerts=%u\n", vbOff, numVerts);
            sdvg_SetFillAndStrokeColorsARGB(c32Fill, c32Stroke);
            sdvg_DrawTrianglesFillFlatUniformVBO32(_glBufId,
                                                   vbOff,
                                                   numVerts
                                                   );
            numOpsTri++;
            break;

         case MINNIE_DRAWOP_TRIANGLES_FILL_FLAT_UNIFORM_14_2:
            vbOff     = Dstream_read_i32(_bufDraw);
            numVerts  = Dstream_read_i32(_bufDraw);
            c32Fill   = Dstream_read_i32(_bufDraw);
            c32Stroke = Dstream_read_i32(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: draw-triangles-fill-flat-uniform<s14.2>-paint: vbOff=%u numVerts=%u\n", vbOff, numVerts);
            sdvg_SetFillAndStrokeColorsARGB(c32Fill, c32Stroke);
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
            c32Fill   = Dstream_read_i32(_bufDraw);
            c32Stroke = Dstream_read_i32(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: draw-polygon-fill-flat<f32>: vbOff=%u numVerts=%u c32Fill=#%08x\n", vbOff, numVerts, c32Fill);
            sdvg_SetFillAndStrokeColorsARGB(c32Fill, c32Stroke);
            sdvg_DrawPolygonFillFlatUniformVBO32(_glBufId,
                                                 vbOff,
                                                 numVerts
                                                 );
            numOpsPoly++;
            break;

         case MINNIE_DRAWOP_POLYGON_FILL_FLAT_UNIFORM_14_2:
            vbOff    = Dstream_read_i32(_bufDraw);
            numVerts = Dstream_read_i32(_bufDraw);
            c32Fill   = Dstream_read_i32(_bufDraw);
            c32Stroke = Dstream_read_i32(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: draw-polygon-fill-flat<s14.2>: vbOff=%u numVerts=%u c32Fill=#%08x\n", vbOff, numVerts, c32Fill);
            sdvg_SetFillAndStrokeColorsARGB(c32Fill, c32Stroke);
            sdvg_DrawPolygonFillFlatUniformVBO14_2(_glBufId,
                                                   vbOff,
                                                   numVerts
                                                   );
            numOpsPoly++;
            break;

         case MINNIE_DRAWOP_POLYGON_FILL_FLAT_UNIFORM_32_AA:
            vbOff    = Dstream_read_i32(_bufDraw);
            numVerts = Dstream_read_i32(_bufDraw);
            c32Fill   = Dstream_read_i32(_bufDraw);
            c32Stroke = Dstream_read_i32(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: draw-polygon-fill-flat-aa<f32>: vbOff=%u numVerts=%u c32Fill=#%08x\n", vbOff, numVerts, c32Fill);
            /* sdvg_SetFillAndStrokeColorsARGB(c32Fill, c32Stroke); */  // (todo) fix pattern decal colors
            sdvg_SetColorARGB(c32Fill);
            sdvg_DrawPolygonFillFlatUniformAAVBO32(_glBufId,
                                                   vbOff,
                                                   numVerts  // includes 2 wrap-around vertices
                                                   );
            numOpsPoly++;
            break;

         case MINNIE_DRAWOP_POLYGON_FILL_FLAT_UNIFORM_14_2_AA:
            vbOff    = Dstream_read_i32(_bufDraw);
            numVerts = Dstream_read_i32(_bufDraw);
            c32Fill   = Dstream_read_i32(_bufDraw);
            c32Stroke = Dstream_read_i32(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: draw-polygon-fill-flat-aa<s14.2>: vbOff=%u numVerts=%u c32Fill=#%08x\n", vbOff, numVerts, c32Fill);
            /* sdvg_SetFillAndStrokeColorsARGB(c32Fill, c32Stroke); */  // (todo) fix pattern decal colors
            sdvg_SetColorARGB(c32Fill);
            sdvg_DrawPolygonFillFlatUniformAAVBO14_2(_glBufId,
                                                     vbOff,
                                                     numVerts  // includes 2 wrap-around vertices
                                                     );
            numOpsPoly++;
            break;

         case MINNIE_DRAWOP_POLYGON_FILL_FLAT_UNIFORM_14_2_BEGIN:
            c32Fill   = Dstream_read_i32(_bufDraw);
            c32Stroke = Dstream_read_i32(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: polygon-fill-flat-begin<s14.2>: c32Fill=#%08x\n", c32Fill);
            /* sdvg_SetFillAndStrokeColorsARGB(c32Fill, c32Stroke); */  // (todo) fix pattern decal colors
            sdvg_SetColorARGB(c32Fill);
            sdvg_PolygonFillFlatUniformVBO14_2_BeginPass1(_glBufId);
            loc_minnie_subpaths[0].num_verts = 0u;
            polySubIdx = 0u;
            numOpsPolyBegin++;
            break;

         case MINNIE_DRAWOP_POLYGON_FILL_FLAT_UNIFORM_14_2_SUB:
            vbOff    = Dstream_read_i32(_bufDraw);
            numVerts = Dstream_read_i32(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: polygon-fill-flat-sub<s14.2>: vbOff=%u numVerts=%u\n", vbOff, numVerts);
            if(polySubIdx < MINNIE_MAX_SUB_PATHS)
            {
               // Update stencil buffer or line RAM
               sdvg_PolygonFillFlatUniformVBO14_2_DrawPass1(vbOff,
                                                            numVerts
                                                            );
               loc_minnie_subpaths[polySubIdx].vb_off    = vbOff;
               loc_minnie_subpaths[polySubIdx].num_verts = numVerts;
               polySubIdx++;
            }
            else
            {
               Ddebug_draw_list_errorprintfv("[---] minExecDrawList: max sub-paths(%u) exceeded\n", MINNIE_MAX_SUB_PATHS);
            }
            numOpsPolySub++;
            break;

         case MINNIE_DRAWOP_POLYGON_FILL_FLAT_UNIFORM_14_2_SUB_AA:
            vbOff    = Dstream_read_i32(_bufDraw);
            numVerts = Dstream_read_i32(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: polygon-fill-flat-sub-aa<s14.2>: vbOff=%u numVerts=%u\n", vbOff, numVerts);
            if(numVerts >= (3u + 2u))
            {
               // Update stencil buffer or line RAM
               if(polySubIdx < MINNIE_MAX_SUB_PATHS)
               {
                  sdvg_PolygonFillFlatUniformVBO14_2_DrawPass1(vbOff,
                                                               numVerts - 1u
                                                               );
                  loc_minnie_subpaths[polySubIdx].vb_off    = vbOff;
                  loc_minnie_subpaths[polySubIdx].num_verts = numVerts;
                  polySubIdx++;
               }
               else
               {
                  Ddebug_draw_list_errorprintfv("[---] minExecDrawList: max sub-paths(%u) exceeded\n", MINNIE_MAX_SUB_PATHS);
               }
            }
            numOpsPolySubAA++;
            break;

         case MINNIE_DRAWOP_POLYGON_FILL_FLAT_UNIFORM_14_2_END:
            Ddebug_draw_list_printfv("[trc] minExecDrawList: polygon-fill-flat-end<s14.2>\n");
            // Draw fill
            sdvg_PolygonFillFlatUniformVBO14_2_BeginPass2();
            for(sUI subPathIdx = 0u; subPathIdx < polySubIdx; subPathIdx++)
            {
               sUI vbOff    = loc_minnie_subpaths[subPathIdx].vb_off;
               sUI numVerts = loc_minnie_subpaths[subPathIdx].num_verts;
               sdvg_PolygonFillFlatUniformVBO14_2_DrawPass2(vbOff,
                                                            numVerts
                                                            );
            }
            sdvg_PolygonFillFlatUniformVBO14_2_End();
            numOpsPolyEnd++;
            break;

         case MINNIE_DRAWOP_POLYGON_FILL_FLAT_UNIFORM_14_2_END_AA:
            Ddebug_draw_list_printfv("[trc] minExecDrawList: polygon-fill-flat-end-aa<s14.2>\n");
            // Draw fill
            sdvg_PolygonFillFlatUniformVBO14_2_BeginPass2();
            for(sUI subPathIdx = 0u; subPathIdx < polySubIdx; subPathIdx++)
            {
               sUI vbOff    = loc_minnie_subpaths[subPathIdx].vb_off;
               sUI numVerts = loc_minnie_subpaths[subPathIdx].num_verts;
               sdvg_PolygonFillFlatUniformVBO14_2_DrawPass2(vbOff,
                                                            numVerts - 1u
                                                            );
            }
            sdvg_PolygonFillFlatUniformVBO14_2_End();
            // Draw AA outlines
            //  (todo) skip when GPU has AA n-polygon rasterizer
            for(sUI subPathIdx = 0u; subPathIdx < polySubIdx; subPathIdx++)
            {
               sUI vbOff    = loc_minnie_subpaths[subPathIdx].vb_off;
               sUI numVerts = loc_minnie_subpaths[subPathIdx].num_verts;  // includes one extra vertex for bevel line strips
               sdvg_PolygonFillFlatUniformVBO14_2_DrawPass3_AA(vbOff,
                                                               numVerts
                                                               );
            }
            numOpsPolyEndAA++;
            break;

         case MINNIE_DRAWOP_RECT_FILL:
            cx             = Dstream_read_f32(_bufDraw);  // center (_not_ top/left corner)
            cy             = Dstream_read_f32(_bufDraw);
            rx             = Dstream_read_f32(_bufDraw);  // radius, not size
            ry             = Dstream_read_f32(_bufDraw);
            c32Fill        = Dstream_read_i32(_bufDraw);
            c32Stroke      = Dstream_read_i32(_bufDraw);
            vbOff          = Dstream_read_i32(_bufDraw);
            numVerts       = Dstream_read_i16(_bufDraw);
            vbOffBorder    = Dstream_read_i32(_bufDraw);
            numVertsBorder = Dstream_read_i16(_bufDraw);
            glPrimType     = Dstream_read_i16(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: draw-rect-fill<f32>: vbOff=%u numVerts=%u numVertsBorder=%u\n", vbOff, numVerts, numVertsBorder);
            sdvg_SetFillAndStrokeColorsARGB(c32Fill, c32Stroke);
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
            c32Fill        = Dstream_read_i32(_bufDraw);
            c32Stroke      = Dstream_read_i32(_bufDraw);
            strokeW        = Dstream_read_f32(_bufDraw);
            vbOffBorder    = Dstream_read_i32(_bufDraw);
            numVertsBorder = Dstream_read_i16(_bufDraw);
            glPrimType     = Dstream_read_i16(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: draw-rect-stroke<f32>: vbOffBorder=%u numVertsBorder=%u\n", vbOffBorder, numVertsBorder);
            sdvg_SetFillAndStrokeColorsARGB(c32Fill, c32Stroke);
            sdvg_SetStrokeRadius(strokeW);
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
            c32Fill        = Dstream_read_i32(_bufDraw);
            c32Stroke      = Dstream_read_i32(_bufDraw);
            strokeW        = Dstream_read_f32(_bufDraw);
            vbOff          = Dstream_read_i32(_bufDraw);
            numVerts       = Dstream_read_i16(_bufDraw);
            vbOffBorder    = Dstream_read_i32(_bufDraw);
            numVertsBorder = Dstream_read_i16(_bufDraw);
            glPrimType     = Dstream_read_i16(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: draw-rect-fill-stroke<f32>: vbOff=%u numVerts=%u vbOffBorder=%u numVertsBorder=%u\n", vbOff, numVerts, vbOffBorder, numVertsBorder);
            sdvg_SetFillAndStrokeColorsARGB(c32Fill, c32Stroke);
            sdvg_SetStrokeRadius(strokeW);
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
            c32Fill        = Dstream_read_i32(_bufDraw);
            c32Stroke      = Dstream_read_i32(_bufDraw);
            vbOff          = Dstream_read_i32(_bufDraw);
            numVerts       = Dstream_read_i16(_bufDraw);
            vbOffBorder    = Dstream_read_i32(_bufDraw);
            numVertsBorder = Dstream_read_i16(_bufDraw);
            glPrimType     = Dstream_read_i16(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: draw-ellipse-fill<f32>: vbOff=%u numVerts=%u vbOffBorder=%u numVertsBorder=%u\n", vbOff, numVerts, vbOffBorder, numVertsBorder);
            sdvg_SetFillAndStrokeColorsARGB(c32Fill, c32Stroke);
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
            c32Fill        = Dstream_read_i32(_bufDraw);
            c32Stroke      = Dstream_read_i32(_bufDraw);
            strokeW        = Dstream_read_f32(_bufDraw);
            vbOffBorder    = Dstream_read_i32(_bufDraw);
            numVertsBorder = Dstream_read_i16(_bufDraw);
            glPrimType     = Dstream_read_i16(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: draw-ellipse-stroke<f32>: vbOffBorder=%u numVertsBorder=%u\n", vbOffBorder, numVertsBorder);
            sdvg_SetFillAndStrokeColorsARGB(c32Fill, c32Stroke);
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
            c32Fill        = Dstream_read_i32(_bufDraw);
            c32Stroke      = Dstream_read_i32(_bufDraw);
            strokeW        = Dstream_read_f32(_bufDraw);
            vbOff          = Dstream_read_i32(_bufDraw);
            numVerts       = Dstream_read_i16(_bufDraw);
            vbOffBorder    = Dstream_read_i32(_bufDraw);
            numVertsBorder = Dstream_read_i16(_bufDraw);
            glPrimType     = Dstream_read_i16(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: draw-ellipse-fill-stroke<f32>: vbOff=%u numVerts=%u vbOffBorder=%u numVertsBorder=%u\n", vbOff, numVerts, vbOffBorder, numVertsBorder);
            sdvg_SetFillAndStrokeColorsARGB(c32Fill, c32Stroke);
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
            c32Fill        = Dstream_read_i32(_bufDraw);
            c32Stroke      = Dstream_read_i32(_bufDraw);
            vbOff          = Dstream_read_i32(_bufDraw);
            numVerts       = Dstream_read_i16(_bufDraw);
            vbOffBorder    = Dstream_read_i32(_bufDraw);
            numVertsBorder = Dstream_read_i16(_bufDraw);
            glPrimType     = Dstream_read_i16(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: draw-roundrect-fill<f32>: vbOff=%u numVerts=%u vbOffBorder=%u numVertsBorder=%u\n", vbOff, numVerts, vbOffBorder, numVertsBorder);
            sdvg_SetFillAndStrokeColorsARGB(c32Fill, c32Stroke);
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
            c32Fill        = Dstream_read_i32(_bufDraw);
            c32Stroke      = Dstream_read_i32(_bufDraw);
            strokeW        = Dstream_read_f32(_bufDraw);
            vbOffBorder    = Dstream_read_i32(_bufDraw);
            numVertsBorder = Dstream_read_i16(_bufDraw);
            glPrimType     = Dstream_read_i16(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: draw-roundrect-stroke<f32>: vbOffBorder=%u numVertsBorder=%u\n", vbOffBorder, numVertsBorder);
            sdvg_SetFillAndStrokeColorsARGB(c32Fill, c32Stroke);
            sdvg_SetStrokeRadius(strokeW);
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
            c32Fill        = Dstream_read_i32(_bufDraw);
            c32Stroke      = Dstream_read_i32(_bufDraw);
            strokeW        = Dstream_read_f32(_bufDraw);
            vbOff          = Dstream_read_i32(_bufDraw);
            numVerts       = Dstream_read_i16(_bufDraw);
            vbOffBorder    = Dstream_read_i32(_bufDraw);
            numVertsBorder = Dstream_read_i16(_bufDraw);
            glPrimType     = Dstream_read_i16(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: draw-roundrect-fill-stroke<f32>: vbOff=%u numVerts=%u vbOffBorder=%u numVertsBorder=%u\n", vbOff, numVerts, vbOffBorder, numVertsBorder);
            sdvg_SetFillAndStrokeColorsARGB(c32Fill, c32Stroke);
            sdvg_SetStrokeRadius(strokeW);
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

         case MINNIE_DRAWOP_PAINT_SOLID:
            Ddebug_draw_list_printfv("[trc] minExecDrawList: paint-solid\n");
            sdvg_PaintSolid();
            break;

         case MINNIE_DRAWOP_PAINT_LINEAR:
            paintStartX = Dstream_read_f32(_bufDraw);
            paintStartY = Dstream_read_f32(_bufDraw);
            paintDirX   = Dstream_read_f32(_bufDraw);
            paintDirY   = Dstream_read_f32(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: paint-linear: start=(%f;%f) dir=(%f;%f)\n", paintStartX, paintStartY, paintDirX, paintDirY);
            sdvg_PaintLinear(paintStartX, paintStartY, paintDirX, paintDirY);
            break;

         case MINNIE_DRAWOP_PAINT_RADIAL:
            paintStartX  = Dstream_read_f32(_bufDraw);
            paintStartY  = Dstream_read_f32(_bufDraw);
            paintRadiusX = Dstream_read_f32(_bufDraw);
            paintRadiusY = Dstream_read_f32(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: paint-radial: start=(%f;%f) radius=(%f;%f)\n", paintStartX, paintStartY, paintRadiusX, paintRadiusY);
            sdvg_PaintRadial(paintStartX, paintStartY, paintRadiusX, paintRadiusY);
            break;

         case MINNIE_DRAWOP_PAINT_CONIC:
            paintStartX  = Dstream_read_f32(_bufDraw);
            paintStartY  = Dstream_read_f32(_bufDraw);
            paintRadiusX = Dstream_read_f32(_bufDraw);
            paintRadiusY = Dstream_read_f32(_bufDraw);
            paintAngle01 = Dstream_read_f32(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: paint-radial: start=(%f;%f) radius=(%f;%f) angle01=%f\n", paintStartX, paintStartY, paintRadiusX, paintRadiusY, paintAngle01);
            sdvg_PaintConic(paintStartX, paintStartY, paintRadiusX, paintRadiusY, paintAngle01);
            break;

         case MINNIE_DRAWOP_PAINT_PATTERN:
            paintStartX = Dstream_read_f32(_bufDraw);
            paintStartY = Dstream_read_f32(_bufDraw);
            paintDirX   = Dstream_read_f32(_bufDraw);
            paintDirY   = Dstream_read_f32(_bufDraw);
            paintSizeX  = Dstream_read_f32(_bufDraw);
            paintSizeY  = Dstream_read_f32(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: paint-pattern: start=(%f;%f) dir=(%f;%f) size(%f;%f)\n", paintStartX, paintStartY, paintDirX, paintDirY, paintSizeX, paintSizeY);
            sdvg_PaintPattern(paintStartX, paintStartY, paintDirX, paintDirY, paintSizeX, paintSizeY);
            break;

         case MINNIE_DRAWOP_PAINT_PATTERN_ALPHA:
            paintStartX = Dstream_read_f32(_bufDraw);
            paintStartY = Dstream_read_f32(_bufDraw);
            paintDirX   = Dstream_read_f32(_bufDraw);
            paintDirY   = Dstream_read_f32(_bufDraw);
            paintSizeX  = Dstream_read_f32(_bufDraw);
            paintSizeY  = Dstream_read_f32(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: paint-pattern-alpha: start=(%f;%f) dir=(%f;%f) size(%f;%f)\n", paintStartX, paintStartY, paintDirX, paintDirY, paintSizeX, paintSizeY);
            sdvg_PaintPatternAlpha(paintStartX, paintStartY, paintDirX, paintDirY, paintSizeX, paintSizeY);
            break;

         case MINNIE_DRAWOP_PAINT_PATTERN_DECAL:
            paintStartX = Dstream_read_f32(_bufDraw);
            paintStartY = Dstream_read_f32(_bufDraw);
            paintDirX   = Dstream_read_f32(_bufDraw);
            paintDirY   = Dstream_read_f32(_bufDraw);
            paintSizeX  = Dstream_read_f32(_bufDraw);
            paintSizeY  = Dstream_read_f32(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: paint-pattern-decal: start=(%f;%f) dir=(%f;%f) size(%f;%f)\n", paintStartX, paintStartY, paintDirX, paintDirY, paintSizeX, paintSizeY);
            sdvg_PaintPatternDecal(paintStartX, paintStartY, paintDirX, paintDirY, paintSizeX, paintSizeY);
            break;

         case MINNIE_DRAWOP_PAINT_PATTERN_DECAL_ALPHA:
            paintStartX = Dstream_read_f32(_bufDraw);
            paintStartY = Dstream_read_f32(_bufDraw);
            paintDirX   = Dstream_read_f32(_bufDraw);
            paintDirY   = Dstream_read_f32(_bufDraw);
            paintSizeX  = Dstream_read_f32(_bufDraw);
            paintSizeY  = Dstream_read_f32(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: paint-pattern-decal-alpha: start=(%f;%f) dir=(%f;%f) size(%f;%f)\n", paintStartX, paintStartY, paintDirX, paintDirY, paintSizeX, paintSizeY);
            sdvg_PaintPatternDecalAlpha(paintStartX, paintStartY, paintDirX, paintDirY, paintSizeX, paintSizeY);
            break;

         case MINNIE_DRAWOP_TRIANGLES_TEX_UV_FLAT_32:
            vbOff    = Dstream_read_i32(_bufDraw);
            numVerts = Dstream_read_i32(_bufDraw);
            c32Fill   = Dstream_read_i32(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: draw-tri-tex-uv-flat<f32>: vbOff=%u numVerts=%u texId=%u texRep=%d texFlt=%d c32Fill=#%08x\n", vbOff, numVerts, dlTexId, dlTexRepeat, dlTexFilter, c32Fill);
            sdvg_SetFillColorARGB(c32Fill);
            sdvg_DrawTrianglesTexUVFlatVBO32(_glBufId,
                                             vbOff,
                                             numVerts
                                             );
            numOpsTriTex++;
            break;

         case MINNIE_DRAWOP_TRIANGLES_TEX_UV_FLAT_DECAL_32:
            vbOff     = Dstream_read_i32(_bufDraw);
            numVerts  = Dstream_read_i32(_bufDraw);
            c32Fill   = Dstream_read_i32(_bufDraw);
            c32Stroke = Dstream_read_i32(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: draw-tri-tex-uv-flat-decal<f32>: vbOff=%u numVerts=%u texId=%u texRep=%d texFlt=%d\n", vbOff, numVerts, dlTexId, dlTexRepeat, dlTexFilter);
            sdvg_SetFillAndStrokeColorsARGB(c32Fill, c32Stroke);
            sdvg_DrawTrianglesTexUVFlatDecalVBO32(_glBufId,
                                                  vbOff,
                                                  numVerts
                                                  );
            numOpsTriTex++;
            break;

         case MINNIE_DRAWOP_TRIANGLES_TEX_UV_GOURAUD_32:
            vbOff    = Dstream_read_i32(_bufDraw);
            numVerts = Dstream_read_i32(_bufDraw);
            c32Fill  = Dstream_read_i32(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: draw-tri-tex-uv-gouraud<f32>: vbOff=%u numVerts=%u texId=%u texRep=%d texFlt=%d\n", vbOff, numVerts, dlTexId, dlTexRepeat, dlTexFilter);
            sdvg_SetFillColorARGB(c32Fill);
            sdvg_DrawTrianglesTexUVGouraudVBO32(_glBufId,
                                                vbOff,
                                                numVerts
                                                );
            numOpsTriTex++;
            break;

         case MINNIE_DRAWOP_TRIANGLES_TEX_UV_GOURAUD_DECAL_32:
            vbOff     = Dstream_read_i32(_bufDraw);
            numVerts  = Dstream_read_i32(_bufDraw);
            c32Fill   = Dstream_read_i32(_bufDraw);
            c32Stroke = Dstream_read_i32(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: draw-tri-tex-uv-gouraud-decal<f32>: vbOff=%u numVerts=%u texId=%u texRep=%d texFlt=%d\n", vbOff, numVerts, dlTexId, dlTexRepeat, dlTexFilter);
            sdvg_SetFillAndStrokeColorsARGB(c32Fill, c32Stroke);
            sdvg_DrawTrianglesTexUVGouraudDecalVBO32(_glBufId,
                                                     vbOff,
                                                     numVerts
                                                     );
            numOpsTriTex++;
            break;

         case MINNIE_DRAWOP_LINE_STRIP_FLAT_14_2:
            vbOff     = Dstream_read_i32(_bufDraw);
            numVerts  = Dstream_read_i32(_bufDraw);
            c32Fill   = Dstream_read_i32(_bufDraw);
            c32Stroke = Dstream_read_i32(_bufDraw);
            strokeW   = Dstream_read_f32(_bufDraw);
            flags     = Dstream_read_i8(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: draw-line-strip-flat<s14.2>: vbOff=%u numVerts=%u strokeW=%f\n", vbOff, numVerts, strokeW);
            sdvg_SetFillAndStrokeColorsARGB(c32Fill, c32Stroke);
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
            vbOff     = Dstream_read_i32(_bufDraw);
            numVerts  = Dstream_read_i32(_bufDraw);
            c32Fill   = Dstream_read_i32(_bufDraw);
            c32Stroke = Dstream_read_i32(_bufDraw);
            strokeW   = Dstream_read_f32(_bufDraw);
            flags     = Dstream_read_i8(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: draw-line-strip-flat-bevel<s14.2>: vbOff=%u numVerts=%u strokeW=%f flags=0x%02x\n", vbOff, numVerts, strokeW, flags);
            sdvg_SetFillAndStrokeColorsARGB(c32Fill, c32Stroke);
            sdvg_SetStrokeRadius(strokeW);
            sdvg_DrawLineStripFlatBevelAAVBO14_2(_glBufId,
                                                 vbOff,
                                                 numVerts,  // includes 2 wrap-around vertices
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
            c32Stroke  = Dstream_read_i32(_bufDraw);
            strokeW    = Dstream_read_f32(_bufDraw);
            miterLimit = Dstream_read_f32(_bufDraw);
            flags      = Dstream_read_i8(_bufDraw);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: draw-line-strip-flat-miter<s14.2>: vbOff=%u numVerts=%u strokeW=%f flags=0x%02x\n", vbOff, numVerts, strokeW, flags);
            sdvg_SetStrokeColorARGB(c32Stroke);
            sdvg_SetStrokeRadius(strokeW);
            sdvg_SetLineMiterLimit(miterLimit);
            sdvg_DrawLineStripFlatMiterAAVBO14_2(_glBufId,
                                                 vbOff,
                                                 numVerts,  // includes 2 wrap-around vertices
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

         case MINNIE_DRAWOP_AA_RANGE:
            aaRange = Dstream_read_f32(_bufDraw);
            sdvg_SetAARange(aaRange);
            Ddebug_draw_list_printfv("[trc] minExecDrawList: aa_range: aaRange=%f\n", aaRange);
            numOpsAARange++;
            break;

         case MINNIE_DRAWOP_FILLRULE_EVENODD:
            Ddebug_draw_list_printfv("[trc] minExecDrawList: fillrule-evenodd:\n");
            sdvg_SetFillRuleEvenOdd();
            numOpsFillRule++;
            break;

         case MINNIE_DRAWOP_FILLRULE_NONZERO:
            Ddebug_draw_list_printfv("[trc] minExecDrawList: fillrule-nonzero:\n");
            sdvg_SetFillRuleNonZero();
            numOpsFillRule++;
            break;

      } // switch op
   } // iterate draw ops

   if(_bDebug)
   {
      Dprintf("[dbg] minExecDrawList: #Tri=%u #Poly=%u #PolyBegin=%u (#sub=%u #end=%u #subAA=%u #endAA=%u) #TriTex=%u #LineStrip=%u #LineStripBevel=%u #LineStripMiter=%u #RectFill=%u #RectStroke=%u #RectFillStroke=%u #EllipseFill=%u #EllipseStroke=%u #EllipseFillStroke=%u #RoundRectFill=%u #RoundRectStroke=%u #RoundRectFillStroke=%u #AARange=%u #FillRule=%u #Ops=%u dl-size=%u\n", numOpsTri, numOpsPoly, numOpsPolyBegin, numOpsPolySub, numOpsPolyEnd, numOpsPolySubAA, numOpsPolyEndAA, numOpsTriTex, numOpsLineStrip, numOpsLineStripBevel, numOpsLineStripMiter, numOpsRectFill, numOpsRectStroke, numOpsRectFillStroke, numOpsEllipseFill, numOpsEllipseStroke, numOpsEllipseFillStroke, numOpsRoundRectFill, numOpsRoundRectStroke, numOpsRoundRectFillStroke, numOpsAARange, numOpsFillRule, numOps, Dstream_get_offset(_bufDraw));
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
