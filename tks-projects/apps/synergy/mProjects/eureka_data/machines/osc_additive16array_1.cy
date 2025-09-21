id     osc_additive16array_v1
name   osc_additive16array_v1
cat    osc
author bsp

#oversample 4

#arg p_amp 0.7

param p_mag        0      0 1
param p_center     0.5    0 1
param p_dampen     0      0 1
param p_freq_scl   0.5    0 1
param p_color      0.5    0 1
param p_amp        0.5    0 1

mod m_mag
mod m_center
mod m_dampen
mod m_freq_scl
mod m_color

curve 0 mag param=p_mag size=16 var=8

var v_center
var v_dampen
var v_freq_add
var v_color

var v_level
var v_freq
var v_index
var v_out
var v_amp


<prepare:
  $m_center
  fma 2 -1
  sto v_center

  $m_dampen
  sto v_dampen

  $m_freq_scl
  fma 2 -1
  bts 4 4
  sto v_freq_add

  $m_color
  fma 8 -4
  sto v_color

  $p_amp
  fma 2 -1
  bts 4 4
  sto v_amp


<out:
  set v_freq  1
  set v_level 1
  set v_index $v_center
  set v_out   0

  rep 16
    $v_out
    + sin freq=$v_freq
      lle $v_color
      * 1/16
      * $v_level
      * $v_index
        lut mag clamp=1 lin=1
    sto v_out

    inc v_freq $v_freq_add
    $v_level
    * $v_dampen
    sto v_level

    inc v_index 1/16

  $v_out
  * $v_amp
  clp
