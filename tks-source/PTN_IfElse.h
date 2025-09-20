/// PTN_IfElse.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_IFELSE_H__
#define __PTN_IFELSE_H__


class PTN_IfElse : public PTN_Statement {

public:
   PTN_Expr     *cond;
   PTN_Statement *body;
   PTN_Statement *else_body;
   Feval         cond_opt;
   
public:
   PTN_IfElse(void);
   PTN_IfElse(PTN_Expr *_cond, PTN_Statement *_body, PTN_Statement *_else_body);
   ~PTN_IfElse();

   
   void    eval           (PTN_Env *_env) const;
   Fevalst getEvalSt      (void) const;
   sUI     getID          (void) const;
   sBool   semanticCheck  (void);
   void    optimize       (void);
   void    subGenCallList (void);
   sBool   resolveXRef    (void);

#ifdef TKS_JIT
   sU8   compile        (VMCore *);
   sBool forceHybrid(void);
#endif
};


#endif // __PTN_IFELSE_H__
