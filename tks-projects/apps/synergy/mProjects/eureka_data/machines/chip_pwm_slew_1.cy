id chip_pwm_slew_1
oversample 8

param p_pw 0.5
param p_up 0.9
param p_dn 0.5

mod m_pw
mod m_up
mod m_dn

var v_out

<out:
  pul width=$m_pw
  slw b=$v_out up=$m_dn down=$m_up
  sto v_out
