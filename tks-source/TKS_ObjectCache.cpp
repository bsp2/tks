/// TKS_ObjectCache.cpp
///
/// (c) 2001-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///
#include "tks.h"

#include "tks_inc_class.h"
#include "tks_inc_compiler.h"

#include "YAC_ObjectArray.h"
#include "YAC_ClassArray.h"


#define DEFAULT_OBJECTCACHE_SIZE 53


//
// Note: The hash table can hold max_entries-1 elements! one element must always remain NULL
//

TKS_ObjectCache::TKS_ObjectCache(void) {
   max_entries = 0u;
   num_entries = 0u;
   objects     = NULL;
}

TKS_ObjectCache::~TKS_ObjectCache(void) {
   TKS_ObjectCache::freeCache();
}

void TKS_ObjectCache::freeCache(sBool _bDebug) {
   if(objects)
   {
      for(sUI i = 0u; i < max_entries; i++)
      {
         TKS_CachedObject *co = objects[i];
         if(co)
         {
            if(_bDebug) ::printf("TKS_ObjectCache::freeCache: delete co name=\"%s\"\n", (const char*)co->name.chars);
            delete co;
            objects[i] = NULL;
         }
      }
      max_entries = 0u;
      num_entries = 0u;
      delete[] objects;
      objects = NULL;
   }
}


sBool TKS_ObjectCache::allocCache(sUI _cache_size) {
   static sUI prim_sizetable[] = {
      23ul,         53ul,         97ul,        193ul,       389ul,       769ul, /* table taken from dough bagley's simple_hash.h code */
      1543ul,       3079ul,       6151ul,      12289ul,     24593ul,
      49157ul,      98317ul,      196613ul,    393241ul,    786433ul,
      1572869ul,    3145739ul,    6291469ul,   12582917ul,  25165843ul,
      50331653ul,   100663319ul,  201326611ul, 402653189ul, 805306457ul,
      1610612741ul, 3221225473ul, 4294967291ul
   };
   sUI k = 0u;
   while( (k < 29u) && (prim_sizetable[k] < _cache_size) )
      k++;
   return allocCache2(prim_sizetable[k]);
}

sBool TKS_ObjectCache::allocCache2(sUI _cache_size) {
   if(NULL != objects)
      freeCache();

   objects = new(std::nothrow) TKS_CachedObject*[_cache_size];

   if(NULL != objects)
   {
      max_entries = _cache_size;

      // (todo) use memset
      for(sUI i = 0u; i < max_entries; i++)
         objects[i] = NULL;

      return YAC_TRUE;
   }
   return YAC_FALSE;
}

TKS_CachedObject *TKS_ObjectCache::findEntry(YAC_String *_name) const {
   if(_name && objects)
   {
      sUI hk = _name->getKey();
      sUI index = hk % TKS_ObjectCache::max_entries;
      TKS_CachedObject *c = objects[index];
      while(c)
      {
         if(c->name.key == hk) // c->name.key has been set when the entry was put in the cache
         {
            if(c->name.compare(_name))
            {
               // Found existing entry
               return c;
            }
         }
         index = (index + 1u) % TKS_ObjectCache::max_entries;
         c = objects[index];
      }
   }
   return NULL;
}

TKS_CachedObject *TKS_ObjectCache::createEntry(YAC_String *_name, sBool _bCopyName) {
   if(!objects)
   {
      // Allocate cache if this has not been done yet
      allocCache(DEFAULT_OBJECTCACHE_SIZE);
   }
   if(_name)
   {
      sUI hk = _name->getKey();
      sUI index = hk % TKS_ObjectCache::max_entries;
      TKS_CachedObject *c = objects[index];
      while(NULL != c)
      {
         if(c->name.key == hk) // c->name.key has been set when the entry was put in the cache
         {
            if(c->name.compare(_name))
            {
               // Overwrite current entry, first unset its current value
               c->unset();
               return c;
            }
         }
         index = (index + 1u) % TKS_ObjectCache::max_entries;
         c = objects[index];
      }
      if(num_entries == (max_entries - 1u))
      {
         // Could not find existing entry to overwrite and there is no free entry left
         Dprintf("[---] TKS_ObjectCache::createEntry: hash table is full (%d entries)\n", num_entries);
         return NULL;
      }
      // Allocate new entry
      c = new(std::nothrow) TKS_CachedObject();
      if(NULL != c)
      {
         c->setName(_name, _bCopyName);
         objects[index] = c;
         num_entries++;
      }
      return c;
   }
   return NULL;
}

sSI TKS_ObjectCache::findIntValueByName(YAC_String *_name) const {
   if(_name && objects)
   {
      sUI hk = _name->getKey();
      sUI index = hk % TKS_ObjectCache::max_entries;
      TKS_CachedObject *c = objects[index];
      while(c)
      {
         if(c->name.key == hk) // c->name.key has been set when the entry was put in the cache
         {
            if(c->name.compare(_name))
            {
               // Found entry
               return c->value.int_val;
            }
         }
         index = (index + 1u) % TKS_ObjectCache::max_entries;
         c = objects[index];
      }
   }
   return -1;
}

sSI TKS_ObjectCache::findIntValueByIndex(sUI _index) const {
   sUI j = 0u;
   for(sUI i = 0u; i < max_entries; i++)
   {
      TKS_CachedObject *c = objects[i];
      if(c)
      {
         if(j == _index)
            return c->value.int_val;
         j++;
      }
   }
   return -1;
}

// from old "varcache":


