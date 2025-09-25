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
/* @function sdvg_SetGLSLVersion,boolean bV3,boolean bGLES,String sVersionStringOrNull
*/
YF void YAC_CALL sdvg_SetGLSLVersion (sBool _bV3, sBool _bGLES, YAC_String *_sVersionStringOrNull);

/* @function sdvg_SetScratchBufferSize,int szBytes
*/
YF void YAC_CALL sdvg_SetScratchBufferSize (sUI _szBytes);

/* @function sdvg_Init,boolean bGLCore:boolean
*/
YF sBool YAC_CALL sdvg_Init (sBool _bGLCore);

/* @function sdvg_Exit
*/
YF void YAC_CALL sdvg_Exit (void);

/* @function sdvg_OnOpen:boolean
*/
YF sBool YAC_CALL sdvg_OnOpen (void);

/* @function sdvg_SetFramebufferSize,int w,int h
*/
YF void YAC_CALL sdvg_SetFramebufferSize (sUI _w, sUI _h);

// -------- begin/end frame --------
/* @function sdvg_BeginFrame
*/
YF void YAC_CALL sdvg_BeginFrame (void);

/* @function sdvg_ReturnToGL
*/
YF void YAC_CALL sdvg_ReturnToGL (void);

/* @function sdvg_EndFrame
*/
YF void YAC_CALL sdvg_EndFrame (void);

// -------- viewport --------
/* @function sdvg_SetViewport,int x,int y,int w,int h
*/
YF void YAC_CALL sdvg_SetViewport (sUI _x, sUI _y, sUI _w, sUI _h);

/* @function sdvg_PushViewport,int x,int y,int w,int h
*/
YF void YAC_CALL sdvg_PushViewport (sUI _x, sUI _y, sUI _w, sUI _h);

/* @function sdvg_PopViewport
*/
YF void YAC_CALL sdvg_PopViewport (void);

/* @function sdvg_GetViewportWidth:int
*/
YF sUI YAC_CALL sdvg_GetViewportWidth (void);

/* @function sdvg_GetViewportHeight:int
*/
YF sUI YAC_CALL sdvg_GetViewportHeight (void);

// -------- scissor --------
/* @function sdvg_SetScissor,int x,int y,int w,int h
*/
YF void YAC_CALL sdvg_SetScissor (sUI _x, sUI _y, sUI _w, sUI _h);

/* @function sdvg_PushScissor,int x,int y,int w,int h
*/
YF void YAC_CALL sdvg_PushScissor (sUI _x, sUI _y, sUI _w, sUI _h);

/* @function sdvg_PopScissor
*/
YF void YAC_CALL sdvg_PopScissor (void);

/* @function sdvg_EnableScissor
*/
YF void YAC_CALL sdvg_EnableScissor (void);

/* @function sdvg_DisableScissor
*/
YF void YAC_CALL sdvg_DisableScissor (void);

/* @function sdvg_Flush
*/
YF void YAC_CALL sdvg_Flush (void);
// (note) helper function that unbinds/unmaps (scratch) VBO and shader program
// (note) needed during app transition from GL to ShaderVG/Minnie API
// (note) !! caution !! this call is _very_ expensive (~150x slowdown when called frequently)

// -------- transform --------
/* @function sdvg_SetTransform,Matrix4f mat4
*/
YF void YAC_CALL sdvg_SetTransform (YAC_Object *_mat4);

/* @function sdvg_GetTransformRef:Matrix4f
*/
YF YAC_Object *YAC_CALL sdvg_GetTransformRef (void);

// -------- AA --------
/* @function sdvg_SetEnableAA,boolean bEnable
*/
YF void YAC_CALL sdvg_SetEnableAA (sBool _bEnable);

/* @function sdvg_SetAARange,float aaRange
*/
YF void YAC_CALL sdvg_SetAARange (sF32 _aaRange);

/* @function sdvg_SetAAExp,float aaExp
*/
YF void YAC_CALL sdvg_SetAAExp (sF32 _aaExp);

// -------- render state (fill / stroke) --------
/* @function sdvg_SetFillColor4f,float fillR,float fillG,float fillB,float A
@arg fillR Red (0..1)
@arg fillG Green (0..1)
@arg fillB Blue (0..1)
@arg fillA Alpha (0..1)
*/
YF void YAC_CALL sdvg_SetFillColor4f (sF32 _fillR, sF32 _fillG, sF32 _fillB, sF32 _fillA);

/* @function sdvg_SetFillAlpha,float fillA
@arg fillA Alpha (0..1)
*/
YF void YAC_CALL sdvg_SetFillAlpha (sF32 _fillA);

/* @function sdvg_SetFillColorARGB,int c32
*/
YF void YAC_CALL sdvg_SetFillColorARGB (sUI _c32);

/* @function sdvg_SetColorARGB,int c32
*/
YF void YAC_CALL sdvg_SetColorARGB (sUI _c32);

/* @function sdvg_SetStrokeWidth,float strokeW
*/
YF void YAC_CALL sdvg_SetStrokeWidth (sF32 _strokeW);

/* @function sdvg_SetPointRadius,float radius
*/
YF void YAC_CALL sdvg_SetPointRadius (sF32 _radius);

/* @function sdvg_SetPixelScaling,float s
*/
YF void YAC_CALL sdvg_SetPixelScaling (sF32 _s);

/* @function sdvg_SetStrokeColor4f,float strokeR,float strokeG,float strokeB, float strokeA
@arg strokeR Red (0..1)
@arg strokeG Green(0..1)
@arg strokeB Blue (0..1)
@arg strokeA Alpha (0..1)
*/
YF void YAC_CALL sdvg_SetStrokeColor4f (sF32 _strokeR, sF32 _strokeG, sF32 _strokeB, sF32 _strokeA);

/* @function sdvg_SetStrokeColorARGB,int c32
*/
YF void YAC_CALL sdvg_SetStrokeColorARGB (sU32 _c32);

/* @function sdvg_SetGlobalAlpha,float a
*/
YF void YAC_CALL sdvg_SetGlobalAlpha (sF32 _a);

/* @function sdvg_SetTextureDecalAlpha,float decalAlpha
*/
YF void YAC_CALL sdvg_SetTextureDecalAlpha (sF32 _decalAlpha);

