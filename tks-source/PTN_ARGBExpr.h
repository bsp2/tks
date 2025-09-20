/// PTN_ARGBExpr.h
///
/// (c) 2001-2013 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#ifndef __PTN_ARGBEXPR_H__
#define __PTN_ARGBEXPR_H__

// (todo) should actually be called Pack4Expr

class PTN_ARGBExpr : public PTN_Expr {
public:
    PTN_Expr *aexpr;
    PTN_Expr *rexpr;
    PTN_Expr *gexpr;
    PTN_Expr *bexpr;
    Feval     aexpr_opt;
    Feval     rexpr_opt;
    Feval     gexpr_opt;
    Feval     bexpr_opt;

    sBool b_rgba_4b; // 1=r,g,b,a byte order in memory (rgba_mem() pseudo builtin fun)

public:
                PTN_ARGBExpr  (PTN_Expr *_aexpr, PTN_Expr *_rexpr, PTN_Expr *_gexpr, PTN_Expr *_bexpr);
                ~PTN_ARGBExpr ();

    sBool       semanticCheck (void);
    sBool       resolveXRef   (void);
    void        optimize      (void);
    void        eval          (PTN_Env *_env, YAC_Value *) const;
    Feval       getEval       (void) const;
    sBool       isConst       (void);
#ifdef TKS_JIT
    sU8         compile       (VMCore *);
    sBool       forceHybrid   (void);
#endif
};


#endif // __PTN_ARGBEXPR_H__
