/// PTN_DynamicVarAssign.cpp
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
#include "PTN_VarAssign.h"
#include "PTN_DynamicVarAssign.h"


PTN_DynamicVarAssign::PTN_DynamicVarAssign(TKS_CachedObject * _var,  
                                           PTN_Expr         * _expr 
                                           )  
                                           : PTN_VarAssign(_var, _expr)
{
}

static void PTN_DynamicVarAssign__eval(PTN_Env *_env, const PTN_Statement *_st) { 
   Dtracest;
   const PTN_DynamicVarAssign *st = (const PTN_DynamicVarAssign*)_st; 

   YAC_Value *co = Dgetvar(st->var); 
   YAC_Value r;
   st->expr_opt(_env, &r, st->expr); 
   
   sBool bSkipUnset = 0;

   if(co->type >= YAC_TYPE_OBJECT)
   {
      if(r.type >= YAC_TYPE_OBJECT)
      {
         if((void*)r.value.object_val == (void*)co->value.object_val)
         {
            // don't unset self
            bSkipUnset = 1;
         }
      }
   }

   if(!bSkipUnset)
   {
      co->unset();
   }

   co->derefPointerAssignValue(&r);

   if(Dhaveexception)
   {
      co->unset();
      Dhandleexception(st->expr);
      return;
   }
} 

Fevalst PTN_DynamicVarAssign::getEvalSt(void) const { 
   return PTN_DynamicVarAssign__eval; 
} 

void PTN_DynamicVarAssign::eval(PTN_Env *_env) const { 
   PTN_DynamicVarAssign__eval(_env, this); 
} 

#ifdef TKS_JIT 
sBool PTN_DynamicVarAssign::forceHybrid(void) { 
   return 1; 
} 
#endif 
