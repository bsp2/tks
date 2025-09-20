/// PTN_IMethodECall.cpp
///
/// (c) 2001-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "PTN_Statement.h"
#include "PTN_CallableExpr.h"
#include "PTN_ArgListNode.h"
#include "PTN_ArgList.h"
#include "PTN_ECallY.h"
#include "PTN_IMethodECall.h"

#include "tks_inc_class.h"
#include "tks_inc_compiler.h"

#include "TKS_Mutex.h"


PTN_IMethodECall::PTN_IMethodECall(PTN_Expr    *_objexpr,
                                   YAC_String  *_methodname,
                                   PTN_ArgList *_args
                                   )
{
   obj_expr       = _objexpr;
   obj_expr_opt   = obj_expr->getEval();
   method_name.yacCopy(_methodname);
   method_arglist = _args;
   b_statement    = YAC_FALSE;
}

PTN_IMethodECall::~PTN_IMethodECall() {
   TKS_DELETE_SAFE(obj_expr);
   TKS_DELETE_SAFE(method_arglist);
}

void PTN_IMethodECall::setStatementHint(void) {
   b_statement = YAC_TRUE;
}

sBool PTN_IMethodECall::semanticCheck(void) {
   //
   // Note: cannot check arglist since formal arglist is still unknown at this point
   //
   return
      (obj_expr ? obj_expr->semanticCheck() : 0) &&
      (method_arglist ? method_arglist->semanticCheck() : 1);
}

sBool PTN_IMethodECall::resolveXRef(void) {
   if(obj_expr)
   {
      if(obj_expr->resolveXRef())
      {
         if(method_arglist)
         {
            return method_arglist->resolveXRef();
         }
         return 1;
      }
   }
   else
   {
      Dprintf("[---] PTN_IMethodECallExpr::resolveXRef: missing obj_expr.\n");
   }
   return 0;
}

void PTN_IMethodECall::optimize(void) {
   if(obj_expr)
   {
      obj_expr->optimize();
      obj_expr_opt = obj_expr->getEval();
   }
   if(method_arglist)
   {
      method_arglist->optimize();
   }
}

