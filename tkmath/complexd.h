/// complexd.h
///
/// (c) 2008 by Carsten Busse <carsten.busse@googlemail.com>
///             Bastian Spiegel <bs@tkscript.de> (additional coding)
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#ifndef __COMPLEXD_H_
#define __COMPLEXD_H_

YG("Math");

/* @function sqrtcd,Complexd a:Complexd
Returns complex double precision &#x221a;a
@arg a complex number
@return complex number
 */
YF void YAC_CALL _sqrtcd_YAC_RVAL(YAC_Object *, YAC_Value *);

/* @function powcd,Complexd a,Complexd b:Complexd
Returns complex a<sup>b</sup>
@arg a complex number
@arg b complex number
@return complex number
 */
YF void YAC_CALL _powcd_YAC_RVAL(YAC_Object *, YAC_Object *, YAC_Value *);
YF void YAC_CALL _powncd_YAC_RVAL(YAC_Object *, YAC_Object *, YAC_Object *, YAC_Value *);

/* @function expcd,Complexd a:Complexd
Returns complex e<sup>a</sup>
@arg a complex number
@return complex number
 */
YF void YAC_CALL _expcd_YAC_RVAL(YAC_Object *, YAC_Value *);

/* @function logcd,Complexd a:Complexd
Returns complex log(a)
@arg a complex number
@return complex number
 */
YF void YAC_CALL _logcd_YAC_RVAL(YAC_Object *, YAC_Value *);
YF void YAC_CALL _logncd_YAC_RVAL(YAC_Object *, YAC_Object *, YAC_Value *);

/* @function sincd,Complexd a:Complexd
Returns complex sin(a)
@arg a complex number
@return complex number
 */
YF void YAC_CALL _sincd_YAC_RVAL(YAC_Object *, YAC_Value *);

/* @function coscd,Complexd a:Complexd
Returns complex cos(a)
@arg a complex number
@return complex number
 */
YF void YAC_CALL _coscd_YAC_RVAL(YAC_Object *, YAC_Value *);

/* @function tancf,Complexd a:Complexd
Returns complex tan(a)
@arg a complex number
@return complex number
 */
YF void YAC_CALL _tancd_YAC_RVAL(YAC_Object *, YAC_Value *);

/* @function asincd,Complexd a:Complexd
Returns complex asin(a)
@arg a complex number
@return complex number
 */
YF void YAC_CALL _asincd_YAC_RVAL(YAC_Object *, YAC_Value *);

/* @function acoscd,Complexd a:Complexd
Returns complex acos(a)
@arg a complex number
@return complex number
 */
YF void YAC_CALL _acoscd_YAC_RVAL(YAC_Object *, YAC_Value *);

/* @function atancd,Complexd a:Complexd
Returns complex atan(a)
@arg a complex number
@return complex number
 */
YF void YAC_CALL _atancd_YAC_RVAL(YAC_Object *, YAC_Value *);

/* @function sinhcd,Complexd a:Complexd
Returns complex sinh(a)
@arg a complex number
@return complex number
 */
YF void YAC_CALL _sinhcd_YAC_RVAL(YAC_Object *, YAC_Value *);

/* @function coshcd,Complexd a:Complexd
Returns complex cosh(a)
@arg a complex number
@return complex number
 */
YF void YAC_CALL _coshcd_YAC_RVAL(YAC_Object *, YAC_Value *);

/* @function tanhcd,Complexd a:Complexd
Returns complex tanh(a)
@arg a complex number
@return complex number
 */
YF void YAC_CALL _tanhcd_YAC_RVAL(YAC_Object *, YAC_Value *);

/* @function asinhcd,Complexd a:Complexd
Returns complex asinh(a)
@arg a complex number
@return complex number
 */
YF void YAC_CALL _asinhcd_YAC_RVAL(YAC_Object *, YAC_Value *);

/* @function acoshcd,Complexd a:Complexd
Returns complex acosh(a)
@arg a complex number
@return complex number
 */
YF void YAC_CALL _acoshcd_YAC_RVAL(YAC_Object *, YAC_Value *);

/* @function atanhcd,Complexd a:Complexd
Returns complex atanh(a)
@arg a complex number
@return complex number
 */
YF void YAC_CALL _atanhcd_YAC_RVAL(YAC_Object *, YAC_Value *);

