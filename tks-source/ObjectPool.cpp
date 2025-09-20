/// ObjectPool.cpp
///
/// (c) 2008-2023 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///
///

#include "tks.h"
#include "Pool.h"
#include "ObjectPool.h"

#ifdef TKS_MT
#include "TKS_Mutex.h"
static TKS_Mutex mtx_pool;
#define Dmtxlock mtx_pool.lock()
#define Dmtxunlock mtx_pool.unlock()
#else
#define Dmtxlock (void)0
#define Dmtxunlock (void)0
#endif // TKS_MT


//#define TKS_OBJECTPOOL_DEBUG defined
//#define TKS_OBJECTPOOL_DEBUG_NEWDELETEPOOL defined
//#define TKS_OBJECTPOOL_DEBUG_NEWDELETE defined
//#define TKS_OBJECTPOOL_DEBUG_SEARCH defined
#define TKS_OBJECTPOOL_DEBUG_EXCEED defined
#define TKS_OBJECTPOOL_DEBUG_EXCEED_ONCE defined

//#define TKS_OBJECTPOOL_PARANOIA defined


#if !defined(TKS_DCON) && defined(TKS_OBJECTPOOL_DEBUG)
#undef TKS_OBJECTPOOL_DEBUG
#endif


sBool ObjectPool::b_initialized = YAC_FALSE;


#define K(a) (a*1024)
//const sUI ObjectPool::chain_sizes[] = { K(4), K(16), K(64), K(128), K(256), K(1024), K(2048), K(4096) }; // kbytes
const sUI ObjectPool::chain_sizes[] = { K(256), K(1024), K(2048), K(4096), K(8192), K(16384), K(24576), K(32768) }; // kbytes
#undef K


sUI  ObjectPool::total_element_data_size_limit          [YAC_NUM_POOL_PRIORITIES];
sUI  ObjectPool::total_element_data_size                [YAC_NUM_POOL_PRIORITIES];
sUI  ObjectPool::freethreshold_total_element_data_size  [YAC_NUM_POOL_PRIORITIES];
sF32 ObjectPool::freethreshold_total_element_data_ratio [YAC_NUM_POOL_PRIORITIES] = { 0.9f, 0.9f, 0.9f };


PoolId       ObjectPool::pool_slots[NUM_POOL_SLOTS][YAC_NUM_POOL_PRIORITIES];
#ifdef TKS_OBJECTPOOL_PURE
ObjectPools *ObjectPool::object_pools;
#else
Pool *       ObjectPool::object_pools;
ObjectPool * ObjectPool::object_pools_data;
#endif // TKS_OBJECTPOOL_PURE

#ifdef TKS_OBJECTPOOL_DEBUG_EXCEED_ONCE
static sBool total_element_data_size_limit_exceeded[YAC_NUM_POOL_PRIORITIES];
static sBool b_pool_chain_limit_once = YAC_FALSE;
#endif // TKS_OBJECTPOOL_DEBUG_EXCEED_ONCE


void ObjectPool::Init(void) {
   sUI i;
   sUI j;

   // Initialize pool slots
   for(j=0; j<YAC_NUM_POOL_PRIORITIES; j++)
   {
      for(i=0; i<NUM_POOL_SLOTS; i++)
      {
         pool_slots[i][j] = NO_ID;
      }

      total_element_data_size_limit [j] = 131072/*2048*/ * 1024; // default limit
      total_element_data_size       [j] = 0;

      freethreshold_total_element_data_size[j] = (sUI) (total_element_data_size_limit[j] * freethreshold_total_element_data_ratio[j]);

#ifdef TKS_OBJECTPOOL_DEBUG_EXCEED_ONCE
      total_element_data_size_limit_exceeded[j] = 0;
#endif // TKS_OBJECTPOOL_DEBUG_EXCEED_ONCE
   }

   // Create pool for the objectpools
   sUI numPools = YAC_NUM_POOL_PRIORITIES * NUM_POOL_SLOTS * MAX_POOL_CHAIN_SIZE;

#ifdef TKS_OBJECTPOOL_DEBUG
   ::fprintf(stderr, "[dbg] ObjectPool::Init: allocating %d object pools.\n", numPools);
#endif // TKS_OBJECTPOOL_DEBUG

#ifdef TKS_OBJECTPOOL_PURE
   object_pools = new ObjectPools();
   object_pools->allocElements(numPools);
#else
   object_pools = new Pool();
   object_pools->allocElements(numPools);
   object_pools_data = new ObjectPool[numPools];

   ObjectPool *pool = object_pools_data;

   for(j=0; j<numPools; j++)
   {
      object_pools->elements[j] = pool++;
   }
#endif // TKS_OBJECTPOOL_PURE

   // Note: do not set b_initialized yet, wait until classes have been registered
}

