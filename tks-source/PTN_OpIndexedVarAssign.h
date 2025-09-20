/// PTN_OpIndexedVarAssign.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_OPINDEXEDVARASSIGN_H__
#define __PTN_OPINDEXEDVARASSIGN_H__


class PTN_OpIndexedVarAssign : public PTN_Statement {
public:
   TKS_CachedObject *var;           // ro reference to variable
   PTN_Expr         *index_expr;    // rw reference to index expression
   PTN_Expr         *expr;          // rw reference to right hand expression
   sU8               expr_operator; 
   
   Feval             index_expr_opt; 
   Feval             expr_opt; 
   
public:
   PTN_OpIndexedVarAssign  (void);
   PTN_OpIndexedVarAssign  (TKS_CachedObject *_var, PTN_Expr *_indexexpr, PTN_Expr *_expr, sU8 _op);
   ~PTN_OpIndexedVarAssign ();
   
   void    eval           (PTN_Env *_env) const;
   Fevalst getEvalSt      (void) const; 
   sBool   semanticCheck  (void);
   void    optimize       (void);
   sBool   resolveXRef    (void);
};


#endif // __PTN_OPINDEXEDVARASSIGN_H__
