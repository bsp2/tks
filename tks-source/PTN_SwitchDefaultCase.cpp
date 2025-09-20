/// PTN_SwitchDefaultCase.cpp
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
#include "PTN_SwitchCase.h"
#include "PTN_SwitchDefaultCase.h"



PTN_SwitchDefaultCase::PTN_SwitchDefaultCase(void) {
   prev     = NULL;
   next     = NULL;
   expr     = NULL;
   body     = NULL;
   do_break = 0;
}

PTN_SwitchDefaultCase::PTN_SwitchDefaultCase(PTN_Expr *_expr, PTN_Statement *_body, sBool _doBreak) {
   prev     = NULL;
   next     = NULL;
   body     = _body;
   do_break = _doBreak;
   expr     = _expr;
   if(expr)
   {
      expr_opt=expr->getEval(); /// xxxx expr is always NULL for default case..
   }
}

sUI PTN_SwitchDefaultCase::getID(void) const {
	return PTN_SWITCHDEFAULTCASE;
}

sBool PTN_SwitchDefaultCase::semanticCheck(void) {
	return
		(body ? body->semanticCheckAll() : 1) &&
		(next ? next->semanticCheck()    : 1)
      ;
}
