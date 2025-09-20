/// PTN_ARGBExpr.cpp
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
#include "PTN_ARGBExpr.h"

 
PTN_ARGBExpr::PTN_ARGBExpr(PTN_Expr *_aexpr, PTN_Expr *_rexpr, PTN_Expr *_gexpr, PTN_Expr *_bexpr) {
   rexpr     = _rexpr;
   gexpr     = _gexpr;
   bexpr     = _bexpr;
   aexpr     = _aexpr;

   b_rgba_4b = YAC_FALSE;
   
   rexpr_opt = rexpr->getEval();
   gexpr_opt = gexpr->getEval();
   bexpr_opt = bexpr->getEval();
   aexpr_opt = aexpr->getEval();
}

PTN_ARGBExpr::~PTN_ARGBExpr() {
   TKS_DELETE_SAFE(aexpr);
   TKS_DELETE_SAFE(rexpr);
   TKS_DELETE_SAFE(gexpr);
   TKS_DELETE_SAFE(bexpr);
}

sBool PTN_ARGBExpr::semanticCheck(void) {
   return  
      (rexpr ? rexpr->semanticCheck() : YAC_FALSE) &&  
      (gexpr ? gexpr->semanticCheck() : YAC_FALSE) && 
      (bexpr ? bexpr->semanticCheck() : YAC_FALSE) && 
      (aexpr ? aexpr->semanticCheck() : YAC_FALSE) ;
}

sBool PTN_ARGBExpr::resolveXRef(void) {
   if(rexpr&&rexpr->resolveXRef()) 
   {
      if(gexpr&&gexpr->resolveXRef()) 
      {
         if(bexpr&&bexpr->resolveXRef()) 
         {
            return aexpr&&aexpr->resolveXRef(); 
         } 
      } 
   }
   return 0;
}

void PTN_ARGBExpr::optimize(void) {
   tks_optimize_expr(&rexpr, 1); 
   tks_optimize_expr(&gexpr, 1); 
   tks_optimize_expr(&bexpr, 1); 
   tks_optimize_expr(&aexpr, 1); 

   rexpr_opt = rexpr->getEval(); 
   gexpr_opt = gexpr->getEval(); 
   bexpr_opt = bexpr->getEval(); 
   aexpr_opt = aexpr->getEval(); 
}

static void PTN_ARGBExpr__eval(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) { 
   const PTN_ARGBExpr *YAC_RESTRICT st = (const PTN_ARGBExpr *YAC_RESTRICT)_st; 
   
   Dtracest;
   
   YAC_Value rv, gv, bv, av;
    
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

   st->aexpr_opt(_env, &av, st->aexpr); 

   if(Dhaveexception)
   {
      rv.unsetFast();
      gv.unsetFast();
      bv.unsetFast();
      av.unsetFast();
      Dhandleexception(st->aexpr);
      return;
   }
    
   rv.typecast(YAC_TYPE_INT); 
   gv.typecast(YAC_TYPE_INT); 
   bv.typecast(YAC_TYPE_INT); 
   av.typecast(YAC_TYPE_INT); 
    
   if(st->b_rgba_4b)
   {
      // r,g,b,a byte-order in memory
      //  (note) argb => rgba expr order
      ret->type = YAC_TYPE_INT;
      sU8 *d = (sU8*)&ret->value.int_val;
#ifdef YAC_LITTLE_ENDIAN
      d[0] = (sU8)av.value.int_val;
      d[1] = (sU8)rv.value.int_val;
      d[2] = (sU8)gv.value.int_val;
      d[3] = (sU8)bv.value.int_val;
#else
      d[3] = (sU8)av.value.int_val;
      d[2] = (sU8)rv.value.int_val;
      d[1] = (sU8)gv.value.int_val;
      d[0] = (sU8)bv.value.int_val;
#endif // YAC_LITTLE_ENDIAN
      // (note) see tks-examples/rgba_4b.tks testcase
   }
   else
   {
      // Packed ARGB32 32bit
      ret->initInt( 
         (sSI)  
         (  
         (((sU32)rv.value.int_val)<<16) |  
         (((sU32)gv.value.int_val)<<8)  | 
         (((sU32)bv.value.int_val))     | 
         (((sU32)av.value.int_val)<<24)  
         )  
         );
   }
}

Feval PTN_ARGBExpr::getEval(void) const {
   return PTN_ARGBExpr__eval; 
} 

void PTN_ARGBExpr::eval(PTN_Env *_env, YAC_Value *_r) const {
   PTN_ARGBExpr__eval(_env, _r, this); 
} 

#ifdef TKS_JIT 
sBool PTN_ARGBExpr::forceHybrid(void) { 
   return
      rexpr->forceHybrid() |  
      gexpr->forceHybrid() |  
      bexpr->forceHybrid() | 
      aexpr->forceHybrid() ; 
} 

sU8 PTN_ARGBExpr::compile(VMCore *_vm) {
   sU8 rr, rg, rb, ra;
    
   if(b_rgba_4b)
   {
      //  (note) argb => rgba expr order
#ifdef YAC_LITTLE_ENDIAN
      //  3210
      //  ARGB
      ra=rexpr->compile(_vm); _vm->typecastStack(ra, YAC_TYPE_INT);
      rr=gexpr->compile(_vm); _vm->typecastStack(rr, YAC_TYPE_INT);
      rg=bexpr->compile(_vm); _vm->typecastStack(rg, YAC_TYPE_INT);
      rb=aexpr->compile(_vm); _vm->typecastStack(rb, YAC_TYPE_INT);
#else
      //  0123
      //  ARGB
      rb=aexpr->compile(_vm); _vm->typecastStack(rb, YAC_TYPE_INT);
      rg=bexpr->compile(_vm); _vm->typecastStack(rg, YAC_TYPE_INT);
      rr=gexpr->compile(_vm); _vm->typecastStack(rr, YAC_TYPE_INT);
      ra=rexpr->compile(_vm); _vm->typecastStack(ra, YAC_TYPE_INT);
#endif
   }
   else
   {
      rb=bexpr->compile(_vm); _vm->typecastStack(rb, YAC_TYPE_INT);
      rg=gexpr->compile(_vm); _vm->typecastStack(rg, YAC_TYPE_INT);
      rr=rexpr->compile(_vm); _vm->typecastStack(rr, YAC_TYPE_INT);
      ra=aexpr->compile(_vm); _vm->typecastStack(ra, YAC_TYPE_INT);
   }
 
   if( (rr!=0xFF) && (rg!=0xFF) && (rb!=0xFF) && (ra!=0xFF) )
   {
      _vm->addOpcode(VMOP_SIPACKARGB);
      return 1;
   }

   return 0xFF;
}
#endif

sBool PTN_ARGBExpr::isConst(void) {
   return  
      (rexpr->isConst()) &&  
      (gexpr->isConst()) && 
      (bexpr->isConst()) && 
      (aexpr->isConst()) ;
}
