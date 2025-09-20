/// TKS_ScriptConstraint.cpp
///
/// (c) 2001-2023 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "PTN_Statement.h"

#include "tks_inc_compiler.h"

#include "PTN_ConstraintVarExpr.h"
#include "TKS_ScriptConstraint.h"


TKS_ScriptConstraint::TKS_ScriptConstraint(void) {
   constraint_expr = NULL;
   cval            = NULL;
   b_done          = YAC_FALSE;
}

TKS_ScriptConstraint::~TKS_ScriptConstraint() {
   if(constraint_expr)
   {
      delete constraint_expr;
      constraint_expr = NULL;
   }
}

sBool TKS_ScriptConstraint::checkConstraintExpr(PTN_Env *_env, YAC_Value *_currentValue) {
   cval = _currentValue;
   YAC_Value r;
   constraint_expr->eval(_env, &r);
   sBool b = YAC_FALSE;
   switch(r.type)
   {
   case YAC_TYPE_VOID:
      break;

   case YAC_TYPE_INT:
      b = (0 != r.value.int_val);
      break;

   case YAC_TYPE_FLOAT:
      b = (0 != ((sSI)r.value.float_val));
      break;

   default:
      if(YAC_VALID(r.value.object_val))
      {
         b = (YAC_CLID_OBJECT != r.value.object_val->class_ID); // not null object
      }
      break;
   }

   // ---- xxx raise constraint failed "exception"

   r.unsetFast();
   return b;
}
