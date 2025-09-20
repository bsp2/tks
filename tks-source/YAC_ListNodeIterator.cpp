/// YAC_ListNodeIterator.cpp
///
/// (c) 2003-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"

#include "YAC_Value.h"
#include "YAC_ValueObject.h"
#include "YAC_ListNode.h"
#include "YAC_ListNodeIterator.h"


YAC_ListNodeIterator::YAC_ListNodeIterator(const YAC_ListNode *_l) {
   l  = _l;
   lc = l;
}

YAC_ListNodeIterator::~YAC_ListNodeIterator() {
   l = NULL;
}

void YAC_VCALL YAC_ListNodeIterator::getNext(YAC_Value *_r) {
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

