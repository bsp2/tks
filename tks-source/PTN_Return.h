/// PTN_Return.h
///
/// (c) 2001-2010 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_RETURN_H__
#define __PTN_RETURN_H__


class PTN_Return : public PTN_Statement {
public:
   PTN_Expr     *ret_expr;      // may be NULL
   Feval         ret_expr_opt;  // may be NULL
   PTN_Function *fun;
   sBool         b_break;
   
public:
   PTN_Return(PTN_Expr *_retexpr, PTN_Function *_fun, sBool _bBreak);
   ~PTN_Return();
   
   sBool   semanticCheck (void);
   void    print         (sU16 _i=0);
   void    eval          (PTN_Env *_env) const;
   Fevalst getEvalSt     (void) const;
   void    optimize      (void);
   sBool   resolveXRef   (void);
#ifdef TKS_JIT
   sU8   compile       (VMCore *_vm);
   sBool forceHybrid   (void);
#endif
};


#endif // __PTN_RETURN_H__
