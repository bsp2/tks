/// PTN_UnresolvedModuleMemberPointerAssign.cpp
///
/// (c) 2001-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"
#include "tks_inc_compiler.h"
#include "tks_inc_jit.h"
#include "PTN_UnresolvedModuleMemberPointerAssign.h"


PTN_UnresolvedModuleMemberPointerAssign::PTN_UnresolvedModuleMemberPointerAssign(void) {
   resolved_var     = NULL;
   expr             = NULL;
   expr_opt         = NULL;
   nspace_array     = NULL;
   b_default_module = 0;
}

PTN_UnresolvedModuleMemberPointerAssign::PTN_UnresolvedModuleMemberPointerAssign(YAC_String *_module, sBool _bDefaultModule,
                                                                                 YAC_String *_member,
                                                                                 PTN_Expr *_expr
                                                                                 ) {
   b_default_module   = _bDefaultModule;
   module_name.yacCopy(_module);
   module_member_name.yacCopy(_member);
   expr               = _expr;
   expr_opt           = expr->getEval();
   resolved_var       = NULL;
   nspace_array       = NULL;
}

PTN_UnresolvedModuleMemberPointerAssign::~PTN_UnresolvedModuleMemberPointerAssign() {
   TKS_DELETE_SAFE(expr);

   resolved_var = NULL;

   if(NULL != nspace_array)
   {
      delete [] nspace_array;
      nspace_array = NULL;
   }

}

sBool PTN_UnresolvedModuleMemberPointerAssign::semanticCheck(void) {
   return
      (expr ? expr->semanticCheck() : 0)
      ;
}

sBool PTN_UnresolvedModuleMemberPointerAssign::resolveXRef(void) {
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
#ifdef TKS_DCON
      tkscript->printf(TKSERR_TXT_UNKNOWNMEMBER,
                       (char*)module_name.chars
                       );
      tkscript->printf("[---] in module \"%s\" line %i.\n",
                       Dtkscompiler->getModuleNameByIndex(Dsrcloc_module(src_loc)),
                       Dsrcloc_linenr(src_loc)
                       );
#endif
      Dcterror(PTNERR_MODULENOTFOUND);
      return 0;
   }
   resolved_var = resolved_module->findModuleVar(&module_member_name);
   if(!resolved_var)
   {
      /// ---- xxx check whether member is a class instance ----
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

void PTN_UnresolvedModuleMemberPointerAssign::optimize(void) {
   tks_optimize_expr(&expr, 0);
   expr_opt = expr->getEval();
}

static void PTN_UnresolvedModuleMemberPointerAssign__eval(PTN_Env *_env, const PTN_Statement *_st) {
   Dtracest;
   const PTN_UnresolvedModuleMemberPointerAssign*st = (const PTN_UnresolvedModuleMemberPointerAssign*)_st;

   if(st->resolved_var->type >= YAC_TYPE_OBJECT)
   {
      YAC_Value r;
      st->expr_opt(_env, &r, st->expr);
      if(Dhaveexception)
      {
         r.unsetFast();
         Dhandleexception(st->expr);
         return;
      }

      if((st->resolved_var->type >= YAC_TYPE_OBJECT) && (r.type >= YAC_TYPE_OBJECT)) // isobject
      {
         if(st->resolved_var->value.object_val!=r.value.object_val) // a<=a;
            st->resolved_var->copySafe(&r);
      }
      else if(r.type==YAC_TYPE_INT) // int-assignment, unset pointer
      {
         if(r.value.int_val==0)
         {
            st->resolved_var->unset();
         }
         else
         {
            if(r.type==0)
            {
               st->resolved_var->unset();
            }
         }
      }
   }
}

Fevalst PTN_UnresolvedModuleMemberPointerAssign::getEvalSt(void) const {
   return PTN_UnresolvedModuleMemberPointerAssign__eval;
}

void PTN_UnresolvedModuleMemberPointerAssign::eval(PTN_Env *_env) const {
   PTN_UnresolvedModuleMemberPointerAssign__eval(_env, this);
}
