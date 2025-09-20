/// PTN_ClassMemberIndirectObjectStat.cpp
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "tks_inc_class.h"
#include "tks_inc_jit.h"

#include "PTN_ClassMemberIndirectObjectStat.h"


PTN_ClassMemberIndirectObjectStat::PTN_ClassMemberIndirectObjectStat(TKS_CachedObject *_member,  
                                                                     PTN_Statement    *_indirectstat 
                                                                     )  
{
   member           = _member;
   indirect_stat    = _indirectstat;
   indirect_stat->initIndirect();
}

PTN_ClassMemberIndirectObjectStat::~PTN_ClassMemberIndirectObjectStat() {
   TKS_DELETE_SAFE(indirect_stat);
   member = NULL;
}

sBool PTN_ClassMemberIndirectObjectStat::semanticCheck(void) {
   return
      (member != NULL)                                      && 
      (indirect_stat ? indirect_stat->semanticCheckAll() : 0)  
      ;
}

sBool PTN_ClassMemberIndirectObjectStat::resolveXRef(void) {
   if(indirect_stat)
   {
      return indirect_stat->resolveXRefAll();
   }
   else
   {
      return 0;
   }
}

void PTN_ClassMemberIndirectObjectStat::optimize(void) {
   if(indirect_stat) 
   {
      indirect_stat->optimizeAll(); 
      indirect_stat_opt = indirect_stat->getEvalSt(); 
   } 
}

static void PTN_ClassMemberIndirectObjectStat__eval(PTN_Env *_env, const PTN_Statement *_st) { 
   Dtracest;
   const PTN_ClassMemberIndirectObjectStat *st = (const PTN_ClassMemberIndirectObjectStat*)_st;
   
   YAC_Value mv; 
   
   // ---- find object member in current class ----
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

Fevalst PTN_ClassMemberIndirectObjectStat::getEvalSt(void) const { 
   return PTN_ClassMemberIndirectObjectStat__eval; 
} 

void PTN_ClassMemberIndirectObjectStat::eval(PTN_Env *_env) const { 
   PTN_ClassMemberIndirectObjectStat__eval(_env, this); 
} 
