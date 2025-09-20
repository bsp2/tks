/// PTN_UnresolvedOpModuleMemberAssign.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_UNRESOLVEDOPMODULEMEMBERASSIGN_H__
#define __PTN_UNRESOLVEDOPMODULEMEMBERASSIGN_H__


class PTN_UnresolvedOpModuleMemberAssign : public PTN_Statement {
public:
    YAC_String                module_name;
    YAC_String                module_member_name;
    sU8                       assign_operator;
    PTN_Expr         *        expr;
    TKS_CachedObject *        resolved_var;
    Feval                     expr_opt;
    TKS_FreezedNamespaceArray nspace_array;
    sBool                     b_default_module;

public:
    PTN_UnresolvedOpModuleMemberAssign(void);
    PTN_UnresolvedOpModuleMemberAssign(YAC_String *_module, sBool _bDefaultModule, YAC_String *_member, sU8 _assignop, PTN_Expr *_expr);
    ~PTN_UnresolvedOpModuleMemberAssign();

    void    eval          (PTN_Env *_env) const;
    sBool   semanticCheck (void);
    void    optimize      (void);
    sBool   resolveXRef   (void);
    Fevalst getEvalSt     (void) const;
};


#endif // __PTN_UNRESOLVEDOPMODULEMEMBERASSIGN_H__
