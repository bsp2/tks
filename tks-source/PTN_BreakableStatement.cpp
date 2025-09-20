/// PTN_BreakableStatement.cpp
///
/// (c) 2014-2018 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///


#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "PTN_Statement.h"
#ifdef TKS_JIT
#include "vmopenum.h"
#include "VMCore.h"
#endif // TKS_JIT
#include "PTN_BreakableStatement.h"
#include "TKS_Context.h"


PTN_BreakableStatement::PTN_BreakableStatement(void) {
#ifdef TKS_JIT
   vmpc_branch_fixups = NULL;
   num_vmpc_branch_fixups = 0;
#endif // TKS_JIT
}

PTN_BreakableStatement::~PTN_BreakableStatement() {
#ifdef TKS_JIT
   if(NULL != vmpc_branch_fixups)
   {
      delete [] vmpc_branch_fixups;
      vmpc_branch_fixups = NULL;
   }
#endif // TKS_JIT
}

void PTN_BreakableStatement::jitAddBreakPC(sU16 *_d) {
#ifdef TKS_JIT
   if(NULL == vmpc_branch_fixups)
   {
      vmpc_branch_fixups = new sU16*[TKS_MAX_JIT_LOOP_BREAKS];
   }
   else if(num_vmpc_branch_fixups >= TKS_MAX_JIT_LOOP_BREAKS)
   {
      tkscript->printf("[---] maximum number of JIT break statements exceeded (%u)\n", TKS_MAX_JIT_LOOP_BREAKS);
      return;
   }

   vmpc_branch_fixups[num_vmpc_branch_fixups++] = _d;
#else
   (void)_d;
#endif // TKS_JIT
}

void PTN_BreakableStatement::jitFixBreakPCs(sU16 _vmpcLoopEnd) {
#ifdef TKS_JIT
   if(NULL != vmpc_branch_fixups)
   {
      sUI i;
      for(i=0; i<num_vmpc_branch_fixups; i++)
      {
         *(vmpc_branch_fixups[i]) = _vmpcLoopEnd;
      }
   }
#endif // TKS_JIT
}
