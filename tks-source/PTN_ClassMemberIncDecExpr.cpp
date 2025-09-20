/// PTN_ClassMemberIncDecExpr.cpp
///
/// (c) 2001-2013 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "tks_inc_class.h"
#include "PTN_ClassMemberExpr.h"
#include "PTN_ClassMemberIncDecExpr.h"


PTN_ClassMemberIncDecExpr::PTN_ClassMemberIncDecExpr(TKS_CachedObject *_co, TKS_CachedObject *_cc_co, sBool _post, sBool _dec) {
   var        = _co;
   member     = _cc_co;
   class_id   = TKS_INVALID_SCRIPT_CLASS_ID;
   post       = _post;
   dec        = _dec;
   b_indirect = 0;

   if(var)
   {
      if(var->type==YAC_TYPE_OBJECT)
      {
         if(var->value.object_val)
         {
            class_id = ((TKS_ScriptClassInstance*)var->value.object_val)->class_decl->class_id;
         }
      }
   }
         
}

PTN_ClassMemberIncDecExpr::~PTN_ClassMemberIncDecExpr() {
   var    = NULL;
   member = NULL;
}

static void PTN_ClassMemberIncDecExpr__eval(PTN_Env *_env, YAC_Value *_r, const PTN_Expr *_st) { 
   const PTN_ClassMemberIncDecExpr*st = (const PTN_ClassMemberIncDecExpr*)_st; 
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
      if(robj->class_ID == YAC_CLID_CLASS)
      { 
         if(robj->class_decl->ancestor_table[st->class_id])
         { 
#endif
            if(st->post)
            {
               if(st->dec)
               { 
                  robj->getPostDecClassMemberValueByID(_r, (sUI)st->member->value.int_val);
                  return;
               }
               else
               {
                  robj->getPostIncClassMemberValueByID(_r, (sUI)st->member->value.int_val);
                  return;
               }
            }
            else
            {
               if(st->dec)
               {
                  robj->getPreDecClassMemberValueByID(_r, (sUI)st->member->value.int_val);
                  return;
               }
               else
               {
                  robj->getPreIncClassMemberValueByID(_r, (sUI)st->member->value.int_val);
                  return;
               }
            } 
#ifdef DX_SAFEMODE 
         }
      }
#endif 
   }
   else
   {
      Drtthrowinvalidpointer(st, "invalid <this> object", robj);
   } 
   _r->initVoid(); 
}

void PTN_ClassMemberIncDecExpr::eval(PTN_Env *_env, YAC_Value *_r) const { 
   PTN_ClassMemberIncDecExpr__eval(_env, _r, this); 
} 

Feval PTN_ClassMemberIncDecExpr::getEval(void) const { 
   return PTN_ClassMemberIncDecExpr__eval; 
} 

#ifdef TKS_JIT 
sBool PTN_ClassMemberIncDecExpr::forceHybrid(void) { 
   return 1; 
} 
#endif  

