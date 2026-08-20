/// PTN_FunctionECallY.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef PTN_FUNCTIONECALLY_H__
#define PTN_FUNCTIONECALLY_H__


class PTN_FunctionECallY : public PTN_CallableExpr {
   //
   // Native C/C++ function call
   //
public:
   const YAC_CommandY *command;
   PTN_ArgList  *arglist;
   PTN_Expr     *retobj_expr; // For =>, <=> style calls (>= v0.9.9.0)
   Feval         retobj_expr_opt;
   sBool         b_ret_retobj; // 1:<=> operator, 0:=> operator or regular function


public:
   PTN_FunctionECallY(void);
   PTN_FunctionECallY(YAC_CommandY *_fun, PTN_ArgList *_args);
   ~PTN_FunctionECallY();

   static sBool CheckRootCommand(sBool _bHaveRetObjExpr, const YAC_CommandY**_cmdAddr, sBool _bRSELF, sBool *_bRSELF_emulRet);

   sBool handleRootCommand (void); // early semantic check: Check whether RARG/RVAL/RSELF fun. variants are in effect, return 0(false) if a problem occured

   sBool resolveXRef          (void);
   sBool semanticCheck        (void);
   void  optimize             (void);
   void  evalCallableExpr     (PTN_Env *_env, YAC_Value *_r) const;
   void  eval                 (PTN_Env *_env, YAC_Value *_r) const;
   Feval getEval              (void) const;

#ifdef TKS_JIT
   sU8   compile              (VMCore *);
   sU8   compileCallStatement (VMCore *);
   sBool forceHybrid          (void);
#endif
};


#endif // PTN_FUNCTIONECALLY_H__
