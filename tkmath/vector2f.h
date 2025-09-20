/// vector2f.h
///
/// (c) 2008-2025 by Carsten Busse <carsten.busse@googlemail.com>
///                  Bastian Spiegel <bs@tkscript.de> (additional coding)
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#ifndef VECTOR2F_H_
#define VECTOR2F_H_


YG("Math");

/* @class Vector2f,Object
This class represents a single precision 2 dimensional coordinate vector.
 */
YC class _Vector2f : public YAC_Object {
  public:
   sF32 floats[2];

  public:
   _Vector2f(void);
   ~_Vector2f();

   YAC_POOLED(_Vector2f, YAC_POOL_PRIORITY_HIGH);

   sUI   YAC_VCALL yacArrayGetNumElements     (void);
   sUI   YAC_VCALL yacArrayGetMaxElements     (void);
   sUI   YAC_VCALL yacArrayGetElementType     (void);
   sUI   YAC_VCALL yacArrayGetElementByteSize (void);
   void  YAC_VCALL yacArraySet                (YAC_ContextHandle, sUI, YAC_Value *);
   void  YAC_VCALL yacArrayGet                (YAC_ContextHandle, sUI, YAC_Value *);
   void *YAC_VCALL yacArrayGetPointer         (void);
   sSI   YAC_VCALL yacTensorRank              (void);
   void  YAC_VCALL yacSerialize               (YAC_Object *_ofs, sUI _rtti);
   sUI   YAC_VCALL yacDeserialize             (YAC_Object *_ifs, sUI _rtti);
   void  YAC_VCALL yacOperatorClamp           (YAC_Object *_min, YAC_Object *_max);
   void  YAC_VCALL yacOperatorWrap            (YAC_Object *_min, YAC_Object *_max);

   // supports:
   // YAC_OP_CEQ, YAC_OP_CNE, YAC_OP_ASSIGN, YAC_OP_ADD = _add,
   // YAC_OP_SUB = _sub, YAC_OP_MUL = _mul, YAC_OP_NEG, YAC_OP_INIT
   void  YAC_VCALL yacOperator               (sSI, YAC_Object *, YAC_Value *);
   void  YAC_VCALL yacOperatorI              (sSI, sSI, YAC_Value *);
   void  YAC_VCALL yacOperatorF32            (sSI, sF32, YAC_Value *);
   sUI   YAC_VCALL yacOperatorPriority       (void) { return YAC_OP_PRIO_FLOAT; }
   sBool YAC_VCALL yacToString               (YAC_String *) const;

  public:
   // Internal helper methods

   void assignFloats (const sF32 *_fv, sUI _ne);
   void assignFloatArray (YAC_FloatArray *_fa);
   void assignGeneric (YAC_ContextHandle _ctx, YAC_Object *_o);

  public:

   inline void copyFrom(const _Vector2f *_o) {
      floats[0] = _o->floats[0];
      floats[1] = _o->floats[1];
   }

   inline void add(const _Vector2f *_o) {
      floats[0] += _o->floats[0];
      floats[1] += _o->floats[1];
   }

   inline void sub(const _Vector2f *_o) {
      floats[0] -= _o->floats[0];
      floats[1] -= _o->floats[1];
   }

   inline void subFrom(const _Vector2f *_a, const _Vector2f *_b) {
      floats[0] = _a->floats[0] - _b->floats[0];
      floats[1] = _a->floats[1] - _b->floats[1];
   }

   inline sF32 dot(const _Vector2f *_o) const {
      return floats[0] * _o->floats[0] + floats[1] * _o->floats[1];
   }

   inline void mulfFrom(const _Vector2f *_o, const sF32 _s) {
      floats[0] = _o->floats[0] * _s;
      floats[1] = _o->floats[1] * _s;
   }

  public:

   /* @method New,float x,float y:Vector2f
      Initialize and return new Vector2f instance
      @return new Vector2f instance
    */
   YM void _New (sF32 _x, sF32 _y, YAC_Value *_r);

   /* @method init,float x,float y
      Initialize vector from scalars
    */
   YM void _init (sF32 _x, sF32 _y);

   /* @method setX,float a
      Set x element to a
      @arg a new value for x
    */
   YM void _setX (sF32 _x);

   /* @method setY,float a
      Set y element to a
      @arg a new value for y
    */
   YM void _setY (sF32 _y);

   /* @method getX:float
      Get value of x element
      @return value of x element
    */
   YM sF32 _getX (void) const;

   /* @method getY:float
      Get value of y element
      @return value of y element
    */
   YM sF32 _getY (void) const;

   /* @method getYX:float
      Get yx vector
      @return yx vector
    */
   YM void _getYx_YAC_RSELF (void) const;
   YM void _getYx_YAC_RVAL  (YAC_Value *_r) const;
   YM void _getYx_YAC_RARG  (YAC_Object *_o) const;

   /* @method getString:String
      Get string representation (x, y) of vector
      @return string representation of vector
    */
   YM void _getString (YAC_Value *) const;

   /* @method add,Vector2f v
      Add another vector v to this instance
      @arg v Vector to add
    */
   YM void _add_YAC_RSELF (YAC_Object *);
   YM void _add_YAC_RVAL (YAC_Object *, YAC_Value *);
   YM void _add_YAC_RARG (YAC_Object *, YAC_Object *) const;

   /* @method sub,Vector2f v
      Substract another vector v from this instance
      @arg v Vector to substract
    */
   YM void _sub_YAC_RSELF (YAC_Object *);
   YM void _sub_YAC_RVAL (YAC_Object *, YAC_Value *);
   YM void _sub_YAC_RARG (YAC_Object *, YAC_Object *) const;


   /* @method dot,Vector2f v:float
      Calculate dot product from this instance and v
      @arg v Vector2f instance
      @return Scalar value
    */
   YM sF32 _dot (YAC_Object *_o);

   /* @method tensor,Vector2f v:Matrix2f
      Create resulting matrix of tensor product from this instance and v
      @arg v Vector2f instance
      @return Matrix2f instance
    */
   YM void _tensor_YAC_RVAL (YAC_Object *, YAC_Value *);
   YM void _tensor_YAC_RARG (YAC_Object *_v2f, YAC_Object *_retM2f) const;

   /* @method addf,float v
      Add scalar value to all components
      @arg v scalar value
    */
   YM void _addf_YAC_RSELF (sF32);
   YM void _addf_YAC_RVAL (sF32, YAC_Value *);
   YM void _addf_YAC_RARG (sF32, YAC_Object *) const;

   /* @method add2f,float tx, float ty
      Translate x / y components
      @arg tx
      @arg ty
    */
   YM void _add2f_YAC_RSELF (sF32 _tx, sF32 _ty);
   YM void _add2f_YAC_RVAL  (sF32 _tx, sF32 _ty, YAC_Value *_r);
   YM void _add2f_YAC_RARG  (sF32 _tx, sF32 _ty, YAC_Object *_r) const;

   /* @method mulf,float s
      Multiply this instance by scalar value s
      @arg s scalar value
    */
   YM void _mulf_YAC_RSELF (sF32);
   YM void _mulf_YAC_RVAL (sF32, YAC_Value *);
   YM void _mulf_YAC_RARG (sF32, YAC_Object *) const;

   /* @method mul2f,float sx,float sy
      Scale x / y components
      @arg sx
      @arg sy
    */
   YM void _mul2f_YAC_RSELF (sF32 _sx, sF32 _sy);
   YM void _mul2f_YAC_RVAL  (sF32 _sx, sF32 _sy, YAC_Value *_r);
   YM void _mul2f_YAC_RARG  (sF32 _sx, sF32 _sy, YAC_Object *_r) const;

   /* @method mul,Vector2f v
      Multiply the x element of this instance by the x element of v and do the same with the y element
      @arg v Vector2f instance
    */
   YM void _mul_YAC_RSELF (YAC_Object *);
   YM void _mul_YAC_RVAL (YAC_Object *, YAC_Value *);
   YM void _mul_YAC_RARG (YAC_Object *, YAC_Object *) const;

   /* @method transform,Matrix m
      Multiply matrix by this vector.
      @arg m Matrix2f (all call styles), or Matrix[34]f (rval,rarg)
    */
   YM void _transform_YAC_RSELF (YAC_Object *_m);
   YM void _transform_YAC_RVAL  (YAC_Object *_m, YAC_Value *);
   YM void _transform_YAC_RARG  (YAC_Object *_m, YAC_Object *);

   /* @method getAbs:float
      Return length of vector
      @return length of vector
    */
   YM sF32 _getAbs(void) const;

   /* @method getAbsSqr:float
      Return squared length of vector
      @return length<sup>2</sup> of vector
    */
   YM sF32 _getAbsSqr (void) const;

   /* @method unit
      Scale vector to length 1 if possible
    */
   YM void _unit_YAC_RSELF (void);
   YM void _unit_YAC_RVAL (YAC_Value *);
   YM void _unit_YAC_RARG (YAC_Object *) const;

   /* @method unitScale,float s
      Set length to s
      @arg s New length of vector
    */
   YM void _unitScale_YAC_RSELF (sF32);
   YM void _unitScale_YAC_RVAL (sF32, YAC_Value *);
   YM void _unitScale_YAC_RARG (sF32, YAC_Object *) const;

   /* @method initScalef,Vector2f v,float s
      Initialise values to s*v
      @arg v Vector2f instance
      @arg s scale to multiply
    */
   YM void _initScalef (YAC_Object *_v, sF32 _s);

   /* @method distanceToPlane,Vector2f q,Vector2f n:float
      Calc distance of vertex to plane defined by Vector2f q (on plane) and plane normal n
      @arg q Point on plane
      @arg n Plane normal
      @return distance
    */
   /*  */
   YM sF32 _distanceToPlane (YAC_Object *_q, YAC_Object *_n) const;

   /* @method intersect,Vector2f v,float s
      Returns intersection point in 'this'
      Returns normalized position on edge v2s<>v2e.
      @arg v1s Edge 1 start
      @arg v1e Edge 1 end
      @arg v2s Edge 2 start
      @arg v2e Edge 2 end
    */
   YM sF32 intersect(YAC_Object *_v1s, YAC_Object *_v1e, YAC_Object *_v2s, YAC_Object *_v2e, sBool _bExtrapolate);

   /* @method bilinearQuadPos
      Calc interpolated quad position from normalized (0..1) coordinates (0;0=left/top)
    */
   YM void _bilinearQuadPos (YAC_Object *_vLT, YAC_Object *_vRT, YAC_Object *_vRB, YAC_Object *_vLB, sF32 _nx, sF32 _ny);

   /* @method rotateCW90
      Rotate 90 degrees clock-wise (x = y, y = -x)
   */
   YM void _rotateCW90_YAC_RSELF (void);
   YM void _rotateCW90_YAC_RVAL (YAC_Value *_r);
   YM void _rotateCW90_YAC_RARG (YAC_Object *_r) const;

   /* @method rotateCCW90
      Rotate 90 degrees counter-clock-wise (x = -y, y = x)
   */
   YM void _rotateCCW90_YAC_RSELF (void);
   YM void _rotateCCW90_YAC_RVAL (YAC_Value *_r);
   YM void _rotateCCW90_YAC_RARG (YAC_Object *_r) const;
};

/* @function vector2f,float a,float b:Vector2f
   Initialise and return new Vector2f instance with x=a and y=b
   @arg a new x value
   @arg b new y value
   @return new Vector2f instance
 */
YF void YAC_CALL _vector2f (sF32 _x, sF32 _y, YAC_Value *_r);


// utility classes for readability's sake
YC class _Point2f : public _Vector2f {
  public:
   YAC_POOLED(_Point2f, YAC_POOL_PRIORITY_HIGH);
};

YC class _Size2f  : public _Vector2f {
  public:
   YAC_POOLED(_Size2f, YAC_POOL_PRIORITY_HIGH);
};


#endif // VECTOR2F_H_
