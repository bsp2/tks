// ----
// ---- file   : shadervg_internal.h
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

#ifndef SHADERVG_INTERNAL_H__
#define SHADERVG_INTERNAL_H__

class ShaderVG_Shape;

// ----------- Log helper macros -----------
#define Dsdvg_printf            if(!MINNIE_PRINTF);else Dyac_host_printf
#define Dsdvg_tracecall         if( MINNIE_PRINTF);else Dsdvg_printf
#define Dsdvg_tracecallv        if( MINNIE_PRINTF);else Dsdvg_printf
#define Dsdvg_warnprintf        if(!MINNIE_PRINTF);else Dsdvg_printf
#define Dsdvg_errorprintf       if(!MINNIE_PRINTF);else Dsdvg_printf
#define Dsdvg_errorbeginprintf  if(!MINNIE_PRINTF);else Dsdvg_printf
#define Dsdvg_debugprintf       if( MINNIE_PRINTF);else Dsdvg_printf
#define Dsdvg_debugprintfv      if( MINNIE_PRINTF);else Dsdvg_printf
#define Dsdvg_debugprintfvv     if( MINNIE_PRINTF);else Dsdvg_printf
#define Dsdvg_transformprintf   if( MINNIE_PRINTF);else Dsdvg_printf
#define Dsdvg_transformprintfv  if( MINNIE_PRINTF);else Dsdvg_printf
#define Dsdvg_transformprintfvv if( MINNIE_PRINTF);else Dsdvg_printf
#define Dsdvg_uniformprintf     if( MINNIE_PRINTF);else Dsdvg_printf
#define Dsdvg_uniformprintfv    if( MINNIE_PRINTF);else Dsdvg_printf
#define Dsdvg_uniformprintfvv   if( MINNIE_PRINTF);else Dsdvg_printf

#include <stdlib.h>
#include <new>

typedef struct sdvg_color4f_s {
   sF32 r, g, b, a;
} sdvg_color4f_t;

typedef struct sdvg_paint_s {
#define PAINT_SOLID                0
#define PAINT_LINEAR               1
#define PAINT_RADIAL               2
#define PAINT_CONIC                3
#define PAINT_PATTERN              4
#define PAINT_PATTERN_ALPHA        5
#define PAINT_PATTERN_DECAL        6
#define PAINT_PATTERN_DECAL_ALPHA  7
   sSI  mode;
   sF32 start_x;
   sF32 start_y;
   sF32 dir_x;    // PAINT_LINEAR
   sF32 dir_y;
   sF32 angle01;  // 0..1 => 0..2PI
   sF32 size_x;   // PAINT_RADIAL/CONIC/PATTERN*
   sF32 size_y;   // PAINT_RADIAL/CONIC/PATTERN*
} sdvg_paint_t;

extern sBool          sdvg_int_b_glcore;
extern GLuint         sdvg_int_current_prg;
#ifdef SHADERVG_SCRIPT_API
extern YAC_Object *sdvg_int_mvp_matrix;  // _Matrix4f  (row major)
extern YAC_Object *sdvg_int_mvp_matrix_unproject;  // (todo)
#else
// MINNIE_LIB build
extern Matrix4f *sdvg_int_mvp_matrix;
extern Matrix4f *sdvg_int_mvp_matrix_unproject;
#endif // SHADERVG_SCRIPT_API
extern sdvg_color4f_t sdvg_int_color_fill;
extern sdvg_color4f_t sdvg_int_color_fill_ga;
extern sdvg_color4f_t sdvg_int_color_stroke;
extern sdvg_color4f_t sdvg_int_color_stroke_ga;
extern sSI            sdvg_int_shape_state_u_transform;
extern sSI            sdvg_int_shape_state_u_color_fill;
extern sSI            sdvg_int_shape_state_u_color_stroke;
extern sdvg_paint_t   sdvg_int_paint;
extern sSI            sdvg_int_shape_state_u_paint;
extern sSI            sdvg_int_attrib_enable_mask;
extern sSI            sdvg_int_active_attrib_enable_mask;
extern sSI            sdvg_int_attrib_divisor_mask;
extern sSI            sdvg_int_active_attrib_divisor_mask;

