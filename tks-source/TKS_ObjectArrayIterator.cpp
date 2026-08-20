/// TKS_ObjectArrayIterator.cpp
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#include "tks.h"
#include "YAC_ObjectArray.h"
#include "TKS_ObjectArrayIterator.h"


TKS_ObjectArrayIterator::TKS_ObjectArrayIterator(const YAC_ObjectArray *_a) {
   array         = _a;
   current_index = 0;
}

TKS_ObjectArrayIterator::~TKS_ObjectArrayIterator() {
   array         = NULL;
   current_index = 0;
}

void YAC_VCALL TKS_ObjectArrayIterator::getNext(YAC_Value *r) {
   if(current_index<array->num_elements)
   {
      r->initObject(array->elements[current_index++], 0);
   }
   else
   {
      r->type      = 0xFF;
      r->value.any = NULL;
      r->deleteme  = 0;
   }
}
