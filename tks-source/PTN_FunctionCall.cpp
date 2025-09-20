/// PTN_FunctionCall.cpp
///
/// (c) 2001-2014 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///
///


#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "PTN_Statement.h"
#include "vmopenum.h"
#include "VMCore.h"
#include "PTN_CallableExpr.h"
#include "PTN_ArgListNode.h"
#include "PTN_ArgList.h"
#include "PTN_ArgVar.h"
#include "PTN_ArgVarList.h" 
#include "TKS_FormalArgument.h"
#include "TKS_ScriptObjectID.h"
#include "TKS_FunctionObject.h"
#include "PTN_Function.h"
#include "PTN_FunctionECall.h"
#include "PTN_Call.h"
#include "PTN_FunctionCall.h"

PTN_FunctionCall::PTN_FunctionCall(PTN_CallableExpr *_ecall) {
	expr = _ecall;
}

static void PTN_FunctionCall__eval(PTN_Env *_env, const PTN_Statement *_st) { 
   Dtracest;
   const PTN_FunctionCall *st = (const PTN_FunctionCall *) _st;

   PTN_Env env(_env->context);
   st->expr_opt(&env, &env.cret, st->expr);
	env.cret.unsetFast();
	_env->continue_flag = 1; //xxx??
}

Fevalst PTN_FunctionCall::getEvalSt(void) const {
	return PTN_FunctionCall__eval;
}

void PTN_FunctionCall::eval(PTN_Env *_env) const {
	PTN_FunctionCall__eval(_env, this);
}

