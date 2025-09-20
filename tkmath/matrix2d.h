/// matrix2d.h
///
/// (c) 2008-2024 by Carsten Busse <carsten.busse@googlemail.com>
///                  Bastian Spiegel <bs@tkscript.de> (additional coding)
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///

#ifndef MATRIX2D_H_
#define MATRIX2D_H_

YG("Math");

/* @class Matrix2d,Object
This class represents a double precision 2x2 square matrix.
   0 1
   2 3
 */
YC class _Matrix2d : public YAC_Object {
  public:
   sF64 doubles[4];

   public:
   _Matrix2d(void);
   ~_Matrix2d();
   YAC_POOLED(_Matrix2d,YAC_POOL_PRIORITY_HIGH);

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
   void  YAC_VCALL yacOperatorF64             (sSI, sF64, YAC_Value *);
   sUI   YAC_VCALL yacOperatorPriority        (void) { return YAC_OP_PRIO_DOUBLE; }
   sBool YAC_VCALL yacToString                (YAC_String *) const;

   // Helper methods
   void assignFloats (sF32 *_fv, sUI _ne);
   void assignFloatArray (YAC_FloatArray *_fa);
   void assignFloatMatrix (YAC_Object *_o);
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

   /* @method getString:String
      Get string representation of matrix ((a, b), (c, d))
      @return string representation of matrix
    */
   YM void _getString (YAC_Value *_r) const;

   /* @method add,Matrix2d m
      Add another matrix m to this instance
      @arg m matrix to add
    */
   YM void _add_YAC_RSELF (YAC_Object *_m);
   YM void _add_YAC_RVAL  (YAC_Object *_m, YAC_Value *_r);
   YM void _add_YAC_RARG  (YAC_Object *_m, YAC_Object *_r);

   /* @method sub,Matrix2d m
      Substract another matrix m from this instance
      @arg m matrix to substract
    */
   YM void _sub_YAC_RSELF (YAC_Object *_m);
   YM void _sub_YAC_RVAL  (YAC_Object *_m, YAC_Value *_r);
   YM void _sub_YAC_RARG  (YAC_Object *_m, YAC_Object *_r);

   /* @method muld,Double v
      Multiply this instance by scalar value v
      @arg v value to multiply
    */
   void muld (const sF64 s);
   YM void _muld_YAC_RSELF (YAC_Object *_v);
   YM void _muld_YAC_RVAL  (YAC_Object *_v, YAC_Value *_r);
   YM void _muld_YAC_RARG  (YAC_Object *_v, YAC_Object *_r);

   /* @method mul,Matrix2d m
      Multiply this instance with matrix m (this * m)
      @arg m matrix to multiply
    */
   YM void _mul_YAC_RSELF (YAC_Object *_m);
   YM void _mul_YAC_RVAL  (YAC_Object *_m, YAC_Value *_r);
   YM void _mul_YAC_RARG  (YAC_Object *_m, YAC_Object *_r);

   /* @method mulRev,Matrix2d m
      Reverse-multiply matrix m by this instance (m * this)
      @arg m matrix to multiply
    */
   YM void _mulRev_YAC_RSELF (YAC_Object *_m);
   YM void _mulRev_YAC_RVAL  (YAC_Object *_m, YAC_Value *_r);
   YM void _mulRev_YAC_RARG  (YAC_Object *_m, YAC_Object *_r);

   /* @method mulv,Vector2d v:Vector2d
      Multiply this instance with a 2dim vector
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
      Normalise matrix with Hilbert-Schmidt norm
    */
   YM void _unit_YAC_RSELF (void);
   YM void _unit_YAC_RVAL  (YAC_Value *_r);
   YM void _unit_YAC_RARG  (YAC_Object *_r);

   /* @method unitScale,Double v
      Normalise matrix with Hilbert-Schmidt norm and multiply with v afterwards
      @arg v value to scale matrix
    */
   void unitScale (sF64);
   YM void _unitScale_YAC_RSELF (YAC_Object *_v);
   YM void _unitScale_YAC_RVAL  (YAC_Object *_v, YAC_Value *_r);
   YM void _unitScale_YAC_RARG  (YAC_Object *_v, YAC_Object *_r);

   /* @method det:Double
      Calculate determinant of matrix
      @return value of determinant
    */
   sF64 det (void);
   YM void _det_YAC_RVAL (YAC_Value *_r);
   YM void _det_YAC_RARG (YAC_Object *_r);

   /* @method invert
      Calculate inversion of matrix, so that A*A<sup>-1</sup>=U
    */
   YM void _invert_YAC_RSELF (void);
   YM void _invert_YAC_RVAL  (YAC_Value *_r);
   YM void _invert_YAC_RARG  (YAC_Object *_r);

   /* @method transpose
      Transpose matrix
    */
   YM void _transpose_YAC_RSELF (void);
   YM void _transpose_YAC_RVAL  (YAC_Value *_r);
   YM void _transpose_YAC_RARG  (YAC_Object *_r);

   /* @method setRow,int r,Vector2d v
      Set row r with values from v
      @arg row row number
      @arg v Vector2f instance
    */
   YM void _setRow (sUI _row, YAC_Object *_v);

   /* @method getRow,int r:Vector2d
      Get row r values
      @arg r row number
      @return vector with values from row r
    */
   YM void _getRow_YAC_RVAL (sUI _row, YAC_Value *_r);
   YM void _getRow_YAC_RARG (sUI _row, YAC_Object *_r);

   /* @method setCol,int c,Vector2d v
      Set column c with values from v
      @arg c column number
      @arg v Vector2f instance
    */
   YM void _setCol (sUI _col, YAC_Object *_v);

   /* @method getCol,int c:Vector2d
      Get column c values
      @arg c column number
      @return vector with values from column c
    */
   YM void _getCol_YAC_RVAL (sUI _col, YAC_Value *_r);
   YM void _getCol_YAC_RARG (sUI _col, YAC_Object *_r);

   /* @method init,Doubla a,Double b,Double c,Double d
      Initialize matrix
      @arg a value for a
      @arg b value for b
      @arg c value for c
      @arg d value for d
    */
   YM void _init (YAC_Object *_a, YAC_Object *_b, YAC_Object *_c, YAC_Object *_d);

   /* @method initf,float a,float b,float c,float d
      Initialize matrix
      @arg a value for a
      @arg b value for b
      @arg c value for c
      @arg d value for d
    */
   YM void _initf (sF32 _a, sF32 _b, sF32 _c, sF32 _d);

   /* @method initRow,int r,Double a,Double b
      Initialize row r of matrix
      @arg row row number
      @arg a value for first column
      @arg b value for second column
    */
   YM void _initRow (sUI _row, YAC_Object *_a, YAC_Object *_b);

   /* @method initCol,int c,Double a,Double b
      Initialize column 'col' of the matrix
      @arg col column number
      @arg a value for first row
      @arg b value for second row
    */
   YM void _initCol (sUI _col, YAC_Object *_a, YAC_Object *_b);

   /* @method initRowV,int r,Vector2d v
      Initialize row 'row' of the matrix
      @arg row row number
      @arg v vector with values
    */
   YM void _initRowV (sUI _row, YAC_Object *_v);

   /* @method initColV,int c,Vector2d v
      Initialize column 'col' of the matrix
      @arg c column number
      @arg v vector with values
    */
   YM void _initColV(sUI,YAC_Object*);

   /* @method New,Double a,Double b,Double c,Double d:Matrix2d
      Initialize and return new instance of Matrix2d
      @arg a value for a
      @arg b value for b
      @arg c value for c
      @arg d value for d
      @return new instance of Matrix2d
    */
   YM void _New(YAC_Object *_a, YAC_Object *_b, YAC_Object *_c, YAC_Object *_d, YAC_Value *_r);

   /* @method initIdentity
      Initialize with identity matrix.
   */
   YM void _initIdentity (void);

   /* @method _initScalef
    */
   YM void _initScalef (sF32 _x, sF32 _y);

   /* @method _initRotatef
      @arg a rotation angle (0..2PI)
    */
   YM void _initRotatef (sF32 _a);
};

/* @function matrix2d,Double a,Double b,Double c,Double d:Matrix2d
   Initialize and return new instance of Matrix2d
   @arg a value for a
   @arg b value for b
   @arg c value for c
   @arg d value for d
   @return new instance of Matrix2d
 */
YF void YAC_CALL _matrix2d(YAC_Object *_a, YAC_Object *_b, YAC_Object *_c, YAC_Object *_d, YAC_Value *_r);


#endif  // MATRIX2D_H_
