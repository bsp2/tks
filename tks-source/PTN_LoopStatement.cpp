/// PTN_LoopStatement.cpp
///
/// (c) 2001-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "tks_inc_compiler.h"
#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "PTN_ConstVal.h"
#include "PTN_Statement.h"
#include "vmopenum.h"
#include "VMCore.h"
#include "PTN_BreakableStatement.h"
#include "PTN_LoopStatement.h"
#include "TKS_Compiler.h"


PTN_LoopStatement::PTN_LoopStatement(void) {
   expr         = NULL;
   expr_opt     = NULL;
   body         = NULL;
   const_expr   = 0;
   const_intval = 0;
   break_tag    = 0;
}

void PTN_LoopStatement::init(PTN_Expr *_expr, PTN_Statement *_body) {
   expr         = _expr;
   expr_opt     = expr->getEval();
   body         = _body;
   const_expr   = 0;
   const_intval = 0;
}

PTN_LoopStatement::~PTN_LoopStatement() {
   TKS_DELETE_SAFE(expr);
   TKS_DELETE_SAFE(body);
}

sUI PTN_LoopStatement::getID(void) const {
   return PTN_LOOP;
}

void PTN_LoopStatement::useBreak(void) {
   break_tag = 1;
}

sBool PTN_LoopStatement::semanticCheck(void) {
   return
      (expr ? expr->semanticCheck()    : 0) &&
      (body ? body->semanticCheckAll() : 0)
      ;
}

sBool PTN_LoopStatement::resolveXRef(void) {
   return
      expr->resolveXRef() &&
      body->resolveXRefAll()
      ;
}

void PTN_LoopStatement::optimize(void) {
   if(expr)
   {
      const_expr = expr->isConst();
      if(const_expr)
      {
         YAC_Value r; expr->evalConst(&r);
         delete expr;

         PTN_ConstVal *cv;
         PTN_NEW_STATIC_NODE(cv, PTN_ConstVal)();
         cv->const_value = &r;
         expr = cv;
         if(r.type!=1)
         {
            Dprintf("\n[---] warning: loop argument is not integer. In module \"%s\":%d\n",
                    Dtkscompiler->getModuleNameByIndex(Dsrcloc_module(src_loc)),
                    Dsrcloc_linenr(src_loc)
                    );
         }
         const_intval = r.value.int_val;
      }
      else
      {
         expr->optimize();
         expr_opt = expr->getEval();
      }
   }
   if(body)
   {
      body->optimizeAll();
   }
}

void PTN_LoopStatement::subGenCallList(void) {
   if(body) body->genCallList();
}

static void PTN_LoopStatement__eval(PTN_Env *_env, const PTN_Statement *_st) {
   Dtracest;
   const PTN_LoopStatement *st = (const PTN_LoopStatement*)_st;

   if(st->const_expr)
   {
      sSI i = st->const_intval;
      while(_env->context->b_running && (--i >= 0) && _env->continue_flag)
         st->body->evalFirst(_env);
   }
   else
   {
      YAC_Value re;
      st->expr_opt(_env, &re, st->expr);

      if(Dhaveexception)
      {
         re.unsetFast();
         Dhandleexception(st->expr);
         return;
      }

      sSI ival=0;
      if(re.type==YAC_TYPE_FLOAT)
      {
         ival=(sSI)re.value.float_val;
      }
      else
      {
         if(re.type==YAC_TYPE_INT)
         {
            ival=re.value.int_val;
         }
         else
         {
            if(re.type>=YAC_TYPE_OBJECT)
            {
               if(YAC_VALID(re.value.object_val))
               {
                  re.value.object_val->yacScanI(&ival);
                  re.unsetFast();
               }
               // xxx raise InvalidPointer exception?
            }
         }
      }

      while((--ival>=0)&&_env->context->b_running&&_env->continue_flag)
         st->body->evalFirst(_env);
   }
}

static void PTN_LoopStatement__eval_break(PTN_Env *_env, const PTN_Statement *_st) {
   Dtracest;
   const PTN_LoopStatement *st = (const PTN_LoopStatement*)_st;

   sBool *bBreak = _env->context->pushBreak();

   if(st->const_expr)
   {
      sSI i=st->const_intval;
      while(!*bBreak && _env->context->b_running && (--i>=0) && _env->continue_flag)
      {
         st->body->evalFirst(_env);
      }
   }
   else
   {
      YAC_Value re;
      st->expr_opt(_env, &re, st->expr);
      if(Dhaveexception)
      {
         re.unsetFast();
         Dhandleexception(st->expr);
         return;
      }

      sSI ival=0;
      if(re.type==YAC_TYPE_FLOAT)
      {
         ival=(sSI)re.value.float_val;
      }
      else
      {
         if(re.type==YAC_TYPE_INT)
         {
            ival=re.value.int_val;
         }
         else
         {
            if(re.type>=YAC_TYPE_OBJECT)
            {
               if(YAC_VALID(re.value.object_val))
               {
                  re.value.object_val->yacScanI(&ival);
                  re.unsetFast();
               }
               // xxx raise InvalidPointer exception?
            }
         }
      }
      while(!*bBreak && _env->context->b_running &&(-- ival>=0) && _env->continue_flag)
      {
         st->body->evalFirst(_env);
      }
   }

   if(*bBreak)
   {
      _env->continue_flag=1;
   }
   _env->context->popBreak();
}

Fevalst PTN_LoopStatement::getEvalSt(void) const {
   return break_tag ? PTN_LoopStatement__eval_break : PTN_LoopStatement__eval;
}

void PTN_LoopStatement::eval(PTN_Env *_env) const {
   if(break_tag)
   {
      PTN_LoopStatement__eval_break(_env, this);
   }
   else
   {
      PTN_LoopStatement__eval(_env, this);
   }
}

#ifdef TKS_JIT
sBool PTN_LoopStatement::forceHybrid(void) {
   return expr->forceHybrid();
}

sU8 PTN_LoopStatement::compile(VMCore *_vm) {
   // XXX add "break" support
   sU8 r=expr->compile(_vm);
   if(r!=0xFF)
   {
      _vm->resetArrayCache(); // ????
      _vm->typecastStack(r, 1);
      Dasmop(VMOP_SITESTZP);
      sU16 spc=_vm->vm_pc;
      _vm->vm_code[_vm->vm_pc++]=0;
      sU16 lpc=_vm->vm_pc;
      r=body->compileHybridStatement(_vm);
      if(r!=0xFF)
      {
         Dasmop(VMOP_SILOOP);
         _vm->vm_code[_vm->vm_pc++]=lpc;
         _vm->vm_code[spc]=_vm->vm_pc;

         jitFixBreakPCs(_vm->vm_pc);

         Dasmop(VMOP_INCSTP); // pop 4 bytes

          _vm->resetArrayCache();

         if(next)
         {
            return next->compileHybridStatement(_vm);
         }
         else
         {
            return 0;
         }
      }
   }
   return 0xFF;
}
#endif
