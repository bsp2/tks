/// PTN_ClassMemberIndirectClassStat.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_CLASSMEMBERINDIRECTCLASSSTAT_H__
#define __PTN_CLASSMEMBERINDIRECTCLASSSTAT_H__


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


#endif // __PTN_CLASSMEMBERINDIRECTCLASSSTAT_H__

