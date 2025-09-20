/// PTN_ClassMemberOpIndexedVarAssign.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_CLASSMEMBEROPINDEXEDVARASSIGN_H__
#define __PTN_CLASSMEMBEROPINDEXEDVARASSIGN_H__


class PTN_ClassMemberOpIndexedVarAssign : public PTN_Statement {
public:
    TKS_CachedObject *var;
    TKS_CachedObject *member;        // ro reference to member co
    PTN_Expr         *index_expr;    // rw reference to index expression
    PTN_Expr         *expr;          // rw reference to right hand expression
    sU8               expr_operator;
    sUI               class_id;
    Feval             index_expr_opt;
    Feval             expr_opt;

public:
    PTN_ClassMemberOpIndexedVarAssign(TKS_CachedObject *_var,
                                      TKS_CachedObject *_member, 
                                      PTN_Expr *_indexexpr, 
                                      PTN_Expr *_expr, 
                                      sU8 _op
                                      );
    ~PTN_ClassMemberOpIndexedVarAssign();

    void    eval          (PTN_Env *_env) const;
    Fevalst getEvalSt     (void) const;
    sBool   semanticCheck (void);
    void    optimize      (void);
    sBool   resolveXRef   (void);
};


#endif // __PTN_CLASSMEMBEROPINDEXEDVARASSIGN_H__
