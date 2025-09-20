/// complexf.h
///
/// (c) 2008 by Carsten Busse <carsten.busse@googlemail.com>
///             Bastian Spiegel <bs@tkscript.de> (additional coding)
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#ifndef __COMPLEXF_H_
#define __COMPLEXF_H_

YG("Math");

/* @function sqrtcf,Complexf a:Complexf
Returns complex single precision &#x221a;a
@arg a complex number
@return complex number
 */
YF void YAC_CALL _sqrtcf_YAC_RVAL(YAC_Object *, YAC_Value *);

/* @function powcf,Complexf a,Complexf b:Complexf
Returns complex a<sup>b</sup>
@arg a complex number
@arg b complex number
@return complex number
 */
YF void YAC_CALL _powcf_YAC_RVAL(YAC_Object *, YAC_Object *, YAC_Value *);
YF void YAC_CALL _powncf_YAC_RVAL(YAC_Object *, YAC_Object *, YAC_Object *, YAC_Value *);

/* @function expcf,Complexf a:Complexf
Returns complex e<sup>a</sup>
@arg a complex number
@return complex number
 */
YF void YAC_CALL _expcf_YAC_RVAL(YAC_Object *, YAC_Value *);

/* @function logcf,Complexf a:Complexf
Returns complex log(a)
@arg a complex number
@return complex number
 */
YF void YAC_CALL _logcf_YAC_RVAL(YAC_Object *, YAC_Value *);
YF void YAC_CALL _logncf_YAC_RVAL(YAC_Object *, YAC_Object *, YAC_Value *);

/* @function sincf,Complexf a:Complexf
Returns complex sin(a)
@arg a complex number
@return complex number
 */
YF void YAC_CALL _sincf_YAC_RVAL(YAC_Object *, YAC_Value *);

/* @function coscf,Complexf a:Complexf
Returns complex cos(a)
@arg a complex number
@return complex number
 */
YF void YAC_CALL _coscf_YAC_RVAL(YAC_Object *, YAC_Value *);

/* @function tancf,Complexf a:Complexf
Returns complex tan(a)
@arg a complex number
@return complex number
 */
YF void YAC_CALL _tancf_YAC_RVAL(YAC_Object *, YAC_Value *);

/* @function asincf,Complexf a:Complexf
Returns complex asin(a)
@arg a complex number
@return complex number
 */
YF void YAC_CALL _asincf_YAC_RVAL(YAC_Object *, YAC_Value *);

/* @function acoscf,Complexf a:Complexf
Returns complex acos(a)
@arg a complex number
@return complex number
 */
YF void YAC_CALL _acoscf_YAC_RVAL(YAC_Object *, YAC_Value *);

/* @function atancf,Complexf a:Complexf
Returns complex atan(a)
@arg a complex number
@return complex number
 */
YF void YAC_CALL _atancf_YAC_RVAL(YAC_Object *, YAC_Value *);

/* @function sinhcf,Complexf a:Complexf
Returns complex sinh(a)
@arg a complex number
@return complex number
 */
YF void YAC_CALL _sinhcf_YAC_RVAL(YAC_Object *, YAC_Value *);

/* @function coshcf,Complexf a:Complexf
Returns complex cosh(a)
@arg a complex number
@return complex number
 */
YF void YAC_CALL _coshcf_YAC_RVAL(YAC_Object *, YAC_Value *);

/* @function tanhcf,Complexf a:Complexf
Returns complex tanh(a)
@arg a complex number
@return complex number
 */
YF void YAC_CALL _tanhcf_YAC_RVAL(YAC_Object *, YAC_Value *);

/* @function asinhcf,Complexf a:Complexf
Returns complex asinh(a)
@arg a complex number
@return complex number
 */
YF void YAC_CALL _asinhcf_YAC_RVAL(YAC_Object *, YAC_Value *);

/* @function acoshcf,Complexf a:Complexf
Returns complex acosh(a)
@arg a complex number
@return complex number
 */
YF void YAC_CALL _acoshcf_YAC_RVAL(YAC_Object *, YAC_Value *);

