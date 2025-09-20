/// YAC_List.h
///
/// (c) 2003-2010 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __YAC_LIST_H__
#define __YAC_LIST_H__


class YAC_List : public YAC_Object {
public:
   YAC_ListNode *head;
   YAC_ListNode *tail;
   
public:
   YAC_List (void);
   ~YAC_List ();
   
   sBool          YAC_VCALL yacToString     (YAC_String *) const;
   void           YAC_VCALL yacArraySet     (void *_context, sUI _index, YAC_Value *_v);
   void           YAC_VCALL yacArrayGet     (void *_context, sUI _index, YAC_Value *_v);
   sUI            YAC_VCALL yacArrayGetNumElements(void); 
   sUI            YAC_VCALL yacArrayGetMaxElements(void); 
   void           YAC_VCALL yacOperator     (sSI, YAC_Object *, YAC_Value *);
   sBool          YAC_VCALL yacIteratorInit (YAC_Iterator *) const;
   sBool          YAC_VCALL yacIsComposite  (void) {return 1;}

   void           freeList        (void);
   sBool          compareToObject (YAC_Object *_o); 
   
   void          _getCopy        (YAC_Value *_r);
   sUI           _getSize        (void);
   sSI            isEmpty        (void);
   void          _getString      (YAC_Value *_r) const;
   void          _removeAll      (YAC_Value *_r);
   void          _removeFirst    (YAC_Value *_r);
   void          _removeLast     (YAC_Value *_r); 
   YAC_ListNode * addFirstNew    (void); // helper 
   YAC_ListNode * addLastNew     (void); // helper 
   YAC_Object *  _addFirst       (YAC_Object *_value);
   YAC_Object *  _addFirstInt    (sSI); 
   YAC_Object *  _addFirstFloat  (sF32); 
   YAC_Object *  _addFirstObject (YAC_Object *_oref); 
   YAC_Object *  _addFirstCopy   (YAC_Object *_oref); 
   YAC_Object *  _addFirstString (YAC_Object *_sval); 
   YAC_Object *  _addLast        (YAC_Object *_value); 
   YAC_Object *  _addLastInt     (sSI); 
   YAC_Object *  _addLastFloat   (sF32); 
   YAC_Object *  _addLastObject  (YAC_Object *_oref); 
   YAC_Object *  _addLastCopy    (YAC_Object *_oref); 
   YAC_Object *  _addLastString  (YAC_Object *_sval); 
   YAC_Object *   getHead        (void);
   YAC_Object *   getTail        (void);
   void          _reverse        (void);
   YAC_Object *  _findPointer    (YAC_Object *_ptr);
   void          _remove         (YAC_Object *_ptr, YAC_Value *_r); 
   YAC_Object *  getSubList      (sSI _index); 
   YAC_Object *  _insert         (YAC_Object *_v, YAC_Object *_ln); 
};
 

#endif
