/// PTN_CallableExpr.cpp
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "PTN_CallableExpr.h"


static void PTN_CallableExpr__eval(PTN_Env *_env, YAC_Value *_r, const PTN_Expr *_st) {
   //
   // This is a fallback function that should ideally never get called.
   //
	const PTN_CallableExpr *st = (const PTN_CallableExpr*)_st; // xxx discard const qualifier (for "this" call)

   Dtracest;

	st->evalCallableExpr(_env, _r); 

   Ddubiousfixstringtype(_r);
} 
 
void PTN_CallableExpr::eval(PTN_Env *_env, YAC_Value *_r) const {

   Dtracethis;

	evalCallableExpr(_env, _r); 

   Ddubiousfixstringtype(_r);
} 
 
Feval PTN_CallableExpr::getEval(void) const { 
	return PTN_CallableExpr__eval; 
}
 
#ifdef TKS_JIT
sU8 PTN_CallableExpr::compileCallStatement(VMCore *_vm) { 
   //
	// this should never be called since it is the default impl. of this virtual method
   //
	Dcterror(PTNERR_CANTCOMPILE);
	return 0xFF;
} 
void PTN_CallableExpr::compileCleanUpReturn(VMCore*) { 
} 
#endif
