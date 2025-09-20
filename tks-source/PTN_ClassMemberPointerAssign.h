/// PTN_ClassMemberPointerAssign.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_CLASSMEMBERPOINTERASSIGN_H__
#define __PTN_CLASSMEMBERPOINTERASSIGN_H__


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

    void initIndirect(void) { b_indirect = 1; }

    sBool   semanticCheck (void);
    void    optimize      (void);
    sBool   resolveXRef   (void);
    void    eval          (PTN_Env *_env) const;
    Fevalst getEvalSt     (void) const;
};


#endif // __PTN_CLASSMEMBERPOINTERASSIGN_H__
