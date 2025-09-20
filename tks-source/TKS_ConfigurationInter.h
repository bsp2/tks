/// TKS_ConfigurationInter.h
///
/// (c) 2001-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __TKS_CONFIGURATIONINTER_H__
#define __TKS_CONFIGURATIONINTER_H__


class TKS_ConfigurationInter : public YAC_Object {
public:
   TKS_ConfigurationInter  (void);
   ~TKS_ConfigurationInter ();

	 void setDebugLevel (sSI);
	 sSI  getDebugLevel (void);

	 void setForceInt   (sSI);
	 sSI  getForceInt   (void);

    // added for vst2scan.tks utility script
    //  (note) this is a workaround for misbehaving VST plugins which should not be used in regular applications
    void setThreadTerminateOnException (sBool _bEnable);
    void setLockGlobalContext (sBool _bEnable);
    
};


#endif // __TKS_CONFIGURATIONINTER_H__
