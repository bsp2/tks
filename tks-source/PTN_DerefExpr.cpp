/// PTN_DerefExpr.cpp
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
 
#include "PTN_DerefExpr.h"


PTN_DerefExpr::PTN_DerefExpr(TKS_CachedObject *_var) {
   var = _var;
}

PTN_DerefExpr::~PTN_DerefExpr() {
   var = 0;
}

sBool PTN_DerefExpr::semanticCheck(void) {
   return (var != 0);
}

static void PTN_DerefExpr__eval(PTN_Env *_env, YAC_Value *_r, const PTN_Expr *_st) { 
   Dtracest;
   const PTN_DerefExpr*st = (const PTN_DerefExpr*)_st; 

   YAC_Value *co = Dgetvar(st->var); 
   
   //::printf("DEREF co type=%i value=%i deleteme=%i\n", co->type, co->value.int_val, co->deleteme);
   
   if(co->type >= YAC_TYPE_OBJECT)
   { 
      if(YAC_VALID(co->value.object_val))
      { 
         _r->value.object_val = co->value.object_val; 
         _r->type             = YAC_TYPE_OBJECT + (_r->value.object_val->class_ID==YAC_CLID_STRING); 
         _r->deleteme         = co->deleteme; 
         co->deleteme         = 0; 
      } 
      else  
      {
         _r->initNull(); 
      }
   }
   else
   {
      // Keep simple "int" and "float" values
      // This is mostly useful when using dynamically typed variables
      _r->type = co->type;
      _r->value.any = co->value.any;
   }
}

void PTN_DerefExpr::eval(PTN_Env *_env, YAC_Value *_r) const { 
   PTN_DerefExpr__eval(_env, _r, this); 
} 

Feval PTN_DerefExpr::getEval(void) const {
   return PTN_DerefExpr__eval; 
}
