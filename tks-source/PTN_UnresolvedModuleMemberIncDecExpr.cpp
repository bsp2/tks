/// PTN_UnresolvedModuleMemberIncDecExpr.cpp
///
/// (c) 2001-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"

#include "tks_inc_compiler.h"
#include "tks_inc_jit.h"

#include "PTN_UnresolvedModuleMemberIncDecExpr.h"


PTN_UnresolvedModuleMemberIncDecExpr::PTN_UnresolvedModuleMemberIncDecExpr(void) {
   dec              = 0;
   post             = 0;
   resolved_var     = NULL;
   nspace_array     = NULL;
   b_default_module = 0;
}

PTN_UnresolvedModuleMemberIncDecExpr::PTN_UnresolvedModuleMemberIncDecExpr(YAC_String *_modulename, sBool _bDefaultModule, YAC_String *_membername, sBool _post, sBool _dec ) {
   b_default_module = _bDefaultModule;
   module_name.yacCopy(_modulename);
   module_member_name.yacCopy(_membername);
   dec          = _dec;
   post         = _post;
   resolved_var = NULL;
   nspace_array = NULL;
}

PTN_UnresolvedModuleMemberIncDecExpr::~PTN_UnresolvedModuleMemberIncDecExpr() {
   resolved_var = NULL;

   if(NULL != nspace_array)
   {
      delete [] nspace_array;
      nspace_array = NULL;
   }
}

sBool PTN_UnresolvedModuleMemberIncDecExpr::resolveXRef(void) {

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

   TKS_CachedScript *resolved_module = Dtkscompiler->findModule(&module_name);
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
      Dprintf("[---] module instance \"%s\" has no member called \"%s\".\n",
              (char*)module_name.chars,
              (char*)module_member_name.chars
              );
      Dcterror(PTNERR_MODULEMEMBERNOTFOUND);
      return 0;
   }
   return 1;
}

static void PTN_UnresolvedModuleMemberIncDecExpr__eval(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {
   (void)_env;

   Dtracest;
   const PTN_UnresolvedModuleMemberIncDecExpr*st = (const PTN_UnresolvedModuleMemberIncDecExpr*)_st;

   if(st->post)
   {
      if(st->dec)
      {
         // ---- myclassinstance.member-- ----
         switch(st->resolved_var->type)
         {
         case YAC_TYPE_INT:
            ret->initInt(st->resolved_var->value.int_val--);
            break;
         case YAC_TYPE_FLOAT:
            ret->initFloat(st->resolved_var->value.float_val);
            st->resolved_var->value.float_val-=1.0f;
            break;
         case YAC_TYPE_STRING:
            ret->initString(st->resolved_var->value.string_val, 0);
            break;
         case YAC_TYPE_OBJECT:
            ret->initObject(st->resolved_var->value.string_val, 0);
            break;
         }
      }
      else
      {
         // ---- myclassinstance.member++ ----
         switch(st->resolved_var->type)
         {
         case YAC_TYPE_INT:
            ret->initInt(st->resolved_var->value.int_val++);
            break;
         case YAC_TYPE_FLOAT:
            ret->initFloat(st->resolved_var->value.float_val);
            st->resolved_var->value.float_val+=1.0f;
            break;
         case YAC_TYPE_STRING:
            ret->initString(st->resolved_var->value.string_val, 0);
            break;
         case YAC_TYPE_OBJECT:
            ret->initObject(st->resolved_var->value.object_val, 0);
            break;
         }
      }
   }
   else
   {
      if(st->dec)
      {
         // ---- --myclassinstance.member ----
         switch(st->resolved_var->type)
         {
         case YAC_TYPE_INT:
            ret->initInt(--st->resolved_var->value.int_val);
            break;
         case YAC_TYPE_FLOAT:
            st->resolved_var->value.float_val-=1.0f;
            ret->initFloat(st->resolved_var->value.float_val);
            break;
         case YAC_TYPE_STRING:
            ret->initString(st->resolved_var->value.string_val, 0);
            break;
         case YAC_TYPE_OBJECT:
            ret->initObject(st->resolved_var->value.object_val, 0);
            break;
         }
      }
      else
      {
         // ---- ++myclassinstance.member ----
         switch(st->resolved_var->type)
         {
         case YAC_TYPE_INT:
            ret->initInt(++st->resolved_var->value.int_val);
            break;
         case YAC_TYPE_FLOAT:
            st->resolved_var->value.float_val+=1.0f;
            ret->initFloat(st->resolved_var->value.float_val);
            break;
         case YAC_TYPE_STRING:
            ret->initString(st->resolved_var->value.string_val, 0);
            break;
         case YAC_TYPE_OBJECT:
            ret->initObject(st->resolved_var->value.string_val, 0);
            break;
         }
      }
   }
}

Feval PTN_UnresolvedModuleMemberIncDecExpr::getEval(void) const {
   return PTN_UnresolvedModuleMemberIncDecExpr__eval;
}

void PTN_UnresolvedModuleMemberIncDecExpr::eval(PTN_Env *_env, YAC_Value *_r) const {
   PTN_UnresolvedModuleMemberIncDecExpr__eval(_env, _r, this);
}
