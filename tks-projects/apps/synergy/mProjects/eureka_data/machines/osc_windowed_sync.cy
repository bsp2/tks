id     osc_sin_windowed_sync
name   "osc sin windowed sync"
author bsp
cat    osc

oversample 16

arg a_window 0 0 3

param p_vsync 0.5
param p_pw 0.5

mod m_vsync
mod m_pw

var v_vsync

<prepare:
  $m_vsync
  fma 2 -1
  bts 2 48
  sto v_vsync

<out:
  sin vsync=$v_vsync window=$a_window
  #tri vsync=$v_vsync window=$a_window
  #saw vsync=$v_vsync window=$a_window
  #pul vsync=$v_vsync window=$a_window width=$m_pw

