/// PTN_FlushStat.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef PTN_FLUSHSTAT_H__
#define PTN_FLUSHSTAT_H__


class PTN_FlushStat : public PTN_Statement {
public:
   PTN_Expr *expr;
   Feval     expr_opt;

public:
   PTN_FlushStat(PTN_Expr *_expr);
   ~PTN_FlushStat();

   void    eval          (PTN_Env *_env) const;
   Fevalst getEvalSt     (void) const;
   sBool   semanticCheck (void);
   void    optimize      (void);
   sBool   resolveXRef   (void);
};


#endif // PTN_FLUSHSTAT_H__
