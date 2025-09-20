/// PTN_PointerAssign.cpp
///
/// (c) 2001-2019 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///
///


#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "PTN_Statement.h"
#include "vmopenum.h"
#include "VMCore.h"
#include "PTN_PointerAssign.h"


PTN_PointerAssign::PTN_PointerAssign(void) {
   left_co    = NULL;
   right_expr = NULL;
}

PTN_PointerAssign::PTN_PointerAssign(TKS_CachedObject *_lco, PTN_Expr *_rexpr) {
   left_co        = _lco;
   right_expr     = _rexpr; 
   right_expr_opt = right_expr->getEval();
}

PTN_PointerAssign::~PTN_PointerAssign() {
   TKS_DELETE_SAFE(right_expr);
}

sBool PTN_PointerAssign::semanticCheck(void) {
   return  
      (NULL != left_co)                           && 
      (right_expr && right_expr->semanticCheck())
      ;
}

sBool PTN_PointerAssign::resolveXRef(void) {
   return
      (right_expr->resolveXRef())
      ;
}

void PTN_PointerAssign::optimize(void) {
   tks_optimize_expr(&right_expr, 0); 
   right_expr_opt = right_expr->getEval(); 
}

static void PTN_PointerAssign__eval(PTN_Env *_env, const PTN_Statement *_st) { 
   Dtracest;
   const PTN_PointerAssign *st = (const PTN_PointerAssign*)_st; 

   YAC_Value *co = Dgetvar(st->left_co); 
   
   YAC_Value r; 
   st->right_expr_opt(_env, &r, st->right_expr); 
   if(Dhaveexception)
   {
      r.unsetFast();
      Dhandleexception(st->right_expr);
      return;
   }
   
   if( (co->type>=YAC_TYPE_OBJECT) && (r.type>=YAC_TYPE_OBJECT) )
   { 
      if(co->value.object_val != r.value.object_val)
      { 
         co->unsetFast(); 
         co->value.object_val=r.value.object_val; 
         co->deleteme = r.deleteme;
         r.deleteme   = 0;
      } 
      else
      {
         co->deleteme = r.deleteme;
         r.deleteme   = 0;
      }
   }
   else 
   { 
      co->unsetObject(); 
   }
}

Fevalst PTN_PointerAssign::getEvalSt(void) const { 
   return PTN_PointerAssign__eval; 
} 

void PTN_PointerAssign::eval(PTN_Env *_env) const { 
   PTN_PointerAssign__eval(_env, this); 
} 


#ifdef TKS_JIT 
sBool PTN_PointerAssign::forceHybrid(void) { 
   return right_expr->forceHybrid(); 
} 

sU8 PTN_PointerAssign::compile(VMCore *_vm) { 
   _vm->jit.b_allowArrayCache=0; // allow "IntArray ia<=someobject; ia[index]=value;.." 
   
   switch(left_co->type) 
   { 
   case YAC_TYPE_OBJECT: 
   case YAC_TYPE_STRING: 
      { 
         sU8 r=right_expr->compile(_vm); 
         if(r == YAC_TYPE_OBJECT) 
         { 
            _vm->pushci((sSI)left_co); // push var 
            
            if(left_co->flags&TKS_CachedObject::ISLOCAL)  
            {
               _vm->pushContext();
               _vm->addAPICall3((sUI)&tksvm_pointerassign_local);  
               Dasmop(VMOP_INCSTP); // cleanup context
            }
            else  
            {
               _vm->addAPICall2((sUI)&tksvm_pointerassign);  
            }
            Dasmop(VMOP_INCSTP); // discard this 
            Dasmop(VMOP_INCSTP); // discard obj 
            if(next)  
            { 
               return next->compileHybridStatement(_vm);  
            }
            else  
            { 
               return 0;  
            }
         } 
         else if(r == YAC_TYPE_INT) 
         { 
            Dasmop(VMOP_INCSTP); 
            _vm->pushci((sSI)left_co); // push var 
            if(left_co->flags&TKS_CachedObject::ISLOCAL)  
            {
               _vm->pushContext();
               _vm->addAPICall2((sUI)&tksvm_delete_local);  
               Dasmop(VMOP_INCSTP); // cleanup context
            }
            else  
            {
               _vm->addAPICall1((sUI)&tksvm_delete);  
            }
            Dasmop(VMOP_INCSTP); // discard this 
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
      break; 
   } 
   Dprintf("[---] PointerAssign: right expr type is not supported by JIT\n"); 
   Dcterror(PTNERR_CANTCOMPILE);
   return 0xFF;
}
#endif
