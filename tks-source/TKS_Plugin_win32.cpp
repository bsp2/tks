/// TKS_Plugin_win32.cpp
///
/// (c) 2001-2009 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"

#ifndef YAC_WIN32
#error "platform dependent module TKS_Plugin_win32.cpp requires YAC_WIN32 macro to be defined."
#endif // YAC_WIN32

#include <windows.h>

#include "TKS_Plugin.h"


#define Dappendlibsuffix path_name.append(".dll")


static void fix_dir_separator(YAC_String *path_name) {
   // Fix trailing directory separator
   if(-1 != path_name->indexOf('/'))
   {
      // Using '/' style directory separators
      if(!path_name->endsWith("/"))
      {
         path_name->append("/");
      }
   }
   else
   {
      // Using '\' style directory separators
      if(!path_name->endsWith("\\"))
      {
         path_name->append("\\");
      }
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
         path_name.yacCopy(&base_name);
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
#ifdef HAVE_CYGWIN
         if('/' == path_name[0])
         {
            path_name.replace("/usr/lib", &tkscript->configuration.cygwin_usrlib);
         }
#endif
         Dappendlibsuffix;
      }
      return -1;
   }
}

sBool TKS_Plugin::plafOpenLibrary(const char *_pathName) {
   HINSTANCE dllHandle = ::LoadLibrary((char*)_pathName);

   dll_handle = (void*) dllHandle;

   return (NULL != dll_handle);
}

void TKS_Plugin::plafCloseLibrary(void) {
   HINSTANCE dllHandle = (HINSTANCE) dll_handle;

   ::FreeLibrary(dllHandle);

   dll_handle = NULL;
}

void *TKS_Plugin::plafFindFunction(const char *_name) const {
   HINSTANCE dllHandle = (HINSTANCE) dll_handle;
   
   FARPROC fxnHandle = ::GetProcAddress(dllHandle, _name);

   return (void*) fxnHandle;
}

const char *TKS_Plugin::plafGetErrorString(void) {
   static char buf[1024];
   DWORD lastError = ::GetLastError();
#ifdef YAC_CYGWIN
   snprintf(buf, 1024, "%u", lastError);
#else
   _snprintf(buf, 1024, "%u", lastError);
#endif // YAC_CYGWIN
   return buf;
}
