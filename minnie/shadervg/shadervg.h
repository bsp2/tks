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
alias for §SDVG_TEXFMT_BGRA8888 (little endian)
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
*/
YF sBool YAC_CALL sdvg_Init (sBool _bGLCore);

/* @function sdvg_Exit
Shutdown ShaderVG library
*/
YF void YAC_CALL sdvg_Exit (void);

/* @function sdvg_OnOpen:boolean
Must be called after GL context has been lost, e.g. after the desktop window has been resized or opened.
*/
YF sBool YAC_CALL sdvg_OnOpen (void);

/* @function sdvg_SetFramebufferSize,int w,int h
Set framebuffer size
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
*/
YF void YAC_CALL sdvg_EndFrame (void);

// -------- viewport --------
/* @function sdvg_SetViewport,int x,int y,int w,int h
Set viewport.

@arg x Left viewport coordinate
@arg y Top viewport coordinate
@arg w Viewport width
@arg h Viewport height

@see sdvg_PushViewport
@see sdvg_PopViewport
*/
YF void YAC_CALL sdvg_SetViewport (sUI _x, sUI _y, sUI _w, sUI _h);

/* @function sdvg_PushViewport,int x,int y,int w,int h
Push current viewport onto viewport stack and set new viewport.

@see sdvg_SetViewport
@see sdvg_PopViewport
*/
YF void YAC_CALL sdvg_PushViewport (sUI _x, sUI _y, sUI _w, sUI _h);

/* @function sdvg_PopViewport
Pop viewport from viewport stack.

@see sdvg_SetViewport
@see sdvg_PushViewport
@see sdvg_PopViewport
*/
YF void YAC_CALL sdvg_PopViewport (void);

/* @function sdvg_GetViewportWidth:int
Query current viewport width
*/
YF sUI YAC_CALL sdvg_GetViewportWidth (void);

/* @function sdvg_GetViewportHeight:int
Query current viewport height
*/
YF sUI YAC_CALL sdvg_GetViewportHeight (void);

// -------- scissor --------
/* @function sdvg_SetScissor,int x,int y,int w,int h
Set current scissor clipping rectangle

@arg x Left scissor position
@arg y Top scissor position
@arg w Scissor width
@arg h Scissor height

@see sdvg_PushScissor
@see sdvg_PopScissor
@see sdvg_EnableScissor
@see sdvg_DisableScissor
*/
YF void YAC_CALL sdvg_SetScissor (sUI _x, sUI _y, sUI _w, sUI _h);

/* @function sdvg_PushScissor,int x,int y,int w,int h
Push current scissor clipping rectangle onto scissor stack and set new scissor rectangle.

@arg x Left scissor position
@arg y Top scissor position
@arg w Scissor width
@arg h Scissor height

@see sdvg_SetScissor
@see sdvg_PopScissor
@see sdvg_EnableScissor
@see sdvg_DisableScissor
*/
YF void YAC_CALL sdvg_PushScissor (sUI _x, sUI _y, sUI _w, sUI _h);

/* @function sdvg_PopScissor
Pop scissor rectangle from scissor stack

@see sdvg_SetScissor
@see sdvg_PushScissor
@see sdvg_EnableScissor
@see sdvg_DisableScissor
*/
YF void YAC_CALL sdvg_PopScissor (void);

/* @function sdvg_EnableScissor
Enable scissor test (clipping rectangle)

@see sdvg_SetScissor
@see sdvg_PushScissor
@see sdvg_PopScissor
@see sdvg_DisableScissor
*/
YF void YAC_CALL sdvg_EnableScissor (void);

/* @function sdvg_DisableScissor
Disable scissor test (clipping rectangle)

@see sdvg_SetScissor
@see sdvg_PushScissor
@see sdvg_PopScissor
@see sdvg_EnableScissor
*/
YF void YAC_CALL sdvg_DisableScissor (void);

/* @function sdvg_Flush
Flush pending GL state changes and draw calls to GPU
*/
YF void YAC_CALL sdvg_Flush (void);
// (note) helper function that unbinds/unmaps (scratch) VBO and shader program
// (note) needed during app transition from GL to ShaderVG/Minnie API
// (note) !! caution !! this call is _very_ expensive (~150x slowdown when called frequently)

// -------- transform --------
/* @function sdvg_SetTransform,Matrix4f mat4
Set modelview-projection transformation matrix 

@see sdvg_GetTransformRef
*/
YF void YAC_CALL sdvg_SetTransform (YAC_Object *_mat4);

/* @function sdvg_GetTransformRef:Matrix4f
Get reference to modelview-projection matrix

@see sdvg_SetTransform
*/
YF YAC_Object *YAC_CALL sdvg_GetTransformRef (void);

// -------- AA --------
/* @function sdvg_SetEnableAA,boolean bEnable
Enable or disable (analytical) anti-aliasing
*/
YF void YAC_CALL sdvg_SetEnableAA (sBool _bEnable);

/* @function sdvg_SetAARange,float aaRange
Change (analytical) anti-aliasing range.

It is not recommended to change the default setting.
*/
YF void YAC_CALL sdvg_SetAARange (sF32 _aaRange);

/* @function sdvg_SetAAExp,float aaExp
Set (analytical) anti-aliasing exponent.

Deprecated, will be removed in future versions.
*/
YF void YAC_CALL sdvg_SetAAExp (sF32 _aaExp);

/* @function sdvg_SetAlphaSDFRange,float aMin,float aMax
Set alpha range of distance field textures.

This controls the bluriness of e.g. anti-aliased glyph edges during text rendering.

@arg aMin Alpha range start (0..1, e.g. 0.8)
@arg aMax Alpha range end (0..1, e.g. 0.95)

@see sdvg_SetAlphaSDFExp
@see sdvg_DrawTrianglesTexUVFlatVBO32AlphaSDF
@see sdvg_BeginTexturedTrianglesAlphaSDF
*/
YF void YAC_CALL sdvg_SetAlphaSDFRange (sF32 _aMin, sF32 _aMax);

/* @function sdvg_SetAlphaSDFExp,float aExp
Set exponent of output alpha

@arg aExp Alpha exponent (gamma correction) (def=0.7)

@see sdvg_SetAlphaSDFRange
@see sdvg_DrawTrianglesTexUVFlatVBO32AlphaSDF
@see sdvg_BeginTexturedTrianglesAlphaSDF
*/
YF void YAC_CALL sdvg_SetAlphaSDFExp (sF32 _aExp);

// -------- render state (fill / stroke) --------
/* @function sdvg_SetFillColor4f,float fillR,float fillG,float fillB,float A
Set fill color (normalized floats)
@arg fillR Red (0..1)
@arg fillG Green (0..1)
@arg fillB Blue (0..1)
@arg fillA Alpha (0..1)

@see sdvg_SetFillAlpha
@see sdvg_SetFillColorARGB
@see sdvg_SetStrokeColor4f
@see sdvg_SetStrokeColorARGB
@see sdvg_SetColorARGB
*/
YF void YAC_CALL sdvg_SetFillColor4f (sF32 _fillR, sF32 _fillG, sF32 _fillB, sF32 _fillA);

/* @function sdvg_SetFillAlpha,float fillA
Set fill alpha (normalized float)
@arg fillA Alpha (0..1)

@see sdvg_SetFillColor4f
@see sdvg_SetFillColorARGB
@see sdvg_SetStrokeColor4f
@see sdvg_SetStrokeColorARGB
@see sdvg_SetColorARGB
*/
YF void YAC_CALL sdvg_SetFillAlpha (sF32 _fillA);

/* @function sdvg_SetFillColorARGB,int c32
Set fill color

@arg c32 packed ARGB32 color

@see sdvg_SetFillColor4f
@see sdvg_SetFillAlpha
@see sdvg_SetStrokeColor4f
@see sdvg_SetStrokeColorARGB
@see sdvg_SetColorARGB
*/
YF void YAC_CALL sdvg_SetFillColorARGB (sUI _c32);

/* @function sdvg_SetColorARGB,int c32
Set fill and stroke colors

@arg c32 packed ARGB32 color

@see sdvg_SetFillColor4f
@see sdvg_SetFillAlpha
@see sdvg_SetFillColorARGB
@see sdvg_SetStrokeColor4f
@see sdvg_SetStrokeColorARGB
*/
YF void YAC_CALL sdvg_SetColorARGB (sUI _c32);

/* @function sdvg_SetStrokeWidth,float strokeW
Set line stroke width.

@arg strokeW Stroke width

The total line width is (2 * strokeW)
*/
YF void YAC_CALL sdvg_SetStrokeWidth (sF32 _strokeW);

/* @function sdvg_SetPointRadius,float radius
Set point radius

@see sdvg_DrawPointsSquareVBO32
@see sdvg_DrawPointsSquareAAVBO32
@see sdvg_DrawPointsRoundVBO32
@see sdvg_DrawPointsRoundAAVBO32
*/
YF void YAC_CALL sdvg_SetPointRadius (sF32 _radius);

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

@see sdvg_SetStrokeColorARGB
*/
YF void YAC_CALL sdvg_SetStrokeColor4f (sF32 _strokeR, sF32 _strokeG, sF32 _strokeB, sF32 _strokeA);

/* @function sdvg_SetStrokeColorARGB,int c32
Set stroke color

@arg c32 Packed ARGB32 color

@see sdvg_SetStrokeColor4f
*/
YF void YAC_CALL sdvg_SetStrokeColorARGB (sU32 _c32);

/* @function sdvg_SetGlobalAlpha,float a
Set global alpha. Applied to all draw calls.
*/
YF void YAC_CALL sdvg_SetGlobalAlpha (sF32 _a);

