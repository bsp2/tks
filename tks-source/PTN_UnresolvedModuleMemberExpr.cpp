/// PTN_UnresolvedModuleMemberExpr.cpp
///
/// (c) 2001-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///


#include "tks.h"

#include "tks_inc_compiler.h"
#include "tks_inc_jit.h"
#include "PTN_UnresolvedModuleMemberExpr.h"
#include "tks_inc_class.h"


PTN_UnresolvedModuleMemberExpr::PTN_UnresolvedModuleMemberExpr(void) {
   resolved_var       = NULL;
   resolved_module    = NULL;
   resolved_funobject = NULL;
   nspace_array       = NULL;
   b_default_module   = 0;
}

PTN_UnresolvedModuleMemberExpr::PTN_UnresolvedModuleMemberExpr(YAC_String *_name, sBool _bDefaultModule, YAC_String *_membername) {
   b_default_module   = _bDefaultModule;
   module_name.yacCopy(_name);
   module_member_name.yacCopy(_membername);
   resolved_var       = NULL;
   resolved_module    = NULL;
   resolved_funobject = NULL;
   nspace_array       = NULL;
}

PTN_UnresolvedModuleMemberExpr::~PTN_UnresolvedModuleMemberExpr() {
   resolved_var       = NULL;
   resolved_module    = NULL;
   resolved_funobject = NULL;

   if(NULL != nspace_array)
   {
      delete [] nspace_array;
      nspace_array = NULL;
   }
}

sUI PTN_UnresolvedModuleMemberExpr::getID(void) const {
   return PTN_MODULEMEMBEREXPR;
}


sBool PTN_UnresolvedModuleMemberExpr::semanticCheck(void) {
   return 1;
}

sBool PTN_UnresolvedModuleMemberExpr::isConst(void) {
   return (const_val.type!=0);
}

