/// vector4f.h
///
/// (c) 2008-2024 by Carsten Busse <carsten.busse@googlemail.com>,
///                  Bastian Spiegel <bs@tkscript.de> (additional coding)
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#ifndef VECTOR4F_H_
#define VECTOR4F_H_


YG("Math");

/* @class Vector4f,Object
This class represents a single precision 4 dimensional euclidean space vector.
 */
YC class _Vector4f : public YAC_Object {
  public:
   sF32 floats[4];

   public:
   _Vector4f(void);
   ~_Vector4f();

   YAC_POOLED(_Vector4f,YAC_POOL_PRIORITY_HIGH);

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
   void  YAC_VCALL yacOperatorAssign          (YAC_Object *_robj);
   sUI   YAC_VCALL yacOperatorPriority        (void) { return YAC_OP_PRIO_FLOAT; }
   sBool YAC_VCALL yacToString                (YAC_String*) const;

  public:
   // Helper methods
   void assignFloats (const sF32 *_fv, sUI _ne);
   void assignFloatArray (YAC_FloatArray *_fa);
   void assignGeneric (YAC_ContextHandle _ctx, YAC_Object *_o);
   void copytoobject_helper (YAC_Object *_o, const sUI _numComp, const sUI _idx0, const sUI _idx1, const sUI _idx2, const sUI _idx3) const;

   static inline sF32 Dot(const _Vector4f *_a, const _Vector4f *_b) {
      return
         _a->floats[0] * _b->floats[0] +
         _a->floats[1] * _b->floats[1] +
         _a->floats[2] * _b->floats[2] +
         _a->floats[3] * _b->floats[3] ;
   }

   inline void diffUnit(const _Vector4f *_a, const _Vector4f *_b) {
      // b - a
      floats[0] = _b->floats[0] - _a->floats[0];
      floats[1] = _b->floats[1] - _a->floats[1];
      floats[2] = _b->floats[2] - _a->floats[2];
      floats[3] = _b->floats[3] - _a->floats[3];

      // calc length
      sF32 l =  sqrtf(floats[0] * floats[0] +
                      floats[1] * floats[1] +
                      floats[2] * floats[2] +
                      floats[3] * floats[3]
                      );

      // normalize
      if(Dfltnonzero(l))
      {
         l = 1.0f / l;
         floats[0] *= l;
         floats[1] *= l;
         floats[2] *= l;
         floats[3] *= l;
      }
      else
      {
         floats[0] = 0.0f;
         floats[1] = 0.0f;
         floats[2] = 0.0f;
         floats[3] = 1.0f;
      }
   }

  public:

   /* @method New,float x,float y,float z,float w:Vector4f
      Initialize and return new Vector4f instance
    */
   YM void _New (sF32 _x, sF32 _y, sF32 _z, sF32 _w, YAC_Value *_r);

   /* @method init,float x,float y,float z,float w
      Initialize vector from scalars
    */
   YM void _init (sF32 _x, sF32 _y, sF32 _z, sF32 _w);

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

   /* @method setW,float a
      Set w element to a
      @arg a new value for w
    */
   YM void _setW (sF32);

   /* @method getX:float
      Get value of x element
      @return value of x element
    */
   YM sF32 _getX (void);

   /* @method getY:float
      Get value of y element
      @return value of y element
    */
   YM sF32 _getY (void);

   /* @method getZ:float
      Get value of z element
      @return value of z element
    */
   YM sF32 _getZ (void);

   /* @method getW:float
      Get value of w element
      @return value of w element
    */
   YM sF32 _getW (void);

   /* @method getXy:float
      Get xy Vector2f
      @return xy Vector2f
    */
   YM void _getXy_YAC_RVAL (YAC_Value *_r) const;
   YM void _getXy_YAC_RARG (YAC_Object *_o) const;

   /* @method getXz:float
      Get xz Vector2f
      @return xz Vector2f
    */
   YM void _getXz_YAC_RVAL (YAC_Value *_r) const;
   YM void _getXz_YAC_RARG (YAC_Object *_o) const;

   /* @method getYz:float
      Get yz Vector2f
      @return yz Vector2f
    */
   YM void _getYz_YAC_RVAL (YAC_Value *_r) const;
   YM void _getYz_YAC_RARG (YAC_Object *_o) const;

   /* @method getXyz:float
      Get xyz Vector3f
      @return xyz Vector3f
    */
   YM void _getXyz_YAC_RVAL (YAC_Value *_r) const;
   YM void _getXyz_YAC_RARG (YAC_Object *_o) const;

   /* @method getString:String
      Get string representation of vector (x, y, z, w)
      @return string representation of vector
    */
   YM void _getString (YAC_Value *) const;

   /* @method add,Vector4f v
      Add vector or array-like object
      @arg v vector to add
    */
   YM void _add_YAC_RSELF (YAC_Object *);
   YM void _add_YAC_RVAL (YAC_Object *, YAC_Value *);
   YM void _add_YAC_RARG (YAC_Object *, YAC_Object *) const;

   /* @method sub,Vector4f v
      Substract vector or array-like object
      @arg v vector to substract
    */
   YM void _sub_YAC_RSELF (YAC_Object *);
   YM void _sub_YAC_RVAL (YAC_Object *, YAC_Value *);
   YM void _sub_YAC_RARG (YAC_Object *, YAC_Object *) const;

   /* @method dot,Vector4f v:float
      Calculate scalar dot product from this instance and v
      @arg v Vector4f or array-like object
      @return scalar value
    */
   YM sF32 _dot (YAC_Object *);

   /* @method tensor,Vector4f v:Matrix4f
      Create resulting matrix of tensor product from this instance and v
      @arg a Vector4f instance
      @return Matrix4f instance
    */
   YM void _tensor_YAC_RVAL (YAC_Object *, YAC_Value *);
   YM void _tensor_YAC_RARG (YAC_Object *_v4f, YAC_Object *_retM4f) const;

   /* @method addf,float v
      Add scalar value to all components
      @arg v scalar value
    */
   YM void _addf_YAC_RSELF (sF32);
   YM void _addf_YAC_RVAL (sF32, YAC_Value *);
   YM void _addf_YAC_RARG (sF32, YAC_Object *) const;

   /* @method mulf,float v
      Multiply this instance by scalar value v
      @arg v scalar value
    */
   YM void _mulf_YAC_RSELF (sF32);
   YM void _mulf_YAC_RVAL (sF32, YAC_Value *);
   YM void _mulf_YAC_RARG (sF32, YAC_Object *) const;

   /* @method mul,Vector4f v
      Multiply the x element of this instance by the x element of v and do the same with the y, z and w elements
      @arg v Vector4f or array-like object
    */
   YM void _mul_YAC_RSELF (YAC_Object *_v);
   YM void _mul_YAC_RVAL  (YAC_Object *_v, YAC_Value *);
   YM void _mul_YAC_RARG  (YAC_Object *_v, YAC_Object *) const;

   /* @method transform,Matrix4f m
      Multiply matrix by this vector
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
      Set length to 1
    */
   YM void _unit_YAC_RSELF (void);
   YM void _unit_YAC_RVAL (YAC_Value *);
   YM void _unit_YAC_RARG (YAC_Object *) const;

   /* @method unitScale,float s
      Set length to s
      @arg s new length of vector
    */
   YM void _unitScale_YAC_RSELF (sF32);
   YM void _unitScale_YAC_RVAL (sF32, YAC_Value *);
   YM void _unitScale_YAC_RARG (sF32, YAC_Object *) const;

   /* @method initScalef,Vector4f v,float s
      Initialise vector to s*v
      @arg v Vector4f or array-like object
      @arg s scale to multiply
    */
   YM void _initScalef (YAC_Object *_o, sF32 _s);

   /* @method rotate,Matrix4f m
      Multiply matrix by this vector and replace this vector by the result

      This is a convenience method to remove the need for temporary Vector4f variables in scripts (the temporary is created on the stack instead).

      @arg m Matrix4f instance (rotation matrix)
    */
   YM void _rotate (YAC_Object *_m);

   /* @method distanceToPlane,Vector4f q,Vector4f n:float
      Calc distance of vertex to plane defined by Vector4f q (on plane) and plane normal n
      @arg q Point on plane
      @arg n Plane normal
      @return distance
    */
   YM sF32 _distanceToPlane (YAC_Object *_q, YAC_Object *_n) const;

   /* @method intersectPlane,Vector4f q,Vector4f n,Vector4f p1,Vector4f p2:boolean
      Calc intersection between line (p1->p2) and plane (q,n).
      @arg q Point on plane
      @arg n Plane normal
      @arg p1 Line start point
      @arg p2 Line end point
      @return true when intersection was found. false when plane and line are colinear (or when any parameter object is invalid).
    */
   YM sBool _intersectPlane (YAC_Object *_q, YAC_Object *_n, YAC_Object *_p1, YAC_Object *_p2);

   /* @method divz
      Divide by Z and return Vector2f
    */
   YM void _divz_YAC_RSELF (void);
   YM void _divz_YAC_RVAL (YAC_Value *_r) const;
   YM void _divz_YAC_RARG (YAC_Object *_r);

   /* @method divw
      Divide by W and return Vector3f
    */
   YM void _divw_YAC_RSELF (void);
   YM void _divw_YAC_RVAL (YAC_Value *_r) const;
   YM void _divw_YAC_RARG (YAC_Object *_r);

   /* @method bilinearQuadPos
      Calc interpolated quad position from normalized (0..1) coordinates (0;0=left/top)
    */
   YM void _bilinearQuadPos (YAC_Object *_vLT, YAC_Object *_vRT, YAC_Object *_vRB, YAC_Object *_vLB, sF32 _nx, sF32 _ny);

   /* @method project
      Convert clip to window coordinates
      @arg r Either a Vector2f, Vector3f, or Vector4f (returns projected coordinate)
      @arg vpX Viewport position X
      @arg vpX Viewport position Y
      @arg vpW Viewport width
      @arg vpH Viewport height
      @arg depthRangeNear Usually 0
      @arg depthRangeFar Usually 1
    */
   YM void _project (YAC_Object *_r, sF32 _vpX, sF32 _vpY, sF32 _vpW, sF32 _vpH, sF32 _depthRangeNear, sF32 _depthRangeFar) const;
};


/* @function vector4f,float x,float y,float z,float w:Vector4f
   Initialise and return new Vector4f instance
   @arg x new x value
   @arg y new y value
   @arg z new z value
   @arg w new w value
 */
YF void YAC_CALL _vector4f (sF32 _x, sF32 _y, sF32 _z, sF32 _w, YAC_Value *_r);


// utility classes for readability's sake
YC class _Point4f : public _Vector4f {
  public:
   YAC_POOLED(_Point4f, YAC_POOL_PRIORITY_HIGH);
};

YC class _Size4f  : public _Vector4f {
  public:
   YAC_POOLED(_Size4f, YAC_POOL_PRIORITY_HIGH);
};


#endif  // VECTOR4F_H_
