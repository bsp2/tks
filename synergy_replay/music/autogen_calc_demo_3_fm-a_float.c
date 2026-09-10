// auto-generated on 20260910-184015



extern void sr_handle_cycle_sample_calc_finished (unsigned int _sampleIdx);
#define HSCF(a)  sr_handle_cycle_sample_calc_finished(a)

void cycle_calc_waveform_demo_3_fm_a(float *_wfAddr) {
   // smp: "fm 1" sz=32 (0.03k) bProc=1
   HSCF(0);
   // procedural sample size: 128 (0.12k)
   // static     sample size: 0 (0k)
   // total      sample size: 128 (0.12k)
}
