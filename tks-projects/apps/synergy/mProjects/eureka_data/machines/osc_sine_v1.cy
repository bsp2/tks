#
# sine oscillator
#
#  - fine + coarse pitch
#  - fine + coarse vsync
#  - windowed sync amount
#  - configurable keyboard tracking
#
#  (note) this osc can also generate pulse and saw-ish waveforms
#

id     osc_sine_v1
name   "osc sine v1"
author bsp
cat    osc

oversample 2

# (note) when enabled, call pow per sample frame
# (note) set to 0 to save some CPU time
arg b_vsync_exp 1 0 1
arg a_win       0 0 7

param p_amp            0.5  0 1  group=amp
param p_kbd_amt        0.75 0 1  group=kbd
param p_fine           0.5  0 1  group=pitch
param p_coarse         0.5  0 1  group=pitch
param p_vsync_fine     0.5  0 1  group=vsync
param p_vsync_coarse   0    0 1  group=vsync
param p_vsync_amt      0    0 1  group=vsync
param p_vsync_exp      0    0 1  group=vsync

mod m_amp
mod m_fine
mod m_coarse
mod m_vsync_fine
mod m_vsync_coarse
mod m_vsync_amt
mod m_vsync_exp

var v_amp
var v_freq

var v_t
var v_xfade

<prepare:
  2.0
  pow
    exp:
      $m_amp
      fma 8 -4
  sto v_amp

  $m_fine
  fma 2 -1
  bts 2
  * 2.0
    pow
      exp:
        $m_coarse
        qua 64
        fma 8 -4   
  kbd mode=1 ctr=60
    kbd:
      $p_kbd_amt
      fma 4 -2
  sto v_freq


<out:
  tri fixed=1 freq=$v_freq
  fma 0.5 0.5
  if b_vsync_exp > 0.5
    then:
      pow
        exp:
          $m_vsync_exp
          * 15.0
          + 1.0
  * $m_vsync_amt
  sto v_xfade

  sin fixed=1 freq=$v_freq
  sto v_t

  sin fixed=1 freq=$v_freq
    vsync:
      $m_vsync_coarse
      qua 64
      * 16
      + $m_vsync_fine
        fma 4 -2

  ipl a=$v_t t=$v_xfade win=$a_win

  * $v_amp
  clp
