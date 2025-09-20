/// PTN_ClassMemberIndirectClassMethodECall.cpp
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "tks_inc_class.h"
#include "tks_inc_jit.h"

#include "YAC_HashTable.h"
#include "PTN_CallableExpr.h"
#include "PTN_ClassMethodECall.h"
#include "PTN_ClassMemberIndirectClassMethodECall.h"


PTN_ClassMemberIndirectClassMethodECall::PTN_ClassMemberIndirectClassMethodECall(TKS_CachedObject     *_member,  
                                                                                 PTN_ClassMethodECall *_indirectecall 
                                                                                 )  
{
   member             = _member;
   indirect_ecall     = _indirectecall;
   indirect_ecall_opt = NULL;
   indirect_ecall->initIndirect();
}

PTN_ClassMemberIndirectClassMethodECall::~PTN_ClassMemberIndirectClassMethodECall() {
   if(indirect_ecall)
   {
      delete indirect_ecall; 
      indirect_ecall = NULL;
   }

   member      = NULL;
}

sBool PTN_ClassMemberIndirectClassMethodECall::semanticCheck(void) {
   return  
      (member != NULL) && 
      (indirect_ecall ? indirect_ecall->semanticCheck() : 0) 
      ;
}

sBool PTN_ClassMemberIndirectClassMethodECall::resolveXRef(void) {
   if(indirect_ecall) 
   {
      return indirect_ecall->resolveXRef(); 
   }
   return 0;
}

void PTN_ClassMemberIndirectClassMethodECall::optimize(void) {
   if(indirect_ecall) 
   {
      indirect_ecall->optimize();
      indirect_ecall_opt = indirect_ecall->getEval();
   }
}

static void PTN_ClassMemberIndirectClassMethodECall__eval(PTN_Env *_env, YAC_Value *_r, const PTN_Expr *_st) {
   const PTN_ClassMemberIndirectClassMethodECall *st = (PTN_ClassMemberIndirectClassMethodECall*)_st;

   // ---- find class member in current class ----
   YAC_Value mv;
   _env->context->tksvm_class_stack_object
      ->getClassMemberValueByID(&mv, (sUI)st->member->value.int_val); 

   if(mv.type >= YAC_TYPE_OBJECT)
   {
      if(TKS_VALID(mv.value.object_val))
      { 
         // xxx do not call this recursively/reentrant with the same context!
         _env->context->tksvm_indirect_object = mv.value.object_val;
         st->indirect_ecall_opt(_env, _r, st->indirect_ecall);
         ////indirect_ecall->evalCallableExpr(_env, _r); 
      }
      else
      {
         Dprintf("[---] invalid member object \"%s\".\n", (char*)st->member->name.chars);
         Drtthrowinvalidpointer(st, "invalid class member object", mv.value.object_val);
      }
   } 
}

Feval PTN_ClassMemberIndirectClassMethodECall::getEval(void) const {
   return PTN_ClassMemberIndirectClassMethodECall__eval;
}

void PTN_ClassMemberIndirectClassMethodECall::evalCallableExpr(PTN_Env *_env, YAC_Value *_r) const {
   PTN_ClassMemberIndirectClassMethodECall__eval(_env, _r, this);
}

void PTN_ClassMemberIndirectClassMethodECall::eval(PTN_Env *_env, YAC_Value *_r) const {
   PTN_ClassMemberIndirectClassMethodECall__eval(_env, _r, this);
}
