/// quaternionf.h
///
/// (c) 2008 by Carsten Busse <carsten.busse@googlemail.com>
///             Bastian Spiegel <bs@tkscript.de> (additional coding)
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#ifndef __QUATERNIONF_H_
#define __QUATERNIONF_H_

YG("Math");

/* @class Quaternionf,Object
This class represents a single precision quaternion of the form a+ib+jc+kd.
 */
YC class _Quaternionf : public YAC_Object {
  public:
   sF32 floats[4];
  public:
   _Quaternionf(void);
   ~_Quaternionf();
   YAC_POOLED(_Quaternionf,YAC_POOL_PRIORITY_HIGH);
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
//YAC_OP_SUB = _sub, YAC_OP_MUL = _mul, YAC_OP_NEG, YAC_OP_INIT
   void  YAC_VCALL yacOperator                (sSI, YAC_Object *, YAC_Value *);
   void  YAC_VCALL yacOperatorI               (sSI, sSI, YAC_Value *);
   void  YAC_VCALL yacOperatorF32             (sSI, sF32, YAC_Value *);
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

   /* @method setR,float a
      Set real part to a
      @arg a new value for real value
    */
   YM void _setR(sF32);
   /* @method setI,float a
      Set imaginery i part to a
      @arg a new value for imaginery i value
    */
   YM void _setI(sF32);
   /* @method setJ,float a
      Set imaginery j part to a
      @arg a new value for imaginery j value
    */
   YM void _setJ(sF32);
   /* @method setK,float a
      Set imagnery k part to a
      @arg a new value for imaginery k value
    */
   YM void _setK(sF32);
   /* @method setV,Vector3f v
      Set vector part to v
      @arg v new value for vector part
    */
   YM void _setV(YAC_Object *);

   /* @method getR:float
      Get Real value
      @return real value
    */
   YM sF32 _getR(void);
   /* @method getI:float
      Get imaginery i value
      @return imaginery i value
    */
   YM sF32 _getI(void);
   /* @method getJ:float
      Get imaginery j value
      @return imaginery j value
    */
   YM sF32 _getJ(void);
   /* @method getK:float
      Get imaginery k value
      qreturn imaginery k value
    */
   YM sF32 _getK(void);
   /* @method getV:Vector3f
      Get vector value
      @return vector part
    */
   YM void _getV_YAC_RVAL(YAC_Value *);
   YM void _getV_YAC_RARG(YAC_Object *);
   /* @method getString:String
      Get string representation of quaternion number (a+ib+jc+kd)
      @return string representation (a+ib+jc+kd)
    */
   YM void _getString(YAC_Value *) const;
   /* @method add,Quaternionf v
      add quaternion v to this instance
      @arg v quaternion value to add
    */
   YM void _add_YAC_RSELF(YAC_Object *);
   YM void _add_YAC_RVAL(YAC_Object *, YAC_Value *);
   YM void _add_YAC_RARG(YAC_Object *, YAC_Object *);
   /* @method sub,Quaternionf v
      substract quaternion v from this instance
      @arg v quaternion value to substract
    */
   YM void _sub_YAC_RSELF(YAC_Object *);
   YM void _sub_YAC_RVAL(YAC_Object *, YAC_Value *);
   YM void _sub_YAC_RARG(YAC_Object *, YAC_Object *);
   /* @method mulf,float v
      Multiply this instance with scalar v
      @arg v scalar value to multiply
    */
   YM void _mulf_YAC_RSELF(sF32);
   YM void _mulf_YAC_RVAL(sF32, YAC_Value *);
   YM void _mulf_YAC_RARG(sF32, YAC_Object *);
   /* @method mul,Quaternionf v
      Multiply this instance with quaternion v (Grassmann product)
      @arg v quaternion value to multiply
    */
   YM void _mul_YAC_RSELF(YAC_Object *);
   YM void _mul_YAC_RVAL(YAC_Object *, YAC_Value *);
   YM void _mul_YAC_RARG(YAC_Object *, YAC_Object *);
   /* @method mulConj,Quaternionf v
      Multiply this instance with conjugated quaternion v (Grassmann product)
      @arg v conjugated quaternion value to multiply
    */
   YM void _mulConj_YAC_RVAL(YAC_Object *, YAC_Value *);
   YM void _mulConj_YAC_RARG(YAC_Object *, YAC_Object *);
   /* @method conjMul,Quaternionf v
      Multiply this conjugated instance with quaternion v (Grassmann product)
      @arg v quaternion value to multiply
    */
   YM void _conjMul_YAC_RVAL(YAC_Object *, YAC_Value *);
   YM void _conjMul_YAC_RARG(YAC_Object *, YAC_Object *);
   /* @method dot,Quaternionf v:float
      Calculate dot product (inner product) of this instance and quaternion v
      @arg v quaternion value to multiply
      @return dot product of quaternions
    */
   YM sF32 _dot(YAC_Object *);
   /* @method outer,Quaternionf v:Quaternionf
      Calculate the outer product of this instance and quaternion v
      @arg v quaternion value to multiply
      @return outer product of quaternions
    */
   YM void _outer_YAC_RVAL(YAC_Object *, YAC_Value *);
   YM void _outer_YAC_RARG(YAC_Object *, YAC_Object *);
   /* @method invert
      Calculate the reciprocal value 1/q
      @return inverse quaternion
    */
   YM void _invert_YAC_RSELF(void);
   YM void _invert_YAC_RVAL(YAC_Value *);
   YM void _invert_YAC_RARG(YAC_Object *);
   /* @method getAbs:float
      Return the absolute value of this instance
      @return length of quaternion
    */
   YM sF32 _getAbs(void);
   /* @method getAbsSqr:float
      Return the squared length of this instance
      @return length<sup>2</sup> of this instance
    */
   YM sF32 _getAbsSqr(void);
   /* @method unit
      Set length of quaternion to 1
    */
   YM void _unit_YAC_RSELF(void);
   YM void _unit_YAC_RVAL(YAC_Value *);
   YM void _unit_YAC_RARG(YAC_Object *);
   /* @method unitScale,float s
      Set length of quaternion to s
      @arg s new length
    */
   YM void _unitScale_YAC_RSELF(sF32);
   YM void _unitScale_YAC_RVAL(sF32, YAC_Value *);
   YM void _unitScale_YAC_RARG(sF32, YAC_Object *);
   /* @method setAngle,float a
      Set new quaternion angle
      @arg a new angle
    */
   YM void _setAngle(sF32);
   /* @method setRadius,float a
      Set new quaternion radius
      @arg a new length
    */
   YM void _setRadius(sF32);
   /* @method setPureUnit,Quaternionf v
      Set new pure unit vector of quaternion
      @arg v pure unit quaternion with real part zero and length 1
    */
   YM void _setPureUnit(YAC_Object *);
   /* @method getAngle:float
      Calculate quaternion angle
      @return quaternion angle
    */
   YM sF32 _getAngle(void);
   /* @method getRadius:float
      Calculate quaternion radius (_getAbs)
      @return length of quaternion
    */
   YM sF32 _getRadius(void);
   /* @method getPureUnit:Quaternionf
      Returns pure unit quaternion from instance
      @return pure unit quaternion
    */
   YM void _getPureUnit_YAC_RVAL(YAC_Value*);
   YM void _getPureUnit_YAC_RARG(YAC_Object*);
   /* @method getMatrix4f:Matrix4f
      Get 4x4 matrix representation of quaternion
      @return matrix representation of quaternion
    */
   YM void _getMatrix4f_YAC_RVAL(YAC_Value *);
   YM void _getMatrix4f_YAC_RARG(YAC_Object *);
   /* @method getRotateMatrix4f:Matrix4f
      Get 4x4 rotation matrix representation of quaternion q*conj q
      @return rotation matrix representation of quaternion
    */
   YM void _getRotateMatrix4f_YAC_RVAL(YAC_Value *);
   YM void _getRotateMatrix4f_YAC_RARG(YAC_Object *);
   /* @method init,float a,float b,float c,float d
      Init quaternion to a+ib+jc+kd
      @arg a new real value
      @arg b new i value
      @arg c new j value
      @arg d new k value
    */
   YM void _init(sF32, sF32, sF32, sF32);
   /* @method New,float a,float b,float c,float d:Quaternionf
      Init quaternion to a+ib+jc+kd and return new instance
      @arg a new real value
      @arg b new i value
      @arg c new j value
      @arg d new k value 
    */
   YM void _New(sF32, sF32, sF32, sF32, YAC_Value *);
};

/* @function quaternionf,float a,float b,float c,float d:Quaternionf
   Init quaternion to a+ib+jc+kd and return new instance
   @arg a new real value
   @arg b new i value
   @arg c new j value
   @arg d new k value 
 */
YF void YAC_CALL _quaternionf(sF32, sF32, sF32, sF32, YAC_Value *);
#endif
