/// PTN_ArgVar.cpp
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "PTN_Statement.h"
#include "vmopenum.h"
#include "VMCore.h"
#include "PTN_ArgVar.h"


PTN_ArgVar::PTN_ArgVar(TKS_CachedObject *_var) {
   next = NULL;
   var  = _var;
}

PTN_ArgVar::~PTN_ArgVar() {
   if(NULL != next)
   {
      delete next;
      next = NULL;
      var  = NULL;
   }
}
