// 01Aug2008, 12Apr2010, 09Jul2021, 10Jul2021, 11Jul2021, 18Jun2022

// (note) [09Jul2021] sending CC is faster but does not work when all timbres listen to the same MIDI ch (but play different voices)
//         => send everthing via sysex param change messages

#include <stdio.h>
#include <stdarg.h>
#define YAC_BIGSTRING
#define YAC_PRINTF
#include <yac.h>
YAC_Host *yac_host;

#include "tkradiastools.h"

#include "ying_tkradiastools.h"

#include "ying_tkradiastools.cpp"


void YAC_CALL YAC_Init(YAC_Host *_host) {
	yac_host=_host;
   YAC_Init_tkradiastools(_host);
   // yac_host->printf("tkradiastools: init done.\n");
}

void YAC_CALL YAC_Exit(YAC_Host *_host) {
   YAC_Exit_tkradiastools(_host);
}


// State fields, updated in tkradiastools_update_program_timbre/tkradiastools_update_drumkit_timbre
static sU8 *prog_state; 
static sU8 *prog_data;
static sU8 *drumkit_state; 
static sU8 *drumkit_data;
static YAC_Buffer *out;
static YAC_Buffer *cc;
static sBool b_force;


//-----------------------------------------------------------------------------
//--                                                                         --
//--                                                                         --
//--                                                                         --
//--                                                                         --
//--                                                                         --
//--                                                                         --
//--                                                                         --
//--                                                                         --
//--                                                                         --
//-- tkradiastools_update_program_timbre                                     --
//--                                                                         --
//--                                                                         --
//--                                                                         --
//--                                                                         --
//--                                                                         --
//--                                                                         --
//--                                                                         --
//--                                                                         --
//-----------------------------------------------------------------------------
#if 0
static void change_program_parameter(sU8 _paramGroup, sU16 _paramIdLSBSubIdLSB, sS16 _paramValue) {
   
   ////trace "Korg_Radias_ChangeParameter(group="+_paramGroup+" paramId="+_paramIdLSBSubIdLSB+" value="+_paramValue+")";
   out->yacStreamWriteI8( 0xF0 );
   out->yacStreamWriteI8( 0x42 ); // Korg Id
   out->yacStreamWriteI8( 0x30 | RADIAS_DEVICE_NR );
   out->yacStreamWriteI8( 0x72 ); // Radias Id
   out->yacStreamWriteI8( RADIAS_FUN_PARAMETERCHANGE ); // 0x41
   out->yacStreamWriteI8( (_paramGroup&0xF0) | ((_paramIdLSBSubIdLSB>>8)&0x0F) ); // Parameter Id LSB
   out->yacStreamWriteI8( 0 );
   out->yacStreamWriteI8( _paramIdLSBSubIdLSB&127 ); // Parameter Sub Id MSB
   out->yacStreamWriteI8( 0 );
   out->yacStreamWriteI8( _paramValue&127 ); 
   out->yacStreamWriteI8( (_paramValue>>7)&127 );
   out->yacStreamWriteI8( 0xF7 );
   ////trace "out:";
   ////HexDump(_out, 0, _out.offset);
}
#endif // 0


static void update_program_timbre_add_pc(sU16 _paramIdSubId, sS16 _paramValue) {
   sU16 id = (_paramIdSubId>>8)&255;
   sU16 subId = _paramIdSubId&255;
   
   out->yacStreamWriteI8( 0xF0 );
   out->yacStreamWriteI8( 0x42 ); // Korg Id
   out->yacStreamWriteI8( 0x30 | RADIAS_DEVICE_NR );
   out->yacStreamWriteI8( 0x72 ); // Radias Id
   out->yacStreamWriteI8( RADIAS_FUN_PARAMETERCHANGE ); // 0x41
   out->yacStreamWriteI8( id&127 ); // Parameter Id LSB
   out->yacStreamWriteI8( (id>>7)&127 ); // Parameter Id MSB
   out->yacStreamWriteI8( subId&127 ); // Parameter Sub Id LSB
   out->yacStreamWriteI8( (subId>>7)&127 ); // Parameter Sub Id MSB
   out->yacStreamWriteI8( _paramValue&127 ); 
   out->yacStreamWriteI8( (_paramValue>>7)&127 );
   out->yacStreamWriteI8( 0xF7 );
}



static void update_program_timbre_byte(sSI _off, sSI _pcOff) {
   // 0..127
   sU8 sv = prog_state[_off];
   sU8 dv = prog_data[_off];
   if(b_force || (sv != dv))
   {
      update_program_timbre_add_pc(_pcOff, dv);
      prog_state[_off] = dv;
   }
}



static void update_program_timbre_sbyte(int _off, int _pcOff) {
   // -63..0..63
   sU8 sv = prog_state[_off];
   sU8 dv = prog_data[_off];
   if(b_force || (sv != dv))
   {
      sS16 v = dv - 64;
      // yac_host->printf("xxx update_program_timbre_sbyte: off=%d dv=%u v=%d\n", _off, dv, v);
      update_program_timbre_add_pc(_pcOff, v);
      prog_state[_off] = dv;
   }
}



#if 0
static void update_program_timbre_cc(sSI _timbre, sSI _off, sU8 _ctl) {
   //explain "Verify that controller change is really necessary and send it to the device if it is.";
   
   sU8 sv = prog_state[_off];
   sU8 dv = prog_data[_off];
   ////yac_host->printf("xxx update_program_timbre_cc off=%d ctl=%d sv=%d dv=%d\n", _off, _ctl, sv, dv);
   if(b_force || (sv != dv))
   {
      if(cc->io_offset == 0) 
      {
         cc->yacStreamWriteI8( 0xB0 | (_timbre+1) );
      }
      cc->yacStreamWriteI8( _ctl&127 );
      cc->yacStreamWriteI8( dv&127 );
      prog_state[_off] = dv;
   }
}
#endif // 0


#if 0
static void update_program_timbre_cc2(sSI _timbre, sU8 _val, sU8 _ctl) {
   //explain "Send controller change to the device.";
   
   if(cc->io_offset == 0) 
   {
      cc->yacStreamWriteI8( 0xB0 | (_timbre+1) );
   }
   cc->yacStreamWriteI8( _ctl&127 );
   cc->yacStreamWriteI8( _val&127 );
}
#endif // 0



#define FX_SIGN_UNSIGNED 0
#define FX_SIGN_SIGNED   1
#define FX_SIGN_UNUSED   2

#define _ 2
      static sU8 ifx_sign[31][20] = {
         {_, _, _, _, _, _, _, _, _, _,   _, _, _, _, _, _, _, _, _, _}, //  0: --No effect--
         {0, _, 0, 0, 0, _, _, _, _, _,   _, _, _, _, _, _, _, _, _, _}, //  1: St.Compressor
         {0, _, 0, 0, 0, 1, _, _, _, _,   _, _, _, _, _, _, _, _, _, _}, //  2: St.Limiter
         {0, _, 0, 0, 0, 1, _, _, _, _,   _, _, _, _, _, _, _, _, _, _}, //  3: St.Gate
         {0, 0, 0, 0, 0, 0, 1, 0, 0, 0,   0, 0, 1, 0, 0, 0, _, _, _, _}, //  4: St.Filter
         {0, 0, 1, 1, 0, 1, 0, 0, 1, 0,   0, 0, 0, 1, 0, 0, 0, _, _, _}, //  5: St.Wah
         {0, 0, 0, 0, 0, 0, 1, 0, 0, 1,   _, _, _, _, _, _, _, _, _, _}, //  6: St.Band EQ
         {0, 0, 0, 0, 1, 0, 0, 1, 0, 0,   1, 0, 0, 1, 0, _, _, _, _, _}, //  7: Distortion
         {0, 0, 0, 0, _, _, _, _, _, _,   _, _, _, _, _, _, _, _, _, _}, //  8: Cabinet Simulator
         {0, 0, 0, 1, 0, 0, 0, 0, 0, 1,   0, 0, 0, _, _, _, _, _, _, _}, //  9: Tube Preamp Simulator
         {0, 0, 0, 0, 0, 0, 1, 0, 0, 0,   0, 1, 0, 0, _, _, _, _, _, _}, // 10: St.Decimator
         {0, 0, 0, 0, _, _, _, _, _, _,   _, _, _, _, _, _, _, _, _, _}, // 11: Reverb
         {0, 0, 0, 0, 0, _, _, 0, 0, _,   _, _, _, _, _, _, _, _, _, _}, // 12: Early Reflections
         {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, _, _, _}, // 13: L/C/R Delay
         {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, _, _, _, _, _, _, _}, // 14: St.Delay
         {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 1, 0, 0, 0, 0, _, _}, // 15: Auto Panning Delay
         {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 1, 0, 0, 1, 0, 0, 0}, // 16: St.Auto Panning Delay
         {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   1, _, _, _, _, _, _, _, _, _}, // 17: Modulation Delay
         {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   1, _, _, _, _, _, _, _, _, _}, // 18: St.Modulation Delay
         {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, _, _}, // 19: Tape Echo
         {0, 0, 0, 1, 0, 0, 0, 1, 1, _,   _, _, _, _, _, _, _, _, _, _}, // 20: St.Chorus
         {0, 0, 0, _, _, _, _, _, _, _,   _, _, _, _, _, _, _, _, _, _}, // 21: Ensemble
         {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 1, 0, 0, 1, 0, _, _, _, _}, // 22: St.Flanger/CombFilter
         {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   1, 0, 0, 1, 0, _, _, _, _, _}, // 23: St.Phaser
         {0, 0, 0, 0, 0, 0, 1, 0, 0, 1,   _, _, _, _, _, _, _, _, _, _}, // 24: St.Tremolo
         {0, 0, 0, 1, 1, 0, 1, 0, 0, 0,   0, 1, 0, 0, 0, _, _, _, _, _}, // 25: St.Ring Modulator
         {0, 1, 1, _, _, _, _, _, _, 0,   0, 0, _, _, _, _, _, _, _, _}, // 26: Pitch Shifter
         {0, 0, 0, 0, _, 0, 0, 0, 0, 0,   _, _, _, _, _, _, _, _, _, _}, // 27: Grain Shifter
         {0, 0, 0, 0, 0, 0, 1, 0, 0, 1,   _, _, _, _, _, _, _, _, _, _}, // 28: St.Vibrato
         {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   1, 0, 0, 0, 0, 0, 0, 0, 0, _}, // 29: Rotary Speaker
         {0, 1, 0, 0, 0, 0, 0, 0, 1, 0,   0, 1, 0, 0, 0, 0, 1, 0, 0, 0}  // 30: Talking Modulator
      };

      static sU8 mfx_sign[31][20] = {
         {_, _, _, _, _, _, _, _, _, _,   _, _, _, _, _, _, _, _, _, _}, //  0: --No effect--
         {0, 0, 0, 0, 0, _, _, _, _, _,   _, _, _, _, _, _, _, _, _, _}, //  1: St.Compressor
         {0, 0, 0, 0, 0, 1, _, _, _, _,   _, _, _, _, _, _, _, _, _, _}, //  2: St.Limiter
         {0, 0, 0, 0, 0, 1, _, _, _, _,   _, _, _, _, _, _, _, _, _, _}, //  3: St.Gate
         {0, 0, 0, 0, 0, 0, 1, 0, 0, 0,   0, 0, 1, 0, 0, 0, _, _, _, _}, //  4: St.Filter
         {0, 0, 1, 1, 0, 1, 0, 0, 1, 0,   0, 0, 0, 1, 0, 0, 0, _, _, _}, //  5: St.Wah
         {0, 0, 0, 0, 0, 0, 1, 0, 0, 1,   0, 0, 1, 0, 0, 1, _, _, _, _}, //  6: 4Band EQ
         {0, 0, 0, 0, 1, 0, 0, 1, 0, 0,   1, 0, 0, 1, 0, _, _, _, _, _}, //  7: Distortion
         {0, 0, 0, 0, _, _, _, _, _, _,   _, _, _, _, _, _, _, _, _, _}, //  8: Cabinet Simulator
         {0, 0, 0, 1, 0, 0, 0, 0, 0, 1,   0, 0, 0, _, _, _, _, _, _, _}, //  9: Tube Preamp Simulator
         {0, 0, 0, 0, 0, 0, 1, 0, 0, 0,   0, 1, 0, 0, _, _, _, _, _, _}, // 10: St.Decimator
         {0, 0, 0, 0, 0, 0, 0, 1, 1, 0,   0, _, _, _, _, _, _, _, _, _}, // 11: Reverb
         {0, 0, 0, 0, 0, 1, 1, 0, 0, _,   _, _, _, _, _, _, _, _, _, _}, // 12: Early Reflections
         {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, _, _, _}, // 13: L/C/R Delay
         {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, _, _, _, _, _, _, _}, // 14: St.Delay
         {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 1, 0, 0, 0, 0, _, _}, // 15: Auto Panning Delay
         {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 1, 0, 0, 1, 0, 0, 0}, // 16: St.Auto Panning Delay
         {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   1, _, _, _, _, _, _, _, _, _}, // 17: Modulation Delay
         {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   1, _, _, _, _, _, _, _, _, _}, // 18: St.Modulation Delay
         {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, _, _}, // 19: Tape Echo
         {0, 0, 0, 1, 0, 0, 0, 1, 1, _,   _, _, _, _, _, _, _, _, _, _}, // 20: St.Chorus
         {0, 0, 0, _, _, _, _, _, _, _,   _, _, _, _, _, _, _, _, _, _}, // 21: Ensemble
         {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 1, 0, 0, 1, 0, _, _, _, _}, // 22: St.Flanger/CombFilter
         {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   1, 0, 0, 1, 0, _, _, _, _, _}, // 23: St.Phaser
         {0, 0, 0, 0, 0, 0, 1, 0, 0, 1,   _, _, _, _, _, _, _, _, _, _}, // 24: St.Tremolo
         {0, 0, 0, 1, 1, 0, 1, 0, 0, 0,   0, 1, 0, 0, 0, _, _, _, _, _}, // 25: St.Ring Modulator
         {0, 1, 1, 0, 0, 0, 0, 0, 0, 0,   0, 0, _, _, _, _, _, _, _, _}, // 26: St.Pitch Shifter
         {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   1, _, _, _, _, _, _, _, _, _}, // 27: St.Grain Shifter
         {0, 0, 0, 0, 0, 0, 1, 0, 0, 1,   _, _, _, _, _, _, _, _, _, _}, // 28: St.Vibrato
         {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   1, 0, 0, 0, 0, 0, 0, 0, 0, _}, // 29: Rotary Speaker
         {0, 1, 0, 0, 0, 0, 0, 0, 1, 0,   0, 1, 0, 0, 0, 0, 1, 0, 0, 0}  // 30: Talking Modulator
      };
