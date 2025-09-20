/// ModInputFilter.cpp
///
/// (c) 2018-2024 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
/// created: 23Feb2018
/// changed: 16Mar2018, 17Mar2018, 30Jul2019, 26Aug2019, 20May2021, 01Jul2024
///
///
///

#include "tkmidipipe.h"

#include <math.h>


ModInputFilter::ModInputFilter(void) {
   loadDefaultsForInstrument();

   note_min = 0;
   note_max = 127;
   note_transpose = 0;

   vel_min = 1;
   vel_max = 127;

   b_vrr           = YAC_FALSE;
   vrr_note_on_idx = 0u;
   vrr_note        = -1;
}

ModInputFilter::~ModInputFilter() {
}

void ModInputFilter::yacOperatorAssign(YAC_Object *_o) {
   if(YAC_BCHK(_o, clid_ModInputFilter))
   {
      YAC_CAST_ARG(ModInputFilter, o, _o);
      *this = *o;
   }
}

void ModInputFilter::loadDefaultsForInstrument(void) {
   b_note_on          = YAC_TRUE;
   b_note_off         = YAC_TRUE;
   b_pitchbend        = YAC_TRUE;
   b_channel_pressure = YAC_TRUE;
   b_poly_pressure    = YAC_TRUE;
   b_prg_chg          = YAC_TRUE;
   b_cc_bankselect    = YAC_TRUE;
   b_cc_modwheel      = YAC_TRUE;
   b_cc_breathcontrol = YAC_TRUE;
   b_cc_footcontrol   = YAC_TRUE;
   b_cc_expression    = YAC_TRUE;
   b_cc_hold1sw       = YAC_TRUE;
   b_cc_hold2sw       = YAC_TRUE;
   b_cc_portamentosw  = YAC_TRUE;
   b_cc_sostenutosw   = YAC_TRUE;
   b_cc_softpedalsw   = YAC_TRUE;
   b_cc_legatosw      = YAC_TRUE;
   b_cc_other         = YAC_TRUE;
   b_sysex            = YAC_TRUE;
}

void ModInputFilter::loadDefaultsForEffect(void) {
   b_note_on          = YAC_FALSE;
   b_note_off         = YAC_FALSE;
   b_pitchbend        = YAC_FALSE;
   b_channel_pressure = YAC_FALSE;
   b_poly_pressure    = YAC_FALSE;
   b_prg_chg          = YAC_FALSE;
   b_cc_bankselect    = YAC_FALSE;
   b_cc_modwheel      = YAC_FALSE;
   b_cc_breathcontrol = YAC_FALSE;
   b_cc_footcontrol   = YAC_FALSE;
   b_cc_expression    = YAC_FALSE;
   b_cc_hold1sw       = YAC_FALSE;
   b_cc_hold2sw       = YAC_FALSE;
   b_cc_portamentosw  = YAC_FALSE;
   b_cc_sostenutosw   = YAC_FALSE;
   b_cc_softpedalsw   = YAC_FALSE;
   b_cc_legatosw      = YAC_FALSE;
   b_cc_other         = YAC_FALSE;
   b_sysex            = YAC_FALSE;
}

void ModInputFilter::setEnableNoteOn(sBool _bEnable) {
   b_note_on = _bEnable;
}

sBool ModInputFilter::getEnableNoteOn(void) {
   return b_note_on;
}

void ModInputFilter::setEnableNoteOff(sBool _bEnable) {
   b_note_off = _bEnable;
}

sBool ModInputFilter::getEnableNoteOff(void) {
   return b_note_off;
}

void ModInputFilter::setEnablePitchbend(sBool _bEnable) {
   b_pitchbend = _bEnable;
}

sBool ModInputFilter::getEnablePitchbend(void) {
   return b_pitchbend;
}

void ModInputFilter::setEnableChannelPressure(sBool _bEnable) {
   b_channel_pressure = _bEnable;
}

sBool ModInputFilter::getEnableChannelPressure(void) {
   return b_channel_pressure;
}

void ModInputFilter::setEnablePolyPressure(sBool _bEnable) {
   b_poly_pressure = _bEnable;
}

sBool ModInputFilter::getEnablePolyPressure(void) {
   return b_poly_pressure;
}

