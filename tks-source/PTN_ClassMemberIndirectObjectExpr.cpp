/// PTN_ClassMemberIndirectObjectExpr.cpp
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "tks_inc_class.h"
#include "tks_inc_jit.h"

#include "PTN_ClassMemberIndirectObjectExpr.h"


PTN_ClassMemberIndirectObjectExpr::PTN_ClassMemberIndirectObjectExpr(TKS_CachedObject *_member,  
                                                                     PTN_Expr         *_indirectexpr 
                                                                     )  
{
   member            = _member;
   indirect_expr     = _indirectexpr;
   indirect_expr->initIndirect();
   indirect_expr_opt = indirect_expr->getEval();
}

PTN_ClassMemberIndirectObjectExpr::~PTN_ClassMemberIndirectObjectExpr() {
   if(indirect_expr)
   {
      delete indirect_expr;
      indirect_expr = NULL;
   }
   member = NULL;
}

sBool PTN_ClassMemberIndirectObjectExpr::semanticCheck(void) {
   return  
      (member != NULL) && 
      (indirect_expr ? indirect_expr->semanticCheck() : 0)
      ;
}

sBool PTN_ClassMemberIndirectObjectExpr::resolveXRef(void) {
   if(indirect_expr)  
   { 
      return indirect_expr->resolveXRef(); 
   }
   return 0;
}

void PTN_ClassMemberIndirectObjectExpr::optimize(void) {
   indirect_expr->optimize(); // tks_optimize_expr xxx ? 
   indirect_expr_opt=indirect_expr->getEval(); 
}

static void PTN_ClassMemberIndirectObjectExpr__eval(PTN_Env *_env, YAC_Value *_r, const PTN_Expr *_st) { 
   Dtracest;
   const PTN_ClassMemberIndirectObjectExpr*st = (const PTN_ClassMemberIndirectObjectExpr*)_st; 
   
   YAC_Value mv;
   // ---- find object member in current class ----
   _env->context->tksvm_class_stack_object->getClassMemberValueByID(&mv, (sUI)st->member->value.int_val); 
   if(mv.type >= YAC_TYPE_OBJECT) 
   { 
      if(TKS_VALID(mv.value.object_val)) 
      {
         _env->context->tksvm_indirect_object = mv.value.object_val;

         st->indirect_expr_opt(_env, _r, st->indirect_expr); // object.member++... expressions

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
   // xxx TODO throw type mismatch
   _r->initVoid(); 
}

void PTN_ClassMemberIndirectObjectExpr::eval(PTN_Env *_env, YAC_Value *_r) const { 
   PTN_ClassMemberIndirectObjectExpr__eval(_env, _r, this); 
} 

Feval PTN_ClassMemberIndirectObjectExpr::getEval(void) const { 
   return PTN_ClassMemberIndirectObjectExpr__eval; 
} 
