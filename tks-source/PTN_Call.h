/// PTN_Call.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_CALL_H__
#define __PTN_CALL_H__


class PTN_Call : public PTN_Statement {
public:
  PTN_CallableExpr *expr;

  Feval            expr_opt;

public:
  PTN_Call(void);
  PTN_Call(PTN_CallableExpr *_expr);
  ~PTN_Call();

  void                 eval          (PTN_Env *_env) const;
  Fevalst              getEvalSt     (void) const;
  const PTN_Statement *getEvalStArg  (void) const;
  sBool                semanticCheck (void);
  void                 optimize      (void);
  sBool                resolveXRef   (void);

#ifdef TKS_JIT
  sU8     compile       (VMCore *);
  sBool   forceHybrid   (void);
#endif
};


#endif // __PTN_CALL_H__
