/// TKS_Compiler.cpp
///
/// (c) 2001-2025 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU General Public License (GPL).
///
///
///

#include <stdlib.h>
#include <string.h>

#include "tks.h"

#include "tks_inc_class.h"
#include "tks_inc_compiler.h"

#include "TKS_ScriptConstraint.h"

extern TKS_ClassDecl Class_ClassDecl;


TKS_Compiler::TKS_Compiler(void) {
   parse_error      = YAC_FALSE;
   // ---- modules ----
   num_modules      = 0u;
   main_module      = NULL;
   main_fun         = NULL;
   compiled_modules = NULL;
   current_module   = NULL;

   // ---- user defined classes -----
   class_decl_sort_table = NULL;
   class_decls.alloc(TKS_MAX_CLASSDECLS);
   class_decls.elements[0] = &Class_ClassDecl;
   class_decls.num_elements = 1u;
   for(sUI i = 1u; i < class_decls.max_elements; i++)
      class_decls.elements[i] = NULL;

   class_initializers.alloc(TKS_MAX_CLASSINITIALIZERS);
   no_destructors = YAC_FALSE;

   yac_namespace.name.visit("yac");
   default_namespace.name.visit("");

   initTempModules();

#ifdef TKS_USE_NODEALLOCATOR
   static_node_allocator = new TKS_LinearAllocator(TKS_NODEALLOCATOR_STRIPESIZE);
#endif // TKS_USE_NODEALLOCATOR
}

TKS_Compiler::~TKS_Compiler() {
	clean();
}

TKS_CachedScript *TKS_Compiler::scanAddModule(YAC_String *_name, YAC_String *_buffer) {
   TKS_CachedScript *ret = NULL;
   if(_name && _buffer && _name->chars)
   {
      if(_buffer->chars && !_buffer->isBlank())
      {
         TKS_CachedScript *cs = new TKS_CachedScript();

#ifdef TKS_USE_NODEALLOCATOR
         cs->visitNodeAllocator(static_node_allocator);
#endif // TKS_USE_NODEALLOCATOR

         cs->module_index = (sU16) ( num_modules +1 );
         cs->src_name.yacCopy(_name);
         //cs->name.yacCopy(_name); // set instance_name
         cs->name.visit(_name); // instance name, can be replaced using "module" statements
         if(cs->scanSource(_buffer, YAC_TRUE/*bRefCopy*/))
         {
            num_modules++;
            StaticList::InsertHead(&compiled_modules, (void *)cs); // store for later instanciation
            ret = cs;
         }
         else
         {
            Dprintf("\n[---] failed to preprocess module \"%s\".\n", (char*)cs->name.chars);
            delete cs;
         }
      }
      else
      {
         Dprintf("[---] cannot preprocess empty module source \"%s\"\n", (char*)_name->chars);
         Dprintf("[---] (check module path? currently using \"%s\"..\n", (const char*)tkscript->configuration.module_path.chars);
      }
   }
   return ret;
}

void TKS_Compiler::clearCompiledModules(void) {
   sBool bDebug = (tkscript->configuration.debug_level > 79);
   StaticList::Node *cc = compiled_modules;
   while(cc)
   {
      if(cc->data)
      {
         TKS_CachedScript *c = (TKS_CachedScript*)cc->data;
         if(bDebug) ::printf("~TKS_Compiler::clearCompiledModules: delete module \"%s\"\n", (const char*)c->name.chars);
         delete c;
         cc->data = NULL;
      }
      cc = cc->next;
   }
   if(bDebug) ::printf("~TKS_Compiler::clearCompiledModules: free compiled_modules list\n");
   StaticList::Free(&compiled_modules);
   num_modules = 0u;
}

TKS_CachedScript *TKS_Compiler::findModule(YAC_String *_name) const {
   StaticList::Node *c = compiled_modules;
   while(c)
   {
      if(c->data)
      {
         TKS_CachedScript *cc = (TKS_CachedScript*) c->data;
         if(cc->name.compare(_name))
         {
            return cc; // ---- found module ----
         }
      }
      c=c->next;
   }
   return NULL;
}

const char *TKS_Compiler::getModuleNameByIndex(sU16 _i) const {
   if(_i > 0u)
   {
      _i--;
      const char *ret = "<illegal>";
      StaticList::Node*cc = compiled_modules;
      cc = StaticList::Tail(cc);
      while(cc && (--_i < 0x3ff/*(todo) fix hardcoded 1024 module limit*/))
         cc = cc->previous;
      TKS_CachedScript *c = (TKS_CachedScript*)cc->data;
      if(c)
         ret = (char*)c->name.chars;
      return ret;
   }
   else
   {
      return "<temporary>";
   }
}

TKS_CachedScript *TKS_Compiler::getModuleByIndex(sU16 _i) const {
   if(_i > 0u)
   {
      _i--;
      StaticList::Node*cc = compiled_modules;
      cc = StaticList::Tail(cc);
      while(cc && (--_i < 0x3ff/*(todo) fix hardcoded 1024 module limit*/))
      {
         cc = cc->previous;
      }
      if(cc)
      {
         return (TKS_CachedScript*)cc->data;
      }
   }
   return NULL;
}

sUI TKS_Compiler::getTotalNumLines(void) const {
   sUI r = 0;
   StaticList::Node*cc = compiled_modules;
   while(cc)
   {
      r += ((TKS_CachedScript*)cc->data)->compiled_script->lines.num_elements;
      cc = cc->next;
   }
   return r;
}

