/// matrix2x3f.h
///
/// (c) 2024-2025 by Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#ifndef MATRIX2x3F_H_
#define MATRIX2x3F_H_

YG("Math");

/* @class Matrix2x3f,Object
This class represents a single precision 2x3 square matrix.
 */
YC class _Matrix2x3f : public _Matrix {
  public:
   sF32 floats[6];

  public:
   _Matrix2x3f(void);
   ~_Matrix2x3f();
   YAC_POOLED(_Matrix2x3f, YAC_POOL_PRIORITY_HIGH);

   sUI   YAC_VCALL yacArrayGetNumElements     (void);
   sUI   YAC_VCALL yacArrayGetMaxElements     (void);
   sUI   YAC_VCALL yacArrayGetElementType     (void);
   sUI   YAC_VCALL yacArrayGetElementByteSize (void);
   void  YAC_VCALL yacArraySet                (YAC_ContextHandle, sUI, YAC_Value *);
   void  YAC_VCALL yacArrayGet                (YAC_ContextHandle, sUI, YAC_Value *);
   void *YAC_VCALL yacArrayGetPointer         (void);
   sUI   YAC_VCALL yacArrayGetWidth           (void);
   sUI   YAC_VCALL yacArrayGetHeight          (void);
   sUI   YAC_VCALL yacArrayGetStride          (void);
   sSI   YAC_VCALL yacTensorRank              (void);
   void  YAC_VCALL yacSerialize               (YAC_Object *, sUI);
   sUI   YAC_VCALL yacDeserialize             (YAC_Object *, sUI);
   void  YAC_VCALL yacOperatorClamp           (YAC_Object *, YAC_Object *);
   void  YAC_VCALL yacOperatorWrap            (YAC_Object *, YAC_Object *);
   void  YAC_VCALL yacOperator                (sSI, YAC_Object *, YAC_Value *);
   void  YAC_VCALL yacOperatorI               (sSI, sSI, YAC_Value *);
   void  YAC_VCALL yacOperatorF32             (sSI, sF32, YAC_Value *);
   sUI   YAC_VCALL yacOperatorPriority        (void) { return YAC_OP_PRIO_FLOAT; }
   sBool YAC_VCALL yacToString                (YAC_String*) const;

   // Helper methods
   void assignFloats (sF32 *_fv, sUI _ne);
   void assignFloatArray (YAC_FloatArray *_fa);
   void assignGeneric (YAC_ContextHandle _ctx, YAC_Object *_o);

   /* @method New,float a,float b,float c,float d,float e,float f:Matrix2x3f
      Initialize and return new instance of Matrix2x3f
      @arg a value for a
      @arg b value for b
      @arg c value for c
      @arg d value for d
      @arg e value for e
      @arg f value for f
      @return new instance of Matrix2x3f
    */
   YM void _New (sF32 _a, sF32 _b, sF32 _c, sF32 _d, sF32 _e, sF32 _f, YAC_Value *_r);

   /* @method setA,float v
      Set a element to m11
      @arg m11 new value for a
    */
   YM void _setA (sF32 _m11);

   /* @method setB,float v
      Set b element to m12
      @arg m12 new value for b
    */
   YM void _setB (sF32 _m12);

   /* @method setC,float v
      Set c element to m13
      @arg m13 new value for c
    */
   YM void _setC (sF32 _m13);

   /* @method setD,float v
      Set d element to m21
      @arg m21 new value for d
    */
   YM void _setD (sF32 _m21);

   /* @method setE,float v
      Set e element to m22
      @arg m22 new value for e
    */
   YM void _setE (sF32 _m22);

   /* @method setF,float v
      Set f element to m23
      @arg m23 new value for f
    */
   YM void _setF (sF32 _m23);

   /* @method getA:float
      Get value of a element
      @return value of a element
    */
   YM sF32 _getA (void);

   /* @method getB:float
      Get value of b element
      @return value of b element
    */
   YM sF32 _getB (void);

   /* @method getC:float
      Get value of c element
      @return value of c element
    */
   YM sF32 _getC (void);

   /* @method getD:float
      Get value of d element
      @return value of d element
    */
   YM sF32 _getD (void);

   /* @method getE:float
      Get value of e element
      @return value of e element
    */
   YM sF32 _getE (void);

   /* @method getF:float
      Get value of f element
      @return value of f element
    */
   YM sF32 _getF(void);

   /* @method getString:String
      Get string representation of matrix ((a, b, c), (d, e, f), (g, h, i))
      @return string representation of matrix
    */
   YM void _getString (YAC_Value *_r) const;

   /* @method setRow,int row,Vector3f v
      Initialize row 'row' with values from v
      @arg row row number
      @arg v Vector3f instance
    */
   YM void _setRow(sUI _row, YAC_Object *_v);

   /* @method getRow,int row:Vector3f
      Get row 'row' vector
      @arg row row number
      @return vector with values from row r
    */
   YM void _getRow_YAC_RVAL(sUI _row, YAC_Value *_r);
   YM void _getRow_YAC_RARG(sUI _row, YAC_Object *_r) const;

   /* @method setCol,int c,Vector3f v
      Set column 'col' with values from v
      @arg c column number
      @arg v Vector3f instance
    */
   YM void _setCol(sUI _col, YAC_Object *_v);

   /* @method getCol,int col:Vector3f
      Get column 'col' values
      @arg col column index
      @return vector with values from column c
    */
   YM void _getCol_YAC_RVAL (sUI _col, YAC_Value *_r);
   YM void _getCol_YAC_RARG (sUI _col, YAC_Object *_r) const;

   /* @method initIdentity
      Initialize with identity matrix.
   */
   YM void _initIdentity (void);

   /* @method init,float a,float b,float c,float d,float e, float f
      Initialize matrix (in row-major order)
      @arg a value for m11
      @arg b value for m12
      @arg c value for m13
      @arg d value for m21
      @arg e value for m22
      @arg f value for m23
    */
   YM void _init (sF32 _a, sF32 _b, sF32 _c, sF32 _d, sF32 _e, sF32 _f);

   /* @method initRow,int row,float a,float b,float c
      Initialize matrix row from scalars
      @arg row row number
      @arg a value for first column
      @arg b value for second column
      @arg c value for third column
    */
   YM void _initRowf (sUI _row, sF32 _a, sF32 _b, sF32 _c);

   /* @method initRowV,int row,Vector3f v
      Initialize matrix row from vector or array-like object
      @arg row row index
      @arg v vector with values
    */
   YM void _initRowv (sUI _row, YAC_Object *_v);

   /* @method initColf,int col,float a,float b,float c
      Initialize matrix column from scalars
      @arg col column index
      @arg a value for first element
      @arg b value for second element
    */
   YM void _initColf (sUI _col, sF32 _a, sF32 _b);

   /* @method initColv,int col,Vector2f v
      Initialize matrix column from vector or array-like object
      @arg c column index
      @arg v vector with values
    */
   YM void _initColv (sUI _col, YAC_Object *_v);

   /* @method initOrtho
    */
   YM void _initOrtho (sF32 _left, sF32 _right, sF32 _bottom, sF32 _top);

   /* @method _initScalef
    */
   YM void _initScalef (sF32 _x, sF32 _y);

   /* @method _initScalev
      @arg scale Vector2f
    */
   YM void _initScalev (YAC_Object *_scale);

   /* @method _initSkewXf
      a in radians (0..2PI)
      see https://www.w3.org/TR/css-transforms-1/#SkewXDefined
    */
   YM void _initSkewXf (sF32 _a);

   /* @method _initSkewYf
      a in radians (0..2PI)
    */
   YM void _initSkewYf (sF32 _a);

   /* @method _initTranslatef
    */
   YM void _initTranslatef (sF32 _x, sF32 _y);

   /* @method _initTranslatev
    */
   YM void _initTranslatev (YAC_Object *_translate);

   /* @method _initRotatef
      @arg a rotation angle (0..2PI)
    */
   YM void _initRotatef (sF32 _a);

   /* @method add,Matrix2xf m
      Add another matrix m to this instance
      @arg m matrix to add
    */
   YM void _add_YAC_RSELF (YAC_Object *_m);
   YM void _add_YAC_RVAL  (YAC_Object *_m, YAC_Value *_r);
   YM void _add_YAC_RARG  (YAC_Object *_m, YAC_Object *_r) const;

   /* @method sub,Matrix2x3f m
      Substract another matrix m from this instance
      @arg m matrix to substract
    */
   YM void _sub_YAC_RSELF (YAC_Object *_m);
   YM void _sub_YAC_RVAL  (YAC_Object *_m, YAC_Value *_r);
   YM void _sub_YAC_RARG  (YAC_Object *_m, YAC_Object *_r) const;

   /* @method mulf,float f
      Multiply this instance by scalar value f
      @arg f value to multiply
    */
   YM void _mulf_YAC_RSELF (sF32 _f);
   YM void _mulf_YAC_RVAL  (sF32 _f, YAC_Value *_r);
   YM void _mulf_YAC_RARG  (sF32 _f, YAC_Object *_r) const;

   /* @method mul,Matrix2x3f m
      Multiply this instance by matrix m (this * m)
      @arg m matrix to multiply (rhs)
    */
   YM void _mul_YAC_RSELF (YAC_Object *_m);
   YM void _mul_YAC_RVAL  (YAC_Object *_m, YAC_Value *_r);
   YM void _mul_YAC_RARG  (YAC_Object *_m, YAC_Object *_r) const;

   /* @method mulRev,Matrix2x3f m
      Reverse-multiply matrix m by this instance (m * this)
      @arg m matrix to multiply (lhs)
    */
   YM void _mulRev_YAC_RSELF (YAC_Object *_m);
   YM void _mulRev_YAC_RVAL  (YAC_Object *_m, YAC_Value *_r);
   YM void _mulRev_YAC_RARG  (YAC_Object *_m, YAC_Object *_r) const;

   /* @method mulv,Vector2f v:Vector2f
      Multiply matrix by a vector
      @arg v vector to multiply
      @return resulting vector
    */
   YM void _mulv_YAC_RVAL (YAC_Object *_v, YAC_Value *_r);
   YM void _mulv_YAC_RARG (YAC_Object *_v, YAC_Object *_r) const;

   /* @method getAbs:float
      Calculate the Hilbert-Schmidt norm of the matrix
      @return scalar matrix norm value
    */
   YM sF32 _getAbs (void) const;

   /* @method getAbsSqr:float
      Calculate the squared Hilbert-Schmidt norm of the matrix
      @return scalar matrix norm value
    */
   YM sF32 _getAbsSqr (void) const;

   /* @method unit
      Normalise matrix with Hilbert-Schmidt norm
    */
   YM void _unit_YAC_RSELF (void);
   YM void _unit_YAC_RVAL  (YAC_Value *_r);
   YM void _unit_YAC_RARG  (YAC_Object *_r) const;

   /* @method unitScale,float s
      Normalise matrix using Hilbert-Schmidt norm and multiply by s afterwards
      @arg s scaling factor
    */
   YM void _unitScale_YAC_RSELF (sF32 _s);
   YM void _unitScale_YAC_RVAL  (sF32 _s, YAC_Value *_r);
   YM void _unitScale_YAC_RARG  (sF32 _s, YAC_Object *_r) const;

   /* @method translatef,float x,float y
      Multiply by translation matrix
   */
   YM void _translatef_YAC_RSELF (sF32 _x, sF32 _y);
   YM void _translatef_YAC_RVAL  (sF32 _x, sF32 _y, YAC_Value *);
   YM void _translatef_YAC_RARG  (sF32 _x, sF32 _y, YAC_Object *);

   /* @method translatev,Vector2f v
      Multiply by translation matrix
   */
   YM void _translatev_YAC_RSELF (YAC_Object *_v);
   YM void _translatev_YAC_RVAL  (YAC_Object *_v, YAC_Value *_r);
   YM void _translatev_YAC_RARG  (YAC_Object *_v, YAC_Object *_r);

   /* @method scalef,float x,float y
      Multiply by scale matrix
   */
   YM void _scalef_YAC_RSELF (sF32 _x, sF32 _y);
   YM void _scalef_YAC_RVAL  (sF32 _x, sF32 _y, YAC_Value *);
   YM void _scalef_YAC_RARG  (sF32 _x, sF32 _y, YAC_Object *);

   /* @method scalev,Vector2f v
      Multiply by scale matrix
   */
   YM void _scalev_YAC_RSELF (YAC_Object *_v);
   YM void _scalev_YAC_RVAL  (YAC_Object *_v, YAC_Value *_r);
   YM void _scalev_YAC_RARG  (YAC_Object *_v, YAC_Object *_r);

   /* @method rotatef,float ang
      Multiply by rotation matrix (angle is in radian measure)
   */
   YM void _rotatef_YAC_RSELF (sF32 _ang);
   YM void _rotatef_YAC_RVAL  (sF32 _ang, YAC_Value *_r);
   YM void _rotatef_YAC_RARG  (sF32 _ang, YAC_Object *_r);

};

/* @function matrix2x3f,float a,float b,float c,float d,float e,float f:Matrix2x3f
   Initialize and return new instance of Matrix2x3f
   @arg a value for a
   @arg b value for b
   @arg c value for c
   @arg d value for d
   @arg e value for e
   @arg f value for f
   @return new instance of Matrix3f
 */
YF void YAC_CALL _matrix2x3f (sF32 _a, sF32 _b, sF32 _c, sF32 _d, sF32 _e, sF32 _f, YAC_Value *_r);


#endif // MATRIX2x3F_H_
