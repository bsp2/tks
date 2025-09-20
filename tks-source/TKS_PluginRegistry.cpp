// TKS_PluginRegistry.cpp
//
// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
//     - distributed under the terms of the GNU general public license (GPL).
//

#include "tks.h" 
 
#include "TKS_Plugin.h"
#include "TKS_PluginRegistry.h"


TKS_PluginRegistry::TKS_PluginRegistry(void) {
	num_plugins = 0;
	
   sU16 i;
	for(i=0; i<TKS_MAX_PLUGINS; i++)
	{
		plugins[i] = NULL;
	}
}

TKS_PluginRegistry::~TKS_PluginRegistry() {
	unloadPlugins();
}

void TKS_PluginRegistry::unloadPlugins(void) {
	sSI i; 
   for(i=(sSI)(num_plugins-1); i>=0; i--)
   {
  		delete plugins[i];
	   plugins[i] = NULL;
   } 
#ifdef TKS_DCON
   if(tkscript->configuration.debug_level >= TKS_PLUGIN_DEBUG_LEVEL)
   {
      Dprintf("[...] unloaded all plugins.\n");
   }
#endif
}

sBool TKS_PluginRegistry::loadPlugin(YAC_String *_name) { 
   if(tkscript->configuration.b_enable_plugins) 
   {
      sU16 i;
      for(i=0; i<num_plugins; i++)
      {
         if(plugins[i]->base_name.compare(_name))
         {
#ifdef TKS_DCON
            if(tkscript->configuration.debug_level >= TKS_PLUGIN_DEBUG_LEVEL)
            {
               Dprintf("[...] load cached  plugin \"%s\"\n", (char*)_name->chars);
            }
#endif
            return 1; /** already loaded **/
         }
      }

      if(num_plugins != TKS_MAX_PLUGINS)
      {
         TKS_Plugin *p = plugins[num_plugins++] = new TKS_Plugin();
         ((YAC_Object*)&p->base_name)->yacCopy(_name);
         return p->load();
      }

      Dprintf("[---] loadPlugin(%s) failed.\n", _name?(char*)_name->chars:"null");
      return 0; 
   } 
   else 
   { 
      // xxx TODO: really ignore loadPlugin if plugins are disabled??
      return 1; 
   }
}
