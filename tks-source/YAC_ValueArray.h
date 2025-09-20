/// YAC_ValueArray.h
///
/// (c) 2001-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __TKSVALUEARRAY_H__
#define __TKSVALUEARRAY_H__


class YAC_ValueArray : public YAC_Object {
public:
   sUI          max_elements;
   sUI          num_elements;
   YAC_Value   *elements;

protected:
   YAC_Value *joinArray(YAC_Value *_d, YAC_Object *_a, sUI _neA, sBool _bCopy, sBool _bUnlink);

public:
   YAC_ValueArray  (void);
   ~YAC_ValueArray ();
   
   sBool YAC_VCALL yacIteratorInit (YAC_Iterator *) const;
   
   void YAC_VCALL yacSerialize   (YAC_Object *, sUI _typeinfo); 
   sUI  YAC_VCALL yacDeserialize (YAC_Object *, sUI _typeinfo); 
   
   void  YAC_VCALL yacArrayCopySize           (YAC_Object *_va); 
   sBool YAC_VCALL yacArrayAlloc              (sUI _num, sUI,sUI,sUI); 
   void  YAC_VCALL yacArraySet                (void *_context, sUI _index, YAC_Value *_value); 
   void  YAC_VCALL yacArrayGet                (void *_context, sUI _index, YAC_Value *_ret); 
   void  YAC_VCALL yacArrayGetDeref           (void *_context, sUI _index, YAC_Value *_ret); 
   sUI   YAC_VCALL yacArrayGetWidth           (void)                          {return num_elements;} 
   void  YAC_VCALL yacArraySetWidth           (sUI); 
   sUI   YAC_VCALL yacArrayGetHeight          (void)                          {return 1;} 
   sUI   YAC_VCALL yacArrayGetElementType     (void)                          {return 0;} 
   sUI   YAC_VCALL yacArrayGetElementByteSize (void)                          {return sizeof(YAC_Object *);} 
   sUI   YAC_VCALL yacArrayGetStride          (void)                          {return 0;} 
   void *YAC_VCALL yacArrayGetPointer         (void)                          {return (void*)elements;} 
   sUI   YAC_VCALL yacArrayGetNumElements     (void)                          {return num_elements;} 
   sUI   YAC_VCALL yacArrayGetMaxElements     (void)                          {return max_elements;} 
   sBool YAC_VCALL yacIsComposite             (void) {return 1;}
   sSI   YAC_VCALL yacTensorRank              (void) {return YAC_TENSOR_RANK_VECTOR;}
   void  YAC_VCALL yacOperator                (sSI, YAC_Object*, YAC_Value*); 
   sBool YAC_VCALL yacToString                (YAC_String *_s) const; 
   
   sBool alloc           (sUI _maxElements); 
   void  free            (void); 
   sBool realloc         (sUI _maxElements); 
   sUI   getNumElements  (void); 
   sUI   getMaxElements  (void); 
   void  empty           (void); 
   sBool isEmpty         (void);
   sBool _add            (YAC_Object *_valueObject);
   sBool _addVoid        (void);
   sBool _insert         (sSI _index, YAC_Object *_valueObject);
   sBool _delete         (sSI _index);
   void  reverse         (void); 
   void  setNumElements  (sUI); 
   void  _getString      (YAC_Value *_r) const; 
   void  get             (sSI _index, YAC_Value *_r);
   void  getDeref        (sSI _index, YAC_Value *_r);
   void  _set            (sUI _index, YAC_Object *_obj);
   void  _getRev         (sSI _index, YAC_Value *_r);
   void  _setRev         (sSI _index, YAC_Object *_obj);
   sSI   _swap           (sSI _indexs, sSI _indexd);
   sSI   containsObject  (YAC_Object *_o); 
   sSI   containsPointer (YAC_Object *_o); 
   sSI   indexOfPointer  (YAC_Object*, sSI _startOff);
   sSI   indexOfObject   (YAC_Object*, sSI _startOff);

   void  _useAll         (void);
   void  _getFirst       (YAC_Value *_r);
   void  _getLast        (YAC_Value *_r);

   sBool _joinArrays (YAC_Object *_a, YAC_Object *_b, sBool _bCopyA, sBool _bCopyB, sBool _bUnlinkA, sBool _bUnlinkB);
   sBool _joinCC (YAC_Object *_a, YAC_Object *_b);
   sBool _joinCR (YAC_Object *_a, YAC_Object *_b);
   sBool _joinCD (YAC_Object *_a, YAC_Object *_b);
   sBool _joinRR (YAC_Object *_a, YAC_Object *_b);
   sBool _joinRC (YAC_Object *_a, YAC_Object *_b);
   sBool _joinRD (YAC_Object *_a, YAC_Object *_b);
   sBool _joinDD (YAC_Object *_a, YAC_Object *_b);
   sBool _joinDC (YAC_Object *_a, YAC_Object *_b);
   sBool _joinDR (YAC_Object *_a, YAC_Object *_b);
};


#endif
