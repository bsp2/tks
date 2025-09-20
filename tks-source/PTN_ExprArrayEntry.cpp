/// PTN_ExprArrayEntry.cpp
///
/// (c) 2001-2008 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "PTN_ExprArrayEntry.h"


PTN_ExprArrayEntry::PTN_ExprArrayEntry(PTN_Expr *_expr) {
   expr     = _expr; 
   expr_opt = expr->getEval();
   next     = 0;
}

PTN_ExprArrayEntry::~PTN_ExprArrayEntry() {
   if(expr)
   {
      delete expr; expr=0;
   }
   if(next)
   {
      delete next; next=0;
   }
}


sBool PTN_ExprArrayEntry::semanticCheck(void) {
   return  
      (expr?expr->semanticCheck():0) && 
      (next?next->semanticCheck():1) ;
}

sBool PTN_ExprArrayEntry::resolveXRef(void) {
   if(expr) 
   {
      if(expr->resolveXRef()) 
      {
         return (next?next->resolveXRef():1); 
      } 
   }
   return 0;
}

void PTN_ExprArrayEntry::optimize(void) {
   tks_optimize_expr(&expr, 0); 
   expr_opt=expr->getEval(); 
   if(next) next->optimize();
}

