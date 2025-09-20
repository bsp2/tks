// ----
// ---- file   : shadervg_internal.h
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

#ifndef SHADERVG_INTERNAL_H__
#define SHADERVG_INTERNAL_H__


#include <stdlib.h>
#include <new>

#define Dsdvg_buffer_ref_t  YAC_Buffer *
#define Dsdvg_mat4_ref_t  YAC_Object *

extern sF32 sdvg_pixel_scl;  // vp/proj (aa_range, stroke_w)
extern sBool sdvg_b_glcore;

// -----------  internal -----------
void BindScratchBuffer (void);
void UnbindScratchBuffer (void);
#ifdef SHADERVG_USE_SCRATCHBUFFERSUBDATA
void UpdateScratchOffset (void);
void UploadScratchToVBO (void);
#endif // SHADERVG_USE_SCRATCHBUFFERSUBDATA
void AllocScratchBuffer (sSI _aVertex, Dsdvg_buffer_ref_t _scratchBuf, sUI _numBytes);
void FixShaderSourceVert (YAC_String *_s, YAC_String *_r);
void FixShaderSourceFrag (YAC_String *_s, YAC_String *_d);
void UniformMatrix4 (sSI _location, Dsdvg_mat4_ref_t _o);
sSI BindFillShader (void);
void EndFillShader (void);
void UnbindFillShader (void);
void BindShape (ShaderVG_Shape *_shape);
void UnsetShapeIfBuiltIn (void);
/* #ifndef TKMINNIE_DUPLICATE_POINT_VERTICES */
/* void BindPointEBO6 (void); */
/* void BindPointEBO9 (void); */
/* void UnbindPointEBO (void); */
/* #endif // SHADERVG_MAX_POINT_VERTICES */
#ifdef SHADERVG_UNMAP_SCRATCHVBO_DURING_DRAW
void sdvg_unmap_scratch_before_draw (void);
void sdvg_remap_scratch_after_draw (void);
#endif // SHADERVG_UNMAP_SCRATCHVBO_DURING_DRAW

// ----------- Log helper macros -----------
#define Dsdvg_printf        if(0);else Dyac_host_printf
#define Dsdvg_tracecall     if(1);else Dsdvg_printf
#define Dsdvg_tracecallv    if(1);else Dsdvg_printf
#define Dsdvg_warnprintf    if(0);else Dsdvg_printf
#define Dsdvg_errorprintf   if(0);else Dsdvg_printf
#define Dsdvg_debugprintf   if(0);else Dsdvg_printf
#define Dsdvg_debugprintfv  if(1);else Dsdvg_printf
#define Dsdvg_debugprintfvv if(1);else Dsdvg_printf

// ----------- OpenGL helper macros -----------
#ifdef SHADERVG_USE_SCRATCHBUFFERSUBDATA
#define Dupdate_scratch_offset UpdateScratchOffset()
#define Dupload_scratch_to_vbo UploadScratchToVBO()
#else
#endif // SHADERVG_USE_SCRATCHBUFFERSUBDATA
#define Dsdvg_glcall(f) tkopengl_shared->_##f
#define Dsdvg_uniform_1i(a,v) Dsdvg_glcall(glUniform1i(a,v))
#define Dsdvg_uniform_1f(a,v) Dsdvg_glcall(glUniform1f(a,v))
#define Dsdvg_uniform_2f(a,v1,v2) Dsdvg_glcall(glUniform2f(a,v1,v2))
#define Dsdvg_uniform_3f(a,v1,v2,v3) Dsdvg_glcall(glUniform3f(a,v1,v2,v3))
#define Dsdvg_uniform_4f(a,v1,v2,v3,v4) Dsdvg_glcall(glUniform4f(a,v1,v2,v3,v4))
#define Dsdvg_uniform_mat4(a,m) UniformMatrix4(a,m)  // load row-major matrix (+convert to GL column-major)
#define Dsdvg_attrib_offset(a,s,t,n,d,o) Dsdvg_glcall(zglVertexAttribOffset(a,s,t,n,d,o))
#define Dsdvg_attrib_pointer(a,s,t,n,d,p) Dsdvg_glcall(glVertexAttribPointer(a,s,t,n,d,p))
#define Dsdvg_attrib_enable(a) Dsdvg_glcall(glEnableVertexAttribArray(a))
#define Dsdvg_attrib_disable(a) Dsdvg_glcall(glDisableVertexAttribArray(a))
#ifdef USE_VERTEX_ATTRIB_DIVISOR
#define Dsdvg_attrib_divisor(a, n) Dsdvg_glcall(glVertexAttribDivisor((a), (n)))
#define Dsdvg_attrib_divisor_reset(a) Dsdvg_glcall(glVertexAttribDivisor((a), 0))
#else
#define Dsdvg_attrib_divisor_reset(a) while(0)
#endif // USE_VERTEX_ATTRIB_DIVISOR
#define Dsdvg_stencil_poly_pass1()                                   \
   Dsdvg_glcall(glEnable(GL_STENCIL_TEST));                          \
   Dsdvg_glcall(glStencilMask(1));                                   \
   Dsdvg_glcall(glStencilFunc(GL_ALWAYS, 0/*ref*/, 1/*mask*/));      \
   Dsdvg_glcall(glStencilOp(GL_INCR, GL_INCR, GL_INCR));             \
   Dsdvg_glcall(glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE))