/* @function sdvg_SetTextureDecalAlpha,float decalAlpha
Set texture opacity in Decal draw calls

@arg decalAlpha normalized opacity (0..1)

@see sdvg_DrawTrianglesTexUVFlatDecalVBO32
@see sdvg_DrawTrianglesTexUVGouraudDecalVBO32
*/
YF void YAC_CALL sdvg_SetTextureDecalAlpha (sF32 _decalAlpha);

// -------- clear --------
/* @function sdvg_Clear4f,float r,float g,float b,float a
Clear screen (scissor clipping rectangle)

@arg r Normalized red value
@arg g Normalized green value
@arg b Normalized blue value
@arg a Normalized alpha value
*/
YF void YAC_CALL sdvg_Clear4f (sF32 _r, sF32 _g, sF32 _b, sF32 _a);

/* @function sdvg_ClearARGB,int c32
Clear screen (scissor clipping rectangle)

@arg c32 Packed ARGB32 color
*/
YF void YAC_CALL sdvg_ClearARGB (sUI _c32);

// -------- blending --------
/* @function sdvg_EnableBlending
Enable source-over blending

@see sdvg_EnableBlendingKeepAlpha
@see sdvg_EnableBlendingAdditive
@see sdvg_EnableBlendingAdditiveKeepAlpha
@see sdvg_EnableBlendingSrcColorKeepAlpha
@see sdvg_EnableBlendingDstColorKeepAlpha
@see sdvg_DisableBlending
*/
YF void YAC_CALL sdvg_EnableBlending (void);

/* @function sdvg_EnableBlendingKeepAlpha
Enable source-over blending (do not modify destination alpha)

@see sdvg_EnableBlending
@see sdvg_EnableBlendingAdditive
@see sdvg_EnableBlendingAdditiveKeepAlpha
@see sdvg_EnableBlendingSrcColorKeepAlpha
@see sdvg_EnableBlendingDstColorKeepAlpha
@see sdvg_DisableBlending
*/
YF void YAC_CALL sdvg_EnableBlendingKeepAlpha (void);

/* @function sdvg_EnableBlendingAdditive
Enable additive blending

@see sdvg_EnableBlending
@see sdvg_EnableBlendingKeepAlpha
@see sdvg_EnableBlendingAdditiveKeepAlpha
@see sdvg_EnableBlendingSrcColorKeepAlpha
@see sdvg_EnableBlendingDstColorKeepAlpha
@see sdvg_DisableBlending
*/
YF void YAC_CALL sdvg_EnableBlendingAdditive (void);

/* @function sdvg_EnableBlendingAdditiveKeepAlpha
Enable additive blending (do not modify destination alpha)

@see sdvg_EnableBlending
@see sdvg_EnableBlendingKeepAlpha
@see sdvg_EnableBlendingAdditive
@see sdvg_EnableBlendingSrcColorKeepAlpha
@see sdvg_EnableBlendingDstColorKeepAlpha
@see sdvg_DisableBlending
*/
YF void YAC_CALL sdvg_EnableBlendingAdditiveKeepAlpha (void);

/* @function sdvg_EnableBlendingSrcColorKeepAlpha

@see sdvg_EnableBlending
@see sdvg_EnableBlendingKeepAlpha
@see sdvg_EnableBlendingAdditive
@see sdvg_EnableBlendingAdditiveKeepAlpha
@see sdvg_EnableBlendingDstColorKeepAlpha
@see sdvg_DisableBlending
*/
YF void YAC_CALL sdvg_EnableBlendingSrcColorKeepAlpha (void);

/* @function sdvg_EnableBlendingDstColorKeepAlpha

@see sdvg_EnableBlending
@see sdvg_EnableBlendingKeepAlpha
@see sdvg_EnableBlendingAdditive
@see sdvg_EnableBlendingAdditiveKeepAlpha
@see sdvg_EnableBlendingSrcColorKeepAlpha
@see sdvg_DisableBlending
*/
YF void YAC_CALL sdvg_EnableBlendingDstColorKeepAlpha (void);

/* @function sdvg_DisableBlending
Disable blending

@see sdvg_EnableBlending
@see sdvg_EnableBlendingKeepAlpha
@see sdvg_EnableBlendingAdditive
@see sdvg_EnableBlendingAdditiveKeepAlpha
@see sdvg_EnableBlendingSrcColorKeepAlpha
@see sdvg_EnableBlendingDstColorKeepAlpha
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

@see sdvg_SetEnableDebug
*/
YF sBool YAC_CALL sdvg_GetEnableDebug (void);

/* @function sdvg_SetEnableDebug,boolean bEnable
Enable or disable debug mode

@see sdvg_GetEnableDebug
*/
YF void YAC_CALL sdvg_SetEnableDebug (sBool _bEnable);

/* @function sdvg_GetEnableDrawInner:boolean
Query draw-inner mode

@see sdvg_SetEnableDrawInner
*/
YF sBool YAC_CALL sdvg_GetEnableDrawInner (void);

/* @function sdvg_SetEnableDrawInner,boolean bEnable
Enable or disable(debug) draw-inner mode

@see sdvg_GetEnableDrawInner
*/
YF void YAC_CALL sdvg_SetEnableDrawInner (sBool _bEnable);

/* @function sdvg_GetEnableDrawBorder:boolean
Query draw-border mode

@see sdvg_SetEnableDrawBorder
*/
YF sBool YAC_CALL sdvg_GetEnableDrawBorder (void);

/* @function sdvg_SetEnableDrawBorder,boolean bEnable
Enable or disable(debug) draw-border mode

@see sdvg_GetEnableDrawBorder
*/
YF void YAC_CALL sdvg_SetEnableDrawBorder (sBool _bEnable);

/* @function sdvg_DebugSetEnableWriteVBO,boolean bEnable
Enable or disable VBO writes (false=debug)

*/
YF void YAC_CALL sdvg_DebugSetEnableWriteVBO (sBool _bEnable);

// -------- VBO --------
/* @function sdvg_CreateVBO,int numBytes:int
Create vertex buffer object (VBO)

@arg numBytes Vertex buffer object size (bytes)
@return return OpenGL VBO id (0 == failed)

@see sdvg_BindVBO
@see sdvg_UpdateVBO
@see sdvg_MapVBO
@see sdvg_GetVBOSize
@see sdvg_GetMappedVBOOffset
@see sdvg_MapVBO
@see sdvg_UnmapVBO
@see sdvg_UnbindVBO
@see sdvg_DestroyVBO
*/
YF sUI YAC_CALL sdvg_CreateVBO (sUI _numBytes);

/* @function sdvg_UpdateVBO,int vboId,int offset,int numBytes,Object data
Update vertex buffer object contents

@see sdvg_CreateVBO
@see sdvg_BindVBO
@see sdvg_MapVBO
@see sdvg_GetVBOSize
@see sdvg_GetMappedVBOOffset
@see sdvg_MapVBO
@see sdvg_UnmapVBO
@see sdvg_UnbindVBO
@see sdvg_DestroyVBO
*/
YF void YAC_CALL sdvg_UpdateVBO (sUI _vboId, sUI _offset, sUI _numBytes, YAC_Object *_data);

/* @function sdvg_BindVBO,int vboId
Bind vertex buffer object

@see sdvg_CreateVBO
@see sdvg_UpdateVBO
@see sdvg_MapVBO
@see sdvg_GetVBOSize
@see sdvg_GetMappedVBOOffset
@see sdvg_MapVBO
@see sdvg_UnmapVBO
@see sdvg_UnbindVBO
@see sdvg_DestroyVBO
*/
YF void YAC_CALL sdvg_BindVBO (sUI _vboId);

/* @function sdvg_GetVBOSize:int
Query vertex buffer object size

@see sdvg_CreateVBO
@see sdvg_BindVBO
@see sdvg_UpdateVBO
@see sdvg_MapVBO
@see sdvg_GetMappedVBOOffset
@see sdvg_MapVBO
@see sdvg_UnmapVBO
@see sdvg_UnbindVBO
@see sdvg_DestroyVBO
*/
YF sUI YAC_CALL sdvg_GetVBOSize (void);

/* @function sdvg_GetMappedVBOOffset:int
Query write offset of currently mapped vertex buffer object

@see sdvg_CreateVBO
@see sdvg_BindVBO
@see sdvg_UpdateVBO
@see sdvg_MapVBO
@see sdvg_GetVBOSize
@see sdvg_MapVBO
@see sdvg_UnmapVBO
@see sdvg_UnbindVBO
@see sdvg_DestroyVBO
*/
YF sUI YAC_CALL sdvg_GetMappedVBOOffset (void);

/* @function sdvg_MapVBO,int vboId
Map vertex buffer object VRAM into virtual address space.

§sdvg_BeginVBO can be used to start writing vertex attribute data to the mapped VBO.

@see sdvg_CreateVBO
@see sdvg_BindVBO
@see sdvg_UpdateVBO
@see sdvg_GetVBOSize
@see sdvg_GetMappedVBOOffset
@see sdvg_UnmapVBO
@see sdvg_UnbindVBO
@see sdvg_DestroyVBO
@see sdvg_BeginVBO
*/
YF void YAC_CALL sdvg_MapVBO (sUI _vboId);

/* @function sdvg_UnmapVBO
Unmap vertex buffer object

@see sdvg_CreateVBO
@see sdvg_BindVBO
@see sdvg_UpdateVBO
@see sdvg_MapVBO
@see sdvg_GetVBOSize
@see sdvg_GetMappedVBOOffset
@see sdvg_MapVBO
@see sdvg_UnbindVBO
@see sdvg_DestroyVBO
*/
YF void YAC_CALL sdvg_UnmapVBO (void);

