/// TKS_StringArrayIterator.cpp
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "YAC_StringArray.h"
#include "TKS_StringArrayIterator.h"


TKS_StringArrayIterator::TKS_StringArrayIterator(const YAC_StringArray *_a) {
	array         = _a;
	current_index = 0;
}

TKS_StringArrayIterator::~TKS_StringArrayIterator() {
	array         = NULL;
	current_index = 0;
}

void YAC_VCALL TKS_StringArrayIterator::getNext(YAC_Value*r) {
	if(current_index<array->num_elements) 
   {
		r->initString(&array->elements[current_index++], 0); 
   }
	else
	{
		r->type      = 0xFF;
		r->value.any = NULL;
		r->deleteme  = 0;
	}
}

