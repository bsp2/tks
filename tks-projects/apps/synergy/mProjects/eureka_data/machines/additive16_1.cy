
oversample 4

arg p_center     0   -1 1
arg p_width      1    0 8
arg p_curve_amt  1    0 1
arg p_slope_b    0    0 1
arg p_amp        0.7  0 8

curve 0 slope_a
curve 1 slope_b
curve 2 phases

var v_freq
var v_index
var v_slope
var v_out

#oversample 4

oct -2


<out:
  set v_freq  0
  set v_index $p_center
  inc v_index 0.5
  set v_out 0

  rep 16
    $v_index
    lut slope_a clamp=1 lin=1
    sto v_slope

    $v_index
    lut slope_b clamp=1 lin=1
    * $p_slope_b
    + $v_slope
      * 1.0
        - $p_slope_b
    pow $p_width
    * $p_curve_amt
    + 1.0
      - $p_curve_amt     
    sto v_slope

    $v_out
    + sin freq=$v_freq
        phase:
          $v_index
          lut phases clamp=1 lin=1
      * 1/32
      * $v_slope
    inc v_freq 1
    inc v_index 1/16
    sto v_out

#  sin freq=1
  $v_out
  * $p_amp
  clp
