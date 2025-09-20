/// PTN_VarAssign.cpp
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
#include "PTN_VarAssign.h"


PTN_VarAssign::PTN_VarAssign(TKS_CachedObject *_var, PTN_Expr *_expr) {
	var      = _var;
	expr     = _expr; 
	expr_opt = expr->getEval();
}

PTN_VarAssign::PTN_VarAssign(void) { }

PTN_VarAssign::~PTN_VarAssign() {
	var = NULL;

   TKS_DELETE_SAFE(expr);
}

sBool PTN_VarAssign::semanticCheck(void) {
	return 
      (var)                              &&
      (expr ? expr->semanticCheck() : 0)
      ;
}

sBool PTN_VarAssign::resolveXRef(void) {
   return expr->resolveXRef();
}

void PTN_VarAssign::optimize(void) {
   tks_optimize_expr(&expr, 0); 
   expr_opt = expr->getEval(); 
}

static void PTN_VarAssign__eval(PTN_Env *_env, const PTN_Statement*_st) { 
   Dtracest;
   const PTN_VarAssign*st = (const PTN_VarAssign*)_st; 

   YAC_Value r;
   st->expr_opt(_env, &r, st->expr); 
   if(Dhaveexception)
   {
      r.unsetFast();
      Dhandleexception(st->expr);
      return;
   }

   Dgetvar(st->var)->assignValue(&r); 
   r.unsetFast();
} 

Fevalst PTN_VarAssign::getEvalSt(void) const { 
   return PTN_VarAssign__eval; 
} 

void PTN_VarAssign::eval(PTN_Env *_env) const { 
   PTN_VarAssign__eval(_env, this); 
} 

#ifdef TKS_JIT 
sBool PTN_VarAssign::forceHybrid(void) { 
   return expr?expr->forceHybrid():0; 
} 

sU8 PTN_VarAssign::compile(VMCore *_vm) {
   if(!expr) 
   { 
      if(next) 
         return next->compileHybridStatement(_vm); 
      else 
         return 0; 
   } 
   if(expr->forceHybrid()) 
   { 
      _vm->addHybridStatement(this); 
      if(next) 
         return next->compileHybridStatement(_vm); 
      else 
         return 0; 
   } 
   yacmemptr varptr;
   varptr.s32 = &var->value.int_val;

   sU8 r=expr->compile(_vm);
   switch(r)
   {
   default:
      //Dcterror(PTNERR_ERRCOMPILE);
      return 0xFF;
   case 1:
      switch(var->type)
      {
      default:
         Dcterror(PTNERR_ERRCOMPILE);
         return 0xFF;
      case 1:
         if(var->flags&TKS_CachedObject::ISLOCAL)
         {
            // local intvar=int
            Dasmop(VMOP_POPLV);
            _vm->vm_code[_vm->vm_pc++]=_vm->addLocalVarRef(VM_VARNAMECHARS);
         }
         else
         {
            // intvar=int
            _vm->addOpcode(VMOP_POPV);
            _vm->vm_code[_vm->vm_pc++]=_vm->addVarRef(varptr, VM_VARNAMECHARS);
         }
         break;
      case 2:
         Dasmop(VMOP_STCIF);
         if(var->flags&TKS_CachedObject::ISLOCAL)
         {
            // local floatvar=int
            Dasmop(VMOP_POPLV);
            _vm->vm_code[_vm->vm_pc++]=_vm->addLocalVarRef(VM_VARNAMECHARS);
         }
         else
         {
            // floatvar=int
            Dasmop(VMOP_POPV);
            _vm->vm_code[_vm->vm_pc++]=_vm->addVarRef(varptr, VM_VARNAMECHARS);
         }
         break;
      case 3:
      case 4: 
         Dcterror(PTNERR_ERRCOMPILE); 
         return 0xFF;
      }
      break;
   case 2:
      switch(var->type)
      {
      default:
         Dcterror(PTNERR_ERRCOMPILE);
         return 0xFF;
      case 1:
         Dasmop(VMOP_STCFI);   // stcfi
         if(var->flags&TKS_CachedObject::ISLOCAL)
         {
            // local intvar=float
            Dasmop(VMOP_POPLV);
            _vm->vm_code[_vm->vm_pc++]=_vm->addLocalVarRef(VM_VARNAMECHARS);
         }
         else
         {
            // intvar=float
            Dasmop(VMOP_POPV);   // popvi var
            _vm->vm_code[_vm->vm_pc++]=_vm->addVarRef(varptr, VM_VARNAMECHARS); 
         }
         break;
      case 2:
         if(var->flags&TKS_CachedObject::ISLOCAL)
         {
            // local intvar=float
            Dasmop(VMOP_POPLV);
            _vm->vm_code[_vm->vm_pc++]=_vm->addLocalVarRef(VM_VARNAMECHARS);
         }
         else
         {
            // floatvar=float
            Dasmop(VMOP_POPV);
            _vm->vm_code[_vm->vm_pc++]=_vm->addVarRef(varptr, VM_VARNAMECHARS);
         }
         break;
      case 3:
      case 4:
         Dcterror(PTNERR_ERRCOMPILE);
         return 0xFF;
      }
      break;
   case 3:
      switch(var->type)
      {
      default: 
      case 0: 
      case 1: 
      case 2: 
         Dcterror(PTNERR_ERRCOMPILE);
         return 0xFF;
      case 3:
      case 4: 
         _vm->pushv(var); 
         _vm->objassign(0,0); 
         Dasmop(VMOP_INCSTP); // discard this 
         Dasmop(VMOP_INCSTP); // discard obj 
         break;
      }
      break;
   case 4:
      Dcterror(PTNERR_ERRCOMPILE);
      return 0xFF;
   }

   if(next) return next->compileHybridStatement(_vm);
   else     return 0;
}

#endif 
