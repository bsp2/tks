/// PTN_ClassMemberIndexedVarExpr.cpp
///
/// (c) 2001-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "tks_inc_class.h"
#include "tks_inc_jit.h"
#include "YAC_HashTable.h"
#include "PTN_ClassMemberIndexedVarExpr.h"


PTN_ClassMemberIndexedVarExpr::PTN_ClassMemberIndexedVarExpr(TKS_CachedObject *_var,
                                                             TKS_CachedObject *_member,
                                                             PTN_Expr         *_expr
                                                             ) {
   var            = _var;
   member         = _member;
   index_expr     = _expr;
   index_expr_opt = index_expr->getEval();
   class_id       = TKS_INVALID_SCRIPT_CLASS_ID;

   if(var)
   {
      if(YAC_TYPE_OBJECT == var->type)
      {
         if(var->value.object_val)
         {
            class_id = ((TKS_ScriptClassInstance*)var->value.object_val)->class_decl->class_id;
         }
      }
   }
}

PTN_ClassMemberIndexedVarExpr::~PTN_ClassMemberIndexedVarExpr() {
   if(index_expr)
   {
      delete index_expr;
      index_expr = NULL;
   }
   var = 0;
   member = 0;
}

sBool PTN_ClassMemberIndexedVarExpr::resolveXRef(void) {
   if(index_expr)
      return index_expr->resolveXRef();
   return YAC_FALSE;
}

sBool PTN_ClassMemberIndexedVarExpr::semanticCheck(void) {
   return (member!=0)&&(index_expr?index_expr->semanticCheck():0);
}

static void PTN_ClassMemberIndexedVarExpr__eval(PTN_Env *_env, YAC_Value *_r, const PTN_Expr *_st) {
   Dtracest;

   const PTN_ClassMemberIndexedVarExpr*st = (const PTN_ClassMemberIndexedVarExpr*)_st;

   YAC_Value *co=st->var?(Dgetvar(st->var)):0;

   TKS_ScriptClassInstance *robj;
   if(!co)	robj=_env->context->tksvm_class_stack_object;
   else	robj=(TKS_ScriptClassInstance*)co->value.object_val;

   if(TKS_VALID(robj))
   {
#ifdef DX_SAFEMODE
      if(robj->class_ID==YAC_CLID_CLASS)
#endif // DX_SAFEMODE
      {
#ifdef DX_SAFEMODE
         if(robj->class_decl->ancestor_table[st->class_id])
#endif // DX_SAFEMODE
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

                  _env->context->exception_default_node = st;

                  if(ind.type>=YAC_TYPE_OBJECT)
                  {
                     if(TKS_VALID(ind.value.object_val))
                     {
                        if(!YAC_Is_String(ind.value.object_val))
                        {
                           YAC_String *s = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
                           ind.value.object_val->yacToString(s);
                           ind.unsetFast();
                           ind.initString(s, 1);
                        }
                        mv.value.object_val->yacHashGet((void*)_env->context, ind.value.string_val, _r);
                        ind.unsetFast();
                     }
                  }
                  else
                  {
                     ind.typecast(YAC_TYPE_INT);
                     mv.value.object_val->yacArrayGet((void*)_env->context, (sUI)ind.value.int_val, _r);
                  }

                  _env->context->exception_default_node = NULL;

                  return;
               }
               else
               {
                  Drtthrowinvalidpointer(st, "invalid member object", mv.value.object_val);
                  return;
               }
            }
            else
            {
               Drtthrow(st, TKS_EXCEPTION_TYPEMISMATCH, "member is not an object");
               return;
            }
         }
      }
   }
   Drtthrowinvalidpointer(st, "invalid <this> object", robj);
}


Feval PTN_ClassMemberIndexedVarExpr::getEval(void) const {
   return PTN_ClassMemberIndexedVarExpr__eval;
}

void PTN_ClassMemberIndexedVarExpr::eval(PTN_Env *_env, YAC_Value *_r) const {
   PTN_ClassMemberIndexedVarExpr__eval(_env, _r, this);
}

void PTN_ClassMemberIndexedVarExpr::optimize(void) {
   tks_optimize_expr(&index_expr, 0); // keep string indices (hash)
   index_expr_opt = index_expr->getEval();
}
