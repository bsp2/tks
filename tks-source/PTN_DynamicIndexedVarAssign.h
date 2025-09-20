/// PTN_DynamicIndexedVarAssign.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_DYNAMICINDEXEDVARASSIGN_H__
#define __PTN_DYNAMICINDEXEDVARASSIGN_H__

 
class PTN_DynamicIndexedVarAssign : public PTN_IndexedVarAssign {
public:
  PTN_DynamicIndexedVarAssign(TKS_CachedObject *_var, PTN_Expr *_indexexpr, PTN_Expr *_expr);

  void    optimize  (void);
  void    eval      (PTN_Env *_env) const;
  Fevalst getEvalSt (void) const;
};


#endif // __PTN_DYNAMICINDEXEDVARASSIGN_H__
