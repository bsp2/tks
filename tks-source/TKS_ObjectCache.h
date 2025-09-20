/// TKS_ObjectCache.h
///
/// (c) 1999-2025 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///

#ifndef TKS_OBJECTCACHE_H__
#define TKS_OBJECTCACHE_H__


class TKS_ObjectCache {
   //
   // a hash table
   //
   //  mostly used to store local/static variables
   //
   //  used by script class instances to map names to members / methods
   //
   //  also see TKS_CachedObject class
   //
public:
   TKS_CachedObject**objects;
   sUI               max_entries; // maximum number of entries
   sUI               num_entries;

public:
   TKS_ObjectCache(void);

#ifdef TKS_USE_NODEALLOCATOR
   virtual
#endif // TKS_USE_NODEALLOCATOR
   ~TKS_ObjectCache();

   sBool             allocCache          (sUI _max_num_of_cache_entries); // find nearest prime number for _max..
   sBool             allocCache2         (sUI _max_num_of_cache_entries); // alloc exactly _max.. entries
   void              freeCache           (sBool _bDebug = YAC_FALSE);

   TKS_CachedObject *findEntry   (YAC_String *_name) const;
   TKS_CachedObject *createEntry (YAC_String *_name, sBool _bCopyName); // re-uses existing entry if present, bCopyName==1: create new "name" string, 0=re-use existing string

   sSI               findIntValueByIndex (sUI _index) const; // -1 if not possible, used to map script class member names
   sSI               findIntValueByName  (YAC_String *_name) const; // -1 if not found

   TKS_CachedObject *createVariantEntry       (YAC_String *_name, sBool _bCopyName);
   TKS_CachedObject *createIntegerEntry       (YAC_String *_name, sBool _bCopyName); // the following 4 methods
   TKS_CachedObject *createFloatEntry         (YAC_String *_name, sBool _bCopyName); // set the type field
   TKS_CachedObject *createStringEntry        (YAC_String *_name, sBool _bCopyName, YAC_String *_allocatedobj, sBool _delpointer);
   TKS_CachedObject *createStringTemplate     (YAC_String *_name, sBool _bCopyName);
   TKS_CachedObject *createStringInstance     (YAC_String *_name, sBool _bCopyName);
   TKS_CachedObject *createAnyEntry           (YAC_String *_name, sBool _bCopyName, void *_allocatedobj);
   TKS_CachedObject *createObjectEntry        (YAC_String *_name, sBool _bCopyName, YAC_Object *_allocatedobj, sBool _delpointer);

   void removeEntry (YAC_String *_name);

#ifdef TKS_FIX_ABSTRACTVARS
   void fixAbstractVars(void);
#endif // TKS_FIX_ABSTRACTVARS

};


#ifdef TKS_USE_NODEALLOCATOR
class TKS_ObjectCache_PN : public TKS_ObjectCache {
public:
   void  operator delete (void*_ptr)    { (void)_ptr; /* do nothing */ }
   void *operator new    (size_t, void *_this) { return _this; }
};
#endif // TKS_USE_NODEALLOCATOR


#endif // TKS_OBJECTCACHE_H__
