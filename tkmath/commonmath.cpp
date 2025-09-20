/// commonmath.cpp
///
/// (c) 2008-2024 by Carsten Busse <carsten.busse@googlemail.com>
///                  Bastian Spiegel <bs@tkscript.de> (additional coding)
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#include <math.h>

#include "tkmath_yac.h"
#include "commonmath.h"
#include "tkmath.h"

// Generate old-style function names for (some) C99 functions
#define c99oldname(a,b) a##b
#define c99err(n) n ## not available
#ifdef YAC_GCC
#define c99call1(n,a,b) n(a)
#define c99call2(n,a,b) n(a,b)
#else
#ifdef _MSC_VER
#define c99call1(n,a) c99oldname(_,n)(a)
#define c99call2(n,a,b) c99oldname(_,n)(a, b)
#else
   #error missing c99 function
#endif
#endif


// Convert object to double value.
// Should be a define so that the exception line number is correct
#define Dgetdouble(o, r, b) \
   if(YAC_VALID(o))\
   {\
      o->yacScanF64(&r);\
      b = 1;\
   }\
   else\
   {\
      yac_host->yacExceptionRaise(yac_host->yacContextGetDefault(), exid_InvalidPointer, "Dgetdouble", __FILE__, __LINE__);\
      b = 0;\
   }

#define Dgetdouble1(o) \
   sBool b;\
   sF64 v;\
   Dgetdouble(o, v, b)


#define Dreturndouble(v) \
   YAC_Double *d = (YAC_Double*) YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);\
   d->value = v;\
  _r->initObject(d, 1)
  
//the variables epsilon_flt* and epsilon_dbl* are in yac_host
void _Math::_setFltEpsilonUnits(sSI units) {
   if (units != 0)
      yac_epsilon_flt_units=abs(units);
}

void _Math::_setDblEpsilonUnits(sSI units) {
   if (units != 0)
      yac_epsilon_dbl_units=(sS64)abs(units);
}

sSI _Math::_getFltEpsilonUnits() {
   return yac_epsilon_flt_units;
}

sSI _Math::_getDblEpsilonUnits() {
   return (sSI) yac_epsilon_dbl_units;
}

void _Math::_setFltEpsilonZeroRange(sF32 epsilon) {
   if (epsilon > 0.0f)
      yac_epsilon_flt=epsilon;
}

void _Math::_setDblEpsilonZeroRange(YAC_Object *_epsilon) {
   sF64 epsilon;
   if (YAC_VALID(_epsilon)) {
      if (_epsilon->yacScanF64(&epsilon)) {
         if (epsilon>0.0)
            yac_epsilon_dbl=epsilon;
      }
   }
}

sF32 _Math::_getFltEpsilonZeroRange(void) {
   return yac_epsilon_flt;
}

void _Math::_getDblEpsilonZeroRange(YAC_Value *_r) {
   YAC_Double *r = (YAC_Double*) YAC_NEW_CORE_POOLED(YAC_CLID_DOUBLE);
   r->value = yac_epsilon_dbl;
   _r->initObject(r,1);
}

sSI YAC_CALL _yac_relCmpf(sF32 a, sF32 b) {
   return yac_fltcmp_rel_fast(a,b);
}

sSI YAC_CALL _yac_relCmpd(YAC_Object *a, YAC_Object *b) {
   if (YAC_VALID(a)&&YAC_VALID(b)) {
      sF64 _a, _b;
      if (!a->yacScanF64(&_a)) _a=0.0;
      if (!b->yacScanF64(&_b)) _b=0.0;
      return yac_dblcmp_rel_fast(_a,_b);
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::relCmpd Objects not valid");
      return -2; //exception is thrown but we must return something
   }
}

sSI YAC_CALL _yac_relErrCmpf(sF32 a, sF32 b, sF32 err) {
   return yac_fltcmp_rel(a,b,err);
}

sSI YAC_CALL _yac_relErrCmpd(YAC_Object *_a, YAC_Object *_b, YAC_Object *_err) {
   sF64 a,b,err;
   if (YAC_VALID(_a)&&YAC_VALID(_b)&&YAC_VALID(_err)) {
      if (!_a->yacScanF64(&a)) a=0.0;
      if (!_b->yacScanF64(&b)) b=0.0;
      if (!_err->yacScanF64(&err)) err=0.0;
      return yac_dblcmp_rel(a,b,err);
   } else {
      Dyac_throw_def(TkMathInvalidValue, "tkmath::relErrCmpd Objects not valid");
      return -2;
   }
}

