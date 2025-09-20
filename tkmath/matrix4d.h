/// matrix4d.h
///
/// (c) 2008-2024 by Carsten Busse <carsten.busse@googlemail.com>
///             Bastian Spiegel <bs@tkscript.de> (additional coding)
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#ifndef MATRIX4D_H_
#define MATRIX4D_H_


YG("Math");


/* @class Matrix4d,Object
This class represents a double precision 4x4 square matrix.
 */
YC class _Matrix4d : public YAC_Object {
  public:
   sF64 doubles[16];

  public:
   _Matrix4d(void);
   ~_Matrix4d();
   YAC_POOLED(_Matrix4d,YAC_POOL_PRIORITY_HIGH);

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
   void  YAC_VCALL yacOperatorF64             (sSI, sF64, YAC_Value *);
   sUI   YAC_VCALL yacOperatorPriority        (void) { return YAC_OP_PRIO_DOUBLE; }
   sBool YAC_VCALL yacToString                (YAC_String*) const;

   // Helper methods
   void assignFloats (sF32 *_fv, sUI _ne);
   void assignFloatArray (YAC_FloatArray *_fa);
   void assignGeneric (YAC_ContextHandle _ctx, YAC_Object *_o);

   /* @method setA,Double v
      Set a element to v
      @arg v new value for a
    */
   YM void _setA (YAC_Object *_v);

   /* @method setB,Double v
      Set b element to v
      @arg v new value for b
    */
   YM void _setB (YAC_Object *_v);

   /* @method setC,Double v
      Set c element to v
      @arg v new value for c
    */
   YM void _setC (YAC_Object *_v);

   /* @method setD,Double v
      Set d element to v
      @arg v new value for d
    */
   YM void _setD (YAC_Object *_v);

   /* @method setE,Double v
      Set e element to v
      @arg v new value for e
    */
   YM void _setE (YAC_Object *_v);

   /* @method setF,Double v
      Set f element to v
      @arg v new value for f
    */
   YM void _setF (YAC_Object *_v);

   /* @method setG,Double v
      Set g element to v
      @arg v new value for g
    */
   YM void _setG (YAC_Object *_v);

   /* @method setH,Double v
      Set h element to v
      @arg v new value for h
    */
   YM void _setH (YAC_Object *_v);

   /* @method setI,Double v
      Set i element to v
      @arg v new value for i
    */
   YM void _setI (YAC_Object *_v);

   /* @method setJ,Double v
      Set j element to v
      @arg v new value for j
    */
   YM void _setJ (YAC_Object *_v);

   /* @method setK,Double v
      Set k element to v
      @arg v new value for k
    */
   YM void _setK (YAC_Object *_v);

   /* @method setL,Double v
      Set l element to v
      @arg v new value for l
    */
   YM void _setL (YAC_Object *_v);

   /* @method setM,Double v
      Set m element to v
      @arg v new value for m
    */
   YM void _setM (YAC_Object *_v);

   /* @method setN,Double v
      Set n element to v
      @arg v new value for n
    */
   YM void _setN (YAC_Object *_v);

   /* @method setO,Double v
      Set o element to v
      @arg v new value for o
    */
   YM void _setO (YAC_Object *_v);

   /* @method setP,Double v
      Set p element to v
      @arg v new value for p
    */
   YM void _setP (YAC_Object *_v);

   /* @method getA:Double
      Get value of a element
      @return value of a element
    */
   YM void _getA_YAC_RVAL (YAC_Value *_r);
   YM void _getA_YAC_RARG (YAC_Object *_r);

   /* @method getB:Double
      Get value of b element
      @return value of b element
    */
   YM void _getB_YAC_RVAL (YAC_Value *_r);
   YM void _getB_YAC_RARG (YAC_Object *_r);

   /* @method getC:Double
      Get value of c element
      @return value of c element
    */
   YM void _getC_YAC_RVAL (YAC_Value *_r);
   YM void _getC_YAC_RARG (YAC_Object *_r);

   /* @method getD:Double
      Get value of d element
      @return value of d element
    */
   YM void _getD_YAC_RVAL (YAC_Value *_r);
   YM void _getD_YAC_RARG (YAC_Object *_r);

   /* @method getE:Double
      Get value of e element
      @return value of e element
    */
   YM void _getE_YAC_RVAL (YAC_Value *_r);
   YM void _getE_YAC_RARG (YAC_Object *_r);

   /* @method getF:Double
      Get value of f element
      @return value of f element
    */
   YM void _getF_YAC_RVAL (YAC_Value *_r);
   YM void _getF_YAC_RARG (YAC_Object *_r);

   /* @method getG:Double
      Get value of g element
      @return value of g element
    */
   YM void _getG_YAC_RVAL (YAC_Value *_r);
   YM void _getG_YAC_RARG (YAC_Object *_r);

   /* @method getH:Double
      Get value of h element
      @return value of h element
    */
   YM void _getH_YAC_RVAL (YAC_Value *_r);
   YM void _getH_YAC_RARG (YAC_Object *_r);

   /* @method getI:Double
      Get value of i element
      @return value of i element
    */
   YM void _getI_YAC_RVAL (YAC_Value *_r);
   YM void _getI_YAC_RARG (YAC_Object *_r);

   /* @method getJ:Double
      Get value of j element
      @return value of j element
    */
   YM void _getJ_YAC_RVAL (YAC_Value *_r);
   YM void _getJ_YAC_RARG (YAC_Object *_r);

   /* @method getK:Double
      Get value of k element
      @return value of k element
    */
   YM void _getK_YAC_RVAL (YAC_Value *_r);
   YM void _getK_YAC_RARG (YAC_Object *_r);

   /* @method getL:Double
      Get value of l element
      @return value of l element
    */
   YM void _getL_YAC_RVAL (YAC_Value *_r);
   YM void _getL_YAC_RARG (YAC_Object *_r);

   /* @method getM:Double
      Get value of m element
      @return value of m element
    */
   YM void _getM_YAC_RVAL (YAC_Value *_r);
   YM void _getM_YAC_RARG (YAC_Object *_r);

   /* @method getN:Double
      Get value of n element
      @return value of n element
    */
   YM void _getN_YAC_RVAL (YAC_Value *_r);
   YM void _getN_YAC_RARG (YAC_Object *_r);

   /* @method getO:Double
      Get value of o element
      @return value of o element
    */
   YM void _getO_YAC_RVAL (YAC_Value *_r);
   YM void _getO_YAC_RARG (YAC_Object *_r);

   /* @method getP:Double
      Get value of p element
      @return value of p element
    */
   YM void _getP_YAC_RVAL (YAC_Value *_r);
   YM void _getP_YAC_RARG (YAC_Object *_r);

   /* @method getString:String
      Get string representation of matrix ((a, b, c, d), (e, f, g, h), (i, j, k, l), (m, n, o, p))
      @return string representation of matrix
    */
   YM void _getString (YAC_Value *_r);

   /* @method add,Matrix4d m
      Add another matrix m to this instance
      @arg m matrix to add
    */
   YM void _add_YAC_RSELF (YAC_Object *_m);
   YM void _add_YAC_RVAL  (YAC_Object *_m, YAC_Value *_r);
   YM void _add_YAC_RARG  (YAC_Object *_m, YAC_Object *_r);

   /* @method sub,Matrix4d m
      Substract another matrix m from this instance
      @arg m matrix to substract
    */
   YM void _sub_YAC_RSELF (YAC_Object *_m);
   YM void _sub_YAC_RVAL  (YAC_Object *_m, YAC_Value *_r);
   YM void _sub_YAC_RARG  (YAC_Object *_m, YAC_Object *_r);

   /* @method mulf,Double v
      Multiply this instance with scalar value v
      @arg v value to multiply
    */
   void muld (const sF64 s);
   YM void _muld_YAC_RSELF (YAC_Object *_v);
   YM void _muld_YAC_RVAL  (YAC_Object *_v, YAC_Value *_r);
   YM void _muld_YAC_RARG  (YAC_Object *_v, YAC_Object *_r);

   /* @method mul,Matrix4d m
      Multiply this instance by matrix m
      @arg m matrix to multiply
    */
   YM void _mul_YAC_RSELF (YAC_Object *_m);
   YM void _mul_YAC_RVAL  (YAC_Object *_m, YAC_Value *_r);
   YM void _mul_YAC_RARG  (YAC_Object *_m, YAC_Object *_r);

   /* @method mulRev,Matrix4d m
      Reverse-multiply matrix m by this instance (m * this)
      @arg m matrix to multiply
    */
   YM void _mulRev_YAC_RSELF(YAC_Object *);
   YM void _mulRev_YAC_RVAL(YAC_Object *, YAC_Value *);
   YM void _mulRev_YAC_RARG(YAC_Object *, YAC_Object *) const;

   /* @method mulv,Vector4d v:Vector4d
      Multiply this instance with a 4dim vector
      @arg v vector to multiply
      @return resulting vector
    */
   YM void _mulv_YAC_RVAL (YAC_Object *_v, YAC_Value *_r);
   YM void _mulv_YAC_RARG (YAC_Object *_v, YAC_Object *_r);

   /* @method getAbs:Double
      Calculate the Hilbert-Schmidt norm of the matrix
      @return scalar matrix norm value
    */
   sF64 getAbs (void);
   YM void _getAbs_YAC_RVAL (YAC_Value *_r);
   YM void _getAbs_YAC_RARG (YAC_Object *_r);

   /* @method getAbsSqr:Double
      Calculate the squared Hilbert-Schmidt norm of the matrix
      @return scalar matrix norm value
    */
   sF64 getAbsSqr (void);
   YM void _getAbsSqr_YAC_RVAL (YAC_Value *_r);
   YM void _getAbsSqr_YAC_RARG (YAC_Object *_r);

   /* @method unit
      Normalise matrix using Hilbert-Schmidt norm
    */
   YM void _unit_YAC_RSELF (void);
   YM void _unit_YAC_RVAL  (YAC_Value *_r);
   YM void _unit_YAC_RARG  (YAC_Object *_r);

   /* @method unitScale,Double s
      Normalise matrix using Hilbert-Schmidt norm and multiply by s afterwards
      @arg s scaling factor
    */
   void unitScale (sF64 _s);
   YM void _unitScale_YAC_RSELF (YAC_Object *_s);
   YM void _unitScale_YAC_RVAL  (YAC_Object *_s, YAC_Value *_r);
   YM void _unitScale_YAC_RARG  (YAC_Object *_s, YAC_Object *_r);

   /* @method det:Double
      Calculate determinant of matrix
      @return value of determinant
    */
   sF64 det (void);
   void _det_YAC_RVAL (YAC_Value *_r);
   void _det_YAC_RARG (YAC_Object *_r);

   /* @method invert
      Calculate inversion of matrix, so that A*A<sup>-1</sup>=U
    */
   YM void _invert_YAC_RSELF (void);
   YM void _invert_YAC_RVAL  (YAC_Value *_r);
   YM void _invert_YAC_RARG  (YAC_Object *_r);

//   YM void _invert2_YAC_RSELF(void);
//   YM void _invert2_YAC_RVAL(YAC_Value *);
//   YM void _invert2_YAC_RARG(YAC_Object *);

   /* @method transpose
      Transpose matrix
    */
   YM void _transpose_YAC_RSELF (void);
   YM void _transpose_YAC_RVAL  (YAC_Value *_r);
   YM void _transpose_YAC_RARG  (YAC_Object *_r);

   /* @method setRow,int r,Vector4d v
      Set row r with values from v
      @arg r row index
      @arg v Vector4d instance
    */
   YM void _setRow (sUI _row, YAC_Object *_v);

   /* @method getRow,int row:Vector4d
      Get row 'row'
      @arg row row index
      @return vector with values from row r
    */
   YM void _getRow_YAC_RVAL (sUI _row, YAC_Value *_r);
   YM void _getRow_YAC_RARG (sUI _row, YAC_Object *_r);

   /* @method setCol,int col,Vector4d v
      Set column 'col' with values from v
      @arg col column index
      @arg v Vector4d instance
    */
   YM void _setCol (sUI _col, YAC_Object *_v);

   /* @method getCol,int col:Vector4d
      Get column 'col' values
      @arg col column index
      @return vector with values from column c
    */
   YM void _getCol_YAC_RVAL (sUI _col, YAC_Value *_r);
   YM void _getCol_YAC_RARG (sUI _col, YAC_Object *_r);

   /* @method initf,float a,float b,float c,float d,float e,float f,float g,float h,float i,float j,float k,float l,float m,float n,float o,float p
      Initialize matrix
      @arg a value for a
      @arg b value for b
      @arg c value for c
      @arg d value for d
      @arg e value for e
      @arg f value for f
      @arg g value for g
      @arg h value for h
      @arg i value for i
      @arg j value for j
      @arg k value for k
      @arg l value for l
      @arg m value for m
      @arg n value for n
      @arg o value for o
      @arg p value for p
    */
   YM void _initf (sF32 _a, sF32 _b, sF32 _c, sF32 _d, sF32 _e, sF32 _f, sF32 _g, sF32 _h, sF32 _i, sF32 _j, sF32 _k, sF32 _l, sF32 _m, sF32 _n, sF32 _o, sF32 _p);

   /* @method init,Double a,Double b,Double c,Double d,Double e,Double f,Double g,Double h,Double i,Double j,Double k,Double l,Double m,Double n,Double o,Double p
      Initialize matrix
      @arg a value for a
      @arg b value for b
      @arg c value for c
      @arg d value for d
      @arg e value for e
      @arg f value for f
      @arg g value for g
      @arg h value for h
      @arg i value for i
      @arg j value for j
      @arg k value for k
      @arg l value for l
      @arg m value for m
      @arg n value for n
      @arg o value for o
      @arg p value for p
    */
   YM void _init (YAC_Object *_a, YAC_Object *_b, YAC_Object *_c, YAC_Object *_d, YAC_Object *_e, YAC_Object *_f, YAC_Object *_g, YAC_Object *_h, YAC_Object *_i, YAC_Object *_j, YAC_Object *_k, YAC_Object *_l, YAC_Object *_m, YAC_Object *_n, YAC_Object *_o, YAC_Object *_p);

   /* @method initRow,int row,Double a,Double b,Double c,Double d
      Initialize row 'row'
      @arg row row index
      @arg a value for first column
      @arg b value for second column
      @arg c value for third column
      @arg d value for fourth column
    */
   YM void _initRow (sUI _row, YAC_Object *_a, YAC_Object *_b, YAC_Object *_c, YAC_Object *_d);

   /* @method initCol,int col,Double a,Double b,Double c,Double d
      Initialize column 'col'
      @arg c column index
      @arg a value for first row
      @arg b value for second row
      @arg c value for third row
      @arg d value for fourth row
    */
   YM void _initCol (sUI _col, YAC_Object *_a, YAC_Object *_b, YAC_Object *_c, YAC_Object *_d);

   /* @method initRowV,int row,Vector4d v
      Initialize row 'row'
      @arg row row index
      @arg v vector with values
    */
   YM void _initRowV (sUI _row, YAC_Object *_v);

   /* @method initColV,int col,Vector4d v
      Initialize column 'col'
      @arg col column index
      @arg v vector with values
    */
   YM void _initColV (sUI _col, YAC_Object *_v);

   /* @method initEulerXYX,Double a,Double b,Double c
      Initialize with euler roation matrix
      @arg a first rotation
      @arg b second rotation
      @arg c third rotation
    */
   YM void _initEulerXYX (YAC_Object *_a, YAC_Object *_b, YAC_Object *_c);

   /* @method initEulerXZX,Double a,Double b,Double c
      Initialize with euler roation matrix
      @arg a first rotation
      @arg b second rotation
      @arg c third rotation
    */
   YM void _initEulerXZX (YAC_Object *_a, YAC_Object *_b, YAC_Object *_c);
   /* @method initEulerYXY,Double a,Double b,Double c
      Initialize with euler roation matrix
      @arg a first rotation
      @arg b second rotation
      @arg c third rotation
    */
   YM void _initEulerYXY (YAC_Object *_a, YAC_Object *_b, YAC_Object *_c);
   /* @method initEulerYZY,Double a,Double b,Double c
      Initialize with euler roation matrix
      @arg a first rotation
      @arg b second rotation
      @arg c third rotation
    */
   YM void _initEulerYZY (YAC_Object *_a, YAC_Object *_b, YAC_Object *_c);
   /* @method initEulerZXZ,Double a,Double b,Double c
      Initialize with euler roation matrix
      @arg a first rotation
      @arg b second rotation
      @arg c third rotation
    */
   YM void _initEulerZXZ (YAC_Object *_a, YAC_Object *_b, YAC_Object *_c);
   /* @method initEulerZYZ,Double a,Double b,Double c
      Initialize with euler roation matrix
      @arg a first rotation
      @arg b second rotation
      @arg c third rotation
    */
   YM void _initEulerZYZ (YAC_Object *_a, YAC_Object *_b, YAC_Object *_c);
   /* @method initEulerXYZ,Double a,Double b,Double c
      Initialize with euler roation matrix
      @arg a first rotation
      @arg b second rotation
      @arg c third rotation
    */
   YM void _initEulerXYZ (YAC_Object *_a, YAC_Object *_b, YAC_Object *_c);
   /* @method initEulerXZY,Double a,Double b,Double c
      Initialize with euler roation matrix
      @arg a first rotation
      @arg b second rotation
      @arg c third rotation
    */
   YM void _initEulerXZY (YAC_Object *_a, YAC_Object *_b, YAC_Object *_c);
   /* @method initEulerYXZ,Double a,Double b,Double c
      Initialize with euler roation matrix
      @arg a first rotation
      @arg b second rotation
      @arg c third rotation
    */
   YM void _initEulerYXZ (YAC_Object *_a, YAC_Object *_b, YAC_Object *_c);
   /* @method initEulerYZX,Double a,Double b,Double c
      Initialize with euler roation matrix
      @arg a first rotation
      @arg b second rotation
      @arg c third rotation
    */
   YM void _initEulerYZX (YAC_Object *_a, YAC_Object *_b, YAC_Object *_c);
   /* @method initEulerZXY,Double a,Double b,Double c
      Initialize with euler roation matrix
      @arg a first rotation
      @arg b second rotation
      @arg c third rotation
    */
   YM void _initEulerZXY (YAC_Object *_a, YAC_Object *_b, YAC_Object *_c);
   /* @method initEulerZYX,Double a,Double b,Double c
      Initialize with euler roation matrix
      @arg a first rotation
      @arg b second rotation
      @arg c third rotation
    */
   YM void _initEulerZYX (YAC_Object *_a, YAC_Object *_b, YAC_Object *_c);

   /* @method New,Object a:Matrix4d
      Initialize from an array and return new instance of Matrix4d
      @arg a array with values
      @return new instance of Matrix4d
    */
   YM void _New (YAC_Object *_a, YAC_Value *_r);

   /* @method BuildPerspectiveMatrix4d,Double fov,Double aspect,Double zNear,Double zFar:Matrix4d
      Builds a perspective matrix
      @arg fov field of view in degrees
      @arg aspect aspect ratio
      @arg zNear zNear value
      @arg zFar zFar value
      @return generated matrix
    */
   YM void _BuildPerspectiveMatrix4d_YAC_RVAL (YAC_Object *_fov, YAC_Object *_aspect, YAC_Object *_znear, YAC_Object *_zfar, YAC_Value *_r);
   YM void _BuildPerspectiveMatrix4d_YAC_RARG (YAC_Object *_fov, YAC_Object *_aspect, YAC_Object *_znear, YAC_Object *_zfar, YAC_Object *_r);

   /* @method BuildLookAtMatrix4d,Vector3d eye,Vector3d center,Vector3d up:Matrix4d
      Build a look at matrix
      @arg eye eye direction vector
      @arg center center of camera vector
      @arg up vector for up direction
      @return generated matrix
    */
   YM void _BuildLookAtMatrix4d_YAC_RVAL (YAC_Object *_eye, YAC_Object *_center, YAC_Object *_up, YAC_Value *_r);
   YM void _BuildLookAtMatrix4d_YAC_RARG (YAC_Object *_eye, YAC_Object *_center, YAC_Object *_up, YAC_Object *_r);

   /* @method BuildRotateMatrix4d,Double angle,Vector3d axis:Matrix4d
      Build a rotation matrix
      @arg angle rotation angle (0..2PI)
      @arg axis rotation axis vector
      @return generated matrix
    */
   YM void _BuildRotateMatrix4d_YAC_RVAL (YAC_Object *_angle, YAC_Object *_axis, YAC_Value *_r);
   YM void _BuildRotateMatrix4d_YAC_RARG (YAC_Object *_angle, YAC_Object *_axis, YAC_Object *_r);

   /* @method BuildTranslateMatrix4d,Vector3d t:Matrix4d
      Build a translation matrix
      @arg t translation vector
      @return generated matrix
    */
   YM void _BuildTranslateMatrix4d_YAC_RVAL (YAC_Object *_t, YAC_Value *_r);
   YM void _BuildTranslateMatrix4d_YAC_RARG (YAC_Object *_t, YAC_Object *_r);

   /* @method BuildEulerXZX4d,Double a,Double b,Double c:Matrix4d
      Build an euler rotation matrix with XZX convention
      @arg a first angle in radians
      @arg b second angle in radians
      @arg c third angle in radians
      @return generated matrix
    */
   YM void _BuildEulerXZX4d_YAC_RVAL (YAC_Object *_a, YAC_Object *_b, YAC_Object *_c, YAC_Value *_r);
   YM void _BuildEulerXZX4d_YAC_RARG (YAC_Object *_a, YAC_Object *_b, YAC_Object *_c, YAC_Object *_r);

   /* @method BuildEulerXYX4d,Double a,Double b,Double c:Matrix4d
      Build an euler rotation matrix with XYX convention
      @arg a first angle in radians
      @arg b second angle in radians
      @arg c third angle in radians
      @return generated matrix
    */
   YM void _BuildEulerXYX4d_YAC_RVAL (YAC_Object *_a, YAC_Object *_b, YAC_Object *_c, YAC_Value *_r);
   YM void _BuildEulerXYX4d_YAC_RARG (YAC_Object *_a, YAC_Object *_b, YAC_Object *_c, YAC_Object *_r);

   /* @method BuildEulerYXY4d,Double a,Double b,Double c:Matrix4d
      Build an euler rotation matrix with YXY convention
      @arg a first angle in radians
      @arg b second angle in radians
      @arg c third angle in radians
      @return generated matrix
    */
   YM void _BuildEulerYXY4d_YAC_RVAL (YAC_Object *_a, YAC_Object *_b, YAC_Object *_c, YAC_Value *_r);
   YM void _BuildEulerYXY4d_YAC_RARG (YAC_Object *_a, YAC_Object *_b, YAC_Object *_c, YAC_Object *_r);

   /* @method BuildEulerYZY4d,Double a,Double b,Double c:Matrix4d
      Build an euler rotation matrix with YZY convention
      @arg a first angle in radians
      @arg b second angle in radians
      @arg c third angle in radians
      @return generated matrix
    */
   YM void _BuildEulerYZY4d_YAC_RVAL (YAC_Object *_a, YAC_Object *_b, YAC_Object *_c, YAC_Value *_r);
   YM void _BuildEulerYZY4d_YAC_RARG (YAC_Object *_a, YAC_Object *_b, YAC_Object *_c, YAC_Object *_r);

   /* @method BuildEulerZYZ4d,Double a,Double b,Double c:Matrix4d
      Build an euler rotation matrix with ZYZ convention
      @arg a first angle in radians
      @arg b second angle in radians
      @arg c third angle in radians
      @return generated matrix
    */
   YM void _BuildEulerZYZ4d_YAC_RVAL (YAC_Object *_a, YAC_Object *_b, YAC_Object *_c, YAC_Value *_r);
   YM void _BuildEulerZYZ4d_YAC_RARG (YAC_Object *_a, YAC_Object *_b, YAC_Object *_c, YAC_Object *_r);

   /* @method BuildEulerZXZ4d,Double a,Double b,Double c:Matrix4d
      Build an euler rotation matrix with ZXZ convention
      @arg a first angle in radians
      @arg b second angle in radians
      @arg c third angle in radians
      @return generated matrix
    */
   YM void _BuildEulerZXZ4d_YAC_RVAL (YAC_Object *_a, YAC_Object *_b, YAC_Object *_c, YAC_Value *_r);
   YM void _BuildEulerZXZ4d_YAC_RARG (YAC_Object *_a, YAC_Object *_b, YAC_Object *_c, YAC_Object *_r);

   /* @method BuildEulerXZY4d,Double a,Double b,Double c:Matrix4d
      Build an euler rotation matrix with XZY convention
      @arg a first angle in radians
      @arg b second angle in radians
      @arg c third angle in radians
      @return generated matrix
    */
   YM void _BuildEulerXZY4d_YAC_RVAL (YAC_Object *_a, YAC_Object *_b, YAC_Object *_c, YAC_Value *_r);
   YM void _BuildEulerXZY4d_YAC_RARG (YAC_Object *_a, YAC_Object *_b, YAC_Object *_c, YAC_Object *_r);

   /* @method BuildEulerXYZ4d,Double a,Double b,Double c:Matrix4d
      Build an euler rotation matrix with XYZ convention
      @arg a first angle in radians
      @arg b second angle in radians
      @arg c third angle in radians
      @return generated matrix
    */
   YM void _BuildEulerXYZ4d_YAC_RVAL (YAC_Object *_a, YAC_Object *_b, YAC_Object *_c, YAC_Value *_r);
   YM void _BuildEulerXYZ4d_YAC_RARG (YAC_Object *_a, YAC_Object *_b, YAC_Object *_c, YAC_Object *_r);

   /* @method BuildEulerYXZ4d,Double a,Double b,Double c:Matrix4d
      Build an euler rotation matrix with YXZ convention
      @arg a first angle in radians
      @arg b second angle in radians
      @arg c third angle in radians
      @return generated matrix
    */
   YM void _BuildEulerYXZ4d_YAC_RVAL (YAC_Object *_a, YAC_Object *_b, YAC_Object *_c, YAC_Value *_r);
   YM void _BuildEulerYXZ4d_YAC_RARG (YAC_Object *_a, YAC_Object *_b, YAC_Object *_c, YAC_Object *_r);

   /* @method BuildEulerYZX4d,Double a,Double b,Double c:Matrix4d
      Build an euler rotation matrix with YZX convention
      @arg a first angle in radians
      @arg b second angle in radians
      @arg c third angle in radians
      @return generated matrix
    */
   YM void _BuildEulerYZX4d_YAC_RVAL (YAC_Object *_a, YAC_Object *_b, YAC_Object *_c, YAC_Value *_r);
   YM void _BuildEulerYZX4d_YAC_RARG (YAC_Object *_a, YAC_Object *_b, YAC_Object *_c, YAC_Object *_r);

   /* @method BuildEulerZYX4d,Double a,Double b,Double c:Matrix4d
      Build an euler rotation matrix with ZYX convention
      @arg a first angle in radians
      @arg b second angle in radians
      @arg c third angle in radians
      @return generated matrix
    */
   YM void _BuildEulerZYX4d_YAC_RVAL (YAC_Object *_a, YAC_Object *_b, YAC_Object *_c, YAC_Value *_r);
   YM void _BuildEulerZYX4d_YAC_RARG (YAC_Object *_a, YAC_Object *_b, YAC_Object *_c, YAC_Object *_r);

   /* @method BuildEulerZXY4d,Double a,Double b,Double c:Matrix4d
      Build an euler rotation matrix with ZXY convention
      @arg a first angle in radians
      @arg b second angle in radians
      @arg c third angle in radians
      @return generated matrix
    */
   YM void _BuildEulerZXY4d_YAC_RVAL (YAC_Object *_a, YAC_Object *_b, YAC_Object *_c, YAC_Value *_r);
   YM void _BuildEulerZXY4d_YAC_RARG (YAC_Object *_a, YAC_Object *_b, YAC_Object *_c, YAC_Object *_r);

   /* @method initIdentity
      Initialize with identity matrix.
   */
   YM void _initIdentity(void);
};

/* @function matrix4d,Object a:Matrix4d
   Initialize from an array and return new instance of Matrix4d
   @arg a array with values
   @return new instance of Matrix4d
*/
YF void YAC_CALL _matrix4d (YAC_Object *_a, YAC_Value *_r);


#endif  // MATRIX4D_H_
