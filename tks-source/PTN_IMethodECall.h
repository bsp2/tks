/// PTN_IMethodECall.h
///
/// (c) 2001-2013 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_IMETHODECALL_H__
#define __PTN_IMETHODECALL_H__


class PTN_IMethodECall : public PTN_CallableExpr {
public:
   PTN_Expr    *obj_expr;
   YAC_String   method_name;
   PTN_ArgList *method_arglist;
   Feval        obj_expr_opt;
   sBool        b_statement;  // 1=expr. is used as a statement (e.g. var x="a"; x.append("1");) (==> use RSELF instead of RVAL)
   
public:
   PTN_IMethodECall(PTN_Expr *_objexpr, YAC_String *_methodname, PTN_ArgList *_args);
   ~PTN_IMethodECall();
   
   void  setStatementHint (void);
   sBool semanticCheck    (void);
   void  evalCallableExpr (PTN_Env *_env, YAC_Value *_r) const;
   void  eval             (PTN_Env *_env, YAC_Value *_r) const;
   Feval getEval          (void) const;
   sBool resolveXRef      (void);
   void  optimize         (void); 
};


#endif // __PTN_IMETHODECALL_H__

