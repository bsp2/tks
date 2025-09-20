/// TKS_PointerArrayIterator.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __TKS_POINTERARRAYITERATOR_H__
#define __TKS_POINTERARRAYITERATOR_H__


class TKS_PointerArrayIterator : public YAC_Iterator {
public:
	const YAC_PointerArray *array;

public:
	TKS_PointerArrayIterator(const YAC_PointerArray *_ht);
	~TKS_PointerArrayIterator();

	void YAC_VCALL getNext(YAC_Value*);

};


#endif // __TKS_POINTERARRAYITERATOR_H__