// -------- clear --------
/* @function sdvg_Clear4f,float r,float g,float b,float a
*/
YF void YAC_CALL sdvg_Clear4f (sF32 _r, sF32 _g, sF32 _b, sF32 _a);

/* @function sdvg_ClearARGB,int c32
*/
YF void YAC_CALL sdvg_ClearARGB (sUI _c32);

// -------- blending --------
/* @function sdvg_EnableBlending
*/
YF void YAC_CALL sdvg_EnableBlending (void);

/* @function sdvg_EnableBlendingKeepAlpha
*/
YF void YAC_CALL sdvg_EnableBlendingKeepAlpha (void);

/* @function sdvg_EnableBlendingAdditive
*/
YF void YAC_CALL sdvg_EnableBlendingAdditive (void);

/* @function sdvg_EnableBlendingAdditiveKeepAlpha
*/
YF void YAC_CALL sdvg_EnableBlendingAdditiveKeepAlpha (void);

/* @function sdvg_EnableBlendingSrcColorKeepAlpha
*/
YF void YAC_CALL sdvg_EnableBlendingSrcColorKeepAlpha (void);

/* @function sdvg_EnableBlendingDstColorKeepAlpha
*/
YF void YAC_CALL sdvg_EnableBlendingDstColorKeepAlpha (void);

/* @function sdvg_DisableBlending
*/
YF void YAC_CALL sdvg_DisableBlending (void);

/* @function sdvg_AlphaWrite,boolean bEnable
*/
YF void YAC_CALL sdvg_AlphaWrite (sBool _bEnable);

// -------- configuration / debug --------
/* @function sdvg_GetEnableDebug:boolean
*/
YF sBool YAC_CALL sdvg_GetEnableDebug (void);

/* @function sdvg_SetEnableDebug,boolean bEnable
*/
YF void YAC_CALL sdvg_SetEnableDebug (sBool _bEnable);

/* @function sdvg_GetEnableDrawInner:boolean
*/
YF sBool YAC_CALL sdvg_GetEnableDrawInner (void);

/* @function sdvg_SetEnableDrawInner,boolean bEnable
*/
YF void YAC_CALL sdvg_SetEnableDrawInner (sBool _bEnable);

/* @function sdvg_GetEnableDrawBorder:boolean
*/
YF sBool YAC_CALL sdvg_GetEnableDrawBorder (void);

/* @function sdvg_SetEnableDrawBorder,boolean bEnable
*/
YF void YAC_CALL sdvg_SetEnableDrawBorder (sBool _bEnable);

/* @function sdvg_DebugSetEnableWriteVBO,boolean bEnable
*/
YF void YAC_CALL sdvg_DebugSetEnableWriteVBO (sBool _bEnable);

// -------- VBO --------
/* @function sdvg_CreateVBO,int numBytes:int
@arg numBytes Vertex buffer object size (bytes)
@return return OpenGL VBO id (0 == failed)
*/
YF sUI YAC_CALL sdvg_CreateVBO (sUI _numBytes);

/* @function sdvg_UpdateVBO,int vboId,int offset,int numBytes,Object data
*/
YF void YAC_CALL sdvg_UpdateVBO (sUI _vboId, sUI _offset, sUI _numBytes, YAC_Object *_data);

/* @function sdvg_BindVBO,int vboId
*/
YF void YAC_CALL sdvg_BindVBO (sUI _vboId);

/* @function sdvg_GetVBOSize:int
*/
YF sUI YAC_CALL sdvg_GetVBOSize (void);

/* @function sdvg_GetMappedVBOOffset:int
*/
YF sUI YAC_CALL sdvg_GetMappedVBOOffset (void);

/* @function sdvg_MapVBO,int vboId
*/
YF void YAC_CALL sdvg_MapVBO (sUI _vboId);

/* @function sdvg_UnmapVBO
*/
YF void YAC_CALL sdvg_UnmapVBO (void);

/* @function sdvg_UnbindVBO
*/
YF void YAC_CALL sdvg_UnbindVBO (void);

/* @function sdvg_DestroyVBO,int vboId
*/
YF void YAC_CALL sdvg_DestroyVBO (sUI _vboId);

// -------- FBO --------
/* @function sdvg_CreateFBOEx,int w,int h,int bMSAA,int bDepth,int bStencil:int
@arg w Width
@arg h Height
@arg bMSAA
@arg bDepth
@arg bStencil  (note) Stencil FBOs may not be available before GL 4.5
@return ShaderVG FBO index (0 == failed)
*/
YF sUI YAC_CALL sdvg_CreateFBOEx (sUI _w, sUI _h, sBool _bMSAA, sBool _bDepth, sBool _bStencil);

/* @function sdvg_CreateFBO,int w,int h:int
@arg w Width
@arg h Height
@return ShaderVG FBO index (0 == failed)
*/
YF sUI YAC_CALL sdvg_CreateFBO (sUI _w, sUI _h);

/* @function sdvg_BindFBO,int fboIdx
*/
YF void YAC_CALL sdvg_BindFBO (sUI _fboIdx);

/* @function sdvg_ResolveFBO,int fboIdx
Resolve MSAA fbo
*/
YF void YAC_CALL sdvg_ResolveFBO (sUI _fboIdx);

/* @function sdvg_UnbindFBO
*/
YF void YAC_CALL sdvg_UnbindFBO (void);

/* @function sdvg_GetFBOTextureId,int fboIdx:int
@arg fboIdx ShaderVG fbo index
@return GL texture id
*/
YF sUI YAC_CALL sdvg_GetFBOTextureId (sUI _fboIdx);

/* @function sdvg_BindFBOTexture,int fboIdx,boolean bRepeat,boolean bFilter
*/
YF void YAC_CALL sdvg_BindFBOTexture (sUI _fboIdx, sBool _bRepeat, sBool _bFilter);

/* @function sdvg_DestroyFBO,int fboIdx
*/
YF void YAC_CALL sdvg_DestroyFBO (sUI _fboIdx);

