/// TKS_ConfigurationInter.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef TKS_CONFIGURATIONINTER_H__
#define TKS_CONFIGURATIONINTER_H__


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


#endif // TKS_CONFIGURATIONINTER_H__
