
/*
 * Module definition (parsed by Cycle):
 *
 *  @cycle_mod      xdly
 *  @cycle_pre      #define ST_DELAY_SIZE 32768
 *  @cycle_pre      #define ST_DELAY_MASK 32767
 *  @cycle_static   // %cymod% module-static test code line
 *  @cycle_new      // %cyid% voice_new() test code line
 *  @cycle_delete   // %cyid% voice_delete() test code line
 *  @cycle_voice    XDly %cyid%;
 *  @cycle_noteon   voice->%cyid%.reset();
 *  @cycle_input    t 0.5
 *  @cycle_input    fb 0
 *  @cycle_input    wet 0.5
 *  @cycle_emit     %cyio% = voice->%cyid%.process(%cyio%, %cyin_t%, %cyin_fb%, %cyin_wet%);
 *
 */

#include "dsp/delay.h"

struct XDly : StDelay {

   float process (const float _inSmp, const float _inT, const float _inFb, const float _inDryWet) {
      push(_inSmp, _inFb);
      float out = readLinearNorm(_inT);
      /* pushRaw(_inSmp); */
      /* float out = readNearestNorm(_inT); */
      return _inSmp + (out - _inSmp) * _inDryWet;
   }
};
