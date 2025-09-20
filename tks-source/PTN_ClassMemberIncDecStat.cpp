/// PTN_ClassMemberIncDecStat.cpp
///
/// (c) 2001-2013 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "tks_inc_class.h"
#include "tks_inc_jit.h"
#include "YAC_HashTable.h"
#include "PTN_ClassMemberIncDecStat.h"


PTN_ClassMemberIncDecStat::PTN_ClassMemberIncDecStat(TKS_CachedObject *_co,  
                                                     TKS_CachedObject *_cc_co,  
                                                     sBool             _dec 
                                                     )  
{
   var        = _co;
   member     = _cc_co;
   dec        = _dec;
   class_id   = TKS_INVALID_SCRIPT_CLASS_ID;
   dec        = _dec;
   b_indirect = 0;
   if(var) 
   {
      if(var->type == YAC_TYPE_OBJECT) 
      {
         if(var->value.object_val) 
         {
            class_id = ((TKS_ScriptClassInstance*)var->value.object_val)->class_decl->class_id; 
         } 
      } 
   }
}

PTN_ClassMemberIncDecStat::~PTN_ClassMemberIncDecStat() {
   var    = NULL;
   member = NULL;
}

sBool PTN_ClassMemberIncDecStat::semanticCheck(void) {
   return 
      (TKS_INVALID_SCRIPT_CLASS_ID != class_id)   &&
      (NULL != member)     
      ;
}

static void PTN_ClassMemberIncDecStat__eval(PTN_Env *_env, const PTN_Statement *_st) { 
   const PTN_ClassMemberIncDecStat *st = (const PTN_ClassMemberIncDecStat*)_st; 
   Dtracest;

   TKS_ScriptClassInstance *robj;
   if(st->b_indirect)
   {
      robj = (TKS_ScriptClassInstance *) _env->context->tksvm_indirect_object;
   }
   else if(st->var)
   {
      robj = (TKS_ScriptClassInstance *) Dgetvar(st->var)->value.object_val;
   }
   else
   {
      robj = _env->context->tksvm_class_stack_object;
   }

   if(TKS_VALID(robj))
   {
#ifdef DX_SAFEMODE 
      if(robj->class_ID==YAC_CLID_CLASS)
      { 
         if(robj->class_decl->ancestor_table[st->class_id])
         { 
#endif
            if(st->dec)
            {
               robj->postDecClassMemberValueByID((sUI)st->member->value.int_val);
            }
            else
            {
               robj->postIncClassMemberValueByID((sUI)st->member->value.int_val);
            }
#ifdef DX_SAFEMODE
         }
         // else classtype mismatch
      }
#endif 
   }
   else
   {
      Drtthrowinvalidpointer(st, "invalid <this> object", robj);
   } 
}

void PTN_ClassMemberIncDecStat::eval(PTN_Env *_env) const { 
   PTN_ClassMemberIncDecStat__eval(_env, this); 
} 

Fevalst PTN_ClassMemberIncDecStat::getEvalSt(void) const { 
   return PTN_ClassMemberIncDecStat__eval; 
} 
