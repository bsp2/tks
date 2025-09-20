/// TKS_PluginRegistry.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __TKS_PLUGINREGISTRY_H__
#define __TKS_PLUGINREGISTRY_H__


#define TKS_MAX_PLUGINS (YAC_MAX_CLASSES-TKS_CLID_NUMINTERNALS)


class TKS_PluginRegistry {
public:
	sU8         num_plugins;
	TKS_Plugin *plugins[TKS_MAX_PLUGINS];
public:
	TKS_PluginRegistry(void);
	~TKS_PluginRegistry();

	sBool loadPlugin    (YAC_String *_name);
	void  unloadPlugins (void);
};


#endif // __TKS_PLUGINREGISTRY_H__
