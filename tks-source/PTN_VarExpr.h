/// PTN_VarExpr.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef PTN_VAREXPR_H__
#define PTN_VAREXPR_H__


class PTN_VarExpr : public PTN_Expr {
public:
   TKS_CachedObject *cached_object; // ro reference to variable

public:
   PTN_VarExpr(void);
   PTN_VarExpr(TKS_CachedObject *_var);
   ~PTN_VarExpr();

   void        eval          (PTN_Env *_env, YAC_Value *) const;
   Feval       getEval       (void) const;
   sBool       semanticCheck (void);
   sUI         getID         (void) const;
   sUI         getType       (void) const;
#ifdef TKS_JIT
   sU8         compile       (VMCore *);
   sBool       forceHybrid   (void);
#endif
};


#endif // PTN_VAREXPR_H__
