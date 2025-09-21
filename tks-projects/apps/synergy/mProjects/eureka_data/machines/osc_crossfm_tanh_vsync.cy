id     osc_crossfm_tanh_vsync
name   "crossfm osc tanh vsync"
author bsp
cat    osc

oversample_factor 16

arg window 1 0 3

param p_color_1 0.5
param p_color_2 0.5
param p_vsync 0.5
param p_ratio_2 0.5
param p_fm_2to1 0
param p_fm_1to2 0
param p_level_1 1
param p_level_2 0

mod m_color_1
mod m_color_2
mod m_vsync_2_off
mod m_ratio_2
mod m_fm_2to1
mod m_fm_1to2
mod m_level_1
mod m_level_2

var v_freq_2

var v_xfd_1
var v_xfd_2

var v_drive_1
var v_drive_2

var v_sin
var v_osc_1
var v_osc_2

var v_vsync_1
var v_vsync_2

lut tanh shared 4096

<lut_tanh:
  # 0..1 => -100..+100
  fma 200 -100
  tanh

<prepare:
  $m_ratio_2
  fma 2 -1
  bts 4096 4
  pow 3
  qua 100
  sto v_freq_2

  1.0
  - $m_color_1
    clp 0.25 0
    * 4
  sto v_xfd_1

  1.0
  - $m_color_2
    clp 0.25 0
    * 4
  sto v_xfd_2

  10
  pow $m_color_1
  sto v_drive_1

  10
  pow $m_color_2
  sto v_drive_2

  $p_vsync
  fma 2 -1
  bts 16
  qua 16
  sto v_vsync_1

  $p_vsync
  + $m_vsync_2_off
  fma 2 -1
  bts 16
  qua 16
  sto v_vsync_2

<out:
  sin window=$window
    vsync:
      $v_vsync_1
    phase:
      $v_osc_2
      * $m_fm_2to1
      * 32
  sto v_sin
  * $v_drive_1
  fma 1/200 0.5
  lut tanh lin=1
  xfd b=$v_sin amount=$v_xfd_1
  sto v_osc_1


  sin window=$window
    vsync:
      $v_vsync_2
    freq:
      $v_freq_2
    phase:
      $v_osc_2
      * $m_fm_1to2
      * 32
  sto v_sin
  * $v_drive_2
  fma 1/200 0.5
  lut tanh lin=1
  xfd b=$v_sin amount=$v_xfd_2
  sto v_osc_2

  * $m_level_2
  + $v_osc_1
    * $m_level_1
