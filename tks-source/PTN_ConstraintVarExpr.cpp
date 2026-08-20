/// PTN_ConstraintVarExpr.cpp
///
/// (c) 2005-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "PTN_Statement.h"
#include "tks_inc_jit.h"

#include "PTN_ConstraintVarExpr.h"


PTN_ConstraintVarExpr::PTN_ConstraintVarExpr(YAC_Value **_cvalref) {
   cvalref = _cvalref;
}

PTN_ConstraintVarExpr::~PTN_ConstraintVarExpr() {
}

static void PTN_ConstraintVarExpr__eval(PTN_Env *_env, YAC_Value *_r, const PTN_Expr *_st) {
   Dtracest;
   _r->initValue( * (((const PTN_ConstraintVarExpr*)_st)->cvalref) );
   (void)_env;
}

void PTN_ConstraintVarExpr::eval(PTN_Env *_env, YAC_Value *_r) const {
   (void)_env;
   _r->initValue(*cvalref);
}

sBool PTN_ConstraintVarExpr::semanticCheck(void) {
   return (NULL != cvalref);
}

Feval PTN_ConstraintVarExpr::getEval(void) const {
   return &PTN_ConstraintVarExpr__eval;
}
