/// PTN_While.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///
///

#ifndef PTN_WHILE_H__
#define PTN_WHILE_H__


class PTN_While : public PTN_BreakableStatement {
public:
   PTN_Expr      *cond;
   PTN_Statement *body;
   Feval          cond_opt;
   sBool          break_tag;

public:
   PTN_While(void);
   PTN_While(PTN_Expr *_cond, PTN_Statement *_body);
   void init(PTN_Expr *_cond, PTN_Statement *_body);
   ~PTN_While();

   void    eval           (PTN_Env *_env) const;
   Fevalst getEvalSt      (void) const;
   sUI     getID          (void) const;
   sBool   semanticCheck  (void);
   void    optimize       (void);
   void    subGenCallList (void);
   sBool   resolveXRef    (void);
   void    useBreak       (void);

#ifdef TKS_JIT
   sU8   compile        (VMCore *);
   sBool forceHybrid(void);
#endif
};


#endif // PTN_WHILE_H__
