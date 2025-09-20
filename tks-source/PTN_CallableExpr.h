/// PTN_CallableExpr.h
///
/// (c) 2001-2013 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_CALLABLEEXPR_H__
#define __PTN_CALLABLEEXPR_H__


#ifdef TKS_PROFILE_FUNCTION_CALLSITES
/* (note) init. to 0 could be skipped but triggers GCC warnings. it's safe though 
           since b_enable_statement_profiling never changes after main startup */
#define Dcondbegincallsiteprofile sF64 __fxn_profiler_start=0;if(tkscript->configuration.b_enable_statement_profiling) __fxn_profiler_start = TKS_ScriptEngine::GetMilliSecondsDouble()
#define Dcondendcallsiteprofile(loc) if(tkscript->configuration.b_enable_statement_profiling) tkscript->statement_profiler.addResult((loc), TKS_ScriptEngine::GetMilliSecondsDouble()-__fxn_profiler_start)
#else
#define Dcondbegincallsiteprofile
#define Dcondendcallsiteprofile(loc)
#endif // TKS_PROFILE_FUNCTION_CALLSITES


class PTN_CallableExpr : public PTN_Expr {
 public:
  virtual void    evalCallableExpr     (PTN_Env *_env, YAC_Value *_r) const { (void)_env; (void)_r; };
#ifdef TKS_JIT
  virtual sU8     compileCallStatement (VMCore *);
  virtual void    compileCleanUpReturn (VMCore *);
#endif 
          void    eval                 (PTN_Env *_env, YAC_Value *) const;
          Feval   getEval              (void) const;
  virtual Fevalst getEvalCallSt        (void) const {return 0;}

  virtual void    setStatementHint     (void) { } // Inform expr. that its return value is not going to be used (e.g. PTN_Call)
};


#endif
