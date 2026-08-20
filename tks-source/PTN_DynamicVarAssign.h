/// PTN_DynamicVarAssign.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef PTN_DYNAMICVARASSIGN_H__
#define PTN_DYNAMICVARASSIGN_H__


class PTN_DynamicVarAssign : public PTN_VarAssign {
public:
	PTN_DynamicVarAssign(TKS_CachedObject *_var, PTN_Expr *_expr);

	void    eval          (PTN_Env *_env) const;
	Fevalst getEvalSt     (void) const;
#ifdef TKS_JIT
   sBool   forceHybrid   (void);
#endif
};


#endif // PTN_DYNAMICVARASSIGN_H__