YF void YAC_CALL _sqrtcd_YAC_RARG(YAC_Object *, YAC_Object *);
YF void YAC_CALL _powcd_YAC_RARG(YAC_Object *, YAC_Object *, YAC_Object *);
YF void YAC_CALL _powncd_YAC_RARG(YAC_Object *, YAC_Object *, YAC_Object *, YAC_Object *);
YF void YAC_CALL _expcd_YAC_RARG(YAC_Object *, YAC_Object *);
YF void YAC_CALL _logcd_YAC_RARG(YAC_Object *, YAC_Object *);
YF void YAC_CALL _logncd_YAC_RARG(YAC_Object *, YAC_Object *, YAC_Object *);
YF void YAC_CALL _sincd_YAC_RARG(YAC_Object *, YAC_Object *);
YF void YAC_CALL _coscd_YAC_RARG(YAC_Object *, YAC_Object *);
YF void YAC_CALL _tancd_YAC_RARG(YAC_Object *, YAC_Object *);
YF void YAC_CALL _asincd_YAC_RARG(YAC_Object *, YAC_Object *);
YF void YAC_CALL _acoscd_YAC_RARG(YAC_Object *, YAC_Object *);
YF void YAC_CALL _atancd_YAC_RARG(YAC_Object *, YAC_Object *);
YF void YAC_CALL _sinhcd_YAC_RARG(YAC_Object *, YAC_Object *);
YF void YAC_CALL _coshcd_YAC_RARG(YAC_Object *, YAC_Object *);
YF void YAC_CALL _tanhcd_YAC_RARG(YAC_Object *, YAC_Object *);
YF void YAC_CALL _asinhcd_YAC_RARG(YAC_Object *, YAC_Object *);
YF void YAC_CALL _acoshcd_YAC_RARG(YAC_Object *, YAC_Object *);
YF void YAC_CALL _atanhcd_YAC_RARG(YAC_Object *, YAC_Object *);


/* @class Complexd,Object
This class represents a double precision complex number.
 */
