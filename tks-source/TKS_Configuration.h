/// TKS_Configuration.h
///
/// (c) 2001-2024 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __TKS_CONFIGURATION_H__
#define __TKS_CONFIGURATION_H__


class TKS_Configuration {
public:

    sBool      b_simulatevfs;                // 1=map logic filenames to local filesystem
    sU8        debug_level;                  // 0..99
    sBool      b_forceint;                   // enable to force interpreted execution, default is OFF
    sBool      b_checkbounds;                // enable array index boundary checks, default in ON
    sBool      b_showasm;                    // show assembler output
    sBool      b_jitopt;                     // optimize JIT code
    sBool      b_enableprint;                // print to console
    sBool      b_enablelocalfiles;           // enable local file access
    sBool      b_enable_unresolved_members;  // enable PTN_UnresolvedModuleMember* x-reference nodes
    sBool      b_enable_unresolved_functions;// enable PTN_UnresolvedModuleMethod* x-reference nodes
    sUI        streamMaxPascalStringSize;    // maximum string size for pascal strings in files.
    sUI        streamMaxArraySize;           // maximum array/hashtable size in streams
    YAC_String plugin_path;                  // path to plugins (os-dependent). read from registry resp. TKS_PLUGIN_PATH environment variable
    YAC_String module_path;                  // path to reusable modules (portable). read from registry resp. TKS_MODULE_PATH environment variable
    YAC_String library_path;                 // path to script libraries. read from registry resp. TKS_LIBRARY_PATH
    YAC_String application_path;             // path to precompiled (.tkx) applications. read from registry resp. TKS_APPLICATION_PATH
    sBool      b_autoresizearrays;           // automagically resize array size if _index > maxElements, default is ON
#ifdef HAVE_CYGWIN
    YAC_String cygwin_usrlib;                // for CygWin, real win32 path to /usr/lib
#endif
    sUI        function_stack_size;          // # of values for recursive script function calls
    sUI        functioncall_stack_size;      // # of recursive script function calls (function pointers)
    sBool      b_enable_constraints;         // 1=enable script constraints, default is 1
    sBool      b_enable_plugins;             // 1=enable c++ plugins, default is 1
    YAC_String default_chapter;              // selects the start-chapter (resp. start-"thread"). empty string (default) means to start with the first chapter in a .tkx/tkp file
    YAC_String sharedmemory_key;             // if executing a Fork ("script-thread") this holds the key to the name of the mmap'd file used for IPC. if !blank; wait for shm call requests after exc'ing global statements/initializers.
    sBool      b_emit_explanations;          // 1=print explanations to stdout after parsing the script then exit without starting main. default is 0.
    sBool      b_rewrite_scripts;            // 1=rewrite script to TKX_File::tokenized_source in addFile() (used by -c, -cl)
    sBool      b_tsl_mode;                   // 1=write .tsl instead of .tkx
#ifdef TKS_ALLOWNODELETEOBJECTS
    sBool      b_enable_delete_objects;      // 0=never delete objects/only flag invalid (useful for debugging to find references to deleted objects)
#endif // TKS_ALLOWNODELETEOBJECTS

#ifdef TKS_PROFILE_STATEMENTS
    sBool      b_enable_statement_profiling; // 1=gather statistics about PTN_Statement nodes or function/method call sites
#endif

#ifdef TKS_PROFILE_FUNCTIONS
    sBool      b_enable_function_profiling;  // 1=gather statistics about function/method calls
#endif

    sBool      b_keep_source_after_compile;  // default=0
    sBool      b_tkx_linenumbers;            // 1=include linenumbers in tkx/tsl files (default) (only relevant if rewrite_scripts is enabled)
    YAC_String tkp_dir;                      // directory where the .tkp file is located (need when the current working directory has changed)
    YAC_String tks_exe_path;                 // Runtime (tks.exe) pathname (absolute path). Extracted from argv[0].
    YAC_String tks_exe_dir;                  // Runtime (tks.exe) directory (absolute path). Extracted from argv[0].

    sBool      b_compile_show_progress;      // print '.' for each parsed module

    YAC_String home_dir;                     // user home directory ('home:' in project files)

    // added for vst2scan.tks utility script
    sBool b_thread_terminate_on_exception;
    sBool b_lock_global_context;

public:
    TKS_Configuration  (void);
    ~TKS_Configuration ();

    void lazySetFallbackPaths (void);  // set app/mod/lib dirs to exe path subdirs when blank
};


#endif // __TKS_CONFIGURATION_H__
