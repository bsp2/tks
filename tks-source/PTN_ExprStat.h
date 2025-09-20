/// PTN_ExprStat.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_EXPRSTAT_H__
#define __PTN_EXPRSTAT_H__

 
class PTN_ExprStat : public PTN_Statement {
public:
   PTN_Expr *expr;
   Feval     expr_opt;
   
public:
   PTN_ExprStat(PTN_Expr *);
   ~PTN_ExprStat();
   
   sBool   semanticCheck (void);
   void    eval          (PTN_Env *_env) const;
   Fevalst getEvalSt     (void) const;
   sBool   resolveXRef   (void);
   void    optimize      (void);
};


#endif // __PTN_EXPRSTAT_H__
