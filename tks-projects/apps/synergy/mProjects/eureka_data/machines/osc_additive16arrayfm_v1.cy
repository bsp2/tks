# band n+1 phase-modulates band n

id     osc_additive16arrayfm_v1
name   osc_additive16arrayfm_v1
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
param p_fm         0      0 1
param p_fm_g       0      0 1

mod m_mag
mod m_center
mod m_dampen
mod m_freq_scl
mod m_color
mod m_fm
mod m_fm_g

curve 0 mag param=p_mag size=16 var=8
curve 1 fm  param=p_fm  size=16 var=8

var v_center
var v_dampen
var v_freq_add
var v_color

var v_level
var v_freq
var v_index
var v_out
var v_amp
var v_outidx
var v_outidx_next

lut partout voice 16


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

<lut_partout:
  0

<out:
  set v_freq  1
  set v_level 1
  set v_index $v_center
  set v_out   0
  set v_outidx 0
  set v_outidx_next 1/16

  rep 16
    $v_out
    + sin
        freq:
          $v_freq
        phase:
          $v_outidx_next
          lut partout clamp=0 lin=0
          * $v_outidx
            lut fm clamp=0 lin=0
          * $m_fm_g
            * 64

      lle $v_color
      * 1/16
      * $v_level
      * $v_index
        lut mag clamp=1 lin=1
    sto v_out
    $v_outidx
    sta partout 1 0 v_out 1 0

    inc v_freq $v_freq_add
    $v_level
    * $v_dampen
    sto v_level
    
    inc v_index 1/16

    $v_outidx_next
    sto v_outidx
    inc v_outidx_next 1/16

  $v_out
  * $v_amp
  clp