TKS_ClassDecl *TKS_Compiler::addClassDecl(YAC_String *_name, TKS_CachedScript *_module, sUI _src_loc, TKS_Namespace *_nspace) {
   //
   // Add new classdecl. If the class already exists, simply return the TKS_ClassDecl object.
   //
   //

   if(NULL == _nspace)
   {
      _nspace = &Dtkscompiler->default_namespace;
   }

   TKS_ClassDecl *cd = findClassDeclNamespace(_name, _nspace);
   if(NULL != cd)
   {
      // ---- already got an entry, just update parent module and src_loc ----
      if(!cd->is_declared)
      {
         if(cd->module != _module)
         {
            // Dprintf("xxx TKS_Compiler::addClassDecl: update cd=%p name=\"%s\" cd->module=%p to %p\n", cd, cd->name.chars, cd->module, _module);
         }
         cd->module = _module; // Update parent module until the class declaration was found (in the correct module!)
         cd->src_loc = _src_loc;
      }
      return cd;
   }

   sBool bNew = YAC_FALSE;

#ifdef TKS_ALLOW_TEMP_CLASSES
   if(_module->is_tmod && tkscript->b_runtime)
   {
      // Try to re-use previously free'd class
      // Dprintf("xxx TKS_Compiler::addClassDecl: try to re-use cd for name=\"%s\"\n", (const char*)_name->chars);
      for(sUI i = 0u; i < class_decls.num_elements; i++)
      {
         TKS_ClassDecl *c = class_decls.elements[i];
         if(c->class_id > 0u && NULL == c->module)  // (note) class_id 0 is &Class_ClassDecl "null" class
         {
            cd = c;
            // Dprintf("xxx TKS_Compiler::addClassDecl: re-using cd=%p (class_id=%u) for name=\"%s\"\n", cd, cd->class_id, (const char*)_name->chars);
            break;
         }
      }
   }
#endif // TKS_ALLOW_TEMP_CLASSES

   if(NULL == cd && class_decls.num_elements != class_decls.max_elements)
   {
      cd = new(std::nothrow)TKS_ClassDecl();
      bNew = YAC_TRUE;
   }

   if(NULL != cd)
   {
      cd->src_loc  = _src_loc; // occurence of reference to this class
      cd->module   = _module;
      cd->name.yacCopy(_name);
      if(bNew)
      {
         cd->class_id = (sU16) class_decls.num_elements;
         // Dprintf("xxx TKS_Compiler::addClassDecl: assigning cd->class_id=%u to new class cd=%p name=\"%s\"\n", cd->class_id, cd, (const char*)cd->name.chars);
         cd->ancestor_table[cd->class_id] = 1; // can cast to self
         class_decls.elements[class_decls.num_elements++] = cd;
      }
      cd->init();
      cd->nspace   = _nspace;
      _nspace->addClassDecl(cd);
   }
   else
   {
      Dprintf("\n[---] TKS_Compiler::addClassDecl: class_decl table is full.\n");
   }

   return cd;
}

#ifdef TKS_ALLOW_TEMP_CLASSES
void TKS_Compiler::cleanTempClasses(TKS_CachedScript *_module) {
   if(_module->is_tmod)
   {
      for(sUI i = 1u; i < class_decls.num_elements; i++)
      {
         TKS_ClassDecl *cd = class_decls.elements[i];
         if(cd->isUsed())
         {
            // Dprintf("xxx TKS_Compiler::cleanTempClasses: cd->class_id=%u cd->name=\"%s\" _module=%p cd->module=%p\n", cd->class_id, cd->name.chars, _module, cd->module);
            if(cd->module == _module)
            {
               if(NULL != cd->nspace)
               {
                  // Dprintf("xxx TKS_Compiler::cleanTempClasses: removing cd=%p cd->name=\"%s\" from nspace=%p\n", cd, (const char*)cd->name.chars, cd->nspace);
                  cd->nspace->removeClassDecl(cd);
               }
               cd->clean();
               cd->init();  // (todo) optimize (re-init when re-used)
               cd->module = NULL;
            }
         }
      }
   }
}
#endif // TKS_ALLOW_TEMP_CLASSES


TKS_ClassDecl *TKS_Compiler::findClassDeclNamespaceList(YAC_String *         _name,
                                                        TKS_ActiveNamespace *_activeNamespaces
                                                        ) const
{
   //
   // Find classdecl in the given active namespaces list
   //
   // xxx TODO: find all callers and try to replace this call with a findAddClassDeclNamespaceList() variant if possible
   //
   TKS_ActiveNamespace *a = _activeNamespaces;
   while(a)
   {
      TKS_ClassDecl *cd = a->nspace->findClassDecl(_name);
      if(NULL != cd)
      {
         // Ok, found it
         return cd;
      }
      else
      {
         // Search next namespace
         a = a->next;
      }
   }
   // Failed
   return NULL;
}

TKS_ClassDecl *TKS_Compiler::findClassDeclNamespace(YAC_String *         _name,
                                                    const TKS_Namespace *_nspace
                                                    ) const
{
   if(_nspace != &Dtkscompiler->yac_namespace) // forced C++ namespace?
   {
      if(NULL != _nspace)
      {
         // Find script class within the given namespace
         return _nspace->findClassDecl(_name);
      }
      else
      {
         // Find script class in default namespace
         return Dtkscompiler->default_namespace.findClassDecl(_name);
      }
   }
   return NULL;
}

