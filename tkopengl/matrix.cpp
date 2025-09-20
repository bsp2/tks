/// matrix.cpp
///
/// (c) 2013-2025 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#include <stdlib.h>
#include <math.h>

#define YAC_PRINTF defined
#include <yac.h>

extern YAC_Host *yac_host;

#include "inc_opengl.h"
#include "tkopengl.h"

#include "glerror.h"

#include "tks-list.h"

#include "bitmap.h"
#include "image.h"
#include "tkstexture.h"
#include "tksopengl.h"

#ifdef DX_EMU_MATRIX

#include "attrib.h"
#include "matrix.h"

#ifdef DX_EMU_FIXFXN
#include "shader.h"
#endif

/*
           m[0]  m[4]  m[8]  m[12]     v[0]
           m[1]  m[5]  m[9]  m[13]     v[1]
   M(v) =  m[2]  m[6]  m[10] m[14]  X  v[2]
           m[3]  m[7]  m[11] m[15]     v[3]

   (0=a, 1=b, ...)
*/

sUI tkopengl_matrix_mode;

sF32 tkopengl_modelview_matrix_stack[TKOPENGL_MATRIX_STACK_SIZE * 4*4];
sSI tkopengl_modelview_matrix_stacki;

sF32 tkopengl_projection_matrix_stack[TKOPENGL_MATRIX_STACK_SIZE * 4*4];
sSI tkopengl_projection_matrix_stacki;

sF32 tkopengl_texture_matrix_stack[TKOPENGL_MATRIX_STACK_SIZE * 4*4];
sSI tkopengl_texture_matrix_stacki;

sF32 *tkopengl_matrix;
sSI *tkopengl_matrix_stackip;

sUI tkopengl_matrix_flags[TKOPENGL_NUM_MATRIX_MODES];

sF32 tkopengl_transform[4 * 4]; // projection * modelview

#ifdef DX_EMU_LIGHT
sF32 tkopengl_normalmatrix[3 * 3];  // transpose(invert(modelview.3x3))
#endif


// ---------------------------------------------------------------------------- tkopengl_matrix_init
void tkopengl_matrix_init(void) {
   tkopengl_projection_matrix_stacki = (TKOPENGL_MATRIX_STACK_SIZE - 1);
   tkopengl_modelview_matrix_stacki  = (TKOPENGL_MATRIX_STACK_SIZE - 1);
   tkopengl_texture_matrix_stacki  = (TKOPENGL_MATRIX_STACK_SIZE - 1);

   /* initial matrix mode = GL_MODELVIEW */
   tkopengl_matrix = &tkopengl_modelview_matrix_stack[tkopengl_modelview_matrix_stacki << 4]; // *16 (4x4 mat)
   tkopengl_matrix_stackip = &tkopengl_modelview_matrix_stacki;

   tkopengl_matrix_flags[TKOPENGL_MATRIX_MODE_MODELVIEW]  =
      (TKOPENGL_MATRIX_FLAG_DIRTY | TKOPENGL_MATRIX_FLAG_ORTHOGONAL | TKOPENGL_MATRIX_FLAG_IDENTITY);
   tkopengl_matrix_flags[TKOPENGL_MATRIX_MODE_PROJECTION] =
      (TKOPENGL_MATRIX_FLAG_DIRTY | TKOPENGL_MATRIX_FLAG_ORTHOGONAL | TKOPENGL_MATRIX_FLAG_IDENTITY);
   tkopengl_matrix_flags[TKOPENGL_MATRIX_MODE_TEXTURE] =
      (TKOPENGL_MATRIX_FLAG_DIRTY | TKOPENGL_MATRIX_FLAG_ORTHOGONAL | TKOPENGL_MATRIX_FLAG_IDENTITY);

   tkopengl_matrix_mode = TKOPENGL_MATRIX_MODE_MODELVIEW;

   tkopengl_matrix_identity(&tkopengl_modelview_matrix_stack [tkopengl_modelview_matrix_stacki  << 4]);
   tkopengl_matrix_identity(&tkopengl_projection_matrix_stack[tkopengl_projection_matrix_stacki << 4]);
   tkopengl_matrix_identity(&tkopengl_texture_matrix_stack   [tkopengl_texture_matrix_stacki    << 4]);
}

