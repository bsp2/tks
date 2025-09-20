/// PTN_NewClassExpr.cpp
///
/// (c) 2001-2014 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "tks_inc_class.h"
#include "PTN_ConstVal.h"
#include "vmopenum.h"
#include "VMCore.h"
#include "PTN_NewClassExpr.h"



PTN_NewClassExpr::PTN_NewClassExpr(void) {
   class_decl=0;
}

PTN_NewClassExpr::PTN_NewClassExpr(TKS_ClassDecl *_cd) {
   class_decl=_cd;
}

PTN_NewClassExpr::~PTN_NewClassExpr() {
   class_decl=0;
}

sBool PTN_NewClassExpr::semanticCheck(void) {
   if(class_decl!=0)
   {
      return (class_decl->is_declared);
   }
   return 0;
}

static void PTN_NewClassExpr__eval(PTN_Env *_env, YAC_Value *_r, const PTN_Expr *_st) { 
   Dtracest;
   const PTN_NewClassExpr*st = (const PTN_NewClassExpr*)_st; 
   
   if(st->class_decl)  
   {
      _r->initObject(YAC_CLONE_POOLED(_env->context, st->class_decl->class_template), 1);  
   }
} 

void PTN_NewClassExpr::eval(PTN_Env *_env, YAC_Value *_r) const { 
   PTN_NewClassExpr__eval(_env, _r, this); 
}

Feval PTN_NewClassExpr::getEval(void) const { 
   return PTN_NewClassExpr__eval; 
}
