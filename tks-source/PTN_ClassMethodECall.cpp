/// PTN_ClassMethodECall.cpp
///
/// (c) 2001-2025 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
///
#include "tks.h"
#include "tks_inc_class.h"
#include "tks_inc_jit.h"

#include "YAC_HashTable.h"
#include "PTN_DoubleArgExpr.h"
#include "PTN_CallableExpr.h"
#include "TKS_Compiler.h"
#include "PTN_ClassMethodECall.h"

#include "TKS_Mutex.h"

PTN_ClassMethodECall::PTN_ClassMethodECall(TKS_CachedObject    * _var,
                                           TKS_ClassDeclMethod * _method,
                                           PTN_ArgList         * _args
                                           ) {
   var         = _var;
   method      = _method;
   arglist     = _args;
   class_id    = TKS_INVALID_SCRIPT_CLASS_ID;

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
   no_virtual = YAC_FALSE;
   b_indirect = YAC_FALSE;
}

PTN_ClassMethodECall::~PTN_ClassMethodECall() {
   if(arglist)
   {
      delete arglist;
      arglist = NULL;
   }
}

sBool PTN_ClassMethodECall::semanticCheck(void) {
   if(NULL != method)
   {
      TKS_ClassDecl *cd = ((TKS_Compiler*)tkscript->compiler)->class_decls.elements[method->class_id];

      if(arglist)
      {
         // Have actual varlist
         if(arglist->semanticCheck())
         {
            if(method->function.argvarlist)
            {
               if(arglist->num_args != method->function.argvarlist->num_vars)
               {
                  Dprintf("\n[---] wrong number of arguments to method \"%s::%s::%s\" (need=%i, have=%i)\n",
                          cd->nspace->name.chars,
                          cd->name.chars,
                          method->function.name.chars,
                          method->function.argvarlist->num_vars,
                          arglist->num_args
                          );
                  Dprintf("[---] in module \"%s\", line %i.\n",
                          Dtkscompiler->getModuleNameByIndex(Dsrcloc_module(src_loc)),
                          Dsrcloc_linenr(src_loc)
                          );
                  return YAC_FALSE;
               }
            }
            else
            {
               Dprintf("xxx argvarlist is NULL\n");
               Dprintf("\n[---] wrong number of arguments to method \"%s::%s::%s\" (need=0, have=%i)\n",
                       (char*)cd->nspace->name.chars,
                       (char*)cd->name.chars,
                       (char*)method->function.name.chars,
                       arglist->num_args
                       );
               Dprintf("[---] in module \"%s\", line %i.\n",
                       Dtkscompiler->getModuleNameByIndex(Dsrcloc_module(src_loc)),
                       Dsrcloc_linenr(src_loc)
                       );
               return YAC_FALSE;
            }

            // Succeeded
            return YAC_TRUE;
         }
         // else: arglist->semanticCheck failed
      }
      else
      {
         // have no args
         if(NULL == method->function.argvarlist)
         {
            return YAC_TRUE;
         }
         else
         {
            tkscript->printf("\n[---] no arguments for call to method \"%s::%s\" (need=%i, have=0)\n",
                             (char*)cd->name.chars,
                             method->function.name.chars,
                             method->function.argvarlist->num_vars
                             );
         }
      }
   }
   return YAC_FALSE;
}

void PTN_ClassMethodECall::optimize(void) {
   if(arglist)
   {
      arglist->optimize();
   }
}

sBool PTN_ClassMethodECall::resolveXRef(void) {
   return arglist ? arglist->resolveXRef() : YAC_TRUE;
}