void ObjectPool::UpdateFreeThresholdRatio(sUI _prio, sF32 _ratio) {
   if(_prio < YAC_NUM_POOL_PRIORITIES)
   {
      if(_ratio < 0.3f)
      {
         _ratio = 0.3f;
      }
      else if(_ratio > 0.99f)
      {
         _ratio = 0.99f;
      }

      freethreshold_total_element_data_size[_prio] = (sUI) (total_element_data_size_limit[_prio] * freethreshold_total_element_data_ratio[_prio]);
   }
}

void ObjectPool::UpdateTotalSizeLimit(sUI _prio, sUI _size) {
   if(_prio < YAC_NUM_POOL_PRIORITIES)
   {
      if(_size > (MAX_POOL_LIMIT*1024))
      {
         _size = MAX_POOL_LIMIT*1024;
      }

      total_element_data_size_limit[_prio] = _size;
      freethreshold_total_element_data_size[_prio] = (sUI) (total_element_data_size_limit[_prio] * freethreshold_total_element_data_ratio[_prio]);
   }
}

void ObjectPool::Exit(void) {
   sUI j;

   // Verify/cleanup pools
   for(j=0; j<YAC_NUM_POOL_PRIORITIES; j++)
   {
      sUI i;

      for(i=0; i<NUM_POOL_SLOTS; i++)
      {
         PoolId poolId = pool_slots[i][j];

         if(NO_ID != poolId)
         {
            // Clean up pool chain
            do
            {
               ObjectPool *pool = (ObjectPool*) (*object_pools)[poolId];

               if(pool->max_elements != pool->free_elements)
               {
#ifdef TKS_DCON
                  sSI numAlive = sSI(pool->max_elements - pool->free_elements);
                  printf("[---] ObjectPool::Exit: %d object%s still alive in pool id=%d size=%d prio=%d.\n",
                         numAlive, (1 == numAlive) ? " is" : "s are",
                         poolId, pool->element_data_size, j
                         );

                  // printf("xxx pool max_elements=%u free_elements=%u\n", pool->max_elements, pool->free_elements);
                  // for(sUI i = 0u; i < pool->free_elements; i++)
                  //    printf("xxx pool freelist[%u]=%u\n", i, pool->freelist[i]);

                  // Print all objects that are still valid
                  sUI i;
                  sUI j = 0;
                  sU8 *ed = pool->element_data;

                  for(i = 0u; i < pool->max_elements; i++)
                  {
                     YAC_Object *o = (YAC_Object*) ed;

                     sBool bAllocated = YAC_TRUE;
                     for(sUI fi = 0u; bAllocated && fi < pool->free_elements; fi++)
                     {
                        bAllocated = bAllocated && (pool->freelist[fi] != (ObjectPoolId)i);
                     }

                     if(bAllocated && YAC_VALID(o))
                     {
                        printf("[---]    %d: %p (class_ID=%d).\n", j, o, o->class_ID);

                        if(YAC_Is_String(o))
                        {
                           YAC_String *t = (YAC_String *)o;
                           if(0 == (t->bflags & (TKS_STRFL_LA))) // (note) linear allocator already shut down at this point
                           {
                              printf("[dbg]       <String> -> \"%s\".\n", ((YAC_String*)o)->chars);
                           }
                           else
                           {
                              printf("[dbg]       <String> -> <linalloc_chars@%p> (gone).\n", t->chars);
                           }
                        }

                        j++;
                     }
                     ed += pool->element_data_size;
                  }
#endif // TKS_DCON
               }

               // //pool->freeElements();
               // //object_pools->qFree(poolId);

               poolId = pool->prev;
            }
            while(NO_ID != poolId);
         }
      }
   }

   // Free object_pools
#ifndef TKS_OBJECTPOOL_PURE
   delete [] object_pools_data; object_pools_data = NULL;
#endif // TKS_OBJECTPOOL_PURE
   delete object_pools; object_pools = NULL;

   b_initialized = YAC_FALSE;
}

