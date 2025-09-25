/// PTN_TryCatchFinally.cpp
///
/// (c) 2006-2025 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "tks_inc_compiler.h"
#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "PTN_Statement.h"
#include "PTN_TryCatchFinally.h"


PTN_CatchBlock::PTN_CatchBlock(TKS_ExceptionType *_t, PTN_Statement *_body) {
   exception_type = _t;
   body           = _body;
   next           = NULL;
}

PTN_CatchBlock::~PTN_CatchBlock() {
   TKS_DELETE_SAFE(body);
   TKS_DELETE_SAFE(next);
}

sBool PTN_CatchBlock::semanticCheck(void) {
   return
      (body ? body->semanticCheckAll() : 0)
      ;
}

sBool PTN_CatchBlock::resolveXRef(void) {
   return body->resolveXRefAll();
}

void PTN_CatchBlock::subGenCallList(void) {
   if(NULL != body)
   {
      body->genCallList();
   }

   if(NULL != next)
   {
      next->subGenCallList();
   }
}

void PTN_CatchBlock::optimize(void) {
   body->optimizeAll();
}

sBool PTN_CatchBlock::isExceptionTypeDerivedFrom(TKS_ExceptionType *_t) const {
   return tkscript->exception_typecast_map[exception_type->exception_id][_t->exception_id];
}


PTN_TryCatchFinally::PTN_TryCatchFinally(PTN_TryCatchFinally *_parent) {
   try_body     = NULL;
   first_catch  = NULL;
   finally_body = NULL;
   parent       = _parent;
}

PTN_TryCatchFinally::~PTN_TryCatchFinally() {
   TKS_DELETE_SAFE(try_body);
   TKS_DELETE_SAFE(first_catch);
   TKS_DELETE_SAFE(finally_body);
}

sUI PTN_TryCatchFinally::getID(void) const {
   return PTN_TRYCATCHFINALLY;
}

sBool PTN_TryCatchFinally::semanticCheck(void) {
   return
      (try_body     ? try_body->semanticCheckAll()     : 0 ) &&
      (first_catch  ? first_catch->semanticCheck()     : 1 ) &&
      (finally_body ? finally_body->semanticCheckAll() : 1 )
      ;
}

sBool PTN_TryCatchFinally::resolveXRef(void) {
   return
      (try_body     ? try_body->resolveXRefAll()     : 0 ) &&
      (first_catch  ? first_catch->resolveXRef()     : 1 ) &&
      (finally_body ? finally_body->resolveXRefAll() : 1 )
      ;
}

void PTN_TryCatchFinally::optimize(void) {
   try_body->optimizeAll();

   if(NULL != first_catch)
   {
      first_catch->optimize();
   }

   if(NULL != finally_body)
   {
      finally_body->optimizeAll();
   }
}

void PTN_TryCatchFinally::subGenCallList(void) {

   if(NULL != try_body)
   {
      try_body->genCallList();
   }

   if(NULL != first_catch)
   {
      first_catch->subGenCallList();
   }

   if(NULL != finally_body)
   {
      finally_body->genCallList();
   }
}


void PTN_TryCatchFinally::addCatchSorted(TKS_ExceptionType *_t, PTN_Statement *_st) {

   // Create new catchblock entry and add sorted by genealogy

   PTN_CatchBlock *n;
   PTN_NEW_STATIC_NODE(n, PTN_CatchBlock)(_t, _st);
   if(NULL == first_catch)
   {
      first_catch = n;
      n->next = NULL;
   }
   else
   {
      PTN_CatchBlock *c = first_catch;
      PTN_CatchBlock *p = NULL;
      while(NULL != c)
      {
         if( !c->isExceptionTypeDerivedFrom(_t))
         {
            // Insert before c
            if(NULL == p)
            {
               first_catch = n;
            }
            else
            {
               p->next = n;
            }
            n->next = c;
            return;
         }
         p = c;
         c = c->next;
      }
      // no base class found in catch list, append to end
      p->next = n;
   }
}