TKS_ClassDecl *TKS_Compiler::findClassDeclNamespaceArray(YAC_String *              _name,
                                                         TKS_FreezedNamespaceArray _nspaceArray
                                                         ) const
{
   //
   // Find classdecl in the given namespace array
   //
   // If the virtual "yac" namespace is found in the array, return NULL
   //
   if(NULL != _nspaceArray)
   {
      while(*_nspaceArray) // iterate array until NULL terminator
      {
         if( (*_nspaceArray) == &Dtkscompiler->yac_namespace ) // xxx maybe make yac_namespace::freeze return a NULL array instead?
         {
            // Force C++ namespace
            return NULL;
         }
         else
         {
            TKS_ClassDecl *cd = (*_nspaceArray)->findClassDecl(_name);
            if(NULL != cd)
            {
               // OK, found it
               return cd;
            }
            else
            {
               _nspaceArray++;
            }
         }
      }
   }
   // Failed
   return NULL;
}

TKS_CachedObject *TKS_Compiler::findVarNamespaceArray(YAC_String *              _name,
                                                      TKS_FreezedNamespaceArray _nspaceArray
                                                      ) const
{
   //
   // Find variable in the given namespace array
   //
   // If the virtual "yac" namespace is found in the array, return NULL
   //
   if(NULL != _nspaceArray)
   {
      while(*_nspaceArray) // iterate array until NULL terminator
      {
         if( (*_nspaceArray) == &Dtkscompiler->yac_namespace ) // xxx maybe make yac_namespace::freeze return a NULL array instead?
         {
            // Force C++ namespace
            return NULL;
         }
         else
         {
            TKS_CachedObject *co = (*_nspaceArray)->findVar(_name);
            if(NULL != co)
            {
               // OK, found it
               return co;
            }
            else
            {
               _nspaceArray++;
            }
         }
      }
   }
   // Failed
   return NULL;
}

sBool TKS_Compiler::createClassTemplates(void) {
   for(sUI i = 1u; i < class_decls.num_elements; i++)
   {
      TKS_ClassDecl *c = class_decls.elements[i];
      // yac_host->printf("xxx TKS_Compiler::createClassTemplates: i=%u c=%p\n", i, c);
      // yac_host->printf("xxx TKS_Compiler::createClassTemplates:   name=\"%s\" initialized=%d\n", c->name.chars, c->template_initialized);
      if(c->isUsed())
      {
         if(!c->template_initialized)
         {
            if(!c->createClassTemplate())
            {
               return YAC_FALSE;
            }
         }
      }
   }
   return YAC_TRUE;
}

void TKS_Compiler::initializeClasses(PTN_Env *_env) {
   sUI i;

   sUI bInitializing = tkscript->b_initializing;
   tkscript->b_initializing = 1; // 08Jan2008,
   //tkscript->b_initializing = 0; // orig pre27May07
   //tkscript->b_initializing = 1; // Should be 1 but this breaks member initializers ATM (see debug_brokenmemberinitializers.tks)

   // yac_host->printf("xxx TKS_Compiler::initializeClasses: class_initializers.num_elements=%u\n", class_initializers.num_elements);

   // Create member_data and call constructors
   for(i = 0u; i < class_initializers.num_elements; i++)
   {
      TKS_ScriptClassInstance *sci = class_initializers.elements[i];
      if(sci)
      {
         // Since b_initializing==1, yacOperatorInit won't call the constructors
         sci->yacOperatorInit(_env->context, sci->class_decl->class_template);
      }
   }
   tkscript->b_initializing = 0;
   for(i = class_initializers.num_elements - 1u; (sSI(i) >= 0); i--)
   {
      TKS_ScriptClassInstance *sci = class_initializers.elements[i];
      if(sci)
      {
         // Call constructors in 2nd pass so we don't access any NULL member_data fields
         // yac_host->printf("xxx TKS_Compiler::initializeClasses: sci->cd->name=\"%s\" callConstructors()\n", sci->class_decl->name.chars);
         sci->callConstructors(_env);
      }
   }
   class_initializers.free();
   tkscript->b_initializing = bInitializing;
}

void TKS_Compiler::addClassInitializer(TKS_ScriptClassInstance *_sci) {
   if(class_initializers.num_elements<class_initializers.max_elements)
   {
      class_initializers.elements[class_initializers.num_elements++]=_sci;
   }
   else
   {
      Dprintf("[---] addClassInitializer: maximum number of init objects exceeded (%i).\n",
              class_initializers.max_elements);
   }
}

void TKS_Compiler::removeClassInitializer(TKS_ScriptClassInstance *_sci) {
   for(sUI i = 0u; i < class_initializers.num_elements; i++)
   {
      TKS_ScriptClassInstance *c = class_initializers.elements[i];
      if(c == _sci)
      {
         ///tkscript->printf("removing class initializer for %08x", (sUI)_sci),
         class_initializers.elements[i] = NULL;
         return;
      }
   }
}

void TKS_Compiler::callStaticClassMemberInitializers(PTN_Env *_env) {
    // ----
    // ---- call static class member initializers ----
    // ---- recursively call ancestor initializers first (if they have not already been called)
    // ----
   for(sUI i = 1u; i < class_decls.num_elements; i++)
   {
      TKS_ClassDecl *cd = class_decls.elements[i];
      if(cd->isUsed())
      {
         if(!cd->b_done)
         {
            // Mark class as "fully processed" (side-effect)
            cd->b_done = YAC_TRUE;

            if(!cd->b_static_members_initialized)
            {
               cd->callRemoveAncestorStaticInitializers( _env );
            }
         }
      }
   }
}

