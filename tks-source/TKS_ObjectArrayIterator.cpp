/// TKS_ObjectArrayIterator.cpp
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
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

