/// PTN_ClassMethodDelegateCall.cpp 
///
/// (c) 2005-2013 Bastian Spiegel <bs@tkscript.de> 
///     - distributed under the terms of the GNU general public license (GPL). 
/// 
 
#include "tks.h" 
#include "tks_inc_class.h" 
#include "tks_inc_jit.h" 
 
#include "YAC_HashTable.h" 
#include "PTN_DoubleArgExpr.h" 
#include "PTN_CallableExpr.h" 
#include "TKS_Compiler.h" 
#include "PTN_ClassMethodDelegateCall.h" 
 
 
PTN_ClassMethodDelegateCall::PTN_ClassMethodDelegateCall(TKS_ClassDecl * _parentClass,
                                                         sBool           _bStatic,
                                                         PTN_Expr      * _name_expr,  
                                                         PTN_ArgList   * _args 
                                                         )  
{ 
   parent_class  = _parentClass;
   b_static      = _bStatic;
   name_expr     = _name_expr; 
   name_expr_opt = NULL; 
   arglist       = _args; 
} 
 
PTN_ClassMethodDelegateCall::~PTN_ClassMethodDelegateCall() {
   TKS_DELETE_SAFE(name_expr);
   TKS_DELETE_SAFE(arglist);
} 
 
sBool PTN_ClassMethodDelegateCall::semanticCheck(void) {  
   if(name_expr) 
   { 
      if(name_expr->semanticCheck()) 
      { 
         if(arglist)  
         {  
            return arglist->semanticCheck(); 
         } 
         else 
         { 
            return 1; 
         } 
      } 
   } 
   return 0; 
} 
 
void PTN_ClassMethodDelegateCall::optimize(void) { 
   if(name_expr) 
   { 
      tks_optimize_expr(&name_expr, YAC_TYPE_STRING);  
      name_expr_opt=name_expr->getEval();  
   } 
   if(arglist) 
   { 
      arglist->optimize(); 
   } 
} 
 
sBool PTN_ClassMethodDelegateCall::resolveXRef(void) {  
   if(name_expr) 
   { 
      if(name_expr->resolveXRef()) 
      { 
         if(arglist) 
         { 
            return arglist->resolveXRef(); 
         } 
         else 
         { 
            return 1; 
         } 
          
      } 
   } 
   return 0; 
} 
 
static void PTN_ClassMethodDelegateCall__eval(PTN_Env *_env, YAC_Value *_r, const PTN_Expr *_st) {
   const PTN_ClassMethodDelegateCall *st = (const PTN_ClassMethodDelegateCall*)_st;
    
   TKS_ScriptClassInstance *sci;
   if(st->b_static)
   {
      // Static delegate call
      sci = st->parent_class->class_template;
   }
   else
   {
      sci = (TKS_ScriptClassInstance*) _env->context->tksvm_class_stack_object; 
   }

   // Evaluate delegate name
   YAC_Value vname; 
   st->name_expr_opt(_env, &vname, st->name_expr); 
   if(Dhaveexception)
   {
      vname.unsetFast();
      Dhandleexception(st->name_expr);
      return;
   }

   vname.typecast(YAC_TYPE_STRING); 

   if(sci->evalDelegate(vname.value.string_val, st->arglist, st->src_loc, _env, _r)) 
   { 
      // delegate evaluated 
   } 
   vname.unset(); 
} 

Feval PTN_ClassMethodDelegateCall::getEval(void) const {
   return PTN_ClassMethodDelegateCall__eval;
}

void PTN_ClassMethodDelegateCall::evalCallableExpr(PTN_Env *_env, YAC_Value *_r) const {
   PTN_ClassMethodDelegateCall__eval(_env, _r, this);
}

void PTN_ClassMethodDelegateCall::eval(PTN_Env *_env, YAC_Value *_r) const {
   PTN_ClassMethodDelegateCall__eval(_env, _r, this);
}

