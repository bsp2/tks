/// TKS_CachedObject.cpp
///
/// (c) 1999-2023 by Bastian Spiegel <bs@tkscript.de>
///     - published under terms of the GNU general public license (GPL).
///
///

#include "tks.h"
#include "TKS_CachedObject.h"

#include "Pool.h"
#include "ObjectPool.h"

#ifdef TKS_CO_OBJECTCOUNTER
sSI TKS_CachedObject::object_counter = 0;
sSI TKS_CachedObject::refname_counter = 0;
sSI TKS_CachedObject::copyname_counter = 0;
#endif // TKS_CO_OBJECTCOUNTER


TKS_CachedObject::TKS_CachedObject(void) {
#ifdef TKS_CO_OBJECTCOUNTER
   TKS_CachedObject::object_counter++;
#endif // TKS_CO_OBJECTCOUNTER

   flags       = 0u;
   type        = 0u;
   value.any   = NULL;
   deleteme    = YAC_FALSE;
   local_index = 0u;
   b_own_name  = YAC_FALSE;
}

TKS_CachedObject::~TKS_CachedObject() {
#ifdef TKS_CO_OBJECTCOUNTER
   TKS_CachedObject::object_counter--;
#endif // TKS_CO_OBJECTCOUNTER

   unset();
   if(b_own_name)
   {
      //YAC_DELETE(name);
#ifdef TKS_CO_OBJECTCOUNTER
      TKS_CachedObject::copyname_counter--;
#endif // TKS_CO_OBJECTCOUNTER
   }
   else
   {
#ifdef TKS_CO_OBJECTCOUNTER
      TKS_CachedObject::refname_counter--;
#endif // TKS_CO_OBJECTCOUNTER
   }
}

void TKS_CachedObject::initScriptValCO(YAC_Value *_v) {
   copySafe(_v);
   flags = 0u;
}

void TKS_CachedObject::setName(YAC_String *_name, sBool _bCopyName) {
   if(_bCopyName)
   {
      name.yacCopy(_name);
      name.getKey();
      b_own_name = YAC_TRUE;
#ifdef TKS_CO_OBJECTCOUNTER
      TKS_CachedObject::copyname_counter++;
#endif // TKS_CO_OBJECTCOUNTER
   }
   else
   {
      name.visit(_name);
      name.getKey();
#ifdef TKS_CO_OBJECTCOUNTER
      TKS_CachedObject::refname_counter++;
#endif // TKS_CO_OBJECTCOUNTER
   }
}
