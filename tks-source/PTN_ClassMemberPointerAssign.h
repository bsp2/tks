/// PTN_ClassMemberPointerAssign.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef PTN_CLASSMEMBERPOINTERASSIGN_H__
#define PTN_CLASSMEMBERPOINTERASSIGN_H__


class PTN_ClassMemberPointerAssign : public PTN_Statement {
public:
    TKS_CachedObject *var;
    TKS_CachedObject *member;
    sUI               class_id;
    PTN_Expr         *expr;
    Feval             expr_opt;
    sBool             b_indirect;

public:
    PTN_ClassMemberPointerAssign(TKS_CachedObject *_co, TKS_CachedObject *_cc_co, PTN_Expr *_expr);
    ~PTN_ClassMemberPointerAssign();

    void initIndirect(void) { b_indirect = YAC_TRUE; }

    sBool   semanticCheck (void);
    void    optimize      (void);
    sBool   resolveXRef   (void);
    void    eval          (PTN_Env *_env) const;
    Fevalst getEvalSt     (void) const;
};


#endif // PTN_CLASSMEMBERPOINTERASSIGN_H__
