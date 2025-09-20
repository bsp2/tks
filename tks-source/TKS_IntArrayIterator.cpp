/// TKS_IntArrayIterator.cpp
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///


#include "tks.h"
#include "YAC_IntArray.h"
#include "TKS_IntArrayIterator.h"


TKS_IntArrayIterator::TKS_IntArrayIterator(const YAC_IntArray *_a) {
	array         = _a;
	current_index = 0;
}

TKS_IntArrayIterator::~TKS_IntArrayIterator() {
	array         = NULL;
	current_index = 0;
}

void YAC_VCALL TKS_IntArrayIterator::getNext(YAC_Value *r) {
	if(current_index<array->num_elements) 
   {
		r->initInt(array->elements[current_index++]); 
   }
	else
	{
		r->type=0xFF;
		r->value.int_val=0;
		r->deleteme=0;
	}
}