// ---------------------------------------------------------------------------- tkopengl_matrix_identity
void tkopengl_matrix_identity(sF32 *_mat) {
   memset((void*)_mat, 0, sizeof(sF32) * 4 * 4);

   _mat[0*4 + 0] = 1.0f;
   _mat[1*4 + 1] = 1.0f;
   _mat[2*4 + 2] = 1.0f;
   _mat[3*4 + 3] = 1.0f;
}

// ---------------------------------------------------------------------------- tkopengl_matrix_translate
void tkopengl_matrix_translate(sF32 *_mat, sF32 _x, sF32 _y, sF32 _z) {
   /*
    *  (a e i m)   (1 0 0 X)   (a, e i, a*x+e*y*i*z+m*1)
    *  (b f j n) * (0 1 0 Y) = (b, f j, b*x+f*y+j*z+n*1)
    *  (c g k o)   (0 0 1 Z)   (c, g k, c*x+g*y+k*z+o*1)
    *  (d h l p)   (0 0 0 1)   (d, h l, d*x+h*y*l*z+p*1)
    */

   _mat[12] = (_mat[ 0] * _x) + (_mat[ 4] * _y) + (_mat[ 8] * _z) + _mat[12];
   _mat[13] = (_mat[ 1] * _x) + (_mat[ 5] * _y) + (_mat[ 9] * _z) + _mat[13];
   _mat[14] = (_mat[ 2] * _x) + (_mat[ 6] * _y) + (_mat[10] * _z) + _mat[14];
   _mat[15] = (_mat[ 3] * _x) + (_mat[ 7] * _y) + (_mat[11] * _z) + _mat[15];
}

// ---------------------------------------------------------------------------- tkopengl_matrix_scale
void tkopengl_matrix_scale(sF32 *_mat, sF32 _x, sF32 _y, sF32 _z) {
   /*
     (a e i m)   (x 0 0 0)   (a*x, e*y, i*z, m)
     (b f j n) * (0 y 0 0) = (b*x, f*y, j*z, n)
     (c g k o)   (0 0 z 0)   (c*x, g*y, k*z, o)
     (d h l p)   (0 0 0 1)   (d*x, h*y, l*z, p)
   */

   _mat[ 0] = _mat[ 0] * _x;
   _mat[ 1] = _mat[ 1] * _x;
   _mat[ 2] = _mat[ 2] * _x;
   _mat[ 3] = _mat[ 3] * _x;

   _mat[ 4] = _mat[ 4] * _y;
   _mat[ 5] = _mat[ 5] * _y;
   _mat[ 6] = _mat[ 6] * _y;
   _mat[ 7] = _mat[ 7] * _y;

   _mat[ 8] = _mat[ 8] * _z;
   _mat[ 9] = _mat[ 9] * _z;
   _mat[10] = _mat[10] * _z;
   _mat[11] = _mat[11] * _z;
}

