/// PTN_UnresolvedModuleMemberPostIncDecStat.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_UNRESOLVEDMODULEMEMBERPOSTINCSTAT_H__
#define __PTN_UNRESOLVEDMODULEMEMBERPOSTINCSTAT_H__


class PTN_UnresolvedModuleMemberPostIncDecStat : public PTN_Statement {
public:
    YAC_String                module_name;
    YAC_String                module_member_name;
    sBool                     dec; // 1=decrement, 0=increment
    TKS_CachedObject *        resolved_var;
    TKS_FreezedNamespaceArray nspace_array;
    sBool                     b_default_module;

public:
    PTN_UnresolvedModuleMemberPostIncDecStat(void);
    PTN_UnresolvedModuleMemberPostIncDecStat(YAC_String *_modulename, sBool _bDefaultModule, YAC_String *_modulemember, sBool _dec);
    ~PTN_UnresolvedModuleMemberPostIncDecStat();

    sBool   resolveXRef (void);
    void    eval        (PTN_Env *_env) const;
    Fevalst getEvalSt   (void) const;
};


#endif // __PTN_UNRESOLVEDMODULEMEMBERPOSTINCSTAT_H__