static void PTN_IMethodECall__eval(PTN_Env *_env, YAC_Value *_r, const PTN_Expr *_st) {
   const PTN_IMethodECall *st = (const PTN_IMethodECall*)_st;

   YAC_Value iobjVal;
   TKS_ClassDeclMethod *metaClassMethod;
   const YAC_CommandY  *cppCommandY;

   // Evaluate object expression
   st->obj_expr_opt(_env, &iobjVal, st->obj_expr);
   if(Dhaveexception)
   {
      iobjVal.unsetFast();
      Dhandleexception(st->obj_expr);
      return;
   }

   //
   // autobox int/float value
   //
   iobjVal.typecast(YAC_TYPE_OBJECT);

   if(iobjVal.type >= YAC_TYPE_OBJECT)
   {
      if(TKS_VALID(iobjVal.value.object_val))
      {
         // Get C++ class ID
         sUI ccid = iobjVal.value.object_val->class_ID;

         sBool btrynext = 1;

         //
         // Try to call meta-class method
         //
         if(YAC_CLID_CLASS == ccid)
         {
            TKS_ScriptClassInstance *sci = (TKS_ScriptClassInstance*) iobjVal.value.object_val;

            // Find method
            metaClassMethod = sci->class_decl->getMethod((YAC_String*) &st->method_name);

            // Do vtable lookup if method exists
            if(metaClassMethod)
            {
#ifdef TKS_CHECK_ABSTRACTCALL
               if(metaClassMethod->b_abstract)
               {
                  TKS_ClassDecl *cd = sci->class_decl;
                  if(metaClassMethod->isAbstractInClass(cd))
                  {
                     Dprintf("\n[---] cannot call abstract method \"%s::%s::%s\".\n",
                             (char*)cd->nspace->name.chars,
                             (char*)cd->name.chars,
                             (char*)metaClassMethod->function.name.chars
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

               if(metaClassMethod->vtable)
               {
                  metaClassMethod = metaClassMethod->vtable[sci->class_decl->class_id];
               }
            }
            if(metaClassMethod)
            {
               // Eval class method ecall
               btrynext=0;
               PTN_Function *f = &metaClassMethod->function;

               Dbeginfuncall(f);

               Dcondbegincallsiteprofile;

               YAC_Value *nsf = _env->context->pushStackFrame(f);

               if(nsf)
               {
                  YAC_Value *stackc = _env->context->tks_fstackc;

                  // Validate arguments
                  if(st->method_arglist)
                  {
                     if(f->argvarlist)
                     {
                        if(st->method_arglist->num_args == f->argvarlist->num_vars)
                        {
                           f->setArgsByList(_env, st->method_arglist, nsf);
                        }
                        else
                        {
                           Dprintf("[---] wrong argument count in call to method \"%s::%s::%s\" (have=%d, expect=%d)\n",
                                   (char*)sci->class_decl->nspace->name.chars,
                                   (char*)sci->class_decl->name.chars,
                                   (char*)f->name.chars,
                                   st->method_arglist->num_args,
                                   f->argvarlist->num_vars
                                   );
                           Dendfuncall(f);
                           // XXX TODO: add new exception type for this kind of error ?
                           Drtthrow(st, TKS_EXCEPTION_CRITICALERROR, "wrong number of args to script method");
                           iobjVal.unsetFast();
                           Dcondendcallsiteprofile(st->src_loc);
                           return;
                        }
                     }
                     else
                     {
                        Dprintf("[---] wrong argument count in call to method \"%s::%s::%s\" (have=%d, expect=0)\n",
                                (char*)sci->class_decl->nspace->name.chars,
                                (char*)sci->class_decl->name.chars,
                                (char*)f->name.chars,
                                st->method_arglist->num_args
                                );
                        Dendfuncall(f);
                        // XXX TODO: add new exception type for this kind of error ?
                        Drtthrow(st, TKS_EXCEPTION_CRITICALERROR, "wrong number of args to script method");
                        iobjVal.unsetFast();
                        Dcondendcallsiteprofile(st->src_loc);
                        return;
                     }
                  }
                  else
                  {
                     if(f->argvarlist)
                     {
                        Dprintf("[---] wrong argument count in call to method \"%s::%s::%s\" (have=0, expect=%d)\n",
                                (char*)sci->class_decl->nspace->name.chars,
                                (char*)sci->class_decl->name.chars,
                                (char*)f->name.chars,
                                f->argvarlist->num_vars
                                );
                        Dendfuncall(f);
                        // XXX TODO: add new exception type for this kind of error ?
                        Drtthrow(st, TKS_EXCEPTION_CRITICALERROR, "wrong number of args to script method");
                        iobjVal.unsetFast();
                        Dcondendcallsiteprofile(st->src_loc);
                        return;
                     }
                  }

#ifdef DX_TRACEFUNCTIONCALLS
                  _env->context->pushFunctionCall(f, st->src_loc);
#endif
                  _env->context->pushStackFrame2(f);

                  _env->context->pushClassStack(sci);

                  {
                     f->evalCall(_env, _r);
                  }

#ifdef DX_TRACEFUNCTIONCALLS
                  _env->context->popFunctionCall();
#endif
                  _env->context->popStackFrame(f);
                  _env->context->tks_fstackc = stackc;

                  _env->continue_flag = 1;
                  _env->context->popClassStack();

               } // if nsf

               Dcondendcallsiteprofile(st->src_loc);

               Dendfuncall(f);

               iobjVal.unsetFast();
               return;

            } // if(metaClassMethod)

            //
            // Dont throw exception; try to resolve to YAC_Object method first
            //

         } // if(YAC_CLID_CLASS == ccid)


         //
         // Try to call C++ method
         //
         if(btrynext)
         {
            //
            // Get C++ class template
            //
            //
            TKS_ClassTemplate *clt = TKSCRIPT__ATOM_SPEEDPATH[ccid];
            cppCommandY = clt->findCommandY((YAC_String*) &st->method_name);
            if(cppCommandY)
            {
               // Check whether command has RVAL variant available and use that if possible
               if(1 == st->b_statement)
               {
                  /* Used as a statement */
                  if(NULL != cppCommandY->y_retself_cmd)
                  {
                     cppCommandY = cppCommandY->y_retself_cmd;
                  }
               }
               else if(cppCommandY->y_retval_cmd)
               {
                  /* Used as an expression */
                  cppCommandY = cppCommandY->y_retval_cmd;
               }

               sBool barg;
               if(!st->method_arglist)
               {
                  barg = cppCommandY->y_arg_num;
               }
               else
               {
                  barg = (cppCommandY->y_arg_num != st->method_arglist->num_args);
               }
               if(barg)
               {
                  Dprintf("\n[---] wrong number of arguments (have=%i, need=%i) to C++ method \"%s\"\n",
                          (NULL != st->method_arglist) ? st->method_arglist->num_args : 0,
                          cppCommandY->y_arg_num,
                          (char*)st->method_name.chars
                          );
                  Dprintf("[---] in module \"%s\", line %i.\n",
                          Dtkscompiler->getModuleNameByIndex(Dsrcloc_module(st->src_loc)),
                          Dsrcloc_linenr(st->src_loc)
                          );
                  Drtthrow(st, TKS_EXCEPTION_NATIVECLASSMETHODNOTFOUND, "wrong number of args to c++ method");
                  iobjVal.unsetFast();
                  return;
               }
               else
               {
                  //
                  // Call C++ method
                  //  (TODO) does not work with RARG/RVAL command variants, yet
                  //
#ifdef DX_TRACEFUNCTIONCALLS
                  _env->context->pushFunctionCallY(iobjVal.value.object_val, cppCommandY, st->src_loc);
#endif

                  Dcondbegincallsiteprofile;

                  yacmemptr current_call_memptr; // xxx TKS_MT put argument values on stack (re-entrance)
                  yacmem linargs[TKS_MAX_ARGS];
                  current_call_memptr.mem = linargs;

                  PTN_ECallY::EvalCPPCall(
                     iobjVal.value.object_val,
                     cppCommandY,
                     st->method_arglist,
                     current_call_memptr,
                     _env, _r
                     );

                  Dcondendcallsiteprofile(st->src_loc);

#ifdef DX_TRACEFUNCTIONCALLS
                  _env->context->popFunctionCallY();
#endif
               }
            }
            else
            {
               Dprintf("\n[---] unknown method \"%s\".\n", (char*)st->method_name.chars);
               Drtthrow(st, TKS_EXCEPTION_CLASSMETHODNOTFOUND, "failed to resolve class method");
            }

         } // if(bTryNext)

      } // if(TKS_VALID(iobjVal
      else
      {
         Drtthrowinvalidpointer(st, "cannot call method of invalid c++ object", iobjVal.value.object_val);
      }
   } // if r is object
   else
   {
      Drtthrow(st, TKS_EXCEPTION_TYPEMISMATCH, "cannot call method of non-object type");
   }

   iobjVal.unsetFast();
}

Feval PTN_IMethodECall::getEval(void) const {
   return PTN_IMethodECall__eval;
}

void PTN_IMethodECall::evalCallableExpr(PTN_Env *_env, YAC_Value *_r) const {
   PTN_IMethodECall__eval(_env, _r, this);
}

void PTN_IMethodECall::eval(PTN_Env *_env, YAC_Value *_r) const {
   PTN_IMethodECall__eval(_env, _r, this);
}
