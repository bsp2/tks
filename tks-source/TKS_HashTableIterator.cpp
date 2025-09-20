/// TKS_HashTableIterator.cpp
///
/// (c) 2001-2022 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "YAC_HashTable.h"
#include "TKS_HashTableIterator.h"


TKS_HashTableIterator::TKS_HashTableIterator(const YAC_HashTable *_a) {
   ht            = _a;
   current_index = 0;
}

TKS_HashTableIterator::~TKS_HashTableIterator() {
   ht            = NULL;
   current_index = 0;
}

void YAC_VCALL TKS_HashTableIterator::getNext(YAC_Value*r) {
   while(current_index<ht->varcache.max_entries)
   {
      TKS_CachedObject *co=ht->varcache.objects[current_index++];
      if(co)
      {
         if(NULL != co->name.chars)
         {
            r->initString(&co->name, 0);
            return;
         }
         // else: entry has been deleted
      }
   }
   r->deleteme = 0;
   r->type     = 0xFF;
}
