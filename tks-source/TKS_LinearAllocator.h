/// TKS_LinearAllocator.h
///
/// (c) 2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///

#ifndef __TKS_LINEARALLOCATOR_H__
#define __TKS_LINEARALLOCATOR_H__


class TKS_LinearAllocatorStripe {
public:
   sU8 *data_start;
   sU8 *data;
   sU8 *data_end;

   TKS_LinearAllocatorStripe *next;

public:
   TKS_LinearAllocatorStripe(sUI _size, TKS_LinearAllocatorStripe *_next);
   ~TKS_LinearAllocatorStripe();
};


class TKS_LinearAllocator {
   // An alloc-only allocator
   //  used for parser tree nodes
public:
   TKS_LinearAllocatorStripe *root;

public:
   TKS_LinearAllocator(sUI _stripeSize);
   ~TKS_LinearAllocator();

private:
   void freeStripes(void);

public:
   sU8 *alloc (sUI _size);

   void calcSizes (sUI &_retNum, sUI &_retMax);
};



#endif // __TKS_LINEARALLOCATOR_H__
