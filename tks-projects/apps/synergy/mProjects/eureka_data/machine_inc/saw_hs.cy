def saw_hs rep=256 f=1
  vst v_s 1
  0
  rep %rep%
    + sin
        f:
          $v_s
          * %f%
      * $v_s
        rcp
    inc v_s 1
