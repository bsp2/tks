/// PTN_UnresolvedModuleMemberPointerAssign.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_UNRESOLVEDMODULEMEMBERPOINTERASSIGN_H__
#define __PTN_UNRESOLVEDMODULEMEMBERPOINTERASSIGN_H__


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


#endif // __PTN_UNRESOLVEDMODULEMEMBERPOINTERASSIGN_H__