// -------- texturing --------
/* @function sdvg_CreateTexture2D,int texFmt,int w,int h,Object data:int
@arg texFmt  §SDVG_TEXFMT_ALPHA8, §SDVG_TEXFMT_RGB565, §SDVG_TEXFMT_BGRA8888, §SDVG_TEXFMT_ARGB32, §SDVG_TEXFMT_RGBA8888, §
@arg w Width
@arg h Height
@arg data Initial pixel data or null (only reserve memory)
@return OpenGL texture id
*/
sUI YAC_CALL sdvg_CreateTexture2D (sUI _texfmt, sUI _w, sUI _h, const void *_data, sUI _dataSz);
#ifdef SHADERVG_SCRIPT_API
YF sUI YAC_CALL _sdvg_CreateTexture2D (sUI _texfmt, sUI _w, sUI _h, YAC_Object *_data);
#endif // SHADERVG_SCRIPT_API

/* @function sdvg_BindMultiTexture2D,int unitIdx,int texId,boolean bRepeat,boolean bFilter
*/
YF void YAC_CALL sdvg_BindMultiTexture2D (sUI _unitIdx, sUI _texId, sBool _bRepeat, sBool _bFilter);

/* @function sdvg_UnbindMultiTexture2D,int unitIdx
*/
YF void YAC_CALL sdvg_UnbindMultiTexture2D (sUI _unitIdx);

/* @function sdvg_BindTexture2D,int texId,boolean bRepeat,boolean bFilter
*/
YF void YAC_CALL sdvg_BindTexture2D (sUI _texId, sBool _bRepeat, sBool _bFilter);

/* @function sdvg_UnbindTexture2D
*/
YF void YAC_CALL sdvg_UnbindTexture2D (void);

/* @function sdvg_DestroyTexture2D,int texId
*/
YF void YAC_CALL sdvg_DestroyTexture2D (sUI _texId);

// -------- VBO utility functions --------
/* @function sdvg_BufferAddRectTexUVFlat32,Buffer b,float x,float y,float w,float h,float ul,float vt,float ur,float vb
*/
YF void YAC_CALL sdvg_BufferAddRectTexUVFlat32 (YAC_Buffer *_b, sF32 _x, sF32 _y, sF32 _w, sF32 _h, sF32 _ul, sF32 _vt, sF32 _ur, sF32 _vb);

/* @function sdvg_WriteC32AsRGBA8,Buffer b,int c32
*/
YF void YAC_CALL sdvg_WriteC32AsRGBA8 (YAC_Buffer *_b, sUI _c32);

/* @function sdvg_BufferAddRectTexUVGouraud32,Buffer b,float x,float y,float w,float h,float ul,float vt,float ur,float vb,int c32LT,int c32RT,int c32
*/
YF void YAC_CALL sdvg_BufferAddRectTexUVGouraud32 (YAC_Buffer *_b,  sF32 _x, sF32 _y, sF32 _w, sF32 _h, sF32 _ul, sF32 _vt, sF32 _ur, sF32 _vb, sUI _c32LT, sUI _c32RT, sUI _c32LB, sUI _c32RB);

/* @function sdvg_BufferAddLinePointFlat14_2,Buffer b,float x,float y
*/
YF void YAC_CALL sdvg_BufferAddLinePointFlat14_2 (YAC_Buffer *_b, sF32 _x, sF32 _y);

/* @function sdvg_BufferAddLinePointFlat32,Buffer b,float x,float y
*/
YF void YAC_CALL sdvg_BufferAddLinePointFlat32 (YAC_Buffer *_b, sF32 _x, sF32 _y);

/* @function sdvg_BufferAddLinePointFlatBevel14_2,Buffer b,float x,float y
*/
YF void YAC_CALL sdvg_BufferAddLinePointFlatBevel14_2 (YAC_Buffer *_b, sF32 _x, sF32 _y);

/* @function sdvg_BufferAddLinePointFlatBevel32,Buffer b,float x,float y
*/
YF void YAC_CALL sdvg_BufferAddLinePointFlatBevel32 (YAC_Buffer *_b, sF32 _x, sF32 _y);

/* @function sdvg_BufferAddLinesPointsFlat14_2,Buffer b,float x1,float y1,float x2,float y2
*/
YF void YAC_CALL sdvg_BufferAddLinesPointsFlat14_2 (YAC_Buffer *_b, sF32 _x1, sF32 _y1, sF32 _x2, sF32 _y2);

/* @function sdvg_BufferAddLinesPointsFlat32,Buffer b,float x1,float y1,float x2,float y2
*/
YF void YAC_CALL sdvg_BufferAddLinesPointsFlat32 (YAC_Buffer *_b, sF32 _x1, sF32 _y1, sF32 _x2, sF32 _y2);

// -------- (low level) draw functions --------
/* @function sdvg_DrawTrianglesFillFlatVBO32,int vboId,int byteOffset,int numTris
*/
YF void YAC_CALL sdvg_DrawTrianglesFillFlatVBO32 (sUI _vboId, sUI _byteOffset, sUI _numTris);

/* @function sdvg_DrawTrianglesFillFlatVBO14_2,int vboId,int byteOffset,int numTris
*/
YF void YAC_CALL sdvg_DrawTrianglesFillFlatVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numTris);

/* @function sdvg_DrawTrianglesFillGouraudVBO32,int vboId,int byteOffset,int numTris
*/
YF void YAC_CALL sdvg_DrawTrianglesFillGouraudVBO32 (sUI _vboId, sUI _byteOffset, sUI _numTris);

/* @function sdvg_DrawTrianglesFillGouraudVBO14_2,int vboId,int byteOffset,int numTris
*/
YF void YAC_CALL sdvg_DrawTrianglesFillGouraudVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numTris);

/* @function sdvg_DrawTrianglesFillFlatEdgeAAVBO32,int vboId,int byteOffset,int numTris
*/
YF void YAC_CALL sdvg_DrawTrianglesFillFlatEdgeAAVBO32 (sUI _vboId, sUI _byteOffset, sUI _numTris);

/* @function sdvg_DrawTrianglesFillFlatEdgeAAVBO14_2,int vboId,int byteOffset,int numTris
*/
YF void YAC_CALL sdvg_DrawTrianglesFillFlatEdgeAAVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numTris);

