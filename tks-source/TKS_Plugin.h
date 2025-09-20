/// TKS_Plugin.h
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __TKS_PLUGIN_H__
#define __TKS_PLUGIN_H__


/// configuration.debug_level threshold for plugin loader log output
#define TKS_PLUGIN_DEBUG_LEVEL 30


class TKS_Plugin {
public:
	sUI        version;    // determined by the return value of the YAC_Version() plugin function
	YAC_String base_name;  // platform-inpendent plugin name ("tksdl", "tkopengl", "yingtest", ..). set by PluginRegistry.
   void      *dll_handle; // opaque, platform dependent DLL/shared object/dylib handle
   YAC_String path_name;  // current path name built by buildPathName()

	void (YAC_CALL*fn_init)(YAC_Host*);
	void (YAC_CALL*fn_exit)(YAC_Host*);


public:
	TKS_Plugin(void);
	~TKS_Plugin();

   // build path name, return next variant index or -1 if all variants have been tried
   int         plafBuildPathName  (int _variant);

   // open platform-dependent library
   sBool       plafOpenLibrary    (const char *_pathName);

   // close platform-dependent library
   void        plafCloseLibrary   (void);

   // find entry point in platform-dependent library
   void *      plafFindFunction   (const char *_name) const;

   // return last error reported by platform-dependent library API
   const char *plafGetErrorString (void);

   // Load plugin
	sBool load   (void);

   // Unload plugin
	void  unload (void);
};


#endif // __TKS_PLUGIN_H__