void TKS_Compiler::clean(void) {
   sUI i;

   sBool bDebug = (tkscript->configuration.debug_level > 79);
   if(bDebug) ::printf("~TKS_Compiler::clean: ENTER\n");

 	no_destructors = (tkscript->ptn_error_code > 0); // no_destruct on parse error

   if(bDebug) ::printf("~TKS_Compiler::clean: free class initializer\n");

   class_initializers.free();

   if(YAC_FALSE == no_destructors)
   {
      // ---- call destructors for static class members ----
      for(i = 1u; i < class_decls.num_elements; i++)
      {
         TKS_ClassDecl *cd = class_decls.elements[i];
         if(cd->members.num_elements)
         {
            for(sUI j = 0u; j < cd->members.num_elements; j++)
            {
               TKS_ClassDeclMember *m = cd->members.elements[j];
               TKS_CachedObject *co = m->co_static;
               if(co)
               {
                  if(co->deleteme)
                  {
                     if(YAC_TYPE_OBJECT == co->type)
                     {
                        if(co->value.object_val)
                        {
                           if(YAC_CLID_CLASS == co->value.object_val->class_ID)
                           {
                              TKS_ScriptClassInstance *sci = (TKS_ScriptClassInstance*)co->value.object_val;
                              if(bDebug) ::printf("~TKS_Compiler::clean: deconstruct static class_decl member name=\"%s\"\n", (const char*)m->name.chars);
                              // xxx TKS_MT: use real compiling thread / create new context ??
                              PTN_Env env; env.initDefault();
                              sci->callDestructors( &env );
                           }
                        }
                     }
                  }
               }
            }
         }
      }
   }

   if(bDebug) ::printf("~TKS_Compiler::clean: clearCompiledModules()\n");

   clearCompiledModules();
	no_destructors = YAC_TRUE; // no_destruct

   //::printf("TKS_Compiler::clean: 4\n");

   // ---- delete static class members (which may also be class instances)
	for(i = 1u; i < class_decls.num_elements; i++)
	{
		if(class_decls.elements[i])
		{
         if(bDebug) ::printf("~TKS_Compiler::clean: delete static class decl member (class=\"%s\")\n", (const char*)class_decls.elements[i]->name.chars);
         class_decls.elements[i]->deleteStaticMembers();
		}
	}

   //::printf("TKS_Compiler::clean: 5\n");

   // ---- delete class methods
   for(i = 1u; i < class_decls.num_elements; i++)
	{
		TKS_ClassDecl *cd = class_decls.elements[i];
		if(cd)
		{
         //::printf("TKS_Compiler::clean: 5 cd->name=\"%s\"\n", cd->name.chars);
			if(cd->methods.elements)
			{
            for(sUI k = 0u; k < cd->methods.num_elements; k++)
            {
               if(bDebug) ::printf("~TKS_Compiler::clean: delete class method \"%s::%s\"\n", (const char*)cd->name.chars, (const char*)cd->methods.elements[k]->function.name.chars);
				   delete cd->methods.elements[k];
               cd->methods.elements[k]=0;
            }
            if(bDebug) ::printf("~TKS_Compiler::clean: delete class \"%s\" methods\n", (const char*)cd->name.chars);
            delete [] cd->methods.elements;
            cd->methods.elements     = NULL;
				cd->methods.num_elements = 0u;
				cd->methods.max_elements = 0u;
			}
		}
	}

   //::printf("TKS_Compiler::clean: 6\n");

   // ---- delete class templates (which are used to instantiate classes)
	for(i = 1u; i < class_decls.num_elements; i++)
	{
		if(class_decls.elements[i])
		{
			if(class_decls.elements[i]->class_template)
			{
            if(bDebug) ::printf("~TKS_Compiler::clean: delete class \"%s\" template\n", (const char*)class_decls.elements[i]->name.chars);
				delete class_decls.elements[i]->class_template;
				class_decls.elements[i]->class_template = NULL;
			}
		}
	}

   //::printf("TKS_Compiler::clean: 7\n");

   // ---- prepare the classes for deletion
	for(i = 1u; i < class_decls.num_elements; i++)
	{
		if(class_decls.elements[i])
		{
         if(bDebug) ::printf("~TKS_Compiler::clean: clean class \"%s\"\n", (const char*)class_decls.elements[i]->name.chars);
			class_decls.elements[i]->clean();
		}
	}

   //::printf("TKS_Compiler::clean: 8\n");

   // ---- delete the classes themselves
	for(i = 1u; i < class_decls.num_elements; i++)
	{
		if(class_decls.elements[i])
		{
         if(bDebug) ::printf("~TKS_Compiler::clean: delete class \"%s\"\n", (const char*)class_decls.elements[i]->name.chars);
			delete class_decls.elements[i];
         class_decls.elements[i] = NULL;
		}
	}

	no_destructors = YAC_FALSE;

   if(bDebug) ::printf("~TKS_Compiler::clean: free default_namespace\n");
   default_namespace.free();

   if(bDebug) ::printf("~TKS_Compiler::clean: free class_decls\n");
   class_decls.free();

   if(class_decl_sort_table)
   {
      if(bDebug) ::printf("~TKS_Compiler::clean: delete class_decl_sort_table\n");
      delete [] class_decl_sort_table;
      class_decl_sort_table = NULL;
   }

   main_module = NULL;
   main_fun = NULL;

#ifdef TKS_USE_NODEALLOCATOR
   if(NULL != static_node_allocator)
   {
      if(bDebug)
         ::printf("~TKS_Compiler::clean: delete static node allocator\n");
      delete static_node_allocator;
      static_node_allocator = NULL;
   }
#endif // TKS_USE_NODEALLOCATOR

   if(bDebug) ::printf("~TKS_Compiler::clean: LEAVE\n");
}

sBool TKS_Compiler::eval(TKS_Context *_context, TKS_CachedScript *_cs) {
   if(_cs)
   {
      if(_cs->parser_tree)
      {
         PTN_Env env;
         env.context = _context;
         env.continue_flag = 1;
         _cs->parser_tree->evalFirst(&env);
         env.cret.unsetFast();
      }
      return YAC_TRUE;
   }
   return YAC_FALSE;
}

