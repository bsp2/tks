/// PTN_DerefExpr.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef PTN_DEREFEXPR_H__
#define PTN_DEREFEXPR_H__


class PTN_DerefExpr : public PTN_Expr {
public:
   TKS_CachedObject *var;

public:
   PTN_DerefExpr(TKS_CachedObject *_var);
   ~PTN_DerefExpr();

   sBool       semanticCheck (void);
   void        eval          (PTN_Env *_env, YAC_Value *) const;
   Feval       getEval       (void) const;
};


#endif // PTN_DEREFEXPR_H__
