/// PTN_ObjClampWrapStat.cpp
///
/// (c) 2002-2014 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "PTN_Statement.h"
#include "PTN_ConstVal.h"
#include "vmopenum.h"
#include "VMCore.h"
#include "PTN_ClampWrapStat.h"
#include "PTN_ObjClampWrapStat.h"


PTN_ObjClampWrapStat::PTN_ObjClampWrapStat(sU8 _mode, TKS_CachedObject *_var, PTN_Expr *_min, PTN_Expr *_max) { 
   mode   = _mode; 
   var    = _var; 
   min    = _min; 
   max    = _max; 
   min_opt= min->getEval(); 
   max_opt= max->getEval(); 
} 

PTN_ObjClampWrapStat::~PTN_ObjClampWrapStat() {
   TKS_DELETE_SAFE(min);
   
   TKS_DELETE_SAFE(max);
} 


sBool PTN_ObjClampWrapStat::semanticCheck(void) { 
   return 
      ((var) && (YAC_TYPE_OBJECT == var->type))
      ;
} 

void PTN_ObjClampWrapStat::optimize(void) { 
   tks_optimize_expr(&min, 0); 
   tks_optimize_expr(&max, 0); 
   min_opt = min->getEval(); 
   max_opt = max->getEval(); 
} 


sBool PTN_ObjClampWrapStat::resolveXRef(void) { 
   return
      (min->resolveXRef()) &&
      (max->resolveXRef())
      ;
} 


static void PTN_ObjClampWrapStat__eval(PTN_Env *_env, const PTN_Statement *_st) {  
   Dtracest;
   const PTN_ObjClampWrapStat *st = (const PTN_ObjClampWrapStat*)_st; 

   YAC_Value *co=Dgetvar(st->var); 
   
   YAC_Value rmin; 
   YAC_Value rmax; 
   st->min_opt(_env, &rmin, st->min); 
   if(Dhaveexception)
   {
      rmin.unsetFast();
      Dhandleexception(st->min);
      return;
   }

   st->max_opt(_env, &rmax, st->max); 
   if(Dhaveexception)
   {
      rmin.unsetFast();
      rmax.unsetFast();
      Dhandleexception(st->max);
      return;
   }

   if((rmin.type==YAC_TYPE_OBJECT) && (rmax.type==YAC_TYPE_OBJECT)) 
   { 
      YAC_Object *o=co->value.object_val; 
      if(o) 
      { 
         if(st->mode==PTN_ClampWrapStat::MD_WRAP)  
         {
            o->yacOperatorWrap(rmin.value.object_val, rmax.value.object_val);  
         }
         else  
         {
            o->yacOperatorClamp(rmin.value.object_val, rmax.value.object_val);  
         }
      } 
   } 
   rmin.unsetFast(); 
   rmax.unsetFast(); 
} 

Fevalst PTN_ObjClampWrapStat::getEvalSt(void) const { 
   return PTN_ObjClampWrapStat__eval; 
} 

void PTN_ObjClampWrapStat::eval(PTN_Env *_env) const { 
   PTN_ObjClampWrapStat__eval(_env, this); 
}
