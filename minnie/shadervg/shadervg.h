// ----
// ---- file   : shadervg.h
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
// ---- info   : ShaderVG render utilities
// ----
// ----
// ----

#ifndef SHADERVG_H__
#define SHADERVG_H__

YG("shadervg");


#define Dsdvg_buffer_ref_t  YAC_Buffer *
#ifdef MINNIE_LIB
#define Dsdvg_mat4_ref_t  Matrix4f *
#else
#define Dsdvg_mat4_ref_t  YAC_Object *
#endif // MINNIE_LIB

// texture formats
#ifdef SHADERVG_SCRIPT_API
/* @class ShaderVG_Constants
 */

YC class __sdvg_constants_class : public YAC_Object {
  public:
   __sdvg_constants_class(void) {}
   ~__sdvg_constants_class() {}
   YAC(__sdvg_constants_class);
#endif // SHADERVG_SCRIPT_API

/* @constant SDVG_TEXFMT_ALPHA8
8bit alpha texture format
 */
#define SDVG_TEXFMT_ALPHA8    YCI 1

/* @constant SDVG_TEXFMT_RGB565
16bit RGB565 texture format
 */
#define SDVG_TEXFMT_RGB565    YCI 2
#define SDVG_TEXFMT_resvd_3   YCI 3

/* @constant SDVG_TEXFMT_BGRA8888
32bit B,G,R,A texture format (packed ARGB32 on little endian)
 */
#define SDVG_TEXFMT_BGRA8888  YCI 4  // b,g,r,a bytes

/* @constant SDVG_TEXFMT_ARGB32
alias for %SDVG_TEXFMT_BGRA8888 (little endian)
 */
#define SDVG_TEXFMT_ARGB32    YCI 4  // alias for BGRA8888 (little endian)

/* @constant SDVG_TEXFMT_RGBA8888
32bit R,G,B,A texture format (packed ABGR32 on little endian)
 */
#define SDVG_TEXFMT_RGBA8888  YCI 5  // r,g,b,a bytes

#ifdef SHADERVG_SCRIPT_API
};
#endif // SHADERVG_SCRIPT_API

#ifndef SHADERVG_SKIP_DECLARATIONS

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// -------- init/exit --------
/* @function sdvg_SetGLSLVersion,boolean bV3,boolean bGLES,String sVersionStringOrNull
Set GLSL version.

@arg bV3 true=Desktop GL 3.x / 4.x or GLES3.x
@arg bGLES true=OpenGL ES  false=Desktop GL
@arg sVersionStringOrNull Custom version string override or null (== auto-select)
*/
YF void YAC_CALL sdvg_SetGLSLVersion (sBool _bV3, sBool _bGLES, YAC_String *_sVersionStringOrNull);

/* @function sdvg_SetScratchBufferSize,int szBytes
Set vertex attrib ring buffer size
*/
YF void YAC_CALL sdvg_SetScratchBufferSize (sUI _szBytes);

/* @function sdvg_Init,boolean bGLCore:boolean
Initialize ShaderVG library

@arg bGLCore true=use Desktop GL 4.x core profile   false=use compatibility profile

@group Init
*/
YF sBool YAC_CALL sdvg_Init (sBool _bGLCore);

/* @function sdvg_Exit
Shutdown ShaderVG library

@group Init
*/
YF void YAC_CALL sdvg_Exit (void);

/* @function sdvg_OnOpen:boolean
Must be called after GL context has been lost, e.g. after the desktop window has been resized or opened.

@group Init
*/
YF sBool YAC_CALL sdvg_OnOpen (void);

/* @function sdvg_SetFramebufferSize,int w,int h
Set framebuffer size

@groupref FBO
*/
YF void YAC_CALL sdvg_SetFramebufferSize (sUI _w, sUI _h);

// -------- begin/end frame --------
/* @function sdvg_BeginFrame
Must be called at the beginning of a new frame

@see sdvg_EndFrame
*/
YF void YAC_CALL sdvg_BeginFrame (void);

/* @function sdvg_ReturnToGL
Should be called before issueing OpenGL calls (unmaps / unbinds current VBO, unbind shader)
*/
YF void YAC_CALL sdvg_ReturnToGL (void);

/* @function sdvg_EndFrame
Should be called at the end of a frame (unmap / unbind current VBO, unbind shader, disable blending and scissor test)

@see sdvg_BeginFrame
*/
YF void YAC_CALL sdvg_EndFrame (void);

// -------- viewport --------
/* @function sdvg_SetViewport,int x,int y,int w,int h
Set viewport.

@arg x Left viewport coordinate
@arg y Top viewport coordinate
@arg w Viewport width
@arg h Viewport height

@group Viewport
*/
YF void YAC_CALL sdvg_SetViewport (sUI _x, sUI _y, sUI _w, sUI _h);

/* @function sdvg_PushViewport,int x,int y,int w,int h
Push current viewport onto viewport stack and set new viewport.

@group Viewport
*/
YF void YAC_CALL sdvg_PushViewport (sUI _x, sUI _y, sUI _w, sUI _h);

/* @function sdvg_PopViewport
Pop viewport from viewport stack.

@group Viewport
*/
YF void YAC_CALL sdvg_PopViewport (void);

/* @function sdvg_GetViewportWidth:int
Query current viewport width

@group Viewport
*/
YF sUI YAC_CALL sdvg_GetViewportWidth (void);

/* @function sdvg_GetViewportHeight:int
Query current viewport height

@group Viewport
*/
YF sUI YAC_CALL sdvg_GetViewportHeight (void);

// -------- scissor --------
/* @function sdvg_SetScissor,int x,int y,int w,int h
Set current scissor clipping rectangle

@arg x Left scissor position
@arg y Top scissor position
@arg w Scissor width
@arg h Scissor height

@group Scissor
*/
YF void YAC_CALL sdvg_SetScissor (sUI _x, sUI _y, sUI _w, sUI _h);

/* @function sdvg_PushScissor,int x,int y,int w,int h
Push current scissor clipping rectangle onto scissor stack and set new scissor rectangle.

@arg x Left scissor position
@arg y Top scissor position
@arg w Scissor width
@arg h Scissor height

@group Scissor
*/
YF void YAC_CALL sdvg_PushScissor (sUI _x, sUI _y, sUI _w, sUI _h);

/* @function sdvg_PopScissor
Pop scissor rectangle from scissor stack

@group Scissor
*/
YF void YAC_CALL sdvg_PopScissor (void);

/* @function sdvg_EnableScissor
Enable scissor test (clipping rectangle)

@group Scissor
*/
YF void YAC_CALL sdvg_EnableScissor (void);

/* @function sdvg_DisableScissor
Disable scissor test (clipping rectangle)

@group Scissor
*/
YF void YAC_CALL sdvg_DisableScissor (void);

/* @function sdvg_Flush
Flush pending GL state changes and draw calls to GPU
*/
YF void YAC_CALL sdvg_Flush (void);

// -------- transform --------
/* @function sdvg_SetTransform,Matrix4f mat4
Set row-major modelview-projection transformation matrix

@group Transform
*/
#ifdef MINNIE_LIB
void sdvg_SetTransform (Matrix4f *_mat4);
#endif // MINNIE_LIB
#ifdef SHADERVG_SCRIPT_API
YF void YAC_CALL _sdvg_SetTransform (YAC_Object *_mat4);
#endif // SHADERVG_SCRIPT_API

/* @function sdvg_GetTransformRef:Matrix4f
Get reference to row-major modelview-projection matrix.

@group Transform
*/
#ifdef MINNIE_LIB
Matrix4f *sdvg_GetTransformRef (void);
#endif // MINNIE_LIB
#ifdef SHADERVG_SCRIPT_API
YF YAC_Object *YAC_CALL _sdvg_GetTransformRef (void);
#endif // SHADERVG_SCRIPT_API


/* @function sdvg_GetProjTransformRef:Matrix4f
Get reference to row-major projection matrix

Applications must call %sdvg_UpdateTransform after the matrix is changed.

@group Transform
*/
#ifdef SHADERVG_SCRIPT_API
YF YAC_Object *YAC_CALL _sdvg_GetProjTransformRef (void);
#else
Matrix4f *sdvg_GetProjTransformRef (void);
#endif // SHADERVG_SCRIPT_API

/* @function sdvg_GetModelTransformRef:Matrix4f
Get reference to row-major model matrix

Applications must call %sdvg_UpdateTransform after the matrix is changed.

@group Transform
*/
#ifdef SHADERVG_SCRIPT_API
YF YAC_Object *YAC_CALL _sdvg_GetModelTransformRef (void);
#else
Matrix4f *sdvg_GetModelTransformRef (void);
#endif // SHADERVG_SCRIPT_API

/* @function sdvg_UpdateTransform
Recalculate combined modelview-projection transform matrix.

This is usually done automatically when modifying the projection / model matrices via the ShaderVG API functions.

However, when an application obtains references to the matrices and modifies them directly, it must call this function afterwards.
*/
YF void YAC_CALL sdvg_UpdateTransform (void);

#ifdef SHADERVG_MATRIX_STACK
/* @function sdvg_PushProjMatrix
Push projection matrix onto projection matrix stack

@group Transform
*/
YF void YAC_CALL sdvg_PushProjMatrix (void);

/* @function sdvg_PopProjMatrix
Pop projection matrix from top of projection matrix stack

@group Transform
*/
YF void YAC_CALL sdvg_PopProjMatrix (void);

/* @function sdvg_PushModelMatrix
Push model matrix onto model matrix stack

@group Transform
*/
YF void YAC_CALL sdvg_PushModelMatrix (void);

/* @function sdvg_PopModelMatrix
Pop model matrix from top of model matrix stack

@group Transform
*/
YF void YAC_CALL sdvg_PopModelMatrix (void);

/* @function sdvg_ProjInitIdentity
Load identity projection matrix

@group Transform
*/
YF void YAC_CALL sdvg_ProjInitIdentity (void);

/* @function sdvg_ProjInit2D,float w,float h
Load 2D projection matrix (origin = left / top, positive y = down)

@arg w Right clipping plane
@arg h Bottom clipping plane

@group Transform
*/
YF void YAC_CALL sdvg_ProjInit2D (sF32 _w, sF32 _h);

/* @function sdvg_ProjInitOrtho,float sx,float sy
Load orthogonal projection matrix (positive y = up)

@arg sx Left / Right clipping planes
@arg sy Bottom / Top clipping planes

@group Transform
*/
YF void YAC_CALL sdvg_ProjInitOrtho (sF32 _sx, sF32 _sy);

/* @function sdvg_ProjTranslate2f,float tx,float ty
Translate projection matrix

@arg tx X translation
@arg ty Y translation

@group Transform
*/
YF void YAC_CALL sdvg_ProjTranslate2f (sF32 _tx, sF32 _ty);

/* @function sdvg_ModelInitIdentity
Load identity model matrix

@group Transform
*/
YF void YAC_CALL sdvg_ModelInitIdentity (void);

/* @function sdvg_ModelTranslate2f,float tx,float ty
Translate model matrix

@arg tx X translation
@arg ty Y translation

@group Transform
*/
YF void YAC_CALL sdvg_ModelTranslate2f (sF32 _tx, sF32 _ty);

/* @function sdvg_ModelTranslate3f,float tx,float ty,float tz
Translate model matrix (3D)

@arg tx X translation
@arg ty Y translation
@arg tz Z translation

@group Transform
*/
YF void YAC_CALL sdvg_ModelTranslate3f (sF32 _tx, sF32 _ty, sF32 _tz);

/* @function sdvg_ModelScale2f,float sx,float sy
Scale model matrix

@arg sx X scale factor
@arg sy Y scale factor

@group Transform
*/
YF void YAC_CALL sdvg_ModelScale2f (sF32 _sx, sF32 _sy);

/* @function sdvg_ModelRotatef,float rad
Rotate model matrix about z axis

@arg rad Rotation angle (radian measure). Positive values rotate counter-clockwise, and negative values rotate clockwise (like in OpenGL).

@group Transform
*/
YF void YAC_CALL sdvg_ModelRotatef (sF32 _rad);
#endif // SHADERVG_MATRIX_STACK_SIZE

// -------- AA --------
/* @function sdvg_SetEnableAA,boolean bEnable
Enable or disable (analytical) anti-aliasing

@group AA
*/
YF void YAC_CALL sdvg_SetEnableAA (sBool _bEnable);

/* @function sdvg_SetAARange,float aaRange
Change (analytical) anti-aliasing range.

It is not recommended to change the default setting.

@arg aaRange AA range (default=1.5)

@group AA
*/
YF void YAC_CALL sdvg_SetAARange (sF32 _aaRange);

/* @function sdvg_SetAAExp,float aaExp
Set (analytical) anti-aliasing exponent.

Deprecated, will be removed in future versions.

@arg aaExp AA exponent (default=1.0)

@group AA
*/
YF void YAC_CALL sdvg_SetAAExp (sF32 _aaExp);

/* @function sdvg_SetAlphaSDFRange,float aMin,float aMax
Set alpha range of distance field textures.

This controls the bluriness of e.g. anti-aliased glyph edges during text rendering.

@arg aMin Alpha range start (0..1, e.g. 0.8)
@arg aMax Alpha range end (0..1, e.g. 0.95)

@group SDF
*/
YF void YAC_CALL sdvg_SetAlphaSDFRange (sF32 _aMin, sF32 _aMax);

/* @function sdvg_SetAlphaSDFExp,float aExp
Set exponent of output alpha

@arg aExp Alpha exponent (gamma correction) (def=0.7)

@group SDF
*/
YF void YAC_CALL sdvg_SetAlphaSDFExp (sF32 _aExp);

// -------- render state (fill / stroke) --------
/* @function sdvg_SetFillColor4f,float fillR,float fillG,float fillB,float A
Set fill color (normalized floats)
@arg fillR Red (0..1)
@arg fillG Green (0..1)
@arg fillB Blue (0..1)
@arg fillA Alpha (0..1)

@group Color
*/
YF void YAC_CALL sdvg_SetFillColor4f (sF32 _fillR, sF32 _fillG, sF32 _fillB, sF32 _fillA);

/* @function sdvg_SetFillAlpha,float fillA
Set fill alpha (normalized float)
@arg fillA Alpha (0..1)

@group Fill
@group Color
*/
YF void YAC_CALL sdvg_SetFillAlpha (sF32 _fillA);

/* @function sdvg_SetFillColorARGB,int c32
Set fill color

@arg c32 packed ARGB32 color

@group Fill
@group Color
*/
YF void YAC_CALL sdvg_SetFillColorARGB (sUI _c32);

/* @function sdvg_SetColor4f,float r,float g,float b,float a
Set fill and stroke colors

@arg r Red (0..1)
@arg g Green (0..1)
@arg b Blue (0..1)
@arg a Alpha (0..1)

@group Fill
@group Stroke
@group Color
*/
YF void YAC_CALL sdvg_SetColor4f (sF32 _r, sF32 _g, sF32 _b, sF32 _a);

/* @function sdvg_SetColorARGB,int c32
Set fill and stroke colors from packed ARGB32 integer

@arg c32 packed ARGB32 color

@group Fill
@group Stroke
@group Color
*/
YF void YAC_CALL sdvg_SetColorARGB (sUI _c32);

/* @function sdvg_SetStrokeRadius,float strokeRadius
Set line stroke radius.

@arg strokeRadius Stroke radius

The total line stroke width is (2 * strokeRadius)

@group Stroke
*/
YF void YAC_CALL sdvg_SetStrokeRadius (sF32 _strokeRadius);

/* @function sdvg_SetStrokeWidth,float lineW
Set line stroke width

@arg lineW Line stroke width

The total line width is lineW and the line stroke radius is (0.5 * lineW).

@group Stroke
*/
YF void YAC_CALL sdvg_SetStrokeWidth (sF32 _lineW);

/* @function sdvg_SetStrokeRadiusAAOffset,float offset
Set AA line stroke radius offset.

Intended to be used to globally finetune the width of anti-aliased lines (i.e. set once during init).

@arg offset AA stroke radius offset (default=0.75. 1.5 better aligns with non-AA lines but does not allow for very thin lines)

The total line stroke width is (2 * (strokeRadius+offset))

@group Stroke
*/
YF void YAC_CALL sdvg_SetStrokeRadiusAAOffset (sF32 _offset);