/* @function sdvg_UnbindVBO
Unbind current vertex buffer object

@see sdvg_CreateVBO
@see sdvg_BindVBO
@see sdvg_UpdateVBO
@see sdvg_MapVBO
@see sdvg_GetVBOSize
@see sdvg_GetMappedVBOOffset
@see sdvg_MapVBO
@see sdvg_UnmapVBO
@see sdvg_DestroyVBO
*/
YF void YAC_CALL sdvg_UnbindVBO (void);

/* @function sdvg_DestroyVBO,int vboId
Destroy vertex buffer object

@see sdvg_CreateVBO
@see sdvg_BindVBO
@see sdvg_UpdateVBO
@see sdvg_MapVBO
@see sdvg_GetVBOSize
@see sdvg_GetMappedVBOOffset
@see sdvg_MapVBO
@see sdvg_UnmapVBO
@see sdvg_UnbindVBO
@see sdvg_DestroyVBO
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

@see sdvg_CreateFBO
@see sdvg_BindFBO
@see sdvg_ResolveFBO
@see sdvg_UnbindFBO
@see sdvg_GetFBOTextureId
@see sdvg_BindFBOTexture
@see sdvg_DestroyFBO
*/
YF sUI YAC_CALL sdvg_CreateFBOEx (sUI _w, sUI _h, sBool _bMSAA, sBool _bDepth, sBool _bStencil);

/* @function sdvg_CreateFBO,int w,int h:int
Create framebuffer object

@arg w Width
@arg h Height

@return ShaderVG FBO index (0 == failed)

@see sdvg_CreateFBOEx
@see sdvg_BindFBO
@see sdvg_ResolveFBO
@see sdvg_UnbindFBO
@see sdvg_GetFBOTextureId
@see sdvg_BindFBOTexture
@see sdvg_DestroyFBO
*/
YF sUI YAC_CALL sdvg_CreateFBO (sUI _w, sUI _h);

/* @function sdvg_BindFBO,int fboIdx
Bind framebuffer object

@see sdvg_CreateFBOEx
@see sdvg_CreateFBO
@see sdvg_ResolveFBO
@see sdvg_UnbindFBO
@see sdvg_GetFBOTextureId
@see sdvg_BindFBOTexture
@see sdvg_DestroyFBO
*/
YF void YAC_CALL sdvg_BindFBO (sUI _fboIdx);

/* @function sdvg_ResolveFBO,int fboIdx
Resolve MSAA framebuffer object

Must be called before FBO can be used as a texture map

@see sdvg_CreateFBOEx
@see sdvg_CreateFBO
@see sdvg_BindFBO
@see sdvg_UnbindFBO
@see sdvg_GetFBOTextureId
@see sdvg_BindFBOTexture
@see sdvg_DestroyFBO
*/
YF void YAC_CALL sdvg_ResolveFBO (sUI _fboIdx);

/* @function sdvg_UnbindFBO
Unbind framebuffer object

@see sdvg_CreateFBOEx
@see sdvg_CreateFBO
@see sdvg_BindFBO
@see sdvg_ResolveFBO
@see sdvg_GetFBOTextureId
@see sdvg_BindFBOTexture
@see sdvg_DestroyFBO
*/
YF void YAC_CALL sdvg_UnbindFBO (void);

/* @function sdvg_GetFBOTextureId,int fboIdx:int
Query framebuffer object OpenGL texture id

@arg fboIdx ShaderVG fbo index
@return GL texture id

@see sdvg_CreateFBOEx
@see sdvg_CreateFBO
@see sdvg_BindFBO
@see sdvg_ResolveFBO
@see sdvg_UnbindFBO
@see sdvg_BindFBOTexture
@see sdvg_DestroyFBO
*/
YF sUI YAC_CALL sdvg_GetFBOTextureId (sUI _fboIdx);

/* @function sdvg_BindFBOTexture,int fboIdx,boolean bRepeat,boolean bFilter
Bind framebuffer object as texture

@see sdvg_CreateFBOEx
@see sdvg_CreateFBO
@see sdvg_BindFBO
@see sdvg_ResolveFBO
@see sdvg_UnbindFBO
@see sdvg_GetFBOTextureId
@see sdvg_DestroyFBO
*/
YF void YAC_CALL sdvg_BindFBOTexture (sUI _fboIdx, sBool _bRepeat, sBool _bFilter);

/* @function sdvg_DestroyFBO,int fboIdx
Destroy framebuffer object

@see sdvg_CreateFBOEx
@see sdvg_CreateFBO
@see sdvg_BindFBO
@see sdvg_ResolveFBO
@see sdvg_UnbindFBO
@see sdvg_GetFBOTextureId
@see sdvg_BindFBOTexture
*/
YF void YAC_CALL sdvg_DestroyFBO (sUI _fboIdx);

// -------- texturing --------

/* @function sdvg_CreateTexture2D,int texFmt,int w,int h,Object data:int
Create and allocate 2D texture
@arg texFmt  §SDVG_TEXFMT_ALPHA8, §SDVG_TEXFMT_RGB565, §SDVG_TEXFMT_BGRA8888, §SDVG_TEXFMT_ARGB32, §SDVG_TEXFMT_RGBA8888, §
@arg w Width
@arg h Height
@arg data Initial pixel data or null (only reserve memory)
@return OpenGL texture id

@see sdvg_UpdateTexture2D
@see sdvg_BindMultiTexture2D
@see sdvg_UnbindMultiTexture2D
@see sdvg_BindTexture2D
@see sdvg_UnbindTexture2D
@see sdvg_DestroyTexture2D
*/
sUI YAC_CALL sdvg_CreateTexture2D (sUI _texfmt, sUI _w, sUI _h, const void *_data, sUI _dataSz);
#ifdef SHADERVG_SCRIPT_API
YF sUI YAC_CALL _sdvg_CreateTexture2D (sUI _texfmt, sUI _w, sUI _h, YAC_Object *_data);
#endif // SHADERVG_SCRIPT_API

/* @function sdvg_UpdateTexture2D,int texFmt,int w,int h,Object data
Update contents of currently bound 2D texture
@arg texFmt  §SDVG_TEXFMT_ALPHA8, §SDVG_TEXFMT_RGB565, §SDVG_TEXFMT_BGRA8888, §SDVG_TEXFMT_ARGB32, §SDVG_TEXFMT_RGBA8888, §
@arg w Width
@arg h Height
@arg data New pixel data

@see sdvg_BindMultiTexture2D
@see sdvg_UnbindMultiTexture2D
@see sdvg_BindTexture2D
@see sdvg_UnbindTexture2D
@see sdvg_DestroyTexture2D
*/
void YAC_CALL sdvg_UpdateTexture2D (sUI _texfmt, sUI _w, sUI _h, const void *_data, sUI _dataSz);
#ifdef SHADERVG_SCRIPT_API
YF void YAC_CALL _sdvg_UpdateTexture2D (sUI _texfmt, sUI _w, sUI _h, YAC_Object *_data);
#endif // SHADERVG_SCRIPT_API

/* @function sdvg_BindMultiTexture2D,int unitIdx,int texId,boolean bRepeat,boolean bFilter
Bind texture to given texture unit

@see sdvg_CreateTexture2D
@see sdvg_UpdateTexture2D
@see sdvg_UnbindMultiTexture2D
@see sdvg_BindTexture2D
@see sdvg_UnbindTexture2D
@see sdvg_DestroyTexture2D
*/
YF void YAC_CALL sdvg_BindMultiTexture2D (sUI _unitIdx, sUI _texId, sBool _bRepeat, sBool _bFilter);

/* @function sdvg_UnbindMultiTexture2D,int unitIdx
Unbind texture from given texture unit

@see sdvg_CreateTexture2D
@see sdvg_UpdateTexture2D
@see sdvg_BindMultiTexture2D
@see sdvg_BindTexture2D
@see sdvg_UnbindTexture2D
@see sdvg_DestroyTexture2D
*/
YF void YAC_CALL sdvg_UnbindMultiTexture2D (sUI _unitIdx);

/* @function sdvg_BindTexture2D,int texId,boolean bRepeat,boolean bFilter
Bind texture to default texture unit

@see sdvg_CreateTexture2D
@see sdvg_UpdateTexture2D
@see sdvg_BindMultiTexture2D
@see sdvg_UnbindMultiTexture2D
@see sdvg_UnbindTexture2D
@see sdvg_DestroyTexture2D
*/
YF void YAC_CALL sdvg_BindTexture2D (sUI _texId, sBool _bRepeat, sBool _bFilter);

/* @function sdvg_UnbindTexture2D
Unbind texture from default texture unit

@see sdvg_CreateTexture2D
@see sdvg_UpdateTexture2D
@see sdvg_BindMultiTexture2D
@see sdvg_UnbindMultiTexture2D
@see sdvg_BindTexture2D
@see sdvg_DestroyTexture2D
*/
YF void YAC_CALL sdvg_UnbindTexture2D (void);

/* @function sdvg_DestroyTexture2D,int texId
Destroy texture

@see sdvg_CreateTexture2D
@see sdvg_UpdateTexture2D
@see sdvg_BindMultiTexture2D
@see sdvg_UnbindMultiTexture2D
@see sdvg_BindTexture2D
@see sdvg_UnbindTexture2D
*/
YF void YAC_CALL sdvg_DestroyTexture2D (sUI _texId);

// -------- VBO utility functions --------
/* @function sdvg_BufferAddRectTexUVFlat32,Buffer b,float x,float y,float w,float h,float ul,float vt,float ur,float vb
Append textured rectangle vertex attributes to buffer
*/
YF void YAC_CALL sdvg_BufferAddRectTexUVFlat32 (YAC_Buffer *_b, sF32 _x, sF32 _y, sF32 _w, sF32 _h, sF32 _ul, sF32 _vt, sF32 _ur, sF32 _vb);

