/// PTN_DynTraceStat.cpp
///
/// (c) 2001-2014 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include <stdio.h>
#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "PTN_Statement.h"
#include "vmopenum.h"
#include "VMCore.h"
#include "PTN_DynTraceStat.h"


PTN_DynTraceStat::PTN_DynTraceStat(PTN_Expr *_enable_expr) {
   enable_expr = _enable_expr;
}

PTN_DynTraceStat::~PTN_DynTraceStat() { 
   TKS_DELETE_SAFE(enable_expr);
}
 
sBool PTN_DynTraceStat::resolveXRef(void) { 
   return enable_expr->resolveXRef();
}
 
sBool PTN_DynTraceStat::semanticCheck(void) { 
   return (NULL != enable_expr);
} 
 
void PTN_DynTraceStat::optimize(void) { 
   tks_optimize_expr(&enable_expr, 1);  
   enable_expr_opt = enable_expr->getEval();  
} 

static void PTN_DynTraceStat__eval(PTN_Env *_env, const PTN_Statement *_st) { 
   Dtracest;
   YAC_Value r; 
   const PTN_DynTraceStat *st = (const PTN_DynTraceStat*)_st; 
   st->enable_expr_opt(_env, &r, st->enable_expr); 
   if(Dhaveexception)
   {
      r.unsetFast();
      Dhandleexception(st->enable_expr);
      return;
   }
   tkscript->dtrace_enabled = (r.value.int_val!=0);
}

Fevalst PTN_DynTraceStat::getEvalSt(void) const { 
   return PTN_DynTraceStat__eval; 
} 

void PTN_DynTraceStat::eval(PTN_Env *_env) const {  
   PTN_DynTraceStat__eval(_env, this);
} 
