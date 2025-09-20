/// matrix4f.h
///
/// (c) 2008-2025 by Carsten Busse <carsten.busse@googlemail.com>
///                  Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#ifndef MATRIX4F_H_
#define MATRIX4F_H_

YG("Math");

/* @class Matrix4f,Matrix
This class represents a single precision 4x4 square matrix.

The default layout is row-major.
 */
YC class _Matrix4f : public _Matrix {
  public:
   sF32 floats[16];

  public:
   _Matrix4f(void);
   ~_Matrix4f();

   YAC_POOLED(_Matrix4f, YAC_POOL_PRIORITY_HIGH);

   sUI   YAC_VCALL yacArrayGetNumElements      (void);
   sUI   YAC_VCALL yacArrayGetMaxElements      (void);
   sUI   YAC_VCALL yacArrayGetElementType      (void);
   sUI   YAC_VCALL yacArrayGetElementByteSize  (void);
   void  YAC_VCALL yacArraySet                 (YAC_ContextHandle, sUI, YAC_Value *);
   void  YAC_VCALL yacArrayGet                 (YAC_ContextHandle, sUI, YAC_Value *);
   void *YAC_VCALL yacArrayGetPointer          (void);
   sUI   YAC_VCALL yacArrayGetWidth            (void);
   sUI   YAC_VCALL yacArrayGetHeight           (void);
   sUI   YAC_VCALL yacArrayGetStride           (void);
   sSI   YAC_VCALL yacTensorRank               (void);
   void  YAC_VCALL yacSerialize                (YAC_Object *, sUI);
   sUI   YAC_VCALL yacDeserialize              (YAC_Object *, sUI);
   void  YAC_VCALL yacOperatorClamp            (YAC_Object *, YAC_Object *);
   void  YAC_VCALL yacOperatorWrap             (YAC_Object *, YAC_Object *);
   void  YAC_VCALL yacOperator                 (sSI, YAC_Object *, YAC_Value *);
   void  YAC_VCALL yacOperatorI                (sSI, sSI, YAC_Value *);
   void  YAC_VCALL yacOperatorF32              (sSI, sF32, YAC_Value *);
   sUI   YAC_VCALL yacOperatorPriority         (void) { return YAC_OP_PRIO_FLOAT; }
   sBool YAC_VCALL yacToString                 (YAC_String*) const;

   // Helper methods
   void assignFloats     (const sF32 *_fv, const sUI _ne);
   void assignFloatArray (YAC_FloatArray *_fa);
   void assignGeneric    (YAC_ContextHandle _ctx, YAC_Object *_o);
   void invertHelper     (_Matrix4f *t) const;

   //void _setColumnMajor (sSI _bEnabled);
   //sSI  _getColumnMajor (void);

   void mulv_helper (const _Vector4f *o, _Vector4f *r) const;
   void mulvrev_helper (const _Vector4f *o, _Vector4f *r) const;
   void rotate_helper (const sF32 _angle, const _Vector3f *_dir);

   /* @method New,Object o:Matrix4f
      Initialize from an array and return new instance of Matrix4f
      @arg o array with values
      @return new instance of Matrix4f
    */
   YM void _New (YAC_Object *, YAC_Value *);

   /* @method set,int row,int col,float v
      Set element at (row, column) to v
      @arg row Row number (0..3)
      @arg col Column number (0..3)
      @arg v new value for a
    */
   YM void _set (sUI _row, sUI _col, sF32 _v);

   /* @method getRC,int row,int col
      Get element at (row, column)
      @arg row Row number (0..3)
      @arg col Column number (0..3)
      @return Element value
    */
   YM sF32 _get (sUI _row, sUI _col);

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

   /* @method setE,float v
      Set e element to v
      @arg v new value for e
    */
   YM void _setE (sF32);

   /* @method setF,float v
      Set f element to v
      @arg v new value for f
    */
   YM void _setF (sF32);

   /* @method setG,float v
      Set g element to v
      @arg v new value for g
    */
   YM void _setG (sF32);

   /* @method setH,float v
      Set h element to v
      @arg v new value for h
    */
   YM void _setH (sF32);

   /* @method setI,float v
      Set i element to v
      @arg v new value for i
    */
   YM void _setI (sF32);

   /* @method setJ,float v
      Set j element to v
      @arg v new value for j
    */
   YM void _setJ (sF32);

   /* @method setK,float v
      Set k element to v
      @arg v new value for k
    */
   YM void _setK (sF32);

   /* @method setL,float v
      Set l element to v
      @arg v new value for l
    */
   YM void _setL (sF32);

   /* @method setM,float v
      Set m element to v
      @arg v new value for m
    */
   YM void _setM (sF32);

   /* @method setN,float v
      Set n element to v
      @arg v new value for n
    */
   YM void _setN (sF32);

   /* @method setO,float v
      Set o element to v
      @arg v new value for o
    */
   YM void _setO (sF32);

   /* @method setP,float v
      Set p element to v
      @arg v new value for p
    */
   YM void _setP (sF32);

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
   YM sF32 _getF (void);

   /* @method getG:float
      Get value of g element
      @return value of g element
    */
   YM sF32 _getG (void);

   /* @method getH:float
      Get value of h element
      @return value of h element
    */
   YM sF32 _getH (void);

   /* @method getI:float
      Get value of i element
      @return value of i element
    */
   YM sF32 _getI (void);

   /* @method getJ:float
      Get value of j element
      @return value of j element
    */
   YM sF32 _getJ (void);

   /* @method getK:float
      Get value of k element
      @return value of k element
    */
   YM sF32 _getK (void);

   /* @method getL:float
      Get value of l element
      @return value of l element
    */
   YM sF32 _getL (void);

   /* @method getM:float
      Get value of m element
      @return value of m element
    */
   YM sF32 _getM (void);


   /* @method getN:float
      Get value of n element
      @return value of n element
    */
   YM sF32 _getN (void);

   /* @method getO:float
      Get value of o element
      @return value of o element
    */
   YM sF32 _getO (void);

   /* @method getP:float
      Get value of p element
      @return value of p element
    */
   YM sF32 _getP (void);

   /* @method getString:String
      Get string representation of matrix ((a, b, c, d), (e, f, g, h), (i, j, k, l), (m, n, o, p))
      @return string representation of matrix
    */
   YM void _getString (YAC_Value *_r) const;

   /* @method setRow,int r,Vector4f v
      Set row r with values from v
      @arg r row number
      @arg v Vector4f instance
    */
   YM void _setRow (sUI _row, YAC_Object *_v);

   /* @method getRow,int r:Vector4f
      Get row r values
      @arg r row number
      @return vector with values from row r
    */
   YM void _getRow_YAC_RVAL (sUI _row, YAC_Value *_r);
   YM void _getRow_YAC_RARG (sUI _row, YAC_Object *_r) const;

   /* @method setCol,int c,Vector4f v
      Set column c with values from v
      @arg c column number
      @arg v Vector4f instance
    */
   YM void _setCol(sUI, YAC_Object*);

   /* @method getCol,int c:Vector4f
      Get column c values
      @arg c column number
      @return vector with values from column c
    */
   YM void _getCol_YAC_RVAL (sUI _col, YAC_Value *_r);
   YM void _getCol_YAC_RARG (sUI _col, YAC_Object *_r) const;

   /* @method init,float a,float b,float c,float d,float e,float f,float g,float h,float i,float j,float k,float l,float m,float n,float o,float p
      Initialize matrix (in native order, default is row-major)
      @arg a value for m11 (in row-major order)
      @arg b value for m12
      @arg c value for m13
      @arg d value for m14
      @arg e value for m21
      @arg f value for m22
      @arg g value for m23
      @arg h value for m24
      @arg i value for m31 (in row-major order, m13 in column-major order)
      @arg j value for m32
      @arg k value for m33
      @arg l value for m34
      @arg m value for m41
      @arg n value for m42
      @arg o value for m43
      @arg p value for m44
    */
   YM void _init (sF32 _a, sF32 _b, sF32 _c, sF32 _d, sF32 _e, sF32 _f, sF32 _g, sF32 _h, sF32 _i, sF32 _j, sF32 _k, sF32 _l, sF32 _m, sF32 _n, sF32 _o, sF32 _p);

   /* @method initRowMajor,float a,float b,float c,float d,float e,float f,float g,float h,float i,float j,float k,float l,float m,float n,float o,float p
      Initialize matrix (args are in row major order, convert to matrix order as required)
      @arg a value for m11
      @arg b value for m12
      @arg c value for m13
      @arg d value for m14
      @arg e value for m21
      @arg f value for m22
      @arg g value for m23
      @arg h value for m24
      @arg i value for m31
      @arg j value for m32
      @arg k value for m33
      @arg l value for m34
      @arg m value for m41
      @arg n value for m42
      @arg o value for m43
      @arg p value for m44
    */
   YM void _initRowMajor (sF32 _a, sF32 _b, sF32 _c, sF32 _d, sF32 _e, sF32 _f, sF32 _g, sF32 _h, sF32 _i, sF32 _j, sF32 _k, sF32 _l, sF32 _m, sF32 _n, sF32 _o, sF32 _p);

   /* @method initColumnMajor,float a,float b,float c,float d,float e,float f,float g,float h,float i,float j,float k,float l,float m,float n,float o,float p
      Initialize matrix (args are in column major (OpenGL) order, convert to matrix order as required)
      @arg a value for m11
      @arg b value for m21
      @arg c value for m31
      @arg d value for m41
      @arg e value for m12
      @arg f value for m22
      @arg g value for m32
      @arg h value for m42
      @arg i value for m13
      @arg j value for m23
      @arg k value for m33
      @arg l value for m43
      @arg m value for m14
      @arg n value for m24
      @arg o value for m34
      @arg p value for m44
    */
   YM void _initColumnMajor (sF32 _a, sF32 _b, sF32 _c, sF32 _d, sF32 _e, sF32 _f, sF32 _g, sF32 _h, sF32 _i, sF32 _j, sF32 _k, sF32 _l, sF32 _m, sF32 _n, sF32 _o, sF32 _p);

   /* @method initTranspose,float a,float b,float c,float d,float e,float f,float g,float h,float i,float j,float k,float l,float m,float n,float o,float p
      Initialize matrix (flip row/column major order, default is row-major)
      @arg a value for m11
      @arg b value for m12
      @arg c value for m13
      @arg d value for m14
      @arg e value for m21
      @arg f value for m22
      @arg g value for m23
      @arg h value for m24
      @arg i value for m31
      @arg j value for m32
      @arg k value for m33
      @arg l value for m34
      @arg m value for m41
      @arg n value for m42
      @arg o value for m43
      @arg p value for m44
    */
   YM void _initTranspose (sF32 _a, sF32 _b, sF32 _c, sF32 _d, sF32 _e, sF32 _f, sF32 _g, sF32 _h, sF32 _i, sF32 _j, sF32 _k, sF32 _l, sF32 _m, sF32 _n, sF32 _o, sF32 _p);

   /* @method initRowf,int row,float a,float b,float c,float d
      Initialize matrix row from scalars
      @arg row row index
      @arg a value for first column
      @arg b value for second column
      @arg c value for third column
      @arg d value for fourth column
    */
   YM void _initRowf (sUI _row, sF32 _a, sF32 _b, sF32 _c, sF32 _d);

   /* @method initRowv,int row,Vector4f v
      Initialize matrix row from vector or array-like object
      @arg row row number
      @arg v row vector
    */
   YM void _initRowv (sUI _row, YAC_Object *_v);

   /* @method initColf,int col,float a,float b,float c,float d
      Initialize matrix column from scalars
      @arg col column index
      @arg a value for first row
      @arg b value for second row
      @arg c value for third row
      @arg d value for fourth row
    */
   YM void _initColf (sUI _col, sF32 _a, sF32 _b, sF32 _c, sF32 _d);

   /* @method initColv,int c,Vector4f v
      Initialize matrix column from vector or array-like object
      @arg col column number
      @arg v vector with values
    */
   YM void _initColv (sUI _col, YAC_Object *_v);

   /* @method initEulerXYX,float a,float b,float c
      Initialize with euler rotation matrix
      @arg a first rotation (radian angles)
      @arg b second rotation
      @arg c third rotation
    */
   YM void _initEulerXYX (sF32, sF32, sF32);

   /* @method initEulerXZX,float a,float b,float c
      Initialize with euler rotation matrix
      @arg a first rotation
      @arg b second rotation
      @arg c third rotation
    */
   YM void _initEulerXZX (sF32, sF32, sF32);

   /* @method initEulerYXY,float a,float b,float c
      Initialize with euler rotation matrix
      @arg a first rotation
      @arg b second rotation
      @arg c third rotation
    */
   YM void _initEulerYXY (sF32, sF32, sF32);

   /* @method initEulerYZY,float a,float b,float c
      Initialize with euler rotation matrix
      @arg a first rotation
      @arg b second rotation
      @arg c third rotation
    */
   YM void _initEulerYZY (sF32, sF32, sF32);

   /* @method initEulerZXZ,float a,float b,float c
      Initialize with euler rotation matrix
      @arg a first rotation
      @arg b second rotation
      @arg c third rotation
    */
   YM void _initEulerZXZ (sF32, sF32, sF32);

   /* @method initEulerZYZ,float a,float b,float c
      Initialize with euler rotation matrix
      @arg a first rotation
      @arg b second rotation
      @arg c third rotation
    */
   YM void _initEulerZYZ (sF32, sF32, sF32);

   /* @method initEulerXYZ,float a,float b,float c
      Initialize with euler rotation matrix
      @arg a first rotation
      @arg b second rotation
      @arg c third rotation
    */
   YM void _initEulerXYZ (sF32, sF32, sF32);

   /* @method initEulerXZY,float a,float b,float c
      Initialize with euler rotation matrix
      @arg a first rotation
      @arg b second rotation
      @arg c third rotation
    */
   YM void _initEulerXZY (sF32, sF32, sF32);

   /* @method initEulerYXZ,float a,float b,float c
      Initialize with euler rotation matrix
      @arg a first rotation
      @arg b second rotation
      @arg c third rotation
    */
   YM void _initEulerYXZ (sF32, sF32, sF32);

   /* @method initEulerYZX,float a,float b,float c
      Initialize with euler rotation matrix
      @arg a first rotation
      @arg b second rotation
      @arg c third rotation
    */
   YM void _initEulerYZX (sF32, sF32, sF32);

   /* @method initEulerZXY,float a,float b,float c
      Initialize with euler rotation matrix
      @arg a first rotation
      @arg b second rotation
      @arg c third rotation
    */
   YM void _initEulerZXY (sF32, sF32, sF32);

   /* @method initEulerZYX,float a,float b,float c
      Initialize with euler rotation matrix
      @arg a first rotation
      @arg b second rotation
      @arg c third rotation
    */
   YM void _initEulerZYX (sF32, sF32, sF32);

   /* @method initIdentity
      Initialize with identity matrix.
   */
   YM void _initIdentity (void);

   /* @method isIdentity
      Check if this is an identity matrix (diagonal=1, other=0)
   */
   YM sBool _isIdentity (void);

   /* @method _initTranslatef
    */
   YM void _initTranslatef (sF32 _x, sF32 _y, sF32 _z);

   /* @method _initTranslatev
      @arg scale Vector2f, or Vector3f
    */
   YM void _initTranslatev (YAC_Object *_translate);

   /* @method _initScalef
    */
   YM void _initScalef (sF32 _x, sF32 _y, sF32 _z);

   /* @method _initScalev
      @arg scale Vector2f, Vector3f, or Vector4f
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

   /* @method initRotatef
      Init X=>Y=>Z rotation matrix (initEulerXYZ())
    */
   YM void _initRotatef (sF32 _x, sF32 _y, sF32 _z);

   /* @method initRotatev
      Init v.x=>v.y=>v.z rotation matrix (initEulerXYZ())
    */
   YM void _initRotatev (YAC_Object *_v);

   /* @method initRotate
      Init rotation matrix (rotate about axis vector)
    */
   YM void _initRotate (sF32 _angle, YAC_Object *_dir);

   /* @method initOrtho
    */
   YM void _initOrtho (sF32 _left, sF32 _right, sF32 _bottom, sF32 _top, sF32 _znear, sF32 _zfar);

   /* @method initFrustum
    */
   YM void _initFrustum (sF32 _left, sF32 _right, sF32 _bottom, sF32 _top, sF32 _znear, sF32 _zfar);

   /* @method initFrustumFov
     (field of view is in degrees)
    */
   YM void _initFrustumFov (sF32 _aspect, sF32 _fov, sF32 _znear, sF32 _zfar);

   /* @method initFrustumFovXY,float left, float right,float bottom,float top,float aspect,float fovX,float fovY,float znear,float zfar
    (field of view is in degrees)
    */
   YM void _initFrustumFovXY (sF32 _left, sF32 _right, sF32 _bottom, sF32 _top, sF32 _aspect, sF32 _fovX, sF32 _fovY, sF32 _znear, sF32 _zfar);

   /* @method initPerspective
      Init perspective matrix
    */
   YM void _initPerspective (sF32 _fov, sF32 _aspect, sF32 _znear, sF32 _zfar);

   /* @method initLookAt
      Init look-at matrix
    */
   YM void _initLookAt (YAC_Object *_eye, YAC_Object *_center, YAC_Object *_up);

   /* @method add,Matrix4f m
      Add another matrix m to this instance
      @arg m matrix to add
    */
   YM void _add_YAC_RSELF (YAC_Object *_m);
   YM void _add_YAC_RVAL  (YAC_Object *_m, YAC_Value *_r);
   YM void _add_YAC_RARG  (YAC_Object *_m, YAC_Object *_r) const;

   /* @method sub,Matrix4f m
      Substract another matrix m from this instance
      @arg m matrix to substract
    */
   YM void _sub_YAC_RSELF (YAC_Object *_m);
   YM void _sub_YAC_RVAL  (YAC_Object *_m, YAC_Value *_r);
   YM void _sub_YAC_RARG  (YAC_Object *_m, YAC_Object *_r) const;

   /* @method mulf,float s
      Multiply this instance with scalar value s
      @arg s value to multiply
    */
   YM void _mulf_YAC_RSELF (sF32 _s);
   YM void _mulf_YAC_RVAL  (sF32 _s, YAC_Value *_r);
   YM void _mulf_YAC_RARG  (sF32 _s, YAC_Object *_r) const;

   /* @method mul,Matrix4f m
      Multiply this instance with matrix m (this * m)
      @arg m matrix to multiply
    */
   YM void _mul_YAC_RSELF (YAC_Object *_m);
   YM void _mul_YAC_RVAL  (YAC_Object *_m, YAC_Value *_r);
   YM void _mul_YAC_RARG  (YAC_Object *_m, YAC_Object *_r);

   /* @method mulRev,Matrix4f m
      Reverse-multiply matrix m by this instance (m * this)
      @arg m matrix to multiply
    */
   YM void _mulRev_YAC_RSELF (YAC_Object *_m);
   YM void _mulRev_YAC_RVAL  (YAC_Object *_m, YAC_Value *_r);
   YM void _mulRev_YAC_RARG  (YAC_Object *_m, YAC_Object *_r);

   /* @method mulv,Vector4f v:Vector4f
      Multiply this instance with a 4dim vector
      @arg v vector to multiply
      @return resulting vector
    */
   YM void _mulv_YAC_RVAL (YAC_Object *_v, YAC_Value *_r) const;
   YM void _mulv_YAC_RARG (YAC_Object *_v, YAC_Object *_r) const;

   /* @method mulvRev,Vector4f v:Vector4f
      Multiply this transposed instance with a 4dim vector
      @arg v vector to multiply
      @return resulting vector
    */
   YM void _mulvRev_YAC_RVAL (YAC_Object *_v, YAC_Value *_r) const;
   YM void _mulvRev_YAC_RARG (YAC_Object *_v, YAC_Object *_r) const;

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
   YM void _unit_YAC_RSELF (void); /* unit */
   YM void _unit_YAC_RVAL  (YAC_Value *_r);
   YM void _unit_YAC_RARG  (YAC_Object *_r) const;

   /* @method unitScale,float v
      Normalise matrix with Hilbert-Schmidt norm and scale by v afterwards
      @arg v value to scale matrix
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

//   YM void _invert2_YAC_RSELF(void);
//   YM void _invert2_YAC_RVAL(YAC_Value *);
//   YM void _invert2_YAC_RARG(YAC_Object *);

   /* @method transpose
      Transpose matrix
    */
   YM void _transpose_YAC_RSELF (void);
   YM void _transpose_YAC_RVAL  (YAC_Value *_r);
   YM void _transpose_YAC_RARG  (YAC_Object *_r) const;

   /* @method getQuaternionf:Quaternionf
      Tries to find a Quaternion representation of the matrix if possible
      @return Quaternion representation of matrix (or zero quaternion if not possible)
    */
   YM void _getQuaternionf_YAC_RVAL (YAC_Value*);
   YM void _getQuaternionf_YAC_RARG (YAC_Object*) const;

   /* @method BuildPerspectiveMatrix4f,float fov,float aspect,float zNear,float zFar:Matrix4f
      Builds a perspective matrix
      @arg fov field of view in degrees
      @arg aspect aspect ratio
      @arg zNear zNear value
      @arg zFar zFar value
      @return generated matrix
    */
   YM void _BuildPerspectiveMatrix4f_YAC_RVAL (sF32 _fov, sF32 _aspect, sF32 _znear, sF32 _zfar, YAC_Value *_r);
   YM void _BuildPerspectiveMatrix4f_YAC_RARG (sF32 _fov, sF32 _aspect, sF32 _znear, sF32 _zfar, YAC_Object *_r) const;

   /* @method BuildLookAtMatrix4f,Vector3f eye,Vector3f center,Vector3f up:Matrix4f
      Build a look at matrix
      @arg eye eye direction vector
      @arg center center of camera vector
      @arg up vector for up direction
      @return generated matrix
    */
   YM void _BuildLookAtMatrix4f_YAC_RVAL (YAC_Object *_eye, YAC_Object *_center, YAC_Object *_up, YAC_Value *_r);
   YM void _BuildLookAtMatrix4f_YAC_RARG (YAC_Object *_eye, YAC_Object *_center, YAC_Object *_up, YAC_Object *_r) const;

   /* @method BuildRotateMatrix4f,float angle,Vector3f axis:Matrix4f
      Build a rotation matrix
      @arg angle rotation angle
      @arg axis rotation axis vector
      @return generated matrix
    */
   YM void _BuildRotateMatrix4f_YAC_RVAL (sF32 _angle, YAC_Object *_axis, YAC_Value *_r);
   YM void _BuildRotateMatrix4f_YAC_RARG (sF32 _angle, YAC_Object *_axis, YAC_Object *_r) const;

   /* @method BuildTranslateMatrix4f,Vector3f t:Matrix4f
      Build a translation matrix
      @arg t translation vector
      @return generated matrix
    */
   YM void _BuildTranslateMatrix4f_YAC_RVAL (YAC_Object *_t, YAC_Value *_r);
   YM void _BuildTranslateMatrix4f_YAC_RARG (YAC_Object *_t, YAC_Object *_r) const;

   /* @method BuildScaleMatrix4f,Vector3f scale:Matrix4f
      Build a scale matrix
      @arg scale vector
      @return generated matrix
    */
   YM void _BuildScaleMatrix4f_YAC_RVAL (YAC_Object *_scale, YAC_Value *_r);
   YM void _BuildScaleMatrix4f_YAC_RARG (YAC_Object *_scale, YAC_Object *_r) const;

   /* @method BuildEulerXZX4f,float a,float b,float c:Matrix4f
      Build an euler rotation matrix with XZX convention
      @arg a first angle in radians
      @arg b second angle in radians
      @arg c third angle in radians
      @return generated matrix
    */
   YM void _BuildEulerXZX4f_YAC_RVAL (sF32 _a, sF32 _b, sF32 _c, YAC_Value *_r);
   YM void _BuildEulerXZX4f_YAC_RARG (sF32 _a, sF32 _b, sF32 _c, YAC_Object *_r) const;

   /* @method BuildEulerXYX4f,float a,float b,float c:Matrix4f
      Build an euler rotation matrix with XYX convention
      @arg a first angle in radians
      @arg b second angle in radians
      @arg c third angle in radians
      @return generated matrix
    */
   YM void _BuildEulerXYX4f_YAC_RVAL(sF32 _a, sF32 _b, sF32 _c, YAC_Value *_r);
   YM void _BuildEulerXYX4f_YAC_RARG(sF32 _a, sF32 _b, sF32 _c, YAC_Object *_r) const;

   /* @method BuildEulerYXY4f,float a,float b,float c:Matrix4f
      Build an euler rotation matrix with YXY convention
      @arg a first angle in radians
      @arg b second angle in radians
      @arg c third angle in radians
      @return generated matrix
    */
   YM void _BuildEulerYXY4f_YAC_RVAL(sF32 _a, sF32 _b, sF32 _c, YAC_Value *_r);
   YM void _BuildEulerYXY4f_YAC_RARG(sF32 _a, sF32 _b, sF32 _c, YAC_Object *_r) const;

   /* @method BuildEulerYZY4f,float a,float b,float c:Matrix4f
      Build an euler rotation matrix with YZY convention
      @arg a first angle in radians
      @arg b second angle in radians
      @arg c third angle in radians
      @return generated matrix
    */
   YM void _BuildEulerYZY4f_YAC_RVAL(sF32 _a, sF32 _b, sF32 _c, YAC_Value *_r);
   YM void _BuildEulerYZY4f_YAC_RARG(sF32 _a, sF32 _b, sF32 _c, YAC_Object *_r) const;

   /* @method BuildEulerZYZ4f,float a,float b,float c:Matrix4f
      Build an euler rotation matrix with ZYZ convention
      @arg a first angle in radians
      @arg b second angle in radians
      @arg c third angle in radians
      @return generated matrix
    */
   YM void _BuildEulerZYZ4f_YAC_RVAL(sF32 _a, sF32 _b, sF32 _c, YAC_Value *_r);
   YM void _BuildEulerZYZ4f_YAC_RARG(sF32 _a, sF32 _b, sF32 _c, YAC_Object *_r) const;

   /* @method BuildEulerZXZ4f,float a,float b,float c:Matrix4f
      Build an euler rotation matrix with ZXZ convention
      @arg a first angle in radians
      @arg b second angle in radians
      @arg c third angle in radians
      @return generated matrix
    */
   YM void _BuildEulerZXZ4f_YAC_RVAL(sF32 _a, sF32 _b, sF32 _c, YAC_Value *_r);
   YM void _BuildEulerZXZ4f_YAC_RARG(sF32 _a, sF32 _b, sF32 _c, YAC_Object *_r) const;

   /* @method BuildEulerXZY4f,float a,float b,float c:Matrix4f
      Build an euler rotation matrix with XZY convention
      @arg a first angle in radians
      @arg b second angle in radians
      @arg c third angle in radians
      @return generated matrix
    */
   YM void _BuildEulerXZY4f_YAC_RVAL(sF32 _a, sF32 _b, sF32 _c, YAC_Value *_r);
   YM void _BuildEulerXZY4f_YAC_RARG(sF32 _a, sF32 _b, sF32 _c, YAC_Object *_r) const;

   /* @method BuildEulerXYZ4f,float a,float b,float c:Matrix4f
      Build an euler rotation matrix with XYZ convention
      @arg a first angle in radians
      @arg b second angle in radians
      @arg c third angle in radians
      @return generated matrix
    */
   YM void _BuildEulerXYZ4f_YAC_RVAL(sF32 _a, sF32 _b, sF32 _c, YAC_Value *_r);
   YM void _BuildEulerXYZ4f_YAC_RARG(sF32 _a, sF32 _b, sF32 _c, YAC_Object *_r) const;

   /* @method BuildEulerYXZ4f,float a,float b,float c:Matrix4f
      Build an euler rotation matrix with YXZ convention
      @arg a first angle in radians
      @arg b second angle in radians
      @arg c third angle in radians
      @return generated matrix
    */
   YM void _BuildEulerYXZ4f_YAC_RVAL(sF32 _a, sF32 _b, sF32 _c, YAC_Value *_r);
   YM void _BuildEulerYXZ4f_YAC_RARG(sF32 _a, sF32 _b, sF32 _c, YAC_Object *_r) const;

   /* @method BuildEulerYZX4f,float a,float b,float c:Matrix4f
      Build an euler rotation matrix with YZX convention
      @arg a first angle in radians
      @arg b second angle in radians
      @arg c third angle in radians
      @return generated matrix
    */
   YM void _BuildEulerYZX4f_YAC_RVAL(sF32 _a, sF32 _b, sF32 _c, YAC_Value *_r);
   YM void _BuildEulerYZX4f_YAC_RARG(sF32 _a, sF32 _b, sF32 _c, YAC_Object *_r) const;

   /* @method BuildEulerZYX4f,float a,float b,float c:Matrix4f
      Build an euler rotation matrix with ZYX convention
      @arg a first angle in radians
      @arg b second angle in radians
      @arg c third angle in radians
      @return generated matrix
    */
   YM void _BuildEulerZYX4f_YAC_RVAL(sF32 _a, sF32 _b, sF32 _c, YAC_Value *_r);
   YM void _BuildEulerZYX4f_YAC_RARG(sF32 _a, sF32 _b, sF32 _c, YAC_Object *_r) const;

   /* @method BuildEulerZXY4f,float a,float b,float c:Matrix4f
      Build an euler rotation matrix with ZXY convention
      @arg a first angle in radians
      @arg b second angle in radians
      @arg c third angle in radians
      @return generated matrix
    */
   YM void _BuildEulerZXY4f_YAC_RVAL(sF32 _a, sF32 _b, sF32 _c, YAC_Value *_r);
   YM void _BuildEulerZXY4f_YAC_RARG(sF32 _a, sF32 _b, sF32 _c, YAC_Object *_r) const;

   /* @method translatef,float x,float y,float z
      Multiply by translation matrix
   */
   YM void _translatef_YAC_RSELF (sF32 _x, sF32 _y, sF32 _z);
   YM void _translatef_YAC_RVAL  (sF32 _x, sF32 _y, sF32 _z, YAC_Value *_r);
   YM void _translatef_YAC_RARG  (sF32 _x, sF32 _y, sF32 _z, YAC_Object *_r);

   /* @method translatev,Vector3f v
      Multiply by translation matrix
   */
   YM void _translatev_YAC_RSELF (YAC_Object *_v);
   YM void _translatev_YAC_RVAL  (YAC_Object *_v, YAC_Value *_r);
   YM void _translatev_YAC_RARG  (YAC_Object *_v, YAC_Object *_r);

   /* @method scalef,float x,float y,float z
      Multiply by scale matrix
   */
   YM void _scalef_YAC_RSELF (sF32 _x, sF32 _y, sF32 _z);
   YM void _scalef_YAC_RVAL  (sF32 _x, sF32 _y, sF32 _z, YAC_Value *_r);
   YM void _scalef_YAC_RARG  (sF32 _x, sF32 _y, sF32 _z, YAC_Object *_r);

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

   /* @method rotate,float angle,Object axis
      Multiply by rotation matrix
   */
   YM void _rotate_YAC_RSELF (sF32 _angle, YAC_Object *_dir);
   YM void _rotate_YAC_RVAL  (sF32 _angle, YAC_Object *_dir, YAC_Value *_r);
   YM void _rotate_YAC_RARG  (sF32 _angle, YAC_Object *_dir, YAC_Object *_r);

   /* @method ortho,float left,float right,float bottom,float top,float znear,float zfar
      Multiply by orthogonal matrix
   */
   YM void _ortho_YAC_RSELF (sF32 _left, sF32 _right, sF32 _bottom, sF32 _top, sF32 _znear, sF32 _zfar);
   YM void _ortho_YAC_RVAL  (sF32 _left, sF32 _right, sF32 _bottom, sF32 _top, sF32 _znear, sF32 _zfar, YAC_Value *_r);
   YM void _ortho_YAC_RARG  (sF32 _left, sF32 _right, sF32 _bottom, sF32 _top, sF32 _znear, sF32 _zfar, YAC_Object *_r);

   /* @method frustum,float left,float right,float bottom,float top,float znear,float zfar
      Multiply by frustum matrix
   */
   YM void _frustum_YAC_RSELF (sF32 _left, sF32 _right, sF32 _bottom, sF32 _top, sF32 _znear, sF32 _zfar);
   YM void _frustum_YAC_RVAL  (sF32 _left, sF32 _right, sF32 _bottom, sF32 _top, sF32 _znear, sF32 _zfar, YAC_Value *_r);
   YM void _frustum_YAC_RARG  (sF32 _left, sF32 _right, sF32 _bottom, sF32 _top, sF32 _znear, sF32 _zfar, YAC_Object *_r);

   /* @method frustumFovXY,float left,float right,float bottom,float top,float aspect,float fovX,float fovY,float znear,float zfar
      Multiply by frustum matrix (fields of view are in degrees)
   */
   YM void _frustumFovXY_YAC_RSELF (sF32 _left, sF32 _right, sF32 _bottom, sF32 _top, sF32 _aspect, sF32 _fovX, sF32 _fovY, sF32 _znear, sF32 _zfar);
   YM void _frustumFovXY_YAC_RVAL  (sF32 _left, sF32 _right, sF32 _bottom, sF32 _top, sF32 _aspect, sF32 _fovX, sF32 _fovY, sF32 _znear, sF32 _zfar, YAC_Value *_r);
   YM void _frustumFovXY_YAC_RARG  (sF32 _left, sF32 _right, sF32 _bottom, sF32 _top, sF32 _aspect, sF32 _fovX, sF32 _fovY, sF32 _znear, sF32 _zfar, YAC_Object *_r);

   /* @method perspective,float fov,float aspect,float znear,float zfar
      Multiply by perspective matrix
   */
   YM void _perspective_YAC_RSELF (sF32 _fov, sF32 _aspect, sF32 _znear, sF32 _zfar);
   YM void _perspective_YAC_RVAL  (sF32 _fov, sF32 _aspect, sF32 _znear, sF32 _zfar, YAC_Value *_r);
   YM void _perspective_YAC_RARG  (sF32 _fov, sF32 _aspect, sF32 _znear, sF32 _zfar, YAC_Object *_r);

   /* @method lookAt,Object eye,Object center,Object up
      Multiply by look-at matrix
   */
   YM void _lookAt_YAC_RSELF (YAC_Object *_eye, YAC_Object *_center, YAC_Object *_up);
   YM void _lookAt_YAC_RVAL  (YAC_Object *_eye, YAC_Object *_center, YAC_Object *_up, YAC_Value *_r);
   YM void _lookAt_YAC_RARG  (YAC_Object *_eye, YAC_Object *_center, YAC_Object *_up, YAC_Object *_r);

   /* @method extractPlanes,Vector4f retN,Vector4f retF,Vector4f retL,Vector4f retR,Vector4f retT,Vector4f retB
      Extract normalized planes from projection matrix (e.g. for view-frustum culling)

      @arg retN returns Vector4f near plane
      @arg retF returns Vector4f far plane
      @arg retL returns Vector4f left plane
      @arg retR returns Vector4f right plane
      @arg retT returns Vector4f top plane
      @arg retB returns Vector4f bottom plane
   */
   YM void _extractPlanes (YAC_Object *_retN, YAC_Object *_retF, YAC_Object *_retL, YAC_Object *_retR, YAC_Object *_retT, YAC_Object *_retB) const;
};


/* @function matrix4f,Object a:Matrix4f
   Initialize from an array and return new instance of Matrix4f
   @arg a array with values
   @return new instance of Matrix4f
*/
YF void YAC_CALL _matrix4f(YAC_Object *_a, YAC_Value *_r);


#endif // MATRIX4F_H_
