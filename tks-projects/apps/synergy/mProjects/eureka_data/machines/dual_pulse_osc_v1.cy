id     dual_pulse_osc_v1
name   "dual pulse osc v1"
author bsp
cat    osc

oversample_factor 2

param p_cutoff 0.9
param p_res 0.3
param p_pw 0.5
param p_pw_mod 0.1
param p_detune 0.5
param p_dephase 0
param p_vsync_1 0
param p_vsync_2 0

mod m_cutoff
mod m_res
mod m_pw_mod
mod m_pw
mod m_detune
mod m_dephase
mod m_vsync_1
mod m_vsync_2

arg flt_gain min=0 max=15
arg hpf 0.13 0 1

var v_osc1_tmp 
var v_osc2_last
var v_cutoff
var v_res

<out:
#  1
#  - 1
#    - $m_cutoff
#    pow 2
#  sto v_cutoff

  $m_cutoff
  - 1
  clp 0.5
  + 0.5
  pow 2
  sto v_cutoff

  1
  - $m_res
  sto v_res

  pul 1.0  cycle=0 reset=0
    width:
      $m_pw
      + $v_osc2_last
        * $m_pw_mod
          * 0.25
    vsync:
      $m_vsync_1
      + 0.1
      * 12.0
  sto v_osc1_tmp
  pul 
    freq:
      1.0
      + $m_detune
        * 2
        - 1
    width:
      $m_pw
      + $m_dephase
    vsync:
      $m_vsync_2
      + 0.1
      * 12.0
  sto v_osc2_last
  + $v_osc1_tmp
  svf sr=0 f=0.5 res=$v_res mode=0
    freq:
      $v_cutoff
      clp
  svf sr=0 f=0.5 res=$v_res mode=0
    freq:
      $v_cutoff
  hbx $hpf
  * $flt_gain


