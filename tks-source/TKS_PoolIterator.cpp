/// TKS_PoolIterator.cpp
///
/// (c) 2001-2010 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#include "tks.h"
#include "Pool.h"
#include "TKS_Pool.h"
#include "TKS_PoolIterator.h"

 
TKS_PoolIterator::TKS_PoolIterator(const TKS_Pool *_pool) {
   pool          = _pool;
   current_index = 0;
   ((TKS_Pool*)pool)->copyUseList(); // xxx not thread safe
}

TKS_PoolIterator::~TKS_PoolIterator() {
   pool = NULL;
}

void YAC_VCALL TKS_PoolIterator::getNext(YAC_Value*r) {
   if(pool->usecopylist)
   {
      if(pool->usecopylist[current_index]>=0)
      {
         r->type=1;
         r->value.int_val=pool->usecopylist[current_index++];
      }
      else
      {
         r->type      = 0xFF;
         r->value.any = NULL;
         r->deleteme  = 0;
      }
   }
   else
   {
      r->type      = 0xFF;
      r->value.any = NULL;
      r->deleteme  = 0;
   }
}
