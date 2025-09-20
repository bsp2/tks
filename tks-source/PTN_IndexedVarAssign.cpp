/// PTN_IndexedVarAssign.cpp
///
/// (c) 2001-2014 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "PTN_Statement.h"
#include "PTN_IndexedVarAssign.h"
#include "vmopenum.h"
#include "VMCore.h"
#include "PTN_DoubleArgExpr.h"


PTN_IndexedVarAssign::PTN_IndexedVarAssign(void) {
   var            = NULL;
   expr           = NULL;
   index_expr     = NULL; 
   expr_opt       = NULL; 
   index_expr_opt = NULL; 
}

PTN_IndexedVarAssign::PTN_IndexedVarAssign(TKS_CachedObject *_var, PTN_Expr *_iexpr, PTN_Expr *_expr) {
   var            = _var;
   expr           = _expr;
   index_expr     = _iexpr; 
   expr_opt       = expr->getEval(); 
   index_expr_opt = index_expr->getEval();
}

PTN_IndexedVarAssign::~PTN_IndexedVarAssign() {
   var = NULL;
   
   TKS_DELETE_SAFE(index_expr);
   
   TKS_DELETE_SAFE(expr);
}

sBool PTN_IndexedVarAssign::semanticCheck(void) {
   return  
      (var) && 
      (index_expr ? index_expr->semanticCheck() : 0) && 
      (expr ? expr->semanticCheck() : 0)
      ;
}

sBool PTN_IndexedVarAssign::resolveXRef(void) { 
   return 
      (index_expr->resolveXRef()) &&
      (expr->resolveXRef())
      ;
} 

void PTN_IndexedVarAssign::optimize(void) {
   tks_optimize_expr(&expr, 0); 
   expr_opt = expr->getEval(); 

   tks_optimize_expr(&index_expr, 0); // keep string indices (hash) 
   index_expr_opt = index_expr->getEval(); 
}

static void PTN_IndexedVarAssign__eval(PTN_Env *_env, const PTN_Statement *_st) {  
   Dtracest;
   const PTN_IndexedVarAssign *st = (const PTN_IndexedVarAssign*)_st; 

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
 
      if(ind.type>=YAC_TYPE_OBJECT)  // hash ?
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
            YAC_Value r; 
            st->expr_opt(_env, &r, st->expr); 
            if(Dhaveexception)
            {
               r.unsetFast();
               Dhandleexception(st->expr);
               return;
            }

            co->value.object_val->yacHashSet((void*)_env->context, ind.value.string_val, &r); 
            r.unsetFast(); 
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
         YAC_Value r; 
         st->expr_opt(_env, &r, st->expr); 
         if(Dhaveexception)
         {
            r.unsetFast();
            Dhandleexception(st->expr);
            return;
         }

         _env->context->exception_default_node = _st
            ;
         co->value.object_val->yacArraySet((void*)_env->context, ind.value.int_val, &r); 

         _env->context->exception_default_node = NULL;

         r.unsetFast(); 
      } 
   } 
   else 
   { 
      Drtthrowinvalidpointer(_st, "cannot index invalid array object", co->value.object_val);
   }
 
} 

void PTN_IndexedVarAssign::eval(PTN_Env *_env) const { 
   PTN_IndexedVarAssign__eval(_env, this); 
} 

Fevalst PTN_IndexedVarAssign::getEvalSt(void) const { 
   return PTN_IndexedVarAssign__eval; 
} 

#ifdef TKS_JIT 
sBool PTN_IndexedVarAssign::forceHybrid(void) { 
   sBool r=expr->forceHybrid()|index_expr->forceHybrid()|(var->type<3); 
   if(!r) 
   { 
      YAC_Object *o=var->value.object_val; 
      if(o&&(o->class_ID!=YAC_CLID_STRING))
      { 
         sSI at=o->yacArrayGetElementType(); 
         if( (at==YAC_TYPE_INT) || (at==YAC_TYPE_FLOAT) )  
         {
            return 0;  
         }
      } 
   } 
   return 1; 
} 

