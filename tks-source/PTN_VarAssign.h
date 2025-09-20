/// PTN_VarAssign.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_VARASSIGN_H__
#define __PTN_VARASSIGN_H__


class PTN_VarAssign : public PTN_Statement {
public:
	TKS_CachedObject *var;  // ro reference to variable
	PTN_Expr         *expr; // rw reference to assigned expression
	Feval             expr_opt;

public:
	PTN_VarAssign(void);
	PTN_VarAssign(TKS_CachedObject *_var, PTN_Expr *_expr);
	~PTN_VarAssign();

	void    eval          (PTN_Env *_env) const;
	Fevalst getEvalSt     (void) const;
	sBool   semanticCheck (void);
	void    optimize      (void);
	sBool   resolveXRef   (void);
#ifdef TKS_JIT
    sU8     compile       (VMCore *);
    sBool   forceHybrid   (void);
#endif
};


#endif // __PTN_VARASSIGN_H__
