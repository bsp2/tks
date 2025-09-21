id vsync_custom_window_1b

modparam vsync 0.5

var v_vsync smooth=1

<prepare:
  $m_vsync
  fma 2 -1
  bts 4 8
  sto v_vsync

<out:
  sin vsync=$v_vsync window=hse5
    