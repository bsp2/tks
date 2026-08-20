/// PTN_Break.cpp
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Statement.h"
#include "vmopenum.h"
#include "VMCore.h"
#include "PTN_BreakableStatement.h"
#include "PTN_Break.h"


PTN_Break::PTN_Break(void) {
   parent_st = NULL;
}

PTN_Break::~PTN_Break() {
}

void PTN_Break::eval(PTN_Env *_env) const {
   _env->context->break_stack[_env->context->break_stacki] = 1;
   _env->continue_flag = YAC_FALSE;
}

static void PTN_Break__eval(PTN_Env *_env, const PTN_Statement *_st) {
   Dtracest;
   _env->context->break_stack[_env->context->break_stacki] = 1;
   _env->continue_flag = YAC_FALSE;
   (void)_st;
}

Fevalst PTN_Break::getEvalSt(void) const {
   return PTN_Break__eval;
}

#ifdef TKS_JIT
sU8 PTN_Break::compile(VMCore *_vm) {
   _vm->addOpcode(VMOP_BRA);
   parent_st->jitAddBreakPC(&_vm->vm_code[_vm->vm_pc++]); // fixed up later when BreakableStatement has been compiled

   if(NULL != next)
   {
      return next->compileHybridStatement(_vm);
   }
   else
   {
      return 0u;
   }
}

sBool PTN_Break::forceHybrid(void) {
   // (todo) check whether parent is switch or foreach
   return 0;
}
#endif // TKS_JIT