sF32 YAC_CALL _yac_sinf(sF32 v) {
   return sinf(v);
}

sF32 YAC_CALL _yac_cosf(sF32 v) {
   return cosf(v);
}

sF32 YAC_CALL _yac_tanf(sF32 v) {
   return tanf(v);
}

sF32 YAC_CALL _yac_asinf(sF32 v) {
   return asinf(v);
}

sF32 YAC_CALL _yac_acosf(sF32 v) {
   return acosf(v);
}

sF32 YAC_CALL _yac_atanf(sF32 v) {
   return atanf(v);
}

sF32 YAC_CALL _yac_atan2f(sF32 y, sF32 x) {
   return atan2f(y,x);
}

sF32 YAC_CALL _yac_hypotf(sF32 x, sF32 y) {
#ifdef HAVE_HYPOTF
   return c99call2(hypotf, x, y);
#else
   return hypotf_emul(x, y);
#endif // HAVE_HYPOTF
}

sF32 YAC_CALL _yac_sinhf(sF32 v) {
   return sinhf(v);
}

sF32 YAC_CALL _yac_coshf(sF32 v) {
   return coshf(v);
}

sF32 YAC_CALL _yac_tanhf(sF32 v) {
   return tanhf(v);
}

sF32 YAC_CALL _yac_asinhf(sF32 v) {
#ifdef YAC_GCC
   return asinhf(v);
#else
   return (sF32) ((v<0?-1.0f:1.0f) * logf((v<0?-v:v) + sqrtf(v * v + 1)));
#endif
}


sF32 YAC_CALL _yac_acoshf(sF32 v) {
#ifdef YAC_GCC
   return acoshf(v);
#else
   return (sF32) log(v + sqrt(v * v - 1));
#endif // YAC_GCC
}


sF32 YAC_CALL _yac_atanhf(sF32 v) {
#ifdef YAC_GCC
   return atanhf(v);
#else
   return (sF32) (0.5f * logf( (1 + v) / (1 - v) ));
#endif // YAC_GCC
}


sF32 YAC_CALL _yac_absf(sF32 v) {
   return fabsf(v);
}

sF32 YAC_CALL _yac_ceilf(sF32 v) {
   return ceilf(v);
}

sF32 YAC_CALL _yac_roundf(sF32 v) {
#ifdef YAC_GCC
   return roundf(v);
#else
   return (sF32) ((sSI)(v+0.5f));
#endif // YAC_GCC
}

sF32 YAC_CALL _yac_floorf(sF32 v) {
   return floorf(v);
}

sF32 YAC_CALL _yac_modf(sF32 a, sF32 b) {
   return fmodf(a,b);
}

sF32 YAC_CALL _yac_sqrtf(sF32 v) {
   return sqrtf(v);
}

sF32 YAC_CALL _yac_powf(sF32 b, sF32 e) {
   return powf(b,e);
}

sF32 YAC_CALL _yac_expf(sF32 v) {
   return expf(v);
}

#if 0
sF32 YAC_CALL _yac_exp2f(sF32 v) {
   return exp2f(v);
}
#endif // 0

sF32 YAC_CALL _yac_exp2f(sF32 v) {
#ifdef YAC_C99
   return exp2f(v);
#else
   return expf((sF32)sM_LN2*v);
#endif
}

#if 0
sF32 YAC_CALL _yac_exp10f(sF32 v) {
   return _exp10f(v);
}
#endif // 0

sF32 YAC_CALL _yac_exp10f(sF32 v) {
#ifdef YAC_C99
   return exp10f(v);
#else
   return expf((sF32)sM_LN10*v);
#endif
}

sF32 YAC_CALL _yac_logf(sF32 v) {
   return logf(v);
}

#if 0
sF32 YAC_CALL _yac_log2f(sF32 v) {
   return log2f(v);
}
#endif // 0

sF32 YAC_CALL _yac_log2f(sF32 v) {
#ifdef YAC_GCC
   return log2f(v);
#else
   return logf(v)*(sF32)sM_LOG2E;
#endif
}

sF32 YAC_CALL _yac_log10f(sF32 v) {
   return log10f(v);
}



void YAC_CALL _yac_sind(YAC_Object *_d, YAC_Value *_r) {
   Dgetdouble1(_d);
   if(b)
   {
      Dreturndouble(sin(v));
   }
}

void YAC_CALL _yac_cosd(YAC_Object *_d, YAC_Value *_r) {
   Dgetdouble1(_d);
   if(b)
   {
      Dreturndouble(cos(v));
   }
}

