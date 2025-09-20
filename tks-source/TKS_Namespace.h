/// TKS_Namespace.h
///
/// (c) 2008-2025 by Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///
///
#ifndef TKS_NAMESPACE_H__
#define TKS_NAMESPACE_H__


class TKS_Namespace;
class PTN_Function;

typedef TKS_Namespace**TKS_FreezedNamespaceArray; // NULL terminated list of namespaces. Used to save the list of visible namespaces
                                                  // for later evaluation (e.g. when resolving an ambiguous class name)


class TKS_Namespace : public YAC_Object {
public:
   YAC_String      name;
   TKS_ObjectCache class_decl_hash; // maps class name to TKS_ClassDecl reference
   TKS_ObjectCache global_vars; // Holds global namespace variables
   TKS_ObjectCache function_hash;   // maps function name to PTN_Function reference

public:

   TKS_Namespace(void); // calls init()
   ~TKS_Namespace();

   void                      init            (void);
   void                      free            (void);
   void                      freeVars        (void);
   void                      freeTypes       (void);
   TKS_CachedObject *        addClassDecl    (TKS_ClassDecl *_cd);    // add by reference to TKS_Compiler::class_decls
   void                      removeClassDecl (TKS_ClassDecl *_cd);
   TKS_ClassDecl *           findClassDecl   (YAC_String *_name) const;
   TKS_CachedObject *        addFunction     (PTN_Function *_fun);
   PTN_Function *            findFunction    (YAC_String *_name) const;
   TKS_FreezedNamespaceArray freeze          (void); // Convert this pointer to NULL terminated array
   TKS_CachedObject *        findVar         (YAC_String *_name) const;
};


#endif // TKS_NAMESPACE_H__
