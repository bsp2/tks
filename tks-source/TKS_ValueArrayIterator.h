/// TKS_ValueArrayIterator.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __TKS_VALUEARRAYITERATOR_H__
#define __TKS_VALUEARRAYITERATOR_H__


class TKS_ValueArrayIterator : public YAC_Iterator {
public:
   const YAC_ValueArray *array;
public:
   
   TKS_ValueArrayIterator(const YAC_ValueArray *_va);
   ~TKS_ValueArrayIterator();
    
   void YAC_VCALL getNext(YAC_Value*);
};


#endif // __TKS_VALUEARRAYITERATOR_H__