/* @function atanhcf,Complexf a:Complexf
Returns complex atanh(a)
@arg a complex number
@return complex number
 */
YF void YAC_CALL _atanhcf_YAC_RVAL(YAC_Object *, YAC_Value *);

YF void YAC_CALL _sqrtcf_YAC_RARG(YAC_Object *, YAC_Object *);
YF void YAC_CALL _powcf_YAC_RARG(YAC_Object *, YAC_Object *, YAC_Object *);
YF void YAC_CALL _powncf_YAC_RARG(YAC_Object *, YAC_Object *, YAC_Object *, YAC_Object *);
YF void YAC_CALL _expcf_YAC_RARG(YAC_Object *, YAC_Object *);
YF void YAC_CALL _logcf_YAC_RARG(YAC_Object *, YAC_Object *);
YF void YAC_CALL _logncf_YAC_RARG(YAC_Object *, YAC_Object *, YAC_Object *);
YF void YAC_CALL _sincf_YAC_RARG(YAC_Object *, YAC_Object *);
YF void YAC_CALL _coscf_YAC_RARG(YAC_Object *, YAC_Object *);
YF void YAC_CALL _tancf_YAC_RARG(YAC_Object *, YAC_Object *);
YF void YAC_CALL _asincf_YAC_RARG(YAC_Object *, YAC_Object *);
YF void YAC_CALL _acoscf_YAC_RARG(YAC_Object *, YAC_Object *);
YF void YAC_CALL _atancf_YAC_RARG(YAC_Object *, YAC_Object *);
YF void YAC_CALL _sinhcf_YAC_RARG(YAC_Object *, YAC_Object *);
YF void YAC_CALL _coshcf_YAC_RARG(YAC_Object *, YAC_Object *);
YF void YAC_CALL _tanhcf_YAC_RARG(YAC_Object *, YAC_Object *);
YF void YAC_CALL _asinhcf_YAC_RARG(YAC_Object *, YAC_Object *);
YF void YAC_CALL _acoshcf_YAC_RARG(YAC_Object *, YAC_Object *);
YF void YAC_CALL _atanhcf_YAC_RARG(YAC_Object *, YAC_Object *);


/* @class Complexf,Object
This class represents a single precision complex number.
 */
