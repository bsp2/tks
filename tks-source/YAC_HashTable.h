/// YAC_HashTable.h
///
/// (c) 2001-2023 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///
#ifndef __YAC_HASHTABLE_H__
#define __YAC_HASHTABLE_H__

 
class YAC_HashTable : public YAC_Object {
public:
   TKS_ObjectCache varcache; 

public:
   YAC_HashTable  (void);
   ~YAC_HashTable ();
   
   void          YAC_VCALL yacSerialize    (YAC_Object *, sUI);
   sUI           YAC_VCALL yacDeserialize  (YAC_Object *, sUI);
   sBool         YAC_VCALL yacIteratorInit (YAC_Iterator *) const;
   sBool         YAC_VCALL yacArrayAlloc   (sUI _num, sUI,sUI,sUI);
   void          YAC_VCALL yacHashSet      (void *_context, YAC_String*_key, YAC_Value*_value);
   void          YAC_VCALL yacHashGet      (void *_context, YAC_String*_key, YAC_Value*_value);
   void          YAC_VCALL yacHashGetDeref (void *_context, YAC_String*_key, YAC_Value*_value);
   void          YAC_VCALL yacArraySet     (void *_context, sUI _index, YAC_Value *_value);
   void          YAC_VCALL yacArrayGet     (void *_context, sUI _index, YAC_Value *_ret); 
   void          YAC_VCALL yacArrayGetDeref(void *_context, sUI _index, YAC_Value *_ret); 
   ////sUI           YAC_VCALL yacArrayGetNumElements(void) { return num_elements; }
   ////sUI           YAC_VCALL yacArrayGetMaxElements(void) { return max_elements; }
   
   void          YAC_VCALL yacOperator     (sSI,YAC_Object*,YAC_Value*);
   sBool         YAC_VCALL yacToString     (YAC_String *_s) const;
   sBool         YAC_VCALL yacIsComposite  (void) {return 1;}

  public:
   sBool yacToString2 (YAC_String *_s, sBool _bReparsable) const;

  public:
   void addEntriesFrom (YAC_HashTable *_o);

  public:
  
   sSI   _addInt                 (YAC_String *_s, sSI _val);
   sSI   _addFloat               (YAC_String *_s, sF32 _val);
   sSI   _addString              (YAC_String *_s, YAC_String *_val);
   sSI   _addObject              (YAC_String *_s, YAC_Object *_val);
   sSI   _addObjectRef           (YAC_String *_s, YAC_Object *_val);
   void  _get                    (YAC_Object *_s, YAC_Value *_r); 
   void  _getDeref               (YAC_Object *_s, YAC_Value *_r);
   sSI   _exists                 (YAC_String *_key);
   void  _delete                 (YAC_String *_s);
   void  _free                   (void);
   sBool _alloc                  (sUI _cachesize);
   sBool _realloc                (sUI _cachesize);
   sSI   _getNumElements         (void); 
   sSI   _getMaxElements         (void); 
   void  _set                    (YAC_Object *_key, YAC_Object *_value); 
   void  _setInt                 (YAC_Object *_key, sSI _i); 
   void  _setFloat               (YAC_Object *_key, sF32 _f); 
   void  _setObject              (YAC_Object *_key, YAC_Object *_o); 
   void  _setCopy                (YAC_Object *_key, YAC_Object *_o); 
   void  _setString              (YAC_Object *_key, YAC_Object *_s); 
   sSI   _keysToArray            (YAC_Object *_array); 
   sSI   _valuesToArray          (YAC_Object *_array); 
   void  _getString              (YAC_Value *_r) const;
   void  _getReparsableString    (YAC_Value *_r) const;
   sBool _replaceKey             (YAC_Object *_oldKey, YAC_Object *_newKey);
};


#endif // __YAC_HASHTABLE_H__
