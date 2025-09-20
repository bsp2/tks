/// PTN_Statement.h
///
/// (c) 2001-2010 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_STATEMENT_H__
#define __PTN_STATEMENT_H__


// Note: also see PTN_CallablExpr.h for fxn profiler macros
#ifdef TKS_PROFILE_STATEMENTS
 #define Dbeginstprofile sF64 __statement_profiler_start = TKS_ScriptEngine::GetMilliSecondsDouble()
 #define Dendstprofile(loc) tkscript->statement_profiler.addResult((loc), TKS_ScriptEngine::GetMilliSecondsDouble()-__statement_profiler_start)
#else
 #define Dbeginstprofile
 #define Dendstprofile(loc)
#endif // TKS_PROFILE_STATEMENTS




class PTN_Statement : public PTN_Node {
public:
   static sSI object_counter;

public:
  PTN_Statement  *next;
  PTN_Statement  *prev;
  const PTN_Statement **st_list; 

  Fevalst        *st_list_opt;

public:
  PTN_Statement(void);
  ~PTN_Statement();

          void           evalFirst      (PTN_Env *_env) const;
  virtual void           eval           (PTN_Env *_env) const;
          sBool          semanticCheck  (void);
          void           optimize       (void);
          void           genCallList    (void);
  virtual Fevalst        getEvalSt      (void) const;

  // query arg for optimized getEvalCallSt fxn ptr
  // used for expressions that can be called used as statements (e.g. PTN_FunctionECall, PTN_FunctionECallY)
  virtual const PTN_Statement *getEvalStArg   (void) const {return this;} 
  virtual void           subGenCallList (void);
          sBool          resolveXRef    (void);
  virtual void           useBreak       (void);
          
          void           optimizeAll      (void);
          sBool          resolveXRefAll   (void);
          sBool          semanticCheckAll (void);


#ifdef TKS_JIT
          sU8     compile                (VMCore *);
          sBool   forceHybrid            (void);
          sU8     compileHybridStatement (VMCore *_vm);
#endif
};


#endif // __PTN_STATEMENT_H__
