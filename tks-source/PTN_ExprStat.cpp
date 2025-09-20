/// PTN_ExprStat.cpp
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
#include "PTN_ExprStat.h"


PTN_ExprStat::PTN_ExprStat(PTN_Expr *_expr) {  
   expr     = _expr;  
   expr_opt = expr->getEval(); 
} 

PTN_ExprStat::~PTN_ExprStat() {
   TKS_DELETE_SAFE(expr);
}

sBool PTN_ExprStat::semanticCheck(void) {
   if(expr)
   {
      return expr->semanticCheck();
   }
   else
   {
      return 0;
   }
}

sBool PTN_ExprStat::resolveXRef(void) {
   if(expr) 
   {
      return expr->resolveXRef();
   }
   else
   {
      return 0;
   }
}

void PTN_ExprStat::optimize(void) {
   if(expr)  
   { 
      expr->optimize(); 
      expr_opt = expr->getEval(); 
   }
}


static void PTN_ExprStat__eval(PTN_Env *_env, const PTN_Statement *_st) { 
   Dtracest;

   YAC_Value r;
   const PTN_ExprStat *st = (const PTN_ExprStat*) _st;
   st->expr_opt(_env, &r, st->expr); 
   if(Dhaveexception)
   {
      r.unsetFast();
      Dhandleexception(st->expr);
      return;
   }
   r.unsetFast();
}

Fevalst PTN_ExprStat::getEvalSt(void) const {
   return PTN_ExprStat__eval;
}

void PTN_ExprStat::eval(PTN_Env *_env) const {
   PTN_ExprStat__eval(_env, this);
}
