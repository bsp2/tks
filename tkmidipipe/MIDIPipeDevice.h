// ----
// ---- file   : MIDIPipeDevice.h
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2014-2025 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : This is part of the "syntracker" sequencer.
// ----
// ---- created: 01Sep2014
// ---- changed: 11Jan2015, 16Feb2015, 11Sep2015, 14Sep2015, 09Sep2016, 03Dec2016, 03Mar2017
// ----          22Nov2017, 22Jan2019, 15Apr2020, 25Jul2021, 24Apr2022, 14Jun2023, 26Jun2023
// ----          08Jul2023, 06Oct2024, 27May2025
// ----
// ----
// ----

#ifndef TK_MIDIPIPEDEVICE_H__
#define TK_MIDIPIPEDEVICE_H__

YG("midipipe")


class midipipe_ctl_state_t {
  public:
   sS16 value;  // -1=undef
};

class midipipe_note_state_t {
  public:
   sS32 ticks_left;  // -1=note inactive, 0=manual note-off, >0=auto-off countdown
   sS16 poly_pressure; // -1=undef
   /* sS16 velocity; */
};

class midipipe_device_ch_t {
  public:
   sS16 prg;
   sS16 ch_pressure;
   sS16 pitchbend;
   midipipe_ctl_state_t  cc_states[128];
   midipipe_ctl_state_t  rpn_states[16384];
   midipipe_ctl_state_t  nrpn_states[16384];
   midipipe_note_state_t note_states[128];
   sS16 note_low;
   sU16 note_hi;
   sS16 num_notes;
};


/* @class MIDIPipeDevice,Object
 */
YC class MIDIPipeDevice : public YAC_Object {
/* #define MIDIPIPEDEVICE_MAX_OVERFLOW_BUFFERS  0 */
#define MIDIPIPEDEVICE_MAX_OVERFLOW_BUFFERS  1024
/* #define MIDIPIPEDEVICE_OVERFLOW_BUFFER_SZ      48   // (note) patchdiff glitches w/ Sequential Pro 3 (macOS) when set >64 */
#define MIDIPIPEDEVICE_OVERFLOW_BUFFER_SZ      32   // (note) patchdiff glitches w/ Sequential/Oberheim OB-X8 (macOS) when set >32
  public:
   sUI dev_idx;
   YAC_Buffer *cmd_buffer;   // ref to STX_MIDIOutDevice.cmd_buffer
   YAC_Buffer *note_buffer;  // ref to STX_MIDIOutDevice.note_buffer

   // 1=spread data that exceeds USB MIDI frame size over multiple packets
   //   (note) added as a work-around for Sequential Pro3 MIDI issues
   //   (note) activated via MIDI synth profile 'overflowBuffers' attribute
   sBool       b_use_overflow_buffers;
   YAC_Buffer *overflow_cmd_buffers [MIDIPIPEDEVICE_MAX_OVERFLOW_BUFFERS];
   YAC_Buffer *overflow_note_buffers[MIDIPIPEDEVICE_MAX_OVERFLOW_BUFFERS];
   sUI         write_overflow_buffer_idx;
   sUI         read_overflow_buffer_idx;
   sUI         num_used_overflow_buffers; // 0..MIDIPIPEDEVICE_MAX_OVERFLOW_BUFFERS
   sUI         num_bytes_written;
   sBool       b_overflow;  // 1=Dgetbufs() returns current overflow buffer

   midipipe_device_ch_t channels[16];        // current state

   // Shadow copy of channels, never reset (for getCCState(), ..)
   //  (todo) only used for notenr (which resets in channels[] at note off) => optimize
   midipipe_device_ch_t shadow_channels[16];

   sBool b_midicache;
   sBool b_note_off;  // 1=use 0x8x for note off, 0=use 0x9x vel=0 for note off
   sBool b_global;    // true=use output of last pipe as input to next pipe
   sBool b_virtual;   // virtual/internal device. skipped by MIDIPipeFrame::emit()

   sSI param_mode[16];  // 0=MSBLSB, 1=LSBMSB, 2=MSB (see MIDIIn.h)

   sBool b_cc_perf_lsb[16];  // emit 14bit MSB+LSB performance controller CCs

   sBool b_channel_pressure_and_note_off;  // 1=allow channel pressure and note off events in same MIDI buffer
   sBool running_status[16];  // 1=allow running status

  public:
   MIDIPipeDevice(void);
   ~MIDIPipeDevice();

   YAC(MIDIPipeDevice);

  public:
   void setDevIdx(sUI _devIdx);

   void selectNextOverflowBuffer (void);

  public:
   YM void setBuffers(YAC_Buffer *_cmdBuffer, YAC_Buffer *_noteBuffer);

   YM void  setEnableUseOverflowBuffers (sBool _bUseOverflowBuffers);
   YM sBool getEnableUseOverflowBuffers (void);

   YM void  setEnableMIDICache (sBool _bEnable);
   YM sBool getEnableMIDICache (void);

   YM void setEnableNoteOffMsg (sBool _bEnable);  // 1=use 0x8x for note off, 0=use 0x9x vel=0 for note off
   YM sBool getEnableNoteOffMsg (void);

   YM void  setEnableGlobal (sBool _bEnable);
   YM sBool getEnableGlobal (void);

   YM void  setEnableVirtual (sBool _bEnable);
   YM sBool getEnableVirtual (void);

   // (note) ret=-1 => undef
   YM sSI getPrg (sUI _ch);
   YM sSI getChPressure (sUI _ch);
   YM sSI getPitchbend (sUI _ch);
   YM sSI getCCState (sUI _ch, sUI _cc);
   YM sSI getRPNState (sUI _ch, sUI _rpn);
   YM sSI getNRPNState (sUI _ch, sUI _nrpn);
   YM sSI getPolyPressure (sUI _ch, sUI _note);

   YM void setPrg (sUI _ch, sUI _val);
   YM void setChPressure (sUI _ch, sUI _val);
   YM void setPitchbend (sUI _ch, sUI _val);
   YM void setCCState (sUI _ch, sUI _cc, sUI _val);
   YM void setRPNState (sUI _ch, sUI _rpn, sUI _val);
   YM void setNRPNState (sUI _ch, sUI _nrpn, sUI _val);
   YM void setPolyPressure (sUI _ch, sUI _note, sUI _val);

   // 0=MSBLSB, 1=LSBMSB, 2=MSB  (see MIDIIn.h:MIDI_DATAENTRYMODE_xxx)
   YM void setParamMode (sUI _ch, sSI _mode);
   YM sSI  getParamMode (sUI _ch);

   // 1=emit 14bit MSB+LSB performance controller CCs
   YM void setEnableCCPerfLSB (sUI _ch, sBool _bEnable);

   // workaround for Novation Peak (FW 2.0) hanging notes issue  (e.g. "D0 00 80 43 00")
   //  - when disabled, don't allow channel pressure messages if frame contains note offs
   //     (note) poly pressure works fine (for some reason)
   YM void  setEnableChannelPressureAndNoteOff (sBool _bEnable);
   YM sBool getEnableChannelPressureAndNoteOff (void);

   YM void  setEnableRunningStatus (sUI _ch, sBool _bEnable);
   YM sBool getEnableRunningStatus (sUI _ch);

   YM void clearMIDICache (void);
   YM void clearMIDICacheForCh (sUI _ch);

};


#endif // TK_MIDIPIPEDEVICE_H__