sBool TKS_Compiler::initializeStaticMethods(void) {
   sBool ret = YAC_TRUE;
   for(sUI k = 1u; ret && (k < class_decls.num_elements); k++)
   {
      TKS_ClassDecl *cd = class_decls.elements[k];
      if(cd->isUsed())
      {
         // Dprintf("[...] TKS_Compiler::initializeStaticMethods: class \"%s\" is_declared=%d b_init_functions_done=%d.\n", (char*)cd->name.chars, cd->is_declared, cd->b_init_functions_done);
         ret = YAC_FALSE;
         if(cd->is_declared)
         {
            ret = cd->b_init_functions_done || cd->initializeFunctions();
         }
         else
         {
            // ---- reference to class has not been resolved during compilation
            tkscript->printResolveClassError((void*)cd);
            return YAC_FALSE;
         }
      }
   }
   return ret;
}

sBool TKS_Compiler::compileCurrentChapter(PTN_Env *_env, sBool _bTSL) {
   // if(_bTSL) yac_host->printf("xxx compileCurrentChapter: bTSL=%d\n", _bTSL);
   TKX_Chapter *chapter = tkscript->tkx.current_chapter;
   sU16 numCompiledFiles = 0u;

   sUI milliSec    = 0u; // assign 0 to satisfy GCC
   sUI milliSecOpt = 0u; // assign 0 to satisfy GCC
   sUI totalNumStrings     = 0u;
   sUI totalNumStringChars = 0u;
   sUI totalNumTokens      = 0u;
   sUI totalNumLines       = 0u;
   sUI totalNumNodes       = 0u;
   sUI totalNumNodesOpt    = 0u;

   if(tkscript->configuration.b_compile_show_progress)
   {
      milliSec = tkscript->getMilliSeconds();
      if(tkscript->configuration.debug_level > 0)
      {
         Dprintf("[...] scanning: ");
      }
   }
   Dpcscan_start();

   TKS_TokenizedScript::AllocTSLCache();

   if(chapter)
   {
      // pass1: scan modules

      if(!_bTSL)
      {
         main_module = NULL;
         main_fun    = NULL;
      }

      TKX_File *cf = chapter->chapter_files;
      while(cf)
      {
         // yac_host->printf("xxx compileCurrentChapter: chapter=%p cf->logic_name=\"%s\" b_done=%d next=%p\n", chapter, cf->logic_name.chars, cf->b_done, cf->next);
         if(!cf->b_done)
         {
            cf->b_done = YAC_TRUE;

            if(cf->logic_name.endsWith(".tks"))
            {
               YAC_String srcbuf;
               if(tkscript->configuration.b_compile_show_progress)
               {
                  if(tkscript->configuration.debug_level > 0)
                  {
                     Dprintf(".");
                  }
               }

               YAC_String srcFileName;
               YAC_String srcFileNameMod;
               sBool bSrcFileLocal;
               sSI cfeType;

               if(tkscript->configuration.b_simulatevfs)
               {
                  cfeType = cf->type;
               }
               else
               {
                  // (note) the file offset hack was necessary because I forgot to save the type of a file entry.
                  //        since offset=size=0 was saved for a non FILE type entry, the offset field was later on
                  //        used to identify the actual resource type.. :)
                  cfeType = cf->file_offset;
               }


               if(TKX_File::CFE_TYPE_LOCAL == cfeType)
               {
                  // filename is absolute or relative to current directory
                  srcFileName.visit(&cf->logic_name);
                  bSrcFileLocal = YAC_TRUE;
               }
               else if(TKX_File::CFE_TYPE_LOCAL_HOME == cfeType)
               {
                  // filename is relative to $HOME directory
                  srcFileName.yacCopy(&tkscript->configuration.home_dir);
                  srcFileName.append("/");
                  srcFileName.append(&cf->logic_name);
                  bSrcFileLocal = YAC_TRUE;
               }
               else
               {
                  // load from pakfile (or local file in simulation mode)
                  srcFileName.visit(&cf->logic_name);
                  bSrcFileLocal = YAC_FALSE;
               }

               if(srcFileName.startsWith("mod:"))
               {
                  YAC_String logicFileNameMod;
                  cf->logic_name.substring(&logicFileNameMod, 4);
                  srcFileName.yacCopy(&tkscript->configuration.module_path);
                  srcFileName.append("/");
                  srcFileName.append(&logicFileNameMod);
               }

               sBool bOpened = srcbuf.sio_readFile(bSrcFileLocal, (char*)srcFileName.chars);

               if(!bOpened)
               {
                  if(TKX_File::CFE_TYPE_LOCAL_HOME == cfeType)
                  {
                     // Fallback: Try local directory
                     tkscript->printf("[~~~] source file \"home:%s\" not found, falling back to local directory.. (home_dir=\"%s\")\n",
                                      srcFileName.chars,
                                      (char*)tkscript->configuration.home_dir.chars
                                      );

                     srcFileName.visit(&cf->logic_name);

                     bOpened = srcbuf.sio_readFile(bSrcFileLocal, (char*)srcFileName.chars);
                  }
               }

               if(bOpened)
               {
                  Dpcscan_startSample(scanAddModule);
                  //Dprintf("xxx scanAddModule \"%s\".\n", cf->logic_name.chars);
                  // // if(-1 != cf->logic_name.indexOf("svg", 0))
                  // // {
                  // //    int xxx = 42;
                  // // }
                  TKS_CachedScript *bc = scanAddModule(&cf->logic_name, &srcbuf);
                  Dpcscan_stopSample(scanAddModule);

                  if(!bc)
                  {
                     parse_error = YAC_TRUE; // ---- module scanner failed ----
                     TKS_TokenizedScript::FreeTSLCache();
                     return YAC_FALSE;
                  }
                  else
                  {
#ifdef TKS_PROFILE_STATEMENTS
                     if(tkscript->configuration.b_enable_statement_profiling)
                     {
                        tkscript->statement_profiler.addModule(bc->compiled_script->lines.num_elements+1);
                     }
#endif
                     cf->freeTokenizedSource();


                     totalNumTokens  += bc->compiled_script->tokens.num_elements;
                     totalNumStrings += bc->compiled_script->strings.num_elements;
                     totalNumLines   += bc->compiled_script->lines.num_elements;
                  }
               }
               else
               {
                  Dprintf("\n[---] failed to load logic source file \"%s\".\n",
                          (char*)(srcFileName.chars));
                  parse_error = YAC_TRUE;
                  TKS_TokenizedScript::FreeTSLCache();
                  return YAC_FALSE;
               }
            }
            else if(cf->logic_name.endsWith(".tsl"))
            {
               // Load packed tkscript library and merge with pakfile (replaces current file)
               if(!chapter->mergeTSL(cf))
               {
                  parse_error = YAC_TRUE; // ---- load script library failed ----
                  TKS_TokenizedScript::FreeTSLCache();
                  return YAC_FALSE;
               }
            }

         } // if !cf->b_done

         cf = cf->next;
      }
   }
   else
   {
      Dprintf("\n[---] compileCurrentChapter: current_chapter==0.\n");
      parse_error = YAC_TRUE;
      TKS_TokenizedScript::FreeTSLCache();
      return YAC_FALSE;
   }

   // yac_host->printf("xxx compileCurrentChapter: module scan OK\n");

   TKS_TokenizedScript::FreeTSLCache();

   // End of scanner pass
   if(tkscript->configuration.b_compile_show_progress)
   {
      if(tkscript->configuration.debug_level > 0)
      {
         Dprintf("\n");
      }
   }
   Dpcscan_stop();


   // generate unique list of dependencies for each and every classdecl found
   for(sUI k = 1u; k < class_decls.num_elements; k++)
   {
      TKS_ClassDecl *cd = class_decls.elements[k];
      if(cd->isUsed())
      {
         //dcon.printf("[...] class \"%s\".\n", (char*)cd->name.chars);
         cd->getNumDependencies(cd);
      }
   }

   // yac_host->printf("xxx compileCurrentChapter: class dependencies OK\n");

   if(!createClassTemplates())
   {
      return YAC_FALSE;
   }

   // yac_host->printf("xxx compileCurrentChapter: class templates OK\n");

   // ---- now that all ADTs have been declared, resolve static class methods (class functions)
   // ---- TODO: one day regular functions should also be added during the scanner pass to avoid
   // ---- "unresolvedmodulemethodxxx" pnodes
   if(!resolveFunctionAndMethodArguments(NULL/*fltMod*/))
   {
      return YAC_FALSE;
   }


   // pass2: compile modules

   if(tkscript->configuration.b_compile_show_progress)
   {
      if(tkscript->configuration.debug_level > 0)
      {
         // Count #chars
         StaticList::Node *c = compiled_modules;
         for(sUI i = 0u; i < num_modules; i++)
         {
            TKS_CachedScript *cs = (TKS_CachedScript*)c->data;
            if(NULL != cs->compiled_script)
            {
               sUI j;
               for(j = 0u; j < cs->compiled_script->strings.num_elements; j++)
               {
                  YAC_String *s = &cs->compiled_script->strings.elements[j];
                  if(s->bflags & TKS_STRFL_DEL)
                  {
                     totalNumStringChars += s->buflen;
                  }
               }
            }
            c = c->next;
         }
      }

      if(tkscript->configuration.debug_level > 0)
      {
         Dprintf("[...] compiling: ");
      }
   }

   // yac_host->printf("xxx compileCurrentChapter: module string chars OK\n");

   sUI numCompiledNodes = 0u;
   (void)numCompiledNodes;
   {
      StaticList::Node *cn = compiled_modules;
      cn = StaticList::Tail(cn);
      while(cn)
      {
         TKS_CachedScript *cs = (TKS_CachedScript*)cn->data;
         if(cs && !cs->b_done)
         {
            if(tkscript->configuration.b_compile_show_progress)
            {
               if(tkscript->configuration.debug_level > 0)
               {
                  Dprintf(".");
               }
            }

            if(cs->compileSource())
            {
               numCompiledNodes += cs->parser_tree_num_nodes;
               if(cs->name.compare("Main"))
               {
                  main_module = cs;
               }
               numCompiledFiles++;
            }
            else
            {
               Dprintf("[---] failed to compile module \"%s\".\n", (char*)cs->name.chars);
               parse_error = YAC_TRUE;
               return YAC_FALSE;
            }
         }
         cn = cn->previous;
      }
   }

   totalNumNodes = PTN_Node::object_counter;

   // yac_host->printf("xxx compileCurrentChapter: module compile OK\n");

   // End of compiler pass
   if(tkscript->configuration.b_compile_show_progress)
   {
      if(tkscript->configuration.debug_level > 0)
      {
         Dprintf("\n");
      }
   }


   // ---- construct classes ----
   //////////initializeClasses();  // done by caller


   if(tkscript->configuration.b_compile_show_progress)
   {
      milliSecOpt = tkscript->getMilliSeconds();
      if(tkscript->configuration.debug_level > 0)
      {
         Dprintf("[...] optimizing: ");
      }
   }

   // ---- Resolve formal arguments


   // ---- perform semanticCheck() / resolveXRef() / optimize() for static class methods
   if(!initializeStaticMethods())
      return YAC_FALSE;

#ifdef DX_SCRIPTCONSTRAINTS
   // ---- perform semanticCheck() for script constraints
   {
      TKS_ScriptConstraint *scs = NULL;
      TKS_ObjectCache      *vc  = &tkscript->script_constraints;
      TKS_CachedObject    **co  = vc->objects;
      for(sUI i = 0u; i < vc->max_entries; i++)
      {
         if(co[i])
         {
            scs = (TKS_ScriptConstraint*)co[i]->value.object_val;
            if(!scs->b_done && scs->constraint_expr)
            {
               scs->b_done = YAC_TRUE;

               if(scs->constraint_expr->semanticCheck())
               {
                  if(scs->constraint_expr->resolveXRef())
                  {
                     scs->constraint_expr->optimize();
                  }
                  else
                  {
                     Dprintf("[---] semanticCheck() for constraint \"%s\" failed.\n", (char*)scs->name.chars);
                     return YAC_FALSE;
                  }
               }
               else
               {
                  Dprintf("[---] resolveXRef() for constraint \"%s\" failed.\n", (char*)scs->name.chars);
                  return YAC_FALSE;
               }
            }
         }
      }
   }
#endif // DX_SCRIPTCONSTRAINTS

   // ---- semantic check for exception types
   if(!tkscript->areAllExceptionsDeclared())
   {
      parse_error = YAC_TRUE;
      return YAC_FALSE;
   }

#ifdef TKS_FIX_ABSTRACTVARS
   tkscript->fixAllAbstractVars();
#endif // TKS_FIX_ABSTRACTVARS

   // ---- initialize static class members
   //       (note) side-effect: set class b_done flag
   callStaticClassMemberInitializers( _env );

   if(!_bTSL)
   {
      if( !tkscript->configuration.b_emit_explanations && (NULL == main_module) )
      {
         Dprintf("\n[---] missing \"Main\" module.\n");
         parse_error = YAC_TRUE;
         return YAC_FALSE;
      }

      if(!tkscript->configuration.b_emit_explanations)
      {
         YAC_String nt;
         nt.copy("main");
         main_fun = main_module->findFunction(&nt);
#ifdef TKS_DCON
         if(tkscript->configuration.debug_level>99)
         {
            if(!main_fun)
            {
               Dprintf("\n[...] warning: Main::main function not found.\n");
            }
            Dprintf("[...] compileCurrentChapter: %i modules compiled (%i nodes).\n",
                    numCompiledFiles, numCompiledNodes/*totalNumNodes*/);
         }
#endif
      }
   }

   // all classes are compiled
   // basemodules have been compiled
   // pass3: setup all compiled nodes
   StaticList::Node *c = compiled_modules;
   for(sUI i = 0u; i < num_modules; i++)
   {
      if(!c)
      {
         Dprintf("\n[---] internal error: compiled_modules list is broken (c==0, i==%u).\n", i);
         parse_error = YAC_TRUE;
         return YAC_FALSE;
      }
      TKS_CachedScript *bc = (TKS_CachedScript*)c->data;
      if(bc)
      {
         if(!bc->b_done)
         {
            // Mark module as "fully processed" (incremental updates / dynamic TSL loader)
            bc->b_done = YAC_TRUE;

            if(bc->parser_tree)
            {
               if(tkscript->configuration.b_compile_show_progress)
               {
                  if(tkscript->configuration.debug_level > 0)
                  {
                     Dprintf(".");
                  }
               }

               if(!bc->parser_tree->resolveXRefAll())
               {
                  Dprintf("[---]\n[---] parse error in module \"%s\"\n[---]  line %i.\n[---]\n",
                          getModuleNameByIndex(Dsrcloc_module(tkscript->ptn_error_loc)),
                          Dsrcloc_linenr(tkscript->ptn_error_loc)
                          );
                  parse_error = YAC_TRUE;
                  return YAC_FALSE;
               }

               bc->parser_tree->optimizeAll();

               if(tkscript->ptn_error_code)
               {
                  parse_error = YAC_TRUE;
                  return YAC_FALSE;
               }
            }
            else
            {
               Dprintf("\n[---] internal error: module \"%s\" parser_tree is empty.\n",
                       (char*)bc->name.chars
                       );
               parse_error = YAC_TRUE;
               return YAC_FALSE;
            }
         } // if !bc->b_done
      }
      else
      {
         Dprintf("\n[---] internal error: compiled_modules list is broken (c->data==0).\n");
         parse_error = YAC_TRUE;
         return YAC_FALSE;
      }

      // Next module
      c = c->next;
   }

   if(tkscript->configuration.b_compile_show_progress)
   {
      milliSecOpt = tkscript->getMilliSeconds() - milliSecOpt;
   }

   totalNumNodesOpt = PTN_Node::object_counter;

   if(tkscript->configuration.b_compile_show_progress && num_modules)
   {
      milliSec = tkscript->getMilliSeconds() - milliSec;
      Dprintf("\n[...] compile time: %d millisec (%4.2f millisec avg. per module).\n",
              milliSec, ((sF32)milliSec) / num_modules
              );
      Dprintf("[...] optimize time: %d millisec (%4.2f millisec avg. per module).\n",
              milliSecOpt, ((sF32)milliSecOpt) / num_modules
              );
      Dprintf("[...]     #modules = % 7u\n", num_modules);
      Dprintf("[...]      #tokens = % 7u\n", totalNumTokens);
      Dprintf("[...]     #strings = % 7u\n", totalNumStrings);
      if(tkscript->configuration.debug_level > 0)
      {
         Dprintf("[...]  #tokenchars = % 7u\n", totalNumStringChars);
      }
      Dprintf("[...]       #lines = % 7u\n", totalNumLines);
      Dprintf("[...]     #LOC/sec = %.2f\n", (1000.0 / milliSec) * totalNumLines);
      Dprintf("[...] #unopt.nodes = % 7u\n", totalNumNodes);
      Dprintf("[...]   #opt.nodes = % 7u\n", totalNumNodesOpt);
#ifdef TKS_USE_NODEALLOCATOR
      {
         sUI nbNum, nbMax;
         static_node_allocator->calcSizes(nbNum, nbMax);
         Dprintf("[...]   #nodebytes = % 7u / %u\n", nbNum, nbMax);
      }
#endif // TKS_USE_NODEALLOCATOR
#ifdef TKS_USE_CHARALLOCATOR
      {
         sUI nbNum, nbMax;
         TKS_ScriptEngine::char_allocator->calcSizes(nbNum, nbMax);
         Dprintf("[...]  #cachechars = % 7u / %u\n", nbNum, nbMax);
      }
#endif // TKS_USE_NODEALLOCATOR
      Dprintf("[...]     #classes = % 7u\n", class_decls.num_elements);
      sUI totalNumMethods = 0u;
      sUI totalNumMembers = 0u;
      sUI totalNumTemplateMembers = 0u; // including baseclasses
      sUI totalMaxTemplateMembers = 0u; // including baseclasses
      sUI totalNumTemplateMethods = 0u; // including baseclasses
      sUI totalMaxTemplateMethods = 0u; // including baseclasses
      for(sUI i = 1u; i < class_decls.num_elements; i++)
      {
         TKS_ClassDecl *cd = class_decls.elements[i];
         if(cd->isUsed())
         {
            totalNumMethods += cd->methods.num_elements;
            totalNumMembers += cd->members.num_elements;
            totalNumTemplateMembers += cd->template_member_hashtable.num_entries;
            totalMaxTemplateMembers += cd->template_member_hashtable.max_entries;
            totalNumTemplateMethods += cd->template_method_hashtable.num_entries;
            totalMaxTemplateMethods += cd->template_method_hashtable.max_entries;
         }
      }
      Dprintf("[...]     #methods = % 7u\n", totalNumMethods);
      Dprintf("[...]     #members = % 7u\n", totalNumMembers);
      Dprintf("[...]   #t.methods = % 7u / %u\n", totalNumTemplateMethods, totalMaxTemplateMethods);
      Dprintf("[...]   #t.members = % 7u / %u\n", totalNumTemplateMembers, totalMaxTemplateMembers);
      Dprintf("[...]  #classinits = % 7u\n", class_initializers.num_elements);
   }

   // yac_host->printf("xxx compileCurrentChapter: LEAVE (ok) bTSL=%d\n", _bTSL);

   // ---- yia! we made it :) ----
   return YAC_TRUE;
}

