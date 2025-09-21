id     osc_dual_tri_1
name   "dual tri osc 1"
author bsp
cat    osc

oversample_factor 8

param p_phase_rand  0
param p_color_1 0.5 -4 4
param p_color_2 0.5 -4 4
param p_detune  0.5 -1 1
param p_dephase 0 0 1
param p_level_2 0.5 -1 1
param p_cutoff  0.5
param p_res     0.3

mod m_vsync
mod m_color_1
mod m_color_2
mod m_detune
mod m_dephase
mod m_level_2
mod m_cutoff
mod m_res

var v_phrand_1
var v_phrand_2
var v_cutoff
var v_res
var v_vsync
var v_freq_2

<init:
  fsr seed=$4489 cycle=0 reset=0
  * $p_phase_rand
  sto v_phrand_1

  fsr seed=$dbc9 cycle=0 reset=0
  * $p_phase_rand
  sto v_phrand_2

<prepare:
  $m_cutoff
  clp 1 0.02
  #p2s 7
  kbd 0.5
  sto v_cutoff

  $m_res
  sto v_res

  $m_vsync
  bts 16
  sto v_vsync

  $m_detune
  bts 16
  sto v_freq_2

<out:
  tri vsync=$v_vsync phase=$v_phrand_1
  lle c=$m_color_1

  + tri vsync=$v_vsync
      phase:
        $v_phrand_1
        $m_dephase
      freq:
        $v_freq_2
    lle c=$m_color_2
    * $m_level_2

  svf sr=0 freq=$v_cutoff res=$v_res
  svf sr=0 freq=$v_cutoff res=1
