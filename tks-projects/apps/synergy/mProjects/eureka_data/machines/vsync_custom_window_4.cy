id vsync_custom_window_4
oversample 16

modparam vsync      0.5
modparam exp        0
modparam cmp_mix    0
modparam startphase 0

var v_vsync smooth=1
var v_pha

<prepare:
  $m_vsync
  fma 2 -1
  bts 4 8
  sto v_vsync

<out:
  saw vsync=$v_vsync reset=0 phase=$m_startphase
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
      saw vsync=$v_vsync window=1 reset=0 phase=$m_startphase
      