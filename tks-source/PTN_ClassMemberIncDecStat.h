/// PTN_ClassMemberIncDecStat.h 
///
/// (c) 2001-2013 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_CLASSMEMBERINCDECSTAT_H__
#define __PTN_CLASSMEMBERINCDECSTAT_H__


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


#endif // __PTN_CLASSMEMBERINCDECSTAT_H__
