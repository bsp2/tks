def pdwarp
  global:
    arg a_%%_exp 0 -16 16

  calc:
    fma 2 -1
    clt 0
        + 1
        pow $a_%%_exp
        - 1
      else:
        r- 1
        pow $a_%%_exp
        r- 1
    fma 0.5 0.5
