/// PTN_IndexedVarExpr.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#ifndef __PTN_INDEXEDVAREXPR_H__
#define __PTN_INDEXEDVAREXPR_H__


class PTN_IndexedVarExpr : public PTN_Expr {
public:
   TKS_CachedObject *cached_object; // ro refcerence to variable
   PTN_Expr         *index_expr;    // rw reference to index expression 
   
   Feval             index_expr_opt;
   
public:
   PTN_IndexedVarExpr  (void);
   PTN_IndexedVarExpr  (TKS_CachedObject *_var, PTN_Expr *_expr);
   ~PTN_IndexedVarExpr ();
   
   void      eval               (PTN_Env *_env, YAC_Value *) const; 
   Feval     getEval            (void) const;
   sBool     semanticCheck      (void); // check whether node is ready to be evaluated
   void      optimize           (void); 
   sBool     resolveXRef        (void); 
#ifdef TKS_JIT
   sU8       compile            (VMCore *); 
   sBool     forceHybrid        (void); 
#endif
};


#endif // __PTN_INDEXEDVAREXPR_H__
