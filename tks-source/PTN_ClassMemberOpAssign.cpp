/// PTN_ClassMemberOpAssign.cpp
///
/// (c) 2001-2014 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "tks_inc_class.h"
#include "tks_inc_jit.h"

#include "PTN_ClassMemberOpAssign.h"


PTN_ClassMemberOpAssign::PTN_ClassMemberOpAssign(TKS_CachedObject *_co,  
                                                 TKS_CachedObject *_cc_co,  
                                                 sU8               _aop,  
                                                 PTN_Expr         *_expr 
                                                 )  
{
   var             = _co;
   member          = _cc_co;
   class_id        = TKS_INVALID_SCRIPT_CLASS_ID;
   assign_operator = _aop;
   expr            = _expr; 
   expr_opt        = expr->getEval();
   b_indirect      = 0;
   if(var) 
   {
      if(var->type == YAC_TYPE_OBJECT) 
      {
         if(var->value.object_val) 
         {
            class_id = ((TKS_ScriptClassInstance*)var->value.object_val)->class_decl->class_id; 
         } 
      } 
   } 
}

PTN_ClassMemberOpAssign::~PTN_ClassMemberOpAssign() {
   var    = NULL;
   member = NULL;
   TKS_DELETE_SAFE(expr);
}

sBool PTN_ClassMemberOpAssign::semanticCheck(void) {
   return 
      (TKS_INVALID_SCRIPT_CLASS_ID != class_id) &&
      (NULL != member)                          &&
      (expr ? expr->semanticCheck() : 0) 
      ;
}

void PTN_ClassMemberOpAssign::optimize(void) {
   tks_optimize_expr(&expr, 0); 
   expr_opt = expr->getEval(); 
}

sBool PTN_ClassMemberOpAssign::resolveXRef(void) {
   if(expr) 
   {
      return expr->resolveXRef();
   }
   else
   {
      return 0;
   }
}

static void PTN_ClassMemberOpAssign__eval(PTN_Env *_env, const PTN_Statement *_st) { 
   Dtracest;
   const PTN_ClassMemberOpAssign *st = (const PTN_ClassMemberOpAssign*) _st; 

   TKS_ScriptClassInstance *robj;
   if(st->b_indirect)
   {
      robj = (TKS_ScriptClassInstance *) _env->context->tksvm_indirect_object;
   }
   else if(st->var)
   {
      robj = (TKS_ScriptClassInstance *) Dgetvar(st->var)->value.object_val;
   }
   else
   {
      robj = _env->context->tksvm_class_stack_object;
   }
  
   if(TKS_VALID(robj))
   {
#ifdef DX_SAFEMODE 
      if(YAC_CLID_CLASS == robj->class_ID)
      { 
         if(robj->class_decl->ancestor_table[st->class_id]) // compatible to req. class?
         { 
#endif
            YAC_Value b;
            st->expr_opt(_env, &b, st->expr);
            if(Dhaveexception)
            {
               b.unsetFast();
               Dhandleexception(st->expr);
               return;
            }

            robj->setClassMemberValueByID(/*(void*)_env->context, */(sUI)st->member->value.int_val, &b, st->assign_operator);
            b.unsetFast(); 
#ifdef DX_SAFEMODE
         }
         // else classtype mismatch
      }
#endif 
   }
   else
   {
      Drtthrowinvalidpointer(st, "invalid <this> object", robj);
   } 
}

void PTN_ClassMemberOpAssign::eval(PTN_Env *_env) const { 
   PTN_ClassMemberOpAssign__eval(_env, this); 
} 

Fevalst PTN_ClassMemberOpAssign::getEvalSt(void) const { 
   return PTN_ClassMemberOpAssign__eval; 
} 


#ifdef TKS_JIT 
sBool PTN_ClassMemberOpAssign::forceHybrid(void) { 
   return (assign_operator!=YAC_OP_ASSIGN)|expr->forceHybrid(); 
} 

sU8 PTN_ClassMemberOpAssign::compile(VMCore *_vm) { 
   sU8 r=expr->compile(_vm); 
   sU8 rtti=((member->value.int_val >> TKS_CLASSDECL_RTTISHIFT) & TKS_CLASSDECL_RTTIMASK); 
   sBool nok=0; 
   if(rtti<3) if(r<3) _vm->typecastStack(r, rtti); else nok=1; 
   else nok=(r!=3); 
   if(nok) 
   { 
      Dprintf("\n[---] ClassMemberAssign::compile: error: expression return type mismatch (expected %s, got %s)\n", 
         tks_typenames[rtti], tks_typenames[r]); 
      Dcterror(PTNERR_CANTCOMPILE); 
      return 0xFF; 
   } 

   _vm->pushci(member->value.int_val); 

   if(NULL != var) 
   { 
      _vm->pushci((sSI)var); 

      if(rtti >= YAC_TYPE_OBJECT) 
      { 
         // myclass.myobjmember=object 
         // st:   +0=var 
         //       +1=member 
         //       +2=rvalue 
         _vm->addAPICall2((sUI)&tksvm_classmemberexpr_o); 
         // st:   +0=var 
         //       +1=lvalue 
         //       +2=rvalue 
         Dasmop(VMOP_INCSTP); // cleanup var, obj/expr are now on stack 
         Dasmop(VMOP_SRETI);
         //       +0=lvalue 
         //       +1=rvalue 
         _vm->objassign(0,0); 
         Dasmop(VMOP_INCSTP); // cleanup lvalue 
         Dasmop(VMOP_INCSTP); // cleanup rvalue 

         return next ? next->compileHybridStatement(_vm) : 0; 
      } 
      else 
      { 
         // myclass.myifmember= if 
         _vm->addAPICall3((sUI)tksvm_classmemberassign_if); 
         Dasmop(VMOP_INCSTP); // cleanup var 
         Dasmop(VMOP_INCSTP); // cleanup member 
         Dasmop(VMOP_INCSTP); // cleanup expr return 
         return next?next->compileHybridStatement(_vm):0; 
      } 
   } 
   else 
   { 
      if( rtti>2 ) 
      { 
         // myclass.myobjmember=object 
         //       +0=context
         //       +1=member 
         //       +2=rvalue 
         _vm->pushContext();
         _vm->addAPICall2((sUI)&tksvm_classmemberexpr_stack_o); 
         Dasmop(VMOP_INCSTP); // cleanup context
         Dasmop(VMOP_SRETI);  // overwrite member with return value (new lhs)
         //       +0=lvalue 
         //       +1=rvalue 
         _vm->objassign(0,0); 
         Dasmop(VMOP_INCSTP); // cleanup lvalue 
         Dasmop(VMOP_INCSTP); // cleanup rvalue 

         return next ? next->compileHybridStatement(_vm) : 0; 
      } 
      else 
      { 
         // myclass.myifmember= if 
         _vm->pushContext();
         _vm->addAPICall3((sUI)&tksvm_classmemberassign_stack_if); 
         Dasmop(VMOP_INCSTP); // cleanup context
         Dasmop(VMOP_INCSTP); // cleanup member 
         Dasmop(VMOP_INCSTP); // cleanup expr return (rvalue) 
         return next ? next->compileHybridStatement(_vm) : 0; 
      } 
   } 
}
#endif // TKS_JIT
