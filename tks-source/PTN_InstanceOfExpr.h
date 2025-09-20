/// PTN_InstanceOfExpr.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_INSTANCEOFEXPR_H__
#define __PTN_INSTANCEOFEXPR_H__


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


#endif // __PTN_INSTANCEOFEXPR_H__
