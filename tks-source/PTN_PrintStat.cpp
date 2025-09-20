/// PTN_PrintStat.cpp
///
/// (c) 2001-2022 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///
///

#include <stdio.h>
#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "PTN_Statement.h"
#include "vmopenum.h"
#include "VMCore.h"
#include "PTN_PrintStat.h"
#include "TKS_Mutex.h"


PTN_PrintStat::PTN_PrintStat(void) {
   target      = 0;
   expr        = NULL;
   dep_dtrace  = 0; 
   add_newline = 0;
}

PTN_PrintStat::PTN_PrintStat(sU8 _d, PTN_Expr *_expr, sBool _depdtrace, sBool _addnewline) {
   target      = _d;
   expr        = _expr; 
   expr_opt    = expr->getEval();
   dep_dtrace  = _depdtrace; 
   add_newline = _addnewline;
}

PTN_PrintStat::~PTN_PrintStat() {
   TKS_DELETE_SAFE(expr);
}

sBool PTN_PrintStat::semanticCheck(void) {
   return 
      (expr ? expr->semanticCheck() : 0)
      ;
}

void PTN_PrintStat::optimize(void) {
   tks_optimize_expr(&expr, 0); 
   expr_opt = expr->getEval(); 
}


sBool PTN_PrintStat::resolveXRef(void) {
   return (expr->resolveXRef());
}

static void PTN_PrintStat__eval(PTN_Env *_env, const PTN_Statement *_st) { 
   Dtracest;
   const PTN_PrintStat *st = (const PTN_PrintStat*)_st; 
   
   const char *e_msg = NULL;

   if(st->dep_dtrace) 
   {
      if(!tkscript->dtrace_enabled) 
      {
         return;  
      } 
   }
   if(!tkscript->configuration.b_enableprint && !st->target)  
   {
      return; 
   }

#ifdef TKS_MT
   tkscript->lockGlobalContext();
#endif // TKS_MT

   YAC_Value r;
   st->expr_opt(_env, &r, st->expr);
   if(Dhaveexception)
   {
#ifdef TKS_MT
      tkscript->unlockGlobalContext();
#endif // TKS_MT

      r.unsetFast();
      Dhandleexception(st->expr);
      return;
   }
   
   switch(r.type)
   {
   case 0:
      // ---- void ----  
      if(st->add_newline) 
      {
         tkscript->print("<void>\n", st->target);  
      } 
      else 
      { 
         tkscript->print("<void>", st->target);  
      }
      e_msg = "<void>";
      break;
   case 1:
      // ---- int ----
      {
         YAC_String t; 
         t.yacValueOfI(r.value.int_val); 
         tkscript->print((char*)t.chars, st->target);
         if(st->add_newline)  
         { 
            tkscript->print("\n", st->target); 
         }
         e_msg = (char*) t.chars;
      }
      break;
   case 2:
      // ---- float ----
      {
         YAC_String t; 
         t.yacValueOfF32(r.value.float_val);
         tkscript->print((char*)t.chars, st->target);
         if(st->add_newline)  
         { 
            tkscript->print("\n", st->target); 
         }
         e_msg = (char*) t.chars;
      }
      break;
   case 3:
      // ---- object ---- 
      if(YAC_VALID(r.value.object_val))
      {
         YAC_String t; 
         if(!r.value.object_val->yacToString(&t)) 
         {
            t.alloc(128);
            t.printf("<Object %s @%p>",
               (char*)r.value.object_val->yacClassName(),  
               (void*)r.value.object_val  
               ); 
         } 
         tkscript->print((char*)t.chars, st->target); 

         if(st->add_newline)  
         { 
            tkscript->print("\n", st->target); 
         }
         e_msg = (char*) t.chars;
      }
      else  
      { 
         if(st->add_newline)
         {
            tkscript->print("<null>\n", st->target); 
         }
         else
         {
            tkscript->print("<null>", st->target); 
         }
         e_msg = "<null>";
      }
      break;
   case 4:
      // ---- string ----
      if(YAC_BCHK(r.value.string_val, YAC_CLID_STRING))
      {
         if(r.value.string_val->length > 1u)
         {
            tkscript->print((char*)r.value.string_val->chars, st->target);
            if(st->add_newline)
            {
               if(!r.value.string_val->endsWith("\n")) 
               {
                  tkscript->print("\n", st->target); 
               }
            }

            e_msg = (char*) r.value.string_val->chars;
         }
         else
         {
            if(st->add_newline)
               tkscript->print("\n", st->target);
            e_msg = "";
         }
      }
      else  
      { 
         if(st->add_newline) 
         { 
            tkscript->print("<null>\n", st->target);  
         } 
         else 
         { 
            tkscript->print("<null>", st->target);  
         } 
         e_msg = "<null>";
      }
      break;
   }

#ifdef TKS_MT
   tkscript->unlockGlobalContext();
#endif // TKS_MT

   if(st->target == 3)
   {
      // die "cu"
      _env->context->raiseException(TKS_EXCEPTION_DEATH, e_msg, NULL, 0, st);
   }
   r.unsetFast();
}

Fevalst PTN_PrintStat::getEvalSt(void) const {
   return PTN_PrintStat__eval; 
} 

void PTN_PrintStat::eval(PTN_Env *_env) const {
   PTN_PrintStat__eval(_env, this); 
}
