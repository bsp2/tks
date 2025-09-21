/*
 * Example user-defined extension module (input amplification + optional saturation)
 *
 *  Usage:
 *    <out:
 *      sin
 *      xamp drive=1.1 sat=1
 *
 */

/*
 * Module definition (parsed by Cycle):
 *
 *  @cycle_mod      xamp
 *  @cycle_pre      // %cymod% pre-include code line
 *  @cycle_static   // %cymod% module-static test code line
 *  @cycle_new      // %cyid% voice_new() test code line
 *  @cycle_delete   // %cyid% voice_delete() test code line
 *  @cycle_voice    // %cyid% voice_t field struct test code line
 *  @cycle_noteon   // %cyid% noteon function test code line
 *  @cycle_input    drive 0.5
 *  @cycle_config   sat 0  0=off 1=on
 *  @cycle_emit     %cyio% *= %cyin_drive%;
 *  @cycle_emit     if(%cycf_sat% > 0) %cyio% = tanhf(%cyio%);
 *
 */


/* add any regular C/C++ code here
 *  (in this simple example module, all code is defined in cycle_emit)
 */
