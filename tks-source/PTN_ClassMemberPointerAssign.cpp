/// PTN_ClassMemberPointerAssign.cpp                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             /// PTN_ClassMemberPointerAssign.cpp
///
/// (c) 2001-2013 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#include "tks.h"
#include "tks_inc_class.h"
#include "tks_inc_jit.h"

#include "YAC_HashTable.h"
#include "PTN_DoubleArgExpr.h"
#include "PTN_ClassMemberPointerAssign.h"


PTN_ClassMemberPointerAssign::PTN_ClassMemberPointerAssign( 
                                                           TKS_CachedObject *_co,  
                                                           TKS_CachedObject *_cc_co,  
                                                           PTN_Expr         *_expr 
                                                           )  
{
   member     = _cc_co;
   expr       = _expr; 
   expr_opt   = expr->getEval();
   var        = _co;
   class_id   = TKS_INVALID_SCRIPT_CLASS_ID;
   b_indirect = 0;

   if(var) 
   {
      if(var->type==YAC_TYPE_OBJECT) 
      {
         if(var->value.object_val) 
         {
            class_id=((TKS_ScriptClassInstance*)var->value.object_val)->class_decl->class_id; 
         } 
      } 
   }
}

PTN_ClassMemberPointerAssign::~PTN_ClassMemberPointerAssign() {
   member = NULL;
   var    = NULL;
   TKS_DELETE_SAFE(expr);
}

sBool PTN_ClassMemberPointerAssign::semanticCheck(void) {
   return 
      (member != NULL)                   &&
      (expr ? expr->semanticCheck() : 0) 
      ;
}

void PTN_ClassMemberPointerAssign::optimize(void) {
   tks_optimize_expr(&expr, 0); 
   expr_opt = expr->getEval();
}

sBool PTN_ClassMemberPointerAssign::resolveXRef(void) {
   return expr->resolveXRef();
}

static void PTN_ClassMemberPointerAssign__eval(PTN_Env *_env, const PTN_Statement *_st) { 
   Dtracest;
   const PTN_ClassMemberPointerAssign *st = (const PTN_ClassMemberPointerAssign*)_st; 


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
            YAC_Value r;
            st->expr_opt(_env, &r, st->expr);
            if(Dhaveexception)
            {
               r.unsetFast();
               Dhandleexception(st->expr);
               return;
            }
            robj->setClassMemberPointerValueByID((sUI)st->member->value.int_val, &r);
#ifdef DX_SAFEMODE
         }
      }
#endif 
   }
   else
   {
      Drtthrowinvalidpointer(st, "invalid <this> object", robj);
   } 
}

void PTN_ClassMemberPointerAssign::eval(PTN_Env *_env) const {
   PTN_ClassMemberPointerAssign__eval(_env, this); 
} 

Fevalst PTN_ClassMemberPointerAssign::getEvalSt(void) const {
   return PTN_ClassMemberPointerAssign__eval; 
}
