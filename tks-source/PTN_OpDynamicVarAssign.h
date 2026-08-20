/// PTN_OpDynamicVarAssign.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef PTN_OPDYNAMICVARASSIGN_H__
#define PTN_OPDYNAMICVARASSIGN_H__


class PTN_OpDynamicVarAssign : public PTN_OpVarAssign {
public:
   PTN_OpDynamicVarAssign(TKS_CachedObject *_co, PTN_Expr *_expr, sU8 _op);

   void    eval          (PTN_Env *_env) const;
   Fevalst getEvalSt     (void) const;

#ifdef TKS_JIT
   sBool   forceHybrid   (void);
#endif
};


#endif // PTN_OPDYNAMICVARASSIGN_H__
