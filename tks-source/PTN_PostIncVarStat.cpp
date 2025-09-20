/// PTN_PostIncVarStat.cpp
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
#include "PTN_PostIncVarStat.h"


static void PTN_PostIncVarStat__eval(PTN_Env *_env, const PTN_Statement*_st) {  
   Dtracest;
   const PTN_PostIncVarStat *st = (const PTN_PostIncVarStat*)_st; 

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

Fevalst PTN_PostIncVarStat::getEvalSt(void) const { 
   return PTN_PostIncVarStat__eval; 
} 

void PTN_PostIncVarStat::eval(PTN_Env *_env) const { 
   PTN_PostIncVarStat__eval(_env, this); 
} 

#ifdef TKS_JIT 
sBool PTN_PostIncVarStat::forceHybrid(void) { 
   return (cached_object->type >= YAC_TYPE_OBJECT); 
} 

sU8 PTN_PostIncVarStat::compile(VMCore *_vm) {
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
         _vm->vm_code[_vm->vm_pc++]=_vm->addLocalVarRef((char*)cached_object->name.chars);
      }
      else
      {
         Dasmop(VMOP_INCIV);
         _vm->vm_code[_vm->vm_pc++]=_vm->addVarRef(varptr, (char*)cached_object->name.chars);
      }
      break;
   case 2:
      if(cached_object->flags&TKS_CachedObject::ISLOCAL)
      {
         Dasmop(VMOP_PUSHLV);
         _vm->vm_code[_vm->vm_pc++]=_vm->addLocalVarRef((char*)cached_object->name.chars);
      }
      else
      {
         Dasmop(VMOP_PUSHV);
         _vm->vm_code[_vm->vm_pc++]=_vm->addVarRef(varptr, (char*)cached_object->name.chars);
      }
      _vm->pushcf(1.0f);
      Dasmop(VMOP_SFADD);
      if(cached_object->flags&TKS_CachedObject::ISLOCAL)
      {
         Dasmop(VMOP_POPLV);
         _vm->vm_code[_vm->vm_pc++]=_vm->addLocalVarRef((char*)cached_object->name.chars);
      }
      else
      {
         Dasmop(VMOP_POPV);
         _vm->vm_code[_vm->vm_pc++]=_vm->addVarRef(varptr, (char*)cached_object->name.chars);
      }
      break;
   } 
   if(next) 
      return next->compileHybridStatement(_vm); 
   else 
      return 0; 
}
#endif