/* @function sdvg_SetStrokeScale,float scale
Set AA line stroke width scaling factor.

@arg scale AA line stroke scaling factor (default=1.0)

The total line stroke width is (2 * (strokeRadius*strokeScale+strokeOffset))

@group Stroke
*/
YF void YAC_CALL sdvg_SetStrokeScale (sF32 _scale);

/* @function sdvg_SetLinePatternScale,float scale
Set line pattern scaling factor.

This maps pixel distances to normalized (0..1) texture coordinates.

@arg scale Pattern scaling factor (default = 1/256, suitable for pattern texture width = 256)

@group LinePattern
*/
YF void YAC_CALL sdvg_SetLinePatternScale (sF32 _scale);

/* @function sdvg_SetLinePatternOffset,float offset
Set line pattern start offset.

The offset is applied after the line pattern scaling factor.

@arg offset Normalized pattern offset (range 0..1) (default = 0)

@group LinePattern
*/
YF void YAC_CALL sdvg_SetLinePatternOffset (sF32 _offset);

/* @function sdvg_SetMiterLimit,float miterLimit
Set line miter limit

@arg miterLimit Line Miter limit (at which line joint reverts to bevel) (def=32)

@group Stroke
*/
YF void YAC_CALL sdvg_SetLineMiterLimit (sF32 _miterLimit);

/* @function sdvg_SetPointRadius,float radius
Set point radius

@arg radius Point radius

The total point size is (2 * radius).

@group Point
*/
YF void YAC_CALL sdvg_SetPointRadius (sF32 _radius);

/* @function sdvg_SetPointSize,float size
Set point size (glPointSize() compatibility)

@arg size Point size

The total point size is (size) and the point radius is (0.5 * size).

@group Point
*/
YF void YAC_CALL sdvg_SetPointSize (sF32 _size);

/* @function sdvg_SetPixelScaling,float s
Set pixel scaling for analytical anti-aliasing draw calls.

This is only required in rare corner cases, i.e. when using non-pixel-based coordinate systems.
*/
YF void YAC_CALL sdvg_SetPixelScaling (sF32 _s);

/* @function sdvg_SetStrokeColor4f,float strokeR,float strokeG,float strokeB, float strokeA
Set stroke color (normalized floats)

@arg strokeR Red (0..1)
@arg strokeG Green(0..1)
@arg strokeB Blue (0..1)
@arg strokeA Alpha (0..1)

@group Stroke
@group Color
@group Decal
*/
YF void YAC_CALL sdvg_SetStrokeColor4f (sF32 _strokeR, sF32 _strokeG, sF32 _strokeB, sF32 _strokeA);

/* @function sdvg_SetStrokeColorARGB,int c32
Set stroke color

@arg c32 Packed ARGB32 color

@group Stroke
@group Color
@group Decal
*/
YF void YAC_CALL sdvg_SetStrokeColorARGB (sU32 _c32);

/* @function sdvg_SetGlobalAlpha,float a
Set global alpha. Applied to all draw calls.

@group Color
@group Fill
@group Stroke
*/
YF void YAC_CALL sdvg_SetGlobalAlpha (sF32 _a);

/* @function sdvg_SetTextureDecalAlpha,float decalAlpha
Set texture opacity in Decal draw calls

@arg decalAlpha normalized opacity (0..1)

@group Texture
@group Decal
*/
YF void YAC_CALL sdvg_SetTextureDecalAlpha (sF32 _decalAlpha);

// -------- clear --------
/* @function sdvg_Clear4f,float r,float g,float b,float a
Clear screen (scissor clipping rectangle)

@arg r Normalized red value
@arg g Normalized green value
@arg b Normalized blue value
@arg a Normalized alpha value

@group Clear
*/
YF void YAC_CALL sdvg_Clear4f (sF32 _r, sF32 _g, sF32 _b, sF32 _a);

/* @function sdvg_ClearARGB,int c32
Clear screen (scissor clipping rectangle)

@arg c32 Packed ARGB32 color

@group Clear
*/
YF void YAC_CALL sdvg_ClearARGB (sUI _c32);

// -------- blending --------
/* @function sdvg_EnableBlending
Enable source-over blending

@group Blend
*/
YF void YAC_CALL sdvg_EnableBlending (void);

/* @function sdvg_EnableBlendingKeepAlpha
Enable source-over blending (do not modify destination alpha)

@group Blend
*/
YF void YAC_CALL sdvg_EnableBlendingKeepAlpha (void);

/* @function sdvg_EnableBlendingReplaceAlpha
Enable source-over blending (replace destination alpha by source alpha)

@group Blend
*/
YF void YAC_CALL sdvg_EnableBlendingReplaceAlpha (void);

/* @function sdvg_EnableBlendingPremultiplied
Enable premultiplied source-over blending

@group Blend
*/
YF void YAC_CALL sdvg_EnableBlendingPremultiplied (void);

/* @function sdvg_EnableBlendingAdditive
Enable additive blending

@group Blend
*/
YF void YAC_CALL sdvg_EnableBlendingAdditive (void);

/* @function sdvg_EnableBlendingAdditiveKeepAlpha
Enable additive blending (do not modify destination alpha)

@group Blend
*/
YF void YAC_CALL sdvg_EnableBlendingAdditiveKeepAlpha (void);

/* @function sdvg_EnableBlendingAdditiveReplaceAlpha
Enable additive blending (replace destination alpha by source alpha)

@group Blend
*/
YF void YAC_CALL sdvg_EnableBlendingAdditiveReplaceAlpha (void);

/* @function sdvg_EnableBlendingSrcColorKeepAlpha

@group Blend
*/
YF void YAC_CALL sdvg_EnableBlendingSrcColorKeepAlpha (void);

/* @function sdvg_EnableBlendingDstColorKeepAlpha

@group Blend
*/
YF void YAC_CALL sdvg_EnableBlendingDstColorKeepAlpha (void);

/* @function sdvg_DisableBlending
Disable blending

@group Blend
*/
YF void YAC_CALL sdvg_DisableBlending (void);

/* @function sdvg_AlphaWrite,boolean bEnable
Enable or disable alpha channel writes

@arg bEnable Alpha channel write state
*/
YF void YAC_CALL sdvg_AlphaWrite (sBool _bEnable);

// -------- configuration / debug --------
/* @function sdvg_GetEnableDebug:boolean
Query debug mode-enable

@group Debug
*/
YF sBool YAC_CALL sdvg_GetEnableDebug (void);

/* @function sdvg_SetEnableDebug,boolean bEnable
Enable or disable debug mode

@group Debug
*/
YF void YAC_CALL sdvg_SetEnableDebug (sBool _bEnable);

/* @function sdvg_GetEnableDrawInner:boolean
Query draw-inner mode

@group Debug
*/
YF sBool YAC_CALL sdvg_GetEnableDrawInner (void);

/* @function sdvg_SetEnableDrawInner,boolean bEnable
Enable or disable(debug) draw-inner mode

@group Debug
*/
YF void YAC_CALL sdvg_SetEnableDrawInner (sBool _bEnable);

/* @function sdvg_GetEnableDrawBorder:boolean
Query draw-border mode

@group Debug
*/
YF sBool YAC_CALL sdvg_GetEnableDrawBorder (void);

/* @function sdvg_SetEnableDrawBorder,boolean bEnable
Enable or disable(debug) draw-border mode

@group Debug
*/
YF void YAC_CALL sdvg_SetEnableDrawBorder (sBool _bEnable);

// -------- VBO --------
/* @function sdvg_CreateVBO,int numBytes:int
Create vertex buffer object (VBO)

@arg numBytes Vertex buffer object size (bytes)
@return return OpenGL VBO id (0 == failed)

@group VBO
*/
YF sUI YAC_CALL sdvg_CreateVBO (sUI _numBytes);

/* @function sdvg_UpdateVBO,int vboId,int offset,int numBytes,Buffer data
Update vertex buffer object contents

@arg vboId
@arg offset Destination byte offset
@arg numBytes Number of bytes to update (0=use data.size)
@arg data

@group VBO
*/
#ifdef SHADERVG_SCRIPT_API
YF void YAC_CALL sdvg_UpdateVBO (sUI _vboId, sUI _offset, sUI _numBytes, YAC_Object *_data);
#else
void YAC_CALL sdvg_UpdateVBO (sUI _vboId, sUI _offset, sUI _numBytes, YAC_Buffer *_data);
#endif // SHADERVG_SCRIPT_API

/* @function sdvg_BindVBO,int vboId
Bind vertex buffer object

@group VBO
*/
YF void YAC_CALL sdvg_BindVBO (sUI _vboId);

/* @function sdvg_GetVBOSize:int
Query vertex buffer object size

@group VBO
*/
YF sUI YAC_CALL sdvg_GetVBOSize (void);

/* @function sdvg_GetMappedVBOOffset:int
Query write offset of currently mapped vertex buffer object

@group VBO
*/
YF sUI YAC_CALL sdvg_GetMappedVBOOffset (void);

/* @function sdvg_GetMappedVBORef,Buffer ret:boolean
Return reference to currently mapped vertex buffer object data.

@arg ret Returns reference to mapped buffer data (valid as long as VBO is mapped). Offset is set to current write offset.
@return true if return Object has been set to mapped buffer

@group VBO
 */
sBool YAC_CALL sdvg_GetMappedVBORef (Dsdvg_buffer_ref_t _ret);
#ifdef SHADERVG_SCRIPT_API
YF sBool YAC_CALL _sdvg_GetMappedVBORef (YAC_Buffer *_ret);
#endif // SHADERVG_SCRIPT_API

/* @function sdvg_MapVBO,int vboId
Map vertex buffer object VRAM into virtual address space.

%sdvg_BeginVBO can be used to write vertex attribute data to the mapped VBO via attribute emit functions like %sdvg_Vertex2f, %sdvg_TexCoord2f or %sdvg_ColorARGB. The closing %sdvg_End call will in this case not trigger a draw call.

Alternatively (for maximum performance), %sdvg_GetMappedVBORef can be used to obtain a user-space %Buffer for writing directly to the mapped memory.

@group VBO
*/
YF void YAC_CALL sdvg_MapVBO (sUI _vboId);

/* @function sdvg_UnmapVBO
Unmap vertex buffer object

@group VBO
*/
YF void YAC_CALL sdvg_UnmapVBO (void);

/* @function sdvg_UnbindVBO
Unbind current vertex buffer object

@group VBO
*/
YF void YAC_CALL sdvg_UnbindVBO (void);

/* @function sdvg_DestroyVBO,int vboId
Destroy vertex buffer object

@group VBO
*/
YF void YAC_CALL sdvg_DestroyVBO (sUI _vboId);

// -------- FBO --------
/* @function sdvg_CreateFBOEx,int w,int h,int bMSAA,int bDepth,int bStencil:int
Create framebuffer object

@arg w Width
@arg h Height
@arg bMSAA Enable multi-sample anti-aliasing (MSAA)
@arg bDepth Allocate depth buffer
@arg bStencil Allocate stencil buffer (non-standard extension in Desktop GL <4.5)

@return ShaderVG FBO index (0 == failed)

@group FBO
*/
YF sUI YAC_CALL sdvg_CreateFBOEx (sUI _w, sUI _h, sBool _bMSAA, sBool _bDepth, sBool _bStencil);

/* @function sdvg_CreateFBO,int w,int h:int
Create framebuffer object

@arg w Width
@arg h Height

@return ShaderVG FBO index (0 == failed)

@group FBO
*/
YF sUI YAC_CALL sdvg_CreateFBO (sUI _w, sUI _h);

/* @function sdvg_BindFBO,int fboIdx
Bind framebuffer object

@see sdvg_SetFramebufferSize
@group FBO
*/
YF void YAC_CALL sdvg_BindFBO (sUI _fboIdx);

/* @function sdvg_ResolveFBO,int fboIdx
Resolve MSAA framebuffer object

Must be called before FBO can be used as a texture map

@group FBO
*/
YF void YAC_CALL sdvg_ResolveFBO (sUI _fboIdx);

/* @function sdvg_UnbindFBO
Unbind framebuffer object

@group FBO
*/
YF void YAC_CALL sdvg_UnbindFBO (void);

/* @function sdvg_GetFBOTextureId,int fboIdx:int
Query framebuffer object OpenGL texture id

@arg fboIdx ShaderVG fbo index
@return GL texture id

@group FBO
*/
YF sUI YAC_CALL sdvg_GetFBOTextureId (sUI _fboIdx);

/* @function sdvg_BindFBOTexture,int fboIdx,boolean bRepeat,boolean bFilter
Bind framebuffer object as texture

@group FBO
*/
YF void YAC_CALL sdvg_BindFBOTexture (sUI _fboIdx, sBool _bRepeat, sBool _bFilter);

/* @function sdvg_DestroyFBO,int fboIdx
Destroy framebuffer object

@group FBO
*/
YF void YAC_CALL sdvg_DestroyFBO (sUI _fboIdx);

// -------- texturing --------

/* @function sdvg_CreateTexture2D,int texFmt,int w,int h,Object data:int
Create and allocate 2D texture
@arg texFmt  %SDVG_TEXFMT_ALPHA8, %SDVG_TEXFMT_RGB565, %SDVG_TEXFMT_BGRA8888, %SDVG_TEXFMT_ARGB32, %SDVG_TEXFMT_RGBA8888, %
@arg w Width
@arg h Height
@arg data Initial pixel data or null (only reserve memory)
@return OpenGL texture id

@group Texture
*/
sUI YAC_CALL sdvg_CreateTexture2D (sUI _texfmt, sUI _w, sUI _h, const void *_data, sUI _dataSz);
#ifdef SHADERVG_SCRIPT_API
YF sUI YAC_CALL _sdvg_CreateTexture2D (sUI _texfmt, sUI _w, sUI _h, YAC_Object *_data);
#endif // SHADERVG_SCRIPT_API

/* @function sdvg_UpdateTexture2D,int texFmt,int w,int h,Object data
Update contents of currently bound 2D texture
@arg texFmt  %SDVG_TEXFMT_ALPHA8, %SDVG_TEXFMT_RGB565, %SDVG_TEXFMT_BGRA8888, %SDVG_TEXFMT_ARGB32, %SDVG_TEXFMT_RGBA8888
@arg w Width
@arg h Height
@arg data New pixel data

@group Texture
*/
void YAC_CALL sdvg_UpdateTexture2D (sUI _texfmt, sUI _w, sUI _h, const void *_data, sUI _dataSz);
#ifdef SHADERVG_SCRIPT_API
YF void YAC_CALL _sdvg_UpdateTexture2D (sUI _texfmt, sUI _w, sUI _h, YAC_Object *_data);
#endif // SHADERVG_SCRIPT_API

/* @function sdvg_BindMultiTexture2D,int unitIdx,int texId,boolean bRepeat,boolean bFilter
Bind texture to given texture unit

@group Texture
*/
YF void YAC_CALL sdvg_BindMultiTexture2D (sUI _unitIdx, sUI _texId, sBool _bRepeat, sBool _bFilter);

/* @function sdvg_UnbindMultiTexture2D,int unitIdx
Unbind texture from given texture unit

@group Texture
*/
YF void YAC_CALL sdvg_UnbindMultiTexture2D (sUI _unitIdx);

/* @function sdvg_BindTexture2D,int texId,boolean bRepeat,boolean bFilter
Bind texture to default texture unit

@group Texture
*/
YF void YAC_CALL sdvg_BindTexture2D (sUI _texId, sBool _bRepeat, sBool _bFilter);

/* @function sdvg_UnbindTexture2D
Unbind texture from default texture unit

@group Texture
*/
YF void YAC_CALL sdvg_UnbindTexture2D (void);

/* @function sdvg_DestroyTexture2D,int texId
Destroy texture

@group Texture
*/
YF void YAC_CALL sdvg_DestroyTexture2D (sUI _texId);

// -------- VBO utility functions --------
/* @function sdvg_BufferAddRectTexUVFlat32,Buffer b,float x,float y,float w,float h,float ul,float vt,float ur,float vb
Append textured rectangle vertex attributes to buffer

@group BufferAdd
*/
YF void YAC_CALL sdvg_BufferAddRectTexUVFlat32 (YAC_Buffer *_b, sF32 _x, sF32 _y, sF32 _w, sF32 _h, sF32 _ul, sF32 _vt, sF32 _ur, sF32 _vb);

