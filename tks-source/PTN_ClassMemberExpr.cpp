/// PTN_ClassMemberExpr.cpp
///
/// (c) 2001-2014 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "tks_inc_class.h"
#include "tks_inc_jit.h"
#include "PTN_ClassMemberExpr.h"
 
PTN_ClassMemberExpr::PTN_ClassMemberExpr(void) 
{ 
   var        = 0; 
   member     = 0; 
   class_id   = TKS_INVALID_SCRIPT_CLASS_ID; 
   b_deref    = 0;
   b_indirect = 0;
}

PTN_ClassMemberExpr::PTN_ClassMemberExpr(TKS_CachedObject *_co,  
                                         TKS_CachedObject *_cc_co 
                                         )  
{
   var        = _co;
   member     = _cc_co;
   class_id   = TKS_INVALID_SCRIPT_CLASS_ID;
   b_deref    = 0;
   b_indirect = 0;

   if(var) 
   {
      if(var->type==YAC_TYPE_OBJECT) 
      {
         if(var->value.object_val) 
         {
            class_id=((TKS_ScriptClassInstance*)var->value.object_val)->class_decl->class_id; 
         } 
      } 
   }
   
}

PTN_ClassMemberExpr::~PTN_ClassMemberExpr() {
   var    = NULL;
   member = NULL;
}

sBool PTN_ClassMemberExpr::semanticCheck(void) {
   return (member != NULL);
}

static void PTN_ClassMemberExpr__eval(PTN_Env *_env, YAC_Value *_r, const PTN_Expr *_st) { 
   const PTN_ClassMemberExpr *st = (const PTN_ClassMemberExpr*) _st; 
   Dtracest;

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
      if(robj->class_ID == YAC_CLID_CLASS)
      { 
#endif
#ifdef DX_SAFEMODE
         if(robj->class_decl->ancestor_table[st->class_id])
#endif 
         { 
            if(st->b_deref)
            {
               robj->getDerefClassMemberValueByID(_r, (sUI)st->member->value.int_val); 
            }
            else
            {
               robj->getClassMemberValueByID(_r, (sUI)st->member->value.int_val); 
            }
            return;
         } 
#ifdef DX_SAFEMODE
         else
         {
            Drtthrow(st, TKS_EXCEPTION_CLASSTYPEMISMATCH, "cannot access member");
         }
#endif
#ifdef DX_SAFEMODE
      }
      else
      {
         Drtthrow(st, TKS_EXCEPTION_CLASSTYPEMISMATCH, "current object is not a script class");
      }
#endif 
   }
   else
   {
      //Drtthrow(st, TKS_EXCEPTION_INVALIDPOINTER, "invalid <this> object");
      if(!_env->context);else _env->context->raiseRuntimeException(
         st,
         TKS_EXCEPTION_CLASSTYPEMISMATCH,
         "current object is not a script class", 
         __FILE__, 
         __LINE__)
         ;
   } 
   _r->initVoid(); 
}

Feval PTN_ClassMemberExpr::getEval(void) const { 
   return PTN_ClassMemberExpr__eval; 
} 

void PTN_ClassMemberExpr::eval(PTN_Env *_env, YAC_Value *_r) const { 
   PTN_ClassMemberExpr__eval(_env, _r, this); 
} 


#ifdef TKS_JIT 
sUI PTN_ClassMemberExpr::getType(void) const {
   sU8 rtti = ((member->value.int_val >> TKS_CLASSDECL_RTTISHIFT) & TKS_CLASSDECL_RTTIMASK); 
   return rtti;
}

sBool PTN_ClassMemberExpr::forceHybrid(void) { 
   return (1 == b_deref); 
} 

sU8 PTN_ClassMemberExpr::compile(VMCore *_vm) {
   if(var) 
   { 
      sU8 rtti = ((member->value.int_val >> TKS_CLASSDECL_RTTISHIFT) & TKS_CLASSDECL_RTTIMASK); 
      _vm->pushci(member->value.int_val); 
      _vm->pushci((sSI)var); 

      if(rtti >= YAC_TYPE_OBJECT) 
      { 
         _vm->addAPICall2((sUI)&tksvm_classmemberexpr_o); 
         Dasmop(VMOP_INCSTP); 
         Dasmop(VMOP_SRETI); // (note) assumes that YAC_Object* is returned 32bit ptr/int
         return 3; 
      } 
      else 
      { 
         _vm->addAPICall2((sUI)&tksvm_classmemberexpr_if); 
         Dasmop(VMOP_INCSTP); 
         Dasmop(VMOP_SRETI); // (note) int/float values always returns as 32bit int (i.e. in eax/d0/r0)
         return rtti; 
      } 
   } 
   else 
   { 
      sU8 rtti=((member->value.int_val >> TKS_CLASSDECL_RTTISHIFT) & TKS_CLASSDECL_RTTIMASK); 
      _vm->pushci(member->value.int_val); 
      _vm->pushContext();

      if(rtti >= YAC_TYPE_OBJECT) 
      { 
         _vm->addAPICall2((sUI)&tksvm_classmemberexpr_stack_o); 
         Dasmop(VMOP_INCSTP); // cleanup context
         Dasmop(VMOP_SRETI);
         return 3; 
      } 
      else 
      { 
         _vm->addAPICall2((sUI)&tksvm_classmemberexpr_stack_if); 
         Dasmop(VMOP_INCSTP); // cleanup context
         Dasmop(VMOP_SRETI);
         return rtti; 
      } 
   } 
}
#endif // TKS_JIT
