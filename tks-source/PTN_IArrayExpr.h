/// PTN_IArrayExpr.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_IARRAYEXPR_H__
#define __PTN_IARRAYEXPR_H__


class PTN_IArrayExpr : public PTN_CallableExpr {
public:
   PTN_Expr  *obj_expr;
   PTN_Expr  *index_expr;
   YAC_Value  iarray_value;
   Feval      obj_expr_opt;
   Feval      index_expr_opt;
   
public:
   PTN_IArrayExpr(PTN_Expr *_objexpr, PTN_Expr *_idexpr);
   ~PTN_IArrayExpr();
   
   sBool semanticCheck     (void);
   void  evalCallableExpr  (PTN_Env *_env, YAC_Value *_r) const;
   void  eval              (PTN_Env *_env, YAC_Value *_r) const;
   Feval getEval           (void) const;
   sBool resolveXRef       (void);
   void  optimize          (void);
};


#endif // __PTN_IARRAYEXPR_H__
