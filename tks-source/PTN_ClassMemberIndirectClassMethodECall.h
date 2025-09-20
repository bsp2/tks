/// PTN_ClassMemberIndirectClassMethodECall.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_CLASSMEMBERINDIRECTCLASSMETHODECALL_H__
#define __PTN_CLASSMEMBERINDIRECTCLASSMETHODECALL_H__


class PTN_ClassMemberIndirectClassMethodECall : public PTN_CallableExpr {
public:
   TKS_CachedObject     *member;
   PTN_ClassMethodECall *indirect_ecall; 
   Feval                 indirect_ecall_opt;
   
public:
   PTN_ClassMemberIndirectClassMethodECall(TKS_CachedObject *_member, PTN_ClassMethodECall *_indirectecall);
   ~PTN_ClassMemberIndirectClassMethodECall();
   
   sBool semanticCheck     (void);
   void  optimize          (void);
   void  evalCallableExpr  (PTN_Env *_env, YAC_Value *_r) const;
   void  eval              (PTN_Env *_env, YAC_Value *_r) const;
   Feval getEval           (void) const;
   sBool resolveXRef       (void); 
};


#endif // __PTN_CLASSMEMBERINDIRECTCLASSMETHODECALL_H__
