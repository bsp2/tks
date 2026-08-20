/// PTN_ConditionalExpr.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef PTN_CONDITIONALEXPR_H__
#define PTN_CONDITIONALEXPR_H__


class PTN_ConditionalExpr : public PTN_Expr {
public:
   PTN_Expr *cond_expr;
   PTN_Expr *vala_expr;
   PTN_Expr *valb_expr;
   Feval     cond_expr_opt;
   Feval     vala_expr_opt;
   Feval     valb_expr_opt;

public:
   PTN_ConditionalExpr  (PTN_Expr *_c, PTN_Expr *_a, PTN_Expr *_b);
   ~PTN_ConditionalExpr ();

   sBool     semanticCheck       (void);
   void      optimize            (void);
   sBool     isConst             (void);
   sBool     resolveXRef         (void);
   void      eval                (PTN_Env *_env, YAC_Value *) const;
   Feval     getEval             (void) const;
};


#endif // PTN_CONDITIONALEXPR_H__