// ---------------------------------------------------------------------------- tkopengl_matrix_rotate
void tkopengl_matrix_rotate(sF32 *_mat, sF32 _ang, sF32 _x, sF32 _y, sF32 _z) {

   /*
    *  (a e i m)   [ A  E  I  0 ]   [ a*A + e*B + i*C,  a*E + e*F + i*G,  a*I + e*J + i*K,  m ]
    *  (b f j n) * [ B  F  J  0 ] = [ b*A + f*B + j*C,  b*E + f*F + j*G,  b*I + f*J + j*K   n ]
    *  (c g k o)   [ C  G  K  0 ]   [ c*A + g*B + k*C,  c*E + g*F + k*G,  c*I + g*J + k*K   o ]
    *  (d h l p)   [ 0  0  0  1 ]   [ d*A + h*B + l*C,  d*E + h*F + l*G,  d*I + h*J + l*K   p ]
    *
    *   A = x²(1-c)+c    E = xy(1-c)-zs   I = xz(1-c)+ys
    *   B = yx(1-c)+zs   F = y²(1-c)+c    J = yz(1-c)-xs
    *   C = xz(1-c)-ys   G = yz(1-c)+xs   K = z²(1-c)+c
    *
    */

   sF32 obrlen = 1.0f / sqrtf(_x*_x + _y*_y + _z*_z);
   if(obrlen != 0.0f)
   {
      _x *= obrlen;
      _y *= obrlen;
      _z *= obrlen;
   }

   _ang = (((sF32)(sM_PI * 2.0)) * _ang) / 360.0f; // deg to rad

   sF32 c = (sF32)cos(_ang);
   sF32 s = (sF32)sin(_ang);

   sF32 omc = 1.0f - c;

   sF32 xs = _x * s;
   sF32 ys = _y * s;
   sF32 zs = _z * s;

   sF32 xz = _x * _z;
   sF32 yz = _y * _z;

   sF32 tm[12];

   for(int i=0; i<12; i++)
   {
      tm[i] = _mat[i];
   }

   sF32 xzomc = xz * omc;
   sF32 yzomc = yz * omc;

   sF32 A = (_x*_x * omc) + c;
   sF32 B = (_y*_x * omc) + zs;
   sF32 C = xzomc - ys;

   sF32 E = (_x*_y * omc) - zs;
   sF32 F = (_y*_y * omc) + c;
   sF32 G = yzomc + xs;

   sF32 I = xzomc + ys;
   sF32 J = yzomc - xs;
   sF32 K = (_z*_z * omc) + c;

   _mat[ 0] = (tm[ 0] * A) + (tm[ 4] * B) + (tm[ 8] * C);
   _mat[ 1] = (tm[ 1] * A) + (tm[ 5] * B) + (tm[ 9] * C);
   _mat[ 2] = (tm[ 2] * A) + (tm[ 6] * B) + (tm[10] * C);
   _mat[ 3] = (tm[ 3] * A) + (tm[ 7] * B) + (tm[11] * C);

   _mat[ 4] = (tm[ 0] * E) + (tm[ 4] * F) + (tm[ 8] * G);
   _mat[ 5] = (tm[ 1] * E) + (tm[ 5] * F) + (tm[ 9] * G);
   _mat[ 6] = (tm[ 2] * E) + (tm[ 6] * F) + (tm[10] * G);
   _mat[ 7] = (tm[ 3] * E) + (tm[ 7] * F) + (tm[11] * G);

   _mat[ 8] = (tm[ 0] * I) + (tm[ 4] * J) + (tm[ 8] * K);
   _mat[ 9] = (tm[ 1] * I) + (tm[ 5] * J) + (tm[ 9] * K);
   _mat[10] = (tm[ 2] * I) + (tm[ 6] * J) + (tm[10] * K);
   _mat[11] = (tm[ 3] * I) + (tm[ 7] * J) + (tm[11] * K);
}

// ---------------------------------------------------------------------------- tkopengl_matrix_frustum
void tkopengl_matrix_frustum(sF32 *_mat, sF32 _left, sF32 _right, sF32 _bottom, sF32 _top, sF32 _znear, sF32 _zfar) {

   /*
    *  (a e i m)   (A 0  C 0)   (a*A, e*B, a*C+e*D+i*E-m, i*F)
    *  (b f j n) * (0 B  D 0) = (b*A, f*B, b*C+f*D+j*E-n, j*F)
    *  (c g k o)   (0 0  E F)   (c*A, g*B, c*C+g*D+k*E-o, k*F)
    *  (d h l p)   (0 0 -1 0)   (d*A, h*B, d*C+h*D+l*E-p, l*F)
    *
    *  A = (2*near) / (right-left)
    *  B = (2*near) / (top-bottom)
    *
    *  C = (right+left) / (right-left)
    *  D = (top+bottom) / (top-bottom)
    *  E = - ( (far+near) / (far-near) )
    *
    *  F = - ( (2*far*near) / (far-near) )
    */

   // _bottom = -_bottom;
   // _top = -_top;

   sF32 near2 = (2.0f * _znear);

   sF32 A = near2 / (_right - _left);
   sF32 B = near2 / (_top - _bottom);

   sF32 C = (_right + _left) / (_right - _left);
   sF32 D = (_top + _bottom) / (_top - _bottom);
   sF32 E = -(_zfar + _znear) / (_zfar - _znear);

   sF32 F = -(2.0f * _zfar * _znear) / (_zfar - _znear);

   sF32 tm[16];

   for(int i=0; i<16; i++)
   {
      tm[i] = _mat[i];
   }

   _mat[ 0] = tm[ 0] * A;
   _mat[ 1] = tm[ 1] * A;
   _mat[ 2] = tm[ 2] * A;
   _mat[ 3] = tm[ 3] * A;

   _mat[ 4] = tm[ 4] * B;
   _mat[ 5] = tm[ 5] * B;
   _mat[ 6] = tm[ 6] * B;
   _mat[ 7] = tm[ 7] * B;

   _mat[ 8] = (tm[ 0] * C) + (tm[ 4] * D) + (tm[ 8] * E) - tm[12];
   _mat[ 9] = (tm[ 1] * C) + (tm[ 5] * D) + (tm[ 9] * E) - tm[13];
   _mat[10] = (tm[ 2] * C) + (tm[ 6] * D) + (tm[10] * E) - tm[14];
   _mat[11] = (tm[ 3] * C) + (tm[ 7] * D) + (tm[11] * E) - tm[15];

   _mat[12] = (tm[ 8] * F);
   _mat[13] = (tm[ 9] * F);
   _mat[14] = (tm[10] * F);
   _mat[15] = (tm[11] * F);
}

