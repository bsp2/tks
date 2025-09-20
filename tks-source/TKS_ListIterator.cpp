/// TKS_ListIterator.cpp
///
/// (c) 2001-2006 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"

#include "YAC_Value.h"
#include "YAC_ValueObject.h"
#include "YAC_ListNode.h"
#include "StaticList.h"
#include "YAC_List.h"
#include "TKS_ListIterator.h"


TKS_ListIterator::TKS_ListIterator(const YAC_List*_l) {
   l=_l?_l->head:0;
   lc=l;
}

TKS_ListIterator::~TKS_ListIterator() {
   l = NULL;
}

void YAC_VCALL TKS_ListIterator::getNext(YAC_Value *_r) {
   if(lc)
   {
      _r->initObject((YAC_Object*)lc, 0);
      lc=lc->next;
   }
   else
   {
      _r->type      = 0xFF;
      _r->value.any = NULL;
      _r->deleteme  = 0;
   }
}

