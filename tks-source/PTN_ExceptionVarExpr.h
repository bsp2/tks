/// PTN_ExceptionVarExpr.h
///
/// (c) 2006-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef PTN_EXCEPTIONVAREXPR_H__
#define PTN_EXCEPTIONVAREXPR_H__


class PTN_ExceptionVarExpr : public PTN_Expr {
public:
   sBool b_deref;

public:
   PTN_ExceptionVarExpr(sBool _bDeref);
   ~PTN_ExceptionVarExpr();

   void  eval        (PTN_Env *_env, YAC_Value *) const;
   Feval getEval     (void) const;
};


#endif // PTN_EXCEPTIONVAREXPR_H__
