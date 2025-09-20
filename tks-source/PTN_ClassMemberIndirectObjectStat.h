/// PTN_ClassMemberIndirectObjectStat.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_CLASSMEMBERINDIRECTOBJECTSTAT_H__
#define __PTN_CLASSMEMBERINDIRECTOBJECTSTAT_H__


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


#endif // __PTN_CLASSMEMBERINDIRECTOBJECTSTAT_H__
