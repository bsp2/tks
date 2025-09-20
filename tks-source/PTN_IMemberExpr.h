/// PTN_IMemberExpr.h
///
/// (c) 2001-2020 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_IMEMBEREXPR_H__
#define __PTN_IMEMBEREXPR_H__


class PTN_IMemberExpr : public PTN_CallableExpr {
public:
   PTN_Expr         *obj_expr; 
   YAC_String        member_name;
   YAC_String        get_method_name;
   Feval             obj_expr_opt;

public:
   PTN_IMemberExpr(PTN_Expr *_objexpr, YAC_String *_membername);
   ~PTN_IMemberExpr();
   
   sBool semanticCheck    (void);
   void  evalCallableExpr (PTN_Env *_env, YAC_Value *_r) const; // Return (<obj_expr>).member_name
   void  eval             (PTN_Env *_env, YAC_Value *_r) const;
   Feval getEval          (void) const;
   sBool resolveXRef      (void);
   void  optimize         (void);
};


#endif // __PTN_IMEMBEREXPR_H__

