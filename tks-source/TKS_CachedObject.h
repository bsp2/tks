/// TKS_CachedObject.h
///
/// (c) 1999-2023 Bastian Spiegel <bs@tkscript.de>
///    - distributed under terms of the GNU general public license (GPL).
///

#ifndef __TKS_CACHEDOBJECT_H__
#define __TKS_CACHEDOBJECT_H__

class TKS_ObjectCache;

#define CO_TYPEMASK 3

#define Dcochars(x) ((char*)x->name.chars)

class TKS_CachedObject : public YAC_Value {
   //
   // a named value
   //
   //  also see TKS_ObjectCache class 
   //  
   //
public:
#ifdef TKS_CO_OBJECTCOUNTER
   static sSI object_counter;
   static sSI refname_counter;
   static sSI copyname_counter;
#endif // TKS_CO_OBJECTCOUNTER

public:
   enum _entry_typesandflags {
      //FL_DELPOINTER=(1<<2),
      ISDYNAMIC    = (1<<4), // variable has variable type
      ISLOCAL      = (1<<6), // is local variable (stackframes)
      TAGGED       = (1<<7), // see "tag" keyword, used by serialization
      PTRASSIGN    = (1<<8), // used to optimize "MyClass c<=...;"
      NOINIT       = (1<<9), // used to optimize "MyClass *ref"
      ISCLASSCONSTUNINITIALIZED = (1<<10), // used to flag class const values that have not been assigned, yet
      FIXABSTRACTDONE = (1 << 11),  // see TKS_ObjectCache::fixAbstractVars(). used for incremental updates / dynamic TSL loader.

      FL_PAD
   };
   YAC_String name;
   sBool      b_own_name; // xxx should be a flag but the flags are currently sometimes set absolutely..(which would cause memleaks)
   sUI        flags;
   sUI        local_index; // ISLOCAL flag => stackframe index 

public:
   TKS_CachedObject(void);

#ifdef TKS_USE_NODEALLOCATOR
   virtual
#endif // TKS_USE_NODEALLOCATOR
   ~TKS_CachedObject();

   void initScriptValCO (YAC_Value *);
   void setName         (YAC_String *_name, sBool _bCopyName);

};


#ifdef TKS_USE_NODEALLOCATOR
class TKS_CachedObject_PN : public TKS_CachedObject {
public:
   void  operator delete (void*_ptr)    { (void)_ptr; /* do nothing */ }
   void *operator new    (size_t, void *_this) { return _this; }
};
#endif // TKS_USE_NODEALLOCATOR


#endif // __TKS_CACHEDOBJECT_H__
