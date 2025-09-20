/// PTN_DynTraceStat.h
///
/// (c) 2001-2006 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_DYNTRACESTAT_H__
#define __PTN_DYNTRACESTAT_H__


class PTN_DynTraceStat : public PTN_Statement {
public: 
    PTN_Expr *enable_expr; 
    Feval     enable_expr_opt;

public:
    PTN_DynTraceStat(PTN_Expr *_enable_expr);
    ~PTN_DynTraceStat();

    void    eval           (PTN_Env *_env) const;
    Fevalst getEvalSt      (void) const;
    sBool   semanticCheck  (void); 
    void    optimize       (void);  
    sBool   resolveXRef    (void); 
};


#endif // __PTN_DYNTRACESTAT_H__
