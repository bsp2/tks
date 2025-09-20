/// ModInputFilter.h
///
/// (c) 2018-2024 Bastian Spiegel <bs@tkscript.de>
///     - Distributed under terms of the Lesser GNU General Public License (LGPL).
///       See COPYING and <http://www.gnu.org/licenses/licenses.html#LGPL> for further information.
///
///
/// created: 23Feb2018
/// changed: 16Mar2018, 17Mar2018, 30Jul2019, 26Aug2019, 20May2021, 01Jul2024
///
///

#ifndef MODINPUTFILTER_H__
#define MODINPUTFILTER_H__


YG("midipipe");


YC class ModInputFilter : public YAC_Object {
   // (note) utility class used by "Eureka" DAW
   // (note) can be passed to VSTPlugin::queueHostVstMidiEventsByFlt()

public:
   sBool b_note_on;
   sBool b_note_off;
   sBool b_pitchbend;
   sBool b_channel_pressure;
   sBool b_poly_pressure;
   sBool b_prg_chg;
   sBool b_cc_bankselect;       // CC0 + CC32
   sBool b_cc_modwheel;         // CC1 + CC33
   sBool b_cc_breathcontrol;    // CC2 + CC34
   sBool b_cc_footcontrol;      // CC4 + CC36
   sBool b_cc_expression;       // CC11 + CC43
   sBool b_cc_hold1sw;          // CC64
   sBool b_cc_hold2sw;          // CC69
   sBool b_cc_portamentosw;     // CC65
   sBool b_cc_sostenutosw;      // CC66
   sBool b_cc_softpedalsw;      // CC67
   sBool b_cc_legatosw;         // CC68
   sBool b_cc_other;            // other CCs
   sBool b_sysex;

   sU8 note_min;  // Note range
   sU8 note_max;
   sS8 note_transpose;

   sU8 vel_min; // Velocity range
   sU8 vel_max;

   sBool b_vrr;            // 1=enable VRR mode/filter  (VRR=voice round robin)
   sUI   vrr_note_on_idx;  // allow input frame note on #idx (chords / lane filter)
   sSI   vrr_note;         // current note. -1=none, 0..127 otherwise

public:
   YAC(ModInputFilter);

   ModInputFilter(void);
   ~ModInputFilter();

   void YAC_VCALL yacOperatorAssign (YAC_Object*);

  public:
   YM void loadDefaultsForInstrument (void);
   YM void loadDefaultsForEffect (void);

   YM void  setEnableNoteOn (sBool _bEnable);
   YM sBool getEnableNoteOn (void);

   YM void  setEnableNoteOff (sBool _bEnable);
   YM sBool getEnableNoteOff (void);

   YM void  setEnablePitchbend (sBool _bEnable);
   YM sBool getEnablePitchbend (void);

   YM void  setEnableChannelPressure (sBool _bEnable);
   YM sBool getEnableChannelPressure (void);

   YM void  setEnablePolyPressure (sBool _bEnable);
   YM sBool getEnablePolyPressure (void);

   YM void  setEnablePrgChg (sBool _bEnable);
   YM sBool getEnablePrgChg (void);

   YM void  setEnableCCBankSelect (sBool _bEnable);
   YM sBool getEnableCCBankSelect (void);

   YM void  setEnableCCModWheel (sBool _bEnable);
   YM sBool getEnableCCModWheel (void);

   YM void  setEnableCCBreathControl (sBool _bEnable);
   YM sBool getEnableCCBreathControl (void);

   YM void  setEnableCCFootControl (sBool _bEnable);
   YM sBool getEnableCCFootControl (void);

   YM void  setEnableCCExpression (sBool _bEnable);
   YM sBool getEnableCCExpression (void);

   YM void  setEnableCCHold1SW (sBool _bEnable);
   YM sBool getEnableCCHold1SW (void);

   YM void  setEnableCCHold2SW (sBool _bEnable);
   YM sBool getEnableCCHold2SW (void);

   YM void  setEnableCCPortamentoSW (sBool _bEnable);
   YM sBool getEnableCCPortamentoSW (void);

   YM void  setEnableCCSostenutoSW (sBool _bEnable);
   YM sBool getEnableCCSostenutoSW (void);

   YM void  setEnableCCSoftPedalSW (sBool _bEnable);
   YM sBool getEnableCCSoftPedalSW (void);

   YM void  setEnableCCLegatoSW (sBool _bEnable);
   YM sBool getEnableCCLegatoSW (void);

   YM void  setEnableCCOther (sBool _bEnable);
   YM sBool getEnableCCOther (void);

   YM void  setEnableSysEx (sBool _bEnable);
   YM sBool getEnableSysEx (void);

   YM void setNoteMin (sUI _v);
   YM sUI getNoteMin (void);

   YM void setNoteMax (sUI _v);
   YM sUI getNoteMax (void);

   YM void setNoteTranspose (sSI _v);
   YM sSI getNoteTranspose (void);

   YM void setVelocityMin (sUI _v);
   YM sUI getVelocityMin (void);

   YM void setVelocityMax (sUI _v);
   YM sUI getVelocityMax (void);

   YM void setEnableVRR (sBool _bEnable);
   YM sBool getEnableVRR (void);

   YM void setVrrNoteOnIdx (sUI _idx);
   YM sUI getVrrNoteOnIdx (void);

   YM void setVrrNote (sSI _note);
   YM sSI getVrrNote (void);

   // returns 1=ok (allow event), 0=nok (discard event)
   YM sBool _checkMessage (sUI _data0, sUI _data1, sUI _data2);

   virtual sBool checkMessage (sUI _data0, sUI _data1, sUI _data2)
#ifdef TKMIDIPIPE_BUILD
   { return _checkMessage(_data0, _data1, _data2); }
#else
      = 0;
#endif // TKMIDIPIPE_BUILD

   YM sBool saveState (YAC_Object *_ofs);
   YM sBool loadState (YAC_Object *_ifs);

};


#endif // MODINPUTFILTER_H__
