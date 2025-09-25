/// tks.h
///
/// (c) 2001-2025 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the GNU general public license (GPL).
///

#ifndef TKS_CORE_H__
#define TKS_CORE_H__

// How to inline methods
#define TKS_INLINE_METHOD inline

class TKS_ScriptEngine;
class _String;
class TKS_PluginCallback;

extern TKS_ScriptEngine *tkscript;

// Version information
#define TKS_VERSION_STR_BASE "0.9.92.4"
#define TKS_VERSION 0x00095C04
#ifdef DX_X64
#define TKS_VERSION_STR TKS_VERSION_STR_BASE " (x64)"
#elif defined(DX_X86)
#define TKS_VERSION_STR TKS_VERSION_STR_BASE " (x86)"
#elif defined(DX_ARM64)
#define TKS_VERSION_STR TKS_VERSION_STR_BASE " (arm64)"
#elif defined(DX_ARM32)
#define TKS_VERSION_STR TKS_VERSION_STR_BASE " (arm32)"
#else
#define TKS_VERSION_STR TKS_VERSION_STR_BASE
#endif

// Maximum number of tokens per module (usually <11000)
#define TKS_SCANNER_MAX_TOKENS  300000

// Maximum number of strings per module (usually <1000)
#define TKS_SCANNER_MAX_STRINGS  8000

// Maximum number of lines per module (usually <4000)
#define TKS_SCANNER_MAX_LINES   30000

// Maximum number of script modules
#define TKS_MAX_MODULES 512

// Maximum number of functions per script module
#define TKS_MAX_FUNCTIONS 512

// Maximum number of variables (static or local) per function
#define TKS_MAX_FUNCTION_VARIABLES 256

// Maximum number of (global) variables per script module
#define TKS_MAX_MODULE_VARIABLES 3512

// Maximum number of (global) variables per namespace
#define TKS_MAX_NAMESPACE_VARIABLES 512

// Maximum number of global variables (across all script modules)
#define TKS_MAX_GLOBAL_VARIABLES 512

// Maximum number of constants (all script modules)
#define TKS_MAX_GLOBAL_CONSTANTS 2600

// Maximum number of tagged variable per temporary script module
#define TKS_MAX_TEMPSCRIPT_TAGGED_VARS 128

// Maximum number of enum constants (all script modules)
#define TKS_MAX_ENUM_CONSTANTS 512

// Parser stack depth
#define TKS_MAX_PARSER_STACKDEPTH 4096

////#define TKSCRIPT_BREAKSTACKDEPTH 96
#define TKS_BREAK_STACK_SIZE 512

// Total function stack size (number of values / local vars)
#define TKS_FUNCTION_STACK_SIZE 8192

// Maximum configurable stack size (cmdline option -fss)
#define TKS_MAX_FUNCTION_STACK_SIZE 524288

// Total number of recursive native/script calls
#define TKS_FUNCTIONCALL_STACK_SIZE 1024

// Maximum configurable call stack size (cmdline option -fcs)
#define TKS_MAX_FUNCTIONCALL_STACK_SIZE 524288

// Maximum number of nested switch * 2 (cmpvalue+breakflag)
#define TKS_SWITCH_STACK_SIZE 512

// Maximum number of signal callbacks per C++ class
#define TKS_MAXSIGNALS 32

// Maximum number of script classes (all modules)
#define TKS_MAX_CLASSDECLS 900

// Maximum number of non-static members per script class
#define TKS_MAX_CLASSMEMBERS 450

// Maximum number of static members per script class
/////#define TKS_MAX_CLASSSTATICMEMBERS 64

// Maximum number of non-static methods per script class
//#define TKS_MAX_CLASSMETHODS 128
//#define TKS_MAX_CLASSMETHODS 256
/* #define TKS_MAX_CLASSMETHODS 384 */
#define TKS_MAX_CLASSMETHODS 512

// Maximum number of static methods per script class
#define TKS_MAX_CLASSFUNCTIONS 256

// Maximum number of arguments to a function/method
#define TKS_MAX_ARGS 16

// Maximum number of constants per script class
#define TKS_MAX_CLASSCONSTANTS 256

// Maximum number of ancestors per script class
#define TKS_MAX_CLASSANCESTORS 32

// Maximum number of ancestors per script class, including all ancestor classes
#define TKS_MAX_CLASSDEPENDENCIES 64

// Maximum class name length (number of chars)
#define TKS_MAX_CLASSNAMELENGTH 64

// Maximum number of compile-time added class member initializers
#define TKS_MAX_CLASSINITIALIZERS 16384

