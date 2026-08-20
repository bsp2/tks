/// PTN_ClassMemberIncDecStat.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef PTN_CLASSMEMBERINCDECSTAT_H__
#define PTN_CLASSMEMBERINCDECSTAT_H__


class PTN_ClassMemberIncDecStat : public PTN_Statement {
public:
    TKS_CachedObject *var;
    TKS_CachedObject *member;
    sUI               class_id;
    sBool             dec;
    sBool             b_indirect;

public:
    PTN_ClassMemberIncDecStat(TKS_CachedObject *_co, TKS_CachedObject *_cc_co, sBool _dec);
    ~PTN_ClassMemberIncDecStat();

    void initIndirect(void) { b_indirect = 1; }

    sBool   semanticCheck (void);
    void    eval          (PTN_Env *_env) const;
    Fevalst getEvalSt     (void) const;
};


#endif // PTN_CLASSMEMBERINCDECSTAT_H__