#undef _

static void update_program_timbre_ifxparam(sSI _off, sSI _pcOff, sU8 _fxType, sU8 _paramNr) {
   switch(ifx_sign[_fxType][_paramNr])
   {
      case FX_SIGN_UNSIGNED:
         update_program_timbre_byte(_off, _pcOff);
         break;

      case FX_SIGN_SIGNED:
         update_program_timbre_sbyte(_off, _pcOff);
         break;

      default:
      case FX_SIGN_UNUSED:
         break;
   }
}

static void update_ifx(sSI off, sSI pcOff) {
   // Fx SW + type
   sU8 sv = prog_state[off];
   sU8 dv = prog_data[off];
   // yac_host->printf("\n xxx Fx%i SW+type: off=%u sv=0x%02x dv=0x%02x\n", i, off, sv, dv);
   if(b_force || ( (sv&0x80) != (dv&0x80) ) )
   {
      // Fx SW
      update_program_timbre_add_pc(pcOff+0x00, (dv&0x80)>>7);
      ////printf("      Fx%i SW: pcOff=0x%04x, val=0x%02x\n", i, pcOff+0x0900, (dv&0x80)>>7);
   }
   prog_state[off] = dv;
   if( (dv&0x80) ) // Fx enabled ?
   {
      sU8 fxType = (dv&0x3f); 
      if(fxType>30) // should never happen (OS update ??)
      {
         fxType=30; 
      }

      if(b_force || ( (sv&0x3F) != (dv&0x3f) ) )
      {
         // Fx type
         update_program_timbre_add_pc(pcOff+0x01, (dv&0x3f));
         // yac_host->printf("      fx%i.type=0x%02x\n", i, (dv&0x3f));
      }
      off++;

      // dummy byte
      off++;
         
      // Fx knob1 assign
      sv = prog_state[off];
      dv = prog_data[off];
      if(b_force || ( (sv&0x3f) != (dv&0x3f) ) )
      {
         update_program_timbre_add_pc(pcOff+0x02, (dv&0x3f));
      }
      off++;
         
      // Fx knob2 assign
      sv = prog_state[off];
      dv = prog_data[off];
      if(b_force || ((sv&0x3f) != (dv&0x3f)))
      {
         update_program_timbre_add_pc(pcOff+0x03, (dv&0x3f));
      }
      off++;

      // Fx Parameters 1..20
      for(sSI paramIdx = 0; paramIdx < 20; paramIdx++)
      {
         update_program_timbre_ifxparam(off, pcOff+0x10+paramIdx, fxType, paramIdx);
         off++;
      }
         
   }
}

static void update_program_timbre_mfxparam(sSI _off, sSI _pcOff, sU8 _fxType, sU8 _paramNr) {
   switch(mfx_sign[_fxType][_paramNr])
   {
      case FX_SIGN_UNSIGNED:
         update_program_timbre_byte(_off, _pcOff);
         break;

      case FX_SIGN_SIGNED:
         update_program_timbre_sbyte(_off, _pcOff);
         break;

      default:
      case FX_SIGN_UNUSED:
         break;
   }
}

static void update_mfx(sSI off, sSI pcOff) {
   // Fx SW + type
   sU8 sv = prog_state[off];
   sU8 dv = prog_data[off];
   // yac_host->printf("\n xxx Fx%i SW+type: off=%u sv=0x%02x dv=0x%02x\n", i, off, sv, dv);
   if(b_force || ( (sv&0x80) != (dv&0x80) ) )
   {
      // Fx SW
      update_program_timbre_add_pc(pcOff+0x00, (dv&0x80)>>7);
      ////printf("      Fx%i SW: pcOff=0x%04x, val=0x%02x\n", i, pcOff+0x0900, (dv&0x80)>>7);
   }
   prog_state[off] = dv;
   if( (dv&0x80) ) // Fx enabled ?
   {
      sU8 fxType = (dv&0x3f); 
      if(fxType>30) // should never happen (OS update ??)
      {
         fxType=30; 
      }

      if(b_force || ( (sv&0x3F) != (dv&0x3f) ) )
      {
         // Fx type
         update_program_timbre_add_pc(pcOff+0x01, (dv&0x3f));
         // yac_host->printf("      fx%i.type=0x%02x\n", i, (dv&0x3f));
      }
      off++;

      // Fx knob assign
      sv = prog_state[off];
      dv = prog_data[off];
      if(b_force || ( (sv&0x3f) != (dv&0x3f) ) )
      {
         update_program_timbre_add_pc(pcOff+0x02, (dv&0x3f));
      }
      off++;
         
      // Fx Parameters 1..20
      for(sSI paramIdx = 0; paramIdx < 20; paramIdx++)
      {
         update_program_timbre_mfxparam(off, pcOff+0x0100+paramIdx, fxType, paramIdx);
         off++;
      }
         
   }
}

static void update_vocoder(sSI off, sSI pcOff) {
   sU8 sv = prog_state[off];
   sU8 dv = prog_data[off];
   if(b_force || ( (sv&0x80) != (dv&0x80) ) )
   {
      // Vocoder SW
      update_program_timbre_add_pc(pcOff+0x00, (dv&0x80)>>7);
   }
   if(b_force || ( (sv&0x40) != (dv&0x40) ) )
   {
      // Vocoder Source
      update_program_timbre_add_pc(pcOff+0x05, (dv&0x40)>>6);
   }
   if(b_force || ( (sv&0x20) != (dv&0x20) ) )
   {
      // Vocoder HPF Gate
      update_program_timbre_add_pc(pcOff+0x04, (dv&0x20)>>5);
   }
   if(b_force || ( (sv&0x10) != (dv&0x10) ) )
   {
      // Vocoder FormantDataPlay
      update_program_timbre_add_pc(pcOff+0x03, (dv&0x10)>>4);
   }
   if(b_force || ( (sv&0x0C) != (dv&0x0C) ) )
   {
      // Vocoder Input Source2
      update_program_timbre_add_pc(pcOff+0x02, (dv&0x0C)>>2);
   }
   if(b_force || ( (sv&0x03) != (dv&0x03) ) )
   {
      // Vocoder Select
      update_program_timbre_add_pc(pcOff+0x01, (dv&0x03)   );
   }
   prog_state[off] = dv;

   // Gate Sens
   update_program_timbre_byte(off+1, pcOff+0x06);

   // Threshold
   update_program_timbre_byte(off+2, pcOff+0x07);

   // HPF Level
   update_program_timbre_byte(off+3, pcOff+0x08);

   // Direct Level
   update_program_timbre_byte(off+4, pcOff+0x09);

   // Inst Bus Level
   update_program_timbre_byte(off+5, pcOff+0x0A);

   // Inst Synth Level
   update_program_timbre_byte(off+6, pcOff+0x0B);

   // Vocoder Level
   update_program_timbre_byte(off+7, pcOff+0x0C);

   for(sUI bandIdx = 0u; bandIdx < 16u; bandIdx++)
   {
      // Band pan
      // update_program_timbre_sbyte(off+8+(bandIdx*2u), pcOff+0x0200u+(bandIdx*2u)+0x00);
      update_program_timbre_byte(off+8+(bandIdx*2u), pcOff+0x0200u+(bandIdx*2u)+0x00);

      // Band level
      update_program_timbre_byte(off+9+(bandIdx*2u), pcOff+0x0200u+(bandIdx*2u)+0x01);
   }

   sv = prog_state[off+40];
   dv = prog_data[off+40];
   if(b_force || ( (sv&0x70) != (dv&0x70) ) )
   {
      // Formant Shift
      update_program_timbre_add_pc(pcOff+0x0301, (dv&0x70)>>4);
      // yac_host->printf("xxx formatshift val=%u\n", (dv&0x70)>>4);
   }
   if(b_force || ( (sv&0x0F) != (dv&0x0F) ) )
   {
      // Fc Mod Source
      update_program_timbre_add_pc(pcOff+0x0300, (dv&0x0F)   );
   }
   prog_state[off+40] = dv;

   // Cutoff Offset
   update_program_timbre_sbyte(off+41, pcOff+0x0310u);

   // Resonance
   update_program_timbre_byte(off+42, pcOff+0x0311u);

   // Fc Mod Intensity
   update_program_timbre_sbyte(off+43, pcOff+0x0312u);

   // E.F.Sens
   update_program_timbre_byte(off+44, pcOff+0x0313u);

   sv = prog_state[off+45];
   dv = prog_data[off+45];
   if(b_force || ( (sv&0xF0) != (dv&0xF0) ) )
   {
      // FM Select
      update_program_timbre_add_pc(pcOff+0x0318, (dv&0xF0)>>4);
   }
   if(b_force || ( (sv&0x03) != (dv&0x03) ) )
   {
      // // Timbre 4 Select
      // update_program_timbre_add_pc(pcOff+0x0317, (dv&0x08)>>3);

      // // Timbre 3 Select
      // update_program_timbre_add_pc(pcOff+0x0316, (dv&0x04)>>2);

      // // Timbre 2 Select
      // update_program_timbre_add_pc(pcOff+0x0315, (dv&0x02)>>1);

      // // Timbre 1 Select
      // update_program_timbre_add_pc(pcOff+0x0314, (dv&0x01)   );
      
      // InSrc1 Timbre Select
      update_program_timbre_add_pc(pcOff+0x0314, (dv&0x03)   );
   }
   prog_state[off+45] = dv;
}