/* @function sdvg_DrawTrianglesFillGouraudEdgeAAVBO32,int vboId,int byteOffset,int numTris
*/
YF void YAC_CALL sdvg_DrawTrianglesFillGouraudEdgeAAVBO32 (sUI _vboId, sUI _byteOffset, sUI _numTris);

/* @function sdvg_DrawTrianglesFillGouraudEdgeAAVBO14_2,int vboId,int byteOffset,int numTris
*/
YF void YAC_CALL sdvg_DrawTrianglesFillGouraudEdgeAAVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numTris);

/* @function sdvg_DrawPolygonFillFlatVBO32,int vboId,int byteOffset,int numVerts
*/
YF void YAC_CALL sdvg_DrawPolygonFillFlatVBO32 (sUI _vboId, sUI _byteOffset, sUI _numVerts);

/* @function sdvg_DrawPolygonFillFlatVBO14_2,int vboId,int byteOffset,int numVerts
*/
YF void YAC_CALL sdvg_DrawPolygonFillFlatVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numVerts);

/* @function sdvg_DrawPolygonFillGouraudVBO32,int vboId,int byteOffset,int numVerts
*/
YF void YAC_CALL sdvg_DrawPolygonFillGouraudVBO32 (sUI _vboId, sUI _byteOffset, sUI _numVerts);

/* @function sdvg_DrawPolygonFillGouraudVBO14_2,int vboId,int byteOffset,int numVerts
*/
YF void YAC_CALL sdvg_DrawPolygonFillGouraudVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numVerts);

/* @function sdvg_SetupRectFillAAVBO32,Buffer vb,Buffer dl,float centerX,float centerY,float sizeX,float sizeY
*/
YF void YAC_CALL sdvg_SetupRectFillAAVBO32 (YAC_Buffer *_vb, YAC_Buffer *_dl, sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY);

/* @function sdvg_DrawRectFillAAVBO32,int vboId,int byteOffsetInner,int numVertsInner,int byteOffsetBorder,int numVertsBorder,int glPrimTypeBorder,float centerX,float centerY,float sizeX,float sizeY
*/
YF void YAC_CALL sdvg_DrawRectFillAAVBO32 (sUI _vboId, sUI _byteOffsetInner, sUI _numVertsInner, sUI _byteOffsetBorder, sUI _numVertsBorder, sUI _glPrimTypeBorder, sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY);

/* @function sdvg_DrawRectFillAA,float centerX,float centerY,float sizeX,float sizeY
*/
YF void YAC_CALL sdvg_DrawRectFillAA (sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY);

/* @function sdvg_SetupRectFillStrokeAAVBO32,Buffer vb,Buffer dl,float centerX,float centerY,float sizeX,float sizeY,float strokeW
*/
YF void YAC_CALL sdvg_SetupRectFillStrokeAAVBO32 (YAC_Buffer *_vb, YAC_Buffer *_dl, sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY, sF32 _strokeW);

/* @function sdvg_DrawRectFillStrokeAAVBO32,int vboId,int byteOffsetInner,int numVertsInner,int byteOffsetBorder,int numVerts
*/
YF void YAC_CALL sdvg_DrawRectFillStrokeAAVBO32 (sUI _vboId, sUI _byteOffsetInner, sUI _numVertsInner, sUI _byteOffsetBorder, sUI _numVertsBorder, sUI _glPrimTypeBorder, sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY);

/* @function sdvg_DrawRectFillStrokeAA,float centerX,float centerY,float sizeX,float sizeY
*/
YF void YAC_CALL sdvg_DrawRectFillStrokeAA (sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY);

/* @function sdvg_SetupRectStrokeAAVBO32,Buffer vb,Buffer dl,float centerX,float centerY,float sizeX,float sizeY,float strokeW
*/
YF void YAC_CALL sdvg_SetupRectStrokeAAVBO32 (YAC_Buffer *_vb, YAC_Buffer *_dl, sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY, sF32 _strokeW);

/* @function sdvg_DrawRectStrokeAAVBO32,int vboId,int byteOffsetBorder,int numVertesBorder,int glPrimTypeBorder,float centerX,float centerY,float sizeX,float sizeY
*/
YF void YAC_CALL sdvg_DrawRectStrokeAAVBO32 (sUI _vboId, sUI _byteOffsetBorder, sUI _numVertsBorder, sUI _glPrimTypeBorder, sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY);

/* @function sdvg_DrawRectStrokeAA,float centerX,float centerY,float sizeX,float sizeY
*/
YF void YAC_CALL sdvg_DrawRectStrokeAA (sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY);

/* @function sdvg_SetupEllipseFillAAVBO32,Buffer vb,Buffer dl,float centerX,float centerY,float radiusX,float radiusY
*/
YF void YAC_CALL sdvg_SetupEllipseFillAAVBO32 (YAC_Buffer *_vb, YAC_Buffer *_dl, sF32 _centerX, sF32 _centerY, sF32 _radiusX, sF32 _radiusY);

/* @function sdvg_DrawEllipseFillAAVBO32,int vboId,int byteOffsetInner,int numVertsInner,int byteOffsetBorder,int numVertsBorder,int glPrimTypeBorder,float centerX,float centerY,float radiusX,float radiusY
*/
YF void YAC_CALL sdvg_DrawEllipseFillAAVBO32 (sUI _vboId, sUI _byteOffsetInner, sUI _numVertsInner, sUI _byteOffsetBorder, sUI _numVertsBorder, sUI _glPrimTypeBorder, sF32 _centerX, sF32 _centerY, sF32 _radiusX, sF32 _radiusY);

/* @function sdvg_DrawEllipseFillAA,float centerX,float centerY,float sizeX,float sizeY
*/
YF void YAC_CALL sdvg_DrawEllipseFillAA (sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY);

/* @function sdvg_SetupEllipseFillStrokeAAVBO32,Buffer vb,Buffer dl,float centerX,float centerY,float radiusX,float radiusY,float strokeW
*/
YF void YAC_CALL sdvg_SetupEllipseFillStrokeAAVBO32 (YAC_Buffer *_vb, YAC_Buffer *_dl, sF32 _centerX, sF32 _centerY, sF32 _radiusX, sF32 _radiusY, sF32 _strokeW);

