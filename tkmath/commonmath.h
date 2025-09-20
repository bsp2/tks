/// commonmath.h
///
/// (c) 2008-2024 by Carsten Busse <carsten.busse@googlemail.com>
///                  Bastian Spiegel <bs@tkscript.de> (additional coding)
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#ifndef COMMONMATH_H_
#define COMMONMATH_H_


YG("Math");

/* @class Math,Object

This class is mainly for importing the most commonly used math constants from math.h
 */

YCS class _Math : public YAC_Object {
	YAC(_Math);

/* @constant M_E,float
e=2.7182818284590452354f
 */
//#define M_E            YCF 2.7182818284590452354f   /* e */

/* @constant M_LOG2E,float
log<sub>2</sub>(e)=1.4426950408889634074f
 */
//#define M_LOG2E        YCF 1.4426950408889634074f   /* log_2 e */

/* @constant M_LOG10E,float
log<sub>10</sub>(e)=0.43429448190325182765f
 */
//#define M_LOG10E       YCF 0.43429448190325182765f  /* log_10 e */

/* @constant M_LN2,float
log(2)=0.69314718055994530942f
 */
//#define M_LN2          YCF 0.69314718055994530942f  /* log_e 2 */

/* @constant M_LN10,float
log(10)=2.30258509299404568402f
 */
//#define M_LN10         YCF 2.30258509299404568402f  /* log_e 10 */

/* @constant M_PI,float
&#x3c0;=3.14159265358979323846f
 */
//#define M_PI           YCF 3.14159265358979323846f  /* pi */

/* @constant M_PI_2,float
&#x3c0;/2=1.57079632679489661923f
 */
//#define M_PI_2         YCF 1.57079632679489661923f  /* pi/2 */

/* @constant M_PI_4,float
&#x3c0;/4=0.78539816339744830962f
 */
//#define M_PI_4         YCF 0.78539816339744830962f  /* pi/4 */

/* @constant M_1_PI,float
1/&#x3c0;=0.31830988618379067154f
 */
//#define M_1_PI         YCF 0.31830988618379067154f  /* 1/pi */

/* @constant M_2_PI,float
2/&#x3c0;=0.63661977236758134308f
 */
//#define M_2_PI         YCF 0.63661977236758134308f  /* 2/pi */

/* @constant M_2_SQRTPI,float
2/&#x221a;&#x3c0;=1.12837916709551257390f
 */
//#define M_2_SQRTPI     YCF 1.12837916709551257390f  /* 2/sqrt(pi) */

/* @constant M_SQRT2,float
&#x221a;2=1.41421356237309504880f
 */
//#define M_SQRT2        YCF 1.41421356237309504880f  /* sqrt(2) */

/* @constant M_SQRT1_2,float
1/&#x221a;2=0.70710678118654752440f
 */
//#define M_SQRT1_2      YCF 0.70710678118654752440f  /* 1/sqrt(2) */

   public:

   /* @method setFltEpsilonUnits,int u
      Set the relative unit range for relative float comparison
      @arg u new range
    */

   YM void _setFltEpsilonUnits (sSI);

   /* @method setDblEpsilonUnits,int u
      Set the relative unit range for relative Double comparison
      @arg u new range
    */
   YM void _setDblEpsilonUnits (sSI);

   /* @method getFltEpsilonUnits:int
      Returns current unit range for relative float comparison, default is 10
      @return current range
    */
   YM sSI _getFltEpsilonUnits (void);

   /* @method getDblEpsilonUnits:int
      Returns current unit range for relative Double comparison, default is 100
      @return current range
    */
   YM sSI _getDblEpsilonUnits (void);

   /* @method setFltEpsilonZeroRange,float a
      Set the relative tolerance for float epsilon range checks
      @arg a new float epsilon range, must be bigger than zero
    */
   YM void _setFltEpsilonZeroRange (sF32);

   /* @method setDblEpsilonZeroRange,Double a
      Set the relative tolerance for Double epsilon range checks
      @arg a new double epsilon range, must be bigger than zero
    */
   YM void _setDblEpsilonZeroRange (YAC_Object *);

   /* @method getFltEpsilonZeroRange:float
      Returns the epsilon range for relative zero float comparison, default is 0.000001
      @return float epsilon range
    */
   YM sF32 _getFltEpsilonZeroRange (void);

   /* @method getDblEpsilonZeroRange:Double
      Returns the epsilon range for relative zero Double comparison, default is 0.000000000001
      @return Double epsilon range
    */
   YM void _getDblEpsilonZeroRange (YAC_Value*);
};


