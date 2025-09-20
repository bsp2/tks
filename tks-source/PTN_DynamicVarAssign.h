/// PTN_DynamicVarAssign.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_DYNAMICVARASSIGN_H__
#define __PTN_DYNAMICVARASSIGN_H__


class PTN_DynamicVarAssign : public PTN_VarAssign {
public:
	PTN_DynamicVarAssign(TKS_CachedObject *_var, PTN_Expr *_expr);

	void    eval          (PTN_Env *_env) const;
	Fevalst getEvalSt     (void) const;
#ifdef TKS_JIT
   sBool   forceHybrid   (void);
#endif
};


#endif
