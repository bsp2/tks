id     osc_fm3s_tanh
name   "fm3s osc tanh"
author bsp
cat    osc

oversample_factor 16

param p_color_1 0.5
param p_color_2 0.5
param p_color_3 0.5
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

var v_freq_2
var v_freq_3

var v_xfd_1
var v_xfd_2
var v_xfd_3

var v_drive_1
var v_drive_2
var v_drive_3

var v_phrand_1
var v_phrand_2
var v_phrand_3

var v_sin

lut tanh shared 4096

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

<lut_tanh:
  # 0..1 => -100..+100
  fma 200 -100
  tanh

<prepare:
  $m_ratio_2
  fma 2 -1
  bts 4096 4
  pow 3
  qua 100
  sto v_freq_2

  $m_ratio_3
  fma 2 -1
  bts 4096 4
  pow 3
  qua 100
  sto v_freq_3

  1.0
  - $m_color_1
    clp 0.25 0
    * 4
  sto v_xfd_1

  1.0
  - $m_color_2
    clp 0.25 0
    * 4
  sto v_xfd_2

  1.0
  - $m_color_3
    clp 0.25 0
    * 4
  sto v_xfd_3

  10
  pow $m_color_1
  sto v_drive_1

  10
  pow $m_color_2
  sto v_drive_2

  10
  pow $m_color_3
  sto v_drive_3

<out:
  sin
    phase:
      sin
        freq:
          $v_freq_2
        phase:
          sin
            phase:
              $v_phrand_3
              + $m_phase_23
            freq:
              $v_freq_3
          sto v_sin
          * $v_drive_3
          fma 1/200 0.5
          lut tanh lin=1
          xfd b=$v_sin amount=$v_xfd_3
          * $m_level_3
            * 2
            pow 2
          + $v_phrand_2
          + $m_phase_23
      sto v_sin
      * $v_drive_2
      fma 1/200 0.5
      lut tanh lin=1
      xfd b=$v_sin amount=$v_xfd_2
      * $m_level_2
        * 2
        pow 2
      + $v_phrand_1
  sto v_sin
  * $v_drive_1
  fma 1/200 0.5
  lut tanh lin=1
  xfd b=$v_sin amount=$v_xfd_1

