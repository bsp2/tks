/// YAC_FloatArray.h
///
/// (c) 2001-2025 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef YAC_FLOATARRAY_H__
#define YAC_FLOATARRAY_H__


class YAC_FloatArray : public YAC_Object {
public:
   sBool own_data;
   sUI   max_elements;
   sUI   num_elements;
   sF32 *elements;

public:
   YAC_FloatArray(void);
   ~YAC_FloatArray();

   sBool YAC_VCALL yacIteratorInit (YAC_Iterator *) const;

   void  YAC_VCALL yacSerialize   (YAC_Object *, sUI _typeinfo);
   sUI   YAC_VCALL yacDeserialize (YAC_Object*, sUI _typeinfo);

   void  YAC_VCALL yacArrayCopySize(YAC_Object *_floatarray);
   sBool YAC_VCALL yacArrayAlloc(sUI _num, sUI,sUI,sUI);
   sBool YAC_VCALL yacArrayRealloc(sUI _num, sUI,sUI,sUI);
   void  YAC_VCALL yacArrayGet(void *_context, sUI _index, YAC_Value *_value);
   void  YAC_VCALL yacArraySet(void *_context, sUI _index, YAC_Value *_ret);
   sUI   YAC_VCALL yacArrayGetWidth(void)            { return num_elements; }
   void  YAC_VCALL yacArraySetWidth(sUI);
   sUI   YAC_VCALL yacArrayGetHeight(void)           { return 1; }
   sUI   YAC_VCALL yacArrayGetElementType(void)      { return 2; }
   sUI   YAC_VCALL yacArrayGetElementByteSize(void)  { return sizeof(sF32); }
   sUI   YAC_VCALL yacArrayGetStride(void)           { return max_elements*sizeof(sSI); }
   void *YAC_VCALL yacArrayGetPointer(void)          { return (void*)elements; }
   sUI   YAC_VCALL yacArrayGetNumElements(void)      { return num_elements; }
   sUI   YAC_VCALL yacArrayGetMaxElements(void)      { return max_elements; }

   void  YAC_VCALL yacOperator          (sSI, YAC_Object*, YAC_Value*);
   sSI   YAC_VCALL yacTensorRank        (void) {return YAC_TENSOR_RANK_VECTOR;}

   sBool YAC_VCALL yacToString    (YAC_String *_s) const;

   sBool alloc          (sUI _maxElements);
   virtual void free    (void);
   sBool add            (sF32 _val);
   sBool add2           (sF32 _val1, sF32 _val2);
   sBool add3           (sF32 _val1, sF32 _val2, sF32 _val3);
   sBool add4           (sF32 _val1, sF32 _val2, sF32 _val3, sF32 _val4);
   sBool addFromARGB32  (sUI _argb32);
   sBool addArray       (YAC_Object *_a, sUI _off, sUI _len);
   sSI   addEmpty       (sUI _numelements);
   sUI   getNumElements (void);
   sUI   getMaxElements (void);
   void  empty          (void);
   sBool isEmpty        (void);
   sBool realloc        (sUI _maxElements);
   sSI   insert         (sSI _index, sF32 _val);
   sBool _delete        (sSI _index);
   sBool _delete2       (sSI _index);
   sBool _delete3       (sSI _index);
   sBool _delete4       (sSI _index);
   void  fill           (sF32);
   void  fillRegion     (sSI _off, sSI _len, sF32 _val);
   sBool allocAndFill   (sUI _num, sF32 _c);
   void  fillFromARGB32 (sUI _argb32);
   void  reverse        (void);
   void  scale          (YAC_Object *, sF32);
   void  setNumElements (sSI);
   void  blend          (YAC_Object*, sF32);
   void  blendAB        (YAC_Object*_a, YAC_Object*_b, sF32);
   void  copyFrom       (YAC_Object *_fa, sSI _off, sSI _len, sSI _doff);
   sUI   _read32        (YAC_Object *_st, sUI _num, sUI _doff);
   sUI   _read64        (YAC_Object *_st, sUI _num, sUI _doff);
   sUI   _write32       (YAC_Object *_st, sUI _num, sUI _doff);
   sUI   _write64       (YAC_Object *_st, sUI _num, sUI _doff);
   sF32  getMin         (void);
   sSI   getMinIdx      (void);
   sF32  getAbsMin      (void);
   sF32  getMax         (void);
   sSI   getMaxIdx      (void);
   sF32  getAbsMax      (void);
   sF32  getMean        (void);
   sF32  getAbsMean     (void);
   sF32  getRms         (void);
   sF32  getRmsMax      (void);
   sF32  get            (sSI _idx);
   void  _set           (sUI _idx, sF32 _val);
   sF32  _getRev        (sSI _idx);
   void  _setRev        (sSI _idx, sF32 _val);
   sF32  _getDefault    (sUI _index, sF32 _def);
   sBool  contains      (sF32);
   void  fixNaNToZero   (void);
   sSI   indexOf        (sF32 _val, sSI _off);
   sSI   indexOfNot     (sF32 _val, sSI _off);
   void  sortByValue    (YAC_Object *_indexTable);
   void  quicksortByValue (sSI *, sSI _l, sSI _r);
   sF32  winLinear      (sF32 _index);
   sF32  winCosine      (sF32 _index);
   sF32  winQuadratic   (sF32 _index);
   sF32  winCubic       (sF32 _index);
   sF32  winQuintic     (sF32 _index);
   sSI   rearrange      (YAC_Object *_ia);
   sSI   _swap          (sSI _indexs, sSI _indexd);
   void  _visit         (YAC_Object *_fa, sSI _off, sSI _len);
   void  _unlinkFrom    (YAC_Object *_fa);
   void  _useAll        (void);
   sF32  _getFirst      (void);
   sF32  _getLast       (void);
   void  _interleave    (sUI _numDestCh, sUI _destCh, YAC_Object *_src, sUI _numSrcCh, sUI _srcCh, sUI numCh, sUI _numElem);
   void  _getString     (YAC_Value *_r) const;
   void  _getString2    (YAC_Value *_r) const;   // (x;y) pairs
   void  _getString3    (YAC_Value *_r) const;   // (x;y;z) pairs

