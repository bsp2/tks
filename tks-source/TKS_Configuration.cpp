/// TKS_Configuration.cpp
///
/// (c) 2001-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///

#include "tks.h"

#ifdef YAC_WIN32
#include <windows.h>
#ifdef HAVE_WINREG
#include <winreg.h>
#endif
#endif
#include <stdlib.h>

#include "TKS_Configuration.h"


TKS_Configuration::TKS_Configuration(void) {
   debug_level                   = 0;
   b_forceint                    = YAC_FALSE;
   b_checkbounds                 = YAC_TRUE;
   b_autoresizearrays            = YAC_TRUE;
   b_showasm                     = YAC_FALSE;
   b_jitopt                      = YAC_TRUE;
   b_enableprint                 = YAC_TRUE;
   b_enablelocalfiles            = YAC_TRUE;
   b_enable_unresolved_members   = YAC_TRUE;
   b_enable_unresolved_functions = YAC_TRUE;
   streamMaxPascalStringSize     = 1024*1024*4;
   streamMaxArraySize            = 2048*1024;
   function_stack_size           = TKS_FUNCTION_STACK_SIZE;
   functioncall_stack_size       = TKS_FUNCTIONCALL_STACK_SIZE;
   b_enable_constraints          = YAC_TRUE;
   b_enable_plugins              = YAC_TRUE;
   b_emit_explanations           = YAC_FALSE;
   b_rewrite_scripts             = YAC_FALSE;
   b_tsl_mode                    = YAC_FALSE;
   b_keep_source_after_compile   = YAC_FALSE;
   b_tkx_linenumbers             = YAC_TRUE;
   b_enable_delete_objects       = YAC_TRUE;
   b_compile_show_progress       = YAC_FALSE;

#ifdef TKS_PROFILE_STATEMENTS
   b_enable_statement_profiling  = YAC_FALSE;
#endif

#ifdef TKS_PROFILE_FUNCTIONS
   b_enable_function_profiling   = YAC_FALSE;
#endif

   plugin_path.visit(::getenv("TKS_PLUGIN_PATH"));
#ifdef YAC_WIN32
#ifdef HAVE_WINREG
   if(plugin_path.isBlank())
   {
      plugin_path.alloc(256);
      HKEY hkey;
      LONG r=::RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("Software\\TKS"),
         0, KEY_QUERY_VALUE, &hkey);

      if(ERROR_SUCCESS == r)
      {
         DWORD dwType, dwSize;
         dwType=REG_SZ;
         dwSize=255;
         ::RegQueryValueEx(hkey, TEXT("TKS_PLUGIN_PATH"), 0, &dwType, (PBYTE)plugin_path.chars/*&m_szPluginPath*/, &dwSize);
         ::RegCloseKey(hkey);
         plugin_path.fixLength();
      }
   }

#ifdef HAVE_CYGWIN
	{
	  HKEY hkey;
     // Try to read from local user settings
	  LONG r=::RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Cygnus Solutions\\Cygwin\\mounts v2\\/usr/lib"),
                           0, KEY_QUERY_VALUE, &hkey);

     if(ERROR_SUCCESS != r)
     {
        // Try to read from global system settings (if cygwin was installed as root)
        r=::RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("Software\\Cygnus Solutions\\Cygwin\\mounts v2\\/usr/lib"),
                              0, KEY_QUERY_VALUE, &hkey);
     }

	  if(ERROR_SUCCESS == r)
     {
        cygwin_usrlib.alloc(256);
        DWORD dwType, dwSize;
        dwType=REG_SZ;
        dwSize=255;
        ::RegQueryValueEx(hkey, TEXT("native"), 0, &dwType, (PBYTE)cygwin_usrlib.chars, &dwSize);
        ::RegCloseKey(hkey);
        cygwin_usrlib.fixLength();
     }
	}
#endif

#endif
#endif

	if(plugin_path.isBlank())
   {
		plugin_path.visit(""); // use CWD
      // (note) plugin loader will try different paths when plugin is not found
   }

	// ---- get module path ----
	module_path.visit(::getenv("TKS_MODULE_PATH"));

