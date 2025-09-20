/// config.h
///
/// (c) 2001-2013 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
/// (note) see tks-config.h for configuration of script engine features
/// (note) see tks.h for configuration of script engine limits
///
 
 
///============================================================================ 
/// 
/// * endianess (e.g. intel=little endian, motorola=big endian) 
///  
///============================================================================ 
  
// (define this in the makefile if the yac.h auto-detection fails)
//////// #define YAC_LITTLE_ENDIAN defined 
//////// #define YAC_BIG_ENDIAN defined 
 
 
///============================================================================ 
/// 
/// * JIT compiler configuration 
///  
///============================================================================ 
  
// (moved to makefile)
//////// define if you want to include the p-code compiler + native code generator 
///////////#define TKS_JIT defined 
 
//////// define if you want to use the Intel 80x86 JIT codepath 
///////////#define DX_X86 defined 
 
//////// define if you want to use the Motorola 680x0 JIT codepath (**outdated, won't compile**)
///////////#define DX_68K defined 
 
 
 
///============================================================================ 
/// 
/// * enable/disable zlib support for the gzipped TKX virtual file system 
///  
///============================================================================ 
 
// (moved to makefile)
// Define if you have the z (gzip) library (to load/save tkx files) 
//#define DX_Z defined 
 
 
 
///============================================================================ 
/// 
/// * enable/disable libdl support for the YAC plugin loader  
///  
///============================================================================ 
 
/// Define if you have the dl library (to load plugin dll's/so's) (see Makefile)
//#define HAVE_DLFCN defined 
 
 
 
///============================================================================ 
/// 
/// * multithreading support (GNU pthreads)  
///  
///============================================================================ 
 
//#define DX_NO_PTHREADS 


///============================================================================ 
/// 
/// * High-Precision timers (win32)
///  
///============================================================================ 
//#define DX_USE_FILETIME defined
#define DX_USE_PERFORMANCE_COUNTER defined

 
#define HAVE_VSNPRINTF defined
 

// the JIT compiler currently only works with 32bit systems (x86, 68k)
#ifdef YAC_64
#undef TKS_JIT
#endif