static void update_program_timbre(sSI _timbre, sBool _bSendFX) {

   // Incrementally update current Radias program state; just send the changes.
   ////int bCCRS = false;
   int pcOff = 0x1000 * (_timbre + 1); // Parameter change offset
   int off = 48 + (RADIAS_TIMBREDATASIZE * _timbre); // byte offset in program dump (decoded!)
   sU8 sv;
   sU8 dv;
   
   // TABLE 2 : TIMBRE PARAMETER
   
   // Timbre SW, Sub Output select, Arpg/Sseq Assign
   sv = prog_state[off];
   dv = prog_data[off];
   // yac_host->printf("xxx timbre1.sw off=%u sv=%02x dv=%02x\n", off, sv, dv);
   if(b_force || (sv != dv))
   {
      if(b_force || ( (sv&0x80) != (dv&0x80) ) ) // Timbre SW
      {
         update_program_timbre_add_pc(pcOff+0x0000, (dv&0x80)>>7);
      }
      if(b_force || ( (sv&0x10) != (dv&0x10) ) ) // Sub Output select
      {
         update_program_timbre_add_pc(pcOff+0x0002, (dv&0x10)>>4);
      }
      if(b_force || ( (sv&0x0C) != (dv&0x0C) ) ) // Arpg/Sseq Assign
      {
         update_program_timbre_add_pc(pcOff+0x0001, (dv&0x0C)>>2);
      }
      prog_state[off] = dv;
   }
   off++;
   
   // Dummy byte (1)
   off++;
   
   // Out Balance
   update_program_timbre_byte(off, pcOff+0x0004);
   off++;
   
   // Dummy byte (3)
   off++;
   
   // Midi Ch
   sv = prog_state[off];
   dv = prog_data[off];
   if(b_force || (sv != dv))
   {
      update_program_timbre_byte(off, pcOff+0x0100);
   }
   off++;
   
   // Pitch bend, .. filters
   sv = prog_state[off];
   dv = prog_data[off];
   if(b_force || (sv != dv))
   {
      if(b_force || ( (sv&0x80) != (dv&0x80) ) ) // Pitchbend filter
      {
         update_program_timbre_add_pc(pcOff+0x010F, (dv&0x80)>>7);
      }
      if(b_force || ( (sv&0x40) != (dv&0x40) ) ) // ControlChg filter
      {
         update_program_timbre_add_pc(pcOff+0x010E, (dv&0x40)>>6);
      }
      if(b_force || ( (sv&0x20) != (dv&0x20) ) ) // Foot Pdl&Sw filter
      {
         update_program_timbre_add_pc(pcOff+0x010D, (dv&0x20)>>5);
      }
      if(b_force || ( (sv&0x10) != (dv&0x10) ) ) // Mod wheel filter
      {
         update_program_timbre_add_pc(pcOff+0x010C, (dv&0x10)>>4);
      }
      if(b_force || ( (sv&0x08) != (dv&0x08) ) ) // PortamentSW filter
      {
         update_program_timbre_add_pc(pcOff+0x010B, (dv&0x08)>>3);
      }
      if(b_force || ( (sv&0x04) != (dv&0x04) ) ) // Damper Pdl filter
      {
         update_program_timbre_add_pc(pcOff+0x010A, (dv&0x04)>>2);
      }
      if(b_force || ( (sv&0x02) != (dv&0x02) ) ) // Aftertouch filter
      {
         update_program_timbre_add_pc(pcOff+0x0109, (dv&0x02)>>1);
      }
      if(b_force || ( (sv&0x01) != (dv&0x01) ) ) // Program.Chg filter
      {
         update_program_timbre_add_pc(pcOff+0x0108, (dv&0x01)>>0);
      }
      prog_state[off] = dv;
   }
   off++;
   
   // Key Window Bottom
   update_program_timbre_byte(off, pcOff+0x0200);
   off++;
   
   // Key Window Top
   update_program_timbre_byte(off, pcOff+0x0201);
   off++;
   
   // Unison switch, Unison Voice
   sv = prog_state[off];
   dv = prog_data[off];
   if(b_force || (sv != dv))
   {
      if(b_force || ( (sv&0x80) != (dv&0x80) ) ) // Unison SW
      {
         update_program_timbre_add_pc(pcOff+0x0300, (dv&0x80)>>7);
         // update_program_timbre_cc2(_timbre, ((dv&0x80)>>7)*127, 3);
      }
      if(b_force || ( (sv&0x07) != (dv&0x07) ) ) // Unison Voice
      {
         update_program_timbre_add_pc(pcOff+0x0301, (dv&0x07)>>0);
      }
      prog_state[off] = dv;
   }
   off++;
   
   // Unison Detune
   update_program_timbre_byte(off, pcOff+0x0302);
   off++;
   
   // Unison Spread
   update_program_timbre_byte(off, pcOff+0x0303);
   off++;
   
   // Bend Range
   update_program_timbre_sbyte(off, pcOff+0x0400);
   off++;
   
   // Portamento Time
   update_program_timbre_byte(off, pcOff+0x0401);
   // update_program_timbre_cc(_timbre, off, 5);
   off++;
   
   // Portamento Curve, ..
   sv = prog_state[off];
   dv = prog_data[off];
   if(b_force || (sv != dv))
   {
      if(b_force || ( (sv&0x07) != (dv&0x07) ) ) // Portamento Curve
      {
         update_program_timbre_add_pc(pcOff+0x0402, (dv&0x07)>>7);
      }
      prog_state[off] = dv;
   }
   off++;
   
   // Dummy byte (14)
   off++;
   
   // Dummy byte (15)
   off++;
   


   //
   //
   // TABLE 7 : TIMBRE PROGRAM PARAMETER (16..RADIAS_TIMBREDATASIZE(228))
   //
   //
   pcOff += 0x0500;


   
   // Program name (SKIP)
   off += 12;
   
   // Dummy bytes (12..15)
   off += 4;
   
   ////trace "xxx off = "+off;
   
   // Voice assign mode, trigger mode, priority
   sv = prog_state[off];
   dv = prog_data[off];
   if(b_force || (sv != dv))
   {
      if(b_force || ( (sv&0x80) != (dv&0x80) ) ) // Assign Mode  (Mono/Poly)
      {
         update_program_timbre_add_pc(pcOff+0x0010, (dv&0x80)>>7);
      }
      if(b_force || ( (sv&0x40) != (dv&0x40) ) ) // Trigger Mode (Single/Multi)
      {
         update_program_timbre_add_pc(pcOff+0x0011, (dv&0x40)>>6);
      }
      if(b_force || ( (sv&0x03) != (dv&0x03) ) ) // Key priority (Last, Low, High)
      {
         update_program_timbre_add_pc(pcOff+0x0012, (dv&0x03)>>0);
      }
      prog_state[off] = dv;
   }
   off++;
   
   // Dummy byte (17)
   off++;
   
   // Analog Tuning
   update_program_timbre_byte(off, pcOff+0x0013);
   off++;
   
   // Transpose
   update_program_timbre_sbyte(off, pcOff+0x0014);
   off++;
   
   // Detune
   update_program_timbre_sbyte(off, pcOff+0x0015);
   off++;
   
   // Vibrato Int
   update_program_timbre_sbyte(off, pcOff+0x0016);
   off++;
   
   // Osc1 Bus Select, Mod, Wave Type
   sv = prog_state[off];
   dv = prog_data[off];
   if(b_force || (sv != dv))
   {
      if(b_force || ( (sv&0x40) != (dv&0x40) ) ) // Bus Select
      {
         update_program_timbre_add_pc(pcOff+0x001D, (dv&0x40)>>6);
      }
      if(b_force || ( (sv&0x30) != (dv&0x30) ) ) // Osc Mod (Waveform, Cross, Unison, VPM)
      {
         update_program_timbre_add_pc(pcOff+0x0018, (dv&0x30)>>4);
         // //update_program_timbre_cc2(_timbre, ((dv&0x30)>>4)<<4, 9);
      }
      if(b_force || ( (sv&0x0F) != (dv&0x0F) ) ) // Wave Type
      {
         update_program_timbre_add_pc(pcOff+0x0017, (dv&0x0F)>>0);
         // update_program_timbre_cc2(_timbre, (dv&0x0F)<<4, 8);
      }
      prog_state[off] = dv;
   }
   off++;
   
   // Osc1 Waveform CTRL1
   //update_program_timbre_byte(prog_data, off, pcOff+0x0019);
   sU8 waveType = (prog_state[off-1]&0x0F);
   sU8 oscMod = (prog_state[off-1]&0x30)>>4;
   if( (waveType == 7) || ((waveType <=3) && (oscMod==3)) )
   {
      // param becomes signed when wavetype == 7 (DrumPCM) or (wavetype <= 3 (Sine) and Osc Mod==VPM). argh.
      update_program_timbre_sbyte(off, pcOff+0x0019); 
   }
   else
   {
      update_program_timbre_byte(off, pcOff+0x0019);
   }
   off++;
   
   // Osc1 Waveform CTRL2
   //update_program_timbre_byte(prog_data, off, pcOff+0x001A);
   if( (waveType >= 4) || ((waveType <= 3) && (oscMod==3)) )
   {
      // param becomes signed when wavetype is >=4 (Formant) or (wavetype <= 3 (Sine) and Osc Mod==VPM). argh.
      update_program_timbre_sbyte(off, pcOff+0x001A); 
   }
   else
   {
      update_program_timbre_byte(off, pcOff+0x001A);
      // update_program_timbre_cc(_timbre, off, 17);
   }
   off++;
   
   // Osc1 DWGS Wave Select
   update_program_timbre_byte(off, pcOff+0x001B);
   off++;
   
   // Osc1 PCM Wave Select
   update_program_timbre_byte(off, pcOff+0x001C);
   off++;
   
   // Osc2 Mod Select, Wave Type
   sv = prog_state[off];
   dv = prog_data[off];
   if(b_force || (sv != dv))
   {
      if(b_force || ( (sv&0x30) != (dv&0x30) ) ) // Mod Select
      {
         // $00..$1F : OFF
         // $20..$3F : RING
         // $40..$5F : SYNC
         // $60..$7F : RING+SYNC
         update_program_timbre_add_pc(pcOff+0x0021, (dv&0x30)>>4);
         // update_program_timbre_cc2(_timbre, ((dv&0x30)>>4)*0x20, 19);
      }
      if(b_force || ( (sv&0x03) != (dv&0x03) ) ) // Wave Type (Saw, Squ, Tri, Sin)
      {
         // $00..$1F : Saw
         // $20..$3F : Square
         // $40..$5F : Tri
         // $60..$7F : Sin
         update_program_timbre_add_pc(pcOff+0x0020, (dv&0x03)>>0);
         // update_program_timbre_cc2(_timbre, (dv&0x03)*0x20, 18); 
      }
      prog_state[off] = dv;
   }
   off++;
   
   // Osc2 Semitone
   update_program_timbre_sbyte(off, pcOff+0x0022);
   // update_program_timbre_cc(_timbre, off, 20);
   off++;
   
   // Osc2 Tune
   update_program_timbre_sbyte(off, pcOff+0x0023);
   // update_program_timbre_cc(_timbre, off, 21);
   off++;
   
   // Mixer Osc1 Level
   update_program_timbre_byte(off, pcOff+0x0028);
   // update_program_timbre_cc(_timbre, off, 23);
   off++;
   
   // Mixer Osc2 Level
   update_program_timbre_byte(off, pcOff+0x0029);
   // update_program_timbre_cc(_timbre, off, 24);
   off++;
   
   // Mixer Noise Level
   update_program_timbre_byte(off, pcOff+0x002A);
   // update_program_timbre_cc(_timbre, off, 25);
   off++;
   
   // Filter Link SW, Filter2 Type, Filter Routing. Important: Send Filter Routing First!!!!!!
   sv = prog_state[off];
   dv = prog_data[off];
   if(b_force || (sv != dv))
   {
      if(b_force || ( (sv&0x03) != (dv&0x03) ) ) // Filter Routing (Single, Serial, Parallel, Indiv)
      {
         // For Synth Timbres:
         // $00..$1F : OFF
         // $20..$3F : SERIAL
         // $40..$5F : PARALLEL
         // $60..$7F : INDIVIDUAL
         //
         // For Vocoder Timbres: (TODO)
         // $00..$3F : Audio
         // $40..$7F : Formant Play
         update_program_timbre_add_pc(pcOff+0x0030, (dv&0x03)>>0);
         // update_program_timbre_cc2(_timbre, (dv&0x03)*0x20, 26);
      }
      if(b_force || ( (sv&0x80) != (dv&0x80) ) ) // Filter Link SW
      {
         update_program_timbre_add_pc(pcOff+0x0037, (dv&0x80)>>7);
      }
      if(b_force || ( (sv&0x30) != (dv&0x30) ) ) // Filter 2 Type (LPF, HPF, BPF, COMBO)
      {
         // Synth Timbre:
         // $00..$1F : LPF
         // $20..$3F : HPF
         // $40..$5F : BPF
         // $60..$7F : COMB
         //
         // Vocoder Timbre: (Formant Shift, TODO)
         // $00..$1F : +1
         // $20..$3F : +2
         // $40..$5F : -1
         // $60..$7F : -2
         update_program_timbre_add_pc(pcOff+0x0040, (dv&0x30)>>4);
         // update_program_timbre_cc2(_timbre, ((dv&0x30)>>4)*0x20, 29);
      }
      prog_state[off] = dv;
   }
   off++;
   
   // Filter1 Balance
   update_program_timbre_byte(off, pcOff+0x0031);
   // update_program_timbre_cc(_timbre, off, 27);
   off++;
   
   // Filter1 Cutoff
   update_program_timbre_byte(off, pcOff+0x0032);
   // update_program_timbre_cc(_timbre, off, 74);
   off++;
   
   // Filter1 Resonance
   update_program_timbre_byte(off, pcOff+0x0033);
   // update_program_timbre_cc(_timbre, off, 71);
   off++;
   
   // F1 EG1 Intensity
   update_program_timbre_sbyte(off, pcOff+0x0034);
   // update_program_timbre_cc(_timbre, off, 79);
   off++;
   
   // F1 Keyboard Track
   update_program_timbre_sbyte(off, pcOff+0x0035);
   // update_program_timbre_cc(_timbre, off, 28);
   off++;
   
   // F1 Velocity Sens
   update_program_timbre_sbyte(off, pcOff+0x0036);
   off++;
   
   // Filter2 Cutoff
   update_program_timbre_byte(off, pcOff+0x0042);
   // update_program_timbre_cc(_timbre, off, 30);
   off++;
   
   // Filter2 Resonance
   update_program_timbre_byte(off, pcOff+0x0043);
   // update_program_timbre_cc(_timbre, off, 68);
   off++;
   
   // F2 EG1 Intensity
   update_program_timbre_sbyte(off, pcOff+0x0044);
   // update_program_timbre_cc(_timbre, off, 69);
   off++;
   
   // F2 Keyboard Track
   update_program_timbre_sbyte(off, pcOff+0x0045);
   // update_program_timbre_cc(_timbre, off, 82);
   off++;
   
   // F2 Velocity Sens
   update_program_timbre_sbyte(off, pcOff+0x0046);
   off++;
   
   // Amp Level
   update_program_timbre_byte(off, pcOff+0x0050);
   // update_program_timbre_cc(_timbre, off, 7);
   off++;
   
   // Amp Drive/WS, Mod Type
   sv = prog_state[off];
   dv = prog_data[off];
   if(b_force || (sv != dv))
   {
      if(b_force || ( (sv&0x70) != (dv&0x70) ) ) // Drive/WS Position (Pre Flt1, Pre Amp)
      {
         update_program_timbre_add_pc(pcOff+0x0052, (dv&0x70)>>4);
      }
      if(b_force || ( (sv&0x03) != (dv&0x03) ) ) // Mod Type (Off/Driver, Waveshape)
      {
         // Drive/WS SW ?!!
         update_program_timbre_add_pc(pcOff+0x0053, (dv&0x03)>>0);
         // Synth Timbre:
         //   $00..$29 : OFF
         //   $2A..$54 : Drive
         //   $55..$7F : WaveShape
         //
         // Vocoder Timbre: (HPF Gate, TODO)
         //   $00..$3F : Disable
         //   $40..$7F : Enable
         // switch((dv&0x03)>>0)
         // {
         // default:
         // case 0:
         //    update_program_timbre_cc2(_timbre, 0x00, 84);
         //    break;
         // case 1:
         //    update_program_timbre_cc2(_timbre, 0x2A, 84);
         //    break;
         // case 2:
         //    update_program_timbre_cc2(_timbre, 0x55, 84);
         //    break;
         // }
      }
      prog_state[off] = dv;
   }
   off++;
   
   // Amp WaveShape Type
   sv = prog_state[off];
   dv = prog_data[off];
   if(b_force || (sv != dv))
   {
      if(b_force || ( (sv&0x0F) != (dv&0x0F) ) ) // WaveShape Type
      {
         update_program_timbre_add_pc(pcOff+0x0051, (dv&0x0F)>>0);
      }
      prog_state[off] = dv;
   }
   off++;
   
   // Amp Driver/Shaper
   update_program_timbre_byte(off, pcOff+0x0054);
   // update_program_timbre_cc(_timbre, off, 83);
   off++;
   
   // Amp PanPot   xxx signed ?
   update_program_timbre_byte(off, pcOff+0x0055);
   // update_program_timbre_cc(_timbre, off, 10);
   off++;
   
   // Amp KeyTrack
   update_program_timbre_sbyte(off, pcOff+0x0056);
   off++;
   
   // Amp Punch Level
   update_program_timbre_byte(off, pcOff+0x0057);
   off++;
   
   // EG1 Attack Time
   update_program_timbre_byte(off, pcOff+0x0060);
   // update_program_timbre_cc(_timbre, off, 85);
   off++;
   
   // EG1 Decay Time
   update_program_timbre_byte(off, pcOff+0x0061);
   // update_program_timbre_cc(_timbre, off, 86);
   off++;
   
   // EG1 Sustain Level
   update_program_timbre_byte(off, pcOff+0x0062);
   // update_program_timbre_cc(_timbre, off, 87);
   off++;
   
   // EG1 Release Time
   update_program_timbre_byte(off, pcOff+0x0063);
   // update_program_timbre_cc(_timbre, off, 88);
   off++;
   
   // EG1 Curve
   sv = prog_state[off];
   dv = prog_data[off];
   if(b_force || (sv != dv))
   {
      if(b_force || ( (sv&0x03) != (dv&0x03) ) ) // Curve
      {
         update_program_timbre_add_pc(pcOff+0x0064, (dv&0x03)>>0);
      }
      prog_state[off] = dv;
   }
   off++;
   
   // EG1 LevelVelocitySens
   update_program_timbre_sbyte(off, pcOff+0x0065);
   off++;
   
   // EG1 TimeVelocitySens
   update_program_timbre_sbyte(off, pcOff+0x0066);
   off++;
   
   // EG1 TimeKeyTrack
   update_program_timbre_sbyte(off, pcOff+0x0067);
   off++;
   
   // EG2 Attack Time
   update_program_timbre_byte(off, pcOff+0x0070);
   // update_program_timbre_cc(_timbre, off, 73);
   off++;
   
   // EG2 Decay Time
   update_program_timbre_byte(off, pcOff+0x0071);
   // update_program_timbre_cc(_timbre, off, 75);
   off++;
   
   // EG2 Sustain Level
   update_program_timbre_byte(off, pcOff+0x0072);
   // update_program_timbre_cc(_timbre, off, 70);
   off++;
   
   // EG2 Release Time
   update_program_timbre_byte(off, pcOff+0x0073);
   // update_program_timbre_cc(_timbre, off, 72);
   off++;
   
   // EG2 Curve
   sv = prog_state[off];
   dv = prog_data[off];
   if(b_force || (sv != dv))
   {
      if(b_force || ( (sv&0x03) != (dv&0x03) ) ) // Curve
      {
         update_program_timbre_add_pc(pcOff+0x0074, (dv&0x03)>>0);
      }
      prog_state[off] = dv;
   }
   off++;
   
   // EG2 LevelVelocitySens
   update_program_timbre_sbyte(off, pcOff+0x0075);
   off++;
   
   // EG2 TimeVelocitySens
   update_program_timbre_sbyte(off, pcOff+0x0076);
   off++;
   
   // EG2 TimeKeyTrack
   update_program_timbre_sbyte(off, pcOff+0x0077);
   off++;
   
   // EG3 Attack Time
   update_program_timbre_byte(off, pcOff+0x0080);
   off++;
   
   // EG3 Decay Time
   update_program_timbre_byte(off, pcOff+0x0081);
   off++;
   
   // EG3 Sustain Level
   update_program_timbre_byte(off, pcOff+0x0082);
   off++;
   
   // EG3 Release Time
   update_program_timbre_byte(off, pcOff+0x0083);
   off++;
   
   // EG3 Curve
   sv = prog_state[off];
   dv = prog_data[off];
   if(b_force || (sv != dv))
   {
      if(b_force || ( (sv&0x03) != (dv&0x03) ) ) // Curve
      {
         update_program_timbre_add_pc(pcOff+0x0084, (dv&0x03)>>0);
      }
      prog_state[off] = dv;
   }
   off++;
   
   // EG3 LevelVelocitySens
   update_program_timbre_sbyte(off, pcOff+0x0085);
   off++;
   
   // EG3 TimeVelocitySens
   update_program_timbre_sbyte(off, pcOff+0x0086);
   off++;
   
   // EG3 TimeKeyTrack
   update_program_timbre_sbyte(off, pcOff+0x0087);
   off++;
   
   // LFO1 Wave
   sv = prog_state[off];
   dv = prog_data[off];
   if(b_force || (sv != dv))
   {
      if(b_force || ( (sv&0x0F) != (dv&0x0F) ) ) // Wave
      {
         update_program_timbre_add_pc(pcOff+0x0090, (dv&0x0F)>>0);
         // $00..$1F : Saw
         // $20..$3F : Squ
         // $40..$5F : Tri
         // $60..$7F : S&H
         // update_program_timbre_cc2(_timbre, (dv&0x0F)*0x20, 89);
      }
      prog_state[off] = dv;
   }
   off++;
   
   // LFO1 Shape
   update_program_timbre_sbyte(off, pcOff+0x0091);
   off++;
   
   // LFO1 Frequency
   // When Tempo Sync is "OFF".
   //   00~07 : 0.01, 0.02, 0.03, 0.04, 0.05, 0.06, 0.07, 0.08,
   //   08~0F : 0.09, 0.10, 0.11, 0.12, 0.13, 0.14, 0.15, 0.16,
   //   10~17 : 0.17, 0.18, 0.19, 0.20, 0.21, 0.22, 0.23, 0.24,
   //   18~1F : 0.25, 0.29, 0.33, 0.42, 0.50, 0.58, 0.67, 0.75,
   //   20~27 : 0.83, 0.92, 1.00, 1.13, 1.25, 1.38, 1.50, 1.63,
   //   28~2F : 1.75, 1.88, 2.00, 2.13, 2.25, 2.38, 2.50, 2.63,
   //   30~37 : 2.75, 2.88, 3.00, 3.13, 3.25, 3.38, 3.50, 3.63,
   //   38~3F : 3.75, 3.88, 4.00, 4.13, 4.25, 4.38, 4.50, 4.63,
   //   40~47 : 4.75, 4.88, 5.00, 5.25, 5.50, 5.75, 6.00, 6.25,
   //   48~4F : 6.50, 6.75, 7.00, 7.25, 7.50, 7.75, 8.00, 8.25,
   //   50~57 : 8.50, 8.75, 9.00, 9.25, 9.50, 9.75, 10.0, 10.5,
   //   58~5F : 11.0, 11.5, 12.0, 12.5, 13.0, 13.5, 14.0, 14.5,
   //   60~67 : 15.0, 16.0, 17.0, 18.0, 19.0, 20.0, 21.5, 23.0,
   //   68~6F : 24.5, 26.0, 27.5, 29.0, 31.0, 33.0, 35.0, 37.0,
   //   70~77 : 39.0, 41.0, 44.0, 47.0, 50.0, 53.0, 57.0, 61.0,
   //   78~7F : 65.0, 70.0, 75.0, 80.0, 85.0, 90.0, 95.0, 100.0
   //
   // When Tempo Sync is "ON".
   //   00~03 : 8/1 24~2B : 1/2 4C~53 : 1/6 74~7B : 1/32
   //   04~0B : 4/1 2C~33 : 3/8 54~5B : 1/8 7C~7F : 1/64
   //   0C~13 : 2/1 34~3B : 1/3 5C~63 : 1/12
   //   14~1B : 1/1 3C~43 : 1/4 64~6B : 1/16
   //   1C~23 : 3/4 44~4B : 3/16 6C~73 : 1/24
   update_program_timbre_byte(off, pcOff+0x0092);
   // update_program_timbre_cc(_timbre, off, 90);
   off++;
   
   // LFO1 Tempo Sync, Key Sync, Init Phase
   sv = prog_state[off];
   dv = prog_data[off];
   if(b_force || (sv != dv))
   {
      if(b_force || ( (sv&0x80) != (dv&0x80) ) ) // Tempo Sync
      {
         update_program_timbre_add_pc(pcOff+0x0093, (dv&0x80)>>7);
      }
      if(b_force || ( (sv&0x60) != (dv&0x60) ) ) // Key Sync
      {
         update_program_timbre_add_pc(pcOff+0x0094, (dv&0x60)>>5);
      }
      if(b_force || ( (sv&0x1F) != (dv&0x1F) ) ) // Init Phase
      {
         update_program_timbre_add_pc(pcOff+0x0095, (dv&0x1F)>>0);
      }
      prog_state[off] = dv;
   }
   off++;
   
   // LFO1 Sync Note
   sv = prog_state[off];
   dv = prog_data[off];
   if(b_force || (sv != dv))
   {
      if(b_force || ( (sv&0x1F) != (dv&0x1F) ) ) // Sync Note
      {
         update_program_timbre_add_pc(pcOff+0x0096, (dv&0x1F)>>0);
      }
      prog_state[off] = dv;
   }
   off++;
   
   // LFO2 Wave
   sv = prog_state[off];
   dv = prog_data[off];
   if(b_force || (sv != dv))
   {
      if(b_force || ( (sv&0x0F) != (dv&0x0F) ) ) // Wave
      {
         update_program_timbre_add_pc(pcOff+0x00A0, (dv&0x0F)>>0);
         // $00..$1F : Saw
         // $20..$3F : Squ
         // $40..$5F : Tri
         // $60..$7F : S&H
         // update_program_timbre_cc2(_timbre, (dv&0x0F)*0x20, 102);
      }
      prog_state[off] = dv;
   }
   off++;
   
   // LFO2 Shape
   update_program_timbre_sbyte(off, pcOff+0x00A1);
   off++;
   
   // LFO2 Frequency
   update_program_timbre_byte(off, pcOff+0x00A2);
   // update_program_timbre_cc(_timbre, off, 76);
   off++;
   
   // LFO2 Tempo Sync, Key Sync, Init Phase
   sv = prog_state[off];
   dv = prog_data[off];
   if(b_force || (sv != dv))
   {
      if(b_force || ( (sv&0x80) != (dv&0x80) ) ) // Tempo Sync
      {
         update_program_timbre_add_pc(pcOff+0x00A3, (dv&0x80)>>7);
      }
      if(b_force || ( (sv&0x60) != (dv&0x60) ) ) // Key Sync
      {
         update_program_timbre_add_pc(pcOff+0x00A4, (dv&0x60)>>5);
      }
      if(b_force || ( (sv&0x1F) != (dv&0x1F) ) ) // Init Phase
      {
         update_program_timbre_add_pc(pcOff+0x00A5, (dv&0x1F)>>0);
      }
      prog_state[off] = dv;
   }
   off++;
   
   // LFO2 Sync Note
   sv = prog_state[off];
   dv = prog_data[off];
   if(b_force || (sv != dv))
   {
      if(b_force || ( (sv&0x1F) != (dv&0x1F) ) ) // Sync Note
      {
         update_program_timbre_add_pc(pcOff+0x00A6, (dv&0x1F)>>0);
      }
      prog_state[off] = dv;
   }
   off++;
   
   // Patch1 Source
   update_program_timbre_byte(off, pcOff+0x0100);
   off++;
   
   // Patch1 Destination
   update_program_timbre_byte(off, pcOff+0x0101);
   off++;
   
   // Patch1 Intensity
   update_program_timbre_sbyte(off, pcOff+0x0102);
   // update_program_timbre_cc(_timbre, off, 103);
   off++;
   
   // Patch2 Source
   update_program_timbre_byte(off, pcOff+0x0104);
   off++;
   
   // Patch2 Destination
   update_program_timbre_byte(off, pcOff+0x0105);
   off++;
   
   // Patch2 Intensity
   update_program_timbre_sbyte(off, pcOff+0x0106);
   // update_program_timbre_cc(_timbre, off, 104);
   off++;
   
   // Patch3 Source
   update_program_timbre_byte(off, pcOff+0x0108);
   off++;
   
   // Patch3 Destination
   update_program_timbre_byte(off, pcOff+0x0109);
   off++;
   
   // Patch3 Intensity
   update_program_timbre_sbyte(off, pcOff+0x010A);
   // update_program_timbre_cc(_timbre, off, 105);
   off++;
   
   // Patch4 Source
   update_program_timbre_byte(off, pcOff+0x010C);
   off++;
   
   // Patch4 Destination
   update_program_timbre_byte(off, pcOff+0x010D);
   off++;
   
   // Patch4 Intensity
   update_program_timbre_sbyte(off, pcOff+0x010E);
   // update_program_timbre_cc(_timbre, off, 106);
   off++;
   
   // Patch5 Source
   update_program_timbre_byte(off, pcOff+0x0110);
   off++;
   
   // Patch5 Destination
   update_program_timbre_byte(off, pcOff+0x0111);
   off++;
   
   // Patch5 Intensity
   update_program_timbre_sbyte(off, pcOff+0x0112);
   // update_program_timbre_cc(_timbre, off, 107);
   off++;
   
   // Patch6 Source
   update_program_timbre_byte(off, pcOff+0x0114);
   off++;
   
   // Patch6 Destination
   update_program_timbre_byte(off, pcOff+0x0115);
   off++;
   
   // Patch6 Intensity
   update_program_timbre_sbyte(off, pcOff+0x0116);
   // update_program_timbre_cc(_timbre, off, 108);
   off++;


   //
   //
   // TABLE 8 : INSERT FX PARAMETER (52 bytes)
   //
   //
   pcOff -= 0x0500; // undo timbre offset

   if(_bSendFX)
   {
      // yac_host->printf("xxx send IFX1: off=%d pcOff=0x%04x\n", off, pcOff);
      update_ifx(off,    pcOff+0x0900);  // IFX1
      update_ifx(off+24, pcOff+0x0930);  // IFX2
   }
   off += 48;

   // Insert FX, EQ Low Freq
   update_program_timbre_byte(off, pcOff+0x0960);
   off++;

   // Insert FX, EQ Low Gain
   update_program_timbre_sbyte(off, pcOff+0x0961);
   off++;

   // Insert FX, EQ Hi Freq
   update_program_timbre_byte(off, pcOff+0x0962);
   off++;

   // Insert FX, EQ Hi Gain
   update_program_timbre_sbyte(off, pcOff+0x0963);
   off++;
   
}



