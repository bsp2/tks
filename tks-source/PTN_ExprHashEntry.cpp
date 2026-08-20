/// PTN_ExprHashEntry.cpp
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "PTN_ExprHashEntry.h"


PTN_ExprHashEntry::PTN_ExprHashEntry(PTN_Expr *_expr, YAC_String *_name) {
   expr     = _expr;
   expr_opt = expr->getEval();
   next     = NULL;
   name.yacCopy(_name);
}

PTN_ExprHashEntry::~PTN_ExprHashEntry() {
   if(expr) { delete expr; expr = NULL; }
   if(next) { delete next; next = NULL; }
}

sBool PTN_ExprHashEntry::semanticCheck(void) {
   return
      (expr ? expr->semanticCheck() : 0) &&
      (next ? next->semanticCheck() : 1) ;
}

sBool PTN_ExprHashEntry::resolveXRef(void) {
   if(expr)
   {
      if(expr->resolveXRef())
      {
         return next ? next->resolveXRef() : 1;
      }
   }
   return YAC_FALSE;
}

void PTN_ExprHashEntry::optimize(void) {
   tks_optimize_expr(&expr, 0);
   expr_opt = expr->getEval();
   if(next)
      next->optimize();
}
