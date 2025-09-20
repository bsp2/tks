/// PTN_LoopStatement.h
///
/// (c) 2001-2014 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_LOOPSTATEMENT_H__
#define __PTN_LOOPSTATEMENT_H__


class PTN_LoopStatement : public PTN_BreakableStatement {
public:
   PTN_Expr     *expr;
   sBool         const_expr;
   sUI           const_intval;
   PTN_Statement*body;
   Feval         expr_opt;
   Fevalst       body_opt;
   sBool         break_tag;
   
public:
   PTN_LoopStatement  (void);
   void init          (PTN_Expr *_expr, PTN_Statement *_body);
   ~PTN_LoopStatement ();
   
   sUI     getID          (void) const;
   void    eval           (PTN_Env *_env) const;
   Fevalst getEvalSt      (void) const;
   sBool   semanticCheck  (void);
   void    optimize       (void);
   void    subGenCallList (void);
   sBool   resolveXRef    (void);
   void    useBreak       (void);

#ifdef TKS_JIT
   sU8     compile        (VMCore *);
   sBool   forceHybrid    (void);
#endif
};


#endif // __PTN_LOOPSTATEMENT_H__