// ---------------------------------------------------------------------------- tkopengl_matrix_ortho
void tkopengl_matrix_ortho(sF32 *_mat, sF32 _left, sF32 _right, sF32 _bottom, sF32 _top, sF32 _znear, sF32 _zfar) {

   /*
    *  (a e i m)   (A 0 0 tx)   (a*A, e*B, i*C, a*tx + e*ty + i*tz + m)
    *  (b f j n) * (0 B 0 ty) = (b*A, f*B, j*C, b*tx + f*ty + j*tz + n)
    *  (c g k o)   (0 0 C tz)   (c*A, g*B, k*C, c*tx + g*ty + k*tz + o)
    *  (d h l p)   (0 0 0 1 )   (d*A, h*B, l*C, d*tx + h*ty + l*tz + p)
    */

   // _bottom = -_bottom;
   // _top = -_top;

   sF32 A = (2.0f / (_right - _left));
   sF32 B = (2.0f / (_top - _bottom));
   sF32 C = (-2.0f / (_zfar - _znear));

   sF32 tx = -(_right + _left) / (_right - _left);
   sF32 ty = -(_top + _bottom) / (_top - _bottom);
   sF32 tz = -(_zfar + _znear) / (_zfar - _znear);

   sF32 tm[16];

   for(int i=0; i<16; i++)
   {
      tm[i] = _mat[i];
   }

   _mat[ 0] = (tm[ 0] * A);
   _mat[ 1] = (tm[ 1] * A);
   _mat[ 2] = (tm[ 2] * A);
   _mat[ 3] = (tm[ 3] * A);

   _mat[ 4] = (tm[ 4] * B);
   _mat[ 5] = (tm[ 5] * B);
   _mat[ 6] = (tm[ 6] * B);
   _mat[ 7] = (tm[ 7] * B);

   _mat[ 8] = (tm[ 8] * C);
   _mat[ 9] = (tm[ 9] * C);
   _mat[10] = (tm[10] * C);
   _mat[11] = (tm[11] * C);

   _mat[12] = (tm[ 0] * tx) + (tm[ 4] * ty) + (tm[ 8] * tz) + tm[12];
   _mat[13] = (tm[ 1] * tx) + (tm[ 5] * ty) + (tm[ 9] * tz) + tm[13];
   _mat[14] = (tm[ 2] * tx) + (tm[ 6] * ty) + (tm[10] * tz) + tm[14];
   _mat[15] = (tm[ 3] * tx) + (tm[ 7] * ty) + (tm[11] * tz) + tm[15];
}

