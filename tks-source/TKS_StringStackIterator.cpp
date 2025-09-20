/// TKS_StringStackIterator.cpp
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#include "tks.h"
#include "YAC_String.h"
#include "TKS_StringIterator.h"
#include "TKS_StringStackIterator.h"



TKS_StringStackIterator::TKS_StringStackIterator(const YAC_String *_s) {
	it_string = _s;
}

TKS_StringStackIterator::~TKS_StringStackIterator() {
	it_string = NULL;
}

void YAC_VCALL TKS_StringStackIterator::begin(void) {
	si.begin(it_string);
	cont_it = si.tail()!=0;
}

void YAC_VCALL TKS_StringStackIterator::end(void) {
	si.end();
}

void YAC_VCALL TKS_StringStackIterator::getNext(YAC_Value *_r) {
	if(cont_it)
	{
		_r->initString(si.current, 0);
		cont_it=si.previous()!=0;
	}
	else
	{
		_r->type      = 0xFF;
		_r->value.any = NULL;
		_r->deleteme  = 0;
	}
}
