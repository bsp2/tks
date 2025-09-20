/// PTN_Env.cpp
///
/// (c) 2001-2008 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#include "tks.h"
#include "PTN_Node.h"


void PTN_Env::initDefault(void) {
   continue_flag = 1;
   context       = (TKS_Context*)tkscript->yacContextGetDefault();
}
