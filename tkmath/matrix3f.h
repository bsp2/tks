/// matrix3f.h
///
/// (c) 2008-2025 by Carsten Busse <carsten.busse@googlemail.com>
///                  Bastian Spiegel <bs@tkscript.de> (additional coding)
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#ifndef MATRIX3F_H_
#define MATRIX3F_H_

YG("Math");

/* @class Matrix3f,Object
This class represents a single precision 3x3 square matrix.

The default layout is row-major.
 */
YC class _Matrix3f : public _Matrix {
  public:
   sF32 floats[9];

  public:
   _Matrix3f(void);
   ~_Matrix3f();
   YAC_POOLED(_Matrix3f,YAC_POOL_PRIORITY_HIGH);
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

   /* @method New,float a,float b,float c,float d,float e,float f,float g,float h,float i:Matrix3f
      Initialize and return new instance of Matrix3f. The parameters are in matrix order (row(default) or column major).
      @arg a value for a
      @arg b value for b
      @arg c value for c
      @arg d value for d
      @arg e value for e
      @arg f value for f
      @arg g value for g
      @arg h value for h
      @arg i value for i
      @return new instance of Matrix3f
    */
   YM void _New (sF32 _a, sF32 _b, sF32 _c, sF32 _d, sF32 _e, sF32 _f, sF32 _g, sF32 _h, sF32 _i, YAC_Value *_r);

   /* @method setA,float v
      Set a element to v
      @arg v new value for a
    */
   YM void _setA (sF32 _v);

   /* @method setB,float v
      Set b element to v
      @arg v new value for b
    */
   YM void _setB (sF32 _v);

   /* @method setC,float v
      Set c element to v
      @arg v new value for c
    */
   YM void _setC (sF32 _v);

   /* @method setD,float v
      Set d element to v
      @arg v new value for d
    */
   YM void _setD (sF32 _v);

   /* @method setE,float v
      Set e element to v
      @arg v new value for e
    */
   YM void _setE (sF32 _v);

   /* @method setF,float v
      Set f element to v
      @arg v new value for f
    */
   YM void _setF (sF32 _v);

   /* @method setG,float v
      Set g element to v
      @arg v new value for g
    */
   YM void _setG (sF32 _v);

   /* @method setH,float v
      Set h element to v
      @arg v new value for h
    */
   YM void _setH (sF32 _v);

   /* @method setI,float v
      Set i element to v
      @arg v new value for i
    */
   YM void _setI (sF32 _v);

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

   /* @method getG:float
      Get value of g element
      @return value of g element
    */
   YM sF32 _getG(void);

   /* @method getH:float
      Get value of h element
      @return value of h element
    */
   YM sF32 _getH(void);

   /* @method getI:float
      Get value of i element
      @return value of i element
    */
   YM sF32 _getI(void);

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

   /* @method isIdentity
      Check if this is an identity matrix (diagonal=1, other=0)
   */
   YM sBool _isIdentity (void);

   /* @method init,float a,float b,float c,float d,float e, float f, float g, float h, float i
      Initialize matrix. The parameters are in matrix order (row(default) or column major).
      @arg a value for m11
      @arg b value for m12
      @arg c value for m13
      @arg d value for m21
      @arg e value for m22
      @arg f value for m23
      @arg g value for m31
      @arg h value for m32
      @arg i value for m33
    */
   YM void _init (sF32 _a, sF32 _b, sF32 _c, sF32 _d, sF32 _e, sF32 _f, sF32 _g, sF32 _h, sF32 _i);

   /* @method initRowMajor,float a,float b,float c,float d,float e, float f, float g, float h, float i
      Initialize matrix (row-major arg order. swap args if matrix is in column-major order).
      @arg a value for m11
      @arg b value for m12
      @arg c value for m13
      @arg d value for m21
      @arg e value for m22
      @arg f value for m23
      @arg g value for m31
      @arg h value for m32
      @arg i value for m33
    */
   YM void _initRowMajor (sF32 _a, sF32 _b, sF32 _c, sF32 _d, sF32 _e, sF32 _f, sF32 _g, sF32 _h, sF32 _i);

   /* @method initColumnMajor,float a,float b,float c,float d,float e, float f, float g, float h, float i
      Initialize matrix (column-major arg order. swap args if matrix is in row-major order).
      @arg a value for m11
      @arg b value for m21
      @arg c value for m31
      @arg d value for m12
      @arg e value for m22
      @arg f value for m32
      @arg g value for m13
      @arg h value for m23
      @arg i value for m33
    */
   YM void _initColumnMajor (sF32 _a, sF32 _b, sF32 _c, sF32 _d, sF32 _e, sF32 _f, sF32 _g, sF32 _h, sF32 _i);

   /* @method initTranspose,float a,float b,float c,float d,float e, float f, float g, float h, float i
      Initialize matrix (swap arg row/column order).
      @arg a value for m11
      @arg b value for m12
      @arg c value for m13
      @arg d value for m21
      @arg e value for m22
      @arg f value for m23
      @arg g value for m31
      @arg h value for m32
      @arg i value for m33
    */
   YM void _initTranspose (sF32 _a, sF32 _b, sF32 _c, sF32 _d, sF32 _e, sF32 _f, sF32 _g, sF32 _h, sF32 _i);

   /* @method initRowf,int row,float a,float b,float c
      Initialize matrix row from scalars
      @arg row row number
      @arg a value for first column
      @arg b value for second column
      @arg c value for third column
    */
   YM void _initRowf (sUI _row, sF32 _a, sF32 _b, sF32 _c);

   /* @method initRowv,int row,Vector3f v
      Initialize matrix row from vector or array-like object
      @arg row row index
      @arg v vector with values
    */
   YM void _initRowv (sUI _row, YAC_Object *_v);

   /* @method initCol,int col,float a,float b,float c
      Initialize matrix column from scalars
      @arg col column index
      @arg a value for first element
      @arg b value for second element
      @arg c value for third element
    */
   YM void _initColf (sUI _col, sF32 _a, sF32 _b, sF32 _c);

   /* @method initColv,int col,Vector3f v
      Initialize matrix column from vector or array-like object
      @arg c column index
      @arg v vector with values
    */
   YM void _initColv (sUI _col, YAC_Object *_v);

   /* @method _initScalef
    */
   YM void _initScalef (sF32 _x, sF32 _y, sF32 _z);

   /* @method _initScalev,float x,float y
    */
   YM void _initScalev (YAC_Object *_scale);

   /* @method _initTranslatef,float x,float y
    */
   YM void _initTranslatef (sF32 _x, sF32 _y);

   /* @method _initTranslatev
    */
   YM void _initTranslatev (YAC_Object *_translate);

   /* @method _initRotatef,float x,float y
      same as initEulerXYZ()
    */
   YM void _initRotatef (sF32 _x, sF32 _y, sF32 _z);

   /* @method _initRotatev,Vector3f v
      same as initEulerXYZ(v.x, v.y, v.z)
    */
   YM void _initRotatev (YAC_Object *_v);

   /* @method initEulerXYX,float a,float b,float c
      Initialize with euler rotation matrix
      @arg a first rotation
      @arg b second rotation
      @arg c third rotation
    */
   YM void _initEulerXYX (sF32 _a, sF32 _b, sF32 _c);

   /* @method initEulerXZX,float a,float b,float c
      Initialize with euler rotation matrix
      @arg a first rotation
      @arg b second rotation
      @arg c third rotation
    */
   YM void _initEulerXZX (sF32 _a, sF32 _b, sF32 _c);

   /* @method initEulerYXY,float a,float b,float c
      Initialize with euler rotation matrix
      @arg a first rotation
      @arg b second rotation
      @arg c third rotation
    */
   YM void _initEulerYXY (sF32 _a, sF32 _b, sF32 _c);

   /* @method initEulerYZY,float a,float b,float c
      Initialize with euler rotation matrix
      @arg a first rotation
      @arg b second rotation
      @arg c third rotation
    */
   YM void _initEulerYZY (sF32 _a, sF32 _b, sF32 _c);

   /* @method initEulerZXZ,float a,float b,float c
      Initialize with euler rotation matrix
      @arg a first rotation
      @arg b second rotation
      @arg c third rotation
    */
   YM void _initEulerZXZ (sF32 _a, sF32 _b, sF32 _c);

   /* @method initEulerZYZ,float a,float b,float c
      Initialize with euler rotation matrix
      @arg a first rotation
      @arg b second rotation
      @arg c third rotation
    */
   YM void _initEulerZYZ (sF32 _a, sF32 _b, sF32 _c);

   /* @method initEulerXYZ,float a,float b,float c
      Initialize with euler rotation matrix
      @arg a first rotation
      @arg b second rotation
      @arg c third rotation
    */
   YM void _initEulerXYZ (sF32 _a, sF32 _b, sF32 _c);

   /* @method initEulerXZY,float a,float b,float c
      Initialize with euler rotation matrix
      @arg a first rotation
      @arg b second rotation
      @arg c third rotation
    */
   YM void _initEulerXZY (sF32 _a, sF32 _b, sF32 _c);

   /* @method initEulerYXZ,float a,float b,float c
      Initialize with euler rotation matrix
      @arg a first rotation
      @arg b second rotation
      @arg c third rotation
    */
   YM void _initEulerYXZ (sF32 _a, sF32 _b, sF32 _c);

   /* @method initEulerYZX,float a,float b,float c
      Initialize with euler rotation matrix
      @arg a first rotation
      @arg b second rotation
      @arg c third rotation
    */
   YM void _initEulerYZX (sF32 _a, sF32 _b, sF32 _c);

   /* @method initEulerZXY,float a,float b,float c
      Initialize with euler rotation matrix
      @arg a first rotation
      @arg b second rotation
      @arg c third rotation
    */
   YM void _initEulerZXY (sF32 _a, sF32 _b, sF32 _c);

   /* @method initEulerZYX,float a,float b,float c
      Initialize with euler rotation matrix
      @arg a first rotation
      @arg b second rotation
      @arg c third rotation
    */
   YM void _initEulerZYX (sF32 _a, sF32 _b, sF32 _c);

   /* @method add,Matrix3f m
      Add another matrix m to this instance
      @arg m matrix to add
    */
   YM void _add_YAC_RSELF (YAC_Object *_m);
   YM void _add_YAC_RVAL  (YAC_Object *_m, YAC_Value *_r);
   YM void _add_YAC_RARG  (YAC_Object *_m, YAC_Object *_r) const;

   /* @method sub,Matrix3f m
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

   /* @method mul,Matrix3f m
      Multiply this instance by matrix m
      @arg m matrix to multiply
    */
   YM void _mul_YAC_RSELF (YAC_Object *_m);
   YM void _mul_YAC_RVAL  (YAC_Object *_m, YAC_Value *_r);
   YM void _mul_YAC_RARG  (YAC_Object *_m, YAC_Object *_r) const;

   /* @method mulv,Vector3f v:Vector3f
      Multiply this instance by a 3dim vector
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

   /* @method scalef,float x,float y,float z
      Multiply by scale matrix
   */
   YM void _scalef_YAC_RSELF (sF32 _x, sF32 _y, sF32 _z);
   YM void _scalef_YAC_RVAL  (sF32 _x, sF32 _y, sF32 _z, YAC_Value *);
   YM void _scalef_YAC_RARG  (sF32 _x, sF32 _y, sF32 _z, YAC_Object *);

   /* @method scalev,Vector3f v
      Multiply by scale matrix
   */
   YM void _scalev_YAC_RSELF (YAC_Object *_v);
   YM void _scalev_YAC_RVAL  (YAC_Object *_v, YAC_Value *_r);
   YM void _scalev_YAC_RARG  (YAC_Object *_v, YAC_Object *_r);

   /* @method rotatef,float x,float y,float z
      Multiply by X=>Y=>Z rotation matrix (radian angles)
   */
   YM void _rotatef_YAC_RSELF (sF32 _x, sF32 _y, sF32 _z);
   YM void _rotatef_YAC_RVAL  (sF32 _x, sF32 _y, sF32 _z, YAC_Value *_r);
   YM void _rotatef_YAC_RARG  (sF32 _x, sF32 _y, sF32 _z, YAC_Object *_r);

   /* @method rotatev,Vector3f v
      Multiply by v.x=>v.y=>v.z rotation matrix (radian angles)
   */
   YM void _rotatev_YAC_RSELF (YAC_Object *_v);
   YM void _rotatev_YAC_RVAL  (YAC_Object *_v, YAC_Value *_r);
   YM void _rotatev_YAC_RARG  (YAC_Object *_v, YAC_Object *_r);
};

/* @function matrix3f,float a,float b,float c,float d,float e,float f,float g,float h,float i:Matrix3f
   Initialize and return new instance of Matrix3f. The parameters are in matrix order (row(default) or column major).
   @arg a value for a
   @arg b value for b
   @arg c value for c
   @arg d value for d
   @arg e value for e
   @arg f value for f
   @arg g value for g
   @arg h value for h
   @arg i value for i
   @return new instance of Matrix3f
 */
YF void YAC_CALL _matrix3f (sF32 _a, sF32 _b, sF32 _c, sF32 _d, sF32 _e, sF32 _f, sF32 _g, sF32 _h, sF32 _i, YAC_Value *_r);


#endif // MATRIX3F_H_
