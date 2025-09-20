/// PTN_DynamicVarPointerAssign.h
///
/// (c) 2001-2006 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_DYNAMICVARPOINTERASSIGN_H__
#define __PTN_DYNAMICVARPOINTERASSIGN_H__


class PTN_DynamicVarPointerAssign : public PTN_PointerAssign {
public: 
   PTN_DynamicVarPointerAssign(TKS_CachedObject *_co, PTN_Expr *_expr); 

   void    eval          (PTN_Env *_env) const;
   Fevalst getEvalSt     (void) const;
#ifdef TKS_JIT
   sBool   forceHybrid   (void);
#endif
};


#endif // __PTN_DYNAMICVARPOINTERASSIGN_H__