void ObjectPool::Print(void) {
   sUI i;
   ::fprintf(stderr, "[dbg] ObjectPool: current pool usage:\n");

   for(i=0; i<YAC_NUM_POOL_PRIORITIES; i++)
   {
      sUI usedSize = 0;
      sUI j;

      for(j=0; j<NUM_POOL_SLOTS; j++)
      {
         PoolId poolId = pool_slots[j][i];

         while(NO_ID != poolId)
         {
            ObjectPool *pool = (ObjectPool*) (*object_pools)[poolId];
            sUI usedElements = pool->max_elements - pool->free_elements;
            usedSize += pool->element_data_size * usedElements;
            poolId = pool->prev;
         }
      }

      ::fprintf(stderr, "[dbg]    priority class %d: %d / %d kbytes allocated, %d kbytes used.\n", 
                i,
                total_element_data_size[i]/1024,
                total_element_data_size_limit[i]/1024,
                usedSize/1024
                );
   }
}


YAC_Object *ObjectPool::New(sUI _clid, sUI _poolHint) {

#ifdef TKS_OBJECTPOOL_PARANOIA
   if(_clid < YAC_MAX_CLASSES)
#endif // TKS_OBJECTPOOL_PARANOIA
   {
      TKS_ClassTemplate *clt = TKSCRIPT__ATOM_SPEEDPATH[_clid];

#ifdef TKS_OBJECTPOOL_PARANOIA
      if(NULL != clt)
#endif // TKS_OBJECTPOOL_PARANOIA
      {
         sUI poolSlot = clt->pool_size;

         if(0 == poolSlot)
         {
            // Class does not allow pooling
            return NULL;
         }

         sUI poolPriority = _poolHint ? YAC_POOL_PRIORITY_HIGH : clt->pool_priority;

         Dmtxlock;

         PoolId poolId = pool_slots[poolSlot][poolPriority];

         ObjectPool *pool;

         if(NO_ID == poolId)
         {
            if( (total_element_data_size[poolPriority] + chain_sizes[0]) <= total_element_data_size_limit[poolPriority] )
            {
               // Create first pool
#ifdef TKS_OBJECTPOOL_PURE
               poolId = object_pools->qAllocPool();
#else
               poolId = object_pools->qAlloc();
#endif // TKS_OBJECTPOOL_PURE
               
#ifdef TKS_OBJECTPOOL_DEBUG_NEWDELETEPOOL
               ::fprintf(stderr, "[dbg] ObjectPool::New: creating new pool for size=%d (bytes) prio=%d with id=%d\n", 
                         clt->pool_size * POOL_GRANULARITY,
                         poolPriority,
                         poolId
                         );
#endif // TKS_OBJECTPOOL_DEBUG_NEWDELETEPOOL
               
               // Remember pool id
               pool_slots[poolSlot][poolPriority] = poolId;
               
               // Initialize ObjectPool
               pool = (ObjectPool*) (*object_pools)[poolId];
               
               pool->initObjectPool(poolId, clt->pool_size * POOL_GRANULARITY, 0, NO_ID, poolPriority);

               // Note: total_element_data_size is increased in initObjectPool()
            }
            else
            {
               // Maximum pool size exceeded, Fall back to malloc() resp. new
#ifdef TKS_OBJECTPOOL_DEBUG_EXCEED
               if(total_element_data_size[poolPriority] > 0)
               {
#ifdef TKS_OBJECTPOOL_DEBUG_EXCEED_ONCE
                  if(!total_element_data_size_limit_exceeded[poolPriority])
                  {
                     total_element_data_size_limit_exceeded[poolPriority] = 1;

                     if(tkscript->configuration.debug_level > 0)
                     {
                        ::fprintf(stderr, "[dbg] ObjectPool::New: Pool limit (%d kbytes) reached for priority class %d.\n",
                                  total_element_data_size_limit[poolPriority]/1024,
                                  poolPriority
                                  );
                     }
                  }
#else
                  if(tkscript->configuration.debug_level > 0)
                  {
                     ::fprintf(stderr, "[dbg] ObjectPool::New: Pool limit (%d kbytes) reached for priority class %d.\n",
                               total_element_data_size_limit[poolPriority]/1024,
                               poolPriority
                               );
                  }
#endif // TKS_OBJECTPOOL_DEBUG_EXCEED_ONCE
               }
#endif // TKS_OBJECTPOOL_DEBUG_EXCEED

               Dmtxunlock;
               return NULL;
            }

         }
         else
         {
            // Found existing pool.
            pool = (ObjectPool*) (*object_pools)[poolId];
         }

         // Pool full ?
         if(0 == pool->free_elements) 
         {
            // Remember last pool in chain
            ObjectPool *lastPool = pool;
            PoolId lastPoolId = poolId;

            // Search pool chain
#ifdef TKS_OBJECTPOOL_DEBUG_SEARCH
            ::fprintf(stderr, "[dbg]     search pool chain: lastPool=%p lastPoolId=%d\n", lastPool, lastPoolId);
#endif // TKS_OBJECTPOOL_DEBUG_SEARCH

            while(NO_ID != pool->prev)
            {
               poolId = pool->prev;
               pool = (ObjectPool*) (*object_pools)[poolId];

#ifdef TKS_OBJECTPOOL_DEBUG_SEARCH
               ::fprintf(stderr, "[dbg]         pool=%p poolId=%d\n", pool, poolId);
#endif //TKS_OBJECTPOOL_DEBUG_SEARCH

               if(pool->free_elements > 0)
               {
#ifdef TKS_OBJECTPOOL_DEBUG_SEARCH
                  ::fprintf(stderr, "[dbg]             -> FOUND\n");
#endif //TKS_OBJECTPOOL_DEBUG_SEARCH
                  break;
               }
            }

            // All pools in chain full ?
            if(0 == pool->free_elements)
            {
               sUI poolChainId = (lastPool->pool_chain_id+1);

               if( poolChainId < MAX_POOL_CHAIN_SIZE )
               {
                  if( (total_element_data_size[poolPriority] + chain_sizes[poolChainId]) <= total_element_data_size_limit[poolPriority] )
                  {
                     // Append new pool to chain
                     
                     // Allocate pool
#ifdef TKS_OBJECTPOOL_PURE
                     poolId = object_pools->qAllocPool();
#else
                     poolId = object_pools->qAlloc();
#endif // TKS_OBJECTPOOL_PURE
                     
#ifdef TKS_OBJECTPOOL_DEBUG_NEWDELETEPOOL
                     ::fprintf(stderr, "[dbg] ObjectPool::New: creating new chained pool for size=%d (bytes) prio=%d chainId=%i with id=%d\n", 
                               clt->pool_size * POOL_GRANULARITY,
                               poolPriority,
                               poolChainId,
                               poolId
                               );
#endif // TKS_OBJECTPOOL_DEBUG_NEWDELETEPOOL

                     lastPool->next = poolId;
                     
                     pool = (ObjectPool*) (*object_pools)[poolId];
                     
                     // Initialize ObjectPool
                     pool->initObjectPool(poolId,
                                          clt->pool_size * POOL_GRANULARITY,
                                          poolChainId,
                                          lastPoolId,
                                          poolPriority
                                          );
                     
                     // Note: total_element_data_size is increased in initObjectPool()

                     // Make it the "frontmost" pool for the given slot
                     pool_slots[poolSlot][poolPriority] = poolId;
                  }
                  else
                  {
                     // Maximum pool size exceeded. Fall back to malloc() resp. new
#ifdef TKS_OBJECTPOOL_DEBUG_EXCEED
#ifdef TKS_OBJECTPOOL_DEBUG_EXCEED_ONCE
                     if(!total_element_data_size_limit_exceeded[poolPriority])
                     {
                        total_element_data_size_limit_exceeded[poolPriority] = 1;

                        if(tkscript->configuration.debug_level > 0)
                        {
                           ::fprintf(stderr, "[dbg] ObjectPool::New: Pool limit (%d kbytes) reached for priority class %d.\n",
                                     total_element_data_size_limit[poolPriority]/1024,
                                     poolPriority
                                     );
                        }
                     }
#else
                     if(tkscript->configuration.debug_level > 0)
                     {
                        ::fprintf(stderr, "[dbg] ObjectPool::New: Pool limit (%d kbytes) reached for priority class %d.\n",
                                  total_element_data_size_limit[poolPriority]/1024,
                                  poolPriority
                                  );
                     }
#endif // TKS_OBJECTPOOL_DEBUG_EXCEED_ONCE
#endif // TKS_OBJECTPOOL_DEBUG_EXCEED
                     Dmtxunlock;
                     return NULL;
                  }
               }
               else
               {
                  // Maximum chain size exceeded (too many objects!). Fall back to malloc() resp. new
#ifdef TKS_OBJECTPOOL_DEBUG_EXCEED
#ifdef TKS_OBJECTPOOL_DEBUG_EXCEED_ONCE
                  if(!b_pool_chain_limit_once)
#endif // TKS_OBJECTPOOL_DEBUG_EXCEED_ONCE
                  ::fprintf(stderr, "[dbg] ObjectPool::New: Pool chain limit reached.\n");
#ifdef TKS_OBJECTPOOL_DEBUG_EXCEED_ONCE
                  b_pool_chain_limit_once = YAC_TRUE;
#endif // TKS_OBJECTPOOL_DEBUG_EXCEED_ONCE
#endif // TKS_OBJECTPOOL_DEBUG_EXCEED
                  Dmtxunlock;
                  return NULL;
               }
            } // all pools in chain full ?
         }

         // Allocate object from pool
#ifdef TKS_OBJECTPOOL_PURE
         ObjectPoolId objectId = pool->qAllocObject();
#else
         PoolId objectId = pool->qAlloc();
#endif // TKS_OBJECTPOOL_PURE
         YAC_Object *r = (YAC_Object*) (*pool)[objectId];

         Dmtxunlock;

#ifdef TKS_OBJECTPOOL_SANITYCHECKS
         if(objectId >= pool->max_elements)
         {
            printf("[!!!] qAllocObject objectId(%d) exceeds max_elements(%d)\n", objectId, pool->max_elements);
         }
#endif // TKS_OBJECTPOOL_SANITYCHECKS

#ifdef TKS_OBJECTPOOL_PARANOIA
         if(YAC_VALID_TAG == r->validation_tag)
         {
            printf("[---] ObjectPool::New: allocated pool object=%p is already alive!!\n");
            // //for(;;);
         }
#endif //TKS_OBJECTPOOL_PARANOIA

         // Install vtable and call constructors
         clt->template_object->yacPoolInit(r);

         // Some classes set a core class ID in their constructors (e.g. FloatArray)
         // so we need to set the class_ID *after* running the c'tors in order to
         // not overwrite it again (e.g. Envelope)
         r->class_ID = _clid;

         // Assign pool handle
         ////r->pool_handle = (YAC_PoolHandle) ( ((((sUI)(poolId+1))&0xFFFF)<<16) | (((sUI)objectId)&0xFFFFF) );
         r->pool_handle.pool_id   = (sUI) (poolId + 1);
         r->pool_handle.object_id = (sUI) objectId;

#ifdef TKS_OBJECTPOOL_DEBUG_NEWDELETE
         ::fprintf(stderr, "[dbg] ObjectPool::New: poolId=%d objectId=%d o=%p\n", poolId, objectId, r);
#endif // TKS_OBJECTPOOL_DEBUG_NEWDELETE

         // Done!
         return r;
      }
#ifdef TKS_OBJECTPOOL_PARANOIA
      else
      {
         Dprintf("[---] ObjectPool::New: illegal class ID %d (no class template found).\n", _clid);
         return NULL;
      }
#endif // TKS_OBJECTPOOL_PARANOIA
   }
#ifdef TKS_OBJECTPOOL_PARANOIA
   else
   {
      // Illegal class ID
      Dprintf("[---] ObjectPool::New: illegal class ID %d.\n", _clid);
      return NULL;
   }
#endif // TKS_OBJECTPOOL_PARANOIA
}


