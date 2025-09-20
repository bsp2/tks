/// PTN_ObjClampWrapStat.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_OBJCLAMPWRAPSTAT_H__
#define __PTN_OBJCLAMPWRAPSTAT_H__


class PTN_ObjClampWrapStat : public PTN_Statement {
public:
   sU8               mode;
   TKS_CachedObject *var;
   PTN_Expr         *min;
   PTN_Expr         *max;
   Feval             min_opt;
   Feval             max_opt;
   
public:
   PTN_ObjClampWrapStat  (sU8 _mode, TKS_CachedObject *_var, PTN_Expr *_min, PTN_Expr *_max);
   ~PTN_ObjClampWrapStat ();
   
   sBool   resolveXRef     (void);
   void    optimize        (void);
   sBool   semanticCheck   (void);
   void    eval            (PTN_Env *_env) const;
   Fevalst getEvalSt       (void) const;
};


#endif // __PTN_OBJCLAMPWRAPSTAT_H__
