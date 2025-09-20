/// PTN_HashIniExpr.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_HASHINIEXPR_H__
#define __PTN_HASHINIEXPR_H__


class PTN_HashIniExpr : public PTN_Expr {
public:
   PTN_ExprHashEntry *first_entry;
   sSI               num_entries;
   YAC_HashTable    *ht;
   sBool             is_const;
   sBool             b_always_new;
   
public:
   PTN_HashIniExpr  (sBool _bAlwaysNew);
   ~PTN_HashIniExpr ();
   
   sBool     semanticCheck   (void);
   sBool     resolveXRef     (void);
   void      optimize        (void); 
   sBool     isConst         (void);
   void      evalConst       (YAC_Value *); 
   void      eval            (PTN_Env *_env, YAC_Value *) const; 
   void      eval            (PTN_Env *_env, YAC_Value *, sBool _const); 
   Feval     getEval         (void) const; 
};


#endif