/* @function sdvg_WriteC32AsRGBA8,Buffer b,int c32
Append r,g,b,a bytes to buffer

@arg c32 Packed ARGB32 color
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
*/
YF void YAC_CALL sdvg_BufferAddRectTexUVGouraud32 (YAC_Buffer *_b,  sF32 _x, sF32 _y, sF32 _w, sF32 _h, sF32 _ul, sF32 _vt, sF32 _ur, sF32 _vb, sUI _c32LT, sUI _c32RT, sUI _c32LB, sUI _c32RB);

/* @function sdvg_BufferAddLinePointFlat14_2,Buffer b,float x,float y
Append line / point vertex coordinate to buffer in 14.2 fixed point format
*/
YF void YAC_CALL sdvg_BufferAddLinePointFlat14_2 (YAC_Buffer *_b, sF32 _x, sF32 _y);

/* @function sdvg_BufferAddLinePointFlat32,Buffer b,float x,float y
Append line / point vertex coordinate to buffer in 32bit float format
*/
YF void YAC_CALL sdvg_BufferAddLinePointFlat32 (YAC_Buffer *_b, sF32 _x, sF32 _y);

/* @function sdvg_BufferAddLinePointFlatBevel14_2,Buffer b,float x,float y
Append bevel line-joint vertex coordinate to buffer in 14.2 fixed point format
*/
YF void YAC_CALL sdvg_BufferAddLinePointFlatBevel14_2 (YAC_Buffer *_b, sF32 _x, sF32 _y);

/* @function sdvg_BufferAddLinePointFlatBevel32,Buffer b,float x,float y
Append bevel line-joint vertex coordinate to buffer in 32bit float format
*/
YF void YAC_CALL sdvg_BufferAddLinePointFlatBevel32 (YAC_Buffer *_b, sF32 _x, sF32 _y);

/* @function sdvg_BufferAddLinesPointsFlat14_2,Buffer b,float x1,float y1,float x2,float y2
Append line segment vertex coordinates to buffer in 14.2 fixed point format

@arg x1 Line start point X
@arg y1 Line start point Y
@arg x2 Line end point X
@arg y2 Line end point Y
*/
YF void YAC_CALL sdvg_BufferAddLinesPointsFlat14_2 (YAC_Buffer *_b, sF32 _x1, sF32 _y1, sF32 _x2, sF32 _y2);

/* @function sdvg_BufferAddLinesPointsFlat32,Buffer b,float x1,float y1,float x2,float y2
Append line segment vertex coordinates to buffer in 32bit float fixed point

@arg x1 Line start point X
@arg y1 Line start point Y
@arg x2 Line end point X
@arg y2 Line end point Y
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
*/
YF void YAC_CALL sdvg_DrawTrianglesFillFlatVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numVerts);

/* @function sdvg_DrawTrianglesFillGouraudVBO32,int vboId,int byteOffset,int numVerts
Draw previously prepared vertex buffer as filled, gouraud shaded triangles (32bit float format)

<pre>
VBO vertex format (12 bytes per vertex):<br>
  f32 x<br>
  f32 y<br>
  u8  r<br>
  u8  g<br>
  u8  b<br>
  u8  a<br>
</pre>
*/
YF void YAC_CALL sdvg_DrawTrianglesFillGouraudVBO32 (sUI _vboId, sUI _byteOffset, sUI _numVerts);

/* @function sdvg_DrawTrianglesFillGouraudVBO14_2,int vboId,int byteOffset,int numVerts
Draw previously prepared vertex buffer as filled, gouraud shaded triangles (14.2 fixed point format)

<pre>
VBO vertex format (8 bytes per vertex):<br>
  +0 s14.2 x<br>
  +2 s14.2 y<br>
  +4 u8    r<br>
  +5 u8    g<br>
  +6 u8    b<br>
  +7 u8    a<br>
</pre>
*/
YF void YAC_CALL sdvg_DrawTrianglesFillGouraudVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numVerts);

/* @function sdvg_DrawTrianglesFillFlatEdgeAAVBO32,int vboId,int byteOffset,int numVerts
experimental
*/
YF void YAC_CALL sdvg_DrawTrianglesFillFlatEdgeAAVBO32 (sUI _vboId, sUI _byteOffset, sUI _numVerts);

/* @function sdvg_DrawTrianglesFillFlatEdgeAAVBO14_2,int vboId,int byteOffset,int numVerts
experimental
*/
YF void YAC_CALL sdvg_DrawTrianglesFillFlatEdgeAAVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numVerts);

/* @function sdvg_DrawTrianglesFillGouraudEdgeAAVBO32,int vboId,int byteOffset,int numVerts
experimental
*/
YF void YAC_CALL sdvg_DrawTrianglesFillGouraudEdgeAAVBO32 (sUI _vboId, sUI _byteOffset, sUI _numVerts);

/* @function sdvg_DrawTrianglesFillGouraudEdgeAAVBO14_2,int vboId,int byteOffset,int numVerts
experimental
*/
YF void YAC_CALL sdvg_DrawTrianglesFillGouraudEdgeAAVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numVerts);

/* @function sdvg_DrawPolygonFillFlatVBO32,int vboId,int byteOffset,int numVerts
Draw previously prepared vertex buffer as filled n-polygon (32bit float format)

<pre>
VBO vertex format (8 bytes per vertex):<br>
  +0 f32 x<br>
  +4 f32 y<br>
</pre>
*/
YF void YAC_CALL sdvg_DrawPolygonFillFlatVBO32 (sUI _vboId, sUI _byteOffset, sUI _numVerts);

/* @function sdvg_DrawPolygonFillFlatVBO14_2,int vboId,int byteOffset,int numVerts
Draw previously prepared vertex buffer as filled n-polygon (14.2 fixed point format)

<pre>
VBO vertex format (4 bytes per vertex):<br>
  s14.2 x<br>
  s14.2 y<br>
</pre>
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
*/
YF void YAC_CALL sdvg_DrawPolygonFillGouraudVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numVerts);

/* @function sdvg_SetupRectFillAAVBO32,Buffer vb,Buffer dl,float centerX,float centerY,float sizeX,float sizeY
Set up vertex buffer and draw list for filled, anti-aliased rectangle (32bit float format)
*/
YF void YAC_CALL sdvg_SetupRectFillAAVBO32 (YAC_Buffer *_vb, YAC_Buffer *_dl, sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY);

/* @function sdvg_DrawRectFillAAVBO32,int vboId,int byteOffsetInner,int numVertsInner,int byteOffsetBorder,int numVertsBorder,int glPrimTypeBorder,float centerX,float centerY,float sizeX,float sizeY
Draw previously prepared vertex buffer as filled, anti-aliased rectangle (32bit float format)
*/
YF void YAC_CALL sdvg_DrawRectFillAAVBO32 (sUI _vboId, sUI _byteOffsetInner, sUI _numVertsInner, sUI _byteOffsetBorder, sUI _numVertsBorder, sUI _glPrimTypeBorder, sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY);

/* @function sdvg_DrawRectFillAA,float centerX,float centerY,float sizeX,float sizeY
Draw filled, anti-aliased rectangle via scratch buffer (32 bit float format)
*/
YF void YAC_CALL sdvg_DrawRectFillAA (sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY);

/* @function sdvg_SetupRectFillStrokeAAVBO32,Buffer vb,Buffer dl,float centerX,float centerY,float sizeX,float sizeY,float strokeW
Set up vertex buffer and draw list for filled, stroked, and anti-aliased rectangle (32bit float format)
*/
YF void YAC_CALL sdvg_SetupRectFillStrokeAAVBO32 (YAC_Buffer *_vb, YAC_Buffer *_dl, sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY, sF32 _strokeW);

/* @function sdvg_DrawRectFillStrokeAAVBO32,int vboId,int byteOffsetInner,int numVertsInner,int byteOffsetBorder,int numVerts
Draw previously prepared vertex buffer as filled, stroked, and anti-aliased rectangle (32bit float format)
*/
YF void YAC_CALL sdvg_DrawRectFillStrokeAAVBO32 (sUI _vboId, sUI _byteOffsetInner, sUI _numVertsInner, sUI _byteOffsetBorder, sUI _numVertsBorder, sUI _glPrimTypeBorder, sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY);

/* @function sdvg_DrawRectFillStrokeAA,float centerX,float centerY,float sizeX,float sizeY
Draw filled, stroked, and anti-aliased rectangle via scratch buffer (32 bit float format)
*/
YF void YAC_CALL sdvg_DrawRectFillStrokeAA (sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY);

/* @function sdvg_SetupRectStrokeAAVBO32,Buffer vb,Buffer dl,float centerX,float centerY,float sizeX,float sizeY,float strokeW
Set up vertex buffer and draw list for stroked, anti-aliased rectangle (32bit float format)
*/
YF void YAC_CALL sdvg_SetupRectStrokeAAVBO32 (YAC_Buffer *_vb, YAC_Buffer *_dl, sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY, sF32 _strokeW);

/* @function sdvg_DrawRectStrokeAAVBO32,int vboId,int byteOffsetBorder,int numVertesBorder,int glPrimTypeBorder,float centerX,float centerY,float sizeX,float sizeY
Draw previously prepared vertex buffer as stroked, anti-aliased rectangle (32bit float format)
*/
YF void YAC_CALL sdvg_DrawRectStrokeAAVBO32 (sUI _vboId, sUI _byteOffsetBorder, sUI _numVertsBorder, sUI _glPrimTypeBorder, sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY);

/* @function sdvg_DrawRectStrokeAA,float centerX,float centerY,float sizeX,float sizeY
Draw stroked, anti-aliased rectangle via scratch buffer (32 bit float format)
*/
YF void YAC_CALL sdvg_DrawRectStrokeAA (sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY);