   sSI bezierCubicDeCasteljau2dRec (sF32 x0, sF32 y0,
                                    sF32 x1, sF32 y1,
                                    sF32 x2, sF32 y2,
                                    sF32 x3, sF32 y3,
                                    sSI _rec, sSI _recMax,
                                    sF32 _thresholdB, sF32 _thresholdP,
                                    YAC_FloatArray *_retT, sF32 _ct
                                    );
   void _bezierCubicDeCasteljau2d (sF32 x0, sF32 y0,
                                   sF32 x1, sF32 y1,
                                   sF32 x2, sF32 y2,
                                   sF32 x3, sF32 y3,
                                   sF32 _scaleX, sF32 _scaleY,
                                   sSI _recMax,
                                   sF32 _thresholdB, sF32 _thresholdP,
                                   YAC_Object *_retT
                                   );

   sSI   findNearestIdx1d (sF32 _x, sUI _off, sUI _stride, sF32 _maxDist);
   sSI   findNearestIdx2d (sF32 _x, sF32 _y, sUI _off, sUI _stride, sF32 _maxDist, sF32 _aspect);

   sF32  sum (sUI _off, sUI _num);

   sF32  sumPathLen1d (sUI _off, sUI _num);
   sF32  sumPathLen2d (sUI _off, sUI _num);

   sBool loadFromStreamUnsigned8 (YAC_Object *_stream, sUI _numElements);
   sBool loadFromStreamSigned16  (YAC_Object *_stream, sUI _numElements);
   sBool loadFromStreamSigned24  (YAC_Object *_stream, sUI _numElements);
   sBool loadFromStreamFloat32   (YAC_Object *_stream, sUI _numElements);

   sBool saveToStreamSigned16 (YAC_Object *_stream, sUI _numElements);
   sBool saveToStreamSigned12FrameEnc (YAC_Object *_stream, sUI _numElements);

   sBool _isEqual (YAC_Object *_fa);

   sBool _join (YAC_Object *_a, YAC_Object *_b);

   sF32  getAbsMaxDiff (YAC_Object *_fa);

   sSI  findIdxBefore (sF32 _val);

   // type: 1=LPF, 2=HPF, 3=BPF, 4=BRF, 5=PEQ, 6=LSH, 7=HSH
   void biquad (YAC_Object *_faIn, sSI _type, sF32 _dbGain, sF32 _freq, sF32 _res);

   void mergeArraySkipVal (YAC_Object *_fa, float _skipVal);
};


#endif // YAC_FLOATARRAY_H__
