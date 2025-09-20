/// PTN_Expr.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_EXPR_H__
#define __PTN_EXPR_H__

 
class PTN_Expr : public PTN_Node {
public:
   PTN_Expr(void);
   ~PTN_Expr();

   virtual void   eval      (PTN_Env *_env, YAC_Value *) const;      // evaluate expression
   virtual void   evalConst (YAC_Value *);      // unlink object(list) (arrayiniexpr,hashini,valuelist..)
   void           evalTC    (PTN_Env *_env, sU8 _rtti, YAC_Value *_r) const; // evaluate expression and typecast return value
   virtual Feval  getEval   (void) const;       // 
   virtual sUI    getType   (void) const;       // get RTTI hint (0=variable, 1,2,3,4=string)
};

 
extern void  tks_optimize_expr(PTN_Expr**, sU8 _rtti_hint);
 

#endif // __PTN_EXPR_H__
