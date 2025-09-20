/// PTN_PreDecVarExpr.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_PREDECVAREXPR_H__
#define __PTN_PREDECVAREXPR_H__


class PTN_PreDecVarExpr : public PTN_VarExpr {
public:
  void      eval    (PTN_Env *_env, YAC_Value *) const;
  Feval     getEval (void) const;

#ifdef TKS_JIT
  sU8       compile (VMCore *);
  sBool     forceHybrid(void);
#endif
};


#endif // __PTN_PREDECVAREXPR_H__