void ModInputFilter::setEnablePrgChg(sBool _bEnable) {
   b_prg_chg = _bEnable;
}

sBool ModInputFilter::getEnablePrgChg(void) {
   return b_prg_chg;
}

void ModInputFilter::setEnableCCBankSelect(sBool _bEnable) {
   b_cc_bankselect = _bEnable;
}

sBool ModInputFilter::getEnableCCBankSelect(void) {
   return b_cc_bankselect;
}

void ModInputFilter::setEnableCCModWheel(sBool _bEnable) {
   b_cc_modwheel = _bEnable;
}

sBool ModInputFilter::getEnableCCModWheel(void) {
   return b_cc_modwheel;
}

void ModInputFilter::setEnableCCBreathControl(sBool _bEnable) {
   b_cc_breathcontrol = _bEnable;
}

sBool ModInputFilter::getEnableCCBreathControl(void) {
   return b_cc_breathcontrol;
}

void ModInputFilter::setEnableCCFootControl(sBool _bEnable) {
   b_cc_footcontrol = _bEnable;
}

sBool ModInputFilter::getEnableCCFootControl(void) {
   return b_cc_footcontrol;
}

void ModInputFilter::setEnableCCExpression(sBool _bEnable) {
   b_cc_expression = _bEnable;
}

sBool ModInputFilter::getEnableCCExpression(void) {
   return b_cc_expression;
}

void ModInputFilter::setEnableCCHold1SW(sBool _bEnable) {
   b_cc_hold1sw = _bEnable;
}

sBool ModInputFilter::getEnableCCHold1SW(void) {
   return b_cc_hold1sw;
}

void ModInputFilter::setEnableCCHold2SW(sBool _bEnable) {
   b_cc_hold2sw = _bEnable;
}

sBool ModInputFilter::getEnableCCHold2SW(void) {
   return b_cc_hold2sw;
}

void ModInputFilter::setEnableCCPortamentoSW(sBool _bEnable) {
   b_cc_portamentosw = _bEnable;
}

sBool ModInputFilter::getEnableCCPortamentoSW(void) {
   return b_cc_portamentosw;
}

void ModInputFilter::setEnableCCSostenutoSW(sBool _bEnable) {
   b_cc_sostenutosw = _bEnable;
}

sBool ModInputFilter::getEnableCCSostenutoSW(void) {
   return b_cc_sostenutosw;
}

void ModInputFilter::setEnableCCSoftPedalSW(sBool _bEnable) {
   b_cc_softpedalsw = _bEnable;
}

sBool ModInputFilter::getEnableCCSoftPedalSW(void) {
   return b_cc_softpedalsw;
}

void ModInputFilter::setEnableCCLegatoSW(sBool _bEnable) {
   b_cc_legatosw = _bEnable;
}

sBool ModInputFilter::getEnableCCLegatoSW(void) {
   return b_cc_legatosw;
}

void ModInputFilter::setEnableCCOther(sBool _bEnable) {
   b_cc_other = _bEnable;
}

sBool ModInputFilter::getEnableCCOther(void) {
   return b_cc_other;
}

void ModInputFilter::setEnableSysEx(sBool _bEnable) {
   b_sysex = _bEnable;
}

sBool ModInputFilter::getEnableSysEx(void) {
   return b_sysex;
}

void ModInputFilter::setNoteMin(sUI _v) {
   note_min = sU8(_v);
}

sUI ModInputFilter::getNoteMin(void) {
   return note_min;
}

void ModInputFilter::setNoteMax(sUI _v) {
   note_max = sU8(_v);
}

sUI ModInputFilter::getNoteMax(void) {
   return note_max;
}

void ModInputFilter::setNoteTranspose(sSI _v) {
   note_transpose = sS8(_v);
}

sSI ModInputFilter::getNoteTranspose(void) {
   return note_transpose;
}

void ModInputFilter::setVelocityMin(sUI _v) {
   vel_min = sU8(_v);
}

sUI ModInputFilter::getVelocityMin(void) {
   return vel_min;
}

void ModInputFilter::setVelocityMax(sUI _v) {
   vel_max = sU8(_v);
}

sUI ModInputFilter::getVelocityMax(void) {
   return vel_max;
}

void ModInputFilter::setEnableVRR(sBool _bEnable) {
   b_vrr = _bEnable;
}