/* @function relCmpf,float a,float b:int
   Do a fast relative comparison of float values
   @arg a first value
   @arg b second value
   @return 0 if equal, -1 if smaller or 1 if bigger
 */
YF sSI YAC_CALL _yac_relCmpf (sF32, sF32);

/* @function relCmpd,Double a,Double b:int
   Do a fast relative comparison of Double values
   @arg a first value
   @arg b second value
   @return 0 if equal, -1 if smaller or 1 if bigger, -2 on error
 */
YF sSI YAC_CALL _yac_relCmpd (YAC_Object *, YAC_Object *);

/* @function relErrCmpf,float a,float b,float err:int
   Does a relative comparison given a certain error range of float values.
   If both values are nonzero, equal means it fullfills fabs(a-b)/fabs(a)<=err, else it fullfills fabs(nonzero(a,b))<=err.
   @arg a first value
   @arg b second value
   @arg err relative error to tolerate for being considered as equal, if zero default is used
   @return 0 if equal, -1 if smaller or 1 if bigger, -2 on error
 */
YF sSI YAC_CALL _yac_relErrCmpf (sF32, sF32, sF32);

/* @function relErrCmpd,Double a,Double b,Double err:int
   Does a relative comparison given a certain error range of Double values.
   If both values are nonzero, equal means it fullfulls fabs(a-b)/fabs(a)<=err, else it fullfills fabs(nonzero(a,b))<=err.
   @arg a first value
   @arg b second value
   @arg err relative error to tolerate for being considered as equal, if zero default is used
   @return 0 if equal, -1 if smaller or 1 if bigger, -2 on error
 */
YF sSI YAC_CALL _yac_relErrCmpd (YAC_Object *, YAC_Object *, YAC_Object *);

/* @function sinf,float a:float
Calculate single precision sin(a)
@arg a scalar value in radians
@return scalar value from -1 to 1
 */
YF sF32 YAC_CALL _yac_sinf (sF32);

/* @function cosf,float a:float
Calculate single precision cos(a)
@arg a scalar value in radians
@return scalar value from -1 to 1
 */
YF sF32 YAC_CALL _yac_cosf (sF32);

/* @function tanf,float a:float
Calculate single precision tan(a)
@arg a scalar value in radians
@return scalar value
 */
YF sF32 YAC_CALL _yac_tanf (sF32);

/* @function asinf,float a:float
Calculate single precision asin(a)
@arg a scalar value from -1 to 1
@return a scalar value from -&#x3c0;/2 to &#x3c0;/2
 */
YF sF32 YAC_CALL _yac_asinf (sF32);

/* @function acosf,float a:float
Calculate single precision acos(a)
@arg a scalar value from -1 to 1
@return scalar value from 0 to &#x3c0;
 */
YF sF32 YAC_CALL _yac_acosf (sF32);

/* @function atanf,float a:float
Calculate single precision atan(a)
@arg a scalar value
@return scalar value from -&#x3c0;/2 to &#x3c0;/2
 */
YF sF32 YAC_CALL _yac_atanf (sF32);

/* @function atan2f,float a, float b:float
Calculate single precision atan(a/b), does not throw an exception when b is zero (but when a==b==0),
returns in range -&#x3c0;&lt;result&lt;&#x3c0;
@arg a scalar value
@arg b scalar value
@return scalar value from -&#x3c0; to &#x3c0;
 */
