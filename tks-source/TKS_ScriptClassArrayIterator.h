/// TKS_ScriptClassArrayIterator.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef TKS_SCRIPTCLASSARRAYITERATOR_H__
#define TKS_SCRIPTCLASSARRAYITERATOR_H__


class TKS_ScriptClassArrayIterator : public YAC_Iterator {
public:
	const YAC_ClassArray *array;

public:
	TKS_ScriptClassArrayIterator(const YAC_ClassArray *_a);
	~TKS_ScriptClassArrayIterator();

   void YAC_VCALL getNext(YAC_Value*);
};


#endif // TKS_SCRIPTCLASSARRAYITERATOR_H__
