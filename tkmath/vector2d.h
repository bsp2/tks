/// vector2d.h
///
/// (c) 2008-2024 by Carsten Busse <carsten.busse@googlemail.com>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#ifndef VECTOR2D_H_
#define VECTOR2D_H_


YG("Math");

/* @class Vector2d,Object
This class represents a double precision 2 dimensional coordinate vector.
 */
YC class _Vector2d : public YAC_Object {
  public:
   sF64 doubles[2];

  public:
   _Vector2d(void);
   ~_Vector2d();

   YAC_POOLED(_Vector2d,YAC_POOL_PRIORITY_HIGH);

   sUI   YAC_VCALL yacArrayGetNumElements    (void);
   sUI   YAC_VCALL yacArrayGetMaxElements    (void);
   sUI   YAC_VCALL yacArrayGetElementType    (void);
   sUI   YAC_VCALL yacArrayGetElementByteSize(void);
   void  YAC_VCALL yacArraySet               (YAC_ContextHandle, sUI, YAC_Value *);
   void  YAC_VCALL yacArrayGet               (YAC_ContextHandle, sUI, YAC_Value *);
   void *YAC_VCALL yacArrayGetPointer        (void);
   sSI   YAC_VCALL yacTensorRank             (void);
   void  YAC_VCALL yacSerialize              (YAC_Object *, sUI);
   sUI   YAC_VCALL yacDeserialize            (YAC_Object *, sUI);
   void  YAC_VCALL yacOperatorClamp          (YAC_Object *, YAC_Object *);
   void  YAC_VCALL yacOperatorWrap           (YAC_Object *, YAC_Object *);
   //supports:
   //YAC_OP_CEQ, YAC_OP_CNE, YAC_OP_ASSIGN, YAC_OP_ADD = _add,
   //YAC_OP_SUB = _sub, YAC_OP_MUL = _mul, YAC_OP_NEG, YAC_OP_INIT
   void  YAC_VCALL yacOperator               (sSI, YAC_Object *, YAC_Value *);
   void  YAC_VCALL yacOperatorI              (sSI, sSI, YAC_Value *);
   void  YAC_VCALL yacOperatorF32            (sSI, sF32, YAC_Value *);
   void  YAC_VCALL yacOperatorF64            (sSI, sF64, YAC_Value *);
   sUI   YAC_VCALL yacOperatorPriority       (void) { return YAC_OP_PRIO_DOUBLE; }
   sBool YAC_VCALL yacToString               (YAC_String *) const;

  public:  
   void assignFloats (const sF32 *_fv, sUI _ne);
   void assignFloatArray (YAC_FloatArray *_fa);
   void assignGeneric (YAC_ContextHandle _ctx, YAC_Object *_o);

  public: 

   /* @method setX,Double a
      Set x element to a
      @arg a new value for x
    */
   YM void _setX (YAC_Object*);

   /* @method setY,Double a
      Set y element to a
      @arg a new value for y
    */
   YM void _setY (YAC_Object*);

   /* @method getX:Double
      Get value of x element
      @return value of x element
    */
   YM void _getX_YAC_RVAL (YAC_Value*);
   YM void _getX_YAC_RARG (YAC_Object*);

   /* @method getY:Double
      Get value of y element
      @return value of y element
    */
   YM void _getY_YAC_RVAL (YAC_Value*);
   YM void _getY_YAC_RARG (YAC_Object*);

   /* @method getString:String
      Get string representation (x, y) of vector
      @return string representation of vector
    */
   YM void _getString (YAC_Value *) const;

   /* @method add,Vector2d v
      Add another vector v to this instance
      @arg v Vector to add
    */
   YM void _add_YAC_RSELF (YAC_Object *);
   YM void _add_YAC_RVAL (YAC_Object *, YAC_Value *);
   YM void _add_YAC_RARG (YAC_Object *, YAC_Object *);

   /* @method sub,Vector2d v
      Substract another vector v from this instance
      @arg v Vector to substract
    */
   YM void _sub_YAC_RSELF (YAC_Object *);
   YM void _sub_YAC_RVAL (YAC_Object *, YAC_Value *);
   YM void _sub_YAC_RARG (YAC_Object *, YAC_Object *);

   /* @method dot,Vector2d v:Double
      Calculate dot product from this instance and v
      @arg v Vector2d instance
      @return Scalar value
    */
   sF64 dot(YAC_Object *);
   YM void _dot_YAC_RVAL(YAC_Object *, YAC_Value *);
   YM void _dot_YAC_RARG(YAC_Object *, YAC_Object *);

   /* @method tensor,Vector2d v:Matrix2d
      Create resulting matrix of tensor product from this instance and v
      @arg v Vector2d instance
      @return Matrix2d instance
    */
   YM void _tensor_YAC_RVAL (YAC_Object *, YAC_Value *);
   YM void _tensor_YAC_RARG (YAC_Object *_v2f, YAC_Object *_retM2f);

   /* @method muld,Double v
      Multiply this instance by scalar value v
      @arg v scalar value
    */
   void muld(sF64);
   YM void _muld_YAC_RSELF (YAC_Object *);
   YM void _muld_YAC_RVAL (YAC_Object *, YAC_Value *);
   YM void _muld_YAC_RARG (YAC_Object *, YAC_Object *);

   /* @method mul,Vector2d v
      Multiply the x element of this instance by the x element of v and do the same with the y element
      @arg v Vector2d instance
    */
   YM void _mul_YAC_RSELF (YAC_Object *);
   YM void _mul_YAC_RVAL (YAC_Object *, YAC_Value *);
   YM void _mul_YAC_RARG (YAC_Object *, YAC_Object *);

   /* @method getAbs:Double
      Return length of vector
      @return length of vector
    */
   sF64 getAbs (void);
   YM void _getAbs_YAC_RVAL (YAC_Value *);
   YM void _getAbs_YAC_RARG (YAC_Object *);

   /* @method getAbsSqr:Double
      Return squared length of vector
      @return length<sup>2</sup> of vector
    */
   sF64 getAbsSqr (void);
   YM void _getAbsSqr_YAC_RVAL (YAC_Value *);
   YM void _getAbsSqr_YAC_RARG (YAC_Object *);

   /* @method unit
      Set length to 1
    */
   YM void _unit_YAC_RSELF (void); /* unit */
   YM void _unit_YAC_RVAL (YAC_Value *);
   YM void _unit_YAC_RARG (YAC_Object *);

   /* @method unitScale,Double s
      Set length to s
      @arg s New length of vector
    */
   void unitScale (sF64);
   YM void _unitScale_YAC_RSELF (YAC_Object *);
   YM void _unitScale_YAC_RVAL (YAC_Object *, YAC_Value *);
   YM void _unitScale_YAC_RARG (YAC_Object *, YAC_Object *);

   /* @method init,Double a,Double b
      Initialize values to x=a and y=b
      @arg a new x value
      @arg b new y value
    */
   YM void _init (YAC_Object *, YAC_Object *);

   /* @method initf,float a,float b
      Initialize with single precision values to x=a and x=b
      @arg a new x value
      @arg b new y value
    */
   YM void _initf(sF32, sF32);

   /* @method initScaled,Vector2d v,Double s
      Initialize values to s*v
      @arg v Vector2d instance
      @arg s scale to multiply
    */
   YM void _initScaled (YAC_Object *, YAC_Object *);

   /* @method New,Double a,Double b:Vector2d
      Initialize and return new Vector2d instance with x=a and y=b
      @arg a new x value
      @arg b new y value
      @return new Vector2d instance
    */
   YM void _New (YAC_Object *,YAC_Object *,YAC_Value*);
};

/* @function vector2d,Double a,Double b:Vector2d
   Initialize and return new Vector2d instance with x=a and y=b
   @arg a new x value
   @arg b new y value
   @return new Vector2d instance
 */
YF void YAC_CALL _vector2d (YAC_Object *,YAC_Object *,YAC_Value*);


// utility classes for readability's sake
YC class _Point2d : public _Vector2d {
  public:
   YAC_POOLED(_Point2d, YAC_POOL_PRIORITY_HIGH);
};

YC class _Size2d  : public _Vector2d {
  public:
   YAC_POOLED(_Size2d, YAC_POOL_PRIORITY_HIGH);
};


#endif // VECTOR2D_H_
