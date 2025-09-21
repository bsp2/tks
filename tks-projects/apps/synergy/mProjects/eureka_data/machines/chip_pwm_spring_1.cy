id chip_pwm_spring_1
oversample 4

param p_prescl 0.5
param p_pw  0.5
param p_dcy 0.4
param p_spd 0.05

mod m_prescl
mod m_pw
mod m_spd
mod m_dcy

var v_spd
var v_dcy

<prepare:
  $m_spd
  fma 1.2 0
  clp 1.2 0.001
  sto v_spd

  $m_dcy
  fma 0.9 0.09999
  sto v_dcy

<out:
  pul width=$m_pw
  * $m_prescl
  spr spd=$v_spd dcy=$v_dcy
  clp
