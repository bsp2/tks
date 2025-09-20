/// PTN_ObjectMemberIncDecExprY.h
///
/// (c) 2001-2014 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_OBJECTMEMBERINCDECEXPRY_H__
#define __PTN_OBJECTMEMBERINCDECEXPRY_H__


class PTN_ObjectMemberIncDecExprY : public PTN_Expr {
public:
   TKS_CachedObject   *var;
   const YAC_CommandY *get_command;
   const YAC_CommandY *set_command;
   sBool               post;
   sBool               dec;
   sUI                 clid;
   sBool               b_indirect;
   
public:
   PTN_ObjectMemberIncDecExprY  (void);
   PTN_ObjectMemberIncDecExprY  (TKS_CachedObject *_var, const YAC_CommandY *_getcommand, const YAC_CommandY *_setcommand, sBool _post, sBool _dec, sUI _clid);
   ~PTN_ObjectMemberIncDecExprY ();

   void initIndirect(void) { var = NULL; b_indirect = YAC_TRUE; }
   
   void  eval    (PTN_Env *_env, YAC_Value *) const;
   Feval getEval (void) const;
};


#endif // __PTN_OBJECTMEMBERINCDECEXPRY_H__