// ---------------------------------------------------------------------------- tkopengl_matrix_mul
void tkopengl_matrix_mul(sF32 *_dmat, const sF32 *_mat, const sF32 *_omat) {

   _dmat[ 0] = (_mat[ 0] * _omat[ 0]) + (_mat[ 4] * _omat[ 1]) + (_mat[ 8] * _omat[ 2]) + (_mat[12] * _omat[ 3]);
   _dmat[ 1] = (_mat[ 1] * _omat[ 0]) + (_mat[ 5] * _omat[ 1]) + (_mat[ 9] * _omat[ 2]) + (_mat[13] * _omat[ 3]);
   _dmat[ 2] = (_mat[ 2] * _omat[ 0]) + (_mat[ 6] * _omat[ 1]) + (_mat[10] * _omat[ 2]) + (_mat[14] * _omat[ 3]);
   _dmat[ 3] = (_mat[ 3] * _omat[ 0]) + (_mat[ 7] * _omat[ 1]) + (_mat[11] * _omat[ 2]) + (_mat[15] * _omat[ 3]);

   _dmat[ 4] = (_mat[ 0] * _omat[ 4]) + (_mat[ 4] * _omat[ 5]) + (_mat[ 8] * _omat[ 6]) + (_mat[12] * _omat[ 7]);
   _dmat[ 5] = (_mat[ 1] * _omat[ 4]) + (_mat[ 5] * _omat[ 5]) + (_mat[ 9] * _omat[ 6]) + (_mat[13] * _omat[ 7]);
   _dmat[ 6] = (_mat[ 2] * _omat[ 4]) + (_mat[ 6] * _omat[ 5]) + (_mat[10] * _omat[ 6]) + (_mat[14] * _omat[ 7]);
   _dmat[ 7] = (_mat[ 3] * _omat[ 4]) + (_mat[ 7] * _omat[ 5]) + (_mat[11] * _omat[ 6]) + (_mat[15] * _omat[ 7]);

   _dmat[ 8] = (_mat[ 0] * _omat[ 8]) + (_mat[ 4] * _omat[ 9]) + (_mat[ 8] * _omat[10]) + (_mat[12] * _omat[11]);
   _dmat[ 9] = (_mat[ 1] * _omat[ 8]) + (_mat[ 5] * _omat[ 9]) + (_mat[ 9] * _omat[10]) + (_mat[13] * _omat[11]);
   _dmat[10] = (_mat[ 2] * _omat[ 8]) + (_mat[ 6] * _omat[ 9]) + (_mat[10] * _omat[10]) + (_mat[14] * _omat[11]);
   _dmat[11] = (_mat[ 3] * _omat[ 8]) + (_mat[ 7] * _omat[ 9]) + (_mat[11] * _omat[10]) + (_mat[15] * _omat[11]);

   _dmat[12] = (_mat[ 0] * _omat[12]) + (_mat[ 4] * _omat[13]) + (_mat[ 8] * _omat[14]) + (_mat[12] * _omat[15]);
   _dmat[13] = (_mat[ 1] * _omat[12]) + (_mat[ 5] * _omat[13]) + (_mat[ 9] * _omat[14]) + (_mat[13] * _omat[15]);
   _dmat[14] = (_mat[ 2] * _omat[12]) + (_mat[ 6] * _omat[13]) + (_mat[10] * _omat[14]) + (_mat[14] * _omat[15]);
   _dmat[15] = (_mat[ 3] * _omat[12]) + (_mat[ 7] * _omat[13]) + (_mat[11] * _omat[14]) + (_mat[15] * _omat[15]);
}

// ---------------------------------------------------------------------------- tkopengl_matrix_mul_inplace
void tkopengl_matrix_mul_inplace(sF32 *_mat, const sF32 *_omat) {
   sF32 tm[16];

   for(int i=0; i<16; i++)
   {
      tm[i] = _mat[i];
   }

   tkopengl_matrix_mul(_mat, tm, _omat);
}

