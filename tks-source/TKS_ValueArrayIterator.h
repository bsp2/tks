/// TKS_ValueArrayIterator.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef TKS_VALUEARRAYITERATOR_H__
#define TKS_VALUEARRAYITERATOR_H__


class TKS_ValueArrayIterator : public YAC_Iterator {
public:
   const YAC_ValueArray *array;
public:

   TKS_ValueArrayIterator(const YAC_ValueArray *_va);
   ~TKS_ValueArrayIterator();

   void YAC_VCALL getNext(YAC_Value*);
};


#endif // TKS_VALUEARRAYITERATOR_H__
