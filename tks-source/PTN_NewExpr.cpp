/// PTN_NewExpr.cpp
///
/// (c) 2001-2014 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Statement.h"
#include "vmopenum.h"
#include "VMCore.h"
#include "PTN_Expr.h"
#include "PTN_NewExpr.h"


PTN_NewExpr::PTN_NewExpr(void) {
   template_object=0;
}

PTN_NewExpr::~PTN_NewExpr() {
   template_object=0;
}

void PTN_NewExpr::eval(PTN_Env *_env, YAC_Value *_r) const {
#ifdef DX_SAFEMODE
   if(template_object)
   { 
#endif 
      _r->initObject(YAC_CLONE_POOLED(_env->context, template_object), 1);
      if(_r->value.object_val->class_ID==YAC_CLID_STRING) 
      { 
         _r->type = YAC_TYPE_STRING; // fix string type  
      }
#ifdef DX_SAFEMODE
   } 
#endif
} 

static void PTN_NewExpr__eval(PTN_Env *_env, YAC_Value *_r, const PTN_Expr *_st) { 
   Dtracest;
   const PTN_NewExpr*st = (const PTN_NewExpr*)_st; 
   
   _r->initObject(YAC_CLONE_POOLED(_env->context, st->template_object), 1); 
   if(_r->value.object_val->class_ID==YAC_CLID_STRING)	  
   {
      _r->type = YAC_TYPE_STRING; // fix string type  
   }
}

Feval PTN_NewExpr::getEval(void) const {
   return PTN_NewExpr__eval;
}
