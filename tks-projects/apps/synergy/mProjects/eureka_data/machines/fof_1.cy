
oversample 4

arg p_fund_level 1 -1 1
arg p_fund_color 0 -8 8

arg p_a_level 0 -1 1
arg p_a_color 0 -8 8
arg p_a_ratio 0 0 1
arg p_a_dampen 0.5 0 1
arg p_a_warp   0.5 0 1

arg p_b_level 0 -1 1
arg p_b_color 0 -8 8
arg p_b_ratio 0 0 1
arg p_b_dampen 0.5 0 1
arg p_b_warp   0.5 0 1

arg p_amp 0.7 0 8

var v_fund_phase
var v_a_freq
var v_a_dampen
var v_a_warp
var v_b_freq
var v_b_dampen
var v_b_warp

curve 0 a_dampen
curve 1 a_warp

curve 2 b_dampen
curve 3 b_warp

<init:
  $p_a_ratio
  bts 2 64
  sto v_a_freq

  $p_a_dampen
  * 2
  - 1
  bts 16 16
  sto v_a_dampen

  $p_a_warp
  * 2
  - 1
  bts 8 8
  sto v_a_warp

  $p_b_ratio
  bts 2 64
  sto v_b_freq

  $p_b_dampen
  * 2
  - 1
  bts 16 16
  sto v_b_dampen

  $p_b_warp
  * 2
  - 1
  bts 8 8
  sto v_b_warp


<out:
  pha
  sto v_fund_phase

  sin freq=0 phase=$v_fund_phase
  lle $p_fund_color
  * $p_fund_level
    
  + sin freq=0
      phase:
        $v_fund_phase
        * $v_fund_phase
          lut a_warp lin=1
          * $v_a_warp
          * $v_a_freq
    lle $p_a_color
    * $p_a_level
      * $v_fund_phase
        lut a_dampen lin=1
        pow $v_a_dampen

  + sin freq=0
      phase:
        $v_fund_phase
        * $v_fund_phase
          lut b_warp lin=1
          * $v_b_warp
          * $v_b_freq
    lle $p_b_color
    * $p_b_level
      * $v_fund_phase
        lut b_dampen lin=1
        pow $v_b_dampen

  * $p_amp
  clp
