arg p_amp 0.7 0 8
arg p_lin 0   0 1
arg p_pos 0   0 1

curve 0 w0
curve 1 w1
curve 2 w2
curve 3 w3

<out:
  pha
  vst ph
  cas p=$p_pos lin=$p_lin
    0:
      $ph
      lut w0 lin=1
    1:
      $ph
      lut w1 lin=1
    2:
      $ph
      lut w2 lin=1
    3:
      $ph
      lut w3 lin=1
  * $p_amp
  clp
