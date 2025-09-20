/// PTN_OpDynamicVarAssign.cpp
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
#include "PTN_VarAssign.h"
#include "PTN_OpVarAssign.h"
#include "PTN_OpDynamicVarAssign.h"
#include "PTN_DoubleArgExpr.h"


PTN_OpDynamicVarAssign::PTN_OpDynamicVarAssign(TKS_CachedObject *_co, PTN_Expr *_expr, sU8 _op)  
: PTN_OpVarAssign(_co, _expr, _op) { } // call parent class constructor

static void PTN_OpDynamicVarAssign__eval(PTN_Env *_env, const PTN_Statement *_st) { 
   Dtracest;
   const PTN_OpDynamicVarAssign*st = (const PTN_OpDynamicVarAssign*)_st; 

   YAC_Value *co=Dgetvar(st->var); 
   
   YAC_Value re; 
   st->expr_opt(_env, &re, st->expr); 
   if(Dhaveexception)
   {
      re.unsetFast();
      Dhandleexception(st->expr);
      return;
   }
   
   YAC_Value r; 
   PTN_DoubleArgExpr::EvalOp(_env, &r, co, &re, st->expr_operator, _st); 
   *co=&r; 
   re.unsetFast();
} 

Fevalst PTN_OpDynamicVarAssign::getEvalSt(void) const { 
   return PTN_OpDynamicVarAssign__eval; 
} 

void PTN_OpDynamicVarAssign::eval(PTN_Env *_env) const { 
   PTN_OpDynamicVarAssign__eval(_env, this); 
} 


#ifdef TKS_JIT 
sBool PTN_OpDynamicVarAssign::forceHybrid(void) { 
   return 1; 
} 
#endif 

