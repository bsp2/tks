/// TKS_StringArrayIterator.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __TKS_STRINGARRAYITERATOR_H__
#define __TKS_STRINGARRAYITERATOR_H__


class TKS_StringArrayIterator : public YAC_Iterator {
public:
	const YAC_StringArray *array;

public:
	TKS_StringArrayIterator(const YAC_StringArray *_a);
	~TKS_StringArrayIterator();

  void YAC_VCALL getNext(YAC_Value*);
};


#endif // __TKS_STRINGARRAYITERATOR_H__
