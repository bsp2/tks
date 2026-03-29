/// tkminnie_shared.h
///
/// (c) 2025-2026 Bastian Spiegel <bs@tkscript.de>
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
      // -------- init/exit --------
      void  (YAC_CALL *sdvg_SetGLSLVersion) (sBool _bV3, sBool _bGLES, YAC_String *_sVersionStringOrNull);
      void  (YAC_CALL *sdvg_SetScratchBufferSize) (sUI _szBytes);
      sBool (YAC_CALL *sdvg_Init) (sBool _bGLCore);
      void  (YAC_CALL *sdvg_Exit) (void);
      sBool (YAC_CALL *sdvg_OnOpen) (void);
      void  (YAC_CALL *sdvg_SetFramebufferSize) (sUI _w, sUI _h);
      // -------- begin/end frame --------
      void (YAC_CALL *sdvg_BeginFrame) (void);
      void (YAC_CALL *sdvg_Flush)      (void);
      void (YAC_CALL *sdvg_ReturnToGL) (void);
      void (YAC_CALL *sdvg_EndFrame)   (void);
      // -------- viewport --------
      void (YAC_CALL *sdvg_SetViewport)       (sUI _x, sUI _y, sUI _w, sUI _h);
      void (YAC_CALL *sdvg_PushViewport)      (sUI _x, sUI _y, sUI _w, sUI _h);
      void (YAC_CALL *sdvg_PopViewport)       (void);
      sUI  (YAC_CALL *sdvg_GetViewportWidth)  (void);
      sUI  (YAC_CALL *sdvg_GetViewportHeight) (void);
      void (YAC_CALL *sdvg_SetPixelScaling)   (sF32 _s);
      // -------- scissor --------
      void (YAC_CALL *sdvg_SetScissor)     (sUI _x, sUI _y, sUI _w, sUI _h);
      void (YAC_CALL *sdvg_PushScissor)    (sUI _x, sUI _y, sUI _w, sUI _h);
      void (YAC_CALL *sdvg_PopScissor)     (void);
      void (YAC_CALL *sdvg_EnableScissor)  (void);
      void (YAC_CALL *sdvg_DisableScissor) (void);
      // -------- transform --------
      void        (YAC_CALL *sdvg_SetTransform)    (YAC_Object *_mat4);
      YAC_Object *(YAC_CALL *sdvg_GetTransformRef) (void);
      // -------- transform (matrix stack) --------
      void (YAC_CALL *sdvg_PushProjMatrix)    (void);
      void (YAC_CALL *sdvg_PopProjMatrix)     (void);
      void (YAC_CALL *sdvg_PushModelMatrix)   (void);
      void (YAC_CALL *sdvg_PopModelMatrix)    (void);
      void (YAC_CALL *sdvg_ProjInitIdentity)  (void);
      void (YAC_CALL *sdvg_ProjInit2D)        (sF32 _w, sF32 _h);
      void (YAC_CALL *sdvg_ProjInitOrtho)     (sF32 _sx, sF32 _sy);
      void (YAC_CALL *sdvg_ProjTranslate2f)   (sF32 _tx, sF32 _ty);
      void (YAC_CALL *sdvg_ModelInitIdentity) (void);
      void (YAC_CALL *sdvg_ModelTranslate2f)  (sF32 _tx, sF32 _ty);
      void (YAC_CALL *sdvg_ModelTranslate3f)  (sF32 _tx, sF32 _ty, sF32 _tz);
      void (YAC_CALL *sdvg_ModelScale2f)      (sF32 _sx, sF32 _sy);
      void (YAC_CALL *sdvg_ModelRotatef)      (sF32 _rad);
      // -------- AA --------
      void (YAC_CALL *sdvg_SetEnableAA) (sBool _bEnable);
      void (YAC_CALL *sdvg_SetAARange)  (sF32 _aaRange);
      void (YAC_CALL *sdvg_SetAAExp)    (sF32 _aaExp);
      void (YAC_CALL *sdvg_SetAlphaSDFRange) (sF32 _aMin, sF32 _aMax);
      void (YAC_CALL *sdvg_SetAlphaSDFExp)   (sF32 _aExp);
      // -------- render state (fill / stroke) --------
      void (YAC_CALL *sdvg_SetFillColor4f)             (sF32 _fillR, sF32 _fillG, sF32 _fillB, sF32 _fillA);
      void (YAC_CALL *sdvg_SetFillAlpha)               (sF32 _fillA);
      void (YAC_CALL *sdvg_SetFillColorARGB)           (sUI _c32);
      void (YAC_CALL *sdvg_SetFillRuleEvenOdd)         (void);
      void (YAC_CALL *sdvg_SetFillRuleNonZero)         (void);
      void (YAC_CALL *sdvg_SetColor4f)                 (sF32 _r, sF32 _g, sF32 _b, sF32 _a);
      void (YAC_CALL *sdvg_SetColorARGB)               (sUI _c32);
      void (YAC_CALL *sdvg_SetStrokeRadius)            (sF32 _radius);
      void (YAC_CALL *sdvg_SetStrokeWidth)             (sF32 _width);
      void (YAC_CALL *sdvg_SetStrokeRadiusAAOffset)    (sF32 _offset);
      void (YAC_CALL *sdvg_SetStrokeScale)             (sF32 _scale);
      void (YAC_CALL *sdvg_SetLinePatternScale)        (sF32 _scale);
      void (YAC_CALL *sdvg_SetLinePatternOffset)       (sF32 _offset);
      void (YAC_CALL *sdvg_SetPointRadius)             (sF32 _radius);
      void (YAC_CALL *sdvg_SetPointSize)               (sF32 _size);
      void (YAC_CALL *sdvg_SetStrokeColor4f)           (sF32 _strokeR, sF32 _strokeG, sF32 _strokeB, sF32 _strokeA);
      void (YAC_CALL *sdvg_SetStrokeColorARGB)         (sU32 _c32);
      void (YAC_CALL *sdvg_SetFillAndStrokeColorsARGB) (sU32 _c32Fill, sU32 _c32Stroke);
      void (YAC_CALL *sdvg_SetGlobalAlpha)             (sF32 _a);
      void (YAC_CALL *sdvg_SetDecalAlpha)              (sF32 _decalAlpha);
      // -------- clear --------
      void (YAC_CALL *sdvg_Clear4f)   (sF32 _r, sF32 _g, sF32 _b, sF32 _a);
      void (YAC_CALL *sdvg_ClearARGB) (sUI _c32);
      void (YAC_CALL *sdvg_ClearStencil) (sUI _s8);
      void (YAC_CALL *sdvg_ClearDepth) (sF32 _v);
      // -------- blending --------
      void (YAC_CALL *sdvg_EnableBlending)                  (void);
      void (YAC_CALL *sdvg_EnableBlendingKeepAlpha)         (void);
      void (YAC_CALL *sdvg_EnableBlendingAdditive)          (void);
      void (YAC_CALL *sdvg_EnableBlendingAdditiveKeepAlpha) (void);
      void (YAC_CALL *sdvg_EnableBlendingSrcColorKeepAlpha) (void);
      void (YAC_CALL *sdvg_EnableBlendingDstColorKeepAlpha) (void);
      void (YAC_CALL *sdvg_DisableBlending)                 (void);
      void (YAC_CALL *sdvg_AlphaWrite)                      (sBool _bEnable);
      // -------- Stencil --------
      void (YAC_CALL *sdvg_EnableStencilMask) (void);
      void (YAC_CALL *sdvg_DisableStencilMask) (void);
      // ----------- custom shaders ------------
      sUI  (YAC_CALL *sdvg_CreateShader)       (const char *vs, const char *fs);
      void (YAC_CALL *sdvg_DestroyShader)      (sUI _shaderIdx);
      void (YAC_CALL *sdvg_BindShader)         (sUI _shaderIdx);
      void (YAC_CALL *sdvg_UnbindShader)       (void);
      sSI  (YAC_CALL *sdvg_GetUniformLocation) (const char *_name);
      sSI  (YAC_CALL *sdvg_GetAttribLocation)  (const char *_name);
      void (YAC_CALL *sdvg_Uniform1f)          (const char *_name, sF32 _f);
      void (YAC_CALL *sdvg_Uniform2f)          (const char *_name, sF32 _f1, sF32 _f2);
      void (YAC_CALL *sdvg_Uniform3f)          (const char *_name, sF32 _f1, sF32 _f2, sF32 _f3);
      void (YAC_CALL *sdvg_Uniform4f)          (const char *_name, sF32 _f1, sF32 _f2, sF32 _f3, sF32 _f4);
      void (YAC_CALL *sdvg_Uniform1i)          (const char *_name, sSI _i);
      // -------- paint --------
      void (YAC_CALL *sdvg_PaintSolid) (void);
      void (YAC_CALL *sdvg_PaintLinear) (sF32 _startX, sF32 _startY, sF32 _dirX, sF32 _dirY);
      void (YAC_CALL *sdvg_PaintRadial) (sF32 _startX, sF32 _startY, sF32 _radiusX, sF32 _radiusY);
      void (YAC_CALL *sdvg_PaintConic) (sF32 _startX, sF32 _startY, sF32 _radiusX, sF32 _radiusY, sF32 _angle01);
      void (YAC_CALL *sdvg_PaintPattern) (sF32 _startX, sF32 _startY, sF32 _dirX, sF32 _dirY, sF32 _sizeX, sF32 _sizeY);
      void (YAC_CALL *sdvg_PaintPatternAlpha) (sF32 _startX, sF32 _startY, sF32 _dirX, sF32 _dirY, sF32 _sizeX, sF32 _sizeY);
      void (YAC_CALL *sdvg_PaintPatternDecal) (sF32 _startX, sF32 _startY, sF32 _dirX, sF32 _dirY, sF32 _sizeX, sF32 _sizeY);
      void (YAC_CALL *sdvg_PaintPatternDecalAlpha) (sF32 _startX, sF32 _startY, sF32 _dirX, sF32 _dirY, sF32 _sizeX, sF32 _sizeY);
      // -------- VBO --------
      sUI  (YAC_CALL *sdvg_CreateVBO)          (sUI _numBytes);
