/// PTN_ConstValReference.h
///
/// (c) 2005-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef PTN_CONSTVALREFERENCE_H__
#define PTN_CONSTVALREFERENCE_H__


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


#endif // PTN_CONSTVALREFERENCE_H__
