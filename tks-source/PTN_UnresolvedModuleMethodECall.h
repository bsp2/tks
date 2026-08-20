/// PTN_UnresolvedModuleMethodECall.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef PTN_UNRESOLVEDMODULEMETHODECALL_H__
#define PTN_UNRESOLVEDMODULEMETHODECALL_H__


class PTN_UnresolvedModuleMethodECall : public PTN_CallableExpr {
public:
    YAC_String       module_name;
    YAC_String       module_method_name;
    PTN_ArgList     *arglist;
    PTN_Function    *resolved_fun;

public:
    PTN_UnresolvedModuleMethodECall(YAC_String *_modulename, YAC_String *_methodname, PTN_ArgList *_args);
    ~PTN_UnresolvedModuleMethodECall();

    sBool semanticCheck    (void);
    void  optimize         (void);
    void  evalCallableExpr (PTN_Env *_env, YAC_Value *_r) const;
    void  eval             (PTN_Env *_env, YAC_Value *_r) const;
    Feval getEval          (void) const;
    sBool resolveXRef      (void);
};


#endif // PTN_UNRESOLVEDMODULEMETHODECALL_H__
