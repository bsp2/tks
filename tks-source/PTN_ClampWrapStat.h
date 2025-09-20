/// PTN_ClampWrapStat.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __PTN_CLAMPWRAPSTAT_H__
#define __PTN_CLAMPWRAPSTAT_H__


class PTN_ClampWrapStat : public PTN_Statement {
public:
   enum __flags {
      MD_CLAMP =(1<<0),
         MD_CLAMPI=(1<<1),
         MD_CLAMPF=(1<<0)|(1<<1),
         MD_WRAP  =(1<<2),
         MD_WRAPI =(1<<3),
         MD_WRAPF =(1<<2)|(1<<3),
         
         MD_PAD
   };
public:
   sU8               mode;
   TKS_CachedObject *var;
   YAC_Value         min;
   YAC_Value         max;
   
   
public:
   PTN_ClampWrapStat  (sU8 _mode, TKS_CachedObject *_var, YAC_Value _min, YAC_Value _max);
   ~PTN_ClampWrapStat ();
   
	  sBool   semanticCheck   (void);
     void    eval            (PTN_Env *_env) const;
     Fevalst getEvalSt       (void) const;
};


#endif // __PTN_CLAMPWRAPSTAT_H__