sBool ModInputFilter::getEnableVRR(void) {
   return b_vrr;
}

void ModInputFilter::setVrrNoteOnIdx(sUI _idx) {
   vrr_note_on_idx = _idx;
}

sUI ModInputFilter::getVrrNoteOnIdx(void) {
   return vrr_note_on_idx;
}

void ModInputFilter::setVrrNote(sSI _note) {
   vrr_note = _note;
}

sSI ModInputFilter::getVrrNote(void) {
   return vrr_note;
}

sBool ModInputFilter::_checkMessage(sUI _data0, sUI _data1, sUI _data2) {
   // called by VST2Plugin::queueHostVstMidiEventsByFlt()
   switch(_data0 & 0xF0u)
   {
      default:
         return YAC_FALSE;

      case 0x80u:
         // apply note range filter ?
         return b_note_off;

      case 0x90u:
         if(b_note_on)
         {
            sU8 noteMin = sMIN(note_min, note_max);
            sU8 noteMax = sMAX(note_min, note_max);
            sU8 velMin = sMIN(vel_min, vel_max);
            sU8 velMax = sMAX(vel_min, vel_max);
            return
               (_data1 >= noteMin) &&
               (_data1 <= noteMax) &&
               (_data2 >= velMin)  &&
               (_data2 <= velMax)  ;
         }
         return YAC_FALSE;

      case 0xA0u:
         if(b_poly_pressure)
         {
            sU8 noteMin = sMIN(note_min, note_max);
            sU8 noteMax = sMAX(note_min, note_max);
            return
               (_data1 >= noteMin) &&
               (_data1 <= noteMax) ;
         }
         return YAC_FALSE;

      case 0xB0u:
         switch(_data1)
         {
            default:
               break;

            case 0:
            case 32:
#if 0
               // (note) not editable in UI, use "other" setting instead
               return b_cc_bankselect;
#else
               return b_cc_other;
#endif

            case 1:
            case 33:
               return b_cc_modwheel;

            case 2:
            case 34:
               return b_cc_breathcontrol;

            case 4:
            case 36:
               return b_cc_footcontrol;

            case 11:
            case 43:
               return b_cc_expression;

            case 64:
               return b_cc_hold1sw;

            case 69:
               return b_cc_hold2sw;

            case 65:
               return b_cc_portamentosw;

            case 66:
               return b_cc_sostenutosw;

            case 67:
               return b_cc_softpedalsw;

            case 68:
               return b_cc_legatosw;
         }
         return b_cc_other;

      case 0xC0u:
         return b_prg_chg;

      case 0xD0u:
         return b_channel_pressure;

      case 0xE0u:
         return b_pitchbend;

      case 0xF0u:
         return b_sysex;
   }
}

sBool ModInputFilter::saveState(YAC_Object *_ofs) {
   // Version
   _ofs->yacStreamWriteI16(4);

   _ofs->yacStreamWriteI8(b_note_on);
   _ofs->yacStreamWriteI8(b_note_off);
   _ofs->yacStreamWriteI8(b_pitchbend);
   _ofs->yacStreamWriteI8(b_channel_pressure);
   _ofs->yacStreamWriteI8(b_poly_pressure);
   _ofs->yacStreamWriteI8(b_prg_chg);
   _ofs->yacStreamWriteI8(b_cc_bankselect);    // v2+
   _ofs->yacStreamWriteI8(b_cc_modwheel);      // v2+
   _ofs->yacStreamWriteI8(b_cc_breathcontrol); // v2+
   _ofs->yacStreamWriteI8(b_cc_footcontrol);   // v2+
   _ofs->yacStreamWriteI8(b_cc_expression);    // v2+
   _ofs->yacStreamWriteI8(b_cc_hold1sw);       // v2+
   _ofs->yacStreamWriteI8(b_cc_hold2sw);       // v2+
   _ofs->yacStreamWriteI8(b_cc_portamentosw);  // v2+
   _ofs->yacStreamWriteI8(b_cc_sostenutosw);   // v2+
   _ofs->yacStreamWriteI8(b_cc_softpedalsw);   // v2+
   _ofs->yacStreamWriteI8(b_cc_legatosw);      // v2+
   _ofs->yacStreamWriteI8(b_cc_other);
   _ofs->yacStreamWriteI8(b_sysex);
   _ofs->yacStreamWriteI8(note_min);        // v3+
   _ofs->yacStreamWriteI8(note_max);        // v3+
   _ofs->yacStreamWriteI8(note_transpose);  // v4+
   _ofs->yacStreamWriteI8(vel_min);         // v3+
   _ofs->yacStreamWriteI8(vel_max);         // v3+

   return YAC_TRUE;
}

