/// PTN_NewClassExpr.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_NEWCLASSEXPR_H__
#define __PTN_NEWCLASSEXPR_H__


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


#endif // __PTN_NEWCLASSEXPR_H__
