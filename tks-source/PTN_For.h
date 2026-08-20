/// PTN_For.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef PTN_FOR_H__
#define PTN_FOR_H__


class PTN_For : public PTN_BreakableStatement {
  public:
   PTN_Statement *pre;
   PTN_Expr      *cond;
   PTN_Statement *post;
   PTN_Statement *body;
   Feval          cond_opt;
   sBool          break_tag;

  public:
   PTN_For(void);
   ~PTN_For();

   void    init           (PTN_Statement *_pre, PTN_Expr *_cond, PTN_Statement *_post, PTN_Statement *_body);
   void    eval           (PTN_Env *_env) const;
   Fevalst getEvalSt      (void) const;
   sUI     getID          (void) const;
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


#endif // PTN_FOR_H__
