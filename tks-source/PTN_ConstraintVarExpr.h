/// PTN_ConstraintVarExpr.h  
///  
/// (c) 2004-2009 Bastian Spiegel <bs@tkscript.de>  
///     - distributed under terms of the GNU general public license (GPL).  
///  
 
#ifndef __PTN_CONSTRAINTVAREXPR_H__ 
#define __PTN_CONSTRAINTVAREXPR_H__ 
 
 
// ---- This expression simply returns the current value of a method argument  
// ---- which has a constraint attached. 
class PTN_ConstraintVarExpr : public PTN_Expr { 
public: 
   YAC_Value **cvalref; // ref. to mem.location which holds the current value to which the constraint is applied 
    
public: 
   PTN_ConstraintVarExpr(YAC_Value **_cvalref); 
   ~PTN_ConstraintVarExpr(); 
    
   sBool  semanticCheck (void); 
   void   eval          (PTN_Env *_env, YAC_Value *) const; 
   Feval  getEval       (void) const;
}; 
 

#endif 
