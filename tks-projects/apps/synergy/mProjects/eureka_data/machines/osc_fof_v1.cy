id     osc_fof_v1
name   osc_fof_v1
cat    osc
author bsp

oversample 4

param p_fund_level 1   -1 1
param p_fund_color 0.5 -8 8

param p_level  0.5 -1 1
param p_color  0.5 -8 8
param p_ratio  0.0  0 1
param p_dampen 0.5  0 1
param p_warp   0.5  0 1

mod m_fund_level
mod m_fund_color
mod m_level
mod m_color
mod m_ratio
mod m_dampen
mod m_warp

arg p_amp 0.7 0 8

var v_fund_phase
var v_freq
var v_dampen
var v_warp

curve 0 dampen
curve 1 warp

<prepare:
  $p_ratio
  + $m_ratio
  bts 2 64
  sto v_freq

  $p_dampen
  * 2
  - 1
  + $m_dampen
  bts 16 16
  sto v_dampen

  $p_warp
  * 2
  - 1
  + $m_warp
  bts 8 8
  sto v_warp

<out:
  pha
  sto v_fund_phase

  sin freq=0 phase=$v_fund_phase
  lle
    c:
      $p_fund_color
      + $m_fund_color
  * $p_fund_level
    + $m_fund_level
    
  + sin freq=0
      phase:
        $v_fund_phase
        * $v_fund_phase
          * $v_freq          
          pow
           exp:
             $v_fund_phase
             lut warp lin=1 clamp=0 scl=31/32
             * $v_warp
    lle
      c:
        $p_color
        + $m_color
    * $p_level
      + $m_level
      * $v_fund_phase
        lut dampen lin=1 clamp=0 scl=31/32
        pow $v_dampen

  * $p_amp
#  clp
  tanh
