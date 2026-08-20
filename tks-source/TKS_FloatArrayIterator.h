/// TKS_FloatArrayIterator.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef TKS_FLOATARRAYITERATOR_H__
#define TKS_FLOATARRAYITERATOR_H__


class TKS_FloatArrayIterator : public YAC_Iterator {
public:
   const YAC_FloatArray *array;

public:
   TKS_FloatArrayIterator(const YAC_FloatArray *_ht);
   ~TKS_FloatArrayIterator();

   void YAC_VCALL getNext(YAC_Value*);
};


#endif // TKS_FLOATARRAYITERATOR_H__
