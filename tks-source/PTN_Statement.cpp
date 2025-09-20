/// PTN_Statement.cpp
///
/// (c) 2001-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Statement.h"
#include "vmopenum.h"
#include "VMCore.h"

#include "TKS_Compiler.h"

sSI PTN_Statement::object_counter = 0;


PTN_Statement::PTN_Statement(void) {
   PTN_Statement::object_counter++;
   next        = NULL;
   prev        = NULL;
   st_list     = NULL;
   st_list_opt = NULL;
}

PTN_Statement::~PTN_Statement() {
   PTN_Statement::object_counter--;
   if(next)
   {
      // delete next, execution stage
      if(prev)
      {
         PTN_Statement *cst=prev;
         while(cst->prev)
            cst = cst->prev;
         cst->next->prev = NULL;
         delete cst->next;
      }
      else
      {
         next->prev = NULL;
         delete next;
         next = NULL;
      }
   }
   else
   {
      // delete previous, parser stage
      PTN_Statement *cst = prev;
      while(cst)
      {
         cst = cst->prev;
         if(cst)
         {
            if(cst->next)
            {
               cst->next->prev = NULL;
               delete cst->next;
               cst->next = NULL;
            }
         }
      }
   }
   if(st_list)
   {
      delete [] st_list;
      st_list = NULL;
   }
   if(st_list_opt)
   {
      delete [] st_list_opt;
      st_list_opt = NULL;
   }
}

void PTN_Statement::useBreak(void) {
}

void PTN_Statement::evalFirst(PTN_Env *_env) const {
#if defined(TKS_PROFILE_STATEMENTS) && !defined(TKS_PROFILE_FUNCTION_CALLSITES)
   if(tkscript->configuration.b_enable_statement_profiling)
   {
      const PTN_Statement **cs = st_list;
      Fevalst *f = st_list_opt;
      while(_env->context->b_running && (*f) && _env->continue_flag )
      {
         sBool needSem = (*cs)->needSemicolon();
         if(!needSem)
         {
            Dbeginstprofile;
            (*f++)(_env, *cs);
            Dendstprofile((*cs++)->src_loc);
         }
         else
         {
            (*f++)(_env, *cs++);
         }
      }
   }
   else
#endif
   {
      const PTN_Statement **cs = st_list;
      Fevalst *f = st_list_opt;
      Fevalst fun;
      while(_env->context->b_running && (fun=*f++) && _env->continue_flag )
         (fun)(_env, *cs++);
   }

   if(Dhaveexception)
   {
      Dhandleexception(this);
      return;
   }
}

void PTN_Statement::eval(PTN_Env *_env) const {
   (void)_env;
}

sBool PTN_Statement::semanticCheck(void) {
   return 1;
}

sBool PTN_Statement::semanticCheckAll(void) {
   PTN_Statement *st = this;
   while(st)
   {
      if(st->semanticCheck())
      {
         st = st->next;
      }
      else
      {
         ////st->semanticCheck();//xxx debug
         Dprintf("[---] semantic check failed\n");
         Dprintf("[---] in module \"%s\", line %i.\n",
                 Dtkscompiler->getModuleNameByIndex(Dsrcloc_module(src_loc)),
                 Dsrcloc_linenr(src_loc)
                 );
         return YAC_FALSE;
      }
   }
   return YAC_TRUE;
}

sBool PTN_Statement::resolveXRef(void) {
   return YAC_TRUE;
}

sBool PTN_Statement::resolveXRefAll(void) {
   PTN_Statement *st = this;
   while(st)
   {
      if(st->resolveXRef())
      {
         st = st->next;
      }
      else
      {
         return YAC_FALSE;
      }
   }
   return YAC_TRUE;
}

void PTN_Statement::optimize(void) {
}

void PTN_Statement::optimizeAll(void) {
   PTN_Statement *st = this;
   while(st)
   {
      st->optimize();

      st = st->next;
   }
}

void PTN_Statement::subGenCallList(void) {
}

void PTN_Statement::genCallList(void) {

   if(NULL == st_list)
   {
      sUI nst = 0;
      PTN_Statement *c = this;

      // Count elements
      while(c)
      {
         nst++;
         c = c->next;
      }

      // Allocate+init. statement and funptr arrays
      c = this;
      st_list     = new const PTN_Statement*[nst+1];
      st_list_opt = new Fevalst[nst+1];
      sUI i = 0;
      sUI j;
      for(j=0; j<nst; j++)
      {
         st_list     [i] = c->getEvalStArg();
         st_list_opt [i] = c->getEvalSt(); // non-virtual "c" call

         if(NULL != st_list_opt[i])
         {
            i++;
            c->subGenCallList();
         }
         c = c->next;
      }

      // Terminate statement array
      st_list     [i] = NULL; // EOL
      st_list_opt [i] = NULL;
   }
   else
   {
#ifdef TKS_DCON
      //tkscript->printf("[~~~] PTN_Statement::genCallList: list already exists!\n");
#endif
   }

}

Fevalst PTN_Statement::getEvalSt(void) const {
   return NULL;
}

#ifdef TKS_JIT
sU8 PTN_Statement::compile(VMCore *_vm) {
   if(next)
   {
      return next->compileHybridStatement(_vm);
   }
   else
   {
      return 0;
   }
}

sU8 PTN_Statement::compileHybridStatement(VMCore *_vm) {
   if(forceHybrid())
   {
      _vm->addHybridStatement(this);
      if(next)
      {
         return next->compileHybridStatement(_vm);
      }
      else
      {
         return 0;
      }
   }
   else
   {
      return compile(_vm);
   }
}

sBool PTN_Statement::forceHybrid(void) {
   return 1;
}

#endif // TKS_JIT
