/// YAC_ListNode.h
///
/// (c) 2002-2010 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __YAC_LISTNODE_H__
#define __YAC_LISTNODE_H__


class YAC_ListNode : public YAC_ValueObject {
public:
   YAC_ListNode *next;
   YAC_ListNode *prev; 
    
   sBool b_immutable;
   
public:
   YAC_ListNode  (void);
   ~YAC_ListNode ();
   
   void           YAC_VCALL yacArraySet            (void *_context, sUI _index, YAC_Value *_v);
   void           YAC_VCALL yacArrayGet            (void *_context, sUI _index, YAC_Value *_v); 
   void           YAC_VCALL yacArrayGetDeref       (void *_context, sUI _index, YAC_Value *_v); 
   sUI            YAC_VCALL yacArrayGetNumElements (void); 
   sUI            YAC_VCALL yacArrayGetMaxElements (void); 

   void           YAC_VCALL yacOperator          (sSI, YAC_Object *, YAC_Value *);
   void           YAC_VCALL yacOperatorAssign    (YAC_Object *);
   sBool          YAC_VCALL yacIteratorInit      (YAC_Iterator *) const;
   sBool          YAC_VCALL yacIsComposite       (void) {return 1;}
   sSI            YAC_VCALL yacTensorRank        (void) {return YAC_TENSOR_RANK_VAR;} // Rank depends on current assignment/interpretation (e.g. a ListNode with no successor is a Value)!

   sBool          YAC_VCALL yacToString    (YAC_String*) const;

   void           toDebugString  (YAC_String*);
   void           appendNode     (YAC_ListNode*);
   YAC_ListNode * concatListRef  (YAC_ListNode*);
   YAC_ListNode * concatListGrab (YAC_ListNode*);
   
   YAC_ListNode * getHead          (void);
   YAC_ListNode * getTail          (void);
   YAC_ListNode * getPrev          (void);
   YAC_ListNode * getNext          (void);
   void           freeList         (void);
   YAC_ListNode * appendEmpty      (void);
   YAC_ListNode * append           (YAC_Object *_v); 
   void          _getString        (YAC_Value *_r) const;
   void          _getDebugString   (YAC_Value *_r);
   void          _getDebugStrings  (YAC_Value *_r);
   void          _getCopy          (YAC_Value *_r);
   sUI           _getSize          (void); 
   YAC_Object    *getSubList       (sSI _index);
   
   void          _freeList        (void); 
   YAC_ListNode *_appendEmpty     (void); 
   YAC_ListNode *_appendValue     (YAC_Object *_v); // deprecated 
   YAC_ListNode *_append          (YAC_Object *_v); 
    
};


#endif // __YAC_LISTNODE_H__

