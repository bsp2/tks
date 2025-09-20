/// PTN_TripleArgExpr.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_TRIPLEARGEXPR_H__
#define __PTN_TRIPLEARGEXPR_H__


class PTN_TripleArgExpr : public PTN_Expr {
public:
  PTN_Expr *expr1;
  PTN_Expr *expr2;
  PTN_Expr *expr3;
  sU8       expr_operator1;
  sU8       expr_operator2;
  Feval     expr1_opt;
  Feval     expr2_opt;
  Feval     expr3_opt;

public:
            PTN_TripleArgExpr  (void);
            PTN_TripleArgExpr  (PTN_Expr *_expr1, sU8 _op1, PTN_Expr *_expr2, sU8 _op2, PTN_Expr *_expr3);
            ~PTN_TripleArgExpr ();
  
  void      eval              (PTN_Env *_env, YAC_Value *) const;
  Feval     getEval           (void) const;
  sBool     semanticCheck     (void);
  sBool     isConst           (void);
  void      optimize          (void);
  sBool     resolveXRef       (void);
};


#endif // __PTN_TRIPLEARGEXPR_H__
