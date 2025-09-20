/// PTN_ObjectMemberIncDecStatY.h
///
/// (c) 2001-2014 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_OBJECTMEMBERINCDECSTATY_H__
#define __PTN_OBJECTMEMBERINCDECSTATY_H__


class PTN_ObjectMemberIncDecStatY : public PTN_Statement {
public:
   TKS_CachedObject   *var;
   const YAC_CommandY *get_command;
   const YAC_CommandY *set_command;
   sBool               dec;
   sUI                 clid;
   sBool               b_indirect;
   
public:
   PTN_ObjectMemberIncDecStatY(void);
   PTN_ObjectMemberIncDecStatY(TKS_CachedObject *_co, const YAC_CommandY *_getcmd, const YAC_CommandY *_setcmd, sBool _dec, sUI _clid);
   ~PTN_ObjectMemberIncDecStatY();

   void initIndirect(void) { var = NULL; b_indirect = YAC_TRUE; }
   
   sBool   semanticCheck (void);
   void    eval          (PTN_Env *_env) const;
   Fevalst getEvalSt     (void) const;
};


#endif // __PTN_OBJECTMEMBERINCDECSTATY_H__
