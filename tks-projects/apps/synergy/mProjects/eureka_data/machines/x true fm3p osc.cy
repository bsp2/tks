id     x_osc_true_fm3p
name   "x true fm3p osc"
author bsp
cat    osc

oversample_factor 16

param p_phase_2
param p_phase_rand
param p_ratio_2 0.5
param p_voicebus 0.1
param p_level_2 0
param p_level_3 0

mod m_phase_2
mod m_ratio_2
mod m_voicebus
mod m_level_2
mod m_level_3

var v_phrand_1
var v_phrand_2

var v_freq_2


<init:
  fsr seed=$4267 cycle=0 reset=0
  * $p_phase_rand
  sto v_phrand_1

  fsr seed=$abcd cycle=0 reset=0
  * $p_phase_rand
  sto v_phrand_2

<prepare:
  $m_ratio_2
  fma 2 -1
  bts 4096 4
  pow 3
  qua 100
  sto v_freq_2

<out:
  sin
    freq:
      sin
        phase:
          $v_phrand_2
          + $m_phase_2
        freq:
          $v_freq_2
        phase:
      * $m_level_2
        * 2
        pow 2
      bts 32 32
    phase:
      + $v_phrand_1
      + bus nrf=$m_voicebus
        * $m_level_3
          * 2
          pow 2

#  bus nr=$m_voicebus
