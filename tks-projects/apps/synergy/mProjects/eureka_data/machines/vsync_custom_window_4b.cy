id vsync_custom_window_4b
oversample 16

modparam vsync      0.5
modparam startphase 0

var v_vsync smooth=1

<prepare:
  $m_vsync
  fma 2 -1
  bts 4 8
  sto v_vsync

<out:
  saw vsync=$v_vsync window=hse1 reset=0 phase=$m_startphase
     