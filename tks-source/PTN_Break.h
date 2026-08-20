/// PTN_Break.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef PTN_BREAK_H__
#define PTN_BREAK_H__


class PTN_Break : public PTN_Statement {
public:
   PTN_BreakableStatement *parent_st;

public:
	PTN_Break(void);
	~PTN_Break(void);

	void    eval      (PTN_Env *_env) const;
	Fevalst getEvalSt (void) const;

#ifdef TKS_JIT
  sU8     compile        (VMCore *);
  sBool   forceHybrid    (void);
#endif // TKS_JIT
};


#endif // __PTN_BREAK_H__
