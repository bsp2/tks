/// PTN_VarDecl.cpp
///
/// (c) 2001-2014 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

/// Note: this should be removed since it duplicates the functionality of PTN_VarAssign..

#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "PTN_Statement.h"
#include "vmopenum.h"
#include "VMCore.h"
#include "PTN_VarAssign.h"
#include "PTN_VarDecl.h"


PTN_VarDecl::PTN_VarDecl(void) {
   var  = NULL;
   expr = NULL;
}

PTN_VarDecl::PTN_VarDecl(TKS_CachedObject *_co, PTN_Expr *_expr) {
   var  = _co;
   expr = _expr; 
   if(expr)  
   {
      expr_opt = expr->getEval();  /// xxx move to optimize()
   }
}
PTN_VarDecl::~PTN_VarDecl() {
   var = NULL;
   
   TKS_DELETE_SAFE(expr);
}

sBool PTN_VarDecl::semanticCheck(void) {
   return 
      (var) &&
      (expr ? expr->semanticCheck() : 1)
      ;
}

sBool PTN_VarDecl::resolveXRef(void) {
   return
      (expr ? expr->resolveXRef() : 1)
      ;
}

void PTN_VarDecl::optimize(void) { 
   if(expr) 
   {
      tks_optimize_expr(&expr, 0); 
      expr_opt = expr->getEval(); 
   } 
}

static void PTN_VarDecl__eval(PTN_Env *_env, const PTN_Statement *_st) { 
   Dtracest;
   const PTN_VarDecl *st = (const PTN_VarDecl*)_st; 
   
   YAC_Value r;
   st->expr_opt(_env, &r, st->expr); 
   if(Dhaveexception)
   {
      r.unsetFast();
      Dhandleexception(st->expr);
      return;
   }

   Dgetvar(st->var)->assignValue(&r);
   r.unsetFast(); 
} 

static void PTN_VarDecl__eval_dynamic(PTN_Env *_env, const PTN_Statement *_st) { 
   Dtracest;
   const PTN_VarDecl *st = (const PTN_VarDecl*)_st;
   
   YAC_Value *co = Dgetvar(st->var);
   st->expr_opt(_env, co, st->expr); 
   if(Dhaveexception)
   {
      Dhandleexception(st->expr);
      return;
   }
} 

Fevalst PTN_VarDecl::getEvalSt(void) const { 
   if(expr)  
   {
      if(var->flags & TKS_CachedObject::ISDYNAMIC)
      {
         return PTN_VarDecl__eval_dynamic;
      }
      else
      {
         return PTN_VarDecl__eval;
      }
   }
   else  
   {
      return 0;  
   }
} 

void PTN_VarDecl::eval(PTN_Env *_env) const { 
   if(expr)  
   {
      PTN_VarDecl__eval(_env, this);  
   }
} 

