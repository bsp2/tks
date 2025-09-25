/// PTN_Call.cpp
///
/// (c) 2001-2025 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "PTN_Statement.h"
#include "PTN_CallableExpr.h"
#include "PTN_Call.h"
#include "vmopenum.h"
#include "VMCore.h"


PTN_Call::PTN_Call(void) {
   expr     = NULL;
   expr_opt = NULL;
}

PTN_Call::PTN_Call(PTN_CallableExpr *_expr) {
   expr     = _expr;
   expr_opt = NULL;
}

PTN_Call::~PTN_Call() {
   if(expr)
   {
      delete expr; 
      expr = NULL;
   }
}

sBool PTN_Call::semanticCheck(void) {
   if(expr)
   {
      expr->setStatementHint();
      return expr->semanticCheck();
   }
   return 0;
}

void PTN_Call::optimize(void) {
   expr->optimize(); 
   expr_opt = expr->getEval(); 
   if(NULL == expr_opt)
   {
      Dprintf("[---] internal error: PTN_Call::optimize: expr_opt is NULL.\n");
   }
}

sBool PTN_Call::resolveXRef(void) {
   return expr->resolveXRef();
}

static void PTN_Call__eval(PTN_Env *_env, const PTN_Statement *_st) { 
   const PTN_Call *st = (const PTN_Call *)_st;

   Dtracest;

   ////PTN_Env env(_env);
   ////st->expr_opt(_env, &_env->cret, st->expr);
   ////// Discard return value
   ////env.cret.unsetFast(); 
   
   YAC_Value r;
   // tkscript->printf("xxx PTN_Call__eval: st=%p\n", st);
   // tkscript->printf("xxx PTN_Call__eval: st->expr_opt=%p\n", st->expr_opt);
   // tkscript->printf("xxx PTN_Call__eval: st->getID()=%d\n", st->getID());

   if(NULL != st->expr_opt)
   {
      st->expr_opt(_env, &r, st->expr);
   }
   else
   {
      st->expr->eval(_env, &r);
   }

   // Discard return value
   r.unsetFast();
} 

void PTN_Call::eval(PTN_Env *_env) const {
   PTN_Call__eval(_env, this);
}


Fevalst PTN_Call::getEvalSt(void) const { 
   Fevalst callst = expr->getEvalCallSt();
   if(callst)
   {
      // Use optimized function
      return callst;
   }
   else
   {
      // Use generic version that simply calls expr->evalCallableExpr(_env)
      return PTN_Call__eval; 
   }
} 

const PTN_Statement *PTN_Call::getEvalStArg(void) const {
   // Check whether expression can be called as a statement
   if(expr->getEvalCallSt())
   {
      // Expression has optimized statement eval function --> return inner expr object instead of statement wrapper
      // hack: temporarily cast to statement (will be cast back to CallableExpr when evaluated)
      return (PTN_Statement*) expr; 
   }
   else
   {
      // Use generic PTN_Call__eval function
      return this;
   }
}

#ifdef TKS_JIT 
sBool PTN_Call::forceHybrid(void) { 
   return expr->forceHybrid(); 
} 

sU8 PTN_Call::compile(VMCore *_vm) { 
   if(expr->forceHybrid())  
   {
      _vm->addHybridStatement(this);  
   }
   else 
   {
      sU8 r=expr->compileCallStatement(_vm);
      if(r!=0xFF)
      { 
         if(r) 
         { 
            if(r==4) 
            { 
               Dprintf("[---] Call: variable return type not supported by JIT.\n"); 
               return 0xFF; 
            } 
            Dasmop(VMOP_INCSTP); 
         } 
      } 
      else  
      { 
         Dprintf("[---] Call: cannot compile call statement (unsupported by JIT).\n"); 
         return 0xFF; 
      } 
   }
   if(next)   
   {
      return next->compileHybridStatement(_vm); 
   }
   else   
   {
      return 0; 
   }
}

#endif
