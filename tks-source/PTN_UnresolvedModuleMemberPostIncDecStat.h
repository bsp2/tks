/// PTN_UnresolvedModuleMemberPostIncDecStat.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef PTN_UNRESOLVEDMODULEMEMBERPOSTINCSTAT_H__
#define PTN_UNRESOLVEDMODULEMEMBERPOSTINCSTAT_H__


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


#endif // PTN_UNRESOLVEDMODULEMEMBERPOSTINCSTAT_H__
