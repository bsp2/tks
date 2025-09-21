id vsync_custom_window_2
oversample 16

modparam vsync    0.5
modparam exp      0
modparam cmp_mix  0

var v_vsync smooth=1
var v_pha

<prepare:
  $m_vsync
  fma 2 -1
  bts 4 8
  sto v_vsync

<out:
  tri vsync=$v_vsync
    window:
      * 0.5
      sto v_pha
      1
      - 1
        - sin freq=0 phase=$v_pha
          pow
            exp:
              $m_exp
              fma 8 0
  ipl t=$m_cmp_mix
    b:
      tri vsync=$v_vsync window=1


      