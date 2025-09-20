/// PTN_RGBExpr.cpp
///
/// (c) 2001-2014 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///


#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Statement.h"
#include "PTN_Expr.h"
#include "vmopenum.h"
#include "VMCore.h"
#include "PTN_RGBExpr.h"


PTN_RGBExpr::PTN_RGBExpr(PTN_Expr *_rexpr, PTN_Expr *_gexpr, PTN_Expr *_bexpr) {
   rexpr = _rexpr;
   gexpr = _gexpr;
   bexpr = _bexpr;  

   rexpr_opt = NULL;
   gexpr_opt = NULL;
   bexpr_opt = NULL;
   ////rexpr_opt = rexpr->getEval(); // xxx
   ////gexpr_opt = gexpr->getEval(); // xxx
   ////bexpr_opt = bexpr->getEval(); // xxx
}

PTN_RGBExpr::~PTN_RGBExpr() {
   TKS_DELETE_SAFE(rexpr);

   TKS_DELETE_SAFE(gexpr);

   TKS_DELETE_SAFE(bexpr);
}

sBool PTN_RGBExpr::semanticCheck(void) {
   return  
      (rexpr ? rexpr->semanticCheck() : 0) &&  
      (gexpr ? gexpr->semanticCheck() : 0) && 
      (bexpr ? bexpr->semanticCheck() : 0) ;
}

sBool PTN_RGBExpr::resolveXRef(void) {
   return
      (rexpr->resolveXRef()) &&
      (gexpr->resolveXRef()) &&
      (bexpr->resolveXRef()) 
      ;
}

sBool PTN_RGBExpr::isConst(void) {
   return  
      (rexpr->isConst()) &&  
      (gexpr->isConst()) && 
      (bexpr->isConst()) ;
}

void PTN_RGBExpr::optimize(void) {
   tks_optimize_expr(&rexpr, YAC_TYPE_INT); 
   tks_optimize_expr(&gexpr, YAC_TYPE_INT); 
   tks_optimize_expr(&bexpr, YAC_TYPE_INT);  

   rexpr_opt = rexpr->getEval(); 
   gexpr_opt = gexpr->getEval(); 
   bexpr_opt = bexpr->getEval(); 
}

static void PTN_RGBExpr__eval(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) { 
   Dtracest;
   const PTN_RGBExpr*st = (const PTN_RGBExpr*)_st; 
   
   YAC_Value rv, gv, bv; 

   st->rexpr_opt(_env, &rv, st->rexpr); 
   if(Dhaveexception)
   {
      rv.unsetFast();
      Dhandleexception(st->rexpr);
      return;
   }

   st->gexpr_opt(_env, &gv, st->gexpr); 
   if(Dhaveexception)
   {
      rv.unsetFast();
      gv.unsetFast();
      Dhandleexception(st->gexpr);
      return;
   }

   st->bexpr_opt(_env, &bv, st->bexpr);  
   if(Dhaveexception)
   {
      rv.unsetFast();
      gv.unsetFast();
      bv.unsetFast();
      Dhandleexception(st->bexpr);
      return;
   }
 
   rv.typecast(YAC_TYPE_INT); 
   gv.typecast(YAC_TYPE_INT); 
   bv.typecast(YAC_TYPE_INT); 

   ret->initInt( 
      (sSI) (  
               (((sU32)rv.value.int_val)<<16)|(((sU32)gv.value.int_val)<<8)|(((sU32)bv.value.int_val))|(0xFF<<24)  
            )  
   );
}

Feval PTN_RGBExpr::getEval(void) const { 
   return PTN_RGBExpr__eval; 
} 

void PTN_RGBExpr::eval(PTN_Env *_env, YAC_Value *_r) const { 
   PTN_RGBExpr__eval(_env, _r, this); 
} 


#ifdef TKS_JIT 
sBool PTN_RGBExpr::forceHybrid(void) { 
   return  
      rexpr->forceHybrid() ||  
      gexpr->forceHybrid() || 
      bexpr->forceHybrid() ; 
} 

sU8 PTN_RGBExpr::compile(VMCore *_vm) {
   sU8 rr, rg, rb;
 
   rb=bexpr->compile(_vm); _vm->typecastStack(rb, YAC_TYPE_INT);
   rg=gexpr->compile(_vm); _vm->typecastStack(rg, YAC_TYPE_INT);
   rr=rexpr->compile(_vm);	_vm->typecastStack(rr, YAC_TYPE_INT); 

   if( (rr!=0xFF) && (rg!=0xFF) && (rb!=0xFF) )
   {
      _vm->pushci(255); // push alpha
      _vm->addOpcode(VMOP_SIPACKARGB);
      return 1;
   }
   return 0xFF;
}
#endif 

