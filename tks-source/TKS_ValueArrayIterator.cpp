/// TKS_ValueArrayIterator.cpp
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "YAC_ValueArray.h"
#include "TKS_ValueArrayIterator.h"


TKS_ValueArrayIterator::TKS_ValueArrayIterator(const YAC_ValueArray *_a) {
   array         = _a;
   current_index = 0;
}

TKS_ValueArrayIterator::~TKS_ValueArrayIterator() {
   array         = NULL;
   current_index = 0;
}

void YAC_VCALL TKS_ValueArrayIterator::getNext(YAC_Value *r) {
   if(current_index<array->num_elements) 
   {
      r->initValue(&array->elements[current_index++]); 
   }
   else
   {
      r->type      = 0xFF;
      r->value.any = NULL;
      r->deleteme  = 0;
   }
}