void YAC_CALL tkradiastools_update_program_timbre(YAC_Object *_progState, 
                                                  YAC_Object *_progData, 
                                                  sSI _timbreNr, 
                                                  sSI _bSendFX,
                                                  YAC_Object *_out, 
                                                  YAC_Object *_cc,
                                                  sBool _bForce
                                                  ) 
{
   if(YAC_BCHK(_progState, YAC_CLID_BUFFER))
   {
      if(YAC_BCHK(_progData, YAC_CLID_BUFFER))
      {
         if(YAC_BCHK(_out, YAC_CLID_BUFFER))
         {
            if(YAC_BCHK(_cc, YAC_CLID_BUFFER))
            {
               YAC_Buffer *progState = (YAC_Buffer *) _progState;
               YAC_Buffer *progData  = (YAC_Buffer *) _progData;
               out        = (YAC_Buffer *) _out;
               cc         = (YAC_Buffer *) _cc;
               if( (_timbreNr >= 0) && (_timbreNr < 4) )
               {
                  if(progState->size == RADIAS_PROGRAMDATASIZE_DEC)
                  {
                     if(progData->size == RADIAS_PROGRAMDATASIZE_DEC)
                     {
                        prog_state = progState->buffer;
                        prog_data  = progData->buffer;
                        b_force = _bForce;
                        update_program_timbre(_timbreNr, _bSendFX);
                        ////yac_host->printf("xxx program timbre %i updated.\n");
                     }
                  }
               }
            }
         }
      }
   }
}




//-----------------------------------------------------------------------------
//--                                                                         --
//--                                                                         --
//--                                                                         --
//--                                                                         --
//--                                                                         --
//--                                                                         --
//--                                                                         --
//--                                                                         --
//--                                                                         --
//-- tkradiastools_copy_program_timbre                                       --
//--                                                                         --
//--                                                                         --
//--                                                                         --
//--                                                                         --
//--                                                                         --
//--                                                                         --
//--                                                                         --
//--                                                                         --
//-----------------------------------------------------------------------------

