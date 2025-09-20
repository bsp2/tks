/// PTN_ConstValReferenceReference.cpp
///
/// (c) 2005-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///


#include "tks.h"
#include "tks_inc_class.h"
#include "tks_inc_jit.h"

#include "PTN_ConstVal.h" 
#include "PTN_ConstValReference.h" 

 
PTN_ConstValReference::PTN_ConstValReference(TKS_CachedObject *_const_value) { 
   const_ref = _const_value;
}

PTN_ConstValReference::~PTN_ConstValReference() { 
} 

sBool PTN_ConstValReference::isConst(void) { 
   return (0 == (const_ref->flags & TKS_CachedObject::ISCLASSCONSTUNINITIALIZED));
} 
 
static void PTN_ConstValReference__eval(PTN_Env *_env, YAC_Value *_r, const PTN_Expr *_st) {  
   Dtracest;
   const PTN_ConstValReference *st = (const PTN_ConstValReference *)_st; 

   if(st->const_ref->type >= YAC_TYPE_OBJECT) 
   { 
      // ---- clone object constant for each reference 
      // ---- to the constant. we don't want the constant 
      // ---- object to be modified. 
      if(YAC_VALID(st->const_ref->value.object_val)) 
      { 
         _r->value.object_val = YAC_CLONE_POOLED(_env->context, st->const_ref->value.object_val); // calls initclass 
         _r->value.object_val->yacOperatorAssign(st->const_ref->value.object_val); 
         _r->deleteme = 1; 
         _r->type = st->const_ref->type; 
      } 
      else 
      { 
         _r->initNull(); 
      } 
   } 
   else 
   { 
      _r->value.any = st->const_ref->value.any; 
      _r->type = st->const_ref->type; 
   } 
}  
 
void PTN_ConstValReference::eval(PTN_Env *_env, YAC_Value *_r) const { 
   PTN_ConstValReference__eval(_env, _r, this);
}

void PTN_ConstValReference::evalConst(YAC_Value *_r) {  
   PTN_Env env; env.initDefault(); // xxx TKS_MT: use *real* thread context
   PTN_ConstValReference__eval(&env, _r, this); 
}  
 

Feval PTN_ConstValReference::getEval(void) const { 
   return PTN_ConstValReference__eval; 
} 

sUI PTN_ConstValReference::getType(void) const { 
   return const_ref->type; 
} 