void YAC_CALL _yac_tand(YAC_Object *_d, YAC_Value *_r) {
   Dgetdouble1(_d);
   if(b)
   {
      Dreturndouble(tan(v));
   }
}

void YAC_CALL _yac_asind(YAC_Object *_d, YAC_Value *_r) {
   Dgetdouble1(_d);
   if(b)
   {
      Dreturndouble(asin(v));
   }
}

void YAC_CALL _yac_acosd(YAC_Object *_d, YAC_Value *_r) {
   Dgetdouble1(_d);
   if(b)
   {
      Dreturndouble(acos(v));
   }
}

void YAC_CALL _yac_atand(YAC_Object *_d, YAC_Value *_r) {
   Dgetdouble1(_d);
   if(b)
   {
      Dreturndouble(atan(v));
   }
}

void YAC_CALL _yac_atan2d(YAC_Object *_y, YAC_Object *_x, YAC_Value *_r) {
   sBool b;
   sF64 y, x;
   Dgetdouble(_y, y, b);
   if(b)
   {
      Dgetdouble(_x, x, b);
      if(b)
      {
         Dreturndouble(atan2(y,x));
      }
   }
}

void YAC_CALL _yac_hypotd(YAC_Object *_x, YAC_Object *_y, YAC_Value *_r) {
   sBool b;
   sF64 x, y;
   Dgetdouble(_x, x, b);
   if(b)
   {
      Dgetdouble(_y, y, b);
      if(b)
      {
#ifdef YAC_GCC
         Dreturndouble(hypot(x, y));
#else
         Dreturndouble(_hypot(x, y));
#endif // YAC_GCC
      }
   }
}

void YAC_CALL _yac_sinhd(YAC_Object *_d, YAC_Value *_r) {
   Dgetdouble1(_d);
   if(b)
   {
      Dreturndouble(sinh(v));
   }
}

void YAC_CALL _yac_coshd(YAC_Object *_d, YAC_Value *_r) {
   Dgetdouble1(_d);
   if(b)
   {
      Dreturndouble(cosh(v));
   }
}

void YAC_CALL _yac_tanhd(YAC_Object *_d, YAC_Value *_r) {
   Dgetdouble1(_d);
   if(b)
   {
      Dreturndouble(tanh(v));
   }
}

void YAC_CALL _yac_asinhd(YAC_Object *_d, YAC_Value *_r) {
   Dgetdouble1(_d);
   if(b)
   {
#ifdef YAC_GCC
      Dreturndouble(asinh(v));
#else
      Dreturndouble((sF64) log(v + sqrt(v*v +1)));
#endif // YAC_GCC
   }
}

void YAC_CALL _yac_acoshd(YAC_Object *_d, YAC_Value *_r) {
   Dgetdouble1(_d);
   if(b)
   {
#ifdef YAC_GCC
      Dreturndouble(acosh(v));
#else
      Dreturndouble((sF64) log(v + sqrt(v * v - 1)));
#endif // YAC_GCC
   }
}

void YAC_CALL _yac_atanhd(YAC_Object *_d, YAC_Value *_r) {
   Dgetdouble1(_d);
   if(b)
   {
#ifdef YAC_GCC
      Dreturndouble(atanh(v));
#else
      Dreturndouble((sF64) 0.5 * log( (1+v) / (1-v) ));
#endif // YAC_GCC
   }
}

void YAC_CALL _yac_absd(YAC_Object *_d, YAC_Value *_r) {
   Dgetdouble1(_d);
   if(b)
   {
      Dreturndouble(fabs(v));
   }
}

void YAC_CALL _yac_ceild(YAC_Object *_d, YAC_Value *_r) {
   Dgetdouble1(_d);
   if(b)
   {
      Dreturndouble(ceil(v));
   }
}

void YAC_CALL _yac_roundd(YAC_Object *_d, YAC_Value *_r) {
   Dgetdouble1(_d);
   if(b)
   {
#ifdef YAC_GCC
      Dreturndouble(round(v));
#else
      Dreturndouble((sF64) ((sSI)(v + 0.5)));
#endif // YAC_GCC
   }
}

void YAC_CALL _yac_floord(YAC_Object *_d, YAC_Value *_r) {
   Dgetdouble1(_d);
   if(b)
   {
      Dreturndouble(floor(v));
   }
}