// Maximum int array size; values beyond this one are considered "insane"
#define TKS_MAX_INTARRAYALLOCSIZE (1024*1024*128)

// Maximum float array size; values beyond this one are considered "insane"
#define TKS_MAX_FLOATARRAYALLOCSIZE (1024*1024*128)

// Maximum generic array size; values beyond this one are considered "insane"
#define TKS_MAX_ARRAYALLOCSIZE (1024*1024*4)

// Maximum number of named constraints (all modules)
#define TKS_MAX_CONSTRAINTS 512

// Maximum number of namespaces
#define TKS_MAX_NAMESPACES 128

////#define TKS_CPP_MAXCOMMANDS 64

// Maximum number of C functions
#define TKS_C_MAXEXTFUNCTIONS 1536

// Maximum number of temporary script modules
#define TKS_MAX_TEMPMODULES 256

// Allow declaration of classes in temporary script modules
//  (note) applications must ensure that all temp class instances have been deleted before the temp script is deleted
//  (note) when undefined, an error is raised when a temp script is trying to declare a class
#define TKS_ALLOW_TEMP_CLASSES defined

// Maximum size of string pool (only used if compiling without multithreading support)
#define TKS_MAX_POOL_SIZE 131072

// Maximum number of user-defined exceptions (+builtin exceptions)
#define TKS_MAX_EXCEPTIONS 256

// Maximum number of nested try..catch statements
#define TKS_TRYCATCHSTACKDEPTH 64

// Maximum number of plugin<->plugin callbacks
#define TKS_MAX_PLUGINCALLBACKS 256

// Maximum number of script execution contexts
#define TKS_MAX_CONTEXTS 128

// Maximum number of named mutexes (internal and for yacMutexCreate, yacMutexDestroy)
#define TKS_MAX_NAMED_MUTEXES 256

// ---- this is used to correct the YAC_Object::object_counter
// ---- see TKS_CachedScript.cpp and TKS_Language.cpp (static YAC_String)
// ---- (tok + Class_ClassDecl + TKS_ClassDecl->static_co->name + tks_local_pathname)
#ifdef DX_STATIC_TKSDL
#define TKS_NUM_STATIC_OBJECTS 12u
#else
#define TKS_NUM_STATIC_OBJECTS 11u
#endif

// Maximum number of loop break statements (JIT)
#define TKS_MAX_JIT_LOOP_BREAKS  (32u)

#include "tks-yac.h"



// ----
// ---- Paranoia: Make sure that JIT is disabled on 64bit platforms
// ----
#ifdef YAC_64
#ifdef TKS_JIT
#error "TKS_JIT is currently not supported on 64bit platforms! Please undefine it in the makefile."
#undef TKS_JIT
#endif // TKS_JIT
#endif // YAC_64


#ifdef DX_SAFEMODE
#define TKS_VALID(a) YAC_VALID(a)
#else
#define TKS_VALID(a) 1
#endif // DX_SAFEMODE

#undef YAC_New_String
#define YAC_New_String new _String

class TKS_Context;

// ---- AST node tracking
#ifdef TKS_TRACE_NODES
#define Dtracenode(a) _env->context->last_seen_srcloc=(a)->src_loc
#define Dtracest _env->context->last_seen_srcloc=((PTN_Node*)_st)->src_loc
#define Dtracethis _env->context->last_seen_srcloc=src_loc
#else
#define Dtracenode(a)
#define Dtracest
#define Dtracethis
#endif // TKS_TRACE_NODES

#ifdef TKS_MT
class TKS_Mutex;
class TKS_NamedMutex;
#endif
#include "YAC_Value.h"
#include "StaticList.h"
#include "command.h"
#include "YAC_String.h"
#include "TKS_Configuration.h"
#include "TKS_ExceptionType.h"
#include "TKS_Exception.h"
#include "Array.h"
#include "TKS_CachedObject.h"
#include "TKS_ObjectCache.h"
#include "TKS_Namespace.h"
#include "TKS_ActiveNamespace.h"
#include "TKX_File.h"
#include "TKX_Chapter.h"
#include "TKX_PakFile.h"
#include "YAC_StreamBase.h"
#include "TKS_SignalEntry.h"
#include "TKS_ClassTemplate.h"
#include "vmopenum.h"
#include "PTN_Env.h"
#include "YAC_IntArray.h"
#ifdef TKS_PROFILE_STATEMENTS
#include "TKS_StatementProfiler.h"
#endif
#include "TKS_ScriptEngine.h"
#include "TKS_Context.h"

#define Dtkscompiler ((TKS_Compiler*)tkscript->compiler)
////#define Dtksapplication ((TKS_Application*)tkscript->application)

