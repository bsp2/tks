/// PTN_ExceptionVarExpr.h
///
/// (c) 2006-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_EXCEPTIONVAREXPR_H__
#define __PTN_EXCEPTIONVAREXPR_H__


class PTN_ExceptionVarExpr : public PTN_Expr {
public:
   sBool b_deref;

public:
   PTN_ExceptionVarExpr(sBool _bDeref);
   ~PTN_ExceptionVarExpr();

   void  eval        (PTN_Env *_env, YAC_Value *) const;
   Feval getEval     (void) const;
};


#endif // __PTN_EXCEPTIONVAREXPR_H__