void YAC_CALL _yac_modd(YAC_Object *_a, YAC_Object *_b, YAC_Value *_r) {
   sBool b;
   sF64 aa, bb;
   Dgetdouble(_a, aa, b);
   if(b)
   {
      Dgetdouble(_b, bb, b);
      if(b)
      {
         Dreturndouble(fmod(aa, bb));
      }
   }
}

void YAC_CALL _yac_sqrtd(YAC_Object *_d, YAC_Value *_r) {
   Dgetdouble1(_d);
   if(b)
   {
      Dreturndouble(sqrt(v));
   }
}

void YAC_CALL _yac_powd(YAC_Object *_b, YAC_Object *_e, YAC_Value *_r) {
   sBool b;
   sF64 bb, ee;
   Dgetdouble(_b, bb, b);
   if(b)
   {
      Dgetdouble(_e, ee, b);
      if(b)
      {
         Dreturndouble(pow(bb, ee));
      }
   }
}

void YAC_CALL _yac_expd(YAC_Object *_d, YAC_Value *_r) {
   Dgetdouble1(_d);
   if(b)
   {
      Dreturndouble(exp(v));
   }
}

#if 0
void YAC_CALL _yac_exp2d(YAC_Object *_d, YAC_Value *_r) {
   Dgetdouble1(_d);
   if(b)
   {
      Dreturndouble(exp2(v));
   }
}
#endif // 0

void YAC_CALL _yac_exp2d(YAC_Object *_d, YAC_Value *_r) {
   Dgetdouble1(_d);
   if (b)
   {
#ifdef YAC_GCC
      Dreturndouble(exp2(v));
#else
      Dreturndouble(exp(sM_LN2*v));
#endif
   }
}

#if 0
sF64 YAC_CALL _yac_exp10d(sF64 v) {
   return exp10(v);
}
#endif // 0

void YAC_CALL _yac_exp10d(YAC_Object *_d, YAC_Value *_r) {
   Dgetdouble1(_d);
   if (b)
   {
#ifdef YAC_C99
      Dreturndouble(exp10(v));
#else
      Dreturndouble(exp(sM_LN10*v));
#endif
   }
}

void YAC_CALL _yac_logd(YAC_Object *_d, YAC_Value *_r) {
   Dgetdouble1(_d);
   if(b)
   {
      Dreturndouble(log(v));
   }
}

#if 0
sF64 YAC_CALL _yac_log2d(sF64 v) {
   return log2(v);
}
#endif // 0

void YAC_CALL _yac_log2d(YAC_Object *_d, YAC_Value *_r) {
   Dgetdouble1(_d);
   if (b)
   {
#ifdef YAC_GCC
      Dreturndouble(log2(v));
#else
      Dreturndouble(log(v)*sM_LOG2E);
#endif
   }
}

void YAC_CALL _yac_log10d(YAC_Object *_d, YAC_Value *_r) {
   Dgetdouble1(_d);
   if(b)
   {
      Dreturndouble(log10(v));
   }
}

sSI YAC_CALL _yac_f16_from_f32(sF32 _v) {
   static const sF32 FLOAT16_MAX =  65472.0f;
   static const sF32 FLOAT16_MIN =  -FLOAT16_MAX;

   _v = sMIN(sMAX(_v, FLOAT16_MIN), FLOAT16_MAX);

   yacmem32 m; m.f32 = _v;
   
   // Convert exponent from 8 bits (bias = 127) to 5 bits (bias = 15)
   sS16 r = (0 != m.u32) ? (sS16( ( (sS8(m.u32 >> 23) & 255) -  127) + 15 ) << 10) : 0;

   // Convert mantissa bits from 23 bits to 10 bits
   r |= (m.u32 & 8388607u) >> (23 - 10);

   // Add sign bit
   r |= (m.u32 >> 31) << 15;

   return r;
}

sF32 YAC_CALL _yac_f32_from_f16(sSI _v) {
   yacmem32 r; r.u32 = 0;

   // Convert exponent from 5 bits (bias = 15) to 8 bits (bias = 127)
   r.u32 = (0 != _v) ? (sUI( ( (sS8(_v >> 10) & 31) -  15) + 127 ) << 23) : 0;

   // Convert mantissa bits from 10 bits to 23 bits
   r.u32 |= (_v & 1023u) << (23 - 10);

   // Add sign bit
   r.u32 |= (_v >> 15) << 31;

   return r.f32;
}

sSI YAC_CALL _yac_fx16_from_f16(sSI _v) {
   return (sSI)(_yac_f32_from_f16(_v) * 65536.0f);
}
