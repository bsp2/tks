/// PTN_IArrayExpr.cpp
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "PTN_CallableExpr.h"
#include "YAC_HashTable.h"
#include "PTN_IArrayExpr.h"


PTN_IArrayExpr::PTN_IArrayExpr(PTN_Expr *_objexpr, PTN_Expr *_idexpr) {
   obj_expr      = _objexpr;
   index_expr    = _idexpr; 
   obj_expr_opt  = obj_expr->getEval(); 
   index_expr_opt= index_expr->getEval(); 
}

PTN_IArrayExpr::~PTN_IArrayExpr() {
   if(obj_expr)  
   {  
      delete obj_expr; obj_expr = NULL;  
   }
   if(index_expr)  
   {  
      delete index_expr; index_expr = NULL;  
   } 
   iarray_value.unset(); 
}

sBool PTN_IArrayExpr::semanticCheck(void) {
   return (obj_expr?obj_expr->semanticCheck():0)&&(index_expr?index_expr->semanticCheck():0);
}

sBool PTN_IArrayExpr::resolveXRef(void) {
   if(obj_expr)
   {
      if(obj_expr->resolveXRef())
      {
         if(index_expr)
         {
            return index_expr->resolveXRef();
         }
         else
         {
            Dprintf("[---] PTN_IArrayExpr::resolveXRef: missing index_expr.\n");
         }
      }
   }
   else
   {
      Dprintf("[---] PTN_IArrayExpr::resolveXRef: missing obj_expr.\n");
   }
   return 0;
}

void PTN_IArrayExpr::optimize(void) {
   if(obj_expr)  
   { 
      obj_expr->optimize(); 
      obj_expr_opt=obj_expr->getEval(); 
   } 
   
   tks_optimize_expr(&index_expr, 0); 
   index_expr_opt=index_expr->getEval(); 
}

static void PTN_IArrayExpr__eval(PTN_Env *_env, YAC_Value *_r, const PTN_Expr *_st) {
   PTN_IArrayExpr *st = (PTN_IArrayExpr*)_st; // xxx discard const qualifier because of cache vars

   st->iarray_value.unset(); 
   st->obj_expr_opt(_env, &st->iarray_value, st->obj_expr);
   if(Dhaveexception)
   {
      st->iarray_value.unset();
      Dhandleexception(st->obj_expr);
      return;
   }

   if(st->iarray_value.type >= YAC_TYPE_OBJECT)
   { 
      if(TKS_VALID(st->iarray_value.value.object_val)) 
      {
         YAC_Value ir;
         st->index_expr_opt(_env, &ir, st->index_expr); 
         if(Dhaveexception)
         {
            ir.unsetFast();
            Dhandleexception(st->index_expr);
            return;
         }

         if(ir.type >= YAC_TYPE_OBJECT) 
         { 
            if(TKS_VALID(ir.value.object_val)) 
            { 
               if(! YAC_Is_String(ir.value.object_val)) 
               { 
                  YAC_String *s = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
                  ir.value.object_val->yacToString(s); 
                  ir.unsetFast(); 
                  ir.initString(s, 1); 
               } 
               // Get hash value 
               st->iarray_value.value.object_val->yacHashGet((void*)_env->context, ir.value.string_val, _r); 
               ir.unsetFast(); 
               Ddubiousfixstringtype(_r);
            } 
            else 
            { 
               Dprintf("\n[---] PTN_IArrayExpr: hash key is not a valid Object.\n"); 
               Drtthrowinvalidpointer(st, "hash key is not a valid Object", ir.value.object_val);
            } 
            return; 
         } 
         else 
         { 
            // Value is not an object, try to get array element 
            if(YAC_TYPE_FLOAT == ir.type)
            {
               ir.value.int_val=(sSI)ir.value.float_val; 
            }

            _env->context->exception_default_node = st;

            st->iarray_value.value.object_val->yacArrayGet((void*)_env->context, ir.value.int_val, _r);

            _env->context->exception_default_node = NULL;

            Ddubiousfixstringtype(_r);
         } 
      } 
      else 
      { 
         Dprintf("[---] PTN_IArrayExpr: object value ist not a a valid Object.\n"); 
         Drtthrowinvalidpointer(st, "invalid array object", st->iarray_value.value.object_val);
      } 
   } 
   else 
   { 
	   Dprintf("[---] PTN_IArrayExpr: object value ist not an Object.\n"); 
      Drtthrow(st, TKS_EXCEPTION_TYPEMISMATCH, "array value is not an object");
   } 
} 
 
Feval PTN_IArrayExpr::getEval(void) const {
   return PTN_IArrayExpr__eval;
}

void PTN_IArrayExpr::evalCallableExpr(PTN_Env *_env, YAC_Value *_r) const {
   PTN_IArrayExpr__eval(_env, _r, this);
}

void PTN_IArrayExpr::eval(PTN_Env *_env, YAC_Value *_r) const {
   PTN_IArrayExpr__eval(_env, _r, this);
}
