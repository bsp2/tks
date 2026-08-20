/// TKS_StringArrayIterator.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef TKS_STRINGARRAYITERATOR_H__
#define TKS_STRINGARRAYITERATOR_H__


class TKS_StringArrayIterator : public YAC_Iterator {
public:
	const YAC_StringArray *array;

public:
	TKS_StringArrayIterator(const YAC_StringArray *_a);
	~TKS_StringArrayIterator();

  void YAC_VCALL getNext(YAC_Value*);
};


#endif // TKS_STRINGARRAYITERATOR_H__
