# "Digital" ADSR envelope that modulates amplitude of input signal
#   - a/d/r shape controls log/lin/exp curvature
#   - envelope level resets when envelope starts ('digital' behaviour)

id     env_adsr_md0_digi_v1
name   env_adsr_md0_digi_v1
cat    amp
author bsp

#oversample 4

arg a_reset 1    0 1
arg a_scl   0.01 0 1
arg a_mode  0    0 1

param p_a             1.0     0  1  group=rate
param p_ashape        0.5     0  1  group=shape
param p_d             0.3     0  1  group=rate
param p_dshape        0.5     0  1  group=shape
param p_s             0.6     0  1  group=level
param p_r             0.1     0  1  group=rate
param p_rshape        0.5     0  1  group=shape
param p_out_shape     0.5     0  1  group=shape

mod m_gate
mod m_a
mod m_ashape
mod m_d
mod m_s
mod m_r
mod m_dr_shape
mod m_out_shape

var v_lvl_c
var v_lvl_n

<init:
  set v_lvl_c 0

<prepare:
  $m_gate
  adsr s=$m_s reset=$a_reset scl=$a_scl mode=$a_mode
    a:
      $m_a
      clp 1 0
      pow 3

    ashape:
      $m_ashape
      fma 16 -8

    d:
      $m_d
      clp 1 0
      pow 3

    dshape:
      $p_dshape
      + $m_dr_shape
      fma 16 -8

    r:
      $m_r
      clp 1 0
      pow 3

    rshape:
      $p_rshape
      + $m_dr_shape
      fma 16 -8

  lle
    c:
      $m_out_shape
      fma 4 -2

  sto v_lvl_n

<out:
  set v_lvl_c
    value:
      $v_lvl_n
      - $v_lvl_c
      * 0.1
      + $v_lvl_c
  * $v_lvl_c
