/// TKS_ScriptVariable.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef TKS_SCRIPTVARIABLE_H__
#define TKS_SCRIPTVARIABLE_H__


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

#endif // TKS_SCRIPTVARIABLE_H__
