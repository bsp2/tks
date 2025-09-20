/// YAC_ObjectArray.h
///
/// (c) 2001-2023 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __YAC_OBJECTARRAY_H__
#define __YAC_OBJECTARRAY_H__


class YAC_ObjectArray : public YAC_Object {
public:
   YAC_Object  *template_object;
   sUI          max_elements;
   sUI          num_elements;
   YAC_Object **elements;
   
public:
   YAC_ObjectArray(void);
   ~YAC_ObjectArray();
   
   void  setTemplateObject(YAC_Object *_template);

   sBool YAC_VCALL yacIteratorInit(YAC_Iterator *) const;
   
   void  YAC_VCALL yacSerialize(YAC_Object *, sUI _typeinfo);
   sUI   YAC_VCALL yacDeserialize(YAC_Object *, sUI _typeinfo);
   
   void  YAC_VCALL yacArrayCopySize           (YAC_Object *_objarray);
   sBool YAC_VCALL yacArrayAlloc              (sUI _num, sUI,sUI,sUI);
   sBool YAC_VCALL yacArrayRealloc            (sUI _num, sUI,sUI,sUI);
   void  YAC_VCALL yacArraySet                (void *_context, sUI _index, YAC_Value *_value);
   void  YAC_VCALL yacArrayGet                (void *_context, sUI _index, YAC_Value *_ret);
   void  YAC_VCALL yacArrayGetDeref           (void *_context, sUI _index, YAC_Value *_ret);
   sUI   YAC_VCALL yacArrayGetWidth           (void) { return num_elements; }
   void  YAC_VCALL yacArraySetWidth           (sUI);
   sUI   YAC_VCALL yacArrayGetHeight          (void) { return 1; }
   sUI   YAC_VCALL yacArrayGetElementType     (void) { return 3; }
   sUI   YAC_VCALL yacArrayGetElementByteSize (void) { return sizeof(YAC_Object *); }
   sUI   YAC_VCALL yacArrayGetStride          (void) { return 0; }
   void *YAC_VCALL yacArrayGetPointer         (void) { return (void*)elements; }
   sUI   YAC_VCALL yacArrayGetNumElements     (void) { return num_elements; } 
   sUI   YAC_VCALL yacArrayGetMaxElements     (void) { return max_elements; } 
   void  YAC_VCALL yacArraySetTemplate        (YAC_Object *_templateObject);
   
   void  YAC_VCALL yacOperator          (sSI, YAC_Object*,YAC_Value*_r); 
   void  YAC_VCALL yacOperatorInit      (void *_context, YAC_Object *_o);
   sBool YAC_VCALL yacToString          (YAC_String *_s) const;
   sBool YAC_VCALL yacIsComposite       (void) {return 1;}
   sSI   YAC_VCALL yacTensorRank        (void) {return YAC_TENSOR_RANK_VECTOR;}
   
   sSI         _add           (YAC_Object *_value); 
   sSI         _remove        (YAC_Object *_o);
   sSI         _insert        (sSI _index, YAC_Object *_value);
   sSI         _delete        (sSI _index);
   sBool       alloc          (sUI _maxElements);
   void        free           (void);
   sBool       realloc        (sUI _maxElements);
   sUI         getNumElements (void);
   sUI         getMaxElements (void);
   void        empty          (void);
   sBool       isEmpty        (void);
   void        reverse        (void);
   YAC_Object *getNextFree    (void);
   void        setNumElements (sSI);
   void        setTemplate    (YAC_Object*);
   YAC_Object *getTemplate    (void); 
   YAC_Object *get            (sSI _idx);
   void        getDeref       (sSI _idx, YAC_Value *_r);
   void        unlink         (YAC_Object *_o, YAC_Value *_r); 
   sSI         indexOfPointer (YAC_Object *, sSI _startOff);
   sSI         indexOfObject  (YAC_Object *, sSI _startOff);
   sSI         containsPointer(YAC_Object *_s);
   sSI         containsObject (YAC_Object *_s);
   sSI         _swap          (sSI _indexs, sSI _indexd);

   void       _getString      (YAC_Value *_r) const;

   void        _useAll         (void);
   YAC_Object *_getFirst       (void);
   YAC_Object *_getLast        (void);

};


#endif // __YAC_OBJECTARRAY_H__
