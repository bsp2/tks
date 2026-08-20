/// TKS_StatementProfiler.h
///
/// (c) 2001-2026 Bastian Spiegel <bs@tkscript.de>
///     - distributed under terms of the Lesser GNU General Public License (LGPL)
///

#ifndef TKS_STATEMENTPROFILER_H__
#define TKS_STATEMENTPROFILER_H__


#ifdef TKS_PROFILE_STATEMENTS

#define synchronized


class TKS_StatementProfiler {
public:
   sUI   num_modules;

   sUI   line_count   [TKS_MAX_MODULES];
   sUI  *call_count   [TKS_MAX_MODULES];
   sF64 *millisec_sum [TKS_MAX_MODULES];
   sF64 *avg_millisec [TKS_MAX_MODULES];

#ifdef TKS_MT
   TKS_Mutex *mtx_result;
#endif // TKS_MT

public:
   TKS_StatementProfiler(void);
   ~TKS_StatementProfiler();

                void init      (void);
                void exit      (void);
                void addModule (sUI _numLines);
   synchronized void addResult (sUI _srcLoc, sF64 _milliSecDelta);

                void writeCountString    (YAC_String *_r);
                void writeMilliString    (YAC_String *_r);
                void writeAvgMilliString (YAC_String *_r);
                void writeCoverageString (YAC_String *_r);
                void writeSortedArray    (YAC_IntArray *_ia, sF64 *_ratios1st, sF64 *_ratiosParent, YAC_String*_r);
                void printResultString   (void);
};

#undef synchronized


#endif // TKS_PROFILE_STATEMENTS


#endif // TKS_STATEMENTPROFILER_H__