YC class _Complexf : public YAC_Object {
  public:
   sF32 floats[2];
   
  public:
   _Complexf& operator=(_Complexf&);

   _Complexf(void);
   ~_Complexf();
   YAC_POOLED(_Complexf,YAC_POOL_PRIORITY_HIGH);
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

   /* @method setX,float x
      Set x property of x+iy
      @arg x new x value
    */
   YM void _setX(sF32);
   /* @method setY,float x
      Set y property of x+iy
      @arg new y value
    */
   YM void _setY(sF32);
   /* @method getX:float
      Get x property of x+iy
      @return x property
    */
   YM sF32 _getX(void);
   /* @method getY:float
      Get y property of x+iy
      @return y property
    */
   YM sF32 _getY(void);
//polar coordinates - use with care, this is slow!
   /* @method setR,float r
      Set r property of re<sup>ia</sup>
      @arg r new r value
    */
   YM void _setR(sF32);
   /* @method setA,float a
      Set a property of re<sup>ia</sup>
      @arg a new a value
    */
   YM void _setA(sF32);
   /* @method getR:float
      Get r property of re<sup>ia</sup>
      @return r property
    */
   YM sF32 _getR(void);
   /* @method getA:float
      Get a property of re<sup>ia</sup>
    */
   YM sF32 _getA(void);
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
   /* @method add,Complexf v
      add complex number v to this instance
      @arg v complex value to add
    */
   YM void _add_YAC_RSELF(YAC_Object *);
   YM void _add_YAC_RVAL(YAC_Object *, YAC_Value *);
   YM void _add_YAC_RARG(YAC_Object *, YAC_Object *);

   /* @method sub,Complexf v
      substract complex number v from this instance
      @arg v complex value to substract
    */
   YM void _sub_YAC_RSELF(YAC_Object *);
   YM void _sub_YAC_RVAL(YAC_Object *, YAC_Value *);
   YM void _sub_YAC_RARG(YAC_Object *, YAC_Object *);

//multiply with scalar
   /* @method mulf,float v
      Multiply this instance with scalar v
      @arg v scalar value to multiply
    */
   YM void _mulf_YAC_RSELF(sF32);
   YM void _mulf_YAC_RVAL(sF32, YAC_Value *);
   YM void _mulf_YAC_RARG(sF32, YAC_Object *);

//complex multiply
   /* @method mul,Complexf v
      Multiply this instance with complex v
      @arg v complex value to multiply
    */
   YM void _mul_YAC_RSELF(YAC_Object *);
   YM void _mul_YAC_RVAL(YAC_Object *, YAC_Value *);
   YM void _mul_YAC_RARG(YAC_Object *, YAC_Object *);

//multiply with conjugated complex (a+ib)*(c+id)=ac+bd
   /* @method mulConj,Complexf v:float
      Multiply this instance with complex conjugated v, ((x<sub>1</sub>+iy<sub>1</sub>)*(x<sub>2</sub>-iy<sub>2</sub>))
      @arg v complex value to conjugate and multiply
      @return result of complex conjugated multiplication (scalar value)
    */
   YM sF32 _mulConj(YAC_Object *);
//complex division
   /* @method div,Complexf v
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
   /* @method getAbs:float
      Returns length of complex number
      @return length
    */
   YM sF32 _getAbs(void);
   /* @method getAbsSqr:float
      Returns squared length of complex number (faster than abs)
      @return length<sup>2</sup>
    */
   YM sF32 _getAbsSqr(void);
//scale vector to length 1 if possible
   /* @method unit
      Set length of instance to 1
    */
   YM void _unit_YAC_RSELF(void); /* unit */
   YM void _unit_YAC_RVAL(YAC_Value *);
   YM void _unit_YAC_RARG(YAC_Object *);

//set Vector length to Scale
   /* @method unitScale,float s
      Set length of instance to s
      @arg s new length for complex number
    */
   YM void _unitScale_YAC_RSELF(sF32);
   YM void _unitScale_YAC_RVAL(sF32, YAC_Value *);
   YM void _unitScale_YAC_RARG(sF32, YAC_Object *);

//init element values
   /* @method init,float a,float b
      Set value of instance to x=a and y=b (x+iy)
      @arg a new x value
      @arg b new y value
    */
   YM void _init(sF32,sF32);
   /* @method initPolar,float _a,float _b
      Set value of instance to r=_a and a=_b (re<sup>ia</sup>)
      @arg _a new r value
      @arg _b new a value
    */
   YM void _initPolar(sF32,sF32);
   /* @method New,float a,float b:Complexf
      Returns new instance with values x=a and y=b of (x+iy)
      @arg a new x value
      @arg b new y value
      @return new instance of Complexf (x+iy)
    */
   YM void _New(sF32,sF32,YAC_Value*);
   /* @method NewPolar,float va,float vb:Complexf
      Returns new instance with values r=va and a=vb of (re<sup>ia</sup>)
      @arg va new r value
      @arg vb new a value
      @return new instance of Complexf (re<sup>ia</sup>)
    */
   YM void _NewPolar(sF32,sF32,YAC_Value*);
};

/* @function complexf,float a,float b:Complexf
   Returns new instance with values x=a and y=b of (x+iy)
   @arg a new x value
   @arg b new y value
   @return new instance of Complexf (x+iy)
 */
YF void YAC_CALL complexf(sF32,sF32,YAC_Value*);
/* @function complexfPolar,float va,float vb:Complexf
   Returns new instance with values r=va and a=vb of (re<sup>ia</sup>)
   @arg va new r value
   @arg vb new a value
   @return new instance of Complexf (re<sup>ia</sup>)
 */
YF void YAC_CALL complexfPolar(sF32,sF32,YAC_Value*);

#endif