/* @function sdvg_DrawEllipseFillStrokeAAVBO32,int vboId,int byteOffsetInner,int numVertsInner,int byteOffsetBorder,int numVertsBorder,int glPrimTypeBorder,float centerX,float centerY,float radiusX,float radiusY
*/
YF void YAC_CALL sdvg_DrawEllipseFillStrokeAAVBO32 (sUI _vboId, sUI _byteOffsetInner, sUI _numVertsInner, sUI _byteOffsetBorder, sUI _numVertsBorder, sUI _glPrimTypeBorder, sF32 _centerX, sF32 _centerY, sF32 _radiusX, sF32 _radiusY);

/* @function sdvg_DrawEllipseFillStrokeAA,float centerX,float centerY,float sizeX,float sizeY
*/
YF void YAC_CALL sdvg_DrawEllipseFillStrokeAA (sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY);

/* @function sdvg_SetupEllipseStrokeAAVBO32,Buffer vb,Buffer dl,float centerX,float centerY,float radiusX,float radiusY,float strokeW
*/
YF void YAC_CALL sdvg_SetupEllipseStrokeAAVBO32 (YAC_Buffer *_vb, YAC_Buffer *_dl, sF32 _centerX, sF32 _centerY, sF32 _radiusX, sF32 _radiusY, sF32 _strokeW);

/* @function sdvg_DrawEllipseStrokeAAVBO32,int vboId,int byteOffsetBorder,int numVertsBorder,int glPrimTypeBorder,float centerX,float centerY,float radiusX,float radiusY
*/
YF void YAC_CALL sdvg_DrawEllipseStrokeAAVBO32 (sUI _vboId, sUI _byteOffsetBorder, sUI _numVertsBorder, sUI _glPrimTypeBorder, sF32 _centerX, sF32 _centerY, sF32 _radiusX, sF32 _radiusY);

/* @function sdvg_DrawEllipseStrokeAA,float centerX,float centerY,float sizeX,float sizeY
*/
YF void YAC_CALL sdvg_DrawEllipseStrokeAA (sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY);

/* @function sdvg_SetupRoundRectFillAAVBO32,Buffer vb,Buffer dl,float centerX,float centerY,float sizeX,float sizeY,float radiusX,float radiusY
*/
YF void YAC_CALL sdvg_SetupRoundRectFillAAVBO32 (YAC_Buffer *_vb, YAC_Buffer *_dl, sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY, sF32 _radiusX, sF32 _radiusY);

/* @function sdvg_DrawRoundRectFillAAVBO32,int vboId,int byteOffsetInner,int numVertsInner,int byteOffsetBorder,int numVertsBorder,int glPrimTypeBorder,float centerX,float centerY,float sizeX,float sizeY,float radiusX,float radiusY
*/
YF void YAC_CALL sdvg_DrawRoundRectFillAAVBO32 (sUI _vboId, sUI _byteOffsetInner, sUI _numVertsInner, sUI _byteOffsetBorder, sUI _numVertsBorder, sUI _glPrimTypeBorder, sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY, sF32 _radiusX, sF32 _radiusY);

/* @function sdvg_DrawRoundRectFillAA,float centerX,float centerY,float sizeX,float sizeY,float radiusX,float radiusY
*/
YF void YAC_CALL sdvg_DrawRoundRectFillAA (sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY, sF32 _radiusX, sF32 _radiusY);

/* @function sdvg_SetupRoundRectFillStrokeAAVBO32,Buffer vb,Buffer dl,float centerX,float centerY,float sizeX,float sizeY,float radiusX,float radiusY,float strokeW
*/
YF void YAC_CALL sdvg_SetupRoundRectFillStrokeAAVBO32 (YAC_Buffer *_vb, YAC_Buffer *_dl, sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY, sF32 _radiusX, sF32 _radiusY, sF32 _strokeW);

/* @function sdvg_DrawRoundRectFillStrokeAAVBO32,int vboId,int byteOffsetInner,int numVertsInner,int byteOffsetBorder,int numVertsBorder,int glPrimTypeBorder,float centerX,float centerY,float sizeX,float sizeY,float radiusX,float radiusY
*/
YF void YAC_CALL sdvg_DrawRoundRectFillStrokeAAVBO32 (sUI _vboId, sUI _byteOffsetInner, sUI _numVertsInner, sUI _byteOffsetBorder, sUI _numVertsBorder, sUI _glPrimTypeBorder, sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY, sF32 _radiusX, sF32 _radiusY);

/* @function sdvg_DrawRoundRectFillStrokeAA,float centerX,float centerY,float sizeX,float sizeY,float radiusX,float radiusY
*/
YF void YAC_CALL sdvg_DrawRoundRectFillStrokeAA (sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY, sF32 _radiusX, sF32 _radiusY);

/* @function sdvg_SetupRoundRectStrokeAAVBO32,Buffer vb,Buffer dl,float centerX,float centerY,float sizeX,float sizeY,float radiusX,float radiusY,float strokeW
*/
YF void YAC_CALL sdvg_SetupRoundRectStrokeAAVBO32 (YAC_Buffer *_vb, YAC_Buffer *_dl, sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY, sF32 _radiusX, sF32 _radiusY, sF32 _strokeW);

/* @function sdvg_DrawRoundRectStrokeAAVBO32,int vboId,int byteOffsetBorder,int numVertsBorder,int glPrimTypeBorder,float centerX,float centerY,float sizeX,float sizeY,float radiusX,float radiusY
*/
YF void YAC_CALL sdvg_DrawRoundRectStrokeAAVBO32(sUI _vboId, sUI _byteOffsetBorder, sUI _numVertsBorder, sUI _glPrimTypeBorder, sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY, sF32 _radiusX, sF32 _radiusY);

/* @function sdvg_DrawRoundRectStrokeAA,float centerX,float centerY,float sizeX,float sizeY,float radiusX,float radiusY
*/
YF void YAC_CALL sdvg_DrawRoundRectStrokeAA (sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY, sF32 _radiusX, sF32 _radiusY);