#ifdef YAC_WIN32
#ifdef HAVE_WINREG
	if(module_path.isBlank())
	{
		module_path.alloc(256);
		HKEY hkey;
		LONG r=::RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("Software\\TKS"),
		0, KEY_QUERY_VALUE, &hkey);

		if(ERROR_SUCCESS == r)
		{
			DWORD dwType, dwSize;
			dwType=REG_SZ;
			dwSize=255;
			::RegQueryValueEx(hkey, TEXT("TKS_MODULE_PATH"), 0, &dwType, (PBYTE)module_path.chars/*&m_szPluginPath*/, &dwSize);
			::RegCloseKey(hkey);
			module_path.fixLength();
		}
	}
#endif
#endif

	// ---- get library path ----
	library_path.visit(::getenv("TKS_LIBRARY_PATH"));
#ifdef YAC_WIN32
#ifdef HAVE_WINREG
	if(library_path.isBlank())
	{
		library_path.alloc(256);
		HKEY hkey;
		LONG r=::RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("Software\\TKS"),
		0, KEY_QUERY_VALUE, &hkey);

		if(ERROR_SUCCESS == r)
		{
			DWORD dwType, dwSize;
			dwType=REG_SZ;
			dwSize=255;
			::RegQueryValueEx(hkey, TEXT("TKS_LIBRARY_PATH"), 0, &dwType, (PBYTE)library_path.chars/*&m_szPluginPath*/, &dwSize);
			::RegCloseKey(hkey);
			library_path.fixLength();
		}
	}
#endif
#endif

	// ---- get application path ----
	application_path.visit(::getenv("TKS_APPLICATION_PATH"));
#ifdef YAC_WIN32
#ifdef HAVE_WINREG

	if(application_path.isBlank())
	{
		application_path.alloc(256);
		HKEY hkey;
		LONG r=::RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("Software\\TKS"),
		0, KEY_QUERY_VALUE, &hkey);

		if(ERROR_SUCCESS == r)
		{
			DWORD dwType, dwSize;
			dwType=REG_SZ;
			dwSize=255;
			::RegQueryValueEx(hkey, TEXT("TKS_APPLICATION_PATH"), 0, &dwType, (PBYTE)application_path.chars/*&m_szPluginPath*/, &dwSize);
			::RegCloseKey(hkey);
			application_path.fixLength();
		}
	}
#endif
#endif

   home_dir.visit(::getenv("HOME"));

#ifdef YAC_WIN32
   if(home_dir.isBlank())
   {
      home_dir.alloc(_MAX_PATH);
      // (todo) widechar support..
      GetEnvironmentVariable("userprofile", (LPSTR)home_dir.chars, _MAX_PATH);
      home_dir.fixLength();
   }
#endif

    // added for vst2scan.tks utility script
   b_thread_terminate_on_exception = YAC_FALSE;
   b_lock_global_context           = YAC_TRUE;
}

TKS_Configuration::~TKS_Configuration() {
}

void TKS_Configuration::lazySetFallbackPaths(void) {
	if(module_path.isBlank())
   {
		// // module_path.visit(""); // use CWD
      // printf("xxx TKS_Configuration debug 1 exe_dir=\"%s\"\n", tkscript->configuration.tks_exe_dir.chars);
      module_path.yacCopy(&tkscript->configuration.tks_exe_dir);  // use exe dir + "/modules/"
      module_path.append("/modules/");
   }

	if(library_path.isBlank())
   {
		// // library_path.visit(""); // use CWD
      library_path.yacCopy(&tkscript->configuration.tks_exe_dir);  // use exe dir + "/libraries/"
      library_path.append("/libraries/");
   }

	if(application_path.isBlank())
   {
		// // application_path.visit(""); // use CWD
      application_path.yacCopy(&tkscript->configuration.tks_exe_dir);  // use exe dir + "/applications/"
      application_path.append("/applications/");
   }
}
