/// TKS_IntArrayIterator.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef TKS_INTARRAYITERATOR_H__
#define TKS_INTARRAYITERATOR_H__


class TKS_IntArrayIterator : public YAC_Iterator {
public:
   const YAC_IntArray *array;

public:
   TKS_IntArrayIterator  (const YAC_IntArray *);
   ~TKS_IntArrayIterator ();

   void YAC_VCALL getNext (YAC_Value*);

};


#endif // TKS_INTARRAYITERATOR_H__
