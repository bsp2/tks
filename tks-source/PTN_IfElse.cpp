/// PTN_IfElse.cpp
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
#include "PTN_IfElse.h"


PTN_IfElse::PTN_IfElse(void) {
   cond      = NULL;
   cond_opt  = NULL;
   body      = NULL;
   else_body = NULL;
}

PTN_IfElse::PTN_IfElse(PTN_Expr *_cond, PTN_Statement *_body, PTN_Statement *_else_body) {
   cond      = _cond; 
   cond_opt  = cond->getEval(); // xxx
   body      = _body;
   else_body = _else_body;
}

PTN_IfElse::~PTN_IfElse() {
   TKS_DELETE_SAFE(cond);

   TKS_DELETE_SAFE(body);

   TKS_DELETE_SAFE(else_body);
}

sUI PTN_IfElse::getID(void) const {
   return PTN_IFELSE;
}

sBool PTN_IfElse::semanticCheck(void) {
   return 
      (cond ? cond->semanticCheck() : 0)    &&
      (body ? body->semanticCheckAll() : 0) &&
      (else_body ? else_body->semanticCheckAll() : 1)
      ;
}

void PTN_IfElse::optimize(void) {
   tks_optimize_expr(&cond, 1); 
   cond_opt = cond->getEval(); 
   if(body)  
   { 
      body->optimizeAll();
   }
   if(else_body)  
   { 
      else_body->optimizeAll();
   }
}

sBool PTN_IfElse::resolveXRef(void) {
   return
      (cond->resolveXRef())    &&
      (body->resolveXRefAll()) &&
      (else_body ? else_body->resolveXRefAll() : 1)
      ;
}

void PTN_IfElse::subGenCallList(void) {
   if(body)  
   { 
      body->genCallList(); 
   }
   if(else_body)  
   { 
      else_body->genCallList(); 
   }
}

static void PTN_IfElse__eval(PTN_Env *_env, const PTN_Statement *_st) { 
   Dtracest;
   const PTN_IfElse *st = (const PTN_IfElse*)_st; 
   
   YAC_Value r; 
   r.value.any = NULL;
   st->cond_opt(_env, &r, st->cond); 
   if(Dhaveexception)
   {
      r.unsetFast();
      Dhandleexception(st->cond);
      return;
   }

   if(!r.isNullOrIF0())   
   {
      st->body->evalFirst(_env); 
   }
   else if(st->else_body)   
   {
      st->else_body->evalFirst(_env); 
   }
   r.unsetFast();
} 

Fevalst PTN_IfElse::getEvalSt(void) const { 
   return PTN_IfElse__eval; 
} 

void PTN_IfElse::eval(PTN_Env *_env) const { 
   PTN_IfElse__eval(_env, this); 
} 

#ifdef TKS_JIT
sBool PTN_IfElse::forceHybrid(void) {
   return cond->forceHybrid();
}

sU8 PTN_IfElse::compile(VMCore *_vm) {
   sU8 r=cond->compile(_vm);
   if(r!=0xFF)
   {
      _vm->typecastStack(r, 1);
      Dasmop(VMOP_SITESTZ);
      sU16 spc=_vm->vm_pc;
      _vm->vm_code[_vm->vm_pc++]=0;
      r=body->compileHybridStatement(_vm);
      sU16 xpc=0;
      if(r!=0xFF)
      {
         sU16 epc;
         if(else_body)
         {
            Dasmop(VMOP_BRA);
            xpc=_vm->vm_pc;
            _vm->vm_code[_vm->vm_pc++]=0;
            epc=_vm->vm_pc;
            r=else_body->compileHybridStatement(_vm);
            if(r==0xFF)  
            { 
               return 0xFF; 
            }
         }
         else 
         {
            epc=_vm->vm_pc; 
         }
         _vm->vm_code[spc]=epc;
         if(xpc)	 
         { 
            _vm->vm_code[xpc]=_vm->vm_pc; 
         }

          _vm->resetArrayCache();

         if(next)  
         { 
            return next->compileHybridStatement(_vm); 
         }
         else  
         { 
            return 0; 
         }
      }
   }
   return 0xFF;
}
#endif