void ObjectPool::Delete(YAC_Object *_o) {

#ifdef TKS_OBJECTPOOL_PARANOIA
   if(_o->pool_handle.pool_id)
#endif // TKS_OBJECTPOOL_PARANOIA
   {
      PoolId         poolId = (PoolId) (_o->pool_handle.pool_id - 1);
#ifdef TKS_OBJECTPOOL_PURE
      ObjectPoolId objectId = (ObjectPoolId) (_o->pool_handle.object_id);
#else
      PoolId       objectId = (PoolId) (_o->pool_handle.object_id);
#endif // TKS_OBJECTPOOL_PURE

#ifdef TKS_OBJECTPOOL_DEBUG_NEWDELETE
      ::fprintf(stderr, "[dbg] ObjectPool::Delete: poolId=%d objectId=%d o=%p\n", poolId, objectId, _o);
#endif // TKS_OBJECTPOOL_DEBUG_NEWDELETE

      // Call destructors
      _o->yacFinalizeObject(NULL);

      Dmtxlock;

      ObjectPool *pool = (ObjectPool*) (*object_pools)[poolId];
#ifdef TKS_OBJECTPOOL_PURE
      pool->qFreeObject(objectId);
#else
      pool->qFree(objectId);
#endif // TKS_OBJECTPOOL_PURE

      if(pool->free_elements == pool->max_elements)
      {
         sUI poolPrio = pool->pool_priority;

         if(0 == pool->pool_chain_id) // Is first pool in chain ?
         {
            if( (pool->pool_data_size >= POOL_DELETE_THRESHOLD) && 
                (total_element_data_size[poolPrio] >= freethreshold_total_element_data_size[poolPrio])
               )
            {
               PoolId *slotAdr = &pool_slots[pool->element_data_size / POOL_GRANULARITY][pool->pool_priority];

               if(*slotAdr == poolId)
               {
                  *slotAdr = NO_ID;
               }

#ifdef TKS_OBJECTPOOL_DEBUG_NEWDELETEPOOL
               ::fprintf(stderr, "[dbg] ObjectPool::Delete: deleting first pool for size=%d (bytes) prio=%d with id=%d\n", 
                         pool->element_data_size,
                         pool->pool_priority,
                         poolId
                         );
#endif // TKS_OBJECTPOOL_DEBUG_NEWDELETEPOOL
               pool->freeObjectPool();
            }
         }
         else
         {
            if( (pool->total_num_allocs >= POOL_DELETE_ALLOC_THRESHOLD) &&
                (total_element_data_size[poolPrio] >= freethreshold_total_element_data_size[poolPrio])
                )
            {
               PoolId *slotAdr = &pool_slots[pool->element_data_size / POOL_GRANULARITY][pool->pool_priority];

               if(*slotAdr == poolId)
               {
                  *slotAdr = pool->prev;
               }

#ifdef TKS_OBJECTPOOL_DEBUG_NEWDELETEPOOL
               ::fprintf(stderr, "[dbg] ObjectPool::Delete: deleting chained pool for size=%d (bytes) (total=%d/%d kb) prio=%d chainId=%d with id=%d\n", 
                         pool->element_data_size,
                         total_element_data_size[pool->pool_chain_id]/1024,
                         total_element_data_size_limit[pool->pool_chain_id]/1024,
                         pool->pool_priority,
                         pool->pool_chain_id,
                         poolId
                         );
#endif // TKS_OBJECTPOOL_DEBUG_NEWDELETEPOOL
               pool->freeObjectPool();
            }
         }
      } // end if pool empty

      Dmtxunlock;
   }
#ifdef TKS_OBJECTPOOL_PARANOIA
   else
   {
      Dprintf("[---] ObjectPool::Delete: object=%p is not assigned to any pool.\n", _o);
   }
#endif // TKS_OBJECTPOOL_PARANOIA
}


