/// PTN_ConditionalExpr.cpp
///
/// (c) 2001-2014 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h" 
#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "PTN_Statement.h"
#include "vmopenum.h"
#include "VMCore.h"
#include "PTN_ConditionalExpr.h"


PTN_ConditionalExpr::PTN_ConditionalExpr(PTN_Expr *_c,  
                                         PTN_Expr *_a,  
                                         PTN_Expr *_b 
                                         )  
{
   cond_expr = _c;
   vala_expr = _a;
   valb_expr = _b;  
   
   cond_expr_opt = cond_expr->getEval(); 
   vala_expr_opt = vala_expr->getEval(); 
   valb_expr_opt = valb_expr->getEval();
}

PTN_ConditionalExpr::~PTN_ConditionalExpr() {
   TKS_DELETE_SAFE(cond_expr);
   TKS_DELETE_SAFE(vala_expr);
   TKS_DELETE_SAFE(valb_expr);
}

sBool PTN_ConditionalExpr::semanticCheck(void) {
   return
      (cond_expr?cond_expr->semanticCheck():0) &&
      (vala_expr?vala_expr->semanticCheck():0) &&
      (valb_expr?valb_expr->semanticCheck():0) ;
}

sBool PTN_ConditionalExpr::resolveXRef(void) {
   if(cond_expr->resolveXRef())
   {
      if(vala_expr->resolveXRef())
      {
         if(valb_expr->resolveXRef())  
         { 
            return 1; 
         }
         else
         {
            Dprintf("[---] ConditionalExpr::resolveXRef: (valb_expr)\n");
         }
      }
      else
      {
         Dprintf("[---] ConditionalExpr::resolveXRef: (vala_expr)\n");
      }
   }
   else
   {
      Dprintf("[---] ConditionalExpr::resolveXRef: (cond_expr)\n");
   }
   return 0;
}

sBool PTN_ConditionalExpr::isConst(void) {
   return  
      cond_expr->isConst() && 
      vala_expr->isConst() && 
      valb_expr->isConst() ;
}

void PTN_ConditionalExpr::optimize(void) { 
   tks_optimize_expr(&cond_expr, 1);
   tks_optimize_expr(&vala_expr, 0); 
   tks_optimize_expr(&valb_expr, 0);  
   
   cond_expr_opt=cond_expr->getEval(); 
   vala_expr_opt=vala_expr->getEval(); 
   valb_expr_opt=valb_expr->getEval();
}

static void PTN_ConditionalExpr__eval(PTN_Env *_env, YAC_Value *_r, const PTN_Expr *_st) { 
   Dtracest;
   const PTN_ConditionalExpr *st = (const PTN_ConditionalExpr*)_st; 
   
   st->cond_expr_opt(_env, _r, st->cond_expr); 
   if(Dhaveexception)
   {
      _r->unset();
      Dhandleexception(st->cond_expr);
      return;
   }
   
   if(_r->value.int_val)
   {
      _r->unset(); 
      st->vala_expr_opt(_env, _r, st->vala_expr);
      if(Dhaveexception)
      {
         _r->unset();
         Dhandleexception(st->vala_expr);
         return;
      }
   }
   else
   {
      _r->unset();
      st->valb_expr_opt(_env, _r, st->valb_expr);
      if(Dhaveexception)
      {
         _r->unset();
         Dhandleexception(st->valb_expr);
         return;
      }
   }
}

Feval PTN_ConditionalExpr::getEval(void) const { 
   return PTN_ConditionalExpr__eval; 
} 

void PTN_ConditionalExpr::eval(PTN_Env *_env, YAC_Value *_r) const { 
   PTN_ConditionalExpr__eval(_env, _r, this); 
} 
