/// PTN_While.cpp
///
/// (c) 2001-2014 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
///
#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "PTN_Statement.h"
#include "vmopenum.h"
#include "VMCore.h"
#include "PTN_BreakableStatement.h"
#include "PTN_While.h"


PTN_While::PTN_While(void) {
   cond       = NULL; 
   cond_opt   = NULL;
   body       = NULL;
   break_tag  = 0;
}

PTN_While::PTN_While(PTN_Expr *_cond, PTN_Statement *_body) {
   cond       = _cond; 
   cond_opt   = _cond->getEval();
   body       = _body; 
}

sUI PTN_While::getID(void) const {
   return PTN_WHILE;
}

void PTN_While::init(PTN_Expr *_cond, PTN_Statement *_body) {
   cond     = _cond; 
   cond_opt = cond->getEval();
   body     = _body;
}

PTN_While::~PTN_While() {
   TKS_DELETE_SAFE(cond);

   TKS_DELETE_SAFE(body);
}

sBool PTN_While::semanticCheck(void) {
   return 
      (cond ? cond->semanticCheck()    : 0) &&
      (body ? body->semanticCheckAll() : 0)
      ;
}

sBool PTN_While::resolveXRef(void) {
   return
      (cond->resolveXRef())    &&
      (body->resolveXRefAll())
      ;
}

void PTN_While::optimize(void) {
   tks_optimize_expr(&cond, 1); 
   cond_opt = cond->getEval(); 

   if(body)  
   { 
      body->optimizeAll();
   }
}

void PTN_While::useBreak(void) {
   break_tag = 1;
}

void PTN_While::subGenCallList(void) {
   if(body)  
   { 
      body->genCallList(); 
   }
}

static void PTN_While__eval(PTN_Env *_env, const PTN_Statement *_st) {  
   Dtracest;
   const PTN_While *st = (const PTN_While*)_st; 
   
   YAC_Value r; 
   sBool beval;
   do 
   { 
      r.value.any = NULL;
      st->cond_opt(_env, &r, st->cond); 
      if(Dhaveexception)
      {
         r.unsetFast();
         Dhandleexception(st->cond);
         return;
      }

      beval=!r.isNullOrIF0();
      if(beval)	  
      {
         st->body->evalFirst(_env); 
      }
      r.unsetFast(); 
   } while(_env->context->b_running && beval && _env->continue_flag);
}

static void PTN_While__eval_break(PTN_Env *_env, const PTN_Statement *_st) { 
   Dtracest;
   const PTN_While *st = (const PTN_While *)_st;
   
   sBool *bBreak = _env->context->pushBreak();
   
   YAC_Value r; 
   sBool beval;
   do 
   { 
      r.value.any = NULL;
      st->cond_opt(_env, &r, st->cond); 
      if(Dhaveexception)
      {
         r.unsetFast();
         Dhandleexception(st->cond);
         return;
      }

      beval=!r.isNullOrIF0();
      if(beval)	
      {
         st->body->evalFirst(_env);
      }
      r.unsetFast();
   } while( (!*bBreak) && _env->context->b_running && beval && _env->continue_flag);

   if(*bBreak)
   {
      _env->continue_flag = 1;
   }
   _env->context->popBreak();
}

Fevalst PTN_While::getEvalSt(void) const {
   return break_tag ? PTN_While__eval_break : PTN_While__eval;
}

void PTN_While::eval(PTN_Env *_env) const { 
   if(break_tag)
   {
      PTN_While__eval_break(_env, this);
   }
   else
   {
      PTN_While__eval(_env, this);
   }
}


#ifdef TKS_JIT
sBool PTN_While::forceHybrid(void) {
   return cond->forceHybrid();
}

sU8 PTN_While::compile(VMCore *_vm) {
   sU16 lpc=_vm->vm_pc;
   sU8 r=cond->compile(_vm);
   if(r!=0xFF)
   {
      _vm->typecastStack(r, 1);
      Dasmop(VMOP_SITESTZ);
      sU16 spc=_vm->vm_pc;
      _vm->vm_code[_vm->vm_pc++]=0;
      r=body->compileHybridStatement(_vm);
      if(r!=0xFF)
      {
         Dasmop(VMOP_BRA);
         _vm->vm_code[_vm->vm_pc++]=lpc;
         _vm->vm_code[spc]=_vm->vm_pc;

         jitFixBreakPCs(_vm->vm_pc);

         if(next)  
         { 
            return next->compileHybridStatement(_vm); 
         }
         else  
         { 
            return 0; 
         }
      }
   }
   return 0xFF;
}
#endif
