// ----
// ---- file   : MIDIPipeDevice.cpp
// ---- author : Bastian Spiegel <bs@tkscript.de>
// ---- legal  : (c) 2014-2024 by Bastian Spiegel.
// ----          Distributed under terms of the GNU LESSER GENERAL PUBLIC LICENSE (LGPL). See
// ----          http://www.gnu.org/licenses/licenses.html#LGPL or COPYING for further information.
// ----
// ---- info   : This is part of the "syntracker" sequencer.
// ----
// ---- created: 01Sep2014
// ---- changed: 11Sep2015, 14Sep2015, 17Jul2016, 09Sep2016, 03Dec2016, 03Mar2017, 27May2017
// ----          18Aug2017, 22Jan2019, 25Jul2021, 24Apr2022, 14Jun2023, 26Jun2023, 08Jul2023
// ----
// ----
// ----

#include "tkmidipipe.h"


MIDIPipeDevice::MIDIPipeDevice(void) {
   dev_idx = 0;
   cmd_buffer = NULL;
   note_buffer = NULL;
   b_use_overflow_buffers = YAC_FALSE;

   // (note) buffers are lazy-alloc'd when setBuffers() is called
   for(sUI i = 0u; i < MIDIPIPEDEVICE_MAX_OVERFLOW_BUFFERS; i++)
   {
      overflow_cmd_buffers [i] = NULL;
      overflow_note_buffers[i] = NULL;
   }
   read_overflow_buffer_idx  = 1u;
   write_overflow_buffer_idx = 0u;
   num_used_overflow_buffers = 0u;
   num_bytes_written         = 0u;
   b_overflow                = YAC_FALSE;

   memset(channels, 0xFF, sizeof(midipipe_device_ch_t) * 16);
   memset(shadow_channels, 0xFF, sizeof(midipipe_device_ch_t) * 16);

   for(sUI chIdx = 0u; chIdx < 16u; chIdx++)
   {
      channels[chIdx].num_notes = 0;
      shadow_channels[chIdx].num_notes = 0;
      param_mode[chIdx] = 0; // MSBLSB
      b_cc_perf_lsb[chIdx] = YAC_FALSE;
      running_status[chIdx] = YAC_FALSE;
   }

   // memset(channel_resets, 0xFF, sizeof(midipipe_device_ch_t) * 16);
   b_midicache = YAC_TRUE;

   b_note_off = YAC_TRUE;
   // b_note_off = YAC_FALSE;

   b_global = YAC_FALSE;

   b_virtual = YAC_FALSE;

   b_channel_pressure_and_note_off = YAC_TRUE;
}

MIDIPipeDevice::~MIDIPipeDevice(void) {
#if MIDIPIPEDEVICE_MAX_OVERFLOW_BUFFERS > 0u
   for(sUI i = 0u; i < MIDIPIPEDEVICE_MAX_OVERFLOW_BUFFERS; i++)
   {
      YAC_DELETE_SAFE(overflow_cmd_buffers [i]);
      YAC_DELETE_SAFE(overflow_note_buffers[i]);
   }
#endif // MIDIPIPEDEVICE_MAX_OVERFLOW_BUFFERS > 0u
}

void MIDIPipeDevice::setDevIdx(sUI _devIdx) {
   dev_idx = _devIdx;
}

void MIDIPipeDevice::selectNextOverflowBuffer(void) {
#if MIDIPIPEDEVICE_MAX_OVERFLOW_BUFFERS > 0u
   if(num_used_overflow_buffers < MIDIPIPEDEVICE_MAX_OVERFLOW_BUFFERS)
   {
      write_overflow_buffer_idx = (write_overflow_buffer_idx + 1u) % MIDIPIPEDEVICE_MAX_OVERFLOW_BUFFERS;
      Dprintf("[>>>] MIDIPipeDevice::selectNextOverflowBuffer: new write_overflow_buffer_idx=%u\n", write_overflow_buffer_idx);
      num_used_overflow_buffers++;
   }
   else
   {
      yac_host->printf("[!!!] MIDIPipeDevice::selectNextOverflowBuffer: LOST EVENTS, re-using write_overflow_buffer_idx=%u\n", write_overflow_buffer_idx);
   }
   YAC_Buffer *buf = overflow_cmd_buffers[write_overflow_buffer_idx];
   buf->io_offset = 0u;
   buf = overflow_note_buffers[write_overflow_buffer_idx];
   buf->io_offset = 0u;
#endif // MIDIPIPEDEVICE_MAX_OVERFLOW_BUFFERS > 0u
}

