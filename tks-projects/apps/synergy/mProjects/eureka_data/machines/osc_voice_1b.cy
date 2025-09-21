id     osc_voice_1b
name   "voice osc 1b"
author bsp
cat    osc

oversample_factor 16

param p_freq_2  1 0.125 8
param p_fm      0 0 8
param p_phase_rand  0
param p_cutoff  0.5
param p_res     0.5
param p_vsync_off 0 -4 4
param p_flt_off 0 -0.5 0.5
param p_color 0 0.01 4

mod m_freq_2
mod m_fm
mod m_cutoff
mod m_res
mod m_vsync_off
mod m_flt_off
mod m_color
mod m_detune

var v_phrand_1
var v_phrand_2

<init:
  fsr seed=$4489 cycle=0 reset=0
  * $p_phase_rand
  sto v_phrand_1

  fsr seed=$dcb9 cycle=0 reset=0
  * $p_phase_rand
  sto v_phrand_2

var v_osc_1
var v_cutoff_1
var v_cutoff_2
var v_res
var v_res_half
var v_freq_sin

<prepare:
  $m_cutoff
  #p2s 7 kbd=1
  kbd 1
  clp 1 0
  sto v_cutoff_1

  $m_cutoff
  + $m_flt_off
  #p2s 7 kbd=1
  kbd 1
  clp 1 0
  sto v_cutoff_2

  $m_res
  clp 1 0.04
  sto v_res

  0.5
  + $v_res
  * 0.5
  sto v_res_half

  $m_freq_2
  qua 128
  + $m_detune
  * 0.01
  sto v_freq_sin

<out:
  saw phase=$v_phrand_2
    vsync:
      $m_freq_2
      + $m_vsync_off
  lle c=$m_color
  sto v_osc_1

  sin freq=$v_freq_sin
    phase:
      $v_osc_1
      * $m_fm
      + $v_phrand_1

  svf sr=0 mode=2 freq=$v_cutoff_1 res=$v_res
  svf sr=0 mode=2 freq=$v_cutoff_1 res=$v_res_half

  pow exp=$m_color

  svf sr=0 mode=2 freq=$v_cutoff_2 res=$v_res
  svf sr=0 mode=2 freq=$v_cutoff_2 res=$v_res_half
