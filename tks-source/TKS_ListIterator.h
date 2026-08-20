/// TKS_ListIterator.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef TKS_LISTITERATOR_H__
#define TKS_LISTITERATOR_H__


class TKS_ListIterator: public YAC_Iterator {
public:
   const YAC_ListNode *l;
   const YAC_ListNode *lc;

public:
   TKS_ListIterator(const YAC_List*_l);
   ~TKS_ListIterator();

   void YAC_VCALL getNext(YAC_Value *_r);
};


#endif // TKS_LISTITERATOR_H__
