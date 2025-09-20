/// PTN_EmptyStatement.cpp
///
/// (c) 2001-2008 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Statement.h"
#include "PTN_EmptyStatement.h"


#ifdef TKS_JIT
sBool PTN_EmptyStatement::forceHybrid(void) {
	return 0;
}
#endif

sUI PTN_EmptyStatementNoTerm::getID(void) const {
   return PTN_EMPTY;
}
