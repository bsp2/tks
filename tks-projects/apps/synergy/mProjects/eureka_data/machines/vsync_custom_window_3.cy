id vsync_custom_window_3
oversample 4

modparam vsync    0.5
modparam exp      0
modparam cmp_mix  0
modparam pw       0.5

var v_vsync smooth=1
var v_pha

<prepare:
  $m_vsync
  fma 2 -1
  bts 4 8
  sto v_vsync

<out:
  pul vsync=$v_vsync width=$m_pw
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
      pul vsync=$v_vsync window=1 width=$m_pw


      