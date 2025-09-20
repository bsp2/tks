/// TKS_IntArrayIterator.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __TKS_INTARRAYITERATOR_H__
#define __TKS_INTARRAYITERATOR_H__


class TKS_IntArrayIterator : public YAC_Iterator {
public:
   const YAC_IntArray *array;

public:
   TKS_IntArrayIterator  (const YAC_IntArray *);
   ~TKS_IntArrayIterator ();

   void YAC_VCALL getNext (YAC_Value*);

};


#endif // __TKS_INTARRAYITERATOR_H__
