/// TKS_PointerArrayIterator.cpp
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "YAC_PointerArray.h"
#include "TKS_PointerArrayIterator.h"


TKS_PointerArrayIterator::TKS_PointerArrayIterator(const YAC_PointerArray *_a) {
	array         = _a;
	current_index = 0;
}

TKS_PointerArrayIterator::~TKS_PointerArrayIterator() {
	array         = NULL;
	current_index = 0;
}

void TKS_PointerArrayIterator::getNext(YAC_Value*r) {
	if(current_index<array->num_elements) 
   {
		r->initObject(array->elements[current_index++].value.object_val, 0); 
   }
	else
	{
		r->type      = 0xFF;
		r->value.any = NULL;
		r->deleteme  = 0;
	}
}


