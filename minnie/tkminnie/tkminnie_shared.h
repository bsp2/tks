/// tkminnie_shared.h
///
/// (c) 2014-2025 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#ifndef TKMINNIE_SHARED_H__
#define TKMINNIE_SHARED_H__

// tkminnie/shadervg functions shared with other plugins (e.g. tksampleedit)

#define MINNIE_SKIP_TYPEDEFS  defined
#include "../minnie.h"

#define SHADERVG_SKIP_DECLARATIONS
#include "../shadervg/shadervg.h"

#define TKMINNIE_SHARED_NAME  "TKMINNIE_SHARED"

extern "C" {
   typedef struct {
      void (YAC_CALL *sdvg_ReturnToGL) (void);
      // -------- transform --------
      void        (YAC_CALL *sdvg_SetTransform)    (YAC_Object *_mat4);
      YAC_Object *(YAC_CALL *sdvg_GetTransformRef) (void);
      // -------- AA --------
      void (YAC_CALL *sdvg_SetEnableAA) (sBool _bEnable);
      void (YAC_CALL *sdvg_SetAARange)  (sF32 _aaRange);
      void (YAC_CALL *sdvg_SetAAExp)    (sF32 _aaExp);
      // -------- render state (fill / stroke) --------
      void (YAC_CALL *sdvg_SetFillColor4f)       (sF32 _fillR, sF32 _fillG, sF32 _fillB, sF32 _fillA);
      void (YAC_CALL *sdvg_SetFillAlpha)         (sF32 _fillA);
      void (YAC_CALL *sdvg_SetFillColorARGB)     (sUI _c32);
      void (YAC_CALL *sdvg_SetStrokeWidth)       (sF32 _strokeW);
      void (YAC_CALL *sdvg_SetPointRadius)       (sF32 _radius);
      void (YAC_CALL *sdvg_SetPixelScaling)      (sF32 _s);
      void (YAC_CALL *sdvg_SetStrokeColor4f)     (sF32 _strokeR, sF32 _strokeG, sF32 _strokeB, sF32 _strokeA);
      void (YAC_CALL *sdvg_SetStrokeColorARGB)   (sU32 _c32);
      void (YAC_CALL *sdvg_SetGlobalAlpha)       (sF32 _a);
      void (YAC_CALL *sdvg_SetTextureDecalAlpha) (sF32 _decalAlpha);
      // -------- clear --------
      void (YAC_CALL *sdvg_Clear4f)   (sF32 _r, sF32 _g, sF32 _b, sF32 _a);
      void (YAC_CALL *sdvg_ClearARGB) (sUI _c32);
      // -------- blending --------
      void (YAC_CALL *sdvg_EnableBlending)                  (void);
      void (YAC_CALL *sdvg_EnableBlendingKeepAlpha)         (void);
      void (YAC_CALL *sdvg_EnableBlendingAdditive)          (void);
      void (YAC_CALL *sdvg_EnableBlendingAdditiveKeepAlpha) (void);
      void (YAC_CALL *sdvg_EnableBlendingSrcColorKeepAlpha) (void);
      void (YAC_CALL *sdvg_EnableBlendingDstColorKeepAlpha) (void);
      void (YAC_CALL *sdvg_DisableBlending)                 (void);
      void (YAC_CALL *sdvg_AlphaWrite)                      (sBool _bEnable);
      // -------- VBO --------
      sUI  (YAC_CALL *sdvg_CreateVBO)          (sUI _numBytes);
      void (YAC_CALL *sdvg_UpdateVBO)          (sUI _vboId, sUI _offset, sUI _numBytes, YAC_Buffer *_data);
      void (YAC_CALL *sdvg_BindVBO)            (sUI _vboId);
      sUI  (YAC_CALL *sdvg_GetVBOSize)         (void);
      sUI  (YAC_CALL *sdvg_GetMappedVBOOffset) (void);
      void (YAC_CALL *sdvg_MapVBO)             (sUI _vboId);
      void (YAC_CALL *sdvg_UnmapVBO)           (void);
      void (YAC_CALL *sdvg_UnbindVBO)          (void);
      void (YAC_CALL *sdvg_DestroyVBO)         (sUI _vboId);
      // ----------- custom shaders and (scratch) draw functions ------------
      sBool (YAC_CALL *sdvg_BeginVBO)              (sUI _numVertices, sUI _stride);
      sBool (YAC_CALL *sdvg_BeginTriangles)        (sUI _numVertices, sUI _stride);
      sBool (YAC_CALL *sdvg_BeginTriangleFan)      (sUI _numVertices, sUI _stride);
      sBool (YAC_CALL *sdvg_BeginTriangleStrip)    (sUI _numVertices, sUI _stride);
      sBool (YAC_CALL *sdvg_BeginFilledTriangles) (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginFilledTriangleFan) (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginFilledTriangleStrip) (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginFilledGouraudTriangles) (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginFilledGouraudTriangleFan) (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginFilledGouraudTriangleStrip) (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginTexturedTriangles) (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginTexturedTriangleFan) (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginTexturedTriangleStrip) (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginLineStrip)        (sUI _numPoints);
      sBool (YAC_CALL *sdvg_BeginLineStripAA)      (sUI _numPoints);
      sBool (YAC_CALL *sdvg_BeginLineStripBevel)   (sUI _numPoints);
      sBool (YAC_CALL *sdvg_BeginLineStripBevelAA) (sUI _numPoints);
      sBool (YAC_CALL *sdvg_BeginLines)            (sUI _numPoints);
      sBool (YAC_CALL *sdvg_BeginLinesAA)          (sUI _numPoints);
      sBool (YAC_CALL *sdvg_BeginPointsSquare)     (sUI _numPoints);
      sBool (YAC_CALL *sdvg_BeginPointsSquareAA)   (sUI _numPoints);
      sBool (YAC_CALL *sdvg_BeginPointsRound)      (sUI _numPoints);
      sBool (YAC_CALL *sdvg_BeginPointsRoundAA)    (sUI _numPoints);
      void  (YAC_CALL *sdvg_VertexOffset2f)        (void);
      void  (YAC_CALL *sdvg_AttribOffsetf)         (const char *_name, sUI _size);
      void  (YAC_CALL *sdvg_AttribOffset1f)        (const char *_name);
      void  (YAC_CALL *sdvg_AttribOffset2f)        (const char *_name);
      void  (YAC_CALL *sdvg_AttribOffset3f)        (const char *_name);
      void  (YAC_CALL *sdvg_AttribOffset4f)        (const char *_name);
      void  (YAC_CALL *sdvg_AttribOffsetARGB)      (const char *_name);
      void  (YAC_CALL *sdvg_Attrib1f)              (sF32 _f);
      void  (YAC_CALL *sdvg_Attrib2f)              (sF32 _f1, sF32 _f2);
      void  (YAC_CALL *sdvg_Vertex2f)              (sF32 _x, sF32 _y);
      void  (YAC_CALL *sdvg_TexCoord2f)            (sF32 _u, sF32 _v);
      void  (YAC_CALL *sdvg_Attrib3f)              (sF32 _f1, sF32 _f2, sF32 _f3);
      void  (YAC_CALL *sdvg_Color3f)               (sF32 _r, sF32 _g, sF32 _b);
      void  (YAC_CALL *sdvg_Attrib4f)              (sF32 _f1, sF32 _f2, sF32 _f3, sF32 _f4);
      void  (YAC_CALL *sdvg_Color4f)               (sF32 _r, sF32 _g, sF32 _b, sF32 _a);
      void  (YAC_CALL *sdvg_AttribARGB)            (sUI _c32);
      void  (YAC_CALL *sdvg_ColorARGB)             (sUI _c32);
      void  (YAC_CALL *sdvg_End)                   (void);
      // ----------- additional (scratch) draw functions ------------
      void (YAC_CALL *sdvg_DrawFilledRectangle) (sF32 _x, sF32 _y, sF32 _w, sF32 _h);
      void (YAC_CALL *sdvg_DrawRectangle) (sF32 _x, sF32 _y, sF32 _w, sF32 _h, sF32 _b);
   } tkminnie_shared_t;
}

#ifndef TKMINNIE_BUILD
extern tkminnie_shared_t *tkminnie_shared;
extern void tkminnie_shared_resolve(void);
#endif

#endif // TKMINNIE_SHARED_H__