static void PTN_ClassMethodECall__eval(PTN_Env *_env, YAC_Value *_r, const PTN_Expr *_st) {
   const PTN_ClassMethodECall *st = (const PTN_ClassMethodECall*)_st;

   TKS_ScriptClassInstance *robj;

   // tkscript->printf("xxx PTN_ClassMethodECall__eval: st=%p\n", st);
   // tkscript->printf("xxx PTN_ClassMethodECall__eval: st->method->function.name.chars=%s\n", st->method->function.name.chars);

   if(st->b_indirect)
   {
      robj = (TKS_ScriptClassInstance*) _env->context->tksvm_indirect_object;
   }
   else if(st->var)
   {
      YAC_Value *co = Dgetvar(st->var);
      if(co->type >= YAC_TYPE_OBJECT)
      {
         robj = (TKS_ScriptClassInstance*) co->value.object_val;
      }
      else
      {
         robj = NULL;
         Drtthrow(st, TKS_EXCEPTION_TYPEMISMATCH, "cannot call class method of non-object variable");
         return;
      }
   }
   else
   {
      robj = _env->context->tksvm_class_stack_object;
   }

   // check whether current object of var matches class_id that this method call was compiled for
   if(TKS_VALID(robj))
   {
      if(YAC_CLID_CLASS == robj->class_ID)
      {
         if(!robj->class_decl->ancestor_table[st->class_id])
         {
            tkscript->printf("[---] class method call: instance type mismatch.\n");
            Drtthrow(st, TKS_EXCEPTION_SCRIPTCLASSTYPEMISMATCH, "cannot call method of incompatible script-class object");
            return;
         }
      }
      else
      {
         Drtthrow(st, TKS_EXCEPTION_CLASSTYPEMISMATCH, "cannot call class method of non-scriptclass variable");
         return;
      }
   }
   else
   {
      Drtthrowinvalidpointer(st, "cannot call class method of invalid object variable", robj);
      return;
   }

   // Determine function
   TKS_ClassDeclMethod *rmethod;

#ifdef TKS_CHECK_ABSTRACTCALL
   if(st->method->b_abstract)
   {
      TKS_ClassDecl *cd = robj->class_decl;
      if(st->method->isAbstractInClass(robj->class_decl))
      {
         Dprintf("\n[---] cannot call abstract method \"%s::%s::%s\".\n",
                 (char*)cd->nspace->name.chars,
                 (char*)cd->name.chars,
                 (char*)st->method->function.name.chars
                 );
         Dprintf("[---] in module \"%s\", line %i.\n",
                 Dtkscompiler->getModuleNameByIndex(Dsrcloc_module(st->src_loc)),
                 Dsrcloc_linenr(st->src_loc)
                 );
         Drtthrow(st, TKS_EXCEPTION_SCRIPTCLASSMETHODNOTFOUND, "cannot call abstract method");
         return;
      }
   }
#endif // TKS_CHECK_ABSTRACTCALL

   if(!st->no_virtual)
   {
      rmethod = st->method->vtable ? st->method->vtable[robj->class_decl->class_id] : st->method;
   }
   else
   {  // MyClass::myMethod
      rmethod = st->method;
   }

   PTN_Function *f = &rmethod->function;

#ifdef TKS_MT
   if(st->method->b_this_mutex)
   {
      robj->mtx_this->lock();
   }
#endif // TKS_MT

   Dbeginfuncall(f);

   Dcondbegincallsiteprofile;

   YAC_Value *nsf = _env->context->pushStackFrame(f);
   if(nsf)
   {
      YAC_Value *stackc = _env->context->tks_fstackc;

#ifdef DX_TRACEFUNCTIONCALLS
      _env->context->pushFunctionCall(f, st->src_loc);
#endif

      if(st->arglist) // semanticCheck() ensures that function expects args
      {
         f->setArgsByList(_env, st->arglist, nsf);
      }

      _env->context->pushStackFrame2(f);

      if(st->var || st->b_indirect)
      {
         _env->context->pushClassStack(robj);
      }

      // Actually call method
      f->evalCall(_env, _r);

#ifdef DX_TRACEFUNCTIONCALLS
      _env->context->popFunctionCall();
#endif

      _env->context->popStackFrame(f);
      _env->context->tks_fstackc = stackc;

      _env->continue_flag = YAC_TRUE;

      if(st->var || st->b_indirect)
      {
         _env->context->popClassStack();
      }
   }

   Dcondendcallsiteprofile(st->src_loc);

   Dendfuncall(f);

#ifdef TKS_MT
   if(st->method->b_this_mutex)
   {
      robj->mtx_this->unlock();
   }
#endif // TKS_MT
}

Feval PTN_ClassMethodECall::getEval(void) const {
   return PTN_ClassMethodECall__eval;
}

void PTN_ClassMethodECall::evalCallableExpr(PTN_Env *_env, YAC_Value *_r) const {
   PTN_ClassMethodECall__eval(_env, _r, this);
}

void PTN_ClassMethodECall::eval(PTN_Env *_env, YAC_Value *_r) const {
   PTN_ClassMethodECall__eval(_env, _r, this);
}