static void copy_program_timbre(sU8 *_progState, sU8 *_progData, sSI _timbreNr) {

   // Just copy the relevant bytes from _progData to _progState
   
   int pcOff = 0x1000 * (_timbreNr + 1); // Parameter change offset
   int off = 48 + (RADIAS_TIMBREDATASIZE * _timbreNr); // byte offset in program dump (decoded!)
   
   // TABLE 2 : TIMBRE PARAMETER
   
   // Timbre SW, Sub Output select, Arpg/Sseq Assign
   _progState[off] = _progData[off];
   off++;
   
   // Dummy byte (1)
   off++;
   
   // Out Balance
   _progState[off] = _progData[off];
   off++;
   
   // Dummy byte (3)
   off++;
   
   // Midi Ch (SKIP)
   off++;
   
   // Pitch bend, .. filters
   _progState[off] = _progData[off];
   off++;
   
   // Key Window Bottom
   _progState[off] = _progData[off];
   off++;
   
   // Key Window Top
   _progState[off] = _progData[off];
   off++;
   
   // Unison switch, Unison Voice
   _progState[off] = _progData[off];
   off++;
   
   // Unison Detune
   _progState[off] = _progData[off];
   off++;
   
   // Unison Spread
   _progState[off] = _progData[off];
   off++;
   
   // Bend Range
   _progState[off] = _progData[off];
   off++;
   
   // Portamento Time
   _progState[off] = _progData[off];
   off++;
   
   // Portamento Curve, ..
   _progState[off] = _progData[off];
   off++;
   
   // Dummy byte (14)
   off++;
   
   // Dummy byte (15)
   off++;
   
   // TABLE 7 : TIMBRE PROGRAM PARAMETER (16..RADIAS_TIMBREDATASIZE(228))
   pcOff += 0x0500;
   
   // Program name (SKIP)
   off += 12;
   
   // Dummy bytes (12..15)
   off += 4;
   
   ////trace "xxx off = "+off;
   
   // Voice assign mode, trigger mode, priority
   _progState[off] = _progData[off];
   off++;
   
   // Dummy byte (17)
   off++;
   
   // Analog Tuning
   _progState[off] = _progData[off];
   off++;
   
   // Transpose
   _progState[off] = _progData[off];
   off++;
   
   // Detune
   _progState[off] = _progData[off];
   off++;
   
   // Vibrato Int
   _progState[off] = _progData[off];
   off++;
   
   // Osc1 Bus Select, Mod, Wave Type
   _progState[off] = _progData[off];
   off++;
   
   // Osc1 Waveform CTRL1
   _progState[off] = _progData[off];
   off++;
   
   // Osc1 Waveform CTRL2
   _progState[off] = _progData[off];
   off++;
   
   // Osc1 DWGS Wave Select
   _progState[off] = _progData[off];
   off++;
   
   // Osc1 PCM Wave Select
   _progState[off] = _progData[off];
   off++;
   
   // Osc2 Mod Select, Wave Type
   _progState[off] = _progData[off];
   off++;
   
   // Osc2 Semitone
   _progState[off] = _progData[off];
   off++;
   
   // Osc2 Tune
   _progState[off] = _progData[off];
   off++;
   
   // Mixer Osc1 Level
   _progState[off] = _progData[off];
   off++;
   
   // Mixer Osc2 Level
   _progState[off] = _progData[off];
   off++;
   
   // Mixer Noise Level
   _progState[off] = _progData[off];
   off++;
   
   // Filter Link SW, Filter2 Type, Filter Routing. Important: Send Filter Routing First!!!!!!
   _progState[off] = _progData[off];
   off++;
   
   // Filter1 Balance
   _progState[off] = _progData[off];
   off++;
   
   // Filter1 Cutoff
   _progState[off] = _progData[off];
   off++;
   
   // Filter1 Resonance
   _progState[off] = _progData[off];
   off++;
   
   // F1 EG1 Intensity
   _progState[off] = _progData[off];
   off++;
   
   // F1 Keyboard Track
   _progState[off] = _progData[off];
   off++;
   
   // F1 Velocity Sens
   _progState[off] = _progData[off];
   off++;
   
   // Filter2 Cutoff
   _progState[off] = _progData[off];
   off++;
   
   // Filter2 Resonance
   _progState[off] = _progData[off];
   off++;
   
   // F2 EG1 Intensity
   _progState[off] = _progData[off];
   off++;
   
   // F2 Keyboard Track
   _progState[off] = _progData[off];
   off++;
   
   // F2 Velocity Sens
   _progState[off] = _progData[off];
   off++;
   
   // Amp Level
   _progState[off] = _progData[off];
   off++;
   
   // Amp Drive/WS, Mod Type
   _progState[off] = _progData[off];
   off++;
   
   // Amp WaveShape Type
   _progState[off] = _progData[off];
   off++;
   
   // Amp Driver/Shaper
   _progState[off] = _progData[off];
   off++;
   
   // Amp PanPot   xxx signed ?
   _progState[off] = _progData[off];
   off++;
   
   // Amp KeyTrack
   _progState[off] = _progData[off];
   off++;
   
   // Amp Punch Level
   _progState[off] = _progData[off];
   off++;
   
   // EG1 Attack Time
   _progState[off] = _progData[off];
   off++;
   
   // EG1 Decay Time
   _progState[off] = _progData[off];
   off++;
   
   // EG1 Sustain Level
   _progState[off] = _progData[off];
   off++;
   
   // EG1 Release Time
   _progState[off] = _progData[off];
   off++;
   
   // EG1 Curve
   _progState[off] = _progData[off];
   off++;
   
   // EG1 LevelVelocitySens
   _progState[off] = _progData[off];
   off++;
   
   // EG1 TimeVelocitySens
   _progState[off] = _progData[off];
   off++;
   
   // EG1 TimeKeyTrack
   _progState[off] = _progData[off];
   off++;
   
   // EG2 Attack Time
   _progState[off] = _progData[off];
   off++;
   
   // EG2 Decay Time
   _progState[off] = _progData[off];
   off++;
   
   // EG2 Sustain Level
   _progState[off] = _progData[off];
   off++;
   
   // EG2 Release Time
   _progState[off] = _progData[off];
   off++;
   
   // EG2 Curve
   _progState[off] = _progData[off];
   off++;
   
   // EG2 LevelVelocitySens
   _progState[off] = _progData[off];
   off++;
   
   // EG2 TimeVelocitySens
   _progState[off] = _progData[off];
   off++;
   
   // EG2 TimeKeyTrack
   _progState[off] = _progData[off];
   off++;
   
   // EG3 Attack Time
   _progState[off] = _progData[off];
   off++;
   
   // EG3 Decay Time
   _progState[off] = _progData[off];
   off++;
   
   // EG3 Sustain Level
   _progState[off] = _progData[off];
   off++;
   
   // EG3 Release Time
   _progState[off] = _progData[off];
   off++;
   
   // EG3 Curve
   _progState[off] = _progData[off];
   off++;
   
   // EG3 LevelVelocitySens
   _progState[off] = _progData[off];
   off++;
   
   // EG3 TimeVelocitySens
   _progState[off] = _progData[off];
   off++;
   
   // EG3 TimeKeyTrack
   _progState[off] = _progData[off];
   off++;
   
   // LFO1 Wave
   _progState[off] = _progData[off];
   off++;
   
   // LFO1 Shape
   _progState[off] = _progData[off];
   off++;
   
   // LFO1 Frequency
   _progState[off] = _progData[off];
   off++;
   
   // LFO1 Tempo Sync, Key Sync, Init Phase
   _progState[off] = _progData[off];
   off++;
   
   // LFO1 Sync Note
   _progState[off] = _progData[off];
   off++;
   
   // LFO2 Wave
   _progState[off] = _progData[off];
   off++;
   
   // LFO2 Shape
   _progState[off] = _progData[off];
   off++;
   
   // LFO2 Frequency
   _progState[off] = _progData[off];
   off++;
   
   // LFO2 Tempo Sync, Key Sync, Init Phase
   _progState[off] = _progData[off];
   off++;
   
   // LFO2 Sync Note
   _progState[off] = _progData[off];
   off++;
   
   // Patch1 Source
   _progState[off] = _progData[off];
   off++;
   
   // Patch1 Destination
   _progState[off] = _progData[off];
   off++;
   
   // Patch1 Intensity
   _progState[off] = _progData[off];
   off++;
   
   // Patch2 Source
   _progState[off] = _progData[off];
   off++;
   
   // Patch2 Destination
   _progState[off] = _progData[off];
   off++;
   
   // Patch2 Intensity
   _progState[off] = _progData[off];
   off++;
   
   // Patch3 Source
   _progState[off] = _progData[off];
   off++;
   
   // Patch3 Destination
   _progState[off] = _progData[off];
   off++;
   
   // Patch3 Intensity
   _progState[off] = _progData[off];
   off++;
   
   // Patch4 Source
   _progState[off] = _progData[off];
   off++;
   
   // Patch4 Destination
   _progState[off] = _progData[off];
   off++;
   
   // Patch4 Intensity
   _progState[off] = _progData[off];
   off++;
   
   // Patch5 Source
   _progState[off] = _progData[off];
   off++;
   
   // Patch5 Destination
   _progState[off] = _progData[off];
   off++;
   
   // Patch5 Intensity
   _progState[off] = _progData[off];
   off++;
   
   // Patch6 Source
   _progState[off] = _progData[off];
   off++;
   
   // Patch6 Destination
   _progState[off] = _progData[off];
   off++;
   
   // Patch6 Intensity
   _progState[off] = _progData[off];
   off++;

   // Insert FX , Effect1 SW + type
   _progState[off] = _progData[off];
   off++;

   // Insert FX , dummy byte
   off++;

   // Insert FX , Fx1 knob1 assign
   _progState[off] = _progData[off];
   off++;

   // Insert FX , Fx1 knob2 assign
   _progState[off] = _progData[off];
   off++;

   // Insert FX , Fx1 Int param 1
   _progState[off] = _progData[off];
   off++;

   // Insert FX , Fx1 Int param 2
   _progState[off] = _progData[off];
   off++;

   // Insert FX , Fx1 Int param 3
   _progState[off] = _progData[off];
   off++;

   // Insert FX , Fx1 Int param 4
   _progState[off] = _progData[off];
   off++;

   // Insert FX , Fx1 Int param 5
   _progState[off] = _progData[off];
   off++;

   // Insert FX , Fx1 Int param 6
   _progState[off] = _progData[off];
   off++;

   // Insert FX , Fx1 Int param 7
   _progState[off] = _progData[off];
   off++;

   // Insert FX , Fx1 Int param 8
   _progState[off] = _progData[off];
   off++;

   // Insert FX , Fx1 Int param 9
   _progState[off] = _progData[off];
   off++;

   // Insert FX , Fx1 Int param 10
   _progState[off] = _progData[off];
   off++;

   // Insert FX , Fx1 Int param 11
   _progState[off] = _progData[off];
   off++;

   // Insert FX , Fx1 Int param 12
   _progState[off] = _progData[off];
   off++;

   // Insert FX , Fx1 Int param 13
   _progState[off] = _progData[off];
   off++;

   // Insert FX , Fx1 Int param 14
   _progState[off] = _progData[off];
   off++;

   // Insert FX , Fx1 Int param 15
   _progState[off] = _progData[off];
   off++;

   // Insert FX , Fx1 Int param 16
   _progState[off] = _progData[off];
   off++;

   // Insert FX , Fx1 Int param 17
   _progState[off] = _progData[off];
   off++;

   // Insert FX , Fx1 Int param 18
   _progState[off] = _progData[off];
   off++;

   // Insert FX , Fx1 Int param 19
   _progState[off] = _progData[off];
   off++;

   // Insert FX , Fx1 Int param 20
   _progState[off] = _progData[off];
   off++;

   //
   //
   // Insert FX , Effect2 SW + type
   _progState[off] = _progData[off];
   off++;

   // Insert FX , dummy byte
   off++;

   // Insert FX , Fx2 knob1 assign
   _progState[off] = _progData[off];
   off++;

   // Insert FX , Fx2 knob2 assign
   _progState[off] = _progData[off];
   off++;

   // Insert FX , Fx2 Int param 1
   _progState[off] = _progData[off];
   off++;

   // Insert FX , Fx2 Int param 2
   _progState[off] = _progData[off];
   off++;

   // Insert FX , Fx2 Int param 3
   _progState[off] = _progData[off];
   off++;

   // Insert FX , Fx2 Int param 4
   _progState[off] = _progData[off];
   off++;

   // Insert FX , Fx2 Int param 5
   _progState[off] = _progData[off];
   off++;

   // Insert FX , Fx2 Int param 6
   _progState[off] = _progData[off];
   off++;

   // Insert FX , Fx2 Int param 7
   _progState[off] = _progData[off];
   off++;

   // Insert FX , Fx2 Int param 8
   _progState[off] = _progData[off];
   off++;

   // Insert FX , Fx2 Int param 9
   _progState[off] = _progData[off];
   off++;

   // Insert FX , Fx2 Int param 10
   _progState[off] = _progData[off];
   off++;

   // Insert FX , Fx2 Int param 11
   _progState[off] = _progData[off];
   off++;

   // Insert FX , Fx2 Int param 12
   _progState[off] = _progData[off];
   off++;

   // Insert FX , Fx2 Int param 13
   _progState[off] = _progData[off];
   off++;

   // Insert FX , Fx2 Int param 14
   _progState[off] = _progData[off];
   off++;

   // Insert FX , Fx2 Int param 15
   _progState[off] = _progData[off];
   off++;

   // Insert FX , Fx2 Int param 16
   _progState[off] = _progData[off];
   off++;

   // Insert FX , Fx2 Int param 17
   _progState[off] = _progData[off];
   off++;

   // Insert FX , Fx2 Int param 18
   _progState[off] = _progData[off];
   off++;

   // Insert FX , Fx2 Int param 19
   _progState[off] = _progData[off];
   off++;

   // Insert FX , Fx2 Int param 20
   _progState[off] = _progData[off];
   off++;

   //
   //
   // Insert FX , EQ Low Freq
   _progState[off] = _progData[off];
   off++;

   // Insert FX , Low Gain
   _progState[off] = _progData[off];
   off++;

   // Insert FX , Hi Freq
   _progState[off] = _progData[off];
   off++;

   // Insert FX , Hi Gain
   _progState[off] = _progData[off];
   off++;

}

