/// PTN_PointerAssign.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///

#ifndef __PTN_POINTERASSIGN_H__
#define __PTN_POINTERASSIGN_H__


class PTN_PointerAssign : public PTN_Statement {
public:
   TKS_CachedObject *left_co;    // ro reference to variable
   PTN_Expr         *right_expr; // rw reference to right hand expression 
   Feval             right_expr_opt; 
   
public:
   PTN_PointerAssign(void);
   PTN_PointerAssign(TKS_CachedObject *_lco, PTN_Expr *_rexpr);
   ~PTN_PointerAssign();
   
   void    eval          (PTN_Env *_env) const; 
   Fevalst getEvalSt     (void) const;
   sBool   semanticCheck (void);
   void    optimize      (void); 
   sBool   resolveXRef   (void); 
#ifdef TKS_JIT 
   sU8   compile       (VMCore *_vm); 
   sBool forceHybrid   (void); 
#endif
};


#endif // __PTN_POINTERASSIGN_H__
