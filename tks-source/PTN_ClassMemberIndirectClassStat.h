/// PTN_ClassMemberIndirectClassStat.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef PTN_CLASSMEMBERINDIRECTCLASSSTAT_H__
#define PTN_CLASSMEMBERINDIRECTCLASSSTAT_H__


class PTN_ClassMemberIndirectClassStat : public PTN_Statement {
public:
    TKS_CachedObject *member; // class member will be pushed on stack, then indirect_statement is called
    PTN_Statement    *indirect_stat;
    Fevalst           indirect_stat_opt;

public:
    PTN_ClassMemberIndirectClassStat(TKS_CachedObject *_member, PTN_Statement *_indirectstatement);
    ~PTN_ClassMemberIndirectClassStat();

    sBool   semanticCheck (void);
    void    optimize      (void);
    void    eval          (PTN_Env *_env) const;
    Fevalst getEvalSt     (void) const;
    sBool   resolveXRef   (void);
};


#endif // PTN_CLASSMEMBERINDIRECTCLASSSTAT_H__
