/// PTN_NewClassExpr.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef PTN_NEWCLASSEXPR_H__
#define PTN_NEWCLASSEXPR_H__


class PTN_NewClassExpr : public PTN_Expr {
public:
   TKS_ClassDecl *class_decl; // read only reference to class declaration

public:
   PTN_NewClassExpr(void);
   PTN_NewClassExpr(TKS_ClassDecl *);
   ~PTN_NewClassExpr();

   sBool  semanticCheck (void);
   void   eval          (PTN_Env *_env, YAC_Value *) const;
   Feval  getEval       (void) const;
};


#endif // PTN_NEWCLASSEXPR_H__
