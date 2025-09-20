/// ObjectPool.h
/// 
/// (c) 2008-2013 Bastian Spiegel <bs@tkscript.de> 
///     - distributed under terms of the GNU general public license (GPL). 
/// 

#ifndef __OBJECTPOOL_H__
#define __OBJECTPOOL_H__


typedef sSI ObjectPoolId;


/// Define to not use the Pool class for allocations (it also has a uselist and does a bit more error checking)
/// The "pure" version has the advantage that only one malloc is required for element data + freelist.
/// Speed wise is can make a big difference because every function call (cycle!??) counts!
#define TKS_OBJECTPOOL_PURE defined


/// Define to enable some additional sanity checks (only useful for debugging)
//#define TKS_OBJECTPOOL_SANITYCHECKS defined


#ifdef TKS_OBJECTPOOL_PURE
class ObjectPools;
class ObjectPool {
#else
class ObjectPool : public Pool {
#endif // TKS_OBJECTPOOL_PURE
public:
   enum { MAX_POOL_CHAIN_SIZE = 8 }; // maximum number of chained pool

   enum { MAX_POOL_LIMIT = 32768 };  // max. kbytes used for pooling (per priority class)
   
   enum { POOL_GRANULARITY = 8/*sizeof(sUI)*/ };
   enum { NUM_POOL_SLOTS = (1024 / POOL_GRANULARITY) }; // 1024=max element size

   static const sUI chain_sizes[MAX_POOL_CHAIN_SIZE]; // "grow" table (kbyte per slot)

   enum { POOL_DELETE_THRESHOLD = 32768 }; // (numbytes) When to delete a pool even if it's the first one in the chain
   ////enum { POOL_DELETE_THRESHOLD = 262144 }; // (numbytes) When to delete a pool even if it's the first one in the chain

   enum { POOL_DELETE_ALLOC_THRESHOLD = 16 }; // minimum number of allocations in a pool until it is auto-purged (to avoid "jittering")

   enum { NO_ID = -1 };

   static sBool b_initialized;
   
   static sUI total_element_data_size_limit           [YAC_NUM_POOL_PRIORITIES]; // max. allowed size per priority
   static sUI total_element_data_size                 [YAC_NUM_POOL_PRIORITIES]; // current total size per priority
   static sUI freethreshold_total_element_data_size   [YAC_NUM_POOL_PRIORITIES]; // when to start freeing pools
   static sF32 freethreshold_total_element_data_ratio [YAC_NUM_POOL_PRIORITIES]; // used to initialize array above

   static PoolId pool_slots[NUM_POOL_SLOTS][YAC_NUM_POOL_PRIORITIES]; // always points to last ObjectPool in pool chain

#ifdef TKS_OBJECTPOOL_PURE
   static ObjectPools *object_pools;
#else
   static Pool *      object_pools;
   static ObjectPool *object_pools_data;
#endif // TKS_OBJECTPOOL_PURE

   static void        Init   (void);
   static void        UpdateFreeThresholdRatio(sUI _prio, sF32 _ratio);
   static void        UpdateTotalSizeLimit(sUI _prio, sUI _size);
   static void        Exit   (void);
   static YAC_Object *New    (sUI _clid, sUI _poolHint);
   static void        Delete (YAC_Object *_o);
   static void        Print  (void);

public:
#ifdef TKS_OBJECTPOOL_PURE
   ObjectPoolId *freelist;
   sUI           free_elements;
   sUI           total_num_allocs;
   sUI           max_elements;
#endif // TKS_OBJECTPOOL_PURE

   PoolId prev;               // pool chain
   PoolId next;               // 
   PoolId self_id;
   sUI    pool_chain_id;      // 0..MAX_POOL_CHAIN_SIZE-1, first pool does not free element_data
   sUI    pool_priority;
   sUI    pool_data_size;     // Must remember allocated size since pool_chain_id is adjusted when previous pool is deleted

   sU8 *  element_data;       // Holds element_data_size * max_elements bytes
   sUI    element_data_size;

#ifdef TKS_OBJECTPOOL_PURE
#ifndef YAC_64
   sUI pad48; // 
   sUI pad52; // 
   sUI pad56; // 
   sUI pad60; // to align class size to 64 bytes
#else
   sUI pad1;  //
   sUI pad2;  // to align class size to 64 bytes
#endif // YAC_64
#endif // TKS_OBJECTPOOL_PURE

public:
   ObjectPool(void);
   ~ObjectPool();

   void initObjectPool (PoolId _selfId, sUI _elementDataSize, sUI _poolChainId, PoolId _prev, sUI _poolPriority);
   void freeObjectPool (void);

#ifdef TKS_OBJECTPOOL_PURE
   inline ObjectPoolId qAllocObject(void) {
      ////if(free_elements) // free_elements>0 has already been checked 
      {
         ObjectPoolId id = freelist[--free_elements];
         total_num_allocs++;
         return id;
      }
      ////return NO_ID;
   }

   inline void qFreeObject(ObjectPoolId _id) {
#ifdef TKS_OBJECTPOOL_SANITYCHECKS
      if(_id >= max_elements)
      {
         printf("xxx qFreeObject: id(%d) exceeds max_elements(%d)\n", _id, max_elements);
      }
#endif // TKS_OBJECTPOOL_SANITYCHECKS
      freelist[free_elements++] = _id;
   }

   inline void *operator [](ObjectPoolId _id) {
      return (void*) (element_data + element_data_size * _id);
   }
#endif // TKS_OBJECTPOOL_PURE

};

#ifdef TKS_OBJECTPOOL_PURE
class ObjectPools {
public:
   ObjectPools(void) { }
   ~ObjectPools() { freeElements(); }

#ifdef TKS_OBJECTPOOL_PURE
   ObjectPoolId *freelist;
   sUI           free_elements;
   sU8 *         pool_data;       // Holds sizeof(ObjectPool) * _maxElements bytes
   sUI           max_elements; // for debugging
#endif // TKS_OBJECTPOOL_PURE

   void allocElements(sUI _maxElements);
   void freeElements(void);

   inline ObjectPoolId qAllocPool(void) {
      if(free_elements)
      {
         ObjectPoolId id = freelist[--free_elements];
         return id;
      }

      return ObjectPool::NO_ID; // should never happen!
   }

   inline void qFreePool(ObjectPoolId _id) {
      freelist[free_elements++] = _id;
   }

   inline void *operator [](ObjectPoolId _id) {
      return (void*) (pool_data + sizeof(ObjectPool) * _id);
   }
};
#endif // TKS_OBJECTPOOL_PURE


#endif  // __OBJECTPOOL_H__
