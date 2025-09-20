/// PTN_IndexedVarExpr.cpp
///
/// (c) 2001-2014 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///


#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "PTN_Statement.h"
#include "PTN_IndexedVarExpr.h"
#include "vmopenum.h"
#include "VMCore.h"


PTN_IndexedVarExpr::PTN_IndexedVarExpr(void) {
   cached_object  = NULL;
   index_expr     = NULL; 
   index_expr_opt = NULL;
}

PTN_IndexedVarExpr::PTN_IndexedVarExpr(TKS_CachedObject *_var, PTN_Expr *_expr) {
   cached_object  = _var;
   index_expr     = _expr;
   index_expr_opt = index_expr->getEval(); 
}

PTN_IndexedVarExpr::~PTN_IndexedVarExpr() {
   if(index_expr)
   {
      delete index_expr; index_expr = NULL;
   }
}

sBool PTN_IndexedVarExpr::resolveXRef(void) { 
   if(index_expr)  
   { 
      return index_expr->resolveXRef();  
   }
   return 0; 
} 

sBool PTN_IndexedVarExpr::semanticCheck(void) {
   return  
      (cached_object!=0) &&  
      (index_expr? index_expr->semanticCheck() : 0) 
      ;
}

static void PTN_IndexedVarExpr__eval(PTN_Env *_env, YAC_Value *_r, const PTN_Expr *_st) { 
   Dtracest;
   const PTN_IndexedVarExpr *st = (const PTN_IndexedVarExpr*)_st; 

   YAC_Value *co=Dgetvar(st->cached_object); 
    
   ////tkscript->ptn_error_loc = st->src_loc; 
 
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

      if(ind.type >= YAC_TYPE_OBJECT) 
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
            co->value.object_val->yacHashGet((void*)_env->context, ind.value.string_val, _r); 
            Ddubiousfixstringtype(_r);
            ind.unsetFast(); 
         } 
         else 
         { 
            Drtthrowinvalidpointer(_st, "invalid index object", ind.value.object_val);
         } 
      } 
      else 
      { 
         ind.typecast(YAC_TYPE_INT); 

         _env->context->exception_default_node = _st;

         co->value.object_val->yacArrayGet((void*)_env->context, ind.value.int_val, _r); 

         _env->context->exception_default_node = NULL;

         Ddubiousfixstringtype(_r);
      } 
   } 
   else 
   { 
      Drtthrowinvalidpointer(_st, "cannot index invalid array object", co->value.object_val);

      _r->initVoid(); 
   }  
 
}

void PTN_IndexedVarExpr::eval(PTN_Env *_env, YAC_Value *_r) const { 
   PTN_IndexedVarExpr__eval(_env, _r, this); 
} 

Feval PTN_IndexedVarExpr::getEval(void) const { 
   return PTN_IndexedVarExpr__eval; 
} 

void PTN_IndexedVarExpr::optimize(void) {
   tks_optimize_expr(&index_expr, 0); // keep string indices (hash) 
   index_expr_opt=index_expr->getEval(); 
}

#ifdef TKS_JIT 
sBool PTN_IndexedVarExpr::forceHybrid(void) { 
   sBool r=index_expr->forceHybrid(); 
   if(!r) 
   { 
      YAC_Object *o=cached_object->value.object_val; 
      if(o&&(o->class_ID!=YAC_CLID_STRING)) 
      { 
         sSI at=o->yacArrayGetElementType(); 
         if((at == YAC_TYPE_INT) || (at == YAC_TYPE_FLOAT))  
         {
            return 0;  
         }
      } 
   } 
   return 1; 
} 

sU8 PTN_IndexedVarExpr::compile(VMCore *_vm) { 
   if(!(cached_object->value.object_val))
   {
      Dprintf("[---] PTN_IndexedVarExpr::compile: array variable \"%s\" is NULL..\n",
         (char*)cached_object->name.chars);
      Dcterror(PTNERR_CANTCOMPILE);
      return 0xFF;
   }
   sU8 r=index_expr->compile(_vm);
   if(r!=0xFF)
   {
      _vm->typecastStack(r, 1); 
      return _vm->apush(cached_object);
   }
   return 0xFF;
}
#endif 