sBool ModInputFilter::loadState(YAC_Object *_ifs) {
   // Version
   sU16 ver = sUI(_ifs->yacStreamReadI16()) & 65535u;

   if(ver >= 1u)
   {
      b_note_on          = _ifs->yacStreamReadI8() ? YAC_TRUE : YAC_FALSE;
      b_note_off         = _ifs->yacStreamReadI8() ? YAC_TRUE : YAC_FALSE;
      b_pitchbend        = _ifs->yacStreamReadI8() ? YAC_TRUE : YAC_FALSE;
      b_channel_pressure = _ifs->yacStreamReadI8() ? YAC_TRUE : YAC_FALSE;
      b_poly_pressure    = _ifs->yacStreamReadI8() ? YAC_TRUE : YAC_FALSE;
      b_prg_chg          = _ifs->yacStreamReadI8() ? YAC_TRUE : YAC_FALSE;

      if(ver >= 2u)
      {
         b_cc_bankselect    = _ifs->yacStreamReadI8() ? YAC_TRUE : YAC_FALSE;  // v2+
         b_cc_modwheel      = _ifs->yacStreamReadI8() ? YAC_TRUE : YAC_FALSE;  // v2+
         b_cc_breathcontrol = _ifs->yacStreamReadI8() ? YAC_TRUE : YAC_FALSE;  // v2+
         b_cc_footcontrol   = _ifs->yacStreamReadI8() ? YAC_TRUE : YAC_FALSE;  // v2+
         b_cc_expression    = _ifs->yacStreamReadI8() ? YAC_TRUE : YAC_FALSE;  // v2+
         b_cc_hold1sw       = _ifs->yacStreamReadI8() ? YAC_TRUE : YAC_FALSE;  // v2+
         b_cc_hold2sw       = _ifs->yacStreamReadI8() ? YAC_TRUE : YAC_FALSE;  // v2+
         b_cc_portamentosw  = _ifs->yacStreamReadI8() ? YAC_TRUE : YAC_FALSE;  // v2+
         b_cc_sostenutosw   = _ifs->yacStreamReadI8() ? YAC_TRUE : YAC_FALSE;  // v2+
         b_cc_softpedalsw   = _ifs->yacStreamReadI8() ? YAC_TRUE : YAC_FALSE;  // v2+
         b_cc_legatosw      = _ifs->yacStreamReadI8() ? YAC_TRUE : YAC_FALSE;  // v2+
      }

      b_cc_other         = _ifs->yacStreamReadI8() ? YAC_TRUE : YAC_FALSE;

      if(1u == ver)
      {
         b_cc_bankselect    = b_cc_other;
         b_cc_modwheel      = b_cc_other;
         b_cc_breathcontrol = b_cc_other;
         b_cc_footcontrol   = b_cc_other;
         b_cc_expression    = b_cc_other;
         b_cc_hold1sw       = b_cc_other;
         b_cc_hold2sw       = b_cc_other;
         b_cc_portamentosw  = b_cc_other;
         b_cc_sostenutosw   = b_cc_other;
         b_cc_softpedalsw   = b_cc_other;
         b_cc_legatosw      = b_cc_other;
      }

      b_sysex            = _ifs->yacStreamReadI8() ? YAC_TRUE : YAC_FALSE;

      if(ver >= 3u)
      {
         note_min = _ifs->yacStreamReadI8() & 127u;  // v3+
         note_max = _ifs->yacStreamReadI8() & 127u;  // v3+
         if(ver >= 4u)
            note_transpose = (sS8)_ifs->yacStreamReadI8();  // v4+
         vel_min  = _ifs->yacStreamReadI8() & 127u;  // v3+
         vel_max  = _ifs->yacStreamReadI8() & 127u;  // v3+
      }

      return YAC_TRUE;
   }
   // else: bad version
   return YAC_FALSE;
}
