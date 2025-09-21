id     osc_additive16_v1
name   osc_additive16_v1
cat    osc
author bsp

#oversample 4

#arg p_amp 0.7

param p_center     0      0 1
param p_width      0.125  0 8
param p_curve_amt  1      0 1
param p_slope_b    0      0 1
param p_slope_t    0.5    0 1
param p_freq_scl   0.5    0 1
param p_color      0.5    0 1
param p_amp        0.5    0 1

mod m_center
mod m_width
mod m_curve_amt
mod m_slope_b
mod m_slope_t
mod m_freq_scl
mod m_color

curve 0 slope_a
curve 1 slope_b
curve 2 phases

var v_freq
var v_index
var v_index_slope
var v_slope
var v_out
var v_slope_t
var v_freq_add
var v_color
var v_amp


#oversample 4

#oct -2

<prepare:
  $m_slope_t
  fma 2 -1
  bts 4 4
  * 1/16
  sto v_slope_t

  $m_freq_scl
  fma 2 -1
  bts 8 8
  sto v_freq_add

  $m_color
  fma 8 -4
  sto v_color

  $p_amp
  fma 2 -1
  bts 4 4
  sto v_amp


<out:
  set v_freq 1

  0.5
  - $m_center
  
  sto v_index
  sto v_index_slope

  set v_out 0

  rep 16
    $v_index_slope
    lut slope_a clamp=1 lin=1
    sto v_slope

    $v_index_slope
    lut slope_b clamp=1 lin=1
    * $m_slope_b
    + $v_slope
      * 1.0
        - $m_slope_b
    pow
      exp:
        $m_width
    * $m_curve_amt
    + 1.0
      - $m_curve_amt
    sto v_slope

    $v_out
    + sin freq=$v_freq
      lle $v_color
      * 1/16
      * $v_slope
    sto v_out

    inc v_freq $v_freq_add
    inc v_index 1/16
    inc v_index_slope $v_slope_t

  $v_out
  * $v_amp
  clp
