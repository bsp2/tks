/// TKS_ListIterator.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __TKS_LISTITERATOR_H__
#define __TKS_LISTITERATOR_H__


class TKS_ListIterator: public YAC_Iterator {
public:
   const YAC_ListNode *l;
   const YAC_ListNode *lc;
   
public:
   TKS_ListIterator(const YAC_List*_l);
   ~TKS_ListIterator();
   
   void YAC_VCALL getNext(YAC_Value *_r);
};


#endif // __TKS_LISTITERATOR_H__
