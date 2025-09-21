# https://till.com/articles/wavepalette/

def eventooth_hs rep=64 f=1
  vst v_s 2
  sin f=%f%
  rep %rep%
    + sin
        f:
          $v_s
          * %f%
      * $v_s
        rcp
    inc v_s 2
