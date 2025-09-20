// ----
// ---- file   : shadervg.h
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
// ----
// ----
// ----

#ifndef SHADERVG_H__
#define SHADERVG_H__

YG("shadervg");


class ShaderVG_Shape;

// texture formats
#ifdef SHADERVG_SCRIPT_API
YC class __sdvg_constants_class : public YAC_Object {
  public:
   __sdvg_constants_class(void) {}
   ~__sdvg_constants_class() {}
   YAC(__sdvg_constants_class);
#endif // SHADERVG_SCRIPT_API

#define SDVG_TEXFMT_ALPHA8    YCI 1
#define SDVG_TEXFMT_RGB565    YCI 2
#define SDVG_TEXFMT_resvd_3   YCI 3
#define SDVG_TEXFMT_BGRA8888  YCI 4  // b,g,r,a bytes
#define SDVG_TEXFMT_ARGB32    YCI 4  // alias for BGRA8888 (little endian)
#define SDVG_TEXFMT_RGBA8888  YCI 5  // r,g,b,a bytes

#ifdef SHADERVG_SCRIPT_API
};
#endif // SHADERVG_SCRIPT_API

#ifndef SHADERVG_SKIP_DECLARATIONS


// -------- init/exit --------
YF void YAC_CALL sdvg_SetGLSLVersion (sBool _bV3, sBool _bGLES, YAC_String *_sVersionStringOrNull);
YF void YAC_CALL sdvg_SetScratchBufferSize (sUI _szBytes);
YF sBool YAC_CALL sdvg_Init (sBool _bGLCore);
YF void YAC_CALL sdvg_Exit (void);
YF sBool YAC_CALL sdvg_OnOpen (void);
YF void YAC_CALL sdvg_SetFramebufferSize (sUI _w, sUI _h);

// -------- begin/end frame --------
YF void YAC_CALL sdvg_BeginFrame (void);
YF void YAC_CALL sdvg_ReturnToGL (void);
YF void YAC_CALL sdvg_EndFrame (void);

// -------- viewport --------
YF void YAC_CALL sdvg_SetViewport (sUI _x, sUI _y, sUI _w, sUI _h);
YF void YAC_CALL sdvg_PushViewport (sUI _x, sUI _y, sUI _w, sUI _h);
YF void YAC_CALL sdvg_PopViewport (void);
YF sUI YAC_CALL sdvg_GetViewportWidth (void);
YF sUI YAC_CALL sdvg_GetViewportHeight (void);

// -------- scissor --------
YF void YAC_CALL sdvg_SetScissor (sUI _x, sUI _y, sUI _w, sUI _h);
YF void YAC_CALL sdvg_PushScissor (sUI _x, sUI _y, sUI _w, sUI _h);
YF void YAC_CALL sdvg_PopScissor (void);
YF void YAC_CALL sdvg_EnableScissor (void);
YF void YAC_CALL sdvg_DisableScissor (void);
YF void YAC_CALL sdvg_Flush (void);
// (note) helper function that unbinds/unmaps (scratch) VBO and shader program
// (note) needed during app transition from GL to ShaderVG/Minnie API
// (note) !! caution !! this call is _very_ expensive (~150x slowdown when called frequently)

// -------- transform --------
YF void YAC_CALL sdvg_SetTransform (YAC_Object *_mat4);
YF YAC_Object *YAC_CALL sdvg_GetTransformRef(void);

// -------- AA --------
YF void YAC_CALL sdvg_SetEnableAA (sBool _bEnable);
YF void YAC_CALL sdvg_SetAARange(sF32 _aaRange);
YF void YAC_CALL sdvg_SetAAExp (sF32 _aaExp);

// -------- render state (fill / stroke) --------
YF void YAC_CALL sdvg_SetFillColor4f (sF32 _fillR, sF32 _fillG, sF32 _fillB, sF32 _fillA);
YF void YAC_CALL sdvg_SetFillAlpha (sF32 _fillA);
YF void YAC_CALL sdvg_SetFillColorARGB (sUI _c32);
YF void YAC_CALL sdvg_SetColorARGB (sUI _c32);
YF void YAC_CALL sdvg_SetStrokeWidth (sF32 _strokeW);
YF void YAC_CALL sdvg_SetPointRadius (sF32 _radius);
YF void YAC_CALL sdvg_SetPixelScaling (sF32 _s);
YF void YAC_CALL sdvg_SetStrokeColor4f (sF32 _strokeR, sF32 _strokeG, sF32 _strokeB, sF32 _strokeA);
YF void YAC_CALL sdvg_SetStrokeColorARGB (sU32 _c32);
YF void YAC_CALL sdvg_SetGlobalAlpha (sF32 _a);
YF void YAC_CALL sdvg_SetTextureDecalAlpha (sF32 _decalAlpha);

