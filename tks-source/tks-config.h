/// tks-config.h
///
///  (c) 2001-2014 Bastian Spiegel <bs@tkscript.de>
///      - distributed under terms of the GNU general public license (GPL).
///
///
///

#ifndef __TKS_CONFIG_H__
#define __TKS_CONFIG_H__


///============================================================================
///
/// Define this to effectively disable exceptions, constraints, node-tracing, 
///  profiling, abstractcall and other paranoia checks
///
///============================================================================
//#define TKS_SPEEDHACKS defined


#include "config.h"


///============================================================================
///
/// usually you won't have to change anything in this file.
/// please see "config.h" instead.
/// 
///============================================================================

#define TKS_PLUGIN defined 
#define TKS_DCON defined 


///============================================================================
///
/// OS-CONFIG
/// 
///============================================================================

 
/// Define if your hardware can access multi-byte values at odd addresses (e.g. intel, not 68k)
///  (note) currently only relevant in the JIT compiler
#ifdef YAC_LITTLE_ENDIAN
#define DX_ODDADDR defined
#endif
 
#ifdef YAC_POSIX 
#ifndef DX_NO_PTHREADS 
#define DX_PTHREADS defined 
#endif 
#endif 
 
#ifdef YAC_MSDOS 
#undef HAVE_VSNPRINTF 
#endif 
 

///============================================================================
///
/// ENVIRONMENT-CONFIG
/// 
///============================================================================

/// Define if you want to compile with the VM disassembler (also requires TKS_JIT to be defined)
#define DX_VMDASM defined

/// Define if you want to include runtime object sanity checks (recommended)
#define DX_SAFEMODE defined

/// Define if you want to include numElements tracking in the hybrid VM arrayindexassign functions
#define DX_JIT_TRACKARRAYELEMENTS defined

/// Define if you the scanner should not re-allocate already existing strings (needs more mem.)
//#define DX_SCANNER_FASTADDUNIQUESTRING defined

/// Define to write out "c" code for the entity scanners ("autogen_tks_entitytree_lookup.cpp")
/* #define TKS_EXPORT_ENTITY_SCANNER defined */
 
/// Define if script function calls shall be tracked in a functioncallstack (useful for debug purposes) 
#define DX_TRACEFUNCTIONCALLS defined 
 
/// Define if you want script constraints
#define DX_SCRIPTCONSTRAINTS defined 

/// Define if you want a thread-safe script engine
/// this will enable TKS_Mutex, TKS_Thread, .. classes
#define TKS_MT defined

/// Define if you want to trace AST node evaluation (activates the "last seen AST node" debug print)
#define TKS_TRACE_NODES defined

/// Define if you want to profile PTN_Statement evaluation (number of calls, duration of calls)
#define TKS_PROFILE_STATEMENTS defined

/// Define to only trace function/method calls when profiler is enabled
///  Note: Enabling profiling for all statements can seriously skew the results
///        because of the GetMilliSecondsDouble() (QueryPerformanceCounter resp. gettimeofday)
///        overhead (e.g. 1578ms instead of 62ms for a function with 4 statements that is called
///        250000 times)
#define TKS_PROFILE_FUNCTION_CALLSITES defined

/// Define to enable function/method profiling
#define TKS_PROFILE_FUNCTIONS defined

/// Define to use x86 RDTSC instruction for profiling
///  Note: This was added because the Win32 QueryPerformanceCounter() call is dog slow
///        However, RDTSC may be unreliable if CPU changes clock frequency
//   Note: this is defined in makefile.msvc (32bit build only)
//#define DX_RDTSC defined

/// Define if you are sure that parameters are never passed in registers 
//#define TKS_ALLOWINTFLOATMIXUP defined

/// Define to check for raised exceptions
#define TKS_CHECKEXCEPTION defined

/// Define to print exceptions immediately (e.g. print backtrace to console even if the exception 
/// is later caught by a try/catch statement). Defining this might cause confusion in e.g. unit-tests
/// (read: the prints destroy the text flow).
//#define TKS_PRINTEXCEPTION defined

/// Define to compile-in the File::openTemp,removeTemp methods (**deprecated**)
//#define TKS_OLD_TEMP_FILES defined

/// Define to compile-in the PakFile::openLocal method (**deprecated**)
#define TKS_PAKFILE_OPENLOCAL defined

/// Define to enable allow the "--nodeleteobjects" cmdline option
#define TKS_ALLOWNODELETEOBJECTS defined

/// Define to prevent abstract method calls (mainly a development aid to find missing method implementations)
#define TKS_CHECK_ABSTRACTCALL defined

/// Define to unset abstract scriptclassinstance(resp.template) pointers in variables (global, namespaces, functions)
///  (if not defined, the template object will be used instead of null)
#define TKS_FIX_ABSTRACTVARS defined

/// Define to count #cachedobjects (object_counter)
#define TKS_CO_OBJECTCOUNTER defined

/// Define to count #strings (object_counter)
#define TKS_STRING_OBJECTCOUNTER defined

/// Define to count #scriptclassinstances (object_counter)
#define TKS_SCI_OBJECTCOUNTER defined

/// Define to use optimized node allocator
#define TKS_USE_NODEALLOCATOR defined

/// Node allocator chunk/stripe size. default=256k
#define TKS_NODEALLOCATOR_STRIPESIZE (256*1024)

/// Define to use optimized char allocator
#define TKS_USE_CHARALLOCATOR defined

/// Char allocator chunk/stripe size. default=64k
#define TKS_CHARALLOCATOR_STRIPESIZE (64*1024)

/// Char allocator max. string size
#define TKS_CHARALLOCATOR_THRESHOLD (64)

#ifdef TKS_SPEEDHACKS
#undef TKS_TRACE_NODES
#undef TKS_PROFILE_STATEMENTS
#undef DX_TRACEFUNCTIONCALLS
#undef TKS_CHECKEXCEPTION
#undef DX_SCRIPTCONSTRAINTS
#undef DX_JIT_TRACKARRAYELEMENTS
#undef DX_SAFEMODE
#undef TKS_CHECK_ABSTRACTCALL
#undef TKS_FIX_ABSTRACTVARS
#endif // TKS_SPEEDHACKS


/// if defined, use QueryPerformanceCounter() for TKS_ScriptEngine::getMilliseconds()
///  to increase accuracy (w/o it's ~16ms)
#define TKS_WIN32_USE_PERFCOUNTER defined


#endif // __TKS_CONFIG_H__