ObjectPool::ObjectPool(void) {
   element_data      = NULL;
   element_data_size = 0;
   pool_chain_id     = 0;
   pool_priority     = 0;
   prev              = NO_ID;
   next              = NO_ID;
   self_id           = NO_ID;

#ifdef TKS_OBJECTPOOL_PURE
   freelist         = NULL;
   max_elements     = 0;
   total_num_allocs = 0;
#endif // TKS_OBJECTPOOL_PURE
}


ObjectPool::~ObjectPool() {
   //
   // Note: this is probably never called when the ObjectPool is pooled, too
   //
   if(NULL != element_data)
   {
      delete [] element_data;
      element_data = NULL;
   }
}


void ObjectPool::initObjectPool(PoolId _selfId, sUI _elementDataSize, 
                                sUI _poolChainId, PoolId _prev, 
                                sUI _poolPriority
                                )
{

   self_id           = _selfId;
   element_data_size = _elementDataSize;
   pool_chain_id     = _poolChainId;
   pool_priority     = _poolPriority;
   prev              = _prev;
   next              = NO_ID;

   sUI poolSize = chain_sizes[pool_chain_id] / element_data_size;

#ifndef TKS_OBJECTPOOL_PURE
   allocElements(poolSize);
#endif // TKS_OBJECTPOOL_PURE

   // Allocate element data and initialize element pointers
   pool_data_size = chain_sizes[pool_chain_id];

#ifdef TKS_OBJECTPOOL_PURE
   element_data = new(std::nothrow) sU8[pool_data_size + sizeof(ObjectPoolId)*poolSize];
   max_elements = poolSize;
   freelist = (ObjectPoolId*) (element_data + pool_data_size);

   sUI i;
   for(i=0; i<poolSize; i++)
   {
      freelist[i] = i;
   }

   free_elements = max_elements;
#else
   element_data = new(std::nothrow) sU8[pool_data_size];
   sU8 *d = element_data;

   sUI i;
   for(i=0; i<poolSize; i++)
   {
      elements[i] = (void*) d;
      d += element_data_size;
   }
#endif // TKS_OBJECTPOOL_PURE

   total_element_data_size[pool_priority] += pool_data_size;
}