void MIDIPipeDevice::setBuffers(YAC_Buffer *_cmdBuffer, YAC_Buffer *_noteBuffer) {
   cmd_buffer  = _cmdBuffer;
   note_buffer = _noteBuffer;

#if MIDIPIPEDEVICE_MAX_OVERFLOW_BUFFERS > 0u
   if(NULL == overflow_cmd_buffers[0])
   {
      // (note) buffers are lazy-alloc'd when setBuffers() is called
      for(sUI i = 0u; i < MIDIPIPEDEVICE_MAX_OVERFLOW_BUFFERS; i++)
      {
         YAC_Buffer *buf = YAC_New_Buffer();
         buf->yacArrayAlloc(MIDIPIPEDEVICE_OVERFLOW_BUFFER_SZ);
         overflow_cmd_buffers [i] = buf;

         buf = YAC_New_Buffer();
         buf->yacArrayAlloc(MIDIPIPEDEVICE_OVERFLOW_BUFFER_SZ);
         overflow_note_buffers[i] = buf;
      }
   }
#endif // MIDIPIPEDEVICE_MAX_OVERFLOW_BUFFERS > 0u

}

void MIDIPipeDevice::setEnableUseOverflowBuffers(sBool _bUseOverflowBuffers) {
   // yac_host->printf("xxx MIDIPipeDevice::setEnableUseOverflowBuffers: bUseOverflowBuffers=%d\n", _bUseOverflowBuffers);
   b_use_overflow_buffers = _bUseOverflowBuffers;
}

sBool MIDIPipeDevice::getEnableUseOverflowBuffers(void) {
   return b_use_overflow_buffers;
}

void MIDIPipeDevice::clearMIDICache(void) {
   // (note) do not reset note data to prevent hanging notes
   // // memset(channels, 0xFF, sizeof(midipipe_device_ch_t) * 16);
   // yac_host->printf("xxx MIDIPipeDevice::clearMIDICache: dev_idx=%u\n", dev_idx);

   for(sUI chIdx = 0; chIdx < 16u; chIdx++)
      clearMIDICacheForCh(chIdx);
}

void MIDIPipeDevice::clearMIDICacheForCh(sUI _ch) {
   if(_ch > 15u)
      _ch = 15u;

   // (note) do not reset note data to prevent hanging notes

   midipipe_device_ch_t *ch = &channels[_ch];

   for(sUI i = 0u; i < 2u; i++)
   {
      ch->prg         = -1;
      ch->ch_pressure = -1;
      ch->pitchbend   = -1;

      memset(ch->cc_states, 0xFF, sizeof(midipipe_ctl_state_t) * 128);
      memset(ch->rpn_states, 0xFF, sizeof(midipipe_ctl_state_t) * 16384);
      memset(ch->nrpn_states, 0xFF, sizeof(midipipe_ctl_state_t) * 16384);

      ch = &shadow_channels[_ch];
   }
}

void MIDIPipeDevice::setEnableMIDICache(sBool _bEnable) {
   b_midicache = _bEnable;
}

sBool MIDIPipeDevice::getEnableMIDICache(void) {
   return b_midicache;
}

void MIDIPipeDevice::setEnableNoteOffMsg(sBool _bEnable) {
   b_note_off = _bEnable;
}

sBool MIDIPipeDevice::getEnableNoteOffMsg(void) {
   return b_note_off;
}

void MIDIPipeDevice::setEnableGlobal(sBool _bEnable) {
   b_global = _bEnable;
}

sBool MIDIPipeDevice::getEnableGlobal(void) {
   return b_global;
}

void MIDIPipeDevice::setEnableVirtual(sBool _bEnable) {
   b_virtual = _bEnable;
}

sBool MIDIPipeDevice::getEnableVirtual(void) {
   return b_virtual;
}

sSI MIDIPipeDevice::getPrg(sUI _ch) {
   if(_ch < 16u)
      return shadow_channels[_ch].prg;
   else
      return -1;
}

