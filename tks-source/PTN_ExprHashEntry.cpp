/// PTN_ExprHashEntry.cpp
///
/// (c) 2001-2008 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "PTN_ExprHashEntry.h"


PTN_ExprHashEntry::PTN_ExprHashEntry(PTN_Expr *_expr, YAC_String *_name) {
   expr     = _expr; 
   expr_opt = expr->getEval();
   next     = 0;
   name.yacCopy(_name);
}

PTN_ExprHashEntry::~PTN_ExprHashEntry() {
   if(expr) { delete expr; expr=0; }
   if(next) { delete next; next=0; }
}

sBool PTN_ExprHashEntry::semanticCheck(void) {
   return  
      (expr?expr->semanticCheck():0) && 
      (next?next->semanticCheck():1) ;
}

sBool PTN_ExprHashEntry::resolveXRef(void) {
   if(expr) 
   {
      if(expr->resolveXRef()) 
      {
         return (next?next->resolveXRef():1); 
      } 
   }
   return 0;
}

void PTN_ExprHashEntry::optimize(void) {
   tks_optimize_expr(&expr, 0); 
   expr_opt=expr->getEval(); 
   if(next) next->optimize();
}


