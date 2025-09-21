# per partial level, detune, phase, vsync (windowed)

id     osc_additive8phasew_v1
name   osc_additive8phasew_v1
cat    osc
author bsp

oversample 3

#arg p_amp 0.7

param p_mag        0      0 1
param p_center     0      0 1
param p_dampen     0      0 1
param p_amp        0.5    0 1
param p_detune     0      0 1
param p_phase      0      0 1
param p_det_amt    1      0 1
param p_vsync      0      0 1

mod m_mag
mod m_center
mod m_dampen
mod m_amp
mod m_detune
mod m_phase
mod m_det_amt
mod m_vsync

curve 0 mag    param=p_mag    size=8 var=8
curve 1 detune param=p_detune size=8 var=8 dpyMin=-16 dpyMax=16
curve 2 phase  param=p_phase  size=8 var=8
curve 3 vsync  param=p_vsync  size=8 var=8 dpyMin=1 dpyMax=16

var v_center
var v_dampen

var v_level
var v_freq
var v_det_amt
var v_index
var v_out
var v_amp
var v_vsync


<prepare:
  $m_center
  clp 1 0
  sto v_center

  $m_dampen
  sto v_dampen

  $p_amp
  fma 2 -1
  bts 4 4
  sto v_amp

  $m_det_amt
  sto v_det_amt

  $m_vsync
  sto v_vsync


<out:
  set v_freq  1
  set v_level 1
  set v_index $v_center
  set v_out   0

  rep 8
    $v_out
    + sin window=1
        freq:
          $v_index
          lut detune clamp=0 lin=1
          fma 2 -1
          bts 16 16
          - 1
          * $v_det_amt
          + 1
        phase:
          $v_index
          lut phase clamp=0 lin=1
        vsync:
          $v_index
          lut vsync clamp=0 lin=1
          fma 15 1

      * 1/16
      * $v_level
      * $v_index
        lut mag clamp=0 lin=1
    sto v_out

    inc v_freq 1.0

    $v_level
    * $v_dampen
    sto v_level
    
    inc v_index 1/8

  $v_out
  * $v_amp
  clp
