/// PTN_ExprStat.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef PTN_EXPRSTAT_H__
#define PTN_EXPRSTAT_H__


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


#endif // PTN_EXPRSTAT_H__
