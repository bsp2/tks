/// PTN_PreDecVarExpr.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef PTN_PREDECVAREXPR_H__
#define PTN_PREDECVAREXPR_H__


class PTN_PreDecVarExpr : public PTN_VarExpr {
public:
  void      eval    (PTN_Env *_env, YAC_Value *) const;
  Feval     getEval (void) const;

#ifdef TKS_JIT
  sU8       compile (VMCore *);
  sBool     forceHybrid(void);
#endif // TKS_JIT
};


#endif // PTN_PREDECVAREXPR_H__
