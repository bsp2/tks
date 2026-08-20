/// PTN_InstanceOfExpr.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef PTN_INSTANCEOFEXPR_H__
#define PTN_INSTANCEOFEXPR_H__


class PTN_InstanceOfExpr : public PTN_Expr {
public:
   PTN_Expr *this_expr;
   PTN_Expr *object_expr;

   Feval     this_expr_opt;
   Feval     object_expr_opt;

public:
   PTN_InstanceOfExpr(PTN_Expr *_this_expr, PTN_Expr *_object_expr);
   ~PTN_InstanceOfExpr();

   void   eval           (PTN_Env *_env, YAC_Value *) const;
   Feval  getEval        (void) const;
   sBool  semanticCheck  (void);
   sBool  isConst        (void);
   void   optimize       (void);
   sBool  resolveXRef    (void);
};


#endif // PTN_INSTANCEOFEXPR_H__