// ---------------------------------------------------------------------------- tkopengl_matrix_update
void tkopengl_matrix_update(void) {

   // Called immediately before rendering

   sBool bUpdateTransform = YAC_FALSE;

   if(0 != (tkopengl_matrix_flags[TKOPENGL_MATRIX_MODE_MODELVIEW] & TKOPENGL_MATRIX_FLAG_DIRTY))
   {
#ifndef DX_EMU_FIXFXN
      ::glMatrixMode(GL_MODELVIEW);
      ::glLoadMatrixf(&tkopengl_modelview_matrix_stack[tkopengl_modelview_matrix_stacki << 4]);
#endif // DX_EMU_FIXFXN

      tkopengl_matrix_flags[TKOPENGL_MATRIX_MODE_MODELVIEW] &= ~TKOPENGL_MATRIX_FLAG_DIRTY;

#if 0
      yac_host->printf("[dbg] tkopengl_matrix_update: GL_MODELVIEW\n");
      {
         sF32 *m = &tkopengl_modelview_matrix_stack[tkopengl_modelview_matrix_stacki << 4];
         yac_host->printf("  %f %f %f %f\n  %f %f %f %f\n  %f %f %f %f\n  %f %f %f %f\n",
                          m[ 0], m[ 4], m[ 8], m[12],
                          m[ 1], m[ 5], m[ 9], m[13],
                          m[ 2], m[ 6], m[10], m[14],
                          m[ 3], m[ 7], m[11], m[15]
                          );
      }
#endif

      bUpdateTransform = YAC_TRUE;

#ifdef DX_EMU_LIGHT
      if(tkopengl_attrib->light.b_enable && (0 != (tkopengl_attrib->light.enable_mask)))
      {
         sF32 *s = &tkopengl_modelview_matrix_stack[tkopengl_modelview_matrix_stacki << 4];

         if(0 != (tkopengl_matrix_flags[TKOPENGL_MATRIX_MODE_MODELVIEW] & TKOPENGL_MATRIX_FLAG_ORTHOGONAL))
         {
            // 4x4 => 3x3
            tkopengl_normalmatrix[0] = s[0];
            tkopengl_normalmatrix[1] = s[1];
            tkopengl_normalmatrix[2] = s[2];

            tkopengl_normalmatrix[3] = s[4];
            tkopengl_normalmatrix[4] = s[5];
            tkopengl_normalmatrix[5] = s[6];

            tkopengl_normalmatrix[6] = s[8];
            tkopengl_normalmatrix[7] = s[9];
            tkopengl_normalmatrix[8] = s[10];
         }
         else
         {
            yac_host->printf("[dbg] tkopengl_matrix_update: transpose/invert modelview\n");
            tkopengl_matrix_transpose_invert3x3(tkopengl_normalmatrix, s);
         }
      }
#endif // DX_EMU_LIGHT
   }

   if(0 != (tkopengl_matrix_flags[TKOPENGL_MATRIX_MODE_PROJECTION] & TKOPENGL_MATRIX_FLAG_DIRTY))
   {
#ifndef DX_EMU_FIXFXN
      ::glMatrixMode(GL_PROJECTION);
      ::glLoadMatrixf(&tkopengl_projection_matrix_stack[tkopengl_projection_matrix_stacki << 4]);
#endif // DX_EMU_FIXFXN

      tkopengl_matrix_flags[TKOPENGL_MATRIX_MODE_PROJECTION] &= ~TKOPENGL_MATRIX_FLAG_DIRTY;

#if 0
      yac_host->printf("[dbg] tkopengl_matrix_update: GL_PROJECTION\n");
      {
         sF32 *m = &tkopengl_projection_matrix_stack[tkopengl_projection_matrix_stacki << 4];
         yac_host->printf("  %f %f %f %f\n  %f %f %f %f\n  %f %f %f %f\n  %f %f %f %f\n",
                          m[ 0], m[ 4], m[ 8], m[12],
                          m[ 1], m[ 5], m[ 9], m[13],
                          m[ 2], m[ 6], m[10], m[14],
                          m[ 3], m[ 7], m[11], m[15]
                          );
      }
#endif

      bUpdateTransform = YAC_TRUE;
   }

#ifdef DX_EMU_FIXFXN
   if(bUpdateTransform)
   {
      tkopengl_matrix_mul(tkopengl_transform,
                          &tkopengl_projection_matrix_stack[tkopengl_projection_matrix_stacki << 4],
                          &tkopengl_modelview_matrix_stack[tkopengl_modelview_matrix_stacki << 4]
                          );
   }
#endif // DX_EMU_FIXFXN
}

#endif // DX_EMU_MATRIX

