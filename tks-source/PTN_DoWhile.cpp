/// PTN_DoWhile.cpp
///
/// (c) 2001-2014 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "PTN_Node.h"
#include "PTN_Expr.h"
#include "PTN_Statement.h"
#include "vmopenum.h"
#include "VMCore.h"
 
#include "PTN_Statement.h"
#include "PTN_BreakableStatement.h"
#include "PTN_DoWhile.h"

 
PTN_DoWhile::PTN_DoWhile(void) {
   body       = NULL;
   cond       = NULL;
   cond_opt   = NULL;
   break_tag  = 0;
}

void PTN_DoWhile::init(PTN_Statement *_body,  
                       PTN_Expr *_cond 
                       )  
{
   body     = _body;
   cond     = _cond; 
   cond_opt = cond->getEval();
}

PTN_DoWhile::~PTN_DoWhile() {
   TKS_DELETE_SAFE(body);
   TKS_DELETE_SAFE(cond);
}

sUI PTN_DoWhile::getID(void) const {
   return PTN_DOWHILE;
}

void PTN_DoWhile::useBreak(void) {
   break_tag = 1;
}

sBool PTN_DoWhile::semanticCheck(void) {
   return  
      (body ? body->semanticCheckAll() : 0) &&
      (cond ? cond->semanticCheck(): 0)
      ;
}

void PTN_DoWhile::optimize(void) {
   tks_optimize_expr(&cond, 1); 
   cond_opt = cond->getEval(); 
   if(body)  
   { 
      body->optimizeAll(); 
   }
}

sBool PTN_DoWhile::resolveXRef(void) {
   if(body->resolveXRefAll()) 
   {
      return cond->resolveXRef();
   }
   else
   {
      return 0;
   }
}

void PTN_DoWhile::subGenCallList(void) {
   if(body)
   {
      body->genCallList();
   }
}

static void PTN_DoWhile__eval(PTN_Env *_env, const PTN_Statement *_st) {  
   Dtracest;
   PTN_DoWhile *st=(PTN_DoWhile*)_st; 
   
   YAC_Value r; 
   do  
   {
      r.safeInitInt(0);

      st->body->evalFirst(_env);

      if( (_env->context->b_running) && (_env->continue_flag) )
      {
         st->cond_opt(_env, &r, st->cond); 
         if(Dhaveexception)
         {
            r.unsetFast();
            Dhandleexception(st->cond);
            return;
         }
      }

   } while(_env->context->b_running && !r.isNullOrIF0() && _env->continue_flag); 
   r.unsetFast();
}

static void PTN_DoWhile__eval_break(PTN_Env *_env, const PTN_Statement *_st) {  
   Dtracest;
   const PTN_DoWhile *st = (const PTN_DoWhile*)_st; 
   
   sBool *bBreak = _env->context->pushBreak();
   
   YAC_Value r; 
   do  
   { 
      r.value.any = NULL; 

      st->body->evalFirst(_env); 

      if(_env->context->b_running && _env->continue_flag)
      {
         r.unsetFast();
         st->cond_opt(_env, &r, st->cond); 
         if(Dhaveexception)
         {
            r.unsetFast();
            Dhandleexception(st->cond);
            return;
         }
      }

      
   } while(!*bBreak && _env->context->b_running && !r.isNullOrIF0() && _env->continue_flag);  
   
   r.unsetFast();

   if(*bBreak)  
   {
      _env->continue_flag = 1;  
   }
   _env->context->popBreak();
} 

void PTN_DoWhile::eval(PTN_Env *_env) const { 
   if(break_tag)  
   {
      PTN_DoWhile__eval_break(_env, this);  
   }
   else  
   {
      PTN_DoWhile__eval(_env, this);  
   }
} 

Fevalst PTN_DoWhile::getEvalSt(void) const { 
   return break_tag ? PTN_DoWhile__eval_break : PTN_DoWhile__eval; 
} 

#ifdef TKS_JIT 
sBool PTN_DoWhile::forceHybrid(void) { 
   return cond->forceHybrid(); 
} 

sU8 PTN_DoWhile::compile(VMCore *_vm) {
   sU16 lpc=_vm->vm_pc;
   sU8 r=body->compileHybridStatement(_vm);

   if(r != 0xFF)
   { 
      _vm->resetArrayCache();//????
      r=cond->compile(_vm);

      if(r != 0xFF)
      {
         _vm->typecastStack(r, 1);
         Dasmop(VMOP_SITESTZ);
         sU16 sd_pc=_vm->vm_pc;
         _vm->vm_code[_vm->vm_pc++]=0;
         Dasmop(VMOP_BRA);
         _vm->vm_code[_vm->vm_pc++]=lpc;
         _vm->vm_code[sd_pc]=_vm->vm_pc;

         jitFixBreakPCs(_vm->vm_pc);

          _vm->resetArrayCache();

         if(next) return next->compileHybridStatement(_vm);
         else return 0;
      }
   }
   return 0xFF;
}
#endif