/* @function sdvg_DrawTrianglesTexUVFlatVBO32,int vboId,int byteOffset,int numTris
*/
YF void YAC_CALL sdvg_DrawTrianglesTexUVFlatVBO32 (sUI _vboId, sUI _byteOffset, sUI _numTris);

/* @function sdvg_DrawTrianglesTexUVGouraudVBO32,int vboId,int byteOffset,int numTris
*/
YF void YAC_CALL sdvg_DrawTrianglesTexUVGouraudVBO32 (sUI _vboId, sUI _byteOffset, sUI _numTris);

/* @function sdvg_DrawTrianglesTexUVFlatDecalVBO32,int vboId,int byteOffset,int numTris
*/
YF void YAC_CALL sdvg_DrawTrianglesTexUVFlatDecalVBO32 (sUI _vboId, sUI _byteOffset, sUI _numTris);

/* @function sdvg_DrawTrianglesTexUVGouraudDecalVBO32,int vboId,int byteOffset,int numTris
*/
YF void YAC_CALL sdvg_DrawTrianglesTexUVGouraudDecalVBO32 (sUI _vboId, sUI _byteOffset, sUI _numTris);

/* @function sdvg_DrawLineStripFlatVBO14_2,int vboId,int byteOffset,int numPoints
*/
YF void YAC_CALL sdvg_DrawLineStripFlatVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawLineStripFlatVBO32,int vboId,int byteOffset,int numPoints
*/
YF void YAC_CALL sdvg_DrawLineStripFlatVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawLineStripFlatAAVBO14_2,int vboId,int byteOffset,int numPoints
*/
YF void YAC_CALL sdvg_DrawLineStripFlatAAVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawLineStripFlatAAVBO32,int vboId,int byteOffset,int numPoints
*/
YF void YAC_CALL sdvg_DrawLineStripFlatAAVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawLineStripFlatBevelVBO14_2,int vboId,int byteOffset,int numPoints
*/
YF void YAC_CALL sdvg_DrawLineStripFlatBevelVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawLineStripFlatBevelVBO32,int vboId,int byteOffset,int numPoints
*/
YF void YAC_CALL sdvg_DrawLineStripFlatBevelVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawLineStripFlatBevelAAVBO14_2,int vboId,int byteOffset,int numPoints
*/
YF void YAC_CALL sdvg_DrawLineStripFlatBevelAAVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawLineStripFlatBevelAAVBO32,int vboId,int byteOffset,int numPoints
*/
YF void YAC_CALL sdvg_DrawLineStripFlatBevelAAVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawLinesFlatVBO14_2,int vboId,int byteOffset,int numPoints
*/
YF void YAC_CALL sdvg_DrawLinesFlatVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawLinesFlatVBO32,int vboId,int byteOffset,int numPoints
*/
YF void YAC_CALL sdvg_DrawLinesFlatVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawLinesFlatAAVBO14_2,int vboId,int byteOffset,int numPoints
*/
YF void YAC_CALL sdvg_DrawLinesFlatAAVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawLinesFlatAAVBO32,int vboId,int byteOffset,int numPoints
*/
YF void YAC_CALL sdvg_DrawLinesFlatAAVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawPointsSquareVBO32,int vboId,int byteOffset,int numPoints
*/
YF void YAC_CALL sdvg_DrawPointsSquareVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawPointsSquareAAVBO32,int vboId,int byteOffset,int numPoints
*/
YF void YAC_CALL sdvg_DrawPointsSquareAAVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawPointsRoundVBO32,int vboId,int byteOffset,int numPoints
*/
YF void YAC_CALL sdvg_DrawPointsRoundVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawPointsRoundAAVBO32,int vboId,int byteOffset,int numPoints
*/
YF void YAC_CALL sdvg_DrawPointsRoundAAVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

// ----------- custom shaders and (scratch) draw functions ------------
/* @function sdvg_CreateShader,String vs,String fs:int
@arg vs Vertex shader source
@arg fs Fragment shader source
@return ShaderVG shader index
*/
sUI YAC_CALL sdvg_CreateShader (const char *vs, const char *fs);

/* @function sdvg_DestroyShader,int shaderIdx
*/
YF void YAC_CALL sdvg_DestroyShader (sUI _shaderIdx);

/* @function sdvg_BindShader,int shaderIdx
*/
YF void YAC_CALL sdvg_BindShader (sUI _shaderIdx);

/* @function sdvg_UnbindShader
*/
YF void YAC_CALL sdvg_UnbindShader (void);

/* @function sdvg_GetUniformLocation,String name:int
*/
sSI YAC_CALL sdvg_GetUniformLocation (const char *_name);

/* @function sdvg_GetAttribLocation,String name:int
*/
sSI YAC_CALL sdvg_GetAttribLocation (const char *_name);

/* @function sdvg_Uniform1f,String name,float f
*/
void YAC_CALL sdvg_Uniform1f (const char *_name, sF32 _f);

/* @function sdvg_Uniform2f,String name,float f1,float f2
*/
void YAC_CALL sdvg_Uniform2f (const char *_name, sF32 _f1, sF32 _f2);

/* @function sdvg_Uniform3f,String name,float f1,float f2,float f3
*/
void YAC_CALL sdvg_Uniform3f (const char *_name, sF32 _f1, sF32 _f2, sF32 _f3);

/* @function sdvg_Uniform4f,String name,float f1,float f2,float f3,float f4
*/
void YAC_CALL sdvg_Uniform4f (const char *_name, sF32 _f1, sF32 _f2, sF32 _f3, sF32 _f4);

/* @function sdvg_Uniform1i,String name,int i
*/
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
/* @function sdvg_BeginVBO,int numVertices,int stride:boolean
@arg numVertices Number of vertices
@arg stride Total number of attribute bytes per vertex
*/
YF sBool YAC_CALL sdvg_BeginVBO (sUI _numVertices, sUI _stride);

/* @function sdvg_BeginTriangles,int numVertices,int stride:boolean
@arg numVertices Number of vertices
@arg stride Total number of attribute bytes per vertex
*/
YF sBool YAC_CALL sdvg_BeginTriangles (sUI _numVertices, sUI _stride);

/* @function sdvg_BeginTriangleFan,int numVertices,int stride:boolean
@arg numVertices Number of vertices
@arg stride Total number of attribute bytes per vertex
*/
YF sBool YAC_CALL sdvg_BeginTriangleFan (sUI _numVertices, sUI _stride);