sBool TKS_Compiler::resolveFunctionAndMethodArguments(TKS_CachedScript *_fltMod) {
   sBool ret = YAC_TRUE;
   sUI k;
   for(k = 1u; ret && k < class_decls.num_elements; k++)
   {
      TKS_ClassDecl *cd = class_decls.elements[k];
      if(!cd->b_args_resolved)
      {
         if(NULL == _fltMod || cd->module == _fltMod)
         {
            // Dprintf("[dbg] TKS_Compiler::resolveFunctionAndMethodArguments: class \"%s\".\n", (char*)cd->name.chars);
            ret = ret && cd->resolveFunctionArguments();
            ret = ret && cd->resolveMethodArguments();
            cd->b_args_resolved = YAC_TRUE;
         }
      }
   }
   return ret;
}

void TKS_Compiler::initTempModules(void) {
   sUI i;
   for(i = 0u; i < TKS_MAX_TEMPMODULES; i++)
   {
      temp_modules[i].cscript = NULL;
      temp_modules[i].magic   = 0u;
   }
}

void TKS_Compiler::deleteTempModules(void) {
   for(sUI i = 0u; i < TKS_MAX_TEMPMODULES; i++)
   {
      if(temp_modules[i].cscript)
      {
         delete temp_modules[i].cscript;
         temp_modules[i].cscript = NULL;
         temp_modules[i].magic = !temp_modules[i].magic;
      }
   }
}

