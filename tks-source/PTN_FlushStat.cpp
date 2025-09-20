/// PTN_FlushStat.cpp
///
/// (c) 2001-2014 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include <stdio.h>
#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "PTN_Statement.h"
#include "vmopenum.h"
#include "VMCore.h"
#include "PTN_FlushStat.h"


PTN_FlushStat::PTN_FlushStat(PTN_Expr *_expr) {
   expr     = _expr; 
   expr_opt = expr->getEval();
}

PTN_FlushStat::~PTN_FlushStat() {
   TKS_DELETE_SAFE(expr);
}

sBool PTN_FlushStat::semanticCheck(void) {
    return 
       (expr ? expr->semanticCheck() : 0)
       ;
}

void PTN_FlushStat::optimize(void) {
   tks_optimize_expr(&expr, 0); 

   expr_opt = expr->getEval(); 
}


sBool PTN_FlushStat::resolveXRef(void) {
   if(expr)
   {
      return expr->resolveXRef();
   }
   else
   {
      return 0;
   }
}

static void PTN_FlushStat__eval(PTN_Env *_env, const PTN_Statement *_st) { 
   Dtracest;
   const PTN_FlushStat *st = (const PTN_FlushStat*)_st; 
   
   YAC_Value r;
   st->expr_opt(_env, &r, st->expr);
   if(Dhaveexception)
   {
      r.unsetFast();
      Dhandleexception(st->expr);
      return;
   }

   sSI ch = 0;
   switch(r.type)
   {
   case 0:
      // ---- void ----
      break;
   case 1:
      // ---- int ----
      ch=r.value.int_val;
      break;
   case 2:
      // ---- float ----
      ch=(sU8)r.value.float_val;
      break;
   case 3:
      // ---- object ---- 
      r.unsetFast();
      break;
   case 4:
      // ---- string ---- 
      if(TKS_VALID(r.value.string_val))
      { 
         sSI iv;
         r.value.string_val->yacScanI(&iv);
         r.unsetFast(); 
      }
      else
      {
         Drtthrowinvalidpointer(_st, "cannot convert invalid object to stream-id", r.value.object_val);
      } 
      break;
   }
   FILE *fch;
   switch(ch)
   {
   case 1:
      fch = stdout;
      break;
   case 2:
      fch = stderr;
      break;
   default:
      fch = 0; // flush all
      break;
   }
   ::fflush(fch);
}

Fevalst PTN_FlushStat::getEvalSt(void) const { 
   return PTN_FlushStat__eval; 
} 

void PTN_FlushStat::eval(PTN_Env *_env) const { 
   PTN_FlushStat__eval(_env, this); 
} 