void tkradiastools_copy_program_timbre(YAC_Object *_progState, YAC_Object *_progData, sSI _timbreNr) 
{
   if(YAC_BCHK(_progState, YAC_CLID_BUFFER))
   {
      if(YAC_BCHK(_progData, YAC_CLID_BUFFER))
      {
         YAC_Buffer *progState = (YAC_Buffer *) _progState;
         YAC_Buffer *progData  = (YAC_Buffer *) _progData;
         if( (_timbreNr >= 0) && (_timbreNr < 4) )
         {
            if(progState->size == RADIAS_PROGRAMDATASIZE_DEC)
            {
               if(progData->size == RADIAS_PROGRAMDATASIZE_DEC)
               {
                  copy_program_timbre(progState->buffer, progData->buffer, _timbreNr);
               }
            }
         }
      }
   }
}







//-----------------------------------------------------------------------------
//--                                                                         --
//--                                                                         --
//--                                                                         --
//--                                                                         --
//--                                                                         --
//--                                                                         --
//--                                                                         --
//--                                                                         --
//--                                                                         --
//-- tkradiastools_update_drumkit_timbre                                     --
//--                                                                         --
//--                                                                         --
//--                                                                         --
//--                                                                         --
//--                                                                         --
//--                                                                         --
//--                                                                         --
//--                                                                         --
//-----------------------------------------------------------------------------

static void update_drumtimbre_add_pc(sU16 _paramIdSubId, sS16 _paramValue)  {
   sU16 id = (_paramIdSubId>>8)&255;
   sU16 subId = _paramIdSubId&255;
   
   out->yacStreamWriteI8( 0xF0 );
   out->yacStreamWriteI8( 0x42 ); // Korg Id
   out->yacStreamWriteI8( 0x30 | RADIAS_DEVICE_NR );
   out->yacStreamWriteI8( 0x72 ); // Radias Id
   out->yacStreamWriteI8( RADIAS_FUN_DRUMKITPARAMETERCHANGE ); // 0x53
   out->yacStreamWriteI8( id&127 ); // Parameter Id LSB
   out->yacStreamWriteI8( (id>>7)&127 ); // Parameter Id MSB
   out->yacStreamWriteI8( subId&127 ); // Parameter Sub Id LSB
   out->yacStreamWriteI8( (subId>>7)&127 ); // Parameter Sub Id MSB
   out->yacStreamWriteI8( _paramValue&127 ); 
   out->yacStreamWriteI8( (_paramValue>>7)&127 );
   out->yacStreamWriteI8( 0xF7 );
}



static void update_drumtimbre_byte(int _off, int _pcOff) {
   // 0..127
   sU8 sv = drumkit_state[_off];
   sU8 dv = drumkit_data[_off];
   if(b_force || (sv != dv))
   {
      update_drumtimbre_add_pc(_pcOff, dv);
      drumkit_state[_off] = dv;
   }
}



static void update_drumtimbre_sbyte(int _off, int _pcOff) {
   // -63..0..63
   sU8 sv = drumkit_state[_off];
   sU8 dv = drumkit_data[_off];
   if(b_force || (sv != dv))
   {
      sS16 v = dv - 64;
      update_drumtimbre_add_pc(_pcOff, v);
      drumkit_state[_off] = dv;
   }
}



