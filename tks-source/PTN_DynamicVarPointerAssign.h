/// PTN_DynamicVarPointerAssign.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef PTN_DYNAMICVARPOINTERASSIGN_H__
#define PTN_DYNAMICVARPOINTERASSIGN_H__


class PTN_DynamicVarPointerAssign : public PTN_PointerAssign {
public:
   PTN_DynamicVarPointerAssign(TKS_CachedObject *_co, PTN_Expr *_expr);

   void    eval          (PTN_Env *_env) const;
   Fevalst getEvalSt     (void) const;
#ifdef TKS_JIT
   sBool   forceHybrid   (void);
#endif
};


#endif // PTN_DYNAMICVARPOINTERASSIGN_H__
