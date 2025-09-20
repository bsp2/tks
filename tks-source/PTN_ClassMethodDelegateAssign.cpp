/// PTN_ClassMethodDelegateAssign.cpp
///
/// (c) 2005-2018 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "tks_inc_class.h"
#include "tks_inc_jit.h"

#include "YAC_HashTable.h"
#include "PTN_DoubleArgExpr.h"
#include "PTN_CallableExpr.h"
#include "TKS_Compiler.h"
#include "PTN_ClassMethodDelegateAssign.h"


PTN_ClassMethodDelegateAssign::PTN_ClassMethodDelegateAssign(PTN_Expr     * _obj_expr,
                                                             PTN_Expr     * _name_expr,
                                                             PTN_Function * _fun
                                                             )
{
   obj_expr        = _obj_expr;
   obj_expr_opt    = NULL;
   name_expr       = _name_expr;
   name_expr_opt   = NULL;
   fun             = _fun;
}

PTN_ClassMethodDelegateAssign::~PTN_ClassMethodDelegateAssign() {
   TKS_DELETE_SAFE(obj_expr);
   TKS_DELETE_SAFE(name_expr);
   fun = NULL;
}

sBool PTN_ClassMethodDelegateAssign::semanticCheck(void) {
   if(obj_expr)
   {
      if(obj_expr->semanticCheck())
      {
         if(name_expr)
         {
            if(name_expr->semanticCheck())
            {
               return (NULL != fun);
            }
         }
      }
   }
   return 0;
}

sBool PTN_ClassMethodDelegateAssign::resolveXRef(void) {
   if(obj_expr)
   {
      if(obj_expr->resolveXRef())
      {
         if(name_expr)
         {
            return name_expr->resolveXRef();
         }
      }
   }
   return 0;
}

void PTN_ClassMethodDelegateAssign::optimize(void) {
   tks_optimize_expr(&obj_expr, YAC_TYPE_OBJECT);
   obj_expr_opt=obj_expr->getEval();
   tks_optimize_expr(&name_expr, YAC_TYPE_STRING);
   name_expr_opt = name_expr->getEval();
}

static void PTN_ClassMethodDelegateAssign__eval(PTN_Env *_env, const PTN_Statement *_st) {
   Dtracest;
   const PTN_ClassMethodDelegateAssign *st = (const PTN_ClassMethodDelegateAssign *)_st;

   YAC_Value vobj;
   st->obj_expr_opt(_env, &vobj, st->obj_expr);
   if(Dhaveexception)
   {
      vobj.unsetFast();
      Dhandleexception(st->obj_expr);
      return;
   }

   if(vobj.type >= YAC_TYPE_OBJECT)
   {
      if(TKS_VALID(vobj.value.object_val))
      {
         if(vobj.value.object_val->class_ID == YAC_CLID_CLASS)
         {
            TKS_ScriptClassInstance *sci=(TKS_ScriptClassInstance *)vobj.value.object_val;

            YAC_Value vname;
            st->name_expr_opt(_env, &vname, st->name_expr);
            if(Dhaveexception)
            {
               vobj.unsetFast();
               vname.unsetFast();
               Dhandleexception(st->name_expr);
               return;
            }

            vname.typecast(YAC_TYPE_STRING);
            if(YAC_TYPE_STRING == vname.type)
            {
               if(TKS_VALID(vname.value.object_val))
               {
                  if(NULL != st->fun->parent_class)
                  {
                     if(sci->class_decl->ancestor_table[st->fun->parent_class->class_id])
                     {
                        sci->setDelegate(vname.value.string_val, st->fun);
                     }
                     else
                     {
                        Drtthrow(_st, TKS_EXCEPTION_SCRIPTCLASSTYPEMISMATCH, "cannot assign delegate method to incompatible script object");
                     }
                  }
                  else
                  {
                     Drtthrow(_st, TKS_EXCEPTION_SCRIPTCLASSTYPEMISMATCH, "cannot assign non-delegate function");
                  }
                  vname.unset();
               }
               else
               {
                  Drtthrowinvalidpointer(st, "cannot assign delegate: invalid name object", vname.value.object_val);
               }
            }
            else
            {
               Dprintf("[---] cannot assign delegate: invalid name object.\n");
               vname.unset();
            }
         }
         else
         {
            Drtthrow(_st, TKS_EXCEPTION_CLASSTYPEMISMATCH, "cannot assign delegate method to c++ object");
         }
         vobj.unset();
      }
      else
      {
         Drtthrowinvalidpointer(st, "cannot assign delegate: invalid object", vobj.value.object_val);
      }
   }
   else
   {
      Dprintf("[---] cannot assign delegate to non-object type value.\n");
   }
}

Fevalst PTN_ClassMethodDelegateAssign::getEvalSt(void) const {
   return PTN_ClassMethodDelegateAssign__eval;
}

void PTN_ClassMethodDelegateAssign::eval(PTN_Env *_env) const {
   PTN_ClassMethodDelegateAssign__eval(_env, this);
}