sU8 PTN_IndexedVarAssign::compile(VMCore*_vm) {
   YAC_Object *o=var->value.object_val;
   sU8 r=expr->compile(_vm); if(r>3)  
   { return 0xFF; } // can't handle variable object type 
   switch(o->class_ID) 
   { 
   default: 
      switch(o->yacArrayGetElementType()) 
      { 
      case 1: 
         _vm->typecastStack(r, 1); 
         r=index_expr->compile(_vm); if(r>3) return 0xFF;  
         _vm->typecastStack(r, 1); 
         _vm->selectIntArray(var); 
         Dasmop(VMOP_SIAPOPL); 
         break; 
      case 2: 
         _vm->typecastStack(r, 2); 
         r=index_expr->compile(_vm); if(r==0xFF) return 0xFF; 
         _vm->typecastStack(r, 1); 
         _vm->selectFloatArray(var); 
         Dasmop(VMOP_SIAPOPL); 
         break; 
      default: 
         Dprintf("[---] IndexedVarAssign::compile: unsupported array object type.\n"); 
         return 0xFF; 
      } 
      break; 
      case YAC_CLID_INTARRAY: 
         _vm->typecastStack(r, 1);
         r=index_expr->compile(_vm); if(r>3) return 0xFF; 
         _vm->typecastStack(r, 1); 
         _vm->selectIntArray(var);
         Dasmop(VMOP_SIAPOPL);
         break; 
      case YAC_CLID_FLOATARRAY: 
      case TKS_CLID_ENVELOPE: 
         _vm->typecastStack(r, 2);
         r=index_expr->compile(_vm); if(r==0xFF) return 0xFF; 
         _vm->typecastStack(r, 1); 
         _vm->selectFloatArray(var);
         Dasmop(VMOP_SIAPOPL);
         break; 
         
      case YAC_CLID_CLASSARRAY: 
         if(r!=3) { tkscript->printf("[---] ClassArrayAssign: rvalue must be Class.\n"); return 0xFF; } 
         r=index_expr->compile(_vm); 
         if(r==0xFF) return 0xFF; 
         _vm->typecastStack(r, 1); 
         _vm->pushci((sSI)var); 
         _vm->addAPICall3((sUI)tksvm_classarrayindexassign); 
         Dasmop(VMOP_INCSTP); // discard var 
         Dasmop(VMOP_INCSTP); // discard index 
         Dasmop(VMOP_INCSTP); // discard value 
         break; 
      case YAC_CLID_STRINGARRAY: 
         if(r!=3) { tkscript->printf("[---] StringArrayAssign: rvalue must be String.\n"); return 0xFF; } 
         r=index_expr->compile(_vm); 
         if(r==0xFF) return 0xFF; 
         _vm->typecastStack(r, 1); 
         _vm->pushci((sSI)var); 
         _vm->addAPICall3((sUI)tksvm_stringarrayindexassign); 
         Dasmop(VMOP_INCSTP); // discard var 
         Dasmop(VMOP_INCSTP); // discard index 
         Dasmop(VMOP_INCSTP); // discard value 
         break; 
      case YAC_CLID_POINTERARRAY: 
         if(r!=3) { tkscript->printf("[---] PointerArrayAssign: rvalue must be Object.\n"); return 0xFF; } 
         r=index_expr->compile(_vm); 
         if(r==0xFF) return 0xFF; 
         _vm->typecastStack(r, 1); 
         _vm->pushci((sSI)var); 
         _vm->addAPICall3((sUI)tksvm_pointerarrayindexassign); 
         Dasmop(VMOP_INCSTP); // discard var 
         Dasmop(VMOP_INCSTP); // discard index 
         Dasmop(VMOP_INCSTP); // discard value 
         break; 
      case YAC_CLID_OBJECTARRAY: 
         if(r!=3) { tkscript->printf("[---] ObjectArrayAssign: rvalue must be Object.\n"); return 0xFF; } 
         r=index_expr->compile(_vm); 
         if(r==0xFF) return 0xFF; 
         _vm->typecastStack(r, 1); 
         _vm->pushci((sSI)var); 
         _vm->addAPICall3((sUI)tksvm_objectarrayindexassign); 
         Dasmop(VMOP_INCSTP); // discard var 
         Dasmop(VMOP_INCSTP); // discard index 
         Dasmop(VMOP_INCSTP); // discard value 
         break; 
         
   }
   if(next)
   {
      sU8 r=next->compileHybridStatement(_vm);
      if(r==0xFF)
      {
         return 0xFF;
      }
   }
   return 0;
}
#endif 
