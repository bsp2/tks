id     osc_fm3s_ftri
name   "fm3s osc ftri"
author bsp
cat    osc

oversample_factor 16

param p_color_1 0.5 0 0.5
param p_color_2 0.5 0 0.5
param p_color_3 0.5 0 0.5
param p_phase_rand
param p_ratio_2 0.5
param p_ratio_3 0.5
param p_level_2 0
param p_level_3 0

mod m_color_1
mod m_color_2 
mod m_color_3
mod m_phase_23
mod m_ratio_2
mod m_ratio_3
mod m_level_2
mod m_level_3

var v_phrand_1
var v_phrand_2
var v_phrand_3

var v_color_1
var v_color_2
var v_color_3

<init:
  fsr seed=$4489 cycle=0 reset=0
  * $p_phase_rand
  sto v_phrand_1

  fsr seed=$dcb9 cycle=0 reset=0
  * $p_phase_rand
  sto v_phrand_2

  fsr seed=$f10a cycle=0 reset=0
  * $p_phase_rand
  sto v_phrand_3

<out:
  $m_color_1
  p2s 7 1/255
  clp 1 0
  sto v_color_1

  $m_color_2
  p2s 7 1/255
  clp 1 0
  sto v_color_2

  $m_color_3
  p2s 7 1/255
  clp 1 0
  sto v_color_3

  tri
    phase:
      tri
        freq:
          $m_ratio_2
          fma 2 -1
          bts 4096 4
          pow 3
          qua 100
        phase:
          tri
            phase:
              $v_phrand_3
              + $m_phase_23
            freq:
              $m_ratio_3
              fma 2 -1
              bts 4096 4
              pow 3
              qua 100
          svf sr=0 freq=$v_color_3
          svf sr=0 freq=$v_color_3
          * $m_level_3
            * 2
            pow 2
          + $v_phrand_2
      svf sr=0 freq=$v_color_2
      svf sr=0 freq=$v_color_2
      * $m_level_2
        * 2
        pow 2
      + $v_phrand_1
      + $m_phase_23
  svf sr=0 freq=$v_color_1
  svf sr=0 freq=$v_color_1
