/// PTN_ThisExpr.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef PTN_THISEXPR_H__
#define PTN_THISEXPR_H__


class PTN_ThisExpr : public PTN_Expr {
public:
   void  eval        (PTN_Env *_env, YAC_Value *) const;
   Feval getEval     (void) const;
};


#endif // PTN_THISEXPR_H__