// -------- clear --------
YF void YAC_CALL sdvg_Clear4f (sF32 _r, sF32 _g, sF32 _b, sF32 _a);
YF void YAC_CALL sdvg_ClearARGB (sUI _c32);

// -------- blending --------
YF void YAC_CALL sdvg_EnableBlending (void);
YF void YAC_CALL sdvg_EnableBlendingKeepAlpha (void);
YF void YAC_CALL sdvg_EnableBlendingAdditive (void);
YF void YAC_CALL sdvg_EnableBlendingAdditiveKeepAlpha (void);
YF void YAC_CALL sdvg_EnableBlendingSrcColorKeepAlpha (void);
YF void YAC_CALL sdvg_EnableBlendingDstColorKeepAlpha (void);
YF void YAC_CALL sdvg_DisableBlending (void);
YF void YAC_CALL sdvg_AlphaWrite (sBool _bEnable);

// -------- configuration / debug --------
YF sBool YAC_CALL sdvg_GetEnableDebug (void);
YF void YAC_CALL sdvg_SetEnableDebug (sBool _bEnable);
YF sBool YAC_CALL sdvg_GetEnableDrawInner (void);
YF void YAC_CALL sdvg_SetEnableDrawInner (sBool _bEnable);
YF sBool YAC_CALL sdvg_GetEnableDrawBorder (void);
YF void YAC_CALL sdvg_SetEnableDrawBorder (sBool _bEnable);
YF void YAC_CALL sdvg_DebugSetEnableWriteVBO (sBool _bEnable);

// -------- VBO --------
// (note) returns OpenGL VBO id (0 == failed)
YF sUI YAC_CALL sdvg_CreateVBO (sUI _numBytes);
YF void YAC_CALL sdvg_UpdateVBO (sUI _vboId, sUI _offset, sUI _numBytes, YAC_Object *_data);
YF void YAC_CALL sdvg_BindVBO (sUI _vboId);
YF sUI YAC_CALL sdvg_GetVBOSize (void);
YF sUI YAC_CALL sdvg_GetMappedVBOOffset (void);
YF void YAC_CALL sdvg_MapVBO (sUI _vboId);
YF void YAC_CALL sdvg_UnmapVBO (void);
YF void YAC_CALL sdvg_UnbindVBO (void);
YF void YAC_CALL sdvg_DestroyVBO (sUI _vboId);

// -------- FBO --------
// (note) returns ShaderVG FBO index (0 == failed)
YF sUI YAC_CALL sdvg_CreateFBOEx (sUI _w, sUI _h, sBool _bMSAA, sBool _bDepth, sBool _bStencil);
YF sUI YAC_CALL sdvg_CreateFBO (sUI _w, sUI _h);
YF void YAC_CALL sdvg_BindFBO (sUI _fboIdx);
YF void YAC_CALL sdvg_ResolveFBO (sUI _fboIdx);
YF void YAC_CALL sdvg_UnbindFBO (void);
YF sUI YAC_CALL sdvg_GetFBOTextureId (sUI _fboIdx);
YF void YAC_CALL sdvg_BindFBOTexture (sUI _fboIdx, sBool _bRepeat, sBool _bFilter);
YF void YAC_CALL sdvg_DestroyFBO (sUI _fboIdx);

// -------- texturing --------
sUI YAC_CALL sdvg_CreateTexture2D (sUI _texfmt, sUI _w, sUI _h, const void *_data, sUI _dataSz);
#ifdef SHADERVG_SCRIPT_API
YF sUI YAC_CALL _sdvg_CreateTexture2D (sUI _texfmt, sUI _w, sUI _h, YAC_Object *_data);
#endif // SHADERVG_SCRIPT_API
YF void YAC_CALL sdvg_BindMultiTexture2D (sUI _unitIdx, sUI _texId, sBool _bRepeat, sBool _bFilter);
YF void YAC_CALL sdvg_UnbindMultiTexture2D (sUI _unitIdx);
YF void YAC_CALL sdvg_BindTexture2D (sUI _texId, sBool _bRepeat, sBool _bFilter);
YF void YAC_CALL sdvg_UnbindTexture2D (void);
YF void YAC_CALL sdvg_DestroyTexture2D (sUI _texId);