/* @function sdvg_WriteC32AsRGBA8,Buffer b,int c32
Append r,g,b,a bytes to buffer

@arg c32 Packed ARGB32 color

@group BufferAdd
*/
YF void YAC_CALL sdvg_WriteC32AsRGBA8 (YAC_Buffer *_b, sUI _c32);

/* @function sdvg_BufferAddRectTexUVGouraud32,Buffer b,float x,float y,float w,float h,float ul,float vt,float ur,float vb,int c32LT,int c32RT,int c32
Append textured, gouraud shaded rectangle vertex attributes to buffer

@arg x Rectangle X position
@arg y Rectangle Y position
@arg w Rectangle width
@arg h Rectangle height
@arg ul Left texture coordinate (0..1)
@arg vt Top texture coordinate (0..1)
@arg ur Right texture coordinate (0..1)
@arg vb Bottom texture coordinate (0..1)
@arg c32LT Left/Top packed ARGB32 color
@arg c32RT Right/Top packed ARGB32 color
@arg c32LB Left/Bottom packed ARGB32 color
@arg c32RB Right/Bottom packed ARGB32 color

@group BufferAdd
*/
YF void YAC_CALL sdvg_BufferAddRectTexUVGouraud32 (YAC_Buffer *_b,  sF32 _x, sF32 _y, sF32 _w, sF32 _h, sF32 _ul, sF32 _vt, sF32 _ur, sF32 _vb, sUI _c32LT, sUI _c32RT, sUI _c32LB, sUI _c32RB);

/* @function sdvg_BufferAddLinePointFlat14_2,Buffer b,float x,float y
Append line / point vertex coordinate to buffer in 14.2 fixed point format

@group Point
@group BufferAdd
*/
YF void YAC_CALL sdvg_BufferAddLinePointFlat14_2 (YAC_Buffer *_b, sF32 _x, sF32 _y);

/* @function sdvg_BufferAddLinePointFlat32,Buffer b,float x,float y
Append line / point vertex coordinate to buffer in 32bit float format

@group Point
@group BufferAdd
*/
YF void YAC_CALL sdvg_BufferAddLinePointFlat32 (YAC_Buffer *_b, sF32 _x, sF32 _y);

/* @function sdvg_BufferAddLinePointFlatBevel14_2,Buffer b,float x,float y
Append bevel line-joint vertex coordinate to buffer in 14.2 fixed point format

@group Point
@group BufferAdd
*/
YF void YAC_CALL sdvg_BufferAddLinePointFlatBevel14_2 (YAC_Buffer *_b, sF32 _x, sF32 _y);

/* @function sdvg_BufferAddLinePointFlatBevel32,Buffer b,float x,float y
Append bevel line-joint vertex coordinate to buffer in 32bit float format

@group Point
@group BufferAdd
*/
YF void YAC_CALL sdvg_BufferAddLinePointFlatBevel32 (YAC_Buffer *_b, sF32 _x, sF32 _y);

/* @function sdvg_BufferAddLinesPointsFlat14_2,Buffer b,float x1,float y1,float x2,float y2
Append line segment vertex coordinates to buffer in 14.2 fixed point format

@arg x1 Line start point X
@arg y1 Line start point Y
@arg x2 Line end point X
@arg y2 Line end point Y

@group Point
@group BufferAdd
*/
YF void YAC_CALL sdvg_BufferAddLinesPointsFlat14_2 (YAC_Buffer *_b, sF32 _x1, sF32 _y1, sF32 _x2, sF32 _y2);

/* @function sdvg_BufferAddLinesPointsFlat32,Buffer b,float x1,float y1,float x2,float y2
Append line segment vertex coordinates to buffer in 32bit float fixed point

@arg x1 Line start point X
@arg y1 Line start point Y
@arg x2 Line end point X
@arg y2 Line end point Y

@group Point
@group BufferAdd
*/
YF void YAC_CALL sdvg_BufferAddLinesPointsFlat32 (YAC_Buffer *_b, sF32 _x1, sF32 _y1, sF32 _x2, sF32 _y2);

// -------- (low level) draw functions --------
/* @function sdvg_DrawTrianglesFillFlatVBO32,int vboId,int byteOffset,int numVerts
Draw previously prepared vertex buffer as filled triangles (32bit float format)

<pre>
VBO vertex format (8 bytes per vertex):<br>
  +0 f32 x<br>
  +4 f32 y<br>
</pre>
*/
YF void YAC_CALL sdvg_DrawTrianglesFillFlatVBO32 (sUI _vboId, sUI _byteOffset, sUI _numVerts);

/* @function sdvg_DrawTrianglesFillFlatVBO14_2,int vboId,int byteOffset,int numVerts
Draw previously prepared vertex buffer as filled triangles (14.2 fixed point format)

<pre>
VBO vertex format (4 bytes per vertex):<br>
  s14.2 x<br>
  s14.2 y<br>
</pre>

@group Triangle
@groupref Fill
*/
YF void YAC_CALL sdvg_DrawTrianglesFillFlatVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numVerts);

/* @function sdvg_DrawTrianglesFillGouraudVBO32,int vboId,int byteOffset,int numVerts
Draw previously prepared vertex buffer as filled, gouraud shaded triangles (32bit float format)

<pre>
VBO vertex format (12 bytes per vertex):<br>
  u8  r<br>
  u8  g<br>
  u8  b<br>
  u8  a<br>
  f32 x<br>
  f32 y<br>
</pre>

@group Triangle
@groupref Fill
*/
YF void YAC_CALL sdvg_DrawTrianglesFillGouraudVBO32 (sUI _vboId, sUI _byteOffset, sUI _numVerts);

/* @function sdvg_DrawTrianglesFillGouraudVBO14_2,int vboId,int byteOffset,int numVerts
Draw previously prepared vertex buffer as filled, gouraud shaded triangles (14.2 fixed point format)

<pre>
VBO vertex format (8 bytes per vertex):<br>
  +4 u8    r<br>
  +5 u8    g<br>
  +6 u8    b<br>
  +7 u8    a<br>
  +0 s14.2 x<br>
  +2 s14.2 y<br>
</pre>

@group Triangle
@groupref Fill
*/
YF void YAC_CALL sdvg_DrawTrianglesFillGouraudVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numVerts);

/* @function sdvg_DrawTrianglesFillFlatEdgeAAVBO32,int vboId,int byteOffset,int numVerts
experimental

@group Triangle
@groupref Fill
*/
YF void YAC_CALL sdvg_DrawTrianglesFillFlatEdgeAAVBO32 (sUI _vboId, sUI _byteOffset, sUI _numVerts);

/* @function sdvg_DrawTrianglesFillFlatEdgeAAVBO14_2,int vboId,int byteOffset,int numVerts
experimental

@group Triangle
@groupref Fill
*/
YF void YAC_CALL sdvg_DrawTrianglesFillFlatEdgeAAVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numVerts);

/* @function sdvg_DrawTrianglesFillGouraudEdgeAAVBO32,int vboId,int byteOffset,int numVerts
experimental
*/
YF void YAC_CALL sdvg_DrawTrianglesFillGouraudEdgeAAVBO32 (sUI _vboId, sUI _byteOffset, sUI _numVerts);

/* @function sdvg_DrawTrianglesFillGouraudEdgeAAVBO14_2,int vboId,int byteOffset,int numVerts
experimental

@group Triangle
@groupref Fill
*/
YF void YAC_CALL sdvg_DrawTrianglesFillGouraudEdgeAAVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numVerts);

/* @function sdvg_DrawPolygonFillFlatVBO32,int vboId,int byteOffset,int numVerts
Draw previously prepared vertex buffer as filled n-polygon (32bit float format)

<pre>
VBO vertex format (8 bytes per vertex):<br>
  +0 f32 x<br>
  +4 f32 y<br>
</pre>

@group Polygon
@groupref Fill
*/
YF void YAC_CALL sdvg_DrawPolygonFillFlatVBO32 (sUI _vboId, sUI _byteOffset, sUI _numVerts);

/* @function sdvg_DrawPolygonFillFlatVBO14_2,int vboId,int byteOffset,int numVerts
Draw previously prepared vertex buffer as filled n-polygon (14.2 fixed point format)

<pre>
VBO vertex format (4 bytes per vertex):<br>
  s14.2 x<br>
  s14.2 y<br>
</pre>

@group Polygon
@groupref Fill
*/
YF void YAC_CALL sdvg_DrawPolygonFillFlatVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numVerts);

/* @function sdvg_DrawPolygonFillGouraudVBO32,int vboId,int byteOffset,int numVerts
Draw previously prepared vertex buffer as filled n-polygon (32bit float format).

<pre>
VBO vertex format (12 bytes per vertex):<br>
  f32 x<br>
  f32 y<br>
  u8  r<br>
  u8  g<br>
  u8  b<br>
  u8  a<br>
</pre>

@group Polygon
@groupref Fill
*/
YF void YAC_CALL sdvg_DrawPolygonFillGouraudVBO32 (sUI _vboId, sUI _byteOffset, sUI _numVerts);

/* @function sdvg_DrawPolygonFillGouraudVBO14_2,int vboId,int byteOffset,int numVerts
Draw previously prepared vertex buffer as filled n-polygon (14.2 fixed point format).

<pre>
VBO vertex format (8 bytes per vertex):<br>
  s14.2 x<br>
  s14.2 y<br>
  u8    r<br>
  u8    g<br>
  u8    b<br>
  u8    a<br>
</pre>

@group Polygon
@groupref Fill
*/
YF void YAC_CALL sdvg_DrawPolygonFillGouraudVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numVerts);

/* @function sdvg_SetupRectFillAAVBO32,Buffer vb,Buffer dl,float centerX,float centerY,float sizeX,float sizeY
Set up vertex buffer and draw list for filled, anti-aliased rectangle (32bit float format)

@group Rect
@groupref Fill
*/
YF void YAC_CALL sdvg_SetupRectFillAAVBO32 (YAC_Buffer *_vb, YAC_Buffer *_dl, sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY);

/* @function sdvg_DrawRectFillAAVBO32,int vboId,int byteOffsetInner,int numVertsInner,int byteOffsetBorder,int numVertsBorder,int glPrimTypeBorder,float centerX,float centerY,float sizeX,float sizeY
Draw previously prepared vertex buffer as filled, anti-aliased rectangle (32bit float format)

@group Rect
@groupref Fill
*/
YF void YAC_CALL sdvg_DrawRectFillAAVBO32 (sUI _vboId, sUI _byteOffsetInner, sUI _numVertsInner, sUI _byteOffsetBorder, sUI _numVertsBorder, sUI _glPrimTypeBorder, sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY);

/* @function sdvg_DrawRectFillAA,float centerX,float centerY,float sizeX,float sizeY
Draw filled, anti-aliased rectangle via scratch buffer (32 bit float format)

@group Rect
@groupref Fill
*/
YF void YAC_CALL sdvg_DrawRectFillAA (sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY);

/* @function sdvg_SetupRectFillStrokeAAVBO32,Buffer vb,Buffer dl,float centerX,float centerY,float sizeX,float sizeY,float strokeW
Set up vertex buffer and draw list for filled, stroked, and anti-aliased rectangle (32bit float format)

@group Rect
@groupref Fill
@groupref Stroke
*/
YF void YAC_CALL sdvg_SetupRectFillStrokeAAVBO32 (YAC_Buffer *_vb, YAC_Buffer *_dl, sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY, sF32 _strokeW);

/* @function sdvg_DrawRectFillStrokeAAVBO32,int vboId,int byteOffsetInner,int numVertsInner,int byteOffsetBorder,int numVerts
Draw previously prepared vertex buffer as filled, stroked, and anti-aliased rectangle (32bit float format)

@group Rect
@groupref Fill
@groupref Stroke
*/
YF void YAC_CALL sdvg_DrawRectFillStrokeAAVBO32 (sUI _vboId, sUI _byteOffsetInner, sUI _numVertsInner, sUI _byteOffsetBorder, sUI _numVertsBorder, sUI _glPrimTypeBorder, sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY);

/* @function sdvg_DrawRectFillStrokeAA,float centerX,float centerY,float sizeX,float sizeY
Draw filled, stroked, and anti-aliased rectangle via scratch buffer (32 bit float format)

@group Rect
@groupref Fill
@groupref Stroke
*/
YF void YAC_CALL sdvg_DrawRectFillStrokeAA (sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY);

/* @function sdvg_SetupRectStrokeAAVBO32,Buffer vb,Buffer dl,float centerX,float centerY,float sizeX,float sizeY,float strokeW
Set up vertex buffer and draw list for stroked, anti-aliased rectangle (32bit float format)

@group Rect
@groupref Stroke
*/
YF void YAC_CALL sdvg_SetupRectStrokeAAVBO32 (YAC_Buffer *_vb, YAC_Buffer *_dl, sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY, sF32 _strokeW);

/* @function sdvg_DrawRectStrokeAAVBO32,int vboId,int byteOffsetBorder,int numVertesBorder,int glPrimTypeBorder,float centerX,float centerY,float sizeX,float sizeY
Draw previously prepared vertex buffer as stroked, anti-aliased rectangle (32bit float format)

@group Rect
@groupref Stroke
*/
YF void YAC_CALL sdvg_DrawRectStrokeAAVBO32 (sUI _vboId, sUI _byteOffsetBorder, sUI _numVertsBorder, sUI _glPrimTypeBorder, sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY);

/* @function sdvg_DrawRectStrokeAA,float centerX,float centerY,float sizeX,float sizeY
Draw stroked, anti-aliased rectangle via scratch buffer (32 bit float format)

@group Rect
@groupref Stroke
*/
YF void YAC_CALL sdvg_DrawRectStrokeAA (sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY);

/* @function sdvg_SetupEllipseFillAAVBO32,Buffer vb,Buffer dl,float centerX,float centerY,float radiusX,float radiusY
Set up vertex buffer and draw list for filled, anti-aliased ellipse (32bit float format)

@group Ellipse
@groupref Fill
*/
YF void YAC_CALL sdvg_SetupEllipseFillAAVBO32 (YAC_Buffer *_vb, YAC_Buffer *_dl, sF32 _centerX, sF32 _centerY, sF32 _radiusX, sF32 _radiusY);

/* @function sdvg_DrawEllipseFillAAVBO32,int vboId,int byteOffsetInner,int numVertsInner,int byteOffsetBorder,int numVertsBorder,int glPrimTypeBorder,float centerX,float centerY,float radiusX,float radiusY
Draw previously prepared vertex buffer as filled, anti-aliased ellipse (32bit float format)

@group Ellipse
@groupref Fill
*/
YF void YAC_CALL sdvg_DrawEllipseFillAAVBO32 (sUI _vboId, sUI _byteOffsetInner, sUI _numVertsInner, sUI _byteOffsetBorder, sUI _numVertsBorder, sUI _glPrimTypeBorder, sF32 _centerX, sF32 _centerY, sF32 _radiusX, sF32 _radiusY);

/* @function sdvg_DrawEllipseFillAA,float centerX,float centerY,float sizeX,float sizeY
Draw filled, anti-aliased ellipse via scratch buffer (32 bit float format)

@group Ellipse
@groupref Fill
*/
YF void YAC_CALL sdvg_DrawEllipseFillAA (sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY);

/* @function sdvg_SetupEllipseFillStrokeAAVBO32,Buffer vb,Buffer dl,float centerX,float centerY,float radiusX,float radiusY,float strokeW
Set up vertex buffer and draw list for filled, stroked, and anti-aliased ellipse (32bit float format)

@group Ellipse
@groupref Fill
@groupref Stroke
*/
YF void YAC_CALL sdvg_SetupEllipseFillStrokeAAVBO32 (YAC_Buffer *_vb, YAC_Buffer *_dl, sF32 _centerX, sF32 _centerY, sF32 _radiusX, sF32 _radiusY, sF32 _strokeW);

/* @function sdvg_DrawEllipseFillStrokeAAVBO32,int vboId,int byteOffsetInner,int numVertsInner,int byteOffsetBorder,int numVertsBorder,int glPrimTypeBorder,float centerX,float centerY,float radiusX,float radiusY
Draw previously prepared vertex buffer as filled, stroked, and anti-aliased ellipse (32bit float format)

@group Ellipse
@groupref Fill
@groupref Stroke
*/
YF void YAC_CALL sdvg_DrawEllipseFillStrokeAAVBO32 (sUI _vboId, sUI _byteOffsetInner, sUI _numVertsInner, sUI _byteOffsetBorder, sUI _numVertsBorder, sUI _glPrimTypeBorder, sF32 _centerX, sF32 _centerY, sF32 _radiusX, sF32 _radiusY);