#ifdef TKS_CHECKEXCEPTION
#define Dhandleexception(n) if(_env->context->b_exception_raised)_env->context->handleException(n)
#define Dhaveexception _env->context->b_exception_raised
#else
#define Dhandleexception(n)
#define Dhaveexception 0
#endif // TKS_CHECKEXCEPTION


extern const char *tks_get_rtti_name_by_id(sU8 _id);
extern sUI tks_grow_array(sUI);

enum __extcoreclasses {
  TKS_CLID_CONFIGURATION=YAC_NUM_CORE_CLASSES,
                            // script engine configuration
  TKS_CLID_ENVELOPE,        // a FloatArray (time/value couples) supporting interpolation, see yacArray*
  TKS_CLID_POOL,            // an unordered ObjectArray feat. optimized memory managment
  TKS_CLID_STACK,           // a LIFO (Last In First Out) memory for recursive funct. calls
  TKS_CLID_STRINGITERATOR,  // an iterator for string stacks (legacy code)
  TKS_CLID_TIME,            // associated with struct timeval
  TKS_CLID_LANGUAGE,        // namespace for some various tool functions
  TKS_CLID_VARIABLE,        // used to access variables in dynamically compiled script modules
  TKS_CLID_FUNCTION,        // object representation of a script function
  TKS_CLID_SCRIPT,          // a dynamically created TKScript module
  TKS_CLID_PIPE,            // POSIX2 stdin/out pipestream
  TKS_CLID_SHAREDBUFFER,    // shared memory buffer
  TKS_CLID_FORK,            // represents a "worker" script-"thread"
  TKS_CLID_EXCEPTION,       // represents a throwable exception
  TKS_CLID_THREAD,          // represents a parallel executing thread (pthreads and win32 threads are currently supported)
  TKS_CLID_MUTEX,           // represents a mutual exclusive semaphore
  TKS_CLID_CONDITION,       // boolean condition variable that is used for multithreaded notifications/barriers
  TKS_CLID_MAILBOX,         // general purpose Event MailBox
  TKS_CLID_DUMMYSTREAM,     // no-op Stream. Can be used to determine buffer sizes for real write ops.
  TKS_CLID_PROCESS,         // external process with re-routed stdin/stdout (via anonymous pipes)

  TKS_CLID_NUMINTERNALS
};

// invalid script class id
#define TKS_INVALID_SCRIPT_CLASS_ID (0xFFFFFFFFu)

// ---- protection types for class variables and methods ----
#define TKS_PROT_NONE      0
#define TKS_PROT_PUBLIC    1 // visible everywhere
#define TKS_PROT_PROTECTED 2 // visible in same or derived classes
#define TKS_PROT_PRIVATE   3 // visible in current class
#define TKS_PROT_MODULE    4 // visible in current source file
#define TKS_PROT_NSPACE    5 // visible in current namespace
#define TKS_PROT_NUM       6

// Documentation control characters (also see the "DOG" script which can convert this format to e.g. HTML)
#define CC_END        1
#define CC_MODULE     2
#define CC_CLASS      3
#define CC_FUNCTION   4
#define CC_METHOD     5
#define CC_CONSTANT   6
#define CC_SEPARATOR 10


// Exit function, implemented in api.cpp and referenced in main.cpp
extern void YAC_CALL APIC_exit(sSI _return);


// To get rid of missing delete operator warning on MSVC6
// Note on _MSC_VER:
//     VC6.0 : 1200
//     VC7.0 : 1300
//     VC7.1 : 1310
//     VC8.0 : 1400
//  (according to http://www.ailight.jp/blog/sha256/archive/2005/08/16/9348.aspx)
#ifdef _MSC_VER
#if _MSC_VER <= 1200
#pragma warning( disable: 4291 )
#endif // _MSC_VER
#endif


//
// Check for NULL pointer, delete object and set pointer to NULL
//
#define TKS_DELETE_SAFE(a) if(a) { delete a; a = NULL; } else (void)0

//
// Console output
//
#ifdef TKS_DCON
#define Dprintf     if(0);else tkscript->printf
#else
#define Dprintf    if(1);else tkscript->printf
#endif // TKS_DCON


//
// Fix value type of return values from Object to String
//   This should not be necessary anymore since Objects and Strings
//   should share the same codepaths.
//
//#define Ddubiousfixstringtype(x) (x)->fixStringType()
#define Ddubiousfixstringtype(x) (void)(x)