YC class _Complexd : public YAC_Object {
  public:
   sF64 doubles[2];
   
  public:
   _Complexd& operator=(_Complexd&);

   _Complexd(void);
   ~_Complexd();
   YAC_POOLED(_Complexd,YAC_POOL_PRIORITY_HIGH);
   sUI   YAC_VCALL yacArrayGetNumElements     (void);
   sUI   YAC_VCALL yacArrayGetMaxElements     (void);
   sUI   YAC_VCALL yacArrayGetElementType     (void);
   sUI   YAC_VCALL yacArrayGetElementByteSize (void);
   void  YAC_VCALL yacArraySet                (YAC_ContextHandle, sUI, YAC_Value *);
   void  YAC_VCALL yacArrayGet                (YAC_ContextHandle, sUI, YAC_Value *);
   void *YAC_VCALL yacArrayGetPointer         (void);
   sSI   YAC_VCALL yacTensorRank              (void);
   void  YAC_VCALL yacSerialize               (YAC_Object *, sUI);
   sUI   YAC_VCALL yacDeserialize             (YAC_Object *, sUI);
   void  YAC_VCALL yacOperatorClamp           (YAC_Object *, YAC_Object *);
   void  YAC_VCALL yacOperatorWrap            (YAC_Object *, YAC_Object *);
//supports:
//YAC_OP_CEQ, YAC_OP_CNE, YAC_OP_ASSIGN, YAC_OP_ADD = _add,
//YAC_OP_SUB = _sub, YAC_OP_MUL = _mul, YAC_OP_DIV = _div, YAC_OP_NEG, YAC_OP_INIT
   void  YAC_VCALL yacOperator                (sSI, YAC_Object *, YAC_Value *);
   void  YAC_VCALL yacOperatorI               (sSI, sSI, YAC_Value*);
   void  YAC_VCALL yacOperatorF32             (sSI, sF32, YAC_Value*);
   void  YAC_VCALL yacOperatorF64             (sSI, sF64, YAC_Value*);
   sBool YAC_VCALL yacToString                (YAC_String*) const;

   void  YAC_VCALL yacValueOfI                (sSI);
   void  YAC_VCALL yacValueOfI64              (sS64);
   void  YAC_VCALL yacValueOfF32              (sF32);
   void  YAC_VCALL yacValueOfF64              (sF64);

   sBool YAC_VCALL yacScanI                   (sSI *);
   sBool YAC_VCALL yacScanI64                 (sS64 *);
   sBool YAC_VCALL yacScanF32                 (sF32 *);
   sBool YAC_VCALL yacScanF64                 (sF64 *);

   // Helper methods
   void assignFloats(sF32 *_fv, sUI _ne);
   void assignFloatArray(YAC_FloatArray *_fa);
   void assignGeneric(YAC_ContextHandle _ctx, YAC_Object *_o);

//Element access methods
//cartesian coordinates - direct access

   /* @method setX,Double x
      Set x property of x+iy
      @arg x new x value
    */
   YM void _setX(YAC_Object *);
   /* @method setY,Double x
      Set y property of x+iy
      @arg new y value
    */
   YM void _setY(YAC_Object *);
   /* @method getX:Double
      Get x property of x+iy
      @return x property
    */
   YM void _getX_YAC_RVAL(YAC_Value *);
   YM void _getX_YAC_RARG(YAC_Object *);
   /* @method getY:Double
      Get y property of x+iy
      @return y property
    */
   YM void _getY_YAC_RVAL(YAC_Value *);
   YM void _getY_YAC_RARG(YAC_Object *);
//polar coordinates - use with care, this is slow!
   /* @method setR,Double r
      Set r property of re<sup>ia</sup>
      @arg r new r value
    */
   YM void _setR(YAC_Object *);
   /* @method setA,Double a
      Set a property of re<sup>ia</sup>
      @arg a new a value
    */
   YM void _setA(YAC_Object *);
   /* @method getR:Double
      Get r property of re<sup>ia</sup>
      @return r property
    */
   sF64 getR(void);
   YM void _getR_YAC_RVAL(YAC_Value *);
   YM void _getR_YAC_RARG(YAC_Object *);
   /* @method getA:Double
      Get a property of re<sup>ia</sup>
    */
   sF64 getA(void);
   YM void _getA_YAC_RVAL(YAC_Value *);
   YM void _getA_YAC_RARG(YAC_Object *);
//produce string representation
//cartesian x+i*y
   /* @method getString:String
      Get string representation of complex number (x+iy)
      @return string representation (x+iy)
    */
   YM void _getString(YAC_Value *) const;
//polar r*exp(i*a);
   /* @method getPolarString:String
      Get string representation of complex number (re<sup>ia</sup>)
    */
   YM void _getPolarString(YAC_Value *);
//arithmetic
   /* @method add,Complexd v
      add complex number v to this instance
      @arg v complex value to add
    */
   YM void _add_YAC_RSELF(YAC_Object *);
   YM void _add_YAC_RVAL(YAC_Object *, YAC_Value *);
   YM void _add_YAC_RARG(YAC_Object *, YAC_Object *);

   /* @method sub,Complexd v
      substract complex number v from this instance
      @arg v complex value to substract
    */
   YM void _sub_YAC_RSELF(YAC_Object *);
   YM void _sub_YAC_RVAL(YAC_Object *, YAC_Value *);
   YM void _sub_YAC_RARG(YAC_Object *, YAC_Object *);

//multiply with scalar
   /* @method muld,Double v
      Multiply this instance with scalar v
      @arg v scalar value to multiply
    */
   void muld(sF64);
   YM void _muld_YAC_RSELF(YAC_Object *);
   YM void _muld_YAC_RVAL(YAC_Object *, YAC_Value *);
   YM void _muld_YAC_RARG(YAC_Object *, YAC_Object *);

//complex multiply
   /* @method mul,Complexd v
      Multiply this instance with complex v
      @arg v complex value to multiply
    */
   YM void _mul_YAC_RSELF(YAC_Object *);
   YM void _mul_YAC_RVAL(YAC_Object *, YAC_Value *);
   YM void _mul_YAC_RARG(YAC_Object *, YAC_Object *);

//multiply with conjugated complex (a+ib)*(c+id)=ac+bd
   /* @method mulConj,Complexd v:float
      Multiply this instance with complex conjugated v, ((x<sub>1</sub>+iy<sub>1</sub>)*(x<sub>2</sub>-iy<sub>2</sub>))
      @arg v complex value to conjugate and multiply
      @return result of complex conjugated multiplication (scalar value)
    */
   YM void _mulConj_YAC_RVAL(YAC_Object *, YAC_Value *);
   YM void _mulConj_YAC_RARG(YAC_Object *, YAC_Object *);
//complex division
   /* @method div,Complexd v
      Divide this instance by complex v
      @arg v complex value to divide through
    */
   YM void _div_YAC_RSELF(YAC_Object *);
   YM void _div_YAC_RVAL(YAC_Object *, YAC_Value *);
   YM void _div_YAC_RARG(YAC_Object *, YAC_Object *);

//invert (1/z)=z*/z^2=(x-i*y)/(x^2+y^2)
   /* @method invert
      Invert this instance (v=1/v)
    */
   YM void _invert_YAC_RSELF(void);
   YM void _invert_YAC_RVAL(YAC_Value *);
   YM void _invert_YAC_RARG(YAC_Object *);

//return (squared) length of vector
   /* @method getAbs:Double
      Returns length of complex number
      @return length
    */
   sF64 getAbs(void);
   YM void _getAbs_YAC_RVAL(YAC_Value *);
   YM void _getAbs_YAC_RARG(YAC_Object *);
   /* @method getAbsSqr:Double
      Returns squared length of complex number (faster than abs)
      @return length<sup>2</sup>
    */
   sF64 getAbsSqr(void);
   YM void _getAbsSqr_YAC_RVAL(YAC_Value *);
   YM void _getAbsSqr_YAC_RARG(YAC_Object *);
//scale vector to length 1 if possible
   /* @method unit
      Set length of instance to 1
    */
   YM void _unit_YAC_RSELF(void); /* unit */
   YM void _unit_YAC_RVAL(YAC_Value *);
   YM void _unit_YAC_RARG(YAC_Object *);

//set Vector length to Scale
   /* @method unitScale,Double s
      Set length of instance to s
      @arg s new length for complex number
    */
   YM void _unitScale_YAC_RSELF(YAC_Object *);
   YM void _unitScale_YAC_RVAL(YAC_Object *, YAC_Value *);
   YM void _unitScale_YAC_RARG(YAC_Object *, YAC_Object *);

//init element values
   //internal
   void initd(sF64,sF64);
   /* @method init,Double a,Double b
      Set value of instance to x=a and y=b (x+iy)
      @arg a new x value
      @arg b new y value
    */
   YM void _init(YAC_Object *,YAC_Object *);
   /* @method initf,float a,float b
      Set value of instance to single precision x=a and y=b (x+iy)
      @arg a new x value
      @arg b new y value
    */
   YM void _initf(sF32, sF32);
   /* @method initPolar,Double _a,Double _b
      Set value of instance to r=_a and a=_b (re<sup>ia</sup>)
      @arg _a new r value
      @arg _b new a value
    */
   YM void _initPolar(YAC_Object *,YAC_Object *);
   /* @method initPolarf,sF32 _a,sF32 _b
      Set value of instance to r=_a and a=_b (re<sup>ia</sup>)
      @arg _a new single precision r value
      @arg _b new single precision a value
    */
   YM void _initPolarf(sF32, sF32);
   /* @method New,Double a,Double b:Complexd
      Returns new instance with values x=a and y=b of (x+iy)
      @arg a new x value
      @arg b new y value
      @return new instance of Complexd (x+iy)
    */
   YM void _New(YAC_Object *,YAC_Object *, YAC_Value*);
   /* @method NewPolar,Double va,Double vb:Complexd
      Returns new instance with values r=va anda a=vb of (re<sup>ia</sup>)
      @arg va new r value
      @arg vb new a value
      @return new instance of Complexd (re<sup>ia</sup>)
    */
   YM void _NewPolar(YAC_Object *,YAC_Object *,YAC_Value*);
};

/* @function complexd,Double a,Double b:Complexd
   Returns new instance with values x=a and y=b of (x+iy)
   @arg a new x value
   @arg b new y value
   @return new instance of Complexd (x+iy)
 */
YF void YAC_CALL complexd(YAC_Object *,YAC_Object *,YAC_Value*);
/* @function complexfPolar,Double va,Double vb:Complexd
   Returns new instance with values r=va and a=vb of (re<sup>ia</sup>)
   @arg va new r value
   @arg vb new a value
   @return new instance of Complexd (re<sup>ia</sup>)
 */
YF void YAC_CALL complexdPolar(YAC_Object *, YAC_Object *, YAC_Value*);

#endif
