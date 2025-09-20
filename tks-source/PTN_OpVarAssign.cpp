/// PTN_OpVarAssign.cpp
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
#include "PTN_OpVarAssign.h"
#include "PTN_DoubleArgExpr.h"


PTN_OpVarAssign::PTN_OpVarAssign(void) {
}

PTN_OpVarAssign::PTN_OpVarAssign(TKS_CachedObject *_var, PTN_Expr *_expr, sU8 _op) {
   var           = _var;
   expr          = _expr; 
   expr_opt      = expr->getEval();
   expr_operator = _op;
}

PTN_OpVarAssign::~PTN_OpVarAssign() {
   var = NULL;
   TKS_DELETE_SAFE(expr);
}

sBool PTN_OpVarAssign::semanticCheck(void) {
   return 
      (0 != expr_operator)               &&
      (var)                              &&
      (expr ? expr->semanticCheck() : 0)
      ;
}

sBool PTN_OpVarAssign::resolveXRef(void) {
   return expr->resolveXRef();
}

void PTN_OpVarAssign::optimize(void) {
   tks_optimize_expr(&expr, 0); 
   expr_opt = expr->getEval(); 
}


static void PTN_OpVarAssign__eval(PTN_Env *_env, const PTN_Statement *_st) { 
   Dtracest;
   const PTN_OpVarAssign *st = (const PTN_OpVarAssign*)_st; 

   YAC_Value *co = Dgetvar(st->var); 
   
   YAC_Value re;
   st->expr_opt(_env, &re, st->expr);
   if(Dhaveexception)
   {
      re.unsetFast();
      Dhandleexception(st->expr);
      return;
   }
   
   YAC_Value vr;
   vr.deleteme  = 0; 
   vr.type      = co->type;
   vr.value.any = co->value.any;
   
   YAC_Value r;
   PTN_DoubleArgExpr::EvalOp(_env, &r, &vr, &re, st->expr_operator, _st);
   re.unsetFast();
   co->assignValue(&r);
   r.unsetFast(); // xxx?
}

Fevalst PTN_OpVarAssign::getEvalSt(void) const { 
   return PTN_OpVarAssign__eval; 
} 

void PTN_OpVarAssign::eval(PTN_Env *_env) const { 
   PTN_OpVarAssign__eval(_env, this); 
} 


#ifdef TKS_JIT 
sBool PTN_OpVarAssign::forceHybrid(void) { 
   return expr->forceHybrid()|(var->type>2); 
} 

sU8 PTN_OpVarAssign::compile(VMCore *_vm) {
   yacmemptr varptr;
   varptr.s32 = &var->value.int_val;
   sU8 r;
   switch(var->type)
   {
   default:
      Dcterror(PTNERR_ERRCOMPILE);
      return 0xFF;
   case 1:
      // ---- intvar OP = expr
      if(var->flags&TKS_CachedObject::ISLOCAL)
      {
         Dasmop(VMOP_PUSHLV);
         _vm->vm_code[_vm->vm_pc++]=_vm->addLocalVarRef(VM_VARNAMECHARS);
      }
      else
      {
         Dasmop(VMOP_PUSHV);
         _vm->vm_code[_vm->vm_pc++]=_vm->addVarRef(varptr, VM_VARNAMECHARS);
      }
      r=PTN_DoubleArgExpr::CompileOp(_vm, 1, expr->compile(_vm), expr_operator);
      break;
   case 2:
      // ---- floatvar OP = expr
      if(var->flags&TKS_CachedObject::ISLOCAL)
      {
         Dasmop(VMOP_PUSHLV);
         _vm->vm_code[_vm->vm_pc++]=_vm->addLocalVarRef(VM_VARNAMECHARS);
      }
      else
      {
         Dasmop(VMOP_PUSHV);
         _vm->vm_code[_vm->vm_pc++]=_vm->addVarRef(varptr, VM_VARNAMECHARS);
      }
      r=PTN_DoubleArgExpr::CompileOp(_vm, 2, expr->compile(_vm), expr_operator);
      break;
   case 3:
   case 4: 
      return 0xFF;
   }
   switch(r)
   {
   case 0xFF:
      //Dcterror(PTNERR_ERRCOMPILE);
      return 0xFF;
   case 1:
      switch(var->type)
      {
      case 1:
         // ---- intvar=int
         if(var->flags&TKS_CachedObject::ISLOCAL)
         {
            Dasmop(VMOP_POPLV);
            _vm->vm_code[_vm->vm_pc++]=_vm->addLocalVarRef(VM_VARNAMECHARS);
         }	
         else
         {
            Dasmop(VMOP_POPV);
            _vm->vm_code[_vm->vm_pc++]=_vm->addVarRef(varptr, VM_VARNAMECHARS);
         }
         return (next?next->compileHybridStatement(_vm):0);
      case 2:
         // ---- floatvar=int
         Dasmop(VMOP_STCIF);
         if(var->flags&TKS_CachedObject::ISLOCAL)
         {
            Dasmop(VMOP_POPLV);
            _vm->vm_code[_vm->vm_pc++]=_vm->addLocalVarRef(VM_VARNAMECHARS);
         }
         else
         {
            Dasmop(VMOP_POPV);
            _vm->vm_code[_vm->vm_pc++]=_vm->addVarRef(varptr, VM_VARNAMECHARS);
         }
         return (next?next->compileHybridStatement(_vm):0);
      }
      break;
      case 2:
         switch(var->type)
         {
         case 1:
            // ---- intvar=float
            Dasmop(VMOP_STCFI);
            if(var->flags&TKS_CachedObject::ISLOCAL)
            {
               Dasmop(VMOP_POPLV);
               _vm->vm_code[_vm->vm_pc++]=_vm->addLocalVarRef(VM_VARNAMECHARS);
            }
            else
            {
               Dasmop(VMOP_POPV);
               _vm->vm_code[_vm->vm_pc++]=_vm->addVarRef(varptr, VM_VARNAMECHARS);
            }
            return (next?next->compileHybridStatement(_vm):0);
         case 2:
            // ---- floatvar=float
            if(var->flags&TKS_CachedObject::ISLOCAL)
            {
               Dasmop(VMOP_POPLV);
               _vm->vm_code[_vm->vm_pc++]=_vm->addLocalVarRef(VM_VARNAMECHARS);
            }
            else
            {
               Dasmop(VMOP_POPV);
               _vm->vm_code[_vm->vm_pc++]=_vm->addVarRef(varptr, VM_VARNAMECHARS);
            }
            return (next?next->compileHybridStatement(_vm):0);
         }
         break; 
   } // switch r
   return 0xFF;
   
}
#endif 
