/// PTN_ClassMemberIncDecExpr.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef PTN_CLASSMEMBERINCDECEXPR_H__
#define PTN_CLASSMEMBERINCDECEXPR_H__


class PTN_ClassMemberIncDecExpr : public PTN_ClassMemberExpr {
public:
   sBool post;
   sBool dec;
   sBool b_indirect;

public:
   PTN_ClassMemberIncDecExpr(TKS_CachedObject *_co, TKS_CachedObject *_cc_co, sBool _post, sBool _dec);
   ~PTN_ClassMemberIncDecExpr();

   void initIndirect(void) { b_indirect = 1; }

   void      eval        (PTN_Env *_env, YAC_Value *) const;
   Feval     getEval     (void) const;
#ifdef TKS_JIT
   //sU8       compile     (VMCore *);
   sBool     forceHybrid (void);
#endif
};


#endif // PTN_CLASSMEMBERINCDECEXPR_H__
