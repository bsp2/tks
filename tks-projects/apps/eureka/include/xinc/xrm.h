// JE ring mod

#include "je_ringmod/constants.h"
#include "je_ringmod/meta.h"
#include "je_ringmod/diode.h"

/*
 * @cycle_mod     xrm
 * @cycle_voice   je_ringmod_state_t %cyid%;
 * @cycle_input   a
 * @cycle_input   b
 * @cycle_input   alvl 1.0f
 * @cycle_input   blvl 1.0f
 * @cycle_input   off 0.0f
 *   forward bias voltage (0..1)
 * @cycle_input   vb 0.1
 *   linear voltage threshold (0..1)
 * @cycle_input   vlvb 0.14
 *   linear voltage slope
 * @cycle_input   slope 0.8
 *   apol/bpol: 0,1,2
 * @cycle_config  apol 0
 * @cycle_config  bpol 0
 *    mode 0=ring 1=sum 2=diff 3=min 4=max
 * @cycle_config  mode 0
 * @cycle_emit    float modVb%cyid% = %cyin_vb%;
 * @cycle_emit    modVb%cyid% = Dstplugin_clamp(modVb%cyid%, 0.0f, 1.0f);
 * @cycle_emit    modVb%cyid% = Dstplugin_scale(modVb%cyid%, std::numeric_limits<float>::epsilon(), 1.0f);//g_controlPeakVoltage);
 * @cycle_emit    float modVlVb%cyid% = %cyin_vlvb%;
 * @cycle_emit    modVlVb%cyid% = Dstplugin_clamp(modVlVb%cyid%, 0.0f, 1.0f);
 * @cycle_emit    modVlVb%cyid% = Dstplugin_scale(modVlVb%cyid%, std::numeric_limits<float>::epsilon(), g_controlPeakVoltage);
 * @cycle_emit    float modSlope%cyid% = %cyin_slope%;
 * @cycle_emit    voice->%cyid%.updateDiodeCharacteristics(modVb%cyid%, modVlVb%cyid%, modSlope%cyid%);
 * @cycle_emit    %cyio% = voice->%cyid%.process(%cyin_a%, %cyin_alvl%, %cycf_apol%, %cyin_b%, %cyin_blvl%, %cycf_bpol%, %cyin_off%, %cycf_mode%);
 *
 */

struct je_ringmod_state_t {

	Diode m_diode;

   void reset(void) {
   }

	inline void updateDiodeCharacteristics(const float _vb, const float _vlMinusVb, const float _hSlope)
	{
		m_diode.setVb(_vb);
		m_diode.setVlMinusVb(_vlMinusVb);
		m_diode.setH(_hSlope);
	}

	inline float getLeveledPolarizedInputValue(const float _smp, const float _lvl, const float _polarity)
	{
		const float inputPolarity = _polarity;
		const float inputValue = _smp * _lvl;

		if (inputPolarity < 0.5f)
			return (inputValue < 0.0f) ? -m_diode(inputValue) : 0.f;
		else if (inputPolarity > 1.5f)
			return (inputValue > 0.0f) ? m_diode(inputValue) : 0.f;
		return inputValue;
	}

   inline float process(const float _in1Smp,
                        const float _in1Lvl,
                        const float _in1Polarity,
                        const float _in2Smp,
                        const float _in2Lvl,
                        const float _in2Polarity,
                        const float _offset,
                        const unsigned char _outMode
                       ) {

		const float vhin = getLeveledPolarizedInputValue(_in1Smp, _in1Lvl, _in1Polarity) * 0.5f;
		const float vc   = getLeveledPolarizedInputValue(_in2Smp, _in2Lvl, _in2Polarity) + _offset;

		const float vc_plus_vhin = vc + vhin;
		const float vc_minus_vhin = vc - vhin;

		const float outSum  = m_diode(vc_plus_vhin);
		const float outDiff = m_diode(vc_minus_vhin);

		float out;
      switch(_outMode & 7u/*compiler hint*/)
      {
         case 0u:  // ring
         default:
            out = outSum - outDiff;
            break;

         case 1u:  // sum
            out = outSum;
            break;

         case 2u:  // diff
            out = outDiff;
            break;

         case 3u:  // min
            out = outSum < outDiff ? outSum : outDiff;
            break;

         case 4u:  // max
            out = outSum > outDiff ? outSum : outDiff;
            break;
      }
      return out;
   }
};
