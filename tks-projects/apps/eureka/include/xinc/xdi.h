// JE diode

/*
 * @cycle_mod     xdi
 * @cycle_voice   je_diode_state_t %cyid%;
 *   forward bias voltage (0..1)
 * @cycle_config  vb 0.1
 *   linear voltage threshold (0..1)
 * @cycle_config  vlvb 0.14
 *   linear voltage slope
 * @cycle_config  slope 0.8
 * @cycle_noteon  float modVb%cyid% = %cycf_vb%;
 * @cycle_noteon  modVb%cyid% = Dstplugin_clamp(modVb%cyid%, 0.0f, 1.0f);
 * @cycle_noteon  modVb%cyid% = Dstplugin_scale(modVb%cyid%, std::numeric_limits<float>::epsilon(), 1.0f);
 * @cycle_noteon  float modVlVb%cyid% = %cycf_vlvb%;
 * @cycle_noteon  modVlVb%cyid% = Dstplugin_clamp(modVlVb%cyid%, 0.0f, 1.0f);
 * @cycle_noteon  modVlVb%cyid% = Dstplugin_scale(modVlVb%cyid%, std::numeric_limits<float>::epsilon(), g_controlPeakVoltage);
 * @cycle_noteon  const float modSlope%cyid% = %cycf_slope%;
 * @cycle_noteon  voice->%cyid%.updateDiodeCharacteristics(modVb%cyid%, modVlVb%cyid%, modSlope%cyid%);
 * @cycle_emit    %cyio% = voice->%cyid%.process(%cyio%);
 *
 */

#include "je_ringmod/diode.h"

struct je_diode_state_t {

   Diode m_diode;

   inline void updateDiodeCharacteristics(const float _vb, const float _vlMinusVb, const float _hSlope) {
      m_diode.setVb(_vb);
      m_diode.setVlMinusVb(_vlMinusVb);
      m_diode.setH(_hSlope);
   }

   inline float process(const float _inSmp) {
      return m_diode(_inSmp);
   }
};
