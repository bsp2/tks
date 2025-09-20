/// TKS_Namespace.cpp
///
/// (c) 2008-2025 by Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///
///
#include "tks.h"
#include "tks_inc_class.h"


TKS_Namespace::TKS_Namespace(void) {
   init();
}

TKS_Namespace::~TKS_Namespace() {
   TKS_Namespace::free();
}

void TKS_Namespace::init(void) {
   class_decl_hash.allocCache(TKS_MAX_CLASSDECLS + 1);
   global_vars.allocCache(TKS_MAX_NAMESPACE_VARIABLES + 1);
   function_hash.allocCache(TKS_MAX_FUNCTIONS + 1);
}

void TKS_Namespace::free(void) {
   freeVars();
   freeTypes();
}

void TKS_Namespace::freeVars(void) {
   global_vars.freeCache();
}

void TKS_Namespace::freeTypes(void) {
   class_decl_hash.freeCache();
}

TKS_CachedObject *TKS_Namespace::addClassDecl(TKS_ClassDecl *_cd) {
   // Dprintf("xxx TKS_Namespace::addClassDecl: cd=%p cd->class_id=%u cd->name=\"%s\" cd->module=%p\n", _cd, _cd->class_id, (const char*)_cd->name.chars, _cd->module);
   TKS_CachedObject *co = class_decl_hash.createAnyEntry(&_cd->name, YAC_FALSE/*bCopyName*/, (void*)_cd);
   return co;
}

TKS_ClassDecl *TKS_Namespace::findClassDecl(YAC_String *_name) const {
   TKS_CachedObject *co = class_decl_hash.findEntry(_name);
   if(NULL != co)
   {
      // Ok, found it
      return (TKS_ClassDecl*) co->value.any;
   }
   // Failed
   return NULL;
}

void TKS_Namespace::removeClassDecl(TKS_ClassDecl *_cd) {
   class_decl_hash.removeEntry(&_cd->name);
}

TKS_CachedObject *TKS_Namespace::addFunction(PTN_Function *_fun) {
   TKS_CachedObject *co = function_hash.createAnyEntry(&_fun->name, YAC_FALSE/*bCopyName*/, (void*)_fun);
   return co;
}

PTN_Function *TKS_Namespace::findFunction(YAC_String *_name) const {
   TKS_CachedObject *co = function_hash.findEntry(_name);
   if(NULL != co)
   {
      // Ok, found it
      return (PTN_Function*) co->value.any;
   }
   // Failed
   return NULL;
}

TKS_FreezedNamespaceArray TKS_Namespace::freeze(void) {
   TKS_FreezedNamespaceArray f = new TKS_Namespace*[2];
   f[0] = this;
   f[1] = NULL;
   return f;
}

TKS_CachedObject *TKS_Namespace::findVar(YAC_String *_name) const {
   return global_vars.findEntry(_name);
}
