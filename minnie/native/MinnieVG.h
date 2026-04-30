// ----
// ---- file   : MinnieVG.h
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : Distributed under terms of the MIT license (https://opensource.org/licenses/MIT)
// ----          Copyright 2025-2026 by bsp
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
// ---- info   : "minnie" test case utilities
// ---- note   :
// ----
// ----
// ----

#pragma once

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

extern sBool MinnieVG_Init (sBool _bGLCore);
extern sBool MinnieVG_InitShaderVG (sBool _bGLCore);
extern void  MinnieVG_InitMinnie (void);
extern sBool MinnieVG_InitMinnieScratchBuffers (void);
extern sBool MinnieVG_OnOpen (void);
extern void  MinnieVG_Exit (void);

extern void  MinnieVG_BeginFrame (void);
extern void  MinnieVG_EndFrame (void);

extern sBool MinnieVG_SetupDrawableFromBuffer (MinnieDrawable *_drawable, YAC_Buffer *_mib);
extern void  MinnieVG_DebugPrintMinnieAndDrawableStats (MinnieDrawable *_drawable);
extern void  MinnieVG_CalcModelTransform (MinnieDrawable *_d, Matrix4f *_mat);
extern void  MinnieVG_SetTransformForDrawable (MinnieDrawable *_d);
extern sBool MinnieVG_HandleDebugKey (sU32 _code, sU32 _mod);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus
