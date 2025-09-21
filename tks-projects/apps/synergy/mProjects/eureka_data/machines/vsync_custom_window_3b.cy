id vsync_custom_window_3b
oversample 4

modparam vsync    0.5
modparam pw       0.5

var v_vsync smooth=1

<prepare:
  $m_vsync
  fma 2 -1
  bts 4 8
  sto v_vsync

<out:
  pul vsync=$v_vsync width=$m_pw window=hse5
