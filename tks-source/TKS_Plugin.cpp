/// TKS_Plugin.cpp
///
/// (c) 2001-2014 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"

#include "TKS_Plugin.h"


#ifdef DX_STATIC_TKSDL
extern void YAC_CALL YAC_Init_tksdl(YAC_Host *_host);
extern void YAC_CALL YAC_Exit_tksdl(YAC_Host *_host);
#endif

#ifdef DX_STATIC_TKOPENGL
extern void YAC_CALL YAC_Init_tkopengl(YAC_Host *_host);
extern void YAC_CALL YAC_Exit_tkopengl(YAC_Host *_host);
#endif


TKS_Plugin::TKS_Plugin(void) {
	version    = 0x00000000;
	dll_handle = NULL;
	fn_init    = NULL;
	fn_exit    = NULL;
}

TKS_Plugin::~TKS_Plugin(void) {
	unload();
}

void TKS_Plugin::unload(void) {
#ifdef DX_STATIC_TKSDL
   if(base_name.compare("tksdl"))
   {
      YAC_Exit_tksdl(tkscript);
      return;
   }
#endif
#ifdef DX_STATIC_TKOPENGL
   else if(base_name.compare("tkopengl"))
   {
      YAC_Exit_tkopengl(tkscript);
      return;
   }
#endif

	if(dll_handle)
	{
      // Call plugin shutdown handler (YAC_Exit)
      if(fn_exit)
      {
         fn_exit(tkscript);   

         Dfixfpuflags;
      }


      // Close native library
      plafCloseLibrary();

      // Unset resolved function pointers (paranoia)
		fn_init = NULL;
		fn_exit = NULL;

		if(tkscript->configuration.debug_level>83)
      {
			tkscript->printf("[...] plugin \"%s\" closed.\n", (sChar*)base_name.chars);
      }
	}
}


sBool TKS_Plugin::load(void) {
#ifdef DX_STATIC_TKSDL
   if(base_name.compare("tksdl"))
   {
      if(tkscript->configuration.debug_level >= TKS_PLUGIN_DEBUG_LEVEL)
      {
         tkscript->printf("[...] load internal plugin \"tksdl\".\n");
      }
      YAC_Init_tksdl(tkscript);
      return 1;
   }
#endif
#ifdef DX_STATIC_TKOPENGL
   if(base_name.compare("tkopengl"))
   {
      if(tkscript->configuration.debug_level >= TKS_PLUGIN_DEBUG_LEVEL)
      {
         tkscript->printf("[...] load internal plugin \"tkopengl\".\n");
      }
      YAC_Init_tkopengl(tkscript);
      return 1;
   }
#endif

#ifdef TKS_DCON
	if(tkscript->configuration.debug_level >= TKS_PLUGIN_DEBUG_LEVEL)
   {
		tkscript->printf("[...] pluginpath=%s\n", (sChar*)tkscript->configuration.plugin_path.chars);
   }
#endif // TKS_DCON

   //
   // Loop pathname variants until library has been openend or until no more variants are left to try
   //
   int variantIdx = 0;
   do 
   {
      //
      // Build plugin pathname for current variant
      //
      int tryIdx = variantIdx;
      variantIdx = plafBuildPathName(variantIdx);

      if(tkscript->configuration.debug_level >= TKS_PLUGIN_DEBUG_LEVEL)
      {
         Dprintf("[...] loadplugin: trying (%d) \"%s\"\n", tryIdx, path_name.chars);
      }

      //
      // Try to open library
      //
      if(plafOpenLibrary((char*)path_name.chars))
      {
#ifdef TKS_DCON
         if(tkscript->configuration.debug_level >= 10/*TKS_PLUGIN_DEBUG_LEVEL*/)
         {
            Dprintf("[...] plugin \"%s\" opened.\n", (char*)path_name.chars);
         }
#endif
         //
         // Resolve/call YAC_Version() [optional]
         //
         void *f = plafFindFunction("YAC_Version");
         if(f)
         {
            sUI (YAC_CALL *fnVersion)(void) = (sUI (YAC_CALL *)(void))f;
            version = fnVersion();

            Dfixfpuflags;

#ifdef TKS_DCON
            if(tkscript->configuration.debug_level >= TKS_PLUGIN_DEBUG_LEVEL)
            {
               Dprintf("[...] loadplugin: plugin version is %08x.\n", version);
            }
#endif
         }

         //
         // Resolve YAC_Init() [required]
         //
         f = plafFindFunction("YAC_Init");
         if(NULL != f)
         {
            fn_init = (void (YAC_CALL*)(YAC_Host*))f;
            if(tkscript->configuration.debug_level >= TKS_PLUGIN_DEBUG_LEVEL)
            {
               Dprintf("[...] loadplugin: found symbol \"YAC_Init\". adr=" YAC_PRINTF_PTRPREFIX "%p\n", fn_init);
            }
         }
         else
         {
   			Dprintf("[---] failed to get symbol \"YAC_Init\" of plugin \"%s\". err=\"%s\"", 
               (char*)path_name.chars, 
               plafGetErrorString()
               );
            plafCloseLibrary();
	   		return 0;
         }

      
         //
         // Resolve YAC_Exit() [required]
         //
         f = plafFindFunction("YAC_Exit");
         if(NULL != f)
         {
            fn_exit = (void (YAC_CALL*)(YAC_Host*))f;
            if(tkscript->configuration.debug_level >= TKS_PLUGIN_DEBUG_LEVEL)
            {
               Dprintf("[...] loadplugin: found symbol \"YAC_Exit\". adr=" YAC_PRINTF_PTRPREFIX "%p\n", fn_exit);
            }

            // 
            // OK, Initialize plugin
            //
            fn_init(tkscript);

            Dfixfpuflags;

            // Succeeded
            return 1;
         }
         else
         {
   			Dprintf("[---] failed to get symbol \"YAC_Exit\" of plugin \"%s\". err=\"%s\"", 
               (char*)path_name.chars, 
               plafGetErrorString()
               );
            plafCloseLibrary();
	   		return 0;
         }
      }
		else
		{
         //
         // Failed to open library. 
         // Try next variant..
         //
      }
   } while(-1 != variantIdx);

   //
   // ..tried all pathname variants..
   //
   Dprintf("[---] failed to open plugin \"%s\" (lastError=\"%s\").\n", (char*)base_name.chars, plafGetErrorString());

   // Failed
	return 0;
}
