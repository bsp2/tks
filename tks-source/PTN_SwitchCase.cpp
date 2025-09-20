/// PTN_SwitchCase.cpp
///
/// (c) 2001-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "PTN_Statement.h"
#include "vmopenum.h"
#include "VMCore.h"
#include "PTN_SwitchCase.h"
#include "PTN_BuiltinFun.h"


PTN_SwitchCase::PTN_SwitchCase(void) {
   // default case
   prev     = NULL;
   next     = NULL;
   expr     = NULL;
   body     = NULL;
   do_break = YAC_FALSE;
   b_beq    = YAC_FALSE;
}

PTN_SwitchCase::PTN_SwitchCase(PTN_Expr *_expr, PTN_Statement *_body, sBool _db) {
   prev     = NULL;
   next     = NULL;
   expr     = _expr;
   expr_opt =  expr->getEval();
   body     = _body;
   do_break = _db;
   b_beq    = YAC_FALSE;
}

PTN_SwitchCase::~PTN_SwitchCase() {
   TKS_DELETE_SAFE(expr);
   TKS_DELETE_SAFE(body);
   TKS_DELETE_SAFE(next);
}

sBool PTN_SwitchCase::semanticCheck(void) {
   return
      (expr ? expr->semanticCheck()    : 0) &&
      (body ? body->semanticCheckAll() : 1) &&
      (next ? next->semanticCheck()    : 1)
      ;
}

void PTN_SwitchCase::optimizeBEQ(void) {
   if(NULL != expr)
   {
      if(PTN_BUILTINFUN == expr->getID())
      {
         PTN_BuiltinFun *fun = (PTN_BuiltinFun*)expr;
         if(fun->isFunAddress())
         {
            // case @(expr):  (after switch(@(expr)))
            b_beq = YAC_TRUE;
            tks_optimize_expr(&fun->expr, 0);

            if(body)
               body->optimizeAll();

            if(next)
               next->optimizeBEQ();

            return;
         }
      }

      // case <expr>:
      tks_optimize_expr(&expr, 0);
      expr_opt = expr->getEval();
   }

   if(body)
      body->optimizeAll();

   if(next)
      next->optimizeBEQ();
}

void PTN_SwitchCase::optimize(void) {
   if(expr)
   {
      tks_optimize_expr(&expr, 0);
      expr_opt = expr->getEval();
   }

   if(body)
      body->optimizeAll();

   if(next)
      next->optimize();
}

void PTN_SwitchCase::subGenCallList(void) {
   if(body)
      body->genCallList();

   if(next)
      next->subGenCallList();
}

sBool PTN_SwitchCase::resolveXRef(void) {
   sBool r = YAC_TRUE;
   if(expr)
   {
      r = r && expr->resolveXRef();
   }
   if(r)
   {
      if(body)
      {
         r = r && body->resolveXRefAll();
      }
      if(r)
      {
         if(next)
         {
            return next->resolveXRef();
         }
         else
         {
            return YAC_TRUE;
         }
      }
   }
   return YAC_FALSE;
}