/* @function sdvg_DrawEllipseFillStrokeAA,float centerX,float centerY,float sizeX,float sizeY
Draw filled, stroked, and anti-aliased ellipse via scratch buffer (32 bit float format)

@group Ellipse
@groupref Fill
@groupref Stroke
*/
YF void YAC_CALL sdvg_DrawEllipseFillStrokeAA (sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY);

/* @function sdvg_SetupEllipseStrokeAAVBO32,Buffer vb,Buffer dl,float centerX,float centerY,float radiusX,float radiusY,float strokeW
Set up vertex buffer and draw list for stroked, and anti-aliased ellipse (32bit float format)

@group Ellipse
@groupref Stroke
*/
YF void YAC_CALL sdvg_SetupEllipseStrokeAAVBO32 (YAC_Buffer *_vb, YAC_Buffer *_dl, sF32 _centerX, sF32 _centerY, sF32 _radiusX, sF32 _radiusY, sF32 _strokeW);

/* @function sdvg_DrawEllipseStrokeAAVBO32,int vboId,int byteOffsetBorder,int numVertsBorder,int glPrimTypeBorder,float centerX,float centerY,float radiusX,float radiusY
Draw previously prepared vertex buffer as stroked, anti-aliased ellipse (32bit float format)

@group Ellipse
@groupref Stroke
*/
YF void YAC_CALL sdvg_DrawEllipseStrokeAAVBO32 (sUI _vboId, sUI _byteOffsetBorder, sUI _numVertsBorder, sUI _glPrimTypeBorder, sF32 _centerX, sF32 _centerY, sF32 _radiusX, sF32 _radiusY);

/* @function sdvg_DrawEllipseStrokeAA,float centerX,float centerY,float sizeX,float sizeY
Draw stroked, anti-aliased ellipse via scratch buffer (32 bit float format)

@group Ellipse
@groupref Stroke
*/
YF void YAC_CALL sdvg_DrawEllipseStrokeAA (sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY);

/* @function sdvg_SetupRoundRectFillAAVBO32,Buffer vb,Buffer dl,float centerX,float centerY,float sizeX,float sizeY,float radiusX,float radiusY
Set up vertex buffer and draw list for filled, and anti-aliased rounded rectangle (32bit float format)

@group RoundRect
@groupref Fill
*/
YF void YAC_CALL sdvg_SetupRoundRectFillAAVBO32 (YAC_Buffer *_vb, YAC_Buffer *_dl, sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY, sF32 _radiusX, sF32 _radiusY);

/* @function sdvg_DrawRoundRectFillAAVBO32,int vboId,int byteOffsetInner,int numVertsInner,int byteOffsetBorder,int numVertsBorder,int glPrimTypeBorder,float centerX,float centerY,float sizeX,float sizeY,float radiusX,float radiusY
Draw previously prepared vertex buffer as filled, anti-aliased rounded rectangle (32bit float format)

@group RoundRect
@groupref Fill
*/
YF void YAC_CALL sdvg_DrawRoundRectFillAAVBO32 (sUI _vboId, sUI _byteOffsetInner, sUI _numVertsInner, sUI _byteOffsetBorder, sUI _numVertsBorder, sUI _glPrimTypeBorder, sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY, sF32 _radiusX, sF32 _radiusY);

/* @function sdvg_DrawRoundRectFillAA,float centerX,float centerY,float sizeX,float sizeY,float radiusX,float radiusY
Draw filled, anti-aliased rounded rectangle via scratch buffer (32 bit float format)

@group RoundRect
@groupref Fill
*/
YF void YAC_CALL sdvg_DrawRoundRectFillAA (sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY, sF32 _radiusX, sF32 _radiusY);

/* @function sdvg_SetupRoundRectFillStrokeAAVBO32,Buffer vb,Buffer dl,float centerX,float centerY,float sizeX,float sizeY,float radiusX,float radiusY,float strokeW
Set up vertex buffer and draw list for filled, stroked, and anti-aliased rounded rectangle (32bit float format)

@group RoundRect
@groupref Fill
@groupref Stroke
*/
YF void YAC_CALL sdvg_SetupRoundRectFillStrokeAAVBO32 (YAC_Buffer *_vb, YAC_Buffer *_dl, sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY, sF32 _radiusX, sF32 _radiusY, sF32 _strokeW);

/* @function sdvg_DrawRoundRectFillStrokeAAVBO32,int vboId,int byteOffsetInner,int numVertsInner,int byteOffsetBorder,int numVertsBorder,int glPrimTypeBorder,float centerX,float centerY,float sizeX,float sizeY,float radiusX,float radiusY
Draw previously prepared vertex buffer as filled, stroked, anti-aliased rounded rectangle (32bit float format)

<pre>
Draw-list format:<br>
  +0  u16 aaRange * 256<br>
  +2  i32 vbOffInner<br>
  +6  u16 numVertsInner  (GL_TRIANGLES)<br>
  +8  i32 vbOffBorder<br>
  +12 u16 numVertsBorder<br>
  +14 u16 primTypeBorder (GL_TRIANGLE_FAN(0x0006) or GL_TRIANGLES(0x0004))<br>
</pre>

@group RoundRect
@groupref Fill
@groupref Stroke
*/
YF void YAC_CALL sdvg_DrawRoundRectFillStrokeAAVBO32 (sUI _vboId, sUI _byteOffsetInner, sUI _numVertsInner, sUI _byteOffsetBorder, sUI _numVertsBorder, sUI _glPrimTypeBorder, sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY, sF32 _radiusX, sF32 _radiusY);

/* @function sdvg_DrawRoundRectFillStrokeAA,float centerX,float centerY,float sizeX,float sizeY,float radiusX,float radiusY
Draw filled, stroked, anti-aliased rounded rectangle via scratch buffer (32 bit float format)

@group RoundRect
@groupref Fill
@groupref Stroke
*/
YF void YAC_CALL sdvg_DrawRoundRectFillStrokeAA (sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY, sF32 _radiusX, sF32 _radiusY);

/* @function sdvg_SetupRoundRectStrokeAAVBO32,Buffer vb,Buffer dl,float centerX,float centerY,float sizeX,float sizeY,float radiusX,float radiusY,float strokeW
Set up vertex buffer and draw list for stroked, anti-aliased rounded rectangle (32bit float format)

@group RoundRect
@groupref Stroke
*/
YF void YAC_CALL sdvg_SetupRoundRectStrokeAAVBO32 (YAC_Buffer *_vb, YAC_Buffer *_dl, sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY, sF32 _radiusX, sF32 _radiusY, sF32 _strokeW);

/* @function sdvg_DrawRoundRectStrokeAAVBO32,int vboId,int byteOffsetBorder,int numVertsBorder,int glPrimTypeBorder,float centerX,float centerY,float sizeX,float sizeY,float radiusX,float radiusY
Draw previously prepared vertex buffer as stroked, anti-aliased rounded rectangle (32bit float format)

@group RoundRect
@groupref Stroke
*/
YF void YAC_CALL sdvg_DrawRoundRectStrokeAAVBO32(sUI _vboId, sUI _byteOffsetBorder, sUI _numVertsBorder, sUI _glPrimTypeBorder, sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY, sF32 _radiusX, sF32 _radiusY);

/* @function sdvg_DrawRoundRectStrokeAA,float centerX,float centerY,float sizeX,float sizeY,float radiusX,float radiusY
Draw stroked, anti-aliased rounded rectangle via scratch buffer (32 bit float format)

@group RoundRect
@groupref Stroke
*/
YF void YAC_CALL sdvg_DrawRoundRectStrokeAA (sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY, sF32 _radiusX, sF32 _radiusY);

/* @function sdvg_DrawTrianglesTexUVFlatVBO32,int vboId,int byteOffset,int numVerts
Draw previously prepared vertex buffer as textured mapped triangles (32bit float format)

<pre>
VBO vertex format (16 bytes per vertex):<br>
    +0  f32 x<br>
    +4  f32 y<br>
    +8  f32 u<br>
    +12 f32 v<br>
</pre>

@group Triangle
@groupref Texture
@groupref Fill
*/
YF void YAC_CALL sdvg_DrawTrianglesTexUVFlatVBO32 (sUI _vboId, sUI _byteOffset, sUI _numVerts);

/* @function sdvg_DrawTrianglesTexUVGouraudVBO32,int vboId,int byteOffset,int numVerts
Draw previously prepared vertex buffer as textured mapped, gouraud shaded triangles (32bit float format)

<pre>
VBO vertex format (20 bytes per vertex):<br>
    +0  f32 x<br>
    +4  f32 y<br>
    +8  f32 u<br>
    +12 f32 v<br>
    +16  u8 r<br>
    +17  u8 g<br>
    +18  u8 b<br>
    +19  u8 a<br>
</pre>

@group Triangle
@groupref Texture
@groupref Fill
*/
YF void YAC_CALL sdvg_DrawTrianglesTexUVGouraudVBO32 (sUI _vboId, sUI _byteOffset, sUI _numVerts);

/* @function sdvg_DrawTrianglesTexUVFlatDecalVBO32,int vboId,int byteOffset,int numVerts
Draw previously prepared vertex buffer as decal-textured mapped triangles (32bit float format)

<pre>
VBO vertex format (16 bytes per vertex):<br>
    +0  f32 x<br>
    +4  f32 y<br>
    +8  f32 u<br>
    +12 f32 v<br>
</pre>

@group Triangle
@groupref Texture
@groupref Fill
@group Decal
*/
YF void YAC_CALL sdvg_DrawTrianglesTexUVFlatDecalVBO32 (sUI _vboId, sUI _byteOffset, sUI _numVerts);

/* @function sdvg_DrawTrianglesTexUVGouraudDecalVBO32,int vboId,int byteOffset,int numVerts
Draw previously prepared vertex buffer as decal-textured mapped, gouraud shaded triangles (32bit float format)

<pre>
VBO vertex format (20 bytes per vertex):<br>
    +0  f32 x<br>
    +4  f32 y<br>
    +8  f32 u<br>
    +12 f32 v<br>
    +16  u8 r<br>
    +17  u8 g<br>
    +18  u8 b<br>
    +19  u8 a<br>
</pre>

@group Triangle
@groupref Texture
@groupref Fill
@group Decal
*/
YF void YAC_CALL sdvg_DrawTrianglesTexUVGouraudDecalVBO32 (sUI _vboId, sUI _byteOffset, sUI _numVerts);

/* @function sdvg_DrawTrianglesTexUVFlatVBO32Alpha,int vboId,int byteOffset,int numVerts
Draw previously prepared vertex buffer as alpha-texture mapped triangles (32bit float format)

<pre>
VBO vertex format (16 bytes per vertex):<br>
    +0  f32 x<br>
    +4  f32 y<br>
    +8  f32 u<br>
    +12 f32 v<br>
</pre>

@group Triangle
@groupref Texture
@groupref Fill
*/
YF void YAC_CALL sdvg_DrawTrianglesTexUVFlatVBO32Alpha (sUI _vboId, sUI _byteOffset, sUI _numVerts);

/* @function sdvg_DrawTrianglesTexUVGouraudVBO32Alpha,int vboId,int byteOffset,int numVerts
Draw previously prepared vertex buffer as alpha-texture mapped, gouraud shaded triangles (32bit float format)

<pre>
VBO vertex format (20 bytes per vertex):<br>
    +0  f32 x<br>
    +4  f32 y<br>
    +8  f32 u<br>
    +12 f32 v<br>
    +16  u8 r<br>
    +17  u8 g<br>
    +18  u8 b<br>
    +19  u8 a<br>
</pre>

@group Triangle
@groupref Texture
@groupref Fill
*/
YF void YAC_CALL sdvg_DrawTrianglesTexUVGouraudVBO32Alpha (sUI _vboId, sUI _byteOffset, sUI _numVerts);

/* @function sdvg_DrawTrianglesTexUVFlatDecalVBO32Alpha,int vboId,int byteOffset,int numVerts
Draw previously prepared vertex buffer as alpha-decal-texture mapped triangles (32bit float format)

<pre>
VBO vertex format (16 bytes per vertex):<br>
    +0  f32 x<br>
    +4  f32 y<br>
    +8  f32 u<br>
    +12 f32 v<br>
</pre>

@group Triangle
@groupref Texture
@groupref Fill
@group Decal
*/
YF void YAC_CALL sdvg_DrawTrianglesTexUVFlatDecalVBO32Alpha (sUI _vboId, sUI _byteOffset, sUI _numVerts);

/* @function sdvg_DrawTrianglesTexUVGouraudDecalVBO32Alpha,int vboId,int byteOffset,int numVerts
Draw previously prepared vertex buffer as alpha-decal-texture mapped, gouraud shaded triangles (32bit float format)

<pre>
VBO vertex format (20 bytes per vertex):<br>
    +0  f32 x<br>
    +4  f32 y<br>
    +8  f32 u<br>
    +12 f32 v<br>
    +16  u8 r<br>
    +17  u8 g<br>
    +18  u8 b<br>
    +19  u8 a<br>
</pre>

@group Triangle
@groupref Texture
@groupref Fill
@group Decal
*/
YF void YAC_CALL sdvg_DrawTrianglesTexUVGouraudDecalVBO32Alpha (sUI _vboId, sUI _byteOffset, sUI _numVerts);

/* @function sdvg_DrawTrianglesTexUVFlatVBO32AlphaSDF,int vboId,int byteOffset,int numVerts
Draw previously prepared vertex buffer as alpha-SDF-texture mapped triangles (32bit float format)

<pre>
VBO vertex format (16 bytes per vertex):<br>
    +0  f32 x<br>
    +4  f32 y<br>
    +8  f32 u<br>
    +12 f32 v<br>
</pre>

@group Triangle
@groupref Texture
@groupref Fill
@group SDF
*/
YF void YAC_CALL sdvg_DrawTrianglesTexUVFlatVBO32AlphaSDF (sUI _vboId, sUI _byteOffset, sUI _numVerts);

/* @function sdvg_DrawLineStripFlatVBO14_2,int vboId,int byteOffset,int numPoints
Draw previously prepared vertex buffer as line strip (14.2 fixed point format)

<pre>
VBO vertex format (4 bytes per vertex):<br>
  +0 s14.2 x<br>
  +2 s14.2 y<br>
</pre>

@group Line
@groupref Stroke
*/
YF void YAC_CALL sdvg_DrawLineStripFlatVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawLineStripFlatVBO32,int vboId,int byteOffset,int numPoints
Draw previously prepared vertex buffer as line strip (32 bit float format)

@group Line
@groupref Stroke
*/
YF void YAC_CALL sdvg_DrawLineStripFlatVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawLineStripFlatAAVBO14_2,int vboId,int byteOffset,int numPoints
Draw previously prepared vertex buffer as anti-aliased line strip (14.2 fixed point format)

<pre>
VBO vertex format (4 bytes per vertex):<br>
  +0 s14.2 x<br>
  +2 s14.2 y<br>
</pre>

@group Line
@groupref Stroke
*/
YF void YAC_CALL sdvg_DrawLineStripFlatAAVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawLineStripFlatAAVBO32,int vboId,int byteOffset,int numPoints
Draw previously prepared vertex buffer as anti-aliased line strip (32 bit float format)

@group Line
@groupref Stroke
*/
YF void YAC_CALL sdvg_DrawLineStripFlatAAVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawLineStripPatternVBO14_2,int vboId,int byteOffset,int numPoints
Draw previously prepared vertex buffer as patterned line strip (14.2 fixed point format)

@group Line
@groupref LinePattern
@groupref Stroke
*/
YF void YAC_CALL sdvg_DrawLineStripPatternVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawLineStripPatternVBO32,int vboId,int byteOffset,int numPoints
Draw previously prepared vertex buffer as patterned line strip (32 bit float format)

