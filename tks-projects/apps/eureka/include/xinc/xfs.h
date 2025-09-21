
// FrequencyShifter (adapted from Squinky Labs' sql_bootyshifter)

/*
 * Module definition (parsed by Cycle):
 *
 *  @cycle_mod      xfs
 *  @cycle_pre      // %cymod% pre-include code line
 *  @cycle_static   // %cymod% module-static test code line
 *  @cycle_init     // %cymod% init function test code line
 *  @cycle_voice    FrequencyShifter *fs_%cyid%;
 *  @cycle_new      voice->fs_%cyid% = new FrequencyShifter();
 *  @cycle_delete   // delete voice->fs_%cyid%;  // (todo) fix occasional crash
 *  @cycle_config   range 500
 *  @cycle_input    freq 0
 *  @cycle_noteon   voice->fs_%cyid%->setSampleRate(48000.0f);
 *  @cycle_noteon   voice->fs_%cyid%->init();
 *  @cycle_noteon   voice->fs_%cyid%->freqRange = %cycf_range%;
 *  @cycle_emit     %cyio% = voice->fs_%cyid%->process(%cyio%, %cyin_freq%);
 *
 */

/* #include <stdio.h> */

#include "dsp/filters/BiquadFilter.h"
#include "dsp/filters/HilbertFilterDesigner.h"
#include "sql/composites/FrequencyShifter.h"
#include "dsp/utils/ObjectCache.cpp"
#include "dsp/third-party/falco/DspFilter.cpp"
#include "dsp/filters/HilbertFilterDesigner.cpp"
