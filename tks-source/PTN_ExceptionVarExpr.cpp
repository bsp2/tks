/// PTN_ExceptionVarExpr.cpp
///
/// (c) 2006-2008 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "PTN_ExceptionVarExpr.h"


PTN_ExceptionVarExpr::PTN_ExceptionVarExpr(sBool _bDeref) {
   b_deref = _bDeref;
}

PTN_ExceptionVarExpr::~PTN_ExceptionVarExpr() {
}

void PTN_ExceptionVarExpr::eval(PTN_Env *_env, YAC_Value *_r) const {
   if(b_deref)
      _r->copySafe(&_env->context->current_exception_var);
   else
      _r->initValue(&_env->context->current_exception_var);
}

static void PTN_ExceptionVarExpr__eval(PTN_Env *_env, YAC_Value *_r, const PTN_Expr *_st) { 
   Dtracest;
   _r->initValue(&_env->context->current_exception_var);
   (void)_st;
}

static void PTN_ExceptionVarExpr__eval_deref(PTN_Env *_env, YAC_Value *_r, const PTN_Expr *_st) { 
   Dtracest;
   _r->copySafe(&_env->context->current_exception_var);
   (void)_st;
}

Feval PTN_ExceptionVarExpr::getEval(void) const {
   if(b_deref)
   {
      return PTN_ExceptionVarExpr__eval_deref;
   }
   else
   {
      return PTN_ExceptionVarExpr__eval;
   }
}