// -----------  internal -----------
void sdvg_int_BindScratchBuffer (void);
void sdvg_int_UnbindScratchBuffer (void);
#ifdef SHADERVG_USE_SCRATCHBUFFERSUBDATA
void sdvg_int_UpdateScratchOffset (void);
void sdvg_int_UploadScratchToVBO (void);
#endif // SHADERVG_USE_SCRATCHBUFFERSUBDATA
void sdvg_int_AllocScratchBuffer (sSI _aVertex, Dsdvg_buffer_ref_t _scratchBuf, sUI _numBytes);
void sdvg_int_FixShaderSourceVert (YAC_String *_s, YAC_String *_r);
void sdvg_int_FixShaderSourceFrag (YAC_String *_s, YAC_String *_d);
void sdvg_int_UniformMatrix4 (sSI _location, Dsdvg_mat4_ref_t _o);
sSI sdvg_int_BindFillShader (void);
void sdvg_int_EndFillShader (void);
void sdvg_int_UnbindFillShader (void);
void sdvg_int_BindShape (ShaderVG_Shape *_shape);
void sdvg_int_UnsetShapeIfBuiltIn (void);
/* #ifndef TKMINNIE_DUPLICATE_POINT_VERTICES */
/* void BindPointEBO6 (void); */
/* void BindPointEBO9 (void); */
/* void UnbindPointEBO (void); */
/* #endif // SHADERVG_MAX_POINT_VERTICES */
#ifdef SHADERVG_UNMAP_SCRATCHVBO_DURING_DRAW
void sdvg_unmap_scratch_before_draw (void);
void sdvg_remap_scratch_after_draw (void);
#endif // SHADERVG_UNMAP_SCRATCHVBO_DURING_DRAW
extern "C" {
#ifdef GL_TES_spirv_program_loader
void sdvg_int_find_spirv_program_by_name (const char *_name, const void **retAddr, uint32_t *retSize);
#endif // GL_TES_spirv_program_loader
void sdvg_int_debug_print_mem_info (void);
}
void sdvg_int_handle_queued_attrib_enable (void);
void sdvg_int_handle_queued_attrib_divisor (void);

// ----------- OpenGL helper macros -----------
#ifdef SHADERVG_USE_SCRATCHBUFFERSUBDATA
#define Dupdate_scratch_offset sdvg_int_UpdateScratchOffset()
#define Dupload_scratch_to_vbo sdvg_int_UploadScratchToVBO()
#else
#endif // SHADERVG_USE_SCRATCHBUFFERSUBDATA
#ifdef MINNIE_LIB
#define Dsdvg_glcall(f) f
#define Dsdvg_glcall_safe(n,f) f
#else
#define Dsdvg_glcall(f) tkopengl_shared->_##f
#define Dsdvg_glcall_safe(n,f) if(tkopengl_shared->_##n)tkopengl_shared->_##f
#endif // MINNIE_LIB
#define Dsdvg_uniform_1i(a,v) Dsdvg_glcall(glUniform1i(a,v))
#define Dsdvg_uniform_1ui(a,v) Dsdvg_glcall(glUniform1ui(a,v))
#define Dsdvg_uniform_1f(a,v) Dsdvg_glcall(glUniform1f(a,v))
#define Dsdvg_uniform_2f(a,v1,v2) Dsdvg_glcall(glUniform2f(a,v1,v2))
#define Dsdvg_uniform_3f(a,v1,v2,v3) Dsdvg_glcall(glUniform3f(a,v1,v2,v3))
#define Dsdvg_uniform_4f(a,v1,v2,v3,v4) Dsdvg_glcall(glUniform4f(a,v1,v2,v3,v4))
#define Dsdvg_uniform_4fv(a,num,v) Dsdvg_glcall(glUniform4fv(a,num,v))
#define Dsdvg_uniform_2fv(a,num,va) Dsdvg_glcall(glUniform2fv(a,num,va))
#define Dsdvg_uniform_mat4(a,m) sdvg_int_UniformMatrix4(a,m)  // load row-major matrix (+convert to GL column-major)
#define Dsdvg_attrib_offset(a,s,t,n,d,o) Dsdvg_glcall(zglVertexAttribOffset(a,s,t,n,d,o))
#define Dsdvg_attrib_pointer(a,s,t,n,d,p) Dsdvg_glcall(glVertexAttribPointer(a,s,t,n,d,p))
#define Dsdvg_attrib_enable(a) Dsdvg_glcall(glEnableVertexAttribArray(a))
#define Dsdvg_attrib_disable(a) Dsdvg_glcall(glDisableVertexAttribArray(a))
#define Dsdvg_queue_attrib_enable(a) sdvg_int_attrib_enable_mask |= (1 << (a))
#define Dsdvg_queue_attrib_disable(a) sdvg_int_attrib_enable_mask &= ~(1 << (a))
#define Dsdvg_handle_queued_attrib_enable() if(sdvg_int_active_attrib_enable_mask != sdvg_int_attrib_enable_mask) sdvg_int_handle_queued_attrib_enable()
#define Dsdvg_attrib_divisor(a, n) Dsdvg_glcall(glVertexAttribDivisor((a), (n)))
#define Dsdvg_attrib_divisor_reset(a) Dsdvg_glcall(glVertexAttribDivisor((a), 0))
#define Dsdvg_queue_attrib_divisor(a, n) sdvg_int_attrib_divisor_mask |= (1 << (a)); (void)(n)
#define Dsdvg_queue_attrib_divisor_reset(a) sdvg_int_attrib_divisor_mask &= ~(1 << (a))
#define Dsdvg_handle_queued_attrib_divisor() if(sdvg_int_active_attrib_divisor_mask != sdvg_int_attrib_divisor_mask) sdvg_int_handle_queued_attrib_divisor()
#define Dsdvg_handle_queued_attrib_enable_and_divisor() Dsdvg_handle_queued_attrib_enable(); Dsdvg_handle_queued_attrib_divisor()

