/// PTN_FunctionECall.cpp
///
/// (c) 2001-2025 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "PTN_Statement.h"
#include "vmopenum.h"
#include "VMCore.h"
#include "PTN_CallableExpr.h"
#include "PTN_ArgListNode.h"
#include "PTN_ArgList.h"
#include "PTN_ArgVar.h"
#include "PTN_ArgVarList.h"
#include "TKS_FormalArgument.h"
#include "TKS_ScriptObjectID.h"
#include "TKS_FunctionObject.h"
#include "PTN_Function.h"
#include "PTN_FunctionECall.h"
#include "PTN_Call.h"
#include "PTN_FunctionCall.h"

#include "tks_inc_class.h"
#include "tks_inc_compiler.h"

#include "TKS_Mutex.h"

PTN_FunctionECall::PTN_FunctionECall(void) {
   fun         = NULL;
   arglist     = NULL;
}

PTN_FunctionECall::PTN_FunctionECall(PTN_Function *_fun, PTN_ArgList *_args) {
   fun         = _fun;
   arglist     = _args;
}

sUI PTN_FunctionECall::getID(void) const {
   return PTN_FUNCTIONECALL;
}

PTN_FunctionECall::~PTN_FunctionECall() {
   if(fun)
   {
      fun = NULL;
   }
   if(arglist)
   {
      delete arglist;
      arglist = NULL;
   }
}

sBool PTN_FunctionECall::SemanticCheck(PTN_Function *fun, PTN_ArgList *arglist, sUI src_loc) {
   if(arglist)
   {
      if(fun->argvarlist)
      {
         if(arglist->num_args == fun->argvarlist->num_vars)
         {
            return arglist->semanticCheck();
         }
         else
         {
            Dprintf("\n[---] wrong number of arguments to function \"%s\" (need=%i, have=%i)\n",
                    fun->name.chars,
                    fun->argvarlist->num_vars,
                    arglist->num_args
                    );
         }
      }
      else
      {
         Dprintf("\n[---] wrong number of arguments to function \"%s\" (need=0, have=%i)\n",
                 fun->name.chars,
                 arglist->num_args
                 );
      }
   }
   else
   {
      if(NULL == fun->argvarlist)
      {
         return YAC_TRUE;
      }
      else
      {
         Dprintf("\n[---] missing arguments for function/method \"%s\" call (need %i)\n",
                 fun->name.chars,
                 fun->argvarlist->num_vars
                 );
      }
   }
   Dprintf("[---] in module \"%s\", line %i.\n",
           Dtkscompiler->getModuleNameByIndex(Dsrcloc_module(src_loc)),
           Dsrcloc_linenr(src_loc)
           );
   return YAC_FALSE;
}

sBool PTN_FunctionECall::semanticCheck(void) {
   if(fun)
   {
      return PTN_FunctionECall::SemanticCheck(fun, arglist, src_loc);
   }
   else
   {
      return YAC_FALSE;
   }
}

// ---- evaluate function call with local variable stackframe
static void PTN_FunctionECall__eval(PTN_Env *_env, YAC_Value *_r, const PTN_Expr *_st) {
   Dtracest;
   const PTN_FunctionECall *st = (const PTN_FunctionECall*)_st;

   Dcondbegincallsiteprofile;

   Dbeginfuncall(st->fun);

   YAC_Value *nsf;
   if((nsf = _env->context->pushStackFrame(st->fun))!=0) // get expected call ("displaced") stackframe
   {
      YAC_Value *stackc = _env->context->tks_fstackc;

      PTN_Env env(_env);

#ifdef DX_TRACEFUNCTIONCALLS
      env.context->pushFunctionCall(st->fun, _st->src_loc);
#endif
      if(st->arglist) // SemanticCheck() ensures that function expects args
      {
         st->fun->setArgsByList(&env, st->arglist, nsf);
      }
      env.context->pushStackFrame2(st->fun); // really change tks_fstackc

      st->fun->evalCall(&env, _r);
      env.context->popStackFrame(st->fun);
      env.context->tks_fstackc = stackc;

#ifdef DX_TRACEFUNCTIONCALLS
      env.context->popFunctionCall();
#endif
   }
   else
   {
      _r->initVoid();
   }

   Dendfuncall(st->fun);

   Dcondendcallsiteprofile(st->src_loc);
}

// ---- evaluate function _without_ local variable stackframe
static void PTN_FunctionECall__eval_nsf(PTN_Env *_env, YAC_Value *_r, const PTN_Expr *_st) {
   Dtracest;
   const PTN_FunctionECall *st = (const PTN_FunctionECall*)_st;

   Dcondbegincallsiteprofile;

   Dbeginfuncall(st->fun);

   {
      PTN_Env env(_env);

#ifdef DX_TRACEFUNCTIONCALLS
      env.context->pushFunctionCall(st->fun, _st->src_loc);
#endif
      if(st->arglist) // SemanticCheck() ensures that function expects args
      {
         st->fun->setArgsByList(&env, st->arglist, 0/*nsf*/);
      }

      st->fun->evalCall(&env, _r);

#ifdef DX_TRACEFUNCTIONCALLS
   _env->context->popFunctionCall();
#endif
   }

   Dendfuncall(st->fun);

   Dcondendcallsiteprofile(st->src_loc);

}

Feval PTN_FunctionECall::getEval(void) const {
   if(0u == fun->num_local_vars)
   {
      return PTN_FunctionECall__eval_nsf;
   }
   else
   {
      return PTN_FunctionECall__eval;
   }
}

void PTN_FunctionECall::evalCallableExpr(PTN_Env *_env, YAC_Value *_r) const {
   eval(_env, _r);
}

void PTN_FunctionECall::eval(PTN_Env *_env, YAC_Value *_r) const {
   if(0u == fun->num_local_vars)
   {
      PTN_FunctionECall__eval_nsf(_env, _r, this);
   }
   else
   {
      PTN_FunctionECall__eval(_env, _r, this);
   }
}

void PTN_FunctionECall::optimize(void) {
   if(arglist)
   {
      arglist->optimize();
   }
}

sBool PTN_FunctionECall::resolveXRef(void) {
   return arglist ? arglist->resolveXRef() : YAC_TRUE;
}