#ifdef SHADERVG_SCRIPT_API
      void (YAC_CALL *sdvg_UpdateVBO)          (sUI _vboId, sUI _offset, sUI _numBytes, YAC_Object *_data);
#else
      void (YAC_CALL *sdvg_UpdateVBO)          (sUI _vboId, sUI _offset, sUI _numBytes, YAC_Buffer *_data);
#endif // SHADERVG_SCRIPT_API
      void  (YAC_CALL *sdvg_BindVBO)            (sUI _vboId);
      sUI   (YAC_CALL *sdvg_GetVBOSize)         (void);
      sUI   (YAC_CALL *sdvg_GetMappedVBOOffset) (void);
      sBool (YAC_CALL *sdvg_GetMappedVBORef)    (Dsdvg_buffer_ref_t _ret);
      void  (YAC_CALL *sdvg_MapVBO)             (sUI _vboId);
      void  (YAC_CALL *sdvg_UnmapVBO)           (void);
      void  (YAC_CALL *sdvg_UnbindVBO)          (void);
      void  (YAC_CALL *sdvg_DestroyVBO)         (sUI _vboId);
      // -------- FBO --------
      sUI  (YAC_CALL *sdvg_CreateFBOEx)     (sUI _w, sUI _h, sBool _bMSAA, sBool _bDepth, sBool _bStencil);
      sUI  (YAC_CALL *sdvg_CreateFBO)       (sUI _w, sUI _h);
      void (YAC_CALL *sdvg_BindFBO)         (sUI _fboIdx);
      void (YAC_CALL *sdvg_ResolveFBO)      (sUI _fboIdx);
      void (YAC_CALL *sdvg_UnbindFBO)       (void);
      sUI  (YAC_CALL *sdvg_GetFBOTextureId) (sUI _fboIdx);
      void (YAC_CALL *sdvg_BindFBOTexture)  (sUI _fboIdx, sBool _bRepeat, sBool _bFilter);
      void (YAC_CALL *sdvg_DestroyFBO)      (sUI _fboIdx);
      // -------- texturing --------
      sUI  (YAC_CALL *sdvg_CreateTexture2D)      (sUI _texfmt, sUI _w, sUI _h, const void *_data, sUI _dataSz);
      void (YAC_CALL *sdvg_UpdateTexture2D)      (sUI _texfmt, sUI _w, sUI _h, const void *_data, sUI _dataSz);
      void (YAC_CALL *sdvg_BindMultiTexture2D)   (sUI _unitIdx, sUI _texId, sBool _bRepeat, sBool _bFilter);
      void (YAC_CALL *sdvg_UnbindMultiTexture2D) (sUI _unitIdx);
      void (YAC_CALL *sdvg_BindTexture2D)        (sUI _texId, sBool _bRepeat, sBool _bFilter);
      void (YAC_CALL *sdvg_UnbindTexture2D)      (void);
      void (YAC_CALL *sdvg_DestroyTexture2D)     (sUI _texId);
      // -------- VBO utility functions --------
      void (YAC_CALL *sdvg_BufferRectTexUVFlat32) (YAC_Buffer *_b, sF32 _x, sF32 _y, sF32 _w, sF32 _h, sF32 _ul, sF32 _vt, sF32 _ur, sF32 _vb);
      void (YAC_CALL *sdvg_WriteC32AsRGBA8) (YAC_Buffer *_b, sUI _c32);
      void (YAC_CALL *sdvg_BufferRectTexUVGouraud32) (YAC_Buffer *_b,  sF32 _x, sF32 _y, sF32 _w, sF32 _h, sF32 _ul, sF32 _vt, sF32 _ur, sF32 _vb, sUI _c32LT, sUI _c32RT, sUI _c32LB, sUI _c32RB);
      // ----------- (scratchbuffer) draw functions ------------
      sBool (YAC_CALL *sdvg_BeginVBO)     (sUI _numVertices, sUI _stride);
      sBool (YAC_CALL *sdvg_BeginVBO32)   (sUI _numVertices, sUI _stride);
      sBool (YAC_CALL *sdvg_BeginVBO14_2) (sUI _numVertices, sUI _stride);
      sBool (YAC_CALL *sdvg_BeginTriangles)     (sUI _numVertices, sUI _stride);
      sBool (YAC_CALL *sdvg_BeginTriangles32)   (sUI _numVertices, sUI _stride);
      sBool (YAC_CALL *sdvg_BeginTriangles14_2) (sUI _numVertices, sUI _stride);
      sBool (YAC_CALL *sdvg_BeginTriangleFan)       (sUI _numVertices, sUI _stride);
      sBool (YAC_CALL *sdvg_BeginTriangleFan32)     (sUI _numVertices, sUI _stride);
      sBool (YAC_CALL *sdvg_BeginTriangleFan14_2)   (sUI _numVertices, sUI _stride);
      sBool (YAC_CALL *sdvg_BeginTriangleStrip)     (sUI _numVertices, sUI _stride);
      sBool (YAC_CALL *sdvg_BeginTriangleStrip32)   (sUI _numVertices, sUI _stride);
      sBool (YAC_CALL *sdvg_BeginTriangleStrip14_2) (sUI _numVertices, sUI _stride);
      sBool (YAC_CALL *sdvg_BeginFilledTriangles)     (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginFilledTriangles32)   (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginFilledTriangles14_2) (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginFilledTriangleFan)     (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginFilledTriangleFan32)   (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginFilledTriangleFan14_2) (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginFilledTriangleStrip)     (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginFilledTriangleStrip32)   (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginFilledTriangleStrip14_2) (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginFilledGouraudTriangles)     (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginFilledGouraudTriangles32)   (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginFilledGouraudTriangles14_2) (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginFilledGouraudTriangleFan)   (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginFilledGouraudTriangleFan32)   (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginFilledGouraudTriangleFan14_2) (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginFilledGouraudTriangleStrip) (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginFilledGouraudModulateTriangles)     (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginFilledGouraudModulateTriangles32)   (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginFilledGouraudModulateTriangles14_2) (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginFilledGouraudModulateTriangleFan)     (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginFilledGouraudModulateTriangleFan32)   (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginFilledGouraudModulateTriangleFan14_2) (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginFilledGouraudModulateTriangleStrip)     (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginFilledGouraudModulateTriangleStrip32)   (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginFilledGouraudModulateTriangleStrip14_2) (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginTexturedTriangles)     (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginTexturedTriangles32)   (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginTexturedTriangles14_2) (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginTexturedTriangleFan)     (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginTexturedTriangleFan32)   (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginTexturedTriangleFan14_2) (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginTexturedTriangleStrip)     (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginTexturedTriangleStrip32)   (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginTexturedTriangleStrip14_2) (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginTexturedGouraudTriangles)     (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginTexturedGouraudTriangles32)   (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginTexturedGouraudTriangles14_2) (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginTexturedGouraudTriangleFan)     (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginTexturedGouraudTriangleFan32)   (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginTexturedGouraudTriangleFan14_2) (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginTexturedGouraudTriangleStrip)     (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginTexturedGouraudTriangleStrip32)   (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginTexturedGouraudTriangleStrip14_2) (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginTexturedTrianglesAlpha)     (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginTexturedTrianglesAlpha32)   (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginTexturedTrianglesAlpha14_2) (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginTexturedTriangleFanAlpha)     (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginTexturedTriangleFanAlpha32)   (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginTexturedTriangleFanAlpha14_2) (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginTexturedTriangleStripAlpha)     (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginTexturedTriangleStripAlpha32)   (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginTexturedTriangleStripAlpha14_2) (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginTexturedGouraudTrianglesAlpha)     (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginTexturedGouraudTrianglesAlpha32)   (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginTexturedGouraudTrianglesAlpha14_2) (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginTexturedGouraudTriangleFanAlpha)     (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginTexturedGouraudTriangleFanAlpha32)   (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginTexturedGouraudTriangleFanAlpha14_2) (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginTexturedGouraudTriangleStripAlpha)     (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginTexturedGouraudTriangleStripAlpha32)   (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginTexturedGouraudTriangleStripAlpha14_2) (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginTexturedTrianglesAlphaSDF)     (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginTexturedTrianglesAlphaSDF32)   (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginTexturedTrianglesAlphaSDF14_2) (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginLineStrip)        (sUI _numPoints);
      sBool (YAC_CALL *sdvg_BeginLineStripAA)      (sUI _numPoints);
      sBool (YAC_CALL *sdvg_BeginLineStripPattern)      (sUI _numPoints);
      sBool (YAC_CALL *sdvg_BeginLineStripPatternAA)    (sUI _numPoints);
      sBool (YAC_CALL *sdvg_BeginLineStripPatternDecal) (sUI _numPoints);
      sBool (YAC_CALL *sdvg_BeginLineStripBevel)         (sUI _numPoints);
      sBool (YAC_CALL *sdvg_BeginLineStripBevelClosed)   (sUI _numPoints);
      sBool (YAC_CALL *sdvg_BeginLineStripBevelAA)       (sUI _numPoints);
      sBool (YAC_CALL *sdvg_BeginLineStripBevelAAClosed) (sUI _numPoints);
      sBool (YAC_CALL *sdvg_BeginLineStripPatternBevel)         (sUI _numPoints);
      sBool (YAC_CALL *sdvg_BeginLineStripPatternBevelClosed)   (sUI _numPoints);
      sBool (YAC_CALL *sdvg_BeginLineStripPatternBevelAA)       (sUI _numPoints);
      sBool (YAC_CALL *sdvg_BeginLineStripPatternBevelAAClosed) (sUI _numPoints);
      sBool (YAC_CALL *sdvg_BeginLineStripPatternDecalBevel)         (sUI _numPoints);
      sBool (YAC_CALL *sdvg_BeginLineStripPatternDecalBevelClosed)   (sUI _numPoints);
      sBool (YAC_CALL *sdvg_BeginLineStripPatternDecalBevelAA)       (sUI _numPoints);
      sBool (YAC_CALL *sdvg_BeginLineStripPatternDecalBevelAAClosed) (sUI _numPoints);
      sBool (YAC_CALL *sdvg_BeginLineStripMiter)         (sUI _numPoints);
      sBool (YAC_CALL *sdvg_BeginLineStripMiterClosed)   (sUI _numPoints);
      sBool (YAC_CALL *sdvg_BeginLineStripMiterAA)       (sUI _numPoints);
      sBool (YAC_CALL *sdvg_BeginLineStripMiterAAClosed) (sUI _numPoints);
      sBool (YAC_CALL *sdvg_BeginLineStripPatternMiter)         (sUI _numPoints);
      sBool (YAC_CALL *sdvg_BeginLineStripPatternMiterClosed)   (sUI _numPoints);
      sBool (YAC_CALL *sdvg_BeginLineStripPatternMiterAA)       (sUI _numPoints);
      sBool (YAC_CALL *sdvg_BeginLineStripPatternMiterAAClosed) (sUI _numPoints);
      sBool (YAC_CALL *sdvg_BeginLineStripPatternDecalMiter)         (sUI _numPoints);
      sBool (YAC_CALL *sdvg_BeginLineStripPatternDecalMiterClosed)   (sUI _numPoints);
      sBool (YAC_CALL *sdvg_BeginLineStripPatternDecalMiterAA)       (sUI _numPoints);
      sBool (YAC_CALL *sdvg_BeginLineStripPatternDecalMiterAAClosed) (sUI _numPoints);
      sBool (YAC_CALL *sdvg_BeginLines)   (sUI _numPoints);
      sBool (YAC_CALL *sdvg_BeginLinesAA) (sUI _numPoints);
      sBool (YAC_CALL *sdvg_BeginLinesGouraud)   (sUI _numPoints);
      sBool (YAC_CALL *sdvg_BeginLinesGouraudAA) (sUI _numPoints);
      sBool (YAC_CALL *sdvg_BeginLinesPattern)   (sUI _numPoints);
      sBool (YAC_CALL *sdvg_BeginLinesPatternAA) (sUI _numPoints);
      sBool (YAC_CALL *sdvg_BeginPointsSquare)     (sUI _numPoints);
      sBool (YAC_CALL *sdvg_BeginPointsSquareAA)   (sUI _numPoints);
      sBool (YAC_CALL *sdvg_BeginPointsRound)      (sUI _numPoints);
      sBool (YAC_CALL *sdvg_BeginPointsRoundAA)    (sUI _numPoints);
      sBool (YAC_CALL *sdvg_BeginPolygon)         (sUI _numVertices, sUI _stride);
      sBool (YAC_CALL *sdvg_BeginPolygon32)       (sUI _numVertices, sUI _stride);
      sBool (YAC_CALL *sdvg_BeginPolygon14_2)     (sUI _numVertices, sUI _stride);
      sBool (YAC_CALL *sdvg_BeginPolygonAA)       (sUI _numVertices, sUI _stride);
      sBool (YAC_CALL *sdvg_BeginPolygonAA32)     (sUI _numVertices, sUI _stride);
      sBool (YAC_CALL *sdvg_BeginPolygonAA14_2)   (sUI _numVertices, sUI _stride);
      sBool (YAC_CALL *sdvg_BeginFilledPolygon)   (sUI _numVertices);
      sBool (YAC_CALL *sdvg_BeginFilledPolygonAA) (sUI _numVertices);
      void  (YAC_CALL *sdvg_VertexOffset2f)        (void);
      void  (YAC_CALL *sdvg_VertexOffset2fi16)     (void);
      void  (YAC_CALL *sdvg_AttribOffsetf)         (const char *_name, sUI _size);
      void  (YAC_CALL *sdvg_AttribOffset1f)        (const char *_name);
      void  (YAC_CALL *sdvg_AttribOffset2f)        (const char *_name);
      void  (YAC_CALL *sdvg_AttribOffset3f)        (const char *_name);
      void  (YAC_CALL *sdvg_AttribOffset4f)        (const char *_name);
      void  (YAC_CALL *sdvg_AttribOffsetARGB)      (const char *_name);
      void  (YAC_CALL *sdvg_Attrib1f)              (sF32 _f);
      void  (YAC_CALL *sdvg_Attrib1i16)            (sS16 _i);
      void  (YAC_CALL *sdvg_Attrib1fx)             (sF32 _f);
      void  (YAC_CALL *sdvg_Attrib2f)              (sF32 _f1, sF32 _f2);
      void  (YAC_CALL *sdvg_Attrib2i16)            (sS16 _i1, sS16 _i2);
      void  (YAC_CALL *sdvg_Attrib2fx)             (sF32 _f1, sF32 _f2);
      void  (YAC_CALL *sdvg_Vertex2f)              (sF32 _x, sF32 _y);
      void  (YAC_CALL *sdvg_Vertex2x)              (sS16 _x, sS16 _y);
      void  (YAC_CALL *sdvg_Vertex3f)              (sF32 _x, sF32 _y, sF32 _z);
      void  (YAC_CALL *sdvg_Vertex3x)              (sS16 _x, sS16 _y, sS16 _z);
      void  (YAC_CALL *sdvg_TexCoord2f)            (sF32 _u, sF32 _v);
      void  (YAC_CALL *sdvg_Attrib3f)              (sF32 _f1, sF32 _f2, sF32 _f3);
      void  (YAC_CALL *sdvg_Attrib3i16)            (sS16 _f1, sS16 _f2, sS16 _f3);
      void  (YAC_CALL *sdvg_Attrib3fx)             (sF32 _f1, sF32 _f2, sF32 _f3);
      void  (YAC_CALL *sdvg_Color3f)               (sF32 _r, sF32 _g, sF32 _b);
      void  (YAC_CALL *sdvg_Attrib4f)              (sF32 _f1, sF32 _f2, sF32 _f3, sF32 _f4);
      void  (YAC_CALL *sdvg_Color4f)               (sF32 _r, sF32 _g, sF32 _b, sF32 _a);
      void  (YAC_CALL *sdvg_AttribARGB)            (sUI _c32);
      void  (YAC_CALL *sdvg_ColorARGB)             (sUI _c32);
      void  (YAC_CALL *sdvg_End)                   (void);
      // ----------- additional (scratchbuffer) draw functions ------------
      void (YAC_CALL *sdvg_DrawFilledRectangle) (sF32 _x, sF32 _y, sF32 _w, sF32 _h);
      void (YAC_CALL *sdvg_DrawRectangle) (sF32 _x, sF32 _y, sF32 _w, sF32 _h, sF32 _b);
      // ----------- color utilities ------------
      sU32 (YAC_CALL *sdvg_ARGB)         (sU8 _a, sU8 _r, sU8 _g, sU8 _b);
      sU32 (YAC_CALL *sdvg_ARGBf)        (sF32 _a, sF32 _r, sF32 _g, sF32 _b);
      sU32 (YAC_CALL *sdvg_MixARGBx)     (sU32 _x, sU32 _y, sU16 _t);
      sU32 (YAC_CALL *sdvg_MixARGBf)     (sU32 _x, sU32 _y, sF32 _t);
      sU32 (YAC_CALL *sdvg_MixRGBx)      (sU32 _x, sU32 _y, sU16 _t);
      sU32 (YAC_CALL *sdvg_MixRGBf)      (sU32 _x, sU32 _y, sF32 _t);
      sU32 (YAC_CALL *sdvg_TintARGB)     (sU32 _x, sU32 _y);
      sU32 (YAC_CALL *sdvg_TintRGBAlpha) (sU32 _x, sU32 _y, sU8 _a8);
      sU32 (YAC_CALL *sdvg_RGBAlpha)     (sU32 _c32, sU8 _a8);
      sU32 (YAC_CALL *sdvg_HSVAToARGB)   (sF32 _h, sF32 _s, sF32 _v, sU8 _a8);
      sU8  (YAC_CALL *sdvg_ARGBToHSVA)   (sU32 _c32, sF32 *_retH, sF32 *_retS, sF32 *_retV);
      void (YAC_CALL *sdvg_GradientToTexture) (sU32 *_dst, sU32 _dstW, const sU32 *_colors, sU32 _numColors, const sSI *_starts, sUI _numStarts, sBool _bSmoothStep);
   } tkminnie_shared_t;
}

#ifndef TKMINNIE_BUILD
extern tkminnie_shared_t *tkminnie_shared;
extern void tkminnie_shared_resolve(void);
#endif

#endif // TKMINNIE_SHARED_H__