//
// FPU rounding mode fix
//  (When compiling with /QIfist under win32/msvc, the FPU rounding mode is undefined.
//   (seems to be set to "nearest" at program start)
//   even worse: it may change during calls to plugin functions/methods so we need to reset
//   the rounding mode after each function/method call.
//   )
//
#if defined(DX_NEED_FPUFIX) && defined(YAC_VC)
#include <float.h>
// fix FPU rounding mode to "round down" so that (floatval - (sSI)floatval) = fractional part
#define Dfixfpuflags _controlfp(_RC_DOWN, _MCW_RC)
#else
// no fix needed, create dummy code
#define Dfixfpuflags (void)(0)
#endif // DX_NEED_FPUFIX



//
//
// performance counters
//
//
/// Define to enable scanner performance counters
//#define ENABLE_PERFCOUNT_SCAN defined

#ifdef ENABLE_PERFCOUNT_SCAN
#include "TKS_PerfCount.h"
#define PC_SCAN_scanAddModule            0
#define PC_SCAN_readTokens               1
#define PC_SCAN_scan                     2
#define PC_SCAN_optimize                 3
#define PC_SCAN_scanClasses              4
#define PC_SCAN_addUniqueString          5
#define PC_SCAN_entitytree_getIDByString 6
#define PC_SCAN_findDelim                7
#define PC_SCAN_str_const_substring      8
#define PC_SCAN_nextToken                9
#define PC_SCAN_scanPrepare              10
#define PC_SCAN_scanPrepare1             11
#define PC_SCAN_scanPrepare2             12
#define PC_SCAN_scanPrepare3             13
#define PC_SCAN_NUMSAMPLES               14
#define PC_SCAN_SAMPLE_NAMES  {\
   "scanAddModule",            \
   "readTokens",               \
   "scan",                     \
   "optimize",                 \
   "scanClasses",              \
   "addUniqueString",          \
   "entitytree_getIDByString", \
   "findDelim",                \
   "str_const_substring",      \
   "nextToken",                \
   "scanPrepare",              \
   "scanPrepare1",             \
   "scanPrepare2",             \
   "scanPrepare3",             \
   NULL\
}
extern TKS_PerfCount pc_scan;
#define Dpcscan_start() pc_scan.start()
#define Dpcscan_stop() pc_scan.stop(); pc_scan.print()
#define Dpcscan_concat(a,b) a ## b
#define Dpcscan_startSample(i) pc_scan.startSample(Dpcscan_concat(PC_SCAN_, i))
#define Dpcscan_stopSample(i) pc_scan.stopSample(Dpcscan_concat(PC_SCAN_, i))
#else
#define Dpcscan_start() (void)0
#define Dpcscan_stop() (void)0
#define Dpcscan_startSample(i) (void)0
#define Dpcscan_stopSample(i) (void)0
#endif // ENABLE_PERFCOUNT_SCAN


//
//
// node allocation
//
//
#ifdef TKS_USE_NODEALLOCATOR
#include "TKS_LinearAllocator.h"
// Alloc node from within a CachedScript
#define PTN_NEW_NODE(d, a) d = (a*) node_allocator->alloc(sizeof(a)); new((a*)(d)) a
// Alloc node from outside side and use current script node allocator
#define PTN_NEW_STATIC_NODE(d, a) d = (a*) Dtkscompiler->current_module->node_allocator->alloc(sizeof(a)); new((a*)(d)) a
#else
#define PTN_NEW_NODE(d, a) d = new(std::nothrow) a
#define PTN_NEW_STATIC_NODE(d, a) d = new(std::nothrow) a
#endif // TKS_USE_NODEALLOCATOR


//
// Function/method profiler
//
#ifdef TKS_PROFILE_FUNCTIONS
/* (note) init. to 0 could be skipped but triggers GCC warnings. it's safe though
           since b_enable_function_profiling never changes after main startup */
#define Dcondbeginfunprofile sF64 __fxn_profiler_start_self=0; if(tkscript->configuration.b_enable_function_profiling) __fxn_profiler_start_self = TKS_ScriptEngine::GetMilliSecondsDouble()
#define Dcondendfunprofile(f) if(tkscript->configuration.b_enable_function_profiling) { (f)->profile_millisec += TKS_ScriptEngine::GetMilliSecondsDouble()-__fxn_profiler_start_self; (f)->profile_callcount++; } if(0);else(void)0
#else
#define Dcondbeginfunprofile
#define Dcondendfunprofile(f)
#endif // TKS_PROFILE_FUNCTION_CALLSITES


//
// For placement new (yacIteratorInit())
//
#include <new>
#define Diteratorinit(t) t*it=(t*)_it; ::new (it) t(this)


extern sBool tks_debug_called;


#endif // TKS_CORE_H__
