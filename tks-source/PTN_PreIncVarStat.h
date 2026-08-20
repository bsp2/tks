/// PTN_PreIncVarStat.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef PTN_PREINCVARSTAT_H__
#define PTN_PREINCVARSTAT_H__


class PTN_PreIncVarStat : public PTN_VarStatement {
public:
	void    eval        (PTN_Env *_env) const;
	Fevalst getEvalSt   (void) const;
#ifdef TKS_JIT
	sU8   compile     (VMCore *);
	sBool forceHybrid (void);
#endif
};


#endif // PTN_PREINCVARSTAT_H__
