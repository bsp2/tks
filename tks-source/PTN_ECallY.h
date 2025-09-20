/// PTN_ECallY.h
///
/// (c) 2001-2014 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_ECALLY_H__
#define __PTN_ECALLY_H__


class PTN_ECallY : public PTN_CallableExpr { 
   //
   // Native C++ method call
   //
public: 
   const YAC_CommandY *command; 
   TKS_CachedObject *cached_object; // NULL = "this" call
   PTN_ArgList      *arglist; 
   sUI               clid;           // class id at compile time 
   sBool             b_indirect;
   PTN_Expr         *retobj_expr; // For =>, <=> style calls (>= v0.9.9.0)
   Feval             retobj_expr_opt;
   sBool             b_ret_retobj; // 1:<=> operator, 0:=> operator or regular function
   sBool             b_rself;      // 1: =>*, <=>* operator was used (method modifies "this")
   sBool             b_rself_emul; // 1: emulate =>* using RARG method variant (i.e. when RSELF is not implemented)

   void (PTN_ECallY::*evalCPPCall_opt)(YAC_Object *_o, PTN_Env *_env, YAC_Value *_r) const; // Method pointer to actual evalCPPCall* variant, set by handleRootCommand()
   sBool             b_statement; // 1= expr is used as a statement (use RSELF instead of RVAL!)

#ifndef TKS_MT
   yacmem            call_values[TKS_MAX_ARGS]; // ---- current call parameters ---- 
   yacmemptr         current_call_memptr; 
#endif
   
public: 
   PTN_ECallY(void); 
   PTN_ECallY(const YAC_CommandY *_fun, TKS_CachedObject *_obj, sBool _bIndirect, PTN_ArgList *_args, sUI _clid); 
   ~PTN_ECallY(); 

   sBool handleRootCommand (void); // early semantic check: Check whether RARG/RVAL fun. variants are in effect, return 0(false) if a problem occured

   void    setStatementHint     (void);
   sBool   resolveXRef          (void); 
   sBool   semanticCheck        (void); 
   void    optimize             (void); 
   Fevalst getEvalCallSt        (void) const; // Optimization when call is used as a statement, PTN_Call node is not executed in this case.
   Feval   getEval              (void) const;

   void  evalCallableExpr     (PTN_Env *_env, YAC_Value *_r) const; 
   void  eval                 (PTN_Env *_env, YAC_Value *_r) const;

   void  evalCPPCall          (YAC_Object *_o, PTN_Env *_env, YAC_Value *_r) const; // regular method call
   void  evalCPPCallRARG      (YAC_Object *_o, PTN_Env *_env, YAC_Value *_r) const; // =>"retobj_expr", <=>"retobj_expr"
   void  evalCPPCallRSELF     (YAC_Object *_o, PTN_Env *_env, YAC_Value *_r) const; // <=>*  (=>* is a regular CPP call!)
   void  evalCPPCallRSELF_emul(YAC_Object *_o, PTN_Env *_env, YAC_Value *_r) const; // emulate =>*, <=>* using RARG

   static void EvalCPPCall    (YAC_Object *_o, const YAC_CommandY*command, PTN_ArgList*_args, yacmemptr current_call_memptr, PTN_Env *_env, YAC_Value *_r); 

#ifdef TKS_JIT 
   sU8   compile              (VMCore *);
   sU8   compileCallStatement (VMCore *);
   sBool forceHybrid(void);
#endif
};


#endif // __PTN_ECALLY_H__
