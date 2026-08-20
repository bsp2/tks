/// PTN_ClassMemberIndirectObjectStat.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef PTN_CLASSMEMBERINDIRECTOBJECTSTAT_H__
#define PTN_CLASSMEMBERINDIRECTOBJECTSTAT_H__


class PTN_ClassMemberIndirectObjectStat : public PTN_Statement {
public:
    TKS_CachedObject *member;
    PTN_Statement    *indirect_stat;
    Fevalst           indirect_stat_opt;

public:
    PTN_ClassMemberIndirectObjectStat(TKS_CachedObject *_member, PTN_Statement *_indirectstatement);
    ~PTN_ClassMemberIndirectObjectStat();

    sBool   semanticCheck (void);
    void    eval          (PTN_Env *_env) const;
    Fevalst getEvalSt     (void) const;
    void    optimize      (void);
    sBool   resolveXRef   (void);
};


#endif // PTN_CLASSMEMBERINDIRECTOBJECTSTAT_H__
