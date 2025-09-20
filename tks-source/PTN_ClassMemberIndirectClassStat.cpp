/// PTN_ClassMemberIndirectClassStat.cpp
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "tks_inc_class.h"
#include "tks_inc_jit.h"

#include "YAC_HashTable.h"
#include "PTN_ClassMemberIndirectClassStat.h"


PTN_ClassMemberIndirectClassStat::PTN_ClassMemberIndirectClassStat(TKS_CachedObject *_member, 
                                                                   PTN_Statement    *_indirectstatement 
                                                                   )  
{
   member           = _member;
   indirect_stat    = _indirectstatement;
   _indirectstatement->initIndirect();
}

PTN_ClassMemberIndirectClassStat::~PTN_ClassMemberIndirectClassStat() {
   TKS_DELETE_SAFE(indirect_stat);

   member = NULL;
}

sBool PTN_ClassMemberIndirectClassStat::semanticCheck(void) {
   return  
      (member != NULL)                                        && 
      (indirect_stat ? indirect_stat->semanticCheckAll() : 0) 
      ;
}

sBool PTN_ClassMemberIndirectClassStat::resolveXRef(void) {
   if(indirect_stat)
   {
      return indirect_stat->resolveXRefAll();
   }
   else
   {
      return 0;
   }
}

void PTN_ClassMemberIndirectClassStat::optimize(void) {
   if(indirect_stat)
   {
      indirect_stat->optimizeAll(); 
      indirect_stat_opt = indirect_stat->getEvalSt();
   }
}

static void PTN_ClassMemberIndirectClassStat__eval(PTN_Env *_env, const PTN_Statement *_st) { 
   Dtracest;
   const PTN_ClassMemberIndirectClassStat*st = (const PTN_ClassMemberIndirectClassStat*)_st; 
   
   // ---- find class member in current class ----
   YAC_Value mv;
   _env->context->tksvm_class_stack_object->getClassMemberValueByID(&mv, (sUI)st->member->value.int_val); 
   if(mv.type >= YAC_TYPE_OBJECT)
   {
      if(TKS_VALID(mv.value.object_val))
      { 
         _env->context->tksvm_indirect_object = mv.value.object_val;

         st->indirect_stat_opt(_env, st->indirect_stat);

         if(Dhaveexception)
         {
            Dhandleexception(st->indirect_stat);
            return;
         }
      }
      else
      {
         Drtthrowinvalidpointer(st, "invalid class member object", mv.value.object_val);
      }
   } 
}

void PTN_ClassMemberIndirectClassStat::eval(PTN_Env *_env) const {
   PTN_ClassMemberIndirectClassStat__eval(_env, this);
}

Fevalst PTN_ClassMemberIndirectClassStat::getEvalSt(void) const {
   return PTN_ClassMemberIndirectClassStat__eval;
}