TKS_CachedObject *TKS_ObjectCache::createVariantEntry(YAC_String *_name, sBool _bCopyName) {
   TKS_CachedObject *ret = createEntry(_name, _bCopyName);
   if(NULL != ret)
   {
      // The ret->type field is already set (YAC_TYPE_VARIANT == YAC_TYPE_VOID)
      ret->flags = TKS_CachedObject::ISDYNAMIC;
   }
   return ret;
}

TKS_CachedObject *TKS_ObjectCache::createIntegerEntry(YAC_String *_name, sBool _bCopyName) {
   TKS_CachedObject *ret = createEntry(_name, _bCopyName);
   if(NULL != ret)
      ret->type = YAC_TYPE_INT;
   return ret;
}

TKS_CachedObject *TKS_ObjectCache::createFloatEntry(YAC_String *_name, sBool _bCopyName) {
   TKS_CachedObject *ret = createEntry(_name, _bCopyName);
   if(NULL != ret)
      ret->type = YAC_TYPE_FLOAT;
   return ret;
}

TKS_CachedObject *TKS_ObjectCache::createStringEntry(YAC_String *_name, sBool _bCopyName, YAC_String *_obj, sBool _del) {
   TKS_CachedObject *r = createEntry(_name, _bCopyName);

   if(NULL != r)
   {
      r->type             = YAC_TYPE_STRING;
      r->value.string_val = _obj;
      r->deleteme         =_del;
   }

   return r;
}

TKS_CachedObject *TKS_ObjectCache::createStringTemplate(YAC_String *_name, sBool _bCopyName) {
   TKS_CachedObject *r = createEntry(_name, _bCopyName);
   if(NULL != r)
   {
      r->type             = YAC_TYPE_STRING;
      r->value.object_val = TKSCRIPT__ATOM_SPEEDPATH[YAC_CLID_STRING]->template_object;
   }
   return r;
}

TKS_CachedObject *TKS_ObjectCache::createStringInstance(YAC_String *_name, sBool _bCopyName) {
   if(NULL == objects)
   {
      // Allocate cache if this has not been done yet
      allocCache(DEFAULT_OBJECTCACHE_SIZE);
   }

   if(_name)
   {
      sUI hk = _name->getKey();
      sUI index = hk % TKS_ObjectCache::max_entries;
      TKS_CachedObject *c = objects[index];

      while(NULL != c)
      {
         if(c->name.key == hk) // c->name.key has been set when the entry was put in the cache
         {
            if(c->name.compare(_name))
            {
               // Overwrite current entry. Unset its value if it is not an object entry or is not deletable
               if(c->type == YAC_TYPE_STRING)
               {
                  if(c->deleteme)
                  {
                     // Do not reallocate String object, just reset number of used chars
                     c->value.string_val->empty();
                     return c;
                  }
               }
               c->unset();
               c->initEmptyString();
               return c;
            }
         }
         index = (index + 1) % TKS_ObjectCache::max_entries;
         c = objects[index];
      }

      if(num_entries == (max_entries-1))
      {
         // Could not find existing entry to overwrite and there is no free entry left
         return NULL;
      }

      // Allocate new entry
      c = new TKS_CachedObject();
      c->setName(_name, _bCopyName);
      c->initEmptyString();
      objects[index] = c;
      num_entries++;

      return c;
   }
   return NULL;
}

TKS_CachedObject *TKS_ObjectCache::createObjectEntry(YAC_String *_name, sBool _bCopyName, YAC_Object *_obj, sBool _del) {
   TKS_CachedObject *r = createEntry(_name, _bCopyName);
   if(NULL != r)
   {
      r->type             = YAC_TYPE_OBJECT;
      r->value.object_val = _obj;
      r->deleteme         = _del;
   }
   return r;
}

TKS_CachedObject *TKS_ObjectCache::createAnyEntry(YAC_String *_name, sBool _bCopyName, void *_obj) {
   // Used to store e.g. function pointers
   TKS_CachedObject *r = createEntry(_name, _bCopyName);
   if(NULL != r)
   {
      r->type      = YAC_TYPE_INT;
      r->value.any = _obj;
      r->deleteme  = YAC_FALSE;
   }
   return r;
}

#ifdef TKS_FIX_ABSTRACTVARS
void TKS_ObjectCache::fixAbstractVars(void) {
   if(num_entries)
   {
      for(sUI i = 0u; i < max_entries; i++)
      {
         TKS_CachedObject *co = objects[i];
         if(NULL != co)
         {
            sUI coType = co->type;
            if(coType >= YAC_TYPE_OBJECT)
            {
               if(!(co->flags & TKS_CachedObject::ISLOCAL))
               {
                  if(0u == (co->flags & TKS_CachedObject::FIXABSTRACTDONE))
                  {
                     co->flags |= TKS_CachedObject::FIXABSTRACTDONE;
                     if(co->value.object_val)
                     {
                        if(co->flags & TKS_CachedObject::NOINIT) // SomeClass *refVar;
                        {
                           co->safeInitNull();
                           co->type = coType; // keep string type
                        }
                        else if(YAC_CLID_CLASS == co->value.object_val->class_ID)
                        {
                           TKS_ScriptClassInstance *sci = (TKS_ScriptClassInstance*) co->value.object_val;
                           if(sci->class_decl->b_abstract)
                           {
                              co->safeInitNull(); // xxx should never be a deleteable object, actually
                           }
                        }
                     }
                  }
               }
            }
         }
      }
   }

}
#endif // TKS_FIX_ABSTRACTVARS

void TKS_ObjectCache::removeEntry(YAC_String *_name) {
   TKS_CachedObject *co = findEntry(_name);
   if(co)
   {
      co->unset();
      co->name.free();
      num_entries--;
   }
}
