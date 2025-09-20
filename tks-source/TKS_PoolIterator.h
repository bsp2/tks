/// TKS_PoolIterator.h
///
/// (c) 2001-2010 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __TKS_POOLITERATOR_H__
#define __TKS_POOLITERATOR_H__


class TKS_PoolIterator : public YAC_Iterator {
public:
   const TKS_Pool *pool;
   
public:
   TKS_PoolIterator  (const TKS_Pool *);
   ~TKS_PoolIterator ();

   void YAC_VCALL getNext (YAC_Value*);
};


#endif // __TKS_POOLITERATOR_H__
