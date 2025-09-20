/// PTN_Switch.cpp
///
/// (c) 2001-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///
///
///

#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "PTN_Statement.h"
#include "vmopenum.h"
#include "VMCore.h"
#include "PTN_DoubleArgExpr.h"
#include "PTN_BreakableStatement.h"
#include "PTN_SwitchCase.h"
#include "PTN_SwitchDefaultCase.h"
#include "PTN_Switch.h"
#include "PTN_BuiltinFun.h"


PTN_Switch::PTN_Switch(void) {
   expr         = NULL;
   switch_case  = NULL;
   default_case = NULL;
   b_beq        = YAC_FALSE;
}

PTN_Switch::PTN_Switch(PTN_Expr *_expr) {
   expr         = _expr;
   expr_opt     = expr->getEval();
   switch_case  = NULL;
   default_case = NULL;
   b_beq        = YAC_FALSE;
}

PTN_Switch::~PTN_Switch() {
   TKS_DELETE_SAFE(expr);
   TKS_DELETE_SAFE(switch_case);

   // the default case is deleted above
   default_case = NULL;
}

sUI PTN_Switch::getID(void) const {
	return PTN_SWITCH;
}

sBool PTN_Switch::semanticCheck(void) {
	return
		(expr ? expr->semanticCheck() : 0) &&
		(switch_case ? switch_case->semanticCheck() : 1)
      ;
   // default case is part of switch_case list
}

sBool PTN_Switch::resolveXRef(void) {
   return
      (expr->resolveXRef()) &&
      (switch_case ? switch_case->resolveXRef() : 1)
      ;
   // default case is part of switch_case list
}

sBool PTN_Switch::optimizeBEQ(void) {
   if(PTN_BUILTINFUN == expr->getID())
   {
      PTN_BuiltinFun *fun = (PTN_BuiltinFun*)expr;
      if(fun->isFunAddress())
      {
         // switch(@(expr))
         b_beq = YAC_TRUE;
         tks_optimize_expr(&fun->expr, 0);
         return YAC_TRUE;
      }
   }
   return YAC_FALSE;
}

void PTN_Switch::optimize(void) {
   if(optimizeBEQ())
   {
      if(switch_case)
         switch_case->optimizeBEQ();
   }
   else
   {
      tks_optimize_expr(&expr, 0);
      expr_opt = expr->getEval();

      if(switch_case)
         switch_case->optimize();
   }
   // default case is part of switch_case list
}

void PTN_Switch::subGenCallList(void) {
	if(switch_case)
   {
      switch_case->subGenCallList();
   }
   // default case is part of switch_case list
}

static void PTN_Switch__eval(PTN_Env *_env, const PTN_Statement *_st) {
   Dtracest;
	const PTN_Switch *st = (const PTN_Switch*)_st;

   sBool *bBreak = _env->context->pushBreak(); // Push break flag

   YAC_Value *vCmp = _env->context->pushSwitchValue(); // Push comparison value

   if(st->b_beq)
   {
      // switch(@(expr))
      PTN_BuiltinFun *fun = (PTN_BuiltinFun*)st->expr;
      fun->expr_opt(_env, vCmp, fun->expr);

      if(Dhaveexception)
      {
         vCmp->unset();
         _env->context->popSwitchValue(); // pop comparison value
         _env->context->popBreak();       // pop break flag
         Dhandleexception(fun->expr);
         return;
      }
   }
   else
   {
      // switch(expr)
      st->expr_opt(_env, vCmp, st->expr);

      if(Dhaveexception)
      {
         vCmp->unset();
         _env->context->popSwitchValue(); // pop comparison value
         _env->context->popBreak();       // pop break flag
         Dhandleexception(st->expr);
         return;
      }
   }

   // Evaluate regular switch cases, compare expr. result to vCmp
   PTN_SwitchCase *swc = st->switch_case;
   while( (!*bBreak) && (NULL != swc) && _env->continue_flag)
   {
      if(swc->expr) // is not default case?
      {
         sBool bCmp; // comparison result
         YAC_Value r;

         if(swc->b_beq)
         {
            // case @(<expr>):  (after switch(@(expr)))
            PTN_BuiltinFun *fun = (PTN_BuiltinFun*)swc->expr;
            fun->expr_opt(_env, &r, fun->expr);

            if(Dhaveexception)
            {
               _env->context->popSwitchValue(); // pop comparison value
               _env->context->popBreak();       // pop break flag
               r.unsetFast();
               Dhandleexception(fun->expr);
               return;
            }

            YAC_Value r2; // boolean comparison result
            YAC_Value vc; vc.initValue(vCmp); // EvalBEQ unsets l/r values !
            PTN_DoubleArgExpr::EvalBEQ(r, vc, &r2);  // always returns an int_val (bool)
            bCmp = r2.value.int_val;
         }
         else
         {
            // case <expr>:
            swc->expr_opt(_env, &r, swc->expr);
            if(Dhaveexception)
            {
               _env->context->popSwitchValue(); // pop comparison value
               _env->context->popBreak();       // pop break flag
               r.unsetFast();
               Dhandleexception(swc->expr);
               return;
            }

            YAC_Value r2; // boolean comparison result
            YAC_Value vc; vc.initValue(vCmp); // EvalOp unsets l/r values !
            PTN_DoubleArgExpr::EvalOp(_env, &r2, &vc, &r, YAC_OP_CEQ, swc);
            if(YAC_TYPE_INT == r2.type)
            {
               bCmp = (0 != r2.value.int_val);
            }
            else
            {
               r2.unset();
               bCmp = YAC_FALSE;
            }
         }

         if(bCmp)
         {
            // Switch case expr matches, run SW body
            do
            {
               if(NULL != swc->body)
               {
                  swc->body->evalFirst(_env);
               }

               if(Dhaveexception)
               {
                  _env->context->popSwitchValue(); // pop comparison value
                  _env->context->popBreak();       // pop break flag
                  r.unsetFast();
                  Dhandleexception(swc->body);
                  return;
               }

               if(_env->continue_flag)
               {
                  *bBreak |= swc->do_break;

                  // Run into next switch case (if break; was not used)
                  swc = swc->next;
               }
            }
            while( swc && !*bBreak && _env->continue_flag);
         }
         else
         {
            // Switch case expr does not match, try next
            swc = swc->next;
         }
      }
      else
      {
         // Do not compare default case
         swc = swc->next;
      }
   }

   if(_env->continue_flag)
   {
      if(!*bBreak)
      {
         // All comparisons failed, run default case if exists
         if(st->default_case)
         {
            swc = st->default_case;
            do
            {
               if(NULL != swc->body)
               {
                  swc->body->evalFirst(_env);
               }

               if(Dhaveexception)
               {
                  _env->context->popSwitchValue(); // pop comparison value
                  _env->context->popBreak();       // pop break flag
                  Dhandleexception(swc->body);
                  return;
               }

               *bBreak |= swc->do_break && _env->continue_flag;
               // Run into next switch case
               swc = swc->next;
            }
            while( swc && !*bBreak);
         }
      }

      if(*bBreak)
      {
         _env->continue_flag = 1;
      }
   }
   else if(*bBreak)
   {
      _env->continue_flag = 1; // Reset by "break", do not set to 1 if "return" was used!
   }

   vCmp->unset();

  _env->context->popSwitchValue(); // pop comparison value
  _env->context->popBreak();       // pop break flag
}

Fevalst PTN_Switch::getEvalSt(void) const {
	return PTN_Switch__eval;
}

void PTN_Switch::eval(PTN_Env *_env) const {
	PTN_Switch__eval(_env, this);
}
