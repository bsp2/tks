/// TKS_PluginRegistry.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef TKS_PLUGINREGISTRY_H__
#define TKS_PLUGINREGISTRY_H__


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


#endif // TKS_PLUGINREGISTRY_H__
