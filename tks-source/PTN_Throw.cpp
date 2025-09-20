/// PTN_Throw.cpp
///
/// (c) 2006-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///


#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "PTN_Statement.h"

#include "PTN_Throw.h" 


PTN_Throw::PTN_Throw(void) {
   exception_type = NULL; 
   msg_expr       = NULL;
   msg_expr_opt   = NULL;
}

PTN_Throw::~PTN_Throw() {
   TKS_DELETE_SAFE(msg_expr);
}

sBool PTN_Throw::semanticCheck(void) {
   return
      (msg_expr ? msg_expr->semanticCheck() : 1)
      ;
}

sBool PTN_Throw::resolveXRef(void) {
   return
      (msg_expr ? msg_expr->resolveXRef() : 1)
      ;
}

void PTN_Throw::optimize(void) {
   if(msg_expr)
   { 
      tks_optimize_expr(&msg_expr, YAC_TYPE_STRING); 
      msg_expr_opt = msg_expr->getEval(); 
   }
}

static void PTN_Throw__eval(PTN_Env *_env, const PTN_Statement *_st) {  
   Dtracest;
   const PTN_Throw *st = (const PTN_Throw*)_st;

   if(st->exception_type != NULL)
   {
      YAC_String *msg = NULL;

      if(st->msg_expr != NULL)
      {
         YAC_Value v;
         st->msg_expr_opt(_env, &v, st->msg_expr);
         if(Dhaveexception)
         {
            v.unsetFast();
            Dhandleexception(st->msg_expr);
            return;
         }
   
         // Cast value to string if it aint void or <null>
         if(v.type != YAC_TYPE_VOID)
         {
            if(v.type >= YAC_TYPE_OBJECT)
            {
               if(YAC_VALID(v.value.object_val))
               {
                  v.typecast(YAC_TYPE_STRING);
                  msg = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
                  msg->yacOperatorAssign(v.value.string_val);
               }
            }  
            else
            {
               v.typecast(YAC_TYPE_STRING);
               msg = (YAC_String*) YAC_NEW_CORE_POOLED(YAC_CLID_STRING);
               msg->yacOperatorAssign(v.value.string_val);
            }
         }

         v.unsetFast();
      
      }
      _env->context->raiseExceptionByTypeAndMsg(_st, st->exception_type, msg);
   }
   else
   {
      // Re-throw current exception
      _env->context->reRaiseException();
   }
}
 
Fevalst PTN_Throw::getEvalSt(void) const {
   return PTN_Throw__eval; 
} 

void PTN_Throw::eval(PTN_Env *_env) const {
   PTN_Throw__eval(_env, this);  
} 

