/// PTN_ClassMethodDelegateCall.h
///
/// (c) 2005-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef PTN_CLASSMETHODDELEGATECALL_H__
#define PTN_CLASSMETHODDELEGATECALL_H__


class PTN_ClassMethodDelegateCall : public PTN_CallableExpr {
public:
   PTN_Expr    *name_expr;
   Feval        name_expr_opt;
   PTN_ArgList *arglist;

   TKS_ClassDecl *parent_class;
   sBool          b_static;

public:
   PTN_ClassMethodDelegateCall(TKS_ClassDecl *_parentClass, sBool _bStatic, PTN_Expr *_name_expr, PTN_ArgList *_args);
   ~PTN_ClassMethodDelegateCall();

   sBool semanticCheck    (void);
   void  optimize         (void);
   sBool resolveXRef      (void);
   void  evalCallableExpr (PTN_Env *_env, YAC_Value *_r) const;
   void  eval             (PTN_Env *_env, YAC_Value *_r) const;
   Feval getEval          (void) const;
};


#endif // PTN_CLASSMETHODDELEGATECALL_H__
