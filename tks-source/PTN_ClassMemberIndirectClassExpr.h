/// PTN_ClassMemberIndirectClassExpr.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_CLASSMEMBERINDIRECTCLASSEXPR_H__
#define __PTN_CLASSMEMBERINDIRECTCLASSEXPR_H__

 
class PTN_ClassMemberIndirectClassExpr : public PTN_Expr {
public:
    TKS_CachedObject *member; 
    PTN_Expr         *indirect_expr;
    Feval             indirect_expr_opt;

public:
    PTN_ClassMemberIndirectClassExpr(TKS_CachedObject *_member, PTN_Expr *_indirectexpr);
    ~PTN_ClassMemberIndirectClassExpr();

    sBool     semanticCheck (void);
    void      optimize      (void);
    void      eval          (PTN_Env *_env, YAC_Value *) const;
    sBool     resolveXRef   (void);
    Feval     getEval       (void) const;
};


#endif // __PTN_CLASSMEMBERINDIRECTCLASSEXPR_H__
