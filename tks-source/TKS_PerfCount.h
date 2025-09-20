/// TKS_PerfCount.h 
/// 
/// (c) 2009 by Bastian Spiegel <bs@tkscript.de> 
///     - published under the terms of the GNU general public license (GPL). 
/// 
/// 

#ifndef __TKS_PERFCOUNT_H__
#define __TKS_PERFCOUNT_H__

class TKS_PerfCountSample {
public:
   sU64 cstart; // current start time
   sUI  num_calls;
   sU64 total_time;
};

class TKS_PerfCount {
public:
   TKS_PerfCountSample *samples;
   const char**sample_names;
   sUI   num_samples;
   sU64  start_time;
   sU64  stop_time;

   TKS_PerfCount(void);
   TKS_PerfCount(sUI _numSamples, const char**_sampleNames);
   ~TKS_PerfCount();

   static void GetPerfCount (sU64 *_r);

   void freeSamples   (void);
   void allocSamples  (sUI _num, const char**_sampleNames);
   void start         (void);
   void startSample   (sUI _sampleIdx);
   void stopSample    (sUI _sampleIdx);
   void stop          (void);
   void print         (void);
};


#endif // __TKS_PERFCOUNT_H__