/* @function sdvg_BeginTriangleStrip,int numVertices,int stride:boolean
@arg numVertices Number of vertices
@arg stride Total number of attribute bytes per vertex
*/
YF sBool YAC_CALL sdvg_BeginTriangleStrip (sUI _numVertices, sUI _stride);

/* @function sdvg_BeginFilledTriangles,int numVertices:boolean
@arg numVertices Number of vertices
*/
YF sBool YAC_CALL sdvg_BeginFilledTriangles (sUI _numVertices);

/* @function sdvg_BeginFilledTriangleFan,int numVertices:boolean
@arg numVertices Number of vertices
*/
YF sBool YAC_CALL sdvg_BeginFilledTriangleFan (sUI _numVertices);

/* @function sdvg_BeginFilledTriangleStrip,int numVertices:boolean
@arg numVertices Number of vertices
*/
YF sBool YAC_CALL sdvg_BeginFilledTriangleStrip (sUI _numVertices);

/* @function sdvg_BeginFilledGouraudTriangles,int numVertices:boolean
@arg numVertices Number of vertices
*/
YF sBool YAC_CALL sdvg_BeginFilledGouraudTriangles (sUI _numVertices);

/* @function sdvg_BeginFilledGouraudTriangleFan,int numVertices:boolean
@arg numVertices Number of vertices
*/
YF sBool YAC_CALL sdvg_BeginFilledGouraudTriangleFan (sUI _numVertices);

/* @function sdvg_BeginFilledGouraudTriangleStrip,int numVertices:boolean
@arg numVertices Number of vertices
*/
YF sBool YAC_CALL sdvg_BeginFilledGouraudTriangleStrip (sUI _numVertices);

/* @function sdvg_BeginTexturedTriangles,int numVertices:boolean
@arg numVertices Number of vertices
*/
YF sBool YAC_CALL sdvg_BeginTexturedTriangles (sUI _numVertices);

/* @function sdvg_BeginTexturedTriangleFan,int numVertices:boolean
@arg numVertices Number of vertices
*/
YF sBool YAC_CALL sdvg_BeginTexturedTriangleFan (sUI _numVertices);

/* @function sdvg_BeginTexturedTriangleStrip,int numVertices:boolean
@arg numVertices Number of vertices
*/
YF sBool YAC_CALL sdvg_BeginTexturedTriangleStrip (sUI _numVertices);

/* @function sdvg_BeginTexturedGouraudTriangles,int numVertices:boolean
@arg numVertices Number of vertices
*/
YF sBool YAC_CALL sdvg_BeginTexturedGouraudTriangles (sUI _numVertices);

/* @function sdvg_BeginTexturedGouraudTriangleFan,int numVertices:boolean
@arg numVertices Number of vertices
*/
YF sBool YAC_CALL sdvg_BeginTexturedGouraudTriangleFan (sUI _numVertices);

/* @function sdvg_BeginTexturedGouraudTriangleStrip,int numVertices:boolean
@arg numVertices Number of vertices
*/
YF sBool YAC_CALL sdvg_BeginTexturedGouraudTriangleStrip (sUI _numVertices);

/* @function sdvg_BeginTexturedTrianglesAlpha,int numVertices:boolean
@arg numVertices Number of vertices
*/
YF sBool YAC_CALL sdvg_BeginTexturedTrianglesAlpha (sUI _numVertices);

/* @function sdvg_BeginTexturedTriangleFanAlpha,int numVertices:boolean
@arg numVertices Number of vertices
*/
YF sBool YAC_CALL sdvg_BeginTexturedTriangleFanAlpha (sUI _numVertices);

/* @function sdvg_BeginTexturedTriangleStripAlpha,int numVertices:boolean
@arg numVertices Number of vertices
*/
YF sBool YAC_CALL sdvg_BeginTexturedTriangleStripAlpha (sUI _numVertices);

/* @function sdvg_BeginTexturedGouraudTrianglesAlpha,int numVertices:boolean
@arg numVertices Number of vertices
*/
YF sBool YAC_CALL sdvg_BeginTexturedGouraudTrianglesAlpha (sUI _numVertices);

/* @function sdvg_BeginTexturedGouraudTriangleFanAlpha,int numVertices:boolean
@arg numVertices Number of vertices
*/
YF sBool YAC_CALL sdvg_BeginTexturedGouraudTriangleFanAlpha (sUI _numVertices);

/* @function sdvg_BeginTexturedGouraudTriangleStripAlpha,int numVertices:boolean
@arg numVertices Number of vertices
*/
YF sBool YAC_CALL sdvg_BeginTexturedGouraudTriangleStripAlpha (sUI _numVertices);

/* @function sdvg_BeginLineStrip,int numPoints:boolean
@arg numPoints Number of vertices
*/
YF sBool YAC_CALL sdvg_BeginLineStrip (sUI _numPoints);

/* @function sdvg_BeginLineStripAA,int numPoints:boolean
@arg numPoints Number of points
*/
YF sBool YAC_CALL sdvg_BeginLineStripAA (sUI _numPoints);

/* @function sdvg_BeginLineStripBevel,int numPoints:boolean
@arg numPoints Number of points
*/
YF sBool YAC_CALL sdvg_BeginLineStripBevel (sUI _numPoints);

/* @function sdvg_BeginLineStripBevelAA,int numPoints:boolean
@arg numPoints Number of points
*/
YF sBool YAC_CALL sdvg_BeginLineStripBevelAA (sUI _numPoints);

/* @function sdvg_BeginLines,int numPoints:boolean
@arg numPoints Number of points
*/
YF sBool YAC_CALL sdvg_BeginLines (sUI _numPoints);

/* @function sdvg_BeginLinesAA,int numPoints:boolean
@arg numPoints Number of points
*/
YF sBool YAC_CALL sdvg_BeginLinesAA (sUI _numPoints);

/* @function sdvg_BeginPointsSquare,int numPoints:boolean
@arg numPoints Number of points
*/
YF sBool YAC_CALL sdvg_BeginPointsSquare (sUI _numPoints);

