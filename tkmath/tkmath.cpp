/// tkmath.cpp
///
/// (c) 2008-2024 by Carsten Busse <carsten.busse@googlemail.com>,
///                  Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#include <math.h>

#include "tkmath_yac.h"

#ifndef DX_STATIC_TKMATH
#include <yac_host.cpp>
#endif

#include "tkmath.h"

#include "commonmath.h"

#include "matrix.h"
#include "vector2f.h"
#include "matrix2f.h"
#include "complexf.h"
#include "vector3f.h"
#include "matrix2x3f.h"
#include "matrix3f.h"
#include "vector4f.h"
#include "matrix4f.h"
#include "quaternionf.h"

#include "complexd.h"
#include "vector2d.h"
#include "matrix2d.h"
#include "vector3d.h"
#include "matrix3d.h"
#include "vector4d.h"
#include "matrix4d.h"
#include "quaterniond.h"

#ifndef DX_STATIC_TKMATH
YAC_Host *yac_host;
#endif // DX_STATIC_TKMATH

// Implement "standard" exid_* variables (e.g. exid_InvalidPointer)
Dyac_std_exid_impl;

// Implement tkmath specific exid_* variables
Dyac_exid_impl(TkMathError);
Dyac_exid_impl(TkMathDivisionByZero);
Dyac_exid_impl(TkMathInvalidValue);
Dyac_exid_impl(TkMathUnsupportedOperator);


#include "ying_Math.h"

#include "ying_Math.cpp"
#include "ying_Math_Math.cpp"
#include "ying_Math_Matrix.cpp"

#include "ying_Math_Vector2f.cpp"
#include "ying_Math_Point2f.cpp"
#include "ying_Math_Size2f.cpp"

#include "ying_Math_Vector2d.cpp"
#include "ying_Math_Point2d.cpp"
#include "ying_Math_Size2d.cpp"

#include "ying_Math_Vector3f.cpp"
#include "ying_Math_Point3f.cpp"
#include "ying_Math_Size3f.cpp"

#include "ying_Math_Vector3d.cpp"
#include "ying_Math_Point3d.cpp"
#include "ying_Math_Size3d.cpp"

#include "ying_Math_Vector4f.cpp"
#include "ying_Math_Point4f.cpp"
#include "ying_Math_Size4f.cpp"

#include "ying_Math_Vector4d.cpp"
#include "ying_Math_Point4d.cpp"
#include "ying_Math_Size4d.cpp"

#include "ying_Math_Matrix2f.cpp"
#include "ying_Math_Matrix2d.cpp"

#include "ying_Math_Matrix2x3f.cpp"

#include "ying_Math_Matrix3f.cpp"
#include "ying_Math_Matrix3d.cpp"

#include "ying_Math_Matrix4f.cpp"
#include "ying_Math_Matrix4d.cpp"

#include "ying_Math_Complexf.cpp"
#include "ying_Math_Complexd.cpp"

#include "ying_Math_Quaternionf.cpp"
#include "ying_Math_Quaterniond.cpp"


#ifndef HAVE_HYPOTF
// from gcc/libstdc++-v3/src/maths_stubs_float.cc
sF32 hypotf_emul(sF32 x, sF32 y)
{
   sF32 s = fabsf(x) + fabsf(y);
   if(0.0f == s)
      return s;
   x /= s;
   y /= s;
   return s * sqrtf(x * x + y * y);
}
#endif // HAVE_HYPOTF


#ifdef DX_STATIC_TKMATH
void YAC_CALL YAC_Init_tkmath(YAC_Host *_host) {
#else
void YAC_CALL YAC_Init(YAC_Host *_host) {
#endif // DX_STATIC_TKMATH

   if(_host->yacGetDebugLevel())
      _host->printf("[dbg] tkmath::YAC_Init called\n");

   yac_host = _host;
   YAC_Init_Math(_host);

   // Matrix inheritance
   yac_host->cpp_typecast_map[clid_Matrix2f]  [clid_Matrix]   = 1;
   yac_host->cpp_typecast_map[clid_Matrix2x3f][clid_Matrix]   = 1;
   yac_host->cpp_typecast_map[clid_Matrix3f]  [clid_Matrix]   = 1;
   yac_host->cpp_typecast_map[clid_Matrix4f]  [clid_Matrix]   = 1;

   yac_host->cpp_typecast_map[clid_Matrix2d][clid_Matrix]   = 1;
   yac_host->cpp_typecast_map[clid_Matrix3d][clid_Matrix]   = 1;
   yac_host->cpp_typecast_map[clid_Matrix4d][clid_Matrix]   = 1;

   // Utility class inheritance
   yac_host->cpp_typecast_map[clid_Point2f ][clid_Vector2f] = 1;
   yac_host->cpp_typecast_map[clid_Size2f  ][clid_Vector2f] = 1;

   yac_host->cpp_typecast_map[clid_Point2d ][clid_Vector2d] = 1;
   yac_host->cpp_typecast_map[clid_Size2d  ][clid_Vector2d] = 1;

   yac_host->cpp_typecast_map[clid_Point3f ][clid_Vector3f] = 1;
   yac_host->cpp_typecast_map[clid_Size3f  ][clid_Vector3f] = 1;

   yac_host->cpp_typecast_map[clid_Point3d ][clid_Vector3d] = 1;
   yac_host->cpp_typecast_map[clid_Size3d  ][clid_Vector3d] = 1;

   yac_host->cpp_typecast_map[clid_Point4f ][clid_Vector4f] = 1;
   yac_host->cpp_typecast_map[clid_Size4f  ][clid_Vector4f] = 1;

   yac_host->cpp_typecast_map[clid_Point4d ][clid_Vector4d] = 1;
   yac_host->cpp_typecast_map[clid_Size4d  ][clid_Vector4d] = 1;

   // Resolve "standard" exception ids
   Dyac_std_exid_resolve;

   // Register tkmath specific exception ids
   Dyac_exid_register(TkMathError,               CriticalError);
   Dyac_exid_register(TkMathDivisionByZero,      TkMathError);
   Dyac_exid_register(TkMathInvalidValue,        TkMathError);
   Dyac_exid_register(TkMathUnsupportedOperator, TkMathError);

   if(yac_host->yacGetDebugLevel())
      yac_host->printf("[dbg] tkmath::YAC_Init finished\n");
}

#ifdef DX_STATIC_TKMATH
void YAC_CALL YAC_Exit_tkmath(YAC_Host *_host) {
#else
void YAC_CALL YAC_Exit(YAC_Host *_host) {
#endif // DX_STATIC_TKMATH
   YAC_Exit_Math(_host);
   if(yac_host->yacGetDebugLevel())
   {
      yac_host->printf("[dbg] tkmath::YAC_Exit finished\n");
   }
}
