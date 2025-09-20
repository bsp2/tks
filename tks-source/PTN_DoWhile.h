/// PTN_DoWhile.h
///
/// (c) 2001-2014 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_DOWHILE_H__
#define __PTN_DOWHILE_H__


class PTN_DoWhile : public PTN_BreakableStatement {
public:
   PTN_Statement *body;
   PTN_Expr      *cond;
   Feval          cond_opt;
   sBool          break_tag; // 1=body uses break;
   
public:
   PTN_DoWhile(void);
   void init(PTN_Statement *_body, PTN_Expr *_cond);
   ~PTN_DoWhile();
   
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


#endif // __PTN_DOWHILE_H__
