/// TKS_ObjectArrayIterator.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __TKS_OBJECTARRAYITERATOR_H__
#define __TKS_OBJECTARRAYITERATOR_H__


class TKS_ObjectArrayIterator : public YAC_Iterator {
public:
   const YAC_ObjectArray *array;

public:
   TKS_ObjectArrayIterator(const YAC_ObjectArray *_ht);
   ~TKS_ObjectArrayIterator();
 
   void YAC_VCALL getNext(YAC_Value*);
};


#endif // __TKS_OBJECTARRAYITERATOR_H__
