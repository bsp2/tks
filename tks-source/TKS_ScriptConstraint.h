/// TKS_ScriptConstraint.h  
///  
/// (c) 2001-2023 Bastian Spiegel <bs@tkscript.de>  
///     - distributed under terms of the GNU general public license (GPL).  
///  
 
#ifndef __TKS_SCRIPTCONSTRAINT_H__ 
#define __TKS_SCRIPTCONSTRAINT_H__ 
 
 
class PTN_Expr; 
 
 
class TKS_ScriptConstraint : public YAC_Object { 
public: 
   YAC_String  name; 
   YAC_String  comment; 
   PTN_Expr   *constraint_expr; 
   YAC_Value  *cval; // referenced by PTN_ConstraintVarExpr 
   sBool       b_done;  // 1=constraint has been fully initialized in TKS_Compiler::compileCurrentChapter()
    
public: 
   TKS_ScriptConstraint(void); 
   ~TKS_ScriptConstraint(); 
    
   sBool checkConstraintExpr(PTN_Env *_env, YAC_Value *_currentValue); 
   char *getConstraintName(void); // name or "notnull" if this is a TKS_CONSTRAINTTYPE_NOTNULL constraint 
    
}; 
 
 
#endif 