// -------- VBO utility functions --------
YF void YAC_CALL sdvg_BufferAddRectTexUVFlat32 (YAC_Buffer *_b, sF32 _x, sF32 _y, sF32 _w, sF32 _h, sF32 _ul, sF32 _ut, sF32 _ur, sF32 _ub);
YF void YAC_CALL sdvg_WriteC32AsRGBA8 (YAC_Buffer *_b, sUI _c32);
YF void YAC_CALL sdvg_BufferAddRectTexUVGouraud32 (YAC_Buffer *_b,  sF32 _x, sF32 _y, sF32 _w, sF32 _h, sF32 _ul, sF32 _ut, sF32 _ur, sF32 _ub, sUI _c32LT, sUI _c32RT, sUI _c32LB, sUI _c32RB);
YF void YAC_CALL sdvg_BufferAddLinePointFlat14_2 (YAC_Buffer *_b, sF32 _x, sF32 _y);
YF void YAC_CALL sdvg_BufferAddLinePointFlat32 (YAC_Buffer *_b, sF32 _x, sF32 _y);
YF void YAC_CALL sdvg_BufferAddLinePointFlatBevel14_2 (YAC_Buffer *_b, sF32 _x, sF32 _y);
YF void YAC_CALL sdvg_BufferAddLinePointFlatBevel32 (YAC_Buffer *_b, sF32 _x, sF32 _y);
YF void YAC_CALL sdvg_BufferAddLinesPointsFlat14_2 (YAC_Buffer *_b, sF32 _x1, sF32 _y1, sF32 _x2, sF32 _y2);
YF void YAC_CALL sdvg_BufferAddLinesPointsFlat32 (YAC_Buffer *_b, sF32 _x1, sF32 _y1, sF32 _x2, sF32 _y2);

