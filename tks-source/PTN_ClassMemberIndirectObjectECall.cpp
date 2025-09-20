/// PTN_ClassMemberIndirectObjectECall.cpp
///
/// (c) 2001-2013 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "tks_inc_class.h"
#include "tks_inc_jit.h"

#include "PTN_CallableExpr.h"
#include "PTN_ClassMemberIndirectObjectECall.h"


PTN_ClassMemberIndirectObjectECall::PTN_ClassMemberIndirectObjectECall(TKS_CachedObject *_member,  
                                                                       PTN_CallableExpr *_indirectecall 
                                                                       )  
{
   member             = _member;
   indirect_ecall     = _indirectecall;
   indirect_ecall_opt = NULL;
   indirect_ecall->initIndirect();
}

PTN_ClassMemberIndirectObjectECall::~PTN_ClassMemberIndirectObjectECall() {
   if(indirect_ecall)
   {
      delete indirect_ecall; 
      indirect_ecall = NULL;
   }

   member      = NULL;
}

sBool PTN_ClassMemberIndirectObjectECall::semanticCheck(void) {
   return
      (member != NULL) && 
      (indirect_ecall ? indirect_ecall->semanticCheck() : 0) 
      ;
}

void PTN_ClassMemberIndirectObjectECall::optimize(void) {
   if(indirect_ecall) 
   {
      indirect_ecall->optimize();
      indirect_ecall_opt = indirect_ecall->getEval();
   }
}

sBool PTN_ClassMemberIndirectObjectECall::resolveXRef(void) {
   if(indirect_ecall) 
   {
      return indirect_ecall->resolveXRef(); 
   }
   return 0;
}

void PTN_ClassMemberIndirectObjectECall::setStatementHint(void) {
   indirect_ecall->setStatementHint();
}

static void PTN_ClassMemberIndirectObjectECall__evalCallStatement(PTN_Env *_env, const PTN_Statement *_st) {
   //
   // Eval call expression as statement
   //
   const PTN_ClassMemberIndirectObjectECall *st = (const PTN_ClassMemberIndirectObjectECall*) _st;

   // ---- find object member in current class ----
   YAC_Value mv;
   _env->context->tksvm_class_stack_object->getClassMemberValueByID(&mv, (sUI)st->member->value.int_val); 
   if(mv.type >= YAC_TYPE_OBJECT)
   {
      if(TKS_VALID(mv.value.object_val))
      { 
         YAC_Value r;

         _env->context->tksvm_indirect_object = mv.value.object_val;

         // TODO: optimize using Feval funptr
         st->indirect_ecall_opt(_env, &r, st->indirect_ecall); 

         // Discard return value
         r.unsetFast();
      }
      else
      {
         Drtthrowinvalidpointer(st, "invalid class member object", mv.value.object_val);
      }
   } 
}

Fevalst PTN_ClassMemberIndirectObjectECall::getEvalCallSt(void) const {
   //
   // This is only called when this node is used as a statement (i.e. called by PTN_Call)
   //
   return PTN_ClassMemberIndirectObjectECall__evalCallStatement;
}

static void PTN_ClassMemberIndirectObjectECall__eval(PTN_Env *_env, YAC_Value *_r, const PTN_Expr *_st) {
   const PTN_ClassMemberIndirectObjectECall *st = (const PTN_ClassMemberIndirectObjectECall*)_st;

   YAC_Value mv;
   // ---- find object member in current class ----
   _env->context->tksvm_class_stack_object->getClassMemberValueByID(&mv, (sUI)st->member->value.int_val); 
   if(mv.type >= YAC_TYPE_OBJECT)
   {
      if(TKS_VALID(mv.value.object_val))
      { 
         _env->context->tksvm_indirect_object = mv.value.object_val;
         ////indirect_ecall->evalCallableExpr(_env, _r); 
         st->indirect_ecall_opt(_env, _r, st->indirect_ecall);
      }
      else
      {
         Drtthrowinvalidpointer(st, "invalid class member object", mv.value.object_val);
      }
   } 
}

Feval PTN_ClassMemberIndirectObjectECall::getEval(void) const {
   return PTN_ClassMemberIndirectObjectECall__eval;
}

void PTN_ClassMemberIndirectObjectECall::evalCallableExpr(PTN_Env *_env, YAC_Value *_r) const {
   PTN_ClassMemberIndirectObjectECall__eval(_env, _r, this);
}

void PTN_ClassMemberIndirectObjectECall::eval(PTN_Env *_env, YAC_Value *_r) const {
   PTN_ClassMemberIndirectObjectECall__eval(_env, _r, this);
}
