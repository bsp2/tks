/// TKS_ObjectArrayIterator.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef TKS_OBJECTARRAYITERATOR_H__
#define TKS_OBJECTARRAYITERATOR_H__


class TKS_ObjectArrayIterator : public YAC_Iterator {
public:
   const YAC_ObjectArray *array;

public:
   TKS_ObjectArrayIterator(const YAC_ObjectArray *_ht);
   ~TKS_ObjectArrayIterator();

   void YAC_VCALL getNext(YAC_Value*);
};


#endif // TKS_OBJECTARRAYITERATOR_H__
