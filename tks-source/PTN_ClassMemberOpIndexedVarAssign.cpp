/// PTN_ClassMemberOpIndexedVarAssign.cpp
///
/// (c) 2001-2013 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "tks_inc_class.h"
#include "tks_inc_jit.h"

#include "YAC_HashTable.h"
#include "PTN_DoubleArgExpr.h"
#include "PTN_ClassMemberOpIndexedVarAssign.h"


PTN_ClassMemberOpIndexedVarAssign::PTN_ClassMemberOpIndexedVarAssign(TKS_CachedObject *_var,  
                                                                     TKS_CachedObject *_member,  
                                                                     PTN_Expr         *_iexpr,  
                                                                     PTN_Expr         *_expr,  
                                                                     sU8               _op 
                                                                     )  
{
   var            = _var;
   member         = _member;
   expr           = _expr;
   index_expr     = _iexpr; 
   expr_opt       = expr->getEval(); 
   index_expr_opt = index_expr->getEval();
   expr_operator  = _op;
   class_id       = TKS_INVALID_SCRIPT_CLASS_ID; 
   
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

PTN_ClassMemberOpIndexedVarAssign::~PTN_ClassMemberOpIndexedVarAssign() {
   var    = NULL;
   member = NULL;
   TKS_DELETE_SAFE(index_expr);
   TKS_DELETE_SAFE(expr);
}

sBool PTN_ClassMemberOpIndexedVarAssign::semanticCheck(void) {
   return (NULL != member)                               && 
          (index_expr ? index_expr->semanticCheck() : 0) && 
          (expr ? expr->semanticCheck() : 0)
          ;
}

void PTN_ClassMemberOpIndexedVarAssign::optimize(void) {
   tks_optimize_expr(&expr, 0); 
   tks_optimize_expr(&index_expr, 0);  // keep string indices (hash)
   expr_opt = expr->getEval(); 
   index_expr_opt = index_expr->getEval(); 
}

sBool PTN_ClassMemberOpIndexedVarAssign::resolveXRef(void) {
   if(index_expr->resolveXRef())
   {
      return expr->resolveXRef();
   }
   else
   {
      return 0;
   }
}

static void PTN_ClassMemberOpIndexedVarAssign__eval(PTN_Env *_env, const PTN_Statement *_st) {  
   Dtracest;
   const PTN_ClassMemberOpIndexedVarAssign *st = (const PTN_ClassMemberOpIndexedVarAssign*)_st;  

   YAC_Value *co=st->var?(Dgetvar(st->var)):0;  
    
   TKS_ScriptClassInstance *robj; 
   if(!co) robj = _env->context->tksvm_class_stack_object; 
   else    robj=(TKS_ScriptClassInstance*)co->value.object_val; 
   if(TKS_VALID(robj)) 
   { 
      if(robj->class_ID==YAC_CLID_CLASS) 
      {  
         if(robj->class_decl->ancestor_table[st->class_id]) 
         {  
            YAC_Value mv; 
            robj->getClassMemberValueByID(&mv, (sUI)st->member->value.int_val); 
            if(mv.type>=YAC_TYPE_OBJECT) 
            { 
               if(TKS_VALID(mv.value.object_val)) 
               {  
                  YAC_Value ind; 
                  st->index_expr_opt(_env, &ind, st->index_expr); 
                  if(Dhaveexception)
                  {
                     ind.unsetFast();
                     Dhandleexception(st->index_expr);
                     return;
                  }

                  _env->context->exception_default_node = _st;

                  if(ind.type>=YAC_TYPE_OBJECT) 
                  { 
                     if(TKS_VALID(ind.value.object_val)) 
                     { 
                        if(! YAC_Is_String(ind.value.object_val)) 
                        { 
                           YAC_String *s = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
                           ind.value.object_val->yacToString(s); 
                           ind.unsetFast(); 
                           ind.initString(s, 1); 
                        } 
                         
                        YAC_Value radd; 
                        st->expr_opt(_env, &radd, st->expr); 
                        if(Dhaveexception)
                        {
                           radd.unsetFast();
                           _env->context->exception_default_node = NULL;
                           Dhandleexception(st->expr);
                           return;
                        }

                        if(st->expr_operator==YAC_OP_ASSIGN) 
                        { 
                           mv.value.object_val->yacHashSet((void*)_env->context, ind.value.string_val, &radd); 
                        } 
                        else 
                        { 
                           YAC_Value r, rold; 
                           mv.value.object_val->yacHashGet((void*)_env->context, ind.value.string_val, &rold); 
                           PTN_DoubleArgExpr::EvalOp(_env, &r, &rold, &radd, st->expr_operator, st); 
                           mv.value.object_val->yacHashSet((void*)_env->context, ind.value.string_val, &r); 
                           r.unsetFast(); 
                           rold.unsetFast(); 
                        } 
                        radd.unsetFast(); 
                        ind.unsetFast(); 
                     } 
                  } 
                  else 
                  { 
                     ind.typecast(YAC_TYPE_INT); 
                     YAC_Value radd; 
                     st->expr_opt(_env, &radd, st->expr); 
                     if(Dhaveexception)
                     {
                        radd.unsetFast();
                        _env->context->exception_default_node = NULL;
                        Dhandleexception(st->expr);
                        return;
                     }

                     if(st->expr_operator==YAC_OP_ASSIGN) 
                     { 
                        mv.value.object_val->yacArraySet((void*)_env->context, ind.value.int_val, &radd); 
                     } 
                     else 
                     { 
                        YAC_Value r, rold; 
                        mv.value.object_val->yacArrayGet((void*)_env->context, ind.value.int_val, &rold); 
                        PTN_DoubleArgExpr::EvalOp(_env, &r, &rold, &radd, st->expr_operator, st); 
                        mv.value.object_val->yacArraySet((void*)_env->context, ind.value.int_val, &r); 
                        r.unsetFast(); 
                        rold.unsetFast(); 
                     } 
                     radd.unsetFast(); 
                  } 

                  _env->context->exception_default_node = NULL;
                   
                  return; 
                   
               } // if member valid  
            } // if member is object 
         } // if compatible class 
      } // if is class 
   } // if valid instance object 

   Drtthrowinvalidpointer(st, "invalid <this> object", robj);
} 

void PTN_ClassMemberOpIndexedVarAssign::eval(PTN_Env *_env) const {
   PTN_ClassMemberOpIndexedVarAssign__eval(_env, this); 
} 

Fevalst PTN_ClassMemberOpIndexedVarAssign::getEvalSt(void) const {
   return PTN_ClassMemberOpIndexedVarAssign__eval;
} 

