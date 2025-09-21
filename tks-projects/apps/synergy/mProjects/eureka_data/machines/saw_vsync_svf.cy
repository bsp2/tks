id      saw_vsync_svf
name   "saw vsync svf"
author  bsp
cat     osc

oversample 8

arg kbd_amt 0.25

param p_vsync  0.15 1 15
param p_freq   0.8
param p_res    0.7
param p_detune 0.5

mod m_vsync
mod m_freq
mod m_res
mod m_detune

var v_freq
var v_phase1
var v_phase2
var v_detune

<init:
  fsr cycle=0 reset=0
  sto v_phase1

  fsr cycle=0 reset=0 seed=$1234
  sto v_phase2

<prepare:
  $m_freq
  kbd $kbd_amt
  sto v_freq

  $m_detune
  fma 2 -1
  bts 2
  sto v_detune

<out:
  saw vsync=$m_vsync window=1 phase=$v_phase1
  + saw vsync=$m_vsync window=1 phase=$v_phase2 freq=$v_detune
  svf sr=0 freq=$v_freq res=$m_res
  svf sr=0 freq=$v_freq res=$m_res
