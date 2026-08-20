/// PTN_FunctionECall.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef PTN_FUNCTIONECALL_H__
#define PTN_FUNCTIONECALL_H__


class PTN_FunctionECall : public PTN_CallableExpr {
   //
   // Script function call
   //
public:
   PTN_Function *fun;
   PTN_ArgList  *arglist;

public:
   PTN_FunctionECall(void);
   PTN_FunctionECall(PTN_Function *_fun, PTN_ArgList *_args);
   ~PTN_FunctionECall();

   void  evalCallableExpr (PTN_Env *_env, YAC_Value *_r) const;
   static sBool  SemanticCheck (PTN_Function *fun, PTN_ArgList *arglist, sUI _srcLoc);

   sUI    getID         (void) const;
   sBool  semanticCheck (void);
   void   eval          (PTN_Env *_env, YAC_Value *_r) const;
   Feval  getEval       (void) const;
   void   optimize      (void);
   sBool  resolveXRef   (void);
};


#endif // PTN_FUNCTIONECALL_H__
