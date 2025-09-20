/// vector3f.h
///
/// (c) 2008-2024 by Carsten Busse <carsten.busse@googlemail.com>,
///                  Bastian Spiegel <bs@tkscript.de> (additional coding)
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#ifndef VECTOR3F_H_
#define VECTOR3F_H_


YG("Math");

/* @class Vector3f
This class represents a single precision 3 dimensional coordinate vector.
 */
YC class _Vector3f : public YAC_Object {
  public:
   sF32 floats[3];

  public:
   _Vector3f(void);
   ~_Vector3f();

   YAC_POOLED(_Vector3f, YAC_POOL_PRIORITY_HIGH);

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
   sUI   YAC_VCALL yacOperatorPriority        (void) { return YAC_OP_PRIO_FLOAT; }
   sBool YAC_VCALL yacToString                (YAC_String*) const;

   // Helper methods
   void assignFloats (const sF32 *_fv, sUI _ne);
   void assignFloatArray (YAC_FloatArray *_fa);
   void assignGeneric (YAC_ContextHandle _ctx, YAC_Object *_o);

   void initFromArrayObject (YAC_Object *_v);

   void copytoobject_helper (YAC_Object *_o, const sUI _numComp, const sUI _idx0, const sUI _idx1, const sUI _idx2) const;

   static inline sF32 Dot(const _Vector3f *_a, const _Vector3f *_b) {
      return
         _a->floats[0] * _b->floats[0] +
         _a->floats[1] * _b->floats[1] +
         _a->floats[2] * _b->floats[2] ;
   }

   inline void diffUnit(const _Vector3f *_a, const _Vector3f *_b) {
      // b - a
      floats[0] = _b->floats[0] - _a->floats[0];
      floats[1] = _b->floats[1] - _a->floats[1];
      floats[2] = _b->floats[2] - _a->floats[2];

      // calc length
      sF32 l =  ::sqrtf(floats[0] * floats[0] +
                        floats[1] * floats[1] +
                        floats[2] * floats[2]
                        );

      // normalize
      if(Dfltnonzero(l))
      {
         l = 1.0f / l;
         floats[0] *= l;
         floats[1] *= l;
         floats[2] *= l;
      }
      else
      {
         floats[0] = 0.0f;
         floats[1] = 0.0f;
         floats[2] = 0.0f;
      }
   }


  public:

   /* @method New,float x,float y,float z:Vector3f
      Initialize and return new Vector3f instance
    */
   YM void _New (sF32 _x, sF32 _y, sF32 _z, YAC_Value *_r);

   /* @method setX,float a
      Set x element to a
      @arg a new value for x
    */
   YM void _setX (sF32);

   /* @method setY,float a
      Set y element to a
      @arg a new value for y
    */
   YM void _setY (sF32);

   /* @method setZ,float a
      Set z element to a
      @arg a new value for z
    */
   YM void _setZ (sF32);

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

   /* @method getZ:float
      Get value of z element
      @return value of z element
    */
   YM sF32 _getZ (void) const;

   /* @method getXy:float
      Get xy vector
      @return xy vector
    */
   YM void _getXy_YAC_RVAL (YAC_Value *_r) const;
   YM void _getXy_YAC_RARG (YAC_Object *_o) const;

   /* @method getXz:float
      Get xz vector
      @return xz vector
    */
   YM void _getXz_YAC_RVAL (YAC_Value *_r) const;
   YM void _getXz_YAC_RARG (YAC_Object *_o) const;

   /* @method getYx:float
      Get yx vector
      @return yx vector
    */
   YM void _getYx_YAC_RVAL (YAC_Value *_r) const;
   YM void _getYx_YAC_RARG (YAC_Object *_o) const;

   /* @method getString:String
      Get string representation of vector (x, y, z)
      @return string representation of vector
    */
   YM void _getString(YAC_Value *) const;

   /* @method init,float x,float y,float z
      Initialize vector from scalars
    */
   YM void _init (sF32 _x, sF32 _y, sF32 _z);

   /* @method add,Vector3f v
      Add another vector v to this instance
      @arg v vector to add
    */
   YM void _add_YAC_RSELF (YAC_Object *);
   YM void _add_YAC_RVAL (YAC_Object *, YAC_Value *);
   YM void _add_YAC_RARG (YAC_Object *, YAC_Object *) const;

   /* @method addf,float v
      Add scalar value to all components
      @arg v scalar value
    */
   YM void _addf_YAC_RSELF (sF32);
   YM void _addf_YAC_RVAL (sF32, YAC_Value *);
   YM void _addf_YAC_RARG (sF32, YAC_Object *) const;

   /* @method add3f,float tx, float ty, float tz
      Translate x / y / z components
      @arg tx
      @arg ty
      @arg tz
    */
   YM void _add3f_YAC_RSELF (sF32 _tx, sF32 _ty, sF32 _tz);
   YM void _add3f_YAC_RVAL  (sF32 _tx, sF32 _ty, sF32 _tz, YAC_Value *_r);
   YM void _add3f_YAC_RARG  (sF32 _tx, sF32 _ty, sF32 _tz, YAC_Object *_r) const;

   /* @method sub,Vector3f v
      Substract another vector v from this instance
      @arg v vector to substract
    */
   YM void _sub_YAC_RSELF (YAC_Object *);
   YM void _sub_YAC_RVAL (YAC_Object *, YAC_Value *);
   YM void _sub_YAC_RARG (YAC_Object *, YAC_Object *) const;

   /* @method dot,Vector3f v:float
      Calculate scalar dot product from this instance and v
      @arg v Vector3f instance
      @return scalar value
    */
   YM sF32 _dot (YAC_Object *);

   /* @method cross,Vector3f v:Vector3f
      Calculate cross product from this instance and v
      @arg v Vector3f instance
      @return orthogonal vector to this instance and v
    */
   YM void _cross_YAC_RVAL (YAC_Object *, YAC_Value *);
   YM void _cross_YAC_RARG (YAC_Object *, YAC_Object *) const;

   //triple product (spatprodukt)
   /* @method triple,Vector3f b,Vector3f c:float
      Calculate triple product this.dot(b.cross(c)) (a*(bxc))
      ærg b second parameter of triple product
      arg c third parameter of triple product
      @return scalar value
    */
   YM sF32 _triple (YAC_Object *, YAC_Object *);

   //tensor product, produces a Matrix3f out of two Vector3f. The result is written to _retM3f
   /* @method tensor,Vector3f v:Matrix3f
      Create resulting matrix of tensor product from this instance and v
      @arg a Vector3f instance
      @return Matrix3f instance
    */
   YM void _tensor_YAC_RVAL (YAC_Object *, YAC_Value *);
   YM void _tensor_YAC_RARG (YAC_Object *_v3f, YAC_Object *_retM3f) const;

   /* @method mulf,float v
      Multiply this instance by scalar value v
      @arg v scalar value
    */
   YM void _mulf_YAC_RSELF (sF32);
   YM void _mulf_YAC_RVAL (sF32, YAC_Value *);
   YM void _mulf_YAC_RARG (sF32, YAC_Object *) const;

   /* @method mul3f,float sx,float sy,float sz
      Scale x / y / z components
      @arg sx
      @arg sy
      @arg sz
    */
   YM void _mul3f_YAC_RSELF (sF32 _sx, sF32 _sy, sF32 _sz);
   YM void _mul3f_YAC_RVAL  (sF32 _sx, sF32 _sy, sF32 _sz, YAC_Value *_r);
   YM void _mul3f_YAC_RARG  (sF32 _sx, sF32 _sy, sF32 _sz, YAC_Object *_r) const;

   /* @method mul,Vector3f v
      Multiply the x element of this instance by the x element of v and do the same with the y and z element
      @arg v Vector3f instance
    */
   YM void _mul_YAC_RSELF (YAC_Object *);
   YM void _mul_YAC_RVAL (YAC_Object *, YAC_Value *);
   YM void _mul_YAC_RARG (YAC_Object *, YAC_Object *) const;

   /* @method transform,Matrix m
      Multiply matrix by this vector.
      @arg m Matrix3f (all call styles), or Matrix4f (rval,rarg)
    */
   YM void _transform_YAC_RSELF (YAC_Object *_m);
   YM void _transform_YAC_RVAL  (YAC_Object *_m, YAC_Value *);
   YM void _transform_YAC_RARG  (YAC_Object *_m, YAC_Object *);

   /* @method getAbs:float
      Return length of vector
      @return length of vector
    */
   YM sF32 _getAbs (void) const;

   /* @method getAbsSqr:float
      Return squared length of vector
      @return length<sup>2</sup> of vector
    */
   YM sF32 _getAbsSqr (void) const;

   /* @method unit
      Scale vector to length 1 if possible
    */
   YM void _unit_YAC_RSELF (void); /* unit */
   YM void _unit_YAC_RVAL (YAC_Value *);
   YM void _unit_YAC_RARG (YAC_Object *) const;

   //set Vector length to Scale
   /* @method unitScale,float s
      Set length to s
      @arg s new length of vector
    */
   YM void _unitScale_YAC_RSELF (sF32);
   YM void _unitScale_YAC_RVAL (sF32, YAC_Value *);
   YM void _unitScale_YAC_RARG (sF32, YAC_Object *) const;

   /* @method initScalef,Vector3f v,float s
      Initialize vector to s*v
      @arg v Vector3f instance
      @arg s scale to multiply
    */
   YM void _initScalef (YAC_Object *_o, sF32 _s);

   /* @method distanceToPlane,Vector3f q,Vector3f n:float
      Calc distance of vertex to plane defined by Vector3f q (on plane) and plane normal n
      @arg q Point on plane
      @arg n Plane normal
      @return distance
    */
   YM sF32 _distanceToPlane (YAC_Object *_q, YAC_Object *_n) const;

   /* @method intersectPlane,Vector3f q,Vector3f n,Vector3f p1,Vector3f p2:boolean
      Calc intersection between line (p1->p2) and plane (q,n).
      @arg q Point on plane
      @arg n Plane normal
      @arg p1 Line start point
      @arg p2 Line end point
      @return true when intersection was found. false when plane and line are colinear (or when any parameter object is invalid).
    */
   YM sBool _intersectPlane (YAC_Object *_q, YAC_Object *_n, YAC_Object *_p1, YAC_Object *_p2);

   /* @method divz
      Divide by Z and return (2D) Vector2f
    */
   YM void _divz_YAC_RSELF (void);
   YM void _divz_YAC_RVAL (YAC_Value *_r) const;
   YM void _divz_YAC_RARG (YAC_Object *_r);

   /* @method bilinearQuadPos
      Calc interpolated quad position from normalized (0..1) coordinates (0;0=left/top)
    */
   YM void _bilinearQuadPos (YAC_Object *_vLT, YAC_Object *_vRT, YAC_Object *_vRB, YAC_Object *_vLB, sF32 _nx, sF32 _ny);
};

/* @function vector3f,float a,float b,float c:Vector3f
   Initialize and return new Vector3f instance with x=a, y=b and z=c
   @arg a new x value
   @arg b new y value
   @arg c new z value
 */
YF void YAC_CALL _vector3f (sF32 _x, sF32 _y, sF32 _z, YAC_Value *_r);


// utility classes for readability's sake
YC class _Point3f : public _Vector3f {
  public:
   YAC_POOLED(_Point3f, YAC_POOL_PRIORITY_HIGH);
};

YC class _Size3f  : public _Vector3f {
  public:
   YAC_POOLED(_Size3f, YAC_POOL_PRIORITY_HIGH);
};


#endif // VECTOR3F_H_
