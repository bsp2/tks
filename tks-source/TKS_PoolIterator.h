/// TKS_PoolIterator.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef TKS_POOLITERATOR_H__
#define TKS_POOLITERATOR_H__


class TKS_PoolIterator : public YAC_Iterator {
public:
   const TKS_Pool *pool;

public:
   TKS_PoolIterator  (const TKS_Pool *);
   ~TKS_PoolIterator ();

   void YAC_VCALL getNext (YAC_Value*);
};


#endif // TKS_POOLITERATOR_H__
