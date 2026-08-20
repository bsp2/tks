/// TKS_PoolIterator.cpp
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
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
         r->type = YAC_TYPE_INT;
         r->value.int_val = pool->usecopylist[current_index++];
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
