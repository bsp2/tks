/// PTN_BuiltinFun.h
///
/// (c) 2001-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///

#ifndef PTN_BUILTINFUN_H__
#define PTN_BUILTINFUN_H__


enum __funselectors {
   TKS_BFUN_NOP = 0,
   TKS_BFUN_SIN,
   TKS_BFUN_COS,
   TKS_BFUN_TAN,
   TKS_BFUN_TANH,
   TKS_BFUN_ASIN,
   TKS_BFUN_ACOS,
   TKS_BFUN_SQRT,
   TKS_BFUN_RAD,
   TKS_BFUN_DEG,
   TKS_BFUN_ABS,
   TKS_BFUN_FRAC,
   TKS_BFUN_TCINT,
   TKS_BFUN_TCFLOAT,
   TKS_BFUN_TCSTR,
   TKS_BFUN_TCOBJECT,
   TKS_BFUN_TCPOINTER,
   TKS_BFUN_ROUND,
   TKS_BFUN_FLOOR,
   TKS_BFUN_CEIL,
   TKS_BFUN_SIGN,
   TKS_BFUN_RND,
   TKS_BFUN_2N,
   TKS_BFUN_TCCHR,
   TKS_BFUN_VALUE,
   TKS_BFUN_TYPEID,
   TKS_BFUN_TYPENAME,
   TKS_BFUN_LOG,
   TKS_BFUN_LOG2,
   TKS_BFUN_EXP,
   TKS_BFUN_ADDRESS,

   TKS_BFUN_PAD
};

class PTN_BuiltinFun : public PTN_Expr {
public:
   PTN_Expr *expr;
   sU8       fun_selector;
   Feval     expr_opt;

public:
   PTN_BuiltinFun  (PTN_Expr *_expr, sU8 _fun_selector);
   ~PTN_BuiltinFun ();

   sUI        getID          (void) const;
   sBool      semanticCheck  (void);
   void       eval           (PTN_Env *_env, YAC_Value *) const;
   Feval      getEval        (void) const;
   sBool      isConst        (void);
   sUI        getType        (void) const;
   sBool      isFunAddress   (void) const;
   void       optimize       (void);
   sBool      resolveXRef    (void);
#ifdef TKS_JIT
   sU8        compile        (VMCore*);
   sBool      forceHybrid    (void);
#endif
};


#endif // PTN_BUILTINFUN_H__

