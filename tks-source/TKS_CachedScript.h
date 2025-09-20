/// TKS_CachedScript.h
///
/// (c) 2001-2025 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///

#ifndef TKS_CACHEDSCRIPT_H__
#define TKS_CACHEDSCRIPT_H__


class TKS_CachedScript {
private:
	sU16 stp;
	sU16 state;
	sU16 state_index;
	static TKS_StateStackEntry state_stack[TKS_MAX_PARSER_STACKDEPTH];

	void pushState        (void);
	void popState         (void);
   void unwindStateStack (void);

private:
	PTN_BreakableStatement        *pbreak;
	static PTN_BreakableStatement *break_stack[TKS_BREAK_STACK_SIZE+1];
	sSI                            breaki;
	void pushBreak    (PTN_BreakableStatement*);
	void popBreak     (void);

   YAC_String *current_exception_varname;
   YAC_String *current_exception_varname_stack[TKS_TRYCATCHSTACKDEPTH]; // exception alias name in catch{}
   sSI trycatch_stacki;
   void pushExceptionVarname (YAC_String *_s);
   void popExceptionVarname  (void);

public:
	sBool             is_tmod; // 1==is temporary script
private:
   TKS_Namespace    *tmod_class_nsp;  // forced namespace, or NULL

public:
	YAC_String           name;
   YAC_String           src_name;
   YAC_String           documentation;
	TKS_ObjectCache      module_vars; // Holds global module variables
	TKS_ObjectCache      functions;
	TKS_TokenizedScript *compiled_script;
	sUI                  parser_tree_num_nodes;
	PTN_Statement       *parser_tree;
	sU16                 module_index;

   TKS_ClassDecl    *cclassdecl;
   PTN_Function     *cfunction;

   Array<TKS_CachedObject*>tagged_vars;  // for temp modules

   sBool b_done;  // 1=module has been fully processed. Used for incremental updates when dynamically loading TSLs.
   sBool b_global_module_statements_done;  // see TKS_ScriptEngine::runGlobalModuleStatements()

public:
   void setTempScriptClassNamespace (TKS_Namespace *_nsp);

   sUI            linenr_offset;

private:
	void parseError(sU16 _line/*, sU16 _state, sU16 _state_index*/, sUI _toki, sU16 _tok);
	sBool compile(void);

	PTAnyNodePtr   popnode;
	PTAnyNodePtr   tnode;
	PTAnyNodePtr   t2node;
	sUI            linenr;
	PTN_Statement *first_stat;

   void setTNode(PTN_Node*);
	void setPNode(PTN_Node*);

   void spawnClassConstantExpr(TKS_CachedObject *_class_constant);

private:
   TKS_ActiveNamespace *active_namespaces; // single linked list of currently imported namespaces, always includes "default_namespace" object
   TKS_Namespace *create_namespace; // which namespace to use to create vars, classes etc.

   void initActiveNamespaces (void);
   void freeActiveNamespaces (void);
   void useNamespace         (TKS_Namespace *_nspace); // Add the given namespace to the top of the search list
   void useDefaultNamespace  (void); // clear search list and revert back to global/default namespace
   void selectNamespace      (TKS_Namespace *_nspace);

private:
   // Find the given class type, add forward decl. if it does not exist, yet (first (scan) pass)
   sBool findAddClassType(YAC_String *         _name,
                          TKS_ActiveNamespace *_activeNSpaces,
                          TKS_Namespace*       _cnspace,
                          sUI                  _srcloc,
                          TKS_ClassTemplate**  _retTemplate,
                          TKS_ClassDecl **     _retClassDecl
                          );

   // Find the given class type, return an error if it does not exist (second pass)
   sBool findDeclaredClassType(YAC_String *         _name,
                               TKS_ActiveNamespace *_activeNSpaces,
                               TKS_Namespace*       _cnspace,
                               TKS_ClassTemplate**  _retTemplate,
                               TKS_ClassDecl **     _retClassDecl
                               ) const;


public:
	TKS_CachedScript(void);
	~TKS_CachedScript();

	void              init  (void);
	void              clean (void);
	sBool             scanSource    (YAC_String *_newsourcescript, sBool _bRefCopy);
  private:
	sBool             scanClasses   (void);
  public:
	sBool             compileSource (void);
	void              addConstant   (YAC_String*,YAC_String*);
	PTN_Function     *findFunction  (YAC_String *_name) const;
	PTN_Function     *findFunction  (const char *_name) const;
	TKS_CachedObject *findModuleVar (YAC_String *_name) const;
	TKS_CachedObject *findVarLocal  (YAC_String *_sv) const;

	TKS_CachedObject *findVarGlobal (YAC_String *         _sv,
                                    TKS_ActiveNamespace *_activeNamespaces
                                    ) const;

	TKS_CachedObject *findVar       (YAC_String *         _sv,
                                    TKS_ActiveNamespace *_activeNamespaces,
                                    TKS_Namespace *      _forcedNSpace
                                    ) const;

	TKS_CachedObject *findConst     (YAC_String *_sv) const;
	sSI               getIntConst   (YAC_String *_sv) const;

   void fixAbstractModuleVars(void);

   sUI getLineIdx (void) { return linenr; }

 public:
   TKS_LinearAllocator *node_allocator;
   sBool                b_own_node_allocator;

   void visitNodeAllocator (TKS_LinearAllocator *_allocator);
   void newNodeAllocator   (void);
   void freeNodeAllocator  (void);

};


#endif // TKS_CACHEDSCRIPT_H__
