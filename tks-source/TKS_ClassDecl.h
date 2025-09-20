/// TKS_ClassDecl.h
///
/// (c) 2001-2025 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef TKS_CLASSDECL_H__
#define TKS_CLASSDECL_H__


class TKS_CachedScript;


enum __classdeclvartypes {
    TKS_CLASSDECL_RTTI_INT          = 1,
    TKS_CLASSDECL_RTTI_FLOAT        = 2,
    TKS_CLASSDECL_RTTI_OBJECT       = 3,
    TKS_CLASSDECL_RTTI_STRING       = 4,
    TKS_CLASSDECL_RTTI_POINTER      = 5,
    TKS_CLASSDECL_RTTI_CLASS        = 6,
    TKS_CLASSDECL_RTTI_INTARRAY     = 7,
    TKS_CLASSDECL_RTTI_FLOATARRAY   = 8,
    TKS_CLASSDECL_RTTI_OBJECTARRAY  = 9,
    TKS_CLASSDECL_RTTI_STRINGARRAY  = 10,
    TKS_CLASSDECL_RTTI_POINTERARRAY = 11,
    TKS_CLASSDECL_RTTI_CLASSARRAY   = 12,

    TKS_CLASSDECL_RTTI_PAD
};

// class member id (32bit):
//   iiiiiiiiiiiiiiiirrrroooooooooooo
//   |               |   \____________ shifted offset (base class members)
//   |               \________________ rtti mask (see above)
//   \________________________________ class id (declaration)

#define TKS_CLASSDECL_OFFMASK 0xFFF

#define TKS_CLASSDECL_RTTISHIFT 12
#define TKS_CLASSDECL_RTTIMASK  0xF

#define TKS_CLASSDECL_IDSHIFT 16


class TKS_ClassDecl {
public:
  YAC_String name;
  YAC_String documentation;
  
  struct __members {
     // Holds all members declared within this class
    sUI                  max_elements;
    sUI                  num_elements;
    TKS_ClassDeclMember**elements;
  } members;
  
  struct __methods {
     // Holds all methods declared within this class
    sUI                  max_elements;
    sUI                  num_elements;
    TKS_ClassDeclMethod**elements;
  } methods;

  struct __ancestors {
     // Holds all direct ancestors classes
    sUI             max_elements;
    sUI             num_elements;
    TKS_ClassDecl **elements;
  } ancestors;
  
  struct __class_dependencies {
     // In earlier versions of tkscript, this array used to hold all classes which 
     // are instantiated in member fields.
     // Nowadays it holds all ancestor (i.e. also ancestors of ancestors) classes. 
     // I guess I should rename it :)
     sUI             max_elements;
     sUI             num_elements;
     TKS_ClassDecl **elements;
  } class_dependencies; 

  // used to detect recursive initialization, 
  // 1 if this class has a member of class type <idx>
  // only used temporarily while compiling the modules
  sU8 *class_field_mask; 

  struct __all_members { // Filled in createClassTemplate()
     sUI max_elements;   // Stores references to non-static members in member_data order
     sUI num_elements;   // Used for object copies and (de-)serialization
     TKS_ClassDeclMember**elements;
  } all_members;

  sUI num_object_members;
 
  // ---- list of members which have an initializer expression attached ---- 
  StaticList::Node *initialization_members; // TKClassInitializationMember* 
  StaticList::Node *effective_init_members; // TKClassInitializationMember*
  sBool b_static_members_initialized; 
 
  sU16                      current_offset;            // while adding members to class, final value is member data size
  TKS_ScriptClassInstance  *class_template; 
  sBool                     template_initialized;      // 1=class_template is initialized, 0=not
  TKS_ObjectCache           template_member_hashtable; // sUI member signatures
  TKS_ObjectCache           template_method_hashtable; // TKS_ClassDeclMethod*
  TKS_ObjectCache          *functions;                 // class functions ("static methods") or NULL if class has no static methods
  TKS_ObjectCache          *constants;                 // (public) class constants or NULL if class has no constants
  TKS_ObjectCache          *constants_documentation;   // maps constant names to explanation Strings
  Array<sU16>               object_indices;
  sU16                      class_id;
  sUI                       member_data_size;
  Array<TKS_ClassDeclMethod*>constructor_list;
  Array<TKS_ClassDeclMethod*>destructor_list;
  sUI                       num_ctors;
  sUI                       num_dtors;
  sU8                       ancestor_table[TKS_MAX_CLASSDECLS]; // 1 if class <idx> is a parent class of this class
  sU16                      class_relocation_table[TKS_MAX_CLASSDECLS];
  sBool                     is_declared; 
  sBool                     b_this_mutex;  // 1=some methods are synchronized on "this"
  sBool                     b_abstract;    // 1=class contains at least one abstract method => cannot instantiate, only create pointer vars
  sBool                     b_args_resolved;  // 1=fun/method args have been resolved
  sBool                     b_init_functions_done;  // 1=initializeFunctions() called
  sBool                     b_done;        // 1=class has been fully processed in TKS_Compiler::compileCurrentChapter()