@group Line
@groupref LinePattern
@groupref Stroke
*/
YF void YAC_CALL sdvg_DrawLineStripPatternVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawLineStripPatternDecalVBO14_2,int vboId,int byteOffset,int numPoints
Draw previously prepared vertex buffer as patterned line strip (14.2 fixed point format)

@group Line
@groupref LinePattern
@groupref Stroke
@groupref Fill
@group Decal
*/
YF void YAC_CALL sdvg_DrawLineStripPatternDecalVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawLineStripPatternDecalVBO32,int vboId,int byteOffset,int numPoints
Draw previously prepared vertex buffer as patterned line strip (32 bit float format)

@group Line
@groupref LinePattern
@groupref Stroke
@groupref Fill
@group Decal
*/
YF void YAC_CALL sdvg_DrawLineStripPatternDecalVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawLineStripPatternAAVBO14_2,int vboId,int byteOffset,int numPoints
Draw previously prepared vertex buffer as anti-aliased, patterned line strip (14.2 fixed point format)

@group Line
@groupref LinePattern
@groupref Stroke
@groupref Fill
@group Decal
*/
YF void YAC_CALL sdvg_DrawLineStripPatternAAVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawLineStripPatternAAVBO32,int vboId,int byteOffset,int numPoints
Draw previously prepared vertex buffer as anti-aliased, patterned line strip (32 bit float format)

@group Line
@groupref LinePattern
@groupref Stroke
@groupref Fill
@group Decal
*/
YF void YAC_CALL sdvg_DrawLineStripPatternAAVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawLineStripPatternDecalAAVBO14_2,int vboId,int byteOffset,int numPoints
Draw previously prepared vertex buffer as anti-aliased, patterned line strip (14.2 fixed point format)

@group Line
@groupref LinePattern
@groupref Stroke
@groupref Fill
@group Decal
*/
YF void YAC_CALL sdvg_DrawLineStripPatternDecalAAVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawLineStripPatternDecalAAVBO32,int vboId,int byteOffset,int numPoints
Draw previously prepared vertex buffer as anti-aliased, patterned line strip (32 bit float format)

@group Line
@groupref LinePattern
@groupref Stroke
@groupref Fill
@group Decal
*/
YF void YAC_CALL sdvg_DrawLineStripPatternDecalAAVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawLineStripFlatBevelVBO14_2,int vboId,int byteOffset,int numPoints,boolean bSkipLastLineJoint
Draw previously prepared vertex buffer as line strip with bevel line joints (14.2 fixed point format)

<pre>
VBO vertex format (4 bytes per vertex):<br>
  +0 s14.2 x<br>
  +2 s14.2 y<br>
</pre>

@arg vboId Vertex buffer object id
@arg byteOffset Vertex buffer start offset
@arg numPoints Number of vertices (numPoints-2 will be drawn). For closed line loops, the last two points must equal the first two.
@arg bSkipLastLineJoint Must be true for closed line loops. false=do not render line joint at end of polyline.

@group Line
@groupref Stroke
*/
YF void YAC_CALL sdvg_DrawLineStripFlatBevelVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numPoints, sBool _bSkipLastLineJoint);

/* @function sdvg_DrawLineStripFlatBevelVBO32,int vboId,int byteOffset,int numPoints,boolean bSkipLastLineJoint
Draw previously prepared vertex buffer as line strip with bevel line joints (32 bit float format)

@arg vboId Vertex buffer object id
@arg byteOffset Vertex buffer start offset
@arg numPoints Number of vertices (numPoints-2 will be drawn). For closed line loops, the last two points must equal the first two.
@arg bSkipLastLineJoint Must be true for closed line loops. false=do not render line joint at end of polyline.

@group Line
@groupref Stroke
*/
YF void YAC_CALL sdvg_DrawLineStripFlatBevelVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints, sBool _bSkipLastLineJoint);

/* @function sdvg_DrawLineStripFlatBevelAAVBO14_2,int vboId,int byteOffset,int numPoints,boolean bSkipLastLineJoint
Draw previously prepared vertex buffer as anti-aliased line strip with bevel line joints (14.2 fixed point format)

<pre>
VBO vertex format (4 bytes per vertex):<br>
  +0 s14.2 x<br>
  +2 s14.2 y<br>
</pre>

@arg vboId Vertex buffer object id
@arg byteOffset Vertex buffer start offset
@arg numPoints Number of vertices (numPoints-2 will be drawn). For closed line loops, the last two points must equal the first two.
@arg bSkipLastLineJoint Must be true for closed line loops. false=do not render line joint at end of polyline.

@group Line
@groupref Stroke
*/
YF void YAC_CALL sdvg_DrawLineStripFlatBevelAAVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numPoints, sBool _bSkipLastLineJoint);

/* @function sdvg_DrawLineStripFlatBevelAAVBO32,int vboId,int byteOffset,int numPoints,boolean bSkipLastLineJoint
Draw previously prepared vertex buffer as anti-aliased line strip with bevel line joints (32 bit float format)

@group Line
@groupref Stroke
*/
YF void YAC_CALL sdvg_DrawLineStripFlatBevelAAVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints, sBool _bSkipLastLineJoint);

/* @function sdvg_DrawLineStripPatternBevelVBO14_2,int vboId,int byteOffset,int numPoints,boolean bSkipLastLineJoint
Draw previously prepared vertex buffer as patterned line strip with bevel line joints (14.2 fixed point format)

@arg vboId Vertex buffer object id
@arg byteOffset Vertex buffer start offset
@arg numPoints Number of vertices (numPoints-2 will be drawn). For closed line loops, the last two points must equal the first two.
@arg bSkipLastLineJoint Must be true for closed line loops. false=do not render line joint at end of polyline.

@group Line
@groupref LinePattern
@groupref Stroke
*/
YF void YAC_CALL sdvg_DrawLineStripPatternBevelVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numPoints, sBool _bSkipLastLineJoint);

/* @function sdvg_DrawLineStripPatternBevelVBO32,int vboId,int byteOffset,int numPoints,boolean bSkipLastLineJoint
Draw previously prepared vertex buffer as patterned line strip with bevel line joints (32 bit float format)

@arg vboId Vertex buffer object id
@arg byteOffset Vertex buffer start offset
@arg numPoints Number of vertices (numPoints-2 will be drawn). For closed line loops, the last two points must equal the first two.
@arg bSkipLastLineJoint Must be true for closed line loops. false=do not render line joint at end of polyline.

@group Line
@groupref LinePattern
@groupref Stroke
*/
YF void YAC_CALL sdvg_DrawLineStripPatternBevelVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints, sBool _bSkipLastLineJoint);

/* @function sdvg_DrawLineStripPatternDecalBevelVBO14_2,int vboId,int byteOffset,int numPoints,boolean bSkipLastLineJoint
Draw previously prepared vertex buffer as patterned line strip with bevel line joints (14.2 fixed point format)

@arg vboId Vertex buffer object id
@arg byteOffset Vertex buffer start offset
@arg numPoints Number of vertices (numPoints-2 will be drawn). For closed line loops, the last two points must equal the first two.
@arg bSkipLastLineJoint Must be true for closed line loops. false=do not render line joint at end of polyline.

@group Line
@groupref LinePattern
@groupref Stroke
*/
YF void YAC_CALL sdvg_DrawLineStripPatternDecalBevelVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numPoints, sBool _bSkipLastLineJoint);

/* @function sdvg_DrawLineStripPatternDecalBevelVBO32,int vboId,int byteOffset,int numPoints,boolean bSkipLastLineJoint
Draw previously prepared vertex buffer as patterned line strip with bevel line joints (32 bit float format)

@arg vboId Vertex buffer object id
@arg byteOffset Vertex buffer start offset
@arg numPoints Number of vertices (numPoints-2 will be drawn). For closed line loops, the last two points must equal the first two.
@arg bSkipLastLineJoint Must be true for closed line loops. false=do not render line joint at end of polyline.

@group Line
@groupref LinePattern
@group LineJoint
@groupref Stroke
*/
YF void YAC_CALL sdvg_DrawLineStripPatternDecalBevelVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints, sBool _bSkipLastLineJoint);

/* @function sdvg_DrawLineStripPatternBevelAAVBO14_2,int vboId,int byteOffset,int numPoints,boolean bSkipLastLineJoint
Draw previously prepared vertex buffer as anti-aliased, patterned line strip with bevel line joints (14.2 fixed point format)

@arg vboId Vertex buffer object id
@arg byteOffset Vertex buffer start offset
@arg numPoints Number of vertices (numPoints-2 will be drawn). For closed line loops, the last two points must equal the first two.
@arg bSkipLastLineJoint Must be true for closed line loops. false=do not render line joint at end of polyline.

@group Line
@groupref LinePattern
@group LineJoint
@groupref Stroke
*/
YF void YAC_CALL sdvg_DrawLineStripPatternBevelAAVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numPoints, sBool _bSkipLastLineJoint);

/* @function sdvg_DrawLineStripPatternBevelAAVBO32,int vboId,int byteOffset,int numPoints,boolean bSkipLastLineJoint
Draw previously prepared vertex buffer as anti-aliased, patterned line strip with bevel line joints (32 bit float format)

@arg vboId Vertex buffer object id
@arg byteOffset Vertex buffer start offset
@arg numPoints Number of vertices (numPoints-2 will be drawn). For closed line loops, the last two points must equal the first two.
@arg bSkipLastLineJoint Must be true for closed line loops. false=do not render line joint at end of polyline.

@group Line
@groupref LinePattern
@group LineJoint
@groupref Stroke
*/
YF void YAC_CALL sdvg_DrawLineStripPatternBevelAAVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints, sBool _bSkipLastLineJoint);

/* @function sdvg_DrawLineStripPatternDecalBevelAAVBO14_2,int vboId,int byteOffset,int numPoints,boolean bSkipLastLineJoint
Draw previously prepared vertex buffer as anti-aliased, patterned line strip with bevel line joints (14.2 fixed point format)

@arg vboId Vertex buffer object id
@arg byteOffset Vertex buffer start offset
@arg numPoints Number of vertices (numPoints-2 will be drawn). For closed line loops, the last two points must equal the first two.
@arg bSkipLastLineJoint Must be true for closed line loops. false=do not render line joint at end of polyline.

@group Line
@groupref LinePattern
@group LineJoint
@groupref Stroke
@groupref Fill
@group Decal
*/
YF void YAC_CALL sdvg_DrawLineStripPatternDecalBevelAAVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numPoints, sBool _bSkipLastLineJoint);

/* @function sdvg_DrawLineStripPatternDecalBevelAAVBO32,int vboId,int byteOffset,int numPoints,boolean bSkipLastLineJoint
Draw previously prepared vertex buffer as anti-aliased, patterned line strip with bevel line joints (32 bit float format)

@arg vboId Vertex buffer object id
@arg byteOffset Vertex buffer start offset
@arg numPoints Number of vertices (numPoints-2 will be drawn). For closed line loops, the last two points must equal the first two.
@arg bSkipLastLineJoint Must be true for closed line loops. false=do not render line joint at end of polyline.

@group Line
@groupref LinePattern
@group LineJoint
@groupref Stroke
@groupref Fill
@group Decal
*/
YF void YAC_CALL sdvg_DrawLineStripPatternDecalBevelAAVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints, sBool _bSkipLastLineJoint);

/* @function sdvg_DrawLineStripFlatMiterVBO14_2,int vboId,int byteOffset,int numPoints
Draw previously prepared vertex buffer as line strip with miter line joints (14.2 fixed point format)

@group Line
@groupref Stroke
*/
YF void YAC_CALL sdvg_DrawLineStripFlatMiterVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawLineStripFlatMiterVBO32,int vboId,int byteOffset,int numPoints
Draw previously prepared vertex buffer as line strip with miter line joints (32 bit float format)

@group Line
@groupref Stroke
*/
YF void YAC_CALL sdvg_DrawLineStripFlatMiterVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawLineStripFlatMiterAAVBO14_2,int vboId,int byteOffset,int numPoints
Draw previously prepared vertex buffer as anti-aliased line strip with miter line joints (14.2 fixed point format)

@group Line
@groupref Stroke
*/
YF void YAC_CALL sdvg_DrawLineStripFlatMiterAAVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawLineStripFlatMiterAAVBO32,int vboId,int byteOffset,int numPoints
Draw previously prepared vertex buffer as anti-aliased line strip with miter line joints (32 bit float format)

@group Line
@groupref Stroke
*/
YF void YAC_CALL sdvg_DrawLineStripFlatMiterAAVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawLinesFlatVBO14_2,int vboId,int byteOffset,int numPoints
Draw previously prepared vertex buffer as line segments (14.2 fixed point format)

<pre>
VBO vertex format (4 bytes per vertex):<br>
  +0 s14.2 x<br>
  +2 s14.2 y<br>
</pre>

@group Line
@groupref Stroke
*/
YF void YAC_CALL sdvg_DrawLinesFlatVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawLinesFlatVBO32,int vboId,int byteOffset,int numPoints
Draw previously prepared vertex buffer as line segments (32 bit float format)

@group Line
@groupref Stroke
*/
YF void YAC_CALL sdvg_DrawLinesFlatVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawLinesFlatAAVBO14_2,int vboId,int byteOffset,int numPoints
Draw previously prepared vertex buffer as anti-aliased line segments (14.2 fixed point format)

<pre>
VBO vertex format (4 bytes per vertex):<br>
  +0 s14.2 x<br>
  +2 s14.2 y<br>
</pre>

@group Line
@groupref Stroke
*/
YF void YAC_CALL sdvg_DrawLinesFlatAAVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawLinesFlatAAVBO32,int vboId,int byteOffset,int numPoints
Draw previously prepared vertex buffer as anti-aliased line segments (32 bit float format)

@group Line
@groupref Stroke
*/
YF void YAC_CALL sdvg_DrawLinesFlatAAVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawLinesGouraudVBO14_2,int vboId,int byteOffset,int numPoints
Draw previously prepared vertex buffer as gouraud-shaded line segments (14.2 fixed point format)

@group Line
@groupref Stroke
@group Gouraud
*/
YF void YAC_CALL sdvg_DrawLinesGouraudVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawLinesGouraudAAVBO14_2,int vboId,int byteOffset,int numPoints
Draw previously prepared vertex buffer as anti-aliased, gouraud-shaded line segments (14.2 fixed point format)

@group Line
@groupref Stroke
@group Gouraud
*/
YF void YAC_CALL sdvg_DrawLinesGouraudAAVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawLinesGouraudVBO32,int vboId,int byteOffset,int numPoints
Draw previously prepared vertex buffer as gouraud-shaded line segments (32 bit float format)

@group Line
@groupref Stroke
@group Gouraud
*/
YF void YAC_CALL sdvg_DrawLinesGouraudVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawLinesGouraudAAVBO32,int vboId,int byteOffset,int numPoints
Draw previously prepared vertex buffer as anti-aliased, gouraud-shaded line segments (32 bit float format)

@group Line
@groupref Stroke
@group Gouraud
*/
YF void YAC_CALL sdvg_DrawLinesGouraudAAVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawLinesPatternVBO14_2,int vboId,int byteOffset,int numPoints
Draw previously prepared vertex buffer as patterned line segments (14.2 fixed point format)