static void update_drumkit_timbre(sSI _timbre) {
   // Incrementally update current Radias program state; just send the changes.
   ////int bCCRS = false;
   int pcOff = 0x1000 + 0x0200 * _timbre; // Parameter change offset
   int off = 52 + (104 * _timbre); // byte offset in drumkit data dump (decoded!)
   sU8 sv;
   sU8 dv;
   
   // TABLE 11 : DRUMKIT PARAMETER
   
   // TABLE 7 : TIMBRE PROGRAM PARAMETER
   
   // Program name (SKIP)
   off += 12;
   
   // Dummy bytes (12..15)
   off += 4;
   
   ////trace "xxx off = "+off;
   
   // Voice assign mode, trigger mode, priority
   sv = drumkit_state[off];
//    dv = drumkit_data[off];
//    dv = 0x00;
//    if(sv != dv)
//    {
//       if( (sv&0x80) != (dv&0x80) ) // Assign Mode  (Mono/Poly)
//       {
//          update_drumtimbre_add_pc(pcOff+0x0010, (dv&0x80)>>7);
//       }
//       if( (sv&0x40) != (dv&0x40) ) // Trigger Mode (Single/Multi)
//       {
//          update_drumtimbre_add_pc(pcOff+0x0011, (dv&0x40)>>6);
//       }
//       if( (sv&0x03) != (dv&0x03) ) // Key priority (Last, Low, High)
//       {
//          update_drumtimbre_add_pc(pcOff+0x0012, (dv&0x03)>>0);
//       }
//       drumkit_state[off] = dv;
//    }
   off++;
   
   // Dummy byte (17)
   off++;
   
   // Analog Tuning
   update_drumtimbre_byte(off, pcOff+0x0013);
   off++;
   
   // Transpose
   update_drumtimbre_sbyte(off, pcOff+0x0014);
   off++;
   
   // Detune
   update_drumtimbre_sbyte(off, pcOff+0x0015);
   off++;
   
   // Vibrato Int
   update_drumtimbre_sbyte(off, pcOff+0x0016);
   off++;
   
   // Osc1 Bus Select, Mod, Wave Type
   sv = drumkit_state[off];
   dv = drumkit_data[off];
   if(b_force || (sv != dv))
   {
      if(b_force || ( (sv&0x40) != (dv&0x40) ) ) // Bus Select
      {
         update_drumtimbre_add_pc(pcOff+0x001D, (dv&0x40)>>6);
      }
      if(b_force || ( (sv&0x30) != (dv&0x30) ) ) // Osc Mod (Waveform, Cross, Unison, VPM)
      {
         update_drumtimbre_add_pc(pcOff+0x0018, (dv&0x30)>>4);
         ////updateDrumTimbre_CC2(_timbre, (dv&0x30)>>4, 9);
      }
      if(b_force || ( (sv&0x0F) != (dv&0x0F) ) ) // Wave Type
      {
         update_drumtimbre_add_pc(pcOff+0x0017, (dv&0x0F)>>0);
         ////updateDrumTimbre_CC2(_timbre, (dv&0x0F)<<4, 8);
      }
      drumkit_state[off] = dv;
   }
   off++;
   
   // Osc1 Waveform CTRL1
   sU8 waveType = (drumkit_state[off-1]&0x0F);
   sU8 oscMod = (drumkit_state[off-1]&0x30)>>4;
   if( (waveType == 7) || ((waveType <=3) && (oscMod==3)) )
   {
      // param becomes signed when wavetype == 7 (DrumPCM) or (wavetype <= 3 (Sine) and Osc Mod==VPM). argh.
      update_drumtimbre_sbyte(off, pcOff+0x0019); 
   }
   else
   {
      update_drumtimbre_byte(off, pcOff+0x0019);
   }
   /////updateDrumTimbre_CC(_timbre, off, 15);
   off++;
   
   // Osc1 Waveform CTRL2
   if( (waveType >= 4) || ((waveType <= 3) && (oscMod==3)) )
   {
      // param becomes signed when wavetype is >=4 (Formant) or (wavetype <= 3 (Sine) and Osc Mod==VPM). argh.
      update_drumtimbre_sbyte(off, pcOff+0x001A); 
   }
   else
   {
      update_drumtimbre_byte(off, pcOff+0x001A);
   }
   ////updateDrumTimbre_CC(_timbre, off, 17);
   off++;
   
   // Osc1 DWGS Wave Select
   update_drumtimbre_byte(off, pcOff+0x001B);
   off++;
   
   // Osc1 PCM Wave Select
   update_drumtimbre_byte(off, pcOff+0x001C);
   off++;
   
   // Osc2 Mod Select, Wave Type
   sv = drumkit_state[off];
   dv = drumkit_data[off];
   if(b_force || (sv != dv))
   {
      if(b_force || ( (sv&0x30) != (dv&0x30) ) ) // Mod Select
      {
         update_drumtimbre_add_pc(pcOff+0x0021, (dv&0x30)>>4);
         // $00..$1F : OFF
         // $20..$3F : RING
         // $40..$5F : SYNC
         // $60..$7F : RING+SYNC
         ////updateDrumTimbre_CC2(_timbre, ((dv&0x30)>>4)*$20, 19);
      }
      if(b_force || ( (sv&0x03) != (dv&0x03) ) ) // Wave Type (Saw, Squ, Tri, Sin)
      {
         update_drumtimbre_add_pc(pcOff+0x0020, (dv&0x03)>>0);
         // $00..$0D : Saw
         // $0E..$1B : Square
         // $1C..$29 : Tri
         // $2A..$37 : Sin
         // $38..$45 : Formant
         // $46..$53 : Noise
         // $54..$61 : Synth PCM
         // $62..$6F : Drum PCM
         // $70..$7F : Audio In
         ////updateDrumTimbre_CC2(_timbre, (dv&0x03)*14, 18); 
      }
      drumkit_state[off] = dv;
   }
   off++;
   
   // Osc2 Semitone
   update_drumtimbre_sbyte(off, pcOff+0x0022);
   ////updateDrumTimbre_CC(_timbre, drumkit_data, off, 20);
   off++;
   
   // Osc2 Tune
   update_drumtimbre_sbyte(off, pcOff+0x0023);
   ////updateDrumTimbre_CC(_timbre, drumkit_data, off, 21);
   off++;
   
   // Mixer Osc1 Level
   update_drumtimbre_byte(off, pcOff+0x0028);
   ////updateDrumTimbre_CC(_timbre, drumkit_data, off, 23);
   off++;
   
   // Mixer Osc2 Level
   update_drumtimbre_byte(off, pcOff+0x0029);
   ////updateDrumTimbre_CC(_timbre, drumkit_data, off, 24);
   off++;
   
   // Mixer Noise Level
   update_drumtimbre_byte(off, pcOff+0x002A);
   ////updateDrumTimbre_CC(_timbre, drumkit_data, off, 25);
   off++;
   
   // Filter Link SW, Filter2 Type, Filter Routing. Important: Send filter routing first!!
   sv = drumkit_state[off];
   dv = drumkit_data[off];
   if(b_force || (sv != dv))
   {
      if(b_force || ( (sv&0x03) != (dv&0x03) ) ) // Filter Routing (Single, Serial, Parallel, Indiv)
      {
         update_drumtimbre_add_pc(pcOff+0x0030, (dv&0x03)>>0);
         // For Synth Timbres:
         // $00..$1F : OFF
         // $20..$3F : SERIAL
         // $40..$5F : PARALLEL
         // $60..$7F : INDIVIDUAL
         //
         // For Vocoder Timbres: (TODO)
         // $00..$3F : Audio
         // $40..$7F : Formant Play
         ////updateDrumTimbre_CC2(_timbre, (dv&0x03)*$20, 26);
      }
      if(b_force || ( (sv&0x80) != (dv&0x80) ) ) // Filter Link SW
      {
         update_drumtimbre_add_pc(pcOff+0x0037, (dv&0x80)>>7);
      }
      if(b_force || ( (sv&0x30) != (dv&0x30) ) ) // Filter 2 Type (LPF, HPF, BPF, COMBO)
      {
         update_drumtimbre_add_pc(pcOff+0x0040, (dv&0x30)>>4);
         // Synth Timbre:
         // $00..$1F : LPF
         // $20..$3F : HPF
         // $40..$5F : BPF
         // $60..$7F : COMB
         //
         // Vocoder Timbre: (Formant Shift, TODO)
         // $00..$1F : +1
         // $20..$3F : +2
         // $40..$5F : -1
         // $60..$7F : -2
         ////updateDrumTimbre_CC2(_timbre, ((dv&0x30)>>4)*$20, 29);
      }
      drumkit_state[off] = dv;
   }
   off++;
   
   // Filter1 Balance
   update_drumtimbre_byte(off, pcOff+0x0031);
   ////updateDrumTimbre_CC(_timbre, drumkit_data, off, 27);
   off++;
   
   // Filter1 Cutoff
   update_drumtimbre_byte(off, pcOff+0x0032);
   ////updateDrumTimbre_CC(_timbre, drumkit_data, off, 74);
   off++;
   
   // Filter1 Resonance
   update_drumtimbre_byte(off, pcOff+0x0033);
   ////updateDrumTimbre_CC(_timbre, drumkit_data, off, 71);
   off++;
   
   // F1 EG1 Intensity
   update_drumtimbre_sbyte(off, pcOff+0x0034);
   ////updateDrumTimbre_CC(_timbre, drumkit_data, off, 79);
   off++;
   
   // F1 Keyboard Track
   update_drumtimbre_sbyte(off, pcOff+0x0035);
   ////updateDrumTimbre_CC(_timbre, drumkit_data, off, 28);
   off++;
   
   // F1 Velocity Sens
   update_drumtimbre_sbyte(off, pcOff+0x0036);
   off++;
   
   // Filter2 Cutoff
   update_drumtimbre_byte(off, pcOff+0x0042);
   ////updateDrumTimbre_CC(_timbre, off, 30);
   off++;
   
   // Filter2 Resonance
   update_drumtimbre_byte(off, pcOff+0x0043);
   ////updateDrumTimbre_CC(_timbre, off, 68);
   off++;
   
   // F2 EG1 Intensity
   update_drumtimbre_sbyte(off, pcOff+0x0044);
   ////updateDrumTimbre_CC(_timbre, off, 69);
   off++;
   
   // F2 Keyboard Track
   update_drumtimbre_sbyte(off, pcOff+0x0045);
   ////updateDrumTimbre_CC(_timbre, off, 82);
   off++;
   
   // F2 Velocity Sens
   update_drumtimbre_sbyte(off, pcOff+0x0046);
   off++;
   
   // Amp Level
   update_drumtimbre_byte(off, pcOff+0x0050);
   ////updateDrumTimbre_CC(_timbre, off, 7);
   off++;
   
   // Amp Drive/WS, Mod Type
   sv = drumkit_state[off];
   dv = drumkit_data[off];
   if(b_force || (sv != dv))
   {
      if(b_force || ( (sv&0x70) != (dv&0x70) ) ) // Drive/WS Position (Pre Flt1, Pre Amp)
      {
         update_drumtimbre_add_pc(pcOff+0x0052, (dv&0x70)>>4);
      }
      if(b_force || ( (sv&0x03) != (dv&0x03) ) ) // Mod Type (Off/Driver, Waveshape)
      {
         // Drive/WS SW ?!!
         update_drumtimbre_add_pc(pcOff+0x0053, (dv&0x03)>>0);
         // Synth Timbre:
         //   $00..$29 : OFF
         //   $2A..$54 : Drive
         //   $55..$7F : WaveShape
         //
         // Vocoder Timbre: (HPF Gate, TODO)
         //   $00..$3F : Disable
         //   $40..$7F : Enable
         /*switch((dv&0x03)>>0)
         {
         default:
         case 0:
         updateDrumTimbre_CC2(_timbre, $00, 84);
         break;
         case 1:
         updateDrumTimbre_CC2(_timbre, $2A, 84);
         break;
         case 2:
         updateDrumTimbre_CC2(_timbre, $55, 84);
         break;
      }*/
      }
      drumkit_state[off] = dv;
   }
   off++;
   
   // Amp WaveShape Type
   sv = drumkit_state[off];
   dv = drumkit_data[off];
   if(b_force || (sv != dv))
   {
      if(b_force || ( (sv&0x0F) != (dv&0x0F) ) ) // WaveShape Type
      {
         update_drumtimbre_add_pc(pcOff+0x0051, (dv&0x0F)>>0);
      }
      drumkit_state[off] = dv;
   }
   off++;
   
   // Amp Driver/Shaper
   update_drumtimbre_byte(off, pcOff+0x0054);
   ////updateDrumTimbre_CC(_timbre, off, 83);
   off++;
   
   // Amp PanPot   xxx signed ?
   update_drumtimbre_byte(off, pcOff+0x0055);
   ////updateDrumTimbre_CC(_timbre, off, 10);
   off++;
   
   // Amp KeyTrack
   update_drumtimbre_sbyte(off, pcOff+0x0056);
   off++;
   
   // Amp Punch Level
   update_drumtimbre_byte(off, pcOff+0x0057);
   off++;
   
   // EG1 Attack Time
   update_drumtimbre_byte(off, pcOff+0x0060);
   ////updateDrumTimbre_CC(_timbre, off, 85);
   off++;
   
   // EG1 Decay Time
   update_drumtimbre_byte(off, pcOff+0x0061);
   ////updateDrumTimbre_CC(_timbre, off, 86);
   off++;
   
   // EG1 Sustain Level
   update_drumtimbre_byte(off, pcOff+0x0062);
   ////updateDrumTimbre_CC(_timbre, off, 87);
   off++;
   
   // EG1 Release Time
   update_drumtimbre_byte(off, pcOff+0x0063);
   ////updateDrumTimbre_CC(_timbre, off, 88);
   off++;
   
   // EG1 Curve
   sv = drumkit_state[off];
   dv = drumkit_data[off];
   if(b_force || (sv != dv))
   {
      if(b_force || ( (sv&0x03) != (dv&0x03) ) ) // Curve
      {
         update_drumtimbre_add_pc(pcOff+0x0064, (dv&0x03)>>0);
      }
      drumkit_state[off] = dv;
   }
   off++;
   
   // EG1 LevelVelocitySens
   update_drumtimbre_sbyte(off, pcOff+0x0065);
   off++;
   
   // EG1 TimeVelocitySens
   update_drumtimbre_sbyte(off, pcOff+0x0066);
   off++;
   
   // EG1 TimeKeyTrack
   update_drumtimbre_sbyte(off, pcOff+0x0067);
   off++;
   
   // EG2 Attack Time
   update_drumtimbre_byte(off, pcOff+0x0070);
   ////updateDrumTimbre_CC(_timbre, off, 73);
   off++;
   
   // EG2 Decay Time
   update_drumtimbre_byte(off, pcOff+0x0071);
   ////updateDrumTimbre_CC(_timbre, off, 75);
   off++;
   
   // EG2 Sustain Level
   update_drumtimbre_byte(off, pcOff+0x0072);
   ////updateDrumTimbre_CC(_timbre, off, 70);
   off++;
   
   // EG2 Release Time
   update_drumtimbre_byte(off, pcOff+0x0073);
   ////updateDrumTimbre_CC(_timbre, off, 72);
   off++;
   
   // EG2 Curve
   sv = drumkit_state[off];
   dv = drumkit_data[off];
   if(b_force || (sv != dv))
   {
      if(b_force || ( (sv&0x03) != (dv&0x03) ) ) // Curve
      {
         update_drumtimbre_add_pc(pcOff+0x0074, (dv&0x03)>>0);
      }
      drumkit_state[off] = dv;
   }
   off++;
   
   // EG2 LevelVelocitySens
   update_drumtimbre_sbyte(off, pcOff+0x0075);
   off++;
   
   // EG2 TimeVelocitySens
   update_drumtimbre_sbyte(off, pcOff+0x0076);
   off++;
   
   // EG2 TimeKeyTrack
   update_drumtimbre_sbyte(off, pcOff+0x0077);
   off++;
   
   // EG3 Attack Time
   update_drumtimbre_byte(off, pcOff+0x0080);
   off++;
   
   // EG3 Decay Time
   update_drumtimbre_byte(off, pcOff+0x0081);
   off++;
   
   // EG3 Sustain Level
   update_drumtimbre_byte(off, pcOff+0x0082);
   off++;
   
   // EG3 Release Time
   update_drumtimbre_byte(off, pcOff+0x0083);
   off++;
   
   // EG3 Curve
   sv = drumkit_state[off];
   dv = drumkit_data[off];
   if(b_force || (sv != dv))
   {
      if(b_force || ( (sv&0x03) != (dv&0x03) ) ) // Curve
      {
         update_drumtimbre_add_pc(pcOff+0x0084, (dv&0x03)>>0);
      }
      drumkit_state[off] = dv;
   }
   off++;
   
   // EG3 LevelVelocitySens
   update_drumtimbre_sbyte(off, pcOff+0x0085);
   off++;
   
   // EG3 TimeVelocitySens
   update_drumtimbre_sbyte(off, pcOff+0x0086);
   off++;
   
   // EG3 TimeKeyTrack
   update_drumtimbre_sbyte(off, pcOff+0x0087);
   off++;
   
   // LFO1 Wave
   sv = drumkit_state[off];
   dv = drumkit_data[off];
   if(b_force || (sv != dv))
   {
      if(b_force || ( (sv&0x0F) != (dv&0x0F) ) ) // Wave
      {
         update_drumtimbre_add_pc(pcOff+0x0090, (dv&0x0F)>>0);
         // $00..$1F : Saw
         // $20..$3F : Squ
         // $40..$5F : Tri
         // $60..$7F : S&H
         ////updateDrumTimbre_CC2(_timbre, (dv&0x0F)*$20, 89);
      }
      drumkit_state[off] = dv;
   }
   off++;
   
   // LFO1 Shape
   update_drumtimbre_sbyte(off, pcOff+0x0091);
   off++;
   
   // LFO1 Frequency
   update_drumtimbre_byte(off, pcOff+0x0092);
   // When Tempo Sync is "OFF".
   //   00~07 : 0.01, 0.02, 0.03, 0.04, 0.05, 0.06, 0.07, 0.08,
   //   08~0F : 0.09, 0.10, 0.11, 0.12, 0.13, 0.14, 0.15, 0.16,
   //   10~17 : 0.17, 0.18, 0.19, 0.20, 0.21, 0.22, 0.23, 0.24,
   //   18~1F : 0.25, 0.29, 0.33, 0.42, 0.50, 0.58, 0.67, 0.75,
   //   20~27 : 0.83, 0.92, 1.00, 1.13, 1.25, 1.38, 1.50, 1.63,
   //   28~2F : 1.75, 1.88, 2.00, 2.13, 2.25, 2.38, 2.50, 2.63,
   //   30~37 : 2.75, 2.88, 3.00, 3.13, 3.25, 3.38, 3.50, 3.63,
   //   38~3F : 3.75, 3.88, 4.00, 4.13, 4.25, 4.38, 4.50, 4.63,
   //   40~47 : 4.75, 4.88, 5.00, 5.25, 5.50, 5.75, 6.00, 6.25,
   //   48~4F : 6.50, 6.75, 7.00, 7.25, 7.50, 7.75, 8.00, 8.25,
   //   50~57 : 8.50, 8.75, 9.00, 9.25, 9.50, 9.75, 10.0, 10.5,
   //   58~5F : 11.0, 11.5, 12.0, 12.5, 13.0, 13.5, 14.0, 14.5,
   //   60~67 : 15.0, 16.0, 17.0, 18.0, 19.0, 20.0, 21.5, 23.0,
   //   68~6F : 24.5, 26.0, 27.5, 29.0, 31.0, 33.0, 35.0, 37.0,
   //   70~77 : 39.0, 41.0, 44.0, 47.0, 50.0, 53.0, 57.0, 61.0,
   //   78~7F : 65.0, 70.0, 75.0, 80.0, 85.0, 90.0, 95.0, 100.0
   //
   // When Tempo Sync is "ON".
   //   00~03 : 8/1 24~2B : 1/2 4C~53 : 1/6 74~7B : 1/32
   //   04~0B : 4/1 2C~33 : 3/8 54~5B : 1/8 7C~7F : 1/64
   //   0C~13 : 2/1 34~3B : 1/3 5C~63 : 1/12
   //   14~1B : 1/1 3C~43 : 1/4 64~6B : 1/16
   //   1C~23 : 3/4 44~4B : 3/16 6C~73 : 1/24
   ////updateDrumTimbre_CC(_timbre, off, 90);
   off++;
   
   // LFO1 Tempo Sync, Key Sync, Init Phase
   sv = drumkit_state[off];
   dv = drumkit_data[off];
   if(b_force || (sv != dv))
   {
      if(b_force || ( (sv&0x80) != (dv&0x80) ) ) // Tempo Sync
      {
         update_drumtimbre_add_pc(pcOff+0x0093, (dv&0x80)>>7);
      }
      if(b_force || ( (sv&0x60) != (dv&0x60) ) ) // Key Sync
      {
         update_drumtimbre_add_pc(pcOff+0x0094, (dv&0x60)>>5);
      }
      if(b_force || ( (sv&0x1F) != (dv&0x1F) ) ) // Init Phase
      {
         update_drumtimbre_add_pc(pcOff+0x0095, (dv&0x1F)>>0);
      }
      drumkit_state[off] = dv;
   }
   off++;
   
   // LFO1 Sync Note
   sv = drumkit_state[off];
   dv = drumkit_data[off];
   if(b_force || (sv != dv))
   {
      if(b_force || ( (sv&0x1F) != (dv&0x1F) ) ) // Sync Note
      {
         update_drumtimbre_add_pc(pcOff+0x0096, (dv&0x1F)>>0);
      }
      drumkit_state[off] = dv;
   }
   off++;
   
   // LFO2 Wave
   sv = drumkit_state[off];
   dv = drumkit_data[off];
   if(b_force || (sv != dv))
   {
      if(b_force || ( (sv&0x0F) != (dv&0x0F) ) ) // Wave
      {
         update_drumtimbre_add_pc(pcOff+0x00A0, (dv&0x0F)>>0);
         // $00..$1F : Saw
         // $20..$3F : Squ
         // $40..$5F : Tri
         // $60..$7F : S&H
         ////updateDrumTimbre_CC2(_timbre, (dv&0x0F)*$20, 102);
      }
      drumkit_state[off] = dv;
   }
   off++;
   
   // LFO2 Shape
   update_drumtimbre_sbyte(off, pcOff+0x00A1);
   off++;
   
   // LFO2 Frequency
   update_drumtimbre_byte(off, pcOff+0x00A2);
   ////updateDrumTimbre_CC(_timbre, off, 76);
   off++;
   
   // LFO2 Tempo Sync, Key Sync, Init Phase
   sv = drumkit_state[off];
   dv = drumkit_data[off];
   if(b_force || (sv != dv))
   {
      if(b_force || ( (sv&0x80) != (dv&0x80) ) ) // Tempo Sync
      {
         update_drumtimbre_add_pc(pcOff+0x00A3, (dv&0x80)>>7);
      }
      if(b_force || ( (sv&0x60) != (dv&0x60) ) ) // Key Sync
      {
         update_drumtimbre_add_pc(pcOff+0x00A4, (dv&0x60)>>5);
      }
      if(b_force || ( (sv&0x1F) != (dv&0x1F) ) ) // Init Phase
      {
         update_drumtimbre_add_pc(pcOff+0x00A5, (dv&0x1F)>>0);
      }
      drumkit_state[off] = dv;
   }
   off++;
   
   // LFO2 Sync Note
   sv = drumkit_state[off];
   dv = drumkit_data[off];
   if(b_force || (sv != dv))
   {
      if(b_force || ( (sv&0x1F) != (dv&0x1F) ) ) // Sync Note
      {
         update_drumtimbre_add_pc(pcOff+0x00A6, (dv&0x1F)>>0);
      }
      drumkit_state[off] = dv;
   }
   off++;
   
   // Patch1 Source
   update_drumtimbre_byte(off, pcOff+0x0100);
   off++;
   
   // Patch1 Destination
   update_drumtimbre_byte(off, pcOff+0x0101);
   off++;
   
   // Patch1 Intensity
   update_drumtimbre_byte(off, pcOff+0x0102);
   ////updateDrumTimbre_CC(_timbre, off, 103);
   off++;
   
   // Patch2 Source
   update_drumtimbre_byte(off, pcOff+0x0104);
   off++;
   
   // Patch2 Destination
   update_drumtimbre_byte(off, pcOff+0x0105);
   off++;
   
   // Patch2 Intensity
   update_drumtimbre_sbyte(off, pcOff+0x0106);
   ////updateDrumTimbre_CC(_timbre, off, 104);
   off++;
   
   // Patch3 Source
   update_drumtimbre_byte(off, pcOff+0x0108);
   off++;
   
   // Patch3 Destination
   update_drumtimbre_byte(off, pcOff+0x0109);
   off++;
   
   // Patch3 Intensity
   update_drumtimbre_sbyte(off, pcOff+0x010A);
   ////updateDrumTimbre_CC(_timbre, off, 105);
   off++;
   
   // Patch4 Source
   update_drumtimbre_byte(off, pcOff+0x010C);
   off++;
   
   // Patch4 Destination
   update_drumtimbre_byte(off, pcOff+0x010D);
   off++;
   
   // Patch4 Intensity
   update_drumtimbre_sbyte(off, pcOff+0x010E);
   ////updateDrumTimbre_CC(_timbre, off, 106);
   off++;
   
   // Patch5 Source
   update_drumtimbre_byte(off, pcOff+0x0110);
   off++;
   
   // Patch5 Destination
   update_drumtimbre_byte(off, pcOff+0x0111);
   off++;
   
   // Patch5 Intensity
   update_drumtimbre_sbyte(off, pcOff+0x0112);
   ////updateDrumTimbre_CC(_timbre, off, 107);
   off++;
   
   // Patch6 Source
   update_drumtimbre_byte(off, pcOff+0x0114);
   off++;
   
   // Patch6 Destination
   update_drumtimbre_byte(off, pcOff+0x0115);
   off++;
   
   // Patch6 Intensity
   update_drumtimbre_sbyte(off, pcOff+0x0116);
   ////updateDrumTimbre_CC(_timbre, off, 108);
   off++;
}