// -------- (low level) draw functions --------
YF void YAC_CALL sdvg_DrawTrianglesFillFlatVBO32 (sUI _vboId, sUI _byteOffset, sUI _numTris);
YF void YAC_CALL sdvg_DrawTrianglesFillFlatVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numTris);
YF void YAC_CALL sdvg_DrawTrianglesFillGouraudVBO32 (sUI _vboId, sUI _byteOffset, sUI _numTris);
YF void YAC_CALL sdvg_DrawTrianglesFillGouraudVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numTris);
YF void YAC_CALL sdvg_DrawTrianglesFillFlatEdgeAAVBO32 (sUI _vboId, sUI _byteOffset, sUI _numTris);
YF void YAC_CALL sdvg_DrawTrianglesFillFlatEdgeAAVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numTris);
YF void YAC_CALL sdvg_DrawTrianglesFillGouraudEdgeAAVBO32 (sUI _vboId, sUI _byteOffset, sUI _numTris);
YF void YAC_CALL sdvg_DrawTrianglesFillGouraudEdgeAAVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numTris);
YF void YAC_CALL sdvg_DrawPolygonFillFlatVBO32 (sUI _vboId, sUI _byteOffset, sUI _numVerts);
YF void YAC_CALL sdvg_DrawPolygonFillFlatVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numVerts);
YF void YAC_CALL sdvg_DrawPolygonFillGouraudVBO32 (sUI _vboId, sUI _byteOffset, sUI _numVerts);
YF void YAC_CALL sdvg_DrawPolygonFillGouraudVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numVerts);
YF void YAC_CALL sdvg_SetupRectFillAAVBO32 (YAC_Buffer *_vb, YAC_Buffer *_dl, sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY);
YF void YAC_CALL sdvg_DrawRectFillAAVBO32 (sUI _vboId, sUI _byteOffsetInner, sUI _numVertsInner, sUI _byteOffsetBorder, sUI _numVertsBorder, sUI _glPrimTypeBorder, sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY);
YF void YAC_CALL sdvg_DrawRectFillAA (sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY);
YF void YAC_CALL sdvg_SetupRectFillStrokeAAVBO32 (YAC_Buffer *_vb, YAC_Buffer *_dl, sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY, sF32 _strokeW);
YF void YAC_CALL sdvg_DrawRectFillStrokeAAVBO32 (sUI _vboId, sUI _byteOffsetInner, sUI _numVertsInner, sUI _byteOffsetBorder, sUI _numVertsBorder, sUI _glPrimTypeBorder, sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY);
YF void YAC_CALL sdvg_DrawRectFillStrokeAA (sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY);
YF void YAC_CALL sdvg_SetupRectStrokeAAVBO32(YAC_Buffer *_vb, YAC_Buffer *_dl, sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY, sF32 _strokeW);
YF void YAC_CALL sdvg_DrawRectStrokeAAVBO32 (sUI _vboId, sUI _byteOffsetBorder, sUI _numVertsBorder, sUI _glPrimTypeBorder, sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY);
YF void YAC_CALL sdvg_DrawRectStrokeAA (sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY);
YF void YAC_CALL sdvg_SetupEllipseFillAAVBO32 (YAC_Buffer *_vb, YAC_Buffer *_dl, sF32 _centerX, sF32 _centerY, sF32 _radiusX, sF32 _radiusY);
YF void YAC_CALL sdvg_DrawEllipseFillAAVBO32 (sUI _vboId, sUI _byteOffsetInner, sUI _numVertsInner, sUI _byteOffsetBorder, sUI _numVertsBorder, sUI _glPrimTypeBorder, sF32 _centerX, sF32 _centerY, sF32 _radiusX, sF32 _radiusY);
YF void YAC_CALL sdvg_DrawEllipseFillAA (sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY);
YF void YAC_CALL sdvg_SetupEllipseFillStrokeAAVBO32 (YAC_Buffer *_vb, YAC_Buffer *_dl, sF32 _centerX, sF32 _centerY, sF32 _radiusX, sF32 _radiusY, sF32 _strokeW);
YF void YAC_CALL sdvg_DrawEllipseFillStrokeAAVBO32 (sUI _vboId, sUI _byteOffsetInner, sUI _numVertsInner, sUI _byteOffsetBorder, sUI _numVertsBorder, sUI _glPrimTypeBorder, sF32 _centerX, sF32 _centerY, sF32 _radiusX, sF32 _radiusY);
YF void YAC_CALL sdvg_DrawEllipseFillStrokeAA (sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY);
YF void YAC_CALL sdvg_SetupEllipseStrokeAAVBO32 (YAC_Buffer *_vb, YAC_Buffer *_dl, sF32 _centerX, sF32 _centerY, sF32 _radiusX, sF32 _radiusY, sF32 _strokeW);
YF void YAC_CALL sdvg_DrawEllipseStrokeAAVBO32 (sUI _vboId, sUI _byteOffsetBorder, sUI _numVertsBorder, sUI _glPrimTypeBorder, sF32 _centerX, sF32 _centerY, sF32 _radiusX, sF32 _radiusY);
YF void YAC_CALL sdvg_DrawEllipseStrokeAA (sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY);
YF void YAC_CALL sdvg_SetupRoundRectFillAAVBO32(YAC_Buffer *_vb, YAC_Buffer *_dl, sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY, sF32 _radiusX, sF32 _radiusY);
YF void YAC_CALL sdvg_DrawRoundRectFillAAVBO32 (sUI _vboId, sUI _byteOffsetInner, sUI _numVertsInner, sUI _byteOffsetBorder, sUI _numVertsBorder, sUI _glPrimTypeBorder, sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY, sF32 _radiusX, sF32 _radiusY);
YF void YAC_CALL sdvg_DrawRoundRectFillAA (sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY, sF32 _radiusX, sF32 _radiusY);
YF void YAC_CALL sdvg_SetupRoundRectFillStrokeAAVBO32 (YAC_Buffer *_vb, YAC_Buffer *_dl, sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY, sF32 _radiusX, sF32 _radiusY, sF32 _strokeW);
YF void YAC_CALL sdvg_DrawRoundRectFillStrokeAAVBO32 (sUI _vboId, sUI _byteOffsetInner, sUI _numVertsInner, sUI _byteOffsetBorder, sUI _numVertsBorder, sUI _glPrimTypeBorder, sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY, sF32 _radiusX, sF32 _radiusY);
YF void YAC_CALL sdvg_DrawRoundRectFillStrokeAA (sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY, sF32 _radiusX, sF32 _radiusY);
YF void YAC_CALL sdvg_SetupRoundRectStrokeAAVBO32 (YAC_Buffer *_vb, YAC_Buffer *_dl, sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY, sF32 _radiusX, sF32 _radiusY, sF32 _strokeW);
YF void YAC_CALL sdvg_DrawRoundRectStrokeAAVBO32(sUI _vboId, sUI _byteOffsetBorder, sUI _numVertsBorder, sUI _glPrimTypeBorder, sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY, sF32 _radiusX, sF32 _radiusY);
YF void YAC_CALL sdvg_DrawRoundRectStrokeAA (sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY, sF32 _radiusX, sF32 _radiusY);
YF void YAC_CALL sdvg_DrawTrianglesTexUVFlatVBO32 (sUI _vboId, sUI _byteOffset, sUI _numTris);
YF void YAC_CALL sdvg_DrawTrianglesTexUVGouraudVBO32 (sUI _vboId, sUI _byteOffset, sUI _numTris);
YF void YAC_CALL sdvg_DrawTrianglesTexUVFlatDecalVBO32 (sUI _vboId, sUI _byteOffset, sUI _numTris);
YF void YAC_CALL sdvg_DrawTrianglesTexUVGouraudDecalVBO32 (sUI _vboId, sUI _byteOffset, sUI _numTris);
YF void YAC_CALL sdvg_DrawLineStripFlatVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numPoints);
YF void YAC_CALL sdvg_DrawLineStripFlatVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints);
YF void YAC_CALL sdvg_DrawLineStripFlatAAVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numPoints);
YF void YAC_CALL sdvg_DrawLineStripFlatAAVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints);
YF void YAC_CALL sdvg_DrawLineStripFlatBevelVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numPoints);
YF void YAC_CALL sdvg_DrawLineStripFlatBevelVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints);
YF void YAC_CALL sdvg_DrawLineStripFlatBevelAAVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numPoints);
YF void YAC_CALL sdvg_DrawLineStripFlatBevelAAVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints);
YF void YAC_CALL sdvg_DrawLinesFlatVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numPoints);
YF void YAC_CALL sdvg_DrawLinesFlatVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints);
YF void YAC_CALL sdvg_DrawLinesFlatAAVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numPoints);
YF void YAC_CALL sdvg_DrawLinesFlatAAVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints);
YF void YAC_CALL sdvg_DrawPointsSquareVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints);
YF void YAC_CALL sdvg_DrawPointsSquareAAVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints);
YF void YAC_CALL sdvg_DrawPointsRoundVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints);
YF void YAC_CALL sdvg_DrawPointsRoundAAVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