sBool PTN_UnresolvedModuleMemberExpr::resolveXRef(void) {

   // ---- try constant table ----
    TKS_CachedObject *co;
    co = tkscript->uconst_table.findEntry(&module_member_name);
    if(co)
    {
        YAC_String sv;
        sv.yacCopy(co->value.string_val);
        sv.resubstEscapeSeq();
        sU16 checkconv=sv.checkConversions();
        switch(checkconv)
        {
        case 1:  // ---- int ----
            {
                sSI intval=0;
                sv.yacScanI(&intval);
                const_val.initInt(intval);
            }
            return YAC_TRUE;
        case 2:  // ---- float ----
            {
                sF32 floatval=0;
                sv.yacScanF32(&floatval);
                const_val.initFloat(floatval);
            }
            return YAC_TRUE;
        case 3:  // ---- newline constant ----
            const_val.initInt((int)'\n');
            return YAC_TRUE;
        case 4:  // ---- tab constant ----
            const_val.initInt((int)'\t');
            return YAC_TRUE;
        default: // ---- char constant ----
            if(checkconv>256)
                const_val.initNewString(&sv);
            else
                const_val.initInt((int)checkconv);
            return YAC_TRUE;
        }
    }

    if(b_default_module)
    {
       // Main.some_var
       co = Dtkscompiler->findVarNamespaceArray(&module_member_name, nspace_array);
       if(NULL != co)
       {
          // Ok, found it
          resolved_var = co;
          return YAC_TRUE;
       }
    }

    resolved_module = Dtkscompiler->findModule(&module_name);
    if(NULL == resolved_module)
    {
       // Try MyClass.MyStaticMethod...  are the unresolved* nodes ever used for anything but modules ??
       ////TKS_ClassDecl *cd = Dtkscompiler->findClassDeclNamespace(&module_name, NULL);
       TKS_ClassDecl *cd = Dtkscompiler->findClassDeclNamespaceArray(&module_name, nspace_array);
       // if(module_member_name.compare("StreamCallback"))
       //    tkscript->printf("xxx resolve FunctionObject StreamCallback module_name=\"%s\" => cd=%p\n", module_name.chars, cd);
       if(NULL != cd)
       {
          PTN_Function *f = cd->findFunctionSuper(&module_member_name);
          // if(module_member_name.compare("StreamCallback"))
          //    tkscript->printf("xxx resolve FunctionObject StreamCallback f=%p\n", f);
          if(NULL != f)
          {
             resolved_funobject = f->getOrCreateFunctionObject();
             const_val.initObject(resolved_funobject, 0);
             // if(module_member_name.compare("StreamCallback"))
             //    tkscript->printf("xxx resolve FunctionObject StreamCallback fo=%p f=%p fo->fun=%p\n", resolved_funobject, f, resolved_funobject->fun);
             return YAC_TRUE;
          }
          else
          {
#ifdef TKS_DCON
             tkscript->printf("[---] static class member, function or  constant \"%s::%s::%s\" not found.\n",
                              (char*)cd->nspace->name.chars,
                              (char*)cd->name.chars,
                              (char*)module_member_name.chars
                              );
             tkscript->printf("[---] in module \"%s\" line %i.\n",
                              Dtkscompiler->getModuleNameByIndex(Dsrcloc_module(src_loc)),
                              Dsrcloc_linenr(src_loc)
                              );
#endif
             Dcterror(PTNERR_MODULENOTFOUND);
             return YAC_FALSE;
          }
       }
#ifdef TKS_DCON
       tkscript->printf(TKSERR_TXT_UNKNOWNMEMBER,
                        (char*)module_name.chars);
       tkscript->printf("[---] in module \"%s\" line %i.\n",
                        Dtkscompiler->getModuleNameByIndex(Dsrcloc_module(src_loc)),
                        Dsrcloc_linenr(src_loc)
                        );
#endif
       Dcterror(PTNERR_MODULENOTFOUND);
       return YAC_FALSE;
    }

    // ---- find member in resolved_module ----
    co = resolved_module->findModuleVar(&module_member_name);
    if(co)
    {
       resolved_var = co;
       return YAC_TRUE;
    }
    else
    {
       PTN_Function *fun = resolved_module->findFunction(&module_member_name);
       if(NULL != fun)
       {
          resolved_funobject = fun->getOrCreateFunctionObject();
          // tkscript->printf("xxx resolve FunctionObject \"%s\" => fo=%p, f=%p\n", module_member_name.chars, resolved_funobject, fun);
          const_val.initObject(resolved_funobject, 0);
          return YAC_TRUE;
       }
       else
       {
          /// ---- xxx check if member is also a module, which is not stored in ObjectCache ----
#ifdef TKS_DCON
          tkscript->printf("[---] variable or function \"%s\" not found in module \"%s\".\n",
                           (char*)module_member_name.chars,
                           (char*)module_name.chars
                           );
          tkscript->printf("[---] in module \"%s\" line %i.\n",
                           Dtkscompiler->getModuleNameByIndex(Dsrcloc_module(src_loc)),
                           Dsrcloc_linenr(src_loc)
                           );
#endif
          Dcterror(PTNERR_MODULEMEMBERNOTFOUND);
          return YAC_FALSE;
       }
    }

}

void PTN_UnresolvedModuleMemberExpr::eval(PTN_Env *_env, YAC_Value *_r) const {

   (void)_env;

   if(const_val.type)
   {
      _r->initValue(&const_val);
   }
   else
   {
      _r->initValue(resolved_var);
   }
}

static void PTN_UnresolvedModuleMemberExpr__eval(PTN_Env *_env, YAC_Value *ret, const PTN_Expr *_st) {

   (void)_env;

   Dtracest;
   const PTN_UnresolvedModuleMemberExpr *st = (const PTN_UnresolvedModuleMemberExpr*)_st;

   if(st->const_val.type)
   {
      ret->initValue(&st->const_val);
   }
   else
   {
      ret->initValue(st->resolved_var);
   }
}

Feval PTN_UnresolvedModuleMemberExpr::getEval(void) const {
	return PTN_UnresolvedModuleMemberExpr__eval;
}


#ifdef TKS_JIT
sBool PTN_UnresolvedModuleMemberExpr::forceHybrid(void) {
	if(const_val.type) return 0;
	else return (resolved_var!=0);
}

sU8 PTN_UnresolvedModuleMemberExpr::compile(VMCore *_vm) {
	if(const_val.type)
	{
		_vm->pushci(const_val.value.int_val);
		if(const_val.type>2) return 3;
		else return (sU8)const_val.type;
	}
	else
	{
		if(resolved_var)
		{
			_vm->pushv(resolved_var);
			return resolved_var->type>2?3:resolved_var->type;
		}
	}
	Dprintf("[---] cannot compile unresolved symbol expr \"%s\"\n",
		(char*)module_member_name.chars);
	Dcterror(PTNERR_CANTCOMPILE);
	tkscript->printResolveError((sChar*)module_member_name.chars);
	return 0xFF;
}
#endif
