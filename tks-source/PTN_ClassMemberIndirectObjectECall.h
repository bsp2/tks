/// PTN_ClassMemberIndirectObjectECall.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_CLASSMEMBERINDIRECTOBJECTECALL_H__
#define __PTN_CLASSMEMBERINDIRECTOBJECTECALL_H__


class PTN_ClassMemberIndirectObjectECall : public PTN_CallableExpr {
public:
    TKS_CachedObject  *member;
    PTN_CallableExpr  *indirect_ecall;
    Feval              indirect_ecall_opt;

public:
    PTN_ClassMemberIndirectObjectECall(TKS_CachedObject *_member, PTN_CallableExpr *_indirectecall);
    ~PTN_ClassMemberIndirectObjectECall();

    void    setStatementHint (void);
    Fevalst getEvalCallSt    (void) const;
    sBool   semanticCheck    (void);
    void    evalCallableExpr (PTN_Env *_env, YAC_Value *_r) const;
    void    eval             (PTN_Env *_env, YAC_Value *_r) const;
    void    optimize         (void);
    sBool   resolveXRef      (void);
    Feval   getEval          (void) const;
};


#endif // __PTN_CLASSMEMBERINDIRECTOBJECTECALL_H__