// ---------------------------------------------------------------------------- tkopengl_matrix_transform_vec4
void tkopengl_matrix_transform_vec4(sF32 *_dvec, const sF32 *_mat, const sF32 *_svec) {
   _dvec[0] = (_mat[0] * _svec[0]) + (_mat[4] * _svec[1]) + (_mat[ 8] * _svec[2]) + (_mat[12] * _svec[3]);
   _dvec[1] = (_mat[1] * _svec[0]) + (_mat[5] * _svec[1]) + (_mat[ 9] * _svec[2]) + (_mat[13] * _svec[3]);
   _dvec[2] = (_mat[2] * _svec[0]) + (_mat[6] * _svec[1]) + (_mat[10] * _svec[2]) + (_mat[14] * _svec[3]);
   _dvec[3] = (_mat[3] * _svec[0]) + (_mat[7] * _svec[1]) + (_mat[11] * _svec[2]) + (_mat[15] * _svec[3]);
}

// ---------------------------------------------------------------------------- tkopengl_matrix_det3x3
sF32 tkopengl_matrix_det3x3(const sF32 *_mat4) {
   // calc determinant of 3x3 submatrix
   sF32 r =
      (_mat4[0*4 + 0] * _mat4[1*4 + 1] * _mat4[2*4 + 2]) -
      (_mat4[0*4 + 0] * _mat4[2*4 + 1] * _mat4[1*4 + 2]) -
      (_mat4[1*4 + 0] * _mat4[0*4 + 1] * _mat4[2*4 + 2]) +
      (_mat4[1*4 + 0] * _mat4[2*4 + 1] * _mat4[0*4 + 2]) +
      (_mat4[2*4 + 0] * _mat4[0*4 + 1] * _mat4[1*4 + 2]) -
      (_mat4[2*4 + 0] * _mat4[1*4 + 1] * _mat4[0*4 + 2]) ;

   return r;
}

// ---------------------------------------------------------------------------- tkopengl_matrix_invert3x3
void tkopengl_matrix_transpose_invert3x3(sF32 *_mat3, const sF32 *_mat4) {

   // invert 3x3 submatrix and transpose it

   sF32 det = tkopengl_matrix_det3x3(_mat4);

   if(0.0f != det)
   {
      det = 1.0f / det;

      _mat3[0*3 + 0] = det * ((_mat4[1*4 + 1] * _mat4[2*4 + 2]) - (_mat4[2*4 +1] * _mat4[1*4 + 2]));
      _mat3[1*3 + 0] = det * ((_mat4[2*4 + 1] * _mat4[0*4 + 2]) - (_mat4[0*4 +1] * _mat4[2*4 + 2]));
      _mat3[2*3 + 0] = det * ((_mat4[0*4 + 1] * _mat4[1*4 + 2]) - (_mat4[1*4 +1] * _mat4[0*4 + 2]));

      _mat3[0*3 + 1] = det * ((_mat4[2*4 + 0] * _mat4[1*4 + 2]) - (_mat4[1*4 +0] * _mat4[2*4 + 2]));
      _mat3[1*3 + 1] = det * ((_mat4[0*4 + 0] * _mat4[2*4 + 2]) - (_mat4[2*4 +0] * _mat4[0*4 + 2]));
      _mat3[2*3 + 1] = det * ((_mat4[1*4 + 0] * _mat4[0*4 + 2]) - (_mat4[0*4 +0] * _mat4[1*4 + 2]));

      _mat3[0*3 + 2] = det * ((_mat4[1*4 + 0] * _mat4[2*4 + 1]) - (_mat4[2*4 +0] * _mat4[1*4 + 1]));
      _mat3[1*3 + 2] = det * ((_mat4[2*4 + 0] * _mat4[0*4 + 1]) - (_mat4[0*4 +0] * _mat4[2*4 + 1]));
      _mat3[2*3 + 2] = det * ((_mat4[0*4 + 0] * _mat4[1*4 + 1]) - (_mat4[1*4 +0] * _mat4[0*4 + 1]));
   }
   else
   {
      // cannot invert matrix
      yac_host->printf("[---] tkopengl_matrix_invert3x3: cannot invert matrix (det=0)\n");
   }
}
