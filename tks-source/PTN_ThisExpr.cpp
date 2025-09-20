/// PTN_ThisExpr.cpp
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "tks_inc_class.h"
#include "PTN_ThisExpr.h"


static void PTN_ThisExpr__eval(PTN_Env *_env, YAC_Value *_r, const PTN_Expr *_st) { 

   (void)_st;

   Dtracest;

   _r->initObject(_env->context->tksvm_class_stack_object, 0);
}

void PTN_ThisExpr::eval(PTN_Env *_env, YAC_Value *_r) const {
   Dtracethis;
   _r->initObject(_env->context->tksvm_class_stack_object, 0);
}

Feval PTN_ThisExpr::getEval(void) const {
   return PTN_ThisExpr__eval;
}