sBool PTN_TryCatchFinally::canCatchType(sUI _exceptionTypeId) const {
   // Check whether this or one of the parent trycatch statements can catch the given exception type
   sBool ret = YAC_FALSE;
   const PTN_TryCatchFinally *tcf = this;
   do
   {
      PTN_CatchBlock *c = tcf->first_catch;
      while(NULL != c)
      {
         if(tkscript->exception_typecast_map[_exceptionTypeId][c->exception_type->exception_id])
         {
            ret = YAC_TRUE;
            break;
         }
         c=c->next;
      }
      tcf = tcf->parent;
   } while(NULL != tcf && !ret);
   return ret;
}

static void PTN_TryCatchFinally__eval(PTN_Env *_env, const PTN_Statement *_st) {
   Dtracest;
   const PTN_TryCatchFinally *st = (const PTN_TryCatchFinally *) _st;

   const PTN_TryCatchFinally *sav_last_try = _env->context->last_try;
   _env->context->last_try = st;
   
   st->try_body->evalFirst(_env);

   TKS_Context *ctx = _env->context;

   sBool bOk = YAC_TRUE;
   if(Dhaveexception)
   {
      Dhandleexception(_st);
      const TKS_ExceptionType *et = ((TKS_Exception*)ctx->current_exception_var.value.object_val)->exception_type;

      // Try to catch (most-compatible) exception
      PTN_CatchBlock *cb = st->first_catch;
      if(NULL != cb)
      {
         bOk = YAC_FALSE;
         while(NULL != cb)
         {
            // Is cb->exception_type a base class of the current exception?
            if(tkscript->exception_typecast_map[et->exception_id][cb->exception_type->exception_id])
            {
               // CatchBlock is compatible
               // Run catch statements
               ctx->b_running = 1;
               ctx->b_exception_raised = 0;

               // Remember current exception (push)
               TKS_CachedObject save_exception_var;
               save_exception_var.copySafe(&ctx->current_exception_var);

               // Remember previously saved exception (push)
               TKS_CachedObject *last_saved_exception = ctx->saved_exception;
               ctx->saved_exception = &save_exception_var;

               cb->body->evalFirst(_env);
               if(Dhaveexception)
               {
                  // New uncaught exception while exc'ing catch block
                  Dhandleexception(cb->body);

                  // Exception in catch overwrites previous exception
                  save_exception_var.unset();
               }
               else
               {
                  bOk = 1; // Exception was handled, no new exception was raised

                  // Pop previous exception
                  ctx->current_exception_var.copySafe(&save_exception_var);
               }

               // Pop last saved exception
               ctx->saved_exception = last_saved_exception;

               break;
            }
            cb = cb->next;
         }
      }
      else
      {
         // No catch blocks
      }
   }

   // Restore last seen tryblock reference
   _env->context->last_try = sav_last_try; // xxx use parent?!!

   if(bOk)
   {
      // If exception was successfully caught or no exception was raised within try{} at all,
      // run the finally statement block
      if(NULL != st->finally_body)
      {
         // Remember current exception (push)
         TKS_CachedObject save_exception_var;
         save_exception_var.copySafe(&ctx->current_exception_var);

         // Remember previously saved exception (push)
         //??? TKS_CachedObject *last_saved_exception = ctx->saved_exception;
         ctx->saved_exception = &save_exception_var;

         sBool bExceptionRaised = ctx->b_exception_raised;
         sBool bRunning = ctx->b_running;
         ctx->b_running = 1;
         ctx->b_exception_raised = 0;

         st->finally_body->evalFirst(_env);
         if(Dhaveexception)
         {
			   // Exception in finally{}
            Dprintf("[---] Exception in finally{} !\n");
            Dhandleexception(st->finally_body);

            // Exception in finally overwrites previous exception
            save_exception_var.unset();
         }
         else
         {
            // Pop previous exception
            ctx->current_exception_var.copySafe(&save_exception_var);

            ctx->b_running = bRunning;
            ctx->b_exception_raised = bExceptionRaised;
         }

         // Pop last saved exception
         //??? ctx->saved_exception = last_saved_exception;

      }
   }
   // else: Exception has not been caught; pass to caller
}

Fevalst PTN_TryCatchFinally::getEvalSt(void) const {
   return PTN_TryCatchFinally__eval;
}

void PTN_TryCatchFinally::eval(PTN_Env *_env) const {
   PTN_TryCatchFinally__eval(_env, this);
}
