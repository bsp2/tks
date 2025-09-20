/// PTN_ClampWrapStat.cpp
///
/// (c) 2001-2014 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///


#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "PTN_Statement.h"
#include "PTN_CallableExpr.h"
#include "PTN_Call.h"
#include "vmopenum.h"
#include "VMCore.h"

#include "YAC_Value.h"
#include "TKS_CachedObject.h"
#include "PTN_ClampWrapStat.h"

 
PTN_ClampWrapStat::PTN_ClampWrapStat(sU8               _mode,  
                                     TKS_CachedObject *_var,  
                                     YAC_Value         _min,  
                                     YAC_Value         _max 
                                     )  
{
   mode = _mode;
   var  = _var;
   min  = _min;
   max  = _max;
}

PTN_ClampWrapStat::~PTN_ClampWrapStat() {
}

sBool PTN_ClampWrapStat::semanticCheck(void) {
   if(var)
   {
      if((var->type>=YAC_TYPE_OBJECT) || min.type>=YAC_TYPE_OBJECT || max.type>=YAC_TYPE_OBJECT)
      {
         // TODO: support object clamp/wrap call
         return 0;
      }
      if(YAC_TYPE_INT == var->type)
      {
         if(MD_CLAMP == mode)
         {
            mode = MD_CLAMPI;
         }
         else
         {
            mode = MD_WRAPI;
         }
         if(YAC_TYPE_INT != min.type)
         {
            min.value.int_val = (sSI)min.value.float_val;
         }
         if(YAC_TYPE_INT != max.type)
         {
            max.value.int_val = (sSI)max.value.float_val;
         }
      }
      else
      {
         if(MD_CLAMP == mode)
         {
            mode=MD_CLAMPF;
         }
         else
         {
            mode=MD_WRAPF;
         }
         if(YAC_TYPE_FLOAT != min.type)
         {
            min.value.float_val = (sF32)min.value.int_val;
         }
         if(YAC_TYPE_FLOAT != max.type)
         {
            max.value.float_val=(sF32)max.value.int_val;
         }
      }

      return 1;
   }
   else
   {
      return 0;
   }
}

static void PTN_ClampWrapStat__eval(PTN_Env *_env, const PTN_Statement *_st) { 
   const PTN_ClampWrapStat *st = (const PTN_ClampWrapStat*)_st; 
   Dtracest;

   YAC_Value *co=Dgetvar(st->var); 
   
   switch(st->mode)
   {
   case PTN_ClampWrapStat::MD_CLAMPI:
      if(co->value.int_val<st->min.value.int_val)
         co->value.int_val=st->min.value.int_val;
      else if(co->value.int_val>st->max.value.int_val)
         co->value.int_val=st->max.value.int_val;
      break;
   case PTN_ClampWrapStat::MD_CLAMPF:
      if(co->value.float_val<st->min.value.float_val)
         co->value.float_val=st->min.value.float_val;
      else if(co->value.float_val>st->max.value.float_val)
         co->value.float_val=st->max.value.float_val;
      break;
   case PTN_ClampWrapStat::MD_WRAPI:
      // (todo) this code fails if value is < minVal-(max-min) or > maxVal+(max-min)
      if(co->value.int_val<st->min.value.int_val)
         co->value.int_val=(co->value.int_val-st->min.value.int_val)+st->max.value.int_val;
      else if(co->value.int_val>=st->max.value.int_val)
         co->value.int_val=st->min.value.int_val+(co->value.int_val-st->max.value.int_val);
      break;
   case PTN_ClampWrapStat::MD_WRAPF:
      if(co->value.float_val<st->min.value.float_val)
         co->value.float_val=(co->value.float_val-st->min.value.float_val)+st->max.value.float_val;
      else if(co->value.float_val>=st->max.value.float_val)
         co->value.float_val=st->min.value.float_val+(co->value.float_val-st->max.value.float_val);
      break;
   }
}

Fevalst PTN_ClampWrapStat::getEvalSt(void) const { 
   return PTN_ClampWrapStat__eval; 
}

void PTN_ClampWrapStat::eval(PTN_Env *_env) const {
   PTN_ClampWrapStat__eval(_env, this);
}
