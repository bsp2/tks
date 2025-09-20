/// PTN_UnresolvedModuleMethodECall.cpp
///
/// (c) 2001-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "tks_inc_compiler.h"
#include "tks_inc_jit.h"
#include "tks_inc_class.h"

#include "PTN_CallableExpr.h"
#include "PTN_ArgListNode.h"
#include "PTN_ArgList.h"
#include "PTN_ArgVar.h"
#include "PTN_ArgVarList.h"
#include "TKS_FormalArgument.h"
#include "PTN_Function.h"
#include "PTN_FunctionECall.h"

#include "PTN_UnresolvedModuleMethodECall.h"

#include "TKS_Mutex.h"


PTN_UnresolvedModuleMethodECall::PTN_UnresolvedModuleMethodECall(YAC_String  * _modulename,
                                                                 YAC_String  * _methodname,
                                                                 PTN_ArgList * _args
                                                                 )
{
   module_name.yacCopy(_modulename);
   module_method_name.yacCopy(_methodname);
   arglist      = _args;
   resolved_fun = NULL;
}

PTN_UnresolvedModuleMethodECall::~PTN_UnresolvedModuleMethodECall() {
   if(arglist)
   {
      delete arglist;
      arglist = NULL;
   }
}

sBool PTN_UnresolvedModuleMethodECall::semanticCheck(void) {
   return
      (arglist ? (arglist->semanticCheck()) : 1);
}

void PTN_UnresolvedModuleMethodECall::optimize(void) {
   if(arglist)
   {
      arglist->optimize();
   }
}

sBool PTN_UnresolvedModuleMethodECall::resolveXRef(void) {
   TKS_CachedScript *resolved_module = Dtkscompiler->findModule(&module_name);

   if(!resolved_module)
   {
      Dprintf("[---] unknown reference to method or function \"%s::%s\" (no such module/variable).\n",
              (char*)module_name.chars,
              (char*)module_method_name.chars
              );
      Dcterror(PTNERR_MODULENOTFOUND);
      return 0;
   }

   resolved_fun = resolved_module->findFunction(&module_method_name);

   if(!resolved_fun)
   {
      Dprintf("[---] module or class \"%s\" has no function or method named \"%s\".\n",
              (char*)module_name.chars,
              (char*)module_method_name.chars
              );
      Dprintf("[---] in module \"%s\" line %i.\n",
              Dtkscompiler->getModuleNameByIndex(Dsrcloc_module(src_loc)),
              Dsrcloc_linenr(src_loc)
              );
      Dcterror(PTNERR_MODULEMETHODNOTFOUND);
      return 0;
   }
   if(resolved_fun->parent_class)
   {
      return 0; // is delegate method
   }
   if(arglist)
   {
      if(!arglist->resolveXRef())
      {
         return 0;
      }
   }
   return PTN_FunctionECall::SemanticCheck(resolved_fun, arglist, src_loc);
}

static void PTN_UnresolvedModuleMethodECall__eval(PTN_Env *_env, YAC_Value *_r, const PTN_Expr *_st) {
   const PTN_UnresolvedModuleMethodECall *st = (const PTN_UnresolvedModuleMethodECall*)_st;

   PTN_Function *f = st->resolved_fun;

   Dbeginfuncall(f);

   YAC_Value *nsf = _env->context->pushStackFrame(f);
   if(nsf)
   {
      YAC_Value *stackc = _env->context->tks_fstackc;

      // Validate arguments
      if(st->arglist)
      {
         if(f->argvarlist)
         {
            if(st->arglist->num_args == f->argvarlist->num_vars)
            {
               f->setArgsByList(_env, st->arglist, nsf);
            }
            else
            {
               Dprintf("[---] wrong argument count in call to function \"%s\" (have=%d, expect=%d)\n",
                       (char*)f->name.chars,
                       st->arglist->num_args,
                       f->argvarlist->num_vars
                       );
               _env->context->popStackFrame(f);
               _env->context->tks_fstackc = stackc;
               Dendfuncall(f);
               // XXX TODO: add new exception type for this kind of error ?
               Drtthrow(st, TKS_EXCEPTION_CRITICALERROR, "wrong number of args to script function");
               return;
            }
         }
         else
         {
            Dprintf("[---] wrong argument count in call to function \"%s\" (have=%d, expect=0)\n",
               (char*)f->name.chars,
               st->arglist->num_args
               );
            _env->context->popStackFrame(f);
            _env->context->tks_fstackc = stackc;
            Dendfuncall(f);
            // XXX TODO: add new exception type for this kind of error ?
            Drtthrow(st, TKS_EXCEPTION_CRITICALERROR, "wrong number of args to script function");
            return;
         }
      }
      else
      {
         if(f->argvarlist)
         {
            Dprintf("[---] wrong argument count in call to function \"%s\" (have=0, expect=%d)\n",
                    (char*)f->name.chars,
                    f->argvarlist->num_vars
                    );
            _env->context->popStackFrame(f);
            _env->context->tks_fstackc = stackc;
            Dendfuncall(f);
            // XXX TODO: add new exception type for this kind of error ?
            Drtthrow(st, TKS_EXCEPTION_CRITICALERROR, "wrong number of args to script function");
            return;
         }
      }
      //////resolved_fun->setArgsByList(_env, arglist, nsf);

#ifdef DX_TRACEFUNCTIONCALLS
      _env->context->pushFunctionCall(f, st->src_loc);
#endif
      _env->context->pushStackFrame2(f);

      f->evalCall(_env, _r);

#ifdef DX_TRACEFUNCTIONCALLS
      _env->context->popFunctionCall();
#endif

      _env->context->popStackFrame(f);
      _env->context->tks_fstackc = stackc;
   }

   Dendfuncall(f);

   _env->continue_flag = 1;
}

Feval PTN_UnresolvedModuleMethodECall::getEval(void) const {
   return PTN_UnresolvedModuleMethodECall__eval;
}

void PTN_UnresolvedModuleMethodECall::evalCallableExpr(PTN_Env *_env, YAC_Value *_r) const {
   PTN_UnresolvedModuleMethodECall__eval(_env, _r, this);
}

void PTN_UnresolvedModuleMethodECall::eval(PTN_Env *_env, YAC_Value *_r) const {
   PTN_UnresolvedModuleMethodECall__eval(_env, _r, this);
}