void MIDIPipeDevice::setPrg(sUI _ch, sUI _val) {
   if(_ch < 16u)
      shadow_channels[_ch].prg = sSI(_val);
}

sSI MIDIPipeDevice::getChPressure(sUI _ch) {
   if(_ch < 16u)
      return shadow_channels[_ch].ch_pressure;
   else
      return -1;
}

void MIDIPipeDevice::setChPressure(sUI _ch, sUI _val) {
   if(_ch < 16u)
   {
      shadow_channels[_ch].ch_pressure = sSI(_val);
   }
}

sSI MIDIPipeDevice::getPitchbend(sUI _ch) {
   if(_ch < 16u)
   {
      return shadow_channels[_ch].pitchbend;
   }
   return -1;
}

void MIDIPipeDevice::setPitchbend(sUI _ch, sUI _val) {
   if(_ch < 16u)
   {
      shadow_channels[_ch].pitchbend = sSI(_val);
   }
}

sSI MIDIPipeDevice::getCCState(sUI _ch, sUI _cc) {
   if( (_ch < 16u) && (_cc < 128u) )
   {
      return shadow_channels[_ch].cc_states[_cc].value;
   }
   return -1;
}

void MIDIPipeDevice::setCCState(sUI _ch, sUI _cc, sUI _val) {
   if( (_ch < 16u) && (_cc < 128u) )
   {
      shadow_channels[_ch].cc_states[_cc].value = sSI(_val);
   }
}

sSI MIDIPipeDevice::getRPNState(sUI _ch, sUI _rpn) {
   if( (_ch < 16u) && (_rpn < 16384u) )
   {
      return shadow_channels[_ch].rpn_states[_rpn].value;
   }
   return -1;
}

void MIDIPipeDevice::setRPNState(sUI _ch, sUI _rpn, sUI _val) {
   if( (_ch < 16u) && (_rpn < 16384u) )
   {
      shadow_channels[_ch].rpn_states[_rpn].value = sSI(_val);
   }
}

sSI MIDIPipeDevice::getNRPNState(sUI _ch, sUI _nrpn) {
   if( (_ch < 16u) && (_nrpn < 16384u) )
   {
      return shadow_channels[_ch].nrpn_states[_nrpn].value;
   }
   return -1;
}

void MIDIPipeDevice::setNRPNState(sUI _ch, sUI _nrpn, sUI _val) {
   if( (_ch < 16u) && (_nrpn < 16384u) )
   {
      shadow_channels[_ch].nrpn_states[_nrpn].value = _val;
   }
}

sSI MIDIPipeDevice::getPolyPressure(sUI _ch, sUI _note) {
   if( (_ch < 16u) && (_note < 128u) )
   {
      return shadow_channels[_ch].note_states[_note].poly_pressure;
   }
   return -1;
}

void MIDIPipeDevice::setPolyPressure(sUI _ch, sUI _note, sUI _val) {
   if( (_ch < 16u) && (_note < 128u) )
   {
      shadow_channels[_ch].note_states[_note].poly_pressure = _val;
   }
}

void MIDIPipeDevice::setParamMode(sUI _ch, sSI _mode) {
   param_mode[_ch & 15u] = _mode;
}

sSI MIDIPipeDevice::getParamMode(sUI _ch) {
   return sSI(param_mode[_ch & 15u]);
}

void MIDIPipeDevice::setEnableCCPerfLSB(sUI _ch, sBool _bEnable) {
   b_cc_perf_lsb[_ch & 15u] = _bEnable;
}

void MIDIPipeDevice::setEnableChannelPressureAndNoteOff(sBool _bEnable) {
   b_channel_pressure_and_note_off = _bEnable ? YAC_TRUE : YAC_FALSE;
}

sBool MIDIPipeDevice::getEnableChannelPressureAndNoteOff(void) {
   return b_channel_pressure_and_note_off;
}

void MIDIPipeDevice::setEnableRunningStatus(sUI _ch, sBool _bEnable) {
   running_status[_ch & 15u] = _bEnable;
}

sBool MIDIPipeDevice::getEnableRunningStatus(sUI _ch) {
   return running_status[_ch & 15u];
}