sSI TKS_Compiler::newTempModule(void) {
   for(sUI i = 0u; i < TKS_MAX_TEMPMODULES; i++)
   {
      if(!temp_modules[i].cscript)
      {
         TKS_CachedScript *cs = new TKS_CachedScript();
         cs->module_index = 0; // 0=temporary module
#ifdef TKS_USE_NODEALLOCATOR
         cs->newNodeAllocator();
#endif // TKS_USE_NODEALLOCATOR
         temp_modules[i].cscript =  cs;
         temp_modules[i].magic   = (sUI)::rand();
         // Dprintf("xxx TKS_Compiler::newTempModule: i=%u\n", i);
         return i;
      }
   }

   printf("[---] TKS_Compiler::newTempModule: max. number of modules (%u) exceeded.\n", TKS_MAX_TEMPMODULES);
   return -1;
}

void TKS_Compiler::deleteTempModule(sSI _id) {
   // Dprintf("[dbg] TKS_Compiler::deleteTempModule: id=%d\n", _id);
   if(sUI(_id) < TKS_MAX_TEMPMODULES)
   {
      s_tmodentry *t = &temp_modules[sUI(_id)];
      // Dprintf("[dbg] TKS_Compiler::deleteTempModule: id=%d t->cscript=0x%p\n", _id, t->cscript);

      if(NULL != t->cscript)
      {
         delete t->cscript;
         t->cscript = NULL;
         t->magic   = !t->magic; // invalidate magic field
      }
   }
}
