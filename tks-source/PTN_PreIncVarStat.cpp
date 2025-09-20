/// PTN_PreIncVarStat.cpp
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
#include "PTN_VarStatement.h"
#include "PTN_PreIncVarStat.h"


static void PTN_PreIncVarStat__eval(PTN_Env *_env, const PTN_Statement *_st) { 
   Dtracest;
	const PTN_PreIncVarStat *st = (const PTN_PreIncVarStat*)_st; 

	YAC_Value*co=Dgetvar(st->cached_object); 

	switch(co->type)
	{
	case YAC_TYPE_INT:
		++co->value.int_val;
		break;
	case YAC_TYPE_FLOAT:
		co->value.float_val+=1.0f;
		break;
	case YAC_TYPE_STRING: 
      break;
	case YAC_TYPE_OBJECT:
      if(co->value.object_val) 
      { 
         YAC_Value r; 
         co->value.object_val->yacOperatorI(YAC_OP_ADD, 1, &r); 
         r.unsetFast(); 
      } 
		break;
	}
}

Fevalst PTN_PreIncVarStat::getEvalSt(void) const { 
   return PTN_PreIncVarStat__eval; 
} 

void PTN_PreIncVarStat::eval(PTN_Env *_env) const { 
   PTN_PreIncVarStat__eval(_env, this); 
} 

#ifdef TKS_JIT 
sBool PTN_PreIncVarStat::forceHybrid(void) { 
   return cached_object->type>2; 
} 

sU8 PTN_PreIncVarStat::compile(VMCore *_vm) {
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
         Dasmop(VMOP_INCLIV);
         _vm->vm_code[_vm->vm_pc++]=_vm->addLocalVarRef(VM_CONAMECHARS);
      }
      else
      {
         Dasmop(VMOP_INCIV);
         _vm->vm_code[_vm->vm_pc++]=_vm->addVarRef(varptr, VM_CONAMECHARS);
      }
      break; 

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
      Dasmop(VMOP_SFADD);
      if(cached_object->flags&TKS_CachedObject::ISLOCAL)
      {
         Dasmop(VMOP_POPLV);
         _vm->vm_code[_vm->vm_pc++]=_vm->addLocalVarRef(VM_CONAMECHARS);
      }
      else
      {
         Dasmop(VMOP_POPV);
         _vm->vm_code[_vm->vm_pc++]=_vm->addVarRef(varptr, VM_CONAMECHARS);
      } 
      break;
   }
   if(next) 
      return next->compileHybridStatement(_vm); 
   else 
      return 0; 
}
#endif
