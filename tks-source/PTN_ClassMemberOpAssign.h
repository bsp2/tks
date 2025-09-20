/// PTN_ClassMemberOpAssign.h
///
/// (c) 2001-2010 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_CLASSMEMBEROPASSIGN_H__
#define __PTN_CLASSMEMBEROPASSIGN_H__


class PTN_ClassMemberOpAssign : public PTN_Statement {
public:
    TKS_CachedObject *var; // NULL if expr result is assigned to member of "this"
    TKS_CachedObject *member;
    sUI               class_id;
    sU8               assign_operator;
    PTN_Expr         *expr;
    Feval             expr_opt;
    sBool             b_indirect;
    
public:
    PTN_ClassMemberOpAssign(TKS_CachedObject *_co, TKS_CachedObject *_cc_co, sU8 _aop, PTN_Expr *_expr);
    ~PTN_ClassMemberOpAssign();

    void initIndirect(void) { b_indirect = 1; }

    sBool   semanticCheck (void);
    void    optimize      (void);
    sBool   resolveXRef   (void);
    void    eval          (PTN_Env *_env) const;
    Fevalst getEvalSt     (void) const;

#ifdef TKS_JIT
    sU8   compile       (VMCore *);
    sBool forceHybrid   (void);
#endif 
};


#endif //__PTN_CLASSMEMBEROPASSIGN_H__