// ----------- custom shaders and (scratch) draw functions ------------
sUI YAC_CALL sdvg_CreateShader (const char *vs, const char *fs);
YF void YAC_CALL sdvg_DestroyShader (sUI _shaderIdx);
YF void YAC_CALL sdvg_BindShader (sUI _shaderIdx);
YF void YAC_CALL sdvg_UnbindShader (void);
sSI YAC_CALL sdvg_GetUniformLocation (const char *_name);
sSI YAC_CALL sdvg_GetAttribLocation (const char *_name);
void YAC_CALL sdvg_Uniform1f (const char *_name, sF32 _f);
void YAC_CALL sdvg_Uniform2f (const char *_name, sF32 _f1, sF32 _f2);
void YAC_CALL sdvg_Uniform3f (const char *_name, sF32 _f1, sF32 _f2, sF32 _f3);
void YAC_CALL sdvg_Uniform4f (const char *_name, sF32 _f1, sF32 _f2, sF32 _f3, sF32 _f4);
void YAC_CALL sdvg_Uniform1i (const char *_name, sSI _i);
#ifdef SHADERVG_SCRIPT_API
YF sUI YAC_CALL _sdvg_CreateShader (YAC_String *vs, YAC_String *fs);
YF sSI YAC_CALL _sdvg_GetUniformLocation (YAC_String *_name);
YF sSI YAC_CALL _sdvg_GetAttribLocation (YAC_String *_name);
YF void YAC_CALL _sdvg_UniformMatrix4 (YAC_String *_name, YAC_Object *_matRowMajor);
YF void YAC_CALL _sdvg_Uniform1f (YAC_String *_name, sF32 _f);
YF void YAC_CALL _sdvg_Uniform2f (YAC_String *_name, sF32 _f1, sF32 _f2);
YF void YAC_CALL _sdvg_Uniform3f (YAC_String *_name, sF32 _f1, sF32 _f2, sF32 _f3);
YF void YAC_CALL _sdvg_Uniform4f (YAC_String *_name, sF32 _f1, sF32 _f2, sF32 _f3, sF32 _f4);
YF void YAC_CALL _sdvg_Uniform1i (YAC_String *_name, sSI _i);
#endif // SHADERVG_SCRIPT_API

