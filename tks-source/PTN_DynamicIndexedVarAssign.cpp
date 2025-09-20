/// PTN_DynamicIndexedVarAssign.cpp
///
/// (c) 2001-2014 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "PTN_Statement.h"
#include "PTN_IndexedVarAssign.h"
#include "PTN_DynamicIndexedVarAssign.h"
#include "vmopenum.h"
#include "VMCore.h"

 
PTN_DynamicIndexedVarAssign::PTN_DynamicIndexedVarAssign( 
                                                         TKS_CachedObject *_var,  
                                                         PTN_Expr *_indexexpr,  
                                                         PTN_Expr *_expr 
                                                         )  
                                                         : PTN_IndexedVarAssign(_var, _indexexpr, _expr)
{
}

void PTN_DynamicIndexedVarAssign::optimize(void) { 
   tks_optimize_expr(&expr, 0); 
   tks_optimize_expr(&index_expr, 0); 
   expr_opt = expr->getEval(); 
   index_expr_opt = index_expr->getEval(); 
} 

static void PTN_DynamicIndexedVarAssign__eval(PTN_Env *_env, const PTN_Statement *_st) { 
   Dtracest;
   const PTN_DynamicIndexedVarAssign*st = (const PTN_DynamicIndexedVarAssign*)_st; 

   YAC_Value *co=Dgetvar(st->var); 
   if(co->type >= YAC_TYPE_OBJECT)  
   {
      if(TKS_VALID(co->value.object_val)) 
      { 
         YAC_Value ind; 
         YAC_Value r;  
         st->index_expr_opt(_env, &ind, st->index_expr); 
         if(Dhaveexception)
         {
            ind.unsetFast();
            Dhandleexception(st->index_expr);
            return;
         }

         st->expr_opt(_env, &r, st->expr); 
         if(Dhaveexception)
         {
            ind.unsetFast();
            r.unsetFast();
            Dhandleexception(st->expr);
            return;
         }

         _env->context->exception_default_node = _st;

         if(st->var->value.object_val->class_ID == YAC_CLID_HASHTABLE) 
         { 
            ind.typecast( YAC_TYPE_STRING ); 
            co->value.object_val->yacHashSet((void*)_env->context, ind.value.string_val, &r); 
            ind.unsetFast(); 
         } 
         else 
         { 
            ind.typecast( YAC_TYPE_INT ); 
            r.typecast(co->value.object_val->yacArrayGetElementType()); 
            co->value.object_val->yacArraySet((void*)_env->context, ind.value.int_val, &r); 
            r.unsetFast(); 
            if(Dhaveexception) 
            { 
               _env->context->exception_default_node = NULL;
               Dhandleexception(_st);
            } 
         } 

         _env->context->exception_default_node = NULL;

         return; 
      }
      else
      {
         Drtthrowinvalidpointer(_st, "cannot index invalid object", co->value.object_val);
      }
   }
   else
   {
      Drtthrow(_st, TKS_EXCEPTION_TYPEMISMATCH, "cannot index int/float");
   }

} 

Fevalst PTN_DynamicIndexedVarAssign::getEvalSt(void) const { 
   return PTN_DynamicIndexedVarAssign__eval; 
} 

void PTN_DynamicIndexedVarAssign::eval(PTN_Env *_env) const {
   PTN_DynamicIndexedVarAssign__eval(_env, this); 
} 
