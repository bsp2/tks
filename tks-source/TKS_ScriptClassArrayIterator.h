/// TKS_ScriptClassArrayIterator.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __TKS_SCRIPTCLASSARRAYITERATOR_H__
#define __TKS_SCRIPTCLASSARRAYITERATOR_H__


class TKS_ScriptClassArrayIterator : public YAC_Iterator {
public:
	const YAC_ClassArray *array;

public:
	TKS_ScriptClassArrayIterator(const YAC_ClassArray *_a);
	~TKS_ScriptClassArrayIterator();
 
   void YAC_VCALL getNext(YAC_Value*);
};


#endif // __TKS_SCRIPTCLASSARRAYITERATOR_H__
