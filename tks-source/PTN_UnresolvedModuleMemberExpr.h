/// PTN_UnresolvedModuleMemberExpr.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///
///
#ifndef __PTN_UNRESOLVEDMODULEMEMBEREXPR_H__
#define __PTN_UNRESOLVEDMODULEMEMBEREXPR_H__


class TKS_FunctionObject;

class PTN_UnresolvedModuleMemberExpr : public PTN_Expr {
public:
    YAC_String                module_name;
    YAC_String                module_member_name;
    TKS_CachedScript   *      resolved_module;
    TKS_CachedObject   *      resolved_var;
    TKS_FunctionObject *      resolved_funobject;
    YAC_Value                 const_val;
    TKS_FreezedNamespaceArray nspace_array;
    sBool                     b_default_module;

public:
    PTN_UnresolvedModuleMemberExpr(void);
    PTN_UnresolvedModuleMemberExpr(YAC_String *_modulename, sBool _bDefaultModule, YAC_String *_membername);
    ~PTN_UnresolvedModuleMemberExpr();
    
    sBool       semanticCheck (void);
    void        eval          (PTN_Env *_env, YAC_Value *) const; 
    Feval       getEval       (void) const;
    sBool       resolveXRef   (void);
    sUI         getID         (void) const;
    sBool       isConst       (void);
#ifdef TKS_JIT 
    sU8         compile       (VMCore *_vm); 
    sBool       forceHybrid   (void); 
#endif 
};


#endif // __PTN_UNRESOLVEDMODULEMEMBEREXPR_H__