/* @function sdvg_SetupEllipseFillAAVBO32,Buffer vb,Buffer dl,float centerX,float centerY,float radiusX,float radiusY
Set up vertex buffer and draw list for filled, anti-aliased ellipse (32bit float format)
*/
YF void YAC_CALL sdvg_SetupEllipseFillAAVBO32 (YAC_Buffer *_vb, YAC_Buffer *_dl, sF32 _centerX, sF32 _centerY, sF32 _radiusX, sF32 _radiusY);

/* @function sdvg_DrawEllipseFillAAVBO32,int vboId,int byteOffsetInner,int numVertsInner,int byteOffsetBorder,int numVertsBorder,int glPrimTypeBorder,float centerX,float centerY,float radiusX,float radiusY
Draw previously prepared vertex buffer as filled, anti-aliased ellipse (32bit float format)
*/
YF void YAC_CALL sdvg_DrawEllipseFillAAVBO32 (sUI _vboId, sUI _byteOffsetInner, sUI _numVertsInner, sUI _byteOffsetBorder, sUI _numVertsBorder, sUI _glPrimTypeBorder, sF32 _centerX, sF32 _centerY, sF32 _radiusX, sF32 _radiusY);

/* @function sdvg_DrawEllipseFillAA,float centerX,float centerY,float sizeX,float sizeY
Draw filled, anti-aliased ellipse via scratch buffer (32 bit float format)
*/
YF void YAC_CALL sdvg_DrawEllipseFillAA (sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY);

/* @function sdvg_SetupEllipseFillStrokeAAVBO32,Buffer vb,Buffer dl,float centerX,float centerY,float radiusX,float radiusY,float strokeW
Set up vertex buffer and draw list for filled, stroked, and anti-aliased ellipse (32bit float format)
*/
YF void YAC_CALL sdvg_SetupEllipseFillStrokeAAVBO32 (YAC_Buffer *_vb, YAC_Buffer *_dl, sF32 _centerX, sF32 _centerY, sF32 _radiusX, sF32 _radiusY, sF32 _strokeW);

/* @function sdvg_DrawEllipseFillStrokeAAVBO32,int vboId,int byteOffsetInner,int numVertsInner,int byteOffsetBorder,int numVertsBorder,int glPrimTypeBorder,float centerX,float centerY,float radiusX,float radiusY
Draw previously prepared vertex buffer as filled, stroked, and anti-aliased ellipse (32bit float format)
*/
YF void YAC_CALL sdvg_DrawEllipseFillStrokeAAVBO32 (sUI _vboId, sUI _byteOffsetInner, sUI _numVertsInner, sUI _byteOffsetBorder, sUI _numVertsBorder, sUI _glPrimTypeBorder, sF32 _centerX, sF32 _centerY, sF32 _radiusX, sF32 _radiusY);

/* @function sdvg_DrawEllipseFillStrokeAA,float centerX,float centerY,float sizeX,float sizeY
Draw filled, stroked, and anti-aliased ellipse via scratch buffer (32 bit float format)
*/
YF void YAC_CALL sdvg_DrawEllipseFillStrokeAA (sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY);

/* @function sdvg_SetupEllipseStrokeAAVBO32,Buffer vb,Buffer dl,float centerX,float centerY,float radiusX,float radiusY,float strokeW
Set up vertex buffer and draw list for stroked, and anti-aliased ellipse (32bit float format)
*/
YF void YAC_CALL sdvg_SetupEllipseStrokeAAVBO32 (YAC_Buffer *_vb, YAC_Buffer *_dl, sF32 _centerX, sF32 _centerY, sF32 _radiusX, sF32 _radiusY, sF32 _strokeW);

/* @function sdvg_DrawEllipseStrokeAAVBO32,int vboId,int byteOffsetBorder,int numVertsBorder,int glPrimTypeBorder,float centerX,float centerY,float radiusX,float radiusY
Draw previously prepared vertex buffer as stroked, anti-aliased ellipse (32bit float format)
*/
YF void YAC_CALL sdvg_DrawEllipseStrokeAAVBO32 (sUI _vboId, sUI _byteOffsetBorder, sUI _numVertsBorder, sUI _glPrimTypeBorder, sF32 _centerX, sF32 _centerY, sF32 _radiusX, sF32 _radiusY);

/* @function sdvg_DrawEllipseStrokeAA,float centerX,float centerY,float sizeX,float sizeY
Draw stroked, anti-aliased ellipse via scratch buffer (32 bit float format)
*/
YF void YAC_CALL sdvg_DrawEllipseStrokeAA (sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY);

/* @function sdvg_SetupRoundRectFillAAVBO32,Buffer vb,Buffer dl,float centerX,float centerY,float sizeX,float sizeY,float radiusX,float radiusY
Set up vertex buffer and draw list for filled, and anti-aliased rounded rectangle (32bit float format)
*/
YF void YAC_CALL sdvg_SetupRoundRectFillAAVBO32 (YAC_Buffer *_vb, YAC_Buffer *_dl, sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY, sF32 _radiusX, sF32 _radiusY);

/* @function sdvg_DrawRoundRectFillAAVBO32,int vboId,int byteOffsetInner,int numVertsInner,int byteOffsetBorder,int numVertsBorder,int glPrimTypeBorder,float centerX,float centerY,float sizeX,float sizeY,float radiusX,float radiusY
Draw previously prepared vertex buffer as filled, anti-aliased rounded rectangle (32bit float format)
*/
YF void YAC_CALL sdvg_DrawRoundRectFillAAVBO32 (sUI _vboId, sUI _byteOffsetInner, sUI _numVertsInner, sUI _byteOffsetBorder, sUI _numVertsBorder, sUI _glPrimTypeBorder, sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY, sF32 _radiusX, sF32 _radiusY);

/* @function sdvg_DrawRoundRectFillAA,float centerX,float centerY,float sizeX,float sizeY,float radiusX,float radiusY
Draw filled, anti-aliased rounded rectangle via scratch buffer (32 bit float format)
*/
YF void YAC_CALL sdvg_DrawRoundRectFillAA (sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY, sF32 _radiusX, sF32 _radiusY);

/* @function sdvg_SetupRoundRectFillStrokeAAVBO32,Buffer vb,Buffer dl,float centerX,float centerY,float sizeX,float sizeY,float radiusX,float radiusY,float strokeW
Set up vertex buffer and draw list for filled, stroked, and anti-aliased rounded rectangle (32bit float format)
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
*/
YF void YAC_CALL sdvg_DrawRoundRectFillStrokeAAVBO32 (sUI _vboId, sUI _byteOffsetInner, sUI _numVertsInner, sUI _byteOffsetBorder, sUI _numVertsBorder, sUI _glPrimTypeBorder, sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY, sF32 _radiusX, sF32 _radiusY);

/* @function sdvg_DrawRoundRectFillStrokeAA,float centerX,float centerY,float sizeX,float sizeY,float radiusX,float radiusY
Draw filled, stroked, anti-aliased rounded rectangle via scratch buffer (32 bit float format)
*/
YF void YAC_CALL sdvg_DrawRoundRectFillStrokeAA (sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY, sF32 _radiusX, sF32 _radiusY);

/* @function sdvg_SetupRoundRectStrokeAAVBO32,Buffer vb,Buffer dl,float centerX,float centerY,float sizeX,float sizeY,float radiusX,float radiusY,float strokeW
Set up vertex buffer and draw list for stroked, anti-aliased rounded rectangle (32bit float format)
*/
YF void YAC_CALL sdvg_SetupRoundRectStrokeAAVBO32 (YAC_Buffer *_vb, YAC_Buffer *_dl, sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY, sF32 _radiusX, sF32 _radiusY, sF32 _strokeW);

/* @function sdvg_DrawRoundRectStrokeAAVBO32,int vboId,int byteOffsetBorder,int numVertsBorder,int glPrimTypeBorder,float centerX,float centerY,float sizeX,float sizeY,float radiusX,float radiusY
Draw previously prepared vertex buffer as stroked, anti-aliased rounded rectangle (32bit float format)
*/
YF void YAC_CALL sdvg_DrawRoundRectStrokeAAVBO32(sUI _vboId, sUI _byteOffsetBorder, sUI _numVertsBorder, sUI _glPrimTypeBorder, sF32 _centerX, sF32 _centerY, sF32 _sizeX, sF32 _sizeY, sF32 _radiusX, sF32 _radiusY);

/* @function sdvg_DrawRoundRectStrokeAA,float centerX,float centerY,float sizeX,float sizeY,float radiusX,float radiusY
Draw stroked, anti-aliased rounded rectangle via scratch buffer (32 bit float format)
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

@see sdvg_SetAlphaSDFRange
*/
YF void YAC_CALL sdvg_DrawTrianglesTexUVFlatVBO32AlphaSDF (sUI _vboId, sUI _byteOffset, sUI _numVerts);

/* @function sdvg_DrawLineStripFlatVBO14_2,int vboId,int byteOffset,int numPoints
Draw previously prepared vertex buffer as line strip (14.2 fixed point format)

<pre>
VBO vertex format (6 bytes per vertex):<br>
  +0 s14.2 x<br>
  +2 s14.2 y<br>
  +4 i16   index (when !defined(USE_VERTEX_ATTRIB_DIVISOR))<br>
</pre>
*/
YF void YAC_CALL sdvg_DrawLineStripFlatVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawLineStripFlatVBO32,int vboId,int byteOffset,int numPoints
Draw previously prepared vertex buffer as line strip (32 bit float format)
*/
YF void YAC_CALL sdvg_DrawLineStripFlatVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawLineStripFlatAAVBO14_2,int vboId,int byteOffset,int numPoints
Draw previously prepared vertex buffer as anti-aliased line strip (14.2 fixed point format)

