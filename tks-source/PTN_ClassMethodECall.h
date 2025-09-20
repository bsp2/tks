/// PTN_ClassMethodECall.h
///
/// (c) 2001-2025 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
///
#ifndef PTN_CLASSMETHODECALL_H__
#define PTN_CLASSMETHODECALL_H__


class PTN_ClassMethodECall : public PTN_CallableExpr {
  public:
   TKS_CachedObject    *var;
   TKS_ClassDeclMethod *method;
   PTN_ArgList         *arglist;
   sUI                  class_id;
   sBool                no_virtual; // disable vtable lookup for Class:: namespace addressing
   sBool                b_indirect; // 1=use context->tksvm_indirect_object, 0=use var

  public:
   PTN_ClassMethodECall(TKS_CachedObject *_var, TKS_ClassDeclMethod *_method, PTN_ArgList *_args);
   ~PTN_ClassMethodECall();

   void initIndirect(void) { b_indirect = 1; }

   sBool semanticCheck    (void);
   void  optimize         (void);
   sBool resolveXRef      (void);
   void  evalCallableExpr (PTN_Env *_env, YAC_Value *_r) const;
   void  eval             (PTN_Env *_env, YAC_Value *_r) const;
   Feval getEval          (void) const;
};


#endif // PTN_CLASSMETHODECALL_H__
