/// TKS_FloatArrayIterator.h
///
/// (c) 2001-2010 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __TKS_FLOATARRAYITERATOR_H__
#define __TKS_FLOATARRAYITERATOR_H__


class TKS_FloatArrayIterator : public YAC_Iterator {
public:
   const YAC_FloatArray *array;

public:
   TKS_FloatArrayIterator(const YAC_FloatArray *_ht);
   ~TKS_FloatArrayIterator(); 
   
   void YAC_VCALL getNext(YAC_Value*);
};


#endif // __TKS_FLOATARRAYITERATOR_H__