<pre>
VBO vertex format (6 bytes per vertex):<br>
  +0 s14.2 x<br>
  +2 s14.2 y<br>
  +4 i16   index (when !defined(USE_VERTEX_ATTRIB_DIVISOR))<br>
</pre>
*/
YF void YAC_CALL sdvg_DrawLineStripFlatAAVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawLineStripFlatAAVBO32,int vboId,int byteOffset,int numPoints
Draw previously prepared vertex buffer as anti-aliased line strip (32 bit float format)
*/
YF void YAC_CALL sdvg_DrawLineStripFlatAAVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawLineStripFlatBevelVBO14_2,int vboId,int byteOffset,int numPoints
Draw previously prepared vertex buffer as line strip with bevel line joints (14.2 fixed point format)

<pre>
VBO vertex format (6 bytes per vertex):<br>
  +0 s14.2 x<br>
  +2 s14.2 y<br>
  +4 i16   index (when !defined(USE_VERTEX_ATTRIB_DIVISOR))<br>
</pre>
*/
YF void YAC_CALL sdvg_DrawLineStripFlatBevelVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawLineStripFlatBevelVBO32,int vboId,int byteOffset,int numPoints
Draw previously prepared vertex buffer as line strip with bevel line joints (32 bit float format)
*/
YF void YAC_CALL sdvg_DrawLineStripFlatBevelVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawLineStripFlatBevelAAVBO14_2,int vboId,int byteOffset,int numPoints
Draw previously prepared vertex buffer as anti-aliased line strip with bevel line joints (14.2 fixed point format)

<pre>
VBO vertex format (6 bytes per vertex):<br>
  +0 s14.2 x<br>
  +2 s14.2 y<br>
  +4 i16   index (when !defined(USE_VERTEX_ATTRIB_DIVISOR))<br>
</pre>
*/
YF void YAC_CALL sdvg_DrawLineStripFlatBevelAAVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawLineStripFlatBevelAAVBO32,int vboId,int byteOffset,int numPoints
Draw previously prepared vertex buffer as anti-aliased line strip with bevel line joints (32 bit float format)
*/
YF void YAC_CALL sdvg_DrawLineStripFlatBevelAAVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawLinesFlatVBO14_2,int vboId,int byteOffset,int numPoints
Draw previously prepared vertex buffer as line segments (14.2 fixed point format)

<pre>
VBO vertex format (6 bytes per vertex):<br>
  +0 s14.2 x<br>
  +2 s14.2 y<br>
  +4 i16   index (when !defined(USE_VERTEX_ATTRIB_DIVISOR))<br>
</pre>
*/
YF void YAC_CALL sdvg_DrawLinesFlatVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawLinesFlatVBO32,int vboId,int byteOffset,int numPoints
Draw previously prepared vertex buffer as line segments (32 bit float format)
*/
YF void YAC_CALL sdvg_DrawLinesFlatVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawLinesFlatAAVBO14_2,int vboId,int byteOffset,int numPoints
Draw previously prepared vertex buffer as anti-aliased line segments (14.2 fixed point format)

<pre>
VBO vertex format (6 bytes per vertex):<br>
  +0 s14.2 x<br>
  +2 s14.2 y<br>
  +4 i16   index (when !defined(USE_VERTEX_ATTRIB_DIVISOR))<br>
</pre>
*/
YF void YAC_CALL sdvg_DrawLinesFlatAAVBO14_2 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawLinesFlatAAVBO32,int vboId,int byteOffset,int numPoints
Draw previously prepared vertex buffer as anti-alised line segments (32 bit float format)
*/
YF void YAC_CALL sdvg_DrawLinesFlatAAVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawPointsSquareVBO32,int vboId,int byteOffset,int numPoints
Draw previously prepared vertex buffer as square points (32 bit float format)

<pre>
VBO vertex format (10 bytes per vertex):<br>
  +0 f32 x<br>
  +4 f32 y<br>
  +8 i16   index (0..5) (when !defined(USE_VERTEX_ATTRIB_DIVISOR))<br>
</pre>
*/
YF void YAC_CALL sdvg_DrawPointsSquareVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawPointsSquareAAVBO32,int vboId,int byteOffset,int numPoints
Draw previously prepared vertex buffer as anti-aliased, square points (32 bit float format)

<pre>
VBO vertex format (10 bytes per vertex):<br>
  +0 f32 x<br>
  +4 f32 y<br>
  +8 i16   index (0..5) (when !defined(USE_VERTEX_ATTRIB_DIVISOR))<br>
</pre>
*/
YF void YAC_CALL sdvg_DrawPointsSquareAAVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawPointsRoundVBO32,int vboId,int byteOffset,int numPoints
Draw previously prepared vertex buffer as round points (32 bit float format)

<pre>
VBO vertex format (10 bytes per vertex):<br>
  +0 f32 x<br>
  +4 f32 y<br>
  +8 i16   index (0..5) (when !defined(USE_VERTEX_ATTRIB_DIVISOR))<br>
</pre>
*/
YF void YAC_CALL sdvg_DrawPointsRoundVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

/* @function sdvg_DrawPointsRoundAAVBO32,int vboId,int byteOffset,int numPoints
Draw previously prepared vertex buffer as anti-aliased, round points (32 bit float format)

<pre>
VBO vertex format (10 bytes per vertex):<br>
  +0 f32 x<br>
  +4 f32 y<br>
  +8 i16   index (0..5) (when !defined(USE_VERTEX_ATTRIB_DIVISOR))<br>
</pre>
*/
YF void YAC_CALL sdvg_DrawPointsRoundAAVBO32 (sUI _vboId, sUI _byteOffset, sUI _numPoints);

// ----------- custom shaders and (scratch) draw functions ------------
/* @function sdvg_CreateShader,String vs,String fs:int
Create user-defined shader program

@arg vs Vertex shader source
@arg fs Fragment shader source

@return ShaderVG shader index

@see sdvg_DestroyShader
@see sdvg_BindShader
@see sdvg_UnbindShader
*/
sUI YAC_CALL sdvg_CreateShader (const char *vs, const char *fs);

/* @function sdvg_DestroyShader,int shaderIdx
Destroy user-defined shader program

@see sdvg_CreateShader
@see sdvg_BindShader
@see sdvg_UnbindShader
*/
YF void YAC_CALL sdvg_DestroyShader (sUI _shaderIdx);

/* @function sdvg_BindShader,int shaderIdx
Bind user-defined shader program

@see sdvg_CreateShader
@see sdvg_DestroyShader
@see sdvg_UnbindShader
*/
YF void YAC_CALL sdvg_BindShader (sUI _shaderIdx);

/* @function sdvg_UnbindShader
Unbind current shader program

@see sdvg_CreateShader
@see sdvg_DestroyShader
@see sdvg_BindShader
*/
YF void YAC_CALL sdvg_UnbindShader (void);

/* @function sdvg_GetUniformLocation,String name:int
Query uniform location in currently bound shader program
*/
sSI YAC_CALL sdvg_GetUniformLocation (const char *_name);

/* @function sdvg_GetAttribLocation,String name:int
Query attribute location in currently bound shader program
*/
sSI YAC_CALL sdvg_GetAttribLocation (const char *_name);

/* @function sdvg_Uniform1f,String name,float f
Set scalar float uniform in currently bound shader program
*/
void YAC_CALL sdvg_Uniform1f (const char *_name, sF32 _f);

/* @function sdvg_Uniform2f,String name,float f1,float f2
Set vec2 float uniform in currently bound shader program
*/
void YAC_CALL sdvg_Uniform2f (const char *_name, sF32 _f1, sF32 _f2);

/* @function sdvg_Uniform3f,String name,float f1,float f2,float f3
Set vec3 float uniform in currently bound shader program
*/
void YAC_CALL sdvg_Uniform3f (const char *_name, sF32 _f1, sF32 _f2, sF32 _f3);

/* @function sdvg_Uniform4f,String name,float f1,float f2,float f3,float f4
Set vec4 float uniform in currently bound shader program
*/
void YAC_CALL sdvg_Uniform4f (const char *_name, sF32 _f1, sF32 _f2, sF32 _f3, sF32 _f4);

/* @function sdvg_Uniform1i,String name,int i
Set integer / sampler uniform in currently bound shader program
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

/* @function sdvg_PaintSolid
Select solid paint

@see sdvg_PaintLinear
@see sdvg_PaintRadial
@see sdvg_PaintConic
*/
YF void YAC_CALL sdvg_PaintSolid (void);

/* @function sdvg_PaintLinear,float startX,float startY,float endX,float endY
Select linear paint

@see sdvg_PaintSolid
@see sdvg_PaintRadial
@see sdvg_PaintConic
*/
YF void YAC_CALL sdvg_PaintLinear (sF32 _startX, sF32 _startY, sF32 _endX, sF32 _endY);

/* @function sdvg_PaintRadial,float startX,float startY,float radiusX,float radiusY
Select radial paint

@see sdvg_PaintSolid
@see sdvg_PaintLinear
@see sdvg_PaintConic
*/
YF void YAC_CALL sdvg_PaintRadial (sF32 _startX, sF32 _startY, sF32 _radiusX, sF32 _radiusY);

/* @function sdvg_PaintConic,float startX,float startY,float radiusX,float radiusY,float angle01
Select conic paint

@arg startX
@arg startY
@arg radiusX
@arg radiusY
@arg angle01 Normalized start angle (0..1 => 0..360 degrees). 0=north

@see sdvg_PaintSolid
@see sdvg_PaintLinear
@see sdvg_PaintRadial
*/
YF void YAC_CALL sdvg_PaintConic (sF32 _startX, sF32 _startY, sF32 _radiusX, sF32 _radiusY, sF32 _angle01);