YF sF32 YAC_CALL _yac_atan2f (sF32, sF32);

/* @function hypotf,float a,float b:float
Calculate single precision length of hypotenuse
@arg a scalar value
@arg b scalar value
@return scalar value
 */
YF sF32 YAC_CALL _yac_hypotf (sF32, sF32);

/* @function sinhf,float a:float
Calculate single precision sinh(a)
@arg a scalar value
@return scalar value
 */
YF sF32 YAC_CALL _yac_sinhf (sF32);

/* @function coshf,float a:float
Calculate single precision cosh(a)
@arg a scalar value
@return scalar value >1
 */
YF sF32 YAC_CALL _yac_coshf (sF32);

/* @function tanhf,float a:float
Calculate single precision tanh(a)
@arg a scalar value
@return scalar value from -1 to 1
 */
YF sF32 YAC_CALL _yac_tanhf (sF32);

/* @function asinhf,float a:float
Calculate single precision asinh(a)
@arg a scalar value
@return scalar value
 */
YF sF32 YAC_CALL _yac_asinhf (sF32);

/* @function acoshf,float a:float
Calculate single precision acosh(a)
@arg a scalar value with a>=1.0
@return scalar value >=0
 */
YF sF32 YAC_CALL _yac_acoshf (sF32);

/* @function atanhf,float a:float
Calculate single precision atanh(a)
@arg a scalar value from -1 to 1
@return scalar value
 */
YF sF32 YAC_CALL _yac_atanhf (sF32);

/* @function absf,float a:float
Returns single precision absolute number
@arg a scalar value
@return positive scalar value
 */
YF sF32 YAC_CALL _yac_absf   (sF32);

/* @function ceilf,float a:float
Returns next natural value to a
@arg a scalar value
@return scalar value
 */
YF sF32 YAC_CALL _yac_ceilf (sF32);

/* @function roundf,float a:float
Returns rounded value
@arg a scalar value
@return scalar value
 */
YF sF32 YAC_CALL _yac_roundf (sF32);

/* @function floorf,float a:float
Returns previous natural value to a
@arg a scalar value
@return scalar value
 */
YF sF32 YAC_CALL _yac_floorf (sF32);

/* @function modf,float a,float b:float
Returns remainder of a/b
@arg a scalar value
@arg b scalar value with b!=0
@return scalar value
 */
YF sF32 YAC_CALL _yac_modf (sF32, sF32);

/* @function sqrtf,float a:float
Returns &#x221a;a
@arg a positive scalar value
@return positive scalar value
 */
YF sF32 YAC_CALL _yac_sqrtf (sF32);

/* @function powf,float a,float b:float
Returns a<sup>b</sup>
@arg a scalar value
@arg b scakar value
@return scalar value
 */
YF sF32 YAC_CALL _yac_powf (sF32, sF32);

/* @function expf,float a:float
Returns e<sup>a</sup>
@arg a scalar value
@return positive scalar value
 */
YF sF32 YAC_CALL _yac_expf (sF32);

/* @function exp2f,float a:float
Returns 2<sup>a</sup>
@arg a scalar value
@return positive scalar value
 */
YF sF32 YAC_CALL _yac_exp2f (sF32);

/* @function exp10f,float a:float
Returns 10<sup>a</sup>
@arg a scalar value
@return positive scalar value
 */
YF sF32 YAC_CALL _yac_exp10f (sF32);

/* @function logf,float a:float
Returns log(a)
@arg a positive scalar value without zero (a>0)
@return scalar value
 */
YF sF32 YAC_CALL _yac_logf (sF32);

/* @function log2f,float a:float
Returns log<sub>2</sub>(a)
@arg a positive scalar value without zero (a>0)
@return scalar value
 */
YF sF32 YAC_CALL _yac_log2f (sF32);

