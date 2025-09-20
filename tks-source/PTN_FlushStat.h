/// PTN_FlushStat.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_FLUSHSTAT_H__
#define __PTN_FLUSHSTAT_H__


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


#endif // __PTN_FLUSHSTAT_H__
