/// PTN_SingleArgExpr.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef PTN_SINGLEARGEXPR_H__
#define PTN_SINGLEARGEXPR_H__


class PTN_SingleArgExpr : public PTN_Expr {
public:
  PTN_Expr *expr1;
  sU8       unary_operator;
  Feval     expr1_opt;

public:
            PTN_SingleArgExpr  (PTN_Expr *, sU8 _operator=YAC_OP_ASSIGN);
            PTN_SingleArgExpr  (void);
            ~PTN_SingleArgExpr ();

  void      eval              (PTN_Env *_env, YAC_Value *) const;
  Feval     getEval           (void) const;
  sBool     semanticCheck     (void);
  sBool     isConst           (void);
  void      optimize          (void);
  sBool     resolveXRef       (void);
#ifdef TKS_JIT
  sU8       compile           (VMCore *);
  sBool     forceHybrid       (void);
#endif

};


#endif // PTN_SINGLEARGEXPR_H__