/* @function sdvg_BeginPointsSquareAA,int numPoints:boolean
@arg numPoints Number of points
*/
YF sBool YAC_CALL sdvg_BeginPointsSquareAA (sUI _numPoints);

/* @function sdvg_BeginPointsRound,int numPoints:boolean
@arg numPoints Number of points
*/
YF sBool YAC_CALL sdvg_BeginPointsRound (sUI _numPoints);

/* @function sdvg_BeginPointsRoundAA,int numPoints:boolean
@arg numPoints Number of points
*/
YF sBool YAC_CALL sdvg_BeginPointsRoundAA (sUI _numPoints);

/* @function sdvg_VertexOffset2f
*/
YF void YAC_CALL sdvg_VertexOffset2f (void);

/* @function sdvg_AttribOffsetf,String name,int size
*/
void YAC_CALL sdvg_AttribOffsetf (const char *_name, sUI _size);

/* @function sdvg_AttribOffset1f,String name
*/
void YAC_CALL sdvg_AttribOffset1f (const char *_name);

/* @function sdvg_AttribOffset2f,String name
*/
void YAC_CALL sdvg_AttribOffset2f (const char *_name);

/* @function sdvg_AttribOffset3f,String name
*/
void YAC_CALL sdvg_AttribOffset3f (const char *_name);

/* @function sdvg_AttribOffset4f,String name
*/
void YAC_CALL sdvg_AttribOffset4f (const char *_name);

/* @function sdvg_AttribOffsetARGB,String name
*/
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
/* @function sdvg_Attribi16,short i
*/
void YAC_CALL sdvg_Attribi16 (sS16 _i);

/* @function sdvg_Attrib1f,float f
*/
YF void YAC_CALL sdvg_Attrib1f (sF32 _f);

/* @function sdvg_Attrib2f,float f1,float f2
*/
YF void YAC_CALL sdvg_Attrib2f (sF32 _f1, sF32 _f2);

/* @function sdvg_Vertex2f,float x,float y
*/
YF void YAC_CALL sdvg_Vertex2f (sF32 _x, sF32 _y);

/* @function sdvg_Vertex3f,float x,float y,float z
*/
YF void YAC_CALL sdvg_Vertex3f (sF32 _x, sF32 _y, sF32 _z);

/* @function sdvg_TexCoord2f,float u,float v
*/
YF void YAC_CALL sdvg_TexCoord2f (sF32 _u, sF32 _v);

/* @function sdvg_Attrib3f,float f1,float f2,float f3
*/
YF void YAC_CALL sdvg_Attrib3f (sF32 _f1, sF32 _f2, sF32 _f3);

/* @function sdvg_Color3f,float r,float g,float b
*/
YF void YAC_CALL sdvg_Color3f (sF32 _r, sF32 _g, sF32 _b);

/* @function sdvg_Attrib4f,float f1,float f2,float f3,float f4
*/
YF void YAC_CALL sdvg_Attrib4f (sF32 _f1, sF32 _f2, sF32 _f3, sF32 _f4);

/* @function sdvg_Color4f,float r,float g,float b,float a
*/
YF void YAC_CALL sdvg_Color4f (sF32 _r, sF32 _g, sF32 _b, sF32 _a);

/* @function sdvg_AttribARGB,int c32
*/
YF void YAC_CALL sdvg_AttribARGB (sUI _c32);

/* @function sdvg_ColorARGB,int c32
*/
YF void YAC_CALL sdvg_ColorARGB (sUI _c32);

/* @function sdvg_End
Finalize vertex buffer and start rendering (unless buffer is currently mapped).
*/
YF void YAC_CALL sdvg_End (void);

// ----------- additional (scratch) draw functions ------------
/* @function sdvg_DrawFilledRectangle,float x,float y,float w,float h
*/
YF void YAC_CALL sdvg_DrawFilledRectangle (sF32 _x, sF32 _y, sF32 _w, sF32 _h);

/* @function sdvg_DrawRectangle,float x,float y,float w,float h,float b
*/
YF void YAC_CALL sdvg_DrawRectangle (sF32 _x, sF32 _y, sF32 _w, sF32 _h, sF32 _b);

// ----------- color utilities ------------
/* @function sdvg_ARGB,byte a,byte r,byte g,byte b:int
*/
sU32 sdvg_ARGB (sU8 _a, sU8 _r, sU8 _g, sU8 _b);

/* @function sdvg_ARGBf,float a,float r,float g,float b:int
*/
YF sU32 YAC_CALL sdvg_ARGBf (sF32 _a, sF32 _r, sF32 _g, sF32 _b);

/* @function sdvg_MixARGBx,int x,int y,float t:int
*/
sU32 sdvg_MixARGBx (sU32 _x, sU32 _y, sU16 _t);

/* @function sdvg_MixARGBf,int x,int y,float t:int
*/
YF sU32 YAC_CALL sdvg_MixARGBf (sU32 _x, sU32 _y, sF32 _t);

/* @function sdvg_MixRGBx,int x,int y,short t
@arg x ARGB32 color
@arg y ARGB32 color
@arg t 0..256
*/
sU32 sdvg_MixRGBx (sU32 _x, sU32 _y, sU16 _t);

/* @function sdvg_MixRGBf,int x,int y,float t:int
*/
YF sU32 YAC_CALL sdvg_MixRGBf (sU32 _x, sU32 _y, sF32 _t);

/* @function sdvg_TintARGB,int x,int y:int
*/
YF sU32 YAC_CALL sdvg_TintARGB (sU32 _x, sU32 _y);

/* @function sdvg_TintRGBAlpha,int x,int y,byte a8:int
*/
sU32 sdvg_TintRGBAlpha (sU32 _x, sU32 _y, sU8 _a8);

/* @function sdvg_RGBAlpha,int c32,byte a8:int
*/
sU32 sdvg_RGBAlpha (sU32 _c32, sU8 _a8);

/* @function sdvg_HSVAToARGB,float h,float s,float v,byte a8:int
*/
sU32 sdvg_HSVAToARGB (sF32 _h, sF32 _s, sF32 _v, sU8 _a8);

/* @function sdvg_ARGBToHSVA,int c32,Float retH,Float retS,Float retV:byte
*/
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
