/// PTN_FunctionBody.cpp
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
#include "PTN_FunctionBody.h"


sUI PTN_FunctionBody::getID(void) const {
	return PTN_FUNCTIONBODY;
}
