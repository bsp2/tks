/// PTN_OpVarAssign.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_OPVARASSIGN_H__
#define __PTN_OPVARASSIGN_H__


class PTN_OpVarAssign : public PTN_Statement {
public:
   TKS_CachedObject *var;  // ro  reference to variable
   PTN_Expr         *expr; // rw reference to right hand exression
   sU8               expr_operator;
   Feval             expr_opt;
   
public:
   PTN_OpVarAssign(void);
   PTN_OpVarAssign(TKS_CachedObject *_var, PTN_Expr *_expr, sU8 _op);
   ~PTN_OpVarAssign();
   
   void    eval          (PTN_Env *_env) const;
   Fevalst getEvalSt     (void) const;
   sBool   semanticCheck (void);
   void    optimize      (void);
   sBool   resolveXRef   (void);
#ifdef TKS_JIT
   sU8     compile       (VMCore *);
   sBool   forceHybrid   (void);
#endif
};


#endif // __PTN_OPVARASSIGN_H__
