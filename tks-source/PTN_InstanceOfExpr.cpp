/// PTN_InstanceOfExpr.cpp
///
/// (c) 2004-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "tks_inc_class.h"
#include "PTN_InstanceOfExpr.h"


PTN_InstanceOfExpr::PTN_InstanceOfExpr(PTN_Expr *_this_expr, PTN_Expr *_object_expr) {
   this_expr   = _this_expr;
   object_expr = _object_expr;
}

PTN_InstanceOfExpr::~PTN_InstanceOfExpr() {
   if(this_expr)
   {
      delete this_expr; this_expr = NULL;
   }
   if(object_expr)
   {
      delete object_expr; object_expr = NULL;
   }
}

static void PTN_InstanceOfExpr__eval(PTN_Env *_env, YAC_Value *_r, const PTN_Expr *_st) { 
   Dtracest;
   const PTN_InstanceOfExpr *e = (const PTN_InstanceOfExpr*)_st;

   YAC_Value lv; 
   e->this_expr_opt(_env, &lv, e->this_expr);
   if(Dhaveexception)
   {
      lv.unsetFast();
      Dhandleexception(e->this_expr);
      return;
   }

   YAC_Value rv; 
   e->object_expr_opt(_env, &rv, e->object_expr);
   if(Dhaveexception)
   {
      lv.unsetFast();
      rv.unsetFast();
      Dhandleexception(e->object_expr);
      return;
   }

   if(lv.type>=YAC_TYPE_OBJECT) 
   {
      if(rv.type>=YAC_TYPE_OBJECT)
      { 
         if(YAC_VALID(lv.value.object_val)) 
         { 
            if(YAC_VALID(rv.value.object_val)) 
            {
               if(YAC_IS_METACLASS(rv.value.object_val))
               {
                  // ---- compare to meta class 
                  _r->initInt(lv.value.object_val->yacMetaClassInstanceOf(rv.value.object_val));
               }
               else
               {
                  // ---- compare to native class
                  _r->initInt(lv.value.object_val->yacInstanceOf(rv.value.object_val));
               }
               lv.unsetFast();
               rv.unsetFast();
               return; 
            }
         } 
         else 
         { 
            _r->initInt(0); 
         }
      } 
      else 
      { 
         _r->initInt(0); 
      } 
   }
   lv.unsetFast();
   rv.unsetFast();
   _r->initInt(0); 
}

void PTN_InstanceOfExpr::eval(PTN_Env *_env, YAC_Value *_r) const {
   PTN_InstanceOfExpr__eval(_env, _r, this);
}

Feval PTN_InstanceOfExpr::getEval(void) const {
   return PTN_InstanceOfExpr__eval;
}

sBool PTN_InstanceOfExpr::semanticCheck(void) {
   if(this_expr) 
   {
      if(this_expr->semanticCheck()) 
      {
         if(object_expr) 
         {
            return object_expr->semanticCheck(); 
         } 
      } 
   }
   return 0;
}

sBool PTN_InstanceOfExpr::resolveXRef(void) {
   if(this_expr) 
   {
      if(this_expr->resolveXRef()) 
      {
         if(object_expr) 
         {
            return object_expr->resolveXRef(); 
         } 
      } 
   }
   return 0;
}

sBool PTN_InstanceOfExpr::isConst(void) {
   return this_expr->isConst() && object_expr->isConst();
}

void PTN_InstanceOfExpr::optimize(void) {
   tks_optimize_expr(&this_expr,   YAC_TYPE_VARIANT); // don't cast int/float/void to objects..
   tks_optimize_expr(&object_expr, YAC_TYPE_VARIANT);
   this_expr_opt   =   this_expr->getEval();
   object_expr_opt = object_expr->getEval();
}

