/// YAC_ClassArray.h
///
/// (c) 2001-2023 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __YAC_CLASSARRAY_H__
#define __YAC_CLASSARRAY_H__


class YAC_ClassArray: public YAC_Object {
public:
   TKS_ClassDecl             *class_decl;
   sUI                        max_elements;
   sUI                        num_elements;
   TKS_ScriptClassInstance **elements;
public:
   
   YAC_ClassArray(void);
   YAC_ClassArray(TKS_ClassDecl *);
   ~YAC_ClassArray();
   
   void          YAC_VCALL yacOperator          (sSI, YAC_Object *, YAC_Value *);
   void          YAC_VCALL yacOperatorInit      (void *_context, YAC_Object *);
   void          YAC_VCALL yacSerialize         (YAC_Object *, sUI _typeinfo);
   sUI           YAC_VCALL yacDeserialize       (YAC_Object *, sUI _typeinfo);
   sBool         YAC_VCALL yacIteratorInit      (YAC_Iterator *) const;
   sBool         YAC_VCALL yacIsComposite       (void) {return 1;}
   sSI           YAC_VCALL yacTensorRank        (void) {return YAC_TENSOR_RANK_VECTOR;}
  
   void  YAC_VCALL yacArrayCopySize(YAC_Object *_objarray);
   sBool YAC_VCALL yacArrayAlloc(sUI _num, sUI,sUI,sUI);
   void  YAC_VCALL yacArraySet(void *_context, sUI _index, YAC_Value *_value);
   void  YAC_VCALL yacArrayGet(void *_context, sUI _index, YAC_Value *_ret);
   void  YAC_VCALL yacArrayGetDeref(void *_context, sUI _index, YAC_Value *_ret);
   sUI   YAC_VCALL yacArrayGetWidth(void)            { return num_elements; }
   void  YAC_VCALL yacArraySetWidth(sUI); 
   sUI   YAC_VCALL yacArrayGetHeight(void)           { return 1; }
   sUI   YAC_VCALL yacArrayGetElementType(void)      { return 3; }
   sUI   YAC_VCALL yacArrayGetElementByteSize(void)  { return sizeof(YAC_Object *); }
   sUI   YAC_VCALL yacArrayGetStride(void)           { return 0; }
   void *YAC_VCALL yacArrayGetPointer(void)          { return (void*)elements; }
   sUI   YAC_VCALL yacArrayGetNumElements(void)      { return num_elements; } 
   sUI   YAC_VCALL yacArrayGetMaxElements(void)      { return max_elements; } 
   void  YAC_VCALL yacArraySetTemplate(YAC_Object *_templateObject);
   
   sBool       alloc           (sUI _maxelements);
   void        free            (void);
   sBool       realloc         (sUI _maxelements);
   sUI         getNumElements  (void);
   sUI         getMaxElements  (void);
   void        empty           (void);
   sBool       isEmpty         (void);
   void        reverse         (void);
   YAC_Object *getNextFree     (void);
   void        setNumElements  (sUI); 
   sSI         _add            (YAC_Object *_value); 
   sSI         _remove         (YAC_Object *_o);
   sSI         _insert         (sSI _index, YAC_Object *_value);
   sSI         _delete         (sSI _index);
   YAC_Object *get             (sSI _index); 
   void        getDeref        (sSI _index, YAC_Value *_r); 
   void        unlink          (YAC_Object *_o, YAC_Value *_r); 
   sSI         contains        (YAC_Object *_o);
   YAC_Object *getTemplate     (void);
   void        setTemplate     (YAC_Object *_sciTemplate);
   sSI         _swap           (sSI _indexs, sSI _indexd);
   sSI         indexOfPointer  (YAC_Object*, sSI _startOff);
   sSI         indexOfObject   (YAC_Object*, sSI _startOff);
   sSI         containsObject  (YAC_Object *_o); 
   sSI         containsPointer (YAC_Object *_o); 
   /*     void          _getString                 (YAC_Value *_r); // TODO  */
   /*     sBool         yacToString                (YAC_String *_s); // TODO  */
   void        _useAll         (void);
   YAC_Object *_getFirst       (void);
   YAC_Object *_getLast        (void);
};


#endif // __YAC_CLASSARRAY_H__
