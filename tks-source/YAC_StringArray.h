/// YAC_StringArray.h
///
/// (c) 2001-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __YAC_STRINGARRAY_H__
#define __YAC_STRINGARRAY_H__


class YAC_StringArray : public YAC_Object {
public:
   sUI         max_elements;
   sUI         num_elements;
   YAC_String *elements;

public:
   YAC_StringArray  (void);
   ~YAC_StringArray ();

   void          YAC_VCALL yacArrayCopySize           (YAC_Object *_stringarray);
   sBool         YAC_VCALL yacArrayAlloc              (sUI _num, sUI,sUI,sUI);
   void          YAC_VCALL yacArraySet                (void *_context, sUI _index, YAC_Value *_value);
   void          YAC_VCALL yacArrayGet                (void *_context, sUI _index, YAC_Value *_ret);
   sUI           YAC_VCALL yacArrayGetWidth           (void)  { return num_elements; }
   void          YAC_VCALL yacArraySetWidth           (sUI);
   sUI           YAC_VCALL yacArrayGetHeight          (void)  { return 1; }
   sUI           YAC_VCALL yacArrayGetElementType     (void)  { return 4; }
   sUI           YAC_VCALL yacArrayGetElementByteSize (void)  { return sizeof(YAC_String *); }
   sUI           YAC_VCALL yacArrayGetStride          (void)  { return 0; }
   void *        YAC_VCALL yacArrayGetPointer         (void)  { return (void*)elements; }
   sUI           YAC_VCALL yacArrayGetNumElements     (void)  { return num_elements; }
   sUI           YAC_VCALL yacArrayGetMaxElements     (void)  { return max_elements; }
   sUI           YAC_VCALL yacDeserialize             (YAC_Object *, sUI _typeinfo);
   sBool         YAC_VCALL yacIteratorInit            (YAC_Iterator *) const;
   void          YAC_VCALL yacOperator                (sSI, YAC_Object*, YAC_Value*);
   void          YAC_VCALL yacSerialize               (YAC_Object *, sUI _typeinfo);
   sBool         YAC_VCALL yacToString                (YAC_String *_s) const;
   sSI           YAC_VCALL yacTensorRank              (void) {return YAC_TENSOR_RANK_VECTOR;}


protected:
   void        quicksortByValue  (sSI *, sSI _l, sSI _r, sSI _bCaseSensitive);
   void        quicksortByLength (sSI *, sSI _l, sSI _r);

public:
   sBool       alloc          (sUI _maxElements);
   void        free           (void);
   sBool       add            (YAC_Object *_s);
   sUI         addEmpty       (sUI _numElements);
   sBool       remove         (YAC_Object *_s);
   sUI         getNumElements (void);
   sUI         getMaxElements (void);
   void        empty          (void);
   sBool       isEmpty        (void);
   sBool       realloc        (sUI _maxElements);
   sBool       insert         (sSI _index, YAC_Object *_s);
   sBool       _delete        (sUI _index);
   void        unset          (void);
   void        reverse        (void);
   YAC_Object *getNextFree    (void);
   void        setNumElements (sUI);
   void        sortByLength   (YAC_Object *_ia);
   void        sortByValue    (YAC_Object *_ia, sSI _cs);
   YAC_Object *get            (sSI _index);
   void        _set           (sUI _idx, YAC_Object *_val);
   YAC_Object *_getRev        (sSI _index);
   void        _setRev        (sUI _idx, YAC_Object *_val);
   void        getCopy        (sSI _index, YAC_Value *_r);
   void        _getDefault    (sUI _index, YAC_Object *_valOrVO, YAC_Value *_r);
   void        _getString     (YAC_Value *_r) const;
   sSI         indexOfPointer (YAC_Object *, sSI _startOff);
   sSI         indexOfObject  (YAC_Object *, sSI _startOff);
   sSI         containsPointer(YAC_Object *_s);
   sSI         containsObject (YAC_Object *_s);
   sBool       rearrange      (YAC_Object *_ia);
   sSI         _swap          (sSI _indexs, sSI _indexd);
   void        _useAll        (void);
   YAC_Object *_getFirst      (void);
   YAC_Object *_getLast       (void);
   sBool       _join          (YAC_Object *_a, YAC_Object *_b);
   sUI         _getMaxChars   (void) const;
   void        _mergeToString (YAC_String *_sepOrNull, YAC_Value *_r) const;

};


#endif // __YAC_STRINGARRAY_H__
