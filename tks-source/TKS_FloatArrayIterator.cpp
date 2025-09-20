/// TKS_FloatArrayIterator.cpp
///
/// (c) 2001-2006 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "YAC_FloatArray.h"
#include "TKS_FloatArrayIterator.h"


TKS_FloatArrayIterator::TKS_FloatArrayIterator(const YAC_FloatArray *_a) {
   array         = _a;
   current_index = 0; 
}

TKS_FloatArrayIterator::~TKS_FloatArrayIterator() {
   array         = NULL;
   current_index = 0;
}

void YAC_VCALL TKS_FloatArrayIterator::getNext(YAC_Value *r) {
   if(current_index<array->num_elements) 
   {
      r->initFloat(array->elements[current_index++]); 
   }
   else
   {
      r->type      = 0xFF;
      r->value.any = NULL;
      r->deleteme  = 0;
   }
}

