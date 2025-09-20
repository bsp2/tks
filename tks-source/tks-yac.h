/// tks-yac.h
///
/// (c) 2001-2022 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
 

#define YAC_NO_EXPORTS defined 

/// Funny thing is: With MSVC, it's actually slower (5%!) when this is defined. (but why??)
/// Note: this is defined by the GCC based makefiles since it is faster with GCC 3.x resp.
///       same speed with 4.x
//#define TKS_NEWDELETE_INLINE defined

#define YAC_CUST_OBJECT defined 
#define YAC_CUST_STRING defined 
#define YAC_CUST_EVENT defined 
#define YAC_CUST_VALUE defined 
#define YAC_CUST_LISTNODE defined 
#define YAC_CUST_TREENODE defined 
#define YAC_CUST_STREAMBASE defined 
#define YAC_CUST_BUFFER defined 
#define YAC_CUST_INTARRAY defined 
#define YAC_CUST_FLOATARRAY defined 
#define YAC_CUST_NUMBEROBJECTS defined 

#define YAC_TRACK_CHARALLOC defined

// Use absolute epsilon float comparisons
#define YAC_EPSILONCOMPARE_ABS defined
#define YAC_EPSILONCOMPARE_ABS_DEFAULT defined

// Use relative epsilon float comparisons
//#define YAC_EPSILONCOMPARE_REL defined
//#define YAC_EPSILONCOMPARE_REL_DEFAULT defined

#include <yac.h> 

/*#ifdef YAC_OBJECT_POOL
#undef YAC_DELETE
#define YAC_DELETE(a) tkscript->yacDelete(a)
#endif // YAC_OBJECT_POOL */



#ifdef TKS_NEWDELETE_INLINE

#include "Pool.h"
#include "ObjectPool.h"

class TKS_ClassTemplate;
extern TKS_ClassTemplate *tkscript__atom_speedpath[YAC_MAX_CLASSES];


#ifdef YAC_OBJECT_POOL

#define TKSCRIPT__ATOM_SPEEDPATH tkscript__atom_speedpath

/*static inline YAC_Object *yacNewPooledByID__inline (sUI _classID, sUI _poolHint) {
   YAC_Object *o = ObjectPool::New(_classID, _poolHint);
   if(NULL == o)
   {
      return TKSCRIPT__ATOM_SPEEDPATH[_classID]->template_object->yacNewObject();
   }
   return o;
}

static inline void yacDelete__inline(YAC_Object *_o) {
   if(_o)
   {
      if(_o->pool_handle.pool_id)
      {
         ObjectPool::Delete(_o);
      }
      else
      {
         delete _o;
      }
   }
}*/

static inline YAC_Object *yacNewPooledByID__inline (sUI _classID, sUI _poolHint);
static inline void        yacDelete__inline        (YAC_Object *_o);

#else

#define TKSCRIPT__ATOM_SPEEDPATH tkscript->atom_speedpath

/*static inline YAC_Object *yacNewPooledByID__inline(sUI _classId, sUI _poolHint) {
   return TKSCRIPT__ATOM_SPEEDPATH[_classID]->template_object->yacNewObject();
}

static inline void yacDelete__inline(YAC_Object *_o) {
   if(_o) 
   {
      delete _o
   }
}*/

static inline YAC_Object *yacNewByID__inline       (sUI _classID);
static inline YAC_Object *yacNewPooledByID__inline (sUI _classID, sUI _poolHint);
static inline void        yacDelete__inline        (YAC_Object *_o);


#endif // YAC_OBJECT_POOL


#undef YAC_NEW
#define YAC_NEW(a) (_##a*)yacNewByID__inline(clid_##a)

#undef YAC_NEW_CORE
#define YAC_NEW_CORE(c) yacNewByID__inline(c)

#undef YAC_NEW_POOLED
#define YAC_NEW_POOLED(a) (_##a*)yacNewPooledByID__inline(clid_##a, YAC_POOL_HINT_DEFAULT)

#undef YAC_NEW_TEMP
#define YAC_NEW_TEMP(a) (_##a*)yacNewPooledByID__inline(clid_##a, YAC_POOL_HINT_TEMPORARY)

#undef YAC_NEW_CORE_POOLED
#define YAC_NEW_CORE_POOLED(c) yacNewPooledByID__inline(c, YAC_POOL_HINT_DEFAULT)

#undef YAC_NEW_CORE_TEMP
#define YAC_NEW_CORE_TEMP(c) yacNewPooledByID__inline(c, YAC_POOL_HINT_TEMPORARY)

/*#undef YAC_CLONE_POOLED 
#define YAC_CLONE_POOLED(x, a) (a)->yacNewPooled((YAC_ContextHandle)x, YAC_POOL_HINT_DEFAULT)

#undef YAC_CLONE_TEMP
#define YAC_CLONE_TEMP(x, a) (a)->yacNewPooled((YAC_ContextHandle)x, YAC_POOL_HINT_TEMPORARY)
  */

#ifdef YAC_OBJECT_POOL
#undef YAC_DELETE
#define YAC_DELETE(a) yacDelete__inline(a)
#endif // YAC_OBJECT_POOL

#else
#define TKSCRIPT__ATOM_SPEEDPATH tkscript->atom_speedpath

#endif // TKS_NEWDELETE_INLINE


#undef Dyac_throw_def
#define Dyac_throw_def(a, b) tkscript->yacExceptionRaise(yac_host->yacContextGetDefault(), TKS_EXCEPTION_##a, b, __FILE__, __LINE__)



#include "tks-config.h" 



extern YAC_Host *yac_host;
