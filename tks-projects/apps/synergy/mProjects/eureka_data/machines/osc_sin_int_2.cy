id     osc_sin_int_2
name   "osc sin integer 2"
author bsp
cat    osc

oversample 8

param p_vsync 0.5
param p_pw 0.5
param p_detune 0.5 -1 1
param p_cutoff 1 0 1
param p_res 1 0 1
param p_kbdamt 0.5 -1 1

mod m_vsync
mod m_pw
mod m_cutoff
mod m_res

var v_freq
var v_vsync_1
var v_vsync_2
var v_cutoff

<prepare:
  $p_vsync
  fma 2 -1
  bts 2 48
  sto v_vsync_1

  $m_vsync
  fma 2 -1
  bts 2 48
  sto v_vsync_2

  $m_cutoff
  kbd $p_kbdamt off=0
  sto v_cutoff

  1.0
  + $p_detune
  sto v_freq

<out:
  int
    sin vsync=$v_vsync_1
    m sin freq=$v_freq vsync=$v_vsync_2
      svf mode=0 freq=$v_cutoff res=$m_res
    #tri vsync=$v_vsync
    #saw vsync=$v_vsync
    #pul vsync=$v_vsync width=$m_pw