#define Dsdvg_inc_shape_state(a) sdvg_int_shape_state_##a = ((sdvg_int_shape_state_##a) + 1) & 1073741823
#define Dsdvg_stencil_poly_even_odd_pass1()                          \
   Dsdvg_glcall(glEnable(GL_STENCIL_TEST));                          \
   Dsdvg_glcall(glStencilMask(1));                                   \
   Dsdvg_glcall(glStencilFunc(GL_ALWAYS, 0/*ref*/, 1/*mask*/));      \
   Dsdvg_glcall(glStencilOp(GL_INCR, GL_INCR, GL_INCR));             \
   Dsdvg_glcall(glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE))
#define Dsdvg_stencil_poly_non_zero_pass1()                                               \
   Dsdvg_glcall(glEnable(GL_STENCIL_TEST));                                               \
   Dsdvg_glcall(glStencilMask(255));                                                      \
   Dsdvg_glcall(glStencilFunc(GL_ALWAYS, 0/*ref*/, 255/*mask*/));                         \
   Dsdvg_glcall(glStencilOpSeparate(GL_FRONT, GL_INCR_WRAP, GL_INCR_WRAP, GL_INCR_WRAP)); \
   Dsdvg_glcall(glStencilOpSeparate(GL_BACK, GL_DECR_WRAP, GL_DECR_WRAP, GL_DECR_WRAP));  \
   Dsdvg_glcall(glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE))
#define Dsdvg_stencil_poly_even_odd_pass2()                       \
   Dsdvg_glcall(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE)); \
   Dsdvg_glcall(glStencilFunc(GL_EQUAL, 1/*ref*/, 1/*mask*/));    \
   Dsdvg_glcall(glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO))
#define Dsdvg_stencil_poly_non_zero_pass2()                         \
   Dsdvg_glcall(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));   \
   Dsdvg_glcall(glStencilFunc(GL_NOTEQUAL, 0/*ref*/, 255/*mask*/)); \
   Dsdvg_glcall(glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO))
#define Dsdvg_stencil_poly_end() Dsdvg_glcall(glDisable(GL_STENCIL_TEST))

#ifdef SHADERVG_UNMAP_SCRATCHVBO_DURING_DRAW
#define Dsdvg_unmap_scratch_before_draw sdvg_unmap_scratch_before_draw()
#define Dsdvg_remap_scratch_after_draw sdvg_remap_scratch_after_draw()
#else
#define Dsdvg_unmap_scratch_before_draw while(0)
#define Dsdvg_remap_scratch_after_draw while(0)
#endif // SHADERVG_UNMAP_SCRATCHVBO_DURING_DRAW

#ifdef SHADERVG_SKIP_DRAW
#define Dsdvg_draw_arrays(t,f,c) while(0 * ((t)*(f)*(c)))
#define Dsdvg_draw_triangles(f,c) while(0 * ((f)*(c)))
#define Dsdvg_draw_triangle_fan(f,c) while(0 * ((f)*(c)))
#define Dsdvg_draw_triangle_strip(f,c) while(0 * ((f)*(c)))
#define Dsdvg_draw_arrays_vbo(t,f,c) while(0 * ((t)*(f)*(c)))
#define Dsdvg_draw_triangles_vbo(f,c) while(0 * ((f)*(c)))
#define Dsdvg_draw_triangle_fan_vbo(f,c) while(0 * ((f)*(c)))
#define Dsdvg_draw_triangle_strip_vbo(f,c) while(0 * ((f)*(c)))
#define Dsdvg_draw_triangles_instanced_vbo(c,ic) while(0 * ((c)*(ic)))
#else
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
#define Dsdvg_draw_triangles_instanced_vbo(c,ic) Dsdvg_glcall(glDrawArraysInstanced(GL_TRIANGLES, 0, (c), (ic)))
#endif // SHADERVG_SKIP_DRAW


#endif // SHADERVG_INTERNAL_H__
