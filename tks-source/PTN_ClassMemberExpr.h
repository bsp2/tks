/// PTN_ClassMemberExpr.h
///
/// (c) 2001-2014 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_CLASSMEMBEREXPR_H__
#define __PTN_CLASSMEMBEREXPR_H__


class PTN_ClassMemberExpr : public PTN_Expr {
public:
   TKS_CachedObject *var;
   TKS_CachedObject *member;
   sUI               class_id; 
   sBool             b_deref;
   sBool             b_indirect;
   
public: 
   PTN_ClassMemberExpr(void);
   PTN_ClassMemberExpr(TKS_CachedObject *_co, TKS_CachedObject *_cc_co);
   ~PTN_ClassMemberExpr();

   void initIndirect(void) { b_indirect = 1; }
   
   sBool     semanticCheck (void);
   void      eval          (PTN_Env *_env, YAC_Value *) const;
   Feval     getEval       (void) const; 

#ifdef TKS_JIT
   sUI       getType       (void) const;
   sU8       compile       (VMCore *);
   sBool     forceHybrid   (void); 
#endif 
};


#endif // __PTN_CLASSMEMBEREXPR_H__

