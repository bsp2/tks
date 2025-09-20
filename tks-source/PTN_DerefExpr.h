/// PTN_DerefExpr.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_DEREFEXPR_H__
#define __PTN_DEREFEXPR_H__


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


#endif
