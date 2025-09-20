/// PTN_OpIndexedVarAssign.cpp
///
/// (c) 2001-2014 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "PTN_Statement.h"
#include "PTN_OpIndexedVarAssign.h"
#include "vmopenum.h"
#include "VMCore.h"
#include "PTN_DoubleArgExpr.h"


PTN_OpIndexedVarAssign::PTN_OpIndexedVarAssign(void) {
   var           = NULL;
   expr          = NULL;
   index_expr    = NULL;
   expr_operator = 0;
}

PTN_OpIndexedVarAssign::PTN_OpIndexedVarAssign(TKS_CachedObject *_var, PTN_Expr *_iexpr, PTN_Expr *_expr, sU8 _op) {
   var            = _var;
   expr           = _expr;
   index_expr     = _iexpr;
   expr_operator  = _op; 
   index_expr_opt = index_expr->getEval();
}

PTN_OpIndexedVarAssign::~PTN_OpIndexedVarAssign() {
   var = NULL;

   TKS_DELETE_SAFE(index_expr);

   TKS_DELETE_SAFE(expr);
}

sBool PTN_OpIndexedVarAssign::semanticCheck(void) {
   return  
      (0 != expr_operator)                           && 
      (var)                                          && 
      (index_expr ? index_expr->semanticCheck() : 0) && 
      (expr ? expr->semanticCheck() : 0)
      ;
}

sBool PTN_OpIndexedVarAssign::resolveXRef(void) {
   return
      (index_expr->resolveXRef()) &&
      (expr->resolveXRef())
      ;
}

void PTN_OpIndexedVarAssign::optimize(void) {
   tks_optimize_expr(&expr, 0); 
   tks_optimize_expr(&index_expr, 0); // keep string indices (hash) 

   expr_opt       = expr->getEval(); 
   index_expr_opt = index_expr->getEval();  
}

static void PTN_OpIndexedVarAssign__eval(PTN_Env *_env, const PTN_Statement *_st) {
   Dtracest;
   const PTN_OpIndexedVarAssign *st = (const PTN_OpIndexedVarAssign*)_st;  

   YAC_Value *co = Dgetvar(st->var);  
    
   if(TKS_VALID(co->value.object_val)) 
   {  
      YAC_Value ind; 
      st->index_expr_opt(_env, &ind, st->index_expr); 
      if(Dhaveexception)
      {
         ind.unsetFast();
         Dhandleexception(st->index_expr);
         return;
      }
 
      if(ind.type>=YAC_TYPE_OBJECT) 
      { 
         if(TKS_VALID(ind.value.object_val)) 
         { 
             
            if(! YAC_Is_String(ind.value.object_val)) 
            { 
               //
               // Since hash key must (currently) be String, convert Object to new String
               // xxx add support for object->val mapping. (YAC_Object::yacKeyGet, yacKeyGen. default key=obj. address)
               //
               YAC_String *s= (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
               ind.value.object_val->yacToString(s); 
               ind.unset(); 
               ind.initString(s, 1); 
            } 
            YAC_Value radd; 
            st->expr_opt(_env, &radd, st->expr); 
            if(Dhaveexception)
            {
               radd.unsetFast();
               Dhandleexception(st->expr);
               return;
            }

            if(st->expr_operator==YAC_OP_ASSIGN) 
            { 
               co->value.object_val->yacHashSet((void*)_env->context, ind.value.string_val, &radd); 
            } 
            else 
            { 
               YAC_Value r, rold; 
               co->value.object_val->yacHashGet((void*)_env->context, ind.value.string_val, &rold); 
               PTN_DoubleArgExpr::EvalOp(_env, &r, &rold, &radd, st->expr_operator, st); 
               co->value.object_val->yacHashSet((void*)_env->context, ind.value.string_val, &r); 
               r.unsetFast(); 
               rold.unsetFast(); 
            } 
            radd.unsetFast(); 
            ind.unsetFast(); 
         } 
         else 
         { 
            Dprintf("[---] PTN_IndexedVarAssign: index value is not a valid Object.\n"); 
            Drtthrowinvalidpointer(_st, "index value is not a valid Object", ind.value.object_val);
         } 
         return; 
      } 
      else 
      { 
         ind.typecast(YAC_TYPE_INT); 
         YAC_Value radd; 
         st->expr_opt(_env, &radd, st->expr); 
         if(Dhaveexception)
         {
            radd.unsetFast();
            Dhandleexception(st->expr);
            return;
         }

         _env->context->exception_default_node = _st;

         if(st->expr_operator==YAC_OP_ASSIGN) 
         { 
            co->value.object_val->yacArraySet((void*)_env->context, ind.value.int_val, &radd); 
         } 
         else 
         { 
            YAC_Value r, rold; 

            co->value.object_val->yacArrayGet((void*)_env->context, ind.value.int_val, &rold); 

            if(_env->context->b_running)
            {
               PTN_DoubleArgExpr::EvalOp(_env, &r, &rold, &radd, st->expr_operator, st); 
               co->value.object_val->yacArraySet((void*)_env->context, ind.value.int_val, &r); 
               r.unsetFast(); 
            }

            rold.unsetFast(); 
         } 

         radd.unsetFast(); 

         _env->context->exception_default_node = NULL;
      } 
   } 
   else 
   { 
      Drtthrowinvalidpointer(_st, "cannot index invalid object", co->value.object_val);
   } 

} 

Fevalst PTN_OpIndexedVarAssign::getEvalSt(void) const { 
   return PTN_OpIndexedVarAssign__eval; 
} 

void PTN_OpIndexedVarAssign::eval(PTN_Env *_env) const { 
   PTN_OpIndexedVarAssign__eval(_env, this); 
} 