void ObjectPool::freeObjectPool(void) {

   total_element_data_size[pool_priority] -= pool_data_size;
   
   // Delete actual object data (+ freelist if TKS_OBJECTPOOL_PURE is defined)
   if(NULL != element_data)
   {
      delete [] element_data;
      element_data = NULL;
   }

   // Unlink from pool chain
   if(NO_ID != prev)
   {
      ObjectPool *prevPool = (ObjectPool*) (*object_pools)[prev];
      prevPool->next = next;
   }

   if(NO_ID != next)
   {
      ObjectPool *nextPool = (ObjectPool*) (*object_pools)[next];
      nextPool->prev = prev;

      // Adjust pool_chain_id in succeeding pools
      do 
      {
         nextPool->pool_chain_id--;

         if(NO_ID != nextPool->next)
         {
            nextPool = (ObjectPool*) (*object_pools)[nextPool->next];
         }
         else
         {
            break;
         }
      }
      while(NULL != nextPool);
   }

   // Put self back into free list
#ifdef TKS_OBJECTPOOL_PURE
   object_pools->qFreePool(self_id);
#else
   object_pools->qFree(self_id);
#endif // TKS_OBJECTPOOL_PURE

   prev = next = self_id = NO_ID;

#ifndef TKS_OBJECTPOOL_PURE
   freeElements();
#endif // TKS_OBJECTPOOL_PURE
}


void ObjectPools::allocElements(sUI _maxElements) {
   sUI totalByteSize = _maxElements * sizeof(ObjectPool) + sizeof(ObjectPoolId)*_maxElements;
   pool_data = new(std::nothrow) sU8[totalByteSize];
   memset(pool_data, 0, totalByteSize);

   freelist = (ObjectPoolId*) (pool_data + _maxElements * sizeof(ObjectPool));

   sUI i;
   for(i=0; i<_maxElements; i++)
   {
      freelist[i] = i;
   }

   free_elements = max_elements = _maxElements;
}


void ObjectPools::freeElements(void) {
   if(NULL != pool_data)
   {
      sUI i;
      for(i=0; i<max_elements; i++)
      {
         ObjectPool *p = (ObjectPool*) (pool_data + sizeof(ObjectPool) * i);

         // (note) all objects in the ObjectPool should already be destructed
         //         at this point. Just clean up the pool memory now.

         if(NULL != p->element_data)
         {
            delete [] p->element_data;
            p->element_data = NULL;
         }
      }

      delete [] pool_data;
      pool_data = NULL;
   }
}
