/// YAC_IntArray.h
///
/// (c) 2001-2025 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef YAC_INTARRAY_H__
#define YAC_INTARRAY_H__


class YAC_IntArray : public YAC_Object {
public:
   sBool own_data; 
   sUI   max_elements;
   sUI   num_elements;
   sSI  *elements; 
   
public:
   YAC_IntArray(void);
   ~YAC_IntArray();
   
   sBool         YAC_VCALL yacIteratorInit (YAC_Iterator *) const;
   
   void          YAC_VCALL yacSerialize               (YAC_Object *, sUI _typeinfo);
   sUI           YAC_VCALL yacDeserialize             (YAC_Object *, sUI _typeinfo);
   
   void          YAC_VCALL yacArrayCopySize           (YAC_Object *_intarray);
   sBool         YAC_VCALL yacArrayAlloc              (sUI _num, sUI,sUI,sUI);
   sBool         YAC_VCALL yacArrayRealloc            (sUI _num, sUI,sUI,sUI);
   void          YAC_VCALL yacArraySet                (void *_context, sUI _index, YAC_Value *_value);
   void          YAC_VCALL yacArrayGet                (void *_context, sUI _index, YAC_Value *_ret);
   sUI           YAC_VCALL yacArrayGetWidth           (void) { return num_elements; }
   sUI           YAC_VCALL yacArrayGetHeight          (void) { return 1; }
   void          YAC_VCALL yacArraySetWidth           (sUI);
   sUI           YAC_VCALL yacArrayGetElementType     (void) { return 1; }
   sUI           YAC_VCALL yacArrayGetElementByteSize (void) { return sizeof(sUI); }
   sUI           YAC_VCALL yacArrayGetStride          (void) { return max_elements*sizeof(sSI); }
   void         *YAC_VCALL yacArrayGetPointer         (void) { return (void*)elements; }
   sUI           YAC_VCALL yacArrayGetNumElements     (void) { return num_elements; }
   sUI           YAC_VCALL yacArrayGetMaxElements     (void) { return max_elements; }

   sBool         YAC_VCALL yacToString                (YAC_String *_s) const;
  
   void          YAC_VCALL yacOperator                (sSI, YAC_Object*, YAC_Value*);

   /* /\* sBool compareArray(YAC_Object *_o); *\/ */
   sBool         _isEqual           (YAC_Object *_o);
   
   sBool         alloc              (sUI _maxElements);
   void          free               (void);
   sBool         add                (sSI);
   sBool         add2               (sSI, sSI);
   sBool         add3               (sSI, sSI, sSI);
   sBool         add4               (sSI, sSI, sSI, sSI);
   sBool         addArray           (YAC_Object *_a, sUI _off, sUI _len);
   sSI           addEmpty           (sUI);
   sSI           getNumElements     (void);
   sSI           getMaxElements     (void);
   void          empty              (void);
   sBool         isEmpty            (void);
   sBool         realloc            (sUI _maxElements);
   sSI           insert             (sSI, sSI);
   sBool         _delete            (sSI);
   sBool         _delete2           (sSI _index);
   sBool         _delete3           (sSI _index);
   sBool         _delete4           (sSI _index);
   void          fill               (sSI);
   sBool         allocAndFill       (sUI _num, sSI _c);
   void          fillRegion         (sSI _off, sSI _len, sSI _c);
   void          reverse            (void);
   void          setNumElements     (sSI);
   void          swapByteOrder      (void);
   sBool         visitBytes         (YAC_Object *, sSI, sSI);
   void          copyFrom           (YAC_Object *_ia, sSI _off, sSI _len, sSI _doff);
   sSI           elementExists      (sSI);  // DEPRECATED 
   sBool         _contains          (sSI); 
   sBool         _remove            (sSI); 
   sUI           _read8             (YAC_Object *_st, sUI _num, sUI _doff); 
   sUI           _read16            (YAC_Object *_st, sUI _num, sUI _doff); 
   sUI           _read32            (YAC_Object *_st, sUI _num, sUI _doff); 
   sUI           _write8            (YAC_Object *_st, sUI _num, sUI _doff); 
   sUI           _write16           (YAC_Object *_st, sUI _num, sUI _doff); 
   sUI           _write32           (YAC_Object *_st, sUI _num, sUI _doff);
   void          _getString         (YAC_Value *_r) const;
   void          _getString2        (YAC_Value *_r) const;   // (x;y) pairs
   void          _getString3        (YAC_Value *_r) const;   // (x;y;z) pairs
   sSI           getMin             (void); 
   sSI           getMax             (void); 
   sSI           getAbsMin          (void); 
   sSI           getAbsMax          (void); 
   sF32          getMean            (void); 
   sF32          getAbsMean         (void); 
   sSI           get                (sSI _idx); 
   void         _set                (sUI _idx, sSI _val);
   sSI           _getRev            (sSI _idx);
   void          _setRev            (sSI _idx, sSI _val);
   sSI           _getDefault        (sUI _index, sSI _def);
   void          identity           (sSI _num); 
   sSI           indexOf            (sSI _val, sSI _off);
   sSI           indexOfNot         (sSI _val, sSI _off);
   void          sortByValue        (YAC_Object *_indexTable); 
   void          quicksortByValue   (sSI *, sSI _l, sSI _r); 
   sBool         rearrange          (YAC_Object *_ia);    
   sBool        _swap               (sSI _indexs, sSI _indexd);
   void         _visit              (YAC_Object *_ia, sSI _off, sSI _len);
   void         _useAll             (void);
   sSI          _getFirst           (void);
   sSI          _getLast            (void);
   sBool        _join               (YAC_Object *_a, YAC_Object *_b);
   sSI          _replace            (sSI _a, sSI _b);
   sSI          _replaceNeg         (sSI _b);
   void         _inc                (sUI _idx);
   void         _dec                (sUI _idx);
   sSI          _getNumUniqueValues (void);
   sSI          _findSubArrayOffset (YAC_Object *_ia);
};


#endif // YAC_INTARRAY_H__