  TKS_CachedScript         *module; 
  TKS_Namespace            *nspace; // NULL = global namespace
 
  sUI                       src_loc; // source occurence of first reference to this class 

  struct __callback { 
     // The fxn are resolved during optimize() call
     PTN_Function *fun_yacOperator; 
     PTN_Function *fun_yacToString; 
     PTN_Function *fun_yacOperatorAssign; 
  } callback; 
    

 public:
                        TKS_ClassDecl     (void);
                       ~TKS_ClassDecl     ();

    void               init               (void); 
    void               clean              (void);
    void               optimize           (void);
    sBool              isUsed             (void) const;
 
    // ---- i n h e r i t a n c e
    void               resetAncestorAndRelocationTables (void);
    TKS_ClassDecl     *findAncestor       (YAC_String *_name) const;
    TKS_ClassDecl     *findAncestorDep    (TKS_ClassDecl *_cd) const;
    sU16               getNumDependencies (TKS_ClassDecl *_cd);
    sBool              addAncestor        (TKS_ClassDecl *_ancestor); 
    sBool              hasBaseClass       (TKS_ClassDecl *_cd) const;
 
    // ---- m e m b e r s 
    void                 addInt             (YAC_String *_name, sBool _array, sU8 _tag, sU8 _tagc, sUI _prot_flags, sUI _static, sBool _bRefOnly); 
    void                 addFloat           (YAC_String *_name, sBool _array, sU8 _tag, sU8 _tagc, sUI _prot_flags, sUI _static, sBool _bRefOnly); 
    void                 addString          (YAC_String *_name, sBool _array, sU8 _tag, sU8 _tagc, sUI _prot_flags, sUI _static, sBool _bRefOnly); 
    void                 addPointer         (YAC_String *_name, sBool _array, sU8 _tag, sU8 _tagc, sUI _prot_flags, sUI _static, sBool _bRefOnly); 
    void                 addObject          (TKS_ClassTemplate *_tmp, YAC_String *_name, sBool _array, sU8 _tag, sU8 _tagc, sUI _prot_flags, sUI _static, sBool _bRefOnly); 
    void                 setLastArraySize   (sSI); 
    void                 addClass           (TKS_ClassDecl *_decl, YAC_String *_name, sBool _array, sU8 _tag, sU8 _tagc, sUI _prot_flags, sUI _static, sBool _bRefOnly); 
    TKS_ClassDeclMember *addScalarMember    (YAC_String *_name, sBool _array, sU8 _tag, sU8 _tagc, sUI _prot_flags, sUI _static); 
    TKS_ClassDeclMember *addADTMember       (YAC_String *_name, sBool _array, sU8 _tag, sU8 _tagc, sUI _prot_flags, sUI _static); 
    TKS_ClassDeclMember *findMemberSuper    (YAC_String *_name, sUI _static) const; // find in list 
    TKS_CachedObject    *getMember          (YAC_String *_name) const; // find in hashtable (non-static) 
    TKS_CachedObject    *getStaticMember    (YAC_String *_name) const; // find static member instance 
    void                 deleteStaticMembers(void); 
 
    // ---- i n i t i a l i z e r s 
    void addInitializationMember           (TKS_ClassDeclMember*, sBool _deref); 
    void addAncestorInitializationMembers  (TKS_ClassDecl *_target); 
    void addInitializationMembers          (TKS_ClassDecl *_target); 
    void freeInitializationMembers         (void); 
    void freeInitializationMembers2        (StaticList::Node**); 
    void callRemoveStaticInitializers      (PTN_Env *_env); // determine/init static member values 
    void callRemoveAncestorStaticInitializers(PTN_Env *_env); 
    void initializeStaticMembers           (void); // called by callRemoveStaticInitializers() 
 
