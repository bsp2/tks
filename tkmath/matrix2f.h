/// matrix2f.h
///
/// (c) 2008-2025 by Carsten Busse <carsten.busse@googlemail.com>
///                  Bastian Spiegel <bs@tkscript.de> (additional coding)
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#ifndef MATRIX2F_H_
#define MATRIX2F_H_


YG("Math");

/* @class Matrix2f,Object
This class represents a single precision 2x2 square matrix.

The default layout is row-major.
   0 1  =  a b
   2 3     c d
 */
YC class _Matrix2f : public _Matrix {
  public:
   sF32 floats[4];

   public:
   _Matrix2f(void);
   ~_Matrix2f();
   YAC_POOLED(_Matrix2f,YAC_POOL_PRIORITY_HIGH);

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
//YAC_OP_CEQ, YAC_OP_CNE, YAC_OP_ASSIGN, YAC_OP_ADD = _add,
//YAC_OP_SUB = _sub, YAC_OP_MUL = _mul, YAC_OP_NEG, YAC_OP_INIT
   void  YAC_VCALL yacOperator                (sSI, YAC_Object *, YAC_Value *);
   void  YAC_VCALL yacOperatorI               (sSI, sSI, YAC_Value *);
   void  YAC_VCALL yacOperatorF32             (sSI, sF32, YAC_Value *);
   sUI   YAC_VCALL yacOperatorPriority        (void) { return YAC_OP_PRIO_FLOAT; }
   sBool YAC_VCALL yacToString                (YAC_String *) const;

   // Helper methods
   void assignFloats (sF32 *_fv, sUI _ne);
   void assignFloatArray (YAC_FloatArray *_fa);
   void assignFloatMatrix (YAC_Object *_o);
   void assignGeneric (YAC_ContextHandle _ctx, YAC_Object *_o);

   /* @method New,float va,float vb,float vc,float vd:Matrix2f
      Initialize and return new instance of Matrix2f (args are in row-major order)
      @arg a value for a
      @arg b value for b
      @arg c value for c
      @arg d value for d
      @return new instance of Matrix2f
    */
   YM void _New (sF32 _a, sF32 _b, sF32 _c, sF32 _d, YAC_Value *_r);

   /* @method setA,float v
      Set a element to v
      @arg v new value for a
    */
   YM void _setA (sF32);

   /* @method setB,float v
      Set b element to v
      @arg v new value for b
    */
   YM void _setB (sF32);

   /* @method setC,float v
      Set c element to v
      @arg v new value for c
    */
   YM void _setC (sF32);

   /* @method setD,float v
      Set d element to v
      @arg v new value for d
    */
   YM void _setD (sF32);

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

   /* @method getString:String
      Get string representation of matrix ((a, b), (c, d))
      @return string representation of matrix
    */
   YM void _getString (YAC_Value *) const;

   /* @method initIdentity
      Initialize with identity matrix.
   */
   YM void _initIdentity(void);

   /* @method isIdentity
      Check if this is an identity matrix (diagonal=1, other=0)
   */
   YM sBool _isIdentity (void);

   /* @method init,float a,float b,float c,float d
      Initialize matrix (in matrix order, default=row-major)
      @arg a value for m11
      @arg b value for m12
      @arg c value for m22
      @arg d value for m23
    */
   YM void _init (sF32 _a, sF32 _b, sF32 _c, sF32 _d);

   /* @method initRowMajor,float a,float b,float c,float d
      Initialize matrix (row-major arg order. swap args if matrix is in (non-default) column-major order).
      @arg a value for m11
      @arg b value for m12
      @arg c value for m21
      @arg d value for m22
    */
   YM void _initRowMajor (sF32 _a, sF32 _b, sF32 _c, sF32 _d);

   /* @method initColumnMajor,float a,float b,float c,float d
      Initialize matrix (column-major arg order. swap args if matrix is in (default) row-major order).
      @arg a value for m11
      @arg b value for m21
      @arg c value for m12
      @arg d value for m22
    */
   YM void _initColumnMajor (sF32 _a, sF32 _b, sF32 _c, sF32 _d);

   /* @method initTranspose,float a,float b,float c,float d
      Initialize matrix (swap arg row/column order).
      @arg a value for m11
      @arg b value for m12
      @arg c value for m13
      @arg d value for m21
    */
   YM void _initTranspose (sF32 _a, sF32 _b, sF32 _c, sF32 _d);

   /* @method add,Matrix2f m
      Add another matrix m to this instance
      @arg m matrix to add
    */
   YM void _add_YAC_RSELF (YAC_Object *);
   YM void _add_YAC_RVAL (YAC_Object *, YAC_Value *);
   YM void _add_YAC_RARG (YAC_Object *, YAC_Object *) const;

   /* @method sub,Matrix2f m
      Substract another matrix m from this instance
      @arg m matrix to substract
    */
   YM void _sub_YAC_RSELF (YAC_Object *);
   YM void _sub_YAC_RVAL (YAC_Object *, YAC_Value *);
   YM void _sub_YAC_RARG (YAC_Object *, YAC_Object *) const;

   /* @method mulf,float v
      Multiply this instance by scalar value v
      @arg v value to multiply
    */
   YM void _mulf_YAC_RSELF (sF32);
   YM void _mulf_YAC_RVAL (sF32, YAC_Value *);
   YM void _mulf_YAC_RARG (sF32, YAC_Object *) const;

   /* @method mul,Matrix2f m
      Multiply this instance by matrix m (this * m)
      @arg m matrix to multiply
    */
   YM void _mul_YAC_RSELF (YAC_Object *);
   YM void _mul_YAC_RVAL (YAC_Object *, YAC_Value *);
   YM void _mul_YAC_RARG (YAC_Object *, YAC_Object *) const;

   /* @method mulRev,Matrix2f m
      Reverse-multiply matrix m by this instance (m * this)
      @arg m matrix to multiply
    */
   YM void _mulRev_YAC_RSELF (YAC_Object *);
   YM void _mulRev_YAC_RVAL (YAC_Object *, YAC_Value *);
   YM void _mulRev_YAC_RARG (YAC_Object *, YAC_Object *) const;

   /* @method mulv,Vector2f v:Vector2f
      Multiply this instance by a 2dim vector
      @arg v vector to multiply
      @return resulting vector
    */
   YM void _mulv_YAC_RVAL (YAC_Object *_v, YAC_Value *_r);
   YM void _mulv_YAC_RARG (YAC_Object *_v, YAC_Object *_o) const;

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
      Normalize matrix using Hilbert-Schmidt norm
    */
   YM void _unit_YAC_RSELF (void);
   YM void _unit_YAC_RVAL  (YAC_Value *_r);
   YM void _unit_YAC_RARG  (YAC_Object *_r) const;

   /* @method unitScale,float s
      Normalise matrix using Hilbert-Schmidt norm and multiply by 's' afterwards
      @arg s scaling factor
    */
   YM void _unitScale_YAC_RSELF (sF32 _s);
   YM void _unitScale_YAC_RVAL  (sF32 _s, YAC_Value *_r);
   YM void _unitScale_YAC_RARG  (sF32 _s, YAC_Object *_r) const;

   /* @method det:float
      Calculate determinant of matrix
      @return value of determinant
    */
   YM sF32 _det (void) const;

   /* @method invert
      Calculate inversion of matrix, so that A*A<sup>-1</sup>=U
    */
   YM void _invert_YAC_RSELF (void);
   YM void _invert_YAC_RVAL  (YAC_Value *_r);
   YM void _invert_YAC_RARG  (YAC_Object *_r) const;

   /* @method transpose
      Transpose matrix
    */
   YM void _transpose_YAC_RSELF (void);
   YM void _transpose_YAC_RVAL  (YAC_Value *_r);
   YM void _transpose_YAC_RARG  (YAC_Object *_r) const;

   /* @method setRow,int row,Vector2f v
      Initialize row 'row' with values from 'v'
      @arg row row index
      @arg v Vector2f instance
    */
   YM void _setRow (sUI _row, YAC_Object *_v);

   /* @method getRow,int row:Vector2f
      Get row 'row' vector
      @arg row row index
      @return vector with values from row 'row'
    */
   YM void _getRow_YAC_RVAL (sUI _row, YAC_Value *_r);
   YM void _getRow_YAC_RARG (sUI _row, YAC_Object *_r) const;

   /* @method setCol,int col,Vector2f v
      Initialize column 'col' with values from 'v'
      @arg col column index
      @arg v Vector2f instance
    */
   YM void _setCol (sUI _col, YAC_Object *_v);

   /* @method getCol,int col:Vector2f
      Get column 'col' vector
      @arg col column index
      @return vector with values from column c
    */
   YM void _getCol_YAC_RVAL (sUI _col, YAC_Value *_r);
   YM void _getCol_YAC_RARG (sUI _col, YAC_Object *_o) const;

   /* @method initRowf,int row,float a,float b
      Initialize matrix row from scalars
      @arg row row index
      @arg a value for first column
      @arg b value for second column
    */
   YM void _initRowf (sUI _row, sF32 _a, sF32 _b);

   /* @method initRowV,int row,Vector2f v
      Initialize matrix row from vector or array-like object
      @arg r row index
      @arg v vector with values
    */
   YM void _initRowv (sUI _row, YAC_Object *_v);

   /* @method initColf,int col,float a,float b
      Initialize matrix column from scalars
      @arg col column index
      @arg a value for first row
      @arg b value for second row
    */
   YM void _initColf (sUI _col, sF32 _a, sF32 _b);

   /* @method initColv,int col,Vector2f v
      Initialise matrix column from vector or array-like object
      @arg col column index
      @arg v vector with values
    */
   YM void _initColv (sUI _col, YAC_Object *_v);

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

   /* @method _initRotatef
      @arg a rotation angle (0..2PI)
    */
   YM void _initRotatef (sF32 _a);

   /* @method scalef,float x,float y,float z
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
      Multiply by rotation matrix (angle is in radiant measure)
   */
   YM void _rotatef_YAC_RSELF (sF32 _ang);
   YM void _rotatef_YAC_RVAL  (sF32 _ang, YAC_Value *);
   YM void _rotatef_YAC_RARG  (sF32 _ang, YAC_Object *);

};

/* @function matrix2f,float va,float vb,float vc,float vd:Matrix2f
   Initialise and return new instance of Matrix2f
   @arg a value for a
   @arg b value for b
   @arg c value for c
   @arg d value for d
   @return new instance of Matrix2f
 */
YF void YAC_CALL _matrix2f (sF32 _a, sF32 _b, sF32 _c, sF32 _d, YAC_Value *_r);


#endif // MATRIX2F_H_
