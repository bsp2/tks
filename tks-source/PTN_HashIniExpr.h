/// PTN_HashIniExpr.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef PTN_HASHINIEXPR_H__
#define PTN_HASHINIEXPR_H__


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


#endif // PTN_HASHINIEXPR_H__
