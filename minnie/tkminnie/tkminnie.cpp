// ----
// ---- file   : tkminnie.cpp
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : Distributed under terms of the MIT license (https://opensource.org/licenses/MIT)
// ----          Copyright 2024-2026 by bsp
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
// ---- info   : "minnie" binary loader + renderer script wrapper
// ---- note   :
// ----
// ----
// ----

#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#define YAC_BIGSTRING
#define YAC_PRINTF
#include <yac.h>
YAC_Host *yac_host;

#define MINNIE_SKIP_TYPEDEFS defined
#define MINNIE_IMPLEMENTATION defined
#include "../minnie.h"

#include "tkminnie.h"
#include "../MinnieDrawable.h"

#include "../inc_opengl.h"

#include "../shadervg/shadervg.h"
#include "../shadervg/shadervg_internal.h"
#include "../shadervg/Shader.h"
#include "../shadervg/Shape.h"
#include "../shadervg/text.h"

#include "../minnie_exec.h"

#include "ying_minnie.h"
#include "ying_minnie_MinnieDrawable.cpp"
#include "ying_minnie.cpp"
#include "ying_shadervg.h"
#include "ying_shadervg.cpp"
#include "ying_shadervg__sdvg_constants_class.cpp"
#include "ying_shadervg_ShaderVG_Font.cpp"


// Implement standard exception ID variables (see yac.h)
Dyac_std_exid_impl;


#include "../../tkopengl/tkopengl_shared.cpp"


// ---------------------------------------------------------------------------
#include "tkminnie_shared.h"
static YAC_CallbackId tkminnie_shared_callback_id;
static tkminnie_shared_t tkminnie_shared;

// --------------------------------------------------------------------------- loc_shared_export()
static void loc_shared_export(void) {
   tkminnie_shared_callback_id = yac_host->yacCallbackCreate(TKMINNIE_SHARED_NAME);
   // Dyac_host_printf("[trc] tkminnie:loc_shared_export: export TKMINNIE_SHARED_NAME=\"%s\" => callback_id=%d\n", TKMINNIE_SHARED_NAME, tkminnie_shared_callback_id);
   // -------- init/exit --------
   tkminnie_shared.sdvg_SetGLSLVersion       = &sdvg_SetGLSLVersion;
   tkminnie_shared.sdvg_SetScratchBufferSize = &sdvg_SetScratchBufferSize;
   tkminnie_shared.sdvg_Init                 = &sdvg_Init;
   tkminnie_shared.sdvg_Exit                 = &sdvg_Exit;
   tkminnie_shared.sdvg_OnOpen               = &sdvg_OnOpen;
   tkminnie_shared.sdvg_SetFramebufferSize   = &sdvg_SetFramebufferSize;
   // -------- begin/end frame --------
   tkminnie_shared.sdvg_BeginFrame = &sdvg_BeginFrame;
   tkminnie_shared.sdvg_Flush      = &sdvg_Flush;
   tkminnie_shared.sdvg_ReturnToGL = &sdvg_ReturnToGL;
   tkminnie_shared.sdvg_EndFrame   = &sdvg_EndFrame;
   // -------- viewport --------
   tkminnie_shared.sdvg_SetViewport       = &sdvg_SetViewport;
   tkminnie_shared.sdvg_PushViewport      = &sdvg_PushViewport;
   tkminnie_shared.sdvg_PopViewport       = &sdvg_PopViewport;
   tkminnie_shared.sdvg_GetViewportWidth  = &sdvg_GetViewportWidth;
   tkminnie_shared.sdvg_GetViewportHeight = &sdvg_GetViewportHeight;
   tkminnie_shared.sdvg_SetPixelScaling   = &sdvg_SetPixelScaling;
   // -------- scissor --------
   tkminnie_shared.sdvg_SetScissor     = &sdvg_SetScissor;
   tkminnie_shared.sdvg_PushScissor    = &sdvg_PushScissor;
   tkminnie_shared.sdvg_PopScissor     = &sdvg_PopScissor;
   tkminnie_shared.sdvg_EnableScissor  = &sdvg_EnableScissor;
   tkminnie_shared.sdvg_DisableScissor = &sdvg_DisableScissor;
   // -------- transform --------
   tkminnie_shared.sdvg_SetTransform    = &_sdvg_SetTransform;
   tkminnie_shared.sdvg_GetTransformRef = &_sdvg_GetTransformRef;
   // -------- transform (matrix stack) --------
   tkminnie_shared.sdvg_PushProjMatrix    = &sdvg_PushProjMatrix;
   tkminnie_shared.sdvg_PopProjMatrix     = &sdvg_PopProjMatrix;
   tkminnie_shared.sdvg_PushModelMatrix   = &sdvg_PushModelMatrix;
   tkminnie_shared.sdvg_PopModelMatrix    = &sdvg_PopModelMatrix;
   tkminnie_shared.sdvg_ProjInitIdentity  = &sdvg_ProjInitIdentity;
   tkminnie_shared.sdvg_ProjInit2D        = &sdvg_ProjInit2D;
   tkminnie_shared.sdvg_ProjInitOrtho     = &sdvg_ProjInitOrtho;
   tkminnie_shared.sdvg_ProjTranslate2f   = &sdvg_ProjTranslate2f;
   tkminnie_shared.sdvg_ModelInitIdentity = &sdvg_ModelInitIdentity;
   tkminnie_shared.sdvg_ModelTranslate2f  = &sdvg_ModelTranslate2f;
   tkminnie_shared.sdvg_ModelTranslate3f  = &sdvg_ModelTranslate3f;
   tkminnie_shared.sdvg_ModelScale2f      = &sdvg_ModelScale2f;
   tkminnie_shared.sdvg_ModelRotatef      = &sdvg_ModelRotatef;
   // -------- AA --------
   tkminnie_shared.sdvg_SetEnableAA = &sdvg_SetEnableAA;
   tkminnie_shared.sdvg_SetAARange  = &sdvg_SetAARange;
   tkminnie_shared.sdvg_SetAAExp    = &sdvg_SetAAExp;
   tkminnie_shared.sdvg_SetAlphaSDFRange = &sdvg_SetAlphaSDFRange;
   tkminnie_shared.sdvg_SetAlphaSDFExp   = &sdvg_SetAlphaSDFExp;
   // -------- render state (fill / stroke) --------
   tkminnie_shared.sdvg_SetFillColor4f             = &sdvg_SetFillColor4f;
   tkminnie_shared.sdvg_SetFillAlpha               = &sdvg_SetFillAlpha;
   tkminnie_shared.sdvg_SetFillColorARGB           = &sdvg_SetFillColorARGB;
   tkminnie_shared.sdvg_SetFillRuleEvenOdd         = &sdvg_SetFillRuleEvenOdd;
   tkminnie_shared.sdvg_SetFillRuleNonZero         = &sdvg_SetFillRuleNonZero;
   tkminnie_shared.sdvg_SetColor4f                 = &sdvg_SetColor4f;
   tkminnie_shared.sdvg_SetColorARGB               = &sdvg_SetColorARGB;
   tkminnie_shared.sdvg_SetStrokeRadius            = &sdvg_SetStrokeRadius;
   tkminnie_shared.sdvg_SetStrokeWidth             = &sdvg_SetStrokeWidth;
   tkminnie_shared.sdvg_SetStrokeRadiusAAOffset    = &sdvg_SetStrokeRadiusAAOffset;
   tkminnie_shared.sdvg_SetStrokeScale             = &sdvg_SetStrokeScale;
   tkminnie_shared.sdvg_SetLinePatternScale        = &sdvg_SetLinePatternScale;
   tkminnie_shared.sdvg_SetLinePatternOffset       = &sdvg_SetLinePatternOffset;
   tkminnie_shared.sdvg_SetPointRadius             = &sdvg_SetPointRadius;
   tkminnie_shared.sdvg_SetPointSize               = &sdvg_SetPointSize;
   tkminnie_shared.sdvg_SetStrokeColor4f           = &sdvg_SetStrokeColor4f;
   tkminnie_shared.sdvg_SetStrokeColorARGB         = &sdvg_SetStrokeColorARGB;
   tkminnie_shared.sdvg_SetFillAndStrokeColorsARGB = &sdvg_SetFillAndStrokeColorsARGB;
   tkminnie_shared.sdvg_SetGlobalAlpha             = &sdvg_SetGlobalAlpha;
   tkminnie_shared.sdvg_SetDecalAlpha              = &sdvg_SetDecalAlpha;
   // -------- clear --------
   tkminnie_shared.sdvg_Clear4f      = &sdvg_Clear4f;
   tkminnie_shared.sdvg_ClearARGB    = &sdvg_ClearARGB;
   tkminnie_shared.sdvg_ClearStencil = &sdvg_ClearStencil;
   tkminnie_shared.sdvg_ClearDepth   = &sdvg_ClearDepth;
   // -------- blending --------
   tkminnie_shared.sdvg_EnableBlending                  = &sdvg_EnableBlending;
   tkminnie_shared.sdvg_EnableBlendingKeepAlpha         = &sdvg_EnableBlendingKeepAlpha;
   tkminnie_shared.sdvg_EnableBlendingAdditive          = &sdvg_EnableBlendingAdditive;
   tkminnie_shared.sdvg_EnableBlendingAdditiveKeepAlpha = &sdvg_EnableBlendingAdditiveKeepAlpha;
   tkminnie_shared.sdvg_EnableBlendingSrcColorKeepAlpha = &sdvg_EnableBlendingSrcColorKeepAlpha;
   tkminnie_shared.sdvg_EnableBlendingDstColorKeepAlpha = &sdvg_EnableBlendingDstColorKeepAlpha;
   tkminnie_shared.sdvg_DisableBlending                 = &sdvg_DisableBlending;
   tkminnie_shared.sdvg_AlphaWrite                      = &sdvg_AlphaWrite;
   // -------- Stencil --------
   tkminnie_shared.sdvg_EnableStencilMask  = &sdvg_EnableStencilMask;
   tkminnie_shared.sdvg_DisableStencilMask = &sdvg_DisableStencilMask;
   // ----------- custom shaders ------------
   tkminnie_shared.sdvg_CreateShader       = &sdvg_CreateShader;
   tkminnie_shared.sdvg_DestroyShader      = &sdvg_DestroyShader;
   tkminnie_shared.sdvg_BindShader         = &sdvg_BindShader;
   tkminnie_shared.sdvg_UnbindShader       = &sdvg_UnbindShader;
   tkminnie_shared.sdvg_GetUniformLocation = &sdvg_GetUniformLocation;
   tkminnie_shared.sdvg_GetAttribLocation  = &sdvg_GetAttribLocation;
   tkminnie_shared.sdvg_Uniform1f          = &sdvg_Uniform1f;
   tkminnie_shared.sdvg_Uniform2f          = &sdvg_Uniform2f;
   tkminnie_shared.sdvg_Uniform3f          = &sdvg_Uniform3f;
   tkminnie_shared.sdvg_Uniform4f          = &sdvg_Uniform4f;
   tkminnie_shared.sdvg_Uniform1i          = &sdvg_Uniform1i;
   // -------- paint --------
   tkminnie_shared.sdvg_PaintSolid             = &sdvg_PaintSolid;
   tkminnie_shared.sdvg_PaintLinear            = &sdvg_PaintLinear;
   tkminnie_shared.sdvg_PaintRadial            = &sdvg_PaintRadial;
   tkminnie_shared.sdvg_PaintConic             = &sdvg_PaintConic;
   tkminnie_shared.sdvg_PaintPattern           = &sdvg_PaintPattern;
   tkminnie_shared.sdvg_PaintPatternAlpha      = &sdvg_PaintPatternAlpha;
   tkminnie_shared.sdvg_PaintPatternDecal      = &sdvg_PaintPatternDecal;
   tkminnie_shared.sdvg_PaintPatternDecalAlpha = &sdvg_PaintPatternDecalAlpha;
   // -------- VBO --------
   tkminnie_shared.sdvg_CreateVBO          = &sdvg_CreateVBO;
   tkminnie_shared.sdvg_UpdateVBO          = &sdvg_UpdateVBO;
   tkminnie_shared.sdvg_BindVBO            = &sdvg_BindVBO;
   tkminnie_shared.sdvg_GetVBOSize         = &sdvg_GetVBOSize;
   tkminnie_shared.sdvg_GetMappedVBOOffset = &sdvg_GetMappedVBOOffset;
   tkminnie_shared.sdvg_GetMappedVBORef    = &sdvg_GetMappedVBORef;
   tkminnie_shared.sdvg_MapVBO             = &sdvg_MapVBO;
   tkminnie_shared.sdvg_UnmapVBO           = &sdvg_UnmapVBO;
   tkminnie_shared.sdvg_UnbindVBO          = &sdvg_UnbindVBO;
   tkminnie_shared.sdvg_DestroyVBO         = &sdvg_DestroyVBO;
   // -------- FBO --------
   tkminnie_shared.sdvg_CreateFBOEx     = &sdvg_CreateFBOEx;
   tkminnie_shared.sdvg_CreateFBO       = &sdvg_CreateFBO;
   tkminnie_shared.sdvg_BindFBO         = &sdvg_BindFBO;
   tkminnie_shared.sdvg_ResolveFBO      = &sdvg_ResolveFBO;
   tkminnie_shared.sdvg_UnbindFBO       = &sdvg_UnbindFBO;
   tkminnie_shared.sdvg_GetFBOTextureId = &sdvg_GetFBOTextureId;
   tkminnie_shared.sdvg_BindFBOTexture  = &sdvg_BindFBOTexture;
   tkminnie_shared.sdvg_DestroyFBO      = &sdvg_DestroyFBO;
   // -------- texturing --------
   tkminnie_shared.sdvg_CreateTexture2D      = &sdvg_CreateTexture2D;
   tkminnie_shared.sdvg_UpdateTexture2D      = &sdvg_UpdateTexture2D;
   tkminnie_shared.sdvg_BindMultiTexture2D   = &sdvg_BindMultiTexture2D;
   tkminnie_shared.sdvg_UnbindMultiTexture2D = &sdvg_UnbindMultiTexture2D;
   tkminnie_shared.sdvg_BindTexture2D        = &sdvg_BindTexture2D;
   tkminnie_shared.sdvg_UnbindTexture2D      = &sdvg_UnbindTexture2D;
   tkminnie_shared.sdvg_DestroyTexture2D     = &sdvg_DestroyTexture2D;
   // -------- VBO utility functions --------
   tkminnie_shared.sdvg_BufferRectTexUVFlat32    = &sdvg_BufferRectTexUVFlat32;
   tkminnie_shared.sdvg_WriteC32AsRGBA8          = &sdvg_WriteC32AsRGBA8;
   tkminnie_shared.sdvg_BufferRectTexUVGouraud32 = &sdvg_BufferRectTexUVGouraud32;
   // ----------- (scratchbuffer) draw functions ------------
   tkminnie_shared.sdvg_BeginVBO     = &sdvg_BeginVBO;
   tkminnie_shared.sdvg_BeginVBO32   = &sdvg_BeginVBO32;
   tkminnie_shared.sdvg_BeginVBO14_2 = &sdvg_BeginVBO14_2;
   tkminnie_shared.sdvg_BeginTriangles     = &sdvg_BeginTriangles;
   tkminnie_shared.sdvg_BeginTriangles32   = &sdvg_BeginTriangles32;
   tkminnie_shared.sdvg_BeginTriangles14_2 = &sdvg_BeginTriangles14_2;
   tkminnie_shared.sdvg_BeginTriangleFan     = &sdvg_BeginTriangleFan;
   tkminnie_shared.sdvg_BeginTriangleFan32   = &sdvg_BeginTriangleFan32;
   tkminnie_shared.sdvg_BeginTriangleFan14_2 = &sdvg_BeginTriangleFan14_2;
   tkminnie_shared.sdvg_BeginTriangleStrip   = &sdvg_BeginTriangleStrip;
   tkminnie_shared.sdvg_BeginTriangleStrip32 = &sdvg_BeginTriangleStrip32;
   tkminnie_shared.sdvg_BeginTriangleStrip14_2 = &sdvg_BeginTriangleStrip14_2;
   tkminnie_shared.sdvg_BeginFilledTriangles     = &sdvg_BeginFilledTriangles;
   tkminnie_shared.sdvg_BeginFilledTriangles32   = &sdvg_BeginFilledTriangles32;
   tkminnie_shared.sdvg_BeginFilledTriangles14_2 = &sdvg_BeginFilledTriangles14_2;
   tkminnie_shared.sdvg_BeginFilledTriangleFan     = &sdvg_BeginFilledTriangleFan;
   tkminnie_shared.sdvg_BeginFilledTriangleFan32   = &sdvg_BeginFilledTriangleFan32;
   tkminnie_shared.sdvg_BeginFilledTriangleFan14_2 = &sdvg_BeginFilledTriangleFan14_2;
   tkminnie_shared.sdvg_BeginFilledTriangleStrip     = &sdvg_BeginFilledTriangleStrip;
   tkminnie_shared.sdvg_BeginFilledTriangleStrip32   = &sdvg_BeginFilledTriangleStrip32;
   tkminnie_shared.sdvg_BeginFilledTriangleStrip14_2 = &sdvg_BeginFilledTriangleStrip14_2;
   tkminnie_shared.sdvg_BeginFilledGouraudTriangles     = &sdvg_BeginFilledGouraudTriangles;
   tkminnie_shared.sdvg_BeginFilledGouraudTriangles32   = &sdvg_BeginFilledGouraudTriangles32;
   tkminnie_shared.sdvg_BeginFilledGouraudTriangles14_2 = &sdvg_BeginFilledGouraudTriangles14_2;
   tkminnie_shared.sdvg_BeginFilledGouraudTriangleFan     = &sdvg_BeginFilledGouraudTriangleFan;
   tkminnie_shared.sdvg_BeginFilledGouraudTriangleFan32   = &sdvg_BeginFilledGouraudTriangleFan32;
   tkminnie_shared.sdvg_BeginFilledGouraudTriangleFan14_2 = &sdvg_BeginFilledGouraudTriangleFan14_2;
   tkminnie_shared.sdvg_BeginFilledGouraudTriangleStrip     = &sdvg_BeginFilledGouraudTriangleStrip;
   tkminnie_shared.sdvg_BeginFilledGouraudModulateTriangles     = &sdvg_BeginFilledGouraudModulateTriangles;
   tkminnie_shared.sdvg_BeginFilledGouraudModulateTriangles32   = &sdvg_BeginFilledGouraudModulateTriangles32;
   tkminnie_shared.sdvg_BeginFilledGouraudModulateTriangles14_2 = &sdvg_BeginFilledGouraudModulateTriangles14_2;
   tkminnie_shared.sdvg_BeginFilledGouraudModulateTriangleFan     = &sdvg_BeginFilledGouraudModulateTriangleFan;
   tkminnie_shared.sdvg_BeginFilledGouraudModulateTriangleFan32   = &sdvg_BeginFilledGouraudModulateTriangleFan32;
   tkminnie_shared.sdvg_BeginFilledGouraudModulateTriangleFan14_2 = &sdvg_BeginFilledGouraudModulateTriangleFan14_2;
   tkminnie_shared.sdvg_BeginFilledGouraudModulateTriangleStrip     = &sdvg_BeginFilledGouraudModulateTriangleStrip;
   tkminnie_shared.sdvg_BeginFilledGouraudModulateTriangleStrip32   = &sdvg_BeginFilledGouraudModulateTriangleStrip32;
   tkminnie_shared.sdvg_BeginFilledGouraudModulateTriangleStrip14_2 = &sdvg_BeginFilledGouraudModulateTriangleStrip14_2;
   tkminnie_shared.sdvg_BeginTexturedTriangles     = &sdvg_BeginTexturedTriangles;
   tkminnie_shared.sdvg_BeginTexturedTriangles32   = &sdvg_BeginTexturedTriangles32;
   tkminnie_shared.sdvg_BeginTexturedTriangles14_2 = &sdvg_BeginTexturedTriangles14_2;
   tkminnie_shared.sdvg_BeginTexturedTriangleFan     = &sdvg_BeginTexturedTriangleFan;
   tkminnie_shared.sdvg_BeginTexturedTriangleFan32   = &sdvg_BeginTexturedTriangleFan32;
   tkminnie_shared.sdvg_BeginTexturedTriangleFan14_2 = &sdvg_BeginTexturedTriangleFan14_2;
   tkminnie_shared.sdvg_BeginTexturedTriangleStrip     = &sdvg_BeginTexturedTriangleStrip;
   tkminnie_shared.sdvg_BeginTexturedTriangleStrip32   = &sdvg_BeginTexturedTriangleStrip32;
   tkminnie_shared.sdvg_BeginTexturedTriangleStrip14_2 = &sdvg_BeginTexturedTriangleStrip14_2;
   tkminnie_shared.sdvg_BeginTexturedGouraudTriangles     = &sdvg_BeginTexturedGouraudTriangles;
   tkminnie_shared.sdvg_BeginTexturedGouraudTriangles32   = &sdvg_BeginTexturedGouraudTriangles32;
   tkminnie_shared.sdvg_BeginTexturedGouraudTriangles14_2 = &sdvg_BeginTexturedGouraudTriangles14_2;
   tkminnie_shared.sdvg_BeginTexturedGouraudTriangleFan     = &sdvg_BeginTexturedGouraudTriangleFan;
   tkminnie_shared.sdvg_BeginTexturedGouraudTriangleFan32   = &sdvg_BeginTexturedGouraudTriangleFan32;
   tkminnie_shared.sdvg_BeginTexturedGouraudTriangleFan14_2 = &sdvg_BeginTexturedGouraudTriangleFan14_2;
   tkminnie_shared.sdvg_BeginTexturedGouraudTriangleStrip     = &sdvg_BeginTexturedGouraudTriangleStrip;
   tkminnie_shared.sdvg_BeginTexturedGouraudTriangleStrip32   = &sdvg_BeginTexturedGouraudTriangleStrip32;
   tkminnie_shared.sdvg_BeginTexturedGouraudTriangleStrip14_2 = &sdvg_BeginTexturedGouraudTriangleStrip14_2;
   tkminnie_shared.sdvg_BeginTexturedTrianglesAlpha     = &sdvg_BeginTexturedTrianglesAlpha;
   tkminnie_shared.sdvg_BeginTexturedTrianglesAlpha32   = &sdvg_BeginTexturedTrianglesAlpha32;
   tkminnie_shared.sdvg_BeginTexturedTrianglesAlpha14_2 = &sdvg_BeginTexturedTrianglesAlpha14_2;
   tkminnie_shared.sdvg_BeginTexturedTriangleFanAlpha     = &sdvg_BeginTexturedTriangleFanAlpha;
   tkminnie_shared.sdvg_BeginTexturedTriangleFanAlpha32   = &sdvg_BeginTexturedTriangleFanAlpha32;
   tkminnie_shared.sdvg_BeginTexturedTriangleFanAlpha14_2 = &sdvg_BeginTexturedTriangleFanAlpha14_2;
   tkminnie_shared.sdvg_BeginTexturedTriangleStripAlpha     = &sdvg_BeginTexturedTriangleStripAlpha;
   tkminnie_shared.sdvg_BeginTexturedTriangleStripAlpha32   = &sdvg_BeginTexturedTriangleStripAlpha32;
   tkminnie_shared.sdvg_BeginTexturedTriangleStripAlpha14_2 = &sdvg_BeginTexturedTriangleStripAlpha14_2;
   tkminnie_shared.sdvg_BeginTexturedGouraudTrianglesAlpha     = &sdvg_BeginTexturedGouraudTrianglesAlpha;
   tkminnie_shared.sdvg_BeginTexturedGouraudTrianglesAlpha32   = &sdvg_BeginTexturedGouraudTrianglesAlpha32;
   tkminnie_shared.sdvg_BeginTexturedGouraudTrianglesAlpha14_2 = &sdvg_BeginTexturedGouraudTrianglesAlpha14_2;
   tkminnie_shared.sdvg_BeginTexturedGouraudTriangleFanAlpha     = &sdvg_BeginTexturedGouraudTriangleFanAlpha;
   tkminnie_shared.sdvg_BeginTexturedGouraudTriangleFanAlpha32   = &sdvg_BeginTexturedGouraudTriangleFanAlpha32;
   tkminnie_shared.sdvg_BeginTexturedGouraudTriangleFanAlpha14_2 = &sdvg_BeginTexturedGouraudTriangleFanAlpha14_2;
   tkminnie_shared.sdvg_BeginTexturedGouraudTriangleStripAlpha     = &sdvg_BeginTexturedGouraudTriangleStripAlpha;
   tkminnie_shared.sdvg_BeginTexturedGouraudTriangleStripAlpha32   = &sdvg_BeginTexturedGouraudTriangleStripAlpha32;
   tkminnie_shared.sdvg_BeginTexturedGouraudTriangleStripAlpha14_2 = &sdvg_BeginTexturedGouraudTriangleStripAlpha14_2;
   tkminnie_shared.sdvg_BeginTexturedTrianglesAlphaSDF     = &sdvg_BeginTexturedTrianglesAlphaSDF;
   tkminnie_shared.sdvg_BeginTexturedTrianglesAlphaSDF32   = &sdvg_BeginTexturedTrianglesAlphaSDF32;
   tkminnie_shared.sdvg_BeginTexturedTrianglesAlphaSDF14_2 = &sdvg_BeginTexturedTrianglesAlphaSDF14_2;
   tkminnie_shared.sdvg_BeginLineStrip        = &sdvg_BeginLineStrip;
   tkminnie_shared.sdvg_BeginLineStripAA      = &sdvg_BeginLineStripAA;
   tkminnie_shared.sdvg_BeginLineStripPattern      = &sdvg_BeginLineStripPattern;
   tkminnie_shared.sdvg_BeginLineStripPatternAA    = &sdvg_BeginLineStripPatternAA;
   tkminnie_shared.sdvg_BeginLineStripPatternDecal = &sdvg_BeginLineStripPatternDecal;
   tkminnie_shared.sdvg_BeginLineStripBevel         = &sdvg_BeginLineStripBevel;
   tkminnie_shared.sdvg_BeginLineStripBevelClosed   = &sdvg_BeginLineStripBevelClosed;
   tkminnie_shared.sdvg_BeginLineStripBevelAA       = &sdvg_BeginLineStripBevelAA;
   tkminnie_shared.sdvg_BeginLineStripBevelAAClosed = &sdvg_BeginLineStripBevelAAClosed;
   tkminnie_shared.sdvg_BeginLineStripPatternBevel         = &sdvg_BeginLineStripPatternBevel;
   tkminnie_shared.sdvg_BeginLineStripPatternBevelClosed   = &sdvg_BeginLineStripPatternBevelClosed;
   tkminnie_shared.sdvg_BeginLineStripPatternBevelAA       = &sdvg_BeginLineStripPatternBevelAA;
   tkminnie_shared.sdvg_BeginLineStripPatternBevelAAClosed = &sdvg_BeginLineStripPatternBevelAAClosed;
   tkminnie_shared.sdvg_BeginLineStripPatternDecalBevel         = &sdvg_BeginLineStripPatternDecalBevel;
   tkminnie_shared.sdvg_BeginLineStripPatternDecalBevelClosed   = &sdvg_BeginLineStripPatternDecalBevelClosed;
   tkminnie_shared.sdvg_BeginLineStripPatternDecalBevelAA       = &sdvg_BeginLineStripPatternDecalBevelAA;
   tkminnie_shared.sdvg_BeginLineStripPatternDecalBevelAAClosed = &sdvg_BeginLineStripPatternDecalBevelAAClosed;
   tkminnie_shared.sdvg_BeginLineStripMiter          = &sdvg_BeginLineStripMiter;
   tkminnie_shared.sdvg_BeginLineStripMiterClosed    = &sdvg_BeginLineStripMiterClosed;
   tkminnie_shared.sdvg_BeginLineStripMiterAA        = &sdvg_BeginLineStripMiterAA;
   tkminnie_shared.sdvg_BeginLineStripMiterAAClosed  = &sdvg_BeginLineStripMiterAAClosed;
   tkminnie_shared.sdvg_BeginLineStripPatternMiter         = &sdvg_BeginLineStripPatternMiter;
   tkminnie_shared.sdvg_BeginLineStripPatternMiterClosed   = &sdvg_BeginLineStripPatternMiterClosed;
   tkminnie_shared.sdvg_BeginLineStripPatternMiterAA       = &sdvg_BeginLineStripPatternMiterAA;
   tkminnie_shared.sdvg_BeginLineStripPatternMiterAAClosed = &sdvg_BeginLineStripPatternMiterAAClosed;
   tkminnie_shared.sdvg_BeginLineStripPatternDecalMiter         = &sdvg_BeginLineStripPatternDecalMiter;
   tkminnie_shared.sdvg_BeginLineStripPatternDecalMiterClosed   = &sdvg_BeginLineStripPatternDecalMiterClosed;
   tkminnie_shared.sdvg_BeginLineStripPatternDecalMiterAA       = &sdvg_BeginLineStripPatternDecalMiterAA;
   tkminnie_shared.sdvg_BeginLineStripPatternDecalMiterAAClosed = &sdvg_BeginLineStripPatternDecalMiterAAClosed;
   tkminnie_shared.sdvg_BeginLines            = &sdvg_BeginLines;
   tkminnie_shared.sdvg_BeginLinesAA          = &sdvg_BeginLinesAA;
   tkminnie_shared.sdvg_BeginLinesGouraud    = &sdvg_BeginLinesGouraud;
   tkminnie_shared.sdvg_BeginLinesGouraudAA  = &sdvg_BeginLinesGouraudAA;
   tkminnie_shared.sdvg_BeginLinesPattern    = &sdvg_BeginLinesPattern;
   tkminnie_shared.sdvg_BeginLinesPatternAA  = &sdvg_BeginLinesPatternAA;
   tkminnie_shared.sdvg_BeginPointsSquare     = &sdvg_BeginPointsSquare;
   tkminnie_shared.sdvg_BeginPointsSquareAA   = &sdvg_BeginPointsSquareAA;
   tkminnie_shared.sdvg_BeginPointsRound      = &sdvg_BeginPointsRound;
   tkminnie_shared.sdvg_BeginPointsRoundAA    = &sdvg_BeginPointsRoundAA;
   tkminnie_shared.sdvg_BeginPolygon          = &sdvg_BeginPolygon;
   tkminnie_shared.sdvg_BeginPolygon32        = &sdvg_BeginPolygon32;
   tkminnie_shared.sdvg_BeginPolygon14_2      = &sdvg_BeginPolygon14_2;
   tkminnie_shared.sdvg_BeginPolygonAA        = &sdvg_BeginPolygonAA;
   tkminnie_shared.sdvg_BeginPolygonAA32      = &sdvg_BeginPolygonAA32;
   tkminnie_shared.sdvg_BeginPolygonAA14_2    = &sdvg_BeginPolygonAA14_2;
   tkminnie_shared.sdvg_BeginFilledPolygon    = &sdvg_BeginFilledPolygon;
   tkminnie_shared.sdvg_BeginFilledPolygonAA  = &sdvg_BeginFilledPolygonAA;
   tkminnie_shared.sdvg_VertexOffset2f        = &sdvg_VertexOffset2f;
   tkminnie_shared.sdvg_VertexOffset2fi16     = &sdvg_VertexOffset2fi16;
   tkminnie_shared.sdvg_AttribOffsetf         = &sdvg_AttribOffsetf;
   tkminnie_shared.sdvg_AttribOffset1f        = &sdvg_AttribOffset1f;
   tkminnie_shared.sdvg_AttribOffset2f        = &sdvg_AttribOffset2f;
   tkminnie_shared.sdvg_AttribOffset3f        = &sdvg_AttribOffset3f;
   tkminnie_shared.sdvg_AttribOffset4f        = &sdvg_AttribOffset4f;
   tkminnie_shared.sdvg_AttribOffsetARGB      = &sdvg_AttribOffsetARGB;
   tkminnie_shared.sdvg_Attrib1f              = &sdvg_Attrib1f;
   tkminnie_shared.sdvg_Attrib1i16            = &sdvg_Attrib1i16;
   tkminnie_shared.sdvg_Attrib1fx             = &sdvg_Attrib1fx;
   tkminnie_shared.sdvg_Attrib2f              = &sdvg_Attrib2f;
   tkminnie_shared.sdvg_Attrib2i16            = &sdvg_Attrib2i16;
   tkminnie_shared.sdvg_Attrib2fx             = &sdvg_Attrib2fx;
   tkminnie_shared.sdvg_Vertex2f              = &sdvg_Vertex2f;
   tkminnie_shared.sdvg_Vertex2x              = &sdvg_Vertex2x;
   tkminnie_shared.sdvg_Vertex3f              = &sdvg_Vertex3f;
   tkminnie_shared.sdvg_Vertex3x              = &sdvg_Vertex3x;
   tkminnie_shared.sdvg_TexCoord2f            = &sdvg_TexCoord2f;
   tkminnie_shared.sdvg_Attrib3f              = &sdvg_Attrib3f;
   tkminnie_shared.sdvg_Attrib3i16            = &sdvg_Attrib3i16;
   tkminnie_shared.sdvg_Attrib3fx             = &sdvg_Attrib3fx;
   tkminnie_shared.sdvg_Color3f               = &sdvg_Color3f;
   tkminnie_shared.sdvg_Attrib4f              = &sdvg_Attrib4f;
   tkminnie_shared.sdvg_Color4f               = &sdvg_Color4f;
   tkminnie_shared.sdvg_AttribARGB            = &sdvg_AttribARGB;
   tkminnie_shared.sdvg_ColorARGB             = &sdvg_ColorARGB;
   tkminnie_shared.sdvg_End                   = &sdvg_End;
   // ----------- additional (scratchbuffer) draw functions ------------
   tkminnie_shared.sdvg_DrawFilledRectangle   = &sdvg_DrawFilledRectangle;
   tkminnie_shared.sdvg_DrawRectangle         = &sdvg_DrawRectangle;
   // ----------- color utilities ------------
   tkminnie_shared.sdvg_ARGB         = &sdvg_ARGB;
   tkminnie_shared.sdvg_ARGBf        = &sdvg_ARGBf;
   tkminnie_shared.sdvg_MixARGBx     = &sdvg_MixARGBx;
   tkminnie_shared.sdvg_MixARGBf     = &sdvg_MixARGBf;
   tkminnie_shared.sdvg_MixRGBx      = &sdvg_MixRGBx;
   tkminnie_shared.sdvg_MixRGBf      = &sdvg_MixRGBf;
   tkminnie_shared.sdvg_TintARGB     = &sdvg_TintARGB;
   tkminnie_shared.sdvg_TintRGBAlpha = &sdvg_TintRGBAlpha;
   tkminnie_shared.sdvg_RGBAlpha     = &sdvg_RGBAlpha;
   tkminnie_shared.sdvg_HSVAToARGB   = &sdvg_HSVAToARGB;
   tkminnie_shared.sdvg_ARGBToHSVA   = &sdvg_ARGBToHSVA;
   tkminnie_shared.sdvg_GradientToTexture = &sdvg_GradientToTexture;

   (void)yac_host->yacCallbackSetFunById(tkminnie_shared_callback_id, (YAC_CFunctionPtr) &tkminnie_shared);
}

// ---------------------------------------------------------------------------- YAC_Init
void YAC_CALL YAC_Init(YAC_Host *_host) {
	yac_host = _host;

   // Resolve "standard" exception IDs
   Dyac_std_exid_resolve;

   YAC_Init_minnie(_host);
   YAC_Init_shadervg(_host);

#if MINNIE_ALLOC_DEBUG
   minnie::g_minnie_alloc_num = 0;
   minnie::g_minnie_alloc_cur = 0;
   minnie::g_minnie_alloc_max = 0;
   minnie::g_minnie_free_num  = 0;

   minnie::g_vgtesselator_alloc_cur = 0;
   minnie::g_vgtesselator_alloc_max = 0;
#endif // MINNIE_ALLOC_DEBUG

   minnie::MinnieSetup::InitStatic();
   minnie::minnie_setup_inst.init();

   loc_shared_export();

	if(yac_host->yacGetDebugLevel() > 1)
		yac_host->yacPrint("[dbg] tkminnie::YAC_Init() finished.\n");
}

void YAC_CALL YAC_Exit(YAC_Host *_host) {

   minnie::minnie_setup_inst.exit();

   YAC_Exit_shadervg(_host);
   YAC_Exit_minnie(_host);

	if(yac_host->yacGetDebugLevel() > 1)
		yac_host->yacPrint("[dbg] tkminnie::YAC_Exit() finished.\n");
}


#include <yac_host.cpp>
