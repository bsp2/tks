/// PTN_UnresolvedModuleMemberIncDecExpr.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_UNRESOLVEDMODULEMEMBERINCDECEXPR_H__
#define __PTN_UNRESOLVEDMODULEMEMBERINCDECEXPR_H__


class PTN_UnresolvedModuleMemberIncDecExpr : public PTN_Expr {
public:
    YAC_String                module_name;
    YAC_String                module_member_name;
    sBool                     post;
    sBool                     dec;
    TKS_CachedObject *        resolved_var;
    TKS_FreezedNamespaceArray nspace_array;
    sBool                     b_default_module;

public:
    PTN_UnresolvedModuleMemberIncDecExpr(void);
    PTN_UnresolvedModuleMemberIncDecExpr(
       YAC_String *_modulename, sBool _bDefaultModule, YAC_String *_classmembername, 
       sBool _post, sBool _dec
       );
    ~PTN_UnresolvedModuleMemberIncDecExpr();

    sBool     resolveXRef (void);
    void      eval        (PTN_Env *_env, YAC_Value *) const;
    Feval     getEval     (void) const;
};


#endif // __PTN_UNRESOLVEDMODULEMEMBERINCDECEXPR_H__
