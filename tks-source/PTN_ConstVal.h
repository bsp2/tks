/// PTN_ConstVal.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_CONSTVAL_H__
#define __PTN_CONSTVAL_H__


class PTN_ConstVal : public PTN_Expr {
public:
   // _s4, _f4, o (nt, float or object pointer)
   // String: o points to constant string
   YAC_Value const_value;

public:
   PTN_ConstVal(void);
   PTN_ConstVal(sF32);
   PTN_ConstVal(sS32);
   PTN_ConstVal(YAC_Object *_o);
   ~PTN_ConstVal();

   void        eval        (PTN_Env *_env, YAC_Value *) const;
   void        evalConst   (YAC_Value *);
   sBool       isConst     (void);
   sUI         getID       (void) const { return PTN_CONSTVAL; } // to avoid self optimization..
   Feval       getEval     (void) const;
   sUI         getType     (void) const;

#ifdef TKS_JIT
   sU8         compile     (VMCore *);
   sBool       forceHybrid (void);
#endif
};


extern sBool tks_isobjectconstval(PTN_Expr *);


#endif // __PTN_CONSTVAL_H__
