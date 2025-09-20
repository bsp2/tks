/// PTN_PrintStat.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///
#ifndef __PTN_PRINTSTAT_H__
#define __PTN_PRINTSTAT_H__


class PTN_PrintStat : public PTN_Statement {
public:
   sUI       target; // 0=stdout, 1=stderr, 2=dcon, 3=die
   PTN_Expr *expr; 
   sBool     dep_dtrace; 
   sBool     add_newline;
   Feval     expr_opt;
   
public:
   PTN_PrintStat(void);
   PTN_PrintStat(sU8 _d, PTN_Expr *_expr, sBool _depdtrace, sBool _addnewline);
   ~PTN_PrintStat();
   
   void    eval          (PTN_Env *_env) const;
   Fevalst getEvalSt     (void) const;
   sBool   semanticCheck (void);
   void    optimize      (void);
   sBool   resolveXRef   (void);
};


#endif // __PTN_PRINTSTAT_H__
