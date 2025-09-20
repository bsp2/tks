/// matrix.h
///
/// (c) 2013-2025 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#ifndef TKOPENGL_MATRIX_H__
#define TKOPENGL_MATRIX_H__


#define TKOPENGL_MATRIX_STACK_SIZE (32)

#define TKOPENGL_MATRIX_MODE_MODELVIEW  (0)
#define TKOPENGL_MATRIX_MODE_PROJECTION (1)
#define TKOPENGL_MATRIX_MODE_TEXTURE    (2)
#define TKOPENGL_NUM_MATRIX_MODES       (3)

extern sUI tkopengl_matrix_mode;

extern sF32 tkopengl_projection_matrix_stack[TKOPENGL_MATRIX_STACK_SIZE * 4*4];
extern sSI tkopengl_projection_matrix_stacki;

extern sF32 tkopengl_modelview_matrix_stack[TKOPENGL_MATRIX_STACK_SIZE * 4*4];
extern sSI tkopengl_modelview_matrix_stacki;

extern sF32 tkopengl_texture_matrix_stack[TKOPENGL_MATRIX_STACK_SIZE * 4*4];
extern sSI tkopengl_texture_matrix_stacki;

extern sSI *tkopengl_matrix_stackip;
extern sF32 *tkopengl_matrix; // depends on matrix mode

#define TKOPENGL_MATRIX_FLAG_DIRTY       (1 << 0)   // 1=matrix has changed, must call glLoadMatrix
#define TKOPENGL_MATRIX_FLAG_ORTHOGONAL  (1 << 1)   // 1=can skip transpose(invert(modelview)) step for normal matrix calc.
#define TKOPENGL_MATRIX_FLAG_IDENTITY    (1 << 2)   // 1=is identity matrix

// (todo) optimize vertex/normal transformation for sparse matrices (=> specialized shaders)
/* #define TKOPENGL_MATRIX_FLAG_SPARSE_SCALEXY */
/* #define TKOPENGL_MATRIX_FLAG_SPARSE_SCALEZ */
/* #define TKOPENGL_MATRIX_FLAG_SPARSE_TRANSLATEXY */
/* #define TKOPENGL_MATRIX_FLAG_SPARSE_TRANSLATEZ */
/* #define TKOPENGL_MATRIX_FLAG_SPARSE_MASK  */

extern sUI tkopengl_matrix_flags[TKOPENGL_NUM_MATRIX_MODES];

#ifdef DX_EMU_FIXFXN
extern sF32 tkopengl_transform[4 * 4];

#ifdef DX_EMU_LIGHT
extern sF32 tkopengl_normalmatrix[3 * 3];
#endif // DX_EMU_LIGHT

#endif // DX_EMU_FIXFXN

extern void tkopengl_matrix_init(void);

extern void tkopengl_matrix_identity(sF32 *_mat);

extern void tkopengl_matrix_translate(sF32 *_mat, sF32 _x, sF32 _y, sF32 _z);

extern void tkopengl_matrix_scale(sF32 *_mat, sF32 _x, sF32 _y, sF32 _z);

extern void tkopengl_matrix_rotate(sF32 *_mat, sF32 _ang, sF32 _x, sF32 _y, sF32 _z);

extern void tkopengl_matrix_frustum(sF32 *_mat, sF32 _left, sF32 _right, sF32 _bottom, sF32 _top, sF32 _znear, sF32 _zfar);

extern void tkopengl_matrix_ortho(sF32 *_mat, sF32 _left, sF32 _right, sF32 _bottom, sF32 _top, sF32 _znear, sF32 _zfar);

extern void tkopengl_matrix_mul(sF32 *_dmat, const sF32 *_mat, const sF32 *_omat);

extern void tkopengl_matrix_mul_inplace(sF32 *_mat, const sF32 *_omat);

extern void tkopengl_matrix_update(void);

extern void tkopengl_matrix_transform_vec4(sF32 *_dvec, const sF32 *_mat, const sF32 *_svec);

//extern void tkopengl_matrix3x3_transform_vec3(sF32 *_dvec, const sF32 *_mat, const sF32 *_svec);

extern sF32 tkopengl_matrix_det3x3(const sF32 *_mat);

extern void tkopengl_matrix_transpose_invert3x3(sF32 *_mat3, const sF32 *_mat4);

#endif // TKOPENGL_MATRIX_H__
