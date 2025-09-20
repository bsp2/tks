/// PTN_ConstValReference.h
///
/// (c) 2005-2010 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_CONSTVALREFERENCE_H__
#define __PTN_CONSTVALREFERENCE_H__

 
class PTN_ConstValReference : public PTN_Expr {
public:
   TKS_CachedObject *const_ref; 
   
public:
   PTN_ConstValReference(TKS_CachedObject *_ref);
   ~PTN_ConstValReference();
   
   void        eval        (PTN_Env *_env, YAC_Value *) const;
   void        evalConst   (YAC_Value *); 
   sBool       isConst     (void);
   Feval       getEval     (void) const;
   sUI         getType     (void) const;
   
};


#endif // __PTN_CONSTVALREFERENCE_H__