@group Line
@groupref Stroke
@groupref LinePattern
@groupref Texture
*/
YF void YAC_CALL sdvg_DrawLinesPatternVBO14_2(sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawLinesPatternAAVBO14_2,int vboId,int byteOffset,int numPoints
Draw previously prepared vertex buffer as anti-aliased, patterned line segments (14.2 fixed point format)

@group Line
@groupref Stroke
@groupref LinePattern
@groupref Texture
*/
YF void YAC_CALL sdvg_DrawLinesPatternAAVBO14_2(sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawLinesPatternVBO32,int vboId,int byteOffset,int numPoints
Draw previously prepared vertex buffer as patterned line segments (32 bit float format)

@group Line
@groupref Stroke
@groupref LinePattern
@groupref Texture
*/
YF void YAC_CALL sdvg_DrawLinesPatternVBO32(sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawLinesPatternAAVBO32,int vboId,int byteOffset,int numPoints
Draw previously prepared vertex buffer as anti-aliased, patterned line segments (32 bit float format)

@group Line
@groupref Stroke
@groupref LinePattern
@groupref Texture
*/
YF void YAC_CALL sdvg_DrawLinesPatternAAVBO32(sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawPointsSquareVBO32,int vboId,int byteOffset,int numPoints
Draw previously prepared vertex buffer as square points (32 bit float format)

<pre>
VBO vertex format (8 bytes per vertex):<br>
  +0 f32 x<br>
  +4 f32 y<br>
</pre>

@group Point
@groupref Stroke
*/
YF void YAC_CALL sdvg_DrawPointsSquareVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawPointsSquareAAVBO32,int vboId,int byteOffset,int numPoints
Draw previously prepared vertex buffer as anti-aliased, square points (32 bit float format)

<pre>
VBO vertex format (8 bytes per vertex):<br>
  +0 f32 x<br>
  +4 f32 y<br>
</pre>

@group Point
@groupref Stroke
*/
YF void YAC_CALL sdvg_DrawPointsSquareAAVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawPointsSquareGouraudVBO32,int vboId,int byteOffset,int numPoints
Draw previously prepared vertex buffer as gouraud-shaded square points (32 bit float format)

<pre>
VBO vertex format (12 bytes per vertex):<br>
  +0 u8  r<br>
  +1 u8  g<br>
  +2 u8  b<br>
  +3 u8  a<br>
  +4 f32 x<br>
  +8 f32 y<br>
</pre>

@group Point
@groupref Stroke
@group Gouraud
*/
YF void YAC_CALL sdvg_DrawPointsSquareGouraudVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawPointsSquareGouraudAAVBO32,int vboId,int byteOffset,int numPoints
Draw previously prepared vertex buffer as anti-aliased, gouraud-shaded square points (32 bit float format)

<pre>
VBO vertex format (12 bytes per vertex):<br>
  +0 u8  r<br>
  +1 u8  g<br>
  +2 u8  b<br>
  +3 u8  a<br>
  +4 f32 x<br>
  +8 f32 y<br>

@group Point
@groupref Stroke
@group Gouraud
*/
YF void YAC_CALL sdvg_DrawPointsSquareGouraudAAVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawPointsRoundVBO32,int vboId,int byteOffset,int numPoints
Draw previously prepared vertex buffer as round points (32 bit float format)

<pre>
VBO vertex format (8 bytes per vertex):<br>
  +0 f32 x<br>
  +4 f32 y<br>
</pre>

@group Point
@groupref Stroke
*/
YF void YAC_CALL sdvg_DrawPointsRoundVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawPointsRoundAAVBO32,int vboId,int byteOffset,int numPoints
Draw previously prepared vertex buffer as anti-aliased, round points (32 bit float format)

<pre>
VBO vertex format (8 bytes per vertex):<br>
  +0 f32 x<br>
  +4 f32 y<br>
</pre>

@group Point
@groupref Stroke
*/
YF void YAC_CALL sdvg_DrawPointsRoundAAVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawPointsRoundGouraudVBO32,int vboId,int byteOffset,int numPoints
Draw previously prepared vertex buffer as gouraud-shaded round points (32 bit float format)

<pre>
VBO vertex format (12 bytes per vertex):<br>
  +0 u8  r<br>
  +1 u8  g<br>
  +2 u8  b<br>
  +3 u8  a<br>
  +4 f32 x<br>
  +8 f32 y<br>
</pre>

@group Point
@groupref Stroke
@group Gouraud
*/
YF void YAC_CALL sdvg_DrawPointsRoundGouraudVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawPointsRoundGouraudAAVBO32,int vboId,int byteOffset,int numPoints
Draw previously prepared vertex buffer as anti-aliased, gouraud-shaded round points (32 bit float format)

<pre>
VBO vertex format (12 bytes per vertex):<br>
  +0 u8  r<br>
  +1 u8  g<br>
  +2 u8  b<br>
  +3 u8  a<br>
  +4 f32 x<br>
  +8 f32 y<br>
</pre>

@group Point
@groupref Stroke
@group Gouraud
*/
YF void YAC_CALL sdvg_DrawPointsRoundGouraudAAVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

// ----------- custom shaders and (scratch) draw functions ------------
/* @function sdvg_CreateShader,String vs,String fs:int
Create user-defined shader program

@arg vs Vertex shader source
@arg fs Fragment shader source

@return ShaderVG shader index

@group Shader
@groupref Attrib
@groupref Uniform
*/
sUI YAC_CALL sdvg_CreateShader (const char *vs, const char *fs);

/* @function sdvg_DestroyShader,int shaderIdx
Destroy user-defined shader program

@group Shader
*/
YF void YAC_CALL sdvg_DestroyShader (sUI _shaderIdx);

/* @function sdvg_BindShader,int shaderIdx
Bind user-defined shader program

@group Shader
@groupref Attrib
@groupref Uniform
*/
YF void YAC_CALL sdvg_BindShader (sUI _shaderIdx);

/* @function sdvg_UnbindShader
Unbind current shader program

@group Shader
*/
YF void YAC_CALL sdvg_UnbindShader (void);

/* @function sdvg_GetUniformLocation,String name:int
Query uniform location in currently bound shader program

@groupref Shader
@group Uniform
*/
sSI YAC_CALL sdvg_GetUniformLocation (const char *_name);

/* @function sdvg_GetAttribLocation,String name:int
Query attribute location in currently bound shader program

@groupref Shader
@group Attrib
*/
sSI YAC_CALL sdvg_GetAttribLocation (const char *_name);

/* @function sdvg_Uniform1f,String name,float f
Set scalar float uniform in currently bound shader program

@groupref Shader
@group Uniform
*/
void YAC_CALL sdvg_Uniform1f (const char *_name, sF32 _f);

/* @function sdvg_Uniform2f,String name,float f1,float f2
Set vec2 float uniform in currently bound shader program

@groupref Shader
@group Uniform
*/
void YAC_CALL sdvg_Uniform2f (const char *_name, sF32 _f1, sF32 _f2);

/* @function sdvg_Uniform3f,String name,float f1,float f2,float f3
Set vec3 float uniform in currently bound shader program

@groupref Shader
@group Uniform
*/
void YAC_CALL sdvg_Uniform3f (const char *_name, sF32 _f1, sF32 _f2, sF32 _f3);

/* @function sdvg_Uniform4f,String name,float f1,float f2,float f3,float f4
Set vec4 float uniform in currently bound shader program

@groupref Shader
@group Uniform
*/
void YAC_CALL sdvg_Uniform4f (const char *_name, sF32 _f1, sF32 _f2, sF32 _f3, sF32 _f4);

/* @function sdvg_Uniform1i,String name,int i
Set integer / sampler uniform in currently bound shader program

@groupref Shader
@group Uniform
*/
void YAC_CALL sdvg_Uniform1i (const char *_name, sSI _i);
#ifdef SHADERVG_SCRIPT_API
YF sUI YAC_CALL _sdvg_CreateShader (YAC_String *vs, YAC_String *fs);
YF sSI YAC_CALL _sdvg_GetUniformLocation (YAC_String *_name);
YF sSI YAC_CALL _sdvg_GetAttribLocation (YAC_String *_name);
/* @function sdvg_UniformMatrix4,String name,Matrix4f m
Set mat4 uniform in currently bound shader program

@groupref Shader
@group Uniform
*/
YF void YAC_CALL _sdvg_UniformMatrix4 (YAC_String *_name, YAC_Object *_matRowMajor);
YF void YAC_CALL _sdvg_Uniform1f (YAC_String *_name, sF32 _f);
YF void YAC_CALL _sdvg_Uniform2f (YAC_String *_name, sF32 _f1, sF32 _f2);
YF void YAC_CALL _sdvg_Uniform3f (YAC_String *_name, sF32 _f1, sF32 _f2, sF32 _f3);
YF void YAC_CALL _sdvg_Uniform4f (YAC_String *_name, sF32 _f1, sF32 _f2, sF32 _f3, sF32 _f4);
YF void YAC_CALL _sdvg_Uniform1i (YAC_String *_name, sSI _i);
#endif // SHADERVG_SCRIPT_API

// -------- (high level) draw functions --------

/* @function sdvg_PaintSolid
Select solid paint

@group Paint
*/
YF void YAC_CALL sdvg_PaintSolid (void);

/* @function sdvg_PaintLinear,float startX,float startY,float endX,float endY
Select linear paint.

The currently bound texture (nx1) is used as a gradient lookup table.

@group Paint
@group Gradient
@groupref Texture
*/
YF void YAC_CALL sdvg_PaintLinear (sF32 _startX, sF32 _startY, sF32 _endX, sF32 _endY);

/* @function sdvg_PaintRadial,float startX,float startY,float radiusX,float radiusY
Select radial paint.

The currently bound texture (nx1) is used as a gradient lookup table.

@arg startX Focal point X
@arg startY Focal point Y
@arg radiusX Horizontal radius
@arg radiusY Vertical radius

@group Paint
@group Gradient
@groupref Texture
*/
YF void YAC_CALL sdvg_PaintRadial (sF32 _startX, sF32 _startY, sF32 _radiusX, sF32 _radiusY);

/* @function sdvg_PaintConic,float startX,float startY,float radiusX,float radiusY,float angle01
Select conic paint.

The currently bound texture (nx1) is used as a gradient lookup table.

@arg startX Focal point X
@arg startY Focal point Y
@arg radiusX Horizontal radius
@arg radiusY Vertical radius
@arg angle01 Normalized start angle (0..1 => 0..360 degrees). 0=north

@group Paint
@group Gradient
@groupref Texture
*/
YF void YAC_CALL sdvg_PaintConic (sF32 _startX, sF32 _startY, sF32 _radiusX, sF32 _radiusY, sF32 _angle01);

/* @function sdvg_PaintPattern,float startX,float startY,float endX,float endY,float sizeX,float sizeY
Select pattern paint.

The currently bound texture is used as the pattern.

@arg startX Start point X
@arg startY Start point Y
@arg endX End point X
@arg endY End point Y
@arg sizeX Pattern size X
@arg sizeY Pattern size Y

@group Paint
@group Pattern
@groupref Texture
*/
YF void YAC_CALL sdvg_PaintPattern (sF32 _startX, sF32 _startY, sF32 _endX, sF32 _endY, sF32 _sizeX, sF32 _sizeY);

/* @function sdvg_PaintPatternAlpha,float startX,float startY,float endX,float endY,float sizeX,float sizeY
Select alpha pattern paint.

The currently bound texture is used as the alpha channel pattern.

@arg startX Start point X
@arg startY Start point Y
@arg endX End point X
@arg endY End point Y
@arg sizeX Pattern size X
@arg sizeY Pattern size Y

@group Paint
@group Pattern
@groupref Texture
*/
YF void YAC_CALL sdvg_PaintPatternAlpha (sF32 _startX, sF32 _startY, sF32 _endX, sF32 _endY, sF32 _sizeX, sF32 _sizeY);

/* @function sdvg_PaintPatternDecal,float startX,float startY,float endX,float endY,float sizeX,float sizeY
Select pattern paint.

The currently bound texture is used as the pattern.

@arg startX Start point X
@arg startY Start point Y
@arg endX End point X
@arg endY End point Y
@arg sizeX Pattern size X
@arg sizeY Pattern size Y

@group Paint
@group Pattern
@groupref Texture
@group Decal
*/
YF void YAC_CALL sdvg_PaintPatternDecal (sF32 _startX, sF32 _startY, sF32 _endX, sF32 _endY, sF32 _sizeX, sF32 _sizeY);

/* @function sdvg_PaintPatternDecalAlpha,float startX,float startY,float endX,float endY,float sizeX,float sizeY
Select alpha pattern paint.

The currently bound texture is used as the alpha channel pattern.

@arg startX Start point X
@arg startY Start point Y
@arg endX End point X
@arg endY End point Y
@arg sizeX Pattern size X
@arg sizeY Pattern size Y

@group Paint
@group Pattern
@groupref Texture
@group Decal
*/
YF void YAC_CALL sdvg_PaintPatternDecalAlpha (sF32 _startX, sF32 _startY, sF32 _endX, sF32 _endY, sF32 _sizeX, sF32 _sizeY);

/* @function sdvg_BeginVBO,int numVertices,int stride:boolean
Begin preparation of mapped vertex buffer

@arg numVertices Number of vertices
@arg stride Total number of attribute bytes per vertex

@group Begin
@group VBO
*/
YF sBool YAC_CALL sdvg_BeginVBO (sUI _numVertices, sUI _stride);

/* @function sdvg_BeginTriangles,int numVertices,int stride:boolean
Begin preparation or rendering of triangles via user-defined shader

@arg numVertices Number of vertices
@arg stride Total number of attribute bytes per vertex

@group Begin
@group Triangle
*/
YF sBool YAC_CALL sdvg_BeginTriangles (sUI _numVertices, sUI _stride);

/* @function sdvg_BeginTriangleFan,int numVertices,int stride:boolean
Begin preparation or rendering of triangle-fan via user-defined shader

@arg numVertices Number of vertices
@arg stride Total number of attribute bytes per vertex

@group Begin
@group Triangle
*/
YF sBool YAC_CALL sdvg_BeginTriangleFan (sUI _numVertices, sUI _stride);

/* @function sdvg_BeginTriangleStrip,int numVertices,int stride:boolean
Begin preparation or rendering of triangle-strip via user-defined shader

@arg numVertices Number of vertices
@arg stride Total number of attribute bytes per vertex

@group Begin
@group Triangle
*/
YF sBool YAC_CALL sdvg_BeginTriangleStrip (sUI _numVertices, sUI _stride);

/* @function sdvg_BeginFilledTriangles,int numVertices:boolean
Begin preparation or rendering of filled triangles

@arg numVertices Number of vertices

@group Begin
@group Triangle
@groupref Fill
*/
YF sBool YAC_CALL sdvg_BeginFilledTriangles (sUI _numVertices);

/* @function sdvg_BeginFilledTriangleFan,int numVertices:boolean
Begin preparation or rendering of filled triangle-fan

@arg numVertices Number of vertices

@group Begin
@group Triangle
@groupref Fill
*/
YF sBool YAC_CALL sdvg_BeginFilledTriangleFan (sUI _numVertices);

/* @function sdvg_BeginFilledTriangleStrip,int numVertices:boolean
Begin preparation or rendering of filled triangle-strip

@arg numVertices Number of vertices

@group Begin
@group Triangle
@groupref Fill
*/
YF sBool YAC_CALL sdvg_BeginFilledTriangleStrip (sUI _numVertices);

/* @function sdvg_BeginFilledGouraudTriangles,int numVertices:boolean
Begin preparation or rendering of filled, gouraud shaded triangles

@arg numVertices Number of vertices

@group Begin
@group Triangle
@groupref Fill
@group Gouraud
*/
YF sBool YAC_CALL sdvg_BeginFilledGouraudTriangles (sUI _numVertices);

/* @function sdvg_BeginFilledGouraudTriangleFan,int numVertices:boolean
Begin preparation or rendering of filled, gouraud shaded triangle-fan

@arg numVertices Number of vertices

@group Begin
@group Triangle
@groupref Fill
@group Gouraud
*/
YF sBool YAC_CALL sdvg_BeginFilledGouraudTriangleFan (sUI _numVertices);

/* @function sdvg_BeginFilledGouraudTriangleStrip,int numVertices:boolean
Begin preparation or rendering of filled, gouraud shaded triangle-strip

@arg numVertices Number of vertices

@group Begin
@group Triangle
@groupref Fill
@group Gouraud
*/
YF sBool YAC_CALL sdvg_BeginFilledGouraudTriangleStrip (sUI _numVertices);

/* @function sdvg_BeginTexturedTriangles,int numVertices:boolean
Begin preparation or rendering of textured triangles

@arg numVertices Number of vertices

@group Begin
@group Triangle
@groupref Fill
@groupref Texture
*/
YF sBool YAC_CALL sdvg_BeginTexturedTriangles (sUI _numVertices);

/* @function sdvg_BeginTexturedTriangleFan,int numVertices:boolean
Begin preparation or rendering of textured triangle fan

@arg numVertices Number of vertices

@group Begin
@group Triangle
@groupref Fill
@groupref Texture
*/
YF sBool YAC_CALL sdvg_BeginTexturedTriangleFan (sUI _numVertices);

/* @function sdvg_BeginTexturedTriangleStrip,int numVertices:boolean
Begin preparation or rendering of textured triangle strip

@arg numVertices Number of vertices

@group Begin
@group Triangle
@groupref Fill
@groupref Texture
*/
YF sBool YAC_CALL sdvg_BeginTexturedTriangleStrip (sUI _numVertices);

/* @function sdvg_BeginTexturedGouraudTriangles,int numVertices:boolean
Begin preparation or rendering of textured, gouraud shaded triangles

@arg numVertices Number of vertices

@group Begin
@group Triangle
@groupref Fill
@groupref Texture
@group Gouraud
*/
YF sBool YAC_CALL sdvg_BeginTexturedGouraudTriangles (sUI _numVertices);

/* @function sdvg_BeginTexturedGouraudTriangleFan,int numVertices:boolean
Begin preparation or rendering of textured, gouraud shaded triangle-fan

@arg numVertices Number of vertices

@group Begin
@group Triangle
@groupref Fill
@groupref Texture
@group Gouraud
*/
YF sBool YAC_CALL sdvg_BeginTexturedGouraudTriangleFan (sUI _numVertices);

/* @function sdvg_BeginTexturedGouraudTriangleStrip,int numVertices:boolean
Begin preparation or rendering of textured, gouraud shaded triangle-strip

@arg numVertices Number of vertices

@group Begin
@group Triangle
@groupref Fill
@groupref Texture
@group Gouraud
*/
YF sBool YAC_CALL sdvg_BeginTexturedGouraudTriangleStrip (sUI _numVertices);

/* @function sdvg_BeginTexturedTrianglesAlpha,int numVertices:boolean
Begin preparation or rendering of alpha-channel-only textured triangles

@arg numVertices Number of vertices

@group Begin
@group Triangle
@groupref Fill
@groupref Texture
*/
YF sBool YAC_CALL sdvg_BeginTexturedTrianglesAlpha (sUI _numVertices);

/* @function sdvg_BeginTexturedTriangleFanAlpha,int numVertices:boolean
Begin preparation or rendering of alpha-channel-only textured triangle-fan

@arg numVertices Number of vertices

@group Begin
@group Triangle
@groupref Fill
@groupref Texture
*/
YF sBool YAC_CALL sdvg_BeginTexturedTriangleFanAlpha (sUI _numVertices);

/* @function sdvg_BeginTexturedTriangleStripAlpha,int numVertices:boolean
Begin preparation or rendering of alpha-channel-only textured triangle-strip

@arg numVertices Number of vertices

@group Begin
@group Triangle
@groupref Fill
@groupref Texture
*/
YF sBool YAC_CALL sdvg_BeginTexturedTriangleStripAlpha (sUI _numVertices);

/* @function sdvg_BeginTexturedGouraudTrianglesAlpha,int numVertices:boolean
Begin preparation or rendering of alpha-channel-only textured, gouraud shaded triangles

@arg numVertices Number of vertices

@group Begin
@group Triangle
@groupref Fill
@groupref Texture
@group Gouraud
*/
YF sBool YAC_CALL sdvg_BeginTexturedGouraudTrianglesAlpha (sUI _numVertices);

/* @function sdvg_BeginTexturedGouraudTriangleFanAlpha,int numVertices:boolean
Begin preparation or rendering of alpha-channel-only textured, gouraud shaded triangle-fan

@arg numVertices Number of vertices

@group Begin
@group Triangle
@groupref Fill
@groupref Texture
@group Gouraud
*/
YF sBool YAC_CALL sdvg_BeginTexturedGouraudTriangleFanAlpha (sUI _numVertices);

/* @function sdvg_BeginTexturedGouraudTriangleStripAlpha,int numVertices:boolean
Begin preparation or rendering of alpha-channel-only textured, gouraud shaded triangle-strip

@arg numVertices Number of vertices

@group Begin
@group Triangle
@groupref Fill
@groupref Texture
@group Gouraud
*/
YF sBool YAC_CALL sdvg_BeginTexturedGouraudTriangleStripAlpha (sUI _numVertices);

/* @function sdvg_BeginTexturedTrianglesAlphaSDF,int numVertices:boolean
Begin preparation or rendering of alpha-SDF-channel-only textured triangles

@arg numVertices Number of vertices

@group Begin
@group Triangle
@groupref Fill
@groupref Texture
@group SDF
*/
YF sBool YAC_CALL sdvg_BeginTexturedTrianglesAlphaSDF (sUI _numVertices);

/* @function sdvg_BeginLineStrip,int numPoints:boolean
Begin preparation or rendering of line strip

@arg numPoints Number of vertices

@group Begin
@group Line
@groupref Stroke
*/
YF sBool YAC_CALL sdvg_BeginLineStrip (sUI _numPoints);

/* @function sdvg_BeginLineStripAA,int numPoints:boolean
Begin preparation or rendering of anti-aliased line strip

@arg numPoints Number of points

@group Begin
@group Line
@groupref Stroke
*/
YF sBool YAC_CALL sdvg_BeginLineStripAA (sUI _numPoints);

/* @function sdvg_BeginLineStripPattern,int numPoints:boolean
Begin preparation or rendering of patterned line strip.

@arg numPoints Number of vertices

@group Begin
@group Line
@groupref Stroke
@groupref LinePattern
@groupref Texture
*/
YF sBool YAC_CALL sdvg_BeginLineStripPattern (sUI _numPoints);

/* @function sdvg_BeginLineStripPatternAA,int numPoints:boolean
Begin preparation or rendering of anti-aliased, patterned line strip.

@arg numPoints Number of points

@group Begin
@group Line
@groupref Stroke
@groupref LinePattern
@groupref Texture
*/
YF sBool YAC_CALL sdvg_BeginLineStripPatternAA (sUI _numPoints);

/* @function sdvg_BeginLineStripPatternDecal,int numPoints:boolean
Begin preparation or rendering of patterned line strip.

@arg numPoints Number of vertices

@group Begin
@group Line
@groupref Stroke
@groupref LinePattern
@groupref Texture
@groupref Fill
@group Decal
*/
YF sBool YAC_CALL sdvg_BeginLineStripPatternDecal (sUI _numPoints);

/* @function sdvg_BeginLineStripPatternDecalAA,int numPoints:boolean
Begin preparation or rendering of anti-aliased, patterned line strip.

@arg numPoints Number of points

@group Begin
@group Line
@groupref Stroke
@groupref LinePattern
@groupref Texture
@groupref Fill
@group Decal
*/
YF sBool YAC_CALL sdvg_BeginLineStripPatternDecalAA (sUI _numPoints);

/* @function sdvg_BeginLineStripBevel,int numPoints:boolean
Begin preparation or rendering of line strip with bevel line joints (open polyline).

One extra point must be added after the last control point (will be ignored for open polylines, though).
Skips line joint after last line segment.

@arg numPoints Number of points ((numPoints-2) line segments will be drawn)

@group Begin
@group Line
@groupref Stroke
@group LineJoint
*/
YF sBool YAC_CALL sdvg_BeginLineStripBevel (sUI _numPoints);

/* @function sdvg_BeginLineStripBevelClosed,int numPoints:boolean
Begin preparation or rendering of line strip with bevel line joints (closed polyline).

The last two points must equal the first two.
Renders line joint after last segment.

@arg numPoints Number of points ((numPoints-2) line segments will be drawn)

@group Begin
@group Line
@groupref Stroke
@group LineJoint
*/
YF sBool YAC_CALL sdvg_BeginLineStripBevelClosed (sUI _numPoints);

/* @function sdvg_BeginLineStripBevelAA,int numPoints:boolean
Begin preparation or rendering of anti-aliased line strip with bevel line joints (open polyline).

One extra point must be added after the last control point (will be ignored for open polylines, though).
Skips line joint after last line segment.

@arg numPoints Number of points ((numPoints-2) line segments will be drawn)

@group Begin
@group Line
@groupref Stroke
@group LineJoint
*/
YF sBool YAC_CALL sdvg_BeginLineStripBevelAA (sUI _numPoints);

/* @function sdvg_BeginLineStripBevelAAClosed,int numPoints:boolean
Begin preparation or rendering of anti-aliased line strip with bevel line joints (closed polyline).

The last two points must equal the first two.
Renders line joint after last segment.

@arg numPoints Number of points ((numPoints-2) line segments will be drawn)

@group Begin
@group Line
@groupref Stroke
@group LineJoint
*/
YF sBool YAC_CALL sdvg_BeginLineStripBevelAAClosed (sUI _numPoints);

/* @function sdvg_BeginLineStripPatternBevel,int numPoints:boolean
Begin preparation or rendering of patterned line strip with bevel line joints (open polyline).

One extra point must be added after the last control point (will be ignored for open polylines, though).
Skips line joint after last line segment.

@arg numPoints Number of vertices ((numPoints-2) line segments will be drawn)

@group Begin
@group Line
@groupref Stroke
@group LineJoint
@groupref LinePattern
@groupref Texture
*/
YF sBool YAC_CALL sdvg_BeginLineStripPatternBevel (sUI _numPoints);

/* @function sdvg_BeginLineStripPatternBevelClosed,int numPoints:boolean
Begin preparation or rendering of patterned line strip with bevel line joints (closed polyline)

The last two points must equal the first two.
Renders line joint after last segment.

@arg numPoints Number of vertices ((numPoints-2) line segments will be drawn)

@group Begin
@group Line
@groupref Stroke
@group LineJoint
@groupref LinePattern
@groupref Texture
*/
YF sBool YAC_CALL sdvg_BeginLineStripPatternBevelClosed (sUI _numPoints);

/* @function sdvg_BeginLineStripPatternBevelAA,int numPoints:boolean
Begin preparation or rendering of anti-aliased, patterned line strip with bevel line joints (open polyline).

One extra point must be added after the last control point (will be ignored for open polylines, though).
Skips line joint after last line segment.

@arg numPoints Number of points ((numPoints-2) line segments will be drawn)

@group Begin
@group Line
@groupref Stroke
@group LineJoint
@groupref LinePattern
@groupref Texture
*/
YF sBool YAC_CALL sdvg_BeginLineStripPatternBevelAA (sUI _numPoints);

/* @function sdvg_BeginLineStripPatternBevelAAClosed,int numPoints:boolean
Begin preparation or rendering of anti-aliased, patterned line strip with bevel line joints (closed polyline).

The last two points must equal the first two.
Renders line joint after last segment.

@arg numPoints Number of points ((numPoints-2) line segments will be drawn)

@group Begin
@group Line
@groupref Stroke
@group LineJoint
@groupref LinePattern
@groupref Texture
*/
YF sBool YAC_CALL sdvg_BeginLineStripPatternBevelAAClosed (sUI _numPoints);

/* @function sdvg_BeginLineStripPatternDecalBevel,int numPoints:boolean
Begin preparation or rendering of patterned line strip with bevel line joints (open polyline).

One extra point must be added after the last control point (will be ignored for open polylines, though).
Skips line joint after last line segment.

@arg numPoints Number of vertices

@group Begin
@group Line
@groupref Stroke
@group LineJoint
@groupref LinePattern
@groupref Texture
@groupref Fill
@group Decal
*/
YF sBool YAC_CALL sdvg_BeginLineStripPatternDecalBevel (sUI _numPoints);

/* @function sdvg_BeginLineStripPatternDecalBevelClosed,int numPoints:boolean
Begin preparation or rendering of patterned line strip with bevel line joints (closed polyline).

The last two points must equal the first two.
Renders line joint after last segment.

@arg numPoints Number of vertices ((numPoints-2) line segments will be drawn)

@group Begin
@group Line
@groupref Stroke
@group LineJoint
@groupref LinePattern
@groupref Texture
@groupref Fill
@group Decal
*/
YF sBool YAC_CALL sdvg_BeginLineStripPatternDecalBevelClosed (sUI _numPoints);

/* @function sdvg_BeginLineStripPatternDecalBevelAA,int numPoints:boolean
Begin preparation or rendering of anti-aliased, patterned line strip with bevel line joints (open polyline).

One extra point must be added after the last control point (will be ignored for open polylines, though).
Skips line joint after last line segment.

@arg numPoints Number of points ((numPoints-2) line segments will be drawn)

@group Begin
@group Line
@groupref Stroke
@group LineJoint
@groupref LinePattern
@groupref Texture
@groupref Fill
@group Decal
*/
YF sBool YAC_CALL sdvg_BeginLineStripPatternDecalBevelAA (sUI _numPoints);

/* @function sdvg_BeginLineStripPatternDecalBevelAAClosed,int numPoints:boolean
Begin preparation or rendering of anti-aliased, patterned line strip with bevel line joints (closed polyline).

The last two points must equal the first two.
Renders line joint after last segment.

@arg numPoints Number of points ((numPoints-2) line segments will be drawn)

@group Begin
@group Line
@groupref Stroke
@group LineJoint
@groupref LinePattern
@groupref Texture
@groupref Fill
@group Decal
*/
YF sBool YAC_CALL sdvg_BeginLineStripPatternDecalBevelAAClosed (sUI _numPoints);

/* @function sdvg_BeginLineStripMiter,int numPoints:boolean
Begin preparation or rendering of line strip with miter line joints

@arg numPoints Number of points

@group Begin
@group Line
@groupref Stroke
@group LineJoint
*/
YF sBool YAC_CALL sdvg_BeginLineStripMiter(sUI _numPoints);

/* @function sdvg_BeginLineStripMiterAA,int numPoints:boolean
Begin preparation or rendering of anti-aliased line strip with miter line joints

@arg numPoints Number of points

@group Begin
@group Line
@groupref Stroke
@group LineJoint
*/
YF sBool YAC_CALL sdvg_BeginLineStripMiterAA(sUI _numPoints);

/* @function sdvg_BeginLines,int numPoints:boolean
Begin preparation or rendering of line segments

@arg numPoints Number of points

@group Begin
@group Line
@groupref Stroke
*/
YF sBool YAC_CALL sdvg_BeginLines (sUI _numPoints);

/* @function sdvg_BeginLinesAA,int numPoints:boolean
Begin preparation or rendering of anti-aliased line segments

@arg numPoints Number of points

@group Begin
@group Line
@groupref Stroke
*/
YF sBool YAC_CALL sdvg_BeginLinesAA (sUI _numPoints);

/* @function sdvg_BeginLinesGouraud,int numPoints:boolean
Begin preparation or rendering of gouraud-shaded line segments

@arg numPoints Number of points

@group Begin
@group Line
@groupref Stroke
@group Gouraud
*/
YF sBool YAC_CALL sdvg_BeginLinesGouraud (sUI _numPoints);

/* @function sdvg_BeginLinesGouraudAA,int numPoints:boolean
Begin preparation or rendering of anti-aliased, gouraud-shaded line segments

@arg numPoints Number of points

@group Begin
@group Line
@groupref Stroke
@group Gouraud
*/
YF sBool YAC_CALL sdvg_BeginLinesGouraudAA (sUI _numPoints);

/* @function sdvg_BeginLinesPattern,int numPoints:boolean
Begin preparation or rendering of patterned line segments

@arg numPoints Number of points

@group Begin
@group Line
@groupref Stroke
@groupref LinePattern
@groupref Texture
*/
YF sBool YAC_CALL sdvg_BeginLinesPattern (sUI _numPoints);

/* @function sdvg_BeginLinesPatternAA,int numPoints:boolean
Begin preparation or rendering of anti-aliased, patterned line segments

@arg numPoints Number of points

@group Begin
@group Line
@groupref LinePattern
@groupref Texture
*/
YF sBool YAC_CALL sdvg_BeginLinesPatternAA (sUI _numPoints);

/* @function sdvg_BeginPointsSquare,int numPoints:boolean
Begin preparation or rendering of square points

@arg numPoints Number of points

@group Begin
@group Point
*/
YF sBool YAC_CALL sdvg_BeginPointsSquare (sUI _numPoints);

/* @function sdvg_BeginPointsSquareAA,int numPoints:boolean
Begin preparation or rendering of anti-aliased, square points

@arg numPoints Number of points

@group Begin
@group Point
*/
YF sBool YAC_CALL sdvg_BeginPointsSquareAA (sUI _numPoints);

/* @function sdvg_BeginPointsSquareGouraud,int numPoints:boolean
Begin preparation or rendering of gouraud-shaded square points

@arg numPoints Number of points

@group Begin
@group Point
@group Gouraud
*/
YF sBool YAC_CALL sdvg_BeginPointsSquareGouraud (sUI _numPoints);

/* @function sdvg_BeginPointsSquareGouraudAA,int numPoints:boolean
Begin preparation or rendering of anti-aliased, gouraud-shaded square points

@arg numPoints Number of points

@group Begin
@group Point
@group Gouraud
*/
YF sBool YAC_CALL sdvg_BeginPointsSquareGouraudAA (sUI _numPoints);

/* @function sdvg_BeginPointsRound,int numPoints:boolean
Begin preparation or rendering of round points

@arg numPoints Number of points

@group Begin
@group Point
*/
YF sBool YAC_CALL sdvg_BeginPointsRound (sUI _numPoints);

/* @function sdvg_BeginPointsRoundAA,int numPoints:boolean
Begin preparation or rendering of anti-aliased, round points

@arg numPoints Number of points

@group Begin
@group Point
*/
YF sBool YAC_CALL sdvg_BeginPointsRoundAA (sUI _numPoints);

/* @function sdvg_BeginPointsRoundGouraud,int numPoints:boolean
Begin preparation or rendering of gouraud-shaded round points

@arg numPoints Number of points

@group Begin
@group Point
@group Gouraud
*/
YF sBool YAC_CALL sdvg_BeginPointsRoundGouraud (sUI _numPoints);

/* @function sdvg_BeginPointsRoundGouraudAA,int numPoints:boolean
Begin preparation or rendering of anti-aliased, gouraud-shaded round points

@arg numPoints Number of points

@group Begin
@group Point
@group Gouraud
*/
YF sBool YAC_CALL sdvg_BeginPointsRoundGouraudAA (sUI _numPoints);

/* @function sdvg_BeginPolygon,int numVertices,int stride:boolean
Begin preparation or rendering of polygon via user-defined shader

@arg numVertices Number of vertices
@arg stride Total number of attribute bytes per vertex

@group Begin
@group Polygon
*/
YF sBool YAC_CALL sdvg_BeginPolygon (sUI _numVertices, sUI _stride);

/* @function sdvg_BeginPolygonAA,int numVertices,int stride:boolean
Begin preparation or rendering of anti-aliased polygon via user-defined shader

@arg numVertices Number of vertices
@arg stride Total number of attribute bytes per vertex

@group Begin
@group Polygon
*/
YF sBool YAC_CALL sdvg_BeginPolygonAA (sUI _numVertices, sUI _stride);

/* @function sdvg_BeginFilledPolygon,int numVertices:boolean
Begin preparation or rendering of filled polygon

@arg numVertices Number of vertices

@group Begin
@group Polygon
@groupref Fill
*/
YF sBool YAC_CALL sdvg_BeginFilledPolygon (sUI _numVertices);

/* @function sdvg_BeginFilledPolygonAA,int numVertices:boolean
Begin preparation or rendering of filled, anti-aliased polygon

@arg numVertices Number of vertices

@group Begin
@group Polygon
@groupref Fill
*/
YF sBool YAC_CALL sdvg_BeginFilledPolygonAA (sUI _numVertices);

/* @function sdvg_VertexOffset2f
Set vertex coordinate buffer GPU read pointer to current write offset

@groupref Shader
@groupref Attrib
@group AttribOffset
*/
YF void YAC_CALL sdvg_VertexOffset2f (void);

/* @function sdvg_VertexOffset2fi16
Set vertex coordinate buffer GPU read pointer to current write offset (14.2 fixed point format)

@groupref Shader
@groupref Attrib
@group AttribOffset
*/
YF void YAC_CALL sdvg_VertexOffset2fi16 (void);

/* @function sdvg_AttribOffsetf,String name,int size
Set vertex attribute buffer GPU read pointer to current write offset (n floats)

@arg name Attribute name

@groupref Shader
@groupref Attrib
@group AttribOffset
*/
void YAC_CALL sdvg_AttribOffsetf (const char *_name, sUI _size);

/* @function sdvg_AttribOffset1f,String name
Set vertex attribute buffer GPU read pointer to current write offset (1 float per vertex)

@arg name Attribute name

@groupref Shader
@groupref Attrib
@group AttribOffset
*/
void YAC_CALL sdvg_AttribOffset1f (const char *_name);

/* @function sdvg_AttribOffset2f,String name
Set vertex attribute buffer GPU read pointer to current write offset (2 floats per vertex)

@arg name Attribute name

@groupref Shader
@groupref Attrib
@group AttribOffset
*/
void YAC_CALL sdvg_AttribOffset2f (const char *_name);

/* @function sdvg_AttribOffset3f,String name
Set vertex attribute buffer GPU read pointer to current write offset (3 floats per vertex)

@arg name Attribute name

@groupref Shader
@groupref Attrib
@group AttribOffset
*/
void YAC_CALL sdvg_AttribOffset3f (const char *_name);

/* @function sdvg_AttribOffset4f,String name
Set vertex attribute buffer GPU read pointer to current write offset (4 floats per vertex)

@arg name Attribute name

@groupref Shader
@groupref Attrib
@group AttribOffset
*/
void YAC_CALL sdvg_AttribOffset4f (const char *_name);

/* @function sdvg_AttribOffsetARGB,String name
Set vertex attribute buffer GPU read pointer to current write offset (4 r,g,b,a bytes per vertex)

@arg name Attribute name

@groupref Shader
@groupref Attrib
@group AttribOffset
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
Emit signed 16bit short vertex attribute to currently mapped vertex buffer

@arg name Attribute name

@groupref Shader
@group Attrib
*/
void YAC_CALL sdvg_Attribi16 (sS16 _i);

/* @function sdvg_Attrib2fi16,short i
Convert from float and emit two signed 16bit short vertex attributes to currently mapped vertex buffer

@arg name Attribute name

@groupref Shader
@group Attrib
*/
void YAC_CALL sdvg_Attrib2fi16 (sF32 _x, sF32 _y);

/* @function sdvg_Attrib1f,float f
Emit 32bit float vertex attribute to currently mapped vertex buffer

@groupref Shader
@group Attrib
*/
YF void YAC_CALL sdvg_Attrib1f (sF32 _f);

/* @function sdvg_Attrib2f,float f1,float f2
Emit two 32bit float vertex attributes to currently mapped vertex attribute buffer

@groupref Shader
@group Attrib
*/
YF void YAC_CALL sdvg_Attrib2f (sF32 _f1, sF32 _f2);

/* @function sdvg_Vertex2f,float x,float y
Emit 2-component (xy) 32bit float vertex coordinate to currently mapped vertex buffer

@groupref Shader
@group Attrib
*/
YF void YAC_CALL sdvg_Vertex2f (sF32 _x, sF32 _y);

/* @function sdvg_Vertex3f,float x,float y,float z
Emit 3-component (xyz) 32bit float vertex coordinate to currently mapped vertex buffer

@groupref Shader
@group Attrib
*/
YF void YAC_CALL sdvg_Vertex3f (sF32 _x, sF32 _y, sF32 _z);

/* @function sdvg_TexCoord2f,float u,float v
Emit 2-component 32bit float UV coordinate to currently mapped vertex buffer

@groupref Shader
@group Attrib
*/
YF void YAC_CALL sdvg_TexCoord2f (sF32 _u, sF32 _v);

/* @function sdvg_Attrib3f,float f1,float f2,float f3
Emit 3-component 32bit float vertex attribute to currently mapped vertex buffer

@groupref Shader
@group Attrib
*/
YF void YAC_CALL sdvg_Attrib3f (sF32 _f1, sF32 _f2, sF32 _f3);

/* @function sdvg_Color3f,float r,float g,float b
Emit 3-component 32bit float RGB color attribute to currently mapped vertex buffer

@groupref Shader
@group Attrib
*/
YF void YAC_CALL sdvg_Color3f (sF32 _r, sF32 _g, sF32 _b);

/* @function sdvg_Attrib4f,float f1,float f2,float f3,float f4
Emit 4-component 32bit float vertex attribute to currently mapped vertex buffer

@groupref Shader
@group Attrib
*/
YF void YAC_CALL sdvg_Attrib4f (sF32 _f1, sF32 _f2, sF32 _f3, sF32 _f4);

/* @function sdvg_Color4f,float r,float g,float b,float a
Emit 4-component 32bit float RGBA color attribute to currently mapped vertex buffer

@groupref Shader
@group Attrib
*/
YF void YAC_CALL sdvg_Color4f (sF32 _r, sF32 _g, sF32 _b, sF32 _a);

/* @function sdvg_AttribARGB,int c32
Emit 4-component unsigned byte r,g,b,a color attribute to currently mapped vertex buffer

@groupref Shader
@group Attrib
*/
YF void YAC_CALL sdvg_AttribARGB (sUI _c32);

/* @function sdvg_ColorARGB,int c32
Emit 4-component unsigned byte r,g,b,a color attribute to currently mapped vertex buffer

@groupref Shader
@group Attrib
*/
YF void YAC_CALL sdvg_ColorARGB (sUI _c32);

/* @function sdvg_End
Finalize vertex buffer and start rendering (unless buffer is currently mapped).

@group Begin
*/
YF void YAC_CALL sdvg_End (void);

// ----------- additional (scratch) draw functions ------------
/* @function sdvg_DrawFilledRectangle,float x,float y,float w,float h
Render a filled rectangle via scratch buffer

@group Rect
@groupref Fill
*/
YF void YAC_CALL sdvg_DrawFilledRectangle (sF32 _x, sF32 _y, sF32 _w, sF32 _h);

/* @function sdvg_DrawRectangle,float x,float y,float w,float h,float b
Render a stroked rectangle via scratch buffer

@arg x X position
@arg y Y position
@arg w Width
@arg h Height
@arg b Border width

@group Rect
@groupref Stroke
*/
YF void YAC_CALL sdvg_DrawRectangle (sF32 _x, sF32 _y, sF32 _w, sF32 _h, sF32 _b);

// ----------- color utilities ------------
/* @function sdvg_ARGB,byte a,byte r,byte g,byte b:int
Compose packed ARGB32 color from a,r,g,b bytes

@arg a Alpha (0..255)
@arg r Red (0..255)
@arg g Green (0..255)
@arg b Blue (0..255)

@return Packed ARGB32 color

@group Color
*/
sU32 sdvg_ARGB (sU8 _a, sU8 _r, sU8 _g, sU8 _b);

/* @function sdvg_ARGBf,float a,float r,float g,float b:int
Compose packed ARGB32 color from normalized a,r,g,b floats (0..1)

@arg a Normalized alpha (0..1)
@arg r Normalized red (0..1)
@arg g Normalized green (0..1)
@arg b Normalized blue (0..1)

@return Packed ARGB32 color

@group Color
*/
YF sU32 YAC_CALL sdvg_ARGBf (sF32 _a, sF32 _r, sF32 _g, sF32 _b);

/* @function sdvg_MixARGBx,int x,int y,short t:int
Mix packed ARGB32 colors (fixed point weight)

@arg x First packed ARGB32 color
@arg y Second packed ARGB32 color
@arg t 0=x .. 256=y
@return Packed RGB24 color

@group Color
*/
sU32 sdvg_MixARGBx (sU32 _x, sU32 _y, sU16 _t);

/* @function sdvg_MixARGBf,int x,int y,float t:int
Mix packed ARGB32 colors (normalized float weight)

@arg x First packed ARGB32 color
@arg y Second packed ARGB32 color
@arg t 0.0=x .. 1.0=y

@return Packed RGB24 color

@group Color
*/
YF sU32 YAC_CALL sdvg_MixARGBf (sU32 _x, sU32 _y, sF32 _t);

/* @function sdvg_MixRGBx,int x,int y,short t
Mix packed RGB24 colors (fixed point weight)

@arg x First packed RGB24 color
@arg y Second packed RGB24 color
@arg t 0=x .. 256=y

@return Packed RGB24 color

@group Color
*/
sU32 sdvg_MixRGBx (sU32 _x, sU32 _y, sU16 _t);

/* @function sdvg_MixRGBf,int x,int y,float t:int
Mix packed RGB24 colors (normalized float weight)

@arg x First packed RGB24 color
@arg y Second packed RGB24 color
@arg t 0.0=x .. 1.0=y

@return Packed RGB24 color

@group Color
*/
YF sU32 YAC_CALL sdvg_MixRGBf (sU32 _x, sU32 _y, sF32 _t);

/* @function sdvg_TintARGB,int x,int y:int
Tint color

@arg x Packed ARGB32 base color
@arg y Packed ARGB32 tint color. Alpha channel determines blend amount.

@return Packed ARGB32 color

@group Color
*/
YF sU32 YAC_CALL sdvg_TintARGB (sU32 _x, sU32 _y);

/* @function sdvg_TintRGBAlpha,int x,int y,byte a8:int
Tint color

@arg x Packed RGB24 base color
@arg y Packed RGB24 tint color. Alpha channel determines blend amount.
@arg a8 Alpha channel (0..25) to be inserted into returned color

@return Packed ARGB32 color

@group Color
*/
sU32 sdvg_TintRGBAlpha (sU32 _x, sU32 _y, sU8 _a8);

/* @function sdvg_RGBAlpha,int c32,byte a8:int
Replace alpha channel of packed ARGB32 color

@arg c32 Packed RGB24 color
@arg a8 Alpha channel (0..255) to be inserted into returned color

@return Packed ARGB32 color

@group Color
*/
sU32 sdvg_RGBAlpha (sU32 _c32, sU8 _a8);

/* @function sdvg_HSVAToARGB,float h,float s,float v,byte a8:int
Convert hue / saturation / value / alpha into packed ARGB32 color

@arg h Hue (0..360)
@arg s Saturation (0..1)
@arg v Value (0..1)
@arg a8 Alpha channel (0..255) to be inserted into returned color

@return Packed ARGB32 color

@group Color
*/
sU32 sdvg_HSVAToARGB (sF32 _h, sF32 _s, sF32 _v, sU8 _a8);

/* @function sdvg_ARGBToHSVA,int c32,Float retH,Float retS,Float retV:byte
Split packed ARGB32 color into hue / saturation / value / alpha components

@arg c32 Packed ARGB32 color
@arg retH Returns hue (0..360)
@arg retS Returns saturation (0..1)
@arg retV Returns value (0..1)

@return Alpha channel (0..255)

@group Color
*/
sU8 sdvg_ARGBToHSVA (sU32 _c32, sF32 *_retH, sF32 *_retS, sF32 *_retV);

/* @function sdvg_GradientToTexture,Texture dst,IntArray colors,IntArray starts
Convert gradient (colors + start positions) to ARGB32 texture.

The first start position must be 0, and the last position determines the total gradient size.

@arg dst Destination texture. Allocation size (at least 1) determines resolution of interpolated gradient.
@arg colors ARGB32 color array. Number of elements must be at least two and determines number of gradient entries.
@arg starts Color start positions. Number of elements must be greater or equal to 'colors' array size.
@arg bSmoothStep false=linear interpolation  true=smoothstep interpolation

@group Gradient
@group Texture
*/
void YAC_CALL sdvg_GradientToTexture (sU32 *_dst, sU32 _dstW, const sU32 *_colors, sU32 _numColors, const sSI *_starts, sUI _numStarts, sBool _bSmoothStep);

#ifdef SHADERVG_SCRIPT_API
YF sU32 YAC_CALL _sdvg_ARGB (sUI _a, sUI _r, sUI _g, sUI _b);
YF sU32 YAC_CALL _sdvg_MixARGBx (sU32 _x, sU32 _y, sUI _t);
YF sU32 YAC_CALL _sdvg_MixRGBx (sU32 _x, sU32 _y, sUI _t);
YF sU32 YAC_CALL _sdvg_TintRGBAlpha (sU32 _x, sU32 _y, sUI _a8);
YF sU32 YAC_CALL _sdvg_RGBAlpha (sU32 _c32, sUI _a8);
YF sU32 YAC_CALL _sdvg_HSVAToARGB (sF32 _h, sF32 _s, sF32 _v, sUI _a8);
YF sUI YAC_CALL _sdvg_ARGBToHSVA (sU32 _c32, YAC_Object *_retH, YAC_Object *_retS, YAC_Object *_retV);
YF void YAC_CALL _sdvg_GradientToTexture (YAC_Object *_tex, YAC_Object *_colors, YAC_Object *_starts, sBool _bSmoothStep);
#endif // SHADERVG_SCRIPT_API

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif // SHADERVG_SKIP_DECLARATIONS


#endif // SHADERVG_H__