    // ---- m e t h o d s
    PTN_Function        *addMethod        (YAC_String *_name, 
                                           sUI         _prot_flags, 
                                           sBool       _bThisMutex, 
                                           sBool       _bAnonymousMutex,
                                           YAC_String *_nextMutexName, 
                                           sBool       _bAbstract,
                                           sBool       _bVirtual,
                                           sUI         _srcLoc
                                           ); 
    TKS_ClassDeclMethod *findMethod       (YAC_String *_name) const;
    TKS_ClassDeclMethod *findMethodSuper  (YAC_String *_name) const;
    TKS_ClassDeclMethod *getMethod        (YAC_String *_name) const; // find in hashtable 
    sBool                resolveMethodArguments(void); 
    PTN_Function *       resolveCallback  (const char *_name) const;

    // ---- f u n c t i o n s 
    PTN_Function      *addFunction              (YAC_String *_name, 
                                                 sUI         _prot_flags, 
                                                 sBool       _bAnonymousMutex, 
                                                 YAC_String *_nextMutexName, 
                                                 sUI         _srcLoc
                                                 ); 
    PTN_Function      *findFunction             (YAC_String *_name) const; 
    PTN_Function      *findFunctionSuper        (YAC_String *_name) const; 
    sBool              resolveFunctionArguments (void); 
    sBool              initializeFunctions      (void); 
    void               freeFunctions            (void);

    // ---- c o n s t a n t s
    TKS_ObjectCache   *getCreateConstants (void);
    TKS_CachedObject  *addIntConstant     (YAC_String *_name);
    TKS_CachedObject  *addFloatConstant   (YAC_String *_name);
    TKS_CachedObject  *addObjectConstant  (YAC_String *_name);
    TKS_CachedObject  *addStringConstant  (YAC_String *_name);
    TKS_CachedObject  *findConstant       (YAC_String *_name) const;
    TKS_CachedObject  *findConstantSuper  (YAC_String *_name) const;
    sBool              setConstantDocumentation (YAC_String *_constantName, YAC_String *_doc);
 
    // ---- v i s i b i l i t y
    sBool              allowMethodCall   (YAC_String  *           _member, 
                                          const TKS_CachedScript *_caller_mod, 
                                          const TKS_ClassDecl *   _caller_class,
                                          TKS_Namespace *         _caller_nspace
                                          ) const;
    sBool              allowMemberAccess (YAC_String  *           _method,
                                          const TKS_CachedScript *_caller_mod,
                                          const TKS_ClassDecl *   _caller_class,
                                          TKS_Namespace *         _caller_nspace
                                          ) const;
    sBool              allowFunctionCall (PTN_Function *          _function, 
                                          const TKS_CachedScript *_caller_mod, 
                                          const TKS_ClassDecl *   _caller_class,
                                          TKS_Namespace *         _caller_nspace
                                          ) const;

    // ---- i n s t a n c i a t i o n 
    void  addClassDependency   (TKS_ClassDecl *_dep); // add unique dependency 
    sBool createClassTemplate       (void);
    sBool addMembersToTemplate      (yacmem**_cmd, sUI *_off, TKS_ClassDeclMember***_allMembers, Array<sU16>*_object_indices, TKS_ObjectCache *_memberhash, sU16 _class_id, sBool *_ancestorTagTable);
    sBool addMethodsToTemplate      (TKS_ObjectCache *_methodhash, sU16 _rclid, sBool *_ancestorTagTable);
    void  addConstructorsToTemplate (Array<TKS_ClassDeclMethod*>*, sBool *_ancestorTagTable);
    void  addDestructorsToTemplate  (Array<TKS_ClassDeclMethod*>*, sBool *_ancestorTagTable);
    void  calcMemberDataSize        (sUI *_mds, sU8 *_mdstable, sU16 *_crt, sUI *_totalNumMembers, sUI *_totalNumObjectIndices, sUI *_totalNumMethods, sUI *_totalNumCtors, sUI *_totalNumDtors);

};


#endif // TKS_CLASSDECL_H__
