/// TKS_Compiler.h
///
/// (c) 2001-2025 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///
#ifndef TKS_COMPILER_H__
#define TKS_COMPILER_H__


typedef struct __s_tmodentry {
	sUI magic;
	TKS_CachedScript *cscript;
} s_tmodentry;


class TKS_Compiler {
  public:
	TKS_Compiler(void);
	~TKS_Compiler();

	// ---- error handling ----
   sBool                  parse_error;

	// ---- modules ----
   StaticList::Node      *compiled_modules;   // a reverse list of all compiled modules in current pakfile chapter
   TKS_CachedScript      *current_module; // temporary during compilation, used for node memory allocation
   sU16                   num_modules;
   TKS_CachedScript      *main_module;
   PTN_Function          *main_fun;
   TKS_CachedScript      *scanAddModule        (YAC_String *_name, YAC_String *_source);
   void                   clearCompiledModules (void);
   TKS_CachedScript      *findModule           (YAC_String *_name) const;
   const char            *getModuleNameByIndex (sU16 _i) const;
   TKS_CachedScript      *getModuleByIndex     (sU16 _i) const;
   sUI                    getTotalNumLines     (void) const;

   // ---- user defined classes ----
   sBool                  no_destructors;
   Array<TKS_ClassDecl*>  class_decls;
   sU16                  *class_decl_sort_table;
   TKS_Namespace          default_namespace;
   TKS_Namespace          yac_namespace; // always empty, used to explicitely search only C++ classes (e.g. yac::String)
   Array<TKS_ScriptClassInstance*> class_initializers;

#ifdef TKS_ALLOW_TEMP_CLASSES
   void cleanTempClasses (TKS_CachedScript *_module);
#endif // TKS_ALLOW_TEMP_CLASSES

   TKS_ClassDecl         *addClassDecl                      (YAC_String *      _name,
                                                             TKS_CachedScript *_module,
                                                             sUI               _srcLoc,
                                                             TKS_Namespace *   _nspace
                                                             ); // find or add if not exists

   TKS_ClassDecl         *findClassDeclNamespaceList        (YAC_String *         _name,
                                                             TKS_ActiveNamespace *_activeNamespaces
                                                             ) const; // find or return 0

   TKS_ClassDecl         *findClassDeclNamespace            (YAC_String *         _name,
                                                             const TKS_Namespace *_nspace
                                                             ) const; // find in given namespace or in default nspace if _nspace is NULL

   TKS_ClassDecl         *findClassDeclNamespaceArray       (YAC_String *              _name,
                                                             TKS_FreezedNamespaceArray _nspaceArray
                                                             ) const; // find in one of the given namespaces

   TKS_CachedObject *     findVarNamespaceArray             (YAC_String *              _name,
                                                             TKS_FreezedNamespaceArray _nspaceArray
                                                             ) const;

   sBool                  createClassTemplates              (void);
   void                   initializeClasses                 (PTN_Env *_env);
   void                   callStaticClassMemberInitializers (PTN_Env *_env);
   sBool                  initializeStaticMethods           (void);

   // ---- temporary script modules ----
   s_tmodentry            temp_modules[TKS_MAX_TEMPMODULES];
   void                   initTempModules (void);
   sSI                    newTempModule (void);
   void                   deleteTempModule (sSI);
   void                   deleteTempModules (void);

  public:
   sBool                  init (void); // call before adding first instances/classes
   void                   clean (void);
   sBool                  compileCurrentChapter (PTN_Env *_env, sBool _bTSL = 0);
   void                   cleanCurrentChapter (void);
   sBool                  eval (TKS_Context *_context, TKS_CachedScript *_cached_and_compiled_script);

   // ---- add scriptobject which should be initialized (constructors) when the "runtime" starts
   void addClassInitializer (TKS_ScriptClassInstance *_sci);

   // ---- remove _sci from initializer list
   void removeClassInitializer (TKS_ScriptClassInstance *_sci);

#ifdef TKS_USE_NODEALLOCATOR
   TKS_LinearAllocator *static_node_allocator;
#endif // TKS_USE_NODEALLOCATOR

   // ---- resolve function/method args. 'fltMod' is used by temporary scripts (see TKS_ScriptModule).
   sBool resolveFunctionAndMethodArguments (TKS_CachedScript *_fltMod);
};


#endif // TKS_COMPILER_H__
