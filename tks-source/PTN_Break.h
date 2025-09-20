/// PTN_Break.h
///
/// (c) 2001-2014 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_BREAK_H__
#define __PTN_BREAK_H__


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
