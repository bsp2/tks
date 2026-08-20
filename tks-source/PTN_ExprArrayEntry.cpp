/// PTN_ExprArrayEntry.cpp
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///
#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "PTN_ExprArrayEntry.h"


PTN_ExprArrayEntry::PTN_ExprArrayEntry(PTN_Expr *_expr) {
   expr     = _expr;
   expr_opt = expr->getEval();
   next     = NULL;
}

PTN_ExprArrayEntry::~PTN_ExprArrayEntry() {
   if(expr)
   {
      delete expr; expr = NULL;
   }
   if(next)
   {
      delete next; next = NULL;
   }
}


sBool PTN_ExprArrayEntry::semanticCheck(void) {
   return
      (expr ? expr->semanticCheck() : 0) &&
      (next ? next->semanticCheck() : 1) ;
}

sBool PTN_ExprArrayEntry::resolveXRef(void) {
   if(expr)
   {
      if(expr->resolveXRef())
      {
         return next ? next->resolveXRef() : 1;
      }
   }
   return YAC_FALSE;
}

void PTN_ExprArrayEntry::optimize(void) {
   tks_optimize_expr(&expr, 0);
   expr_opt = expr->getEval();
   if(next)
      next->optimize();
}
