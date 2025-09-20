/// PTN_Return.cpp
///
/// (c) 2001-2014 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///


#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "PTN_Statement.h"
#include "vmopenum.h"
#include "VMCore.h" 
#include "tks_inc_class.h"
#include "tks_inc_compiler.h" 
#include "PTN_Return.h" 


PTN_Return::PTN_Return(PTN_Expr *_retexpr, PTN_Function *_fun, sBool _bBreak) {
   ret_expr     = _retexpr; 
   ret_expr_opt = NULL;
   fun          = _fun;
   b_break      = _bBreak;
}

PTN_Return::~PTN_Return() {
   TKS_DELETE_SAFE(ret_expr);
}

sBool PTN_Return::semanticCheck(void) {
   return
      (ret_expr ? ret_expr->semanticCheck() : 1)
      ;
}

sBool PTN_Return::resolveXRef(void) {
   return
      (ret_expr ? ret_expr->resolveXRef() : 1)
      ;
}

void PTN_Return::optimize(void) {
   if(NULL != ret_expr)
   { 
      tks_optimize_expr(&ret_expr, fun->return_type); 
      ret_expr_opt = ret_expr->getEval(); 
   }
}

static void PTN_Return__eval_RV(PTN_Env *_env, const PTN_Statement *_st) {  
   Dtracest;
   const PTN_Return *st = (const PTN_Return*)_st;

   YAC_Value r;
   st->ret_expr_opt(_env, &r, st->ret_expr);
   if(Dhaveexception)
   {
      r.unset();
      Dhandleexception(st->ret_expr);
      return;
   }
   else
   {
      _env->cret.copySafe(&r);
   }
   if(st->b_break) // return <expr>; or return=<expr>  ?
   {
      _env->continue_flag = 0;
   }
}
 
static void PTN_Return__eval(PTN_Env *_env, const PTN_Statement *_st) {   
   Dtracest;
   (void)_st;

   // Note: no explicit return_expr -- return value has possibly been set already using return=<expr>;
   _env->continue_flag = 0;
} 

Fevalst PTN_Return::getEvalSt(void) const {
   if(NULL != ret_expr) 
   { 
      return PTN_Return__eval_RV; // return something
   } 
   else 
   {
      return PTN_Return__eval; // variant return type 
   }
} 

void PTN_Return::eval(PTN_Env *_env) const {   
   if(NULL != ret_expr) 
   { 
      PTN_Return__eval_RV(_env, this);  
   } 
   else 
   { 
      _env->continue_flag = 0; // same as PTN_Return__eval(_env, this), but w/o fxn call
   }
} 


#ifdef TKS_JIT 
sBool PTN_Return::forceHybrid(void) { 
   return 0; // must use interpreted exec because PTN_Env is not available in JIT 
} 

sU8 PTN_Return::compile(VMCore *_vm) {
   Dprintf("[---] Return: not supported by JIT compiler.\n");
   Dcterror(PTNERR_CANTCOMPILE);
   return 0xFF;
}
#endif

