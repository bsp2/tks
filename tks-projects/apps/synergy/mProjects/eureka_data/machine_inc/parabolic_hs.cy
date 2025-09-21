# https://till.com/articles/wavepalette/

def parabolic_hs rep=128 f=1 ph=0.25
  vst v_s 2
  sin f=%f% ph=%ph%
  rep %rep%
    + sin ph=%ph%
        f:
          $v_s
          * %f%
      * $v_s
        pow
        rcp
    inc v_s 1
