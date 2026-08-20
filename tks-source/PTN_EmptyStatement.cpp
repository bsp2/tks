/// PTN_EmptyStatement.cpp
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Statement.h"
#include "PTN_EmptyStatement.h"


#ifdef TKS_JIT
sBool PTN_EmptyStatement::forceHybrid(void) {
	return YAC_FALSE;
}
#endif // TKS_JIT

sUI PTN_EmptyStatementNoTerm::getID(void) const {
   return PTN_EMPTY;
}
