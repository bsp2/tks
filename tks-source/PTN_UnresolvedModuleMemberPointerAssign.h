/// PTN_UnresolvedModuleMemberPointerAssign.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef PTN_UNRESOLVEDMODULEMEMBERPOINTERASSIGN_H__
#define PTN_UNRESOLVEDMODULEMEMBERPOINTERASSIGN_H__


class PTN_UnresolvedModuleMemberPointerAssign : public PTN_Statement {
public:
    YAC_String                module_name;
    YAC_String                module_member_name;
    PTN_Expr         *        expr;
    TKS_CachedObject *        resolved_var;
    Feval                     expr_opt;
    TKS_FreezedNamespaceArray nspace_array;
    sBool                     b_default_module;

public:
    PTN_UnresolvedModuleMemberPointerAssign(void);
    PTN_UnresolvedModuleMemberPointerAssign(YAC_String *_module, sBool _bDefaultModule,
                                            YAC_String *_member, PTN_Expr *_expr
                                            );
    ~PTN_UnresolvedModuleMemberPointerAssign();

    sBool   semanticCheck (void);
    void    optimize      (void);
    sBool   resolveXRef   (void);
    void    eval          (PTN_Env *_env) const;
    Fevalst getEvalSt     (void) const;
};


#endif // PTN_UNRESOLVEDMODULEMEMBERPOINTERASSIGN_H__
