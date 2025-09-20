/// PTN_ClassMemberIndexedVarExpr.h
///
/// (c) 2001-2006 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_CLASSMEMBERINDEXEDVAREXPR_H__
#define __PTN_CLASSMEMBERINDEXEDVAREXPR_H__


class PTN_ClassMemberIndexedVarExpr : public PTN_Expr {
public:
   TKS_CachedObject *var;
   TKS_CachedObject *member;     // ro reference to member co
   PTN_Expr         *index_expr; // rw reference to index expression
   sUI               class_id; 
   Feval             index_expr_opt;
   
public:
   PTN_ClassMemberIndexedVarExpr(TKS_CachedObject *_o, TKS_CachedObject *_member, PTN_Expr *_expr);
   ~PTN_ClassMemberIndexedVarExpr();
   
   void      eval          (PTN_Env *_env, YAC_Value *) const;
   Feval     getEval       (void) const;
   sBool     semanticCheck (void);
   void      optimize      (void);
   sBool     resolveXRef   (void);
};



#endif
