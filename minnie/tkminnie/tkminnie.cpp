// ----
// ---- file   : tkminnie.cpp
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
   tkminnie_shared.sdvg_ReturnToGL = &sdvg_ReturnToGL;
   // -------- transform --------
   tkminnie_shared.sdvg_SetTransform    = &sdvg_SetTransform;
   tkminnie_shared.sdvg_GetTransformRef = &sdvg_GetTransformRef;
   // -------- AA --------
   tkminnie_shared.sdvg_SetEnableAA = &sdvg_SetEnableAA;
   tkminnie_shared.sdvg_SetAARange  = &sdvg_SetAARange;
   tkminnie_shared.sdvg_SetAAExp    = &sdvg_SetAAExp;
   // -------- render state (fill / stroke) --------
   tkminnie_shared.sdvg_SetFillColor4f       = &sdvg_SetFillColor4f;
   tkminnie_shared.sdvg_SetFillAlpha         = &sdvg_SetFillAlpha;
   tkminnie_shared.sdvg_SetFillColorARGB     = &sdvg_SetFillColorARGB;
   tkminnie_shared.sdvg_SetStrokeWidth       = &sdvg_SetStrokeWidth;
   tkminnie_shared.sdvg_SetPointRadius       = &sdvg_SetPointRadius;
   tkminnie_shared.sdvg_SetPixelScaling      = &sdvg_SetPixelScaling;
   tkminnie_shared.sdvg_SetStrokeColor4f     = &sdvg_SetStrokeColor4f;
   tkminnie_shared.sdvg_SetStrokeColorARGB   = &sdvg_SetStrokeColorARGB;
   tkminnie_shared.sdvg_SetGlobalAlpha       = &sdvg_SetGlobalAlpha;
   tkminnie_shared.sdvg_SetTextureDecalAlpha = &sdvg_SetTextureDecalAlpha;
   // -------- clear --------
   tkminnie_shared.sdvg_Clear4f   = &sdvg_Clear4f;
   tkminnie_shared.sdvg_ClearARGB = &sdvg_ClearARGB;
   // -------- blending --------
   tkminnie_shared.sdvg_EnableBlending                  = &sdvg_EnableBlending;
   tkminnie_shared.sdvg_EnableBlendingKeepAlpha         = &sdvg_EnableBlendingKeepAlpha;
   tkminnie_shared.sdvg_EnableBlendingAdditive          = &sdvg_EnableBlendingAdditive;
   tkminnie_shared.sdvg_EnableBlendingAdditiveKeepAlpha = &sdvg_EnableBlendingAdditiveKeepAlpha;
   tkminnie_shared.sdvg_EnableBlendingSrcColorKeepAlpha = &sdvg_EnableBlendingSrcColorKeepAlpha;
   tkminnie_shared.sdvg_EnableBlendingDstColorKeepAlpha = &sdvg_EnableBlendingDstColorKeepAlpha;
   tkminnie_shared.sdvg_DisableBlending                 = &sdvg_DisableBlending;
   tkminnie_shared.sdvg_AlphaWrite                      = &sdvg_AlphaWrite;
   // -------- VBO --------
   tkminnie_shared.sdvg_CreateVBO          = &sdvg_CreateVBO;
   tkminnie_shared.sdvg_UpdateVBO          = &sdvg_UpdateVBO;
   tkminnie_shared.sdvg_BindVBO            = &sdvg_BindVBO;
   tkminnie_shared.sdvg_GetVBOSize         = &sdvg_GetVBOSize;
   tkminnie_shared.sdvg_GetMappedVBOOffset = &sdvg_GetMappedVBOOffset;
   tkminnie_shared.sdvg_MapVBO             = &sdvg_MapVBO;
   tkminnie_shared.sdvg_UnmapVBO           = &sdvg_UnmapVBO;
   tkminnie_shared.sdvg_UnbindVBO          = &sdvg_UnbindVBO;
   tkminnie_shared.sdvg_DestroyVBO         = &sdvg_DestroyVBO;
   // ----------- custom shaders and (scratch) draw functions ------------
   tkminnie_shared.sdvg_BeginVBO              = &sdvg_BeginVBO;
   tkminnie_shared.sdvg_BeginTriangles        = &sdvg_BeginTriangles;
   tkminnie_shared.sdvg_BeginTriangleFan      = &sdvg_BeginTriangleFan;
   tkminnie_shared.sdvg_BeginTriangleStrip    = &sdvg_BeginTriangleStrip;
   tkminnie_shared.sdvg_BeginFilledTriangles            = &sdvg_BeginFilledTriangles;
   tkminnie_shared.sdvg_BeginFilledTriangleFan          = &sdvg_BeginFilledTriangleFan;
   tkminnie_shared.sdvg_BeginFilledTriangleStrip        = &sdvg_BeginFilledTriangleStrip;
   tkminnie_shared.sdvg_BeginFilledGouraudTriangles     = &sdvg_BeginFilledGouraudTriangles;
   tkminnie_shared.sdvg_BeginFilledGouraudTriangleFan   = &sdvg_BeginFilledGouraudTriangleFan;
   tkminnie_shared.sdvg_BeginFilledGouraudTriangleStrip = &sdvg_BeginFilledGouraudTriangleStrip;
   tkminnie_shared.sdvg_BeginTexturedTriangles          = &sdvg_BeginTexturedTriangles;
   tkminnie_shared.sdvg_BeginTexturedTriangleFan        = &sdvg_BeginTexturedTriangleFan;
   tkminnie_shared.sdvg_BeginTexturedTriangleStrip      = &sdvg_BeginTexturedTriangleStrip;
   tkminnie_shared.sdvg_BeginLineStrip        = &sdvg_BeginLineStrip;
   tkminnie_shared.sdvg_BeginLineStripAA      = &sdvg_BeginLineStripAA;
   tkminnie_shared.sdvg_BeginLineStripBevel   = &sdvg_BeginLineStripBevel;
   tkminnie_shared.sdvg_BeginLineStripBevelAA = &sdvg_BeginLineStripBevelAA;
   tkminnie_shared.sdvg_BeginLines            = &sdvg_BeginLines;
   tkminnie_shared.sdvg_BeginLinesAA          = &sdvg_BeginLinesAA;
   tkminnie_shared.sdvg_BeginPointsSquare     = &sdvg_BeginPointsSquare;
   tkminnie_shared.sdvg_BeginPointsSquareAA   = &sdvg_BeginPointsSquareAA;
   tkminnie_shared.sdvg_BeginPointsRound      = &sdvg_BeginPointsRound;
   tkminnie_shared.sdvg_BeginPointsRoundAA    = &sdvg_BeginPointsRoundAA;
   tkminnie_shared.sdvg_VertexOffset2f        = &sdvg_VertexOffset2f;
   tkminnie_shared.sdvg_AttribOffsetf         = &sdvg_AttribOffsetf;
   tkminnie_shared.sdvg_AttribOffset1f        = &sdvg_AttribOffset1f;
   tkminnie_shared.sdvg_AttribOffset2f        = &sdvg_AttribOffset2f;
   tkminnie_shared.sdvg_AttribOffset3f        = &sdvg_AttribOffset3f;
   tkminnie_shared.sdvg_AttribOffset4f        = &sdvg_AttribOffset4f;
   tkminnie_shared.sdvg_AttribOffsetARGB      = &sdvg_AttribOffsetARGB;
   tkminnie_shared.sdvg_Attrib1f              = &sdvg_Attrib1f;
   tkminnie_shared.sdvg_Attrib2f              = &sdvg_Attrib2f;
   tkminnie_shared.sdvg_Vertex2f              = &sdvg_Vertex2f;
   tkminnie_shared.sdvg_TexCoord2f            = &sdvg_TexCoord2f;
   tkminnie_shared.sdvg_Attrib3f              = &sdvg_Attrib3f;
   tkminnie_shared.sdvg_Color3f               = &sdvg_Color3f;
   tkminnie_shared.sdvg_Attrib4f              = &sdvg_Attrib4f;
   tkminnie_shared.sdvg_Color4f               = &sdvg_Color4f;
   tkminnie_shared.sdvg_AttribARGB            = &sdvg_AttribARGB;
   tkminnie_shared.sdvg_ColorARGB             = &sdvg_ColorARGB;
   tkminnie_shared.sdvg_End                   = &sdvg_End;
   // ----------- additional (scratch) draw functions ------------
   tkminnie_shared.sdvg_DrawFilledRectangle   = &sdvg_DrawFilledRectangle;
   tkminnie_shared.sdvg_DrawRectangle         = &sdvg_DrawRectangle;

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

   minnie::setup::Init();
   // // sdvgInit();

   loc_shared_export();

	if(yac_host->yacGetDebugLevel() > 1)
		yac_host->yacPrint("[dbg] tkminnie::YAC_Init() finished.\n");
}

void YAC_CALL YAC_Exit(YAC_Host *_host) {

   // // sdvgExit();
   minnie::setup::Exit();

   YAC_Exit_shadervg(_host);
   YAC_Exit_minnie(_host);

	if(yac_host->yacGetDebugLevel() > 1)
		yac_host->yacPrint("[dbg] tkminnie::YAC_Exit() finished.\n");
}


#include <yac_host.cpp>
