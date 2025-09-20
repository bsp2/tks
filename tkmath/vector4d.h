/// vector4d.h
///
/// (c) 2008-2024 by Carsten Busse <carsten.busse@googlemail.com>,
///                  Bastian Spiegel <bs@tkscript.de> (additional coding)
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#ifndef VECTOR4D_H_
#define VECTOR4D_H_


YG("Math");

/* @class Vector4d,Object
This class represents a double precision 4 dimensional euclidean space vector.
 */
YC class _Vector4d : public YAC_Object {
  public:
   sF64 doubles[4];

   public:
   _Vector4d(void);
   ~_Vector4d();

   YAC_POOLED(_Vector4d,YAC_POOL_PRIORITY_HIGH);

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
   void  YAC_VCALL yacOperatorF64             (sSI, sF64, YAC_Value *);
   sUI   YAC_VCALL yacOperatorPriority        (void) { return YAC_OP_PRIO_DOUBLE; }
   sBool YAC_VCALL yacToString                (YAC_String*) const;

  public:
   // Helper methods
   void assignFloats (const sF32 *_fv, sUI _ne);
   void assignFloatArray (YAC_FloatArray *_fa);
   void assignGeneric (YAC_ContextHandle _ctx, YAC_Object *_o);

  public:

   /* @method setX,Double a
      Set x element to a
      @arg a new value for x
    */
   YM void _setX (YAC_Object *_x);

   /* @method setY,Double a
      Set y element to a
      @arg a new value for y
    */
   YM void _setY (YAC_Object *_y);

   /* @method setZ,Double a
      Set z element to a
      @arg a new value for z
    */
   YM void _setZ (YAC_Object *_z);

   /* @method setW,Double a
      Set w element to a
      @arg a new value for w
    */
   YM void _setW (YAC_Object *_w);

   /* @method getX:Double
      Get value of x element
      @return value of x element
    */
   YM void _getX_YAC_RVAL(YAC_Value *_r);
   YM void _getX_YAC_RARG(YAC_Object *);

   /* @method getY:Double
      Get value of y element
      @return value of y element
    */
   YM void _getY_YAC_RVAL (YAC_Value *_r);
   YM void _getY_YAC_RARG (YAC_Object *);

   /* @method getZ:Double
      Get value of z element
      @return value of z element
    */
   YM void _getZ_YAC_RVAL (YAC_Value *_r);
   YM void _getZ_YAC_RARG (YAC_Object *);

   /* @method getW:Double
      Get value of w element
      @return value of w element
    */
   YM void _getW_YAC_RVAL (YAC_Value *_r);
   YM void _getW_YAC_RARG (YAC_Object *);

   /* @method getString:String
      Get string representation of vector (x, y, z, w)
      @return string representation of vector
    */
   YM void _getString (YAC_Value *_r) const;

   /* @method add,Vector4d v
      Add another vector v to this instance
      @arg v vector to add
    */
   YM void _add_YAC_RSELF (YAC_Object *);
   YM void _add_YAC_RVAL (YAC_Object *, YAC_Value *_r);
   YM void _add_YAC_RARG (YAC_Object *, YAC_Object *);

   /* @method sub,Vector4d v
      Substract another vector v from this instance
      @arg v vector to substract
    */
   YM void _sub_YAC_RSELF (YAC_Object *);
   YM void _sub_YAC_RVAL (YAC_Object *, YAC_Value *_r);
   YM void _sub_YAC_RARG (YAC_Object *, YAC_Object *);

   /* @method dot,Vector4d v:Double
      Calculate scalar dot product from this instance and v
      @arg v Vector4d instance
      @return scalar value
    */
   sF64 dot (YAC_Object *);
   YM void _dot_YAC_RVAL (YAC_Object *, YAC_Value *_r);
   YM void _dot_YAC_RARG (YAC_Object *, YAC_Object *);

   //tensor product, produces a Matrix4f out of two Vector4d. Result is written to _retM4f
   /* @method tensor,Vector4d v:Matrix4f
      Create resulting matrix of tensor product from this instance and v
      @arg a Vector4d instance
      @return Matrix4f instance
    */
   YM void _tensor_YAC_RVAL (YAC_Object *, YAC_Value *_r);
   YM void _tensor_YAC_RARG (YAC_Object *_v4f, YAC_Object *_retM4f);

   /* @method muld,Double v
      Multiply this instance by scalar value v
      @arg v scalar value
    */
   void muld (sF64);
   YM void _muld_YAC_RSELF (YAC_Object *);
   YM void _muld_YAC_RVAL (YAC_Object *, YAC_Value *_r);
   YM void _muld_YAC_RARG (YAC_Object *, YAC_Object *);

   /* @method mul,Vector4d v
      Multiply the x element of this instance by the x element of v and do the same with the y, z and w elements
      @arg v Vector4d instance
    */
   YM void _mul_YAC_RSELF (YAC_Object *);
   YM void _mul_YAC_RVAL (YAC_Object *, YAC_Value *_r);
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
   YM void _unit_YAC_RSELF (void);
   YM void _unit_YAC_RVAL (YAC_Value *);
   YM void _unit_YAC_RARG (YAC_Object *);

   /* @method unitScale,Double s
      Set length to s
      @arg s new length of vector
    */
   void unitScale (sF64);
   YM void _unitScale_YAC_RSELF (YAC_Object *);
   YM void _unitScale_YAC_RVAL (YAC_Object *, YAC_Value *);
   YM void _unitScale_YAC_RARG (YAC_Object *, YAC_Object *);

   /* @method initf,float a,float b,float c,float d
      Initialise values to single precision x=a, y=b, z=c and w=d
      @arg a new x value
      @arg b new y value
      @arg c new z value
      @arg d new w value
    */
   YM void _initf (sF32, sF32, sF32, sF32);


   /* @method init,Double a,Double b,Double c,Double d
      Initialise values to x=a, y=b, z=c and w=d
      @arg a new x value
      @arg b new y value
      @arg c new z value
      @arg d new w value
    */
   YM void _init (YAC_Object *, YAC_Object *, YAC_Object *, YAC_Object *);

   /* @method initScaled,Vector4d v,Double s
      Initialise vector to s*v
      @arg v Vector4d instance
      @arg s scale to multiply
    */
   YM void _initScaled (YAC_Object *_o, YAC_Object *_s);


   /* @method New,Double a,Double b,Double c,Double d:Vector4d
      Initialise and return new Vector4d instance with x=a, y=b, z=c and w=d
      @arg a new x value
      @arg b new y value
      @arg c new z value
      @arg d new w value
    */
   YM void _New (YAC_Object *, YAC_Object *, YAC_Object *, YAC_Object *, YAC_Value *);
};


/* @function vector4d,Double a,Double b,Double c,Double d:Vector4d
   Initialise and return new Vector4d instance with x=a, y=b, z=c and w=d
   @arg a new x value
   @arg b new y value
   @arg c new z value
   @arg d new w value
 */
YF void YAC_CALL _vector4d (YAC_Object *, YAC_Object *, YAC_Object *, YAC_Object *, YAC_Value*);


// utility classes for readability's sake
YC class _Point4d : public _Vector4d {
  public:
   YAC_POOLED(_Point4d, YAC_POOL_PRIORITY_HIGH);
};

YC class _Size4d  : public _Vector4d {
  public:
   YAC_POOLED(_Size4d, YAC_POOL_PRIORITY_HIGH);
};


#endif // VECTOR4D_H_
