/// PTN_For.cpp
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
#include "PTN_BreakableStatement.h"
#include "PTN_For.h"
#include "TKS_Context.h"


PTN_For::PTN_For(void) {
  pre       = NULL;
  cond      = NULL;
  post      = NULL;
  body      = NULL;
  break_tag = 0;
}

/*PTN_For::PTN_For(PTN_Statement *_pre, PTN_Expr *_cond, PTN_Statement *_post, PTN_Statement *_body) {
  break_tag = 0;
  init(_pre, _cond, _post, _body);
}*/

void PTN_For::init(PTN_Statement *_pre, PTN_Expr *_cond, PTN_Statement *_post, PTN_Statement *_body) {
  pre       = _pre;
  cond      = _cond;
  cond_opt  = cond->getEval();
  post      = _post;
  body      = _body;
}

PTN_For::~PTN_For() {
   TKS_DELETE_SAFE(pre);
   TKS_DELETE_SAFE(cond);
   TKS_DELETE_SAFE(post);
   TKS_DELETE_SAFE(body);
}

sUI PTN_For::getID(void) const {
	return PTN_FOR;
}

void PTN_For::useBreak(void) {
   break_tag = 1;
}

sBool PTN_For::semanticCheck(void) {
	return 
		(pre  ? pre->semanticCheckAll()  : 0) &&
		(cond ? cond->semanticCheck()    : 0) &&
		(post ? post->semanticCheckAll() : 0) &&
		(body ? body->semanticCheckAll() : 0)
      ;
}

void PTN_For::optimize(void) {
	if(pre)  
   { 
      pre->optimizeAll(); 
   }
	tks_optimize_expr(&cond, 1); 
	cond_opt = cond->getEval(); 
	if(body)  
   { 
      body->optimizeAll(); 
   }
	if(post)  
   { 
      post->optimizeAll(); 
   }
}

sBool PTN_For::resolveXRef(void) {
   return
      (pre->resolveXRefAll() ) &&
      (cond->resolveXRef()   ) &&
      (post->resolveXRefAll()) &&
      (body->resolveXRefAll())
      ;
}

void PTN_For::subGenCallList(void) {
	if(pre)  
   {  
      pre->genCallList();  
   } 
	if(body)  
   {  
      body->genCallList();  
   }
	if(post)  
   {  
      post->genCallList();  
   }
}
 
static void PTN_For__eval(PTN_Env *_env, const PTN_Statement *_st) { 
   Dtracest;
   const PTN_For*st = (const PTN_For*)_st; 
   
   st->pre->evalFirst(_env);
   YAC_Value r;
   st->cond->eval(_env, &r);
   while(_env->context->b_running && r.value.int_val && _env->continue_flag)  
   {
      st->body->evalFirst(_env);

      if( (_env->context->b_running) && (_env->continue_flag) )
      {
         st->post->evalFirst(_env);
         if( (_env->context->b_running) && (_env->continue_flag) )
         {
            r.unset();
            st->cond_opt(_env, &r, st->cond);
            if(Dhaveexception)
            {
               r.unsetFast();
               Dhandleexception(st->cond);
               return;
            }
         }
      }
   }
   r.unsetFast();
}

static void PTN_For__eval_break(PTN_Env *_env, const PTN_Statement *_st) { 
   Dtracest;
   const PTN_For*st = (const PTN_For*)_st; 

   sBool *bBreak = _env->context->pushBreak();
   
   st->pre->evalFirst(_env);
   YAC_Value r;
   st->cond->eval(_env, &r);
   while(!*bBreak && _env->context->b_running && r.value.int_val && _env->continue_flag)  
   {
      st->body->evalFirst(_env);

      if(_env->context->b_running)
      {
         st->post->evalFirst(_env);
         if(_env->context->b_running)
         {
            r.unset();
            st->cond_opt(_env, &r, st->cond);
            if(Dhaveexception)
            {
               _env->context->popBreak();
               r.unsetFast();
               Dhandleexception(st->cond);
               return;
            }
         }
      }
   }
   r.unsetFast();

   if(*bBreak)
   {
      _env->continue_flag = 1;
   }
   _env->context->popBreak();
}

Fevalst PTN_For::getEvalSt(void) const { 
   return break_tag ? PTN_For__eval_break : PTN_For__eval; 
} 
 
void PTN_For::eval(PTN_Env *_env) const { 
	if(break_tag)
   {
      PTN_For__eval_break(_env, this); 
   }
   else
   {
      PTN_For__eval(_env, this); 
   }
} 

#ifdef TKS_JIT
sBool PTN_For::forceHybrid(void) {
  return cond->forceHybrid();
}

sU8 PTN_For::compile(VMCore *_vm) {
	sU8 r=pre->compileHybridStatement(_vm);
	if(r!=0xFF)
	{ 
      _vm->resetArrayCache(); // ????
		sU16 cpc=_vm->vm_pc;
		r=cond->compile(_vm);
		if(r!=0xFF)
		{
			_vm->typecastStack(r, 1);
			_vm->addOpcode(VMOP_SITESTZ);
			sU16 epc=_vm->vm_pc;
			_vm->vm_code[_vm->vm_pc++]=0;
			r=body->compileHybridStatement(_vm);
			if(r!=0xFF)
			{
				r=post->compileHybridStatement(_vm);
				if(r!=0xFF)
				{
					_vm->addOpcode(VMOP_BRA);
					_vm->vm_code[_vm->vm_pc++]=cpc;
					_vm->vm_code[epc]=_vm->vm_pc; // set sitestz branch dst.

               jitFixBreakPCs(_vm->vm_pc);

               _vm->resetArrayCache();

					if(next) return next->compileHybridStatement(_vm);
					else return 0;
				}
			}
		}
	}
	return 0xFF;
}
#endif