/* @function log10f,float a:float
Returns log<sub>10</sub>(a)
@arg a positive scalar value without zero (a>0)
@return scalar value
 */
YF sF32 YAC_CALL _yac_log10f (sF32);

/* @function sind,Double a:Double
Calculate double precision sin(a)
@arg a scalar value in radians
@return scalar value from -1 to 1
 */
YF void YAC_CALL _yac_sind (YAC_Object *_o, YAC_Value *_r);

/* @function cosd,Double a:Double
Calculate double precision cos(a)
@arg a scalar value in radians
@return scalar value from -1 to 1
 */
YF void YAC_CALL _yac_cosd (YAC_Object *_o, YAC_Value *_r);

/* @function tand,Double a:Double
Calculate double precision tan(a)
@arg a scalar value in radians
@return scalar value
 */
YF void YAC_CALL _yac_tand (YAC_Object *_o, YAC_Value *_r);

/* @function asind,Double a:Double
Calculate double precision asin(a)
@arg a scalar value from -1 to 1
@return a scalar value from -&#x3c0;/2 to &#x3c0;/2
 */
YF void YAC_CALL _yac_asind (YAC_Object *_o, YAC_Value *_r);

/* @function acosd,Double a:Double
Calculate double precision acos(a)
@arg a scalar value from -1 to 1
@return scalar value from 0 to &#x3c0;
 */
YF void YAC_CALL _yac_acosd (YAC_Object *_o, YAC_Value *_r);

/* @function atand,Double a:Double
Calculate double precision atan(a)
@arg a scalar value
@return scalar value from -&#x3c0;/2 to &#x3c0;/2
 */
YF void YAC_CALL _yac_atand (YAC_Object *_o, YAC_Value *_r);

/* @function atan2d,Double a, Double b:Double
Calculate double precision atan(a/b), does not throw an exception when b is zero,
returns in range -&#x3c0;&lt;result&lt;&#x3c0;
@arg a scalar value
@arg b scalar value
@return scalar value from -&#x3c0; to &#x3c0;
 */
YF void YAC_CALL _yac_atan2d (YAC_Object *_o, YAC_Object *_o2, YAC_Value *_r);

/* @function hypotd,Double a,Double b:Double
Calculate double precision length of hypotenuse
@arg a scalar value
@arg b scalar value
@return scalar value
 */
YF void YAC_CALL _yac_hypotd (YAC_Object *_o, YAC_Object *_o2, YAC_Value *_r);

/* @function sinhd,Double a:Double
Calculate double precision sinh(a)
@arg a scalar value
@return scalar value
 */
YF void YAC_CALL _yac_sinhd (YAC_Object *_o, YAC_Value *_r);

/* @function coshd,Double a:Double
Calculate double precision cosh(a)
@arg a scalar value
@return scalar value >1
 */
YF void YAC_CALL _yac_coshd (YAC_Object *_o, YAC_Value *_r);

/* @function tanhd,Double a:Double
Calculate double precision tanh(a)
@arg a scalar value
@return scalar value from -1 to 1
 */
YF void YAC_CALL _yac_tanhd (YAC_Object *_o, YAC_Value *_r);

/* @function asinhd,Double a:Double
Calculate double precision asinh(a)
@arg a scalar value
@return scalar value
 */
YF void YAC_CALL _yac_asinhd (YAC_Object *_o, YAC_Value *_r);

/* @function acoshd,Double a:Double
Calculate double precision acosh(a)
@arg a scalar value with a>=1.0
@return scalar value >=0
 */
YF void YAC_CALL _yac_acoshd (YAC_Object *_o, YAC_Value *_r);

/* @function atanhd,Double a:Double
Calculate double precision atanh(a)
@arg a scalar value from -1 to 1
@return scalar value
 */
YF void YAC_CALL _yac_atanhd (YAC_Object *_o, YAC_Value *_r);

