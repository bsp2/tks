/// TKS_HashTableIterator.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __TKS_HASHTABLEITERATOR_H__
#define __TKS_HASHTABLEITERATOR_H__
 

class TKS_HashTableIterator : public YAC_Iterator {
public:
   const YAC_HashTable *ht;

public:
   TKS_HashTableIterator  (const YAC_HashTable *_ht);
   ~TKS_HashTableIterator ();
 
   void YAC_VCALL getNext (YAC_Value*);
};
 

#endif // __TKS_HASHTABLEITERATOR_H__
