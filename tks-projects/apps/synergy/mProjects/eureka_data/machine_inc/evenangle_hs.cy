# https://till.com/articles/wavepalette/

def evenangle_hs rep=128 f=1
  vst v_s 2
  sin f=%f%
  rep %rep%
    + sin
        f:
          $v_s
          * %f%
      * $v_s
        pow
        rcp
    inc v_s 2
