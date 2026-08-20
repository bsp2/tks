/// PTN_RGBExpr.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef PTN_RGBEXPR_H__
#define PTN_RGBEXPR_H__


class PTN_RGBExpr : public PTN_Expr {
public:
    PTN_Expr *rexpr;
    PTN_Expr *gexpr;
    PTN_Expr *bexpr;
    Feval     rexpr_opt;
    Feval     gexpr_opt;
    Feval     bexpr_opt;

public:
                PTN_RGBExpr    (PTN_Expr *_rexpr, PTN_Expr *_gexpr, PTN_Expr *_bexpr);
                ~PTN_RGBExpr   ();

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


#endif // PTN_RGBEXPR_H__
