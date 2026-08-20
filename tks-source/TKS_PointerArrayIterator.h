/// TKS_PointerArrayIterator.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef TKS_POINTERARRAYITERATOR_H__
#define TKS_POINTERARRAYITERATOR_H__


class TKS_PointerArrayIterator : public YAC_Iterator {
public:
	const YAC_PointerArray *array;

public:
	TKS_PointerArrayIterator(const YAC_PointerArray *_ht);
	~TKS_PointerArrayIterator();

	void YAC_VCALL getNext(YAC_Value*);

};


#endif // TKS_POINTERARRAYITERATOR_H__
