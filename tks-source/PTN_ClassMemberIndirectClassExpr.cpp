/// PTN_ClassMemberIndirectClassExpr.cpp
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "tks_inc_class.h"
#include "tks_inc_jit.h"
#include "YAC_HashTable.h"
#include "PTN_ClassMemberIndirectClassExpr.h"

 
PTN_ClassMemberIndirectClassExpr::PTN_ClassMemberIndirectClassExpr(TKS_CachedObject *_member,  
                                                                   PTN_Expr *_indirectexpr 
                                                                   )  
{
   member            = _member;
   indirect_expr     = _indirectexpr;
   _indirectexpr->initIndirect();
   indirect_expr_opt = indirect_expr->getEval();
}

PTN_ClassMemberIndirectClassExpr::~PTN_ClassMemberIndirectClassExpr() {
   if(indirect_expr)
   {
      delete indirect_expr;
      indirect_expr = NULL;
   }
   member = NULL;
}

sBool PTN_ClassMemberIndirectClassExpr::semanticCheck(void) {
   return 
      (member != NULL) &&
      (indirect_expr ? indirect_expr->semanticCheck() : 0)
      ;
}

sBool PTN_ClassMemberIndirectClassExpr::resolveXRef(void) {
   if(indirect_expr)
   {
      return indirect_expr->resolveXRef();
   }
   return 0;
}

void PTN_ClassMemberIndirectClassExpr::optimize(void) {
   if(indirect_expr)
   {
      indirect_expr->optimize(); 
      indirect_expr_opt=indirect_expr->getEval(); 
   }
}

static void PTN_ClassMemberIndirectClassExpr__eval(PTN_Env *_env, YAC_Value *_r, const PTN_Expr *_st) { 
   Dtracest;

   const PTN_ClassMemberIndirectClassExpr*st = (const PTN_ClassMemberIndirectClassExpr*)_st; 
   
   YAC_Value mv;
   // ---- find class member in current class ----
   _env->context->tksvm_class_stack_object->getClassMemberValueByID(&mv, (sUI)st->member->value.int_val);
   if(mv.type >= YAC_TYPE_OBJECT)
   {
      if(TKS_VALID(mv.value.object_val))
      { 
         _env->context->tksvm_indirect_object = mv.value.object_val;

         st->indirect_expr_opt(_env, _r, st->indirect_expr);

         if(Dhaveexception)
         {
            _r->unset();
            Dhandleexception(st->indirect_expr);
            return;
         }
         return;
      }
      else
      {
         Drtthrowinvalidpointer(st, "invalid class member object", mv.value.object_val);
      }
   } 
   _r->initVoid(); 
}

void PTN_ClassMemberIndirectClassExpr::eval(PTN_Env *_env, YAC_Value *_r) const { 
   PTN_ClassMemberIndirectClassExpr__eval(_env, _r, this); 
} 

Feval PTN_ClassMemberIndirectClassExpr::getEval(void) const { 
   return PTN_ClassMemberIndirectClassExpr__eval; 
} 