#define Dsdvg_stencil_poly_pass2()                                \
   Dsdvg_glcall(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE)); \
   Dsdvg_glcall(glStencilFunc(GL_EQUAL, 1/*ref*/, 1/*mask*/));    \
   Dsdvg_glcall(glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO))
#define Dsdvg_stencil_poly_end() Dsdvg_glcall(glDisable(GL_STENCIL_TEST))

#ifdef SHADERVG_UNMAP_SCRATCHVBO_DURING_DRAW
#define Dsdvg_unmap_scratch_before_draw sdvg_unmap_scratch_before_draw()
#define Dsdvg_remap_scratch_after_draw sdvg_remap_scratch_after_draw()
#else
#define Dsdvg_unmap_scratch_before_draw while(0)
#define Dsdvg_remap_scratch_after_draw while(0)
#endif // SHADERVG_UNMAP_SCRATCHVBO_DURING_DRAW

#ifdef SHADERVG_USE_SCRATCHBUFFERSUBDATA
#define Dsdvg_draw_arrays(t,f,c) Dupload_scratch_to_vbo; Dsdvg_glcall(glDrawArrays(t,f,c))
#define Dsdvg_draw_triangles(f,c) Dupload_scratch_to_vbo; Dsdvg_glcall(glDrawArrays(GL_TRIANGLES,f,c))
#define Dsdvg_draw_triangle_fan(f,c) Dupload_scratch_to_vbo; Dsdvg_glcall(glDrawArrays(GL_TRIANGLE_FAN,f,c))
#define Dsdvg_draw_triangle_strip(f,c) Dupload_scratch_to_vbo; Dsdvg_glcall(glDrawArrays(GL_TRIANGLE_STRIP,f,c))
#else
#ifdef SHADERVG_UNMAP_SCRATCHVBO_DURING_DRAW
#define Dsdvg_draw_arrays(t,f,c) Dsdvg_unmap_scratch_before_draw; Dsdvg_glcall(glDrawArrays(t,f,c)); Dsdvg_remap_scratch_after_draw

#define Dsdvg_draw_triangles(f,c) Dsdvg_unmap_scratch_before_draw; Dsdvg_glcall(glDrawArrays(GL_TRIANGLES,f,c)); Dsdvg_remap_scratch_after_draw
#define Dsdvg_draw_triangle_fan(f,c) Dsdvg_unmap_scratch_before_draw; Dsdvg_glcall(glDrawArrays(GL_TRIANGLE_FAN,f,c)); Dsdvg_remap_scratch_after_draw
#define Dsdvg_draw_triangle_strip(f,c) Dsdvg_unmap_scratch_before_draw; Dsdvg_glcall(glDrawArrays(GL_TRIANGLE_STRIP,f,c)); Dsdvg_remap_scratch_after_draw
#else
#define Dsdvg_draw_arrays(t,f,c) Dsdvg_glcall(glDrawArrays(t,f,c))
#define Dsdvg_draw_triangles(f,c) Dsdvg_glcall(glDrawArrays(GL_TRIANGLES,f,c))
#define Dsdvg_draw_triangle_fan(f,c) Dsdvg_glcall(glDrawArrays(GL_TRIANGLE_FAN,f,c))
#define Dsdvg_draw_triangle_strip(f,c) Dsdvg_glcall(glDrawArrays(GL_TRIANGLE_STRIP,f,c))
#endif // SHADERVG_UNMAP_SCRATCHVBO_DURING_DRAW
#endif // SHADERVG_USE_SCRATCHBUFFERSUBDATA

#define Dsdvg_draw_arrays_vbo(t,f,c) Dsdvg_glcall(glDrawArrays(t,f,c))
#define Dsdvg_draw_triangles_vbo(f,c) Dsdvg_glcall(glDrawArrays(GL_TRIANGLES,f,c))
#define Dsdvg_draw_triangle_fan_vbo(f,c) Dsdvg_glcall(glDrawArrays(GL_TRIANGLE_FAN,f,c))
#define Dsdvg_draw_triangle_strip_vbo(f,c) Dsdvg_glcall(glDrawArrays(GL_TRIANGLE_STRIP,f,c))

#ifdef USE_VERTEX_ATTRIB_DIVISOR
#define Dsdvg_draw_triangles_instanced_vbo(c,ic) Dsdvg_glcall(glDrawArraysInstanced(GL_TRIANGLES, 0, (c), (ic)))
#endif // USE_VERTEX_ATTRIB_DIVISOR


#endif // SHADERVG_INTERNAL_H__
