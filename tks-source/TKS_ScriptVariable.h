/// TKS_ScriptVariable.h
///
/// (c) 2001-2005 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __TKS_SCRIPTVARIABLE_H__
#define __TKS_SCRIPTVARIABLE_H__


class TKS_ScriptVariable : public YAC_ValueObject {
public:
   TKS_CachedObject *var;
   TKS_ScriptObjectID script_id;
   
public:
   TKS_ScriptVariable(void);
   ~TKS_ScriptVariable();

   sBool isValid(void);

   void        assign      (YAC_Object *_valueObject);
   sSI         query       (void);
   sSI         store       (void);
   sSI         storeDeref  (void);
   YAC_String *getName     (void);
};

#endif
