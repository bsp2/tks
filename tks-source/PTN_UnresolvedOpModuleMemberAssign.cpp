/// PTN_UnresolvedOpModuleMemberAssign.cpp
///
/// (c) 2001-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///
///

#include "tks.h"

#include "tks_inc_compiler.h"
#include "tks_inc_jit.h"

#include "PTN_UnresolvedOpModuleMemberAssign.h"
#include "PTN_DoubleArgExpr.h"


PTN_UnresolvedOpModuleMemberAssign::PTN_UnresolvedOpModuleMemberAssign(void) {
   assign_operator  = 0;
   expr             = NULL;
   expr_opt         = NULL;
   resolved_var     = NULL;
   b_default_module = 0;
   nspace_array     = NULL;
}

PTN_UnresolvedOpModuleMemberAssign::PTN_UnresolvedOpModuleMemberAssign(YAC_String *_module, sBool _bDefaultModule,
                                                                       YAC_String *_member, sU8 _assignop, PTN_Expr *_expr) {
   b_default_module = _bDefaultModule;
   module_name.yacCopy(_module);
   module_member_name.yacCopy(_member);
   expr             = _expr;
   expr_opt         = expr->getEval();
   assign_operator  = _assignop;
   resolved_var     = NULL;
   nspace_array     = NULL;
}

PTN_UnresolvedOpModuleMemberAssign::~PTN_UnresolvedOpModuleMemberAssign() {
   TKS_DELETE_SAFE(expr);

   if(NULL != nspace_array)
   {
      delete [] nspace_array;
      nspace_array = NULL;
   }

}

sBool PTN_UnresolvedOpModuleMemberAssign::semanticCheck(void) {
   return
      (expr ? expr->semanticCheck() : 1)
      ;
}

sBool PTN_UnresolvedOpModuleMemberAssign::resolveXRef(void) {

   if(b_default_module)
   {
      // Main.some_var
      resolved_var = Dtkscompiler->findVarNamespaceArray(&module_member_name, nspace_array);
      if(NULL != resolved_var)
      {
         // Ok, found it
         return 1;
      }
   }

   TKS_CachedScript *resolved_module=Dtkscompiler->findModule(&module_name);
   if(!resolved_module)
   {
      Dprintf(TKSERR_TXT_UNKNOWNMEMBER,
              (char*)module_name.chars
              );
      Dprintf("[---] in module \"%s\" line %i.\n",
              Dtkscompiler->getModuleNameByIndex(Dsrcloc_module(src_loc)),
              Dsrcloc_linenr(src_loc)
              );
      Dcterror(PTNERR_MODULENOTFOUND);
      return 0;
   }

   resolved_var = resolved_module->findModuleVar(&module_member_name);
   if(!resolved_var)
   {
      /// xxx check whether member is a class instance
      Dprintf("[---] variable \"%s\" not found in module \"%s\".\n",
              (char*)module_member_name.chars,
              (char*)module_name.chars
              );
      Dprintf("[---] in module \"%s\" line %i.\n",
              Dtkscompiler->getModuleNameByIndex(Dsrcloc_module(src_loc)),
              Dsrcloc_linenr(src_loc)
              );
      Dcterror(PTNERR_MODULEMEMBERNOTFOUND);
      return 0;
   }

   return expr->resolveXRef();
}

void PTN_UnresolvedOpModuleMemberAssign::optimize(void) {
   tks_optimize_expr(&expr, 0);
   expr_opt = expr->getEval();
}

static void PTN_UnresolvedOpModuleMemberAssign__eval(PTN_Env *_env, const PTN_Statement *_st) {
   Dtracest;
   const PTN_UnresolvedOpModuleMemberAssign *st = (const PTN_UnresolvedOpModuleMemberAssign*)_st;

   YAC_Value ra;
   ra.initValue(st->resolved_var);

   YAC_Value rb;
   st->expr_opt(_env, &rb, st->expr);
   if(Dhaveexception)
   {
      rb.unsetFast();
      Dhandleexception(st->expr);
      return;
   }

   YAC_Value r;
   if((st->assign_operator==YAC_OP_ASSIGN)&&(ra.type>=YAC_TYPE_OBJECT))
   {
      // r= ra = rb
      PTN_DoubleArgExpr::EvalOp(_env, &r, &ra, &rb, YAC_OP_ASSIGN, _st);
      rb.unsetFast();
      return; /// avoid double object/string assignment, object directly manipulated
   }
   else
   {
      // r= ra + rb
      PTN_DoubleArgExpr::EvalOp(_env, &r, &ra, &rb, st->assign_operator, _st);
      // r might contain new object, previous r was unset by assignment above
   }
   rb.unsetFast();

   st->resolved_var->assignValue(&r);
   r.unsetFast();
}

Fevalst PTN_UnresolvedOpModuleMemberAssign::getEvalSt(void) const {
   return PTN_UnresolvedOpModuleMemberAssign__eval;
}

void PTN_UnresolvedOpModuleMemberAssign::eval(PTN_Env *_env) const {
	PTN_UnresolvedOpModuleMemberAssign__eval(_env, this);
}
