/// PTN_ClassMethodDelegateAssign.h
///
/// (c) 2005-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///
///

#ifndef PTN_CLASSMETHODDELEGATEASSIGN_H__
#define PTN_CLASSMETHODDELEGATEASSIGN_H__


class PTN_ClassMethodDelegateAssign : public PTN_Statement {
public:
   PTN_Expr            *obj_expr;
   Feval                obj_expr_opt;
   PTN_Expr            *name_expr;
   Feval                name_expr_opt;
   PTN_Function        *fun;
   PTN_ArgList         *arglist;

public:
   PTN_ClassMethodDelegateAssign(PTN_Expr *_obj_expr, PTN_Expr *_name_expr, PTN_Function *_fun);
   ~PTN_ClassMethodDelegateAssign();

   sBool   semanticCheck (void);
   void    optimize      (void);
   sBool   resolveXRef   (void);
   void    eval          (PTN_Env *_env) const;
   Fevalst getEvalSt     (void) const;
};


#endif // PTN_CLASSMETHODDELEGATEASSIGN_H__
