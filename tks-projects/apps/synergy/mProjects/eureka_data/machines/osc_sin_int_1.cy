id     osc_sin_int_1
name   "osc sin integer"
author bsp
cat    osc

#oversample 16

param p_vsync 0.5
param p_pw 0.5
param p_cutoff 1 0 1
param p_res 1 0 1
param p_kbdamt 0.5 -1 1

mod m_vsync
mod m_pw
mod m_cutoff
mod m_res

var v_vsync
var v_cutoff

<prepare:
  $m_vsync
  fma 2 -1
  bts 2 48
  sto v_vsync

  $m_cutoff
  kbd $p_kbdamt off=0
  sto v_cutoff

<out:
  int
    tri vsync=$v_vsync
    #tri vsync=$v_vsync
    #saw vsync=$v_vsync
    #pul vsync=$v_vsync width=$m_pw
    svf mode=0 freq=$v_cutoff res=$m_res


