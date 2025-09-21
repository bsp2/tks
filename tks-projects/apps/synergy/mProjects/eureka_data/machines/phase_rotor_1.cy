id phase_rotor_1
oversample 4

param p_scl 1
param p_add 0
param p_vsync 0
param p_slew_up 1
param p_slew_down 1

mod m_scl
mod m_add
mod m_vsync
mod m_slew_up
mod m_slew_down

lut wav voice 4

var v_tmp
var v_out

<lut_wav:
  0

<out:
  sin
  sto v_tmp
  0/4
  sta wav 1 0 v_tmp 1 0

  tri
  sto v_tmp
  1/4
  sta wav 1 0 v_tmp 1 0

  pul
  sto v_tmp
  2/4
  sta wav 1 0 v_tmp 1 0

  saw
  sto v_tmp
  3/4
  sta wav 1 0 v_tmp 1 0

  sin
    vsync:
      $m_vsync
      fma 15 1
  fma $m_scl $m_add
  lut wav clamp=0 lin=1


 slw a=$v_out up=$m_slew_up down=$m_slew_down
 sto v_out

 # hbx 0.73
 # hld 0.2
