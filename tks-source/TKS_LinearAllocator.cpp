/// TKS_LinearAllocator.cpp
///
/// (c) 2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///

#include "tks.h"

#include "TKS_LinearAllocator.h"

TKS_LinearAllocatorStripe::TKS_LinearAllocatorStripe(sUI _stripeSize, TKS_LinearAllocatorStripe *_next)
{
   data_start = (sU8*) malloc(_stripeSize);
   data       = data_start;
   data_end   = data_start + _stripeSize;
   next      = _next;
}

TKS_LinearAllocatorStripe::~TKS_LinearAllocatorStripe() {
   if(NULL != data_start)
   {
      free(data_start);
      data_start = NULL;
      data_end   = NULL;
      data       = NULL;
   }
}

TKS_LinearAllocator::TKS_LinearAllocator(sUI _stripeSize) {
   //printf("xxx alloc node_allocator=%p\n", this);
   root = new(std::nothrow) TKS_LinearAllocatorStripe(_stripeSize, NULL/*next*/);
}

TKS_LinearAllocator::~TKS_LinearAllocator() {
   //printf("xxx free node_allocator=%p\n", this);
   freeStripes();
}

void TKS_LinearAllocator::freeStripes(void) {
   TKS_LinearAllocatorStripe *c = root, *n;
   while(NULL != c)
   {
      //printf("xxx TKS_LinearAllocator::freeStripe c=%p\n", c);
      n = c->next;
      delete c;
      c = n;
   }
   root = NULL;
}

sU8 *TKS_LinearAllocator::alloc(sUI _size) {
   sUI alignSize = (1+(_size>>3))<<3;
   ////if(alignSize > 512) printf("alignSize=%d\n", alignSize);
   for(;;)
   {
      TKS_LinearAllocatorStripe *s = root;
      if( (s->data + alignSize) <= s->data_end)
      {
         sU8 *r = s->data;
         s->data += alignSize;
         return r;
      }
      else
      {
         // Stripe full, add new one
         sUI stripeSize = sUI(s->data_end - s->data_start);
         root = new(std::nothrow) TKS_LinearAllocatorStripe(stripeSize, root);
      }
   }
}

void TKS_LinearAllocator::calcSizes(sUI &num, sUI &max) {
   num = 0;
   max = 0;
   TKS_LinearAllocatorStripe *s = root;
   while(NULL != s)
   {
      num += sUI(s->data - s->data_start);
      max += sUI(s->data_end - s->data_start);
      s = s->next;
   }
}
