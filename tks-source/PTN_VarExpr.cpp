/// PTN_VarExpr.cpp
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
#include "PTN_VarExpr.h"


PTN_VarExpr::PTN_VarExpr(void) {
	cached_object=0;
}

PTN_VarExpr::PTN_VarExpr(TKS_CachedObject *_var) {
	cached_object = _var;
}

PTN_VarExpr::~PTN_VarExpr() {
}

sUI PTN_VarExpr::getID(void) const {
	return PTN_VAREXPR;
}
 
sUI PTN_VarExpr::getType(void) const { 
	return cached_object->type; 
} 

void PTN_VarExpr::eval(PTN_Env *_env, YAC_Value *_r) const { 
	YAC_Value *co=Dgetvar(cached_object); 
 
	_r->initValue(co);
}
 
static void PTN_VarExpr__eval(PTN_Env *_env, YAC_Value *_r, const PTN_Expr *_st) { 
   Dtracest;
	_r->initValue( ((const PTN_VarExpr*)_st)->cached_object ); 
   (void)_env;
} 
 
static void PTN_VarExpr__eval_local(PTN_Env *_env, YAC_Value *_r, const PTN_Expr *_st) { 
   Dtracest;
	_r->initValue(&_env->context->tks_fstackc[((const PTN_VarExpr*)_st)->cached_object->local_index]); 
} 
 
Feval PTN_VarExpr::getEval(void) const { 
	return (cached_object->flags&TKS_CachedObject::ISLOCAL) ? PTN_VarExpr__eval_local : PTN_VarExpr__eval; 
} 
 

sBool PTN_VarExpr::semanticCheck(void) {
	return (NULL != cached_object);
}
 
#ifdef TKS_JIT 
sBool PTN_VarExpr::forceHybrid(void) { 
   return (cached_object->flags & TKS_CachedObject::ISDYNAMIC) ? 1 : 0; 
}
 

sU8 PTN_VarExpr::compile(VMCore *_vm) {
   yacmemptr varptr;
   varptr.s32 = &cached_object->value.int_val;
   switch(cached_object->type)
   {
   default:
      Dcterror(PTNERR_ERRCOMPILE);
      return 0xFF;
   case 1:
      if(cached_object->flags&TKS_CachedObject::ISLOCAL)
      {
         _vm->addOpcode(VMOP_PUSHLV);
         _vm->vm_code[_vm->vm_pc++]=_vm->addLocalVarRef(VM_CONAMECHARS);
      }
      else
      {
         _vm->addOpcode(VMOP_PUSHV);
         _vm->vm_code[_vm->vm_pc++]=_vm->addVarRef(varptr, VM_CONAMECHARS);
      }
      return 1;
   case 2:
      if(cached_object->flags&TKS_CachedObject::ISLOCAL)
      {
         _vm->addOpcode(VMOP_PUSHLV);
         _vm->vm_code[_vm->vm_pc++]=_vm->addLocalVarRef(VM_CONAMECHARS);
      }
      else
      {
         _vm->addOpcode(VMOP_PUSHV);
         _vm->vm_code[_vm->vm_pc++]=_vm->addVarRef(varptr, VM_CONAMECHARS);
      }
      return 2;
   case 3:
   case 4:
      _vm->addOpcode(VMOP_PUSHV);
      _vm->vm_code[_vm->vm_pc++]=_vm->addVarRef(varptr, VM_CONAMECHARS);
      return 3;
   }
}
#endif



