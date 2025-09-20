/// TKS_FunctionProfiler.h
///
/// (c) 2010 Bastian Spiegel <bs@tkscript.de>
///     - distributed under the terms of the GNU general public license (GPL).
///
#ifndef __TKS_FUNCTIONPROFILER_H__
#define __TKS_FUNCTIONPROFILER_H__


#ifdef TKS_PROFILE_FUNCTIONS

class TKS_FunctionProfiler {

protected:
   static void AppendScriptResults (YAC_String &buf, Array<const PTN_Function*>&a, const char *sortTypeName);
   static void PrintScriptResults (void);

   static void AppendNativeResults (YAC_String &buf, Array<const YAC_CommandY*>&a, const char *sortTypeName);
   static void PrintNativeResults (void);

public:
   static void PrintResults (void);
};

#endif // TKS_PROFILE_FUNCTIONS


#endif // __TKS_FUNCTIONPROFILER_H__

