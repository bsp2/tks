id     osc_fm3s_fb
name   "fm3s osc fb"
author bsp
cat    osc

oversample_factor 16

param p_fb_1 0
param p_fb_2 0
param p_fb_3 0
param p_phase_rand 0 0 1
param p_ratio_2 0.5
param p_ratio_3 0.5
param p_level_2 0
param p_level_3 0

mod m_fb_1
mod m_fb_2
mod m_fb_3
mod m_init
mod m_ratio_2
mod m_ratio_3
mod m_level_2
mod m_level_3

var v_fb_1
var v_fb_2
var v_fb_3

var v_level_2
var v_level_3

var v_freq_2
var v_freq_3

var v_osc_1
var v_osc_2
var v_osc_3

var v_phrand_1
var v_phrand_2
var v_phrand_3

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

  $v_osc_1
  * $m_init
    * 0.01
  sto v_osc_1

  $v_osc_2
  * $m_init
    * 0.01
  sto v_osc_2

  $v_osc_3
  * $m_init
    * 0.01
  sto v_osc_3

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

  $m_fb_1
  sto v_fb_1

  $m_fb_2
  * 4
  sto v_fb_2

  $m_fb_3
  * 4
  sto v_fb_3

<out:
  $m_level_2
  * 16
  sto v_level_2

  $m_level_3
  * 16
  sto v_level_3

  sin freq=$v_freq_3
    phase:
      $v_phrand_3
      + $v_osc_3
        * $v_fb_3
  * $v_level_3
  sto v_osc_3

  sin freq=$v_freq_2
    phase:
      $v_phrand_2
      + $v_osc_3
      + $v_osc_2
        * $v_fb_2
  * $v_level_2
  sto v_osc_2

  sin
    phase:
      $v_phrand_1
      $v_osc_2
      + $v_osc_1
        * $v_fb_1
  sto v_osc_1
