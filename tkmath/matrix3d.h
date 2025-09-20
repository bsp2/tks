/// matrix3d.h
///
/// (c) 2008-2024 by Carsten Busse <carsten.busse@googlemail.com>
///                  Bastian Spiegel <bs@tkscript.de> (additional coding)
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#ifndef MATRIX3D_H_
#define MATRIX3D_H_

YG("Math");

/* @class Matrix3d,Object
This class represents a single precision 3x3 square matrix.
 */
YC class _Matrix3d : public YAC_Object {
  public:
   sF64 doubles[9];

  public:
   _Matrix3d(void);
   ~_Matrix3d();
   YAC_POOLED(_Matrix3d,YAC_POOL_PRIORITY_HIGH);

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

   void assignFloats (const sF32 *_fv, sUI _ne);
   void assignFloatArray (const YAC_FloatArray *_fa);
   void assignGeneric (YAC_ContextHandle _ctx, YAC_Object *_o);

   /* @method setA,Double v
      Set a element to v
      @arg v new value for a
    */
   YM void _setA(YAC_Object *);

   /* @method setB,Double v
      Set b element to v
      @arg v new value for b
    */
   YM void _setB(YAC_Object *);

   /* @method setC,Double v
      Set c element to v
      @arg v new value for c
    */
   YM void _setC(YAC_Object *);

   /* @method setD,Double v
      Set d element to v
      @arg v new value for d
    */
   YM void _setD(YAC_Object *);

   /* @method setE,Double v
      Set e element to v
      @arg v new value for e
    */
   YM void _setE(YAC_Object *);

   /* @method setF,Double v
      Set f element to v
      @arg v new value for f
    */
   YM void _setF(YAC_Object *);

   /* @method setG,Double v
      Set g element to v
      @arg v new value for g
    */
   YM void _setG(YAC_Object *);

   /* @method setH,Double v
      Set h element to v
      @arg v new value for h
    */
   YM void _setH(YAC_Object *);

   /* @method setI,Double v
      Set i element to v
      @arg v new value for i
    */
   YM void _setI(YAC_Object *);

   /* @method getA:Double
      Get value of a element
      @return value of a element
    */
   YM void _getA_YAC_RVAL(YAC_Value *);
   YM void _getA_YAC_RARG(YAC_Object *);

   /* @method getB:Double
      Get value of b element
      @return value of b element
    */
   YM void _getB_YAC_RVAL(YAC_Value *);
   YM void _getB_YAC_RARG(YAC_Object *);

   /* @method getC:Double
      Get value of c element
      @return value of c element
    */
   YM void _getC_YAC_RVAL(YAC_Value *);
   YM void _getC_YAC_RARG(YAC_Object *);

   /* @method getD:Double
      Get value of d element
      @return value of d element
    */
   YM void _getD_YAC_RVAL(YAC_Value *);
   YM void _getD_YAC_RARG(YAC_Object *);

   /* @method getE:Double
      Get value of e element
      @return value of e element
    */
   YM void _getE_YAC_RVAL(YAC_Value *);
   YM void _getE_YAC_RARG(YAC_Object *);

   /* @method getF:Double
      Get value of f element
      @return value of f element
    */
   YM void _getF_YAC_RVAL(YAC_Value *);
   YM void _getF_YAC_RARG(YAC_Object *);

   /* @method getG:Double
      Get value of g element
      @return value of g element
    */
   YM void _getG_YAC_RVAL(YAC_Value *);
   YM void _getG_YAC_RARG(YAC_Object *);

   /* @method getH:Double
      Get value of h element
      @return value of h element
    */
   YM void _getH_YAC_RVAL(YAC_Value *);
   YM void _getH_YAC_RARG(YAC_Object *);

   /* @method getI:Double
      Get value of i element
      @return value of i element
    */
   YM void _getI_YAC_RVAL(YAC_Value *);
   YM void _getI_YAC_RARG(YAC_Object *);

   /* @method getString:String
      Get string representation of matrix ((a, b, c), (d, e, f), (g, h, i))
      @return string representation of matrix
    */
   YM void _getString(YAC_Value *) const;

   /* @method add,Matrix3d m
      Add another matrix m to this instance
      @arg m matrix to add
    */
   YM void _add_YAC_RSELF(YAC_Object *);
   YM void _add_YAC_RVAL(YAC_Object *, YAC_Value *);
   YM void _add_YAC_RARG(YAC_Object *, YAC_Object *);

   /* @method sub,Matrix3d m
      Substract another matrix m from this instance
      @arg m matrix to substract
    */
   YM void _sub_YAC_RSELF(YAC_Object *);
   YM void _sub_YAC_RVAL(YAC_Object *, YAC_Value *);
   YM void _sub_YAC_RARG(YAC_Object *, YAC_Object *);

   /* @method muld,Double v
      Multiply this instance by scalar value v
      @arg v value to multiply
    */
   void muld(const sF64 s);
   YM void _muld_YAC_RSELF(YAC_Object *);
   YM void _muld_YAC_RVAL(YAC_Object *, YAC_Value *);
   YM void _muld_YAC_RARG(YAC_Object *, YAC_Object *);

   /* @method mul,Matrix3d m
      Multiply this instance by matrix m (this * m)
      @arg m matrix to multiply
    */
   YM void _mul_YAC_RSELF(YAC_Object *);
   YM void _mul_YAC_RVAL(YAC_Object *, YAC_Value *);
   YM void _mul_YAC_RARG(YAC_Object *, YAC_Object *) const;

   /* @method mulRev,Matrix3d m
      Reverse-multiply matrix m by this instance (m * this)
      @arg m matrix to multiply
    */
   YM void _mulRev_YAC_RSELF(YAC_Object *);
   YM void _mulRev_YAC_RVAL(YAC_Object *, YAC_Value *);
   YM void _mulRev_YAC_RARG(YAC_Object *, YAC_Object *) const;

   /* @method mulv,Vector3d v:Vector3d
      Multiply this instance by a 3d vector
      @arg v vector to multiply
      @return resulting vector
    */
   YM void _mulv_YAC_RVAL(YAC_Object *, YAC_Value *);
   YM void _mulv_YAC_RARG(YAC_Object *, YAC_Object *);

   /* @method getAbs:Double
      Calculate the Hilbert-Schmidt norm of the matrix
      @return scalar matrix norm value
    */
   sF64 getAbs(void);
   YM void _getAbs_YAC_RVAL(YAC_Value *);
   YM void _getAbs_YAC_RARG(YAC_Object *);

   /* @method getAbsSqr:Double
      Calculate the squared Hilbert-Schmidt norm of the matrix
      @return scalar matrix norm value
    */
   sF64 getAbsSqr(void);
   YM void _getAbsSqr_YAC_RVAL(YAC_Value *);
   YM void _getAbsSqr_YAC_RARG(YAC_Object *);

   /* @method unit
      Normalize matrix with Hilbert-Schmidt norm
    */
   YM void _unit_YAC_RSELF(void); /* unit */
   YM void _unit_YAC_RVAL(YAC_Value *);
   YM void _unit_YAC_RARG(YAC_Object *);

   /* @method unitScale,Double v
      Normalize matrix with Hilbert-Schmidt norm and multiply by v afterwards
      @arg v value to scale matrix
    */
   void unitScale(sF64);
   YM void _unitScale_YAC_RSELF(YAC_Object *);
   YM void _unitScale_YAC_RVAL(YAC_Object *, YAC_Value *);
   YM void _unitScale_YAC_RARG(YAC_Object *, YAC_Object *);

   /* @method det:Double
      Calculate determinant of matrix
      @return value of determinant
    */
   sF64 det(void);
   YM void _det_YAC_RVAL(YAC_Value *);
   YM void _det_YAC_RARG(YAC_Object *);

   /* @method invert
      Calculate inversion of matrix, so that A*A<sup>-1</sup>=U
    */
   YM void _invert_YAC_RSELF(void);
   YM void _invert_YAC_RVAL(YAC_Value *);
   YM void _invert_YAC_RARG(YAC_Object *);

   /* @method transpose
      Transpose matrix
    */
   YM void _transpose_YAC_RSELF(void);
   YM void _transpose_YAC_RVAL(YAC_Value *);
   YM void _transpose_YAC_RARG(YAC_Object *);

   /* @method setRow,int r,Vector3d v
      Set row r with values from v
      @arg r row number
      @arg v Vector3d instance
    */
   YM void _setRow(sUI, YAC_Object*);

   /* @method getRow,int r:Vector3d
      Get row r values
      @arg r row number
      @return vector with values from row r
    */
   YM void _getRow_YAC_RVAL(sUI, YAC_Value*);
   YM void _getRow_YAC_RARG(sUI, YAC_Object*);

   /* @method setCol,int c,Vector3d v
      Set column c with values from v
      @arg c column number
      @arg v Vector3d instance
    */
   YM void _setCol(sUI, YAC_Object*);

   /* @method getCol,int c:Vector3d
      Get column c values
      @arg c column number
      @return vector with values from column c
    */
   YM void _getCol_YAC_RVAL(sUI, YAC_Value*);
   YM void _getCol_YAC_RARG(sUI, YAC_Object*);

   /* @method initf,float va,float vb,float vc,float vd,float ve, float vf, float vg, float vh, float vi
      Initialise matrix
      @arg va value for a
      @arg vb value for b
      @arg vc value for c
      @arg vd value for d
      @arg ve value for e
      @arg vf value for f
      @arg vg value for g
      @arg vh value for h
      @arg vi value for i
    */
   YM void _initf(sF32,sF32,sF32,sF32,sF32,sF32,sF32,sF32,sF32);

   /* @method init,Double va,Double vb,Double vc,Double vd,Double ve, Double vf, Double vg, Double vh, Double vi
      Initialise matrix
      @arg va value for a
      @arg vb value for b
      @arg vc value for c
      @arg vd value for d
      @arg ve value for e
      @arg vf value for f
      @arg vg value for g
      @arg vh value for h
      @arg vi value for i
    */
   YM void _init(YAC_Object *,YAC_Object *,YAC_Object *,YAC_Object *,YAC_Object *,YAC_Object *,YAC_Object *,YAC_Object *,YAC_Object *);

   /* @method initRow,int r,Double va,Double vb,Double vc
      Initialise row r of matrix
      @arg r row number
      @arg va value for first element
      @arg vb value for second element
      @arg vc value for third element
    */
   YM void _initRow(sUI,YAC_Object *,YAC_Object *,YAC_Object *);

   /* @method initCol,int c,Double va,Double vb,Double vc
      Initialise column c of matrix
      @arg c column number
      @arg va value for first element
      @arg vb value for second element
      @arg vc value for third element
    */
   YM void _initCol(sUI,YAC_Object *,YAC_Object *,YAC_Object *);

   /* @method initRowV,int r,Vector3d v
      Initialise row r of matrix
      @arg r row number
      @arg v vector with values
    */
   YM void _initRowV(sUI,YAC_Object*);

   /* @method initColV,int c,Vector3d v
      Initialise column c of matrix
      @arg c column number
      @arg v vector with values
    */
   YM void _initColV(sUI,YAC_Object*);

   /* @method New,Double va,Double vb,Double vc,Double vd,Double ve,Double vf,Double vg,Double vh,Double vi:Matrix3d
      Initialise and return new instance of Matrix3d
      @arg va value for a
      @arg vb value for b
      @arg vc value for c
      @arg vd value for d
      @arg ve value for e
      @arg vf value for f
      @arg vg value for g
      @arg vh value for h
      @arg vi value for i
      @return new instance of Matrix3d
    */
   YM void _New(YAC_Object *,YAC_Object *,YAC_Object *,YAC_Object *,YAC_Object *,YAC_Object *,YAC_Object *,YAC_Object *,YAC_Object *,YAC_Value*);

   /* @method initIdentity
      Initialize with identity matrix.
   */
   YM void _initIdentity(void);
};

/* @function matrix3d,Double va,Double vb,Double vc,Double vd,Double ve,Double vf,Double vg,Double vh,Double vi:Matrix3d
   Initialise and return new instance of Matrix3d
   @arg va value for a
   @arg vb value for b
   @arg vc value for c
   @arg vd value for d
   @arg ve value for e
   @arg vf value for f
   @arg vg value for g
   @arg vh value for h
   @arg vi value for i
   @return new instance of Matrix3d
 */
YF void YAC_CALL _matrix3d(YAC_Object *,YAC_Object *,YAC_Object *,YAC_Object *,YAC_Object *,YAC_Object *,YAC_Object *,YAC_Object *,YAC_Object *,YAC_Value*);


#endif // MATRIX3D_H_
