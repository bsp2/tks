/// PTN_VarAssign.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef PTN_VARASSIGN_H__
#define PTN_VARASSIGN_H__


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


#endif // PTN_VARASSIGN_H__
