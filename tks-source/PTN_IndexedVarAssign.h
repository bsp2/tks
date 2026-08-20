/// PTN_IndexedVarAssign.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef PTN_INDEXEDVARASSIGN_H__
#define PTN_INDEXEDVARASSIGN_H__


class PTN_IndexedVarAssign : public PTN_Statement {
public:
   TKS_CachedObject *var;        // ro reference to variable
   PTN_Expr         *index_expr; // rw reference to index expression
   PTN_Expr         *expr;       // rw reference to right hand expression

   Feval             index_expr_opt;
   Feval             expr_opt;

public:
   PTN_IndexedVarAssign  (void);
   PTN_IndexedVarAssign  (TKS_CachedObject *_var, PTN_Expr *_indexexpr, PTN_Expr *_expr);
   ~PTN_IndexedVarAssign ();

   void    eval                 (PTN_Env *_env) const;
   Fevalst getEvalSt            (void) const;
   sBool   semanticCheck        (void);
   void    optimize             (void);
   sBool   resolveXRef          (void);
#ifdef TKS_JIT
   sBool   forceHybrid          (void);
   sU8     compile              (VMCore*);
#endif
};


#endif // PTN_INDEXEDVARASSIGN_H__
