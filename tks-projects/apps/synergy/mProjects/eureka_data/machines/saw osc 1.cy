id     osc_saw_1
name   "saw osc 1"
author bsp
cat    osc

oversample_factor 8

param p_phase_rand  0
param p_color 0.5 -4 4
param p_cutoff  0.5
param p_res     0.3
param p_detune  0.5 -1 1

mod m_color
mod m_cutoff
mod m_res
mod m_detune

var v_phrand_1
var v_cutoff
var v_res

<init:
  fsr seed=$4489 cycle=0 reset=0
  * $p_phase_rand
  sto v_phrand_1

<prepare:
  $m_cutoff
  clp 1 0
  pow 2
  p2s 7
  sto v_cutoff

  $m_res
  sto v_res

<out:
  saw phase=$v_phrand_1
    freq:
      1.0
      + $m_detune
  lle c=$m_color
  svf freq=$v_cutoff res=$v_res
  svf freq=$v_cutoff res=$v_res
