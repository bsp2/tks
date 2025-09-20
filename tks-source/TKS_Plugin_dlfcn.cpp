/// TKS_Plugin_dlfcn.cpp
///
/// (c) 2001-2023 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#ifndef HAVE_DLFCN
#error "platform dependent module TKS_Plugin_dlfcn.cpp requires HAVE_DLFCN macro to be defined."
#endif // HAVE_DLFCN


#include "tks.h"

#include "TKS_Plugin.h"

#include <dlfcn.h>


#ifdef YAC_LINUX
#define Dappendlibsuffix path_name.append(".so")
#endif 

#ifdef YAC_MACOS 
#define Dappendlibsuffix path_name.append(".dylib")
#endif 

static void fix_dir_separator(YAC_String *path_name) {
   if(!path_name->endsWith("/"))
   {
      path_name->append("/");
   }
}

int TKS_Plugin::plafBuildPathName(int _variant) {
   path_name.empty();

   switch(_variant)
   {
   default: // no more variants
      return -1;

   case 0: // try CWD
      {
         ////path_name.yacCopy(&base_name);
         path_name.copy("./");
         path_name.append(&base_name);
         Dappendlibsuffix;
      }
      return 1;

   case 1: // try CWD/plugins/
      {
         path_name.copy("plugins/");
         path_name.append(&base_name);
         Dappendlibsuffix;
      }
      return 2;

   case 2: // try STARTDIR
      {
         path_name.yacCopy(&tkscript->configuration.tks_exe_dir);
         path_name.append(&base_name);
         Dappendlibsuffix;
      }
      return 3;

   case 3: // try STARTDIR/plugins/
      {
         path_name.yacCopy(&tkscript->configuration.tks_exe_dir);
         path_name.append("plugins/");
         path_name.append(&base_name);
         Dappendlibsuffix;
      }
      return 4;

   case 4: // try plugin repository
      {
         path_name.yacCopy(&tkscript->configuration.plugin_path);
         fix_dir_separator(&path_name);
         path_name.append(&base_name);
         Dappendlibsuffix;
      }
      return -1;
   }
}



sBool TKS_Plugin::plafOpenLibrary(const char *_pathName) {
   //
   // Note: RTLD_NOW crashes tksdl after tkopengl loading in some rare cases (read: very old linux installations)
   //       RTLD_GLOBAL crashes/hangs on new ones :) (read: better try RTLD_NOW first)
   sUI flags = RTLD_NOW;
   // sUI flags = RTLD_GLOBAL;

   dll_handle = ::dlopen(_pathName, flags);

   return (NULL != dll_handle);
}

void TKS_Plugin::plafCloseLibrary(void) {
   if(NULL != dll_handle)
   {
	   ::dlclose(dll_handle);
      dll_handle = NULL;
   }
}

void *TKS_Plugin::plafFindFunction(const char *_name) const {
   void *fxnHandle = ::dlsym(dll_handle, _name);

   return fxnHandle;
}

const char *TKS_Plugin::plafGetErrorString (void) {
   const char *msg = ::dlerror();

   return msg;
}