/* @function sdvg_BeginVBO,int numVertices,int stride:boolean
Begin preparation of mapped vertex buffer

@arg numVertices Number of vertices
@arg stride Total number of attribute bytes per vertex

@see sdvg_MapVBO
*/
YF sBool YAC_CALL sdvg_BeginVBO (sUI _numVertices, sUI _stride);

/* @function sdvg_BeginTriangles,int numVertices,int stride:boolean
Begin preparation or rendering of triangles via user-defined shader

@arg numVertices Number of vertices
@arg stride Total number of attribute bytes per vertex
*/
YF sBool YAC_CALL sdvg_BeginTriangles (sUI _numVertices, sUI _stride);

/* @function sdvg_BeginTriangleFan,int numVertices,int stride:boolean
Begin preparation or rendering of triangle-fan via user-defined shader

@arg numVertices Number of vertices
@arg stride Total number of attribute bytes per vertex
*/
YF sBool YAC_CALL sdvg_BeginTriangleFan (sUI _numVertices, sUI _stride);

/* @function sdvg_BeginTriangleStrip,int numVertices,int stride:boolean
Begin preparation or rendering of triangle-strip via user-defined shader

@arg numVertices Number of vertices
@arg stride Total number of attribute bytes per vertex
*/
YF sBool YAC_CALL sdvg_BeginTriangleStrip (sUI _numVertices, sUI _stride);

/* @function sdvg_BeginFilledTriangles,int numVertices:boolean
Begin preparation or rendering of filled triangles

@arg numVertices Number of vertices
*/
YF sBool YAC_CALL sdvg_BeginFilledTriangles (sUI _numVertices);

/* @function sdvg_BeginFilledTriangleFan,int numVertices:boolean
Begin preparation or rendering of filled triangle-fan

@arg numVertices Number of vertices
*/
YF sBool YAC_CALL sdvg_BeginFilledTriangleFan (sUI _numVertices);

/* @function sdvg_BeginFilledTriangleStrip,int numVertices:boolean
Begin preparation or rendering of filled triangle-strip

@arg numVertices Number of vertices
*/
YF sBool YAC_CALL sdvg_BeginFilledTriangleStrip (sUI _numVertices);

/* @function sdvg_BeginFilledGouraudTriangles,int numVertices:boolean
Begin preparation or rendering of filled, gouraud shaded triangles

@arg numVertices Number of vertices
*/
YF sBool YAC_CALL sdvg_BeginFilledGouraudTriangles (sUI _numVertices);

/* @function sdvg_BeginFilledGouraudTriangleFan,int numVertices:boolean
Begin preparation or rendering of filled, gouraud shaded triangle-fan

@arg numVertices Number of vertices
*/
YF sBool YAC_CALL sdvg_BeginFilledGouraudTriangleFan (sUI _numVertices);

/* @function sdvg_BeginFilledGouraudTriangleStrip,int numVertices:boolean
Begin preparation or rendering of filled, gouraud shaded triangle-strip

@arg numVertices Number of vertices
*/
YF sBool YAC_CALL sdvg_BeginFilledGouraudTriangleStrip (sUI _numVertices);

/* @function sdvg_BeginTexturedTriangles,int numVertices:boolean
Begin preparation or rendering of textured triangles

@arg numVertices Number of vertices
*/
YF sBool YAC_CALL sdvg_BeginTexturedTriangles (sUI _numVertices);

/* @function sdvg_BeginTexturedTriangleFan,int numVertices:boolean
Begin preparation or rendering of textured triangle fan

@arg numVertices Number of vertices
*/
YF sBool YAC_CALL sdvg_BeginTexturedTriangleFan (sUI _numVertices);

/* @function sdvg_BeginTexturedTriangleStrip,int numVertices:boolean
Begin preparation or rendering of textured triangle strip

@arg numVertices Number of vertices
*/
YF sBool YAC_CALL sdvg_BeginTexturedTriangleStrip (sUI _numVertices);

/* @function sdvg_BeginTexturedGouraudTriangles,int numVertices:boolean
Begin preparation or rendering of textured, gouraud shaded triangles

@arg numVertices Number of vertices
*/
YF sBool YAC_CALL sdvg_BeginTexturedGouraudTriangles (sUI _numVertices);

/* @function sdvg_BeginTexturedGouraudTriangleFan,int numVertices:boolean
Begin preparation or rendering of textured, gouraud shaded triangle-fan

@arg numVertices Number of vertices
*/
YF sBool YAC_CALL sdvg_BeginTexturedGouraudTriangleFan (sUI _numVertices);

/* @function sdvg_BeginTexturedGouraudTriangleStrip,int numVertices:boolean
Begin preparation or rendering of textured, gouraud shaded triangle-strip

@arg numVertices Number of vertices
*/
YF sBool YAC_CALL sdvg_BeginTexturedGouraudTriangleStrip (sUI _numVertices);

/* @function sdvg_BeginTexturedTrianglesAlpha,int numVertices:boolean
Begin preparation or rendering of alpha-channel-only textured triangles

@arg numVertices Number of vertices
*/
YF sBool YAC_CALL sdvg_BeginTexturedTrianglesAlpha (sUI _numVertices);

/* @function sdvg_BeginTexturedTriangleFanAlpha,int numVertices:boolean
Begin preparation or rendering of alpha-channel-only textured triangle-fan

@arg numVertices Number of vertices
*/
YF sBool YAC_CALL sdvg_BeginTexturedTriangleFanAlpha (sUI _numVertices);

/* @function sdvg_BeginTexturedTriangleStripAlpha,int numVertices:boolean
Begin preparation or rendering of alpha-channel-only textured triangle-strip

@arg numVertices Number of vertices
*/
YF sBool YAC_CALL sdvg_BeginTexturedTriangleStripAlpha (sUI _numVertices);

/* @function sdvg_BeginTexturedGouraudTrianglesAlpha,int numVertices:boolean
Begin preparation or rendering of alpha-channel-only textured, gouraud shaded triangles

@arg numVertices Number of vertices
*/
YF sBool YAC_CALL sdvg_BeginTexturedGouraudTrianglesAlpha (sUI _numVertices);

/* @function sdvg_BeginTexturedGouraudTriangleFanAlpha,int numVertices:boolean
Begin preparation or rendering of alpha-channel-only textured, gouraud shaded triangle-fan

@arg numVertices Number of vertices
*/
YF sBool YAC_CALL sdvg_BeginTexturedGouraudTriangleFanAlpha (sUI _numVertices);

/* @function sdvg_BeginTexturedGouraudTriangleStripAlpha,int numVertices:boolean
Begin preparation or rendering of alpha-channel-only textured, gouraud shaded triangle-strip

@arg numVertices Number of vertices
*/
YF sBool YAC_CALL sdvg_BeginTexturedGouraudTriangleStripAlpha (sUI _numVertices);

/* @function sdvg_BeginTexturedTrianglesAlphaSDF,int numVertices:boolean
Begin preparation or rendering of alpha-SDF-channel-only textured triangles

@arg numVertices Number of vertices

@see sdvg_SetAlphaSDFRange
*/
YF sBool YAC_CALL sdvg_BeginTexturedTrianglesAlphaSDF (sUI _numVertices);

/* @function sdvg_BeginLineStrip,int numPoints:boolean
Begin preparation or rendering of line strip

@arg numPoints Number of vertices
*/
YF sBool YAC_CALL sdvg_BeginLineStrip (sUI _numPoints);

/* @function sdvg_BeginLineStripAA,int numPoints:boolean
Begin preparation or rendering of anti-aliased line strip

@arg numPoints Number of points
*/
YF sBool YAC_CALL sdvg_BeginLineStripAA (sUI _numPoints);

/* @function sdvg_BeginLineStripBevel,int numPoints:boolean
Begin preparation or rendering of line strip with bevel line joints

@arg numPoints Number of points
*/
YF sBool YAC_CALL sdvg_BeginLineStripBevel (sUI _numPoints);

/* @function sdvg_BeginLineStripBevelAA,int numPoints:boolean
Begin preparation or rendering of anti-aliased line strip with bevel line joints

@arg numPoints Number of points
*/
YF sBool YAC_CALL sdvg_BeginLineStripBevelAA (sUI _numPoints);

/* @function sdvg_BeginLines,int numPoints:boolean
Begin preparation or rendering of line segments

@arg numPoints Number of points
*/
YF sBool YAC_CALL sdvg_BeginLines (sUI _numPoints);

/* @function sdvg_BeginLinesAA,int numPoints:boolean
Begin preparation or rendering of anti-aliased line segments

@arg numPoints Number of points
*/
YF sBool YAC_CALL sdvg_BeginLinesAA (sUI _numPoints);

/* @function sdvg_BeginPointsSquare,int numPoints:boolean
Begin preparation or rendering of square points

@arg numPoints Number of points
*/
YF sBool YAC_CALL sdvg_BeginPointsSquare (sUI _numPoints);

/* @function sdvg_BeginPointsSquareAA,int numPoints:boolean
Begin preparation or rendering of anti-aliased, square points

@arg numPoints Number of points
*/
YF sBool YAC_CALL sdvg_BeginPointsSquareAA (sUI _numPoints);

/* @function sdvg_BeginPointsRound,int numPoints:boolean
Begin preparation or rendering of round points

@arg numPoints Number of points
*/
YF sBool YAC_CALL sdvg_BeginPointsRound (sUI _numPoints);

/* @function sdvg_BeginPointsRoundAA,int numPoints:boolean
Begin preparation or rendering of anti-aliased, round points

@arg numPoints Number of points
*/
YF sBool YAC_CALL sdvg_BeginPointsRoundAA (sUI _numPoints);

/* @function sdvg_VertexOffset2f
Set vertex coordinate buffer GPU read pointer to current write offset
*/
YF void YAC_CALL sdvg_VertexOffset2f (void);

