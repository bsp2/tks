/// PTN_PreDecVarStat.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_PREDECVARSTAT_H__
#define __PTN_PREDECVARSTAT_H__


class PTN_PreDecVarStat : public PTN_VarStatement {
public:
  void    eval        (PTN_Env *_env) const;
  Fevalst getEvalSt   (void) const;

#ifdef TKS_JIT
  sU8   compile     (VMCore *);
  sBool forceHybrid (void);
#endif
};


#endif // __PTN_PREDECVARSTAT_H__