// -------- (high level) draw functions --------
YF sBool YAC_CALL sdvg_BeginVBO (sUI _numVertices, sUI _stride);
YF sBool YAC_CALL sdvg_BeginTriangles (sUI _numVertices, sUI _stride);
YF sBool YAC_CALL sdvg_BeginTriangleFan (sUI _numVertices, sUI _stride);
YF sBool YAC_CALL sdvg_BeginTriangleStrip (sUI _numVertices, sUI _stride);
YF sBool YAC_CALL sdvg_BeginFilledTriangles (sUI _numVertices);
YF sBool YAC_CALL sdvg_BeginFilledTriangleFan (sUI _numVertices);
YF sBool YAC_CALL sdvg_BeginFilledTriangleStrip (sUI _numVertices);
YF sBool YAC_CALL sdvg_BeginFilledGouraudTriangles (sUI _numVertices);
YF sBool YAC_CALL sdvg_BeginFilledGouraudTriangleFan (sUI _numVertices);
YF sBool YAC_CALL sdvg_BeginFilledGouraudTriangleStrip (sUI _numVertices);
YF sBool YAC_CALL sdvg_BeginTexturedTriangles (sUI _numVertices);
YF sBool YAC_CALL sdvg_BeginTexturedTriangleFan (sUI _numVertices);
YF sBool YAC_CALL sdvg_BeginTexturedTriangleStrip (sUI _numVertices);
YF sBool YAC_CALL sdvg_BeginTexturedGouraudTriangles (sUI _numVertices);
YF sBool YAC_CALL sdvg_BeginTexturedGouraudTriangleFan (sUI _numVertices);
YF sBool YAC_CALL sdvg_BeginTexturedGouraudTriangleStrip (sUI _numVertices);
YF sBool YAC_CALL sdvg_BeginTexturedTrianglesAlpha (sUI _numVertices);
YF sBool YAC_CALL sdvg_BeginTexturedTriangleFanAlpha (sUI _numVertices);
YF sBool YAC_CALL sdvg_BeginTexturedTriangleStripAlpha (sUI _numVertices);
YF sBool YAC_CALL sdvg_BeginTexturedGouraudTrianglesAlpha (sUI _numVertices);
YF sBool YAC_CALL sdvg_BeginTexturedGouraudTriangleFanAlpha (sUI _numVertices);
YF sBool YAC_CALL sdvg_BeginTexturedGouraudTriangleStripAlpha (sUI _numVertices);
YF sBool YAC_CALL sdvg_BeginLineStrip (sUI _numPoints);
YF sBool YAC_CALL sdvg_BeginLineStripAA (sUI _numPoints);
YF sBool YAC_CALL sdvg_BeginLineStripBevel (sUI _numPoints);
YF sBool YAC_CALL sdvg_BeginLineStripBevelAA (sUI _numPoints);
YF sBool YAC_CALL sdvg_BeginLines (sUI _numPoints);
YF sBool YAC_CALL sdvg_BeginLinesAA (sUI _numPoints);
YF sBool YAC_CALL sdvg_BeginPointsSquare (sUI _numPoints);
YF sBool YAC_CALL sdvg_BeginPointsSquareAA (sUI _numPoints);
YF sBool YAC_CALL sdvg_BeginPointsRound (sUI _numPoints);
YF sBool YAC_CALL sdvg_BeginPointsRoundAA (sUI _numPoints);
YF void YAC_CALL sdvg_VertexOffset2f (void);
void YAC_CALL sdvg_AttribOffsetf (const char *_name, sUI _size);
void YAC_CALL sdvg_AttribOffset1f (const char *_name);
void YAC_CALL sdvg_AttribOffset2f (const char *_name);
void YAC_CALL sdvg_AttribOffset3f (const char *_name);
void YAC_CALL sdvg_AttribOffset4f (const char *_name);
void YAC_CALL sdvg_AttribOffsetARGB (const char *_name);
#ifdef SHADERVG_SCRIPT_API
YF void YAC_CALL _sdvg_AttribOffsetf (YAC_String *_name, sUI _size);
YF void YAC_CALL _sdvg_AttribOffset1f (YAC_String *_name);
YF void YAC_CALL _sdvg_AttribOffset2f (YAC_String *_name);
YF void YAC_CALL _sdvg_AttribOffset3f (YAC_String *_name);
YF void YAC_CALL _sdvg_AttribOffset4f (YAC_String *_name);
YF void YAC_CALL _sdvg_AttribOffsetARGB (YAC_String *_name);
YF void YAC_CALL _sdvg_Attribi16 (sSI _i);
#endif // SHADERVG_SCRIPT_API
void YAC_CALL sdvg_Attribi16 (sS16 _i);
YF void YAC_CALL sdvg_Attrib1f (sF32 _f);
YF void YAC_CALL sdvg_Attrib2f (sF32 _f1, sF32 _f2);
YF void YAC_CALL sdvg_Vertex2f (sF32 _x, sF32 _y);
YF void YAC_CALL sdvg_Vertex3f (sF32 _x, sF32 _y, sF32 _z);
YF void YAC_CALL sdvg_TexCoord2f (sF32 _u, sF32 _v);
YF void YAC_CALL sdvg_Attrib3f (sF32 _f1, sF32 _f2, sF32 _f3);
YF void YAC_CALL sdvg_Color3f (sF32 _r, sF32 _g, sF32 _b);
YF void YAC_CALL sdvg_Attrib4f (sF32 _f1, sF32 _f2, sF32 _f3, sF32 _f4);
YF void YAC_CALL sdvg_Color4f (sF32 _r, sF32 _g, sF32 _b, sF32 _a);
YF void YAC_CALL sdvg_AttribARGB (sUI _c32);
YF void YAC_CALL sdvg_ColorARGB (sUI _c32);
YF void YAC_CALL sdvg_End (void);