/* @function sdvg_AttribOffsetf,String name,int size
Set vertex attribute buffer GPU read pointer to current write offset (n floats)

@arg name Attribute name
*/
void YAC_CALL sdvg_AttribOffsetf (const char *_name, sUI _size);

/* @function sdvg_AttribOffset1f,String name
Set vertex attribute buffer GPU read pointer to current write offset (1 float per vertex)

@arg name Attribute name
*/
void YAC_CALL sdvg_AttribOffset1f (const char *_name);

/* @function sdvg_AttribOffset2f,String name
Set vertex attribute buffer GPU read pointer to current write offset (2 floats per vertex)

@arg name Attribute name
*/
void YAC_CALL sdvg_AttribOffset2f (const char *_name);

/* @function sdvg_AttribOffset3f,String name
Set vertex attribute buffer GPU read pointer to current write offset (3 floats per vertex)

@arg name Attribute name
*/
void YAC_CALL sdvg_AttribOffset3f (const char *_name);

/* @function sdvg_AttribOffset4f,String name
Set vertex attribute buffer GPU read pointer to current write offset (4 floats per vertex)

@arg name Attribute name
*/
void YAC_CALL sdvg_AttribOffset4f (const char *_name);

/* @function sdvg_AttribOffsetARGB,String name
Set vertex attribute buffer GPU read pointer to current write offset (4 r,g,b,a bytes per vertex)

@arg name Attribute name
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
*/
void YAC_CALL sdvg_Attribi16 (sS16 _i);

/* @function sdvg_Attrib1f,float f
Emit 32bit float vertex attribute to currently mapped vertex buffer
*/
YF void YAC_CALL sdvg_Attrib1f (sF32 _f);

/* @function sdvg_Attrib2f,float f1,float f2
Emit two 32bit float vertex attributes to currently mapped vertex attribute buffer
*/
YF void YAC_CALL sdvg_Attrib2f (sF32 _f1, sF32 _f2);

/* @function sdvg_Vertex2f,float x,float y
Emit 2-component (xy) 32bit float vertex coordinate to currently mapped vertex buffer
*/
YF void YAC_CALL sdvg_Vertex2f (sF32 _x, sF32 _y);

/* @function sdvg_Vertex3f,float x,float y,float z
Emit 3-component (xyz) 32bit float vertex coordinate to currently mapped vertex buffer
*/
YF void YAC_CALL sdvg_Vertex3f (sF32 _x, sF32 _y, sF32 _z);

/* @function sdvg_TexCoord2f,float u,float v
Emit 2-component 32bit float UV coordinate to currently mapped vertex buffer
*/
YF void YAC_CALL sdvg_TexCoord2f (sF32 _u, sF32 _v);

/* @function sdvg_Attrib3f,float f1,float f2,float f3
Emit 3-component 32bit float vertex attribute to currently mapped vertex buffer
*/
YF void YAC_CALL sdvg_Attrib3f (sF32 _f1, sF32 _f2, sF32 _f3);

/* @function sdvg_Color3f,float r,float g,float b
Emit 3-component 32bit float RGB color attribute to currently mapped vertex buffer
*/
YF void YAC_CALL sdvg_Color3f (sF32 _r, sF32 _g, sF32 _b);

/* @function sdvg_Attrib4f,float f1,float f2,float f3,float f4
Emit 4-component 32bit float vertex attribute to currently mapped vertex buffer
*/
YF void YAC_CALL sdvg_Attrib4f (sF32 _f1, sF32 _f2, sF32 _f3, sF32 _f4);

/* @function sdvg_Color4f,float r,float g,float b,float a
Emit 4-component 32bit float RGBA color attribute to currently mapped vertex buffer
*/
YF void YAC_CALL sdvg_Color4f (sF32 _r, sF32 _g, sF32 _b, sF32 _a);

/* @function sdvg_AttribARGB,int c32
Emit 4-component unsigned byte r,g,b,a color attribute to currently mapped vertex buffer
*/
YF void YAC_CALL sdvg_AttribARGB (sUI _c32);

/* @function sdvg_ColorARGB,int c32
Emit 4-component unsigned byte r,g,b,a color attribute to currently mapped vertex buffer
*/
YF void YAC_CALL sdvg_ColorARGB (sUI _c32);

/* @function sdvg_End
Finalize vertex buffer and start rendering (unless buffer is currently mapped).
*/
YF void YAC_CALL sdvg_End (void);

// ----------- additional (scratch) draw functions ------------
/* @function sdvg_DrawFilledRectangle,float x,float y,float w,float h
Render a filled rectangle via scratch buffer
*/
YF void YAC_CALL sdvg_DrawFilledRectangle (sF32 _x, sF32 _y, sF32 _w, sF32 _h);

/* @function sdvg_DrawRectangle,float x,float y,float w,float h,float b
Render a stroked rectangle via scratch buffer

@arg x X position
@arg y Y position
@arg w Width
@arg h Height
@arg b Border width
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
*/
sU32 sdvg_ARGB (sU8 _a, sU8 _r, sU8 _g, sU8 _b);

/* @function sdvg_ARGBf,float a,float r,float g,float b:int
Compose packed ARGB32 color from normalized a,r,g,b floats (0..1)

@arg a Normalized alpha (0..1)
@arg r Normalized red (0..1)
@arg g Normalized green (0..1)
@arg b Normalized blue (0..1)

@return Packed ARGB32 color
*/
YF sU32 YAC_CALL sdvg_ARGBf (sF32 _a, sF32 _r, sF32 _g, sF32 _b);

/* @function sdvg_MixARGBx,int x,int y,short t:int
Mix packed ARGB32 colors (fixed point weight)

@arg x First packed ARGB32 color
@arg y Second packed ARGB32 color
@arg t 0=x .. 256=y
@return Packed RGB24 color
*/
sU32 sdvg_MixARGBx (sU32 _x, sU32 _y, sU16 _t);

/* @function sdvg_MixARGBf,int x,int y,float t:int
Mix packed ARGB32 colors (normalized float weight)

@arg x First packed ARGB32 color
@arg y Second packed ARGB32 color
@arg t 0.0=x .. 1.0=y

@return Packed RGB24 color
*/
YF sU32 YAC_CALL sdvg_MixARGBf (sU32 _x, sU32 _y, sF32 _t);

/* @function sdvg_MixRGBx,int x,int y,short t
Mix packed RGB24 colors (fixed point weight)

@arg x First packed RGB24 color
@arg y Second packed RGB24 color
@arg t 0=x .. 256=y

@return Packed RGB24 color
*/
sU32 sdvg_MixRGBx (sU32 _x, sU32 _y, sU16 _t);

/* @function sdvg_MixRGBf,int x,int y,float t:int
Mix packed RGB24 colors (normalized float weight)

@arg x First packed RGB24 color
@arg y Second packed RGB24 color
@arg t 0.0=x .. 1.0=y

@return Packed RGB24 color
*/
YF sU32 YAC_CALL sdvg_MixRGBf (sU32 _x, sU32 _y, sF32 _t);

/* @function sdvg_TintARGB,int x,int y:int
Tint color

@arg x Packed ARGB32 base color
@arg y Packed ARGB32 tint color. Alpha channel determines blend amount.

@return Packed ARGB32 color
*/
YF sU32 YAC_CALL sdvg_TintARGB (sU32 _x, sU32 _y);

/* @function sdvg_TintRGBAlpha,int x,int y,byte a8:int
Tint color

@arg x Packed RGB24 base color
@arg y Packed RGB24 tint color. Alpha channel determines blend amount.
@arg a8 Alpha channel (0..25) to be inserted into returned color

@return Packed ARGB32 color
*/
sU32 sdvg_TintRGBAlpha (sU32 _x, sU32 _y, sU8 _a8);

/* @function sdvg_RGBAlpha,int c32,byte a8:int
Replace alpha channel of packed ARGB32 color

@arg c32 Packed RGB24 color
@arg a8 Alpha channel (0..25) to be inserted into returned color

@return Packed ARGB32 color
*/
sU32 sdvg_RGBAlpha (sU32 _c32, sU8 _a8);

/* @function sdvg_HSVAToARGB,float h,float s,float v,byte a8:int
Convert hue / saturation / value / alpha into packed ARGB32 color

@arg h Hue (0..360)
@arg s Saturation (0..1)
@arg v Value (0..1)
@arg a8 Alpha channel (0..255) to be inserted into returned color

@return Packed ARGB32 color
*/
sU32 sdvg_HSVAToARGB (sF32 _h, sF32 _s, sF32 _v, sU8 _a8);

/* @function sdvg_ARGBToHSVA,int c32,Float retH,Float retS,Float retV:byte
Split packed ARGB32 color into hue / saturation / value / alpha components

@arg c32 Packed ARGB32 color
@arg retH Returns hue (0..360)
@arg retS Returns saturation (0..1)
@arg retV Returns value (0..1)

@return Alpha channel (0..255)
*/
sU8 sdvg_ARGBToHSVA (sU32 _c32, sF32 *_retH, sF32 *_retS, sF32 *_retV);

/* @function sdvg_GradientToTexture,Texture dst,IntArray colors,IntArray starts
Convert gradient (colors + start positions) to ARGB32 texture.

The first start position must be 0, and the last position determines the total gradient size.

@arg dst Destination texture. Allocation size (at least 1) determines resolution of interpolated gradient.
@arg colors ARGB32 color array. Number of elements must be at least two and determines number of gradient entries.
@arg starts Color start positions. Number of elements must be greater or equal to 'colors' array size.
@arg bSmoothStep false=linear interpolation  true=smoothstep interpolation
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


#endif // SHADERVG_SKIP_DECLARATIONS


#endif // SHADERVG_H__
