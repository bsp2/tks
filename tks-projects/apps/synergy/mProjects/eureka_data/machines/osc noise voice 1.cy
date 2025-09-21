id     osc_noise_voice_1
name   "noise voice osc 1"
author bsp
cat    osc

oversample_factor 2

arg kbd 0.5 0.25 2.0

param p_cutoff_1  0.5
param p_cutoff_2  0.5
param p_cutoff_3  0.5
param p_res_1     0.3
param p_res_2     0.3
param p_res_3     0.3

mod m_cutoff_1
mod m_cutoff_2
mod m_cutoff_3
mod m_level_1
mod m_level_2
mod m_level_3
mod m_w_1

var v_cutoff_1l
var v_cutoff_1h

var v_cutoff_2l
var v_cutoff_2h

var v_cutoff_3l
var v_cutoff_3h

var v_res_1
var v_res_1h
var v_res_1h2

var v_res_2
var v_res_2h
var v_res_2h2

var v_res_3
var v_res_3h
var v_res_3h2

var v_1
var v_2
var v_3

<prepare:
  $m_cutoff_1
  p2s 7 kbd=$kbd
    off:
      $m_w_1
      neg
  clp 1 0.02
  * 0.5
  sto v_cutoff_1l

  $m_cutoff_1
  p2s 7 kbd=$kbd
    off:
      $m_w_1
  clp 1 0.02
  * 0.5
  sto v_cutoff_1h

  1.0
  - $p_res_1
  sto v_res_1
  * 0.5
  + 0.5
  sto v_res_1h
  * 0.5
  + 0.5
  * 0.5
  + 0.5
  sto v_res_1h2



  $m_cutoff_2
  p2s 7 kbd=$kbd
    off:
      $m_w_1
      neg
  clp 1 0.02
  * 0.5
  sto v_cutoff_2l

  $m_cutoff_2
  p2s 7 kbd=$kbd
    off:
      $m_w_1
  clp 1 0.02
  * 0.5
  sto v_cutoff_2h

  1.0
  - $p_res_2
  sto v_res_2
  * 0.5
  + 0.5
  sto v_res_2h
  * 0.5
  + 0.5
  * 0.5
  + 0.5
  sto v_res_2h2


  $m_cutoff_3
  p2s 7 kbd=$kbd
    off:
      $m_w_1
      neg
  clp 1 0.02
  * 0.5
  sto v_cutoff_3l

  $m_cutoff_3
  p2s 7 kbd=$kbd
    off:
      $m_w_1
  clp 1 0.02
  * 0.5
  sto v_cutoff_3h

  1.0
  - $p_res_3
  sto v_res_3
  * 0.5
  + 0.5
  sto v_res_3h
  * 0.5
  + 0.5
  * 0.5
  + 0.5
  sto v_res_3h2



<out:
  fsr seed=$4489 cycle=0 reset=0
  rep 4
    svf sr=0 mode=1 freq=$v_cutoff_1h res=1
  rep 2
    svf sr=0 mode=0 freq=$v_cutoff_1l res=$v_res_1
  rep 1
    svf sr=0 mode=0 freq=$v_cutoff_1l res=$v_res_1h
  rep 4
    svf sr=0 mode=0 freq=$v_cutoff_1l res=$v_res_1h2
  sto v_1

  fsr seed=$dbc9 cycle=0 reset=0
  rep 4
    svf sr=0 mode=1 freq=$v_cutoff_2h res=1
  rep 2
    svf sr=0 mode=0 freq=$v_cutoff_2l res=$v_res_2
  rep 1
    svf sr=0 mode=0 freq=$v_cutoff_2l res=$v_res_2h
  rep 4
    svf sr=0 mode=0 freq=$v_cutoff_2l res=$v_res_2h2
  sto v_2

  fsr seed=$cdd5 cycle=0 reset=0
  rep 4
    svf sr=0 mode=1 freq=$v_cutoff_3h res=1
  rep 2
    svf sr=0 mode=0 freq=$v_cutoff_3l res=$v_res_3
  rep 1
    svf sr=0 mode=0 freq=$v_cutoff_3l res=$v_res_3h
  rep 4
    svf sr=0 mode=0 freq=$v_cutoff_3l res=$v_res_3h2
  sto v_3

  $v_1
  * $m_level_1
  + $v_2
    * $m_level_2
  + $v_3
    * $m_level_3

#  $v_3
