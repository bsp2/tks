/// PTN_DynamicVarPointerAssign.cpp
///
/// (c) 2003-2014 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///


#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "PTN_Statement.h"
#include "vmopenum.h"
#include "VMCore.h"
 
#include "PTN_PointerAssign.h"
#include "PTN_DynamicVarPointerAssign.h"


PTN_DynamicVarPointerAssign::PTN_DynamicVarPointerAssign(TKS_CachedObject * _co,  
                                                         PTN_Expr         * _expr 
                                                         )  
                                                         : PTN_PointerAssign(_co, _expr)
{
}
 
static void PTN_DynamicVarPointerAssign__eval(PTN_Env *_env, const PTN_Statement *_st) {  
   Dtracest;
	const PTN_DynamicVarPointerAssign*st = (const PTN_DynamicVarPointerAssign*)_st; 

	YAC_Value *co = Dgetvar(st->left_co); 
 
	co->unset(); 
	st->right_expr_opt(_env, co, st->right_expr); 
   if(Dhaveexception)
   {
      co->unset();
      Dhandleexception(st->right_expr);
      return;
   }
} 
 
Fevalst PTN_DynamicVarPointerAssign::getEvalSt(void) const { 
	return PTN_DynamicVarPointerAssign__eval; 
} 
 
void PTN_DynamicVarPointerAssign::eval(PTN_Env *_env) const {
	PTN_DynamicVarPointerAssign__eval(_env, this); 
}

#ifdef TKS_JIT 
sBool PTN_DynamicVarPointerAssign::forceHybrid(void) { 
	return 1; 
} 
#endif 
