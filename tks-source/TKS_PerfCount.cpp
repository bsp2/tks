// TKS_PerfCount.cpp 
/// 
/// (c) 2009 by Bastian Spiegel <bs@tkscript.de> 
///     - distributed under terms of the GNU general public license (GPL). 
/// 

#include "tks.h"

#include "TKS_PerfCount.h"


#ifdef ENABLE_PERFCOUNT_SCAN
const char*pc_scan_names[] = PC_SCAN_SAMPLE_NAMES;
TKS_PerfCount pc_scan(PC_SCAN_NUMSAMPLES, pc_scan_names);
#endif // ENABLE_PERFCOUNT_SCAN



TKS_PerfCount::TKS_PerfCount(void) {
   samples = NULL;
   num_samples = 0;
}

TKS_PerfCount::TKS_PerfCount(sUI _numSamples, const char**_sampleNames) {
   samples = NULL;
   num_samples = 0;
   allocSamples(_numSamples, _sampleNames);
}

TKS_PerfCount::~TKS_PerfCount() {
   freeSamples();
}

void TKS_PerfCount::freeSamples(void) {
   if(NULL != samples)
   {
      delete [] samples;
      samples = NULL;
   }
}

void TKS_PerfCount::allocSamples(sUI _num, const char**_sampleNames) {
   freeSamples();

   sample_names = _sampleNames;

   samples = new TKS_PerfCountSample[_num];
   memset(samples, 0, sizeof(TKS_PerfCountSample)*_num);
   num_samples = _num;
}

void TKS_PerfCount::start(void) {
   GetPerfCount(&start_time);
}

void TKS_PerfCount::stop(void) {
   GetPerfCount(&stop_time);
}

void TKS_PerfCount::startSample(sUI _sampleIdx) {
   if(_sampleIdx < num_samples)
   {
      TKS_PerfCountSample *s = &samples[_sampleIdx];
      GetPerfCount(&s->cstart);
   }
}

void TKS_PerfCount::stopSample(sUI _sampleIdx) {
   if(_sampleIdx < num_samples)
   {
      TKS_PerfCountSample *s = &samples[_sampleIdx];
      sU64 t;
      GetPerfCount(&t);
      s->total_time += (t - s->cstart);
      s->num_calls++;
   }
}

void TKS_PerfCount::print(void) {
   Dprintf("\n[dbg] TKS_PerfCount: %u samples, total_time=%u\n", num_samples, (stop_time - start_time));
   sUI i;
   sF64 totalTime = (sF64) (stop_time - start_time);
   for(i=0; i<num_samples; i++)
   {
      TKS_PerfCountSample *s = &samples[i];
      sF64 relTime = ((sF64)s->total_time);
      //Dprintf("s->num_calls=%d\n", s->num_calls);
      //Dprintf("int relTime=%.llu\n", s->total_time);
      //Dprintf("  relTime=%G\n", relTime);
      //Dprintf("totalTime=%G\n", totalTime);
      Dprintf("[dbg]   sample[%u\"%s\"]: #calls=%u time=%.llu rel=%G\n", 
         i, 
         sample_names[i],
         s->num_calls, 
         s->total_time, 
         100.0 * (((sF64)s->total_time) / totalTime)
         );
   }
   Dprintf("\n");
}
