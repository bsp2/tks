/// PTN_ClassMemberIndirectObjectExpr.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_CLASSMEMBERINDIRECTOBJECTEXPR_H__
#define __PTN_CLASSMEMBERINDIRECTOBJECTEXPR_H__

 
class PTN_ClassMemberIndirectObjectExpr : public PTN_Expr {
public:
    TKS_CachedObject *member;
    PTN_Expr         *indirect_expr;
    Feval             indirect_expr_opt;

public:
    PTN_ClassMemberIndirectObjectExpr(TKS_CachedObject *_member, PTN_Expr *_indirectexpr);
    ~PTN_ClassMemberIndirectObjectExpr();

    sBool     semanticCheck (void);
    sBool     resolveXRef   (void);
    void      optimize      (void);
    void      eval          (PTN_Env *_env, YAC_Value *) const;
    Feval     getEval       (void) const;
};


#endif // __PTN_CLASSMEMBERINDIRECTOBJECTEXPR_H__