void YAC_CALL tkradiastools_update_drumkit_timbre(YAC_Object *_drumkitState, 
                                                  YAC_Object *_drumkitData, 
                                                  sSI _timbreNr, 
                                                  YAC_Object *_out,
                                                  sBool _bForce
                                                  ) 
{
   if(YAC_BCHK(_drumkitState, YAC_CLID_BUFFER))
   {
      if(YAC_BCHK(_drumkitData, YAC_CLID_BUFFER))
      {
         if(YAC_BCHK(_out, YAC_CLID_BUFFER))
         {
            YAC_Buffer *drumkitState = (YAC_Buffer *) _drumkitState;
            YAC_Buffer *drumkitData  = (YAC_Buffer *) _drumkitData;
            out = (YAC_Buffer *) _out;
            if( (_timbreNr >= 0) && (_timbreNr < 16) )
            {
               if(drumkitState->size == RADIAS_DRUMKITDATASIZE_DEC)
               {
                  if(drumkitData->size == RADIAS_DRUMKITDATASIZE_DEC)
                  {
                     drumkit_state = drumkitState->buffer;
                     drumkit_data  = drumkitData->buffer;
                     b_force = _bForce;
                     update_drumkit_timbre(_timbreNr);
                  }
               }
            }
         }
      }
   }
}




//-----------------------------------------------------------------------------
//--                                                                         --
//--                                                                         --
//--                                                                         --
//--                                                                         --
//--                                                                         --
//--                                                                         --
//--                                                                         --
//--                                                                         --
//--                                                                         --
//-- tkradiastools_copy_drumkit_timbre                                       --
//--                                                                         --
//--                                                                         --
//--                                                                         --
//--                                                                         --
//--                                                                         --
//--                                                                         --
//--                                                                         --
//--                                                                         --
//-----------------------------------------------------------------------------
static void copy_drumkit_timbre(sU8 *_drumkitState, sSI _dstTimbreNr, sU8 *_drumkitData, sSI _srcTimbreNr) {

   // Just copy the relevant bytes from _drumkitData to _drumkitState
   
   /////int pcOff = 0x1000 + 0x0200 * _timbreNr; // Parameter change offset
   int doff = 52 + (104 * _dstTimbreNr); // byte offset in drumkit state dump (decoded!)
   int soff = 52 + (104 * _srcTimbreNr); // byte offset in drumkit data dump (decoded!)

   
   // TABLE 11 : DRUMKIT PARAMETER
   
   // Program name (SKIP)
   doff += 12;
   soff += 12;
   
   // Dummy bytes (12..15)
   doff += 4;
   soff += 4;
   
   
   // TABLE 2 : TIMBRE PARAMETER
   
   // Voice assign mode, trigger mode, priority
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // Dummy byte (17)
   soff++;
   doff++;
   
   // Analog Tuning
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // Transpose
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // Detune
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // Vibrato Int
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // Osc1 Bus Select, Mod, Wave Type
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // Osc1 Waveform CTRL1
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // Osc1 Waveform CTRL2
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // Osc1 DWGS Wave Select
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // Osc1 PCM Wave Select
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // Osc2 Mod Select, Wave Type
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // Osc2 Semitone
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // Osc2 Tune
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // Mixer Osc1 Level
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // Mixer Osc2 Level
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // Mixer Noise Level
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // Filter Link SW, Filter2 Type, Filter Routing. Important: Send Filter Routing First!!!!!!
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // Filter1 Balance
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // Filter1 Cutoff
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // Filter1 Resonance
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // F1 EG1 Intensity
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // F1 Keyboard Track
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // F1 Velocity Sens
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // Filter2 Cutoff
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // Filter2 Resonance
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // F2 EG1 Intensity
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // F2 Keyboard Track
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // F2 Velocity Sens
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // Amp Level
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // Amp Drive/WS, Mod Type
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // Amp WaveShape Type
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // Amp Driver/Shaper
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // Amp PanPot   xxx signed ?
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // Amp KeyTrack
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // Amp Punch Level
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // EG1 Attack Time
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // EG1 Decay Time
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // EG1 Sustain Level
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // EG1 Release Time
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // EG1 Curve
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // EG1 LevelVelocitySens
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // EG1 TimeVelocitySens
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // EG1 TimeKeyTrack
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // EG2 Attack Time
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // EG2 Decay Time
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // EG2 Sustain Level
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // EG2 Release Time
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // EG2 Curve
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // EG2 LevelVelocitySens
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // EG2 TimeVelocitySens
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // EG2 TimeKeyTrack
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // EG3 Attack Time
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // EG3 Decay Time
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // EG3 Sustain Level
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // EG3 Release Time
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // EG3 Curve
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // EG3 LevelVelocitySens
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // EG3 TimeVelocitySens
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // EG3 TimeKeyTrack
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // LFO1 Wave
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // LFO1 Shape
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // LFO1 Frequency
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // LFO1 Tempo Sync, Key Sync, Init Phase
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // LFO1 Sync Note
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // LFO2 Wave
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // LFO2 Shape
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // LFO2 Frequency
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // LFO2 Tempo Sync, Key Sync, Init Phase
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // LFO2 Sync Note
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // Patch1 Source
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // Patch1 Destination
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // Patch1 Intensity
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // Patch2 Source
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // Patch2 Destination
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // Patch2 Intensity
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // Patch3 Source
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // Patch3 Destination
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // Patch3 Intensity
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // Patch4 Source
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // Patch4 Destination
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // Patch4 Intensity
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // Patch5 Source
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // Patch5 Destination
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // Patch5 Intensity
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // Patch6 Source
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // Patch6 Destination
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
   
   // Patch6 Intensity
   _drumkitState[doff] = _drumkitData[soff];
   doff++;
   soff++;
}


void tkradiastools_copy_drumkit_timbre(YAC_Object *_drumkitState, sSI _dstTimbreNr, YAC_Object *_drumkitData, sSI _srcTimbreNr) 
{
   if(YAC_BCHK(_drumkitState, YAC_CLID_BUFFER))
   {
      if(YAC_BCHK(_drumkitData, YAC_CLID_BUFFER))
      {
         YAC_Buffer *drumkitState = (YAC_Buffer *) _drumkitState;
         YAC_Buffer *drumkitData  = (YAC_Buffer *) _drumkitData;
         if( (_srcTimbreNr >= 0) && (_srcTimbreNr < 16) )
         {
            if( (_dstTimbreNr >= 0) && (_dstTimbreNr < 16) )
            {
               if(drumkitState->size == RADIAS_DRUMKITDATASIZE_DEC)
               {
                  if(drumkitData->size == RADIAS_DRUMKITDATASIZE_DEC)
                  {
                     copy_drumkit_timbre(drumkitState->buffer, _dstTimbreNr, drumkitData->buffer, _srcTimbreNr);
                  }
               }
            }
         }
      }
   }
}


static void update_program(sBool _bSendFX) {
   sSI off = 0;
   sSI pcOff = 0;
   sU8 sv;
   sU8 dv;

   // Name
   for(sSI nameIdx = 0; nameIdx < 12; nameIdx++)
   {
      update_program_timbre_byte(off++, pcOff+nameIdx);
   }

   // 4 Dummy bytes (12~15)
   off += 4;

   // Category (16)
   update_program_timbre_byte(off++, pcOff+0x10);

   // Dummy byte (17)
   off++;

#if 0
   // Vocoder select + timbre select
   //   no-op ??!
   sv = prog_state[off];
   dv = prog_data[off];
   if(b_force || (sv != dv))
   {
      if(b_force || ( (sv&0x01) != (dv&0x01) ) ) // Timbre1 select
      {      
         update_program_timbre_add_pc(pcOff+0x12, (dv&0x01) >> 0);
      }

      if(b_force || ( (sv&0x02) != (dv&0x02) ) ) // Timbre2 select
      {      
         update_program_timbre_add_pc(pcOff+0x13, (dv&0x02) >> 1);
      }

      if(b_force || ( (sv&0x04) != (dv&0x04) ) ) // Timbre3 select
      {      
         update_program_timbre_add_pc(pcOff+0x14, (dv&0x04) >> 2);
      }

      if(b_force || ( (sv&0x08) != (dv&0x08) ) ) // Timbre4 select
      {      
         update_program_timbre_add_pc(pcOff+0x15, (dv&0x08) >> 3);
      }

      if(b_force || ( (sv&0x10) != (dv&0x10) ) ) // Vocoder select
      {      
         update_program_timbre_add_pc(pcOff+0x16, (dv&0x10) >> 4);
      }

      prog_state[off] = dv;
   }
#endif
   off++;

   // Scale (19)
   sv = prog_state[off];
   dv = prog_data[off];
   if(b_force || (sv != dv))
   {
      if(b_force || ( (sv&0x0F) != (dv&0x0F) ) ) // Scale Type
      {      
         update_program_timbre_add_pc(pcOff+0x17, (dv&0x0F) >> 0);
      }

      if(b_force || ( (sv&0xF0) != (dv&0xF0) ) ) // Scale Key
      {      
         update_program_timbre_add_pc(pcOff+0x18, (dv&0xF0) >> 0);
      }
   }
   off++;

   // Dummy byte (20)
   off++;

   // Dummy byte (21)
   off++;

   // Illumination mode (22)
   update_program_timbre_byte(off++, pcOff+0x1F);

   // Link panel edit etc (23)
   sv = prog_state[off];
   dv = prog_data[off];
   if(b_force || (sv != dv))
   {
      if(b_force || ( (sv&0x01) != (dv&0x01) ) ) // Link Panel Edit
      {      
         update_program_timbre_add_pc(pcOff+0x20, (dv&0x01) >> 0);
      }

#if 0
      // no-op ??!
      if(b_force || ( (sv&0x02) != (dv&0x02) ) ) // Timb1 panel sel
      {      
         update_program_timbre_add_pc(pcOff+0x21, (dv&0x02) >> 1);
      }

      if(b_force || ( (sv&0x04) != (dv&0x04) ) ) // Timb2 panel sel
      {      
         update_program_timbre_add_pc(pcOff+0x22, (dv&0x04) >> 2);
      }

      if(b_force || ( (sv&0x08) != (dv&0x08) ) ) // Timb3 panel sel
      {      
         update_program_timbre_add_pc(pcOff+0x23, (dv&0x08) >> 3);
      }

      if(b_force || ( (sv&0x10) != (dv&0x10) ) ) // Timb4 panel sel
      {      
         update_program_timbre_add_pc(pcOff+0x24, (dv&0x10) >> 4);
      }

      if(b_force || ( (sv&0x20) != (dv&0x20) ) ) // Vocoder panel sel
      {      
         update_program_timbre_add_pc(pcOff+0x25, (dv&0x20) >> 5);
      }
#endif
   }
   off++;

   // Drum bank + timbre (24)
   sv = prog_state[off];
   dv = prog_data[off];
   if(b_force || (sv != dv))
   {
      if(b_force || ( (sv&0x1F) != (dv&0x1F) ) ) // Drum Bank Select
      {      
         update_program_timbre_add_pc(pcOff+0x28, (dv&0x1F) >> 0);
      }

      if(b_force || ( (sv&0xE0) != (dv&0xE0) ) ) // Drum Timbre Select
      {      
         update_program_timbre_add_pc(pcOff+0x29, (dv&0xE0) >> 5);
      }
   }
   off++;

   // Drum Level (25)
   update_program_timbre_byte(off++, pcOff+0x2A);

   // Drum Pan Pot (26)
   update_program_timbre_byte(off++, pcOff+0x2B);

   // Drum note shift (27)
   update_program_timbre_sbyte(off++, pcOff+0x2C);

   // Dummy bytes (28..31)
   off += 4;

   // Envelope follower: Input source (32)
   update_program_timbre_byte(off++, pcOff+0x30);

   // Envelope follower: Sensitivity (33)
   update_program_timbre_byte(off++, pcOff+0x31);

   // Envelope follower: Response (34)
   update_program_timbre_byte(off++, pcOff+0x32);

   // Envelope follower: Trigger Threshold (35)
   update_program_timbre_byte(off++, pcOff+0x33);

   // Dummy bytes (36..39)
   off += 4;

   // Trigger notes 1..8 (40..47)
   for(int triggerNoteIdx = 0; triggerNoteIdx < 8; triggerNoteIdx++)
   {
      update_program_timbre_byte(off++, pcOff+0x38+triggerNoteIdx);
   }

   // MFX
   if(_bSendFX)
   {
      // (note) docs say 1030 but it's actually 1038
      update_mfx(1038/*off*/, 0x6000/*pcOff*/);
   }

   // Vocoder
   // (note) docs say 952 but it's actually 960
   update_vocoder(960/*off*/, 0x5000/*pcOff*/);

}

void YAC_CALL tkradiastools_update_program(YAC_Object *_progState, 
                                           YAC_Object *_progData,
                                           sBool _bSendFX,
                                           YAC_Object *_out, 
                                           YAC_Object *_cc,
                                           sBool _bForce
                                           ) 
{
   if(YAC_BCHK(_progState, YAC_CLID_BUFFER))
   {
      if(YAC_BCHK(_progData, YAC_CLID_BUFFER))
      {
         if(YAC_BCHK(_out, YAC_CLID_BUFFER))
         {
            if(YAC_BCHK(_cc, YAC_CLID_BUFFER))
            {
               YAC_Buffer *progState = (YAC_Buffer *) _progState;
               YAC_Buffer *progData  = (YAC_Buffer *) _progData;
               out        = (YAC_Buffer *) _out;
               cc         = (YAC_Buffer *) _cc;
               if(progState->size == RADIAS_PROGRAMDATASIZE_DEC)
               {
                  if(progData->size == RADIAS_PROGRAMDATASIZE_DEC)
                  {
                     prog_state = progState->buffer;
                     prog_data  = progData->buffer;
                     b_force = _bForce;
                     update_program(_bSendFX);
                     ////yac_host->printf("xxx program updated.\n");
                  }
               }
            }
         }
      }
   }
}



#include <yac_host.cpp>