/* @function absd,Double a:Double
Returns double precision absolute number
@arg a scalar value
@return positive scalar value
 */
YF void YAC_CALL _yac_absd (YAC_Object *_o, YAC_Value *_r);

/* @function ceild,Double a:Double
Returns next natural value to a
@arg a scalar value
@return scalar value
 */
YF void YAC_CALL _yac_ceild (YAC_Object *_o, YAC_Value *_r);

/* @function roundd,Double a:Double
Returns rounded value
@arg a scalar value
@return scalar value
 */
YF void YAC_CALL _yac_roundd (YAC_Object *_o, YAC_Value *_r);

/* @function floord,Double a:Double
Returns previous natural value to a
@arg a scalar value
@return scalar value
 */
YF void YAC_CALL _yac_floord (YAC_Object *_o, YAC_Value *_r);

/* @function modd,Double a,Double b:Double
Returns remainder of a/b
@arg a scalar value
@arg b scalar value with b!=0
@return scalar value
 */
YF void YAC_CALL _yac_modd (YAC_Object *_o, YAC_Object *_o2, YAC_Value *_r);

/* @function sqrtd,Double a:Double
Returns &#x221a;a
@arg a positive scalar value
@return positive scalar value
 */
YF void YAC_CALL _yac_sqrtd (YAC_Object *_o, YAC_Value *_r);

/* @function powd,Double a,Double b:Double
Returns a<sup>b</sup>
@arg a scalar value
@arg b scakar value
@return scalar value
 */
YF void YAC_CALL _yac_powd (YAC_Object *_o, YAC_Object *_o2, YAC_Value *_r);

/* @function expd,Double a:Double
Returns e<sup>a</sup>
@arg a scalar value
@return positive scalar value
 */
YF void YAC_CALL _yac_expd (YAC_Object *_o, YAC_Value *_r);

/* @function exp2d,Double a:Double
Returns 2<sup>a</sup>
@arg a scalar value
@return positive scalar value
 */
YF void YAC_CALL _yac_exp2d (YAC_Object *_o, YAC_Value *_r);

/* @function exp10d,Double a:Double
Returns 10<sup>a</sup>
@arg a scalar value
@return positive scalar value
 */
YF void YAC_CALL _yac_exp10d (YAC_Object *_o, YAC_Value *_r);

/* @function logd,Double a:Double
Returns log(a)
@arg a positive scalar value without zero (a>0)
@return scalar value
 */
YF void YAC_CALL _yac_logd (YAC_Object *_o, YAC_Value *_r);

/* @function log2d,Double a:Double
Returns log<sub>2</sub>(a)
@arg a positive scalar value without zero (a>0)
@return scalar value
 */
YF void YAC_CALL _yac_log2d (YAC_Object *_o, YAC_Value *_r);

/* @function log10d,Double a:Double
Returns log<sub>10</sub>(a)
@arg a positive scalar value without zero (a>0)
@return scalar value
 */
YF void YAC_CALL _yac_log10d (YAC_Object *_o, YAC_Value *_r);

/* @function f16_from_f32,float v:int
Converts 32bit IEEE floating point value to 16bit float
@arg 32bit float
@return 16bit float (as signed 16bit short integer)
 */
YF sSI  YAC_CALL _yac_f16_from_f32 (sF32 _v);

/* @function f32_from_f16,int v:float
Converts 16bit float to 32bit IEEE floating point value
@arg 16bit float (as signed 16bit short integer)
@return 32bit float
 */
YF sF32 YAC_CALL _yac_f32_from_f16 (sSI _v);

/* @function fx16_from_f16,int v:int
Converts 16bit float to 32bit 16:16 fixed point value
@arg 16bit float (as signed 16bit short integer)
@return 32bit 16:16 fixed point value (integer)
 */
YF sSI  YAC_CALL _yac_fx16_from_f16 (sSI _v);


/* @endmodule tkmath
 */
#endif // COMMONMATH_H_
