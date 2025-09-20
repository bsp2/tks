/// TKS_FunctionObject.cpp
///
/// (c) 2005-2023 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "tks_inc_class.h"

#include "YAC_Value.h"
#include "YAC_ValueObject.h"
#include "PTN_Function.h"
#include "TKS_ScriptObjectID.h"
#include "TKS_FunctionObject.h"
#include "TKS_ScriptVariable.h"


TKS_FunctionObject::TKS_FunctionObject(void) {
   fun = NULL;
}

TKS_FunctionObject::~TKS_FunctionObject() {
}

void YAC_VCALL TKS_FunctionObject::yacOperatorAssign(YAC_Object *_robj) {
   // called when optimizing PTN_UnresolvedModuleMemberExpr (PTN_Expr.cpp:tks_optimize_expr())
   if(YAC_BCHK(_robj, TKS_CLID_FUNCTION))
   {
      YAC_CAST_ARG(TKS_FunctionObject, o, _robj);

      fun            = o->fun;
      script_id.s_id = o->script_id.s_id;
   }
}

sBool TKS_FunctionObject::isValid(void) {
   return script_id.isValid();
}

YAC_String *TKS_FunctionObject::getName(void) {
   if(isValid()) 
   {
      if(NULL != fun)
         return &fun->name; // xxx caller could modify internal function name..
   }
   return tks_empty_string; 
}

void TKS_FunctionObject::eval(YAC_Object *_args, YAC_Value *_r) {
   if(NULL != fun)
   {
      sUI numArgs = 0;
      YAC_Value args[16]; // xxx numargs hardcoded 

      if(YAC_VALID(_args))
      {

         numArgs = _args->yacArrayGetNumElements();

         if(numArgs > 0)
         {
            // Convert arguments (if any) to array 
            if(numArgs <= 16)
            {
               sUI i;
               for(i=0; i<numArgs; i++)
               {
                  _args->yacArrayGet(NULL, i, &args[i]);
               }
            }
            else
            {
               Dprintf("[---] Function::eval: maximum number of arguments exceeded (%u > 16).\n", numArgs);
               return;
            }
         }
      }

      YAC_ContextHandle context = tkscript->yacContextGetDefault();

      tkscript->yacEvalFunctionReturn(context, this, numArgs, args, _r);
   }
   else
      _r->initVoid();
}


void TKS_FunctionObject::findVariable(YAC_String *_s, YAC_Value *_r) {
   if(NULL != fun)
   {
      if(isValid())
      {
         TKS_CachedObject *co = fun->findFunVar(_s);
         if(co)
         {
            if(!(co->flags & TKS_CachedObject::ISLOCAL))
            {
               TKS_ScriptVariable *v=(TKS_ScriptVariable*)YAC_NEW_CORE_POOLED(TKS_CLID_VARIABLE);
               v->var       = co;
               v->value.any = co->value.any;
               v->type      = co->type;
               v->deleteme  = 0;
               v->script_id.s_id.id    = script_id.s_id.id;
               v->script_id.s_id.magic = script_id.s_id.magic;
               _r->initObject(v, 1);
               return;
            }
            else
            {
               Dprintf("[---] Function::findVariable: local variables cannot be queried.\n");
            }
            // else xxx need to build stackframe first
         }
      }
   }
   _r->initObject(0, 0);
}
