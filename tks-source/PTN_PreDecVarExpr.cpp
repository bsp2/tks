/// PTN_PreDecVarExpr.cpp
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
#include "PTN_PreDecVarExpr.h"


static void PTN_PreDecVarExpr__eval(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) { 
   Dtracest;
	const PTN_PreDecVarExpr*st = (const PTN_PreDecVarExpr*)_st; 

	YAC_Value*co = Dgetvar(st->cached_object); 

	switch(co->type)
	{
	case YAC_TYPE_INT:
		ret->initInt(--co->value.int_val);
		break;
	case YAC_TYPE_FLOAT:
		co->value.float_val-=1.0f;
		ret->initFloat(co->value.float_val);
		break;
	case YAC_TYPE_STRING:
      Dprintf("[---] warning: PreDecVarExpr<String> no-op.\n"); 
		ret->initString(co->value.string_val, 0);
		break;
	case YAC_TYPE_OBJECT:
      if(co->value.object_val) 
      { 
         YAC_Value r; 
         co->value.object_val->yacOperatorI(YAC_OP_SUB, 1, &r); 
         r.unsetFast(); 
         ret->initObject(co->value.object_val, 0); 
      } 
      break;
   }
}

void PTN_PreDecVarExpr::eval(PTN_Env *_env, YAC_Value *_r) const { 
   PTN_PreDecVarExpr__eval(_env, _r, this); 
} 

Feval PTN_PreDecVarExpr::getEval(void) const {
   return PTN_PreDecVarExpr__eval; 
} 

#ifdef TKS_JIT 
sBool PTN_PreDecVarExpr::forceHybrid(void) { 
   return cached_object->type>2; 
} 

sU8 PTN_PreDecVarExpr::compile(VMCore *_vm) {
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
         Dasmop(VMOP_PUSHDECLIV);
         _vm->vm_code[_vm->vm_pc++]=_vm->addLocalVarRef(VM_CONAMECHARS);
      }
      else
      {
         Dasmop(VMOP_PUSHDECIV);
         _vm->vm_code[_vm->vm_pc++]=_vm->addVarRef(varptr, VM_CONAMECHARS);
      }
      return 1;
   case 2:
      if(cached_object->flags&TKS_CachedObject::ISLOCAL)
      {
         Dasmop(VMOP_PUSHLV);
         _vm->vm_code[_vm->vm_pc++]=_vm->addLocalVarRef(VM_CONAMECHARS);
      }
      else
      {
         Dasmop(VMOP_PUSHV);
         _vm->vm_code[_vm->vm_pc++]=_vm->addVarRef(varptr, VM_CONAMECHARS);
      }
      _vm->pushcf(1.0f);
      Dasmop(VMOP_SFSUB);
      if(cached_object->flags&TKS_CachedObject::ISLOCAL)
      {
         Dasmop(VMOP_POPLV);
         _vm->vm_code[_vm->vm_pc++]=_vm->addLocalVarRef(VM_CONAMECHARS);
         Dasmop(VMOP_PUSHLV);
         _vm->vm_code[_vm->vm_pc++]=_vm->addLocalVarRef(VM_CONAMECHARS);
      }
      else
      {
         Dasmop(VMOP_POPV);
         _vm->vm_code[_vm->vm_pc++]=_vm->addVarRef(varptr, VM_CONAMECHARS);
         Dasmop(VMOP_PUSHV);
         _vm->vm_code[_vm->vm_pc++]=_vm->addVarRef(varptr, VM_CONAMECHARS);
      }
      return 2;
   }
}
#endif
