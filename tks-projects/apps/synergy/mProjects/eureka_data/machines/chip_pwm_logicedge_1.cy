id chip_pwm_logicedge_1
# oversample 8

param p_pw 0.5
param p_up 0.5
param p_dn 0.5
param p_slew_min 0.5
param p_slew_max 0.5
param p_amp 0.5
param p_hpf 0.5

mod m_pw
mod m_up
mod m_dn
mod m_slew_min
mod m_slew_max
mod m_amp
mod m_hpf

var v_pul
var v_rise
var v_fall
var v_out
var v_slew_min
var v_slew_max

<prepare:
  $m_slew_min
  fma 0.9 0.001
  sto v_slew_min

  $m_slew_max
  fma 0.9 0.001
  sto v_slew_max

<out:
  pul width=$m_pw phase=0.5
  sto v_pul
  red
    set v_rise 1
  $v_pul
  fed
    set v_fall 1
  $v_rise
  *sr 1.0
      - $m_up
        * 0.01
        # pow 2
  sto v_rise
  $v_fall
  *sr 1.0
      - $m_dn
        * 0.01
        # pow 2
  sto v_fall
  
  1
  - $v_rise
    slw $v_slew_max $v_slew_min
  + $v_fall
    slw $v_slew_min $v_slew_max
  fma 2 -1
  * $m_amp
    * 2
  hbx $m_hpf
  clp
