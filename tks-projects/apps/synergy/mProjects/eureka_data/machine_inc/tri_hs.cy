def tri_hs rep=64 f=1
  vst v_s 1
  0
  rep %rep%
    + sin
        f:
          $v_s
          * %f%
      * $v_s
        pow
        rcp
    inc v_s 2
