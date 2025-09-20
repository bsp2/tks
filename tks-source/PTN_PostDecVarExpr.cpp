/// PTN_PostDecVarExpr.cpp
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
#include "PTN_PostDecVarExpr.h"


static void PTN_PostDecVarExpr__eval(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) { 
   Dtracest;
   const PTN_PostDecVarExpr *st = (const PTN_PostDecVarExpr*)_st; 

   YAC_Value*co=Dgetvar(st->cached_object); 
   
   switch(co->type)
   {
   case YAC_TYPE_INT:
      ret->initInt(co->value.int_val--);
      break;
   case YAC_TYPE_FLOAT:
      ret->initFloat(co->value.float_val);
      co->value.float_val-=1.0f;
      break;
   case YAC_TYPE_STRING:
      ret->initString(co->value.string_val, 0);
      break;
   case YAC_TYPE_OBJECT: 
      if(co->value.object_val) 
      { 
         YAC_Object *no = YAC_CLONE_POOLED(_env->context, co->value.object_val);
         no->yacOperatorAssign(co->value.object_val);
         YAC_Value r; 
         co->value.object_val->yacOperatorI(YAC_OP_SUB, 1, &r);
         r.unsetFast(); 
         ret->initObject(no, 1); 
      } 
      break;
   }
}

void PTN_PostDecVarExpr::eval(PTN_Env *_env, YAC_Value *_r) const {
   PTN_PostDecVarExpr__eval(_env, _r, this); 
} 

Feval PTN_PostDecVarExpr::getEval(void) const {
   return PTN_PostDecVarExpr__eval; 
} 


#ifdef TKS_JIT 
sBool PTN_PostDecVarExpr::forceHybrid(void) { 
   return (cached_object->type >= YAC_TYPE_OBJECT); 
} 

sU8 PTN_PostDecVarExpr::compile(VMCore *_vm) {
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
         Dasmop(VMOP_PUSHLIVDEC);
         _vm->vm_code[_vm->vm_pc++]=_vm->addLocalVarRef(VM_CONAMECHARS);
      }
      else
      {
         Dasmop(VMOP_PUSHIVDEC);
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
         Dasmop(VMOP_PUSHV);
         _vm->vm_code[_vm->vm_pc++]=_vm->addVarRef(varptr, VM_CONAMECHARS);
      }
      _vm->pushcf(1.0f);
      Dasmop(VMOP_SFSUB);
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
      return 2;
   }
}
#endif 