// ----------- additional (scratch) draw functions ------------
YF void YAC_CALL sdvg_DrawFilledRectangle (sF32 _x, sF32 _y, sF32 _w, sF32 _h);
YF void YAC_CALL sdvg_DrawRectangle (sF32 _x, sF32 _y, sF32 _w, sF32 _h, sF32 _b);

// ----------- color utilities ------------
sU32 sdvg_ARGB (sU8 _a, sU8 _r, sU8 _g, sU8 _b);
YF sU32 YAC_CALL sdvg_ARGBf (sF32 _a, sF32 _r, sF32 _g, sF32 _b);
sU32 sdvg_MixARGBx (sU32 _x, sU32 _y, sU16 _t);
YF sU32 YAC_CALL sdvg_MixARGBf (sU32 _x, sU32 _y, sF32 _t);
sU32 sdvg_MixRGBx (sU32 _x, sU32 _y, sU16 _t);
YF sU32 YAC_CALL sdvg_MixRGBf (sU32 _x, sU32 _y, sF32 _t);
YF sU32 YAC_CALL sdvg_TintARGB (sU32 _x, sU32 _y);
sU32 sdvg_TintRGBAlpha (sU32 _x, sU32 _y, sU8 _a8);
sU32 sdvg_RGBAlpha (sU32 _c32, sU8 _a8);
sU32 sdvg_HSVAToARGB (sF32 _h, sF32 _s, sF32 _v, sU8 _a8);
sU8 sdvg_ARGBToHSVA (sU32 _c32, sF32 *_retH, sF32 *_retS, sF32 *_retV);
#ifdef SHADERVG_SCRIPT_API
YF sU32 YAC_CALL _sdvg_ARGB (sUI _a, sUI _r, sUI _g, sUI _b);
YF sU32 YAC_CALL _sdvg_MixARGBx (sU32 _x, sU32 _y, sUI _t);
YF sU32 YAC_CALL _sdvg_MixRGBx (sU32 _x, sU32 _y, sUI _t);
YF sU32 YAC_CALL _sdvg_TintRGBAlpha (sU32 _x, sU32 _y, sUI _a8);
YF sU32 YAC_CALL _sdvg_RGBAlpha (sU32 _c32, sUI _a8);
YF sU32 YAC_CALL _sdvg_HSVAToARGB (sF32 _h, sF32 _s, sF32 _v, sUI _a8);
YF sUI YAC_CALL _sdvg_ARGBToHSVA (sU32 _c32, YAC_Object *_retH, YAC_Object *_retS, YAC_Object *_retV);
#endif // SHADERVG_SCRIPT_API


#endif // SHADERVG_SKIP_DECLARATIONS


#endif // SHADERVG_H__
