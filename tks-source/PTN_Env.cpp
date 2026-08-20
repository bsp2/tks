/// PTN_Env.cpp
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#include "tks.h"
#include "PTN_Node.h"


void PTN_Env::initDefault(void) {
   continue_flag = YAC_TRUE;
   context       = (TKS_Context*)tkscript->yacContextGetDefault();
}
