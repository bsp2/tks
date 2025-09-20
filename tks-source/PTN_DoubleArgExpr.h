/// PTN_DoubleArgExpr.h
///
/// (c) 2001-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL)
///
///

#ifndef PTN_DOUBLEARGEXPR_H__
#define PTN_DOUBLEARGEXPR_H__


class PTN_DoubleArgExpr : public PTN_Expr {
public:
  PTN_Expr *expr1;
  PTN_Expr *expr2;
  sU8       expr_operator;  // YAC_OP_xxx
  Feval     expr1_opt;
  Feval     expr2_opt;

public:
  PTN_DoubleArgExpr        (PTN_Expr *_expr1, sU8 _operator, PTN_Expr *_expr2);
  PTN_DoubleArgExpr        (void);
  ~PTN_DoubleArgExpr       ();

         void      eval          (PTN_Env *_env, YAC_Value *) const;
         Feval     getEval       (void) const;
         sUI       getType       (void) const;
         sBool     isConst       (void);
  static void      EvalOp        (PTN_Env *_env, YAC_Value *ret, YAC_Value *_r1, YAC_Value *_r2, sU8 _op, const PTN_Node *_nodeOrNull);
  static void      EvalBEQ       (YAC_Value &r1, YAC_Value &r2, YAC_Value *_r);
  static void      EvalBNE       (YAC_Value &r1, YAC_Value &r2, YAC_Value *_r);
         sBool     semanticCheck (void);
         sBool     optimizeBEQ   (void);  // @(a)==@(b)
         void      optimize      (void);
         sBool     resolveXRef   (void);
#ifdef TKS_JIT
         sU8       compile       (VMCore *);
  static sU8       CompileOp     (VMCore *, sU8 r1, sU8 r2, sU8 expr_operator);
         sBool     forceHybrid   (void);
#endif
};


#endif // PTN_DOUBLEARGEXPR_H__
