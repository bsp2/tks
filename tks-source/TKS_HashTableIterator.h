/// TKS_HashTableIterator.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef TKS_HASHTABLEITERATOR_H__
#define TKS_HASHTABLEITERATOR_H__


class TKS_HashTableIterator : public YAC_Iterator {
public:
   const YAC_HashTable *ht;

public:
   TKS_HashTableIterator  (const YAC_HashTable *_ht);
   ~TKS_HashTableIterator ();

   void YAC_VCALL getNext (YAC_Value*);
};


#endif // TKS_HASHTABLEITERATOR_H__
