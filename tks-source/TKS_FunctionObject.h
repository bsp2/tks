/// TKS_FunctionObject.h
///
/// (c) 2005-2023 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __TKS_FUNCTIONOBJECT_H__
#define __TKS_FUNCTIONOBJECT_H__

 
class TKS_FunctionObject : public YAC_Object {
public:
   PTN_Function *fun; // inner PTN_Function object

   TKS_ScriptObjectID script_id; // default = always valid

public:
   TKS_FunctionObject(void);
   ~TKS_FunctionObject();

   void  YAC_VCALL yacOperatorAssign (YAC_Object *_robj);

   sBool isValid(void); // Used by derived class ScriptFunction to verify that temporary module function still exists

   YAC_String *getName      (void);
   void        eval         (YAC_Object *_args, YAC_Value *_r);
   void        findVariable (YAC_String *_s, YAC_Value *_r);
};


#endif // __TKS_FUNCTIONOBJECT_H__
