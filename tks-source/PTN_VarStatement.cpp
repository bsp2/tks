/// PTN_VarStatement.cpp
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
#include "PTN_VarStatement.h"


PTN_VarStatement::PTN_VarStatement(void) {
	cached_object = NULL;
}

PTN_VarStatement::PTN_VarStatement(TKS_CachedObject *_var) {
	cached_object = _var;
}

PTN_VarStatement::~PTN_VarStatement() {
}

sBool PTN_VarStatement::semanticCheck(void) {
	return 
      (NULL != cached_object)
      ;
}
