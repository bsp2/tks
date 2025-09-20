/// PTN_ThisExpr.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_THISEXPR_H__
#define __PTN_THISEXPR_H__


class PTN_ThisExpr : public PTN_Expr {
public:
   void  eval        (PTN_Env *_env, YAC_Value *) const;
   Feval getEval     (void) const;
};


#endif // __PTN_THISEXPR_H__
