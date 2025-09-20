/// TKS_FormalArgument.h 
/// 
/// (c) 2001-2020 Bastian Spiegel <bs@tkscript.de> 
///     - distributed under terms of the GNU general public license (GPL). 
/// 
 
#ifndef __TKS_FORMALARGUMENT_H__
#define __TKS_FORMALARGUMENT_H__
 

class PTN_Function; 
class TKS_ScriptConstraint; 
 
enum __tksconstrainttypes { 
   TKS_CONSTRAINTTYPE_NONE     = 0,  // no constraint applied 
   TKS_CONSTRAINTTYPE_EXPR     = 1,  // use assertion expression 
   TKS_CONSTRAINTTYPE_NOTNULL  = 2   // use instanceof test  
}; 
 
// ---- a single node in a formal argument (single linked) list 
class TKS_FormalArgument : public YAC_TypeS { 
public: 
    YAC_String                name; 
    YAC_String                class_name; // for relaxed crossreferences 
    sBool                     is_pointerarray;  // MyClass *myarg[], String[]*myarg[]
    sBool                     is_local; 
    sBool                     is_array; 
    TKS_FreezedNamespaceArray freezed_namespaces; // Namespace context if ambigious
    TKS_Namespace *           forced_nspace; // Search only this namespace if != NULL
    TKS_CachedObject        * var; 
    TKS_FormalArgument      * next; 
    TKS_ScriptConstraint    * script_constraint; 
    sUI                       constraint_type; // see TKS_CONSTRAINTTYPE_xxx 

    TKS_ScriptClassInstance *cached_meta_class; // meta class template object
    YAC_Object              *cached_cpp_class; // class template object
 
    TKS_FormalArgument(void); 
    TKS_FormalArgument(YAC_String *_name, sSI _type, YAC_String *_class_name,  
                       sBool _bPointerArray, sBool _array, sBool _local
                       ); 
 
    ~TKS_FormalArgument(); 
 
    sBool resolveClassType(void);
    TKS_CachedObject *createVariable (TKS_ObjectCache *_vc, PTN_Function *_cfun); // resolve class_names, convert to real variable 
}; 


#endif // __TKS_FORMALARGUMENT_H__
